/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision$
// $Date$
// $URL$

// Written: Minjie Zhu

// Description: Quasi-incompressible solver, M and Mp

#include <stdlib.h>
#include <PFEMQuasiLinSOE.h>
#include <PFEMQuasiSolver.h>
#include <Matrix.h>
#include <Graph.h>
#include <Vertex.h>
#include <VertexIter.h>
#include <math.h>
#include <Element.h>
#include <ElementIter.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <iostream>
using std::nothrow;
#include <Pressure_Constraint.h>
#include <Pressure_ConstraintIter.h>
#include <Domain.h>
#include <Node.h>
#include <NodeIter.h>
#include <DOF_Group.h>
#include <AnalysisModel.h>

PFEMQuasiLinSOE::PFEMQuasiLinSOE(PFEMQuasiSolver &the_Solver)
    :PFEMLinSOE(LinSOE_TAGS_PFEMQuasiLinSOE),
     M(0), Mp(0), Gt(0), newDofID()
{
    the_Solver.setLinearSOE(*this);
    LinearSOE::setSolver(the_Solver);
}


PFEMQuasiLinSOE::PFEMQuasiLinSOE()
    :PFEMLinSOE(LinSOE_TAGS_PFEMQuasiLinSOE),
     M(0), Mp(0), Gt(0), newDofID()
{

}



    
PFEMQuasiLinSOE::~PFEMQuasiLinSOE()
{
    if(M != 0) cs_spfree(M);
    if(Mp != 0) cs_spfree(Mp);
    if(Gt != 0) cs_spfree(Gt);
}


int 
PFEMQuasiLinSOE::addA(const Matrix &m, const ID &id, double fact)
{
    // check for a quick return 
    if (fact == 0.0)  
	return 0;
    const Vector& X = this->getX();
    const ID& dofType = this->getDofType();
    const ID& dofID = this->getDofID();

    int idSize = id.Size();
    int size = X.Size();
    
    // check that m and id are of similar size
    if (idSize != m.noRows() && idSize != m.noCols()) {
	opserr << "PFEMQuasiLinSOE::addA() ";
	opserr << " - Matrix and ID not of similar sizes\n";
	return -1;
    }

    if (fact == 1.0) { // do not need to multiply 
        for (int i=0; i<idSize; i++) {
            int col = id(i);
            if(col>=size || col<0) continue;
            int coltype = dofType(col);         // column type
            int colid = dofID(col);             // column id

            if(coltype==4 || coltype<0) continue;

            for (int j=0; j<idSize; j++) {
                int row = id(j);
                if(row>=size || row<0) continue;
                cs* mat = 0;

                int rowtype = dofType(row);     // row type
                int rowid = dofID(row);         // row id

                // get right matrix
                if(rowtype<3 && coltype<3) {                 // M
                    mat = M;
                } else if(rowtype==3 && coltype==3) {         // Mp
                    mat = Mp;
		} else if(rowtype==3 && coltype<3) {         // Gt
                    mat = Gt;
                }

                if(mat == 0) continue;

                // find place in mat
                for(int k=mat->p[colid]; k<mat->p[colid+1]; k++) {
                    if(mat->i[k] == rowid) {
                        mat->x[k] += m(j,i);
                        break;
                    }
                }
            }  // for j		
        }  // for i
    } else {
        for (int i=0; i<idSize; i++) {
            int col = id(i);
            if(col>=size || col<0) continue;
            int coltype = dofType(col);         // column type
            int colid = dofID(col);             // column id

            if(coltype==4 || coltype<0) continue;

            for (int j=0; j<idSize; j++) {
                int row = id(j);
                if(row>=size || row<0) continue;
                cs* mat = 0;

                int rowtype = dofType(row);     // row type
                int rowid = dofID(row);         // row id

                // get right matrix
                if(rowtype<3 && coltype<3) {                 // M
                    mat = M;
                } else if(rowtype==3 && coltype==3) {         // Mp
                    mat = Mp;
		} else if(rowtype==3 && coltype<3) {         // Gt
                    mat = Gt;
                }

                if(mat == 0) continue;

                // find place in mat
                for(int k=mat->p[colid]; k<mat->p[colid+1]; k++) {
                    if(mat->i[k] == rowid) {
                        mat->x[k] += fact*m(j,i);
                        break;
                    }
                }
            }  // for j		
        }  // for i
    }

    return 0;
}

    
void 
PFEMQuasiLinSOE::zeroA(void)
{
    for(int i=0; i<M->nzmax; i++)
	M->x[i] = 0.0;
    for(int i=0; i<Mp->nzmax; i++)
	Mp->x[i] = 0.0;
    for(int i=0; i<Gt->nzmax; i++)
	Gt->x[i] = 0.0;
}
	
int 
PFEMQuasiLinSOE::sendSelf(int cTag, Channel &theChannel)
{
    return 0;
}

int 
PFEMQuasiLinSOE::recvSelf(int cTag, Channel &theChannel, 
                     FEM_ObjectBroker &theBroker)  
{
    return 0;
}

int
PFEMQuasiLinSOE::setMatIDs(Graph& theGraph, int Ssize, int Fsize, 
                                  int Isize, int Psize, int Pisize)
{
    int Vsize = Ssize+Fsize+Isize;
    const ID& dofType = this->getDofType();
    const ID& dofID0 = this->PFEMLinSOE::getDofID();
    newDofID = dofID0;
    int size = dofType.Size();
    for(int i=0; i<size; i++) {
        if(dofType(i) == 1) {
            newDofID(i) += Ssize;
        } else if(dofType(i) == 2) {
            newDofID(i) += Ssize+Fsize;
        }
    }

    cs* M1 = cs_spalloc(Vsize, Vsize, 1, 1, 1);
    cs* Mp1 = cs_spalloc(Psize, Psize, 1, 1, 1);
    cs* Gt1 = cs_spalloc(Psize, Vsize, 1, 1, 1);

    Vertex* theVertex = 0;
    for (int a=0; a<size; a++) {    // columns
        theVertex = theGraph.getVertexPtr(a);
        if (theVertex == 0) {
            opserr << "WARNING:PFEMQuasiLinSOE::setSize :";
            opserr << " vertex " << a << " not in graph!\n";
            break;
        }

        int col = theVertex->getTag();  // column 
        int coltype = dofType(col);     // column type
        int colid = newDofID(col);         // column id
        if(coltype==4 || coltype<0) continue;      // don't need this column

        // diagonal terms
        if(coltype < 3) {                      // momentum
            cs_entry(M1, colid, colid, 0.0);   
        } else if (coltype == 3) {
	    cs_entry(Mp1, colid, colid, 0.0);
	}

        // off diagonal terms
        const ID &theAdjacency = theVertex->getAdjacency();
        int idSize = theAdjacency.Size();
        for (int i=0; i<idSize; i++) {       // rows
            int row = theAdjacency(i);       // row 
            int rowtype = dofType(row);      // row type
            int rowid = newDofID(row);          // row id
            if(rowtype < 0) continue;
            
            if(rowtype<3 && coltype<3) {                // M
                cs_entry(M1, rowid, colid, 0.0);
            } else if(rowtype==3 && coltype==3) {        // Mp
                cs_entry(Mp1, rowid, colid, 0.0);
	    } else if(rowtype==3 && coltype<3) {        // Gt
                cs_entry(Gt1, rowid, colid, 0.0);
            }
        }
    }

    // convert to compressed format
    if(M != 0) cs_spfree(M);
    M = cs_compress(M1);
    cs_spfree(M1);

    if(Mp != 0) cs_spfree(Mp);
    Mp = cs_compress(Mp1);
    cs_spfree(Mp1);

    if(Gt != 0) cs_spfree(Gt);
    Gt = cs_compress(Gt1);
    cs_spfree(Gt1);


    return 0;
}


include ../../../Makefile.def

OBJS       = DispBeamColumn2d.o \
	DispBeamColumn3d.o \
	DispBeamColumn2dThermal.o \
	DispBeamColumn3dThermal.o \
	DispBeamColumnNL2d.o \
	DispBeamColumnNL3d.o \
	DispBeamColumn2dWithSensitivity.o \
	DispBeamColumn3dWithSensitivity.o \
	DispBeamColumnWarping3d.o \
	DispBeamColumnAsym3d.o \
	AxEqDispBeamColumn2d.o \
	TimoshenkoBeamColumn2d.o \
	TimoshenkoBeamColumn3d.o \
	DispBeamColumn3dID.o 


all:         $(OBJS)

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.

include ../../../Makefile.def

OBJS       = Response.o \
	ElementResponse.o \
	MaterialResponse.o \
	FiberResponse.o \
	CrdTransfResponse.o \
	CompositeResponse.o

# Compilation control

all:         $(OBJS)

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless


# DO NOT DELETE THIS LINE -- make depend depends on it.

#############################################
#  makefile for particle identification     #
#  Created by Weihu Ma on 4/2021            #
#############################################
CXX             = g++ 
CXXFLAGS        = -g -O -w -Wextra -fPIC -fopenmp 
LD		= g++ 
LDFLAGS		= -g -fopenmp
OutputOpt       = -o 

CONTOUR		= PIDplots
DENSPLT		= PIDplots_exe
INCLUDE		= $(ROOTSYS)/include

ROOTCFLAGS      = $(shell root-config --cflags)
ROOTLIBS        = $(shell root-config --libs)

CXXFLAGS        += $(ROOTCFLAGS)
LIBS            = $(ROOTLIBS)

 #####

all:            $(DENSPLT)

$(DENSPLT):	$(CONTOUR).o 
		$(LD) $(LDFLAGS) $(CONTOUR).o $(LIBS) $(OutputOpt) $(DENSPLT)
		@echo "$(DEMSPLT) done"

.cxx.o:
	$(CXX) $(CXXFLAGS) -c $<
clean:	 
	 @rm -f *.o *~ core
	 @rm -f ${DENSPLT}
	 @echo '*.o *~ core $(DEMSPLT) clean'
 

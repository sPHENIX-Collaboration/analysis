##
# sPHENIX EMCal LEDs
#
# @file
# @version 0.1

# root-config --cflags
# CPPFLAGS = -I./include
ROOT_CPPFLAGS = `root-config --cflags`
sPHENIX_CPPFLAGS = -I/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.1/include
# root-config --libs
# LIBFLAGS = -L$(MYINSTALL)/lib `root-config --libs`
LIBFLAGS = `root-config --libs`
sPHENIX_LIBFLAGS = -L/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.1/lib -lcalo_io

all: bin/read-LEDs

bin/read-LEDs: macro/read-LEDs.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(sPHENIX_LIBFLAGS)

clean:
	rm bin/read-LEDs

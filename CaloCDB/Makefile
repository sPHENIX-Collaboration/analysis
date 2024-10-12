##
# CaloCDB
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib

all: bin/genStatus

bin/genStatus: macros/genStatus.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lcdbobjects -lcalo_io

clean:
	rm bin/genStatus

# end

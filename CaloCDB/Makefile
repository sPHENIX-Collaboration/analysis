##
# CaloCDB
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib

all: bin/genStatus bin/genFittingStatus bin/Fun4All_CaloCDB

bin/genStatus: macros/genStatus.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lcdbobjects -lcalo_io

bin/genFittingStatus: macros/genFittingStatus.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lcdbobjects -lcalo_io


bin/Fun4All_CaloCDB: macros/Fun4All_CaloCDB.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lfun4all -lphool -lphparameter -lffarawobjects -lffamodules -lmbd -lglobalvertex -lcalo_reco

clean:
	rm bin/genStatus
	rm bin/genFittingStatus
	rm bin/Fun4All_CaloCDB

# end

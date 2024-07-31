##
# sPHENIX EMCal Tower Systematics
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros
LIBFLAGS = -L$(MYINSTALL)/lib -lcaloTreeGen
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lfun4all -lg4centrality -lphparameter -lcalotrigger -lcalo_reco -lphool

all: bin/Fun4All_CaloTreeGen

bin/Fun4All_CaloTreeGen: macros/Fun4All_CaloTreeGen.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS)

clean:
	rm bin/Fun4All_CaloTreeGen

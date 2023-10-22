##
# sPHENIX pi0-vn analysis
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include
LIBFLAGS = -L$(MYINSTALL)/lib -lcaloTreeGen
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lcalo_io -lfun4all -lfun4allraw

all: bin/Fun4All_CaloTreeGen bin/pi0Analysis

bin/Fun4All_CaloTreeGen: macro/Fun4All_CaloTreeGen.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/pi0Analysis: macro/pi0Analysis.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

clean:
	rm bin/Fun4All_CaloTreeGen
	rm bin/pi0Analysis

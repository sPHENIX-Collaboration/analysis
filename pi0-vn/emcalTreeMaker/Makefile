##
# sPHENIX CaloTreeGen
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include
LIBFLAGS = -L$(MYINSTALL)/lib `root-config --libs` -lcaloTreeGen
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lcalo_io -lfun4all -lfun4allraw

all: bin/Fun4All_CaloTreeGen

bin/Fun4All_CaloTreeGen: macro/Fun4All_CaloTreeGen.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(sPHENIX_LIBFLAGS)

clean:
	rm bin/Fun4All_CaloTreeGen

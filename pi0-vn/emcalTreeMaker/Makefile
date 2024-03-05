##
# sPHENIX pi0-vn analysis
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include
LIBFLAGS = -L$(MYINSTALL)/lib -lcaloTreeGen
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lfun4all -lg4centrality -lphparameter -lcalotrigger

all: bin/Fun4All_CaloTreeGen bin/pi0Ana bin/convert bin/Q-vec-corr bin/mixedEvent bin/vnAna

bin/Fun4All_CaloTreeGen: macro/Fun4All_CaloTreeGen.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/pi0Ana: macro/pi0Analysis.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/vnAna: macro/vnAnalysis.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/mixedEvent: macro/mixedEvent.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/Q-vec-corr: macro/Q-vector-correction.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/convert: macro/convert.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

clean:
	rm bin/Fun4All_CaloTreeGen
	rm bin/pi0Ana
	rm bin/convert
	rm bin/Q-vec-corr
	rm bin/vnAna

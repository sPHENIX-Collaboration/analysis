##
# sPHENIX pi0-vn analysis
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros
LIBFLAGS = -L$(MYINSTALL)/lib -lcaloTreeGen
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lfun4all -lg4centrality -lphparameter -lcalotrigger -lcalo_reco -lphool

all: bin/Fun4All_CaloTreeGen bin/pi0Ana bin/truthPi0Ana bin/convert bin/Q-vec-corr bin/mixedEvent bin/vnAna bin/vnAnaMC bin/runQA bin/runQA-2 bin/fits

bin/Fun4All_CaloTreeGen: macro/Fun4All/Fun4All_CaloTreeGen.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/pi0Ana: macro/pi0-Analysis/pi0Analysis.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/truthPi0Ana: macro/pi0-Analysis/truthPi0Analysis.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/vnAna: macro/vn-Analysis/vnAnalysis.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/vnAnaMC: macro/vn-Analysis/vnAnalysisMC.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/runQA: macro/QA/runQA.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/fits: macro/fit/fits.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/runQA-2: macro/QA/runQA-2.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/mixedEvent: macro/mixed-event/mixedEvent.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/Q-vec-corr: macro/vn-Analysis/Q-vector-correction.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/convert: macro/utils/convert.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

clean:
	rm bin/Fun4All_CaloTreeGen
	rm bin/pi0Ana
	rm bin/truthPi0Ana
	rm bin/convert
	rm bin/Q-vec-corr
	rm bin/vnAna
	rm bin/vnAnaMC
	rm bin/runQA
	rm bin/fits

##
# EMCal Bias Study
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lcalo_io
CPPFLAGS = -I$(MYINSTALL)/include
LIBFLAGS = -L$(MYINSTALL)/lib -lCaloTower

all: bin/read-Bias bin/update-Bias bin/update-Offset bin/write-Bias bin/display bin/displayv2 bin/Fun4All_Year2_Fitting

bin/read-Bias: macros/read-Bias.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS)

bin/write-Bias: macros/write-Bias.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS)

bin/update-Bias: macros/update-Bias.C
	mkdir -p bin
	g++ -g -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS)

bin/update-Offset: macros/update-Offset.C
	mkdir -p bin
	g++ -g -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS)

bin/display: macros/display.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS)

bin/displayv2: macros/displayv2.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS)

bin/Fun4All_Year2_Fitting: macros/Fun4All_Year2_Fitting.C
	mkdir -p bin
	g++ -g -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lfun4all -lfun4allraw -lphool -lcalo_reco -lffamodules

clean:
	rm -f bin/read-Bias
	rm -f bin/update-Bias
	rm -f bin/update-Offset
	rm -f bin/write-Bias
	rm -f bin/display
	rm -f bin/displayv2
	rm -f bin/Fun4All_Year2_Fitting

# end

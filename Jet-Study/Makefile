##
# Jets
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib
CPPFLAGS = -I$(MYINSTALL)/include
LIBFLAGS = -L$(MYINSTALL)/lib -lJetValidation

all: bin/Fun4All_JetVal bin/Fun4All_JetValv2 bin/Fun4All_EventVal bin/event-stats bin/display-events

bin/Fun4All_JetVal: macros/Fun4All_JetVal.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lfun4all -lphool -lphparameter -lffarawobjects -lffamodules -lmbd -lglobalvertex -lg4jets -ljetbase -ljetbackground -ljetbackground_io -lcalo_reco

bin/Fun4All_JetValv2: macros/Fun4All_JetValv2.C
	mkdir -p bin
	g++ -g -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lfun4all -lphool -lcalotrigger -lcalo_reco -lffamodules -ljetbackgroundcut

bin/Fun4All_EventVal: macros/Fun4All_EventVal.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lfun4all -lphool -lphparameter -lffarawobjects -lcalo_reco -ljetbackground -lffamodules

bin/event-stats: macros/event-stats.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS)

bin/display-events: macros/display-events.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(LIBFLAGS) $(CPPFLAGS)

clean:
	rm bin/Fun4All_JetVal
	rm bin/Fun4All_JetValv2
	rm bin/Fun4All_EventVal
	rm bin/event-stats
	rm bin/display-events

# end

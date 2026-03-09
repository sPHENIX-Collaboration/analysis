##
# sPHENIX EMCal LEDs
#
# @file
# @version 0.1

# root-config --cflags
# CPPFLAGS = -I./include
ROOT_CPPFLAGS = `root-config --cflags`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include
# root-config --libs
# LIBFLAGS = -L$(MYINSTALL)/lib `root-config --libs`
LIBFLAGS = -L$(MYINSTALL)/lib `root-config --libs` -lledtowerbuilder
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lcalo_io -lfun4all -lfun4allraw -lcalo_reco -lg4dst

all: bin/read-LEDs bin/Fun4All_LEDTowerBuilder bin/event-display

bin/read-LEDs: macro/read-LEDs.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/Fun4All_LEDTowerBuilder: macro/Fun4All_LEDTowerBuilder.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/event-display: macro/event-display.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(sPHENIX_LIBFLAGS)

clean:
	rm bin/read-LEDs
	rm bin/Fun4All_LEDTowerBuilder
	rm bin/event-display

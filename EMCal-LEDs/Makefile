##
# sPHENIX EMCal LEDs
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`

CPPFLAGS = -I$(MYINSTALL)/include
LIBFLAGS = -L$(MYINSTALL)/lib -lledtowerbuilder

sPHENIX_CPPFLAGS = -I$(OFFLINE_MAIN)/include
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib

all: bin/read-LEDs bin/Fun4All_LEDTowerBuilder

bin/read-LEDs: macro/read-LEDs.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS) -lcalo_io

bin/Fun4All_LEDTowerBuilder: macro/Fun4All_LEDTowerBuilder.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS) -lfun4all -lfun4allraw $(CPPFLAGS) $(LIBFLAGS)

clean:
	rm bin/read-LEDs
	rm bin/Fun4All_LEDTowerBuilder

##
# Hot Towers Analysis
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lcalo_io -lcdbobjects -lffamodules -lphool

all: bin/hotAna bin/plots

bin/hotAna: macro/HotTowerAnalysis.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/plots: macro/display.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) -I$(OFFLINE_MAIN)/rootmacros

clean:
	rm bin/hotAna
	rm bin/plots

# end

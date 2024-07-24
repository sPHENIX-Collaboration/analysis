##
# Hot Towers Analysis
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lcalo_io -lcdbobjects -lffamodules -lphool
LIBFLAGS = -L$(MYINSTALL)/lib -lCaloHotTower

all: bin/hotAna bin/plots bin/genHotTowerList bin/Fun4All_CaloHotTower bin/plots-hot

bin/hotAna: macro/HotTowerAnalysis.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/plots: macro/display.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) -I$(OFFLINE_MAIN)/rootmacros

bin/genHotTowerList: macro/genHotTowerList.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/plots-hot: macro/display-hot.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/Fun4All_CaloHotTower: macro/Fun4All_CaloHotTower.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lfun4all -lphparameter -lffarawobjects

clean:
	rm bin/hotAna
	rm bin/plots
	rm bin/genHotTowerList
	rm bin/Fun4All_CaloHotTower

# end

##
# sPHENIX pi0-vn analysis
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include
LIBFLAGS = -L$(MYINSTALL)/lib
CPPFLAGS = -I./include
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lcalo_io -lfun4all -lfun4allraw -lcalo_reco -lphool
G4_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -L$(OFFLINE_MAIN)/lib64 -lfun4all -lphool -lg4testbench -lffamodules -lphhepmc -lphparameter -lg4mbd -lmbd_io -lmbd -lg4detectors -lqa_modules -ltpc_io -lcalo_reco -lg4calo -lg4epd -lg4ihcal -lg4ohcal -ltrack_reco -lg4tpc -lg4intt -lg4mvtx -ltrackeralign -ltpccalib -lg4micromegas -lcalotrigger -lg4centrality -ljetbase -lg4jets -ljetbackground -lPHPythia6 -lPHPythia8 -lFermimotionAfterburner -lKFParticle -lkfparticle_sphenix -lqa_kfparticle -ldecayfinder -lparticleflow -lmvtx -lintt -ltpc -lmicromegas -lTrackingDiagnostics -lActsCore -lstdc++ -lgcc_s -lc -lglobalvertex -lg4vertex -lqautils -lHIJINGFlipAfterburner -lReactionPlaneAfterburner -lg4eval
G4_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros -I$(OFFLINE_MAIN)/include/trackreco

all: bin/Fun4All_CaloTreeGen bin/Fun4All_G4_sPHENIX bin/PDC-train bin/PDC-test bin/PDC-fits

bin/Fun4All_CaloTreeGen: macro/Fun4All_CaloTreeGen.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS) -lg4eval

bin/Fun4All_G4_sPHENIX: macro/new/Fun4All_G4_sPHENIX.C
	g++ -o $@ $< $(LIBFLAGS) $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(G4_CPPFLAGS) $(G4_LIBFLAGS)

bin/PDC-train: macro/PDC-train.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(CPPFLAGS) $(ROOT_LIBFLAGS) $(LIBFLAGS)

bin/PDC-test: macro/PDC-test.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(CPPFLAGS) $(ROOT_LIBFLAGS) $(LIBFLAGS)

bin/PDC-fits: macro/PDC-fits.C libFitUtils.so
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(CPPFLAGS) $(ROOT_LIBFLAGS) $(LIBFLAGS) -lFitUtils

libFitUtils.so: src/FitUtils.cc
	mkdir -p lib
	g++ -shared -fPIC -o $(MYINSTALL)/lib/$@ $^ $(CPPFLAGS) $(ROOT_CPPFLAGS)

clean:
	rm bin/Fun4All_CaloTreeGen
	rm bin/Fun4All_G4_sPHENIX
	rm bin/PDC-train
	rm bin/PDC-test
	rm bin/PDC-fits
	rm $(MYINSTALL)/lib/libFitUtils.so

##
# sPHENIX pi0-vn analysis
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include
LIBFLAGS = -L$(MYINSTALL)/lib -lg4eval
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lcalo_io -lfun4all -lfun4allraw -lcalo_reco -lphool
G4_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -L$(OFFLINE_MAIN)/lib64 -lfun4all -lphool -lg4testbench -lffamodules -lphhepmc -lphparameter -lg4mbd -lmbd_io -lmbd -lg4detectors -lqa_modules -ltpc_io -lcalo_reco -lg4calo -lg4epd -lg4ihcal -lg4ohcal -ltrack_reco -lg4tpc -lg4intt -lg4mvtx -ltrackeralign -ltpccalib -lg4micromegas -lcalotrigger -lg4centrality -ljetbase -lg4jets -ljetbackground -lPHPythia6 -lPHPythia8 -lFermimotionAfterburner -lKFParticle -lkfparticle_sphenix -lqa_kfparticle -ldecayfinder -lparticleflow -lmvtx -lintt -ltpc -lmicromegas -lTrackingDiagnostics -lActsCore -lstdc++ -lgcc_s -lc -lglobalvertex -lg4vertex -lqautils -lHIJINGFlipAfterburner -lReactionPlaneAfterburner
G4_CPPFLAGS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros -I$(OFFLINE_MAIN)/include/trackreco

all: bin/Fun4All_CaloTreeGen bin/Fun4All_G4_sPHENIX

bin/Fun4All_CaloTreeGen: macro/Fun4All_CaloTreeGen.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(sPHENIX_CPPFLAGS) $(LIBFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_LIBFLAGS)

bin/Fun4All_G4_sPHENIX: macro/new/Fun4All_G4_sPHENIX.C
	g++ -o $@ $< $(LIBFLAGS) $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(G4_CPPFLAGS) $(G4_LIBFLAGS)

clean:
	rm bin/Fun4All_CaloTreeGen
	rm bin/Fun4All_G4_sPHENIX

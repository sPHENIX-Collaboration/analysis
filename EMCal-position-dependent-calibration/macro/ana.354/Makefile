##
# sPHENIX EMCal Position Dependent Correction
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
sPHENIX_CPPFLAGS = -I./macro -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros

LIBFLAGS = -L$(MYINSTALL)/lib -lg4eval
ROOT_LIBFLAGS = -L`root-config --libdir` -lCore -lm -rdynamic
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -L$(OFFLINE_MAIN)/lib64 -lfun4all -lphool -lg4testbench -lffamodules -lphhepmc -lphparameter -lg4bbc -lbbc_io -lg4detectors -lqa_modules -ltpc_io -lcalo_reco -lg4calo -lg4epd -lg4ihcal -lg4ohcal -ltrack_reco -lg4tpc -lg4intt -lg4mvtx -ltrackeralign -ltpccalib -lg4micromegas -lcalotrigger -lg4centrality -lg4vertex -lg4jets -ljetbackground -lPHPythia6 -lPHPythia8 -lFermimotionAfterburner -lKFParticle -lkfparticle_sphenix -lqa_kfparticle -ldecayfinder -lparticleflow -lmvtx -lintt -ltpc -lmicromegas -lTrackingDiagnostics -lActsCore -lstdc++ -lgcc_s -lc

all: bin/Fun4All_G4_sPHENIX

bin/Fun4All_G4_sPHENIX: macro/Fun4All_G4_sPHENIX.C
	mkdir -p bin
	g++ -o $@ $< $(LIBFLAGS) $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS)

clean:
	rm bin/Fun4All_G4_sPHENIX

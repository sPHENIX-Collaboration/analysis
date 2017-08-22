#include "macro/globals.h"

int g42rc(
	const char *filename_g4,
	const char *filename_rc,
	const char *flavor,
	Float_t pseudorapidity,
	Float_t momentum)
{
	///--------------
	/// INTRODUCTION
	///--------------
	
	/// this macro is designed to...
	/// 	
	/// 	reconstruct calorimeter clusters
	/// 
	/// this job is distinct from reconstructing electron energies.
	/// it does not even appear in the README! top secret :)
	
	/// it uses only one specified flavor/energy/pseudorapidity tuple.
	
	/// it produces a ROOT TTree output file.
	/// it uses the in-house ClusterPeeler task.
	
	///-----------------------
	/// LOAD LIBRARIES/MACROS
	///-----------------------
	
	gSystem->Load("libfun4all.so");
	gSystem->Load("libClusterPeeler.so");
	
	gROOT->SetMacroPath("/sphenix/user/jordan/sphenix-git/macros/macros/g4simulations");
	gROOT->LoadMacro("G4Setup_EICDetector.C");
	
	///---------------------
	/// SET UP INPUT/OUTPUT
	///---------------------
	
	/// ClusterPeeler (later) is basically the output manager.
	/// if i put it here, however, the output TTree's do not write.
	/// dunno why, but.. it needs to go later.
	
	Fun4AllServer *se = Fun4AllServer::instance();
	
	Fun4AllDstInputManager *in = new Fun4AllDstInputManager("DSTIN");
	in->fileopen(filename_g4);
	se->registerInputManager(in);
	
	///-----------------------------
	/// SET UP RECONSTRUCTION TASKS
	///-----------------------------
	
	/// call G4Init() to ensure same macros as detector simulation.
	
	/// detector segmentation and towering is needed for clusterizing.
	
	G4Init();
	
	EEMC_Cells();
	EEMC_Towers();
	EEMC_Clusters();
	
	CEMC_Cells();
	CEMC_Towers();
	CEMC_Clusters();
	
	FEMC_Cells();
	FEMC_Towers();
	FEMC_Clusters();
	
	///-------------------------------
	/// SET UP IN-HOUSE ANALYSIS TASK
	///-------------------------------
	
	/// it makes user do TTree and TFile management.
	
	TFile *file_rc = new TFile(filename_rc, "RECREATE");
	ClusterPeeler *peelers[NCALOS];
	for (int i_calo = 0; i_calo < NCALOS; ++i_calo) {
		string task_name = "CLUSTER_PEELER_" + string(calo_names[i_calo]);
		peelers[i_calo] = new ClusterPeeler(task_name, calo_names[i_calo], file_rc);
		se->registerSubsystem(peelers[i_calo]);
	}
	
	///---------------------------
	/// RUN EVENT LOOP / CLEAN UP
	///---------------------------
	
	//se->run(1000);
	se->run(200);
	//se->run(10);
	se->End();
	delete se;
	file_rc->Write("", TObject::kOverwrite);
	file_rc->Close();
	return 0;
}

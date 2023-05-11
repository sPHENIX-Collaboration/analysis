/*!
 *  \file	    RunGenFitTrackProp.C	
 *  \brief		Example macro to run PHG4KalmanPatRec
 *  \author		Haiwang Yu <yuhw@nmsu.edu>
 */

void RunGenFitTrackProp (
		const char* input = "SvtxClusters.root",
		const int pid_guess = 321,//pi: 211, mu: 13, e: 11, Kaon: 321, proton: 2212
		const int nevents = 0,
		const bool use_kalman_pat_rec = true,
		const char* output = "GenFitTrackProp.root") {

	gSystem->Load("libphool.so");
	gSystem->Load("libfun4all.so");
	gSystem->Load("libg4dst.so");
	gSystem->Load("libg4hough.so");

	Fun4AllServer *se = Fun4AllServer::instance();

	if (use_kalman_pat_rec) {
		//---------------------
		// PHG4KalmanPatRec
		//---------------------

		PHG4KalmanPatRec* kalman_pat_rec = new PHG4KalmanPatRec("PHG4KalmanPatRec", 3, 4, 40);
		//kalman_pat_rec->set_primary_pid_guess(pid_guess);
		//kalman_pat_rec->Verbosity(100);

		se->registerSubsystem(kalman_pat_rec);

	} else {
		//---------------------
		// Truth Pattern Recognition
		//---------------------
		PHG4TruthPatRec* pat_rec = new PHG4TruthPatRec();
		se->registerSubsystem(pat_rec);

	}

	//---------------------
	// Kalman Filter
	//---------------------
	PHG4TrackKalmanFitter* kalman = new PHG4TrackKalmanFitter();
	//kalman->set_primary_pid_guess(pid_guess);
	//kalman->set_fit_primary_tracks(true);
	//kalman->set_use_truth_vertex(true);
	//kalman->Verbosity(100);
	//kalman->set_do_evt_display(true);
	//kalman->set_track_fitting_alg_name("DafRef");
	//kalman->set_over_write_svtxtrackmap(true);
	//kalman->set_over_write_svtxvertexmap(true);
	//kalman->set_do_eval(true);
	//kalman->set_eval_filename("PHG4TrackKalmanFitter_eval.root");

	se->registerSubsystem(kalman);

	//---------------
	// Load libraries
	//---------------

	gSystem->Load("libfun4all.so");
	gSystem->Load("libGenFitTrackProp.so");

	Fun4AllServer *se = Fun4AllServer::instance();
	//se->Verbosity(10);

	//----------------
	// GenFitTrackProp
	//----------------

	GenFitTrackProp *genfit_prop = new GenFitTrackProp("GenFitTrackProp",pid_guess);
	genfit_prop->Verbosity(100);
	se->registerSubsystem(genfit_prop);

	Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");

	TString tstr_input(input);
	if (tstr_input.EndsWith(".root"))
		in->AddFile(input);
	else
		in->AddListFile(input);
	se->registerInputManager(in);

	se->run(nevents);

	se->End();

	std::cout << " Success!! " << std::endl;
}

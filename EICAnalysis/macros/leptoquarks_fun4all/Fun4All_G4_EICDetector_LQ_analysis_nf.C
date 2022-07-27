int Fun4All_G4_EICDetector_LQ_analysis_nf(
					  string fileindex="1",
                                          string n="10",
                                          string ebeam="20",
                                          string pbeam="250",
                                          //string inputFile,
                                          //string output="tau"
                                          )
{
  // string inputFile="/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/G4_Leptoquark_DST_p250_e20_"+fileindex+"events.root";
  //string inputFile="/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/G4_Leptoquark_DST_p250_e20_1000events_"+fileindex+".root";
  //    string inputFile="/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/G4_Leptoquark_DST_p250_e20_1093events.root";

  //  string inputFile = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/g4sim/G4_Leptoquark_DST_p250_e20_1000events_"+fileindex+"seed_3pion.root";
  string inputFile = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/g4sim/G4_Leptoquark_DST_p250_e20_1000events_"+fileindex+"seed_3pion.root";

  cout << "Using input file: " << inputFile << endl;

  int nEvents;
  stringstream geek(n);
  geek>>nEvents;

  string directory = "./";//"/direct/phenix+u/spjeffas/leptoquark/output/"+output+"/";

  //===============
  // Input options
  //===============

  // Either:
  // read previously generated g4-hits files, in this case it opens a DST and skips
  // the simulations step completely. The G4Setup macro is only loaded to get information
  // about the number of layers used for the cell reco code
  //
  // In case reading production output, please double check your G4Setup_sPHENIX.C and G4_*.C consistent with those in the production macro folder
  // E.g. /sphenix/sim//sim01/production/2016-07-21/single_particle/spacal2d/
  const bool readhits = true;

  //do leptoquark analysis modules
  bool do_lepto_analysis = true;

  // Other options
  bool do_cemc_eval = false;

  bool do_jet_reco = true;
  bool do_jet_eval = do_jet_reco && true;

  bool do_fwd_jet_reco = false;
  bool do_fwd_jet_eval = do_fwd_jet_reco && false;

  // HI Jet Reco for jet simulations in Au+Au (default is false for
  // single particle / p+p simulations, or for Au+Au simulations which
  // don't care about jets)
  bool do_HIjetreco = false && do_jet_reco && do_cemc_twr && do_hcalin_twr && do_hcalout_twr;

  // Compress DST files
  bool do_dst_compress = false;

  //Option to convert DST to human command readable TTree for quick poke around the outputs
  bool do_DSTReader = false;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4eval.so");
  gSystem->Load("libeicana.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0); // uncomment for batch production running with minimal output messages
  // se->Verbosity(Fun4AllServer::VERBOSITY_SOME); // uncomment for some info for interactive running

  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();
  // By default every random number generator uses
  // PHRandomSeed() which reads /dev/urandom to get its seed
  // if the RANDOMSEED flag is set its value is taken as seed
  // You can either set this to a random value using PHRandomSeed()
  // which will make all seeds identical (not sure what the point of
  // this would be:
  //  rc->set_IntFlag("RANDOMSEED",PHRandomSeed());
  // or set it to a fixed value so you can debug your code
  // rc->set_IntFlag("RANDOMSEED", 12345);

  //-----------------
  // Event generation
  //-----------------

  //---------
  // Jet reco
  //---------

  if (do_jet_reco)
    {
      gROOT->LoadMacro("G4_Jets.C");
      Jet_Reco();
    }

  if (do_HIjetreco) {
    gROOT->LoadMacro("G4_HIJetReco.C");
    HIJetReco();
  }

  if (do_fwd_jet_reco)
    {
      gROOT->LoadMacro("G4_FwdJets.C");
      Jet_FwdReco();
    }


  if (do_cemc_eval)
    {
      //gROOT->LoadMacro("G4_CEmc_Spacal.C");
      //CEMC_Eval("cemc_eval_lq.root");

      CaloEvaluator *eval = new CaloEvaluator("CEMCEVALUATOR", "CEMC",
					      directory+"p"+pbeam+"_e"+ebeam+"_"+n+"events_file"+fileindex+"_cemceval.root");
      se->registerSubsystem(eval);

    }

  //----------------------
  // Simulation evaluation
  //----------------------

  if (do_jet_eval)
    {
      Jet_Eval(directory+"p"+pbeam+"_e"+ebeam+"_"+n+"events_file"+fileindex+"_jeteval_r05.root");
    }

  if (do_fwd_jet_eval) Jet_FwdEval(directory+"p"+pbeam+"_e"+ebeam+"_"+n+"events_file"+fileindex+"_fwdjeteval.root");

  if(do_lepto_analysis){
    gROOT->LoadMacro("G4_Lepto.C");
    G4_Lepto(directory+"p"+pbeam+"_e"+ebeam+"_"+n+"events_file"+fileindex+"_LeptoAna");
  }


  //--------------
  // IO management
  //--------------

  if (readhits)
    {
      // Hits file
      Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
      hitsin->fileopen(inputFile);
      se->registerInputManager(hitsin);
    }

  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    {
      return;
    }


  se->run(nEvents);

  //-----
  // Exit
  //-----

  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}

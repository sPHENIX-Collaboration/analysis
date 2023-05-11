
int Fun4All_G4_sPHENIX(
		       const int nEvents = 10000,
		       const char * inputFile = "/sphenix/sim//sim01/production/2016-07-21/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-0002.root",
		       const char * outputFile = "G4sPHENIXCells.root",
           const char * embed_input_file = "/sphenix/sim/sim01/production/2016-07-12/sHijing/spacal2d/G4Hits_sPHENIX_sHijing-0-4.4fm.list"
		       )
{

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
  const bool readhits = false;
  // Or:
  // read files in HepMC format (typically output from event generators like hijing or pythia)
  const bool readhepmc = false; // read HepMC files
  // Or:
  // Use particle generator
  const bool runpythia8 = true;
  const bool runpythia6 = false;
  // And
  // Further choose to embed newly simulated events to a previous simulation. Not compatible with `readhits = true`
  // In case embedding into a production output, please double check your G4Setup_sPHENIX.C and G4_*.C consistent with those in the production macro folder
  // E.g. /sphenix/sim//sim01/production/2016-07-21/single_particle/spacal2d/
  const bool do_embedding = false;

  //======================
  // What to run
  //======================

  bool do_bbc = false;

  bool do_pipe = false;

  bool do_svtx = false;
  bool do_svtx_cell = false;
  bool do_svtx_track = false;
  bool do_svtx_eval = false;

  bool do_preshower = false;

  bool do_cemc = false;
  bool do_cemc_cell = false;
  bool do_cemc_twr = false;
  bool do_cemc_cluster = false;
  bool do_cemc_eval = false;

  bool do_hcalin = false;
  bool do_hcalin_cell = false;
  bool do_hcalin_twr = false;
  bool do_hcalin_cluster = false;
  bool do_hcalin_eval = false;

  bool do_magnet = false;

  bool do_hcalout = false;
  bool do_hcalout_cell = false;
  bool do_hcalout_twr = false;
  bool do_hcalout_cluster = false;
  bool do_hcalout_eval = false;

  bool do_global = false;
  bool do_global_fastsim = false;

  bool do_jet_reco = true;
  bool do_jet_eval = false;

  bool do_dst_compress = false;

  //Option to convert DST to human command readable TTree for quick poke around the outputs
  bool do_DSTReader = true;
  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libg4calo.so");
  gSystem->Load("libg4eval.so");

  // establish the geometry and reconstruction setup
  gROOT->LoadMacro("G4Setup_sPHENIX.C");
  G4Init(do_svtx,do_preshower,do_cemc,do_hcalin,do_magnet,do_hcalout,do_pipe);

  int absorberactive = 1; // set to 1 to make all absorbers active volumes
    const string magfield = "0"; // if like float -> solenoidal field in T, if string use as fieldmap name (including path)
//  const string magfield = "/phenix/upgrades/decadal/fieldmaps/sPHENIX.2d.root"; // if like float -> solenoidal field in T, if string use as fieldmap name (including path)
  const float magfield_rescale = 1.4/1.5; // scale the map to a 1.4 T field

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();
  // By default every random number generator uses
  // PHRandomSeed() which reads /dev/urandom to get its seed
  // if the RANDOMSEED flag is set its value is taken as seed
  // You ca neither set this to a random value using PHRandomSeed()
  // which will make all seeds identical (not sure what the point of
  // this would be:
  //  rc->set_IntFlag("RANDOMSEED",PHRandomSeed());
  // or set it to a fixed value so you can debug your code
  // rc->set_IntFlag("RANDOMSEED", 12345);

  //-----------------
  // Event generation
  //-----------------

  if (readhits)
    {
      // Get the hits from a file
      // The input manager is declared later

      if (do_embedding)
       {
         cout <<"Do not support read hits and embed background at the same time."<<endl;
         exit(1);
       }

    }
  else if (readhepmc)
    {
      // this module is needed to read the HepMC records into our G4 sims
      // but only if you read HepMC input files
      HepMCNodeReader *hr = new HepMCNodeReader();
      se->registerSubsystem(hr);
    }
  else if (runpythia8)
    {
      gSystem->Load("libPHPythia8.so");


      PHPy8JetTrigger *theTrigger = new PHPy8JetTrigger();
//      theTrigger->Verbosity(10);
      theTrigger->SetEtaHighLow(-1, 1);
      theTrigger->SetJetR(.4);
      theTrigger->SetMinJetPt(25);

      PHPythia8* pythia8 = new PHPythia8();
      // see coresoftware/generators/PHPythia8 for example config
      pythia8->set_config_file("phpythia8.cfg");

      pythia8->beam_vertex_parameters(0,0,0,0,0,5);
      pythia8->register_trigger(theTrigger);
//      pythia8->set_trigger_AND();

      se->registerSubsystem(pythia8);
      pythia8->print_config();
//      pythia8->Verbosity(10);

      HepMCNodeReader *hr = new HepMCNodeReader();
      se->registerSubsystem(hr);
    }
  else if (runpythia6)
    {
      gSystem->Load("libPHPythia6.so");

      PHPythia6 *pythia6 = new PHPythia6();
      pythia6->set_config_file("phpythia6.cfg");
      se->registerSubsystem(pythia6);

      HepMCNodeReader *hr = new HepMCNodeReader();
      se->registerSubsystem(hr);
    }
  else
    {
      // toss low multiplicity dummy events
      PHG4SimpleEventGenerator *gen = new PHG4SimpleEventGenerator();
      gen->add_particles("e-",1); // mu+,e+,proton,pi+,Upsilon
      // gen->add_particles("e+",5); // mu-,e-,anti_proton,pi-
      if (readhepmc || do_embedding) {
	gen->set_reuse_existing_vertex(true);
	gen->set_existing_vertex_offset_vector(0.0,0.0,0.0);
      } else {
	gen->set_vertex_distribution_function(PHG4SimpleEventGenerator::Uniform,
					       PHG4SimpleEventGenerator::Uniform,
					       PHG4SimpleEventGenerator::Uniform);
	gen->set_vertex_distribution_mean(0.0,0.0,0.0);
	gen->set_vertex_distribution_width(0.0,0.0,5.0);
      }
      gen->set_vertex_size_function(PHG4SimpleEventGenerator::Uniform);
      gen->set_vertex_size_parameters(10.0,0.0);
      gen->set_eta_range(-0.1, 0.1);
      gen->set_phi_range(-1.0*TMath::Pi(), 1.0*TMath::Pi());
      gen->set_pt_range(24, 24);
      gen->Embed(1);
      gen->Verbosity(0);
      se->registerSubsystem(gen);
    }

  if (!readhits)
    {
      //---------------------
      // Detector description
      //---------------------

      G4Setup(absorberactive, magfield, TPythia6Decayer::kAll,
	      do_svtx, do_preshower, do_cemc, do_hcalin, do_magnet, do_hcalout, do_pipe, magfield_rescale);
    }

  //---------
  // BBC Reco
  //---------

  if (do_bbc)
    {
      gROOT->LoadMacro("G4_Bbc.C");
      BbcInit();
      Bbc_Reco();
    }
  //------------------
  // Detector Division
  //------------------

  if (do_svtx_cell) Svtx_Cells();

  if (do_cemc_cell) CEMC_Cells();

  if (do_hcalin_cell) HCALInner_Cells();

  if (do_hcalout_cell) HCALOuter_Cells();

  //-----------------------------
  // CEMC towering and clustering
  //-----------------------------

  if (do_cemc_twr) CEMC_Towers();
  if (do_cemc_cluster) CEMC_Clusters();

  //-----------------------------
  // HCAL towering and clustering
  //-----------------------------

  if (do_hcalin_twr) HCALInner_Towers();
  if (do_hcalin_cluster) HCALInner_Clusters();

  if (do_hcalout_twr) HCALOuter_Towers();
  if (do_hcalout_cluster) HCALOuter_Clusters();

  if (do_dst_compress) ShowerCompress();

  //--------------
  // SVTX tracking
  //--------------

  if (do_svtx_track) Svtx_Reco();

  //-----------------
  // Global Vertexing
  //-----------------

  if (do_global)
    {
      gROOT->LoadMacro("G4_Global.C");
      Global_Reco();
    }

  else if (do_global_fastsim)
    {
      gROOT->LoadMacro("G4_Global.C");
      Global_FastSim();
    }

  //---------
  // Jet reco
  //---------

  if (do_jet_reco)
    {
      gROOT->LoadMacro("G4_Jets.C");
      Jet_Reco();
    }
  //----------------------
  // Simulation evaluation
  //----------------------

  if (do_svtx_eval) Svtx_Eval("g4svtx_eval.root");

  if (do_cemc_eval) CEMC_Eval("g4cemc_eval.root");

  if (do_hcalin_eval) HCALInner_Eval("g4hcalin_eval.root");

  if (do_hcalout_eval) HCALOuter_Eval("g4hcalout_eval.root");

  if (do_jet_eval) Jet_Eval("g4jet_eval.root");



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
  if (do_embedding)
    {
      if (embed_input_file == NULL)
        {
          cout << "Missing embed_input_file! Exit";
          exit(3);
        }

      Fun4AllDstInputManager *in1 = new Fun4AllNoSyncDstInputManager("DSTinEmbed");
      //      in1->AddFile(embed_input_file); // if one use a single input file
      in1->AddListFile(embed_input_file); // RecommendedL: if one use a text list of many input files
      se->registerInputManager(in1);
    }
  if (readhepmc)
    {
      Fun4AllInputManager *in = new Fun4AllHepMCInputManager( "DSTIN");
      se->registerInputManager( in );
      se->fileopen( in->Name().c_str(), inputFile );
    }
  else
    {
      // for single particle generators we just need something which drives
      // the event loop, the Dummy Input Mgr does just that
      Fun4AllInputManager *in = new Fun4AllDummyInputManager( "JADE");
      se->registerInputManager( in );
    }

  //temp lines for QA modules
    {

      gSystem->Load("libqa_modules");

        if (do_jet_reco)
          {
            QAG4SimulationJet * calo_jet7 = new QAG4SimulationJet(
                "AntiKt_Truth_r07",QAG4SimulationJet:: kProcessTruthSpectrum);
            se->registerSubsystem(calo_jet7);

            QAG4SimulationJet * calo_jet7 = new QAG4SimulationJet(
                "AntiKt_Truth_r04", QAG4SimulationJet::kProcessTruthSpectrum);
            se->registerSubsystem(calo_jet7);

            QAG4SimulationJet * calo_jet7 = new QAG4SimulationJet(
                "AntiKt_Truth_r02",QAG4SimulationJet:: kProcessTruthSpectrum);
            se->registerSubsystem(calo_jet7);
          }
      }

    // HF jet trigger moudle
      assert (gSystem->Load("libHFJetTruthGeneration") == 0);
      {
        if (do_jet_reco)
          {
            HFJetTruthTrigger * jt = new HFJetTruthTrigger(
                "HFJetTruthTrigger.root",5 , "AntiKt_Truth_r07");
//            jt->Verbosity(HFJetTruthTrigger::VERBOSITY_MORE);
            se->registerSubsystem(jt);

            HFJetTruthTrigger * jt = new HFJetTruthTrigger(
                "HFJetTruthTrigger.root",5 , "AntiKt_Truth_r04");
//            jt->Verbosity(HFJetTruthTrigger::VERBOSITY_MORE);
            se->registerSubsystem(jt);

            HFJetTruthTrigger * jt = new HFJetTruthTrigger(
                "HFJetTruthTrigger.root",5 , "AntiKt_Truth_r02");
//            jt->Verbosity(HFJetTruthTrigger::VERBOSITY_MORE);
            se->registerSubsystem(jt);
          }
      }

  // HF jet analysis moudle
    if (gSystem->Load("libslt") == 0)
    {


      if (do_jet_reco)
        {
          SoftLeptonTaggingTruth * slt = new SoftLeptonTaggingTruth(
              "AntiKt_Truth_r07");
          se->registerSubsystem(slt);

          SoftLeptonTaggingTruth * slt = new SoftLeptonTaggingTruth(
              "AntiKt_Truth_r04");
//          slt->Verbosity(1);
          se->registerSubsystem(slt);

          SoftLeptonTaggingTruth * slt = new SoftLeptonTaggingTruth(
              "AntiKt_Truth_r02");
          se->registerSubsystem(slt);
        }
    }


  if (do_DSTReader)
    {
      //Convert DST to human command readable TTree for quick poke around the outputs
      gROOT->LoadMacro("G4_DSTReader.C");

      G4DSTreader( outputFile, //
          /*int*/ absorberactive ,
          /*bool*/ do_svtx ,
          /*bool*/ do_preshower ,
          /*bool*/ do_cemc ,
          /*bool*/ do_hcalin ,
          /*bool*/ do_magnet ,
          /*bool*/ do_hcalout ,
          /*bool*/ do_cemc_twr ,
          /*bool*/ do_hcalin_twr ,
          /*bool*/ do_magnet  ,
          /*bool*/ do_hcalout_twr
          );
    }


   Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", outputFile);
   if (do_dst_compress) DstCompress(out);
   se->registerOutputManager(out);

  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    {
      return;
    }
  // if we run the particle generator and use 0 it'll run forever
  if (nEvents == 0 && !readhits && !readhepmc)
    {
      cout << "using 0 for number of events is a bad idea when using particle generators" << endl;
      cout << "it will run forever, so I just return without running anything" << endl;
      return;
    }

  se->run(nEvents);

  {

    gSystem->Load("libqa_modules");
    QAHistManagerDef::saveQARootFile(string(outputFile) + "_qa.root");

      if (gSystem->Load("libslt") == 0)
        {
          SoftLeptonTaggingTruth::getHistoManager()->dumpHistos(
              string(outputFile) + "_slt.root");
        }
  }

  //-----
  // Exit
  //-----
  gSystem->Exec("ps -o sid,ppid,pid,user,comm,vsize,rssize,time");
  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}

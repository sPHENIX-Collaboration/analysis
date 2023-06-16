#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <GlobalVariables.C>

#include <DisplayOn.C>
#include <G4Setup_sPHENIX.C>
#include <G4_Bbc.C>
#include <G4_CaloTrigger.C>
#include <G4_Centrality.C>
#include <G4_DSTReader.C>
#include <G4_Global.C>
#include <G4_HIJetReco.C>
#include <G4_Input.C>
#include <G4_Jets.C>
#include <G4_KFParticle.C>
#include <G4_ParticleFlow.C>
#include <G4_Production.C>
#include <G4_TopoClusterReco.C>

#include <G4_User.C>
#include <QA.C>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <hcaljetphishift/HCalJetPhiShift.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libHCalJetPhiShift.so)

// For HepMC Hijing
// try inputFile = /sphenix/sim/sim01/sphnxpro/sHijing_HepMC/sHijing_0-12fm.dat

int Fun4All_HCalJetPhiShift(
    const int nEvents = 1,
    const int index = 0,
    const string &outdir = ".")
{

  int event_number = (int)index*nEvents;

  //  string outputFile = "HCalJetPhiShift"
  string outputroot = outdir + "/HCalJetPhiShift";
  if (index>-1) {
    outputroot += "_" + to_string(index);
  }
  
  string outputFile = outputroot + ".root";
  
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  //Opt to print all random seed used for debugging reproducibility. Comment out to reduce stdout prints.
//  PHRandomSeed::Verbosity(1);

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
  //  rc->set_IntFlag("RANDOMSEED", 12345);


  //===============
  // Input options
  //===============
  // verbosity setting (applies to all input managers)
//  Input::VERBOSITY = 0;

  Input::SIMPLE = true;
  // Input::SIMPLE_NUMBER = 2; // if you need 2 of them
//  Input::SIMPLE_VERBOSITY = 0;

  //-----------------
  // Initialize the selected Input/Event generation
  //-----------------
  // This creates the input generator(s)
  InputInit();

  //--------------
  // Set generator specific options
  //--------------
  // can only be set after InputInit() is called

  // Simple Input generator:
  // if you run more than one of these Input::SIMPLE_NUMBER > 1
  // add the settings for other with [1], next with [2]...
  if (Input::SIMPLE)
  {
    INPUTGENERATOR::SimpleEventGenerator[0]->add_particles("pi-", 1);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_vertex_distribution_function(PHG4SimpleEventGenerator::Gaus,
                                                                            PHG4SimpleEventGenerator::Gaus,
                                                                            PHG4SimpleEventGenerator::Gaus);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_vertex_distribution_mean(0., 0., 0.);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_vertex_distribution_width(0.0, 0.0, 0.0);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_eta_range(-1, 1);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_phi_range(-M_PI, M_PI);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_pt_range(1., 30.);
  }

  //--------------
  // Set Input Manager specific options
  //--------------

  // register all input generators with Fun4All
  InputRegister();

  rc->set_IntFlag("RUNNUMBER",1);

  SyncReco *sync = new SyncReco();
  se->registerSubsystem(sync);

  HeadReco *head = new HeadReco();
  se->registerSubsystem(head);

// Flag Handler is always needed to read flags from input (if used)
// and update our rc flags with them. At the end it saves all flags
// again on the DST in the Flags node under the RUN node
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  //======================
  // Write the DST
  //======================

  //Enable::DSTOUT = true;
  Enable::DSTOUT_COMPRESS = false;
  DstOut::OutputDir = outdir;
  DstOut::OutputFile = outputFile;

  //Option to convert DST to human command readable TTree for quick poke around the outputs
  //  Enable::DSTREADER = true;

  // turn the display on (default off)
   //Enable::DISPLAY = true;

  //======================
  // What to run
  //======================

  // QA, main switch
  Enable::QA = false;

  // Global options (enabled for all enables subsystems - if implemented)
  //  Enable::ABSORBER = true;
  //  Enable::OVERLAPCHECK = true;
  //  Enable::VERBOSITY = 1;

  // Enable::BBC = true;
  // Enable::BBC_SUPPORT = true; // save hist in bbc support structure
  // Enable::BBCRECO = Enable::BBC && true
  Enable::BBCFAKE = false;  // Smeared vtx and t0, use if you don't want real BBC in simulation

  Enable::PIPE = false;
  Enable::PIPE_ABSORBER = false;

  // central tracking
  Enable::MVTX = false;
  Enable::MVTX_CELL = Enable::MVTX && true;
  Enable::MVTX_CLUSTER = Enable::MVTX_CELL && true;
  Enable::MVTX_QA = Enable::MVTX_CLUSTER && Enable::QA && true;

  Enable::INTT = false;
//  Enable::INTT_ABSORBER = true; // enables layerwise support structure readout
//  Enable::INTT_SUPPORT = true; // enable global support structure readout
  Enable::INTT_CELL = Enable::INTT && true;
  Enable::INTT_CLUSTER = Enable::INTT_CELL && true;
  Enable::INTT_QA = Enable::INTT_CLUSTER && Enable::QA && true;

  Enable::TPC = false;
  Enable::TPC_ABSORBER = false;
  Enable::TPC_CELL = Enable::TPC && true;
  Enable::TPC_CLUSTER = Enable::TPC_CELL && true;
  Enable::TPC_QA = Enable::TPC_CLUSTER && Enable::QA && true;

  Enable::MICROMEGAS = false;
  Enable::MICROMEGAS_CELL = Enable::MICROMEGAS && true;
  Enable::MICROMEGAS_CLUSTER = Enable::MICROMEGAS_CELL && true;
  Enable::MICROMEGAS_QA = Enable::MICROMEGAS_CLUSTER && Enable::QA && true;

  Enable::TRACKING_TRACK = (Enable::MICROMEGAS_CLUSTER && Enable::TPC_CLUSTER && Enable::INTT_CLUSTER && Enable::MVTX_CLUSTER) && true;
  Enable::TRACKING_EVAL = Enable::TRACKING_TRACK && true;
  Enable::TRACKING_QA = Enable::TRACKING_TRACK && Enable::QA && true;

  Enable::CEMC = false;
  Enable::CEMC_ABSORBER = false;
  Enable::CEMC_CELL = Enable::CEMC && true;
  Enable::CEMC_TOWER = Enable::CEMC_CELL && true;
  Enable::CEMC_CLUSTER = Enable::CEMC_TOWER && true;
  Enable::CEMC_EVAL = Enable::CEMC_CLUSTER && true;
  Enable::CEMC_QA = Enable::CEMC_CLUSTER && Enable::QA && true;

  Enable::HCALIN = true;
  Enable::HCALIN_ABSORBER = true;
  Enable::HCALIN_CELL = Enable::HCALIN && true;
  Enable::HCALIN_TOWER = Enable::HCALIN_CELL && true;
  Enable::HCALIN_CLUSTER = Enable::HCALIN_TOWER && true;
  Enable::HCALIN_EVAL = Enable::HCALIN_CLUSTER && true;
  Enable::HCALIN_QA = Enable::HCALIN_CLUSTER && Enable::QA && true;

  Enable::MAGNET = false;
  Enable::MAGNET_ABSORBER = false;

  Enable::HCALOUT = true;
  Enable::HCALOUT_ABSORBER = true;
  Enable::HCALOUT_CELL = Enable::HCALOUT && true;
  Enable::HCALOUT_TOWER = Enable::HCALOUT_CELL && true;
  Enable::HCALOUT_CLUSTER = Enable::HCALOUT_TOWER && true;
  Enable::HCALOUT_EVAL = Enable::HCALOUT_CLUSTER && true;
  Enable::HCALOUT_QA = Enable::HCALOUT_CLUSTER && Enable::QA && true;

  Enable::EPD = false;
  Enable::EPD_TILE = Enable::EPD && true;

  Enable::BEAMLINE = false;
//  Enable::BEAMLINE_ABSORBER = true;  // makes the beam line magnets sensitive volumes
//  Enable::BEAMLINE_BLACKHOLE = true; // turns the beamline magnets into black holes
  Enable::ZDC = false;
//  Enable::ZDC_ABSORBER = true;
//  Enable::ZDC_SUPPORT = true;
//  Enable::ZDC_TOWER = Enable::ZDC && true;
//  Enable::ZDC_EVAL = Enable::ZDC_TOWER && true;

  //! forward flux return plug door. Out of acceptance and off by default.
  //Enable::PLUGDOOR = true;
  Enable::PLUGDOOR_ABSORBER = false;

//  Enable::GLOBAL_RECO = (Enable::BBCFAKE || Enable::TRACKING_TRACK) && true;
  //Enable::GLOBAL_FASTSIM = true;

  //Enable::KFPARTICLE = true;
  //Enable::KFPARTICLE_VERBOSITY = 1;
  //Enable::KFPARTICLE_TRUTH_MATCH = true;
  //Enable::KFPARTICLE_SAVE_NTUPLE = true;

  Enable::CALOTRIGGER = Enable::CEMC_TOWER && Enable::HCALIN_TOWER && Enable::HCALOUT_TOWER && false;

  Enable::JETS = (Enable::GLOBAL_RECO || Enable::GLOBAL_FASTSIM) && false;
  Enable::JETS_EVAL = Enable::JETS && true;
  Enable::JETS_QA = Enable::JETS && Enable::QA && true;

  // HI Jet Reco for p+Au / Au+Au collisions (default is false for
  // single particle / p+p-only simulations, or for p+Au / Au+Au
  // simulations which don't particularly care about jets)
  Enable::HIJETS = Enable::JETS && Enable::CEMC_TOWER && Enable::HCALIN_TOWER && Enable::HCALOUT_TOWER && false;

  // 3-D topoCluster reconstruction, potentially in all calorimeter layers
  Enable::TOPOCLUSTER = Enable::CEMC_TOWER && Enable::HCALIN_TOWER && Enable::HCALOUT_TOWER && false;
  // particle flow jet reconstruction - needs topoClusters!
  Enable::PARTICLEFLOW = Enable::TOPOCLUSTER && true;
  // centrality reconstruction
  Enable::CENTRALITY = false;

  // new settings using Enable namespace in GlobalVariables.C
  Enable::BLACKHOLE = false;
  //Enable::BLACKHOLE_SAVEHITS = false; // turn off saving of bh hits
  //Enable::BLACKHOLE_FORWARD_SAVEHITS = false; // disable forward/backward hits
  //BlackHoleGeometry::visible = true;

  // run user provided code (from local G4_User.C)
  //Enable::USER = true;

  //===============
  // conditions DB flags
  //===============
  Enable::CDB = false;
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  //---------------
  // World Settings
  //---------------
  G4WORLD::PhysicsList = "FTFP_BERT"; //FTFP_BERT_HP best for calo
  //  G4WORLD::WorldMaterial = "G4_AIR"; // set to G4_GALACTIC for material scans

  //---------------
  // Magnet Settings
  //---------------

  //G4MAGNET::magfield =  string(getenv("CALIBRATIONROOT"))+ string("/Field/Map/sphenix3dbigmapxyz.root");  // default map from the calibration database
  G4MAGNET::magfield = "0."; // alternatively to specify a constant magnetic field, give a float number, which will be translated to solenoidal field in T, if string use as fieldmap name (including path)
//  G4MAGNET::magfield_rescale = 1.;  // make consistent with expected Babar field strength of 1.4T

  //---------------
  // Pythia Decayer
  //---------------
  // list of decay types in
  // $OFFLINE_MAIN/include/g4decayer/EDecayType.hh
  // default is All:
  // G4P6DECAYER::decayType = EDecayType::kAll;

  // Initialize the selected subsystems
  G4Init();

  //---------------------
  // GEANT4 Detector description
  //---------------------
  if (!Input::READHITS)
  {
    G4Setup();
  }

  //------------------
  // Detector Division
  //------------------

//  if ((Enable::BBC && Enable::BBCRECO) || Enable::BBCFAKE) Bbc_Reco();
//
//  if (Enable::MVTX_CELL) Mvtx_Cells();
//  if (Enable::INTT_CELL) Intt_Cells();
//  if (Enable::TPC_CELL) TPC_Cells();
//  if (Enable::MICROMEGAS_CELL) Micromegas_Cells();
//
//  if (Enable::CEMC_CELL) CEMC_Cells();

  if (Enable::HCALIN_CELL) HCALInner_Cells();

  if (Enable::HCALOUT_CELL) HCALOuter_Cells();

  //-----------------------------
  // CEMC towering and clustering
  //-----------------------------

//  if (Enable::CEMC_TOWER) CEMC_Towers();
//  if (Enable::CEMC_CLUSTER) CEMC_Clusters();

  //--------------
  // EPD tile reconstruction
  //--------------

//  if (Enable::EPD_TILE) EPD_Tiles();

  //-----------------------------
  // HCAL towering and clustering
  //-----------------------------

  if (Enable::HCALIN_TOWER) HCALInner_Towers();
  if (Enable::HCALIN_CLUSTER) HCALInner_Clusters();

  if (Enable::HCALOUT_TOWER) HCALOuter_Towers();
  if (Enable::HCALOUT_CLUSTER) HCALOuter_Clusters();

  // if enabled, do topoClustering early, upstream of any possible jet reconstruction
//  if (Enable::TOPOCLUSTER) TopoClusterReco();

  //--------------
  // SVTX tracking
  //--------------
//  if(Enable::TRACKING_TRACK)
//    {
//      TrackingInit();
//    }
//  if (Enable::MVTX_CLUSTER) Mvtx_Clustering();
//  if (Enable::INTT_CLUSTER) Intt_Clustering();
//  if (Enable::TPC_CLUSTER)
//    {
//      if(G4TPC::ENABLE_DIRECT_LASER_HITS || G4TPC::ENABLE_CENTRAL_MEMBRANE_HITS)
//	{
//	  TPC_LaserClustering();
//	}
//      else
//	{
//	  TPC_Clustering();
//	}
//    }
//  if (Enable::MICROMEGAS_CLUSTER) Micromegas_Clustering();
//
//  if (Enable::TRACKING_TRACK)
//  {
//    Tracking_Reco();
//  }
//
//  if(Enable::TRACKING_DIAGNOSTICS)
//    {
//      const std::string kshortFile = "./kshort_" + outputFile;
//      const std::string residualsFile = "./residuals_" + outputFile;
//
//      G4KshortReconstruction(kshortFile);
//      seedResiduals(residualsFile);
//    }

  //-----------------
  // Global Vertexing
  //-----------------

//  if (Enable::GLOBAL_RECO && Enable::GLOBAL_FASTSIM)
//  {
//    cout << "You can only enable Enable::GLOBAL_RECO or Enable::GLOBAL_FASTSIM, not both" << endl;
//    gSystem->Exit(1);
//  }
//  if (Enable::GLOBAL_RECO)
//  {
//    Global_Reco();
//  }
//  else if (Enable::GLOBAL_FASTSIM)
//  {
//    Global_FastSim();
//  }

  //-----------------
  // Centrality Determination
  //-----------------

  if (Enable::CENTRALITY)
  {
      Centrality();
  }

  //-----------------
  // Calo Trigger Simulation
  //-----------------

  if (Enable::CALOTRIGGER)
  {
    CaloTrigger_Sim();
  }

  //---------
  // Jet reco
  //---------

//  if (Enable::JETS) Jet_Reco();
//  if (Enable::HIJETS) HIJetReco();
//
//  if (Enable::PARTICLEFLOW) ParticleFlow();

  //----------------------
  // Simulation evaluation
  //----------------------
//  string outputroot = outputFile;
//  string remove_this = ".root";
//  size_t pos = outputroot.find(remove_this);
//  if (pos != string::npos)
//  {
//    outputroot.erase(pos, remove_this.length());
//  }
//  int start_event = (int) index*10;
//  if (Enable::HCALOUT_EVAL) HCALOuter_Eval(outputroot + "_g4hcalout_eval.root", start_event);
//  if (Enable::HCALIN_EVAL) HCALInner_Eval(outputroot + "_g4hcalin_eval.root", start_event);

  //--------------
  // Set up Input Managers
  //--------------

  InputManagers();

  if (Enable::PRODUCTION)
  {
    Production_CreateOutputDir();
  }

  if (Enable::DSTOUT)
  {
    string FullOutFile = DstOut::OutputDir + "/" + DstOut::OutputFile;
    Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", FullOutFile);
    if (Enable::DSTOUT_COMPRESS)
    {
      ShowerCompress();
      DstCompress(out);
    }
    se->registerOutputManager(out);
  }
  //-----------------
  // Event processing
  //-----------------

  // if we use a negative number of events we go back to the command line here
  if (nEvents < 0)
  {
    return 0;
  }
  // if we run the particle generator and use 0 it'll run forever
  // for embedding it runs forever if the repeat flag is set
  if (nEvents == 0 && !Input::HEPMC && !Input::READHITS && INPUTEMBED::REPEAT)
  {
    cout << "using 0 for number of events is a bad idea when using particle generators" << endl;
    cout << "it will run forever, so I just return without running anything" << endl;
    return 0;
  }
  
  HCalJetPhiShift *caloPhiShift = new HCalJetPhiShift("caloPhiShift",outputFile);
  caloPhiShift->SetEventNumber(event_number);
  se->registerSubsystem(caloPhiShift);
  se->run(nEvents);

  //-----
  // QA output
  //-----

  if (Enable::QA) QA_Output(outputroot + "_qa.root");

  //-----
  // Exit
  //-----

//  CDBInterface::instance()->Print(); // print used DB files
  se->End();
  std::cout << "All done" << std::endl;
  delete se;

  gSystem->Exit(0);
  return 0;
}
#endif

// initial val setter in evaluator, set ievent --> call function to set ievent in macro
// then modify g4 macro or put line in my macro

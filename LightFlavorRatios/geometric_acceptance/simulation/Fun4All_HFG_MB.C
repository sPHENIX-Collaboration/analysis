#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include "G4_Input.C"
#include <G4_Global.C>
#include <G4Setup_sPHENIX.C>

#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <Trkr_TruthTables.C>
#include <Trkr_Reco.C>
#include <Trkr_Eval.C>

#include <phpythia8/PHPy8ParticleTrigger.h>

#include <decayfinder/DecayFinder.h>
#include <hftrackefficiency/HFTrackEfficiency.h>
#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>
#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <simqa_modules/QAG4SimulationTracking.h>
#include <qautils/QAHistManagerDef.h>

#include "HF_selections.C"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libhftrackefficiency.so)
R__LOAD_LIBRARY(libsimqa_modules.so)

int Fun4All_HFG_MB(std::string processID = "000000", std::string channel = "lambdaKshort")
{
  int nEvents = 1000;

  int ndigits = 6;
  //std::string infile_base_reco = "/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/G4Hits_pileup_sample/DST_TRKR_G4HIT_pythia8_NONE-0000000029-";
  //std::string infile_base_truth = "/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/G4Hits_pileup_sample/DST_TRUTH_G4HIT_pythia8_NONE-0000000029-";
  std::string infile_base_reco = "DST_TRKR_G4HIT_pythia8_Detroit_200kHz-0000000029-";
  std::string infile_base_truth = "DST_TRUTH_G4HIT_pythia8_Detroit_200kHz-0000000029-";
  std::string infile_base = "/sphenix/lustre01/sphnxpro/mdc2/js_pp200_signal/g4hits/run0029/detroit/G4Hits_pythia8_Detroit-0000000029-";
  //std::string process_str = std::string(ndigits - std::to_string(process).length(),'0') + std::to_string(process);
  std::string infile = infile_base+processID+".root";
  std::string infile_reco = infile_base_reco+processID+".root";
  std::string infile_truth = infile_base_truth+processID+".root";

  //std::string outDir = "/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/" + channel + "_20260422_DetroitMB_CR_2_mode_pTref_1p4/";
  //std::string outDir = "./";

  std::string outDir = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/";

  string makeDirectory = "mkdir -p " + outDir + "hfEff";
  system(makeDirectory.c_str());
  makeDirectory = "mkdir -p " + outDir + "evaluator";
  system(makeDirectory.c_str());

  //F4A setup
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  PHRandomSeed::Verbosity(1);
  recoConsts *rc = recoConsts::instance();

  Enable::VERBOSITY = 0;
  Input::VERBOSITY = 0;

  //Generator setup

  //Input::PILEUPRATE = 3e6;


  Input::READHITS = false;
  //INPUTREADHITS::filename[0] = infile;
  INPUTREADHITS::filename[0] = infile_reco;
  INPUTREADHITS::filename[1] = infile_truth;

  std::cout << "readhits ssetup" << std::endl;
  Input::SIMPLE = true;

  Input::PYTHIA8 = true;
  int particleID = 421;
  PYTHIA8::config_file[0] = "steeringCards/pythia8_MB_Detroit_Tony.cfg";
  if (channel == "Kshort2pipi")
  {
    run_pipi_reco = true;
    particleID = 310;
  }
  else if (channel == "Lambda2ppi")
  {
    run_ppi_reco = true;
    particleID = 3122;
  }
  else if (channel == "Lambdabar2ppi")
  {
    run_anti_ppi_reco = true;
    particleID = -3122;
  }
  else if (channel == "cascade")
  {
    run_cascade_reco = true;
    particleID = 3312;
  }
  else if (channel == "minBias")
  {
    std::cout << "Min bias simulations" << std::endl; 
  }
  else if (channel == "lambdaKshort")
  {
    run_pipi_reco = true;
    run_ppi_reco = true;
    run_anti_ppi_reco = true;
  }
  else
  {
    std::cout << "Your decay channel " << channel << " is not known" << std::endl;
    exit(1); 
  }
  Input::BEAM_CONFIGURATION = Input::pp_COLLISION;

  InputInit();

  std::cout << "input init" << std::endl;

  float abs_eta = 1.1;

  
/*
  if (channel != "minBias" && channel != "lambdaKshort")
  {

    int pidtrig = particleID;

    auto trigger = new PHPy8ParticleTrigger();
    trigger->Verbosity(0);
    trigger->AddParticles(pidtrig);
    trigger->AddParticles(-1 * pidtrig);
    trigger->SetYHighLow(1.2, -1.2);
    trigger->SetStableParticleOnly(false);
    trigger->PrintConfig();
    // assign
    INPUTGENERATOR::Pythia8[0]->register_trigger(trigger);
    INPUTGENERATOR::Pythia8[0]->set_trigger_OR();
    Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8[0]);
*/
/*
    PHPy8ParticleTrigger * p8_hf_signal_trigger = new PHPy8ParticleTrigger("thisTrigger");
    p8_hf_signal_trigger->SetPtLow(0.);
    p8_hf_signal_trigger->SetPtHigh(5.);
    p8_hf_signal_trigger->SetYHighLow(1, -1); // sample a rapidity range higher than the sPHENIX tracking pseudorapidity
    p8_hf_signal_trigger->SetStableParticleOnly(false); // process unstable particles that include quarks
    p8_hf_signal_trigger->SetParticleRadialDecayVertexHigh(4);
    p8_hf_signal_trigger->AddParticles(particleID);
    p8_hf_signal_trigger->AddParticles(-1*particleID);
    p8_hf_signal_trigger->PrintConfig();
    INPUTGENERATOR::Pythia8[0]->register_trigger(p8_hf_signal_trigger);
    INPUTGENERATOR::Pythia8[0]->set_trigger_OR();

    vector<int> particleList;
    if (channel == "Kshort2pipi") particleList = {211, -211};
    else if (channel == "Lambda2ppi") particleList = {2212, -211};
    else if (channel == "cascade") particleList = {3122, -211};
    else particleList = {-2212, 211};
    
    for (unsigned int i = 0; i < particleList.size(); ++i)
    {
      string trigger_name = "particle_trigger_" + to_string(i);
      PHPy8ParticleTrigger * p8_hf_signal_trigger = new PHPy8ParticleTrigger(trigger_name.c_str());
      p8_hf_signal_trigger->SetEtaHighLow(abs_eta, -1*abs_eta);
      p8_hf_signal_trigger->SetPtLow(0.15);
      //p8_hf_signal_trigger->SetParentRadialDecayVertexHigh(4);
      p8_hf_signal_trigger->SetStableParticleOnly(false);
      p8_hf_signal_trigger->AddParents(particleID);
      p8_hf_signal_trigger->AddParticles(particleList[i]);
      p8_hf_signal_trigger->PrintConfig();
      INPUTGENERATOR::Pythia8[0]->register_trigger(p8_hf_signal_trigger);
    }
    INPUTGENERATOR::Pythia8[0]->set_trigger_AND();
  }
*/
  if(channel == "lambdaKshort")
  {
    run_pipi_reco = true;
    run_ppi_reco = true;
    run_anti_ppi_reco = true;
/*
    PHPy8ParticleTrigger* p8_lktrigger = new PHPy8ParticleTrigger("lktrigger");
    p8_lktrigger->SetEtaHighLow(abs_eta,-1*abs_eta);
    p8_lktrigger->SetPtLow(0.);
    //p8_lktrigger->SetParentRadialDecayVertexHigh(4);
    p8_lktrigger->SetStableParticleOnly(false);

    std::vector<int> mothers = {310,3122,-3122};

    p8_lktrigger->AddParticles(mothers);
    p8_lktrigger->PrintConfig();
    INPUTGENERATOR::Pythia8[0]->register_trigger(p8_lktrigger);
*/
  }

  Input::BEAM_CONFIGURATION = Input::pp_COLLISION;
  Enable::MVTX_APPLYMISALIGNMENT = true;

  Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8[0]);

  if(Input::SIMPLE)
  {
    INPUTGENERATOR::SimpleEventGenerator[0]->add_particles(310,10);
    INPUTGENERATOR::SimpleEventGenerator[0]->add_particles(3122,5);
    INPUTGENERATOR::SimpleEventGenerator[0]->add_particles(-3122,5);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_reuse_existing_vertex(true);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_eta_range(-1.,1.);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_phi_range(-M_PI,M_PI);
    INPUTGENERATOR::SimpleEventGenerator[0]->set_pt_range(0.,5.);
  }

  InputRegister();

  std::cout << "input registered" << std::endl;

  //CDB flags and such

  Enable::CDB = true;
  //rc->set_StringFlag("CDB_GLOBALTAG","MDC2");
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  //rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  rc->set_IntFlag("RUNNUMBER",29);

  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;
/*
  SyncReco *sync = new SyncReco();
  se->registerSubsystem(sync);

  HeadReco *head = new HeadReco();
  se->registerSubsystem(head);
*/
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  //Simulation setup
  Enable::MBDFAKE = true;
  Enable::PIPE = true;
  Enable::PIPE_ABSORBER = true;
  Enable::MVTX = true;
  Enable::INTT = true;
  Enable::TPC = true;
  Enable::MICROMEGAS = true;

  //Tracking setup
  TRACKING::streaming_mode = true;

  InputManagers();

  G4Init();
  MagnetInit();
  MagnetFieldInit();

  G4Setup();

  std::cout << "g4setup" << std::endl;
/*
  //Tagging stuff
  DecayFinder *myFinder = new DecayFinder("myFinder");
  myFinder->Verbosity(INT_MAX);
  if (channel == "Kshort2pipi") myFinder->setDecayDescriptor(pipi_decay_descriptor);
  else if (channel == "Lambda2ppi") myFinder->setDecayDescriptor(ppi_decay_descriptor);
  else if (channel == "cascade") myFinder->setDecayDescriptor(cascade_decay_descriptor);
  else myFinder->setDecayDescriptor(anti_ppi_decay_descriptor);
  myFinder->saveDST(1);
  myFinder->allowPi0(1);
  myFinder->allowPhotons(1);
  myFinder->triggerOnDecay(1);
  myFinder->setPTmin(0.); //Note: sPHENIX min pT is 0.2 GeV for tracking
  myFinder->setEtaRange(-1*abs_eta, abs_eta); //Note: sPHENIX acceptance is |eta| <= 1.1
  myFinder->useDecaySpecificEtaRange(false);
  if (channel != "minBias") se->registerSubsystem(myFinder);  
*/
  Mbd_Reco();
  Mvtx_Cells();
  Intt_Cells();
  TPC_Cells();
  Micromegas_Cells();

  TrackingInit();

  Mvtx_Clustering();
  Intt_Clustering();
  TPC_Clustering();
  Micromegas_Clustering();

  Tracking_Reco();

  auto vtxfinder = new PHSimpleVertexFinder;
  vtxfinder->Verbosity(0);
  vtxfinder->setDcaCut(1);
  vtxfinder->setTrackPtCut(-99999.);
  vtxfinder->setBeamLineCut(1);
  vtxfinder->setTrackQualityCut(1000000000);
  vtxfinder->setRequireMVTX(false);
  //vtxfinder->setNmvtxRequired(2);
  vtxfinder->setOutlierPairCut(1);  
  se->registerSubsystem(vtxfinder);

  Global_Reco();

  build_truthreco_tables();

  Tracking_Eval(outDir+"evaluator/outputeval_"+processID+".root");
/*
  HFTrackEfficiency *myTrackEff = new HFTrackEfficiency("myTrackEff");
  myTrackEff->Verbosity(INT_MAX);
  myTrackEff->setDFNodeName("myFinder");
  myTrackEff->triggerOnDecay(1);
  myTrackEff->writeSelectedTrackMap(true);
  myTrackEff->writeOutputFile(true);
  std::string outputHFEffFile = outDir + "/hfEff/outputHFTrackEff_" + channel + "_" + processID + ".root";
  myTrackEff->setOutputFileName(outputHFEffFile);
  if (channel != "minBias") se->registerSubsystem(myTrackEff);
*/
  output_dir = outDir;

  if (run_pipi_reco) create_hf_directories(pipi_reconstruction_name, pipi_output_dir, pipi_output_reco_file, processID);
  if (run_ppi_reco || run_anti_ppi_reco) create_hf_directories(ppi_reconstruction_name, ppi_output_dir, ppi_output_reco_file, processID);
  //if (run_anti_ppi_reco) create_hf_directories(anti_ppi_reconstruction_name, anti_ppi_output_dir, anti_ppi_output_reco_file, processID);
  if (run_cascade_reco) create_hf_directories(cascade_reconstruction_name, cascade_output_dir, cascade_output_reco_file, processID);

  if (run_pipi_reco) reconstruct_pipi_mass();
  if (run_ppi_reco || run_anti_ppi_reco) reconstruct_ppi_mass();
  //if (run_anti_ppi_reco) reconstruct_ppi_mass();
  if (run_cascade_reco) reconstruct_Lambdapi_mass();
/*
  //Output file handling
  makeDirectory = "mkdir -p " + outDir + "DST";
  system(makeDirectory.c_str());

  string FullOutFile = outDir + "/DST/" + channel + "_DST_" + processID + ".root";
  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", FullOutFile);
  out->StripNode("G4HIT_PIPE");
  out->StripNode("G4HIT_SVTXSUPPORT");
  //out->StripNode("PHG4INEVENT");
  //out->StripNode("Sync");
  out->StripNode("myFinder_DecayMap");
  out->StripNode("G4HIT_PIPE");
  out->StripNode("G4HIT_MVTX");
  out->StripNode("G4HIT_INTT");
  out->StripNode("G4HIT_TPC");
  out->StripNode("G4HIT_MICROMEGAS");
  out->StripNode("TRKR_HITSET");
  out->StripNode("TRKR_HITTRUTHASSOC");
  //out->StripNode("TRKR_CLUSTER");
  //out->StripNode("TRKR_CLUSTERHITASSOC");
  out->StripNode("TRKR_CLUSTERCROSSINGASSOC");
  out->StripNode("TRAINING_HITSET");
  out->StripNode("TRKR_TRUTHTRACKCONTAINER");
  out->StripNode("TRKR_TRUTHCLUSTERCONTAINER");
  out->StripNode("alignmentTransformationContainer");
  out->StripNode("alignmentTransformationContainerTransient");
  //out->StripNode("SiliconTrackSeedContainer");
  //out->StripNode("TpcTrackSeedContainer");
  //out->StripNode("SvtxTrackSeedContainer");
  out->StripNode("ActsTrajectories");
  //out->StripNode("SvtxTrackMap");
  out->StripNode("SvtxAlignmentStateMap");
  //out->SaveRunNode(0);
  se->registerOutputManager(out);
*/
  se->run(nEvents);

  se->End();

  if (run_pipi_reco) end_kfparticle(pipi_output_reco_file, pipi_output_dir);
  if (run_ppi_reco || run_anti_ppi_reco) end_kfparticle(ppi_output_reco_file, ppi_output_dir);
  //if (run_anti_ppi_reco) end_kfparticle(ppi_output_reco_file, ppi_output_dir);
  if (run_cascade_reco) end_kfparticle(cascade_output_reco_file, cascade_output_dir);

  gSystem->Exit(0);

  return 0;
}

#endif

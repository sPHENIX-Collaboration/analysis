#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include "G4_Input.C"
#include <G4_Global.C>
#include <G4Setup_sPHENIX.C>

#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <Trkr_TruthTables.C>
#include <Trkr_Reco.C>

#include <phpythia8/PHPy8ParticleTrigger.h>

#include <decayfinder/DecayFinder.h>
#include <hftrackefficiency/HFTrackEfficiency.h>

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

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libhftrackefficiency.so)
R__LOAD_LIBRARY(libsimqa_modules.so)

int Fun4All_HFG(std::string processID = "0")
{
  int nEvents = 2e2;
  std::string channel = "Lambda2ppi";
  std::string outDir = "./" + channel + "_20260304/";
  output_dir = outDir;

  string makeDirectory = "mkdir -p " + outDir + "hfEff";
  system(makeDirectory.c_str());

  //F4A setup
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  PHRandomSeed::Verbosity(1);
  recoConsts *rc = recoConsts::instance();

  //Generator setup

  Input::PYTHIA8 = true;
  int particleID = 421;

  if (channel == "Kshort2pipi")
  {
    PYTHIA8::config_file[0] = "steeringCards/pythia8_K2pipi_Detroit_limited.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/K2pipi.DEC";
    particleID = 310;
  }
  else if (channel == "Lambda2ppi")
  {
    PYTHIA8::config_file[0] = "steeringCards/pythia8_L02ppi_Detroit_limited.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/L02ppi.DEC";
    particleID = 3122;
  }
  else if (channel == "minBias")
  {
    std::cout << "Min bias simulations" << std::endl; 
  }
  else
  {
    std::cout << "Your decay channel " << channel << " is not known" << std::endl;
    exit(1); 
  }

  Input::BEAM_CONFIGURATION = Input::pp_COLLISION;

  InputInit();

  PHPy8ParticleTrigger * p8_hf_signal_trigger = new PHPy8ParticleTrigger();
  p8_hf_signal_trigger->SetPtLow(0.);
  p8_hf_signal_trigger->SetEtaHighLow(1e6, -1e6); // sample a rapidity range higher than the sPHENIX tracking pseudorapidity
  p8_hf_signal_trigger->SetStableParticleOnly(false); // process unstable particles that include quarks

  if (channel != "minBias")
  {
    p8_hf_signal_trigger->AddParticles(particleID);
    p8_hf_signal_trigger->AddParticles(-1*particleID);
    p8_hf_signal_trigger->PrintConfig();
    INPUTGENERATOR::Pythia8[0]->register_trigger(p8_hf_signal_trigger);
    INPUTGENERATOR::Pythia8[0]->set_trigger_OR();
  
    Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8);
  }

  InputRegister();

  //CDB flags and such

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP",1);
  rc->set_IntFlag("RUNNUMBER",1);

  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;

  SyncReco *sync = new SyncReco();
  se->registerSubsystem(sync);

  HeadReco *head = new HeadReco();
  se->registerSubsystem(head);

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

  G4Init();
  MagnetInit();
  MagnetFieldInit();

  G4Setup();

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
  vtxfinder->setDcaCut(0.5);
  vtxfinder->setTrackPtCut(-99999.);
  vtxfinder->setBeamLineCut(1);
  vtxfinder->setTrackQualityCut(1000000000);
  vtxfinder->setNmvtxRequired(3);
  vtxfinder->setOutlierPairCut(0.1);  
  se->registerSubsystem(vtxfinder);

  Global_Reco();

  build_truthreco_tables();

  //Tagging stuff
  DecayFinder *myFinder = new DecayFinder("myFinder");
  myFinder->Verbosity(INT_MAX);
  if (channel == "Kshort2pipi") myFinder->setDecayDescriptor("K_S0 -> pi^- pi^+");
  else myFinder->setDecayDescriptor("[Lambda0 -> proton^+ pi^-]cc");
  myFinder->saveDST(1);
  myFinder->allowPi0(1);
  myFinder->allowPhotons(1);
  myFinder->triggerOnDecay(1);
  myFinder->setPTmin(0.); //Note: sPHENIX min pT is 0.2 GeV for tracking
  myFinder->setEtaRange(-1e6,1e6); //Note: sPHENIX acceptance is |eta| <= 1.1
  myFinder->useDecaySpecificEtaRange(false);
  if (channel != "minBias") se->registerSubsystem(myFinder);  

  HFTrackEfficiency *myTrackEff = new HFTrackEfficiency("myTrackEff");
  myTrackEff->Verbosity(INT_MAX);
  myTrackEff->setDFNodeName("myFinder");
  myTrackEff->triggerOnDecay(0);
  myTrackEff->writeSelectedTrackMap(true);
  myTrackEff->writeOutputFile(true);
  std::string outputHFEffFile = outDir + "/hfEff/outputHFTrackEff_" + channel + "_" + processID + ".root";
  myTrackEff->setOutputFileName(outputHFEffFile);
  if (channel != "minBias") se->registerSubsystem(myTrackEff);
/*
  //Output file handling
  makeDirectory = "mkdir -p " + outDir + "DST";
  system(makeDirectory.c_str());

  string FullOutFile = outDir + "/DST/" + channel + "_DST_" + processID + ".root";
  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", FullOutFile);
  out->StripNode("G4HIT_PIPE");
  out->StripNode("G4HIT_SVTXSUPPORT");
  out->StripNode("PHG4INEVENT");
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

  gSystem->Exit(0);

  return 0;
}

#endif

#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <G4_Input.C>
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

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libhftrackefficiency.so)

int Fun4All_HFG(std::string processID = "0")
{
  int nEvents = 2e3;
  std::string channel = "D2Kpi";
  //F4A setup

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  PHRandomSeed::Verbosity(1);
  recoConsts *rc = recoConsts::instance();

  //Generator setup

  Input::PYTHIA8 = true;

  if (channel == "bs2jpsiks0")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_bs2jpsiks0.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/Bs2JpsiKS0.DEC";
  }
  else if (channel == "b2DX")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_b2DX.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/b2DX.DEC";
  }
  else if (channel == "D2Kpi")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_D2Kpi.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/D2Kpi.DEC";
  }
  else
  {
    std::cout << "Your decay channel " << channel << " is not known" << std::endl;
    exit(1); 
  }

  Input::BEAM_CONFIGURATION = Input::pp_COLLISION;

  InputInit();

  PHPy8ParticleTrigger * p8_hf_signal_trigger = new PHPy8ParticleTrigger();
  if (channel == "bs2jpsiks0")
  {
    p8_hf_signal_trigger->AddParticles(531);
    p8_hf_signal_trigger->AddParticles(-531);
  }
  else if (channel == "b2DX")
  {
    p8_hf_signal_trigger->AddParticles(5);
    p8_hf_signal_trigger->AddParticles(-5);
  }
  else
  {
    p8_hf_signal_trigger->AddParticles(421);
    p8_hf_signal_trigger->AddParticles(-421);
  }
  p8_hf_signal_trigger->SetPtLow(1.);
  p8_hf_signal_trigger->SetEtaHighLow(1.3, -1.3); // sample a rapidity range higher than the sPHENIX tracking pseudorapidity
  p8_hf_signal_trigger->SetStableParticleOnly(false); // process unstable particles that include quarks
  p8_hf_signal_trigger->PrintConfig();
  INPUTGENERATOR::Pythia8->register_trigger(p8_hf_signal_trigger);
  INPUTGENERATOR::Pythia8->set_trigger_OR();

  Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8);

  InputRegister();

  //CDB flags and such

  rc->set_IntFlag("RUNNUMBER",1);

  SyncReco *sync = new SyncReco();
  se->registerSubsystem(sync);

  HeadReco *head = new HeadReco();
  se->registerSubsystem(head);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);


  DecayFinder *myFinder = new DecayFinder("myFinder");
  myFinder->Verbosity(INT_MAX);
  if (channel == "bs2jpsiks0") myFinder->setDecayDescriptor("[B_s0 -> {J/psi -> e^+ e^-} {K_S0 -> pi^+ pi^-}]cc");
  else myFinder->setDecayDescriptor("[D0 -> K^- pi^+]cc");
  myFinder->saveDST(1);
  myFinder->allowPi0(1);
  myFinder->allowPhotons(1);
  myFinder->triggerOnDecay(1);
  myFinder->setPTmin(0.16); //Note: sPHENIX min pT is 0.2 GeV for tracking
  myFinder->setEtaRange(-1.2, 1.2); //Note: sPHENIX acceptance is |eta| <= 1.1
  myFinder->useDecaySpecificEtaRange(false);
  se->registerSubsystem(myFinder);  

  //Simulation setup

  Enable::MBDFAKE = true;
  Enable::PIPE = true;
  Enable::PIPE_ABSORBER = true;
  Enable::MVTX = true;
  Enable::INTT = true;
  Enable::TPC = true;
  Enable::MICROMEGAS = true;

  //Tracking setup

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);

  MagnetInit();
  MagnetFieldInit();

  G4Init();
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
  Global_Reco();

  build_truthreco_tables();

  //HF stuff

  HFTrackEfficiency *myTrackEff = new HFTrackEfficiency("myTrackEff");
  myTrackEff->Verbosity(INT_MAX);
  myTrackEff->setDFNodeName("myFinder");
  myTrackEff->triggerOnDecay(1);
  myTrackEff->writeSelectedTrackMap(true);
  myTrackEff->writeOutputFile(true);
  std::string outputHFEffFile = "./outputHFTrackEff_" + channel + "_" + processID + ".root";
  myTrackEff->setOutputFileName(outputHFEffFile);
  se->registerSubsystem(myTrackEff);

  //Output file handling

  string FullOutFile;
  if (channel == "bs2jpsiks0") FullOutFile = "./Bs2JpsiKS0_DST_" + processID + ".root";
  else if (channel == "b2DX") FullOutFile = "./b2DX_DST_" + processID + ".root";
  else FullOutFile = "./D2Kpi_DST_" + processID + ".root";
  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", FullOutFile);
  out->StripNode("G4HIT_PIPE");
  out->StripNode("G4HIT_SVTXSUPPORT");
  out->StripNode("PHG4INEVENT");
  out->StripNode("Sync");
  out->StripNode("myFinder_DecayMap");
  out->StripNode("G4HIT_PIPE");
  out->StripNode("G4HIT_MVTX");
  out->StripNode("G4HIT_INTT");
  out->StripNode("G4HIT_TPC");
  out->StripNode("G4HIT_MICROMEGAS");
  out->StripNode("TRKR_HITSET");
  out->StripNode("TRKR_HITTRUTHASSOC");
  out->StripNode("TRKR_CLUSTER");
  out->StripNode("TRKR_CLUSTERHITASSOC");
  out->StripNode("TRKR_CLUSTERCROSSINGASSOC");
  out->StripNode("TRAINING_HITSET");
  out->StripNode("TRKR_TRUTHTRACKCONTAINER");
  out->StripNode("TRKR_TRUTHCLUSTERCONTAINER");
  out->StripNode("alignmentTransformationContainer");
  out->StripNode("alignmentTransformationContainerTransient");
  out->StripNode("SiliconTrackSeedContainer");
  out->StripNode("TpcTrackSeedContainer");
  out->StripNode("SvtxTrackSeedContainer");
  out->StripNode("ActsTrajectories");
  out->StripNode("SvtxTrackMap");
  out->StripNode("SvtxAlignmentStateMap");
  out->SaveRunNode(0);
  se->registerOutputManager(out);

  se->run(nEvents);
  se->End();
  gSystem->Exit(0);

  return 0;
}

#endif

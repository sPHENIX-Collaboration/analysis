#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <G4_Input.C>
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

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>
#include <trackingdiagnostics/TrkrNtuplizer.h>

#include "../data/HF_selections_QM25.C"

//#include <dstarreco/DstarReco.h>

//R__LOAD_LIBRARY(libDstarReco.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libhftrackefficiency.so)
R__LOAD_LIBRARY(libsimqa_modules.so)

using namespace HeavyFlavorReco;

int Fun4All_HFG(std::string processID = "0",
                int nEvents = 10,
                std::string channel = "Kshort2pipi",
                std::string outDir = "./")
{
  //F4A setup

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  PHRandomSeed::Verbosity(1);
  recoConsts *rc = recoConsts::instance();

  //Generator setup

  Input::PYTHIA8 = true;
  int particleID = 421;

  if (channel == "bs2jpsiks0")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_bs2jpsiks0.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/Bs2JpsiKS0.DEC";
    particleID = 531;
  }
  else if (channel == "b2DX")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_b2DX.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/b2DX.DEC";
    particleID = 5;
  }
  else if (channel == "D2Kpi")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_D2Kpi.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/D2Kpi.DEC";
  }
  else if ((channel == "Dp2KKpi") || (channel == "Ds2KKpi"))
  {
    PYTHIA8::config_file = "steeringCards/pythia8_D2KKpi.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/D2KKpi.DEC";
    if (channel == "Dp2KKpi") particleID = 411;
    else particleID = 431;
  }
  else if (channel == "Dstar2D0pi")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_Dstar2D0pi.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/Dstar2D0pi.DEC";
    particleID = 413;
  }
  else if (channel == "Kshort2pipi")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_K2pipi_Detroit.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/K2pipi.DEC";
    particleID = 310;
  }
  else if (channel == "phi2KK")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_phi2KK_Detroit.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/phi2KK.DEC";
    particleID = 333;
  }
  else if (channel == "lambda2ppi")
  {
    PYTHIA8::config_file = "steeringCards/pythia8_lambda2ppi_Detroit.cfg";
    EVTGENDECAYER::DecayFile = "decFiles/lambda2ppi.DEC";
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
  p8_hf_signal_trigger->SetEtaHighLow(1.3, -1.3); // sample a rapidity range higher than the sPHENIX tracking pseudorapidity
  p8_hf_signal_trigger->SetStableParticleOnly(false); // process unstable particles that include quarks
  p8_hf_signal_trigger->PrintConfig();

  if (channel != "minBias")
  {
    p8_hf_signal_trigger->AddParticles(particleID);
    p8_hf_signal_trigger->AddParticles(-1*particleID);
    INPUTGENERATOR::Pythia8->register_trigger(p8_hf_signal_trigger);
    INPUTGENERATOR::Pythia8->set_trigger_OR();
  
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

  DecayFinder *myFinder = new DecayFinder("myFinder");
  myFinder->Verbosity(INT_MAX);
  if (channel == "bs2jpsiks0") myFinder->setDecayDescriptor("[B_s0 -> {J/psi -> e^+ e^-} {K_S0 -> pi^+ pi^-}]cc");
  else if  (channel == "Kshort2pipi") myFinder->setDecayDescriptor("K_S0 -> pi^- pi^+");
  else if  (channel == "Dp2KKpi") myFinder->setDecayDescriptor("[D+ -> {phi -> K^+ K^-} pi^+]cc");
  else if  (channel == "Ds2KKpi") myFinder->setDecayDescriptor("[D_s+ -> {phi -> K^+ K^-} pi^+]cc");
  else if  (channel == "Dstar2D0pi") myFinder->setDecayDescriptor("[D*+ -> {D0 -> K^- pi^+} pi^+]cc");
  else if  (channel == "phi2KK") myFinder->setDecayDescriptor("phi -> K^+ K^-");
  else if  (channel == "lambda2ppi") myFinder->setDecayDescriptor("[Lambda0 -> proton^+ pi^-]cc");
  else myFinder->setDecayDescriptor("[D0 -> K^- pi^+]cc");
  myFinder->saveDST(1);
  myFinder->allowPi0(1);
  myFinder->allowPhotons(1);
  myFinder->triggerOnDecay(1);
  myFinder->setPTmin(0.16); //Note: sPHENIX min pT is 0.2 GeV for tracking
  myFinder->setEtaRange(-1.2, 1.2); //Note: sPHENIX acceptance is |eta| <= 1.1
  myFinder->useDecaySpecificEtaRange(false);
  if (channel != "minBias") se->registerSubsystem(myFinder);  

  //Simulation setup
/*
  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

  Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo);
*/
  Enable::MBDFAKE = true;
  Enable::PIPE = true;
  Enable::PIPE_ABSORBER = true;
  Enable::MVTX = true;
  Enable::INTT = true;
  Enable::TPC = true;
  Enable::MICROMEGAS = true;

  //Tracking setup

  G4Init();
/*
  PHG4Reco *g4Reco = new PHG4Reco();
  g4Reco->set_rapidity_coverage(1.8);  // according to drawings
  WorldInit(g4Reco);
*/
  MagnetInit();
  MagnetFieldInit();

  G4Setup();
/*
  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  se->registerSubsystem(g4Reco);
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

  string eval_dir = outDir + "/trutheval/";
  string eval_output_reco_file = eval_dir + processing_folder + "trutheval_output" + trailer;

  std::string makeDirectory = "mkdir -p " + eval_dir + processing_folder;
  system(makeDirectory.c_str());

  Tracking_Eval(eval_output_reco_file);

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

  //HF stuff

  output_dir = outDir; //Top dir of where the output nTuples will be written
  trailer = "_" + processID + ".root";

  if (run_pipi_reco) create_hf_directories(pipi_reconstruction_name, pipi_output_dir, pipi_output_reco_file);
  if (run_ppi_reco) create_hf_directories(ppi_reconstruction_name, ppi_output_dir, ppi_output_reco_file);
  if (run_KK_reco) create_hf_directories(KK_reconstruction_name, KK_output_dir, KK_output_reco_file);

  if (run_pipi_reco) reconstruct_pipi_mass();
  if (run_ppi_reco) reconstruct_ppi_mass();
  if (run_KK_reco) reconstruct_KK_mass();

  string nTuplizer_dir = output_dir + "/nTP/";
  string NTP_output_reco_file = nTuplizer_dir + processing_folder + "nTuplizer_tracks" + trailer;

  std::string makeDirectory = "mkdir -p " + nTuplizer_dir + processing_folder;
  system(makeDirectory.c_str());

  TrkrNtuplizer* eval = new TrkrNtuplizer("TrkrNtuplizer", NTP_output_reco_file, "SvtxTrackMap", 3, 4, 48);
  int do_default = 0;
  eval->Verbosity(0);
  eval->do_hit_eval(false);
  eval->do_cluster_eval(false);
  eval->do_clus_trk_eval(false);
  eval->do_vertex_eval(false);
  eval->do_track_eval(true);
  eval->do_tpcseed_eval(false);
  eval->do_siseed_eval(false);
  se->registerSubsystem(eval);

  HFTrackEfficiency *myTrackEff = new HFTrackEfficiency("myTrackEff");
  myTrackEff->Verbosity(INT_MAX);
  //myTrackEff->setTruthRecoMatchingPercentage(100.);
  myTrackEff->setDFNodeName("myFinder");
  myTrackEff->triggerOnDecay(1);
  myTrackEff->writeSelectedTrackMap(true);
  myTrackEff->writeOutputFile(true);
  std::string outputHFEffFile = outDir + "/hfEff/outputHFTrackEff_" + channel + "_" + processID + ".root";
  myTrackEff->setOutputFileName(outputHFEffFile);
  if (channel != "minBias") se->registerSubsystem(myTrackEff);
/*
  if (channel == "Dstar2D0pi")
  {
    DstarReco *myDstarReco = new DstarReco();
    std::string outputDstarfFile = outDir + "/standalone/outputStandaloneReco_" + channel + "_" + processID + ".root";
    myDstarReco->setOutputName(outputDstarfFile);
    se->registerSubsystem(myDstarReco);
  }

  if (run_pipi_reco || run_Kpi_reco || run_Kpipi_reco || run_KKpi_reco || run_pKpi_reco || run_Lambdapi_reco) init_kfp_dependencies();
  if (run_pipi_reco) reconstruct_pipi_mass();
*/
  //Output file handling

  string FullOutFile = outDir + "/" + channel + "_DST_" + processID + ".root";
  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", FullOutFile);
  //out->StripNode("G4HIT_PIPE");
  //out->StripNode("G4HIT_SVTXSUPPORT");
  //out->StripNode("PHG4INEVENT");
  //out->StripNode("Sync");
  out->StripNode("myFinder_DecayMap");
  //out->StripNode("G4HIT_PIPE");
  //out->StripNode("G4HIT_MVTX");
  //out->StripNode("G4HIT_INTT");
  //out->StripNode("G4HIT_TPC");
  //out->StripNode("G4HIT_MICROMEGAS");
  //out->StripNode("TRKR_HITSET");
  //out->StripNode("TRKR_HITTRUTHASSOC");
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

  se->run(nEvents);

  se->End();

  end_kfparticle(NTP_output_reco_file, nTuplizer_dir);

  if (run_pipi_reco) end_kfparticle(pipi_output_reco_file, pipi_output_dir);
  if (run_ppi_reco) end_kfparticle(ppi_output_reco_file, pipi_output_dir);
  if (run_KK_reco) end_kfparticle(KK_output_reco_file, KK_output_dir);

  gSystem->Exit(0);

  return 0;
}

#endif

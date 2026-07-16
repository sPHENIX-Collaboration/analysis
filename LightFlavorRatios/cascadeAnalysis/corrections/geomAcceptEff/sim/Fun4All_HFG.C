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

#include <Trkr_Eval.C>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libhftrackefficiency.so)
R__LOAD_LIBRARY(libsimqa_modules.so)

int Fun4All_HFG(std::string processID = "0", std::string channel = "cascade")
{
  int nEvents = 1e3;
  //int nEvents = 1e1;
  string date = "20260513";
  std::string outDir = "./" + channel + "_" + date + "/";

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
  PYTHIA8::config_file[0] = "steeringCards/pythia8_MB_Detroit_Tony.cfg";
  if (channel == "Kshort2pipi")
  {
    run_pipi_reco = false;
    particleID = 310;
  }
  else if (channel == "Lambda2ppi")
  {
    run_ppi_reco = false;
    particleID = 3122;
  }
  else if (channel == "Lambdabar2ppi")
  {
    run_anti_ppi_reco = false;
    particleID = -3122;
  }
  else if (channel == "cascade")
  {
    run_cascade_reco = false;
    particleID = 3312;
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

  float abs_eta = 10;

  if (channel != "minBias")
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
*/
/*
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
      p8_hf_signal_trigger->SetParentRadialDecayVertexHigh(4);
      p8_hf_signal_trigger->SetStableParticleOnly(false);
      p8_hf_signal_trigger->AddParents(particleID);
      p8_hf_signal_trigger->AddParticles(particleList[i]);
      p8_hf_signal_trigger->PrintConfig();
      INPUTGENERATOR::Pythia8[0]->register_trigger(p8_hf_signal_trigger);
    }
    INPUTGENERATOR::Pythia8[0]->set_trigger_AND();
*/
  }

  Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8[0]);

  InputRegister();

  //CDB flags and such

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG","MDC2");
  rc->set_uint64Flag("TIMESTAMP",1);
  rc->set_IntFlag("RUNNUMBER",1);

  Enable::MVTX_APPLYMISALIGNMENT = true; //Check if this is default
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
  myFinder->triggerOnDecay(0);
  myFinder->setPTmin(0.); //Note: sPHENIX min pT is 0.2 GeV for tracking
  myFinder->setEtaRange(-1*abs_eta, abs_eta); //Note: sPHENIX acceptance is |eta| <= 1.1
  myFinder->useDecaySpecificEtaRange(false);
  if (channel != "minBias") se->registerSubsystem(myFinder);  

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
  vtxfinder->setRequireMVTX(false); //Set back to true?
  //vtxfinder->setNmvtxRequired(2);
  vtxfinder->setOutlierPairCut(1);  
  se->registerSubsystem(vtxfinder);

  Global_Reco();

  build_truthreco_tables();

/*
 *
 * Thomas, I also added the lines below (ending before HFTrackEfficiency)
 *
 */
  string evalDir = outDir + "evaluatorFiles/" + channel + "/" + date + "/";
  makeDirectory = "mkdir -p " + evalDir;
  system(makeDirectory.c_str());
  string evalFile = evalDir + "evalFile_" + channel + "_" + processID + ".root";

  SvtxEvaluator* eval;
  eval = new SvtxEvaluator("SVTXEVALUATOR", evalFile, "SvtxTrackMap",
                           G4MVTX::n_maps_layer,
                           G4INTT::n_intt_layer,
                           G4TPC::n_gas_layer,
                           G4MICROMEGAS::n_micromegas_layer);
  eval->do_cluster_eval(false);
  eval->do_g4hit_eval(false);
  eval->do_hit_eval(false);  // enable to see the hits that includes the chamber physics...
  eval->do_gpoint_eval(false);
  eval->do_vtx_eval_light(false);
  eval->do_eval_light(false);
  eval->do_info_eval(false);
  eval->do_vertex_eval(false);
  eval->do_g4cluster_eval(false);
  eval->do_track_eval(false);
  eval->do_gtrack_eval(true);
  eval->do_track_match(true);
  eval->set_use_initial_vertex(G4TRACKING::g4eval_use_initial_vertex);
  bool embed_scan = false;
  if (TRACKING::pp_mode) embed_scan = false;
  eval->scan_for_embedded(embed_scan);   // take all tracks if false - take only embedded tracks if true
  eval->scan_for_primaries(embed_scan);  // defaults to only thrown particles for ntp_gtrack
  eval->Verbosity(0);
  se->registerSubsystem(eval);

  HFTrackEfficiency *myTrackEff = new HFTrackEfficiency("myTrackEff");
  myTrackEff->Verbosity(INT_MAX);
  myTrackEff->setDFNodeName("myFinder");
  myTrackEff->triggerOnDecay(0);
  myTrackEff->writeSelectedTrackMap(true);
  myTrackEff->writeOutputFile(true);
  std::string outputHFEffFile = outDir + "/hfEff/outputHFTrackEff_" + channel + "_" + processID + ".root";
  myTrackEff->setOutputFileName(outputHFEffFile);
  if (channel != "minBias" && false) se->registerSubsystem(myTrackEff);

  output_dir = outDir;

  if (run_pipi_reco) create_hf_directories(pipi_reconstruction_name, pipi_output_dir, pipi_output_reco_file, processID);
  if (run_ppi_reco) create_hf_directories(ppi_reconstruction_name, ppi_output_dir, ppi_output_reco_file, processID);
  if (run_anti_ppi_reco) create_hf_directories(anti_ppi_reconstruction_name, anti_ppi_output_dir, anti_ppi_output_reco_file, processID);
  if (run_cascade_reco) create_hf_directories(cascade_reconstruction_name, cascade_output_dir, cascade_output_reco_file, processID);

  if (run_pipi_reco) reconstruct_pipi_mass();
  if (run_ppi_reco) reconstruct_ppi_mass();
  if (run_anti_ppi_reco) reconstruct_ppi_mass();
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
  if (run_ppi_reco) end_kfparticle(ppi_output_reco_file, ppi_output_dir);
  if (run_anti_ppi_reco) end_kfparticle(ppi_output_reco_file, ppi_output_dir);
  if (run_cascade_reco) end_kfparticle(cascade_output_reco_file, cascade_output_dir);

  gSystem->Exit(0);

  return 0;
}

#endif

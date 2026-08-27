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

#include "HF_Selections.C"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libhftrackefficiency.so)
R__LOAD_LIBRARY(libsimqa_modules.so)

int Fun4All_HFG(std::string processID = "0", std::string channel = "cascade")
{
  int nEvents = 1e3;
  //int nEvents = 10;
  std::string outDir = "./" + channel + "_20260513_DetroitMB_CR_2_mode_pTref_1p4/";

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
  if (channel == "cascade_minus")
  {
    particleID = 3312;
  }
  else if (channel == "cascade_plus")
  {
    particleID = -3312;
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

  float abs_eta = 1.1;
  float pt_cut = 0.0;

  if (channel != "minBias")
  {
  
    vector<int> particleList;
    int lambdaID;
    if (channel == "cascade_minus") 
    {
      particleList = {2212,-211};
      lambdaID = 3122;
    }
    else if (channel == "cascade_plus") 
    {
      particleList = {-2212,211};
      lambdaID = -3122;
    }    

    for (unsigned int i = 0; i < particleList.size(); ++i)
    {
      string trigger_name = "particle_trigger_" + to_string(i);
      PHPy8ParticleTrigger * p8_hf_signal_trigger = new PHPy8ParticleTrigger(trigger_name.c_str());
      p8_hf_signal_trigger->SetEtaHighLow(abs_eta, -1*abs_eta);
      p8_hf_signal_trigger->SetPtLow(pt_cut);
      p8_hf_signal_trigger->SetStableParticleOnly(false);
      p8_hf_signal_trigger->AddParents(lambdaID);
      p8_hf_signal_trigger->AddParticles(particleList[i]);
      p8_hf_signal_trigger->SetParentRadialDecayVertexHigh(3.5);
      //p8_hf_signal_trigger->AddParticles(particleID);
      //p8_hf_signal_trigger->AddParticles(-1*particleID);
      p8_hf_signal_trigger->PrintConfig();
      INPUTGENERATOR::Pythia8[0]->register_trigger(p8_hf_signal_trigger);
    }
    PHPy8ParticleTrigger * p8_hf_signal_trigger_bachelor = new PHPy8ParticleTrigger("particle_trigger_bachelor");
    p8_hf_signal_trigger_bachelor->SetEtaHighLow(abs_eta, -1*abs_eta);
    p8_hf_signal_trigger_bachelor->SetPtLow(pt_cut);
    p8_hf_signal_trigger_bachelor->SetParentRadialDecayVertexHigh(3.5);
    p8_hf_signal_trigger_bachelor->SetStableParticleOnly(false);
    p8_hf_signal_trigger_bachelor->AddParents(particleID);
    if (channel == "cascade_minus")
    {
      p8_hf_signal_trigger_bachelor->AddParticles(-211);
    }
    else if (channel == "cascade_plus")
    {
      p8_hf_signal_trigger_bachelor->AddParticles(211);
    }
    //p8_hf_signal_trigger_bachelor->AddParticles(particleID);
    //p8_hf_signal_trigger_bachelor->AddParticles(-1*particleID);
    p8_hf_signal_trigger_bachelor->PrintConfig();
    INPUTGENERATOR::Pythia8[0]->register_trigger(p8_hf_signal_trigger_bachelor);

    INPUTGENERATOR::Pythia8[0]->set_trigger_AND();
    //INPUTGENERATOR::Pythia8[0]->set_trigger_OR();
  
    Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8);
  }

  InputRegister();

  //CDB flags and such

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
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

  //Tagging stuff
  DecayFinder *myFinder = new DecayFinder("myFinder");
  myFinder->Verbosity(INT_MAX);
  if (channel == "cascade_minus" || channel == "cascade_plus") myFinder->setDecayDescriptor("[Xi- -> {Lambda0 -> proton^+ pi^-} pi^-]cc");
  myFinder->saveDST(1);
  myFinder->allowPi0(1);
  myFinder->allowPhotons(1);
  myFinder->triggerOnDecay(1);
  myFinder->setPTmin(pt_cut); //Note: sPHENIX min pT is 0.2 GeV for tracking
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
  vtxfinder->setRequireMVTX(false);
  //vtxfinder->setNmvtxRequired(2);
  vtxfinder->setOutlierPairCut(1);  
  se->registerSubsystem(vtxfinder);

  Global_Reco();

  build_truthreco_tables();
  
  output_dir = outDir;
  if (true) create_hf_directories(Lambdapi_reconstruction_name, Lambdapi_output_dir, Lambdapi_output_reco_file, processID);

  HFTrackEfficiency *myTrackEff = new HFTrackEfficiency("myTrackEff");
  myTrackEff->Verbosity(INT_MAX);
  myTrackEff->setDFNodeName("myFinder");
  myTrackEff->triggerOnDecay(1);
  myTrackEff->writeSelectedTrackMap(true);
  myTrackEff->writeOutputFile(true);
  std::string outputHFEffFile = outDir + "/hfEff/outputHFTrackEff_" + channel + "_" + processID + ".root";
  myTrackEff->setOutputFileName(outputHFEffFile);
  if (channel != "minBias" && false) se->registerSubsystem(myTrackEff);  

  std::string dir_eval = "TrackingEval";
  if ((channel == "cascade_minus" || channel == "cascade_plus") && false)
  {
    system(("mkdir -p " + Lambdapi_output_dir + "/" + dir_eval).c_str());
    Tracking_Eval(Form("%s/%s/outputTrackingEval_%s_%s.root", Lambdapi_output_dir.c_str(), dir_eval.c_str(), channel.c_str(), processID.c_str()));
  }

  //if (run_pipi_reco) create_hf_directories(pipi_reconstruction_name, pipi_output_dir, pipi_output_reco_file);
  //if (run_ppi_reco) create_hf_directories(ppi_reconstruction_name, ppi_output_dir, ppi_output_reco_file);
  //if (run_anti_ppi_reco) create_hf_directories(anti_ppi_reconstruction_name, anti_ppi_output_dir, anti_ppi_output_reco_file);


  //if (run_pipi_reco) reconstruct_pipi_mass();
  //if (run_ppi_reco) reconstruct_ppi_mass();
  //if (run_anti_ppi_reco) reconstruct_ppi_mass();
  if (run_Lambdapi_reco) reconstruct_Lambdapi_mass();
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

  //if (run_pipi_reco) end_kfparticle(pipi_output_reco_file, pipi_output_dir);
  //if (run_ppi_reco) end_kfparticle(ppi_output_reco_file, ppi_output_dir);
  //if (run_anti_ppi_reco) end_kfparticle(ppi_output_reco_file, ppi_output_dir);
  if (run_Lambdapi_reco) end_kfparticle(Lambdapi_output_reco_file, Lambdapi_output_dir);

  gSystem->Exit(0);

  return 0;
}

#endif

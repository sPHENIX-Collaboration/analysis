#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetReco.h>
#include <g4jets/TruthJetInput.h>
#include <jetbackground/RetowerCEMC.h>
#include <jetbase/TrackJetInput.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <TSystem.h>

#include <g4centrality/PHG4CentralityReco.h>
#include <string>

//#include "HIJetReco.C"

#include <caloreco/RawClusterBuilderTopo.h>
#include <particleflowreco/ParticleFlowReco.h>
#include <particleflowreco/ParticleFlowJetInput.h>


//#include <jetvertextagging/JetVertexTagging.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundefined-internal"
#include <FullJetFinder.h>
#pragma GCC diagnostic pop

#include <fstream>

#include <globalvertex/GlobalVertexReco.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libFullJetFinder.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libg4jets.so)


//void Fun4All_JetVal(const char *filelisttruth = "dst_truth_jet.list",
//                     const char *filelistcalo = "dst_calo_cluster.list",  
//                     const char *outname = "outputest.root")
void Fun4All_FullJetFinder(std::string outDir = "./", std::vector<std::string> myInputLists = {"condorJob/fileLists/productionFiles-CHARM-dst_tracks-00000.LIST"}, const int nEvents = 1){

  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  bool whichR[7] = {false, false, true,false, false, false, false};

  FullJetFinder::TYPE jet_type = FullJetFinder::TYPE::CHARGEJET;

  //std::string outDir = "./";

  //std::string outDir = "/sphenix/tg/tg01/hf/jkvapil/JET30_r8/";
  if (outDir.substr(outDir.size() - 1, 1) != "/") outDir += "/";
  outDir += "myTestJets";



  std::string fileNumber = myInputLists[0];
  size_t findLastDash = fileNumber.find_last_of("-");
  if (findLastDash != std::string::npos) fileNumber.erase(0, findLastDash + 1);
  std::string remove_this = ".list";
  size_t pos = fileNumber.find(remove_this);
  if (pos != std::string::npos) fileNumber.erase(pos, remove_this.length());
  std::string outputFileName = "outputData_" + fileNumber + ".root";

  std::string outputRecoDir = outDir + "/inReconstruction/";
  std::string makeDirectory = "mkdir -p " + outputRecoDir;
  system(makeDirectory.c_str());
  std::string outputRecoFile = outputRecoDir + outputFileName;

  //Create the server
  se->Verbosity(verbosity);

  //Add all required input files
  for (unsigned int i = 0; i < myInputLists.size(); ++i)
  {
    Fun4AllInputManager *infile = new Fun4AllDstInputManager("DSTin_" + std::to_string(i));
    infile->AddListFile(myInputLists[i]);
    se->registerInputManager(infile);
  }
  //recoConsts *rc = recoConsts::instance();

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, std::string(getenv("CALIBRATIONROOT")) + std::string("/Centrality/"));
  se->registerSubsystem( cent );

  /* GlobalVertexReco* gblvertex = new GlobalVertexReco();
  gblvertex->Verbosity(0);
  se->registerSubsystem(gblvertex);*/

  //HIJetReco();
  //JetReco *truthjetreco = new JetReco("JetReco_truth",JetReco::TRANSITION::JET_MAP);
  JetReco *truthjetreco = new JetReco("JetReco_truth");
      TruthJetInput *tji = new TruthJetInput(Jet::PARTICLE);
      //tji->Verbosity(10);
      tji->add_embedding_flag(1);  // changes depending on signal vs. embedded
      truthjetreco->add_input(tji);
     /* if(whichR[0])truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_Truth_r02");  actually you cannot rename it, or you get empty trees...sad
      if(whichR[1])truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.3), "AntiKt_Truth_r03");
      if(whichR[2])truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "AntiKt_Truth_r04");
      if(whichR[3])truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.5), "AntiKt_Truth_r05");
      if(whichR[4])truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.6), "AntiKt_Truth_r06");
      if(whichR[5])truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.7), "AntiKt_Truth_r07");
      if(whichR[6])truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.8), "AntiKt_Truth_r08");*/
      if(whichR[0])truthjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.2, VERBOSITY, verbosity, CALC_AREA}}), "C_AntiKt_Truth_r02");
      if(whichR[1])truthjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.3, VERBOSITY, verbosity, CALC_AREA}}), "C_AntiKt_Truth_r03");
      if(whichR[2])truthjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.4, VERBOSITY, verbosity, CALC_AREA}}), "C_AntiKt_Truth_r04");
      if(whichR[3])truthjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.5, VERBOSITY, verbosity, CALC_AREA}}), "C_AntiKt_Truth_r05");
      if(whichR[4])truthjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.6, VERBOSITY, verbosity, CALC_AREA}}), "C_AntiKt_Truth_r06");
      if(whichR[5])truthjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.7, VERBOSITY, verbosity, CALC_AREA}}), "C_AntiKt_Truth_r07");
      if(whichR[6])truthjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.8, VERBOSITY, verbosity, CALC_AREA}}), "C_AntiKt_Truth_r08");
      truthjetreco->set_algo_node("ANTIKT");
      truthjetreco->set_input_node("TRUTH");
      truthjetreco->Verbosity(0);
      se->registerSubsystem(truthjetreco);

  if(jet_type == FullJetFinder::TYPE::FULLJET){

        RetowerCEMC *rcemc = new RetowerCEMC(); 
  rcemc->Verbosity(verbosity); 
  rcemc->set_towerinfo(true);
 se->registerSubsystem(rcemc);


  RawClusterBuilderTopo* ClusterBuilder1 = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo1");
  ClusterBuilder1->Verbosity(verbosity);
  ClusterBuilder1->set_nodename("TOPOCLUSTER_EMCAL");
  ClusterBuilder1->set_enable_HCal(false);
  ClusterBuilder1->set_enable_EMCal(true);
  ClusterBuilder1->set_noise(0.0025, 0.006, 0.03);
  ClusterBuilder1->set_significance(4.0, 2.0, 0.0);
  ClusterBuilder1->allow_corner_neighbor(true);
  ClusterBuilder1->set_do_split(true);
  ClusterBuilder1->set_minE_local_max(1.0, 2.0, 0.5);
  ClusterBuilder1->set_R_shower(0.025);
  se->registerSubsystem(ClusterBuilder1);

  RawClusterBuilderTopo* ClusterBuilder2 = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo2");
  ClusterBuilder2->Verbosity(verbosity);
  ClusterBuilder2->set_nodename("TOPOCLUSTER_HCAL");
  ClusterBuilder2->set_enable_HCal(true);
  ClusterBuilder2->set_enable_EMCal(false);
  ClusterBuilder2->set_noise(0.0025, 0.006, 0.03);
  ClusterBuilder2->set_significance(4.0, 2.0, 0.0);
  ClusterBuilder2->allow_corner_neighbor(true);
  ClusterBuilder2->set_do_split(true);
  ClusterBuilder2->set_minE_local_max(1.0, 2.0, 0.5);
  ClusterBuilder2->set_R_shower(0.025);
  se->registerSubsystem(ClusterBuilder2);

  ParticleFlowReco *pfr = new ParticleFlowReco();
  pfr->set_energy_match_Nsigma(1.5);
  pfr->Verbosity(verbosity);
  se->registerSubsystem(pfr);

  }

  JetReco *towerjetreco = new JetReco();
  if(jet_type == FullJetFinder::TYPE::FULLJET)towerjetreco->add_input(new ParticleFlowJetInput());
  if(jet_type == FullJetFinder::TYPE::CHARGEJET)towerjetreco->add_input(new TrackJetInput(Jet::TRACK));
  /*if(whichR[0])towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2, verbosity), "AntiKt_reco_r02");
  if(whichR[1])towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.3, verbosity), "AntiKt_reco_r03");
  if(whichR[2])towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4, verbosity), "AntiKt_reco_r04");
  if(whichR[3])towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.5, verbosity), "AntiKt_reco_r05");
  if(whichR[4])towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.6, verbosity), "AntiKt_reco_r06");
  if(whichR[5])towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.7, verbosity), "AntiKt_reco_r07");
  if(whichR[6])towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.8, verbosity), "AntiKt_reco_r08");*/
  if(whichR[0])towerjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.2, VERBOSITY, verbosity, CALC_AREA}}), "AntiKt_reco_r02");
  if(whichR[1])towerjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.3, VERBOSITY, verbosity, CALC_AREA}}), "AntiKt_reco_r03");
  if(whichR[2])towerjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.4, VERBOSITY, verbosity, CALC_AREA, CONSTITUENT_MIN_PT, 0.3}}), "AntiKt_reco_r04");
  if(whichR[3])towerjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.5, VERBOSITY, verbosity, CALC_AREA}}), "AntiKt_reco_r05");
  if(whichR[4])towerjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.6, VERBOSITY, verbosity, CALC_AREA}}), "AntiKt_reco_r06");
  if(whichR[5])towerjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.7, VERBOSITY, verbosity, CALC_AREA}}), "AntiKt_reco_r07");
  if(whichR[6])towerjetreco->add_algo(new FastJetAlgo({{Jet::ANTIKT, JET_R, 0.8, VERBOSITY, verbosity, CALC_AREA}}), "AntiKt_reco_r08");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("INCLUSIVE_RECO");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);
 
  FullJetFinder *myJetVal = new FullJetFinder(outputRecoFile,jet_type);//{"AntiKt_r02","AntiKt_r03","AntiKt_r04","AntiKt_r05","AntiKt_r06"});
  if(whichR[0])myJetVal->add_input("AntiKt_reco_r02", "C_AntiKt_Truth_r02","AntiKt_r02");
  if(whichR[1])myJetVal->add_input("AntiKt_reco_r03", "C_AntiKt_Truth_r03","AntiKt_r03");
  if(whichR[2])myJetVal->add_input("AntiKt_reco_r04", "C_AntiKt_Truth_r04","AntiKt_r04");
  if(whichR[3])myJetVal->add_input("AntiKt_reco_r05", "C_AntiKt_Truth_r05","AntiKt_r05");
  if(whichR[4])myJetVal->add_input("AntiKt_reco_r06", "C_AntiKt_Truth_r06","AntiKt_r06");
  if(whichR[5])myJetVal->add_input("AntiKt_reco_r07", "C_AntiKt_Truth_r07","AntiKt_r07");
  if(whichR[6])myJetVal->add_input("AntiKt_reco_r08", "C_AntiKt_Truth_r08","AntiKt_r08");
  myJetVal->doFiducialAcceptance(true);
  myJetVal->setPtRangeReco(5, 100);
  myJetVal->setPtRangeTruth(10, 100);
  myJetVal->doTruth(true);
  se->registerSubsystem(myJetVal);


  
  se->run(nEvents);
  se->End();

  std::ifstream file(outputRecoFile.c_str());
  if (file.good())
  {
    std::string moveOutput = "mv " + outputRecoFile + " " + outDir;
    system(moveOutput.c_str());
  }

  gSystem->Exit(0);
  return;

}

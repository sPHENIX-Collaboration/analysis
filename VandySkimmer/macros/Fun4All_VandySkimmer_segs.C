#include <string>
#include <iostream>
#include <format>

#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <ffamodules/CDBInterface.h>
#include <Calo_Calib.C>

#include <caloreco/RawClusterBuilderTopo.h>

#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/JetCalib.h>
#include <jetbackground/FastJetAlgoSub.h>

#include <g4jets/TruthJetInput.h>

//#include <globalvertex/GlobalVertexReco.h>
//#include <GlobalVertex.h>                                                            
//#include <MbdDigitization.h>
//#include <MbdReco.h>

#include <jetbackground/TimingCut.h>

#include <vandyskimmer/VandyJetDSTSkimmer.h>
//#include <jetbackground/RetowerCEMC.h>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libVandySkimmer.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)

void Fun4All_VandySkimmer_segs(int segStart, const std::string caloDSTlist, const std::string jetDSTlist, const std::string outDir = "/sphenix/tg/tg01/jets/bkimelman/wEEC/")
{

  bool doSim = false;

  Fun4AllServer* se=Fun4AllServer::instance();
  se->Verbosity(1);

  int runnumber = 0;
  
  std::ifstream ifs(caloDSTlist);
  std::string filepath;
  std::getline(ifs,filepath);
  std::pair<int,int> runseg = Fun4AllUtils::GetRunSegment(filepath);
  runnumber = runseg.first;

  Fun4AllInputManager *inCalo = new Fun4AllDstInputManager("InputManagerCalo");
  inCalo->AddListFile(caloDSTlist);
  se->registerInputManager(inCalo);
  
  Fun4AllInputManager *inJet = new Fun4AllDstInputManager("InputManagerJet");
  inJet->AddListFile(jetDSTlist);
  se->registerInputManager(inJet);
  
  auto rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(0);
  Process_Calo_Calib();

  RawClusterBuilderTopo* ClusterBuilder = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo");
  ClusterBuilder->Verbosity(0);
  ClusterBuilder->set_nodename("TOPOCLUSTER_ALLCALO");
  ClusterBuilder->set_enable_HCal(true);
  ClusterBuilder->set_enable_EMCal(true);
  ClusterBuilder->set_noise(0.0053, 0.0351, 0.0684); // 3sigma of pedestal noise
  ClusterBuilder->set_significance(4.0, 2.0, 1.0);
  ClusterBuilder->allow_corner_neighbor(true);
  ClusterBuilder->set_do_split(true);
  ClusterBuilder->set_minE_local_max(1.0, 2.0, 0.5);
  ClusterBuilder->set_R_shower(0.025);
  ClusterBuilder->set_use_only_good_towers(true);
  ClusterBuilder->set_absE(true);
  se->registerSubsystem(ClusterBuilder);

  JetReco *towerjetreco = new JetReco("towerJetReco");
  TowerJetInput* emtji = new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER,"TOWERINFO_CALIB");
  TowerJetInput* ihtji = new TowerJetInput(Jet::HCALIN_TOWERINFO,"TOWERINFO_CALIB");
  TowerJetInput* ohtji = new TowerJetInput(Jet::HCALOUT_TOWERINFO,"TOWERINFO_CALIB");
  emtji->set_GlobalVertexType(GlobalVertex::MBD);
  ihtji->set_GlobalVertexType(GlobalVertex::MBD);
  ohtji->set_GlobalVertexType(GlobalVertex::MBD);
  towerjetreco->add_input(emtji);
  towerjetreco->add_input(ihtji);
  towerjetreco->add_input(ohtji);
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2), "AntiKt_r02");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.3), "AntiKt_r03");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4), "AntiKt_r04");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.5), "AntiKt_r05");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(0);
  se->registerSubsystem(towerjetreco);

  JetCalib *jetCalib02 = new JetCalib("JetCalib02");
  jetCalib02->set_InputNode("AntiKt_r02");
  jetCalib02->set_OutputNode("AntiKt_r02_calib");
  jetCalib02->set_JetRadius(0.2);
  jetCalib02->set_ZvrtxNode("GlobalVertexMap");
  jetCalib02->set_ApplyZvrtxDependentCalib(true);
  jetCalib02->set_ApplyEtaDependentCalib(true);
  se->registerSubsystem(jetCalib02);

  JetCalib *jetCalib03 = new JetCalib("JetCalib03");
  jetCalib03->set_InputNode("AntiKt_r03");
  jetCalib03->set_OutputNode("AntiKt_r03_calib");
  jetCalib03->set_JetRadius(0.3);
  jetCalib03->set_ZvrtxNode("GlobalVertexMap");
  jetCalib03->set_ApplyZvrtxDependentCalib(true);
  jetCalib03->set_ApplyEtaDependentCalib(true);
  se->registerSubsystem(jetCalib03);

  JetCalib *jetCalib04 = new JetCalib("JetCalib04");
  jetCalib04->set_InputNode("AntiKt_r04");
  jetCalib04->set_OutputNode("AntiKt_r04_calib");
  jetCalib04->set_JetRadius(0.4);
  jetCalib04->set_ZvrtxNode("GlobalVertexMap");
  jetCalib04->set_ApplyZvrtxDependentCalib(true);
  jetCalib04->set_ApplyEtaDependentCalib(true);
  se->registerSubsystem(jetCalib04);

  JetCalib *jetCalib05 = new JetCalib("JetCalib05");
  jetCalib05->set_InputNode("AntiKt_r05");
  jetCalib05->set_OutputNode("AntiKt_r05_calib");
  jetCalib05->set_JetRadius(0.5);
  jetCalib05->set_ZvrtxNode("GlobalVertexMap");
  jetCalib05->set_ApplyZvrtxDependentCalib(true);
  jetCalib05->set_ApplyEtaDependentCalib(true);
  se->registerSubsystem(jetCalib05);

  if(doSim)
  {
    JetReco *truthjetreco = new JetReco("truthJetReco");
    truthjetreco->add_input(new TruthJetInput(Jet::PARTICLE));
    truthjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2), "AntiKt_Truth_r02");
    truthjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.3), "AntiKt_Truth_r03");
    truthjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4), "AntiKt_Truth_r04");
    truthjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.5), "AntiKt_Truth_r05");
    truthjetreco->set_algo_node("ANTIKT");
    truthjetreco->set_input_node("TRUTH");
    truthjetreco->Verbosity(0);
    se->registerSubsystem(truthjetreco);
  }

  TimingCut* tc = new TimingCut("AntiKt_r04","TimingCutModule",false); //arguments: 1. jet node name to use, 2. name for fun4all module, 3. doAbort. The last of these tells the module whether or not to abort events that fail the cuts.
  tc->Verbosity(0);
  se->registerSubsystem(tc);

  VandyJetDSTSkimmer *vs = new VandyJetDSTSkimmer("VandyJetDSTSkimmer");
  vs->SetRunnumber(runnumber);
  vs->SetOutfileName(std::format("{}/VandyDSTs_run2pp_ana521_2025p007_v001-{:08}-{}_{}.root",outDir, runnumber,25*segStart,25*segStart+24).c_str());
  vs->SetDoSim(doSim);
  vs->SetDoCalib(true);
  vs->Verbosity(0);
  se->registerSubsystem(vs);

  se->run(-1);
  se->End();
  se->PrintTimer();

  delete se;
  gSystem->Exit(0);
}

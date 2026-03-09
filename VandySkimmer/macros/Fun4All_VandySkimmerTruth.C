#include <string>
#include <sstream>
#include <iostream>
#include <format>

#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <ffamodules/CDBInterface.h>
#include "Calo_CalibLocal.C"

#include <caloreco/RawClusterBuilderTopo.h>

#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbackground/FastJetAlgoSub.h>

#include <g4jets/TruthJetInput.h>
//#include <globalvertex/GlobalVertexReco.h>
//#include <GlobalVertex.h>                                                            
//#include <MbdDigitization.h>
//#include <MbdReco.h>

#include <jetbackground/TimingCut.h>

#include <vandyskimmer/VandyJetDSTSkimmer.h>
#include <jetbackground/RetowerCEMC.h>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libVandySkimmer.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libg4dst.so)

void Fun4All_VandySkimmerTruth(const std::string caloDSTlist, const std::string jetDSTlist, const std::string g4HitsDSTlist, const std::string globalDSTlist, const std::string outDir = "/sphenix/tg/tg01/jets/bkimelman/wEEC/", const std::string nfiles="25")
{

  bool doSim = true;
	
  Fun4AllServer* se=Fun4AllServer::instance();
  se->Verbosity(0);


  int runnumber = 0;
  int seg = 0;
  int n=0;
  n = std::stoi(nfiles);

  std::ifstream ifs(caloDSTlist);
  std::string filepath;
  std::getline(ifs,filepath);
  std::pair<int,int> runseg = Fun4AllUtils::GetRunSegment(filepath);
  runnumber = runseg.first;
  seg = runseg.second;

  std::string sample_name {"Jet20"};
  std::stringstream fullfilename (caloDSTlist);
  std::string temp1, temp2;
  while(std::getline(fullfilename, temp1, '/'))
  {
	  if(temp1.find("Jet") == std::string::npos) continue;
	  std::stringstream filetag (temp1);
	  while(std::getline(filetag, temp2, '_'))
	  {
		  if(temp2.find("data") == std::string::npos){
			  sample_name = temp2;
			  break;
		  }
	  }
	  break;
  }
  Fun4AllInputManager *inCalo = new Fun4AllDstInputManager("InputManagerCalo");
  inCalo->AddListFile(caloDSTlist);
  se->registerInputManager(inCalo);
  
  Fun4AllInputManager *inJet = new Fun4AllDstInputManager("InputManagerJet");
  inJet->AddListFile(jetDSTlist);
  se->registerInputManager(inJet);
  
  Fun4AllInputManager *inTruth = new Fun4AllDstInputManager("InputManagerG4Hits");
  inTruth->AddListFile(g4HitsDSTlist);
  se->registerInputManager(inTruth);
  
  Fun4AllInputManager *inGlobal = new Fun4AllDstInputManager("InputManagerGlobal");
  inGlobal->AddListFile(globalDSTlist);
  se->registerInputManager(inGlobal);
  
  auto rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
  rc->set_uint64Flag("TIMESTAMP", 28);
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

  RetowerCEMC* rtcemc = new RetowerCEMC("RetowerCEMV");
  rtcemc->Verbosity(0);
  rtcemc->set_towerinfo(true);
  rtcemc->set_frac_cut(0.5);
  se->registerSubsystem(rtcemc);

  JetReco *towerjetreco = new JetReco("towerJetReco");
  TowerJetInput* emtji = new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER,"TOWERINFO_CALIB_RETOWER");
  TowerJetInput* ohtji = new TowerJetInput(Jet::HCALIN_TOWERINFO,"TOWERINFO_CALIB");
  TowerJetInput* ihtji = new TowerJetInput(Jet::HCALOUT_TOWERINFO,"TOWERINFO_CALIB");
  emtji->set_GlobalVertexType(GlobalVertex::VTXTYPE::MBD);
  ohtji->set_GlobalVertexType(GlobalVertex::VTXTYPE::MBD);
  ihtji->set_GlobalVertexType(GlobalVertex::VTXTYPE::MBD);
  towerjetreco->add_input(emtji);
  towerjetreco->add_input(ohtji);
  towerjetreco->add_input(ihtji);
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2), "AntiKt_r02");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.3), "AntiKt_r03");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4), "AntiKt_r04");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.5), "AntiKt_r05");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(0);
  se->registerSubsystem(towerjetreco);

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

  if(!doSim)
  {
	TimingCut* tc = new TimingCut("AntiKt_r04","TimingCutModule",false); //arguments: 1. jet node name to use, 2. name for fun4all module, 3. doAbort. The last of these tells the module whether or not to abort events that fail the cuts.
  	tc->Verbosity(0);
 	se->registerSubsystem(tc);
  }
  VandyJetDSTSkimmer *vs = new VandyJetDSTSkimmer("VandyJetDSTSkimmer");
  vs->SetRunnumber(runnumber);
  vs->SetOutfileName(std::format("{}/VandyDSTs_run2pp_ana521_2025p007_v001_{}-{}-{:06d}_to-{:06d}.root", outDir, sample_name, runnumber,seg, seg+n).c_str());
  vs->SetDoSim(doSim);
  std::cout<<"Sample name: " <<sample_name<<std::endl;
  if(doSim) vs->SetDoCalib(false);
  if(doSim) vs->SetSimSample(sample_name);
  std::cout<<vs->GetSimSample() <<std::endl;
  se->registerSubsystem(vs);
  se->run(0);
  se->End();
  se->PrintTimer();

  delete se;
  gSystem->Exit(0);
}

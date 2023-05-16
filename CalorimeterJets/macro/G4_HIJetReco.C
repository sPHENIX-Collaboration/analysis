#ifndef MACRO_G4HIJETRECO_C
#define MACRO_G4HIJETRECO_C

#include <GlobalVariables.C>

#include <g4jets/FastJetAlgo.h>
#include <g4jets/JetReco.h>
#include <g4jets/TowerJetInput.h>
#include <g4jets/TruthJetInput.h>
#include <g4centrality/PHG4CentralityReco.h>

#include <jetbackground/CopyAndSubtractJets.h>
#include <jetbackground/DetermineTowerBackground.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/SubtractTowers.h>
#include <jetbackground/SubtractTowersCS.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
 
#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <myjetanalysis/MyJetAnalysis.h>
#include <negativetowerremover/NegativeTowerRemover.h>

#include <fastjet/PseudoJet.hh>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libmyjetanalysis.so)
R__LOAD_LIBRARY(libg4centrality.so)

namespace Enable
{
  bool HIJETS = false;
  int HIJETS_VERBOSITY = 0;
}  // namespace Enable

namespace G4HIJETS
{
  bool do_flow = false;
  bool do_CS = false;
}  // namespace G4HIJETS

void G4_HIJetReco(const char *filelisttruth = "dst_truth.list",
                  const char *filelisttruthjet = "dst_truth_jet.list", 
                  const char *filelisthits = "dst_bbc_g4hit.list",
 		  const char *filelistcalo = "dst_calo_cluster.list",
		  const string &outname = "output_000002",
                  int n_skip = 2,
                  int n_event = 5
	         )
   {
   gSystem->Load("libg4dst");
   gSystem->Load("libmyjetanalysis");

  int verbosity = 0;//std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY);

  //---------------
  // Fun4All server
  //--------------1-

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );

//JetRecolysis::InitRun" << end`l;
//  //m_jetEvalStack = shared_ptr<JetEvalStack>(new JetEvalStack(topNode, m_recoJetName, m_truthJetName));
//    //m_jetEvalStack->get_stvx_eval_stack()->set_use_initial_vertex(initial_vertex);
//      return Fun4AllReturnCodes::EVENT_OK;
//      *truthjetreco = new JetReco();
  //TruthJetInput *tji = new TruthJetInput(Jet::PARTICLE);
  //tji->add_embedding_flag(1);  // changes depending on signal vs. embedded
  //truthjetreco->add_input(tji);
  //truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_Truth_r02_sub");
  //truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.3), "AntiKt_Truth_r03_sub");
  //truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "AntiKt_Truth_r04_sub");
  //truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.5), "AntiKt_Truth_r05_sub");
  ///truthjetreco->set_algo_node("ANTIKT");
  //truthjetreco->set_input_node("TRUTH");
  //truthjetreco->Verbosity(verbosity);
  //se->registerSubsystem(truthjetreco);

  RetowerCEMC *rcemc = new RetowerCEMC();
  rcemc->Verbosity(verbosity);
  se->registerSubsystem(rcemc);
  
  JetReco *towerjetreco = new JetReco();
  //towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_RETOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_Tower_HIRecoSeedsRaw_r02");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);
  
  DetermineTowerBackground *dtb = new DetermineTowerBackground();
  dtb->SetBackgroundOutputName("TowerBackground_Sub1");
  dtb->SetFlow(G4HIJETS::do_flow);
  dtb->SetSeedType(0);
  dtb->SetSeedJetD(3);
  dtb->Verbosity(verbosity);
  se->registerSubsystem(dtb);

  CopyAndSubtractJets *casj = new CopyAndSubtractJets();
  casj->SetFlowModulation(G4HIJETS::do_flow);
  //casj->SetBackgroundInputName("TowerBackground_Sub1");
  //casj->SetJetInputName("AntiKt_Tower_HIRecoSeedsRaw_r02");
  //casj->SetJetOutputName("AntiKt_Tower_HIRecoSeedsSub_r02");
  casj->Verbosity(verbosity);
  se->registerSubsystem(casj);

  DetermineTowerBackground *dtb2 = new DetermineTowerBackground();
  dtb2->SetBackgroundOutputName("TowerBackground_Sub2");
  dtb2->SetFlow(G4HIJETS::do_flow);
  dtb2->SetSeedType(1);
  dtb2->SetSeedJetPt(7);
  dtb2->Verbosity(verbosity);
  se->registerSubsystem(dtb2);

  //CopyAndSubtractJets *casj_final = new CopyAndSubtractJets();
  //casj_final->SetFlowModulation(G4HIJETS::do_flow);
  //casj_final->SetBackgroundInputName("TowerBackground_Sub2");
  //casj_final->SetJetInputName("AntiKt_Tower_HIRecoSeedsRaw_r02");
  //casj_final->SetJetOutputName("AntiKt_Tower_r02_Sub1_V2");
  //casj_final->Verbosity(verbosity);
  //se->registerSubsystem(casj_final);

  //cout << "Started Subtracting Towers" << endl;

  SubtractTowers *st = new SubtractTowers();
  st->SetFlowModulation(G4HIJETS::do_flow);
  st->Verbosity(verbosity);
  se->registerSubsystem(st);
  /*
  NegativeTowerRemover *ntr = new NegativeTowerRemover();
  ntr->SetFlowModulation(G4HIJETS::do_flow);
  ntr->Verbosity(verbosity);
  se->registerSubsystem(ntr);
  */
 
  //JetReco *towerjetreco = new JetReco();
  towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_SUB1));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER_SUB1));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER_SUB1));
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2, verbosity), "AntiKt_Tower_r02_Sub1");
  //towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.3, verbosity), "AntiKt_Tower_r03_Sub1");
  //towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4, verbosity), "AntiKt_Tower_r04_Sub1");
  //towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.5, verbosity), "AntiKt_Tower_r05_Sub1");
  
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  MyJetAnalysis *myJetAnalysis = new MyJetAnalysis("AntiKt_Tower_r02_Sub1", "AntiKt_Truth_r02", "myjet_ppPileup_R02_" + outname + ".root");
  myJetAnalysis->setPtRange(15, 200);
  myJetAnalysis->setEtaRange(-1.1, 1.1);
  myJetAnalysis->setMindR(0.2);
  //myJetAnalysis->doECCprint(false);
  se->registerSubsystem(myJetAnalysis);

  //cout << "Finished Constructing the Jets" << endl;

  //MyJetAnalysis *myJetAnalysis_R04 = new MyJetAnalysis("AntiKt_Tower_r04", "AntiKt_Truth_r04", "output/myjet_ppPileup_R04_" + outname + ".root");
  //myJetAnalysis_R04->setPtRange(15, 200);
  //myJetAnalysis_R04->setEtaRange(-1.1, 1.1);
  //myJetAnalysis_R04->setMindR(0.2);
  //myJetAnalysis_R04->doECCprint(false);
  //se->registerSubsystem(myJetAnalysis_R04);

  string inputFile = "DST_TRUTH_JET_pythia8_Jet30_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000062-00000.root";
  string inputFile0 = "DST_CALO_CLUSTER_pythia8_Jet30_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000062-00000.root";
  string inputFile1 = "DST_TRUTH_pythia8_Jet30_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000062-00000.root";
  string inputFile2 = "DST_BBC_G4HIT_pythia8_Jet30_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000062-00000.root";

  //cout << "Got Strings" << endl;

  Fun4AllInputManager *intrue2 = new Fun4AllDstInputManager("DSTtruth2");
  intrue2->AddListFile(filelisttruthjet,1);
  se->registerInputManager(intrue2);

  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTtruth");
  intrue->AddListFile(filelisttruth,1);
  se->registerInputManager(intrue);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddListFile(filelistcalo,1);
  se->registerInputManager(in);
   
  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSThit");
  in2->AddListFile(filelisthits,1);
  se->registerInputManager(in2);
  
  //se->skip(134);
  
  //se->run(-1);
  //cout << "Ran" << endl;
  se->run(n_event);
  se->skip(n_skip);

  se->End();
  
  gSystem->Exit(0);
  return 0;
}
#endif

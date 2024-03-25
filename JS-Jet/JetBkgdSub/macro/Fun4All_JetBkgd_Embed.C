#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllSyncManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <centrality/CentralityReco.h>

#include <caloreco/CaloTowerStatus.h>

#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/FastJetAlgoSub.h>

#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/FastJetAlgo.h>
#include <g4jets/TruthJetInput.h>

#include <caloembedding/caloTowerEmbed.h>

#include <jetbkgdsub/JetBkgdSub.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libJetBkgdSub.so)
R__LOAD_LIBRARY(libcentrality_io.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libCaloEmbedding.so)
R__LOAD_LIBRARY(libcalo_reco.so)


#endif


void Fun4All_JetBkgd_Embed(
  const char *filelistdata = "dst_data.list",
  const char *filelistsim = "dst_sim.list",
  const char * output = "output",
  const double jet_parameter = 0.4
)
{

  std::string outputbase = output;

  bool doCEMC = true;
  bool doIHCAL = true;
  bool doOHCAL = true;

  //-----------------------------------
  // Fun4All server initialization
  //-----------------------------------

  // create fun4all server
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 1;
  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();
  
  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2023");
  rc->set_uint64Flag("TIMESTAMP",23745);


  CentralityReco *cent = new CentralityReco("CentralityReco");
  se->registerSubsystem(cent);

  
  /*
  // retower CEMC
  RetowerCEMC *rcemc = new RetowerCEMC();
  rcemc->set_towerinfo(true);
  rcemc->Verbosity(verbosity); 
  se->registerSubsystem(rcemc);
  */

  /*
  //-----------------------------------
  // Jet reco
  //-----------------------------------
  Enable::HIJETS_TRUTH=false;
  Enable::HIJETS_VERBOSITY=0;
  HIJetReco();
    
  // tower jets
  // create jetreco and jettruth node names
  string rawname = "AntiKt_Tower_r0" + to_string(int(jet_parameter * 10));
  JetReco *towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, jet_parameter), rawname);
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  */


  if(doCEMC){
    CaloTowerStatus *statusEMC = new CaloTowerStatus("CEMCSTATUS");
    statusEMC->set_detector_type(CaloTowerDefs::CEMC);
    statusEMC->set_time_cut(1);
    statusEMC->set_inputNodePrefix("TOWERINFO_CALIB_");
    se->registerSubsystem(statusEMC);

    caloTowerEmbed *embedder_CEMC = new caloTowerEmbed("embedder_CEMC");
    embedder_CEMC->set_detector_type(CaloTowerDefs::CEMC);
    embedder_CEMC->set_removeBadTowers(true);
    embedder_CEMC->set_inputNodePrefix("TOWERINFO_CALIB_");
    se->registerSubsystem(embedder_CEMC);
  }

  if(doIHCAL){
    CaloTowerStatus *statusIHCAL = new CaloTowerStatus("IHCALSTATUS");
    statusIHCAL->set_detector_type(CaloTowerDefs::HCALIN);
    statusIHCAL->set_time_cut(2);
    statusIHCAL->set_inputNodePrefix("TOWERINFO_CALIB_");
    se->registerSubsystem(statusIHCAL);

    caloTowerEmbed *embedder_IHCAL = new caloTowerEmbed("embedder_IHCAL");
    embedder_IHCAL->set_detector_type(CaloTowerDefs::HCALIN);
    embedder_IHCAL->set_removeBadTowers(true);
    embedder_IHCAL->set_inputNodePrefix("TOWERINFO_CALIB_");
    se->registerSubsystem(embedder_IHCAL);
  }

  if(doOHCAL){
    caloTowerEmbed *embedder_OHCAL = new caloTowerEmbed("embedder_OHCal");
    embedder_OHCAL->set_detector_type(CaloTowerDefs::HCALOUT);
    embedder_OHCAL->set_removeBadTowers(true);
    embedder_OHCAL->set_inputNodePrefix("TOWERINFO_CALIB_");
    se->registerSubsystem(embedder_OHCAL);
    
    CaloTowerStatus *statusOHCAL = new CaloTowerStatus("OHCALSTATUS");
    statusOHCAL->set_detector_type(CaloTowerDefs::HCALOUT);
    statusOHCAL->set_time_cut(2);
    statusOHCAL->set_inputNodePrefix("TOWERINFO_CALIB_");
    se->registerSubsystem(statusOHCAL);
  }

  // ==============
  // Jet Bkgd Sub
  // ==============
  double etamin = -1.1 + jet_parameter;
  double etamax = 1.1 - jet_parameter;
  //data
  JetBkgdSub *myJetTree = new JetBkgdSub(jet_parameter,outputbase + ".root");
  if(doCEMC) myJetTree->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO));
  if(doIHCAL) myJetTree->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO));
  if(doOHCAL) myJetTree->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO));
  myJetTree->doIterative(false);
  myJetTree->doAreaSub(true);
  myJetTree->doMultSub(false);
  myJetTree->doTruth(false);
  myJetTree->doData(true);
  myJetTree->doEmbed(false);
  myJetTree->doTowerECut(true);
  myJetTree->setTowerThreshold(0.05);
  myJetTree->setMinRecoPt(5.0); // only sets range for reco jets
  myJetTree->setEtaRange(etamin, etamax);
  myJetTree->setPtRange(10, 100); // only sets range for truth jets
  myJetTree->Verbosity(verbosity);
  se->registerSubsystem(myJetTree);


  //embed
  JetBkgdSub *myJetTreeEmbed = new JetBkgdSub(jet_parameter,outputbase + "_embed.root");
  if(doCEMC) myJetTreeEmbed->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_EMBED));
  if(doIHCAL) myJetTreeEmbed->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO_EMBED));
  if(doOHCAL) myJetTreeEmbed->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO_EMBED));
  myJetTreeEmbed->doIterative(false);
  myJetTreeEmbed->doAreaSub(true);
  myJetTreeEmbed->doMultSub(false);
  myJetTreeEmbed->doTruth(false);
  myJetTreeEmbed->doData(true);
  myJetTreeEmbed->doEmbed(true);
  myJetTreeEmbed->doTowerECut(true);
  myJetTreeEmbed->setTowerThreshold(0.05);
  myJetTreeEmbed->setMinRecoPt(5.0); // only sets range for reco jets
  myJetTreeEmbed->setEtaRange(etamin, etamax);
  myJetTreeEmbed->setPtRange(10, 100); // only sets range for truth jets
  myJetTreeEmbed->Verbosity(verbosity);
  se->registerSubsystem(myJetTreeEmbed);

  //sim
  JetBkgdSub *myJetTreeSim = new JetBkgdSub(jet_parameter,outputbase + "_sim.root");
  if(doCEMC) myJetTreeSim->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_SIM));
  if(doIHCAL) myJetTreeSim->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO_SIM));
  if(doOHCAL) myJetTreeSim->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO_SIM));
  myJetTreeSim->doIterative(false);
  myJetTreeSim->doAreaSub(true);
  myJetTreeSim->doMultSub(false);
  myJetTreeSim->doTruth(false);
  myJetTreeSim->doData(false);
  myJetTreeSim->doEmbed(false);
  myJetTreeSim->doTowerECut(true);
  myJetTreeSim->setTowerThreshold(0.05);
  myJetTreeSim->setMinRecoPt(5.0); // only sets range for reco jets
  myJetTreeSim->setEtaRange(etamin, etamax);
  myJetTreeSim->setPtRange(10, 100); // only sets range for truth jets
  myJetTreeSim->Verbosity(verbosity);
  se->registerSubsystem(myJetTreeSim);

  //-----------------------------------
  // Input managers
  //-----------------------------------

  Fun4AllSyncManager *sync = se->getSyncManager();
  sync->MixRunsOk(true);

  Fun4AllInputManager *inData = new Fun4AllDstInputManager("DSTData","DST");
  inData->AddListFile(filelistdata);
  se->registerInputManager(inData);

  Fun4AllInputManager *inSim = new Fun4AllNoSyncDstInputManager("DSTSim","DST","TOPSim");
  inSim->AddListFile(filelistsim);
  se->registerInputManager(inSim);
  //-----------------------------------
  // Run the analysis
  //-----------------------------------
  
  se->run(100);
  se->End();

  gSystem->Exit(0);
  return 0;

}

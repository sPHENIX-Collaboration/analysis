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

  //-----------------------------------
  // Fun4All server initialization
  //-----------------------------------

  // create fun4all server
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 1;
  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();
  
  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2023");
  rc->set_uint64Flag("TIMESTAMP",23696);


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


  CaloTowerStatus *statusEMC = new CaloTowerStatus("CEMCSTATUS");
  statusEMC->set_detector_type(CaloTowerDefs::CEMC);
  statusEMC->set_time_cut(1);
  statusEMC->set_inputNodePrefix("TOWERINFO_CALIB_");
  se->registerSubsystem(statusEMC);

  CaloTowerStatus *statusIHCAL = new CaloTowerStatus("IHCALSTATUS");
  statusIHCAL->set_detector_type(CaloTowerDefs::HCALIN);
  statusIHCAL->set_time_cut(2);
  statusIHCAL->set_inputNodePrefix("TOWERINFO_CALIB_");
  se->registerSubsystem(statusIHCAL);


  CaloTowerStatus *statusOHCAL = new CaloTowerStatus("OHCALSTATUS");
  statusOHCAL->set_detector_type(CaloTowerDefs::HCALOUT);
  statusOHCAL->set_time_cut(2);
  statusOHCAL->set_inputNodePrefix("TOWERINFO_CALIB_");
  se->registerSubsystem(statusOHCAL);


  caloTowerEmbed *embedder_CEMC = new caloTowerEmbed("embedder_CEMC");
  embedder_CEMC->set_detector_type(CaloTowerDefs::CEMC);
  embedder_CEMC->set_inputNodePrefix("TOWERINFO_CALIB_");
  se->registerSubsystem(embedder_CEMC);

  caloTowerEmbed *embedder_IHCAL = new caloTowerEmbed("embedder_IHCAL");
  embedder_IHCAL->set_detector_type(CaloTowerDefs::HCALIN);
  embedder_IHCAL->set_inputNodePrefix("TOWERINFO_CALIB_");
  se->registerSubsystem(embedder_IHCAL);

  caloTowerEmbed *embedder_OHCAL = new caloTowerEmbed("embedder_OHCal");
  embedder_OHCAL->set_detector_type(CaloTowerDefs::HCALOUT);
  embedder_OHCAL->set_inputNodePrefix("TOWERINFO_CALIB_");
  se->registerSubsystem(embedder_OHCAL);


  
  // ==============
  // Jet Bkgd Sub
  // ==============
  double etamin = -1.1 + jet_parameter;
  double etamax = 1.1 - jet_parameter;
  //data
  JetBkgdSub *myJetTree = new JetBkgdSub(jet_parameter,outputbase + ".root");
  TowerJetInput *inputCEMC = new TowerJetInput(Jet::CEMC_TOWERINFO);
  inputCEMC->set_remove_bad_towers(true);
  myJetTree->add_input(inputCEMC);
  TowerJetInput *inputIHCAL = new TowerJetInput(Jet::HCALIN_TOWERINFO);
  inputIHCAL->set_remove_bad_towers(true);
  myJetTree->add_input(inputIHCAL);
  TowerJetInput *inputOHCAL = new TowerJetInput(Jet::HCALOUT_TOWERINFO);
  inputOHCAL->set_remove_bad_towers(true);
  myJetTree->add_input(inputOHCAL);
  myJetTree->doIterative(false);
  myJetTree->doAreaSub(true);
  myJetTree->doMultSub(false);
  myJetTree->doTruth(false);
  myJetTree->doData(true);
  myJetTree->doEmbed(false);
  myJetTree->setMinRecoPt(5.0); // only sets range for reco jets
  myJetTree->setEtaRange(etamin, etamax);
  myJetTree->setPtRange(10, 100); // only sets range for truth jets
  myJetTree->Verbosity(verbosity);
  se->registerSubsystem(myJetTree);


  //embed
  JetBkgdSub *myJetTreeEmbed = new JetBkgdSub(jet_parameter,outputbase + "_embed.root");
  TowerJetInput *inputCEMCEmbed = new TowerJetInput(Jet::CEMC_TOWERINFO_EMBED);
  inputCEMCEmbed->set_remove_bad_towers(true);
  myJetTreeEmbed->add_input(inputCEMCEmbed);
  TowerJetInput *inputIHCALEmbed = new TowerJetInput(Jet::HCALIN_TOWERINFO_EMBED);
  inputIHCALEmbed->set_remove_bad_towers(true);
  myJetTreeEmbed->add_input(inputIHCALEmbed);
  TowerJetInput *inputOHCALEmbed = new TowerJetInput(Jet::HCALOUT_TOWERINFO_EMBED);
  inputOHCAL->set_remove_bad_towers(true);
  myJetTreeEmbed->add_input(inputOHCALEmbed);
  myJetTreeEmbed->doIterative(false);
  myJetTreeEmbed->doAreaSub(true);
  myJetTreeEmbed->doMultSub(false);
  myJetTreeEmbed->doTruth(false);
  myJetTreeEmbed->doData(true);
  myJetTreeEmbed->doEmbed(true);
  myJetTreeEmbed->setMinRecoPt(5.0); // only sets range for reco jets
  myJetTreeEmbed->setEtaRange(etamin, etamax);
  myJetTreeEmbed->setPtRange(10, 100); // only sets range for truth jets
  myJetTreeEmbed->Verbosity(verbosity);
  se->registerSubsystem(myJetTreeEmbed);

  //sim
  JetBkgdSub *myJetTreeSim = new JetBkgdSub(jet_parameter,outputbase + "_sim.root");
  TowerJetInput *inputCEMCSim = new TowerJetInput(Jet::CEMC_TOWERINFO_SIM);
  inputCEMCSim->set_remove_bad_towers(true);
  myJetTreeSim->add_input(inputCEMCSim);
  TowerJetInput *inputIHCALSim = new TowerJetInput(Jet::HCALIN_TOWERINFO_SIM);
  inputIHCALSim->set_remove_bad_towers(true);
  myJetTreeSim->add_input(inputIHCALSim);
  TowerJetInput *inputOHCALSim = new TowerJetInput(Jet::HCALOUT_TOWERINFO_SIM);
  inputOHCALSim->set_remove_bad_towers(true);
  myJetTreeSim->add_input(inputOHCALSim);
  myJetTreeSim->doIterative(false);
  myJetTreeSim->doAreaSub(true);
  myJetTreeSim->doMultSub(false);
  myJetTreeSim->doTruth(false);
  myJetTreeSim->doData(false);
  myJetTreeSim->doEmbed(false);
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

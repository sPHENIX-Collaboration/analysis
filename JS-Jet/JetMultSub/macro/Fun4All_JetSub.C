#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4centrality/PHG4CentralityReco.h>

#include <HIJetReco.C>

#include <jetmultsub/JetMultSub.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libJetTree.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libJetMultSub.so)


#endif


void Fun4All_JetSub(const char *filelisttruth = "dst_truth_jet.list",
                    const char *filelistcalo = "dst_calo_cluster.list",
		    const char *filelistglobal = "dst_global.list")
{

  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );

  //-----------------------------------
  // Jet reco
  //-----------------------------------

  // kT jets (for mulitplicity rhos)
  JetReco *ktjetreco = new JetReco();
  ktjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_RETOWER));
  ktjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  ktjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  ktjetreco->add_algo(new FastJetAlgo(Jet::KT, 0.4), "Kt_Tower_r04");
  ktjetreco->set_algo_node("KT");
  ktjetreco->set_input_node("TOWER");
  ktjetreco->Verbosity(verbosity);
  se->registerSubsystem(ktjetreco);

  // // tower jets
  JetReco *towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_RETOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "AntiKt_Tower_r04");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);


  JetMultSub *jetmultsub = new JetMultSub();
  jetmultsub->add_reco_input("AntiKt_Tower_r04");
  jetmultsub->add_kt_input("Kt_Tower_r04");
  jetmultsub->set_output_name("AntiKt_Tower_r04_sub");
  jetmultsub->setEtaRange(-1.0, 1.0);
  jetmultsub->setPtRange(0.5,100);
  se->registerSubsystem(jetmultsub);

  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTtruth");
  intrue->AddListFile(filelisttruth,1);
  se->registerInputManager(intrue);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
  in2->AddListFile(filelistcalo,1);
  se->registerInputManager(in2);

  Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTglobal");
  in3->AddListFile(filelistglobal,1);
  se->registerInputManager(in3);
  
  se->run(-1);
  se->End();

  gSystem->Exit(0);
  return 0;

}

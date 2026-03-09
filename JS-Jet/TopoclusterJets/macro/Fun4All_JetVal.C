#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4centrality/PHG4CentralityReco.h>

#include "HIJetReco_softdrop.C"
#include <G4_TopoClusterReco.C>
#include <caloreco/RawClusterBuilderTopo.h>
#include <jetvalidation/JetValidationTC.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libJetValidationTC.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libg4dst.so)


#endif
void Fun4All_JetVal(const char *filelisttruth = "dst_truth_jet.list",
                    const char *filelistcalo = "dst_calo_cluster.list",
		    const char *filelistglobal = "dst_global.list",
		    const char *outname = "outputest.root", float minJetPt = 0.)
{

  std::string jetNode = "AntiKt_TopoCluster_Unsubtracted_r04";
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 5;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(verbosity);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );
  
  Enable::VERBOSITY = verbosity;
  
  
  RawClusterBuilderTopo* ClusterBuilder1 = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo1");
  ClusterBuilder1->Verbosity(verbosity);
  ClusterBuilder1->set_nodename("TOPOCLUSTER_EMCAL");
  ClusterBuilder1->set_enable_HCal(false);
  ClusterBuilder1->set_enable_EMCal(true);
  ClusterBuilder1->set_noise(0.02, 0.03, 0.03);
  ClusterBuilder1->set_significance(4.0, 2.0, 1.0);
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
  ClusterBuilder2->set_noise(0.01, 0.03, 0.03);
  ClusterBuilder2->set_significance(4.0, 2.0, 1.0);
  ClusterBuilder2->allow_corner_neighbor(true);
  ClusterBuilder2->set_do_split(true);
  ClusterBuilder2->set_minE_local_max(1.0, 2.0, 0.5);
  ClusterBuilder2->set_R_shower(0.025);
  se->registerSubsystem(ClusterBuilder2);
/*
  RawClusterBuilderTopo* ClusterBuilder3 = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo3");
  ClusterBuilder3->Verbosity(verbosity);
  ClusterBuilder3->set_nodename("TOPOCLUSTER_ALLCALO");
  ClusterBuilder3->set_enable_HCal(true);
  ClusterBuilder3->set_enable_EMCal(true);
  ClusterBuilder3->set_noise(0.01, 0.03, 0.03);
  ClusterBuilder3->set_significance(4.0, 2.0, 1.0);
  ClusterBuilder3->allow_corner_neighbor(true);
  ClusterBuilder3->set_do_split(true);
  ClusterBuilder3->set_minE_local_max(1.0, 2.0, 0.5);
  ClusterBuilder3->set_R_shower(0.025);
  se->registerSubsystem(ClusterBuilder3);*/

  //TopoClusterReco();

  HIJetReco_softdrop();

  JetValidationTC *myJetVal = new JetValidationTC(jetNode, "AntiKt_Truth_r04", outname);

  myJetVal->setPtRange(5, 100);
  myJetVal->setEtaRange(-0.7, 0.7);
  myJetVal->doUnsub(0);
  myJetVal->doTruth(0);
  myJetVal->doSeeds(1);
  myJetVal->doClusters(1);
  myJetVal->setJetPtThreshold(minJetPt);
  myJetVal->setMinClusterE(0.8);
  myJetVal->setMinClusterDeltaR(0.08);
  myJetVal->setMinPhotonProb(0.1);
  myJetVal->setMaxdR(0.4);
  myJetVal->setClusterType("CLUSTERINFO_POS_COR_CEMC");
  myJetVal->removeClustersInJets(1);
  se->registerSubsystem(myJetVal);

  //We need to remove all DST inputs other than DSTcalo for data
 
  /*
  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTtruth");
  intrue->AddListFile(filelisttruth,1);
  se->registerInputManager(intrue);*/

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
  in2->AddFile(filelistcalo);
  se->registerInputManager(in2);

  /*
  Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTglobal");
  in3->AddListFile(filelistglobal,1);
  se->registerInputManager(in3);*/
  
  //se->skip(1000);
  se->run(1200);
  se->End();

  gSystem->Exit(0);
  return 0;
}

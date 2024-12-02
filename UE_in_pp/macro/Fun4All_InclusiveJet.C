#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>

#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4centrality/PHG4CentralityReco.h>
#include <caloreco/RawClusterBuilderTopo.h>

//#include <HIJetReco.C>
#include "HIJetReco_nosub.C"
#include <InclusiveJet.h>

#include <g4mbd/MbdDigitization.h>
#include <mbd/MbdReco.h>
#include <globalvertex/GlobalVertexReco.h>

#include <stdio.h>
#include <string.h>

#include<Calo_Calib.C>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libInclusiveJet.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libg4mbd.so)
R__LOAD_LIBRARY(libmbd_io.so)
R__LOAD_LIBRARY(libmbd.so)

#endif
void Fun4All_InclusiveJet(int nEvents = 100, int seg = 0, int isSim = 0, const char *filelistcalo = "dst_calo_cluster.list", const char *sim_type = "run22_jet10")
{
  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  std::ifstream calolist;
  std::string caloline;
  std::string mcfilelistcalo;
  std::string filelisttruth;
  std::string filelistglobal;
  std::string filelisttruthparticle;

  int runnumber = 0;
  int segment = 0;

  if (!isSim) {
    calolist.open(filelistcalo);
    for (int i = 0; i < seg + 1; i++) 
    {
      getline(calolist, caloline);
    }
    std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(caloline);
    runnumber = runseg.first;
    segment = runseg.second;
  } else {
    std::ostringstream sseg;
    sseg << std::setw(4) << std::setfill('0') << seg;
    std::cout << sseg.str() << std::endl;
    if (!strcmp(sim_type,"run22_jet10")) {
      mcfilelistcalo = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_jet10_dst_calo_cluster_" + sseg.str() + ".list";
      filelisttruth = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_jet10_dst_truth_jet_" + sseg.str() + ".list";
      filelistglobal = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_jet10_dst_global_" + sseg.str() + ".list"; 
      filelisttruthparticle = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_jet10_dst_truthinfo_" + sseg.str() + ".list";
    } else if (!strcmp(sim_type,"run22_jet30")) {
      mcfilelistcalo = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_jet30_dst_calo_cluster_" + sseg.str() + ".list";
      filelisttruth = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_jet30_dst_truth_jet_" + sseg.str() + ".list";
      filelistglobal = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_jet30_dst_global_" + sseg.str() + ".list"; 
      filelisttruthparticle = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_jet30_dst_truthinfo_" + sseg.str() + ".list";
    } else if (!strcmp(sim_type,"run22_mb")) {
      mcfilelistcalo = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_mb_dst_calo_cluster_" + sseg.str() + ".list";
      filelisttruth = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_mb_dst_truth_jet_" + sseg.str() + ".list";
      filelistglobal = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_mb_dst_global_" + sseg.str() + ".list"; 
      filelisttruthparticle = "/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/mc_dst_files/run22_mb_dst_truthinfo_" + sseg.str() + ".list";
    } else {
      std::cout << "unknown simulation dst type" << std::endl;
    }
  }

  std::string outfilename;
  if (!isSim) {
    outfilename = "/sphenix/tg/tg01/jets/egm2153/UEinppOutput/output_" + to_string(runnumber) + "_" + to_string(segment) + ".root";
    //outfilename = "/sphenix/user/egm2153/calib_study/JetValidation/analysis/output_" + to_string(runnumber) + "_" + to_string(segment) + ".root";
  } else {
    if (!strcmp(sim_type,"run22_jet10")) {
      outfilename = "/sphenix/tg/tg01/jets/egm2153/UEinppOutput/sim_run22_jet10_output_" + to_string(seg) + ".root";
    } else if (!strcmp(sim_type,"run22_jet30")) {
      outfilename = "/sphenix/tg/tg01/jets/egm2153/UEinppOutput/sim_run22_jet30_output_" + to_string(seg) + ".root";
    } else if (!strcmp(sim_type,"run22_mb")) { 
      outfilename = "/sphenix/tg/tg01/jets/egm2153/UEinppOutput/sim_run22_mb_output_" + to_string(seg) + ".root";
    } else {
      std::cout << "unknown simulation dst type" << std::endl;
    }
  }
  
  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();
  if (!isSim) {
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    rc->set_uint64Flag("TIMESTAMP", runnumber);
  } else {
    rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
    rc->set_uint64Flag("TIMESTAMP", 6);
  }
    CDBInterface::instance()->Verbosity(1);
  
  MbdReco *mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  GlobalVertexReco *gvertex = new GlobalVertexReco();
  se->registerSubsystem(gvertex);

  if (!isSim) { Process_Calo_Calib(); }

  /*
  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(verbosity);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );
  */

  RawClusterBuilderTopo* ClusterBuilder = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo");
  ClusterBuilder->Verbosity(verbosity);
  ClusterBuilder->set_nodename("TOPOCLUSTER_ALLCALO");
  ClusterBuilder->set_enable_HCal(true);
  ClusterBuilder->set_enable_EMCal(true);
  ClusterBuilder->set_noise(0.0025, 0.006, 0.03);
  ClusterBuilder->set_significance(4.0, 2.0, 0.0);
  ClusterBuilder->allow_corner_neighbor(true);
  ClusterBuilder->set_do_split(true);
  ClusterBuilder->set_minE_local_max(1.0, 2.0, 0.5);
  ClusterBuilder->set_R_shower(0.025);
  ClusterBuilder->set_use_only_good_towers(true);
  ClusterBuilder->set_absE(true);
  se->registerSubsystem(ClusterBuilder);
  
  Enable::VERBOSITY = verbosity;
  HIJetReco();

  InclusiveJet *myJetVal = new InclusiveJet("AntiKt_Tower_r04_Sub1", "AntiKt_Truth_r04", outfilename.c_str());
  myJetVal->setPtRange(5, 100);
  myJetVal->setEtaRange(-0.7, 0.7);
  if (isSim) myJetVal->doTruth(1);
  myJetVal->doSeeds(0);
  myJetVal->doTowers(1);
  if (isSim) myJetVal->doTruthParticles(1);
  myJetVal->doTopoclusters(1);
  myJetVal->doEmcalClusters(0);
  se->registerSubsystem(myJetVal);

  if (!isSim) {
    Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
    in2->AddListFile(caloline,1);
    se->registerInputManager(in2);

    Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
    std::string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
    intrue2->AddFile(geoLocation);
    se->registerInputManager(intrue2);

  } else {
    Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
    in2->AddListFile(mcfilelistcalo,1);
    se->registerInputManager(in2);

    Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTtruth");
    intrue->AddListFile(filelisttruth,1);
    se->registerInputManager(intrue);

    Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTglobal");
    in3->AddListFile(filelistglobal,1);
    se->registerInputManager(in3);

    Fun4AllInputManager *in4 = new Fun4AllDstInputManager("DSTtruthparticle");
    in4->AddListFile(filelisttruthparticle,1);
    se->registerInputManager(in4);

    Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
    std::string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
    intrue2->AddFile(geoLocation);
    se->registerInputManager(intrue2);

  }
  
  se->run(nEvents);
  se->End();
  CDBInterface::instance()->Print();  // print used DB files
  gSystem->Exit(0);
  return 0;

}

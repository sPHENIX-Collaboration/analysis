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

#include <HIJetReco.C>
//#include "HIJetReco_nosub.C"
#include <JetValidation.h>

#include <g4mbd/MbdDigitization.h>
#include <mbd/MbdReco.h>
#include <globalvertex/GlobalVertexReco.h>

#include <stdio.h>
#include <string.h>

#include<Calo_Calib.C>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libJetValidation.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libg4mbd.so)
R__LOAD_LIBRARY(libmbd_io.so)
R__LOAD_LIBRARY(libmbd.so)

#endif
void Fun4All_JetVal(int nEvents = 100, int seg = 0, int isSim = 0, const char *filelistcalo = "dst_calo_cluster.list", const char *sim_type = "run15", const char *calo_type = "waveform")
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
    if (!strcmp(sim_type,"run11_nopileup")) {
      mcfilelistcalo = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/run11_dst_calo_cluster_" + sseg.str() + ".list";
      filelisttruth = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/run11_dst_truth_jet_" + sseg.str() + ".list";
      filelistglobal = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/run11_dst_global_" + sseg.str() + ".list"; 
      filelisttruthparticle = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/dst_truthinfo_" + sseg.str() + ".list";
    } else if (!strcmp(sim_type,"detroit")) {
      if (!strcmp(calo_type,"cluster")) {
        mcfilelistcalo = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/detroit_dst_calo_cluster_" + sseg.str() + ".list";
      } else if (!strcmp(calo_type,"nozero")) {
        mcfilelistcalo = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/detroit_dst_calo_nozero_" + sseg.str() + ".list";
      } else {
        mcfilelistcalo = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/detroit_dst_calo_waveform_" + sseg.str() + ".list";
      }
      filelisttruth = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/detroit_dst_truth_jet_" + sseg.str() + ".list";
      filelistglobal = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/detroit_dst_global_" + sseg.str() + ".list"; 
      filelisttruthparticle = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/detroit_dst_truthinfo_" + sseg.str() + ".list";
    } else if (!strcmp(sim_type,"detroit_jet10")) {
      mcfilelistcalo = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/detroit_jet10_dst_calo_waveform_" + sseg.str() + ".list";
      filelistglobal = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/detroit_jet10_g4hits_" + sseg.str() + ".list";
    } else {
      if (!strcmp(calo_type,"cluster")) {
        mcfilelistcalo = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/dst_calo_cluster_" + sseg.str() + ".list";
      } else if (!strcmp(calo_type,"nozero")) {
        mcfilelistcalo = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/dst_calo_nozero_" + sseg.str() + ".list";
      } else {
        mcfilelistcalo = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/dst_calo_waveform_" + sseg.str() + ".list";
      }
      filelisttruth = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/dst_truth_jet_" + sseg.str() + ".list";
      filelistglobal = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/dst_global_" + sseg.str() + ".list"; 
      filelisttruthparticle = "/sphenix/user/egm2153/calib_study/JetValidation/macro/dst_files/dst_truthinfo_" + sseg.str() + ".list";
    }
  }

  std::string outfilename;
  if (!isSim) {
    outfilename = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/output_" + to_string(runnumber) + "_" + to_string(segment) + ".root";
    //outfilename = "/sphenix/user/egm2153/calib_study/JetValidation/analysis/output_" + to_string(runnumber) + "_" + to_string(segment) + ".root";
  } else {
    if (!strcmp(sim_type,"run11_nopileup")) {
      outfilename = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/sim_run11_nopileup_calo_cluster_output_" + to_string(seg) + ".root";
    } else if (!strcmp(sim_type,"detroit")) {
      if (!strcmp(calo_type,"cluster")) {
        outfilename = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/sim_detroit_calo_cluster_output_" + to_string(seg) + ".root";
      } else if (!strcmp(calo_type,"nozero")) {
        outfilename = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/sim_detroit_calo_nozero_output_" + to_string(seg) + ".root";
      } else {
        outfilename = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/sim_detroit_output_" + to_string(seg) + ".root";
      }
    } else if (!strcmp(sim_type,"detroit_jet10")) { 
      outfilename = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/sim_detroit_jet10_output_" + to_string(seg) + ".root";
    } else {
      if (!strcmp(calo_type,"cluster")) {
        outfilename = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/sim_calo_cluster_output_" + to_string(seg) + ".root";
      } else if (!strcmp(calo_type,"nozero")) {
        outfilename = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/sim_calo_nozero_output_" + to_string(seg) + ".root";
      } else {
        outfilename = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/sim_output_" + to_string(seg) + ".root";
      }
    }
  }

  // default sim_type is default run 15 with 2Mhz pileup
  // default calo_type is calo_waveform dst file 
  
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
  
  MbdDigitization* mbddigi = new MbdDigitization();
  if (isSim && !strcmp(sim_type,"detroit_jet10")) {
    se->registerSubsystem(mbddigi);
  }
  
  MbdReco *mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  GlobalVertexReco *gvertex = new GlobalVertexReco();
  se->registerSubsystem(gvertex);

  if (!isSim) { Process_Calo_Calib(); }

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(verbosity);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );

  /*
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
  */
  
  Enable::VERBOSITY = verbosity;
  HIJetReco();

  JetValidation *myJetVal = new JetValidation("AntiKt_Tower_r04_Sub1", "AntiKt_Truth_r04", outfilename.c_str());
  myJetVal->setPtRange(7, 100);
  myJetVal->setEtaRange(-1.1, 1.1);
  myJetVal->doTruth(0);
  //if (isSim) myJetVal->doTruth(1);
  myJetVal->doSeeds(0);
  myJetVal->doTowers(1);
  //myJetVal->doTopoclusters(1);
  myJetVal->doEmcalClusters(1);
  if (isSim) myJetVal->doTruthParticles(1);
  se->registerSubsystem(myJetVal);

  if (!isSim) {
    Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
    in2->AddListFile(caloline,1);
    se->registerInputManager(in2);

    Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
    std::string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
    intrue2->AddFile(geoLocation);
    se->registerInputManager(intrue2);
    
  } else if (!strcmp(sim_type,"detroit_jet10")) {
    Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
    in2->AddListFile(mcfilelistcalo,1);
    se->registerInputManager(in2);

    Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTglobal");
    in3->AddListFile(filelistglobal,1);
    se->registerInputManager(in3);

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
  se->PrintTimer();
  gSystem->Exit(0);
  return 0;

}

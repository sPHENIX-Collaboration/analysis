#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <GlobalVariables.C>
#include <G4_Input.C>         // global variable "Input"
#include <G4_TrkrVariables.C> // global variable "Input"
#include <G4_ActsGeom.C>      // global variable "ACTSGEOM"
#include <G4_Global.C>
#include <Trkr_Reco.C>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllUtils.h>

#include <siliconseedsana/SiliconCaloMatching.h>
#include <siliconseedsana/SiliconSeedsAna.h>
#include <siliconseedsana/ChargeRecalc.h>

#include <calotrigger/TriggerRunInfoReco.h>
#include <mbd/MbdReco.h>
#include <caloreco/RawClusterBuilderTopo.h>

#include <phool/recoConsts.h>

#include <sys/stat.h>
#include <limits.h> // PATH_MAX
#include <unistd.h>

#include "Calo_Calib.C"

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wundefined-internal"

#pragma GCC diagnostic pop

//--R__LOAD_LIBRARY(libkfparticle_sphenix.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
//--R__LOAD_LIBRARY(libffamodules.so)
//--R__LOAD_LIBRARY(libcdbobjects.so)
//--R__LOAD_LIBRARY(libmvtx.so)
//--R__LOAD_LIBRARY(libintt.so)
//--R__LOAD_LIBRARY(libtpc.so)
//--R__LOAD_LIBRARY(libmicromegas.so)
// R__LOAD_LIBRARY(libTrackingDiagnostics.so)
// R__LOAD_LIBRARY(libtrackingqa.so)
// R__LOAD_LIBRARY(libtpcqa.so)
// R__LOAD_LIBRARY(libtrack_to_calo.so)
//--R__LOAD_LIBRARY(libtrack_reco.so)
//--R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libcalotrigger.so)
// R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libepd.so)
R__LOAD_LIBRARY(libzdcinfo.so)

R__LOAD_LIBRARY(libsiliconseedsana.so)

bool useTopologicalCluster = false;

int Fun4All_DataDST_SiliconSeedAna_run3pp(
    const int nEvents = 1000
    , const string inlst_dst_strk = "list/test/run53879_seed_0.list"
    , const string inlst_dst_calo = "list/test/run53879_calo_0.list"
    , const string out_root = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/run24pp/ana/newgeo/calocalib/recalc_charge/ana_53879_1000evt.root"
    , const int startnumber = 0)
{

  std::cout << "inlst_dst_trk  : " << inlst_dst_strk << endl;
  std::cout << "inlst_dst_calo : " << inlst_dst_calo << endl;
  // std::cout << "std::cout << "Input DST: " << in_dst      <<std::endl;
  // std::cout << "out  ROOT: " << out_root    <<std::endl;
  std::cout << "Nevent   : " << nEvents << std::endl;
  std::cout << "#start   : " << startnumber << std::endl;

  // gSystem->ListLibraries();
    

  std::ifstream fin(inlst_dst_strk.c_str());
  std::string filename;
  fin>>filename;
  std::cout<<"Filename : "<<filename<<std::endl;
  fin.close();

  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filename);
  int runnumber = runseg.first;
  int segment = runseg.second;
    
    
    

  G4TRACKING::SC_CALIBMODE = false;
  
  //Enable::MVTX_APPLYMISALIGNMENT = true;
  //ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;
  //
  TRACKING::pp_mode = true;

  Fun4AllServer *se = Fun4AllServer::instance();
  // se->Verbosity(10);

  recoConsts *rc = recoConsts::instance();
  Input::VERBOSITY = 0;

  //int runnum = 79516;
  std::cout<<"Run : "<<runnumber<<std::endl;
  // Tracking setup
  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");
  Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo);

  // this assume Geometory data is in the DST
  ACTSGEOM::ActsGeomInit();

  //--input
  Fun4AllInputManager *in_calo = new Fun4AllDstInputManager("DSTCalo");
  in_calo->AddListFile(inlst_dst_calo.c_str());
  // in->Verbosity(2);
  se->registerInputManager(in_calo);

  //--Fun4AllInputManager *in_clus = new Fun4AllDstInputManager("DSTTrkrClus");
  //--in_clus->AddListFile(inlst_dst_clus.c_str());
  //--// in->Verbosity(2);
  //--se->registerInputManager(in_clus);

  Fun4AllInputManager *in_strk = new Fun4AllDstInputManager("DSTTrkrTrack");
  in_strk->AddListFile(inlst_dst_strk.c_str());
  // in->Verbosity(2);
  se->registerInputManager(in_strk);

///////////////////////////////
  TriggerRunInfoReco *triggerruninforeco = new TriggerRunInfoReco();
  se->registerSubsystem(triggerruninforeco);

  // MBD/BBC Reconstruction
  MbdReco *mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

///////////////////////////////

  //Process_Calo_Calib_Mahirochan();
  Process_Calo_Calib();

  //////////////////////////////
  //TopoClusterReco(); // too slow
  //topo clusters for EMCal and HCal (layer 0 - iHCal, layer 1 - oHCal, layer 2 - EMCal)
  RawClusterBuilderTopo* ClusterBuilder2 = new RawClusterBuilderTopo("SiHCalRawClusterBuilderTopo");
  //ClusterBuilder2->Verbosity(11);
  ClusterBuilder2->set_nodename("TOPOCLUSTER_ALLCALO");
  ClusterBuilder2->set_enable_HCal(true);
  ClusterBuilder2->set_enable_EMCal(true);
  //ClusterBuilder2->set_noise(0.0025, 0.006, 0.03);
  //ClusterBuilder2->set_noise(0.01, 0.03, 0.03);
  ClusterBuilder2->set_noise(0.002, 0.01, 0.03); //From Emma (1 sigma)
  //ClusterBuilder2->set_noise(0.004, 0.02, 0.06); //From Emma (2 sigma)
  // ClusterBuilder2->set_noise(0.006, 0.03, 0.09); //From Emma (3 sigma)
  ClusterBuilder2->set_significance(4.0, 2.0, 1.0); // seed threshold, grow threshold, perimeter threshold in unit of noise sigma
  ClusterBuilder2->allow_corner_neighbor(true);
  ClusterBuilder2->set_do_split(true);
  ClusterBuilder2->set_minE_local_max(1.0, 2.0, 0.5);
  ClusterBuilder2->set_R_shower(0.025);
  //ClusterBuilder2->set_use_only_good_towers(false);
  se->registerSubsystem(ClusterBuilder2);

  //////////////////////////////


  auto converter = new TrackSeedTrackMapConverter;
  // SiliconTrackSeedContainer or TpcTrackSeedContainer
  converter->setTrackSeedName("SiliconTrackSeedContainer");
  converter->setClusterMapName("TRKR_CLUSTER_SEED");
  converter->setFieldMap(G4MAGNET::magfield_tracking);
  // converter->Verbosity(10);
  se->registerSubsystem(converter);

  PHSimpleVertexFinder *finder = new PHSimpleVertexFinder;
  finder->Verbosity(0);
  finder->setTrkrClusterContainerName("TRKR_CLUSTER_SEED");
  finder->setDcaCut(0.1);
  finder->setTrackPtCut(0.);
  finder->setBeamLineCut(1);
  finder->setTrackQualityCut(20);
  finder->setNmvtxRequired(3);
  finder->setOutlierPairCut(0.1);
  se->registerSubsystem(finder);

  Global_Reco();

  // --- charge recalculation (before projection) ---
  auto chrecalc = new ChargeRecalc("ChargeRecalc");
  chrecalc->setVerbosity(0);
  chrecalc->setClusterContainerName("TRKR_CLUSTER_SEED");
  se->registerSubsystem(chrecalc);

  // track projection CALO
  auto projection = new PHActsTrackProjection("CaloProjection");
  // float emcal_radius = 104.5; // set by my self due to emcal geometry was cahnged
  // projection->setLayerRadius(SvtxTrack::CEMC, emcal_radius);
  se->registerSubsystem(projection);



  // Si-Calo Matching
  SiliconCaloMatching* sicalomatch = new SiliconCaloMatching();
  //sicalomatch->Verbosity(1);
  sicalomatch->setClusterContainerName("TRKR_CLUSTER_SEED");
  sicalomatch->setEMCalClusterContainerName("CLUSTERINFO_CEMC");
  sicalomatch->setEmcalLowEcut(0.2);
  sicalomatch->setTrackLowpTcut(0.2);
  se->registerSubsystem(sicalomatch);

  auto siana = new SiliconSeedsAna("SiliconSeedsAna");
  // siana->setMC(true);
  siana->setVtxSkip(true);
  siana->setClusterContainerName("TRKR_CLUSTER_SEED");
  siana->setEMCalClusterContainerName("CLUSTERINFO_CEMC");
  siana->setTopoCluster(useTopologicalCluster);
  // siana->setEMcalRadius(emcal_radius); // set new emcal radius for silicon track production to emcal
  siana->setOutputFileName(out_root.c_str());

  siana->setStartEventNumber(startnumber);

  se->registerSubsystem(siana);

  se->run(nEvents);

  //-----
  // Exit
  //-----

  //  CDBInterface::instance()->Print(); // print used DB files
  se->End();
  std::cout << "All done" << std::endl;
  delete se;

  gSystem->Exit(0);
  return 0;
}
#endif

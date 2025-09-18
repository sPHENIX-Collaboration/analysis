#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <GlobalVariables.C>
#include <G4_Input.C> // global variable "Input"
#include <G4_TrkrVariables.C> // global variable "Input"
#include <G4_ActsGeom.C> // global variable "ACTSGEOM"

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

#include <phool/recoConsts.h>


#include <sys/stat.h>
#include <limits.h>     // PATH_MAX
#include <unistd.h>


#include <siliconseedsana/SiliconSeedsAna.h>

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
//R__LOAD_LIBRARY(libTrackingDiagnostics.so)
//R__LOAD_LIBRARY(libtrackingqa.so)
//R__LOAD_LIBRARY(libtpcqa.so)
//R__LOAD_LIBRARY(libtrack_to_calo.so)
//--R__LOAD_LIBRARY(libtrack_reco.so)
//--R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libcalotrigger.so)
//R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libepd.so)
R__LOAD_LIBRARY(libzdcinfo.so)

R__LOAD_LIBRARY(libsiliconseedsana.so)


bool useTopologicalCluster = false;

int Fun4All_DataDST_SiliconSeedAna(
    const int nEvents = 2000
  , const string inlst_dst_clus = "run53879_clus_0.list"
//  , const string inlst_dst_strk = "run53879_strk_0.list"
  , const string inlst_dst_strk = "run53879_seed_0.list"
  , const string inlst_dst_calo = "run53879_calo_0.list"
  , const string out_root = "test_ana.root"
  , const int startnumber = 0
)
{

  std::cout << "inlst_dst_clus : "<<inlst_dst_clus<<endl;
  std::cout << "inlst_dst_trk  : "<<inlst_dst_strk<<endl;
  std::cout << "inlst_dst_calo : "<<inlst_dst_calo<<endl;
  //std::cout << "std::cout << "Input DST: " << in_dst      <<std::endl;
  //std::cout << "out  ROOT: " << out_root    <<std::endl;
  std::cout << "Nevent   : " << nEvents     <<std::endl;
  std::cout << "#start   : " << startnumber <<std::endl;

  //gSystem->ListLibraries();

  G4TRACKING::SC_CALIBMODE = false;
  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;
  TRACKING::pp_mode = true;


  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(10);


  recoConsts *rc = recoConsts::instance();
  Input::VERBOSITY = 0;

  int runnum = 53879;
  // Tracking setup
  Enable::CDB = true;
  //rc->set_StringFlag("CDB_GLOBALTAG", "Prod_2024A");
  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
  rc->set_uint64Flag("TIMESTAMP", runnum);

  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");
  Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo);


  // this assume Geometory data is in the DST
  ACTSGEOM::ActsGeomInit();


  //--input
  Fun4AllInputManager *in_calo = new Fun4AllDstInputManager("DSTCalo");
  in_calo->AddListFile(inlst_dst_calo.c_str());
  //in->Verbosity(2);
  se->registerInputManager(in_calo);

  Fun4AllInputManager *in_clus = new Fun4AllDstInputManager("DSTTrkrClus");
  in_clus->AddListFile(inlst_dst_clus.c_str());
  //in->Verbosity(2);
  se->registerInputManager(in_clus);

  Fun4AllInputManager *in_strk = new Fun4AllDstInputManager("DSTTrkrTrack");
  in_strk->AddListFile(inlst_dst_strk.c_str());
  //in->Verbosity(2);
  se->registerInputManager(in_strk);


  auto converter = new TrackSeedTrackMapConverter;
  //SiliconTrackSeedContainer or TpcTrackSeedContainer
  converter->setTrackSeedName("SiliconTrackSeedContainer");
  converter->setFieldMap(G4MAGNET::magfield_tracking);
  //converter->Verbosity(10);
  se->registerSubsystem(converter);

  PHSimpleVertexFinder *finder = new PHSimpleVertexFinder;
  finder->Verbosity(0);
  finder->setDcaCut(0.1);
  finder->setTrackPtCut(0.);
  finder->setBeamLineCut(1);
  finder->setTrackQualityCut(20);
  finder->setNmvtxRequired(3);
  finder->setOutlierPairCut(0.1);
  se->registerSubsystem(finder);


  auto projection = new PHActsTrackProjection("CaloProjection");
  se->registerSubsystem(projection);
  
  

  auto siana = new SiliconSeedsAna("SiliconSeedsAna");
  //siana->setMC(true);
  siana->setVtxSkip(true);
  siana->setEMCalClusterContainerName("CLUSTERINFO_CEMC");
  siana->setTopoCluster(useTopologicalCluster);
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

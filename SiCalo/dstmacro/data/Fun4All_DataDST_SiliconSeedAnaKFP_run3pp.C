#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <GlobalVariables.C>
#include <G4_Input.C>         // global variable "Input"
#include <G4_TrkrVariables.C> // global variable "Input"
#include <G4_ActsGeom.C>      // global variable "ACTSGEOM"
#include <G4_Global.C>
#include <G4_TopoClusterReco.C>
#include <Trkr_RecoInit.C>
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

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <siliconseedsana/SiliconCaloMatching.h>
#include <siliconseedsana/SiliconSeedsAna.h>
#include <siliconseedsana/ChargeRecalc.h>
#include <siliconseedsana/KFParticle_nTupleSCIF.h>

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

int Fun4All_DataDST_SiliconSeedAnaKFP_run3pp(
    const int nEvents = 1000
    , const string inlst_dst_strk = "list/test/run53879_seed_0.list"
    , const string inlst_dst_calo = "list/test/run53879_calo_0.list"
    , const string out_root = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/run24pp/ana/newgeo/calocalib/recalc_charge/ana_53879_1000evt.root"
    , const string out_KFProot = "KFP_0.root"
    , const int startnumber = 0
    , bool withtopo=false
    )
{

  std::cout << "inlst_dst_trk  : " << inlst_dst_strk << endl;
  std::cout << "inlst_dst_calo : " << inlst_dst_calo << endl;
  // std::cout << "std::cout << "Input DST: " << in_dst      <<std::endl;
  // std::cout << "out  ROOT: " << out_root    <<std::endl;
  std::cout << "Nevent   : " << nEvents << std::endl;
  std::cout << "#start   : " << startnumber << std::endl;

  bool doEMCalRadiusCorr=true;
  bool doSiCalopTUpdate=false;

  // gSystem->ListLibraries();
  //
    

  std::ifstream fin(inlst_dst_strk.c_str());
  std::string filename;
  fin>>filename;
  std::cout<<"Filename : "<<filename<<std::endl;
  fin.close();

  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filename);
  int runnumber = runseg.first;
  int segment = runseg.second;
    
    
    

  G4TRACKING::SC_CALIBMODE = false;
  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;

  //TRACKING::pp_mode = true;
  TRACKING::streaming_mode = true;

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

  //--std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");
  //--Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  //--ingeo->AddFile(geofile);
  //--se->registerInputManager(ingeo);

  //--// this assume Geometory data is in the DST
  //--ACTSGEOM::ActsGeomInit();
  TrackingInit();

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
  if(withtopo){
    TopoClusterReco(); // too slow
    //--//topo clusters for EMCal and HCal (layer 0 - iHCal, layer 1 - oHCal, layer 2 - EMCal)
    //--RawClusterBuilderTopo* ClusterBuilder2 = new RawClusterBuilderTopo("SiHCalRawClusterBuilderTopo");
    //--//ClusterBuilder2->Verbosity(11);
    //--ClusterBuilder2->set_nodename("TOPOCLUSTER_ALLCALO");
    //--ClusterBuilder2->set_enable_HCal(true);
    //--ClusterBuilder2->set_enable_EMCal(true);
    //--//ClusterBuilder2->set_noise(0.0025, 0.006, 0.03);
    //--//ClusterBuilder2->set_noise(0.01, 0.03, 0.03);
    //--ClusterBuilder2->set_noise(0.002, 0.01, 0.03); //From Emma (1 sigma)
    //--//ClusterBuilder2->set_noise(0.004, 0.02, 0.06); //From Emma (2 sigma)
    //--// ClusterBuilder2->set_noise(0.006, 0.03, 0.09); //From Emma (3 sigma)
    //--ClusterBuilder2->set_significance(4.0, 2.0, 1.0); // seed threshold, grow threshold, perimeter threshold in unit of noise sigma
    //--ClusterBuilder2->allow_corner_neighbor(true);
    //--ClusterBuilder2->set_do_split(true);
    //--ClusterBuilder2->set_minE_local_max(1.0, 2.0, 0.5);
    //--ClusterBuilder2->set_R_shower(0.025);
    //--//ClusterBuilder2->set_use_only_good_towers(false);
    //--se->registerSubsystem(ClusterBuilder2);
  }

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
  float new_cemc_rad=104.0;
  if (doEMCalRadiusCorr) 
  {
    projection->setLayerRadius(SvtxTrack::CEMC, new_cemc_rad);
  }            
  se->registerSubsystem(projection);

  // Si-Calo Matching
  SiliconCaloMatching* sicalomatch = new SiliconCaloMatching();
  //sicalomatch->Verbosity(1);
  sicalomatch->setClusterContainerName("TRKR_CLUSTER_SEED");
  sicalomatch->setEMCalClusterContainerName("CLUSTERINFO_CEMC");
  sicalomatch->setEmcalLowEcut(0.2);
  //--sicalomatch->setTrackLowpTcut(0.2);
  if (doEMCalRadiusCorr ) {
    sicalomatch->setEmcalRadius(new_cemc_rad);
    sicalomatch->setEmcalInOutFrac(1); // inner EMCAL radius
  }                                                         
  sicalomatch->setUpdate_pTinSvtxTrack(doSiCalopTUpdate);  
  se->registerSubsystem(sicalomatch);

  //  Create KFP object:
  { // originally Ishigaki-san's setting
    KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX("KFParticle_Jpsi");
    //kfparticle->Verbosity(12);
  
    kfparticle->setDecayDescriptor("J/psi -> e^+ e^-");
    kfparticle->setContainerName("KFParticle_Jpsi");
    //kfparticle->setTrackMapNodeName("SvtxTrackMap");
    //--kfparticle->useFakePrimaryVertex(true);
  
  
    //  Apply selections we set in namespace HeavyFlavorReco
    //Selection -- cuts, settings
    // kfparticle->usePID(use_pid);
    // kfparticle->setPIDacceptFraction(pid_frac);
    kfparticle->dontUseGlobalVertex(true);
    // kfparticle->requireTrackVertexBunchCrossingMatch(require_track_and_vertex_match);
    kfparticle->getAllPVInfo(false);
    kfparticle->allowZeroMassTracks();
    // kfparticle->use2Dmatching(use_2D_matching);
    kfparticle->getTriggerInfo(false);
    kfparticle->getDetectorInfo(true);
    kfparticle->saveDST(true);
    kfparticle->magFieldFile("FIELDMAP_TRACKING");
  
  
    kfparticle->extrapolateTracksToSV(true);
  
    // 🌸 Get truth information! (Only able to be used on truth/simulation DSTs)
    kfparticle->doTruthMatching(false);
  
    // 🌸 Cuts you can change:
    //PV to SV cuts
    kfparticle->constrainToPrimaryVertex(true);//cameron rewrite
    // // kfparticle->useFakePrimaryVertex(true);//cameron rewrite
    // kfparticle->setMinimumRadialSV(0.5);
    // kfparticle->setMotherIPchi2(100);
    // kfparticle->setFlightDistancechi2(-1);
    // kfparticle->setMinDIRA(0.95);
    // kfparticle->setDecayLengthRange(0.1, FLT_MAX);
    // kfparticle->setDecayLengthRange_XY(-10000, FLT_MAX);
    // kfparticle->setDecayTimeRange_XY(-10000, FLT_MAX);
    // kfparticle->setDecayTimeRange(-10000, FLT_MAX);
    // kfparticle->setMinDecayTimeSignificance(-1e5);
    // kfparticle->setMinDecayLengthSignificance(-1e5);
    // kfparticle->setMinDecayLengthSignificance_XY(-1e5);
  
    //Track parameters
    kfparticle->bunchCrossingZeroOnly(true);
    kfparticle->setMinimumTrackPT(0.3);
    // kfparticle->setMinimumTrackIPchi2_XY(-1.);
    // kfparticle->setMinimumTrackIP_XY(-100.);
    // kfparticle->setMaximumTrackchi2nDOF(100.);
    kfparticle->setMinTPChits(0);
    kfparticle->setMinMVTXhits(3);
    kfparticle->setMinINTThits(1);
    //Vertex parameters
    // kfparticle->setMaximumVertexchi2nDOF(20);
    kfparticle->setMaximumDaughterDCA(1.0); //5 mm
    kfparticle->setMaximumDaughterDCA_XY(100); //5 mm
  
    //Parent parameters
    kfparticle->setMotherPT(0);
    kfparticle->setMinimumMass(0.0001); //Check mass ranges
    kfparticle->setMaximumMass(12.00);
    kfparticle->setMaximumMotherVertexVolume(0.1); // ???
  
    // TPC based caloinfo
    kfparticle->getCaloInfo(true);
    if (doEMCalRadiusCorr ) {
  	  kfparticle->set_emcal_radius_user(new_cemc_rad);
    }
  
    // Sets where KFP output file will be stored and what it is called:
    //kfparticle->setOutputName("KFPout.root");
    kfparticle->setOutputName(out_KFProot.c_str());
  
    se->registerSubsystem(kfparticle);
  }
   
  auto siana = new SiliconSeedsAna("SiliconSeedsAna");
  // siana->setMC(true);
  siana->setVtxSkip(true);
  siana->setClusterContainerName("TRKR_CLUSTER_SEED");
  siana->setEMCalClusterContainerName("CLUSTERINFO_CEMC");
  siana->setTopoCluster(useTopologicalCluster);
  // siana->setEMcalRadius(emcal_radius); // set new emcal radius for silicon track production to emcal
  siana->setOutputFileName(out_root.c_str());

  siana->setStartEventNumber(startnumber);

  siana->getKFParticleNtuple()->setNumberOfTracks(2); // Ndaughters
  siana->getKFParticleNtuple()->constrainToPrimaryVertex(true);
  //siana->getKFParticleNtuple()->useSCIF(true);
  //siana->getKFParticleNtuple()->getCaloInfo(true);
  //--siana->getKFParticleNtuple()->GetDetailedCalorimetry(true);
  if ( doEMCalRadiusCorr ) {
	  siana->setEMcalRadius(new_cemc_rad);
  }
  

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

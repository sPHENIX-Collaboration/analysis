#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllServer.h>
#include <GlobalVariables.C>
#include <G4_ActsGeom.C>
#include <G4_Global.C>
#include <G4_Magnet.C>
#include <G4_Mbd.C>
#include <Trkr_Clustering.C>
#include <Trkr_LaserClustering.C>
#include <Trkr_RecoInit.C>
#include <Trkr_TpcReadoutInit.C>

#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

#include <centralmembranestripematching/TpcCentralMembraneStripeMatching.h>

#include <fstream>
#include <iostream>
#include <string>

#include <TString.h>
#include <TSystem.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libtpc.so)
R__LOAD_LIBRARY(libcentralmembranestripematching.so)

void Fun4All_CentralMembraneStripeMatching(
    const int nEvents = -1,
    const std::string filelist = "stripelists_measured/run79516.list",
    const std::string outdir = "output/",
    const std::string referenceStripeFile = "referencestripes/run83319.root")
{
  auto se = Fun4AllServer::instance();
  se->Verbosity(0);

  TRACKING::tpc_zero_supp = true;
  G4TPC::ENABLE_CENTRAL_MEMBRANE_CLUSTERING = true;
  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;

  auto rc = recoConsts::instance();
  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");

  std::ifstream inputList(filelist);
  std::string firstInputFile;
  std::getline(inputList, firstInputFile);
  if (firstInputFile.empty())
  {
    std::cout << "Fun4All_CentralMembraneStripeMatching - empty input file list: " << filelist << std::endl;
    gSystem->Exit(1);
  }

  const auto runSegment = Fun4AllUtils::GetRunSegment(firstInputFile);
  const int runnumber = runSegment.first;
  rc->set_IntFlag("RUNNUMBER", runnumber);
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  auto hitsin = new Fun4AllDstInputManager("InputManager");
  hitsin->AddListFile(filelist);
  se->registerInputManager(hitsin);

  const std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");
  auto ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo);

  const std::string staticCorrectionFile = "/sphenix/user/dloomis/Distortions/DistortionMaps_Static/static_only_inverted_10-new_hugoupdate.root";

  G4TPC::ENABLE_MODULE_EDGE_CORRECTIONS = true;
  G4TPC::ENABLE_STATIC_CORRECTIONS = true;
  G4TPC::static_correction_filename = staticCorrectionFile;
  G4TPC::USE_PHI_AS_RAD_STATIC_CORRECTIONS = false;
  G4TPC::ENABLE_AVERAGE_CORRECTIONS = false;
  G4TPC::laser_adc_threshold = 100;

  TrackingInit();
  ACTSGEOM::ActsGeomInit();

  auto matcher = new TpcCentralMembraneStripeMatching;
  matcher->setReferenceStripeFile(referenceStripeFile);
  matcher->setStaticCorrectionFile(staticCorrectionFile);
  matcher->setOutputfile(Form("%s/CMDistortionCorrections-%08d_reference83319.root", outdir.c_str(), runnumber));
  se->registerSubsystem(matcher);

  se->run(nEvents);
  se->End();
  se->PrintTimer();

  delete se;
  std::cout << "Finished" << std::endl;
  gSystem->Exit(0);
}

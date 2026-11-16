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

#include <TFile.h>
#include <TH2.h>
#include <TString.h>
#include <TSystem.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libtpc.so)
R__LOAD_LIBRARY(libcentralmembranestripematching.so)

void Fun4All_CentralMembraneReferenceHistograms(
    const int nEvents = -1,
    const std::string filelist = "stripelists_reference/run75073.list",
    const std::string outputfile = "referencestripes/run75073.root")
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
    std::cout << "Fun4All_CentralMembraneReferenceHistograms - empty input file list: " << filelist << std::endl;
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

  G4TPC::ENABLE_MODULE_EDGE_CORRECTIONS = true;
  G4TPC::ENABLE_STATIC_CORRECTIONS = false;
  G4TPC::static_correction_filename = "/sphenix/user/dloomis/Distortions/DistortionMaps_Static/static_only_inverted_10-new_hugoupdate.root";
  G4TPC::USE_PHI_AS_RAD_STATIC_CORRECTIONS = false;
  G4TPC::ENABLE_AVERAGE_CORRECTIONS = false;
  G4TPC::laser_adc_threshold = 100;

  TrackingInit();
  ACTSGEOM::ActsGeomInit();

  auto matcher = new TpcCentralMembraneStripeMatching("TpcCentralMembraneReferenceHistograms");
  matcher->setFillReferenceHistogramsOnly();
  se->registerSubsystem(matcher);

  se->run(nEvents);
  se->End();

  auto *hSouth = matcher->cloneReferenceHistogram(0, "hPetal_South");
  auto *hNorth = matcher->cloneReferenceHistogram(1, "hPetal_North");
  if (!hSouth || !hNorth)
  {
    std::cout << "Fun4All_CentralMembraneReferenceHistograms - failed to generate reference histograms" << std::endl;
    gSystem->Exit(1);
  }

  TFile *outfile = new TFile(outputfile.c_str(), "RECREATE");
  outfile->cd();
  hSouth->Write("", TObject::kOverwrite);
  hNorth->Write("", TObject::kOverwrite);
  outfile->Close();

  se->PrintTimer();

  delete hSouth;
  delete hNorth;
  delete se;
  std::cout << "Finished reference histogram file: " << outputfile << std::endl;
  gSystem->Exit(0);
}

#include <fun4all/Fun4AllUtils.h>
#include <G4_ActsGeom.C>
#include <G4_Magnet.C>
#include <GlobalVariables.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>

#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

#include <cdbobjects/CDBTTree.h>

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <stdio.h>

#include "HF_selections_QM25.C"

using namespace HeavyFlavorReco;

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)

void Fun4All_HF_QM25(
    const int nEvents = 100,
    const std::string inputDST = "DST_TRKR_TRACKS_run2pp_ana475_2024p018_v001-00053877-00000.root",
    const std::string inputDir = "/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana475_2024p018_v001/DST_TRKR_TRACKS/run_00053800_00053900/dst/",
    const int nSkip = 0)
{
  std::string inputTrackFile = inputDir + inputDST;

  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(inputDST);
  int runnumber = runseg.first;
  std::stringstream nice_runnumber;
  nice_runnumber << std::setw(8) << std::setfill('0') << to_string(runnumber);

  int rounded_up = 100*(std::ceil((float) runnumber/100));
  std::stringstream nice_rounded_up;
  nice_rounded_up << std::setw(8) << std::setfill('0') << to_string(rounded_up);

  int rounded_down = 100*(std::floor((float) runnumber/100));
  std::stringstream nice_rounded_down;
  nice_rounded_down << std::setw(8) << std::setfill('0') << to_string(rounded_down);

  int segment = runseg.second;
  std::stringstream nice_segment;
  nice_segment << std::setw(5) << std::setfill('0') << to_string(segment);

  std::stringstream nice_skip;
  nice_skip << std::setw(5) << std::setfill('0') << to_string(nSkip);

  auto rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER", runnumber);

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

  output_dir = "./"; //Top dir of where the output nTuples will be written
  trailer = "_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + ".root";

  if (run_pipi_reco) create_hf_directories(pipi_reconstruction_name, pipi_output_dir, pipi_output_reco_file);
  if (run_Kpi_reco) create_hf_directories(Kpi_reconstruction_name, Kpi_output_dir, Kpi_output_reco_file);
  if (run_Kpi_reco_likeSign) create_hf_directories(Kpi_like_reconstruction_name, Kpi_like_output_dir, Kpi_like_output_reco_file);
  if (run_Kpipi_reco) create_hf_directories(Kpipi_reconstruction_name, Kpipi_output_dir, Kpipi_output_reco_file);
  if (run_Kpipi_reco_likeSign) create_hf_directories(Kpipi_like_reconstruction_name, Kpipi_like_output_dir, Kpipi_like_output_reco_file);
  if (run_KKpi_reco) create_hf_directories(KKpi_reconstruction_name, KKpi_output_dir, KKpi_output_reco_file);
  if (run_pKpi_reco) create_hf_directories(pKpi_reconstruction_name, pKpi_output_dir, pKpi_output_reco_file);
  if (run_pKpi_reco_likeSign) create_hf_directories(pKpi_like_reconstruction_name, pKpi_like_output_dir, pKpi_like_output_reco_file);
  if (run_Kstar_reco) create_hf_directories(Kstar_reconstruction_name, Kstar_output_dir, Kstar_output_reco_file);
  if (run_KK_reco) create_hf_directories(KK_reconstruction_name, KK_output_dir, KK_output_reco_file);
  if (run_Lambdapi_reco) create_hf_directories(Lambdapi_reconstruction_name, Lambdapi_output_dir, Lambdapi_output_reco_file);

  auto se = Fun4AllServer::instance();
  se->Verbosity(1);

  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;

  Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo);

  TrackingInit();

  auto hitsintrack = new Fun4AllDstInputManager("TrackInputManager");
  hitsintrack->fileopen(inputTrackFile);
  se->registerInputManager(hitsintrack);

  if (get_trigger_info)
  {
    std::string gl1_file = "/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana464_nocdbtag_v001/DST_STREAMING_EVENT_INTT0/run_"
                         + nice_rounded_down.str() + "_" + nice_rounded_up.str()
                         + "/dst/DST_STREAMING_EVENT_INTT0_run2pp_ana464_nocdbtag_v001-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";

    auto hitsintrig = new Fun4AllDstInputManager("TriggerInputManager");
    hitsintrig->fileopen(gl1_file);
    se->registerInputManager(hitsintrig);
  }

  if (get_dEdx_info || get_detector_info)
  {
    std::string clus_file = "/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana466_2024p012_v001/DST_TRKR_CLUSTER/run_"
                          + nice_rounded_down.str() + "_" + nice_rounded_up.str()
                          + "/dst/DST_TRKR_CLUSTER_run2pp_ana466_2024p012_v001-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";

    auto hitsinclus = new Fun4AllDstInputManager("ClusterInputManager");
    hitsinclus->fileopen(clus_file);
    se->registerInputManager(hitsinclus);
  }

  if (run_pipi_reco || run_Kpi_reco || run_Kpipi_reco || run_KKpi_reco || run_pKpi_reco || run_Lambdapi_reco) init_kfp_dependencies();

  if (run_pipi_reco) reconstruct_pipi_mass();
  if (run_Kpi_reco) reconstruct_Kpi_mass();
  if (run_Kpi_reco_likeSign) reconstruct_Kpi_mass_likeSign();
  if (run_Kpipi_reco) reconstruct_Kpipi_mass();
  if (run_Kpipi_reco_likeSign) reconstruct_Kpipi_mass_likeSign();
  if (run_KKpi_reco) reconstruct_KKpi_mass();
  if (run_pKpi_reco) reconstruct_pKpi_mass();
  if (run_pKpi_reco_likeSign) reconstruct_pKpi_mass_likeSign();
  if (run_Kstar_reco) reconstruct_Kstar_mass();
  if (run_KK_reco) reconstruct_KK_mass();
  if (run_Lambdapi_reco) reconstruct_Lambdapi_mass();

  se->skip(nSkip);
  se->run(nEvents);
  se->End();
  se->PrintTimer();

  if (run_pipi_reco) end_kfparticle(pipi_output_reco_file, pipi_output_dir);
  if (run_Kpi_reco) end_kfparticle(Kpi_output_reco_file, Kpi_output_dir);
  if (run_Kpi_reco_likeSign) end_kfparticle(Kpi_like_output_reco_file, Kpi_like_output_dir);
  if (run_Kpipi_reco) end_kfparticle(Kpipi_output_reco_file, Kpipi_output_dir);
  if (run_Kpipi_reco_likeSign) end_kfparticle(Kpipi_like_output_reco_file, Kpipi_like_output_dir);
  if (run_KKpi_reco) end_kfparticle(KKpi_output_reco_file, KKpi_output_dir);
  if (run_pKpi_reco) end_kfparticle(pKpi_output_reco_file, pKpi_output_dir);
  if (run_pKpi_reco_likeSign) end_kfparticle(pKpi_like_output_reco_file, pKpi_like_output_dir);
  if (run_Kstar_reco) end_kfparticle(Kstar_output_reco_file, Kstar_output_dir);
  if (run_KK_reco) end_kfparticle(KK_output_reco_file, KK_output_dir);
  if (run_Lambdapi_reco) end_kfparticle(Lambdapi_output_reco_file, Lambdapi_output_dir);

  delete se;

  std::cout << "Finished" << std::endl;
  gSystem->Exit(0);
}

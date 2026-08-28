#include <fun4all/Fun4AllServer.h>

#include <calotrigger/TriggerRunInfoReco.h>
#include <globalvertex/GlobalVertexReco.h>

#include <kfparticleqa/QAKFParticle.h>
#include <kfparticleqa/QAKFParticleTrackPtAsymmetry.h>

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wundefined-internal"

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#pragma GCC diagnostic pop

R__LOAD_LIBRARY(libkfparticle_sphenix.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libkfparticleqa.so)

namespace HeavyFlavorReco
{
  int VERBOSITY_HF = 0;

  bool run_pipi_reco = true;
  bool run_ppi_reco = true; // set to true if needed
  bool run_KK_reco = false; // set to true if needed
  bool run_Kpi_reco = false; // set to true if needed

  std::string output_dir = "./"; //Top dir of where the output nTuples will be written
  std::string kfp_header = "outputKFParticle_";
  std::string processing_folder = "inReconstruction/";
  std::string trailer = ".root";

  std::string trackMapName = "SvtxTrackMap";

  // https://wiki.bnl.gov/sPHENIX/index.php/KFParticle
  std::string pipi_decay_descriptor = "K_S0 -> pi^+ pi^-"; //See twiki on how to set this
  std::string pipi_reconstruction_name = "pipi_reco"; //Used for naming output folder, file and node
  std::string pipi_output_reco_file;
  std::string pipi_output_dir;

  std::string ppi_decay_descriptor = "[Lambda0 -> proton^+ pi^-]cc"; //See twiki on how to set this
  std::string ppi_reconstruction_name = "ppi_reco"; //Used for naming output folder, file and node
  std::string ppi_output_reco_file;
  std::string ppi_output_dir;

  std::string KK_decay_descriptor = "phi -> K^+ K^-"; //See twiki on how to set this
  std::string KK_reconstruction_name = "KK_reco"; //Used for naming output folder, file and node
  std::string KK_output_reco_file;
  std::string KK_output_dir;

  std::string Kpi_decay_descriptor = "[D0 -> K^- pi^+]cc"; //See twiki on how to set this
  std::string Kpi_reconstruction_name = "Kpi_reco"; //Used for naming output folder, file and node
  std::string Kpi_output_reco_file;
  std::string Kpi_output_dir;

  bool save_kfpntuple = true;
  bool use_pid = false; //was false
  bool save_tracks_to_DST = false;
  bool dont_use_global_vertex = true;
  bool use_fake_pv = false;
  bool require_track_and_vertex_match = false;
  bool save_all_vtx_info = true;
  bool constrain_phi_mass = true;
  bool use_2D_matching = false;
  bool get_trigger_info = false;
  bool get_detector_info = false; //was true
  bool get_dEdx_info = true; //was false
  float pid_frac = 0.6;
  bool constrain_lambda_mass = true;

  //Used
  float lf_cuts_setMinDIRA{0.9};
  float lf_cuts_setDecayLengthRange_min{0.};
  float lf_cuts_setMinimumTrackIP_XY{-1.};
  float lf_cuts_setMinMVTXhits{0};
  float lf_cuts_setMinINTThits{0};
  float lf_cuts_setMinTPChits{20};
  float lf_cuts_setMaximumVertexchi2nDOF{FLT_MAX};
  float lf_cuts_setMaximumDaughterDCA{1.5};

  float lf_cuts_setMaximumTrackPTchi2{1.}; // track pt must not be completely uncertain

  // restrict SV radius to (outside beampipe and within the first half of the TPC)
  float lf_cuts_setMinimumRadialSV{2.5};
  float lf_cuts_setMaximumRadialSV{40.};
 
  float lf_cuts_setMotherIPchi2{FLT_MAX};
  float lf_cuts_setFlightDistancechi2{-1.};
  float lf_cuts_setMinDIRA_XY{0.};
  float lf_cuts_setDecayLengthRange_max{FLT_MAX};
  float lf_cuts_setDecayLengthRange_XY_min{0.};
  float lf_cuts_setDecayLengthRange_XY_max{FLT_MAX};
  float lf_cuts_setDecayTimeRange_XY_min{0.};
  float lf_cuts_setDecayTimeRange_XY_max{FLT_MAX};
  float lf_cuts_setDecayTimeRange_min{0.};
  float lf_cuts_setDecayTimeRange_max{FLT_MAX};
  float lf_cuts_setMinDecayTimeSignificance{-1e5};
  float lf_cuts_setMinDecayLengthSignificance{-1e5};
  float lf_cuts_setMinDecayLengthSignificance_XY{-1e5};
  float lf_cuts_setMinimumTrackPT{0.0};
  float lf_cuts_setMinimumTrackIPchi2{-1.};
  float lf_cuts_setMinimumTrackIPchi2_XY{-1.};
  float lf_cuts_setMinimumTrackIP{-1.};
  float lf_cuts_setMaximumTrackchi2nDOF{1000.};
  float lf_cuts_setMaximumDaughterDCA_XY{1.5};
  float lf_cuts_setMotherPT{0};
  float lf_cuts_setMaximumMotherVertexVolume{10000.};
};  // namespace HeavyFlavorReco'

using namespace HeavyFlavorReco;

void init_kfp_dependencies()
{
  //dE/dx needs TRKR_CLUSTER and CYLINDERCELLGEOM_SVTX which need to be in the DST or loaded from a geo file
  Fun4AllServer *se = Fun4AllServer::instance();

  GlobalVertexReco* gblvertex = new GlobalVertexReco();
  gblvertex->Verbosity(VERBOSITY_HF);
  se->registerSubsystem(gblvertex);


  if (get_trigger_info)
  {
    TriggerRunInfoReco *triggerruninforeco = new TriggerRunInfoReco();
    se->registerSubsystem(triggerruninforeco);
  }
}

void create_hf_directories(std::string reconstruction_name, std::string &final_output_dir, std::string &output_reco_file)
{
  std::string output_file_name = kfp_header + reconstruction_name + trailer;
  final_output_dir = output_dir + reconstruction_name + "/";
  std::string output_reco_dir = final_output_dir + processing_folder;
  output_reco_file = output_reco_dir + output_file_name;

  std::string makeDirectory = "mkdir -p " + output_reco_dir;
  system(makeDirectory.c_str());
}

void reconstruct_pipi_mass()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(pipi_reconstruction_name);
  kfparticle->Verbosity(VERBOSITY_HF);

  kfparticle->setTrackMapNodeName(trackMapName);

  kfparticle->setDecayDescriptor(pipi_decay_descriptor);

  kfparticle->saveOutput(save_kfpntuple);

  kfparticle->usePID(use_pid);
  kfparticle->setPIDacceptFraction(pid_frac);
  kfparticle->get_dEdx_info();
  kfparticle->useFakePrimaryVertex(use_fake_pv);
  kfparticle->dontUseGlobalVertex(dont_use_global_vertex);
  kfparticle->requireTrackVertexBunchCrossingMatch(require_track_and_vertex_match);
  kfparticle->getAllPVInfo(save_all_vtx_info);
  kfparticle->allowZeroMassTracks();
  kfparticle->use2Dmatching(use_2D_matching);
  kfparticle->getTriggerInfo(get_trigger_info);
  kfparticle->getDetectorInfo(get_detector_info);
  kfparticle->saveDST(save_tracks_to_DST);
  kfparticle->setContainerName(pipi_reconstruction_name);
  kfparticle->saveParticleContainer(true);
  kfparticle->magFieldFile("FIELDMAP_TRACKING");

  //PV to SV cuts
  kfparticle->constrainToPrimaryVertex(true);
  kfparticle->setMotherPV_DCA_StdDev(lf_cuts_setMotherIPchi2);
  kfparticle->setFlightDistancechi2(lf_cuts_setFlightDistancechi2);
  kfparticle->setMinDIRA(lf_cuts_setMinDIRA);
  kfparticle->setMinDIRA_XY(lf_cuts_setMinDIRA_XY);
  kfparticle->setDecayLengthRange(lf_cuts_setDecayLengthRange_min, lf_cuts_setDecayLengthRange_max);
  kfparticle->setDecayLengthRange_XY(lf_cuts_setDecayLengthRange_XY_min, lf_cuts_setDecayLengthRange_XY_max);
  kfparticle->setDecayTimeRange_XY(lf_cuts_setDecayTimeRange_XY_min, lf_cuts_setDecayTimeRange_XY_max);
  kfparticle->setDecayTimeRange(lf_cuts_setDecayTimeRange_min, lf_cuts_setDecayTimeRange_max);
  kfparticle->setMinDecayTimeSignificance(lf_cuts_setMinDecayTimeSignificance);
  kfparticle->setMinDecayLengthSignificance(lf_cuts_setMinDecayLengthSignificance);
  kfparticle->setMinDecayLengthSignificance_XY(lf_cuts_setMinDecayLengthSignificance_XY);

  kfparticle->setMinimumRadialSV(lf_cuts_setMinimumRadialSV);
  kfparticle->setMaximumRadialSV(lf_cuts_setMaximumRadialSV);

  //Track parameters
  kfparticle->setMinimumTrackPT(lf_cuts_setMinimumTrackPT);
  kfparticle->setMaximumTrackPTchi2(lf_cuts_setMaximumTrackPTchi2);
  kfparticle->setMinimumTrackPV_DCA_StdDev(lf_cuts_setMinimumTrackIPchi2);
  kfparticle->setMinimumTrackPV_DCA_StdDev_XY(lf_cuts_setMinimumTrackIPchi2_XY);
  kfparticle->setMinimumTrackPV_DCA(lf_cuts_setMinimumTrackIP);
  kfparticle->setMinimumTrackPV_DCA_XY(lf_cuts_setMinimumTrackIP_XY);
  kfparticle->setMaximumTrackchi2nDOF(lf_cuts_setMaximumTrackchi2nDOF);
  kfparticle->setMinMVTXhits(lf_cuts_setMinMVTXhits);
  kfparticle->setMinINTThits(lf_cuts_setMinINTThits);
  kfparticle->setMinTPChits(lf_cuts_setMinTPChits);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(lf_cuts_setMaximumVertexchi2nDOF);
  kfparticle->setMaximumDaughterDCA(lf_cuts_setMaximumDaughterDCA);
  kfparticle->setMaximumDaughterDCA_XY(lf_cuts_setMaximumDaughterDCA_XY);

  //Parent parameters
  kfparticle->setMotherPT(lf_cuts_setMotherPT);
  kfparticle->setMinimumMass(0.40);
  kfparticle->setMaximumMass(0.60);
  kfparticle->setMaximumMotherVertexVolume(lf_cuts_setMaximumMotherVertexVolume);
  kfparticle->setOutputName(pipi_output_reco_file);

  se->registerSubsystem(kfparticle);

  //QAKFParticle *kfpqa = new QAKFParticle("QAKFParticle_K_S0","K_S0",0.2,0.7);
  //kfpqa->setKFParticleNodeName(pipi_reconstruction_name);
  //kfpqa->enableTrackPtAsymmetry(true); 
  //kfpqa->Verbosity(VERBOSITY_HF);
  //se->registerSubsystem(kfpqa);

}

void reconstruct_KK_mass()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(KK_reconstruction_name);
  kfparticle->Verbosity(VERBOSITY_HF);

  kfparticle->setDecayDescriptor(KK_decay_descriptor);
  kfparticle->saveOutput(save_kfpntuple);

  kfparticle->setTrackMapNodeName(trackMapName);

  kfparticle->usePID(true); //use_pid
  kfparticle->setPIDacceptFraction(pid_frac);
  kfparticle->get_dEdx_info();
  kfparticle->useFakePrimaryVertex(use_fake_pv);
  kfparticle->dontUseGlobalVertex(dont_use_global_vertex);
  kfparticle->requireTrackVertexBunchCrossingMatch(require_track_and_vertex_match);
  kfparticle->getAllPVInfo(save_all_vtx_info);
  kfparticle->allowZeroMassTracks();
  kfparticle->use2Dmatching(use_2D_matching);
  kfparticle->getTriggerInfo(get_trigger_info);
  kfparticle->getDetectorInfo(get_detector_info);
  kfparticle->saveDST(save_tracks_to_DST);
  kfparticle->setContainerName(KK_reconstruction_name);
  kfparticle->saveParticleContainer(true);
  kfparticle->magFieldFile("FIELDMAP_TRACKING");

  //PV to SV cuts
  kfparticle->constrainToPrimaryVertex(true);
  kfparticle->setMotherPV_DCA_StdDev(FLT_MAX);
  kfparticle->setFlightDistancechi2(-1.);

  //Track parameters
  kfparticle->setMinimumTrackPT(0.1);
  kfparticle->setMaximumTrackPT(0.7);
  kfparticle->setMaximumTrackchi2nDOF(FLT_MAX);
  kfparticle->setMinTPChits(25);
  kfparticle->setMinMVTXhits(0);
  kfparticle->setMinINTThits(0);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(FLT_MAX);
  kfparticle->setMaximumDaughterDCA(FLT_MAX);
  kfparticle->setMaximumDaughterDCA_XY(FLT_MAX);

  //Parent parameters
  kfparticle->setMotherPT(0);
  kfparticle->setMinimumMass(0.98);
  kfparticle->setMaximumMass(1.1);
  kfparticle->setMaximumMotherVertexVolume(FLT_MAX);

  kfparticle->setOutputName(KK_output_reco_file);

  se->registerSubsystem(kfparticle);

  //QAKFParticle *kfpqa = new QAKFParticle("QAKFParticle_phi","phi",0.98,1.1);
  //kfpqa->setKFParticleNodeName(KK_reconstruction_name);
  //se->registerSubsystem(kfpqa);
}

void reconstruct_ppi_mass()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(ppi_reconstruction_name);
  kfparticle->Verbosity(VERBOSITY_HF);

  kfparticle->setDecayDescriptor(ppi_decay_descriptor);
  kfparticle->saveOutput(save_kfpntuple);

  kfparticle->setTrackMapNodeName(trackMapName);

  kfparticle->usePID(use_pid);
  kfparticle->setPIDacceptFraction(pid_frac);
  kfparticle->get_dEdx_info();
  kfparticle->useFakePrimaryVertex(use_fake_pv);
  kfparticle->dontUseGlobalVertex(dont_use_global_vertex);
  kfparticle->requireTrackVertexBunchCrossingMatch(require_track_and_vertex_match);
  kfparticle->getAllPVInfo(save_all_vtx_info);
  kfparticle->allowZeroMassTracks();
  kfparticle->use2Dmatching(use_2D_matching);
  kfparticle->getTriggerInfo(get_trigger_info);
  kfparticle->getDetectorInfo(get_detector_info);
  kfparticle->saveDST(save_tracks_to_DST);
  kfparticle->setContainerName(ppi_reconstruction_name);
  kfparticle->saveParticleContainer(true);
  kfparticle->magFieldFile("FIELDMAP_TRACKING");

  //PV to SV cuts
  kfparticle->constrainToPrimaryVertex(true);
  kfparticle->setMotherPV_DCA_StdDev(lf_cuts_setMotherIPchi2);
  kfparticle->setFlightDistancechi2(lf_cuts_setFlightDistancechi2);
  kfparticle->setMinDIRA(lf_cuts_setMinDIRA);
  kfparticle->setMinDIRA_XY(lf_cuts_setMinDIRA_XY);
  kfparticle->setDecayLengthRange(lf_cuts_setDecayLengthRange_min, lf_cuts_setDecayLengthRange_max);
  kfparticle->setDecayLengthRange_XY(lf_cuts_setDecayLengthRange_XY_min, lf_cuts_setDecayLengthRange_XY_max);
  kfparticle->setDecayTimeRange_XY(lf_cuts_setDecayTimeRange_XY_min, lf_cuts_setDecayTimeRange_XY_max);
  kfparticle->setDecayTimeRange(lf_cuts_setDecayTimeRange_min, lf_cuts_setDecayTimeRange_max);
  kfparticle->setMinDecayTimeSignificance(lf_cuts_setMinDecayTimeSignificance);
  kfparticle->setMinDecayLengthSignificance(lf_cuts_setMinDecayLengthSignificance);
  kfparticle->setMinDecayLengthSignificance_XY(lf_cuts_setMinDecayLengthSignificance_XY);

  kfparticle->setMinimumRadialSV(lf_cuts_setMinimumRadialSV);
  kfparticle->setMaximumRadialSV(lf_cuts_setMaximumRadialSV);

  //Track parameters
  kfparticle->setMinimumTrackPT(lf_cuts_setMinimumTrackPT);
  kfparticle->setMaximumTrackPTchi2(lf_cuts_setMaximumTrackPTchi2);
  kfparticle->setMinimumTrackPV_DCA_StdDev(lf_cuts_setMinimumTrackIPchi2);
  kfparticle->setMinimumTrackPV_DCA_StdDev_XY(lf_cuts_setMinimumTrackIPchi2_XY);
  kfparticle->setMinimumTrackPV_DCA(lf_cuts_setMinimumTrackIP);
  kfparticle->setMinimumTrackPV_DCA_XY(lf_cuts_setMinimumTrackIP_XY);
  kfparticle->setMaximumTrackchi2nDOF(lf_cuts_setMaximumTrackchi2nDOF);
  kfparticle->setMinMVTXhits(lf_cuts_setMinMVTXhits);
  kfparticle->setMinINTThits(lf_cuts_setMinINTThits);
  kfparticle->setMinTPChits(lf_cuts_setMinTPChits);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(lf_cuts_setMaximumVertexchi2nDOF);
  kfparticle->setMaximumDaughterDCA(lf_cuts_setMaximumDaughterDCA);
  kfparticle->setMaximumDaughterDCA_XY(lf_cuts_setMaximumDaughterDCA_XY);

  //Parent parameters
  kfparticle->setMotherPT(lf_cuts_setMotherPT);
  kfparticle->setMinimumMass(1.08);
  kfparticle->setMaximumMass(1.15);
  kfparticle->setMaximumMotherVertexVolume(lf_cuts_setMaximumMotherVertexVolume);
  kfparticle->setOutputName(ppi_output_reco_file);

  se->registerSubsystem(kfparticle);

  //QAKFParticle *kfpqa = new QAKFParticle("QAKFParticle_Lambda0","Lambda0",0.88,1.35);
  //kfpqa->setKFParticleNodeName(ppi_reconstruction_name);
  //se->registerSubsystem(kfpqa);
}

void reconstruct_Kpi_mass()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(Kpi_reconstruction_name);
  kfparticle->Verbosity(VERBOSITY_HF);

  kfparticle->setDecayDescriptor(Kpi_decay_descriptor);
  kfparticle->saveOutput(save_kfpntuple);

  kfparticle->setTrackMapNodeName(trackMapName);

  kfparticle->usePID(false);
  kfparticle->setPIDacceptFraction(pid_frac);
  kfparticle->get_dEdx_info();
  kfparticle->useFakePrimaryVertex(use_fake_pv);
  kfparticle->dontUseGlobalVertex(dont_use_global_vertex);
  kfparticle->requireTrackVertexBunchCrossingMatch(require_track_and_vertex_match);
  kfparticle->getAllPVInfo(save_all_vtx_info);
  kfparticle->allowZeroMassTracks();
  kfparticle->use2Dmatching(use_2D_matching);
  kfparticle->getTriggerInfo(get_trigger_info);
  kfparticle->getDetectorInfo(get_detector_info);
  kfparticle->saveDST(save_tracks_to_DST);
  kfparticle->setContainerName(Kpi_reconstruction_name);
  kfparticle->saveParticleContainer(true);
  kfparticle->magFieldFile("FIELDMAP_TRACKING");

  //PV to SV cuts
  kfparticle->constrainToPrimaryVertex(true);
  //kfparticle->setMotherPV_DCA_StdDev(100);
  kfparticle->setMotherPV_DCA_StdDev(FLT_MAX);
  kfparticle->setFlightDistancechi2(-1.);
  kfparticle->setMinDIRA(-1.);
  kfparticle->setMinDIRA_XY(-1.1);
  kfparticle->setDecayLengthRange(-10., FLT_MAX); //was -10. then 0.006
  kfparticle->setDecayLengthRange_XY(-10., FLT_MAX);
  kfparticle->setDecayTimeRange_XY(-10000, FLT_MAX);
  kfparticle->setDecayTimeRange(-10000, FLT_MAX);
  kfparticle->setMinDecayTimeSignificance(-1e5);
  kfparticle->setMinDecayLengthSignificance(-1e5);
  kfparticle->setMinDecayLengthSignificance_XY(-1e5);

  kfparticle->setMinimumRadialSV(0.5);

  //Track parameters
  kfparticle->setMinimumTrackPT(0.0);
  kfparticle->setMinimumTrackPV_DCA_StdDev(-1.);
  kfparticle->setMinimumTrackPV_DCA_StdDev_XY(-1.);
  kfparticle->setMinimumTrackPV_DCA(-1.);
  kfparticle->setMinimumTrackPV_DCA_XY(-1.);//0.005
  kfparticle->setMaximumTrackchi2nDOF(FLT_MAX); //300
  kfparticle->setMinMVTXhits(0);
  kfparticle->setMinINTThits(0);
  kfparticle->setMinTPChits(20);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(FLT_MAX); //20
  kfparticle->setMaximumDaughterDCA(0.5);
  kfparticle->setMaximumDaughterDCA_XY(0.5);

  //Parent parameters
  kfparticle->setMotherPT(0);
  kfparticle->setMinimumMass(1.75);
  kfparticle->setMaximumMass(1.95);
  kfparticle->setMaximumMotherVertexVolume(FLT_MAX);
  kfparticle->setOutputName(Kpi_output_reco_file);

  se->registerSubsystem(kfparticle);

  //QAKFParticle *kfpqa = new QAKFParticle("QAKFParticle_D0","D0",1.75,1.95);
  //kfpqa->setKFParticleNodeName(Kpi_reconstruction_name);
  //se->registerSubsystem(kfpqa);
}

void end_kfparticle(std::string full_file_name, std::string final_path)
{
  ifstream file(full_file_name.c_str());
  if (file.good())
  {
    string moveOutput = "mv " + full_file_name + " " + final_path;
    system(moveOutput.c_str());
  }
}

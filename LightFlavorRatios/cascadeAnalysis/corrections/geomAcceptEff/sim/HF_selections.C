#include <fun4all/Fun4AllServer.h>

#include <calotrigger/TriggerRunInfoReco.h>
#include <globalvertex/GlobalVertexReco.h>

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

  bool run_pipi_reco = false;
  bool run_ppi_reco = false; // set to true if needed
  bool run_anti_ppi_reco = false; // set to true if needed
  bool run_KK_reco = false; // set to true if needed
  bool run_Kpi_reco = false; // set to true if needed
  bool run_ee_reco = false; // set to true if needed
  bool run_cascade_reco = false; // set to true if needed

  std::string output_dir = "./"; //Top dir of where the output nTuples will be written
  std::string kfp_header = "outputKFParticle_";
  std::string processing_folder = "inReconstruction/";
  std::string trailer = ".root";

  // https://wiki.bnl.gov/sPHENIX/index.php/KFParticle
  std::string pipi_decay_descriptor = "K_S0 -> pi^+ pi^-"; //See twiki on how to set this
  std::string pipi_reconstruction_name = "pipi_reco"; //Used for naming output folder, file and node
  std::string pipi_output_reco_file;
  std::string pipi_output_dir;

  std::string ppi_decay_descriptor = "[Lambda0 -> proton^+ pi^-]cc"; //See twiki on how to set this
  //std::string ppi_decay_descriptor = "Lambda0 -> proton^+ pi^-"; //See twiki on how to set this
  std::string ppi_reconstruction_name = "ppi_reco"; //Used for naming output folder, file and node
  std::string ppi_output_reco_file;
  std::string ppi_output_dir;

  std::string anti_ppi_decay_descriptor = "Lambda0 -> proton^- pi^+"; //See twiki on how to set this
  std::string anti_ppi_reconstruction_name = "anti_ppi_reco"; //Used for naming output folder, file and node
  std::string anti_ppi_output_reco_file;
  std::string anti_ppi_output_dir;

  std::string cascade_decay_descriptor = "[Xi- -> {Lambda0 -> proton^+ pi^-} pi^-]cc"; //See twiki on how to set this
  std::string cascade_reconstruction_name = "cascade_reco"; //Used for naming output folder, file and node
  std::string cascade_output_reco_file;
  std::string cascade_output_dir;

  bool truthMatch = true;
  bool save_kfpntuple = true;
  bool use_pid = false;
  bool save_tracks_to_DST = false;
  bool dont_use_global_vertex = true;
  bool require_track_and_vertex_match = true;
  bool save_all_vtx_info = true;
  bool constrain_phi_mass = true;
  bool use_2D_matching = false;
  bool get_trigger_info = false;
  bool get_detector_info = true;
  bool get_dEdx_info = false;
  bool get_calo_info = false;
  float pid_frac = 0.4;
  float cemc_proj_radius = 102.9; //Virgile recommendation according to DetailedCalorimeterGeometry
  bool constrain_lambda_mass = true;
  bool extraolate_tracks_to_secondary_vertex = false; // Set to false to ensure the pT map is accurate for asymmetry study

  //Used
  float lf_cuts_setMinDIRA{-1.1};
  float lf_cuts_setDecayLengthRange_min{-1.};
  float lf_cuts_setMinimumTrackIP_XY{-1.};
  float lf_cuts_setMinMVTXhits{0};
  float lf_cuts_setMinINTThits{0};
  float lf_cuts_setMinTPChits{0};
  float lf_cuts_setMaximumVertexchi2nDOF{2000};
  float lf_cuts_setMaximumDaughterDCA{10};

  //Unused
  float lf_cuts_setMotherIPchi2{FLT_MAX};
  float lf_cuts_setFlightDistancechi2{-1.};
  float lf_cuts_setMinDIRA_XY{-1.1};
  float lf_cuts_setDecayLengthRange_max{FLT_MAX};
  float lf_cuts_setDecayLengthRange_XY_min{-10.};
  float lf_cuts_setDecayLengthRange_XY_max{FLT_MAX};
  float lf_cuts_setDecayTimeRange_XY_min{-10000};
  float lf_cuts_setDecayTimeRange_XY_max{FLT_MAX};
  float lf_cuts_setDecayTimeRange_min{-10000};
  float lf_cuts_setDecayTimeRange_max{FLT_MAX};
  float lf_cuts_setMinDecayTimeSignificance{-1e5};
  float lf_cuts_setMinDecayLengthSignificance{-1e5};
  float lf_cuts_setMinDecayLengthSignificance_XY{-1e5};
  float lf_cuts_setMinimumTrackPT{0.0};
  float lf_cuts_setMinimumTrackIPchi2{-1.};
  float lf_cuts_setMinimumTrackIPchi2_XY{-1.};
  float lf_cuts_setMinimumTrackIP{-1.};
  float lf_cuts_setMaximumTrackchi2nDOF{3000.};
  float lf_cuts_setMaximumDaughterDCA_XY{10};
  float lf_cuts_setMotherPT{0};
  float lf_cuts_setMaximumMotherVertexVolume{100};
/*
  //Used
  float lf_cuts_setMinDIRA{0.99};
  float lf_cuts_setDecayLengthRange_min{0.05};
  float lf_cuts_setMinimumTrackIP_XY{0.05};
  float lf_cuts_setMinMVTXhits{1};
  float lf_cuts_setMinINTThits{1};
  float lf_cuts_setMinTPChits{20};
  float lf_cuts_setMaximumVertexchi2nDOF{20};
  float lf_cuts_setMaximumDaughterDCA{0.5};

  //Unused
  float lf_cuts_setMotherIPchi2{FLT_MAX};
  float lf_cuts_setFlightDistancechi2{-1.};
  float lf_cuts_setMinDIRA_XY{-1.1};
  float lf_cuts_setDecayLengthRange_max{FLT_MAX};
  float lf_cuts_setDecayLengthRange_XY_min{-10.};
  float lf_cuts_setDecayLengthRange_XY_max{FLT_MAX};
  float lf_cuts_setDecayTimeRange_XY_min{-10000};
  float lf_cuts_setDecayTimeRange_XY_max{FLT_MAX};
  float lf_cuts_setDecayTimeRange_min{-10000};
  float lf_cuts_setDecayTimeRange_max{FLT_MAX};
  float lf_cuts_setMinDecayTimeSignificance{-1e5};
  float lf_cuts_setMinDecayLengthSignificance{-1e5};
  float lf_cuts_setMinDecayLengthSignificance_XY{-1e5};
  float lf_cuts_setMinimumTrackPT{0.0};
  float lf_cuts_setMinimumTrackIPchi2{-1.};
  float lf_cuts_setMinimumTrackIPchi2_XY{-1.};
  float lf_cuts_setMinimumTrackIP{-1.};
  float lf_cuts_setMaximumTrackchi2nDOF{300.};
  float lf_cuts_setMaximumDaughterDCA_XY{1};
  float lf_cuts_setMotherPT{0};
  float lf_cuts_setMaximumMotherVertexVolume{0.1};
*/
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

void create_hf_directories(std::string reconstruction_name, std::string &final_output_dir, std::string &output_reco_file, std::string ID)
{
  std::string output_file_name = kfp_header + reconstruction_name + "_" + ID + trailer;
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

  kfparticle->setDecayDescriptor(pipi_decay_descriptor);

  kfparticle->setTrackMapNodeName("HFSelected_SvtxTrackMap");
  kfparticle->extraolateTracksToSV(extraolate_tracks_to_secondary_vertex);
  kfparticle->saveOutput(save_kfpntuple);

  kfparticle->doTruthMatching(truthMatch);
  kfparticle->usePID(use_pid);
  kfparticle->setPIDacceptFraction(pid_frac);
  kfparticle->get_dEdx_info();
  kfparticle->dontUseGlobalVertex(dont_use_global_vertex);
  kfparticle->requireTrackVertexBunchCrossingMatch(require_track_and_vertex_match);
  kfparticle->getAllPVInfo(save_all_vtx_info);
  kfparticle->allowZeroMassTracks();
  kfparticle->use2Dmatching(use_2D_matching);
  kfparticle->getTriggerInfo(get_trigger_info);
  kfparticle->getDetectorInfo(get_detector_info);
  kfparticle->get_dEdx_info(get_dEdx_info);
  kfparticle->saveDST(save_tracks_to_DST);
  kfparticle->setContainerName(pipi_reconstruction_name);
  kfparticle->saveParticleContainer(true);
  kfparticle->magFieldFile("FIELDMAP_TRACKING");

  //PV to SV cuts
  kfparticle->constrainToPrimaryVertex();
  kfparticle->setMotherIPchi2(lf_cuts_setMotherIPchi2);
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

  //Track parameters
  kfparticle->setMinimumTrackPT(lf_cuts_setMinimumTrackPT);
  kfparticle->setMinimumTrackIPchi2(lf_cuts_setMinimumTrackIPchi2);
  kfparticle->setMinimumTrackIPchi2_XY(lf_cuts_setMinimumTrackIPchi2_XY);
  kfparticle->setMinimumTrackIP(lf_cuts_setMinimumTrackIP);
  kfparticle->setMinimumTrackIP_XY(lf_cuts_setMinimumTrackIP_XY);
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
}

void reconstruct_ppi_mass()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(ppi_reconstruction_name);
  kfparticle->Verbosity(VERBOSITY_HF);

  if (run_ppi_reco) kfparticle->setDecayDescriptor(ppi_decay_descriptor);
  else kfparticle->setDecayDescriptor(anti_ppi_decay_descriptor);

  kfparticle->setTrackMapNodeName("HFSelected_SvtxTrackMap");
  kfparticle->extraolateTracksToSV(extraolate_tracks_to_secondary_vertex);
  kfparticle->saveOutput(save_kfpntuple);

  kfparticle->doTruthMatching(truthMatch);
  kfparticle->usePID(use_pid);
  kfparticle->setPIDacceptFraction(pid_frac);
  kfparticle->get_dEdx_info();
  kfparticle->dontUseGlobalVertex(dont_use_global_vertex);
  kfparticle->requireTrackVertexBunchCrossingMatch(require_track_and_vertex_match);
  kfparticle->getAllPVInfo(save_all_vtx_info);
  kfparticle->allowZeroMassTracks();
  kfparticle->use2Dmatching(use_2D_matching);
  kfparticle->getTriggerInfo(get_trigger_info);
  kfparticle->getDetectorInfo(get_detector_info);
  kfparticle->get_dEdx_info(get_dEdx_info);
  kfparticle->saveDST(save_tracks_to_DST);
  kfparticle->setContainerName(ppi_reconstruction_name);
  kfparticle->saveParticleContainer(true);
  kfparticle->magFieldFile("FIELDMAP_TRACKING");

  //PV to SV cuts
  kfparticle->constrainToPrimaryVertex();
  kfparticle->setMotherIPchi2(lf_cuts_setMotherIPchi2);
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

  //Track parameters
  kfparticle->setMinimumTrackPT(lf_cuts_setMinimumTrackPT);
  kfparticle->setMinimumTrackIPchi2(lf_cuts_setMinimumTrackIPchi2);
  kfparticle->setMinimumTrackIPchi2_XY(lf_cuts_setMinimumTrackIPchi2_XY);
  kfparticle->setMinimumTrackIP(lf_cuts_setMinimumTrackIP);
  kfparticle->setMinimumTrackIP_XY(lf_cuts_setMinimumTrackIP_XY);
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
}

void reconstruct_Lambdapi_mass()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(cascade_reconstruction_name);
  kfparticle->Verbosity(VERBOSITY_HF);

  kfparticle->setDecayDescriptor(cascade_decay_descriptor);
  //kfparticle->setTrackMapNodeName("HFSelected_SvtxTrackMap");

  kfparticle->saveOutput(save_kfpntuple);

  kfparticle->usePID(use_pid);
  //kfparticle->setPIDacceptFraction(pid_frac);
  kfparticle->dontUseGlobalVertex(dont_use_global_vertex);
  kfparticle->requireTrackVertexBunchCrossingMatch(require_track_and_vertex_match);
  kfparticle->getAllPVInfo(save_all_vtx_info);
  kfparticle->allowZeroMassTracks();
  kfparticle->use2Dmatching(use_2D_matching);
  kfparticle->getTriggerInfo(get_trigger_info);
  kfparticle->getDetectorInfo(get_detector_info);
  kfparticle->saveDST(save_tracks_to_DST);
  kfparticle->setContainerName(cascade_reconstruction_name);
  kfparticle->magFieldFile("FIELDMAP_TRACKING");

  //PV to SV cuts
  //kfparticle->constrainToPrimaryVertex(true);
  kfparticle->constrainToPrimaryVertex(false);
  kfparticle->useFakePrimaryVertex(true);
  kfparticle->setMotherIPchi2(100);
  kfparticle->setFlightDistancechi2(-1.);
  kfparticle->setMinDIRA(-1.1);
  kfparticle->setDecayLengthRange(-1*FLT_MAX, FLT_MAX);

  //Track parameters
  kfparticle->setMinimumTrackPT(0.0);
  kfparticle->setMaximumTrackPTchi2(FLT_MAX);
  kfparticle->setMaximumTrackchi2nDOF(FLT_MAX);
  kfparticle->setMinTPChits(0); // 20
  kfparticle->setMinMVTXhits(0); // 1
  kfparticle->setMinINTThits(0); // 1
  kfparticle->setMinimumTrackIP_XY(-100.);
  kfparticle->setMinimumTrackIPchi2_XY(-1000.);
  kfparticle->setMinimumTrackIP(-100.);
  kfparticle->setMinimumTrackIPchi2(-1000.);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(FLT_MAX);
  kfparticle->setMaximumDaughterDCA(5); //1 mm
  kfparticle->setMaximumDaughterDCA_XY(5);

  //Parent parameters
  kfparticle->setMotherPT(0);
  kfparticle->setMinimumMass(1.0); //Check mass ranges
  kfparticle->setMaximumMass(2.0);
  kfparticle->setMaximumMotherVertexVolume(10);

  //Intermediate parameters
  kfparticle->constrainIntermediateMasses(constrain_lambda_mass);

  std::vector<std::pair<float, float>> intermediate_mass_range;
  intermediate_mass_range.push_back(make_pair(1.0, 2.0));
  kfparticle->setIntermediateMassRange(intermediate_mass_range);

  std::vector<float> intermediate_min_dira = {-1.1};
  kfparticle->setIntermediateMinDIRA(intermediate_min_dira);

  std::vector<float> intermediate_min_pt = {0.0};
  kfparticle->setIntermediateMinPT(intermediate_min_pt);

  std::vector<std::pair<float, float>> intermediate_IP_range;
  intermediate_IP_range.push_back(make_pair(-1., FLT_MAX));
  kfparticle->setIntermediateIPRange(intermediate_IP_range);

  std::vector<std::pair<float, float>> intermediate_IPchi2_range;
  intermediate_IPchi2_range.push_back(make_pair(-1., FLT_MAX));
  kfparticle->setIntermediateIPchi2Range(intermediate_IPchi2_range);

  std::vector<float> intermediate_min_FDchi2 = {-1.};
  kfparticle->setIntermediateMinFDchi2(intermediate_min_FDchi2);

  std::vector<float> intermediate_max_vertex_vol = {1.1};
  kfparticle->setIntermediateMaxVertexVolume(intermediate_max_vertex_vol);

  std::vector<std::pair<float, float>> intermediate_IPxy_range;
  intermediate_IPxy_range.push_back(make_pair(-1., FLT_MAX));
  kfparticle->setIntermediateIPRange_XY(intermediate_IPxy_range);

  std::vector<std::pair<float, float>> intermediate_IPchi2xy_range;
  intermediate_IPchi2xy_range.push_back(make_pair(-1., FLT_MAX));
  kfparticle->setIntermediateIPchi2Range_XY(intermediate_IPchi2xy_range);

  kfparticle->setOutputName(cascade_output_reco_file);

  se->registerSubsystem(kfparticle);
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
    

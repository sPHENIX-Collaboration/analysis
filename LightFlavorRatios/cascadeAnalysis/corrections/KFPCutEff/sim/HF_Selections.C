#include <fun4all/Fun4AllServer.h>

#include <calotrigger/TriggerRunInfoReco.h>
#include <globalvertex/GlobalVertexReco.h>

#include <kfparticleqa/QAKFParticle.h>
#include <kfparticleqa/QAKFParticleTrackPtAsymmetry.h>

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wundefined-internal"

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#pragma GCC diagnostic pop

//#include <dstarreco/DstarReco.h>

R__LOAD_LIBRARY(libkfparticle_sphenix.so)
R__LOAD_LIBRARY(libcalotrigger.so)
//R__LOAD_LIBRARY(libDstarReco.so)
R__LOAD_LIBRARY(libkfparticleqa.so)

namespace HeavyFlavorReco
{
  int VERBOSITY_HF = 1;

  std::string output_dir = "/sphenix/tg/tg01/hf/rosstom/HF-analysis/"; //Top dir of where the output nTuples will be written
  std::string kfp_header = "outputKFParticle_";
  std::string processing_folder = "inReconstruction/";
  std::string trailer = ".root";

  bool run_Lambdapi_reco = true;

  std::string Lambdapi_decay_descriptor = "[Xi- -> {Lambda0 -> proton^+ pi^-} pi^-]cc"; //See twiki on how to set this
  std::string Lambdapi_reconstruction_name = "Lambdapi_reco"; //Used for naming output folder, file and node
  std::string Lambdapi_output_reco_file;
  std::string Lambdapi_output_dir;

  bool truthMatch = true;  
  bool save_kfpntuple = true;
  bool use_pid = false;
  bool save_tracks_to_DST = false;
  bool dont_use_global_vertex = true;
  bool require_track_and_vertex_match = true;
  //bool require_track_and_vertex_match = false;
  bool save_all_vtx_info = true;
  bool constrain_phi_mass = true;
  bool use_2D_matching = false;
  bool get_trigger_info = false;
  bool get_detector_info = true;
  bool get_dEdx_info = false; 
  bool get_calo_info = false;
  float pid_frac = 0.6;
  float cemc_proj_radius = 102.9; //Virgile recommendation according to DetailedCalorimeterGeometry
  bool constrain_lambda_mass = true;
  bool extraolate_tracks_to_secondary_vertex = true; // Set to false to ensure the pT map is accurate for asymmetry study  
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

void reconstruct_Lambdapi_mass()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(Lambdapi_reconstruction_name);
  kfparticle->Verbosity(VERBOSITY_HF);

  kfparticle->setDecayDescriptor(Lambdapi_decay_descriptor);
  //kfparticle->setTrackMapNodeName("HFSelected_SvtxTrackMap");

  kfparticle->saveOutput(save_kfpntuple);

  kfparticle->doTruthMatching(truthMatch);
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
  kfparticle->setContainerName(Lambdapi_reconstruction_name);
  kfparticle->magFieldFile("FIELDMAP_TRACKING");

  //PV to SV cuts
  kfparticle->constrainToPrimaryVertex(true);
  //kfparticle->constrainToPrimaryVertex(false);
  //kfparticle->useFakePrimaryVertex(true);
  //kfparticle->useTruthPrimaryVertex(true);
  kfparticle->setMotherIPchi2(1000);
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
  kfparticle->setMinimumMass(1.2); //Check mass ranges
  kfparticle->setMaximumMass(1.5);
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

  kfparticle->setOutputName(Lambdapi_output_reco_file);

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

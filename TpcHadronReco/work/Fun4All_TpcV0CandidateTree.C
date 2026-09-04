#ifndef MACRO_FUN4ALLTPCV0CANDIDATETREE_C
#define MACRO_FUN4ALLTPCV0CANDIDATETREE_C

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libTpcHadronReco.so)

#include <tpchadronreco/TpcV0CandidateTree.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <GlobalVariables.C>
#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>


#include <TSystem.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/recoConsts.h>

namespace
{
  std::string env_string(const char *name, const std::string &fallback)
  {
    const char *value = std::getenv(name);
    return value ? std::string(value) : fallback;
  }

  int env_int(const char *name, const int fallback)
  {
    const char *value = std::getenv(name);
    return value ? std::stoi(value) : fallback;
  }

  double env_double(const char *name, const double fallback)
  {
    const char *value = std::getenv(name);
    return value ? std::stod(value) : fallback;
  }

  bool env_bool(const char *name, const bool fallback)
  {
    const std::string value = env_string(name, fallback ? "1" : "0");
    return value == "1" || value == "true" || value == "TRUE" ||
           value == "yes" || value == "YES" || value == "on" || value == "ON";
  }
}

int Fun4All_TpcV0CandidateTree(
    const std::string &input_list = "input.list",
    const std::string &output_file = "TpcV0Candidates.root",
    const int nEvents = 0,
    const int skipEvents = 0,
    const bool apply_spatial_correction = true,
    const std::string &spatial_correction_file = "/sphenix/user/mitrankov/novel/coresoftware_TrackingDiagnostics/work/input/v0_spatial_map_transverse_only.root",
    const bool apply_spatial_correction_z = true,
    const double spatial_correction_scale = 1.0)
{

  auto *se = Fun4AllServer::instance();
  se->Verbosity(env_int("V0_SERVER_VERBOSITY", 0));

  recoConsts* rc = recoConsts::instance();  
  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
  rc->set_uint64Flag("TIMESTAMP", 1);
  
  auto *v0 = new TpcV0CandidateTree("TpcV0CandidateTree", output_file);
  v0->Verbosity(env_int("V0_MODULE_VERBOSITY", 0));

  v0->use_pattern_cluster_tracks(env_bool("V0_USE_PATTERN_TRACKS", true));
  v0->set_tpc_sa_cluster_node(env_string("V0_CLUSTER_NODE", "TPC_POLYCLUSTERS"));
  v0->set_tpc_sa_track_node(env_string("V0_TRACK_NODE", "TPC_POLYTRACKS"));
  v0->set_tpc_sa_track_vertex_node(env_string("V0_VERTEX_NODE", "TPC_POLYTRACKVERTICES"));

  // Real data: use the fixed beam position, not G4 truth.
  v0->set_use_truth_primary_vertex(env_bool("V0_USE_TRUTH_VERTEX", false));
  v0->set_primary_vertex(
      env_double("V0_BEAM_X", 0.158),
      env_double("V0_BEAM_Y", 0.285),
      env_double("V0_BEAM_Z", 0.0));

  // Optional prompt-track Gaussian vertex update.  The default XY mode uses
  // the configured beam position with 0.5 mm uncertainty and leaves z free.
  // Existing primary_* variables remain unconstrained; the second result is
  // written to primary_constrained_* and *_primary_constrained mass branches.
  const bool use_primary_constraint =
      env_bool("V0_USE_PRIMARY_VERTEX_CONSTRAINT", true);
  const std::string primary_constraint_mode =
      env_string("V0_PRIMARY_VERTEX_CONSTRAINT_MODE", "xy");
  v0->set_primary_vertex_constraint(use_primary_constraint);
  if (!v0->set_primary_vertex_constraint_mode(primary_constraint_mode))
  {
    std::cerr << "Unknown V0_PRIMARY_VERTEX_CONSTRAINT_MODE='"
              << primary_constraint_mode << "'" << std::endl;
    return 2;
  }
  v0->set_primary_vertex_constraint_sigmas(
      env_double("V0_PRIMARY_VERTEX_SIGMA_XY_CM", 0.05),
      env_double("V0_PRIMARY_VERTEX_SIGMA_Z_CM", 0.05));
  v0->set_compute_constrained_phi(
      env_bool("V0_COMPUTE_CONSTRAINED_PHI", true));
  v0->set_compute_constrained_d0(
      env_bool("V0_COMPUTE_CONSTRAINED_D0", true));
  v0->set_use_constrained_phi_selection(
      env_bool("V0_USE_CONSTRAINED_PHI_SELECTION", false));
  v0->set_use_constrained_d0_selection(
      env_bool("V0_USE_CONSTRAINED_D0_SELECTION", false));

  // Optional residual spatial map applied to cluster coordinates before any
  // Kalman/helix fit.  Function arguments provide convenient interactive use;
  // the environment variables below override them for batch jobs.
  const bool use_spatial_correction =
      env_bool("V0_APPLY_SPATIAL_CORRECTION", apply_spatial_correction);
  const std::string spatial_map_file =
      env_string("V0_SPATIAL_CORRECTION_FILE", spatial_correction_file);
  const bool use_spatial_correction_z =
      env_bool("V0_APPLY_SPATIAL_CORRECTION_Z", apply_spatial_correction_z);
  const double spatial_map_scale =
      env_double("V0_SPATIAL_CORRECTION_SCALE", spatial_correction_scale);

  v0->set_apply_spatial_correction(use_spatial_correction);
  v0->set_spatial_correction_file(spatial_map_file);
  v0->set_apply_spatial_correction_z(use_spatial_correction_z);
  v0->set_spatial_correction_scale(spatial_map_scale);

  if (use_spatial_correction)
  {
    std::cout << "V0 spatial correction: ON"
              << " file=" << spatial_map_file
              << " apply_z=" << (use_spatial_correction_z ? 1 : 0)
              << " scale=" << spatial_map_scale
              << std::endl;
  }
  else
  {
    std::cout << "V0 spatial correction: OFF" << std::endl;
  }

  const std::string fit_mode = env_string("V0_FIT_MODE", "kalman");
  if (!v0->set_track_fit_method(fit_mode))
  {
    std::cerr << "Unknown V0_FIT_MODE='" << fit_mode << "'" << std::endl;
    return 2;
  }

  v0->set_point_order(env_string("V0_POINT_ORDER", "radius-descending"));
  v0->set_min_points(env_int("V0_MIN_POINTS", 30));
  v0->set_fit_first_points(env_int("V0_FIT_FIRST_POINTS", 8));
  v0->set_bfield(env_double("V0_BFIELD_T", 1.4));

  const bool use_field_map = env_bool("V0_USE_FIELD_MAP", true);
  v0->use_kalman_field_map(use_field_map);
  v0->set_kalman_analytic_uniform_propagation(
      use_field_map ? false : env_bool("V0_ANALYTIC_UNIFORM", true));
  v0->set_kalman_rkn4(
      env_double("V0_RKN_MAX_STEP_CM", 10.0),
      env_double("V0_RKN_TOLERANCE", 0),
      env_int("V0_RKN_MAX_TRIALS", 1),
      env_int("V0_RKN_MAX_STEPS", 2000));
  v0->set_kalman_variable_field_within_step(
      env_bool("V0_RKN_VARIABLE_FIELD_WITHIN_STEP", false));
  v0->set_kalman_fast_field_jacobian(
      env_bool("V0_RKN_FAST_FIELD_JACOBIAN", true));
  v0->set_kalman_fast_field_pca(
      env_bool("V0_RKN_FAST_FIELD_PCA", true));
  v0->set_kalman_field_pca_refine_iterations(
      env_int("V0_RKN_FIELD_PCA_REFINE_ITERATIONS", 1));

  // Tight global-helix prior and robust, iterated Kalman update.  The q/pT
  // prior is relative to the helix seed so high-pT tracks are not assigned an
  // unrealistically loose fixed curvature uncertainty.
  v0->set_kalman_initial_sigmas(
      env_double("V0_INITIAL_SIGMA_POS_CM", 0.05),
      env_double("V0_INITIAL_SIGMA_PHI", 0.010),
      env_double("V0_INITIAL_SIGMA_TANL", 0.020));
  v0->set_kalman_relative_qop_seed(
      env_bool("V0_USE_RELATIVE_QOPT_SEED", true),
      env_double("V0_INITIAL_SIGMA_QOPT_RELATIVE", 0.05),
      env_double("V0_INITIAL_SIGMA_QOPT_FLOOR", 5.0e-4));
  v0->set_kalman_absolute_qop_seed_sigma(
      env_double("V0_INITIAL_SIGMA_QOPT_ABSOLUTE", 0.20));
  v0->set_kalman_robust_measurement_updates(
      env_bool("V0_KALMAN_ROBUST_UPDATES", true),
      env_double("V0_KALMAN_ROBUST_CHI2_SOFT", 16.0),
      env_double("V0_KALMAN_ROBUST_CHI2_HARD", 100.0));
  const int kalman_iterations = env_int("V0_KALMAN_ITERATIONS", 1);
  v0->set_kalman_fit_iterations(kalman_iterations);

  // Deterministic, helix-like propagation. Do not use process noise to absorb
  // TPC module misalignment; the regional sigmas and robust update handle it.
  v0->set_kalman_process_sigmas(
      env_double("V0_PROCESS_SIGMA_POS", 0.0),
      env_double("V0_PROCESS_SIGMA_PHI", 0.0),
      env_double("V0_PROCESS_SIGMA_QOPT", 0.0),
      env_double("V0_PROCESS_SIGMA_TANL", 0.0));
  v0->set_kalman_material(
      env_double("V0_MATERIAL_X0_PER_CM", 0.0),
      env_double("V0_SCATTERING_SCALE", 0.0),
      env_double("V0_ELOSS_GEV_PER_CM", 0.0),
      env_double("V0_ELOSS_SIGMA_FRACTION", 0.0));

  // Region-dependent measurement uncertainties [cm].
  v0->set_tpc_region_measurement_sigmas(
      env_double("V0_R1_SIGMA_RPHI", 0.040),
      env_double("V0_R1_SIGMA_R", 0.200),
      env_double("V0_R2_SIGMA_RPHI", 0.025),
      env_double("V0_R2_SIGMA_R", 0.050),
      env_double("V0_R3_SIGMA_RPHI", 0.025),
      env_double("V0_R3_SIGMA_R", 0.200),
      env_double("V0_SIGMA_Z", 0.100));
  v0->set_tpc_transition_measurement_sigmas(
      env_double("V0_TRANSITION_SIGMA_RPHI", 0.200),
      env_double("V0_TRANSITION_SIGMA_R", 0.500),
      env_double("V0_TRANSITION_SIGMA_Z", 0.200));
  v0->set_exclude_tpc_transition_layers(env_bool("V0_EXCLUDE_TRANSITION_LAYERS", true));

  // Fast common track-only preselection. Pair-level rough line PCA cuts stay
  // disabled so prompt pairs always reach the primary-PCA reconstruction.
  v0->set_pre_track_pt_min(env_double("V0_PRE_TRACK_PT_MIN", 0.20));
  v0->set_pre_track_dca_xy_min(env_double("V0_PRE_TRACK_DCA_XY_MIN", -1.0));
  v0->set_pre_track_dca_z_min(env_double("V0_PRE_TRACK_DCA_Z_MIN", -1.0));
  v0->set_pre_track_dca_xy_max(env_double("V0_PRE_TRACK_DCA_XY_MAX", -1.0));
  v0->set_pre_track_dca_z_max(env_double("V0_PRE_TRACK_DCA_Z_MAX", -1.0));
  v0->set_pre_pair_dca_max(env_double("V0_PRE_PAIR_DCA_MAX", -1.0));
  v0->set_pre_lproj_min(env_double("V0_PRE_LPROJ_MIN", -1.0));
  v0->set_pre_cos_theta_min(env_double("V0_PRE_COSTHETA_MIN", -2.0));
  v0->set_pre_track_quality_max(env_double("V0_PRE_TRACK_QUALITY_MAX", -1.0));
  v0->set_pre_track_npoints_min(env_int("V0_PRE_TRACK_NPOINTS_MIN", 20));

  // K0S -> pi+ pi-. These defaults reproduce the cuts you specified.
  v0->set_kshort_selection(
      env_bool("V0_ENABLE_KSHORT", true),
      env_int("V0_KSHORT_MIN_TPC_CLUSTERS", 20),
      env_double("V0_KSHORT_TRACK_PT_MIN", 0.20),
      env_double("V0_KSHORT_MASS_MIN", 0.40),
      env_double("V0_KSHORT_MASS_MAX", 0.60),
      env_double("V0_KSHORT_PCA_Z_MAX", 20.0),
      env_double("V0_KSHORT_PCA_DZ_MAX", 0.7),
      env_double("V0_KSHORT_DECAY_R_MIN", 2.0),
      env_double("V0_KSHORT_ALPHA_ABS_MAX", 1.0),
      env_double("V0_KSHORT_PAIR_DCA_MAX", 3.0),
      env_double("V0_KSHORT_DIRA_MIN", 0.75));

   v0->set_kshort_mass_range(
      env_double("V0_KSHORT_MASS_MIN", 0.40),
      env_double("V0_KSHORT_MASS_MAX", 0.60));

  // Lambda -> p pi- and anti-Lambda -> anti-p pi+.
  v0->set_lambda_selection(
      env_bool("V0_ENABLE_LAMBDA", true),
      env_int("V0_LAMBDA_MIN_TPC_CLUSTERS", 20),
      env_double("V0_LAMBDA_TRACK_PT_MIN", 0.20),
      env_double("V0_LAMBDA_MASS_MIN", 1.08),
      env_double("V0_LAMBDA_MASS_MAX", 1.16),
      env_double("V0_LAMBDA_PCA_Z_MAX", 20.0),
      env_double("V0_LAMBDA_PCA_DZ_MAX", 0.7),
      env_double("V0_LAMBDA_DECAY_R_MIN", 2.0),
      env_double("V0_LAMBDA_ALPHA_ABS_MAX", 1.0),
      env_double("V0_LAMBDA_PAIR_DCA_MAX", 3.0),
      env_double("V0_LAMBDA_DIRA_MIN", 0.75));

  v0->set_antilambda_selection(
      env_bool("V0_ENABLE_ANTILAMBDA", true),
      env_int("V0_ANTILAMBDA_MIN_TPC_CLUSTERS", 20),
      env_double("V0_ANTILAMBDA_TRACK_PT_MIN", 0.20),
      env_double("V0_ANTILAMBDA_MASS_MIN", 1.08),
      env_double("V0_ANTILAMBDA_MASS_MAX", 1.16),
      env_double("V0_ANTILAMBDA_PCA_Z_MAX", 20.0),
      env_double("V0_ANTILAMBDA_PCA_DZ_MAX", 0.7),
      env_double("V0_ANTILAMBDA_DECAY_R_MIN", 2.0),
      env_double("V0_ANTILAMBDA_ALPHA_ABS_MAX", 1.0),
      env_double("V0_ANTILAMBDA_PAIR_DCA_MAX", 3.0),
      env_double("V0_ANTILAMBDA_DIRA_MIN", 0.75));

  // Lambda seeds for Sigma(1385), Xi, and Omega.  Do not impose a primary-
  // vertex pointing requirement here: cascade Lambdas originate at Xi/Omega.
  v0->set_hyperon_lambda_seed_selection(
      env_int("V0_HYPERON_LAMBDA_MIN_TPC_CLUSTERS", 20),
      env_double("V0_HYPERON_LAMBDA_TRACK_PT_MIN", 0.20),
      env_double("V0_HYPERON_LAMBDA_MASS_MIN", 1.08),
      env_double("V0_HYPERON_LAMBDA_MASS_MAX", 1.16),
      env_double("V0_HYPERON_LAMBDA_PCA_Z_MAX", 20.0),
      env_double("V0_HYPERON_LAMBDA_PCA_DZ_MAX", 0.7),
      env_double("V0_HYPERON_LAMBDA_DECAY_R_MIN", 1.0),
      env_double("V0_HYPERON_LAMBDA_ALPHA_ABS_MAX", 1.0),
      env_double("V0_HYPERON_LAMBDA_PAIR_DCA_MAX", 3.0));

  // Sigma(1385)^+/- -> Lambda pi^+/- (and anti-Sigma).
  v0->set_sigma1385_selection(
      env_bool("V0_ENABLE_SIGMA1385", false),
      env_int("V0_SIGMA1385_MIN_TPC_CLUSTERS", 20),
      env_double("V0_SIGMA1385_BACHELOR_PT_MIN", 0.20),
      env_double("V0_SIGMA1385_MASS_MIN", 1.25),
      env_double("V0_SIGMA1385_MASS_MAX", 1.55),
      env_double("V0_SIGMA1385_BACHELOR_DCA_XY_MAX", 3.0),
      env_double("V0_SIGMA1385_BACHELOR_DCA_Z_MAX", -1.0));

  // Xi^- -> Lambda pi^- and anti-Xi^+ -> anti-Lambda pi^+.
  v0->set_xi_selection(
      env_bool("V0_ENABLE_XI", false),
      env_int("V0_XI_MIN_TPC_CLUSTERS", 20),
      env_double("V0_XI_BACHELOR_PT_MIN", 0.20),
      env_double("V0_XI_MASS_MIN", 1.25),
      env_double("V0_XI_MASS_MAX", 1.40),
      env_double("V0_XI_PAIR_DCA_MAX", 2.0),
      env_double("V0_XI_DECAY_R_MIN", 0.5),
      env_double("V0_XI_DIRA_MIN", 0.75),
      env_double("V0_XI_LAMBDA_FLIGHT_MIN", 0.2),
      env_double("V0_XI_LAMBDA_DIRA_MIN", 0.75),
      env_double("V0_XI_BACHELOR_DCA_XY_MIN", -1.0));

  // Omega^- -> Lambda K^- and anti-Omega^+ -> anti-Lambda K^+.
  v0->set_omega_selection(
      env_bool("V0_ENABLE_OMEGA", false),
      env_int("V0_OMEGA_MIN_TPC_CLUSTERS", 20),
      env_double("V0_OMEGA_BACHELOR_PT_MIN", 0.20),
      env_double("V0_OMEGA_MASS_MIN", 1.60),
      env_double("V0_OMEGA_MASS_MAX", 1.75),
      env_double("V0_OMEGA_PAIR_DCA_MAX", 2.0),
      env_double("V0_OMEGA_DECAY_R_MIN", 0.5),
      env_double("V0_OMEGA_DIRA_MIN", 0.75),
      env_double("V0_OMEGA_LAMBDA_FLIGHT_MIN", 0.2),
      env_double("V0_OMEGA_LAMBDA_DIRA_MIN", 0.75),
      env_double("V0_OMEGA_BACHELOR_DCA_XY_MIN", -1.0));

  v0->set_cascade_search(
      env_double("V0_CASCADE_BACHELOR_MAX_UPSTREAM_CM", 80.0),
      env_double("V0_CASCADE_BACHELOR_DOWNSTREAM_MARGIN_CM", 5.0),
      env_double("V0_CASCADE_LAMBDA_MAX_UPSTREAM_CM", 80.0),
      env_double("V0_CASCADE_LAMBDA_DOWNSTREAM_MARGIN_CM", 1.0),
      env_int("V0_CASCADE_COARSE_STEPS", 48),
      env_int("V0_CASCADE_REFINE_CANDIDATES", 8));
  v0->set_use_nominal_lambda_mass_for_composites(
      env_bool("V0_COMPOSITE_USE_NOMINAL_LAMBDA_MASS", true));

  // Prompt phi -> K+K-. Keep reconstruction cuts loose; the QA macro scans
  // the strongest same-collision variable |primary_pca1_z-primary_pca2_z|.
  v0->set_phi_selection(
    env_bool("V0_ENABLE_PHI", true),
    env_int("V0_PHI_MIN_TPC_CLUSTERS", 30),
    env_double("V0_PHI_TRACK_PT_MIN", 0.20),
    env_double("V0_PHI_MASS_MIN", 0.98),
    env_double("V0_PHI_MASS_MAX", 1.08),
    env_double("V0_PHI_TRACK_DCA_XY_MAX", 3.0),
    env_double("V0_PHI_TRACK_DCA_Z_MAX", -1.0),
    env_double("V0_PHI_PAIR_DCA_MAX", -1.0),
    env_double("V0_PHI_FLIGHT_LENGTH_MAX", -1.0),
    env_double("V0_PHI_PRIMARY_PCA_Z_MAX", 20.0),
    env_double("V0_PHI_PRIMARY_PCA_DZ_MAX", 2.0)
  );

  // Prompt J/psi -> e+e-. Electron identification is not applied here;
  // the branch is an opposite-sign dielectron mass hypothesis.
  v0->set_jpsi_selection(
    env_bool("V0_ENABLE_JPSI", false),
    env_int("V0_JPSI_MIN_TPC_CLUSTERS", 30),
    env_double("V0_JPSI_TRACK_PT_MIN", 0.50),
    env_double("V0_JPSI_MASS_MIN", 2.0),
    env_double("V0_JPSI_MASS_MAX", 4.0),
    env_double("V0_JPSI_TRACK_DCA_XY_MAX", 3.0),
    env_double("V0_JPSI_TRACK_DCA_Z_MAX", -1.0),
    env_double("V0_JPSI_PAIR_DCA_MAX", -1.0),
    env_double("V0_JPSI_FLIGHT_LENGTH_MAX", -1.0),
    env_double("V0_JPSI_PRIMARY_PCA_Z_MAX", 20.0),
    env_double("V0_JPSI_PRIMARY_PCA_DZ_MAX", 2.0)
  );

  // Prompt D0 -> K- pi+ and anti-D0 -> K+ pi-.
  v0->set_d0_selection(
    env_bool("V0_ENABLE_D0", true),
    env_int("V0_D0_MIN_TPC_CLUSTERS", 30),
    env_double("V0_D0_TRACK_PT_MIN", 0.20),
    env_double("V0_D0_MASS_MIN", 1.70),
    env_double("V0_D0_MASS_MAX", 2.05),
    env_double("V0_D0_TRACK_DCA_XY_MAX", 3.0),
    env_double("V0_D0_TRACK_DCA_Z_MAX", -1.0),
    env_double("V0_D0_PAIR_DCA_MAX", -1.0),
    env_double("V0_D0_FLIGHT_LENGTH_MAX", -1.0),
    env_double("V0_D0_PRIMARY_PCA_Z_MAX", 20.0),
    env_double("V0_D0_PRIMARY_PCA_DZ_MAX", 2.0)
);

  v0->set_antid0_selection(
    env_bool("V0_ENABLE_ANTID0", true),
    env_int("V0_ANTID0_MIN_TPC_CLUSTERS", 30),
    env_double("V0_ANTID0_TRACK_PT_MIN", 0.20),
    env_double("V0_ANTID0_MASS_MIN", 1.70),
    env_double("V0_ANTID0_MASS_MAX", 2.05),
    env_double("V0_ANTID0_TRACK_DCA_XY_MAX", 3.0),
    env_double("V0_ANTID0_TRACK_DCA_Z_MAX", -1.0),
    env_double("V0_ANTID0_PAIR_DCA_MAX", -1.0),
    env_double("V0_ANTID0_FLIGHT_LENGTH_MAX", -1.0),
    env_double("V0_ANTID0_PRIMARY_PCA_Z_MAX", 20.0),
    env_double("V0_ANTID0_PRIMARY_PCA_DZ_MAX", 2.0));

  // Like-sign backgrounds are written to a separate tree with exactly the
  // same branch layout as pairTree.  V0_WRITE_SAME_SIGN remains a legacy K0S
  // fallback; species-specific switches control Lambda, phi, and D0.
  const bool legacy_same_sign = env_bool("V0_WRITE_SAME_SIGN", true);
  const bool write_likesign_tree =
      env_bool("V0_WRITE_LIKESIGN_TREE", legacy_same_sign);
  v0->set_write_likesign_tree(write_likesign_tree);
  v0->set_write_likesign_kshort(
      env_bool("V0_WRITE_LIKESIGN_KSHORT", true));//legacy_same_sign));
  v0->set_write_likesign_lambda(
      env_bool("V0_WRITE_LIKESIGN_LAMBDA", true));
  v0->set_write_likesign_phi(
      env_bool("V0_WRITE_LIKESIGN_PHI", true));
  v0->set_write_likesign_jpsi(
      env_bool("V0_WRITE_LIKESIGN_JPSI", false));
  v0->set_write_likesign_d0(
      env_bool("V0_WRITE_LIKESIGN_D0", true));
  v0->set_likesign_keep_fraction(
      env_double("V0_LIKESIGN_KEEP_FRACTION", 1.0));
  
  
  v0->set_tpc_crossing_decision_node(
      env_string("V0_CROSSING_NODE", "TPC_CROSSING_DECISIONS"));
  v0->set_require_same_crossing(env_bool("V0_REQUIRE_SAME_CROSSING", true));
  
  
  // Full daughter cluster payload remains K0S-only. TrackTree is independent
  // and can be enabled for residual/DCA QA with V0_WRITE_TRACK_TREE=1.
  v0->set_write_track_tree(env_bool("V0_WRITE_TRACK_TREE", false));
  //v0->set_write_track_tree(true);
  v0->set_write_cluster_residual_tree(env_bool("V0_WRITE_CLUSTER_RESIDUAL_TREE", false));
  v0->set_write_kalman_innovation_diagnostics(env_bool("V0_WRITE_KALMAN_DIAGNOSTICS", true));
  v0->set_write_kshort_daughter_details(env_bool("V0_WRITE_KSHORT_DETAILS", true));
  v0->set_kshort_detail_mass_range(
      env_double("V0_KSHORT_DETAIL_MASS_MIN", 0.40),
      env_double("V0_KSHORT_DETAIL_MASS_MAX", 0.60));
  v0->set_print_timing(env_bool("V0_PRINT_TIMING", false));


  std::cout << "V0 Kalman configuration:"
            << " point_order=" << env_string("V0_POINT_ORDER", "radius-descending")
            << " field_map=" << (use_field_map ? 1 : 0)
            << " variable_field_within_step="
            << (env_bool("V0_RKN_VARIABLE_FIELD_WITHIN_STEP", false) ? 1 : 0)
//            << " analytic_uniform="
//            << (use_field_map ? 0 : (env_bool("V0_ANALYTIC_UNIFORM", true) ? 1 : 0))
//            << " rkn_max_step_cm=" << env_double("V0_RKN_MAX_STEP_CM", 5.0)
//            << " rkn_tolerance=" << env_double("V0_RKN_TOLERANCE", 1.0e-4)
//            << " fast_jacobian="
//            << (env_bool("V0_RKN_FAST_FIELD_JACOBIAN", true) ? 1 : 0)
//            << " fast_pca="
//            << (env_bool("V0_RKN_FAST_FIELD_PCA", true) ? 1 : 0)
            << " iterations=" << kalman_iterations
            << " robust=" << (env_bool("V0_KALMAN_ROBUST_UPDATES", true) ? 1 : 0)
            << " exclude_transition="
            << (env_bool("V0_EXCLUDE_TRANSITION_LAYERS", true) ? 1 : 0)
            << " primary_constraint=" << (use_primary_constraint ? 1 : 0)
            << " primary_mode=" << primary_constraint_mode
            << " likeSignPairTree=" << (write_likesign_tree ? 1 : 0)
            << " sigma1385=" << (env_bool("V0_ENABLE_SIGMA1385", false) ? 1 : 0)
            << " xi=" << (env_bool("V0_ENABLE_XI", false) ? 1 : 0)
            << " omega=" << (env_bool("V0_ENABLE_OMEGA", false) ? 1 : 0)
            << " trackTree=" << (env_bool("V0_WRITE_TRACK_TREE", false) ? 1 : 0)
            << std::endl;

  se->registerSubsystem(v0);

  auto *in = new Fun4AllDstInputManager("DSTin");
  if (in->AddListFile(input_list) != 0)
  {
    std::cerr << "Failed to open input list: " << input_list << std::endl;
    return 3;
  }
  se->registerInputManager(in);

  if (skipEvents > 0)
  {
    se->skip(skipEvents);
  }

  se->run(nEvents);
  se->End();
  delete se;

  std::cout << "Wrote " << output_file << std::endl;
  gSystem->Exit(0);
  return 0;
}

#endif


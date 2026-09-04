// Tell emacs that this is a C++ source
// -*- C++ -*-.
#ifndef TPCHADRONRECO_TPCV0CANDIDATETREE_H
#define TPCHADRONRECO_TPCV0CANDIDATETREE_H

#include "TpcTrackFit.h"

#include <fun4all/SubsysReco.h>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

class PHCompositeNode;
class PHField;
class PHG4Hit;
class PHG4HitContainer;
class PHG4TruthInfoContainer;
class TFile;
class TH3;
class TTree;
class Tpc_PolyClusterContainer;
class Tpc_PolyTrackContainer;
class Tpc_PolyTrackVertexContainer;
class TpcCrossingDecisionContainer;

class TpcV0CandidateTree : public SubsysReco
{
 public:
  TpcV0CandidateTree(const std::string &name = "TpcV0CandidateTree",
                     const std::string &filename = "TpcV0Candidates.root");
  ~TpcV0CandidateTree() override;

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_output_file(const std::string &filename) { m_filename = filename; }
  void set_truth_point_node(const std::string &name) { m_truth_point_node = name; }
  void set_truth_info_node(const std::string &name) { m_truth_info_node = name; }
  void set_tpc_sa_cluster_node(const std::string &name) { m_tpc_sa_cluster_node = name; }
  void set_tpc_sa_track_node(const std::string &name) { m_tpc_sa_track_node = name; }
  void set_tpc_sa_track_vertex_node(const std::string &name) { m_tpc_sa_track_vertex_node = name; }
  void set_tpc_crossing_decision_node(const std::string &name) { m_tpc_crossing_decision_node = name; }
  void set_require_same_crossing(const bool value = true) { m_require_same_crossing = value; }
  void use_pattern_cluster_tracks(const bool value = true) { m_use_pattern_cluster_tracks = value; }
  void set_use_truth_primary_vertex(const bool value) { m_use_truth_primary_vertex = value; }
  void set_primary_vertex(const double x, const double y, const double z);

  // Optional cluster-position correction applied before the track fit.
  // The ROOT file must contain:
  //   side0/h3_delta_r, side0/h3_rdelta_phi, side0/h3_delta_z
  //   side1/h3_delta_r, side1/h3_rdelta_phi, side1/h3_delta_z
  // with axes (r [cm], phi, z [cm]).  The map convention is
  // measured-minus-fit, so corrected coordinates are measured - map.
  void set_apply_spatial_correction(const bool value = true)
  {
    m_apply_spatial_correction = value;
  }
  void set_spatial_correction_file(const std::string &filename)
  {
    m_spatial_correction_filename = filename;
  }
  void set_spatial_correction_scale(const double value)
  {
    m_spatial_correction_scale = value;
  }
  void set_apply_spatial_correction_z(const bool value = true)
  {
    m_apply_spatial_correction_z = value;
  }

  void set_min_points(const int value) { m_min_points = value; }
  void set_fit_helix(const bool value)
  {
    m_fit_helix_tracks = value;
    if (value)
    {
      m_fit_kalman_tracks = false;
    }
  }
  void set_fit_kalman(const bool value)
  {
    m_fit_kalman_tracks = value;
    if (value)
    {
      m_fit_helix_tracks = false;
      m_use_final_track_helix = false;
    }
  }
  bool set_track_fit_method(const std::string &mode);
  void set_use_final_track_helix(const bool value) { m_use_final_track_helix = value; }
  bool set_point_order(const std::string &mode);
  void set_fit_first_points(const int value) { m_fit_first_points = value; }
  void set_bfield(const double value)
  {
    m_bfield_t = value;
    m_kalman_config.bfield_t = value;
  }
  void set_kalman_magnetic_field(const PHField *field);
  void set_kalman_analytic_uniform_propagation(const bool value = true)
  {
    m_kalman_config.analytic_uniform_propagation = value;
  }
  void use_kalman_field_map(const bool value = true)
  {
    m_use_kalman_field_map = value;
    if (!value)
    {
      m_kalman_config.magnetic_field = nullptr;
    }
  }
  void set_kalman_rkn4(const double max_step_cm,
                       const double step_tolerance,
                       const int max_step_trials = 12,
                       const int max_total_steps = 2000)
  {
    m_kalman_config.rkn_max_step_cm = max_step_cm;
    m_kalman_config.rkn_step_tolerance = step_tolerance;
    m_kalman_config.rkn_max_step_trials = max_step_trials;
    m_kalman_config.rkn_max_total_steps = max_total_steps;
  }
  void set_kalman_variable_field_within_step(const bool value = true)
  {
    m_kalman_config.rkn_variable_field_within_step = value;
  }
  void set_kalman_fast_field_jacobian(const bool value = true)
  {
    m_kalman_config.rkn_fast_field_jacobian = value;
  }
  void set_kalman_fast_field_pca(const bool value = true)
  {
    m_kalman_config.rkn_fast_field_pca = value;
  }
  void set_kalman_field_pca_refine_iterations(const int value)
  {
    m_kalman_config.rkn_field_pca_refine_iterations = value;
  }
  void set_theta_extension(const double value) { m_theta_extension = value; }
  void set_coarse_steps(const int value) { m_coarse_steps = value; }
  void set_pca_candidates(const int value) { m_pca_candidates = value; }
  void set_print_timing(const bool value = true) { m_print_timing = value; }
  void set_downstream_margin(const double value) { m_downstream_margin = value; }
  void set_final_track_helix_search(const double max_upstream_cm,
                                    const double downstream_margin_cm)
  {
    m_final_track_helix_max_upstream_cm = max_upstream_cm;
    m_final_track_helix_downstream_margin_cm = downstream_margin_cm;
  }
  void set_kalman_search(const double max_upstream_cm, const double downstream_margin_cm)
  {
    m_kalman_max_upstream_cm = max_upstream_cm;
    m_kalman_downstream_margin_cm = downstream_margin_cm;
  }
  void set_kalman_measurement_sigmas(const double xy_cm, const double z_cm)
  {
    set_kalman_measurement_sigmas(xy_cm, xy_cm, z_cm);
  }
  void set_kalman_measurement_sigmas(const double rphi_cm,
                                     const double r_cm,
                                     const double z_cm)
  {
    m_kalman_config.meas_sigma_rphi_cm = rphi_cm;
    m_kalman_config.meas_sigma_r_cm = r_cm;
    m_kalman_config.meas_sigma_z_cm = z_cm;
  }
  void set_write_kalman_innovation_diagnostics(const bool value = true)
  {
    m_kalman_config.collect_innovation_components = value;
  }
  void set_kalman_initial_sigmas(const double position_cm,
                                  const double phi,
                                  const double tan_lambda)
  {
    m_kalman_config.initial_sigma_pos_cm = position_cm;
    m_kalman_config.initial_sigma_phi = phi;
    m_kalman_config.initial_sigma_tanl = tan_lambda;
  }
  void set_kalman_relative_qop_seed(const bool enabled,
                                    const double relative_fraction,
                                    const double absolute_floor)
  {
    m_kalman_config.use_relative_initial_sigma_qop_t = enabled;
    m_kalman_config.initial_sigma_qop_t_relative = relative_fraction;
    m_kalman_config.initial_sigma_qop_t_floor = absolute_floor;
  }
  void set_kalman_absolute_qop_seed_sigma(const double value)
  {
    m_kalman_config.initial_sigma_qop_t = value;
  }
  void set_kalman_robust_measurement_updates(const bool enabled,
                                             const double soft_chi2,
                                             const double hard_chi2)
  {
    m_kalman_config.robust_measurement_updates = enabled;
    m_kalman_config.robust_chi2_soft = soft_chi2;
    m_kalman_config.robust_chi2_hard = hard_chi2;
  }
  void set_kalman_fit_iterations(const int value)
  {
    m_kalman_config.fit_iterations = value;
  }
  void set_kalman_process_sigmas(const double pos_cm,
                                 const double phi,
                                 const double qop_t,
                                 const double tanl)
  {
    m_kalman_config.process_sigma_pos_cm = pos_cm;
    m_kalman_config.process_sigma_phi = phi;
    m_kalman_config.process_sigma_qop_t = qop_t;
    m_kalman_config.process_sigma_tanl = tanl;
  }
  void set_kalman_material(const double x0_per_cm,
                           const double multiple_scattering_scale,
                           const double energy_loss_gev_per_cm,
                           const double energy_loss_sigma_fraction)
  {
    m_kalman_config.material_x0_per_cm = x0_per_cm;
    m_kalman_config.multiple_scattering_scale = multiple_scattering_scale;
    m_kalman_config.energy_loss_gev_per_cm = energy_loss_gev_per_cm;
    m_kalman_config.energy_loss_sigma_fraction = energy_loss_sigma_fraction;
  }
  void set_prefer_positive_pointing(const bool value) { m_prefer_positive_pointing = value; }

  void set_pre_track_pt_min(const double value) { m_pre_track_pt_min = value; }
  void set_pre_track_dca_xy_min(const double value) { m_pre_track_dca_xy_min = value; }
  void set_pre_track_dca_z_min(const double value) { m_pre_track_dca_z_min = value; }
  void set_pre_track_dca_xy_max(const double value) { m_pre_track_dca_xy_max = value; }
  void set_pre_track_dca_z_max(const double value) { m_pre_track_dca_z_max = value; }
  void set_pre_pair_dca_max(const double value) { m_pre_pair_dca_max = value; }
  void set_pre_lproj_min(const double value) { m_pre_lproj_min = value; }
  void set_pre_cos_theta_min(const double value) { m_pre_cos_theta_min = value; }
  void set_pre_track_quality_max(const double value) { m_pre_track_quality_max = value; }
  void set_pre_track_npoints_min(const int value) { m_pre_track_npoints_min = value; }
  void set_pair_pca_z_max(const double value) { m_pair_pca_z_max = value; }
  void set_pair_pca_dz_max(const double value) { m_pair_pca_dz_max = value; }
  void set_pair_decay_radius_min(const double value) { m_pair_decay_radius_min = value; }
  void set_pair_alpha_abs_max(const double value) { m_pair_alpha_abs_max = value; }
  void set_pair_dca_max(const double value) { m_pair_dca_max = value; }
  void set_pair_dira_min(const double value) { m_pair_dira_min = value; }
  // Legacy compatibility: enabling the old same-sign switch enables the
  // separate likeSignPairTree and the K0S hypothesis.
  void set_write_same_sign_pairs(const bool value)
  {
    m_write_same_sign_pairs = value;
    if (value)
    {
      m_write_likesign_tree = true;
      m_write_likesign_kshort = true;
    }
  }
  void set_write_likesign_tree(const bool value = true) { m_write_likesign_tree = value; }
  void set_write_likesign_kshort(const bool value = true)
  {
    m_write_likesign_kshort = value;
    if (value) m_write_likesign_tree = true;
  }
  void set_write_likesign_lambda(const bool value = true)
  {
    m_write_likesign_lambda = value;
    if (value) m_write_likesign_tree = true;
  }
  void set_write_likesign_phi(const bool value = true)
  {
    m_write_likesign_phi = value;
    if (value) m_write_likesign_tree = true;
  }
  void set_write_likesign_jpsi(const bool value = true)
  {
    m_write_likesign_jpsi = value;
    if (value) m_write_likesign_tree = true;
  }
  void set_write_likesign_d0(const bool value = true)
  {
    m_write_likesign_d0 = value;
    if (value) m_write_likesign_tree = true;
  }
  void set_likesign_keep_fraction(const double value) { m_likesign_keep_fraction = value; }

  void set_primary_vertex_constraint(const bool value = true)
  {
    m_use_primary_vertex_constraint = value;
  }
  bool set_primary_vertex_constraint_mode(const std::string &mode);
  void set_primary_vertex_constraint_sigmas(const double sigma_xy_cm,
                                             const double sigma_z_cm)
  {
    m_primary_vertex_constraint_sigma_xy_cm = sigma_xy_cm;
    m_primary_vertex_constraint_sigma_z_cm = sigma_z_cm;
  }
  void set_compute_constrained_phi(const bool value = true)
  {
    m_compute_constrained_phi = value;
  }
  void set_compute_constrained_d0(const bool value = true)
  {
    m_compute_constrained_d0 = value;
  }
  void set_use_constrained_phi_selection(const bool value = true)
  {
    m_use_constrained_phi_selection = value;
  }
  void set_use_constrained_d0_selection(const bool value = true)
  {
    m_use_constrained_d0_selection = value;
  }

  void set_write_cluster_residual_tree(const bool value) { m_write_cluster_residual_tree = value; }
  void set_write_track_tree(const bool value = true) { m_write_track_tree = value; }
  void set_write_kshort_daughter_details(const bool value = true)
  {
    m_write_kshort_daughter_details = value;
  }
  void set_kshort_detail_mass_range(const double min_mass, const double max_mass)
  {
    m_kshort_detail_mass_min = min_mass;
    m_kshort_detail_mass_max = max_mass;
  }

  // Dedicated pi-pi mass interval used by the K0S hypothesis in the
  // separate likeSignPairTree. Other species use their own configured windows.
  void set_kshort_mass_range(const double min_mass, const double max_mass)
  {
    m_same_sign_kshort_mass_min = min_mass;
    m_same_sign_kshort_mass_max = max_mass;
  }

  // Kept for macro compatibility. Both momentum definitions are always stored:
  // px1...pz2/v0_* are secondary pair-PCA kinematics; primary_* are evaluated
  // at each daughter's transverse PCA to the configured primary vertex.
  void set_use_primary_vertex_kinematics(const bool value)
  {
    m_use_primary_vertex_kinematics = value;
  }

  // Species-specific final selections. Negative cut values disable that cut.
  void set_kshort_selection(bool enabled, int min_tpc_clusters, double track_pt_min,
                            double mass_min, double mass_max, double pca_z_max,
                            double pca_dz_max, double decay_radius_min,
                            double alpha_abs_max, double pair_dca_max, double dira_min);
  void set_lambda_selection(bool enabled, int min_tpc_clusters, double track_pt_min,
                            double mass_min, double mass_max, double pca_z_max,
                            double pca_dz_max, double decay_radius_min,
                            double alpha_abs_max, double pair_dca_max, double dira_min);
  void set_antilambda_selection(bool enabled, int min_tpc_clusters, double track_pt_min,
                                double mass_min, double mass_max, double pca_z_max,
                                double pca_dz_max, double decay_radius_min,
                                double alpha_abs_max, double pair_dca_max, double dira_min);
  void set_phi_selection(bool enabled, int min_tpc_clusters, double track_pt_min,
                         double mass_min, double mass_max, double track_dca_xy_abs_max,
                         double track_dca_z_abs_max, double pair_dca_max,
                         double flight_length_max, double primary_pca_z_abs_max, double primary_pca_dz_max);
  void set_jpsi_selection(bool enabled, int min_tpc_clusters, double track_pt_min,
                          double mass_min, double mass_max, double track_dca_xy_abs_max,
                          double track_dca_z_abs_max, double pair_dca_max,
                          double flight_length_max, double primary_pca_z_abs_max, double primary_pca_dz_max);
  void set_d0_selection(bool enabled, int min_tpc_clusters, double track_pt_min,
                        double mass_min, double mass_max, double track_dca_xy_abs_max,
                        double track_dca_z_abs_max, double pair_dca_max,
                        double flight_length_max, double primary_pca_z_abs_max, double primary_pca_dz_max);
  void set_antid0_selection(bool enabled, int min_tpc_clusters, double track_pt_min,
                            double mass_min, double mass_max, double track_dca_xy_abs_max,
                            double track_dca_z_abs_max, double pair_dca_max,
                            double flight_length_max, double primary_pca_z_abs_max, double primary_pca_dz_max);

  // Lambda seeds used by Sigma(1385), Xi, and Omega reconstruction.  These are
  // intentionally independent of the inclusive-Lambda pointing cut: a Lambda
  // from a Xi/Omega cascade does not in general point back to the primary
  // vertex.  Negative cut values disable the corresponding requirement.
  void set_hyperon_lambda_seed_selection(int min_tpc_clusters,
                                         double track_pt_min,
                                         double mass_min,
                                         double mass_max,
                                         double pca_z_max,
                                         double pca_dz_max,
                                         double decay_radius_min,
                                         double alpha_abs_max,
                                         double pair_dca_max);

  // Sigma(1385)^+/- -> Lambda pi^+/- (and charge conjugates).  The bachelor is
  // treated as a prompt pion and its momentum is evaluated at the primary PCA.
  void set_sigma1385_selection(bool enabled,
                               int min_bachelor_tpc_clusters,
                               double bachelor_pt_min,
                               double mass_min,
                               double mass_max,
                               double bachelor_dca_xy_abs_max = -1.0,
                               double bachelor_dca_z_abs_max = -1.0);

  // Xi^- -> Lambda pi^- and anti-Xi^+ -> anti-Lambda pi^+.
  void set_xi_selection(bool enabled,
                        int min_bachelor_tpc_clusters,
                        double bachelor_pt_min,
                        double mass_min,
                        double mass_max,
                        double pair_dca_max,
                        double decay_radius_min,
                        double dira_min,
                        double lambda_flight_min = -1.0,
                        double lambda_dira_min = -2.0,
                        double bachelor_dca_xy_min = -1.0);

  // Omega^- -> Lambda K^- and anti-Omega^+ -> anti-Lambda K^+.
  void set_omega_selection(bool enabled,
                           int min_bachelor_tpc_clusters,
                           double bachelor_pt_min,
                           double mass_min,
                           double mass_max,
                           double pair_dca_max,
                           double decay_radius_min,
                           double dira_min,
                           double lambda_flight_min = -1.0,
                           double lambda_dira_min = -2.0,
                           double bachelor_dca_xy_min = -1.0);

  // Search ranges for the charged-bachelor / neutral-Lambda PCA.  The Lambda
  // line starts at the reconstructed V0 decay vertex and is searched upstream
  // (opposite its momentum) toward a possible cascade vertex.
  void set_cascade_search(double bachelor_max_upstream_cm,
                          double bachelor_downstream_margin_cm,
                          double lambda_max_upstream_cm,
                          double lambda_downstream_margin_cm,
                          int coarse_steps = 48,
                          int refine_candidates = 8);

  void set_use_nominal_lambda_mass_for_composites(bool value = true)
  {
    m_use_nominal_lambda_mass_for_composites = value;
  }
  void set_exclude_tpc_transition_layers(const bool value = true)
  {
    m_exclude_tpc_transition_layers = value;
  }
  void set_tpc_region_measurement_sigmas(const double r1_rphi_cm,
                                         const double r1_r_cm,
                                         const double r2_rphi_cm,
                                         const double r2_r_cm,
                                         const double r3_rphi_cm,
                                         const double r3_r_cm,
                                         const double z_cm)
  {
    m_sigma_rphi_r1_cm = r1_rphi_cm;
    m_sigma_r_r1_cm = r1_r_cm;
    m_sigma_rphi_r2_cm = r2_rphi_cm;
    m_sigma_r_r2_cm = r2_r_cm;
    m_sigma_rphi_r3_cm = r3_rphi_cm;
    m_sigma_r_r3_cm = r3_r_cm;
    m_sigma_z_cm = z_cm;
  }
  void set_tpc_transition_measurement_sigmas(const double rphi_cm,
                                             const double r_cm,
                                             const double z_cm)
  {
    m_sigma_rphi_transition_cm = rphi_cm;
    m_sigma_r_transition_cm = r_cm;
    m_sigma_z_transition_cm = z_cm;
  }

 private:
  using Vec3 = TpcTrackVec3;
  using TruthPoint = TpcTrackPoint;
  using HelixFit = TpcTrackHelix;
  using HelixPca = TpcTrackHelixPca;
  using HelixSearchRange = TpcTrackHelixSearchRange;
  using LinePca = TpcTrackLinePca;
  using PointOrder = TpcTrackPointOrder;

  enum class PrimaryVertexConstraintMode
  {
    XY,
    XYZ
  };

  struct Tracklet
  {
    int track_id{0};
    unsigned int source_assembled_track_id{0};
    bool crossing_valid{false};
    short crossing{0};
    unsigned char crossing_status{0};
    int shower_id{0};
    int pid{0};
    int parent_id{0};
    int parent_pid{0};
    int primary_id{0};
    int vtx_id{0};
    int barcode{0};
    int embed_id{0};
    int is_primary{0};
    int charge{0};
    int side{-1};
    int npoints{0};
    unsigned int ntpc_clusters{0};
    bool has_dedx{false};
    double dedx{0.0};
    Vec3 position;
    Vec3 momentum;
    Vec3 truth_momentum;
    double truth_e{0.0};
    Vec3 truth_vertex;
    double truth_vt{0.0};
    std::vector<TruthPoint> points;
    bool has_helix{false};
    HelixFit helix;
    bool has_helix_search_range{false};
    HelixSearchRange helix_search_range;
    bool has_kalman{false};
    TpcKalmanResult kalman;
    double fit_chi2{0.0};
    int fit_ndf{0};
    double fit_chi2_ndf{0.0};
    bool has_vertex_dca{false};
    std::pair<double, double> vertex_dca;
    bool has_beamline_pca{false};
    Vec3 beamline_pca;
    double rdca_zero{0.0};
    bool has_pattern_vertex{false};
    Vec3 pattern_vertex;
    double pattern_vertex_z_rms{0.0};
    unsigned int pattern_vertex_ntracks{0};

    // Optional prompt-track state after a Gaussian primary-vertex update.
    // It is cached once per track/event and reused by every pair candidate.
    bool has_primary_constrained_momentum{false};
    Vec3 primary_constrained_momentum;
    double primary_constraint_chi2{0.0};
    double primary_constraint_path_cm{0.0};

    unsigned int spatial_correction_points{0};
  };

  struct LambdaCandidate
  {
    bool anti{false};
    const Tracklet *daughter1{nullptr};
    const Tracklet *daughter2{nullptr};
    Vec3 decay_vertex;
    Vec3 momentum;
    double mass{0.0};
    double pair_dca{0.0};
    double dira{0.0};
  };

  struct TrackLinePca
  {
    bool valid{false};
    Vec3 track_pca;
    Vec3 line_pca;
    double dca{0.0};
    double track_parameter{0.0};
    double line_path_cm{0.0};
  };

  struct KalmanPca
  {
    Vec3 pca1;
    Vec3 pca2;
    double dca{0.0};
    double s1{0.0};
    double s2{0.0};
  };

  struct DaughterDetailRow
  {
    int track_id{0};
    int charge{0};
    int side{-1};
    int npoints{0};
    unsigned int ntpc_clusters{0};
    float dedx{0.0F};
    float px{0.0F};
    float py{0.0F};
    float pz{0.0F};
    float pt{0.0F};
    float eta{0.0F};
    float fit_chi2{0.0F};
    int fit_ndf{0};
    float fit_chi2_ndf{0.0F};

    std::vector<unsigned int> cluster_index;
    std::vector<int> cluster_side;
    std::vector<unsigned int> layer;
    std::vector<double> cluster_x;
    std::vector<double> cluster_y;
    std::vector<double> cluster_z;
    std::vector<double> cluster_r;
    std::vector<double> cluster_phi;
    std::vector<double> fit_x;
    std::vector<double> fit_y;
    std::vector<double> fit_z;
    std::vector<double> fit_px;
    std::vector<double> fit_py;
    std::vector<double> fit_pz;
    std::vector<double> residual_r;
    std::vector<double> residual_rphi;
    std::vector<double> residual_z;
    std::vector<double> assigned_sigma_r;
    std::vector<double> assigned_sigma_rphi;
    std::vector<double> assigned_sigma_z;
    std::vector<unsigned char> recommended_for_reference_fit;
    std::vector<double> kalman_measurement_chi2;
    std::vector<double> kalman_measurement_chi2_raw;
    std::vector<unsigned char> kalman_measurement_used;
    std::vector<unsigned char> kalman_measurement_rejection_reason;
    std::vector<double> kalman_measurement_weight_scale;
    std::vector<double> kalman_measurement_sigma_r_used;
    std::vector<double> kalman_measurement_sigma_rphi_used;
    std::vector<double> kalman_measurement_sigma_z_used;
  };

  struct SpeciesCuts
  {
    bool enabled{false};
    int min_tpc_clusters{0};
    double track_pt_min{-1.0};
    double mass_min{-1.0};
    double mass_max{-1.0};

    // Secondary-decay topology cuts.
    double pca_z_max{-1.0};
    double pca_dz_max{-1.0};
    double decay_radius_min{-1.0};
    double alpha_abs_max{-1.0};
    double dira_min{-2.0};

    // Shared/prompt topology cuts.
    double track_dca_xy_abs_max{-1.0};
    double track_dca_z_abs_max{-1.0};
    double pair_dca_max{-1.0};
    double flight_length_max{-1.0};

    // Prompt-track PCA to the beam axis.
    double primary_pca_z_abs_max{-1.0};
    double primary_pca_dz_max{-1.0};
  };

  struct SigmaCuts
  {
    bool enabled{false};
    int min_bachelor_tpc_clusters{0};
    double bachelor_pt_min{-1.0};
    double mass_min{-1.0};
    double mass_max{-1.0};
    double bachelor_dca_xy_abs_max{-1.0};
    double bachelor_dca_z_abs_max{-1.0};
  };

  struct CascadeCuts
  {
    bool enabled{false};
    int min_bachelor_tpc_clusters{0};
    double bachelor_pt_min{-1.0};
    double mass_min{-1.0};
    double mass_max{-1.0};
    double pair_dca_max{-1.0};
    double decay_radius_min{-1.0};
    double dira_min{-2.0};
    double lambda_flight_min{-1.0};
    double lambda_dira_min{-2.0};
    double bachelor_dca_xy_min{-1.0};
  };

  enum CascadeCandidateMask : unsigned int
  {
    CandidateXi = 1U << 0,
    CandidateOmega = 1U << 1
  };

  struct SigmaRow
  {
    int run{0};
    int evt{0};
    int is_antilambda{0};
    int sigma_charge{0};
    int lambda_track_id1{0};
    int lambda_track_id2{0};
    int bachelor_track_id{0};
    int bachelor_charge{0};
    int same_crossing{0};
    unsigned int bachelor_ntpc_clusters{0};

    float lambda_mass{0.0F};
    float lambda_pair_dca{0.0F};
    float lambda_dira{0.0F};
    float lambda_decay_x{0.0F};
    float lambda_decay_y{0.0F};
    float lambda_decay_z{0.0F};
    float lambda_px{0.0F};
    float lambda_py{0.0F};
    float lambda_pz{0.0F};
    float lambda_pt{0.0F};

    float bachelor_px{0.0F};
    float bachelor_py{0.0F};
    float bachelor_pz{0.0F};
    float bachelor_pt{0.0F};
    float bachelor_dca_xy{0.0F};
    float bachelor_dca_z{0.0F};
    float bachelor_dedx{0.0F};

    float sigma_px{0.0F};
    float sigma_py{0.0F};
    float sigma_pz{0.0F};
    float sigma_pt{0.0F};
    float mass_sigma1385{0.0F};
  };

  struct CascadeRow
  {
    int run{0};
    int evt{0};
    int is_antilambda{0};
    int cascade_charge{0};
    unsigned int candidate_mask{0};
    int lambda_track_id1{0};
    int lambda_track_id2{0};
    int bachelor_track_id{0};
    int bachelor_charge{0};
    int same_crossing{0};
    unsigned int bachelor_ntpc_clusters{0};

    float lambda_mass{0.0F};
    float lambda_pair_dca{0.0F};
    float lambda_decay_x{0.0F};
    float lambda_decay_y{0.0F};
    float lambda_decay_z{0.0F};
    float lambda_px{0.0F};
    float lambda_py{0.0F};
    float lambda_pz{0.0F};
    float lambda_pt{0.0F};

    float cascade_x{0.0F};
    float cascade_y{0.0F};
    float cascade_z{0.0F};
    float cascade_track_pca_x{0.0F};
    float cascade_track_pca_y{0.0F};
    float cascade_track_pca_z{0.0F};
    float cascade_lambda_pca_x{0.0F};
    float cascade_lambda_pca_y{0.0F};
    float cascade_lambda_pca_z{0.0F};
    float cascade_pair_dca{0.0F};
    float cascade_decay_radius{0.0F};
    float cascade_flight_length{0.0F};
    float cascade_dira{0.0F};
    float lambda_flight_from_cascade{0.0F};
    float lambda_dira_from_cascade{0.0F};
    float bachelor_track_parameter{0.0F};
    float lambda_line_path_cm{0.0F};

    float bachelor_px{0.0F};
    float bachelor_py{0.0F};
    float bachelor_pz{0.0F};
    float bachelor_pt{0.0F};
    float bachelor_dca_xy{0.0F};
    float bachelor_dca_z{0.0F};
    float bachelor_dedx{0.0F};

    float cascade_px{0.0F};
    float cascade_py{0.0F};
    float cascade_pz{0.0F};
    float cascade_pt{0.0F};
    float mass_xi{0.0F};
    float mass_omega{0.0F};
  };

  enum CandidateMask : unsigned int
  {
    CandidateKShort = 1U << 0,
    CandidateLambda = 1U << 1,
    CandidateAntiLambda = 1U << 2,
    CandidatePhi = 1U << 3,
    CandidateD0 = 1U << 4,
    CandidateAntiD0 = 1U << 5,
    CandidateJpsi = 1U << 6
  };

  struct PairRow
  {
    int run{0};
    int evt{0};
    short cross1{0};
    short cross2{0};

    int spatial_correction_applied{0};
    int spatial_correction_z_applied{0};
    float spatial_correction_scale{0.0F};

    float px1{0.0F};
    float py1{0.0F};
    float pz1{0.0F};
    float px2{0.0F};
    float py2{0.0F};
    float pz2{0.0F};

    // Daughter momenta at each track's PCA to the configured primary vertex.
    float primary_px1{0.0F};
    float primary_py1{0.0F};
    float primary_pz1{0.0F};
    float primary_pt1{0.0F};
    float primary_px2{0.0F};
    float primary_py2{0.0F};
    float primary_pz2{0.0F};
    float primary_pt2{0.0F};
    float primary_pair_px{0.0F};
    float primary_pair_py{0.0F};
    float primary_pair_pz{0.0F};
    float primary_pair_pt{0.0F};

    // Optional Gaussian primary-vertex-constrained states. The ordinary
    // primary_* variables above remain unchanged and unconstrained.
    int primary_constrained_valid1{0};
    int primary_constrained_valid2{0};
    float primary_constrained_chi2_1{0.0F};
    float primary_constrained_chi2_2{0.0F};
    float primary_constrained_path_cm1{0.0F};
    float primary_constrained_path_cm2{0.0F};
    float primary_constrained_px1{0.0F};
    float primary_constrained_py1{0.0F};
    float primary_constrained_pz1{0.0F};
    float primary_constrained_pt1{0.0F};
    float primary_constrained_px2{0.0F};
    float primary_constrained_py2{0.0F};
    float primary_constrained_pz2{0.0F};
    float primary_constrained_pt2{0.0F};
    float primary_constrained_pair_px{0.0F};
    float primary_constrained_pair_py{0.0F};
    float primary_constrained_pair_pz{0.0F};
    float primary_constrained_pair_pt{0.0F};

    // Independent daughter PCAs to the configured beam axis.
    float primary_pca1_x{0.0F};
    float primary_pca1_y{0.0F};
    float primary_pca1_z{0.0F};
    float primary_pca2_x{0.0F};
    float primary_pca2_y{0.0F};
    float primary_pca2_z{0.0F};
    float primary_pca_dz{0.0F};
    int primary_pca_valid{0};

    // Local two-track PCA seeded around the two beam-axis PCAs.
    float prompt_pca_x{0.0F};
    float prompt_pca_y{0.0F};
    float prompt_pca_z{0.0F};
    float prompt_pca1_x{0.0F};
    float prompt_pca1_y{0.0F};
    float prompt_pca1_z{0.0F};
    float prompt_pca2_x{0.0F};
    float prompt_pca2_y{0.0F};
    float prompt_pca2_z{0.0F};
    float prompt_pairDCA{0.0F};
    int prompt_pca_valid{0};

    float dca_xy1{0.0F};
    float dca_z1{0.0F};
    float dca_xy2{0.0F};
    float dca_z2{0.0F};
    float pairDCA{0.0F};

    float alpha{0.0F};
    float qT{0.0F};
    float charge1{0.0F};
    float charge2{0.0F};
    int pair_charge{0};
    int charge_product{0};
    float dedx_1{0.0F};
    float dedx_2{0.0F};
    float cosThetaReco{0.0F};
    float Lproj{0.0F};

    float pca_x{0.0F};
    float pca_y{0.0F};
    float pca_z{0.0F};
    float pca1_x{0.0F};
    float pca1_y{0.0F};
    float pca1_z{0.0F};
    float pca2_x{0.0F};
    float pca2_y{0.0F};
    float pca2_z{0.0F};

    float v0_px{0.0F};
    float v0_py{0.0F};
    float v0_pz{0.0F};
    float v0_pt{0.0F};
    float mass_Kshort{0.0F};
    float mass_Lambda{0.0F};
    float mass_AntiLambda{0.0F};
    float mass_Phi{0.0F};
    float mass_Jpsi{0.0F};
    float mass_D0{0.0F};
    float mass_AntiD0{0.0F};

    // Explicit daughter-order hypotheses, especially useful for like-sign
    // backgrounds where charge cannot identify the proton/kaon daughter.
    float mass_P1Pi2{0.0F};
    float mass_Pi1P2{0.0F};
    float mass_K1Pi2{0.0F};
    float mass_Pi1K2{0.0F};

    float mass_Phi_primary_constrained{0.0F};
    float mass_D0_primary_constrained{0.0F};
    float mass_AntiD0_primary_constrained{0.0F};
    float mass_K1Pi2_primary_constrained{0.0F};
    float mass_Pi1K2_primary_constrained{0.0F};
    unsigned int candidate_mask{0};

    float true_decay_x{0.0F};
    float true_decay_y{0.0F};
    float true_decay_z{0.0F};
    float pca_to_true_3d{0.0F};
    float pca_to_true_xy{0.0F};
    float pca_to_true_z{0.0F};
    float truth_alpha{0.0F};
    float truth_qT{0.0F};
    float delta_alpha{0.0F};
    float delta_qT{0.0F};
    float truth_px1{0.0F};
    float truth_py1{0.0F};
    float truth_pz1{0.0F};
    float truth_px2{0.0F};
    float truth_py2{0.0F};
    float truth_pz2{0.0F};
    float cos_mom1_truth{0.0F};
    float cos_mom2_truth{0.0F};
    float pca_theta1{0.0F};
    float pca_theta2{0.0F};
    float kalman_chi2_1{0.0F};
    float kalman_chi2_2{0.0F};
    float kalman_chi2_ndf1{0.0F};
    float kalman_chi2_ndf2{0.0F};
    float quality1{0.0F};
    float quality2{0.0F};

    int track_id1{0};
    int track_id2{0};
    unsigned int source_assembled_track_id1{0};
    unsigned int source_assembled_track_id2{0};
    int crossing_valid1{0};
    int crossing_valid2{0};
    short crossing1{0};
    short crossing2{0};
    int same_crossing{0};
    int pid1{0};
    int pid2{0};
    int parent_id1{0};
    int parent_id2{0};
    int parent_pid{0};
    int kalman_ndof1{0};
    int kalman_ndof2{0};
    short npoints1{0};
    short npoints2{0};
    unsigned int ntpc_clusters1{0};
    unsigned int ntpc_clusters2{0};

    int has_kshort_daughter_details{0};
    DaughterDetailRow daughter1;
    DaughterDetailRow daughter2;
  };

  struct TrackRow
  {
    int run{0};
    int evt{0};
    int track_id{0};
    unsigned int source_assembled_track_id{0};
    int crossing_valid{0};
    short crossing{0};
    int crossing_status{0};
    int shower_id{0};
    int pid{0};
    int parent_id{0};
    int parent_pid{0};
    double charge{0.0};
    int side{-1};
    int npoints{0};
    unsigned int ntpc_clusters{0};
    int has_helix{0};
    int has_kalman{0};
    int is_primary{0};
    int spatial_correction_applied{0};
    int spatial_correction_z_applied{0};
    unsigned int spatial_correction_points{0};
    float spatial_correction_scale{0.0F};

    double px{0.0};
    double py{0.0};
    double pz{0.0};
    double pt{0.0};
    double p{0.0};
    double eta{0.0};
    double dedx{0.0};
    float x{0.0F};
    float y{0.0F};
    float z{0.0F};

    float first_x{0.0F};
    float first_y{0.0F};
    float first_z{0.0F};
    float first_r{0.0F};
    float last_x{0.0F};
    float last_y{0.0F};
    float last_z{0.0F};
    float last_r{0.0F};

    float dca_xy{0.0F};
    float dca_z{0.0F};
    double vertex_x{0.0};
    double vertex_y{0.0};
    double vertex_z{0.0};
    int vertex_from_upstream{0};
    double vertex_z_rms{0.0};
    unsigned int vertex_ntracks{0};
    double pca_x{0.0};
    double pca_y{0.0};
    double pca_z{0.0};
    double rDCA_zero{0.0};
    double zDCA{0.0};

    float helix_cx{0.0F};
    float helix_cy{0.0F};
    float helix_radius{0.0F};
    float helix_z0{0.0F};
    float helix_pitch{0.0F};
    float helix_theta_first{0.0F};
    float helix_theta_last{0.0F};
    float helix_direction{0.0F};
    int helix_search_anchored{0};
    int helix_anchor_point_index{-1};
    float helix_anchor_theta{0.0F};
    float helix_anchor_path_cm{0.0F};
    float helix_anchor_residual_cm{0.0F};
    float helix_search_theta_min{0.0F};
    float helix_search_theta_max{0.0F};
    float helix_search_upstream_cm{0.0F};
    float helix_search_downstream_cm{0.0F};

    float kalman_chi2{0.0F};
    int kalman_ndof{0};
    unsigned int kalman_naccepted{0};
    unsigned int kalman_nrejected{0};
    float kalman_measurement_sigma_r{0.0F};
    float kalman_measurement_sigma_rphi{0.0F};
    float kalman_measurement_sigma_z{0.0F};
    float kalman_qop_t{0.0F};
    float kalman_omega{0.0F};
    float kalman_cx{0.0F};
    float kalman_cy{0.0F};
    float kalman_radius{0.0F};
    float fit_chi2{0.0F};
    int fit_ndf{0};
    float quality{0.0F};

    float truth_px{0.0F};
    float truth_py{0.0F};
    float truth_pz{0.0F};
    float cos_mom_truth{0.0F};

    std::vector<unsigned int> cluster_index;
    std::vector<int> cluster_side;
    std::vector<unsigned int> layer;
    std::vector<double> cluster_z;
    std::vector<double> cluster_r;
    std::vector<double> cluster_phi;
    std::vector<double> residual_z;
    std::vector<double> residual_r;
    std::vector<double> residual_rphi;
    std::vector<double> kalman_measurement_chi2;
    std::vector<double> kalman_measurement_chi2_raw;
    std::vector<unsigned char> kalman_measurement_used;
    std::vector<unsigned char> kalman_measurement_rejection_reason;
    std::vector<double> kalman_measurement_weight_scale;
    std::vector<double> kalman_measurement_sigma_r_used;
    std::vector<double> kalman_measurement_sigma_rphi_used;
    std::vector<double> kalman_measurement_sigma_z_used;
    std::vector<unsigned char> kalman_measurement_in_seed;
    std::vector<double> kalman_innovation_residual_r;
    std::vector<double> kalman_innovation_residual_rphi;
    std::vector<double> kalman_innovation_residual_z;
    std::vector<double> kalman_prediction_sigma_r;
    std::vector<double> kalman_prediction_sigma_rphi;
    std::vector<double> kalman_prediction_sigma_z;
    std::vector<double> kalman_innovation_sigma_r;
    std::vector<double> kalman_innovation_sigma_rphi;
    std::vector<double> kalman_innovation_sigma_z;
    std::vector<double> kalman_innovation_rho_r_rphi;
    std::vector<double> kalman_innovation_rho_r_z;
    std::vector<double> kalman_innovation_rho_rphi_z;
    std::vector<double> kalman_innovation_whitened_0;
    std::vector<double> kalman_innovation_whitened_1;
    std::vector<double> kalman_innovation_whitened_2;
  };

  struct ClusterResidualRow
  {
    int run{0};
    int evt{0};
    int track_id{0};
    int charge{0};
    int side{0};
    int layer{0};
    int cluster_index{0};
    int ntp_cluster{0};
    int npoints{0};
    int has_helix{0};
    int has_kalman{0};

    float cluster_x{0.0F};
    float cluster_y{0.0F};
    float cluster_z{0.0F};
    float cluster_r{0.0F};
    float cluster_phi{0.0F};

    float fit_x{0.0F};
    float fit_y{0.0F};
    float fit_z{0.0F};
    float fit_r{0.0F};
    float fit_phi{0.0F};

    float residual_x{0.0F};
    float residual_y{0.0F};
    float residual_z{0.0F};
    float residual_r{0.0F};
    float residual_rphi{0.0F};

    float fit_chi2{0.0F};
    int fit_ndf{0};
    float fit_chi2_ndf{0.0F};
  };

  int get_event_number(PHCompositeNode *topNode) const;
  int get_run_number(PHCompositeNode *topNode) const;
  Vec3 get_primary_vertex(PHG4TruthInfoContainer *truth_info) const;
  std::map<int, Tracklet> build_tracklets(PHG4HitContainer *truth_points,
                                          PHG4TruthInfoContainer *truth_info) const;
  std::map<int, Tracklet> build_pattern_tracklets(Tpc_PolyClusterContainer *clusters,
                                                  Tpc_PolyTrackContainer *tracks,
                                                  const TpcCrossingDecisionContainer *crossing_decisions) const;
  bool finalize_pattern_tracklet(Tracklet &tracklet, bool has_upstream_state) const;
  bool make_pair_row(const Tracklet &track1, const Tracklet &track2,
                     const Vec3 &primary_vertex, const int run_number,
                     const int event_number,
                     std::vector<LambdaCandidate> *lambda_candidates = nullptr);
  void reconstruct_sigma_candidates(const std::vector<LambdaCandidate> &lambdas,
                                    const std::vector<const Tracklet *> &tracklets,
                                    const Vec3 &primary_vertex,
                                    int run_number,
                                    int event_number);
  void reconstruct_cascade_candidates(const std::vector<LambdaCandidate> &lambdas,
                                      const std::vector<const Tracklet *> &tracklets,
                                      const Vec3 &primary_vertex,
                                      int run_number,
                                      int event_number);
  bool track_line_pca(const Tracklet &tracklet,
                      const Vec3 &line_origin,
                      const Vec3 &line_direction,
                      const Vec3 &reference_vertex,
                      TrackLinePca &pca,
                      Vec3 &track_momentum) const;
  bool helix_line_pca(const Tracklet &tracklet,
                      const Vec3 &line_origin,
                      const Vec3 &line_direction,
                      TrackLinePca &pca,
                      Vec3 &track_momentum) const;
  bool kalman_line_pca(const Tracklet &tracklet,
                       const Vec3 &line_origin,
                       const Vec3 &line_direction,
                       const Vec3 &reference_vertex,
                       TrackLinePca &pca,
                       Vec3 &track_momentum) const;
  bool line_track_line_pca(const Tracklet &tracklet,
                           const Vec3 &line_origin,
                           const Vec3 &line_direction,
                           TrackLinePca &pca,
                           Vec3 &track_momentum) const;
  bool same_crossing(const LambdaCandidate &lambda, const Tracklet &bachelor) const;
  bool passes_sigma_selection(const LambdaCandidate &lambda,
                              const Tracklet &bachelor,
                              const std::pair<double, double> &bachelor_dca,
                              const Vec3 &bachelor_momentum,
                              double mass) const;
  bool passes_cascade_selection(const CascadeCuts &cuts,
                                const Tracklet &bachelor,
                                const std::pair<double, double> &bachelor_dca,
                                const TrackLinePca &pca,
                                const Vec3 &cascade_vertex,
                                const Vec3 &primary_vertex,
                                const Vec3 &cascade_momentum,
                                const Vec3 &lambda_momentum,
                                const Vec3 &lambda_decay_vertex,
                                double mass) const;
  void fill_track_row(const Tracklet &tracklet, const Vec3 &primary_vertex,
                      int run_number, int event_number);
  void fill_daughter_detail_row(const Tracklet &tracklet, DaughterDetailRow &row) const;
  void create_daughter_detail_branches(TTree *tree,
                                       const std::string &prefix,
                                       DaughterDetailRow &row);
  bool is_tpc_transition_layer(int layer) const;
  void measurement_sigmas_for_layer(int layer, double &sigma_rphi_cm,
                                    double &sigma_r_cm, double &sigma_z_cm) const;
  void assign_point_measurement_metadata(TruthPoint &point,
                                         std::size_t original_index) const;
  void fill_cluster_residual_rows(const Tracklet &tracklet, const Vec3 &primary_vertex,
                                  int run_number, int event_number);
  bool track_pca_to_xy(const Tracklet &tracklet, const Vec3 &beamline,
                       Vec3 &pca, double &signed_dca_xy,
                       Vec3 *momentum_at_pca = nullptr,
                       double *path_at_pca_cm = nullptr) const;
  bool choose_pattern_collision_vertex(const Tracklet &tracklet,
                                       Tpc_PolyTrackVertexContainer *vertices,
                                       Vec3 &vertex, double &z_rms,
                                       unsigned int &ntracks) const;
  void assign_fit_quality(Tracklet &tracklet) const;
  void reset_pair_row();
  void reset_sigma_row();
  void reset_cascade_row();
  void reset_track_row();
  void reset_cluster_residual_row();
  void create_branches();
  void create_pair_branches(TTree *tree);
  void create_sigma_branches();
  void create_cascade_branches();
  bool primary_vertex_constrained_momentum(const Tracklet &tracklet,
                                           const Vec3 &primary_vertex,
                                           Vec3 &momentum,
                                           double &chi2,
                                           double &path_cm) const;

  bool load_spatial_correction_map();
  void close_spatial_correction_map();
  unsigned int apply_spatial_correction(std::vector<TruthPoint> &points,
                                        int side) const;
  bool evaluate_spatial_correction(int side, double r, double phi, double z,
                                   Vec3 &correction) const;

  static int pdg_charge(int pid);
  static bool parse_point_order(const std::string &mode, PointOrder &order);
  static float quiet_nan();
  static bool finite(const Vec3 &value);
  static Vec3 add(const Vec3 &lhs, const Vec3 &rhs);
  static Vec3 subtract(const Vec3 &lhs, const Vec3 &rhs);
  static Vec3 scale(const Vec3 &value, double factor);
  static double dot(const Vec3 &lhs, const Vec3 &rhs);
  static Vec3 cross(const Vec3 &lhs, const Vec3 &rhs);
  static double norm(const Vec3 &value);
  static Vec3 unit(const Vec3 &value);
  static double pt(const Vec3 &value);
  static double distance(const Vec3 &lhs, const Vec3 &rhs);
  static double vector_cosine(const Vec3 &lhs, const Vec3 &rhs);
  static bool fit_circle_least_squares(const std::vector<TruthPoint> &points,
                                       std::size_t nfit,
                                       double &cx,
                                       double &cy,
                                       double &radius);
  static void order_track_points(std::vector<TruthPoint> &points, PointOrder order);

  static bool fit_helix(const std::vector<TruthPoint> &points, int fit_first_points,
                        int charge, double bfield_t, HelixFit &helix);
  bool fit_kalman(const std::vector<TruthPoint> &points,
                  int charge,
                  TpcKalmanResult &kalman) const;
  static bool helix_from_state(const Vec3 &position, const Vec3 &momentum,
                               int charge, double bfield_t, HelixFit &helix);
  static Vec3 helix_point(const HelixFit &helix, double theta);
  static Vec3 helix_tangent(const HelixFit &helix, double theta);
  static Vec3 helix_momentum(const HelixFit &helix, double theta);
  static std::pair<double, double> theta_search_range(const HelixFit &helix,
                                                      double theta_extension,
                                                      double downstream_margin);
  static bool line_line_pca(const Vec3 &pos1, const Vec3 &dir1,
                            const Vec3 &pos2, const Vec3 &dir2,
                            LinePca &pca, bool normalize_dirs);
  static HelixPca refine_helix_pair(const HelixFit &helix1, const HelixFit &helix2,
                                    double theta1, double theta2,
                                    double min1, double max1,
                                    double min2, double max2,
                                    double max_step);
  static std::vector<HelixPca> helix_helix_pca_candidates(const HelixFit &helix1,
                                                          const HelixFit &helix2,
                                                          double theta_extension,
                                                          int coarse_steps,
                                                          double downstream_margin,
                                                          int max_candidates);
  static Vec3 kalman_point(const TpcKalmanResult &kalman,
                           double s_cm,
                           const TpcKalmanConfig &config,
                           const Vec3 &reference_vertex);
  static Vec3 kalman_tangent(const TpcKalmanResult &kalman,
                             double s_cm,
                             const TpcKalmanConfig &config,
                             const Vec3 &reference_vertex);
  static Vec3 kalman_momentum(const TpcKalmanResult &kalman,
                              double s_cm,
                              const TpcKalmanConfig &config,
                              const Vec3 &reference_vertex);
  static KalmanPca refine_kalman_pair(const TpcKalmanResult &kalman1,
                                      const TpcKalmanResult &kalman2,
                                      const TpcKalmanConfig &config,
                                      const Vec3 &reference_vertex,
                                      double s1, double s2,
                                      double min1, double max1,
                                      double min2, double max2,
                                      double max_step,
                                      int max_iterations = 30);
  static std::vector<KalmanPca> kalman_pca_candidates(const TpcKalmanResult &kalman1,
                                                      const TpcKalmanResult &kalman2,
                                                      const TpcKalmanConfig &config,
                                                      const Vec3 &reference_vertex,
                                                      double max_upstream_cm,
                                                      double downstream_margin_cm,
                                                      int coarse_steps,
                                                      int max_candidates);
  static std::pair<double, double> track_dca_to_vertex(const Vec3 &pos,
                                                       const Vec3 &mom,
                                                       const Vec3 &vertex);
  static std::pair<double, double> helix_dca_to_vertex(const HelixFit &helix,
                                                       const Vec3 &vertex);
  std::pair<double, double> fitted_track_dca_to_vertex(const Tracklet &tracklet,
                                                       const Vec3 &vertex) const;
  static bool armenteros(const Vec3 &pplus, const Vec3 &pminus,
                         double &alpha, double &qt);
  static double invariant_mass(const Vec3 &mom1, double mass1,
                               const Vec3 &mom2, double mass2);

  bool passes_preselection(const Tracklet &track1, const Tracklet &track2,
                           const Vec3 &primary_vertex) const;
  bool passes_pair_selection(const Vec3 &pca1, const Vec3 &pca2,
                             const Vec3 &pair_vertex, const Vec3 &primary_vertex,
                             double pair_dca, double cos_theta, double alpha) const;
  bool passes_species_selection(const SpeciesCuts &cuts,
                                const Tracklet &track1, const Tracklet &track2,
                                const Vec3 &pca1, const Vec3 &pca2,
                                const Vec3 &pair_vertex,
                                const Vec3 &primary_vertex,
                                const Vec3 &primary_pca1,
                                const Vec3 &primary_pca2,
                                const Vec3 &primary_mom1,
                                const Vec3 &primary_mom2,
                                bool have_primary_pair,
                                const std::pair<double, double> &dca1,
                                const std::pair<double, double> &dca2,
                                double pair_dca, double cos_theta, double alpha,
                                double mass, bool secondary_topology) const;

  std::string m_filename;
  std::string m_truth_point_node{"G4HIT_TPC_TRUECLUSTER"};
  std::string m_truth_info_node{"G4TruthInfo"};
  std::string m_tpc_sa_cluster_node{"TPC_POLYCLUSTERS"};
  std::string m_tpc_sa_track_node{"TPC_POLYTRACKS"};
  std::string m_tpc_sa_track_vertex_node{"TPC_POLYTRACKVERTICES"};
  std::string m_tpc_crossing_decision_node{"TPC_CROSSING_DECISIONS"};
  bool m_use_pattern_cluster_tracks{false};

  bool m_apply_spatial_correction{false};
  bool m_apply_spatial_correction_z{false};
  double m_spatial_correction_scale{1.0};
  std::string m_spatial_correction_filename;
  TFile *m_spatial_correction_file{nullptr};
  TH3 *m_spatial_delta_r[2]{nullptr, nullptr};
  TH3 *m_spatial_rdelta_phi[2]{nullptr, nullptr};
  TH3 *m_spatial_delta_z[2]{nullptr, nullptr};

  TFile *m_file{nullptr};
  TTree *m_pair_tree{nullptr};
  TTree *m_like_sign_pair_tree{nullptr};
  TTree *m_sigma_tree{nullptr};
  TTree *m_cascade_tree{nullptr};
  TTree *m_track_tree{nullptr};
  TTree *m_cluster_residual_tree{nullptr};
  PairRow m_pair;
  SigmaRow m_sigma;
  CascadeRow m_cascade;
  TrackRow m_track;
  ClusterResidualRow m_cluster_residual;

  Vec3 m_fixed_primary_vertex{0.0, 0.0, 0.0};
  bool m_use_truth_primary_vertex{true};
  int m_min_points{5};
  bool m_fit_helix_tracks{true};
  bool m_fit_kalman_tracks{false};
  bool m_use_final_track_helix{false};
  PointOrder m_point_order{PointOrder::Path};
  int m_fit_first_points{8};
  double m_bfield_t{1.4};
  TpcKalmanConfig m_kalman_config;
  std::unique_ptr<PHField> m_owned_kalman_field;
  bool m_use_kalman_field_map{true};
  double m_kalman_max_upstream_cm{80.0};
  double m_kalman_downstream_margin_cm{5.0};
  double m_theta_extension{2.0};
  int m_coarse_steps{64};
  int m_pca_candidates{32};
  double m_downstream_margin{0.2};
  double m_final_track_helix_max_upstream_cm{80.0};
  double m_final_track_helix_downstream_margin_cm{5.0};
  bool m_prefer_positive_pointing{false};

  bool m_use_primary_vertex_constraint{false};
  PrimaryVertexConstraintMode m_primary_vertex_constraint_mode{PrimaryVertexConstraintMode::XY};
  double m_primary_vertex_constraint_sigma_xy_cm{0.05};
  double m_primary_vertex_constraint_sigma_z_cm{0.05};
  bool m_compute_constrained_phi{true};
  bool m_compute_constrained_d0{true};
  bool m_use_constrained_phi_selection{false};
  bool m_use_constrained_d0_selection{false};

  bool m_write_cluster_residual_tree{false};
  bool m_write_track_tree{false};
  bool m_require_same_crossing{true};
  bool m_write_kshort_daughter_details{false};
  bool m_exclude_tpc_transition_layers{true};

  // TPC layer convention: R1=7..22, R2=23..38, R3=39..54.
  // The four module-boundary layers 22, 23, 38, and 39 are marked
  // as not recommended for the future reference fit by default.
  double m_kshort_detail_mass_min{0.45};
  double m_kshort_detail_mass_max{0.55};
  double m_sigma_rphi_r1_cm{0.04};  // 400 um
  double m_sigma_r_r1_cm{0.20};     // 2 mm
  double m_sigma_rphi_r2_cm{0.025}; // 250 um
  double m_sigma_r_r2_cm{0.05};     // 0.5 mm
  double m_sigma_rphi_r3_cm{0.025}; // 250 um
  double m_sigma_r_r3_cm{0.20};     // 2 mm
  double m_sigma_z_cm{0.10};
  double m_sigma_rphi_transition_cm{0.20};
  double m_sigma_r_transition_cm{0.50};
  double m_sigma_z_transition_cm{0.20};

  SpeciesCuts m_kshort_cuts;
  SpeciesCuts m_lambda_cuts;
  SpeciesCuts m_antilambda_cuts;
  SpeciesCuts m_phi_cuts;
  SpeciesCuts m_jpsi_cuts;
  SpeciesCuts m_d0_cuts;
  SpeciesCuts m_antid0_cuts;
  SpeciesCuts m_hyperon_lambda_seed_cuts;
  SigmaCuts m_sigma1385_cuts;
  CascadeCuts m_xi_cuts;
  CascadeCuts m_omega_cuts;

  bool m_use_nominal_lambda_mass_for_composites{true};
  double m_cascade_bachelor_max_upstream_cm{80.0};
  double m_cascade_bachelor_downstream_margin_cm{5.0};
  double m_cascade_lambda_max_upstream_cm{80.0};
  double m_cascade_lambda_downstream_margin_cm{1.0};
  int m_cascade_coarse_steps{48};
  int m_cascade_refine_candidates{8};

  double m_pre_track_pt_min{0.2};
  double m_pre_track_dca_xy_min{0.03};
  double m_pre_track_dca_z_min{-1.0};
  double m_pre_track_dca_xy_max{-1.0};
  double m_pre_track_dca_z_max{-1.0};
  double m_pre_pair_dca_max{5.0};
  double m_pre_lproj_min{0.2};
  double m_pre_cos_theta_min{-2.0};
  double m_pre_track_quality_max{-1.0};
  int m_pre_track_npoints_min{0};
  double m_pair_pca_z_max{-1.0};
  double m_pair_pca_dz_max{-1.0};
  double m_pair_decay_radius_min{-1.0};
  double m_pair_alpha_abs_max{-1.0};
  double m_pair_dca_max{-1.0};
  double m_pair_dira_min{-2.0};
  bool m_write_same_sign_pairs{false};
  bool m_write_likesign_tree{false};
  bool m_write_likesign_kshort{false};
  bool m_write_likesign_lambda{false};
  bool m_write_likesign_phi{false};
  bool m_write_likesign_jpsi{false};
  bool m_write_likesign_d0{false};
  double m_likesign_keep_fraction{1.0};
  bool m_use_primary_vertex_kinematics{false};
  double m_same_sign_kshort_mass_min{0.40};
  double m_same_sign_kshort_mass_max{0.60};
  bool m_print_timing{false};

  std::uint64_t m_counter_raw_pairs{0};
  std::uint64_t m_counter_reject_charge{0};
  std::uint64_t m_counter_reject_preselection{0};
  std::uint64_t m_counter_reject_pca{0};
  std::uint64_t m_counter_reject_pointing{0};
  std::uint64_t m_counter_reject_ap{0};
  std::uint64_t m_counter_reject_pair_selection{0};
  std::uint64_t m_counter_written{0};
  std::uint64_t m_counter_likesign_written{0};
  std::uint64_t m_counter_likesign_downsampled{0};
  std::uint64_t m_counter_tracks_written{0};
  std::uint64_t m_counter_sigma_written{0};
  std::uint64_t m_counter_cascade_written{0};
  std::uint64_t m_counter_xi_written{0};
  std::uint64_t m_counter_omega_written{0};
  std::uint64_t m_counter_cluster_residuals_written{0};
  mutable std::uint64_t m_counter_spatial_points_seen{0};
  mutable std::uint64_t m_counter_spatial_points_corrected{0};
  mutable std::uint64_t m_counter_spatial_points_outside{0};
  mutable std::uint64_t m_counter_spatial_points_invalid{0};
  mutable std::uint64_t m_counter_reject_helix_anchor{0};
  std::uint64_t m_timing_events{0};
  mutable std::uint64_t m_timing_kalman_fits{0};
  mutable std::uint64_t m_timing_rkn_propagations{0};
  mutable std::uint64_t m_timing_rkn_accepted_steps{0};
  mutable std::uint64_t m_timing_rkn_rejected_trials{0};
  mutable std::uint64_t m_timing_rkn_failures{0};
  double m_timing_total_seconds{0.0};
  double m_timing_track_build_seconds{0.0};
  mutable double m_timing_kalman_fit_seconds{0.0};
  mutable double m_timing_rkn_seconds{0.0};
  double m_timing_track_qa_seconds{0.0};
  double m_timing_dca_cache_seconds{0.0};
  double m_timing_pair_loop_seconds{0.0};
  double m_timing_kalman_pca_seconds{0.0};
};

#endif

#ifndef TPCCALIB_GLOBALFIELDFITTER_H
#define TPCCALIB_GLOBALFIELDFITTER_H

#include <array>
#include <vector>

// A field observation is std::array<double, 6>.
// It represents one measured/reference stripe displacement used in the fit.
inline constexpr int observation_phi = 0;
inline constexpr int observation_r = 1;
inline constexpr int observation_delta_r = 2;
inline constexpr int observation_r_delta_phi = 3;
inline constexpr int observation_sigma_delta_r = 4;
inline constexpr int observation_sigma_r_delta_phi = 5;

// A fitted control point is std::array<double, 7>.
// The last entry is stored as 0.0 or 1.0 because this version avoids custom
// record objects and keeps everything in basic STL containers.
inline constexpr int control_point_phi = 0;
inline constexpr int control_point_r = 1;
inline constexpr int control_point_delta_r = 2;
inline constexpr int control_point_r_delta_phi = 3;
inline constexpr int control_point_delta_phi = 4;
inline constexpr int control_point_support = 5;
inline constexpr int control_point_active = 6;

class GlobalFieldFitter {
public:
  // observations: each entry is indexed with observation_* above.
  // control_r_positions: optional radial grid from lamination gap centers.
  explicit GlobalFieldFitter(const std::vector<std::array<double, 6>> &observations, const std::vector<double> &control_r_positions = {});

  // Fits DeltaR and R*DeltaPhi on one shared phi/R control grid.
  bool fit();

  // True only after fit() succeeds and coefficient arrays are usable.
  bool is_valid() const;

  // Evaluate the fitted distortion components at any phi/R point.
  double evaluate_delta_r(double phi, double r) const;
  double evaluate_r_delta_phi(double phi, double r) const;
  double evaluate_delta_phi(double phi, double r) const;

  // Returns robust global fallback uncertainties from the fitted residuals.
  double predictive_sigma_delta_r(double phi, double r) const;
  double predictive_sigma_r_delta_phi(double phi, double r) const;

  // Returns std::array<double, 7> points indexed with control_point_*.
  std::vector<std::array<double, 7>> control_points() const;

private:
  // Build the rectangular phi/R control grid before solving.
  void initialize_grid();

  // Converts a 2D control-grid coordinate into a flat vector index.
  int control_index(int phi_index, int r_index) const;

  // Finds the four surrounding controls and bilinear weights for one point.
  void accumulate_bilinear_weights(double phi, double r, std::vector<int> &indices, std::vector<double> &weights) const;

  // Adds finite-difference smoothing terms to the normal equations.
  void add_smoothness_penalty(std::vector<std::vector<double>> &normal_matrix, const std::vector<int> &full_to_fit_index) const;

  // Plain Gaussian elimination solver for the small dense normal equations.
  bool solve_linear_system(std::vector<std::vector<double>> matrix, std::vector<double> rhs, std::vector<double> &solution) const;

  // Evaluates one fitted component from a coefficient vector.
  double evaluate_component(const std::vector<double> &coefficients, double phi, double r) const;

  // Counts how much observation weight reaches each control point.
  void compute_control_support();

  // Chooses active controls when sparse interpolation is enabled.
  void select_active_controls(std::vector<int> &full_to_fit_index);

  // Fills inactive controls from neighboring active controls for evaluation.
  void build_dense_evaluation_grid();

  // Control-grid coordinate helpers.
  double control_phi(int phi_index) const;
  double control_r(int r_index) const;

  // Interpolates a value at an inactive control from nearby active controls.
  double interpolate_active_value(const std::vector<double> &coefficients, double phi, double r) const;

  // Robust residual scale used when writing uncertainty diagnostics.
  double estimate_global_residual_sigma(bool fit_delta_r) const;

  // Raw fitted observations indexed by observation_*.
  std::vector<std::array<double, 6>> m_observations;

  // Compact fitted coefficient vectors. These may contain only active controls.
  std::vector<double> m_coefficients_delta_r;
  std::vector<double> m_coefficients_r_delta_phi;

  // Dense coefficient vectors used for fast evaluation everywhere on the grid.
  std::vector<double> m_evaluation_coefficients_delta_r;
  std::vector<double> m_evaluation_coefficients_r_delta_phi;

  // Per-control support and active flags.
  std::vector<double> m_controlSupport;
  std::vector<bool> m_activeControl;

  // Control-grid dimensions and numeric ranges.
  int m_nControlPhi = 0;
  int m_nControlR = 0;
  double m_phiMin = 0.0;
  double m_phiMax = 0.0;
  double m_phiStep = 0.0;
  double m_rMin = 0.0;
  double m_rMax = 0.0;
  double m_rStep = 0.0;

  // Requested radial controls from the caller and final sorted usable controls.
  std::vector<double> m_requestedControlRPositions;
  std::vector<double> m_controlRPositions;

  // Set true only after both DeltaR and R*DeltaPhi solves succeed.
  bool m_isValid = false;
};

#endif

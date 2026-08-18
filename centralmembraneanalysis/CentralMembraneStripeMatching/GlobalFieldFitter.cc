#include "GlobalFieldFitter.h"

#include "helpers.h"
#include "parameters.h"

#include <algorithm>
#include <cmath>
#include <utility>
#include <array>
#include <vector>

bool nearly_same_control_r(double lhs, double rhs)
{
  return std::abs(lhs - rhs) < 1e-3;
}

bool smaller_neighbor_distance(const std::pair<double, double> &lhs, const std::pair<double, double> &rhs)
{
  return lhs.first < rhs.first;
}

// GlobalFieldFitter represents DeltaR and R*DeltaPhi as two independent scalar
// fields on the same periodic (phi, R) control grid. Observations contribute
// bilinearly to nearby control points, while second-difference penalties keep
// the fitted surfaces smooth between measured stripes.
GlobalFieldFitter::GlobalFieldFitter(const std::vector<std::array<double, 6>> &observations, const std::vector<double> &control_r_positions) : m_observations(observations), m_requestedControlRPositions(control_r_positions) {}

// Build and solve the two regularized weighted-least-squares systems.
//
// The coefficient layout may be sparse when active-control interpolation is
// enabled. In that case only controls with sufficient data support participate
// in the linear solve; build_dense_evaluation_grid() reconstructs values at
// inactive controls afterward for fast, continuous evaluation.
bool GlobalFieldFitter::fit()
{
  // A fitter can be reused, so remove every product of a previous fit first.
  m_isValid = false;
  m_coefficients_delta_r.clear();
  m_coefficients_r_delta_phi.clear();
  m_evaluation_coefficients_delta_r.clear();
  m_evaluation_coefficients_r_delta_phi.clear();

  if (m_observations.empty())
  {
    return false;
  }

  initialize_grid();
  if (m_nControlPhi < 2 || m_nControlR < 2)
  {
    return false;
  }

  compute_control_support();
  std::vector<int> full_to_fit_index;
  select_active_controls(full_to_fit_index);

  // full_to_fit_index maps the rectangular control grid into the compact
  // coefficient vector used by the solve. A value of -1 marks an inactive
  // control that will later be filled by interpolation.
  int nFit = 0;
  for (int fit_index : full_to_fit_index)
  {
    if (fit_index >= 0)
    {
      nFit = std::max(nFit, fit_index + 1);
    }
  }
  if (nFit < global_field_min_active_controls)
  {
    return false;
  }

  std::vector<std::vector<double>> normal_delta_r(nFit, std::vector<double>(nFit, 0.0));
  std::vector<std::vector<double>> normal_r_delta_phi(nFit, std::vector<double>(nFit, 0.0));
  std::vector<double> rhs_delta_r(nFit, 0.0);
  std::vector<double> rhs_r_delta_phi(nFit, 0.0);

  // Accumulate A^T W A and A^T W y directly. Each observation has at most four
  // bilinear basis weights, so no explicit design matrix is needed.
  for (const auto &observation : m_observations)
  {
    std::vector<int> indices;
    std::vector<double> weights;
    accumulate_bilinear_weights(observation[observation_phi], observation[observation_r], indices, weights);
    if (indices.empty())
    {
      continue;
    }

    const double sigma_delta_r = std::max(observation[observation_sigma_delta_r], 1e-6);
    const double sigma_r_delta_phi = std::max(observation[observation_sigma_r_delta_phi], 1e-6);
    const double inv_var_delta_r = 1.0 / (sigma_delta_r * sigma_delta_r);
    const double inv_var_r_delta_phi = 1.0 / (sigma_r_delta_phi * sigma_r_delta_phi);

    // If one or more of the four surrounding controls are inactive, renormalize
    // the remaining basis weights so this observation retains unit influence.
    double active_weight_sum = 0.0;
    for (size_t a = 0; a < indices.size(); a++)
    {
      if (full_to_fit_index[indices[a]] >= 0)
      {
        active_weight_sum += weights[a];
      }
    }
    if (active_weight_sum <= 1e-6)
    {
      continue;
    }

    for (size_t a = 0; a < indices.size(); a++)
    {
      const int ia = full_to_fit_index[indices[a]];
      if (ia < 0)
      {
        continue;
      }

      const double wa = weights[a] / active_weight_sum;
      rhs_delta_r[ia] += wa * inv_var_delta_r * observation[observation_delta_r];
      rhs_r_delta_phi[ia] += wa * inv_var_r_delta_phi * observation[observation_r_delta_phi];

      for (size_t b = 0; b < indices.size(); b++)
      {
        const int ib = full_to_fit_index[indices[b]];
        if (ib < 0)
        {
          continue;
        }

        const double wb = weights[b] / active_weight_sum;
        normal_delta_r[ia][ib] += wa * inv_var_delta_r * wb;
        normal_r_delta_phi[ia][ib] += wa * inv_var_r_delta_phi * wb;
      }
    }
  }

  add_smoothness_penalty(normal_delta_r, full_to_fit_index);
  add_smoothness_penalty(normal_r_delta_phi, full_to_fit_index);

  // The nugget makes weakly constrained systems nonsingular and limits the
  // numerical condition number without imposing a meaningful field shape.
  for (int i = 0; i < nFit; i++)
  {
    normal_delta_r[i][i] += global_field_kernel_nugget;
    normal_r_delta_phi[i][i] += global_field_kernel_nugget;
  }

  if (!solve_linear_system(normal_delta_r, rhs_delta_r, m_coefficients_delta_r))
  {
    return false;
  }
  if (!solve_linear_system(normal_r_delta_phi, rhs_r_delta_phi, m_coefficients_r_delta_phi))
  {
    return false;
  }

  build_dense_evaluation_grid();

  // Evaluation routines intentionally return zero until the complete fit has
  // succeeded, so mark the object valid only at the end.
  m_isValid = true;
  return true;
}

bool GlobalFieldFitter::is_valid() const
{
  return m_isValid;
}

double GlobalFieldFitter::evaluate_delta_r(double phi, double r) const
{
  // Prefer the reconstructed dense grid. The compact coefficients are retained
  // as a fallback for configurations that do not require reconstruction.
  if (!m_evaluation_coefficients_delta_r.empty())
  {
    return evaluate_component(m_evaluation_coefficients_delta_r, phi, r);
  }
  return evaluate_component(m_coefficients_delta_r, phi, r);
}

double GlobalFieldFitter::evaluate_r_delta_phi(double phi, double r) const
{
  if (!m_evaluation_coefficients_r_delta_phi.empty())
  {
    return evaluate_component(m_evaluation_coefficients_r_delta_phi, phi, r);
  }
  return evaluate_component(m_coefficients_r_delta_phi, phi, r);
}

double GlobalFieldFitter::evaluate_delta_phi(double phi, double r) const
{
  // R*DeltaPhi is fitted because it has distance units and behaves better
  // numerically. Convert back to angular displacement only at evaluation time.
  if (std::abs(r) < 1e-6)
  {
    return 0.0;
  }
  return evaluate_r_delta_phi(phi, r) / r;
}

double GlobalFieldFitter::predictive_sigma_delta_r(double /*phi*/, double /*r*/) const
{
  // The current uncertainty model is global rather than position-dependent.
  // The arguments remain in the API so a local model can be introduced later.
  return estimate_global_residual_sigma(true);
}

double GlobalFieldFitter::predictive_sigma_r_delta_phi(double /*phi*/, double /*r*/) const
{
  return estimate_global_residual_sigma(false);
}

std::vector<std::array<double, 7>> GlobalFieldFitter::control_points() const
{
  // Export the full rectangular grid, including interpolated inactive controls,
  // together with the support metadata used by diagnostic plots.
  std::vector<std::array<double, 7>> points;
  points.reserve(m_nControlPhi * m_nControlR);
  for (int rIndex = 0; rIndex < m_nControlR; ++rIndex)
  {
    const double r = control_r(rIndex);
    for (int phiIndex = 0; phiIndex < m_nControlPhi; ++phiIndex)
    {
      const int index = control_index(phiIndex, rIndex);
      const double phi = control_phi(phiIndex);
      std::array<double, 7> point{};
      point[control_point_phi] = phi;
      point[control_point_r] = r;
      point[control_point_delta_r] = evaluate_delta_r(phi, r);
      point[control_point_r_delta_phi] = evaluate_r_delta_phi(phi, r);
      point[control_point_delta_phi] = std::abs(r) > 1e-6 ? point[control_point_r_delta_phi] / r : 0.0;
      if (index >= 0 && index < static_cast<int>(m_controlSupport.size()))
      {
        point[control_point_support] = m_controlSupport[index];
      }
      if (index >= 0 && index < static_cast<int>(m_activeControl.size()))
      {
        point[control_point_active] = m_activeControl[index];
      }
      points.push_back(point);
    }
  }
  return points;
}

void GlobalFieldFitter::initialize_grid()
{
  // Phi controls are uniformly spaced and periodic. R controls may instead be
  // supplied by the measured radial geometry, which avoids forcing a uniform
  // grid across irregular radial bands.
  const double phi_spacing = std::max(0.05, global_field_control_phi_spacing_rad);
  m_nControlPhi = std::max(3, static_cast<int>(std::round((2.0 * M_PI) / phi_spacing)));
  m_phiMin = 0.0;
  m_phiMax = 2.0 * M_PI;
  m_phiStep = (m_phiMax - m_phiMin) / static_cast<double>(m_nControlPhi);

  m_rMin = fit_r_min_cm;
  m_rMax = fit_r_max_cm;
  if (m_rMax <= m_rMin)
  {
    m_rMax = m_rMin + 1.0;
  }

  m_controlRPositions.clear();
  m_controlRPositions.reserve(m_requestedControlRPositions.size());
  for (double r : m_requestedControlRPositions)
  {
    if (r < m_rMin || r > m_rMax)
    {
      continue;
    }
    m_controlRPositions.push_back(r);
  }
  std::sort(m_controlRPositions.begin(), m_controlRPositions.end());
  // Nearly identical requested radii would create zero-width interpolation
  // cells, so collapse them before deciding whether the custom grid is usable.
  m_controlRPositions.erase(std::unique(m_controlRPositions.begin(), m_controlRPositions.end(), nearly_same_control_r), m_controlRPositions.end());

  if (m_controlRPositions.size() < 2)
  {
    // Fall back to a uniform radial grid when no usable geometry was supplied.
    m_nControlR = std::max(2, global_field_control_r_bins);
    m_rStep = (m_rMax - m_rMin) / static_cast<double>(m_nControlR - 1);
    m_controlRPositions.resize(m_nControlR);
    for (int i = 0; i < m_nControlR; i++)
    {
      m_controlRPositions[i] = m_rMin + static_cast<double>(i) * m_rStep;
    }
  }
  else
  {
    m_nControlR = static_cast<int>(m_controlRPositions.size());
    m_rStep = 0.0;
  }
}

int GlobalFieldFitter::control_index(int phi_index, int r_index) const
{
  // Phi wraps across the detector seam; R is bounded by the fitted radial
  // interval. Keeping this policy in one helper prevents seam inconsistencies.
  int wrapped_phi = phi_index % m_nControlPhi;
  if (wrapped_phi < 0)
  {
    wrapped_phi += m_nControlPhi;
  }
  const int clamped_r = std::clamp(r_index, 0, m_nControlR - 1);
  return clamped_r * m_nControlPhi + wrapped_phi;
}

void GlobalFieldFitter::accumulate_bilinear_weights(double phi, double r, std::vector<int> &indices, std::vector<double> &weights) const
{
  indices.clear();
  weights.clear();

  // Normalize phi before finding its two neighboring periodic controls.
  const double phi_width = m_phiMax - m_phiMin;
  while (phi < m_phiMin)
  {
    phi += phi_width;
  }
  while (phi >= m_phiMax)
  {
    phi -= phi_width;
  }

  const double phi_u = (phi - m_phiMin) / m_phiStep;
  const int phi0 = static_cast<int>(std::floor(phi_u));
  const int phi1 = phi0 + 1;
  const double phi_t = phi_u - std::floor(phi_u);

  // Locate the enclosing radial interval. This works for both uniform and
  // geometry-provided radial control positions.
  const double r_clamped = std::clamp(r, control_r(0), control_r(m_nControlR - 1));
  auto upper = std::upper_bound(m_controlRPositions.begin(), m_controlRPositions.end(), r_clamped);
  int r1 = static_cast<int>(upper - m_controlRPositions.begin());
  if (r1 <= 0)
  {
    r1 = 1;
  }
  if (r1 >= m_nControlR)
  {
    r1 = m_nControlR - 1;
  }
  const int r0 = r1 - 1;
  const double r_span = std::max(control_r(r1) - control_r(r0), 1e-6);
  const double r_t = std::clamp((r_clamped - control_r(r0)) / r_span, 0.0, 1.0);

  // Tensor products of the one-dimensional linear weights give the four basis
  // coefficients used by fitting and field evaluation.
  const double w00 = (1.0 - phi_t) * (1.0 - r_t);
  const double w10 = phi_t * (1.0 - r_t);
  const double w01 = (1.0 - phi_t) * r_t;
  const double w11 = phi_t * r_t;

  indices = {control_index(phi0, r0), control_index(phi1, r0), control_index(phi0, r1), control_index(phi1, r1)};
  weights = {w00, w10, w01, w11};
}

void GlobalFieldFitter::add_smoothness_penalty(std::vector<std::vector<double>> &normal_matrix, const std::vector<int> &full_to_fit_index) const
{
  const double lambda = global_field_regularization_lambda;

  // Penalize the periodic phi second difference c[p-1]-2c[p]+c[p+1].
  // Adding lambda*D^T*D to the normal matrix discourages curvature without
  // forcing the field toward zero.
  for (int r = 0; r < m_nControlR; r++)
  {
    for (int p = 0; p < m_nControlPhi; p++)
    {
      const int im1 = control_index(p - 1, r);
      const int i0 = control_index(p, r);
      const int ip1 = control_index(p + 1, r);
      const double phi_coeffs[3] = {1.0, -2.0, 1.0};
      const int phi_fit_indices[3] = {full_to_fit_index[im1], full_to_fit_index[i0], full_to_fit_index[ip1]};
      if (phi_fit_indices[0] < 0 || phi_fit_indices[1] < 0 || phi_fit_indices[2] < 0)
      {
        continue;
      }

      for (int a = 0; a < 3; a++)
      {
        for (int b = 0; b < 3; b++)
        {
          normal_matrix[phi_fit_indices[a]][phi_fit_indices[b]] += lambda * phi_coeffs[a] * phi_coeffs[b];
        }
      }
    }
  }

  // Apply the analogous second-difference penalty along R. Radial endpoints
  // have no two-sided stencil and are constrained by data and neighboring rows.
  for (int r = 1; r < m_nControlR - 1; r++)
  {
    for (int p = 0; p < m_nControlPhi; p++)
    {
      const int jm1 = control_index(p, r - 1);
      const int j0 = control_index(p, r);
      const int jp1 = control_index(p, r + 1);
      const double r_coeffs[3] = {1.0, -2.0, 1.0};
      const int r_fit_indices[3] = {full_to_fit_index[jm1], full_to_fit_index[j0], full_to_fit_index[jp1]};
      if (r_fit_indices[0] < 0 || r_fit_indices[1] < 0 || r_fit_indices[2] < 0)
      {
        continue;
      }

      for (int a = 0; a < 3; a++)
      {
        for (int b = 0; b < 3; b++)
        {
          normal_matrix[r_fit_indices[a]][r_fit_indices[b]] += lambda * r_coeffs[a] * r_coeffs[b];
        }
      }
    }
  }
}

bool GlobalFieldFitter::solve_linear_system(std::vector<std::vector<double>> matrix, std::vector<double> rhs, std::vector<double> &solution) const
{
  // Solve the dense augmented system with Gauss-Jordan elimination and partial
  // pivoting. The matrices are modest control-grid systems, so avoiding an
  // additional linear-algebra dependency is reasonable here.
  const size_t n = matrix.size();
  solution.assign(n, 0.0);
  if (rhs.size() != n)
  {
    return false;
  }

  for (size_t i = 0; i < n; i++)
  {
    matrix[i].push_back(rhs[i]);
  }

  for (size_t pivot_col = 0; pivot_col < n; pivot_col++)
  {
    // Choose the largest available pivot in this column for numerical stability.
    size_t pivot_row = pivot_col;
    double pivot_abs = std::abs(matrix[pivot_row][pivot_col]);

    for (size_t row = pivot_col + 1; row < n; row++)
    {
      const double candidate_abs = std::abs(matrix[row][pivot_col]);
      if (candidate_abs > pivot_abs)
      {
        pivot_abs = candidate_abs;
        pivot_row = row;
      }
    }

    if (pivot_abs < 1e-12)
    {
      return false;
    }

    if (pivot_row != pivot_col)
    {
      std::swap(matrix[pivot_row], matrix[pivot_col]);
    }

    // Normalize the pivot row, then eliminate this column from every other row.
    const double pivot = matrix[pivot_col][pivot_col];
    for (size_t col = pivot_col; col <= n; col++)
    {
      matrix[pivot_col][col] /= pivot;
    }

    for (size_t row = 0; row < n; row++)
    {
      if (row == pivot_col)
      {
        continue;
      }

      const double factor = matrix[row][pivot_col];
      for (size_t col = pivot_col; col <= n; col++)
      {
        matrix[row][col] -= factor * matrix[pivot_col][col];
      }
    }
  }

  for (size_t row = 0; row < n; row++)
  {
    solution[row] = matrix[row][n];
  }

  return true;
}

double GlobalFieldFitter::evaluate_component(const std::vector<double> &coefficients, double phi, double r) const
{
  if (!m_isValid || coefficients.empty())
  {
    return 0.0;
  }

  if (static_cast<int>(coefficients.size()) != m_nControlPhi * m_nControlR)
  {
    // A compact vector contains active controls only and therefore cannot be
    // indexed as a rectangular grid. Evaluate it with neighbor interpolation.
    return interpolate_active_value(coefficients, phi, r);
  }

  std::vector<int> indices;
  std::vector<double> weights;
  accumulate_bilinear_weights(phi, r, indices, weights);

  double value = 0.0;
  for (size_t i = 0; i < indices.size(); i++)
  {
    value += weights[i] * coefficients[indices[i]];
  }

  return value;
}

void GlobalFieldFitter::compute_control_support()
{
  // Support is the sum of bilinear basis weights contributed by observations.
  // It is more informative than a hard count near control-cell boundaries.
  const int nCtrl = m_nControlPhi * m_nControlR;
  m_controlSupport.assign(nCtrl, 0.0);

  for (const auto &observation : m_observations)
  {
    std::vector<int> indices;
    std::vector<double> weights;
    accumulate_bilinear_weights(observation[observation_phi], observation[observation_r], indices, weights);
    for (size_t i = 0; i < indices.size(); i++)
    {
      m_controlSupport[indices[i]] += weights[i];
    }
  }
}

void GlobalFieldFitter::select_active_controls(std::vector<int> &full_to_fit_index)
{
  const int nCtrl = m_nControlPhi * m_nControlR;
  full_to_fit_index.assign(nCtrl, -1);
  m_activeControl.assign(nCtrl, false);

  if (!use_active_control_interpolation)
  {
    // Dense mode fits every control directly, regardless of local support.
    for (int i = 0; i < nCtrl; i++)
    {
      m_activeControl[i] = true;
      full_to_fit_index[i] = i;
    }
    return;
  }

  // Sparse mode solves only controls with enough direct observation support.
  int nActive = 0;
  for (int i = 0; i < nCtrl; i++)
  {
    if (m_controlSupport[i] < global_field_min_control_support)
    {
      continue;
    }

    m_activeControl[i] = true;
    full_to_fit_index[i] = nActive++;
  }

  if (nActive >= global_field_min_active_controls)
  {
    return;
  }

  // A sparse solve with too few controls is not meaningful. Fall back to the
  // dense grid and let regularization plus the nugget stabilize the solution.
  for (int i = 0; i < nCtrl; i++)
  {
    m_activeControl[i] = true;
    full_to_fit_index[i] = i;
  }
}

void GlobalFieldFitter::build_dense_evaluation_grid()
{
  // Convert compact fitted coefficients into a full grid so subsequent map
  // generation uses inexpensive bilinear interpolation everywhere.
  const int nCtrl = m_nControlPhi * m_nControlR;
  m_evaluation_coefficients_delta_r.assign(nCtrl, 0.0);
  m_evaluation_coefficients_r_delta_phi.assign(nCtrl, 0.0);

  if (!use_active_control_interpolation || static_cast<int>(m_coefficients_delta_r.size()) == nCtrl || static_cast<int>(m_coefficients_r_delta_phi.size()) == nCtrl)
  {
    m_evaluation_coefficients_delta_r = m_coefficients_delta_r;
    m_evaluation_coefficients_r_delta_phi = m_coefficients_r_delta_phi;
    return;
  }

  // Copy fitted active-control values back into their rectangular locations.
  int coeff_index = 0;
  // Fill unsupported controls from nearby active controls. This extrapolation
  // affects evaluation only; inactive values never enter the original solve.
  for (int ir = 0; ir < m_nControlR; ir++)
  {
    for (int iphi = 0; iphi < m_nControlPhi; iphi++)
    {
      const int full_index = control_index(iphi, ir);
      if (full_index >= static_cast<int>(m_activeControl.size()) || !m_activeControl[full_index])
      {
        continue;
      }
      if (coeff_index >= static_cast<int>(m_coefficients_delta_r.size()) || coeff_index >= static_cast<int>(m_coefficients_r_delta_phi.size()))
      {
        break;
      }

      m_evaluation_coefficients_delta_r[full_index] = m_coefficients_delta_r[coeff_index];
      m_evaluation_coefficients_r_delta_phi[full_index] = m_coefficients_r_delta_phi[coeff_index];
      coeff_index++;
    }
  }

  for (int ir = 0; ir < m_nControlR; ir++)
  {
    const double r = control_r(ir);
    for (int iphi = 0; iphi < m_nControlPhi; iphi++)
    {
      const int full_index = control_index(iphi, ir);
      if (full_index < static_cast<int>(m_activeControl.size()) && m_activeControl[full_index])
      {
        continue;
      }

      const double phi = control_phi(iphi);
      m_evaluation_coefficients_delta_r[full_index] = interpolate_active_value(m_coefficients_delta_r, phi, r);
      m_evaluation_coefficients_r_delta_phi[full_index] = interpolate_active_value(m_coefficients_r_delta_phi, phi, r);
    }
  }
}

double GlobalFieldFitter::control_phi(int phi_index) const
{
  int wrapped_phi = phi_index % m_nControlPhi;
  if (wrapped_phi < 0)
  {
    wrapped_phi += m_nControlPhi;
  }
  return m_phiMin + static_cast<double>(wrapped_phi) * m_phiStep;
}

double GlobalFieldFitter::control_r(int r_index) const
{
  const int clamped_r = std::clamp(r_index, 0, m_nControlR - 1);
  if (clamped_r < static_cast<int>(m_controlRPositions.size()))
  {
    return m_controlRPositions[clamped_r];
  }
  return m_rMin;
}

double GlobalFieldFitter::interpolate_active_value(const std::vector<double> &coefficients, double phi, double r) const
{
  if (coefficients.empty())
  {
    return 0.0;
  }

  // Retain only the nearest active controls in a scaled detector metric.
  // Separate R and R*phi scales encode the intended interpolation anisotropy.
  std::vector<std::pair<double, double>> neighbors;
  neighbors.reserve(std::max(1, global_field_active_interpolation_neighbors));
  int coeff_index = 0;
  const int max_neighbors = std::max(1, global_field_active_interpolation_neighbors);
  const double r_scale = std::max(1e-6, global_field_active_interpolation_r_scale_cm);
  const double rphi_scale = std::max(1e-6, global_field_active_interpolation_rphi_scale_cm);

  for (int ir = 0; ir < m_nControlR; ir++)
  {
    const double controlR = control_r(ir);
    for (int iphi = 0; iphi < m_nControlPhi; iphi++)
    {
      const int full_index = control_index(iphi, ir);
      if (full_index >= static_cast<int>(m_activeControl.size()) || !m_activeControl[full_index])
      {
        continue;
      }
      if (coeff_index >= static_cast<int>(coefficients.size()))
      {
        break;
      }

      // Convert angular separation to an arc length at the mean radius.
      const double dphi = wrap_delta_phi(phi - control_phi(iphi));
      const double dr = r - controlR;
      const double rmean = 0.5 * (r + controlR);
      const double radial_distance = dr / r_scale;
      const double phi_distance = (rmean * dphi) / rphi_scale;
      const double dist2 = radial_distance * radial_distance + phi_distance * phi_distance;
      if (dist2 <= 1e-12)
      {
        return coefficients[coeff_index];
      }

      if (static_cast<int>(neighbors.size()) < max_neighbors)
      {
        neighbors.emplace_back(dist2, coefficients[coeff_index]);
      }
      else
      {
        auto farthest = std::max_element(neighbors.begin(), neighbors.end(), smaller_neighbor_distance);
        if (farthest != neighbors.end() && dist2 < farthest->first)
        {
          *farthest = std::make_pair(dist2, coefficients[coeff_index]);
        }
      }
      coeff_index++;
    }
  }

  if (neighbors.empty())
  {
    return 0.0;
  }

  // Inverse-distance weighting supplies a smooth value without introducing a
  // second global fit for unsupported controls.
  const double power = std::max(0.1, global_field_active_interpolation_power);
  double weighted_sum = 0.0;
  double weight_sum = 0.0;
  for (const auto &neighbor : neighbors)
  {
    const double dist2 = neighbor.first;
    const double value = neighbor.second;
    const double dist = std::max(std::sqrt(dist2), 1e-6);
    const double weight = 1.0 / std::pow(dist, power);
    weighted_sum += weight * value;
    weight_sum += weight;
  }

  if (weight_sum <= 0.0)
  {
    return 0.0;
  }
  return weighted_sum / weight_sum;
}

double GlobalFieldFitter::estimate_global_residual_sigma(bool fit_delta_r) const
{
  if (!m_isValid || m_observations.empty())
  {
    return 999.0;
  }

  // Use MAD rather than RMS so a small number of incorrect stripe assignments
  // does not dominate the reported predictive uncertainty.
  std::vector<double> residuals;
  residuals.reserve(m_observations.size());
  for (const auto &observation : m_observations)
  {
    const double model = fit_delta_r ? evaluate_delta_r(observation[observation_phi], observation[observation_r]) : evaluate_r_delta_phi(observation[observation_phi], observation[observation_r]);
    const double value = fit_delta_r ? observation[observation_delta_r] : observation[observation_r_delta_phi];
    residuals.push_back(value - model);
  }

  return robust_mad_sigma(residuals, fit_delta_r ? fallback_sigma_prior_delta_r_cm : fallback_sigma_prior_r_delta_phi_cm);
}

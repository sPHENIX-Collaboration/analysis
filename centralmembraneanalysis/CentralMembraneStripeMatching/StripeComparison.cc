#include "StripeComparison.h"

#include "GlobalFieldFitter.h"
#include "helpers.h"
#include "parameters.h"

#include <TGraph.h>
#include <TGraph2D.h>
#include <TH1.h>
#include <TH2.h>
#include <TTree.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <array>
#include <vector>

// Complete measured-to-reference comparison workflow:
//
//   1. filter stripe candidates and build radial-row bookkeeping;
//   2. run full-R locked branch probes for q = -1, 0, +1;
//   3. select the viable branch with the smallest cleaned score;
//   4. robustly refit and write matching, field, and stripe-residual
//      diagnostics.
//
constexpr std::array<const char *, 2> kComparisonSideNames = {"negz", "posz"};

bool nearly_same_control_position(double lhs, double rhs)
{
  return std::abs(lhs - rhs) < 1e-3;
}

std::vector<std::array<double, 3>> remove_topology_region_edge_rows(const std::vector<std::array<double, 3>> &stripes, size_t &removed);

StripeComparison::~StripeComparison()
{
  clear();
}

bool StripeComparison::initialize(const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference, int side, const std::vector<double> &controlRPositions)
{
  // Reset all state from the previous detector side and preserve raw inputs for
  // output diagnostics before any filtering or matching occurs.
  if (measured.empty() || reference.empty())
  {
    std::cout << "ComputeStripeComparisonMaps: Empty measured or reference stripes" << std::endl;
    return false;
  }

  clear();
  m_sideName = kComparisonSideNames.at(side);
  m_controlRPositions = controlRPositions;
  std::sort(m_controlRPositions.begin(), m_controlRPositions.end());
  m_controlRPositions.erase(std::unique(m_controlRPositions.begin(), m_controlRPositions.end(), nearly_same_control_position), m_controlRPositions.end());
  return true;
}

bool StripeComparison::filter_isolated_inputs(const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference)
{
  // Suppress isolated noise candidates before nearest-neighbor construction.
  filter_isolated_stripes(measured, m_measuredFiltered);
  filter_isolated_stripes(reference, m_referenceFiltered);
  size_t removedMeasuredEdgeStripes = 0;
  size_t removedReferenceEdgeStripes = 0;
  m_measuredFiltered = remove_topology_region_edge_rows(m_measuredFiltered, removedMeasuredEdgeStripes);
  m_referenceFiltered = remove_topology_region_edge_rows(m_referenceFiltered, removedReferenceEdgeStripes);
  std::cout << "Topology edge-row filter for side " << m_sideName << ": removed measured=" << removedMeasuredEdgeStripes << " reference=" << removedReferenceEdgeStripes << std::endl;
  if (m_measuredFiltered.empty() || m_referenceFiltered.empty())
  {
    std::cout << "ComputeStripeComparisonMaps: Empty measured or reference "
                 "after isolation/topology edge filtering"
              << std::endl;
    return false;
  }
  return true;
}

// --------------------------------------------------------------------------
// Flattened data slots used by the stripe assignment pipeline
// --------------------------------------------------------------------------

// A candidate match is std::array<double, 6>.
// It stores measured index, reference index, displacement values, and a simple
// geometric distance. Indices are stored as doubles only because the flattened
// container is all-double; every use as a vector index casts back to size_t.
inline constexpr int candidate_measured_idx = 0;
inline constexpr int candidate_reference_idx = 1;
inline constexpr int candidate_delta_r = 2;
inline constexpr int candidate_delta_phi = 3;
inline constexpr int candidate_r_delta_phi = 4;
inline constexpr int candidate_distance = 5;


// A region is std::array<size_t, 2> = {begin row, one-past-last row}.
inline constexpr int region_begin = 0;
inline constexpr int region_end = 1;

// A branch hypothesis result is
// std::tuple<candidates, score, median residual, unmatched fraction,
//            cleaning fraction, rejected-by-cleaning count, iterations>.
inline constexpr int hypothesis_candidates = 0;
inline constexpr int hypothesis_score = 1;
inline constexpr int hypothesis_median_residual = 2;
inline constexpr int hypothesis_unmatched_fraction = 3;
inline constexpr int hypothesis_cleaning_fraction = 4;
inline constexpr int hypothesis_rejected_by_cleaning = 5;
inline constexpr int hypothesis_iterations = 6;


void sort_topology_row(std::pair<double, std::vector<size_t>> &row, const std::vector<std::array<double, 3>> &stripes)
{
  // row.first is the row center R.
  // row.second is the list of stripe indices in this radial row.
  // Phi ordering supplies the topology needed for +/- one-stripe seed shifts.
  for (size_t i = 0; i < row.second.size(); i++)
  {
    for (size_t j = i + 1; j < row.second.size(); j++)
    {
      if (stripes[row.second[j]][stripe_phi] < stripes[row.second[i]][stripe_phi])
      {
        std::swap(row.second[i], row.second[j]);
      }
    }
  }
  if (row.second.empty())
  {
    return;
  }
  double sum = 0.0;
  for (size_t index : row.second)
  {
    sum += stripes[index][stripe_r];
  }
  row.first = sum / static_cast<double>(row.second.size());
}

std::vector<std::pair<double, std::vector<size_t>>> build_topology_rows(const std::vector<std::array<double, 3>> &stripes)
{
  // Return rows as std::pair<center R, vector of stripe indices>.
  // Build radial bands from this stripe set alone. A new row begins when the
  // next radius lies beyond the tolerance from the running row center.
  std::vector<size_t> radialOrder(stripes.size());
  for (size_t i = 0; i < stripes.size(); ++i)
  {
    radialOrder[i] = i;
  }
  for (size_t i = 0; i < radialOrder.size(); i++)
  {
    for (size_t j = i + 1; j < radialOrder.size(); j++)
    {
      if (stripes[radialOrder[j]][stripe_r] < stripes[radialOrder[i]][stripe_r])
      {
        std::swap(radialOrder[i], radialOrder[j]);
      }
    }
  }
  std::vector<std::pair<double, std::vector<size_t>>> rows;
  for (size_t index : radialOrder)
  {
    if (rows.empty() || std::abs(stripes[index][stripe_r] - rows.back().first) > topology_row_tolerance_cm)
    {
      rows.push_back({stripes[index][stripe_r], {index}});
      continue;
    }
    auto &row = rows.back();
    row.second.push_back(index);
    row.first += (stripes[index][stripe_r] - row.first) / static_cast<double>(row.second.size());
  }
  for (auto &row : rows)
  {
    sort_topology_row(row, stripes);
  }
  std::vector<std::pair<double, std::vector<size_t>>> keptRows;
  for (const auto &row : rows)
  {
    if (static_cast<int>(row.second.size()) >= topology_min_stripes_per_row)
    {
      keptRows.push_back(row);
    }
  }
  rows.swap(keptRows);
  return rows;
}

std::vector<std::array<size_t, 2>> build_topology_regions(const std::vector<std::pair<double, std::vector<size_t>>> &rows)
{
  // Split rows into coarse radial regions at the largest row-to-row gaps.
  // Each region is {begin row, one-past-last row}.
  if (rows.empty())
  {
    return {};
  }

  std::vector<size_t> regionStarts{0};
  if (rows.size() > 1 && topology_region_count > 1)
  {
    std::vector<size_t> gapOrder(rows.size() - 1);
    for (size_t i = 0; i < gapOrder.size(); ++i)
    {
      gapOrder[i] = i;
    }
    for (size_t i = 0; i < gapOrder.size(); i++)
    {
      for (size_t j = i + 1; j < gapOrder.size(); j++)
      {
        const double gapI = rows[gapOrder[i] + 1].first - rows[gapOrder[i]].first;
        const double gapJ = rows[gapOrder[j] + 1].first - rows[gapOrder[j]].first;
        if (gapJ > gapI)
        {
          std::swap(gapOrder[i], gapOrder[j]);
        }
      }
    }

    const size_t splitCount = std::min(gapOrder.size(), static_cast<size_t>(std::max(0, topology_region_count - 1)));
    for (size_t i = 0; i < splitCount; ++i)
    {
      regionStarts.push_back(gapOrder[i] + 1);
    }
    std::sort(regionStarts.begin(), regionStarts.end());
  }
  regionStarts.push_back(rows.size());

  std::vector<std::array<size_t, 2>> regions;
  regions.reserve(regionStarts.size() - 1);
  for (size_t region = 0; region + 1 < regionStarts.size(); ++region)
  {
    regions.push_back({regionStarts[region], regionStarts[region + 1]});
  }
  return regions;
}

std::vector<std::array<double, 3>> remove_topology_region_edge_rows(const std::vector<std::array<double, 3>> &stripes, size_t &removed)
{
  // Remove configurable inner/outer rows from each topology region.
  // This avoids using poorly constrained region boundaries as matching seeds.
  removed = 0;
  const auto rows = build_topology_rows(stripes);
  if (rows.empty())
  {
    removed = stripes.size();
    return {};
  }

  const auto regions = build_topology_regions(rows);
  std::vector<bool> keep(stripes.size(), false);
  for (const auto &row : rows)
  {
    for (size_t index : row.second)
    {
      if (index < keep.size())
      {
        keep[index] = true;
      }
    }
  }
  for (const auto &region : regions)
  {
    const size_t begin = region[region_begin];
    const size_t end = region[region_end];
    const size_t regionRows = end - begin;
    const size_t innerRows = std::min(static_cast<size_t>(std::max(0, topology_excluded_inner_rows_per_region)), regionRows);
    const size_t outerRows = std::min(static_cast<size_t>(std::max(0, topology_excluded_outer_rows_per_region)), regionRows);
    for (size_t offset = 0; offset < innerRows; ++offset)
    {
      for (size_t index : rows[begin + offset].second)
      {
        keep[index] = false;
      }
    }
    for (size_t offset = 0; offset < outerRows; ++offset)
    {
      for (size_t index : rows[end - 1 - offset].second)
      {
        keep[index] = false;
      }
    }
  }

  std::vector<std::array<double, 3>> filtered;
  filtered.reserve(stripes.size());
  for (size_t i = 0; i < stripes.size(); ++i)
  {
    if (keep[i])
    {
      filtered.push_back(stripes[i]);
    }
    else
    {
      ++removed;
    }
  }
  return filtered;
}

std::array<double, 6> make_assignment_candidate(size_t measuredIndex, size_t referenceIndex, const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference)
{
  // Fill one std::array<double, 6> candidate using candidate_* slots.
  // Use measured R for R*DeltaPhi in diagnostics and field observations. The
  // broad phi gate and Hungarian field-consistency term use DeltaPhi directly
  // so angular aliases are penalized uniformly in radius.
  std::array<double, 6> candidate{};
  candidate[candidate_measured_idx] = static_cast<double>(measuredIndex);
  candidate[candidate_reference_idx] = static_cast<double>(referenceIndex);
  candidate[candidate_delta_r] = measured[measuredIndex][stripe_r] - reference[referenceIndex][stripe_r];
  candidate[candidate_delta_phi] = wrap_delta_phi(measured[measuredIndex][stripe_phi] - reference[referenceIndex][stripe_phi]);
  candidate[candidate_r_delta_phi] = measured[measuredIndex][stripe_r] * candidate[candidate_delta_phi];
  candidate[candidate_distance] = std::sqrt(candidate[candidate_delta_r] * candidate[candidate_delta_r] + candidate[candidate_r_delta_phi] * candidate[candidate_r_delta_phi]);
  return candidate;
}

bool assignment_candidate_within_gates(const std::array<double, 6> &candidate)
{
  // Fast broad cut before more expensive field-based scoring.
  // These are broad admissibility gates, not the final matching rule. The phi
  // gate is angular, not RDeltaPhi, so the same DeltaPhi mismatch is rejected
  // consistently at small and large radius.
  return std::abs(candidate[candidate_delta_r]) <= global_assignment_max_abs_delta_r_cm && std::abs(candidate[candidate_delta_phi]) <= global_assignment_max_abs_delta_phi_rad;
}


// --------------------------------------------------------------------------
// Radial-row topology and branch seeds
// --------------------------------------------------------------------------

std::pair<std::vector<std::pair<double, std::vector<size_t>>>, std::vector<std::pair<double, std::vector<size_t>>>> build_topology_initialization(const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference)
{
  // Return {measured rows, reference rows}.
  // Rows are discovered independently. Their integer labels are bookkeeping and
  // do not assert that measured row a corresponds to reference row a.
  return {build_topology_rows(measured), build_topology_rows(reference)};
}

std::vector<std::array<double, 6>> keep_unique_assignment_candidates(std::vector<std::array<double, 6>> candidates, size_t measuredCount, size_t referenceCount)
{
  // Sort by candidate_distance using plain loops, then keep each measured and
  // reference index no more than once.
  for (size_t i = 0; i < candidates.size(); i++)
  {
    for (size_t j = i + 1; j < candidates.size(); j++)
    {
      if (candidates[j][candidate_distance] < candidates[i][candidate_distance])
      {
        std::swap(candidates[i], candidates[j]);
      }
    }
  }
  std::vector<bool> usedMeasured(measuredCount, false);
  std::vector<bool> usedReference(referenceCount, false);
  std::vector<std::array<double, 6>> unique;
  unique.reserve(candidates.size());
  for (const auto &candidate : candidates)
  {
    if (!usedMeasured[static_cast<size_t>(candidate[candidate_measured_idx])] && !usedReference[static_cast<size_t>(candidate[candidate_reference_idx])])
    {
      usedMeasured[static_cast<size_t>(candidate[candidate_measured_idx])] = true;
      usedReference[static_cast<size_t>(candidate[candidate_reference_idx])] = true;
      unique.push_back(candidate);
    }
  }
  return unique;
}

int nearest_reference_row_for_measured_row(const std::pair<std::vector<std::pair<double, std::vector<size_t>>>, std::vector<std::pair<double, std::vector<size_t>>>> &topology, size_t measuredRow)
{
  // topology.first is measured rows and topology.second is reference rows.
  // Pick the reference row with the closest radial center before applying any
  // tested branch shift.
  if (measuredRow >= topology.first.size() || topology.second.empty())
  {
    return -1;
  }

  int nearestReferenceRow = 0;
  double nearestRadialDistance = std::abs(topology.first[measuredRow].first - topology.second.front().first);
  for (size_t referenceRow = 1; referenceRow < topology.second.size(); ++referenceRow)
  {
    const double radialDistance = std::abs(topology.first[measuredRow].first - topology.second[referenceRow].first);
    if (radialDistance < nearestRadialDistance)
    {
      nearestRadialDistance = radialDistance;
      nearestReferenceRow = static_cast<int>(referenceRow);
    }
  }
  return nearestReferenceRow;
}

int expected_reference_row_ordinal(size_t measuredOrdinal, size_t measuredCount, size_t referenceCount, int branchShift)
{
  // Map a local measured-row ordinal into the same fractional position in the
  // reference region, then offset it by the tested branch. This handles regions
  // with different row counts without assuming one-to-one row numbering.
  if (referenceCount == 0)
  {
    return -1;
  }

  size_t referenceOrdinal = 0;
  if (measuredCount > 1 && referenceCount > 1)
  {
    const double rowFraction = static_cast<double>(measuredOrdinal) / static_cast<double>(measuredCount - 1);
    referenceOrdinal = std::min(static_cast<size_t>(std::lround(rowFraction * static_cast<double>(referenceCount - 1))), referenceCount - 1);
  }
  return static_cast<int>(referenceOrdinal) + branchShift;
}

std::vector<int> solve_monotone_region_row_map(const std::pair<std::vector<std::pair<double, std::vector<size_t>>>, std::vector<std::pair<double, std::vector<size_t>>>> &topology, const std::array<size_t, 2> &measuredRegion, const std::array<size_t, 2> &referenceRegion, int branchShift, double &medianDeltaR)
{
  // Dynamic programming row matcher for one radial region. It allows measured
  // rows to be skipped, allows reference rows to be skipped at no measured cost,
  // and keeps the final row order monotone.
  const size_t measuredCount = measuredRegion[region_end] > measuredRegion[region_begin] ? measuredRegion[region_end] - measuredRegion[region_begin] : 0;
  const size_t referenceCount = referenceRegion[region_end] > referenceRegion[region_begin] ? referenceRegion[region_end] - referenceRegion[region_begin] : 0;
  medianDeltaR = 0.0;
  std::vector<int> mappedReferenceByMeasured(measuredCount, -1);
  if (measuredCount == 0 || referenceCount == 0)
  {
    return mappedReferenceByMeasured;
  }

  std::vector<int> expectedReference(measuredCount, -1);
  std::vector<double> deltaRSeeds;
  deltaRSeeds.reserve(measuredCount);
  for (size_t measuredOrdinal = 0; measuredOrdinal < measuredCount; ++measuredOrdinal)
  {
    // Build a rough DeltaR prior from the branch-shifted fractional row map.
    // The median keeps the later DP cost centered even if a few rows are bad.
    expectedReference[measuredOrdinal] = expected_reference_row_ordinal(measuredOrdinal, measuredCount, referenceCount, branchShift);
    if (expectedReference[measuredOrdinal] < 0 || expectedReference[measuredOrdinal] >= static_cast<int>(referenceCount))
    {
      continue;
    }
    const auto &measuredRow = topology.first[measuredRegion[region_begin] + measuredOrdinal];
    const auto &referenceRow = topology.second[referenceRegion[region_begin] + static_cast<size_t>(expectedReference[measuredOrdinal])];
    deltaRSeeds.push_back(measuredRow.first - referenceRow.first);
  }
  if (!deltaRSeeds.empty())
  {
    medianDeltaR = median_value(deltaRSeeds);
  }

  const double sigmaDeltaR = std::max(topology_row_mapping_sigma_delta_r_cm, 1e-6);
  const double indexWeight = std::max(0.0, topology_row_mapping_index_weight);
  const double measuredSkipCost = std::max(0.0, topology_row_mapping_measured_skip_cost);
  const double infinity = 1.0e100;

  std::vector<std::vector<double>> dp(measuredCount + 1, std::vector<double>(referenceCount + 1, infinity));
  std::vector<std::vector<char>> previous(measuredCount + 1, std::vector<char>(referenceCount + 1, 0));
  // dp[i][j] is the best cost after considering the first i measured rows and
  // first j reference rows. previous records whether the best step skipped a
  // reference row, skipped a measured row, or matched the two current rows.
  dp[0][0] = 0.0;
  for (size_t j = 1; j <= referenceCount; ++j)
  {
    dp[0][j] = 0.0;
    previous[0][j] = 'r';
  }
  for (size_t i = 1; i <= measuredCount; ++i)
  {
    dp[i][0] = dp[i - 1][0] + measuredSkipCost;
    previous[i][0] = 'm';
  }

  for (size_t i = 1; i <= measuredCount; ++i)
  {
    for (size_t j = 1; j <= referenceCount; ++j)
    {
      double best = dp[i][j - 1];
      char choice = 'r';

      const double skipMeasured = dp[i - 1][j] + measuredSkipCost;
      if (skipMeasured < best)
      {
        best = skipMeasured;
        choice = 'm';
      }

      const auto &measuredRow = topology.first[measuredRegion[region_begin] + i - 1];
      const auto &referenceRow = topology.second[referenceRegion[region_begin] + j - 1];
      // The match cost combines radial consistency with the expected branch
      // position. This prevents a smooth but wrong branch from drifting across
      // the region one row at a time.
      const double deltaRResidual = ((measuredRow.first - referenceRow.first) - medianDeltaR) / sigmaDeltaR;
      const double expectedResidual = static_cast<double>(static_cast<int>(j - 1) - expectedReference[i - 1]);
      const double rowPairCost = deltaRResidual * deltaRResidual + indexWeight * expectedResidual * expectedResidual;
      const double match = dp[i - 1][j - 1] + rowPairCost;
      if (match < best)
      {
        best = match;
        choice = 'x';
      }

      dp[i][j] = best;
      previous[i][j] = choice;
    }
  }

  size_t i = measuredCount;
  size_t j = referenceCount;
  while (i > 0 || j > 0)
  {
    // Backtrack the selected DP path into absolute reference-row indices.
    const char choice = previous[i][j];
    if (choice == 'x')
    {
      mappedReferenceByMeasured[i - 1] = static_cast<int>(referenceRegion[region_begin] + j - 1);
      --i;
      --j;
    }
    else if (choice == 'm')
    {
      --i;
    }
    else
    {
      --j;
    }
  }

  return mappedReferenceByMeasured;
}

std::vector<int> build_monotone_row_mapping(const std::pair<std::vector<std::pair<double, std::vector<size_t>>>, std::vector<std::pair<double, std::vector<size_t>>>> &topology, int branchShift)
{
  // Run the monotone DP independently inside each large radial region. The
  // returned vector is indexed by measured row and stores a reference-row index
  // or -1 when that measured row has no allowed row in this branch.
  std::vector<int> mappedReferenceByMeasured(topology.first.size(), -1);
  const auto measuredRegions = build_topology_regions(topology.first);
  const auto referenceRegions = build_topology_regions(topology.second);
  const size_t regionCount = std::min(measuredRegions.size(), referenceRegions.size());
  for (size_t region = 0; region < regionCount; ++region)
  {
    double medianDeltaR = 0.0;
    const auto regionMap = solve_monotone_region_row_map(topology, measuredRegions[region], referenceRegions[region], branchShift, medianDeltaR);
    size_t mappedRows = 0;
    for (size_t localMeasuredRow = 0; localMeasuredRow < regionMap.size(); ++localMeasuredRow)
    {
      if (regionMap[localMeasuredRow] < 0)
      {
        continue;
      }
      mappedReferenceByMeasured[measuredRegions[region][region_begin] + localMeasuredRow] = regionMap[localMeasuredRow];
      ++mappedRows;
    }
    std::cout << "Row map branch=" << branchShift << " region=" << region << " measuredRows=" << regionMap.size() << " referenceRows=" << (referenceRegions[region][region_end] - referenceRegions[region][region_begin]) << " mappedRows=" << mappedRows << " unmappedRows=" << (regionMap.size() - mappedRows) << " medianDeltaR=" << medianDeltaR << std::endl;
  }
  return mappedReferenceByMeasured;
}

std::vector<int> build_nearest_row_mapping(const std::pair<std::vector<std::pair<double, std::vector<size_t>>>, std::vector<std::pair<double, std::vector<size_t>>>> &topology, int branchShift)
{
  // Simpler row map: nearest-R reference row plus the tested branch shift.
  // This is useful when the rows are already well behaved and the DP is not
  // requested by parameters.
  std::vector<int> mappedReferenceByMeasured(topology.first.size(), -1);
  for (size_t measuredRow = 0; measuredRow < topology.first.size(); ++measuredRow)
  {
    const int nearestReferenceRow = nearest_reference_row_for_measured_row(topology, measuredRow);
    const int lockedReferenceRow = nearestReferenceRow + branchShift;
    if (lockedReferenceRow < 0 || lockedReferenceRow >= static_cast<int>(topology.second.size()))
    {
      continue;
    }
    mappedReferenceByMeasured[measuredRow] = lockedReferenceRow;
  }
  return mappedReferenceByMeasured;
}

void repair_edge_row_branch_mapping(const std::pair<std::vector<std::pair<double, std::vector<size_t>>>, std::vector<std::pair<double, std::vector<size_t>>>> &topology, int branchShift, std::vector<int> &mappedReferenceByMeasuredRow)
{
  // Edge rows can be biased by missing neighbors. Use the non-edge row branch
  // majority inside each region, then snap disagreeing edge rows back onto that
  // majority branch when the repaired row remains in range.
  const auto measuredRegions = build_topology_regions(topology.first);
  const auto referenceRegions = build_topology_regions(topology.second);
  const size_t regionCount = std::min(measuredRegions.size(), referenceRegions.size());
  const size_t edgeRows = static_cast<size_t>(std::max(0, topology_edge_row_branch_repair_edge_rows));
  if (edgeRows == 0)
  {
    return;
  }

  for (size_t region = 0; region < regionCount; ++region)
  {
    const auto &measuredRegion = measuredRegions[region];
    const auto &referenceRegion = referenceRegions[region];
    const size_t measuredCount = measuredRegion[region_end] > measuredRegion[region_begin] ? measuredRegion[region_end] - measuredRegion[region_begin] : 0;
    const size_t referenceCount = referenceRegion[region_end] > referenceRegion[region_begin] ? referenceRegion[region_end] - referenceRegion[region_begin] : 0;
    if (measuredCount == 0 || referenceCount == 0)
    {
      continue;
    }

    std::vector<int> rowBranchLabels(measuredCount, 999999);
    for (size_t measuredOrdinal = 0; measuredOrdinal < measuredCount; ++measuredOrdinal)
    {
      // Convert absolute row choices into branch labels relative to the
      // unshifted fractional row expectation.
      const size_t measuredRow = measuredRegion[region_begin] + measuredOrdinal;
      if (measuredRow >= mappedReferenceByMeasuredRow.size() || mappedReferenceByMeasuredRow[measuredRow] < 0)
      {
        continue;
      }
      const int localReferenceRow = mappedReferenceByMeasuredRow[measuredRow] - static_cast<int>(referenceRegion[region_begin]);
      const int expectedReferenceRow = expected_reference_row_ordinal(measuredOrdinal, measuredCount, referenceCount, 0);
      if (localReferenceRow < 0 || localReferenceRow >= static_cast<int>(referenceCount) || expectedReferenceRow < 0 || expectedReferenceRow >= static_cast<int>(referenceCount))
      {
        continue;
      }
      rowBranchLabels[measuredOrdinal] = localReferenceRow - expectedReferenceRow;
    }

    std::vector<int> branchVotes;
    branchVotes.reserve(measuredCount);
    for (size_t measuredOrdinal = 0; measuredOrdinal < measuredCount; ++measuredOrdinal)
    {
      // Prefer interior rows when deciding the branch. If no interior row was
      // mapped, fall back to all mapped rows so the repair can still proceed.
      const bool edgeRow = measuredOrdinal < edgeRows || measuredOrdinal + edgeRows >= measuredCount;
      if (!edgeRow && rowBranchLabels[measuredOrdinal] != 999999)
      {
        branchVotes.push_back(rowBranchLabels[measuredOrdinal]);
      }
    }
    if (branchVotes.empty())
    {
      for (int label : rowBranchLabels)
      {
        if (label != 999999)
        {
          branchVotes.push_back(label);
        }
      }
    }
    if (branchVotes.empty())
    {
      continue;
    }

    int majorityBranch = branchVotes.front();
    int majorityCount = 0;
    for (int candidate : branchVotes)
    {
      int count = 0;
      for (int label : branchVotes)
      {
        if (label == candidate)
        {
          ++count;
        }
      }
      if (count > majorityCount || (count == majorityCount && std::abs(candidate - branchShift) < std::abs(majorityBranch - branchShift)))
      {
        majorityBranch = candidate;
        majorityCount = count;
      }
    }

    size_t repairedRows = 0;
    for (size_t measuredOrdinal = 0; measuredOrdinal < measuredCount; ++measuredOrdinal)
    {
      const bool edgeRow = measuredOrdinal < edgeRows || measuredOrdinal + edgeRows >= measuredCount;
      if (!edgeRow || rowBranchLabels[measuredOrdinal] == 999999 || rowBranchLabels[measuredOrdinal] == majorityBranch)
      {
        continue;
      }
      const int expectedReferenceRow = expected_reference_row_ordinal(measuredOrdinal, measuredCount, referenceCount, 0);
      const int repairedLocalReferenceRow = expectedReferenceRow + majorityBranch;
      if (repairedLocalReferenceRow < 0 || repairedLocalReferenceRow >= static_cast<int>(referenceCount))
      {
        continue;
      }
      const size_t measuredRow = measuredRegion[region_begin] + measuredOrdinal;
      mappedReferenceByMeasuredRow[measuredRow] = static_cast<int>(referenceRegion[region_begin]) + repairedLocalReferenceRow;
      ++repairedRows;
    }

    std::cout << "Row branch repair testedBranch=" << branchShift << " region=" << region << " orderBranchMajority=" << majorityBranch << " relativeToTested=" << (majorityBranch - branchShift) << " votes=" << branchVotes.size() << " repairedEdgeRows=" << repairedRows << std::endl;
  }
}

std::vector<std::array<double, 6>> build_ordered_row_branch_seed(const std::pair<std::vector<std::pair<double, std::vector<size_t>>>, std::vector<std::pair<double, std::vector<size_t>>>> &topology, const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference, const std::pair<std::vector<int>, std::vector<int>> &allowedCandidates, int rowShift, int phiShift)
{
  // Build a seed that assumes a coherent radial branch rather than minimizing
  // local displacement. Within each selected row pair, stripes are paired by
  // cyclic phi order. This can initialize large-DeltaR branches that a local
  // nearest-neighbor seed would avoid.
  (void) rowShift;
  std::vector<std::array<double, 6>> seed;
  for (size_t measuredRow = 0; measuredRow < topology.first.size(); ++measuredRow)
  {
    const auto &measuredIndices = topology.first[measuredRow].second;
    int referenceRow = -1;
    for (size_t measuredIndex : measuredIndices)
    {
      if (measuredIndex < allowedCandidates.second.size() && allowedCandidates.second[measuredIndex] >= 0)
      {
        referenceRow = allowedCandidates.second[measuredIndex];
        break;
      }
    }
    if (referenceRow < 0 || referenceRow >= static_cast<int>(topology.second.size()))
    {
      continue;
    }

    const auto &referenceIndices = topology.second[static_cast<size_t>(referenceRow)].second;
    if (measuredIndices.empty() || referenceIndices.empty())
    {
      continue;
    }

    for (size_t measuredPosition = 0; measuredPosition < measuredIndices.size(); ++measuredPosition)
    {
      const size_t measuredIndex = measuredIndices[measuredPosition];
      size_t referencePosition = 0;
      if (measuredIndices.size() == 1 || referenceIndices.size() == 1)
      {
        referencePosition = 0;
      }
      else
      {
        // Pair by fractional phi-order position so rows with different stripe
        // counts still seed the same angular neighborhood.
        const double rowFraction = static_cast<double>(measuredPosition) / static_cast<double>(measuredIndices.size() - 1);
        referencePosition = static_cast<size_t>(std::lround(rowFraction * static_cast<double>(referenceIndices.size() - 1)));
        referencePosition = std::min(referencePosition, referenceIndices.size() - 1);
      }

      int shiftedReferencePosition = static_cast<int>(referencePosition) + phiShift;
      shiftedReferencePosition %= static_cast<int>(referenceIndices.size());
      if (shiftedReferencePosition < 0)
      {
        shiftedReferencePosition += static_cast<int>(referenceIndices.size());
      }
      const size_t referenceIndex = referenceIndices[static_cast<size_t>(shiftedReferencePosition)];
      if (!(measuredIndex < allowedCandidates.second.size() && referenceIndex < allowedCandidates.first.size() && allowedCandidates.second[measuredIndex] >= 0 && allowedCandidates.second[measuredIndex] == allowedCandidates.first[referenceIndex]))
      {
        continue;
      }

      const auto candidate = make_assignment_candidate(measuredIndex, referenceIndex, measured, reference);
      if (assignment_candidate_within_gates(candidate))
      {
        seed.push_back(candidate);
      }
    }
  }
  return keep_unique_assignment_candidates(std::move(seed), measured.size(), reference.size());
}

std::pair<std::vector<int>, std::vector<int>> build_full_r_branch_locked_candidate_mask(const std::pair<std::vector<std::pair<double, std::vector<size_t>>>, std::vector<std::pair<double, std::vector<size_t>>>> &topology, size_t measuredCount, size_t referenceCount, int branchShift, bool applyEdgeRepair)
{
  // Branch probes compare coherent row-shift hypotheses over the full detector.
  // Each measured row is allowed to match
  // only the nearest-R reference row plus the tested branch shift.
  std::pair<std::vector<int>, std::vector<int>> allowed;
  allowed.first.assign(referenceCount, -1);
  allowed.second.assign(measuredCount, -1);
  for (size_t referenceRow = 0; referenceRow < topology.second.size(); ++referenceRow)
  {
    for (size_t referenceIndex : topology.second[referenceRow].second)
    {
      if (referenceIndex < referenceCount)
      {
        allowed.first[referenceIndex] = static_cast<int>(referenceRow);
      }
    }
  }
  std::vector<int> mappedReferenceByMeasuredRow;
  if (use_topology_row_mapping_dp)
  {
    mappedReferenceByMeasuredRow = build_monotone_row_mapping(topology, branchShift);
  }
  else
  {
    mappedReferenceByMeasuredRow = build_nearest_row_mapping(topology, branchShift);
    if (applyEdgeRepair && use_topology_edge_row_branch_repair)
    {
      repair_edge_row_branch_mapping(topology, branchShift, mappedReferenceByMeasuredRow);
    }
  }

  for (size_t measuredRow = 0; measuredRow < topology.first.size(); ++measuredRow)
  {
    const int lockedReferenceRow = measuredRow < mappedReferenceByMeasuredRow.size() ? mappedReferenceByMeasuredRow[measuredRow] : -1;
    if (lockedReferenceRow < 0 || lockedReferenceRow >= static_cast<int>(topology.second.size()))
    {
      continue;
    }
    for (size_t measuredIndex : topology.first[measuredRow].second)
    {
      if (measuredIndex >= measuredCount)
      {
        continue;
      }
      allowed.second[measuredIndex] = lockedReferenceRow;
    }
  }
  return allowed;
}

// --------------------------------------------------------------------------
// Hungarian assignment and field-guided costs
// --------------------------------------------------------------------------

std::vector<int> solve_hungarian_assignment(const std::vector<std::vector<double>> &cost)
{
  // Hungarian shortest-augmenting-path formulation for a square cost matrix.
  // The returned vector maps each matrix row to exactly one matrix column.
  const int n = static_cast<int>(cost.size());
  if (n == 0)
  {
    return {};
  }

  const double inf = std::numeric_limits<double>::infinity();
  std::vector<double> u(n + 1, 0.0);
  std::vector<double> v(n + 1, 0.0);
  std::vector<int> p(n + 1, 0);
  std::vector<int> way(n + 1, 0);

  // Add one matrix row at a time to the current optimal partial assignment.
  for (int i = 1; i <= n; i++)
  {
    p[0] = i;
    int j0 = 0;
    std::vector<double> minv(n + 1, inf);
    std::vector<char> used(n + 1, false);

    // Search an augmenting path in reduced-cost space.
    do
    {
      used[j0] = true;
      const int i0 = p[j0];
      double delta = inf;
      int j1 = 0;

      for (int j = 1; j <= n; j++)
      {
        if (used[j])
        {
          continue;
        }

        const double cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
        if (cur < minv[j])
        {
          minv[j] = cur;
          way[j] = j0;
        }
        if (minv[j] < delta)
        {
          delta = minv[j];
          j1 = j;
        }
      }

      if (!std::isfinite(delta))
      {
        break;
      }

      // Update dual potentials and remaining reduced distances.
      for (int j = 0; j <= n; j++)
      {
        if (used[j])
        {
          u[p[j]] += delta;
          v[j] -= delta;
        }
        else
        {
          minv[j] -= delta;
        }
      }
      j0 = j1;
    } while (p[j0] != 0);

    // Reverse the discovered path to augment the assignment.
    do
    {
      const int j1 = way[j0];
      p[j0] = p[j1];
      j0 = j1;
    } while (j0 != 0);
  }

  std::vector<int> row_to_col(n, -1);
  for (int j = 1; j <= n; j++)
  {
    if (p[j] > 0)
    {
      row_to_col[p[j] - 1] = j - 1;
    }
  }

  return row_to_col;
}

double global_assignment_candidate_cost(const std::array<double, 3> &measured, const std::array<double, 3> &reference, const GlobalFieldFitter &field, std::array<double, 6> &candidate)
{
  // A pair is inexpensive when both displacement components agree with the
  // current smooth field at the measured stripe position. Raw displacement is
  // used only for broad gates; it is not itself an assignment cost.
  // Use an angular residual in the field-consistency cost so a one-stripe phi
  // alias is penalized similarly at small and large radius. DeltaR remains a
  // physical cm residual because radial distortions can wander substantially
  // while still needing to agree with the smooth fitted field.

  candidate[candidate_delta_r] = measured[stripe_r] - reference[stripe_r];
  candidate[candidate_delta_phi] = wrap_delta_phi(measured[stripe_phi] - reference[stripe_phi]);
  candidate[candidate_r_delta_phi] = measured[stripe_r] * candidate[candidate_delta_phi];
  candidate[candidate_distance] = std::sqrt(candidate[candidate_delta_r] * candidate[candidate_delta_r] + candidate[candidate_r_delta_phi] * candidate[candidate_r_delta_phi]);

  if (std::abs(candidate[candidate_delta_r]) > global_assignment_max_abs_delta_r_cm)
  {
    return std::numeric_limits<double>::infinity();
  }
  if (std::abs(candidate[candidate_delta_phi]) > global_assignment_max_abs_delta_phi_rad)
  {
    return std::numeric_limits<double>::infinity();
  }
  if (!field.is_valid())
  {
    return std::numeric_limits<double>::infinity();
  }
  // Normalize radial residuals in cm and angular residuals in radians. The field
  // itself is still fitted in RDeltaPhi units, but evaluate_delta_phi converts
  // it to the angular displacement needed for radius-independent phi matching.
  const double fieldDeltaR = field.evaluate_delta_r(measured[stripe_phi], measured[stripe_r]);
  const double fieldDeltaPhi = field.evaluate_delta_phi(measured[stripe_phi], measured[stripe_r]);
  const double deltaRSigma = std::max(global_assignment_field_sigma_delta_r_cm, 1e-6);
  const double deltaPhiSigma = std::max(global_assignment_field_sigma_delta_phi_rad, 1e-6);
  const double deltaRResidual = (candidate[candidate_delta_r] - fieldDeltaR) / deltaRSigma;
  const double deltaPhiResidual = wrap_delta_phi(candidate[candidate_delta_phi] - fieldDeltaPhi) / deltaPhiSigma;
  return global_assignment_field_delta_r_weight * deltaRResidual * deltaRResidual + global_assignment_field_delta_phi_weight * deltaPhiResidual * deltaPhiResidual;
}

std::vector<std::array<double, 6>> solve_global_assignment_once(const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference, const GlobalFieldFitter &field, const std::pair<std::vector<int>, std::vector<int>> &allowedCandidates)
{
  // With a branch-locked mask, candidate sets are disjoint by reference row.
  // Solve one smaller Hungarian problem per reference row instead of one dense
  // all-stripe problem. This is exactly equivalent because no assignment in one
  // row can use a reference stripe from another row.
  if (measured.empty() || reference.empty())
  {
    return {};
  }

  int rowCount = 0;
  for (int row : allowedCandidates.first)
  {
    rowCount = std::max(rowCount, row + 1);
  }
  std::vector<std::vector<size_t>> measuredByRow(static_cast<size_t>(rowCount));
  std::vector<std::vector<size_t>> referenceByRow(static_cast<size_t>(rowCount));
  for (size_t i = 0; i < allowedCandidates.second.size(); ++i)
  {
    const int row = allowedCandidates.second[i];
    if (row >= 0 && row < rowCount)
    {
      measuredByRow[static_cast<size_t>(row)].push_back(i);
    }
  }
  for (size_t j = 0; j < allowedCandidates.first.size(); ++j)
  {
    const int row = allowedCandidates.first[j];
    if (row >= 0 && row < rowCount)
    {
      referenceByRow[static_cast<size_t>(row)].push_back(j);
    }
  }

  const double largeCost = 1.0e9;
  std::vector<std::array<double, 6>> matches;
  matches.reserve(std::min(measured.size(), reference.size()));
  for (int row = 0; row < rowCount; ++row)
  {
    const auto &measuredIndices = measuredByRow[static_cast<size_t>(row)];
    const auto &referenceIndices = referenceByRow[static_cast<size_t>(row)];
    const size_t nMeasured = measuredIndices.size();
    const size_t nReference = referenceIndices.size();
    const size_t n = nMeasured + nReference;
    if (nMeasured == 0 || nReference == 0)
    {
      continue;
    }

    std::vector<std::vector<double>> cost(n, std::vector<double>(n, largeCost));
    for (size_t localMeasured = 0; localMeasured < nMeasured; ++localMeasured)
    {
      const size_t measuredIndex = measuredIndices[localMeasured];
      for (size_t localReference = 0; localReference < nReference; ++localReference)
      {
        const size_t referenceIndex = referenceIndices[localReference];
        std::array<double, 6> candidate{};
        candidate[candidate_measured_idx] = static_cast<double>(measuredIndex);
        candidate[candidate_reference_idx] = static_cast<double>(referenceIndex);
        const double candidateCost = global_assignment_candidate_cost(measured[measuredIndex], reference[referenceIndex], field, candidate);
        if (std::isfinite(candidateCost))
        {
          cost[localMeasured][localReference] = candidateCost;
        }
      }
      cost[localMeasured][nReference + localMeasured] = global_assignment_unmatched_cost;
    }
    for (size_t localReference = 0; localReference < nReference; ++localReference)
    {
      cost[nMeasured + localReference][localReference] = 0.0;
    }
    for (size_t i = nMeasured; i < n; ++i)
    {
      for (size_t j = nReference; j < n; ++j)
      {
        cost[i][j] = 0.0;
      }
    }

    const auto assignment = solve_hungarian_assignment(cost);
    for (size_t localMeasured = 0; localMeasured < nMeasured && localMeasured < assignment.size(); ++localMeasured)
    {
      const int localReference = assignment[localMeasured];
      if (localReference < 0 || localReference >= static_cast<int>(nReference))
      {
        continue;
      }
      const size_t measuredIndex = measuredIndices[localMeasured];
      const size_t referenceIndex = referenceIndices[static_cast<size_t>(localReference)];
      std::array<double, 6> candidate{};
      candidate[candidate_measured_idx] = static_cast<double>(measuredIndex);
      candidate[candidate_reference_idx] = static_cast<double>(referenceIndex);
      const double candidateCost = global_assignment_candidate_cost(measured[measuredIndex], reference[referenceIndex], field, candidate);
      if (std::isfinite(candidateCost) && candidateCost <= global_assignment_max_final_cost)
      {
        matches.push_back(candidate);
      }
    }
  }
  return matches;
}

// --------------------------------------------------------------------------
// Field fitting, assignment iteration, and branch scoring
// --------------------------------------------------------------------------

std::vector<std::array<double, 6>> make_observations(const std::vector<std::array<double, 6>> &candidates, const std::vector<std::array<double, 3>> &measured, bool useFixedSigma)
{
  // Convert candidate arrays into GlobalFieldFitter observation arrays. The
  // measured stripe supplies the (phi, R) position and the candidate supplies
  // the measured-reference displacement.
  std::vector<std::array<double, 6>> observations;
  observations.reserve(candidates.size());
  for (const auto &candidate : candidates)
  {
    std::array<double, 6> observation;
    observation[observation_phi] = measured[static_cast<size_t>(candidate[candidate_measured_idx])][stripe_phi];
    observation[observation_r] = measured[static_cast<size_t>(candidate[candidate_measured_idx])][stripe_r];
    observation[observation_delta_r] = candidate[candidate_delta_r];
    observation[observation_r_delta_phi] = candidate[candidate_r_delta_phi];
    if (useFixedSigma)
    {
      // During assignment iteration every candidate gets the same uncertainty
      // so branch comparisons are not influenced by sample-dependent scales.
      observation[observation_sigma_delta_r] = fallback_sigma_prior_delta_r_cm;
      observation[observation_sigma_r_delta_phi] = fallback_sigma_prior_r_delta_phi_cm;
    }
    observations.push_back(observation);
  }
  return observations;
}

GlobalFieldFitter *fit_assignment_field(const std::vector<std::array<double, 6>> &candidates, const std::vector<std::array<double, 3>> &measured, const std::vector<double> &controlRPositions)
{
  // Iterative Hungarian guidance uses fixed observation uncertainties so every
  // hypothesis follows the same update rule before final cleaning.
  if (static_cast<int>(candidates.size()) < min_ml_seed_neighbors)
  {
    return nullptr;
  }

  auto observations = make_observations(candidates, measured, true);
  auto *field = new GlobalFieldFitter(observations, controlRPositions);
  if (!field->fit())
  {
    delete field;
    return nullptr;
  }

  return field;
}

GlobalFieldFitter *fit_robust_field(std::vector<std::array<double, 6>> &observations, const std::vector<double> &controlRPositions)
{
  // Final hypothesis fields use component-wise robust global scales. This same
  // helper is used by hypothesis scoring and the selected output path.
  if (static_cast<int>(observations.size()) < min_ml_seed_neighbors)
  {
    return nullptr;
  }
  std::vector<double> deltaRValues;
  std::vector<double> rDeltaPhiValues;
  deltaRValues.reserve(observations.size());
  rDeltaPhiValues.reserve(observations.size());
  for (const auto &observation : observations)
  {
    deltaRValues.push_back(observation[observation_delta_r]);
    rDeltaPhiValues.push_back(observation[observation_r_delta_phi]);
  }
  // Clamp MAD estimates so a nearly constant or highly contaminated sample
  // cannot make the regularized solve arbitrarily stiff or weak.
  const double sigmaDeltaR = std::clamp(robust_mad_sigma(deltaRValues, fallback_sigma_prior_delta_r_cm), min_sigma_prior_delta_r_cm, max_sigma_prior_delta_r_cm);
  const double sigmaRDeltaPhi = std::clamp(robust_mad_sigma(rDeltaPhiValues, fallback_sigma_prior_r_delta_phi_cm), min_sigma_prior_r_delta_phi_cm, max_sigma_prior_r_delta_phi_cm);
  for (auto &observation : observations)
  {
    observation[observation_sigma_delta_r] = sigmaDeltaR;
    observation[observation_sigma_r_delta_phi] = sigmaRDeltaPhi;
  }
  auto *field = new GlobalFieldFitter(observations, controlRPositions);
  if (!field->fit())
  {
    delete field;
    return nullptr;
  }
  return field;
}

GlobalFieldFitter *fit_robust_assignment_field(const std::vector<std::array<double, 6>> &candidates, const std::vector<std::array<double, 3>> &measured, const std::vector<double> &controlRPositions)
{
  // Convert internal candidate records into the observation representation used
  // by GlobalFieldFitter, then apply the shared robust final-fit policy.
  auto observations = make_observations(candidates, measured, false);
  return fit_robust_field(observations, controlRPositions);
}

double assignment_change_fraction(const std::vector<std::array<double, 6>> &previous, const std::vector<std::array<double, 6>> &current, size_t measuredCount)
{
  // Compare the complete assignment state of every measured stripe. The -1
  // sentinel ensures match-to-unmatched transitions count toward convergence.
  if (measuredCount == 0)
  {
    return 0.0;
  }
  std::vector<int> previousReference(measuredCount, -1);
  std::vector<int> currentReference(measuredCount, -1);
  for (const auto &candidate : previous)
  {
    if (candidate[candidate_measured_idx] < measuredCount)
    {
      previousReference[static_cast<size_t>(candidate[candidate_measured_idx])] = static_cast<int>(candidate[candidate_reference_idx]);
    }
  }
  for (const auto &candidate : current)
  {
    if (candidate[candidate_measured_idx] < measuredCount)
    {
      currentReference[static_cast<size_t>(candidate[candidate_measured_idx])] = static_cast<int>(candidate[candidate_reference_idx]);
    }
  }
  size_t changed = 0;
  for (size_t measuredIndex = 0; measuredIndex < measuredCount; ++measuredIndex)
  {
    if (previousReference[measuredIndex] != currentReference[measuredIndex])
    {
      ++changed;
    }
  }
  return static_cast<double>(changed) / static_cast<double>(measuredCount);
}

std::vector<std::array<double, 6>> clean_assignment_candidates(const std::vector<std::array<double, 6>> &candidates, const std::vector<std::array<double, 3>> &measured, size_t &rejected)
{
  // Require each match to agree with robust displacement medians from nearby
  // stripes in the same narrow radial band and broad phi neighborhood.
  rejected = 0;
  std::vector<bool> keep(candidates.size(), true);
  for (size_t a = 0; a < candidates.size(); ++a)
  {
    std::vector<double> neighborDeltaR;
    std::vector<double> neighborRDeltaPhi;
    for (size_t b = 0; b < candidates.size(); ++b)
    {
      if (a == b)
      {
        continue;
      }
      const auto &measuredA = measured[candidates[a][candidate_measured_idx]];
      const auto &measuredB = measured[candidates[b][candidate_measured_idx]];
      if (std::abs(measuredA[stripe_r] - measuredB[stripe_r]) < seed_clean_radial_window_cm && std::abs(wrap_delta_phi(measuredA[stripe_phi] - measuredB[stripe_phi])) < seed_clean_phi_window_rad)
      {
        neighborDeltaR.push_back(candidates[b][candidate_delta_r]);
        neighborRDeltaPhi.push_back(candidates[b][candidate_r_delta_phi]);
      }
    }
    // Sparse matches are removed rather than allowed to control extrapolation.
    if (static_cast<int>(neighborDeltaR.size()) < seed_clean_min_neighbors)
    {
      keep[a] = false;
      ++rejected;
      continue;
    }
    const double deltaRResidual = candidates[a][candidate_delta_r] - median_value(neighborDeltaR);
    const double rDeltaPhiResidual = candidates[a][candidate_r_delta_phi] - median_value(neighborRDeltaPhi);
    if (std::hypot(deltaRResidual, rDeltaPhiResidual) > seed_clean_max_local_residual_cm)
    {
      keep[a] = false;
      ++rejected;
    }
  }

  std::vector<std::array<double, 6>> cleaned;
  cleaned.reserve(candidates.size() - rejected);
  for (size_t i = 0; i < candidates.size(); ++i)
  {
    if (keep[i])
    {
      cleaned.push_back(candidates[i]);
    }
  }
  return cleaned;
}

std::tuple<std::vector<std::array<double, 6>>, double, double, double, double, size_t, int> run_branch_probe(const std::vector<std::array<double, 6>> &seed, const std::pair<std::vector<int>, std::vector<int>> &allowedCandidates, const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference, const std::vector<double> &controlRPositions)
{
  // Follow one seed basin to a complete assignment:
  // seed -> fixed-sigma field -> Hungarian -> repeat until stable.
  std::tuple<std::vector<std::array<double, 6>>, double, double, double, double, size_t, int> result{std::vector<std::array<double, 6>>{}, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), 1.0, 1.0, 0, 0};
  std::get<hypothesis_candidates>(result) = seed;
  if (std::get<hypothesis_candidates>(result).empty())
  {
    return result;
  }

  const int maximumIterations = std::max(1, global_assignment_max_iterations);
  for (int iteration = 0; iteration < maximumIterations; ++iteration)
  {
    GlobalFieldFitter *field = fit_assignment_field(std::get<hypothesis_candidates>(result), measured, controlRPositions);
    if (!field)
    {
      std::get<hypothesis_candidates>(result).clear();
      return result;
    }
    auto assigned = solve_global_assignment_once(measured, reference, *field, allowedCandidates);
    delete field;
    // Convergence includes matches that appear or disappear, not just changed
    // reference indices among the surviving matched subset.
    const double changedFraction = assignment_change_fraction(std::get<hypothesis_candidates>(result), assigned, measured.size());
    std::get<hypothesis_iterations>(result) = iteration + 1;
    if (assigned.empty())
    {
      std::get<hypothesis_candidates>(result).clear();
      return result;
    }
    std::get<hypothesis_candidates>(result) = std::move(assigned);
    if (changedFraction < global_assignment_convergence_fraction)
    {
      break;
    }
  }

  // Hypothesis comparison is performed only after applying the exact production
  // cleaner and the robust final-field fit.
  const size_t preCleaningCount = std::get<hypothesis_candidates>(result).size();
  std::get<hypothesis_candidates>(result) = clean_assignment_candidates(std::get<hypothesis_candidates>(result), measured, std::get<hypothesis_rejected_by_cleaning>(result));
  if (std::get<hypothesis_candidates>(result).empty())
  {
    return result;
  }
  GlobalFieldFitter *finalField = fit_robust_assignment_field(std::get<hypothesis_candidates>(result), measured, controlRPositions);
  if (!finalField)
  {
    std::get<hypothesis_candidates>(result).clear();
    return result;
  }
  // Score the cleaned field by robust residual, unmatched fraction, and the
  // fraction removed by cleaning. All hypotheses use identical weights.
  std::vector<double> residuals;
  residuals.reserve(std::get<hypothesis_candidates>(result).size());
  for (const auto &candidate : std::get<hypothesis_candidates>(result))
  {
    const auto &stripe = measured[static_cast<size_t>(candidate[candidate_measured_idx])];
    const double deltaRResidual = candidate[candidate_delta_r] - finalField->evaluate_delta_r(stripe[stripe_phi], stripe[stripe_r]);
    const double rDeltaPhiResidual = candidate[candidate_r_delta_phi] - finalField->evaluate_r_delta_phi(stripe[stripe_phi], stripe[stripe_r]);
    residuals.push_back(std::hypot(deltaRResidual, rDeltaPhiResidual));
  }
  std::get<hypothesis_median_residual>(result) = median_value(residuals);
  std::get<hypothesis_unmatched_fraction>(result) = 1.0 - static_cast<double>(std::get<hypothesis_candidates>(result).size()) / static_cast<double>(std::max<size_t>(1, measured.size()));
  std::get<hypothesis_cleaning_fraction>(result) = static_cast<double>(std::get<hypothesis_rejected_by_cleaning>(result)) / static_cast<double>(std::max<size_t>(1, preCleaningCount));
  std::get<hypothesis_score>(result) = matching_hypothesis_residual_weight * std::get<hypothesis_median_residual>(result) + matching_hypothesis_unmatched_weight * std::get<hypothesis_unmatched_fraction>(result) + matching_hypothesis_cleaning_weight * std::get<hypothesis_cleaning_fraction>(result);
  delete finalField;
  return result;
}

bool StripeComparison::build_global_pattern_matches()
{
  // Main matching driver. This function stays deliberately linear:
  // build row bookkeeping, probe branch shifts, optionally repair edges, then
  // copy the selected flattened candidates into the persistent seed-match array.
  auto &measured = m_measuredFiltered;
  auto &reference = m_referenceFiltered;
  m_seedMatches.clear();

  const auto topology = build_topology_initialization(measured, reference);
  const int minShift = matching_branch_probe_min_shift;
  const int maxShift = matching_branch_probe_max_shift;
  const std::string rowMappingMode = use_topology_row_mapping_dp ? "monotone_dp" : (use_topology_edge_row_branch_repair ? "nearest_r_selected_edge_repair" : "nearest_r");
  std::cout << "Branch probe setup for side " << m_sideName << ": measuredRows=" << topology.first.size() << " referenceRows=" << topology.second.size() << " branchProbes=" << (maxShift - minShift + 1) << " rowMapping=" << rowMappingMode << std::endl;

  std::tuple<std::vector<std::array<double, 6>>, double, double, double, double, size_t, int> bestViableResult{std::vector<std::array<double, 6>>{}, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), 1.0, 1.0, 0, 0};
  std::tuple<std::vector<std::array<double, 6>>, double, double, double, double, size_t, int> bestFallbackResult{std::vector<std::array<double, 6>>{}, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), 1.0, 1.0, 0, 0};
  std::string bestViableName;
  std::string bestFallbackName;
  int bestViableBranch = 0;
  int bestFallbackBranch = 0;

  // Probe each coherent radial branch. Each branch is independently iterated,
  // cleaned, robustly refit, and scored; the selected cleaned probe is the final
  // production matching.
  for (int branchShift = minShift; branchShift <= maxShift; ++branchShift)
  {
    const auto branchMask = build_full_r_branch_locked_candidate_mask(topology, measured.size(), reference.size(), branchShift, false);
    const auto branchSeed = build_ordered_row_branch_seed(topology, measured, reference, branchMask, branchShift, 0);
    const auto result = run_branch_probe(branchSeed, branchMask, measured, reference, m_controlRPositions);
    const std::string name = "branch_probe_row_" + std::to_string(branchShift);
    const double matchedFraction = static_cast<double>(std::get<hypothesis_candidates>(result).size()) / static_cast<double>(std::max<size_t>(1, measured.size()));
    const double cleaningSurvival = 1.0 - std::get<hypothesis_cleaning_fraction>(result);
    const bool viable = matchedFraction >= matching_branch_probe_min_matched_fraction && cleaningSurvival >= matching_branch_probe_min_cleaning_survival;

    std::cout << "Branch probe " << name << " for side " << m_sideName << ": seed=" << branchSeed.size() << " final=" << std::get<hypothesis_candidates>(result).size() << " iterations=" << std::get<hypothesis_iterations>(result) << " cleaningRejected=" << std::get<hypothesis_rejected_by_cleaning>(result) << " residualMedian=" << std::get<hypothesis_median_residual>(result) << " unmatchedFraction=" << std::get<hypothesis_unmatched_fraction>(result) << " cleaningFraction=" << std::get<hypothesis_cleaning_fraction>(result) << " totalScore=" << std::get<hypothesis_score>(result) << std::endl;
    std::cout << "Branch summary for side " << m_sideName << ": branch=" << branchShift << " bestScore=" << std::get<hypothesis_score>(result) << " bestResidual=" << std::get<hypothesis_median_residual>(result) << " matchedFraction=" << matchedFraction << " cleaningSurvival=" << cleaningSurvival << " viable=" << viable << std::endl;

    if (std::get<hypothesis_score>(result) < std::get<hypothesis_score>(bestFallbackResult))
    {
      bestFallbackResult = result;
      bestFallbackName = name;
      bestFallbackBranch = branchShift;
    }
    if (viable && std::get<hypothesis_score>(result) < std::get<hypothesis_score>(bestViableResult))
    {
      bestViableResult = result;
      bestViableName = name;
      bestViableBranch = branchShift;
    }
  }

  const bool hasViable = !std::get<hypothesis_candidates>(bestViableResult).empty();
  auto selectedResult = hasViable ? bestViableResult : bestFallbackResult;
  std::string selectedName = hasViable ? bestViableName : bestFallbackName;
  const int selectedBranchShift = hasViable ? bestViableBranch : bestFallbackBranch;
  std::pair<std::vector<int>, std::vector<int>> selectedMask = build_full_r_branch_locked_candidate_mask(topology, measured.size(), reference.size(), selectedBranchShift, false);
  if (std::get<hypothesis_candidates>(selectedResult).empty())
  {
    std::cout << "ComputeStripeComparisonMaps: Selected branch probe produced no usable assignment" << std::endl;
    return false;
  }

  if (!use_topology_row_mapping_dp && use_topology_edge_row_branch_repair)
  {
    // Edge repair is only tried after a branch has already won. That keeps the
    // broad branch search simple and uses repair only as a final local cleanup.
    const auto repairedMask = build_full_r_branch_locked_candidate_mask(topology, measured.size(), reference.size(), selectedBranchShift, true);
    const auto repairedSeed = build_ordered_row_branch_seed(topology, measured, reference, repairedMask, selectedBranchShift, 0);
    auto repairedResult = run_branch_probe(repairedSeed, repairedMask, measured, reference, m_controlRPositions);
    std::cout << "Selected branch edge repair for side " << m_sideName << ": branch=" << selectedBranchShift << " seed=" << repairedSeed.size() << " final=" << std::get<hypothesis_candidates>(repairedResult).size() << " iterations=" << std::get<hypothesis_iterations>(repairedResult) << " cleaningRejected=" << std::get<hypothesis_rejected_by_cleaning>(repairedResult) << " residualMedian=" << std::get<hypothesis_median_residual>(repairedResult) << " unmatchedFraction=" << std::get<hypothesis_unmatched_fraction>(repairedResult) << " cleaningFraction=" << std::get<hypothesis_cleaning_fraction>(repairedResult) << " totalScore=" << std::get<hypothesis_score>(repairedResult) << std::endl;
    if (!std::get<hypothesis_candidates>(repairedResult).empty() && std::get<hypothesis_score>(repairedResult) <= std::get<hypothesis_score>(selectedResult))
    {
      selectedResult = std::move(repairedResult);
      selectedMask = repairedMask;
      selectedName += "_edge_repaired";
    }
    else if (std::get<hypothesis_candidates>(repairedResult).empty())
    {
      std::cout << "Selected branch edge repair produced no usable assignment; keeping unrepaired selected branch" << std::endl;
    }
    else
    {
      std::cout << "Selected branch edge repair worsened score from " << std::get<hypothesis_score>(selectedResult) << " to " << std::get<hypothesis_score>(repairedResult) << "; keeping unrepaired selected branch" << std::endl;
    }
  }

  const auto &selectedCandidates = std::get<hypothesis_candidates>(selectedResult);
  const double selectedScore = std::get<hypothesis_score>(selectedResult);
  m_selectedBranchShift = selectedBranchShift;
  m_selectedReferenceRowByIndex = selectedMask.first;
  m_selectedAllowedReferenceRowByMeasured = selectedMask.second;
  std::vector<bool> measuredMatched(measured.size(), false);
  for (const auto &candidate : selectedCandidates)
  {
    // m_seedMatches is the stored match table used by the final field fit and
    // output trees. It repeats the stripe coordinates so later diagnostics do
    // not need to chase measured/reference indices for every value.
    measuredMatched[static_cast<size_t>(candidate[candidate_measured_idx])] = true;
    std::array<double, 9> seed{};
    seed[seed_measured_idx] = candidate[candidate_measured_idx];
    seed[seed_reference_idx] = candidate[candidate_reference_idx];
    seed[seed_phi] = measured[static_cast<size_t>(candidate[candidate_measured_idx])][stripe_phi];
    seed[seed_r] = measured[static_cast<size_t>(candidate[candidate_measured_idx])][stripe_r];
    seed[seed_reference_phi] = reference[static_cast<size_t>(candidate[candidate_reference_idx])][stripe_phi];
    seed[seed_reference_r] = reference[static_cast<size_t>(candidate[candidate_reference_idx])][stripe_r];
    seed[seed_delta_r] = seed[seed_r] - seed[seed_reference_r];
    seed[seed_delta_phi] = wrap_delta_phi(seed[seed_phi] - seed[seed_reference_phi]);
    seed[seed_r_delta_phi] = seed[seed_r] * seed[seed_delta_phi];
    m_seedMatches.push_back(seed);
  }

  const size_t unmatched = static_cast<size_t>(std::count(measuredMatched.begin(), measuredMatched.end(), false));
  std::cout << "Selected matching hypothesis " << selectedName << " for side " << m_sideName << ": score=" << selectedScore << " matches=" << m_seedMatches.size() << " unmatchedMeasured=" << unmatched << "/" << measured.size() << " selectedBranch=" << selectedBranchShift << std::endl;
  return true;
}

bool StripeComparison::build_global_field_estimates()
{
  // Refit the selected matches with robust global scales. This is the field
  // written to the output file and used for final candidate diagnostics.
  m_globalObservations.clear();
  m_globalObservations.reserve(m_seedMatches.size());
  for (const auto &seed : m_seedMatches)
  {
    std::array<double, 6> observation;
    observation[observation_phi] = seed[seed_phi];
    observation[observation_r] = seed[seed_r];
    observation[observation_delta_r] = seed[seed_delta_r];
    observation[observation_r_delta_phi] = seed[seed_r_delta_phi];
    m_globalObservations.push_back(observation);
  }
  delete m_globalFieldFitter;
  m_globalFieldFitter = fit_robust_field(m_globalObservations, m_controlRPositions);
  std::cout << "Global field estimates: built " << m_globalObservations.size() << " observations" << std::endl;
  return m_globalFieldFitter && m_globalFieldFitter->is_valid();
}

void StripeComparison::fill_candidate_diagnostics(size_t diagnosticMeasuredIndex, size_t diagnosticReferenceIndex, double &referencePhi, double &referenceR, double &deltaR, double &deltaPhi, double &rDeltaPhi, double &cost, double &residualDeltaR, double &residualDeltaPhi, double &residualRDeltaPhi, int &withinGate)
{
  // Fill one row of the candidate diagnostic tree for an arbitrary
  // measured/reference pair. The caller chooses which pair to inspect; this
  // routine computes raw displacement, gate status, field cost, and residuals.
  std::array<double, 6> candidate = make_assignment_candidate(diagnosticMeasuredIndex, diagnosticReferenceIndex, m_measuredFiltered, m_referenceFiltered);
  withinGate = assignment_candidate_within_gates(candidate) ? 1 : 0;
  cost = global_assignment_candidate_cost(m_measuredFiltered[diagnosticMeasuredIndex], m_referenceFiltered[diagnosticReferenceIndex], *m_globalFieldFitter, candidate);
  referencePhi = m_referenceFiltered[diagnosticReferenceIndex][stripe_phi];
  referenceR = m_referenceFiltered[diagnosticReferenceIndex][stripe_r];
  deltaR = candidate[candidate_delta_r];
  deltaPhi = candidate[candidate_delta_phi];
  rDeltaPhi = candidate[candidate_r_delta_phi];
  residualDeltaR = m_globalFieldFitter->evaluate_delta_r(m_measuredFiltered[diagnosticMeasuredIndex][stripe_phi], m_measuredFiltered[diagnosticMeasuredIndex][stripe_r]) - candidate[candidate_delta_r];
  residualDeltaPhi = wrap_delta_phi(m_globalFieldFitter->evaluate_delta_phi(m_measuredFiltered[diagnosticMeasuredIndex][stripe_phi], m_measuredFiltered[diagnosticMeasuredIndex][stripe_r]) - candidate[candidate_delta_phi]);
  residualRDeltaPhi = m_globalFieldFitter->evaluate_r_delta_phi(m_measuredFiltered[diagnosticMeasuredIndex][stripe_phi], m_measuredFiltered[diagnosticMeasuredIndex][stripe_r]) - candidate[candidate_r_delta_phi];
}

void StripeComparison::write_output_maps()
{
  // All output is diagnostic ROOT content. The actual matching and field have
  // already been computed before this routine starts.
  if (!m_globalFieldFitter || !m_globalFieldFitter->is_valid())
  {
    return;
  }

  TGraph prematchMeasured;
  // Filtered input positions show exactly what survived isolation and row-edge
  // cuts before any assignment was attempted.
  prematchMeasured.SetName((std::string("gr_prematch_measured_") + m_sideName).c_str());
  prematchMeasured.SetTitle((std::string("Pre-match measured stripes after isolation and topology edge-row filtering - ") + m_sideName + ";#phi [rad];R [cm]").c_str());
  prematchMeasured.SetMarkerStyle(20);
  for (size_t i = 0; i < m_measuredFiltered.size(); i++)
  {
    prematchMeasured.SetPoint(static_cast<int>(i), m_measuredFiltered[i][stripe_phi], m_measuredFiltered[i][stripe_r]);
  }
  safe_write_object(&prematchMeasured);

  TGraph prematchReference;
  prematchReference.SetName((std::string("gr_prematch_reference_") + m_sideName).c_str());
  prematchReference.SetTitle((std::string("Pre-match reference stripes after isolation and topology edge-row filtering - ") + m_sideName + ";#phi [rad];R [cm]").c_str());
  prematchReference.SetMarkerStyle(24);
  for (size_t i = 0; i < m_referenceFiltered.size(); i++)
  {
    prematchReference.SetPoint(static_cast<int>(i), m_referenceFiltered[i][stripe_phi], m_referenceFiltered[i][stripe_r]);
  }
  safe_write_object(&prematchReference);

  TGraph matchedMeasured;
  // Matched measured/reference graphs are written separately so they can be
  // overlaid with different marker styles in ROOT.
  matchedMeasured.SetName((std::string("gr_hungarian_matched_measured_") + m_sideName).c_str());
  matchedMeasured.SetTitle((std::string("Hungarian matched measured stripes in original coordinates - ") + m_sideName + ";#phi [rad];R [cm]").c_str());
  matchedMeasured.SetMarkerStyle(20);
  for (size_t i = 0; i < m_seedMatches.size(); i++)
  {
    matchedMeasured.SetPoint(static_cast<int>(i), m_seedMatches[i][seed_phi], m_seedMatches[i][seed_r]);
  }
  safe_write_object(&matchedMeasured);

  TGraph matchedReference;
  matchedReference.SetName((std::string("gr_hungarian_matched_reference_") + m_sideName).c_str());
  matchedReference.SetTitle((std::string("Hungarian matched reference stripes - ") + m_sideName + ";#phi [rad];R [cm]").c_str());
  matchedReference.SetMarkerStyle(24);
  for (size_t i = 0; i < m_seedMatches.size(); i++)
  {
    matchedReference.SetPoint(static_cast<int>(i), m_seedMatches[i][seed_reference_phi], m_seedMatches[i][seed_reference_r]);
  }
  safe_write_object(&matchedReference);

  const auto referenceRows = build_topology_rows(m_referenceFiltered);
  // Build reverse lookup tables from stripe index to row number. These support
  // row-level accounting without carrying a custom row object through the code.
  std::vector<int> referenceRowByIndex(m_referenceFiltered.size(), -1);
  std::vector<int> referencePositionByIndex(m_referenceFiltered.size(), -1);
  for (size_t row = 0; row < referenceRows.size(); ++row)
  {
    for (size_t position = 0; position < referenceRows[row].second.size(); ++position)
    {
      const size_t referenceIndex = referenceRows[row].second[position];
      if (referenceIndex < m_referenceFiltered.size())
      {
        referenceRowByIndex[referenceIndex] = static_cast<int>(row);
        referencePositionByIndex[referenceIndex] = static_cast<int>(position);
      }
    }
  }

  const auto measuredRows = build_topology_rows(m_measuredFiltered);
  std::vector<int> measuredRowByIndex(m_measuredFiltered.size(), -1);
  for (size_t row = 0; row < measuredRows.size(); ++row)
  {
    for (size_t index : measuredRows[row].second)
    {
      if (index < m_measuredFiltered.size())
      {
        measuredRowByIndex[index] = static_cast<int>(row);
      }
    }
  }

  std::vector<int> matchedReferenceStripesByRow(referenceRows.size(), 0);
  // Count how many stripes each selected branch allowed and how many were
  // actually matched. This catches branch-locking failures quickly in output.
  std::vector<int> matchedMeasuredStripesByRow(measuredRows.size(), 0);
  std::vector<int> allowedMeasuredStripesByReferenceRow(referenceRows.size(), 0);
  std::vector<int> allowedMeasuredRowsByReferenceRow(referenceRows.size(), 0);
  std::vector<std::vector<char>> allowedMeasuredRowSeen(referenceRows.size(), std::vector<char>(measuredRows.size(), false));
  for (const auto &match : m_seedMatches)
  {
    if (static_cast<size_t>(match[seed_reference_idx]) < referenceRowByIndex.size() && referenceRowByIndex[static_cast<size_t>(match[seed_reference_idx])] >= 0)
    {
      ++matchedReferenceStripesByRow[static_cast<size_t>(referenceRowByIndex[static_cast<size_t>(match[seed_reference_idx])])];
    }
    if (static_cast<size_t>(match[seed_measured_idx]) < measuredRowByIndex.size() && measuredRowByIndex[static_cast<size_t>(match[seed_measured_idx])] >= 0)
    {
      ++matchedMeasuredStripesByRow[static_cast<size_t>(measuredRowByIndex[static_cast<size_t>(match[seed_measured_idx])])];
    }
  }
  for (size_t measuredIndex = 0; measuredIndex < m_selectedAllowedReferenceRowByMeasured.size(); ++measuredIndex)
  {
    const int referenceRowForMeasured = m_selectedAllowedReferenceRowByMeasured[measuredIndex];
    if (referenceRowForMeasured < 0 || referenceRowForMeasured >= static_cast<int>(referenceRows.size()))
    {
      continue;
    }
    ++allowedMeasuredStripesByReferenceRow[static_cast<size_t>(referenceRowForMeasured)];
    if (measuredIndex < measuredRowByIndex.size() && measuredRowByIndex[measuredIndex] >= 0)
    {
      allowedMeasuredRowSeen[static_cast<size_t>(referenceRowForMeasured)][static_cast<size_t>(measuredRowByIndex[measuredIndex])] = true;
    }
  }
  for (size_t referenceRowIndex = 0; referenceRowIndex < allowedMeasuredRowSeen.size(); ++referenceRowIndex)
  {
    allowedMeasuredRowsByReferenceRow[referenceRowIndex] = static_cast<int>(std::count(allowedMeasuredRowSeen[referenceRowIndex].begin(), allowedMeasuredRowSeen[referenceRowIndex].end(), true));
  }

  TTree rowDiagnostics((std::string("t_row_assignment_diagnostics_") + m_sideName).c_str(), (std::string("Row assignment diagnostics - ") + m_sideName).c_str());
  // One tree holds both reference rows and measured rows. diagnosticKind marks
  // which side the row came from so plotting macros can split them later.
  int diagnosticKind = 0;
  int rowIndex = -1;
  double rowCenterR = 0.0;
  int rowSize = 0;
  int matchedStripeCount = 0;
  int allowedStripeCount = 0;
  int allowedRowCount = 0;
  int selectedBranch = 0;
  rowDiagnostics.Branch("kind", &diagnosticKind); // 0=reference row, 1=measured row
  rowDiagnostics.Branch("row_index", &rowIndex);
  rowDiagnostics.Branch("row_center_r", &rowCenterR);
  rowDiagnostics.Branch("row_size", &rowSize);
  rowDiagnostics.Branch("matched_stripe_count", &matchedStripeCount);
  rowDiagnostics.Branch("allowed_stripe_count", &allowedStripeCount);
  rowDiagnostics.Branch("allowed_row_count", &allowedRowCount);
  rowDiagnostics.Branch("selected_branch", &selectedBranch);
  selectedBranch = m_selectedBranchShift;
  diagnosticKind = 0;
  for (size_t row = 0; row < referenceRows.size(); ++row)
  {
    rowIndex = static_cast<int>(row);
    rowCenterR = referenceRows[row].first;
    rowSize = static_cast<int>(referenceRows[row].second.size());
    matchedStripeCount = matchedReferenceStripesByRow[row];
    allowedStripeCount = allowedMeasuredStripesByReferenceRow[row];
    allowedRowCount = allowedMeasuredRowsByReferenceRow[row];
    rowDiagnostics.Fill();
  }
  diagnosticKind = 1;
  for (size_t row = 0; row < measuredRows.size(); ++row)
  {
    rowIndex = static_cast<int>(row);
    rowCenterR = measuredRows[row].first;
    rowSize = static_cast<int>(measuredRows[row].second.size());
    matchedStripeCount = matchedMeasuredStripesByRow[row];
    allowedStripeCount = 0;
    allowedRowCount = 0;
    rowDiagnostics.Fill();
  }
  safe_write_object(&rowDiagnostics);

  TTree rowMapDiagnostics((std::string("t_row_mapping_diagnostics_") + m_sideName).c_str(), (std::string("Selected row mapping diagnostics - ") + m_sideName).c_str());
  // This tree is measured-row centered. For each measured row it records the
  // selected reference row that the branch mask allowed, plus match counts on
  // both sides of that row relation.
  int measuredRowIndex = -1;
  double measuredRowCenterR = 0.0;
  int measuredRowSize = 0;
  int allowedReferenceRow = -1;
  double allowedReferenceRowCenterR = 0.0;
  int allowedReferenceRowSize = 0;
  int measuredRowMatchedStripes = 0;
  int allowedReferenceRowMatchedStripes = 0;
  rowMapDiagnostics.Branch("measured_row_index", &measuredRowIndex);
  rowMapDiagnostics.Branch("measured_row_center_r", &measuredRowCenterR);
  rowMapDiagnostics.Branch("measured_row_size", &measuredRowSize);
  rowMapDiagnostics.Branch("allowed_reference_row", &allowedReferenceRow);
  rowMapDiagnostics.Branch("allowed_reference_row_center_r", &allowedReferenceRowCenterR);
  rowMapDiagnostics.Branch("allowed_reference_row_size", &allowedReferenceRowSize);
  rowMapDiagnostics.Branch("measured_row_matched_stripes", &measuredRowMatchedStripes);
  rowMapDiagnostics.Branch("allowed_reference_row_matched_stripes", &allowedReferenceRowMatchedStripes);
  rowMapDiagnostics.Branch("selected_branch", &selectedBranch);
  for (size_t measuredRow = 0; measuredRow < measuredRows.size(); ++measuredRow)
  {
    measuredRowIndex = static_cast<int>(measuredRow);
    measuredRowCenterR = measuredRows[measuredRow].first;
    measuredRowSize = static_cast<int>(measuredRows[measuredRow].second.size());
    measuredRowMatchedStripes = matchedMeasuredStripesByRow[measuredRow];
    allowedReferenceRow = -1;
    allowedReferenceRowCenterR = std::numeric_limits<double>::quiet_NaN();
    allowedReferenceRowSize = 0;
    allowedReferenceRowMatchedStripes = 0;
    for (size_t measuredIndex : measuredRows[measuredRow].second)
    {
      if (measuredIndex < m_selectedAllowedReferenceRowByMeasured.size() && m_selectedAllowedReferenceRowByMeasured[measuredIndex] >= 0)
      {
        allowedReferenceRow = m_selectedAllowedReferenceRowByMeasured[measuredIndex];
        break;
      }
    }
    if (allowedReferenceRow >= 0 && allowedReferenceRow < static_cast<int>(referenceRows.size()))
    {
      allowedReferenceRowCenterR = referenceRows[static_cast<size_t>(allowedReferenceRow)].first;
      allowedReferenceRowSize = static_cast<int>(referenceRows[static_cast<size_t>(allowedReferenceRow)].second.size());
      allowedReferenceRowMatchedStripes = matchedReferenceStripesByRow[static_cast<size_t>(allowedReferenceRow)];
    }
    rowMapDiagnostics.Fill();
  }
  safe_write_object(&rowMapDiagnostics);

  TTree phiDiagnostics((std::string("t_phi_assignment_diagnostics_") + m_sideName).c_str(), (std::string("Phi assignment diagnostics - ") + m_sideName).c_str());
  // Phi diagnostics compare the chosen reference stripe with its immediate
  // previous/next neighbors in the same reference row. This makes one-stripe
  // angular slips visible without rerunning the matcher.
  int matchIndex = 0;
  int measuredIndex = -1;
  int chosenReferenceIndex = -1;
  int referenceRow = -1;
  int referencePhiIndex = -1;
  int referenceRowSize = 0;
  double measuredPhi = 0.0;
  double measuredR = 0.0;
  double fittedDeltaR = 0.0;
  double fittedDeltaPhi = 0.0;
  double fittedRDeltaPhi = 0.0;
  double chosenReferencePhi = 0.0;
  double chosenReferenceR = 0.0;
  double chosenDeltaR = 0.0;
  double chosenDeltaPhi = 0.0;
  double chosenRDeltaPhi = 0.0;
  double chosenCost = 0.0;
  double chosenResidualDeltaR = 0.0;
  double chosenResidualDeltaPhi = 0.0;
  double chosenResidualRDeltaPhi = 0.0;
  int chosenWithinGate = 0;
  int prevReferenceIndex = -1;
  int prevWithinGate = 0;
  double prevReferencePhi = 0.0;
  double prevReferenceR = 0.0;
  double prevDeltaR = 0.0;
  double prevDeltaPhi = 0.0;
  double prevRDeltaPhi = 0.0;
  double prevCost = 0.0;
  double prevResidualDeltaR = 0.0;
  double prevResidualDeltaPhi = 0.0;
  double prevResidualRDeltaPhi = 0.0;
  int nextReferenceIndex = -1;
  int nextWithinGate = 0;
  double nextReferencePhi = 0.0;
  double nextReferenceR = 0.0;
  double nextDeltaR = 0.0;
  double nextDeltaPhi = 0.0;
  double nextRDeltaPhi = 0.0;
  double nextCost = 0.0;
  double nextResidualDeltaR = 0.0;
  double nextResidualDeltaPhi = 0.0;
  double nextResidualRDeltaPhi = 0.0;

  phiDiagnostics.Branch("match_index", &matchIndex);
  phiDiagnostics.Branch("measured_index", &measuredIndex);
  phiDiagnostics.Branch("chosen_reference_index", &chosenReferenceIndex);
  phiDiagnostics.Branch("reference_row", &referenceRow);
  phiDiagnostics.Branch("reference_phi_index", &referencePhiIndex);
  phiDiagnostics.Branch("reference_row_size", &referenceRowSize);
  phiDiagnostics.Branch("measured_phi", &measuredPhi);
  phiDiagnostics.Branch("measured_r", &measuredR);
  phiDiagnostics.Branch("fitted_delta_r", &fittedDeltaR);
  phiDiagnostics.Branch("fitted_delta_phi", &fittedDeltaPhi);
  phiDiagnostics.Branch("fitted_r_delta_phi", &fittedRDeltaPhi);
  phiDiagnostics.Branch("chosen_reference_phi", &chosenReferencePhi);
  phiDiagnostics.Branch("chosen_reference_r", &chosenReferenceR);
  phiDiagnostics.Branch("chosen_delta_r", &chosenDeltaR);
  phiDiagnostics.Branch("chosen_delta_phi", &chosenDeltaPhi);
  phiDiagnostics.Branch("chosen_r_delta_phi", &chosenRDeltaPhi);
  phiDiagnostics.Branch("chosen_cost", &chosenCost);
  phiDiagnostics.Branch("chosen_residual_delta_r", &chosenResidualDeltaR);
  phiDiagnostics.Branch("chosen_residual_delta_phi", &chosenResidualDeltaPhi);
  phiDiagnostics.Branch("chosen_residual_r_delta_phi", &chosenResidualRDeltaPhi);
  phiDiagnostics.Branch("chosen_within_gate", &chosenWithinGate);
  phiDiagnostics.Branch("prev_reference_index", &prevReferenceIndex);
  phiDiagnostics.Branch("prev_within_gate", &prevWithinGate);
  phiDiagnostics.Branch("prev_reference_phi", &prevReferencePhi);
  phiDiagnostics.Branch("prev_reference_r", &prevReferenceR);
  phiDiagnostics.Branch("prev_delta_r", &prevDeltaR);
  phiDiagnostics.Branch("prev_delta_phi", &prevDeltaPhi);
  phiDiagnostics.Branch("prev_r_delta_phi", &prevRDeltaPhi);
  phiDiagnostics.Branch("prev_cost", &prevCost);
  phiDiagnostics.Branch("prev_residual_delta_r", &prevResidualDeltaR);
  phiDiagnostics.Branch("prev_residual_delta_phi", &prevResidualDeltaPhi);
  phiDiagnostics.Branch("prev_residual_r_delta_phi", &prevResidualRDeltaPhi);
  phiDiagnostics.Branch("next_reference_index", &nextReferenceIndex);
  phiDiagnostics.Branch("next_within_gate", &nextWithinGate);
  phiDiagnostics.Branch("next_reference_phi", &nextReferencePhi);
  phiDiagnostics.Branch("next_reference_r", &nextReferenceR);
  phiDiagnostics.Branch("next_delta_r", &nextDeltaR);
  phiDiagnostics.Branch("next_delta_phi", &nextDeltaPhi);
  phiDiagnostics.Branch("next_r_delta_phi", &nextRDeltaPhi);
  phiDiagnostics.Branch("next_cost", &nextCost);
  phiDiagnostics.Branch("next_residual_delta_r", &nextResidualDeltaR);
  phiDiagnostics.Branch("next_residual_delta_phi", &nextResidualDeltaPhi);
  phiDiagnostics.Branch("next_residual_r_delta_phi", &nextResidualRDeltaPhi);

  for (size_t i = 0; i < m_seedMatches.size(); ++i)
  {
    const auto &match = m_seedMatches[i];
    matchIndex = static_cast<int>(i);
    measuredIndex = static_cast<int>(static_cast<size_t>(match[seed_measured_idx]));
    chosenReferenceIndex = static_cast<int>(static_cast<size_t>(match[seed_reference_idx]));
    measuredPhi = match[seed_phi];
    measuredR = match[seed_r];
    fittedDeltaR = m_globalFieldFitter->evaluate_delta_r(match[seed_phi], match[seed_r]);
    fittedDeltaPhi = m_globalFieldFitter->evaluate_delta_phi(match[seed_phi], match[seed_r]);
    fittedRDeltaPhi = m_globalFieldFitter->evaluate_r_delta_phi(match[seed_phi], match[seed_r]);
    referenceRow = static_cast<size_t>(match[seed_reference_idx]) < referenceRowByIndex.size() ? referenceRowByIndex[static_cast<size_t>(match[seed_reference_idx])] : -1;
    referencePhiIndex = static_cast<size_t>(match[seed_reference_idx]) < referencePositionByIndex.size() ? referencePositionByIndex[static_cast<size_t>(match[seed_reference_idx])] : -1;
    referenceRowSize = referenceRow >= 0 ? static_cast<int>(referenceRows[static_cast<size_t>(referenceRow)].second.size()) : 0;
    fill_candidate_diagnostics(static_cast<size_t>(match[seed_measured_idx]), static_cast<size_t>(match[seed_reference_idx]), chosenReferencePhi, chosenReferenceR, chosenDeltaR, chosenDeltaPhi, chosenRDeltaPhi, chosenCost, chosenResidualDeltaR, chosenResidualDeltaPhi, chosenResidualRDeltaPhi, chosenWithinGate);

    prevReferenceIndex = -1;
    prevWithinGate = 0;
    prevReferencePhi = prevReferenceR = prevDeltaR = prevDeltaPhi = prevRDeltaPhi = prevCost = prevResidualDeltaR = prevResidualDeltaPhi = prevResidualRDeltaPhi = std::numeric_limits<double>::quiet_NaN();
    nextReferenceIndex = -1;
    nextWithinGate = 0;
    nextReferencePhi = nextReferenceR = nextDeltaR = nextDeltaPhi = nextRDeltaPhi = nextCost = nextResidualDeltaR = nextResidualDeltaPhi = nextResidualRDeltaPhi = std::numeric_limits<double>::quiet_NaN();
    if (referenceRow >= 0 && referenceRowSize > 1 && referencePhiIndex >= 0)
    {
      // Neighbor indices wrap around the row because phi is periodic.
      const auto &rowIndices = referenceRows[static_cast<size_t>(referenceRow)].second;
      prevReferenceIndex = static_cast<int>(rowIndices[static_cast<size_t>((referencePhiIndex + referenceRowSize - 1) % referenceRowSize)]);
      nextReferenceIndex = static_cast<int>(rowIndices[static_cast<size_t>((referencePhiIndex + 1) % referenceRowSize)]);
      fill_candidate_diagnostics(static_cast<size_t>(match[seed_measured_idx]), static_cast<size_t>(prevReferenceIndex), prevReferencePhi, prevReferenceR, prevDeltaR, prevDeltaPhi, prevRDeltaPhi, prevCost, prevResidualDeltaR, prevResidualDeltaPhi, prevResidualRDeltaPhi, prevWithinGate);
      fill_candidate_diagnostics(static_cast<size_t>(match[seed_measured_idx]), static_cast<size_t>(nextReferenceIndex), nextReferencePhi, nextReferenceR, nextDeltaR, nextDeltaPhi, nextRDeltaPhi, nextCost, nextResidualDeltaR, nextResidualDeltaPhi, nextResidualRDeltaPhi, nextWithinGate);
    }
    phiDiagnostics.Fill();
  }
  safe_write_object(&phiDiagnostics);

  // Control values and support show where the field is directly constrained
  // versus reconstructed from neighboring active controls.
  TGraph controlPoints;
  controlPoints.SetName((std::string("gr_global_field_control_points_") + m_sideName).c_str());
  controlPoints.SetTitle((std::string("Global field control points - ") + m_sideName + ";#phi [rad];R [cm]").c_str());
  controlPoints.SetMarkerStyle(24);

  TGraph activeControlPoints;
  activeControlPoints.SetName((std::string("gr_global_field_active_control_points_") + m_sideName).c_str());
  activeControlPoints.SetTitle((std::string("Active global field control points - ") + m_sideName + ";#phi [rad];R [cm]").c_str());
  activeControlPoints.SetMarkerStyle(20);

  TGraph2D controlSupport;
  controlSupport.SetName((std::string("gr2_global_field_control_support_") + m_sideName).c_str());
  controlSupport.SetTitle((std::string("Global field control support - ") + m_sideName + ";#phi [rad];R [cm];Support").c_str());

  TGraph2D controlDeltaR;
  controlDeltaR.SetName((std::string("gr2_global_field_control_delta_r_") + m_sideName).c_str());
  controlDeltaR.SetTitle((std::string("Global field control #DeltaR - ") + m_sideName + ";#phi [rad];R [cm];#DeltaR [cm]").c_str());

  TGraph2D controlRDeltaPhi;
  controlRDeltaPhi.SetName((std::string("gr2_global_field_control_r_delta_phi_") + m_sideName).c_str());
  controlRDeltaPhi.SetTitle((std::string("Global field control R#Delta#phi - ") + m_sideName + ";#phi [rad];R [cm];R#Delta#phi [cm]").c_str());

  int activeIndex = 0;
  const auto fittedControlPoints = m_globalFieldFitter->control_points();
  for (size_t i = 0; i < fittedControlPoints.size(); ++i)
  {
    const auto &point = fittedControlPoints[i];
    controlPoints.SetPoint(static_cast<int>(i), point[control_point_phi], point[control_point_r]);
    controlSupport.SetPoint(static_cast<int>(i), point[control_point_phi], point[control_point_r], point[control_point_support]);
    controlDeltaR.SetPoint(static_cast<int>(i), point[control_point_phi], point[control_point_r], point[control_point_delta_r]);
    controlRDeltaPhi.SetPoint(static_cast<int>(i), point[control_point_phi], point[control_point_r], point[control_point_r_delta_phi]);
    if (point[control_point_active])
    {
      activeControlPoints.SetPoint(activeIndex++, point[control_point_phi], point[control_point_r]);
    }
  }

  safe_write_object(&controlPoints);
  safe_write_object(&activeControlPoints);
  safe_write_object(&controlSupport);
  safe_write_object(&controlDeltaR);
  safe_write_object(&controlRDeltaPhi);

  // Residuals compare cleaned observations with the exact selected hypothesis
  // field that is sampled into the production maps below.
  const std::string suffix = "_" + m_sideName;
  TH1D stripeResidualDeltaR(("h_stripe_residual_delta_r" + suffix).c_str(), ("Stripe fit residual #DeltaR" + suffix + ";fitted-observed #DeltaR [cm];Counts").c_str(), 160, -3.0, 3.0);
  TH1D stripeResidualDeltaPhi(("h_stripe_residual_delta_phi" + suffix).c_str(), ("Stripe fit residual #Delta#phi" + suffix + ";fitted-observed #Delta#phi [rad];Counts").c_str(), 160, -0.05, 0.05);
  TH1D stripeResidualRDeltaPhi(("h_stripe_residual_r_delta_phi" + suffix).c_str(), ("Stripe fit residual R#Delta#phi" + suffix + ";fitted-observed R#Delta#phi [cm];Counts").c_str(), 160, -3.0, 3.0);
  TH1D stripeResidualMagnitude(("h_stripe_residual_magnitude" + suffix).c_str(), ("Stripe fit residual magnitude" + suffix + ";sqrt((#delta#DeltaR)^{2}+(#deltaR#Delta#phi)^{2}) [cm];Counts").c_str(), 120, 0.0, 3.0);
  TH2D stripeResidualMagnitudeVsPosition(("h_stripe_residual_magnitude_vs_position" + suffix).c_str(), ("Stripe fit residual magnitude vs position" + suffix + ";measured #phi [rad];R [cm];Residual magnitude [cm]").c_str(), 80, 0.0, 2.0 * M_PI, 52, 20.0, 80.0);
  TH2D stripeResidualDeltaRVsR(("h_stripe_residual_delta_r_vs_r" + suffix).c_str(), ("Stripe #DeltaR residual vs R" + suffix + ";R [cm];fitted-observed #DeltaR [cm]").c_str(), 52, 20.0, 80.0, 160, -3.0, 3.0);
  TH2D stripeResidualDeltaPhiVsR(("h_stripe_residual_delta_phi_vs_r" + suffix).c_str(), ("Stripe #Delta#phi residual vs R" + suffix + ";R [cm];fitted-observed #Delta#phi [rad]").c_str(), 52, 20.0, 80.0, 160, -0.05, 0.05);

  stripeResidualDeltaR.SetDirectory(nullptr);
  stripeResidualDeltaPhi.SetDirectory(nullptr);
  stripeResidualRDeltaPhi.SetDirectory(nullptr);
  stripeResidualMagnitude.SetDirectory(nullptr);
  stripeResidualMagnitudeVsPosition.SetDirectory(nullptr);
  stripeResidualDeltaRVsR.SetDirectory(nullptr);
  stripeResidualDeltaPhiVsR.SetDirectory(nullptr);
  stripeResidualMagnitudeVsPosition.SetStats(false);
  stripeResidualDeltaRVsR.SetStats(false);
  stripeResidualDeltaPhiVsR.SetStats(false);

  for (const auto &match : m_seedMatches)
  {
    const double residualDeltaR = m_globalFieldFitter->evaluate_delta_r(match[seed_phi], match[seed_r]) - match[seed_delta_r];
    const double residualDeltaPhi = m_globalFieldFitter->evaluate_delta_phi(match[seed_phi], match[seed_r]) - match[seed_delta_phi];
    const double residualRDeltaPhi = m_globalFieldFitter->evaluate_r_delta_phi(match[seed_phi], match[seed_r]) - match[seed_r_delta_phi];
    const double magnitude = std::hypot(residualDeltaR, residualRDeltaPhi);
    stripeResidualDeltaR.Fill(residualDeltaR);
    stripeResidualDeltaPhi.Fill(residualDeltaPhi);
    stripeResidualRDeltaPhi.Fill(residualRDeltaPhi);
    stripeResidualMagnitude.Fill(magnitude);
    stripeResidualMagnitudeVsPosition.Fill(match[seed_phi], match[seed_r], magnitude);
    stripeResidualDeltaRVsR.Fill(match[seed_r], residualDeltaR);
    stripeResidualDeltaPhiVsR.Fill(match[seed_r], residualDeltaPhi);
  }

  safe_write_object(&stripeResidualDeltaR);
  safe_write_object(&stripeResidualDeltaPhi);
  safe_write_object(&stripeResidualRDeltaPhi);
  safe_write_object(&stripeResidualMagnitude);
  safe_write_object(&stripeResidualMagnitudeVsPosition);
  safe_write_object(&stripeResidualDeltaRVsR);
  safe_write_object(&stripeResidualDeltaPhiVsR);

  // Sample the continuous field on the standard distortion-map binning.
  auto *deltaR = new TH2D((std::string("hIntDistortionR_") + m_sideName).c_str(), (std::string("#DeltaR map, global field estimate - ") + m_sideName + ";#phi [rad];R [cm];#DeltaR [cm]").c_str(), 80, 0.0, 2.0 * M_PI, 52, 20.0, 80.0);
  auto *deltaPhi = new TH2D((std::string("hIntDistortionP_") + m_sideName).c_str(), (std::string("#Delta#phi map, global field estimate - ") + m_sideName + ";#phi [rad];R [cm];#Delta#phi [rad]").c_str(), 80, 0.0, 2.0 * M_PI, 52, 20.0, 80.0);
  auto *deltaZ = new TH2D((std::string("hIntDistortionZ_") + m_sideName).c_str(), (std::string("#DeltaZ map, global field estimate - ") + m_sideName + ";#phi [rad];R [cm];#DeltaZ [cm]").c_str(), 80, 0.0, 2.0 * M_PI, 52, 20.0, 80.0);

  for (int phiBin = 1; phiBin <= deltaR->GetNbinsX(); ++phiBin)
  {
    const double phi = deltaR->GetXaxis()->GetBinCenter(phiBin);
    for (int rBin = 1; rBin <= deltaR->GetNbinsY(); ++rBin)
    {
      const double r = deltaR->GetYaxis()->GetBinCenter(rBin);
      if (r < fit_r_min_cm || r > fit_r_max_cm)
      {
        continue;
      }
      deltaR->SetBinContent(phiBin, rBin, m_globalFieldFitter->evaluate_delta_r(phi, r));
      deltaPhi->SetBinContent(phiBin, rBin, m_globalFieldFitter->evaluate_r_delta_phi(phi, r) / r);
    }
  }

  deltaR->SetStats(false);
  deltaPhi->SetStats(false);
  deltaZ->SetStats(false);
  safe_write_object(deltaR);
  safe_write_object(deltaPhi);
  safe_write_object(deltaZ);
  delete deltaR;
  delete deltaPhi;
  delete deltaZ;
}

void StripeComparison::write_shifted_histogram(TH2 *sourceHistogram, const std::string &name, const std::string &title, double shiftSign)
{
  // Move histogram content by the fitted field. shiftSign=-1 applies the
  // correction to measured data; shiftSign=+1 distorts the reference pattern
  // into measured coordinates.
  if (!sourceHistogram || !m_globalFieldFitter || !m_globalFieldFitter->is_valid())
  {
    return;
  }

  const int nPhiBins = sourceHistogram->GetNbinsX();
  const int nRBins = sourceHistogram->GetNbinsY();
  auto *shifted = new TH2D(name.c_str(), title.c_str(), nPhiBins, sourceHistogram->GetXaxis()->GetXmin(), sourceHistogram->GetXaxis()->GetXmax(), nRBins, sourceHistogram->GetYaxis()->GetXmin(), sourceHistogram->GetYaxis()->GetXmax());
  shifted->SetDirectory(nullptr);

  for (int phiBin = 1; phiBin <= nPhiBins; ++phiBin)
  {
    const double phi = sourceHistogram->GetXaxis()->GetBinCenter(phiBin);
    for (int rBin = 1; rBin <= nRBins; ++rBin)
    {
      const double content = sourceHistogram->GetBinContent(phiBin, rBin);
      if (content == 0.0)
      {
        continue;
      }

      const double r = sourceHistogram->GetYaxis()->GetBinCenter(rBin);
      const double shiftedR = r + shiftSign * m_globalFieldFitter->evaluate_delta_r(phi, r);
      double shiftedPhi = phi + shiftSign * m_globalFieldFitter->evaluate_delta_phi(phi, r);
      // Keep shifted phi inside the source histogram range before finding the
      // destination bin.
      const double phiMin = sourceHistogram->GetXaxis()->GetXmin();
      const double phiMax = sourceHistogram->GetXaxis()->GetXmax();
      const double phiWidth = phiMax - phiMin;
      while (shiftedPhi < phiMin)
      {
        shiftedPhi += phiWidth;
      }
      while (shiftedPhi >= phiMax)
      {
        shiftedPhi -= phiWidth;
      }

      const int shiftedPhiBin = shifted->GetXaxis()->FindBin(shiftedPhi);
      const int shiftedRBin = shifted->GetYaxis()->FindBin(shiftedR);
      if (shiftedPhiBin < 1 || shiftedPhiBin > nPhiBins || shiftedRBin < 1 || shiftedRBin > nRBins)
      {
        continue;
      }
      shifted->SetBinContent(shiftedPhiBin, shiftedRBin, shifted->GetBinContent(shiftedPhiBin, shiftedRBin) + content);
    }
  }

  shifted->SetStats(false);
  safe_write_object(shifted);
  delete shifted;
}

void StripeComparison::write_corrected_measured_histogram(TH2 *measuredHistogram)
{
  // Diagnostic view: measured clusters after subtracting the fitted distortion.
  write_shifted_histogram(measuredHistogram, "hPetal_measured_corrected_" + m_sideName, "Measured cluster histogram shifted by fitted distortion map - " + m_sideName + ";#phi [rad];R [cm]", -1.0);
}

void StripeComparison::write_distorted_reference_histogram(TH2 *referenceHistogram)
{
  // Diagnostic view: reference clusters after applying the fitted distortion.
  write_shifted_histogram(referenceHistogram, "hPetal_reference_distorted_" + m_sideName, "Reference cluster histogram shifted by fitted distortion map - " + m_sideName + ";#phi [rad];R [cm]", 1.0);
}

void StripeComparison::clear()
{
  // Reset owned memory and all per-side containers so one object can process
  // both detector sides without stale state leaking between them.
  delete m_globalFieldFitter;
  m_globalFieldFitter = nullptr;
  m_sideName.clear();
  m_controlRPositions.clear();
  m_measuredFiltered.clear();
  m_referenceFiltered.clear();
  m_seedMatches.clear();
  m_globalObservations.clear();
  m_selectedBranchShift = 0;
  m_selectedReferenceRowByIndex.clear();
  m_selectedAllowedReferenceRowByMeasured.clear();
}

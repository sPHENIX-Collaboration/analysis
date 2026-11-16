#ifndef TPCCALIB_STRIPECOMPARISON_H
#define TPCCALIB_STRIPECOMPARISON_H

#include "GlobalFieldFitter.h"
#include "StripeMatchingTypes.h"

#include <cstddef>
#include <string>
#include <array>
#include <vector>

class TH2;

// A selected seed match is std::array<double, 9>.
// It stores measured/reference indices and the displacement values that seed
// the global field fit. This replaces the old SeedMatch record.
inline constexpr int seed_measured_idx = 0;
inline constexpr int seed_reference_idx = 1;
inline constexpr int seed_phi = 2;
inline constexpr int seed_r = 3;
inline constexpr int seed_reference_phi = 4;
inline constexpr int seed_reference_r = 5;
inline constexpr int seed_delta_r = 6;
inline constexpr int seed_delta_phi = 7;
inline constexpr int seed_r_delta_phi = 8;

class StripeComparison {
public:
  ~StripeComparison();

  // Sets side name and stores radial controls for one detector side.
  bool initialize(const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference, int side, const std::vector<double> &controlRPositions);

  // Removes isolated stripes and topology edge rows before matching.
  bool filter_isolated_inputs(const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference);

  // Selects measured/reference stripe pairs.
  bool build_global_pattern_matches();

  // Builds the final global distortion field from selected pairs.
  bool build_global_field_estimates();

  // Writes ROOT diagnostics and distortion maps.
  void write_output_maps();

  // Writes histograms shifted by the fitted field.
  void write_corrected_measured_histogram(TH2 *measuredHistogram);
  void write_distorted_reference_histogram(TH2 *referenceHistogram);

  // Clears side-local state.
  void clear();

private:
  // Fills one row of the phi-assignment diagnostic tree.
  void fill_candidate_diagnostics(size_t diagnosticMeasuredIndex, size_t diagnosticReferenceIndex, double &referencePhi, double &referenceR, double &deltaR, double &deltaPhi, double &rDeltaPhi, double &cost, double &residualDeltaR, double &residualDeltaPhi, double &residualRDeltaPhi, int &withinGate);

  // Applies a fitted shift to every filled bin of a source histogram.
  void write_shifted_histogram(TH2 *sourceHistogram, const std::string &name, const std::string &title, double shiftSign);

  // Human-readable side label: negz or posz.
  std::string m_sideName;

  // Radial grid controls from lamination cleaning.
  std::vector<double> m_controlRPositions;

  // Filtered measured/reference stripes, indexed with stripe_*.
  std::vector<std::array<double, 3>> m_measuredFiltered;
  std::vector<std::array<double, 3>> m_referenceFiltered;

  // Selected matches and final field observations.
  std::vector<std::array<double, 9>> m_seedMatches;
  std::vector<std::array<double, 6>> m_globalObservations;

  // Diagnostics for the selected branch hypothesis.
  int m_selectedBranchShift = 0;
  std::vector<int> m_selectedReferenceRowByIndex;
  std::vector<int> m_selectedAllowedReferenceRowByMeasured;

  // Owned fitter for the current detector side.
  GlobalFieldFitter *m_globalFieldFitter = nullptr;
};

#endif

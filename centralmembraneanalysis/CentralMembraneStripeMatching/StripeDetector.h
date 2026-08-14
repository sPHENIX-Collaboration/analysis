#ifndef TPCCALIB_STRIPEDETECTOR_H
#define TPCCALIB_STRIPEDETECTOR_H

#include "StripeMatchingTypes.h"

#include <cstddef>
#include <array>
#include <vector>

class TH2;

// A flood-fill bin is std::array<double, 5>.
// Entries are original histogram bin x/y plus physical phi/R/content.
inline constexpr int component_bin_x = 0;
inline constexpr int component_bin_y = 1;
inline constexpr int component_phi = 2;
inline constexpr int component_r = 3;
inline constexpr int component_content = 4;

class StripeDetector {
public:
  // Finds stripe centroids in one input histogram.
  // Output stripes are std::array<double, 3> indexed with stripe_*.
  bool detect(TH2 *histogram, std::vector<std::array<double, 3>> &stripes);

  // Deletes the temporary cloned histogram.
  void clear();

private:
  // Sets dimensions and clears output/state for a new detection pass.
  bool initialize(TH2 *histogram, std::vector<std::array<double, 3>> &stripes);

  // Clones the input histogram so flood fill can work on a detached copy.
  bool create_working_histogram(TH2 *histogram);

  // Collects bins above the seed threshold, grouped by phi bin.
  void collect_seed_bins(TH2 *histogram);

  // Grows connected components from the seed bins.
  void build_connected_components();

  // Converts connected components into one centroid per stripe.
  void convert_components_to_raw_stripes(TH2 *histogram);

  // Wraps phi-bin arithmetic around the periodic histogram axis.
  static int wrapped_phi_bin(int bin, int nBinsX);

  // Normalizes a physical phi value back into the histogram x-axis range.
  static void normalize_phi_to_axis(TH2 *histogram, double &phi);

  // Temporary histogram used only during one detect() call.
  TH2 *m_workingHist = nullptr;

  // Cached histogram dimensions.
  int m_nBinsX = 0;
  int m_nBinsY = 0;

  // Seed bins and connected components using component_* array indices.
  std::vector<std::vector<std::array<double, 5>>> m_seedBinsByPhi;
  std::vector<std::vector<std::array<double, 5>>> m_components;

  // Final raw stripe candidates before duplicate removal.
  std::vector<std::array<double, 3>> m_rawStripes;
  size_t m_nSeedBins = 0;
};

#endif

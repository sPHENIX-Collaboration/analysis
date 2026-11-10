// Tell emacs that this is a C++ source
// -*- C++ -*-.
#ifndef TPCHADRONRECO_TPCTRACKACTSFITTER_H
#define TPCHADRONRECO_TPCTRACKACTSFITTER_H

#include "TpcTrackFit.h"

#include <memory>
#include <string>
#include <vector>

class PHCompositeNode;

namespace tpchadronreco
{
  // ACTS Kalman fit of TPC pattern-track points.
  //
  // Every point gets its own Acts::CylinderSurface (z axis, radius equal to the
  // point radius), so the measurement is exactly (r*phi, z) and nothing is
  // projected onto the ACTS TPC geometry. The fit uses the DirectNavigator,
  // the same PHField as TpcTrackKalmanFitter (config.magnetic_field), and no
  // material effects. ActsGeometry is only needed for the ACTS contexts.
  //
  // The output fills TpcKalmanResult in the same convention as
  // TpcTrackKalmanFitter, so propagation, PCA and vertex constraints downstream
  // are unchanged. Points with use_in_kalman == false, or above the hard robust
  // chi2 cut, are kept as ACTS outliers and still receive a state.
  class TpcTrackActsFitter
  {
   public:
    // Returns nullptr when the ActsGeometry node is missing.
    static std::unique_ptr<TpcTrackActsFitter> create(PHCompositeNode *topNode);

    ~TpcTrackActsFitter();

    bool fit(const std::vector<TpcTrackPoint> &points,
             int charge,
             const TpcKalmanConfig &config,
             TpcKalmanResult &result,
             double mass_gev = 0.13957039) const;

    // Fit counters: successes, failure reasons, outliers, charge disagreement.
    void print_summary(const std::string &prefix) const;

   private:
    bool fit_impl(const std::vector<TpcTrackPoint> &points,
                  int charge,
                  const TpcKalmanConfig &config,
                  TpcKalmanResult &result,
                  double mass_gev) const;

    struct Impl;
    explicit TpcTrackActsFitter(std::unique_ptr<Impl> impl);
    std::unique_ptr<Impl> m_impl;
  };
}  // namespace tpchadronreco

#endif

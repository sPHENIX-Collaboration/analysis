#ifndef TPCCALIB_TpcCentralMembraneStripeMatching_H
#define TPCCALIB_TpcCentralMembraneStripeMatching_H

#include "StripeComparison.h"
#include "StripeDetector.h"
#include <tpc/TpcDistortionCorrection.h>
#include <tpc/TpcDistortionCorrectionContainer.h>
#include <fun4all/SubsysReco.h>

#include <string>
#include <array>
#include <vector>

class LaserClusterContainer;
class PHCompositeNode;

class TH2;

// Fun4All subsystem wrapper around the flattened stripe analysis.
//
// This class handles framework tasks: fetch nodes, fill measured/reference
// histograms, run the detector and comparison helpers, and write the output
// file at End(). The matching math itself lives in StripeComparison.
class TpcCentralMembraneStripeMatching : public SubsysReco {
public:
  TpcCentralMembraneStripeMatching(const std::string &name = "TpcCentralMembraneStripeMatching");
  ~TpcCentralMembraneStripeMatching() override = default;

  // Output ROOT file for distortion maps and diagnostics.
  void setOutputfile(const std::string &outputfile);

  // ROOT file containing the measured-coordinate reference stripe histogram.
  void setReferenceStripeFile(const std::string &stripePatternFile)
  {
    m_referenceStripePatternFile = stripePatternFile;
  }

  // ROOT file containing the ideal stripe pattern, used when parameters request
  // the ideal pattern instead of the measured-coordinate reference file.
  void setIdealStripeFile(const std::string &stripePatternFile)
  {
    m_idealStripePatternFile = stripePatternFile;
  }

  void setStaticCorrectionFile(const std::string &staticCorrectionFile)
  {
    m_staticCorrectionFile = staticCorrectionFile;
  }

  void setFillReferenceHistogramsOnly()
  {
    m_fillReferenceHistogramsOnly = true;
  }

  TH2 *cloneReferenceHistogram(int side, const std::string &name) const;

  // Fun4All lifecycle hooks.
  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int End(PHCompositeNode *topNode) override;

private:
  // Pull node pointers needed for cluster input and optional distortion maps.
  int GetNodes(PHCompositeNode *topNode);

  // Allocate measured histograms with the binning configured below.
  void CreateMeasuredHistograms();

  // Allocate reference histograms when building a measured-coordinate reference.
  void CreateReferenceHistograms();

  // Read reference histograms from the selected reference or ideal-pattern file.
  void LoadReferenceHistograms();

  // Shared cluster filling for measured/reference histogram arrays.
  void FillClusterHistograms(int histogramSet);

  // Convert laser clusters into side-separated phi/R occupancy histograms.
  void FillMeasuredHistograms();

  // Fill measured-coordinate reference histograms from laser clusters.
  void FillReferenceHistograms();

  // Put cluster phi values into the histogram convention used by this analysis.
  double NormalizeClusterPhi(double phi) const;

  // Run StripeComparison for one detector side after stripe detection.
  void ComputeStripeComparisonMaps(const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference, const std::vector<double> &measuredRadialGapCenters, int side);

  // Writes measured clusters shifted by the static distortion map for diagnostics.
  void WriteStaticCorrectedMeasuredHistogram(int side);

  // Apply lamination cleaning to histograms before stripe finding.
  int CleanClusterHistograms();

  // Finds stripe centers from TH2 input using only flat arrays.
  StripeDetector m_detector;

  // Output path configured by the macro or by a calling job.
  std::string m_outputfile{"CMDistortionCorrections_idealtpccoordinatesTEST.root"};

  // Framework node pointers. They are owned by Fun4All, not by this class.
  LaserClusterContainer *m_laserClusterContainer = nullptr;
  TpcDistortionCorrectionContainer *m_dcc_in_module_edge = nullptr;
  TpcDistortionCorrectionContainer *m_dcc_in_static = nullptr;
  TpcDistortionCorrection m_distortionCorrection;

  // Basic histogram and event counters.
  unsigned int m_minClusterLayers = 1;
  int m_phiBins = 8000;
  int m_rBins = 500;
  double m_rMin = 30.0;
  double m_rMax = 80.0;
  long long m_processedEvents = 0;
  long long m_filledClusters[2]{0, 0};
  long long m_filledReferenceClusters[2]{0, 0};
  bool m_fillReferenceHistogramsOnly = false;

  // Histogram arrays use [measured/reference][side].
  TH2 *m_hPetal[2][2]{nullptr};        // [measured/reference][side]
  TH2 *m_hPetalCleaned[2][2]{nullptr}; // [measured/reference][side]

  // Detected stripe arrays use [measured/reference][side]. Each stripe is
  // std::array<double, 3> with slots documented in StripeMatchingTypes.h.
  std::vector<std::array<double, 3>> m_stripes[2][2]; // [measured/reference][side]

  // Radial controls derived while masking laminations.
  std::vector<double> m_lamination_radial_gap_centers[2][2];

  // Reused comparison helper. clear() is called inside initialize() for each
  // side so previous-side state is removed before the next side begins.
  StripeComparison m_comparison;

  // ********************** Field off beam off reference *************** //

  // std::string m_referenceStripePatternFile = "/sphenix/user/dloomis/DistortionsAnalysis/laminationfits/"
  //                                            "78242_fieldoffbeamoff/idealtpccoordinates_updatedfitstripereference/"
  //                                            "Laminations_run3auau_new_nocdbtag_v001-00078242.root";

  // ******************************************************************* //

  // ********************** Field on beam off reference *************** //
  std::string m_referenceStripePatternFile = "/sphenix/user/dloomis/DistortionsAnalysis/laminationfits/"
                                             "fieldonbeamoff_all/idealtpccoordinates_hugoupdatedstaticdistortionmap/"
                                             "laminations.root";

  // ******************************************************************* //

  // ************************** Ideal Stripe Pattern ****************** //
  std::string m_idealStripePatternFile = "/sphenix/user/dloomis/Distortions/StripePatterns/"
                                         "CMStripePattern_ideal.root";

  std::string m_staticCorrectionFile = "/sphenix/user/dloomis/Distortions/DistortionMaps_Static/"
                                       "static_only_inverted_10-new_hugoupdate.root";

  // ******************************************************************* //
};

#endif

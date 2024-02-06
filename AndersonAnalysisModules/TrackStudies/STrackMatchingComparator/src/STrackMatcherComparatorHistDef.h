// ----------------------------------------------------------------------------
// 'STrackMatcherComparatorHistDef.h'
// Derek Anderson
// 02.01.2024
//
// Struct to hold histogram info for 'STrackMatcherComparator' module.
// ----------------------------------------------------------------------------

#ifndef STRACKMATCHERCOMPARATORHISTDEF_H
#define STRACKMATCHERCOMPARATORHISTDEF_H

// c++ utilities
#include <string>
#include <vector>
#include <utility>

// make common namespaces implicit
using namespace std;



// STrackMatcherComparatorHistDef definition ----------------------------------

struct STrackMatcherComparatorHistDef {

  // output histogram base names
  vector<vector<string>> vecNameBase = {
    {"hTruthNumTot",  "hTrackNumTot",  "hWeirdNumTot",  "hNormNumTot"},
    {"hTruthNumIntt", "hTrackNumIntt", "hWeirdNumIntt", "hNormNumIntt"},
    {"hTruthNumMvtx", "hTrackNumMvtx", "hWeirdNumMvtx", "hNormNumMvtx"},
    {"hTruthNumTpc",  "hTrackNumTpc",  "hWeirdNumTpc",  "hNormNumTpc"},
    {"hTruthRatTot",  "hTrackRatTot",  "hWeirdRatTot",  "hNormRatTot"},
    {"hTruthRatIntt", "hTrackRatIntt", "hWeirdRatIntt", "hNormRatIntt"},
    {"hTruthRatMvtx", "hTrackRatMvtx", "hWeirdRatMvtx", "hNormRatMvtx"},
    {"hTruthRatTpc",  "hTrackRatTpc",  "hWeirdRatTpc",  "hNormRatTpc"},
    {"hTruthPhi",     "hTrackPhi",     "hWeirdPhi",     "hNormPhi"},
    {"hTruthEta",     "hTrackEta",     "hWeirdEta",     "hNormEta"},
    {"hTruthPt",      "hTrackPt",      "hWeirdPt",      "hNormPt"},
    {"hTruthFrac",    "hTrackFrac",    "hWeirdFrac",    "hNormFrac"},
    {"hTruthQual",    "hTrackQual",    "hWeirdQual",    "hNormQual"},
    {"hTruthPtErr",   "hTrackPtErr",   "hWeirdPtErr",   "hNormPtErr"},
    {"hTruthEtaErr",  "hTrackEtaErr",  "hWeirdEtaErr",  "hNormEtaErr"},
    {"hTruthPhiErr",  "hTrackPhiErr",  "hWeirdPhiErr",  "hNormPhiErr"},
    {"hTruthPtRes",   "hTrackPtRes",   "hWeirdPtRes",   "hNormPtRes"},
    {"hTruthEtaRes",  "hTrackEtaRes",  "hWeirdEtaRes",  "hNormEtaRes"},
    {"hTruthPhiRes",  "hTrackPhiRes",  "hWeirdPhiRes",  "hNormPhiRes"}
  };

  // 2D histogram name modifiers
  vector<string> vecVsModifiers = {
    "VsTruthPt",
    "VsNumTpc"
  };

  // axis titles
  vector<string> vecBaseAxisVars = {
    "N^{tot} = N_{hit}^{mvtx} + N_{hit}^{intt} + N_{clust}^{tpc}",
    "N_{hit}^{intt}",
    "N_{hit}^{mvtx}",
    "N_{clust}^{tpc}",
    "N_{reco}^{tot} / N_{true}^{tot}",
    "N_{reco}^{intt} / N_{true}^{intt}",
    "N_{reco}^{mvtx} / N_{true}^{mvtx}",
    "N_{reco}^{tpc} / N_{true}^{tpc}",
    "#varphi",
    "#eta",
    "p_{T} [GeV/c]",
    "p_{T}^{reco} / p_{T}^{true}",
    "#chi^{2} / ndf",
    "#deltap_{T} / p_{T}^{reco}",
    "#delta#eta / #eta^{reco}",
    "#delta#varphi / #varphi^{reco}",
    "|p_{T}^{reco} - p_{T}^{true}| / p_{T}^{true}",
    "|#eta^{reco} - #eta^{true}| / #eta^{true}",
    "|#varphi^{reco} - #varphi^{true}| / #varphi^{true}"
  };
  vector<string> vecVsAxisVars = {
    "p_{T}^{true} [GeV/c]",
    "N_{clust}^{tpc}"
  };

  // output histogram no. of bins
  uint32_t nNumBins  = 101;
  uint32_t nRatBins  = 120;
  uint32_t nEtaBins  = 80;
  uint32_t nPhiBins  = 360;
  uint32_t nPtBins   = 101;
  uint32_t nFracBins = 220;
  uint32_t nQualBins = 210;
  uint32_t nResBins  = 110;

  // output histogram bin ranges
  pair<float, float> xNumBins  = {-0.5,  100.5};
  pair<float, float> xRatBins  = {-0.5,  5.5};
  pair<float, float> xEtaBins  = {-2.,   2.};
  pair<float, float> xPhiBins  = {-3.15, 3.15};
  pair<float, float> xPtBins   = {-0.5,  100.5};
  pair<float, float> xFracBins = {-0.5,  10.5};
  pair<float, float> xQualBins = {-0.5,  20.5};
  pair<float, float> xResBins  = {-5.5,  5.5};

  // construct list of 1d binning
  vector<tuple<uint32_t, pair<float, float>>> GetVecHistBins() {

    vector<tuple<uint32_t, pair<float, float>>> vecBaseHistBins = {
      make_tuple(nNumBins,  xNumBins),
      make_tuple(nNumBins,  xNumBins),
      make_tuple(nNumBins,  xNumBins),
      make_tuple(nNumBins,  xNumBins),
      make_tuple(nRatBins,  xRatBins),
      make_tuple(nRatBins,  xRatBins),
      make_tuple(nRatBins,  xRatBins),
      make_tuple(nRatBins,  xRatBins),
      make_tuple(nPhiBins,  xPhiBins),
      make_tuple(nEtaBins,  xEtaBins),
      make_tuple(nPtBins,   xPtBins),
      make_tuple(nFracBins, xFracBins),
      make_tuple(nQualBins, xQualBins),
      make_tuple(nResBins,  xResBins),
      make_tuple(nResBins,  xResBins),
      make_tuple(nResBins,  xResBins),
      make_tuple(nResBins,  xResBins),
      make_tuple(nResBins,  xResBins),
      make_tuple(nResBins,  xResBins)
    };
    return vecBaseHistBins;

  }  // end 'GetVecHistBins()'

  // contruct list of 2d x-axis binning
  vector<tuple<uint32_t, pair<float, float>>> GetVecVsHistBins() {

    vector<tuple<uint32_t, pair<float, float>>> vecVsHistBins = {
      make_tuple(nPtBins,  xPtBins),
      make_tuple(nNumBins, xNumBins)
    };
    return vecVsHistBins;

  }  // end 'GetVecVsHistBins()'

};

#endif

// end ------------------------------------------------------------------------


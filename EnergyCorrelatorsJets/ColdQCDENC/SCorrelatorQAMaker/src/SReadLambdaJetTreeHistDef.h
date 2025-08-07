// ----------------------------------------------------------------------------
// 'ReadLambdaJetTreeHistDef.h'
// Derek Anderson
// 03.11.2024
//
// SCorrelatorQAMaker plugin to read lambda-tagged jet
// trees and draw plots and calculate the total no.
// of lambdas.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SREADLAMBDAJETTREEHISTDEF_H
#define SCORRELATORQAMAKER_SREADLAMBDAJETTREEHISTDEF_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SReadLambdaJetTreeHistDef definition -------------------------------------

  struct SReadLambdaJetTreeHistDef {

    // histogram base names
    vector<string> vecTypeNames = {
      "Lam",
      "LeadLam",
      "LamInMultiJet",
      "Jet",
      "LamJet",
      "LeadLamJet",
      "MultiLamJet",
      "HighestPtJet"
    };
    vector<string> vecBaseNames = {
      "hEta",
      "hEne",
      "hPt",
      "hDeltaPhi",
      "hDeltaEta",
      "hDeltaR",
      "hFrac",
      "hNLam",
      "hNCst",
      "hPerLam"
    };
    vector<string> vecVsMods = {
      "VsEta",
      "VsEne",
      "VsPt",
      "VsDeltaPhi",
      "VsDeltaEta"
    };

    // event histogram names and titles
    vector<pair<string, string>> vecEvtNameAndTitles = {
      make_pair("hNumJet",         ";N_{jet};counts"),
      make_pair("hNumTagJet",      ";N_{jet};counts"),
      make_pair("hNumLeadLamJet",  ";N_{jet};counts"),
      make_pair("hNumMultiLamJet", ";N_{jet};counts"),
      make_pair("hNumLambda",      ";N_{#Lambda};counts"),
      make_pair("hNumLambdaInJet", ";N_{#Lambda};counts"),
      make_pair("hNumLeadLambda",  ";N_{#Lambda};counts")
    };

    // no. of histogram binning
    size_t nNumBins  = 100;
    size_t nFracBins = 500;
    size_t nEneBins  = 100;
    size_t nEtaBins  = 80;
    size_t nDfBins   = 180;
    size_t nDhBins   = 160;
    size_t nDrBins   = 500;

    // histogram ranges
    pair<float, float> rNumBins  = {0.0,   100.};
    pair<float, float> rFracBins = {0.0,   5.0};
    pair<float, float> rEneBins  = {0.0,   100.};
    pair<float, float> rEtaBins  = {-2.0,  2.0};
    pair<float, float> rDfBins   = {-3.15, 3.15};
    pair<float, float> rDhBins   = {-4.,   4.};
    pair<float, float> rDrBins   = {0.0,   5.0};

    // axis definitions
    vector<tuple<string, size_t, pair<float, float>>> vecAxisDef = {
      make_tuple("#eta",                nEtaBins,  rEtaBins),
      make_tuple("E [GeV/c]",           nEneBins,  rEneBins),
      make_tuple("p_{T} [GeV/c]",       nEneBins,  rEneBins),
      make_tuple("#Delta#varphi",       nDfBins,   rDfBins),
      make_tuple("#Delta#eta",          nDhBins,   rDhBins),
      make_tuple("#Deltar",             nDrBins,   rDrBins),
      make_tuple("z = p / p^{jet}",     nFracBins, rFracBins),
      make_tuple("N_{#Lambda}",         nNumBins,  rNumBins),
      make_tuple("N_{cst}",             nNumBins,  rNumBins),
      make_tuple("N_{#Lambda}/N_{cst}", nFracBins, rFracBins)
    };
    vector<tuple<string, size_t, pair<float, float>>> vecVsDef = {
      make_tuple("#eta",            nEtaBins,  rEtaBins),
      make_tuple("E [GeV/c]",       nEneBins,  rEneBins),
      make_tuple("p_{T} [GeV/c]",   nEneBins,  rEneBins),
      make_tuple("#Delta#varphi",   nDfBins,   rDfBins),
      make_tuple("#Delta#eta",      nDhBins,   rDhBins)
    };

  };  // end SReadLambdaJetTreeHistDef

  // variables to histogram
  struct Hist {
    double eta  = -1.;
    double ene  = -1.;
    double mom  = -1.;
    double pt   = -1.;
    double df   = -1.;
    double dh   = -1.;
    double dr   = -1.;
    double z    = -1.;
    double nlam = -1.;
    double ncst = -1.;
    double plam = -1.;
  };  // end Hist

  // x-axis variables for 2d histograms
  struct VsVar {
    double eta = -1.;
    double ene = -1.;
    double mom = -1.;
    double pt  = -1.;
    double df  = -1.;
    double dh  = -1.;
  };  // end VsVar

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------


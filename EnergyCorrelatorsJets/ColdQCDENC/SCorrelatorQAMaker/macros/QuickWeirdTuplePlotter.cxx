// ----------------------------------------------------------------------------
// 'QuickWeirdTuplePlotter.cxx
// Derek Anderson
// 10.26.2023
//
// Use to quick plot some leaves from the weird tracks ntuple.
// ----------------------------------------------------------------------------

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <TH2.h>
#include <TFile.h>
#include <TError.h>
#include <TNtuple.h>
#include <TDirectory.h>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDF/HistoModels.hxx>

// make common namespaces implicit
using namespace std;
using namespace ROOT;

// set up aliases
using TH2Def = ROOT::RDF::TH2DModel;



// quickly plot variables vs. delta-r froma tuple -----------------------------

void QuickWeirdTuplePlotter() {

  // lower verbosity
  gErrorIgnoreLevel = kError;
  cout << "\n  Beginning weird track tuple plotting..." << endl;

  // options, histogram binning, and histogram definitions --------------------

  // i/o parameters
  const string sOutput  = "weirdTracksVsDr.pp200py8jet10run8.d27m10y2023.root";
  const string sInput   = "output/condor/final_merge/correlatorJetTree.pp200py8jet10run8_roughCutsWithWeirdTrackTuple.d27m10y2023.root";
  const string sInTuple = "ntWeirdTracks";

  // cuts to apply, drawing options & delta-r leaf
  const string sCut("");
  const string sDeltaR("deltartrack");

  // deltra-r binning
  const size_t         nDrBins(75);
  const vector<double> drBinEdges = {
    1e-05,
    1.16591e-05,
    1.35936e-05,
    1.58489e-05,
    1.84785e-05,
    2.15443e-05,
    2.51189e-05,
    2.92864e-05,
    3.41455e-05,
    3.98107e-05,
    4.64159e-05,
    5.4117e-05,
    6.30957e-05,
    7.35642e-05,
    8.57696e-05,
    0.0001,
    0.000116591,
    0.000135936,
    0.000158489,
    0.000184785,
    0.000215443,
    0.000251189,
    0.000292864,
    0.000341455,
    0.000398107,
    0.000464159,
    0.00054117,
    0.000630957,
    0.000735642,
    0.000857696,
    0.001,
    0.00116591,
    0.00135936,
    0.00158489,
    0.00184785,
    0.00215443,
    0.00251189,
    0.00292864,
    0.00341455,
    0.00398107,
    0.00464159,
    0.0054117,
    0.00630957,
    0.00735642,
    0.00857696,
    0.01,
    0.0116591,
    0.0135936,
    0.0158489,
    0.0184785,
    0.0215443,
    0.0251189,
    0.0292864,
    0.0341455,
    0.0398107,
    0.0464159,
    0.054117,
    0.0630957,
    0.0735642,
    0.0857696,
    0.1,
    0.116591,
    0.135936,
    0.158489,
    0.184785,
    0.215443,
    0.251189,
    0.292864,
    0.341455,
    0.398107,
    0.464159,
    0.54117,
    0.630957,
    0.735642,
    0.857696,
    1
  };  // end delta-r bin edges

  // binning accessor
  enum Var {VNum, VEne, VEta, VPhi, VDca, VVtx, VQual, VDPt, VFrac};

  // other variable binning
  //   <0> = no. of bins
  //   <1> = 1st bin lower edge
  //   <2> = last bin upper edge
  vector<tuple<size_t, double, double>> vecBins = {
    make_tuple(100,    0.,   100.),
    make_tuple(200,    0.,   100.),
    make_tuple(400,   -2.,   2.),
    make_tuple(360,   -3.15, 3.15),
    make_tuple(10000, -10.,  10.),
    make_tuple(10000, -10.,  10.),
    make_tuple(100,    0.,   10.),
    make_tuple(5000,   0.,   5.),
    make_tuple(5000,   0.,   5.)
  };

  // histogram accessor 
  enum Hist {HPt, HPFrac, HEta, HPhi, HEne, HDcaXY, HDcaZ, HVtxX, HVtxY, HVtxZ, HQual, HDPt, HNLMvtx, HNLIntt, HNLTpc, HNCMvtx, HNCIntt, HNCTpc, HNKey, HNSame, HNFrac};

  // histogram titles
  const vector<string> vecTitles = {
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};p_{T}^{trk} [GeV]",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};p_{T,i}^{trk} / p_{T,j}^{trk}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};#eta^{trk}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};#varphi^{trk}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};E_{trk}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};DCA_{xy} [cm]",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};DCZ_{z} [cm]",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};v_{x} [cm]",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};v_{y} [cm]",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};v_{z} [cm]",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};#chi^{2}/ndf",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};#deltap_{T}^{trk}/p_{T}^{trk}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};N_{layer}^{mvtx}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};N_{layer}^{intt}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};N_{layer}^{tpc}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};N_{clust}^{mvtx}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};N_{clust}^{intt}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};N_{clust}^{tpc}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};N_{clust}^{tot}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};N_{clust}^{same}",
    ";#DeltaR = #sqrt{(#Delta#varphi)^{2} + (#Delta#eta)^{2})};N_{clust}^{same} / N_{clust}^{tot}"
  };

  // leaves to draw vs. delta-r & histogram definitions
  //  <0> = leaf to draw
  //  <1> = histogram name
  //  <2> = axis titles
  //  <3> = y-axis binning
  const vector<tuple<string, string, string, tuple<size_t, double, double>>> vecHistDef = {
    make_tuple("pt_a",          "hPtVsDr",         vecTitles[Hist::HPt],     vecBins[Var::VEne]),
    make_tuple("ptFrac",        "hPtFracVsDr",     vecTitles[Hist::HPFrac],  vecBins[Var::VFrac]),
    make_tuple("eta_a",         "hEtaVsDr",        vecTitles[Hist::HEta],    vecBins[Var::VEta]),
    make_tuple("phi_a",         "hPhiVsDr",        vecTitles[Hist::HPhi],    vecBins[Var::VPhi]),
    make_tuple("ene_a",         "hEneVsDr",        vecTitles[Hist::HEne],    vecBins[Var::VEne]),
    make_tuple("dcaxy_a",       "hDcaXYVsDr",      vecTitles[Hist::HDcaXY],  vecBins[Var::VDca]),
    make_tuple("dcaz_a",        "hDcaZVsDr",       vecTitles[Hist::HDcaZ],   vecBins[Var::VDca]),
    make_tuple("vtxx_a",        "hVtxXVsDr",       vecTitles[Hist::HVtxX],   vecBins[Var::VVtx]),
    make_tuple("vtxy_a",        "hVtxYVsDr",       vecTitles[Hist::HVtxY],   vecBins[Var::VVtx]),
    make_tuple("vtxz_a",        "hVtxZVsDr",       vecTitles[Hist::HVtxZ],   vecBins[Var::VVtx]),
    make_tuple("quality_a",     "hQualityVsDr",    vecTitles[Hist::HQual],   vecBins[Var::VQual]),
    make_tuple("deltapt_a",     "hDeltaPtVsDr",    vecTitles[Hist::HDPt],    vecBins[Var::VDPt]),
    make_tuple("nmvtxlayers_a", "hNMvtxLayerVsDr", vecTitles[Hist::HNLMvtx], vecBins[Var::VNum]),
    make_tuple("ninttlayers_a", "hNInttLayerVsDr", vecTitles[Hist::HNLIntt], vecBins[Var::VNum]),
    make_tuple("ntpclayers_a",  "hNTpcLayerVsDr",  vecTitles[Hist::HNLTpc],  vecBins[Var::VNum]),
    make_tuple("nmvtxclusts_a", "hNMvtxClustVsDr", vecTitles[Hist::HNCMvtx], vecBins[Var::VNum]),
    make_tuple("ninttclusts_a", "hNInttClustVsDr", vecTitles[Hist::HNCIntt], vecBins[Var::VNum]),
    make_tuple("ntpcclusts_a",  "hNTpcClustVsDr",  vecTitles[Hist::HNCTpc],  vecBins[Var::VNum]),
    make_tuple("nclustkey_a",   "hNClustKeyVsDr",  vecTitles[Hist::HNKey],   vecBins[Var::VNum]),
    make_tuple("nsameclustkey", "hNSameKeyVsDr",   vecTitles[Hist::HNSame],  vecBins[Var::VNum]),
    make_tuple("nSameFrac",     "hNSameFracVsDr",  vecTitles[Hist::HNFrac],  vecBins[Var::VFrac])
  };
  cout << "    Defined output histograms." << endl;

  // create output histograms & TNtuple arguments
  //   <0> = what to draw on y-axis
  //   <1> = what to draw on x-axis
  //   <2> = pointer to histogram
  //vector<tuple<string, TH2D*>> vecArgAndHist;
  vector<tuple<string, string, TH2Def>> vecArgAndHist;
  for (const auto histDef : vecHistDef) {

    // make histogram
    auto   bins = get<3>(histDef);
    TH2Def hist = TH2Def(get<1>(histDef).data(), get<2>(histDef).data(), nDrBins, drBinEdges.data(), get<0>(bins), get<1>(bins), get<2>(bins));

    // add argument and hist to vector
    vecArgAndHist.push_back(make_tuple(get<0>(histDef), sDeltaR, hist));
  }
  cout << "    Created output histograms and TNtuple arguments." << endl;

  // helper lambdas -----------------------------------------------------------

  auto getFrac = [](const float a, const float b) { 
    return a / b;
  };

  // open files and make frame ------------------------------------------------

  // open output file
  TFile* fOutput = new TFile(sOutput.data(), "recreate");
  if (!fOutput) {
    cerr << "PANIC: couldn't open a file!\n"
         << "       fOutput = " << fOutput <<  "\n"
         << endl;
    return;
  }
  cout << "    Opened output file." << endl;

  // create data frame
  RDataFrame frame(sInTuple.data(), sInput.data());

  // make sure file isn't empty
  auto pairs = frame.Count();
  if (pairs == 0) {
    cerr << "Error: No track pairs found!" << endl;
    return;
  }
  cout << "    Set up data frame." << endl;

  // run any analysis and make histograms -------------------------------------

  // define additional columns
  auto analysis = frame.Define("ptFrac",    getFrac, {"pt_a", "pt_b"})
                       .Define("nSameFrac", getFrac, {"nsameclustkey", "nclustkey_a"});
  cout << "    Defined additional columns.\n"
       << "    Beginning draw loop..."
       << endl;

  // draw histograms & save to output
  for (auto argAndHist : vecArgAndHist) {

    // draw histogram
    auto hist = analysis.Histo2D(get<2>(argAndHist), get<1>(argAndHist), get<0>(argAndHist));
    cout << "      Drawing '" << get<0>(argAndHist) << "'..." << endl;

    // save to output file
    fOutput -> cd();
    hist    -> Write();
  }
  cout << "    Finished drawing and saved all histograms." << endl;

  // close output and exit ----------------------------------------------------

  // close files
  fOutput -> cd();
  fOutput -> Close();
  cout << "    Closed files." << endl;

  // announce end and exit
  cout << "  Finished weird track tuple plotting!\n" << endl;
  return;

}

// end ------------------------------------------------------------------------

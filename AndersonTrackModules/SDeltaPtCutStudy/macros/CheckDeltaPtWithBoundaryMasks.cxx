// ----------------------------------------------------------------------------
// 'CheckDeltaPtWithBoundaryMasks.cxx
// Derek Anderson
// 10.30.2023
//
// Use to quickly check what the delta-pt/pt distribution
// looks like if we mask the TPC sector boundaries.
// ----------------------------------------------------------------------------

// standard c libraries
#include <array>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
// ROOT libraries
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TFile.h>
#include <TError.h>
#include <TNtuple.h>
#include <TDirectory.h>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDF/HistoModels.hxx>

// make common namespaces implicit
using namespace std;
using namespace ROOT;

// set up RDF aliases
using TH1Mod = ROOT::RDF::TH1DModel;
using TH2Mod = ROOT::RDF::TH2DModel;
using RH1Ptr = ROOT::RDF::RResultPtr<::TH1D>;
using RH2Ptr = ROOT::RDF::RResultPtr<::TH2D>;

// set up tuple aliases
using THBins = tuple<size_t, double, double>;
using TH1Def = tuple<string, string, THBins>;
using TH2Def = tuple<string, string, THBins, THBins>;
using Leaves = pair<string, string>;

// global constants
static const size_t NSectors = 12;



// check delta-pt/pt after masking sector boundaries --------------------------

void CheckDeltaPtWithBoundaryMasks() {

  // lower verbosity
  gErrorIgnoreLevel = kError;
  cout << "\n  Beginning sector boundary-masking check..." << endl;

  // options ------------------------------------------------------------------

  // i/o parameters
  const string sOutput  = "test.root";
  const string sInput   = "../TruthMatching/input/merged/sPhenixG4_oneMatchPerParticle_oldEvaluator.pt020num5evt500pim.d19m10y2023.root";
  const string sInTuple = "ntp_track";

  // mask parameters
  const double maskSize = 0.02;

  // fit guesses
  const array<float, NSectors> arrMeanGuess = {
    -2.877,
    -2.354,
    -1.831,
    -1.308,
    -0.785,
    -0.262,
    0.262,
    0.785,
    1.308,
    1.831,
    2.354,
    2.877
  };
  const float sigmaGuess = maskSize;
  const float ampGuess   = 10.;
  const float fitRange   = 2. * maskSize;

  // histogram definitions ----------------------------------------------------

  // binning accessor
  enum Var {VEne, VPhi, VDPt, VFrac};

  // other variable binning
  //   <0> = no. of bins
  //   <1> = 1st bin lower edge
  //   <2> = last bin upper edge
  const vector<THBins> vecBins = {
    make_tuple(200,    0.,   100.),
    make_tuple(360,   -3.15, 3.15),
    make_tuple(5000,   0.,   5.),
    make_tuple(5000,   0.,   5.)
  };

  // histogram accessors
  enum Cut  {Before, Left, Cut};
  enum Hist {HPtReco, HPtTrue, HPtFrac, HPhi, HDPt, HDPtVsPhi};

  // before vs. after labels
  const vector<string> vecCutLabels = {
    "_beforeMask",
    "_afterMask_leftIn",
    "_afterMask_cutOut"
  };

  // axis titles
  const vector<string> vecAxisTitles = {
    ";p_{T}^{reco} [GeV/c];counts",
    ";p_{T}^{true} [GeV/c];counts",
    ";p_{T}^{reco}/p_{T}^{true};counts",
    ";#varphi^{trk};counts",
    ";#deltap_{T}^{reco}/p_{T}^{reco}",
    ";#varphi^{trk};#deltap_{T}^{reco}/p_{T}^{reco}" 
  };

  // leaves to draw
  const vector<Leaves> vecLeaves = {
    make_pair("pt",     ""),
    make_pair("gpt",    ""),
    make_pair("ptFrac", ""),
    make_pair("phi",    ""),
    make_pair("ptErr",  ""),
    make_pair("phi",    "ptErr")
  };

  // 1d histogram definitions
  //   first = leaves to draw
  //   second = hist definition
  //     <0> = histogram name
  //     <1> = axis titles
  //     <2> = x-axis binning
  const vector<pair<Leaves, TH1Def>> vecHistDef1D = {
    make_pair(vecLeaves[Hist::HPtReco], make_tuple("hPtReco",  vecAxisTitles[Hist::HPtReco].data(), vecBins[Var::VEne])),
    make_pair(vecLeaves[Hist::HPtTrue], make_tuple("hPtTrue",  vecAxisTitles[Hist::HPtTrue].data(), vecBins[Var::VEne])),
    make_pair(vecLeaves[Hist::HPtFrac], make_tuple("hPtFrac",  vecAxisTitles[Hist::HPtFrac].data(), vecBins[Var::VFrac])),
    make_pair(vecLeaves[Hist::HPhi],    make_tuple("hPhi",     vecAxisTitles[Hist::HPhi].data(),    vecBins[Var::VPhi])),
    make_pair(vecLeaves[Hist::HDPt],    make_tuple("hDeltaPt", vecAxisTitles[Hist::HDPt].data(),    vecBins[Var::VDPt]))
  };

  // 2d histogram definitions
  //   first  = leaves to draw
  //   second = hist definition
  //     <0> = histogram name
  //     <1> = axis titles
  //     <2> = x-axis binning
  //     <3> = y-axis binning
  const vector<pair<Leaves, TH2Def>> vecHistDef2D = {
    make_pair(vecLeaves[Hist::HDPtVsPhi], make_tuple("hDPtVsPhi", vecAxisTitles[Hist::HDPtVsPhi].data(), vecBins[Var::VPhi], vecBins[Var::VDPt]))
  };
  cout << "    Defined histograms." << endl;

  // create histograms
  const size_t nCuts = vecCutLabels.size();

  // for storing vectors
  vector<pair<Leaves, TH1Mod>>         vecArgAndHistRow1D;
  vector<pair<Leaves, TH2Mod>>         vecArgAndHistRow2D;
  vector<vector<pair<Leaves, TH1Mod>>> vecArgAndHist1D(nCuts);
  vector<vector<pair<Leaves, TH2Mod>>> vecArgAndHist2D(nCuts);

  // instantiate histograms & load into vectors
  for (const string cut : vecCutLabels) {

    // make 1d hists
    vecArgAndHistRow1D.clear();
    for (const auto histDef1D : vecHistDef1D) {

      // make name
      string name = get<0>(histDef1D.second);
      name += cut;

      // instantiate hist
      THBins bins = get<2>(histDef1D.second);
      TH1Mod hist = TH1Mod(name.data(), get<1>(histDef1D.second).data(), get<0>(bins), get<1>(bins), get <2>(bins));
      vecArgAndHistRow1D.push_back(make_pair(histDef1D.first, hist));
    }
    vecArgAndHist1D.push_back(vecArgAndHistRow1D);

    // make 2d hists
    vecArgAndHistRow2D.clear();
    for (const auto histDef2D : vecHistDef2D) {

      // make name
      string name = get<0>(histDef2D.second);
      name += cut;

      // instantiate hist
      THBins xBins = get<2>(histDef2D.second);
      THBins yBins = get<3>(histDef2D.second);
      TH2Mod hist  = TH2Mod(name.data(), get<1>(histDef2D.second).data(), get<0>(xBins), get<1>(xBins), get <2>(xBins), get<0>(yBins), get<1>(yBins), get<2>(yBins));
      vecArgAndHistRow2D.push_back(make_pair(histDef2D.first, hist));
    }
    vecArgAndHist2D.push_back(vecArgAndHistRow2D);
  }  // end cut loop
  cout << "    Created histograms." << endl;

  // open files and make frames -----------------------------------------------

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
  auto tracks = frame.Count();
  if (tracks == 0) {
    cerr << "Error: No tracks found!" << endl;
    return;
  }
  cout << "    Set up data frame." << endl;

  // analysis lambdas ---------------------------------------------------------

  auto getFrac = [](const float a, const float b) { 
    return a / b;
  };  // end 'getFrac(float, float)'

  auto isInMask = [](const float phi, const double mask, const auto& sectors) {
    bool inMask = false;
    for (const float sector : sectors) {
      if ((phi > (sector - (mask / 2.))) && (phi < (sector + (mask / 2.)))) {
        inMask = true;
        break;
      }
    }
    return inMask;
  };  // end 'isInMask(float)'

  // run analyses -------------------------------------------------------------

  // get histograms before masking
  auto before = frame.Define("ptFrac", getFrac, {"pt", "gpt"})
                     .Define("ptErr",  getFrac, {"deltapt", "pt"});

  // get initial 1d histograms
  vector<vector<RH1Ptr>> vecHistResult1D(nCuts);
  vector<vector<RH2Ptr>> vecHistResult2D(nCuts);
  for (const auto argAndHistBefore1D : vecArgAndHist1D[Cut::Before]) {
    auto hist1D = before.Histo1D(argAndHistBefore1D.second, argAndHistBefore1D.first.first.data());
    vecHistResult1D[Cut::Before].push_back(hist1D);
  }
  for (const auto argAndHistBefore2D : vecArgAndHist2D[Cut::Before]) {
    auto hist2D = before.Histo2D(argAndHistBefore2D.second, argAndHistBefore2D.first.first.data(), argAndHistBefore2D.first.second.data());
    vecHistResult2D[Cut::Before].push_back(hist2D);
  }

  // retrieve results
  vector<vector<TH1D*>> vecOutHist1D(nCuts);
  vector<vector<TH2D*>> vecOutHist2D(nCuts);
  for (auto histResultBefore1D : vecHistResult1D[Cut::Before]) {
    TH1D* hist1D = (TH1D*) histResultBefore1D -> Clone();
    vecOutHist1D[Cut::Before].push_back(hist1D);
  }
  for (auto histResultBefore2D : vecHistResult2D[Cut::Before]) {
    TH2D* hist2D = (TH2D*) histResultBefore2D -> Clone();
    vecOutHist2D[Cut::Before].push_back(hist2D);
  }

  /* TODO
   *   - get sector bourndaries with fits
   */

  // for masking sector boundaries
  array<float, NSectors> arrSectors;
  arrSectors.fill(0.);
  
  // save & close -------------------------------------------------------------

  // save histograms
  fOutput -> cd();
  for (auto histRow1D : vecOutHist1D) {
    for (auto hist1D : histRow1D) {
      hist1D -> Write();
    }
  }
  for (auto histRow2D : vecOutHist2D) {
    for (auto hist2D : histRow2D) {
      hist2D -> Write();
    }
  }
  cout << "    Saved histograms." << endl;

  // close files
  fOutput -> cd();
  fOutput -> Close();
  cout << "  Finished sector boundary-masking check!\n" << endl;

}

// end ------------------------------------------------------------------------

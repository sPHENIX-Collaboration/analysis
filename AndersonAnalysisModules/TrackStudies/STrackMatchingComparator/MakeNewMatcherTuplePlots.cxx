// ----------------------------------------------------------------------------
// 'MakeNewMatcherTuplePlots.cxx'
// Derek Anderson
// 01.24.2024
//
// Short macro to make plots from the
// output of the modified ClusMatchTree
// module.
// ----------------------------------------------------------------------------

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TError.h"

using namespace std;



void MakeNewMatcherTuplePlots() {

  // io parameters
  const string sOutput("newMatcherTuplePlots_oneMatchPerParticle_oddFrac02120.pt10n1evt500pim.d24m1y2024.root");
  const string sInTrue("input/merged/sPhenixG4_oneMatchPerParticle_newMatcher.pt10num1evt500pim.d4m1y2024.root");
  const string sInReco("input/merged/sPhenixG4_oneMatchPerParticle_newMatcher.pt10num1evt500pim.d4m1y2024.root");
  const string sTreeTrue("ntForEvalComp");
  const string sTreeReco("ntForEvalComp");

  // weird track parameters
  const pair<float, float> oddPtFrac = {0.2, 1.2};

  // lower verbosity
  gErrorIgnoreLevel = kError;
  cout << "\n  Staring new matcher plot script..." << endl;

  // open files
  TFile* fOutput = new TFile(sOutput.data(), "recreate");
  TFile* fInTrue = new TFile(sInTrue.data(), "read");
  TFile* fInReco = new TFile(sInReco.data(), "read");
  if (!fOutput || !fInTrue || !fInReco) {
     cerr << "PANIC: couldn't open a file!\n"
          << "       fOutput = " << fOutput << "fInTrue = " << fInTrue << ", fInReco = " << fInReco << "\n"
          << endl;
    return;
  }
  cout << "    Opened files." << endl;

  // grab input tree
  TTree* tInTrue = (TTree*) fInTrue -> Get(sTreeTrue.data());
  TTree* tInReco = (TTree*) fInReco -> Get(sTreeReco.data());
  if (!tInTrue || !tInReco) {
    cerr << "PANIC: couldn't grab input tree!\n"
         << "       sTreeTrue = " << sTreeTrue << ", tInTrue = " << tInTrue << "\n"
         << "       sTreeReco = " << sTreeReco << ", tInReco = " << tInReco << "\n"
         << endl;
    return;
  }
  cout << "    Grabbed input trees." << endl;

  // declare input leaves
  Float_t tru_evtid;
  Float_t tru_trkid;
  Float_t tru_pt;
  Float_t tru_eta;
  Float_t tru_phi;
  Float_t tru_nmvtxclust_trkmatcher;
  Float_t tru_ninttclust_trkmatcher;
  Float_t tru_ntpclust_trkmatcher;
  Float_t tru_nmvtxclust_manual;
  Float_t tru_ninttclust_manual;
  Float_t tru_ntpcclust_manual;
  Float_t tru_nmvtxlayer_;
  Float_t tru_ninttlayer;
  Float_t tru_ntpclayer;
  Float_t tru_deltapt;
  Float_t tru_quality;
  Float_t tru_dcaxy;
  Float_t tru_dcaz;
  Float_t tru_sigmadcaxy;
  Float_t tru_sigmadacz;
  Float_t tru_vx;
  Float_t tru_vy;
  Float_t tru_vz;
  Float_t tru_gtrkid;
  Float_t tru_gpt;
  Float_t tru_geta;
  Float_t tru_gphi;
  Float_t tru_gnmvtxclust_trkmatcher;
  Float_t tru_gninttclust_trkmatcher;
  Float_t tru_gntpclust_trkmatchergnmvtxclust_manual;
  Float_t tru_gninttclust_manual;
  Float_t tru_gntpcclust_manual;
  Float_t tru_gnmvtxlayer;
  Float_t tru_gninttlayer;
  Float_t tru_gntpclayer;
  Float_t tru_gdeltapt;
  Float_t tru_gquality;
  Float_t tru_gdcaxy;
  Float_t tru_gdcaz;
  Float_t tru_gsigmadcaxy;
  Float_t tru_gsigmadacz;
  Float_t tru_gvx;
  Float_t tru_gvy;
  Float_t tru_gvz;
  Float_t tru_fracnmvtxmatched;
  Float_t tru_fracninttmatched;
  Float_t tru_fracntpcmatched;

  Float_t rec_evtid;
  Float_t rec_trkid;
  Float_t rec_pt;
  Float_t rec_eta;
  Float_t rec_phi;
  Float_t rec_nmvtxclust_trkmatcher;
  Float_t rec_ninttclust_trkmatcher;
  Float_t rec_ntpclust_trkmatcher;
  Float_t rec_nmvtxclust_manual;
  Float_t rec_ninttclust_manual;
  Float_t rec_ntpcclust_manual;
  Float_t rec_nmvtxlayer_;
  Float_t rec_ninttlayer;
  Float_t rec_ntpclayer;
  Float_t rec_deltapt;
  Float_t rec_quality;
  Float_t rec_dcaxy;
  Float_t rec_dcaz;
  Float_t rec_sigmadcaxy;
  Float_t rec_sigmadacz;
  Float_t rec_vx;
  Float_t rec_vy;
  Float_t rec_vz;
  Float_t rec_gtrkid;
  Float_t rec_gpt;
  Float_t rec_geta;
  Float_t rec_gphi;
  Float_t rec_gnmvtxclust_trkmatcher;
  Float_t rec_gninttclust_trkmatcher;
  Float_t rec_gntpclust_trkmatchergnmvtxclust_manual;
  Float_t rec_gninttclust_manual;
  Float_t rec_gntpcclust_manual;
  Float_t rec_gnmvtxlayer;
  Float_t rec_gninttlayer;
  Float_t rec_gntpclayer;
  Float_t rec_gdeltapt;
  Float_t rec_gquality;
  Float_t rec_gdcaxy;
  Float_t rec_gdcaz;
  Float_t rec_gsigmadcaxy;
  Float_t rec_gsigmadacz;
  Float_t rec_gvx;
  Float_t rec_gvy;
  Float_t rec_gvz;
  Float_t rec_fracnmvtxmatched;
  Float_t rec_fracninttmatched;
  Float_t rec_fracntpcmatched;
 
  // set branch addresses
  tInTrue -> SetBranchAddress("evtid",                                  &tru_evtid);
  tInTrue -> SetBranchAddress("trkid",                                  &tru_trkid);
  tInTrue -> SetBranchAddress("pt",                                     &tru_pt);
  tInTrue -> SetBranchAddress("eta",                                    &tru_eta);
  tInTrue -> SetBranchAddress("phi",                                    &tru_phi);
  tInTrue -> SetBranchAddress("nmvtxclust_trkmatcher",                  &tru_nmvtxclust_trkmatcher);
  tInTrue -> SetBranchAddress("ninttclust_trkmatcher",                  &tru_ninttclust_trkmatcher);
  tInTrue -> SetBranchAddress("ntpclust_trkmatcher",                    &tru_ntpclust_trkmatcher);
  tInTrue -> SetBranchAddress("nmvtxclust_manual",                      &tru_nmvtxclust_manual);
  tInTrue -> SetBranchAddress("ninttclust_manual",                      &tru_ninttclust_manual);
  tInTrue -> SetBranchAddress("ntpcclust_manual",                       &tru_ntpcclust_manual);
  tInTrue -> SetBranchAddress("nmvtxlayer_",                            &tru_nmvtxlayer_);
  tInTrue -> SetBranchAddress("ninttlayer",                             &tru_ninttlayer);
  tInTrue -> SetBranchAddress("ntpclayer",                              &tru_ntpclayer);
  tInTrue -> SetBranchAddress("deltapt",                                &tru_deltapt);
  tInTrue -> SetBranchAddress("quality",                                &tru_quality);
  tInTrue -> SetBranchAddress("dcaxy",                                  &tru_dcaxy);
  tInTrue -> SetBranchAddress("dcaz",                                   &tru_dcaz);
  tInTrue -> SetBranchAddress("sigmadcaxy",                             &tru_sigmadcaxy);
  tInTrue -> SetBranchAddress("sigmadacz",                              &tru_sigmadacz);
  tInTrue -> SetBranchAddress("vx",                                     &tru_vx);
  tInTrue -> SetBranchAddress("vy",                                     &tru_vy);
  tInTrue -> SetBranchAddress("vz",                                     &tru_vz);
  tInTrue -> SetBranchAddress("gtrkid",                                 &tru_gtrkid);
  tInTrue -> SetBranchAddress("gpt",                                    &tru_gpt);
  tInTrue -> SetBranchAddress("geta",                                   &tru_geta);
  tInTrue -> SetBranchAddress("gphi",                                   &tru_gphi);
  tInTrue -> SetBranchAddress("gnmvtxclust_trkmatcher",                 &tru_gnmvtxclust_trkmatcher);
  tInTrue -> SetBranchAddress("gninttclust_trkmatcher",                 &tru_gninttclust_trkmatcher);
  tInTrue -> SetBranchAddress("gntpclust_trkmatchergnmvtxclust_manual", &tru_gntpclust_trkmatchergnmvtxclust_manual);
  tInTrue -> SetBranchAddress("gninttclust_manual",                     &tru_gninttclust_manual);
  tInTrue -> SetBranchAddress("gntpcclust_manual",                      &tru_gntpcclust_manual);
  tInTrue -> SetBranchAddress("gnmvtxlayer",                            &tru_gnmvtxlayer);
  tInTrue -> SetBranchAddress("gninttlayer",                            &tru_gninttlayer);
  tInTrue -> SetBranchAddress("gntpclayer",                             &tru_gntpclayer);
  tInTrue -> SetBranchAddress("gdeltapt",                               &tru_gdeltapt);
  tInTrue -> SetBranchAddress("gquality",                               &tru_gquality);
  tInTrue -> SetBranchAddress("gdcaxy",                                 &tru_gdcaxy);
  tInTrue -> SetBranchAddress("gdcaz",                                  &tru_gdcaz);
  tInTrue -> SetBranchAddress("gsigmadcaxy",                            &tru_gsigmadcaxy);
  tInTrue -> SetBranchAddress("gsigmadacz",                             &tru_gsigmadacz);
  tInTrue -> SetBranchAddress("gvx",                                    &tru_gvx);
  tInTrue -> SetBranchAddress("gvy",                                    &tru_gvy);
  tInTrue -> SetBranchAddress("gvz",                                    &tru_gvz);
  tInTrue -> SetBranchAddress("fracnmvtxmatched",                       &tru_fracnmvtxmatched);
  tInTrue -> SetBranchAddress("fracninttmatched",                       &tru_fracninttmatched);
  tInTrue -> SetBranchAddress("fracntpcmatched",                        &tru_fracntpcmatched);

  tInReco -> SetBranchAddress("evtid",                                  &rec_evtid);
  tInReco -> SetBranchAddress("trkid",                                  &rec_trkid);
  tInReco -> SetBranchAddress("pt",                                     &rec_pt);
  tInReco -> SetBranchAddress("eta",                                    &rec_eta);
  tInReco -> SetBranchAddress("phi",                                    &rec_phi);
  tInReco -> SetBranchAddress("nmvtxclust_trkmatcher",                  &rec_nmvtxclust_trkmatcher);
  tInReco -> SetBranchAddress("ninttclust_trkmatcher",                  &rec_ninttclust_trkmatcher);
  tInReco -> SetBranchAddress("ntpclust_trkmatcher",                    &rec_ntpclust_trkmatcher);
  tInReco -> SetBranchAddress("nmvtxclust_manual",                      &rec_nmvtxclust_manual);
  tInReco -> SetBranchAddress("ninttclust_manual",                      &rec_ninttclust_manual);
  tInReco -> SetBranchAddress("ntpcclust_manual",                       &rec_ntpcclust_manual);
  tInReco -> SetBranchAddress("nmvtxlayer_",                            &rec_nmvtxlayer_);
  tInReco -> SetBranchAddress("ninttlayer",                             &rec_ninttlayer);
  tInReco -> SetBranchAddress("ntpclayer",                              &rec_ntpclayer);
  tInReco -> SetBranchAddress("deltapt",                                &rec_deltapt);
  tInReco -> SetBranchAddress("quality",                                &rec_quality);
  tInReco -> SetBranchAddress("dcaxy",                                  &rec_dcaxy);
  tInReco -> SetBranchAddress("dcaz",                                   &rec_dcaz);
  tInReco -> SetBranchAddress("sigmadcaxy",                             &rec_sigmadcaxy);
  tInReco -> SetBranchAddress("sigmadacz",                              &rec_sigmadacz);
  tInReco -> SetBranchAddress("vx",                                     &rec_vx);
  tInReco -> SetBranchAddress("vy",                                     &rec_vy);
  tInReco -> SetBranchAddress("vz",                                     &rec_vz);
  tInReco -> SetBranchAddress("gtrkid",                                 &rec_gtrkid);
  tInReco -> SetBranchAddress("gpt",                                    &rec_gpt);
  tInReco -> SetBranchAddress("geta",                                   &rec_geta);
  tInReco -> SetBranchAddress("gphi",                                   &rec_gphi);
  tInReco -> SetBranchAddress("gnmvtxclust_trkmatcher",                 &rec_gnmvtxclust_trkmatcher);
  tInReco -> SetBranchAddress("gninttclust_trkmatcher",                 &rec_gninttclust_trkmatcher);
  tInReco -> SetBranchAddress("gntpclust_trkmatchergnmvtxclust_manual", &rec_gntpclust_trkmatchergnmvtxclust_manual);
  tInReco -> SetBranchAddress("gninttclust_manual",                     &rec_gninttclust_manual);
  tInReco -> SetBranchAddress("gntpcclust_manual",                      &rec_gntpcclust_manual);
  tInReco -> SetBranchAddress("gnmvtxlayer",                            &rec_gnmvtxlayer);
  tInReco -> SetBranchAddress("gninttlayer",                            &rec_gninttlayer);
  tInReco -> SetBranchAddress("gntpclayer",                             &rec_gntpclayer);
  tInReco -> SetBranchAddress("gdeltapt",                               &rec_gdeltapt);
  tInReco -> SetBranchAddress("gquality",                               &rec_gquality);
  tInReco -> SetBranchAddress("gdcaxy",                                 &rec_gdcaxy);
  tInReco -> SetBranchAddress("gdcaz",                                  &rec_gdcaz);
  tInReco -> SetBranchAddress("gsigmadcaxy",                            &rec_gsigmadcaxy);
  tInReco -> SetBranchAddress("gsigmadacz",                             &rec_gsigmadacz);
  tInReco -> SetBranchAddress("gvx",                                    &rec_gvx);
  tInReco -> SetBranchAddress("gvy",                                    &rec_gvy);
  tInReco -> SetBranchAddress("gvz",                                    &rec_gvz);
  tInReco -> SetBranchAddress("fracnmvtxmatched",                       &rec_fracnmvtxmatched);
  tInReco -> SetBranchAddress("fracninttmatched",                       &rec_fracninttmatched);
  tInReco -> SetBranchAddress("fracntpcmatched",                        &rec_fracntpcmatched);
  cout << "    Set input branches." << endl;

  // histogram indices
  enum Var {
    NTot,
    NIntt,
    NMvtx,
    NTpc,
    RTot,
    RIntt,
    RMvtx,
    RTpc,
    Phi,
    Eta,
    Pt,
    Frac
  };
  enum Type {
    Truth,
    Track,
    Weird,
    Normal
  };
  enum Comp {
    VsTruthPt,
    VsNumTpc
  };

  // output histogram base names
  const vector<vector<string>> vecHistBase = {
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
    {"hTruthFrac",    "hTrackFrac",    "hWeirdFrac",    "hNormFrac"}
  };
  const size_t nHistRows  = vecHistBase.size();
  const size_t nHistTypes = vecHistBase[0].size();

  // 2D histogram name modifiers
  const vector<string> vecVsHistModifiers = {
    "VsTruthPt",
    "VsNumTpc"
  };
  const size_t nVsHistMods = vecVsHistModifiers.size();

  // axis titles
  const string         sCount("counts");
  const vector<string> vecBaseAxisVars = {
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
    "p_{T}^{reco} / p_{T}^{true}"
  };
  const vector<string> vecVsAxisVars = {
    "p_{T}^{true} [GeV/c]",
    "N_{clust}^{tpc}"
  };

  // output histogram no. of bins
  const uint32_t nNumBins  = 101;
  const uint32_t nRatBins  = 120;
  const uint32_t nEtaBins  = 80;
  const uint32_t nPhiBins  = 360;
  const uint32_t nPtBins   = 202;
  const uint32_t nFracBins = 220;

  // output histogram bin ranges
  const pair<float, float> xNumBins  = {-0.5,  100.5};
  const pair<float, float> xRatBins  = {-0.5,  5.5};
  const pair<float, float> xEtaBins  = {-2.,   2.};
  const pair<float, float> xPhiBins  = {-3.15, 3.15};
  const pair<float, float> xPtBins   = {-1.,   100.};
  const pair<float, float> xFracBins = {-0.5,  10.5};

  // output histogram base binning definitions
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
    make_tuple(nFracBins, xFracBins)
  };

  // output 2D histogram x-axis binning
  vector<tuple<uint32_t, pair<float, float>>> vecVsHistBins = {
    make_tuple(nPtBins,  xPtBins),
    make_tuple(nNumBins, xNumBins)
  };

  // make 1D histograms
  vector<vector<TH1D*>> vecHist1D(nHistRows);
  for (size_t iHistRow = 0; iHistRow < nHistRows; iHistRow++) {
    for (const string sHistBase : vecHistBase[iHistRow]) {
      vecHist1D[iHistRow].push_back(
        new TH1D(
          sHistBase.data(),
          "",
          get<0>(vecBaseHistBins[iHistRow]),
          get<1>(vecBaseHistBins[iHistRow]).first,
          get<1>(vecBaseHistBins[iHistRow]).second
        )
      );
    }  // end type loop
  }  // end row loop

  // make 2D histograms
  vector<vector<vector<TH2D*>>> vecHist2D(nHistRows, vector<vector<TH2D*>>(nHistTypes));
  for (size_t iHistRow = 0; iHistRow < nHistRows; iHistRow++) {
    for (size_t iHistType = 0; iHistType < nHistTypes; iHistType++) {
      for (size_t iVsHistMod = 0; iVsHistMod < nVsHistMods; iVsHistMod++) {
        const string sHistName2D = vecHistBase[iHistRow][iHistType] + vecVsHistModifiers[iVsHistMod];
        vecHist2D[iHistRow][iHistType].push_back(
          new TH2D(
            sHistName2D.data(),
            "",
            get<0>(vecVsHistBins[iVsHistMod]),
            get<1>(vecVsHistBins[iVsHistMod]).first,
            get<1>(vecVsHistBins[iVsHistMod]).second,
            get<0>(vecBaseHistBins[iHistRow]),
            get<1>(vecBaseHistBins[iHistRow]).first,
            get<1>(vecBaseHistBins[iHistRow]).second
          )
        );
      }  // end modifier loop
    }  // end type loop
  }  // end row loop

  // set errors
  for (auto histRow1D : vecHist1D) {
    for (auto hist1D : histRow1D) {
      hist1D -> Sumw2();
    }
  }

  for (auto histRow2D : vecHist2D) {
    for (auto histType2D : histRow2D) {
      for (auto hist2D: histType2D) {
        hist2D -> Sumw2();
      }
    }
  }

  // grab no. of entries
  const int64_t nTrueEntries = tInTrue -> GetEntries();
  const int64_t nRecoEntries = tInReco -> GetEntries(); 
  cout << "    Beginning truth particle loop: " << nTrueEntries << " to process" << endl;

  // loop over truth particles
  int64_t nTrueBytes = 0;
  for (int64_t iTrueEntry = 0; iTrueEntry < nTrueEntries; iTrueEntry++) {

    // grab truth particle entry
    const int64_t trueBytes = tInTrue -> GetEntry(iTrueEntry);
    if (trueBytes < 0) {
      cerr << "PANIC: issue with entry " << iTrueEntry << "! Aborting loop!\n" << endl;
      break;
    } else {
      nTrueBytes += trueBytes;
    }

    const int64_t iTrueProg = iTrueEntry + 1;
    if (iTrueProg == nTrueEntries) {
      cout << "      Processing entry " << iTrueProg << "/" << nTrueEntries << "..." << endl;
    } else {
      cout << "      Processing entry " << iTrueProg << "/" << nTrueEntries << "...\r" << flush;
    }

    // run calculations
    const float tru_gnclust = tru_gnmvtxclust_trkmatcher + tru_gninttclust_trkmatcher + tru_gntpcclust_manual;

    // fill truth 1D histograms
    vecHist1D[Var::NTot][Type::Truth]  -> Fill(tru_gnclust);
    vecHist1D[Var::NIntt][Type::Truth] -> Fill(tru_gninttclust_trkmatcher);
    vecHist1D[Var::NMvtx][Type::Truth] -> Fill(tru_gnmvtxclust_trkmatcher);
    vecHist1D[Var::NTpc][Type::Truth]  -> Fill(tru_gntpcclust_manual);
    vecHist1D[Var::RTot][Type::Truth]  -> Fill(1.);
    vecHist1D[Var::RIntt][Type::Truth] -> Fill(1.);
    vecHist1D[Var::RMvtx][Type::Truth] -> Fill(1.);
    vecHist1D[Var::RTpc][Type::Truth]  -> Fill(1.);
    vecHist1D[Var::Phi][Type::Truth]   -> Fill(tru_gpt);
    vecHist1D[Var::Eta][Type::Truth]   -> Fill(tru_geta);
    vecHist1D[Var::Pt][Type::Truth]    -> Fill(tru_gphi);
    vecHist1D[Var::Frac][Type::Truth]  -> Fill(1.);

    // fill truth 2D histograms
    vecHist2D[Var::NTot][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, tru_gnclust);
    vecHist2D[Var::NIntt][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, tru_gninttclust_trkmatcher);
    vecHist2D[Var::NMvtx][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, tru_gnmvtxclust_trkmatcher);
    vecHist2D[Var::NTpc][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, tru_gntpcclust_manual);
    vecHist2D[Var::RTot][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    vecHist2D[Var::RIntt][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    vecHist2D[Var::RMvtx][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    vecHist2D[Var::RTpc][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    vecHist2D[Var::Phi][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, tru_gphi);
    vecHist2D[Var::Eta][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, tru_geta);
    vecHist2D[Var::Pt][Type::Truth][Comp::VsTruthPt]    -> Fill(tru_gpt, tru_gpt);
    vecHist2D[Var::Frac][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);

    vecHist2D[Var::NTot][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, tru_gnclust);
    vecHist2D[Var::NIntt][Type::Truth][Comp::VsNumTpc] -> Fill(tru_ntpclust_trkmatcher, tru_gninttclust_trkmatcher);
    vecHist2D[Var::NMvtx][Type::Truth][Comp::VsNumTpc] -> Fill(tru_ntpclust_trkmatcher, tru_gnmvtxclust_trkmatcher);
    vecHist2D[Var::NTpc][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, tru_gntpcclust_manual);
    vecHist2D[Var::RTot][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, 1.);
    vecHist2D[Var::RIntt][Type::Truth][Comp::VsNumTpc] -> Fill(tru_ntpclust_trkmatcher, 1.);
    vecHist2D[Var::RMvtx][Type::Truth][Comp::VsNumTpc] -> Fill(tru_ntpclust_trkmatcher, 1.);
    vecHist2D[Var::RTpc][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, 1.);
    vecHist2D[Var::Phi][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_ntpclust_trkmatcher, tru_gphi);
    vecHist2D[Var::Eta][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_ntpclust_trkmatcher, tru_geta);
    vecHist2D[Var::Pt][Type::Truth][Comp::VsNumTpc]    -> Fill(tru_ntpclust_trkmatcher, tru_gpt);
    vecHist2D[Var::Frac][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, 1.);
  }  // end truth particle loop

  // announce next entry loop
  cout << "    Finished truth particle loop.\n"
       << "    Beginning reconstructed track loop: " << nRecoEntries << " to process"
       << endl;

  // loop over reco tracks
  int64_t nRecoBytes = 0;
  for (int64_t iRecoEntry = 0; iRecoEntry < nRecoEntries; iRecoEntry++) {

    // grab reco track entry
    const int64_t recoBytes = tInReco -> GetEntry(iRecoEntry);
    if (recoBytes < 0) {
      cerr << "PANIC: issue with entry " << iRecoEntry << "! Aborting loop!\n" << endl;
      break;
    } else {
      nRecoBytes += recoBytes;
    }

    const int64_t iRecoProg = iRecoEntry + 1;
    if (iRecoProg == nRecoEntries) {
      cout << "      Processing entry " << iRecoProg << "/" << nRecoEntries << "..." << endl;
    } else {
      cout << "      Processing entry " << iRecoProg << "/" << nRecoEntries << "...\r" << flush;
    }

    // run calculations
    const double rec_nclus  = rec_ninttclust_trkmatcher + rec_nmvtxclust_trkmatcher + rec_ntpclust_trkmatcher;
    const double rec_gnclus = rec_gninttclust_trkmatcher + rec_gnmvtxclust_trkmatcher + rec_gntpcclust_manual;
    const double rec_rnclus = rec_nclus / rec_gnclus;
    const double rec_rintt  = rec_ninttclust_trkmatcher / rec_gninttclust_trkmatcher;
    const double rec_rmaps  = rec_nmvtxclust_trkmatcher / rec_gnmvtxclust_trkmatcher;
    const double rec_rtpc   = rec_ntpclust_trkmatcher / rec_gntpcclust_manual;
    const double rec_ptfrac = rec_pt / rec_gpt;

    // fill all matched reco 1D histograms
    vecHist1D[Var::NTot][Type::Track]  -> Fill(rec_nclus);
    vecHist1D[Var::NIntt][Type::Track] -> Fill(rec_ninttclust_trkmatcher);
    vecHist1D[Var::NMvtx][Type::Track] -> Fill(rec_nmvtxclust_trkmatcher);
    vecHist1D[Var::NTpc][Type::Track]  -> Fill(rec_ntpclust_trkmatcher);
    vecHist1D[Var::RTot][Type::Track]  -> Fill(rec_rnclus);
    vecHist1D[Var::RIntt][Type::Track] -> Fill(rec_rintt);
    vecHist1D[Var::RMvtx][Type::Track] -> Fill(rec_rmaps);
    vecHist1D[Var::RTpc][Type::Track]  -> Fill(rec_rtpc);
    vecHist1D[Var::Phi][Type::Track]   -> Fill(rec_phi);
    vecHist1D[Var::Eta][Type::Track]   -> Fill(rec_eta);
    vecHist1D[Var::Pt][Type::Track]    -> Fill(rec_pt);
    vecHist1D[Var::Frac][Type::Track]  -> Fill(rec_ptfrac);

    // fill all matched reco 2D histograms
    vecHist2D[Var::NTot][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nclus);
    vecHist2D[Var::NIntt][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_ninttclust_trkmatcher);
    vecHist2D[Var::NMvtx][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_nmvtxclust_trkmatcher);
    vecHist2D[Var::NTpc][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ntpclust_trkmatcher);
    vecHist2D[Var::RTot][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rnclus);
    vecHist2D[Var::RIntt][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rintt);
    vecHist2D[Var::RMvtx][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rmaps);
    vecHist2D[Var::RTpc][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rtpc);
    vecHist2D[Var::Phi][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_phi);
    vecHist2D[Var::Eta][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_eta);
    vecHist2D[Var::Pt][Type::Track][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_pt);
    vecHist2D[Var::Frac][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptfrac);

    vecHist2D[Var::NTot][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_nclus);
    vecHist2D[Var::NIntt][Type::Track][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_ninttclust_trkmatcher);
    vecHist2D[Var::NMvtx][Type::Track][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_nmvtxclust_trkmatcher);
    vecHist2D[Var::NTpc][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ntpclust_trkmatcher);
    vecHist2D[Var::RTot][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rnclus);
    vecHist2D[Var::RIntt][Type::Track][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_rintt);
    vecHist2D[Var::RMvtx][Type::Track][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_rmaps);
    vecHist2D[Var::RTpc][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rtpc);
    vecHist2D[Var::Phi][Type::Track][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_phi);
    vecHist2D[Var::Eta][Type::Track][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_eta);
    vecHist2D[Var::Pt][Type::Track][Comp::VsNumTpc]    -> Fill(rec_ntpclust_trkmatcher, rec_pt);
    vecHist2D[Var::Frac][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ptfrac);

    // fill weird and normal matched reco 1D histograms
    const bool isNormalTrack = ((rec_ptfrac >= oddPtFrac.first) && (rec_ptfrac <= oddPtFrac.second));
    if (isNormalTrack) {
      vecHist1D[Var::NTot][Type::Normal]  -> Fill(rec_nclus);
      vecHist1D[Var::NIntt][Type::Normal] -> Fill(rec_ninttclust_trkmatcher);
      vecHist1D[Var::NMvtx][Type::Normal] -> Fill(rec_nmvtxclust_trkmatcher);
      vecHist1D[Var::NTpc][Type::Normal]  -> Fill(rec_ntpclust_trkmatcher);
      vecHist1D[Var::RTot][Type::Normal]  -> Fill(rec_rnclus);
      vecHist1D[Var::RIntt][Type::Normal] -> Fill(rec_rintt);
      vecHist1D[Var::RMvtx][Type::Normal] -> Fill(rec_rmaps);
      vecHist1D[Var::RTpc][Type::Normal]  -> Fill(rec_rtpc);
      vecHist1D[Var::Phi][Type::Normal]   -> Fill(rec_phi);
      vecHist1D[Var::Eta][Type::Normal]   -> Fill(rec_eta);
      vecHist1D[Var::Pt][Type::Normal]    -> Fill(rec_pt);
      vecHist1D[Var::Frac][Type::Normal]  -> Fill(rec_ptfrac);

      vecHist2D[Var::NTot][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nclus);
      vecHist2D[Var::NIntt][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_ninttclust_trkmatcher);
      vecHist2D[Var::NMvtx][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_nmvtxclust_trkmatcher);
      vecHist2D[Var::NTpc][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ntpclust_trkmatcher);
      vecHist2D[Var::RTot][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rnclus);
      vecHist2D[Var::RIntt][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rintt);
      vecHist2D[Var::RMvtx][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rmaps);
      vecHist2D[Var::RTpc][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rtpc);
      vecHist2D[Var::Phi][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_phi);
      vecHist2D[Var::Eta][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_eta);
      vecHist2D[Var::Pt][Type::Normal][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_pt);
      vecHist2D[Var::Frac][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptfrac);

      vecHist2D[Var::NTot][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_nclus);
      vecHist2D[Var::NIntt][Type::Normal][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_ninttclust_trkmatcher);
      vecHist2D[Var::NMvtx][Type::Normal][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_nmvtxclust_trkmatcher);
      vecHist2D[Var::NTpc][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ntpclust_trkmatcher);
      vecHist2D[Var::RTot][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rnclus);
      vecHist2D[Var::RIntt][Type::Normal][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_rintt);
      vecHist2D[Var::RMvtx][Type::Normal][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_rmaps);
      vecHist2D[Var::RTpc][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rtpc);
      vecHist2D[Var::Phi][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_phi);
      vecHist2D[Var::Eta][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_eta);
      vecHist2D[Var::Pt][Type::Normal][Comp::VsNumTpc]    -> Fill(rec_ntpclust_trkmatcher, rec_pt);
      vecHist2D[Var::Frac][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ptfrac);
    } else {
      vecHist1D[Var::NTot][Type::Weird]  -> Fill(rec_nclus);
      vecHist1D[Var::NIntt][Type::Weird] -> Fill(rec_ninttclust_trkmatcher);
      vecHist1D[Var::NMvtx][Type::Weird] -> Fill(rec_nmvtxclust_trkmatcher);
      vecHist1D[Var::NTpc][Type::Weird]  -> Fill(rec_ntpclust_trkmatcher);
      vecHist1D[Var::RTot][Type::Weird]  -> Fill(rec_rnclus);
      vecHist1D[Var::RIntt][Type::Weird] -> Fill(rec_rintt);
      vecHist1D[Var::RMvtx][Type::Weird] -> Fill(rec_rmaps);
      vecHist1D[Var::RTpc][Type::Weird]  -> Fill(rec_rtpc);
      vecHist1D[Var::Phi][Type::Weird]   -> Fill(rec_phi);
      vecHist1D[Var::Eta][Type::Weird]   -> Fill(rec_eta);
      vecHist1D[Var::Pt][Type::Weird]    -> Fill(rec_pt);
      vecHist1D[Var::Frac][Type::Weird]  -> Fill(rec_ptfrac);

      vecHist2D[Var::NTot][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nclus);
      vecHist2D[Var::NIntt][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_ninttclust_trkmatcher);
      vecHist2D[Var::NMvtx][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_nmvtxclust_trkmatcher);
      vecHist2D[Var::NTpc][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ntpclust_trkmatcher);
      vecHist2D[Var::RTot][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rnclus);
      vecHist2D[Var::RIntt][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rintt);
      vecHist2D[Var::RMvtx][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rmaps);
      vecHist2D[Var::RTpc][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rtpc);
      vecHist2D[Var::Phi][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_phi);
      vecHist2D[Var::Eta][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_eta);
      vecHist2D[Var::Pt][Type::Weird][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_pt);
      vecHist2D[Var::Frac][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptfrac);

      vecHist2D[Var::NTot][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_nclus);
      vecHist2D[Var::NIntt][Type::Weird][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_ninttclust_trkmatcher);
      vecHist2D[Var::NMvtx][Type::Weird][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_nmvtxclust_trkmatcher);
      vecHist2D[Var::NTpc][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ntpclust_trkmatcher);
      vecHist2D[Var::RTot][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rnclus);
      vecHist2D[Var::RIntt][Type::Weird][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_rintt);
      vecHist2D[Var::RMvtx][Type::Weird][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_rmaps);
      vecHist2D[Var::RTpc][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rtpc);
      vecHist2D[Var::Phi][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_phi);
      vecHist2D[Var::Eta][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_eta);
      vecHist2D[Var::Pt][Type::Weird][Comp::VsNumTpc]    -> Fill(rec_ntpclust_trkmatcher, rec_pt);
      vecHist2D[Var::Frac][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ptfrac);
    }
  }  // end reco track loop
  cout << "    Finished reconstructed track loop."  << endl;

  // set axis titles
  size_t iVar = 0;
  for (auto histRow1D : vecHist1D) {
    for (auto hist1D : histRow1D) {
      hist1D -> GetXaxis() -> SetTitle(vecBaseAxisVars.at(iVar).data());
      hist1D -> GetYaxis() -> SetTitle(sCount.data());
    }
    ++iVar;
  }
  iVar = 0;

  size_t iComp = 0;
  for (auto histRow2D : vecHist2D) {
    for (auto histType2D : histRow2D) {
      iComp = 0;
      for (auto hist2D : histType2D) {
        hist2D -> GetXaxis() -> SetTitle(vecVsAxisVars.at(iComp).data());
        hist2D -> GetYaxis() -> SetTitle(vecBaseAxisVars.at(iVar).data());
        hist2D -> GetZaxis() -> SetTitle(sCount.data());
        ++iComp;
      }
    }
    ++iVar;
  }
  cout << "    Set axis titles." << endl;

  // save histograms
  fOutput -> cd();
  for (auto histRow1D : vecHist1D) {
    for (auto hist1D : histRow1D) {
      hist1D -> Write();
    }
  }
  for (auto histRow2D : vecHist2D) {
    for (auto histType2D : histRow2D) {
      for (auto hist2D: histType2D) {
        hist2D -> Write();
      }
    }
  }
  cout << "    Saved histograms." << endl;

  // close files
  fOutput -> cd();
  fOutput -> Close();
  fInTrue -> cd();
  fInTrue -> Close();
  fInReco -> cd();
  fInReco -> Close();

  // exit
  cout << "  Finished new matcher plot script!\n" << endl;
  return;

}

// end ------------------------------------------------------------------------


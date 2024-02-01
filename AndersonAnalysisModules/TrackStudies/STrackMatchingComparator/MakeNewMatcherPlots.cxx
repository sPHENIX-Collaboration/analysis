// ----------------------------------------------------------------------------
// 'MakeNewMatcherPlots.cxx'
// Derek Anderson
// 08.24.2023
//
// Short macro to make plots from the
// output of the ClusMatchTree module.
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



void MakeNewMatcherPlots() {

  // io parameters
  const string sOutput("newMatcherPlots_oneMatchPerParticle_oddFrac02120.pt10n1evt500pim.d18m1y2024.root");
  const string sInTrue("input/merged/sPhenixG4_oneMatchPerParticle_newMatcher.pt10num1evt500pim.d4m1y2024.root");
  const string sInReco("input/merged/sPhenixG4_oneMatchPerParticle_newMatcher.pt10num1evt500pim.d4m1y2024.root");
  const string sTreeTrue("T");
  const string sTreeReco("T");

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

  // declare input leaves (ignore cluster branches for now)
  Int_t   tru_event;
  Int_t   tru_nphg4_part;
  Float_t tru_centrality;
  Int_t   tru_ntrackmatches;
  Int_t   tru_nphg4;
  Int_t   tru_nsvtx;
  Int_t   tru_trackid;
  Bool_t  tru_is_G4track;
  Bool_t  tru_is_Svtrack;
  Bool_t  tru_is_matched;
  Float_t tru_trkpt;
  Float_t tru_trketa;
  Float_t tru_trkphi;
  Int_t   tru_nclus;
  Int_t   tru_nclustpc;
  Int_t   tru_nclusmvtx;
  Int_t   tru_nclusintt;
  Float_t tru_matchrat;
  Float_t tru_matchrat_intt;
  Float_t tru_matchrat_mvtx;
  Float_t tru_matchrat_tpc;

  Int_t   rec_event;
  Int_t   rec_nphg4_part;
  Float_t rec_centrality;
  Int_t   rec_ntrackmatches;
  Int_t   rec_nphg4;
  Int_t   rec_nsvtx;
  Int_t   rec_trackid;
  Bool_t  rec_is_G4track;
  Bool_t  rec_is_Svtrack;
  Bool_t  rec_is_matched;
  Float_t rec_trkpt;
  Float_t rec_trketa;
  Float_t rec_trkphi;
  Int_t   rec_nclus;
  Int_t   rec_nclustpc;
  Int_t   rec_nclusmvtx;
  Int_t   rec_nclusintt;
  Float_t rec_matchrat;
  Float_t rec_matchrat_intt;
  Float_t rec_matchrat_mvtx;
  Float_t rec_matchrat_tpc;

  // set branch addresses (ignore cluster branches for now)
  tInTrue -> SetBranchAddress("event",         &tru_event);
  tInTrue -> SetBranchAddress("nphg4_part",    &tru_nphg4_part);
  tInTrue -> SetBranchAddress("centrality",    &tru_centrality);
  tInTrue -> SetBranchAddress("ntrackmatches", &tru_ntrackmatches);
  tInTrue -> SetBranchAddress("nphg4",         &tru_nphg4);
  tInTrue -> SetBranchAddress("nsvtx",         &tru_nsvtx);
  tInTrue -> SetBranchAddress("trackid",       &tru_trackid);
  tInTrue -> SetBranchAddress("is_G4track",    &tru_is_G4track);
  tInTrue -> SetBranchAddress("is_Svtrack",    &tru_is_Svtrack);
  tInTrue -> SetBranchAddress("is_matched",    &tru_is_matched);
  tInTrue -> SetBranchAddress("trkpt",         &tru_trkpt);
  tInTrue -> SetBranchAddress("trketa",        &tru_trketa);
  tInTrue -> SetBranchAddress("trkphi",        &tru_trkphi);
  tInTrue -> SetBranchAddress("nclus",         &tru_nclus);
  tInTrue -> SetBranchAddress("nclustpc",      &tru_nclustpc);
  tInTrue -> SetBranchAddress("nclusmvtx",     &tru_nclusmvtx);
  tInTrue -> SetBranchAddress("nclusintt",     &tru_nclusintt);
  tInTrue -> SetBranchAddress("matchrat",      &tru_matchrat);
  tInTrue -> SetBranchAddress("matchrat_intt", &tru_matchrat_intt);
  tInTrue -> SetBranchAddress("matchrat_mvtx", &tru_matchrat_mvtx);
  tInTrue -> SetBranchAddress("matchrat_tpc",  &tru_matchrat_tpc);

  tInReco -> SetBranchAddress("event",         &rec_event);
  tInReco -> SetBranchAddress("nphg4_part",    &rec_nphg4_part);
  tInReco -> SetBranchAddress("centrality",    &rec_centrality);
  tInReco -> SetBranchAddress("ntrackmatches", &rec_ntrackmatches);
  tInReco -> SetBranchAddress("nphg4",         &rec_nphg4);
  tInReco -> SetBranchAddress("nsvtx",         &rec_nsvtx);
  tInReco -> SetBranchAddress("trackid",       &rec_trackid);
  tInReco -> SetBranchAddress("is_G4track",    &rec_is_G4track);
  tInReco -> SetBranchAddress("is_Svtrack",    &rec_is_Svtrack);
  tInReco -> SetBranchAddress("is_matched",    &rec_is_matched);
  tInReco -> SetBranchAddress("trkpt",         &rec_trkpt);
  tInReco -> SetBranchAddress("trketa",        &rec_trketa);
  tInReco -> SetBranchAddress("trkphi",        &rec_trkphi);
  tInReco -> SetBranchAddress("nclus",         &rec_nclus);
  tInReco -> SetBranchAddress("nclustpc",      &rec_nclustpc);
  tInReco -> SetBranchAddress("nclusmvtx",     &rec_nclusmvtx);
  tInReco -> SetBranchAddress("nclusintt",     &rec_nclusintt);
  tInReco -> SetBranchAddress("matchrat",      &rec_matchrat);
  tInReco -> SetBranchAddress("matchrat_intt", &rec_matchrat_intt);
  tInReco -> SetBranchAddress("matchrat_mvtx", &rec_matchrat_mvtx);
  tInReco -> SetBranchAddress("matchrat_tpc",  &rec_matchrat_tpc);
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

    // select truth particles
    if (!tru_is_G4track) continue;

    // fill truth 1D histograms
    vecHist1D[Var::NTot][Type::Truth]  -> Fill(tru_nclus);
    vecHist1D[Var::NIntt][Type::Truth] -> Fill(tru_nclusintt);
    vecHist1D[Var::NMvtx][Type::Truth] -> Fill(tru_nclusmvtx);
    vecHist1D[Var::NTpc][Type::Truth]  -> Fill(tru_nclustpc);
    vecHist1D[Var::RTot][Type::Truth]  -> Fill(1.);
    vecHist1D[Var::RIntt][Type::Truth] -> Fill(1.);
    vecHist1D[Var::RMvtx][Type::Truth] -> Fill(1.);
    vecHist1D[Var::RTpc][Type::Truth]  -> Fill(1.);
    vecHist1D[Var::Phi][Type::Truth]   -> Fill(tru_trkphi);
    vecHist1D[Var::Eta][Type::Truth]   -> Fill(tru_trketa);
    vecHist1D[Var::Pt][Type::Truth]    -> Fill(tru_trkpt);
    vecHist1D[Var::Frac][Type::Truth]  -> Fill(1.);

    // fill truth 2D histograms
    vecHist2D[Var::NTot][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, tru_nclus);
    vecHist2D[Var::NIntt][Type::Truth][Comp::VsTruthPt] -> Fill(tru_trkpt, tru_nclusintt);
    vecHist2D[Var::NMvtx][Type::Truth][Comp::VsTruthPt] -> Fill(tru_trkpt, tru_nclusmvtx);
    vecHist2D[Var::NTpc][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, tru_nclustpc);
    vecHist2D[Var::RTot][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, 1.);
    vecHist2D[Var::RIntt][Type::Truth][Comp::VsTruthPt] -> Fill(tru_trkpt, 1.);
    vecHist2D[Var::RMvtx][Type::Truth][Comp::VsTruthPt] -> Fill(tru_trkpt, 1.);
    vecHist2D[Var::RTpc][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, 1.);
    vecHist2D[Var::Phi][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_trkpt, tru_trkphi);
    vecHist2D[Var::Eta][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_trkpt, tru_trketa);
    vecHist2D[Var::Pt][Type::Truth][Comp::VsTruthPt]    -> Fill(tru_trkpt, tru_trkpt);
    vecHist2D[Var::Frac][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, 1.);

    vecHist2D[Var::NTot][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, tru_nclus);
    vecHist2D[Var::NIntt][Type::Truth][Comp::VsNumTpc] -> Fill(tru_nclustpc, tru_nclusintt);
    vecHist2D[Var::NMvtx][Type::Truth][Comp::VsNumTpc] -> Fill(tru_nclustpc, tru_nclusmvtx);
    vecHist2D[Var::NTpc][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, tru_nclustpc);
    vecHist2D[Var::RTot][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, 1.);
    vecHist2D[Var::RIntt][Type::Truth][Comp::VsNumTpc] -> Fill(tru_nclustpc, 1.);
    vecHist2D[Var::RMvtx][Type::Truth][Comp::VsNumTpc] -> Fill(tru_nclustpc, 1.);
    vecHist2D[Var::RTpc][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, 1.);
    vecHist2D[Var::Phi][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_nclustpc, tru_trkphi);
    vecHist2D[Var::Eta][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_nclustpc, tru_trketa);
    vecHist2D[Var::Pt][Type::Truth][Comp::VsNumTpc]    -> Fill(tru_nclustpc, tru_trkpt);
    vecHist2D[Var::Frac][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, 1.);
  }  // end truth particle loop

  // announce next entry loop
  cout << "    Finished truth particle loop.\n"
       << "    Beginning reconstructed track loop: " << nRecoEntries << " to process"
       << endl;

  // loop over reco tracks
  // TODO identify matched truth particles
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

    // select only tracks matched to truth particle
    if (!rec_is_matched || !rec_is_Svtrack) continue;

    // fill all matched reco 1D histograms
    // FIXME actually calculate pt fraction
    vecHist1D[Var::NTot][Type::Track]  -> Fill(rec_nclus);
    vecHist1D[Var::NIntt][Type::Track] -> Fill(rec_nclusintt);
    vecHist1D[Var::NMvtx][Type::Track] -> Fill(rec_nclusmvtx);
    vecHist1D[Var::NTpc][Type::Track]  -> Fill(rec_nclustpc);
    vecHist1D[Var::RTot][Type::Track]  -> Fill(rec_matchrat);
    vecHist1D[Var::RIntt][Type::Track] -> Fill(rec_matchrat_intt);
    vecHist1D[Var::RMvtx][Type::Track] -> Fill(rec_matchrat_mvtx);
    vecHist1D[Var::RTpc][Type::Track]  -> Fill(rec_matchrat_tpc);
    vecHist1D[Var::Phi][Type::Track]   -> Fill(rec_trkphi);
    vecHist1D[Var::Eta][Type::Track]   -> Fill(rec_trketa);
    vecHist1D[Var::Pt][Type::Track]    -> Fill(rec_trkpt);
    vecHist1D[Var::Frac][Type::Track]  -> Fill(1.);

    // fill all matched reco 2D histograms
    // FIXME use actual truth pt & ntpc of matched particle
    vecHist2D[Var::NTot][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclus);
    vecHist2D[Var::NIntt][Type::Track][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_nclusintt);
    vecHist2D[Var::NMvtx][Type::Track][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_nclusmvtx);
    vecHist2D[Var::NTpc][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclustpc);
    vecHist2D[Var::RTot][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat);
    vecHist2D[Var::RIntt][Type::Track][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_matchrat_intt);
    vecHist2D[Var::RMvtx][Type::Track][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_matchrat_mvtx);
    vecHist2D[Var::RTpc][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat_tpc);
    vecHist2D[Var::Phi][Type::Track][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_trkphi);
    vecHist2D[Var::Eta][Type::Track][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_trketa);
    vecHist2D[Var::Pt][Type::Track][Comp::VsTruthPt]    -> Fill(rec_trkpt, rec_trkpt);
    vecHist2D[Var::Frac][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, 1.);

    vecHist2D[Var::NTot][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclus);
    vecHist2D[Var::NIntt][Type::Track][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_nclusintt);
    vecHist2D[Var::NMvtx][Type::Track][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_nclusmvtx);
    vecHist2D[Var::NTpc][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclustpc);
    vecHist2D[Var::RTot][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat);
    vecHist2D[Var::RIntt][Type::Track][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_matchrat_intt);
    vecHist2D[Var::RMvtx][Type::Track][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_matchrat_mvtx);
    vecHist2D[Var::RTpc][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat_tpc);
    vecHist2D[Var::Phi][Type::Track][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_trkphi);
    vecHist2D[Var::Eta][Type::Track][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_trketa);
    vecHist2D[Var::Pt][Type::Track][Comp::VsNumTpc]    -> Fill(rec_nclustpc, rec_trkpt);
    vecHist2D[Var::Frac][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, 1.);

    // fill weird and normal matched reco 1D histograms
    // FIXME actually check if is a weird track
    const bool isWeirdTrack = true;
    if (isWeirdTrack) {
      vecHist1D[Var::NTot][Type::Weird]  -> Fill(rec_nclus);
      vecHist1D[Var::NIntt][Type::Weird] -> Fill(rec_nclusintt);
      vecHist1D[Var::NMvtx][Type::Weird] -> Fill(rec_nclusmvtx);
      vecHist1D[Var::NTpc][Type::Weird]  -> Fill(rec_nclustpc);
      vecHist1D[Var::RTot][Type::Weird]  -> Fill(rec_matchrat);
      vecHist1D[Var::RIntt][Type::Weird] -> Fill(rec_matchrat_intt);
      vecHist1D[Var::RMvtx][Type::Weird] -> Fill(rec_matchrat_mvtx);
      vecHist1D[Var::RTpc][Type::Weird]  -> Fill(rec_matchrat_tpc);
      vecHist1D[Var::Phi][Type::Weird]   -> Fill(rec_trkphi);
      vecHist1D[Var::Eta][Type::Weird]   -> Fill(rec_trketa);
      vecHist1D[Var::Pt][Type::Weird]    -> Fill(rec_trkpt);
      vecHist1D[Var::Frac][Type::Weird]  -> Fill(1.);

      vecHist2D[Var::NTot][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclus);
      vecHist2D[Var::NIntt][Type::Weird][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_nclusintt);
      vecHist2D[Var::NMvtx][Type::Weird][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_nclusmvtx);
      vecHist2D[Var::NTpc][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclustpc);
      vecHist2D[Var::RTot][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat);
      vecHist2D[Var::RIntt][Type::Weird][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_matchrat_intt);
      vecHist2D[Var::RMvtx][Type::Weird][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_matchrat_mvtx);
      vecHist2D[Var::RTpc][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat_tpc);
      vecHist2D[Var::Phi][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_trkphi);
      vecHist2D[Var::Eta][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_trketa);
      vecHist2D[Var::Pt][Type::Weird][Comp::VsTruthPt]    -> Fill(rec_trkpt, rec_trkpt);
      vecHist2D[Var::Frac][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, 1.);

      vecHist2D[Var::NTot][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclus);
      vecHist2D[Var::NIntt][Type::Weird][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_nclusintt);
      vecHist2D[Var::NMvtx][Type::Weird][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_nclusmvtx);
      vecHist2D[Var::NTpc][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclustpc);
      vecHist2D[Var::RTot][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat);
      vecHist2D[Var::RIntt][Type::Weird][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_matchrat_intt);
      vecHist2D[Var::RMvtx][Type::Weird][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_matchrat_mvtx);
      vecHist2D[Var::RTpc][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat_tpc);
      vecHist2D[Var::Phi][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_trkphi);
      vecHist2D[Var::Eta][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_trketa);
      vecHist2D[Var::Pt][Type::Weird][Comp::VsNumTpc]    -> Fill(rec_nclustpc, rec_trkpt);
      vecHist2D[Var::Frac][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, 1.);
    } else {
      vecHist1D[Var::NTot][Type::Normal]  -> Fill(rec_nclus);
      vecHist1D[Var::NIntt][Type::Normal] -> Fill(rec_nclusintt);
      vecHist1D[Var::NMvtx][Type::Normal] -> Fill(rec_nclusmvtx);
      vecHist1D[Var::NTpc][Type::Normal]  -> Fill(rec_nclustpc);
      vecHist1D[Var::RTot][Type::Normal]  -> Fill(rec_matchrat);
      vecHist1D[Var::RIntt][Type::Normal] -> Fill(rec_matchrat_intt);
      vecHist1D[Var::RMvtx][Type::Normal] -> Fill(rec_matchrat_mvtx);
      vecHist1D[Var::RTpc][Type::Normal]  -> Fill(rec_matchrat_tpc);
      vecHist1D[Var::Phi][Type::Normal]   -> Fill(rec_trkphi);
      vecHist1D[Var::Eta][Type::Normal]   -> Fill(rec_trketa);
      vecHist1D[Var::Pt][Type::Normal]    -> Fill(rec_trkpt);
      vecHist1D[Var::Frac][Type::Normal]  -> Fill(1.);

      vecHist2D[Var::NTot][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclus);
      vecHist2D[Var::NIntt][Type::Normal][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_nclusintt);
      vecHist2D[Var::NMvtx][Type::Normal][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_nclusmvtx);
      vecHist2D[Var::NTpc][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclustpc);
      vecHist2D[Var::RTot][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat);
      vecHist2D[Var::RIntt][Type::Normal][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_matchrat_intt);
      vecHist2D[Var::RMvtx][Type::Normal][Comp::VsTruthPt] -> Fill(rec_trkpt, rec_matchrat_mvtx);
      vecHist2D[Var::RTpc][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat_tpc);
      vecHist2D[Var::Phi][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_trkphi);
      vecHist2D[Var::Eta][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_trketa);
      vecHist2D[Var::Pt][Type::Normal][Comp::VsTruthPt]    -> Fill(rec_trkpt, rec_trkpt);
      vecHist2D[Var::Frac][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, 1.);

      vecHist2D[Var::NTot][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclus);
      vecHist2D[Var::NIntt][Type::Normal][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_nclusintt);
      vecHist2D[Var::NMvtx][Type::Normal][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_nclusmvtx);
      vecHist2D[Var::NTpc][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclustpc);
      vecHist2D[Var::RTot][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat);
      vecHist2D[Var::RIntt][Type::Normal][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_matchrat_intt);
      vecHist2D[Var::RMvtx][Type::Normal][Comp::VsNumTpc] -> Fill(rec_nclustpc, rec_matchrat_mvtx);
      vecHist2D[Var::RTpc][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat_tpc);
      vecHist2D[Var::Phi][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_trkphi);
      vecHist2D[Var::Eta][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_trketa);
      vecHist2D[Var::Pt][Type::Normal][Comp::VsNumTpc]    -> Fill(rec_nclustpc, rec_trkpt);
      vecHist2D[Var::Frac][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, 1.);
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

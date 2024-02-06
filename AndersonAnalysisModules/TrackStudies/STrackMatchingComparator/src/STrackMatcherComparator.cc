// ----------------------------------------------------------------------------
// 'STrackMatcherComparator.cc'
// Derek Anderson
// 01.30.2024
//
// Small module to produce plots from the output of the
// SvtxEvaluator and FillMatchedClusTree modules.
// ----------------------------------------------------------------------------

// c++ utilities
#include <cmath>
#include <array>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <iostream>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TFile.h>
#include <TLine.h>
#include <TError.h>
#include <TString.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TDirectory.h>
// analysis utilities
#include "STrackMatcherComparator.h"
#include "STrackMatcherComparatorConfig.h"
#include "STrackMatcherComparatorHistDef.h"

// make common namespaces implicit
using namespace std;



// ctor/dtor ------------------------------------------------------------------

STrackMatcherComparator::STrackMatcherComparator(optional<STrackMatcherComparatorConfig> config) {

  if (config.has_value()) {
    m_config = config.value();
  }

  // make sure vectors are empty
  m_vecHistDirs.clear();
  m_vecRatioDirs.clear();
  m_vecPlotDirs.clear();
  m_vecTreeHists1D.clear();
  m_vecTupleHists1D.clear();
  m_vecOldHists1D.clear();
  m_vecTreeHists2D.clear();
  m_vecTupleHists2D.clear();
  m_vecOldHists2D.clear();

}  // end ctor



STrackMatcherComparator::~STrackMatcherComparator() {

  /* nothing to do */

}  // end dtor



// public methods -------------------------------------------------------------

void STrackMatcherComparator::Init() {

  // announce start
  cout << "\n  Beginning track matcher comparison...\n"
       << "    Initialization:"
       << endl;

  // run initialization routines
  OpenOutput();
  OpenInput();
  InitHists();
  return;

}  // end 'Init()'



void STrackMatcherComparator::Analyze() {

  // announce analyzing
  cout << "    Analysis:" << endl;

  // run analysis routines
  GetNewTreeHists();
  GetNewTupleHists();
  GetOldTupleHists();
  return;

}  // end 'Analysis()'



void STrackMatcherComparator::End() {

  // announce end
  cout << "    End:" << endl;

  // run ending routines
  MakeRatiosAndPlots(m_vecTreeHists1D,  m_vecTreeHists2D,  Src::NewTree,  "VsNewTree");
  MakeRatiosAndPlots(m_vecTupleHists1D, m_vecTupleHists2D, Src::NewTuple, "VsNewTuple");
  SaveHistograms();
  CloseInput();
  CloseOutput();

  // exit
  cout << "  Finished track matcher comparison!\n" << endl;
  return;
}



// internal methods -----------------------------------------------------------

void STrackMatcherComparator::OpenOutput() {

  // open output files
  m_outFile = new TFile(m_config.outFileName.data(), "recreate");
  if (!m_outFile) {
    cerr << "PANIC: couldn't open output file!" << endl;
    assert(m_outFile);
  }
  cout << "      Opened output file." << endl;

  // create output directories
  m_vecHistDirs.resize(m_const.nDirHist);
  m_vecRatioDirs.resize(m_const.nDirRatio);
  m_vecPlotDirs.resize(m_const.nDirPlot);

  m_vecHistDirs[Src::NewTree]   = (TDirectory*) m_outFile -> mkdir(m_config.histDirNames.at(Src::NewTree).data());
  m_vecHistDirs[Src::NewTuple]  = (TDirectory*) m_outFile -> mkdir(m_config.histDirNames.at(Src::NewTuple).data());
  m_vecHistDirs[Src::OldTuple]  = (TDirectory*) m_outFile -> mkdir(m_config.histDirNames.at(Src::OldTuple).data());
  m_vecRatioDirs[Src::NewTree]  = (TDirectory*) m_outFile -> mkdir(m_config.ratioDirNames.at(Src::NewTree).data());
  m_vecRatioDirs[Src::NewTuple] = (TDirectory*) m_outFile -> mkdir(m_config.ratioDirNames.at(Src::NewTuple).data());
  m_vecPlotDirs[Src::NewTree]   = (TDirectory*) m_outFile -> mkdir(m_config.plotDirNames.at(Src::NewTree).data());
  m_vecPlotDirs[Src::NewTuple]  = (TDirectory*) m_outFile -> mkdir(m_config.plotDirNames.at(Src::NewTuple).data());

  // announce end and return
  cout << "      Created output directories." << endl;
  return;

}  // end 'OpenOutput()'



void STrackMatcherComparator::OpenInput() {

  // open true files
  m_treeInFileTrue  = new TFile(m_config.newInFileName.data(), "read");
  m_tupleInFileTrue = new TFile(m_config.newInFileName.data(), "read");
  m_oldInFileTrue   = new TFile(m_config.oldInFileName.data(), "read");
  if (!m_treeInFileTrue || !m_tupleInFileTrue || !m_oldInFileTrue) {
    cerr << "PANIC: couldn't open an input file!\n"
         << "       m_treeInFileTrue  = " << m_treeInFileTrue  << "\n"
         << "       m_tupleInFileTrue = " << m_tupleInFileTrue << "\n"
         << "       m_oldInFileTrue   = " << m_oldInFileTrue
         << endl;
    assert(m_treeInFileTrue && m_tupleInFileTrue && m_oldInFileTrue);
  }
  cout << "      Opened truth input files." << endl;

  // open reco files
  m_treeInFileReco  = new TFile(m_config.newInFileName.data(), "read");
  m_tupleInFileReco = new TFile(m_config.newInFileName.data(), "read");
  m_oldInFileReco   = new TFile(m_config.oldInFileName.data(), "read");
  if (!m_treeInFileReco || !m_tupleInFileReco || !m_oldInFileReco) {
    cerr << "PANIC: couldn't open an input file!\n"
         << "       m_treeInFileReco  = " << m_treeInFileReco  << "\n"
         << "       m_tupleInFileReco = " << m_tupleInFileReco << "\n"
         << "       m_oldInFileReco   = " << m_oldInFileReco
         << endl;
    assert(m_treeInFileReco && m_tupleInFileReco && m_oldInFileReco);
  }
  cout << "      Opened reco input files." << endl;

  // grab true input trees/tuples
  m_tTreeTrue   = (TTree*)   m_treeInFileTrue  -> Get(m_config.newTreeTrueName.data());
  m_ntTupleTrue = (TNtuple*) m_tupleInFileTrue -> Get(m_config.newTupleTrueName.data());
  m_ntOldTrue   = (TNtuple*) m_oldInFileTrue   -> Get(m_config.oldTupleTrueName.data());
  if (!m_tTreeTrue || !m_ntTupleTrue || !m_ntOldTrue) {
    cerr << "PANIC: couldn't grab an input truth tree/tuple!\n"
         << "       m_tTreeTrue   = " << m_tTreeTrue   << "\n"
         << "       m_ntTupleTrue = " << m_ntTupleTrue << "\n"
         << "       m_ntOldTrue   = " << m_ntOldTrue
         << endl;
    assert(m_tTreeTrue && m_ntTupleTrue && m_ntOldTrue);
  }
  cout << "      Grabbed input truth trees/tuples." << endl;

  // grab reco input trees/tuples
  m_tTreeReco   = (TTree*)   m_treeInFileReco  -> Get(m_config.newTreeRecoName.data());
  m_ntTupleReco = (TNtuple*) m_tupleInFileReco -> Get(m_config.newTupleRecoName.data());
  m_ntOldReco   = (TNtuple*) m_oldInFileReco   -> Get(m_config.oldTupleRecoName.data());
  if (!m_tTreeReco || !m_ntTupleReco || !m_ntOldReco) {
    cerr << "PANIC: couldn't grab an input truth tree/tuple!\n"
         << "       m_tTreeReco   = " << m_tTreeReco   << "\n"
         << "       m_ntTupleReco = " << m_ntTupleReco << "\n"
         << "       m_ntOldReco   = " << m_ntOldReco
         << endl;
    assert(m_tTreeReco && m_ntTupleReco && m_ntOldReco);
  }

  // announce end and return
  cout << "      Grabbed input truth trees/tuples." << endl;
  return;

}  // end 'OpenInput()'



void STrackMatcherComparator::InitHists() {

  // announce start of routine
  const size_t nHistRows   = m_hist.vecNameBase.size();
  const size_t nHistTypes  = m_hist.vecNameBase[0].size();
  const size_t nVsHistMods = m_hist.vecVsModifiers.size();
  cout << "      Initializing histograms." << endl;

  // output histogram base binning definitions
  vector<tuple<uint32_t, pair<float, float>>> vecBaseHistBins = m_hist.GetVecHistBins();
  vector<tuple<uint32_t, pair<float, float>>> vecVsHistBins   = m_hist.GetVecVsHistBins();

  // turn on errors
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // set up 1d output histograms
  m_vecTreeHists1D.resize(nHistRows);
  m_vecTupleHists1D.resize(nHistRows);
  m_vecOldHists1D.resize(nHistRows);
  for (size_t iHistRow = 0; iHistRow < nHistRows; iHistRow++) {
    for (size_t iHistType = 0; iHistType < nHistTypes; iHistType++) {

      // create names
      const string sTreeHist1D  = m_hist.vecNameBase[iHistRow][iHistType] + "_" + m_config.newTreeLabel;
      const string sTupleHist1D = m_hist.vecNameBase[iHistRow][iHistType] + "_" + m_config.newTupleLabel;
      const string sOldHist1D   = m_hist.vecNameBase[iHistRow][iHistType] + "_" + m_config.oldTupleLabel;

      // create output histograms
      m_vecTreeHists1D[iHistRow].push_back(
        new TH1D(
          sTreeHist1D.data(),
          "",
          get<0>(vecBaseHistBins[iHistRow]),
          get<1>(vecBaseHistBins[iHistRow]).first,
          get<1>(vecBaseHistBins[iHistRow]).second
        )
      );
      m_vecTupleHists1D[iHistRow].push_back(
        new TH1D(
          sTupleHist1D.data(),
          "",
          get<0>(vecBaseHistBins[iHistRow]),
          get<1>(vecBaseHistBins[iHistRow]).first,
          get<1>(vecBaseHistBins[iHistRow]).second
        )
      );
      m_vecOldHists1D[iHistRow].push_back(
        new TH1D(
          sOldHist1D.data(),
          "",
          get<0>(vecBaseHistBins[iHistRow]),
          get<1>(vecBaseHistBins[iHistRow]).first,
          get<1>(vecBaseHistBins[iHistRow]).second
        )
      );

      // set axis titles
      m_vecTreeHists1D[iHistRow].back()  -> GetXaxis() -> SetTitle(m_hist.vecBaseAxisVars.at(iHistRow).data());
      m_vecTupleHists1D[iHistRow].back() -> GetXaxis() -> SetTitle(m_hist.vecBaseAxisVars.at(iHistRow).data());
      m_vecOldHists1D[iHistRow].back()   -> GetXaxis() -> SetTitle(m_hist.vecBaseAxisVars.at(iHistRow).data());
      m_vecTreeHists1D[iHistRow].back()  -> GetYaxis() -> SetTitle(m_config.count.data());
      m_vecTupleHists1D[iHistRow].back() -> GetYaxis() -> SetTitle(m_config.count.data());
      m_vecOldHists1D[iHistRow].back()   -> GetYaxis() -> SetTitle(m_config.count.data());
    }  // end type loop
  }  // end row loop
  cout << "      Initialized 1D histograms." << endl;

  // set up 2d output histograms
  m_vecTreeHists2D.resize(nHistRows);
  m_vecTupleHists2D.resize(nHistRows);
  m_vecOldHists2D.resize(nHistRows);
  for (size_t iHistRow = 0; iHistRow < nHistRows; iHistRow++) {

    // reserve space for all histograms in row
    m_vecTreeHists2D[iHistRow].resize(nHistTypes);
    m_vecTupleHists2D[iHistRow].resize(nHistTypes);
    m_vecOldHists2D[iHistRow].resize(nHistTypes);
    for (size_t iHistType = 0; iHistType < nHistTypes; iHistType++) {
      for (size_t iVsHistMod = 0; iVsHistMod < nVsHistMods; iVsHistMod++) {

        // create names
        const string sTreeHist2D  = m_hist.vecNameBase[iHistRow][iHistType] + m_hist.vecVsModifiers[iVsHistMod] + "_" + m_config.newTreeLabel;
        const string sTupleHist2D = m_hist.vecNameBase[iHistRow][iHistType] + m_hist.vecVsModifiers[iVsHistMod] + "_" + m_config.newTupleLabel;
        const string sOldHist2D   = m_hist.vecNameBase[iHistRow][iHistType] + m_hist.vecVsModifiers[iVsHistMod] + "_" + m_config.oldTupleLabel;

        // create output histograms
        m_vecTreeHists2D[iHistRow][iHistType].push_back(
          new TH2D(
            sTreeHist2D.data(),
            "",
            get<0>(vecVsHistBins[iVsHistMod]),
            get<1>(vecVsHistBins[iVsHistMod]).first,
            get<1>(vecVsHistBins[iVsHistMod]).second,
            get<0>(vecBaseHistBins[iHistRow]),
            get<1>(vecBaseHistBins[iHistRow]).first,
            get<1>(vecBaseHistBins[iHistRow]).second
          )
        );
        m_vecTupleHists2D[iHistRow][iHistType].push_back(
          new TH2D(
            sTupleHist2D.data(),
            "",
            get<0>(vecVsHistBins[iVsHistMod]),
            get<1>(vecVsHistBins[iVsHistMod]).first,
            get<1>(vecVsHistBins[iVsHistMod]).second,
            get<0>(vecBaseHistBins[iHistRow]),
            get<1>(vecBaseHistBins[iHistRow]).first,
            get<1>(vecBaseHistBins[iHistRow]).second
          )
        );
        m_vecOldHists2D[iHistRow][iHistType].push_back(
          new TH2D(
            sOldHist2D.data(),
            "",
            get<0>(vecVsHistBins[iVsHistMod]),
            get<1>(vecVsHistBins[iVsHistMod]).first,
            get<1>(vecVsHistBins[iVsHistMod]).second,
            get<0>(vecBaseHistBins[iHistRow]),
            get<1>(vecBaseHistBins[iHistRow]).first,
            get<1>(vecBaseHistBins[iHistRow]).second
          )
        );

        // set axis titles
        m_vecTreeHists2D[iHistRow][iHistType].back()  -> GetXaxis() -> SetTitle(m_hist.vecVsAxisVars.at(iVsHistMod).data());
        m_vecTupleHists2D[iHistRow][iHistType].back() -> GetXaxis() -> SetTitle(m_hist.vecVsAxisVars.at(iVsHistMod).data());
        m_vecOldHists2D[iHistRow][iHistType].back()   -> GetXaxis() -> SetTitle(m_hist.vecVsAxisVars.at(iVsHistMod).data());
        m_vecTreeHists2D[iHistRow][iHistType].back()  -> GetYaxis() -> SetTitle(m_hist.vecBaseAxisVars.at(iHistRow).data());
        m_vecTupleHists2D[iHistRow][iHistType].back() -> GetYaxis() -> SetTitle(m_hist.vecBaseAxisVars.at(iHistRow).data());
        m_vecOldHists2D[iHistRow][iHistType].back()   -> GetYaxis() -> SetTitle(m_hist.vecBaseAxisVars.at(iHistRow).data());
        m_vecTreeHists2D[iHistRow][iHistType].back()  -> GetZaxis() -> SetTitle(m_config.count.data());
        m_vecTupleHists2D[iHistRow][iHistType].back() -> GetZaxis() -> SetTitle(m_config.count.data());
        m_vecOldHists2D[iHistRow][iHistType].back()   -> GetZaxis() -> SetTitle(m_config.count.data());
      }  // end modifier loop
    }  // end type loop
  }  // end row loop

  // announce end and exit
  cout << "      Initialized 2D output histograms." << endl;
  return;

}  // end 'InitHists()'



void STrackMatcherComparator::GetNewTreeHists() {

  // announce start of routine
  cout << "      Grabbing new matcher tree histograms:" << endl;

  // declare input leaves (ignore cluster branches for now)
  int   tru_event;
  int   tru_nphg4_part;
  float tru_centrality;
  int   tru_ntrackmatches;
  int   tru_nphg4;
  int   tru_nsvtx;
  int   tru_trackid;
  bool  tru_is_G4track;
  bool  tru_is_Svtrack;
  bool  tru_is_matched;
  float tru_trkpt;
  float tru_trketa;
  float tru_trkphi;
  int   tru_nclus;
  int   tru_nclustpc;
  int   tru_nclusmvtx;
  int   tru_nclusintt;
  float tru_matchrat;
  float tru_matchrat_intt;
  float tru_matchrat_mvtx;
  float tru_matchrat_tpc;

  int   rec_event;
  int   rec_nphg4_part;
  float rec_centrality;
  int   rec_ntrackmatches;
  int   rec_nphg4;
  int   rec_nsvtx;
  int   rec_trackid;
  bool  rec_is_G4track;
  bool  rec_is_Svtrack;
  bool  rec_is_matched;
  float rec_trkpt;
  float rec_trketa;
  float rec_trkphi;
  int   rec_nclus;
  int   rec_nclustpc;
  int   rec_nclusmvtx;
  int   rec_nclusintt;
  float rec_matchrat;
  float rec_matchrat_intt;
  float rec_matchrat_mvtx;
  float rec_matchrat_tpc;

  // set branch addresses (ignore cluster branches for now)
  m_tTreeTrue -> SetBranchAddress("event",         &tru_event);
  m_tTreeTrue -> SetBranchAddress("nphg4_part",    &tru_nphg4_part);
  m_tTreeTrue -> SetBranchAddress("centrality",    &tru_centrality);
  m_tTreeTrue -> SetBranchAddress("ntrackmatches", &tru_ntrackmatches);
  m_tTreeTrue -> SetBranchAddress("nphg4",         &tru_nphg4);
  m_tTreeTrue -> SetBranchAddress("nsvtx",         &tru_nsvtx);
  m_tTreeTrue -> SetBranchAddress("trackid",       &tru_trackid);
  m_tTreeTrue -> SetBranchAddress("is_G4track",    &tru_is_G4track);
  m_tTreeTrue -> SetBranchAddress("is_Svtrack",    &tru_is_Svtrack);
  m_tTreeTrue -> SetBranchAddress("is_matched",    &tru_is_matched);
  m_tTreeTrue -> SetBranchAddress("trkpt",         &tru_trkpt);
  m_tTreeTrue -> SetBranchAddress("trketa",        &tru_trketa);
  m_tTreeTrue -> SetBranchAddress("trkphi",        &tru_trkphi);
  m_tTreeTrue -> SetBranchAddress("nclus",         &tru_nclus);
  m_tTreeTrue -> SetBranchAddress("nclustpc",      &tru_nclustpc);
  m_tTreeTrue -> SetBranchAddress("nclusmvtx",     &tru_nclusmvtx);
  m_tTreeTrue -> SetBranchAddress("nclusintt",     &tru_nclusintt);
  m_tTreeTrue -> SetBranchAddress("matchrat",      &tru_matchrat);
  m_tTreeTrue -> SetBranchAddress("matchrat_intt", &tru_matchrat_intt);
  m_tTreeTrue -> SetBranchAddress("matchrat_mvtx", &tru_matchrat_mvtx);
  m_tTreeTrue -> SetBranchAddress("matchrat_tpc",  &tru_matchrat_tpc);

  m_tTreeReco -> SetBranchAddress("event",         &rec_event);
  m_tTreeReco -> SetBranchAddress("nphg4_part",    &rec_nphg4_part);
  m_tTreeReco -> SetBranchAddress("centrality",    &rec_centrality);
  m_tTreeReco -> SetBranchAddress("ntrackmatches", &rec_ntrackmatches);
  m_tTreeReco -> SetBranchAddress("nphg4",         &rec_nphg4);
  m_tTreeReco -> SetBranchAddress("nsvtx",         &rec_nsvtx);
  m_tTreeReco -> SetBranchAddress("trackid",       &rec_trackid);
  m_tTreeReco -> SetBranchAddress("is_G4track",    &rec_is_G4track);
  m_tTreeReco -> SetBranchAddress("is_Svtrack",    &rec_is_Svtrack);
  m_tTreeReco -> SetBranchAddress("is_matched",    &rec_is_matched);
  m_tTreeReco -> SetBranchAddress("trkpt",         &rec_trkpt);
  m_tTreeReco -> SetBranchAddress("trketa",        &rec_trketa);
  m_tTreeReco -> SetBranchAddress("trkphi",        &rec_trkphi);
  m_tTreeReco -> SetBranchAddress("nclus",         &rec_nclus);
  m_tTreeReco -> SetBranchAddress("nclustpc",      &rec_nclustpc);
  m_tTreeReco -> SetBranchAddress("nclusmvtx",     &rec_nclusmvtx);
  m_tTreeReco -> SetBranchAddress("nclusintt",     &rec_nclusintt);
  m_tTreeReco -> SetBranchAddress("matchrat",      &rec_matchrat);
  m_tTreeReco -> SetBranchAddress("matchrat_intt", &rec_matchrat_intt);
  m_tTreeReco -> SetBranchAddress("matchrat_mvtx", &rec_matchrat_mvtx);
  m_tTreeReco -> SetBranchAddress("matchrat_tpc",  &rec_matchrat_tpc);
  cout << "        Set input branches." << endl;

  // grab no. of entries
  const int64_t nTrueEntries = m_tTreeTrue -> GetEntries();
  const int64_t nRecoEntries = m_tTreeReco -> GetEntries(); 
  cout << "        Beginning truth particle loop: " << nTrueEntries << " to process" << endl;

  // loop over truth particles
  int64_t nTrueBytes = 0;
  for (int64_t iTrueEntry = 0; iTrueEntry < nTrueEntries; iTrueEntry++) {

    // grab truth particle entry
    const int64_t trueBytes = m_tTreeTrue -> GetEntry(iTrueEntry);
    if (trueBytes < 0) {
      cerr << "PANIC: issue with entry " << iTrueEntry << "! Aborting loop!\n" << endl;
      break;
    } else {
      nTrueBytes += trueBytes;
    }

    const int64_t iTrueProg = iTrueEntry + 1;
    if (iTrueProg == nTrueEntries) {
      cout << "          Processing entry " << iTrueProg << "/" << nTrueEntries << "..." << endl;
    } else {
      cout << "          Processing entry " << iTrueProg << "/" << nTrueEntries << "...\r" << flush;
    }

    // select truth particles
    if (!tru_is_G4track) continue;

    // fill truth 1D histograms
    m_vecTreeHists1D[Var::NTot][Type::Truth]   -> Fill(tru_nclus);
    m_vecTreeHists1D[Var::NIntt][Type::Truth]  -> Fill(tru_nclusintt);
    m_vecTreeHists1D[Var::NMvtx][Type::Truth]  -> Fill(tru_nclusmvtx);
    m_vecTreeHists1D[Var::NTpc][Type::Truth]   -> Fill(tru_nclustpc);
    m_vecTreeHists1D[Var::RTot][Type::Truth]   -> Fill(1.);
    m_vecTreeHists1D[Var::RIntt][Type::Truth]  -> Fill(1.);
    m_vecTreeHists1D[Var::RMvtx][Type::Truth]  -> Fill(1.);
    m_vecTreeHists1D[Var::RTpc][Type::Truth]   -> Fill(1.);
    m_vecTreeHists1D[Var::Phi][Type::Truth]    -> Fill(tru_trkphi);
    m_vecTreeHists1D[Var::Eta][Type::Truth]    -> Fill(tru_trketa);
    m_vecTreeHists1D[Var::Pt][Type::Truth]     -> Fill(tru_trkpt);
    m_vecTreeHists1D[Var::Frac][Type::Truth]   -> Fill(1.);
    m_vecTreeHists1D[Var::Qual][Type::Truth]   -> Fill(1.);
    m_vecTreeHists1D[Var::PtErr][Type::Truth]  -> Fill(1.);
    m_vecTreeHists1D[Var::EtaErr][Type::Truth] -> Fill(1.);
    m_vecTreeHists1D[Var::PhiErr][Type::Truth] -> Fill(1.);
    m_vecTreeHists1D[Var::PtRes][Type::Truth]  -> Fill(1.);
    m_vecTreeHists1D[Var::EtaRes][Type::Truth] -> Fill(1.);
    m_vecTreeHists1D[Var::PhiRes][Type::Truth] -> Fill(1.);

    // fill truth 2D histograms
    m_vecTreeHists2D[Var::NTot][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_trkpt, tru_nclus);
    m_vecTreeHists2D[Var::NIntt][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, tru_nclusintt);
    m_vecTreeHists2D[Var::NMvtx][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, tru_nclusmvtx);
    m_vecTreeHists2D[Var::NTpc][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_trkpt, tru_nclustpc);
    m_vecTreeHists2D[Var::RTot][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::RIntt][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::RMvtx][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::RTpc][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::Phi][Type::Truth][Comp::VsTruthPt]    -> Fill(tru_trkpt, tru_trkphi);
    m_vecTreeHists2D[Var::Eta][Type::Truth][Comp::VsTruthPt]    -> Fill(tru_trkpt, tru_trketa);
    m_vecTreeHists2D[Var::Pt][Type::Truth][Comp::VsTruthPt]     -> Fill(tru_trkpt, tru_trkpt);
    m_vecTreeHists2D[Var::Frac][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::Qual][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::PtErr][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::EtaErr][Type::Truth][Comp::VsTruthPt] -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::PhiErr][Type::Truth][Comp::VsTruthPt] -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::PtRes][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::EtaRes][Type::Truth][Comp::VsTruthPt] -> Fill(tru_trkpt, 1.);
    m_vecTreeHists2D[Var::PhiRes][Type::Truth][Comp::VsTruthPt] -> Fill(tru_trkpt, 1.);

    m_vecTreeHists2D[Var::NTot][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_nclustpc, tru_nclus);
    m_vecTreeHists2D[Var::NIntt][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, tru_nclusintt);
    m_vecTreeHists2D[Var::NMvtx][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, tru_nclusmvtx);
    m_vecTreeHists2D[Var::NTpc][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_nclustpc, tru_nclustpc);
    m_vecTreeHists2D[Var::RTot][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::RIntt][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::RMvtx][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::RTpc][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::Phi][Type::Truth][Comp::VsNumTpc]    -> Fill(tru_nclustpc, tru_trkphi);
    m_vecTreeHists2D[Var::Eta][Type::Truth][Comp::VsNumTpc]    -> Fill(tru_nclustpc, tru_trketa);
    m_vecTreeHists2D[Var::Pt][Type::Truth][Comp::VsNumTpc]     -> Fill(tru_nclustpc, tru_trkpt);
    m_vecTreeHists2D[Var::Frac][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::Qual][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::PtErr][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::EtaErr][Type::Truth][Comp::VsNumTpc] -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::PhiErr][Type::Truth][Comp::VsNumTpc] -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::PtRes][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::EtaRes][Type::Truth][Comp::VsNumTpc] -> Fill(tru_nclustpc, 1.);
    m_vecTreeHists2D[Var::PhiRes][Type::Truth][Comp::VsNumTpc] -> Fill(tru_nclustpc, 1.);
  }  // end truth particle loop

  // announce next entry loop
  cout << "        Finished truth particle loop.\n"
       << "        Beginning reconstructed track loop: " << nRecoEntries << " to process"
       << endl;

  // loop over reco tracks
  // TODO identify matched truth particles
  int64_t nRecoBytes = 0;
  for (int64_t iRecoEntry = 0; iRecoEntry < nRecoEntries; iRecoEntry++) {

    // grab reco track entry
    const int64_t recoBytes = m_tTreeReco -> GetEntry(iRecoEntry);
    if (recoBytes < 0) {
      cerr << "PANIC: issue with entry " << iRecoEntry << "! Aborting loop!\n" << endl;
      break;
    } else {
      nRecoBytes += recoBytes;
    }

    const int64_t iRecoProg = iRecoEntry + 1;
    if (iRecoProg == nRecoEntries) {
      cout << "          Processing entry " << iRecoProg << "/" << nRecoEntries << "..." << endl;
    } else {
      cout << "          Processing entry " << iRecoProg << "/" << nRecoEntries << "...\r" << flush;
    }

    // select only tracks matched to truth particle
    if (!rec_is_matched || !rec_is_Svtrack) continue;

    // fill all matched reco 1D histograms
    //   - FIXME actually calculate pt fraction
    //   - FIXME include quality, errors, and resolutions
    m_vecTreeHists1D[Var::NTot][Type::Track]   -> Fill(rec_nclus);
    m_vecTreeHists1D[Var::NIntt][Type::Track]  -> Fill(rec_nclusintt);
    m_vecTreeHists1D[Var::NMvtx][Type::Track]  -> Fill(rec_nclusmvtx);
    m_vecTreeHists1D[Var::NTpc][Type::Track]   -> Fill(rec_nclustpc);
    m_vecTreeHists1D[Var::RTot][Type::Track]   -> Fill(rec_matchrat);
    m_vecTreeHists1D[Var::RIntt][Type::Track]  -> Fill(rec_matchrat_intt);
    m_vecTreeHists1D[Var::RMvtx][Type::Track]  -> Fill(rec_matchrat_mvtx);
    m_vecTreeHists1D[Var::RTpc][Type::Track]   -> Fill(rec_matchrat_tpc);
    m_vecTreeHists1D[Var::Phi][Type::Track]    -> Fill(rec_trkphi);
    m_vecTreeHists1D[Var::Eta][Type::Track]    -> Fill(rec_trketa);
    m_vecTreeHists1D[Var::Pt][Type::Track]     -> Fill(rec_trkpt);
    m_vecTreeHists1D[Var::Frac][Type::Track]   -> Fill(1.);
    m_vecTreeHists1D[Var::Frac][Type::Track]   -> Fill(1.);
    m_vecTreeHists1D[Var::Qual][Type::Track]   -> Fill(1.);
    m_vecTreeHists1D[Var::PtErr][Type::Track]  -> Fill(1.);
    m_vecTreeHists1D[Var::EtaErr][Type::Track] -> Fill(1.);
    m_vecTreeHists1D[Var::PhiErr][Type::Track] -> Fill(1.);
    m_vecTreeHists1D[Var::PtRes][Type::Track]  -> Fill(1.);
    m_vecTreeHists1D[Var::EtaRes][Type::Track] -> Fill(1.);
    m_vecTreeHists1D[Var::PhiRes][Type::Track] -> Fill(1.);

    // fill all matched reco 2D histograms
    //   - FIXME use actual truth pt & ntpc of matched particle
    m_vecTreeHists2D[Var::NTot][Type::Track][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_nclus);
    m_vecTreeHists2D[Var::NIntt][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclusintt);
    m_vecTreeHists2D[Var::NMvtx][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclusmvtx);
    m_vecTreeHists2D[Var::NTpc][Type::Track][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_nclustpc);
    m_vecTreeHists2D[Var::RTot][Type::Track][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_matchrat);
    m_vecTreeHists2D[Var::RIntt][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat_intt);
    m_vecTreeHists2D[Var::RMvtx][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat_mvtx);
    m_vecTreeHists2D[Var::RTpc][Type::Track][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_matchrat_tpc);
    m_vecTreeHists2D[Var::Phi][Type::Track][Comp::VsTruthPt]    -> Fill(rec_trkpt, rec_trkphi);
    m_vecTreeHists2D[Var::Eta][Type::Track][Comp::VsTruthPt]    -> Fill(rec_trkpt, rec_trketa);
    m_vecTreeHists2D[Var::Pt][Type::Track][Comp::VsTruthPt]     -> Fill(rec_trkpt, rec_trkpt);
    m_vecTreeHists2D[Var::Frac][Type::Track][Comp::VsTruthPt]   -> Fill(rec_trkpt, 1.);
    m_vecTreeHists2D[Var::Qual][Type::Track][Comp::VsTruthPt]   -> Fill(rec_trkpt, 1.);
    m_vecTreeHists2D[Var::PtErr][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, 1.);
    m_vecTreeHists2D[Var::EtaErr][Type::Track][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);
    m_vecTreeHists2D[Var::PhiErr][Type::Track][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);
    m_vecTreeHists2D[Var::PtRes][Type::Track][Comp::VsTruthPt]  -> Fill(rec_trkpt, 1.);
    m_vecTreeHists2D[Var::EtaRes][Type::Track][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);
    m_vecTreeHists2D[Var::PhiRes][Type::Track][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);

    m_vecTreeHists2D[Var::NTot][Type::Track][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_nclus);
    m_vecTreeHists2D[Var::NIntt][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclusintt);
    m_vecTreeHists2D[Var::NMvtx][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclusmvtx);
    m_vecTreeHists2D[Var::NTpc][Type::Track][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_nclustpc);
    m_vecTreeHists2D[Var::RTot][Type::Track][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_matchrat);
    m_vecTreeHists2D[Var::RIntt][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat_intt);
    m_vecTreeHists2D[Var::RMvtx][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat_mvtx);
    m_vecTreeHists2D[Var::RTpc][Type::Track][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_matchrat_tpc);
    m_vecTreeHists2D[Var::Phi][Type::Track][Comp::VsNumTpc]    -> Fill(rec_nclustpc, rec_trkphi);
    m_vecTreeHists2D[Var::Eta][Type::Track][Comp::VsNumTpc]    -> Fill(rec_nclustpc, rec_trketa);
    m_vecTreeHists2D[Var::Pt][Type::Track][Comp::VsNumTpc]     -> Fill(rec_nclustpc, rec_trkpt);
    m_vecTreeHists2D[Var::Frac][Type::Track][Comp::VsNumTpc]   -> Fill(rec_nclustpc, 1.);
    m_vecTreeHists2D[Var::Qual][Type::Track][Comp::VsNumTpc]   -> Fill(rec_nclustpc, 1.);
    m_vecTreeHists2D[Var::PtErr][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, 1.);
    m_vecTreeHists2D[Var::EtaErr][Type::Track][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
    m_vecTreeHists2D[Var::PhiErr][Type::Track][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
    m_vecTreeHists2D[Var::PtRes][Type::Track][Comp::VsNumTpc]  -> Fill(rec_nclustpc, 1.);
    m_vecTreeHists2D[Var::EtaRes][Type::Track][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
    m_vecTreeHists2D[Var::PhiRes][Type::Track][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);

    // fill weird and normal matched reco 1D histograms
    // FIXME actually check if is a weird track
    const bool isWeirdTrack = true;
    if (isWeirdTrack) {
      m_vecTreeHists1D[Var::NTot][Type::Weird]   -> Fill(rec_nclus);
      m_vecTreeHists1D[Var::NIntt][Type::Weird]  -> Fill(rec_nclusintt);
      m_vecTreeHists1D[Var::NMvtx][Type::Weird]  -> Fill(rec_nclusmvtx);
      m_vecTreeHists1D[Var::NTpc][Type::Weird]   -> Fill(rec_nclustpc);
      m_vecTreeHists1D[Var::RTot][Type::Weird]   -> Fill(rec_matchrat);
      m_vecTreeHists1D[Var::RIntt][Type::Weird]  -> Fill(rec_matchrat_intt);
      m_vecTreeHists1D[Var::RMvtx][Type::Weird]  -> Fill(rec_matchrat_mvtx);
      m_vecTreeHists1D[Var::RTpc][Type::Weird]   -> Fill(rec_matchrat_tpc);
      m_vecTreeHists1D[Var::Phi][Type::Weird]    -> Fill(rec_trkphi);
      m_vecTreeHists1D[Var::Eta][Type::Weird]    -> Fill(rec_trketa);
      m_vecTreeHists1D[Var::Pt][Type::Weird]     -> Fill(rec_trkpt);
      m_vecTreeHists1D[Var::Frac][Type::Weird]   -> Fill(1.);
      m_vecTreeHists1D[Var::Qual][Type::Weird]   -> Fill(1.);
      m_vecTreeHists1D[Var::PtErr][Type::Weird]  -> Fill(1.);
      m_vecTreeHists1D[Var::EtaErr][Type::Weird] -> Fill(1.);
      m_vecTreeHists1D[Var::PhiErr][Type::Weird] -> Fill(1.);
      m_vecTreeHists1D[Var::PtRes][Type::Weird]  -> Fill(1.);
      m_vecTreeHists1D[Var::EtaRes][Type::Weird] -> Fill(1.);
      m_vecTreeHists1D[Var::PhiRes][Type::Weird] -> Fill(1.);

      m_vecTreeHists2D[Var::NTot][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_nclus);
      m_vecTreeHists2D[Var::NIntt][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclusintt);
      m_vecTreeHists2D[Var::NMvtx][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclusmvtx);
      m_vecTreeHists2D[Var::NTpc][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_nclustpc);
      m_vecTreeHists2D[Var::RTot][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_matchrat);
      m_vecTreeHists2D[Var::RIntt][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat_intt);
      m_vecTreeHists2D[Var::RMvtx][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat_mvtx);
      m_vecTreeHists2D[Var::RTpc][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_matchrat_tpc);
      m_vecTreeHists2D[Var::Phi][Type::Weird][Comp::VsTruthPt]    -> Fill(rec_trkpt, rec_trkphi);
      m_vecTreeHists2D[Var::Eta][Type::Weird][Comp::VsTruthPt]    -> Fill(rec_trkpt, rec_trketa);
      m_vecTreeHists2D[Var::Pt][Type::Weird][Comp::VsTruthPt]     -> Fill(rec_trkpt, rec_trkpt);
      m_vecTreeHists2D[Var::Frac][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::Qual][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::PtErr][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::EtaErr][Type::Weird][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::PhiErr][Type::Weird][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::PtRes][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::EtaRes][Type::Weird][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::PhiRes][Type::Weird][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);

      m_vecTreeHists2D[Var::NTot][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_nclus);
      m_vecTreeHists2D[Var::NIntt][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclusintt);
      m_vecTreeHists2D[Var::NMvtx][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclusmvtx);
      m_vecTreeHists2D[Var::NTpc][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_nclustpc);
      m_vecTreeHists2D[Var::RTot][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_matchrat);
      m_vecTreeHists2D[Var::RIntt][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat_intt);
      m_vecTreeHists2D[Var::RMvtx][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat_mvtx);
      m_vecTreeHists2D[Var::RTpc][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_matchrat_tpc);
      m_vecTreeHists2D[Var::Phi][Type::Weird][Comp::VsNumTpc]    -> Fill(rec_nclustpc, rec_trkphi);
      m_vecTreeHists2D[Var::Eta][Type::Weird][Comp::VsNumTpc]    -> Fill(rec_nclustpc, rec_trketa);
      m_vecTreeHists2D[Var::Pt][Type::Weird][Comp::VsNumTpc]     -> Fill(rec_nclustpc, rec_trkpt);
      m_vecTreeHists2D[Var::Frac][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::Qual][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::PtErr][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::EtaErr][Type::Weird][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::PhiErr][Type::Weird][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::PtRes][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::EtaRes][Type::Weird][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::PhiRes][Type::Weird][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
    } else {
      m_vecTreeHists1D[Var::NTot][Type::Normal]   -> Fill(rec_nclus);
      m_vecTreeHists1D[Var::NIntt][Type::Normal]  -> Fill(rec_nclusintt);
      m_vecTreeHists1D[Var::NMvtx][Type::Normal]  -> Fill(rec_nclusmvtx);
      m_vecTreeHists1D[Var::NTpc][Type::Normal]   -> Fill(rec_nclustpc);
      m_vecTreeHists1D[Var::RTot][Type::Normal]   -> Fill(rec_matchrat);
      m_vecTreeHists1D[Var::RIntt][Type::Normal]  -> Fill(rec_matchrat_intt);
      m_vecTreeHists1D[Var::RMvtx][Type::Normal]  -> Fill(rec_matchrat_mvtx);
      m_vecTreeHists1D[Var::RTpc][Type::Normal]   -> Fill(rec_matchrat_tpc);
      m_vecTreeHists1D[Var::Phi][Type::Normal]    -> Fill(rec_trkphi);
      m_vecTreeHists1D[Var::Eta][Type::Normal]    -> Fill(rec_trketa);
      m_vecTreeHists1D[Var::Pt][Type::Normal]     -> Fill(rec_trkpt);
      m_vecTreeHists1D[Var::Frac][Type::Normal]   -> Fill(1.);
      m_vecTreeHists1D[Var::Qual][Type::Normal]   -> Fill(1.);
      m_vecTreeHists1D[Var::PtErr][Type::Normal]  -> Fill(1.);
      m_vecTreeHists1D[Var::EtaErr][Type::Normal] -> Fill(1.);
      m_vecTreeHists1D[Var::PhiErr][Type::Normal] -> Fill(1.);
      m_vecTreeHists1D[Var::PtRes][Type::Normal]  -> Fill(1.);
      m_vecTreeHists1D[Var::EtaRes][Type::Normal] -> Fill(1.);
      m_vecTreeHists1D[Var::PhiRes][Type::Normal] -> Fill(1.);

      m_vecTreeHists2D[Var::NTot][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_nclus);
      m_vecTreeHists2D[Var::NIntt][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclusintt);
      m_vecTreeHists2D[Var::NMvtx][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_nclusmvtx);
      m_vecTreeHists2D[Var::NTpc][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_nclustpc);
      m_vecTreeHists2D[Var::RTot][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_matchrat);
      m_vecTreeHists2D[Var::RIntt][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat_intt);
      m_vecTreeHists2D[Var::RMvtx][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, rec_matchrat_mvtx);
      m_vecTreeHists2D[Var::RTpc][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_trkpt, rec_matchrat_tpc);
      m_vecTreeHists2D[Var::Phi][Type::Normal][Comp::VsTruthPt]    -> Fill(rec_trkpt, rec_trkphi);
      m_vecTreeHists2D[Var::Eta][Type::Normal][Comp::VsTruthPt]    -> Fill(rec_trkpt, rec_trketa);
      m_vecTreeHists2D[Var::Pt][Type::Normal][Comp::VsTruthPt]     -> Fill(rec_trkpt, rec_trkpt);
      m_vecTreeHists2D[Var::Frac][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::Qual][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::PtErr][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::EtaErr][Type::Normal][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::PhiErr][Type::Normal][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::PtRes][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::EtaRes][Type::Normal][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);
      m_vecTreeHists2D[Var::PhiRes][Type::Normal][Comp::VsTruthPt] -> Fill(rec_trkpt, 1.);

      m_vecTreeHists2D[Var::NTot][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_nclus);
      m_vecTreeHists2D[Var::NIntt][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclusintt);
      m_vecTreeHists2D[Var::NMvtx][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_nclusmvtx);
      m_vecTreeHists2D[Var::NTpc][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_nclustpc);
      m_vecTreeHists2D[Var::RTot][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_matchrat);
      m_vecTreeHists2D[Var::RIntt][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat_intt);
      m_vecTreeHists2D[Var::RMvtx][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, rec_matchrat_mvtx);
      m_vecTreeHists2D[Var::RTpc][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_nclustpc, rec_matchrat_tpc);
      m_vecTreeHists2D[Var::Phi][Type::Normal][Comp::VsNumTpc]    -> Fill(rec_nclustpc, rec_trkphi);
      m_vecTreeHists2D[Var::Eta][Type::Normal][Comp::VsNumTpc]    -> Fill(rec_nclustpc, rec_trketa);
      m_vecTreeHists2D[Var::Pt][Type::Normal][Comp::VsNumTpc]     -> Fill(rec_nclustpc, rec_trkpt);
      m_vecTreeHists2D[Var::Frac][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::Qual][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::PtErr][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::EtaErr][Type::Normal][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::PhiErr][Type::Normal][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::PtRes][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::EtaRes][Type::Normal][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
      m_vecTreeHists2D[Var::PhiRes][Type::Normal][Comp::VsNumTpc] -> Fill(rec_nclustpc, 1.);
    }
  }  // end reco track loop

  // announce method end and return
  cout << "        Finished reconstructed track loop.\n"
       << "      Finished getting histograms from new matcher cluster tree."
       << endl;
  return;

}  // end 'GetNewTreeHists()'



void STrackMatcherComparator::GetNewTupleHists() {

  // announce start of method
  cout << "      Grabbing new matcher tuple histograms:" << endl;

  // declare input leaves
  float tru_evtid;
  float tru_trkid;
  float tru_pt;
  float tru_eta;
  float tru_phi;
  float tru_nmvtxclust_trkmatcher;
  float tru_ninttclust_trkmatcher;
  float tru_ntpclust_trkmatcher;
  float tru_nmvtxclust_manual;
  float tru_ninttclust_manual;
  float tru_ntpcclust_manual;
  float tru_nmvtxlayer_;
  float tru_ninttlayer;
  float tru_ntpclayer;
  float tru_deltapt;
  float tru_quality;
  float tru_dcaxy;
  float tru_dcaz;
  float tru_sigmadcaxy;
  float tru_sigmadacz;
  float tru_vx;
  float tru_vy;
  float tru_vz;
  float tru_gtrkid;
  float tru_gpt;
  float tru_geta;
  float tru_gphi;
  float tru_gnmvtxclust_trkmatcher;
  float tru_gninttclust_trkmatcher;
  float tru_gntpclust_trkmatcher;
  float tru_gnmvtxclust_manual;
  float tru_gninttclust_manual;
  float tru_gntpcclust_manual;
  float tru_gnmvtxlayer;
  float tru_gninttlayer;
  float tru_gntpclayer;
  float tru_gdeltapt;
  float tru_gquality;
  float tru_gdcaxy;
  float tru_gdcaz;
  float tru_gsigmadcaxy;
  float tru_gsigmadacz;
  float tru_gvx;
  float tru_gvy;
  float tru_gvz;
  float tru_fracnmvtxmatched;
  float tru_fracninttmatched;
  float tru_fracntpcmatched;

  float rec_evtid;
  float rec_trkid;
  float rec_pt;
  float rec_eta;
  float rec_phi;
  float rec_nmvtxclust_trkmatcher;
  float rec_ninttclust_trkmatcher;
  float rec_ntpclust_trkmatcher;
  float rec_nmvtxclust_manual;
  float rec_ninttclust_manual;
  float rec_ntpcclust_manual;
  float rec_nmvtxlayer_;
  float rec_ninttlayer;
  float rec_ntpclayer;
  float rec_deltapt;
  float rec_quality;
  float rec_dcaxy;
  float rec_dcaz;
  float rec_sigmadcaxy;
  float rec_sigmadacz;
  float rec_vx;
  float rec_vy;
  float rec_vz;
  float rec_gtrkid;
  float rec_gpt;
  float rec_geta;
  float rec_gphi;
  float rec_gnmvtxclust_trkmatcher;
  float rec_gninttclust_trkmatcher;
  float rec_gntpclust_trkmatcher;
  float rec_gnmvtxclust_manual;
  float rec_gninttclust_manual;
  float rec_gntpcclust_manual;
  float rec_gnmvtxlayer;
  float rec_gninttlayer;
  float rec_gntpclayer;
  float rec_gdeltapt;
  float rec_gquality;
  float rec_gdcaxy;
  float rec_gdcaz;
  float rec_gsigmadcaxy;
  float rec_gsigmadacz;
  float rec_gvx;
  float rec_gvy;
  float rec_gvz;
  float rec_fracnmvtxmatched;
  float rec_fracninttmatched;
  float rec_fracntpcmatched;
 
  // set branch addresses
  m_ntTupleTrue -> SetBranchAddress("evtid",                 &tru_evtid);
  m_ntTupleTrue -> SetBranchAddress("trkid",                 &tru_trkid);
  m_ntTupleTrue -> SetBranchAddress("pt",                    &tru_pt);
  m_ntTupleTrue -> SetBranchAddress("eta",                   &tru_eta);
  m_ntTupleTrue -> SetBranchAddress("phi",                   &tru_phi);
  m_ntTupleTrue -> SetBranchAddress("nmvtxclust_trkmatcher", &tru_nmvtxclust_trkmatcher);
  m_ntTupleTrue -> SetBranchAddress("ninttclust_trkmatcher", &tru_ninttclust_trkmatcher);
  m_ntTupleTrue -> SetBranchAddress("ntpclust_trkmatcher",   &tru_ntpclust_trkmatcher);
  m_ntTupleTrue -> SetBranchAddress("nmvtxclust_manual",     &tru_nmvtxclust_manual);
  m_ntTupleTrue -> SetBranchAddress("ninttclust_manual",     &tru_ninttclust_manual);
  m_ntTupleTrue -> SetBranchAddress("ntpcclust_manual",      &tru_ntpcclust_manual);
  m_ntTupleTrue -> SetBranchAddress("nmvtxlayer_",           &tru_nmvtxlayer_);
  m_ntTupleTrue -> SetBranchAddress("ninttlayer",            &tru_ninttlayer);
  m_ntTupleTrue -> SetBranchAddress("ntpclayer",             &tru_ntpclayer);
  m_ntTupleTrue -> SetBranchAddress("deltapt",               &tru_deltapt);
  m_ntTupleTrue -> SetBranchAddress("quality",               &tru_quality);
  m_ntTupleTrue -> SetBranchAddress("dcaxy",                 &tru_dcaxy);
  m_ntTupleTrue -> SetBranchAddress("dcaz",                  &tru_dcaz);
  m_ntTupleTrue -> SetBranchAddress("sigmadcaxy",            &tru_sigmadcaxy);
  m_ntTupleTrue -> SetBranchAddress("sigmadacz",             &tru_sigmadacz);
  m_ntTupleTrue -> SetBranchAddress("vx",                    &tru_vx);
  m_ntTupleTrue -> SetBranchAddress("vy",                    &tru_vy);
  m_ntTupleTrue -> SetBranchAddress("vz",                    &tru_vz);
  m_ntTupleTrue -> SetBranchAddress("gtrkid",                &tru_gtrkid);
  m_ntTupleTrue -> SetBranchAddress("gpt",                   &tru_gpt);
  m_ntTupleTrue -> SetBranchAddress("geta",                  &tru_geta);
  m_ntTupleTrue -> SetBranchAddress("gphi",                  &tru_gphi);
  m_ntTupleTrue -> SetBranchAddress("gnmvtxclust_trkmatcher",&tru_gnmvtxclust_trkmatcher);
  m_ntTupleTrue -> SetBranchAddress("gninttclust_trkmatcher",&tru_gninttclust_trkmatcher);
  m_ntTupleTrue -> SetBranchAddress("gntpclust_trkmatcher",  &tru_gntpclust_trkmatcher);
  m_ntTupleTrue -> SetBranchAddress("gnmvtxclust_manual",    &tru_gnmvtxclust_manual);
  m_ntTupleTrue -> SetBranchAddress("gninttclust_manual",    &tru_gninttclust_manual);
  m_ntTupleTrue -> SetBranchAddress("gntpcclust_manual",     &tru_gntpcclust_manual);
  m_ntTupleTrue -> SetBranchAddress("gnmvtxlayer",           &tru_gnmvtxlayer);
  m_ntTupleTrue -> SetBranchAddress("gninttlayer",           &tru_gninttlayer);
  m_ntTupleTrue -> SetBranchAddress("gntpclayer",            &tru_gntpclayer);
  m_ntTupleTrue -> SetBranchAddress("gdeltapt",              &tru_gdeltapt);
  m_ntTupleTrue -> SetBranchAddress("gquality",              &tru_gquality);
  m_ntTupleTrue -> SetBranchAddress("gdcaxy",                &tru_gdcaxy);
  m_ntTupleTrue -> SetBranchAddress("gdcaz",                 &tru_gdcaz);
  m_ntTupleTrue -> SetBranchAddress("gsigmadcaxy",           &tru_gsigmadcaxy);
  m_ntTupleTrue -> SetBranchAddress("gsigmadacz",            &tru_gsigmadacz);
  m_ntTupleTrue -> SetBranchAddress("gvx",                   &tru_gvx);
  m_ntTupleTrue -> SetBranchAddress("gvy",                   &tru_gvy);
  m_ntTupleTrue -> SetBranchAddress("gvz",                   &tru_gvz);
  m_ntTupleTrue -> SetBranchAddress("fracnmvtxmatched",      &tru_fracnmvtxmatched);
  m_ntTupleTrue -> SetBranchAddress("fracninttmatched",      &tru_fracninttmatched);
  m_ntTupleTrue -> SetBranchAddress("fracntpcmatched",       &tru_fracntpcmatched);

  m_ntTupleReco -> SetBranchAddress("evtid",                  &rec_evtid);
  m_ntTupleReco -> SetBranchAddress("trkid",                  &rec_trkid);
  m_ntTupleReco -> SetBranchAddress("pt",                     &rec_pt);
  m_ntTupleReco -> SetBranchAddress("eta",                    &rec_eta);
  m_ntTupleReco -> SetBranchAddress("phi",                    &rec_phi);
  m_ntTupleReco -> SetBranchAddress("nmvtxclust_trkmatcher",  &rec_nmvtxclust_trkmatcher);
  m_ntTupleReco -> SetBranchAddress("ninttclust_trkmatcher",  &rec_ninttclust_trkmatcher);
  m_ntTupleReco -> SetBranchAddress("ntpclust_trkmatcher",    &rec_ntpclust_trkmatcher);
  m_ntTupleReco -> SetBranchAddress("nmvtxclust_manual",      &rec_nmvtxclust_manual);
  m_ntTupleReco -> SetBranchAddress("ninttclust_manual",      &rec_ninttclust_manual);
  m_ntTupleReco -> SetBranchAddress("ntpcclust_manual",       &rec_ntpcclust_manual);
  m_ntTupleReco -> SetBranchAddress("nmvtxlayer_",            &rec_nmvtxlayer_);
  m_ntTupleReco -> SetBranchAddress("ninttlayer",             &rec_ninttlayer);
  m_ntTupleReco -> SetBranchAddress("ntpclayer",              &rec_ntpclayer);
  m_ntTupleReco -> SetBranchAddress("deltapt",                &rec_deltapt);
  m_ntTupleReco -> SetBranchAddress("quality",                &rec_quality);
  m_ntTupleReco -> SetBranchAddress("dcaxy",                  &rec_dcaxy);
  m_ntTupleReco -> SetBranchAddress("dcaz",                   &rec_dcaz);
  m_ntTupleReco -> SetBranchAddress("sigmadcaxy",             &rec_sigmadcaxy);
  m_ntTupleReco -> SetBranchAddress("sigmadacz",              &rec_sigmadacz);
  m_ntTupleReco -> SetBranchAddress("vx",                     &rec_vx);
  m_ntTupleReco -> SetBranchAddress("vy",                     &rec_vy);
  m_ntTupleReco -> SetBranchAddress("vz",                     &rec_vz);
  m_ntTupleReco -> SetBranchAddress("gtrkid",                 &rec_gtrkid);
  m_ntTupleReco -> SetBranchAddress("gpt",                    &rec_gpt);
  m_ntTupleReco -> SetBranchAddress("geta",                   &rec_geta);
  m_ntTupleReco -> SetBranchAddress("gphi",                   &rec_gphi);
  m_ntTupleReco -> SetBranchAddress("gnmvtxclust_trkmatcher", &rec_gnmvtxclust_trkmatcher);
  m_ntTupleReco -> SetBranchAddress("gninttclust_trkmatcher", &rec_gninttclust_trkmatcher);
  m_ntTupleReco -> SetBranchAddress("gntpclust_trkmatcher",   &rec_gntpclust_trkmatcher);
  m_ntTupleReco -> SetBranchAddress("gnmvtxclust_manual",     &rec_gnmvtxclust_manual);
  m_ntTupleReco -> SetBranchAddress("gninttclust_manual",     &rec_gninttclust_manual);
  m_ntTupleReco -> SetBranchAddress("gntpcclust_manual",      &rec_gntpcclust_manual);
  m_ntTupleReco -> SetBranchAddress("gnmvtxlayer",            &rec_gnmvtxlayer);
  m_ntTupleReco -> SetBranchAddress("gninttlayer",            &rec_gninttlayer);
  m_ntTupleReco -> SetBranchAddress("gntpclayer",             &rec_gntpclayer);
  m_ntTupleReco -> SetBranchAddress("gdeltapt",               &rec_gdeltapt);
  m_ntTupleReco -> SetBranchAddress("gquality",               &rec_gquality);
  m_ntTupleReco -> SetBranchAddress("gdcaxy",                 &rec_gdcaxy);
  m_ntTupleReco -> SetBranchAddress("gdcaz",                  &rec_gdcaz);
  m_ntTupleReco -> SetBranchAddress("gsigmadcaxy",            &rec_gsigmadcaxy);
  m_ntTupleReco -> SetBranchAddress("gsigmadacz",             &rec_gsigmadacz);
  m_ntTupleReco -> SetBranchAddress("gvx",                    &rec_gvx);
  m_ntTupleReco -> SetBranchAddress("gvy",                    &rec_gvy);
  m_ntTupleReco -> SetBranchAddress("gvz",                    &rec_gvz);
  m_ntTupleReco -> SetBranchAddress("fracnmvtxmatched",       &rec_fracnmvtxmatched);
  m_ntTupleReco -> SetBranchAddress("fracninttmatched",       &rec_fracninttmatched);
  m_ntTupleReco -> SetBranchAddress("fracntpcmatched",        &rec_fracntpcmatched);
  cout << "        Set input branches." << endl;

  // grab no. of entries
  const int64_t nTrueEntries = m_ntTupleTrue -> GetEntries();
  const int64_t nRecoEntries = m_ntTupleReco -> GetEntries(); 
  cout << "        Beginning truth particle loop: " << nTrueEntries << " to process" << endl;

  // loop over truth particles
  int64_t nTrueBytes = 0;
  for (int64_t iTrueEntry = 0; iTrueEntry < nTrueEntries; iTrueEntry++) {

    // grab truth particle entry
    const int64_t trueBytes = m_ntTupleTrue -> GetEntry(iTrueEntry);
    if (trueBytes < 0) {
      cerr << "PANIC: issue with entry " << iTrueEntry << "! Aborting loop!\n" << endl;
      break;
    } else {
      nTrueBytes += trueBytes;
    }

    const int64_t iTrueProg = iTrueEntry + 1;
    if (iTrueProg == nTrueEntries) {
      cout << "          Processing entry " << iTrueProg << "/" << nTrueEntries << "..." << endl;
    } else {
      cout << "          Processing entry " << iTrueProg << "/" << nTrueEntries << "...\r" << flush;
    }

    // run calculations
    const float tru_gnclust = tru_gnmvtxclust_trkmatcher + tru_gninttclust_trkmatcher + tru_gntpclust_trkmatcher;

    // fill truth 1D histograms
    m_vecTupleHists1D[Var::NTot][Type::Truth]   -> Fill(tru_gnclust);
    m_vecTupleHists1D[Var::NIntt][Type::Truth]  -> Fill(tru_gninttclust_trkmatcher);
    m_vecTupleHists1D[Var::NMvtx][Type::Truth]  -> Fill(tru_gnmvtxclust_trkmatcher);
    m_vecTupleHists1D[Var::NTpc][Type::Truth]   -> Fill(tru_gntpclust_trkmatcher);
    m_vecTupleHists1D[Var::RTot][Type::Truth]   -> Fill(1.);
    m_vecTupleHists1D[Var::RIntt][Type::Truth]  -> Fill(1.);
    m_vecTupleHists1D[Var::RMvtx][Type::Truth]  -> Fill(1.);
    m_vecTupleHists1D[Var::RTpc][Type::Truth]   -> Fill(1.);
    m_vecTupleHists1D[Var::Phi][Type::Truth]    -> Fill(tru_gphi);
    m_vecTupleHists1D[Var::Eta][Type::Truth]    -> Fill(tru_geta);
    m_vecTupleHists1D[Var::Pt][Type::Truth]     -> Fill(tru_gpt);
    m_vecTupleHists1D[Var::Frac][Type::Truth]   -> Fill(1.);
    m_vecTupleHists1D[Var::Qual][Type::Truth]   -> Fill(1.);
    m_vecTupleHists1D[Var::PtErr][Type::Truth]  -> Fill(1.);
    m_vecTupleHists1D[Var::EtaErr][Type::Truth] -> Fill(1.);
    m_vecTupleHists1D[Var::PhiErr][Type::Truth] -> Fill(1.);
    m_vecTupleHists1D[Var::PtRes][Type::Truth]  -> Fill(1.);
    m_vecTupleHists1D[Var::EtaRes][Type::Truth] -> Fill(1.);
    m_vecTupleHists1D[Var::PhiRes][Type::Truth] -> Fill(1.);

    // fill truth 2D histograms
    m_vecTupleHists2D[Var::NTot][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, tru_gnclust);
    m_vecTupleHists2D[Var::NIntt][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, tru_gninttclust_trkmatcher);
    m_vecTupleHists2D[Var::NMvtx][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, tru_gnmvtxclust_trkmatcher);
    m_vecTupleHists2D[Var::NTpc][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, tru_gntpcclust_manual);
    m_vecTupleHists2D[Var::RTot][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::RIntt][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::RMvtx][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::RTpc][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::Phi][Type::Truth][Comp::VsTruthPt]    -> Fill(tru_gpt, tru_gphi);
    m_vecTupleHists2D[Var::Eta][Type::Truth][Comp::VsTruthPt]    -> Fill(tru_gpt, tru_geta);
    m_vecTupleHists2D[Var::Pt][Type::Truth][Comp::VsTruthPt]     -> Fill(tru_gpt, tru_gpt);
    m_vecTupleHists2D[Var::Frac][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::Qual][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::PtErr][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::EtaErr][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::PhiErr][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::PtRes][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::EtaRes][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    m_vecTupleHists2D[Var::PhiRes][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);

    m_vecTupleHists2D[Var::NTot][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_ntpclust_trkmatcher, tru_gnclust);
    m_vecTupleHists2D[Var::NIntt][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, tru_gninttclust_trkmatcher);
    m_vecTupleHists2D[Var::NMvtx][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, tru_gnmvtxclust_trkmatcher);
    m_vecTupleHists2D[Var::NTpc][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_ntpclust_trkmatcher, tru_gntpcclust_manual);
    m_vecTupleHists2D[Var::RTot][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::RIntt][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::RMvtx][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::RTpc][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::Phi][Type::Truth][Comp::VsNumTpc]    -> Fill(tru_ntpclust_trkmatcher, tru_gphi);
    m_vecTupleHists2D[Var::Eta][Type::Truth][Comp::VsNumTpc]    -> Fill(tru_ntpclust_trkmatcher, tru_geta);
    m_vecTupleHists2D[Var::Pt][Type::Truth][Comp::VsNumTpc]     -> Fill(tru_ntpclust_trkmatcher, tru_gpt);
    m_vecTupleHists2D[Var::Frac][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::Qual][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::PtErr][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::EtaErr][Type::Truth][Comp::VsNumTpc] -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::PhiErr][Type::Truth][Comp::VsNumTpc] -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::PtRes][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::EtaRes][Type::Truth][Comp::VsNumTpc] -> Fill(tru_ntpclust_trkmatcher, 1.);
    m_vecTupleHists2D[Var::PhiRes][Type::Truth][Comp::VsNumTpc] -> Fill(tru_ntpclust_trkmatcher, 1.);
  }  // end truth particle loop

  // announce next entry loop
  cout << "        Finished truth particle loop.\n"
       << "        Beginning reconstructed track loop: " << nRecoEntries << " to process"
       << endl;

  // loop over reco tracks
  int64_t nRecoBytes = 0;
  for (int64_t iRecoEntry = 0; iRecoEntry < nRecoEntries; iRecoEntry++) {

    // grab reco track entry
    const int64_t recoBytes = m_ntTupleReco -> GetEntry(iRecoEntry);
    if (recoBytes < 0) {
      cerr << "PANIC: issue with entry " << iRecoEntry << "! Aborting loop!\n" << endl;
      break;
    } else {
      nRecoBytes += recoBytes;
    }

    const int64_t iRecoProg = iRecoEntry + 1;
    if (iRecoProg == nRecoEntries) {
      cout << "          Processing entry " << iRecoProg << "/" << nRecoEntries << "..." << endl;
    } else {
      cout << "          Processing entry " << iRecoProg << "/" << nRecoEntries << "...\r" << flush;
    }

    // run calculations
    //   - FIXME add other errors
    const double rec_nclus  = rec_ninttclust_trkmatcher + rec_nmvtxclust_trkmatcher + rec_ntpclust_trkmatcher;
    const double rec_gnclus = rec_gninttclust_trkmatcher + rec_gnmvtxclust_trkmatcher + rec_gntpcclust_manual;
    const double rec_rnclus = rec_nclus / rec_gnclus;
    const double rec_rintt  = rec_ninttclust_trkmatcher / rec_gninttclust_trkmatcher;
    const double rec_rmaps  = rec_nmvtxclust_trkmatcher / rec_gnmvtxclust_trkmatcher;
    const double rec_rtpc   = rec_ntpclust_trkmatcher / rec_gntpcclust_manual;
    const double rec_ptfrac = rec_pt / rec_gpt;
    const double rec_ptper  = rec_deltapt / rec_pt;
    const double rec_etaper = 1.;
    const double rec_phiper = 1.;
    const double rec_ptres  = abs(rec_pt - rec_gpt) / rec_gpt;
    const double rec_etares = abs(rec_eta - rec_geta) / rec_geta;
    const double rec_phires = abs(rec_phi - rec_gphi) / rec_gphi;

    // fill all matched reco 1D histograms
    m_vecTupleHists1D[Var::NTot][Type::Track]   -> Fill(rec_nclus);
    m_vecTupleHists1D[Var::NIntt][Type::Track]  -> Fill(rec_ninttclust_trkmatcher);
    m_vecTupleHists1D[Var::NMvtx][Type::Track]  -> Fill(rec_nmvtxclust_trkmatcher);
    m_vecTupleHists1D[Var::NTpc][Type::Track]   -> Fill(rec_ntpclust_trkmatcher);
    m_vecTupleHists1D[Var::RTot][Type::Track]   -> Fill(rec_rnclus);
    m_vecTupleHists1D[Var::RIntt][Type::Track]  -> Fill(rec_rintt);
    m_vecTupleHists1D[Var::RMvtx][Type::Track]  -> Fill(rec_rmaps);
    m_vecTupleHists1D[Var::RTpc][Type::Track]   -> Fill(rec_rtpc);
    m_vecTupleHists1D[Var::Phi][Type::Track]    -> Fill(rec_phi);
    m_vecTupleHists1D[Var::Eta][Type::Track]    -> Fill(rec_eta);
    m_vecTupleHists1D[Var::Pt][Type::Track]     -> Fill(rec_pt);
    m_vecTupleHists1D[Var::Frac][Type::Track]   -> Fill(rec_ptfrac);
    m_vecTupleHists1D[Var::Qual][Type::Track]   -> Fill(rec_quality);
    m_vecTupleHists1D[Var::PtErr][Type::Track]  -> Fill(rec_ptper);
    m_vecTupleHists1D[Var::EtaErr][Type::Track] -> Fill(rec_etaper);
    m_vecTupleHists1D[Var::PhiErr][Type::Track] -> Fill(rec_phiper);
    m_vecTupleHists1D[Var::PtRes][Type::Track]  -> Fill(rec_ptres);
    m_vecTupleHists1D[Var::EtaRes][Type::Track] -> Fill(rec_etares);
    m_vecTupleHists1D[Var::PhiRes][Type::Track] -> Fill(rec_phires);

    // fill all matched reco 2D histograms
    m_vecTupleHists2D[Var::NTot][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_nclus);
    m_vecTupleHists2D[Var::NIntt][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ninttclust_trkmatcher);
    m_vecTupleHists2D[Var::NMvtx][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nmvtxclust_trkmatcher);
    m_vecTupleHists2D[Var::NTpc][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ntpclust_trkmatcher);
    m_vecTupleHists2D[Var::RTot][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rnclus);
    m_vecTupleHists2D[Var::RIntt][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rintt);
    m_vecTupleHists2D[Var::RMvtx][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rmaps);
    m_vecTupleHists2D[Var::RTpc][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rtpc);
    m_vecTupleHists2D[Var::Phi][Type::Track][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_phi);
    m_vecTupleHists2D[Var::Eta][Type::Track][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_eta);
    m_vecTupleHists2D[Var::Pt][Type::Track][Comp::VsTruthPt]     -> Fill(rec_gpt, rec_pt);
    m_vecTupleHists2D[Var::Frac][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ptfrac);
    m_vecTupleHists2D[Var::Qual][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_quality);
    m_vecTupleHists2D[Var::PtErr][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptper);
    m_vecTupleHists2D[Var::EtaErr][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etaper);
    m_vecTupleHists2D[Var::PhiErr][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phiper);
    m_vecTupleHists2D[Var::PtRes][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptres);
    m_vecTupleHists2D[Var::EtaRes][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etares);
    m_vecTupleHists2D[Var::PhiRes][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phires);

    m_vecTupleHists2D[Var::NTot][Type::Track][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_nclus);
    m_vecTupleHists2D[Var::NIntt][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ninttclust_trkmatcher);
    m_vecTupleHists2D[Var::NMvtx][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_nmvtxclust_trkmatcher);
    m_vecTupleHists2D[Var::NTpc][Type::Track][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_ntpclust_trkmatcher);
    m_vecTupleHists2D[Var::RTot][Type::Track][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_rnclus);
    m_vecTupleHists2D[Var::RIntt][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rintt);
    m_vecTupleHists2D[Var::RMvtx][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rmaps);
    m_vecTupleHists2D[Var::RTpc][Type::Track][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_rtpc);
    m_vecTupleHists2D[Var::Phi][Type::Track][Comp::VsNumTpc]    -> Fill(rec_ntpclust_trkmatcher, rec_phi);
    m_vecTupleHists2D[Var::Eta][Type::Track][Comp::VsNumTpc]    -> Fill(rec_ntpclust_trkmatcher, rec_eta);
    m_vecTupleHists2D[Var::Pt][Type::Track][Comp::VsNumTpc]     -> Fill(rec_ntpclust_trkmatcher, rec_pt);
    m_vecTupleHists2D[Var::Frac][Type::Track][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_ptfrac);
    m_vecTupleHists2D[Var::Qual][Type::Track][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_quality);
    m_vecTupleHists2D[Var::PtErr][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ptper);
    m_vecTupleHists2D[Var::EtaErr][Type::Track][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_etaper);
    m_vecTupleHists2D[Var::PhiErr][Type::Track][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_phiper);
    m_vecTupleHists2D[Var::PtRes][Type::Track][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ptres);
    m_vecTupleHists2D[Var::EtaRes][Type::Track][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_etares);
    m_vecTupleHists2D[Var::PhiRes][Type::Track][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_phires);

    // fill weird and normal matched reco 1D histograms
    const bool isNormalTrack = ((rec_ptfrac >= m_config.oddPtFrac.first) && (rec_ptfrac <= m_config.oddPtFrac.second));
    if (isNormalTrack) {
      m_vecTupleHists1D[Var::NTot][Type::Normal]   -> Fill(rec_nclus);
      m_vecTupleHists1D[Var::NIntt][Type::Normal]  -> Fill(rec_ninttclust_trkmatcher);
      m_vecTupleHists1D[Var::NMvtx][Type::Normal]  -> Fill(rec_nmvtxclust_trkmatcher);
      m_vecTupleHists1D[Var::NTpc][Type::Normal]   -> Fill(rec_ntpclust_trkmatcher);
      m_vecTupleHists1D[Var::RTot][Type::Normal]   -> Fill(rec_rnclus);
      m_vecTupleHists1D[Var::RIntt][Type::Normal]  -> Fill(rec_rintt);
      m_vecTupleHists1D[Var::RMvtx][Type::Normal]  -> Fill(rec_rmaps);
      m_vecTupleHists1D[Var::RTpc][Type::Normal]   -> Fill(rec_rtpc);
      m_vecTupleHists1D[Var::Phi][Type::Normal]    -> Fill(rec_phi);
      m_vecTupleHists1D[Var::Eta][Type::Normal]    -> Fill(rec_eta);
      m_vecTupleHists1D[Var::Pt][Type::Normal]     -> Fill(rec_pt);
      m_vecTupleHists1D[Var::Frac][Type::Normal]   -> Fill(rec_ptfrac);
      m_vecTupleHists1D[Var::Qual][Type::Normal]   -> Fill(rec_quality);
      m_vecTupleHists1D[Var::PtErr][Type::Normal]  -> Fill(rec_ptper);
      m_vecTupleHists1D[Var::EtaErr][Type::Normal] -> Fill(rec_etaper);
      m_vecTupleHists1D[Var::PhiErr][Type::Normal] -> Fill(rec_phiper);
      m_vecTupleHists1D[Var::PtRes][Type::Normal]  -> Fill(rec_ptres);
      m_vecTupleHists1D[Var::EtaRes][Type::Normal] -> Fill(rec_etares);
      m_vecTupleHists1D[Var::PhiRes][Type::Normal] -> Fill(rec_phires);

      m_vecTupleHists2D[Var::NTot][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_nclus);
      m_vecTupleHists2D[Var::NIntt][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ninttclust_trkmatcher);
      m_vecTupleHists2D[Var::NMvtx][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nmvtxclust_trkmatcher);
      m_vecTupleHists2D[Var::NTpc][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ntpclust_trkmatcher);
      m_vecTupleHists2D[Var::RTot][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rnclus);
      m_vecTupleHists2D[Var::RIntt][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rintt);
      m_vecTupleHists2D[Var::RMvtx][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rmaps);
      m_vecTupleHists2D[Var::RTpc][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rtpc);
      m_vecTupleHists2D[Var::Phi][Type::Normal][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_phi);
      m_vecTupleHists2D[Var::Eta][Type::Normal][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_eta);
      m_vecTupleHists2D[Var::Pt][Type::Normal][Comp::VsTruthPt]     -> Fill(rec_gpt, rec_pt);
      m_vecTupleHists2D[Var::Frac][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ptfrac);
      m_vecTupleHists2D[Var::Qual][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_quality);
      m_vecTupleHists2D[Var::PtErr][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptper);
      m_vecTupleHists2D[Var::EtaErr][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etaper);
      m_vecTupleHists2D[Var::PhiErr][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phiper);
      m_vecTupleHists2D[Var::PtRes][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptres);
      m_vecTupleHists2D[Var::EtaRes][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etares);
      m_vecTupleHists2D[Var::PhiRes][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phires);

      m_vecTupleHists2D[Var::NTot][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_nclus);
      m_vecTupleHists2D[Var::NIntt][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ninttclust_trkmatcher);
      m_vecTupleHists2D[Var::NMvtx][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_nmvtxclust_trkmatcher);
      m_vecTupleHists2D[Var::NTpc][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_ntpclust_trkmatcher);
      m_vecTupleHists2D[Var::RTot][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_rnclus);
      m_vecTupleHists2D[Var::RIntt][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rintt);
      m_vecTupleHists2D[Var::RMvtx][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rmaps);
      m_vecTupleHists2D[Var::RTpc][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_rtpc);
      m_vecTupleHists2D[Var::Phi][Type::Normal][Comp::VsNumTpc]    -> Fill(rec_ntpclust_trkmatcher, rec_phi);
      m_vecTupleHists2D[Var::Eta][Type::Normal][Comp::VsNumTpc]    -> Fill(rec_ntpclust_trkmatcher, rec_eta);
      m_vecTupleHists2D[Var::Pt][Type::Normal][Comp::VsNumTpc]     -> Fill(rec_ntpclust_trkmatcher, rec_pt);
      m_vecTupleHists2D[Var::Frac][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_ptfrac);
      m_vecTupleHists2D[Var::Qual][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_quality);
      m_vecTupleHists2D[Var::PtErr][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ptper);
      m_vecTupleHists2D[Var::EtaErr][Type::Normal][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_etaper);
      m_vecTupleHists2D[Var::PhiErr][Type::Normal][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_phiper);
      m_vecTupleHists2D[Var::PtRes][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ptres);
      m_vecTupleHists2D[Var::EtaRes][Type::Normal][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_etares);
      m_vecTupleHists2D[Var::PhiRes][Type::Normal][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_phires);
    } else {
      m_vecTupleHists1D[Var::NTot][Type::Weird]   -> Fill(rec_nclus);
      m_vecTupleHists1D[Var::NIntt][Type::Weird]  -> Fill(rec_ninttclust_trkmatcher);
      m_vecTupleHists1D[Var::NMvtx][Type::Weird]  -> Fill(rec_nmvtxclust_trkmatcher);
      m_vecTupleHists1D[Var::NTpc][Type::Weird]   -> Fill(rec_ntpclust_trkmatcher);
      m_vecTupleHists1D[Var::RTot][Type::Weird]   -> Fill(rec_rnclus);
      m_vecTupleHists1D[Var::RIntt][Type::Weird]  -> Fill(rec_rintt);
      m_vecTupleHists1D[Var::RMvtx][Type::Weird]  -> Fill(rec_rmaps);
      m_vecTupleHists1D[Var::RTpc][Type::Weird]   -> Fill(rec_rtpc);
      m_vecTupleHists1D[Var::Phi][Type::Weird]    -> Fill(rec_phi);
      m_vecTupleHists1D[Var::Eta][Type::Weird]    -> Fill(rec_eta);
      m_vecTupleHists1D[Var::Pt][Type::Weird]     -> Fill(rec_pt);
      m_vecTupleHists1D[Var::Frac][Type::Weird]   -> Fill(rec_ptfrac);
      m_vecTupleHists1D[Var::Qual][Type::Weird]   -> Fill(rec_quality);
      m_vecTupleHists1D[Var::PtErr][Type::Weird]  -> Fill(rec_ptper);
      m_vecTupleHists1D[Var::EtaErr][Type::Weird] -> Fill(rec_etaper);
      m_vecTupleHists1D[Var::PhiErr][Type::Weird] -> Fill(rec_phiper);
      m_vecTupleHists1D[Var::PtRes][Type::Weird]  -> Fill(rec_ptres);
      m_vecTupleHists1D[Var::EtaRes][Type::Weird] -> Fill(rec_etares);
      m_vecTupleHists1D[Var::PhiRes][Type::Weird] -> Fill(rec_phires);

      m_vecTupleHists2D[Var::NTot][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_nclus);
      m_vecTupleHists2D[Var::NIntt][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ninttclust_trkmatcher);
      m_vecTupleHists2D[Var::NMvtx][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nmvtxclust_trkmatcher);
      m_vecTupleHists2D[Var::NTpc][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ntpclust_trkmatcher);
      m_vecTupleHists2D[Var::RTot][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rnclus);
      m_vecTupleHists2D[Var::RIntt][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rintt);
      m_vecTupleHists2D[Var::RMvtx][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rmaps);
      m_vecTupleHists2D[Var::RTpc][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rtpc);
      m_vecTupleHists2D[Var::Phi][Type::Weird][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_phi);
      m_vecTupleHists2D[Var::Eta][Type::Weird][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_eta);
      m_vecTupleHists2D[Var::Pt][Type::Weird][Comp::VsTruthPt]     -> Fill(rec_gpt, rec_pt);
      m_vecTupleHists2D[Var::Frac][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ptfrac);
      m_vecTupleHists2D[Var::Qual][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_quality);
      m_vecTupleHists2D[Var::PtErr][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptper);
      m_vecTupleHists2D[Var::EtaErr][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etaper);
      m_vecTupleHists2D[Var::PhiErr][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phiper);
      m_vecTupleHists2D[Var::PtRes][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptres);
      m_vecTupleHists2D[Var::EtaRes][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etares);
      m_vecTupleHists2D[Var::PhiRes][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phires);

      m_vecTupleHists2D[Var::NTot][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_nclus);
      m_vecTupleHists2D[Var::NIntt][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ninttclust_trkmatcher);
      m_vecTupleHists2D[Var::NMvtx][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_nmvtxclust_trkmatcher);
      m_vecTupleHists2D[Var::NTpc][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_ntpclust_trkmatcher);
      m_vecTupleHists2D[Var::RTot][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_rnclus);
      m_vecTupleHists2D[Var::RIntt][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rintt);
      m_vecTupleHists2D[Var::RMvtx][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_rmaps);
      m_vecTupleHists2D[Var::RTpc][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_rtpc);
      m_vecTupleHists2D[Var::Phi][Type::Weird][Comp::VsNumTpc]    -> Fill(rec_ntpclust_trkmatcher, rec_phi);
      m_vecTupleHists2D[Var::Eta][Type::Weird][Comp::VsNumTpc]    -> Fill(rec_ntpclust_trkmatcher, rec_eta);
      m_vecTupleHists2D[Var::Pt][Type::Weird][Comp::VsNumTpc]     -> Fill(rec_ntpclust_trkmatcher, rec_pt);
      m_vecTupleHists2D[Var::Frac][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_ptfrac);
      m_vecTupleHists2D[Var::Qual][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_ntpclust_trkmatcher, rec_quality);
      m_vecTupleHists2D[Var::PtErr][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ptper);
      m_vecTupleHists2D[Var::EtaErr][Type::Weird][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_etaper);
      m_vecTupleHists2D[Var::PhiErr][Type::Weird][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_phiper);
      m_vecTupleHists2D[Var::PtRes][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_ntpclust_trkmatcher, rec_ptres);
      m_vecTupleHists2D[Var::EtaRes][Type::Weird][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_etares);
      m_vecTupleHists2D[Var::PhiRes][Type::Weird][Comp::VsNumTpc] -> Fill(rec_ntpclust_trkmatcher, rec_phires);
    }
  }  // end reco track loop

  // announce method end and return
  cout << "        Finished reconstructed track loop.\n"
       << "      Finished getting histograms from new matcher track tuple."
       << endl;
  return;

}  // end 'GetNewTupleHists()'



void STrackMatcherComparator::GetOldTupleHists() {

  // announce start of method
  cout << "      Grabbing old matcher tuple histograms:" << endl;

  // declare input leaves
  float tru_event;
  float tru_seed;
  float tru_gntracks;
  float tru_gnchghad;
  float tru_gtrackID;
  float tru_gflavor;
  float tru_gnhits;
  float tru_gnmaps;
  float tru_gnintt;
  float tru_gnmms;
  float tru_gnintt1;
  float tru_gnintt2;
  float tru_gnintt3;
  float tru_gnintt4;
  float tru_gnintt5;
  float tru_gnintt6;
  float tru_gnintt7;
  float tru_gnintt8;
  float tru_gntpc;
  float tru_gnlmaps;
  float tru_gnlintt;
  float tru_gnltpc;
  float tru_gnlmms;
  float tru_gpx;
  float tru_gpy;
  float tru_gpz;
  float tru_gpt;
  float tru_geta;
  float tru_gphi;
  float tru_gvx;
  float tru_gvy;
  float tru_gvz;
  float tru_gvt;
  float tru_gfpx;
  float tru_gfpy;
  float tru_gfpz;
  float tru_gfx;
  float tru_gfy;
  float tru_gfz;
  float tru_gembed;
  float tru_gprimary;
  float tru_trackID;
  float tru_px;
  float tru_py;
  float tru_pz;
  float tru_pt;
  float tru_eta;
  float tru_phi;
  float tru_deltapt;
  float tru_deltaeta;
  float tru_deltaphi;
  float tru_siqr;
  float tru_siphi;
  float tru_sithe;
  float tru_six0;
  float tru_siy0;
  float tru_tpqr;
  float tru_tpphi;
  float tru_tpthe;
  float tru_tpx0;
  float tru_tpy0;
  float tru_charge;
  float tru_quality;
  float tru_chisq;
  float tru_ndf;
  float tru_nhits;
  float tru_layers;
  float tru_nmaps;
  float tru_nintt;
  float tru_ntpc;
  float tru_nmms;
  float tru_ntpc1;
  float tru_ntpc11;
  float tru_ntpc2;
  float tru_ntpc3;
  float tru_nlmaps;
  float tru_nlintt;
  float tru_nltpc;
  float tru_nlmms;
  float tru_vertexID;
  float tru_vx;
  float tru_vy;
  float tru_vz;
  float tru_dca2d;
  float tru_dca2dsigma;
  float tru_dca3dxy;
  float tru_dca3dxysigma;
  float tru_dca3dz;
  float tru_dca3dzsigma;
  float tru_pcax;
  float tru_pcay;
  float tru_pcaz;
  float tru_nfromtruth;
  float tru_nwrong;
  float tru_ntrumaps;
  float tru_nwrongmaps;
  float tru_ntruintt;
  float tru_nwrongintt;
  float tru_ntrutpc;
  float tru_nwrongtpc;
  float tru_ntrumms;
  float tru_nwrongmms;
  float tru_ntrutpc1;
  float tru_nwrongtpc1;
  float tru_ntrutpc11;
  float tru_nwrongtpc11;
  float tru_ntrutpc2;
  float tru_nwrongtpc2;
  float tru_ntrutpc3;
  float tru_nwrongtpc3;
  float tru_layersfromtruth;
  float tru_npedge;
  float tru_nredge;
  float tru_nbig;
  float tru_novlp;
  float tru_merr;
  float tru_msize;
  float tru_nhittpcall;
  float tru_nhittpcin;
  float tru_nhittpcmid;
  float tru_nhittpcout;
  float tru_nclusall;
  float tru_nclustpc;
  float tru_nclusintt;
  float tru_nclusmaps;
  float tru_nclusmms;

  float rec_event;
  float rec_seed;
  float rec_trackID;
  float rec_crossing;
  float rec_px;
  float rec_py;
  float rec_pz;
  float rec_pt;
  float rec_eta;
  float rec_phi;
  float rec_deltapt;
  float rec_deltaeta;
  float rec_deltaphi;
  float rec_siqr;
  float rec_siphi;
  float rec_sithe;
  float rec_six0;
  float rec_siy0;
  float rec_tpqr;
  float rec_tpphi;
  float rec_tpthe;
  float rec_tpx0;
  float rec_tpy0;
  float rec_charge;
  float rec_quality;
  float rec_chisq;
  float rec_ndf;
  float rec_nhits;
  float rec_nmaps;
  float rec_nintt;
  float rec_ntpc;
  float rec_nmms;
  float rec_ntpc1;
  float rec_ntpc11;
  float rec_ntpc2;
  float rec_ntpc3;
  float rec_nlmaps;
  float rec_nlintt;
  float rec_nltpc;
  float rec_nlmms;
  float rec_layers;
  float rec_vertexID;
  float rec_vx;
  float rec_vy;
  float rec_vz;
  float rec_dca2d;
  float rec_dca2dsigma;
  float rec_dca3dxy;
  float rec_dca3dxysigma;
  float rec_dca3dz;
  float rec_dca3dzsigma;
  float rec_pcax;
  float rec_pcay;
  float rec_pcaz;
  float rec_gtrackID;
  float rec_gflavor;
  float rec_gnhits;
  float rec_gnmaps;
  float rec_gnintt;
  float rec_gntpc;
  float rec_gnmms;
  float rec_gnlmaps;
  float rec_gnlintt;
  float rec_gnltpc;
  float rec_gnlmms;
  float rec_gpx;
  float rec_gpy;
  float rec_gpz;
  float rec_gpt;
  float rec_geta;
  float rec_gphi;
  float rec_gvx;
  float rec_gvy;
  float rec_gvz;
  float rec_gvt;
  float rec_gfpx;
  float rec_gfpy;
  float rec_gfpz;
  float rec_gfx;
  float rec_gfy;
  float rec_gfz;
  float rec_gembed;
  float rec_gprimary;
  float rec_nfromtruth;
  float rec_nwrong;
  float rec_ntrumaps;
  float rec_nwrongmaps;
  float rec_ntruintt;
  float rec_nwrongintt;
  float rec_ntrutpc;
  float rec_nwrongtpc;
  float rec_ntrumms;
  float rec_nwrongmms;
  float rec_ntrutpc1;
  float rec_nwrongtpc1;
  float rec_ntrutpc11;
  float rec_nwrongtpc11;
  float rec_ntrutpc2;
  float rec_nwrongtpc2;
  float rec_ntrutpc3;
  float rec_nwrongtpc3;
  float rec_layersfromtruth;
  float rec_npedge;
  float rec_nredge;
  float rec_nbig;
  float rec_novlp;
  float rec_merr;
  float rec_msize;
  float rec_nhittpcall;
  float rec_nhittpcin;
  float rec_nhittpcmid;
  float rec_nhittpcout;
  float rec_nclusall;
  float rec_nclustpc;
  float rec_nclusintt;
  float rec_nclusmaps;
  float rec_nclusmms;

  // Set branch addresses.
  m_ntOldTrue -> SetBranchAddress("event",           &tru_event);
  m_ntOldTrue -> SetBranchAddress("seed",            &tru_seed);
  m_ntOldTrue -> SetBranchAddress("gntracks",        &tru_gntracks);
  m_ntOldTrue -> SetBranchAddress("gnchghad",        &tru_gnchghad);
  m_ntOldTrue -> SetBranchAddress("gtrackID",        &tru_gtrackID);
  m_ntOldTrue -> SetBranchAddress("gflavor",         &tru_gflavor);
  m_ntOldTrue -> SetBranchAddress("gnhits",          &tru_gnhits);
  m_ntOldTrue -> SetBranchAddress("gnmaps",          &tru_gnmaps);
  m_ntOldTrue -> SetBranchAddress("gnintt",          &tru_gnintt);
  m_ntOldTrue -> SetBranchAddress("gnmms",           &tru_gnmms);
  m_ntOldTrue -> SetBranchAddress("gnintt1",         &tru_gnintt1);
  m_ntOldTrue -> SetBranchAddress("gnintt2",         &tru_gnintt2);
  m_ntOldTrue -> SetBranchAddress("gnintt3",         &tru_gnintt3);
  m_ntOldTrue -> SetBranchAddress("gnintt4",         &tru_gnintt4);
  m_ntOldTrue -> SetBranchAddress("gnintt5",         &tru_gnintt5);
  m_ntOldTrue -> SetBranchAddress("gnintt6",         &tru_gnintt6);
  m_ntOldTrue -> SetBranchAddress("gnintt7",         &tru_gnintt7);
  m_ntOldTrue -> SetBranchAddress("gnintt8",         &tru_gnintt8);
  m_ntOldTrue -> SetBranchAddress("gntpc",           &tru_gntpc);
  m_ntOldTrue -> SetBranchAddress("gnlmaps",         &tru_gnlmaps);
  m_ntOldTrue -> SetBranchAddress("gnlintt",         &tru_gnlintt);
  m_ntOldTrue -> SetBranchAddress("gnltpc",          &tru_gnltpc);
  m_ntOldTrue -> SetBranchAddress("gnlmms",          &tru_gnlmms);
  m_ntOldTrue -> SetBranchAddress("gpx",             &tru_gpx);
  m_ntOldTrue -> SetBranchAddress("gpy",             &tru_gpy);
  m_ntOldTrue -> SetBranchAddress("gpz",             &tru_gpz);
  m_ntOldTrue -> SetBranchAddress("gpt",             &tru_gpt);
  m_ntOldTrue -> SetBranchAddress("geta",            &tru_geta);
  m_ntOldTrue -> SetBranchAddress("gphi",            &tru_gphi);
  m_ntOldTrue -> SetBranchAddress("gvx",             &tru_gvx);
  m_ntOldTrue -> SetBranchAddress("gvy",             &tru_gvy);
  m_ntOldTrue -> SetBranchAddress("gvz",             &tru_gvz);
  m_ntOldTrue -> SetBranchAddress("gvt",             &tru_gvt);
  m_ntOldTrue -> SetBranchAddress("gfpx",            &tru_gfpx);
  m_ntOldTrue -> SetBranchAddress("gfpy",            &tru_gfpy);
  m_ntOldTrue -> SetBranchAddress("gfpz",            &tru_gfpz);
  m_ntOldTrue -> SetBranchAddress("gfx",             &tru_gfx);
  m_ntOldTrue -> SetBranchAddress("gfy",             &tru_gfy);
  m_ntOldTrue -> SetBranchAddress("gfz",             &tru_gfz);
  m_ntOldTrue -> SetBranchAddress("gembed",          &tru_gembed);
  m_ntOldTrue -> SetBranchAddress("gprimary",        &tru_gprimary);
  m_ntOldTrue -> SetBranchAddress("trackID",         &tru_trackID);
  m_ntOldTrue -> SetBranchAddress("px",              &tru_px);
  m_ntOldTrue -> SetBranchAddress("py",              &tru_py);
  m_ntOldTrue -> SetBranchAddress("pz",              &tru_pz);
  m_ntOldTrue -> SetBranchAddress("pt",              &tru_pt);
  m_ntOldTrue -> SetBranchAddress("eta",             &tru_eta);
  m_ntOldTrue -> SetBranchAddress("phi",             &tru_phi);
  m_ntOldTrue -> SetBranchAddress("deltapt",         &tru_deltapt);
  m_ntOldTrue -> SetBranchAddress("deltaeta",        &tru_deltaeta);
  m_ntOldTrue -> SetBranchAddress("deltaphi",        &tru_deltaphi);
  m_ntOldTrue -> SetBranchAddress("siqr",            &tru_siqr);
  m_ntOldTrue -> SetBranchAddress("siphi",           &tru_siphi);
  m_ntOldTrue -> SetBranchAddress("sithe",           &tru_sithe);
  m_ntOldTrue -> SetBranchAddress("six0",            &tru_six0);
  m_ntOldTrue -> SetBranchAddress("siy0",            &tru_siy0);
  m_ntOldTrue -> SetBranchAddress("tpqr",            &tru_tpqr);
  m_ntOldTrue -> SetBranchAddress("tpphi",           &tru_tpphi);
  m_ntOldTrue -> SetBranchAddress("tpthe",           &tru_tpthe);
  m_ntOldTrue -> SetBranchAddress("tpx0",            &tru_tpx0);
  m_ntOldTrue -> SetBranchAddress("tpy0",            &tru_tpy0);
  m_ntOldTrue -> SetBranchAddress("charge",          &tru_charge);
  m_ntOldTrue -> SetBranchAddress("quality",         &tru_quality);
  m_ntOldTrue -> SetBranchAddress("chisq",           &tru_chisq);
  m_ntOldTrue -> SetBranchAddress("ndf",             &tru_ndf);
  m_ntOldTrue -> SetBranchAddress("nhits",           &tru_nhits);
  m_ntOldTrue -> SetBranchAddress("layers",          &tru_layers);
  m_ntOldTrue -> SetBranchAddress("nmaps",           &tru_nmaps);
  m_ntOldTrue -> SetBranchAddress("nintt",           &tru_nintt);
  m_ntOldTrue -> SetBranchAddress("ntpc",            &tru_ntpc);
  m_ntOldTrue -> SetBranchAddress("nmms",            &tru_nmms);
  m_ntOldTrue -> SetBranchAddress("ntpc1",           &tru_ntpc1);
  m_ntOldTrue -> SetBranchAddress("ntpc11",          &tru_ntpc11);
  m_ntOldTrue -> SetBranchAddress("ntpc2",           &tru_ntpc2);
  m_ntOldTrue -> SetBranchAddress("ntpc3",           &tru_ntpc3);
  m_ntOldTrue -> SetBranchAddress("nlmaps",          &tru_nlmaps);
  m_ntOldTrue -> SetBranchAddress("nlintt",          &tru_nlintt);
  m_ntOldTrue -> SetBranchAddress("nltpc",           &tru_nltpc);
  m_ntOldTrue -> SetBranchAddress("nlmms",           &tru_nlmms);
  m_ntOldTrue -> SetBranchAddress("vertexID",        &tru_vertexID);
  m_ntOldTrue -> SetBranchAddress("vx",              &tru_vx);
  m_ntOldTrue -> SetBranchAddress("vy",              &tru_vy);
  m_ntOldTrue -> SetBranchAddress("vz",              &tru_vz);
  m_ntOldTrue -> SetBranchAddress("dca2d",           &tru_dca2d);
  m_ntOldTrue -> SetBranchAddress("dca2dsigma",      &tru_dca2dsigma);
  m_ntOldTrue -> SetBranchAddress("dca3dxy",         &tru_dca3dxy);
  m_ntOldTrue -> SetBranchAddress("dca3dxysigma",    &tru_dca3dxysigma);
  m_ntOldTrue -> SetBranchAddress("dca3dz",          &tru_dca3dz);
  m_ntOldTrue -> SetBranchAddress("dca3dzsigma",     &tru_dca3dzsigma);
  m_ntOldTrue -> SetBranchAddress("pcax",            &tru_pcax);
  m_ntOldTrue -> SetBranchAddress("pcay",            &tru_pcay);
  m_ntOldTrue -> SetBranchAddress("pcaz",            &tru_pcaz);
  m_ntOldTrue -> SetBranchAddress("nfromtruth",      &tru_nfromtruth);
  m_ntOldTrue -> SetBranchAddress("nwrong",          &tru_nwrong);
  m_ntOldTrue -> SetBranchAddress("ntrumaps",        &tru_ntrumaps);
  m_ntOldTrue -> SetBranchAddress("nwrongmaps",      &tru_nwrongmaps);
  m_ntOldTrue -> SetBranchAddress("ntruintt",        &tru_ntruintt);
  m_ntOldTrue -> SetBranchAddress("nwrongintt",      &tru_nwrongintt);
  m_ntOldTrue -> SetBranchAddress("ntrutpc",         &tru_ntrutpc);
  m_ntOldTrue -> SetBranchAddress("nwrongtpc",       &tru_nwrongtpc);
  m_ntOldTrue -> SetBranchAddress("ntrumms",         &tru_ntrumms);
  m_ntOldTrue -> SetBranchAddress("nwrongmms",       &tru_nwrongmms);
  m_ntOldTrue -> SetBranchAddress("ntrutpc1",        &tru_ntrutpc1);
  m_ntOldTrue -> SetBranchAddress("nwrongtpc1",      &tru_nwrongtpc1);
  m_ntOldTrue -> SetBranchAddress("ntrutpc11",       &tru_ntrutpc11);
  m_ntOldTrue -> SetBranchAddress("nwrongtpc11",     &tru_nwrongtpc11);
  m_ntOldTrue -> SetBranchAddress("ntrutpc2",        &tru_ntrutpc2);
  m_ntOldTrue -> SetBranchAddress("nwrongtpc2",      &tru_nwrongtpc2);
  m_ntOldTrue -> SetBranchAddress("ntrutpc3",        &tru_ntrutpc3);
  m_ntOldTrue -> SetBranchAddress("nwrongtpc3",      &tru_nwrongtpc3);
  m_ntOldTrue -> SetBranchAddress("layersfromtruth", &tru_layersfromtruth);
  m_ntOldTrue -> SetBranchAddress("npedge",          &tru_npedge);
  m_ntOldTrue -> SetBranchAddress("nredge",          &tru_nredge);
  m_ntOldTrue -> SetBranchAddress("nbig",            &tru_nbig);
  m_ntOldTrue -> SetBranchAddress("novlp",           &tru_novlp);
  m_ntOldTrue -> SetBranchAddress("merr",            &tru_merr);
  m_ntOldTrue -> SetBranchAddress("msize",           &tru_msize);
  m_ntOldTrue -> SetBranchAddress("nhittpcall",      &tru_nhittpcall);
  m_ntOldTrue -> SetBranchAddress("nhittpcin",       &tru_nhittpcin);
  m_ntOldTrue -> SetBranchAddress("nhittpcmid",      &tru_nhittpcmid);
  m_ntOldTrue -> SetBranchAddress("nhittpcout",      &tru_nhittpcout);
  m_ntOldTrue -> SetBranchAddress("nclusall",        &tru_nclusall);
  m_ntOldTrue -> SetBranchAddress("nclustpc",        &tru_nclustpc);
  m_ntOldTrue -> SetBranchAddress("nclusintt",       &tru_nclusintt);
  m_ntOldTrue -> SetBranchAddress("nclusmaps",       &tru_nclusmaps);
  m_ntOldTrue -> SetBranchAddress("nclusmms",        &tru_nclusmms);

  m_ntOldReco -> SetBranchAddress("event",           &rec_event);
  m_ntOldReco -> SetBranchAddress("seed",            &rec_seed);
  m_ntOldReco -> SetBranchAddress("trackID",         &rec_trackID);
  m_ntOldReco -> SetBranchAddress("crossing",        &rec_crossing);
  m_ntOldReco -> SetBranchAddress("px",              &rec_px);
  m_ntOldReco -> SetBranchAddress("py",              &rec_py);
  m_ntOldReco -> SetBranchAddress("pz",              &rec_pz);
  m_ntOldReco -> SetBranchAddress("pt",              &rec_pt);
  m_ntOldReco -> SetBranchAddress("eta",             &rec_eta);
  m_ntOldReco -> SetBranchAddress("phi",             &rec_phi);
  m_ntOldReco -> SetBranchAddress("deltapt",         &rec_deltapt);
  m_ntOldReco -> SetBranchAddress("deltaeta",        &rec_deltaeta);
  m_ntOldReco -> SetBranchAddress("deltaphi",        &rec_deltaphi);
  m_ntOldReco -> SetBranchAddress("siqr",            &rec_siqr);
  m_ntOldReco -> SetBranchAddress("siphi",           &rec_siphi);
  m_ntOldReco -> SetBranchAddress("sithe",           &rec_sithe);
  m_ntOldReco -> SetBranchAddress("six0",            &rec_six0);
  m_ntOldReco -> SetBranchAddress("siy0",            &rec_siy0);
  m_ntOldReco -> SetBranchAddress("tpqr",            &rec_tpqr);
  m_ntOldReco -> SetBranchAddress("tpphi",           &rec_tpphi);
  m_ntOldReco -> SetBranchAddress("tpthe",           &rec_tpthe);
  m_ntOldReco -> SetBranchAddress("tpx0",            &rec_tpx0);
  m_ntOldReco -> SetBranchAddress("tpy0",            &rec_tpy0);
  m_ntOldReco -> SetBranchAddress("charge",          &rec_charge);
  m_ntOldReco -> SetBranchAddress("quality",         &rec_quality);
  m_ntOldReco -> SetBranchAddress("chisq",           &rec_chisq);
  m_ntOldReco -> SetBranchAddress("ndf",             &rec_ndf);
  m_ntOldReco -> SetBranchAddress("nhits",           &rec_nhits);
  m_ntOldReco -> SetBranchAddress("nmaps",           &rec_nmaps);
  m_ntOldReco -> SetBranchAddress("nintt",           &rec_nintt);
  m_ntOldReco -> SetBranchAddress("ntpc",            &rec_ntpc);
  m_ntOldReco -> SetBranchAddress("nmms",            &rec_nmms);
  m_ntOldReco -> SetBranchAddress("ntpc1",           &rec_ntpc1);
  m_ntOldReco -> SetBranchAddress("ntpc11",          &rec_ntpc11);
  m_ntOldReco -> SetBranchAddress("ntpc2",           &rec_ntpc2);
  m_ntOldReco -> SetBranchAddress("ntpc3",           &rec_ntpc3);
  m_ntOldReco -> SetBranchAddress("nlmaps",          &rec_nlmaps);
  m_ntOldReco -> SetBranchAddress("nlintt",          &rec_nlintt);
  m_ntOldReco -> SetBranchAddress("nltpc",           &rec_nltpc);
  m_ntOldReco -> SetBranchAddress("nlmms",           &rec_nlmms);
  m_ntOldReco -> SetBranchAddress("layers",          &rec_layers);
  m_ntOldReco -> SetBranchAddress("vertexID",        &rec_vertexID);
  m_ntOldReco -> SetBranchAddress("vx",              &rec_vx);
  m_ntOldReco -> SetBranchAddress("vy",              &rec_vy);
  m_ntOldReco -> SetBranchAddress("vz",              &rec_vz);
  m_ntOldReco -> SetBranchAddress("dca2d",           &rec_dca2d);
  m_ntOldReco -> SetBranchAddress("dca2dsigma",      &rec_dca2dsigma);
  m_ntOldReco -> SetBranchAddress("dca3dxy",         &rec_dca3dxy);
  m_ntOldReco -> SetBranchAddress("dca3dxysigma",    &rec_dca3dxysigma);
  m_ntOldReco -> SetBranchAddress("dca3dz",          &rec_dca3dz);
  m_ntOldReco -> SetBranchAddress("dca3dzsigma",     &rec_dca3dzsigma);
  m_ntOldReco -> SetBranchAddress("pcax",            &rec_pcax);
  m_ntOldReco -> SetBranchAddress("pcay",            &rec_pcay);
  m_ntOldReco -> SetBranchAddress("pcaz",            &rec_pcaz);
  m_ntOldReco -> SetBranchAddress("gtrackID",        &rec_gtrackID);
  m_ntOldReco -> SetBranchAddress("gflavor",         &rec_gflavor);
  m_ntOldReco -> SetBranchAddress("gnhits",          &rec_gnhits);
  m_ntOldReco -> SetBranchAddress("gnmaps",          &rec_gnmaps);
  m_ntOldReco -> SetBranchAddress("gnintt",          &rec_gnintt);
  m_ntOldReco -> SetBranchAddress("gntpc",           &rec_gntpc);
  m_ntOldReco -> SetBranchAddress("gnmms",           &rec_gnmms);
  m_ntOldReco -> SetBranchAddress("gnlmaps",         &rec_gnlmaps);
  m_ntOldReco -> SetBranchAddress("gnlintt",         &rec_gnlintt);
  m_ntOldReco -> SetBranchAddress("gnltpc",          &rec_gnltpc);
  m_ntOldReco -> SetBranchAddress("gnlmms",          &rec_gnlmms);
  m_ntOldReco -> SetBranchAddress("gpx",             &rec_gpx);
  m_ntOldReco -> SetBranchAddress("gpy",             &rec_gpy);
  m_ntOldReco -> SetBranchAddress("gpz",             &rec_gpz);
  m_ntOldReco -> SetBranchAddress("gpt",             &rec_gpt);
  m_ntOldReco -> SetBranchAddress("geta",            &rec_geta);
  m_ntOldReco -> SetBranchAddress("gphi",            &rec_gphi);
  m_ntOldReco -> SetBranchAddress("gvx",             &rec_gvx);
  m_ntOldReco -> SetBranchAddress("gvy",             &rec_gvy);
  m_ntOldReco -> SetBranchAddress("gvz",             &rec_gvz);
  m_ntOldReco -> SetBranchAddress("gvt",             &rec_gvt);
  m_ntOldReco -> SetBranchAddress("gfpx",            &rec_gfpx);
  m_ntOldReco -> SetBranchAddress("gfpy",            &rec_gfpy);
  m_ntOldReco -> SetBranchAddress("gfpz",            &rec_gfpz);
  m_ntOldReco -> SetBranchAddress("gfx",             &rec_gfx);
  m_ntOldReco -> SetBranchAddress("gfy",             &rec_gfy);
  m_ntOldReco -> SetBranchAddress("gfz",             &rec_gfz);
  m_ntOldReco -> SetBranchAddress("gembed",          &rec_gembed);
  m_ntOldReco -> SetBranchAddress("gprimary",        &rec_gprimary);
  m_ntOldReco -> SetBranchAddress("nfromtruth",      &rec_nfromtruth);
  m_ntOldReco -> SetBranchAddress("nwrong",          &rec_nwrong);
  m_ntOldReco -> SetBranchAddress("ntrumaps",        &rec_ntrumaps);
  m_ntOldReco -> SetBranchAddress("nwrongmaps",      &rec_nwrongmaps);
  m_ntOldReco -> SetBranchAddress("ntruintt",        &rec_ntruintt);
  m_ntOldReco -> SetBranchAddress("nwrongintt",      &rec_nwrongintt);
  m_ntOldReco -> SetBranchAddress("ntrutpc",         &rec_ntrutpc);
  m_ntOldReco -> SetBranchAddress("nwrongtpc",       &rec_nwrongtpc);
  m_ntOldReco -> SetBranchAddress("ntrumms",         &rec_ntrumms);
  m_ntOldReco -> SetBranchAddress("nwrongmms",       &rec_nwrongmms);
  m_ntOldReco -> SetBranchAddress("ntrutpc1",        &rec_ntrutpc1);
  m_ntOldReco -> SetBranchAddress("nwrongtpc1",      &rec_nwrongtpc1);
  m_ntOldReco -> SetBranchAddress("ntrutpc11",       &rec_ntrutpc11);
  m_ntOldReco -> SetBranchAddress("nwrongtpc11",     &rec_nwrongtpc11);
  m_ntOldReco -> SetBranchAddress("ntrutpc2",        &rec_ntrutpc2);
  m_ntOldReco -> SetBranchAddress("nwrongtpc2",      &rec_nwrongtpc2);
  m_ntOldReco -> SetBranchAddress("ntrutpc3",        &rec_ntrutpc3);
  m_ntOldReco -> SetBranchAddress("nwrongtpc3",      &rec_nwrongtpc3);
  m_ntOldReco -> SetBranchAddress("layersfromtruth", &rec_layersfromtruth);
  m_ntOldReco -> SetBranchAddress("npedge",          &rec_npedge);
  m_ntOldReco -> SetBranchAddress("nredge",          &rec_nredge);
  m_ntOldReco -> SetBranchAddress("nbig",            &rec_nbig);
  m_ntOldReco -> SetBranchAddress("novlp",           &rec_novlp);
  m_ntOldReco -> SetBranchAddress("merr",            &rec_merr);
  m_ntOldReco -> SetBranchAddress("msize",           &rec_msize);
  m_ntOldReco -> SetBranchAddress("nhittpcall",      &rec_nhittpcall);
  m_ntOldReco -> SetBranchAddress("nhittpcin",       &rec_nhittpcin);
  m_ntOldReco -> SetBranchAddress("nhittpcmid",      &rec_nhittpcmid);
  m_ntOldReco -> SetBranchAddress("nhittpcout",      &rec_nhittpcout);
  m_ntOldReco -> SetBranchAddress("nclusall",        &rec_nclusall);
  m_ntOldReco -> SetBranchAddress("nclustpc",        &rec_nclustpc);
  m_ntOldReco -> SetBranchAddress("nclusintt",       &rec_nclusintt);
  m_ntOldReco -> SetBranchAddress("nclusmaps",       &rec_nclusmaps);
  m_ntOldReco -> SetBranchAddress("nclusmms",        &rec_nclusmms);
  cout << "        Set input branches." << endl;

  // grab no. of entries
  const int64_t nTrueEntries = m_ntOldTrue -> GetEntries();
  const int64_t nRecoEntries = m_ntOldReco -> GetEntries(); 
  cout << "        Beginning truth particle loop: " << nTrueEntries << " to process" << endl;

  // loop over truth particles
  int64_t nTrueBytes = 0;
  for (int64_t iTrueEntry = 0; iTrueEntry < nTrueEntries; iTrueEntry++) {

    // grab truth particle entry
    const int64_t trueBytes = m_ntOldTrue -> GetEntry(iTrueEntry);
    if (trueBytes < 0) {
      cerr << "PANIC: issue with entry " << iTrueEntry << "! Aborting loop!\n" << endl;
      break;
    } else {
      nTrueBytes += trueBytes;
    }

    const int64_t iTrueProg = iTrueEntry + 1;
    if (iTrueProg == nTrueEntries) {
      cout << "          Processing entry " << iTrueProg << "/" << nTrueEntries << "..." << endl;
    } else {
      cout << "          Processing entry " << iTrueProg << "/" << nTrueEntries << "...\r" << flush;
    }

    // select only primary truth particles
    const bool isPrimary = ((tru_gprimary == 1) && !isnan(tru_trackID));
    if (!isPrimary) continue;

    // run calculations
    const double tru_gntot = tru_gnintt + tru_gnmaps + tru_gntpc;

    // fill truth 1D histograms
    m_vecOldHists1D[Var::NTot][Type::Truth]   -> Fill(tru_gntot);
    m_vecOldHists1D[Var::NIntt][Type::Truth]  -> Fill(tru_gnintt);
    m_vecOldHists1D[Var::NMvtx][Type::Truth]  -> Fill(tru_gnmaps);
    m_vecOldHists1D[Var::NTpc][Type::Truth]   -> Fill(tru_gntpc);
    m_vecOldHists1D[Var::RTot][Type::Truth]   -> Fill(1.);
    m_vecOldHists1D[Var::RIntt][Type::Truth]  -> Fill(1.);
    m_vecOldHists1D[Var::RMvtx][Type::Truth]  -> Fill(1.);
    m_vecOldHists1D[Var::RTpc][Type::Truth]   -> Fill(1.);
    m_vecOldHists1D[Var::Phi][Type::Truth]    -> Fill(tru_gphi);
    m_vecOldHists1D[Var::Eta][Type::Truth]    -> Fill(tru_geta);
    m_vecOldHists1D[Var::Pt][Type::Truth]     -> Fill(tru_gpt);
    m_vecOldHists1D[Var::Frac][Type::Truth]   -> Fill(1.);
    m_vecOldHists1D[Var::Qual][Type::Truth]   -> Fill(1.);
    m_vecOldHists1D[Var::PtErr][Type::Truth]  -> Fill(1.);
    m_vecOldHists1D[Var::EtaErr][Type::Truth] -> Fill(1.);
    m_vecOldHists1D[Var::PhiErr][Type::Truth] -> Fill(1.);
    m_vecOldHists1D[Var::PtRes][Type::Truth]  -> Fill(1.);
    m_vecOldHists1D[Var::EtaRes][Type::Truth] -> Fill(1.);
    m_vecOldHists1D[Var::PhiRes][Type::Truth] -> Fill(1.);

    // fill truth 2D histograms
    m_vecOldHists2D[Var::NTot][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, tru_gntot);
    m_vecOldHists2D[Var::NIntt][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, tru_gnintt);
    m_vecOldHists2D[Var::NMvtx][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, tru_gnmaps);
    m_vecOldHists2D[Var::NTpc][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, tru_gntpc);
    m_vecOldHists2D[Var::RTot][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::RIntt][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::RMvtx][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::RTpc][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::Phi][Type::Truth][Comp::VsTruthPt]    -> Fill(tru_gpt, tru_gphi);
    m_vecOldHists2D[Var::Eta][Type::Truth][Comp::VsTruthPt]    -> Fill(tru_gpt, tru_geta);
    m_vecOldHists2D[Var::Pt][Type::Truth][Comp::VsTruthPt]     -> Fill(tru_gpt, tru_gpt);
    m_vecOldHists2D[Var::Frac][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::Qual][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::PtErr][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::EtaErr][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::PhiErr][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::PtRes][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::EtaRes][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    m_vecOldHists2D[Var::PhiRes][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);

    m_vecOldHists2D[Var::NTot][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_gntpc, tru_gntot);
    m_vecOldHists2D[Var::NIntt][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, tru_gnintt);
    m_vecOldHists2D[Var::NMvtx][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, tru_gnmaps);
    m_vecOldHists2D[Var::NTpc][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_gntpc, tru_gntpc);
    m_vecOldHists2D[Var::RTot][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::RIntt][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::RMvtx][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::RTpc][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::Phi][Type::Truth][Comp::VsNumTpc]    -> Fill(tru_gntpc, tru_gphi);
    m_vecOldHists2D[Var::Eta][Type::Truth][Comp::VsNumTpc]    -> Fill(tru_gntpc, tru_geta);
    m_vecOldHists2D[Var::Pt][Type::Truth][Comp::VsNumTpc]     -> Fill(tru_gntpc, tru_gpt);
    m_vecOldHists2D[Var::Frac][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::Qual][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::PtErr][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::EtaErr][Type::Truth][Comp::VsNumTpc] -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::PhiErr][Type::Truth][Comp::VsNumTpc] -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::PtRes][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::EtaRes][Type::Truth][Comp::VsNumTpc] -> Fill(tru_gntpc, 1.);
    m_vecOldHists2D[Var::PhiRes][Type::Truth][Comp::VsNumTpc] -> Fill(tru_gntpc, 1.);
  }  // end truth particle loop

  // announce next entry loop
  cout << "        Finished truth particle loop.\n"
       << "        Beginning reconstructed track loop: " << nRecoEntries << " to process"
       << endl;

  // loop over reco tracks
  int64_t nRecoBytes = 0;
  for (int64_t iRecoEntry = 0; iRecoEntry < nRecoEntries; iRecoEntry++) {

    // grab reco track entry
    const int64_t recoBytes = m_ntOldReco -> GetEntry(iRecoEntry);
    if (recoBytes < 0) {
      cerr << "PANIC: issue with entry " << iRecoEntry << "! Aborting loop!\n" << endl;
      break;
    } else {
      nRecoBytes += recoBytes;
    }

    const int64_t iRecoProg = iRecoEntry + 1;
    if (iRecoProg == nRecoEntries) {
      cout << "          Processing entry " << iRecoProg << "/" << nRecoEntries << "..." << endl;
    } else {
      cout << "          Processing entry " << iRecoProg << "/" << nRecoEntries << "...\r" << flush;
    }

    // skip nan's
    //   - TODO also add option to filter out non-primary tracks
    if (isnan(rec_gpt)) continue;

    // run calculations
    const double rec_ntot   = rec_nintt + rec_nmaps + rec_ntpc;
    const double rec_gntot  = rec_gnintt + rec_gnmaps + rec_gntpc;
    const double rec_rtot   = rec_ntot / rec_gntot;
    const double rec_rintt  = rec_nintt / rec_gnintt;
    const double rec_rmaps  = rec_nmaps / rec_gnmaps;
    const double rec_rtpc   = rec_ntpc / rec_gntpc;
    const double rec_ptfrac = rec_pt / rec_gpt;
    const double rec_ptper  = rec_deltapt / rec_pt;
    const double rec_etaper = rec_deltaeta / rec_eta;
    const double rec_phiper = rec_deltaphi / rec_phi;
    const double rec_ptres  = abs(rec_pt - rec_gpt) / rec_gpt;
    const double rec_etares = abs(rec_eta - rec_geta) / rec_geta;
    const double rec_phires = abs(rec_phi - rec_gphi) / rec_gphi;

    // fill all matched reco 1D histograms
    m_vecOldHists1D[Var::NTot][Type::Track]   -> Fill(rec_ntot);
    m_vecOldHists1D[Var::NIntt][Type::Track]  -> Fill(rec_nintt);
    m_vecOldHists1D[Var::NMvtx][Type::Track]  -> Fill(rec_nmaps);
    m_vecOldHists1D[Var::NTpc][Type::Track]   -> Fill(rec_ntpc);
    m_vecOldHists1D[Var::RTot][Type::Track]   -> Fill(rec_rtot);
    m_vecOldHists1D[Var::RIntt][Type::Track]  -> Fill(rec_rintt);
    m_vecOldHists1D[Var::RMvtx][Type::Track]  -> Fill(rec_rmaps);
    m_vecOldHists1D[Var::RTpc][Type::Track]   -> Fill(rec_rtpc);
    m_vecOldHists1D[Var::Phi][Type::Track]    -> Fill(rec_phi);
    m_vecOldHists1D[Var::Eta][Type::Track]    -> Fill(rec_eta);
    m_vecOldHists1D[Var::Pt][Type::Track]     -> Fill(rec_pt);
    m_vecOldHists1D[Var::Frac][Type::Track]   -> Fill(rec_ptfrac);
    m_vecOldHists1D[Var::Qual][Type::Track]   -> Fill(rec_quality);
    m_vecOldHists1D[Var::PtErr][Type::Track]  -> Fill(rec_ptper);
    m_vecOldHists1D[Var::EtaErr][Type::Track] -> Fill(rec_etaper);
    m_vecOldHists1D[Var::PhiErr][Type::Track] -> Fill(rec_phiper);
    m_vecOldHists1D[Var::PtRes][Type::Track]  -> Fill(rec_ptres);
    m_vecOldHists1D[Var::EtaRes][Type::Track] -> Fill(rec_etares);
    m_vecOldHists1D[Var::PhiRes][Type::Track] -> Fill(rec_phires);

    // fill all matched reco 2D histograms
    m_vecOldHists2D[Var::NTot][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ntot);
    m_vecOldHists2D[Var::NIntt][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nintt);
    m_vecOldHists2D[Var::NMvtx][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nmaps);
    m_vecOldHists2D[Var::NTpc][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ntpc);
    m_vecOldHists2D[Var::RTot][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rtot);
    m_vecOldHists2D[Var::RIntt][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rintt);
    m_vecOldHists2D[Var::RMvtx][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rmaps);
    m_vecOldHists2D[Var::RTpc][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rtpc);
    m_vecOldHists2D[Var::Phi][Type::Track][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_phi);
    m_vecOldHists2D[Var::Eta][Type::Track][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_eta);
    m_vecOldHists2D[Var::Pt][Type::Track][Comp::VsTruthPt]     -> Fill(rec_gpt, rec_pt);
    m_vecOldHists2D[Var::Frac][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ptfrac);
    m_vecOldHists2D[Var::Qual][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_quality);
    m_vecOldHists2D[Var::PtErr][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptper);
    m_vecOldHists2D[Var::EtaErr][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etaper);
    m_vecOldHists2D[Var::PhiErr][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phiper);
    m_vecOldHists2D[Var::PtRes][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptres);
    m_vecOldHists2D[Var::EtaRes][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etares);
    m_vecOldHists2D[Var::PhiRes][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phires);

    m_vecOldHists2D[Var::NTot][Type::Track][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_ntot);
    m_vecOldHists2D[Var::NIntt][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_nintt);
    m_vecOldHists2D[Var::NMvtx][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_nmaps);
    m_vecOldHists2D[Var::NTpc][Type::Track][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_ntpc);
    m_vecOldHists2D[Var::RTot][Type::Track][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_rtpc);
    m_vecOldHists2D[Var::RIntt][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rintt);
    m_vecOldHists2D[Var::RMvtx][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rmaps);
    m_vecOldHists2D[Var::RTpc][Type::Track][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_rtpc);
    m_vecOldHists2D[Var::Phi][Type::Track][Comp::VsNumTpc]    -> Fill(rec_gntpc, rec_phi);
    m_vecOldHists2D[Var::Eta][Type::Track][Comp::VsNumTpc]    -> Fill(rec_gntpc, rec_eta);
    m_vecOldHists2D[Var::Pt][Type::Track][Comp::VsNumTpc]     -> Fill(rec_gntpc, rec_pt);
    m_vecOldHists2D[Var::Frac][Type::Track][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_ptfrac);
    m_vecOldHists2D[Var::Qual][Type::Track][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_quality);
    m_vecOldHists2D[Var::PtErr][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ptper);
    m_vecOldHists2D[Var::EtaErr][Type::Track][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_etaper);
    m_vecOldHists2D[Var::PhiErr][Type::Track][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_phiper);
    m_vecOldHists2D[Var::PtRes][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ptres);
    m_vecOldHists2D[Var::EtaRes][Type::Track][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_etares);
    m_vecOldHists2D[Var::PhiRes][Type::Track][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_phires);

    // fill weird and normal matched reco 1D histograms
    const bool isNormalTrack = ((rec_ptfrac >= m_config.oddPtFrac.first) && (rec_ptfrac <= m_config.oddPtFrac.second));
    if (isNormalTrack) {
      m_vecOldHists1D[Var::NTot][Type::Normal]   -> Fill(rec_ntot);
      m_vecOldHists1D[Var::NIntt][Type::Normal]  -> Fill(rec_nintt);
      m_vecOldHists1D[Var::NMvtx][Type::Normal]  -> Fill(rec_nmaps);
      m_vecOldHists1D[Var::NTpc][Type::Normal]   -> Fill(rec_ntpc);
      m_vecOldHists1D[Var::RTot][Type::Normal]   -> Fill(rec_rtot);
      m_vecOldHists1D[Var::RIntt][Type::Normal]  -> Fill(rec_rintt);
      m_vecOldHists1D[Var::RMvtx][Type::Normal]  -> Fill(rec_rmaps);
      m_vecOldHists1D[Var::RTpc][Type::Normal]   -> Fill(rec_rtpc);
      m_vecOldHists1D[Var::Phi][Type::Normal]    -> Fill(rec_phi);
      m_vecOldHists1D[Var::Eta][Type::Normal]    -> Fill(rec_eta);
      m_vecOldHists1D[Var::Pt][Type::Normal]     -> Fill(rec_pt);
      m_vecOldHists1D[Var::Frac][Type::Normal]   -> Fill(rec_ptfrac);
      m_vecOldHists1D[Var::Qual][Type::Normal]   -> Fill(rec_quality);
      m_vecOldHists1D[Var::PtErr][Type::Normal]  -> Fill(rec_ptper);
      m_vecOldHists1D[Var::EtaErr][Type::Normal] -> Fill(rec_etaper);
      m_vecOldHists1D[Var::PhiErr][Type::Normal] -> Fill(rec_phiper);
      m_vecOldHists1D[Var::PtRes][Type::Normal]  -> Fill(rec_ptres);
      m_vecOldHists1D[Var::EtaRes][Type::Normal] -> Fill(rec_etares);
      m_vecOldHists1D[Var::PhiRes][Type::Normal] -> Fill(rec_phires);

      m_vecOldHists2D[Var::NTot][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ntot);
      m_vecOldHists2D[Var::NIntt][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nintt);
      m_vecOldHists2D[Var::NMvtx][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nmaps);
      m_vecOldHists2D[Var::NTpc][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ntpc);
      m_vecOldHists2D[Var::RTot][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rtot);
      m_vecOldHists2D[Var::RIntt][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rintt);
      m_vecOldHists2D[Var::RMvtx][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rmaps);
      m_vecOldHists2D[Var::RTpc][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rtpc);
      m_vecOldHists2D[Var::Phi][Type::Normal][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_phi);
      m_vecOldHists2D[Var::Eta][Type::Normal][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_eta);
      m_vecOldHists2D[Var::Pt][Type::Normal][Comp::VsTruthPt]     -> Fill(rec_gpt, rec_pt);
      m_vecOldHists2D[Var::Frac][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ptfrac);
      m_vecOldHists2D[Var::Qual][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_quality);
      m_vecOldHists2D[Var::PtErr][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptper);
      m_vecOldHists2D[Var::EtaErr][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etaper);
      m_vecOldHists2D[Var::PhiErr][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phiper);
      m_vecOldHists2D[Var::PtRes][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptres);
      m_vecOldHists2D[Var::EtaRes][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etares);
      m_vecOldHists2D[Var::PhiRes][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phires);

      m_vecOldHists2D[Var::NTot][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_ntot);
      m_vecOldHists2D[Var::NIntt][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_nintt);
      m_vecOldHists2D[Var::NMvtx][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_nmaps);
      m_vecOldHists2D[Var::NTpc][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_ntpc);
      m_vecOldHists2D[Var::RTot][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_rtot);
      m_vecOldHists2D[Var::RIntt][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rintt);
      m_vecOldHists2D[Var::RMvtx][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rmaps);
      m_vecOldHists2D[Var::RTpc][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_rtpc);
      m_vecOldHists2D[Var::Phi][Type::Normal][Comp::VsNumTpc]    -> Fill(rec_gntpc, rec_phi);
      m_vecOldHists2D[Var::Eta][Type::Normal][Comp::VsNumTpc]    -> Fill(rec_gntpc, rec_eta);
      m_vecOldHists2D[Var::Pt][Type::Normal][Comp::VsNumTpc]     -> Fill(rec_gntpc, rec_pt);
      m_vecOldHists2D[Var::Frac][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_ptfrac);
      m_vecOldHists2D[Var::Qual][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_quality);
      m_vecOldHists2D[Var::PtErr][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ptper);
      m_vecOldHists2D[Var::EtaErr][Type::Normal][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_etaper);
      m_vecOldHists2D[Var::PhiErr][Type::Normal][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_phiper);
      m_vecOldHists2D[Var::PtRes][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ptres);
      m_vecOldHists2D[Var::EtaRes][Type::Normal][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_etares);
      m_vecOldHists2D[Var::PhiRes][Type::Normal][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_phires);
    } else {
      m_vecOldHists1D[Var::NTot][Type::Weird]   -> Fill(rec_ntot);
      m_vecOldHists1D[Var::NIntt][Type::Weird]  -> Fill(rec_nintt);
      m_vecOldHists1D[Var::NMvtx][Type::Weird]  -> Fill(rec_nmaps);
      m_vecOldHists1D[Var::NTpc][Type::Weird]   -> Fill(rec_ntpc);
      m_vecOldHists1D[Var::RTot][Type::Weird]   -> Fill(rec_rtot);
      m_vecOldHists1D[Var::RIntt][Type::Weird]  -> Fill(rec_rintt);
      m_vecOldHists1D[Var::RMvtx][Type::Weird]  -> Fill(rec_rmaps);
      m_vecOldHists1D[Var::RTpc][Type::Weird]   -> Fill(rec_rtpc);
      m_vecOldHists1D[Var::Phi][Type::Weird]    -> Fill(rec_phi);
      m_vecOldHists1D[Var::Eta][Type::Weird]    -> Fill(rec_eta);
      m_vecOldHists1D[Var::Pt][Type::Weird]     -> Fill(rec_pt);
      m_vecOldHists1D[Var::Frac][Type::Weird]   -> Fill(rec_ptfrac);
      m_vecOldHists1D[Var::Qual][Type::Weird]   -> Fill(rec_quality);
      m_vecOldHists1D[Var::PtErr][Type::Weird]  -> Fill(rec_ptper);
      m_vecOldHists1D[Var::EtaErr][Type::Weird] -> Fill(rec_etaper);
      m_vecOldHists1D[Var::PhiErr][Type::Weird] -> Fill(rec_phiper);
      m_vecOldHists1D[Var::PtRes][Type::Weird]  -> Fill(rec_ptres);
      m_vecOldHists1D[Var::EtaRes][Type::Weird] -> Fill(rec_etares);
      m_vecOldHists1D[Var::PhiRes][Type::Weird] -> Fill(rec_phires);

      m_vecOldHists2D[Var::NTot][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ntot);
      m_vecOldHists2D[Var::NIntt][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nintt);
      m_vecOldHists2D[Var::NMvtx][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_nmaps);
      m_vecOldHists2D[Var::NTpc][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ntpc);
      m_vecOldHists2D[Var::RTot][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rtot);
      m_vecOldHists2D[Var::RIntt][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rintt);
      m_vecOldHists2D[Var::RMvtx][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rmaps);
      m_vecOldHists2D[Var::RTpc][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_rtpc);
      m_vecOldHists2D[Var::Phi][Type::Weird][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_phi);
      m_vecOldHists2D[Var::Eta][Type::Weird][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_eta);
      m_vecOldHists2D[Var::Pt][Type::Weird][Comp::VsTruthPt]     -> Fill(rec_gpt, rec_pt);
      m_vecOldHists2D[Var::Frac][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_ptfrac);
      m_vecOldHists2D[Var::Qual][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_quality);
      m_vecOldHists2D[Var::PtErr][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptper);
      m_vecOldHists2D[Var::EtaErr][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etaper);
      m_vecOldHists2D[Var::PhiErr][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phiper);
      m_vecOldHists2D[Var::PtRes][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptres);
      m_vecOldHists2D[Var::EtaRes][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_etares);
      m_vecOldHists2D[Var::PhiRes][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_phires);

      m_vecOldHists2D[Var::NTot][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_ntot);
      m_vecOldHists2D[Var::NIntt][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_nintt);
      m_vecOldHists2D[Var::NMvtx][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_nmaps);
      m_vecOldHists2D[Var::NTpc][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_ntpc);
      m_vecOldHists2D[Var::RTot][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_rtpc);
      m_vecOldHists2D[Var::RIntt][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rintt);
      m_vecOldHists2D[Var::RMvtx][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rmaps);
      m_vecOldHists2D[Var::RTpc][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_rtpc);
      m_vecOldHists2D[Var::Phi][Type::Weird][Comp::VsNumTpc]    -> Fill(rec_gntpc, rec_phi);
      m_vecOldHists2D[Var::Eta][Type::Weird][Comp::VsNumTpc]    -> Fill(rec_gntpc, rec_eta);
      m_vecOldHists2D[Var::Pt][Type::Weird][Comp::VsNumTpc]     -> Fill(rec_gntpc, rec_pt);
      m_vecOldHists2D[Var::Frac][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_ptfrac);
      m_vecOldHists2D[Var::Qual][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_quality);
      m_vecOldHists2D[Var::PtErr][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ptper);
      m_vecOldHists2D[Var::EtaErr][Type::Weird][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_etaper);
      m_vecOldHists2D[Var::PhiErr][Type::Weird][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_phiper);
      m_vecOldHists2D[Var::PtRes][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ptres);
      m_vecOldHists2D[Var::EtaRes][Type::Weird][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_etares);
      m_vecOldHists2D[Var::PhiRes][Type::Weird][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_phires);
    }
  }  // end reco track loop

  // announce method end and return
  cout << "        Finished reconstructed track loop.\n"
       << "      Finished getting histograms from old matcher track tuple."
       << endl;
  return;

}  // end 'GetOldTupleHists()'



void STrackMatcherComparator::MakeRatiosAndPlots(
  const vector<vector<TH1D*>> vecNewHists1D,
  const vector<vector<vector<TH2D*>>> vecNewHists2D,
  const int iDir,
  const string sLabel
) {

  // announce start of routine
  const size_t nHistRows1D  = m_vecOldHists1D.size();
  const size_t nHistRows2D  = m_vecOldHists2D.size();
  const size_t nHistTypes1D = m_vecOldHists1D.front().size();
  const size_t nHistTypes2D = m_vecOldHists2D.front().size();
  const size_t nHist2D      = m_vecOldHists2D.front().front().size();
  cout << "      Making ratios and plots:" << endl;

  // create 1d canvas names
  vector<vector<string>> vecCanvasNames1D(nHistRows1D);
  for (size_t iHistRow1D = 0; iHistRow1D < nHistRows1D; iHistRow1D++) {
    vecCanvasNames1D[iHistRow1D].resize(nHistTypes1D);
    for (size_t iHistType1D = 0; iHistType1D < nHistTypes1D; iHistType1D++) {
      vecCanvasNames1D[iHistRow1D][iHistType1D] = m_hist.vecNameBase[iHistRow1D][iHistType1D];
      vecCanvasNames1D[iHistRow1D][iHistType1D].replace(0, 1, "c");
    }
  }

  // create 2d canvas names
  vector<vector<vector<string>>> vecCanvasNames2D(nHistRows2D);
  for (size_t iHistRow2D = 0; iHistRow2D < nHistRows2D; iHistRow2D++) {
    vecCanvasNames2D[iHistRow2D].resize(nHistRows2D);
    for (size_t iHistType2D = 0; iHistType2D < nHistTypes2D; iHistType2D++) {
      vecCanvasNames2D[iHistRow2D][iHistType2D].resize(nHist2D);
      for (size_t iHist2D = 0; iHist2D < nHist2D; iHist2D++) {
        vecCanvasNames2D[iHistRow2D][iHistType2D][iHist2D] = m_hist.vecNameBase[iHistRow2D][iHistType2D];
        vecCanvasNames2D[iHistRow2D][iHistType2D][iHist2D] += m_hist.vecVsModifiers[iHist2D];
        vecCanvasNames2D[iHistRow2D][iHistType2D][iHist2D].replace(0, 1, "c");
      }
    }
  }
  cout << "        Set up canvas names." << endl;

  // normalize histograms and set axes' ranges as needed ----------------------

  // normalize by integral if needed
  if (m_config.doIntNorm) {
    for (auto oldHistRow1D : m_vecOldHists1D) {
      for (auto hOldHist1D : oldHistRow1D) {
        const double intOld1D = hOldHist1D -> Integral();
        if (intOld1D > 0.) {
          hOldHist1D -> Scale(1. / intOld1D);
        }
      }
    }
    for (auto newHistRow1D : vecNewHists1D) {
      for (auto hNewHist1D : newHistRow1D) {
        const double intNew1D = hNewHist1D -> Integral();
        if (intNew1D > 0.) {
          hNewHist1D -> Scale(1. / intNew1D);
        }
      }
    }
    for (auto oldHistRow2D : m_vecOldHists2D) {
      for (auto oldHistTypes2D : oldHistRow2D) {
        for (auto hOldHist2D : oldHistTypes2D) {
          const double intOld2D = hOldHist2D -> Integral();
          if (intOld2D > 0.) {
            hOldHist2D -> Scale(1. / intOld2D);
          }
        }
      }
    }
    for (auto newHistRow2D : vecNewHists2D) {
      for (auto newHistTypes2D : newHistRow2D) {
        for (auto hNewHist2D : newHistTypes2D) {
          const double intNew2D = hNewHist2D -> Integral();
          if (intNew2D > 0.) {
            hNewHist2D -> Scale(1. / intNew2D);
          }
        }
      }
    }
    cout << "        Normalized histograms." << endl;
  }

  // set z-axis ranges if needed
  if (m_config.matchVertScales) {
    for (size_t iHistRow2D = 0; iHistRow2D < nHistRows2D; iHistRow2D++) {
      for (size_t iHistType2D = 0; iHistType2D < nHistTypes2D; iHistType2D++) {
        for (size_t iHist2D = 0; iHist2D < nHist2D; iHist2D++) {
          const float oldMin = m_vecOldHists2D[iHistRow2D][iHistType2D][iHist2D] -> GetMinimum(0.);
          const float oldMax = m_vecOldHists2D[iHistRow2D][iHistType2D][iHist2D] -> GetMaximum();
          const float newMin = vecNewHists2D[iHistRow2D][iHistType2D][iHist2D]   -> GetMinimum(0.);
          const float newMax = vecNewHists2D[iHistRow2D][iHistType2D][iHist2D]   -> GetMaximum();
          const float setMin = min(oldMin, newMin);
          const float setMax = max(oldMax, newMax);
          m_vecOldHists2D[iHistRow2D][iHistType2D][iHist2D] -> GetZaxis() -> SetRangeUser(setMin, setMax);
          vecNewHists2D[iHistRow2D][iHistType2D][iHist2D]   -> GetZaxis() -> SetRangeUser(setMin, setMax);
        }
      }
    }
    cout << "        Adjusted z-axis scales to match." << endl;
  }

  // pick relevant count
  string countUse("");
  if (m_config.doIntNorm) {
    countUse = m_config.norm;
  } else {
    countUse = m_config.count;
  }

  // calculate ratios ---------------------------------------------------------

  // calculate 1d ratios
  vector<vector<TH1D*>> vecRatios1D(nHistRows1D);
  for (size_t iHistRow1D = 0; iHistRow1D < nHistRows1D; iHistRow1D++) {
    vecRatios1D[iHistRow1D].resize(nHistTypes1D);
    for (size_t iHist1D = 0; iHist1D < nHistTypes1D; iHist1D++) {

      // make histogram name
      TString sRatioName = m_vecOldHists1D[iHistRow1D][iHist1D] -> GetName();
      sRatioName.Append("_");
      sRatioName.Append(sLabel.data());

      vecRatios1D[iHistRow1D][iHist1D] = (TH1D*) m_vecOldHists1D[iHistRow1D][iHist1D] -> Clone();
      vecRatios1D[iHistRow1D][iHist1D] -> SetName(sRatioName.Data());
      vecRatios1D[iHistRow1D][iHist1D] -> Reset("ICES");
      vecRatios1D[iHistRow1D][iHist1D] -> Divide(m_vecOldHists1D[iHistRow1D][iHist1D], vecNewHists1D[iHistRow1D][iHist1D], 1., 1.);
    }
  }

  // calculate 2d ratios
  vector<vector<vector<TH2D*>>> vecRatios2D(nHistRows2D);
  for (size_t iHistRow2D = 0; iHistRow2D < nHistRows2D; iHistRow2D++) {
    vecRatios2D[iHistRow2D].resize(nHistTypes2D);
    for (size_t iHistType2D = 0; iHistType2D < nHistTypes2D; iHistType2D++) {
      vecRatios2D[iHistRow2D][iHistType2D].resize(nHist2D);
      for (size_t iHist2D = 0; iHist2D < nHist2D; iHist2D++) {

        // make histogram name
        TString sRatioName = m_vecOldHists2D[iHistRow2D][iHistType2D][iHist2D] -> GetName();
        sRatioName.Append("_");
        sRatioName.Append(sLabel.data());

        vecRatios2D[iHistRow2D][iHistType2D][iHist2D] = (TH2D*) m_vecOldHists2D[iHistRow2D][iHistType2D][iHist2D] -> Clone();
        vecRatios2D[iHistRow2D][iHistType2D][iHist2D] -> SetName(sRatioName.Data());
        vecRatios2D[iHistRow2D][iHistType2D][iHist2D] -> Reset("ICES");
        vecRatios2D[iHistRow2D][iHistType2D][iHist2D] -> Divide(m_vecOldHists2D[iHistRow2D][iHistType2D][iHist2D], vecNewHists2D[iHistRow2D][iHistType2D][iHist2D], 1., 1.);
      }
    }
  }
  cout << "        Calculated ratios." << endl;

  // set histogram styles -----------------------------------------------------

  // style options
  const uint32_t fTxt(42);
  const uint32_t fAln(12);
  const uint32_t fCnt(1);
  const float    fLabH[m_const.nAxes]   = {0.04,  0.04,  0.03};
  const float    fLabR1[m_const.nAxes]  = {0.074, 0.074, 0.056};
  const float    fLabR2[m_const.nAxes]  = {0.04,  0.04,  0.03};
  const float    fTitH[m_const.nAxes]   = {0.04,  0.04,  0.04};
  const float    fTitR1[m_const.nAxes]  = {0.074, 0.074, 0.056};
  const float    fTitR2[m_const.nAxes]  = {0.04,  0.04,  0.04};
  const float    fOffTH[m_const.nAxes]  = {1.0,   1.3,   1.2};
  const float    fOffTR1[m_const.nAxes] = {0.8,   0.8,   1.0};
  const float    fOffTR2[m_const.nAxes] = {1.0,   1.3,   1.2};
  const float    fOffLH[m_const.nAxes]  = {0.005, 0.005, 0.000};
  const float    fOffLR1[m_const.nAxes] = {0.005, 0.005, 0.000};
  const float    fOffLR2[m_const.nAxes] = {0.005, 0.005, 0.000};

  // set old histogram styles
  for (auto oldHistRow1D : m_vecOldHists1D) {
    for (auto hOldHist1D : oldHistRow1D) {
      hOldHist1D -> SetMarkerColor(m_config.fCol.first);
      hOldHist1D -> SetMarkerStyle(m_config.fMar.first);
      hOldHist1D -> SetFillColor(m_config.fCol.first);
      hOldHist1D -> SetFillStyle(m_config.fFil.first);
      hOldHist1D -> SetLineColor(m_config.fCol.first);
      hOldHist1D -> SetLineStyle(m_config.fLin.first);
      hOldHist1D -> SetLineWidth(m_config.fWid.first);
      hOldHist1D -> SetTitle("");
      hOldHist1D -> SetTitleFont(fTxt);
      hOldHist1D -> GetXaxis() -> SetTitleFont(fTxt);
      hOldHist1D -> GetXaxis() -> SetTitleSize(fTitH[0]);
      hOldHist1D -> GetXaxis() -> SetTitleOffset(fOffTH[0]);
      hOldHist1D -> GetXaxis() -> SetLabelFont(fTxt);
      hOldHist1D -> GetXaxis() -> SetLabelSize(fLabH[0]);
      hOldHist1D -> GetXaxis() -> SetLabelOffset(fOffLH[0]);
      hOldHist1D -> GetXaxis() -> CenterTitle(fCnt);
      hOldHist1D -> GetYaxis() -> SetTitle(countUse.data());
      hOldHist1D -> GetYaxis() -> SetTitleFont(fTxt);
      hOldHist1D -> GetYaxis() -> SetTitleSize(fTitH[1]);
      hOldHist1D -> GetYaxis() -> SetTitleOffset(fOffTH[1]);
      hOldHist1D -> GetYaxis() -> SetLabelFont(fTxt);
      hOldHist1D -> GetYaxis() -> SetLabelSize(fLabH[1]);
      hOldHist1D -> GetYaxis() -> SetLabelOffset(fOffLH[1]);
      hOldHist1D -> GetYaxis() -> CenterTitle(fCnt);
    }
  }
  for (auto oldHistRow2D : m_vecOldHists2D) {
    for (auto oldHistTypes2D : oldHistRow2D) {
      for (auto hOldHist2D : oldHistTypes2D) {
        hOldHist2D -> SetMarkerColor(m_config.fCol.first);
        hOldHist2D -> SetMarkerStyle(m_config.fMar.first);
        hOldHist2D -> SetFillColor(m_config.fCol.first);
        hOldHist2D -> SetFillStyle(m_config.fFil.first);
        hOldHist2D -> SetLineColor(m_config.fCol.first);
        hOldHist2D -> SetLineStyle(m_config.fLin.first);
        hOldHist2D -> SetLineWidth(m_config.fWid.first);
        hOldHist2D -> SetTitle(m_config.legOld.data());
        hOldHist2D -> SetTitleFont(fTxt);
        hOldHist2D -> GetXaxis() -> SetTitleFont(fTxt);
        hOldHist2D -> GetXaxis() -> SetTitleSize(fTitH[0]);
        hOldHist2D -> GetXaxis() -> SetTitleOffset(fOffTH[0]);
        hOldHist2D -> GetXaxis() -> SetLabelFont(fTxt);
        hOldHist2D -> GetXaxis() -> SetLabelSize(fLabH[0]);
        hOldHist2D -> GetXaxis() -> SetLabelOffset(fOffLH[0]);
        hOldHist2D -> GetXaxis() -> CenterTitle(fCnt);
        hOldHist2D -> GetYaxis() -> SetTitleFont(fTxt);
        hOldHist2D -> GetYaxis() -> SetTitleSize(fTitH[1]);
        hOldHist2D -> GetYaxis() -> SetTitleOffset(fOffTH[1]);
        hOldHist2D -> GetYaxis() -> SetLabelFont(fTxt);
        hOldHist2D -> GetYaxis() -> SetLabelSize(fLabH[1]);
        hOldHist2D -> GetYaxis() -> SetLabelOffset(fOffLH[1]);
        hOldHist2D -> GetYaxis() -> CenterTitle(fCnt);
        hOldHist2D -> GetZaxis() -> SetTitle(countUse.data());
        hOldHist2D -> GetZaxis() -> SetTitleFont(fTxt);
        hOldHist2D -> GetZaxis() -> SetTitleSize(fTitH[2]);
        hOldHist2D -> GetZaxis() -> SetTitleOffset(fOffTH[2]);
        hOldHist2D -> GetZaxis() -> SetLabelFont(fTxt);
        hOldHist2D -> GetZaxis() -> SetLabelSize(fLabH[2]);
        hOldHist2D -> GetZaxis() -> SetLabelOffset(fOffLH[2]);
        hOldHist2D -> GetZaxis() -> CenterTitle(fCnt);
      }
    }
  }

  // set new histogram styles
  for (auto newHistRow1D : vecNewHists1D) {
    for (auto hNewHist1D : newHistRow1D) {
      hNewHist1D -> SetMarkerColor(m_config.fCol.second);
      hNewHist1D -> SetMarkerStyle(m_config.fMar.second);
      hNewHist1D -> SetFillColor(m_config.fCol.second);
      hNewHist1D -> SetFillStyle(m_config.fFil.second);
      hNewHist1D -> SetLineColor(m_config.fCol.second);
      hNewHist1D -> SetLineStyle(m_config.fLin.second);
      hNewHist1D -> SetLineWidth(m_config.fWid.second);
      hNewHist1D -> SetTitle("");
      hNewHist1D -> SetTitleFont(fTxt);
      hNewHist1D -> GetXaxis() -> SetTitleFont(fTxt);
      hNewHist1D -> GetXaxis() -> SetTitleSize(fTitH[0]);
      hNewHist1D -> GetXaxis() -> SetTitleOffset(fOffTH[0]);
      hNewHist1D -> GetXaxis() -> SetLabelFont(fTxt);
      hNewHist1D -> GetXaxis() -> SetLabelSize(fLabH[0]);
      hNewHist1D -> GetXaxis() -> SetLabelOffset(fOffLH[0]);
      hNewHist1D -> GetXaxis() -> CenterTitle(fCnt);
      hNewHist1D -> GetYaxis() -> SetTitle(countUse.data());
      hNewHist1D -> GetYaxis() -> SetTitleFont(fTxt);
      hNewHist1D -> GetYaxis() -> SetTitleSize(fTitH[1]);
      hNewHist1D -> GetYaxis() -> SetTitleOffset(fOffTH[1]);
      hNewHist1D -> GetYaxis() -> SetLabelFont(fTxt);
      hNewHist1D -> GetYaxis() -> SetLabelSize(fLabH[1]);
      hNewHist1D -> GetYaxis() -> SetLabelOffset(fOffLH[1]);
      hNewHist1D -> GetYaxis() -> CenterTitle(fCnt);
    }
  }
  for (auto newHistRow2D : vecNewHists2D) {
    for (auto newHistTypes2D : newHistRow2D) {
      for (auto hNewHist2D : newHistTypes2D) {
        hNewHist2D -> SetMarkerColor(m_config.fCol.first);
        hNewHist2D -> SetMarkerStyle(m_config.fMar.first);
        hNewHist2D -> SetFillColor(m_config.fCol.first);
        hNewHist2D -> SetFillStyle(m_config.fFil.first);
        hNewHist2D -> SetLineColor(m_config.fCol.first);
        hNewHist2D -> SetLineStyle(m_config.fLin.first);
        hNewHist2D -> SetLineWidth(m_config.fWid.first);
        hNewHist2D -> SetTitle(m_config.legNew.data());
        hNewHist2D -> SetTitleFont(fTxt);
        hNewHist2D -> GetXaxis() -> SetTitleFont(fTxt);
        hNewHist2D -> GetXaxis() -> SetTitleSize(fTitH[0]);
        hNewHist2D -> GetXaxis() -> SetTitleOffset(fOffTH[0]);
        hNewHist2D -> GetXaxis() -> SetLabelFont(fTxt);
        hNewHist2D -> GetXaxis() -> SetLabelSize(fLabH[0]);
        hNewHist2D -> GetXaxis() -> SetLabelOffset(fOffLH[0]);
        hNewHist2D -> GetXaxis() -> CenterTitle(fCnt);
        hNewHist2D -> GetYaxis() -> SetTitleFont(fTxt);
        hNewHist2D -> GetYaxis() -> SetTitleSize(fTitH[1]);
        hNewHist2D -> GetYaxis() -> SetTitleOffset(fOffTH[1]);
        hNewHist2D -> GetYaxis() -> SetLabelFont(fTxt);
        hNewHist2D -> GetYaxis() -> SetLabelSize(fLabH[1]);
        hNewHist2D -> GetYaxis() -> SetLabelOffset(fOffLH[1]);
        hNewHist2D -> GetYaxis() -> CenterTitle(fCnt);
        hNewHist2D -> GetZaxis() -> SetTitle(countUse.data());
        hNewHist2D -> GetZaxis() -> SetTitleFont(fTxt);
        hNewHist2D -> GetZaxis() -> SetTitleSize(fTitH[2]);
        hNewHist2D -> GetZaxis() -> SetTitleOffset(fOffTH[2]);
        hNewHist2D -> GetZaxis() -> SetLabelFont(fTxt);
        hNewHist2D -> GetZaxis() -> SetLabelSize(fLabH[2]);
        hNewHist2D -> GetZaxis() -> SetLabelOffset(fOffLH[2]);
        hNewHist2D -> GetZaxis() -> CenterTitle(fCnt);
      }
    }
  }

  // set ratio styles
  for (auto ratioRow1D : vecRatios1D) {
    for (auto hRatio1D : ratioRow1D) {
      hRatio1D -> SetMarkerColor(m_config.fCol.first);
      hRatio1D -> SetMarkerStyle(m_config.fMar.first);
      hRatio1D -> SetFillColor(m_config.fCol.first);
      hRatio1D -> SetFillStyle(m_config.fFil.first);
      hRatio1D -> SetLineColor(m_config.fCol.first);
      hRatio1D -> SetLineStyle(m_config.fLin.first);
      hRatio1D -> SetLineWidth(m_config.fWid.first);
      hRatio1D -> SetTitle("");
      hRatio1D -> SetTitleFont(fTxt);
      hRatio1D -> GetXaxis() -> SetTitleFont(fTxt);
      hRatio1D -> GetXaxis() -> SetTitleSize(fTitR1[0]);
      hRatio1D -> GetXaxis() -> SetTitleOffset(fOffTR1[0]);
      hRatio1D -> GetXaxis() -> SetLabelFont(fTxt);
      hRatio1D -> GetXaxis() -> SetLabelSize(fLabR1[0]);
      hRatio1D -> GetXaxis() -> SetLabelOffset(fOffLR1[0]);
      hRatio1D -> GetXaxis() -> CenterTitle(fCnt);
      hRatio1D -> GetYaxis() -> SetTitle(m_config.ratio.data());
      hRatio1D -> GetYaxis() -> SetTitleFont(fTxt);
      hRatio1D -> GetYaxis() -> SetTitleSize(fTitR1[1]);
      hRatio1D -> GetYaxis() -> SetTitleOffset(fOffTR1[1]);
      hRatio1D -> GetYaxis() -> SetLabelFont(fTxt);
      hRatio1D -> GetYaxis() -> SetLabelSize(fLabR1[1]);
      hRatio1D -> GetYaxis() -> SetLabelOffset(fOffLR1[1]);
      hRatio1D -> GetYaxis() -> CenterTitle(fCnt);
    }
  }
  for (auto ratioRow2D : vecRatios2D) {
    for (auto ratioTypes2D : ratioRow2D) {
      for (auto hRatio2D : ratioTypes2D) {
        hRatio2D -> SetMarkerColor(m_config.fCol.first);
        hRatio2D -> SetMarkerStyle(m_config.fMar.first);
        hRatio2D -> SetFillColor(m_config.fCol.first);
        hRatio2D -> SetFillStyle(m_config.fFil.first);
        hRatio2D -> SetLineColor(m_config.fCol.first);
        hRatio2D -> SetLineStyle(m_config.fLin.first);
        hRatio2D -> SetLineWidth(m_config.fWid.first);
        hRatio2D -> SetTitle("");
        hRatio2D -> SetTitleFont(fTxt);
        hRatio2D -> GetXaxis() -> SetTitleFont(fTxt);
        hRatio2D -> GetXaxis() -> SetTitleSize(fTitR2[0]);
        hRatio2D -> GetXaxis() -> SetTitleOffset(fOffTR2[0]);
        hRatio2D -> GetXaxis() -> SetLabelFont(fTxt);
        hRatio2D -> GetXaxis() -> SetLabelSize(fLabR2[0]);
        hRatio2D -> GetXaxis() -> SetLabelOffset(fOffLR2[0]);
        hRatio2D -> GetXaxis() -> CenterTitle(fCnt);
        hRatio2D -> GetYaxis() -> SetTitleFont(fTxt);
        hRatio2D -> GetYaxis() -> SetTitleSize(fTitR2[1]);
        hRatio2D -> GetYaxis() -> SetTitleOffset(fOffTR2[1]);
        hRatio2D -> GetYaxis() -> SetLabelFont(fTxt);
        hRatio2D -> GetYaxis() -> SetLabelSize(fLabR2[1]);
        hRatio2D -> GetYaxis() -> SetLabelOffset(fOffLR2[1]);
        hRatio2D -> GetYaxis() -> CenterTitle(fCnt);
        hRatio2D -> GetZaxis() -> SetTitle(m_config.ratio.data());
        hRatio2D -> GetZaxis() -> SetTitleFont(fTxt);
        hRatio2D -> GetZaxis() -> SetTitleSize(fTitR2[2]);
        hRatio2D -> GetZaxis() -> SetTitleOffset(fOffTR2[2]);
        hRatio2D -> GetZaxis() -> SetLabelFont(fTxt);
        hRatio2D -> GetZaxis() -> SetLabelSize(fLabR2[2]);
        hRatio2D -> GetZaxis() -> SetLabelOffset(fOffLR2[2]);
        hRatio2D -> GetZaxis() -> CenterTitle(fCnt);
      }
    }
  }
  cout << "        Set styles." << endl;

  // make legends and text boxes ----------------------------------------------

  // make legend
  const uint32_t fColLe(0);
  const uint32_t fFilLe(0);
  const uint32_t fLinLe(0);
  const float    fLegXY[m_const.nVtx] = {0.1, 0.1, 0.3, 0.2};

  TLegend *leg = new TLegend(fLegXY[0], fLegXY[1], fLegXY[2], fLegXY[3], m_config.header.data());
  leg -> SetFillColor(fColLe);
  leg -> SetFillStyle(fFilLe);
  leg -> SetLineColor(fColLe);
  leg -> SetLineStyle(fLinLe);
  leg -> SetTextFont(fTxt);
  leg -> SetTextAlign(fAln);
  leg -> AddEntry(m_vecOldHists1D.front().front(), m_config.legOld.data(), "pf");
  leg -> AddEntry(vecNewHists1D.front().front(),   m_config.legNew.data(), "pf");
  cout << "        Made legend." << endl;

  // make text
  const uint32_t fColTx(0);
  const uint32_t fFilTx(0);
  const uint32_t fLinTx(0);
  const float    fTxtXY[m_const.nVtx] = {0.3, 0.1, 0.5, 0.25};

  TPaveText *txt = new TPaveText(fTxtXY[0], fTxtXY[1], fTxtXY[2], fTxtXY[3], "NDC NB");
  txt -> SetFillColor(fColTx);
  txt -> SetFillStyle(fFilTx);
  txt -> SetLineColor(fColTx);
  txt -> SetLineStyle(fLinTx);
  txt -> SetTextFont(fTxt);
  txt -> SetTextAlign(fAln);
  for (string txtLine : m_config.info) {
    txt -> AddText(txtLine.data());
  }
  cout << "        Made text." << endl;

  // make plots ---------------------------------------------------------------

  // canvas parameters
  const uint32_t width1D(750);
  const uint32_t width2D(1500);
  const uint32_t width2DR(2250);
  const uint32_t height(750);
  const uint32_t heightR1(900);
  const uint32_t heightR2(500);
  const uint32_t fMode(0);
  const uint32_t fBord(2);
  const uint32_t fGrid(0);
  const uint32_t fTick(1);
  const uint32_t fLogX(0);
  const uint32_t fLogY(1);
  const uint32_t fLogZ(1);
  const uint32_t fFrame(0);
  const string   sOldPadName("pOld");
  const string   sNewPadName("pNew");
  const string   sHistPadName("pHist");
  const string   sRatPadName("pRatio");
  const float    fMargin1D[m_const.nSide]  = {0.02,  0.02, 0.15,  0.15};
  const float    fMargin1DH[m_const.nSide] = {0.02,  0.02, 0.005, 0.15};
  const float    fMargin1DR[m_const.nSide] = {0.005, 0.02, 0.2,   0.15};
  const float    fMargin2D[m_const.nSide]  = {0.10, 0.15, 0.15, 0.15};
  const float    xyOldPad[m_const.nVtx]    = {0.0,  0.0,  0.5,  1.0};
  const float    xyOldPadR[m_const.nVtx]   = {0.0,  0.0,  0.33, 1.0};
  const float    xyNewPad[m_const.nVtx]    = {0.5,  0.0,  1.0,  1.0};  
  const float    xyNewPadR[m_const.nVtx]   = {0.33, 0.0,  0.66, 1.0};
  const float    xyHistPadR[m_const.nVtx]  = {0.0,  0.33, 1.0,  1.0};
  const float    xyRatPadR1[m_const.nVtx]  = {0.0,  0.0,  1.0,  0.33};
  const float    xyRatPadR2[m_const.nVtx]  = {0.66, 0.0,  1.0,  1.0};

  // make 1d plot without ratio
  for (size_t iHistRow1D = 0; iHistRow1D < nHistRows1D; iHistRow1D++) {
    for (size_t iHist1D = 0; iHist1D < nHistTypes1D; iHist1D++) {

      // make new name
      const string sName = vecCanvasNames1D[iHistRow1D][iHist1D] + "_" + sLabel;


      // construct canvas
      TCanvas* cPlot1D = new TCanvas(sName.data(), "", width1D, height);
      cPlot1D -> SetGrid(fGrid, fGrid);
      cPlot1D -> SetTicks(fTick, fTick);
      cPlot1D -> SetBorderMode(fMode);
      cPlot1D -> SetBorderSize(fBord);
      cPlot1D -> SetFrameBorderMode(fFrame);
      cPlot1D -> SetTopMargin(fMargin1D[0]);
      cPlot1D -> SetRightMargin(fMargin1D[1]);
      cPlot1D -> SetBottomMargin(fMargin1D[2]);
      cPlot1D -> SetLeftMargin(fMargin1D[3]);
      cPlot1D -> SetLogx(fLogX);
      cPlot1D -> SetLogy(fLogY);
      cPlot1D -> cd();

      // draw old vs. new histograms
      m_vecOldHists1D[iHistRow1D][iHist1D] -> Draw();
      vecNewHists1D[iHistRow1D][iHist1D]   -> Draw("same");

      // draw text and save
      leg                    -> Draw();
      txt                    -> Draw();
      m_vecPlotDirs.at(iDir) -> cd();
      cPlot1D                -> Write();
      cPlot1D                -> Close();
    }
  }

  // make 1d plot with ratio
  for (size_t iHistRow1D = 0; iHistRow1D < nHistRows1D; iHistRow1D++) {
    for (size_t iHist1D = 0; iHist1D < nHistTypes1D; iHist1D++) {

      // make new name
      const string sNameWithRatio = vecCanvasNames1D[iHistRow1D][iHist1D] + "WithRatio_" + sLabel;

      // construct canvas
      TCanvas* cPlot1D = new TCanvas(sNameWithRatio.data(), "", width1D, heightR1);
      TPad*    pPadH1D = new TPad(sHistPadName.data(), "", xyHistPadR[0], xyHistPadR[1], xyHistPadR[2], xyHistPadR[3]);
      TPad*    pPadR1D = new TPad(sRatPadName.data(),  "", xyRatPadR1[0], xyRatPadR1[1], xyRatPadR1[2], xyRatPadR1[3]);
      cPlot1D -> SetGrid(fGrid, fGrid);
      cPlot1D -> SetTicks(fTick, fTick);
      cPlot1D -> SetBorderMode(fMode);
      cPlot1D -> SetBorderSize(fBord);
      pPadH1D -> SetGrid(fGrid, fGrid);
      pPadH1D -> SetTicks(fTick, fTick);
      pPadH1D -> SetBorderMode(fMode);
      pPadH1D -> SetBorderSize(fBord);
      pPadH1D -> SetFrameBorderMode(fFrame);
      pPadH1D -> SetTopMargin(fMargin1DH[0]);
      pPadH1D -> SetRightMargin(fMargin1DH[1]);
      pPadH1D -> SetBottomMargin(fMargin1DH[2]);
      pPadH1D -> SetLeftMargin(fMargin1DH[3]);
      pPadH1D -> SetLogx(fLogX);
      pPadH1D -> SetLogy(fLogY);
      pPadR1D -> SetGrid(fGrid, fGrid);
      pPadR1D -> SetTicks(fTick, fTick);
      pPadR1D -> SetBorderMode(fMode);
      pPadR1D -> SetBorderSize(fBord);
      pPadR1D -> SetFrameBorderMode(fFrame);
      pPadR1D -> SetTopMargin(fMargin1DR[0]);
      pPadR1D -> SetRightMargin(fMargin1DR[1]);
      pPadR1D -> SetBottomMargin(fMargin1DR[2]);
      pPadR1D -> SetLeftMargin(fMargin1DR[3]);
      pPadR1D -> SetLogx(fLogX);
      pPadR1D -> SetLogy(fLogY);
      cPlot1D -> cd();
      pPadH1D -> Draw();
      pPadR1D -> Draw();

      // draw old vs. new histograms
      pPadH1D                              -> cd();
      m_vecOldHists1D[iHistRow1D][iHist1D] -> Draw();
      vecNewHists1D[iHistRow1D][iHist1D]   -> Draw("same");
      leg                                  -> Draw();
      txt                                  -> Draw();

      // draw ratio
      pPadR1D                          -> cd();
      vecRatios1D[iHistRow1D][iHist1D] -> Draw();

      // draw text and save
      m_vecPlotDirs.at(iDir) -> cd();
      cPlot1D                -> Write();
      cPlot1D                -> Close();
    }
  }

  // make 2d plot without ratio
  for (size_t iHistRow2D = 0; iHistRow2D < nHistRows2D; iHistRow2D++) {
    for (size_t iHistType2D = 0; iHistType2D < nHistTypes2D; iHistType2D++) {
      for (size_t iHist2D = 0; iHist2D < nHist2D; iHist2D++) {

        // construct canvas
        TCanvas* cPlot2D = new TCanvas(vecCanvasNames2D[iHistRow2D][iHistType2D][iHist2D].data(), "", width2D, height);
        TPad*    pOld    = new TPad(sOldPadName.data(), "", xyOldPad[0], xyOldPad[1], xyOldPad[2], xyOldPad[3]);
        TPad*    pNew    = new TPad(sNewPadName.data(), "", xyNewPad[0], xyNewPad[1], xyNewPad[2], xyNewPad[3]);
        cPlot2D -> SetGrid(fGrid, fGrid);
        cPlot2D -> SetTicks(fTick, fTick);
        cPlot2D -> SetBorderMode(fMode);
        cPlot2D -> SetBorderSize(fBord);
        pOld    -> SetGrid(fGrid, fGrid);
        pOld    -> SetTicks(fTick, fTick);
        pOld    -> SetBorderMode(fMode);
        pOld    -> SetBorderSize(fBord);
        pOld    -> SetFrameBorderMode(fFrame);
        pOld    -> SetTopMargin(fMargin2D[0]);
        pOld    -> SetRightMargin(fMargin2D[1]);
        pOld    -> SetBottomMargin(fMargin2D[2]);
        pOld    -> SetLeftMargin(fMargin2D[3]);
        pOld    -> SetLogx(fLogX);
        pOld    -> SetLogy(fLogY);
        pOld    -> SetLogz(fLogZ);
        pNew    -> SetGrid(fGrid, fGrid);
        pNew    -> SetTicks(fTick, fTick);
        pNew    -> SetBorderMode(fMode);
        pNew    -> SetBorderSize(fBord);
        pNew    -> SetFrameBorderMode(fFrame);
        pNew    -> SetTopMargin(fMargin2D[0]);
        pNew    -> SetRightMargin(fMargin2D[1]);
        pNew    -> SetBottomMargin(fMargin2D[2]);
        pNew    -> SetLeftMargin(fMargin2D[3]);
        pNew    -> SetLogx(fLogX);
        pNew    -> SetLogy(fLogY);
        pNew    -> SetLogz(fLogZ);
        cPlot2D -> cd();
        pOld    -> Draw();
        pNew    -> Draw();

        // draw old vs. new histograms
        pOld                                              -> cd();
        m_vecOldHists2D[iHistRow2D][iHistType2D][iHist2D] -> Draw("colz");
        pNew                                              -> cd();
        vecNewHists2D[iHistRow2D][iHistType2D][iHist2D]   -> Draw("colz");

        // draw text and save
        pNew                   -> cd();
        txt                    -> Draw();
        m_vecPlotDirs.at(iDir) -> cd();
        cPlot2D                -> Write();
        cPlot2D                -> Close();
      }
    }
  }

  // make 2d plot with ratio
  for (size_t iHistRow2D = 0; iHistRow2D < nHistRows2D; iHistRow2D++) {
    for (size_t iHistType2D = 0; iHistType2D < nHistTypes2D; iHistType2D++) {
      for (size_t iHist2D = 0; iHist2D < nHist2D; iHist2D++) {

        // make new name
        const string sNameWithRatio = vecCanvasNames2D[iHistRow2D][iHistType2D][iHist2D] + "_" + sLabel;

        // construct canvas
        TCanvas* cPlot2D = new TCanvas(sNameWithRatio.data(), "", width2DR, heightR2);
        TPad*    pOld    = new TPad(sOldPadName.data(), "", xyOldPadR[0],  xyOldPadR[1],  xyOldPadR[2],  xyOldPadR[3]);
        TPad*    pNew    = new TPad(sNewPadName.data(), "", xyNewPadR[0],  xyNewPadR[1],  xyNewPadR[2],  xyNewPadR[3]);
        TPad*    pRat    = new TPad(sRatPadName.data(), "", xyRatPadR2[0], xyRatPadR2[1], xyRatPadR2[2], xyRatPadR2[3]);
        cPlot2D -> SetGrid(fGrid, fGrid);
        cPlot2D -> SetTicks(fTick, fTick);
        cPlot2D -> SetBorderMode(fMode);
        cPlot2D -> SetBorderSize(fBord);
        pOld    -> SetGrid(fGrid, fGrid);
        pOld    -> SetTicks(fTick, fTick);
        pOld    -> SetBorderMode(fMode);
        pOld    -> SetBorderSize(fBord);
        pOld    -> SetFrameBorderMode(fFrame);
        pOld    -> SetTopMargin(fMargin2D[0]);
        pOld    -> SetRightMargin(fMargin2D[1]);
        pOld    -> SetBottomMargin(fMargin2D[2]);
        pOld    -> SetLeftMargin(fMargin2D[3]);
        pOld    -> SetLogx(fLogX);
        pOld    -> SetLogy(fLogY);
        pOld    -> SetLogz(fLogZ);
        pNew    -> SetGrid(fGrid, fGrid);
        pNew    -> SetTicks(fTick, fTick);
        pNew    -> SetBorderMode(fMode);
        pNew    -> SetBorderSize(fBord);
        pNew    -> SetFrameBorderMode(fFrame);
        pNew    -> SetTopMargin(fMargin2D[0]);
        pNew    -> SetRightMargin(fMargin2D[1]);
        pNew    -> SetBottomMargin(fMargin2D[2]);
        pNew    -> SetLeftMargin(fMargin2D[3]);
        pNew    -> SetLogx(fLogX);
        pNew    -> SetLogy(fLogY);
        pNew    -> SetLogz(fLogZ);
        pRat    -> SetGrid(fGrid, fGrid);
        pRat    -> SetTicks(fTick, fTick);
        pRat    -> SetBorderMode(fMode);
        pRat    -> SetBorderSize(fBord);
        pRat    -> SetFrameBorderMode(fFrame);
        pRat    -> SetTopMargin(fMargin2D[0]);
        pRat    -> SetRightMargin(fMargin2D[1]);
        pRat    -> SetBottomMargin(fMargin2D[2]);
        pRat    -> SetLeftMargin(fMargin2D[3]);
        pRat    -> SetLogx(fLogX);
        pRat    -> SetLogy(fLogY);
        pRat    -> SetLogz(fLogZ);
        cPlot2D -> cd();
        pOld    -> Draw();
        pNew    -> Draw();
        pRat    -> Draw();

        // draw old vs. new vs. ratio histograms
        pOld                                              -> cd();
        m_vecOldHists2D[iHistRow2D][iHistType2D][iHist2D] -> Draw("colz");
        txt                                               -> Draw();
        pNew                                              -> cd();
        vecNewHists2D[iHistRow2D][iHistType2D][iHist2D]   -> Draw("colz");
        pRat                                              -> cd();
        vecRatios2D[iHistRow2D][iHistType2D][iHist2D]     -> Draw("colz");

        // draw text and save
        m_vecPlotDirs.at(iDir) -> cd();
        cPlot2D                -> Write();
        cPlot2D                -> Close();
      }
    }
  }
  cout << "        Made 2D plot with ratio." << endl;

  // save ratio histograms
  m_vecRatioDirs.at(iDir) -> cd();
  for (auto histRow1D : vecRatios1D) {
    for (auto hRatio1D : histRow1D) {
      hRatio1D -> Write();
    }
  }
  for (auto histRow2D : vecRatios2D) {
    for (auto histTypes2D : histRow2D) {
      for (auto hRatio2D : histTypes2D) {
        hRatio2D -> Write();
      }
    } 
  }

  // announce end and return
  cout << "       Saved ratio histograms.\n"
       << "      Finished making ratios and plots."
       << endl;
  return;

}  // end 'MakeRatiosAndPlots(vector<TH1D*>, vector<TH2D*>, int, string)'



void STrackMatcherComparator::SaveHistograms() {

  // save 1d histograms
  for (size_t iHistRow = 0; iHistRow < m_vecTreeHists1D.size(); iHistRow++) {
    for (size_t iHistType = 0; iHistType < m_vecTreeHists1D[iHistRow].size(); iHistType++) {
      m_vecHistDirs[Src::NewTree]            -> cd();
      m_vecTreeHists1D[iHistRow][iHistType]  -> Write();
      m_vecHistDirs[Src::NewTuple]           -> cd(); 
      m_vecTupleHists1D[iHistRow][iHistType] -> Write();
      m_vecHistDirs[Src::OldTuple]           -> cd();
      m_vecOldHists1D[iHistRow][iHistType]   -> Write();
    }  // end type loop
  }  // end row loop
  cout << "      Saved 1d histograms." << endl;

  // save 2d histograms
  for (size_t iHistRow = 0; iHistRow < m_vecTreeHists2D.size(); iHistRow++) {
    for (size_t iHistType = 0; iHistType < m_vecTreeHists2D[iHistRow].size(); iHistType++) {
      for (size_t iVsHistMod = 0; iVsHistMod < m_vecTreeHists2D[iHistRow][iHistType].size(); iVsHistMod++) {
        m_vecHistDirs[Src::NewTree]                        -> cd();
        m_vecTreeHists2D[iHistRow][iHistType][iVsHistMod]  -> Write();
        m_vecHistDirs[Src::NewTuple]                       -> cd(); 
        m_vecTupleHists2D[iHistRow][iHistType][iVsHistMod] -> Write();
        m_vecHistDirs[Src::OldTuple]                       -> cd();
        m_vecOldHists2D[iHistRow][iHistType][iVsHistMod]   -> Write();
      }  // end modifier loop
    }  // end type loop
  }  // end row loop

  // announce end and return
  cout << "      Saved 2d histograms." << endl;
  return;

}  // end 'SaveHistograms()'



void STrackMatcherComparator::CloseInput() {

  m_treeInFileTrue  -> cd();
  m_treeInFileTrue  -> Close();
  m_treeInFileReco  -> cd();
  m_treeInFileReco  -> Close();
  m_tupleInFileTrue -> cd();
  m_tupleInFileTrue -> Close();
  m_tupleInFileReco -> cd();
  m_tupleInFileReco -> Close();
  m_oldInFileTrue   -> cd();
  m_oldInFileTrue   -> Close();
  m_oldInFileReco   -> cd();
  m_oldInFileReco   -> Close();

  // announce end and return
  cout << "      Closed input files." << endl;
  return;

}  // end 'CloseInput()'



void STrackMatcherComparator::CloseOutput() {

  m_outFile -> cd();
  m_outFile -> Close();

  // announce end and return
  cout << "      Closed output file." << endl;
  return;

}  // end 'CloseOutput()'

// end ------------------------------------------------------------------------

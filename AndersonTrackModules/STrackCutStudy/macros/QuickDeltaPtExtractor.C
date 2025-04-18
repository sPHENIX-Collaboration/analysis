// ----------------------------------------------------------------------------
// 'QuickDeltaPtExtractor.C'
// Derek Anderson
// 04.25.2023
//
// Quickly apply cuts to and extract plots of the track
// DeltaPt/Pt from the track evaluator.
// ----------------------------------------------------------------------------

// standard c includes
#include <iostream>
// root includes
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TPad.h"
#include "TFile.h"
#include "TLine.h"
#include "TGraph.h"
#include "TError.h"
#include "TString.h"
#include "TNtuple.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TDirectory.h"

using namespace std;

// global constants
static const Ssiz_t  NTxt     = 3;
static const Ssiz_t  NPad     = 2;
static const Ssiz_t  NPar     = 3;
static const Ssiz_t  NVtx     = 4;
static const Ssiz_t  NRange   = 2;
static const Ssiz_t  NProj    = 8;
static const Ssiz_t  NTrkCuts = 6;
static const Ssiz_t  NDPtCuts = 7;
static const Ssiz_t  NSigCuts = 5;
static const TString SInTrack = "ntp_track";
static const TString SInTruth = "ntp_gtrack";

// default parameters
static const TString SInDef  = "input/embed_only/final_merge/sPhenixG4_forPtCheck_embedScanOn_embedOnly.pt2040n100pim.d8m5y2023.root";
static const TString SOutDef = "varyDeltaPtCut.withPtDependentDeltaPtCut.pt2040n100pim.d10m5y2023.root";



void QuickDeltaPtExtractor(const TString sInput = SInDef, const TString sOutput = SOutDef) {

  // lower verbosity
  gErrorIgnoreLevel = kError;
  cout << "\n  Beginning delta-pt extractor script..." << endl;

  // cut parameters
  const UInt_t   nInttTrkMin          = 1;
  const UInt_t   nMVtxTrkMin          = 2;
  const UInt_t   nTpcTrkMin           = 35;
  const Double_t qualTrkMax           = 10.;
  const Double_t vzTrkMax             = 10.;
  const Double_t ptTrkMin             = 0.1;
  const Double_t ptDeltaMax[NDPtCuts] = {0.5, 0.25, 0.1, 0.05, 0.03, 0.02, 0.01};
  const Double_t ptDeltaSig[NSigCuts] = {1.,  1.5,  2.,  2.5,  3.};
  const Double_t normRange[NRange]    = {0.2, 1.2};

  // histogram parameters
  const TString sPtTrueBase          = "PtTrue";
  const TString sPtRecoBase          = "PtReco";
  const TString sPtFracBase          = "PtFrac";
  const TString sPtDeltaBase         = "DeltaPt";
  const TString sPtTrkTruBase        = "PtTrkTruth";
  const TString sPtProjBase          = "DeltaPtProj";
  const TString sRejCutBase          = "Reject_flatDPtCut";
  const TString sRejSigBase          = "Reject_sigmaCut";
  const TString sMuHiBase            = "MeanPlusSigma";
  const TString sMuLoBase            = "MeanMinusSigma";
  const TString sSigBase             = "ProjectionSigma";
  const TString sMuBase              = "ProjectionMean";
  const TString sEffBase             = "Efficiency";
  const TString sProjSuffix[NProj]   = {"_pt05",    "_pt1",      "_pt2",     "_pt5",      "_pt10",  "_pt20",  "_pt30", "_pt40"}; 
  const TString sDPtSuffix[NDPtCuts] = {"_dPt50",   "_dPt25",    "_dPt10",   "_dPt05",    "_dPt03", "_dPt02", "_dPt01"};
  const TString sSigSuffix[NSigCuts] = {"_sigDPt1", "_sidDpt15", "_sigDPt2", "_sigDPt25", "_sigDPt3"};

  // plot parameters
  const UInt_t  iCutToDraw     = NDPtCuts - 3;
  const UInt_t  iSigToDraw     = NSigCuts - 3;
  const UInt_t  nEffRebin      = 5;
  const Bool_t  doEffRebin     = true;
  const TString sBeforeTitle   = "Before #Deltap_{T}/p_{T} cuts";
  const TString sAfterCutTitle = "After #Deltap_{T}/p_{T} < 0.03 cut";
  const TString sAfterSigTitle = "After 2 #times #sigma(#Deltap_{T}/p_{T}) cut";

  // histogram text parameters
  const TString sTitle        = "";
  const TString sCounts       = "counts";
  const TString sPtTrueAxis   = "p_{T}^{true} [GeV/c]";
  const TString sPtRecoAxis   = "p_{T}^{reco} [GeV/c]";
  const TString sPtFracAxis   = "p_{T}^{reco} / p_{T}^{true}";
  const TString sPtDeltaAxis  = "#Deltap_{T} / p_{T}^{reco}";
  const TString sDeltaCutAxis = "max #Deltap_{T} / p_{T}^{reco}";
  const TString sSigmaCutAxis = "n #times #sigma(#Deltap_{T} / p_{T}^{reco})";
  const TString sSigProjAxis  = "#sigma(#Deltap_{T} / p_{T}^{reco})";
  const TString sMuProjAxis   = "#mu(#Deltap_{T} / p_{T}^{reco}) #pm (n #times #sigma(#Deltap_{T} / p_{T}^{reco}))";
  const TString sRejectAxis   = "rejection factor";
  const TString sEffAxis      = "#epsilon_{trk}";

  // sigma calculation parameters
  const Double_t ptProj[NProj]         = {0.5, 1., 2., 5., 10., 20., 30., 40.};
  const Double_t sigHiGuess[NPar]      = {1., -1., 1.};
  const Double_t sigLoGuess[NPar]      = {1., -1., 1.};
  const Double_t deltaFitRange[NRange] = {0.,  0.1};
  const Double_t ptFitRange[NRange]    = {0.5, 40.};

  // histogram style parameters
  const UInt_t  fColTrue(923);
  const UInt_t  fColPure(923);
  const UInt_t  fColTrk(809);
  const UInt_t  fMarTrue(20);
  const UInt_t  fMarPure(20);
  const UInt_t  fMarTrk(46);
  const UInt_t  fColProj[NProj]     = {799, 633, 899, 617, 879, 859, 839, 819};
  const UInt_t  fMarProj[NProj]     = {20,  22,  23,  21,  33,  34,  47,  20};
  const UInt_t  fColCut[NDPtCuts]   = {899, 909, 879, 889, 859, 869, 839};
  const UInt_t  fMarCut[NDPtCuts]   = {24,  26,  32,  25,  27,  28,  30};
  const Float_t rPtRange[NRange]    = {0., 60.};
  const Float_t rFracRange[NRange]  = {0., 4.};
  const Float_t rDeltaRange[NRange] = {0., 0.1};

  // graph/fit style parameters
  const UInt_t fColFit[NProj]       = {803, 636, 893, 620, 883, 863, 843, 813};
  const UInt_t fColSigFit[NSigCuts] = {893, 903, 873, 883, 863};
  const UInt_t fColSig[NSigCuts]    = {899, 909, 879, 889, 859};
  const UInt_t fMarSig[NSigCuts]    = {24,  26,  32,  25,  27};

  // legend parameters
  const TString sLegTrue("truth");
  const TString sLegTrack("tracks (w/ cuts)");
  const TString sLegMu("Mean #Deltap_{T} / p_{T}^{reco} (n = 0)");
  const TString sInfo[NTxt] = {
    "#bf{#it{sPHENIX}} Simulation",
    "100 #pi^{-}/event, p_{T} #in (20, 40) GeV/c",
    "#bf{Only #pi^{-}}"
  };
  const TString sLegProj[NProj] = {
    "p_{T}^{reco} = 0.5 GeV/c",
    "p_{T}^{reco} = 1 GeV/c",
    "p_{T}^{reco} = 2 GeV/c",
    "p_{T}^{reco} = 5 GeV/c",
    "p_{T}^{reco} = 10 GeV/c",
    "p_{T}^{reco} = 20 GeV/c",
    "p_{T}^{reco} = 30 GeV/c",
    "p_{T}^{reco} = 40 GeV/c"
  };
  const TString sLegCut[NDPtCuts] = {
    "#Deltap_{T} / p_{T}^{reco} < 0.5",
    "#Deltap_{T} / p_{T}^{reco} < 0.25",
    "#Deltap_{T} / p_{T}^{reco} < 0.1",
    "#Deltap_{T} / p_{T}^{reco} < 0.05",
    "#Deltap_{T} / p_{T}^{reco} < 0.03",
    "#Deltap_{T} / p_{T}^{reco} < 0.02",
    "#Deltap_{T} / p_{T}^{reco} < 0.01"
  };
  const TString sLegProjSig[NSigCuts] = {
    "n = 1",
    "n = 1.5",
    "n = 2",
    "n = 2.5",
    "n = 3"
  };
  const TString sLegDelta[NSigCuts] = {
    "1 #times #sigma(#Deltap_{T} / p_{T}^{reco})",
    "1.5 #times #sigma(#Deltap_{T} / p_{T}^{reco})",
    "2 #times #sigma(#Deltap_{T} / p_{T}^{reco})",
    "2.5 #times #sigma(#Deltap_{T} / p_{T}^{reco})",
    "3 #times #sigma(#Deltap_{T} / p_{T}^{reco})"
  };
  const TString sLegSig[NSigCuts] = {
    "#Deltap_{T} / p_{T}^{reco} #in 1 #times sigma(#Deltap_{T} / p_{T}^{reco})",
    "#Deltap_{T} / p_{T}^{reco} #in 1.5 #times sigma(#Deltap_{T} / p_{T}^{reco})",
    "#Deltap_{T} / p_{T}^{reco} #in 2 #times sigma(#Deltap_{T} / p_{T}^{reco})",
    "#Deltap_{T} / p_{T}^{reco} #in 2.5 #times sigma(#Deltap_{T} / p_{T}^{reco})",
    "#Deltap_{T} / p_{T}^{reco} #in 3 #times sigma(#Deltap_{T} / p_{T}^{reco})"
  };
  const TString sTrkCuts[NTrkCuts] = {
    "|v_{z}| < 10 cm",
    "N_{hit}^{intt} #geq 1",
    "N_{hit}^{mvtx} > 2",
    "N_{hit}^{tpc} > 35",
    "p_{T}^{reco} > 0.1 GeV/c",
    "quality < 10"
  };

  // open files
  TFile *fOutput = new TFile(sOutput.Data(), "recreate");
  TFile *fInput  = new TFile(sInput.Data(),  "read");
  if (!fInput || !fOutput) {
    cerr << "PANIC: couldn't open a file!\n"
         << "       fInput  = " << fInput  << "\n"
         << "       fOutput = " << fOutput << "\n"
         << endl;
    return;
  }
  cout << "    Opened files." << endl;

  // grab input tuples
  TNtuple *ntTrack = (TNtuple*) fInput -> Get(SInTrack.Data());
  TNtuple *ntTruth = (TNtuple*) fInput -> Get(SInTruth.Data());
  if (!ntTrack || !ntTruth) {
    cerr << "PANIC: couldn't grab aninput tuple!\n"
         << "       ntTrack = " << ntTrack << "\n"
         << "       ntTruth = " << ntTruth << "\n"
         << endl;
    return;
  }
  cout << "    Grabbed input tuples." << endl;

  // declare track tuple addresses
  Float_t trk_event;
  Float_t trk_seed;
  Float_t trk_trackID;
  Float_t trk_crossing;
  Float_t trk_px;
  Float_t trk_py;
  Float_t trk_pz;
  Float_t trk_pt;
  Float_t trk_eta;
  Float_t trk_phi;
  Float_t trk_deltapt;
  Float_t trk_deltaeta;
  Float_t trk_deltaphi;
  Float_t trk_charge;
  Float_t trk_quality;
  Float_t trk_chisq;
  Float_t trk_ndf;
  Float_t trk_nhits;
  Float_t trk_nmaps;
  Float_t trk_nintt;
  Float_t trk_ntpc;
  Float_t trk_nmms;
  Float_t trk_ntpc1;
  Float_t trk_ntpc11;
  Float_t trk_ntpc2;
  Float_t trk_ntpc3;
  Float_t trk_nlmaps;
  Float_t trk_nlintt;
  Float_t trk_nltpc;
  Float_t trk_nlmms;
  Float_t trk_layers;
  Float_t trk_vertexID;
  Float_t trk_vx;
  Float_t trk_vy;
  Float_t trk_vz;
  Float_t trk_dca2d;
  Float_t trk_dca2dsigma;
  Float_t trk_dca3dxy;
  Float_t trk_dca3dxysigma;
  Float_t trk_dca3dz;
  Float_t trk_dca3dzsigma;
  Float_t trk_pcax;
  Float_t trk_pcay;
  Float_t trk_pcaz;
  Float_t trk_gtrackID;
  Float_t trk_gflavor;
  Float_t trk_gnhits;
  Float_t trk_gnmaps;
  Float_t trk_gnintt;
  Float_t trk_gntpc;
  Float_t trk_gnmms;
  Float_t trk_gnlmaps;
  Float_t trk_gnlintt;
  Float_t trk_gnltpc;
  Float_t trk_gnlmms;
  Float_t trk_gpx;
  Float_t trk_gpy;
  Float_t trk_gpz;
  Float_t trk_gpt;
  Float_t trk_geta;
  Float_t trk_gphi;
  Float_t trk_gvx;
  Float_t trk_gvy;
  Float_t trk_gvz;
  Float_t trk_gvt;
  Float_t trk_gfpx;
  Float_t trk_gfpy;
  Float_t trk_gfpz;
  Float_t trk_gfx;
  Float_t trk_gfy;
  Float_t trk_gfz;
  Float_t trk_gembed;
  Float_t trk_gprimary;
  Float_t trk_nfromtruth;
  Float_t trk_nwrong;
  Float_t trk_ntrumaps;
  Float_t trk_ntruintt;
  Float_t trk_ntrutpc;
  Float_t trk_ntrumms;
  Float_t trk_ntrutpc1;
  Float_t trk_ntrutpc11;
  Float_t trk_ntrutpc2;
  Float_t trk_ntrutpc3;
  Float_t trk_layersfromtruth;
  Float_t trk_nhittpcall;
  Float_t trk_nhittpcin;
  Float_t trk_nhittpcmid;
  Float_t trk_nhittpcout;
  Float_t trk_nclusall;
  Float_t trk_nclustpc;
  Float_t trk_nclusintt;
  Float_t trk_nclusmaps;
  Float_t trk_nclusmms;

  // declare truth tuple addresses
  Float_t tru_event;
  Float_t tru_seed;
  Float_t tru_gntracks;
  Float_t tru_gtrackID;
  Float_t tru_gflavor;
  Float_t tru_gnhits;
  Float_t tru_gnmaps;
  Float_t tru_gnintt;
  Float_t tru_gnmms;
  Float_t tru_gnintt1;
  Float_t tru_gnintt2;
  Float_t tru_gnintt3;
  Float_t tru_gnintt4;
  Float_t tru_gnintt5;
  Float_t tru_gnintt6;
  Float_t tru_gnintt7;
  Float_t tru_gnintt8;
  Float_t tru_gntpc;
  Float_t tru_gnlmaps;
  Float_t tru_gnlintt;
  Float_t tru_gnltpc;
  Float_t tru_gnlmms;
  Float_t tru_gpx;
  Float_t tru_gpy;
  Float_t tru_gpz;
  Float_t tru_gpt;
  Float_t tru_geta;
  Float_t tru_gphi;
  Float_t tru_gvx;
  Float_t tru_gvy;
  Float_t tru_gvz;
  Float_t tru_gvt;
  Float_t tru_gfpx;
  Float_t tru_gfpy;
  Float_t tru_gfpz;
  Float_t tru_gfx;
  Float_t tru_gfy;
  Float_t tru_gfz;
  Float_t tru_gembed;
  Float_t tru_gprimary;
  Float_t tru_trackID;
  Float_t tru_px;
  Float_t tru_py;
  Float_t tru_pz;
  Float_t tru_pt;
  Float_t tru_eta;
  Float_t tru_phi;
  Float_t tru_deltapt;
  Float_t tru_deltaeta;
  Float_t tru_deltaphi;
  Float_t tru_charge;
  Float_t tru_quality;
  Float_t tru_chisq;
  Float_t tru_ndf;
  Float_t tru_nhits;
  Float_t tru_layers;
  Float_t tru_nmaps;
  Float_t tru_nintt;
  Float_t tru_ntpc;
  Float_t tru_nmms;
  Float_t tru_ntpc1;
  Float_t tru_ntpc11;
  Float_t tru_ntpc2;
  Float_t tru_ntpc3;
  Float_t tru_nlmaps;
  Float_t tru_nlintt;
  Float_t tru_nltpc;
  Float_t tru_nlmms;
  Float_t tru_vertexID;
  Float_t tru_vx;
  Float_t tru_vy;
  Float_t tru_vz;
  Float_t tru_dca2d;
  Float_t tru_dca2dsigma;
  Float_t tru_dca3dxy;
  Float_t tru_dca3dxysigma;
  Float_t tru_dca3dz;
  Float_t tru_dca3dzsigma;
  Float_t tru_pcax;
  Float_t tru_pcay;
  Float_t tru_pcaz;
  Float_t tru_nfromtruth;
  Float_t tru_nwrong;
  Float_t tru_ntrumaps;
  Float_t tru_ntruintt;
  Float_t tru_ntrutpc;
  Float_t tru_ntrumms;
  Float_t tru_ntrutpc1;
  Float_t tru_ntrutpc11;
  Float_t tru_ntrutpc2;
  Float_t tru_ntrutpc3;
  Float_t tru_layersfromtruth;
  Float_t tru_nhittpcall;
  Float_t tru_nhittpcin;
  Float_t tru_nhittpcmid;
  Float_t tru_nhittpcout;
  Float_t tru_nclusall;
  Float_t tru_nclustpc;
  Float_t tru_nclusintt;
  Float_t tru_nclusmaps;
  Float_t tru_nclusmms;

  // set track branch addresses
  ntTrack -> SetBranchAddress("event",           &trk_event);
  ntTrack -> SetBranchAddress("seed",            &trk_seed);
  ntTrack -> SetBranchAddress("trackID",         &trk_trackID);
  ntTrack -> SetBranchAddress("crossing",        &trk_crossing);
  ntTrack -> SetBranchAddress("px",              &trk_px);
  ntTrack -> SetBranchAddress("py",              &trk_py);
  ntTrack -> SetBranchAddress("pz",              &trk_pz);
  ntTrack -> SetBranchAddress("pt",              &trk_pt);
  ntTrack -> SetBranchAddress("eta",             &trk_eta);
  ntTrack -> SetBranchAddress("phi",             &trk_phi);
  ntTrack -> SetBranchAddress("deltapt",         &trk_deltapt);
  ntTrack -> SetBranchAddress("deltaeta",        &trk_deltaeta);
  ntTrack -> SetBranchAddress("deltaphi",        &trk_deltaphi);
  ntTrack -> SetBranchAddress("charge",          &trk_charge);
  ntTrack -> SetBranchAddress("quality",         &trk_quality);
  ntTrack -> SetBranchAddress("chisq",           &trk_chisq);
  ntTrack -> SetBranchAddress("ndf",             &trk_ndf);
  ntTrack -> SetBranchAddress("nhits",           &trk_nhits);
  ntTrack -> SetBranchAddress("nmaps",           &trk_nmaps);
  ntTrack -> SetBranchAddress("nintt",           &trk_nintt);
  ntTrack -> SetBranchAddress("ntpc",            &trk_ntpc);
  ntTrack -> SetBranchAddress("nmms",            &trk_nmms);
  ntTrack -> SetBranchAddress("ntpc1",           &trk_ntpc1);
  ntTrack -> SetBranchAddress("ntpc11",          &trk_ntpc11);
  ntTrack -> SetBranchAddress("ntpc2",           &trk_ntpc2);
  ntTrack -> SetBranchAddress("ntpc3",           &trk_ntpc3);
  ntTrack -> SetBranchAddress("nlmaps",          &trk_nlmaps);
  ntTrack -> SetBranchAddress("nlintt",          &trk_nlintt);
  ntTrack -> SetBranchAddress("nltpc",           &trk_nltpc);
  ntTrack -> SetBranchAddress("nlmms",           &trk_nlmms);
  ntTrack -> SetBranchAddress("layers",          &trk_layers);
  ntTrack -> SetBranchAddress("vertexID",        &trk_vertexID);
  ntTrack -> SetBranchAddress("vx",              &trk_vx);
  ntTrack -> SetBranchAddress("vy",              &trk_vy);
  ntTrack -> SetBranchAddress("vz",              &trk_vz);
  ntTrack -> SetBranchAddress("dca2d",           &trk_dca2d);
  ntTrack -> SetBranchAddress("dca2dsigma",      &trk_dca2dsigma);
  ntTrack -> SetBranchAddress("dca3dxy",         &trk_dca3dxy);
  ntTrack -> SetBranchAddress("dca3dxysigma",    &trk_dca3dxysigma);
  ntTrack -> SetBranchAddress("dca3dz",          &trk_dca3dz);
  ntTrack -> SetBranchAddress("dca3dzsigma",     &trk_dca3dzsigma);
  ntTrack -> SetBranchAddress("pcax",            &trk_pcax);
  ntTrack -> SetBranchAddress("pcay",            &trk_pcay);
  ntTrack -> SetBranchAddress("pcaz",            &trk_pcaz);
  ntTrack -> SetBranchAddress("gtrackID",        &trk_gtrackID);
  ntTrack -> SetBranchAddress("gflavor",         &trk_gflavor);
  ntTrack -> SetBranchAddress("gnhits",          &trk_gnhits);
  ntTrack -> SetBranchAddress("gnmaps",          &trk_gnmaps);
  ntTrack -> SetBranchAddress("gnintt",          &trk_gnintt);
  ntTrack -> SetBranchAddress("gntpc",           &trk_gntpc);
  ntTrack -> SetBranchAddress("gnmms",           &trk_gnmms);
  ntTrack -> SetBranchAddress("gnlmaps",         &trk_gnlmaps);
  ntTrack -> SetBranchAddress("gnlintt",         &trk_gnlintt);
  ntTrack -> SetBranchAddress("gnltpc",          &trk_gnltpc);
  ntTrack -> SetBranchAddress("gnlmms",          &trk_gnlmms);
  ntTrack -> SetBranchAddress("gpx",             &trk_gpx);
  ntTrack -> SetBranchAddress("gpy",             &trk_gpy);
  ntTrack -> SetBranchAddress("gpz",             &trk_gpz);
  ntTrack -> SetBranchAddress("gpt",             &trk_gpt);
  ntTrack -> SetBranchAddress("geta",            &trk_geta);
  ntTrack -> SetBranchAddress("gphi",            &trk_gphi);
  ntTrack -> SetBranchAddress("gvx",             &trk_gvx);
  ntTrack -> SetBranchAddress("gvy",             &trk_gvy);
  ntTrack -> SetBranchAddress("gvz",             &trk_gvz);
  ntTrack -> SetBranchAddress("gvt",             &trk_gvt);
  ntTrack -> SetBranchAddress("gfpx",            &trk_gfpx);
  ntTrack -> SetBranchAddress("gfpy",            &trk_gfpy);
  ntTrack -> SetBranchAddress("gfpz",            &trk_gfpz);
  ntTrack -> SetBranchAddress("gfx",             &trk_gfx);
  ntTrack -> SetBranchAddress("gfy",             &trk_gfy);
  ntTrack -> SetBranchAddress("gfz",             &trk_gfz);
  ntTrack -> SetBranchAddress("gembed",          &trk_gembed);
  ntTrack -> SetBranchAddress("gprimary",        &trk_gprimary);
  ntTrack -> SetBranchAddress("nfromtruth",      &trk_nfromtruth);
  ntTrack -> SetBranchAddress("nwrong",          &trk_nwrong);
  ntTrack -> SetBranchAddress("ntrumaps",        &trk_ntrumaps);
  ntTrack -> SetBranchAddress("ntruintt",        &trk_ntruintt);
  ntTrack -> SetBranchAddress("ntrutpc",         &trk_ntrutpc);
  ntTrack -> SetBranchAddress("ntrumms",         &trk_ntrumms);
  ntTrack -> SetBranchAddress("ntrutpc1",        &trk_ntrutpc1);
  ntTrack -> SetBranchAddress("ntrutpc11",       &trk_ntrutpc11);
  ntTrack -> SetBranchAddress("ntrutpc2",        &trk_ntrutpc2);
  ntTrack -> SetBranchAddress("ntrutpc3",        &trk_ntrutpc3);
  ntTrack -> SetBranchAddress("layersfromtruth", &trk_layersfromtruth);
  ntTrack -> SetBranchAddress("nhittpcall",      &trk_nhittpcall);
  ntTrack -> SetBranchAddress("nhittpcin",       &trk_nhittpcin);
  ntTrack -> SetBranchAddress("nhittpcmid",      &trk_nhittpcmid);
  ntTrack -> SetBranchAddress("nhittpcout",      &trk_nhittpcout);
  ntTrack -> SetBranchAddress("nclusall",        &trk_nclusall);
  ntTrack -> SetBranchAddress("nclustpc",        &trk_nclustpc);
  ntTrack -> SetBranchAddress("nclusintt",       &trk_nclusintt);
  ntTrack -> SetBranchAddress("nclusmaps",       &trk_nclusmaps);
  ntTrack -> SetBranchAddress("nclusmms",        &trk_nclusmms);

  // Set branch addresses.
  ntTruth -> SetBranchAddress("event",           &tru_event);
  ntTruth -> SetBranchAddress("seed",            &tru_seed);
  ntTruth -> SetBranchAddress("gntracks",        &tru_gntracks);
  ntTruth -> SetBranchAddress("gtrackID",        &tru_gtrackID);
  ntTruth -> SetBranchAddress("gflavor",         &tru_gflavor);
  ntTruth -> SetBranchAddress("gnhits",          &tru_gnhits);
  ntTruth -> SetBranchAddress("gnmaps",          &tru_gnmaps);
  ntTruth -> SetBranchAddress("gnintt",          &tru_gnintt);
  ntTruth -> SetBranchAddress("gnmms",           &tru_gnmms);
  ntTruth -> SetBranchAddress("gnintt1",         &tru_gnintt1);
  ntTruth -> SetBranchAddress("gnintt2",         &tru_gnintt2);
  ntTruth -> SetBranchAddress("gnintt3",         &tru_gnintt3);
  ntTruth -> SetBranchAddress("gnintt4",         &tru_gnintt4);
  ntTruth -> SetBranchAddress("gnintt5",         &tru_gnintt5);
  ntTruth -> SetBranchAddress("gnintt6",         &tru_gnintt6);
  ntTruth -> SetBranchAddress("gnintt7",         &tru_gnintt7);
  ntTruth -> SetBranchAddress("gnintt8",         &tru_gnintt8);
  ntTruth -> SetBranchAddress("gntpc",           &tru_gntpc);
  ntTruth -> SetBranchAddress("gnlmaps",         &tru_gnlmaps);
  ntTruth -> SetBranchAddress("gnlintt",         &tru_gnlintt);
  ntTruth -> SetBranchAddress("gnltpc",          &tru_gnltpc);
  ntTruth -> SetBranchAddress("gnlmms",          &tru_gnlmms);
  ntTruth -> SetBranchAddress("gpx",             &tru_gpx);
  ntTruth -> SetBranchAddress("gpy",             &tru_gpy);
  ntTruth -> SetBranchAddress("gpz",             &tru_gpz);
  ntTruth -> SetBranchAddress("gpt",             &tru_gpt);
  ntTruth -> SetBranchAddress("geta",            &tru_geta);
  ntTruth -> SetBranchAddress("gphi",            &tru_gphi);
  ntTruth -> SetBranchAddress("gvx",             &tru_gvx);
  ntTruth -> SetBranchAddress("gvy",             &tru_gvy);
  ntTruth -> SetBranchAddress("gvz",             &tru_gvz);
  ntTruth -> SetBranchAddress("gvt",             &tru_gvt);
  ntTruth -> SetBranchAddress("gfpx",            &tru_gfpx);
  ntTruth -> SetBranchAddress("gfpy",            &tru_gfpy);
  ntTruth -> SetBranchAddress("gfpz",            &tru_gfpz);
  ntTruth -> SetBranchAddress("gfx",             &tru_gfx);
  ntTruth -> SetBranchAddress("gfy",             &tru_gfy);
  ntTruth -> SetBranchAddress("gfz",             &tru_gfz);
  ntTruth -> SetBranchAddress("gembed",          &tru_gembed);
  ntTruth -> SetBranchAddress("gprimary",        &tru_gprimary);
  ntTruth -> SetBranchAddress("trackID",         &tru_trackID);
  ntTruth -> SetBranchAddress("px",              &tru_px);
  ntTruth -> SetBranchAddress("py",              &tru_py);
  ntTruth -> SetBranchAddress("pz",              &tru_pz);
  ntTruth -> SetBranchAddress("pt",              &tru_pt);
  ntTruth -> SetBranchAddress("eta",             &tru_eta);
  ntTruth -> SetBranchAddress("phi",             &tru_phi);
  ntTruth -> SetBranchAddress("deltapt",         &tru_deltapt);
  ntTruth -> SetBranchAddress("deltaeta",        &tru_deltaeta);
  ntTruth -> SetBranchAddress("deltaphi",        &tru_deltaphi);
  ntTruth -> SetBranchAddress("charge",          &tru_charge);
  ntTruth -> SetBranchAddress("quality",         &tru_quality);
  ntTruth -> SetBranchAddress("chisq",           &tru_chisq);
  ntTruth -> SetBranchAddress("ndf",             &tru_ndf);
  ntTruth -> SetBranchAddress("nhits",           &tru_nhits);
  ntTruth -> SetBranchAddress("layers",          &tru_layers);
  ntTruth -> SetBranchAddress("nmaps",           &tru_nmaps);
  ntTruth -> SetBranchAddress("nintt",           &tru_nintt);
  ntTruth -> SetBranchAddress("ntpc",            &tru_ntpc);
  ntTruth -> SetBranchAddress("nmms",            &tru_nmms);
  ntTruth -> SetBranchAddress("ntpc1",           &tru_ntpc1);
  ntTruth -> SetBranchAddress("ntpc11",          &tru_ntpc11);
  ntTruth -> SetBranchAddress("ntpc2",           &tru_ntpc2);
  ntTruth -> SetBranchAddress("ntpc3",           &tru_ntpc3);
  ntTruth -> SetBranchAddress("nlmaps",          &tru_nlmaps);
  ntTruth -> SetBranchAddress("nlintt",          &tru_nlintt);
  ntTruth -> SetBranchAddress("nltpc",           &tru_nltpc);
  ntTruth -> SetBranchAddress("nlmms",           &tru_nlmms);
  ntTruth -> SetBranchAddress("vertexID",        &tru_vertexID);
  ntTruth -> SetBranchAddress("vx",              &tru_vx);
  ntTruth -> SetBranchAddress("vy",              &tru_vy);
  ntTruth -> SetBranchAddress("vz",              &tru_vz);
  ntTruth -> SetBranchAddress("dca2d",           &tru_dca2d);
  ntTruth -> SetBranchAddress("dca2dsigma",      &tru_dca2dsigma);
  ntTruth -> SetBranchAddress("dca3dxy",         &tru_dca3dxy);
  ntTruth -> SetBranchAddress("dca3dxysigma",    &tru_dca3dxysigma);
  ntTruth -> SetBranchAddress("dca3dz",          &tru_dca3dz);
  ntTruth -> SetBranchAddress("dca3dzsigma",     &tru_dca3dzsigma);
  ntTruth -> SetBranchAddress("pcax",            &tru_pcax);
  ntTruth -> SetBranchAddress("pcay",            &tru_pcay);
  ntTruth -> SetBranchAddress("pcaz",            &tru_pcaz);
  ntTruth -> SetBranchAddress("nfromtruth",      &tru_nfromtruth);
  ntTruth -> SetBranchAddress("nwrong",          &tru_nwrong);
  ntTruth -> SetBranchAddress("ntrumaps",        &tru_ntrumaps);
  ntTruth -> SetBranchAddress("ntruintt",        &tru_ntruintt);
  ntTruth -> SetBranchAddress("ntrutpc",         &tru_ntrutpc);
  ntTruth -> SetBranchAddress("ntrumms",         &tru_ntrumms);
  ntTruth -> SetBranchAddress("ntrutpc1",        &tru_ntrutpc1);
  ntTruth -> SetBranchAddress("ntrutpc11",       &tru_ntrutpc11);
  ntTruth -> SetBranchAddress("ntrutpc2",        &tru_ntrutpc2);
  ntTruth -> SetBranchAddress("ntrutpc3",        &tru_ntrutpc3);
  ntTruth -> SetBranchAddress("layersfromtruth", &tru_layersfromtruth);
  ntTruth -> SetBranchAddress("nhittpcall",      &tru_nhittpcall);
  ntTruth -> SetBranchAddress("nhittpcin",       &tru_nhittpcin);
  ntTruth -> SetBranchAddress("nhittpcmid",      &tru_nhittpcmid);
  ntTruth -> SetBranchAddress("nhittpcout",      &tru_nhittpcout);
  ntTruth -> SetBranchAddress("nclusall",        &tru_nclusall);
  ntTruth -> SetBranchAddress("nclustpc",        &tru_nclustpc);
  ntTruth -> SetBranchAddress("nclusintt",       &tru_nclusintt);
  ntTruth -> SetBranchAddress("nclusmaps",       &tru_nclusmaps);
  ntTruth -> SetBranchAddress("nclusmms",        &tru_nclusmms);
  cout << "    Set track tuple branches." << endl;

  // declare histograms
  TH1D *hEff;
  TH1D *hPtTruth;
  TH1D *hPtDelta;
  TH1D *hPtTrack;
  TH1D *hPtFrac;
  TH1D *hPtTrkTru;
  TH1D *hPtDeltaProj[NProj];
  TH1D *hPtDeltaCut[NDPtCuts];
  TH1D *hPtDeltaSig[NSigCuts];
  TH1D *hPtTrackCut[NDPtCuts];
  TH1D *hPtTrackSig[NSigCuts];
  TH1D *hPtFracCut[NDPtCuts];
  TH1D *hPtFracSig[NSigCuts];
  TH1D *hPtTrkTruCut[NDPtCuts];
  TH1D *hPtTrkTruSig[NSigCuts];
  TH1D *hEffCut[NDPtCuts];
  TH1D *hEffSig[NSigCuts];

  TH2D *hPtDeltaVsFrac;
  TH2D *hPtDeltaVsTrue;
  TH2D *hPtDeltaVsTrack;
  TH2D *hPtTrueVsTrack;
  TH2D *hPtDeltaVsFracCut[NDPtCuts];
  TH2D *hPtDeltaVsFracSig[NSigCuts];
  TH2D *hPtDeltaVsTrueCut[NDPtCuts];
  TH2D *hPtDeltaVsTrueSig[NSigCuts];
  TH2D *hPtDeltaVsTrackCut[NDPtCuts];
  TH2D *hPtDeltaVsTrackSig[NSigCuts];
  TH2D *hPtTrueVsTrackCut[NDPtCuts];
  TH2D *hPtTrueVsTrackSig[NSigCuts];

  // histogram binning
  const UInt_t  nPtBins(1000);
  const UInt_t  nFracBins(1000);
  const UInt_t  nDeltaBins(5000);
  const Float_t rPtBins[NRange]    = {0., 100.};
  const Float_t rFracBins[NRange]  = {0., 10.};
  const Float_t rDeltaBins[NRange] = {0., 5.};

  // create names
  TString sPtTruth("h");
  TString sPtDelta("h");
  TString sPtTrack("h");
  TString sPtFrac("h");
  TString sPtTrkTru("h");
  sPtTruth.Append(sPtTrueBase.Data());
  sPtDelta.Append(sPtDeltaBase.Data());
  sPtTrack.Append(sPtRecoBase.Data());
  sPtFrac.Append(sPtFracBase.Data());
  sPtTrkTru.Append(sPtTrkTruBase.Data());

  TString sPtDeltaVsFrac("h");
  TString sPtDeltaVsTrue("h");
  TString sPtDeltaVsTrack("h");
  TString sPtTrueVsTrack("h");
  sPtDeltaVsFrac.Append(sPtDeltaBase.Data());
  sPtDeltaVsTrue.Append(sPtDeltaBase.Data());
  sPtDeltaVsTrack.Append(sPtDeltaBase.Data());
  sPtTrueVsTrack.Append(sPtTrueBase.Data());
  sPtDeltaVsFrac.Append("Vs");
  sPtDeltaVsTrue.Append("Vs");
  sPtDeltaVsTrack.Append("Vs");
  sPtTrueVsTrack.Append("Vs");
  sPtDeltaVsFrac.Append(sPtFracBase.Data());
  sPtDeltaVsTrue.Append(sPtTrueBase.Data());
  sPtDeltaVsTrack.Append(sPtRecoBase.Data());
  sPtTrueVsTrack.Append(sPtRecoBase.Data());

  // delta-pt projection names
  TString sPtProj[NProj];
  for (Ssiz_t iProj = 0; iProj < NProj; iProj++) {
    sPtProj[iProj] = "h";
    sPtProj[iProj].Append(sPtProjBase.Data());
    sPtProj[iProj].Append(sProjSuffix[iProj].Data());
  }

  // flat delta-pt cut names
  TString sPtDeltaCut[NDPtCuts];
  TString sPtTrackCut[NDPtCuts];
  TString sPtFracCut[NDPtCuts];
  TString sPtTrkTruCut[NDPtCuts];
  TString sPtDeltaVsFracCut[NDPtCuts];
  TString sPtDeltaVsTrueCut[NDPtCuts];
  TString sPtDeltaVsTrackCut[NDPtCuts];
  TString sPtTrueVsTrackCut[NDPtCuts];
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    sPtDeltaCut[iCut]  = "h";
    sPtTrackCut[iCut]  = "h";
    sPtFracCut[iCut]   = "h";
    sPtTrkTruCut[iCut] = "h";
    sPtDeltaCut[iCut].Append(sPtDeltaBase.Data());
    sPtTrackCut[iCut].Append(sPtRecoBase.Data());
    sPtFracCut[iCut].Append(sPtFracBase.Data());
    sPtTrkTruCut[iCut].Append(sPtTrkTruBase.Data());
    sPtDeltaCut[iCut].Append(sDPtSuffix[iCut].Data());
    sPtTrackCut[iCut].Append(sDPtSuffix[iCut].Data());
    sPtFracCut[iCut].Append(sDPtSuffix[iCut].Data());
    sPtTrkTruCut[iCut].Append(sDPtSuffix[iCut].Data());

    sPtDeltaVsFracCut[iCut]  = "h";
    sPtDeltaVsTrueCut[iCut]  = "h";
    sPtDeltaVsTrackCut[iCut] = "h";
    sPtTrueVsTrackCut[iCut]  = "h";
    sPtDeltaVsFracCut[iCut].Append(sPtDeltaBase.Data());
    sPtDeltaVsFracCut[iCut].Append(sPtDeltaBase.Data());
    sPtDeltaVsTrueCut[iCut].Append(sPtDeltaBase.Data());
    sPtDeltaVsTrackCut[iCut].Append(sPtDeltaBase.Data());
    sPtTrueVsTrackCut[iCut].Append(sPtTrueBase.Data());
    sPtDeltaVsFracCut[iCut].Append("Vs");
    sPtDeltaVsTrueCut[iCut].Append("Vs");
    sPtDeltaVsTrackCut[iCut].Append("Vs");
    sPtTrueVsTrackCut[iCut].Append("Vs");
    sPtDeltaVsFracCut[iCut].Append(sPtFracBase.Data());
    sPtDeltaVsTrueCut[iCut].Append(sPtTrueBase.Data());
    sPtDeltaVsTrackCut[iCut].Append(sPtRecoBase.Data());
    sPtTrueVsTrackCut[iCut].Append(sPtRecoBase.Data());
    sPtDeltaVsFracCut[iCut].Append(sDPtSuffix[iCut].Data());
    sPtDeltaVsTrueCut[iCut].Append(sDPtSuffix[iCut].Data());
    sPtDeltaVsTrackCut[iCut].Append(sDPtSuffix[iCut].Data());
    sPtTrueVsTrackCut[iCut].Append(sDPtSuffix[iCut].Data());
  }

  // pt-dependent delta-pt cut names
  TString sPtDeltaSig[NSigCuts];
  TString sPtTrackSig[NSigCuts];
  TString sPtFracSig[NSigCuts];
  TString sPtTrkTruSig[NSigCuts];
  TString sPtDeltaVsFracSig[NSigCuts];
  TString sPtDeltaVsTrueSig[NSigCuts];
  TString sPtDeltaVsTrackSig[NSigCuts];
  TString sPtTrueVsTrackSig[NSigCuts];
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    sPtDeltaSig[iSig]  = "h";
    sPtTrackSig[iSig]  = "h";
    sPtFracSig[iSig]   = "h";
    sPtTrkTruSig[iSig] = "h";
    sPtDeltaSig[iSig].Append(sPtDeltaBase.Data());
    sPtTrackSig[iSig].Append(sPtRecoBase.Data());
    sPtFracSig[iSig].Append(sPtFracBase.Data());
    sPtTrkTruSig[iSig].Append(sPtTrkTruBase.Data());
    sPtDeltaSig[iSig].Append(sSigSuffix[iSig].Data());
    sPtTrackSig[iSig].Append(sSigSuffix[iSig].Data());
    sPtFracSig[iSig].Append(sSigSuffix[iSig].Data());
    sPtTrkTruSig[iSig].Append(sSigSuffix[iSig].Data());

    sPtDeltaVsFracSig[iSig]  = "h";
    sPtDeltaVsTrueSig[iSig]  = "h";
    sPtDeltaVsTrackSig[iSig] = "h";
    sPtTrueVsTrackSig[iSig]  = "h";
    sPtDeltaVsFracSig[iSig].Append(sPtDeltaBase.Data());
    sPtDeltaVsFracSig[iSig].Append(sPtDeltaBase.Data());
    sPtDeltaVsTrueSig[iSig].Append(sPtDeltaBase.Data());
    sPtDeltaVsTrackSig[iSig].Append(sPtDeltaBase.Data());
    sPtTrueVsTrackSig[iSig].Append(sPtTrueBase.Data());
    sPtDeltaVsFracSig[iSig].Append("Vs");
    sPtDeltaVsTrueSig[iSig].Append("Vs");
    sPtDeltaVsTrackSig[iSig].Append("Vs");
    sPtTrueVsTrackSig[iSig].Append("Vs");
    sPtDeltaVsFracSig[iSig].Append(sPtFracBase.Data());
    sPtDeltaVsTrueSig[iSig].Append(sPtTrueBase.Data());
    sPtDeltaVsTrackSig[iSig].Append(sPtRecoBase.Data());
    sPtTrueVsTrackSig[iSig].Append(sPtRecoBase.Data());
    sPtDeltaVsFracSig[iSig].Append(sSigSuffix[iSig].Data());
    sPtDeltaVsTrueSig[iSig].Append(sSigSuffix[iSig].Data());
    sPtDeltaVsTrackSig[iSig].Append(sSigSuffix[iSig].Data());
    sPtTrueVsTrackSig[iSig].Append(sSigSuffix[iSig].Data());
  }

  // initialize histograms
  hPtTruth  = new TH1D(sPtTruth.Data(),  "", nPtBins,    rPtBins[0],    rPtBins[1]);
  hPtDelta  = new TH1D(sPtDelta.Data(),  "", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
  hPtTrack  = new TH1D(sPtTrack.Data(),  "", nPtBins,    rPtBins[0],    rPtBins[1]);
  hPtFrac   = new TH1D(sPtFrac.Data(),   "", nFracBins,  rFracBins[0],  rFracBins[1]);
  hPtTrkTru = new TH1D(sPtTrkTru.Data(), "", nPtBins,    rPtBins[0],    rPtBins[1]);
  hPtTruth  -> Sumw2();
  hPtDelta  -> Sumw2();
  hPtTrack  -> Sumw2();
  hPtFrac   -> Sumw2();
  hPtTrkTru -> Sumw2();

  hPtDeltaVsFrac  = new TH2D(sPtDeltaVsFrac.Data(),  "", nFracBins, rFracBins[0], rFracBins[1], nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
  hPtDeltaVsTrue  = new TH2D(sPtDeltaVsTrue.Data(),  "", nPtBins,   rPtBins[0],   rPtBins[1],   nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
  hPtDeltaVsTrack = new TH2D(sPtDeltaVsTrack.Data(), "", nPtBins,   rPtBins[0],   rPtBins[1],   nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
  hPtTrueVsTrack  = new TH2D(sPtTrueVsTrack.Data(),  "", nPtBins,   rPtBins[0],   rPtBins[1],   nPtBins,    rPtBins[0],    rPtBins[1]);
  hPtDeltaVsFrac  -> Sumw2();
  hPtDeltaVsTrue  -> Sumw2();
  hPtDeltaVsTrack -> Sumw2();
  hPtTrueVsTrack  -> Sumw2();

  // delta-pt projection histograms
  for (Ssiz_t iProj = 0; iProj < NProj; iProj++) {
    hPtDeltaProj[iProj] = new TH1D(sPtProj[iProj].Data(), "", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    hPtDeltaProj[iProj] -> Sumw2();
  }

  // flat delta-pt cut histograms
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    hPtDeltaCut[iCut]  = new TH1D(sPtDeltaCut[iCut].Data(),  "", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    hPtTrackCut[iCut]  = new TH1D(sPtTrackCut[iCut].Data(),  "", nPtBins,    rPtBins[0],    rPtBins[1]);
    hPtFracCut[iCut]   = new TH1D(sPtFracCut[iCut].Data(),   "", nFracBins,  rFracBins[0],  rFracBins[1]);
    hPtTrkTruCut[iCut] = new TH1D(sPtTrkTruCut[iCut].Data(), "", nPtBins,    rPtBins[0],    rPtBins[1]);
    hPtDeltaCut[iCut]  -> Sumw2();
    hPtTrackCut[iCut]  -> Sumw2();
    hPtFracCut[iCut]   -> Sumw2();
    hPtTrkTruCut[iCut] -> Sumw2();

    hPtDeltaVsFracCut[iCut]  = new TH2D(sPtDeltaVsFracCut[iCut].Data(),  "", nFracBins, rFracBins[0], rFracBins[1], nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    hPtDeltaVsTrueCut[iCut]  = new TH2D(sPtDeltaVsTrueCut[iCut].Data(),  "", nPtBins,   rPtBins[0],   rPtBins[1],   nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    hPtDeltaVsTrackCut[iCut] = new TH2D(sPtDeltaVsTrackCut[iCut].Data(), "", nPtBins,   rPtBins[0],   rPtBins[1],   nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    hPtTrueVsTrackCut[iCut]  = new TH2D(sPtTrueVsTrackCut[iCut].Data(),  "", nPtBins,   rPtBins[0],   rPtBins[1],   nPtBins,    rPtBins[0],    rPtBins[1]);
    hPtDeltaVsFracCut[iCut]  -> Sumw2();
    hPtDeltaVsTrueCut[iCut]  -> Sumw2();
    hPtDeltaVsTrackCut[iCut] -> Sumw2();
    hPtTrueVsTrackCut[iCut]  -> Sumw2();
  }

  // pt-dependent delta-pt cut histograms
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    hPtDeltaSig[iSig]  = new TH1D(sPtDeltaSig[iSig].Data(),  "", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    hPtTrackSig[iSig]  = new TH1D(sPtTrackSig[iSig].Data(),  "", nPtBins,    rPtBins[0],    rPtBins[1]);
    hPtFracSig[iSig]   = new TH1D(sPtFracSig[iSig].Data(),   "", nFracBins,  rFracBins[0],  rFracBins[1]);
    hPtTrkTruSig[iSig] = new TH1D(sPtTrkTruSig[iSig].Data(), "", nPtBins,    rPtBins[0],    rPtBins[1]);
    hPtDeltaSig[iSig]  -> Sumw2();
    hPtTrackSig[iSig]  -> Sumw2();
    hPtFracSig[iSig]   -> Sumw2();
    hPtTrkTruSig[iSig] -> Sumw2();

    hPtDeltaVsFracSig[iSig]  = new TH2D(sPtDeltaVsFracSig[iSig].Data(),  "", nFracBins, rFracBins[0], rFracBins[1], nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    hPtDeltaVsTrueSig[iSig]  = new TH2D(sPtDeltaVsTrueSig[iSig].Data(),  "", nPtBins,   rPtBins[0],   rPtBins[1],   nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    hPtDeltaVsTrackSig[iSig] = new TH2D(sPtDeltaVsTrackSig[iSig].Data(), "", nPtBins,   rPtBins[0],   rPtBins[1],   nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    hPtTrueVsTrackSig[iSig]  = new TH2D(sPtTrueVsTrackSig[iSig].Data(),  "", nPtBins,   rPtBins[0],   rPtBins[1],   nPtBins,    rPtBins[0],    rPtBins[1]);
    hPtDeltaVsFracSig[iSig]  -> Sumw2();
    hPtDeltaVsTrueSig[iSig]  -> Sumw2();
    hPtDeltaVsTrackSig[iSig] -> Sumw2();
    hPtTrueVsTrackSig[iSig]  -> Sumw2();
  }

  // grab no. of entries
  const Long64_t nTrks = ntTrack -> GetEntries();
  const Long64_t nTrus = ntTruth -> GetEntries();
  cout << "    Beginning tuple loops: " << nTrks << " reco. tracks and " << nTrus << " particles to process" << endl;

  // for sigma calculatin
  Double_t muProj[NProj];
  Double_t sigProj[NProj];
  Double_t muHiProj[NSigCuts][NProj];
  Double_t muLoProj[NSigCuts][NProj];
  for (Ssiz_t iProj = 0; iProj < NProj; iProj++) {
    muProj[iProj]  = 0.;
    sigProj[iProj] = 0.;
  }
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    for (Ssiz_t iProj = 0; iProj < NProj; iProj++) {
      muHiProj[iSig][iProj] = 0.;
      muLoProj[iSig][iProj] = 0.;
    }
  }

  // for reject calculation
  UInt_t   nNormCut[NDPtCuts];
  UInt_t   nNormSig[NSigCuts];
  UInt_t   nWeirdCut[NDPtCuts];
  UInt_t   nWeirdSig[NSigCuts];
  Double_t rejCut[NDPtCuts];
  Double_t rejSig[NSigCuts];
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    nNormCut[iCut]  = 0;
    nWeirdCut[iCut] = 0;
    rejCut[iCut]    = 0.;
  }
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    nNormSig[iSig]  = 0;
    nWeirdSig[iSig] = 0;
    rejSig[iSig]    = 0.;
  }
  cout << "      First loop over reco. tracks:" << endl;

  // 1st track loop
  Long64_t nBytesTrk = 0;
  for (Long64_t iTrk = 0; iTrk < nTrks; iTrk++) {

    // grab entry
    const Long64_t bytesTrk = ntTrack -> GetEntry(iTrk);
    if (bytesTrk < 0.) {
      cerr << "WARNING: something wrong with track #" << iTrk << "! Aborting loop!" << endl;
      break;
    }
    nBytesTrk += bytesTrk;

    // announce progress
    const Long64_t iProgTrk = iTrk + 1;
    //if (iProgTrk == nTrks) {
    //  cout << "        Processing track " << iProgTrk << "/" << nTrks << "..." << endl;
    //} else {
    //  cout << "        Processing track " << iProgTrk << "/" << nTrks << "...\r" << flush;
    //}

    // do calculations
    const Double_t ptFrac  = trk_pt / trk_gpt;
    const Double_t ptDelta = trk_deltapt / trk_pt;

    // apply trk cuts
    const Bool_t isInZVtxCut = (abs(trk_vz) <  vzTrkMax);
    const Bool_t isInInttCut = (trk_nintt   >= nInttTrkMin);
    const Bool_t isInMVtxCut = (trk_nlmaps  >  nMVtxTrkMin);
    const Bool_t isInTpcCut  = (trk_ntpc    >  nTpcTrkMin);
    const Bool_t isInPtCut   = (trk_pt      >  ptTrkMin);
    const Bool_t isInQualCut = (trk_quality <  qualTrkMax);
    const Bool_t isGoodTrk   = (isInZVtxCut && isInInttCut && isInMVtxCut && isInTpcCut && isInPtCut && isInQualCut);
    if (!isGoodTrk) continue;

    if (trk_pt < 20) {
      cout << "CHECK intt = " << trk_nintt << ", mvtx = " << trk_nlmaps << ", tpc = " << trk_ntpc << ", quality = " << trk_quality << ", ptTrue = " << trk_gpt
           << ", dcaXY = " << trk_dca3dxy << ", dcaZ = " << trk_dca3dz << endl;
    }

    // fill histograms
    hPtDelta        -> Fill(ptDelta);
    hPtTrack        -> Fill(trk_pt);
    hPtFrac         -> Fill(ptFrac);
    hPtTrkTru       -> Fill(trk_gpt);
    hPtDeltaVsFrac  -> Fill(ptFrac,  ptDelta);
    hPtDeltaVsTrue  -> Fill(trk_gpt, ptDelta);
    hPtDeltaVsTrack -> Fill(trk_pt,  ptDelta);
    hPtTrueVsTrack  -> Fill(trk_pt,  trk_gpt);

    // apply delta-pt cuts
    const Bool_t isNormalTrk = ((ptFrac > normRange[0]) && (ptFrac < normRange[1]));
    for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
      const Bool_t isInDeltaPtCut = (ptDelta < ptDeltaMax[iCut]);
      if (isInDeltaPtCut) {

        // fill histograms
        hPtDeltaCut[iCut]        -> Fill(ptDelta);
        hPtTrackCut[iCut]        -> Fill(trk_pt);
        hPtFracCut[iCut]         -> Fill(ptFrac);
        hPtTrkTruCut[iCut]       -> Fill(trk_gpt);
        hPtDeltaVsFracCut[iCut]  -> Fill(ptFrac,  ptDelta);
        hPtDeltaVsTrueCut[iCut]  -> Fill(trk_gpt, ptDelta);
        hPtDeltaVsTrackCut[iCut] -> Fill(trk_pt,  ptDelta);
        hPtTrueVsTrackCut[iCut]  -> Fill(trk_pt,  trk_gpt);

        // increment counters
        if (isNormalTrk) {
          ++nNormCut[iCut];
        } else {
          ++nWeirdCut[iCut];
        }
      }
    }  // end delta-pt cut
  }  // end 1st track loop
  cout << "      First loop over reco. tracks finished!" << endl;

  // calculate flat delta-pt rejection factors
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    rejCut[iCut] = (Double_t) nNormCut[iCut] / (Double_t) nWeirdCut[iCut];
  }
  cout << "      Calculated flat delta-pt rejection factors." << endl;

  // projection fit names
  TString sFitProj[NProj];
  for (Ssiz_t iProj = 0; iProj < NProj; iProj++) {
    sFitProj[iProj] = "f";
    sFitProj[iProj].Append(sPtProjBase.Data());
    sFitProj[iProj].Append(sProjSuffix[iProj].Data());
  }

  // project slices of delta-pt and get sigmas
  const UInt_t fWidFit = 2;
  const UInt_t fLinFit = 1;

  TF1 *fPtDeltaProj[NProj];
  for (Ssiz_t iProj = 0; iProj < NProj; iProj++) {

    // do projection
    const UInt_t iBinProj = hPtDeltaVsTrack -> GetXaxis() -> FindBin(ptProj[iProj]);
    hPtDeltaProj[iProj]   = hPtDeltaVsTrack -> ProjectionY(sPtProj[iProj], iBinProj, iBinProj, "");

    // get initial values for fit
    const Float_t ampGuess = hPtDeltaProj[iProj] -> GetMaximum();
    const Float_t muGuess  = hPtDeltaProj[iProj] -> GetMean();
    const Float_t sigGuess = hPtDeltaProj[iProj] -> GetRMS();

    // fit with gaussian
    fPtDeltaProj[iProj] = new TF1(sFitProj[iProj].Data(), "gaus", deltaFitRange[0], deltaFitRange[1]);
    fPtDeltaProj[iProj] -> SetLineColor(fColFit[iProj]);
    fPtDeltaProj[iProj] -> SetLineStyle(fLinFit);
    fPtDeltaProj[iProj] -> SetLineWidth(fWidFit);
    fPtDeltaProj[iProj] -> SetParameter(0, ampGuess);
    fPtDeltaProj[iProj] -> SetParameter(1, muGuess);
    fPtDeltaProj[iProj] -> SetParameter(2, sigGuess);
    hPtDeltaProj[iProj] -> Fit(sFitProj[iProj].Data(), "R");

    // add values to arrays
    muProj[iProj]  = fPtDeltaProj[iProj] -> GetParameter(1);
    sigProj[iProj] = fPtDeltaProj[iProj] -> GetParameter(2);
    for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
      muHiProj[iSig][iProj] = muProj[iProj] + (ptDeltaSig[iSig] * sigProj[iProj]);
      muLoProj[iSig][iProj] = muProj[iProj] - (ptDeltaSig[iSig] * sigProj[iProj]);
    }
  }  // end projection loop
  cout << "      Obtained delta-pt projections, fits, and sigmas." << endl;

  // sigma graph names
  TString sMuProj("gr");
  TString sSigProj("gr");
  sMuProj.Append(sMuBase.Data());
  sSigProj.Append(sSigBase.Data());

  TString sGrMuHiProj[NSigCuts];
  TString sGrMuLoProj[NSigCuts];
  TString sFnMuHiProj[NSigCuts];
  TString sFnMuLoProj[NSigCuts];
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    sGrMuHiProj[iSig] = "gr";
    sGrMuLoProj[iSig] = "gr";
    sFnMuHiProj[iSig] = "f";
    sFnMuLoProj[iSig] = "f";
    sGrMuHiProj[iSig].Append(sMuHiBase.Data());
    sGrMuLoProj[iSig].Append(sMuLoBase.Data());
    sFnMuHiProj[iSig].Append(sMuHiBase.Data());
    sFnMuLoProj[iSig].Append(sMuLoBase.Data());
    sGrMuHiProj[iSig].Append(sSigSuffix[iSig].Data());
    sGrMuLoProj[iSig].Append(sSigSuffix[iSig].Data());
    sFnMuHiProj[iSig].Append(sSigSuffix[iSig].Data());
    sFnMuLoProj[iSig].Append(sSigSuffix[iSig].Data());
  }

  // construct sigma graphs
  TGraph *grMuProj  = new TGraph(NProj, ptProj, muProj);
  TGraph *grSigProj = new TGraph(NProj, ptProj, sigProj);
  grMuProj  -> SetName(sMuProj);
  grSigProj -> SetName(sSigProj);

  TF1    *fMuHiProj[NSigCuts];
  TF1    *fMuLoProj[NSigCuts];
  TGraph *grMuHiProj[NSigCuts];
  TGraph *grMuLoProj[NSigCuts];
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {

    // create graphs
    grMuHiProj[iSig] = new TGraph(NProj, ptProj, muHiProj[iSig]);
    grMuLoProj[iSig] = new TGraph(NProj, ptProj, muLoProj[iSig]);
    grMuHiProj[iSig] -> SetName(sGrMuHiProj[iSig].Data());
    grMuLoProj[iSig] -> SetName(sGrMuLoProj[iSig].Data());

    // create fit functions
    fMuHiProj[iSig] = new TF1(sFnMuHiProj[iSig].Data(), "pol2", rPtRange[0], rPtRange[1]);
    fMuLoProj[iSig] = new TF1(sFnMuLoProj[iSig].Data(), "pol2", rPtRange[0], rPtRange[1]);
    fMuHiProj[iSig] -> SetLineColor(fColSigFit[iSig]);
    fMuLoProj[iSig] -> SetLineColor(fColSigFit[iSig]);
    fMuHiProj[iSig] -> SetLineStyle(fLinFit);
    fMuLoProj[iSig] -> SetLineStyle(fLinFit);
    fMuHiProj[iSig] -> SetLineWidth(fWidFit);
    fMuLoProj[iSig] -> SetLineWidth(fWidFit);
    fMuHiProj[iSig] -> SetParameter(0, sigHiGuess[0]);
    fMuLoProj[iSig] -> SetParameter(0, sigLoGuess[0]);
    fMuHiProj[iSig] -> SetParameter(1, sigHiGuess[1]);
    fMuLoProj[iSig] -> SetParameter(1, sigLoGuess[1]);
    fMuHiProj[iSig] -> SetParameter(2, sigHiGuess[2]);
    fMuLoProj[iSig] -> SetParameter(2, sigLoGuess[2]);

    // fit graphs
    grMuHiProj[iSig] -> Fit(sFnMuHiProj[iSig].Data(), "", "", ptFitRange[0], ptFitRange[1]);
    grMuLoProj[iSig] -> Fit(sFnMuLoProj[iSig].Data(), "", "", ptFitRange[0], ptFitRange[1]);
  }
  cout << "      Created and fit sigma graphs.\n"
       << "      Second loop over reco. tracks:"
       << endl;

  // 2nd track loop
  nBytesTrk = 0;
  for (Long64_t iTrk = 0; iTrk < nTrks; iTrk++) {

    // grab entry
    const Long64_t bytesTrk = ntTrack -> GetEntry(iTrk);
    if (bytesTrk < 0.) {
      cerr << "WARNING: something wrong with track #" << iTrk << "! Aborting loop!" << endl;
      break;
    }
    nBytesTrk += bytesTrk;

    // announce progress
    const Long64_t iProgTrk = iTrk + 1;
    if (iProgTrk == nTrks) {
      cout << "        Processing track " << iProgTrk << "/" << nTrks << "..." << endl;
    } else {
      cout << "        Processing track " << iProgTrk << "/" << nTrks << "...\r" << flush;
    }

    // do calculations
    const Double_t ptFrac  = trk_pt / trk_gpt;
    const Double_t ptDelta = trk_deltapt / trk_pt;

    // apply trk cuts
    const Bool_t isInZVtxCut = (abs(trk_vz) <  vzTrkMax);
    const Bool_t isInInttCut = (trk_nintt   >= nInttTrkMin);
    const Bool_t isInMVtxCut = (trk_nlmaps  >  nMVtxTrkMin);
    const Bool_t isInTpcCut  = (trk_ntpc    >  nTpcTrkMin);
    const Bool_t isInPtCut   = (trk_pt      >  ptTrkMin);
    const Bool_t isInQualCut = (trk_quality <  qualTrkMax);
    const Bool_t isGoodTrk   = (isInZVtxCut && isInInttCut && isInMVtxCut && isInTpcCut && isInPtCut && isInQualCut);
    if (!isGoodTrk) continue;

    // apply delta-pt cuts
    const Bool_t isNormalTrk = ((ptFrac > normRange[0]) && (ptFrac < normRange[1]));
    for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {

      // get bounds
      const Float_t ptDeltaMin = fMuLoProj[iSig] -> Eval(trk_pt);
      const Float_t ptDeltaMax = fMuHiProj[iSig] -> Eval(trk_pt);

      const Bool_t isInDeltaPtSigma = ((ptDelta >= ptDeltaMin) && (ptDelta <= ptDeltaMax));
      if (isInDeltaPtSigma) {

        // fill histograms
        hPtDeltaSig[iSig]        -> Fill(ptDelta);
        hPtTrackSig[iSig]        -> Fill(trk_pt);
        hPtFracSig[iSig]         -> Fill(ptFrac);
        hPtTrkTruSig[iSig]       -> Fill(trk_gpt);
        hPtDeltaVsFracSig[iSig]  -> Fill(ptFrac,  ptDelta);
        hPtDeltaVsTrueSig[iSig]  -> Fill(trk_gpt, ptDelta);
        hPtDeltaVsTrackSig[iSig] -> Fill(trk_pt,  ptDelta);
        hPtTrueVsTrackSig[iSig]  -> Fill(trk_pt,  trk_gpt);

        // increment counters
        if (isNormalTrk) {
          ++nNormSig[iSig];
        } else {
          ++nWeirdSig[iSig];
        }
      }
    }  // end delta-pt cut
  }  // end 1st track loop
  cout << "      Second loop over reco. tracks finished!" << endl;

  // calculate pt-dependent delta-pt rejection factors
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    rejSig[iSig] = (Double_t) nNormSig[iSig] / (Double_t) nWeirdSig[iSig];
  }
  cout << "      Calculated pt-depdendent delta-pt rejection factors.\n"
       << "      Loop over particles:"
       << endl;

  // truth loop
  Long64_t nBytesTru = 0;
  for (Long64_t iTru = 0; iTru < nTrus; iTru++) {

    // grab entry
    const Long64_t bytesTru = ntTruth -> GetEntry(iTru);
    if (bytesTru < 0.) {
      cerr << "WARNING: something wrong with particle #" << iTru << "! Aborting loop!" << endl;
      break;
    }
    nBytesTru += bytesTru;

    // announce progress
    const Long64_t iProgTru = iTru + 1;
    if (iProgTru == nTrus) {
      cout << "        Processing particle " << iProgTru << "/" << nTrus << "..." << endl;
    } else {
      cout << "        Processing particle" << iProgTru << "/" << nTrus << "...\r" << flush;
    }

    // fill truth histogram
    const Bool_t isPrimary = (tru_gprimary == 1);
    if (isPrimary) {
      hPtTruth -> Fill(tru_gpt);
    }
  }  // end track loop
  cout << "      Loop over particles finished!" << endl;

  // announce rejection factors
  cout << "    Finished tuple loops! Rejection factors:" << endl;

  // flat delta-t rejection factors
  cout << "      Flat delta-pt cuts" << endl;
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    cout << "        n(Norm, Weird) = (" << nNormCut[iCut] << ", " << nWeirdCut[iCut] << "), rejection = " << rejCut[iCut] << endl;
  }

  // flat delta-t rejection factors
  cout << "      Pt-dependent delta-pt cuts" << endl;
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    cout << "        n(Norm, Weird) = (" << nNormSig[iSig] << ", " << nWeirdSig[iSig] << "), rejection = " << rejSig[iSig] << endl;
  }

  // make rejection graphs
  TString sRejCut("gr");
  TString sRejSig("gr");
  sRejCut.Append(sRejCutBase.Data());
  sRejSig.Append(sRejSigBase.Data());

  TGraph *grRejCut = new TGraph(NDPtCuts, ptDeltaMax, rejCut);
  TGraph *grRejSig = new TGraph(NSigCuts, ptDeltaSig, rejSig);
  grRejCut -> SetName(sRejCut.Data());
  grRejSig -> SetName(sRejSig.Data());
  cout << "    Made rejection factor graph." << endl; 

  // rebin efficiency histograms if needed
  if (doEffRebin) {
    hPtTruth  -> Rebin(nEffRebin);
    hPtTrkTru -> Rebin(nEffRebin);
    for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
      hPtTrkTruCut[iCut] -> Rebin(nEffRebin);
    }
    for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
      hPtTrkTruSig[iSig] -> Rebin(nEffRebin);
    }
    cout << "    Rebinned efficiency histograms." << endl;
  }

  // calculate efficiencies
  TString sEff("h");
  sEff.Append(sEffBase.Data());

  // flat delta-pt cut efficiency names
  TString sEffCut[NDPtCuts];
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    sEffCut[iCut] = "h";
    sEffCut[iCut].Append(sEffBase.Data());
    sEffCut[iCut].Append(sDPtSuffix[iCut].Data());
  }

  // pt-dependent delta-pt cut efficiency names
  TString sEffSig[NSigCuts];
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    sEffSig[iSig] = "h";
    sEffSig[iSig].Append(sEffBase.Data());
    sEffSig[iSig].Append(sSigSuffix[iSig].Data());
  }

  hEff = (TH1D*) hPtTruth -> Clone();
  hEff -> SetName(sEff.Data());
  hEff -> Reset("ICES");
  hEff -> Divide(hPtTrkTru, hPtTruth, 1., 1.);

  // flat delta-pt cut efficiencies
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    hEffCut[iCut] = (TH1D*) hPtTruth -> Clone();
    hEffCut[iCut] -> SetName(sEffCut[iCut].Data());
    hEffCut[iCut] -> Reset("ICES");
    hEffCut[iCut] -> Divide(hPtTrkTruCut[iCut], hPtTruth, 1., 1.);
  }

  // pt-dependent delta-pt cut efficiencies
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    hEffSig[iSig] = (TH1D*) hPtTruth -> Clone();
    hEffSig[iSig] -> SetName(sEffSig[iSig].Data());
    hEffSig[iSig] -> Reset("ICES");
    hEffSig[iSig] -> Divide(hPtTrkTruSig[iSig], hPtTruth, 1., 1.);
  }
  cout << "    Calculated efficiencies." << endl;

  // set styles
  const UInt_t  fFil(0);
  const UInt_t  fLin(1);
  const UInt_t  fWid(1);
  const UInt_t  fTxt(42);
  const UInt_t  fAln(12);
  const UInt_t  fCnt(1);
  const Float_t fLab[NPad]  = {0.074, 0.04};
  const Float_t fTit[NPad]  = {0.074, 0.04};
  const Float_t fOffX[NPad] = {1.1,   1.};
  const Float_t fOffY[NPad] = {0.7,   1.3};
  const Float_t fOffZ[NPad] = {1.1,   1.1};
  grRejCut        -> SetMarkerColor(fColTrue);
  grRejCut        -> SetMarkerStyle(fMarTrue);
  grRejCut        -> SetFillColor(fColTrue);
  grRejCut        -> SetFillStyle(fFil);
  grRejCut        -> SetLineColor(fColTrue);
  grRejCut        -> SetLineStyle(fLin);
  grRejCut        -> SetLineWidth(fWid);
  grRejCut        -> SetTitle(sTitle.Data());
  grRejCut        -> GetXaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
  grRejCut        -> GetXaxis() -> SetTitle(sDeltaCutAxis.Data());
  grRejCut        -> GetXaxis() -> SetTitleFont(fTxt);
  grRejCut        -> GetXaxis() -> SetTitleSize(fTit[1]);
  grRejCut        -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  grRejCut        -> GetXaxis() -> SetLabelFont(fTxt);
  grRejCut        -> GetXaxis() -> SetLabelSize(fLab[1]);
  grRejCut        -> GetXaxis() -> CenterTitle(fCnt);
  grRejCut        -> GetYaxis() -> SetTitle(sRejectAxis.Data());
  grRejCut        -> GetYaxis() -> SetTitleFont(fTxt);
  grRejCut        -> GetYaxis() -> SetTitleSize(fTit[1]);
  grRejCut        -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  grRejCut        -> GetYaxis() -> SetLabelFont(fTxt);
  grRejCut        -> GetYaxis() -> SetLabelSize(fLab[1]);
  grRejCut        -> GetYaxis() -> CenterTitle(fCnt);
  grRejSig        -> SetMarkerColor(fColTrue);
  grRejSig        -> SetMarkerStyle(fMarTrue);
  grRejSig        -> SetFillColor(fColTrue);
  grRejSig        -> SetFillStyle(fFil);
  grRejSig        -> SetLineColor(fColTrue);
  grRejSig        -> SetLineStyle(fLin);
  grRejSig        -> SetLineWidth(fWid);
  grRejSig        -> SetTitle(sTitle.Data());
  grRejSig        -> GetXaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
  grRejSig        -> GetXaxis() -> SetTitle(sSigmaCutAxis.Data());
  grRejSig        -> GetXaxis() -> SetTitleFont(fTxt);
  grRejSig        -> GetXaxis() -> SetTitleSize(fTit[1]);
  grRejSig        -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  grRejSig        -> GetXaxis() -> SetLabelFont(fTxt);
  grRejSig        -> GetXaxis() -> SetLabelSize(fLab[1]);
  grRejSig        -> GetXaxis() -> CenterTitle(fCnt);
  grRejSig        -> GetYaxis() -> SetTitle(sRejectAxis.Data());
  grRejSig        -> GetYaxis() -> SetTitleFont(fTxt);
  grRejSig        -> GetYaxis() -> SetTitleSize(fTit[1]);
  grRejSig        -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  grRejSig        -> GetYaxis() -> SetLabelFont(fTxt);
  grRejSig        -> GetYaxis() -> SetLabelSize(fLab[1]);
  grRejSig        -> GetYaxis() -> CenterTitle(fCnt);
  hEff            -> SetMarkerColor(fColTrk);
  hEff            -> SetMarkerStyle(fMarTrk);
  hEff            -> SetFillColor(fColTrk);
  hEff            -> SetFillStyle(fFil);
  hEff            -> SetLineColor(fColTrk);
  hEff            -> SetLineStyle(fLin);
  hEff            -> SetLineWidth(fWid);
  hEff            -> SetTitle(sTitle.Data());
  hEff            -> SetTitleFont(fTxt);
  hEff            -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  hEff            -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
  hEff            -> GetXaxis() -> SetTitleFont(fTxt);
  hEff            -> GetXaxis() -> SetTitleSize(fTit[0]);
  hEff            -> GetXaxis() -> SetTitleOffset(fOffX[0]);
  hEff            -> GetXaxis() -> SetLabelFont(fTxt);
  hEff            -> GetXaxis() -> SetLabelSize(fLab[0]);
  hEff            -> GetXaxis() -> CenterTitle(fCnt);
  hEff            -> GetYaxis() -> SetTitle(sEffAxis.Data());
  hEff            -> GetYaxis() -> SetTitleFont(fTxt);
  hEff            -> GetYaxis() -> SetTitleSize(fTit[0]);
  hEff            -> GetYaxis() -> SetTitleOffset(fOffY[0]);
  hEff            -> GetYaxis() -> SetLabelFont(fTxt);
  hEff            -> GetYaxis() -> SetLabelSize(fLab[0]);
  hEff            -> GetYaxis() -> CenterTitle(fCnt);
  hPtTruth        -> SetMarkerColor(fColTrue);
  hPtTruth        -> SetMarkerStyle(fMarTrue);
  hPtTruth        -> SetFillColor(fColTrue);
  hPtTruth        -> SetFillStyle(fFil);
  hPtTruth        -> SetLineColor(fColTrue);
  hPtTruth        -> SetLineStyle(fLin);
  hPtTruth        -> SetLineWidth(fWid);
  hPtTruth        -> SetTitle(sTitle.Data());
  hPtTruth        -> SetTitleFont(fTxt);
  hPtTruth        -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  hPtTruth        -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
  hPtTruth        -> GetXaxis() -> SetTitleFont(fTxt);
  hPtTruth        -> GetXaxis() -> SetTitleSize(fTit[1]);
  hPtTruth        -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hPtTruth        -> GetXaxis() -> SetLabelFont(fTxt);
  hPtTruth        -> GetXaxis() -> SetLabelSize(fLab[1]);
  hPtTruth        -> GetXaxis() -> CenterTitle(fCnt);
  hPtTruth        -> GetYaxis() -> SetTitle(sCounts.Data());
  hPtTruth        -> GetYaxis() -> SetTitleFont(fTxt);
  hPtTruth        -> GetYaxis() -> SetTitleSize(fTit[1]);
  hPtTruth        -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hPtTruth        -> GetYaxis() -> SetLabelFont(fTxt);
  hPtTruth        -> GetYaxis() -> SetLabelSize(fLab[1]);
  hPtTruth        -> GetYaxis() -> CenterTitle(fCnt);
  hPtDelta        -> SetMarkerColor(fColTrue);
  hPtDelta        -> SetMarkerStyle(fMarTrue);
  hPtDelta        -> SetFillColor(fColTrue);
  hPtDelta        -> SetFillStyle(fFil);
  hPtDelta        -> SetLineColor(fColTrue);
  hPtDelta        -> SetLineStyle(fLin);
  hPtDelta        -> SetLineWidth(fWid);
  hPtDelta        -> SetTitle(sTitle.Data());
  hPtDelta        -> SetTitleFont(fTxt);
  hPtDelta        -> GetXaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
  hPtDelta        -> GetXaxis() -> SetTitle(sPtDeltaAxis.Data());
  hPtDelta        -> GetXaxis() -> SetTitleFont(fTxt);
  hPtDelta        -> GetXaxis() -> SetTitleSize(fTit[1]);
  hPtDelta        -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hPtDelta        -> GetXaxis() -> SetLabelFont(fTxt);
  hPtDelta        -> GetXaxis() -> SetLabelSize(fLab[1]);
  hPtDelta        -> GetXaxis() -> CenterTitle(fCnt);
  hPtDelta        -> GetYaxis() -> SetTitle(sCounts.Data());
  hPtDelta        -> GetYaxis() -> SetTitleFont(fTxt);
  hPtDelta        -> GetYaxis() -> SetTitleSize(fTit[1]);
  hPtDelta        -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hPtDelta        -> GetYaxis() -> SetLabelFont(fTxt);
  hPtDelta        -> GetYaxis() -> SetLabelSize(fLab[1]);
  hPtDelta        -> GetYaxis() -> CenterTitle(fCnt);
  hPtTrack        -> SetMarkerColor(fColTrue);
  hPtTrack        -> SetMarkerStyle(fMarTrue);
  hPtTrack        -> SetFillColor(fColTrue);
  hPtTrack        -> SetFillStyle(fFil);
  hPtTrack        -> SetLineColor(fColTrue);
  hPtTrack        -> SetLineStyle(fLin);
  hPtTrack        -> SetLineWidth(fWid);
  hPtTrack        -> SetTitle(sTitle.Data());
  hPtTrack        -> SetTitleFont(fTxt);
  hPtTrack        -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  hPtTrack        -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
  hPtTrack        -> GetXaxis() -> SetTitleFont(fTxt);
  hPtTrack        -> GetXaxis() -> SetTitleSize(fTit[1]);
  hPtTrack        -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hPtTrack        -> GetXaxis() -> SetLabelFont(fTxt);
  hPtTrack        -> GetXaxis() -> SetLabelSize(fLab[1]);
  hPtTrack        -> GetXaxis() -> CenterTitle(fCnt);
  hPtTrack        -> GetYaxis() -> SetTitle(sCounts.Data());
  hPtTrack        -> GetYaxis() -> SetTitleFont(fTxt);
  hPtTrack        -> GetYaxis() -> SetTitleSize(fTit[1]);
  hPtTrack        -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hPtTrack        -> GetYaxis() -> SetLabelFont(fTxt);
  hPtTrack        -> GetYaxis() -> SetLabelSize(fLab[1]);
  hPtTrack        -> GetYaxis() -> CenterTitle(fCnt);
  hPtFrac         -> SetMarkerColor(fColTrue);
  hPtFrac         -> SetMarkerStyle(fMarTrue);
  hPtFrac         -> SetFillColor(fColTrue);
  hPtFrac         -> SetFillStyle(fFil);
  hPtFrac         -> SetLineColor(fColTrue);
  hPtFrac         -> SetLineStyle(fLin);
  hPtFrac         -> SetLineWidth(fWid);
  hPtFrac         -> SetTitle(sTitle.Data());
  hPtFrac         -> SetTitleFont(fTxt);
  hPtFrac         -> GetXaxis() -> SetRangeUser(rFracRange[0], rFracRange[1]);
  hPtFrac         -> GetXaxis() -> SetTitle(sPtFracAxis.Data());
  hPtFrac         -> GetXaxis() -> SetTitleFont(fTxt);
  hPtFrac         -> GetXaxis() -> SetTitleSize(fTit[1]);
  hPtFrac         -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hPtFrac         -> GetXaxis() -> SetLabelFont(fTxt);
  hPtFrac         -> GetXaxis() -> SetLabelSize(fLab[1]);
  hPtFrac         -> GetXaxis() -> CenterTitle(fCnt);
  hPtFrac         -> GetYaxis() -> SetTitle(sCounts.Data());
  hPtFrac         -> GetYaxis() -> SetTitleFont(fTxt);
  hPtFrac         -> GetYaxis() -> SetTitleSize(fTit[1]);
  hPtFrac         -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hPtFrac         -> GetYaxis() -> SetLabelFont(fTxt);
  hPtFrac         -> GetYaxis() -> SetLabelSize(fLab[1]);
  hPtFrac         -> GetYaxis() -> CenterTitle(fCnt);
  hPtTrkTru       -> SetMarkerColor(fColTrk);
  hPtTrkTru       -> SetMarkerStyle(fMarTrk);
  hPtTrkTru       -> SetFillColor(fColTrk);
  hPtTrkTru       -> SetFillStyle(fFil);
  hPtTrkTru       -> SetLineColor(fColTrk);
  hPtTrkTru       -> SetLineStyle(fLin);
  hPtTrkTru       -> SetLineWidth(fWid);
  hPtTrkTru       -> SetTitle(sTitle.Data());
  hPtTrkTru       -> SetTitleFont(fTxt);
  hPtTrkTru       -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  hPtTrkTru       -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
  hPtTrkTru       -> GetXaxis() -> SetTitleFont(fTxt);
  hPtTrkTru       -> GetXaxis() -> SetTitleSize(fTit[1]);
  hPtTrkTru       -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hPtTrkTru       -> GetXaxis() -> SetLabelFont(fTxt);
  hPtTrkTru       -> GetXaxis() -> SetLabelSize(fLab[1]);
  hPtTrkTru       -> GetXaxis() -> CenterTitle(fCnt);
  hPtTrkTru       -> GetYaxis() -> SetTitle(sCounts.Data());
  hPtTrkTru       -> GetYaxis() -> SetTitleFont(fTxt);
  hPtTrkTru       -> GetYaxis() -> SetTitleSize(fTit[1]);
  hPtTrkTru       -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hPtTrkTru       -> GetYaxis() -> SetLabelFont(fTxt);
  hPtTrkTru       -> GetYaxis() -> SetLabelSize(fLab[1]);
  hPtTrkTru       -> GetYaxis() -> CenterTitle(fCnt);
  hPtDeltaVsFrac  -> SetMarkerColor(fColTrk);
  hPtDeltaVsFrac  -> SetMarkerStyle(fMarTrk);
  hPtDeltaVsFrac  -> SetFillColor(fColTrk);
  hPtDeltaVsFrac  -> SetFillStyle(fFil);
  hPtDeltaVsFrac  -> SetLineColor(fColTrk);
  hPtDeltaVsFrac  -> SetLineStyle(fLin);
  hPtDeltaVsFrac  -> SetLineWidth(fWid);
  hPtDeltaVsFrac  -> SetTitle(sTitle.Data());
  hPtDeltaVsFrac  -> SetTitleFont(fTxt);
  hPtDeltaVsFrac  -> GetXaxis() -> SetRangeUser(rFracRange[0], rFracRange[1]);
  hPtDeltaVsFrac  -> GetXaxis() -> SetTitle(sPtFracAxis.Data());
  hPtDeltaVsFrac  -> GetXaxis() -> SetTitleFont(fTxt);
  hPtDeltaVsFrac  -> GetXaxis() -> SetTitleSize(fTit[1]);
  hPtDeltaVsFrac  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hPtDeltaVsFrac  -> GetXaxis() -> SetLabelFont(fTxt);
  hPtDeltaVsFrac  -> GetXaxis() -> SetLabelSize(fLab[1]);
  hPtDeltaVsFrac  -> GetXaxis() -> CenterTitle(fCnt);
  hPtDeltaVsFrac  -> GetYaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
  hPtDeltaVsFrac  -> GetYaxis() -> SetTitle(sPtDeltaAxis.Data());
  hPtDeltaVsFrac  -> GetYaxis() -> SetTitleFont(fTxt);
  hPtDeltaVsFrac  -> GetYaxis() -> SetTitleSize(fTit[1]);
  hPtDeltaVsFrac  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hPtDeltaVsFrac  -> GetYaxis() -> SetLabelFont(fTxt);
  hPtDeltaVsFrac  -> GetYaxis() -> SetLabelSize(fLab[1]);
  hPtDeltaVsFrac  -> GetYaxis() -> CenterTitle(fCnt);
  hPtDeltaVsFrac  -> GetZaxis() -> SetTitle(sCounts.Data());
  hPtDeltaVsFrac  -> GetZaxis() -> SetTitleFont(fTxt);
  hPtDeltaVsFrac  -> GetZaxis() -> SetTitleSize(fTit[1]);
  hPtDeltaVsFrac  -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
  hPtDeltaVsFrac  -> GetZaxis() -> SetLabelFont(fTxt);
  hPtDeltaVsFrac  -> GetZaxis() -> SetLabelSize(fLab[1]);
  hPtDeltaVsFrac  -> GetZaxis() -> CenterTitle(fCnt);
  hPtDeltaVsTrue  -> SetMarkerColor(fColTrk);
  hPtDeltaVsTrue  -> SetMarkerStyle(fMarTrk);
  hPtDeltaVsTrue  -> SetFillColor(fColTrk);
  hPtDeltaVsTrue  -> SetFillStyle(fFil);
  hPtDeltaVsTrue  -> SetLineColor(fColTrk);
  hPtDeltaVsTrue  -> SetLineStyle(fLin);
  hPtDeltaVsTrue  -> SetLineWidth(fWid);
  hPtDeltaVsTrue  -> SetTitle(sTitle.Data());
  hPtDeltaVsTrue  -> SetTitleFont(fTxt);
  hPtDeltaVsTrue  -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  hPtDeltaVsTrue  -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
  hPtDeltaVsTrue  -> GetXaxis() -> SetTitleFont(fTxt);
  hPtDeltaVsTrue  -> GetXaxis() -> SetTitleSize(fTit[1]);
  hPtDeltaVsTrue  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hPtDeltaVsTrue  -> GetXaxis() -> SetLabelFont(fTxt);
  hPtDeltaVsTrue  -> GetXaxis() -> SetLabelSize(fLab[1]);
  hPtDeltaVsTrue  -> GetXaxis() -> CenterTitle(fCnt);
  hPtDeltaVsTrue  -> GetYaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
  hPtDeltaVsTrue  -> GetYaxis() -> SetTitle(sPtDeltaAxis.Data());
  hPtDeltaVsTrue  -> GetYaxis() -> SetTitleFont(fTxt);
  hPtDeltaVsTrue  -> GetYaxis() -> SetTitleSize(fTit[1]);
  hPtDeltaVsTrue  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hPtDeltaVsTrue  -> GetYaxis() -> SetLabelFont(fTxt);
  hPtDeltaVsTrue  -> GetYaxis() -> SetLabelSize(fLab[1]);
  hPtDeltaVsTrue  -> GetYaxis() -> CenterTitle(fCnt);
  hPtDeltaVsTrue  -> GetZaxis() -> SetTitle(sCounts.Data());
  hPtDeltaVsTrue  -> GetZaxis() -> SetTitleFont(fTxt);
  hPtDeltaVsTrue  -> GetZaxis() -> SetTitleSize(fTit[1]);
  hPtDeltaVsTrue  -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
  hPtDeltaVsTrue  -> GetZaxis() -> SetLabelFont(fTxt);
  hPtDeltaVsTrue  -> GetZaxis() -> SetLabelSize(fLab[1]);
  hPtDeltaVsTrue  -> GetZaxis() -> CenterTitle(fCnt);
  hPtDeltaVsTrack -> SetMarkerColor(fColTrk);
  hPtDeltaVsTrack -> SetMarkerStyle(fMarTrk);
  hPtDeltaVsTrack -> SetFillColor(fColTrk);
  hPtDeltaVsTrack -> SetFillStyle(fFil);
  hPtDeltaVsTrack -> SetLineColor(fColTrk);
  hPtDeltaVsTrack -> SetLineStyle(fLin);
  hPtDeltaVsTrack -> SetLineWidth(fWid);
  hPtDeltaVsTrack -> SetTitle(sTitle.Data());
  hPtDeltaVsTrack -> SetTitleFont(fTxt);
  hPtDeltaVsTrack -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  hPtDeltaVsTrack -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
  hPtDeltaVsTrack -> GetXaxis() -> SetTitleFont(fTxt);
  hPtDeltaVsTrack -> GetXaxis() -> SetTitleSize(fTit[1]);
  hPtDeltaVsTrack -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hPtDeltaVsTrack -> GetXaxis() -> SetLabelFont(fTxt);
  hPtDeltaVsTrack -> GetXaxis() -> SetLabelSize(fLab[1]);
  hPtDeltaVsTrack -> GetXaxis() -> CenterTitle(fCnt);
  hPtDeltaVsTrack -> GetYaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
  hPtDeltaVsTrack -> GetYaxis() -> SetTitle(sPtDeltaAxis.Data());
  hPtDeltaVsTrack -> GetYaxis() -> SetTitleFont(fTxt);
  hPtDeltaVsTrack -> GetYaxis() -> SetTitleSize(fTit[1]);
  hPtDeltaVsTrack -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hPtDeltaVsTrack -> GetYaxis() -> SetLabelFont(fTxt);
  hPtDeltaVsTrack -> GetYaxis() -> SetLabelSize(fLab[1]);
  hPtDeltaVsTrack -> GetYaxis() -> CenterTitle(fCnt);
  hPtDeltaVsTrack -> GetZaxis() -> SetTitle(sCounts.Data());
  hPtDeltaVsTrack -> GetZaxis() -> SetTitleFont(fTxt);
  hPtDeltaVsTrack -> GetZaxis() -> SetTitleSize(fTit[1]);
  hPtDeltaVsTrack -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
  hPtDeltaVsTrack -> GetZaxis() -> SetLabelFont(fTxt);
  hPtDeltaVsTrack -> GetZaxis() -> SetLabelSize(fLab[1]);
  hPtDeltaVsTrack -> GetZaxis() -> CenterTitle(fCnt);
  hPtTrueVsTrack  -> SetMarkerColor(fColTrk);
  hPtTrueVsTrack  -> SetMarkerStyle(fMarTrk);
  hPtTrueVsTrack  -> SetFillColor(fColTrk);
  hPtTrueVsTrack  -> SetFillStyle(fFil);
  hPtTrueVsTrack  -> SetLineColor(fColTrk);
  hPtTrueVsTrack  -> SetLineStyle(fLin);
  hPtTrueVsTrack  -> SetLineWidth(fWid);
  hPtTrueVsTrack  -> SetTitle(sTitle.Data());
  hPtTrueVsTrack  -> SetTitleFont(fTxt);
  hPtTrueVsTrack  -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  hPtTrueVsTrack  -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
  hPtTrueVsTrack  -> GetXaxis() -> SetTitleFont(fTxt);
  hPtTrueVsTrack  -> GetXaxis() -> SetTitleSize(fTit[1]);
  hPtTrueVsTrack  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  hPtTrueVsTrack  -> GetXaxis() -> SetLabelFont(fTxt);
  hPtTrueVsTrack  -> GetXaxis() -> SetLabelSize(fLab[1]);
  hPtTrueVsTrack  -> GetXaxis() -> CenterTitle(fCnt);
  hPtTrueVsTrack  -> GetYaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  hPtTrueVsTrack  -> GetYaxis() -> SetTitle(sPtTrueAxis.Data());
  hPtTrueVsTrack  -> GetYaxis() -> SetTitleFont(fTxt);
  hPtTrueVsTrack  -> GetYaxis() -> SetTitleSize(fTit[1]);
  hPtTrueVsTrack  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  hPtTrueVsTrack  -> GetYaxis() -> SetLabelFont(fTxt);
  hPtTrueVsTrack  -> GetYaxis() -> SetLabelSize(fLab[1]);
  hPtTrueVsTrack  -> GetYaxis() -> CenterTitle(fCnt);
  hPtTrueVsTrack  -> GetZaxis() -> SetTitle(sCounts.Data());
  hPtTrueVsTrack  -> GetZaxis() -> SetTitleFont(fTxt);
  hPtTrueVsTrack  -> GetZaxis() -> SetTitleSize(fTit[1]);
  hPtTrueVsTrack  -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
  hPtTrueVsTrack  -> GetZaxis() -> SetLabelFont(fTxt);
  hPtTrueVsTrack  -> GetZaxis() -> SetLabelSize(fLab[1]);
  hPtTrueVsTrack  -> GetZaxis() -> CenterTitle(fCnt);

  // set styles of delta-pt projection histograms
  grMuProj  -> SetMarkerColor(fColTrue);
  grMuProj  -> SetMarkerStyle(fMarTrue);
  grMuProj  -> SetFillColor(fColTrue);
  grMuProj  -> SetFillStyle(fFil);
  grMuProj  -> SetLineColor(fColTrue);
  grMuProj  -> SetLineStyle(fLin);
  grMuProj  -> SetLineWidth(fWid);
  grMuProj  -> SetTitle(sTitle.Data());
  grMuProj  -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  grMuProj  -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
  grMuProj  -> GetXaxis() -> SetTitleFont(fTxt);
  grMuProj  -> GetXaxis() -> SetTitleSize(fTit[1]);
  grMuProj  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  grMuProj  -> GetXaxis() -> SetLabelFont(fTxt);
  grMuProj  -> GetXaxis() -> SetLabelSize(fLab[1]);
  grMuProj  -> GetXaxis() -> CenterTitle(fCnt);
  grMuProj  -> GetYaxis() -> SetTitle(sMuProjAxis.Data());
  grMuProj  -> GetYaxis() -> SetTitleFont(fTxt);
  grMuProj  -> GetYaxis() -> SetTitleSize(fTit[1]);
  grMuProj  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  grMuProj  -> GetYaxis() -> SetLabelFont(fTxt);
  grMuProj  -> GetYaxis() -> SetLabelSize(fLab[1]);
  grMuProj  -> GetYaxis() -> CenterTitle(fCnt);
  grSigProj -> SetMarkerColor(fColTrue);
  grSigProj -> SetMarkerStyle(fMarTrue);
  grSigProj -> SetFillColor(fColTrue);
  grSigProj -> SetFillStyle(fFil);
  grSigProj -> SetLineColor(fColTrue);
  grSigProj -> SetLineStyle(fLin);
  grSigProj -> SetLineWidth(fWid);
  grSigProj -> SetTitle(sTitle.Data());
  grSigProj -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
  grSigProj -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
  grSigProj -> GetXaxis() -> SetTitleFont(fTxt);
  grSigProj -> GetXaxis() -> SetTitleSize(fTit[1]);
  grSigProj -> GetXaxis() -> SetTitleOffset(fOffX[1]);
  grSigProj -> GetXaxis() -> SetLabelFont(fTxt);
  grSigProj -> GetXaxis() -> SetLabelSize(fLab[1]);
  grSigProj -> GetXaxis() -> CenterTitle(fCnt);
  grSigProj -> GetYaxis() -> SetTitle(sSigProjAxis.Data());
  grSigProj -> GetYaxis() -> SetTitleFont(fTxt);
  grSigProj -> GetYaxis() -> SetTitleSize(fTit[1]);
  grSigProj -> GetYaxis() -> SetTitleOffset(fOffY[1]);
  grSigProj -> GetYaxis() -> SetLabelFont(fTxt);
  grSigProj -> GetYaxis() -> SetLabelSize(fLab[1]);
  grSigProj -> GetYaxis() -> CenterTitle(fCnt);
  for (Ssiz_t iProj = 0; iProj < NProj; iProj++) {
    hPtDeltaProj[iProj] -> SetMarkerColor(fColProj[iProj]);
    hPtDeltaProj[iProj] -> SetMarkerStyle(fMarProj[iProj]);
    hPtDeltaProj[iProj] -> SetFillColor(fColProj[iProj]);
    hPtDeltaProj[iProj] -> SetFillStyle(fFil);
    hPtDeltaProj[iProj] -> SetLineColor(fColProj[iProj]);
    hPtDeltaProj[iProj] -> SetLineStyle(fLin);
    hPtDeltaProj[iProj] -> SetLineWidth(fWid);
    hPtDeltaProj[iProj] -> SetTitle(sTitle.Data());
    hPtDeltaProj[iProj] -> SetTitleFont(fTxt);
    hPtDeltaProj[iProj] -> GetXaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
    hPtDeltaProj[iProj] -> GetXaxis() -> SetTitle(sPtDeltaAxis.Data());
    hPtDeltaProj[iProj] -> GetXaxis() -> SetTitleFont(fTxt);
    hPtDeltaProj[iProj] -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaProj[iProj] -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtDeltaProj[iProj] -> GetXaxis() -> SetLabelFont(fTxt);
    hPtDeltaProj[iProj] -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaProj[iProj] -> GetXaxis() -> CenterTitle(fCnt);
    hPtDeltaProj[iProj] -> GetYaxis() -> SetTitle(sCounts.Data());
    hPtDeltaProj[iProj] -> GetYaxis() -> SetTitleFont(fTxt);
    hPtDeltaProj[iProj] -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaProj[iProj] -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtDeltaProj[iProj] -> GetYaxis() -> SetLabelFont(fTxt);
    hPtDeltaProj[iProj] -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaProj[iProj] -> GetYaxis() -> CenterTitle(fCnt);
  }
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    grMuHiProj[iSig]  -> SetMarkerColor(fColSig[iSig]);
    grMuHiProj[iSig]  -> SetMarkerStyle(fMarSig[iSig]);
    grMuHiProj[iSig]  -> SetFillColor(fColSig[iSig]);
    grMuHiProj[iSig]  -> SetFillStyle(fFil);
    grMuHiProj[iSig]  -> SetLineColor(fColSig[iSig]);
    grMuHiProj[iSig]  -> SetLineStyle(fLin);
    grMuHiProj[iSig]  -> SetLineWidth(fWid);
    grMuHiProj[iSig]  -> SetTitle(sTitle.Data());
    grMuHiProj[iSig]  -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    grMuHiProj[iSig]  -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
    grMuHiProj[iSig]  -> GetXaxis() -> SetTitleFont(fTxt);
    grMuHiProj[iSig]  -> GetXaxis() -> SetTitleSize(fTit[1]);
    grMuHiProj[iSig]  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    grMuHiProj[iSig]  -> GetXaxis() -> SetLabelFont(fTxt);
    grMuHiProj[iSig]  -> GetXaxis() -> SetLabelSize(fLab[1]);
    grMuHiProj[iSig]  -> GetXaxis() -> CenterTitle(fCnt);
    grMuHiProj[iSig]  -> GetYaxis() -> SetTitle(sMuProjAxis.Data());
    grMuHiProj[iSig]  -> GetYaxis() -> SetTitleFont(fTxt);
    grMuHiProj[iSig]  -> GetYaxis() -> SetTitleSize(fTit[1]);
    grMuHiProj[iSig]  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    grMuHiProj[iSig]  -> GetYaxis() -> SetLabelFont(fTxt);
    grMuHiProj[iSig]  -> GetYaxis() -> SetLabelSize(fLab[1]);
    grMuHiProj[iSig]  -> GetYaxis() -> CenterTitle(fCnt);
    grMuLoProj[iSig]  -> SetMarkerColor(fColSig[iSig]);
    grMuLoProj[iSig]  -> SetMarkerStyle(fMarSig[iSig]);
    grMuLoProj[iSig]  -> SetFillColor(fColSig[iSig]);
    grMuLoProj[iSig]  -> SetFillStyle(fFil);
    grMuLoProj[iSig]  -> SetLineColor(fColSig[iSig]);
    grMuLoProj[iSig]  -> SetLineStyle(fLin);
    grMuLoProj[iSig]  -> SetLineWidth(fWid);
    grMuLoProj[iSig]  -> SetTitle(sTitle.Data());
    grMuLoProj[iSig]  -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    grMuLoProj[iSig]  -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
    grMuLoProj[iSig]  -> GetXaxis() -> SetTitleFont(fTxt);
    grMuLoProj[iSig]  -> GetXaxis() -> SetTitleSize(fTit[1]);
    grMuLoProj[iSig]  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    grMuLoProj[iSig]  -> GetXaxis() -> SetLabelFont(fTxt);
    grMuLoProj[iSig]  -> GetXaxis() -> SetLabelSize(fLab[1]);
    grMuLoProj[iSig]  -> GetXaxis() -> CenterTitle(fCnt);
    grMuLoProj[iSig]  -> GetYaxis() -> SetTitle(sMuProjAxis.Data());
    grMuLoProj[iSig]  -> GetYaxis() -> SetTitleFont(fTxt);
    grMuLoProj[iSig]  -> GetYaxis() -> SetTitleSize(fTit[1]);
    grMuLoProj[iSig]  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    grMuLoProj[iSig]  -> GetYaxis() -> SetLabelFont(fTxt);
    grMuLoProj[iSig]  -> GetYaxis() -> SetLabelSize(fLab[1]);
    grMuLoProj[iSig]  -> GetYaxis() -> CenterTitle(fCnt);
  }

  // set styles of flat delta-pt cut histograms
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    hEffCut[iCut]            -> SetMarkerColor(fColCut[iCut]);
    hEffCut[iCut]            -> SetMarkerStyle(fMarCut[iCut]);
    hEffCut[iCut]            -> SetFillColor(fColCut[iCut]);
    hEffCut[iCut]            -> SetFillStyle(fFil);
    hEffCut[iCut]            -> SetLineColor(fColCut[iCut]);
    hEffCut[iCut]            -> SetLineStyle(fLin);
    hEffCut[iCut]            -> SetLineWidth(fWid);
    hEffCut[iCut]            -> SetTitle(sTitle.Data());
    hEffCut[iCut]            -> SetTitleFont(fTxt);
    hEffCut[iCut]            -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hEffCut[iCut]            -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
    hEffCut[iCut]            -> GetXaxis() -> SetTitleFont(fTxt);
    hEffCut[iCut]            -> GetXaxis() -> SetTitleSize(fTit[0]);
    hEffCut[iCut]            -> GetXaxis() -> SetTitleOffset(fOffX[0]);
    hEffCut[iCut]            -> GetXaxis() -> SetLabelFont(fTxt);
    hEffCut[iCut]            -> GetXaxis() -> SetLabelSize(fLab[0]);
    hEffCut[iCut]            -> GetXaxis() -> CenterTitle(fCnt);
    hEffCut[iCut]            -> GetYaxis() -> SetTitle(sEffAxis.Data());
    hEffCut[iCut]            -> GetYaxis() -> SetTitleFont(fTxt);
    hEffCut[iCut]            -> GetYaxis() -> SetTitleSize(fTit[0]);
    hEffCut[iCut]            -> GetYaxis() -> SetTitleOffset(fOffY[0]);
    hEffCut[iCut]            -> GetYaxis() -> SetLabelFont(fTxt);
    hEffCut[iCut]            -> GetYaxis() -> SetLabelSize(fLab[0]);
    hEffCut[iCut]            -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaCut[iCut]        -> SetMarkerColor(fColCut[iCut]);
    hPtDeltaCut[iCut]        -> SetMarkerStyle(fMarCut[iCut]);
    hPtDeltaCut[iCut]        -> SetFillColor(fColCut[iCut]);
    hPtDeltaCut[iCut]        -> SetFillStyle(fFil);
    hPtDeltaCut[iCut]        -> SetLineColor(fColCut[iCut]);
    hPtDeltaCut[iCut]        -> SetLineStyle(fLin);
    hPtDeltaCut[iCut]        -> SetLineWidth(fWid);
    hPtDeltaCut[iCut]        -> SetTitle(sTitle.Data());
    hPtDeltaCut[iCut]        -> SetTitleFont(fTxt);
    hPtDeltaCut[iCut]        -> GetXaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
    hPtDeltaCut[iCut]        -> GetXaxis() -> SetTitle(sPtDeltaAxis.Data());
    hPtDeltaCut[iCut]        -> GetXaxis() -> SetTitleFont(fTxt);
    hPtDeltaCut[iCut]        -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaCut[iCut]        -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtDeltaCut[iCut]        -> GetXaxis() -> SetLabelFont(fTxt);
    hPtDeltaCut[iCut]        -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaCut[iCut]        -> GetXaxis() -> CenterTitle(fCnt);
    hPtDeltaCut[iCut]        -> GetYaxis() -> SetTitle(sCounts.Data());
    hPtDeltaCut[iCut]        -> GetYaxis() -> SetTitleFont(fTxt);
    hPtDeltaCut[iCut]        -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaCut[iCut]        -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtDeltaCut[iCut]        -> GetYaxis() -> SetLabelFont(fTxt);
    hPtDeltaCut[iCut]        -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaCut[iCut]        -> GetYaxis() -> CenterTitle(fCnt);
    hPtTrackCut[iCut]        -> SetMarkerColor(fColCut[iCut]);
    hPtTrackCut[iCut]        -> SetMarkerStyle(fMarCut[iCut]);
    hPtTrackCut[iCut]        -> SetFillColor(fColCut[iCut]);
    hPtTrackCut[iCut]        -> SetFillStyle(fFil);
    hPtTrackCut[iCut]        -> SetLineColor(fColCut[iCut]);
    hPtTrackCut[iCut]        -> SetLineStyle(fLin);
    hPtTrackCut[iCut]        -> SetLineWidth(fWid);
    hPtTrackCut[iCut]        -> SetTitle(sTitle.Data());
    hPtTrackCut[iCut]        -> SetTitleFont(fTxt);
    hPtTrackCut[iCut]        -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtTrackCut[iCut]        -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
    hPtTrackCut[iCut]        -> GetXaxis() -> SetTitleFont(fTxt);
    hPtTrackCut[iCut]        -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtTrackCut[iCut]        -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtTrackCut[iCut]        -> GetXaxis() -> SetLabelFont(fTxt);
    hPtTrackCut[iCut]        -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtTrackCut[iCut]        -> GetXaxis() -> CenterTitle(fCnt);
    hPtTrackCut[iCut]        -> GetYaxis() -> SetTitle(sCounts.Data());
    hPtTrackCut[iCut]        -> GetYaxis() -> SetTitleFont(fTxt);
    hPtTrackCut[iCut]        -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtTrackCut[iCut]        -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtTrackCut[iCut]        -> GetYaxis() -> SetLabelFont(fTxt);
    hPtTrackCut[iCut]        -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtTrackCut[iCut]        -> GetYaxis() -> CenterTitle(fCnt);
    hPtFracCut[iCut]         -> SetMarkerColor(fColCut[iCut]);
    hPtFracCut[iCut]         -> SetMarkerStyle(fMarCut[iCut]);
    hPtFracCut[iCut]         -> SetFillColor(fColCut[iCut]);
    hPtFracCut[iCut]         -> SetFillStyle(fFil);
    hPtFracCut[iCut]         -> SetLineColor(fColCut[iCut]);
    hPtFracCut[iCut]         -> SetLineStyle(fLin);
    hPtFracCut[iCut]         -> SetLineWidth(fWid);
    hPtFracCut[iCut]         -> SetTitle(sTitle.Data());
    hPtFracCut[iCut]         -> SetTitleFont(fTxt);
    hPtFracCut[iCut]         -> GetXaxis() -> SetRangeUser(rFracRange[0], rFracRange[1]);
    hPtFracCut[iCut]         -> GetXaxis() -> SetTitle(sPtFracAxis.Data());
    hPtFracCut[iCut]         -> GetXaxis() -> SetTitleFont(fTxt);
    hPtFracCut[iCut]         -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtFracCut[iCut]         -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtFracCut[iCut]         -> GetXaxis() -> SetLabelFont(fTxt);
    hPtFracCut[iCut]         -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtFracCut[iCut]         -> GetXaxis() -> CenterTitle(fCnt);
    hPtFracCut[iCut]         -> GetYaxis() -> SetTitle(sCounts.Data());
    hPtFracCut[iCut]         -> GetYaxis() -> SetTitleFont(fTxt);
    hPtFracCut[iCut]         -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtFracCut[iCut]         -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtFracCut[iCut]         -> GetYaxis() -> SetLabelFont(fTxt);
    hPtFracCut[iCut]         -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtFracCut[iCut]         -> GetYaxis() -> CenterTitle(fCnt);
    hPtTrkTruCut[iCut]       -> SetMarkerColor(fColCut[iCut]);
    hPtTrkTruCut[iCut]       -> SetMarkerStyle(fMarCut[iCut]);
    hPtTrkTruCut[iCut]       -> SetFillColor(fColCut[iCut]);
    hPtTrkTruCut[iCut]       -> SetFillStyle(fFil);
    hPtTrkTruCut[iCut]       -> SetLineColor(fColCut[iCut]);
    hPtTrkTruCut[iCut]       -> SetLineStyle(fLin);
    hPtTrkTruCut[iCut]       -> SetLineWidth(fWid);
    hPtTrkTruCut[iCut]       -> SetTitle(sTitle.Data());
    hPtTrkTruCut[iCut]       -> SetTitleFont(fTxt);
    hPtTrkTruCut[iCut]       -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtTrkTruCut[iCut]       -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
    hPtTrkTruCut[iCut]       -> GetXaxis() -> SetTitleFont(fTxt);
    hPtTrkTruCut[iCut]       -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtTrkTruCut[iCut]       -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtTrkTruCut[iCut]       -> GetXaxis() -> SetLabelFont(fTxt);
    hPtTrkTruCut[iCut]       -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtTrkTruCut[iCut]       -> GetXaxis() -> CenterTitle(fCnt);
    hPtTrkTruCut[iCut]       -> GetYaxis() -> SetTitle(sCounts.Data());
    hPtTrkTruCut[iCut]       -> GetYaxis() -> SetTitleFont(fTxt);
    hPtTrkTruCut[iCut]       -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtTrkTruCut[iCut]       -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtTrkTruCut[iCut]       -> GetYaxis() -> SetLabelFont(fTxt);
    hPtTrkTruCut[iCut]       -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtTrkTruCut[iCut]       -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaVsFracCut[iCut]  -> SetMarkerColor(fColCut[iCut]);
    hPtDeltaVsFracCut[iCut]  -> SetMarkerStyle(fMarCut[iCut]);
    hPtDeltaVsFracCut[iCut]  -> SetFillColor(fColCut[iCut]);
    hPtDeltaVsFracCut[iCut]  -> SetFillStyle(fFil);
    hPtDeltaVsFracCut[iCut]  -> SetLineColor(fColCut[iCut]);
    hPtDeltaVsFracCut[iCut]  -> SetLineStyle(fLin);
    hPtDeltaVsFracCut[iCut]  -> SetLineWidth(fWid);
    hPtDeltaVsFracCut[iCut]  -> SetTitle(sTitle.Data());
    hPtDeltaVsFracCut[iCut]  -> SetTitleFont(fTxt);
    hPtDeltaVsFracCut[iCut]  -> GetXaxis() -> SetRangeUser(rFracRange[0], rFracRange[1]);
    hPtDeltaVsFracCut[iCut]  -> GetXaxis() -> SetTitle(sPtFracAxis.Data());
    hPtDeltaVsFracCut[iCut]  -> GetXaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsFracCut[iCut]  -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsFracCut[iCut]  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtDeltaVsFracCut[iCut]  -> GetXaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsFracCut[iCut]  -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsFracCut[iCut]  -> GetXaxis() -> CenterTitle(fCnt);
    hPtDeltaVsFracCut[iCut]  -> GetYaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
    hPtDeltaVsFracCut[iCut]  -> GetYaxis() -> SetTitle(sPtDeltaAxis.Data());
    hPtDeltaVsFracCut[iCut]  -> GetYaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsFracCut[iCut]  -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsFracCut[iCut]  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtDeltaVsFracCut[iCut]  -> GetYaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsFracCut[iCut]  -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsFracCut[iCut]  -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaVsFracCut[iCut]  -> GetZaxis() -> SetTitle(sCounts.Data());
    hPtDeltaVsFracCut[iCut]  -> GetZaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsFracCut[iCut]  -> GetZaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsFracCut[iCut]  -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
    hPtDeltaVsFracCut[iCut]  -> GetZaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsFracCut[iCut]  -> GetZaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsFracCut[iCut]  -> GetZaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrueCut[iCut]  -> SetMarkerColor(fColTrk);
    hPtDeltaVsTrueCut[iCut]  -> SetMarkerStyle(fMarTrk);
    hPtDeltaVsTrueCut[iCut]  -> SetFillColor(fColTrk);
    hPtDeltaVsTrueCut[iCut]  -> SetFillStyle(fFil);
    hPtDeltaVsTrueCut[iCut]  -> SetLineColor(fColTrk);
    hPtDeltaVsTrueCut[iCut]  -> SetLineStyle(fLin);
    hPtDeltaVsTrueCut[iCut]  -> SetLineWidth(fWid);
    hPtDeltaVsTrueCut[iCut]  -> SetTitle(sTitle.Data());
    hPtDeltaVsTrueCut[iCut]  -> SetTitleFont(fTxt);
    hPtDeltaVsTrueCut[iCut]  -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
    hPtDeltaVsTrueCut[iCut]  -> GetXaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrueCut[iCut]  -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetXaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrueCut[iCut]  -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetXaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrueCut[iCut]  -> GetYaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetYaxis() -> SetTitle(sPtDeltaAxis.Data());
    hPtDeltaVsTrueCut[iCut]  -> GetYaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrueCut[iCut]  -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetYaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrueCut[iCut]  -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrueCut[iCut]  -> GetZaxis() -> SetTitle(sCounts.Data());
    hPtDeltaVsTrueCut[iCut]  -> GetZaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrueCut[iCut]  -> GetZaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetZaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrueCut[iCut]  -> GetZaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrueCut[iCut]  -> GetZaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrackCut[iCut] -> SetMarkerColor(fColTrk);
    hPtDeltaVsTrackCut[iCut] -> SetMarkerStyle(fMarTrk);
    hPtDeltaVsTrackCut[iCut] -> SetFillColor(fColTrk);
    hPtDeltaVsTrackCut[iCut] -> SetFillStyle(fFil);
    hPtDeltaVsTrackCut[iCut] -> SetLineColor(fColTrk);
    hPtDeltaVsTrackCut[iCut] -> SetLineStyle(fLin);
    hPtDeltaVsTrackCut[iCut] -> SetLineWidth(fWid);
    hPtDeltaVsTrackCut[iCut] -> SetTitle(sTitle.Data());
    hPtDeltaVsTrackCut[iCut] -> SetTitleFont(fTxt);
    hPtDeltaVsTrackCut[iCut] -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtDeltaVsTrackCut[iCut] -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
    hPtDeltaVsTrackCut[iCut] -> GetXaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrackCut[iCut] -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrackCut[iCut] -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtDeltaVsTrackCut[iCut] -> GetXaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrackCut[iCut] -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrackCut[iCut] -> GetXaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrackCut[iCut] -> GetYaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
    hPtDeltaVsTrackCut[iCut] -> GetYaxis() -> SetTitle(sPtDeltaAxis.Data());
    hPtDeltaVsTrackCut[iCut] -> GetYaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrackCut[iCut] -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrackCut[iCut] -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtDeltaVsTrackCut[iCut] -> GetYaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrackCut[iCut] -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrackCut[iCut] -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrackCut[iCut] -> GetZaxis() -> SetTitle(sCounts.Data());
    hPtDeltaVsTrackCut[iCut] -> GetZaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrackCut[iCut] -> GetZaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrackCut[iCut] -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
    hPtDeltaVsTrackCut[iCut] -> GetZaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrackCut[iCut] -> GetZaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrackCut[iCut] -> GetZaxis() -> CenterTitle(fCnt);
    hPtTrueVsTrackCut[iCut]  -> SetMarkerColor(fColCut[iCut]);
    hPtTrueVsTrackCut[iCut]  -> SetMarkerStyle(fMarCut[iCut]);
    hPtTrueVsTrackCut[iCut]  -> SetFillColor(fColCut[iCut]);
    hPtTrueVsTrackCut[iCut]  -> SetFillStyle(fFil);
    hPtTrueVsTrackCut[iCut]  -> SetLineColor(fColCut[iCut]);
    hPtTrueVsTrackCut[iCut]  -> SetLineStyle(fLin);
    hPtTrueVsTrackCut[iCut]  -> SetLineWidth(fWid);
    hPtTrueVsTrackCut[iCut]  -> SetTitle(sTitle.Data());
    hPtTrueVsTrackCut[iCut]  -> SetTitleFont(fTxt);
    hPtTrueVsTrackCut[iCut]  -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtTrueVsTrackCut[iCut]  -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
    hPtTrueVsTrackCut[iCut]  -> GetXaxis() -> SetTitleFont(fTxt);
    hPtTrueVsTrackCut[iCut]  -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtTrueVsTrackCut[iCut]  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtTrueVsTrackCut[iCut]  -> GetXaxis() -> SetLabelFont(fTxt);
    hPtTrueVsTrackCut[iCut]  -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtTrueVsTrackCut[iCut]  -> GetXaxis() -> CenterTitle(fCnt);
    hPtTrueVsTrackCut[iCut]  -> GetYaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtTrueVsTrackCut[iCut]  -> GetYaxis() -> SetTitle(sPtTrueAxis.Data());
    hPtTrueVsTrackCut[iCut]  -> GetYaxis() -> SetTitleFont(fTxt);
    hPtTrueVsTrackCut[iCut]  -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtTrueVsTrackCut[iCut]  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtTrueVsTrackCut[iCut]  -> GetYaxis() -> SetLabelFont(fTxt);
    hPtTrueVsTrackCut[iCut]  -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtTrueVsTrackCut[iCut]  -> GetYaxis() -> CenterTitle(fCnt);
    hPtTrueVsTrackCut[iCut]  -> GetZaxis() -> SetTitle(sCounts.Data());
    hPtTrueVsTrackCut[iCut]  -> GetZaxis() -> SetTitleFont(fTxt);
    hPtTrueVsTrackCut[iCut]  -> GetZaxis() -> SetTitleSize(fTit[1]);
    hPtTrueVsTrackCut[iCut]  -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
    hPtTrueVsTrackCut[iCut]  -> GetZaxis() -> SetLabelFont(fTxt);
    hPtTrueVsTrackCut[iCut]  -> GetZaxis() -> SetLabelSize(fLab[1]);
    hPtTrueVsTrackCut[iCut]  -> GetZaxis() -> CenterTitle(fCnt);
  }

  // set styles of flat delta-pt cut histograms
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    hEffSig[iSig]            -> SetMarkerColor(fColSig[iSig]);
    hEffSig[iSig]            -> SetMarkerStyle(fMarSig[iSig]);
    hEffSig[iSig]            -> SetFillColor(fColSig[iSig]);
    hEffSig[iSig]            -> SetFillStyle(fFil);
    hEffSig[iSig]            -> SetLineColor(fColSig[iSig]);
    hEffSig[iSig]            -> SetLineStyle(fLin);
    hEffSig[iSig]            -> SetLineWidth(fWid);
    hEffSig[iSig]            -> SetTitle(sTitle.Data());
    hEffSig[iSig]            -> SetTitleFont(fTxt);
    hEffSig[iSig]            -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hEffSig[iSig]            -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
    hEffSig[iSig]            -> GetXaxis() -> SetTitleFont(fTxt);
    hEffSig[iSig]            -> GetXaxis() -> SetTitleSize(fTit[0]);
    hEffSig[iSig]            -> GetXaxis() -> SetTitleOffset(fOffX[0]);
    hEffSig[iSig]            -> GetXaxis() -> SetLabelFont(fTxt);
    hEffSig[iSig]            -> GetXaxis() -> SetLabelSize(fLab[0]);
    hEffSig[iSig]            -> GetXaxis() -> CenterTitle(fCnt);
    hEffSig[iSig]            -> GetYaxis() -> SetTitle(sEffAxis.Data());
    hEffSig[iSig]            -> GetYaxis() -> SetTitleFont(fTxt);
    hEffSig[iSig]            -> GetYaxis() -> SetTitleSize(fTit[0]);
    hEffSig[iSig]            -> GetYaxis() -> SetTitleOffset(fOffY[0]);
    hEffSig[iSig]            -> GetYaxis() -> SetLabelFont(fTxt);
    hEffSig[iSig]            -> GetYaxis() -> SetLabelSize(fLab[0]);
    hEffSig[iSig]            -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaSig[iSig]        -> SetMarkerColor(fColSig[iSig]);
    hPtDeltaSig[iSig]        -> SetMarkerStyle(fMarSig[iSig]);
    hPtDeltaSig[iSig]        -> SetFillColor(fColSig[iSig]);
    hPtDeltaSig[iSig]        -> SetFillStyle(fFil);
    hPtDeltaSig[iSig]        -> SetLineColor(fColSig[iSig]);
    hPtDeltaSig[iSig]        -> SetLineStyle(fLin);
    hPtDeltaSig[iSig]        -> SetLineWidth(fWid);
    hPtDeltaSig[iSig]        -> SetTitle(sTitle.Data());
    hPtDeltaSig[iSig]        -> SetTitleFont(fTxt);
    hPtDeltaSig[iSig]        -> GetXaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
    hPtDeltaSig[iSig]        -> GetXaxis() -> SetTitle(sPtDeltaAxis.Data());
    hPtDeltaSig[iSig]        -> GetXaxis() -> SetTitleFont(fTxt);
    hPtDeltaSig[iSig]        -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaSig[iSig]        -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtDeltaSig[iSig]        -> GetXaxis() -> SetLabelFont(fTxt);
    hPtDeltaSig[iSig]        -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaSig[iSig]        -> GetXaxis() -> CenterTitle(fCnt);
    hPtDeltaSig[iSig]        -> GetYaxis() -> SetTitle(sCounts.Data());
    hPtDeltaSig[iSig]        -> GetYaxis() -> SetTitleFont(fTxt);
    hPtDeltaSig[iSig]        -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaSig[iSig]        -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtDeltaSig[iSig]        -> GetYaxis() -> SetLabelFont(fTxt);
    hPtDeltaSig[iSig]        -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaSig[iSig]        -> GetYaxis() -> CenterTitle(fCnt);
    hPtTrackSig[iSig]        -> SetMarkerColor(fColSig[iSig]);
    hPtTrackSig[iSig]        -> SetMarkerStyle(fMarSig[iSig]);
    hPtTrackSig[iSig]        -> SetFillColor(fColSig[iSig]);
    hPtTrackSig[iSig]        -> SetFillStyle(fFil);
    hPtTrackSig[iSig]        -> SetLineColor(fColSig[iSig]);
    hPtTrackSig[iSig]        -> SetLineStyle(fLin);
    hPtTrackSig[iSig]        -> SetLineWidth(fWid);
    hPtTrackSig[iSig]        -> SetTitle(sTitle.Data());
    hPtTrackSig[iSig]        -> SetTitleFont(fTxt);
    hPtTrackSig[iSig]        -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtTrackSig[iSig]        -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
    hPtTrackSig[iSig]        -> GetXaxis() -> SetTitleFont(fTxt);
    hPtTrackSig[iSig]        -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtTrackSig[iSig]        -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtTrackSig[iSig]        -> GetXaxis() -> SetLabelFont(fTxt);
    hPtTrackSig[iSig]        -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtTrackSig[iSig]        -> GetXaxis() -> CenterTitle(fCnt);
    hPtTrackSig[iSig]        -> GetYaxis() -> SetTitle(sCounts.Data());
    hPtTrackSig[iSig]        -> GetYaxis() -> SetTitleFont(fTxt);
    hPtTrackSig[iSig]        -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtTrackSig[iSig]        -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtTrackSig[iSig]        -> GetYaxis() -> SetLabelFont(fTxt);
    hPtTrackSig[iSig]        -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtTrackSig[iSig]        -> GetYaxis() -> CenterTitle(fCnt);
    hPtFracSig[iSig]         -> SetMarkerColor(fColSig[iSig]);
    hPtFracSig[iSig]         -> SetMarkerStyle(fMarSig[iSig]);
    hPtFracSig[iSig]         -> SetFillColor(fColSig[iSig]);
    hPtFracSig[iSig]         -> SetFillStyle(fFil);
    hPtFracSig[iSig]         -> SetLineColor(fColSig[iSig]);
    hPtFracSig[iSig]         -> SetLineStyle(fLin);
    hPtFracSig[iSig]         -> SetLineWidth(fWid);
    hPtFracSig[iSig]         -> SetTitle(sTitle.Data());
    hPtFracSig[iSig]         -> SetTitleFont(fTxt);
    hPtFracSig[iSig]         -> GetXaxis() -> SetRangeUser(rFracRange[0], rFracRange[1]);
    hPtFracSig[iSig]         -> GetXaxis() -> SetTitle(sPtFracAxis.Data());
    hPtFracSig[iSig]         -> GetXaxis() -> SetTitleFont(fTxt);
    hPtFracSig[iSig]         -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtFracSig[iSig]         -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtFracSig[iSig]         -> GetXaxis() -> SetLabelFont(fTxt);
    hPtFracSig[iSig]         -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtFracSig[iSig]         -> GetXaxis() -> CenterTitle(fCnt);
    hPtFracSig[iSig]         -> GetYaxis() -> SetTitle(sCounts.Data());
    hPtFracSig[iSig]         -> GetYaxis() -> SetTitleFont(fTxt);
    hPtFracSig[iSig]         -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtFracSig[iSig]         -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtFracSig[iSig]         -> GetYaxis() -> SetLabelFont(fTxt);
    hPtFracSig[iSig]         -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtFracSig[iSig]         -> GetYaxis() -> CenterTitle(fCnt);
    hPtTrkTruSig[iSig]       -> SetMarkerColor(fColSig[iSig]);
    hPtTrkTruSig[iSig]       -> SetMarkerStyle(fMarSig[iSig]);
    hPtTrkTruSig[iSig]       -> SetFillColor(fColSig[iSig]);
    hPtTrkTruSig[iSig]       -> SetFillStyle(fFil);
    hPtTrkTruSig[iSig]       -> SetLineColor(fColSig[iSig]);
    hPtTrkTruSig[iSig]       -> SetLineStyle(fLin);
    hPtTrkTruSig[iSig]       -> SetLineWidth(fWid);
    hPtTrkTruSig[iSig]       -> SetTitle(sTitle.Data());
    hPtTrkTruSig[iSig]       -> SetTitleFont(fTxt);
    hPtTrkTruSig[iSig]       -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtTrkTruSig[iSig]       -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
    hPtTrkTruSig[iSig]       -> GetXaxis() -> SetTitleFont(fTxt);
    hPtTrkTruSig[iSig]       -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtTrkTruSig[iSig]       -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtTrkTruSig[iSig]       -> GetXaxis() -> SetLabelFont(fTxt);
    hPtTrkTruSig[iSig]       -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtTrkTruSig[iSig]       -> GetXaxis() -> CenterTitle(fCnt);
    hPtTrkTruSig[iSig]       -> GetYaxis() -> SetTitle(sCounts.Data());
    hPtTrkTruSig[iSig]       -> GetYaxis() -> SetTitleFont(fTxt);
    hPtTrkTruSig[iSig]       -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtTrkTruSig[iSig]       -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtTrkTruSig[iSig]       -> GetYaxis() -> SetLabelFont(fTxt);
    hPtTrkTruSig[iSig]       -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtTrkTruSig[iSig]       -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaVsFracSig[iSig]  -> SetMarkerColor(fColSig[iSig]);
    hPtDeltaVsFracSig[iSig]  -> SetMarkerStyle(fMarSig[iSig]);
    hPtDeltaVsFracSig[iSig]  -> SetFillColor(fColSig[iSig]);
    hPtDeltaVsFracSig[iSig]  -> SetFillStyle(fFil);
    hPtDeltaVsFracSig[iSig]  -> SetLineColor(fColSig[iSig]);
    hPtDeltaVsFracSig[iSig]  -> SetLineStyle(fLin);
    hPtDeltaVsFracSig[iSig]  -> SetLineWidth(fWid);
    hPtDeltaVsFracSig[iSig]  -> SetTitle(sTitle.Data());
    hPtDeltaVsFracSig[iSig]  -> SetTitleFont(fTxt);
    hPtDeltaVsFracSig[iSig]  -> GetXaxis() -> SetRangeUser(rFracRange[0], rFracRange[1]);
    hPtDeltaVsFracSig[iSig]  -> GetXaxis() -> SetTitle(sPtFracAxis.Data());
    hPtDeltaVsFracSig[iSig]  -> GetXaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsFracSig[iSig]  -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsFracSig[iSig]  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtDeltaVsFracSig[iSig]  -> GetXaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsFracSig[iSig]  -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsFracSig[iSig]  -> GetXaxis() -> CenterTitle(fCnt);
    hPtDeltaVsFracSig[iSig]  -> GetYaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
    hPtDeltaVsFracSig[iSig]  -> GetYaxis() -> SetTitle(sPtDeltaAxis.Data());
    hPtDeltaVsFracSig[iSig]  -> GetYaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsFracSig[iSig]  -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsFracSig[iSig]  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtDeltaVsFracSig[iSig]  -> GetYaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsFracSig[iSig]  -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsFracSig[iSig]  -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaVsFracSig[iSig]  -> GetZaxis() -> SetTitle(sCounts.Data());
    hPtDeltaVsFracSig[iSig]  -> GetZaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsFracSig[iSig]  -> GetZaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsFracSig[iSig]  -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
    hPtDeltaVsFracSig[iSig]  -> GetZaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsFracSig[iSig]  -> GetZaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsFracSig[iSig]  -> GetZaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrueSig[iSig]  -> SetMarkerColor(fColTrk);
    hPtDeltaVsTrueSig[iSig]  -> SetMarkerStyle(fMarTrk);
    hPtDeltaVsTrueSig[iSig]  -> SetFillColor(fColTrk);
    hPtDeltaVsTrueSig[iSig]  -> SetFillStyle(fFil);
    hPtDeltaVsTrueSig[iSig]  -> SetLineColor(fColTrk);
    hPtDeltaVsTrueSig[iSig]  -> SetLineStyle(fLin);
    hPtDeltaVsTrueSig[iSig]  -> SetLineWidth(fWid);
    hPtDeltaVsTrueSig[iSig]  -> SetTitle(sTitle.Data());
    hPtDeltaVsTrueSig[iSig]  -> SetTitleFont(fTxt);
    hPtDeltaVsTrueSig[iSig]  -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetXaxis() -> SetTitle(sPtTrueAxis.Data());
    hPtDeltaVsTrueSig[iSig]  -> GetXaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrueSig[iSig]  -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetXaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrueSig[iSig]  -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetXaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrueSig[iSig]  -> GetYaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetYaxis() -> SetTitle(sPtDeltaAxis.Data());
    hPtDeltaVsTrueSig[iSig]  -> GetYaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrueSig[iSig]  -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetYaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrueSig[iSig]  -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrueSig[iSig]  -> GetZaxis() -> SetTitle(sCounts.Data());
    hPtDeltaVsTrueSig[iSig]  -> GetZaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrueSig[iSig]  -> GetZaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetZaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrueSig[iSig]  -> GetZaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrueSig[iSig]  -> GetZaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrackSig[iSig] -> SetMarkerColor(fColTrk);
    hPtDeltaVsTrackSig[iSig] -> SetMarkerStyle(fMarTrk);
    hPtDeltaVsTrackSig[iSig] -> SetFillColor(fColTrk);
    hPtDeltaVsTrackSig[iSig] -> SetFillStyle(fFil);
    hPtDeltaVsTrackSig[iSig] -> SetLineColor(fColTrk);
    hPtDeltaVsTrackSig[iSig] -> SetLineStyle(fLin);
    hPtDeltaVsTrackSig[iSig] -> SetLineWidth(fWid);
    hPtDeltaVsTrackSig[iSig] -> SetTitle(sTitle.Data());
    hPtDeltaVsTrackSig[iSig] -> SetTitleFont(fTxt);
    hPtDeltaVsTrackSig[iSig] -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtDeltaVsTrackSig[iSig] -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
    hPtDeltaVsTrackSig[iSig] -> GetXaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrackSig[iSig] -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrackSig[iSig] -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtDeltaVsTrackSig[iSig] -> GetXaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrackSig[iSig] -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrackSig[iSig] -> GetXaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrackSig[iSig] -> GetYaxis() -> SetRangeUser(rDeltaRange[0], rDeltaRange[1]);
    hPtDeltaVsTrackSig[iSig] -> GetYaxis() -> SetTitle(sPtDeltaAxis.Data());
    hPtDeltaVsTrackSig[iSig] -> GetYaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrackSig[iSig] -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrackSig[iSig] -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtDeltaVsTrackSig[iSig] -> GetYaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrackSig[iSig] -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrackSig[iSig] -> GetYaxis() -> CenterTitle(fCnt);
    hPtDeltaVsTrackSig[iSig] -> GetZaxis() -> SetTitle(sCounts.Data());
    hPtDeltaVsTrackSig[iSig] -> GetZaxis() -> SetTitleFont(fTxt);
    hPtDeltaVsTrackSig[iSig] -> GetZaxis() -> SetTitleSize(fTit[1]);
    hPtDeltaVsTrackSig[iSig] -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
    hPtDeltaVsTrackSig[iSig] -> GetZaxis() -> SetLabelFont(fTxt);
    hPtDeltaVsTrackSig[iSig] -> GetZaxis() -> SetLabelSize(fLab[1]);
    hPtDeltaVsTrackSig[iSig] -> GetZaxis() -> CenterTitle(fCnt);
    hPtTrueVsTrackSig[iSig]  -> SetMarkerColor(fColSig[iSig]);
    hPtTrueVsTrackSig[iSig]  -> SetMarkerStyle(fMarSig[iSig]);
    hPtTrueVsTrackSig[iSig]  -> SetFillColor(fColSig[iSig]);
    hPtTrueVsTrackSig[iSig]  -> SetFillStyle(fFil);
    hPtTrueVsTrackSig[iSig]  -> SetLineColor(fColSig[iSig]);
    hPtTrueVsTrackSig[iSig]  -> SetLineStyle(fLin);
    hPtTrueVsTrackSig[iSig]  -> SetLineWidth(fWid);
    hPtTrueVsTrackSig[iSig]  -> SetTitle(sTitle.Data());
    hPtTrueVsTrackSig[iSig]  -> SetTitleFont(fTxt);
    hPtTrueVsTrackSig[iSig]  -> GetXaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtTrueVsTrackSig[iSig]  -> GetXaxis() -> SetTitle(sPtRecoAxis.Data());
    hPtTrueVsTrackSig[iSig]  -> GetXaxis() -> SetTitleFont(fTxt);
    hPtTrueVsTrackSig[iSig]  -> GetXaxis() -> SetTitleSize(fTit[1]);
    hPtTrueVsTrackSig[iSig]  -> GetXaxis() -> SetTitleOffset(fOffX[1]);
    hPtTrueVsTrackSig[iSig]  -> GetXaxis() -> SetLabelFont(fTxt);
    hPtTrueVsTrackSig[iSig]  -> GetXaxis() -> SetLabelSize(fLab[1]);
    hPtTrueVsTrackSig[iSig]  -> GetXaxis() -> CenterTitle(fCnt);
    hPtTrueVsTrackSig[iSig]  -> GetYaxis() -> SetRangeUser(rPtRange[0], rPtRange[1]);
    hPtTrueVsTrackSig[iSig]  -> GetYaxis() -> SetTitle(sPtTrueAxis.Data());
    hPtTrueVsTrackSig[iSig]  -> GetYaxis() -> SetTitleFont(fTxt);
    hPtTrueVsTrackSig[iSig]  -> GetYaxis() -> SetTitleSize(fTit[1]);
    hPtTrueVsTrackSig[iSig]  -> GetYaxis() -> SetTitleOffset(fOffY[1]);
    hPtTrueVsTrackSig[iSig]  -> GetYaxis() -> SetLabelFont(fTxt);
    hPtTrueVsTrackSig[iSig]  -> GetYaxis() -> SetLabelSize(fLab[1]);
    hPtTrueVsTrackSig[iSig]  -> GetYaxis() -> CenterTitle(fCnt);
    hPtTrueVsTrackSig[iSig]  -> GetZaxis() -> SetTitle(sCounts.Data());
    hPtTrueVsTrackSig[iSig]  -> GetZaxis() -> SetTitleFont(fTxt);
    hPtTrueVsTrackSig[iSig]  -> GetZaxis() -> SetTitleSize(fTit[1]);
    hPtTrueVsTrackSig[iSig]  -> GetZaxis() -> SetTitleOffset(fOffZ[1]);
    hPtTrueVsTrackSig[iSig]  -> GetZaxis() -> SetLabelFont(fTxt);
    hPtTrueVsTrackSig[iSig]  -> GetZaxis() -> SetLabelSize(fLab[1]);
    hPtTrueVsTrackSig[iSig]  -> GetZaxis() -> CenterTitle(fCnt);
  }
  cout << "    Set styles." << endl;

  // make legends
  const UInt_t  fColLe          = 0;
  const UInt_t  fFilLe          = 0;
  const UInt_t  fLinLe          = 0;
  const Float_t yObjLe          = 0.1 + ((NDPtCuts + 2) * 0.05);
  const Float_t yObjMu          = 0.1 + ((NSigCuts + 1) * 0.05);
  const Float_t yObjDel         = 0.1 + (NSigCuts * 0.05);
  const Float_t yObjPro         = 0.1 + (NProj * 0.05);
  const Float_t yObjSig         = 0.1 + ((NSigCuts + 2) * 0.05);
  const Float_t fLegXY[NVtx]    = {0.1, 0.1, 0.3, yObjLe};
  const Float_t fLegMuXY[NVtx]  = {0.1, 0.1, 0.3, yObjMu};
  const Float_t fLegDelXY[NVtx] = {0.1, 0.1, 0.3, yObjDel};
  const Float_t fLegProXY[NVtx] = {0.1, 0.1, 0.3, yObjPro};
  const Float_t fLegSigXY[NVtx] = {0.1, 0.1, 0.3, yObjSig};

  TLegend *leg = new TLegend(fLegXY[0], fLegXY[1], fLegXY[2], fLegXY[3]);
  leg -> SetFillColor(fColLe);
  leg -> SetFillStyle(fFilLe);
  leg -> SetLineColor(fColLe);
  leg -> SetLineStyle(fLinLe);
  leg -> SetTextFont(fTxt);
  leg -> SetTextAlign(fAln);
  leg -> AddEntry(hPtTruth,  sLegTrue.Data(),  "pf");
  leg -> AddEntry(hPtTrkTru, sLegTrack.Data(), "pf");
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    leg -> AddEntry(hPtTrkTruCut[iCut], sLegCut[iCut].Data(), "pf");
  }

  TLegend *legMu = new TLegend(fLegMuXY[0], fLegMuXY[1], fLegMuXY[2], fLegMuXY[3]);
  legMu -> SetFillColor(fColLe);
  legMu -> SetFillStyle(fFilLe);
  legMu -> SetLineColor(fColLe);
  legMu -> SetLineStyle(fLinLe);
  legMu -> SetTextFont(fTxt);
  legMu -> SetTextAlign(fAln);
  legMu -> AddEntry(grMuProj, sLegMu.Data(), "p");
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    legMu -> AddEntry(grMuHiProj[iSig], sLegProjSig[iSig].Data(), "p");
  }

  TLegend *legDel = new TLegend(fLegDelXY[0], fLegDelXY[1], fLegDelXY[2], fLegDelXY[3]);
  legDel -> SetFillColor(fColLe);
  legDel -> SetFillStyle(fFilLe);
  legDel -> SetLineColor(fColLe);
  legDel -> SetLineStyle(fLinLe);
  legDel -> SetTextFont(fTxt);
  legDel -> SetTextAlign(fAln);
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    legDel -> AddEntry(fMuHiProj[iSig], sLegDelta[iSig].Data(), "l");
  }

  TLegend *legPro = new TLegend(fLegProXY[0], fLegProXY[1], fLegProXY[2], fLegProXY[3]);
  legPro -> SetFillColor(fColLe);
  legPro -> SetFillStyle(fFilLe);
  legPro -> SetLineColor(fColLe);
  legPro -> SetLineStyle(fLinLe);
  legPro -> SetTextFont(fTxt);
  legPro -> SetTextAlign(fAln);
  for (Ssiz_t iProj = 0; iProj < NProj; iProj++) {
    legPro -> AddEntry(hPtDeltaProj[iProj], sLegProj[iProj].Data(), "pf");
  }

  TLegend *legSig = new TLegend(fLegSigXY[0], fLegSigXY[1], fLegSigXY[2], fLegSigXY[3]);
  legSig -> SetFillColor(fColLe);
  legSig -> SetFillStyle(fFilLe);
  legSig -> SetLineColor(fColLe);
  legSig -> SetLineStyle(fLinLe);
  legSig -> SetTextFont(fTxt);
  legSig -> SetTextAlign(fAln);
  legSig -> AddEntry(hPtTruth,  sLegTrue.Data(),  "pf");
  legSig -> AddEntry(hPtTrkTru, sLegTrack.Data(), "pf");
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    legSig -> AddEntry(hPtTrkTruSig[iSig], sLegSig[iSig].Data(), "pf");
  }
  cout << "    Made legends." << endl;

  // make text boxes
  const UInt_t  fColInf       = 0;
  const UInt_t  fFilInf       = 0;
  const UInt_t  fLinInf       = 0;
  const Float_t yObjInf       = 0.1 + (NTxt * 0.05);
  const Float_t yObjCut       = 0.1 + (NTrkCuts * 0.05);
  const Float_t fInfXY[NVtx] = {0.3, 0.1, 0.5, yObjInf};
  const Float_t fCutXY[NVtx] = {0.5, 0.1, 0.7, yObjCut};

  TPaveText *info = new TPaveText(fInfXY[0], fInfXY[1], fInfXY[2], fInfXY[3], "NDC NB");
  info -> SetFillColor(fColInf);
  info -> SetFillStyle(fFilInf);
  info -> SetLineColor(fColInf);
  info -> SetLineStyle(fLinInf);
  info -> SetTextFont(fTxt);
  info -> SetTextAlign(fAln);
  for (Ssiz_t iTxt = 0; iTxt < NTxt; iTxt++) {
    info -> AddText(sInfo[iTxt].Data());
  }

  TPaveText *cuts = new TPaveText(fCutXY[0], fCutXY[1], fCutXY[2], fCutXY[3], "NDC NB");
  cuts -> SetFillColor(fColInf);
  cuts -> SetFillStyle(fFilInf);
  cuts -> SetLineColor(fColInf);
  cuts -> SetLineStyle(fLinInf);
  cuts -> SetTextFont(fTxt);
  cuts -> SetTextAlign(fAln);
  for (Ssiz_t iTrkCut = 0; iTrkCut < NTrkCuts; iTrkCut++) {
    cuts -> AddText(sTrkCuts[iTrkCut].Data());
  }
  cout << "    Made text." << endl;

  // make line
  const UInt_t  fColLi       = 1;
  const UInt_t  fLinLi       = 9;
  const UInt_t  fWidLi       = 1;
  const Float_t fLinXY[NVtx] = {rPtRange[0], 1., rPtRange[1], 1.};

  TLine *line = new TLine(fLinXY[0], fLinXY[1], fLinXY[2], fLinXY[3]);
  line -> SetLineColor(fColLi);
  line -> SetLineStyle(fLinLi);
  line -> SetLineWidth(fWidLi);
  cout << "    Made line." << endl;

  // make plots
  const UInt_t  width(750);
  const UInt_t  width2D(1500);
  const UInt_t  height(950);
  const UInt_t  heightNR(750);
  const UInt_t  fMode(0);
  const UInt_t  fBord(2);
  const UInt_t  fGrid(0);
  const UInt_t  fTick(1);
  const UInt_t  fLogX(0);
  const UInt_t  fLogY1(1);
  const UInt_t  fLogY2(1);
  const UInt_t  fLogYNR(0);
  const UInt_t  fFrame(0);
  const Float_t fMarginL(0.15);
  const Float_t fMarginR(0.02);
  const Float_t fMarginR2D(0.15);
  const Float_t fMarginT1(0.005);
  const Float_t fMarginT2(0.02);
  const Float_t fMarginTNR(0.02);
  const Float_t fMarginB1(0.25);
  const Float_t fMarginB2(0.005);
  const Float_t fMarginBNR(0.15);
  const Float_t fEffXY[NVtx]       = {0.,  0.,   1.,  0.35};
  const Float_t fTrksXY[NVtx]      = {0.,  0.35, 1.,  1.};
  const Float_t fTwoDimXY[NVtx]    = {0.,  0.,   0.5, 1.};
  const Float_t fProjectXY[NVtx]   = {0.5, 0.,   1.,  1.};
  const Float_t fBeforeDPtXY[NVtx] = {0.,  0.,   0.5, 1.};
  const Float_t fAfterDPtXY[NVtx]  = {0.5, 0.,   1.,  1.};

  TCanvas *cEffCut = new TCanvas("cEfficiency_FlatCut", "", width, height);
  TPad    *pEffCut = new TPad("pEffCut", "", fEffXY[0],  fEffXY[1],  fEffXY[2],  fEffXY[3]);
  TPad    *pTrkCut = new TPad("pTrkCut", "", fTrksXY[0], fTrksXY[1], fTrksXY[2], fTrksXY[3]);
  cEffCut -> SetGrid(fGrid, fGrid);
  cEffCut -> SetTicks(fTick, fTick);
  cEffCut -> SetBorderMode(fMode);
  cEffCut -> SetBorderSize(fBord);
  pEffCut -> SetGrid(fGrid, fGrid);
  pEffCut -> SetTicks(fTick, fTick);
  pEffCut -> SetLogx(fLogX);
  pEffCut -> SetLogy(fLogY1);
  pEffCut -> SetBorderMode(fMode);
  pEffCut -> SetBorderSize(fBord);
  pEffCut -> SetFrameBorderMode(fFrame);
  pEffCut -> SetLeftMargin(fMarginL);
  pEffCut -> SetRightMargin(fMarginR);
  pEffCut -> SetTopMargin(fMarginT1);
  pEffCut -> SetBottomMargin(fMarginB1);
  pTrkCut -> SetGrid(fGrid, fGrid);
  pTrkCut -> SetTicks(fTick, fTick);
  pTrkCut -> SetLogx(fLogX);
  pTrkCut -> SetLogy(fLogY2);
  pTrkCut -> SetBorderMode(fMode);
  pTrkCut -> SetBorderSize(fBord);
  pTrkCut -> SetFrameBorderMode(fFrame);
  pTrkCut -> SetLeftMargin(fMarginL);
  pTrkCut -> SetRightMargin(fMarginR);
  pTrkCut -> SetTopMargin(fMarginT2);
  pTrkCut -> SetBottomMargin(fMarginB2);
  cEffCut -> cd();
  pEffCut -> Draw();
  pTrkCut -> Draw();
  pEffCut -> cd();
  hEff    -> Draw();
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    hEffCut[iCut] -> Draw("SAME");
  }
  line      -> Draw();
  pTrkCut   -> cd();
  hPtTruth  -> Draw();
  hPtTrkTru -> Draw("SAME");
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    hPtTrkTruCut[iCut] -> Draw("SAME");
  }
  leg     -> Draw();
  info    -> Draw();
  cuts    -> Draw();
  fOutput -> cd();
  cEffCut -> Write();
  cEffCut -> Close();

  TCanvas *cEffSig = new TCanvas("cEfficiency_SigmaCut", "", width, height);
  TPad    *pEffSig = new TPad("pEffSig", "", fEffXY[0],  fEffXY[1],  fEffXY[2],  fEffXY[3]);
  TPad    *pTrkSig = new TPad("pTrkSig", "", fTrksXY[0], fTrksXY[1], fTrksXY[2], fTrksXY[3]);
  cEffSig -> SetGrid(fGrid, fGrid);
  cEffSig -> SetTicks(fTick, fTick);
  cEffSig -> SetBorderMode(fMode);
  cEffSig -> SetBorderSize(fBord);
  pEffSig -> SetGrid(fGrid, fGrid);
  pEffSig -> SetTicks(fTick, fTick);
  pEffSig -> SetLogx(fLogX);
  pEffSig -> SetLogy(fLogY1);
  pEffSig -> SetBorderMode(fMode);
  pEffSig -> SetBorderSize(fBord);
  pEffSig -> SetFrameBorderMode(fFrame);
  pEffSig -> SetLeftMargin(fMarginL);
  pEffSig -> SetRightMargin(fMarginR);
  pEffSig -> SetTopMargin(fMarginT1);
  pEffSig -> SetBottomMargin(fMarginB1);
  pTrkSig -> SetGrid(fGrid, fGrid);
  pTrkSig -> SetTicks(fTick, fTick);
  pTrkSig -> SetLogx(fLogX);
  pTrkSig -> SetLogy(fLogY2);
  pTrkSig -> SetBorderMode(fMode);
  pTrkSig -> SetBorderSize(fBord);
  pTrkSig -> SetFrameBorderMode(fFrame);
  pTrkSig -> SetLeftMargin(fMarginL);
  pTrkSig -> SetRightMargin(fMarginR);
  pTrkSig -> SetTopMargin(fMarginT2);
  pTrkSig -> SetBottomMargin(fMarginB2);
  cEffSig -> cd();
  pEffSig -> Draw();
  pTrkSig -> Draw();
  pEffSig -> cd();
  hEff    -> Draw();
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    hEffSig[iSig] -> Draw("SAME");
  }
  line      -> Draw();
  pTrkSig   -> cd();
  hPtTruth  -> Draw();
  hPtTrkTru -> Draw("SAME");
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    hPtTrkTruSig[iSig] -> Draw("SAME");
  }
  legSig  -> Draw();
  info    -> Draw();
  cuts    -> Draw();
  fOutput -> cd();
  cEffSig -> Write();
  cEffSig -> Close();

  TCanvas *cRejCut = new TCanvas("cReject_FlatCut", "", width, heightNR);
  cRejCut  -> SetGrid(fGrid, fGrid);
  cRejCut  -> SetTicks(fTick, fTick);
  cRejCut  -> SetBorderMode(fMode);
  cRejCut  -> SetBorderSize(fBord);
  cRejCut  -> SetFrameBorderMode(fFrame);
  cRejCut  -> SetLeftMargin(fMarginL);
  cRejCut  -> SetRightMargin(fMarginR);
  cRejCut  -> SetTopMargin(fMarginTNR);
  cRejCut  -> SetBottomMargin(fMarginBNR);
  cRejCut  -> SetLogx(fLogX);
  cRejCut  -> SetLogy(fLogYNR);
  cRejCut  -> cd();
  grRejCut -> Draw("ALP");
  info     -> Draw();
  cuts     -> Draw();
  fOutput  -> cd();
  cRejCut  -> Write();
  cRejCut  -> Close();

  TCanvas *cRejSig = new TCanvas("cReject_SigmaCut", "", width, heightNR);
  cRejSig  -> SetGrid(fGrid, fGrid);
  cRejSig  -> SetTicks(fTick, fTick);
  cRejSig  -> SetBorderMode(fMode);
  cRejSig  -> SetBorderSize(fBord);
  cRejSig  -> SetFrameBorderMode(fFrame);
  cRejSig  -> SetLeftMargin(fMarginL);
  cRejSig  -> SetRightMargin(fMarginR);
  cRejSig  -> SetTopMargin(fMarginTNR);
  cRejSig  -> SetBottomMargin(fMarginBNR);
  cRejSig  -> SetLogx(fLogX);
  cRejSig  -> SetLogy(fLogYNR);
  cRejSig  -> cd();
  grRejSig -> Draw("ALP");
  info     -> Draw();
  cuts     -> Draw();
  fOutput  -> cd();
  cRejSig  -> Write();
  cRejSig  -> Close();

  TCanvas *cPtTruVsTrkCut = new TCanvas("cPtTruthVsReco_FlatCut", "", width2D, heightNR);
  TPad    *pBeforeCut     = new TPad("pBeforeCut", "", fBeforeDPtXY[0], fBeforeDPtXY[1], fBeforeDPtXY[2], fBeforeDPtXY[3]);
  TPad    *pAfterCut      = new TPad("pAfterCut",  "", fAfterDPtXY[0],  fAfterDPtXY[1],  fAfterDPtXY[2],  fAfterDPtXY[3]);
  cPtTruVsTrkCut                -> SetGrid(fGrid, fGrid);
  cPtTruVsTrkCut                -> SetTicks(fTick, fTick);
  cPtTruVsTrkCut                -> SetBorderMode(fMode);
  cPtTruVsTrkCut                -> SetBorderSize(fBord);
  pBeforeCut                    -> SetGrid(fGrid, fGrid);
  pBeforeCut                    -> SetTicks(fTick, fTick);
  pBeforeCut                    -> SetLogx(fLogX);
  pBeforeCut                    -> SetLogy(fLogYNR);
  pBeforeCut                    -> SetBorderMode(fMode);
  pBeforeCut                    -> SetBorderSize(fBord);
  pBeforeCut                    -> SetFrameBorderMode(fFrame);
  pBeforeCut                    -> SetLeftMargin(fMarginL);
  pBeforeCut                    -> SetRightMargin(fMarginR2D);
  pBeforeCut                    -> SetBottomMargin(fMarginBNR);
  pAfterCut                     -> SetGrid(fGrid, fGrid);
  pAfterCut                     -> SetTicks(fTick, fTick);
  pAfterCut                     -> SetLogx(fLogX);
  pAfterCut                     -> SetLogy(fLogYNR);
  pAfterCut                     -> SetBorderMode(fMode);
  pAfterCut                     -> SetBorderSize(fBord);
  pAfterCut                     -> SetFrameBorderMode(fFrame);
  pAfterCut                     -> SetLeftMargin(fMarginL);
  pAfterCut                     -> SetRightMargin(fMarginR2D);
  pAfterCut                     -> SetBottomMargin(fMarginBNR);
  cPtTruVsTrkCut                -> cd();
  pBeforeCut                    -> Draw();
  pAfterCut                     -> Draw();
  pBeforeCut                    -> cd();
  hPtTrueVsTrack                -> SetTitle(sBeforeTitle.Data());
  hPtTrueVsTrack                -> Draw("colz");
  cuts                          -> Draw();
  pAfterCut                     -> cd();
  hPtTrueVsTrackCut[iCutToDraw] -> SetTitle(sAfterCutTitle.Data());
  hPtTrueVsTrackCut[iCutToDraw] -> Draw("colz");
  info                          -> Draw();
  fOutput                       -> cd();
  cPtTruVsTrkCut                -> Write();
  cPtTruVsTrkCut                -> Close();

  TCanvas *cPtTruVsTrkSig = new TCanvas("cPtTruthVsReco_SigmaCut", "", width2D, heightNR);
  TPad    *pBeforeSig     = new TPad("pBeforeSig", "", fBeforeDPtXY[0], fBeforeDPtXY[1], fBeforeDPtXY[2], fBeforeDPtXY[3]);
  TPad    *pAfterSig      = new TPad("pAfterSig",  "", fAfterDPtXY[0],  fAfterDPtXY[1],  fAfterDPtXY[2],  fAfterDPtXY[3]);
  cPtTruVsTrkSig                -> SetGrid(fGrid, fGrid);
  cPtTruVsTrkSig                -> SetTicks(fTick, fTick);
  cPtTruVsTrkSig                -> SetBorderMode(fMode);
  cPtTruVsTrkSig                -> SetBorderSize(fBord);
  pBeforeSig                    -> SetGrid(fGrid, fGrid);
  pBeforeSig                    -> SetTicks(fTick, fTick);
  pBeforeSig                    -> SetLogx(fLogX);
  pBeforeSig                    -> SetLogy(fLogYNR);
  pBeforeSig                    -> SetBorderMode(fMode);
  pBeforeSig                    -> SetBorderSize(fBord);
  pBeforeSig                    -> SetFrameBorderMode(fFrame);
  pBeforeSig                    -> SetLeftMargin(fMarginL);
  pBeforeSig                    -> SetRightMargin(fMarginR2D);
  pBeforeSig                    -> SetBottomMargin(fMarginBNR);
  pAfterSig                     -> SetGrid(fGrid, fGrid);
  pAfterSig                     -> SetTicks(fTick, fTick);
  pAfterSig                     -> SetLogx(fLogX);
  pAfterSig                     -> SetLogy(fLogYNR);
  pAfterSig                     -> SetBorderMode(fMode);
  pAfterSig                     -> SetBorderSize(fBord);
  pAfterSig                     -> SetFrameBorderMode(fFrame);
  pAfterSig                     -> SetLeftMargin(fMarginL);
  pAfterSig                     -> SetRightMargin(fMarginR2D);
  pAfterSig                     -> SetBottomMargin(fMarginBNR);
  cPtTruVsTrkSig                -> cd();
  pBeforeSig                    -> Draw();
  pAfterSig                     -> Draw();
  pBeforeSig                    -> cd();
  hPtTrueVsTrack                -> Draw("colz");
  cuts                          -> Draw();
  pAfterSig                     -> cd();
  hPtTrueVsTrackSig[iSigToDraw] -> SetTitle(sAfterSigTitle.Data());
  hPtTrueVsTrackSig[iSigToDraw] -> Draw("colz");
  info                          -> Draw();
  fOutput                       -> cd();
  cPtTruVsTrkSig                -> Write();
  cPtTruVsTrkSig                -> Close();

  TCanvas *cPtDelVsTrk = new TCanvas("cPtDeltaVsTrack", "", width2D, heightNR);
  TPad    *pTwoDim     = new TPad("pTwoDim",      "", fTwoDimXY[0],  fTwoDimXY[1],  fTwoDimXY[2],  fTwoDimXY[3]);
  TPad    *pProject    = new TPad("pProjections", "", fProjectXY[0], fProjectXY[1], fProjectXY[2], fProjectXY[3]);
  cPtDelVsTrk     -> SetGrid(fGrid, fGrid);
  cPtDelVsTrk     -> SetTicks(fTick, fTick);
  cPtDelVsTrk     -> SetBorderMode(fMode);
  cPtDelVsTrk     -> SetBorderSize(fBord);
  pTwoDim         -> SetGrid(fGrid, fGrid);
  pTwoDim         -> SetTicks(fTick, fTick);
  pTwoDim         -> SetLogx(fLogX);
  pTwoDim         -> SetLogy(fLogYNR);
  pTwoDim         -> SetBorderMode(fMode);
  pTwoDim         -> SetBorderSize(fBord);
  pTwoDim         -> SetFrameBorderMode(fFrame);
  pTwoDim         -> SetLeftMargin(fMarginL);
  pTwoDim         -> SetRightMargin(fMarginR2D);
  pTwoDim         -> SetTopMargin(fMarginTNR);
  pTwoDim         -> SetBottomMargin(fMarginBNR);
  pProject        -> SetGrid(fGrid, fGrid);
  pProject        -> SetTicks(fTick, fTick);
  pProject        -> SetLogx(fLogX);
  pProject        -> SetLogy(fLogYNR);
  pProject        -> SetBorderMode(fMode);
  pProject        -> SetBorderSize(fBord);
  pProject        -> SetFrameBorderMode(fFrame);
  pProject        -> SetLeftMargin(fMarginL);
  pProject        -> SetRightMargin(fMarginR2D);
  pProject        -> SetTopMargin(fMarginTNR);
  pProject        -> SetBottomMargin(fMarginBNR);
  cPtDelVsTrk     -> cd();
  pTwoDim         -> Draw();
  pProject        -> Draw();
  pTwoDim         -> cd();
  hPtDeltaVsTrack -> Draw("colz");
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    fMuHiProj[iSig] -> Draw("same");
    fMuLoProj[iSig] -> Draw("same");
  }
  legDel          -> Draw();
  cuts            -> Draw();
  pProject        -> cd();
  hPtDeltaProj[0] -> Draw();
  fPtDeltaProj[0] -> Draw("same");
  for (Ssiz_t iProj = 1; iProj < NProj; iProj++) {
    hPtDeltaProj[iProj] -> Draw("same");
    fPtDeltaProj[iProj] -> Draw("same");
  }
  legPro      -> Draw();
  info        -> Draw();
  fOutput     -> cd();
  cPtDelVsTrk -> Write();
  cPtDelVsTrk -> Close();

  TCanvas *cDeltaPt = new TCanvas("cDeltaPt", "", width, heightNR);
  cDeltaPt  -> SetGrid(fGrid, fGrid);
  cDeltaPt  -> SetTicks(fTick, fTick);
  cDeltaPt  -> SetBorderMode(fMode);
  cDeltaPt  -> SetBorderSize(fBord);
  cDeltaPt  -> SetFrameBorderMode(fFrame);
  cDeltaPt  -> SetLeftMargin(fMarginL);
  cDeltaPt  -> SetRightMargin(fMarginR);
  cDeltaPt  -> SetTopMargin(fMarginTNR);
  cDeltaPt  -> SetBottomMargin(fMarginBNR);
  cDeltaPt  -> SetLogx(fLogX);
  cDeltaPt  -> SetLogy(fLogYNR);
  cDeltaPt  -> cd();
  hPtDelta  -> Draw();
  info      -> Draw();
  cuts      -> Draw();
  fOutput   -> cd();
  cDeltaPt  -> Write();
  cDeltaPt  -> Close();

  TCanvas *cMuProj = new TCanvas("cMuDeltaPt", "", width, heightNR);
  cMuProj  -> SetGrid(fGrid, fGrid);
  cMuProj  -> SetTicks(fTick, fTick);
  cMuProj  -> SetBorderMode(fMode);
  cMuProj  -> SetBorderSize(fBord);
  cMuProj  -> SetFrameBorderMode(fFrame);
  cMuProj  -> SetLeftMargin(fMarginL);
  cMuProj  -> SetRightMargin(fMarginR);
  cMuProj  -> SetTopMargin(fMarginTNR);
  cMuProj  -> SetBottomMargin(fMarginBNR);
  cMuProj  -> SetLogx(fLogX);
  cMuProj  -> SetLogy(fLogYNR);
  cMuProj  -> cd();
  grMuProj -> Draw("ALP");
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    grMuHiProj[iSig] -> Draw("LP");
    grMuLoProj[iSig] -> Draw("LP");
  }
  legMu   -> Draw();
  info    -> Draw();
  cuts    -> Draw();
  fOutput -> cd();
  cMuProj -> Write();
  cMuProj -> Close();

  TCanvas *cSigProj = new TCanvas("cSigmaDeltaPt", "", width, heightNR);
  cSigProj  -> SetGrid(fGrid, fGrid);
  cSigProj  -> SetTicks(fTick, fTick);
  cSigProj  -> SetBorderMode(fMode);
  cSigProj  -> SetBorderSize(fBord);
  cSigProj  -> SetFrameBorderMode(fFrame);
  cSigProj  -> SetLeftMargin(fMarginL);
  cSigProj  -> SetRightMargin(fMarginR);
  cSigProj  -> SetTopMargin(fMarginTNR);
  cSigProj  -> SetBottomMargin(fMarginBNR);
  cSigProj  -> SetLogx(fLogX);
  cSigProj  -> SetLogy(fLogYNR);
  cSigProj  -> cd();
  grSigProj -> Draw("ALP");
  info      -> Draw();
  cuts      -> Draw();
  fOutput   -> cd();
  cSigProj  -> Write();
  cSigProj  -> Close();
  cout << "    Made plots." << endl;

  // make directories
  TDirectory *dNoCut    = (TDirectory*) fOutput -> mkdir("NoCuts");
  TDirectory *dFlatCut  = (TDirectory*) fOutput -> mkdir("FlatCuts");
  TDirectory *dSigmaCut = (TDirectory*) fOutput -> mkdir("SigmaCuts");
  TDirectory *dProject  = (TDirectory*) fOutput -> mkdir("Projections");

  // save histograms
  fOutput         -> cd();
  dNoCut          -> cd();
  hEff            -> Write();
  hPtTruth        -> Write();
  hPtDelta        -> Write();
  hPtTrack        -> Write();
  hPtFrac         -> Write();
  hPtTrkTru       -> Write();
  hPtDeltaVsFrac  -> Write();
  hPtDeltaVsTrue  -> Write();
  hPtDeltaVsTrack -> Write();
  hPtTrueVsTrack  -> Write();

  // save flat delta-pt cut histograms
  dFlatCut -> cd();
  grRejCut -> Write();
  for (Ssiz_t iCut = 0; iCut < NDPtCuts; iCut++) {
    hEffCut[iCut]            -> Write();
    hPtDeltaCut[iCut]        -> Write();
    hPtTrackCut[iCut]        -> Write();
    hPtFracCut[iCut]         -> Write();
    hPtTrkTruCut[iCut]       -> Write();
    hPtDeltaVsFracCut[iCut]  -> Write();
    hPtDeltaVsTrueCut[iCut]  -> Write();
    hPtDeltaVsTrackCut[iCut] -> Write();
    hPtTrueVsTrackCut[iCut]  -> Write();
  }

  // save pt-dependent delta-pt cut histograms
  dSigmaCut -> cd();
  grRejSig  -> Write();
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    hEffSig[iSig]            -> Write();
    hPtDeltaSig[iSig]        -> Write();
    hPtTrackSig[iSig]        -> Write();
    hPtFracSig[iSig]         -> Write();
    hPtTrkTruSig[iSig]       -> Write();
    hPtDeltaVsFracSig[iSig]  -> Write();
    hPtDeltaVsTrueSig[iSig]  -> Write();
    hPtDeltaVsTrackSig[iSig] -> Write();
    hPtTrueVsTrackSig[iSig]  -> Write();
  }

  // save delta-pt projection histograms
  dProject  -> cd();
  grMuProj  -> Write();
  grSigProj -> Write();
  for (Ssiz_t iProj = 0; iProj < NProj; iProj++) {
    hPtDeltaProj[iProj] -> Write();
    fPtDeltaProj[iProj] -> Write();
  }
  for (Ssiz_t iSig = 0; iSig < NSigCuts; iSig++) {
    fMuHiProj[iSig]  -> Write();
    fMuLoProj[iSig]  -> Write();
    grMuHiProj[iSig] -> Write();
    grMuLoProj[iSig] -> Write();
  }
  cout << "    Saved histograms." << endl;

  // close files
  fOutput -> cd();
  fOutput -> Close();
  fInput  -> cd();
  fInput  -> Close();
  cout << "  Finished delta-pt extractor script!\n" << endl;

}

// end ------------------------------------------------------------------------

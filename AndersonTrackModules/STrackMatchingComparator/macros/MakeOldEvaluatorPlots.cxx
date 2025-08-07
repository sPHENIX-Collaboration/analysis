// ----------------------------------------------------------------------------
// 'MakeNewMatcherPlots.cxx'
// Derek Anderson
// 08.24.2023
//
// Short macro to make plots from the
// output of the SvtxEvaluator module.
// ----------------------------------------------------------------------------

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TError.h"
#include "TChain.h"
#include "TNtuple.h"

using namespace std;

// global constants
static const size_t NSectors = 12;



// main body of macro ---------------------------------------------------------


void MakeOldEvaluatorPlots() {

  // io parameters
  const string         sOutput("oldEvalPlots_oneMatchPerParticle_embedScanOn_forZVtxCutComp_oddFrac05150.pt10n1evt500pim.d1m2y2024.root");
  const string         sTupleTrue("ntp_gtrack");
  const string         sTupleReco("ntp_track");
  const vector<string> vecInTrue = {
    "input/merged/sPhenixG4_oneMatchPerParticle_oldEval_forCrossCheck.pt10num1evt500pim.d25m1y2024.root"
  };
  const vector<string> vecInReco = {
    "input/merged/sPhenixG4_oneMatchPerParticle_oldEval_forCrossCheck.pt10num1evt500pim.d25m1y2024.root"
  };

  // cut options
  const bool useOnlyPrimTrks(true);
  const bool doZVtxCut(true);
  const bool doPhiCut(false);

  // weird track parameters
  const pair<float, float> oddPtFrac = {0.5,  1.5};
  const pair<float, float> zVtxRange = {-1.,  1.};

  // phi cut parameters
  const float sigCutVal(0.75);
  const array<pair<float, float>, NSectors> phiSectors = {
    make_pair(-2.92, 0.12),
    make_pair(-2.38, 0.05),
    make_pair(-1.93, 0.18),
    make_pair(-1.33, 0.07),
    make_pair(-0.90, 0.24),
    make_pair(-0.29, 0.09),
    make_pair(0.23,  0.11),
    make_pair(0.73,  0.10),
    make_pair(1.28,  0.10),
    make_pair(1.81,  0.08),
    make_pair(2.23,  0.18),
    make_pair(2.80,  0.17)
  };

  // lower verbosity
  gErrorIgnoreLevel = kError;
  cout << "\n  Staring old evaluator plot script..." << endl;

  // open files
  TFile* fOutput = new TFile(sOutput.data(), "recreate");
  if (!fOutput) {
     cerr << "PANIC: couldn't open output file!\n"
          << "       fOutput = " << fOutput << "\n"
          << endl;
    return;
  }
  cout << "    Opened files." << endl;

  // grab input trees
  TChain* tInTrue = new TChain(sTupleTrue.data());
  TChain* tInReco = new TChain(sTupleReco.data());
  for (const string sInTrue : vecInTrue) {
    const int added = tInTrue -> Add(sInTrue.data());
    if (added != 1) {
      cerr << "WARNING: was not able to add ntuple from file \'" << sInTrue << "\'!\n"
           << "         return value = " << added
           << endl;
    }
  }
  for (const string sInReco : vecInReco) {
    const int added = tInReco -> Add(sInReco.data());
    if (added != 1) {
      cerr << "WARNING: was not able to add ntuple from file \'" << sInReco << "\'!\n"
           << "         return value = " << added
           << endl;
    }
  }
  cout << "    Grabbed input trees." << endl;

  // declare input leaves
  Float_t tru_event;
  Float_t tru_seed;
  Float_t tru_gntracks;
  Float_t tru_gnchghad;
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
  Float_t tru_siqr;
  Float_t tru_siphi;
  Float_t tru_sithe;
  Float_t tru_six0;
  Float_t tru_siy0;
  Float_t tru_tpqr;
  Float_t tru_tpphi;
  Float_t tru_tpthe;
  Float_t tru_tpx0;
  Float_t tru_tpy0;
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
  Float_t tru_nwrongmaps;
  Float_t tru_ntruintt;
  Float_t tru_nwrongintt;
  Float_t tru_ntrutpc;
  Float_t tru_nwrongtpc;
  Float_t tru_ntrumms;
  Float_t tru_nwrongmms;
  Float_t tru_ntrutpc1;
  Float_t tru_nwrongtpc1;
  Float_t tru_ntrutpc11;
  Float_t tru_nwrongtpc11;
  Float_t tru_ntrutpc2;
  Float_t tru_nwrongtpc2;
  Float_t tru_ntrutpc3;
  Float_t tru_nwrongtpc3;
  Float_t tru_layersfromtruth;
  Float_t tru_npedge;
  Float_t tru_nredge;
  Float_t tru_nbig;
  Float_t tru_novlp;
  Float_t tru_merr;
  Float_t tru_msize;
  Float_t tru_nhittpcall;
  Float_t tru_nhittpcin;
  Float_t tru_nhittpcmid;
  Float_t tru_nhittpcout;
  Float_t tru_nclusall;
  Float_t tru_nclustpc;
  Float_t tru_nclusintt;
  Float_t tru_nclusmaps;
  Float_t tru_nclusmms;

  Float_t rec_event;
  Float_t rec_seed;
  Float_t rec_trackID;
  Float_t rec_crossing;
  Float_t rec_px;
  Float_t rec_py;
  Float_t rec_pz;
  Float_t rec_pt;
  Float_t rec_eta;
  Float_t rec_phi;
  Float_t rec_deltapt;
  Float_t rec_deltaeta;
  Float_t rec_deltaphi;
  Float_t rec_siqr;
  Float_t rec_siphi;
  Float_t rec_sithe;
  Float_t rec_six0;
  Float_t rec_siy0;
  Float_t rec_tpqr;
  Float_t rec_tpphi;
  Float_t rec_tpthe;
  Float_t rec_tpx0;
  Float_t rec_tpy0;
  Float_t rec_charge;
  Float_t rec_quality;
  Float_t rec_chisq;
  Float_t rec_ndf;
  Float_t rec_nhits;
  Float_t rec_nmaps;
  Float_t rec_nintt;
  Float_t rec_ntpc;
  Float_t rec_nmms;
  Float_t rec_ntpc1;
  Float_t rec_ntpc11;
  Float_t rec_ntpc2;
  Float_t rec_ntpc3;
  Float_t rec_nlmaps;
  Float_t rec_nlintt;
  Float_t rec_nltpc;
  Float_t rec_nlmms;
  Float_t rec_layers;
  Float_t rec_vertexID;
  Float_t rec_vx;
  Float_t rec_vy;
  Float_t rec_vz;
  Float_t rec_dca2d;
  Float_t rec_dca2dsigma;
  Float_t rec_dca3dxy;
  Float_t rec_dca3dxysigma;
  Float_t rec_dca3dz;
  Float_t rec_dca3dzsigma;
  Float_t rec_pcax;
  Float_t rec_pcay;
  Float_t rec_pcaz;
  Float_t rec_gtrackID;
  Float_t rec_gflavor;
  Float_t rec_gnhits;
  Float_t rec_gnmaps;
  Float_t rec_gnintt;
  Float_t rec_gntpc;
  Float_t rec_gnmms;
  Float_t rec_gnlmaps;
  Float_t rec_gnlintt;
  Float_t rec_gnltpc;
  Float_t rec_gnlmms;
  Float_t rec_gpx;
  Float_t rec_gpy;
  Float_t rec_gpz;
  Float_t rec_gpt;
  Float_t rec_geta;
  Float_t rec_gphi;
  Float_t rec_gvx;
  Float_t rec_gvy;
  Float_t rec_gvz;
  Float_t rec_gvt;
  Float_t rec_gfpx;
  Float_t rec_gfpy;
  Float_t rec_gfpz;
  Float_t rec_gfx;
  Float_t rec_gfy;
  Float_t rec_gfz;
  Float_t rec_gembed;
  Float_t rec_gprimary;
  Float_t rec_nfromtruth;
  Float_t rec_nwrong;
  Float_t rec_ntrumaps;
  Float_t rec_nwrongmaps;
  Float_t rec_ntruintt;
  Float_t rec_nwrongintt;
  Float_t rec_ntrutpc;
  Float_t rec_nwrongtpc;
  Float_t rec_ntrumms;
  Float_t rec_nwrongmms;
  Float_t rec_ntrutpc1;
  Float_t rec_nwrongtpc1;
  Float_t rec_ntrutpc11;
  Float_t rec_nwrongtpc11;
  Float_t rec_ntrutpc2;
  Float_t rec_nwrongtpc2;
  Float_t rec_ntrutpc3;
  Float_t rec_nwrongtpc3;
  Float_t rec_layersfromtruth;
  Float_t rec_npedge;
  Float_t rec_nredge;
  Float_t rec_nbig;
  Float_t rec_novlp;
  Float_t rec_merr;
  Float_t rec_msize;
  Float_t rec_nhittpcall;
  Float_t rec_nhittpcin;
  Float_t rec_nhittpcmid;
  Float_t rec_nhittpcout;
  Float_t rec_nclusall;
  Float_t rec_nclustpc;
  Float_t rec_nclusintt;
  Float_t rec_nclusmaps;
  Float_t rec_nclusmms;

  // Set branch addresses.
  tInTrue -> SetBranchAddress("event",           &tru_event);
  tInTrue -> SetBranchAddress("seed",            &tru_seed);
  tInTrue -> SetBranchAddress("gntracks",        &tru_gntracks);
  tInTrue -> SetBranchAddress("gnchghad",        &tru_gnchghad);
  tInTrue -> SetBranchAddress("gtrackID",        &tru_gtrackID);
  tInTrue -> SetBranchAddress("gflavor",         &tru_gflavor);
  tInTrue -> SetBranchAddress("gnhits",          &tru_gnhits);
  tInTrue -> SetBranchAddress("gnmaps",          &tru_gnmaps);
  tInTrue -> SetBranchAddress("gnintt",          &tru_gnintt);
  tInTrue -> SetBranchAddress("gnmms",           &tru_gnmms);
  tInTrue -> SetBranchAddress("gnintt1",         &tru_gnintt1);
  tInTrue -> SetBranchAddress("gnintt2",         &tru_gnintt2);
  tInTrue -> SetBranchAddress("gnintt3",         &tru_gnintt3);
  tInTrue -> SetBranchAddress("gnintt4",         &tru_gnintt4);
  tInTrue -> SetBranchAddress("gnintt5",         &tru_gnintt5);
  tInTrue -> SetBranchAddress("gnintt6",         &tru_gnintt6);
  tInTrue -> SetBranchAddress("gnintt7",         &tru_gnintt7);
  tInTrue -> SetBranchAddress("gnintt8",         &tru_gnintt8);
  tInTrue -> SetBranchAddress("gntpc",           &tru_gntpc);
  tInTrue -> SetBranchAddress("gnlmaps",         &tru_gnlmaps);
  tInTrue -> SetBranchAddress("gnlintt",         &tru_gnlintt);
  tInTrue -> SetBranchAddress("gnltpc",          &tru_gnltpc);
  tInTrue -> SetBranchAddress("gnlmms",          &tru_gnlmms);
  tInTrue -> SetBranchAddress("gpx",             &tru_gpx);
  tInTrue -> SetBranchAddress("gpy",             &tru_gpy);
  tInTrue -> SetBranchAddress("gpz",             &tru_gpz);
  tInTrue -> SetBranchAddress("gpt",             &tru_gpt);
  tInTrue -> SetBranchAddress("geta",            &tru_geta);
  tInTrue -> SetBranchAddress("gphi",            &tru_gphi);
  tInTrue -> SetBranchAddress("gvx",             &tru_gvx);
  tInTrue -> SetBranchAddress("gvy",             &tru_gvy);
  tInTrue -> SetBranchAddress("gvz",             &tru_gvz);
  tInTrue -> SetBranchAddress("gvt",             &tru_gvt);
  tInTrue -> SetBranchAddress("gfpx",            &tru_gfpx);
  tInTrue -> SetBranchAddress("gfpy",            &tru_gfpy);
  tInTrue -> SetBranchAddress("gfpz",            &tru_gfpz);
  tInTrue -> SetBranchAddress("gfx",             &tru_gfx);
  tInTrue -> SetBranchAddress("gfy",             &tru_gfy);
  tInTrue -> SetBranchAddress("gfz",             &tru_gfz);
  tInTrue -> SetBranchAddress("gembed",          &tru_gembed);
  tInTrue -> SetBranchAddress("gprimary",        &tru_gprimary);
  tInTrue -> SetBranchAddress("trackID",         &tru_trackID);
  tInTrue -> SetBranchAddress("px",              &tru_px);
  tInTrue -> SetBranchAddress("py",              &tru_py);
  tInTrue -> SetBranchAddress("pz",              &tru_pz);
  tInTrue -> SetBranchAddress("pt",              &tru_pt);
  tInTrue -> SetBranchAddress("eta",             &tru_eta);
  tInTrue -> SetBranchAddress("phi",             &tru_phi);
  tInTrue -> SetBranchAddress("deltapt",         &tru_deltapt);
  tInTrue -> SetBranchAddress("deltaeta",        &tru_deltaeta);
  tInTrue -> SetBranchAddress("deltaphi",        &tru_deltaphi);
  tInTrue -> SetBranchAddress("siqr",            &tru_siqr);
  tInTrue -> SetBranchAddress("siphi",           &tru_siphi);
  tInTrue -> SetBranchAddress("sithe",           &tru_sithe);
  tInTrue -> SetBranchAddress("six0",            &tru_six0);
  tInTrue -> SetBranchAddress("siy0",            &tru_siy0);
  tInTrue -> SetBranchAddress("tpqr",            &tru_tpqr);
  tInTrue -> SetBranchAddress("tpphi",           &tru_tpphi);
  tInTrue -> SetBranchAddress("tpthe",           &tru_tpthe);
  tInTrue -> SetBranchAddress("tpx0",            &tru_tpx0);
  tInTrue -> SetBranchAddress("tpy0",            &tru_tpy0);
  tInTrue -> SetBranchAddress("charge",          &tru_charge);
  tInTrue -> SetBranchAddress("quality",         &tru_quality);
  tInTrue -> SetBranchAddress("chisq",           &tru_chisq);
  tInTrue -> SetBranchAddress("ndf",             &tru_ndf);
  tInTrue -> SetBranchAddress("nhits",           &tru_nhits);
  tInTrue -> SetBranchAddress("layers",          &tru_layers);
  tInTrue -> SetBranchAddress("nmaps",           &tru_nmaps);
  tInTrue -> SetBranchAddress("nintt",           &tru_nintt);
  tInTrue -> SetBranchAddress("ntpc",            &tru_ntpc);
  tInTrue -> SetBranchAddress("nmms",            &tru_nmms);
  tInTrue -> SetBranchAddress("ntpc1",           &tru_ntpc1);
  tInTrue -> SetBranchAddress("ntpc11",          &tru_ntpc11);
  tInTrue -> SetBranchAddress("ntpc2",           &tru_ntpc2);
  tInTrue -> SetBranchAddress("ntpc3",           &tru_ntpc3);
  tInTrue -> SetBranchAddress("nlmaps",          &tru_nlmaps);
  tInTrue -> SetBranchAddress("nlintt",          &tru_nlintt);
  tInTrue -> SetBranchAddress("nltpc",           &tru_nltpc);
  tInTrue -> SetBranchAddress("nlmms",           &tru_nlmms);
  tInTrue -> SetBranchAddress("vertexID",        &tru_vertexID);
  tInTrue -> SetBranchAddress("vx",              &tru_vx);
  tInTrue -> SetBranchAddress("vy",              &tru_vy);
  tInTrue -> SetBranchAddress("vz",              &tru_vz);
  tInTrue -> SetBranchAddress("dca2d",           &tru_dca2d);
  tInTrue -> SetBranchAddress("dca2dsigma",      &tru_dca2dsigma);
  tInTrue -> SetBranchAddress("dca3dxy",         &tru_dca3dxy);
  tInTrue -> SetBranchAddress("dca3dxysigma",    &tru_dca3dxysigma);
  tInTrue -> SetBranchAddress("dca3dz",          &tru_dca3dz);
  tInTrue -> SetBranchAddress("dca3dzsigma",     &tru_dca3dzsigma);
  tInTrue -> SetBranchAddress("pcax",            &tru_pcax);
  tInTrue -> SetBranchAddress("pcay",            &tru_pcay);
  tInTrue -> SetBranchAddress("pcaz",            &tru_pcaz);
  tInTrue -> SetBranchAddress("nfromtruth",      &tru_nfromtruth);
  tInTrue -> SetBranchAddress("nwrong",          &tru_nwrong);
  tInTrue -> SetBranchAddress("ntrumaps",        &tru_ntrumaps);
  tInTrue -> SetBranchAddress("nwrongmaps",      &tru_nwrongmaps);
  tInTrue -> SetBranchAddress("ntruintt",        &tru_ntruintt);
  tInTrue -> SetBranchAddress("nwrongintt",      &tru_nwrongintt);
  tInTrue -> SetBranchAddress("ntrutpc",         &tru_ntrutpc);
  tInTrue -> SetBranchAddress("nwrongtpc",       &tru_nwrongtpc);
  tInTrue -> SetBranchAddress("ntrumms",         &tru_ntrumms);
  tInTrue -> SetBranchAddress("nwrongmms",       &tru_nwrongmms);
  tInTrue -> SetBranchAddress("ntrutpc1",        &tru_ntrutpc1);
  tInTrue -> SetBranchAddress("nwrongtpc1",      &tru_nwrongtpc1);
  tInTrue -> SetBranchAddress("ntrutpc11",       &tru_ntrutpc11);
  tInTrue -> SetBranchAddress("nwrongtpc11",     &tru_nwrongtpc11);
  tInTrue -> SetBranchAddress("ntrutpc2",        &tru_ntrutpc2);
  tInTrue -> SetBranchAddress("nwrongtpc2",      &tru_nwrongtpc2);
  tInTrue -> SetBranchAddress("ntrutpc3",        &tru_ntrutpc3);
  tInTrue -> SetBranchAddress("nwrongtpc3",      &tru_nwrongtpc3);
  tInTrue -> SetBranchAddress("layersfromtruth", &tru_layersfromtruth);
  tInTrue -> SetBranchAddress("npedge",          &tru_npedge);
  tInTrue -> SetBranchAddress("nredge",          &tru_nredge);
  tInTrue -> SetBranchAddress("nbig",            &tru_nbig);
  tInTrue -> SetBranchAddress("novlp",           &tru_novlp);
  tInTrue -> SetBranchAddress("merr",            &tru_merr);
  tInTrue -> SetBranchAddress("msize",           &tru_msize);
  tInTrue -> SetBranchAddress("nhittpcall",      &tru_nhittpcall);
  tInTrue -> SetBranchAddress("nhittpcin",       &tru_nhittpcin);
  tInTrue -> SetBranchAddress("nhittpcmid",      &tru_nhittpcmid);
  tInTrue -> SetBranchAddress("nhittpcout",      &tru_nhittpcout);
  tInTrue -> SetBranchAddress("nclusall",        &tru_nclusall);
  tInTrue -> SetBranchAddress("nclustpc",        &tru_nclustpc);
  tInTrue -> SetBranchAddress("nclusintt",       &tru_nclusintt);
  tInTrue -> SetBranchAddress("nclusmaps",       &tru_nclusmaps);
  tInTrue -> SetBranchAddress("nclusmms",        &tru_nclusmms);

  tInReco -> SetBranchAddress("event",           &rec_event);
  tInReco -> SetBranchAddress("seed",            &rec_seed);
  tInReco -> SetBranchAddress("trackID",         &rec_trackID);
  tInReco -> SetBranchAddress("crossing",        &rec_crossing);
  tInReco -> SetBranchAddress("px",              &rec_px);
  tInReco -> SetBranchAddress("py",              &rec_py);
  tInReco -> SetBranchAddress("pz",              &rec_pz);
  tInReco -> SetBranchAddress("pt",              &rec_pt);
  tInReco -> SetBranchAddress("eta",             &rec_eta);
  tInReco -> SetBranchAddress("phi",             &rec_phi);
  tInReco -> SetBranchAddress("deltapt",         &rec_deltapt);
  tInReco -> SetBranchAddress("deltaeta",        &rec_deltaeta);
  tInReco -> SetBranchAddress("deltaphi",        &rec_deltaphi);
  tInReco -> SetBranchAddress("siqr",            &rec_siqr);
  tInReco -> SetBranchAddress("siphi",           &rec_siphi);
  tInReco -> SetBranchAddress("sithe",           &rec_sithe);
  tInReco -> SetBranchAddress("six0",            &rec_six0);
  tInReco -> SetBranchAddress("siy0",            &rec_siy0);
  tInReco -> SetBranchAddress("tpqr",            &rec_tpqr);
  tInReco -> SetBranchAddress("tpphi",           &rec_tpphi);
  tInReco -> SetBranchAddress("tpthe",           &rec_tpthe);
  tInReco -> SetBranchAddress("tpx0",            &rec_tpx0);
  tInReco -> SetBranchAddress("tpy0",            &rec_tpy0);
  tInReco -> SetBranchAddress("charge",          &rec_charge);
  tInReco -> SetBranchAddress("quality",         &rec_quality);
  tInReco -> SetBranchAddress("chisq",           &rec_chisq);
  tInReco -> SetBranchAddress("ndf",             &rec_ndf);
  tInReco -> SetBranchAddress("nhits",           &rec_nhits);
  tInReco -> SetBranchAddress("nmaps",           &rec_nmaps);
  tInReco -> SetBranchAddress("nintt",           &rec_nintt);
  tInReco -> SetBranchAddress("ntpc",            &rec_ntpc);
  tInReco -> SetBranchAddress("nmms",            &rec_nmms);
  tInReco -> SetBranchAddress("ntpc1",           &rec_ntpc1);
  tInReco -> SetBranchAddress("ntpc11",          &rec_ntpc11);
  tInReco -> SetBranchAddress("ntpc2",           &rec_ntpc2);
  tInReco -> SetBranchAddress("ntpc3",           &rec_ntpc3);
  tInReco -> SetBranchAddress("nlmaps",          &rec_nlmaps);
  tInReco -> SetBranchAddress("nlintt",          &rec_nlintt);
  tInReco -> SetBranchAddress("nltpc",           &rec_nltpc);
  tInReco -> SetBranchAddress("nlmms",           &rec_nlmms);
  tInReco -> SetBranchAddress("layers",          &rec_layers);
  tInReco -> SetBranchAddress("vertexID",        &rec_vertexID);
  tInReco -> SetBranchAddress("vx",              &rec_vx);
  tInReco -> SetBranchAddress("vy",              &rec_vy);
  tInReco -> SetBranchAddress("vz",              &rec_vz);
  tInReco -> SetBranchAddress("dca2d",           &rec_dca2d);
  tInReco -> SetBranchAddress("dca2dsigma",      &rec_dca2dsigma);
  tInReco -> SetBranchAddress("dca3dxy",         &rec_dca3dxy);
  tInReco -> SetBranchAddress("dca3dxysigma",    &rec_dca3dxysigma);
  tInReco -> SetBranchAddress("dca3dz",          &rec_dca3dz);
  tInReco -> SetBranchAddress("dca3dzsigma",     &rec_dca3dzsigma);
  tInReco -> SetBranchAddress("pcax",            &rec_pcax);
  tInReco -> SetBranchAddress("pcay",            &rec_pcay);
  tInReco -> SetBranchAddress("pcaz",            &rec_pcaz);
  tInReco -> SetBranchAddress("gtrackID",        &rec_gtrackID);
  tInReco -> SetBranchAddress("gflavor",         &rec_gflavor);
  tInReco -> SetBranchAddress("gnhits",          &rec_gnhits);
  tInReco -> SetBranchAddress("gnmaps",          &rec_gnmaps);
  tInReco -> SetBranchAddress("gnintt",          &rec_gnintt);
  tInReco -> SetBranchAddress("gntpc",           &rec_gntpc);
  tInReco -> SetBranchAddress("gnmms",           &rec_gnmms);
  tInReco -> SetBranchAddress("gnlmaps",         &rec_gnlmaps);
  tInReco -> SetBranchAddress("gnlintt",         &rec_gnlintt);
  tInReco -> SetBranchAddress("gnltpc",          &rec_gnltpc);
  tInReco -> SetBranchAddress("gnlmms",          &rec_gnlmms);
  tInReco -> SetBranchAddress("gpx",             &rec_gpx);
  tInReco -> SetBranchAddress("gpy",             &rec_gpy);
  tInReco -> SetBranchAddress("gpz",             &rec_gpz);
  tInReco -> SetBranchAddress("gpt",             &rec_gpt);
  tInReco -> SetBranchAddress("geta",            &rec_geta);
  tInReco -> SetBranchAddress("gphi",            &rec_gphi);
  tInReco -> SetBranchAddress("gvx",             &rec_gvx);
  tInReco -> SetBranchAddress("gvy",             &rec_gvy);
  tInReco -> SetBranchAddress("gvz",             &rec_gvz);
  tInReco -> SetBranchAddress("gvt",             &rec_gvt);
  tInReco -> SetBranchAddress("gfpx",            &rec_gfpx);
  tInReco -> SetBranchAddress("gfpy",            &rec_gfpy);
  tInReco -> SetBranchAddress("gfpz",            &rec_gfpz);
  tInReco -> SetBranchAddress("gfx",             &rec_gfx);
  tInReco -> SetBranchAddress("gfy",             &rec_gfy);
  tInReco -> SetBranchAddress("gfz",             &rec_gfz);
  tInReco -> SetBranchAddress("gembed",          &rec_gembed);
  tInReco -> SetBranchAddress("gprimary",        &rec_gprimary);
  tInReco -> SetBranchAddress("nfromtruth",      &rec_nfromtruth);
  tInReco -> SetBranchAddress("nwrong",          &rec_nwrong);
  tInReco -> SetBranchAddress("ntrumaps",        &rec_ntrumaps);
  tInReco -> SetBranchAddress("nwrongmaps",      &rec_nwrongmaps);
  tInReco -> SetBranchAddress("ntruintt",        &rec_ntruintt);
  tInReco -> SetBranchAddress("nwrongintt",      &rec_nwrongintt);
  tInReco -> SetBranchAddress("ntrutpc",         &rec_ntrutpc);
  tInReco -> SetBranchAddress("nwrongtpc",       &rec_nwrongtpc);
  tInReco -> SetBranchAddress("ntrumms",         &rec_ntrumms);
  tInReco -> SetBranchAddress("nwrongmms",       &rec_nwrongmms);
  tInReco -> SetBranchAddress("ntrutpc1",        &rec_ntrutpc1);
  tInReco -> SetBranchAddress("nwrongtpc1",      &rec_nwrongtpc1);
  tInReco -> SetBranchAddress("ntrutpc11",       &rec_ntrutpc11);
  tInReco -> SetBranchAddress("nwrongtpc11",     &rec_nwrongtpc11);
  tInReco -> SetBranchAddress("ntrutpc2",        &rec_ntrutpc2);
  tInReco -> SetBranchAddress("nwrongtpc2",      &rec_nwrongtpc2);
  tInReco -> SetBranchAddress("ntrutpc3",        &rec_ntrutpc3);
  tInReco -> SetBranchAddress("nwrongtpc3",      &rec_nwrongtpc3);
  tInReco -> SetBranchAddress("layersfromtruth", &rec_layersfromtruth);
  tInReco -> SetBranchAddress("npedge",          &rec_npedge);
  tInReco -> SetBranchAddress("nredge",          &rec_nredge);
  tInReco -> SetBranchAddress("nbig",            &rec_nbig);
  tInReco -> SetBranchAddress("novlp",           &rec_novlp);
  tInReco -> SetBranchAddress("merr",            &rec_merr);
  tInReco -> SetBranchAddress("msize",           &rec_msize);
  tInReco -> SetBranchAddress("nhittpcall",      &rec_nhittpcall);
  tInReco -> SetBranchAddress("nhittpcin",       &rec_nhittpcin);
  tInReco -> SetBranchAddress("nhittpcmid",      &rec_nhittpcmid);
  tInReco -> SetBranchAddress("nhittpcout",      &rec_nhittpcout);
  tInReco -> SetBranchAddress("nclusall",        &rec_nclusall);
  tInReco -> SetBranchAddress("nclustpc",        &rec_nclustpc);
  tInReco -> SetBranchAddress("nclusintt",       &rec_nclusintt);
  tInReco -> SetBranchAddress("nclusmaps",       &rec_nclusmaps);
  tInReco -> SetBranchAddress("nclusmms",        &rec_nclusmms);
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
  const uint32_t nPtBins   = 101;
  const uint32_t nFracBins = 110;

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

    // skip nan's
    if (isnan(tru_trackID)) continue;

    // run calculations
    const double tru_gntot = tru_gnintt + tru_gnmaps + tru_gntpc;

    // check if near sector
    bool isNearSector = false;
    if (doPhiCut) {
      for (size_t iSector = 0; iSector < NSectors; iSector++) {
        const float cutVal = sigCutVal * phiSectors[iSector].second;
        const float minPhi = phiSectors[iSector].first - cutVal;
        const float maxPhi = phiSectors[iSector].first + cutVal;
        const bool  isNear = ((tru_gphi >= minPhi) && (tru_gphi <= maxPhi));
        if (isNear) {
          isNearSector = true;
          break;
        }
      }  // end sector loop
    }  // end if (doPhiCut)

    // apply cuts
    const bool isPrimary   = (tru_gprimary == 1);
    const bool isInZVtxCut = ((tru_gvz > zVtxRange.first) && (tru_gvz < zVtxRange.second));
    if (useOnlyPrimTrks && !isPrimary)   continue;
    if (doZVtxCut       && !isInZVtxCut) continue;
    if (doPhiCut        && isNearSector) continue;

    // fill truth 1D histograms
    vecHist1D[Var::NTot][Type::Truth]  -> Fill(tru_gntot);
    vecHist1D[Var::NIntt][Type::Truth] -> Fill(tru_gnintt);
    vecHist1D[Var::NMvtx][Type::Truth] -> Fill(tru_gnmaps);
    vecHist1D[Var::NTpc][Type::Truth]  -> Fill(tru_gntpc);
    vecHist1D[Var::RTot][Type::Truth]  -> Fill(1.);
    vecHist1D[Var::RIntt][Type::Truth] -> Fill(1.);
    vecHist1D[Var::RMvtx][Type::Truth] -> Fill(1.);
    vecHist1D[Var::RTpc][Type::Truth]  -> Fill(1.);
    vecHist1D[Var::Phi][Type::Truth]   -> Fill(tru_gphi);
    vecHist1D[Var::Eta][Type::Truth]   -> Fill(tru_geta);
    vecHist1D[Var::Pt][Type::Truth]    -> Fill(tru_gpt);
    vecHist1D[Var::Frac][Type::Truth]  -> Fill(1.);

    // fill truth 2D histograms
    vecHist2D[Var::NTot][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, tru_gntot);
    vecHist2D[Var::NIntt][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, tru_gnintt);
    vecHist2D[Var::NMvtx][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, tru_gnmaps);
    vecHist2D[Var::NTpc][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, tru_gntpc);
    vecHist2D[Var::RTot][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    vecHist2D[Var::RIntt][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    vecHist2D[Var::RMvtx][Type::Truth][Comp::VsTruthPt] -> Fill(tru_gpt, 1.);
    vecHist2D[Var::RTpc][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);
    vecHist2D[Var::Phi][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, tru_gphi);
    vecHist2D[Var::Eta][Type::Truth][Comp::VsTruthPt]   -> Fill(tru_gpt, tru_geta);
    vecHist2D[Var::Pt][Type::Truth][Comp::VsTruthPt]    -> Fill(tru_gpt, tru_gpt);
    vecHist2D[Var::Frac][Type::Truth][Comp::VsTruthPt]  -> Fill(tru_gpt, 1.);

    vecHist2D[Var::NTot][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, tru_gntot);
    vecHist2D[Var::NIntt][Type::Truth][Comp::VsNumTpc] -> Fill(tru_gntpc, tru_gnintt);
    vecHist2D[Var::NMvtx][Type::Truth][Comp::VsNumTpc] -> Fill(tru_gntpc, tru_gnmaps);
    vecHist2D[Var::NTpc][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, tru_gntpc);
    vecHist2D[Var::RTot][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, 1.);
    vecHist2D[Var::RIntt][Type::Truth][Comp::VsNumTpc] -> Fill(tru_gntpc, 1.);
    vecHist2D[Var::RMvtx][Type::Truth][Comp::VsNumTpc] -> Fill(tru_gntpc, 1.);
    vecHist2D[Var::RTpc][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, 1.);
    vecHist2D[Var::Phi][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_gntpc, tru_gphi);
    vecHist2D[Var::Eta][Type::Truth][Comp::VsNumTpc]   -> Fill(tru_gntpc, tru_geta);
    vecHist2D[Var::Pt][Type::Truth][Comp::VsNumTpc]    -> Fill(tru_gntpc, tru_gpt);
    vecHist2D[Var::Frac][Type::Truth][Comp::VsNumTpc]  -> Fill(tru_gntpc, 1.);
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

    // skip nan's
    if (isnan(rec_gpt)) continue;

    // run calculations
    const double rec_ntot   = rec_nintt + rec_nmaps + rec_ntpc;
    const double rec_gntot  = rec_gnintt + rec_gnmaps + rec_gntpc;
    const double rec_rtot   = rec_ntot / rec_gntot;
    const double rec_rintt  = rec_nintt / rec_gnintt;
    const double rec_rmaps  = rec_nmaps / rec_gnmaps;
    const double rec_rtpc   = rec_ntpc / rec_gntpc;
    const double rec_ptfrac = rec_pt / rec_gpt;

    // check if near sector
    bool isNearSector = false;
    if (doPhiCut) {
      for (size_t iSector = 0; iSector < NSectors; iSector++) {
       const float cutVal = sigCutVal * phiSectors[iSector].second;
        const float minPhi = phiSectors[iSector].first - cutVal;
        const float maxPhi = phiSectors[iSector].first + cutVal;
        const bool  isNear = ((rec_phi >= minPhi) && (rec_phi <= maxPhi));
        if (isNear) {
          isNearSector = true;
          break;
        }
      }  // end sector loop
    }  // end if (doPhiCut)

    // apply cuts
    const bool isPrimary   = (rec_gprimary == 1);
    const bool isInZVtxCut = ((rec_gvz > zVtxRange.first) && (rec_gvz < zVtxRange.second));
    if (useOnlyPrimTrks && !isPrimary)   continue;
    if (doZVtxCut       && !isInZVtxCut) continue;
    if (doPhiCut        && isNearSector) continue;

    // fill all matched reco 1D histograms
    vecHist1D[Var::NTot][Type::Track]  -> Fill(rec_ntot);
    vecHist1D[Var::NIntt][Type::Track] -> Fill(rec_nintt);
    vecHist1D[Var::NMvtx][Type::Track] -> Fill(rec_nmaps);
    vecHist1D[Var::NTpc][Type::Track]  -> Fill(rec_ntpc);
    vecHist1D[Var::RTot][Type::Track]  -> Fill(rec_rtot);
    vecHist1D[Var::RIntt][Type::Track] -> Fill(rec_rintt);
    vecHist1D[Var::RMvtx][Type::Track] -> Fill(rec_rmaps);
    vecHist1D[Var::RTpc][Type::Track]  -> Fill(rec_rtpc);
    vecHist1D[Var::Phi][Type::Track]   -> Fill(rec_phi);
    vecHist1D[Var::Eta][Type::Track]   -> Fill(rec_eta);
    vecHist1D[Var::Pt][Type::Track]    -> Fill(rec_pt);
    vecHist1D[Var::Frac][Type::Track]  -> Fill(rec_ptfrac);

    // fill all matched reco 2D histograms
    vecHist2D[Var::NTot][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ntot);
    vecHist2D[Var::NIntt][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_nintt);
    vecHist2D[Var::NMvtx][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_nmaps);
    vecHist2D[Var::NTpc][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ntpc);
    vecHist2D[Var::RTot][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rtot);
    vecHist2D[Var::RIntt][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rintt);
    vecHist2D[Var::RMvtx][Type::Track][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rmaps);
    vecHist2D[Var::RTpc][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rtpc);
    vecHist2D[Var::Phi][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_phi);
    vecHist2D[Var::Eta][Type::Track][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_eta);
    vecHist2D[Var::Pt][Type::Track][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_pt);
    vecHist2D[Var::Frac][Type::Track][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptfrac);

    vecHist2D[Var::NTot][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ntot);
    vecHist2D[Var::NIntt][Type::Track][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_nintt);
    vecHist2D[Var::NMvtx][Type::Track][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_nmaps);
    vecHist2D[Var::NTpc][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ntpc);
    vecHist2D[Var::RTot][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rtpc);
    vecHist2D[Var::RIntt][Type::Track][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_rintt);
    vecHist2D[Var::RMvtx][Type::Track][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_rmaps);
    vecHist2D[Var::RTpc][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rtpc);
    vecHist2D[Var::Phi][Type::Track][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_phi);
    vecHist2D[Var::Eta][Type::Track][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_eta);
    vecHist2D[Var::Pt][Type::Track][Comp::VsNumTpc]    -> Fill(rec_gntpc, rec_pt);
    vecHist2D[Var::Frac][Type::Track][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ptfrac);

    // fill weird and normal matched reco 1D histograms
    const bool isNormalTrack = ((rec_ptfrac >= oddPtFrac.first) && (rec_ptfrac <= oddPtFrac.second));
    if (isNormalTrack) {
      vecHist1D[Var::NTot][Type::Normal]  -> Fill(rec_ntot);
      vecHist1D[Var::NIntt][Type::Normal] -> Fill(rec_nintt);
      vecHist1D[Var::NMvtx][Type::Normal] -> Fill(rec_nmaps);
      vecHist1D[Var::NTpc][Type::Normal]  -> Fill(rec_ntpc);
      vecHist1D[Var::RTot][Type::Normal]  -> Fill(rec_rtot);
      vecHist1D[Var::RIntt][Type::Normal] -> Fill(rec_rintt);
      vecHist1D[Var::RMvtx][Type::Normal] -> Fill(rec_rmaps);
      vecHist1D[Var::RTpc][Type::Normal]  -> Fill(rec_rtpc);
      vecHist1D[Var::Phi][Type::Normal]   -> Fill(rec_phi);
      vecHist1D[Var::Eta][Type::Normal]   -> Fill(rec_eta);
      vecHist1D[Var::Pt][Type::Normal]    -> Fill(rec_pt);
      vecHist1D[Var::Frac][Type::Normal]  -> Fill(rec_ptfrac);

      vecHist2D[Var::NTot][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ntot);
      vecHist2D[Var::NIntt][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_nintt);
      vecHist2D[Var::NMvtx][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_nmaps);
      vecHist2D[Var::NTpc][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ntpc);
      vecHist2D[Var::RTot][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rtot);
      vecHist2D[Var::RIntt][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rintt);
      vecHist2D[Var::RMvtx][Type::Normal][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rmaps);
      vecHist2D[Var::RTpc][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rtpc);
      vecHist2D[Var::Phi][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_phi);
      vecHist2D[Var::Eta][Type::Normal][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_eta);
      vecHist2D[Var::Pt][Type::Normal][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_pt);
      vecHist2D[Var::Frac][Type::Normal][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptfrac);

      vecHist2D[Var::NTot][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ntot);
      vecHist2D[Var::NIntt][Type::Normal][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_nintt);
      vecHist2D[Var::NMvtx][Type::Normal][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_nmaps);
      vecHist2D[Var::NTpc][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ntpc);
      vecHist2D[Var::RTot][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rtot);
      vecHist2D[Var::RIntt][Type::Normal][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_rintt);
      vecHist2D[Var::RMvtx][Type::Normal][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_rmaps);
      vecHist2D[Var::RTpc][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rtpc);
      vecHist2D[Var::Phi][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_phi);
      vecHist2D[Var::Eta][Type::Normal][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_eta);
      vecHist2D[Var::Pt][Type::Normal][Comp::VsNumTpc]    -> Fill(rec_gntpc, rec_pt);
      vecHist2D[Var::Frac][Type::Normal][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ptfrac);
    } else {
      vecHist1D[Var::NTot][Type::Weird]  -> Fill(rec_ntot);
      vecHist1D[Var::NIntt][Type::Weird] -> Fill(rec_nintt);
      vecHist1D[Var::NMvtx][Type::Weird] -> Fill(rec_nmaps);
      vecHist1D[Var::NTpc][Type::Weird]  -> Fill(rec_ntpc);
      vecHist1D[Var::RTot][Type::Weird]  -> Fill(rec_rtot);
      vecHist1D[Var::RIntt][Type::Weird] -> Fill(rec_rintt);
      vecHist1D[Var::RMvtx][Type::Weird] -> Fill(rec_rmaps);
      vecHist1D[Var::RTpc][Type::Weird]  -> Fill(rec_rtpc);
      vecHist1D[Var::Phi][Type::Weird]   -> Fill(rec_phi);
      vecHist1D[Var::Eta][Type::Weird]   -> Fill(rec_eta);
      vecHist1D[Var::Pt][Type::Weird]    -> Fill(rec_pt);
      vecHist1D[Var::Frac][Type::Weird]  -> Fill(rec_ptfrac);

      vecHist2D[Var::NTot][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ntot);
      vecHist2D[Var::NIntt][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_nintt);
      vecHist2D[Var::NMvtx][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_nmaps);
      vecHist2D[Var::NTpc][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ntpc);
      vecHist2D[Var::RTot][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rtot);
      vecHist2D[Var::RIntt][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rintt);
      vecHist2D[Var::RMvtx][Type::Weird][Comp::VsTruthPt] -> Fill(rec_gpt, rec_rmaps);
      vecHist2D[Var::RTpc][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_rtpc);
      vecHist2D[Var::Phi][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_phi);
      vecHist2D[Var::Eta][Type::Weird][Comp::VsTruthPt]   -> Fill(rec_gpt, rec_eta);
      vecHist2D[Var::Pt][Type::Weird][Comp::VsTruthPt]    -> Fill(rec_gpt, rec_pt);
      vecHist2D[Var::Frac][Type::Weird][Comp::VsTruthPt]  -> Fill(rec_gpt, rec_ptfrac);

      vecHist2D[Var::NTot][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ntot);
      vecHist2D[Var::NIntt][Type::Weird][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_nintt);
      vecHist2D[Var::NMvtx][Type::Weird][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_nmaps);
      vecHist2D[Var::NTpc][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ntpc);
      vecHist2D[Var::RTot][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rtpc);
      vecHist2D[Var::RIntt][Type::Weird][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_rintt);
      vecHist2D[Var::RMvtx][Type::Weird][Comp::VsNumTpc] -> Fill(rec_gntpc, rec_rmaps);
      vecHist2D[Var::RTpc][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_rtpc);
      vecHist2D[Var::Phi][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_phi);
      vecHist2D[Var::Eta][Type::Weird][Comp::VsNumTpc]   -> Fill(rec_gntpc, rec_eta);
      vecHist2D[Var::Pt][Type::Weird][Comp::VsNumTpc]    -> Fill(rec_gntpc, rec_pt);
      vecHist2D[Var::Frac][Type::Weird][Comp::VsNumTpc]  -> Fill(rec_gntpc, rec_ptfrac);
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

  // exit
  cout << "  Finished old evaluator plot script!\n" << endl;
  return;

}

// end ------------------------------------------------------------------------

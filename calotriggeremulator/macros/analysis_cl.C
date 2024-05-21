#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <vector>

void analysis_cl() {
  // Base directories and parameters
  //const char* baseDirEmulator = "/sphenix/user/hanpuj/Jettrigger/condor/condor_emulator/sep";
  // const char* baseDirEmulator = "/sphenix/tg/tg01/jets/hanpuj/condor_emulator_caliblut/lut_sep";
  // const char* baseDirJet = "/sphenix/user/hanpuj/Jettrigger/condor/condor_jet";
  const char* baseDirEmulator = "/sphenix/user/jamesj3j3/analysis/calotriggeremulator/macros";
  const char* baseDirJet = "/sphenix/user/jamesj3j3/analysis/JS-Jet/JetValidation/macro";
  const int n_emulator_file[1] = {1875}; //, 1875, 1875, 1875, 1875, 1875, 1875, 1874};
  const int n_jet_file = 1;
  const int expect_emulator_events = 29998000;
  const int expect_jet_events = 28987925;

  const int n_hcal_etabin = 24;
  const int n_hcal_phibin = 64;
  const int n_emcal_etabin = 96;
  const int n_emcal_phibin = 256;
  const int n_hcaltrigger_etabin = 12;
  const int n_hcaltrigger_phibin = 32;
  const int n_emcaltrigger_etabin = 48;
  const int n_emcaltrigger_phibin = 128;
  const int n_jettrigger_etabin = 9;
  const int n_jettrigger_phibin = 32;
  const int jettrigger_min_etabin[9] = {0, 2, 4, 6, 8, 10, 12, 14, 16};
  const int jettrigger_max_etabin[9] = {7, 9, 11, 13, 15, 17, 19, 21, 23};
  const int jettrigger_min_phibin[32] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62};
  const int jettrigger_max_phibin[32] = {7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69}; // wrap around 0 and 63 in analysis part.
  const float eta_map[n_hcal_etabin] = {-1.05417,-0.9625,-0.870833,-0.779167,-0.6875,-0.595833,-0.504167,-0.4125,-0.320833,-0.229167,-0.1375,-0.0458333,0.0458333,0.1375,0.229167,0.320833,0.4125,0.504167,0.595833,0.6875,0.779167,0.870833,0.9625,1.05417};

  //const int n_threshold = 15;
  //int trigger_threshold[n_threshold] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20};
  const int n_threshold = 5;
  int trigger_threshold[n_threshold] = {1, 2, 3, 4, 5};
  int fired_event[n_threshold] = {0};

  TFile* output = new TFile("output_cl.root", "RECREATE");

  // Histograms
  TH2D* h_2D_fired_rate[n_threshold];
  TH2D* h_2D_fired_leadjet2_triggeret[n_threshold];
  TH2D* h_2D_fired_leadjet3_triggeret[n_threshold];
  TH2D* h_2D_fired_leadjet4_triggeret[n_threshold];
  TH2D* h_2D_fired_leadjet5_triggeret[n_threshold];
  TH2D* h_2D_fired_leadjet2_triggeret_emcal[n_threshold];
  TH2D* h_2D_fired_leadjet3_triggeret_emcal[n_threshold];
  TH2D* h_2D_fired_leadjet4_triggeret_emcal[n_threshold];
  TH2D* h_2D_fired_leadjet5_triggeret_emcal[n_threshold];
  TH2D* h_2D_fired_leadjet2_triggeret_ihcal[n_threshold];
  TH2D* h_2D_fired_leadjet3_triggeret_ihcal[n_threshold];
  TH2D* h_2D_fired_leadjet4_triggeret_ihcal[n_threshold];
  TH2D* h_2D_fired_leadjet5_triggeret_ihcal[n_threshold];
  TH2D* h_2D_fired_leadjet2_triggeret_ohcal[n_threshold];
  TH2D* h_2D_fired_leadjet3_triggeret_ohcal[n_threshold];
  TH2D* h_2D_fired_leadjet4_triggeret_ohcal[n_threshold];
  TH2D* h_2D_fired_leadjet5_triggeret_ohcal[n_threshold];
  TH2D* h_2D_fired_leadjet2_triggere[n_threshold];
  TH2D* h_2D_fired_leadjet3_triggere[n_threshold];
  TH2D* h_2D_fired_leadjet4_triggere[n_threshold];
  TH2D* h_2D_fired_leadjet5_triggere[n_threshold];
  TH2D* h_2D_fired_leadjet2_triggere_emcal[n_threshold];
  TH2D* h_2D_fired_leadjet3_triggere_emcal[n_threshold];
  TH2D* h_2D_fired_leadjet4_triggere_emcal[n_threshold];
  TH2D* h_2D_fired_leadjet5_triggere_emcal[n_threshold];
  TH2D* h_2D_fired_leadjet2_triggere_ihcal[n_threshold];
  TH2D* h_2D_fired_leadjet3_triggere_ihcal[n_threshold];
  TH2D* h_2D_fired_leadjet4_triggere_ihcal[n_threshold];
  TH2D* h_2D_fired_leadjet5_triggere_ihcal[n_threshold];
  TH2D* h_2D_fired_leadjet2_triggere_ohcal[n_threshold];
  TH2D* h_2D_fired_leadjet3_triggere_ohcal[n_threshold];
  TH2D* h_2D_fired_leadjet4_triggere_ohcal[n_threshold];
  TH2D* h_2D_fired_leadjet5_triggere_ohcal[n_threshold];
  TH1D* h_1D_fired_jet2pt[n_threshold];
  TH1D* h_1D_fired_jet3pt[n_threshold];
  TH1D* h_1D_fired_jet4pt[n_threshold];
  TH1D* h_1D_fired_jet5pt[n_threshold];
  TH1D* h_1D_jet2pt_efficiency[n_threshold];
  TH1D* h_1D_jet3pt_efficiency[n_threshold];
  TH1D* h_1D_jet4pt_efficiency[n_threshold];
  TH1D* h_1D_jet5pt_efficiency[n_threshold];
  for (int i = 0; i < n_threshold; ++i) {
    h_2D_fired_rate[i] = new TH2D(Form("h_2D_fired_rate_thre%d", trigger_threshold[i]), Form("Fired rate of trigger threshold = %d;#eta;#phi", trigger_threshold[i]), n_hcal_etabin, 0, n_hcal_etabin, n_hcal_phibin, 0, n_hcal_phibin);
    h_2D_fired_leadjet2_triggeret[i] = new TH2D(Form("h_2D_fired_leadjet2_triggeret_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet3_triggeret[i] = new TH2D(Form("h_2D_fired_leadjet3_triggeret_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet4_triggeret[i] = new TH2D(Form("h_2D_fired_leadjet4_triggeret_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet5_triggeret[i] = new TH2D(Form("h_2D_fired_leadjet5_triggeret_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet2_triggeret_emcal[i] = new TH2D(Form("h_2D_fired_leadjet2_triggeret_emcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);EMCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet3_triggeret_emcal[i] = new TH2D(Form("h_2D_fired_leadjet3_triggeret_emcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);EMCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet4_triggeret_emcal[i] = new TH2D(Form("h_2D_fired_leadjet4_triggeret_emcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);EMCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet5_triggeret_emcal[i] = new TH2D(Form("h_2D_fired_leadjet5_triggeret_emcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);EMCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet2_triggeret_ihcal[i] = new TH2D(Form("h_2D_fired_leadjet2_triggeret_ihcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);iHCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet3_triggeret_ihcal[i] = new TH2D(Form("h_2D_fired_leadjet3_triggeret_ihcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);iHCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet4_triggeret_ihcal[i] = new TH2D(Form("h_2D_fired_leadjet4_triggeret_ihcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);iHCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet5_triggeret_ihcal[i] = new TH2D(Form("h_2D_fired_leadjet5_triggeret_ihcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);iHCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet2_triggeret_ohcal[i] = new TH2D(Form("h_2D_fired_leadjet2_triggeret_ohcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);oHCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet3_triggeret_ohcal[i] = new TH2D(Form("h_2D_fired_leadjet3_triggeret_ohcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);oHCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet4_triggeret_ohcal[i] = new TH2D(Form("h_2D_fired_leadjet4_triggeret_ohcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);oHCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet5_triggeret_ohcal[i] = new TH2D(Form("h_2D_fired_leadjet5_triggeret_ohcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger ET of trigger threshold = %d;jet p_{T} (GeV);oHCal trigger patch E_{T} (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet2_triggere[i] = new TH2D(Form("h_2D_fired_leadjet2_triggere_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet3_triggere[i] = new TH2D(Form("h_2D_fired_leadjet3_triggere_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet4_triggere[i] = new TH2D(Form("h_2D_fired_leadjet4_triggere_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet5_triggere[i] = new TH2D(Form("h_2D_fired_leadjet5_triggere_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet2_triggere_emcal[i] = new TH2D(Form("h_2D_fired_leadjet2_triggere_emcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);EMCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet3_triggere_emcal[i] = new TH2D(Form("h_2D_fired_leadjet3_triggere_emcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);EMCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet4_triggere_emcal[i] = new TH2D(Form("h_2D_fired_leadjet4_triggere_emcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);EMCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet5_triggere_emcal[i] = new TH2D(Form("h_2D_fired_leadjet5_triggere_emcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);EMCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet2_triggere_ihcal[i] = new TH2D(Form("h_2D_fired_leadjet2_triggere_ihcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);iHCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet3_triggere_ihcal[i] = new TH2D(Form("h_2D_fired_leadjet3_triggere_ihcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);iHCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet4_triggere_ihcal[i] = new TH2D(Form("h_2D_fired_leadjet4_triggere_ihcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);iHCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet5_triggere_ihcal[i] = new TH2D(Form("h_2D_fired_leadjet5_triggere_ihcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);iHCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet2_triggere_ohcal[i] = new TH2D(Form("h_2D_fired_leadjet2_triggere_ohcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);oHCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet3_triggere_ohcal[i] = new TH2D(Form("h_2D_fired_leadjet3_triggere_ohcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);oHCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet4_triggere_ohcal[i] = new TH2D(Form("h_2D_fired_leadjet4_triggere_ohcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);oHCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_2D_fired_leadjet5_triggere_ohcal[i] = new TH2D(Form("h_2D_fired_leadjet5_triggere_ohcal_thre%d", trigger_threshold[i]), Form("Leading jet trigger E of trigger threshold = %d;jet p_{T} (GeV);oHCal trigger patch E (GeV)", trigger_threshold[i]), 300, 0, 60, 300, 0, 60);
    h_1D_fired_jet2pt[i] = new TH1D(Form("h_1D_fired_jet2pt_thre%d", trigger_threshold[i]), Form("Fired jet (R=0.2) p_{T} of trigger threshold = %d;jet p_{T} (GeV);Counts", trigger_threshold[i]), 300, 0, 30);
    h_1D_fired_jet3pt[i] = new TH1D(Form("h_1D_fired_jet3pt_thre%d", trigger_threshold[i]), Form("Fired jet (R=0.3) p_{T} of trigger threshold = %d;jet p_{T} (GeV);Counts", trigger_threshold[i]), 300, 0, 30);
    h_1D_fired_jet4pt[i] = new TH1D(Form("h_1D_fired_jet4pt_thre%d", trigger_threshold[i]), Form("Fired jet (R=0.4) p_{T} of trigger threshold = %d;jet p_{T} (GeV);Counts", trigger_threshold[i]), 300, 0, 30);
    h_1D_fired_jet5pt[i] = new TH1D(Form("h_1D_fired_jet5pt_thre%d", trigger_threshold[i]), Form("Fired jet (R=0.5) p_{T} of trigger threshold = %d;jet p_{T} (GeV);Counts", trigger_threshold[i]), 300, 0, 30);
    h_1D_jet2pt_efficiency[i] = new TH1D(Form("h_1D_jet2pt_efficiency_thre%d", trigger_threshold[i]), Form("Jet (R=0.2) efficiency of trigger threshold = %d;jet p_{T} (GeV);Efficiency", trigger_threshold[i]), 300, 0, 30);
    h_1D_jet3pt_efficiency[i] = new TH1D(Form("h_1D_jet3pt_efficiency_thre%d", trigger_threshold[i]), Form("Jet (R=0.3) efficiency of trigger threshold = %d;jet p_{T} (GeV);Efficiency", trigger_threshold[i]), 300, 0, 30);
    h_1D_jet4pt_efficiency[i] = new TH1D(Form("h_1D_jet4pt_efficiency_thre%d", trigger_threshold[i]), Form("Jet (R=0.4) efficiency of trigger threshold = %d;jet p_{T} (GeV);Efficiency", trigger_threshold[i]), 300, 0, 30);
    h_1D_jet5pt_efficiency[i] = new TH1D(Form("h_1D_jet5pt_efficiency_thre%d", trigger_threshold[i]), Form("Jet (R=0.5) efficiency of trigger threshold = %d;jet p_{T} (GeV);Efficiency", trigger_threshold[i]), 300, 0, 30);
  }
  TH2D* h_2D_all_leadjet2_triggeret = new TH2D("h_2D_all_leadjet2_triggeret", "All leading jet trigger ET;jet p_{T} (GeV);trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet3_triggeret = new TH2D("h_2D_all_leadjet3_triggeret", "All leading jet trigger ET;jet p_{T} (GeV);trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet4_triggeret = new TH2D("h_2D_all_leadjet4_triggeret", "All leading jet trigger ET;jet p_{T} (GeV);trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet5_triggeret = new TH2D("h_2D_all_leadjet5_triggeret", "All leading jet trigger ET;jet p_{T} (GeV);trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet2_triggeret_emcal = new TH2D("h_2D_all_leadjet2_triggeret_emcal", "All leading jet trigger ET;jet p_{T} (GeV);EMCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet3_triggeret_emcal = new TH2D("h_2D_all_leadjet3_triggeret_emcal", "All leading jet trigger ET;jet p_{T} (GeV);EMCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet4_triggeret_emcal = new TH2D("h_2D_all_leadjet4_triggeret_emcal", "All leading jet trigger ET;jet p_{T} (GeV);EMCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet5_triggeret_emcal = new TH2D("h_2D_all_leadjet5_triggeret_emcal", "All leading jet trigger ET;jet p_{T} (GeV);EMCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet2_triggeret_ihcal = new TH2D("h_2D_all_leadjet2_triggeret_ihcal", "All leading jet trigger ET;jet p_{T} (GeV);iHCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet3_triggeret_ihcal = new TH2D("h_2D_all_leadjet3_triggeret_ihcal", "All leading jet trigger ET;jet p_{T} (GeV);iHCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet4_triggeret_ihcal = new TH2D("h_2D_all_leadjet4_triggeret_ihcal", "All leading jet trigger ET;jet p_{T} (GeV);iHCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet5_triggeret_ihcal = new TH2D("h_2D_all_leadjet5_triggeret_ihcal", "All leading jet trigger ET;jet p_{T} (GeV);iHCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet2_triggeret_ohcal = new TH2D("h_2D_all_leadjet2_triggeret_ohcal", "All leading jet trigger ET;jet p_{T} (GeV);oHCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet3_triggeret_ohcal = new TH2D("h_2D_all_leadjet3_triggeret_ohcal", "All leading jet trigger ET;jet p_{T} (GeV);oHCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet4_triggeret_ohcal = new TH2D("h_2D_all_leadjet4_triggeret_ohcal", "All leading jet trigger ET;jet p_{T} (GeV);oHCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet5_triggeret_ohcal = new TH2D("h_2D_all_leadjet5_triggeret_ohcal", "All leading jet trigger ET;jet p_{T} (GeV);oHCal trigger patch E_{T} (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet2_triggere = new TH2D("h_2D_all_leadjet2_triggere", "All leading jet trigger E;jet p_{T} (GeV);trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet3_triggere = new TH2D("h_2D_all_leadjet3_triggere", "All leading jet trigger E;jet p_{T} (GeV);trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet4_triggere = new TH2D("h_2D_all_leadjet4_triggere", "All leading jet trigger E;jet p_{T} (GeV);trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet5_triggere = new TH2D("h_2D_all_leadjet5_triggere", "All leading jet trigger E;jet p_{T} (GeV);trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet2_triggere_emcal = new TH2D("h_2D_all_leadjet2_triggere_emcal", "All leading jet trigger E;jet p_{T} (GeV);EMCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet3_triggere_emcal = new TH2D("h_2D_all_leadjet3_triggere_emcal", "All leading jet trigger E;jet p_{T} (GeV);EMCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet4_triggere_emcal = new TH2D("h_2D_all_leadjet4_triggere_emcal", "All leading jet trigger E;jet p_{T} (GeV);EMCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet5_triggere_emcal = new TH2D("h_2D_all_leadjet5_triggere_emcal", "All leading jet trigger E;jet p_{T} (GeV);EMCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet2_triggere_ihcal = new TH2D("h_2D_all_leadjet2_triggere_ihcal", "All leading jet trigger E;jet p_{T} (GeV);iHCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet3_triggere_ihcal = new TH2D("h_2D_all_leadjet3_triggere_ihcal", "All leading jet trigger E;jet p_{T} (GeV);iHCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet4_triggere_ihcal = new TH2D("h_2D_all_leadjet4_triggere_ihcal", "All leading jet trigger E;jet p_{T} (GeV);iHCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet5_triggere_ihcal = new TH2D("h_2D_all_leadjet5_triggere_ihcal", "All leading jet trigger E;jet p_{T} (GeV);iHCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet2_triggere_ohcal = new TH2D("h_2D_all_leadjet2_triggere_ohcal", "All leading jet trigger E;jet p_{T} (GeV);oHCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet3_triggere_ohcal = new TH2D("h_2D_all_leadjet3_triggere_ohcal", "All leading jet trigger E;jet p_{T} (GeV);oHCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet4_triggere_ohcal = new TH2D("h_2D_all_leadjet4_triggere_ohcal", "All leading jet trigger E;jet p_{T} (GeV);oHCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH2D* h_2D_all_leadjet5_triggere_ohcal = new TH2D("h_2D_all_leadjet5_triggere_ohcal", "All leading jet trigger E;jet p_{T} (GeV);oHCal trigger patch E (GeV)", 300, 0, 60, 300, 0, 60);
  TH1D* h_1D_all_jet2pt = new TH1D("h_1D_all_jet2pt", "All jet p_{T};jet p_{T} (GeV);Counts", 300, 0, 30);
  TH1D* h_1D_all_jet3pt = new TH1D("h_1D_all_jet3pt", "All jet p_{T};jet p_{T} (GeV);Counts", 300, 0, 30);
  TH1D* h_1D_all_jet4pt = new TH1D("h_1D_all_jet4pt", "All jet p_{T};jet p_{T} (GeV);Counts", 300, 0, 30);
  TH1D* h_1D_all_jet5pt = new TH1D("h_1D_all_jet5pt", "All jet p_{T};jet p_{T} (GeV);Counts", 300, 0, 30);

  TH1D* h_1D_emcal_towerenergy = new TH1D("h_1D_emcal_towerenergy", "EMCAL tower energy;Energy (GeV);Counts", 600, -0.2, 0.4);
  TH1D* h_1D_ihcal_towerenergy = new TH1D("h_1D_ihcal_towerenergy", "IHCAL tower energy;Energy (GeV);Counts", 600, -0.2, 0.4);
  TH1D* h_1D_ohcal_towerenergy = new TH1D("h_1D_ohcal_towerenergy", "OHCAL tower energy;Energy (GeV);Counts", 600, -0.2, 0.4);

  TGraphErrors* g_1D_efficiency = new TGraphErrors(n_threshold);

  // Check histograms
  //TH2D* h_2D_emcalcheck = new TH2D("h_2D_emcalcheck", "EMCAL check;#eta;#phi", n_emcal_etabin, 0, n_emcal_etabin, n_emcal_phibin, 0, n_emcal_phibin);
  //TH2D* h_2D_ihcalcheck = new TH2D("h_2D_ihcalcheck", "IHCAL check;#eta;#phi", n_hcal_etabin, 0, n_hcal_etabin, n_hcal_phibin, 0, n_hcal_phibin);
  //TH2D* h_2D_ohcalcheck = new TH2D("h_2D_ohcalcheck", "OHCAL check;#eta;#phi", n_hcal_etabin, 0, n_hcal_etabin, n_hcal_phibin, 0, n_hcal_phibin);

  // Emulator data chains
  TChain chain_emulator("ttree");
  UInt_t  trigger_sum_emcal[6144], trigger_sum_ihcal[384], trigger_sum_ohcal[384];
  UInt_t  trigger_sum_emcal_ll1[384], trigger_sum_hcal_ll1[384];
  UInt_t  trigger_sum_jet[288];
  std::vector<float>* emcal_energy = nullptr;
  std::vector<float>* emcal_etabin = nullptr;
  std::vector<float>* emcal_phibin = nullptr;
  std::vector<float>* ihcal_energy = nullptr;
  std::vector<float>* ihcal_etabin = nullptr;
  std::vector<float>* ihcal_phibin = nullptr;
  std::vector<float>* ohcal_energy = nullptr;
  std::vector<float>* ohcal_etabin = nullptr;
  std::vector<float>* ohcal_phibin = nullptr;

  chain_emulator.SetBranchStatus("*", 0); 
  chain_emulator.SetBranchStatus("trigger_sum_emcal", 1);
  chain_emulator.SetBranchStatus("trigger_sum_hcalin", 1);
  chain_emulator.SetBranchStatus("trigger_sum_hcalout", 1);
  chain_emulator.SetBranchStatus("trigger_sum_emcal_ll1", 1);
  chain_emulator.SetBranchStatus("trigger_sum_hcal_ll1", 1);
  chain_emulator.SetBranchStatus("trigger_sum_jet", 1);
  chain_emulator.SetBranchStatus("emcal_energy", 1);
  chain_emulator.SetBranchStatus("emcal_etabin", 1);
  chain_emulator.SetBranchStatus("emcal_phibin", 1);
  chain_emulator.SetBranchStatus("hcalin_energy", 1);
  chain_emulator.SetBranchStatus("hcalin_etabin", 1);
  chain_emulator.SetBranchStatus("hcalin_phibin", 1);
  chain_emulator.SetBranchStatus("hcalout_energy", 1);
  chain_emulator.SetBranchStatus("hcalout_etabin", 1);
  chain_emulator.SetBranchStatus("hcalout_phibin", 1);
  chain_emulator.SetBranchAddress("trigger_sum_emcal", trigger_sum_emcal);
  chain_emulator.SetBranchAddress("trigger_sum_hcalin", trigger_sum_ihcal);
  chain_emulator.SetBranchAddress("trigger_sum_hcalout", trigger_sum_ohcal);
  chain_emulator.SetBranchAddress("trigger_sum_emcal_ll1", trigger_sum_emcal_ll1);
  chain_emulator.SetBranchAddress("trigger_sum_hcal_ll1", trigger_sum_hcal_ll1);
  chain_emulator.SetBranchAddress("trigger_sum_jet", trigger_sum_jet);
  chain_emulator.SetBranchAddress("emcal_energy", &emcal_energy);
  chain_emulator.SetBranchAddress("emcal_etabin", &emcal_etabin);
  chain_emulator.SetBranchAddress("emcal_phibin", &emcal_phibin);
  chain_emulator.SetBranchAddress("hcalin_energy", &ihcal_energy);
  chain_emulator.SetBranchAddress("hcalin_etabin", &ihcal_etabin);
  chain_emulator.SetBranchAddress("hcalin_phibin", &ihcal_phibin);
  chain_emulator.SetBranchAddress("hcalout_energy", &ohcal_energy);
  chain_emulator.SetBranchAddress("hcalout_etabin", &ohcal_etabin);
  chain_emulator.SetBranchAddress("hcalout_phibin", &ohcal_phibin);

  // Jet data chains
  TChain chain_jet2("T"); // jet with radius 0.2
  TChain chain_jet3("T"); // jet with radius 0.3
  TChain chain_jet4("T"); // jet with radius 0.4
  TChain chain_jet5("T"); // jet with radius 0.5

  int m_event, n_recojet2;
  float zvtx;
  std::vector<float>* recojet2_eta = nullptr;
  std::vector<float>* recojet2_phi = nullptr;
  std::vector<float>* recojet2_e = nullptr;
  std::vector<float>* recojet2_pt = nullptr;
  chain_jet2.SetBranchStatus("*", 0);
  chain_jet2.SetBranchStatus("m_event", 1);
  chain_jet2.SetBranchStatus("nJet", 1);
  chain_jet2.SetBranchStatus("zvtx", 1);
  chain_jet2.SetBranchStatus("eta", 1);
  chain_jet2.SetBranchStatus("phi", 1);
  chain_jet2.SetBranchStatus("e", 1);
  chain_jet2.SetBranchStatus("pt", 1);
  chain_jet2.SetBranchAddress("m_event", &m_event);
  chain_jet2.SetBranchAddress("nJet", &n_recojet2);
  chain_jet2.SetBranchAddress("zvtx", &zvtx);
  chain_jet2.SetBranchAddress("eta", &recojet2_eta);
  chain_jet2.SetBranchAddress("phi", &recojet2_phi);
  chain_jet2.SetBranchAddress("e", &recojet2_e);
  chain_jet2.SetBranchAddress("pt", &recojet2_pt);

  int n_recojet3;
  std::vector<float>* recojet3_eta = nullptr;
  std::vector<float>* recojet3_phi = nullptr;
  std::vector<float>* recojet3_e = nullptr;
  std::vector<float>* recojet3_pt = nullptr;
  chain_jet3.SetBranchStatus("*", 0);
  chain_jet3.SetBranchStatus("nJet", 1);
  chain_jet3.SetBranchStatus("eta", 1);
  chain_jet3.SetBranchStatus("phi", 1);
  chain_jet3.SetBranchStatus("e", 1);
  chain_jet3.SetBranchStatus("pt", 1);
  chain_jet3.SetBranchAddress("nJet", &n_recojet3);
  chain_jet3.SetBranchAddress("eta", &recojet3_eta);
  chain_jet3.SetBranchAddress("phi", &recojet3_phi);
  chain_jet3.SetBranchAddress("e", &recojet3_e);
  chain_jet3.SetBranchAddress("pt", &recojet3_pt);

  int n_recojet4;
  std::vector<float>* recojet4_eta = nullptr;
  std::vector<float>* recojet4_phi = nullptr;
  std::vector<float>* recojet4_e = nullptr;
  std::vector<float>* recojet4_pt = nullptr;
  chain_jet4.SetBranchStatus("*", 0);
  chain_jet4.SetBranchStatus("nJet", 1);
  chain_jet4.SetBranchStatus("eta", 1);
  chain_jet4.SetBranchStatus("phi", 1);
  chain_jet4.SetBranchStatus("e", 1);
  chain_jet4.SetBranchStatus("pt", 1);
  chain_jet4.SetBranchAddress("nJet", &n_recojet4);
  chain_jet4.SetBranchAddress("eta", &recojet4_eta);
  chain_jet4.SetBranchAddress("phi", &recojet4_phi);
  chain_jet4.SetBranchAddress("e", &recojet4_e);
  chain_jet4.SetBranchAddress("pt", &recojet4_pt);

  int n_recojet5;
  std::vector<float>* recojet5_eta = nullptr;
  std::vector<float>* recojet5_phi = nullptr;
  std::vector<float>* recojet5_e = nullptr;
  std::vector<float>* recojet5_pt = nullptr;
  chain_jet5.SetBranchStatus("*", 0);
  chain_jet5.SetBranchStatus("nJet", 1);
  chain_jet5.SetBranchStatus("eta", 1);
  chain_jet5.SetBranchStatus("phi", 1);
  chain_jet5.SetBranchStatus("e", 1);
  chain_jet5.SetBranchStatus("pt", 1);
  chain_jet5.SetBranchAddress("nJet", &n_recojet5);
  chain_jet5.SetBranchAddress("eta", &recojet5_eta);
  chain_jet5.SetBranchAddress("phi", &recojet5_phi);
  chain_jet5.SetBranchAddress("e", &recojet5_e);
  chain_jet5.SetBranchAddress("pt", &recojet5_pt);

  // Loading files into chains
  for (int i = 0; i < 1; ++i) {
    for (int j = 0; j < n_emulator_file[i]; ++j) {
      chain_emulator.Add(Form("30GeVJetTestR04.root", baseDirEmulator, i, j));
    }
  }
  for (int j = 0; j < n_jet_file; ++j) {
    // chain_jet2.Add(Form("%s/0.2/OutDir%d/output.root", baseDirJet, j));
    // chain_jet3.Add(Form("%s/0.3/OutDir%d/output.root", baseDirJet, j));
    chain_jet4.Add(Form("outputJETVAL10000.root", baseDirJet, j));
    // chain_jet5.Add(Form("%s/0.5/OutDir%d/output.root", baseDirJet, j));
  }


//********** Data Analysis Loop **********//
  std::cout << "Data analysis started." << std::endl;
  //int n_events = chain_jet2.GetEntries(); // go through all jet files, and match the emulator event with jet event.
  //int n_events = 28987925; // hard coded number of events in jet files to save time.
  int n_events = 10000; // for testing.
  int temp_jet_event = 0;
  int jet_event_record = 0;
  int emulator_events;
  std::cout << "Total number of events: " << n_events << std::endl;
  for (Long64_t i = 0; i < n_events; i++) { // event loop start.
    if (i % 1 == 0) {
      std::cout << "Processing event " << i << "..." << std::endl;
    }

    // Load data
    chain_jet2.GetEntry(i);
    chain_jet3.GetEntry(i);
    chain_jet4.GetEntry(i);
    chain_jet5.GetEntry(i);
    if (TMath::Abs(zvtx) >= 10) continue; // skip events with zvtx >= 10.
    if (temp_jet_event > m_event) jet_event_record++; // check if the m_event is reset.
    temp_jet_event = m_event;
    emulator_events = jet_event_record * 10000 + m_event; // 50000 events per jet file, m_event is reset.
    chain_emulator.GetEntry(emulator_events); // re-match the emulator event with jet event. (jet drop events).

    // Check if the event is fired
    int fired_check[n_threshold] = {0};
    for (int nt = 0; nt < n_threshold; ++nt) {
      for (int njp = 0; njp < n_jettrigger_phibin; ++njp) {
        for (int nje = 0; nje < n_jettrigger_etabin; ++nje) {
          if (trigger_sum_jet[njp * n_jettrigger_etabin + nje] >= trigger_threshold[nt]) {
            fired_check[nt] = 1;
            for (int i = jettrigger_min_etabin[nje]; i <= jettrigger_max_etabin[nje]; ++i) {
              for (int j = jettrigger_min_phibin[njp]; j <= jettrigger_max_phibin[njp]; ++j) {
                if (j >= 64) {
                  h_2D_fired_rate[nt]->Fill(i, j-64, 1);
                } else {
                  h_2D_fired_rate[nt]->Fill(i, j, 1);
                }
              }
            }
          }
        }
      }
    }
    // Get the highest jet pT
    float highest_jet2_pt = -100;
    float highest_jet3_pt = -100;
    float highest_jet4_pt = -100;
    float highest_jet5_pt = -100;
    for (int nj = 0; nj < n_recojet2; ++nj) {
      if (recojet2_pt->at(nj) > highest_jet2_pt) highest_jet2_pt = recojet2_pt->at(nj);
      h_1D_all_jet2pt->Fill(recojet2_pt->at(nj));
    }
    for (int nj = 0; nj < n_recojet3; ++nj) {
      if (recojet3_pt->at(nj) > highest_jet3_pt) highest_jet3_pt = recojet3_pt->at(nj);
      h_1D_all_jet3pt->Fill(recojet3_pt->at(nj));
    }
    for (int nj = 0; nj < n_recojet4; ++nj) {
      if (recojet4_pt->at(nj) > highest_jet4_pt) highest_jet4_pt = recojet4_pt->at(nj);
      h_1D_all_jet4pt->Fill(recojet4_pt->at(nj));
    }
    for (int nj = 0; nj < n_recojet5; ++nj) {
      if (recojet5_pt->at(nj) > highest_jet5_pt) highest_jet5_pt = recojet5_pt->at(nj);
      h_1D_all_jet5pt->Fill(recojet5_pt->at(nj));
    }
    // Calculate the trigger patch ET, and get the highest trigger patch ET
    float emcal_tower_e[n_hcal_etabin][n_hcal_phibin];
    float ihcal_tower_e[n_hcal_etabin][n_hcal_phibin];
    float ohcal_tower_e[n_hcal_etabin][n_hcal_phibin];
    float trigger_emcal_et[n_jettrigger_etabin][n_jettrigger_phibin];
    float trigger_ihcal_et[n_jettrigger_etabin][n_jettrigger_phibin];
    float trigger_ohcal_et[n_jettrigger_etabin][n_jettrigger_phibin];
    float trigger_et[n_jettrigger_etabin][n_jettrigger_phibin];
    float trigger_emcal_e[n_jettrigger_etabin][n_jettrigger_phibin];
    float trigger_ihcal_e[n_jettrigger_etabin][n_jettrigger_phibin];
    float trigger_ohcal_e[n_jettrigger_etabin][n_jettrigger_phibin];
    float trigger_e[n_jettrigger_etabin][n_jettrigger_phibin];
    int highest_trigger_eta = -1;
    int highest_trigger_phi = -1;
    float highest_trigger_et = -10000;
    float highest_trigger_e;
    float highest_trigger_emcal_et, highest_trigger_ihcal_et, highest_trigger_ohcal_et;
    float highest_trigger_emcal_e, highest_trigger_ihcal_e, highest_trigger_ohcal_e;
    for (int i = 0; i < n_hcal_etabin; ++i) { // initialize the arrays.
      for (int j = 0; j < n_hcal_phibin; ++j) {
        emcal_tower_e[i][j] = 0;
      }
    }
    for (int i = 0; i < emcal_energy->size(); ++i) { // fill the emcal et and re-tower.
      emcal_tower_e[(int)emcal_etabin->at(i)/4][(int)emcal_phibin->at(i)/4] += emcal_energy->at(i);
      h_1D_emcal_towerenergy->Fill(emcal_energy->at(i));
    }
    for (int i = 0; i < ihcal_energy->size(); ++i) { // fill the ihcal and ohcal et.
      ihcal_tower_e[(int)ihcal_etabin->at(i)][(int)ihcal_phibin->at(i)] = ihcal_energy->at(i);
      ohcal_tower_e[(int)ohcal_etabin->at(i)][(int)ohcal_phibin->at(i)] = ohcal_energy->at(i);
      h_1D_ihcal_towerenergy->Fill(ihcal_energy->at(i));
      h_1D_ohcal_towerenergy->Fill(ohcal_energy->at(i));
    }
    for (int i = 0; i < n_jettrigger_etabin; ++i) { // calculate the trigger et, and get the highest trigger et.
      for (int j = 0; j < n_jettrigger_phibin; ++j) {
        trigger_emcal_e[i][j] = 0;
        trigger_ihcal_e[i][j] = 0;
        trigger_ohcal_e[i][j] = 0;
        trigger_e[i][j] = 0;
        trigger_emcal_et[i][j] = 0;
        trigger_ihcal_et[i][j] = 0;
        trigger_ohcal_et[i][j] = 0;
        trigger_et[i][j] = 0;
        for (int k = jettrigger_min_etabin[i]; k <= jettrigger_max_etabin[i]; ++k) {
          for (int l = jettrigger_min_phibin[j]; l <= jettrigger_max_phibin[j]; ++l) {
            if (l >= 64) {
              trigger_emcal_e[i][j] += emcal_tower_e[k][l-64];
              trigger_ihcal_e[i][j] += ihcal_tower_e[k][l-64];
              trigger_ohcal_e[i][j] += ohcal_tower_e[k][l-64];
              trigger_e[i][j] += emcal_tower_e[k][l-64] + ihcal_tower_e[k][l-64] + ohcal_tower_e[k][l-64];
              trigger_emcal_et[i][j] += emcal_tower_e[k][l-64] / (float)(TMath::CosH(eta_map[k]));
              trigger_ihcal_et[i][j] += ihcal_tower_e[k][l-64] / (float)(TMath::CosH(eta_map[k]));
              trigger_ohcal_et[i][j] += ohcal_tower_e[k][l-64] / (float)(TMath::CosH(eta_map[k]));
              trigger_et[i][j] += (emcal_tower_e[k][l-64] + ihcal_tower_e[k][l-64] + ohcal_tower_e[k][l-64]) / (float)(TMath::CosH(eta_map[k]));
            } else {
              trigger_emcal_e[i][j] += emcal_tower_e[k][l];
              trigger_ihcal_e[i][j] += ihcal_tower_e[k][l];
              trigger_ohcal_e[i][j] += ohcal_tower_e[k][l];
              trigger_e[i][j] += emcal_tower_e[k][l] + ihcal_tower_e[k][l] + ohcal_tower_e[k][l];
              trigger_emcal_et[i][j] += emcal_tower_e[k][l] / (float)(TMath::CosH(eta_map[k]));
              trigger_ihcal_et[i][j] += ihcal_tower_e[k][l] / (float)(TMath::CosH(eta_map[k]));
              trigger_ohcal_et[i][j] += ohcal_tower_e[k][l] / (float)(TMath::CosH(eta_map[k]));
              trigger_et[i][j] += (emcal_tower_e[k][l] + ihcal_tower_e[k][l] + ohcal_tower_e[k][l]) / (float)(TMath::CosH(eta_map[k]));
            }
          }
        }
        if (trigger_et[i][j] > highest_trigger_et) {
          highest_trigger_et = trigger_et[i][j];
          highest_trigger_emcal_et = trigger_emcal_et[i][j];
          highest_trigger_ihcal_et = trigger_ihcal_et[i][j];
          highest_trigger_ohcal_et = trigger_ohcal_et[i][j];
          highest_trigger_e = trigger_e[i][j];
          highest_trigger_emcal_e = trigger_emcal_e[i][j];
          highest_trigger_ihcal_e = trigger_ihcal_e[i][j];
          highest_trigger_ohcal_e = trigger_ohcal_e[i][j];
          highest_trigger_eta = i;
          highest_trigger_phi = j;
        }
      }
    }
    h_2D_all_leadjet2_triggeret->Fill(highest_jet2_pt, highest_trigger_et);
    h_2D_all_leadjet3_triggeret->Fill(highest_jet3_pt, highest_trigger_et);
    h_2D_all_leadjet4_triggeret->Fill(highest_jet4_pt, highest_trigger_et);
    h_2D_all_leadjet5_triggeret->Fill(highest_jet5_pt, highest_trigger_et);
    h_2D_all_leadjet2_triggeret_emcal->Fill(highest_jet2_pt, highest_trigger_emcal_et);
    h_2D_all_leadjet3_triggeret_emcal->Fill(highest_jet3_pt, highest_trigger_emcal_et);
    h_2D_all_leadjet4_triggeret_emcal->Fill(highest_jet4_pt, highest_trigger_emcal_et);
    h_2D_all_leadjet5_triggeret_emcal->Fill(highest_jet5_pt, highest_trigger_emcal_et);
    h_2D_all_leadjet2_triggeret_ihcal->Fill(highest_jet2_pt, highest_trigger_ihcal_et);
    h_2D_all_leadjet3_triggeret_ihcal->Fill(highest_jet3_pt, highest_trigger_ihcal_et);
    h_2D_all_leadjet4_triggeret_ihcal->Fill(highest_jet4_pt, highest_trigger_ihcal_et);
    h_2D_all_leadjet5_triggeret_ihcal->Fill(highest_jet5_pt, highest_trigger_ihcal_et);
    h_2D_all_leadjet2_triggeret_ohcal->Fill(highest_jet2_pt, highest_trigger_ohcal_et);
    h_2D_all_leadjet3_triggeret_ohcal->Fill(highest_jet3_pt, highest_trigger_ohcal_et);
    h_2D_all_leadjet4_triggeret_ohcal->Fill(highest_jet4_pt, highest_trigger_ohcal_et);
    h_2D_all_leadjet5_triggeret_ohcal->Fill(highest_jet5_pt, highest_trigger_ohcal_et);
    h_2D_all_leadjet2_triggere->Fill(highest_jet2_pt, highest_trigger_e);
    h_2D_all_leadjet3_triggere->Fill(highest_jet3_pt, highest_trigger_e);
    h_2D_all_leadjet4_triggere->Fill(highest_jet4_pt, highest_trigger_e);
    h_2D_all_leadjet5_triggere->Fill(highest_jet5_pt, highest_trigger_e);
    h_2D_all_leadjet2_triggere_emcal->Fill(highest_jet2_pt, highest_trigger_emcal_e);
    h_2D_all_leadjet3_triggere_emcal->Fill(highest_jet3_pt, highest_trigger_emcal_e);
    h_2D_all_leadjet4_triggere_emcal->Fill(highest_jet4_pt, highest_trigger_emcal_e);
    h_2D_all_leadjet5_triggere_emcal->Fill(highest_jet5_pt, highest_trigger_emcal_e);
    h_2D_all_leadjet2_triggere_ihcal->Fill(highest_jet2_pt, highest_trigger_ihcal_e);
    h_2D_all_leadjet3_triggere_ihcal->Fill(highest_jet3_pt, highest_trigger_ihcal_e);
    h_2D_all_leadjet4_triggere_ihcal->Fill(highest_jet4_pt, highest_trigger_ihcal_e);
    h_2D_all_leadjet5_triggere_ihcal->Fill(highest_jet5_pt, highest_trigger_ihcal_e);
    h_2D_all_leadjet2_triggere_ohcal->Fill(highest_jet2_pt, highest_trigger_ohcal_e);
    h_2D_all_leadjet3_triggere_ohcal->Fill(highest_jet3_pt, highest_trigger_ohcal_e);
    h_2D_all_leadjet4_triggere_ohcal->Fill(highest_jet4_pt, highest_trigger_ohcal_e);
    h_2D_all_leadjet5_triggere_ohcal->Fill(highest_jet5_pt, highest_trigger_ohcal_e);

    for (int nt = 0; nt < n_threshold; ++nt) {
      if (fired_check[nt] == 1) {
        fired_event[nt]++;
        h_2D_fired_leadjet2_triggeret[nt]->Fill(highest_jet2_pt, highest_trigger_et);
        h_2D_fired_leadjet3_triggeret[nt]->Fill(highest_jet3_pt, highest_trigger_et);
        h_2D_fired_leadjet4_triggeret[nt]->Fill(highest_jet4_pt, highest_trigger_et);
        h_2D_fired_leadjet5_triggeret[nt]->Fill(highest_jet5_pt, highest_trigger_et);
        h_2D_fired_leadjet2_triggeret_emcal[nt]->Fill(highest_jet2_pt, highest_trigger_emcal_et);
        h_2D_fired_leadjet3_triggeret_emcal[nt]->Fill(highest_jet3_pt, highest_trigger_emcal_et);
        h_2D_fired_leadjet4_triggeret_emcal[nt]->Fill(highest_jet4_pt, highest_trigger_emcal_et);
        h_2D_fired_leadjet5_triggeret_emcal[nt]->Fill(highest_jet5_pt, highest_trigger_emcal_et);
        h_2D_fired_leadjet2_triggeret_ihcal[nt]->Fill(highest_jet2_pt, highest_trigger_ihcal_et);
        h_2D_fired_leadjet3_triggeret_ihcal[nt]->Fill(highest_jet3_pt, highest_trigger_ihcal_et);
        h_2D_fired_leadjet4_triggeret_ihcal[nt]->Fill(highest_jet4_pt, highest_trigger_ihcal_et);
        h_2D_fired_leadjet5_triggeret_ihcal[nt]->Fill(highest_jet5_pt, highest_trigger_ihcal_et);
        h_2D_fired_leadjet2_triggeret_ohcal[nt]->Fill(highest_jet2_pt, highest_trigger_ohcal_et);
        h_2D_fired_leadjet3_triggeret_ohcal[nt]->Fill(highest_jet3_pt, highest_trigger_ohcal_et);
        h_2D_fired_leadjet4_triggeret_ohcal[nt]->Fill(highest_jet4_pt, highest_trigger_ohcal_et);
        h_2D_fired_leadjet5_triggeret_ohcal[nt]->Fill(highest_jet5_pt, highest_trigger_ohcal_et);
        h_2D_fired_leadjet2_triggere[nt]->Fill(highest_jet2_pt, highest_trigger_e);
        h_2D_fired_leadjet3_triggere[nt]->Fill(highest_jet3_pt, highest_trigger_e);
        h_2D_fired_leadjet4_triggere[nt]->Fill(highest_jet4_pt, highest_trigger_e);
        h_2D_fired_leadjet5_triggere[nt]->Fill(highest_jet5_pt, highest_trigger_e);
        h_2D_fired_leadjet2_triggere_emcal[nt]->Fill(highest_jet2_pt, highest_trigger_emcal_e);
        h_2D_fired_leadjet3_triggere_emcal[nt]->Fill(highest_jet3_pt, highest_trigger_emcal_e);
        h_2D_fired_leadjet4_triggere_emcal[nt]->Fill(highest_jet4_pt, highest_trigger_emcal_e);
        h_2D_fired_leadjet5_triggere_emcal[nt]->Fill(highest_jet5_pt, highest_trigger_emcal_e);
        h_2D_fired_leadjet2_triggere_ihcal[nt]->Fill(highest_jet2_pt, highest_trigger_ihcal_e);
        h_2D_fired_leadjet3_triggere_ihcal[nt]->Fill(highest_jet3_pt, highest_trigger_ihcal_e);
        h_2D_fired_leadjet4_triggere_ihcal[nt]->Fill(highest_jet4_pt, highest_trigger_ihcal_e);
        h_2D_fired_leadjet5_triggere_ihcal[nt]->Fill(highest_jet5_pt, highest_trigger_ihcal_e);
        h_2D_fired_leadjet2_triggere_ohcal[nt]->Fill(highest_jet2_pt, highest_trigger_ohcal_e);
        h_2D_fired_leadjet3_triggere_ohcal[nt]->Fill(highest_jet3_pt, highest_trigger_ohcal_e);
        h_2D_fired_leadjet4_triggere_ohcal[nt]->Fill(highest_jet4_pt, highest_trigger_ohcal_e);
        h_2D_fired_leadjet5_triggere_ohcal[nt]->Fill(highest_jet5_pt, highest_trigger_ohcal_e);
        for (int nj = 0; nj < n_recojet2; ++nj) {
          h_1D_fired_jet2pt[nt]->Fill(recojet2_pt->at(nj));
        }
        for (int nj = 0; nj < n_recojet3; ++nj) {
          h_1D_fired_jet3pt[nt]->Fill(recojet3_pt->at(nj));
        }
        for (int nj = 0; nj < n_recojet4; ++nj) {
          h_1D_fired_jet4pt[nt]->Fill(recojet4_pt->at(nj));
        }
        for (int nj = 0; nj < n_recojet5; ++nj) {
          h_1D_fired_jet5pt[nt]->Fill(recojet5_pt->at(nj));
        }
      }
    }
  } // event loop end.
  std::cout << "Event loop completed." << std::endl;
  g_1D_efficiency->SetName("g_1D_efficiency");
  g_1D_efficiency->SetMarkerStyle(20);
  for (int i = 0; i < n_threshold; ++i) {
    g_1D_efficiency->SetPoint(i, trigger_threshold[i], (float)fired_event[i] / (float)n_events);
    g_1D_efficiency->SetPointError(i, 0, TMath::Sqrt((float)fired_event[i]) / (float)fired_event[i] / (float)n_events);
  }
  for (int i = 0; i < n_threshold; ++i) {
    for (int j = 0; j < 4000; ++j) {
      if (h_1D_all_jet2pt->GetBinContent(j+1) == 0) {
        h_1D_jet2pt_efficiency[i]->SetBinContent(j+1, 0);
      } else {
        h_1D_jet2pt_efficiency[i]->SetBinContent(j+1, h_1D_fired_jet2pt[i]->GetBinContent(j+1) / (float)h_1D_all_jet2pt->GetBinContent(j+1));
      }
      if (h_1D_all_jet3pt->GetBinContent(j+1) == 0) {
        h_1D_jet3pt_efficiency[i]->SetBinContent(j+1, 0);
      } else {
        h_1D_jet3pt_efficiency[i]->SetBinContent(j+1, h_1D_fired_jet3pt[i]->GetBinContent(j+1) / (float)h_1D_all_jet3pt->GetBinContent(j+1));
      }
      if (h_1D_all_jet4pt->GetBinContent(j+1) == 0) {
        h_1D_jet4pt_efficiency[i]->SetBinContent(j+1, 0);
      } else {
        h_1D_jet4pt_efficiency[i]->SetBinContent(j+1, h_1D_fired_jet4pt[i]->GetBinContent(j+1) / (float)h_1D_all_jet4pt->GetBinContent(j+1));
      }
      if (h_1D_all_jet5pt->GetBinContent(j+1) == 0) {
        h_1D_jet5pt_efficiency[i]->SetBinContent(j+1, 0);
      } else {
        h_1D_jet5pt_efficiency[i]->SetBinContent(j+1, h_1D_fired_jet5pt[i]->GetBinContent(j+1) / (float)h_1D_all_jet5pt->GetBinContent(j+1));
      }
    }
  }

  // Write histograms
  std::cout << "Writing histograms..." << std::endl;
  output->cd();
  for (int i = 0; i < n_threshold; ++i) {
    h_2D_fired_rate[i]->Write();
    h_2D_fired_leadjet2_triggeret[i]->Write();
    h_2D_fired_leadjet3_triggeret[i]->Write();
    h_2D_fired_leadjet4_triggeret[i]->Write();
    h_2D_fired_leadjet5_triggeret[i]->Write();
    h_2D_fired_leadjet2_triggeret_emcal[i]->Write();
    h_2D_fired_leadjet3_triggeret_emcal[i]->Write();
    h_2D_fired_leadjet4_triggeret_emcal[i]->Write();
    h_2D_fired_leadjet5_triggeret_emcal[i]->Write();
    h_2D_fired_leadjet2_triggeret_ihcal[i]->Write();
    h_2D_fired_leadjet3_triggeret_ihcal[i]->Write();
    h_2D_fired_leadjet4_triggeret_ihcal[i]->Write();
    h_2D_fired_leadjet5_triggeret_ihcal[i]->Write();
    h_2D_fired_leadjet2_triggeret_ohcal[i]->Write();
    h_2D_fired_leadjet3_triggeret_ohcal[i]->Write();
    h_2D_fired_leadjet4_triggeret_ohcal[i]->Write();
    h_2D_fired_leadjet5_triggeret_ohcal[i]->Write();
    h_2D_fired_leadjet2_triggere[i]->Write();
    h_2D_fired_leadjet3_triggere[i]->Write();
    h_2D_fired_leadjet4_triggere[i]->Write();
    h_2D_fired_leadjet5_triggere[i]->Write();
    h_2D_fired_leadjet2_triggere_emcal[i]->Write();
    h_2D_fired_leadjet3_triggere_emcal[i]->Write();
    h_2D_fired_leadjet4_triggere_emcal[i]->Write();
    h_2D_fired_leadjet5_triggere_emcal[i]->Write();
    h_2D_fired_leadjet2_triggere_ihcal[i]->Write();
    h_2D_fired_leadjet3_triggere_ihcal[i]->Write();
    h_2D_fired_leadjet4_triggere_ihcal[i]->Write();
    h_2D_fired_leadjet5_triggere_ihcal[i]->Write();
    h_2D_fired_leadjet2_triggere_ohcal[i]->Write();
    h_2D_fired_leadjet3_triggere_ohcal[i]->Write();
    h_2D_fired_leadjet4_triggere_ohcal[i]->Write();
    h_2D_fired_leadjet5_triggere_ohcal[i]->Write();
    h_1D_fired_jet2pt[i]->Write();
    h_1D_fired_jet3pt[i]->Write();
    h_1D_fired_jet4pt[i]->Write();
    h_1D_fired_jet5pt[i]->Write();
    h_1D_jet2pt_efficiency[i]->Write();
    h_1D_jet3pt_efficiency[i]->Write();
    h_1D_jet4pt_efficiency[i]->Write();
    h_1D_jet5pt_efficiency[i]->Write();
  }
  h_2D_all_leadjet2_triggeret->Write();
  h_2D_all_leadjet3_triggeret->Write();
  h_2D_all_leadjet4_triggeret->Write();
  h_2D_all_leadjet5_triggeret->Write();
  h_2D_all_leadjet2_triggeret_emcal->Write();
  h_2D_all_leadjet3_triggeret_emcal->Write();
  h_2D_all_leadjet4_triggeret_emcal->Write();
  h_2D_all_leadjet5_triggeret_emcal->Write();
  h_2D_all_leadjet2_triggeret_ihcal->Write();
  h_2D_all_leadjet3_triggeret_ihcal->Write();
  h_2D_all_leadjet4_triggeret_ihcal->Write();
  h_2D_all_leadjet5_triggeret_ihcal->Write();
  h_2D_all_leadjet2_triggeret_ohcal->Write();
  h_2D_all_leadjet3_triggeret_ohcal->Write();
  h_2D_all_leadjet4_triggeret_ohcal->Write();
  h_2D_all_leadjet5_triggeret_ohcal->Write();
  h_2D_all_leadjet2_triggere->Write();
  h_2D_all_leadjet3_triggere->Write();
  h_2D_all_leadjet4_triggere->Write();
  h_2D_all_leadjet5_triggere->Write();
  h_2D_all_leadjet2_triggere_emcal->Write();
  h_2D_all_leadjet3_triggere_emcal->Write();
  h_2D_all_leadjet4_triggere_emcal->Write();
  h_2D_all_leadjet5_triggere_emcal->Write();
  h_2D_all_leadjet2_triggere_ihcal->Write();
  h_2D_all_leadjet3_triggere_ihcal->Write();
  h_2D_all_leadjet4_triggere_ihcal->Write();
  h_2D_all_leadjet5_triggere_ihcal->Write();
  h_2D_all_leadjet2_triggere_ohcal->Write();
  h_2D_all_leadjet3_triggere_ohcal->Write();
  h_2D_all_leadjet4_triggere_ohcal->Write();
  h_2D_all_leadjet5_triggere_ohcal->Write();
  h_1D_all_jet2pt->Write();
  h_1D_all_jet3pt->Write();
  h_1D_all_jet4pt->Write();
  h_1D_all_jet5pt->Write();
  h_1D_emcal_towerenergy->Write();
  h_1D_ihcal_towerenergy->Write();
  h_1D_ohcal_towerenergy->Write();
  g_1D_efficiency->Write();
  output->Close();
  std::cout << "Data analysis completed successfully." << std::endl;
}

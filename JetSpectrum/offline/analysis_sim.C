#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "TRandom.h"
#include "TRandom3.h"
#include "unfold_Def.h"

void get_leading_jet(int& leadingjet_index, std::vector<float>* jet_pt);
void get_leading_subleading_jet(int& leadingjet_index, int& subleadingjet_index, std::vector<float>* jet_et);
bool match_leading_subleading_jet(float leadingjet_phi, float subleadingjet_phi);
void get_jet_filter(std::vector<bool>& jet_filter, std::vector<float>* jet_e, std::vector<float>* jet_pt, std::vector<float>* jet_eta, float zvertex, float jet_radius);
void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& jet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi, std::vector<bool>* jet_background, TF1* f_corr, float jes_para, float jer_para);
void get_truthjet(std::vector<float>& goodtruthjet_pt, std::vector<float>& goodtruthjet_eta, std::vector<float>& goodtruthjet_phi, std::vector<bool>& truthjet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi);
void match_meas_truth(std::vector<float>& meas_eta, std::vector<float>& meas_phi, std::vector<float>& meas_matched, std::vector<float>& truth_eta, std::vector<float>& truth_phi, std::vector<float>& truth_matched, float jet_radius);
void fill_response_matrix(TH1D*& h_truth, TH1D*& h_meas, TH2D*& h_resp, TH1D*& h_fake, TH1D*& h_miss, std::vector<float>& meas_pt, std::vector<float>& meas_matched, std::vector<float>& truth_pt, std::vector<float>& truth_matched, float weight_scale);

TF1* f_jer = new TF1("f_jer", "sqrt( 0.095077098*0.095077098 + (0.63134847*0.63134847/x) + (2.1664610*2.1664610/(x*x)) )", 0, 100);
TRandom3 randGen(1234);

////////////////////////////////////////// Main Function //////////////////////////////////////////
void analysis_sim(std::string runtype, int nseg, int iseg)  {
  ////////// General Set up //////////
  double weight_scale = 1.0, truthjet_pt_min = 0, truthjet_pt_max = 3000;
  if (runtype == "MB") {
    weight_scale = MB_scale;
    truthjet_pt_min = 0;
    truthjet_pt_max = 14;
  } else if (runtype == "Jet10GeV") {
    weight_scale = Jet10GeV_scale;
    truthjet_pt_min = 14;
    truthjet_pt_max = 30;
  } else if (runtype == "Jet30GeV") {
    weight_scale = Jet30GeV_scale;
    truthjet_pt_min = 30;
    truthjet_pt_max = 3000;
  } else {
    std::cout << "Unknown runtype" << std::endl;
    return;
  }
 
  ////////// Files //////////
  TFile *f_out = new TFile(Form("output_sim/output_%s_%d_%d.root", runtype.c_str(), iseg, iseg+nseg), "RECREATE");
  TChain chain("ttree");
  for (int i = iseg; i < iseg + nseg; ++i) {
    chain.Add(Form("/sphenix/tg/tg01/jets/hanpuj/JES_MC_run21/%s/OutDir%d/output_sim.root", runtype.c_str(), i));
  }
  chain.SetBranchStatus("*", 0);

  float zvertex; chain.SetBranchStatus("z_vertex", 1); chain.SetBranchAddress("z_vertex", &zvertex);
  std::vector<float>* unsubjet_e = nullptr; chain.SetBranchStatus("unsubjet04_e", 1); chain.SetBranchAddress("unsubjet04_e", &unsubjet_e);
  std::vector<float>* unsubjet_pt = nullptr; chain.SetBranchStatus("unsubjet04_pt", 1); chain.SetBranchAddress("unsubjet04_pt", &unsubjet_pt);
  std::vector<float>* unsubjet_et = nullptr; chain.SetBranchStatus("unsubjet04_et", 1); chain.SetBranchAddress("unsubjet04_et", &unsubjet_et);
  std::vector<float>* unsubjet_eta = nullptr; chain.SetBranchStatus("unsubjet04_eta", 1); chain.SetBranchAddress("unsubjet04_eta", &unsubjet_eta);
  std::vector<float>* unsubjet_phi = nullptr; chain.SetBranchStatus("unsubjet04_phi", 1); chain.SetBranchAddress("unsubjet04_phi", &unsubjet_phi);
  std::vector<float>* unsubjet_emcal_calo_e = nullptr; chain.SetBranchStatus("unsubjet04_emcal_calo_e", 1); chain.SetBranchAddress("unsubjet04_emcal_calo_e", &unsubjet_emcal_calo_e);
  std::vector<float>* unsubjet_ihcal_calo_e = nullptr; chain.SetBranchStatus("unsubjet04_ihcal_calo_e", 1); chain.SetBranchAddress("unsubjet04_ihcal_calo_e", &unsubjet_ihcal_calo_e);
  std::vector<float>* unsubjet_ohcal_calo_e = nullptr; chain.SetBranchStatus("unsubjet04_ohcal_calo_e", 1); chain.SetBranchAddress("unsubjet04_ohcal_calo_e", &unsubjet_ohcal_calo_e);

  std::vector<float>* truthjet_e = nullptr; chain.SetBranchStatus("truthjet04_e", 1); chain.SetBranchAddress("truthjet04_e", &truthjet_e);
  std::vector<float>* truthjet_pt = nullptr; chain.SetBranchStatus("truthjet04_pt", 1); chain.SetBranchAddress("truthjet04_pt", &truthjet_pt);
  std::vector<float>* truthjet_eta = nullptr; chain.SetBranchStatus("truthjet04_eta", 1); chain.SetBranchAddress("truthjet04_eta", &truthjet_eta);
  std::vector<float>* truthjet_phi = nullptr; chain.SetBranchStatus("truthjet04_phi", 1); chain.SetBranchAddress("truthjet04_phi", &truthjet_phi);

  ////////// JES func //////////
  TFile *corrFile = new TFile("/sphenix/user/hanpuj/JES_MC_Calibration/offline/JES_Calib_Default.root", "READ");
  if (!corrFile) {
    std::cout << "Error: cannot open JES_Calib_Default.root" << std::endl;
    return;
  }
  TF1 *f_corr = (TF1*)corrFile->Get("JES_Calib_Default_Func");
  if (!f_corr) {
    std::cout << "Error: cannot open f_corr" << std::endl;
    return;
  }

  ////////// Histograms //////////
  TH1D* h_zvertex = new TH1D("h_zvertex", ";Z-vertex [cm]", 60, -30, 30);
  TH1D *h_recojet_pt_record = new TH1D("h_recojet_pt_record", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_record_dijet = new TH1D("h_recojet_pt_record_dijet", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_record_frac = new TH1D("h_recojet_pt_record_frac", ";p_{T} [GeV]", 1000, 0, 100);

  TH1D* h_truth_calib_dijet = new TH1D("h_truth_calib_dijet", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_dijet = new TH1D("h_measure_calib_dijet", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_dijet = new TH2D("h_respmatrix_calib_dijet", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_dijet = new TH1D("h_fake_calib_dijet", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_dijet = new TH1D("h_miss_calib_dijet", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D* h_truth_calib_dijet_jesdown = new TH1D("h_truth_calib_dijet_jesdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_dijet_jesdown = new TH1D("h_measure_calib_dijet_jesdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_dijet_jesdown = new TH2D("h_respmatrix_calib_dijet_jesdown", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_dijet_jesdown = new TH1D("h_fake_calib_dijet_jesdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_dijet_jesdown = new TH1D("h_miss_calib_dijet_jesdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D* h_truth_calib_dijet_jesup = new TH1D("h_truth_calib_dijet_jesup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_dijet_jesup = new TH1D("h_measure_calib_dijet_jesup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_dijet_jesup = new TH2D("h_respmatrix_calib_dijet_jesup", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_dijet_jesup = new TH1D("h_fake_calib_dijet_jesup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_dijet_jesup = new TH1D("h_miss_calib_dijet_jesup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D* h_truth_calib_dijet_jerdown = new TH1D("h_truth_calib_dijet_jerdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_dijet_jerdown = new TH1D("h_measure_calib_dijet_jerdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_dijet_jerdown = new TH2D("h_respmatrix_calib_dijet_jerdown", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_dijet_jerdown = new TH1D("h_fake_calib_dijet_jerdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_dijet_jerdown = new TH1D("h_miss_calib_dijet_jerdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D* h_truth_calib_dijet_jerup = new TH1D("h_truth_calib_dijet_jerup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_dijet_jerup = new TH1D("h_measure_calib_dijet_jerup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_dijet_jerup = new TH2D("h_respmatrix_calib_dijet_jerup", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_dijet_jerup = new TH1D("h_fake_calib_dijet_jerup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_dijet_jerup = new TH1D("h_miss_calib_dijet_jerup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D* h_truth_calib_frac = new TH1D("h_truth_calib_frac", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_frac = new TH1D("h_measure_calib_frac", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_frac = new TH2D("h_respmatrix_calib_frac", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_frac = new TH1D("h_fake_calib_frac", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_frac = new TH1D("h_miss_calib_frac", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D* h_truth_calib_frac_jesdown = new TH1D("h_truth_calib_frac_jesdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_frac_jesdown = new TH1D("h_measure_calib_frac_jesdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_frac_jesdown = new TH2D("h_respmatrix_calib_frac_jesdown", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_frac_jesdown = new TH1D("h_fake_calib_frac_jesdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_frac_jesdown = new TH1D("h_miss_calib_frac_jesdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D* h_truth_calib_frac_jesup = new TH1D("h_truth_calib_frac_jesup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_frac_jesup = new TH1D("h_measure_calib_frac_jesup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_frac_jesup = new TH2D("h_respmatrix_calib_frac_jesup", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_frac_jesup = new TH1D("h_fake_calib_frac_jesup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_frac_jesup = new TH1D("h_miss_calib_frac_jesup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D* h_truth_calib_frac_jerdown = new TH1D("h_truth_calib_frac_jerdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_frac_jerdown = new TH1D("h_measure_calib_frac_jerdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_frac_jerdown = new TH2D("h_respmatrix_calib_frac_jerdown", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_frac_jerdown = new TH1D("h_fake_calib_frac_jerdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_frac_jerdown = new TH1D("h_miss_calib_frac_jerdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D* h_truth_calib_frac_jerup = new TH1D("h_truth_calib_frac_jerup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D* h_measure_calib_frac_jerup = new TH1D("h_measure_calib_frac_jerup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D* h_respmatrix_calib_frac_jerup = new TH2D("h_respmatrix_calib_frac_jerup", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D* h_fake_calib_frac_jerup = new TH1D("h_fake_calib_frac_jerup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D* h_miss_calib_frac_jerup = new TH1D("h_miss_calib_frac_jerup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  //TH1D* h_truth_calib_frac_half1 = new TH1D("h_truth_calib_frac_half1", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  //TH1D* h_measure_calib_frac_half1 = new TH1D("h_measure_calib_frac_half1", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  //TH2D* h_respmatrix_calib_frac_half1 = new TH2D("h_respmatrix_calib_frac_half1", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  //TH1D* h_fake_calib_frac_half1 = new TH1D("h_fake_calib_frac_half1", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  //TH1D* h_miss_calib_frac_half1 = new TH1D("h_miss_calib_frac_half1", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  //TH1D* h_truth_calib_frac_half2 = new TH1D("h_truth_calib_frac_half2", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  //TH1D* h_measure_calib_frac_half2 = new TH1D("h_measure_calib_frac_half2", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  //TH2D* h_respmatrix_calib_frac_half2 = new TH2D("h_respmatrix_calib_frac_half2", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  //TH1D* h_fake_calib_frac_half2 = new TH1D("h_fake_calib_frac_half2", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  //TH1D* h_miss_calib_frac_half2 = new TH1D("h_miss_calib_frac_half2", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  ////////// Event Loop //////////
  std::cout << "Data analysis started." << std::endl;
  int n_events = chain.GetEntries();
  std::cout << "Total number of events: " << n_events << std::endl;
  // Event variables setup.
  std::vector<bool>* unsubjet_background_dijet = new std::vector<bool>;
  std::vector<bool>* unsubjet_background_frac = new std::vector<bool>;
  std::vector<bool> jet_filter, truthjet_filter;
  std::vector<float> goodtruthjet_pt, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched;
  std::vector<float> calibjet_pt_dijet, calibjet_eta_dijet, calibjet_phi_dijet, calibjet_matched_dijet;
  std::vector<float> calibjet_pt_dijet_jesdown, calibjet_eta_dijet_jesdown, calibjet_phi_dijet_jesdown, calibjet_matched_dijet_jesdown;
  std::vector<float> calibjet_pt_dijet_jesup, calibjet_eta_dijet_jesup, calibjet_phi_dijet_jesup, calibjet_matched_dijet_jesup;
  std::vector<float> calibjet_pt_dijet_jerdown, calibjet_eta_dijet_jerdown, calibjet_phi_dijet_jerdown, calibjet_matched_dijet_jerdown;
  std::vector<float> calibjet_pt_dijet_jerup, calibjet_eta_dijet_jerup, calibjet_phi_dijet_jerup, calibjet_matched_dijet_jerup;
  std::vector<float> calibjet_pt_frac, calibjet_eta_frac, calibjet_phi_frac, calibjet_matched_frac;
  std::vector<float> calibjet_pt_frac_jesdown, calibjet_eta_frac_jesdown, calibjet_phi_frac_jesdown, calibjet_matched_frac_jesdown;
  std::vector<float> calibjet_pt_frac_jesup, calibjet_eta_frac_jesup, calibjet_phi_frac_jesup, calibjet_matched_frac_jesup;
  std::vector<float> calibjet_pt_frac_jerdown, calibjet_eta_frac_jerdown, calibjet_phi_frac_jerdown, calibjet_matched_frac_jerdown;
  std::vector<float> calibjet_pt_frac_jerup, calibjet_eta_frac_jerup, calibjet_phi_frac_jerup, calibjet_matched_frac_jerup;
  for (int ie = 0; ie < n_events; ++ie) { // event loop start
    // Load event.
    if (ie % 1000 == 0) {
      std::cout << "Processing event " << ie << "..." << std::endl;
    }
    chain.GetEntry(ie);

    // Z-vertex cut.
    if (fabs(zvertex) > 30) continue;

    // Get truth leading jet
    int leadingtruthjet_index = -9999;
    get_leading_jet(leadingtruthjet_index, truthjet_pt);
    if (leadingtruthjet_index < 0) continue;
    float leadingtruthjet_pt = truthjet_pt->at(leadingtruthjet_index);
    if (leadingtruthjet_pt < truthjet_pt_min || leadingtruthjet_pt > truthjet_pt_max) continue;

    // Get reco leading jet and subleading jet. Do basic jet cuts.
    int leadingunsubjet_index = -1;
    int subleadingunsubjet_index = -1;
    get_leading_subleading_jet(leadingunsubjet_index, subleadingunsubjet_index, unsubjet_pt);
    if (leadingunsubjet_index < 0) continue;

    // Fill z-vertex histogram.
    h_zvertex->Fill(zvertex);

    // Beam background cut. Jet level recording.
    unsubjet_background_dijet->clear();
    unsubjet_background_frac->clear();
    // Dijet cut.
    bool match_dijet = false;
    if (subleadingunsubjet_index >= 0 && unsubjet_e->at(subleadingunsubjet_index) / (float) unsubjet_e->at(leadingunsubjet_index) > 0.3) {
      match_dijet = match_leading_subleading_jet(unsubjet_phi->at(leadingunsubjet_index), unsubjet_phi->at(subleadingunsubjet_index));
    }
    bool isbeambackground_dijet = true;
    if (match_dijet) isbeambackground_dijet = false;
    for (int ij = 0; ij < unsubjet_e->size(); ++ij) {
      if (isbeambackground_dijet) {
        unsubjet_background_dijet->push_back(true);
      } else {
        unsubjet_background_dijet->push_back(false);
      }
    }
    // Fraction cut.
    for (int ij = 0; ij < unsubjet_e->size(); ++ij) {
      double emfrac = unsubjet_emcal_calo_e->at(ij) / (double)(unsubjet_e->at(ij));
      double ihfrac = unsubjet_ihcal_calo_e->at(ij) / (double)(unsubjet_e->at(ij));
      double ohfrac = unsubjet_ohcal_calo_e->at(ij) / (double)(unsubjet_e->at(ij));
      if (emfrac < 0.1 || emfrac > 0.9 || ohfrac < 0.1 || ohfrac > 0.9 || ihfrac > 0.9) {
        unsubjet_background_frac->push_back(true);
      } else {
        unsubjet_background_frac->push_back(false);
      }
    }

    // Do jet filter
    get_jet_filter(jet_filter, unsubjet_e, unsubjet_pt, unsubjet_eta, zvertex, jet_radius);
    get_jet_filter(truthjet_filter, truthjet_e, truthjet_pt, truthjet_eta, zvertex, jet_radius);
    for (int ij = 0; ij < unsubjet_pt->size(); ++ij) {
      if (jet_filter[ij]) continue;
      h_recojet_pt_record->Fill(unsubjet_pt->at(ij), weight_scale);
      if (!unsubjet_background_dijet->at(ij)) h_recojet_pt_record_dijet->Fill(unsubjet_pt->at(ij), weight_scale);
      if (!unsubjet_background_frac->at(ij)) h_recojet_pt_record_frac->Fill(unsubjet_pt->at(ij), weight_scale);
    }

    // Get truth jet and calib jet.
    get_truthjet(goodtruthjet_pt, goodtruthjet_eta, goodtruthjet_phi, truthjet_filter, truthjet_pt, truthjet_eta, truthjet_phi);
    get_calibjet(calibjet_pt_dijet, calibjet_eta_dijet, calibjet_phi_dijet, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_dijet, f_corr, 1, 0.1);
    get_calibjet(calibjet_pt_dijet_jesdown, calibjet_eta_dijet_jesdown, calibjet_phi_dijet_jesdown, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_dijet, f_corr, 0.94, 0.1);
    get_calibjet(calibjet_pt_dijet_jesup, calibjet_eta_dijet_jesup, calibjet_phi_dijet_jesup, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_dijet, f_corr, 1.06, 0.1);
    get_calibjet(calibjet_pt_dijet_jerdown, calibjet_eta_dijet_jerdown, calibjet_phi_dijet_jerdown, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_dijet, f_corr, 1, 0.05);
    get_calibjet(calibjet_pt_dijet_jerup, calibjet_eta_dijet_jerup, calibjet_phi_dijet_jerup, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_dijet, f_corr, 1, 0.15);
    get_calibjet(calibjet_pt_frac, calibjet_eta_frac, calibjet_phi_frac, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_frac, f_corr, 1, 0.1);
    get_calibjet(calibjet_pt_frac_jesdown, calibjet_eta_frac_jesdown, calibjet_phi_frac_jesdown, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_frac, f_corr, 0.94, 0.1);
    get_calibjet(calibjet_pt_frac_jesup, calibjet_eta_frac_jesup, calibjet_phi_frac_jesup, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_frac, f_corr, 1.06, 0.1);
    get_calibjet(calibjet_pt_frac_jerdown, calibjet_eta_frac_jerdown, calibjet_phi_frac_jerdown, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_frac, f_corr, 1, 0.05);
    get_calibjet(calibjet_pt_frac_jerup, calibjet_eta_frac_jerup, calibjet_phi_frac_jerup, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_frac, f_corr, 1, 0.15);

    // Match truth jet and calib jet.
    match_meas_truth(calibjet_eta_dijet, calibjet_phi_dijet, calibjet_matched_dijet, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_dijet, h_measure_calib_dijet, h_respmatrix_calib_dijet, h_fake_calib_dijet, h_miss_calib_dijet, calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    match_meas_truth(calibjet_eta_dijet_jesdown, calibjet_phi_dijet_jesdown, calibjet_matched_dijet_jesdown, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_dijet_jesdown, h_measure_calib_dijet_jesdown, h_respmatrix_calib_dijet_jesdown, h_fake_calib_dijet_jesdown, h_miss_calib_dijet_jesdown, calibjet_pt_dijet_jesdown, calibjet_matched_dijet_jesdown, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    match_meas_truth(calibjet_eta_dijet_jesup, calibjet_phi_dijet_jesup, calibjet_matched_dijet_jesup, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_dijet_jesup, h_measure_calib_dijet_jesup, h_respmatrix_calib_dijet_jesup, h_fake_calib_dijet_jesup, h_miss_calib_dijet_jesup, calibjet_pt_dijet_jesup, calibjet_matched_dijet_jesup, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    match_meas_truth(calibjet_eta_dijet_jerdown, calibjet_phi_dijet_jerdown, calibjet_matched_dijet_jerdown, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_dijet_jerdown, h_measure_calib_dijet_jerdown, h_respmatrix_calib_dijet_jerdown, h_fake_calib_dijet_jerdown, h_miss_calib_dijet_jerdown, calibjet_pt_dijet_jerdown, calibjet_matched_dijet_jerdown, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    match_meas_truth(calibjet_eta_dijet_jerup, calibjet_phi_dijet_jerup, calibjet_matched_dijet_jerup, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_dijet_jerup, h_measure_calib_dijet_jerup, h_respmatrix_calib_dijet_jerup, h_fake_calib_dijet_jerup, h_miss_calib_dijet_jerup, calibjet_pt_dijet_jerup, calibjet_matched_dijet_jerup, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    match_meas_truth(calibjet_eta_frac, calibjet_phi_frac, calibjet_matched_frac, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_frac, h_measure_calib_frac, h_respmatrix_calib_frac, h_fake_calib_frac, h_miss_calib_frac, calibjet_pt_frac, calibjet_matched_frac, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    //if(ie%2 == 0){
    //  fill_response_matrix(h_truth_calib_frac_half1, h_measure_calib_frac_half1, h_respmatrix_calib_frac_half1, h_fake_calib_frac_half1, h_miss_calib_frac_half1, calibjet_pt_frac, calibjet_matched_frac, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    //} else {
    //  fill_response_matrix(h_truth_calib_frac_half2, h_measure_calib_frac_half2, h_respmatrix_calib_frac_half2, h_fake_calib_frac_half2, h_miss_calib_frac_half2, calibjet_pt_frac, calibjet_matched_frac, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    //}
    match_meas_truth(calibjet_eta_frac_jesdown, calibjet_phi_frac_jesdown, calibjet_matched_frac_jesdown, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_frac_jesdown, h_measure_calib_frac_jesdown, h_respmatrix_calib_frac_jesdown, h_fake_calib_frac_jesdown, h_miss_calib_frac_jesdown, calibjet_pt_frac_jesdown, calibjet_matched_frac_jesdown, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    match_meas_truth(calibjet_eta_frac_jesup, calibjet_phi_frac_jesup, calibjet_matched_frac_jesup, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_frac_jesup, h_measure_calib_frac_jesup, h_respmatrix_calib_frac_jesup, h_fake_calib_frac_jesup, h_miss_calib_frac_jesup, calibjet_pt_frac_jesup, calibjet_matched_frac_jesup, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    match_meas_truth(calibjet_eta_frac_jerdown, calibjet_phi_frac_jerdown, calibjet_matched_frac_jerdown, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_frac_jerdown, h_measure_calib_frac_jerdown, h_respmatrix_calib_frac_jerdown, h_fake_calib_frac_jerdown, h_miss_calib_frac_jerdown, calibjet_pt_frac_jerdown, calibjet_matched_frac_jerdown, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
    match_meas_truth(calibjet_eta_frac_jerup, calibjet_phi_frac_jerup, calibjet_matched_frac_jerup, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_calib_frac_jerup, h_measure_calib_frac_jerup, h_respmatrix_calib_frac_jerup, h_fake_calib_frac_jerup, h_miss_calib_frac_jerup, calibjet_pt_frac_jerup, calibjet_matched_frac_jerup, goodtruthjet_pt, goodtruthjet_matched, weight_scale);
  } // event loop end

  // Write histograms.
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();

  h_zvertex->Write();
  h_recojet_pt_record->Write();
  h_recojet_pt_record_dijet->Write();
  h_recojet_pt_record_frac->Write();

  h_truth_calib_dijet->Write();
  h_measure_calib_dijet->Write();
  h_respmatrix_calib_dijet->Write();
  h_fake_calib_dijet->Write();
  h_miss_calib_dijet->Write();

  h_truth_calib_dijet_jesdown->Write();
  h_measure_calib_dijet_jesdown->Write();
  h_respmatrix_calib_dijet_jesdown->Write();
  h_fake_calib_dijet_jesdown->Write();
  h_miss_calib_dijet_jesdown->Write();

  h_truth_calib_dijet_jesup->Write();
  h_measure_calib_dijet_jesup->Write();
  h_respmatrix_calib_dijet_jesup->Write();
  h_fake_calib_dijet_jesup->Write();
  h_miss_calib_dijet_jesup->Write();

  h_truth_calib_dijet_jerdown->Write();
  h_measure_calib_dijet_jerdown->Write();
  h_respmatrix_calib_dijet_jerdown->Write();
  h_fake_calib_dijet_jerdown->Write();
  h_miss_calib_dijet_jerdown->Write();

  h_truth_calib_dijet_jerup->Write();
  h_measure_calib_dijet_jerup->Write();
  h_respmatrix_calib_dijet_jerup->Write();
  h_fake_calib_dijet_jerup->Write();
  h_miss_calib_dijet_jerup->Write();

  h_truth_calib_frac->Write();
  h_measure_calib_frac->Write();
  h_respmatrix_calib_frac->Write();
  h_fake_calib_frac->Write();
  h_miss_calib_frac->Write();

  h_truth_calib_frac_jesdown->Write();
  h_measure_calib_frac_jesdown->Write();
  h_respmatrix_calib_frac_jesdown->Write();
  h_fake_calib_frac_jesdown->Write();
  h_miss_calib_frac_jesdown->Write();

  h_truth_calib_frac_jesup->Write();
  h_measure_calib_frac_jesup->Write();
  h_respmatrix_calib_frac_jesup->Write();
  h_fake_calib_frac_jesup->Write();
  h_miss_calib_frac_jesup->Write();

  h_truth_calib_frac_jerdown->Write();
  h_measure_calib_frac_jerdown->Write();
  h_respmatrix_calib_frac_jerdown->Write();
  h_fake_calib_frac_jerdown->Write();
  h_miss_calib_frac_jerdown->Write();

  h_truth_calib_frac_jerup->Write();
  h_measure_calib_frac_jerup->Write();
  h_respmatrix_calib_frac_jerup->Write();
  h_fake_calib_frac_jerup->Write();
  h_miss_calib_frac_jerup->Write();

  //h_truth_calib_frac_half1->Write();
  //h_measure_calib_frac_half1->Write();
  //h_respmatrix_calib_frac_half1->Write();
  //h_fake_calib_frac_half1->Write();
  //h_miss_calib_frac_half1->Write();

  //h_truth_calib_frac_half2->Write();
  //h_measure_calib_frac_half2->Write();
  //h_respmatrix_calib_frac_half2->Write();
  //h_fake_calib_frac_half2->Write();
  //h_miss_calib_frac_half2->Write();

  f_out->Close();
  std::cout << "All done!" << std::endl;
}

////////////////////////////////////////// Helper Functions //////////////////////////////////////////
float get_deta(float eta1, float eta2) {
  return eta1 - eta2;
}

float get_dphi(float phi1, float phi2) {
  float dphi1 = phi1 - phi2;
  float dphi2 = phi1 - phi2 + 2*TMath::Pi();
  float dphi3 = phi1 - phi2 - 2*TMath::Pi();
  if (fabs(dphi1) > fabs(dphi2)) {
    dphi1 = dphi2;
  }
  if (fabs(dphi1) > fabs(dphi3)) {
    dphi1 = dphi3;
  }
  return dphi1;
}

float get_dR(float eta1, float phi1, float eta2, float phi2) {
  float deta = get_deta(eta1, eta2);
  float dphi = get_dphi(phi1, phi2);
  return sqrt(deta*deta + dphi*dphi);
}

float get_emcal_mineta_zcorrected(float zvertex) {
  float minz_EM = -130.23;
  float radius_EM = 93.5;
  float z = minz_EM - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_EM);
  return eta_zcorrected;
}

float get_emcal_maxeta_zcorrected(float zvertex) {
  float maxz_EM = 130.23;
  float radius_EM = 93.5;
  float z = maxz_EM - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_EM);
  return eta_zcorrected;
}

float get_ihcal_mineta_zcorrected(float zvertex) {
  float minz_IH = -170.299;
  float radius_IH = 127.503;
  float z = minz_IH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_IH);
  return eta_zcorrected;
}

float get_ihcal_maxeta_zcorrected(float zvertex) {
  float maxz_IH = 170.299;
  float radius_IH = 127.503;
  float z = maxz_IH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_IH);
  return eta_zcorrected;
}

float get_ohcal_mineta_zcorrected(float zvertex) {
  float minz_OH = -301.683;
  float radius_OH = 225.87;
  float z = minz_OH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_OH);
  return eta_zcorrected;
}

float get_ohcal_maxeta_zcorrected(float zvertex) {
  float maxz_OH = 301.683;
  float radius_OH = 225.87;
  float z = maxz_OH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_OH);
  return eta_zcorrected;
}

bool check_bad_jet_eta(float jet_eta, float zertex, float jet_radius) {
  float emcal_mineta = get_emcal_mineta_zcorrected(zertex);
  float emcal_maxeta = get_emcal_maxeta_zcorrected(zertex);
  float ihcal_mineta = get_ihcal_mineta_zcorrected(zertex);
  float ihcal_maxeta = get_ihcal_maxeta_zcorrected(zertex);
  float ohcal_mineta = get_ohcal_mineta_zcorrected(zertex);
  float ohcal_maxeta = get_ohcal_maxeta_zcorrected(zertex);
  float minlimit = emcal_mineta;
  if (ihcal_mineta > minlimit) minlimit = ihcal_mineta;
  if (ohcal_mineta > minlimit) minlimit = ohcal_mineta;
  float maxlimit = emcal_maxeta;
  if (ihcal_maxeta < maxlimit) maxlimit = ihcal_maxeta;
  if (ohcal_maxeta < maxlimit) maxlimit = ohcal_maxeta;
  minlimit += jet_radius;
  maxlimit -= jet_radius;
  return jet_eta < minlimit || jet_eta > maxlimit;
}

////////////////////////////////////////// Functions //////////////////////////////////////////
void get_leading_jet(int& leadingjet_index, std::vector<float>* jet_pt) {
  leadingjet_index = -1;
  float leadingjet_pt = -9999;
  for (int ij = 0; ij < jet_pt->size(); ++ij) {
    float jetpt = jet_pt->at(ij);
    if (jetpt > leadingjet_pt) {
      leadingjet_pt = jetpt;
      leadingjet_index = ij;
    }
  }
}

void get_leading_subleading_jet(int& leadingjet_index, int& subleadingjet_index, std::vector<float>* jet_et) {
  leadingjet_index = -1;
  subleadingjet_index = -1;
  float leadingjet_et = -9999;
  float subleadingjet_et = -9999;
  for (int ij = 0; ij < jet_et->size(); ++ij) {
    float jetet = jet_et->at(ij);
    if (jetet > leadingjet_et) {
      subleadingjet_et = leadingjet_et;
      subleadingjet_index = leadingjet_index;
      leadingjet_et = jetet;
      leadingjet_index = ij;
    } else if (jetet > subleadingjet_et) {
      subleadingjet_et = jetet;
      subleadingjet_index = ij;
    }
  }
}

bool match_leading_subleading_jet(float leadingjet_phi, float subleadingjet_phi) {
  float dijet_min_phi = 3*TMath::Pi()/4.;
  float dphi = get_dphi(leadingjet_phi, subleadingjet_phi);
  return dphi > dijet_min_phi;
}

void get_jet_filter(std::vector<bool>& jet_filter, std::vector<float>* jet_e, std::vector<float>* jet_pt, std::vector<float>* jet_eta, float zvertex, float jet_radius) {
  jet_filter.clear();
  int njet = jet_e->size();
  for (int ij = 0; ij < njet; ++ij) {
    jet_filter.push_back(jet_e->at(ij) < 0 || check_bad_jet_eta(jet_eta->at(ij), zvertex, jet_radius));
  }
}

void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& jet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi, std::vector<bool>* jet_background, TF1* f_corr, float jes_para, float jer_para) {
  calibjet_pt.clear();
  calibjet_eta.clear();
  calibjet_phi.clear();
  for (int ij = 0; ij < jet_filter.size(); ++ij) {
    if (jet_filter.at(ij) || jet_background->at(ij)) continue;
    double calib_pt = f_corr->Eval(jet_pt->at(ij)) * (1 + randGen.Gaus(0.0, jer_para)) * jes_para;
    if (calib_pt < calibptbins[0] || calib_pt > calibptbins[calibnpt]) continue;
    calibjet_pt.push_back(calib_pt);
    calibjet_eta.push_back(jet_eta->at(ij));
    calibjet_phi.push_back(jet_phi->at(ij));
  }
}

void get_truthjet(std::vector<float>& goodtruthjet_pt, std::vector<float>& goodtruthjet_eta, std::vector<float>& goodtruthjet_phi, std::vector<bool>& truthjet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi) {
  goodtruthjet_pt.clear();
  goodtruthjet_eta.clear();
  goodtruthjet_phi.clear();
  for (int ij = 0; ij < truthjet_filter.size(); ++ij) {
    if (truthjet_filter.at(ij)) continue;
    if (jet_pt->at(ij) < truthptbins[0] || jet_pt->at(ij) > truthptbins[truthnpt]) continue;
    goodtruthjet_pt.push_back(jet_pt->at(ij));
    goodtruthjet_eta.push_back(jet_eta->at(ij));
    goodtruthjet_phi.push_back(jet_phi->at(ij));
  }
}

void match_meas_truth(std::vector<float>& meas_eta, std::vector<float>& meas_phi, std::vector<float>& meas_matched, std::vector<float>& truth_eta, std::vector<float>& truth_phi, std::vector<float>& truth_matched, float jet_radius) {
  meas_matched.assign(meas_eta.size(), -1);
  truth_matched.assign(truth_eta.size(), -1);
  float max_match_dR = jet_radius * 0.75;
  for (int im = 0; im < meas_eta.size(); ++im) {
    float min_dR = 100;
    int match_index = -9999;
    for (int it = 0; it < truth_eta.size(); ++it) {
      float dR = get_dR(meas_eta[im], meas_phi[im], truth_eta[it], truth_phi[it]);
      if (dR < min_dR) {
        match_index = it;
        min_dR = dR;
      }
    }
    if (min_dR < max_match_dR) {
      if (truth_matched[match_index] == -1) {
        meas_matched[im] = match_index;
        truth_matched[match_index] = im;
      } else {
        float dR1 = get_dR(meas_eta[truth_matched[match_index]], meas_phi[truth_matched[match_index]], truth_eta[match_index], truth_phi[match_index]);
        if (min_dR < dR1) {
          meas_matched[truth_matched[match_index]] = -1;
          meas_matched[im] = match_index;
          truth_matched[match_index] = im;
        }
      }
    }
  }
}

void fill_response_matrix(TH1D*& h_truth, TH1D*& h_meas, TH2D*& h_resp, TH1D*& h_fake, TH1D*& h_miss, std::vector<float>& meas_pt, std::vector<float>& meas_matched, std::vector<float>& truth_pt, std::vector<float>& truth_matched, float weight_scale) {
  for (int im = 0; im < meas_pt.size(); ++im) {
    h_meas->Fill(meas_pt[im], weight_scale);
    if (meas_matched[im] < 0) {
      h_fake->Fill(meas_pt[im], weight_scale);
    } else {
      h_resp->Fill(meas_pt[im], truth_pt[meas_matched[im]], weight_scale);
    }
  }
  for (int it = 0; it < truth_pt.size(); ++it) {
    h_truth->Fill(truth_pt[it], weight_scale);
    if (truth_matched[it] < 0) {
      h_miss->Fill(truth_pt[it], weight_scale);
    }
  }
}

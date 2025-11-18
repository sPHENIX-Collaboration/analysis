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

void get_leading_subleading_jet(int& leadingjet_index, int& subleadingjet_index, std::vector<float>* jet_et);
bool match_leading_subleading_jet(float leadingjet_phi, float subleadingjet_phi);
void get_jet_filter(std::vector<bool>& jet_filter, std::vector<float>* jet_e, std::vector<float>* jet_pt, std::vector<float>* jet_eta, float zvertex, float jet_radius);
int get_zvertex_index(float zvertex);
void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& jet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi, const std::vector<std::vector<TF1*>>& f_corr, int zvertex_index, double jet_radius, float jes_para, float jer_para, bool jet_background);
void get_truthjet(std::vector<float>& goodtruthjet_pt, std::vector<float>& goodtruthjet_eta, std::vector<float>& goodtruthjet_phi, std::vector<bool>& truthjet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi);
void match_meas_truth(std::vector<float>& meas_eta, std::vector<float>& meas_phi, std::vector<float>& meas_matched, std::vector<float>& truth_eta, std::vector<float>& truth_phi, std::vector<float>& truth_matched, float jet_radius);
void fill_response_matrix(TH1D*& h_truth, TH1D*& h_meas, TH2D*& h_resp, TH1D*& h_fake, TH1D*& h_miss, double scale, std::vector<float>& meas_pt, std::vector<float>& meas_matched, std::vector<float>& truth_pt, std::vector<float>& truth_matched);
TRandom3 randGen(1234);

////////////////////////////////////////// Main Function //////////////////////////////////////////
void analysis_sim(std::string runtype, int nseg, int iseg, double jet_radius) {
  ////////// General Set up //////////
  int jet_radius_index = (int)(10 * jet_radius);
  TFile *f_out = new TFile(Form("output_sim/output_r0%d_%s_%d_%d.root", jet_radius_index, runtype.c_str(), iseg, iseg+nseg), "RECREATE");

  double truthjet_pt_min = 0, truthjet_pt_max = 3000;
  if (runtype == "MB") {
    if (jet_radius == 0.2) {truthjet_pt_min = 0; truthjet_pt_max = 5;}
    else if (jet_radius == 0.3) {truthjet_pt_min = 0; truthjet_pt_max = 6;}
    else if (jet_radius == 0.4) {truthjet_pt_min = 0; truthjet_pt_max = 7;}
    else if (jet_radius == 0.5) {truthjet_pt_min = 0; truthjet_pt_max = 10;}
    else if (jet_radius == 0.6) {truthjet_pt_min = 0; truthjet_pt_max = 11;}
  } else if (runtype == "Jet5GeV") {
    if (jet_radius == 0.2) {truthjet_pt_min = 5; truthjet_pt_max = 12;}
    else if (jet_radius == 0.3) {truthjet_pt_min = 6; truthjet_pt_max = 13;}
    else if (jet_radius == 0.4) {truthjet_pt_min = 7; truthjet_pt_max = 14;}
    else if (jet_radius == 0.5) {truthjet_pt_min = 10; truthjet_pt_max = 15;}
    else if (jet_radius == 0.6) {truthjet_pt_min = 11; truthjet_pt_max = 17;}
  } else if (runtype == "Jet10GeV") {
    if (jet_radius == 0.2) {truthjet_pt_min = 12; truthjet_pt_max = 15;}
    else if (jet_radius == 0.3) {truthjet_pt_min = 13; truthjet_pt_max = 16;}
    else if (jet_radius == 0.4) {truthjet_pt_min = 14; truthjet_pt_max = 17;}
    else if (jet_radius == 0.5) {truthjet_pt_min = 15; truthjet_pt_max = 24;}
    else if (jet_radius == 0.6) {truthjet_pt_min = 17; truthjet_pt_max = 26;}
  } else if (runtype == "Jet15GeV") {
    if (jet_radius == 0.2) {truthjet_pt_min = 15; truthjet_pt_max = 20;}
    else if (jet_radius == 0.3) {truthjet_pt_min = 16; truthjet_pt_max = 21;}
    else if (jet_radius == 0.4) {truthjet_pt_min = 17; truthjet_pt_max = 22;}
    else if (jet_radius == 0.5) {truthjet_pt_min = 24; truthjet_pt_max = 30;}
    else if (jet_radius == 0.6) {truthjet_pt_min = 26; truthjet_pt_max = 35;}
  } else if (runtype == "Jet20GeV") {
    if (jet_radius == 0.2) {truthjet_pt_min = 20; truthjet_pt_max = 31;}
    else if (jet_radius == 0.3) {truthjet_pt_min = 21; truthjet_pt_max = 33;}
    else if (jet_radius == 0.4) {truthjet_pt_min = 22; truthjet_pt_max = 35;}
    else if (jet_radius == 0.5) {truthjet_pt_min = 30; truthjet_pt_max = 40;}
    else if (jet_radius == 0.6) {truthjet_pt_min = 35; truthjet_pt_max = 45;}
  } else if (runtype == "Jet30GeV") {
    if (jet_radius == 0.2) {truthjet_pt_min = 31; truthjet_pt_max = 50;}
    else if (jet_radius == 0.3) {truthjet_pt_min = 33; truthjet_pt_max = 51;}
    else if (jet_radius == 0.4) {truthjet_pt_min = 35; truthjet_pt_max = 52;}
    else if (jet_radius == 0.5) {truthjet_pt_min = 40; truthjet_pt_max = 60;}
    else if (jet_radius == 0.6) {truthjet_pt_min = 45; truthjet_pt_max = 63;}
  } else if (runtype == "Jet50GeV") {
    if (jet_radius == 0.2) {truthjet_pt_min = 50; truthjet_pt_max = 70;}
    else if (jet_radius == 0.3) {truthjet_pt_min = 51; truthjet_pt_max = 70;}
    else if (jet_radius == 0.4) {truthjet_pt_min = 52; truthjet_pt_max = 71;}
    else if (jet_radius == 0.5) {truthjet_pt_min = 60; truthjet_pt_max = 75;}
    else if (jet_radius == 0.6) {truthjet_pt_min = 63; truthjet_pt_max = 79;}
  } else if (runtype == "Jet70GeV") {
    if (jet_radius == 0.2) {truthjet_pt_min = 70; truthjet_pt_max = 3000;}
    else if (jet_radius == 0.3) {truthjet_pt_min = 70; truthjet_pt_max = 3000;}
    else if (jet_radius == 0.4) {truthjet_pt_min = 71; truthjet_pt_max = 3000;}
    else if (jet_radius == 0.5) {truthjet_pt_min = 75; truthjet_pt_max = 3000;}
    else if (jet_radius == 0.6) {truthjet_pt_min = 79; truthjet_pt_max = 3000;}
  }
 
  ////////// Files //////////
  TChain chain("ttree");
  for (int i = iseg; i < iseg + nseg; ++i) {
    chain.Add(Form("/sphenix/tg/tg01/jets/hanpuj/JES_MC_run28/%s/OutDir%d/output_sim.root", runtype.c_str(), i));
  }
  chain.SetBranchStatus("*", 0);
  // Read necessary branches for analysis
  float zvertex; chain.SetBranchStatus("z_vertex", 1); chain.SetBranchAddress("z_vertex", &zvertex);
  std::vector<float>* unsubjet_e = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_e", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_e", jet_radius_index), &unsubjet_e);
  std::vector<float>* unsubjet_pt = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_pt", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_pt", jet_radius_index), &unsubjet_pt);
  std::vector<float>* unsubjet_eta = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_eta", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_eta", jet_radius_index), &unsubjet_eta);
  std::vector<float>* unsubjet_phi = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_phi", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_phi", jet_radius_index), &unsubjet_phi);
  std::vector<float>* truthjet_e = nullptr; chain.SetBranchStatus(Form("truthjet0%d_e", jet_radius_index), 1); chain.SetBranchAddress(Form("truthjet0%d_e", jet_radius_index), &truthjet_e);
  std::vector<float>* truthjet_pt = nullptr; chain.SetBranchStatus(Form("truthjet0%d_pt", jet_radius_index), 1); chain.SetBranchAddress(Form("truthjet0%d_pt", jet_radius_index), &truthjet_pt);
  std::vector<float>* truthjet_eta = nullptr; chain.SetBranchStatus(Form("truthjet0%d_eta", jet_radius_index), 1); chain.SetBranchAddress(Form("truthjet0%d_eta", jet_radius_index), &truthjet_eta);
  std::vector<float>* truthjet_phi = nullptr; chain.SetBranchStatus(Form("truthjet0%d_phi", jet_radius_index), 1); chain.SetBranchAddress(Form("truthjet0%d_phi", jet_radius_index), &truthjet_phi);
  // Read R=0.4 jet branches for event selection.
  std::vector<float>* unsubjet04_e_reader = nullptr, *unsubjet04_pt_reader = nullptr, *unsubjet04_eta_reader = nullptr, *unsubjet04_phi_reader = nullptr;
  if (jet_radius_index != 4) {
    chain.SetBranchStatus("unsubjet04_e", 1); chain.SetBranchAddress("unsubjet04_e", &unsubjet04_e_reader);
    chain.SetBranchStatus("unsubjet04_pt", 1); chain.SetBranchAddress("unsubjet04_pt", &unsubjet04_pt_reader);
    chain.SetBranchStatus("unsubjet04_eta", 1); chain.SetBranchAddress("unsubjet04_eta", &unsubjet04_eta_reader);
    chain.SetBranchStatus("unsubjet04_phi", 1); chain.SetBranchAddress("unsubjet04_phi", &unsubjet04_phi_reader);
  }
  std::vector<float>* &unsubjet04_e = (jet_radius_index != 4) ? unsubjet04_e_reader : unsubjet_e;
  std::vector<float>* &unsubjet04_pt = (jet_radius_index != 4) ? unsubjet04_pt_reader : unsubjet_pt;
  std::vector<float>* &unsubjet04_eta = (jet_radius_index != 4) ? unsubjet04_eta_reader : unsubjet_eta;
  std::vector<float>* &unsubjet04_phi = (jet_radius_index != 4) ? unsubjet04_phi_reader : unsubjet_phi;

  /////////////// JES func ///////////////
  TFile *corrFile = new TFile("input_jescalib.root", "READ");
  if (!corrFile || corrFile->IsZombie()) {
    std::cout << "Error: cannot open JES_Calib_Default.root" << std::endl;
    return;
  }
  std::vector<std::vector<TF1 *>> f_corr;
  int nZvrtxBins = 5, nEtaBins = 4;
  f_corr.resize(nZvrtxBins);
  for (auto &row : f_corr) {
    row.resize(nEtaBins);
  }
  for (int iz = 0; iz < nZvrtxBins; ++iz) {
    if (iz <= 2 && jet_radius_index <= 4) {
      for (int ieta = 0; ieta < nEtaBins; ++ieta) {
        f_corr[iz][ieta] = (TF1*)corrFile->Get(Form("JES_Calib_Func_R0%d_Z%d_Eta%d", jet_radius_index, iz, ieta));
        if (!f_corr[iz][ieta]) {
          std::cout << "Error: cannot open f_corr for zbin " << iz << " and etabin " << ieta << std::endl;
          return;
        }
      }
    } else if (iz <= 2 && jet_radius_index > 4) {
      for (int ieta = 0; ieta < nEtaBins; ++ieta) {
        f_corr[iz][ieta] = (TF1*)corrFile->Get(Form("JES_Calib_Func_R0%d_Default", jet_radius_index));
        if (!f_corr[iz][ieta]) {
          std::cout << "Error: cannot open f_corr for radius " << jet_radius_index << ", zbin " << iz << " and etabin " << ieta << std::endl;
          return;
        }
      }
    } else if (iz > 2) {
      f_corr[iz][0] = (TF1*)corrFile->Get(Form("JES_Calib_Func_R0%d_Z%d", jet_radius_index, iz));
      if (!f_corr[iz][0]) {
        std::cout << "Error: cannot open f_corr for radius " << jet_radius_index << " and zbin " << iz << std::endl;
        return;
      }
    } else {
      std::cout << "Error: invalid z-vertex bin: iz = " << iz << " with jet radius = " << jet_radius_index << std::endl;
      return;
    }
  }

  /////////////// Z-vertex Reweighting ///////////////
  TFile *f_zvertexreweight = new TFile("input_zvertexreweight.root", "READ");
  if (!f_zvertexreweight || f_zvertexreweight->IsZombie()) {
    std::cout << "Error: cannot open input_zvertexreweight.root" << std::endl;
    return;
  }
  TH1D *h_zvertex_reweight = (TH1D*)f_zvertexreweight->Get("h_zvertex_reweight");
  if (!h_zvertex_reweight) {
    std::cout << "Error: cannot open h_zvertex_reweight" << std::endl;
    return;
  }

  ////////// Histograms //////////
  TH1D *h_event = new TH1D("h_event", ";Event Number", 1, 0, 1);
  TH1D *h_recojet_pt_record = new TH1D("h_recojet_pt_record", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_passcut_record = new TH1D("h_recojet_pt_passcut_record", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_truthjet_pt_record = new TH1D("h_truthjet_pt_record", ";p_{T}^{Truth jet} [GeV]", 1000, 0, 100);
  TH1D *h_truthjet_pt_passcut_record = new TH1D("h_truthjet_pt_passcut_record", ";p_{T}^{Truth jet} [GeV]", 1000, 0, 100);

  TH1D *h_truth_nominal = new TH1D("h_truth_nominal", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D *h_measure_nominal = new TH1D("h_measure_nominal", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D *h_respmatrix_nominal = new TH2D("h_respmatrix_nominal", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D *h_fake_nominal = new TH1D("h_fake_nominal", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_miss_nominal = new TH1D("h_miss_nominal", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  // JES up/down histograms
  TH1D *h_truth_jesup = new TH1D("h_truth_jesup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D *h_measure_jesup = new TH1D("h_measure_jesup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D *h_respmatrix_jesup = new TH2D("h_respmatrix_jesup", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D *h_fake_jesup = new TH1D("h_fake_jesup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_miss_jesup = new TH1D("h_miss_jesup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D *h_truth_jesdown = new TH1D("h_truth_jesdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D *h_measure_jesdown = new TH1D("h_measure_jesdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D *h_respmatrix_jesdown = new TH2D("h_respmatrix_jesdown", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D *h_fake_jesdown = new TH1D("h_fake_jesdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_miss_jesdown = new TH1D("h_miss_jesdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  // JER up/down histograms
  TH1D *h_truth_jerup = new TH1D("h_truth_jerup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D *h_measure_jerup = new TH1D("h_measure_jerup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D *h_respmatrix_jerup = new TH2D("h_respmatrix_jerup", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D *h_fake_jerup = new TH1D("h_fake_jerup", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_miss_jerup = new TH1D("h_miss_jerup", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D *h_truth_jerdown = new TH1D("h_truth_jerdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH1D *h_measure_jerdown = new TH1D("h_measure_jerdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH2D *h_respmatrix_jerdown = new TH2D("h_respmatrix_jerdown", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D *h_fake_jerdown = new TH1D("h_fake_jerdown", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_miss_jerdown = new TH1D("h_miss_jerdown", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  // Closure test histograms
  TH1D *h_fullclosure_measure = new TH1D("h_fullclosure_measure", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_fullclosure_truth = new TH1D("h_fullclosure_truth", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH2D *h_fullclosure_respmatrix = new TH2D("h_fullclosure_respmatrix", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
  TH1D *h_fullclosure_fake = new TH1D("h_fullclosure_fake", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_fullclosure_miss = new TH1D("h_fullclosure_miss", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D *h_halfclosure_measure = new TH1D("h_halfclosure_measure", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_halfclosure_truth = new TH1D("h_halfclosure_truth", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH2D *h_halfclosure_respmatrix = new TH2D("h_halfclosure_respmatrix", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt , truthptbins);
  TH1D *h_halfclosure_fake = new TH1D("h_halfclosure_fake", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_halfclosure_miss = new TH1D("h_halfclosure_miss", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  TH1D *h_halfclosure_measure_check = new TH1D("h_halfclosure_measure_check", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_halfclosure_truth_check = new TH1D("h_halfclosure_truth_check", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);
  TH2D *h_halfclosure_respmatrix_check = new TH2D("h_halfclosure_respmatrix_check", ";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt , truthptbins);
  TH1D *h_halfclosure_fake_check = new TH1D("h_halfclosure_fake_check", ";p_{T}^{Calib jet} [GeV]", calibnpt, calibptbins);
  TH1D *h_halfclosure_miss_check = new TH1D("h_halfclosure_miss_check", ";p_{T}^{Truth jet} [GeV]", truthnpt, truthptbins);

  ////////// Event Loop //////////
  std::cout << "Data analysis started." << std::endl;
  int n_events = chain.GetEntries();
  //n_events = 10;
  std::cout << "Total number of events: " << n_events << std::endl;
  // Event variables setup.
  std::vector<bool> jet_filter, truthjet_filter;
  std::vector<float> goodtruthjet_pt, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched;
  std::vector<float> calibjet_pt_nominal, calibjet_eta_nominal, calibjet_phi_nominal, calibjet_matched_nominal;
  std::vector<float> calibjet_pt_jesup, calibjet_eta_jesup, calibjet_phi_jesup, calibjet_matched_jesup;
  std::vector<float> calibjet_pt_jesdown, calibjet_eta_jesdown, calibjet_phi_jesdown, calibjet_matched_jesdown;
  std::vector<float> calibjet_pt_jerup, calibjet_eta_jerup, calibjet_phi_jerup, calibjet_matched_jerup;
  std::vector<float> calibjet_pt_jerdown, calibjet_eta_jerdown, calibjet_phi_jerdown, calibjet_matched_jerdown;
  for (int ie = 0; ie < n_events; ++ie) { // event loop start
    // Load event.
    if (ie % 10000 == 0) {
      std::cout << "Processing event " << ie << "..." << std::endl;
    }
    chain.GetEntry(ie);

    // Get z-vertex index, reweighting factor and re-assign no-zvertex events.
    int zvertex_index = get_zvertex_index(zvertex);
    double scale_zvertexreweight = h_zvertex_reweight->GetBinContent(h_zvertex_reweight->GetXaxis()->FindBin(zvertex));
    if (zvertex < -900) zvertex = 0;

    // Do data set efficiency check.
    int leadingtruthjet_index = -1;
    float leadingtruthjet_pt = -9999;
    for (int ij = 0; ij < truthjet_pt->size(); ++ij) {
      if (truthjet_eta->at(ij) > 1.1 - jet_radius || truthjet_eta->at(ij) < -1.1 + jet_radius) continue;
      float jetpt = truthjet_pt->at(ij);
      if (jetpt > leadingtruthjet_pt) {
        leadingtruthjet_pt = jetpt;
        leadingtruthjet_index = ij;
      }
    }
    if (leadingtruthjet_index < 0) continue;
    if (leadingtruthjet_pt < truthjet_pt_min || leadingtruthjet_pt > truthjet_pt_max) continue;

    // Event selection.
    bool pass_dijet = false;
    bool pass_njet = false;
    // Leading and subleading jet with energy and R = 0.4 for dijet and timing requirement. 
    int leadingunsubjet04_index = -1;
    int subleadingunsubjet04_index = -1;
    get_leading_subleading_jet(leadingunsubjet04_index, subleadingunsubjet04_index, unsubjet04_e);
    if (leadingunsubjet04_index < 0) continue;
    // Dijet requirement.
    if (subleadingunsubjet04_index >= 0 && unsubjet04_e->at(subleadingunsubjet04_index) / (float) unsubjet04_e->at(leadingunsubjet04_index) > 0.3) {
      pass_dijet = match_leading_subleading_jet(unsubjet04_phi->at(leadingunsubjet04_index), unsubjet04_phi->at(subleadingunsubjet04_index));
    }
    // Njet requirement.
    int n_jets = 0;
    for (int ij = 0; ij < unsubjet04_e->size(); ++ij) {
      if (unsubjet04_e->at(ij) > 5.0) {
        n_jets++;
      }
    }
    if (n_jets <= 9) {
      pass_njet = true;
    }

    // Do jet filter
    get_jet_filter(jet_filter, unsubjet_e, unsubjet_pt, unsubjet_eta, zvertex, jet_radius);
    get_jet_filter(truthjet_filter, truthjet_e, truthjet_pt, truthjet_eta, zvertex, jet_radius);
    for (int ij = 0; ij < unsubjet_pt->size(); ++ij) {
      if (jet_filter[ij]) continue;
      h_recojet_pt_record->Fill(unsubjet_pt->at(ij));
      if (pass_dijet && pass_njet) h_recojet_pt_passcut_record->Fill(unsubjet_pt->at(ij));
    }
    for (int ij = 0; ij < truthjet_pt->size(); ++ij) {
      if (truthjet_filter[ij]) continue;
      h_truthjet_pt_record->Fill(truthjet_pt->at(ij));
      if (pass_dijet && pass_njet) h_truthjet_pt_passcut_record->Fill(truthjet_pt->at(ij));
    }

    // Get truth jet and calib jet.
    get_truthjet(goodtruthjet_pt, goodtruthjet_eta, goodtruthjet_phi, truthjet_filter, truthjet_pt, truthjet_eta, truthjet_phi);

    get_calibjet(calibjet_pt_nominal, calibjet_eta_nominal, calibjet_phi_nominal, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, f_corr, zvertex_index, jet_radius, 1, 0.1, (!pass_dijet||!pass_njet));
    get_calibjet(calibjet_pt_jesup, calibjet_eta_jesup, calibjet_phi_jesup, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, f_corr, zvertex_index, jet_radius, 1.06, 0.1, (!pass_dijet||!pass_njet));
    get_calibjet(calibjet_pt_jesdown, calibjet_eta_jesdown, calibjet_phi_jesdown, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, f_corr, zvertex_index, jet_radius, 0.94, 0.1, (!pass_dijet||!pass_njet));
    get_calibjet(calibjet_pt_jerup, calibjet_eta_jerup, calibjet_phi_jerup, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, f_corr, zvertex_index, jet_radius, 1, 0.13, (!pass_dijet||!pass_njet));
    get_calibjet(calibjet_pt_jerdown, calibjet_eta_jerdown, calibjet_phi_jerdown, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, f_corr, zvertex_index, jet_radius, 1, 0.07, (!pass_dijet||!pass_njet));

    // Match truth jet and calib jet.
    match_meas_truth(calibjet_eta_nominal, calibjet_phi_nominal, calibjet_matched_nominal, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_nominal, h_measure_nominal, h_respmatrix_nominal, h_fake_nominal, h_miss_nominal, scale_zvertexreweight, calibjet_pt_nominal, calibjet_matched_nominal, goodtruthjet_pt, goodtruthjet_matched);
    
    fill_response_matrix(h_fullclosure_truth, h_fullclosure_measure, h_fullclosure_respmatrix, h_fullclosure_fake, h_fullclosure_miss, scale_zvertexreweight, calibjet_pt_nominal, calibjet_matched_nominal, goodtruthjet_pt, goodtruthjet_matched);
    if (ie % 2 == 0) {
      fill_response_matrix(h_halfclosure_truth, h_halfclosure_measure, h_halfclosure_respmatrix, h_halfclosure_fake, h_halfclosure_miss, scale_zvertexreweight, calibjet_pt_nominal, calibjet_matched_nominal, goodtruthjet_pt, goodtruthjet_matched);
    } else {
      fill_response_matrix(h_halfclosure_truth_check, h_halfclosure_measure_check, h_halfclosure_respmatrix_check, h_halfclosure_fake_check, h_halfclosure_miss_check, scale_zvertexreweight, calibjet_pt_nominal, calibjet_matched_nominal, goodtruthjet_pt, goodtruthjet_matched);
    }

    match_meas_truth(calibjet_eta_jesup, calibjet_phi_jesup, calibjet_matched_jesup, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_jesup, h_measure_jesup, h_respmatrix_jesup, h_fake_jesup, h_miss_jesup, scale_zvertexreweight, calibjet_pt_jesup, calibjet_matched_jesup, goodtruthjet_pt, goodtruthjet_matched);

    match_meas_truth(calibjet_eta_jesdown, calibjet_phi_jesdown, calibjet_matched_jesdown, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_jesdown, h_measure_jesdown, h_respmatrix_jesdown, h_fake_jesdown, h_miss_jesdown, scale_zvertexreweight, calibjet_pt_jesdown, calibjet_matched_jesdown, goodtruthjet_pt, goodtruthjet_matched);

    match_meas_truth(calibjet_eta_jerup, calibjet_phi_jerup, calibjet_matched_jerup, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_jerup, h_measure_jerup, h_respmatrix_jerup, h_fake_jerup, h_miss_jerup, scale_zvertexreweight, calibjet_pt_jerup, calibjet_matched_jerup, goodtruthjet_pt, goodtruthjet_matched);

    match_meas_truth(calibjet_eta_jerdown, calibjet_phi_jerdown, calibjet_matched_jerdown, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius);
    fill_response_matrix(h_truth_jerdown, h_measure_jerdown, h_respmatrix_jerdown, h_fake_jerdown, h_miss_jerdown, scale_zvertexreweight, calibjet_pt_jerdown, calibjet_matched_jerdown, goodtruthjet_pt, goodtruthjet_matched);
  } // event loop end
  // Fill event histograms.
  h_event->Fill(0.5, n_events);

  // Write histograms.
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();
  h_event->Write();
  h_recojet_pt_record->Write();
  h_recojet_pt_passcut_record->Write();
  h_truthjet_pt_record->Write();
  h_truthjet_pt_passcut_record->Write();

  h_truth_nominal->Write();
  h_measure_nominal->Write();
  h_respmatrix_nominal->Write();
  h_fake_nominal->Write();
  h_miss_nominal->Write();

  h_truth_jesup->Write();
  h_measure_jesup->Write();
  h_respmatrix_jesup->Write();
  h_fake_jesup->Write();
  h_miss_jesup->Write();

  h_truth_jesdown->Write();
  h_measure_jesdown->Write();
  h_respmatrix_jesdown->Write();
  h_fake_jesdown->Write();
  h_miss_jesdown->Write();

  h_truth_jerup->Write();
  h_measure_jerup->Write();
  h_respmatrix_jerup->Write();
  h_fake_jerup->Write();
  h_miss_jerup->Write();

  h_truth_jerdown->Write();
  h_measure_jerdown->Write();
  h_respmatrix_jerdown->Write();
  h_fake_jerdown->Write();
  h_miss_jerdown->Write();

  h_fullclosure_measure->Write();
  h_fullclosure_truth->Write();
  h_fullclosure_respmatrix->Write();
  h_fullclosure_fake->Write();
  h_fullclosure_miss->Write();

  h_halfclosure_measure->Write();
  h_halfclosure_truth->Write();
  h_halfclosure_respmatrix->Write();
  h_halfclosure_fake->Write();
  h_halfclosure_miss->Write();

  h_halfclosure_measure_check->Write();
  h_halfclosure_truth_check->Write();
  h_halfclosure_respmatrix_check->Write();
  h_halfclosure_fake_check->Write();
  h_halfclosure_miss_check->Write();

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

bool check_bad_jet_eta(float jet_eta, float zvertex, float jet_radius) {
  float emcal_mineta = get_emcal_mineta_zcorrected(zvertex);
  float emcal_maxeta = get_emcal_maxeta_zcorrected(zvertex);
  float ihcal_mineta = get_ihcal_mineta_zcorrected(zvertex);
  float ihcal_maxeta = get_ihcal_maxeta_zcorrected(zvertex);
  float ohcal_mineta = get_ohcal_mineta_zcorrected(zvertex);
  float ohcal_maxeta = get_ohcal_maxeta_zcorrected(zvertex);
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

int get_zvertex_index(float zvertex) {
  if (zvertex >= -60.0 && zvertex < -30.0) return 1;
  else if (zvertex >= -30.0 && zvertex < 30.0) return 0;
  else if (zvertex >= 30.0 && zvertex < 60) return 2;
  else if ((zvertex < -60.0 && zvertex >= -900) || zvertex >= 60.0) return 3;
  else if (zvertex < -900) return 4;
  return -1; // should not reach here
}

int get_eta_index(float jet_eta, int zvertex_index, double jet_radius) {
  float eta_low = 0;
  float eta_high = 0;
  if (zvertex_index == 0) {// -30 < zvrtx < 30
    eta_low = -1.2 + jet_radius;
    eta_high = 1.2 - jet_radius;
  } else if (zvertex_index == 1) {// -60 < zvrtx < -30
    eta_low = -0.95 + jet_radius;
    eta_high = 1.25 - jet_radius;
  } else if (zvertex_index == 2) {// 30 < zvrtx < inf
    eta_low = -1.25 + jet_radius;
    eta_high = 0.95 - jet_radius;
  } else if (zvertex_index == 3 || zvertex_index == 4) {
    return 0;
  }
  float threshold1 = eta_low + ((eta_high - eta_low) / 4.0);
  float threshold2 = eta_low + ((eta_high - eta_low) / 2.0);
  float threshold3 = eta_low + (3.0 * (eta_high - eta_low) / 4.0);
  if (jet_eta < threshold1) return 0;  // -inf to threshold1
  if (jet_eta < threshold2) return 1;  // threshold1 to threshold2
  if (jet_eta < threshold3) return 2;  // threshold2 to threshold3
  return 3;  // threshold3 to inf
}

////////////////////////////////////////// Functions //////////////////////////////////////////
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
  float dphi = fabs(get_dphi(leadingjet_phi, subleadingjet_phi));
  return dphi > dijet_min_phi;
}

void get_jet_filter(std::vector<bool>& jet_filter, std::vector<float>* jet_e, std::vector<float>* jet_pt, std::vector<float>* jet_eta, float zvertex, float jet_radius) {
  jet_filter.clear();
  int njet = jet_e->size();
  for (int ij = 0; ij < njet; ++ij) {
    jet_filter.push_back(jet_e->at(ij) < 0 || check_bad_jet_eta(jet_eta->at(ij), zvertex, jet_radius));
  }
}

void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& jet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi, const std::vector<std::vector<TF1*>>& f_corr, int zvertex_index, double jet_radius, float jes_para, float jer_para, bool jet_background) {
  calibjet_pt.clear();
  calibjet_eta.clear();
  calibjet_phi.clear();
  for (int ij = 0; ij < jet_filter.size(); ++ij) {
    if (jet_filter.at(ij) || jet_background) continue;
    int eta_index;
    if (zvertex_index == 0 || zvertex_index == 1 || zvertex_index == 2) {
      eta_index = get_eta_index(jet_eta->at(ij), zvertex_index, jet_radius);
    } else {
      eta_index = 0;
    }
    double calib_pt = f_corr[zvertex_index][eta_index]->Eval(jet_pt->at(ij)) * (1 + randGen.Gaus(0.0, jer_para)) * jes_para;
    //std::cout << "Debug: jet_pt = " << jet_pt->at(ij) << ", calib_pt = " << calib_pt << " with zvertex = " << zvertex_index << ", eta = " << jet_eta->at(ij) << std::endl;
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

void fill_response_matrix(TH1D*& h_truth, TH1D*& h_meas, TH2D*& h_resp, TH1D*& h_fake, TH1D*& h_miss, double scale, std::vector<float>& meas_pt, std::vector<float>& meas_matched, std::vector<float>& truth_pt, std::vector<float>& truth_matched) {
  for (int im = 0; im < meas_pt.size(); ++im) {
    h_meas->Fill(meas_pt[im], scale);
    if (meas_matched[im] < 0) {
      h_fake->Fill(meas_pt[im], scale);
    } else {
      h_resp->Fill(meas_pt[im], truth_pt[meas_matched[im]], scale);
    }
  }
  for (int it = 0; it < truth_pt.size(); ++it) {
    h_truth->Fill(truth_pt[it], scale);
    if (truth_matched[it] < 0) {
      h_miss->Fill(truth_pt[it], scale);
    }
  }
}
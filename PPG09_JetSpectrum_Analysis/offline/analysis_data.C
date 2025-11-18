#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "unfold_Def.h"

void get_leading_subleading_jet(int& leadingjet_index, int& subleadingjet_index, std::vector<float>* jet_et);
bool match_leading_subleading_jet(float leadingjet_phi, float subleadingjet_phi);
void get_jet_filter(std::vector<bool>& jet_filter, std::vector<float>* jet_e, std::vector<float>* jet_pt, std::vector<float>* jet_eta, float zvertex, float jet_radius);
int get_zvertex_index(float zvertex);
void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& jet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi, const std::vector<std::vector<TF1*>>& f_corr, int zvertex_index, double jet_radius);

////////////////////////////////////////// Main Function //////////////////////////////////////////
void analysis_data(int runnumber, int nseg, int iseg, double jet_radius) {
  /////////////// General Set up ///////////////
  int jet_radius_index = (int)(10 * jet_radius);
  TFile *f_out = new TFile(Form("output_data/output_r0%d_%d_%d_%d.root", jet_radius_index, runnumber, iseg, iseg+nseg),"RECREATE");

  /////////////// Read Files ///////////////
  const char* baseDirJet = "/sphenix/tg/tg01/jets/hanpuj/CaloDataAna_ppg09";
  TChain chain("ttree");
  for (int i = iseg; i < iseg + nseg; ++i) {
    chain.Add(Form("%s/Run%d/OutDir%d/output_data.root", baseDirJet, runnumber, i));
  }
  chain.SetBranchStatus("*", 0);
  // Read necessary branches for analysis.
  float zvertex; chain.SetBranchStatus("z_vertex", 1); chain.SetBranchAddress("z_vertex", &zvertex);
  int gl1_trigger_vector_scaled[64]; chain.SetBranchStatus("gl1_trigger_vector_scaled", 1); chain.SetBranchAddress("gl1_trigger_vector_scaled", gl1_trigger_vector_scaled);  
  std::vector<float>* unsubjet_e = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_e", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_e", jet_radius_index), &unsubjet_e);
  std::vector<float>* unsubjet_pt = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_pt", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_pt", jet_radius_index), &unsubjet_pt);
  std::vector<float>* unsubjet_eta = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_eta", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_eta", jet_radius_index), &unsubjet_eta);
  std::vector<float>* unsubjet_phi = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_phi", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_phi", jet_radius_index), &unsubjet_phi);
  std::vector<float>* unsubjet_time = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_time", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_time", jet_radius_index), &unsubjet_time);
  // Read R=0.4 jet branches for event selection.
  std::vector<float>* unsubjet04_e_reader = nullptr, *unsubjet04_pt_reader = nullptr, *unsubjet04_eta_reader = nullptr, *unsubjet04_phi_reader = nullptr, *unsubjet04_time_reader = nullptr;
  if (jet_radius_index != 4) {
    chain.SetBranchStatus("unsubjet04_e", 1); chain.SetBranchAddress("unsubjet04_e", &unsubjet04_e_reader);
    chain.SetBranchStatus("unsubjet04_pt", 1); chain.SetBranchAddress("unsubjet04_pt", &unsubjet04_pt_reader);
    chain.SetBranchStatus("unsubjet04_eta", 1); chain.SetBranchAddress("unsubjet04_eta", &unsubjet04_eta_reader);
    chain.SetBranchStatus("unsubjet04_phi", 1); chain.SetBranchAddress("unsubjet04_phi", &unsubjet04_phi_reader);
    chain.SetBranchStatus("unsubjet04_time", 1); chain.SetBranchAddress("unsubjet04_time", &unsubjet04_time_reader);
  }
  std::vector<float>* &unsubjet04_e = (jet_radius_index != 4) ? unsubjet04_e_reader : unsubjet_e;
  std::vector<float>* &unsubjet04_pt = (jet_radius_index != 4) ? unsubjet04_pt_reader : unsubjet_pt;
  std::vector<float>* &unsubjet04_eta = (jet_radius_index != 4) ? unsubjet04_eta_reader : unsubjet_eta;
  std::vector<float>* &unsubjet04_phi = (jet_radius_index != 4) ? unsubjet04_phi_reader : unsubjet_phi;
  std::vector<float>* &unsubjet04_time = (jet_radius_index != 4) ? unsubjet04_time_reader : unsubjet_time;

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

  /////////////// Jet Trigger Efficiency ///////////////
  TFile *f_jettrigger = new TFile("input_jetefficiency.root", "READ");
  if (!f_jettrigger) {
    std::cout << "Error: cannot open input_jetefficiency.root" << std::endl;
    return;
  }
  TF1* f_jettrigger_nominal = (TF1*)f_jettrigger->Get(Form("jettrig_0%d_pt_nominal", jet_radius_index));
  TF1* f_jettrigger_up = (TF1*)f_jettrigger->Get(Form("jettrig_0%d_pt_up", jet_radius_index));
  TF1* f_jettrigger_down = (TF1*)f_jettrigger->Get(Form("jettrig_0%d_pt_down", jet_radius_index));
  if (!f_jettrigger_nominal || !f_jettrigger_up || !f_jettrigger_down) {
    std::cout << "Error: cannot open jet trigger functions" << std::endl;
    return;
  }

  /////////////// Histograms ///////////////
  TH1D *h_recojet_pt_record = new TH1D("h_recojet_pt_record", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record = new TH1D("h_calibjet_pt_record", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_scaled_nominal = new TH1D("h_calibjet_pt_scaled_nominal", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_scaled_jettrigup = new TH1D("h_calibjet_pt_scaled_jettrigup", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_scaled_jettrigdown = new TH1D("h_calibjet_pt_scaled_jettrigdown", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_scaled_jettimingup = new TH1D("h_calibjet_pt_scaled_jettimingup", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_scaled_jettimingdown = new TH1D("h_calibjet_pt_scaled_jettimingdown", ";p_{T} [GeV]", calibnpt, calibptbins);

  /////////////// Event Loop ///////////////
  std::cout << "Data analysis started." << std::endl;
  Long64_t n_events = chain.GetEntries();
  std::cout << "Total number of events: " << n_events << std::endl;
  // Event variables setup.
  std::vector<bool> jet_filter;
  std::vector<float> calibjet_pt, calibjet_eta, calibjet_phi;
  for (Long64_t ie = 0; ie < n_events; ++ie) { // event loop start
    // Load event.
    if (ie % 10000 == 0) {
      std::cout << "Processing event " << ie << "..." << std::endl;
    }
    chain.GetEntry(ie);

    // Trigger check.
    if (gl1_trigger_vector_scaled[22] != 1) continue;

    // Get z-vertex index and re-assign no-zvertex events.
    int zvertex_index = get_zvertex_index(zvertex);
    if (zvertex < -990) zvertex = 0;

    // Event selection.
    bool pass_dijet = false;
    bool pass_timing = false;
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
    // Timing requirement.
    if (subleadingunsubjet04_index >= 0 && fabs(unsubjet04_time->at(leadingunsubjet04_index)*17.6+2) < 6 && fabs(unsubjet04_time->at(leadingunsubjet04_index)*17.6 - unsubjet04_time->at(subleadingunsubjet04_index)*17.6) < 3) {
      pass_timing = true;
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
    if (!pass_dijet || !pass_timing || !pass_njet) continue;

    // Leading jet with pT for trigger efficiency.
    int leadingunsubjet_index = -1;
    int subleadingunsubjet_index = -1;
    get_leading_subleading_jet(leadingunsubjet_index, subleadingunsubjet_index, unsubjet_pt);
    if (leadingunsubjet_index < 0) continue;
    double leadingunsubjet_pt = unsubjet_pt->at(leadingunsubjet_index);

    // Get jet trigger scale factors.
    double jettrigger_nominal = f_jettrigger_nominal->Eval(leadingunsubjet_pt);
    if (jettrigger_nominal < 0.01) jettrigger_nominal = 0.01;
    double scale_jettrig_nominal = 1.0 / jettrigger_nominal;

    double jettrigger_up = f_jettrigger_up->Eval(leadingunsubjet_pt);
    if (jettrigger_up < 0.01) jettrigger_up = 0.01;
    double scale_jettrig_up = 1.0 / jettrigger_up;

    double jettrigger_down = f_jettrigger_down->Eval(leadingunsubjet_pt);
    if (jettrigger_down < 0.01) jettrigger_down = 0.01;
    double scale_jettrig_down = 1.0 / jettrigger_down;

    // Get jet timing scale factors.
    double jettiming_nominal = 0.92;
    double scale_jettiming_nominal = 1.0 / jettiming_nominal;

    double jettiming_up = 0.95;
    double scale_jettiming_up = 1.0 / jettiming_up;

    double jettiming_down = 0.89;
    double scale_jettiming_down = 1.0 / jettiming_down;

    // Fill histograms.
    get_jet_filter(jet_filter, unsubjet_e, unsubjet_pt, unsubjet_eta, zvertex, jet_radius);
    for (int ij = 0; ij < jet_filter.size(); ++ij) {
      if (jet_filter.at(ij)) continue;
      h_recojet_pt_record->Fill(unsubjet_pt->at(ij));
    }

    get_calibjet(calibjet_pt, calibjet_eta, calibjet_phi, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, f_corr, zvertex_index, jet_radius);
    for (int ij = 0; ij < calibjet_pt.size(); ++ij) {
      h_calibjet_pt_record->Fill(calibjet_pt[ij]);
      h_calibjet_pt_scaled_nominal->Fill(calibjet_pt[ij], scale_jettrig_nominal * scale_jettiming_nominal);
      h_calibjet_pt_scaled_jettrigup->Fill(calibjet_pt[ij], scale_jettrig_up * scale_jettiming_nominal);
      h_calibjet_pt_scaled_jettrigdown->Fill(calibjet_pt[ij], scale_jettrig_down * scale_jettiming_nominal);
      h_calibjet_pt_scaled_jettimingup->Fill(calibjet_pt[ij], scale_jettrig_nominal * scale_jettiming_up);
      h_calibjet_pt_scaled_jettimingdown->Fill(calibjet_pt[ij], scale_jettrig_nominal * scale_jettiming_down);
    }
  } // event loop end

  // Write histograms.
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();
  h_recojet_pt_record->Write();
  h_calibjet_pt_record->Write();
  h_calibjet_pt_scaled_nominal->Write();
  h_calibjet_pt_scaled_jettrigup->Write();
  h_calibjet_pt_scaled_jettrigdown->Write();
  h_calibjet_pt_scaled_jettimingup->Write();
  h_calibjet_pt_scaled_jettimingdown->Write();
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
    jet_filter.push_back(jet_e->at(ij) < 0 || check_bad_jet_eta(jet_eta->at(ij), zvertex, jet_radius) || jet_pt->at(ij) < 1);
  }
}

void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& jet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi, const std::vector<std::vector<TF1*>>& f_corr, int zvertex_index, double jet_radius) {
  calibjet_pt.clear();
  calibjet_eta.clear();
  calibjet_phi.clear();
  for (int ij = 0; ij < jet_filter.size(); ++ij) {
    if (jet_filter.at(ij)) continue;
    int eta_index;
    if (zvertex_index == 0 || zvertex_index == 1 || zvertex_index == 2) {
      eta_index = get_eta_index(jet_eta->at(ij), zvertex_index, jet_radius);
    } else {
      eta_index = 0;
    }
    double calib_pt = f_corr[zvertex_index][eta_index]->Eval(jet_pt->at(ij));
    if (calib_pt < calibptbins[0] || calib_pt > calibptbins[calibnpt]) continue;
    calibjet_pt.push_back(calib_pt);
    calibjet_eta.push_back(jet_eta->at(ij));
    calibjet_phi.push_back(jet_phi->at(ij));
  }
}
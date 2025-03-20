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

bool check_bad_trigger(int gl1_trigger_vector_scaled[n_hcal_phibin]);
void get_leading_subleading_jet(int& leadingjet_index, int& subleadingjet_index, std::vector<float>* jet_et);
bool match_leading_subleading_jet(float leadingjet_phi, float subleadingjet_phi);
void get_jet_filter(std::vector<bool>& jet_filter, std::vector<float>* jet_e, std::vector<float>* jet_pt, std::vector<float>* jet_eta, float zvertex, float jet_radius);
void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& calibjet_background_dijet, std::vector<bool>& calibjet_background_frac, std::vector<bool>& jet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi, std::vector<bool>* jet_background_dijet, std::vector<bool>* jet_background_frac, TF1* f_corr);

////////////////////////////////////////// Main Function //////////////////////////////////////////
void analysis_data(int runnumber, int nseg, int iseg)  {
  /////////////// General Set up ///////////////
  TFile *f_out = new TFile(Form("output_data/output_%d_%d_%d.root", runnumber, iseg, iseg+nseg),"RECREATE");

  /////////////// Read Files ///////////////
  const char* baseDirJet = "/sphenix/tg/tg01/jets/hanpuj/CaloDataAna_skimmed";
  TChain chain("ttree");
  for (int i = iseg; i < iseg + nseg; ++i) {
    chain.Add(Form("%s/Run%d/OutDir%d/output_data.root", baseDirJet, runnumber, i));
  }
  chain.SetBranchStatus("*", 0);

  float zvertex; chain.SetBranchStatus("z_vertex", 1); chain.SetBranchAddress("z_vertex", &zvertex);
  int gl1_trigger_vector_scaled[64]; chain.SetBranchStatus("gl1_trigger_vector_scaled", 1); chain.SetBranchAddress("gl1_trigger_vector_scaled", &gl1_trigger_vector_scaled);
  std::vector<float>* unsubjet_e = nullptr; chain.SetBranchStatus("unsubjet04_e", 1); chain.SetBranchAddress("unsubjet04_e", &unsubjet_e);
  std::vector<float>* unsubjet_pt = nullptr; chain.SetBranchStatus("unsubjet04_pt", 1); chain.SetBranchAddress("unsubjet04_pt", &unsubjet_pt);
  std::vector<float>* unsubjet_et = nullptr; chain.SetBranchStatus("unsubjet04_et", 1); chain.SetBranchAddress("unsubjet04_et", &unsubjet_et);
  std::vector<float>* unsubjet_eta = nullptr; chain.SetBranchStatus("unsubjet04_eta", 1); chain.SetBranchAddress("unsubjet04_eta", &unsubjet_eta);
  std::vector<float>* unsubjet_phi = nullptr; chain.SetBranchStatus("unsubjet04_phi", 1); chain.SetBranchAddress("unsubjet04_phi", &unsubjet_phi);
  std::vector<float>* unsubjet_emcal_calo_e = nullptr; chain.SetBranchStatus("unsubjet04_emcal_calo_e", 1); chain.SetBranchAddress("unsubjet04_emcal_calo_e", &unsubjet_emcal_calo_e);
  std::vector<float>* unsubjet_ihcal_calo_e = nullptr; chain.SetBranchStatus("unsubjet04_ihcal_calo_e", 1); chain.SetBranchAddress("unsubjet04_ihcal_calo_e", &unsubjet_ihcal_calo_e);
  std::vector<float>* unsubjet_ohcal_calo_e = nullptr; chain.SetBranchStatus("unsubjet04_ohcal_calo_e", 1); chain.SetBranchAddress("unsubjet04_ohcal_calo_e", &unsubjet_ohcal_calo_e);

  /////////////// JES func ///////////////
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

  /////////////// Trigger Efficiency ///////////////
  TFile *f_trigger = new TFile("output_jettrigeff.root", "READ");
  if (!f_trigger) {
    std::cout << "Error: cannot open output_jettrigeff.root" << std::endl;
    return;
  }
  TF1* f_turnon = (TF1*)f_trigger->Get("f_fit");
  TF1* f_turnon_down = (TF1*)f_trigger->Get("f_fit_shiftdown"); // for trigger efficiency uncertainty
  TF1* f_turnon_up = (TF1*)f_trigger->Get("f_fit_shiftup"); // for trigger efficiency uncertainty
  if (!f_turnon || !f_turnon_down || !f_turnon_up) {
    std::cout << "Error: cannot open trigger efficiency functions" << std::endl;
    return;
  }

  /////////////// Histograms ///////////////
  TH1D *h_zvertex = new TH1D("h_zvertex", ";Z-vertex [cm]", 60, -30, 30);
  TH1D *h_recojet_pt_record = new TH1D("h_recojet_pt_record", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_record_dijet = new TH1D("h_recojet_pt_record_dijet", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_record_frac = new TH1D("h_recojet_pt_record_frac", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt = new TH1D("h_calibjet_pt", ";p_{T} [GeV]", calibnpt, calibptbins); // raw calib jet pT spectrum
  TH1D *h_calibjet_pt_record = new TH1D("h_calibjet_pt_record", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_dijet_eff = new TH1D("h_calibjet_pt_dijet_eff", ";p_{T} [GeV]", calibnpt, calibptbins); // with trigger efficiency correction applied
  TH1D *h_calibjet_pt_dijet_effdown = new TH1D("h_calibjet_pt_dijet_effdown", ";p_{T} [GeV]", calibnpt, calibptbins); // for trigger efficiency uncertainty
  TH1D *h_calibjet_pt_dijet_effup = new TH1D("h_calibjet_pt_dijet_effup", ";p_{T} [GeV]", calibnpt, calibptbins); // for trigger efficiency uncertainty
  TH1D *h_calibjet_pt_frac_eff = new TH1D("h_calibjet_pt_frac_eff", ";p_{T} [GeV]", calibnpt, calibptbins); // with trigger efficiency correction applied
  TH1D *h_calibjet_pt_frac_effdown = new TH1D("h_calibjet_pt_frac_effdown", ";p_{T} [GeV]", calibnpt, calibptbins); // for trigger efficiency uncertainty
  TH1D *h_calibjet_pt_frac_effup = new TH1D("h_calibjet_pt_frac_effup", ";p_{T} [GeV]", calibnpt, calibptbins); // for trigger efficiency uncertainty

  /////////////// Event Loop ///////////////
  std::cout << "Data analysis started." << std::endl;
  int n_events = chain.GetEntries();
  std::cout << "Total number of events: " << n_events << std::endl;
  // Event variables setup.
  std::vector<bool>* unsubjet_background_dijet = new std::vector<bool>;
  std::vector<bool>* unsubjet_background_frac = new std::vector<bool>;
  std::vector<bool> jet_filter;
  std::vector<float> calibjet_pt, calibjet_eta, calibjet_phi;
  std::vector<bool> calibjet_background_dijet, calibjet_background_frac;
  for (int ie = 0; ie < n_events; ++ie) { // event loop start
    // Load event.
    if (ie % 1000 == 0) {
      std::cout << "Processing event " << ie << "..." << std::endl;
    }
    chain.GetEntry(ie);

    // Trigger and Z-vertex cut.
    if (check_bad_trigger(gl1_trigger_vector_scaled)) continue;
    if (fabs(zvertex) > 30) continue;

    // Get leading jet and subleading jet. Do basic jet cuts.
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

    // Trigger efficiency.
    double leadingunsubjet_et = unsubjet_et->at(leadingunsubjet_index);

    double jettrigeff = f_turnon->Eval(leadingunsubjet_et);
    if (jettrigeff < 0.01) jettrigeff = 0.01;
    double jettrig_scale = 1.0 / jettrigeff;

    double jettrigeff_down = f_turnon_down->Eval(leadingunsubjet_et);
    if (jettrigeff_down < 0.01) jettrigeff_down = 0.01;
    double jettrig_scale_down = 1.0 / jettrigeff_down;

    double jettrigeff_up = f_turnon_up->Eval(leadingunsubjet_et);
    if (jettrigeff_up < 0.01) jettrigeff_up = 0.01;
    double jettrig_scale_up = 1.0 / jettrigeff_up;

    // Fill histograms.
    get_jet_filter(jet_filter, unsubjet_e, unsubjet_pt, unsubjet_eta, zvertex, jet_radius);
    for (int ij = 0; ij < jet_filter.size(); ++ij) {
      if (jet_filter.at(ij)) continue;
      h_recojet_pt_record->Fill(unsubjet_pt->at(ij));
      if (!unsubjet_background_dijet->at(ij)) h_recojet_pt_record_dijet->Fill(unsubjet_pt->at(ij));
      if (!unsubjet_background_frac->at(ij)) h_recojet_pt_record_frac->Fill(unsubjet_pt->at(ij));
    }

    get_calibjet(calibjet_pt, calibjet_eta, calibjet_phi, calibjet_background_dijet, calibjet_background_frac, jet_filter, unsubjet_pt, unsubjet_eta, unsubjet_phi, unsubjet_background_dijet, unsubjet_background_frac, f_corr);
    for (int ij = 0; ij < calibjet_pt.size(); ++ij) {
      h_calibjet_pt->Fill(calibjet_pt[ij]);
      h_calibjet_pt_record->Fill(calibjet_pt[ij]);
      if (!calibjet_background_dijet[ij]) {
        h_calibjet_pt_dijet_eff->Fill(calibjet_pt[ij], jettrig_scale);
        h_calibjet_pt_dijet_effdown->Fill(calibjet_pt[ij], jettrig_scale_down);
        h_calibjet_pt_dijet_effup->Fill(calibjet_pt[ij], jettrig_scale_up);
      }
      if (!calibjet_background_frac[ij]) {
        h_calibjet_pt_frac_eff->Fill(calibjet_pt[ij], jettrig_scale);
        h_calibjet_pt_frac_effdown->Fill(calibjet_pt[ij], jettrig_scale_down);
        h_calibjet_pt_frac_effup->Fill(calibjet_pt[ij], jettrig_scale_up);
      }
    }
  } // event loop end

  // Write histograms.
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();
  h_zvertex->Write();
  h_recojet_pt_record->Write();
  h_recojet_pt_record_dijet->Write();
  h_recojet_pt_record_frac->Write();
  h_calibjet_pt->Write();
  h_calibjet_pt_record->Write();
  h_calibjet_pt_dijet_eff->Write();
  h_calibjet_pt_dijet_effdown->Write();
  h_calibjet_pt_dijet_effup->Write();
  h_calibjet_pt_frac_eff->Write();
  h_calibjet_pt_frac_effdown->Write();
  h_calibjet_pt_frac_effup->Write();
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

int get_hcal_up(int iphi) {
  if (iphi == 63) {
    return 0;
  } else {
    return iphi + 1;
  }
}

int get_hcal_bottom(int iphi) {
  if (iphi == 0) {
    return 63;
  } else {
    return iphi - 1;
  }
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
bool check_bad_trigger(int gl1_trigger_vector_scaled[64]) {
  return gl1_trigger_vector_scaled[18] != 1;
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
    jet_filter.push_back(jet_e->at(ij) < 0 || check_bad_jet_eta(jet_eta->at(ij), zvertex, jet_radius) || jet_pt->at(ij) < 1);
  }
}

void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& calibjet_background_dijet, std::vector<bool>& calibjet_background_frac, std::vector<bool>& jet_filter, std::vector<float>* jet_pt, std::vector<float>* jet_eta, std::vector<float>* jet_phi, std::vector<bool>* jet_background_dijet, std::vector<bool>* jet_background_frac, TF1* f_corr) {
  calibjet_pt.clear();
  calibjet_eta.clear();
  calibjet_phi.clear();
  calibjet_background_dijet.clear();
  calibjet_background_frac.clear();
  for (int ij = 0; ij < jet_filter.size(); ++ij) {
    if (jet_filter.at(ij)) continue;
    double calib_pt = f_corr->Eval(jet_pt->at(ij));
    if (calib_pt < calibptbins[0] || calib_pt > calibptbins[calibnpt]) continue;
    calibjet_pt.push_back(calib_pt);
    calibjet_eta.push_back(jet_eta->at(ij));
    calibjet_phi.push_back(jet_phi->at(ij));
    calibjet_background_dijet.push_back(jet_background_dijet->at(ij));
    calibjet_background_frac.push_back(jet_background_frac->at(ij));
  }
}
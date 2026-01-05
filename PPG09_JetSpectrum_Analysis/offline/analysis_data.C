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
void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& jet_filter, std::vector<float>* jet_pt_calib, std::vector<float>* jet_eta, std::vector<float>* jet_phi, double jet_radius);
////////////////////////////////////////// Main Function //////////////////////////////////////////
void analysis_data(int runnumber, int nseg, int iseg, double jet_radius)  {
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

  float zvertex; chain.SetBranchStatus("z_vertex", 1); chain.SetBranchAddress("z_vertex", &zvertex);
  int gl1_trigger_vector_scaled[64]; chain.SetBranchStatus("gl1_trigger_vector_scaled", 1); chain.SetBranchAddress("gl1_trigger_vector_scaled", gl1_trigger_vector_scaled);
  std::vector<float>* unsubjet_e = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_e", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_e", jet_radius_index), &unsubjet_e);
  std::vector<float>* unsubjet_pt = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_pt", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_pt", jet_radius_index), &unsubjet_pt);
  std::vector<float>* unsubjet_et = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_et", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_et", jet_radius_index), &unsubjet_et);
  std::vector<float>* unsubjet_eta = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_eta", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_eta", jet_radius_index), &unsubjet_eta);
  std::vector<float>* unsubjet_phi = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_phi", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_phi", jet_radius_index), &unsubjet_phi);
  std::vector<float>* unsubjet_time = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_time", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_time", jet_radius_index), &unsubjet_time);
  std::vector<float>* unsubjet_pt_calib = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_pt_calib", jet_radius_index), 1); chain.SetBranchAddress(Form("unsubjet0%d_pt_calib", jet_radius_index), &unsubjet_pt_calib);

  /////////////// Jet Trigger Efficiency ///////////////
  TFile *f_jettrigger = new TFile("output_jetefficiency.root", "READ");
  if (!f_jettrigger) {
    std::cout << "Error: cannot open output_jetefficiency.root" << std::endl;
    return;
  }
  TF1* f_jettrigger_nominal = (TF1*)f_jettrigger->Get(Form("jettrig_0%d_pt_nominal", jet_radius_index));
  TF1* f_jettrigger_up = (TF1*)f_jettrigger->Get(Form("jettrig_0%d_pt_up", jet_radius_index));
  TF1* f_jettrigger_down = (TF1*)f_jettrigger->Get(Form("jettrig_0%d_pt_down", jet_radius_index));
  if (!f_jettrigger_nominal || !f_jettrigger_up || !f_jettrigger_down) {
    std::cout << "Error: cannot open jet trigger functions" << std::endl;
    return;
  }

  /////////////// MBD Trigger Efficiency ///////////////
  TFile *f_mbdtrigger = new TFile("output_mbdefficiency.root", "READ");
  if (!f_mbdtrigger) {
    std::cout << "Error: cannot open output_mbdefficiency.root" << std::endl;
    return;
  }
  TF1* f_mbdtrigger_nominal = (TF1*)f_mbdtrigger->Get(Form("mbdtrig0%d_nominal", jet_radius_index));
  TF1* f_mbdtrigger_up = (TF1*)f_mbdtrigger->Get(Form("mbdtrig0%d_up", jet_radius_index));
  TF1* f_mbdtrigger_down = (TF1*)f_mbdtrigger->Get(Form("mbdtrig0%d_down", jet_radius_index));
  if (!f_mbdtrigger_nominal || !f_mbdtrigger_up || !f_mbdtrigger_down) {
    std::cout << "Error: cannot open mbd trigger functions" << std::endl;
    return;
  }

  /////////////// Histograms ///////////////
  TH1D *h_event_all = new TH1D("h_event_all", ";Event Number", 1, 0, 1);
  TH1D *h_event_beforecut = new TH1D("h_event_beforecut", ";Event Number", 1, 0, 1);
  TH1D *h_event_passed = new TH1D("h_event_passed", ";Event Number", 1, 0, 1);

  TH1D *h_recojet_pt_record_nocut_all = new TH1D("h_recojet_pt_record_nocut_all", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_record_all = new TH1D("h_recojet_pt_record_all", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_record_nocut_zvertex30 = new TH1D("h_recojet_pt_record_nocut_zvertex30", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_record_zvertex30 = new TH1D("h_recojet_pt_record_zvertex30", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_record_nocut_zvertex60 = new TH1D("h_recojet_pt_record_nocut_zvertex60", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_recojet_pt_record_zvertex60 = new TH1D("h_recojet_pt_record_zvertex60", ";p_{T} [GeV]", 1000, 0, 100);

  TH1D *h_calibjet_pt_all = new TH1D("h_calibjet_pt_all", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_all_jetup = new TH1D("h_calibjet_pt_all_jetup", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_all_jetdown = new TH1D("h_calibjet_pt_all_jetdown", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_record_all = new TH1D("h_calibjet_pt_record_all", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_all_jetup = new TH1D("h_calibjet_pt_record_all_jetup", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_all_jetdown = new TH1D("h_calibjet_pt_record_all_jetdown", ";p_{T} [GeV]", 1000, 0, 100);
  
  TH1D *h_calibjet_pt_zvertex30 = new TH1D("h_calibjet_pt_zvertex30", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_zvertex30_jetup = new TH1D("h_calibjet_pt_zvertex30_jetup", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_zvertex30_jetdown = new TH1D("h_calibjet_pt_zvertex30_jetdown", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_zvertex30_mbdup = new TH1D("h_calibjet_pt_zvertex30_mbdup", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_zvertex30_mbddown = new TH1D("h_calibjet_pt_zvertex30_mbddown", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_record_zvertex30 = new TH1D("h_calibjet_pt_record_zvertex30", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_zvertex30_jetup = new TH1D("h_calibjet_pt_record_zvertex30_jetup", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_zvertex30_jetdown = new TH1D("h_calibjet_pt_record_zvertex30_jetdown", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_zvertex30_mbdup = new TH1D("h_calibjet_pt_record_zvertex30_mbdup", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_zvertex30_mbddown = new TH1D("h_calibjet_pt_record_zvertex30_mbddown", ";p_{T} [GeV]", 1000, 0, 100);  

  TH1D *h_calibjet_pt_zvertex60 = new TH1D("h_calibjet_pt_zvertex60", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_zvertex60_jetup = new TH1D("h_calibjet_pt_zvertex60_jetup", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_zvertex60_jetdown = new TH1D("h_calibjet_pt_zvertex60_jetdown", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_zvertex60_mbdup = new TH1D("h_calibjet_pt_zvertex60_mbdup", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_zvertex60_mbddown = new TH1D("h_calibjet_pt_zvertex60_mbddown", ";p_{T} [GeV]", calibnpt, calibptbins);
  TH1D *h_calibjet_pt_record_zvertex60 = new TH1D("h_calibjet_pt_record_zvertex60", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_zvertex60_jetup = new TH1D("h_calibjet_pt_record_zvertex60_jetup", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_zvertex60_jetdown = new TH1D("h_calibjet_pt_record_zvertex60_jetdown", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_zvertex60_mbdup = new TH1D("h_calibjet_pt_record_zvertex60_mbdup", ";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_calibjet_pt_record_zvertex60_mbddown = new TH1D("h_calibjet_pt_record_zvertex60_mbddown", ";p_{T} [GeV]", 1000, 0, 100);

  /////////////// Event Loop ///////////////
  std::cout << "Data analysis started." << std::endl;
  Long64_t n_events = chain.GetEntries();
  std::cout << "Total number of events: " << n_events << std::endl;
  // Event variables setup.
  bool unsubjet_background_dijet, unsubjet_background_njet;
  bool is_zvertex30, is_zvertex60;
  bool is_trigger22;
  std::vector<bool> jet_filter;
  std::vector<float> calibjet_pt, calibjet_eta, calibjet_phi;
  for (Long64_t ie = 0; ie < n_events; ++ie) { // event loop start
    // Load event.
    //if (ie % 1000 == 0) {
    //  std::cout << "Processing event " << ie << "..." << std::endl;
    //}
    chain.GetEntry(ie);

    // Get Z-vertex and trigger check.
    is_trigger22 = (gl1_trigger_vector_scaled[22] == 1);
    if (!is_trigger22) continue;
    is_zvertex30 = (zvertex > -30 && zvertex < 30);
    is_zvertex60 = (zvertex > -60 && zvertex < 60);
    if (zvertex < -900) zvertex = 0;
    
    // Beam background cut. Jet level recording.
    unsubjet_background_dijet = true;
    unsubjet_background_njet = true;
    int leadingunsubjet_index = -1;
    int subleadingunsubjet_index = -1;
    get_leading_subleading_jet(leadingunsubjet_index, subleadingunsubjet_index, unsubjet_e);
    if (leadingunsubjet_index < 0) continue;

    // Dijet cut.
    bool match_dijet = false;
    if (subleadingunsubjet_index >= 0 && unsubjet_e->at(subleadingunsubjet_index) / (float) unsubjet_e->at(leadingunsubjet_index) > 0.3) {
      match_dijet = match_leading_subleading_jet(unsubjet_phi->at(leadingunsubjet_index), unsubjet_phi->at(subleadingunsubjet_index));
      if (fabs(unsubjet_time->at(leadingunsubjet_index)*17.6+2) > 6 || fabs(unsubjet_time->at(leadingunsubjet_index)*17.6 - unsubjet_time->at(subleadingunsubjet_index)*17.6) > 3) {
        match_dijet = false;
      }
    }
    if (match_dijet) unsubjet_background_dijet = false;
    // Njet cut.
    int n_jets = 0;
    for (int ij = 0; ij < unsubjet_e->size(); ++ij) {
      if (unsubjet_e->at(ij) > 5.0) {
        n_jets++;
      }
    }
    if (n_jets <= 9) unsubjet_background_njet = false;

    // Trigger efficiency
    double leadingunsubjet_pt = unsubjet_pt->at(leadingunsubjet_index);

    double jettrigger_nominal = f_jettrigger_nominal->Eval(leadingunsubjet_pt);
    if (jettrigger_nominal < 0.01) jettrigger_nominal = 0.01;
    double jettrig_scale_nominal = 1.0 / jettrigger_nominal;

    double jettrigger_up = f_jettrigger_up->Eval(leadingunsubjet_pt);
    if (jettrigger_up < 0.01) jettrigger_up = 0.01;
    double jettrig_scale_jetup = 1.0 / jettrigger_up;
    
    double jettrigger_down = f_jettrigger_down->Eval(leadingunsubjet_pt);
    if (jettrigger_down < 0.01) jettrigger_down = 0.01;
    double jettrig_scale_jetdown = 1.0 / jettrigger_down;

    double mbdtrigger_nominal = f_mbdtrigger_nominal->Eval(leadingunsubjet_pt);
    if (mbdtrigger_nominal < 0.01) mbdtrigger_nominal = 0.01;
    double mbdtrig_scale_nominal = 1.0 / mbdtrigger_nominal;

    double mbdtrigger_up = f_mbdtrigger_up->Eval(leadingunsubjet_pt);
    if (mbdtrigger_up < 0.01) mbdtrigger_up = 0.01;
    double mbdtrig_scale_mbdup = 1.0 / mbdtrigger_up;

    double mbdtrigger_down = f_mbdtrigger_down->Eval(leadingunsubjet_pt);
    if (mbdtrigger_down < 0.01) mbdtrigger_down = 0.01;
    double mbdtrig_scale_mbddown = 1.0 / mbdtrigger_down;

    double jettiming = 0.95;
    double scale_jettiming = 1.0 / jettiming;

    // Fill histograms.
    get_jet_filter(jet_filter, unsubjet_e, unsubjet_pt, unsubjet_eta, zvertex, jet_radius);

    for (int ij = 0; ij < jet_filter.size(); ++ij) {
      if (jet_filter.at(ij)) continue;

      if (is_trigger22) {
        h_recojet_pt_record_nocut_all->Fill(unsubjet_pt->at(ij));
        if (!unsubjet_background_dijet && !unsubjet_background_njet) h_recojet_pt_record_all->Fill(unsubjet_pt->at(ij));
        }

      if (is_trigger22 && is_zvertex30) {
        h_recojet_pt_record_nocut_zvertex30->Fill(unsubjet_pt->at(ij));
        if (!unsubjet_background_dijet && !unsubjet_background_njet) h_recojet_pt_record_zvertex30->Fill(unsubjet_pt->at(ij));
      }

      if (is_trigger22 && is_zvertex60) {
        h_recojet_pt_record_nocut_zvertex60->Fill(unsubjet_pt->at(ij));
        if (!unsubjet_background_dijet && !unsubjet_background_njet) h_recojet_pt_record_zvertex60->Fill(unsubjet_pt->at(ij));
      }
    }

    get_calibjet(calibjet_pt, calibjet_eta, calibjet_phi, jet_filter, unsubjet_pt_calib, unsubjet_eta, unsubjet_phi, jet_radius);

    for (int ij = 0; ij < calibjet_pt.size(); ++ij) {

      if (is_trigger22) {
        if (!unsubjet_background_dijet && !unsubjet_background_njet) {
          h_calibjet_pt_all->Fill(calibjet_pt[ij], jettrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_all_jetup->Fill(calibjet_pt[ij], jettrig_scale_jetup*scale_jettiming);
          h_calibjet_pt_all_jetdown->Fill(calibjet_pt[ij], jettrig_scale_jetdown*scale_jettiming);
          h_calibjet_pt_record_all->Fill(calibjet_pt[ij], jettrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_record_all_jetup->Fill(calibjet_pt[ij], jettrig_scale_jetup*scale_jettiming);
          h_calibjet_pt_record_all_jetdown->Fill(calibjet_pt[ij], jettrig_scale_jetdown*scale_jettiming);
        }
      }

      if (is_trigger22 && is_zvertex30) {
        if (!unsubjet_background_dijet && !unsubjet_background_njet) {
          h_calibjet_pt_zvertex30->Fill(calibjet_pt[ij], jettrig_scale_nominal*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_zvertex30_jetup->Fill(calibjet_pt[ij], jettrig_scale_jetup*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_zvertex30_jetdown->Fill(calibjet_pt[ij], jettrig_scale_jetdown*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_zvertex30_mbdup->Fill(calibjet_pt[ij], jettrig_scale_nominal*mbdtrig_scale_mbdup*scale_jettiming);
          h_calibjet_pt_zvertex30_mbddown->Fill(calibjet_pt[ij], jettrig_scale_nominal* mbdtrig_scale_mbddown*scale_jettiming);
          h_calibjet_pt_record_zvertex30->Fill(calibjet_pt[ij], jettrig_scale_nominal*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_record_zvertex30_jetup->Fill(calibjet_pt[ij], jettrig_scale_jetup*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_record_zvertex30_jetdown->Fill(calibjet_pt[ij], jettrig_scale_jetdown*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_record_zvertex30_mbdup->Fill(calibjet_pt[ij], jettrig_scale_nominal*mbdtrig_scale_mbdup*scale_jettiming);
          h_calibjet_pt_record_zvertex30_mbddown->Fill(calibjet_pt[ij], jettrig_scale_nominal* mbdtrig_scale_mbddown*scale_jettiming);
        }
      }

      if (is_trigger22 && is_zvertex60) {
        if (!unsubjet_background_dijet && !unsubjet_background_njet) {
          h_calibjet_pt_zvertex60->Fill(calibjet_pt[ij], jettrig_scale_nominal*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_zvertex60_jetup->Fill(calibjet_pt[ij], jettrig_scale_jetup*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_zvertex60_jetdown->Fill(calibjet_pt[ij], jettrig_scale_jetdown*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_zvertex60_mbdup->Fill(calibjet_pt[ij], jettrig_scale_nominal*mbdtrig_scale_mbdup*scale_jettiming);
          h_calibjet_pt_zvertex60_mbddown->Fill(calibjet_pt[ij], jettrig_scale_nominal*mbdtrig_scale_mbddown*scale_jettiming);
          h_calibjet_pt_record_zvertex60->Fill(calibjet_pt[ij], jettrig_scale_nominal*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_record_zvertex60_jetup->Fill(calibjet_pt[ij], jettrig_scale_jetup*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_record_zvertex60_jetdown->Fill(calibjet_pt[ij], jettrig_scale_jetdown*mbdtrig_scale_nominal*scale_jettiming);
          h_calibjet_pt_record_zvertex60_mbdup->Fill(calibjet_pt[ij], jettrig_scale_nominal*mbdtrig_scale_mbdup*scale_jettiming);
          h_calibjet_pt_record_zvertex60_mbddown->Fill(calibjet_pt[ij], jettrig_scale_nominal* mbdtrig_scale_mbddown*scale_jettiming);
        }
      }
    }
  } // event loop end

  // Write histograms.
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();

  h_recojet_pt_record_nocut_all->Write();
  h_recojet_pt_record_all->Write();
  h_recojet_pt_record_nocut_zvertex30->Write();
  h_recojet_pt_record_zvertex30->Write();
  h_recojet_pt_record_nocut_zvertex60->Write();
  h_recojet_pt_record_zvertex60->Write();

  h_calibjet_pt_all->Write();
  h_calibjet_pt_all_jetup->Write();
  h_calibjet_pt_all_jetdown->Write();
  h_calibjet_pt_record_all->Write();
  h_calibjet_pt_record_all_jetup->Write();
  h_calibjet_pt_record_all_jetdown->Write();

  h_calibjet_pt_zvertex30->Write();
  h_calibjet_pt_zvertex30_jetup->Write();
  h_calibjet_pt_zvertex30_jetdown->Write();
  h_calibjet_pt_zvertex30_mbdup->Write();
  h_calibjet_pt_zvertex30_mbddown->Write();
  h_calibjet_pt_record_zvertex30->Write();
  h_calibjet_pt_record_zvertex30_jetup->Write();
  h_calibjet_pt_record_zvertex30_jetdown->Write();
  h_calibjet_pt_record_zvertex30_mbdup->Write();
  h_calibjet_pt_record_zvertex30_mbddown->Write();

  h_calibjet_pt_zvertex60->Write();
  h_calibjet_pt_zvertex60_jetup->Write();
  h_calibjet_pt_zvertex60_jetdown->Write();
  h_calibjet_pt_zvertex60_mbdup->Write();
  h_calibjet_pt_zvertex60_mbddown->Write();
  h_calibjet_pt_record_zvertex60->Write();
  h_calibjet_pt_record_zvertex60_jetup->Write();
  h_calibjet_pt_record_zvertex60_jetdown->Write();
  h_calibjet_pt_record_zvertex60_mbdup->Write();
  h_calibjet_pt_record_zvertex60_mbddown->Write();

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
    jet_filter.push_back(jet_e->at(ij) < 0 || check_bad_jet_eta(jet_eta->at(ij), zvertex, jet_radius) || jet_pt->at(ij) < 1 || jet_eta->at(ij) > 1.1 - jet_radius || jet_eta->at(ij) < -1.1 + jet_radius);
  }
}

void get_calibjet(std::vector<float>& calibjet_pt, std::vector<float>& calibjet_eta, std::vector<float>& calibjet_phi, std::vector<bool>& jet_filter, std::vector<float>* jet_pt_calib, std::vector<float>* jet_eta, std::vector<float>* jet_phi, double jet_radius) {
  calibjet_pt.clear();
  calibjet_eta.clear();
  calibjet_phi.clear();
  for (int ij = 0; ij < jet_filter.size(); ++ij) {
    if (jet_filter.at(ij)) continue;
    double calib_pt = jet_pt_calib->at(ij);
    if (calib_pt < calibptbins[0] || calib_pt > calibptbins[calibnpt]) continue;
    calibjet_pt.push_back(calib_pt);
    calibjet_eta.push_back(jet_eta->at(ij));
    calibjet_phi.push_back(jet_phi->at(ij));
  }
}
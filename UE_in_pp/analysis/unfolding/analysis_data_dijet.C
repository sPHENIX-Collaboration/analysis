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

bool check_bad_trigger(std::vector<int>* gl1_trigger_vector_scaled);
void get_leading_subleading_jet(int& leadingjet_index, int& subleadingjet_index, std::vector<float>* jet_et);
bool match_leading_subleading_jet(float leadingjet_phi, float subleadingjet_phi);

std::vector<int> run_numbers;
std::vector<double> collision_rates;
std::vector<double> pileup_rates;
std::vector<double> pileup_bins = {0.0,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.1,0.2};

void get_pileup_rates(const char* infilename) {
  std::ifstream infile(infilename);
  if (!infile) {
      std::cerr << "Error: Unable to open input file!" << std::endl;
      return;
  }
  std::string line;
  while (std::getline(infile, line)) {
      std::istringstream iss(line);
      int run_number;
      double collision_rate;  
      if (!(iss >> run_number >> collision_rate)) {
          std::cerr << "Error: Malformed line in input file!" << std::endl;
          continue;
      }
      run_numbers.push_back(run_number);
      collision_rates.push_back(collision_rate);

      // Calculate lambda
      double denominator = 78000.0 * 111.0;
      double lambda = collision_rate / denominator;
      double exp_neg_lambda = exp(-lambda);
      
      // Calculate pk
      double pk = 1 - lambda * exp_neg_lambda - exp_neg_lambda;
      
      // Calculate pileup rate
      double pileup_rate = pk / (pk + lambda * exp_neg_lambda);
      pileup_rates.push_back(pileup_rate);
  }
  infile.close();
}

////////////////////////////////////////// Main Function //////////////////////////////////////////
void analysis_data_dijet(int runnumber = 51274, bool clusters = true, bool emcal_clusters = false)  {
  /////////////// General Set up ///////////////
  TFile *f_out = new TFile(Form("analysis_data_run21_output/output_pu_correct_dijet_%d.root", runnumber),"RECREATE");

  /////////////// Read Files ///////////////
  get_pileup_rates("/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/analysis/pileup/0mrad_collision_rates.txt");
  get_pileup_rates("/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/analysis/pileup/updated_1.5mrad_collision_rates.txt");

  const char* baseDirJet = "/sphenix/tg/tg01/jets/egm2153/UEinppOutput";
  TChain chain("T");
  if (runnumber < 51274) {
    chain.Add(Form("%s/output_0mrad_ana468_%d_*.root", baseDirJet, runnumber));
  } else {
    chain.Add(Form("%s/output1.5mrad_ana468_%d_*.root", baseDirJet, runnumber));
  }
  chain.SetBranchStatus("*", 0);

  float zvertex; chain.SetBranchStatus("zvtx", 1); chain.SetBranchAddress("zvtx", &zvertex);
  std::vector<int>* gl1_trigger_vector_scaled = nullptr; chain.SetBranchStatus("triggerVector", 1); chain.SetBranchAddress("triggerVector", &gl1_trigger_vector_scaled);
  std::vector<float>* unsubjet_e = nullptr; chain.SetBranchStatus("e", 1); chain.SetBranchAddress("e", &unsubjet_e);
  std::vector<float>* unsubjet_pt = nullptr; chain.SetBranchStatus("pt", 1); chain.SetBranchAddress("pt", &unsubjet_pt);
  std::vector<float>* unsubjet_eta = nullptr; chain.SetBranchStatus("eta", 1); chain.SetBranchAddress("eta", &unsubjet_eta);
  std::vector<float>* unsubjet_phi = nullptr; chain.SetBranchStatus("phi", 1); chain.SetBranchAddress("phi", &unsubjet_phi);
  std::vector<float>* unsubjet_emcal_calo_e = nullptr; chain.SetBranchStatus("jetEmcalE", 1); chain.SetBranchAddress("jetEmcalE", &unsubjet_emcal_calo_e);
  std::vector<float>* unsubjet_ihcal_calo_e = nullptr; chain.SetBranchStatus("jetIhcalE", 1); chain.SetBranchAddress("jetIhcalE", &unsubjet_ihcal_calo_e);
  std::vector<float>* unsubjet_ohcal_calo_e = nullptr; chain.SetBranchStatus("jetOhcalE", 1); chain.SetBranchAddress("jetOhcalE", &unsubjet_ohcal_calo_e);

  int emcaln = 0; float emcale[24576] = {0.0}; float emcaleta[24576] = {0.0}; float emcalphi[24576] = {0.0};
  int ihcaln = 0; float ihcale[1536] = {0.0}; float ihcaleta[1536] = {0.0}; float ihcalphi[1536] = {0.0};
  int ohcaln = 0; float ohcale[1536] = {0.0}; float ohcaleta[1536] = {0.0}; float ohcalphi[1536] = {0.0};
  int clsmult = 0; float cluster_e[10000] = {0.0}; float cluster_eta[10000] = {0.0}; float cluster_phi[10000] = {0.0};
  if (!clusters) {
      chain.SetBranchStatus("emcaln", 1); chain.SetBranchAddress("emcaln",&emcaln);
      chain.SetBranchStatus("emcale", 1); chain.SetBranchAddress("emcale",emcale);
      chain.SetBranchStatus("emcaleta", 1); chain.SetBranchAddress("emcaleta",emcaleta);
      chain.SetBranchStatus("emcalphi", 1); chain.SetBranchAddress("emcalphi",emcalphi);
      chain.SetBranchStatus("ihcaln", 1); chain.SetBranchAddress("ihcaln",&ihcaln);
      chain.SetBranchStatus("ihcale", 1); chain.SetBranchAddress("ihcale",ihcale);
      chain.SetBranchStatus("ihcaleta", 1); chain.SetBranchAddress("ihcaleta",ihcaleta);
      chain.SetBranchStatus("ihcalphi", 1); chain.SetBranchAddress("ihcalphi",ihcalphi);
      chain.SetBranchStatus("ohcaln", 1); chain.SetBranchAddress("ohcaln",&ohcaln);
      chain.SetBranchStatus("ohcale", 1); chain.SetBranchAddress("ohcale",ohcale);
      chain.SetBranchStatus("ohcaleta", 1); chain.SetBranchAddress("ohcaleta",ohcaleta);
      chain.SetBranchStatus("ohcalphi", 1); chain.SetBranchAddress("ohcalphi",ohcalphi);
  } else if (clusters && !emcal_clusters) {
      chain.SetBranchStatus("clsmult", 1); chain.SetBranchAddress("clsmult",&clsmult);
      chain.SetBranchStatus("cluster_e", 1); chain.SetBranchAddress("cluster_e",cluster_e);
      chain.SetBranchStatus("cluster_eta", 1); chain.SetBranchAddress("cluster_eta",cluster_eta);
      chain.SetBranchStatus("cluster_phi", 1); chain.SetBranchAddress("cluster_phi",cluster_phi);
  } else {
      chain.SetBranchStatus("emcal_clsmult", 1); chain.SetBranchAddress("emcal_clsmult",&clsmult);
      chain.SetBranchStatus("emcal_cluster_e", 1); chain.SetBranchAddress("emcal_cluster_e",cluster_e);
      chain.SetBranchStatus("emcal_cluster_eta", 1); chain.SetBranchAddress("emcal_cluster_eta",cluster_eta);
      chain.SetBranchStatus("emcal_cluster_phi", 1); chain.SetBranchAddress("emcal_cluster_phi",cluster_phi);
  }

  /////////////// JES func ///////////////
  TFile *corrFile = new TFile("JES_Calib_Default.root", "READ");
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
  TH1D* h_deltaphi_record = new TH1D("h_deltaphi_record","",125,-2*M_PI,2*M_PI);
  TH1D* h_xj_record = new TH1D("h_xj_record","",20,0,1);
  TH1D* h_lead_spectra_record = new TH1D("h_lead_spectra_record",";p_{T} [GeV]", 1000, 0, 100);
  TH1D* h_sub_spectra_record = new TH1D("h_sub_spectra_record",";p_{T} [GeV]", 1000, 0, 100);
  TH1D *h_et_transverse_record = new TH1D("h_et_transverse_record", ";#SigmaE_{T} [GeV]", 7000, -20, 50);
  TH2D* h_ue_pt_transverse_record = new TH2D("h_ue_pt_transverse_record","", calibnpt, calibptbins, calibnet, calibetbins);
  TH2D *h_calibjet_pt_dijet_eff = new TH2D("h_calibjet_pt_dijet_eff", ";p_{T} [GeV];#SigmaE_{T} [GeV]", calibnpt, calibptbins, calibnet, calibetbins); // with trigger efficiency + beam background efficiency correction applied
  TH2D *h_calibjet_pt_dijet_effdown = new TH2D("h_calibjet_pt_dijet_effdown", ";p_{T} [GeV];#SigmaE_{T} [GeV]", calibnpt, calibptbins, calibnet, calibetbins); // for trigger efficiency uncertainty
  TH2D *h_calibjet_pt_dijet_effup = new TH2D("h_calibjet_pt_dijet_effup", ";p_{T} [GeV];#SigmaE_{T} [GeV]", calibnpt, calibptbins, calibnet, calibetbins); // for trigger efficiency uncertainty
  TH2D *h_calibjet_pt_dijet_pu_correct_et = new TH2D("h_calibjet_pt_dijet_pu_correct_et", ";p_{T} [GeV];#SigmaE_{T} [GeV]", calibnpt, calibptbins, calibnet, calibetbins); // with pileup correction

  /////////////// Event Loop ///////////////
  std::cout << "Data analysis started." << std::endl;
  int n_events = chain.GetEntries();
  std::cout << "Total number of events: " << n_events << std::endl;

  // Event variables setup.
  bool reco_dijet = false; 

  for (int ie = 0; ie < n_events; ++ie) { // event loop start
  //for (int ie = 0; ie < 20; ++ie) {
    // Load event.
    if (ie % 1000 == 0) {
      std::cout << "Processing event " << ie << "..." << std::endl;
    }
    chain.GetEntry(ie);

    // Trigger and Z-vertex cut.
    if (!check_bad_trigger(gl1_trigger_vector_scaled)) continue;
    if (isnan(zvertex)) { continue; }
    if (fabs(zvertex) > 30) continue;

    //////////////////////////// SETUP JET VARIABLES ////////////////////////////

    // Get leading jet and subleading jet. Do basic jet cuts.
    int leadingunsubjet_index = -1;
    int subleadingunsubjet_index = -1;
    get_leading_subleading_jet(leadingunsubjet_index, subleadingunsubjet_index, unsubjet_pt);
    if (leadingunsubjet_index < 0) continue;

    // Fill z-vertex histogram.
    h_zvertex->Fill(zvertex);

    //////////////////////////// SETUP JET VARIABLES FOR UNFOLDING ////////////////////////////

    // indices to find leading and subleading jets 
    int ind_lead = -1; int ind_sub = -1;
    float lead_e = 0; reco_dijet = false;

    //std::cout << "reco jets " << unsubjet_pt->size() << std::endl;

    TVector3 lead, sub;
    if (unsubjet_pt->size() < 2) {
        continue;
    } else {
        get_leading_subleading_jet(ind_lead, ind_sub, unsubjet_pt);  
        lead.SetPtEtaPhi(unsubjet_pt->at(ind_lead), unsubjet_eta->at(ind_lead), unsubjet_phi->at(ind_lead));
        sub.SetPtEtaPhi(unsubjet_pt->at(ind_sub), unsubjet_eta->at(ind_sub), unsubjet_phi->at(ind_sub));
        lead_e = unsubjet_e->at(ind_lead);
        if (unsubjet_e->at(ind_sub)/unsubjet_e->at(ind_lead) > 0.3 && match_leading_subleading_jet(unsubjet_phi->at(ind_lead), unsubjet_phi->at(ind_sub))) {
            reco_dijet = true;
        } else {
            reco_dijet = false;
        }
    } 
    if (!reco_dijet) { continue; }

    // Trigger efficiency.
    double jettrigeff = f_turnon->Eval(lead.Pt());
    if (jettrigeff < 0.01) jettrigeff = 0.01;
    double jettrig_scale = 1.0 / jettrigeff;

    double jettrigeff_down = f_turnon_down->Eval(lead.Pt());
    if (jettrigeff_down < 0.01) jettrigeff_down = 0.01;
    double jettrig_scale_down = 1.0 / jettrigeff_down;

    double jettrigeff_up = f_turnon_up->Eval(lead.Pt());
    if (jettrigeff_up < 0.01) jettrigeff_up = 0.01;
    double jettrig_scale_up = 1.0 / jettrigeff_up;

    //std::cout << "Reco lead: pt " << lead.Pt() << " e " << lead_e << " eta " << lead.Eta() << " phi " << lead.Phi()  << " dijet " << reco_dijet << " sublead: pt " << sub.Pt() << " eta " << sub.Eta() << " phi " << sub.Phi() << std::endl;

    TVector3 caliblead;
    caliblead.SetPtEtaPhi(f_corr->Eval(lead.Pt()), lead.Eta(), lead.Phi());

    //std::cout << "Good reco lead: pt " << caliblead.Pt() << " eta " << caliblead.Eta() << " phi " << caliblead.Phi() << std::endl;
    
    //////////////////////////// SETUP UE VARIABLES //////////////////////////////

    // find reco ET information 
    float et_transverse = 0;
    if (!clusters) {
        for (int i = 0; i < emcaln; i++) {
            float dphi = get_dphi(lead.Phi(),emcalphi[i]);
            if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { et_transverse += emcale[i]/cosh(emcaleta[i]); } 
        }
        for (int i = 0; i < ihcaln; i++) {
            float dphi = get_dphi(lead.Phi(),ihcalphi[i]);
            if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { et_transverse += ihcale[i]/cosh(ihcaleta[i]); } 
        }
        for (int i = 0; i < ohcaln; i++) {
            float dphi = get_dphi(lead.Phi(),ohcalphi[i]);
            if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { et_transverse += ohcale[i]/cosh(ohcaleta[i]); } 
        }
    } else {
        for (int i = 0; i < clsmult; i++) {
            float dphi = get_dphi(lead.Phi(),cluster_phi[i]);
            if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { et_transverse += cluster_e[i]/cosh(cluster_eta[i]); }
        }
    }

    float pu_correct_et_transverse = et_transverse;
    for (int i = 0; i < run_numbers.size(); i++) {
      if (run_numbers[i] == runnumber) {
        pu_correct_et_transverse -= 0.00378883*100.0*pileup_rates[i];
        break;
      }
    }

    //std::cout << "Reco ET " << et_transverse << std::endl;
    //std::cout << std::endl;

    //////////////////////////// RECORD QA PLOTS FOR ALL EVENTS IN UNFOLDING PROCEDURE ////////////////////////////
    h_deltaphi_record->Fill(lead.DeltaPhi(sub));
    h_xj_record->Fill(sub.Pt()/lead.Pt());
    h_lead_spectra_record->Fill(lead.Pt());
    h_sub_spectra_record->Fill(sub.Pt());
    h_et_transverse_record->Fill(et_transverse);
    h_ue_pt_transverse_record->Fill(lead.Pt(),et_transverse);

    //////////////////////////// FILL HISTOGRAMS FOR UNFOLDING ////////////////////////////
    if (caliblead.Pt() >= calibptbins[0] && caliblead.Pt() <= calibptbins[calibnpt] && et_transverse >= calibetbins[0] && et_transverse <= calibetbins[calibnet]) {
      h_calibjet_pt_dijet_eff->Fill(caliblead.Pt(), et_transverse, jettrig_scale);
      h_calibjet_pt_dijet_effdown->Fill(caliblead.Pt(), et_transverse, jettrig_scale_down);
      h_calibjet_pt_dijet_effup->Fill(caliblead.Pt(), et_transverse, jettrig_scale_up);
    }
    if (caliblead.Pt() >= calibptbins[0] && caliblead.Pt() <= calibptbins[calibnpt] && pu_correct_et_transverse >= calibetbins[0] && pu_correct_et_transverse <= calibetbins[calibnet]) {
      h_calibjet_pt_dijet_pu_correct_et->Fill(caliblead.Pt(), pu_correct_et_transverse, jettrig_scale);
    }
  } // event loop end

  // Write histograms.
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();
  h_zvertex->Write();
  h_deltaphi_record->Write();
  h_xj_record->Write();
  h_lead_spectra_record->Write();
  h_sub_spectra_record->Write();
  h_et_transverse_record->Write();
  h_ue_pt_transverse_record->Write();
  h_calibjet_pt_dijet_eff->Write();
  h_calibjet_pt_dijet_effdown->Write();
  h_calibjet_pt_dijet_effup->Write();
  h_calibjet_pt_dijet_pu_correct_et->Write();
  f_out->Close();
  std::cout << "All done!" << std::endl;
}

////////////////////////////////////////// Functions //////////////////////////////////////////
bool check_bad_trigger(std::vector<int>* gl1_trigger_vector_scaled) {
  if (std::find(gl1_trigger_vector_scaled->begin(), gl1_trigger_vector_scaled->end(), 18) != gl1_trigger_vector_scaled->end()) { return true; }
  if (std::find(gl1_trigger_vector_scaled->begin(), gl1_trigger_vector_scaled->end(), 34) != gl1_trigger_vector_scaled->end()) { return true; }
  return false;
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
  //std::cout << "delta phi: " << dphi << std::endl;
  return dphi > dijet_min_phi;
}
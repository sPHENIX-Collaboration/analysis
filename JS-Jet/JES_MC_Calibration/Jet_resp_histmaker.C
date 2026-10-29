#include "Jet_resp_Def.h"
#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TF1.h>
#include <TMath.h>
#include <TSystem.h>
#include <TString.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

int get_zvertex_bin(float zvertex);
void get_eta_acceptance(float zvertex, float jet_radius, float &minlimit, float &maxlimit);
float get_etafrac_raw(float jet_eta, float zvertex, float jet_radius);
int get_etafrac_bin(float jet_eta, float zvertex, float jet_radius);
double GetZetaCorr(TH2D *h2_corr, float zvertex, float jet_eta, float jet_radius);
double GetZetaCorrNoz(TF1 *f_corr, float jet_eta, float jet_radius);
bool check_efficiency(std::vector<float>* truthjet_pt, std::vector<float>* truthjet_eta, float jet_radius, float truthjet_pt_min, float truthjet_pt_max);
void get_goodrecojet(std::vector<float>& goodrecojet_pt, std::vector<float>& goodrecojet_eta, std::vector<float>& goodrecojet_phi, std::vector<float>& goodrecojet_emfrac, std::vector<float>& goodrecojet_ihfrac, std::vector<float>& goodrecojet_ohfrac, std::vector<float>& goodrecojet_oldcalibpt, std::vector<float>* recojet_pt, std::vector<float>* recojet_eta, std::vector<float>* recojet_phi, std::vector<float>* recojet_e, std::vector<float>* recojet_emcal_frac, std::vector<float>* recojet_ihcal_frac, std::vector<float>* recojet_ohcal_frac, std::vector<float>* recojet_calibpt, float zvertex, float jet_radius);
void get_goodtruthjet(std::vector<float>& goodtruthjet_pt, std::vector<float>& goodtruthjet_eta, std::vector<float>& goodtruthjet_phi, std::vector<float>* truthjet_pt, std::vector<float>* truthjet_eta, std::vector<float>* truthjet_phi, float zvertex, float jet_radius);
void match_meas_truth(std::vector<float>& meas_eta, std::vector<float>& meas_phi, std::vector<float>& meas_matched, std::vector<float>& truth_eta, std::vector<float>& truth_phi, std::vector<float>& truth_matched, float jet_radius, bool has_zvertex);
double GetCalibratedPt(TH2D *h2_inv, double recopt, double emfrac);

////////////////////////////////////////// Main Function //////////////////////////////////////////
// closure_mode: 0 = first pass, 1 = closure test (apply first-pass calibration),
//               2 = full-chain closure (first pass x residual (z, eta) correction)
void Jet_resp_histmaker(int trig = 0, int nseg = 1, int iseg = 0, int closure_mode = 0, int runindex = 28) {
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();
  TH3::SetDefaultSumw2();
  //********** Input Check **********//
  if (trig != 0 && trig != 5 && trig != 12 && trig != 20 && trig != 30 && trig != 40 && trig != 50 && trig != 60 && trig != 80) {
    std::cout << "Error: invalid trigger type: trig = " << trig << std::endl;
    return;
  }

  std::string runtype;
  if (trig == 0) runtype = "MB";
  else if (trig == 5) runtype = "Jet5GeV";
  else if (trig == 12) runtype = "Jet12GeV";
  else if (trig == 20) runtype = "Jet20GeV";
  else if (trig == 30) runtype = "Jet30GeV";
  else if (trig == 40) runtype = "Jet40GeV";
  else if (trig == 50) runtype = "Jet50GeV";
  else if (trig == 60) runtype = "Jet60GeV";
  else if (trig == 80) runtype = "Jet80GeV";
  if (closure_mode < 0 || closure_mode > 2) {
    std::cout << "Error: invalid closure mode: closure_mode = " << closure_mode << std::endl;
    return;
  }
  // Guard against a mismatched Jet_resp_Def.h configuration (installed via ./set_config.sh).
  if (config_firstpass && closure_mode != 0) {
    std::cout << "Error: Jet_resp_Def.h is the FIRST-PASS configuration but closure_mode = " << closure_mode
              << ". Run ./set_config.sh closure first." << std::endl;
    return;
  }
  if (!config_firstpass && closure_mode == 0) {
    std::cout << "Error: Jet_resp_Def.h is the CLOSURE configuration but closure_mode = 0 (first pass)."
              << " Run ./set_config.sh firstpass first." << std::endl;
    return;
  }
  bool do_closure = (closure_mode >= 1);

  // z-vertex reweighting (all productions): per-event weight as a function of the
  // TRUTH z-vertex, matching the simulated vertex shape to the data MBD distribution.
  // The ratio histogram is RUN-SPECIFIC (the denominator is each campaign's own
  // simulated profile: natural for Run 28, flat for Run 38):
  // input_zvertexreweight_run28.root / input_zvertexreweight_run38.root.
  TFile *f_zreweight = nullptr;
  TH1D *h_zvertex_reweight = nullptr;
  {
    TString zrw_name = Form("input_zvertexreweight_run%d.root", runindex);
    f_zreweight = TFile::Open(zrw_name);
    if (!f_zreweight || f_zreweight->IsZombie()) {
      std::cout << "Error: cannot open " << zrw_name << " (required: run-specific z-vertex reweighting)" << std::endl;
      return;
    }
    h_zvertex_reweight = (TH1D*)f_zreweight->Get("h_zvertex_reweight");
    if (!h_zvertex_reweight) {
      std::cout << "Error: cannot find h_zvertex_reweight in " << zrw_name << std::endl;
      return;
    }
  }

  TFile *f_out = new TFile(Form("condor_sim/Run%d_%s/output_%d_%d.root", runindex, runtype.c_str(), nseg, iseg),"RECREATE");

  //********** Histogram Set up **********/
  TH1D *h_zvertex = new TH1D("h_zvertex", "", 2000, -1000, 1000);
  TH3D *h_truthpt_emfrac_ptratio[n_jetradius][n_zvertex];
  TH3D *h_truthpt_emfrac_ptratio_eta[n_jetradius][n_zvertex][n_etafracbin_max];
  for (int ir = 0; ir < n_jetradius; ++ir) {
    int n_eta = get_n_etafracbin(jet_radius_index[ir]);
    for (int iz = 0; iz < n_zvertex; ++iz) {
      h_truthpt_emfrac_ptratio[ir][iz] = new TH3D(Form("h_truthpt_emfrac_ptratio_r0%d_z%d", jet_radius_index[ir], (int)zvertex_index[iz]), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet};p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins, n_ptratiobin, ptratiobins);
      for (int ieta = 0; ieta < n_etafracbin_max; ++ieta) {
        h_truthpt_emfrac_ptratio_eta[ir][iz][ieta] = nullptr;
        if (ieta < n_eta) h_truthpt_emfrac_ptratio_eta[ir][iz][ieta] = new TH3D(Form("h_truthpt_emfrac_ptratio_r0%d_z%d_eta%d", jet_radius_index[ir], (int)zvertex_index[iz], ieta), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet};p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins, n_ptratiobin, ptratiobins);
      }
    }
  }

  // Per-(z, f) weighted sums for the bin-centering correction of the residual
  // (z, eta) analysis: barycenters <z> = sumwz/sumw and <f> = sumwf/sumw per
  // (signed z bin, f bin), plus the no-vertex <f> per f bin. Used by
  // Jet_resp_zeta_interpolation.C as fit-point positions instead of bin centers.
  TH2D *h2_zf_sumw[n_jetradius];
  TH2D *h2_zf_sumwz[n_jetradius];
  TH2D *h2_zf_sumwf[n_jetradius];
  TH1D *h_noz_sumw[n_jetradius];
  TH1D *h_noz_sumwf[n_jetradius];
  for (int ir = 0; ir < n_jetradius; ++ir) {
    int n_eta = get_n_etafracbin(jet_radius_index[ir]);
    h2_zf_sumw[ir] = new TH2D(Form("h2_zf_sumw_r0%d", jet_radius_index[ir]), ";z_{vertex} [cm];#eta fraction f", n_zvertex_phys, zvertex_edges, n_eta, 0, 1);
    h2_zf_sumwz[ir] = new TH2D(Form("h2_zf_sumwz_r0%d", jet_radius_index[ir]), ";z_{vertex} [cm];#eta fraction f", n_zvertex_phys, zvertex_edges, n_eta, 0, 1);
    h2_zf_sumwf[ir] = new TH2D(Form("h2_zf_sumwf_r0%d", jet_radius_index[ir]), ";z_{vertex} [cm];#eta fraction f", n_zvertex_phys, zvertex_edges, n_eta, 0, 1);
    h_noz_sumw[ir] = new TH1D(Form("h_noz_sumw_r0%d", jet_radius_index[ir]), ";#eta fraction f", n_eta, 0, 1);
    h_noz_sumwf[ir] = new TH1D(Form("h_noz_sumwf_r0%d", jet_radius_index[ir]), ";#eta fraction f", n_eta, 0, 1);
  }

  // Per-cell weighted sums for the bin-centering correction: the barycenters
  // <truthpt> = sumwpt/sumw and <EMfrac> = sumwem/sumw give the spectrum-weighted
  // mean position of the entries in each (pT, EMfrac) cell, used by the smoothing
  // fits instead of the geometric bin centers.
  TH2D *h2_sumw[n_jetradius][n_zvertex];
  TH2D *h2_sumwpt[n_jetradius][n_zvertex];
  TH2D *h2_sumwem[n_jetradius][n_zvertex];
  for (int ir = 0; ir < n_jetradius; ++ir) {
    for (int iz = 0; iz < n_zvertex; ++iz) {
      h2_sumw[ir][iz] = new TH2D(Form("h2_sumw_r0%d_z%d", jet_radius_index[ir], (int)zvertex_index[iz]), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins);
      h2_sumwpt[ir][iz] = new TH2D(Form("h2_sumwpt_r0%d_z%d", jet_radius_index[ir], (int)zvertex_index[iz]), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins);
      h2_sumwem[ir][iz] = new TH2D(Form("h2_sumwem_r0%d_z%d", jet_radius_index[ir], (int)zvertex_index[iz]), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins);
    }
  }

  // Validation histograms (closure modes only): unmirrored-f response split by vertex sign
  // for the mirror-symmetry check, and truth-pT response in small/large |z| selections for
  // the JER check.
  TH2D *h_fraw_ptratio_pos[n_jetradius][n_zvertex];
  TH2D *h_fraw_ptratio_neg[n_jetradius][n_zvertex];
  TH2D *h_truthpt_ptratio_z30[n_jetradius];
  TH2D *h_truthpt_ptratio_z3060[n_jetradius];
  TH2D *h_truthpt_ptratio_raw_z30[n_jetradius];   // uncalibrated (raw reco pT) reference
  TH2D *h_truthpt_ptratio_raw_z3060[n_jetradius];
  for (int ir = 0; ir < n_jetradius; ++ir) {
    int n_eta = get_n_etafracbin(jet_radius_index[ir]);
    for (int iz = 0; iz < n_zvertex; ++iz) {
      h_fraw_ptratio_pos[ir][iz] = nullptr;
      h_fraw_ptratio_neg[ir][iz] = nullptr;
      if (iz < n_zvertex - 1) { // physical |z| bins only
        h_fraw_ptratio_pos[ir][iz] = new TH2D(Form("h_fraw_ptratio_r0%d_z%d_pos", jet_radius_index[ir], (int)zvertex_index[iz]), ";f (unmirrored);p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_eta, 0, 1, n_ptratiobin, ptratiobins);
        h_fraw_ptratio_neg[ir][iz] = new TH2D(Form("h_fraw_ptratio_r0%d_z%d_neg", jet_radius_index[ir], (int)zvertex_index[iz]), ";f (unmirrored);p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_eta, 0, 1, n_ptratiobin, ptratiobins);
      }
    }
    h_truthpt_ptratio_z30[ir] = new TH2D(Form("h_truthpt_ptratio_r0%d_z30", jet_radius_index[ir]), ";p_{T}^{truth jet} [GeV];p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_ptratiobin, ptratiobins);
    h_truthpt_ptratio_z3060[ir] = new TH2D(Form("h_truthpt_ptratio_r0%d_z3060", jet_radius_index[ir]), ";p_{T}^{truth jet} [GeV];p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_ptratiobin, ptratiobins);
    h_truthpt_ptratio_raw_z30[ir] = new TH2D(Form("h_truthpt_ptratio_raw_r0%d_z30", jet_radius_index[ir]), ";p_{T}^{truth jet} [GeV];p_{T}^{raw reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_ptratiobin, ptratiobins);
    h_truthpt_ptratio_raw_z3060[ir] = new TH2D(Form("h_truthpt_ptratio_raw_r0%d_z3060", jet_radius_index[ir]), ";p_{T}^{truth jet} [GeV];p_{T}^{raw reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_ptratiobin, ptratiobins);
  }

  TH1D *h_oldcalibpt_truthpt_22_26 = new TH1D("h_oldcalibpt_truthpt_22_26", ";Old calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
  TH1D *h_newcalibpt_truthpt_22_26 = new TH1D("h_newcalibpt_truthpt_22_26", ";New calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
  TH1D *h_oldcalibpt_truthpt_22_26_z30 = new TH1D("h_oldcalibpt_truthpt_22_26_z30", ";Old calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
  TH1D *h_newcalibpt_truthpt_22_26_z30 = new TH1D("h_newcalibpt_truthpt_22_26_z30", ";New calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
  TH1D *h_nontail_eta_22_26_z30 = new TH1D("h_nontail_eta_22_26_z30", ";#eta;N_{jet}", 1000, -5, 5);
  TH1D *h_nontail_deltaz_22_26_z30 = new TH1D("h_nontail_deltaz_22_26_z30", ";z_{vertex}^{reco} - z_{vertex}^{truth} [cm];N_{jet}", 4000, -200, 200);
  TH1D *h_nontail_emfrac_22_26_z30 = new TH1D("h_nontail_emfrac_22_26_z30", ";EMfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_nontail_ihfrac_22_26_z30 = new TH1D("h_nontail_ihfrac_22_26_z30", ";IHfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_nontail_ohfrac_22_26_z30 = new TH1D("h_nontail_ohfrac_22_26_z30", ";OHfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_nontail_oldemfrac_22_26_z30 = new TH1D("h_nontail_oldemfrac_22_26_z30", ";Old EMfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_tail_eta_22_26_z30 = new TH1D("h_tail_eta_22_26_z30", ";#eta;N_{jet}", 1000, -5, 5);
  TH1D *h_tail_deltaz_22_26_z30 = new TH1D("h_tail_deltaz_22_26_z30", ";z_{vertex}^{reco} - z_{vertex}^{truth} [cm];N_{jet}", 4000, -200, 200);
  TH1D *h_tail_emfrac_22_26_z30 = new TH1D("h_tail_emfrac_22_26_z30", ";EMfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_tail_ihfrac_22_26_z30 = new TH1D("h_tail_ihfrac_22_26_z30", ";IHfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_tail_ohfrac_22_26_z30 = new TH1D("h_tail_ohfrac_22_26_z30", ";OHfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_tail_oldemfrac_22_26_z30 = new TH1D("h_tail_oldemfrac_22_26_z30", ";Old EMfrac;N_{jet}", 1500, -0.3, 1.2);

  TH1D *h_oldcalibpt_truthpt_42_48 = new TH1D("h_oldcalibpt_truthpt_42_48", ";Old calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
  TH1D *h_newcalibpt_truthpt_42_48 = new TH1D("h_newcalibpt_truthpt_42_48", ";New calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
  TH1D *h_oldcalibpt_truthpt_42_48_z30 = new TH1D("h_oldcalibpt_truthpt_42_48_z30", ";Old calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
  TH1D *h_newcalibpt_truthpt_42_48_z30 = new TH1D("h_newcalibpt_truthpt_42_48_z30", ";New calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
  TH1D *h_nontail_eta_42_48_z30 = new TH1D("h_nontail_eta_42_48_z30", ";#eta;N_{jet}", 1000, -5, 5);
  TH1D *h_nontail_deltaz_42_48_z30 = new TH1D("h_nontail_deltaz_42_48_z30", ";z_{vertex}^{reco} - z_{vertex}^{truth} [cm];N_{jet}", 4000, -200, 200);
  TH1D *h_nontail_emfrac_42_48_z30 = new TH1D("h_nontail_emfrac_42_48_z30", ";EMfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_nontail_ihfrac_42_48_z30 = new TH1D("h_nontail_ihfrac_42_48_z30", ";IHfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_nontail_ohfrac_42_48_z30 = new TH1D("h_nontail_ohfrac_42_48_z30", ";OHfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_nontail_oldemfrac_42_48_z30 = new TH1D("h_nontail_oldemfrac_42_48_z30", ";Old EMfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_tail_eta_42_48_z30 = new TH1D("h_tail_eta_42_48_z30", ";#eta;N_{jet}", 1000, -5, 5);
  TH1D *h_tail_deltaz_42_48_z30 = new TH1D("h_tail_deltaz_42_48_z30", ";z_{vertex}^{reco} - z_{vertex}^{truth} [cm];N_{jet}", 4000, -200, 200);
  TH1D *h_tail_emfrac_42_48_z30 = new TH1D("h_tail_emfrac_42_48_z30", ";EMfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_tail_ihfrac_42_48_z30 = new TH1D("h_tail_ihfrac_42_48_z30", ";IHfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_tail_ohfrac_42_48_z30 = new TH1D("h_tail_ohfrac_42_48_z30", ";OHfrac;N_{jet}", 1500, -0.3, 1.2);
  TH1D *h_tail_oldemfrac_42_48_z30 = new TH1D("h_tail_oldemfrac_42_48_z30", ";Old EMfrac;N_{jet}", 1500, -0.3, 1.2);

  TH1D *h_nocalibpt_truthpt_42_47 = new TH1D("h_nocalibpt_truthpt_42_47", ";No calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
  TH1D *h_nocalibpt_truthpt_42_47_z30 = new TH1D("h_nocalibpt_truthpt_42_47_z30", ";No calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);

  for (int ir = 0; ir < jet_radius_index.size(); ++ir) {
    // jet data set parameters.
    double jet_radius = jet_radius_index[ir] / 10.;
    const float max_distance = 0.75*jet_radius;
    float truthjet_pt_min, truthjet_pt_max;
    if (trig == 0) {
      if (jet_radius == 0.2) {truthjet_pt_min = 0; truthjet_pt_max = 5;}
      else if (jet_radius == 0.3) {truthjet_pt_min = 0; truthjet_pt_max = 6;}
      else if (jet_radius == 0.4) {truthjet_pt_min = 0; truthjet_pt_max = 7;}
      else if (jet_radius == 0.5) {truthjet_pt_min = 0; truthjet_pt_max = 10;}
      else if (jet_radius == 0.6) {truthjet_pt_min = 0; truthjet_pt_max = 12;}
      else if (jet_radius == 0.7) {truthjet_pt_min = 0; truthjet_pt_max = 14;}
      else if (jet_radius == 0.8) {truthjet_pt_min = 0; truthjet_pt_max = 15;}
    } else if (trig == 5) {
      if (jet_radius == 0.2) {truthjet_pt_min = 5; truthjet_pt_max = 12;}
      else if (jet_radius == 0.3) {truthjet_pt_min = 6; truthjet_pt_max = 13;}
      else if (jet_radius == 0.4) {truthjet_pt_min = 7; truthjet_pt_max = 14;}
      else if (jet_radius == 0.5) {truthjet_pt_min = 10; truthjet_pt_max = 19;}
      else if (jet_radius == 0.6) {truthjet_pt_min = 11; truthjet_pt_max = 22;}
      else if (jet_radius == 0.7) {truthjet_pt_min = 14; truthjet_pt_max = 24;}
      else if (jet_radius == 0.8) {truthjet_pt_min = 15; truthjet_pt_max = 25;}
    } else if (trig == 12) {
      if (jet_radius == 0.2) {truthjet_pt_min = 12; truthjet_pt_max = 20;}
      else if (jet_radius == 0.3) {truthjet_pt_min = 13; truthjet_pt_max = 21;}
      else if (jet_radius == 0.4) {truthjet_pt_min = 14; truthjet_pt_max = 21;}
      else if (jet_radius == 0.5) {truthjet_pt_min = 19; truthjet_pt_max = 27;}
      else if (jet_radius == 0.6) {truthjet_pt_min = 22; truthjet_pt_max = 29;}
      else if (jet_radius == 0.7) {truthjet_pt_min = 24; truthjet_pt_max = 32;}
      else if (jet_radius == 0.8) {truthjet_pt_min = 25; truthjet_pt_max = 34;}
    } else if (trig == 20) {
      if (jet_radius == 0.2) {truthjet_pt_min = 20; truthjet_pt_max = 30;}
      else if (jet_radius == 0.3) {truthjet_pt_min = 21; truthjet_pt_max = 31;}
      else if (jet_radius == 0.4) {truthjet_pt_min = 21; truthjet_pt_max = 32;}
      else if (jet_radius == 0.5) {truthjet_pt_min = 27; truthjet_pt_max = 38;}
      else if (jet_radius == 0.6) {truthjet_pt_min = 29; truthjet_pt_max = 41;}
      else if (jet_radius == 0.7) {truthjet_pt_min = 32; truthjet_pt_max = 45;}
      else if (jet_radius == 0.8) {truthjet_pt_min = 34; truthjet_pt_max = 47;}
    } else if (trig == 30) {
      if (jet_radius == 0.2) {truthjet_pt_min = 30; truthjet_pt_max = 40;}
      else if (jet_radius == 0.3) {truthjet_pt_min = 31; truthjet_pt_max = 41;}
      else if (jet_radius == 0.4) {truthjet_pt_min = 32; truthjet_pt_max = 42;}
      else if (jet_radius == 0.5) {truthjet_pt_min = 38; truthjet_pt_max = 49;}
      else if (jet_radius == 0.6) {truthjet_pt_min = 41; truthjet_pt_max = 53;}
      else if (jet_radius == 0.7) {truthjet_pt_min = 45; truthjet_pt_max = 57;}
      else if (jet_radius == 0.8) {truthjet_pt_min = 47; truthjet_pt_max = 58;}
    } else if (trig == 40) {
      if (jet_radius == 0.2) {truthjet_pt_min = 40; truthjet_pt_max = 50;}
      else if (jet_radius == 0.3) {truthjet_pt_min = 41; truthjet_pt_max = 51;}
      else if (jet_radius == 0.4) {truthjet_pt_min = 42; truthjet_pt_max = 52;}
      else if (jet_radius == 0.5) {truthjet_pt_min = 49; truthjet_pt_max = 58;}
      else if (jet_radius == 0.6) {truthjet_pt_min = 53; truthjet_pt_max = 63;}
      else if (jet_radius == 0.7) {truthjet_pt_min = 57; truthjet_pt_max = 66;}
      else if (jet_radius == 0.8) {truthjet_pt_min = 58; truthjet_pt_max = 68;}
    } else if (trig == 50) {
      if (jet_radius == 0.2) {truthjet_pt_min = 50; truthjet_pt_max = 60;}
      else if (jet_radius == 0.3) {truthjet_pt_min = 51; truthjet_pt_max = 61;}
      else if (jet_radius == 0.4) {truthjet_pt_min = 52; truthjet_pt_max = 62;}
      else if (jet_radius == 0.5) {truthjet_pt_min = 58; truthjet_pt_max = 68;}
      else if (jet_radius == 0.6) {truthjet_pt_min = 63; truthjet_pt_max = 72;}
      else if (jet_radius == 0.7) {truthjet_pt_min = 66; truthjet_pt_max = 75;}
      else if (jet_radius == 0.8) {truthjet_pt_min = 68; truthjet_pt_max = 76;}
    } else if (trig == 60) {
      if (jet_radius == 0.2) {truthjet_pt_min = 60; truthjet_pt_max = 80;}
      else if (jet_radius == 0.3) {truthjet_pt_min = 61; truthjet_pt_max = 81;}
      else if (jet_radius == 0.4) {truthjet_pt_min = 62; truthjet_pt_max = 81;}
      else if (jet_radius == 0.5) {truthjet_pt_min = 68; truthjet_pt_max = 84;}
      else if (jet_radius == 0.6) {truthjet_pt_min = 72; truthjet_pt_max = 86;}
      else if (jet_radius == 0.7) {truthjet_pt_min = 75; truthjet_pt_max = 89;}
      else if (jet_radius == 0.8) {truthjet_pt_min = 76; truthjet_pt_max = 90;}
    } else if (trig == 80) {
      if (jet_radius == 0.2) {truthjet_pt_min = 80; truthjet_pt_max = 3000;}
      else if (jet_radius == 0.3) {truthjet_pt_min = 81; truthjet_pt_max = 3000;}
      else if (jet_radius == 0.4) {truthjet_pt_min = 81; truthjet_pt_max = 3000;}
      else if (jet_radius == 0.5) {truthjet_pt_min = 84; truthjet_pt_max = 3000;}
      else if (jet_radius == 0.6) {truthjet_pt_min = 86; truthjet_pt_max = 3000;}
      else if (jet_radius == 0.7) {truthjet_pt_min = 89; truthjet_pt_max = 3000;}
      else if (jet_radius == 0.8) {truthjet_pt_min = 90; truthjet_pt_max = 3000;}
    } else {
      std::cout << "Error: invalid trigger type: trig = " << trig << std::endl;
      return;
    }

    // Input & Output
    TFile *f_input_calib;
    if (do_closure) {
      f_input_calib = TFile::Open(Form("calib_r0%d.root", jet_radius_index[ir]));
      if (!f_input_calib || f_input_calib->IsZombie()) {
        std::cout << "Error: cannot open input calibration file: calib_r0" << jet_radius_index[ir] << ".root" << std::endl;
        return;
      }
    }
    TH2D *h2_calib;
    if (do_closure) {
      h2_calib = (TH2D*)f_input_calib->Get(Form("h2_jes_r0%d_z0", jet_radius_index[ir]));
      if (!h2_calib) h2_calib = (TH2D*)f_input_calib->Get(Form("h2_jes_r0%d_z1", jet_radius_index[ir])); // legacy first-pass naming
      if (!h2_calib) {
        std::cout << "Error: cannot find histogram h2_jes_r0" << jet_radius_index[ir] << "_z0 (or legacy _z1) in file calib_r0" << jet_radius_index[ir] << ".root" << std::endl;
        f_input_calib->Close();
        return;
      }
    }
    TFile *f_input_zeta = nullptr;
    TH2D *h2_zeta_corr = nullptr;
    if (closure_mode == 2) {
      f_input_zeta = TFile::Open(Form("JES_Calibration_r0%d.root", jet_radius_index[ir]));
      if (!f_input_zeta || f_input_zeta->IsZombie()) {
        std::cout << "Error: cannot open input calibration file: JES_Calibration_r0" << jet_radius_index[ir] << ".root" << std::endl;
        return;
      }
      h2_zeta_corr = (TH2D*)f_input_zeta->Get(Form("h2_zeta_corr_r0%d", jet_radius_index[ir]));
      if (!h2_zeta_corr) {
        std::cout << "Error: cannot find histogram h2_zeta_corr_r0" << jet_radius_index[ir] << " in file JES_Calibration_r0" << jet_radius_index[ir] << ".root" << std::endl;
        f_input_zeta->Close();
        return;
      }
    }
    TF1 *f_zeta_noz_corr = nullptr;
    if (closure_mode == 2) {
      f_zeta_noz_corr = (TF1*)f_input_zeta->Get(Form("f_zeta_noz_corr_r0%d", jet_radius_index[ir])); // optional (older calibration files may not have it)
      if (!f_zeta_noz_corr) std::cout << "Warning: f_zeta_noz_corr_r0" << jet_radius_index[ir] << " not found; no-vertex events get no residual correction." << std::endl;
    }

    // Input trees: the MBD-vertex simulation production
    //   /sphenix/tg/tg01/jets/hanpuj/Run<runindex>_MBDz/single/output_<type>_<index>.root
    // Files that do not exist (failed condor jobs leave gaps in the index sequence) are
    // skipped explicitly rather than handed to TChain, so a missing file cannot silently
    // truncate the chain.
    TChain chain("ttree");
    int n_added = 0;
    for (int i = iseg; i < iseg + nseg; ++i) {
      TString fname = Form("/sphenix/tg/tg01/jets/hanpuj/Run%d_MBDz/single/output_%s_%d.root", runindex, runtype.c_str(), i);
      if (gSystem->AccessPathName(fname)) { // true = does NOT exist
        std::cout << "Warning: input file missing, skipped: " << fname << std::endl;
        continue;
      }
      chain.Add(fname);
      ++n_added;
    }
    std::cout << "Input: " << n_added << " / " << nseg << " files added for " << runtype << ", segments " << iseg << "-" << iseg + nseg - 1 << std::endl;
    chain.SetBranchStatus("*", 0);
  
    float zvertex; chain.SetBranchStatus("z_vertex", 1); chain.SetBranchAddress("z_vertex", &zvertex);
    float truth_z_vertex; chain.SetBranchStatus("truth_z_vertex", 1); chain.SetBranchAddress("truth_z_vertex", &truth_z_vertex);
    std::vector<float>* recojet_e = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_e", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("unsubjet0%d_e", jet_radius_index[ir]), &recojet_e);
    std::vector<float>* recojet_pt = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_pt", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("unsubjet0%d_pt", jet_radius_index[ir]), &recojet_pt);
    std::vector<float>* recojet_eta = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_eta", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("unsubjet0%d_eta", jet_radius_index[ir]), &recojet_eta);
    std::vector<float>* recojet_phi = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_phi", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("unsubjet0%d_phi", jet_radius_index[ir]), &recojet_phi);
    std::vector<float>* recojet_calibpt = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_pt_calib", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("unsubjet0%d_pt_calib", jet_radius_index[ir]), &recojet_calibpt);
    std::vector<float>* recojet_emcal_frac = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_emcal_frac", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("unsubjet0%d_emcal_frac", jet_radius_index[ir]), &recojet_emcal_frac);
    std::vector<float>* recojet_ihcal_frac = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_ihcal_frac", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("unsubjet0%d_ihcal_frac", jet_radius_index[ir]), &recojet_ihcal_frac);
    std::vector<float>* recojet_ohcal_frac = nullptr; chain.SetBranchStatus(Form("unsubjet0%d_ohcal_frac", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("unsubjet0%d_ohcal_frac", jet_radius_index[ir]), &recojet_ohcal_frac);

    std::vector<float>* truthjet_pt = nullptr; chain.SetBranchStatus(Form("truthjet0%d_pt", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("truthjet0%d_pt", jet_radius_index[ir]), &truthjet_pt);
    std::vector<float>* truthjet_eta = nullptr; chain.SetBranchStatus(Form("truthjet0%d_eta", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("truthjet0%d_eta", jet_radius_index[ir]), &truthjet_eta);
    std::vector<float>* truthjet_phi = nullptr; chain.SetBranchStatus(Form("truthjet0%d_phi", jet_radius_index[ir]), 1); chain.SetBranchAddress(Form("truthjet0%d_phi", jet_radius_index[ir]), &truthjet_phi);

    // Data analysis loop.
    std::cout << "Data analysis started." << std::endl;
    int n_events = chain.GetEntries();
    std::cout << "Total number of events: " << n_events << std::endl;
    // Event variables setup.
    std::vector<float> goodrecojet_pt, goodrecojet_eta, goodrecojet_phi, goodrecojet_emfrac, goodrecojet_ihfrac, goodrecojet_ohfrac, goodrecojet_matched, goodrecojet_oldcalibpt, goodrecojet_newcalibpt;
    std::vector<float> goodtruthjet_pt, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched;
    for (int ie = 0; ie < n_events; ++ie) { // event loop start
      // Load event.
      if (ie % 10000 == 0) {
        std::cout << "Processing event " << ie << "..." << std::endl;
      }
      chain.GetEntry(ie);

      // z-vertex reweighting scale (1 for samples without reweighting)
      double scale_zvertexreweight = 1.0;
      if (h_zvertex_reweight) scale_zvertexreweight = h_zvertex_reweight->GetBinContent(h_zvertex_reweight->GetXaxis()->FindBin(truth_z_vertex));

      h_zvertex->Fill(zvertex, scale_zvertexreweight);

      // Leading jet cut.
      if(check_efficiency(truthjet_pt, truthjet_eta, jet_radius, truthjet_pt_min, truthjet_pt_max)) continue;

      bool has_zvertex = 1;
      int i_zvertex_bin = get_zvertex_bin(zvertex);
      if (i_zvertex_bin < 0) continue; // outside the binning (first pass: not |z| <= 30 cm; closure: |z| > 200 cm), skip event
      if (zvertex < -990) {/*No-z events (closure binning: last bin)*/
        zvertex = 0;
        has_zvertex = 0;
      }
  
      // Get good recojet.
      get_goodrecojet(goodrecojet_pt, goodrecojet_eta, goodrecojet_phi, goodrecojet_emfrac, goodrecojet_ihfrac, goodrecojet_ohfrac, goodrecojet_oldcalibpt, recojet_pt, recojet_eta, recojet_phi, recojet_e, recojet_emcal_frac, recojet_ihcal_frac, recojet_ohcal_frac, recojet_calibpt, zvertex, jet_radius);
      get_goodtruthjet(goodtruthjet_pt, goodtruthjet_eta, goodtruthjet_phi, truthjet_pt, truthjet_eta, truthjet_phi, zvertex, jet_radius);
      match_meas_truth(goodrecojet_eta, goodrecojet_phi, goodrecojet_matched, goodtruthjet_eta, goodtruthjet_phi, goodtruthjet_matched, jet_radius, has_zvertex);

      // Get matched jet info.
      for (int tj = 0; tj < goodtruthjet_pt.size(); ++tj) { // truth jet loop start
        int matchjet_index = goodtruthjet_matched[tj];
        if (matchjet_index == -1) continue;
        float matchjet_pt = goodrecojet_pt[matchjet_index];
        if (do_closure) matchjet_pt = GetCalibratedPt(h2_calib, goodrecojet_pt[matchjet_index], goodrecojet_emfrac[matchjet_index]);
        if (closure_mode == 2) {
          if (has_zvertex) matchjet_pt *= GetZetaCorr(h2_zeta_corr, zvertex, goodrecojet_eta[matchjet_index], jet_radius);
          else if (f_zeta_noz_corr) matchjet_pt *= GetZetaCorrNoz(f_zeta_noz_corr, goodrecojet_eta[matchjet_index], jet_radius);
        }
        float matchjet_emfrac = goodrecojet_emfrac[matchjet_index];
        float matchjet_ihfrac = goodrecojet_ihfrac[matchjet_index];
        float matchjet_ohfrac = goodrecojet_ohfrac[matchjet_index];
        float pt_ratio = matchjet_pt / (double)goodtruthjet_pt[tj];

        // Fill histograms.
        //if (zvertex < 0 && goodrecojet_eta[matchjet_index] < 1.25 && goodrecojet_eta[matchjet_index] > 0.7) {
          h_truthpt_emfrac_ptratio[ir][i_zvertex_bin]->Fill(goodtruthjet_pt[tj], matchjet_emfrac, pt_ratio, scale_zvertexreweight);
          h2_sumw[ir][i_zvertex_bin]->Fill(goodtruthjet_pt[tj], matchjet_emfrac, scale_zvertexreweight);
          h2_sumwpt[ir][i_zvertex_bin]->Fill(goodtruthjet_pt[tj], matchjet_emfrac, scale_zvertexreweight * goodtruthjet_pt[tj]);
          h2_sumwem[ir][i_zvertex_bin]->Fill(goodtruthjet_pt[tj], matchjet_emfrac, scale_zvertexreweight * matchjet_emfrac);
          int i_etafrac_bin = get_etafrac_bin(goodrecojet_eta[matchjet_index], zvertex, jet_radius);
          if (i_etafrac_bin >= 0) {
            h_truthpt_emfrac_ptratio_eta[ir][i_zvertex_bin][i_etafrac_bin]->Fill(goodtruthjet_pt[tj], matchjet_emfrac, pt_ratio, scale_zvertexreweight);
            float f_bary = get_etafrac_raw(goodrecojet_eta[matchjet_index], zvertex, jet_radius);
            if (has_zvertex) {
              h2_zf_sumw[ir]->Fill(zvertex, f_bary, scale_zvertexreweight);
              h2_zf_sumwz[ir]->Fill(zvertex, f_bary, scale_zvertexreweight * zvertex);
              h2_zf_sumwf[ir]->Fill(zvertex, f_bary, scale_zvertexreweight * f_bary);
            } else {
              h_noz_sumw[ir]->Fill(f_bary, scale_zvertexreweight);
              h_noz_sumwf[ir]->Fill(f_bary, scale_zvertexreweight * f_bary);
            }
          }
          // Validation fills (closure modes only)
          if (closure_mode >= 1) {
            if (has_zvertex && i_zvertex_bin < n_zvertex - 1) {
              float f_raw = get_etafrac_raw(goodrecojet_eta[matchjet_index], zvertex, jet_radius);
              if (f_raw >= 0) {
                if (zvertex >= 0) h_fraw_ptratio_pos[ir][i_zvertex_bin]->Fill(f_raw, pt_ratio, scale_zvertexreweight);
                else h_fraw_ptratio_neg[ir][i_zvertex_bin]->Fill(f_raw, pt_ratio, scale_zvertexreweight);
              }
            }
            double pt_ratio_raw = goodrecojet_pt[matchjet_index] / (double)goodtruthjet_pt[tj]; // uncalibrated
            if (has_zvertex && TMath::Abs(zvertex) <= 30) { // |z| <= 30 cm
              h_truthpt_ptratio_z30[ir]->Fill(goodtruthjet_pt[tj], pt_ratio, scale_zvertexreweight);
              h_truthpt_ptratio_raw_z30[ir]->Fill(goodtruthjet_pt[tj], pt_ratio_raw, scale_zvertexreweight);
            } else if (has_zvertex && TMath::Abs(zvertex) <= 60) { // 30 < |z| <= 60 cm
              h_truthpt_ptratio_z3060[ir]->Fill(goodtruthjet_pt[tj], pt_ratio, scale_zvertexreweight);
              h_truthpt_ptratio_raw_z3060[ir]->Fill(goodtruthjet_pt[tj], pt_ratio_raw, scale_zvertexreweight);
            }
          }
        //}
      }

      // For test
      if (do_closure) {
        for (int rj = 0; rj < goodrecojet_pt.size(); ++rj) {
          float nocalibpt = goodrecojet_pt[rj];
          float oldcalibpt = goodrecojet_oldcalibpt[rj];
          float newcalibpt = GetCalibratedPt(h2_calib, goodrecojet_pt[rj], goodrecojet_emfrac[rj]);
          int matchtruth_index = goodrecojet_matched[rj];
          if (matchtruth_index == -1) continue;
          float truthpt = goodtruthjet_pt[matchtruth_index];
          if (truthpt >= 22 && truthpt < 26) {
            h_oldcalibpt_truthpt_22_26->Fill(oldcalibpt/(double)truthpt, scale_zvertexreweight);
            h_newcalibpt_truthpt_22_26->Fill(newcalibpt/(double)truthpt, scale_zvertexreweight);
            if (has_zvertex && TMath::Abs(zvertex) <= 30) { // |z| <= 30 cm
              h_oldcalibpt_truthpt_22_26_z30->Fill(oldcalibpt/(double)truthpt, scale_zvertexreweight);
              h_newcalibpt_truthpt_22_26_z30->Fill(newcalibpt/(double)truthpt, scale_zvertexreweight);
              if (newcalibpt/(double)truthpt <= 1.2 && newcalibpt/(double)truthpt > 0.8) {
                h_nontail_eta_22_26_z30->Fill(goodrecojet_eta[rj], scale_zvertexreweight);
                h_nontail_deltaz_22_26_z30->Fill(zvertex - truth_z_vertex, scale_zvertexreweight);
                h_nontail_emfrac_22_26_z30->Fill(goodrecojet_emfrac[rj], scale_zvertexreweight);
                h_nontail_ihfrac_22_26_z30->Fill(goodrecojet_ihfrac[rj], scale_zvertexreweight);
                h_nontail_ohfrac_22_26_z30->Fill(goodrecojet_ohfrac[rj], scale_zvertexreweight);
              } else if (newcalibpt/(double)truthpt > 1.7) {
                h_tail_eta_22_26_z30->Fill(goodrecojet_eta[rj], scale_zvertexreweight);
                h_tail_deltaz_22_26_z30->Fill(zvertex - truth_z_vertex, scale_zvertexreweight);
                h_tail_emfrac_22_26_z30->Fill(goodrecojet_emfrac[rj], scale_zvertexreweight);
                h_tail_ihfrac_22_26_z30->Fill(goodrecojet_ihfrac[rj], scale_zvertexreweight);
                h_tail_ohfrac_22_26_z30->Fill(goodrecojet_ohfrac[rj], scale_zvertexreweight);
              }
            }
          } else if (truthpt >= 42 && truthpt < 48) {
            h_oldcalibpt_truthpt_42_48->Fill(oldcalibpt/(double)truthpt, scale_zvertexreweight);
            h_newcalibpt_truthpt_42_48->Fill(newcalibpt/(double)truthpt, scale_zvertexreweight);
            if (has_zvertex && TMath::Abs(zvertex) <= 30) { // |z| <= 30 cm
              h_oldcalibpt_truthpt_42_48_z30->Fill(oldcalibpt/(double)truthpt, scale_zvertexreweight);
              h_newcalibpt_truthpt_42_48_z30->Fill(newcalibpt/(double)truthpt, scale_zvertexreweight);
              if (newcalibpt/(double)truthpt <= 1.2 && newcalibpt/(double)truthpt > 0.8) {
                h_nontail_eta_42_48_z30->Fill(goodrecojet_eta[rj], scale_zvertexreweight);
                h_nontail_deltaz_42_48_z30->Fill(zvertex - truth_z_vertex, scale_zvertexreweight);
                h_nontail_emfrac_42_48_z30->Fill(goodrecojet_emfrac[rj], scale_zvertexreweight);
                h_nontail_ihfrac_42_48_z30->Fill(goodrecojet_ihfrac[rj], scale_zvertexreweight);
                h_nontail_ohfrac_42_48_z30->Fill(goodrecojet_ohfrac[rj], scale_zvertexreweight);
              } else if (newcalibpt/(double)truthpt > 1.5) {
                h_tail_eta_42_48_z30->Fill(goodrecojet_eta[rj], scale_zvertexreweight);
                h_tail_deltaz_42_48_z30->Fill(zvertex - truth_z_vertex, scale_zvertexreweight);
                h_tail_emfrac_42_48_z30->Fill(goodrecojet_emfrac[rj], scale_zvertexreweight);
                h_tail_ihfrac_42_48_z30->Fill(goodrecojet_ihfrac[rj], scale_zvertexreweight);
                h_tail_ohfrac_42_48_z30->Fill(goodrecojet_ohfrac[rj], scale_zvertexreweight);
              }
            }
          }
          if (truthpt >= 42 && truthpt < 47) {
            h_nocalibpt_truthpt_42_47->Fill(nocalibpt/(double)truthpt, scale_zvertexreweight);
            if (has_zvertex && TMath::Abs(zvertex) <= 30) { // |z| <= 30 cm
              h_nocalibpt_truthpt_42_47_z30->Fill(nocalibpt/(double)truthpt, scale_zvertexreweight);
            }
          }
        }
      }
    } // truth jet loop end
  } // event loop end

  // Write histograms.
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();
  h_zvertex->Write();
  for (int ir = 0; ir < n_jetradius; ++ir) {
    int n_eta = get_n_etafracbin(jet_radius_index[ir]);
    for (int iz = 0; iz < n_zvertex; ++iz) {
      h_truthpt_emfrac_ptratio[ir][iz]->Write();
      h2_sumw[ir][iz]->Write();
      h2_sumwpt[ir][iz]->Write();
      h2_sumwem[ir][iz]->Write();
      for (int ieta = 0; ieta < n_eta; ++ieta) {
        h_truthpt_emfrac_ptratio_eta[ir][iz][ieta]->Write();
      }
    }
    h2_zf_sumw[ir]->Write();
    h2_zf_sumwz[ir]->Write();
    h2_zf_sumwf[ir]->Write();
    h_noz_sumw[ir]->Write();
    h_noz_sumwf[ir]->Write();
  }
  if (closure_mode >= 1) {
    for (int ir = 0; ir < n_jetradius; ++ir) {
      for (int iz = 0; iz < n_zvertex - 1; ++iz) {
        h_fraw_ptratio_pos[ir][iz]->Write();
        h_fraw_ptratio_neg[ir][iz]->Write();
      }
      h_truthpt_ptratio_z30[ir]->Write();
      h_truthpt_ptratio_z3060[ir]->Write();
      h_truthpt_ptratio_raw_z30[ir]->Write();
      h_truthpt_ptratio_raw_z3060[ir]->Write();
    }
  }
  if (do_closure) {
    h_oldcalibpt_truthpt_22_26->Write();
    h_newcalibpt_truthpt_22_26->Write();
    h_oldcalibpt_truthpt_22_26_z30->Write();
    h_newcalibpt_truthpt_22_26_z30->Write();
    h_oldcalibpt_truthpt_42_48->Write();
    h_newcalibpt_truthpt_42_48->Write();
    h_oldcalibpt_truthpt_42_48_z30->Write();
    h_newcalibpt_truthpt_42_48_z30->Write();
    h_tail_eta_22_26_z30->Write();
    h_tail_deltaz_22_26_z30->Write();
    h_tail_emfrac_22_26_z30->Write();
    h_tail_ihfrac_22_26_z30->Write();
    h_tail_ohfrac_22_26_z30->Write();
    h_tail_eta_42_48_z30->Write();
    h_tail_deltaz_42_48_z30->Write();
    h_tail_emfrac_42_48_z30->Write();
    h_tail_ihfrac_42_48_z30->Write();
    h_tail_ohfrac_42_48_z30->Write();
    h_nontail_eta_22_26_z30->Write();
    h_nontail_deltaz_22_26_z30->Write();
    h_nontail_emfrac_22_26_z30->Write();
    h_nontail_ihfrac_22_26_z30->Write();
    h_nontail_ohfrac_22_26_z30->Write();
    h_nontail_eta_42_48_z30->Write();
    h_nontail_deltaz_42_48_z30->Write();
    h_nontail_emfrac_42_48_z30->Write();
    h_nontail_ihfrac_42_48_z30->Write();
    h_nontail_ohfrac_42_48_z30->Write();
    h_nocalibpt_truthpt_42_47->Write();
    h_nocalibpt_truthpt_42_47_z30->Write();
  }
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

void get_eta_acceptance(float zvertex, float jet_radius, float &minlimit, float &maxlimit) {
  float emcal_mineta = get_emcal_mineta_zcorrected(zvertex);
  float emcal_maxeta = get_emcal_maxeta_zcorrected(zvertex);
  float ihcal_mineta = get_ihcal_mineta_zcorrected(zvertex);
  float ihcal_maxeta = get_ihcal_maxeta_zcorrected(zvertex);
  float ohcal_mineta = get_ohcal_mineta_zcorrected(zvertex);
  float ohcal_maxeta = get_ohcal_maxeta_zcorrected(zvertex);
  minlimit = emcal_mineta;
  if (ihcal_mineta > minlimit) minlimit = ihcal_mineta;
  if (ohcal_mineta > minlimit) minlimit = ohcal_mineta;
  maxlimit = emcal_maxeta;
  if (ihcal_maxeta < maxlimit) maxlimit = ihcal_maxeta;
  if (ohcal_maxeta < maxlimit) maxlimit = ohcal_maxeta;
  minlimit += jet_radius;
  maxlimit -= jet_radius;
}

bool check_bad_jet_eta(float jet_eta, float zvertex, float jet_radius) {
  float minlimit, maxlimit;
  get_eta_acceptance(zvertex, jet_radius, minlimit, maxlimit);
  return jet_eta < minlimit || jet_eta > maxlimit;
}

// Unmirrored fractional position of the jet within the z-corrected eta acceptance; -1 if undefined.
float get_etafrac_raw(float jet_eta, float zvertex, float jet_radius) {
  float minlimit, maxlimit;
  get_eta_acceptance(zvertex, jet_radius, minlimit, maxlimit);
  if (maxlimit <= minlimit) return -1;
  float f = (jet_eta - minlimit) / (maxlimit - minlimit);
  if (f < 0 || f > 1) return -1;
  return f;
}

// Fractional position of the jet within the z-corrected eta acceptance, in uniform bins
// whose count depends on the jet radius (get_n_etafracbin). No mirror folding:
// negative and positive z are separate bins and f is used as-is.
int get_etafrac_bin(float jet_eta, float zvertex, float jet_radius) {
  int n_eta = get_n_etafracbin((int)TMath::Nint(jet_radius * 10));
  float f = get_etafrac_raw(jet_eta, zvertex, jet_radius);
  if (f < 0) return -1;
  int ie = (int)(f * n_eta);
  if (ie == n_eta) ie = n_eta - 1; // f exactly at the upper edge
  return ie;
}

int get_zvertex_bin(float zvertex) {
  // Selected by config_firstpass (Jet_resp_Def.h, installed via ./set_config.sh).
  if (config_firstpass) {
    // First pass: a single bin, |z| <= 30 cm; all other events (including
    // no-vertex, encoded as z < -990) are skipped.
    if (TMath::Abs(zvertex) <= 30) return 0;
    return -1;
  }
  // Closure: signed z bins from zvertex_edges; no-z is the last index.
  if (zvertex < -990) return n_zvertex - 1; // no vertex
  if (zvertex < zvertex_edges[0] || zvertex > zvertex_edges[n_zvertex_phys]) return -1; // |z| > 200 cm: excluded
  for (int i = 0; i < n_zvertex_phys; ++i) {
    if (zvertex < zvertex_edges[i+1]) return i;
  }
  return n_zvertex_phys - 1; // z exactly at the upper edge
}

bool check_efficiency(std::vector<float>* truthjet_pt, std::vector<float>* truthjet_eta, float jet_radius, float truthjet_pt_min, float truthjet_pt_max) {
  float leadingjet_pt = -9999;
  for (int tj = 0; tj < truthjet_pt->size(); ++tj) {
    if (truthjet_eta->at(tj) > 1.5 - jet_radius || truthjet_eta->at(tj) < -1.5 + jet_radius) continue;
    if (truthjet_pt->at(tj) > leadingjet_pt) {
      leadingjet_pt = truthjet_pt->at(tj);
    }
  }
  if(leadingjet_pt < truthjet_pt_max && leadingjet_pt >= truthjet_pt_min) return false;
  return true;
}

void get_goodrecojet(std::vector<float>& goodrecojet_pt, std::vector<float>& goodrecojet_eta, std::vector<float>& goodrecojet_phi, std::vector<float>& goodrecojet_emfrac, std::vector<float>& goodrecojet_ihfrac, std::vector<float>& goodrecojet_ohfrac, std::vector<float>& goodrecojet_oldcalibpt, std::vector<float>* recojet_pt, std::vector<float>* recojet_eta, std::vector<float>* recojet_phi, std::vector<float>* recojet_e, std::vector<float>* recojet_emcal_frac, std::vector<float>* recojet_ihcal_frac, std::vector<float>* recojet_ohcal_frac, std::vector<float>* recojet_calibpt, float zvertex, float jet_radius) {
  goodrecojet_pt.clear();
  goodrecojet_eta.clear();
  goodrecojet_phi.clear();
  goodrecojet_emfrac.clear();
  goodrecojet_ihfrac.clear();
  goodrecojet_ohfrac.clear();
  goodrecojet_oldcalibpt.clear();
  for(int rj = 0; rj < recojet_e->size(); ++rj) {
    if (recojet_e->at(rj) < 0 || check_bad_jet_eta(recojet_eta->at(rj), zvertex, jet_radius)) continue;
    goodrecojet_pt.push_back(recojet_pt->at(rj));
    goodrecojet_eta.push_back(recojet_eta->at(rj));
    goodrecojet_phi.push_back(recojet_phi->at(rj));
    goodrecojet_emfrac.push_back(recojet_emcal_frac->at(rj));
    goodrecojet_ihfrac.push_back(recojet_ihcal_frac->at(rj));
    goodrecojet_ohfrac.push_back(recojet_ohcal_frac->at(rj));
    goodrecojet_oldcalibpt.push_back(recojet_calibpt->at(rj));
  }
}

void get_goodtruthjet(std::vector<float>& goodtruthjet_pt, std::vector<float>& goodtruthjet_eta, std::vector<float>& goodtruthjet_phi, std::vector<float>* truthjet_pt, std::vector<float>* truthjet_eta, std::vector<float>* truthjet_phi, float zvertex, float jet_radius) {
  goodtruthjet_pt.clear();
  goodtruthjet_eta.clear();
  goodtruthjet_phi.clear();
  for(int tj = 0; tj < truthjet_pt->size(); ++tj) {
    goodtruthjet_pt.push_back(truthjet_pt->at(tj));
    goodtruthjet_eta.push_back(truthjet_eta->at(tj));
    goodtruthjet_phi.push_back(truthjet_phi->at(tj));
  }
}

void match_meas_truth(std::vector<float>& meas_eta, std::vector<float>& meas_phi, std::vector<float>& meas_matched, std::vector<float>& truth_eta, std::vector<float>& truth_phi, std::vector<float>& truth_matched, float jet_radius, bool has_zvertex) {
  meas_matched.assign(meas_eta.size(), -1);
  truth_matched.assign(truth_eta.size(), -1);
  float max_match_dR = jet_radius * 0.75;
  
  // Build list of all valid pairs with their dR
  std::vector<std::tuple<float, int, int>> pairs;  // (dR, meas_idx, truth_idx)
  
  for (int im = 0; im < meas_eta.size(); ++im) {
    for (int it = 0; it < truth_eta.size(); ++it) {
      float dR;
      if (has_zvertex) dR = get_dR(meas_eta[im], meas_phi[im], truth_eta[it], truth_phi[it]);
      //else dR = get_dR_ellipsoid(meas_eta[im], meas_phi[im], truth_eta[it], truth_phi[it]);
      else dR = fabs(get_dphi(meas_phi[im], truth_phi[it]));

      if (dR < max_match_dR) {
        pairs.push_back(std::make_tuple(dR, im, it));
      }
    }
  }
  
  // Sort by dR (smallest first)
  std::sort(pairs.begin(), pairs.end());
  
  // Greedy matching: assign pairs in order of increasing dR
  for (auto& p : pairs) {
    int im = std::get<1>(p);
    int it = std::get<2>(p);
    
    // Only match if both are still unmatched
    if (meas_matched[im] == -1 && truth_matched[it] == -1) {
      meas_matched[im] = it;
      truth_matched[it] = im;
    }
  }
}

// Residual (z, eta) scale factor from h2_zeta_corr, using the same f convention as
// get_etafrac_bin (signed-z acceptance, no mirror), looked up at the SIGNED z with
// z and f clamped to the histogram's bin-center range.
double GetZetaCorr(TH2D *h2_corr, float zvertex, float jet_eta, float jet_radius) {
  float minlimit, maxlimit;
  get_eta_acceptance(zvertex, jet_radius, minlimit, maxlimit);
  if (maxlimit <= minlimit) return 1.0;
  double f = (jet_eta - minlimit) / (double)(maxlimit - minlimit);
  double z = zvertex; // SIGNED
  TAxis *xa = h2_corr->GetXaxis();
  TAxis *ya = h2_corr->GetYaxis();
  double x_lo = xa->GetBinCenter(1);
  double x_hi = xa->GetBinCenter(xa->GetNbins());
  double y_lo = ya->GetBinCenter(1);
  double y_hi = ya->GetBinCenter(ya->GetNbins());
  if (z < x_lo) z = x_lo;
  if (z > x_hi) z = x_hi;
  if (f < y_lo) f = y_lo;
  if (f > y_hi) f = y_hi;
  return h2_corr->Interpolate(z, f);
}

// Residual scale factor for events without a z-vertex: the stored reciprocal of the
// symmetric-quadratic closure fit (f_zeta_noz_corr), evaluated at the fractional
// acceptance position f computed with z = 0 (no mirror), consistent with how
// no-vertex events are treated everywhere else.
double GetZetaCorrNoz(TF1 *f_corr, float jet_eta, float jet_radius) {
  float f = get_etafrac_raw(jet_eta, 0.0, jet_radius);
  if (f < 0) return 1.0;
  double corr = f_corr->Eval(f);
  if (corr < 0.5 || corr > 2.0) return 1.0; // sanity guard
  return corr;
}

double GetCalibratedPt(TH2D *h2_inv, double recopt, double emfrac) {
  TAxis *xa = h2_inv->GetXaxis();
  TAxis *ya = h2_inv->GetYaxis();

  double x_lo = xa->GetBinCenter(1);
  double x_hi = xa->GetBinCenter(xa->GetNbins());
  double y_lo = ya->GetBinCenter(1);
  double y_hi = ya->GetBinCenter(ya->GetNbins());

  if (recopt < x_lo) recopt = x_lo;
  if (recopt > x_hi) recopt = x_hi;
  if (emfrac < y_lo) emfrac = y_lo;
  if (emfrac > y_hi) emfrac = y_hi;

  return h2_inv->Interpolate(recopt, emfrac);
}
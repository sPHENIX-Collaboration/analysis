#include "AtlasStyle.C"
#include "AtlasUtils.C"
#include "Jet_resp_Def.h"
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// runindex: 28 or 38; the trigger list is selected automatically from it.
void Jet_resp_histcombiner(bool do_closure = false, int runindex = 38) {
  SetAtlasStyle();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  TFile *fout = new TFile("output_combined.root", "RECREATE");

  double mb_cross_section = 4.1970e-2;
  double jet5_cross_section = 1.3878e-4;
  double jet12_cross_section = 1.4903e-6;
  double jet20_cross_section = 6.2623e-8;
  double jet30_cross_section = 2.5298e-9;
  double jet40_cross_section = 1.3553e-10;
  double jet50_cross_section = 7.3113e-12;
  double jet60_cross_section = 3.3261e-13;
  double jet80_cross_section = 1.5795e-16;

  // ********** Input & Output ********** //
  std::vector<int> runindex_array = {runindex};
  std::vector<int> trig_array;

  for (int i = 0; i < runindex_array.size(); ++i) {
    if (runindex_array[i] == 28) {
      trig_array = {0, 5, 12, 20, 30, 40, 50, 60, 80};
    } else if (runindex_array[i] == 38) {
      trig_array = {12, 20, 30, 40, 50, 60, 80};
    }
    // Set histograms.
    TH3D* h_truthpt_emfrac_ptratio_comb[n_jetradius][n_zvertex];
    TH3D* h_truthpt_emfrac_ptratio_eta_comb[n_jetradius][n_zvertex][n_etafracbin_max];
    for (int ir = 0; ir < n_jetradius; ++ir) {
      int n_eta = get_n_etafracbin(jet_radius_index[ir]);
      for (int iz = 0; iz < n_zvertex; ++iz) {
        h_truthpt_emfrac_ptratio_comb[ir][iz] = new TH3D(Form("h_truthpt_emfrac_ptratio_comb_r0%d_z%d", jet_radius_index[ir], (int)zvertex_index[iz]), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet};p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins, n_ptratiobin, ptratiobins);
        for (int ie = 0; ie < n_etafracbin_max; ++ie) {
          h_truthpt_emfrac_ptratio_eta_comb[ir][iz][ie] = nullptr;
          if (ie < n_eta) h_truthpt_emfrac_ptratio_eta_comb[ir][iz][ie] = new TH3D(Form("h_truthpt_emfrac_ptratio_comb_r0%d_z%d_eta%d", jet_radius_index[ir], (int)zvertex_index[iz], ie), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet};p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins, n_ptratiobin, ptratiobins);
        }
      }
    }
    // Per-cell weighted sums for the bin-centering correction (barycenters).
    // Combined with the same weights; skipped if the inputs predate these histograms.
    bool have_bary = true;
    TH2D* h2_sumw_comb[n_jetradius][n_zvertex];
    TH2D* h2_sumwpt_comb[n_jetradius][n_zvertex];
    TH2D* h2_sumwem_comb[n_jetradius][n_zvertex];
    for (int ir = 0; ir < n_jetradius; ++ir) {
      for (int iz = 0; iz < n_zvertex; ++iz) {
        h2_sumw_comb[ir][iz] = new TH2D(Form("h2_sumw_comb_r0%d_z%d", jet_radius_index[ir], (int)zvertex_index[iz]), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins);
        h2_sumwpt_comb[ir][iz] = new TH2D(Form("h2_sumwpt_comb_r0%d_z%d", jet_radius_index[ir], (int)zvertex_index[iz]), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins);
        h2_sumwem_comb[ir][iz] = new TH2D(Form("h2_sumwem_comb_r0%d_z%d", jet_radius_index[ir], (int)zvertex_index[iz]), ";p_{T}^{truth jet} [GeV];E_{EMfrac}^{matched reco jet}", n_truthptbin, truthptbins, n_emfracbin, emfracbins);
      }
    }
    // Per-(z, f) weighted sums for the bin-centering correction of the residual
    // (z, eta) analysis (plus the no-vertex f sums).
    bool have_zfbary = true;
    TH2D* h2_zf_sumw_comb[n_jetradius];
    TH2D* h2_zf_sumwz_comb[n_jetradius];
    TH2D* h2_zf_sumwf_comb[n_jetradius];
    TH1D* h_noz_sumw_comb[n_jetradius];
    TH1D* h_noz_sumwf_comb[n_jetradius];
    for (int ir = 0; ir < n_jetradius; ++ir) {
      int n_eta = get_n_etafracbin(jet_radius_index[ir]);
      h2_zf_sumw_comb[ir] = new TH2D(Form("h2_zf_sumw_comb_r0%d", jet_radius_index[ir]), ";z_{vertex} [cm];#eta fraction f", n_zvertex_phys, zvertex_edges, n_eta, 0, 1);
      h2_zf_sumwz_comb[ir] = new TH2D(Form("h2_zf_sumwz_comb_r0%d", jet_radius_index[ir]), ";z_{vertex} [cm];#eta fraction f", n_zvertex_phys, zvertex_edges, n_eta, 0, 1);
      h2_zf_sumwf_comb[ir] = new TH2D(Form("h2_zf_sumwf_comb_r0%d", jet_radius_index[ir]), ";z_{vertex} [cm];#eta fraction f", n_zvertex_phys, zvertex_edges, n_eta, 0, 1);
      h_noz_sumw_comb[ir] = new TH1D(Form("h_noz_sumw_comb_r0%d", jet_radius_index[ir]), ";#eta fraction f", n_eta, 0, 1);
      h_noz_sumwf_comb[ir] = new TH1D(Form("h_noz_sumwf_comb_r0%d", jet_radius_index[ir]), ";#eta fraction f", n_eta, 0, 1);
    }
    // Validation histograms (closure productions): mirror-symmetry and JER checks.
    TH2D* h_fraw_ptratio_pos_comb[n_jetradius][n_zvertex];
    TH2D* h_fraw_ptratio_neg_comb[n_jetradius][n_zvertex];
    TH2D* h_truthpt_ptratio_z30_comb[n_jetradius];
    TH2D* h_truthpt_ptratio_z3060_comb[n_jetradius];
    TH2D* h_truthpt_ptratio_raw_z30_comb[n_jetradius];
    TH2D* h_truthpt_ptratio_raw_z3060_comb[n_jetradius];
    for (int ir = 0; ir < n_jetradius; ++ir) {
      int n_eta = get_n_etafracbin(jet_radius_index[ir]);
      for (int iz = 0; iz < n_zvertex; ++iz) {
        h_fraw_ptratio_pos_comb[ir][iz] = nullptr;
        h_fraw_ptratio_neg_comb[ir][iz] = nullptr;
        if (iz < n_zvertex - 1) {
          h_fraw_ptratio_pos_comb[ir][iz] = new TH2D(Form("h_fraw_ptratio_comb_r0%d_z%d_pos", jet_radius_index[ir], (int)zvertex_index[iz]), ";f (unmirrored);p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_eta, 0, 1, n_ptratiobin, ptratiobins);
          h_fraw_ptratio_neg_comb[ir][iz] = new TH2D(Form("h_fraw_ptratio_comb_r0%d_z%d_neg", jet_radius_index[ir], (int)zvertex_index[iz]), ";f (unmirrored);p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_eta, 0, 1, n_ptratiobin, ptratiobins);
        }
      }
      h_truthpt_ptratio_z30_comb[ir] = new TH2D(Form("h_truthpt_ptratio_comb_r0%d_z30", jet_radius_index[ir]), ";p_{T}^{truth jet} [GeV];p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_ptratiobin, ptratiobins);
      h_truthpt_ptratio_z3060_comb[ir] = new TH2D(Form("h_truthpt_ptratio_comb_r0%d_z3060", jet_radius_index[ir]), ";p_{T}^{truth jet} [GeV];p_{T}^{matched reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_ptratiobin, ptratiobins);
      h_truthpt_ptratio_raw_z30_comb[ir] = new TH2D(Form("h_truthpt_ptratio_raw_comb_r0%d_z30", jet_radius_index[ir]), ";p_{T}^{truth jet} [GeV];p_{T}^{raw reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_ptratiobin, ptratiobins);
      h_truthpt_ptratio_raw_z3060_comb[ir] = new TH2D(Form("h_truthpt_ptratio_raw_comb_r0%d_z3060", jet_radius_index[ir]), ";p_{T}^{truth jet} [GeV];p_{T}^{raw reco jet} / p_{T}^{truth jet}", n_truthptbin, truthptbins, n_ptratiobin, ptratiobins);
    }
    TH1D *h_oldcalibpt_truthpt_22_26_comb = new TH1D("h_oldcalibpt_truthpt_22_26_comb", ";Old calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
    TH1D *h_newcalibpt_truthpt_22_26_comb = new TH1D("h_newcalibpt_truthpt_22_26_comb", ";New calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
    TH1D *h_oldcalibpt_truthpt_22_26_z30_comb = new TH1D("h_oldcalibpt_truthpt_22_26_z30_comb", ";Old calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
    TH1D *h_newcalibpt_truthpt_22_26_z30_comb = new TH1D("h_newcalibpt_truthpt_22_26_z30_comb", ";New calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
    TH1D *h_oldcalibpt_truthpt_42_48_comb = new TH1D("h_oldcalibpt_truthpt_42_48_comb", ";Old calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
    TH1D *h_newcalibpt_truthpt_42_48_comb = new TH1D("h_newcalibpt_truthpt_42_48_comb", ";New calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
    TH1D *h_oldcalibpt_truthpt_42_48_z30_comb = new TH1D("h_oldcalibpt_truthpt_42_48_z30_comb", ";Old calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
    TH1D *h_newcalibpt_truthpt_42_48_z30_comb = new TH1D("h_newcalibpt_truthpt_42_48_z30_comb", ";New calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);

    TH1D *h_nocalibpt_truthpt_42_47_comb = new TH1D("h_nocalibpt_truthpt_42_47_comb", ";No calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);
    TH1D *h_nocalibpt_truthpt_42_47_z30_comb = new TH1D("h_nocalibpt_truthpt_42_47_z30_comb", ";No calibrated p_{T}^{jet} [GeV];N_{jet}", 1000, 0, 5);

    TH1D *h_nontail_eta_22_26_z30_comb = new TH1D("h_nontail_eta_22_26_z30_comb", ";#eta;N_{jet}", 1000, -5, 5);
    TH1D *h_nontail_deltaz_22_26_z30_comb = new TH1D("h_nontail_deltaz_22_26_z30_comb", ";z_{vertex}^{reco} - z_{vertex}^{truth} [cm];N_{jet}", 4000, -200, 200);
    TH1D *h_nontail_emfrac_22_26_z30_comb = new TH1D("h_nontail_emfrac_22_26_z30_comb", ";EMfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_nontail_ihfrac_22_26_z30_comb = new TH1D("h_nontail_ihfrac_22_26_z30_comb", ";IHfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_nontail_ohfrac_22_26_z30_comb = new TH1D("h_nontail_ohfrac_22_26_z30_comb", ";OHfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_tail_eta_22_26_z30_comb = new TH1D("h_tail_eta_22_26_z30_comb", ";#eta;N_{jet}", 1000, -5, 5);
    TH1D *h_tail_deltaz_22_26_z30_comb = new TH1D("h_tail_deltaz_22_26_z30_comb", ";z_{vertex}^{reco} - z_{vertex}^{truth} [cm];N_{jet}", 4000, -200, 200);
    TH1D *h_tail_emfrac_22_26_z30_comb = new TH1D("h_tail_emfrac_22_26_z30_comb", ";EMfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_tail_ihfrac_22_26_z30_comb = new TH1D("h_tail_ihfrac_22_26_z30_comb", ";IHfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_tail_ohfrac_22_26_z30_comb = new TH1D("h_tail_ohfrac_22_26_z30_comb", ";OHfrac;N_{jet}", 1500, -0.3, 1.2);

    TH1D *h_nontail_eta_42_48_z30_comb = new TH1D("h_nontail_eta_42_48_z30_comb", ";#eta;N_{jet}", 1000, -5, 5);
    TH1D *h_nontail_deltaz_42_48_z30_comb = new TH1D("h_nontail_deltaz_42_48_z30_comb", ";z_{vertex}^{reco} - z_{vertex}^{truth} [cm];N_{jet}", 4000, -200, 200);
    TH1D *h_nontail_emfrac_42_48_z30_comb = new TH1D("h_nontail_emfrac_42_48_z30_comb", ";EMfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_nontail_ihfrac_42_48_z30_comb = new TH1D("h_nontail_ihfrac_42_48_z30_comb", ";IHfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_nontail_ohfrac_42_48_z30_comb = new TH1D("h_nontail_ohfrac_42_48_z30_comb", ";OHfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_tail_eta_42_48_z30_comb = new TH1D("h_tail_eta_42_48_z30_comb", ";#eta;N_{jet}", 1000, -5, 5);
    TH1D *h_tail_deltaz_42_48_z30_comb = new TH1D("h_tail_deltaz_42_48_z30_comb", ";z_{vertex}^{reco} - z_{vertex}^{truth} [cm];N_{jet}", 4000, -200, 200);
    TH1D *h_tail_emfrac_42_48_z30_comb = new TH1D("h_tail_emfrac_42_48_z30_comb", ";EMfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_tail_ihfrac_42_48_z30_comb = new TH1D("h_tail_ihfrac_42_48_z30_comb", ";IHfrac;N_{jet}", 1500, -0.3, 1.2);
    TH1D *h_tail_ohfrac_42_48_z30_comb = new TH1D("h_tail_ohfrac_42_48_z30_comb", ";OHfrac;N_{jet}", 1500, -0.3, 1.2);

    // Get input files.
    for (int k = 0; k < trig_array.size(); ++k) {
      std::string inputfile_kind;
      double cross_section = 0;
      if (trig_array[k] == 0) {
        inputfile_kind = "MB";
        cross_section = mb_cross_section;
      } else if (trig_array[k] == 5) {
        inputfile_kind = "Jet5GeV";
        cross_section = jet5_cross_section;
      } else if (trig_array[k] == 12) {
        inputfile_kind = "Jet12GeV";
        cross_section = jet12_cross_section;
      } else if (trig_array[k] == 20) {
        inputfile_kind = "Jet20GeV";
        cross_section = jet20_cross_section;
      } else if (trig_array[k] == 30) {
        inputfile_kind = "Jet30GeV";
        cross_section = jet30_cross_section;
      } else if (trig_array[k] == 40) {
        inputfile_kind = "Jet40GeV";
        cross_section = jet40_cross_section;
      } else if (trig_array[k] == 50) {
        inputfile_kind = "Jet50GeV";
        cross_section = jet50_cross_section;
      } else if (trig_array[k] == 60) {
        inputfile_kind = "Jet60GeV";
        cross_section = jet60_cross_section;
      } else if (trig_array[k] == 80) {
        inputfile_kind = "Jet80GeV";
        cross_section = jet80_cross_section;
      } else {
        std::cout << "Error: invalid trigger type: trig = " << trig_array[k] << std::endl;
        return;
      }
      TFile *fin = new TFile(Form("condor_sim/output_run%d_%s.root", runindex_array[i], inputfile_kind.c_str()));
      if (!fin) {
        std::cout << "Error: cannot open input file: " << Form("output_run%d_%s.root", runindex_array[i], inputfile_kind.c_str()) << std::endl;
        return;
      }
      TH1D *h_zvertex = (TH1D*)fin->Get("h_zvertex");
      double n_events_zvertex = h_zvertex->GetEntries();
      // Add histograms.
      for (int j = 0; j < jet_radius_index.size(); ++j) {
        for (int iz = 0; iz < n_zvertex; ++iz) {
          TString hname = Form("h_truthpt_emfrac_ptratio_r0%d_z%d", jet_radius_index[j], (int)zvertex_index[iz]);
          TH3D* h_temp = (TH3D*)fin->Get(hname);
          if (!h_temp) {
            std::cout << "Error: cannot find histogram " << hname << " in file " << Form("output_run%d_%s.root", runindex_array[i], inputfile_kind.c_str()) << std::endl;
            return;
          }
          TH2D* h_temp_sumw = (TH2D*)fin->Get(Form("h2_sumw_r0%d_z%d", jet_radius_index[j], (int)zvertex_index[iz]));
          TH2D* h_temp_sumwpt = (TH2D*)fin->Get(Form("h2_sumwpt_r0%d_z%d", jet_radius_index[j], (int)zvertex_index[iz]));
          TH2D* h_temp_sumwem = (TH2D*)fin->Get(Form("h2_sumwem_r0%d_z%d", jet_radius_index[j], (int)zvertex_index[iz]));
          bool bary_here = (h_temp_sumw && h_temp_sumwpt && h_temp_sumwem);
          for (int ix = 1; ix <= h_temp->GetNbinsX(); ++ix) {
            for (int iy = 1; iy <= h_temp->GetNbinsY(); ++iy) {
              double sum_z = 0;
              for (int iz = 1; iz <= h_temp->GetNbinsZ(); ++iz) {
                sum_z += h_temp->GetBinContent(ix, iy, iz);
              }
              if (sum_z < 20.0) {
                for (int iz = 1; iz <= h_temp->GetNbinsZ(); ++iz) {
                  h_temp->SetBinContent(ix, iy, iz, 0.0);
                  h_temp->SetBinError(ix, iy, iz, 0.0);
                }
                if (bary_here) { // keep the barycenter population identical to the response population
                  h_temp_sumw->SetBinContent(ix, iy, 0.0);
                  h_temp_sumw->SetBinError(ix, iy, 0.0);
                  h_temp_sumwpt->SetBinContent(ix, iy, 0.0);
                  h_temp_sumwpt->SetBinError(ix, iy, 0.0);
                  h_temp_sumwem->SetBinContent(ix, iy, 0.0);
                  h_temp_sumwem->SetBinError(ix, iy, 0.0);
                }
              }
            }
          }
          double weight = cross_section / (double)n_events_zvertex;
          h_truthpt_emfrac_ptratio_comb[j][iz]->Add(h_temp, weight);
          delete h_temp;
          if (bary_here) {
            h2_sumw_comb[j][iz]->Add(h_temp_sumw, weight);
            h2_sumwpt_comb[j][iz]->Add(h_temp_sumwpt, weight);
            h2_sumwem_comb[j][iz]->Add(h_temp_sumwem, weight);
          } else {
            have_bary = false;
          }
          if (h_temp_sumw) delete h_temp_sumw;
          if (h_temp_sumwpt) delete h_temp_sumwpt;
          if (h_temp_sumwem) delete h_temp_sumwem;

          for (int ie = 0; ie < get_n_etafracbin(jet_radius_index[j]); ++ie) {
            TString hname_eta = Form("h_truthpt_emfrac_ptratio_r0%d_z%d_eta%d", jet_radius_index[j], (int)zvertex_index[iz], ie);
            TH3D* h_temp_eta = (TH3D*)fin->Get(hname_eta);
            if (!h_temp_eta) {
              std::cout << "Error: cannot find histogram " << hname_eta << " in file " << Form("output_run%d_%s.root", runindex_array[i], inputfile_kind.c_str()) << std::endl;
              return;
            }
            for (int ix = 1; ix <= h_temp_eta->GetNbinsX(); ++ix) {
              for (int iy = 1; iy <= h_temp_eta->GetNbinsY(); ++iy) {
                double sum_z = 0;
                for (int izb = 1; izb <= h_temp_eta->GetNbinsZ(); ++izb) {
                  sum_z += h_temp_eta->GetBinContent(ix, iy, izb);
                }
                if (sum_z < 20.0) {
                  for (int izb = 1; izb <= h_temp_eta->GetNbinsZ(); ++izb) {
                    h_temp_eta->SetBinContent(ix, iy, izb, 0.0);
                    h_temp_eta->SetBinError(ix, iy, izb, 0.0);
                  }
                }
              }
            }
            h_truthpt_emfrac_ptratio_eta_comb[j][iz][ie]->Add(h_temp_eta, weight);
            delete h_temp_eta;
          }
        }
        // (z, f) barycenter sums (one set per radius)
        {
          double weight_zf = cross_section / (double)n_events_zvertex;
          TH2D* h_t_zfw = (TH2D*)fin->Get(Form("h2_zf_sumw_r0%d", jet_radius_index[j]));
          TH2D* h_t_zfz = (TH2D*)fin->Get(Form("h2_zf_sumwz_r0%d", jet_radius_index[j]));
          TH2D* h_t_zff = (TH2D*)fin->Get(Form("h2_zf_sumwf_r0%d", jet_radius_index[j]));
          TH1D* h_t_nozw = (TH1D*)fin->Get(Form("h_noz_sumw_r0%d", jet_radius_index[j]));
          TH1D* h_t_nozf = (TH1D*)fin->Get(Form("h_noz_sumwf_r0%d", jet_radius_index[j]));
          if (h_t_zfw && h_t_zfz && h_t_zff && h_t_nozw && h_t_nozf) {
            h2_zf_sumw_comb[j]->Add(h_t_zfw, weight_zf);
            h2_zf_sumwz_comb[j]->Add(h_t_zfz, weight_zf);
            h2_zf_sumwf_comb[j]->Add(h_t_zff, weight_zf);
            h_noz_sumw_comb[j]->Add(h_t_nozw, weight_zf);
            h_noz_sumwf_comb[j]->Add(h_t_nozf, weight_zf);
          } else {
            have_zfbary = false;
          }
          if (h_t_zfw) delete h_t_zfw;
          if (h_t_zfz) delete h_t_zfz;
          if (h_t_zff) delete h_t_zff;
          if (h_t_nozw) delete h_t_nozw;
          if (h_t_nozf) delete h_t_nozf;
        }
      }
      if (do_closure) {
        // Validation histograms
        double weight_val = cross_section / (double)n_events_zvertex;
        for (int j = 0; j < jet_radius_index.size(); ++j) {
          for (int iz = 0; iz < n_zvertex - 1; ++iz) {
            TH2D* h_temp_pos = (TH2D*)fin->Get(Form("h_fraw_ptratio_r0%d_z%d_pos", jet_radius_index[j], (int)zvertex_index[iz]));
            TH2D* h_temp_neg = (TH2D*)fin->Get(Form("h_fraw_ptratio_r0%d_z%d_neg", jet_radius_index[j], (int)zvertex_index[iz]));
            if (!h_temp_pos || !h_temp_neg) {
              std::cout << "Error: cannot find h_fraw_ptratio_r0" << jet_radius_index[j] << "_z" << (int)zvertex_index[iz] << "_pos/_neg in file " << Form("output_run%d_%s.root", runindex_array[i], inputfile_kind.c_str()) << std::endl;
              return;
            }
            h_fraw_ptratio_pos_comb[j][iz]->Add(h_temp_pos, weight_val);
            h_fraw_ptratio_neg_comb[j][iz]->Add(h_temp_neg, weight_val);
            delete h_temp_pos;
            delete h_temp_neg;
          }
          TH2D* h_temp_z30 = (TH2D*)fin->Get(Form("h_truthpt_ptratio_r0%d_z30", jet_radius_index[j]));
          TH2D* h_temp_z3060 = (TH2D*)fin->Get(Form("h_truthpt_ptratio_r0%d_z3060", jet_radius_index[j]));
          TH2D* h_temp_raw_z30 = (TH2D*)fin->Get(Form("h_truthpt_ptratio_raw_r0%d_z30", jet_radius_index[j]));
          TH2D* h_temp_raw_z3060 = (TH2D*)fin->Get(Form("h_truthpt_ptratio_raw_r0%d_z3060", jet_radius_index[j]));
          if (!h_temp_z30 || !h_temp_z3060 || !h_temp_raw_z30 || !h_temp_raw_z3060) {
            std::cout << "Error: cannot find h_truthpt_ptratio[_raw]_r0" << jet_radius_index[j] << "_z30/_z3060 in file " << Form("output_run%d_%s.root", runindex_array[i], inputfile_kind.c_str()) << std::endl;
            return;
          }
          h_truthpt_ptratio_z30_comb[j]->Add(h_temp_z30, weight_val);
          h_truthpt_ptratio_z3060_comb[j]->Add(h_temp_z3060, weight_val);
          h_truthpt_ptratio_raw_z30_comb[j]->Add(h_temp_raw_z30, weight_val);
          h_truthpt_ptratio_raw_z3060_comb[j]->Add(h_temp_raw_z3060, weight_val);
          delete h_temp_z30;
          delete h_temp_z3060;
          delete h_temp_raw_z30;
          delete h_temp_raw_z3060;
        }

        TH1D* h_oldcalibpt_truthpt_22_26 = (TH1D*)fin->Get("h_oldcalibpt_truthpt_22_26");
        TH1D* h_newcalibpt_truthpt_22_26 = (TH1D*)fin->Get("h_newcalibpt_truthpt_22_26");
        TH1D* h_oldcalibpt_truthpt_22_26_z30 = (TH1D*)fin->Get("h_oldcalibpt_truthpt_22_26_z30");
        TH1D* h_newcalibpt_truthpt_22_26_z30 = (TH1D*)fin->Get("h_newcalibpt_truthpt_22_26_z30");
        TH1D* h_oldcalibpt_truthpt_42_48 = (TH1D*)fin->Get("h_oldcalibpt_truthpt_42_48");
        TH1D* h_newcalibpt_truthpt_42_48 = (TH1D*)fin->Get("h_newcalibpt_truthpt_42_48");
        TH1D* h_oldcalibpt_truthpt_42_48_z30 = (TH1D*)fin->Get("h_oldcalibpt_truthpt_42_48_z30");
        TH1D* h_newcalibpt_truthpt_42_48_z30 = (TH1D*)fin->Get("h_newcalibpt_truthpt_42_48_z30");

        TH1D* h_nocalibpt_truthpt_42_47 = (TH1D*)fin->Get("h_nocalibpt_truthpt_42_47");
        TH1D* h_nocalibpt_truthpt_42_47_z30 = (TH1D*)fin->Get("h_nocalibpt_truthpt_42_47_z30");

        TH1D* h_nontail_eta_22_26_z30 = (TH1D*)fin->Get("h_nontail_eta_22_26_z30");
        TH1D* h_nontail_deltaz_22_26_z30 = (TH1D*)fin->Get("h_nontail_deltaz_22_26_z30");
        TH1D* h_nontail_emfrac_22_26_z30 = (TH1D*)fin->Get("h_nontail_emfrac_22_26_z30");
        TH1D* h_nontail_ihfrac_22_26_z30 = (TH1D*)fin->Get("h_nontail_ihfrac_22_26_z30");
        TH1D* h_nontail_ohfrac_22_26_z30 = (TH1D*)fin->Get("h_nontail_ohfrac_22_26_z30");
        TH1D* h_tail_eta_22_26_z30 = (TH1D*)fin->Get("h_tail_eta_22_26_z30");
        TH1D* h_tail_deltaz_22_26_z30 = (TH1D*)fin->Get("h_tail_deltaz_22_26_z30");
        TH1D* h_tail_emfrac_22_26_z30 = (TH1D*)fin->Get("h_tail_emfrac_22_26_z30");
        TH1D* h_tail_ihfrac_22_26_z30 = (TH1D*)fin->Get("h_tail_ihfrac_22_26_z30");
        TH1D* h_tail_ohfrac_22_26_z30 = (TH1D*)fin->Get("h_tail_ohfrac_22_26_z30");
        TH1D* h_nontail_eta_42_48_z30 = (TH1D*)fin->Get("h_nontail_eta_42_48_z30");
        TH1D* h_nontail_deltaz_42_48_z30 = (TH1D*)fin->Get("h_nontail_deltaz_42_48_z30");
        TH1D* h_nontail_emfrac_42_48_z30 = (TH1D*)fin->Get("h_nontail_emfrac_42_48_z30");
        TH1D* h_nontail_ihfrac_42_48_z30 = (TH1D*)fin->Get("h_nontail_ihfrac_42_48_z30");
        TH1D* h_nontail_ohfrac_42_48_z30 = (TH1D*)fin->Get("h_nontail_ohfrac_42_48_z30");
        TH1D* h_tail_eta_42_48_z30 = (TH1D*)fin->Get("h_tail_eta_42_48_z30");
        TH1D* h_tail_deltaz_42_48_z30 = (TH1D*)fin->Get("h_tail_deltaz_42_48_z30");
        TH1D* h_tail_emfrac_42_48_z30 = (TH1D*)fin->Get("h_tail_emfrac_42_48_z30");
        TH1D* h_tail_ihfrac_42_48_z30 = (TH1D*)fin->Get("h_tail_ihfrac_42_48_z30");
        TH1D* h_tail_ohfrac_42_48_z30 = (TH1D*)fin->Get("h_tail_ohfrac_42_48_z30");

        double weight = cross_section / (double)n_events_zvertex;
        h_oldcalibpt_truthpt_22_26_comb->Add(h_oldcalibpt_truthpt_22_26, weight);
        h_newcalibpt_truthpt_22_26_comb->Add(h_newcalibpt_truthpt_22_26, weight);
        h_oldcalibpt_truthpt_22_26_z30_comb->Add(h_oldcalibpt_truthpt_22_26_z30, weight);
        h_newcalibpt_truthpt_22_26_z30_comb->Add(h_newcalibpt_truthpt_22_26_z30, weight);
        h_oldcalibpt_truthpt_42_48_comb->Add(h_oldcalibpt_truthpt_42_48, weight);
        h_newcalibpt_truthpt_42_48_comb->Add(h_newcalibpt_truthpt_42_48, weight);
        h_oldcalibpt_truthpt_42_48_z30_comb->Add(h_oldcalibpt_truthpt_42_48_z30, weight);
        h_newcalibpt_truthpt_42_48_z30_comb->Add(h_newcalibpt_truthpt_42_48_z30, weight);

        h_nocalibpt_truthpt_42_47_comb->Add(h_nocalibpt_truthpt_42_47, weight);
        h_nocalibpt_truthpt_42_47_z30_comb->Add(h_nocalibpt_truthpt_42_47_z30, weight);

        h_nontail_eta_22_26_z30_comb->Add(h_nontail_eta_22_26_z30, weight);
        h_nontail_deltaz_22_26_z30_comb->Add(h_nontail_deltaz_22_26_z30, weight);
        h_nontail_emfrac_22_26_z30_comb->Add(h_nontail_emfrac_22_26_z30, weight);
        h_nontail_ihfrac_22_26_z30_comb->Add(h_nontail_ihfrac_22_26_z30, weight);
        h_nontail_ohfrac_22_26_z30_comb->Add(h_nontail_ohfrac_22_26_z30, weight);
        h_tail_eta_22_26_z30_comb->Add(h_tail_eta_22_26_z30, weight);
        h_tail_deltaz_22_26_z30_comb->Add(h_tail_deltaz_22_26_z30, weight);
        h_tail_emfrac_22_26_z30_comb->Add(h_tail_emfrac_22_26_z30, weight);
        h_tail_ihfrac_22_26_z30_comb->Add(h_tail_ihfrac_22_26_z30, weight);
        h_tail_ohfrac_22_26_z30_comb->Add(h_tail_ohfrac_22_26_z30, weight);
        h_nontail_eta_42_48_z30_comb->Add(h_nontail_eta_42_48_z30, weight);
        h_nontail_deltaz_42_48_z30_comb->Add(h_nontail_deltaz_42_48_z30, weight);
        h_nontail_emfrac_42_48_z30_comb->Add(h_nontail_emfrac_42_48_z30, weight);
        h_nontail_ihfrac_42_48_z30_comb->Add(h_nontail_ihfrac_42_48_z30, weight);
        h_nontail_ohfrac_42_48_z30_comb->Add(h_nontail_ohfrac_42_48_z30, weight);
        h_tail_eta_42_48_z30_comb->Add(h_tail_eta_42_48_z30, weight);
        h_tail_deltaz_42_48_z30_comb->Add(h_tail_deltaz_42_48_z30, weight);
        h_tail_emfrac_42_48_z30_comb->Add(h_tail_emfrac_42_48_z30, weight);
        h_tail_ihfrac_42_48_z30_comb->Add(h_tail_ihfrac_42_48_z30, weight);
        h_tail_ohfrac_42_48_z30_comb->Add(h_tail_ohfrac_42_48_z30, weight);

        delete h_oldcalibpt_truthpt_22_26;
        delete h_newcalibpt_truthpt_22_26;
        delete h_oldcalibpt_truthpt_22_26_z30;
        delete h_newcalibpt_truthpt_22_26_z30;
        delete h_oldcalibpt_truthpt_42_48;
        delete h_newcalibpt_truthpt_42_48;
        delete h_oldcalibpt_truthpt_42_48_z30;
        delete h_newcalibpt_truthpt_42_48_z30;

        delete h_nocalibpt_truthpt_42_47;
        delete h_nocalibpt_truthpt_42_47_z30;

        delete h_nontail_eta_22_26_z30;
        delete h_nontail_deltaz_22_26_z30;
        delete h_nontail_emfrac_22_26_z30;
        delete h_nontail_ihfrac_22_26_z30;
        delete h_nontail_ohfrac_22_26_z30;
        delete h_tail_eta_22_26_z30;
        delete h_tail_deltaz_22_26_z30;
        delete h_tail_emfrac_22_26_z30;
        delete h_tail_ihfrac_22_26_z30;
        delete h_tail_ohfrac_22_26_z30;
        delete h_nontail_eta_42_48_z30;
        delete h_nontail_deltaz_42_48_z30;
        delete h_nontail_emfrac_42_48_z30;
        delete h_nontail_ihfrac_42_48_z30;
        delete h_nontail_ohfrac_42_48_z30;
        delete h_tail_eta_42_48_z30;
        delete h_tail_deltaz_42_48_z30;
        delete h_tail_emfrac_42_48_z30;
        delete h_tail_ihfrac_42_48_z30;
        delete h_tail_ohfrac_42_48_z30;
      }
      fin->Close();
    } // trig loop ends.

    // ********** Write histograms ********** //
    fout->cd();
    for (int ir = 0; ir < n_jetradius; ++ir) {
      int n_eta = get_n_etafracbin(jet_radius_index[ir]);
      for (int iz = 0; iz < n_zvertex; ++iz) {
        h_truthpt_emfrac_ptratio_comb[ir][iz]->Write();
        for (int ie = 0; ie < n_eta; ++ie) {
          h_truthpt_emfrac_ptratio_eta_comb[ir][iz][ie]->Write();
        }
      }
    }
    if (!have_bary) {
      std::cout << "Note: h2_sumw*/h2_sumwpt*/h2_sumwem* missing in (some) inputs; combined barycenter maps not written (smoothing will use bin centers)." << std::endl;
    }
    if (!have_zfbary) {
      std::cout << "Note: h2_zf_sumw*/h_noz_sumw* missing in (some) inputs; combined (z, f) barycenter maps not written (zeta fits will use bin centers)." << std::endl;
    }
    for (int ir = 0; ir < n_jetradius; ++ir) {
      for (int iz = 0; iz < n_zvertex; ++iz) {
        if (have_bary) {
          h2_sumw_comb[ir][iz]->Write();
          h2_sumwpt_comb[ir][iz]->Write();
          h2_sumwem_comb[ir][iz]->Write();
        }
      }
      if (have_zfbary) {
        h2_zf_sumw_comb[ir]->Write();
        h2_zf_sumwz_comb[ir]->Write();
        h2_zf_sumwf_comb[ir]->Write();
        h_noz_sumw_comb[ir]->Write();
        h_noz_sumwf_comb[ir]->Write();
      }
    }
    for (int ir = 0; ir < n_jetradius; ++ir) {
      int n_eta = get_n_etafracbin(jet_radius_index[ir]);
      for (int iz = 0; iz < n_zvertex; ++iz) {
        delete h_truthpt_emfrac_ptratio_comb[ir][iz];
        delete h2_sumw_comb[ir][iz];
        delete h2_sumwpt_comb[ir][iz];
        delete h2_sumwem_comb[ir][iz];
        for (int ie = 0; ie < n_eta; ++ie) {
          delete h_truthpt_emfrac_ptratio_eta_comb[ir][iz][ie];
        }
      }
      delete h2_zf_sumw_comb[ir];
      delete h2_zf_sumwz_comb[ir];
      delete h2_zf_sumwf_comb[ir];
      delete h_noz_sumw_comb[ir];
      delete h_noz_sumwf_comb[ir];
    }
    if (do_closure) {
      for (int ir = 0; ir < n_jetradius; ++ir) {
        for (int iz = 0; iz < n_zvertex - 1; ++iz) {
          h_fraw_ptratio_pos_comb[ir][iz]->Write();
          h_fraw_ptratio_neg_comb[ir][iz]->Write();
        }
        h_truthpt_ptratio_z30_comb[ir]->Write();
        h_truthpt_ptratio_z3060_comb[ir]->Write();
        h_truthpt_ptratio_raw_z30_comb[ir]->Write();
        h_truthpt_ptratio_raw_z3060_comb[ir]->Write();
      }
      h_oldcalibpt_truthpt_22_26_comb->Write();
      h_newcalibpt_truthpt_22_26_comb->Write();
      h_oldcalibpt_truthpt_22_26_z30_comb->Write();
      h_newcalibpt_truthpt_22_26_z30_comb->Write();
      h_oldcalibpt_truthpt_42_48_comb->Write();
      h_newcalibpt_truthpt_42_48_comb->Write();
      h_oldcalibpt_truthpt_42_48_z30_comb->Write();
      h_newcalibpt_truthpt_42_48_z30_comb->Write();

      h_nocalibpt_truthpt_42_47_comb->Write();
      h_nocalibpt_truthpt_42_47_z30_comb->Write();

      h_nontail_eta_22_26_z30_comb->Write();
      h_nontail_deltaz_22_26_z30_comb->Write();
      h_nontail_emfrac_22_26_z30_comb->Write();
      h_nontail_ihfrac_22_26_z30_comb->Write();
      h_nontail_ohfrac_22_26_z30_comb->Write();
      h_tail_eta_22_26_z30_comb->Write();
      h_tail_deltaz_22_26_z30_comb->Write();
      h_tail_emfrac_22_26_z30_comb->Write();
      h_tail_ihfrac_22_26_z30_comb->Write();
      h_tail_ohfrac_22_26_z30_comb->Write();
      h_nontail_eta_42_48_z30_comb->Write();
      h_nontail_deltaz_42_48_z30_comb->Write();
      h_nontail_emfrac_42_48_z30_comb->Write();
      h_nontail_ihfrac_42_48_z30_comb->Write();
      h_nontail_ohfrac_42_48_z30_comb->Write();
      h_tail_eta_42_48_z30_comb->Write();
      h_tail_deltaz_42_48_z30_comb->Write();
      h_tail_emfrac_42_48_z30_comb->Write();
      h_tail_ihfrac_42_48_z30_comb->Write();
      h_tail_ohfrac_42_48_z30_comb->Write();

      for (int ir = 0; ir < n_jetradius; ++ir) {
        for (int iz = 0; iz < n_zvertex - 1; ++iz) {
          delete h_fraw_ptratio_pos_comb[ir][iz];
          delete h_fraw_ptratio_neg_comb[ir][iz];
        }
        delete h_truthpt_ptratio_z30_comb[ir];
        delete h_truthpt_ptratio_z3060_comb[ir];
        delete h_truthpt_ptratio_raw_z30_comb[ir];
        delete h_truthpt_ptratio_raw_z3060_comb[ir];
      }
      delete h_oldcalibpt_truthpt_22_26_comb;
      delete h_newcalibpt_truthpt_22_26_comb;
      delete h_oldcalibpt_truthpt_22_26_z30_comb;
      delete h_newcalibpt_truthpt_22_26_z30_comb;
      delete h_oldcalibpt_truthpt_42_48_comb;
      delete h_newcalibpt_truthpt_42_48_comb;
      delete h_oldcalibpt_truthpt_42_48_z30_comb;
      delete h_newcalibpt_truthpt_42_48_z30_comb;

      delete h_nocalibpt_truthpt_42_47_comb;
      delete h_nocalibpt_truthpt_42_47_z30_comb;

      delete h_nontail_eta_22_26_z30_comb;
      delete h_nontail_deltaz_22_26_z30_comb;
      delete h_nontail_emfrac_22_26_z30_comb;
      delete h_nontail_ihfrac_22_26_z30_comb;
      delete h_nontail_ohfrac_22_26_z30_comb;
      delete h_tail_eta_22_26_z30_comb;
      delete h_tail_deltaz_22_26_z30_comb;
      delete h_tail_emfrac_22_26_z30_comb;
      delete h_tail_ihfrac_22_26_z30_comb;
      delete h_tail_ohfrac_22_26_z30_comb;
      delete h_nontail_eta_42_48_z30_comb;
      delete h_nontail_deltaz_42_48_z30_comb;
      delete h_nontail_emfrac_42_48_z30_comb;
      delete h_nontail_ihfrac_42_48_z30_comb;
      delete h_nontail_ohfrac_42_48_z30_comb;
      delete h_tail_eta_42_48_z30_comb;
      delete h_tail_deltaz_42_48_z30_comb;
      delete h_tail_emfrac_42_48_z30_comb;
      delete h_tail_ihfrac_42_48_z30_comb;
      delete h_tail_ohfrac_42_48_z30_comb;
    }
  } // runindex loop ends.
  fout->Close();
}
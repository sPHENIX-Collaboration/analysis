#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TString.h>
#include <TMath.h>
#include <iostream>
#include <vector>

#include "Jet_resp_Def.h"

// ============================================================================
// Validation test 1: full-chain closure.
//
// Run AFTER the closure_mode = 2 production (first pass x h2_zeta_corr applied)
// has been processed through hadd -> histcombiner -> do_analysis.sh, so that the
// current output_gaus_r0R_zZ_etaE.root files contain the fully corrected response.
//
// The "before" reference (first pass only) is read from zetadependence_r0R.root
// (h2_zeta_mean_raw_r0R), which must be the file saved from the closure_mode = 1
// analysis -- do NOT rerun Jet_resp_zeta_interpolation.C on the mode-2 output
// before running this macro.
//
// Output: figure_dependence/fullclosure_r0R_vs_f.png (per-z grid, before/after),
//         figure_dependence/fullclosure_r0R_vs_z.png (after, per f bin),
//         printed maximum deviation from unity.
// ============================================================================

// Gaussian mean of the h2_mean bin-content distribution (same gates as
// Jet_resp_zeta_interpolation.C). Returns 0 good fit, 1 raw-mean fallback, -1 rejected.
int fullclosure_gausmean(TH2D* h2, TH2D* h2_chi2, double &mean, double &meanerr) {
  TH1D h1("h1_tmp", "", 120, 0.7, 1.3);
  for (int ix = 1; ix <= h2->GetNbinsX(); ++ix) {
    for (int iy = 1; iy <= h2->GetNbinsY(); ++iy) {
      double v = h2->GetBinContent(ix, iy);
      if (v == 0) continue;
      if (h2_chi2 && h2_chi2->GetBinContent(ix, iy) < 0) continue; // skip failed-fit cells (raw mean/RMS fallback)
      h1.Fill(v);
    }
  }
  if (h1.GetEntries() < 8) return -1;
  double seed_mean = h1.GetMean();
  double seed_rms  = (h1.GetRMS() > 0) ? h1.GetRMS() : 0.05;
  TF1 f("f_tmp", "gaus", 0.7, 1.3);
  f.SetParameters(h1.GetMaximum(), seed_mean, seed_rms);
  h1.Rebin(2);
  double half_width = TMath::Max(0.05, 1.5 * seed_rms); // fit window: mean +/- 1.5 sigma, floored at +/- 0.05
  h1.Fit(&f, "Q R N", "", TMath::Max(0.7, seed_mean - half_width), TMath::Min(1.3, seed_mean + half_width));
  mean = f.GetParameter(1);
  meanerr = f.GetParError(1);
  double sigma = f.GetParameter(2);
  double chi2ndf = (f.GetNDF() > 0) ? f.GetChisquare() / (double)f.GetNDF() : -1;
  if (TMath::Abs(mean - 1.0) < 0.2 && sigma > 0 && sigma < 0.15 && meanerr > 0 && meanerr < 0.02 && chi2ndf < 10) return 0;
  mean = h1.GetMean();
  meanerr = h1.GetMeanError();
  if (TMath::Abs(mean - 1.0) < 0.2 && meanerr > 0 && meanerr < 0.03) return 1;
  return -1;
}

void draw_fullclosure(int r_val = 4) {
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  const char* figdir = "figure_dependence";
  gSystem->mkdir(figdir, kTRUE);

  const int n_eta = get_n_etafracbin(r_val);
  int eta_color[n_etafracbin_max] = {kBlack, kBlue+1, kGreen+2, kRed+1, kOrange+7, kMagenta+1};

  const int n_zphys = n_zvertex_phys; // signed z bins, edges from Jet_resp_Def.h
  double *z_edge = zvertex_edges;

  // "Before" reference from the mode-1 analysis
  TFile *f_before = TFile::Open(Form("zetadependence_r0%d.root", r_val), "READ");
  TH2D *h2_before = nullptr;
  if (f_before && !f_before->IsZombie()) {
    h2_before = (TH2D*)f_before->Get(Form("h2_zeta_mean_raw_r0%d", r_val));
  }
  if (!h2_before) {
    std::cerr << "Warning: no before-correction reference (zetadependence_r0" << r_val
              << ".root / h2_zeta_mean_raw); plotting after-correction only." << std::endl;
  }

  // "After" means from the current (mode-2) output_gaus files
  double after_mu[n_zphys][n_etafracbin_max];
  double after_err[n_zphys][n_etafracbin_max];
  bool   after_ok[n_zphys][n_etafracbin_max];
  double max_dev = 0;
  int max_dev_iz = -1, max_dev_ie = -1;

  for (int iz = 0; iz < n_zphys; ++iz) {
    for (int ie = 0; ie < n_eta; ++ie) {
      after_ok[iz][ie] = false;
      TString fname = Form("output_gaus/output_gaus_r0%d_z%d_eta%d.root", r_val, iz, ie);
      TFile *fin = TFile::Open(fname, "READ");
      if (!fin || fin->IsZombie()) {
        std::cerr << "Cannot open " << fname << std::endl;
        continue;
      }
      TH2D *h2 = (TH2D*)fin->Get(Form("h2_mean_r0%d_z%d_eta%d", r_val, iz, ie));
      TH2D *h2c = (TH2D*)fin->Get(Form("h2_chi2_r0%d_z%d_eta%d", r_val, iz, ie)); // may be null for old files
      if (h2) {
        double m, me;
        if (fullclosure_gausmean(h2, h2c, m, me) >= 0) {
          after_mu[iz][ie] = m;
          after_err[iz][ie] = me;
          after_ok[iz][ie] = true;
          if (TMath::Abs(m - 1.0) > max_dev) {
            max_dev = TMath::Abs(m - 1.0);
            max_dev_iz = iz;
            max_dev_ie = ie;
          }
        }
      }
      fin->Close();
      delete fin;
    }
  }

  // ---------- Canvas A: per-z grid, mean vs f, before vs after ----------
  TCanvas *c_f = new TCanvas("c_fullclosure_vs_f", "", 1600, 1200);
  c_f->Divide(4, (n_zphys + 3) / 4);
  std::vector<TGraphErrors*> keep_graphs;
  for (int iz = 0; iz < n_zphys; ++iz) {
    c_f->cd(iz+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.15);

    // Frame
    TH1D *frame = new TH1D(Form("frame_z%d", iz), ";#eta fraction f;<p_{T}^{calib} / p_{T}^{truth}>", 1, 0, 1);
    frame->SetDirectory(0);
    frame->GetYaxis()->SetRangeUser(0.9, 1.1);
    frame->Draw("AXIS");

    TLine *l1 = new TLine(0, 1.0, 1, 1.0);
    l1->SetLineColor(kGray+2);
    l1->SetLineStyle(2);
    l1->Draw("same");
    TLine *lu = new TLine(0, 1.01, 1, 1.01);
    lu->SetLineColor(kGray+1);
    lu->SetLineStyle(3);
    lu->Draw("same");
    TLine *ld = new TLine(0, 0.99, 1, 0.99);
    ld->SetLineColor(kGray+1);
    ld->SetLineStyle(3);
    ld->Draw("same");

    // Before (open blue)
    if (h2_before) {
      std::vector<double> gx, gy, gxe, gye;
      for (int ie = 0; ie < n_eta; ++ie) {
        double v = h2_before->GetBinContent(iz+1, ie+1);
        if (v == 0) continue;
        gx.push_back((ie + 0.5) / (double)n_eta);
        gxe.push_back(0.5 / (double)n_eta);
        gy.push_back(v);
        gye.push_back(h2_before->GetBinError(iz+1, ie+1));
      }
      if (!gx.empty()) {
        TGraphErrors *gb = new TGraphErrors(gx.size(), gx.data(), gy.data(), gxe.data(), gye.data());
        gb->SetMarkerStyle(24);
        gb->SetMarkerColor(kBlue+1);
        gb->SetLineColor(kBlue+1);
        gb->Draw("PE same");
        keep_graphs.push_back(gb);
      }
    }

    // After (filled red)
    std::vector<double> gx, gy, gxe, gye;
    for (int ie = 0; ie < n_eta; ++ie) {
      if (!after_ok[iz][ie]) continue;
      gx.push_back((ie + 0.5) / (double)n_eta);
      gxe.push_back(0.5 / (double)n_eta);
      gy.push_back(after_mu[iz][ie]);
      gye.push_back(after_err[iz][ie]);
    }
    if (!gx.empty()) {
      TGraphErrors *ga = new TGraphErrors(gx.size(), gx.data(), gy.data(), gxe.data(), gye.data());
      ga->SetMarkerStyle(20);
      ga->SetMarkerColor(kRed+1);
      ga->SetLineColor(kRed+1);
      ga->Draw("PE same");
      keep_graphs.push_back(ga);
    }

    TLatex tex;
    tex.SetNDC();
    tex.SetTextSize(0.055);
    tex.DrawLatex(0.18, 0.88, z_label[iz]);
    if (iz == 0) {
      tex.SetTextColor(kBlue+1);
      tex.DrawLatex(0.18, 0.24, "open: first pass calibration");
      tex.SetTextColor(kRed+1);
      tex.DrawLatex(0.18, 0.18, "filled: second pass calibration");
      tex.SetTextColor(kBlack);
    }
  }
  c_f->SaveAs(Form("%s/fullclosure_r0%d_vs_f.png", figdir, r_val));
  delete c_f;

  // ---------- Canvas B: mean vs |z| per f bin, after correction ----------
  TCanvas *c_z = new TCanvas("c_fullclosure_vs_z", "", 900, 650);
  c_z->SetLeftMargin(0.12);
  c_z->SetRightMargin(0.05);
  c_z->SetTopMargin(0.08);
  c_z->SetBottomMargin(0.13);

  TH1D *frame_z = new TH1D("frame_fullclosure_z", ";z_{vertex} [cm];<p_{T}^{calib} / p_{T}^{truth}>", 1, -200, 200);
  frame_z->SetDirectory(0);
  frame_z->GetYaxis()->SetRangeUser(0.95, 1.05);
  frame_z->Draw("AXIS");

  TLine *l1z = new TLine(-200, 1.0, 200, 1.0);
  l1z->SetLineColor(kGray+2);
  l1z->SetLineStyle(2);
  l1z->Draw("same");
  TLine *luz = new TLine(-200, 1.01, 200, 1.01);
  luz->SetLineColor(kGray+1);
  luz->SetLineStyle(3);
  luz->Draw("same");
  TLine *ldz = new TLine(-200, 0.99, 200, 0.99);
  ldz->SetLineColor(kGray+1);
  ldz->SetLineStyle(3);
  ldz->Draw("same");

  TLegend *leg = new TLegend(0.15, 0.15, 0.50, 0.40);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.030);
  leg->SetHeader("#eta fraction (second pass calibration)");

  for (int ie = 0; ie < n_eta; ++ie) {
    std::vector<double> gx, gy, gxe, gye;
    for (int iz = 0; iz < n_zphys; ++iz) {
      if (!after_ok[iz][ie]) continue;
      gx.push_back(0.5*(z_edge[iz] + z_edge[iz+1]));
      gxe.push_back(0.5*(z_edge[iz+1] - z_edge[iz]));
      gy.push_back(after_mu[iz][ie]);
      gye.push_back(after_err[iz][ie]);
    }
    if (gx.empty()) continue;
    TGraphErrors *g = new TGraphErrors(gx.size(), gx.data(), gy.data(), gxe.data(), gye.data());
    g->SetMarkerStyle(20 + ie);
    g->SetMarkerColor(eta_color[ie]);
    g->SetLineColor(eta_color[ie]);
    g->Draw("PE same");
    keep_graphs.push_back(g);
    leg->AddEntry(g, Form("%.2f #leq f < %.2f", (double)ie/n_eta, (double)(ie+1)/n_eta), "lep");
  }
  leg->Draw();

  TLatex tex_z;
  tex_z.SetNDC();
  tex_z.SetTextFont(42);
  tex_z.SetTextSize(0.04);
  tex_z.DrawLatex(0.14, 0.85, Form("Anti-k_{T} R=0.%d, full-chain closure", r_val));

  c_z->SaveAs(Form("%s/fullclosure_r0%d_vs_z.png", figdir, r_val));
  delete c_z;
  delete leg;

  if (f_before) f_before->Close();

  if (max_dev_iz >= 0) {
    std::cout << "Full-chain closure: maximum deviation from unity = " << max_dev*100 << "%"
              << " at z bin " << max_dev_iz << " (" << z_label[max_dev_iz] << "),"
              << " f bin " << max_dev_ie << std::endl;
  }
  std::cout << "Plots written to " << figdir << "/fullclosure_r0" << r_val << "_*.png" << std::endl;
}

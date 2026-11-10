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
// Validation test 4: pT / EMfrac factorization of the residual (z, eta) correction.
//
// The residual correction is averaged over the (pT, EMfrac) cells; this macro
// checks that the (z, f) dependence does not itself depend on pT or EMfrac by
// splitting the bin-content distributions of h2_mean into low/high pT halves
// (cell center below/above pt_split) and low/high EMfrac halves (below/above
// em_split), and comparing the extracted closure means.
//
// Pure post-processing: reads the closure-pass output_gaus_r0R_zZ[_etaE].root
// files. Run it BEFORE the closure_mode = 2 production overwrites them.
//
// Output: figure_dependence/factorization_r0R_vs_z.png   (eta-integrated, splits vs |z|)
//         figure_dependence/factorization_r0R_pt_vs_f.png (pT split vs f, per-z grid)
//         figure_dependence/factorization_r0R_em_vs_f.png (EMfrac split vs f, per-z grid)
// ============================================================================

// Mean of the bin-content distribution of h2_mean restricted to cells selected by
// axis ('x' = pT, 'y' = EMfrac) and side (0 = below split, 1 = above split).
// Returns true if a usable mean was extracted.
bool factorization_mean(TH2D* h2, char axis, int side, double split, double &mean, double &meanerr) {
  TH1D h1("h1_tmp_fact", "", 120, 0.7, 1.3);
  for (int ix = 1; ix <= h2->GetNbinsX(); ++ix) {
    for (int iy = 1; iy <= h2->GetNbinsY(); ++iy) {
      double v = h2->GetBinContent(ix, iy);
      if (v == 0) continue;
      double coord = (axis == 'x') ? h2->GetXaxis()->GetBinCenter(ix) : h2->GetYaxis()->GetBinCenter(iy);
      if (side == 0 && coord >= split) continue;
      if (side == 1 && coord < split) continue;
      h1.Fill(v);
    }
  }
  if (h1.GetEntries() < 2) return false;
  if (h1.GetEntries() >= 5) {
    double seed_mean = h1.GetMean();
    double seed_rms  = (h1.GetRMS() > 0) ? h1.GetRMS() : 0.05;
    TF1 f("f_tmp_fact", "gaus", 0.7, 1.3);
    f.SetParameters(h1.GetMaximum(), seed_mean, seed_rms);
    h1.Rebin(2);
    double half_width = TMath::Max(0.05, 1.5 * seed_rms); // fit window: mean +/- 1.5 sigma, floored at +/- 0.05
    h1.Fit(&f, "Q R N", "", TMath::Max(0.7, seed_mean - half_width), TMath::Min(1.3, seed_mean + half_width));
    double m = f.GetParameter(1);
    double me = f.GetParError(1);
    double s = f.GetParameter(2);
    if (TMath::Abs(m - 1.0) < 0.2 && s > 0 && s < 0.15 && me > 0 && me < 0.03) {
      mean = m;
      meanerr = me;
      return true;
    }
  }
  mean = h1.GetMean();
  meanerr = h1.GetMeanError();
  return (TMath::Abs(mean - 1.0) < 0.2 && meanerr > 0 && meanerr < 0.05);
}

void draw_factorization(int r_val = 4, double pt_split = 30.0, double em_split = 0.5) {
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  const char* figdir = "figure_dependence";
  gSystem->mkdir(figdir, kTRUE);

  const int n_eta = get_n_etafracbin(r_val);
  const int n_zphys = n_zvertex_phys; // signed z bins, edges from Jet_resp_Def.h
  double *z_edge = zvertex_edges;

  std::vector<TGraphErrors*> keep_graphs;

  // ---------- Part 1: eta-integrated splits versus |z| ----------
  // 4 categories: pT low/high, EMfrac low/high, from output_gaus_r0R_zZ.root
  double cat_mu[4][n_zphys], cat_err[4][n_zphys];
  bool   cat_ok[4][n_zphys];
  for (int iz = 0; iz < n_zphys; ++iz) {
    for (int ic = 0; ic < 4; ++ic) cat_ok[ic][iz] = false;
    TFile *fin = TFile::Open(Form("output_gaus/output_gaus_r0%d_z%d.root", r_val, iz), "READ");
    if (!fin || fin->IsZombie()) {
      std::cerr << "Cannot open output_gaus/output_gaus_r0" << r_val << "_z" << iz << ".root" << std::endl;
      continue;
    }
    TH2D *h2 = (TH2D*)fin->Get(Form("h2_mean_r0%d_z%d", r_val, iz));
    if (h2) {
      cat_ok[0][iz] = factorization_mean(h2, 'x', 0, pt_split, cat_mu[0][iz], cat_err[0][iz]);
      cat_ok[1][iz] = factorization_mean(h2, 'x', 1, pt_split, cat_mu[1][iz], cat_err[1][iz]);
      cat_ok[2][iz] = factorization_mean(h2, 'y', 0, em_split, cat_mu[2][iz], cat_err[2][iz]);
      cat_ok[3][iz] = factorization_mean(h2, 'y', 1, em_split, cat_mu[3][iz], cat_err[3][iz]);
    }
    fin->Close();
    delete fin;
  }

  const char* cat_label[4] = {
    Form("p_{T} < %.0f GeV", pt_split), Form("p_{T} #geq %.0f GeV", pt_split),
    Form("EMfrac < %.2f", em_split), Form("EMfrac #geq %.2f", em_split)
  };
  int cat_color[4] = {kRed+1, kBlue+1, kRed+1, kBlue+1};
  int cat_marker[4] = {20, 24, 21, 25};

  TCanvas *c_z = new TCanvas("c_factorization_vs_z", "", 1400, 600);
  c_z->Divide(2, 1);
  for (int ipad = 0; ipad < 2; ++ipad) { // 0: pT split, 1: EMfrac split
    c_z->cd(ipad+1);
    gPad->SetLeftMargin(0.13);
    gPad->SetBottomMargin(0.13);
    TH1D *frame = new TH1D(Form("frame_fact_%d", ipad), ";z_{vertex} [cm];<p_{T}^{calib} / p_{T}^{truth}>", 1, -200, 200);
    frame->SetDirectory(0);
    frame->GetYaxis()->SetRangeUser(0.9, 1.1);
    frame->Draw("AXIS");
    TLine *l1 = new TLine(-200, 1.0, 200, 1.0);
    l1->SetLineColor(kGray+2);
    l1->SetLineStyle(2);
    l1->Draw("same");

    TLegend *leg = new TLegend(0.16, 0.16, 0.55, 0.32);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.035);
    for (int is = 0; is < 2; ++is) {
      int ic = 2*ipad + is;
      std::vector<double> gx, gy, gxe, gye;
      for (int iz = 0; iz < n_zphys; ++iz) {
        if (!cat_ok[ic][iz]) continue;
        gx.push_back(0.5*(z_edge[iz] + z_edge[iz+1]));
        gxe.push_back(0.5*(z_edge[iz+1] - z_edge[iz]));
        gy.push_back(cat_mu[ic][iz]);
        gye.push_back(cat_err[ic][iz]);
      }
      if (gx.empty()) continue;
      TGraphErrors *g = new TGraphErrors(gx.size(), gx.data(), gy.data(), gxe.data(), gye.data());
      g->SetMarkerStyle(cat_marker[ic]);
      g->SetMarkerColor(cat_color[ic]);
      g->SetLineColor(cat_color[ic]);
      g->Draw("PE same");
      keep_graphs.push_back(g);
      leg->AddEntry(g, cat_label[ic], "lep");
    }
    leg->Draw();
    TLatex tex;
    tex.SetNDC();
    tex.SetTextFont(42);
    tex.SetTextSize(0.04);
    tex.DrawLatex(0.15, 0.86, Form("Anti-k_{T} R=0.%d, %s split (#eta-integrated)", r_val, (ipad == 0) ? "p_{T}" : "EMfrac"));
  }
  c_z->SaveAs(Form("%s/factorization_r0%d_vs_z.png", figdir, r_val));
  delete c_z;

  // ---------- Part 2: splits versus f, per z bin ----------
  // itype 0: pT split ('x'), itype 1: EMfrac split ('y')
  for (int itype = 0; itype < 2; ++itype) {
    char axis = (itype == 0) ? 'x' : 'y';
    double split = (itype == 0) ? pt_split : em_split;
    const char* typetag = (itype == 0) ? "pt" : "em";

    TCanvas *c_f = new TCanvas(Form("c_factorization_%s_vs_f", typetag), "", 1600, 1200);
    c_f->Divide(4, (n_zphys + 3) / 4);
    for (int iz = 0; iz < n_zphys; ++iz) {
      c_f->cd(iz+1);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.15);
      TH1D *frame = new TH1D(Form("frame_fact_%s_z%d", typetag, iz), ";#eta fraction f;<p_{T}^{calib} / p_{T}^{truth}>", 1, 0, 1);
      frame->SetDirectory(0);
      frame->GetYaxis()->SetRangeUser(0.9, 1.1);
      frame->Draw("AXIS");
      TLine *l1 = new TLine(0, 1.0, 1, 1.0);
      l1->SetLineColor(kGray+2);
      l1->SetLineStyle(2);
      l1->Draw("same");

      for (int side = 0; side < 2; ++side) {
        std::vector<double> gx, gy, gxe, gye;
        for (int ie = 0; ie < n_eta; ++ie) {
          TFile *fin = TFile::Open(Form("output_gaus/output_gaus_r0%d_z%d_eta%d.root", r_val, iz, ie), "READ");
          if (!fin || fin->IsZombie()) continue;
          TH2D *h2 = (TH2D*)fin->Get(Form("h2_mean_r0%d_z%d_eta%d", r_val, iz, ie));
          double m, me;
          if (h2 && factorization_mean(h2, axis, side, split, m, me)) {
            gx.push_back((ie + 0.5) / (double)n_eta);
            gxe.push_back(0.5 / (double)n_eta);
            gy.push_back(m);
            gye.push_back(me);
          }
          fin->Close();
          delete fin;
        }
        if (gx.empty()) continue;
        TGraphErrors *g = new TGraphErrors(gx.size(), gx.data(), gy.data(), gxe.data(), gye.data());
        g->SetMarkerStyle((side == 0) ? 20 : 24);
        g->SetMarkerColor((side == 0) ? kRed+1 : kBlue+1);
        g->SetLineColor((side == 0) ? kRed+1 : kBlue+1);
        g->Draw("PE same");
        keep_graphs.push_back(g);
      }

      TLatex tex;
      tex.SetNDC();
      tex.SetTextSize(0.055);
      tex.DrawLatex(0.18, 0.88, z_label[iz]);
      if (iz == 0) {
        tex.SetTextColor(kRed+1);
        tex.DrawLatex(0.18, 0.24, (itype == 0) ? Form("filled: p_{T} < %.0f", pt_split) : Form("filled: EMfrac < %.2f", em_split));
        tex.SetTextColor(kBlue+1);
        tex.DrawLatex(0.18, 0.18, (itype == 0) ? Form("open: p_{T} #geq %.0f", pt_split) : Form("open: EMfrac #geq %.2f", em_split));
        tex.SetTextColor(kBlack);
      }
    }
    c_f->SaveAs(Form("%s/factorization_r0%d_%s_vs_f.png", figdir, r_val, typetag));
    delete c_f;
  }

  std::cout << "Factorization check plots written to " << figdir << "/factorization_r0" << r_val << "_*.png" << std::endl;
}

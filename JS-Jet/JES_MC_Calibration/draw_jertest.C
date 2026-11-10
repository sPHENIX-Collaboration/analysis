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
// Validation test 6: jet energy resolution before/after the residual correction.
//
// Compares JES (Gaussian mean) and JER (sigma/mean) versus truth pT in a small-|z|
// (|z| <= 30 cm) and a mid-|z| (30 < |z| <= 60 cm) selection, between:
//   "raw"    = no calibration (uncalibrated reco pT, from the closure_mode 2 file),
//   "before" = closure_mode 1 production (first-pass calibration only),
//   "after"  = closure_mode 2 production (first pass x (z, eta) correction).
//
// Input: h_truthpt_ptratio_comb_r0R_z30 / _z3060 (and _raw_) from the two combined
// files (save them as output_combined_closure1.root / output_combined_closure2.root).
//
// Output: figure_dependence/jertest_r0R.png (2x2: JES and JER, small/mid |z|).
// ============================================================================

// Iterative core Gaussian fit; returns false if unusable.
bool jer_fit_response(TH1D* h, double &mean, double &meanerr, double &sigma, double &sigmaerr) {
  if (!h || h->GetEntries() < 100) return false;
  double m = h->GetMean();
  double s = (h->GetRMS() > 0) ? h->GetRMS() : 0.15;
  TF1 f("f_core_jer", "gaus", 0, 2);
  f.SetParameters(h->GetMaximum(), m, s);
  h->Fit(&f, "Q R N", "", m - 1.5*s, m + 1.5*s);
  m = f.GetParameter(1);
  s = f.GetParameter(2);
  if (s <= 0 || s > 0.5) return false;
  h->Fit(&f, "Q R N", "", m - 1.5*s, m + 1.5*s);
  mean = f.GetParameter(1);
  meanerr = f.GetParError(1);
  sigma = f.GetParameter(2);
  sigmaerr = f.GetParError(2);
  return (mean > 0.4 && mean < 1.5 && sigma > 0 && sigma < 0.5); // lower bound covers the raw (uncalibrated) response
}

// Extract JES and JER versus truth pT from one 2D histogram.
void jer_extract(TH2D* h2, std::vector<double> &pt, std::vector<double> &pte,
                 std::vector<double> &jes, std::vector<double> &jese,
                 std::vector<double> &jer, std::vector<double> &jere) {
  for (int ix = 1; ix <= h2->GetNbinsX(); ++ix) {
    TH1D *p = h2->ProjectionY(Form("p_jer_%s_%d", h2->GetName(), ix), ix, ix);
    p->SetDirectory(0);
    double m, me, s, se;
    if (jer_fit_response(p, m, me, s, se)) {
      pt.push_back(h2->GetXaxis()->GetBinCenter(ix));
      pte.push_back(0.5 * h2->GetXaxis()->GetBinWidth(ix));
      jes.push_back(m);
      jese.push_back(me);
      double r = s / m;
      jer.push_back(r);
      jere.push_back(r * TMath::Sqrt((se/s)*(se/s) + (me/m)*(me/m)));
    }
    delete p;
  }
}

void draw_jertest(int r_val = 4,
                  const char* file_before = "output_combined_closure1.root",
                  const char* file_after  = "output_combined_closure2.root") {
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  const char* figdir = "figure_dependence";
  gSystem->mkdir(figdir, kTRUE);

  TFile *f_before = TFile::Open(file_before, "READ");
  TFile *f_after  = TFile::Open(file_after, "READ");
  if (!f_before || f_before->IsZombie() || !f_after || f_after->IsZombie()) {
    std::cerr << "Cannot open " << file_before << " and/or " << file_after << std::endl;
    return;
  }

  const char* zone_tag[2] = {"z30", "z3060"};
  const char* zone_label[2] = {"|z| #leq 30 cm", "30 < |z| #leq 60 cm"};

  TCanvas *c = new TCanvas("c_jertest", "", 1400, 1000);
  c->Divide(2, 2); // columns: z zones; rows: JES (top), JER (bottom)
  std::vector<TGraphErrors*> keep_graphs;

  for (int izone = 0; izone < 2; ++izone) {
    TH2D *h2_b = (TH2D*)f_before->Get(Form("h_truthpt_ptratio_comb_r0%d_%s", r_val, zone_tag[izone]));
    TH2D *h2_a = (TH2D*)f_after->Get(Form("h_truthpt_ptratio_comb_r0%d_%s", r_val, zone_tag[izone]));
    TH2D *h2_r = (TH2D*)f_after->Get(Form("h_truthpt_ptratio_raw_comb_r0%d_%s", r_val, zone_tag[izone])); // uncalibrated reference
    if (!h2_b || !h2_a) {
      std::cerr << "Missing h_truthpt_ptratio_comb_r0" << r_val << "_" << zone_tag[izone]
                << " in the input files" << std::endl;
      continue;
    }
    if (!h2_r) {
      std::cerr << "Note: no h_truthpt_ptratio_raw_comb_r0" << r_val << "_" << zone_tag[izone]
                << " (old input file); raw curve skipped" << std::endl;
    }

    std::vector<double> pt_b, pte_b, jes_b, jese_b, jer_b, jere_b;
    std::vector<double> pt_a, pte_a, jes_a, jese_a, jer_a, jere_a;
    std::vector<double> pt_r, pte_r, jes_r, jese_r, jer_r, jere_r;
    jer_extract(h2_b, pt_b, pte_b, jes_b, jese_b, jer_b, jere_b);
    jer_extract(h2_a, pt_a, pte_a, jes_a, jese_a, jer_a, jere_a);
    if (h2_r) jer_extract(h2_r, pt_r, pte_r, jes_r, jese_r, jer_r, jere_r);

    // --- Top row: JES ---
    c->cd(izone + 1);
    gPad->SetLeftMargin(0.13);
    gPad->SetBottomMargin(0.13);
    TH1D *frame_jes = new TH1D(Form("frame_jes_%d", izone), ";p_{T}^{truth jet} [GeV];<p_{T}^{calib} / p_{T}^{truth}>", 1, 0, 100);
    frame_jes->SetDirectory(0);
    frame_jes->GetYaxis()->SetRangeUser(0.6, 1.1); // low enough to cover the raw (uncalibrated) JES
    frame_jes->Draw("AXIS");
    TLine *l1 = new TLine(0, 1.0, 100, 1.0);
    l1->SetLineColor(kGray+2);
    l1->SetLineStyle(2);
    l1->Draw("same");

    TLegend *leg = new TLegend(0.55, 0.16, 0.93, 0.36);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.035);

    if (!pt_r.empty()) {
      TGraphErrors *gr = new TGraphErrors(pt_r.size(), pt_r.data(), jes_r.data(), pte_r.data(), jese_r.data());
      gr->SetMarkerStyle(26);
      gr->SetMarkerColor(kBlack);
      gr->SetLineColor(kBlack);
      gr->Draw("PE same");
      keep_graphs.push_back(gr);
      leg->AddEntry(gr, "no calibration", "lep");
    }
    if (!pt_b.empty()) {
      TGraphErrors *gb = new TGraphErrors(pt_b.size(), pt_b.data(), jes_b.data(), pte_b.data(), jese_b.data());
      gb->SetMarkerStyle(24);
      gb->SetMarkerColor(kBlue+1);
      gb->SetLineColor(kBlue+1);
      gb->Draw("PE same");
      keep_graphs.push_back(gb);
      leg->AddEntry(gb, "first pass calibration", "lep");
    }
    if (!pt_a.empty()) {
      TGraphErrors *ga = new TGraphErrors(pt_a.size(), pt_a.data(), jes_a.data(), pte_a.data(), jese_a.data());
      ga->SetMarkerStyle(20);
      ga->SetMarkerColor(kRed+1);
      ga->SetLineColor(kRed+1);
      ga->Draw("PE same");
      keep_graphs.push_back(ga);
      leg->AddEntry(ga, "second pass calibration", "lep");
    }
    leg->Draw();
    TLatex tex;
    tex.SetNDC();
    tex.SetTextFont(42);
    tex.SetTextSize(0.04);
    tex.DrawLatex(0.15, 0.86, Form("Anti-k_{T} R=0.%d, JES, %s", r_val, zone_label[izone]));

    // --- Bottom row: JER ---
    c->cd(izone + 3);
    gPad->SetLeftMargin(0.13);
    gPad->SetBottomMargin(0.13);
    TH1D *frame_jer = new TH1D(Form("frame_jer_%d", izone), ";p_{T}^{truth jet} [GeV];#sigma / #mu", 1, 0, 100);
    frame_jer->SetDirectory(0);
    frame_jer->GetYaxis()->SetRangeUser(0.0, 0.5);
    frame_jer->Draw("AXIS");

    if (!pt_r.empty()) {
      TGraphErrors *gr = new TGraphErrors(pt_r.size(), pt_r.data(), jer_r.data(), pte_r.data(), jere_r.data());
      gr->SetMarkerStyle(26);
      gr->SetMarkerColor(kBlack);
      gr->SetLineColor(kBlack);
      gr->Draw("PE same");
      keep_graphs.push_back(gr);
    }
    if (!pt_b.empty()) {
      TGraphErrors *gb = new TGraphErrors(pt_b.size(), pt_b.data(), jer_b.data(), pte_b.data(), jere_b.data());
      gb->SetMarkerStyle(24);
      gb->SetMarkerColor(kBlue+1);
      gb->SetLineColor(kBlue+1);
      gb->Draw("PE same");
      keep_graphs.push_back(gb);
    }
    if (!pt_a.empty()) {
      TGraphErrors *ga = new TGraphErrors(pt_a.size(), pt_a.data(), jer_a.data(), pte_a.data(), jere_a.data());
      ga->SetMarkerStyle(20);
      ga->SetMarkerColor(kRed+1);
      ga->SetLineColor(kRed+1);
      ga->Draw("PE same");
      keep_graphs.push_back(ga);
    }
    TLatex tex2;
    tex2.SetNDC();
    tex2.SetTextFont(42);
    tex2.SetTextSize(0.04);
    tex2.DrawLatex(0.15, 0.86, Form("Anti-k_{T} R=0.%d, JER, %s", r_val, zone_label[izone]));
  }

  c->SaveAs(Form("%s/jertest_r0%d.png", figdir, r_val));
  delete c;

  f_before->Close();
  f_after->Close();

  std::cout << "JER test plot written to " << figdir << "/jertest_r0" << r_val << ".png" << std::endl;
}

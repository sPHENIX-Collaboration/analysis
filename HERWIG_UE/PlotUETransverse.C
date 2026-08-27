// PlotUETransverse.C
//
// Reproduce Fig. 4 (top right) of the HERWIG7 RHIC tune paper
// [arXiv:2411.16897]: Transverse-region <dNch/(deta dphi)> (pT > 0.2 GeV/c)
// vs leading-jet pT, HERWIG (Nashville) vs STAR, with an MC/data ratio panel.
//
// STAR data points are PRD 101, 052004 (2020), Figure 2 Transverse region
// (HEPData / Rivet ref STAR_2019_I1771348, d01-x01-y03).
//
//   root -l 'PlotUETransverse.C("herwig_ue_check.root")'

#include <TCanvas.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TProfile.h>
#include <TStyle.h>

#include <cmath>
#include <iostream>

void PlotUETransverse(const char *mcfile = "herwig_ue_check.root",
                      const char *outname = "ue_transverse_nashville_vs_star")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  // ------------------------------------------------------------------
  // STAR PRD 101, 052004: Transverse <dNch/detadphi>, pT > 0.2 GeV/c
  // ------------------------------------------------------------------
  const int nb = 8;
  const double edges[nb + 1] = {5., 7., 9., 11., 15., 20., 25., 35., 45.};
  const double val[nb] = {0.7585, 0.7454, 0.7195, 0.6641,
                          0.5665, 0.5415, 0.5205, 0.4944};
  const double stat[nb] = {2.0e-4, 1.0e-4, 2.0e-4, 2.0e-4,
                           3.0e-4, 8.0e-4, 1.5e-3, 6.0e-3};
  const double sysDn[nb] = {0.060, 0.070, 0.070, 0.050,
                            0.033, 0.034, 0.027, 0.033};
  const double sysUp[nb] = {0.060, 0.040, 0.050, 0.050,
                            0.050, 0.040, 0.032, 0.032};

  TGraphAsymmErrors *gData = new TGraphAsymmErrors(nb);
  TGraphAsymmErrors *gDataSys = new TGraphAsymmErrors(nb);  // sys band
  for (int i = 0; i < nb; ++i)
  {
    const double xc = 0.5 * (edges[i] + edges[i + 1]);
    const double xw = 0.5 * (edges[i + 1] - edges[i]);
    gData->SetPoint(i, xc, val[i]);
    gData->SetPointError(i, 0., 0., stat[i], stat[i]);
    gDataSys->SetPoint(i, xc, val[i]);
    gDataSys->SetPointError(i, xw, xw, sysDn[i], sysUp[i]);
  }

  // ------------------------------------------------------------------
  // HERWIG Nashville prediction from the Fun4All module
  // ------------------------------------------------------------------
  TFile *fin = TFile::Open(mcfile, "READ");
  if (!fin || fin->IsZombie())
  {
    std::cerr << "cannot open " << mcfile << std::endl;
    return;
  }
  TProfile *prof = dynamic_cast<TProfile *>(fin->Get("p_dens_trans_pt02"));
  if (!prof)
  {
    std::cerr << "p_dens_trans_pt02 not found in " << mcfile << std::endl;
    return;
  }
  TH1D *hMC = prof->ProjectionX("h_mc_trans");
  hMC->SetDirectory(nullptr);

  // ratio MC/data with the data uncertainty band around unity
  TH1D *hRatio = (TH1D *) hMC->Clone("h_ratio");
  hRatio->SetDirectory(nullptr);
  TGraphAsymmErrors *gBand = new TGraphAsymmErrors(nb);
  for (int i = 0; i < nb; ++i)
  {
    const double d = val[i];
    hRatio->SetBinContent(i + 1, hMC->GetBinContent(i + 1) / d);
    hRatio->SetBinError(i + 1, hMC->GetBinError(i + 1) / d);
    const double xc = 0.5 * (edges[i] + edges[i + 1]);
    const double xw = 0.5 * (edges[i + 1] - edges[i]);
    const double eDn = std::sqrt(stat[i] * stat[i] + sysDn[i] * sysDn[i]) / d;
    const double eUp = std::sqrt(stat[i] * stat[i] + sysUp[i] * sysUp[i]) / d;
    gBand->SetPoint(i, xc, 1.);
    gBand->SetPointError(i, xw, xw, eDn, eUp);
  }

  // ------------------------------------------------------------------
  // draw
  // ------------------------------------------------------------------
  TCanvas *c = new TCanvas("c", "", 700, 750);
  TPad *pTop = new TPad("pTop", "", 0., 0.33, 1., 1.);
  TPad *pBot = new TPad("pBot", "", 0., 0., 1., 0.33);
  pTop->SetBottomMargin(0.02);
  pBot->SetTopMargin(0.03);
  pBot->SetBottomMargin(0.30);
  pTop->Draw();
  pBot->Draw();

  pTop->cd();
  TH1D *frame = new TH1D("frame", ";;#LTdN_{ch}/d#etad#phi#GT", nb, edges);
  frame->GetYaxis()->SetRangeUser(0., 1.1);
  frame->GetYaxis()->SetTitleSize(0.06);
  frame->GetYaxis()->SetLabelSize(0.05);
  frame->GetXaxis()->SetLabelSize(0.);
  frame->Draw("AXIS");

  gDataSys->SetFillColorAlpha(kOrange - 2, 0.5);
  gDataSys->Draw("2 SAME");
  gData->SetMarkerStyle(20);
  gData->SetMarkerSize(1.1);
  gData->SetMarkerColor(kBlack);
  gData->SetLineColor(kBlack);
  gData->Draw("P SAME");

  hMC->SetLineColor(kRed + 1);
  hMC->SetMarkerColor(kRed + 1);
  hMC->SetMarkerStyle(24);
  hMC->SetLineWidth(2);
  hMC->Draw("E1 SAME");

  TLegend *leg = new TLegend(0.45, 0.62, 0.88, 0.87);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(gData, "STAR, PRD 101, 052004", "p");
  leg->AddEntry(gDataSys, "syst. uncertainty", "f");
  leg->AddEntry(hMC, "HERWIG 7.3 Nashville (sPHENIX prod.)", "lp");
  leg->Draw();

  TLatex tx;
  tx.SetNDC();
  tx.SetTextSize(0.05);
  tx.DrawLatex(0.14, 0.85, "p+p #sqrt{s} = 200 GeV");
  tx.DrawLatex(0.14, 0.78, "Transverse region, p_{T}^{ch} > 0.2 GeV/c");
  tx.SetTextSize(0.042);
  tx.DrawLatex(0.14, 0.71, "anti-k_{T} R = 0.6, |#eta_{jet}| < 0.4, |#eta_{ch}| < 1");

  pBot->cd();
  TH1D *rframe = new TH1D("rframe",
                          ";leading jet p_{T} [GeV/c];MC / data", nb, edges);
  rframe->GetYaxis()->SetRangeUser(0.5, 1.5);
  rframe->GetYaxis()->SetNdivisions(505);
  rframe->GetYaxis()->SetTitleSize(0.11);
  rframe->GetYaxis()->SetTitleOffset(0.5);
  rframe->GetYaxis()->SetLabelSize(0.10);
  rframe->GetXaxis()->SetTitleSize(0.12);
  rframe->GetXaxis()->SetLabelSize(0.10);
  rframe->Draw("AXIS");

  gBand->SetFillColorAlpha(kOrange - 2, 0.5);
  gBand->Draw("2 SAME");
  TLine unity(edges[0], 1., edges[nb], 1.);
  unity.SetLineStyle(2);
  unity.DrawClone("SAME");

  hRatio->SetLineColor(kRed + 1);
  hRatio->SetMarkerColor(kRed + 1);
  hRatio->SetMarkerStyle(24);
  hRatio->SetLineWidth(2);
  hRatio->Draw("E1 SAME");

  c->SaveAs(Form("%s.pdf", outname));
  c->SaveAs(Form("%s.png", outname));

  // quick numerical summary
  std::cout << "\n  bin [GeV/c]      STAR        HERWIG      MC/data\n";
  for (int i = 0; i < nb; ++i)
  {
    printf("  %4.0f - %4.0f    %.4f      %.4f      %.3f\n",
           edges[i], edges[i + 1], val[i], hMC->GetBinContent(i + 1),
           hMC->GetBinContent(i + 1) / val[i]);
  }
}

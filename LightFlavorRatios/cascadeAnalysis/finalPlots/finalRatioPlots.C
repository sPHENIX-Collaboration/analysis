#include <fstream>
#include <iostream>
#include "stdio.h"

#include <TChain.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TROOT.h>
#include <TMath.h>
#include <TString.h>
#include <TTree.h>
#include <TVectorD.h>
#include <TVirtualFitter.h>
#include <cassert>
#include <cmath>
#include <vector>

void finalRatioPlots()
{
  TFile* rawYieldFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/systematics/rawDataYield.root", "READ"); 

  const std::vector<std::string> kinVars  = {"pT", "phi", "eta", "rap"};
  std::map<std::string, std::string> xAxisLabels  = {{"pT","Mother p_{T} [GeV/c]"}, {"phi","Mother #phi [rad]"}, {"eta","Mother #eta"}, {"rap","Mother Rapidity"}};
  std::map<std::string, TH1F*> hMap_raw;
  for (const auto& kin : kinVars)
  {
    for (const auto& par : {"KS0", "Xi"})
    {
      std::string name    = "h_" + kin + "_base_" + par;
      TH1F* h             = (TH1F*)rawYieldFile->Get(name.c_str());
      h->SetDirectory(0);
      hMap_raw[kin + "_" + par] = h;
    }
  }
  rawYieldFile->Close();

  TFile* geoAccEffFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_evaluator/geometricAcceptanceEfficiencyCorrections.root","READ");
  std::map<std::string, TH1F*> hMap_geoAccEff;
  for (const auto& kin : kinVars)
  {
    for (const auto& par : {"KS0", "Xi"})
    {
      std::string name    = "h_geoAccEff_" + kin + "_" + par;
      TH1F* h             = (TH1F*)geoAccEffFile->Get(name.c_str());
      h->SetDirectory(0);
      hMap_geoAccEff[kin + "_" + par] = h;
    }
  }  
  geoAccEffFile->Close();
 
  TFile* cutEffFile = TFile::Open("/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/pTComp/cutEfficiencyCorrections.root","READ");
  std::map<std::string, TH1F*> hMap_cutEff;
  for (const auto& kin : kinVars)
  {
    for (const auto& par : {"KS0", "Xi"})
    {
      std::string name    = "h_cutEff_" + kin + "_" + par;
      TH1F* h             = (TH1F*)cutEffFile->Get(name.c_str());
      h->SetDirectory(0);
      hMap_cutEff[kin + "_" + par] = h;
    }
  }  
  cutEffFile->Close();
 
  TFile* trackAccFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/bachelor_pi_efficiency/analysis/trackingAcceptanceEfficiencyCorrections_withSystematics.root","READ");
  std::map<std::string, TH1F*> hMap_trackAcc;
  for (const auto& kin : kinVars)
  {
    for (const auto& par : {"KS0", "Xi"})
    {
      std::string name    = "h_trackAcc_" + kin + "_" + par;
      TH1F* h             = (TH1F*)trackAccFile->Get(name.c_str());
      h->SetDirectory(0);
      hMap_trackAcc[kin + "_" + par] = h;
    }
  }  
  trackAccFile->Close();

  double KS02pipi_BR_PDG = 0.6920;
  double KS02pipi_BR_uncert_PDG = 0.0005;
  double Xi2Lambdapi_BR_PDG = 0.99887;
  double Xi2Lambdapi_BR_uncert_PDG = 0.00035;
  double Lambda2ppi_BR_PDG = 0.641;
  double Lambda2ppi_BR_uncert_PDG = 0.005;

  TFile* geoAccEffSysFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/systematics/geoAccEffSystematicUncertainties.root","READ");
  std::map<std::string, TH1F*> hMap_geoAccEffSys;
  for (const auto& kin : kinVars)
  {
    std::string name    = "h_ratio_base_" + kin;
    TH1F* h1            = (TH1F*)geoAccEffSysFile->Get(name.c_str());
    name                = "h_syst_" + kin;
    TH1F* h2            = (TH1F*)geoAccEffSysFile->Get(name.c_str());
    h1->SetDirectory(0);
    h2->SetDirectory(0);

    TH1F* h_syst = (TH1F*)h2->Clone(("h_geoAccEffSys_" + kin).c_str());
    h_syst->SetDirectory(0);
    h_syst->Divide(h_syst, h1, 1., 1.);

    hMap_geoAccEffSys[kin] = h_syst;
  }  
  geoAccEffSysFile->Close();
 
  TFile* cutEffSysFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/systematics/cutEffSystematicUncertainties.root","READ");
  std::map<std::string, TH1F*> hMap_cutEffSys;
  for (const auto& kin : kinVars)
  {
    std::string name    = "h_ratio_base_" + kin;
    TH1F* h1            = (TH1F*)cutEffSysFile->Get(name.c_str());
    name                = "h_syst_" + kin;
    TH1F* h2            = (TH1F*)cutEffSysFile->Get(name.c_str());
    h1->SetDirectory(0);
    h2->SetDirectory(0);

    TH1F* h_syst = (TH1F*)h2->Clone(("h_cutEffSys_" + kin).c_str());
    h_syst->SetDirectory(0);
    h_syst->Divide(h_syst, h1, 1., 1.);

    hMap_cutEffSys[kin] = h_syst;
  }  
  cutEffSysFile->Close();
 
  TFile* trackAccSysFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/systematics/trackAccSystematicUncertainties.root","READ");
  std::map<std::string, TH1F*> hMap_trackAccSys;
  for (const auto& kin : kinVars)
  {
    std::string name    = "h_ratio_base_" + kin;
    TH1F* h1            = (TH1F*)trackAccSysFile->Get(name.c_str());
    name                = "h_syst_" + kin;
    TH1F* h2            = (TH1F*)trackAccSysFile->Get(name.c_str());
    h1->SetDirectory(0);
    h2->SetDirectory(0);

    TH1F* h_syst = (TH1F*)h2->Clone(("h_trackAccSys_" + kin).c_str());
    h_syst->SetDirectory(0);
    h_syst->Divide(h_syst, h1, 1., 1.);

    hMap_trackAccSys[kin] = h_syst;
  }  
  trackAccSysFile->Close();
 
  TFile* cutVarSysFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/systematics/cutVarianceSystematicUncertainties.root","READ");
  std::map<std::string, TH1F*> hMap_cutVarSys;
  for (const auto& kin : kinVars)
  {
    std::string name    = "h_ratio_base_" + kin;
    TH1F* h1            = (TH1F*)cutVarSysFile->Get(name.c_str());
    name                = "h_syst_" + kin;
    TH1F* h2            = (TH1F*)cutVarSysFile->Get(name.c_str());
    h1->SetDirectory(0);
    h2->SetDirectory(0);

    TH1F* h_syst = (TH1F*)h2->Clone(("h_cutVarSys_" + kin).c_str());
    h_syst->SetDirectory(0);
    h_syst->Divide(h_syst, h1, 1., 1.);

    hMap_cutVarSys[kin] = h_syst;
  }  
  cutVarSysFile->Close();
 
  TFile* fitFuncSysFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/systematics/fitFunctionSystematicUncertainties.root","READ");
  std::map<std::string, TH1F*> hMap_fitFuncSys;
  for (const auto& kin : kinVars)
  {
    std::string name    = "h_ratio_base_" + kin;
    TH1F* h1            = (TH1F*)fitFuncSysFile->Get(name.c_str());
    name                = "h_syst_" + kin;
    TH1F* h2            = (TH1F*)fitFuncSysFile->Get(name.c_str());
    h1->SetDirectory(0);
    h2->SetDirectory(0);

    TH1F* h_syst = (TH1F*)h2->Clone(("h_fitFuncSys_" + kin).c_str());
    h_syst->SetDirectory(0);
    h_syst->Divide(h_syst, h1, 1., 1.);

    hMap_fitFuncSys[kin] = h_syst;
  }  
  fitFuncSysFile->Close(); 

  for (const auto& kin : kinVars)
  {
    TH1F* h_KS0 = (TH1F*)hMap_raw[kin + "_KS0"]->Clone(("h_KS0" + kin).c_str());
    TH1F* h_Xi = (TH1F*)hMap_raw[kin + "_Xi"]->Clone(("h_Xi" + kin).c_str());
    h_KS0->SetDirectory(0);
    h_Xi->SetDirectory(0);

    int nBins = h_KS0->GetNbinsX();

    for (int b = 1; b <= nBins; b++)
    {
      double KS0_yield    = h_KS0->GetBinContent(b);
      double KS0_statErr  = h_KS0->GetBinError(b);
      double KS0_geoAcc   = hMap_geoAccEff[kin + "_KS0"]->GetBinContent(b);
      double KS0_cutEff   = hMap_cutEff[kin + "_KS0"]->GetBinContent(b);
      double KS0_trackAcc = hMap_trackAcc[kin + "_KS0"]->GetBinContent(b);

      double KS0_totalCorr  = KS0_geoAcc * KS0_cutEff * KS0_trackAcc * KS02pipi_BR_PDG;
      double KS0_corrYield  = KS0_yield / KS0_totalCorr;
      double KS0_corrErr    = KS0_statErr / KS0_totalCorr;
      h_KS0->SetBinContent(b, KS0_corrYield);
      h_KS0->SetBinError(b, KS0_corrErr);

      double Xi_yield    = h_Xi->GetBinContent(b);
      double Xi_statErr  = h_Xi->GetBinError(b);
      double Xi_geoAcc   = hMap_geoAccEff[kin + "_Xi"]->GetBinContent(b);
      double Xi_cutEff   = hMap_cutEff[kin + "_Xi"]->GetBinContent(b);
      double Xi_trackAcc = hMap_trackAcc[kin + "_Xi"]->GetBinContent(b);

      double Xi_totalCorr  = Xi_geoAcc * Xi_cutEff * Xi_trackAcc * Xi2Lambdapi_BR_PDG * Lambda2ppi_BR_PDG;
      double Xi_corrYield  = Xi_yield / Xi_totalCorr;
      double Xi_corrErr    = Xi_statErr / Xi_totalCorr;
      h_Xi->SetBinContent(b, Xi_corrYield);
      h_Xi->SetBinError(b, Xi_corrErr);
    }
   
    std::string ratioName  = "h_ratio_" + kin;
    TH1F* h_ratio = (TH1F*)h_Xi->Clone(ratioName.c_str());
    h_ratio->SetDirectory(0);

    for (int b = 1; b <= nBins; b++)
    {
      double Xi_val    = h_Xi->GetBinContent(b);
      double Xi_err    = h_Xi->GetBinError(b);
      double KS0_val   = h_KS0->GetBinContent(b);
      double KS0_err   = h_KS0->GetBinError(b);

      double ratio    = Xi_val / (2.0 * KS0_val);

      double relErrXi  = Xi_err  / Xi_val;
      double relErrKS0 = KS0_err / KS0_val;
      double ratioErr  = ratio * sqrt(relErrXi * relErrXi + relErrKS0 * relErrKS0);

      h_ratio->SetBinContent(b, ratio);
      h_ratio->SetBinError(b, ratioErr);
    }

    TGraphErrors* g_stat = new TGraphErrors(nBins);
    TGraphErrors* g_syst = new TGraphErrors(nBins); 
    for (int b = 1; b <= nBins; b++)
    {
      double x        = h_ratio->GetBinCenter(b);
      double binWidth = h_ratio->GetBinWidth(b);
      double val      = h_ratio->GetBinContent(b);
      double statErr  = h_ratio->GetBinError(b);

      double geoAccEffSyst = hMap_geoAccEffSys[kin]->GetBinContent(b) * val;
      double cutEffSyst    = hMap_cutEffSys[kin]->GetBinContent(b) * val;
      double trackAccSyst  = hMap_trackAccSys[kin]->GetBinContent(b) * val;
      double cutVarSyst    = hMap_cutVarSys[kin]->GetBinContent(b) * val;
      double fitFuncSyst   = hMap_fitFuncSys[kin]->GetBinContent(b) * val;

      double totalSyst = sqrt(geoAccEffSyst * geoAccEffSyst +
                              cutEffSyst * cutEffSyst    +
                              trackAccSyst * trackAccSyst  +
                              cutVarSyst * cutVarSyst    +
                              fitFuncSyst * fitFuncSyst);
       
      g_stat->SetPoint(b - 1, x, val);
      g_stat->SetPointError(b - 1, 0, statErr);  

      g_syst->SetPoint(b - 1, x, val);
      g_syst->SetPointError(b - 1, binWidth / 2.0, totalSyst); 
    }

    g_stat->SetMarkerStyle(20);
    g_stat->SetMarkerSize(0.8);
    g_stat->SetMarkerColor(kBlack);
    g_stat->SetLineColor(kBlack);
    g_stat->SetLineWidth(2);

    g_syst->SetMarkerStyle(0);
    g_syst->SetFillColorAlpha(kBlue, 0.35);
    g_syst->SetFillStyle(1001);
    g_syst->SetLineColor(kBlue);
    g_syst->SetLineWidth(1);

    TCanvas* c = new TCanvas(("c_" + kin).c_str(), ("c_" + kin).c_str(), 800, 600);

    g_syst->Draw("A E2");
    g_stat->Draw("P E same");

    std::string axisLabel = xAxisLabels[kin];
    g_syst->GetXaxis()->SetTitle(axisLabel.c_str());
    g_syst->GetYaxis()->SetTitle("(#Xi^{-} + #bar{#Xi}^{+}) / (2K_{S}^{0})");
    g_syst->SetMinimum(0.0);
    g_syst->SetMaximum(0.2);

    TLegend* leg = new TLegend(0.2, 0.78, 0.6, 0.9);
    leg->SetBorderSize(0);
    leg->AddEntry(g_stat, "#it{#bf{sPHENIX}} p+p #sqrt{s} = 200 GeV", "lpe");
    //leg->AddEntry(g_syst, "Systematic uncertainty",  "f");

    if (kin == "pT")
    {
      const double m0_Xi  = 1.3217;
      const double m0_KS0 = 0.4976;

      // STAR Fit Values
      double C_Xim  = 0.237066; double eC_Xim  = 0.0131962;
      double dNdy_Xim = 0.00280512; double edNdy_Xim = 0.000345498;
      double n_Xim  = 14.2103; double en_Xim  = 1.95642;

      double C_Xip  = 0.235707; double eC_Xip  = 0.0108784;
      double dNdy_Xip = 0.00276249; double edNdy_Xip = 0.000266289;
      double n_Xip  = 13.9646; double en_Xip  = 1.71798;

      double C_KS0  = 0.196422; double eC_KS0  = 0.00376875;
      double dNdy_KS0 = 0.131203; double edNdy_KS0 = 0.00698827;
      double n_KS0  = 12.2662; double en_KS0  = 0.90061;

      // ALICE Fit Values
      double C_Xi_LHC  = 0.566197; double eC_Xi_LHC  = 0.00458052;
      double dNdy_Xi_LHC = 0.183844; double edNdy_Xi_LHC = 0.00159675;
      double n_Xi_LHC  = 10.5974; double en_Xi_LHC  = 0.319702;

      double C_KS0_LHC = 0.424953; double eC_KS0_LHC = 0.00987472;
      double dNdy_KS0_LHC = 1.73686; double edNdy_KS0_LHC = 0.0161898;
      double n_KS0_LHC = 8.15947; double en_KS0_LHC = 0.466203;

      // --- Levy evaluation lambda ---
      auto levy = [](double pT, double m0, double C, double dNdy, double n) {
	return dNdy * (((n-1)*(n-2))/(2*M_PI*n*C*(n*C + m0*(n-2))))
		 * pow(1 + (std::sqrt(pT*pT + m0*m0) - m0)/(n*C), -n);
      };

      auto levyErr = [](double pT,   double m0,
		      double dNdy, double edNdy,
		      double C,    double eC,
		      double n,    double en) -> double
      {
	double mT      = sqrt(pT * pT + m0 * m0);
	double u       = (mT - m0) / (n * C);          // (mT-m0)/(nC)
	double base    = 1.0 + u;                       // 1 + (mT-m0)/(nC)
	double numer   = n * C + m0 * (n - 2);          // nC + m0(n-2)

	// Normalization prefactor A and power term P
	double A = (n - 1) * (n - 2) / (2 * M_PI * n * C * numer);
	double P = pow(base, -n);
	double f = dNdy * A * P;

	// d/d(dNdy) -- simplest, just f/dNdy
	double dfdNdy = f / dNdy;

	// d/dC
	// Three contributions: from A's 1/C, from A's 1/numer * n, from P's base
 	double dfdC = f * (
	  - 1.0 / C
	  - n * n / numer                               // d/dC of 1/(nC+m0(n-2)) * numer factor
	  + n * (mT - m0) / (n * C * C * base)          // d/dC of (1 + (mT-m0)/(nC))^-n
	);

        // d/dn
	// Contributions from A's (n-1), (n-2), n, numer, and from P
	double dPdn = P * (-(mT - m0) / (n * C * base) - log(base));  // d/dn of base^(-n)
	double dAdn = A * (
	  1.0 / (n - 1)
	  + 1.0 / (n - 2)
	  - 1.0 / n
	  - (C + m0) / numer                            // d/dn of nC + m0(n-2) = C + m0
	);
	double dfdn = dNdy * (dAdn * P + A * dPdn);

	return sqrt(pow(dfdNdy * edNdy, 2) +
		    pow(dfdC   * eC,    2) +
		    pow(dfdn   * en,    2));
      };

      // --- Build ratio and uncertainty band ---
      const int    nPts  = 500;
      const double pTmin = 0.5;
      const double pTmax = 4.0;
      const double step  = (pTmax - pTmin) / nPts;

      TGraph*      g_ratio    = new TGraph(nPts);
      TGraphErrors* g_band    = new TGraphErrors(nPts);
      TGraph*      g_ratio_LHC    = new TGraph(nPts);
      TGraphErrors* g_band_LHC    = new TGraphErrors(nPts);

      for (int i = 0; i < nPts; i++)
      {
	double pT = pTmin + i * step;

	double fXim = levy(pT, m0_Xi,  C_Xim,  dNdy_Xim,  n_Xim);
	double fXip = levy(pT, m0_Xi,  C_Xip,  dNdy_Xip,  n_Xip);
	double fKS0 = levy(pT, m0_KS0, C_KS0,  dNdy_KS0,  n_KS0);

        double fXi_LHC = levy(pT, m0_Xi,  C_Xi_LHC,  dNdy_Xi_LHC,  n_Xi_LHC);
        double fKS0_LHC = levy(pT, m0_KS0,  C_KS0_LHC,  dNdy_KS0_LHC,  n_KS0_LHC);

	double numerator   = fXim + fXip;
	double denominator = 2.0 * fKS0;

        double numerator_LHC   = fXi_LHC;
        double denominator_LHC = 2.0 * fKS0_LHC;

	if (denominator <= 0) continue;
	double ratio = numerator / denominator;
	double ratio_LHC = numerator_LHC / denominator_LHC;

        // --- Error propagation on ratio R = (Xim + Xip) / (2*KS0) ---
	// dR/d(Xim) =  1 / denom
	// dR/d(Xip) =  1 / denom
	// dR/d(KS0) = -numerator / denom^2 * 2 = -R / KS0_val
	double eXim = levyErr(pT, m0_Xi,  C_Xim, eC_Xim, dNdy_Xim, edNdy_Xim, n_Xim, en_Xim);
	double eXip = levyErr(pT, m0_Xi,  C_Xip, eC_Xip, dNdy_Xip, edNdy_Xip, n_Xip, en_Xip);
	double eKS0 = levyErr(pT, m0_KS0, C_KS0, eC_KS0, dNdy_KS0, edNdy_KS0, n_KS0, en_KS0);
	double eXi_LHC = levyErr(pT, m0_Xi, C_Xi_LHC, eC_Xi_LHC, dNdy_Xi_LHC, edNdy_Xi_LHC, n_Xi_LHC, en_Xi_LHC);
	double eKS0_LHC = levyErr(pT, m0_KS0, C_KS0_LHC, eC_KS0_LHC, dNdy_KS0_LHC, edNdy_KS0_LHC, n_KS0_LHC, en_KS0_LHC);

	double ratioErr = sqrt(
	  pow(eXim / denominator,       2) +
	  pow(eXip / denominator,       2) +
	  pow(ratio * eKS0 / fKS0,      2)
	);
	
        double ratioErr_LHC = sqrt(
	  pow(eXi_LHC / denominator_LHC,       2) +
	  pow(ratio_LHC * eKS0_LHC / fKS0_LHC,      2)
	);

	g_ratio->SetPoint(i,      pT, ratio);
	g_band->SetPoint(i,       pT, ratio);
	g_band->SetPointError(i,  0,  ratioErr);
	g_ratio_LHC->SetPoint(i,      pT, ratio_LHC);
	g_band_LHC->SetPoint(i,       pT, ratio_LHC);
	g_band_LHC->SetPointError(i,  0,  ratioErr_LHC);
      }

      // Draw band first so curve sits on top
      g_band->SetFillColorAlpha(kRed, 0.3);
      g_band->SetFillStyle(1001);
      g_band->SetLineColorAlpha(kRed, 0.0); // hide band outline
      g_band->Draw("E3 same");                 // E3 = filled uncertainty band

      g_ratio->SetLineColor(kRed);
      g_ratio->SetLineWidth(2);
      g_ratio->Draw("L same");
 
      leg->AddEntry(g_ratio, "STAR PRC 75 (2007) 064901", "l");
      
      g_band_LHC->SetFillColorAlpha(kGreen, 0.3);
      g_band_LHC->SetFillStyle(1001);
      g_band_LHC->SetLineColorAlpha(kGreen, 0.0); // hide band outline
      g_band_LHC->Draw("E3 same");                 // E3 = filled uncertainty band

      g_ratio_LHC->SetLineColor(kGreen);
      g_ratio_LHC->SetLineWidth(2);
      g_ratio_LHC->Draw("L same");
 
      leg->AddEntry(g_ratio_LHC, "ALICE EPJC 81 (2021) 256", "l");

      for (int q = 0; q < 4; ++q)
      {
        double x, y;
        g_stat->GetPoint(q, x, y);
        double stat_err = g_stat->GetErrorY(q);
        double syst_err = g_syst->GetErrorY(q);
        double fXim = levy(x, m0_Xi,  C_Xim,  dNdy_Xim,  n_Xim);
        double fXip = levy(x, m0_Xi,  C_Xip,  dNdy_Xip,  n_Xip);
        double fKS0 = levy(x, m0_KS0, C_KS0,  dNdy_KS0,  n_KS0);
        double fXi_LHC = levy(x, m0_Xi,  C_Xi_LHC,  dNdy_Xi_LHC,  n_Xi_LHC);
        double fKS0_LHC = levy(x, m0_KS0,  C_KS0_LHC,  dNdy_KS0_LHC,  n_KS0_LHC);
        double numerator   = fXim + fXip;
        double denominator = 2.0 * fKS0;
        double numerator_LHC   = fXi_LHC;
        double denominator_LHC = 2.0 * fKS0_LHC;
        double ratio = numerator / denominator;
        double ratio_LHC = numerator_LHC / denominator_LHC;
        double eXim = levyErr(x, m0_Xi,  C_Xim, eC_Xim, dNdy_Xim, edNdy_Xim, n_Xim, en_Xim);
        double eXip = levyErr(x, m0_Xi,  C_Xip, eC_Xip, dNdy_Xip, edNdy_Xip, n_Xip, en_Xip);
        double eKS0 = levyErr(x, m0_KS0, C_KS0, eC_KS0, dNdy_KS0, edNdy_KS0, n_KS0, en_KS0);
        double eXi_LHC = levyErr(x, m0_Xi, C_Xi_LHC, eC_Xi_LHC, dNdy_Xi_LHC, edNdy_Xi_LHC, n_Xi_LHC, en_Xi_LHC);
        double eKS0_LHC = levyErr(x, m0_KS0, C_KS0_LHC, eC_KS0_LHC, dNdy_KS0_LHC, edNdy_KS0_LHC, n_KS0_LHC, en_KS0_LHC);
        double ratioErr = sqrt(
          pow(eXim / denominator,       2) +
          pow(eXip / denominator,       2) +
          pow(ratio * eKS0 / fKS0,      2)
        );
        double ratioErr_LHC = sqrt(
          pow(eXi_LHC / denominator_LHC,       2) +
          pow(ratio_LHC * eKS0_LHC / fKS0_LHC,      2)
        );
        double sigma = std::sqrt(pow(stat_err,2) + pow(syst_err,2) + pow(ratioErr,2));
        double pull = std::abs(ratio - y)/sigma;
        double sigma_LHC = std::sqrt(pow(stat_err,2) + pow(syst_err,2) + pow(ratioErr_LHC,2));
        double pull_LHC = std::abs(ratio_LHC - y)/sigma_LHC;
        std::cout << "pT: " << x << ", pull_STAR = " << pull << ", pull_LHC = " << pull_LHC << std::endl;
      }
    }
    leg->Draw();

    TPaveText *pt;
    pt = new TPaveText(0.75,0.8,0.95,1., "NDC");
    pt->SetFillColor(0);
    pt->SetFillStyle(0);
    pt->SetTextFont(42);
    string label = "#it{#bf{sPHENIX}} Internal";
    TText *pt_LaTex = pt->AddText(label.c_str());
    pt->SetBorderSize(0);
    pt->Draw();
    gPad->Modified();

    //c->SaveAs(("finalRatio_" + kin + ".png").c_str());
  } 
} 

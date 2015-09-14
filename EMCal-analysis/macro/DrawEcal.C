#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

void
DrawEcal(void)
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");

//  DrawSF();

//  DrawCluster_SingleE();
//  DrawCluster_AnaP() ;
  DrawCluster_Res();

}

void
DrawCluster_Res(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0");
  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90");

  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0");
  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90");

  TCanvas *c1 = new TCanvas("DrawCluster", "DrawCluster", 1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 35, 20e-2,
      ";Incoming Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2, 10e-2, 35, 18e-2, NULL, "br");

  p = (TPad *) c1->cd(idx);
  c1->Update();

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  gamma_eta0->Fit(f_calo, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);

  gamma_eta0->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "2D-proj., #eta = 0.0-0.1, #DeltaE/E = %.1f #oplus %.1f%%",
      f_calo->GetParameter(0), f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0, ltitle.Data(), "pl");



  TF1 * f_calo = new TF1("f_calo_gamma_eta9", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  gamma_eta9->Fit(f_calo, "RM0");

  gamma_eta9->SetLineColor(kRed + 3);
  gamma_eta9->SetMarkerColor(kRed + 3);
  gamma_eta9->SetLineWidth(2);
  gamma_eta9->SetMarkerStyle(kFullCircle);
  gamma_eta9->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 3);
  f_calo->SetLineWidth(2);

  gamma_eta9->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "2D-proj., #eta = 0.9-1.0, #DeltaE/E = %.1f #oplus %.1f%%",
      f_calo->GetParameter(0), f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9, ltitle.Data(), "pl");
  cout<<"Title = " <<ltitle<<endl;



  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);

  gamma_eta0_1d->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "1D-proj., #eta = 0.0-0.1, #DeltaE/E = %.1f #oplus %.1f%%",
      f_calo->GetParameter(0), f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0_1d, ltitle.Data(), "pl");



  TF1 * f_calo = new TF1("f_calo_gamma_eta9_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  gamma_eta9_1d->Fit(f_calo, "RM0");

  gamma_eta9_1d->SetLineColor(kBlue + 3);
  gamma_eta9_1d->SetMarkerColor(kBlue + 3);
  gamma_eta9_1d->SetLineWidth(2);
  gamma_eta9_1d->SetMarkerStyle(kOpenCircle);
  gamma_eta9_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 3);
  f_calo->SetLineWidth(2);

  gamma_eta9_1d->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "1D-proj., #eta = 0.9-1.0, #DeltaE/E = %.1f #oplus %.1f%%",
      f_calo->GetParameter(0), f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9_1d, ltitle.Data(), "pl");
  cout<<"Title = " <<ltitle<<endl;






  lg->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

TGraphErrors *
DrawCluster_AnaP(
    const TString config =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX", //
//        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX", //
    const TString particle = "gamma_eta0.90", bool mean_or_res = false)
{
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_10GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_16GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_1GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_2GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_32GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_40GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_4GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_50GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_8GeV.root_hist.root

  const int N = 10;
  double es[100] =
    { 1, 2, 4, 8, 10, 16, 24, 32, 40, 50 };
  double mean[100] =
    { 0 };
  double mean_err[100] =
    { 0 };
  double res[100] =
    { 0 };
  double res_err[100] =
    { 0 };

  for (int i = 0; i < N; ++i)
    {
      TVectorD fit(4);

      fit = DrawCluster_SingleE(config, particle, es[i]);

      mean[i] = fit[0];
      mean_err[i] = fit[1];
      res[i] = fit[2] / mean[i];
      res_err[i] = fit[3] / mean[i];
    }

  if (mean_or_res)
    {
      TGraphErrors * ge = new TGraphErrors(N, es, mean, 0, mean_err);
      return ge;
    }
  else
    {
      TGraphErrors * ge = new TGraphErrors(N, es, res, 0, res_err);
      return ge;
    }

}

TVectorD
DrawCluster_SingleE( //
    const TString config =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX", //
    const TString particle = "gamma_eta0", const double e = 8)
{
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_8GeV.root_g4cemc_eval.root

  const TString e_GeV = Form("%.0fGeV", e);

  const TString file_name = config + "_" + particle + "_" + e_GeV
      + ".root_g4cemc_eval.root";
  +".root_hist.root";
  cout << "DrawCluster_SingleE - lead " << file_name << endl;

  TFile *f = new TFile(file_name);
  assert(f);
  TTree * ntp_cluster = (TTree *) f->GetObjectChecked("ntp_cluster", "TTree");
  assert(ntp_cluster);

  const TString h_cluster_E_name = TString("h_cluster_E_") + particle + e_GeV;
  TH1F * h_cluster_E = new TH1F(h_cluster_E_name, h_cluster_E_name, 200, 0,
      e * 1.5);

  TCanvas *c1 = new TCanvas(file_name, file_name);
  ntp_cluster->Draw("e>>" + h_cluster_E_name, "gparticleID==1 && e>0.2");

  const double mean = h_cluster_E->GetMean();
  const double rms = h_cluster_E->GetRMS();

//  const double lower_lim = mean - ((e >= 8) ? (4 / e * rms) : (1.5 * rms));
//  const double lower_lim = mean - ((e >= 8) ? (rms) : (1.5 * rms));
  TF1 * f_gaus_pilot = new TF1("f_gaus_pilot" + h_cluster_E_name, "gaus", e*0.5,
      e*1.5);
  f_gaus_pilot->SetParameters(1, mean, rms);
  f_gaus_pilot->SetLineColor(kRed);
  h_cluster_E->Fit(f_gaus_pilot, "MRQS0");
  f_gaus_pilot->Draw("same");

  TF1 * f_gaus = new TF1("f_gaus" + h_cluster_E_name, "gaus", f_gaus_pilot->GetParameter(1)-1.5*f_gaus_pilot->GetParameter(2),
      f_gaus_pilot->GetParameter(1)+ 4*f_gaus_pilot->GetParameter(2));
  f_gaus->SetParameters(f_gaus_pilot->GetParameter(0), f_gaus_pilot->GetParameter(1), f_gaus_pilot->GetParameter(2));

  h_cluster_E->Fit(f_gaus, "MRQS0");
  f_gaus->Draw("same");

  cout << "load_SF - result = " << f_gaus->GetParameter(1) << "+/-"
      << f_gaus->GetParError(1) << endl;

  TVectorD ret(4);
  ret[0] = f_gaus->GetParameter(1);
  ret[1] = f_gaus->GetParError(1);
  ret[2] = f_gaus->GetParameter(2);
  ret[3] = f_gaus->GetParError(2);

  return ret;
}

void
DrawSF(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TCanvas *c1 = new TCanvas("DrawSF", "DrawSF", 1800, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(-0.1, 2e-2, 1.2, 3.0e-2,
      ";Pseudorapidity;EMCal Sampling fraction");

  TLegend * lg = new TLegend(0, 2.55e-2, 0.6, 2.9e-2, NULL, "br");

  TGraphErrors * ge = Load_CurveSet(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX_gamma", "24GeV");
  ge->SetLineColor(kRed + 3);
  ge->SetMarkerColor(kRed + 3);
  ge->SetLineWidth(2);
  ge->SetMarkerStyle(kFullSquare);
  ge->SetMarkerSize(2);
  ge->Draw("lep");
  lg->AddEntry(ge, "24 GeV #gamma in 2D-proj. SPACAL", "lp");

  TGraphErrors * ge = Load_CurveSet(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX_gamma", "24GeV");
  ge->SetLineColor(kBlue + 3);
  ge->SetMarkerColor(kBlue + 3);
  ge->SetLineStyle(kDashed);
  ge->SetLineWidth(2);
  ge->SetMarkerStyle(kOpenSquare);
  ge->SetMarkerSize(2);
  ge->Draw("lep");
  lg->AddEntry(ge, "24 GeV #gamma in 1D-proj. SPACAL", "lp");

  TGraphErrors * ge = Load_CurveSet(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX_e-", "4GeV");
  ge->SetLineColor(kRed + 3);
  ge->SetMarkerColor(kRed + 3);
  ge->SetLineWidth(2);
  ge->SetMarkerStyle(kFullCircle);
  ge->SetMarkerSize(2);
  ge->Draw("lep");
  lg->AddEntry(ge, "4 GeV e^{-} in 2D-proj. SPACAL", "lp");

  TGraphErrors * ge = Load_CurveSet(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX_e-", "4GeV");
  ge->SetLineColor(kBlue + 3);
  ge->SetMarkerColor(kBlue + 3);
  ge->SetLineStyle(kDashed);
  ge->SetLineWidth(2);
  ge->SetMarkerStyle(kOpenCircle);
  ge->SetMarkerSize(2);
  ge->Draw("lep");
  lg->AddEntry(ge, "4 GeV e^{-} in 1D-proj. SPACAL", "lp");

  lg->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

TGraphErrors *
Load_CurveSet(
    const TString config =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma",
    const TString energy = "24GeV")
{
  double etas[100] =
    { 0 };
  double SFs[100] =
    { 0 };
  double SF_Errs[100] =
    { 0 };

  int n = 0;
  TVectorD SF(2);

  etas[n] = 0 + 0.05;
  SF = Load_SF("eta0", config, energy);
  SFs[n] = SF[0];
  SF_Errs[n] = SF[1];
  n++;

  etas[n] = 0.3 + 0.05;
  SF = Load_SF("eta0.30", config, energy);
  SFs[n] = SF[0];
  SF_Errs[n] = SF[1];
  n++;

  etas[n] = 0.6 + 0.05;
  SF = Load_SF("eta0.60", config, energy);
  SFs[n] = SF[0];
  SF_Errs[n] = SF[1];
  n++;

  etas[n] = 0.9 + 0.05;
  SF = Load_SF("eta0.90", config, energy);
  SFs[n] = SF[0];
  SF_Errs[n] = SF[1];
  n++;

  TGraphErrors * ge = new TGraphErrors(n, etas, SFs, 0, SF_Errs);

  return ge;

}

TVectorD
Load_SF(const TString eta_bin = "eta0",
    const TString config =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma",
    const TString energy = "24GeV")
{
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma_eta0.30_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma_eta0.60_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma_eta0.90_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma_eta0_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0.30_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0.60_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0.90_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_24GeV.root_hist.root

  const TString file_name = config + "_" + eta_bin + "_" + energy
      + ".root_hist.root";
  cout << "load_SF - lead " << file_name << endl;

  TFile *f = new TFile(file_name);
  assert(f);
  TH1F * EMCalAna_h_CEMC_SF = (TH1F *) f->GetObjectChecked("EMCalAna_h_CEMC_SF",
      "TH1F");
  assert(EMCalAna_h_CEMC_SF);

  const double mean = EMCalAna_h_CEMC_SF->GetMean();
  const double rms = EMCalAna_h_CEMC_SF->GetRMS();

  TF1 * f_gaus = new TF1("f_gaus" + eta_bin, "gaus", mean - 2 * rms,
      mean + 2 * rms);
  f_gaus->SetParameters(1, mean, rms);

  EMCalAna_h_CEMC_SF->Fit(f_gaus, "MRQS0");

  cout << "load_SF - result = " << f_gaus->GetParameter(1) << "+/-"
      << f_gaus->GetParError(1) << endl;

  TVectorD ret(2);
  ret[0] = f_gaus->GetParameter(1);
  ret[1] = f_gaus->GetParError(1);

  return ret;
}

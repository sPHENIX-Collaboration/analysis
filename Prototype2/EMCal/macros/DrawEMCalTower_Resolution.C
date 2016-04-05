// $Id: $                                                                                             

/*!
 * \file Draw.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

void
DrawEMCalTower_Resolution( //
    const TString base =
        "../../sPHENIX_work/Prototype_2016/test_sim/15Degree_1Col_No0Sup/")
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  const int N = 7;
  const double Es[] =
    {1, 2, 4, 8, 12, 16, 24};
  double mean[1000];
  double dmean[1000];
  double res[1000];
  double dres[1000];

  for (int i = 0; i < N; ++i)
    {
      const double E = Es[i];

      vector<double> v(4);

      const TString sE = Form("%.0f", E);
      v = GetOnePlot(base, "e-", sE);

      mean[i] = v[0];
      dmean[i] = v[1];
      res[i] = v[2]/v[0];
      dres[i] = v[3]/v[0];

      cout <<"mean[i] = "<<mean[i] <<", "//
          <<"dmean[i] = "<<dmean[i] <<", "//
          <<"res[i] = "<<res[i] <<", "//
          <<"dres[i] = "<<dres[i] <<endl;
    }

  TGraphErrors * ge_linear = new TGraphErrors(N, Es, mean, 0, dmean);
  TGraphErrors * ge_res = new TGraphErrors(N, Es, res, 0, dres);

  ge_linear->SetLineColor(kBlue + 3);
  ge_linear->SetMarkerColor(kBlue + 3);
  ge_linear->SetLineWidth(2);
  ge_linear->SetMarkerStyle(kFullCircle);
  ge_linear->SetMarkerSize(1.5);

  ge_res->SetLineColor(kBlue + 3);
  ge_res->SetMarkerColor(kBlue + 3);
  ge_res->SetLineWidth(2);
  ge_res->SetMarkerStyle(kFullCircle);
  ge_res->SetMarkerSize(1.5);

  TF1 * f_calo_r = new TF1("f_calo_r", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l", "pol2", 0.5, 60);

  TText * t;
  TCanvas *c1 = new TCanvas("DrawEMCalTower_Resolution",
      "DrawEMCalTower_Resolution", 1300, 600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 30, 30,
      Form("Linearity;Input energy (GeV);Measured Energy (GeV)",
          E));
  TLine * l = new TLine(0, 0, 30, 30);
  l->SetLineColor(kGray);
  l->Draw();

  ge_linear->Draw("p");
  ge_linear->Fit(f_calo_l, "RM0");
  f_calo_l->Draw("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * hframe = p->DrawFrame(0, 0, 30, 0.2,
      Form("Resolution;Input energy (GeV);#DeltaE/<E>",
          E));

  ge_res->Draw("p");
  ge_res->Fit(f_calo_r, "RM0");
  f_calo_r->Draw("same");

  hframe->SetTitle(Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo_r->GetParameter(0),
      f_calo_r->GetParameter(1)));

  SaveCanvas(c1, base + TString(c1->GetName()), kTRUE);
}

vector<double>
GetOnePlot(const TString base, const TString particle, const TString sE)
{

  TString fname = base + TString("Prototype_") + particle + "_" + sE
      + "_SegALL_DSTReader.root_DrawEMCalTower_EMCDistribution_SUM_all_event.root";
//  Prototype_e-_2_Seg0_DSTReader.root_DrawEMCalTower_EMCDistribution_SUMall_event.root

  cout <<"Process "<<fname<<endl;
  TFile * f = new TFile(fname);
  assert(f->IsOpen());

  TH1 * hEnergySum = (TH1 *) f->GetObjectChecked("EnergySum_LG", "TH1");
  assert(hEnergySum);
  new TCanvas();
  hEnergySum->DrawClone();

  hEnergySum->Scale(1. / hEnergySum->Integral(1, -1));
  TF1 * fgaus = hEnergySum->GetFunction("fgaus_LG");
  assert(fgaus);


  vector<double> v(4);
  v[0] = fgaus->GetParameter(1);
  v[1] = fgaus->GetParError(1);
  v[2] = fgaus->GetParameter(2);
  v[3] = fgaus->GetParError(2);

  return v;
}

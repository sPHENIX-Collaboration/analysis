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

//#include "Prototype2_DSTReader.h"

TCut event_sel;
TString cuts;
TFile * _file0 = NULL;
TTree * T = NULL;

class lin_res
{
public:
  TString name;
  TGraphErrors * linearity;
  TGraphErrors * resolution;
  TF1 * f_res;
};

void
DrawPrototype2ShowerCalib_Sum( //
)
{

  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libPrototype2.so");
  gSystem->Load("libProto2ShowCalib.so");

  RejectionCompare(4);
  RejectionCompare(8);
  RejectionCompare(12);

//  LineShapeCompare(4, "QGSP_BERT_HP");
//  LineShapeCompare(8, "QGSP_BERT_HP");
////  LineShapeCompare(12, "QGSP_BERT_HP");
//  LineShapeCompare(8, "QGSP_BERT");
//  LineShapeCompare(8, "FTFP_BERT_HP");
//  LineShapeCompare(8, "FTFP_BERT");

//  LineShapeCompare_Electron(4, "QGSP_BERT_HP");
//  LineShapeCompare_Electron(8, "QGSP_BERT_HP");
//  LineShapeCompare_Electron(12, "QGSP_BERT_HP");

//  LineShapeCompare(8, "BirkConst0.151");
//  LineShapeCompare(8, "BirkConst0.18");
//    LineShapeCompare_Electron(8, "BirkConst0.151");

}

double
GetMu2Pi(const double E)
{
  double mu2pi = 0;
  if (E == 4)
    {
      mu2pi = 0.02 / 0.105;
    }

  else if (E == 8)
    {
      mu2pi = 0.02 / 0.28;
    }

  else if (E == 12)
    {
      mu2pi = 0.04 / 0.48;
    }
  else
    {
      cout << "GetMu2Pi - missing mu2pi data!!" << endl;
      assert(0);
    }

  return mu2pi;
}

void
RejectionCompare(const double E = 4)
{

  double mu2pi = GetMu2Pi(E);

  TString energy(Form("%.0f", E));

  TString data_file(
      "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/ShowerCalib/Tilt0.lst_EMCalCalib.root_DrawPrototype2ShowerCalib_LineShapeData_Neg"
          + energy + "GeV_quality_h12345_v34567_col2_row2.root");

  TFile * fdata = new TFile(data_file);
  assert(fdata->IsOpen());

  TFile * fmu =
      new TFile(
          "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/EMCal_sim/./0Degree_1Col_LightCollectionSeanStoll/Prototype_mu-_"
              + energy
              + "_SegALL_EMCalCalib.root_DrawPrototype2ShowerCalib_LineShapeSim_0DegreeRot_h5_v5_col2_row2.root");
  assert(fmu->IsOpen());
  cout << "Processing " << fmu->GetName() << endl;

  TH1F * h_5x5sum_c2_sum_mu = fmu->Get("h_5x5sum_c2_sum")->Clone(
      "h_5x5sum_c2_sum_mu");

  int idx = 1;
  TPad * p;

  TH1F * h_5x5sum_c2_h = fdata->Get("h_5x5sum_c2_h");

  TH1F * h_5x5sum_c2_sum_data_sub_mu = h_5x5sum_c2_h->Clone(
      "h_5x5sum_c2_sum_data_sub_mu");

  cout << "h_5x5sum_c2_sum_data->GetNbinsX() = "
      << h_5x5sum_c2_sum_data_sub_mu->GetNbinsX() << endl;
  cout << "h_5x5sum_c2_sum_mu->GetNbinsX() = "
      << h_5x5sum_c2_sum_mu->GetNbinsX() << endl;

  assert(
      h_5x5sum_c2_sum_data_sub_mu->GetNbinsX()
          == h_5x5sum_c2_sum_mu->GetNbinsX());

  const double muon_count = h_5x5sum_c2_sum_data_sub_mu->GetSum()
      * (mu2pi / (1 + mu2pi));
  h_5x5sum_c2_sum_data_sub_mu->Add(h_5x5sum_c2_sum_mu,
      -muon_count / h_5x5sum_c2_sum_mu->GetSum());

  TGraphErrors * ge_data = Distribution2Efficiency(h_5x5sum_c2_sum_data_sub_mu);
  ge_data->SetFillColor(kGray);
  ge_data->SetLineWidth(3);

  TGraphErrors * ge_pi_QGSP_BERT_HP = GetSimRejCurve("QGSP_BERT_HP", energy,
      "pi-");
  TGraphErrors * ge_kaon_QGSP_BERT_HP = GetSimRejCurve("QGSP_BERT_HP", energy,
      "kaon-");

  ge_pi_QGSP_BERT_HP->SetLineColor(kBlue);
  ge_kaon_QGSP_BERT_HP->SetLineColor(kCyan + 3);

  TGraphErrors * ge_pi_FTFP_BERT_HP = GetSimRejCurve("FTFP_BERT_HP", energy,
      "pi-");
  TGraphErrors * ge_kaon_FTFP_BERT_HP = GetSimRejCurve("FTFP_BERT_HP", energy,
      "kaon-");

  ge_pi_FTFP_BERT_HP->SetLineColor(kBlue);
  ge_kaon_FTFP_BERT_HP->SetLineColor(kCyan + 3);
  ge_pi_FTFP_BERT_HP->SetLineStyle(kDashed);
  ge_kaon_FTFP_BERT_HP->SetLineStyle(kDashed);

  TGraphErrors * ge_pi_BirkConst18 = GetSimRejCurve("BirkConst0.18", energy,
      "pi-");
  TGraphErrors * ge_kaon_BirkConst18 = GetSimRejCurve("BirkConst0.18", energy,
      "kaon-");

  ge_pi_BirkConst18->SetLineColor(kBlue);
  ge_kaon_BirkConst18->SetLineColor(kCyan + 3);
  ge_pi_BirkConst18->SetLineStyle(9);
  ge_kaon_BirkConst18->SetLineStyle(9);

//  new TCanvas;
//  h_5x5sum_c2_sum_data_sub_mu->Draw();
//  h_5x5sum_c2_sum_mu->Draw("same");

  TGraphErrors * ge_ref = ge_pi_QGSP_BERT_HP->Clone("ge_ref");

  for (int i = 0; i < ge_ref->GetN(); ++i)
    {
      (ge_ref->GetY())[i] = ((ge_pi_QGSP_BERT_HP->GetY())[i]
          + (ge_kaon_QGSP_BERT_HP->GetY())[i]) / 2.;
    }

  TText * t;
  TCanvas *c1 = new TCanvas("RejectionCompare_" + energy + "GeV_",
      "RejectionCompare_" + energy + "GeV_", 800, 1000);

  c1->Divide(1, 2);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->SetBottomMargin(0.03);

  TH1 * hframe = p->DrawFrame(0, 5e-4, E * 1, 1);
  hframe->SetTitle(
      Form(
          ";;1/(Hadron Rejection)",
          E));
  hframe->GetXaxis()->SetLabelSize(.06);
  hframe->GetXaxis()->SetTitleSize(.06);
  hframe->GetYaxis()->SetLabelSize(.06);
  hframe->GetYaxis()->SetTitleSize(.06);
  hframe->GetYaxis()->SetTitleOffset(0.8);

  ge_data->Draw("p3");
  ge_data->Draw("lX");

  ge_pi_QGSP_BERT_HP->Draw("lX");
  ge_kaon_QGSP_BERT_HP->Draw("lX");
  ge_pi_FTFP_BERT_HP->Draw("lX");
  ge_kaon_FTFP_BERT_HP->Draw("lX");
  ge_pi_BirkConst18->Draw("lX");
  ge_kaon_BirkConst18->Draw("lX");

  TLegend * leg = new TLegend(.12, .05, .72, .5);
  leg->SetHeader(Form("Beam momentum = -%.0f GeV/c",E));
  leg->AddEntry(ge_data, "T-1044 non-e^{-}  Data - Muon Sim.", "fl");
  leg->AddEntry(ge_pi_QGSP_BERT_HP, "\pi^{-}, QGSP_BERT_HP (Default)", "l");
  leg->AddEntry(ge_kaon_QGSP_BERT_HP, "K^{-}, QGSP_BERT_HP (Default)", "l");
  leg->AddEntry(ge_pi_FTFP_BERT_HP, "\pi^{-}, FTFP_BERT_HP", "l");
  leg->AddEntry(ge_kaon_FTFP_BERT_HP, "K^{-}, FTFP_BERT_HP", "l");
  leg->AddEntry(ge_pi_BirkConst18, "\pi^{-}, k_{B} = 0.18 mm/MeV", "l");
  leg->AddEntry(ge_kaon_BirkConst18, "K^{-}, k_{B} = 0.18 mm/MeV", "l");

  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetTopMargin(0);

  TH1 * hframe =
  p->DrawFrame(0, 0, E * 1, 2);
  hframe->SetTitle(
      Form(
          ";Minimal cut on 5x5 Cluster Energy (GeV/c);Ratio of 1/(Hadron Rejection) and Ref.",
          E));
  hframe->GetXaxis()->SetLabelSize(.06);
  hframe->GetXaxis()->SetTitleSize(.06);
  hframe->GetYaxis()->SetLabelSize(.06);
  hframe->GetYaxis()->SetTitleSize(.06);
  hframe->GetYaxis()->SetTitleOffset(0.8);

  TGraphErrors * ge_data_ratio = GetTGraphRatio(ge_data, ge_ref);

  ge_data_ratio->Draw("p3");
  ge_data_ratio->Draw("lX");

  GetTGraphRatio(ge_pi_QGSP_BERT_HP, ge_ref)->Draw("lX");
  GetTGraphRatio(ge_kaon_QGSP_BERT_HP, ge_ref)->Draw("lX");
  GetTGraphRatio(ge_pi_FTFP_BERT_HP, ge_ref)->Draw("lX");
  GetTGraphRatio(ge_kaon_FTFP_BERT_HP, ge_ref)->Draw("lX");
  GetTGraphRatio(ge_pi_BirkConst18, ge_ref)->Draw("lX");
  GetTGraphRatio(ge_kaon_BirkConst18, ge_ref)->Draw("lX");

  SaveCanvas(c1,
      data_file + "_DrawPrototype2ShowerCalib_Sum" + TString(c1->GetName()),
      kTRUE);
}

TGraphErrors *
GetSimRejCurve(const TString physics_lst, const TString energy,
    const TString particle)
{
  const TString filename =
      "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/EMCal_sim/./0Degree_1Col_LightCollectionSeanStoll_"
          + physics_lst + "/Prototype_" + particle + "_" + energy
          + "_SegALL_EMCalCalib.root_DrawPrototype2ShowerCalib_LineShapeSim_0DegreeRot_h5_v5_col2_row2.root";
  cout << "GetSimRejCurve - processing " << filename << endl;

  TFile * f = new TFile(filename);
  assert(f->IsOpen());

  TGraphErrors * ge = Distribution2Efficiency(
      (TH1F *) f->Get("h_5x5sum_c2_sum"));
//  ge->SetLineColor(kBlue);
  ge->SetLineWidth(3);

  return ge;
}

void
LineShapeCompare(const double E = 4, //
    const TString physics_lst = "QGSP_BERT_HP")
{

  double mu2pi = GetMu2Pi(E);

  TString energy(Form("%.0f", E));

  TText * t;
  TCanvas *c1 = new TCanvas("LineShapeCompare_" + energy + "GeV_" + physics_lst,
      "LineShapeCompare_" + energy + "GeV_" + physics_lst, 1000, 650);

  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TString data_file(
      "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/ShowerCalib/Tilt0.lst_EMCalCalib.root_DrawPrototype2ShowerCalib_LineShapeData_Neg"
          + energy + "GeV_quality_h12345_v34567_col2_row2.root");

  TFile * fdata = new TFile(data_file);
  assert(fdata->IsOpen());

  TH1F * h_5x5sum_c2_sum_data = fdata->Get("h_5x5sum_c2_h3")->Clone(
      "h_5x5sum_c2_sum_data");
  assert(h_5x5sum_c2_sum_data);

  h_5x5sum_c2_sum_data->Scale(1. / h_5x5sum_c2_sum_data->GetSum());
  h_5x5sum_c2_sum_data->SetLineColor(kBlack);
//  h_5x5sum_c2_sum_data->SetLineStyle(kDashed);

  h_5x5sum_c2_sum_data->Draw();

  TFile * fpi =
      new TFile(
          "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/EMCal_sim/./0Degree_1Col_LightCollectionSeanStoll_"
              + physics_lst + "/Prototype_pi-_" + energy
              + "_SegALL_EMCalCalib.root_DrawPrototype2ShowerCalib_LineShapeSim_0DegreeRot_h5_v5_col2_row2.root");
  assert(fpi->IsOpen());

  TH1F * h_5x5sum_c2_sum_pi = fpi->Get("h_5x5sum_c2_sum")->Clone(
      "h_5x5sum_c2_sum_pi");
  assert(h_5x5sum_c2_sum_pi);

  h_5x5sum_c2_sum_pi->Scale((1. / (1 + mu2pi)) / h_5x5sum_c2_sum_pi->GetSum());
  h_5x5sum_c2_sum_pi->SetLineColor(kBlue);
//  h_5x5sum_c2_sum_pi->SetLineStyle(kDashed);

  h_5x5sum_c2_sum_pi->Draw("same");

  TFile * fkaon =
      new TFile(
          "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/EMCal_sim/./0Degree_1Col_LightCollectionSeanStoll_"
              + physics_lst + "/Prototype_kaon-_" + energy
              + "_SegALL_EMCalCalib.root_DrawPrototype2ShowerCalib_LineShapeSim_0DegreeRot_h5_v5_col2_row2.root");
  assert(fkaon->IsOpen());

  TH1F * h_5x5sum_c2_sum_kaon = fkaon->Get("h_5x5sum_c2_sum")->Clone(
      "h_5x5sum_c2_sum_kaon");
  assert(h_5x5sum_c2_sum_kaon);

  h_5x5sum_c2_sum_kaon->Scale(
      (1. / (1 + mu2pi)) / h_5x5sum_c2_sum_kaon->GetSum());
  h_5x5sum_c2_sum_kaon->SetLineColor(kCyan + 3);
//  h_5x5sum_c2_sum_pi->SetLineStyle(kDashed);

  h_5x5sum_c2_sum_kaon->Draw("same");

  TFile * fmu =
      new TFile(
          "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/EMCal_sim/./0Degree_1Col_LightCollectionSeanStoll_"
              + physics_lst + "/Prototype_mu-_" + energy
              + "_SegALL_EMCalCalib.root_DrawPrototype2ShowerCalib_LineShapeSim_0DegreeRot_h5_v5_col2_row2.root");
  assert(fmu->IsOpen());
  cout << "Processing " << fmu->GetName() << endl;

  TH1F * h_5x5sum_c2_sum_mu = fmu->Get("h_5x5sum_c2_sum")->Clone(
      "h_5x5sum_c2_sum_mu");
  assert(h_5x5sum_c2_sum_mu);

  h_5x5sum_c2_sum_mu->Scale(
      (mu2pi / (1 + mu2pi)) / h_5x5sum_c2_sum_mu->GetSum());
  h_5x5sum_c2_sum_mu->SetLineColor(kBlack);
//  h_5x5sum_c2_sum_pi->SetLineStyle(kDashed);

  h_5x5sum_c2_sum_mu->Draw("same");

  c1->Update();

  SaveCanvas(c1,
      data_file + "_DrawPrototype2ShowerCalib_Sum" + TString(c1->GetName()),
      kTRUE);

}

void
LineShapeCompare_Electron(const double E = 4, //
    const TString physics_lst = "QGSP_BERT_HP")
{

  TString energy(Form("%.0f", E));

  TText * t;
  TCanvas *c1 = new TCanvas(
      "LineShapeCompare_Electron_" + energy + "GeV_" + physics_lst,
      "LineShapeCompare_Electron_" + energy + "GeV_" + physics_lst, 1000, 650);

  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TString data_file(
      "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/ShowerCalib/Tilt0.lst_EMCalCalib.root_DrawPrototype2ShowerCalib_LineShapeData_Neg"
          + energy + "GeV_quality_h3_v5_col2_row2.root");

  TFile * fdata = new TFile(data_file);
  assert(fdata->IsOpen());

  TH1F * h_5x5sum_c2_sum_data = fdata->Get("h_5x5sum_c2_e");
  assert(h_5x5sum_c2_sum_data);

  h_5x5sum_c2_sum_data->Scale(1. / h_5x5sum_c2_sum_data->GetSum());
  h_5x5sum_c2_sum_data->SetLineColor(kBlack);
//  h_5x5sum_c2_sum_data->SetLineStyle(kDashed);

  h_5x5sum_c2_sum_data->Draw();

  TFile * fe =
      new TFile(
          "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/EMCal_sim/./0Degree_1Col_LightCollectionSeanStoll_"
              + physics_lst + "/Prototype_e-_" + energy
              + "_SegALL_EMCalCalib.root_DrawPrototype2ShowerCalib_LineShapeSim_0DegreeRot_h1_v1_col2_row2.root");
  assert(fe->IsOpen());

  TH1F * h_5x5sum_c2_sum_e = fe->Get("h_5x5sum_c2_sum");
  assert(h_5x5sum_c2_sum_e);

  h_5x5sum_c2_sum_e->Scale((0.27 / 0.30) / h_5x5sum_c2_sum_e->GetSum());
  h_5x5sum_c2_sum_e->SetLineColor(kBlue + 3);
//  h_5x5sum_c2_sum_e->SetLineStyle(kDashed);

  h_5x5sum_c2_sum_e->Draw("same");

  c1->Update();

  SaveCanvas(c1,
      data_file + "_DrawPrototype2ShowerCalib_Sum" + TString(c1->GetName()),
      kTRUE);
}

TGraphErrors *
FitProfile(const TH2 * h2)
{

  TProfile * p2 = h2->ProfileX();

  int n = 0;
  double x[1000];
  double ex[1000];
  double y[1000];
  double ey[1000];

  for (int i = 1; i <= h2->GetNbinsX(); i++)
    {
      TH1D * h1 = h2->ProjectionY(Form("htmp_%d", rand()), i, i);

      if (h1->GetSum() < 30)
        {
          cout << "FitProfile - ignore bin " << i << endl;
          continue;
        }
      else
        {
          cout << "FitProfile - fit bin " << i << endl;
        }

      TF1 fgaus("fgaus", "gaus", -p2->GetBinError(i) * 4,
          p2->GetBinError(i) * 4);

      TF1 f2(Form("dgaus"), "gaus + [3]*exp(-0.5*((x-[1])/[4])**2) + [5]",
          -p2->GetBinError(i) * 4, p2->GetBinError(i) * 4);

      fgaus.SetParameter(1, p2->GetBinContent(i));
      fgaus.SetParameter(2, p2->GetBinError(i));

      h1->Fit(&fgaus, "MQ");

      f2.SetParameters(fgaus.GetParameter(0) / 2, fgaus.GetParameter(1),
          fgaus.GetParameter(2), fgaus.GetParameter(0) / 2,
          fgaus.GetParameter(2) / 4, 0);

      h1->Fit(&f2, "MQ");

//      new TCanvas;
//      h1->Draw();
//      fgaus.Draw("same");
//      break;

      x[n] = p2->GetBinCenter(i);
      ex[n] = (p2->GetBinCenter(2) - p2->GetBinCenter(1)) / 2;
      y[n] = fgaus.GetParameter(1);
      ey[n] = fgaus.GetParError(1);

//      p2->SetBinContent(i, fgaus.GetParameter(1));
//      p2->SetBinError(i, fgaus.GetParameter(2));

      n++;
      delete h1;
    }

  return new TGraphErrors(n, x, y, ex, ey);
}

TGraphErrors *
Distribution2Efficiency(TH1F * hCEMC3_Max)
{
  double threshold[10000] =
    { 0 };
  double eff[10000] =
    { 0 };
  double eff_err[10000] =
    { 0 };

  assert(hCEMC3_Max->GetNbinsX() < 10000);

  const double n = hCEMC3_Max->GetSum();
  double pass = 0;
  int cnt = 0;
  for (int i = hCEMC3_Max->GetNbinsX(); i >= 1; i--)
    {
      pass += hCEMC3_Max->GetBinContent(i);

      const double pp = pass / n;
//      const double z = 1.96;
      const double z = 1.;

      const double A = z * sqrt(1. / n * pp * (1 - pp) + z * z / 4 / n / n);
      const double B = 1 / (1 + z * z / n);

      threshold[cnt] = hCEMC3_Max->GetBinCenter(i);
      eff[cnt] = (pp + z * z / 2 / n) * B;
      eff_err[cnt] = A * B;

//      cout << threshold[cnt] << ": " << "CL " << eff[cnt] << "+/-"
//          << eff_err[cnt] << endl;
      cnt++;
    }
  TGraphErrors * ge = new TGraphErrors(cnt, threshold, eff, NULL, eff_err);
  ge->SetName(TString("ge_") + hCEMC3_Max->GetName());
  return ge;
}

TGraphErrors *
GetTGraphRatio(TGraphErrors *input, TGraphErrors * baseline)
{
  assert(input);
  assert(baseline);

  assert(input->GetN() == baseline->GetN());

  TGraphErrors * ge = input->Clone(TString(input->GetName()) + "_Ratio");

  for (int i = 0; i < input->GetN(); ++i)
    {
      (ge->GetY())[i] /= (baseline->GetY())[i];
      (ge->GetEY())[i] /= (baseline->GetY())[i];
    }

  return ge;
}


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

TFile * _file0 = NULL;
TTree * T = NULL;
TString cuts = "";

void
DrawEMCalTower( //
    const TString infile = // "data/Prototype2_CEMC.root_DSTReader.root" //
//        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/EMCal_sim/15Degree_1Col_LightCollection//Prototype_e-_16_SegALL_DSTReader.root"//
//        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/EMCal_sim/./0Degree_1Col_LightCollectionSeanStoll_CrackScan/Prototype_e-_8_SegALL_DSTReader.root"//
//        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/EMCal_sim/./10DegreeRot_1Col_LightCollectionSeanStoll_CrackScan/Prototype_e-_8_SegALL_DSTReader.root"//
"/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2017/sim/MuonCalib//1k_Muon_DSTReader.root"
)
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");
  gSystem->Load("libqa_modules.so");

  if (!_file0)
    {
      TString chian_str = infile;
      chian_str.ReplaceAll("ALL", "*");

      TChain * t = new TChain("T");
      const int n = t->Add(chian_str);

      cout << "Loaded " << n << " root files with " << chian_str << endl;
      assert(n > 0);

      T = t;

      _file0 = new TFile;
      _file0->SetName(infile);
    }

  assert(_file0);

  T->SetAlias("ActiveTower_LG",
      "TOWER_CALIB_LG_CEMC[].get_binphi()<8 && TOWER_CALIB_LG_CEMC[].get_bineta()<8");
  T->SetAlias("EnergySum_LG",
      "1*Sum$(TOWER_CALIB_LG_CEMC[].get_energy() * ActiveTower_LG)");

  T->SetAlias("ActiveTower_HG",
      "TOWER_CALIB_HG_CEMC[].get_binphi()<8 && TOWER_CALIB_HG_CEMC[].get_bineta()<8");
  T->SetAlias("EnergySum_HG",
      "1*Sum$(TOWER_CALIB_HG_CEMC[].get_energy() * ActiveTower_HG)");

//
  const TCut event_sel = "1*1";
  cuts = "_all_event";
//  const TCut event_sel = "Entry$==3";
//  cuts = "ev3";
//  const TCut event_sel = "Entry$<2";
//  cuts = "2ev";
//  const TCut event_sel = "Entry$<1000";
//  cuts = "1000ev";

  cout << "Build event selection of " << (const char *) event_sel << endl;

  T->Draw(">>EventList", event_sel);
  TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected"
      << endl;

  T->SetEventList(elist);

  EMCDistribution_Fast("HG");
//  EMCDistribution_Fast("LG", true);
//  EMCDistributionVSBeam_SUM();
//  EMCDistributionVSBeam_SUM("TOWER_CEMC", -0, 5); // 0 degree tilted
//  EMCDistributionVSBeam_SUM("TOWER_CEMC", -15); // 10 degree tilted
//  EMCDistributionVSBeam_SUM("TOWER_CEMC", -15, 5); // 10 degree tilted
//  EMCDistribution_SUM();
}

void
EMCDistribution_Fast(TString gain = "LG", bool log_scale = false)
{
  TString hname = "EMCDistribution_" + gain + TString(log_scale ? "_Log" : "")
      + cuts;

  TH2 * h2 = NULL;
  if (log_scale)
    {
      h2 = new TH2F(hname,
          Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 300, 10e-3,
          100, 64, -.5, 63.5);
      QAHistManagerDef::useLogBins(h2->GetXaxis());
    }
  else
    {
      h2 = new TH2F(hname,
          Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 100, -.050,
          .5, 64, -.5, 63.5);
    }

  T->Draw(
      "TOWER_CALIB_" + gain + "_CEMC[].get_bineta() + 8* TOWER_CALIB_" + gain
          + "_CEMC[].get_binphi():TOWER_CALIB_" + gain
          + "_CEMC[].get_energy()>>" + hname, "", "goff");
  TText * t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_" + gain + TString(log_scale ? "_Log" : "") + cuts,
      "EMCDistribution_" + gain + TString(log_scale ? "_Log" : "") + cuts, 1800,
      950);
  c1->Divide(8, 8, 0., 0.01);
  int idx = 1;
  TPad * p;

  for (int iphi = 8 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 8; ieta++)
        {
          p = (TPad *) c1->cd(idx++);
          c1->Update();

          if (log_scale)
            {
              p->SetLogy();
              p->SetLogx();
            }
          p->SetGridx(0);
          p->SetGridy(0);

          TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi)
              + TString(log_scale ? "_Log" : "");

          TH1 * h = h2->ProjectionX(hname, ieta + 8 * iphi + 1,
              ieta + 8 * iphi + 1); // axis bin number is encoded as ieta+8*iphi+1

          h->SetLineWidth(0);
          h->SetLineColor(kBlue + 3);
          h->SetFillColor(kBlue + 3);
          h->GetXaxis()->SetTitleSize(.09);
          h->GetXaxis()->SetLabelSize(.08);
          h->GetYaxis()->SetLabelSize(.08);

          h->Draw();

          TText *t = new TText(.9, .9, Form("Col%d Row%d", ieta, iphi));
          t->SetTextAlign(33);
          t->SetTextSize(.15);
          t->SetNDC();
          t->Draw();
        }
    }

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEMCalTower_")
          + TString(c1->GetName()), kTRUE);

}

void
EMCDistribution(TString gain = "LG", bool log_scale = false)
{

  TText * t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_" + gain + TString(log_scale ? "_Log" : "") + cuts,
      "EMCDistribution_" + gain + TString(log_scale ? "_Log" : "") + cuts, 1800,
      1000);
  c1->Divide(8, 8, 0., 0.01);
  int idx = 1;
  TPad * p;

  for (int iphi = 8 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 8; ieta++)
        {
          p = (TPad *) c1->cd(idx++);
          c1->Update();

          if (log_scale)
            {
              p->SetLogy();
              p->SetLogx();
            }
          p->SetGridx(0);
          p->SetGridy(0);

          TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi)
              + TString(log_scale ? "_Log" : "");

          TH1 * h = NULL;

          if (log_scale)
            h = new TH1F(hname,
                Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 300,
                5e-3, 100);
          else
            h = new TH1F(hname,
                Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 100,
                -.050, .5);

          h->SetLineWidth(0);
          h->SetLineColor(kBlue + 3);
          h->SetFillColor(kBlue + 3);
          h->GetXaxis()->SetTitleSize(.09);
          h->GetXaxis()->SetLabelSize(.08);
          h->GetYaxis()->SetLabelSize(.08);

          if (log_scale)
            QAHistManagerDef::useLogBins(h->GetXaxis());

          T->Draw("TOWER_CALIB_" + gain + "_CEMC[].get_energy()>>" + hname,
              Form(
                  "TOWER_CALIB_%s_CEMC[].get_bineta()==%d && TOWER_CALIB_%s_CEMC[].get_binphi()==%d",
                  gain.Data(), ieta, gain.Data(), iphi), "");

          TText *t = new TText(.9, .9, Form("Col%d Row%d", ieta, iphi));
          t->SetTextAlign(33);
          t->SetTextSize(.15);
          t->SetNDC();
          t->Draw();
        }
    }

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEMCalTower_")
          + TString(c1->GetName()), kTRUE);

}
void
EMCDistribution_SUM(TString sTOWER = "TOWER_CEMC")
{
  TH1 * EnergySum_LG = new TH1F("EnergySum_LG",
      ";Low-gain Tower Energy Sum (GeV);Count / bin", 1500, 0, 100);
  TH1 * EnergySum_HG = new TH1F("EnergySum_HG",
      ";High-gain Tower Energy Sum (GeV);Count / bin", 300, 0, 3);

  T->Draw("EnergySum_LG>>EnergySum_LG", "", "goff");
  T->Draw("EnergySum_HG>>EnergySum_HG", "", "goff");

  TText * t;
  TCanvas *c1 = new TCanvas("EMCDistribution_SUM" + cuts,
      "EMCDistribution_SUM" + cuts, 1000, 960);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * h = (TH1 *) EnergySum_LG->DrawClone();
  h->SetLineWidth(2);
  h->SetLineColor(kBlue + 3);
//  h->Sumw2();
  h->GetXaxis()->SetRangeUser(0, h->GetMean() + 5 * h->GetRMS());

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * h = (TH1 *) EnergySum_LG->DrawClone();

  TF1 * fgaus = new TF1("fgaus_LG", "gaus", 0, 100);
  fgaus->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
      h->GetMean() + 2 * h->GetRMS());
  h->Fit(fgaus, "M");

  h->Sumw2();
  h->GetXaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
      h->GetMean() + 4 * h->GetRMS());

  h->SetLineWidth(2);
  h->SetMarkerStyle(kFullCircle);

  h->SetTitle(
      Form("#DeltaE/<E> = %.1f%%",
          100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * h = (TH1 *) EnergySum_HG->DrawClone();
  h->SetLineWidth(2);
  h->SetLineColor(kBlue + 3);
//  h->Sumw2();
  h->GetXaxis()->SetRangeUser(0, h->GetMean() + 5 * h->GetRMS());

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * h = (TH1 *) EnergySum_HG->DrawClone();

  TF1 * fgaus = new TF1("fgaus_HG", "gaus", 0, 100);
  fgaus->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
      h->GetMean() + 2 * h->GetRMS());
  h->Fit(fgaus, "M");

  h->Sumw2();
  h->GetXaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
      h->GetMean() + 4 * h->GetRMS());

  h->SetLineWidth(2);
  h->SetMarkerStyle(kFullCircle);

  h->SetTitle(
      Form("#DeltaE/<E> = %.1f%%",
          100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEMCalTower_")
          + TString(c1->GetName()), kTRUE);

}

void
EMCDistributionVSBeam_SUM(TString sTOWER = "TOWER_CEMC", const double z_shift =
    0, const int n_div = 1)
{
  TH3F * EnergySum_LG3 =
      new TH3F("EnergySum_LG3",
          ";Beam Horizontal Pos (cm);Beam Horizontal Vertical (cm);Low-gain Tower Energy Sum (GeV)", //
          20 * n_div, z_shift - 5, z_shift + 5, //
          20 * n_div, -5, 5, //
          200, 0, 20);

  T->Draw("EnergySum_LG:PHG4VtxPoint.vy:PHG4VtxPoint.vz>>EnergySum_LG3", "",
      "goff");

  TProfile2D * EnergySum_LG3_xy = EnergySum_LG3->Project3DProfile("yx");
  TH2 * EnergySum_LG3_zx = EnergySum_LG3->Project3D("zx");
  TH2 * EnergySum_LG3_zy = EnergySum_LG3->Project3D("zy");

  TGraphErrors * ge_EnergySum_LG3_zx = FitProfile(EnergySum_LG3_zx);
  TGraphErrors * ge_EnergySum_LG3_zy = FitProfile(EnergySum_LG3_zy);

  TText * t;
  TCanvas *c1 = new TCanvas(
      TString(Form("EMCDistributionVSBeam_SUM_NDiv%d", n_div)) + cuts,
      TString(Form("EMCDistributionVSBeam_SUM_NDiv%d", n_div)) + cuts, 1000,
      960);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_xy->Draw("colz");
  EnergySum_LG3_xy->SetTitle(
      "Position scan;Beam Horizontal Pos (cm);Beam Horizontal Vertical (cm)");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_zx->Draw("colz");
  EnergySum_LG3_zx->SetTitle(
      "Position scan;Beam Horizontal Pos (cm);Energy Sum (GeV)");

  ge_EnergySum_LG3_zx->SetLineWidth(2);
  ge_EnergySum_LG3_zx->SetMarkerStyle(kFullCircle);
  ge_EnergySum_LG3_zx->Draw("pe");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_zy->Draw("colz");
  EnergySum_LG3_zy->SetTitle(
      "Position scan;Beam Vertical Pos (cm);Energy Sum (GeV)");

  ge_EnergySum_LG3_zy->SetLineWidth(2);
  ge_EnergySum_LG3_zy->SetMarkerStyle(kFullCircle);
  ge_EnergySum_LG3_zy->Draw("pe");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * h = (TH1 *) EnergySum_LG3->ProjectionZ();

  TF1 * fgaus = new TF1("fgaus_LG", "gaus", 0, 100);
  fgaus->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
      h->GetMean() + 2 * h->GetRMS());
  h->Fit(fgaus, "M");

  h->Sumw2();
  h->GetXaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
      h->GetMean() + 4 * h->GetRMS());
  EnergySum_LG3_zx->GetYaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
      h->GetMean() + 4 * h->GetRMS());
  EnergySum_LG3_zy->GetYaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
      h->GetMean() + 4 * h->GetRMS());

  h->SetLineWidth(2);
  h->SetMarkerStyle(kFullCircle);

  h->SetTitle(
      Form("#DeltaE/<E> = %.1f%%",
          100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEMCalTower_")
          + TString(c1->GetName()), kTRUE);

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


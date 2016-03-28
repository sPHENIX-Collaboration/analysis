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
    const TString infile =
//    "G4o.root_DSTReader.root" //
//        "../../sPHENIX_work/Prototype_2016/test_sim/15Degree_1Col/Prototype_e-_8_Seg0_DSTReader.root"//
//        "../../sPHENIX_work/Prototype_2016/test_sim/90Degree_1Col/Prototype_proton_120_Seg0_DSTReader.root"//
        "../../sPHENIX_work/Prototype_2016/test_sim/90Degree_100mrad_No0Sup/Prototype_mu-_2_Seg0_DSTReader.root"//
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

  T->SetAlias("ActiveTower",
      "TOWER_CALIB_CEMC[].get_binphi()<8 && TOWER_CALIB_CEMC[].get_bineta()<8");
  T->SetAlias("EnergySum",
      "1*Sum$(TOWER_CALIB_CEMC[].get_energy() * ActiveTower)");

//
  const TCut event_sel = "1*1";
  cuts = "all_event";
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

//  FHCal_Dist();
//  TrigDistribution();
//  EMCDistribution();

//  HCALDistribution("TOWER_HCALIN", "Inner HCal");
//  HCALDistribution("TOWER_HCALOUT", "Outer HCal");

//  EMCDistribution();
//  EMCDistribution(true);
//  EMCDistribution_SUM();
}

void
EMCDistribution( bool log_scale = false)
{

  TText * t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution" + TString(log_scale ? "_Log" : "") + cuts,
      "EMCDistribution" + TString(log_scale ? "_Log" : "") + cuts, 1800, 1000);
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
                Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 100, -.050,
                .5);

          h->SetLineWidth(0);
          h->SetLineColor(kBlue + 3);
          h->SetFillColor(kBlue + 3);
          h->GetXaxis()->SetTitleSize(.09);
          h->GetXaxis()->SetLabelSize(.08);
          h->GetYaxis()->SetLabelSize(.08);

          if (log_scale)
            QAHistManagerDef::useLogBins(h->GetXaxis());

          T->Draw("TOWER_CALIB_CEMC[].get_energy()>>" + hname,
              Form(
                  "TOWER_CALIB_CEMC[].get_bineta()==%d && TOWER_CALIB_CEMC[].get_binphi()==%d",
                  ieta, iphi), "");

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
  TH1 * hEnergySum = new TH1F("hEnergySum",
      ";Calibrated Tower Energy Sum (GeV);Count / bin", 300, 0, 30);

  T->Draw("EnergySum>>hEnergySum", "", "goff");

  TText * t;
  TCanvas *c1 = new TCanvas("EMCDistribution_SUM" + cuts,
      "EMCDistribution_SUM" + cuts, 1000, 600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * h = hEnergySum->DrawClone();
  h->SetLineWidth(2);
  h->SetLineColor(kBlue + 3);
//  h->Sumw2();
  h->GetXaxis()->SetRangeUser(0, h->GetMean() + 5 * h->GetRMS());

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 * h = (TH1 *) hEnergySum->DrawClone();

  TF1 * fgaus = new TF1("fgaus", "gaus", 0, 100);
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

TGraphErrors *
FitProfile(const TH2F * h2)
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


// $Id: $

/*!
 * \file Draw_PHG4DSTReader.C
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
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

TFile * _file0 = NULL;
TTree * T = NULL;
TString cuts = "";

void
DrawEcal_Likelihood(
//
    TString base_dir =
        "../..//sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
//    TString base_dir =
//        "../../sPHENIX_work/production_analysis_cemc2x2/embedding/emcstudies/pidstudies/spacal2d/fieldmap/",
//        TString pid = "anti_proton", //
    TString pid = "e-", //
    TString kine_config = "eta0_8GeV", int eval_mode = 1)
{

  const TString infile = base_dir + "G4Hits_sPHENIX_" + pid + "_" + kine_config
      + "-ALL.root_Ana.root.lst_EMCalLikelihood.root";
//                "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root.lst_EMCalLikelihood.root"//

  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libemcal_ana.so");
  gSystem->Load("libg4vertex.so");

  if (!_file0)
    {
      TString chian_str = infile;
      chian_str.ReplaceAll("ALL", "*");
      chian_str.ReplaceAll("+", "\\+");


      TChain * t = new TChain("T");
      const int n = t->Add(chian_str);

      cout << "Loaded " << n << " root files with " << chian_str << endl;
      assert(n>0);

      T = t;

      _file0 = new TFile;
      _file0->SetName(infile);

      fstream flst(infile + ".lst", ios_base::out);

      TObjArray *fileElements = t->GetListOfFiles();
      TIter next(fileElements);
      TChainElement *chEl = 0;
      while ((chEl = (TChainElement*) next()))
        {
          flst << chEl->GetTitle() << endl;
        }
      flst.close();

      cout << "Saved file list to " << infile + ".lst" << endl;
    }

  assert(_file0);

  T->SetAlias("UpsilonPair_trk_gpt",
      "1*sqrt(DST.UpsilonPair.trk.gpx**2 + DST.UpsilonPair.trk.gpy**2)");
  T->SetAlias("UpsilonPair_trk_pt",
      "1*sqrt(DST.UpsilonPair.trk.px**2 + DST.UpsilonPair.trk.py**2)");

  T->SetAlias("EMCalTrk_pt", "1*sqrt(DST.EMCalTrk.px**2 + DST.EMCalTrk.py**2)");
  T->SetAlias("EMCalTrk_gpt",
      "1*sqrt(DST.EMCalTrk.gpx**2 + DST.EMCalTrk.gpy**2)");

  TCut event_sel = "1*1";
  cuts = "_all_event";
  if (eval_mode == 0)
    {
      event_sel = "Entry$<50000 && fabs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05";
      cuts = "_ll_sample";
    }
  else if (eval_mode == 1)
    {
//      event_sel = "Entry$>50000 && fabs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05";
      event_sel = " fabs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05";
      cuts = "_eval_sample";
    }
  cout << "Build event selection of " << (const char *) event_sel << endl;

  T->Draw(">>EventList", event_sel);
  TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected"
      << endl;

  T->SetEventList(elist);

  if (eval_mode == 0)
    {
      Edep_Distribution(infile);
    }
  else if (eval_mode == 1)
    {
      Edep_LL_Distribution(infile);
      EP_LL_Distribution(infile);
    }

//  Edep_Checks(infile, "fabs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05");
//  Ep_Checks(infile, "fabs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05");
  Edep_Checks(infile, "1");
  Ep_Checks(infile, "1");
  //  ShowerShape_Checks(infile, "fabs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05 && DST.EMCalTrk.cemc_sum_energy>3");
}

void
EP_LL_Distribution(TString infile)
{

  TCanvas *c1 = new TCanvas("EP_LL_Distribution" + cuts,
      "EP_LL_Distribution" + cuts, 1900, 900);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("DST.EMCalTrk.ll_ep_e>>hll_ep_e(300,-30,30)", "", "");
  hll_ep_e->SetTitle(
      Form(
          "EMCal only: Electron likelihood distribution;log(Electron likelihood);A. U."));

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("DST.EMCalTrk.ll_ep_h>>hll_ep_h(300,-30,30)", "", "");
  hll_ep_h->SetTitle(
      Form(
          "EMCal only: Hadron likelihood distribution;log(Hadron likelihood);A. U."));

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw(
      "DST.EMCalTrk.ll_ep_e - DST.EMCalTrk.ll_ep_h>>hll_ep_diff(300,-30,30)",
      "", "");
  TH1F *hll_ep_diff = (TH1F *) gDirectory->GetObjectChecked("hll_ep_diff",
      "TH1F");

  hll_ep_diff->SetTitle(
      Form(
          "EMCal only: log likelihood cut;log(Electron likelihood) - log(Hadron likelihood);Count / bin"));
//  hll_ep_diff->Scale(1./hll_ep_diff->GetSum());

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();

  p->DrawFrame(-30, 1e-4, 30, 1,
      "EMCal only: Cut Efficiency;Cut on log(Electron likelihood) - log(Hadron likelihood);Cut Efficiency");
  TGraphErrors * ge = Distribution2Efficiency(hll_ep_diff);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_Likelihood_")
          + TString(c1->GetName()), kFALSE);

}

void
Edep_LL_Distribution(TString infile)
{

  TCanvas *c1 = new TCanvas("Edep_LL_Distribution" + cuts,
      "Edep_LL_Distribution" + cuts, 1900, 900);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("DST.EMCalTrk.ll_edep_e>>hll_edep_e(300,-30,30)", "", "");
  hll_edep_e->SetTitle(
      Form(
          "EMCal + HCal_{in}: Electron likelihood distribution;log(Electron likelihood);A. U."));

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("DST.EMCalTrk.ll_edep_h>>hll_edep_h(300,-30,30)", "", "");
  hll_edep_h->SetTitle(
      Form(
          "EMCal + HCal_{in}: Hadron likelihood distribution;log(Hadron likelihood);A. U."));

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw(
      "DST.EMCalTrk.ll_edep_e - DST.EMCalTrk.ll_edep_h>>hll_edep_diff(300,-30,30)",
      "", "");
  TH1F *hll_edep_diff = (TH1F *) gDirectory->GetObjectChecked("hll_edep_diff",
      "TH1F");

  hll_edep_diff->SetTitle(
      Form(
          "EMCal + HCal_{in}: log likelihood cut;log(Electron likelihood) - log(Hadron likelihood);Count / bin"));
//  hll_edep_diff->Scale(1./hll_edep_diff->GetSum());

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();

  p->DrawFrame(-30, 1e-4, 30, 1,
      "EMCal + HCal_{in}: Cut Efficiency;log(Electron likelihood) - log(Hadron likelihood);Cut Efficiency");
  TGraphErrors * ge = Distribution2Efficiency(hll_edep_diff);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_Likelihood_")
          + TString(c1->GetName()), kFALSE);

}

void
Edep_Distribution(TString infile)
{

  double N_Event = T->GetEntries();

  TCanvas *c1 = new TCanvas("Edep_Distribution" + cuts,
      "Edep_Distribution" + cuts, 1900, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "DST.EMCalTrk.hcalin_sum_energy:DST.EMCalTrk.get_ep()>>h2_Edep_Distribution_raw(240,-.0,2, 240,-.0,12)",
      "", "colz");
  h2_Edep_Distribution_raw->SetTitle(
      Form(
          "Energy distribution;CEMC Cluster Energy in GeV;HCal_{in} Cluster Energy in GeV"));
  h2_Edep_Distribution_raw->Scale(1. / N_Event);
//  h2_Edep_Distribution_raw->GetZaxis()->SetRangeUser(1. / N_Event, 1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  TH2F * h2_Edep_Distribution = (TH2F *) h2_Edep_Distribution_raw->Clone(
      "h2_Edep_Distribution");

  h2_Edep_Distribution->Smooth(1, "k5b");
  h2_Edep_Distribution->Scale(1. / h2_Edep_Distribution->GetSum());
  h2_Edep_Distribution->Draw("colz");

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_Likelihood_")
          + TString(c1->GetName()), kFALSE);

}

void
ShowerShape_Checks(TString infile, TCut good_track_cut)
{

  double N_Event = T->GetEntries();

  TCanvas *c1 = new TCanvas("ShowerShape_Checks" + cuts,
      "ShowerShape_Checks" + cuts, 1900, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  T->Draw(
      "DST.EMCalTrk.cemc_energy/DST.EMCalTrk.cemc_sum_energy:DST.EMCalTrk.cemc_radius>>hEMCalTrk_cemc_shape(30,0,2,100,0,1)",
      good_track_cut, "goff");

  TH2 * hEMCalTrk_cemc_shape = (TH2 *) gDirectory->GetObjectChecked(
      "hEMCalTrk_cemc_shape", "TH2");
  hEMCalTrk_cemc_shape->SetTitle(
      Form(
          "CEMC Shower Shape;Distance to track projection (Cluster width);Tower Energy/Cluster Energy"));
//  hEMCalTrk_cemc_shape->Scale(1. / N_Event);

  TH1D * hEMCalTrk_cemc_shape_px = hEMCalTrk_cemc_shape->ProjectionX(
      "hEMCalTrk_cemc_shape_px");

  for (int r = 1; r <= hEMCalTrk_cemc_shape->GetNbinsX(); r++)
    for (int e = 1; e <= hEMCalTrk_cemc_shape->GetNbinsY(); e++)
      {
        hEMCalTrk_cemc_shape->SetBinContent(r, e,
            hEMCalTrk_cemc_shape->GetBinContent(r, e)
                / hEMCalTrk_cemc_shape_px->GetBinContent(r));
      }

  hEMCalTrk_cemc_shape->Draw("colz");

  //  return;
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  T->Draw(
      "DST.EMCalTrk.hcalin_energy/DST.EMCalTrk.hcalin_sum_energy:DST.EMCalTrk.hcalin_radius>>hEMCalTrk_hcalin_shape(30,0,2,100,0,1)",
      good_track_cut, "colz");
  TH2 * hEMCalTrk_hcalin_shape = (TH2 *) gDirectory->GetObjectChecked(
      "hEMCalTrk_hcalin_shape", "TH2");

  hEMCalTrk_hcalin_shape->SetTitle(
      Form(
          "HCal_{in} Shower Shape;Distance to track projection (Cluster width);Tower Energy/Cluster Energy"));
//  hEMCalTrk_hcalin_shape->Scale(1. / N_Event);

  TH1D * hEMCalTrk_hcalin_shape_px = hEMCalTrk_hcalin_shape->ProjectionX(
      "hEMCalTrk_hcalin_shape_px");

  for (int r = 1; r <= hEMCalTrk_hcalin_shape->GetNbinsX(); r++)
    for (int e = 1; e <= hEMCalTrk_hcalin_shape->GetNbinsY(); e++)
      {
        hEMCalTrk_hcalin_shape->SetBinContent(r, e,
            hEMCalTrk_hcalin_shape->GetBinContent(r, e)
                / hEMCalTrk_hcalin_shape_px->GetBinContent(r));
      }

  hEMCalTrk_hcalin_shape->Draw("colz");

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_Likelihood_")
          + TString(c1->GetName()), kFALSE);

}

void
Edep_Checks(TString infile, TCut good_track_cut)
{

  double N_Event = T->GetEntries();

  TCanvas *c1 = new TCanvas("Edep_Checks" + cuts, "Edep_Checks" + cuts, 1900,
      950);
  c1->Divide(3, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("DST.EMCalTrk.cemc_sum_n_tower>>hEMCalTrk_cemc_ntower(16,-.5,15.5)",
      good_track_cut);
  hEMCalTrk_cemc_ntower->SetTitle(
      Form("CEMC Cluster Size;Cluster Size (Towers);Probability"));
  hEMCalTrk_cemc_ntower->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("DST.EMCalTrk.cemc_sum_energy>>hEMCalTrk_cemc_e(240,-.0,12)",
      good_track_cut);
  hEMCalTrk_cemc_e->SetTitle(
      Form("CEMC Cluster Energy;Cluster Energy (GeV);Count/bin"));
//  hEMCalTrk_cemc_e->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->DrawFrame(-.0, 1e-3, 12, 1,
      "CEMC Cut Eff;Cut on Cluster Energy (GeV);Efficiency");

  TGraphErrors * ge = Distribution2Efficiency(hEMCalTrk_cemc_e);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw(
      "DST.EMCalTrk.hcalin_sum_n_tower>>hEMCalTrk_hcalin_ntower(16,-.5,15.5)",
      good_track_cut);
  hEMCalTrk_hcalin_ntower->SetTitle(
      Form("HCal_{in} Cluster Size;Cluster Size (Towers);Probability"));
  hEMCalTrk_hcalin_ntower->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("DST.EMCalTrk.hcalin_sum_energy>>hEMCalTrk_hcalin_e(240,-.0,12)",
      good_track_cut);
  hEMCalTrk_hcalin_e->SetTitle(
      Form("HCal_{in} Cluster Energy;Cluster Energy (GeV);Count/bin"));
//  hEMCalTrk_hcalin_e->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->DrawFrame(-.0, 1e-3, 12, 1,
      "HCal_{in} Cut Eff;Cut on Cluster Energy (GeV);Efficiency");

  TGraphErrors * ge = Distribution2Efficiency(hEMCalTrk_hcalin_e);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_pDST_")
          + TString(c1->GetName()), kFALSE);

  TCanvas *c1 = new TCanvas("Edep_Checks_2D" + cuts, "Edep_Checks_2D" + cuts,
      900, 900);
//  c1->Divide(2, 2);
//  int idx = 1;
//  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "DST.EMCalTrk.hcalin_sum_energy:DST.EMCalTrk.cemc_sum_energy>>h2_EMCalTrk_hcalin_e_EMCalTrk_cemc_e(240,-.0,8, 240,-.0,8)",
      good_track_cut, "colz");
  h2_EMCalTrk_hcalin_e_EMCalTrk_cemc_e->SetTitle(
      Form(
          "Energy distribution;CEMC Cluster Energy in GeV;HCal_{in} Cluster Energy in GeV"));
  h2_EMCalTrk_hcalin_e_EMCalTrk_cemc_e->Scale(1. / N_Event);
  h2_EMCalTrk_hcalin_e_EMCalTrk_cemc_e->GetZaxis()->SetRangeUser(1. / N_Event,
      1);

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_Likelihood_")
          + TString(c1->GetName()), kFALSE);

}

void
Ep_Checks(TString infile, TCut good_track_cut)
{

  double N_Event = T->GetEntries();

  TCanvas *c1 = new TCanvas("Ep_Checks" + cuts, "Ep_Checks" + cuts, 1900, 950);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("DST.EMCalTrk.get_ep()>>hEMCalTrk_get_ep(240,-.0,2)",
      good_track_cut);
  hEMCalTrk_get_ep->SetTitle(
      Form("CEMC Cluster Energy/Track Momentum;E/p;Count/bin"));
//  hEMCalTrk_cemc_e->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  if (infile.Contains("e-") || infile.Contains("e+"))
    {
      p->DrawFrame(-.0, 0.8, 1.5, 1,
          "CEMC E/p Cut Eff;Cut on E/p;Signal Efficiency");
    }
  else
    {
      p->DrawFrame(-.0, 1e-3, 1.5, 1,
          "CEMC E/p Cut Eff;Cut on E/p;Background Efficiency or 1/Rejection");
      p->SetLogy();
    }
  TGraphErrors * ge = Distribution2Efficiency(hEMCalTrk_get_ep);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_Likelihood_")
          + TString(c1->GetName()), kFALSE);

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

  assert(hCEMC3_Max->GetNbinsX()<10000);

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

  return ge;
}

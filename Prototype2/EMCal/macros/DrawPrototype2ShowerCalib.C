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
DrawPrototype2ShowerCalib( //
    const TString infile =
//        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/ShowerCalib/UIUC18.lst_EMCalCalib.root" //
//        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/ShowerCalib/THP.lst_EMCalCalib.root"//
        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/ShowerCalib/Tilt0.lst_EMCalCalib.root"//
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

//  gROOT->LoadMacro("Prototype2_DSTReader.C+");

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

  event_sel = "good_data";
  cuts = "_good_data";
  //  event_sel = "good_data && info.hodo_h==4 && info.hodo_v==3";
  //  cuts = "_good_data_h4_v3";
//    event_sel = "good_data && info.hodo_h>=3 && info.hodo_h<=4 && info.hodo_v>=2 && info.hodo_v<=4";
//    cuts = "_good_data_h4_v3";

  cout << "Build event selection of " << (const char *) event_sel << endl;

  T->Draw(">>EventList", event_sel);
  TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected"
      << endl;

  T->SetEventList(elist);

  HodoscopeCheck();
  vector<double> beam_mom(GetBeamMom());

  lin_res ges_clus_5x5_prod = GetResolution("clus_5x5_prod", beam_mom, kBlue);
  lin_res ges_clus_3x3_prod = GetResolution("clus_3x3_prod", beam_mom,
      kBlue + 3);
  lin_res ges_clus_5x5_temp = GetResolution("clus_5x5_temp", beam_mom,
      kRed -2);
  lin_res ges_clus_5x5_recalib = GetResolution("clus_5x5_recalib", beam_mom,
      kRed + 3);

  TCanvas *c1 = new TCanvas(Form("Res_linear")+ cuts, Form("Res_linear")+ cuts, 1300, 600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();

  TLegend* leg = new TLegend(.15, .7, .6, .85);

  p->DrawFrame(0, 0, 25, 25,
      Form("Electron Linearity;Input energy (GeV);Measured Energy (GeV)"));
  TLine * l = new TLine(0, 0, 25, 25);
  l->SetLineColor(kGray);
//  l->Draw();

  TF1 * f_calo_l_sim = new TF1("f_calo_l", "pol2", 0.5, 25);
//  f_calo_l_sim->SetParameters(-0.03389, 0.9666, -0.0002822);
  f_calo_l_sim->SetParameters(-0., 1, -0.);
//  f_calo_l_sim->SetLineWidth(1);
  f_calo_l_sim->SetLineColor(kGreen + 2);
  f_calo_l_sim->SetLineWidth(3);

  f_calo_l_sim->Draw("same");
  ges_clus_5x5_prod.linearity->Draw("p");
  ges_clus_3x3_prod.linearity->Draw("p");
  ges_clus_5x5_temp.linearity->Draw("p");
  ges_clus_5x5_recalib.linearity->Draw("p");
//  ge_linear->Fit(f_calo_l, "RM0");
//  f_calo_l->Draw("same");

  leg->AddEntry(ges_clus_5x5_prod.linearity, ges_clus_5x5_prod.name, "ep");
  leg->AddEntry(ges_clus_3x3_prod.linearity, ges_clus_3x3_prod.name, "ep");
  leg->AddEntry(ges_clus_5x5_temp.linearity, ges_clus_5x5_temp.name, "ep");
  leg->AddEntry(ges_clus_5x5_recalib.linearity, ges_clus_5x5_recalib.name, "ep");
  leg->AddEntry(f_calo_l_sim, "Unity", "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
//  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TF1 * f_calo_sim = new TF1("f_calo_sim", "sqrt([0]*[0]+[1]*[1]/x)/100", 0.5,
      25);
  f_calo_sim->SetParameters(2.4, 11.8);
  f_calo_sim->SetLineWidth(3);
  f_calo_sim->SetLineColor(kGreen + 2);

  TH1 * hframe = p->DrawFrame(0, 0, 25, 0.3,
      Form("Resolution;Input energy (GeV);#DeltaE/<E>"));

  TLegend* leg = new TLegend(.2, .6, .85, .9);

  ges_clus_5x5_prod.f_res->Draw("same");
  ges_clus_5x5_prod.resolution->Draw("ep");
  ges_clus_3x3_prod.f_res->Draw("same");
  ges_clus_3x3_prod.resolution->Draw("ep");
  ges_clus_5x5_temp.f_res->Draw("same");
  ges_clus_5x5_temp.resolution->Draw("ep");
  ges_clus_5x5_recalib.f_res->Draw("same");
  ges_clus_5x5_recalib.resolution->Draw("ep");
  f_calo_sim->Draw("same");

  leg->AddEntry(ges_clus_5x5_prod.resolution, ges_clus_5x5_prod.name, "ep");
  leg->AddEntry(ges_clus_5x5_prod.f_res,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
          ges_clus_5x5_prod.f_res->GetParameter(0),
          ges_clus_5x5_prod.f_res->GetParameter(1)), "l");

  leg->AddEntry(ges_clus_3x3_prod.resolution, ges_clus_3x3_prod.name, "ep");
  leg->AddEntry(ges_clus_3x3_prod.f_res,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
          ges_clus_3x3_prod.f_res->GetParameter(0),
          ges_clus_3x3_prod.f_res->GetParameter(1)), "l");

  leg->AddEntry(ges_clus_5x5_temp.resolution, ges_clus_5x5_temp.name, "ep");
  leg->AddEntry(ges_clus_5x5_temp.f_res,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
          ges_clus_5x5_temp.f_res->GetParameter(0),
          ges_clus_5x5_temp.f_res->GetParameter(1)), "l");

  leg->AddEntry(ges_clus_5x5_recalib.resolution, ges_clus_5x5_recalib.name, "ep");
  leg->AddEntry(ges_clus_5x5_recalib.f_res,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
          ges_clus_5x5_recalib.f_res->GetParameter(0),
          ges_clus_5x5_recalib.f_res->GetParameter(1)), "l");
//  leg->AddEntry(new TH1(), "", "l");
//  leg->AddEntry((TObject*) 0, " ", "");

  leg->Draw();

  TLegend* leg = new TLegend(.2, .1, .85, .3);

  leg->AddEntry(f_calo_sim,
      Form("Prelim. Sim., #DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
          f_calo_sim->GetParameter(0), f_calo_sim->GetParameter(1)), "l");
  leg->Draw();

  hframe->SetTitle("Electron Resolution");

  SaveCanvas(c1,
      TString(_file0->GetName()) + "_DrawPrototype2ShowerCalib_"
          + TString(c1->GetName()), kTRUE);
}

void
HodoscopeCheck()
{
  vector<double> mom;

  TText * t;
  TCanvas *c1 = new TCanvas("HodoscopeCheck" + cuts, "HodoscopeCheck" + cuts,
      1300, 950);

  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetGridx(1);
  p->SetGridy(1);
  p->SetLogz();

  T->Draw("clus_5x5_prod.average_col:hodo_h>>h2_h(8,-.5,7.5,160,-.5,7.5)",
      "good_data", "colz");
  h2_h->SetTitle(
      "Horizontal hodoscope check;Horizontal Hodoscope;5x5 cluster mean col");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetGridx(1);
  p->SetGridy(1);
  p->SetLogz();

  T->Draw("clus_5x5_prod.average_row:hodo_v>>h2_v(8,-.5,7.5,160,-.5,7.5)",
      "good_data", "colz");
  h2_v->SetTitle(
      "Vertical hodoscope check;Vertical Hodoscope;5x5 cluster mean row");

  SaveCanvas(c1,
      TString(_file0->GetName()) + "_DrawPrototype2ShowerCalib_"
          + TString(c1->GetName()), kTRUE);

  return mom;
}

vector<double>
GetBeamMom()
{
  vector<double> mom;

  TH1F * hbeam_mom = new TH1F("hbeam_mom", ";beam momentum (GeV)", 32, .5,
      32.5);

  TText * t;
  TCanvas *c1 = new TCanvas("GetBeamMom" + cuts, "GetBeamMom" + cuts, 1800,
      900);

  T->Draw("abs(info.beam_mom)>>hbeam_mom");

  for (int bin = 1; bin < hbeam_mom->GetNbinsX(); bin++)
    {
      if (hbeam_mom->GetBinContent(bin) > 50)
        {
          const double momentum = hbeam_mom->GetBinCenter(bin);
          cout << "GetBeamMom - " << momentum << " GeV for "
              << hbeam_mom->GetBinContent(bin) << " event" << endl;
          mom.push_back(momentum);
        }
    }

  SaveCanvas(c1,
      TString(_file0->GetName()) + "_DrawPrototype2ShowerCalib_"
          + TString(c1->GetName()), kTRUE);

  return mom;
}

lin_res
GetResolution(TString cluster_name, vector<double> beam_mom, Color_t col)
{
  vector<double> mean;
  vector<double> mean_err;
  vector<double> res;
  vector<double> res_err;

  TCanvas *c1 = new TCanvas("GetResolution_LineShape_" + cluster_name + cuts,
      "GetResolution_LineShape_" + cluster_name + cuts, 1800, 900);

  c1->Divide(4, 2);
  int idx = 1;
  TPad * p;

  for (int i = 0; i < beam_mom.size(); ++i)
    {
      p = (TPad *) c1->cd(idx++);
      c1->Update();

      const double momemtum = beam_mom[i];
      const TString histname = Form("hLineShape%.0fGeV", momemtum);

      TH1F * h = new TH1F(histname, histname + ";Observed energy (GeV)", (momemtum<=6 ? 25:50),
          momemtum / 2, momemtum * 1.5);
      T->Draw(cluster_name + ".sum_E>>" + histname,
          Form("abs(info.beam_mom)==%f", momemtum));

      h->Sumw2();

      TF1 * fgaus_g = new TF1("fgaus_LG_g", "gaus",
          h->GetMean() - 1 * h->GetRMS(), h->GetMean() + 4 * h->GetRMS());
      fgaus_g->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
          h->GetMean() + 2 * h->GetRMS());
      h->Fit(fgaus_g, "MR0N");

      TF1 * fgaus = new TF1("fgaus_LG", "gaus",
          fgaus_g->GetParameter(1) - 2 * fgaus_g->GetParameter(2),
          fgaus_g->GetParameter(1) + 2 * fgaus_g->GetParameter(2));
      fgaus->SetParameters(fgaus_g->GetParameter(0), fgaus_g->GetParameter(1),
          fgaus_g->GetParameter(2));
      h->Fit(fgaus, "MR");

      h->SetLineWidth(2);
      h->SetMarkerStyle(kFullCircle);
      fgaus->SetLineWidth(2);

      h->SetTitle(
          Form("%.0f GeV/c: #DeltaE/<E> = %.1f%%", momemtum,
              100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

      mean.push_back(fgaus->GetParameter(1));
      mean_err.push_back(fgaus->GetParError(1));
      res.push_back(fgaus->GetParameter(2) / fgaus->GetParameter(1));
      res_err.push_back(fgaus->GetParError(2) / fgaus->GetParameter(1));

    }

  SaveCanvas(c1,
      TString(_file0->GetName()) + "_DrawPrototype2ShowerCalib_"
          + TString(c1->GetName()), kTRUE);

  TGraphErrors * ge_linear = new TGraphErrors(beam_mom.size(), &beam_mom[0],
      &mean[0], 0, &mean_err[0]);

  TGraphErrors * ge_res = new TGraphErrors(beam_mom.size(), &beam_mom[0],
      &res[0], 0, &res_err[0]);
  ge_res->GetHistogram()->SetStats(0);
  ge_res->Print();

  lin_res ret;
  ret.name = cluster_name;
  ret.linearity = ge_linear;
  ret.resolution = ge_res;
  ret.f_res = new TF1("f_calo_r", "sqrt([0]*[0]+[1]*[1]/x)/100", 0.5, 25);
  ge_res->Fit(ret.f_res, "RM0QN");

  ge_linear->SetLineColor(col);
  ge_linear->SetMarkerColor(col);
  ge_linear->SetLineWidth(2);
  ge_linear->SetMarkerStyle(kFullCircle);
  ge_linear->SetMarkerSize(1.5);

  ge_res->SetLineColor(col);
  ge_res->SetMarkerColor(col);
  ge_res->SetLineWidth(2);
  ge_res->SetMarkerStyle(kFullCircle);
  ge_res->SetMarkerSize(1.5);

  ge_res->GetHistogram()->SetStats(0);

  ret.f_res->SetLineColor(col);
  ret.f_res->SetLineWidth(3);

  return ret;
}

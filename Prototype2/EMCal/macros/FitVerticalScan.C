// $Id: $                                                                                             

/*!
 * \file Draw.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <cmath>
#include <vector>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TF1.h>
#include <cassert>
#include <iostream>
#include <fstream>

#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

vector<TGraphErrors *> vg_src;

void
OnlineAnalysis(TString filename =
    "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/column3_214x.txt")
{
  std::map<int, float> runtoz;

  runtoz[2138] = 29.7;
  runtoz[2140] = 55.2;
  runtoz[2141] = 80.6;
  runtoz[2142] = 106.3;
  runtoz[2143] = 131.5;
  runtoz[2144] = 156.6;
  runtoz[2146] = 182.4;

  runtoz[2171] = 142.1;
  runtoz[2172] = 122.0;
  runtoz[2174] = 102.1;
  runtoz[2176] = 81.9;
  runtoz[2177] = 61.9;
  runtoz[2178] = 42.1;
  runtoz[2179] = 21.9;

  runtoz[2181] = 21.85;
  runtoz[2182] = 41.99;
  runtoz[2183] = 61.87;
  runtoz[2184] = 61.87;
  runtoz[2185] = 82.12;
  runtoz[2186] = 101.9;
  runtoz[2187] = 121.9;
  runtoz[2188] = 142.0;
  runtoz[2189] = 152.0;
  runtoz[2190] = 162.1;
  runtoz[2191] = 172.0;
  runtoz[2192] = 172.0;

  TCanvas *c1 = new TCanvas("c1", "Vertical scan", 640, 480);

  TLegend *leg = new TLegend(0.9122257, 0.6483516, 0.9968652, 0.9010989, NULL,
      "brNDC");
  leg->SetFillColor(0);

  int rn;
  int tower;
  float mpv;
  float dmpv;

  TGraphErrors *gr[8];

  vector<float> v_x[8];
  vector<float> v_y[8];
  vector<float> v_dx[8];
  vector<float> v_dy[8];

  ifstream sumfile;
  sumfile.open(filename);

  std::string line;
  int linenum = 0;
  int index;
  while (sumfile >> rn >> tower >> mpv >> dmpv)
    {
      index = (tower / 8);
      cout << index << " " << rn << " " << runtoz[rn] << " " << mpv << " "
          << dmpv << endl;
      v_x[index].push_back(runtoz[rn]);
      v_dx[index].push_back(0.2);
      v_y[index].push_back(mpv);
      v_dy[index].push_back(dmpv);
      linenum++;
    }

  TMultiGraph *mg = new TMultiGraph();

  TString towername;
  int col = 1;
  int marker = 21;
  for (int i = 0; i < 8; i++)
    {
      gr[i] = new TGraphErrors(v_x[i].size(), &v_x[i][0], &v_y[i][0],
          &v_dx[i][0], &v_dy[i][0]);
      gr[i]->SetMarkerStyle(marker);
      gr[i]->SetMarkerSize(0.4);
      gr[i]->SetLineColor(col);
      gr[i]->SetMarkerColor(col);
      gr[i]->Fit("expo");
      gr[i]->GetFunction("expo")->SetLineColor(col);

      vg_src.push_back((TGraphErrors *) gr[i]->Clone());

      towername = "Tower ";
      towername += i;
      leg->AddEntry(gr[i], towername, "LP");
      mg->Add(gr[i]);
      col++;
      marker++;
    }

  mg->Draw("AP");
  TString title = "Longitudinal EMCAL scan (";
  title += filename;
  title += ")";
  mg->SetTitle(title);
  mg->GetXaxis()->SetTitle("Vertical table position (mm)");
  mg->GetYaxis()->SetTitle("MPV of 200 GeV p (ADC counts)");
  leg->Draw();

  c1->Update();
  c1->Modified();

}

void
FitVerticalScan(const TString input =
//    "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/mpvPerTower.root"
    "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/getMeanRMS_Summary.root")
{
//  gROOT->SetStyle("Modern");
  SetOKStyle();

  TFile * _file0 = new TFile(input);
  assert(_file0->IsOpen());

//  TCanvas * c2 = (TCanvas *) _file0->Get("c2");
  TCanvas * c2 = (TCanvas *) _file0->Get("getMeanRMS_Summary");
  assert(c2);

  c2->Draw();

  int i = 2;
  vg_src.push_back(
      (TGraphErrors *) c2->GetListOfPrimitives()->At(i++)->Clone());
  vg_src.push_back(
      (TGraphErrors *) c2->GetListOfPrimitives()->At(i++)->Clone());
  vg_src.push_back(
      (TGraphErrors *) c2->GetListOfPrimitives()->At(i++)->Clone());
  vg_src.push_back(
      (TGraphErrors *) c2->GetListOfPrimitives()->At(i++)->Clone());
  vg_src.push_back(
      (TGraphErrors *) c2->GetListOfPrimitives()->At(i++)->Clone());
  vg_src.push_back(
      (TGraphErrors *) c2->GetListOfPrimitives()->At(i++)->Clone());
  vg_src.push_back(
      (TGraphErrors *) c2->GetListOfPrimitives()->At(i++)->Clone());
  vg_src.push_back(
      (TGraphErrors *) c2->GetListOfPrimitives()->At(i++)->Clone());

//  OnlineAnalysis();

  TGraphErrors * g_final = new TGraphErrors((vg_src[0])->GetN());
  g_final->SetMarkerSize(3);
  g_final->SetMarkerStyle(kFullCircle);
  g_final->SetMarkerColor(kBlue + 3);
  g_final->SetFillColor(kWhite);
  g_final->SetLineWidth(3);
  g_final->SetFillStyle(0);

//  TF1 * f_att = new TF1("f_att","(exp(-((14-x)/[0])) + 0.3 * exp(-((x-10 + 13)/[0])) )*[1]",0,20);
//  f_att->SetParameters(105,1);
//  f_att->SetLineColor(kBlue+3);
//  f_att->SetLineWidth(4);
//  TF1 * f_att = new TF1("f_att","(exp(-((14-x)/[0])) + 0.3 * exp(-((x + 13)/[0])) )*[1]",0,20);
//  f_att->SetParameters(105,1);
  TF1 * f_att = new TF1("f_att", "(exp(-((13.9-x)/[0])))*[1]", 0, 20);
  f_att->SetParameters(105, 1);

  f_att->SetLineColor(kBlue + 3);
  f_att->SetLineWidth(4);

  const int N_rows = 8;

  TGraphErrors * g_src = vg_src[0];
//  g_src->Print();
  for (int j = 0; j < (g_src->GetN()); ++j)
    {
      assert(g_src);

      (g_final->GetY())[j] = 0;
      (g_final->GetEY())[j] = 0;
      (g_final->GetEX())[j] = 0;
      (g_final->GetX())[j] = (g_src->GetX())[j] / 10 - 0.8;
    }

  for (int i = 0; i < N_rows; ++i)
    {
      new TCanvas();

      TGraphErrors * g_src = vg_src[i];

      assert(g_src);

//      g_src->Set
      g_src->DrawClone("ap*");

      double sum = 0;
      for (int j = 0; j < (g_src->GetN()); ++j)
        {
          sum += (g_src->GetY())[j];
        }
      double average = sum / g_src->GetN();

      for (int j = 0; j < (g_src->GetN()); ++j)
        {
          (g_src->GetY())[j] /= average;
          (g_src->GetEY())[j] /= average;

          (g_final->GetY())[j] += (g_src->GetY())[j];
          (g_final->GetEY())[j] += (g_src->GetEY())[j] * (g_src->GetEY())[j];
        }
    }

  for (int j = 0; j < (g_src->GetN()); ++j)
    {
      (g_final->GetY())[j] /= N_rows;
      (g_final->GetEY())[j] = sqrt((g_final->GetEY())[j]) / N_rows;
    }

  TText * t;
  TCanvas *c1 = new TCanvas("FitVerticalScan", "FitVerticalScan", 1800, 650);

  gPad->SetGridx(0);
  gPad->SetGridy(0);
  gPad->SetRightMargin(0.99);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.2);

  TH1 * frame = gPad->DrawFrame(0, .9, 13.9, 1.1);
  frame->SetTitle(
      ";Beam position along the length of module (cm);MIP amplitude (A. U.)");
  frame->GetXaxis()->SetTitleOffset(1);
  frame->GetXaxis()->SetTitleSize(0.08);
  frame->GetXaxis()->SetLabelSize(0.08);
  frame->GetYaxis()->SetTitleOffset(.7);
  frame->GetYaxis()->SetTitleSize(0.08);
  frame->GetYaxis()->SetLabelSize(0.08);
  frame->GetXaxis()->CenterTitle();

//  f_att->DrawClone("same");
  g_final->Draw("p");
  g_final->Fit(f_att, "MR");
//  g_final->Fit("pol1");
  g_final->Print();

  TLegend *leg = new TLegend(.15, .7, .8, .9);
  leg->AddEntry(g_final, "Data, Averaged over 8 towers","ep");
  leg->AddEntry(f_att,
      Form("Fit, C#timesExp[ -(13.9 - x) / L_{eff} ], L_{eff} = %.0f#pm%.0f cm",
          f_att->GetParameter(0), f_att->GetParError(0)),"l");

  leg->Draw();

  TLatex latex;
  latex.SetTextSize(0.08);
  latex.SetTextAlign(12);
  latex.DrawLatex(0,0.92,"#leftarrow Mirror side");
  latex.SetTextAlign(32);
  latex.DrawLatex(13.9,0.92,"Lightguide side #rightarrow");

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}


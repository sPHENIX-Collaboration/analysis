#include <string>
#include "TString.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"

void plotQA_Linearity()
{
  // 2018 pion with outer hcal only
  string input_2018_ohcal = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/T1044_2018_pion_hcalout.root";
  TFile *File_2018_ohcal = TFile::Open(input_2018_ohcal.c_str());
  TGraphAsymmErrors *g_linearity_2018_pion_hcalout = (TGraphAsymmErrors*)File_2018_ohcal->Get("g_linearity_2018_pion_hcalout");

  // 2018 electron
  string input_2018_electron = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/T1044_2018_electron.root";
  TFile *File_2018_electron = TFile::Open(input_2018_electron.c_str());
  TGraphAsymmErrors *g_linearity_2018_electron = (TGraphAsymmErrors*)File_2018_electron->Get("g_linearity_2018_electron");

  // 2018 pion
  string input_2018_pion = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/T1044_2018_pion.root";
  TFile *File_2018_pion = TFile::Open(input_2018_pion.c_str());
  TGraphAsymmErrors *g_linearity_2018_pion = (TGraphAsymmErrors*)File_2018_pion->Get("g_linearity_2018_pion");

  // 2017 pion
  string input_2017_pion = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/T1044_2017_pion.root";
  TFile *File_2017_pion = TFile::Open(input_2017_pion.c_str());
  TGraphAsymmErrors *g_linearity_2017_pion = (TGraphAsymmErrors*)File_2017_pion->Get("g_linearity_2017_pion");

  TCanvas *c_Linearity = new TCanvas("c_Linearity","c_Linearity",10,10,800,800);
  c_Linearity->cd();
  c_Linearity->cd()->SetLeftMargin(0.15);
  c_Linearity->cd()->SetBottomMargin(0.15);
  c_Linearity->cd()->SetTicks(1,1);
  c_Linearity->cd()->SetGrid(0,0);

  TH1F *h_play = new TH1F("h_play","h_play",100,0.0,100.0);
  for(int i_bin = 0; i_bin < 100; ++i_bin)
  {
    h_play->SetBinContent(i_bin+1,-10.0);
    h_play->SetBinError(i_bin+1,1.0);
  }
  h_play->SetTitle("");
  h_play->SetStats(0);
  h_play->GetXaxis()->SetTitle("input Energy (GeV)");
  h_play->GetXaxis()->CenterTitle();
  h_play->GetXaxis()->SetNdivisions(505);
  h_play->GetXaxis()->SetRangeUser(0.0,40.0);

  h_play->GetYaxis()->SetTitle("Tower Calibrated Energy (GeV)");
  h_play->GetYaxis()->CenterTitle();
  h_play->GetYaxis()->SetRangeUser(0.0,40.0);
  h_play->DrawCopy("pE");

  TLine *l_unity = new TLine(1.0,1.0,39.0,39.0);
  l_unity->SetLineColor(4);
  l_unity->SetLineStyle(2);
  l_unity->SetLineWidth(2);
  l_unity->Draw("l same");

  g_linearity_2017_pion->SetMarkerStyle(21);
  g_linearity_2017_pion->SetMarkerColor(2);
  g_linearity_2017_pion->SetMarkerSize(2.0);
  g_linearity_2017_pion->Draw("pE same");

  g_linearity_2018_pion->SetMarkerStyle(20);
  g_linearity_2018_pion->SetMarkerColor(kGray+2);
  g_linearity_2018_pion->SetMarkerSize(2.0);
  g_linearity_2018_pion->Draw("pE same");

  g_linearity_2018_pion_hcalout->SetMarkerStyle(34);
  g_linearity_2018_pion_hcalout->SetMarkerColor(6);
  g_linearity_2018_pion_hcalout->SetMarkerSize(2.0);
  g_linearity_2018_pion_hcalout->Draw("pE same");

  g_linearity_2018_electron->SetMarkerStyle(20);
  g_linearity_2018_electron->SetMarkerColor(4);
  g_linearity_2018_electron->SetMarkerSize(2.0);
  g_linearity_2018_electron->Draw("pE same");

  TLegend *leg_linear = new TLegend(0.2,0.6,0.5,0.8);
  leg_linear->SetBorderSize(0);
  leg_linear->SetFillColor(0);
  leg_linear->AddEntry(g_linearity_2017_pion,"#pi- T1044-2017","p");
  leg_linear->AddEntry(g_linearity_2018_pion,"#pi- T1044-2018","p");
  leg_linear->AddEntry(g_linearity_2018_pion_hcalout,"#pi- T1044-2018 & OHCal","p");
  leg_linear->AddEntry(g_linearity_2018_electron,"e- T1044-2018","p");
  leg_linear->AddEntry(l_unity,"unity","l");
  leg_linear->Draw("same");

  c_Linearity->SaveAs("../figures/HCAL_ShowerCalib/c_Linearity.eps");
}

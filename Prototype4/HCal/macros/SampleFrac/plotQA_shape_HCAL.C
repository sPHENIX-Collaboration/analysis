#include "TPolyLine.h"
#include "TH1F.h"
#include "TLine.h"
#include "TCanvas.h"

void PlotLine(double x1_val, double x2_val, double y1_val, double y2_val, int Line_Col, int LineWidth, int LineStyle)
{
  TLine* Zero_line = new TLine();
  Zero_line -> SetX1(x1_val);
  Zero_line -> SetX2(x2_val);
  Zero_line -> SetY1(y1_val);
  Zero_line -> SetY2(y2_val);
  Zero_line -> SetLineWidth(LineWidth);
  Zero_line -> SetLineStyle(LineStyle);
  Zero_line -> SetLineColor(Line_Col);
  Zero_line -> Draw();
  //delete Zero_line;
}

void plotQA_shape_HCAL()
{
  TH1F *h_ihcal = new TH1F("h_ihcal","h_ihcal",3000,1100,1400);
  for(int i_bin = 0; i_bin < 2000; ++i_bin)
  {
    h_ihcal->SetBinContent(i_bin+1,100.0);
    h_ihcal->SetBinError(i_bin+1,1.0);
  }

  h_ihcal->SetTitle("");
  h_ihcal->SetStats(0);
  h_ihcal->GetXaxis()->SetNdivisions(505);
  h_ihcal->GetXaxis()->SetTitle("x (mm)");
  h_ihcal->GetYaxis()->SetRangeUser(-400.0,-100.0);
  h_ihcal->GetYaxis()->SetTitle("y (mm)");

  TCanvas *c_ihcal = new TCanvas("c_ihcal","c_ihcal",10,10,800,800);
  c_ihcal->SetLeftMargin(0.15);
  c_ihcal->SetBottomMargin(0.15);
  c_ihcal->SetTicks(1,1);
  c_ihcal->SetGrid(1,1);
  h_ihcal->Draw("pE");

  double ihcal3_uL[2] = {1154.49, -189.06};
  double ihcal3_uR[2] = {1297.94, -349.22};
  double ihcal3_lR[2] = {1288.18, -357.80};
  double ihcal3_lL[2] = {1157.30, -205.56};
  PlotLine(ihcal3_uL[0],ihcal3_uR[0],ihcal3_uL[1],ihcal3_uR[1],1,2,2);
  PlotLine(ihcal3_uL[0],ihcal3_lL[0],ihcal3_uL[1],ihcal3_lL[1],1,2,2);
  PlotLine(ihcal3_lL[0],ihcal3_lR[0],ihcal3_lL[1],ihcal3_lR[1],1,2,2);
  PlotLine(ihcal3_lR[0],ihcal3_uR[0],ihcal3_lR[1],ihcal3_uR[1],1,2,2);

  double ihcal2_uL[2] = {1157.5, -151.44};
  double ihcal2_uR[2] = {1308.5, -286.96};
  double ihcal2_lR[2] = {1298.8, -297.39};
  double ihcal2_lL[2] = {1155.8, -163.92};
  PlotLine(ihcal2_uL[0],ihcal2_uR[0],ihcal2_uL[1],ihcal2_uR[1],2,2,2);
  PlotLine(ihcal2_uL[0],ihcal2_lL[0],ihcal2_uL[1],ihcal2_lL[1],2,2,2);
  PlotLine(ihcal2_lL[0],ihcal2_lR[0],ihcal2_lL[1],ihcal2_lR[1],2,2,2);
  PlotLine(ihcal2_lR[0],ihcal2_uR[0],ihcal2_lR[1],ihcal2_uR[1],2,2,2);
  c_ihcal->SaveAs("./figures/c_shape_ihcal.eps");

  TH1F *h_ohcal = new TH1F("h_ohcal","h_ohcal",3000,1500,3000);
  for(int i_bin = 0; i_bin < 2000; ++i_bin)
  {
    h_ohcal->SetBinContent(i_bin+1,100.0);
    h_ohcal->SetBinError(i_bin+1,1.0);
  }

  h_ohcal->SetTitle("");
  h_ohcal->SetStats(0);
  h_ohcal->GetXaxis()->SetNdivisions(505);
  h_ohcal->GetXaxis()->SetTitle("x (mm)");
  h_ohcal->GetYaxis()->SetRangeUser(-500.0,-200.0);
  h_ohcal->GetYaxis()->SetTitle("y (mm)");

  TCanvas *c_ohcal = new TCanvas("c_ohcal","c_ohcal",10,10,800,800);
  c_ohcal->SetLeftMargin(0.15);
  c_ohcal->SetBottomMargin(0.15);
  c_ohcal->SetTicks(1,1);
  c_ohcal->SetGrid(1,1);
  h_ohcal->Draw("pE");
  double ohcal_uL[2] = {1777.6, -433.5};
  double ohcal_uR[2] = {2600.4, -417.4};
  double ohcal_lR[2] = {2601.2, -459.8};
  double ohcal_lL[2] = {1770.9, -459.8};
  PlotLine(ohcal_uL[0],ohcal_uR[0],ohcal_uL[1],ohcal_uR[1],2,2,2);
  PlotLine(ohcal_uL[0],ohcal_lL[0],ohcal_uL[1],ohcal_lL[1],2,2,2);
  PlotLine(ohcal_lL[0],ohcal_lR[0],ohcal_lL[1],ohcal_lR[1],2,2,2);
  PlotLine(ohcal_lR[0],ohcal_uR[0],ohcal_lR[1],ohcal_uR[1],2,2,2);
  c_ohcal->SaveAs("./figures/c_shape_ohcal.eps");
}

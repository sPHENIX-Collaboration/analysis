#include "TSystem.h"
#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TLegend.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>

using namespace std;

float ErrorAdd(float x, float y)
{
    return sqrt(x*x+y*y);
}

float ErrTimes(float x, float y, float dx, float dy)
{
    return x*y*ErrorAdd(dx/x,dy/y);
}

float ErrDiv(float x, float y, float dx, float dy)
{
    return x/y*ErrorAdd(dx/x,dy/y);
}

void calLinearity()
{
  gSystem->Load("libPrototype4.so");
  gSystem->Load("libProto4_HCalShowerCalib.so");

  TGraphAsymmErrors *g_linearity = new TGraphAsymmErrors();
  TGraphAsymmErrors *g_resolution = new TGraphAsymmErrors();
  float mBeamEnergy[5] = {8.0,12.0,16.0,24.0,30.0};

  string inputlist = "EnergyScan.list";
  FILE *fp = fopen(inputlist.c_str(), "r");
  if(fp == NULL)
  {
    perror("Error opening file!");
  }
  else
  {
    char line[1024];
    int line_counter = 0;
    while(fgets(line,1024,fp))
    {
      line[strcspn(line, "\n")] = '\0';
      string inputfile = line;
      cout << "read in: " << inputfile.c_str() << endl;
      TFile *File_HCAL = TFile::Open(inputfile.c_str());
      TH1F *h_BeamMom = (TH1F*)File_HCAL->Get("hBeam_Mom")->Clone();
      // float inputEnergy = TMath::Abs(h_BeamMom->GetMean()); // get input energy
      float inputEnergy = mBeamEnergy[line_counter]; // get input energy

      TH1F *h_calib = (TH1F*)File_HCAL->Get("h_hcal_total_calib")->Clone();
      TF1 *f_gaus = new TF1("f_gaus","gaus",0,100);
      f_gaus->SetParameter(0,1.0);
      f_gaus->SetParameter(1,inputEnergy);
      f_gaus->SetParameter(2,1.0);
      h_calib->Fit(f_gaus,"NQ");
      float val_calib = f_gaus->GetParameter(1); // extract calibrated energy
      float err_calib = f_gaus->GetParError(1);
      g_linearity->SetPoint(line_counter,inputEnergy,val_calib);
      g_linearity->SetPointError(line_counter,0.0,0.0,err_calib,err_calib);

      float val_sigma = f_gaus->GetParameter(2); // extract calibrated energy
      float err_sigma = f_gaus->GetParError(2);
      float val_resolution = val_sigma/val_calib;
      float err_resolution = ErrDiv(val_sigma,val_calib,err_sigma,err_calib);
      g_resolution->SetPoint(line_counter,inputEnergy,val_resolution);
      g_resolution->SetPointError(line_counter,0.0,0.0,err_resolution,err_resolution);

      line_counter++;
      File_HCAL->Close();
      cout << "input energy is " << inputEnergy << ", calibrated energy is " << val_calib << " +/- " << err_calib << ", sigma is " << val_sigma << endl;
    }
  }

  TCanvas *c_Linearity = new TCanvas("c_Linearity","c_Linearity",10,10,800,800);
  c_Linearity->cd();
  c_Linearity->cd()->SetLeftMargin(0.15);
  c_Linearity->cd()->SetBottomMargin(0.15);
  c_Linearity->cd()->SetTicks(1,1);
  c_Linearity->cd()->SetGrid(0,0);

  TH1F *h_play = new TH1F("h_play","h_play",100,-0.5,99.5);
  for(int i_bin = 0; i_bin < 100; ++i_bin)
  {
    h_play->SetBinContent(i_bin+1,-100.0);
    h_play->SetBinError(i_bin+1,1.0);
  }
  h_play->SetTitle("");
  h_play->SetStats(0);

  h_play->GetXaxis()->SetTitle("Input Energy (GeV)");
  h_play->GetXaxis()->CenterTitle();
  h_play->GetXaxis()->SetRangeUser(0,40);

  h_play->GetYaxis()->SetTitle("Calibrated Energy (GeV)");
  h_play->GetYaxis()->CenterTitle();
  h_play->GetYaxis()->SetRangeUser(0,40);
  h_play->DrawCopy("pE");

  TGraphAsymmErrors *g_linearity_old = new TGraphAsymmErrors();
  string inputlinearity = "linearity.txt";
  FILE *flinear = fopen(inputlinearity.c_str(), "r");
  if(flinear == NULL)
  {
    perror("Error opening file!");
  }
  else
  {
    float x, y, err_x_low, err_x_high, err_y_low, err_y_high;
    char line[1024];
    int line_counter = 0;
    while(fgets(line,1024,flinear))
    {
      sscanf(&line[0], "%f %f %f %f %f %f", &x, &y, &err_x_low, &err_x_high, &err_y_low, &err_y_high);
      // cout << "x = " << x << ", y = " << y << ", err_x_low = " << err_x_low << ", err_x_high = " << err_x_high << ", err_y_low = " << err_y_low << ", err_y_high = " << err_y_high << endl;
      g_linearity_old->SetPoint(line_counter,x,y);
      g_linearity_old->SetPointError(line_counter,err_x_low,err_x_high,err_y_low,err_y_high);
      line_counter++;
    }
  }

  g_linearity_old->SetMarkerStyle(21);
  g_linearity_old->SetMarkerColor(2);
  g_linearity_old->SetMarkerSize(1.8);
  g_linearity_old->Draw("pE same");

  g_linearity->SetMarkerStyle(20);
  g_linearity->SetMarkerColor(kGray+2);
  g_linearity->SetMarkerSize(2.0);
  g_linearity->Draw("pE same");

  TLine *l_unity = new TLine(1.0,1.0,39.0,39.0);
  l_unity->SetLineColor(4);
  l_unity->SetLineStyle(2);
  l_unity->SetLineWidth(2);
  l_unity->Draw("l same");

  TLegend *leg_linear = new TLegend(0.2,0.6,0.5,0.8);
  leg_linear->SetBorderSize(0);
  leg_linear->SetFillColor(0);
  leg_linear->AddEntry(g_linearity_old,"#pi- T1044-2017","p");
  leg_linear->AddEntry(g_linearity,"#pi- T1044-2018","p");
  leg_linear->AddEntry(l_unity,"unity","l");
  leg_linear->Draw("same");

  c_Linearity->SaveAs("/gpfs/mnt/gpfs04/sphenix/user/xusun/HCAL_BeamTest/figures/c_Linearity.eps");

  TCanvas *c_Resolution = new TCanvas("c_Resolution","c_Resolution",10,10,800,800);
  c_Resolution->cd();
  c_Resolution->cd()->SetLeftMargin(0.15);
  c_Resolution->cd()->SetBottomMargin(0.15);
  c_Resolution->cd()->SetTicks(1,1);
  c_Resolution->cd()->SetGrid(0,0);

  h_play->GetXaxis()->SetTitle("Input Energy (GeV)");
  h_play->GetXaxis()->CenterTitle();
  h_play->GetXaxis()->SetRangeUser(0,40);

  h_play->GetYaxis()->SetTitle("#DeltaE/<E>");
  h_play->GetYaxis()->CenterTitle();
  h_play->GetYaxis()->SetRangeUser(0,0.7);
  h_play->DrawCopy("pE");

  TGraphAsymmErrors *g_resolution_old = new TGraphAsymmErrors();
  string inputresolution = "resolution.txt";
  FILE *fres = fopen(inputresolution.c_str(), "r");
  if(fres == NULL)
  {
    perror("Error opening file!");
  }
  else
  {
    float x, y, err_x_low, err_x_high, err_y_low, err_y_high;
    char line[1024];
    int line_counter = 0;
    while(fgets(line,1024,fres))
    {
      sscanf(&line[0], "%f %f %f %f %f %f", &x, &y, &err_x_low, &err_x_high, &err_y_low, &err_y_high);
      // cout << "x = " << x << ", y = " << y << ", err_x_low = " << err_x_low << ", err_x_high = " << err_x_high << ", err_y_low = " << err_y_low << ", err_y_high = " << err_y_high << endl;
      g_resolution_old->SetPoint(line_counter,x,y);
      g_resolution_old->SetPointError(line_counter,err_x_low,err_x_high,err_y_low,err_y_high);
      line_counter++;
    }
  }

  g_resolution_old->SetMarkerStyle(21);
  g_resolution_old->SetMarkerColor(2);
  g_resolution_old->SetMarkerSize(1.8);
  g_resolution_old->Draw("pE same");

  g_resolution->SetMarkerStyle(20);
  g_resolution->SetMarkerColor(kGray+2);
  g_resolution->SetMarkerSize(2.0);
  g_resolution->Draw("pE same");

  TLegend *leg_res = new TLegend(0.55,0.6,0.85,0.8);
  leg_res->SetBorderSize(0);
  leg_res->SetFillColor(0);
  leg_res->AddEntry(g_resolution_old,"#pi- T1044-2017","p");
  leg_res->AddEntry(g_resolution,"#pi- T1044-2018","p");
  leg_res->Draw("same");

  c_Resolution->SaveAs("/gpfs/mnt/gpfs04/sphenix/user/xusun/HCAL_BeamTest/figures/c_Resolution.eps");
}

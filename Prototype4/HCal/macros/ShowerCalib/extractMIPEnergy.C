#include <string>
#include <TString.h>
#include <TFile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphAsymmErrors.h>
#include <iostream>
#include <fstream>

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

void extractMIPEnergy()
{
  string inputenergy[4] = {"5GeV","8GeV","12GeV","60GeV"};
  string runId[4] = {"1087","0422","0571","0821"};
  float mean_energy[4] = {5.0,8.0,12.0,60.0};

  TH2F *h_mAsymmEnergy_pion[4];
  TH1F *h_mMIPEnergy_pion[4];
  TFile *File_InPut[4];

  for(int i_energy = 0; i_energy < 4; ++i_energy)
  {
    string inputfile = Form("/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_%s.root",runId[i_energy].c_str());
    File_InPut[i_energy] = TFile::Open(inputfile.c_str());
    h_mAsymmEnergy_pion[i_energy] = (TH2F*)File_InPut[i_energy]->Get("h_mAsymmEnergy_pion");
    h_mMIPEnergy_pion[i_energy] = (TH1F*)h_mAsymmEnergy_pion[i_energy]->ProjectionY()->Clone();
  }

  TF1 *f_gaus_MIP[4];
  float MIP_mean[4];
  float MIP_width[4];
  TCanvas *c_MIPEnergy = new TCanvas("c_MIPEnergy","c_MIPEnergy",1500,500);
  c_MIPEnergy->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_MIPEnergy->cd(i_pad+1);
    c_MIPEnergy->cd(i_pad+1)->SetLeftMargin(0.15);
    c_MIPEnergy->cd(i_pad+1)->SetBottomMargin(0.15);
    c_MIPEnergy->cd(i_pad+1)->SetTicks(1,1);
    c_MIPEnergy->cd(i_pad+1)->SetGrid(0,0);
    h_mMIPEnergy_pion[i_pad]->SetTitle(inputenergy[i_pad].c_str());
    h_mMIPEnergy_pion[i_pad]->GetXaxis()->SetTitle("Tower Calibrated Energy (GeV)");
    h_mMIPEnergy_pion[i_pad]->Draw("hE");
    string FuncName = Form("f_gaus_MIP_%d",i_pad);
    f_gaus_MIP[i_pad] = new TF1(FuncName.c_str(),"gaus",0,5);
    f_gaus_MIP[i_pad]->SetParameter(0,1.0);
    f_gaus_MIP[i_pad]->SetParameter(1,1.0);
    f_gaus_MIP[i_pad]->SetParameter(2,0.1);
    f_gaus_MIP[i_pad]->SetRange(0.4,0.9);
    h_mMIPEnergy_pion[i_pad]->Fit(f_gaus_MIP[i_pad],"NQR");
    f_gaus_MIP[i_pad]->SetLineColor(2);
    f_gaus_MIP[i_pad]->SetLineStyle(2);
    f_gaus_MIP[i_pad]->SetLineWidth(2);
    f_gaus_MIP[i_pad]->Draw("l same");
    MIP_mean[i_pad] = f_gaus_MIP[i_pad]->GetParameter(1);
    MIP_width[i_pad] = f_gaus_MIP[i_pad]->GetParameter(2);
    cout << "MIP for " << inputenergy[i_pad] << " is: " << MIP_mean[i_pad] << " +/- " << MIP_width[i_pad] << endl;
  }

  int i_MIP = 1; // use 8 GeV as defualt MIP cut
  cout << "default MIP for all energy chosen to be " << inputenergy[i_MIP] << " is: " << MIP_mean[i_MIP] << " +/- " << MIP_width[i_MIP] << endl;

  ofstream File_OutPut("MIPEnergy.txt");
  File_OutPut << "default MIP at " << inputenergy[i_MIP] << " is: " << MIP_mean[i_MIP] << " +/- " << MIP_width[i_MIP] << endl;
  File_OutPut.close();

  c_MIPEnergy->SaveAs("../figures/HCAL_ShowerCalib/c_MIPEnergy.eps");

#if 0
  TF1 *f_gaus_Reco[4];
  float Reco_mean[4];
  float err_mean[4];
  float Reco_width[4];
  float err_width[4];
  float Reco_resolution[4];
  float err_resolution[4];
  TCanvas *c_RecoEnergy = new TCanvas("c_RecoEnergy","c_RecoEnergy",1500,500);
  c_RecoEnergy->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_RecoEnergy->cd(i_pad+1);
    c_RecoEnergy->cd(i_pad+1)->SetLeftMargin(0.15);
    c_RecoEnergy->cd(i_pad+1)->SetBottomMargin(0.15);
    c_RecoEnergy->cd(i_pad+1)->SetTicks(1,1);
    c_RecoEnergy->cd(i_pad+1)->SetGrid(0,0);
    h_mMIPEnergy_pion[i_pad]->Draw("hE");

    string FuncName = Form("f_gaus_Reco_%d",i_pad);
    f_gaus_Reco[i_pad] = new TF1(FuncName.c_str(),"gaus",0,100);
    f_gaus_Reco[i_pad]->SetParameter(0,1.0);
    f_gaus_Reco[i_pad]->SetParameter(1,mean_energy[i_pad]/3.0);
    f_gaus_Reco[i_pad]->SetParameter(2,1.0);
    f_gaus_Reco[i_pad]->SetRange(MIP_mean[i_MIP]+2.0*MIP_width[i_MIP],0.5*mean_energy[i_pad]);
    h_mMIPEnergy_pion[i_pad]->Fit(f_gaus_Reco[i_pad],"NQR");
    f_gaus_Reco[i_pad]->SetLineColor(2);
    f_gaus_Reco[i_pad]->SetLineStyle(2);
    f_gaus_Reco[i_pad]->SetLineWidth(2);
    f_gaus_Reco[i_pad]->Draw("l same");

    Reco_mean[i_pad] = f_gaus_Reco[i_pad]->GetParameter(1);
    err_mean[i_pad] = f_gaus_Reco[i_pad]->GetParError(1);
    Reco_width[i_pad] = f_gaus_Reco[i_pad]->GetParameter(2);
    err_width[i_pad] = f_gaus_Reco[i_pad]->GetParError(2);
    cout << "Reco for " << inputenergy[i_pad] << " is: " << Reco_mean[i_pad] << " +/- " << Reco_width[i_pad] << endl;

    Reco_resolution[i_pad] = Reco_width[i_pad]/Reco_mean[i_pad];
    err_resolution[i_pad] = ErrDiv(Reco_width[i_pad],Reco_mean[i_pad],err_width[i_pad],err_mean[i_pad]);
  }

  TGraphAsymmErrors *g_lieanrity = new TGraphAsymmErrors();
  TGraphAsymmErrors *g_resolution = new TGraphAsymmErrors();
  for(int i_point = 0; i_point < 3; ++i_point)
  {
    g_lieanrity->SetPoint(i_point,mean_energy[i_point],Reco_mean[i_point]);
    g_lieanrity->SetPointError(i_point,0.0,0.0,err_mean[i_point],err_mean[i_point]);

    g_resolution->SetPoint(i_point,mean_energy[i_point],Reco_resolution[i_point]);
    g_resolution->SetPointError(i_point,0.0,0.0,err_resolution[i_point],err_resolution[i_point]);
  }

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
  h_play->GetXaxis()->SetRangeUser(0.0,20.0);

  h_play->GetYaxis()->SetTitle("Tower Calibrated Energy (GeV)");
  h_play->GetYaxis()->CenterTitle();
  h_play->GetYaxis()->SetRangeUser(0.0,10.0);
  h_play->DrawCopy("pE");

  g_lieanrity->SetMarkerStyle(24);
  g_lieanrity->SetMarkerColor(2);
  g_lieanrity->SetMarkerSize(1.2);
  g_lieanrity->Draw("PE same");

  TF1 *f_pol = new TF1("f_pol","pol1",0.0,20.0);
  f_pol->SetParameter(0,0.0);
  f_pol->SetParameter(1,1.0);
  f_pol->SetRange(0.0,20.0);
  g_lieanrity->Fit(f_pol,"NR");
  f_pol->SetLineColor(2);
  f_pol->SetLineStyle(2);
  f_pol->SetLineWidth(2);
  f_pol->Draw("l same");

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
  TCanvas *c_Resolution = new TCanvas("c_Resolution","c_Resolution",10,10,800,800);
  c_Resolution->cd();
  c_Resolution->cd()->SetLeftMargin(0.15);
  c_Resolution->cd()->SetBottomMargin(0.15);
  c_Resolution->cd()->SetTicks(1,1);
  c_Resolution->cd()->SetGrid(0,0);

  h_play->GetYaxis()->SetTitle("#DeltaE/<E>");
  h_play->GetYaxis()->CenterTitle();
  h_play->GetYaxis()->SetRangeUser(0.0,0.8);
  h_play->DrawCopy("pE");

  g_resolution->SetMarkerStyle(20);
  g_resolution->SetMarkerColor(kGray+2);
  g_resolution->SetMarkerSize(2.0);
  g_resolution->Draw("pE same");

  g_resolution_old->SetMarkerStyle(21);
  g_resolution_old->SetMarkerColor(2);
  g_resolution_old->SetMarkerSize(1.8);
  g_resolution_old->Draw("pE same");

  TLegend *leg_res = new TLegend(0.55,0.6,0.85,0.8);
  leg_res->SetBorderSize(0);
  leg_res->SetFillColor(0);
  leg_res->AddEntry(g_resolution_old,"#pi- T1044-2017","p");
  leg_res->AddEntry(g_resolution,"#pi- T1044-2018","p");
  leg_res->Draw("same");
#endif
}

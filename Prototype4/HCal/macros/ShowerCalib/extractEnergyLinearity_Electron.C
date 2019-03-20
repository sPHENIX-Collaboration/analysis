#include <string>
#include <TString.h>
#include <TFile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphAsymmErrors.h>
#include <TProfile.h>
#include <TLine.h>

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

void extractEnergyLinearity_Electron()
{
  string input_5GeV = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_1087.root";
  TFile *File_5GeV = TFile::Open(input_5GeV.c_str());
  TH2F *h_mAsymmEnergy_pion_5GeV = (TH2F*)File_5GeV->Get("h_mAsymmEnergy_electron_ShowerCalib");
  TH1F *h_mEnergy_pion_5GeV = (TH1F*)h_mAsymmEnergy_pion_5GeV->ProjectionY()->Clone();

  string input_8GeV = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_0422.root";
  TFile *File_8GeV = TFile::Open(input_8GeV.c_str());
  TH2F *h_mAsymmEnergy_pion_8GeV = (TH2F*)File_8GeV->Get("h_mAsymmEnergy_electron_ShowerCalib");
  TH1F *h_mEnergy_pion_8GeV = (TH1F*)h_mAsymmEnergy_pion_8GeV->ProjectionY()->Clone();

  string input_12GeV = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_0571.root";
  TFile *File_12GeV = TFile::Open(input_12GeV.c_str());
  TH2F *h_mAsymmEnergy_pion_12GeV = (TH2F*)File_12GeV->Get("h_mAsymmEnergy_electron_ShowerCalib");
  TH1F *h_mEnergy_pion_12GeV = (TH1F*)h_mAsymmEnergy_pion_12GeV->ProjectionY()->Clone();

  TCanvas *c_AsymmEnergy = new TCanvas("c_AsymmEnergy","c_AsymmEnergy",1500,500);
  c_AsymmEnergy->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_AsymmEnergy->cd(i_pad+1);
    c_AsymmEnergy->cd(i_pad+1)->SetLeftMargin(0.15);
    c_AsymmEnergy->cd(i_pad+1)->SetBottomMargin(0.15);
    c_AsymmEnergy->cd(i_pad+1)->SetTicks(1,1);
    c_AsymmEnergy->cd(i_pad+1)->SetGrid(0,0);
  }
  c_AsymmEnergy->cd(1);
  h_mAsymmEnergy_pion_5GeV->Draw("colz");
  h_mAsymmEnergy_pion_5GeV->ProfileX("p_mAsymmEnergy_pion_5GeV",1,-1,"i");
  p_mAsymmEnergy_pion_5GeV->SetMarkerStyle(20);
  p_mAsymmEnergy_pion_5GeV->SetMarkerColor(1);
  p_mAsymmEnergy_pion_5GeV->SetMarkerSize(1.0);
  // p_mAsymmEnergy_pion_5GeV->Draw("pE same");
  
  c_AsymmEnergy->cd(2);
  h_mAsymmEnergy_pion_8GeV->Draw("colz");
  h_mAsymmEnergy_pion_8GeV->ProfileX("p_mAsymmEnergy_pion_8GeV",1,-1,"i");
  p_mAsymmEnergy_pion_8GeV->SetMarkerStyle(20);
  p_mAsymmEnergy_pion_8GeV->SetMarkerColor(1);
  p_mAsymmEnergy_pion_8GeV->SetMarkerSize(1.0);
  // p_mAsymmEnergy_pion_8GeV->Draw("pE same");
  
  c_AsymmEnergy->cd(3);
  h_mAsymmEnergy_pion_12GeV->Draw("colz");
  h_mAsymmEnergy_pion_12GeV->ProfileX("p_mAsymmEnergy_pion_12GeV",1,-1,"i");
  p_mAsymmEnergy_pion_12GeV->SetMarkerStyle(20);
  p_mAsymmEnergy_pion_12GeV->SetMarkerColor(1);
  p_mAsymmEnergy_pion_12GeV->SetMarkerSize(1.0);
  // p_mAsymmEnergy_pion_12GeV->Draw("pE same");

  c_AsymmEnergy->SaveAs("../figures/HCAL_ShowerCalib/c_AsymmEnergy_ShowerCalib.eps");
  
  float mean_energy[4] = {5.0,8.0,12.0,60.0};
  float val_mean[4];
  float err_mean[4];
  float val_sigma[4];
  float err_sigma[4];
  float val_resolution[4];
  float err_resolution[4];

  TCanvas *c_Energy = new TCanvas("c_Energy","c_Energy",1500,500);
  c_Energy->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_Energy->cd(i_pad+1);
    c_Energy->cd(i_pad+1)->SetLeftMargin(0.15);
    c_Energy->cd(i_pad+1)->SetBottomMargin(0.15);
    c_Energy->cd(i_pad+1)->SetTicks(1,1);
    c_Energy->cd(i_pad+1)->SetGrid(0,0);
  }
  c_Energy->cd(1);
  h_mEnergy_pion_5GeV->Draw("hE");
  TF1 *f_gaus_5GeV = new TF1("f_gaus_5GeV","gaus",0,100);
  f_gaus_5GeV->SetParameter(0,1.0);
  f_gaus_5GeV->SetParameter(1,h_mEnergy_pion_5GeV->GetMean());
  f_gaus_5GeV->SetParameter(2,1.0);
  f_gaus_5GeV->SetRange(5.0,10.0);
  h_mEnergy_pion_5GeV->Fit(f_gaus_5GeV,"NR");
  f_gaus_5GeV->SetLineColor(2);
  f_gaus_5GeV->SetLineStyle(2);
  f_gaus_5GeV->SetLineWidth(2);
  f_gaus_5GeV->Draw("l same");
  val_mean[0]       = f_gaus_5GeV->GetParameter(1);
  err_mean[0]       = f_gaus_5GeV->GetParError(1);
  val_sigma[0]      = f_gaus_5GeV->GetParameter(2); // extract calibrated energy
  err_sigma[0]      = f_gaus_5GeV->GetParError(2);
  val_resolution[0] = val_sigma[0]/val_mean[0];
  err_resolution[0] = ErrDiv(val_sigma[0],val_mean[0],err_sigma[0],err_mean[0]);
  
  c_Energy->cd(2);
  h_mEnergy_pion_8GeV->Draw("hE");
  TF1 *f_gaus_8GeV = new TF1("f_gaus_8GeV","gaus",0,100);
  f_gaus_8GeV->SetParameter(0,1.0);
  f_gaus_8GeV->SetParameter(1,h_mEnergy_pion_8GeV->GetMean());
  f_gaus_8GeV->SetParameter(2,1.0);
  f_gaus_8GeV->SetRange(7.0,15.0);
  h_mEnergy_pion_8GeV->Fit(f_gaus_8GeV,"NR");
  f_gaus_8GeV->SetLineColor(2);
  f_gaus_8GeV->SetLineStyle(2);
  f_gaus_8GeV->SetLineWidth(2);
  f_gaus_8GeV->Draw("l same");
  val_mean[1]       = f_gaus_8GeV->GetParameter(1);
  err_mean[1]       = f_gaus_8GeV->GetParError(1);
  val_sigma[1]      = f_gaus_8GeV->GetParameter(2);
  err_sigma[1]      = f_gaus_8GeV->GetParError(2);
  val_resolution[1] = val_sigma[1]/val_mean[1];
  err_resolution[1] = ErrDiv(val_sigma[1],val_mean[1],err_sigma[1],err_mean[1]);
  
  c_Energy->cd(3);
  h_mEnergy_pion_12GeV->Draw("hE");
  TF1 *f_gaus_12GeV = new TF1("f_gaus_12GeV","gaus",0,100);
  f_gaus_12GeV->SetParameter(0,1.0);
  f_gaus_12GeV->SetParameter(1,h_mEnergy_pion_12GeV->GetMean());
  f_gaus_12GeV->SetParameter(2,1.0);
  f_gaus_12GeV->SetRange(10.0,24.0);
  h_mEnergy_pion_12GeV->Fit(f_gaus_12GeV,"NR");
  f_gaus_12GeV->SetLineColor(2);
  f_gaus_12GeV->SetLineStyle(2);
  f_gaus_12GeV->SetLineWidth(2);
  f_gaus_12GeV->Draw("l same");
  val_mean[2]       = f_gaus_12GeV->GetParameter(1);
  err_mean[2]       = f_gaus_12GeV->GetParError(1);
  val_sigma[2]      = f_gaus_12GeV->GetParameter(2);
  err_sigma[2]      = f_gaus_12GeV->GetParError(2);
  val_resolution[2] = val_sigma[2]/val_mean[2];
  err_resolution[2] = ErrDiv(val_sigma[2],val_mean[2],err_sigma[2],err_mean[2]);

  c_Energy->SaveAs("../figures/HCAL_ShowerCalib/c_Energy_ShowerCalib.eps");
  
  //------------------------linearity------------------------
  TGraphAsymmErrors *g_linearity = new TGraphAsymmErrors();
  for(int i_point = 0; i_point < 3; ++i_point)
  {
    g_linearity->SetPoint(i_point,mean_energy[i_point],val_mean[i_point]);
    g_linearity->SetPointError(i_point,0.0,0.0,err_mean[i_point],err_mean[i_point]);
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
  h_play->GetYaxis()->SetRangeUser(0.0,20.0);
  h_play->DrawCopy("pE");

  g_linearity->SetMarkerStyle(20);
  g_linearity->SetMarkerColor(kGray+2);
  g_linearity->SetMarkerSize(2.0);
  g_linearity->Draw("pE same");

  TLine *l_unity = new TLine(1.0,1.0,79.0,79.0);
  l_unity->SetLineColor(4);
  l_unity->SetLineStyle(2);
  l_unity->SetLineWidth(2);
  l_unity->Draw("l same");

  TLegend *leg_linear = new TLegend(0.2,0.6,0.5,0.8);
  leg_linear->SetBorderSize(0);
  leg_linear->SetFillColor(0);
  leg_linear->AddEntry(g_linearity,"e- T1044-2018","p");
  leg_linear->AddEntry(l_unity,"unity","l");
  leg_linear->Draw("same");

  c_Linearity->SaveAs("../figures/HCAL_ShowerCalib/c_Linearity_ShowerCalib.eps");

  //------------------------linearity------------------------

  //------------------------resolution-----------------------
  TGraphAsymmErrors *g_resolution = new TGraphAsymmErrors();
  for(int i_point = 0; i_point < 3; ++i_point)
  {
    g_resolution->SetPoint(i_point,mean_energy[i_point],val_resolution[i_point]);
    g_resolution->SetPointError(i_point,0.0,0.0,err_resolution[i_point],err_resolution[i_point]);
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

  TLegend *leg_res = new TLegend(0.55,0.6,0.85,0.8);
  leg_res->SetBorderSize(0);
  leg_res->SetFillColor(0);
  leg_res->AddEntry(g_resolution,"e- T1044-2018","p");
  leg_res->Draw("same");

  c_Resolution->SaveAs("../figures/HCAL_ShowerCalib/c_Resolution_ShowerCalib.eps");
  //------------------------resolution-----------------------

  string outputfile;
  outputfile = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/T1044_2018_electron.root";
  TFile *File_OutPut = new TFile(outputfile.c_str(),"RECREATE");
  File_OutPut->cd();
  g_linearity->SetName("g_linearity_2018_electron");
  g_linearity->Write();
  g_resolution->SetName("g_resolution_2018_electron");
  g_resolution->Write();
  File_OutPut->Close();
}

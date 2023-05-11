#include "TSystem.h"
#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TLegend.h"
#include "TString.h"

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

void calLinearity_ShowerCalib()
{
  TGraphAsymmErrors *g_linearity = new TGraphAsymmErrors();
  TGraphAsymmErrors *g_resolution = new TGraphAsymmErrors();
  float mBeamEnergy[6] = {4.0,8.0,12.0,16.0,24.0,30.0};
  string runID[6] = {"1241","0422","0571","1683","1666","1684"};

  TFile *File_InPut[6];
  TH2F *h_mAsymmEnergy_pion_ShowerCalib[6];
  TH1F *h_mEnergy_pion_ShowerCalib[6];

  for(int i_energy = 0; i_energy < 6; ++i_energy)
  {
    string inputfile = Form("/sphenix/user/xusun/software/data/beam/ShowerCalibAna/Proto4ShowerInfoRAW_%s.root",runID[i_energy].c_str());
    File_InPut[i_energy] = TFile::Open(inputfile.c_str());
    string title = Form("%1.0f GeV & #pi^{-}",mBeamEnergy[i_energy]);

    h_mAsymmEnergy_pion_ShowerCalib[i_energy] = (TH2F*)File_InPut[i_energy]->Get("h_mAsymmEnergy_pion_ShowerCalib")->Clone();
    h_mAsymmEnergy_pion_ShowerCalib[i_energy]->SetTitle(title.c_str());
    h_mAsymmEnergy_pion_ShowerCalib[i_energy]->GetYaxis()->SetTitle("Energy (GeV)");
    h_mAsymmEnergy_pion_ShowerCalib[i_energy]->GetYaxis()->CenterTitle();
    h_mAsymmEnergy_pion_ShowerCalib[i_energy]->GetYaxis()->SetNdivisions(505);
    h_mAsymmEnergy_pion_ShowerCalib[i_energy]->GetXaxis()->SetTitle("E_{Asymm}");
    h_mAsymmEnergy_pion_ShowerCalib[i_energy]->GetXaxis()->CenterTitle();

    h_mEnergy_pion_ShowerCalib[i_energy] = (TH1F*)h_mAsymmEnergy_pion_ShowerCalib[i_energy]->ProjectionY()->Clone();
    h_mEnergy_pion_ShowerCalib[i_energy]->SetStats(0);
    h_mEnergy_pion_ShowerCalib[i_energy]->SetTitle(title.c_str());
    h_mEnergy_pion_ShowerCalib[i_energy]->GetXaxis()->SetTitle("Energy (GeV)");
    h_mEnergy_pion_ShowerCalib[i_energy]->GetXaxis()->CenterTitle();
    h_mEnergy_pion_ShowerCalib[i_energy]->GetXaxis()->SetNdivisions(505);
    // h_mEnergy_pion_ShowerCalib[i_energy]->SetLineColor(2);
    h_mEnergy_pion_ShowerCalib[i_energy]->GetYaxis()->SetRangeUser(0,1.2*h_mEnergy_pion_ShowerCalib[i_energy]->GetMaximum());
  }

  TCanvas *c_energy_asymm = new TCanvas("c_energy_asymm","c_energy_asymm",800,1200);
  c_energy_asymm->Divide(2,3);
  for(int i_pad = 0; i_pad < 6; ++i_pad)
  {
    c_energy_asymm->cd(i_pad+1);
    c_energy_asymm->cd(i_pad+1)->SetLeftMargin(0.15);
    c_energy_asymm->cd(i_pad+1)->SetBottomMargin(0.15);
    c_energy_asymm->cd(i_pad+1)->SetTicks(1,1);
    c_energy_asymm->cd(i_pad+1)->SetGrid(0,0);
    h_mAsymmEnergy_pion_ShowerCalib[i_pad]->Draw("colz");
  }
  c_energy_asymm->SaveAs("./figures/sPHENIX_CollMeeting/c_energy_asymm_ShowerCalib.eps");

  TF1 *f_landau[6];
  float mpv[6], err_mpv[6];
  float sigma[6], err_sigma[6];
  TCanvas *c_showercalib = new TCanvas("c_showercalib","c_showercalib",800,1200);
  c_showercalib->Divide(2,3);
  for(int i_pad = 0; i_pad < 6; ++i_pad)
  {
    c_showercalib->cd(i_pad+1);
    c_showercalib->cd(i_pad+1)->SetLeftMargin(0.15);
    c_showercalib->cd(i_pad+1)->SetBottomMargin(0.15);
    c_showercalib->cd(i_pad+1)->SetTicks(1,1);
    c_showercalib->cd(i_pad+1)->SetGrid(0,0);
    h_mEnergy_pion_ShowerCalib[i_pad]->Draw();
    string FuncName = Form("f_landau_%1.0fGeV",mBeamEnergy[i_pad]);
    f_landau[i_pad] = new TF1(FuncName.c_str(),"landau",0,100);
    f_landau[i_pad]->SetParameter(0,1.0);
    f_landau[i_pad]->SetParameter(1,mBeamEnergy[i_pad]);
    f_landau[i_pad]->SetParameter(2,1.0);
    f_landau[i_pad]->SetRange(0.5*mBeamEnergy[i_pad],mBeamEnergy[i_pad]*2.0);
    if(i_pad > 3) f_landau[i_pad]->SetRange(0.75*mBeamEnergy[i_pad],mBeamEnergy[i_pad]*1.9);
    h_mEnergy_pion_ShowerCalib[i_pad]->Fit(f_landau[i_pad],"R");

    mpv[i_pad] = f_landau[i_pad]->GetParameter(1);
    err_mpv[i_pad] = f_landau[i_pad]->GetParError(1);
    g_linearity->SetPoint(i_pad,mBeamEnergy[i_pad],mpv[i_pad]);
    g_linearity->SetPointError(i_pad,0.0,0.0,err_mpv[i_pad],err_mpv[i_pad]);

    sigma[i_pad] = f_landau[i_pad]->GetParameter(2);
    err_sigma[i_pad] = f_landau[i_pad]->GetParError(2);
    float val_resolution = sigma[i_pad]/mpv[i_pad];
    float err_resolution = ErrDiv(sigma[i_pad],mpv[i_pad],err_sigma[i_pad],err_mpv[i_pad]);
    g_resolution->SetPoint(i_pad,mBeamEnergy[i_pad],val_resolution);
    g_resolution->SetPointError(i_pad,0.0,0.0,err_resolution,err_resolution);

    cout << "input energy is " << mBeamEnergy[i_pad] << ", calibrated energy is " << mpv[i_pad] << " +/- " << err_mpv[i_pad] << ", sigma is " << sigma[i_pad] << endl;
  }
  c_showercalib->SaveAs("./figures/sPHENIX_CollMeeting/c_showercalib.eps");

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

  c_Linearity->SaveAs("./figures/sPHENIX_CollMeeting/c_Linearity.eps");

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

  c_Resolution->SaveAs("./figures/sPHENIX_CollMeeting/c_Resolution.eps");

}

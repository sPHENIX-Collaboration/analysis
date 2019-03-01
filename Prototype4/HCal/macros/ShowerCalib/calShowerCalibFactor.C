#include <string>
#include "TString.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include <iostream>
#include <fstream>

void calShowerCalibFactor() // use 16 GeV for shower calbrtion
{
  string input_16GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/16GeV/Proto4ShowerInfoRAW_1683.root";
  TFile *File_InPut_16GeV = TFile::Open(input_16GeV.c_str());
  TH1F *h_mEnergy_16GeV_out = (TH1F*)File_InPut_16GeV->Get("h_mEnergyOut_pion_calib");
  TH2F *h_mAsymmEnergy_16GeV_levl = (TH2F*)File_InPut_16GeV->Get("h_mAsymmEnergy_pion_calib_leveling");
  TH1F *h_mEnergy_16GeV_levl = (TH1F*)h_mAsymmEnergy_16GeV_levl->ProjectionY()->Clone("h_mEnergy_16GeV_levl");

  TCanvas *c_shower = new TCanvas("c_shower","c_shower",1600,800);
  c_shower->Divide(2,1);
  for(int i_pad = 0; i_pad < 2; ++i_pad)
  {
    c_shower->cd(i_pad+1);
    c_shower->cd(i_pad+1)->SetLeftMargin(0.15);
    c_shower->cd(i_pad+1)->SetBottomMargin(0.15);
    c_shower->cd(i_pad+1)->SetTicks(1,1);
    c_shower->cd(i_pad+1)->SetGrid(0,0);
  }

  string energy_cut = "E_{in} > 0.2 GeV & E_{out} > 0.2 GeV";

  c_shower->cd(1);
  h_mAsymmEnergy_16GeV_levl->SetStats(0);
  h_mAsymmEnergy_16GeV_levl->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_16GeV_levl->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_16GeV_levl->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_16GeV_levl->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_16GeV_levl->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_16GeV_levl->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_16GeV_levl->Draw("colz");

  TLegend *leg_16GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_16GeV->SetBorderSize(0);
  leg_16GeV->SetFillColor(10);
  leg_16GeV->AddEntry(h_mAsymmEnergy_16GeV_levl,"16 GeV","h");
  leg_16GeV->Draw();

  c_shower->cd(2);
  h_mEnergy_16GeV_levl->SetStats(0);
  h_mEnergy_16GeV_levl->SetTitle("16 GeV & #pi^{-}");
  h_mEnergy_16GeV_levl->GetXaxis()->SetTitle("Energy (GeV)");
  h_mEnergy_16GeV_levl->GetXaxis()->CenterTitle();
  h_mEnergy_16GeV_levl->GetXaxis()->SetNdivisions(505);
  h_mEnergy_16GeV_levl->Draw();

  TF1 *f_gaus = new TF1("f_gaus","gaus",0,100);
  f_gaus->SetParameter(0,1.0);
  f_gaus->SetParameter(1,8.0);
  f_gaus->SetParameter(2,1.0);
  h_mEnergy_16GeV_levl->Fit(f_gaus,"MN");

  float norm = f_gaus->GetParameter(0);
  float mean = f_gaus->GetParameter(1); // extract calibrated energy
  float sigma = f_gaus->GetParameter(2); // extract calibrated energy
  f_gaus->SetParameter(0,norm);
  f_gaus->SetParameter(1,mean);
  f_gaus->SetParameter(2,sigma);
  // f_gaus->SetRange(mean-1.0*sigma,mean+1.0*sigma);
  f_gaus->SetRange(4.0,10.0);
  h_mEnergy_16GeV_levl->Fit(f_gaus,"MNR");
  float energy_twrcalib = f_gaus->GetParameter(1);

  f_gaus->SetLineColor(1);
  f_gaus->SetLineStyle(2);
  f_gaus->SetLineWidth(2);
  f_gaus->Draw("l same");

  const float showercalib = 16.0/energy_twrcalib;
  cout << "showercalib = " << showercalib << endl;

  h_mEnergy_16GeV_out->SetLineColor(2);
  h_mEnergy_16GeV_out->Draw("same");

  TF1 *f_landau= new TF1("f_landau","landau",0,100);
  f_landau->SetParameter(0,1.0);
  f_landau->SetParameter(1,2.0);
  f_landau->SetParameter(2,1.0);
  f_landau->SetRange(1.0,8.0);
  h_mEnergy_16GeV_out->Fit(f_landau,"MN");

  float energy_twrcalib_out = f_landau->GetParameter(1);

  f_landau->SetLineColor(2);
  f_landau->SetLineStyle(2);
  f_landau->SetLineWidth(2);
  f_landau->Draw("l same");

  const float showercalib_out = 16.0/energy_twrcalib_out;
  cout << "showercalib_out = " << showercalib_out << endl;

  ofstream File_OutPut("showercalib.txt");
  File_OutPut << "16 GeV: showercalib = " << showercalib << endl;
  File_OutPut << "16 GeV HCALOUT: showercalib_out = " << showercalib_out << endl;
  File_OutPut.close();
}

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

void plotQA_LevelingEnergy()
{
  string input_5GeV = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_1087.root"; // 5 GeV
  TFile *File_5GeV = TFile::Open(input_5GeV.c_str());
  TH2F *h_mAsymmEnergy_pion_5GeV = (TH2F*)File_5GeV->Get("h_mAsymmEnergy_pion");
  TH1F *h_mEnergy_pion_5GeV = (TH1F*)h_mAsymmEnergy_pion_5GeV->ProjectionY()->Clone();
  TH2F *h_mAsymmEnergy_pion_leveling_5GeV = (TH2F*)File_5GeV->Get("h_mAsymmEnergy_pion_leveling");
  TH1F *h_mEnergy_pion_leveling_5GeV = (TH1F*)h_mAsymmEnergy_pion_leveling_5GeV->ProjectionY()->Clone();

  string input_8GeV = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_0422.root"; // 8 GeV
  TFile *File_8GeV = TFile::Open(input_8GeV.c_str());
  TH2F *h_mAsymmEnergy_pion_8GeV = (TH2F*)File_8GeV->Get("h_mAsymmEnergy_pion");
  TH1F *h_mEnergy_pion_8GeV = (TH1F*)h_mAsymmEnergy_pion_8GeV->ProjectionY()->Clone();
  TH2F *h_mAsymmEnergy_pion_leveling_8GeV = (TH2F*)File_8GeV->Get("h_mAsymmEnergy_pion_leveling");
  TH1F *h_mEnergy_pion_leveling_8GeV = (TH1F*)h_mAsymmEnergy_pion_leveling_8GeV->ProjectionY()->Clone();

  string input_12GeV = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_0571.root"; // 12 GeV
  TFile *File_12GeV = TFile::Open(input_12GeV.c_str());
  TH2F *h_mAsymmEnergy_pion_12GeV = (TH2F*)File_12GeV->Get("h_mAsymmEnergy_pion");
  TH1F *h_mEnergy_pion_12GeV = (TH1F*)h_mAsymmEnergy_pion_12GeV->ProjectionY()->Clone();
  TH2F *h_mAsymmEnergy_pion_leveling_12GeV = (TH2F*)File_12GeV->Get("h_mAsymmEnergy_pion_leveling");
  TH1F *h_mEnergy_pion_leveling_12GeV = (TH1F*)h_mAsymmEnergy_pion_leveling_12GeV->ProjectionY()->Clone();

  TCanvas *c_AsymmEnergy_energy = new TCanvas("c_AsymmEnergy_energy","c_AsymmEnergy_energy",1500,500);
  c_AsymmEnergy_energy->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_AsymmEnergy_energy->cd(i_pad+1);
    c_AsymmEnergy_energy->cd(i_pad+1)->SetLeftMargin(0.15);
    c_AsymmEnergy_energy->cd(i_pad+1)->SetBottomMargin(0.15);
    c_AsymmEnergy_energy->cd(i_pad+1)->SetTicks(1,1);
    c_AsymmEnergy_energy->cd(i_pad+1)->SetGrid(0,0);
  }

  c_AsymmEnergy_energy->cd(1);
  h_mEnergy_pion_5GeV->SetLineColor(1);
  h_mEnergy_pion_5GeV->Draw("h");
  h_mEnergy_pion_leveling_5GeV->SetLineColor(2);
  h_mEnergy_pion_leveling_5GeV->Draw("h same");

  TLegend *leg_5GeV = new TLegend(0.55,0.7,0.75,0.85);
  leg_5GeV->SetBorderSize(0);
  leg_5GeV->SetFillColor(10);
  leg_5GeV->AddEntry(h_mEnergy_pion_5GeV,"before leveling","h");
  leg_5GeV->AddEntry(h_mEnergy_pion_leveling_5GeV,"after leveling","h");
  leg_5GeV->Draw();

  c_AsymmEnergy_energy->cd(2);
  h_mEnergy_pion_8GeV->SetLineColor(1);
  h_mEnergy_pion_8GeV->Draw("h");
  h_mEnergy_pion_leveling_8GeV->SetLineColor(2);
  h_mEnergy_pion_leveling_8GeV->Draw("h same");

  TLegend *leg_8GeV = new TLegend(0.55,0.7,0.75,0.85);
  leg_8GeV->SetBorderSize(0);
  leg_8GeV->SetFillColor(10);
  leg_8GeV->AddEntry(h_mEnergy_pion_8GeV,"before leveling","h");
  leg_8GeV->AddEntry(h_mEnergy_pion_leveling_8GeV,"after leveling","h");
  leg_8GeV->Draw();

  c_AsymmEnergy_energy->cd(3);
  h_mEnergy_pion_12GeV->SetLineColor(1);
  h_mEnergy_pion_12GeV->Draw("h");
  h_mEnergy_pion_leveling_12GeV->SetLineColor(2);
  h_mEnergy_pion_leveling_12GeV->Draw("h same");

  TLegend *leg_12GeV = new TLegend(0.55,0.7,0.75,0.85);
  leg_12GeV->SetBorderSize(0);
  leg_12GeV->SetFillColor(10);
  leg_12GeV->AddEntry(h_mEnergy_pion_12GeV,"before leveling","h");
  leg_12GeV->AddEntry(h_mEnergy_pion_leveling_12GeV,"after leveling","h");
  leg_12GeV->Draw();

}

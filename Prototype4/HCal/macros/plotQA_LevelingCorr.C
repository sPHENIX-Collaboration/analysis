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

void plotQA_LevelingCorr()
{
  string input_4GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/4GeV/Proto4ShowerInfoRAW_1241.root";
  TFile *File_InPut_4GeV = TFile::Open(input_4GeV.c_str());
  TH2F *h_mAsymmEnergy_4GeV_levl = (TH2F*)File_InPut_4GeV->Get("h_mAsymmEnergy_mixed_calib_leveling");

  //------------------------------------------

  string input_8GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/8GeV/Proto4ShowerInfoRAW_0422.root";
  TFile *File_InPut_8GeV = TFile::Open(input_8GeV.c_str());
  TH2F *h_mAsymmEnergy_8GeV_levl = (TH2F*)File_InPut_8GeV->Get("h_mAsymmEnergy_mixed_calib_leveling");

  //------------------------------------------

  string input_12GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/12GeV/Proto4ShowerInfoRAW_0571.root";
  TFile *File_InPut_12GeV = TFile::Open(input_12GeV.c_str());
  TH2F *h_mAsymmEnergy_12GeV_levl = (TH2F*)File_InPut_12GeV->Get("h_mAsymmEnergy_mixed_calib_leveling");

  //------------------------------------------

  string input_16GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/16GeV/Proto4ShowerInfoRAW_1683.root";
  TFile *File_InPut_16GeV = TFile::Open(input_16GeV.c_str());
  TH2F *h_mAsymmEnergy_16GeV_levl = (TH2F*)File_InPut_16GeV->Get("h_mAsymmEnergy_mixed_calib_leveling");

  //------------------------------------------

  string input_24GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/24GeV/Proto4ShowerInfoRAW_1666.root";
  TFile *File_InPut_24GeV = TFile::Open(input_24GeV.c_str());
  TH2F *h_mAsymmEnergy_24GeV_levl = (TH2F*)File_InPut_24GeV->Get("h_mAsymmEnergy_mixed_calib_leveling");

  //------------------------------------------

  string input_30GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/30GeV/Proto4ShowerInfoRAW_1684.root";
  TFile *File_InPut_30GeV = TFile::Open(input_30GeV.c_str());
  TH2F *h_mAsymmEnergy_30GeV_levl = (TH2F*)File_InPut_30GeV->Get("h_mAsymmEnergy_mixed_calib_leveling");

  //------------------------------------------

  TCanvas *c_mAsymmEnergy = new TCanvas("c_mAsymmEnergy","c_mAsymmEnergy",800,1200);
  c_mAsymmEnergy->Divide(2,3);
  for(int i_pad = 0; i_pad < 6; ++i_pad)
  {
    c_mAsymmEnergy->cd(i_pad+1);
    c_mAsymmEnergy->cd(i_pad+1)->SetLeftMargin(0.15);
    c_mAsymmEnergy->cd(i_pad+1)->SetBottomMargin(0.15);
    c_mAsymmEnergy->cd(i_pad+1)->SetTicks(1,1);
    c_mAsymmEnergy->cd(i_pad+1)->SetGrid(0,0);
  }
  string energy_cut = "E_{in} > 0.2 GeV & E_{out} > 0.2 GeV";

  c_mAsymmEnergy->cd(1);
  h_mAsymmEnergy_4GeV_levl->SetStats(0);
  h_mAsymmEnergy_4GeV_levl->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_4GeV_levl->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_4GeV_levl->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_4GeV_levl->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_4GeV_levl->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_4GeV_levl->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_4GeV_levl->Draw("colz");

  TLegend *leg_4GeV = new TLegend(0.55,0.7,0.75,0.85);
  leg_4GeV->SetBorderSize(0);
  leg_4GeV->SetFillColor(10);
  leg_4GeV->AddEntry(h_mAsymmEnergy_4GeV_levl,"4 GeV","h");
  leg_4GeV->Draw();

  c_mAsymmEnergy->cd(2);
  h_mAsymmEnergy_8GeV_levl->SetStats(0);
  h_mAsymmEnergy_8GeV_levl->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_8GeV_levl->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_8GeV_levl->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_8GeV_levl->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_8GeV_levl->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_8GeV_levl->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_8GeV_levl->Draw("colz");

  TLegend *leg_8GeV = new TLegend(0.55,0.7,0.75,0.85);
  leg_8GeV->SetBorderSize(0);
  leg_8GeV->SetFillColor(10);
  leg_8GeV->AddEntry(h_mAsymmEnergy_8GeV_levl,"8 GeV","h");
  leg_8GeV->Draw();

  c_mAsymmEnergy->cd(3);
  h_mAsymmEnergy_12GeV_levl->SetStats(0);
  h_mAsymmEnergy_12GeV_levl->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_12GeV_levl->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_12GeV_levl->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_12GeV_levl->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_12GeV_levl->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_12GeV_levl->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_12GeV_levl->Draw("colz");

  TLegend *leg_12GeV = new TLegend(0.55,0.7,0.75,0.85);
  leg_12GeV->SetBorderSize(0);
  leg_12GeV->SetFillColor(10);
  leg_12GeV->AddEntry(h_mAsymmEnergy_12GeV_levl,"12 GeV","h");
  leg_12GeV->Draw();

  c_mAsymmEnergy->cd(4);
  h_mAsymmEnergy_16GeV_levl->SetStats(0);
  h_mAsymmEnergy_16GeV_levl->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_16GeV_levl->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_16GeV_levl->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_16GeV_levl->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_16GeV_levl->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_16GeV_levl->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_16GeV_levl->Draw("colz");

  TLegend *leg_16GeV = new TLegend(0.55,0.7,0.75,0.85);
  leg_16GeV->SetBorderSize(0);
  leg_16GeV->SetFillColor(10);
  leg_16GeV->AddEntry(h_mAsymmEnergy_16GeV_levl,"16 GeV","h");
  leg_16GeV->Draw();

  c_mAsymmEnergy->cd(5);
  h_mAsymmEnergy_24GeV_levl->SetStats(0);
  h_mAsymmEnergy_24GeV_levl->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_24GeV_levl->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_24GeV_levl->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_24GeV_levl->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_24GeV_levl->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_24GeV_levl->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_24GeV_levl->Draw("colz");

  TLegend *leg_24GeV = new TLegend(0.55,0.7,0.75,0.85);
  leg_24GeV->SetBorderSize(0);
  leg_24GeV->SetFillColor(10);
  leg_24GeV->AddEntry(h_mAsymmEnergy_24GeV_levl,"24 GeV","h");
  leg_24GeV->Draw();

  c_mAsymmEnergy->cd(6);
  h_mAsymmEnergy_30GeV_levl->SetStats(0);
  h_mAsymmEnergy_30GeV_levl->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_30GeV_levl->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_30GeV_levl->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_30GeV_levl->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_30GeV_levl->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_30GeV_levl->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_30GeV_levl->Draw("colz");

  TLegend *leg_30GeV = new TLegend(0.55,0.7,0.75,0.85);
  leg_30GeV->SetBorderSize(0);
  leg_30GeV->SetFillColor(10);
  leg_30GeV->AddEntry(h_mAsymmEnergy_30GeV_levl,"30 GeV","h");
  leg_30GeV->Draw();

  string fig_Name = "./figures/sPHENIX_CollMeeting/c_mAsymmEnergy_mixed_levelinig.eps";
  c_mAsymmEnergy->SaveAs(fig_Name.c_str());
}

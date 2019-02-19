#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include <string>

void plotQA_cosmic_vertical(const int colID = 0)
{
  string inputfile_hcalin_sim = "/sphenix/user/xusun/software/data/cosmic/TowerCalibAna/Proto4TowerInfoSIM_HCALIN_0.root"; 
  TFile *File_HCALIN_SIM = TFile::Open(inputfile_hcalin_sim.c_str());

  TH1F *h_hcalin_tower_sim[16];
  TF1 *f_landau_hcalin_sim[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName = Form("h_mHCALIN_SIM_twr_%d",i_tower);
    h_hcalin_tower_sim[i_tower] = (TH1F*)File_HCALIN_SIM->Get(HistName.c_str())->Clone();
    string FuncName = Form("f_mHCALIN_SIM_twr_%d",i_tower);
    f_landau_hcalin_sim[i_tower] = new TF1(FuncName.c_str(),"landau",0,20);
  }

  TCanvas *c_hcalin_sim = new TCanvas("c_hcalin_sim","c_hcalin_sim",10,10,1600,1600);
  c_hcalin_sim->Divide(4,4);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col = 0; i_col < 4; ++i_col)
    {
      int i_pad = 4*(4-(i_row+1))+i_col+1;
      int i_tower = i_row + 4*i_col;
      c_hcalin_sim->cd(i_pad);
      h_hcalin_tower_sim[i_tower]->GetXaxis()->SetRangeUser(2,20);
      h_hcalin_tower_sim[i_tower]->Draw();

      for(int i_par = 0; i_par < 3; ++i_par)
      {
	f_landau_hcalin_sim[i_tower]->ReleaseParameter(i_par);
      }
      f_landau_hcalin_sim[i_tower]->SetParameter(0,10.0);
      f_landau_hcalin_sim[i_tower]->SetParameter(1,10.0);
      f_landau_hcalin_sim[i_tower]->SetParameter(2,10.0);
      f_landau_hcalin_sim[i_tower]->SetRange(0,20);
      if(h_hcalin_tower_sim[i_tower]->GetEntries() > 100)
      {
	h_hcalin_tower_sim[i_tower]->Fit(f_landau_hcalin_sim[i_tower],"R");
	string lable = Form("MPV %1.1f",f_landau_hcalin_sim[i_tower]->GetParameter(1));
	TLegend *leg = new TLegend(0.4,0.5,0.8,0.8);
	leg->SetFillColor(0);
	leg->SetBorderSize(0);
	leg->AddEntry(h_hcalin_tower_sim[i_tower],lable.c_str(),"L");
	leg->Draw("same");
      }
    }
  }
  c_hcalin_sim->SaveAs("./figures/c_hcalin_sim_col_0.eps");

  string inputfile_hcalin_raw = "/sphenix/user/xusun/software/data/cosmic/TowerCalibAna/Proto4TowerInfoRAW_HCALIN_0.root"; 
  TFile *File_HCALIN_RAW = TFile::Open(inputfile_hcalin_raw.c_str());

  TH1F *h_hcalin_tower_raw[16];
  TF1 *f_landau_hcalin_raw[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName = Form("h_mHCALIN_RAW_twr_%d",i_tower);
    h_hcalin_tower_raw[i_tower] = (TH1F*)File_HCALIN_RAW->Get(HistName.c_str())->Clone();
    string FuncName = Form("f_mHCALIN_RAW_twr_%d",i_tower);
    f_landau_hcalin_raw[i_tower] = new TF1(FuncName.c_str(),"landau",0,20);
  }

  TCanvas *c_hcalin_raw = new TCanvas("c_hcalin_raw","c_hcalin_raw",10,10,1600,1600);
  c_hcalin_raw->Divide(4,4);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col = 0; i_col < 4; ++i_col)
    {
      int i_pad = 4*(4-(i_row+1))+i_col+1;
      int i_tower = i_row + 4*i_col;
      c_hcalin_raw->cd(i_pad);
      c_hcalin_raw->cd(i_pad)->SetLogy();
      h_hcalin_tower_raw[i_tower]->Draw();

      /*
      for(int i_par = 0; i_par < 3; ++i_par)
      {
	f_landau_hcalin_raw[i_tower]->ReleaseParameter(i_par);
      }
      f_landau_hcalin_raw[i_tower]->SetParameter(0,10.0);
      f_landau_hcalin_raw[i_tower]->SetParameter(1,10.0);
      f_landau_hcalin_raw[i_tower]->SetParameter(2,10.0);
      f_landau_hcalin_raw[i_tower]->SetRange(0,20);
      if(h_hcalin_tower_raw[i_tower]->GetEntries() > 100)
      {
	h_hcalin_tower_raw[i_tower]->Fit(f_landau_hcalin_raw[i_tower],"R");
	string lable = Form("MPV %1.1f",f_landau_hcalin_raw[i_tower]->GetParameter(1));
	TLegend *leg = new TLegend(0.4,0.5,0.8,0.8);
	leg->SetFillColor(0);
	leg->SetBorderSize(0);
	leg->AddEntry(h_hcalin_tower_raw[i_tower],lable.c_str(),"L");
	leg->Draw("same");
      }
      */
    }
  }
  c_hcalin_raw->SaveAs("./figures/c_hcalin_raw_col_0.eps");

  /*
  string inputfile_hcalout = "/sphenix/user/xusun/software/data/cosmic/TowerCalib/Proto4TowerCalib_HCALOUT_00.root"; 
  TFile *File_HCALOUT = TFile::Open(inputfile_hcalout.c_str());

  TH1F *h_hcalout_tower_sim[16];
  TF1 *f_landau_hcalout[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName = Form("h_hcalout_tower_%d_sim",i_tower);
    h_hcalout_tower_sim[i_tower] = (TH1F*)File_HCALOUT->Get(HistName.c_str())->Clone();
    string FuncName = Form("f_hcalout_tower_%d_sim",i_tower);
    f_landau_hcalout[i_tower] = new TF1(FuncName.c_str(),"landau",0,20);
  }

  TCanvas *c_hcalout = new TCanvas("c_hcalout","c_hcalout",10,10,1600,1600);
  c_hcalout->Divide(4,4,0,0);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col = 0; i_col < 4; ++i_col)
    {
      int i_pad = 4*(4-(i_row+1))+i_col+1;
      int i_tower = i_row + 4*i_col;
      c_hcalout->cd(i_pad);
      h_hcalout_tower_sim[i_tower]->GetXaxis()->SetRangeUser(2,20);
      h_hcalout_tower_sim[i_tower]->Draw();

      for(int i_par = 0; i_par < 3; ++i_par)
      {
	f_landau_hcalout[i_tower]->ReleaseParameter(i_par);
      }
      f_landau_hcalout[i_tower]->SetParameter(0,10.0);
      f_landau_hcalout[i_tower]->SetParameter(1,10.0);
      f_landau_hcalout[i_tower]->SetParameter(2,10.0);
      f_landau_hcalout[i_tower]->SetRange(0,20);
      if(h_hcalout_tower_sim[i_tower]->GetEntries() > 0)
      {
	h_hcalout_tower_sim[i_tower]->Fit(f_landau_hcalout[i_tower],"R");
	string lable = Form("MPV %1.1f",f_landau_hcalout[i_tower]->GetParameter(1));
	TLegend *leg = new TLegend(0.4,0.5,0.8,0.8);
	leg->SetFillColor(0);
	leg->SetBorderSize(0);
	leg->AddEntry(h_hcalout_tower_sim[i_tower],lable.c_str(),"L");
	leg->Draw("same");
      }
    }
  }
  c_hcalout->SaveAs("./figures/c_hcalout_vertical.eps");
  */
}

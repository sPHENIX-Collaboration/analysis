#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"

void plotQA_noise()
{
  int hbdchanIHC[4][4] = {{16, 12, 8, 4},
                          {15, 11, 7, 3},
                          {14, 10, 6, 2},
                          {13,  9, 5, 1}};

  string inputfile = "/sphenix/user/xusun/software/data/cosmic/TowerCalibAna/Proto4TowerInfoRAW_HCALIN_3.root"; 
  TFile *File_InPut = TFile::Open(inputfile.c_str());

  TH1F *h_hcal_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName = Form("h_mHCALIN_RAW_twr_%d",i_tower);
    h_hcal_tower_calib[i_tower] = (TH1F*)File_InPut->Get(HistName.c_str())->Clone();
    h_hcal_tower_calib[i_tower]->Sumw2();
  }

  TCanvas *c_hcal = new TCanvas("c_hcal","c_hcal",10,10,1600,1600);
  c_hcal->Divide(4,4);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col = 0; i_col < 4; ++i_col)
    {
      int i_pad = 4*i_row+i_col+1;
      int i_tower = hbdchanIHC[i_row][i_col]-1;
      c_hcal->cd(i_pad);
      c_hcal->cd(i_pad)->SetLogy();
      h_hcal_tower_calib[i_tower]->GetYaxis()->SetRangeUser(1.0,500.0);
      h_hcal_tower_calib[i_tower]->Draw();
    }
  }

  TH1F *h_hcal_noise;
  for(int i_tower = 0; i_tower < 12; ++i_tower)
  {
    if(i_tower == 0) h_hcal_noise = (TH1F*)h_hcal_tower_calib[i_tower]->Clone();
    else h_hcal_noise->Add(h_hcal_tower_calib[i_tower],1.0);
  }
  h_hcal_noise->Scale(1.0/12.0);

  TH1F *h_hcal_signal;
  for(int i_tower = 12; i_tower < 16; ++i_tower)
  {
    if(i_tower == 12) h_hcal_signal = (TH1F*)h_hcal_tower_calib[i_tower]->Clone();
    else h_hcal_signal->Add(h_hcal_tower_calib[i_tower],1.0);
  }
  h_hcal_signal->Scale(1.0/4.0);

  TCanvas *c_noise = new TCanvas("c_noise","c_noise",10,10,1600,800);
  c_noise->Divide(2,1);
  for(int i_pad = 0; i_pad < 2; ++i_pad)
  {
    c_noise->cd(i_pad+1);
    c_noise->cd(i_pad+1)->SetLeftMargin(0.15);
    c_noise->cd(i_pad+1)->SetBottomMargin(0.15);
    c_noise->cd(i_pad+1)->SetTicks(1,1);
    c_noise->cd(i_pad+1)->SetGrid(0,0);
    c_noise->cd(i_pad+1)->SetLogy();
  }

  c_noise->cd(1);
  TH1F *h_play = new TH1F("h_play","h_play",500,-0.5,99.5);
  for(int i_bin = 0; i_bin < 500; ++i_bin)
  {
    h_play->SetBinContent(i_bin+1,-10000.0);
    h_play->SetBinError(i_bin+1,1.0);
  }
  h_play->SetStats(0);
  h_play->SetTitle("cosmic signal");
  h_play->SetLineColor(1);
  h_play->GetXaxis()->SetTitle("energy (MeV)");
  h_play->GetXaxis()->CenterTitle();
  h_play->GetXaxis()->SetNdivisions(505);
  h_play->GetYaxis()->SetRangeUser(1.0,500.0);
  h_play->DrawCopy("h");

  for(i_tower = 12; i_tower < 16; ++i_tower)
  {
    h_hcal_tower_calib[i_tower]->SetStats(0);
    h_hcal_tower_calib[i_tower]->SetLineColor(i_tower-12+1);
    h_hcal_tower_calib[i_tower]->GetXaxis()->SetTitle("energy (MeV)");
    h_hcal_tower_calib[i_tower]->GetXaxis()->CenterTitle();
    h_hcal_tower_calib[i_tower]->GetXaxis()->SetNdivisions(505);
    h_hcal_tower_calib[i_tower]->DrawCopy("h same");
  }

  c_noise->cd(2);
  h_hcal_noise->SetStats(0);
  h_hcal_noise->SetTitle("noise (averaged over all non-triggered towers)");
  h_hcal_noise->SetLineColor(1);
  h_hcal_noise->GetXaxis()->SetTitle("energy (MeV)");
  h_hcal_noise->GetXaxis()->CenterTitle();
  h_hcal_noise->GetXaxis()->SetNdivisions(505);
  h_hcal_noise->GetYaxis()->SetRangeUser(1.0,500.0);
  h_hcal_noise->DrawCopy("h");

  TCanvas *c_noise_ave = new TCanvas("c_noise_ave","c_noise_ave",10,10,1800,600);
  c_noise_ave->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_noise_ave->cd(i_pad+1);
    c_noise_ave->cd(i_pad+1)->SetLeftMargin(0.15);
    c_noise_ave->cd(i_pad+1)->SetBottomMargin(0.15);
    c_noise_ave->cd(i_pad+1)->SetTicks(1,1);
    c_noise_ave->cd(i_pad+1)->SetGrid(0,0);
    c_noise_ave->cd(i_pad+1)->SetLogy();
  }

  c_noise_ave->cd(1);
  h_hcal_signal->SetStats(0);
  h_hcal_signal->SetTitle("signal (averaged over all triggered towers)");
  h_hcal_signal->GetXaxis()->SetTitle("energy (MeV)");
  h_hcal_signal->GetXaxis()->CenterTitle();
  h_hcal_signal->GetXaxis()->SetNdivisions(505);
  h_hcal_signal->GetYaxis()->SetRangeUser(1.0,500.0);
  h_hcal_signal->SetLineColor(2);
  h_hcal_signal->Draw("h same");

  c_noise_ave->cd(2);
  h_hcal_noise->SetStats(0);
  h_hcal_noise->SetTitle("noise (averaged over all non-triggered towers)");
  h_hcal_noise->SetLineColor(1);
  h_hcal_noise->GetXaxis()->SetTitle("energy (MeV)");
  h_hcal_noise->GetXaxis()->CenterTitle();
  h_hcal_noise->GetXaxis()->SetNdivisions(505);
  h_hcal_noise->GetYaxis()->SetRangeUser(1.0,500.0);
  h_hcal_noise->Draw("h");

  c_noise_ave->cd(3);
  c_noise_ave->cd(3)->SetLogy(0);
  TH1F *h_ratio = (TH1F*)h_hcal_noise->Clone("h_ratio");
  h_ratio->SetTitle("noise/signal");
  h_ratio->Divide(h_hcal_signal);
  // h_ratio->Reset();
  // h_ratio->Divide(h_hcal_noise,h_hcal_signal,1,1,"B");
  h_ratio->GetYaxis()->SetRangeUser(-0.2,0.6);
  h_ratio->GetXaxis()->SetRangeUser(0.0,60.0);
  h_ratio->SetMarkerStyle(20);
  h_ratio->SetMarkerColor(1);
  h_ratio->SetMarkerSize(0.8);
  h_ratio->Draw("pE");

  c_hcal->SaveAs("./figures/sPHENIX_CollMeeting/c_cosmic_hcalin.eps");
  c_noise->SaveAs("./figures/sPHENIX_CollMeeting/c_noise_hcalin.eps");
  c_noise_ave->SaveAs("./figures/sPHENIX_CollMeeting/c_noise_hcalin_averaged.eps");
}

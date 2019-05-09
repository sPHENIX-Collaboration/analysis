#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TLegend.h"
#include "TCanvas.h"
#include <iostream>
#include <fstream>

void calSampleFrac()
{
  TFile *File_InPut_HCALIN = TFile::Open("/sphenix/user/xusun/software/data/beam/SampleFrac/Proto4SampleFrac_HCALIN.root");
  TH1F *h_sf_in = (TH1F*)File_InPut_HCALIN->Get("h_QAG4Sim_HCALIN_G4Hit_SF");
  TH2F *h_mCal_in = (TH2F*)File_InPut_HCALIN->Get("h_QAG4Sim_HCALIN_G4Hit_XY_cal");
  TH2F *h_mAbs_in = (TH2F*)File_InPut_HCALIN->Get("h_QAG4Sim_HCALIN_G4Hit_XY_abs");

  TFile *File_InPut_HCALOUT = TFile::Open("/sphenix/user/xusun/software/data/beam/SampleFrac/Proto4SampleFrac_HCALOUT.root");
  TH1F *h_sf_out = (TH1F*)File_InPut_HCALOUT->Get("h_QAG4Sim_HCALOUT_G4Hit_SF");
  TH2F *h_mCal_out = (TH2F*)File_InPut_HCALOUT->Get("h_QAG4Sim_HCALOUT_G4Hit_XY_cal");
  TH2F *h_mAbs_out = (TH2F*)File_InPut_HCALOUT->Get("h_QAG4Sim_HCALOUT_G4Hit_XY_abs");

  TH1F *h_play = new TH1F("h_play","h_play",500,0,500);
  for(int i_bin = 0; i_bin < 500; ++i_bin)
  {
    h_play->SetBinContent(i_bin+1,-10000.0);
    h_play->SetBinError(i_bin+1,1.0);
  }
  // h_play->SetTitle("G4Hit Display");
  h_play->SetStats(0);
  h_play->GetYaxis()->SetRangeUser(-100,100);
  h_play->GetXaxis()->SetRangeUser(50,350);

  TCanvas *c_cal_in = new TCanvas("c_cal","c_cal",10,10,1000,1000);
  c_cal->Divide(2,2);
  for(int i_pad = 0; i_pad < 4; ++i_pad)
  {
    c_cal->cd(i_pad+1)->SetLeftMargin(0.15);
    c_cal->cd(i_pad+1)->SetBottomMargin(0.15);
    c_cal->cd(i_pad+1)->SetGrid(0,0);
    c_cal->cd(i_pad+1)->SetTicks(1,1);
  }

  c_cal->cd(1);
  h_play->SetTitle("HCALIN Cal G4Hit Display");
  h_play->DrawCopy("pE");
  h_mCal_in->Draw("colz same");

  c_cal->cd(2);
  h_play->SetTitle("HCALOUT Cal G4Hit Display");
  h_play->DrawCopy("pE");
  h_mCal_out->Draw("colz same");

  c_cal->cd(3);
  h_play->SetTitle("HCALIN Abs G4Hit Display");
  h_play->DrawCopy("pE");
  h_mAbs_in->Draw("colz same");

  c_cal->cd(4);
  h_play->SetTitle("HCALOUT Abs G4Hit Display");
  h_play->DrawCopy("pE");
  h_mAbs_out->Draw("colz same");

  c_cal->SaveAs("./figures/sPHENIX_CollMeeting/c_G4HitDisplay.eps");

  float nSigma = 1.5;

  TCanvas *c_play = new TCanvas("c_play","c_play",10,10,1600,800);
  c_play->Divide(2,1);
  for(int i_pad = 0; i_pad < 2; ++i_pad)
  {
    c_play->cd(i_pad+1)->SetLeftMargin(0.15);
    c_play->cd(i_pad+1)->SetBottomMargin(0.15);
    c_play->cd(i_pad+1)->SetGrid(0,0);
    c_play->cd(i_pad+1)->SetTicks(1,1);
  }

  c_play->cd(1);
  h_sf_in->GetXaxis()->SetRangeUser(0.0,0.3);
  h_sf_in->Draw("hE");
  TF1 *f_sf_in = new TF1("f_sf_in","gaus",0.0,0.3);
  f_sf_in->SetParameter(0,100);
  f_sf_in->SetParameter(1,0.1);
  f_sf_in->SetParameter(2,0.1);
  f_sf_in->SetRange(0.0,0.3);
  h_sf_in->Fit(f_sf_in,"NR");

  float mean_sf_in = f_sf_in->GetParameter(1);
  float sigma_sf_in = f_sf_in->GetParameter(2);
  f_sf_in->SetParameter(0,100);
  f_sf_in->SetParameter(1,mean_sf_in);
  f_sf_in->SetParameter(2,sigma_sf_in);
  f_sf_in->SetRange(mean_sf_in-nSigma*sigma_sf_in,mean_sf_in+nSigma*sigma_sf_in);
  h_sf_in->Fit(f_sf_in,"R");
  float sf_in = f_sf_in->GetParameter(1);

  std::string label_sf_in = Form("inner sf: %1.4f",sf_in);
  TLegend *leg_in = new TLegend(0.5,0.5,0.9,0.8);                                                     
  leg_in->SetFillColor(0);
  leg_in->SetBorderSize(0);
  leg_in->AddEntry(f_sf_in,label_sf_in.c_str(),"L");
  leg_in->Draw("same");

  c_play->cd(2);
  h_sf_out->GetXaxis()->SetRangeUser(0.0,0.3);
  h_sf_out->Draw("hE");
  TF1 *f_sf_out = new TF1("f_sf_out","gaus",0.0,0.3);
  f_sf_out->SetParameter(0,100);
  f_sf_out->SetParameter(1,0.03);
  f_sf_out->SetParameter(2,0.03);
  f_sf_out->SetRange(0.0,0.3);
  h_sf_out->Fit(f_sf_out,"NR");
  float mean_sf_out = f_sf_out->GetParameter(1);
  float sigma_sf_out = f_sf_out->GetParameter(2);
  f_sf_out->SetParameter(0,100);
  f_sf_out->SetParameter(1,mean_sf_out);
  f_sf_out->SetParameter(2,sigma_sf_out);
  f_sf_out->SetRange(mean_sf_out-nSigma*sigma_sf_out,mean_sf_out+nSigma*sigma_sf_out);
  h_sf_out->Fit(f_sf_out,"R");
  float sf_out = f_sf_out->GetParameter(1);

  std::string label_sf_out = Form("outer sf: %1.4f",sf_out);
  TLegend *leg_out = new TLegend(0.4,0.5,0.8,0.8);                                                     
  leg_out->SetFillColor(0);
  leg_out->SetBorderSize(0);
  leg_out->AddEntry(f_sf_out,label_sf_out.c_str(),"L");
  leg_out->Draw("same");

  c_play->SaveAs("./figures/sPHENIX_CollMeeting/c_sf.eps");

  ofstream File_OutPut("samplefrac.txt");
  File_OutPut << "inner sf = " << sf_in << endl;
  File_OutPut << "outer sf = " << sf_out << endl;
  File_OutPut.close();
}


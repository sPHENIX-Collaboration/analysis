#include <string>
#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH3F.h"
#include "TLegend.h"

void plotQA_cosmic_mat_XY()
{
  string inputdir = "Cosmic_orig";
  // string inputdir = "Cosmic_Scint";
  // string inputdir = "BeamTest_SF_orig";
  // string inputdir = "BeamTest_SF_Scint";
  string inputfile_in = Form("./OutPut/SampleFrac/%s/Proto4SampleFrac_HCALIN.root",inputdir.c_str());
  TFile *File_HCALIN = TFile::Open(inputfile_in.c_str());
  TH2F *h_in_cal = (TH2F*)File_HCALIN->Get("h_QAG4Sim_HCALIN_G4Hit_Mat_XY_cal");
  TH2F *h_in_abs = (TH2F*)File_HCALIN->Get("h_QAG4Sim_HCALIN_G4Hit_Mat_XY_abs");

  string inputfile_out = Form("./OutPut/SampleFrac/%s/Proto4SampleFrac_HCALOUT.root",inputdir.c_str());
  TFile *File_HCALOUT = TFile::Open(inputfile_out.c_str());
  TH2F *h_out_cal = (TH2F*)File_HCALOUT->Get("h_QAG4Sim_HCALOUT_G4Hit_Mat_XY_cal");
  TH2F *h_out_abs = (TH2F*)File_HCALOUT->Get("h_QAG4Sim_HCALOUT_G4Hit_Mat_XY_abs");

  TH2F *h_cal = (TH2F*)h_in_cal->Clone("h_cal");
  h_cal->Add(h_out_cal,1.0);

  TH2F *h_abs = (TH2F*)h_in_abs->Clone("h_abs");
  h_abs->Add(h_out_abs,1.0);

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

  TCanvas *c_cal = new TCanvas("c_cal","c_cal",10,10,1600,800);
  c_cal->Divide(2,1);
  for(int i_pad = 0; i_pad < 2; ++i_pad)
  {
    c_cal->cd(i_pad+1)->SetLeftMargin(0.15);
    c_cal->cd(i_pad+1)->SetBottomMargin(0.15);
    c_cal->cd(i_pad+1)->SetGrid(0,0);
    c_cal->cd(i_pad+1)->SetTicks(1,1);
  }

  c_cal->cd(1);
  h_play->SetTitle("Cal G4Hit Display");
  h_play->DrawCopy("pE");
  // h_in_cal->Draw("colz same");
  // h_out_cal->Draw("col same");
  h_cal->Draw("colz same");

  c_cal->cd(2);
  h_play->SetTitle("Abs G4Hit Display");
  h_play->DrawCopy("pE");
  // h_in_abs->Draw("colz same");
  // h_out_abs->Draw("col same");
  h_abs->Draw("colz same");

  string output_fig = Form("./figures/c_G4Hit_%s_Mat.eps",inputdir.c_str());
  c_cal->SaveAs(output_fig.c_str());
}

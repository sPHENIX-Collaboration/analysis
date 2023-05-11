#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH3F.h"

void plotQA_cosmic_hcalin()
{
  TCanvas *c_samplefrac = new TCanvas("c_samplefrac","c_samplefrac",10,10,1200,1500);
  c_samplefrac->Divide(4,5);
  for(int i_pos = 0; i_pos < 5; ++i_pos)
  {
    for(int i_add = 0; i_add < 4; ++i_add)
    {
      int i_pad = 4*i_pos + i_add;
      c_samplefrac->cd(i_pad+1);
      c_samplefrac->cd(i_pad+1)->SetLeftMargin(0.15);
      c_samplefrac->cd(i_pad+1)->SetBottomMargin(0.15);
      c_samplefrac->cd(i_pad+1)->SetGrid(0,0);
      c_samplefrac->cd(i_pad+1)->SetTicks(1,1);
      std::string inputfile = Form("./OutPut/SampleFrac/Proto4SampleFrac_HCALIN_%d%d.root",i_pos,i_add);
      TFile *File_InPut = TFile::Open(inputfile.c_str());
      TH1F *h_samplefrac = (TH1F*)File_InPut->Get("h_QAG4Sim_HCALIN_G4Hit_SF");
      h_samplefrac->DrawCopy();
      h_samplefrac->Reset();
      File_InPut->Close();
    }
  }
  c_samplefrac->SaveAs("./figures/c_cosmic_hcalin_sf.eps");

  TFile *File_InPut = TFile::Open("./OutPut/SampleFrac/Proto4SampleFrac_HCALIN.root");
  TH3F *h_track = (TH3F*)File_InPut->Get("h_QAG4Sim_HCALIN_G4Hit_XYZ");
  TH1F *h_sf = (TH1F*)File_InPut->Get("h_QAG4Sim_HCALIN_G4Hit_SF");

  TCanvas *c_cosmic = new TCanvas("c_cosmic","c_cosmic",10,10,800,800);
  c_cosmic->cd()->SetLeftMargin(0.15);
  c_cosmic->cd()->SetBottomMargin(0.15);
  c_cosmic->cd()->SetGrid(0,0);
  c_cosmic->cd()->SetTicks(1,1);
  h_track->GetYaxis()->SetRangeUser(120,150);
  h_track->DrawCopy();
  c_cosmic->SaveAs("./figures/c_cosmic_hcalin_track.eps");

  TCanvas *c_samplefrac_all = new TCanvas("c_samplefrac_all","c_samplefrac_all",10,10,800,800);
  c_samplefrac_all->cd()->SetLeftMargin(0.15);
  c_samplefrac_all->cd()->SetBottomMargin(0.15);
  c_samplefrac_all->cd()->SetGrid(0,0);
  c_samplefrac_all->cd()->SetTicks(1,1);
  h_sf->DrawCopy();
  c_samplefrac_all->SaveAs("./figures/c_cosmic_hcalin_sf_all.eps");

  File_InPut->Close();
}

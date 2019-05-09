#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include <string>

void plotQA_cosmic_horizontal()
{
  string inputfile = "./OutPut/Proto4SampleFrac.root"; 
  TFile *File_InPut = TFile::Open(inputfile.c_str());

  TH1F *h_hcalin_tower_sim[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName = Form("h_hcalin_tower_%d_sim",i_tower);
    h_hcalin_tower_sim[i_tower] = (TH1F*)File_InPut->Get(HistName.c_str())->Clone();
  }

  TCanvas *c_hcalin = new TCanvas("c_hcalin","c_hcalin",10,10,1600,1600);
  c_hcalin->Divide(4,4,0,0);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col =0; i_col < 4; ++i_col)
    {
      int i_pad = 4*(4-(i_row+1))+i_col+1;
      int i_hist = i_row + 4*i_col;
      c_hcalin->cd(i_pad);
      h_hcalin_tower_sim[i_hist]->GetXaxis()->SetRangeUser(2,20);
      h_hcalin_tower_sim[i_hist]->Draw();
    }
  }
  c_hcalin->SaveAs("./figures/c_hcalin_horizontal.eps");

  TH1F *h_hcalout_tower_sim[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName = Form("h_hcalout_tower_%d_sim",i_tower);
    h_hcalout_tower_sim[i_tower] = (TH1F*)File_InPut->Get(HistName.c_str())->Clone();
  }

  TCanvas *c_hcalout = new TCanvas("c_hcalout","c_hcalout",10,10,1600,1600);
  c_hcalout->Divide(4,4,0,0);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col =0; i_col < 4; ++i_col)
    {
      int i_pad = 4*(4-(i_row+1))+i_col+1;
      int i_hist = i_row + 4*i_col;
      c_hcalout->cd(i_pad);
      h_hcalout_tower_sim[i_hist]->GetXaxis()->SetRangeUser(2,20);
      h_hcalout_tower_sim[i_hist]->Draw();
    }
  }
  c_hcalout->SaveAs("./figures/c_hcalout_horizontal.eps");
}

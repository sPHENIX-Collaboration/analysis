#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TLegend.h"

void plotQA_tower_distribution(const string runId = "0571")
{
  int hbdchanIHC[4][4] = {{16, 12, 8, 4},
                          {15, 11, 7, 3},
                          {14, 10, 6, 2},
                          {13,  9, 5, 1}};

  string inputfile = Form("/sphenix/user/xusun/TestBeam/ShowerCalib/Proto4ShowerInfoRAW_%s.root",runId.c_str()); 
  TFile *File_InPut = TFile::Open(inputfile.c_str());

  TH1F *h_hcalin_tower_calib[16];
  TH1F *h_hcalout_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName;

    HistName= Form("h_hcalin_lg_tower_%d_calib",i_tower);
    h_hcalin_tower_calib[i_tower] = (TH1F*)File_InPut->Get(HistName.c_str())->Clone();

    HistName= Form("h_hcalout_lg_tower_%d_calib",i_tower);
    h_hcalout_tower_calib[i_tower] = (TH1F*)File_InPut->Get(HistName.c_str())->Clone();
  }

  TCanvas *c_hcal_in = new TCanvas("c_hcal_in","c_hcal_in",10,10,1600,1600);
  c_hcal_in->Divide(4,4);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col = 0; i_col < 4; ++i_col)
    {
      int i_pad = 4*i_row+i_col+1;
      int i_tower = hbdchanIHC[i_row][i_col]-1;
      c_hcal_in->cd(i_pad);
      c_hcal_in->cd(i_pad)->SetLogy();
      h_hcalin_tower_calib[i_tower]->Draw();
    }
  }
  c_hcal_in->SaveAs("../figures/HCAL_TowerMap/c_hcal_in_0571.eps");
  // c_hcal_in->SaveAs("../figures/HCAL_TowerMap/c_hcal_in.pdf");

  TCanvas *c_hcal_out = new TCanvas("c_hcal_out","c_hcal_out",10,10,1600,1600);
  c_hcal_out->Divide(4,4);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col = 0; i_col < 4; ++i_col)
    {
      int i_pad = 4*i_row+i_col+1;
      int i_tower = hbdchanIHC[i_row][i_col]-1;
      c_hcal_out->cd(i_pad);
      c_hcal_out->cd(i_pad)->SetLogy();
      h_hcalout_tower_calib[i_tower]->Draw();
    }
  }
  c_hcal_out->SaveAs("../figures/HCAL_TowerMap/c_hcal_out_0571.eps");
  // c_hcal_out->SaveAs("../figures/HCAL_TowerMap/c_hcal_out.pdf");
}

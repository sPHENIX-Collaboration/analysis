#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include <string>

void hadd_Cosmic_SIM_HCAL(string det = "HCALIN")
{
  int hbdchanIHC[4][4] = {{4, 8, 12, 16},
                          {3, 7, 11, 15},
                          {2, 6, 10, 14},
                          {1, 5,  9, 13}};

  TH1F *h_hcal_tower_sim[16];

  TFile *File_InPut[4];
  string HistName;
  for(int i_col = 0; i_col < 4; ++i_col)
  {
    string inputfile = Form("/sphenix/user/xusun/software/data/cosmic/TowerCalibAna/Proto4TowerInfoSIM_%s_%d.root",det.c_str(),i_col); 
    File_InPut[i_col] = TFile::Open(inputfile.c_str());
    for(int i_row = 0; i_row < 4; ++i_row)
    {
      int i_tower = hbdchanIHC[i_row][i_col]-1;
      if(det == "HCALIN") HistName = Form("h_mHCALIN_SIM_twr_%d",i_tower);
      if(det == "HCALOUT") HistName = Form("h_mHCALOUT_SIM_twr_%d",i_tower);
      h_hcal_tower_sim[i_tower] = (TH1F*)File_InPut[i_col]->Get(HistName.c_str())->Clone();
    }
  }

  string outputfile = Form("/sphenix/user/xusun/software/data/cosmic/TowerCalibAna/Proto4TowerInfoSIM_%s.root",det.c_str());
  TFile *File_OutPut = new TFile(outputfile.c_str(),"RECREATE");
  File_OutPut->cd();
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    h_hcal_tower_sim[i_tower]->Write();
  }
  File_OutPut->Close();
}

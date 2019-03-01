#include <string>
#include "TString.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"

void plotQA_EnergyAsmm_production(string runID = "0422")
{
  string mom;
  if(runID == "0422") mom = "8GeV"; 
  string inputfile = Form("/sphenix/user/xusun/TestBeam/ShowerCalib/Proto4ShowerInfoRAW_%s.root",runID.c_str());
  TFile *File_InPut = TFile::Open(inputfile.c_str());
  TH2F *h_mAsymmEnergy_raw= (TH2F*)File_InPut->Get("h_tower_energy_raw");
  TH2F *h_mAsymmEnergy_calib = (TH2F*)File_InPut->Get("h_tower_energy_calib");

  TCanvas *c_EnergyAsmm = new TCanvas("c_EnergyAsmm","c_EnergyAsmm",10,10,1600,800);
  c_EnergyAsmm->Divide(2,1);
  for(int i_pad = 0; i_pad < 2; ++i_pad)
  {
    c_EnergyAsmm->cd(i_pad+1);
    c_EnergyAsmm->cd(i_pad+1)->SetLeftMargin(0.15);
    c_EnergyAsmm->cd(i_pad+1)->SetBottomMargin(0.15);
    c_EnergyAsmm->cd(i_pad+1)->SetTicks(1,1);
    c_EnergyAsmm->cd(i_pad+1)->SetGrid(0,0);
  }
  c_EnergyAsmm->cd(1);
  // h_mAsymmEnergy_raw->SetStats(0);
  h_mAsymmEnergy_raw->SetTitle("ADC vs. Asymm");

  h_mAsymmEnergy_raw->GetYaxis()->SetTitle("ADC");
  h_mAsymmEnergy_raw->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_raw->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_raw->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_raw->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_raw->Draw("colz");

  c_EnergyAsmm->cd(2);
  // h_mAsymmEnergy_calib->SetStats(0);
  h_mAsymmEnergy_calib->SetTitle("Energy vs. Asymm");

  h_mAsymmEnergy_calib->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_calib->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_calib->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_calib->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_calib->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_calib->Draw("colz");

  string fig_name = Form("../figures/HCAL_ShowerCalib/c_EnergyAsmm_nocuts_%s.eps",mom.c_str());
  c_EnergyAsmm->SaveAs(fig_name.c_str());
}

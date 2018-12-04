#include <string>
#include "TString.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"

void plotQA_E_Asym(string energy = "4GeV", string runID = "1241")
{
  string inputfile = Form("/Users/xusun/Data/sPHENIX/HCAL_BeamTest/ShowerCalibAna/%s/Proto4ShowerInfoRAW_%s.root",energy.c_str(),runID.c_str());
  TFile *File_InPut = TFile::Open(inputfile.c_str());
  TH2F *h_mAsymmEnergy_mixed_raw = (TH2F*)File_InPut->Get("h_mAsymmEnergy_mixed_raw");
  TH2F *h_mAsymmEnergy_mixed_calib_wo_cut = (TH2F*)File_InPut->Get("h_mAsymmEnergy_mixed_calib_wo_cut");

  TCanvas *c_E_Asym = new TCanvas("c_E_Asym","c_E_Asym",10,10,1800,600);
  c_E_Asym->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_E_Asym->cd(i_pad+1);
    c_E_Asym->cd(i_pad+1)->SetLeftMargin(0.15);
    c_E_Asym->cd(i_pad+1)->SetBottomMargin(0.15);
    c_E_Asym->cd(i_pad+1)->SetTicks(1,1);
    c_E_Asym->cd(i_pad+1)->SetGrid(0,0);
  }
  c_E_Asym->cd(1);
  h_mAsymmEnergy_mixed_raw->SetStats(0);
  h_mAsymmEnergy_mixed_raw->SetTitle("ADC vs. Asymm");

  h_mAsymmEnergy_mixed_raw->GetYaxis()->SetTitle("ADC");
  h_mAsymmEnergy_mixed_raw->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_mixed_raw->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_mixed_raw->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_mixed_raw->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_mixed_raw->Draw("colz");

  c_E_Asym->cd(2);
  h_mAsymmEnergy_mixed_calib_wo_cut->SetStats(0);
  h_mAsymmEnergy_mixed_calib_wo_cut->SetTitle("Energy vs. Asymm");

  h_mAsymmEnergy_mixed_calib_wo_cut->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_mixed_calib_wo_cut->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_mixed_calib_wo_cut->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_mixed_calib_wo_cut->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_mixed_calib_wo_cut->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_mixed_calib_wo_cut->Draw("colz");

  c_E_Asym->cd(3);
  h_mAsymmEnergy_mixed_calib->SetStats(0);
  // h_mAsymmEnergy_mixed_calib->SetTitle("Energy vs. Asymm & E_{in} > 0.2 GeV & E_{out} > 0.2 GeV");
  h_mAsymmEnergy_mixed_calib->SetTitle("E_{in} > 0.2 GeV & E_{out} > 0.2 GeV");

  h_mAsymmEnergy_mixed_calib->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_mixed_calib->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_mixed_calib->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_mixed_calib->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_mixed_calib->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_mixed_calib->Draw("colz");

  string fig_name = Form("./figures/runQA/c_E_Asym_%s_%s.eps",energy.c_str(),runID.c_str());
  c_E_Asym->SaveAs(fig_name.c_str());
}

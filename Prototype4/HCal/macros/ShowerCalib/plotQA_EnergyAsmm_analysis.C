#include <string>
#include "TString.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"

void plotQA_EnergyAsmm_analysis(string runID = "0571")
{
  string mom;
  if(runID == "1087") mom = "5GeV"; 
  if(runID == "0422") mom = "8GeV"; 
  if(runID == "0571") mom = "12GeV"; 
  if(runID == "0821") mom = "60GeV"; 
  if(runID == "0498") mom = "120GeV"; 
  string inputfile = Form("/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_%s.root",runID.c_str());
  TFile *File_InPut = TFile::Open(inputfile.c_str());
  TH2F *h_mAsymmAdc_mixed    = (TH2F*)File_InPut->Get("h_mAsymmAdc_mixed");
  TH2F *h_mAsymmAdc_electron = (TH2F*)File_InPut->Get("h_mAsymmAdc_electron");
  TH2F *h_mAsymmAdc_pion     = (TH2F*)File_InPut->Get("h_mAsymmAdc_pion");

  TCanvas *c_AsymmAdc = new TCanvas("c_AsymmAdc","c_AsymmAdc",10,10,1500,500);
  c_AsymmAdc->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_AsymmAdc->cd(i_pad+1);
    c_AsymmAdc->cd(i_pad+1)->SetLeftMargin(0.15);
    c_AsymmAdc->cd(i_pad+1)->SetBottomMargin(0.15);
    c_AsymmAdc->cd(i_pad+1)->SetTicks(1,1);
    c_AsymmAdc->cd(i_pad+1)->SetGrid(0,0);
  }
  c_AsymmAdc->cd(1);
  h_mAsymmAdc_mixed->SetStats(0);
  h_mAsymmAdc_mixed->SetTitle("ADC vs. Asymm");

  h_mAsymmAdc_mixed->GetYaxis()->SetTitle("ADC");
  h_mAsymmAdc_mixed->GetYaxis()->CenterTitle();
  h_mAsymmAdc_mixed->GetYaxis()->SetNdivisions(505);

  h_mAsymmAdc_mixed->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmAdc_mixed->GetXaxis()->CenterTitle();

  h_mAsymmAdc_mixed->Draw("colz");

  c_AsymmAdc->cd(2);
  h_mAsymmAdc_electron->SetStats(0);
  h_mAsymmAdc_electron->SetTitle("ADC vs. Asymm");

  h_mAsymmAdc_electron->GetYaxis()->SetTitle("ADC");
  h_mAsymmAdc_electron->GetYaxis()->CenterTitle();
  h_mAsymmAdc_electron->GetYaxis()->SetNdivisions(505);

  h_mAsymmAdc_electron->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmAdc_electron->GetXaxis()->CenterTitle();

  h_mAsymmAdc_electron->Draw("colz");

  c_AsymmAdc->cd(3);
  h_mAsymmAdc_pion->SetStats(0);
  h_mAsymmAdc_pion->SetTitle("ADC vs. Asymm");

  h_mAsymmAdc_pion->GetYaxis()->SetTitle("ADC");
  h_mAsymmAdc_pion->GetYaxis()->CenterTitle();
  h_mAsymmAdc_pion->GetYaxis()->SetNdivisions(505);

  h_mAsymmAdc_pion->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmAdc_pion->GetXaxis()->CenterTitle();

  h_mAsymmAdc_pion->Draw("colz");

  string fig_name;
  fig_name = Form("../figures/HCAL_ShowerCalib/c_AsymmAdc_%s.eps",mom.c_str());
  c_AsymmAdc->SaveAs(fig_name.c_str());

  //-----------------------------------------------------------------

  TH2F *h_mAsymmEnergy_mixed_wo_cut    = (TH2F*)File_InPut->Get("h_mAsymmEnergy_mixed_wo_cut");
  TH2F *h_mAsymmEnergy_electron_wo_cut = (TH2F*)File_InPut->Get("h_mAsymmEnergy_electron_wo_cut");
  TH2F *h_mAsymmEnergy_pion_wo_cut     = (TH2F*)File_InPut->Get("h_mAsymmEnergy_pion_wo_cut");

  TCanvas *c_AsymmEnergy_wo_cut = new TCanvas("c_AsymmEnergy_wo_cut","c_AsymmEnergy_wo_cut",30,150,1500,500);
  c_AsymmEnergy_wo_cut->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_AsymmEnergy_wo_cut->cd(i_pad+1);
    c_AsymmEnergy_wo_cut->cd(i_pad+1)->SetLeftMargin(0.15);
    c_AsymmEnergy_wo_cut->cd(i_pad+1)->SetBottomMargin(0.15);
    c_AsymmEnergy_wo_cut->cd(i_pad+1)->SetTicks(1,1);
    c_AsymmEnergy_wo_cut->cd(i_pad+1)->SetGrid(0,0);
  }
  c_AsymmEnergy_wo_cut->cd(1);
  h_mAsymmEnergy_mixed_wo_cut->SetStats(0);
  h_mAsymmEnergy_mixed_wo_cut->SetTitle("Energy vs. Asymm");

  h_mAsymmEnergy_mixed_wo_cut->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_mixed_wo_cut->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_mixed_wo_cut->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_mixed_wo_cut->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_mixed_wo_cut->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_mixed_wo_cut->Draw("colz");

  c_AsymmEnergy_wo_cut->cd(2);
  h_mAsymmEnergy_electron_wo_cut->SetStats(0);
  h_mAsymmEnergy_electron_wo_cut->SetTitle("Energy vs. Asymm");

  h_mAsymmEnergy_electron_wo_cut->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_electron_wo_cut->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_electron_wo_cut->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_electron_wo_cut->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_electron_wo_cut->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_electron_wo_cut->Draw("colz");

  c_AsymmEnergy_wo_cut->cd(3);
  h_mAsymmEnergy_pion_wo_cut->SetStats(0);
  h_mAsymmEnergy_pion_wo_cut->SetTitle("Energy vs. Asymm");

  h_mAsymmEnergy_pion_wo_cut->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_pion_wo_cut->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_pion_wo_cut->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_pion_wo_cut->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_pion_wo_cut->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_pion_wo_cut->Draw("colz");

  fig_name = Form("../figures/HCAL_ShowerCalib/c_AsymmEnergy_wo_cut_%s.eps",mom.c_str());
  c_AsymmEnergy_wo_cut->SaveAs(fig_name.c_str());

  //-----------------------------------------------------------------

  TH2F *h_mAsymmEnergy_mixed = (TH2F*)File_InPut->Get("h_mAsymmEnergy_mixed");
  TH2F *h_mAsymmEnergy_electron = (TH2F*)File_InPut->Get("h_mAsymmEnergy_electron");
  TH2F *h_mAsymmEnergy_pion = (TH2F*)File_InPut->Get("h_mAsymmEnergy_pion");

  TCanvas *c_AsymmEnergy = new TCanvas("c_AsymmEnergy","c_AsymmEnergy",60,300,1500,500);
  c_AsymmEnergy->Divide(3,1);
  for(int i_pad = 0; i_pad < 3; ++i_pad)
  {
    c_AsymmEnergy->cd(i_pad+1);
    c_AsymmEnergy->cd(i_pad+1)->SetLeftMargin(0.15);
    c_AsymmEnergy->cd(i_pad+1)->SetBottomMargin(0.15);
    c_AsymmEnergy->cd(i_pad+1)->SetTicks(1,1);
    c_AsymmEnergy->cd(i_pad+1)->SetGrid(0,0);
  }
  c_AsymmEnergy->cd(1);
  h_mAsymmEnergy_mixed->SetStats(0);
  h_mAsymmEnergy_mixed->SetTitle("Energy vs. Asymm");

  h_mAsymmEnergy_mixed->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_mixed->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_mixed->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_mixed->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_mixed->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_mixed->Draw("colz");

  c_AsymmEnergy->cd(2);
  h_mAsymmEnergy_electron->SetStats(0);
  h_mAsymmEnergy_electron->SetTitle("Energy vs. Asymm");

  h_mAsymmEnergy_electron->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_electron->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_electron->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_electron->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_electron->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_electron->Draw("colz");

  c_AsymmEnergy->cd(3);
  h_mAsymmEnergy_pion->SetStats(0);
  h_mAsymmEnergy_pion->SetTitle("Energy vs. Asymm");

  h_mAsymmEnergy_pion->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_pion->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_pion->GetYaxis()->SetNdivisions(505);

  h_mAsymmEnergy_pion->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_pion->GetXaxis()->CenterTitle();

  h_mAsymmEnergy_pion->Draw("colz");

  fig_name = Form("../figures/HCAL_ShowerCalib/c_AsymmEnergy_%s.eps",mom.c_str());
  c_AsymmEnergy->SaveAs(fig_name.c_str());
}

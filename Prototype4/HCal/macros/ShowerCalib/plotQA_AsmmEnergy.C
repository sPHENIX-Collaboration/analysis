#include <string>
#include "TString.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"

void plotQA_AsmmEnergy()
{
  string inputfile = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_0422.root";
  TFile *File_InPut = TFile::Open(inputfile.c_str());
  TH2F *h_mAsymmEnergy_mixed    = (TH2F*)File_InPut->Get("h_mAsymmEnergy_mixed");
  TH2F *h_mAsymmEnergy_electron = (TH2F*)File_InPut->Get("h_mAsymmEnergy_electron");
  TH2F *h_mAsymmEnergy_pion     = (TH2F*)File_InPut->Get("h_mAsymmEnergy_pion");

  TCanvas *c_AsymmEnergy_mixed = new TCanvas("c_AsymmEnergy_mixed","c_AsymmEnergy_mixed",30,150,800,800);
  c_AsymmEnergy_mixed->cd();
  c_AsymmEnergy_mixed->cd()->SetLeftMargin(0.15);
  c_AsymmEnergy_mixed->cd()->SetBottomMargin(0.15);
  c_AsymmEnergy_mixed->cd()->SetTicks(1,1);
  c_AsymmEnergy_mixed->cd()->SetGrid(0,0);

  h_mAsymmEnergy_mixed->SetStats(0);
  h_mAsymmEnergy_mixed->SetTitle("Energy vs. Asymm");
  h_mAsymmEnergy_mixed->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_mixed->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_mixed->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_mixed->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_mixed->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_mixed->Draw("colz");

  c_AsymmEnergy_mixed->SaveAs("../figures/HCAL_ShowerCalib/c_AsymmEnergy_mixed_8GeV.eps");
}

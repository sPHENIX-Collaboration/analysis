#include <string>
#include <TString.h>
#include <TFile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphAsymmErrors.h>
#include <TProfile.h>
#include <TLine.h>

void extractLevelingPar()
{
  string input_5GeV = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_1087.root"; // 5 GeV
  TFile *File_5GeV = TFile::Open(input_5GeV.c_str());
  TH2F *h_mAsymmEnergy_pion_5GeV = (TH2F*)File_5GeV->Get("h_mAsymmEnergy_pion_balancing");
  TProfile *p_mAsymmEnergy_pion_5GeV = h_mAsymmEnergy_pion_5GeV->ProfileX("p_mAsymmEnergy_pion_5GeV",1,-1,"i");
  p_mAsymmEnergy_pion_5GeV->SetMarkerStyle(20);
  p_mAsymmEnergy_pion_5GeV->SetMarkerColor(1);
  p_mAsymmEnergy_pion_5GeV->SetMarkerSize(1.0);

  string input_8GeV = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_0422.root"; // 8 GeV
  TFile *File_8GeV = TFile::Open(input_8GeV.c_str());
  TH2F *h_mAsymmEnergy_pion_8GeV = (TH2F*)File_8GeV->Get("h_mAsymmEnergy_pion_balancing");
  TProfile *p_mAsymmEnergy_pion_8GeV = h_mAsymmEnergy_pion_8GeV->ProfileX("p_mAsymmEnergy_pion_8GeV",1,-1,"i");
  p_mAsymmEnergy_pion_8GeV->SetMarkerStyle(20);
  p_mAsymmEnergy_pion_8GeV->SetMarkerColor(1);
  p_mAsymmEnergy_pion_8GeV->SetMarkerSize(1.0);

  string input_12GeV = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_0571.root"; // 12 GeV
  TFile *File_12GeV = TFile::Open(input_12GeV.c_str());
  TH2F *h_mAsymmEnergy_pion_12GeV = (TH2F*)File_12GeV->Get("h_mAsymmEnergy_pion_balancing");
  TProfile *p_mAsymmEnergy_pion_12GeV = h_mAsymmEnergy_pion_12GeV->ProfileX("p_mAsymmEnergy_pion_12GeV",1,-1,"i");
  p_mAsymmEnergy_pion_12GeV->SetMarkerStyle(20);
  p_mAsymmEnergy_pion_12GeV->SetMarkerColor(1);
  p_mAsymmEnergy_pion_12GeV->SetMarkerSize(1.0);

  float fit_start = -0.8;
  float fit_stop  = 0.8;
  TCanvas *c_AsymmEnergy = new TCanvas("c_AsymmEnergy","c_AsymmEnergy",1500,500);
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
  h_mAsymmEnergy_pion_5GeV->Draw("colz");
  p_mAsymmEnergy_pion_5GeV->Draw("pE same");
  TF1 *f_pol_5GeV = new TF1("f_pol_5GeV","pol1",-1.0,1.0);
  f_pol_5GeV->SetRange(fit_start,fit_stop);
  p_mAsymmEnergy_pion_5GeV->Fit(f_pol_5GeV,"NR");
  f_pol_5GeV->SetLineColor(2);
  f_pol_5GeV->SetLineWidth(4);
  f_pol_5GeV->SetLineStyle(2);
  f_pol_5GeV->Draw("l same");
  string formula_5GeV    = Form("%2.2f#timesE_{Asymm}+%2.2f",f_pol_5GeV->GetParameter(1),f_pol_5GeV->GetParameter(0));
  const float c_in_5GeV = f_pol_5GeV->Eval(0.0)/f_pol_5GeV->Eval(1.0);
  const float c_out_5GeV = f_pol_5GeV->Eval(0.0)/f_pol_5GeV->Eval(-1.0);

  TLegend *leg_5GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_5GeV->SetBorderSize(0);
  leg_5GeV->SetFillColor(10);
  leg_5GeV->AddEntry(h_mAsymmEnergy_pion_5GeV,"5 GeV","h");
  leg_5GeV->AddEntry(f_pol_5GeV,formula_5GeV.c_str(),"l");
  leg_5GeV->Draw();

  c_AsymmEnergy->cd(2);
  h_mAsymmEnergy_pion_8GeV->Draw("colz");
  p_mAsymmEnergy_pion_8GeV->Draw("pE same");
  TF1 *f_pol_8GeV = new TF1("f_pol_8GeV","pol1",-1.0,1.0);
  f_pol_8GeV->SetRange(fit_start,fit_stop);
  p_mAsymmEnergy_pion_8GeV->Fit(f_pol_8GeV,"NR");
  f_pol_8GeV->SetLineColor(2);
  f_pol_8GeV->SetLineWidth(4);
  f_pol_8GeV->SetLineStyle(2);
  f_pol_8GeV->Draw("l same");
  string formula_8GeV    = Form("%2.2f#timesE_{Asymm}+%2.2f",f_pol_8GeV->GetParameter(1),f_pol_8GeV->GetParameter(0));
  const float c_in_8GeV = f_pol_8GeV->Eval(0.0)/f_pol_8GeV->Eval(1.0);
  const float c_out_8GeV = f_pol_8GeV->Eval(0.0)/f_pol_8GeV->Eval(-1.0);

  TLegend *leg_8GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_8GeV->SetBorderSize(0);
  leg_8GeV->SetFillColor(10);
  leg_8GeV->AddEntry(h_mAsymmEnergy_pion_8GeV,"8 GeV","h");
  leg_8GeV->AddEntry(f_pol_8GeV,formula_8GeV.c_str(),"l");
  leg_8GeV->Draw();

  c_AsymmEnergy->cd(3);
  h_mAsymmEnergy_pion_12GeV->Draw("colz");
  p_mAsymmEnergy_pion_12GeV->Draw("pE same");
  TF1 *f_pol_12GeV = new TF1("f_pol_12GeV","pol1",-1.0,1.0);
  f_pol_12GeV->SetRange(fit_start,fit_stop);
  p_mAsymmEnergy_pion_12GeV->Fit(f_pol_12GeV,"NR");
  f_pol_12GeV->SetLineColor(2);
  f_pol_12GeV->SetLineWidth(4);
  f_pol_12GeV->SetLineStyle(2);
  f_pol_12GeV->Draw("l same");
  string formula_12GeV    = Form("%2.2f#timesE_{Asymm}+%2.2f",f_pol_12GeV->GetParameter(1),f_pol_12GeV->GetParameter(0));
  const float c_in_12GeV = f_pol_12GeV->Eval(0.0)/f_pol_12GeV->Eval(1.0);
  const float c_out_12GeV = f_pol_12GeV->Eval(0.0)/f_pol_12GeV->Eval(-1.0);

  TLegend *leg_12GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_12GeV->SetBorderSize(0);
  leg_12GeV->SetFillColor(10);
  leg_12GeV->AddEntry(h_mAsymmEnergy_pion_12GeV,"12 GeV","h");
  leg_12GeV->AddEntry(f_pol_12GeV,formula_12GeV.c_str(),"l");
  leg_12GeV->Draw();

  c_AsymmEnergy->SaveAs("../figures/HCAL_ShowerCalib/c_AsymmEnergy_LevelingPar.eps");

  ofstream File_OutPut("leveling_corr.txt");
  File_OutPut << "5  GeV: c_in = " << c_in_5GeV << ", c_out = " << c_out_5GeV << endl;
  File_OutPut << "8  GeV: c_in = " << c_in_8GeV << ", c_out = " << c_out_8GeV << endl;
  File_OutPut << "12 GeV: c_in = " << c_in_12GeV << ", c_out = " << c_out_12GeV << endl;
  File_OutPut.close();
}

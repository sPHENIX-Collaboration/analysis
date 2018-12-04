#include <string>
#include "TString.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include <iostream>
#include <fstream>

void calLevelingCorr()
{
  string input_4GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/4GeV/Proto4ShowerInfoRAW_1241.root";
  TFile *File_InPut_4GeV = TFile::Open(input_4GeV.c_str());
  // TH2F *h_mAsymmEnergy_4GeV = (TH2F*)File_InPut_4GeV->Get("h_mAsymmEnergy_mixed_calib_wo_cut");
  TH2F *h_mAsymmEnergy_4GeV = (TH2F*)File_InPut_4GeV->Get("h_mAsymmEnergy_mixed_calib");

  string input_8GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/8GeV/Proto4ShowerInfoRAW_0422.root";
  TFile *File_InPut_8GeV = TFile::Open(input_8GeV.c_str());
  // TH2F *h_mAsymmEnergy_8GeV = (TH2F*)File_InPut_8GeV->Get("h_mAsymmEnergy_mixed_calib_wo_cut");
  TH2F *h_mAsymmEnergy_8GeV = (TH2F*)File_InPut_8GeV->Get("h_mAsymmEnergy_mixed_calib");

  string input_12GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/12GeV/Proto4ShowerInfoRAW_0571.root";
  TFile *File_InPut_12GeV = TFile::Open(input_12GeV.c_str());
  // TH2F *h_mAsymmEnergy_12GeV = (TH2F*)File_InPut_12GeV->Get("h_mAsymmEnergy_mixed_calib_wo_cut");
  TH2F *h_mAsymmEnergy_12GeV = (TH2F*)File_InPut_12GeV->Get("h_mAsymmEnergy_mixed_calib");

  string input_16GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/16GeV/Proto4ShowerInfoRAW_1683.root";
  TFile *File_InPut_16GeV = TFile::Open(input_16GeV.c_str());
  // TH2F *h_mAsymmEnergy_16GeV = (TH2F*)File_InPut_16GeV->Get("h_mAsymmEnergy_mixed_calib_wo_cut");
  TH2F *h_mAsymmEnergy_16GeV = (TH2F*)File_InPut_16GeV->Get("h_mAsymmEnergy_mixed_calib");

  string input_24GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/24GeV/Proto4ShowerInfoRAW_1666.root";
  TFile *File_InPut_24GeV = TFile::Open(input_24GeV.c_str());
  // TH2F *h_mAsymmEnergy_24GeV = (TH2F*)File_InPut_24GeV->Get("h_mAsymmEnergy_mixed_calib_wo_cut");
  TH2F *h_mAsymmEnergy_24GeV = (TH2F*)File_InPut_24GeV->Get("h_mAsymmEnergy_mixed_calib");

  string input_30GeV = "/sphenix/user/xusun/software/data/beam/ShowerCalibAna/30GeV/Proto4ShowerInfoRAW_1684.root";
  TFile *File_InPut_30GeV = TFile::Open(input_30GeV.c_str());
  // TH2F *h_mAsymmEnergy_30GeV = (TH2F*)File_InPut_30GeV->Get("h_mAsymmEnergy_mixed_calib_wo_cut");
  TH2F *h_mAsymmEnergy_30GeV = (TH2F*)File_InPut_30GeV->Get("h_mAsymmEnergy_mixed_calib");

  TCanvas *c_play = new TCanvas("c_play","c_play",800,1200);
  c_play->Divide(2,3);
  for(int i_pad = 0; i_pad < 6; ++i_pad)
  {
    c_play->cd(i_pad+1);
    c_play->cd(i_pad+1)->SetLeftMargin(0.15);
    c_play->cd(i_pad+1)->SetBottomMargin(0.15);
    c_play->cd(i_pad+1)->SetTicks(1,1);
    c_play->cd(i_pad+1)->SetGrid(0,0);
  }
  float fit_start = -0.8;
  float fit_stop  =  0.2;
  string energy_cut = "E_{in} > 0.2 GeV & E_{out} > 0.2 GeV";

  c_play->cd(1);
  h_mAsymmEnergy_4GeV->SetStats(0);
  h_mAsymmEnergy_4GeV->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_4GeV->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_4GeV->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_4GeV->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_4GeV->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_4GeV->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_4GeV->Draw("colz");
  TF1 *f_pol_4GeV = new TF1("f_pol_4GeV","pol1",fit_start,fit_stop);
  h_mAsymmEnergy_4GeV->Fit(f_pol_4GeV,"NR");

  f_pol_4GeV->SetLineColor(1);
  f_pol_4GeV->SetLineWidth(4);
  f_pol_4GeV->SetLineStyle(2);
  f_pol_4GeV->Draw("l same");

  string formula_4GeV = Form("%2.2f#timesE_{Asymm}+%2.2f",f_pol_4GeV->GetParameter(1),f_pol_4GeV->GetParameter(0));

  const float c_in_4GeV = f_pol_4GeV->Eval(0.0)/f_pol_4GeV->Eval(1.0);
  const float c_out_4GeV = f_pol_4GeV->Eval(0.0)/f_pol_4GeV->Eval(-1.0);

  TLegend *leg_4GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_4GeV->SetBorderSize(0);
  leg_4GeV->SetFillColor(10);
  leg_4GeV->AddEntry(h_mAsymmEnergy_4GeV,"4 GeV","h");
  leg_4GeV->AddEntry(f_pol_4GeV,formula_4GeV.c_str(),"l");
  leg_4GeV->Draw();

  c_play->cd(2);
  h_mAsymmEnergy_8GeV->SetStats(0);
  h_mAsymmEnergy_8GeV->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_8GeV->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_8GeV->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_8GeV->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_8GeV->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_8GeV->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_8GeV->Draw("colz");
  TF1 *f_pol_8GeV = new TF1("f_pol_8GeV","pol1",fit_start,fit_stop);
  h_mAsymmEnergy_8GeV->Fit(f_pol_8GeV,"NR");
  f_pol_8GeV->SetLineColor(1);
  f_pol_8GeV->SetLineWidth(4);
  f_pol_8GeV->SetLineStyle(2);
  f_pol_8GeV->Draw("l same");

  string formula_8GeV = Form("%2.2f#timesE_{Asymm}+%2.2f",f_pol_8GeV->GetParameter(1),f_pol_8GeV->GetParameter(0));

  const float c_in_8GeV = f_pol_8GeV->Eval(0.0)/f_pol_8GeV->Eval(1.0);
  const float c_out_8GeV = f_pol_8GeV->Eval(0.0)/f_pol_8GeV->Eval(-1.0);

  TLegend *leg_8GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_8GeV->SetBorderSize(0);
  leg_8GeV->SetFillColor(10);
  leg_8GeV->AddEntry(h_mAsymmEnergy_8GeV,"8 GeV","h");
  leg_8GeV->AddEntry(f_pol_8GeV,formula_8GeV.c_str(),"l");
  leg_8GeV->Draw();

  c_play->cd(3);
  h_mAsymmEnergy_12GeV->SetStats(0);
  h_mAsymmEnergy_12GeV->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_12GeV->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_12GeV->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_12GeV->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_12GeV->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_12GeV->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_12GeV->Draw("colz");
  TF1 *f_pol_12GeV = new TF1("f_pol_12GeV","pol1",fit_start,fit_stop);
  h_mAsymmEnergy_12GeV->Fit(f_pol_12GeV,"NR");
  f_pol_12GeV->SetLineColor(1);
  f_pol_12GeV->SetLineWidth(4);
  f_pol_12GeV->SetLineStyle(2);
  f_pol_12GeV->Draw("l same");

  string formula_12GeV = Form("%2.2f#timesE_{Asymm}+%2.2f",f_pol_12GeV->GetParameter(1),f_pol_12GeV->GetParameter(0));

  const float c_in_12GeV = f_pol_12GeV->Eval(0.0)/f_pol_12GeV->Eval(1.0);
  const float c_out_12GeV = f_pol_12GeV->Eval(0.0)/f_pol_12GeV->Eval(-1.0);

  TLegend *leg_12GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_12GeV->SetBorderSize(0);
  leg_12GeV->SetFillColor(10);
  leg_12GeV->AddEntry(h_mAsymmEnergy_12GeV,"12 GeV","h");
  leg_12GeV->AddEntry(f_pol_12GeV,formula_12GeV.c_str(),"l");
  leg_12GeV->Draw();

  c_play->cd(4);
  h_mAsymmEnergy_16GeV->SetStats(0);
  h_mAsymmEnergy_16GeV->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_16GeV->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_16GeV->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_16GeV->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_16GeV->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_16GeV->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_16GeV->Draw("colz");
  TF1 *f_pol_16GeV = new TF1("f_pol_16GeV","pol1",fit_start,fit_stop);
  h_mAsymmEnergy_16GeV->Fit(f_pol_16GeV,"NR");
  f_pol_16GeV->SetLineColor(1);
  f_pol_16GeV->SetLineWidth(4);
  f_pol_16GeV->SetLineStyle(2);
  f_pol_16GeV->Draw("l same");

  string formula_16GeV = Form("%2.2f#timesE_{Asymm}+%2.2f",f_pol_16GeV->GetParameter(1),f_pol_16GeV->GetParameter(0));

  const float c_in_16GeV = f_pol_16GeV->Eval(0.0)/f_pol_16GeV->Eval(1.0);
  const float c_out_16GeV = f_pol_16GeV->Eval(0.0)/f_pol_16GeV->Eval(-1.0);

  TLegend *leg_16GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_16GeV->SetBorderSize(0);
  leg_16GeV->SetFillColor(10);
  leg_16GeV->AddEntry(h_mAsymmEnergy_16GeV,"16 GeV","h");
  leg_16GeV->AddEntry(f_pol_16GeV,formula_16GeV.c_str(),"l");
  leg_16GeV->Draw();

  c_play->cd(5);
  h_mAsymmEnergy_24GeV->SetStats(0);
  h_mAsymmEnergy_24GeV->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_24GeV->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_24GeV->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_24GeV->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_24GeV->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_24GeV->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_24GeV->Draw("colz");
  TF1 *f_pol_24GeV = new TF1("f_pol_24GeV","pol1",fit_start,fit_stop);
  h_mAsymmEnergy_24GeV->Fit(f_pol_24GeV,"NR");
  f_pol_24GeV->SetLineColor(1);
  f_pol_24GeV->SetLineWidth(4);
  f_pol_24GeV->SetLineStyle(2);
  f_pol_24GeV->Draw("l same");

  string formula_24GeV = Form("%2.2f#timesE_{Asymm}+%2.2f",f_pol_24GeV->GetParameter(1),f_pol_24GeV->GetParameter(0));

  const float c_in_24GeV = f_pol_24GeV->Eval(0.0)/f_pol_24GeV->Eval(1.0);
  const float c_out_24GeV = f_pol_24GeV->Eval(0.0)/f_pol_24GeV->Eval(-1.0);

  TLegend *leg_24GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_24GeV->SetBorderSize(0);
  leg_24GeV->SetFillColor(10);
  leg_24GeV->AddEntry(h_mAsymmEnergy_24GeV,"24 GeV","h");
  leg_24GeV->AddEntry(f_pol_24GeV,formula_24GeV.c_str(),"l");
  leg_24GeV->Draw();

  c_play->cd(6);
  h_mAsymmEnergy_30GeV->SetStats(0);
  h_mAsymmEnergy_30GeV->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_30GeV->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_30GeV->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_30GeV->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_30GeV->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_30GeV->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_30GeV->Draw("colz");
  TF1 *f_pol_30GeV = new TF1("f_pol_30GeV","pol1",fit_start,fit_stop);
  h_mAsymmEnergy_30GeV->Fit(f_pol_30GeV,"NR");
  f_pol_30GeV->SetLineColor(1);
  f_pol_30GeV->SetLineWidth(4);
  f_pol_30GeV->SetLineStyle(2);
  f_pol_30GeV->Draw("l same");
  string formula_30GeV = Form("%2.2f#timesE_{Asymm}+%2.2f",f_pol_30GeV->GetParameter(1),f_pol_30GeV->GetParameter(0));

  const float c_in_30GeV = f_pol_30GeV->Eval(0.0)/f_pol_30GeV->Eval(1.0);
  const float c_out_30GeV = f_pol_30GeV->Eval(0.0)/f_pol_30GeV->Eval(-1.0);

  TLegend *leg_30GeV = new TLegend(0.35,0.7,0.75,0.85);
  leg_30GeV->SetBorderSize(0);
  leg_30GeV->SetFillColor(10);
  leg_30GeV->AddEntry(h_mAsymmEnergy_30GeV,"30 GeV","h");
  leg_30GeV->AddEntry(f_pol_30GeV,formula_30GeV.c_str(),"l");
  leg_30GeV->Draw();

  string fig_Name = "./figures/sPHENIX_CollMeeting/c_mAsymmEnergy_mixed.eps";
  c_play->SaveAs(fig_Name.c_str());

  ofstream File_OutPut("leveling_corr.txt");
  File_OutPut << "4 GeV:  c_in = " << c_in_4GeV << ", c_out = " << c_out_4GeV << endl;
  File_OutPut << "8 GeV:  c_in = " << c_in_8GeV << ", c_out = " << c_out_8GeV << endl;
  File_OutPut << "12 GeV: c_in = " << c_in_12GeV << ", c_out = " << c_out_12GeV << endl;
  File_OutPut << "16 GeV: c_in = " << c_in_16GeV << ", c_out = " << c_out_16GeV << endl;
  File_OutPut << "24 GeV: c_in = " << c_in_24GeV << ", c_out = " << c_out_24GeV << endl;
  File_OutPut << "30 GeV: c_in = " << c_in_30GeV << ", c_out = " << c_out_30GeV << endl;
  File_OutPut.close();
}

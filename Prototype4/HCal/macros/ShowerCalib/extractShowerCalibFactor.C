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

void extractShowerCalibFactor() // use 8 GeV & run 422 for shower calbrtion
{
  string inputfile = "/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_0422.root";
  TFile *File_InPut = TFile::Open(inputfile.c_str());
  TH2F *h_mAsymmEnergy_pion_levl = (TH2F*)File_InPut->Get("h_mAsymmEnergy_pion_leveling");
  TH1F *h_mEnergy_pion_levl = (TH1F*)h_mAsymmEnergy_pion_levl->ProjectionY()->Clone("h_mEnergy_pion_levl");

  TCanvas *c_shower = new TCanvas("c_shower","c_shower",1600,800);
  c_shower->Divide(2,1);
  for(int i_pad = 0; i_pad < 2; ++i_pad)
  {
    c_shower->cd(i_pad+1);
    c_shower->cd(i_pad+1)->SetLeftMargin(0.15);
    c_shower->cd(i_pad+1)->SetBottomMargin(0.15);
    c_shower->cd(i_pad+1)->SetTicks(1,1);
    c_shower->cd(i_pad+1)->SetGrid(0,0);
  }

  string energy_cut = "E_{in} > 1 MeV & E_{out} > 1 MeV && E > MIP+3#sigma";

  c_shower->cd(1);
  h_mAsymmEnergy_pion_levl->SetStats(0);
  h_mAsymmEnergy_pion_levl->SetTitle(energy_cut.c_str());
  h_mAsymmEnergy_pion_levl->GetYaxis()->SetTitle("Energy (GeV)");
  h_mAsymmEnergy_pion_levl->GetYaxis()->CenterTitle();
  h_mAsymmEnergy_pion_levl->GetYaxis()->SetNdivisions(505);
  h_mAsymmEnergy_pion_levl->GetXaxis()->SetTitle("E_{Asymm}");
  h_mAsymmEnergy_pion_levl->GetXaxis()->CenterTitle();
  h_mAsymmEnergy_pion_levl->Draw("colz");

  TLegend *leg = new TLegend(0.35,0.7,0.75,0.85);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->AddEntry(h_mAsymmEnergy_pion_levl,"8 GeV","h");
  leg->Draw();

  c_shower->cd(2);
  h_mEnergy_pion_levl->SetStats(0);
  h_mEnergy_pion_levl->SetTitle("8 GeV & #pi^{-}");
  h_mEnergy_pion_levl->GetXaxis()->SetTitle("Energy (GeV)");
  h_mEnergy_pion_levl->GetXaxis()->CenterTitle();
  h_mEnergy_pion_levl->GetXaxis()->SetNdivisions(505);
  h_mEnergy_pion_levl->Draw("hE");

  TF1 *f_gaus = new TF1("f_gaus","gaus",0,100);
  f_gaus->SetParameter(0,1.0);
  f_gaus->SetParameter(1,h_mEnergy_pion_levl->GetMean());
  f_gaus->SetParameter(2,1.0);
  f_gaus->SetRange(1.0,7.0);
  h_mEnergy_pion_levl->Fit(f_gaus,"NR");

  float norm = f_gaus->GetParameter(0);
  float mean = f_gaus->GetParameter(1); // extract calibrated energy
  float sigma = f_gaus->GetParameter(2); // extract calibrated energy
  f_gaus->SetParameter(0,norm);
  f_gaus->SetParameter(1,mean);
  f_gaus->SetParameter(2,sigma);
  f_gaus->SetRange(mean-3.0*sigma,mean+3.0*sigma);
  // f_gaus->SetRange(1.5,7.0);
  h_mEnergy_pion_levl->Fit(f_gaus,"MNR");
  float energy_twrcalib = f_gaus->GetParameter(1);

  f_gaus->SetLineColor(2);
  f_gaus->SetLineStyle(2);
  f_gaus->SetLineWidth(2);
  f_gaus->Draw("l same");

  const float showercalib = 8.0/energy_twrcalib;
  cout << "showercalib = " << showercalib << endl;

  ofstream File_OutPut("showercalib.txt");
  File_OutPut << "8 GeV: showercalib = " << showercalib << endl;
  File_OutPut.close();
}

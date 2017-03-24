// $Id: $                                                                                             

/*!
 * \file ExampleAnalysisModulePlot.C
 * \brief // this macro is part of analysis tutorial: https://wiki.bnl.gov/sPHENIX/index.php/2017_calorimeter_beam_test/Tutorial#Level_2:_formal_analysis_via_analysis_module
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <cassert>
using namespace std;

void
ExampleAnalysisModulePlot(

)
{
  // assuming you compiled the analysis module defined in ../ExampleAnalysisModule/
  gSystem->Load("libProto3_ExampleAnalysisModule.so");

  // assuming you already run Fun4All_TestBeam_ExampleAnalysisModule.C
  TFile * file = TFile::Open("ExampleAnalysis.root");

  // Now you have a TTree called T accessible in your command line. Nevertheless, let's get it to use programtically.
  assert(file);
  TTree* T = (TTree *) file->GetObjectChecked("T", "TTree");
  assert(T);

  // internal plots in analysis module checking events and cuts
  TH1F* hNormalization = (TH1F *) file->GetObjectChecked("hNormalization",
      "TH1F");
  assert(hNormalization);

  // internal plots in analysis module checking cherenkov signals
  TH2F* hCheck_Cherenkov = (TH2F *) file->GetObjectChecked("hCheck_Cherenkov",
      "TH2F");
  assert(hCheck_Cherenkov);

  // Now make the plots
  TText * t;
  TCanvas *c1 = new TCanvas("ExampleAnalysisModulePlot",
      "ExampleAnalysisModulePlot", 1200, 1000);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  c1->Update();
  p = (TPad *) c1->cd(idx++);
  p->SetLogy();

  hNormalization->Draw();

  c1->Update();
  p = (TPad *) c1->cd(idx++);
  p->SetLogz();

  hCheck_Cherenkov->Draw("colz");

  c1->Update();
  p = (TPad *) c1->cd(idx++);
  p->SetLogy();

  T->Draw("clus_5x5_CEMC.sum_E>>hEnergy_Sum_CEMC(400,0,20)", "info.good_anti_e",
      "");
  T->Draw("clus_5x5_CEMC.sum_E>>hEnergy_Sum_CEMC_C(400,0,20)", "info.good_e",
      "same");
  hEnergy_Sum_CEMC->SetTitle(
      "EMCal 5x5 cluster, anti-e cut (blue) and electron cut (green);EMCal 5x5 cluster Energy (GeV)");
  hEnergy_Sum_CEMC_C->SetLineColor(kGreen + 2);
  hEnergy_Sum_CEMC_C->SetFillColor(kGreen);

  c1->Update();
  p = (TPad *) c1->cd(idx++);
  p->SetLogy();

  T->Draw(
      "info.sum_E_CEMC + info.sum_E_HCAL_IN + info.sum_E_HCAL_OUT>>hEnergy_Sum(400,0,20)",
      "info.good_anti_e", "");
  hEnergy_Sum->SetTitle(
      "All three calorimeter energy with anti-e cut (hadron + muon);Energy Sum (GeV)");

}


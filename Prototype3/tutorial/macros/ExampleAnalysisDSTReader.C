// $Id: $                                                                                             

/*!
 * \file ExampleAnalysisDSTReader.C
 * \brief example plotting macro using DSTReader TTrees from the production output
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
ExampleAnalysisDSTReader(

)
{
  // here, load the lib for prototype3 so we can use get_*() functions in T->Draw functions.
  gSystem->Load("libPrototype3.so");

  // Let's take a look at run 3533, which is -8 GeV/c secondary beam centered on EMCal tower 45.
  // More runs are produced in data production: https://wiki.bnl.gov/sPHENIX/index.php/2017_calorimeter_beam_test/Data_Production_and_Analysis#Production_Information
  // This tutorial focuses on the *_DSTReader.root, which is interctive TTree file containing all tower and channels.
  TFile * file =
      TFile::Open(
          "/gpfs/mnt/gpfs02/sphenix/data/data01/t1044-2016a/production.2017/Production_0203_init/beam_00003533-0000_DSTReader.root");

  // Now you have a TTree called T accessible in your command line. Nevertheless, let's get it to use programtically.
  assert(file);
  TTree* T = (TTree *) file->GetObjectChecked("T", "TTree");
  assert(T);

  // To make life easier, TTree allow us to define Alias of formulas, which can be used as if TTree varialbles.

  // Cherenkov C2 energy sum
  T->SetAlias("C2_Sum_e",
      "TOWER_CALIB_C2[2].energy + TOWER_CALIB_C2[3].energy");

  // Average column and rows on EMCal
  T->SetAlias("Average_column",
      "Sum$(TOWER_CALIB_CEMC.get_column() * TOWER_CALIB_CEMC.get_energy())/Sum$(TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Average_row",
      "Sum$(TOWER_CALIB_CEMC.get_row() * TOWER_CALIB_CEMC.get_energy())/Sum$(TOWER_CALIB_CEMC.get_energy())");

  // Average hodoscope and whether there is a valid hit
  T->SetAlias("Average_HODO_VERTICAL",
      "Sum$(TOWER_CALIB_HODO_VERTICAL.towerid * (abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) * abs(TOWER_CALIB_HODO_VERTICAL.energy))/Sum$((abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) * abs(TOWER_CALIB_HODO_VERTICAL.energy))");
  T->SetAlias("Valid_HODO_VERTICAL",
      "Sum$(abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) > 0");

  T->SetAlias("Average_HODO_HORIZONTAL",
      "Sum$(TOWER_CALIB_HODO_HORIZONTAL.towerid * (abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))/Sum$((abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))");
  T->SetAlias("Valid_HODO_HORIZONTAL",
      "Sum$(abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) > 0");

  T->SetAlias("Hodoscope_h3_v2",
      "abs(Average_HODO_HORIZONTAL-3)<.5 && abs(Average_HODO_VERTICAL-2)<.5");

  // Nothing in trigger veto counter
  T->SetAlias("No_Triger_VETO",
      "Sum$(abs(TOWER_RAW_TRIGGER_VETO.energy)>15)==0");

  // EMCal, HCal_in and HCal_out energy sums
  T->SetAlias("Energy_Sum_CEMC", "1*Sum$(TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Energy_Sum_HCALIN",
      "1*Sum$(TOWER_CALIB_LG_HCALIN.get_energy())");
  T->SetAlias("Energy_Sum_HCALOUT",
      "1*Sum$(TOWER_CALIB_LG_HCALOUT.get_energy())");

  // Good event
  T->SetAlias("Valid_Hodoscope_Clean_VETO_Counter",
      "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO");
  T->SetAlias("CherenkovCut", "C2_Sum_e>200");

  // Now make the plots
  TText * t;
  TCanvas *c1 = new TCanvas("ExampleAnalysisDSTReader",
      "ExampleAnalysisDSTReader", 1200, 1000);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  c1->Update();
  p = (TPad *) c1->cd(idx++);
  p->SetLogy();

  T->Draw("C2_Sum_e>>hC2_Sum_e(1000,0,2000)",
      "Valid_Hodoscope_Clean_VETO_Counter", "");
  T->Draw("C2_Sum_e>>hC2_Sum_e_c(1000,0,2000)",
      "Valid_Hodoscope_Clean_VETO_Counter && CherenkovCut", "same");

  hC2_Sum_e->SetTitle(
      "Cherenkov C2 Sum, inclusive (blue) and with C2 cut (green);Cherenkov C2 Sum (ADC)");
//  hC2_Sum_e_c->SetLineColor(kGreen + 2);
  hC2_Sum_e_c->SetFillColor(kGreen);

  c1->Update();
  p = (TPad *) c1->cd(idx++);
  p->SetLogy();

  T->Draw("Energy_Sum_CEMC>>hEnergy_Sum_CEMC(400,0,20)",
      "Valid_Hodoscope_Clean_VETO_Counter", "");
  T->Draw("Energy_Sum_CEMC>>hEnergy_Sum_CEMC_C(400,0,20)",
      "Valid_Hodoscope_Clean_VETO_Counter && CherenkovCut", "same");
  hEnergy_Sum_CEMC->SetTitle(
      "EMCal Sum, inclusive (black) and with C2 cut (green);EMCal Calibrated Energy Sum (GeV)");
//  hEnergy_Sum_CEMC_C->SetLineColor(kGreen + 2);
  hEnergy_Sum_CEMC_C->SetFillColor(kGreen);

  c1->Update();
  p = (TPad *) c1->cd(idx++);
  p->SetLogy();

  T->Draw(
      "Energy_Sum_CEMC + Energy_Sum_HCALIN + Energy_Sum_HCALOUT>>hEnergy_Sum(400,0,20)",
      "Valid_Hodoscope_Clean_VETO_Counter", "");
  hEnergy_Sum->SetTitle(
      "All three calorimeter energy Sum;Energy Sum (GeV)");

  c1->Update();
  p = (TPad *) c1->cd(idx++);
  p->SetLogy();

  TH1 * hEnergy_Sum_CEMC_v2h3 = new TH1F("hEnergy_Sum_CEMC_v2h3",
      ";EMCal Calibrated Energy Sum (GeV);Count / bin", 100, 0, 20);

  T->Draw("Energy_Sum_CEMC>>hEnergy_Sum_CEMC_v2h3",
      "Valid_Hodoscope_Clean_VETO_Counter && CherenkovCut && Hodoscope_h3_v2",
      "");

  TF1 * fgaus_g = new TF1("fgaus_LG_g", "gaus",
      hEnergy_Sum_CEMC_v2h3->GetMean() - 2 * hEnergy_Sum_CEMC_v2h3->GetRMS(),
      hEnergy_Sum_CEMC_v2h3->GetMean() + 2 * hEnergy_Sum_CEMC_v2h3->GetRMS());
  fgaus_g->SetParameters(1,
      hEnergy_Sum_CEMC_v2h3->GetMean() - 2 * hEnergy_Sum_CEMC_v2h3->GetRMS(),
      hEnergy_Sum_CEMC_v2h3->GetMean() + 2 * hEnergy_Sum_CEMC_v2h3->GetRMS());
  hEnergy_Sum_CEMC_v2h3->Fit(fgaus_g, "MR0N");

  TF1 * fgaus = new TF1("fgaus_LG", "gaus",
      fgaus_g->GetParameter(1) - 3 * fgaus_g->GetParameter(2),
      fgaus_g->GetParameter(1) + 3 * fgaus_g->GetParameter(2));
  fgaus->SetParameters(fgaus_g->GetParameter(0), fgaus_g->GetParameter(1),
      fgaus_g->GetParameter(2));
  hEnergy_Sum_CEMC_v2h3->Fit(fgaus, "MR");

  hEnergy_Sum_CEMC_v2h3->SetTitle(
      Form("EMCal energy (1x1 hodoscope H=3 V=2), #DeltaE/<E> = %.1f%%",
          100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

}


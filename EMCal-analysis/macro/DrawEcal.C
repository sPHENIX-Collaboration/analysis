#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

void
DrawEcal(void)
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");

//  DrawSF();

//  DrawCluster_SingleE();
//  DrawCluster_AnaP() ;
//  DrawCluster_Linearality();
//  DrawCluster_Res();
//  DrawCluster_Res_2Fit();
//  DrawTowerSum_SingleE();
//  DrawTowerSum_AnaP()->Draw("ap*");
//  DrawCluster_Res_2Fit_Electron_NoSVX();
  //  DrawTowerSum_Res_2Fit_1DSpacalNoSVX();
//  DrawTowerSum_Res_2Fit_1DSpacalNoSVX_Eta3();
  //  DrawCluster_Res_2Fit_2DSpacalNoSVX();
//    DrawCluster_Res_2Fit_1DSpacalNoSVX_Eta3();

//  DrawTower_EMCDistribution(
//      "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal2d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
//      "2D-proj. SPACAL in HIJING Au+Au 0-10% C");
  //  DrawTower_EMCDistribution(
  //      "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal1d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
  //      "1D-proj. SPACAL in HIJING Au+Au 0-10% C");
//    DrawTower_EMCDistribution5x5(
//        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal1d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
//        "1D-proj. SPACAL in HIJING Au+Au 0-10% C");

//    DrawEnergyDensity(
//        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal2d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
//        "Scintilator Energy Density with 2D-proj. SPACAL in HIJING Au+Au 0-10% C");
  //    DrawEnergyDensity(
  //        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal1d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
  //        "Scintilator Energy Density with 1D-proj. SPACAL in HIJING Au+Au 0-10% C");
//      DrawEnergyDensityXY(
//          "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal2d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
//          "Scintilator Energy Density with 2D-proj. SPACAL in HIJING Au+Au 0-10% C");
//        DrawEnergyDensityXY(
//            "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal1d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
//            "Scintilator Energy Density with 1D-proj. SPACAL in HIJING Au+Au 0-10% C");


//  DrawTower_EMCTrigEff();
//  DrawTower_EMCTrigEff_SlideingWindow2();

}

void
DrawEnergyDensity(
    const TString infile =
      "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal2d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
    const TString title = "HIJING Au+Au 0-10% C + Geant4")
{
  TH1 * EMCalAna_h_CEMC_RZ = DrawTower_Load(infile, "EMCalAna_h_CEMC_RZ");
  TH1 * EMCalAna_h_HCALIN_RZ = DrawTower_Load(infile, "EMCalAna_h_HCALIN_RZ");
  TH1 * EMCalAna_h_HCALOUT_RZ = DrawTower_Load(infile, "EMCalAna_h_HCALOUT_RZ");

  EMCalAna_h_CEMC_RZ->Add(EMCalAna_h_HCALIN_RZ);
  EMCalAna_h_CEMC_RZ->Add(EMCalAna_h_HCALOUT_RZ);

  for (int r = 1; r <= EMCalAna_h_CEMC_RZ->GetNbinsY(); r++)
    {
      const double radius = EMCalAna_h_CEMC_RZ->GetYaxis()->GetBinCenter(r);
      const double circ = 2 * TMath::Pi() * radius;
      for (int z = 1; z <= EMCalAna_h_CEMC_RZ->GetNbinsX(); z++)
        {

          EMCalAna_h_CEMC_RZ->SetBinContent(z, r,
              EMCalAna_h_CEMC_RZ->GetBinContent(z, r) / circ);
        }
    }

  TCanvas *c1 = new TCanvas("DrawEnergyDensity", "DrawEnergyDensity", 1800,
      900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
//  p->SetGridx(0);
//  p->SetGridy(0);

  EMCalAna_h_CEMC_RZ->SetTitle(";Z (cm);Radius (cm)");
  EMCalAna_h_CEMC_RZ->Draw("colz");

  t = new TText(.5, .95, (title));
  t->SetNDC();
  t->SetTextAlign(22);
  t->Draw();

  SaveCanvas(c1, infile + TString("_DrawEcal_") + TString(c1->GetName()),
      kTRUE);
}

void
DrawEnergyDensityXY(
    const TString infile =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal2d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
    const TString title = "HIJING Au+Au 0-10% C + Geant4")
{
  TH1 * EMCalAna_h_CEMC_RZ = DrawTower_Load(infile, "EMCalAna_h_CEMC_XY");
  TH1 * EMCalAna_h_HCALIN_RZ = DrawTower_Load(infile, "EMCalAna_h_HCALIN_XY");
  TH1 * EMCalAna_h_HCALOUT_RZ = DrawTower_Load(infile, "EMCalAna_h_HCALOUT_XY");

  EMCalAna_h_CEMC_RZ->Add(EMCalAna_h_HCALIN_RZ);
  EMCalAna_h_CEMC_RZ->Add(EMCalAna_h_HCALOUT_RZ);

//  for (int r = 1; r <= EMCalAna_h_CEMC_RZ->GetNbinsY(); r++)
//    {
//      const double radius = EMCalAna_h_CEMC_RZ->GetYaxis()->GetBinCenter(r);
//      const double circ = 2 * TMath::Pi() * radius;
//      for (int z = 1; z <= EMCalAna_h_CEMC_RZ->GetNbinsX(); z++)
//        {
//
//          EMCalAna_h_CEMC_RZ->SetBinContent(z, r,
//              EMCalAna_h_CEMC_RZ->GetBinContent(z, r) / circ);
//        }
//    }

  TCanvas *c1 = new TCanvas("DrawEnergyDensityXY", "DrawEnergyDensityXY", 900,
      900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
//  p->SetGridx(0);
//  p->SetGridy(0);

  EMCalAna_h_CEMC_RZ->SetTitle(";X (cm);Y (cm)");
  EMCalAna_h_CEMC_RZ->Draw("colz");

  TText *
  t = new TText(.5, .95, (title));
  t->SetNDC();
  t->SetTextSize(0.03);
  t->SetTextAlign(22);
  t->Draw();

  SaveCanvas(c1, infile + TString("_DrawEcal_") + TString(c1->GetName()),
      kTRUE);
}

void
DrawTower_EMCTrigEff(
    const TString infile =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/")
{
  TH1F * rej_EMCalAna_h_CEMC_TOWER_4x4_max = DrawTower_Load(
      infile
          + "/pythia8/spacal2d/G4Hits_sPHENIX_pythia8-ALL.root_EMCalAna.root",
      "EMCalAna_h_CEMC_TOWER_4x4_max");
//  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
//      DrawTower_Load(
//          infile
//              + "/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0ALL_4GeVALL_EMCalAna.root",
//          "EMCalAna_h_CEMC_TOWER_4x4_max");
//  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
//      DrawTower_Load(
//          infile
//              + "emcstudies/nosvtx/spacal2d/fieldmap/ALLe-ALL_4GeVALLEMCalAna.root",
//          "EMCalAna_h_CEMC_TOWER_4x4_max");
  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
      DrawTower_Load(
          infile
              + "../test_production/Upsilon/spacal2d/fieldon/SimALL_PythiaUpsilon.root_EMCalAna.root",
          "EMCalAna_h_CEMC_TOWER_4x4_max");

  TH1F * rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC = DrawTower_Load(
      infile
          + "/pythia8/spacal2d/G4Hits_sPHENIX_pythia8-ALL.root_EMCalAna.root",
      "EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC");
//  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
//      DrawTower_Load(
//          infile
//              + "/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0ALL_4GeVALL_EMCalAna.root",
//          "EMCalAna_h_CEMC_TOWER_4x4_max");
//  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
//      DrawTower_Load(
//          infile
//              + "emcstudies/nosvtx/spacal2d/fieldmap/ALLe-ALL_4GeVALLEMCalAna.root",
//          "EMCalAna_h_CEMC_TOWER_4x4_max");
  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC =
      DrawTower_Load(
          infile
              + "../test_production/Upsilon/spacal2d/fieldon/SimALL_PythiaUpsilon.root_EMCalAna.root",
          "EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC");

  TGraphErrors* ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max = Distribution2Efficiency(
      rej_EMCalAna_h_CEMC_TOWER_4x4_max);
  TGraphErrors* ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max = Distribution2Efficiency(
      sig_EMCalAna_h_CEMC_TOWER_4x4_max);

  TGraphErrors* ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC = Distribution2Efficiency(
      rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC);
  TGraphErrors* ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC = Distribution2Efficiency(
      sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC);

  TCanvas *c1 = new TCanvas("DrawTower_EMCTrigEff", "DrawTower_EMCTrigEff",
      1800, 900);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  rej_EMCalAna_h_CEMC_TOWER_4x4_max->Draw();
  rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Draw("same");
  rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetLineColor(kRed);
  rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetMarkerColor(kRed);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  sig_EMCalAna_h_CEMC_TOWER_4x4_max->Draw();
  sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Draw("same");
  sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetLineColor(kRed);
  sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetMarkerColor(kRed);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("AP*");
  ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Draw("P");
  ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetLineColor(kRed);
  ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetMarkerColor(kRed);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("AP*");
  ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Draw("P");
  ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetLineColor(kRed);
  ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetMarkerColor(kRed);

  SaveCanvas(c1, infile + TString("_DrawEcal_") + TString(c1->GetName()),
      kTRUE);

  TGraphErrors* reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max =
      (TGraphErrors*) ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Clone(
          "reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max");

  TGraphErrors* reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC =
      (TGraphErrors*) ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Clone(
          "reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC");

//  const double * eff = reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetY();
//  const double reg = eff[reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetN()-1];
//  for (int i = 0; i < reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetN(); ++i)
//    {
//
//      eff[i] /=reg;
//
//      cout << i << ": regularted to "
//          << eff[i] << " by "
//          <<reg << endl;
//    }

  const double y_min = 1;
  const double y_max = 1e5;
  const double y_min2 = .8;
  const double y_max2 = 1;

  TGraphErrors* plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max =
      (TGraphErrors*) reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Clone(
          "plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max");
  TGraphErrors* plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max =
      (TGraphErrors*) ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->Clone(
          "plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max");


  TGraphErrors* plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC =
      (TGraphErrors*) reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Clone(
          "plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC");
  TGraphErrors* plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC =
      (TGraphErrors*) ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Clone(
          "plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC");

  for (int i = 0; i < plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->GetN(); ++i)
    {
      (plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->GetY())[i] =1./(plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->GetY())[i];
    }

  for (int i = 0; i < plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetN(); ++i)
    {
      (plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetY())[i] = exp(

          ((plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetY())[i] - y_min2)/(y_max2 - y_min2)*(log(y_max) - log(y_min)) + log(y_min)

      );
    }


  for (int i = 0; i < plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->GetN(); ++i)
    {
      (plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->GetY())[i] =1./(plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->GetY())[i];
    }

  for (int i = 0; i < plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->GetN(); ++i)
    {
      (plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->GetY())[i] = exp(

          ((plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->GetY())[i] - y_min2)/(y_max2 - y_min2)*(log(y_max) - log(y_min)) + log(y_min)

      );
    }


  TCanvas *c1 = new TCanvas("DrawTower_EMCTrigEff_Compile",
      "DrawTower_EMCTrigEff_Compile", 1800, 900);
//  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);
  p->SetTicky(0);

  p->DrawFrame(0, y_min, 5.5, y_max,
      ";EMCal trigger #Sigma_{4x4}[ E_{Tower} ] requirement (GeV);Rejection factor for MB events");

  TGaxis * a = new TGaxis(5.5,y_min,5.5, y_max,y_min2,y_max2,510,"+L");
  a->SetTitle("Trigger efficiency for inclusive Upsilon (1S)");
  a->SetLabelColor(kRed);
  a->SetTitleColor(kRed);
  a->SetLineColor(kRed);
  a->SetTextFont(42);
  a->Draw();

  TLine * l = new TLine(4.3,y_min,4.3,y_max);
  l->SetLineColor(kGray);
  l->SetLineWidth(5);
  l->Draw();

  TLatex * t = new TLatex(0, y_max*1.1,"PYTHIA8 p+p #sqrt{s} = 200 GeV + Geant4 + Trigger Emulator");
  t->Draw();

  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->SetFillColor(kRed);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineColor(kRed);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineWidth(3);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineStyle(kDashed);
//  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("3");
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("lx");

  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->SetFillColor(kBlack);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineColor(kBlack);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineWidth(3);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineStyle(kDashed);
//  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("3");
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("lx");


  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetFillColor(kRed);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetLineColor(kRed);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetLineWidth(3);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Draw("3");
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Draw("lx");

  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetFillColor(kBlack);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetLineColor(kBlack);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->SetLineWidth(3);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Draw("3");
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC->Draw("lx");

  TLegend * legd = new TLegend(0.13,0.5,0.5,0.9);

  TLegendEntry   * le =
  legd->AddEntry(plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC, "Rej. for MB (8-bit truncated)", "lx");
  le->SetTextFont(42);
  TLegendEntry   * le =
  legd->AddEntry(plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC, "Eff. for #Upsilon (8-bit truncated)", "lx");
  le->SetTextFont(42);
  le->SetTextColor(kRed);
  TLegendEntry   * le =
  legd->AddEntry(plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max, "Rej. for MB (full bit ADC)", "lx");
  le->SetTextFont(42);
  TLegendEntry   * le =
  legd->AddEntry(plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max, "Eff. for #Upsilon (full bit ADC)", "lx");
  le->SetTextFont(42);
  le->SetTextColor(kRed);

  legd->Draw();

  SaveCanvas(c1, infile + TString("_DrawEcal_") + TString(c1->GetName()),
      kTRUE);
}


void
DrawTower_EMCTrigEff_SlideingWindow2(
    const TString infile =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/")
{
  TH1F * rej_EMCalAna_h_CEMC_TOWER_4x4_max = DrawTower_Load(
      infile
          + "/pythia8/spacal2d/G4Hits_sPHENIX_pythia8-ALL.root_EMCalAna.root",
      "EMCalAna_h_CEMC_TOWER_4x4_max");
//  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
//      DrawTower_Load(
//          infile
//              + "/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0ALL_4GeVALL_EMCalAna.root",
//          "EMCalAna_h_CEMC_TOWER_4x4_max");
//  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
//      DrawTower_Load(
//          infile
//              + "emcstudies/nosvtx/spacal2d/fieldmap/ALLe-ALL_4GeVALLEMCalAna.root",
//          "EMCalAna_h_CEMC_TOWER_4x4_max");
  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
      DrawTower_Load(
          infile
              + "../test_production/Upsilon/spacal2d/fieldon/SimALL_PythiaUpsilon.root_EMCalAna.root",
          "EMCalAna_h_CEMC_TOWER_4x4_max");

  TH1F * rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC = DrawTower_Load(
      infile
          + "/pythia8/spacal2d/G4Hits_sPHENIX_pythia8-ALL.root_EMCalAna.root",
      "EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC");
//  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
//      DrawTower_Load(
//          infile
//              + "/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0ALL_4GeVALL_EMCalAna.root",
//          "EMCalAna_h_CEMC_TOWER_4x4_max");
//  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_max =
//      DrawTower_Load(
//          infile
//              + "emcstudies/nosvtx/spacal2d/fieldmap/ALLe-ALL_4GeVALLEMCalAna.root",
//          "EMCalAna_h_CEMC_TOWER_4x4_max");
  TH1F * sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC =
      DrawTower_Load(
          infile
              + "../test_production/Upsilon/spacal2d/fieldon/SimALL_PythiaUpsilon.root_EMCalAna.root",
          "EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC");

  TGraphErrors* ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max = Distribution2Efficiency(
      rej_EMCalAna_h_CEMC_TOWER_4x4_max);
  TGraphErrors* ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max = Distribution2Efficiency(
      sig_EMCalAna_h_CEMC_TOWER_4x4_max);

  TGraphErrors* ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC = Distribution2Efficiency(
      rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC);
  TGraphErrors* ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC = Distribution2Efficiency(
      sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC);

  TCanvas *c1 = new TCanvas("DrawTower_EMCTrigEff_SlideingWindow2", "DrawTower_EMCTrigEff_SlideingWindow2",
      1800, 900);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  rej_EMCalAna_h_CEMC_TOWER_4x4_max->Draw();
  rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Draw("same");
  rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetLineColor(kRed);
  rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetMarkerColor(kRed);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  sig_EMCalAna_h_CEMC_TOWER_4x4_max->Draw();
  sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Draw("same");
  sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetLineColor(kRed);
  sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetMarkerColor(kRed);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("AP*");
  ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Draw("P");
  ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetLineColor(kRed);
  ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetMarkerColor(kRed);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("AP*");
  ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Draw("P");
  ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetLineColor(kRed);
  ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetMarkerColor(kRed);

  SaveCanvas(c1, infile + TString("_DrawEcal_") + TString(c1->GetName()),
      kTRUE);

  TGraphErrors* reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max =
      (TGraphErrors*) ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Clone(
          "reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max");

  TGraphErrors* reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC =
      (TGraphErrors*) ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Clone(
          "reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC");

//  const double * eff = reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetY();
//  const double reg = eff[reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetN()-1];
//  for (int i = 0; i < reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetN(); ++i)
//    {
//
//      eff[i] /=reg;
//
//      cout << i << ": regularted to "
//          << eff[i] << " by "
//          <<reg << endl;
//    }

  const double y_min = 1;
  const double y_max = 1e5;
  const double y_min2 = .8;
  const double y_max2 = 1;

  TGraphErrors* plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max =
      (TGraphErrors*) reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Clone(
          "plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max");
  TGraphErrors* plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max =
      (TGraphErrors*) ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->Clone(
          "plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max");


  TGraphErrors* plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC =
      (TGraphErrors*) reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Clone(
          "plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC");
  TGraphErrors* plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC =
      (TGraphErrors*) ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Clone(
          "plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC");

  for (int i = 0; i < plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->GetN(); ++i)
    {
      (plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->GetY())[i] =1./(plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->GetY())[i];
    }

  for (int i = 0; i < plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetN(); ++i)
    {
      (plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetY())[i] = exp(

          ((plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->GetY())[i] - y_min2)/(y_max2 - y_min2)*(log(y_max) - log(y_min)) + log(y_min)

      );
    }


  for (int i = 0; i < plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->GetN(); ++i)
    {
      (plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->GetY())[i] =1./(plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->GetY())[i];
    }

  for (int i = 0; i < plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->GetN(); ++i)
    {
      (plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->GetY())[i] = exp(

          ((plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->GetY())[i] - y_min2)/(y_max2 - y_min2)*(log(y_max) - log(y_min)) + log(y_min)

      );
    }


  TCanvas *c1 = new TCanvas("DrawTower_EMCTrigEff_SlideingWindow2_Compile",
      "DrawTower_EMCTrigEff_SlideingWindow2_Compile", 1800, 900);
//  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);
  p->SetTicky(0);

  p->DrawFrame(0, y_min, 5.5, y_max,
      ";EMCal trigger #Sigma_{4x4}[ E_{Tower} ] requirement (GeV);Rejection factor for MB events");

  TGaxis * a = new TGaxis(5.5,y_min,5.5, y_max,y_min2,y_max2,510,"+L");
  a->SetTitle("Trigger efficiency for inclusive Upsilon (1S)");
  a->SetLabelColor(kRed);
  a->SetTitleColor(kRed);
  a->SetLineColor(kRed);
  a->SetTextFont(42);
  a->Draw();

  TLine * l = new TLine(4.3,y_min,4.3,y_max);
  l->SetLineColor(kGray);
  l->SetLineWidth(5);
  l->Draw();

  TLatex * t = new TLatex(0, y_max*1.1,"PYTHIA8 p+p #sqrt{s} = 200 GeV + Geant4 + Trigger Emulator");
  t->Draw();

  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->SetFillColor(kRed);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineColor(kRed);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineWidth(3);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineStyle(kDashed);
//  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("3");
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("lx");

  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->SetFillColor(kBlack);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineColor(kBlack);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineWidth(3);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->SetLineStyle(kDashed);
//  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("3");
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max->Draw("lx");


  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetFillColor(kRed);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetLineColor(kRed);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetLineWidth(3);
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Draw("3");
  plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Draw("lx");

  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetFillColor(kBlack);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetLineColor(kBlack);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->SetLineWidth(3);
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Draw("3");
  plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC->Draw("lx");

  TLegend * legd = new TLegend(0.13,0.5,0.6,0.9);

  TLegendEntry   * le =
  legd->AddEntry(plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC, "Rej. for MB (8-bit truncated 2x2 Sum)", "lx");
  le->SetTextFont(42);
  TLegendEntry   * le =
  legd->AddEntry(plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC, "Eff. for #Upsilon (8-bit truncated 2x2 Sum)", "lx");
  le->SetTextFont(42);
  le->SetTextColor(kRed);
  TLegendEntry   * le =
  legd->AddEntry(plot_ge_rej_EMCalAna_h_CEMC_TOWER_4x4_max, "Rej. for MB (full ADC info)", "lx");
  le->SetTextFont(42);
  TLegendEntry   * le =
  legd->AddEntry(plot_reg_ge_sig_EMCalAna_h_CEMC_TOWER_4x4_max, "Eff. for #Upsilon (full ADC info)", "lx");
  le->SetTextFont(42);
  le->SetTextColor(kRed);

  legd->Draw();

  SaveCanvas(c1, infile + TString("_DrawEcal_") + TString(c1->GetName()),
      kTRUE);
}



TGraphErrors *
Distribution2Efficiency(TH1F * hCEMC3_Max)
{
  double threshold[10000] =
    { 0 };
  double eff[10000] =
    { 0 };
  double eff_err[10000] =
    { 0 };

  assert(hCEMC3_Max->GetNbinsX()<10000);

  const double n = hCEMC3_Max->GetSum();
  double pass = 0;
  int cnt = 0;
  for (int i = hCEMC3_Max->GetNbinsX(); i >= 1; i--)
    {
      pass += hCEMC3_Max->GetBinContent(i);

      const double pp = pass / n;
//      const double z = 1.96;
      const double z = 1.;

      const double A = z * sqrt(1. / n * pp * (1 - pp) + z * z / 4 / n / n);
      const double B = 1 / (1 + z * z / n);

      threshold[cnt] = hCEMC3_Max->GetBinCenter(i);
      eff[cnt] = (pp + z * z / 2 / n) * B;
      eff_err[cnt] = A * B;

      cout << threshold[cnt] << ": " << "CL " << eff[cnt] << "+/-"
          << eff_err[cnt] << endl;
      cnt++;
    }
  TGraphErrors * ge = new TGraphErrors(cnt, threshold, eff, NULL, eff_err);

  return ge;
}

void
DrawTower_EMCDistribution(
    const TString infile =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal2d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
    const TString title = "HIJING Au+Au 0-10% C + Geant4 + Digi.")
{
  TH1F * hCEMC1 = DrawTower_Load(infile, "EMCalAna_h_CEMC_TOWER_1x1");
  TH1F * hCEMC3 = DrawTower_Load(infile, "EMCalAna_h_CEMC_TOWER_3x3");

  hCEMC1->Scale(1. / hCEMC1->GetSum());
  const double mean1 = hCEMC1->GetMean();
  hCEMC3->Scale(1. / hCEMC3->GetSum());
  const double mean3 = hCEMC3->GetMean();

  TCanvas *c1 = new TCanvas("DrawTower_EMCDistribution",
      "DrawTower_EMCDistribution", 1800, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  hCEMC1->GetXaxis()->SetRangeUser(0, .6);
  hCEMC1->SetTitle(";EMCal Single Tower Energy [GeV];A.U.");
  hCEMC1->SetLineColor(kBlue + 2);
  hCEMC1->SetLineWidth(3);
  hCEMC1->Draw();

  TText * t;
  t = new TText(.6, .8, Form("Tower Mean = %.0f MeV", mean1 * 1000));
  t->SetNDC();
  t->SetTextAlign(22);
  t->Draw();
  t = new TText(.5, .95, (title));
  t->SetNDC();
  t->SetTextAlign(22);
  t->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  hCEMC3->GetXaxis()->SetRangeUser(0, 2);
  hCEMC3->SetTitle(";EMCal 3x3 Tower Energy Sum [GeV];A.U.");
  hCEMC3->SetLineColor(kRed + 2);
  hCEMC3->SetLineWidth(3);
  hCEMC3->Draw();

  t = new TText(.6, .8, Form("3x3 Tower Mean = %.0f MeV", mean3 * 1000));
  t->SetNDC();
  t->SetTextAlign(22);
  t->Draw();

  SaveCanvas(c1, infile + TString("_DrawEcal_") + TString(c1->GetName()),
      kTRUE);
}

void
DrawTower_EMCDistribution5x5(
    const TString infile =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal2d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
    const TString title = "HIJING Au+Au 0-10% C + Geant4 + Digi.")
{
  TH1F * hCEMC1 = DrawTower_Load(infile, "EMCalAna_h_CEMC_TOWER_1x1");
  TH1F * hCEMC3 = DrawTower_Load(infile, "EMCalAna_h_CEMC_TOWER_5x5");

  hCEMC1->Scale(1. / hCEMC1->GetSum());
  const double mean1 = hCEMC1->GetMean();
  hCEMC3->Scale(1. / hCEMC3->GetSum());
  const double mean3 = hCEMC3->GetMean();

  TCanvas *c1 = new TCanvas("DrawTower_EMCDistribution5x5",
      "DrawTower_EMCDistribution5x5", 1800, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  hCEMC1->GetXaxis()->SetRangeUser(0, .6);
  hCEMC1->SetTitle(";EMCal Single Tower Energy [GeV];A.U.");
  hCEMC1->SetLineColor(kBlue + 2);
  hCEMC1->SetLineWidth(3);
  hCEMC1->Draw();

  TText * t;
  t = new TText(.6, .8, Form("Tower Mean = %.0f MeV", mean1 * 1000));
  t->SetNDC();
  t->SetTextAlign(22);
  t->Draw();
  t = new TText(.5, .95, (title));
  t->SetNDC();
  t->SetTextAlign(22);
  t->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  hCEMC3->GetXaxis()->SetRangeUser(0, 2);
  hCEMC3->SetTitle(";EMCal 5x5 Tower Energy Sum [GeV];A.U.");
  hCEMC3->SetLineColor(kRed + 2);
  hCEMC3->SetLineWidth(3);
  hCEMC3->Draw();

  t = new TText(.6, .8, Form("5x5 Tower Mean = %.0f MeV", mean3 * 1000));
  t->SetNDC();
  t->SetTextAlign(22);
  t->Draw();

  SaveCanvas(c1, infile + TString("_DrawEcal_") + TString(c1->GetName()),
      kTRUE);
}

TH1 *
DrawTower_Load(
    const TString infile =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/sHijing/spacal2d/G4Hits_sPHENIX_sHijing-0-4.4fm_ALL.root_EMCalAna.root",
    const TString hist_name = "EMCalAna_h_CEMC_TOWER_1x1")
{
  TString chian_str = infile;
  chian_str.ReplaceAll("ALL", "*");

  TChain * t = new TChain("T_Index");
  const int n = t->Add(chian_str);

  cout << "Loaded " << n << " root files with " << chian_str << endl;
  assert(n>0);

  TH1 * h_sum = NULL;

  TObjArray *fileElements = t->GetListOfFiles();
  TIter next(fileElements);
  TChainElement *chEl = 0;
  while ((chEl = (TChainElement*) next()))
    {

      cout << "DrawTower_Load - processing " << chEl->GetTitle() << endl;
      TFile * f = new TFile(chEl->GetTitle());
      assert(f);

      TH1 * h = (TH1 *) f->GetObjectChecked(hist_name, "TH1");
      assert(h);

      if (!h_sum)
        {
          gDirectory->cd("/");
          h_sum = (TH1 *) h->Clone();
          h_sum->ResetBit(kMustCleanup);
          assert(h_sum);
        }
      else
        {
          h_sum->Add(h);
          delete f;
        }
    }

  return h_sum;
}

void
DrawCluster_Linearality(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

//  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/old/G4Hits_sPHENIX", "gamma_eta0", true);
//  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/old/G4Hits_sPHENIX", "gamma_eta0.90", true);
//
//  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
//      base + "/spacal1d/zerofield/old/G4Hits_sPHENIX", "gamma_eta0", true);
//  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
//      base + "/spacal1d/zerofield/old/G4Hits_sPHENIX", "gamma_eta0.90", true);

//  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", true);
//  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", true);
//
//  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0", true);
//  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", true);

  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", true);
  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", true);

  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0", true);
  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", true);

  TCanvas *c1 = new TCanvas("DrawCluster_Linearality",
      "DrawCluster_Linearality", 1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 55, 55,
      ";Incoming Energy (GeV);Reconstructed cluster energy, E_{reco} (GeV)");
  TLine * l = new TLine(0, 0, 55, 55);
  l->SetLineColor(kGray);
  l->Draw();

  TLegend * lg = new TLegend(1, 42, 44, 53, NULL, "br");
  TLegend * lg2 = new TLegend(12, 40 - 40, 43 + 12, 51 - 40, NULL, "br");

  p = (TPad *) c1->cd(idx);
  c1->Update();

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "pol2", 0.5, 60);
  gamma_eta0->Fit(f_calo, "RM0");

  double * Ys = gamma_eta0->GetY();
  for (int i = 0; i < gamma_eta0->GetN(); i++)
    {
      Ys[i] /= f_calo->GetParameter(1);
    }
  gamma_eta0->Fit(f_calo, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);

  gamma_eta0->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "2D-proj., #eta = 0.0-0.1, E_{reco} ~ %.2f E + %.1e E^{2} ",
      f_calo->GetParameter(1), f_calo->GetParameter(2));
  lg->AddEntry(gamma_eta0, ltitle.Data(), "pl");

  TF1 * f_calo = new TF1("f_calo_gamma_eta9", "pol2", 0.5, 60);
  gamma_eta9->Fit(f_calo, "RM0");
  double * Ys = gamma_eta9->GetY();
  for (int i = 0; i < gamma_eta9->GetN(); i++)
    {
      Ys[i] /= f_calo->GetParameter(1);
    }
  gamma_eta9->Fit(f_calo, "RM0");

  gamma_eta9->SetLineColor(kRed + 3);
  gamma_eta9->SetMarkerColor(kRed + 3);
  gamma_eta9->SetLineWidth(2);
  gamma_eta9->SetMarkerStyle(kFullCircle);
  gamma_eta9->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 3);
  f_calo->SetLineWidth(2);

  gamma_eta9->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "2D-proj., #eta = 0.9-1.0, E_{reco} ~ %.2f E + %.1e E^{2} ",
      f_calo->GetParameter(1), f_calo->GetParameter(2));
  lg->AddEntry(gamma_eta9, ltitle.Data(), "pl");
  cout << "Title = " << ltitle << endl;
//
//
//
  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "pol2", 0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");
  double * Ys = gamma_eta0_1d->GetY();
  for (int i = 0; i < gamma_eta0_1d->GetN(); i++)
    {
      Ys[i] /= f_calo->GetParameter(1);
    }
  gamma_eta0_1d->Fit(f_calo, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);

  gamma_eta0_1d->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "1D-proj., #eta = 0.0-0.1, E_{reco} ~ %.2f E + %.1e E^{2} ",
      f_calo->GetParameter(1), f_calo->GetParameter(2));
  lg2->AddEntry(gamma_eta0_1d, ltitle.Data(), "pl");
//
//
//
  TF1 * f_calo = new TF1("f_calo_gamma_eta9_1d", "pol2", 0.5, 60);
  gamma_eta9_1d->Fit(f_calo, "RM0");
  double * Ys = gamma_eta9_1d->GetY();
  for (int i = 0; i < gamma_eta9_1d->GetN(); i++)
    {
      Ys[i] /= f_calo->GetParameter(1);
    }
  gamma_eta9_1d->Fit(f_calo, "RM0");

  gamma_eta9_1d->SetLineColor(kBlue + 3);
  gamma_eta9_1d->SetMarkerColor(kBlue + 3);
  gamma_eta9_1d->SetLineWidth(2);
  gamma_eta9_1d->SetMarkerStyle(kOpenCircle);
  gamma_eta9_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 3);
  f_calo->SetLineWidth(2);

  gamma_eta9_1d->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "1D-proj., #eta = 0.9-1.0, E_{reco} ~ %.2f E + %.1e E^{2} ",
      f_calo->GetParameter(1), f_calo->GetParameter(2));
  lg2->AddEntry(gamma_eta9_1d, ltitle.Data(), "pl");
  cout << "Title = " << ltitle << endl;

  lg->Draw();
  lg2->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

void
DrawCluster_Res_2Fit(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

//  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);
//
//  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0.90",
//      false);

  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);

  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);

  TCanvas *c1 = new TCanvas("DrawCluster_Res_2Fit", "DrawCluster_Res_2Fit",
      1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 35, 20e-2,
      ";Incoming Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2, 9.6e-2, 17, 19.6e-2, NULL, "br");
  TLegend * lg2 = new TLegend(16, 9e-2, 33, 19e-2, NULL, "br");

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta0->Fit(f_calo, "RM0");
  gamma_eta0->Fit(f_calo_l, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0->Draw("p");

  lg->AddEntry(gamma_eta0,
      Form("2D-proj., #eta = 0.0-0.1", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta9", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta9", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta9->Fit(f_calo, "RM0");
  gamma_eta9->Fit(f_calo_l, "RM0");

  gamma_eta9->SetLineColor(kRed + 3);
  gamma_eta9->SetMarkerColor(kRed + 3);
  gamma_eta9->SetLineWidth(2);
  gamma_eta9->SetMarkerStyle(kFullCircle);
  gamma_eta9->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9->Draw("p");

  TString ltitle = Form("2D-proj., #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");
  gamma_eta0_1d->Fit(f_calo_l, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0_1d->Draw("p");

  TString ltitle = Form("1D-proj., #eta = 0.0-0.1", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta9_1d->Fit(f_calo, "RM0");
  gamma_eta9_1d->Fit(f_calo_l, "RM0");

  gamma_eta9_1d->SetLineColor(kBlue + 3);
  gamma_eta9_1d->SetMarkerColor(kBlue + 3);
  gamma_eta9_1d->SetLineWidth(2);
  gamma_eta9_1d->SetMarkerStyle(kOpenCircle);
  gamma_eta9_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9_1d->Draw("p");

  TString ltitle = Form("1D-proj., #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  lg->Draw();
  lg2->Draw();

  TLatex * t = new TLatex(0, 0.205,"Single photon in full sPHENIX detector");
  t->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}


void
DrawCluster_Res_2Fit_Electron_NoSVX(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
      base + "/nosvtx/spacal2d/fieldmap/G4Hits_sPHENIX", "e-_eta0", false);
  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
      base + "/nosvtx/spacal2d/fieldmap/G4Hits_sPHENIX", "e-_eta0.90", false);
//
  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "e-_eta0", false);
  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "e-_eta0.90", false);


  TCanvas *c1 = new TCanvas("DrawCluster_Res_2Fit_Electron_NoSVX", "DrawCluster_Res_2Fit_Electron_NoSVX",
      1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 35, 20e-2,
      ";Incoming Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2, 9.6e-2, 17, 19.6e-2, NULL, "br");
  TLegend * lg2 = new TLegend(16, 9e-2, 33, 19e-2, NULL, "br");

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta0->Fit(f_calo, "RM0");
  gamma_eta0->Fit(f_calo_l, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0->Draw("p");

  lg->AddEntry(gamma_eta0,
      Form("2D-proj., #eta = 0.0-0.1", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta9", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta9", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta9->Fit(f_calo, "RM0");
  gamma_eta9->Fit(f_calo_l, "RM0");

  gamma_eta9->SetLineColor(kRed + 3);
  gamma_eta9->SetMarkerColor(kRed + 3);
  gamma_eta9->SetLineWidth(2);
  gamma_eta9->SetMarkerStyle(kFullCircle);
  gamma_eta9->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9->Draw("p");

  TString ltitle = Form("2D-proj., #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");
  gamma_eta0_1d->Fit(f_calo_l, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0_1d->Draw("p");

  TString ltitle = Form("1D-proj., #eta = 0.0-0.1", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta9_1d->Fit(f_calo, "RM0");
  gamma_eta9_1d->Fit(f_calo_l, "RM0");

  gamma_eta9_1d->SetLineColor(kBlue + 3);
  gamma_eta9_1d->SetMarkerColor(kBlue + 3);
  gamma_eta9_1d->SetLineWidth(2);
  gamma_eta9_1d->SetMarkerStyle(kOpenCircle);
  gamma_eta9_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9_1d->Draw("p");

  TString ltitle = Form("1D-proj., #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  lg->Draw();
  lg2->Draw();


  TLatex * t = new TLatex(0, 0.205,"Single electron in sPHENIX calorimetry (no tracker)");
  t->Draw();


  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}


void
DrawCluster_Res_2Fit_1DSpacalNoSVX(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

//  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);
//
//  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0.90",
//      false);

//  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);
//
//  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
//      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
//      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);

  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "e-_eta0", false);
  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "e-_eta0.90", false);

  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "gamma_eta0", false);
  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "gamma_eta0.90",
      false);
  TCanvas *c1 = new TCanvas("DrawCluster_Res_2Fit_1DSpacalNoSVX",
      "DrawCluster_Res_2Fit_1DSpacalNoSVX", 1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 35, 20e-2,
      ";Incoming Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2, 9.6e-2, 17, 19.6e-2, NULL, "br");
  TLegend * lg2 = new TLegend(16, 9e-2, 33, 19e-2, NULL, "br");

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta0->Fit(f_calo, "RM0");
  gamma_eta0->Fit(f_calo_l, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0->Draw("p");

  lg->AddEntry(gamma_eta0,
      Form("Electrons, #eta = 0.0-0.1", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta9", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta9", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta9->Fit(f_calo, "RM0");
  gamma_eta9->Fit(f_calo_l, "RM0");

  gamma_eta9->SetLineColor(kRed + 3);
  gamma_eta9->SetMarkerColor(kRed + 3);
  gamma_eta9->SetLineWidth(2);
  gamma_eta9->SetMarkerStyle(kFullCircle);
  gamma_eta9->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9->Draw("p");

  TString ltitle = Form("Electrons, #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");
  gamma_eta0_1d->Fit(f_calo_l, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0_1d->Draw("p");

  TString ltitle = Form("Photons, #eta = 0.0-0.1", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta9_1d->Fit(f_calo, "RM0");
  gamma_eta9_1d->Fit(f_calo_l, "RM0");

  gamma_eta9_1d->SetLineColor(kBlue + 3);
  gamma_eta9_1d->SetMarkerColor(kBlue + 3);
  gamma_eta9_1d->SetLineWidth(2);
  gamma_eta9_1d->SetMarkerStyle(kOpenCircle);
  gamma_eta9_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9_1d->Draw("p");

  TString ltitle = Form("Photons, #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  lg->Draw();
  lg2->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

void
DrawCluster_Res_2Fit_1DSpacalNoSVX_Eta3(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

//  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);
//
//  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0.90",
//      false);

//  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);
//
//  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
//      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
//      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);

  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "e-_eta0.30", false);

  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "gamma_eta0.30",
      false);
  TCanvas *c1 = new TCanvas("DrawCluster_Res_2Fit_1DSpacalNoSVX_Eta3",
      "DrawCluster_Res_2Fit_1DSpacalNoSVX_Eta3", 1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 35, 20e-2,
      ";Incoming Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2, 10e-2, 17, 20e-2, NULL, "br");
  TLegend * lg2 = new TLegend(16, 9e-2, 33, 19e-2, NULL, "br");

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta0->Fit(f_calo, "RM0");
  gamma_eta0->Fit(f_calo_l, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0->Draw("p");

  lg->AddEntry(gamma_eta0,
      Form("Electrons, #eta = 0.3-0.4", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");
  gamma_eta0_1d->Fit(f_calo_l, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0_1d->Draw("p");

  TString ltitle = Form("Photons, #eta = 0.3-0.4", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  lg->Draw();
  lg2->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

void
DrawCluster_Res_2Fit_2DSpacalNoSVX(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

//  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);
//
//  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
//      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0.90",
//      false);

//  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
//      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);
//
//  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
//      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
//  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
//      base + "/spacal1d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);

  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
      base + "/nosvtx/spacal2d/fieldmap/G4Hits_sPHENIX", "e-_eta0", false);
  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
      base + "/nosvtx/spacal2d/fieldmap/G4Hits_sPHENIX", "e-_eta0.90", false);

  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
      base + "/nosvtx/spacal2d/fieldmap/G4Hits_sPHENIX", "gamma_eta0", false);
  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
      base + "/nosvtx/spacal2d/fieldmap/G4Hits_sPHENIX", "gamma_eta0.90",
      false);
  TCanvas *c1 = new TCanvas("DrawCluster_Res_2Fit_2DSpacalNoSVX",
      "DrawCluster_Res_2Fit_2DSpacalNoSVX", 1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 35, 20e-2,
      ";Incoming Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2, 9.6e-2, 17, 19.6e-2, NULL, "br");
  TLegend * lg2 = new TLegend(16, 9e-2, 33, 19e-2, NULL, "br");

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta0->Fit(f_calo, "RM0");
  gamma_eta0->Fit(f_calo_l, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0->Draw("p");

  lg->AddEntry(gamma_eta0,
      Form("Electrons, #eta = 0.0-0.1", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta9", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta9", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta9->Fit(f_calo, "RM0");
  gamma_eta9->Fit(f_calo_l, "RM0");

  gamma_eta9->SetLineColor(kRed + 3);
  gamma_eta9->SetMarkerColor(kRed + 3);
  gamma_eta9->SetLineWidth(2);
  gamma_eta9->SetMarkerStyle(kFullCircle);
  gamma_eta9->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9->Draw("p");

  TString ltitle = Form("Electrons, #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");
  gamma_eta0_1d->Fit(f_calo_l, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0_1d->Draw("p");

  TString ltitle = Form("Photons, #eta = 0.0-0.1", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta9_1d->Fit(f_calo, "RM0");
  gamma_eta9_1d->Fit(f_calo_l, "RM0");

  gamma_eta9_1d->SetLineColor(kBlue + 3);
  gamma_eta9_1d->SetMarkerColor(kBlue + 3);
  gamma_eta9_1d->SetLineWidth(2);
  gamma_eta9_1d->SetMarkerStyle(kOpenCircle);
  gamma_eta9_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9_1d->Draw("p");

  TString ltitle = Form("Photons, #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  lg->Draw();
  lg2->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

void
DrawCluster_Res(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TGraphErrors * gamma_eta0 = DrawCluster_AnaP(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
  TGraphErrors * gamma_eta9 = DrawCluster_AnaP(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX", "gamma_eta0.90", false);

  TGraphErrors * gamma_eta0_1d = DrawCluster_AnaP(
      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0", false);
  TGraphErrors * gamma_eta9_1d = DrawCluster_AnaP(
      base + "/spacal1d_1dtower/zerofield/G4Hits_sPHENIX", "gamma_eta0.90",
      false);

  TCanvas *c1 = new TCanvas("DrawCluster_Res", "DrawCluster_Res", 1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 35, 20e-2,
      ";Incoming Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2, 10e-2, 33, 18e-2, NULL, "br");

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  gamma_eta0->Fit(f_calo, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);

  gamma_eta0->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "2D-proj., #eta = 0.0-0.1, #DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
      f_calo->GetParameter(0), f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0, ltitle.Data(), "pl");

  TF1 * f_calo = new TF1("f_calo_gamma_eta9", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  gamma_eta9->Fit(f_calo, "RM0");

  gamma_eta9->SetLineColor(kRed + 3);
  gamma_eta9->SetMarkerColor(kRed + 3);
  gamma_eta9->SetLineWidth(2);
  gamma_eta9->SetMarkerStyle(kFullCircle);
  gamma_eta9->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 3);
  f_calo->SetLineWidth(2);

  gamma_eta9->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "2D-proj., #eta = 0.9-1.0, #DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
      f_calo->GetParameter(0), f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9, ltitle.Data(), "pl");
  cout << "Title = " << ltitle << endl;

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);

  gamma_eta0_1d->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "1D-proj., #eta = 0.0-0.1, #DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
      f_calo->GetParameter(0), f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0_1d, ltitle.Data(), "pl");

  TF1 * f_calo = new TF1("f_calo_gamma_eta9_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  gamma_eta9_1d->Fit(f_calo, "RM0");

  gamma_eta9_1d->SetLineColor(kBlue + 3);
  gamma_eta9_1d->SetMarkerColor(kBlue + 3);
  gamma_eta9_1d->SetLineWidth(2);
  gamma_eta9_1d->SetMarkerStyle(kOpenCircle);
  gamma_eta9_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 3);
  f_calo->SetLineWidth(2);

  gamma_eta9_1d->Draw("p");
  f_calo->Draw("same");

  TString ltitle = Form(
      "1D-proj., #eta = 0.9-1.0, #DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
      f_calo->GetParameter(0), f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9_1d, ltitle.Data(), "pl");
  cout << "Title = " << ltitle << endl;

  lg->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

TGraphErrors *
DrawCluster_AnaP(
    const TString config =
//        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX", //
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d_1dtower/zerofield/G4Hits_sPHENIX",//
    const TString particle = "gamma_eta0.90", bool mean_or_res = false)
{
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_10GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_16GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_1GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_2GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_32GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_40GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_4GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_50GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_8GeV.root_hist.root

  const int N = 10;
  double es[100] =
    { 1, 2, 4, 8, 10, 16, 24, 32, 40, 50 };
  double mean[100] =
    { 0 };
  double mean_err[100] =
    { 0 };
  double res[100] =
    { 0 };
  double res_err[100] =
    { 0 };

  TCanvas *c1 = new TCanvas(config, config);
  c1->Print(config + ".pdf[");

  for (int i = 0; i < N; ++i)
    {
      TVectorD fit(4);

      fit = DrawCluster_SingleE(config, particle, es[i]);

      mean[i] = fit[0];
      mean_err[i] = fit[1];
      res[i] = fit[2] / mean[i];
      res_err[i] = fit[3] / mean[i];
    }
  c1->Print(config + ".pdf]");
  cout << "DrawCluster_AnaP - save to " << config << ".pdf" << endl;

  if (mean_or_res)
    {
      TGraphErrors * ge = new TGraphErrors(N, es, mean, 0, mean_err);
      ge->Print();
      return ge;
    }
  else
    {
      TGraphErrors * ge = new TGraphErrors(N, es, res, 0, res_err);
      ge->Print();
      return ge;
    }

}

TVectorD
DrawCluster_SingleE( //
    const TString config =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX", //
    const TString particle = "gamma_eta0", const double e = 8)
{
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_8GeV.root_g4cemc_eval.root

  const TString e_GeV = Form("%.0fGeV", e);

  const TString file_name = config + "_" + particle + "_" + e_GeV
      + ".root_g4cemc_eval.root";
  +".root_hist.root";
  cout << "DrawCluster_SingleE - lead " << file_name << endl;

  TFile *f = new TFile(file_name);
  assert(f);
  TTree * ntp_cluster = (TTree *) f->GetObjectChecked("ntp_cluster", "TTree");
  assert(ntp_cluster);

  const TString h_cluster_E_name = TString("h_cluster_E_") + particle + e_GeV;
  TH1F * h_cluster_E = new TH1F(h_cluster_E_name, file_name, 200, 0, e * 1.5);

  TCanvas *c1 = new TCanvas(file_name, file_name);
  ntp_cluster->Draw("e>>" + h_cluster_E_name, "gparticleID==1 && e>0.2");

  const double mean = h_cluster_E->GetMean();
  const double rms = h_cluster_E->GetRMS();

//  const double lower_lim = mean - ((e >= 8) ? (4 / e * rms) : (1.5 * rms));
//  const double lower_lim = mean - ((e >= 8) ? (rms) : (1.5 * rms));
  TF1 * f_gaus_pilot = new TF1("f_gaus_pilot" + h_cluster_E_name, "gaus",
      e * 0.5, e * 1.5);
  f_gaus_pilot->SetParameters(1, mean, rms);
  f_gaus_pilot->SetLineColor(kRed);
  h_cluster_E->Fit(f_gaus_pilot, "MRQS0");
  f_gaus_pilot->Draw("same");

  TF1 * f_gaus = new TF1("f_gaus" + h_cluster_E_name, "gaus",
      f_gaus_pilot->GetParameter(1) - 1.5 * f_gaus_pilot->GetParameter(2),
      f_gaus_pilot->GetParameter(1) + 4 * f_gaus_pilot->GetParameter(2));
  f_gaus->SetParameters(f_gaus_pilot->GetParameter(0),
      f_gaus_pilot->GetParameter(1), f_gaus_pilot->GetParameter(2));

  h_cluster_E->Fit(f_gaus, "MRQS0");
  f_gaus->Draw("same");

  cout << "load_SF - result = " << f_gaus->GetParameter(1) << "+/-"
      << f_gaus->GetParError(1) << endl;

  TVectorD ret(4);
  ret[0] = f_gaus->GetParameter(1);
  ret[1] = f_gaus->GetParError(1);
  ret[2] = f_gaus->GetParameter(2);
  ret[3] = f_gaus->GetParError(2);

  c1->Print(config + ".pdf");

  return ret;
}

void
DrawTowerSum_Res_2Fit_1DSpacalNoSVX(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TGraphErrors * gamma_eta0 = DrawTowerSum_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "e-_eta0", false);
  TGraphErrors * gamma_eta9 = DrawTowerSum_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "e-_eta0.90", false);

  TGraphErrors * gamma_eta0_1d = DrawTowerSum_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "gamma_eta0", false);
  TGraphErrors * gamma_eta9_1d = DrawTowerSum_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "gamma_eta0.90",
      false);

  TCanvas *c1 = new TCanvas("DrawTowerSum_Res_2Fit_1DSpacalNoSVX",
      "DrawTowerSum_Res_2Fit_1DSpacalNoSVX", 1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 35, 20e-2,
      ";Incoming Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2, 9.6e-2, 17, 19.6e-2, NULL, "br");
  TLegend * lg2 = new TLegend(16, 9e-2, 33, 19e-2, NULL, "br");

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta0->Fit(f_calo, "RM0");
  gamma_eta0->Fit(f_calo_l, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0->Draw("p");

  lg->AddEntry(gamma_eta0,
      Form("Electrons, #eta = 0.0-0.1", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta9", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta9", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta9->Fit(f_calo, "RM0");
  gamma_eta9->Fit(f_calo_l, "RM0");

  gamma_eta9->SetLineColor(kRed + 3);
  gamma_eta9->SetMarkerColor(kRed + 3);
  gamma_eta9->SetLineWidth(2);
  gamma_eta9->SetMarkerStyle(kFullCircle);
  gamma_eta9->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9->Draw("p");

  TString ltitle = Form("Electrons, #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");
  gamma_eta0_1d->Fit(f_calo_l, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0_1d->Draw("p");

  TString ltitle = Form("Photons, #eta = 0.0-0.1", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta9_1d->Fit(f_calo, "RM0");
  gamma_eta9_1d->Fit(f_calo_l, "RM0");

  gamma_eta9_1d->SetLineColor(kBlue + 3);
  gamma_eta9_1d->SetMarkerColor(kBlue + 3);
  gamma_eta9_1d->SetLineWidth(2);
  gamma_eta9_1d->SetMarkerStyle(kOpenCircle);
  gamma_eta9_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 3);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 3);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta9_1d->Draw("p");

  TString ltitle = Form("Photons, #eta = 0.9-1.0", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta9_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  cout << "Title = " << ltitle << endl;

  lg->Draw();
  lg2->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

void
DrawTowerSum_Res_2Fit_1DSpacalNoSVX_Eta3(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TGraphErrors * gamma_eta0 = DrawTowerSum_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "e-_eta0.30", false);
//  TGraphErrors * gamma_eta9 = DrawTowerSum_AnaP(
//      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "e-_eta0.90", false);

  TGraphErrors * gamma_eta0_1d = DrawTowerSum_AnaP(
      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "gamma_eta0.30",
      false);
//  TGraphErrors * gamma_eta9_1d = DrawTowerSum_AnaP(
//      base + "/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", "gamma_eta0.90",
//      false);

  TCanvas *c1 = new TCanvas("DrawTowerSum_Res_2Fit_1DSpacalNoSVX_Eta3",
      "DrawTowerSum_Res_2Fit_1DSpacalNoSVX_Eta3", 1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, 0, 35, 20e-2,
      ";Incoming Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2, 9.6e-2, 17, 19.6e-2, NULL, "br");
  TLegend * lg2 = new TLegend(16, 9e-2, 33, 19e-2, NULL, "br");

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta0->Fit(f_calo, "RM0");
  gamma_eta0->Fit(f_calo_l, "RM0");

  gamma_eta0->SetLineColor(kRed + 1);
  gamma_eta0->SetMarkerColor(kRed + 1);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);
  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0->Draw("p");

  lg->AddEntry(gamma_eta0,
      Form("Electrons, #eta = 0.3-0.4", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  TF1 * f_calo = new TF1("f_calo_gamma_eta0_1d", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0_1d", "([0]+[1]/sqrt(x))/100",
      0.5, 60);
  gamma_eta0_1d->Fit(f_calo, "RM0");
  gamma_eta0_1d->Fit(f_calo_l, "RM0");

  gamma_eta0_1d->SetLineColor(kBlue + 1);
  gamma_eta0_1d->SetMarkerColor(kBlue + 1);
  gamma_eta0_1d->SetLineWidth(2);
  gamma_eta0_1d->SetMarkerStyle(kOpenSquare);
  gamma_eta0_1d->SetMarkerSize(2);
  f_calo->SetLineColor(kBlue + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kBlue + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  gamma_eta0_1d->Draw("p");

  TString ltitle = Form("Photons, #eta = 0.3-0.4", f_calo->GetParameter(0),
      f_calo->GetParameter(1));
  lg->AddEntry(gamma_eta0_1d, ltitle.Data(), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", f_calo_l->GetParameter(0),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

  lg->Draw();
  lg2->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

TGraphErrors *
DrawTowerSum_AnaP(
    const TString config =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", //
    const TString particle = "e-_eta0", bool mean_or_res = false)
{
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_10GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_16GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_1GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_2GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_32GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_40GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_4GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_50GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_8GeV.root_hist.root

  const int N = 10;
  double es[100] =
    { 1, 2, 4, 8, 10, 16, 24, 32, 40, 50 };
  double mean[100] =
    { 0 };
  double mean_err[100] =
    { 0 };
  double res[100] =
    { 0 };
  double res_err[100] =
    { 0 };

  TCanvas *c1 = new TCanvas(config, config);
  c1->Print(config + ".pdf[");

  for (int i = 0; i < N; ++i)
    {
      TVectorD fit(4);

      fit = DrawTowerSum_SingleE(config, particle, es[i]);

      mean[i] = fit[0];
      mean_err[i] = fit[1];
      res[i] = fit[2] / mean[i];
      res_err[i] = fit[3] / mean[i];
    }
  c1->Print(config + ".pdf]");
  cout << "DrawTowerSum_AnaP - save to " << config << ".pdf" << endl;

  if (mean_or_res)
    {
      TGraphErrors * ge = new TGraphErrors(N, es, mean, 0, mean_err);
      ge->Print();
      return ge;
    }
  else
    {
      TGraphErrors * ge = new TGraphErrors(N, es, res, 0, res_err);
      ge->Print();
      return ge;
    }

}

TVectorD
DrawTowerSum_SingleE( //
    const TString config =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/nosvtx/spacal1d/fieldmap/G4Hits_sPHENIX", //
    const TString particle = "e-_eta0", const double e = 8)
{
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_8GeV.root_g4cemc_eval.root

  const TString e_GeV = Form("%.0fGeV", e);

  const TString file_name = config + "_" + particle + "_" + e_GeV
      + ".root_DSTReader.root";
  cout << "DrawTowerSum_SingleE - lead " << file_name << endl;

  TFile *f = new TFile(file_name);
  assert(f);
  TTree * T = (TTree *) f->GetObjectChecked("T", "TTree");
  assert(T);

  const TString h_cluster_E_name = TString("h_cluster_E_") + particle + e_GeV;
  TH1F * h_cluster_E = new TH1F(h_cluster_E_name, file_name, 200, 0, e * 1.5);

  TCanvas *c1 = new TCanvas(file_name, file_name);
  T->Draw("Sum$(TOWER_SIM_CEMC.energy)/ 2.36081e-02>>" + h_cluster_E_name, "");

  const double mean = h_cluster_E->GetMean();
  const double rms = h_cluster_E->GetRMS();

//  const double lower_lim = mean - ((e >= 8) ? (4 / e * rms) : (1.5 * rms));
//  const double lower_lim = mean - ((e >= 8) ? (rms) : (1.5 * rms));
  TF1 * f_gaus_pilot = new TF1("f_gaus_pilot" + h_cluster_E_name, "gaus",
      e * 0.5, e * 1.5);
  f_gaus_pilot->SetParameters(1, mean, rms);
  f_gaus_pilot->SetLineColor(kRed);
  h_cluster_E->Fit(f_gaus_pilot, "MRQS0");
  f_gaus_pilot->Draw("same");

  TF1 * f_gaus = new TF1("f_gaus" + h_cluster_E_name, "gaus",
      f_gaus_pilot->GetParameter(1) - 1.5 * f_gaus_pilot->GetParameter(2),
      f_gaus_pilot->GetParameter(1) + 4 * f_gaus_pilot->GetParameter(2));
  f_gaus->SetParameters(f_gaus_pilot->GetParameter(0),
      f_gaus_pilot->GetParameter(1), f_gaus_pilot->GetParameter(2));

  h_cluster_E->Fit(f_gaus, "MRQS0");
  f_gaus->Draw("same");

  cout << "load_SF - result = " << f_gaus->GetParameter(1) << "+/-"
      << f_gaus->GetParError(1) << endl;

  TVectorD ret(4);
  ret[0] = f_gaus->GetParameter(1);
  ret[1] = f_gaus->GetParError(1);
  ret[2] = f_gaus->GetParameter(2);
  ret[3] = f_gaus->GetParError(2);

  c1->Print(config + ".pdf");

  return ret;
}

void
DrawSF(
    const TString base =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/",
    const TString config = "")
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TCanvas *c1 = new TCanvas("DrawSF", "DrawSF", 1800, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(-0.1, 2e-2, 1.2, 3.0e-2,
      ";Pseudorapidity;EMCal Sampling fraction");

  TLegend * lg = new TLegend(0, 2.55e-2, 0.6, 2.9e-2, NULL, "br");

  TGraphErrors * ge = Load_CurveSet(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX_gamma", "24GeV");
  ge->SetLineColor(kRed + 3);
  ge->SetMarkerColor(kRed + 3);
  ge->SetLineWidth(2);
  ge->SetMarkerStyle(kFullSquare);
  ge->SetMarkerSize(2);
  ge->Draw("lep");
  lg->AddEntry(ge, "24 GeV #gamma in 2D-proj. SPACAL", "lp");

  TGraphErrors * ge = Load_CurveSet(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX_gamma", "24GeV");
  ge->SetLineColor(kBlue + 3);
  ge->SetMarkerColor(kBlue + 3);
  ge->SetLineStyle(kDashed);
  ge->SetLineWidth(2);
  ge->SetMarkerStyle(kOpenSquare);
  ge->SetMarkerSize(2);
  ge->Draw("lep");
  lg->AddEntry(ge, "24 GeV #gamma in 1D-proj. SPACAL", "lp");

  TGraphErrors * ge = Load_CurveSet(
      base + "/spacal2d/zerofield/G4Hits_sPHENIX_e-", "4GeV");
  ge->SetLineColor(kRed + 3);
  ge->SetMarkerColor(kRed + 3);
  ge->SetLineWidth(2);
  ge->SetMarkerStyle(kFullCircle);
  ge->SetMarkerSize(2);
  ge->Draw("lep");
  lg->AddEntry(ge, "4 GeV e^{-} in 2D-proj. SPACAL", "lp");

  TGraphErrors * ge = Load_CurveSet(
      base + "/spacal1d/zerofield/G4Hits_sPHENIX_e-", "4GeV");
  ge->SetLineColor(kBlue + 3);
  ge->SetMarkerColor(kBlue + 3);
  ge->SetLineStyle(kDashed);
  ge->SetLineWidth(2);
  ge->SetMarkerStyle(kOpenCircle);
  ge->SetMarkerSize(2);
  ge->Draw("lep");
  lg->AddEntry(ge, "4 GeV e^{-} in 1D-proj. SPACAL", "lp");

  lg->Draw();

  SaveCanvas(c1, base + "DrawEcal_" + TString(c1->GetName()), true);
}

TGraphErrors *
Load_CurveSet(
    const TString config =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma",
    const TString energy = "24GeV")
{
  double etas[100] =
    { 0 };
  double SFs[100] =
    { 0 };
  double SF_Errs[100] =
    { 0 };

  int n = 0;
  TVectorD SF(2);

  etas[n] = 0 + 0.05;
  SF = Load_SF("eta0", config, energy);
  SFs[n] = SF[0];
  SF_Errs[n] = SF[1];
  n++;

  etas[n] = 0.3 + 0.05;
  SF = Load_SF("eta0.30", config, energy);
  SFs[n] = SF[0];
  SF_Errs[n] = SF[1];
  n++;

  etas[n] = 0.6 + 0.05;
  SF = Load_SF("eta0.60", config, energy);
  SFs[n] = SF[0];
  SF_Errs[n] = SF[1];
  n++;

  etas[n] = 0.9 + 0.05;
  SF = Load_SF("eta0.90", config, energy);
  SFs[n] = SF[0];
  SF_Errs[n] = SF[1];
  n++;

  TGraphErrors * ge = new TGraphErrors(n, etas, SFs, 0, SF_Errs);

  return ge;

}

TVectorD
Load_SF(const TString eta_bin = "eta0",
    const TString config =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma",
    const TString energy = "24GeV")
{
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma_eta0.30_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma_eta0.60_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma_eta0.90_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal1d/zerofield/G4Hits_sPHENIX_gamma_eta0_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0.30_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0.60_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0.90_24GeV.root_hist.root
//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_eta0_24GeV.root_hist.root

  const TString file_name = config + "_" + eta_bin + "_" + energy
      + ".root_EMCalAna.root";
  cout << "load_SF - lead " << file_name << endl;

  TFile *f = new TFile(file_name);
  assert(f);
  TH1F * EMCalAna_h_CEMC_SF = (TH1F *) f->GetObjectChecked("EMCalAna_h_CEMC_SF",
      "TH1F");
  assert(EMCalAna_h_CEMC_SF);

  const double mean = EMCalAna_h_CEMC_SF->GetMean();
  const double rms = EMCalAna_h_CEMC_SF->GetRMS();

  TF1 * f_gaus = new TF1("f_gaus" + eta_bin, "gaus", mean - 2 * rms,
      mean + 2 * rms);
  f_gaus->SetParameters(1, mean, rms);

  EMCalAna_h_CEMC_SF->Fit(f_gaus, "MRQS0");

  cout << "load_SF - result = " << f_gaus->GetParameter(1) << "+/-"
      << f_gaus->GetParError(1) << endl;

  TVectorD ret(2);
  ret[0] = f_gaus->GetParameter(1);
  ret[1] = f_gaus->GetParError(1);

  return ret;
}

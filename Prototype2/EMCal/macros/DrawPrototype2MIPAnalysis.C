// $Id: $                                                                                             

/*!
 * \file Draw.C
 * \brief 
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
#include "SaveCanvas.C"
using namespace std;

//#include "Prototype2_DSTReader.h"

TFile * _file0 = NULL;
TTree * T = NULL;
TString cuts = "";
//double beam_momentum_selection = -16;
double beam_momentum_selection = 120;

void
DrawPrototype2MIPAnalysis( //
    const TString infile =
        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/Production_1101_EMCalSet2_HCalPR12/beam_00002187-0000_DSTReader.root", //
    bool plot_all = false, const double momentum = 120)
{
  beam_momentum_selection = momentum;
  gROOT->SetStyle("Modern");


//  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");
  gSystem->Load("libqa_modules.so");
  gSystem->Load("libPrototype2.so");

//  gROOT->LoadMacro("Prototype2_DSTReader.C+");

  if (!_file0)
    {
      TString chian_str = infile;
      chian_str.ReplaceAll("ALL", "*");

      TChain * t = new TChain("T");
      const int n = t->Add(chian_str);

      cout << "Loaded " << n << " root files with " << chian_str << endl;
      assert(n > 0);

      T = t;

      _file0 = new TFile;
      _file0->SetName(infile);
    }

  assert(_file0);

  T->SetAlias("ActiveTower_LG",
      "TOWER_LG_CEMC[].get_binphi()<8 && TOWER_LG_CEMC[].get_bineta()<8");
  T->SetAlias("EnergySum_LG",
      "1*Sum$(TOWER_LG_CEMC[].get_energy() * ActiveTower_LG)");

  T->SetAlias("ActiveTower_HG",
      "TOWER_HG_CEMC[].get_binphi()<8 && TOWER_HG_CEMC[].get_bineta()<8");
  T->SetAlias("EnergySum_HG",
      "1*Sum$(TOWER_HG_CEMC[].get_energy() * ActiveTower_HG)");

//  T->SetAlias("C2_Inner_e", "1*TOWER_RAW_C2[0].energy");
  T->SetAlias("C2_Inner_e", "1*abs(TOWER_RAW_C2[2].energy)");
  T->SetAlias("C2_Outer_e", "1*abs(TOWER_RAW_C2[3].energy)");
  T->SetAlias("C2_Sum_e", "C2_Inner_e + C2_Outer_e");

//  "TOWER_CALIB_CEMC.energy * ( Sum$( TOWER_CALIB_CEMC.get_column()==2 && TOWER_CALIB_CEMC.get_row()==1

  T->SetAlias("Average_column",
      "Sum$(TOWER_CALIB_CEMC.get_column() * TOWER_CALIB_CEMC.get_energy())/Sum$(TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Average_row",
      "Sum$(TOWER_CALIB_CEMC.get_row() * TOWER_CALIB_CEMC.get_energy())/Sum$(TOWER_CALIB_CEMC.get_energy())");

  T->SetAlias("Average_HODO_VERTICAL",
      "Sum$(TOWER_CALIB_HODO_VERTICAL.towerid * (abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) * abs(TOWER_CALIB_HODO_VERTICAL.energy))/Sum$((abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) * abs(TOWER_CALIB_HODO_VERTICAL.energy))");
  //T->SetAlias("Valid_HODO_VERTICAL",
  //  "Sum$(abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) > 0");

  T->SetAlias("Valid_HODO_VERTICAL",
      "Sum$(abs(TOWER_CALIB_HODO_VERTICAL.energy)>30 && TOWER_CALIB_HODO_VERTICAL.towerid==3 ) ");
  //  "Sum$(abs(TOWER_CALIB_HODO_VERTICAL.energy)>30)==1");

  T->SetAlias("Average_HODO_HORIZONTAL",
      "Sum$(TOWER_CALIB_HODO_HORIZONTAL.towerid * (abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))/Sum$((abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))");
  //T->SetAlias("Valid_HODO_HORIZONTAL",
  //  "Sum$(abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) > 0");
  T->SetAlias("Valid_HODO_HORIZONTAL",
      "Sum$(abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30)==1");

  T->SetAlias("No_Triger_VETO",
      "Sum$(abs(TOWER_RAW_TRIGGER_VETO.energy)>15)==0");

  T->SetAlias("Energy_Sum_col1_row2_3x3",
      "Sum$( (abs(TOWER_CALIB_CEMC.get_column()-1)<=1 && abs(TOWER_CALIB_CEMC.get_row()-2)<=1 ) * TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Energy_Sum_col1_row2_5x5",
      "Sum$( (abs(TOWER_CALIB_CEMC.get_column()-1)<=2 && abs(TOWER_CALIB_CEMC.get_row()-2)<=2 ) * TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Energy_Sum_col2_row2_5x5",
      "Sum$( (abs(TOWER_CALIB_CEMC.get_column()-2)<=2 && abs(TOWER_CALIB_CEMC.get_row()-2)<=2 ) * TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Energy_Sum_CEMC", "1*Sum$(TOWER_CALIB_CEMC.get_energy())");

  // 12 GeV calibration
//  EDM=9.83335e-18    STRATEGY= 1      ERROR MATRIX ACCURATE
//EXT PARAMETER                                   STEP         FIRST
//NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE
//1  p0           1.19768e+01   7.30605e-02   3.76799e-05   4.24290e-09
//2  p1           8.71776e+00   6.82987e-02   3.52240e-05   6.80808e-08

//    T->SetAlias("Energy_Sum_Hadron_CEMC",
//        "1*Sum$(TOWER_CALIB_CEMC.get_energy())"); // full bias
  T->SetAlias("Energy_Sum_Hadron_CEMC",
      "1.14*12./8.71776e+00*Sum$(TOWER_CALIB_CEMC.get_energy())"); // full bias
//  T->SetAlias("Energy_Sum_Hadron_CEMC",
//      "1.14*12./8.71776e+00*(16./6.93250e+00)*(28/33.3405)*Sum$(TOWER_CALIB_CEMC.get_energy())"); // half-gain bias
  T->SetAlias("CEMC_MIP", "Energy_Sum_Hadron_CEMC<0.7");

  // 12 GeV calibration
//  FCN=9.63681 FROM HESSE     STATUS=OK             14 CALLS          56 TOTAL
//                      EDM=1.49963e-17    STRATEGY= 1      ERROR MATRIX ACCURATE
//   EXT PARAMETER                                   STEP         FIRST
//   NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE
//    1  p0           9.50430e+00   8.42691e-02   3.83666e-06   1.41105e-08
//    2  p1           6.99727e+00   1.06583e-01   4.85258e-06   5.85631e-08

//  T->SetAlias("Energy_Sum_Hadron_HCALIN",
//      "1*Sum$(TOWER_CALIB_LG_HCALIN.get_energy())");
//  T->SetAlias("HCALIN_MIP", "Energy_Sum_Hadron_HCALIN<0.5");
//  T->SetAlias("Energy_Sum_Hadron_HCALOUT",
//      "1*Sum$(TOWER_CALIB_LG_HCALOUT.get_energy())");
  T->SetAlias("Energy_Sum_Hadron_HCALIN",
      "12./6.99727e+00*Sum$(TOWER_CALIB_LG_HCALIN.get_energy())");
  T->SetAlias("HCALIN_MIP", "Energy_Sum_Hadron_HCALIN<0.5");
  T->SetAlias("Energy_Sum_Hadron_HCALOUT",
      "12./9.50430e+00*Sum$(TOWER_CALIB_LG_HCALOUT.get_energy())");

  T->SetAlias("MIP_Count_Col2",
      "Sum$( abs( TOWER_RAW_CEMC.get_energy() )>20 && abs( TOWER_RAW_CEMC.get_energy() )<400 && TOWER_CALIB_CEMC.get_column() == 2)");

  T->SetAlias("Pedestal_Count_AllCEMC",
      "Sum$( abs( TOWER_RAW_CEMC.get_energy() )<20 && TOWER_CALIB_CEMC.get_column() != 2)");
  T->SetAlias("TowerTimingGood_Count_AllCEMC",
      "Sum$( abs( TOWER_RAW_CEMC.get_time() )>6 && abs( TOWER_RAW_CEMC.get_time() )<13 && TOWER_CALIB_CEMC.get_column() == 2)");

//
  TCut event_sel = "1*1";

  if (plot_all)
    {
//      event_sel = "1*1";
//      cuts = "_all_event";
//      event_sel = "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL";
//      cuts = "_Valid_HODO";
      event_sel =
          "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO";
      cuts = "_Valid_HODO_Trigger_VETO";
    }
  else
    {

//      event_sel = "1*1";
//      cuts = "_all_event";
//      event_sel = "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL";
//      cuts = "_Valid_HODO";
//      event_sel =
//          "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO";
//      cuts = "_Valid_HODO_Trigger_VETO";

//      event_sel =
//          "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO && (Pedestal_Count_AllCEMC >= 64 -8)"; // && (MIP_Count_Col2==8)";
//      cuts = "_Valid_HODO_MIP_Col2_PedestalOther";

      event_sel =
          "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO && (Pedestal_Count_AllCEMC >= 64 -8) && (TowerTimingGood_Count_AllCEMC == 8)"; // && (MIP_Count_Col2==8)";
      cuts = "_Valid_HODO_MIP_Col2_PedestalOther_TimingGood";

      //            event_sel =
      //      event_sel
      // 	    + "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO";
      // cuts = cuts + "_Valid_HODO_Trigger_VETO";
      //+ "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO  && (C2_Sum_e<50)";
      //     cuts = cuts + "_Valid_HODO_Trigger_VETO_C2_Sum_Hadron";

//      event_sel =
//          event_sel
//              + "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO && CEMC_MIP && (C2_Sum_e<100)";
//      cuts = cuts + "_Valid_HODO_Trigger_VETO_CEMC_MIP_C2_Sum_Hadron";

//      event_sel = "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && (MIP_Count_Col2 == 8) && (Pedestal_Count_AllCEMC >= 64 - 8)";
//      cuts = "_Valid_HODO_MIP_Col2_PedestalOther";

//      event_sel = "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && Average_HODO_VERTICAL>1.5 && Average_HODO_VERTICAL<4.5 && Average_HODO_HORIZONTAL>3.5 && Average_HODO_HORIZONTAL<6.5";
//      cuts = "_Valid_HODO_center_col1_row2";

//      event_sel =
//          "C2_Inner_e>100 && Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && Average_HODO_HORIZONTAL>1.5 && Average_HODO_HORIZONTAL<4.5 && Average_HODO_VERTICAL>3.5 && Average_HODO_VERTICAL<6.5";
//      cuts = "_Valid_HODO_center_col1_row2_C2";

//      event_sel = "abs(Average_column - 1)<.5 && abs(Average_row - 2) < .5";
//      cuts = "_tower_center_col1_row2";
//      event_sel = "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && abs(C2_Inner_e)<10";
//      cuts = "_Valid_HODO_VetoC2";
    }

  cout << "Build event selection of " << (const char *) event_sel << endl;

  T->Draw(">>EventList", event_sel);
  TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected"
      << endl;

  T->SetEventList(elist);

//  int rnd = rand();
//  gDirectory->mkdir(Form("dir_%d", rnd));
//  gDirectory->cd(Form("dir_%d", rnd));
//  if (plot_all)
//    EMCDistribution_SUM("Energy_Sum_col1_row2_5x5");

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
//  if (plot_all)
  EMCDistribution_Fast("RAW");

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  //if (plot_all)
  EMCDistribution_PeakSample_Fast();

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  if (plot_all)
    EMCDistribution_Fast("CALIB", true);

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  if (plot_all)
  EMCDistribution_ADC();

//  if (!plot_all)
//    T->Process("Prototype2_DSTReader.C+",
//        TString(_file0->GetName())
//            + TString("_DrawPrototype2MIPAnalysis_Prototype2_DSTReader") + cuts
//            + TString(".dat"));
}

void
EMCDistribution_Fast(TString gain = "CALIB", bool full_gain = false)
{
  TString hname = "EMCDistribution_" + gain
      + TString(full_gain ? "_FullGain" : "") + cuts;

  TH2 * h2 = NULL;
  if (gain.Contains("CALIB"))
    {
      if (full_gain)
        {
          h2 = new TH2F(hname,
              Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 100, .05,
              25, 64, -.5, 63.5);
          QAHistManagerDef::useLogBins(h2->GetXaxis());
        }
      else
        {
          h2 = new TH2F(hname,
              Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 260, -.2,
              5, 64, -.5, 63.5);
        }
      T->Draw(
          "TOWER_" + gain + "_CEMC[].get_bineta() + 8* TOWER_" + gain
              + "_CEMC[].get_binphi():TOWER_" + gain + "_CEMC[].get_energy()>>"
              + hname, "", "goff");
    }
  else if (gain.Contains("RAW"))
    {
      if (full_gain)
        {
          h2 = new TH2F(hname,
              Form(";Calibrated Tower Energy Sum (ADC);Count / bin"), 100,
              .05 * 100, 25 * 100, 64, -.5, 63.5);
          QAHistManagerDef::useLogBins(h2->GetXaxis());
        }
      else
        {
          h2 = new TH2F(hname,
              Form(";Calibrated Tower Energy Sum (ADC);Count / bin"), 260,
              -.2 * 100, 5 * 100, 64, -.5, 63.5);
        }
      T->Draw(
          "TOWER_" + gain + "_CEMC[].get_bineta() + 8* TOWER_" + gain
              + "_CEMC[].get_binphi():TOWER_" + gain
              + "_CEMC[].get_energy()*(-1)>>" + hname, "", "goff");
    }

  TText * t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_" + gain + TString(full_gain ? "_FullGain" : "") + cuts,
      "EMCDistribution_" + gain + TString(full_gain ? "_FullGain" : "") + cuts,
      1800, 950);
  c1->Divide(8, 8, 0., 0.01);
  int idx = 1;
  TPad * p;

  for (int iphi = 8 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 8; ieta++)
        {
          p = (TPad *) c1->cd(idx++);
          c1->Update();

          p->SetLogy();
          if (full_gain)
            {
              p->SetLogx();
            }
          p->SetGridx(0);
          p->SetGridy(0);

          TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi)
              + TString(full_gain ? "_FullGain" : "");

          TH1 * h = h2->ProjectionX(hname, ieta + 8 * iphi + 1,
              ieta + 8 * iphi + 1); // axis bin number is encoded as ieta+8*iphi+1

          h->SetLineWidth(0);
          h->SetLineColor(kBlue + 3);
          h->SetFillColor(kBlue + 3);

          h->GetXaxis()->SetTitleSize(.09);
          h->GetXaxis()->SetLabelSize(.08);
          h->GetYaxis()->SetLabelSize(.08);

          h->Draw();

          if (full_gain)
            h->Fit("x*gaus", "M");
          else
            h->Fit("landau", "M");

          double peak = -1;

          TF1 * fit = ((TF1 *) (h->GetListOfFunctions()->At(0)));
          if (fit)
            {

              fit->SetLineColor(kRed);
              peak = fit->GetParameter(1);

            }

          cout << Form("Finished <Col%d Row%d> = %.1f", ieta, iphi, peak)
              << endl;

          TText *t = new TText(.9, .9,
              Form("<Col%d Row%d> = %.1f", ieta, iphi, peak));
          t->SetTextAlign(33);
          t->SetTextSize(.15);
          t->SetNDC();
          t->Draw();
        }
    }

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawPrototype2MIPAnalysis_")
          + TString(c1->GetName()), false);

}

void
EMCDistribution_PeakSample_Fast(bool full_gain = false)
{

  const TString gain = "RAW";

  TString hname = "EMCDistribution_" + gain
      + TString(full_gain ? "_FullGain" : "") + cuts;

  TH2 * h2 = NULL;
    {
      if (full_gain)
        {
          h2 = new TH2F(hname,
              Form(";Calibrated Tower Energy Sum (ADC);Count / bin"), 100,
              .05 * 100, 25 * 100, 64, -.5, 63.5);
          QAHistManagerDef::useLogBins(h2->GetXaxis());
        }
      else
        {
          h2 = new TH2F(hname,
              Form(";Calibrated Tower Energy Sum (ADC);Count / bin"), 260,
              -.2 * 100, 5 * 100, 64, -.5, 63.5);
        }
      T->Draw(
          "TOWER_" + gain + "_CEMC[].get_bineta() + 8* TOWER_" + gain
              + "_CEMC[].get_binphi():(TOWER_RAW_CEMC[].signal_samples[10] - TOWER_RAW_CEMC[].signal_samples[0])*(-1)>>"
              + hname, "", "goff");
    }

  TText * t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_PeakSample_Fast_" + TString(full_gain ? "_FullGain" : "")
          + cuts,
      "EMCDistribution_PeakSample_Fast_" + TString(full_gain ? "_FullGain" : "")
          + cuts, 1800, 950);
  c1->Divide(8, 8, 0., 0.01);
  int idx = 1;
  TPad * p;

  for (int iphi = 8 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 8; ieta++)
        {
          p = (TPad *) c1->cd(idx++);
          c1->Update();

          p->SetLogy();
          if (full_gain)
            {
              p->SetLogx();
            }
          p->SetGridx(0);
          p->SetGridy(0);

          TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi)
              + TString(full_gain ? "_FullGain" : "");

          TH1 * h = h2->ProjectionX(hname, ieta + 8 * iphi + 1,
              ieta + 8 * iphi + 1); // axis bin number is encoded as ieta+8*iphi+1

          h->SetLineWidth(0);
          h->SetLineColor(kBlue + 3);
          h->SetFillColor(kBlue + 3);

          h->GetXaxis()->SetTitleSize(.09);
          h->GetXaxis()->SetLabelSize(.08);
          h->GetYaxis()->SetLabelSize(.08);

          h->Draw();

          if (full_gain)
            h->Fit("x*gaus", "M");
          else
            h->Fit("landau", "M");

          double peak = -1;

          TF1 * fit = ((TF1 *) (h->GetListOfFunctions()->At(0)));
          if (fit)
            {

              fit->SetLineColor(kRed);
              peak = fit->GetParameter(1);

            }

          cout << Form("Finished <Col%d Row%d> = %.1f", ieta, iphi, peak)
              << endl;

          TText *t = new TText(.9, .9,
              Form("<Col%d Row%d> = %.1f", ieta, iphi, peak));
          t->SetTextAlign(33);
          t->SetTextSize(.15);
          t->SetNDC();
          t->Draw();
        }
    }

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawPrototype2MIPAnalysis_")
          + TString(c1->GetName()), false);

}

void
EMCDistribution_ADC(bool log_scale = true)
{
  TString gain = "RAW";

  TText * t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_ADC_" + gain + TString(log_scale ? "_Log" : "") + cuts,
      "EMCDistribution_ADC_" + gain + TString(log_scale ? "_Log" : "") + cuts,
      1800, 1000);
  c1->Divide(8, 8, 0., 0.01);
  int idx = 1;
  TPad * p;

  for (int iphi = 8 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 8; ieta++)
        {
          p = (TPad *) c1->cd(idx++);
          c1->Update();

          if (log_scale)
            {
              p->SetLogz();
            }
          p->SetGridx(0);
          p->SetGridy(0);

          TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi)
              + TString(log_scale ? "_Log" : "");

          TH1 * h = NULL;

          if (log_scale)
            h = new TH2F(hname,
                Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 24, -.5,
                23.5,
//                128+64, 0, 3096);
                550, 1500, 2050);
//          else
//            h = new TH2F(hname,
//                Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 100,
//                -.050, .5,128,0,2048);

          h->SetLineWidth(0);
          h->SetLineColor(kBlue + 3);
          h->SetFillColor(kBlue + 3);
          h->GetXaxis()->SetTitleSize(.09);
          h->GetXaxis()->SetLabelSize(.08);
          h->GetYaxis()->SetLabelSize(.08);

//          if (log_scale)
//            QAHistManagerDef::useLogBins(h->GetYaxis());

          TString sdraw = "TOWER_" + gain
              + "_CEMC[].signal_samples[]:fmod(Iteration$,24)>>" + hname;
          TString scut =
              Form(
                  "TOWER_%s_CEMC[].get_bineta()==%d && TOWER_%s_CEMC[].get_binphi()==%d",
                  gain.Data(), ieta, gain.Data(), iphi);

          cout << "T->Draw(\"" << sdraw << "\",\"" << scut << "\");" << endl;

          T->Draw(sdraw, scut, "colz");

          TText *t = new TText(.9, .9, Form("Col%d Row%d", ieta, iphi));
          t->SetTextAlign(33);
          t->SetTextSize(.15);
          t->SetNDC();
          t->Draw();

//          return;
        }
    }

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawPrototype2MIPAnalysis_")
          + TString(c1->GetName()), false);

}


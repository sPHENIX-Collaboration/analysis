// $Id: $

/*!
 * \file Draw.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <TFile.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TLine.h>
#include <TString.h>
#include <TTree.h>
#include <cassert>
#include <cmath>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

//#include "Prototype4_DSTReader.h"

TFile *_file0 = NULL;
TTree *T = NULL;
TString cuts = "";
double beam_momentum_selection = -16;

void DrawPrototype4EMCalTower(                                        //
                                                                      //    const TString infile = "/gpfs/mnt/gpfs04/sphenix/user/jinhuang/Prototype_2018/test_production_5/beam_00000683-0000_DSTReader.root",  //
                                                                      //    const TString infile = "/gpfs/mnt/gpfs04/sphenix/user/jinhuang/Prototype_2018/Scan2Block18/beam_0000068ALL-0000_DSTReader.root",  //
                                                                      //    const TString infile = "/gpfs/mnt/gpfs04/sphenix/user/jinhuang/Prototype_2018/Scan1Block36/2GeV/beam_00000ALL-0000_DSTReader.root",  //
    const TString infile = "data/beam_00001214.root_DSTReader.root",  //
    bool plot_all = false, const double momentum = -16)
{
  beam_momentum_selection = momentum;

  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");
  gSystem->Load("libqa_modules.so");
  gSystem->Load("libPrototype4.so");

  //  gROOT->LoadMacro("Prototype4_DSTReader.C+");

  if (!_file0)
  {
    TString chian_str = infile;
    chian_str.ReplaceAll("ALL", "*");

    TChain *t = new TChain("T");
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
  T->SetAlias("C2_Inner_e", "1*(TOWER_CALIB_C2[0].energy)");
  T->SetAlias("C2_Outer_e", "1*(TOWER_CALIB_C2[1].energy)");
  T->SetAlias("C2_Sum_e", "C2_Inner_e + C2_Outer_e");
  T->SetAlias("C1", "0 + TOWER_CALIB_C1[0].energy");

  //  "TOWER_CALIB_CEMC.energy * ( Sum$( TOWER_CALIB_CEMC.get_column()==2 && TOWER_CALIB_CEMC.get_row()==1

  T->SetAlias("Average_column",
              "Sum$(TOWER_CALIB_CEMC.get_column() * TOWER_CALIB_CEMC.get_energy())/Sum$(TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Average_row",
              "Sum$(TOWER_CALIB_CEMC.get_row() * TOWER_CALIB_CEMC.get_energy())/Sum$(TOWER_CALIB_CEMC.get_energy())");

  T->SetAlias("Average_HODO_VERTICAL",
              "Sum$(TOWER_CALIB_HODO_VERTICAL.towerid * (abs(TOWER_CALIB_HODO_VERTICAL.energy)>0.5) * abs(TOWER_CALIB_HODO_VERTICAL.energy))/Sum$((abs(TOWER_CALIB_HODO_VERTICAL.energy)>0.5) * abs(TOWER_CALIB_HODO_VERTICAL.energy))");
  T->SetAlias("Valid_HODO_VERTICAL", "Sum$((TOWER_CALIB_HODO_VERTICAL.energy)>0.5) > 0");

  T->SetAlias("Average_HODO_HORIZONTAL",
              "Sum$(TOWER_CALIB_HODO_HORIZONTAL.towerid * (abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>0.5) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))/Sum$((abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>0.5) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))");
  T->SetAlias("Valid_HODO_HORIZONTAL", "Sum$((TOWER_CALIB_HODO_HORIZONTAL.energy)>0.5) > 0");

  T->SetAlias("No_Triger_VETO",
              "Sum$((TOWER_CALIB_TRIGGER_VETO.energy)>0.2)==0");

  T->SetAlias("Energy_Sum_col1_row2_3x3",
              "Sum$( (abs(TOWER_CALIB_CEMC.get_column()-1)<=1 && abs(TOWER_CALIB_CEMC.get_row()-2)<=1 ) * TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Energy_Sum_col1_row2_5x5",
              "Sum$( (abs(TOWER_CALIB_CEMC.get_column()-1)<=2 && abs(TOWER_CALIB_CEMC.get_row()-2)<=2 ) * TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Energy_Sum_col2_row2_5x5",
              "Sum$( (abs(TOWER_CALIB_CEMC.get_column()-2)<=2 && abs(TOWER_CALIB_CEMC.get_row()-2)<=2 ) * TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Energy_Sum_CEMC", "1*Sum$(TOWER_CALIB_CEMC.get_energy())");
  T->SetAlias("Energy_Sum_RAW_CEMC", "1*Sum$(TOWER_RAW_CEMC.get_energy())");

  // 12 GeV calibration
  //  EDM=9.83335e-18    STRATEGY= 1      ERROR MATRIX ACCURATE
  //EXT PARAMETER                                   STEP         FIRST
  //NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE
  //1  p0           1.19768e+01   7.30605e-02   3.76799e-05   4.24290e-09
  //2  p1           8.71776e+00   6.82987e-02   3.52240e-05   6.80808e-08

  //    T->SetAlias("Energy_Sum_Hadron_CEMC",
  //        "1*Sum$(TOWER_CALIB_CEMC.get_energy())"); // full bias
  T->SetAlias("Energy_Sum_Hadron_CEMC",
              "1.14*12./8.71776e+00*Sum$(TOWER_CALIB_CEMC.get_energy())");  // full bias
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
              "Sum$( abs( TOWER_RAW_CEMC.get_energy() )>20 && abs( TOWER_RAW_CEMC.get_energy() )<200 && TOWER_CALIB_CEMC.get_column() == 2 )");
  T->SetAlias("Pedestal_Count_AllCEMC",
              "Sum$( abs( TOWER_RAW_CEMC.get_energy() )<20)");

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
    if (0)
    {  // energy selection
      event_sel = Form("(beam_MTNRG_GeV == %f)", beam_momentum_selection);
      cuts = Form("_%.0fGeV", beam_momentum_selection);

      cout << "Build event selection of " << (const char *) event_sel
           << endl;

      T->Draw(">>EventListRunCut", event_sel);
      TEventList *elist = gDirectory->GetObjectChecked("EventListRunCut",
                                                       "TEventList");
      cout << elist->GetN() << " / " << T->GetEntriesFast()
           << " events selected" << endl;
      T->SetEventList(elist);
    }

    //    event_sel = "1*1";
    //    cuts = "_all_event";

    //      event_sel = "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL";
    //      cuts = "_Valid_HODO";

        event_sel =
            "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO";
        cuts = "_Valid_HODO_Trigger_VETO";

//    event_sel =
//        "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO && abs(Average_HODO_HORIZONTAL - 3)<.2 && abs(Average_HODO_VERTICAL - 4)<.2 ";
//    cuts = "_Valid_HODO_Trigger_VETO_h3_v4";
    //    event_sel =
    //        "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO && abs(Average_HODO_HORIZONTAL - 3)<.2 && abs(Average_HODO_VERTICAL - 4)<.2 ";
    //    cuts = "_Valid_HODO_Trigger_VETO_h3_v4";
    //    event_sel =
    //        "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO && abs(Average_HODO_HORIZONTAL - 3)<.2 && abs(Average_HODO_VERTICAL - 4)<.2 && (C2_Outer_e>2000 || C2_Inner_e>1000)";
    //    cuts = "_Valid_HODO_Trigger_VETO_good_e_h3_v4";
    //    event_sel =
    //        "Entry$==1643 && Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO && abs(Average_HODO_HORIZONTAL - 5)<.2 && abs(Average_HODO_VERTICAL - 2)<.2 && (C2_Outer_e>2000 || C2_Inner_e>1000)";
    //    cuts = "_Valid_HODO_Trigger_VETO_good_e_h5_v2_ev1643";
  }

  cout << "Build event selection of " << (const char *) event_sel << endl;

  T->Draw(">>EventList", event_sel);
  TEventList *elist = gDirectory->GetObjectChecked("EventList", "TEventList");
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
    EMC_HodoScope_Calibration("C2_Sum_e");

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  if (plot_all)
    EMCDistribution_ShowShape("C2_Sum_e");

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  if (plot_all)
    EMCDistribution_SUM("Energy_Sum_CEMC", "C2_Sum_e");

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  //  if (plot_all)
  EMCDistribution_SUM_RawADC("Energy_Sum_RAW_CEMC", "C2_Sum_e");

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  if (plot_all)
    EMCDistribution_HCalCalibration();

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  if (plot_all)
    EMCDistribution_Fast();

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  if (plot_all)
    EMCDistribution_Fast("RAW");

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  if (plot_all)
    EMCDistribution_Fast("RAW", true);
  //
  //  int rnd = rand();
  //  gDirectory->mkdir(Form("dir_%d", rnd));
  //  gDirectory->cd(Form("dir_%d", rnd));
  //  if (plot_all)
  //    EMCDistribution_PeakSample_Fast();

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
  if (plot_all)
    EMCDistribution_Fast("CALIB", true);

  int rnd = rand();
  gDirectory->mkdir(Form("dir_%d", rnd));
  gDirectory->cd(Form("dir_%d", rnd));
//  if (plot_all)
    EMCDistribution_ADC();

  //  if (!plot_all)
  //    T->Process("Prototype4_DSTReader.C+",
  //        TString(_file0->GetName())
  //            + TString("_DrawPrototype4EMCalTower_Prototype4_DSTReader") + cuts
  //            + TString(".dat"));
}

void EMC_HodoScope_Calibration(TString Cherenkov_Choice)
{
  TString cut_C(Cherenkov_Choice + " > 1000");
  TString cuts = Cherenkov_Choice;

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);

  TH1F *h1_HCalOut = new TH1F("h1_HCalOut",
                              "(CEMC MIP && HCal_{IN} MIP);EMCal + HCal_{IN} + HCal_{OUT} (GeV)", 100,
                              0, abs(beam_momentum_selection) * 2.);

  TH2 *h2_hodo_v_h =
      new TH2F("h2_hodo_v_h",
               "Hodoscope hit distribution;Average Horizontal Hodoscope Idx;Average Vertical Hodoscope Idx",
               8, -.5, 7.5, 8, -.5, 7.5);

  TH2 *h2_hodoE_v_h =
      new TH2F("h2_hodoE_v_h",
               "Event-Averaged Tower-Summed EMCal Energy [A.U.];Average Horizontal Hodoscope Idx;Average Vertical Hodoscope Idx",
               8, -.5, 7.5, 8, -.5, 7.5);
  TH2 *h2_hodoE_v_h_norm =
      new TH2F("h2_hodoE_v_h_norm",
               "Event-Averaged Tower-Summed EMCal Energy [A.U.];Average Horizontal Hodoscope Idx;Average Vertical Hodoscope Idx",
               8, -.5, 7.5, 8, -.5, 7.5);

  TH2 *h2_hodo_c_h =
      new TH2F("h2_hodo_c_h",
               "Column VS Horizontal Hodoscope;Average Horizontal Hodoscope Idx;Average EMCal Column",
               24, -.5, 7.5, 24, -.5, 7.5);
  TH2 *h2_hodo_r_v =
      new TH2F("h2_hodo_r_v",
               "Row VS Vertical Hodoscope;Average Vertical Hodoscope Idx;Average EMCal Row",
               24, -.5, 7.5, 24, -.5, 7.5);

  TH2 *h2_hodoE_h =
      new TH2F("h2_hodoE_h",
               "Total EMCal Energy VS Horizontal Hodoscope;Average Horizontal Hodoscope Idx;Total EMCal Energy [A.U.]",
               8, -.5, 7.5, 80, -.5, 70);
  TH2 *h2_hodoE_v =
      new TH2F("h2_hodoE_v",
               "Total EMCal Energy VS Vertical Hodoscope;Average Vertical Hodoscope Idx;Total EMCal Energy [A.U.]",
               8, -.5, 7.5, 80, -.5, 70);

  TText *t;
  TCanvas *c1 = new TCanvas("EMC_HodoScope_Calibration" + cuts,
                            "EMC_HodoScope_Calibration" + cuts, 1800, 1100);
  c1->Divide(3, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw("Average_HODO_VERTICAL:Average_HODO_HORIZONTAL>>h2_hodo_v_h", cut_C, "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  T->Draw("Average_column:Average_HODO_HORIZONTAL>>h2_hodo_c_h", cut_C, "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw("Energy_Sum_RAW_CEMC:Average_HODO_HORIZONTAL>>h2_hodoE_h", cut_C, "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw("Average_HODO_VERTICAL:Average_HODO_HORIZONTAL>>h2_hodoE_v_h", "(" + cut_C + ")*(Energy_Sum_RAW_CEMC) * (Energy_Sum_RAW_CEMC>0.1)", "goff");
  T->Draw("Average_HODO_VERTICAL:Average_HODO_HORIZONTAL>>h2_hodoE_v_h_norm", "(" + cut_C + " ) && (Energy_Sum_RAW_CEMC>0.1)", "goff");
  h2_hodoE_v_h->Divide(h2_hodoE_v_h_norm);
  h2_hodoE_v_h->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  T->Draw("Average_row:Average_HODO_VERTICAL>>h2_hodo_r_v", cut_C, "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw("Energy_Sum_RAW_CEMC:Average_HODO_VERTICAL>>h2_hodoE_v", cut_C, "colz");

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_DrawPrototype4EMCalTower_") + TString(c1->GetName()), false);
}

void EMCDistribution_HCalCalibration()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);

  TH1F *h1_HCalOut = new TH1F("h1_HCalOut",
                              "(CEMC MIP && HCal_{IN} MIP);EMCal + HCal_{IN} + HCal_{OUT} (GeV)", 100,
                              0, abs(beam_momentum_selection) * 2.);

  TH2 *h2_HCalOut_HCalIn =
      new TH2F("h2_HCalOut_HCalIn",
               "Energy Balance HCal_{OUT} and HCal_{IN} (CEMC MIP && NOT HCal_{IN} MIP);(HCal_{OUT} - HCal_{IN})/HCal Sum;EMCal + HCal_{IN} + HCal_{OUT} (GeV)",
               10, -1, 1, 100, 0, abs(beam_momentum_selection) * 2.);

  TH2 *h2_HCal_EMCal =
      new TH2F("h2_HCal_EMCal",
               "Energy Balance HCal_{SUM} and EMCal (NOT CEMC MIP);(HCal_{SUM} - EMCal)/Sum;EMCal + HCal_{IN} + HCal_{OUT} (GeV)",
               10, -1, 1, 100, 0, abs(beam_momentum_selection) * 2.);

  TText *t;
  TCanvas *c1 = new TCanvas("EMCDistribution_HCalCalibration" + cuts,
                            "EMCDistribution_HCalCalibration" + cuts, 1800, 600);
  c1->Divide(3, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "Energy_Sum_Hadron_CEMC + Energy_Sum_Hadron_HCALIN + Energy_Sum_Hadron_HCALOUT:(Energy_Sum_Hadron_HCALOUT - Energy_Sum_Hadron_HCALIN)/(Energy_Sum_Hadron_HCALIN+Energy_Sum_Hadron_HCALOUT)>>h2_HCalOut_HCalIn",
      "(!HCALIN_MIP) && CEMC_MIP", "goff");
  h2_HCalOut_HCalIn->FitSlicesY();

  TH1 *h2_HCalOut_HCalIn_1 = (TH1 *) gDirectory->GetObjectChecked(
      "h2_HCalOut_HCalIn_1", "TH1D");
  assert(h2_HCalOut_HCalIn_1);
  h2_HCalOut_HCalIn_1->SetMarkerStyle(kFullCircle);

  TF1 *f_HCalOut_HCalIn = new TF1("f_HCalOut_HCalIn",
                                  "[0] * (x+1)/2 + [1]*(1-x)/2", -.8, 0.8);
  f_HCalOut_HCalIn->SetLineColor(kMagenta);
  f_HCalOut_HCalIn->SetLineWidth(3);
  h2_HCalOut_HCalIn_1->Fit(f_HCalOut_HCalIn, "MR0");

  h2_HCalOut_HCalIn->DrawClone("colz");
  h2_HCalOut_HCalIn_1->DrawClone("SAME");
  f_HCalOut_HCalIn->DrawClone("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "Energy_Sum_Hadron_CEMC + Energy_Sum_Hadron_HCALIN + Energy_Sum_Hadron_HCALOUT:(Energy_Sum_Hadron_HCALIN + Energy_Sum_Hadron_HCALOUT - Energy_Sum_Hadron_CEMC)/(Energy_Sum_Hadron_CEMC + Energy_Sum_Hadron_HCALIN + Energy_Sum_Hadron_HCALOUT)>>h2_HCal_EMCal",
      "! CEMC_MIP", "goff");
  h2_HCal_EMCal->FitSlicesY();

  TH1 *h2_HCal_EMCal_1 = (TH1 *) gDirectory->GetObjectChecked(
      "h2_HCal_EMCal_1", "TH1D");
  assert(h2_HCal_EMCal_1);
  h2_HCal_EMCal_1->SetMarkerStyle(kFullCircle);

  TF1 *f_HCal_EMCal = new TF1("f_HCal_EMCal", "[0] * (x+1)/2 + [1]*(1-x)/2",
                              -.8, 0.8);
  f_HCal_EMCal->SetLineColor(kMagenta);
  f_HCal_EMCal->SetLineWidth(3);
  h2_HCal_EMCal_1->Fit(f_HCal_EMCal, "MR0");

  h2_HCal_EMCal->DrawClone("colz");
  h2_HCal_EMCal_1->DrawClone("SAME");
  f_HCal_EMCal->DrawClone("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "Energy_Sum_Hadron_CEMC + Energy_Sum_Hadron_HCALIN + Energy_Sum_Hadron_HCALOUT>>h1_HCalOut",
      "HCALIN_MIP && CEMC_MIP", "goff");

  TH1 *h2_HCal_EMCal_ProjY = (TH1 *) h2_HCal_EMCal->ProjectionY(
                                                      "h2_HCal_EMCal_ProjY", 2, 9)
                                 ->DrawClone();
  TH1 *h2_HCalOut_HCalIn_ProjY = (TH1 *) h2_HCalOut_HCalIn->ProjectionY(
                                                              "h2_HCalOut_HCalIn_ProjY")
                                     ->DrawClone("same");
  h1_HCalOut->Draw("same");

  h2_HCal_EMCal_ProjY->SetTitle(
      "Red: 3 Calo shower, Blue: MIP EMCal, Green: MIP EMCal & HCal_{IN}");

  h2_HCal_EMCal_ProjY->SetLineColor(kRed + 2);
  h2_HCal_EMCal_ProjY->SetLineWidth(3);
  h2_HCalOut_HCalIn_ProjY->SetLineColor(kBlue + 2);
  h2_HCalOut_HCalIn_ProjY->SetLineWidth(2);
  h1_HCalOut->SetLineColor(kGreen + 3);
  h1_HCalOut->SetLineWidth(3);

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_DrawPrototype4EMCalTower_") + TString(c1->GetName()), false);
}

void EMCDistribution_ShowShape(TString CherenkovSignal = "C2_Inner",
                               const double che_cut = 2000)
{
  TString cut_pass = CherenkovSignal + Form(">%.1f", che_cut);
  TString cut_rej = CherenkovSignal + Form("<%.1f", che_cut);

  const double event_pass = T->GetEntries(cut_pass);
  const double event_rej = T->GetEntries(cut_rej);

  TH2 *EnergyDist_pass = new TH2F("EnergyDist_pass",
                                  cut_pass + ";Column;Row;<Energy> / Event / Tower", 8, -.5, 7.5, 8, -.5,
                                  7.5);
  TH2 *EnergyDist_rej = new TH2F("EnergyDist_rej",
                                 cut_rej + ";Column;Row;<Energy> / Event / Tower", 8, -.5, 7.5, 8, -.5,
                                 7.5);
  TH2 *Hodoscope_dist = new TH2F("Hodoscope_dist",
                                 cut_rej + ";7 - Horizontal Hodoscope (5 mm);7 - Vertical Hodoscope (5 mm); Event / finger^2", 8, -.5, 7.5, 8, -.5,
                                 7.5);

  TH1 *Che_full = new TH1F("Che_full",
                           ";" + CherenkovSignal + " Signal (ADC);Count / bin", 200, 0, 17000);
  TH1 *Che_pass = new TH1F("Che_pass",
                           ";" + CherenkovSignal + " Signal (ADC);Count / bin", 200, 0, 17000);
  TH1 *Che_rej = new TH1F("Che_rej",
                          ";" + CherenkovSignal + " Signal (ADC);Count / bin", 200, 0, 17000);

  Che_full->SetLineColor(kBlue + 3);
  Che_full->SetLineWidth(2);
  Che_pass->SetLineColor(kGreen + 3);
  Che_pass->SetLineWidth(2);
  Che_pass->SetFillColor(kGreen + 3);
  Che_pass->SetFillStyle(1);
  Che_rej->SetLineColor(kBlue + 3);
  Che_rej->SetLineWidth(2);
  Che_rej->SetFillColor(kBlue + 3);
  Che_rej->SetFillStyle(1);

  T->Draw(
      "TOWER_CALIB_CEMC[].get_binphi():TOWER_CALIB_CEMC[].get_bineta()>>EnergyDist_pass",
      Form("(%s) * (TOWER_CALIB_CEMC[].get_energy())", cut_pass.Data()),
      "goff");
  T->Draw(
      "TOWER_CALIB_CEMC[].get_binphi():TOWER_CALIB_CEMC[].get_bineta()>>EnergyDist_rej",
      Form("(%s) * (TOWER_CALIB_CEMC[].get_energy())", cut_rej.Data()), "goff");
  T->Draw(
      "7 - Average_HODO_VERTICAL:7 - Average_HODO_HORIZONTAL>>Hodoscope_dist", "1", "goff");

  T->Draw(CherenkovSignal + ">>Che_full", NULL, "goff");
  T->Draw(CherenkovSignal + ">>Che_pass", cut_pass, "goff");
  T->Draw(CherenkovSignal + ">>Che_rej", cut_rej, "goff");

  EnergyDist_pass->Scale(1. / event_pass);
  EnergyDist_rej->Scale(1. / event_rej);

  TText *t;
  TCanvas *c1 = new TCanvas("EMCDistribution_ShowShape" + cuts,
                            "EMCDistribution_ShowShape" + cuts, 1100, 950);
  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  Che_full->DrawClone();
  Che_pass->DrawClone("same");
  Che_rej->DrawClone("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  EnergyDist_pass->DrawClone("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  //  p->SetGridx(0);
  //  p->SetGridy(0);

  EnergyDist_rej->DrawClone("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  //  p->SetGridx(0);
  //  p->SetGridy(0);

  Hodoscope_dist->DrawClone("colz");

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_DrawPrototype4EMCalTower_") + TString(c1->GetName()), false);
}

void EMCDistribution_SUM(TString sTOWER = "Energy_Sum_col1_row2_5x5",
                         TString CherenkovSignal = "C2_Inner")
{
  TH1 *EnergySum_LG_full = new TH1F("EnergySum_LG_full",
                                    ";Full range Tower Energy Sum (GeV);Count / bin", 900, -5, 120);
  TH1 *EnergySum_LG = new TH1F("EnergySum_LG",
                               ";Full range Tower Energy Sum (GeV);Count / bin", 900, -5, 120);
  //  TH1 * EnergySum_HG = new TH1F("EnergySum_HG",
  //      ";Low range Tower Energy Sum (ADC);Count / bin", 50, 0, 500);

  TH1 *C2_Inner_full = new TH1F("C2_Inner_full",
                                CherenkovSignal + ";Cherenkov Signal (ADC);Count / bin", 180, -1000, 17000);
  TH1 *C2_Inner = new TH1F("C2_Inner",
                           CherenkovSignal + ";Cherenkov Inner Signal (ADC);Count / bin", 180, -1000,
                           17000);

  EnergySum_LG_full->SetLineColor(kBlue + 3);
  EnergySum_LG_full->SetLineWidth(2);

  EnergySum_LG->SetLineColor(kGreen + 3);
  EnergySum_LG->SetLineWidth(3);
  EnergySum_LG->SetMarkerColor(kGreen + 3);

  C2_Inner_full->SetLineColor(kBlue + 3);
  C2_Inner_full->SetLineWidth(2);

  C2_Inner->SetLineColor(kGreen + 3);
  C2_Inner->SetLineWidth(3);
  C2_Inner->SetMarkerColor(kGreen + 3);

  TCut c2 = CherenkovSignal + ">2000";

  T->Draw(sTOWER + ">>EnergySum_LG_full", "", "goff");
  T->Draw(sTOWER + ">>EnergySum_LG", c2, "goff");
  T->Draw(CherenkovSignal + ">>C2_Inner_full", "", "goff");
  T->Draw(CherenkovSignal + ">>C2_Inner", c2, "goff");

  TText *t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_SUM_" + sTOWER + "_" + CherenkovSignal + cuts,
      "EMCDistribution_SUM_" + sTOWER + "_" + CherenkovSignal + cuts, 1800,
      600);
  c1->Divide(3, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  C2_Inner_full->DrawClone();
  C2_Inner->DrawClone("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 *h = (TH1 *) EnergySum_LG_full->DrawClone();
  h->GetXaxis()->SetRangeUser(-1, h->GetMean() + 5 * h->GetRMS());
  (TH1 *) EnergySum_LG->DrawClone("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 *h_full = (TH1 *) EnergySum_LG_full->DrawClone();
  //  h_full->GetXaxis()->SetRangeUser(0.5,32);
  TH1 *h = (TH1 *) EnergySum_LG->DrawClone("same");

  TF1 *fgaus_g = new TF1("fgaus_LG_g", "gaus", h->GetMean() - 1 * h->GetRMS(),
                         h->GetMean() + 4 * h->GetRMS());
  fgaus_g->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
                         h->GetMean() + 2 * h->GetRMS());
  h->Fit(fgaus_g, "MR0N");

  TF1 *fgaus = new TF1("fgaus_LG", "gaus",
                       fgaus_g->GetParameter(1) - 1 * fgaus_g->GetParameter(2),
                       fgaus_g->GetParameter(1) + 4 * fgaus_g->GetParameter(2));
  fgaus->SetParameters(fgaus_g->GetParameter(0), fgaus_g->GetParameter(1),
                       fgaus_g->GetParameter(2));
  h->Fit(fgaus, "MR");

  h->Sumw2();
  h_full->Sumw2();
  h_full->GetXaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
                                   h->GetMean() + 4 * h->GetRMS());
  h_full->GetYaxis()->SetRangeUser(0,
                                   fgaus_g->GetParameter(0) * 4);

  h->SetLineWidth(2);
  h->SetMarkerStyle(kFullCircle);

  h_full->SetTitle(
      Form("#DeltaE/<E> = %.1f%%",
           100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  //  p->SetLogy();
  //  p->SetGridx(0);
  //  p->SetGridy(0);
  //
  //  TH1 * h = (TH1 *) EnergySum_LG->DrawClone();
  //  h->GetXaxis()->SetRangeUser(0,500);
  //  h->SetLineWidth(2);
  //  h->SetLineColor(kBlue + 3);
  ////  h->Sumw2();
  //  h->GetXaxis()->SetRangeUser(0, h->GetMean() + 5 * h->GetRMS());
  //
  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  ////  p->SetLogy();
  //  p->SetGridx(0);
  //  p->SetGridy(0);
  //
  //  TH1 * h = (TH1 *) EnergySum_LG->DrawClone();
  //  h->GetXaxis()->SetRangeUser(0,500);
  //
  //  TF1 * fgaus = new TF1("fgaus_HG", "gaus", 0, 100);
  //  fgaus->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
  //      h->GetMean() + 2 * h->GetRMS());
  //  h->Fit(fgaus, "M");
  //
  //  h->Sumw2();
  //  h->GetXaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
  //      h->GetMean() + 4 * h->GetRMS());
  //
  //  h->SetLineWidth(2);
  //  h->SetMarkerStyle(kFullCircle);
  //
  //  h->SetTitle(
  //      Form("#DeltaE/<E> = %.1f%%",
  //          100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_DrawPrototype4EMCalTower_") + TString(c1->GetName()), false);
}

void EMCDistribution_SUM_RawADC(TString sTOWER = "Energy_Sum_col1_row2_5x5",
                                TString CherenkovSignal = "C2_Inner")
{
  TH1 *EnergySum_LG_full = new TH1F("EnergySum_LG_full",
                                    ";Tower Energy Sum (ADC);Count / bin", 210, -1000, 20000);
  TH1 *EnergySum_LG = new TH1F("EnergySum_LG",
                               ";Tower Energy Sum (ADC);Count / bin", 210, -1000, 20000);
  //  TH1 * EnergySum_HG = new TH1F("EnergySum_HG",
  //      ";Low range Tower Energy Sum (ADC);Count / bin", 50, 0, 500);

  TH1 *C2_Inner_full = new TH1F("C2_Inner_full",
                                CherenkovSignal + ";Cherenkov Signal (ADC);Count / bin", 180, -1000, 17000);
  TH1 *C2_Inner = new TH1F("C2_Inner",
                           CherenkovSignal + ";Cherenkov Inner Signal (ADC);Count / bin", 180, -1000,
                           17000);

  EnergySum_LG_full->SetLineColor(kBlue + 3);
  EnergySum_LG_full->SetLineWidth(2);

  EnergySum_LG->SetLineColor(kGreen + 3);
  EnergySum_LG->SetLineWidth(3);
  EnergySum_LG->SetMarkerColor(kGreen + 3);

  C2_Inner_full->SetLineColor(kBlue + 3);
  C2_Inner_full->SetLineWidth(2);

  C2_Inner->SetLineColor(kGreen + 3);
  C2_Inner->SetLineWidth(3);
  C2_Inner->SetMarkerColor(kGreen + 3);

  TCut c2 = CherenkovSignal + ">2000";

  T->Draw(sTOWER + ">>EnergySum_LG_full", "", "goff");
  T->Draw(sTOWER + ">>EnergySum_LG", c2, "goff");
  T->Draw(CherenkovSignal + ">>C2_Inner_full", "", "goff");
  T->Draw(CherenkovSignal + ">>C2_Inner", c2, "goff");

  TText *t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_SUM_RawADC_" + sTOWER + "_" + CherenkovSignal + cuts,
      "EMCDistribution_SUM_RawADC_" + sTOWER + "_" + CherenkovSignal + cuts, 1800,
      600);
  c1->Divide(3, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  C2_Inner_full->DrawClone();
  C2_Inner->DrawClone("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 *h = (TH1 *) EnergySum_LG_full->DrawClone();
  //  h->GetXaxis()->SetRangeUser(-1, h->GetMean() + 5 * h->GetRMS());
  (TH1 *) EnergySum_LG->DrawClone("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 *h_full = (TH1 *) EnergySum_LG_full->DrawClone();
  //  h_full->GetXaxis()->SetRangeUser(0.5,32);
  TH1 *h = (TH1 *) EnergySum_LG->DrawClone("same");

  TF1 *fgaus_g = new TF1("fgaus_LG_g", "gaus", h->GetMean() - 1 * h->GetRMS(),
                         h->GetMean() + 4 * h->GetRMS());
  fgaus_g->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
                         h->GetMean() + 2 * h->GetRMS());
  h->Fit(fgaus_g, "MR0N");

  TF1 *fgaus = new TF1("fgaus_LG", "gaus",
                       fgaus_g->GetParameter(1) - 1 * fgaus_g->GetParameter(2),
                       fgaus_g->GetParameter(1) + 4 * fgaus_g->GetParameter(2));
  fgaus->SetParameters(fgaus_g->GetParameter(0), fgaus_g->GetParameter(1),
                       fgaus_g->GetParameter(2));
  h->Fit(fgaus, "MR");

  h->Sumw2();
  h_full->Sumw2();
  h_full->GetXaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
                                   h->GetMean() + 4 * h->GetRMS());
  h_full->GetYaxis()->SetRangeUser(0,
                                   fgaus_g->GetParameter(0) * 4);

  h->SetLineWidth(2);
  h->SetMarkerStyle(kFullCircle);

  h_full->SetTitle(
      Form("#DeltaE/<E> = %.1f%%",
           100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  //  p->SetLogy();
  //  p->SetGridx(0);
  //  p->SetGridy(0);
  //
  //  TH1 * h = (TH1 *) EnergySum_LG->DrawClone();
  //  h->GetXaxis()->SetRangeUser(0,500);
  //  h->SetLineWidth(2);
  //  h->SetLineColor(kBlue + 3);
  ////  h->Sumw2();
  //  h->GetXaxis()->SetRangeUser(0, h->GetMean() + 5 * h->GetRMS());
  //
  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  ////  p->SetLogy();
  //  p->SetGridx(0);
  //  p->SetGridy(0);
  //
  //  TH1 * h = (TH1 *) EnergySum_LG->DrawClone();
  //  h->GetXaxis()->SetRangeUser(0,500);
  //
  //  TF1 * fgaus = new TF1("fgaus_HG", "gaus", 0, 100);
  //  fgaus->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
  //      h->GetMean() + 2 * h->GetRMS());
  //  h->Fit(fgaus, "M");
  //
  //  h->Sumw2();
  //  h->GetXaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
  //      h->GetMean() + 4 * h->GetRMS());
  //
  //  h->SetLineWidth(2);
  //  h->SetMarkerStyle(kFullCircle);
  //
  //  h->SetTitle(
  //      Form("#DeltaE/<E> = %.1f%%",
  //          100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_DrawPrototype4EMCalTower_") + TString(c1->GetName()), false);
}

void EMCDistribution_Fast(TString gain = "CALIB", bool full_gain = false)
{
  TString hname = "EMCDistribution_" + gain + TString(full_gain ? "_FullGain" : "") + cuts;

  TH2 *h2 = NULL;
  if (gain.Contains("CALIB"))
  {
    if (full_gain)
    {
      h2 = new TH2F(hname,
                    Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 100, -1,
                    30, 64, -.5, 63.5);
      //      QAHistManagerDef::useLogBins(h2->GetXaxis());
    }
    else
    {
      h2 = new TH2F(hname,
                    Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 260, -.2,
                    5, 64, -.5, 63.5);
    }
    T->Draw(
        "TOWER_" + gain + "_CEMC[].get_bineta() + 8* TOWER_" + gain + "_CEMC[].get_binphi():TOWER_" + gain + "_CEMC[].get_energy()>>" + hname, "", "goff");
  }
  else if (gain.Contains("RAW"))
  {
    if (full_gain)
    {
      h2 = new TH2F(hname,
                    Form(";Tower Peak Amp (ADC);Count / bin"), 1 << 8,
                    -3000, 17000, 64, -.5, 63.5);
      //      QAHistManagerDef::useLogBins(h2->GetXaxis());
    }
    else
    {
      h2 = new TH2F(hname,
                    Form(";Tower Peak Amp (ADC);Count / bin"), 1 << 8,
                    0, 1 << 14, 64, -.5, 63.5);
    }
    T->Draw(
        "TOWER_" + gain + "_CEMC[].get_bineta() + 8* TOWER_" + gain + "_CEMC[].get_binphi():TOWER_" + gain + "_CEMC[].get_energy()>>" + hname, "", "goff");
  }

  TText *t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_" + gain + TString(full_gain ? "_FullGain" : "") + cuts,
      "EMCDistribution_" + gain + TString(full_gain ? "_FullGain" : "") + cuts,
      1800, 950);
  c1->Divide(8, 8, 0., 0.01);
  int idx = 1;
  TPad *p;

  for (int iphi = 8 - 1; iphi >= 0; iphi--)
  {
    for (int ieta = 0; ieta < 8; ieta++)
    {
      p = (TPad *) c1->cd(idx++);
      c1->Update();

      p->SetLogy();
      //      if (full_gain)
      //      {
      //        p->SetLogx();
      //      }
      p->SetGridx(0);
      p->SetGridy(0);

      TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi) + TString(full_gain ? "_FullGain" : "");

      TH1 *h = h2->ProjectionX(hname, ieta + 8 * iphi + 1,
                               ieta + 8 * iphi + 1);  // axis bin number is encoded as ieta+8*iphi+1

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

      TF1 *fit = ((TF1 *) (h->GetListOfFunctions()->At(0)));
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
             TString(_file0->GetName()) + TString("_DrawPrototype4EMCalTower_") + TString(c1->GetName()), false);
}

void EMCDistribution_PeakSample_Fast(bool full_gain = false)
{
  const TString gain = "RAW";

  TString hname = "EMCDistribution_" + gain + TString(full_gain ? "_FullGain" : "") + cuts;

  TH2 *h2 = NULL;
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
        "TOWER_" + gain + "_CEMC[].get_bineta() + 8* TOWER_" + gain + "_CEMC[].get_binphi():(TOWER_RAW_CEMC[].signal_samples[10] - TOWER_RAW_CEMC[].signal_samples[0])*(-1)>>" + hname, "", "goff");
  }

  TText *t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_PeakSample_Fast_" + TString(full_gain ? "_FullGain" : "") + cuts,
      "EMCDistribution_PeakSample_Fast_" + TString(full_gain ? "_FullGain" : "") + cuts, 1800, 950);
  c1->Divide(8, 8, 0., 0.01);
  int idx = 1;
  TPad *p;

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

      TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi) + TString(full_gain ? "_FullGain" : "");

      TH1 *h = h2->ProjectionX(hname, ieta + 8 * iphi + 1,
                               ieta + 8 * iphi + 1);  // axis bin number is encoded as ieta+8*iphi+1

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

      TF1 *fit = ((TF1 *) (h->GetListOfFunctions()->At(0)));
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
             TString(_file0->GetName()) + TString("_DrawPrototype4EMCalTower_") + TString(c1->GetName()), false);
}

void EMCDistribution(TString gain = "CALIB", bool log_scale = false)
{
  TText *t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_" + gain + TString(log_scale ? "_Log" : "") + cuts,
      "EMCDistribution_" + gain + TString(log_scale ? "_Log" : "") + cuts, 1800,
      1000);
  c1->Divide(8, 8, 0., 0.01);
  int idx = 1;
  TPad *p;

  for (int iphi = 8 - 1; iphi >= 0; iphi--)
  {
    for (int ieta = 0; ieta < 8; ieta++)
    {
      p = (TPad *) c1->cd(idx++);
      c1->Update();

      p->SetLogy();
      p->SetGridx(0);
      p->SetGridy(0);

      TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi) + TString(log_scale ? "_Log" : "");

      TH1 *h = NULL;

      if (log_scale)
        h = new TH1F(hname,
                     Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 300,
                     5e-3, 3096);
      else
        //            h = new TH1F(hname,
        //                Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 196,
        //                1900, 2096);
        h = new TH1F(hname,
                     Form(";Calibrated Tower Energy Sum (GeV);Count / bin"), 596,
                     -96, 500);

      h->SetLineWidth(0);
      h->SetLineColor(kBlue + 3);
      h->SetFillColor(kBlue + 3);
      h->GetXaxis()->SetTitleSize(.09);
      h->GetXaxis()->SetLabelSize(.08);
      h->GetYaxis()->SetLabelSize(.08);

      if (log_scale)
        QAHistManagerDef::useLogBins(h->GetXaxis());

      T->Draw(
          "TOWER_" + gain + "_CEMC[].get_energy_power_law_exp()>>" + hname,
          Form(
              "TOWER_%s_CEMC[].get_bineta()==%d && TOWER_%s_CEMC[].get_binphi()==%d",
              gain.Data(), ieta, gain.Data(), iphi),
          "");

      TText *t = new TText(.9, .9, Form("Col%d Row%d", ieta, iphi));
      t->SetTextAlign(33);
      t->SetTextSize(.15);
      t->SetNDC();
      t->Draw();

      //          return;
    }
  }

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_DrawPrototype4EMCalTower_") + TString(c1->GetName()), false);
}

void EMCDistribution_ADC(bool log_scale = true)
{
  TString gain = "RAW";

  TText *t;
  TCanvas *c1 = new TCanvas(
      "EMCDistribution_ADC_" + gain + TString(log_scale ? "_Log" : "") + cuts,
      "EMCDistribution_ADC_" + gain + TString(log_scale ? "_Log" : "") + cuts,
      1800, 1000);
  c1->Divide(8, 8, 0., 0.01);
  int idx = 1;
  TPad *p;

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

      TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi) + TString(log_scale ? "_Log" : "");

      TH1 *h = NULL;

      if (log_scale)
        h = new TH2F(hname,
                     Form(";Sample ID;ADC"), 31, -.5,
                     30.5,
                     //                128+64, 0, 3096);
                     1 << 10, 0, 1 << 14);
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
      //          h->GetYaxis()->SetRangeUser(2000,3000);

      //          if (log_scale)
      //            QAHistManagerDef::useLogBins(h->GetYaxis());

      TString sdraw = "TOWER_" + gain + "_CEMC[].signal_samples[]:fmod(Iteration$,31)>>" + hname;
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
             TString(_file0->GetName()) + TString("_DrawPrototype4EMCalTower_") + TString(c1->GetName()), false);
}

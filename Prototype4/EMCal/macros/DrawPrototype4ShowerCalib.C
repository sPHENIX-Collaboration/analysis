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

//#include "Prototype3_DSTReader.h"

TCut event_sel;
TString cuts;
TFile *_file0 = NULL;
TTree *T = NULL;

class lin_res
{
 public:
  TString name;
  TGraphErrors *linearity;
  TGraphErrors *resolution;
  TF1 *f_res;
};

void DrawPrototype4ShowerCalib(  //
    const TString infile =
        //        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/ShowerCalib/dst.lst_EMCalCalib.root"  //
    //        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/ShowerCalib_tilted/dst.lst_EMCalCalib.root"//
    //    "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan1Block36/dst.lst_EMCalCalib.root"  //
    //        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan2Block34/dst.lst_EMCalCalib.root"  //
    //    "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan2Block18/dst.lst_EMCalCalib.root"  //
    //        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan64.28V/dst.lst_EMCalCalib.root"  //
    //        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan4Block45/dst.lst_EMCalCalib.root"  //
    "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan2018b1Tower36/dst.lst_EMCalCalib.root"  //
    )
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libPrototype4.so");
  gSystem->Load("libProto4ShowCalib.so");

  //  gROOT->LoadMacro("Prototype3_DSTReader.C+");

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

  //  event_sel = "1";
  //  cuts = "_all_data";
//      event_sel = "good_e";
//      cuts = "_good_e";

  //    event_sel = "info.beam_mom == -8 && good_e";
  //    cuts = "_8GeV_good_e";
//      event_sel = "info.beam_mom == -6 && good_e";
//      cuts = "_6GeV_good_e";
  //  event_sel = " good_e && info.run == 409";
  //  cuts = "_good_e_run409";
  //      event_sel = "info.beam_mom == -12 && good_e";
  //      cuts = "_12GeV_good_e";
  //      event_sel = "info.beam_mom == -16 && good_e";
  //      cuts = "_16GeV_good_e";
  //  event_sel = "info.beam_mom == -6";
  //  cuts = "_Neg6GeV";

  //    event_sel = "good_e  && info.hodo_h==3 && info.hodo_v==5";  // Tower 36
  //    cuts = "_good_e_h3_v5";
  //    event_sel = "good_e && info.hodo_h>=2 && info.hodo_h<=4 && info.hodo_v>=4 && info.hodo_v<=6";  // Tower 36
  //    cuts = "_good_e_h234_v456";
  //  event_sel = "info.beam_mom == -8 && valid_hodo_v && valid_hodo_h&& trigger_veto_pass && info.hodo_h==3 && info.hodo_v==5";  // Tower 36
  //  cuts = "_valid_data_h3_v5_8GeV";  // Tower 36
  //    event_sel = "info.beam_mom == -2 && valid_hodo_v && valid_hodo_h&& trigger_veto_pass && info.hodo_h>=2 && info.hodo_h<=4 && info.hodo_v>=4 && info.hodo_v<=6";  // Tower 36
  //    cuts = "_valid_data_h234_v456_2GeV";  // Tower 36

  //  event_sel = "good_e  && info.hodo_h==3 && info.hodo_v==4";  // Tower 34/18
  //  cuts = "_good_e_h3_v4";
  //  event_sel = "good_e && info.hodo_h>=2 && info.hodo_h<=4 && info.hodo_v>=3 && info.hodo_v<=5";  // Tower 34/18
  //  cuts = "_good_e_h234_v345";
  //  event_sel = "good_e && info.hodo_h>=1 && info.hodo_h<=5 && info.hodo_v>=2 && info.hodo_v<=6";  // Tower 34/18
  //  cuts = "_good_e_h12345_v23456";

  event_sel = "good_e  && info.hodo_h==3 && info.hodo_v==3";  // Tower 34/18 - 2018b
  cuts = "_good_e_h3_v3";
  //  event_sel = "good_e && info.hodo_h>=3 && info.hodo_h<=4 && info.hodo_v>=2 && info.hodo_v<=4";  // Tower 34/18
  //  cuts = "_good_e_h34_v234";
  //  event_sel = "good_e && info.hodo_h>=1 && info.hodo_h<=5 && info.hodo_v>=2 && info.hodo_v<=6";  // Tower 34/18
  //  cuts = "_good_e_h12345_v23456";

  //  event_sel = "good_e  && info.hodo_h==5 && info.hodo_v==2";  // Tower 34/18 between towers
  //  cuts = "_good_e_h5_v2";

  T->SetAlias("SimEnergyScale", "1*1");
  //    // based on /phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/ShowerCalib/UIUC21.lst_EMCalCalib.root_DrawPrototype3ShowerCalib_LineShapeData_Neg8GeV_good_data_h5_v3.svg
  //    T->SetAlias("SimEnergyScale","8.74635e+00/7.60551");
  //  // based on /phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/ShowerCalib/Tilt0.lst_EMCalCalib.root_DrawPrototype3ShowerCalib_LineShapeData_Neg8GeV_quality_h3_v5_col2_row2.root_DrawPrototype3ShowerCalib_SumLineShapeCompare_Electron_8GeV_QGSP_BERT_HP.root
  //    T->SetAlias("SimEnergyScale","8.88178/8.16125e+00");
  // Tilt0.lst <-> QGSP_BERT_HP Birk 0.151
  //    T->SetAlias("SimEnergyScale","8.88178/8.01845");
  // Tilt0.lst <-> QGSP_BERT_HP Birk 0.18
  //    T->SetAlias("SimEnergyScale","8.88178/7.94838");

  //  Dummy
  //    T->SetAlias("SimEnergyScale","1*1");

  cout << "Build event selection of " << (const char *) event_sel << endl;

  T->Draw(">>EventList", event_sel);
  TEventList *elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected"
       << endl;

  T->SetEventList(elist);

  //  // data stuff
  PositionDependenceData("clus_5x5_prod.sum_E");
  //  PositionDependenceData("clus_5x5_recalib.sum_E");
  HodoscopeCheck();

  //    LineShapeData("abs(info.C2_sum)<200", "(info.C2_sum)>2000");

  Get_Res_linear_Summmary("sum_E*.55", 30);
  //  Get_Res_linear_Summmary("sum_E*.13", 10);

  // simulation stuff
  //  SimPositionCheck(-0); // 0 degree tilted
  //  LineShapeSim();

  //  PositionDependenceSim("clus_5x5_prod.sum_E", -0, 5); // 0 degree tilted
  //  SimPositionCheck(-15); // 10 degree tilted
  //  PositionDependenceSim("clus_5x5_prod.sum_E", -15, 5); // 10 degree tilted
  //    SimPositionCheck(-40+3); // 45 degree tilted
  //  Get_Res_linear_Summmary_Sim();
}

void PositionDependenceData(TString sTOWER = "clus_5x5_prod.sum_E",
                            const double z_shift = 0, const int n_div = 1)
{
  TH3F *EnergySum_LG3 =
      new TH3F("EnergySum_LG3",
               ";Horizontal Hodoscope (5 mm);Vertical Hodoscope (5 mm);5x5 Cluster Energy (GeV)",  //
               8, -.5, 7.5,                                                                        //
               8, -.5, 7.5,                                                                        //
               200, 1, 20);

  T->Draw(sTOWER + ":7-hodo_v:hodo_h>>EnergySum_LG3", "", "goff");

  TProfile2D *EnergySum_LG3_prof_xy = EnergySum_LG3->Project3DProfile("yx");
  TH2 *EnergySum_LG3_yx = EnergySum_LG3->Project3D("yx");
  TH2 *EnergySum_LG3_zx = EnergySum_LG3->Project3D("zx");
  TH2 *EnergySum_LG3_zy = EnergySum_LG3->Project3D("zy");

  TGraphErrors *ge_EnergySum_LG3_zx = FitProfile(EnergySum_LG3_zx);
  TGraphErrors *ge_EnergySum_LG3_zy = FitProfile(EnergySum_LG3_zy);

  TText *t;
  TCanvas *c1 = new TCanvas(
      TString(Form("EMCDistributionVSBeam_SUM_NDiv%d_", n_div)) + sTOWER + cuts,
      TString(Form("EMCDistributionVSBeam_SUM_NDiv%d_", n_div)) + sTOWER + cuts,
      1000, 960);
  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_prof_xy->SetMinimum(0);

  EnergySum_LG3_prof_xy->Draw("colz");
  EnergySum_LG3_prof_xy->SetTitle(
      "Energy response;Horizontal Hodoscope (5 mm);7 - Vertical Hodoscope (5 mm)");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_yx->SetMinimum(0);
  EnergySum_LG3_yx->Draw("colz");
  EnergySum_LG3_yx->SetTitle(
      "Event counts;Horizontal Hodoscope (5 mm);7 - Vertical Hodoscope (5 mm)");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_zx->Draw("colz");
  EnergySum_LG3_zx->SetTitle(
      "Position scan;Horizontal Hodoscope (5 mm);5x5 Cluster Energy (GeV)");

  ge_EnergySum_LG3_zx->SetLineWidth(2);
  ge_EnergySum_LG3_zx->SetMarkerStyle(kFullCircle);
  ge_EnergySum_LG3_zx->Draw("pe");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_zy->Draw("colz");
  EnergySum_LG3_zy->SetTitle(
      "Position scan;7 - Vertical Hodoscope (5 mm);5x5 Cluster Energy (GeV)");

  ge_EnergySum_LG3_zy->SetLineWidth(2);
  ge_EnergySum_LG3_zy->SetMarkerStyle(kFullCircle);
  ge_EnergySum_LG3_zy->Draw("pe");

  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  ////  p->SetLogy();
  //  p->SetGridx(0);
  //  p->SetGridy(0);
  //
  //  TH1 * h = (TH1 *) EnergySum_LG3->ProjectionZ();
  //
  //  TF1 * fgaus = new TF1("fgaus_LG", "gaus", 0, 100);
  //  fgaus->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
  //      h->GetMean() + 2 * h->GetRMS());
  //  h->Fit(fgaus, "M");
  //
  //  h->Sumw2();
  //  h->GetXaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
  //      h->GetMean() + 4 * h->GetRMS());
  //  EnergySum_LG3_zx->GetYaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
  //      h->GetMean() + 4 * h->GetRMS());
  //  EnergySum_LG3_zy->GetYaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
  //      h->GetMean() + 4 * h->GetRMS());
  //
  //  h->SetLineWidth(2);
  //  h->SetMarkerStyle(kFullCircle);
  //
  //  h->SetTitle(
  //      Form("#DeltaE/<E> = %.1f%%",
  //          100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_DrawPrototype3ShowerCalib_") + TString(c1->GetName()), kTRUE);
}

void PositionDependenceSim(TString sTOWER = "clus_5x5_prod.sum_E",
                           const double z_shift = 0, const int n_div = 1)
{
  TH3F *EnergySum_LG3 =
      new TH3F("EnergySum_LG3",
               ";Beam Horizontal Pos (cm);Beam Vertical Pos (cm);5x5 Cluster Energy (GeV)",  //
               20 * n_div, z_shift - 5, z_shift + 5,                                         //
               20 * n_div, -5, 5,                                                            //
               200, 1, 20);

  T->Draw(sTOWER + ":info.truth_y:info.truth_z>>EnergySum_LG3", "", "goff");

  TProfile2D *EnergySum_LG3_xy = EnergySum_LG3->Project3DProfile("yx");
  TH2 *EnergySum_LG3_zx = EnergySum_LG3->Project3D("zx");
  TH2 *EnergySum_LG3_zy = EnergySum_LG3->Project3D("zy");

  TGraphErrors *ge_EnergySum_LG3_zx = FitProfile(EnergySum_LG3_zx);
  TGraphErrors *ge_EnergySum_LG3_zy = FitProfile(EnergySum_LG3_zy);

  TText *t;
  TCanvas *c1 = new TCanvas(
      TString(Form("EMCDistributionVSBeam_SUM_NDiv%d_", n_div)) + sTOWER + cuts,
      TString(Form("EMCDistributionVSBeam_SUM_NDiv%d_", n_div)) + sTOWER + cuts,
      1000, 960);
  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_xy->Draw("colz");
  EnergySum_LG3_xy->SetTitle(
      "Position scan;Beam Horizontal Pos (cm);Beam Vertical Pos (cm)");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_zx->Draw("colz");
  EnergySum_LG3_zx->SetTitle(
      "Position scan;Beam Horizontal Pos (cm);5x5 Cluster Energy (GeV)");

  ge_EnergySum_LG3_zx->SetLineWidth(2);
  ge_EnergySum_LG3_zx->SetMarkerStyle(kFullCircle);
  ge_EnergySum_LG3_zx->Draw("pe");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  EnergySum_LG3_zy->Draw("colz");
  EnergySum_LG3_zy->SetTitle(
      "Position scan;Beam Vertical Pos (cm);5x5 Cluster Energy (GeV)");

  ge_EnergySum_LG3_zy->SetLineWidth(2);
  ge_EnergySum_LG3_zy->SetMarkerStyle(kFullCircle);
  ge_EnergySum_LG3_zy->Draw("pe");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TH1 *h = (TH1 *) EnergySum_LG3->ProjectionZ();

  TF1 *fgaus = new TF1("fgaus_LG", "gaus", 0, 100);
  fgaus->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
                       h->GetMean() + 2 * h->GetRMS());
  h->Fit(fgaus, "M");

  h->Sumw2();
  h->GetXaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
                              h->GetMean() + 4 * h->GetRMS());
  EnergySum_LG3_zx->GetYaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
                                             h->GetMean() + 4 * h->GetRMS());
  EnergySum_LG3_zy->GetYaxis()->SetRangeUser(h->GetMean() - 4 * h->GetRMS(),
                                             h->GetMean() + 4 * h->GetRMS());

  h->SetLineWidth(2);
  h->SetMarkerStyle(kFullCircle);

  h->SetTitle(
      Form("#DeltaE/<E> = %.1f%%",
           100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_DrawPrototype3ShowerCalib_") + TString(c1->GetName()), kTRUE);
}

void LineShapeData(TCut c2_h = "abs(info.C2_sum)<100", TCut c2_e =
                                                           "(info.C2_sum)>500 && (info.C2_sum)<1300")
{
  vector<double> mom;

  TText *t;
  TCanvas *c1 = new TCanvas("LineShapeData" + cuts, "LineShapeData" + cuts,
                            1800, 950);

  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetGridx(1);
  //  p->SetGridy(1);
  p->SetLogy();

  T->Draw("info.C2_sum>>h_c2_sum(300,-500,22500)");
  h_c2_sum->SetTitle("Cherenkov Checks;Sum C2 (ADC)");
  T->Draw("info.C2_sum>>h_c2_h(300,-500,22500)", c2_h, "same");
  T->Draw("info.C2_sum>>h_c2_e(300,-500,22500)", c2_e, "same");

  h_c2_h->SetLineColor(kBlue);
  h_c2_e->SetLineColor(kRed);

  h_c2_sum->SetLineWidth(2);
  h_c2_h->SetLineWidth(2);
  h_c2_e->SetLineWidth(2);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("clus_5x5_prod.sum_E>>h_5x5sum_c2_sum(170,-1,16)");
  h_5x5sum_c2_sum->SetTitle(
      "Cluster spectrum decomposition;5x5 cluster energy (GeV)");
  T->Draw("clus_5x5_prod.sum_E>>h_5x5sum_c2_h(170,-1,16)", c2_h, "same");
  T->Draw("clus_5x5_prod.sum_E>>h_5x5sum_c2_rej_h(170,-1,16)", !c2_h,
          "same");
  T->Draw("clus_5x5_prod.sum_E>>h_5x5sum_c2_e(170,-1,16)", c2_e, "same");

  h_5x5sum_c2_h->SetLineColor(kBlue);
  h_5x5sum_c2_rej_h->SetLineColor(kMagenta);
  h_5x5sum_c2_e->SetLineColor(kRed);

  h_5x5sum_c2_sum->SetLineWidth(2);
  h_5x5sum_c2_h->SetLineWidth(2);
  h_5x5sum_c2_rej_h->SetLineWidth(2);
  h_5x5sum_c2_e->SetLineWidth(2);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 *h_5x5sum_c2_h2 = h_5x5sum_c2_h->DrawClone();
  h_5x5sum_c2_h2->Sumw2();
  h_5x5sum_c2_h2->SetMarkerColor(kBlue);
  h_5x5sum_c2_h2->SetMarkerStyle(kFullCircle);
  h_5x5sum_c2_h2->SetTitle(";5x5 cluster energy (GeV)");

  TH1 *h_5x5sum_c2_e2 = h_5x5sum_c2_e->DrawClone("same");
  h_5x5sum_c2_e2->Sumw2();
  h_5x5sum_c2_e2->SetMarkerColor(kRed);
  h_5x5sum_c2_e2->SetMarkerStyle(kFullCircle);
  h_5x5sum_c2_e2->SetTitle(";5x5 cluster energy (GeV)");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1F *h_5x5sum_c2_h3 = h_5x5sum_c2_h->DrawClone();
  h_5x5sum_c2_h3->SetName("h_5x5sum_c2_h3");
  h_5x5sum_c2_h3->Sumw2();
  h_5x5sum_c2_h3->Scale(1. / h_5x5sum_c2_h3->GetSum());
  h_5x5sum_c2_h3->SetMarkerColor(kBlue);
  h_5x5sum_c2_h3->SetMarkerStyle(kFullCircle);
  h_5x5sum_c2_h3->SetTitle(";5x5 cluster energy (GeV);Probability / bin");

  SaveCanvas(c1,
             TString(_file0->GetName()) + "_DrawPrototype3ShowerCalib_" + TString(c1->GetName()), kTRUE);
}

void LineShapeSim()
{
  vector<double> mom;

  TText *t;
  TCanvas *c1 = new TCanvas("LineShapeSim" + cuts, "LineShapeSim" + cuts, 1000,
                            650);

  //  c1->Divide(2,2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("clus_5x5_prod.sum_E*SimEnergyScale>>h_5x5sum_c2_sum(170,-1,16)");
  h_5x5sum_c2_sum->SetTitle(
      "Cluster spectrum decomposition;5x5 cluster energy (GeV)");
  h_5x5sum_c2_sum->SetLineWidth(2);

  SaveCanvas(c1,
             TString(_file0->GetName()) + "_DrawPrototype3ShowerCalib_" + TString(c1->GetName()), kTRUE);
}

void HodoscopeCheck()
{
  vector<double> mom;

  TText *t;
  TCanvas *c1 = new TCanvas("HodoscopeCheck" + cuts, "HodoscopeCheck" + cuts,
                            1300, 950);

  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetGridx(1);
  p->SetGridy(1);
  p->SetLogz();

  T->Draw("clus_5x5_prod.average_col:hodo_h>>h2_h(8,-.5,7.5,160,-.5,7.5)",
          "good_e", "colz");
  h2_h->SetTitle(
      "Horizontal hodoscope check;Horizontal Hodoscope;5x5 cluster mean col");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetGridx(1);
  p->SetGridy(1);
  p->SetLogz();

  T->Draw("clus_5x5_prod.average_row:hodo_v>>h2_v(8,-.5,7.5,160,-.5,7.5)",
          "good_e", "colz");
  h2_v->SetTitle(
      "Vertical hodoscope check;Vertical Hodoscope;5x5 cluster mean row");

  SaveCanvas(c1,
             TString(_file0->GetName()) + "_DrawPrototype3ShowerCalib_" + TString(c1->GetName()), kTRUE);

  return mom;
}

void SimPositionCheck(const double shift_z = 0)
{
  vector<double> mom;

  TText *t;
  TCanvas *c1 = new TCanvas("SimPositionCheck" + cuts,
                            "SimPositionCheck" + cuts, 1300, 950);

  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetGridx(1);
  p->SetGridy(1);
  p->SetLogz();

  T->Draw(
      Form("clus_5x5_prod.average_col:truth_z>>h2_h(30,%f,%f,160,-.5,7.5)",
           shift_z - 1.5, shift_z + 1.5),
      "1", "colz");
  h2_h->SetTitle(
      "Horizontal hodoscope check;Horizontal beam pos;5x5 cluster mean col");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetGridx(1);
  p->SetGridy(1);
  p->SetLogz();

  T->Draw("clus_5x5_prod.average_row:truth_y>>h2_v(30,-1.5,1.5,160,-.5,7.5)",
          "1", "colz");
  h2_v->SetTitle(
      "Vertical hodoscope check;Vertical beam pos;5x5 cluster mean row");

  SaveCanvas(c1,
             TString(_file0->GetName()) + "_DrawPrototype3ShowerCalib_" + TString(c1->GetName()), kTRUE);

  return;
}

void Get_Res_linear_Summmary(TString e_sum = "sum_E", const double max_E = 32)
{
  vector<double> beam_mom(GetBeamMom());

  //  return;

  lin_res ges_clus_5x5_prod = GetResolution("clus_5x5_prod", beam_mom, kMagenta + 2, e_sum);
  lin_res ges_clus_3x3_prod = GetResolution("clus_3x3_prod", beam_mom,
                                            kBlue + 3, e_sum);
  lin_res ges_clus_1x1_prod = GetResolution("clus_1x1_prod", beam_mom,
                                            kCyan + 3, e_sum);
  lin_res ges_clus_5x5_recalib = GetResolution("clus_5x5_recalib", beam_mom,
                                               kRed + 3, e_sum);

  TCanvas *c1 = new TCanvas(Form("Res_linear") + cuts,
                            Form("Res_linear") + cuts, 1300, 600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  TLegend *leg = new TLegend(.15, .7, .6, .85);

  p->DrawFrame(0, 0, max_E, max_E,
               Form("Electron Linearity;Input energy (GeV);Measured Energy (GeV)"));

  TF1 *f_calo_l_sim = new TF1("f_calo_l", "pol2", 0.5, max_E);
  //  f_calo_l_sim->SetParameters(-0.03389, 0.9666, -0.0002822);
  f_calo_l_sim->SetParameters(-0., 1, -0.);
  //  f_calo_l_sim->SetLineWidth(1);
  f_calo_l_sim->SetLineColor(kGreen + 2);
  f_calo_l_sim->SetLineWidth(3);

  f_calo_l_sim->Draw("same");
  ges_clus_5x5_prod.linearity->Draw("p");
  ges_clus_3x3_prod.linearity->Draw("p");
  ges_clus_1x1_prod.linearity->Draw("p");
  ges_clus_5x5_recalib.linearity->Draw("p");
  //  ge_linear->Fit(f_calo_l, "RM0");
  //  f_calo_l->Draw("same");

  leg->AddEntry(ges_clus_5x5_prod.linearity, ges_clus_5x5_prod.name, "ep");
  leg->AddEntry(ges_clus_3x3_prod.linearity, ges_clus_3x3_prod.name, "ep");
  leg->AddEntry(ges_clus_1x1_prod.linearity, ges_clus_1x1_prod.name, "ep");
  leg->AddEntry(ges_clus_5x5_recalib.linearity, "clus_5x5_recalib", "ep");
  leg->AddEntry(f_calo_l_sim, "Unity", "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TF1 *f_calo_sim = new TF1("f_calo_sim", "sqrt([0]*[0]+[1]*[1]/x)/100", 0.5,
                            30);
  //  TF1 *f_calo_sim = new TF1("f_calo_sim", "sqrt([0]*[0]+[1]*[1]/x+[2]*[2]/x/x)/100", 0.5,
  //                            30);
  f_calo_sim->SetParameters(3.7, 12.8, 0);
  f_calo_sim->SetLineWidth(3);
  f_calo_sim->SetLineColor(kGreen + 2);

  TH1 *hframe = p->DrawFrame(0, 0, max_E, 0.3,
                             Form("Resolution;Input energy (GeV);#DeltaE/<E>"));

  TLegend *leg = new TLegend(.2, .6, .85, .9);

  ges_clus_5x5_prod.f_res->Draw("same");
  ges_clus_5x5_prod.resolution->Draw("ep");
  //  ges_clus_3x3_prod.f_res->Draw("same");
  //  ges_clus_3x3_prod.resolution->Draw("ep");
  //  ges_clus_1x1_prod.f_res->Draw("same");
  //  ges_clus_1x1_prod.resolution->Draw("ep");
  ges_clus_5x5_recalib.f_res->Draw("same");
  ges_clus_5x5_recalib.resolution->Draw("ep");
  f_calo_sim->Draw("same");

  leg->AddEntry(ges_clus_5x5_prod.resolution, ges_clus_5x5_prod.name, "ep");
  leg->AddEntry(ges_clus_5x5_prod.f_res,
                Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
                     //          Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E} ",
                     ges_clus_5x5_prod.f_res->GetParameter(0),
                     ges_clus_5x5_prod.f_res->GetParameter(1)
                     //                     ges_clus_5x5_prod.f_res->GetParameter(2)
                     ),
                "l");

  leg->AddEntry(ges_clus_3x3_prod.resolution, ges_clus_3x3_prod.name, "ep");
  leg->AddEntry(ges_clus_3x3_prod.f_res,
                Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
                     //            Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E} #oplus %.1f%%/E",
                     ges_clus_3x3_prod.f_res->GetParameter(0),
                     ges_clus_3x3_prod.f_res->GetParameter(1)
                     //                       ges_clus_3x3_prod.f_res->GetParameter(2)
                     ),
                "l");
  //
  //  leg->AddEntry(ges_clus_1x1_prod.resolution, ges_clus_1x1_prod.name, "ep");
  //  leg->AddEntry(ges_clus_1x1_prod.f_res,
  //                Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E} #oplus %.1f%%/E",
  //                     ges_clus_1x1_prod.f_res->GetParameter(0),
  //                     ges_clus_1x1_prod.f_res->GetParameter(1),
  //                     ges_clus_1x1_prod.f_res->GetParameter(2)),
  //                "l");
  //
  //  leg->AddEntry(ges_clus_5x5_recalib.resolution, "clus_5x5_recalib", "ep");
  //  leg->AddEntry(ges_clus_5x5_recalib.f_res,
  //                Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
  //                     //          Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E} #oplus %.1f%%/E",
  //                     ges_clus_5x5_recalib.f_res->GetParameter(0),
  //                     ges_clus_5x5_recalib.f_res->GetParameter(1)
  //                     //                     ges_clus_5x5_recalib.f_res->GetParameter(2)
  //                     ),
  //                "l");
  //  leg->AddEntry(new TH1(), "", "l");
  //  leg->AddEntry((TObject *) 0, " ", "");

  leg->Draw();

  TLegend *leg = new TLegend(.1, .15, .85, .25);

  leg->AddEntry(f_calo_sim,
                Form(
                    "#splitline{Simulation w/ flat light collection}{#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}}",
                    f_calo_sim->GetParameter(0), f_calo_sim->GetParameter(1)),
                "l");
  leg->Draw();

  hframe->SetTitle("Electron Resolution");

  SaveCanvas(c1,
             TString(_file0->GetName()) + "_DrawPrototype3ShowerCalib_" + TString(c1->GetName()), kTRUE);
}

void Get_Res_linear_Summmary_Sim()
{
  vector<double> beam_mom(GetBeamMom());

  //  return;

  lin_res ges_clus_5x5_prod = GetResolution("clus_5x5_prod", beam_mom, kBlue, "sum_E");
  lin_res ges_clus_3x3_prod = GetResolution("clus_3x3_prod", beam_mom,
                                            kBlue + 3, "sum_E");

  TCanvas *c1 = new TCanvas(Form("Res_linear") + cuts,
                            Form("Res_linear") + cuts, 1300, 600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  TLegend *leg = new TLegend(.15, .7, .6, .85);

  p->DrawFrame(0, 0, 25, 25,
               Form("Electron Linearity;Input energy (GeV);Measured Energy (GeV)"));
  TLine *l = new TLine(0, 0, 25, 25);
  l->SetLineColor(kGray);
  //  l->Draw();

  TF1 *f_calo_l_sim = new TF1("f_calo_l", "pol2", 0.5, 25);
  //  f_calo_l_sim->SetParameters(-0.03389, 0.9666, -0.0002822);
  f_calo_l_sim->SetParameters(-0., 1, -0.);
  //  f_calo_l_sim->SetLineWidth(1);
  f_calo_l_sim->SetLineColor(kGreen + 2);
  f_calo_l_sim->SetLineWidth(3);

  f_calo_l_sim->Draw("same");
  ges_clus_5x5_prod.linearity->Draw("p");
  ges_clus_3x3_prod.linearity->Draw("p");
  //  ge_linear->Fit(f_calo_l, "RM0");
  //  f_calo_l->Draw("same");

  leg->AddEntry(ges_clus_5x5_prod.linearity, ges_clus_5x5_prod.name, "ep");
  leg->AddEntry(ges_clus_3x3_prod.linearity, ges_clus_3x3_prod.name, "ep");
  leg->AddEntry(f_calo_l_sim, "Unity", "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  TF1 *f_calo_sim = new TF1("f_calo_sim", "sqrt([0]*[0]+[1]*[1]/x)/100", 0.5,
                            25);
  f_calo_sim->SetParameters(2.4, 11.8);
  f_calo_sim->SetLineWidth(3);
  f_calo_sim->SetLineColor(kGreen + 2);

  TH1 *hframe = p->DrawFrame(0, 0, 25, 0.3,
                             Form("Resolution;Input energy (GeV);#DeltaE/<E>"));

  TLegend *leg = new TLegend(.2, .6, .85, .9);

  ges_clus_5x5_prod.f_res->Draw("same");
  ges_clus_5x5_prod.resolution->Draw("ep");
  ges_clus_3x3_prod.f_res->Draw("same");
  ges_clus_3x3_prod.resolution->Draw("ep");
  f_calo_sim->Draw("same");

  leg->AddEntry(ges_clus_5x5_prod.resolution, ges_clus_5x5_prod.name, "ep");
  leg->AddEntry(ges_clus_5x5_prod.f_res,
                Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
                     ges_clus_5x5_prod.f_res->GetParameter(0),
                     ges_clus_5x5_prod.f_res->GetParameter(1)),
                "l");

  leg->AddEntry(ges_clus_3x3_prod.resolution, ges_clus_3x3_prod.name, "ep");
  leg->AddEntry(ges_clus_3x3_prod.f_res,
                Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
                     ges_clus_3x3_prod.f_res->GetParameter(0),
                     ges_clus_3x3_prod.f_res->GetParameter(1)),
                "l");

  //  leg->AddEntry(new TH1(), "", "l");
  //  leg->AddEntry((TObject*) 0, " ", "");

  leg->Draw();

  TLegend *leg = new TLegend(.2, .1, .85, .3);

  leg->AddEntry(f_calo_sim,
                Form("Prelim. Sim., #DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",
                     f_calo_sim->GetParameter(0), f_calo_sim->GetParameter(1)),
                "l");
  leg->Draw();

  hframe->SetTitle("Electron Resolution");

  SaveCanvas(c1,
             TString(_file0->GetName()) + "_DrawPrototype3ShowerCalib_" + TString(c1->GetName()), kTRUE);
}

vector<double>
GetBeamMom()
{
  vector<double> mom;

  TH1F *hbeam_mom = new TH1F("hbeam_mom", ";beam momentum (GeV)", 32, .5,
                             32.5);

  TText *t;
  TCanvas *c1 = new TCanvas("GetBeamMom" + cuts, "GetBeamMom" + cuts, 1800,
                            900);

  T->Draw("abs(info.beam_mom)>>hbeam_mom");

  for (int bin = 1; bin < hbeam_mom->GetNbinsX(); bin++)
  {
    if (hbeam_mom->GetBinContent(bin) > 40)
    {
      const double momentum = hbeam_mom->GetBinCenter(bin);

      if (momentum == 1 || momentum == 2 || momentum == 3 || momentum == 4 || momentum == 5 || momentum == 6 || momentum == 8 || momentum == 12 || momentum == 16 || momentum == 24 || momentum == 28 || momentum == 32)
      {
        mom.push_back(momentum);

        cout << "GetBeamMom - " << momentum << " GeV for "
             << hbeam_mom->GetBinContent(bin) << " event" << endl;
      }
    }
  }

  SaveCanvas(c1,
             TString(_file0->GetName()) + "_DrawPrototype3ShowerCalib_" + TString(c1->GetName()), kTRUE);

  return mom;
}

lin_res
GetResolution(TString cluster_name, vector<double> beam_mom, Color_t col, TString e_sum = "sum_E")
{
  vector<double> mean;
  vector<double> mean_err;
  vector<double> res;
  vector<double> res_err;

  TCanvas *c1 = new TCanvas("GetResolution_LineShape_" + cluster_name + cuts,
                            "GetResolution_LineShape_" + cluster_name + cuts, 1800, 900);

  c1->Divide(4, 2);
  int idx = 1;
  TPad *p;

  for (int i = 0; i < beam_mom.size(); ++i)
  {
    p = (TPad *) c1->cd(idx++);
    c1->Update();

    const double momemtum = beam_mom[i];
    const TString histname = Form("hLineShape%.0fGeV_", momemtum) + cluster_name;

    TH1F *h = new TH1F(histname, histname + ";Observed energy (GeV)",
                       (momemtum < 6 ? 25 : 40), 0.5, momemtum * 1.5);
    T->Draw(cluster_name + "." + e_sum + ">>" + histname,
            Form("abs(abs(info.beam_mom)-%f)/%f<.06", momemtum, momemtum));

    h->Sumw2();

    TF1 *fgaus_g = new TF1("fgaus_LG_g_" + cluster_name, "gaus",
                           h->GetMean() - 1 * h->GetRMS(), h->GetMean() + 4 * h->GetRMS());
    fgaus_g->SetParameters(1, h->GetMean() - 2 * h->GetRMS(),
                           h->GetMean() + 2 * h->GetRMS());
    h->Fit(fgaus_g, "MR0N");

    TF1 *fgaus = new TF1("fgaus_LG_" + cluster_name, "gaus",
                         fgaus_g->GetParameter(1) - 2 * fgaus_g->GetParameter(2),
                         fgaus_g->GetParameter(1) + 3 * fgaus_g->GetParameter(2));
    fgaus->SetParameters(fgaus_g->GetParameter(0), fgaus_g->GetParameter(1),
                         fgaus_g->GetParameter(2));
    h->Fit(fgaus, "MR");

    h->SetLineWidth(2);
    h->SetMarkerStyle(kFullCircle);
    fgaus->SetLineWidth(2);

    h->SetTitle(
        Form("%.0f GeV/c: #DeltaE/<E> = %.1f%%", momemtum,
             100 * fgaus->GetParameter(2) / fgaus->GetParameter(1)));

    mean.push_back(fgaus->GetParameter(1));
    mean_err.push_back(fgaus->GetParError(1));
    res.push_back(fgaus->GetParameter(2) / fgaus->GetParameter(1));
    res_err.push_back(fgaus->GetParError(2) / fgaus->GetParameter(1));
  }

  SaveCanvas(c1,
             TString(_file0->GetName()) + "_DrawPrototype3ShowerCalib_" + TString(c1->GetName()), kTRUE);

  TGraphErrors *ge_linear = new TGraphErrors(beam_mom.size(), &beam_mom[0],
                                             &mean[0], 0, &mean_err[0]);

  TGraphErrors *ge_res = new TGraphErrors(beam_mom.size(), &beam_mom[0],
                                          &res[0], 0, &res_err[0]);
  ge_res->GetHistogram()->SetStats(0);
  ge_res->Print();

  lin_res ret;
  ret.name = cluster_name;
  ret.linearity = ge_linear;
  ret.resolution = ge_res;
  ret.f_res = new TF1("f_calo_r_" + cluster_name, "sqrt([0]*[0]+[1]*[1]/x)/100",
                      //      ret.f_res = new TF1("f_calo_r_" + cluster_name, "sqrt([0]*[0]+[1]*[1]/x+[2]*[2]/x/x)/100",
                      0.5, 30);
  ret.f_res->SetParLimits(0, 2, 20);
  ret.f_res->SetParLimits(1, 10, 40);
  ret.f_res->SetParLimits(2, 10, 10);
  ret.f_res->SetParameters(2, 12, 10);
  ge_res->Fit(ret.f_res, "RM0QN");

  static int MarkerStyle = kOpenCircle - 1;
  ++MarkerStyle;

  ge_linear->SetLineColor(col);
  ge_linear->SetMarkerColor(col);
  ge_linear->SetLineWidth(2);
  ge_linear->SetMarkerStyle(MarkerStyle);
  ge_linear->SetMarkerSize(2);

  ge_res->SetLineColor(col);
  ge_res->SetMarkerColor(col);
  ge_res->SetLineWidth(2);
  ge_res->SetMarkerStyle(MarkerStyle);
  ge_res->SetMarkerSize(2);

  ge_res->GetHistogram()->SetStats(0);

  ret.f_res->SetLineColor(col);
  ret.f_res->SetLineWidth(3);

  return ret;
}

TGraphErrors *
FitProfile(const TH2 *h2)
{
  TProfile *p2 = h2->ProfileX();

  int n = 0;
  double x[1000];
  double ex[1000];
  double y[1000];
  double ey[1000];

  for (int i = 1; i <= h2->GetNbinsX(); i++)
  {
    TH1D *h1 = h2->ProjectionY(Form("htmp_%d", rand()), i, i);

    if (h1->GetSum() < 30)
    {
      cout << "FitProfile - ignore bin " << i << endl;
      continue;
    }
    else
    {
      cout << "FitProfile - fit bin " << i << endl;
    }

    TF1 fgaus("fgaus", "gaus", -p2->GetBinError(i) * 4,
              p2->GetBinError(i) * 4);

    TF1 f2(Form("dgaus"), "gaus + [3]*exp(-0.5*((x-[1])/[4])**2) + [5]",
           -p2->GetBinError(i) * 4, p2->GetBinError(i) * 4);

    fgaus.SetParameter(1, p2->GetBinContent(i));
    fgaus.SetParameter(2, p2->GetBinError(i));

    h1->Fit(&fgaus, "MQ");

    f2.SetParameters(fgaus.GetParameter(0) / 2, fgaus.GetParameter(1),
                     fgaus.GetParameter(2), fgaus.GetParameter(0) / 2,
                     fgaus.GetParameter(2) / 4, 0);

    h1->Fit(&f2, "MQ");

    //      new TCanvas;
    //      h1->Draw();
    //      fgaus.Draw("same");
    //      break;

    x[n] = p2->GetBinCenter(i);
    ex[n] = (p2->GetBinCenter(2) - p2->GetBinCenter(1)) / 2;
    y[n] = fgaus.GetParameter(1);
    ey[n] = fgaus.GetParError(1);

    //      p2->SetBinContent(i, fgaus.GetParameter(1));
    //      p2->SetBinError(i, fgaus.GetParameter(2));

    n++;
    delete h1;
  }

  return new TGraphErrors(n, x, y, ex, ey);
}

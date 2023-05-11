// $Id: $

/*!
 * \file Draw_PHG4DSTReader.C
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
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

TFile * _file0 = NULL;
TTree * T = NULL;
TString cuts = "";

void
DrawEcal_pDST( //
//    const TString infile = "../../sPHENIX_work//test_production/Upsilon/spacal2d/fieldon/SimALL_PythiaUpsilon.root_Ana.root"

//    const TString infile = "../../sPHENIX_work/production_analysis//emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root.lst_EMCalLikelihood.root"
        const TString infile = "../../sPHENIX_work/production_analysis_cemc2x2//emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root.lst_EMCalLikelihood.root"

//        const TString infile =   "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root"
//        const TString infile =   "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root"
//    const TString infile =  "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root"
//  const TString infile = "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root"
//  const TString infile = "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root"
//    const TString infile = "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root"

//        const TString infile =   "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root"
//        const TString infile =   "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root"
//    const TString infile =  "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root"
//  const TString infile = "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root"
//  const TString infile = "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root"
//    const TString infile = "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root"

//            const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root"
//                const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root"
//                const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root"
//                    const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root"
//                const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root"
//                    const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root"
    //
    //        const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root"
    //        const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root"
    //            const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root"
    //            const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root"
    //            const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root"
    //                const TString infile =   "../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root"
    //
//        const TString infile =   "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root"
//        const TString infile =   "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root"
//    const TString infile =  "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root"
//  const TString infile = "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root"
//  const TString infile = "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root"
//    const TString infile = "../../sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root"

    //            const TString infile =   "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root"
    //            const TString infile =   "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root"
    //        const TString infile =  "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root"
    //      const TString infile = "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root"
    //      const TString infile = "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root"
    //        const TString infile = "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root"

//                const TString infile =   "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root"
//    const TString infile =   "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root"
//            const TString infile =  "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root"
//          const TString infile = "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root"
//          const TString infile = "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root"
//            const TString infile = "../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root"

//    const TString infile = "G4Hits_sPHENIX_e-_eta0_8GeV-0000.root_Ana.root" //
//            const TString infile = "G4Hits_sPHENIX_pi-_eta0_8GeV-0000.root_Ana.root"//
    )
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libemcal_ana.so");
  gSystem->Load("libg4vertex.so");

  if (!_file0)
    {
      TString chian_str = infile;
      chian_str.ReplaceAll("ALL", "*");
      chian_str.ReplaceAll("+", "\\+");

      TChain * t = new TChain("T");
      const int n = t->Add(chian_str);

      cout << "Loaded " << n << " root files with " << chian_str << endl;
      assert(n>0);

      T = t;

      _file0 = new TFile;
      _file0->SetName(infile);

      fstream flst(infile + ".lst", ios_base::out);

      TObjArray *fileElements = t->GetListOfFiles();
      TIter next(fileElements);
      TChainElement *chEl = 0;
      while ((chEl = (TChainElement*) next()))
        {
          flst << chEl->GetTitle() << endl;
        }
      flst.close();

      cout << "Saved file list to " << infile + ".lst" << endl;
    }

  assert(_file0);

  T->SetAlias("UpsilonPair_trk_gpt",
      "1*sqrt(DST.UpsilonPair.trk.gpx**2 + DST.UpsilonPair.trk.gpy**2)");
  T->SetAlias("UpsilonPair_trk_pt",
      "1*sqrt(DST.UpsilonPair.trk.px**2 + DST.UpsilonPair.trk.py**2)");

  T->SetAlias("EMCalTrk_pt", "1*sqrt(DST.EMCalTrk.px**2 + DST.EMCalTrk.py**2)");
  T->SetAlias("EMCalTrk_gpt",
      "1*sqrt(DST.EMCalTrk.gpx**2 + DST.EMCalTrk.gpy**2)");

  const TCut event_sel = "1*1";
  cuts = "_all_event";

  cout << "Build event selection of " << (const char *) event_sel << endl;

  T->Draw(">>EventList", event_sel);
  TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected"
      << endl;

  T->SetEventList(elist);
////
//  UpsilonPair_Checks(infile);

//  TrackProjection_Checks(infile, "abs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05");
//  Edep_Checks(infile, 1.4, 1.4, "abs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05");

  TrackProjection_Checks(infile, "abs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05 && DST.EMCalTrk.cemce>2");
  Edep_Checks(infile, 1.4, 1.4, "abs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05 && DST.EMCalTrk.cemce>2");
}

void
MakeRadiusCut(TString infile, const double R_CEMC, const double R_HCALIN,
    TCut good_track_cut)
{
  TCanvas *c1 = new TCanvas("MakeRadiusCut" + cuts, "MakeRadiusCut" + cuts,
      1800, 900);
  c1->Divide(4, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("DST.EMCalTrk.cemc_iphi>>hcemc_iphi(130,-6.5,6.5)",
      TString("DST.EMCalTrk.cemc_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");
  hcemc_iphi->SetTitle(
      ";CEMC Azimuthal Distance (Tower Width);Energy Distribution");
  hcemc_iphi->Fit("gaus", "M");
  TF1* f = (TF1*) (hcemc_iphi->GetListOfFunctions()->At(0));
  assert(f);
  T->SetAlias("EMCalTrk_cor_cemc_iphi",
      Form("DST.EMCalTrk.cemc_iphi - %f", f->GetParameter(1)));

  const double center_cemc_iphi = f->GetParameter(1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("abs(EMCalTrk_cor_cemc_iphi)>>hEMCalTrk_cor_cemc_iphi(130,0,6.5)",
      TString("DST.EMCalTrk.cemc_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");

  p->DrawFrame(0, 1e-3, 6.5, 1,
      ";CEMC Azimuthal Distance (Tower Width);Energy Leakage Ratio");
  TGraphErrors * ge = Distribution2Efficiency(hEMCalTrk_cor_cemc_iphi);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("DST.EMCalTrk.cemc_ieta>>hcemc_ieta(130,-6.5,6.5)",
      TString("DST.EMCalTrk.cemc_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");
  hcemc_ieta->SetTitle(
      ";CEMC Polar Distance (Tower Width);Energy Distribution");
  hcemc_ieta->Fit("gaus", "M");
  TF1* f = (TF1*) (hcemc_ieta->GetListOfFunctions()->At(0));
  assert(f);
  T->SetAlias("EMCalTrk_cor_cemc_ieta",
      Form("DST.EMCalTrk.cemc_ieta - %f", f->GetParameter(1)));

  const double center_cemc_ieta = f->GetParameter(1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("abs(EMCalTrk_cor_cemc_ieta)>>hEMCalTrk_cor_cemc_ieta(130,0,6.5)",
      TString("DST.EMCalTrk.cemc_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");

  p->DrawFrame(0, 1e-3, 6.5, 1,
      ";CEMC Polar Distance (Tower Width);Energy Leakage Ratio");
  TGraphErrors * ge = Distribution2Efficiency(hEMCalTrk_cor_cemc_ieta);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("DST.EMCalTrk.hcalin_iphi>>hhcalin_iphi(130,-6.5,6.5)",
      TString("DST.EMCalTrk.hcalin_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");
  hhcalin_iphi->SetTitle(
      ";HCal_{IN} Azimuthal Distance (Tower Width);Energy Distribution");
  hhcalin_iphi->Fit("gaus", "M");
  TF1* f = (TF1*) (hhcalin_iphi->GetListOfFunctions()->At(0));
  assert(f);
  T->SetAlias("EMCalTrk_cor_hcalin_iphi",
      Form("DST.EMCalTrk.hcalin_iphi - %f", f->GetParameter(1)));

  const double center_hcalin_iphi = f->GetParameter(1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("abs(EMCalTrk_cor_hcalin_iphi)>>hEMCalTrk_cor_hcalin_iphi(130,0,6.5)",
      TString("DST.EMCalTrk.hcalin_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");

  p->DrawFrame(0, 1e-3, 6.5, 1,
      ";HCal_{IN} Azimuthal Distance (Tower Width);Energy Leakage Ratio");
  TGraphErrors * ge = Distribution2Efficiency(hEMCalTrk_cor_hcalin_iphi);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("DST.EMCalTrk.hcalin_ieta>>hhcalin_ieta(130,-6.5,6.5)",
      TString("DST.EMCalTrk.hcalin_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");
  hhcalin_ieta->SetTitle(
      ";HCal_{IN} Polar Distance (Tower Width);Energy Distribution");
  hhcalin_ieta->Fit("gaus", "M");
  TF1* f = (TF1*) (hhcalin_ieta->GetListOfFunctions()->At(0));
  assert(f);
  T->SetAlias("EMCalTrk_cor_hcalin_ieta",
      Form("DST.EMCalTrk.hcalin_ieta - %f", f->GetParameter(1)));

  const double center_hcalin_ieta = f->GetParameter(1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("abs(EMCalTrk_cor_hcalin_ieta)>>hEMCalTrk_cor_hcalin_ieta(130,0,6.5)",
      TString("DST.EMCalTrk.hcalin_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");

  p->DrawFrame(0, 1e-3, 6.5, 1,
      ";HCal_{IN} Polar Distance (Tower Width);Energy Leakage Ratio");
  TGraphErrors * ge = Distribution2Efficiency(hEMCalTrk_cor_hcalin_ieta);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  T->SetAlias("EMCalTrk_cemc_e",
      Form(
          "1*Sum$( DST.EMCalTrk.cemc_energy * ( sqrt(EMCalTrk_cor_cemc_iphi*EMCalTrk_cor_cemc_iphi + EMCalTrk_cor_cemc_ieta*EMCalTrk_cor_cemc_ieta) < %f )   )",
          R_CEMC));
  T->SetAlias("EMCalTrk_hcalin_e",
      Form(
          "1*Sum$( DST.EMCalTrk.hcalin_energy * ( sqrt(EMCalTrk_cor_hcalin_iphi*EMCalTrk_cor_hcalin_iphi + EMCalTrk_cor_hcalin_ieta*EMCalTrk_cor_hcalin_ieta) < %f )   )",
          R_CEMC));

  T->SetAlias("EMCalTrk_cemc_ntower",
      Form(
          "1*Sum$(( sqrt(EMCalTrk_cor_cemc_iphi*EMCalTrk_cor_cemc_iphi + EMCalTrk_cor_cemc_ieta*EMCalTrk_cor_cemc_ieta) < %f )   )",
          R_HCALIN));
  T->SetAlias("EMCalTrk_hcalin_ntower",
      Form(
          "1*Sum$( ( sqrt(EMCalTrk_cor_hcalin_iphi*EMCalTrk_cor_hcalin_iphi + EMCalTrk_cor_hcalin_ieta*EMCalTrk_cor_hcalin_ieta) < %f )   )",
          R_HCALIN));

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_pDST_")
          + Form("RCEMC%.1f_RCEMC%.1f_", R_CEMC, R_HCALIN)
          + TString(c1->GetName()), kFALSE);

  cout << "///////////////////////////////////////////////" << endl;
  cout << "// Projection center based on " << _file0->GetName() << endl;
  cout << "///////////////////////////////////////////////" << endl;

  cout << "const double center_cemc_iphi = " << center_cemc_iphi << "; // "
      << _file0->GetName() << endl;
  cout << "const double center_cemc_ieta = " << center_cemc_ieta << "; // "
      << _file0->GetName() << endl;
  cout << "const double center_hcalin_iphi = " << center_hcalin_iphi << "; // "
      << _file0->GetName() << endl;
  cout << "const double center_hcalin_ieta = " << center_hcalin_ieta << "; // "
      << _file0->GetName() << endl;

}

void
Edep_Checks(TString infile, const double R_CEMC, const double R_HCALIN,
    TCut good_track_cut)
{
  MakeRadiusCut(infile, R_CEMC, R_HCALIN, good_track_cut);

  double N_Event = T->GetEntries();

  TCanvas *c1 = new TCanvas("Edep_Checks" + cuts, "Edep_Checks" + cuts, 1900,
      950);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("EMCalTrk_cemc_ntower>>hEMCalTrk_cemc_ntower(16,-.5,15.5)",
      good_track_cut);
  hEMCalTrk_cemc_ntower->SetTitle(
      Form("CEMC Cluster Size (R = %.1f);Cluster Size (Towers);Probability",
          R_CEMC));
  hEMCalTrk_cemc_ntower->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("EMCalTrk_hcalin_ntower>>hEMCalTrk_hcalin_ntower(16,-.5,15.5)",
      good_track_cut);
  hEMCalTrk_hcalin_ntower->SetTitle(
      Form(
          "HCal_{in} Cluster Size (R = %.1f);Cluster Size (Towers);Probability",
          R_HCALIN));
  hEMCalTrk_hcalin_ntower->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("EMCalTrk_cemc_e>>hEMCalTrk_cemc_e(240,-.0,12)", good_track_cut);
  hEMCalTrk_cemc_e->SetTitle(
      Form(
          "CEMC Cluster Energy (R = %.1f);Cluster Energy (/bin);Probability/bin",
          R_CEMC));
  hEMCalTrk_cemc_e->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("EMCalTrk_hcalin_e>>hEMCalTrk_hcalin_e(240,-.0,12)", good_track_cut);
  hEMCalTrk_hcalin_e->SetTitle(
      Form(
          "HCal_{in} Cluster Energy (R = %.1f);Cluster Energy (GeV);Probability/bin",
          R_HCALIN));
  hEMCalTrk_hcalin_e->Scale(1. / N_Event);

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_pDST_")
          + Form("RCEMC%.1f_RCEMC%.1f_", R_CEMC, R_HCALIN)
          + TString(c1->GetName()), kFALSE);

  TCanvas *c1 = new TCanvas("Edep_Checks_2D" + cuts, "Edep_Checks_2D" + cuts,
      900, 900);
//  c1->Divide(2, 2);
//  int idx = 1;
//  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "EMCalTrk_hcalin_e:EMCalTrk_cemc_e>>h2_EMCalTrk_hcalin_e_EMCalTrk_cemc_e(240,-.0,12, 240,-.0,12)",
      good_track_cut, "colz");
  h2_EMCalTrk_hcalin_e_EMCalTrk_cemc_e->SetTitle(
      Form(
          "Energy distribution;CEMC Cluster Energy (R = %.1f) in GeV;HCal_{in} Cluster Energy (R = %.1f) in GeV",
          R_CEMC, R_HCALIN));
  h2_EMCalTrk_hcalin_e_EMCalTrk_cemc_e->Scale(1. / N_Event);
  h2_EMCalTrk_hcalin_e_EMCalTrk_cemc_e->GetZaxis()->SetRangeUser(1e-6, 1);

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_pDST_")
          + Form("RCEMC%.1f_RCEMC%.1f_", R_CEMC, R_HCALIN)
          + TString(c1->GetName()), kFALSE);

}

void
TrackProjection_Checks(TString infile, TCut good_track_cut)
{

  double N_Event = T->GetEntries();

  TCanvas *c1 = new TCanvas("TrackProjection_Checks_Raw" + cuts,
      "TrackProjection_Checks_Raw" + cuts, 1800, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "DST.EMCalTrk.cemc_iphi:DST.EMCalTrk.cemc_ieta>>hcemc(130,-6.5,6.5,130,-6.5,6.5)",
      "DST.EMCalTrk.cemc_energy", "colz");
  hcemc->SetTitle(
      "CEMC Tower Energy Distribution;Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");
  hcemc->Scale(1. / N_Event);
  hcemc->GetZaxis()->SetRangeUser(1e-5, 30);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "DST.EMCalTrk.hcalin_iphi:DST.EMCalTrk.hcalin_ieta>>hcalin(130,-6.5,6.5,130,-6.5,6.5)",
      "DST.EMCalTrk.hcalin_energy", "colz");
  hcalin->SetTitle(
      "HCal_{in} Tower Energy Distribution;Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");
  hcalin->Scale(1. / N_Event);
  hcalin->GetZaxis()->SetRangeUser(1e-5, 30);

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_pDST_")
          + TString(c1->GetName()), kFALSE);

  TCanvas *c1 = new TCanvas("TrackProjection_Checks_Tracking" + cuts,
      "TrackProjection_Checks_Tracking" + cuts, 1800, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("EMCalTrk_pt/EMCalTrk_gpt>>htracking_pt(100,0,2)", "", "");
  htracking_pt->SetTitle(
      "Tracking reco precision;p_{T, reco}/p_{T, truth};Probability per bin");
  htracking_pt->Scale(1. / N_Event);
  T->Draw("EMCalTrk_pt/EMCalTrk_gpt>>htracking_pt_cut(100,0,2)", good_track_cut,
      "same");
  htracking_pt_cut->Scale(1. / N_Event);
  htracking_pt_cut->SetLineColor(kRed);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("EMCalTrk_pt/EMCalTrk_gpt>0>>h_have_trak(3,-.5,2.5)", "", "");
  h_have_trak->Scale(1. / N_Event);
  T->Draw("2>>h_have_trak_cut(3,-.5,2.5)", good_track_cut, "same");
  h_have_trak_cut->Scale(1. / N_Event);
  h_have_trak_cut->SetLineColor(kRed);

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_pDST_")
          + TString(c1->GetName()), kFALSE);

  TCanvas *c1 = new TCanvas("TrackProjection_Checks_Cut" + cuts,
      "TrackProjection_Checks_Cut" + cuts, 1800, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "DST.EMCalTrk.cemc_iphi:DST.EMCalTrk.cemc_ieta>>hcemcc(130,-6.5,6.5,130,-6.5,6.5)",
      TString("DST.EMCalTrk.cemc_energy * (")
          + TString(good_track_cut.GetTitle()) + ")", "colz");
  hcemcc->SetTitle(
      "CEMC Tower Energy Distribution;Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");
  hcemcc->Scale(1. / N_Event);
  hcemcc->GetZaxis()->SetRangeUser(1e-5, 30);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "DST.EMCalTrk.hcalin_iphi:DST.EMCalTrk.hcalin_ieta>>hcalinc(130,-6.5,6.5,130,-6.5,6.5)",
      TString("DST.EMCalTrk.hcalin_energy * (")
          + TString(good_track_cut.GetTitle()) + ")", "colz");
  hcalinc->SetTitle(
      "HCal_{in} Tower Energy Distribution;Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");
  hcalinc->Scale(1. / N_Event);
  hcalinc->GetZaxis()->SetRangeUser(1e-5, 30);

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_pDST_")
          + TString(c1->GetName()), kFALSE);

  TCanvas *c1 = new TCanvas("TrackProjection_Checks_Proj" + cuts,
      "TrackProjection_Checks_Proj" + cuts, 1800, 900);
  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 * proj = hcemc->ProjectionX();
  proj->SetLineColor(kBlack);
  proj->Draw("");
  TH1 * proj = hcemcc->ProjectionX();
  proj->SetLineColor(kRed);
  proj->Draw("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 * proj = hcalin->ProjectionX();
  proj->SetLineColor(kBlack);
  proj->Draw("");
  TH1 * proj = hcalinc->ProjectionX();
  proj->SetLineColor(kRed);
  proj->Draw("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 * proj = hcemc->ProjectionY();
  proj->SetLineColor(kBlack);
  proj->Draw("");
  TH1 * proj = hcemcc->ProjectionY();
  proj->SetLineColor(kRed);
  proj->Draw("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 * proj = hcalin->ProjectionY();
  proj->SetLineColor(kBlack);
  proj->Draw("");
  TH1 * proj = hcalinc->ProjectionY();
  proj->SetLineColor(kRed);
  proj->Draw("same");

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_pDST_")
          + TString(c1->GetName()), kFALSE);
}

void
UpsilonPair_Checks(TString infile)
{

  TCanvas *c1 = new TCanvas("UpsilonPair_Checks" + cuts,
      "UpsilonPair_Checks" + cuts, 1800, 900);
  c1->Divide(3, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw("DST.UpsilonPair.gmass>>gmass(100,0,10)");
  T->Draw("DST.UpsilonPair.gmass>>gmassc(100,0,10)",
      "DST.UpsilonPair.good_upsilon", "same");
  gmassc->SetLineColor(kRed);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw("DST.UpsilonPair.mass>>mass(100,0,10)");
  T->Draw("DST.UpsilonPair.mass>>massc(100,0,10)",
      "DST.UpsilonPair.good_upsilon", "same");
  massc->SetLineColor(kRed);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw("UpsilonPair_trk_gpt>>UpsilonPair_trk_gpt(100,0,10)");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw("UpsilonPair_trk_pt:UpsilonPair_trk_gpt>>pt2(100,0,10,100,0,10)", "",
      "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw("UpsilonPair_trk_pt:UpsilonPair_trk_gpt>>pt2c(100,0,10,100,0,10)",
      "DST.UpsilonPair.good_upsilon", "colz");

  SaveCanvas(c1,
      TString(_file0->GetName()) + TString("_DrawEcal_pDST_")
          + TString(c1->GetName()), kFALSE);
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
TrackProjection_Checks_Comparison()
{

  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libemcal_ana.so");
  gSystem->Load("libg4vertex.so");


  TFile * f =
      new TFile(
          "../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root_DrawEcal_pDST_TrackProjection_Checks_Cut_all_event.root");

  assert(f->IsOpen());
  TH2F * hcemcc_2d = (TH2F *) f->GetObjectChecked("hcemcc", "TH2F");
  assert(hcemcc_2d);

  TFile * f =
      new TFile(
          "../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root_DrawEcal_pDST_TrackProjection_Checks_Cut_all_event.root");

  assert(f->IsOpen());
  TH2F * hcemcc_1d = (TH2F *) f->GetObjectChecked("hcemcc", "TH2F");
  assert(hcemcc_1d);

  TCanvas *c1 = new TCanvas("TrackProjection_Checks_Comparison" ,
      "TrackProjection_Checks_Comparison" , 1800, 1000);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  hcemcc_2d -> Draw("colz");
  hcemcc_2d->SetTitle(";Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");

  TLatex *text = new TLatex(0,7,"2-D Projective SPACAL, 0.9 < #eta_{e^{-}} < 1.0, E_{e^{-}} = 8 GeV");
  text->SetTextAlign(22);
  text->SetTextSize(0.044);
  text->Draw();


  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  hcemcc_1d -> Draw("colz");
  hcemcc_1d->SetTitle(";Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");

  TLatex *text = new TLatex(0,7,"1-D Projective SPACAL, 0.9 < #eta_{e^{-}} < 1.0, E_{e^{-}} = 8 GeV");
  text->SetTextAlign(22);
  text->SetTextSize(0.044);
  text->Draw();


  SaveCanvas(c1,
      "../../sPHENIX_work/production_analysis/" + TString("DrawEcal_pDST_")
          + TString(c1->GetName()));
}

void
Logs()
{

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0202159; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00453938; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.488432; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.00936002; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0402477; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00574989; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.331081; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.000527382; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root


  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0649585; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00465546; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.552238; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0102162; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.044375; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00680547; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.336741; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = 0.00108616; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0952638; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00397331; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.592338; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0118107; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0577477; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00609392; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.354109; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.000273002; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.170699; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00345903; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.668343; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0130487; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0898377; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00445598; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.376608; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0074984; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.122055; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.52588; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.644177; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.799621; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.122055; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.52588; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.644177; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.799621; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.251162; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.35067; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.761301; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.874644; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.177891; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.62604; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.393433; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.116632; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.53236; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.15917; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.899527; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.889783; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.378081; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.47524; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.445272; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.180098; // ../../sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.129035; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0222246; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.701734; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.474402; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0736277; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0556152; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.385379; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0596968; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.249273; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0146006; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.843407; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.592856; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.12442; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0514677; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.447927; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.101503; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0829824; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0275653; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.635325; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.408261; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.054392; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0852583; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.348779; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0348952; // ../../sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root




  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.16237; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00993428; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.312564; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.568171; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.039828; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0489088; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.338006; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0939571; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.0455698; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0181539; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.444123; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.475483; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0114778; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0580504; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.340425; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0670486; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root


  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.00278605; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0237387; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.504268; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.401039; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0313807; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0800593; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.336144; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0484305; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root




  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.122248; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.52555; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.500768; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.802395; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.0819904; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.83299; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.33193; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0749197; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.252243; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.34503; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.411387; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.860032; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.171931; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.65013; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.334954; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0992204; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.530751; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.15259; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.228558; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.923731; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.371817; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.46165; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.317738; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.119789; // ../../sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root



}

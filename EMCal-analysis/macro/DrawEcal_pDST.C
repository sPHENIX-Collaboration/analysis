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
//    const TString infile = "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work//test_production/Upsilon/spacal2d/fieldon/SimALL_PythiaUpsilon.root_Ana.root"
//        const TString infile =
//            "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root"
        const TString infile =
            "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root"
//        const TString infile =
//            "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root"
//    const TString infile =
//        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_24GeV-ALL.root_Ana.root"
//    const TString infile = "./G4Hits_sPHENIX_pi-_eta0_8GeV.root_Ana.root"//
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

      TChain * t = new TChain("T");
      const int n = t->Add(chian_str);

      cout << "Loaded " << n << " root files with " << chian_str << endl;
      assert(n>0);

      T = t;

      _file0 = new TFile;
      _file0->SetName(infile);
    }

  assert(_file0);

  T->SetAlias("UpsilonPair_trk_gpt",
      "1*sqrt(DST.UpsilonPair.trk.gpx**2 + DST.UpsilonPair.trk.gpy**2)");
  T->SetAlias("UpsilonPair_trk_pt",
      "1*sqrt(DST.UpsilonPair.trk.px**2 + DST.UpsilonPair.trk.py**2)");

  T->SetAlias("EMCalTrk_pt", "1*sqrt(DST.EMCalTrk.px**2 + DST.EMCalTrk.px**2)");
  T->SetAlias("EMCalTrk_gpt",
      "1*sqrt(DST.EMCalTrk.gpx**2 + DST.EMCalTrk.gpx**2)");

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

  TrackProjection_Checks(infile, "fabs(EMCalTrk_pt/EMCalTrk_gpt - 1)<0.05");
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
  hcemc->GetZaxis()->SetRangeUser(1e-5,30);


  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "DST.EMCalTrk.hcalin_iphi:DST.EMCalTrk.hcalin_ieta>>hcalin(130,-6.5,6.5,130,-6.5,6.5)",
      "DST.EMCalTrk.hcalin_energy", "colz");
  hcalin->SetTitle(
      "HCal_{in} Tower Energy Distribution;Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");
  hcalin->Scale(1. / N_Event);
  hcalin->GetZaxis()->SetRangeUser(1e-5,30);


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
      TString("DST.EMCalTrk.cemc_energy * (") + TString( good_track_cut.GetTitle()) + ")", "colz");
  hcemcc->SetTitle(
      "CEMC Tower Energy Distribution;Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");
  hcemcc->Scale(1. / N_Event);
  hcemcc->GetZaxis()->SetRangeUser(1e-5,30);


  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "DST.EMCalTrk.hcalin_iphi:DST.EMCalTrk.hcalin_ieta>>hcalinc(130,-6.5,6.5,130,-6.5,6.5)",
      TString("DST.EMCalTrk.hcalin_energy * (") + TString( good_track_cut.GetTitle()) + ")", "colz");
  hcalinc->SetTitle(
      "HCal_{in} Tower Energy Distribution;Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");
  hcalinc->Scale(1. / N_Event);
  hcalinc->GetZaxis()->SetRangeUser(1e-5,30);

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


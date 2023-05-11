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
            const TString infile = "G4Hits_sPHENIX_gamma_eta0_8GeV-0000.root_Ana.root"//
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

  T->SetAlias("MCPhoton_pt", "1*sqrt(DST.MCPhoton.px**2 + DST.MCPhoton.py**2)");
  T->SetAlias("MCPhoton_gpt",
      "1*sqrt(DST.MCPhoton.gpx**2 + DST.MCPhoton.gpy**2)");

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

  TrackProjection_Checks(infile, " ");
//  Edep_Checks(infile, 1.4, 1.4, " ");

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

  T->Draw("DST.MCPhoton.cemc_iphi>>hcemc_iphi(130,-6.5,6.5)",
      TString("DST.MCPhoton.cemc_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");
  hcemc_iphi->SetTitle(
      ";CEMC Azimuthal Distance (Tower Width);Energy Distribution");
  hcemc_iphi->Fit("gaus", "M");
  TF1* f = (TF1*) (hcemc_iphi->GetListOfFunctions()->At(0));
  assert(f);
  T->SetAlias("MCPhoton_cor_cemc_iphi",
      Form("DST.MCPhoton.cemc_iphi - %f", f->GetParameter(1)));

  const double center_cemc_iphi = f->GetParameter(1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("abs(MCPhoton_cor_cemc_iphi)>>hMCPhoton_cor_cemc_iphi(130,0,6.5)",
      TString("DST.MCPhoton.cemc_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");

  p->DrawFrame(0, 1e-3, 6.5, 1,
      ";CEMC Azimuthal Distance (Tower Width);Energy Leakage Ratio");
  TGraphErrors * ge = Distribution2Efficiency(hMCPhoton_cor_cemc_iphi);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("DST.MCPhoton.cemc_ieta>>hcemc_ieta(130,-6.5,6.5)",
      TString("DST.MCPhoton.cemc_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");
  hcemc_ieta->SetTitle(
      ";CEMC Polar Distance (Tower Width);Energy Distribution");
  hcemc_ieta->Fit("gaus", "M");
  TF1* f = (TF1*) (hcemc_ieta->GetListOfFunctions()->At(0));
  assert(f);
  T->SetAlias("MCPhoton_cor_cemc_ieta",
      Form("DST.MCPhoton.cemc_ieta - %f", f->GetParameter(1)));

  const double center_cemc_ieta = f->GetParameter(1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("abs(MCPhoton_cor_cemc_ieta)>>hMCPhoton_cor_cemc_ieta(130,0,6.5)",
      TString("DST.MCPhoton.cemc_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");

  p->DrawFrame(0, 1e-3, 6.5, 1,
      ";CEMC Polar Distance (Tower Width);Energy Leakage Ratio");
  TGraphErrors * ge = Distribution2Efficiency(hMCPhoton_cor_cemc_ieta);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("DST.MCPhoton.hcalin_iphi>>hhcalin_iphi(130,-6.5,6.5)",
      TString("DST.MCPhoton.hcalin_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");
  hhcalin_iphi->SetTitle(
      ";HCal_{IN} Azimuthal Distance (Tower Width);Energy Distribution");
  hhcalin_iphi->Fit("gaus", "M");
  TF1* f = (TF1*) (hhcalin_iphi->GetListOfFunctions()->At(0));
  assert(f);
  T->SetAlias("MCPhoton_cor_hcalin_iphi",
      Form("DST.MCPhoton.hcalin_iphi - %f", f->GetParameter(1)));

  const double center_hcalin_iphi = f->GetParameter(1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("abs(MCPhoton_cor_hcalin_iphi)>>hMCPhoton_cor_hcalin_iphi(130,0,6.5)",
      TString("DST.MCPhoton.hcalin_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");

  p->DrawFrame(0, 1e-3, 6.5, 1,
      ";HCal_{IN} Azimuthal Distance (Tower Width);Energy Leakage Ratio");
  TGraphErrors * ge = Distribution2Efficiency(hMCPhoton_cor_hcalin_iphi);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("DST.MCPhoton.hcalin_ieta>>hhcalin_ieta(130,-6.5,6.5)",
      TString("DST.MCPhoton.hcalin_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");
  hhcalin_ieta->SetTitle(
      ";HCal_{IN} Polar Distance (Tower Width);Energy Distribution");
  hhcalin_ieta->Fit("gaus", "M");
  TF1* f = (TF1*) (hhcalin_ieta->GetListOfFunctions()->At(0));
  assert(f);
  T->SetAlias("MCPhoton_cor_hcalin_ieta",
      Form("DST.MCPhoton.hcalin_ieta - %f", f->GetParameter(1)));

  const double center_hcalin_ieta = f->GetParameter(1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("abs(MCPhoton_cor_hcalin_ieta)>>hMCPhoton_cor_hcalin_ieta(130,0,6.5)",
      TString("DST.MCPhoton.hcalin_energy * (")
          + TString(good_track_cut.GetTitle()) + ")");

  p->DrawFrame(0, 1e-3, 6.5, 1,
      ";HCal_{IN} Polar Distance (Tower Width);Energy Leakage Ratio");
  TGraphErrors * ge = Distribution2Efficiency(hMCPhoton_cor_hcalin_ieta);
  ge->SetLineColor(kBlue + 2);
  ge->SetMarkerColor(kBlue + 21);
  ge->SetMarkerColor(kFullCircle);
  ge->SetLineWidth(3);
  ge->Draw("lp");

  T->SetAlias("MCPhoton_cemc_e",
      Form(
          "1*Sum$( DST.MCPhoton.cemc_energy * ( sqrt(MCPhoton_cor_cemc_iphi*MCPhoton_cor_cemc_iphi + MCPhoton_cor_cemc_ieta*MCPhoton_cor_cemc_ieta) < %f )   )",
          R_CEMC));
  T->SetAlias("MCPhoton_hcalin_e",
      Form(
          "1*Sum$( DST.MCPhoton.hcalin_energy * ( sqrt(MCPhoton_cor_hcalin_iphi*MCPhoton_cor_hcalin_iphi + MCPhoton_cor_hcalin_ieta*MCPhoton_cor_hcalin_ieta) < %f )   )",
          R_CEMC));

  T->SetAlias("MCPhoton_cemc_ntower",
      Form(
          "1*Sum$(( sqrt(MCPhoton_cor_cemc_iphi*MCPhoton_cor_cemc_iphi + MCPhoton_cor_cemc_ieta*MCPhoton_cor_cemc_ieta) < %f )   )",
          R_HCALIN));
  T->SetAlias("MCPhoton_hcalin_ntower",
      Form(
          "1*Sum$( ( sqrt(MCPhoton_cor_hcalin_iphi*MCPhoton_cor_hcalin_iphi + MCPhoton_cor_hcalin_ieta*MCPhoton_cor_hcalin_ieta) < %f )   )",
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
  T->Draw("MCPhoton_cemc_ntower>>hMCPhoton_cemc_ntower(16,-.5,15.5)",
      good_track_cut);
  hMCPhoton_cemc_ntower->SetTitle(
      Form("CEMC Cluster Size (R = %.1f);Cluster Size (Towers);Probability",
          R_CEMC));
  hMCPhoton_cemc_ntower->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("MCPhoton_hcalin_ntower>>hMCPhoton_hcalin_ntower(16,-.5,15.5)",
      good_track_cut);
  hMCPhoton_hcalin_ntower->SetTitle(
      Form(
          "HCal_{in} Cluster Size (R = %.1f);Cluster Size (Towers);Probability",
          R_HCALIN));
  hMCPhoton_hcalin_ntower->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("MCPhoton_cemc_e>>hMCPhoton_cemc_e(240,-.0,12)", good_track_cut);
  hMCPhoton_cemc_e->SetTitle(
      Form(
          "CEMC Cluster Energy (R = %.1f);Cluster Energy (/bin);Probability/bin",
          R_CEMC));
  hMCPhoton_cemc_e->Scale(1. / N_Event);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("MCPhoton_hcalin_e>>hMCPhoton_hcalin_e(240,-.0,12)", good_track_cut);
  hMCPhoton_hcalin_e->SetTitle(
      Form(
          "HCal_{in} Cluster Energy (R = %.1f);Cluster Energy (GeV);Probability/bin",
          R_HCALIN));
  hMCPhoton_hcalin_e->Scale(1. / N_Event);

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
      "MCPhoton_hcalin_e:MCPhoton_cemc_e>>h2_MCPhoton_hcalin_e_MCPhoton_cemc_e(240,-.0,12, 240,-.0,12)",
      good_track_cut, "colz");
  h2_MCPhoton_hcalin_e_MCPhoton_cemc_e->SetTitle(
      Form(
          "Energy distribution;CEMC Cluster Energy (R = %.1f) in GeV;HCal_{in} Cluster Energy (R = %.1f) in GeV",
          R_CEMC, R_HCALIN));
  h2_MCPhoton_hcalin_e_MCPhoton_cemc_e->Scale(1. / N_Event);
  h2_MCPhoton_hcalin_e_MCPhoton_cemc_e->GetZaxis()->SetRangeUser(1e-6, 1);

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
      "DST.MCPhoton.cemc_iphi:DST.MCPhoton.cemc_ieta>>hcemc(130,-6.5,6.5,130,-6.5,6.5)",
      "DST.MCPhoton.cemc_energy", "colz");
  hcemc->SetTitle(
      "CEMC Tower Energy Distribution;Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");
  hcemc->Scale(1. / N_Event);
  hcemc->GetZaxis()->SetRangeUser(1e-5, 30);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  T->Draw(
      "DST.MCPhoton.hcalin_iphi:DST.MCPhoton.hcalin_ieta>>hcalin(130,-6.5,6.5,130,-6.5,6.5)",
      "DST.MCPhoton.hcalin_energy", "colz");
  hcalin->SetTitle(
      "HCal_{in} Tower Energy Distribution;Polar Distance (Tower Width);Azimuthal Distance (Tower Width)");
  hcalin->Scale(1. / N_Event);
  hcalin->GetZaxis()->SetRangeUser(1e-5, 30);

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
//  TH1 * proj = hcemcc->ProjectionX();
//  proj->SetLineColor(kRed);
//  proj->Draw("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 * proj = hcalin->ProjectionX();
  proj->SetLineColor(kBlack);
  proj->Draw("");
//  TH1 * proj = hcalinc->ProjectionX();
//  proj->SetLineColor(kRed);
//  proj->Draw("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 * proj = hcemc->ProjectionY();
  proj->SetLineColor(kBlack);
  proj->Draw("");
//  TH1 * proj = hcemcc->ProjectionY();
//  proj->SetLineColor(kRed);
//  proj->Draw("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 * proj = hcalin->ProjectionY();
  proj->SetLineColor(kBlack);
  proj->Draw("");
//  TH1 * proj = hcalinc->ProjectionY();
//  proj->SetLineColor(kRed);
//  proj->Draw("same");

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
          "/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root_DrawEcal_pDST_TrackProjection_Checks_Cut_all_event.root");

  assert(f->IsOpen());
  TH2F * hcemcc_2d = (TH2F *) f->GetObjectChecked("hcemcc", "TH2F");
  assert(hcemcc_2d);

  TFile * f =
      new TFile(
          "/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root_DrawEcal_pDST_TrackProjection_Checks_Cut_all_event.root");

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
      "/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/" + TString("DrawEcal_pDST_")
          + TString(c1->GetName()));
}

void
Logs()
{

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0202159; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00453938; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.488432; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.00936002; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0402477; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00574989; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.331081; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.000527382; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0_8GeV-ALL.root_Ana.root


  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0649585; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00465546; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.552238; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0102162; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.044375; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00680547; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.336741; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = 0.00108616; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0952638; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00397331; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.592338; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0118107; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0577477; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00609392; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.354109; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.000273002; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.170699; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00345903; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.668343; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0130487; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0898377; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00445598; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.376608; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0074984; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.122055; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.52588; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.644177; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.799621; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.122055; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.52588; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.644177; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.799621; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.251162; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.35067; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.761301; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.874644; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.177891; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.62604; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.393433; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.116632; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.53236; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.15917; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.899527; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.889783; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.378081; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.47524; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.445272; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.180098; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.129035; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0222246; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.701734; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.474402; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0736277; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0556152; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.385379; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0596968; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.249273; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0146006; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.843407; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.592856; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.12442; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0514677; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.447927; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.101503; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0829824; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0275653; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.635325; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.408261; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.054392; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0852583; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.348779; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0348952; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root




  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.16237; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.00993428; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.312564; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.568171; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.039828; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0489088; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.338006; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0939571; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.0455698; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0181539; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.444123; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.475483; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0114778; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0580504; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.340425; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0670486; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root


  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.00278605; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0237387; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.504268; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.401039; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = 0.0313807; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 0.0800593; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.336144; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0484305; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root




  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.122248; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.52555; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.500768; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.802395; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.0819904; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.83299; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.33193; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0749197; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.252243; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.34503; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.411387; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.860032; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.171931; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.65013; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.334954; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.0992204; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.530751; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 1.15259; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.228558; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.923731; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root

  ///////////////////////////////////////////////
  // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  ///////////////////////////////////////////////
  const double center_cemc_iphi = -0.371817; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_cemc_ieta = 2.46165; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_iphi = 0.317738; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
  const double center_hcalin_ieta = -0.119789; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root



}


#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TString.h>
#include <TTree.h>
#include <cassert>
#include <cmath>
#include "SaveCanvas.C"
#include "sPhenixStyle.C"

TFile *_file0 = NULL;
TString description;
TString configuration;

void DrawFluence(
//    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_25k_Iter4/AuAu200_25k_Iter4_SUM.xml_g4score.root",
//    const TString config = "QGSP_BERT_HP",
    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_25k_Iter4_FTFP/AuAu200_25k_Iter4_FTFP_SUM.xml_g4score.root",
    const TString config = "FTFP_BERT_HP",
        const TString disc = "Au+Au #sqrt{s_{NN}}=200 GeV, sHIJING 0-20fm" , //
        const double nTarget = 1.5e12 , // 1.5 Trillion event from 5-year projection, sPH-GEN-2017-001
        const TString projection_desc = "5-year run plan (1.5 Trillion Collisions)"
    //    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_PHENIX_Iter2/AuAu200_PHENIX_Iter2_Sum.xml_g4score.root",
//    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_PHENIX_Iter5/AuAu200_PHENIX_Iter5_SUM.xml_g4score.root",
//    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_PHENIX_Iter6/AuAu200_PHENIX_Iter6_SUM.xml_g4score.root",
    //    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_PHENIX_Iter7/AuAu200_PHENIX_Iter7_SUM.xml_g4score.root",
    //    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_PHENIX_Iter7_FTFP_BERT/AuAu200_PHENIX_Iter7_FTFP_BERT_SUM.xml_g4score.root",
//        const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_PHENIX_Iter9/AuAu200_PHENIX_Iter9_SUM.xml_g4score.root",
//            const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_PHENIX_Iter9_FTFP/AuAu200_PHENIX_Iter9_FTFP_SUM.xml_g4score.root",
//    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_PHENIX_SingleTrack_Iter2/AuAu200_PHENIX_SingleTrack_Iter2_SUM.xml_g4score.root",
//        const TString disc = "Au+Au #sqrt{s_{NN}}=200 GeV, sHIJING 0-20fm",  //
//        const double nTarget = 23.1e9 * 6.8,                           // 23.1 nb-1 delivered http://www.rhichome.bnl.gov/RHIC/Runs/index.html#Run-14.
//        const TString projection_desc = "PHENIX Run14, 23.1 nb-1"
    //    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/pp200_MB_Iter1/pp200_MB_Iter1_Sum.cfg_g4score.root",
    //    const TString disc = "p+p #sqrt{s_{NN}}=200 GeV, PYTHIA8",          //
    //    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/EIC_20x250_MB_Iter1/Sum_g4score.root",
    //    const TString disc = "e+p, 20+250 GeV/c, eRHIC Pythia6",                       //
    //    const double nTarget = 10e15 * 50e-6,                                          //10 /fb @ 50ub
    //    const TString projection_desc = "10 fb^-1, collision-originated fluence only"  //
)
{
  SetsPhenixStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  _file0 = new TFile(infile);
  assert(_file0->IsOpen());
  description = disc;
  configuration = config;

//  //===================================================
//  hNormalization->SetBinContent(1, 1815);
//  //===================================================

  const double nEvent = hNormalization->GetBinContent(1);

  const double normalization = nTarget / nEvent;

  Check();
  dNchdEta();
  FullCyl(normalization, projection_desc);

//  FullCylRProjPHENIXComparison(normalization, projection_desc);

  FullCylRProj(normalization, projection_desc, 100);
  FullCylRProj(normalization, projection_desc, 30);
  //  VertexCyl(normalization, projection_desc);
  //  FullCylEIC(normalization, projection_desc);
  //  FullCylZProj(normalization, projection_desc, 2);
    FullCylZProj(normalization, projection_desc, 3);
    FullCylZProj(normalization, projection_desc, 5);
    FullCylZProj(normalization, projection_desc, 8);
}

void VertexCyl(const double normalization, const TString projection_desc)
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("VertexCyl", "VertexCyl", 1900, 960);
  c1->Divide(3, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  //  p->SetLeftMargin(.2);
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  //  p->DrawFrame(-200,-200,200,200);

  TH2 *hScore_VertexCylinder_edep_zy = hScore_VertexCylinder_edep->Project3D("zy")->DrawClone("colz");
  hScore_VertexCylinder_edep_zy->Scale(normalization);
  hScore_VertexCylinder_edep_zy->GetZaxis()->SetRangeUser(1e4, 1e12);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+", " + description, "");
  leg->AddEntry("", "Total energy deposition [MeV] for " + projection_desc, "");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_VertexCylinder_dose_zy = hScore_VertexCylinder_dose->Project3D("zy")->DrawClone("colz");
  hScore_VertexCylinder_dose_zy->Scale(normalization / hScore_VertexCylinder_dose->GetNbinsX() * 100.);  // Gy -> Rad
  hScore_VertexCylinder_dose_zy->GetZaxis()->SetRangeUser(1e-2, 1e5);
  hScore_VertexCylinder_dose_zy->GetYaxis()->SetRangeUser(20, 200);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+", " + description, "");
  leg->AddEntry("", "Radiation dose [rad] for " + projection_desc, "");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);  ///////////////////////////////////////////////////////////////////////////////////////
  c1->Update();
  p->SetLogy();

  p->DrawFrame(25, 5e2, 200, 1e6, ";R index;Radiation dose [rad]");

  TH1 *hScore_VertexCylinder_dose_z =
      hScore_VertexCylinder_dose_zy->ProjectionY()->DrawClone("same");
  const int n_rebin_hScore_VertexCylinder_dose_z = 5;
  hScore_VertexCylinder_dose_z->Rebin(n_rebin_hScore_VertexCylinder_dose_z);
  hScore_VertexCylinder_dose_z->Scale(1. / hScore_VertexCylinder_dose_zy->GetNbinsX() / n_rebin_hScore_VertexCylinder_dose_z);

  TLegend *leg = new TLegend(.2, .6, .9, .9);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+"", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", projection_desc, "");
  leg->AddEntry(hScore_VertexCylinder_dose_z, "Radiation dose", "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);  ///////////////////////////////////////////////////////////////////////////////////////
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_VertexCylinder_flux_charged_zy = hScore_VertexCylinder_flux_charged->Project3D("zy");
  hScore_VertexCylinder_flux_charged_zy->Scale(normalization / hScore_VertexCylinder_flux_charged->GetNbinsX());

  TH2 *hScore_VertexCylinder_flux_charged_EkMin1MeV_zy = hScore_VertexCylinder_flux_charged_EkMin1MeV->Project3D("zy")->DrawClone("colz");
  hScore_VertexCylinder_flux_charged_EkMin1MeV_zy->Scale(normalization / hScore_VertexCylinder_flux_charged_EkMin1MeV->GetNbinsX());
  hScore_VertexCylinder_flux_charged_EkMin1MeV_zy->GetZaxis()->SetRangeUser(1e5, 1e12);
  hScore_VertexCylinder_flux_charged_EkMin1MeV_zy->GetYaxis()->SetRangeUser(20, 200);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+", " + description, "");
  leg->AddEntry("", "Min-1-MeV Charged particle fluence [N_{ch}/cm^{2}] for " + projection_desc, "");
  leg->Draw();

  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  //  p->SetLogz();
  //  p->SetRightMargin(.15);
  //
  //  TH2 *hScore_VertexCylinder_flux_charged_EkMin100MeV_zy = hScore_VertexCylinder_flux_charged_EkMin100MeV->Project3D("zy")->DrawClone("colz");
  //  hScore_VertexCylinder_flux_charged_EkMin100MeV_zy->Scale(normalization / hScore_VertexCylinder_flux_charged_EkMin100MeV_zy->GetNbinsX());

  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  //  p->SetLogz();
  //  p->SetRightMargin(.15);
  //
  //  TH2 *hScore_VertexCylinder_flux_neutron_zy = hScore_VertexCylinder_flux_neutron->Project3D("zy")->DrawClone("colz");
  //  hScore_VertexCylinder_flux_neutron_zy->Scale(normalization / hScore_VertexCylinder_flux_neutron->GetNbinsX());

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_VertexCylinder_flux_neutron_EkMin100keV_zy = hScore_VertexCylinder_flux_neutron_EkMin100keV->Project3D("zy")->DrawClone("colz");
  hScore_VertexCylinder_flux_neutron_EkMin100keV_zy->Scale(normalization / hScore_VertexCylinder_flux_neutron_EkMin100keV->GetNbinsX());
  hScore_VertexCylinder_flux_neutron_EkMin100keV_zy->GetZaxis()->SetRangeUser(1e5, 1e12);
  hScore_VertexCylinder_flux_neutron_EkMin100keV_zy->GetYaxis()->SetRangeUser(20, 200);

  TH2 *hScore_VertexCylinder_flux_neutron_zy = hScore_VertexCylinder_flux_neutron->Project3D("zy");
  hScore_VertexCylinder_flux_neutron_zy->Scale(normalization / hScore_VertexCylinder_flux_neutron->GetNbinsX());

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+", " + description, "");
  leg->AddEntry("", "Min-100-keV Neutron fluence [n/cm^{2}] for " + projection_desc, "");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(20, 1e9, 200, 1e14, ";R index;Fluence [N/cm^{2}]");

  TH1 *hScore_VertexCylinder_flux_neutron_EkMin100keV_z =
      hScore_VertexCylinder_flux_neutron_EkMin100keV_zy->ProjectionY()->DrawClone("same");
  hScore_VertexCylinder_flux_neutron_EkMin100keV_z->Scale(1. / hScore_VertexCylinder_flux_neutron_EkMin100keV_zy->GetNbinsX());

  TH1 *hScore_VertexCylinder_flux_neutron_z =
      hScore_VertexCylinder_flux_neutron_zy->ProjectionY()->DrawClone("same");
  hScore_VertexCylinder_flux_neutron_z->Scale(1. / hScore_VertexCylinder_flux_neutron_zy->GetNbinsX());

  TH1 *hScore_VertexCylinder_flux_charged_EkMin1MeV_z =
      hScore_VertexCylinder_flux_charged_EkMin1MeV_zy->ProjectionY()->DrawClone("same");
  hScore_VertexCylinder_flux_charged_EkMin1MeV_z->Scale(1. / hScore_VertexCylinder_flux_charged_EkMin1MeV_zy->GetNbinsX());

  TH1 *hScore_VertexCylinder_flux_charged_z =
      hScore_VertexCylinder_flux_charged_zy->ProjectionY()->DrawClone("same");
  hScore_VertexCylinder_flux_charged_z->Scale(1. / hScore_VertexCylinder_flux_charged_zy->GetNbinsX());

  hScore_VertexCylinder_flux_neutron_EkMin100keV_z->SetLineColor(kRed + 2);
  hScore_VertexCylinder_flux_neutron_z->SetLineColor(kPink - 2);
  hScore_VertexCylinder_flux_charged_EkMin1MeV_z->SetLineColor(kBlue + 2);
  hScore_VertexCylinder_flux_charged_z->SetLineColor(kCyan - 2);

  TLegend *leg = new TLegend(.3, .6, .9, .9);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+"", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", projection_desc, "");
  leg->AddEntry(hScore_VertexCylinder_flux_neutron_EkMin100keV_z, "Min-100-keV Neutron", "l");
  leg->AddEntry(hScore_VertexCylinder_flux_neutron_z, "All neutron", "l");
  leg->AddEntry(hScore_VertexCylinder_flux_charged_EkMin1MeV_z, "Min-1-MeV Charged particle", "l");
  leg->AddEntry(hScore_VertexCylinder_flux_charged_z, "All charged particle", "l");
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void FullCyl(const double normalization, const TString projection_desc)
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("FullCyl", "FullCyl", 1900, 960);
  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_FullCylinder_edep_zx = hScore_FullCylinder_edep->Project3D("zx")->DrawClone("colz");
  hScore_FullCylinder_edep_zx->Scale(normalization);
  hScore_FullCylinder_edep_zx->GetZaxis()->SetRangeUser(1e8, 1e14);
  hScore_FullCylinder_edep_zx->GetXaxis()->SetRangeUser(-380, 380);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+", " + description, "");
  leg->AddEntry("", "Total energy deposition [MeV] for " + projection_desc, "");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_FullCylinder_flux_charged_EkMin1MeV_zx = hScore_FullCylinder_flux_charged_EkMin1MeV->Project3D("zx")->DrawClone("colz");
  hScore_FullCylinder_flux_charged_EkMin1MeV_zx->Scale(normalization / hScore_FullCylinder_flux_charged_EkMin1MeV->GetNbinsY());
  hScore_FullCylinder_flux_charged_EkMin1MeV_zx->GetZaxis()->SetRangeUser(1e5, 1e12);
  hScore_FullCylinder_flux_charged_EkMin1MeV_zx->GetYaxis()->SetRangeUser(2, 300);
  hScore_FullCylinder_flux_charged_EkMin1MeV_zx->GetXaxis()->SetRangeUser(-380, 380);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+", " + description, "");
  leg->AddEntry("", "Min-1-MeV Charged particle fluence [N_{ch}/cm^{2}] for " + projection_desc, "");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_FullCylinder_dose_zx = hScore_FullCylinder_dose->Project3D("zx")->DrawClone("colz");
  hScore_FullCylinder_dose_zx->Scale(normalization / hScore_FullCylinder_dose->GetNbinsY() * 100.);  // Gy -> Rad
  hScore_FullCylinder_dose_zx->GetZaxis()->SetRangeUser(.1, 1e5);
  hScore_FullCylinder_dose_zx->GetYaxis()->SetRangeUser(2, 300);
  hScore_FullCylinder_dose_zx->GetXaxis()->SetRangeUser(-380, 380);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+", " + description, "");
  leg->AddEntry("", "Radiation dose [rad] for " + projection_desc, "");
  leg->Draw();

  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  //  p->SetLogz();
  //  p->SetRightMargin(.15);
  //
  //  TH2 *hScore_FullCylinder_flux_charged_EkMin100MeV_zx = hScore_FullCylinder_flux_charged_EkMin100MeV->Project3D("zx")->DrawClone("colz");
  //  hScore_FullCylinder_flux_charged_EkMin100MeV_zx->Scale(normalization / hScore_FullCylinder_flux_charged_EkMin100MeV_zx->GetNbinsY());

  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  //  p->SetLogz();
  //  p->SetRightMargin(.15);
  //
  //  TH2 *hScore_FullCylinder_flux_neutron_zx = hScore_FullCylinder_flux_neutron->Project3D("zx")->DrawClone("colz");
  //  hScore_FullCylinder_flux_neutron_zx->Scale(normalization / hScore_FullCylinder_flux_neutron->GetNbinsY());

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_FullCylinder_flux_neutron_EkMin100keV_zx = hScore_FullCylinder_flux_neutron_EkMin100keV->Project3D("zx")->DrawClone("colz");
  hScore_FullCylinder_flux_neutron_EkMin100keV_zx->Scale(normalization / hScore_FullCylinder_flux_neutron_EkMin100keV->GetNbinsY());
  hScore_FullCylinder_flux_neutron_EkMin100keV_zx->GetZaxis()->SetRangeUser(1e5, 1e12);
  hScore_FullCylinder_flux_neutron_EkMin100keV_zx->GetYaxis()->SetRangeUser(2, 300);
  hScore_FullCylinder_flux_neutron_EkMin100keV_zx->GetXaxis()->SetRangeUser(-380, 380);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, "+configuration+", " + description, "");
  leg->AddEntry("", "Min-100-keV Neutron fluence [n/cm^{2}] for " + projection_desc, "");
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void FullCylEIC(const double normalization, const TString projection_desc)
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("FullCylEIC", "FullCylEIC", 1900, 860);
  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_FullCylinder_edep_zx = hScore_FullCylinder_edep->Project3D("zx")->DrawClone("colz");
  hScore_FullCylinder_edep_zx->Scale(normalization);
  hScore_FullCylinder_edep_zx->GetZaxis()->SetRangeUser(1e4, 1e12);
  //  hScore_FullCylinder_edep_zx->GetXaxis()->SetRangeUser(-380, 380);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX-EIC}} Simulation, Collision only, " + description, "");
  leg->AddEntry("", "Total energy deposition [MeV] for " + projection_desc, "");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_FullCylinder_flux_charged_EkMin1MeV_zx = hScore_FullCylinder_flux_charged_EkMin1MeV->Project3D("zx")->DrawClone("colz");
  hScore_FullCylinder_flux_charged_EkMin1MeV_zx->Scale(normalization / hScore_FullCylinder_flux_charged_EkMin1MeV->GetNbinsY());
  hScore_FullCylinder_flux_charged_EkMin1MeV_zx->GetZaxis()->SetRangeUser(1, 1e11);
  hScore_FullCylinder_flux_charged_EkMin1MeV_zx->GetYaxis()->SetRangeUser(2, 300);
  //  hScore_FullCylinder_flux_charged_EkMin1MeV_zx->GetXaxis()->SetRangeUser(-380, 380);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX-EIC}} Simulation, Collision only, " + description, "");
  leg->AddEntry("", "Min-1-MeV Charged particle fluence [N_{ch}/cm^{2}] for " + projection_desc, "");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_FullCylinder_dose_zx = hScore_FullCylinder_dose->Project3D("zx")->DrawClone("colz");
  hScore_FullCylinder_dose_zx->Scale(normalization / hScore_FullCylinder_dose->GetNbinsY() * 100.);  // Gy -> Rad
  hScore_FullCylinder_dose_zx->GetZaxis()->SetRangeUser(1e-6, 1e5);
  hScore_FullCylinder_dose_zx->GetYaxis()->SetRangeUser(2, 300);
  //  hScore_FullCylinder_dose_zx->GetXaxis()->SetRangeUser(-380, 380);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX-EIC}} Simulation, Collision only, " + description, "");
  leg->AddEntry("", "Radiation dose [rad] for " + projection_desc, "");
  leg->Draw();

  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  //  p->SetLogz();
  //  p->SetRightMargin(.15);
  //
  //  TH2 *hScore_FullCylinder_flux_charged_EkMin100MeV_zx = hScore_FullCylinder_flux_charged_EkMin100MeV->Project3D("zx")->DrawClone("colz");
  //  hScore_FullCylinder_flux_charged_EkMin100MeV_zx->Scale(normalization / hScore_FullCylinder_flux_charged_EkMin100MeV_zx->GetNbinsY());

  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  //  p->SetLogz();
  //  p->SetRightMargin(.15);
  //
  //  TH2 *hScore_FullCylinder_flux_neutron_zx = hScore_FullCylinder_flux_neutron->Project3D("zx")->DrawClone("colz");
  //  hScore_FullCylinder_flux_neutron_zx->Scale(normalization / hScore_FullCylinder_flux_neutron->GetNbinsY());

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);
  p->SetTopMargin(.15);

  TH2 *hScore_FullCylinder_flux_neutron_EkMin100keV_zx = hScore_FullCylinder_flux_neutron_EkMin100keV->Project3D("zx")->DrawClone("colz");
  hScore_FullCylinder_flux_neutron_EkMin100keV_zx->Scale(normalization / hScore_FullCylinder_flux_neutron_EkMin100keV->GetNbinsY());
  hScore_FullCylinder_flux_neutron_EkMin100keV_zx->GetZaxis()->SetRangeUser(1, 1e11);
  hScore_FullCylinder_flux_neutron_EkMin100keV_zx->GetYaxis()->SetRangeUser(2, 300);
  //  hScore_FullCylinder_flux_neutron_EkMin100keV_zx->GetXaxis()->SetRangeUser(-380, 380);

  TLegend *leg = new TLegend(-.1, .85, .9, .99);
  leg->AddEntry("", "#it{#bf{sPHENIX-EIC}} Simulation, Collision only, " + description, "");
  leg->AddEntry("", "Min-100-keV Neutron fluence [n/cm^{2}] for " + projection_desc, "");
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void FullCylRProj(const double normalization, const TString projection_desc, const double z_range = 100)
{
  assert(_file0);

  const bool isEIC = description.Contains("eRHIC");
  const double vertical_scale = isEIC ? 1e-3 : 1;

  if (isEIC)
    cout << "FullCylRProj - use vertical scale for EIC = " << vertical_scale << endl;

  TString CanvasName = Form("FullCylRProj_%d", (int) (z_range));

  TCanvas *c1 = new TCanvas(CanvasName, CanvasName, 1900, 960);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);  ///////////////////////////////////////////////////////////////////////////////////////
  c1->Update();
  p->SetLogy();

  p->DrawFrame(0, 1e-3 * vertical_scale, 280, 1e6 * vertical_scale, ";R [cm];Radiation dose [rad]");

  TH1 *hScore_FullCylinder_dose_z = GetZProjection(hScore_FullCylinder_dose, -z_range, z_range, normalization * 100 /*Gy -> rad*/);

  hScore_FullCylinder_dose_z->SetBinContent(2, 0);  // avoid dose for the 2nd bin which touch vacumn region
  hScore_FullCylinder_dose_z->Draw("same");

  TLegend *leg = new TLegend(.2, .7, .9, .9);
  leg->AddEntry("", isEIC ? "#it{#bf{sPHENIX-EIC}} Simulation, Collision only" : "#it{#bf{sPHENIX}} Simulation, "+configuration+"", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", projection_desc, "");
  leg->AddEntry("", Form("Averaged over |z|<%.0f cm, R>4 cm", z_range), "");
  leg->AddEntry(hScore_FullCylinder_dose_z, "Radiation dose", "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(0, 1e6 * vertical_scale, 280, 1e17 * vertical_scale, ";R [cm];Fluence [N/cm^{2}]");
  //
  TH1 *hScore_FullCylinder_flux_neutron_EkMin100keV_z = GetZProjection(hScore_FullCylinder_flux_neutron_EkMin100keV, -z_range, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_neutron_EkMin1MeV_z = GetZProjection(hScore_FullCylinder_flux_neutron_EkMin1MeV, -z_range, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_neutron_z = GetZProjection(hScore_FullCylinder_flux_neutron, -z_range, z_range, normalization);

  TH1 *hScore_FullCylinder_flux_charged_EkMin20MeV_z = GetZProjection(hScore_FullCylinder_flux_charged_EkMin20MeV, -z_range, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_charged_EkMin1MeV_z = GetZProjection(hScore_FullCylinder_flux_charged_EkMin1MeV, -z_range, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_charged_z = GetZProjection(hScore_FullCylinder_flux_charged, -z_range, z_range, normalization);

  hScore_FullCylinder_flux_neutron_EkMin1MeV_z->SetLineColor(kOrange - 3);
  hScore_FullCylinder_flux_neutron_EkMin1MeV_z->SetLineStyle(kDashed);
  hScore_FullCylinder_flux_neutron_EkMin100keV_z->SetLineColor(kRed + 2);
  hScore_FullCylinder_flux_neutron_EkMin100keV_z->SetLineStyle(kDotted);
  hScore_FullCylinder_flux_neutron_z->SetLineColor(kPink + 8);

  hScore_FullCylinder_flux_charged_EkMin20MeV_z->SetLineColor(kBlue + 2);
  hScore_FullCylinder_flux_charged_EkMin20MeV_z->SetLineStyle(kDashed);
  hScore_FullCylinder_flux_charged_EkMin1MeV_z->SetLineColor(kAzure);
  hScore_FullCylinder_flux_charged_EkMin1MeV_z->SetLineStyle(kDotted);
  hScore_FullCylinder_flux_charged_z->SetLineColor(kCyan - 2);

  hScore_FullCylinder_flux_neutron_EkMin1MeV_z->Draw("same");
  hScore_FullCylinder_flux_neutron_EkMin100keV_z->Draw("same");
  hScore_FullCylinder_flux_neutron_z->Draw("same");

  hScore_FullCylinder_flux_charged_EkMin20MeV_z->Draw("same");
  hScore_FullCylinder_flux_charged_EkMin1MeV_z->Draw("same");
  hScore_FullCylinder_flux_charged_z->Draw("same");
  //
  TLegend *leg = new TLegend(.3, .55, .9, .9);
  leg->AddEntry("", isEIC ? "#it{#bf{sPHENIX-EIC}} Simulation, Collision only" : "#it{#bf{sPHENIX}} Simulation, "+configuration+"", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", projection_desc, "");
  leg->AddEntry("", Form("Averaged over |z|<%.0f cm, R>2 cm", z_range), "");

  leg->AddEntry(hScore_FullCylinder_flux_charged_z, "All charged particle", "l");
  leg->AddEntry(hScore_FullCylinder_flux_charged_EkMin1MeV_z, "Min-1-MeV Charged particle", "l");
  leg->AddEntry(hScore_FullCylinder_flux_charged_EkMin20MeV_z, "Min-20-MeV Charged particle", "l");

  leg->AddEntry(hScore_FullCylinder_flux_neutron_z, "All neutron", "l");
  leg->AddEntry(hScore_FullCylinder_flux_neutron_EkMin100keV_z, "Min-100-keV Neutron", "l");
  leg->AddEntry(hScore_FullCylinder_flux_neutron_EkMin1MeV_z, "Min-1-MeV Neutron", "l");

  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void FullCylRProjPHENIXComparison(const double normalization, const TString projection_desc)
{
  assert(_file0);

  const double z_range = 39;

  const bool isEIC = description.Contains("eRHIC");

  assert(!isEIC);
  const double vertical_scale = isEIC ? 1e-3 : 1;

  TString CanvasName = Form("FullCylRProjPHENIXComparison_z%dcm", int(z_range));

  TCanvas *c1 = new TCanvas(CanvasName, CanvasName, 1900, 960);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);  ///////////////////////////////////////////////////////////////////////////////////////
  c1->Update();
  p->SetLogy();

  p->DrawFrame(0, 200 * vertical_scale, 28, 1e6 * vertical_scale, ";R [cm];Radiation dose [rad]");

  TH1 *hScore_FullCylinder_dose_z = GetZProjection(hScore_FullCylinder_dose, z_range, z_range, normalization * 100 /*Gy -> rad*/);

  hScore_FullCylinder_dose_z->SetBinContent(2, 0);  // avoid dose for the 2nd bin which touch vacumn region
  hScore_FullCylinder_dose_z->Draw("same");

  TGraph *gPHENIXDose = GetPHENIXDose();
  gPHENIXDose->SetMarkerStyle(kFullCross);
  gPHENIXDose->SetMarkerSize(4);
  gPHENIXDose->Draw("p");

  TLegend *leg = new TLegend(.25, .7, .9, .9);
  leg->AddEntry("", isEIC ? "#it{#bf{sPHENIX-EIC}} Simulation, Collision only" : "#it{#bf{sPHENIX}} Simulation, "+configuration+"", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", projection_desc, "");
  leg->AddEntry("", Form("z around %.0f cm", z_range), "");
  leg->AddEntry(hScore_FullCylinder_dose_z, "Simulated radiation dose", "l");
  leg->AddEntry(gPHENIXDose, "PHENIX TID data", "p");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(0, 1e9 * vertical_scale, 50, 1e17 * vertical_scale, ";R [cm];Fluence [N/cm^{2}]");
  //
  TH1 *hScore_FullCylinder_flux_neutron_EkMin100keV_z = GetZProjection(hScore_FullCylinder_flux_neutron_EkMin100keV, z_range, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_neutron_EkMin1MeV_z = GetZProjection(hScore_FullCylinder_flux_neutron_EkMin1MeV, z_range, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_neutron_z = GetZProjection(hScore_FullCylinder_flux_neutron, z_range, z_range, normalization);

  TGraph *gPHENIXNeutron = GetPHENIXNeutron();
  gPHENIXNeutron->SetMarkerStyle(kFullCross);
  gPHENIXNeutron->SetMarkerSize(4);
  gPHENIXNeutron->SetMarkerColor(kRed + 2);
  gPHENIXNeutron->Draw("p");

  //  TH1 *hScore_FullCylinder_flux_charged_EkMin20MeV_z = GetZProjection(hScore_FullCylinder_flux_charged_EkMin20MeV, z_range, z_range, normalization);
  //  TH1 *hScore_FullCylinder_flux_charged_EkMin1MeV_z = GetZProjection(hScore_FullCylinder_flux_charged_EkMin1MeV, z_range, z_range, normalization);
  //  TH1 *hScore_FullCylinder_flux_charged_z = GetZProjection(hScore_FullCylinder_flux_charged, z_range, z_range, normalization);

  hScore_FullCylinder_flux_neutron_EkMin1MeV_z->SetLineColor(kOrange - 3);
  hScore_FullCylinder_flux_neutron_EkMin1MeV_z->SetLineStyle(kDashed);
  hScore_FullCylinder_flux_neutron_EkMin100keV_z->SetLineColor(kRed + 2);
  hScore_FullCylinder_flux_neutron_EkMin100keV_z->SetLineStyle(kDotted);
  hScore_FullCylinder_flux_neutron_z->SetLineColor(kPink + 8);

  //  hScore_FullCylinder_flux_charged_EkMin20MeV_z->SetLineColor(kBlue + 2);
  //  hScore_FullCylinder_flux_charged_EkMin20MeV_z->SetLineStyle(kDashed);
  //  hScore_FullCylinder_flux_charged_EkMin1MeV_z->SetLineColor(kAzure);
  //  hScore_FullCylinder_flux_charged_EkMin1MeV_z->SetLineStyle(kDotted);
  //  hScore_FullCylinder_flux_charged_z->SetLineColor(kCyan - 2);

  hScore_FullCylinder_flux_neutron_EkMin1MeV_z->Draw("same");
  hScore_FullCylinder_flux_neutron_EkMin100keV_z->Draw("same");
  hScore_FullCylinder_flux_neutron_z->Draw("same");

  //  hScore_FullCylinder_flux_charged_EkMin20MeV_z->Draw("same");
  //  hScore_FullCylinder_flux_charged_EkMin1MeV_z->Draw("same");
  //  hScore_FullCylinder_flux_charged_z->Draw("same");
  //
  TLegend *leg = new TLegend(.3, .55, .9, .9);
  leg->AddEntry("", isEIC ? "#it{#bf{sPHENIX-EIC}} Simulation, Collision only" : "#it{#bf{sPHENIX}} Simulation, "+configuration+"", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", projection_desc, "");
  leg->AddEntry("", Form("z around %.0f cm", z_range), "");

//    leg->AddEntry(hScore_FullCylinder_flux_charged_z, "All charged particle", "l");
  //  leg->AddEntry(hScore_FullCylinder_flux_charged_EkMin1MeV_z, "Min-1-MeV Charged particle", "l");
  //  leg->AddEntry(hScore_FullCylinder_flux_charged_EkMin20MeV_z, "Min-20-MeV Charged particle", "l");

  leg->AddEntry(hScore_FullCylinder_flux_neutron_z, "Simulated all neutron", "l");
  leg->AddEntry(hScore_FullCylinder_flux_neutron_EkMin100keV_z, "Simulated min-100-keV Neutron", "l");
  leg->AddEntry(hScore_FullCylinder_flux_neutron_EkMin1MeV_z, "Simulated min-1-MeV Neutron", "l");
  leg->AddEntry(gPHENIXNeutron, "Measured TID neutron fluence", "p");

  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

TGraph *GetPHENIXDose()
{
  //  My notes have that there were 3 species in Run 14:
  //  3.4 weeks 7.3 GeV Au-Au Start 2-09
  //  13.3 weeks 100 GeV Au-Au Start 3-11
  //  2.4 weeks 100 GeV h-Au Start 6-16
  //
  //  So here are the integrated luminosity as function of run period
  //
  //  Channel 0: (r= 16.2cm)
  //  2014-02-01:  TID: 0.0 krad
  //  2014-02-09:  TID: 0.1 krad
  //  2014-03-11:  TID: 0.2 krad
  //  2014-06-16:  TID: 1.2 krad
  //  2014-07-06:  TID: 1.4 krad
  //
  //  Channel 1: (r = 3.5 cm)
  //  2014-02-01:  TID: 0.0 krad
  //  2014-02-09:  TID: 0.0 krad
  //  2014-03-11:  TID: 0.1 krad
  //  2014-06-16:  TID: 8.8 krad
  //  2014-07-06:  TID: 11. krad
  //
  //  Channel 2: (r = 8.5 cm)
  //  2014-02-01:  TID: 0.0  krad
  //  2014-02-09:  TID: 0.0 krad
  //  2014-03-11:  TID: 0.1 krad
  //  2014-06-16:  TID: 4.6 krad
  //  2014-07-06:  TID: 5.6 krad
  //
  //  Eric
  //
  //  --
  //  Eric J. Mannel, Ph.D.
  //  PHENIX Group
  //  Physics Dept.
  //  Brookhaven National Laboratory
  //  631/344-7626 (Office)
  //  914/659-3235 (Cell)
  //

  const double r_cm[] = {16.2, 3.5, 8.5};
  const double doseAuAu_rad[] =
      {
          (1.2 - 0.2) * 1e3,
          (8.8 - 0.1) * 1e3,
          (4.6 - 0.1) * 1e3};

  TGraph *g = new TGraph(3, r_cm, doseAuAu_rad);

  return g;
}

TGraph *GetPHENIXNeutron()
{
  //  Change over dates of different run species
  //
  //  Channel: 0 2014-02-09:  TID: 0.00 n/cm^2
  //  Channel: 0 2014-03-11:  TID: 0.00 x 10^12 n/cm^2
  //  Channel: 0 2014-06-16:  TID: 0.12 x 10^12 n/cm^2
  //  Channel: 0 2014-07-06:  TID: 0.13 x 10^12 n/cm^2
  //
  //  Channel: 1 2014-02-09:  TID: 0 x 10^12 n/cm^2
  //  Channel: 1 2014-03-11:  TID: 0.01 x 10^12 n/cm^2
  //  Channel: 1 2014-06-16:  TID: 0.16 x 10^12 n/cm^2
  //  Channel: 1 2014-07-06:  TID: 0.18 x 10^12 n/cm^2
  //
  //  Channel: 2 2014-02-09:  TID: 0 x 10^12 n/cm^2
  //  Channel: 2 2014-03-11:  TID: 0.01 x 10^12 n/cm^2
  //  Channel: 2 2014-06-16:  TID: 0.16 x 10^12 n/cm^2
  //  Channel: 2 2014-07-06:  TID: 0.18 x 10^12 n/cm^2
  //
  //  --
  //  Eric J. Mannel, Ph.D.
  //  PHENIX Group
  //  Physics Dept.
  //  Brookhaven National Laboratory
  //  631/344-7626 (Office)
  //  914/659-3235 (Cell)

  const double r_cm[] = {16.2, 3.5, 8.5};
  const double fluenceAuAu_icm2[] =
      {
          (0.12 - 0.0) * 1e12,
          (0.16 - 0.01) * 1e12,
          (0.16 - 0.01) * 1e12};

  TGraph *g = new TGraph(3, r_cm, fluenceAuAu_icm2);

  return g;
}

void FullCylZProj(const double normalization, const TString projection_desc, const int r_bin = 2)
{
  assert(_file0);

  const bool isEIC = description.Contains("eRHIC");
  const double vertical_scale = isEIC ? 1e-3 : 1;

  if (isEIC)
    cout << "FullCylZProj - use vertical scale for EIC = " << vertical_scale << endl;

  const double r_min = hScore_FullCylinder_dose->GetZaxis()->GetBinLowEdge(r_bin);
  const double r_max = hScore_FullCylinder_dose->GetZaxis()->GetBinUpEdge(r_bin);

  TString CanvasName = Form("FullCylZProj_%d", (int) (r_bin));

  TCanvas *c1 = new TCanvas(CanvasName, CanvasName, 1900, 800);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);  ///////////////////////////////////////////////////////////////////////////////////////
  c1->Update();
  p->SetLogy();

  p->DrawFrame(-450, 1e2 * vertical_scale, 450, 1e9 * vertical_scale, ";z [cm];Radiation dose [rad]");

  TH1 *hScore_FullCylinder_dose_z = GetRProjection(hScore_FullCylinder_dose, r_bin, normalization * 100 /*Gy -> rad*/);

  hScore_FullCylinder_dose_z->SetBinContent(2, 0);  // avoid dose for the 2nd bin which touch vacumn region
  hScore_FullCylinder_dose_z->Draw("same");

  TLegend *leg = new TLegend(.2, .7, .9, .9);
  leg->AddEntry("", isEIC ? "#it{#bf{sPHENIX-EIC}} Simulation, Collision only" : "#it{#bf{sPHENIX}} Simulation, "+configuration+"", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", projection_desc, "");
  leg->AddEntry("", Form("Averaged over %.1f<R<%.1f cm", r_min, r_max), "");
  leg->AddEntry(hScore_FullCylinder_dose_z, "Radiation dose", "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(-450, 1e8 * vertical_scale, 450, 1e18 * vertical_scale, ";z [cm];Fluence [N/cm^{2}]");
  //
  TH1 *hScore_FullCylinder_flux_neutron_EkMin100keV_z = GetRProjection(hScore_FullCylinder_flux_neutron_EkMin100keV, r_bin, normalization);
  TH1 *hScore_FullCylinder_flux_neutron_EkMin1MeV_z = GetRProjection(hScore_FullCylinder_flux_neutron_EkMin1MeV, r_bin, normalization);
  TH1 *hScore_FullCylinder_flux_neutron_z = GetRProjection(hScore_FullCylinder_flux_neutron, r_bin, normalization);

  TH1 *hScore_FullCylinder_flux_charged_EkMin20MeV_z = GetRProjection(hScore_FullCylinder_flux_charged_EkMin20MeV, r_bin, normalization);
  TH1 *hScore_FullCylinder_flux_charged_EkMin1MeV_z = GetRProjection(hScore_FullCylinder_flux_charged_EkMin1MeV, r_bin, normalization);
  TH1 *hScore_FullCylinder_flux_charged_z = GetRProjection(hScore_FullCylinder_flux_charged, r_bin, normalization);

  hScore_FullCylinder_flux_neutron_EkMin1MeV_z->SetLineColor(kOrange - 3);
  hScore_FullCylinder_flux_neutron_EkMin1MeV_z->SetLineStyle(kDashed);
  hScore_FullCylinder_flux_neutron_EkMin100keV_z->SetLineColor(kRed + 2);
  hScore_FullCylinder_flux_neutron_EkMin100keV_z->SetLineStyle(kDotted);
  hScore_FullCylinder_flux_neutron_z->SetLineColor(kPink + 8);

  hScore_FullCylinder_flux_charged_EkMin20MeV_z->SetLineColor(kBlue + 2);
  hScore_FullCylinder_flux_charged_EkMin20MeV_z->SetLineStyle(kDashed);
  hScore_FullCylinder_flux_charged_EkMin1MeV_z->SetLineColor(kAzure);
  hScore_FullCylinder_flux_charged_EkMin1MeV_z->SetLineStyle(kDotted);
  hScore_FullCylinder_flux_charged_z->SetLineColor(kCyan - 2);

  hScore_FullCylinder_flux_neutron_EkMin1MeV_z->Draw("same");
  hScore_FullCylinder_flux_neutron_EkMin100keV_z->Draw("same");
  hScore_FullCylinder_flux_neutron_z->Draw("same");

  hScore_FullCylinder_flux_charged_EkMin20MeV_z->Draw("same");
  hScore_FullCylinder_flux_charged_EkMin1MeV_z->Draw("same");
  hScore_FullCylinder_flux_charged_z->Draw("same");
  //
  TLegend *leg = new TLegend(.3, .58, .9, .93);
  leg->AddEntry("", isEIC ? "#it{#bf{sPHENIX-EIC}} Simulation, Collision only" : "#it{#bf{sPHENIX}} Simulation, "+configuration+"", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", projection_desc, "");
  leg->AddEntry("", Form("Averaged over %.1f<R<%.1f cm", r_min, r_max), "");

  leg->AddEntry(hScore_FullCylinder_flux_charged_z, "All charged particle", "l");
  leg->AddEntry(hScore_FullCylinder_flux_charged_EkMin1MeV_z, "Min-1-MeV Charged particle", "l");
  leg->AddEntry(hScore_FullCylinder_flux_charged_EkMin20MeV_z, "Min-20-MeV Charged particle", "l");

  leg->AddEntry(hScore_FullCylinder_flux_neutron_z, "All neutron", "l");
  leg->AddEntry(hScore_FullCylinder_flux_neutron_EkMin100keV_z, "Min-100-keV Neutron", "l");
  leg->AddEntry(hScore_FullCylinder_flux_neutron_EkMin1MeV_z, "Min-1-MeV Neutron", "l");

  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void dNchdEta()
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("dNchdEta", "dNchdEta", 1000, 960);
  int idx = 1;
  TPad *p;

  double nEvent = hNormalization->GetBinContent(1);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  TH1 *hNChEta = (TH1 *) _file0->GetObjectChecked("hNChEta", "TH1");
  assert(hNChEta);

  hNChEta = (TH1 *) (hNChEta->DrawClone());

  hNChEta->Sumw2();
  hNChEta->Rebin(10);
  hNChEta->Scale(1. / hNChEta->GetBinWidth(1) / nEvent);
  hNChEta->GetYaxis()->SetTitle("dN_{Ch}/d#eta");
  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

double Check()
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("Check", "Check", 1900, 960);
  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 *hNormalization = (TH1 *) _file0->GetObjectChecked("hNormalization", "TH1");
  assert(hNormalization);

  double nEvent = hNormalization->GetBinContent(1);

  hNormalization->DrawClone();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  TH1 *hVertexZ = (TH1 *) _file0->GetObjectChecked("hVertexZ", "TH1");
  assert(hVertexZ);

  hVertexZ = (TH1 *) (hVertexZ->DrawClone());

  hVertexZ->Sumw2();
  hVertexZ->Rebin(10);
  //  hVertexZ->Scale(1. / hNChEta->GetBinWidth(1) / nEvent);
  //  hVertexZ->GetYaxis()->SetTitle("dN_{Ch}/d#eta");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);

  TH2 *hScore_FullCylinder_edep_zx = hScore_FullCylinder_edep->Project3D("zx")->DrawClone("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);

  TH2 *hScore_FullCylinder_edep_zy = hScore_FullCylinder_edep->Project3D("zy")->DrawClone("colz");

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
  return nEvent;
}

TH1 *GetZProjection(const TH3 *h3, const double z_range_min, const double z_range_max, const double normalization)
{
  TH2 *h_Rz = h3->Project3D("zx");
  h_Rz->Scale(normalization / h3->GetNbinsY());

  const int bin1 = h_Rz->GetXaxis()->FindBin(z_range_min);
  const int bin2 = h_Rz->GetXaxis()->FindBin(z_range_max);
  const int nbins = bin2 - bin1 + 1;
  assert(nbins >= 1);

  cout << "GetZProjection - " << h3->GetName() << ": z " << bin1 << ", " << bin2 << endl;

  TH1 *h_R =
      h_Rz->ProjectionY(Form("%s_ProjR_z_%d_%d",
                             h3->GetName(), bin1, bin2),
                        bin1, bin2);
  h_R->Scale(1. / nbins);

  h_R->SetBinContent(1, 0);
  //  h_R->SetBinContent(2, 0);

  h_R->SetLineWidth(3);

  return h_R;
}

TH1 *GetRProjection(const TH3 *h3, const int r_bin, const double normalization)
{
  TH2 *h_Rz = h3->Project3D("zx");
  h_Rz->Scale(normalization / h3->GetNbinsY());

  assert(r_bin >= 1);

  cout << "GetRProjection - " << h3->GetName() << ": r_bin " << r_bin << endl;

  TH1 *h_R =
      h_Rz->ProjectionX(Form("%s_ProjR_r_%d",
                             h3->GetName(), r_bin),
                        r_bin, r_bin);

  h_R->SetLineWidth(3);

  return h_R;
}

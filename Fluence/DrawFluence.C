
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

void DrawFluence(
    const TString infile = "/phenix/u/jinhuang/links/sPHENIX_work/Fluence/AuAu200_10k_Iter2/AuAu200_10k_Iter2_Sum.xml_g4score.root",
    const TString disc = "Au+Au #sqrt{s_{NN}}=200 GeV, sHIJING 0-20fm"  //
)
{
  SetsPhenixStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  _file0 = new TFile(infile);
  assert(_file0->IsOpen());
  description = disc;
  //

  const double nEvent = hNormalization->GetBinContent(1);
  const double normalization = 1.5e12 / nEvent;  // 1.5 Trillion event from 5-year projection, sPH-GEN-2017-001
  const TString projection_desc = "5-year accumulated (1.5 Trillion Collisions)";

  Check();

  FullCylRProj(normalization, projection_desc, 100);
  FullCylRProj(normalization, projection_desc, 30);
  //  VertexCyl(normalization, projection_desc);
  FullCyl(normalization, projection_desc);
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated, " + description, "");
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated, " + description, "");
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated", "");
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated, " + description, "");
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated, " + description, "");
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated", "");
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated, " + description, "");
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated, " + description, "");
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated, " + description, "");
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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated, " + description, "");
  leg->AddEntry("", "Min-100-keV Neutron fluence [n/cm^{2}] for " + projection_desc, "");
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void FullCylRProj(const double normalization, const TString projection_desc, const double z_range = 100)
{
  assert(_file0);

  TString CanvasName = Form("FullCylRProj_%d", (int) (z_range));

  TCanvas *c1 = new TCanvas(CanvasName, CanvasName, 1900, 960);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);  ///////////////////////////////////////////////////////////////////////////////////////
  c1->Update();
  p->SetLogy();

  p->DrawFrame(0, 1e-3, 280, 1e6, ";R [cm];Radiation dose [rad]");

  TH1 *hScore_FullCylinder_dose_z = GetZProjection(hScore_FullCylinder_dose, z_range, normalization * 100 /*Gy -> rad*/);

  hScore_FullCylinder_dose_z->SetBinContent(2, 0);  // avoid dose for the 2nd bin which touch vacumn region
  hScore_FullCylinder_dose_z->Draw("same");

  TLegend *leg = new TLegend(.2, .7, .9, .9);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", projection_desc, "");
  leg->AddEntry("", Form("Averaged over |z|<%.0f cm, R>4 cm", z_range), "");
  leg->AddEntry(hScore_FullCylinder_dose_z, "Radiation dose", "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(0, 1e6, 280, 1e15, ";R index;Fluence [N/cm^{2}]");
  //
  TH1 *hScore_FullCylinder_flux_neutron_EkMin100keV_z = GetZProjection(hScore_FullCylinder_flux_neutron_EkMin100keV, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_neutron_EkMin1MeV_z = GetZProjection(hScore_FullCylinder_flux_neutron_EkMin1MeV, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_neutron_z = GetZProjection(hScore_FullCylinder_flux_neutron, z_range, normalization);

  TH1 *hScore_FullCylinder_flux_charged_EkMin20MeV_z = GetZProjection(hScore_FullCylinder_flux_charged_EkMin20MeV, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_charged_EkMin1MeV_z = GetZProjection(hScore_FullCylinder_flux_charged_EkMin1MeV, z_range, normalization);
  TH1 *hScore_FullCylinder_flux_charged_z = GetZProjection(hScore_FullCylinder_flux_charged, z_range, normalization);

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
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation, Uncalibrated", "");
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

  TH1 *hNChEta = (TH1 *) _file0->GetObjectChecked("hNChEta", "TH1");
  assert(hNChEta);

  hNChEta = (TH1 *) (hNChEta->DrawClone());

  hNChEta->Sumw2();
  hNChEta->Rebin(10);
  hNChEta->Scale(1. / hNChEta->GetBinWidth(1) / nEvent);
  hNChEta->GetYaxis()->SetTitle("dN_{Ch}/d#eta");

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

TH1 *GetZProjection(const TH3 *h3, const double z_range, const double normalization)
{
  TH2 *h_Rz = h3->Project3D("zx");
  h_Rz->Scale(normalization / h3->GetNbinsY());

  const int bin1 = h_Rz->GetXaxis()->FindBin(-z_range);
  const int bin2 = h_Rz->GetXaxis()->FindBin(+z_range);
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

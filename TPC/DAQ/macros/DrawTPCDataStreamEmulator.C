
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TH2.h>
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

void DataRate()
{
  assert(_file0);
  TH1 *hNormalization = (TH1 *) _file0->GetObjectChecked("hNormalization", "TH1");
  assert(hNormalization);

  TCanvas *c1 = new TCanvas("DataRate", "DataRate", 1700, 960);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.15);
  //  p->SetLogy();

  TH1 *hDataSize = (TH1 *) _file0->GetObjectChecked("hDataSize", "TH1");
  assert(hDataSize);

  TH1 *h = (TH1 *) hDataSize->DrawClone();
  h->Sumw2();
  h->Scale(1. / hNormalization->GetBinContent(1));
  h->SetTitle(";TPC Event Size [Byte];Probability / bin");

  //    h->Rebin(100);
  //    h->GetXaxis()->SetRangeUser(0,5e6);
  h->Rebin(100);
  h->GetXaxis()->SetRangeUser(0, 20e6);

  h->GetYaxis()->SetRangeUser(0, .05);

  TLegend *leg = new TLegend(.2, .7, .8, .92);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", Form("<Event size> = %.1f MB (before compression)", hDataSize->GetMean() / 1e6), "");
  leg->AddEntry("", Form("Data rate = %.0f Gbps (15 kHz trig., LZO compression)", hDataSize->GetMean() * 8 * .6 * 15e3 / 1e9), "");
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

TH1 *MakeCDF(TH1 *h)
{
  assert(h);

  TH1 *hCDF = (TH1 *) h->Clone(TString("CDF") + h->GetName());
  hCDF->SetDirectory(NULL);

  double integral = 0;

  for (int bin = h->GetNbinsX() + 1; bin >= 0; --bin)
  {
    integral += h->GetBinContent(bin);
    hCDF->SetBinContent(bin, integral);
  }

  for (int bin = h->GetNbinsX() + 1; bin >= 0; --bin)
  {
    hCDF->SetBinContent(bin, hCDF->GetBinContent(bin) / integral);
    hCDF->SetBinError(bin, 0);
  }

  return hCDF;
}

void FEEDataSize()
{
  assert(_file0);
  TH1 *hNormalization = (TH1 *) _file0->GetObjectChecked("hNormalization", "TH1");
  assert(hNormalization);

  TH2 *hFEEDataSize = (TH2 *) _file0->GetObjectChecked("hFEEDataSize", "TH2");
  assert(hFEEDataSize);

  Color_t linecolors[] = {kBlack, kRed + 2, kBlue + 2, kGreen + 2, kBlack};

  // Save raw
  TCanvas *c = new TCanvas("FEEDataSize_raw", "FEEDataSize_raw", 1900, 760);
  c->Divide(3, 1);
  int idx = 1;
  TPad *p;
  for (int module = 1; module <= 3; module++)
  {
    p = (TPad *) c->cd(idx++);
    c->Update();
    p->SetRightMargin(.15);

    TString name = Form("hFEEDataSize_module%d", module);

    (TH1 *) hFEEDataSize->ProjectionX(name, module, module)->DrawClone("");
  }
  SaveCanvas(c, TString(_file0->GetName()) + TString(c->GetName()), kTRUE);

  // Plot
  TCanvas *c1 = new TCanvas("FEEDataSize", "FEEDataSize", 1900, 760);
  c1->Divide(2, 1);
  idx = 1;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.15);
  TH1 *frame = p->DrawFrame(0, 0, 50e3, .8e6);
  frame->SetTitle(";TPC FELIX Per-Drift Data Size [Byte];Count");
  frame->GetXaxis()->SetNdivisions(15, 5, 1, true);

  TLegend *leg = new TLegend(.2, .6, .85, .92);

  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  leg->AddEntry("", description, "");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.15);
  frame = p->DrawFrame(0, 1e-5, 50e3, 1);
  frame->SetTitle(";TPC FELIX Per-Drift Data Size [Byte];CCDF");
  frame->GetXaxis()->SetNdivisions(15, 5, 1, true);
  p->SetLogy();

  for (int module = 1; module <= 3; module++)
  {
    p = (TPad *) c1->cd(1);
    c1->Update();

    TString name = Form("hFEEDataSize_module%d", module);

    TH1 *h_FEE_raw = (TH1 *) hFEEDataSize->ProjectionX(name, module, module);
    TH1 *h_FEE = (TH1 *) h_FEE_raw->DrawClone("same");
    h_FEE->Sumw2();
    h_FEE->Rebin(100);

    h_FEE->SetLineColor(linecolors[module]);
    h_FEE->SetMarkerColor(linecolors[module]);

    leg->AddEntry(h_FEE, Form("Module%d: <Data/Drift/FEE> = %.1f kB", module, h_FEE->GetMean() / 1e3), "lp");

    p = (TPad *) c1->cd(2);
    c1->Update();

    h_FEE = MakeCDF(h_FEE_raw);
    h_FEE->SetLineColor(linecolors[module]);
    h_FEE->Draw("same");
    h_FEE->GetXaxis()->SetRangeUser(0, 1e6);
  }

  p = (TPad *) c1->cd(1);
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void SectorDataSize()
{
  assert(_file0);
  TH1 *hNormalization = (TH1 *) _file0->GetObjectChecked("hNormalization", "TH1");
  assert(hNormalization);

  TH1 *hSectorDataSize = (TH1 *) _file0->GetObjectChecked("hSectorDataSize", "TH1");
  assert(hSectorDataSize);

  // Save raw
  TCanvas *c = new TCanvas("SectorDataSize_raw", "SectorDataSize_raw", 1000, 760);
  TH1 *h = (TH1 *) hSectorDataSize->DrawClone();
  SaveCanvas(c, TString(_file0->GetName()) + TString(c->GetName()), kTRUE);

  //plot
  TCanvas *c1 = new TCanvas("SectorDataSize", "SectorDataSize", 2000, 760);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.15);
  //    p->SetLogy();

  h = (TH1 *) hSectorDataSize->DrawClone();
  h->Sumw2();
  //  h->Scale(1. / hNormalization->GetBinContent(1) / 24);
  h->SetTitle(";TPC FELIX Per-DriftWindow Data Size [Byte];Count");

  h->Rebin(100);
  //    h->GetXaxis()->SetRangeUser(0,5e6);
  //  h->Rebin(20);
  h->GetXaxis()->SetRangeUser(0, 2e6);

  //  h->GetYaxis()->SetRangeUser(0, .5);

  TLegend *leg = new TLegend(.35, .7, .85, .92);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", Form("<Data/Drift Window/FELIX> = %.2f MB", hSectorDataSize->GetMean() / 1e6), "");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.15);
  p->SetLogy();

  h = MakeCDF(hSectorDataSize);
  h->Draw();
  h->SetTitle(";TPC FELIX Per-Drift Data Size [Byte];CCDF");

  h->GetXaxis()->SetRangeUser(0, 1e6);

  //  h->GetYaxis()->SetRangeUser(0, .5);

  //  TLegend *leg = new TLegend(.2, .7, .8, .92);
  //  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  //  leg->AddEntry("", description, "");
  //  leg->AddEntry("", Form("<Event size> = %.1f MB (before compression)", hDataSize->GetMean() / 1e6), "");
  //  leg->AddEntry("", Form("Data rate = %.0f Gbps (15 kHz trig., LZO compression)", hDataSize->GetMean() * 8 * .6 * 15e3 / 1e9), "");
  //  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void dNdeta()
{
  assert(_file0);
  TH1 *hNormalization = (TH1 *) _file0->GetObjectChecked("hNormalization", "TH1");
  assert(hNormalization);

  TCanvas *c1 = new TCanvas("dNdeta", "dNdeta", 1800, 960);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  TH1 *hNChEta = (TH1 *) _file0->GetObjectChecked("hNChEta", "TH1");
  assert(hNChEta);

  TH1 *h = (TH1 *) hNChEta->DrawClone();
  h->Sumw2();
  h->Rebin(10);
  h->Scale(1. / hNormalization->GetBinContent(2));
  h->Scale(1. / h->GetXaxis()->GetBinWidth(1));
  h->SetTitle(";#eta;dN_{Ch}/d#eta");

  TLegend *leg = new TLegend(.3, .25, .8, .45);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", "dN_{Ch}/d#eta, sHIJING to Geant4 input", "");
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void ChargeCheck()
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("ChargeCheck", "ChargeCheck", 1800, 960);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.2);

  TH2 *hLayerZBinADC = (TH2 *) _file0->GetObjectChecked("hLayerZBinADC", "TH2");
  assert(hLayerZBinADC);

  hLayerZBinADC->Draw("colz");

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void WaveletCheck()
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("WaveletCheck", "WaveletCheck", 1800, 960);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);

  TH2 *hLayerWaveletSize = (TH2 *) _file0->GetObjectChecked("hLayerWaveletSize", "TH2");
  assert(hLayerWaveletSize);

  hLayerWaveletSize->GetYaxis()->SetRangeUser(0, 260);
  hLayerWaveletSize->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  hLayerWaveletSize->ProjectionY()->Draw();

  TLegend *leg = new TLegend(.2, .7, .8, .92);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  leg->AddEntry("", description, "");
  leg->Draw();
  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void Check1()
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("Check1", "Check1", 1800, 960);
  c1->Divide(3, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 *hNormalization = (TH1 *) _file0->GetObjectChecked("hNormalization", "TH1");
  assert(hNormalization);

  hNormalization->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetRightMargin(.1);

  TH1 *hDataSize = (TH1 *) _file0->GetObjectChecked("hDataSize", "TH1");
  assert(hDataSize);

  TH1 *h = (TH1 *) hDataSize->DrawClone();
  h->Rebin(400);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 *hWavelet = (TH1 *) _file0->GetObjectChecked("hWavelet", "TH1");
  assert(hWavelet);

  h = (TH1 *) hWavelet->DrawClone();
  h->Rebin(400);

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);

  TH2 *hLayerWaveletSize = (TH2 *) _file0->GetObjectChecked("hLayerWaveletSize", "TH2");
  assert(hLayerWaveletSize);

  hLayerWaveletSize->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  TH1 *hNChEta = (TH1 *) _file0->GetObjectChecked("hNChEta", "TH1");
  assert(hNChEta);

  hNChEta->DrawClone();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  hNChEta = (TH1 *) _file0->GetObjectChecked("hNChEta", "TH1");
  assert(hNChEta);

  h = (TH1 *) hNChEta->DrawClone();
  h->Sumw2();
  h->Rebin(100);
  h->Scale(1. / hNormalization->GetBinContent(1));

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void Occupancy()
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("Occupancy", "Occupancy", 1800, 960);
  //  c1->Divide(3, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);

  TH2 *hLayerHit = (TH2 *) _file0->GetObjectChecked("hLayerHit", "TH2");
  assert(hLayerHit);

  hLayerHit->GetYaxis()->SetRangeUser(0, 260);
  hLayerHit->Draw("colz");

  hLayerHit->ProfileY()->Draw("same");

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void Check2()
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("Check2", "Check2", 1800, 960);
  c1->Divide(3, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);

  TH2 *hLayerHit = (TH2 *) _file0->GetObjectChecked("hLayerHit", "TH2");
  assert(hLayerHit);

  hLayerHit->GetYaxis()->SetRangeUser(0, 260);
  hLayerHit->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();
  p->SetRightMargin(.15);

  TH2 *hLayerDataSize = (TH2 *) _file0->GetObjectChecked("hLayerDataSize", "TH2");
  assert(hLayerDataSize);

  hLayerDataSize->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.15);
  //  p->SetLogz();

  TH2 *hLayerSumHit = (TH2 *) _file0->GetObjectChecked("hLayerSumHit", "TH2");
  assert(hLayerSumHit);

  hLayerSumHit->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.15);
  //  p->SetLogz();

  TH2 *hLayerSumDataSize = (TH2 *) _file0->GetObjectChecked("hLayerSumDataSize", "TH2");
  assert(hLayerSumDataSize);

  hLayerSumDataSize->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.15);
  //  p->SetLogz();

  TH2 *hLayerZBinHit = (TH2 *) _file0->GetObjectChecked("hLayerZBinHit", "TH2");
  assert(hLayerZBinHit);

  hLayerZBinHit->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.15);
  //  p->SetLogz();

  TH2 *hLayerZBinADC = (TH2 *) _file0->GetObjectChecked("hLayerZBinADC", "TH2");
  assert(hLayerZBinADC);

  hLayerZBinADC->Draw("colz");
  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void DrawTPCDataStreamEmulator(
    //        const TString infile = "data/TPCIntegratedCharge_AuAu0-4fm_200kHz.root",
    //        const TString disc = "Au+Au 0-7%C Triggered + 200 kHz collision"  //
    //    const TString infile = "data/TPCDataStreamEmulator_AuAu0-12fm_170kHz_2.root",
    //    const TString disc = "Au+Au 0-12fm Triggered + 170 kHz collision"  //
    //            const TString infile = "data/TPCDataStreamEmulator_AuAu0-14.7fm_0kHz_1.root",
    //            const TString disc = "Au+Au 0-14.7fm Triggered + No pileup collision"  //
    //    const TString infile = "data/TPCDataStreamEmulator_AuAu0-14.7fm_170kHz_1.root",
    //    const TString infile = "/sphenix/user/jinhuang/TPC/Multiplicity/AuAu200MB_170kHz_Iter2/AuAu200MB_170kHz_Iter2_SUM.xml_TPCDataStreamEmulator.root",
    //    const TString disc = "Au+Au 0-20fm Triggered + 170 kHz collision"  //
    //    const TString infile = "/sphenix/user/jinhuang/TPC/Multiplicity/AuAu200MB_170kHz_Stream_Iter3/AuAu200MB_170kHz_Stream_Iter3_SUM.xml_TPCDataStreamEmulator.root",
//    const TString infile = "/sphenix/user/jinhuang/TPC/TPCMLDataInterface//170kHz_RandomTimeFrame_Iter2/170kHz_RandomTimeFrame_Iter2_SUM.xml_TPCMLDataInterface.root",
//    const TString disc = "Au+Au 0-20fm 170 kHz collision in 13#mus"  //
        const TString infile = "/sphenix/user/jinhuang/TPC/TPCMLDataInterface/AuAu200_170kHz_MB_Iter2/AuAu200_170kHz_MB_Iter2_SUM.xml_TPCMLDataInterface.root",
        const TString disc = "Au+Au 170 kHz collision + M.B. in 13#mus"
        //        const TString infile = "/sphenix/user/jinhuang/TPC/TPCMLDataInterface/AuAu200_170kHz_10C_Iter2/AuAu200_170kHz_10C_Iter2_SUM.xml_TPCMLDataInterface.root",
//        const TString disc = "Au+Au 0-20fm 170 kHz + 10%C collision in 13#mus"  //
                                                                     //        const TString infile = "data/TPCDataStreamEmulator_AuAu0-12fm_0kHz.root",
                                                                     //        const TString disc = "Au+Au MB Triggered + 0 kHz collision"  //
)
{
  SetsPhenixStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  _file0 = new TFile(infile);
  assert(_file0->IsOpen());
  description = disc;

  WaveletCheck();
  DataRate();
//  SectorDataSize();
//  FEEDataSize();
  Occupancy();
  //
  dNdeta();
  ChargeCheck();
  //
  Check1();
  Check2();
}

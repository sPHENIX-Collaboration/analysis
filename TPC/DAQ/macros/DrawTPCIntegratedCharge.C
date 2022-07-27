
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

void DrawTPCIntegratedCharge(
//        const TString infile = "data/TPCIntegratedCharge_AuAu0-4fm_200kHz.root",
//        const TString disc = "Au+Au 0-7%C Triggered + 200 kHz collision"  //
//    const TString infile = "data/TPCIntegratedCharge_AuAu0-12fm_170kHz.root",
    const TString infile = "/sphenix/user/jinhuang/TPC/Multiplicity/AuAu200MB_170kHz_Iter2/AuAu200MB_170kHz_Iter2_SUM.xml_TPCIntegratedCharge.root",
    const TString disc = "Au+Au MB Triggered + 170 kHz collision"  //
)
{
  SetsPhenixStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  _file0 = new TFile(infile);
  assert(_file0->IsOpen());
  description = disc;

  vector<int> layers;
  layers.push_back(1);
  layers.push_back(16);
  layers.push_back(17);
  layers.push_back(32);
  layers.push_back(33);
  layers.push_back(48);

  Check();

  ChargePerLayer(layers);
}

void ChargePerLayer(vector<int> layers)
{
  assert(_file0);

  TH2 *hLayerCellCharge = (TH2 *) _file0->GetObjectChecked("hLayerCellCharge", "TH2");
  assert(hLayerCellCharge);

  TH1 *chargePDFs[100000];
  TH1 *chargeCDFs[100000];

  Color_t colors[] = {kPink + 2, kRed + 2, kSpring + 2, kGreen + 2, kAzure + 2, kBlue + 2};

  for (int i = 0; i < layers.size(); ++i)
  {
    Color_t color = colors[i];

    const int layer = layers[i];
    TH1 *chargePDF = hLayerCellCharge->ProjectionY(Form("hLayerCellCharge_PDF_Layer%d", layer), layer, layer);
    chargePDF->Sumw2();

    chargePDF->SetMarkerStyle(kFullCircle);
    chargePDF->SetMarkerColor(color);
    chargePDF->SetLineColor(color);

    chargePDF->Scale(1 / chargePDF->Integral(0, -1));

    TH1 *chargeCDF = chargePDF->Clone(Form("hLayerCellCharge_CDF_Layer%d", layer));
    for (int bin = chargeCDF->GetNbinsX() + 1; bin >= 0; --bin)
    {
      double cdf = 0;
      double cdf_err = 0;
      cdf = chargePDF->IntegralAndError(bin, -1, cdf_err);

      chargeCDF->SetBinContent(bin, cdf);
      chargeCDF->SetBinError(bin, cdf_err);
    }

    chargePDF->Rebin(10);

    chargePDFs[i] = (chargePDF);
    chargeCDFs[i] = chargeCDF;
  }

  TCanvas *c1 = new TCanvas("ChargePerLayer", "ChargePerLayer", 1800, 960);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(0, 1e-4, 610, 1, ";Charge [fC];Probability/bin");

  TLegend *leg = new TLegend(.17, .8, .93, .93);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation,  CD-1 configuration", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", "TPC charge PDF per FEE chan. over 13 us drift window", "");
  leg->Draw();

  leg = new TLegend(.55, .5, .93, .75);
  for (int i = 0; i < layers.size(); ++i)
  {
    chargePDFs[i]->Draw("same");

    TString performance = Form("Layer #%d: <Q> = %.0f fC", layers[i], chargePDFs[i]->GetMean());
    cout << "ChargePerLayer : " << performance << endl;
    leg->AddEntry(chargePDFs[i], performance, "lp");
  }

  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->DrawFrame(0, 1e-3, 610, 1, ";Charge Threshold [fC];Probability[Q > Charge Threshold]");

  TLegend *leg = new TLegend(.17, .8, .93, .93);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation,  CD-1 configuration", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", "TPC charge CCDF per FEE chan. over 13 us drift window", "");
  leg->Draw();

  leg = new TLegend(.2, .2, .65, .4);
  for (int i = 0; i < layers.size(); ++i)
  {
    chargeCDFs[i]->Draw("same");

    TString cdf_desk = Form("Layer #%d: P[Q>300fC] = %.1f%%",
                            layers[i],
                            100 * chargeCDFs[i]->GetBinContent(chargeCDFs[i]->FindBin(300)));
    cout << "ChargePerLayer : " << cdf_desk << endl;
    leg->AddEntry(chargeCDFs[i], cdf_desk, "lp");
  }
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void Check()
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("Check", "Check", 1800, 960);
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
  p->SetLogz();

  TH2 *hLayerCellHit = (TH2 *) _file0->GetObjectChecked("hLayerCellHit", "TH2");
  assert(hLayerCellHit);

  hLayerCellHit->Draw("colz");

  TProfile *hLayerCellHit_px = hLayerCellHit->ProfileX();
  hLayerCellHit_px->Draw("same");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  TH2 *hLayerCellCharge = (TH2 *) _file0->GetObjectChecked("hLayerCellCharge", "TH2");
  assert(hLayerCellCharge);

  hLayerCellCharge->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  TH2 *hLayerSumCellHit = (TH2 *) _file0->GetObjectChecked("hLayerSumCellHit", "TH2");
  assert(hLayerSumCellHit);

  hLayerSumCellHit->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  TH2 *hLayerSumCellCharge = (TH2 *) _file0->GetObjectChecked("hLayerSumCellCharge", "TH2");
  assert(hLayerSumCellCharge);

  hLayerSumCellCharge->Draw("colz");

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

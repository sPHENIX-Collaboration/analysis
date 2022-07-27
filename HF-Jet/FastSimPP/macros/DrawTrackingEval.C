#include "SaveCanvas.C"
#include "sPhenixStyle.C"

#include <TChain.h>
#include <TCut.h>
#include <TEfficiency.h>
#include <TF1.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TH2.h>
#include <TH3.h>

#include <TFile.h>

#include <TColor.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>

#include <TMath.h>
#include <TString.h>
#include <TTree.h>
#include <TVirtualFitter.h>

#include <cmath>
#include <iostream>

using namespace std;

// ROOT6 disabled assert. Well....
#ifdef assert
#undef assert
#endif
#define assert(exp)                                                                             \
  {                                                                                             \
    if (!(exp))                                                                                 \
    {                                                                                           \
      cout << "Assert (" << #exp << ") failed at " << __FILE__ << " line " << __LINE__ << endl; \
      exit(1);                                                                                  \
    }                                                                                           \
  }

TTree *ntp_gtrack(nullptr);
TFile *_file0 = NULL;
TString description;
TCut primPartCut("gpt>.1 && abs(geta)<1 && abs(gvt)<1e-10");  // primary particle in acceptance
TCut recoCut("nlmaps>=2 && pt>0");
const double pTMin(.3);

//#define ASSERT(exp) {{exit(1);} }

//! utility function to
void useLogBins(TAxis *axis)
{
  assert(axis);
  assert(axis->GetXmin() > 0);
  assert(axis->GetXmax() > 0);

  const int bins = axis->GetNbins();

  Axis_t from = log10(axis->GetXmin());
  Axis_t to = log10(axis->GetXmax());
  Axis_t width = (to - from) / bins;
  vector<Axis_t> new_bins(bins + 1);

  for (int i = 0; i <= bins; i++)
  {
    new_bins[i] = TMath::Power(10, from + i * width);
  }
  axis->Set(bins, new_bins.data());
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
      //      cout << "FitProfile - fit bin " << i << endl;
    }

    TF1 fgaus("fgaus", "gaus", -p2->GetBinError(i) * 4,
              p2->GetBinError(i) * 4);

    //    TF1 f2(Form("dgaus"), "gaus + [3]*exp(-0.5*((x-[1])/[4])**2) + [5]",
    //           -p2->GetBinError(i) * 4, p2->GetBinError(i) * 4);

    fgaus.SetParameter(1, p2->GetBinContent(i));
    fgaus.SetParameter(2, p2->GetBinError(i));

    h1->Fit(&fgaus, "MQ");

    TF1 f2(Form("dgaus"), "gaus  + [3]",
           -fgaus.GetParameter(2) * 1.5, fgaus.GetParameter(2) * 1.5);

    f2.SetParameters(fgaus.GetParameter(0), fgaus.GetParameter(1),
                     fgaus.GetParameter(2), 0);

    h1->Fit(&f2, "MQR");

    //      new TCanvas;
    //      h1->Draw();
    //      fgaus.Draw("same");
    //      break;

    x[n] = p2->GetBinCenter(i);
    ex[n] = (p2->GetBinCenter(2) - p2->GetBinCenter(1)) / 2;
    y[n] = fgaus.GetParameter(2);
    ey[n] = fgaus.GetParError(2);

    //      p2->SetBinContent(i, fgaus.GetParameter(1));
    //      p2->SetBinError(i, fgaus.GetParameter(2));

    n++;
    delete h1;
  }

  return new TGraphErrors(n, x, y, ex, ey);
}

void RecoCheck(const TString &name = "pi", const TCut &cut = "abs(gflavor)==211", const int nBins = 100)
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("RecoCheck_" + name, "RecoCheck_" + name, 1800, 960);
  c1->Divide(3, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogx();

  p->DrawFrame(pTMin, 0, 20, 1.1, "Efficiency;Truth p_{T} [GeV/c];Tracking efficiency");
  TLine *l = new TLine(pTMin, 1, 20, 1);
  l->Draw();

  TH1 *hTruthpT = new TH1F("hTruthpT", "hTruthpT;Truth p_{T} [GeV/c]", nBins, pTMin, 20);
  useLogBins(hTruthpT->GetXaxis());
  TH1 *hRecopT = new TH1F("hRecopT", "hRecopT;Truth p_{T} [GeV/c]", nBins, pTMin, 20);
  useLogBins(hRecopT->GetXaxis());
  TH1 *hRecopTMVTX = new TH1F("hRecopTMVTX", "hRecopT;Truth p_{T} [GeV/c]", nBins, pTMin, 20);
  useLogBins(hRecopTMVTX->GetXaxis());

  ntp_gtrack->Draw("gpt>>hTruthpT", cut && primPartCut, "goff");
  ntp_gtrack->Draw("gpt>>hRecopT", cut && primPartCut && "pt>0", "goff");
  ntp_gtrack->Draw("gpt>>hRecopTMVTX", cut && primPartCut && recoCut, "goff");

  TEfficiency *hEffpTMVTX = new TEfficiency(*hRecopTMVTX, *hTruthpT);
  hEffpTMVTX->SetName("hEffpTMVTX");
  hEffpTMVTX->SetTitle("Efficiency;Truth p_{T} [GeV/c];Tracking efficiency");
  hEffpTMVTX->Draw("p same");
  hEffpTMVTX->SetLineColor(kBlue + 2);
  hEffpTMVTX->SetMarkerColor(kBlue + 2);

  //  TH1 * hDCAxy = new TH1F("hDCAxy","")
  TEfficiency *hEffpT = new TEfficiency(*hRecopT, *hTruthpT);
  hEffpT->SetName("hEffpT");
  hEffpT->SetTitle("Efficiency;Truth p_{T} [GeV/c];Tracking efficiency");
  hEffpT->Draw("P same");
  hEffpT->SetLineColor(kRed + 2);
  hEffpT->SetMarkerColor(kRed + 2);

  //  TH2F * hDCA;
  //  ntp_gtrack -> Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogx();
  p->SetLogz();

  TH2 *hpTResolution = new TH2F("hpTResolution", "hpTResolution;Truth p_{T} [GeV/c];#Deltap_{T}/p_{T}", nBins, pTMin, 20, 1000, -.2, .2);
  useLogBins(hpTResolution->GetXaxis());
  hpTResolution->GetYaxis()->SetRangeUser(-.1, .1);

  ntp_gtrack->Draw("pt/gpt - 1 : gpt>>hpTResolution", cut && primPartCut && recoCut, "goff");

  TGraphErrors *gepTResolution = FitProfile(hpTResolution);
  gepTResolution->SetName("gepTResolution");

  hpTResolution->Draw("colz");
  gepTResolution->Draw("p");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogx();
  p->SetLogz();

  TH2 *hcotThetaResolution = new TH2F("hcotThetaResolution", "hcotThetaResolution;Truth p_{T} [GeV/c];#Delta[p_{z}/p_{T}]", nBins, pTMin, 20, 1000, -.1, .1);
  useLogBins(hcotThetaResolution->GetXaxis());
  hcotThetaResolution->GetYaxis()->SetRangeUser(-.01, .01);

  ntp_gtrack->Draw("pz/pt - gpz/gpt: gpt>>hcotThetaResolution", cut && primPartCut && recoCut, "goff");

  TGraphErrors *gecotThetaResolution = FitProfile(hcotThetaResolution);
  gecotThetaResolution->SetName("gecotThetaResolution");

  hcotThetaResolution->Draw("colz");
  gecotThetaResolution->Draw("p");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogx();
  p->SetLogz();

  TH2 *hsinPhiResolution = new TH2F("hsinPhiResolution", "hsinPhiResolution;Truth p_{T} [GeV/c];sin(#Delta#phi)", nBins, pTMin, 20, 1000, -.1, .1);
  useLogBins(hsinPhiResolution->GetXaxis());
  hsinPhiResolution->GetYaxis()->SetRangeUser(-.01, .01);

  ntp_gtrack->Draw("sin(atan2(py,px) - atan2(gpy,gpx)) : gpt>>hsinPhiResolution", cut && primPartCut && recoCut, "goff");

  TGraphErrors *gesinPhiResolution = FitProfile(hsinPhiResolution);
  gesinPhiResolution->SetName("gesinPhiResolution");

  hsinPhiResolution->Draw("colz");
  gesinPhiResolution->Draw("p");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogx();
  p->SetLogz();

  TH2 *hDCA3DXYResolution = new TH2F("hDCA3DXYResolution", "hDCA3DXYResolution;Truth p_{T} [GeV/c];DCA_{xy} [cm]", nBins, pTMin, 20, 1000, -.2, .2);
  useLogBins(hDCA3DXYResolution->GetXaxis());
  hDCA3DXYResolution->GetYaxis()->SetRangeUser(-.02, .02);

  ntp_gtrack->Draw("dca3dxy : gpt>>hDCA3DXYResolution", cut && primPartCut && recoCut, "goff");

  TGraphErrors *geDCA3DXYResolution = FitProfile(hDCA3DXYResolution);
  geDCA3DXYResolution->SetName("geDCA3DXYResolution");

  hDCA3DXYResolution->Draw("colz");
  geDCA3DXYResolution->Draw("p");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogx();
  p->SetLogz();

  TH2 *hDCA3DZResolution = new TH2F("hDCA3DZResolution", "hDCA3DZResolution;Truth p_{T} [GeV/c];DCA_{z} [cm]", nBins, pTMin, 20, 1000, -.2, .2);
  useLogBins(hDCA3DZResolution->GetXaxis());
  hDCA3DZResolution->GetYaxis()->SetRangeUser(-.02, .02);

  ntp_gtrack->Draw("dca3dz : gpt>>hDCA3DZResolution", cut && primPartCut && recoCut, "goff");

  TGraphErrors *geDCA3DZResolution = FitProfile(hDCA3DZResolution);
  geDCA3DZResolution->SetName("geDCA3DZResolution");

  hDCA3DZResolution->Draw("colz");
  geDCA3DZResolution->Draw("p");

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), false);
}

void DCACheck(const TString &name = "pi", const TCut &cut = "abs(gflavor)==211", const int nBins = 14)
{
  assert(_file0);

  const int nDCAbins = 2 * (100 / 1 * 2 + (1000 - 100) / 10 + (2000 - 1000) / 100);

  TH3F *hDCA3Dpz = new TH3F("hDCA3Dpz", "hDCA3Dpz;DCA_{xy} [cm];DCA_{z} [cm];Truth p_{T} [GeV/c]",  //
                            nDCAbins, -.2, .2,                                                      //
                            nDCAbins, -.2, .2,                                                      //
                            nBins, pTMin, 20);

  vector<Axis_t> new_bins(nDCAbins + 1);
  int binedge = -20000;
  for (int i = 0; i <= nDCAbins; i++)
  {
    new_bins[i] = binedge * 1e-5;  // convert .1um to cm

    double dedge = 1000;
    if (abs(binedge + .5) <= 10000)
      dedge = 100;
    if (abs(binedge + .5) <= 1000)
      dedge = 5;

    //    cout << "new_bins[" << i << "] = " << binedge << "* 1e-4 = " << new_bins[i] << ", dedge = " << dedge << endl;

    binedge += dedge;
  }
  assert(binedge == 21000);
  hDCA3Dpz->GetXaxis()->Set(nDCAbins, new_bins.data());
  hDCA3Dpz->GetYaxis()->Set(nDCAbins, new_bins.data());
  //    cout << "hDCA3Dpz->GetXaxis()->GetBinCenter(nDCAbins) = " << hDCA3Dpz->GetXaxis()->GetBinCenter(nDCAbins) << endl;

  //  ntp_gtrack->Draw("dca3dxy : dca3dz : gpt >> hDCA3Dpz", cut && primPartCut && recoCut, "goff");
  ntp_gtrack->Draw("gpt : dca3dz : dca3dxy >> hDCA3Dpz", cut && primPartCut && recoCut, "goff");

  // plot
  TCanvas *c1 = new TCanvas("DCACheck_" + name, "DCACheck_" + name, 1800, 960);

  c1->Divide(5, 3);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogx();
  p->SetLogz();

  TH2 *hDCA3DXYCheck = new TH2F("hDCA3DXYCheck", "hDCA3DXYCheck;Truth p_{T} [GeV/c];DCA_{xy}", nBins, pTMin, 20, 1000, -.2, .2);
  useLogBins(hDCA3DXYCheck->GetXaxis());
  hDCA3DXYCheck->GetYaxis()->SetRangeUser(-.02, .02);

  ntp_gtrack->Draw("dca3dxy : gpt>>hDCA3DXYCheck", cut && primPartCut && recoCut, "goff");

  TGraphErrors *geDCA3DXYCheck = FitProfile(hDCA3DXYCheck);
  geDCA3DXYCheck->SetName("geDCA3DXYCheck");

  hDCA3DXYCheck->Draw("colz");
  geDCA3DXYCheck->Draw("p");

  for (int bin = 1; bin <= nBins; bin++)
  {
    p = (TPad *) c1->cd(idx++);
    c1->Update();
    //    p->SetLogx();
    p->SetLogz();
    p->SetRightMargin(.15);

    hDCA3Dpz->GetZaxis()->SetRange(bin, bin);
    TH1 *h2D = hDCA3Dpz->Project3D("yx");
    h2D->SetName(Form("hDCA3Dpz%d", bin));
    h2D->Draw("colz");
  }

  // ping the 3D histogram for saving
  c1->GetListOfPrimitives()->Add(hDCA3Dpz);

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), false);
}

void DrawTrackingEval(
    const TString infile = "/gpfs/mnt/gpfs02/sphenix/user/jinhuang/HF-jet/HF-production-piKp-pp200-dataset1_333ALL.cfg_ALL_g4svtx_eval.root",
    const TString &name = "pi", const TCut &cut = "abs(gflavor)==211"  //
)
{
  SetsPhenixStyle();
  TVirtualFitter::SetDefaultFitter("Minuit2");

//  description = disc;
  gSystem->Load("libg4eval.so");

  if (!_file0)
  {
    TString chian_str = infile;
    chian_str.ReplaceAll("ALL", "*");

    TChain *t = new TChain("ntp_gtrack");
    const int n = t->Add(chian_str);

    cout << "Loaded " << n << " root files with " << chian_str << endl;
    assert(n > 0);
    ntp_gtrack = t;

    _file0 = new TFile;
    _file0->SetName(infile);
  }

//  gDirectory->mkdir("/pi");
//  gDirectory->Cd("/pi");
  RecoCheck(name, cut);
  DCACheck(name, cut);

  //  gDirectory->mkdir("/kaon");
  //  gDirectory->Cd("/kaon");
  //  RecoCheck("kaon", "abs(gflavor)==321");
  //  DCACheck("kaon", "abs(gflavor)==321");
  //
  //  gDirectory->mkdir("/proton");
  //  gDirectory->Cd("/proton");
  //  RecoCheck("proton", "abs(gflavor)==2212");
  //  DCACheck("proton", "abs(gflavor)==2212");
}

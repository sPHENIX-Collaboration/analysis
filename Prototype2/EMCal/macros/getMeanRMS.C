#include <cmath>
#include <vector>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TAxis.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1.h>
#include <TMath.h>
#include <cassert>
#include <iostream>
#include <fstream>

#include "SaveCanvas.C"

Double_t
langaufun(Double_t *x, Double_t *par)
{

  //Fit parameters:
  //par[0]=Width (scale) parameter of Landau density
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Total area (integral -inf to inf, normalization constant)
  //par[3]=Width (sigma) of convoluted Gaussian function
  //
  //In the Landau distribution (represented by the CERNLIB approximation),
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014; // (2 pi)^(-1/2)
  Double_t mpshift = -0.22278298; // Landau maximum location

  // Control constants
  Double_t np = 100.0; // number of convolution steps
  Double_t sc = 5.0; // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow, xupp;
  Double_t step;
  Double_t i;

  // MP shift correction
  mpc = par[1] - mpshift * par[0];

  // Range of convolution integral
  xlow = x[0] - sc * par[3];
  xupp = x[0] + sc * par[3];

  step = (xupp - xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for (i = 1.0; i <= np / 2; i++)
    {
      xx = xlow + (i - .5) * step;

      fland = TMath::Landau(xx, mpc, par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0], xx, par[3]);

      xx = xupp - (i - .5) * step;
      fland = TMath::Landau(xx, mpc, par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0], xx, par[3]);
    }

  return (par[2] * step * sum * invsq2pi / par[3]);
}

void
getMeanRMS(void)
{
  ofstream Output;
  Output.open("meanRMS_v1.txt");

  char name[20000];

  int runList[100] =
    { 2181, 2182, 2184, 2185, 2186, 2187, 2188, 2189, 2190, 2192 };
  double vert[10] =
    { 22, 42, 62, 82, 102, 122, 142, 152, 162, 172 };
  double mu[8][100];
  double rms[8][100];
  double err[8][100];
  double vt[8][100];
  double up[8][100];
  double mip[80] =
    { 43.4997, 45.6349, 40.8449, 47.7902, 48.4663, 39.2068, 48.3755, 83.1102 };

  for (int j = 0; j < 8; j++)
    {
      sprintf(name,
          "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2016/Production_1101_EMCalSet2_HCalPR12/beam_0000%d-0000_DSTReader.root_DrawPrototype2MIPAnalysis_EMCDistribution_RAW_Valid_HODO_MIP_Col2_PedestalOther_TimingGood.root",
          runList[j]);
      TFile *_file0 = TFile::Open(name);

      TCanvas *c1 = new TCanvas(Form("getMeanRMS_run%d", runList[j]),
          Form("getMeanRMS_run%d", runList[j]), 1800, 950);
      c1->Divide(4, 2);

      for (int i = 0; i < 8; i++)
        {
          double min = 0;
          double max = 150;
          //double max=400;
          //if(runList[j]==2418 || runList[j]==2417)max=400;
          //if(runList[j]==2551)max=200;
          //if(runList[j]==2560)max=400;
          /*TF1 *fitFunc = new TF1("fitFunc","[0]*TMath::Landau(x,[1],[2])+[3]*TMath::Gaus(x,[4],[5])",min,max);
           //TF1 *fitFunc = new TF1("fitFunc","[0]*TMath::Landau(x,[1],[2])",min,max);
           //TF1 *fitFunc = new TF1("fitFunc","[0]*TMath::Gaus(x,[1],[2])",min,max);
           fitFunc->SetParameter(0,100);
           fitFunc->SetParameter(1,10);
           fitFunc->SetParameter(2,10);
           fitFunc->SetParameter(3,10);
           fitFunc->SetParameter(4,10);
           fitFunc->SetParameter(5,10);*/

          c1->cd(i + 1);
          c1->cd(i + 1)->SetLogy();
          sprintf(name, "hEnergy_ieta2_iphi%d", i);
          TH1F *h = (TH1F*) gDirectory->Get(name)->Clone();
          assert(h);
          h->Draw();

          TF1 *ffit = new TF1(TString(h->GetName()) + "_langaufun", langaufun,
              0, 500, 4);
          ffit->SetParameters(6, 50, 3000, 15);
          ffit->SetParNames("Width", "MP", "Area", "GSigma");
          ffit->SetParLimits(0,0,20);
          ffit->SetParLimits(1,0,200);
          ffit->SetParLimits(2,0,20000);
//          ffit->SetParLimits(3,10,20);
          ffit->SetParLimits(3,1,30);

          h->Fit(ffit, "RBM");

          c1->Update();

//          Output << col[j] << "\t" << i << "\t" << fitFunc->GetMaximumX(20, 500)
//              << endl;
          Output << runList[j] << "\t" << vert[j] << "\t" << i << "\t"
              << ffit->GetParameter(1) << "\t" << ffit->GetParError(1) << "\t"
              << h->GetEntries() << endl;
//          mu[i][j]=h->GetMean();
          mu[i][j] = ffit->GetParameter(1);
//          h->Fit("landau");
//          mu[i][j] = landau->GetParameter(1);
//          rms[i][j] = h->GetRMS();
          err[i][j] = ffit->GetParError(1);
          vt[i][j] = vert[j]; //+i*.1;
//          up[i][j] =
//              (h->GetMean() + 1.281 * h->GetRMS() / sqrt(h->GetEntries()))
//                  / mip[i];
//          //cout<<up[i][j]<<endl;
//          Output << runList[j] << "\t" << vert[j] << "\t" << i << "\t"
//              << h->GetMean() << "\t" << h->GetRMS() << "\t" << h->GetEntries()
//              << "\t" << up[i][j] << endl;

          //if(vert[j]<160 || vert[j]>170)vt[i][j]=vert[j]+i*.4;

        }
//      sprintf(name, "meanRMS_%d_v2.root", runList[j]);
//      c1->Print(name);
      SaveCanvas(c1, TString(name) + TString(c1->GetName()), kTRUE);
    }

//  double avg[7];
//  for (int j = 0; j < 7; j++) //veritcal
//    {
//      double sum = 0.;
//      for (int i = 0; i < 8; i++) //tower
//        {
//          sum = sum + mu[i][j];
//        }
//      avg[j] = sum / 8.;
//    }

//  TCanvas c4("c4");
//  for (int i = 0; i < 8; i++)
//    {
//      TGraphErrors *gr = new TGraphErrors(7, vert, avg, 0, 0);
//      gr->SetMarkerStyle(20);
//      //gr->SetMaximum(27);
//      //gr->SetMinimum(23);
//      gr->SetTitle("");
//      gr->GetXaxis()->SetTitle("vertical position (mm)");
//      gr->GetYaxis()->SetTitle("<MPV ADC>");
//      gr->Draw("ap");
//    }

  TLegend * leg = new TLegend(0.91, 0.51, 0.99, 0.89, NULL, "brNDC");

  TCanvas * c1 = new TCanvas(Form("getMeanRMS_Summary"),
      Form("getMeanRMS_Summary"), 1800, 950);
  gPad->DrawFrame(0, 0, 200, 200);
  for (int i = 0; i < 8; i++)
    {
      TGraphErrors *gr = new TGraphErrors(7, vt[i], mu[i], 0, err[i]);
      //TGraphErrors *gr = new TGraphErrors(11,vt[i],mu[i],0,err[i]);
      gr->SetMarkerStyle(20);
      gr->SetMarkerColor(i + 1);
      //gr->SetMaximum(32);
      //gr->SetMinimum(-14);
      gr->SetMaximum(60);
      gr->SetMinimum(10);
      gr->SetTitle("");
      gr->GetXaxis()->SetTitle("vertical position (mm)");
      gr->GetYaxis()->SetTitle("MPV ADC");
//      if (i == 0)
//        gr->Draw("ap");
//      if (i > 0)
      gr->Draw("p");
      //gr->Fit("pol1");
      //gr->GetXaxis()->TAxis::SetNdivisions(505);
      //gr->GetXaxis()->SetLimits(160.6,166);
      //gr->GetXaxis()->SetLimits(130,190);
      sprintf(name, "row %d", i);
      leg->AddEntry(gr, name, "p");

//      gr->Draw("p");
    }
  leg->Draw();
  SaveCanvas(c1, TString(c1->GetName()), kTRUE);

//  leg2 = new TLegend(0.54, 0.35, 0.62, 0.73, NULL, "brNDC");
//  TCanvas c3("c3");
//  for (int i = 0; i < 8; i++)
//    {
//      TGraphErrors *gr2 = new TGraphErrors(11, vert, up[i], 0, 0);
//      //TGraphErrors *gr2 = new TGraphErrors(11,vert,mu[i],0,err[i]);
//      gr2->SetMarkerStyle(20);
//      gr2->SetMarkerColor(i + 1);
//      //gr2->SetMaximum(32);
//      //gr2->SetMinimum(-14);
//      gr2->SetMaximum(.4);
//      gr2->SetMinimum(-.04);
//      gr2->SetTitle("");
//      gr2->GetXaxis()->SetTitle("vertical position (mm)");
//      gr2->GetYaxis()->SetTitle("Cherenkov/MIP");
//      if (i == 0)
//        gr2->Draw("ap");
//      if (i > 0)
//        gr2->Draw("p");
//      //gr2->GetXaxis()->TAxis::SetNdivisions(505);
//      //gr2->GetXaxis()->SetLimits(160.6,166);
//      gr2->GetXaxis()->SetLimits(130, 190);
//      sprintf(name, "row %d", i);
//      leg2->AddEntry(gr2, name, "p");
//    }
//  leg2->Draw();

}


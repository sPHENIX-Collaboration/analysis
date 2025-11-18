#include "TROOT.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TRandom.h"
#include "TMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TBox.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TPolyLine.h"
#include "TLegend.h"

// author:  J.Nagle - email jamie.nagle@colorado.edu for more information

#include <iostream>
#include <fstream>

void nlo_plot () {
  // read ascii files for pp@200 GeV NLO pQCD calculations from Werner V.
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  TCanvas *c1 = new TCanvas("c1","c1",10,10,700,900);
  c1->SetLogy(1);

  // template histogram for overlays  
  TH1F *h1 = new TH1F("h1","h1",100,0.0,100.0);
  h1->SetMaximum(1.e-2);
  h1->SetMinimum(1.e-21);
  h1->SetYTitle("Counts/Event with p_{T} > p_{T}(cut) [p-p @ 200 GeV]");
  h1->SetYTitle("Counts/Event [p-p @ 200 GeV]");
  h1->GetYaxis()->SetTitleOffset(1.5);
  h1->SetXTitle("Transverse Momentum (GeV/c)");
  h1->Draw();

  // Werner's numbers are in pb/GeV^2 which gets multiplied up x28 
  // to correspond to 24 pb^-1 or 1e12 pp@200 GeV events
  // Also need to multiply by 2*pi*pt*dpt*deta to get counts per bin with dpt = 2.5 and deta = 2.0
  // To get counts per event scale down by 1 / 1e12
  double scalefactor = 28.0 * (2.0*3.14159*2.5*2.0) / 1.e12;  // need to include pt factor point-by-point

  ifstream myfile;
  TGraph *tjet = new TGraph();

  myfile.open("jets_newphenix_sc1.dat");
  for (int index=0; index<38; index++) {
    double pt; double yield;
    myfile >> pt >> yield;
    yield = yield * scalefactor*pt;
    tjet->SetPoint(index,pt,yield);
  }
  myfile.close();
  tjet->SetMarkerStyle(21);
  tjet->SetMarkerColor(2);
  tjet->SetLineColor(2);
  tjet->Draw("p,l,same");

  //myfile.open("jets_newphenix_sc05.dat");
  //TGraph *tjet05 = new TGraph();
  //for (int index=0; index<38; index++) {
  //  double pt; double yield;
  //  myfile >> pt >> yield;
  //  yield = yield * scalefactor*pt;
  //  tjet05->SetPoint(index,pt,yield);
  //}
  //myfile.close();
  //tjet05->SetMarkerStyle(21);
  //tjet05->SetMarkerColor(2);
  //tjet05->SetLineColor(2);
  //tjet05->Draw("l,same");

  //myfile.open("jets_newphenix_sc2.dat");
  //TGraph *tjet2 = new TGraph();
  //for (int index=0; index<38; index++) {
  //  double pt; double yield;
  //  myfile >> pt >> yield;
  //  yield = yield * scalefactor*pt;
  //  tjet2->SetPoint(index,pt,yield);
  //}
  //myfile.close();
  //tjet2->SetMarkerStyle(21);
  //tjet2->SetMarkerColor(2);
  //tjet2->SetLineColor(2);
  //tjet2->Draw("l,same");

  TLegend *tleg = new TLegend(0.5,0.7,0.9,0.9,"Hard Processes pQCD @ 200 GeV","brNDC");
  tleg->AddEntry(tjet,"NLO pQCD W. Vogelsang","");
  tleg->AddEntry(tjet,"Light q + g jets","p,l");
  tleg->Draw("same");
}
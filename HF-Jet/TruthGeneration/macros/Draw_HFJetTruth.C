// $Id: $                                                                                             

/*!
 * \file Draw_HFJetTruth.C
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
#include <TLatex.h>
#include <TGraphErrors.h>
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"

TFile * _file0 = NULL;
TTree * T = NULL;

void
Draw_HFJetTruth(const TString infile =
    "../macros3/G4sPHENIXCells.root_DSTReader.root",
    const double int_lumi = 678156/7.882e-05 / 1e9, const double dy = 0.6*2)
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");

  if (!_file0)
    {
      TString chian_str = infile;
      chian_str.ReplaceAll("ALL", "*");

      TChain * t = new TChain("T");
      const int n = t->Add(chian_str);

      cout << "Loaded " << n << " root files with " << chian_str << endl;
      assert(n > 0);

      T = t;

      _file0 = new TFile;
      _file0->SetName(infile);
    }

  TH1 * hall = new TH1F("hall", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_b = new TH1F("h_b", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_bq = new TH1F("h_bq", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_bh = new TH1F("h_bh", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_bh5 = new TH1F("h_bh5", ";p_{T} (GeV/c)", 100, 0, 100);
  TH1 * h_c = new TH1F("h_c", ";p_{T} (GeV/c)", 100, 0, 100);

  T->Draw("AntiKt_Truth_r04.get_pt()>>hall",
      "AntiKt_Truth_r04.get_pt()>25 && abs(AntiKt_Truth_r04.get_eta())<0.6",
      "goff");

  T->Draw("AntiKt_Truth_r04.get_pt()>>h_b",
      "AntiKt_Truth_r04.get_pt()>25 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1000))==5",
      "goff");
  T->Draw("AntiKt_Truth_r04.get_pt()* AntiKt_Truth_r04.get_property(1001) >>h_bq",
      "AntiKt_Truth_r04.get_pt()>25 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1000))==5",
      "goff");

//  T->Draw("AntiKt_Truth_r04.get_pt()>>h_bh",
//      "AntiKt_Truth_r04.get_pt()>25 && abs(AntiKt_Truth_r04.get_eta())<0.6 && AntiKt_Truth_r04.get_property(1010)==5",
//      "goff");

//  T->Draw("AntiKt_Truth_r04.get_pt()>>h_bh5",
//      "AntiKt_Truth_r04.get_pt()>25 && abs(AntiKt_Truth_r04.get_eta())<0.6 && AntiKt_Truth_r04.get_property(1010)==5 &&  AntiKt_Truth_r04.get_property(1011) * AntiKt_Truth_r04.get_pt() > 5",
//      "goff");
//
//  T->Draw("AntiKt_Truth_r04.get_pt()>>h_c",
//      "AntiKt_Truth_r04.get_pt()>25 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1000))==4",
//      "goff");

  Convert2CrossSection(hall , int_lumi, dy);
  Convert2CrossSection(h_b , int_lumi, dy);
  Convert2CrossSection(h_bq , int_lumi, dy);

  TCanvas *c1 = new TCanvas("Draw_HFJetTruth" ,
      "Draw_HFJetTruth" , 1000, 960);
//  c1->Divide(2, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  hall->Draw();
  h_b->Draw("same");
  h_bq->Draw("same");
}

void
Convert2CrossSection(TH1* h, const double int_lumi, const double dy)
{
  h->Sumw2();

  for (int i = 1; i<= h->GetXaxis()->GetNbins(); ++i)
    {
      const double pT1 = h->GetXaxis()->GetBinLowEdge(i);
      const double pT2 = h->GetXaxis()->GetBinUpEdge(i);

//      const double dpT2 =  pT2*pT2 - pT1*pT1;
      const double dpT =  pT2 - pT1;

//      const double count2sigma = 1./dpT2/dy/int_lumi;
      const double count2sigma = 1./dpT/dy/int_lumi;

      h->SetBinContent(i, h->GetBinContent(i) * count2sigma);
      h->SetBinError(i, h->GetBinError(i) * count2sigma);
    }

//  h->GetYaxis()->SetTitle("#sigma/(dp_{T}^{2} dy) (pb/(GeV/c)^{2})");
  h->GetYaxis()->SetTitle("#sigma/(dp_{T} dy) (pb/(GeV/c))");

  h->SetMarkerStyle(kFullCircle);
}

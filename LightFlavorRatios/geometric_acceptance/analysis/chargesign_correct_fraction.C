#include "TChain.h"
#include "TGraphAsymmErrors.h"

void chargesign_correct_fraction()
{
  TChain* c = new TChain("ntp_gtrack");
  c->Add("/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/evaluator/outputeval_00000*.root");

  TH1F* all_pt_lambda = new TH1F("all_lambda","all_lambda",100,0.,4.);
  TH1F* correctsign_pt_lambda = new TH1F("correctsign_lambda","correctsign_lambda",100,0.,4.);

  TH1F* all_pt_kshort = new TH1F("all_kshort","all_kshort",100,0.1,4.);
  TH1F* correctsign_pt_kshort = new TH1F("correctsign_kshort","correctsign_kshort",100,0.1,4.);

//  c->Draw("gpt>>all_lambda","!std::isnan(trackID) && (fabs(gflavor)==211 || fabs(gflavor)==2212) && fabs(gparentflavor)==3122");
//  c->Draw("gpt>>correctsign_lambda","(charge*gflavor)>0. && (fabs(gflavor)==211 || fabs(gflavor)==2212) && fabs(gparentflavor)==3122");

  c->Draw("gpt>>all_kshort","!std::isnan(trackID) && fabs(gflavor)==211 && fabs(gparentflavor)==310");
  c->Draw("gpt>>correctsign_kshort","(charge*gflavor)>0. && fabs(gflavor)==211 && fabs(gparentflavor)==310");

//  TCanvas* l = new TCanvas("l","l",800,800);
//  TGraphAsymmErrors* frac_lambda = new TGraphAsymmErrors(correctsign_pt_lambda,all_pt_lambda);
//  frac_lambda->SetTitle("(#Lambda+#bar{#Lambda}) daughter correct charge sign fraction;gpt");
//  frac_lambda->Draw("AL");

  TCanvas* k = new TCanvas("k","k",800,800);
  TGraphAsymmErrors* frac_kshort = new TGraphAsymmErrors(correctsign_pt_kshort,all_pt_kshort);
  frac_kshort->SetTitle("K_{S}^{0} daughter correct charge sign fraction;gpt");
  frac_kshort->Draw("AL");
}

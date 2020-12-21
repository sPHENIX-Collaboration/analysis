// $Id: $

/*!
 * \file Draw_HFJetTruth_InvMass.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TLine.h>
#include <TString.h>
#include <TTree.h>
#include <cassert>
#include <cmath>
#include "SaveCanvas.C"
#include "sPhenixStyle.C"

TFile *_file0 = NULL;
TTree *T = NULL;

void Draw_HFJetTruth_InvMass(const TString infile =
                                 //                         "/sphenix/user/jinhuang/HF-jet/event_gen/200pp_pythia8_CTEQ6L_20GeV/200pp_pythia8_CTEQ6L_20GeV_ALL.cfg_eneg_DSTReader.root",
                             //                     double int_lumi = 210715 / 5.533e-05 / 1e9, const double dy = 0.6 * 2)

                             "/sphenix/user/jinhuang/HF-jet/event_gen/200pp_pythia8_CTEQ6L_7GeV/200pp_pythia8_CTEQ6L_7GeV_ALL.cfg_eneg_DSTReader.root",
                             double int_lumi = 891093 / 3.332e-02 / 1e9, const double dy = 0.6 * 2)

//"/sphenix/user/jinhuang/HF-jet/event_gen/200pp_pythia8_CTEQ6L/200pp_pythia8_CTEQ6L_111ALL.cfg_eneg_DSTReader.root",
//double int_lumi = 789908/4.631e-03 / 1e9, const double dy = 0.6*2)

//Draw_HFJetTruth_InvMass(const TString infile =
//    "../macros3/G4sPHENIXCells.root_DSTReader.root",
//    double int_lumi = 789908/4.631e-03 / 1e9, const double dy = 0.6*2)
//Draw_HFJetTruth_InvMass(const TString infile =
//    "../macros2/G4sPHENIXCells.root_DSTReader.root",
//    const double int_lumi = 1842152 / 5.801e-03 / 1e9,
//    const double dy = 0.6 * 2)
{
  SetsPhenixStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libg4dst.so");

  if (!_file0)
  {
    TString chian_str = infile;
    chian_str.ReplaceAll("ALL", "*");

    TChain *t = new TChain("T");
    const int n = t->Add(chian_str);

    int_lumi *= n;
    //      cout << "Loaded " << n << " root files with " << chian_str << endl;
    cout << "int_lumi = " << int_lumi << " pb^-1 from " << n << " root files with " << chian_str << endl;
    assert(n > 0);

    T = t;

    _file0 = new TFile;
    _file0->SetName(infile);
  }

  // step1: get the cross section
  //    DrawCrossSection(int_lumi, dy);

  // step2: ploting

  // Draw_HFJetTruth_InvMass_DrawCrossSection_PR(infile);
  //  CrossSection2RAA_Proposal(infile);
  //      CrossSection2RAA(infile);
  const double acceptance1 = 2. * (1.1 - .4);
  // CrossSection2RAA(infile, false, acceptance1);
  CrossSection2RAA(infile, false, acceptance1, true);

  //  const double acceptance2 = 2.* (0.85 - .4);
  //  CrossSection2RAA(infile, false, acceptance2);

  //  CrossSection2v2(infile, false, .7, acceptance);
  //  CrossSection2v2(infile, false, .4, acceptance);
}

void DrawCrossSection(double int_lumi, const double dy)
{
  TCut basicDijetEventQA("(AntiKt_Truth_r04[n_AntiKt_Truth_r04-1].get_pt())>15 && (AntiKt_Truth_r04[n_AntiKt_Truth_r04-2].get_pt())>10 && abs(AntiKt_Truth_r04[n_AntiKt_Truth_r04-1].get_eta())<.6 && abs(AntiKt_Truth_r04[n_AntiKt_Truth_r04-2].get_eta())<.6 && cos(AntiKt_Truth_r04[n_AntiKt_Truth_r04-1].get_phi() - AntiKt_Truth_r04[n_AntiKt_Truth_r04-2].get_phi())<-.5");

  cout << "Build event selection of " << (const char *) basicDijetEventQA << endl;

  T->Draw(">>EventList", basicDijetEventQA);
  TEventList *elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  assert(elist);
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected" << endl;
  T->SetEventList(elist);

  T->SetAlias("DiJetInvMass", "sqrt( (AntiKt_Truth_r04[n_AntiKt_Truth_r04-1].get_e() + AntiKt_Truth_r04[n_AntiKt_Truth_r04-2].get_e())**2 - (AntiKt_Truth_r04[n_AntiKt_Truth_r04-1].get_px() + AntiKt_Truth_r04[n_AntiKt_Truth_r04-2].get_px())**2 - (AntiKt_Truth_r04[n_AntiKt_Truth_r04-1].get_py() + AntiKt_Truth_r04[n_AntiKt_Truth_r04-2].get_py())**2 - (AntiKt_Truth_r04[n_AntiKt_Truth_r04-1].get_pz() + AntiKt_Truth_r04[n_AntiKt_Truth_r04-2].get_pz())**2  )");

  TH1 *hall = new TH1F("hall", ";m_{12} [GeV]", 200, 0, 200);
  TH1 *h_b = new TH1F("h_b", ";m_{12} [GeV]", 200, 0, 200);
  TH1 *h_bq = new TH1F("h_bq", ";m_{12} [GeV]", 200, 0, 200);
  TH1 *h_bh = new TH1F("h_bh", ";m_{12} [GeV]", 200, 0, 200);
  TH1 *h_bh5 = new TH1F("h_bh5", ";m_{12} [GeV]", 200, 0, 200);
  TH1 *h_ch5 = new TH1F("h_ch5", ";m_{12} [GeV]", 200, 0, 200);
  TH1 *h_c = new TH1F("h_c", ";m_{12} [GeV]", 200, 0, 200);

  cout << "DiJetInvMass>>hall" << endl;
  T->Draw("DiJetInvMass>>hall",
          "",
          "goff");

  cout << "DiJetInvMass>>h_b" << endl;
  T->Draw("DiJetInvMass>>h_b",
          "abs(AntiKt_Truth_r04[n_AntiKt_Truth_r04-1].get_property(1000))==5 && abs(AntiKt_Truth_r04[n_AntiKt_Truth_r04-2].get_property(1000))==5",
          "goff");
  //  T->Draw(
  //      "AntiKt_Truth_r04.get_pt()* AntiKt_Truth_r04.get_property(1001) >>h_bq",
  //      "AntiKt_Truth_r04.get_pt()>15 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1000))==5",
  //      "goff");

  //    T->Draw("AntiKt_Truth_r04.get_pt()>>h_bh",
  //        "AntiKt_Truth_r04.get_pt()>15 && abs(AntiKt_Truth_r04.get_eta())<0.6 && AntiKt_Truth_r04.get_property(1010)==5",
  //        "goff");

  //  T->Draw("AntiKt_Truth_r04.get_pt()>>h_bh5",
  //          "AntiKt_Truth_r04.get_pt()>15 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1010))==5 &&  AntiKt_Truth_r04.get_property(1011) * AntiKt_Truth_r04.get_pt() > 5",
  //          "goff");
  //
  //  T->Draw("AntiKt_Truth_r04.get_pt()>>h_c",
  //          "AntiKt_Truth_r04.get_pt()>15 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1000))==4",
  //          "goff");
  //  T->Draw("AntiKt_Truth_r04.get_pt()>>h_ch5",
  //          "AntiKt_Truth_r04.get_pt()>15 && abs(AntiKt_Truth_r04.get_eta())<0.6 && abs(AntiKt_Truth_r04.get_property(1010))==4 &&  AntiKt_Truth_r04.get_property(1011) * AntiKt_Truth_r04.get_pt() > 5",
  //          "goff");

  Convert2CrossSection(hall, int_lumi, dy);
  Convert2CrossSection(h_b, int_lumi, dy);
  //  Convert2CrossSection(h_bq, int_lumi, dy);
  //  Convert2CrossSection(h_bh5, int_lumi, dy);
  //  Convert2CrossSection(h_ch5, int_lumi, dy);
  //  Convert2CrossSection(h_c, int_lumi, dy);

  h_b->SetLineColor(kBlue);
  h_b->SetMarkerColor(kBlue);

  h_bq->SetLineColor(kBlue + 3);
  h_bq->SetMarkerColor(kBlue + 3);

  h_bh5->SetLineColor(kBlue + 3);
  h_bh5->SetMarkerColor(kBlue + 3);
  h_bh5->SetMarkerStyle(kStar);

  h_c->SetLineColor(kRed);
  h_c->SetMarkerColor(kRed);

  h_ch5->SetLineColor(kRed + 3);
  h_ch5->SetMarkerColor(kRed + 3);
  h_ch5->SetMarkerStyle(kStar);

  //  TGraphAsymmErrors *gr_fonll_b = GetFONLL_B();
  //  gr_fonll_b->SetFillColor(kBlue - 7);
  //  gr_fonll_b->SetFillStyle(3002);
  //  TGraphAsymmErrors *gr_fonll_c = GetFONLL_C();
  //  gr_fonll_c->SetFillColor(kRed - 7);
  //  gr_fonll_c->SetFillStyle(3003);
  //  TGraph *gr_phenix = GetPHENIX_jet();
  //  gr_phenix->SetLineColor(kGray + 2);
  //  gr_phenix->SetMarkerColor(kBlack);
  //  gr_phenix->SetMarkerStyle(kOpenCross);

  TCanvas *c1 = new TCanvas("Draw_HFJetTruth_InvMass_DrawCrossSection", "Draw_HFJetTruth_InvMass_DrawCrossSection", 1000, 860);
  //  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetGridx(0);
  p->SetGridy(0);
  p->SetLogy();

  hall->Draw();

  h_b->Draw("same");
  //  h_bh5->Draw("same");
  //  h_bq->Draw("same");
  //  h_c->Draw("same");
  //  h_ch5->Draw("same");

  //  hall->GetXaxis()->SetRangeUser(30, 160);
  hall->GetYaxis()->SetTitle(
      "d^{3}#sigma/(d#eta_{1}d#eta_{2}dm_{12}) [pb/(GeV)]");

  TLegend *leg = new TLegend(0.45, 0.6, 0.95, 0.93);
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(1001);
  leg->SetHeader("#splitline{#it{#bf{sPHENIX}} fast simulation, #it{p}+#it{p} #sqrt{s} = 200 GeV}{|#eta_{1,2}|<0.6, |#Delta#phi_{1,2}|>2#pi/3, p_{T,1}>15 GeV/c, p_{T,2}>10 GeV/c}");
  leg->AddEntry(hall, "Inclusive jet, PYTHIA8, Truth, anti-k_{t}, R=0.4",
                "lpe");
  //  leg->AddEntry(h_c, "c-quark jet, Pythia8, Truth, anti-k_{t}, R=0.4", "lpe");
  leg->AddEntry(h_b, "b-quark jet, PYTHIA8, Truth, anti-k_{t}, R=0.4", "lpe");
  //  leg->AddEntry(h_bq, "Leading b-quark in jet, Pythia8, anti-k_{t}, R=0.4", "lpe");
  //  leg->AddEntry(h_bh5,
  //                "b-hadron jet, Pythia8, Truth, anti-k_{t}, R=0.4, p_{T, b-hadron}>5 GeV/c",
  //                "lpe");
  //  leg->AddEntry(gr_phenix, "PHENIX inclusive jet, PRL 116, 122301 (2016)", "ple");
  //  leg->AddEntry(gr_fonll_c, "c-quark, FONLL v1.3.2, CTEQ6.6, |y|<0.6", "f");
  //  leg->AddEntry(gr_fonll_b, "b-quark, FONLL v1.3.2, CTEQ6.6, |y|<0.6", "f");
  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void Draw_HFJetTruth_InvMass_DrawCrossSection_PR(const TString infile)
{
  TFile *f = TFile::Open(infile + "Draw_HFJetTruth_InvMass_DrawCrossSection.root");
  assert(f);

  TH1F *hall = (TH1F *) f->GetObjectChecked("hall", "TH1F");
  assert(hall);
  TH1F *h_b = (TH1F *) f->GetObjectChecked("h_b", "TH1F");
  assert(h_b);

  hall->SetMarkerColor(kBlack);
  hall->SetLineColor(kBlack);
  hall->SetMarkerStyle(kFullCircle);

  h_b->SetMarkerColor(kBlue + 2);
  h_b->SetLineColor(kBlue + 2);
  h_b->SetMarkerStyle(kFullCircle);

  TGraph *gr_star = GetSTAR2019();
  gr_star->SetLineColor(kGray + 2);
  gr_star->SetLineWidth(3);
  gr_star->SetMarkerColor(kGray + 2);
  gr_star->SetMarkerSize(2);
  gr_star->SetMarkerStyle(kFullSquare);

  TCanvas *c1 = new TCanvas("Draw_HFJetTruth_InvMass_DrawCrossSection_PR", "Draw_HFJetTruth_InvMass_DrawCrossSection_PR", 1000, 860);
  //  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 *hframe = p->DrawFrame(35, 1e-2, 140, 1e6);
  hframe->SetTitle(";m_{12} [GeV/c^{2}];d^{3}#sigma/(d#eta_{1}d#eta_{2}dm_{12}) [pb/(GeV/c^{2})]");


  gr_star->Draw("pe");
  hall->Draw("same");
  h_b->Draw("same");

  TLegend *leg = new TLegend(0.2, 0.7, 0.95, 0.92);
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(1001);
  //  leg->SetHeader("#splitline{#it{#bf{sPHENIX }} Simulation}{#it{p}+#it{p}, #sqrt{s} = 200 GeV, |#eta|<0.6}");
  leg->SetHeader("#splitline{#it{#bf{sPHENIX}} fast simulation, #it{p}+#it{p} #sqrt{s} = 200 GeV}{|#eta_{1,2}|<0.6, |#Delta#phi_{1,2}|>2#pi/3, p_{T,1}>15 GeV/c, p_{T,2}>10 GeV/c}");
  leg->AddEntry("", "",
                "");
  leg->AddEntry(hall, "Inclusive jet, PYTHIA8 + CTEQ6L, anti-k_{T} R=0.4",
                "lpe");
  leg->AddEntry(gr_star, "STAR, PRD95 (2017) no.7, 071103, R=0.6", "ple");
  leg->AddEntry(h_b, "#it{b}-quark jet, PYTHIA8 + CTEQ6L, anti-k_{T} R=0.4", "lpe");

  leg->Draw();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void CrossSection2RAA(const TString infile, const bool use_AA_jet_trigger = true, const double dy = .7 * 2, const bool b3yr = false)
{
  TFile *f = TFile::Open(infile + "Draw_HFJetTruth_InvMass_DrawCrossSection.root");
  assert(f);

  TH1F *hall = (TH1F *) f->GetObjectChecked("hall", "TH1F");
  assert(hall);
  TH1F *h_b = (TH1F *) f->GetObjectChecked("h_b", "TH1F");
  assert(h_b);

  TString s_suffix(use_AA_jet_trigger ? "_AAJetTriggered" : "");
  s_suffix += b3yr ? "_3yr" : "";
  s_suffix += Form("_deta%.2f", dy / 2);

  const double b_jet_RAA = 0.4;
  const double target_RAA_ratio = 4;;

  const double pp_eff = 0.6;
  const double pp_purity = 0.4;
  const double AuAu_eff = 0.4;
  const double AuAu_purity = 0.4;

  ////////////////////////////
  // 5-year lumi in [sPH-TRG-000]
  ////////////////////////////

  const double pp_lumi = b3yr ? 62 : 200;                          // pb^-1 [sPH-TRG-000], rounded up from 197 pb^-1
  const double pp_inelastic_crosssec = 42e-3 / 1e-12;  // 42 mb in pb [sPH-TRG-000]

  const double AuAu_MB_Evt = use_AA_jet_trigger ? (b3yr ? 320e9 : 550e9) : (b3yr ? 142e9 : 240e9);  // [sPH-TRG-000], depending on whether jet trigger applied in AA collisions
  const double pAu_MB_Evt = 600e9;                                // [sPH-TRG-000]

  const double AuAu_Ncoll_C0_10 = 960.2;  // [DOI:?10.1103/PhysRevC.87.034911?]
  const double AuAu_Ncoll_C0_20 = 770.6;  // [DOI:?10.1103/PhysRevC.91.064904?]
  const double AuAu_Ncoll_C0_100 = 250;   // pb^-1 [sPH-TRG-000]
  const double pAu_Ncoll_C0_100 = 4.7;    // pb^-1 [sPH-TRG-000]

  const double AuAu_eq_lumi_C0_10 = AuAu_MB_Evt * 0.1 * AuAu_Ncoll_C0_10 / pp_inelastic_crosssec;  //
  const double AuAu_eq_lumi_C0_20 = AuAu_MB_Evt * 0.2 * AuAu_Ncoll_C0_20 / pp_inelastic_crosssec;  //
  const double AuAu_eq_lumi_C0_100 = AuAu_MB_Evt * 1 * AuAu_Ncoll_C0_100 / pp_inelastic_crosssec;  //

  const double pAu_eq_lumi_C0_100 = pAu_MB_Evt * 1 * pAu_Ncoll_C0_100 / pp_inelastic_crosssec;  //

  cout << "CrossSection2RAA integrated luminosity assumptions in pb^-1: " << endl;
  cout << "\t"
       << "pp_lumi = " << pp_lumi << endl;
  cout << "\t"
       << "AuAu_eq_lumi_C0_10 = " << AuAu_eq_lumi_C0_10 << endl;
  cout << "\t"
       << "AuAu_eq_lumi_C0_20 = " << AuAu_eq_lumi_C0_20 << endl;
  cout << "\t"
       << "AuAu_eq_lumi_C0_100 = " << AuAu_eq_lumi_C0_100 << endl;
  cout << "\t"
       << "pAu_eq_lumi_C0_100 = " << pAu_eq_lumi_C0_100 << endl;

  TCanvas *c1 = new TCanvas("Draw_HFJetTruth_InvMass_CrossSection2RAA_Ratio" + s_suffix, "Draw_HFJetTruth_InvMass_CrossSection2RAA_Ratio" + s_suffix, 1100, 800);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetGridx(0);
  //  p->SetGridy(0);
  //  p->SetLogy();

  // in sPH-HF-2017-001, dijet purity efficiency was conservatively assumed to be the product of that of two single b-jets,
  // i.e. without benifit of the enhanace of 2nd b-jet abundance after the 1st jet in the event is tagged as a b-jet
  TH1 *g_pp = CrossSection2RelUncert(h_b, 1., dy, pp_lumi * pp_eff * pp_purity * pp_eff * pp_purity);
  TH1 *g_AA = CrossSection2RelUncert(h_b, b_jet_RAA, dy, AuAu_eq_lumi_C0_10 * AuAu_eff * AuAu_purity * AuAu_eff * AuAu_purity);

  g_pp->SetLineColor(kRed);
  g_AA->SetLineColor(kBlue);

  g_pp->Draw();
  g_AA->Draw("same");
  SaveCanvas(c1, infile + "_" + TString(c1->GetName()), kTRUE);

  TCanvas *c1 = new TCanvas("Draw_HFJetTruth_InvMass_CrossSection2RAA" + s_suffix, "Draw_HFJetTruth_InvMass_CrossSection2RAA" + s_suffix, 1100, 800);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->DrawFrame(30, 0, 75, 1.3)->SetTitle(";Di-jet invariant mass [GeV/#it{c}^{2}];#it{R}^{bb}_{AA}");

  TGraphErrors *ge_RAA = GetRAA(g_pp, g_AA);

  ge_RAA->SetLineWidth(4);
  ge_RAA->SetMarkerStyle(kFullCircle);
  ge_RAA->SetMarkerSize(3);

  ge_RAA->Draw("pe");
  ge_RAA->Print();

  TLegend *leg = new TLegend(.0, .7, .85, .93);
  leg->SetFillStyle(0);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Projection, #it{b}-jet, 0-10% Au+Au, Year 1-3", "");
  leg->AddEntry("", Form("|#eta_{1,2}|<%.1f, |#Delta#phi_{1,2}|>2#pi/3, p_{T,1}>15 GeV/c, p_{T,2}>10 GeV/c", dy / 2), "");
  leg->AddEntry("", Form("#it{p}+#it{p}: %.0f pb^{-1} samp., %.0f%% Eff., %.0f%% Pur.", pp_lumi, pp_eff * 100, pp_purity * 100), "");
  leg->AddEntry("", Form("Au+Au: %.0fnb^{-1} rec., %.0f%% Eff., %.0f%% Pur.", '%', AuAu_MB_Evt/6.8252 / 1e9, AuAu_eff * 100, AuAu_purity * 100), "");
  leg->Draw();

//  TLegend *leg2 = new TLegend(.17, .70, .88, .77);
//  leg2->AddEntry(ge_RAA, "#it{b}-jet #it{R}_{AA}, Au+Au 0-10%C, #sqrt{s_{NN}}=200 GeV", "pl");
//  leg2->Draw();

  SaveCanvas(c1, infile + "_" + TString(c1->GetName()), kTRUE);

  TCanvas *c1 = new TCanvas("Draw_HFJetTruth_InvMass_CrossSection2RAA_Theory" + s_suffix, "Draw_HFJetTruth_InvMass_CrossSection2RAA_Theory" + s_suffix, 1100, 800);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->DrawFrame(35, 0, 75, 1.4)->SetTitle(";Di-jet invariant mass [GeV];#it{R}^{bb}_{AA}");

  //
  TGraph *g18 = GetRAAKang2019(1, 1.8);
  TGraph *g20 = GetRAAKang2019(1, 2.0);
  TGraph *g22 = GetRAAKang2019(1, 2.2);
  //
  g18->SetLineColor(kBlue + 3);
  g20->SetLineColor(kGreen + 3);
  g22->SetLineColor(kRed + 3);
  g18->SetLineWidth(3);
  g20->SetLineWidth(3);
  g22->SetLineWidth(3);
  //
  g18->Draw("l");
  g20->Draw("l");
  g22->Draw("l");
  //
  ge_RAA->DrawClone("pe");
  leg->DrawClone();
//  leg2->DrawClone();
  //
  TLegend *leg1 = new TLegend(.19, .6, .92, .7);
//  leg1->AddEntry("", "Kang et al, PRD #bf{99}, 034006 (2019), m=m_{b}, rad.+col.", "");
  leg1->SetHeader("Kang et al, PRD #bf{99}, 034006 (2019), m=m_{b}, rad.+col.");
  leg1->Draw();
  TLegend *leg11 = new TLegend(.35, .5, .55, .62);
  leg11->AddEntry(g18, "g_{med} = 1.8", "l");
  leg11->AddEntry(g20, "g_{med} = 2.0", "l");
  leg11->AddEntry(g22, "g_{med} = 2.2", "l");
  leg11->Draw();

  SaveCanvas(c1, infile + "_" + TString(c1->GetName()), kTRUE);


  TCanvas *c1 = new TCanvas("Draw_HFJetTruth_InvMass_CrossSection2RAARatio_Theory" + s_suffix, "Draw_HFJetTruth_InvMass_CrossSection2RAARatio_Theory" + s_suffix, 1100, 800);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->DrawFrame(35, 0, 75, 15)->SetTitle(";Di-jet invariant mass [GeV];#it{R}^{bb}_{AA}/#it{R}^{jj}_{AA}");
  TLine * l = new TLine(35, 1, 75, 1);
//  l->SetLineStyle(kDashed);
  l->Draw();

  //
  TGraph *g18 = GetRAARatioKang2019(1, 1.8);
  TGraph *g20 = GetRAARatioKang2019(1, 2.0);
  TGraph *g22 = GetRAARatioKang2019(1, 2.2);
  //
  g18->SetLineColor(kBlue + 3);
  g20->SetLineColor(kGreen + 3);
  g22->SetLineColor(kRed + 3);
  g18->SetLineWidth(3);
  g20->SetLineWidth(3);
  g22->SetLineWidth(3);
  //
  g18->Draw("l");
  g20->Draw("l");
  g22->Draw("l");
  //
  TGraphErrors * ge_RAARatio = (TGraphErrors *) ge_RAA->DrawClone("pe");
  assert(ge_RAARatio);
  for (int bin = 0; bin<ge_RAARatio->GetN();++bin)
  {
    ge_RAARatio->GetY()[bin] *= target_RAA_ratio/b_jet_RAA;
    ge_RAARatio->GetEY()[bin] *= target_RAA_ratio/b_jet_RAA;
  }

  leg->DrawClone();
//  leg2->DrawClone();
  //
  TLegend *leg1 = new TLegend(.2, .6, .92, .70);
//  leg1->AddEntry("", "Kang et al, PRD #bf{99}, 034006 (2019), m=m_{b}, rad.+col.", "");
  leg1->SetHeader("Kang et al, PRD #bf{99}, 034006 (2019), m=m_{b}, rad.+col.");
  leg1->Draw();
  TLegend *leg11 = new TLegend(.35, .5, .55, .62);
  leg11->AddEntry(g18, "g_{med} = 1.8", "l");
  leg11->AddEntry(g20, "g_{med} = 2.0", "l");
  leg11->AddEntry(g22, "g_{med} = 2.2", "l");
  leg11->Draw();

  SaveCanvas(c1, infile + "_" + TString(c1->GetName()), kTRUE);


}

TGraphErrors *GetRAA(TH1 *h_pp, TH1 *h_AA)
{
  int n_bin = 0;

  double xs[1000] = {0};
  double ys[1000] = {0};
  double eys[1000] = {0};

  assert(h_pp);
  assert(h_AA);
  assert(h_pp->GetNbinsX() == h_AA->GetNbinsX());

  for (int i = 1; i <= h_pp->GetNbinsX(); ++i)
  {
    if (h_pp->GetBinError(i) > 0 && h_pp->GetBinError(i) < 1 && h_AA->GetBinError(i) > 0 && h_AA->GetBinError(i) < 1)
    {
      xs[n_bin] = h_pp->GetXaxis()->GetBinCenter(i);

      ys[n_bin] = h_AA->GetBinContent(i) / h_pp->GetBinContent(i);

      eys[n_bin] = ys[n_bin] * sqrt(pow(h_AA->GetBinError(i) / h_AA->GetBinContent(i), 2) + pow(h_pp->GetBinError(i) / h_pp->GetBinContent(i), 2));

      // final uncertainty cut
      if (eys[n_bin] < .4)
        n_bin += 1;
    }
  }

  TGraphErrors *ge = new TGraphErrors(n_bin, xs, ys, NULL, eys);
  ge->SetName(TString("RAA_") + h_AA->GetName());

  return ge;
}

TH1 *CrossSection2RelUncert(const TH1F *h_cross,
                            const double suppression,
                            const double deta,
                            const double pp_quiv_int_lum)
{
  assert(h_cross);
  TH1 *
      h_ratio = (TH1 *)
                    h_cross->Clone(TString(h_cross->GetName()) + Form("_copy%d", rand()));

  //convert to count per bin
  h_ratio->Scale(deta * deta * h_ratio->GetXaxis()->GetBinWidth(0) * pp_quiv_int_lum * suppression);

  // define the x-binning
  h_ratio->Rebin(5);

  for (int i = 1; i <= h_ratio->GetNbinsX(); ++i)
  {
    const double yield = h_ratio->GetBinContent(i);

    if (yield > 0)
    {
      h_ratio->SetBinContent(i, suppression);

      h_ratio->SetBinError(i, suppression / sqrt(yield));
    }
    else
    {
      h_ratio->SetBinContent(i, 0);

      h_ratio->SetBinError(i, 0);
    }
  }

  h_ratio->GetYaxis()->SetTitle("Relative Cross Section and Uncertainty");

  return h_ratio;
}

void Convert2CrossSection(TH1 *h, const double int_lumi, const double dy)
{
  h->Sumw2();

  for (int i = 1; i <= h->GetXaxis()->GetNbins(); ++i)
  {
    const double m1 = h->GetXaxis()->GetBinLowEdge(i);
    const double m2 = h->GetXaxis()->GetBinUpEdge(i);

    //      const double dpT2 =  pT2*pT2 - pT1*pT1;
    const double dm = m2 - m1;

    //      const double count2sigma = 1./dpT2/dy/int_lumi;
    const double count2sigma = 1. / dm / int_lumi / dy / dy;

    h->SetBinContent(i, h->GetBinContent(i) * count2sigma);
    h->SetBinError(i, h->GetBinError(i) * count2sigma);
  }

  //  h->GetYaxis()->SetTitle("#sigma/(dp_{T}^{2} dy) (pb/(GeV/c)^{2})");

  h->SetMarkerStyle(kFullCircle);
}

TGraphAsymmErrors *GetSTAR2019(const TString hepdata = "/sphenix/user/jinhuang/HF-jet/event_gen/HEPData-ins1493842-v1-Table_4.root")
{
  ////  https://doi.org/10.17182/hepdata.77208.v1/t4
  //  Measurement of the cross section and longitudinal double-spin asymmetry for di-jet production in polarized pp collisions at s = 200 GeV
  //
  //  The STAR collaboration
  //  Adamczyk, L. , Adkins, J.K. , Agakishiev, G. , Aggarwal, M.M. , Ahammed, Z. , Alekseev, I. , Anderson, D.M. , Aoyama, R. , Aparin, A. , Arkhipkin, D.
  //  No Journal Information, 2016
  //  https://doi.org/10.17182/hepdata.77208

  TFile *_file0 = TFile::Open(hepdata);
  _file0->cd("Table 4");
  TGraphAsymmErrors *gae = (TGraphAsymmErrors *)
                               gDirectory->GetObjectChecked("Graph1D_y1", "TGraphAsymmErrors");
  assert(gae);

  for (int bin = 0; bin < gae->GetN(); ++bin)
  {
    (gae->GetY())[bin] *= 1e6;                                // ub -> pb
    gae->SetPointEYhigh(bin, gae->GetErrorYhigh(bin) * 1e6);  // ub -> pb
    gae->SetPointEYlow(bin, gae->GetErrorYlow(bin) * 1e6);    // ub -> pb
  }
  gae->SetName("gaeSTARDiJetMass");

  return gae;
}

TGraph *GetRAAKang2019(const int mass = 1, const double g = 1.8)
{
  //  m12    2mb,g=1.8    1mb,g=1.8    2mb,g=2.0    1mb,g=2.0    2mb,g=2.2    1mb,g=2.2
  //  17     0.959318     0.813631     0.937975     0.718446     0.904169     0.583889
  //  19     0.924265     0.734466     0.887045     0.618448     0.831249     0.469604
  //  21     0.889933     0.676922     0.838959     0.550413     0.765768     0.397484
  //  23     0.852022     0.624044      0.78657     0.489034     0.696008     0.335986
  //  25     0.813713     0.572249     0.736696     0.435365     0.634859     0.287682
  //  27     0.829553     0.588906     0.752964     0.444663     0.647904     0.287249
  //  29     0.787303     0.554276     0.702613     0.412568     0.592341      0.26188
  //  31     0.767304     0.519823     0.674582     0.374897     0.555677      0.22774
  //  33     0.774209     0.531997      0.68111     0.383097     0.559728       0.2299
  //  35     0.733919     0.503672     0.633725     0.359385     0.509993     0.213934
  //  37     0.763963     0.528822     0.665819     0.373186     0.537606     0.214043
  //  39     0.741506     0.564112     0.645253     0.416396      0.52631     0.253036
  //  41      0.74574     0.497485     0.638774     0.336391     0.500389     0.179804
  //  43     0.725435     0.496459     0.617035     0.336753     0.479825     0.179778
  //  45     0.714991     0.472252     0.599281     0.309914     0.453767      0.15839
  //  47     0.680055     0.443568     0.559106     0.287112     0.413458     0.145234
  //  49     0.679489     0.450226     0.557737      0.28911     0.410027     0.142904
  //  51     0.652643     0.420073     0.525378     0.262171     0.375742     0.124363
  //  53     0.648852     0.421612     0.520426     0.260902     0.368284     0.121749
  //  55     0.659723     0.424185     0.522925     0.255923      0.36165     0.114938
  //  57     0.608096     0.394726     0.474039      0.23987     0.324512     0.109253
  //  59     0.599733     0.385379     0.462397     0.228792     0.309607     0.100507
  //  61     0.602495     0.398467     0.466165     0.238347     0.312724     0.103957
  //  63      0.58698     0.369703     0.441268      0.20886     0.281604    0.0844847
  //  65      0.58804      0.39896     0.449849     0.238734     0.296312     0.102839
  //  67     0.568857     0.369444     0.423411     0.208794     0.264683    0.0833302
  //  69     0.569308     0.374558     0.421737     0.210743     0.259612    0.0834087
  //  71     0.570937     0.379757     0.418615     0.213217     0.253412    0.0841172
  //  73     0.571426     0.368965     0.404248     0.201111     0.234995    0.0773808
  //  75     0.444747     0.285571     0.308362     0.153737      0.17623    0.0575338
  //  77     0.518565     0.353508     0.368773     0.198289     0.215024    0.0772062
  //  79      0.52314     0.354647      0.36598     0.194516     0.207028    0.0732502
  //  81     0.462486     0.306947     0.313368     0.163098     0.169892    0.0593907
  //  83     0.494343     0.338822     0.337503     0.182332     0.182904    0.0665247
  //  85     0.477053     0.321068     0.315372     0.167256     0.163585    0.0589566
  //  87     0.425877     0.287827     0.276778      0.14819     0.140167    0.0513555
  //  89     0.449382     0.310267     0.291343      0.16052     0.146033    0.0555645
  //  91     0.426898     0.294991     0.270933     0.150014     0.131644    0.0508301
  //  93      0.58802     0.451299     0.406374     0.248587     0.210967    0.0874347
  //  95     0.367013     0.245387     0.216773      0.11698    0.0963793    0.0374309
  //  97     0.363587     0.251356     0.213642     0.120293    0.0932188    0.0381195
  //  99     0.349846     0.240864     0.197647     0.111828    0.0824488    0.0345796
  //  101     0.294855      0.20189     0.158814    0.0907104    0.0634189    0.0274697
  //  103     0.264038      0.18211     0.137505    0.0801353    0.0531663    0.0239006

  const int n_Data = 44;

  const double m12[] = {
      17,
      19,
      21,
      23,
      25,
      27,
      29,
      31,
      33,
      35,
      37,
      39,
      41,
      43,
      45,
      47,
      49,
      51,
      53,
      55,
      57,
      59,
      61,
      63,
      65,
      67,
      69,
      71,
      73,
      75,
      77,
      79,
      81,
      83,
      85,
      87,
      89,
      91,
      93,
      95,
      97,
      99,
      101,
      103};

  const double RAA_2mb_g18[] = {
      0.959318,
      0.924265,
      0.889933,
      0.852022,
      0.813713,
      0.829553,
      0.787303,
      0.767304,
      0.774209,
      0.733919,
      0.763963,
      0.741506,
      0.74574,
      0.725435,
      0.714991,
      0.680055,
      0.679489,
      0.652643,
      0.648852,
      0.659723,
      0.608096,
      0.599733,
      0.602495,
      0.58698,
      0.58804,
      0.568857,
      0.569308,
      0.570937,
      0.571426,
      0.444747,
      0.518565,
      0.52314,
      0.462486,
      0.494343,
      0.477053,
      0.425877,
      0.449382,
      0.426898,
      0.58802,
      0.367013,
      0.363587,
      0.349846,
      0.294855,
      0.264038};

  const double RAA_1mb_g_18[] = {
      0.813631,
      0.734466,
      0.676922,
      0.624044,
      0.572249,
      0.588906,
      0.554276,
      0.519823,
      0.531997,
      0.503672,
      0.528822,
      0.564112,
      0.497485,
      0.496459,
      0.472252,
      0.443568,
      0.450226,
      0.420073,
      0.421612,
      0.424185,
      0.394726,
      0.385379,
      0.398467,
      0.369703,
      0.39896,
      0.369444,
      0.374558,
      0.379757,
      0.368965,
      0.285571,
      0.353508,
      0.354647,
      0.306947,
      0.338822,
      0.321068,
      0.287827,
      0.310267,
      0.294991,
      0.451299,
      0.245387,
      0.251356,
      0.240864,
      0.20189,
      0.18211};

  const double RAA_2mb_g_20[] = {
      0.937975,
      0.887045,
      0.838959,
      0.78657,
      0.736696,
      0.752964,
      0.702613,
      0.674582,
      0.68111,
      0.633725,
      0.665819,
      0.645253,
      0.638774,
      0.617035,
      0.599281,
      0.559106,
      0.557737,
      0.525378,
      0.520426,
      0.522925,
      0.474039,
      0.462397,
      0.466165,
      0.441268,
      0.449849,
      0.423411,
      0.421737,
      0.418615,
      0.404248,
      0.308362,
      0.368773,
      0.36598,
      0.313368,
      0.337503,
      0.315372,
      0.276778,
      0.291343,
      0.270933,
      0.406374,
      0.216773,
      0.213642,
      0.197647,
      0.158814,
      0.137505};

  const double RAA_1mb_g_20[] = {
      0.718446,
      0.618448,
      0.550413,
      0.489034,
      0.435365,
      0.444663,
      0.412568,
      0.374897,
      0.383097,
      0.359385,
      0.373186,
      0.416396,
      0.336391,
      0.336753,
      0.309914,
      0.287112,
      0.28911,
      0.262171,
      0.260902,
      0.255923,
      0.23987,
      0.228792,
      0.238347,
      0.20886,
      0.238734,
      0.208794,
      0.210743,
      0.213217,
      0.201111,
      0.153737,
      0.198289,
      0.194516,
      0.163098,
      0.182332,
      0.167256,
      0.14819,
      0.16052,
      0.150014,
      0.248587,
      0.11698,
      0.120293,
      0.111828,
      0.0907104,
      0.0801353};

  const double RAA_2mb_g_22[] = {
      0.904169,
      0.831249,
      0.765768,
      0.696008,
      0.634859,
      0.647904,
      0.592341,
      0.555677,
      0.559728,
      0.509993,
      0.537606,
      0.52631,
      0.500389,
      0.479825,
      0.453767,
      0.413458,
      0.410027,
      0.375742,
      0.368284,
      0.36165,
      0.324512,
      0.309607,
      0.312724,
      0.281604,
      0.296312,
      0.264683,
      0.259612,
      0.253412,
      0.234995,
      0.17623,
      0.215024,
      0.207028,
      0.169892,
      0.182904,
      0.163585,
      0.140167,
      0.146033,
      0.131644,
      0.210967,
      0.0963793,
      0.0932188,
      0.0824488,
      0.0634189,
      0.0531663};

  const double RAA_1mb_g_22[] = {
      0.583889,
      0.469604,
      0.397484,
      0.335986,
      0.287682,
      0.287249,
      0.26188,
      0.22774,
      0.2299,
      0.213934,
      0.214043,
      0.253036,
      0.179804,
      0.179778,
      0.15839,
      0.145234,
      0.142904,
      0.124363,
      0.121749,
      0.114938,
      0.109253,
      0.100507,
      0.103957,
      0.0844847,
      0.102839,
      0.0833302,
      0.0834087,
      0.0841172,
      0.0773808,
      0.0575338,
      0.0772062,
      0.0732502,
      0.0593907,
      0.0665247,
      0.0589566,
      0.0513555,
      0.0555645,
      0.0508301,
      0.0874347,
      0.0374309,
      0.0381195,
      0.0345796,
      0.0274697,
      0.0239006};

  TGraph *gRAAKang2019(NULL);

  if (mass == 1)
  {
    if (g == 1.8)
    {
      gRAAKang2019 = new TGraph(n_Data, m12, RAA_1mb_g_18);
    }
    else if (g == 2)
    {
      gRAAKang2019 = new TGraph(n_Data, m12, RAA_1mb_g_20);
    }
    else if (g == 2.2)
    {
      gRAAKang2019 = new TGraph(n_Data, m12, RAA_1mb_g_22);
    }
  }
  else if (mass == 2)
  {
    if (g == 1.8)
    {
      gRAAKang2019 = new TGraph(n_Data, m12, RAA_2mb_g_18);
    }
    else if (g == 2)
    {
      gRAAKang2019 = new TGraph(n_Data, m12, RAA_2mb_g_20);
    }
    else if (g == 2.2)
    {
      gRAAKang2019 = new TGraph(n_Data, m12, RAA_2mb_g_22);
    }
  }

  cout <<"mass = "<<mass<<", g="<<g<<endl;
  assert(gRAAKang2019);
  gRAAKang2019->Print();

  return gRAAKang2019;
}

TGraph *GetRAARatioKang2019(const int mass = 1, const double g = 1.8)
{
  //  m12     g=1.8        g=2.0        g=2.2
  //  17      4.10724      7.40351      16.6273
  //  19      4.44519      7.83383      18.0237
  //  21      4.57977       7.8564      17.5948
  //  23      4.29797      7.07926      15.2511
  //  25      3.58437      5.58706       11.445
  //  27      3.81735      5.93123      12.1819
  //  29      2.88061      4.20894      8.22601
  //  31       3.0618       4.4508      8.61545
  //  33      2.94732      4.17463      7.85741
  //  35      2.92894      4.20026       8.0066
  //  37      2.70991      3.76532      6.85238
  //  39      2.91857      4.25505      8.23559
  //  41      2.39766      3.14728      5.31672
  //  43      2.41189      3.19676      5.40647
  //  45       2.0471      2.56789      4.11909
  //  47      2.04743      2.57154      4.09681
  //  49      1.97205      2.43492       3.7728
  //  51      1.67187      1.96796      2.89214
  //  53      1.75599       2.0696      2.98919
  //  55      1.69526      1.94817      2.71746
  //  57      1.02735      1.09074      1.46593
  //  59      1.56319      1.76551      2.39532
  //  61      1.60436      1.82217      2.45074
  //  63      1.39696      1.47696      1.82356
  //  65      1.45008      1.62795      2.14264
  //  67      1.36533      1.43573      1.73491
  //  69      1.40584       1.4939      1.80942
  //  71      1.38037      1.45466      1.75179
  //  73      1.27568      1.28553      1.49002
  //  75      1.03127      1.04286      1.19033
  //  77      1.20987      1.25797      1.47642
  //  79      1.27784      1.31767      1.51129
  //  81      1.06928      1.06112       1.1719
  //  83      1.22018       1.2414        1.386
  //  85      1.15586      1.13797      1.23096
  //  87      1.00112     0.966856      1.02192
  //  89      1.07953      1.05446       1.1226
  //  91      1.08107      1.06196       1.1313
  //  93       1.6204      1.70796      1.87276
  //  95     0.905063     0.833268     0.839565
  //  97      0.94488     0.884202     0.892716
  //  99     0.956238     0.882098     0.880081
  //  101     0.855383     0.777236     0.768626
  //  103     0.832522     0.754264     0.742384

  const int n_Data = 44;

  const double m12[] = {
      17,
      19,
      21,
      23,
      25,
      27,
      29,
      31,
      33,
      35,
      37,
      39,
      41,
      43,
      45,
      47,
      49,
      51,
      53,
      55,
      57,
      59,
      61,
      63,
      65,
      67,
      69,
      71,
      73,
      75,
      77,
      79,
      81,
      83,
      85,
      87,
      89,
      91,
      93,
      95,
      97,
      99,
      101,
      103};

  const double RAA_1mb_g_18[] = {
      4.10724,
      4.44519,
      4.57977,
      4.29797,
      3.58437,
      3.81735,
      2.88061,
      3.0618,
      2.94732,
      2.92894,
      2.70991,
      2.91857,
      2.39766,
      2.41189,
      2.0471,
      2.04743,
      1.97205,
      1.67187,
      1.75599,
      1.69526,
      1.02735,
      1.56319,
      1.60436,
      1.39696,
      1.45008,
      1.36533,
      1.40584,
      1.38037,
      1.27568,
      1.03127,
      1.20987,
      1.27784,
      1.06928,
      1.22018,
      1.15586,
      1.00112,
      1.07953,
      1.08107,
      1.6204,
      0.905063,
      0.94488,
      0.956238,
      0.855383,
      0.832522};

  const double RAA_1mb_g_20[] = {
      7.40351,
      7.83383,
      7.8564,
      7.07926,
      5.58706,
      5.93123,
      4.20894,
      4.4508,
      4.17463,
      4.20026,
      3.76532,
      4.25505,
      3.14728,
      3.19676,
      2.56789,
      2.57154,
      2.43492,
      1.96796,
      2.0696,
      1.94817,
      1.09074,
      1.76551,
      1.82217,
      1.47696,
      1.62795,
      1.43573,
      1.4939,
      1.45466,
      1.28553,
      1.04286,
      1.25797,
      1.31767,
      1.06112,
      1.2414,
      1.13797,
      .966856,
      1.05446,
      1.06196,
      1.70796,
      .833268,
      .884202,
      .882098,
      0.777236,
      0.754264};

  const double RAA_1mb_g_22[] = {
      16.6273,
      18.0237,
      17.5948,
      15.2511,
      11.445,
      12.1819,
      8.22601,
      8.61545,
      7.85741,
      8.0066,
      6.85238,
      8.23559,
      5.31672,
      5.40647,
      4.11909,
      4.09681,
      3.7728,
      2.89214,
      2.98919,
      2.71746,
      1.46593,
      2.39532,
      2.45074,
      1.82356,
      2.14264,
      1.73491,
      1.80942,
      1.75179,
      1.49002,
      1.19033,
      1.47642,
      1.51129,
      1.1719,
      1.386,
      1.23096,
      1.02192,
      1.1226,
      1.1313,
      1.87276,
      0.839565,
      0.892716,
      0.880081,
      0.768626,
      0.742384};

  TGraph *gRAARatioKang2019(NULL);

  if (mass == 1)
  {
    if (g == 1.8)
    {
      gRAARatioKang2019 = new TGraph(n_Data, m12, RAA_1mb_g_18);
    }
    else if (g == 2)
    {
      gRAARatioKang2019 = new TGraph(n_Data, m12, RAA_1mb_g_20);
    }
    else if (g == 2.2)
    {
      gRAARatioKang2019 = new TGraph(n_Data, m12, RAA_1mb_g_22);
    }
  }

  assert(gRAARatioKang2019);

  return gRAARatioKang2019;
}

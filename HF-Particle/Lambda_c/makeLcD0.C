#include <stdio.h>
#include "iomanip.h"

void makeLcD0()
{
  gROOT->Reset();

  const Int_t NConfig = 4;
  const Int_t NMax = 5;
  const Int_t N[NConfig] = {3, 5, 4, 1}; // ALICE_pp, ALICE_pPb, STAR_AuAu
  const Char_t *ExpName[NConfig] = {"STAR","ALICE","ALICE","ALICE"};
  const Char_t *CollName[NConfig] = {"AuAu_10_80_200GeV","pp_7TeV","pPb_5p02TeV","PbPb_5p02TeV"};
  double x[NConfig][NMax], y[NConfig][NMax], ye[NConfig][NMax], yes_u[NConfig][NMax], yes_d[NConfig][NMax];
  TGraphErrors *gr[NConfig];

  // STAR data points
  ifstream inData;
  for(int i=0;i<NConfig;i++) {
    inData.open(Form("dat/%s_LcD0_%s.txt",ExpName[i],CollName[i]));
    for(int j=0;j<N[i];j++) {
      inData >> x[i][j] >> y[i][j] >> ye[i][j] >> yes_u[i][j] >> yes_d[i][j];
    }
    inData.close();
    gr[i] = new TGraphErrors(N[i], x[i], y[i], 0, ye[i]);
    cout << Form("dat/%s_LcD0_%s.txt",ExpName[i],CollName[i]) << endl;
    gr[i]->Print();
  }

  // STAR Central data
  double x_St[1] = {3.96};
  double y_St[1] = {1.3935};
  double ye_St[1] = {0.3112};
  double yes_St[1] = {0.3446};
  TGraphErrors *gr_St = new TGraphErrors(1, x_St, y_St, 0, ye_St);
  gr_St->SetMarkerStyle(29);
  gr_St->SetMarkerSize(2.5);
  gr_St->SetMarkerColor(2);
  gr_St->SetLineWidth(2);
  gr_St->SetLineColor(2);
  
  // Theory calculations
  // pythia6
  TFile *fin = new TFile("root/pythia_D_ratio.root");
  TGraphErrors *gr_pythia_tmp = (TGraphErrors *)fin->Get("Ratio_Lc_D0");
  TGraph *gr_pythia = new TGraph(*gr_pythia_tmp);
  gr_pythia->SetLineWidth(2);
  gr_pythia->SetLineColor(16);
  // pythia8
  TFile *fin = new TFile("root/PYTHIA8_D_ratio_200GeV_20181029_0.root");
  TDirectoryFile *fLc = (TDirectoryFile *)fin->Get("Lc");
  TH1D *h_pythia8 = (TH1D *)fLc->Get("LctoD0_pt_whole_rebin");
  TGraph *gr_pythia8 = new TGraph(h_pythia8);
  gr_pythia8->SetLineWidth(2);
  gr_pythia8->SetLineColor(1);
  gr_pythia8->SetLineStyle(2);

  TFile *fin = new TFile("root/PYTHIA8_D_ratio_200GeV_20181029_CR.root");
  TDirectoryFile *fLc = (TDirectoryFile *)fin->Get("Lc");
  TH1D *h_pythia8_CR = (TH1D *)fLc->Get("LctoD0_pt_whole_rebin");
  TGraph *gr_pythia8_CR = new TGraph(h_pythia8_CR);
  gr_pythia8_CR->SetLineWidth(2);
  gr_pythia8_CR->SetLineColor(1);
  gr_pythia8_CR->SetLineStyle(4);
  

  TGraph *gr_pythia_7TeV = new TGraph("dat/PYTHIA_Lc_D0_pp_7TeV.txt","%lg %lg");
  TGraph *gr_pythia_7TeV_CR = new TGraph("dat/PYTHIACR_Lc_D0_pp_7TeV.txt","%lg %lg");
  TGraph *gr_Greco_LHC_1 = new TGraph("dat/Greco_LcD0_1_PbPb5p02TeV.txt","%lg %lg");
  TGraph *gr_Greco_LHC_2 = new TGraph("dat/Greco_LcD0_2_PbPb5p02TeV.txt","%lg %lg");
  TGraph *gr_ShaoSong_LHC_1 = new TGraph("dat/ShaoSong_LcD0_1_PbPb5p02TeV.txt","%lg %lg");
  TGraph *gr_ShaoSong_LHC_2 = new TGraph("dat/ShaoSong_LcD0_2_PbPb5p02TeV.txt","%lg %lg");
  /*
  const Int_t N_Greco = 14;
  Double_t pt_Greco[N_Greco], y_Greco[N_Greco], ye_Greco[N_Greco];
  inData.open("dat/LcD0_Greco_1407.5069.txt");
  for(int i=0;i<N_Greco;i++) {
    double a, b, c;
    inData >> a >> b >> c;
    pt_Greco[i] = a;
    y_Greco[i] = (b+c)/2.;
    ye_Greco[i] = fabs(b-c)/2.;
  }
  inData.close();
  TGraphErrors *gr_Greco = new TGraphErrors(N_Greco, pt_Greco, y_Greco, 0, ye_Greco);
  gr_Greco->SetFillColor(16);
  gr_Greco->SetLineColor(16);
  */
  TGraph *gr_Greco_1 = new TGraph("dat/Greco_LcD0_1_AuAu200GeV_0_20.dat","%lg %lg");
  gr_Greco_1->SetLineWidth(2);
  gr_Greco_1->SetLineStyle(2);
  gr_Greco_1->SetLineColor(4);
  TGraph *gr_Greco_2 = new TGraph("dat/Greco_LcD0_2_AuAu200GeV_0_20.dat","%lg %lg");
  gr_Greco_2->SetLineWidth(2);
  gr_Greco_2->SetLineStyle(4);
  gr_Greco_2->SetLineColor(4);
  TGraph *gr_Tsinghua_1 = new TGraph("dat/Tsinghua_LcD0_1_AuAu200GeV_10_80.txt","%lg %lg");
  gr_Tsinghua_1->SetLineWidth(2);
  gr_Tsinghua_1->SetLineStyle(2);
  gr_Tsinghua_1->SetLineColor(kGreen-6);
  TGraph *gr_Tsinghua_2 = new TGraph("dat/Tsinghua_LcD0_2_AuAu200GeV_10_80.txt","%lg %lg");
  gr_Tsinghua_2->SetLineWidth(2);
  gr_Tsinghua_2->SetLineStyle(4);
  gr_Tsinghua_2->SetLineColor(kGreen-6);
  
  
  const Int_t N_Ko = 12;
  Double_t pt_Ko[N_Ko], y1_Ko[N_Ko], y2_Ko[N_Ko], y3_Ko[N_Ko];
  inData.open("dat/LcD0_Ko_0901.1382.txt");
  for(int i=0;i<N_Ko;i++) {
    inData >> pt_Ko[i] >> y1_Ko[i] >> y2_Ko[i] >> y3_Ko[i];
  }
  inData.close();
  TGraph *gr_Ko_1 = new TGraph(N_Ko, pt_Ko, y1_Ko);
  gr_Ko_1->SetLineWidth(2);
  gr_Ko_1->SetLineColor(2);
  gr_Ko_1->SetLineStyle(3);
  TGraph *gr_Ko_2 = new TGraph(N_Ko, pt_Ko, y2_Ko);
  gr_Ko_2->SetLineWidth(2);
  gr_Ko_2->SetLineColor(2);
  gr_Ko_2->SetLineStyle(2);
  TGraph *gr_Ko_3 = new TGraph(N_Ko, pt_Ko, y3_Ko);
  gr_Ko_3->SetLineWidth(2);
  gr_Ko_3->SetLineColor(2);
  gr_Ko_3->SetLineStyle(4);
  // sPHENIX projection
  const Int_t N_sPH = 20;
  double pT_sPH[N_sPH], sig_sPH_noPID[N_sPH], sig_sPH_ideal[N_sPH];
  double r_sPH[N_sPH], r_err_sPH_noPID[N_sPH], r_err_sPH_ideal[N_sPH];
  double pT_sPH_noPID[N_sPH], pT_sPH_ideal[N_sPH];
  const Double_t offset = 0.06;
  //  TFile *fin = new TFile("signi_0_10.root");
  TFile *fin = new TFile("significance/signi_0.root");
  TH1D *h1 = (TH1D *)fin->Get("NoPID_signi");
  TH1D *h2 = (TH1D *)fin->Get("Cleanideal_signi");
  // cout<<" ok "<<endl;
  int n_sPH = 0;
  for(int i=0;i<h1->GetNbinsX();i++) {
    pT_sPH[i] = h1->GetBinCenter(i+1);
    sig_sPH_noPID[i] = h1->GetBinContent(i+1);
    sig_sPH_ideal[i] = h2->GetBinContent(i+1);

    pT_sPH_noPID[i] = pT_sPH[i]-offset;
    pT_sPH_ideal[i] = pT_sPH[i]+offset;
    r_sPH[i] = gr[0]->Eval(pT_sPH[i]);
    r_err_sPH_noPID[i] = r_sPH[i]/sig_sPH_noPID[i];
    r_err_sPH_ideal[i] = r_sPH[i]/sig_sPH_ideal[i];
    n_sPH++;
  }
  TGraphErrors *gr_sPH_noPID = new TGraphErrors(n_sPH, pT_sPH_noPID, r_sPH, 0, r_err_sPH_noPID);
  gr_sPH_noPID->SetMarkerStyle(24);
  // gr_sPH_noPID->SetMarkerSize(1.8);
  gr_sPH_noPID->SetMarkerSize(1.3);
  gr_sPH_noPID->SetMarkerColor(1);
  gr_sPH_noPID->SetLineColor(1);
  gr_sPH_noPID->SetLineWidth(2);
  TGraphErrors *gr_sPH_ideal = new TGraphErrors(n_sPH, pT_sPH_ideal, r_sPH, 0, r_err_sPH_ideal);
  gr_sPH_ideal->SetMarkerStyle(20);
  // gr_sPH_ideal->SetMarkerSize(1.8);
  gr_sPH_ideal->SetMarkerSize(1.3);
  gr_sPH_ideal->SetMarkerColor(1);
  gr_sPH_ideal->SetLineColor(1);
  gr_sPH_ideal->SetLineWidth(2);


  TCanvas *c1 = new TCanvas("c1", "c1",0,0,800,600);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetEndErrorSize(0);

  c1->SetFillColor(10);
  c1->SetFillStyle(0);
  c1->SetBorderMode(0);
  c1->SetBorderSize(0);
  c1->SetFrameFillColor(10);
  c1->SetFrameFillStyle(0);
  c1->SetFrameBorderMode(0);
  c1->SetLeftMargin(0.11);
  c1->SetRightMargin(0.03);
  c1->SetTopMargin(0.03);
  c1->SetBottomMargin(0.14);
  //  c1->SetLogy();
  // c1->SetTickx();
  // c1->SetTicky();
  c1->Draw();
  c1->cd();


  double x1 = 0.01;
  double x2 = 8.99;
  double y1 = 0.001;
  double y2 = 4;
  TH1 *h0 = new TH1D("h0","",1,x1, x2);
  h0->SetMinimum(y1);
  h0->SetMaximum(y2);
  h0->GetXaxis()->SetNdivisions(208);
  h0->GetXaxis()->CenterTitle();
  h0->GetXaxis()->SetTitle("Transverse Momentum p_{T} (GeV/c)");
  h0->GetXaxis()->SetTitleOffset(0.95);
  h0->GetXaxis()->SetTitleSize(0.06);
  h0->GetXaxis()->SetLabelOffset(0.01);
  h0->GetXaxis()->SetLabelSize(0.045);
  h0->GetXaxis()->SetLabelFont(42);
  h0->GetXaxis()->SetTitleFont(42);
  h0->GetYaxis()->SetNdivisions(505);
  h0->GetYaxis()->SetTitle("(#Lambda_{c}^{+}+#bar{#Lambda_{c}}^{-})/(D^{0}+#bar{D}^{0})");
  h0->GetYaxis()->SetTitleOffset(0.8);
  h0->GetYaxis()->SetTitleSize(0.06);
  h0->GetYaxis()->SetLabelOffset(0.018);
  h0->GetYaxis()->SetLabelSize(0.045);
  h0->GetYaxis()->SetLabelFont(42);
  h0->GetYaxis()->SetTitleFont(42);
  h0->Draw("c");

  TLine *l1 = new TLine(x1,y1,x2,y1);
  l1->SetLineWidth(3);
  l1->Draw("same");
  TLine *l2 = new TLine(x1,y2,x2,y2);
  l2->SetLineWidth(3);
  l2->Draw("same");
  TLine *l3 = new TLine(x1,y1,x1,y2);
  l3->SetLineWidth(3);
  l3->Draw("same");
  TLine *l4 = new TLine(x2,y1,x2,y2);
  l4->SetLineWidth(3);
  l4->Draw("same");


  // gr_pythia->SetFillColor(16);
  // gr_pythia->SetLineWidth(2);
  // gr_pythia->SetLineStyle(2);
  // gr_pythia->SetLineColor(1);
  // gr_pythia->Draw("c");

  gr_pythia8->Draw("c");
  gr_pythia8_CR->Draw("c");
  
  gr_Greco_1->Draw("c");
  gr_Greco_2->Draw("c");
  //  gr_Ko_1->Draw("c");
  gr_Ko_2->Draw("c");
  gr_Ko_3->Draw("c"); // pythia
  gr_Tsinghua_1->Draw("c");
  gr_Tsinghua_2->Draw("c");

  
  TLine *la = new TLine(0.2, 0.24, 1.5, 0.24);
  la->SetLineWidth(5);
  la->SetLineColor(18);
  //  la->Draw("same");

  TLatex *tex = new TLatex(0.5, 0.26, "SHM");
  tex->SetTextFont(12);
  tex->SetTextSize(0.06);
  //  tex->Draw("same");

  /*
  // plotting the data points
  const Int_t kStyle[NConfig] = {20, 20, 24, 20};
  const Int_t kColor[NConfig] = {1, 2, 4, 1};
  const double xo = fabs(x2-x1)/80.;
  const double yo = fabs(y2-y1)/80.;
  for(int i=0;i<1;i++) {  // STAR

    for(int j=0;j<N[i];j++) { 
      double x1 = x[i][j] - xo;
      double x2 = x[i][j] + xo;
      double y1 = y[i][j] - yes_d[i][j];
      double y2 = y[i][j] + yes_u[i][j];
      
      TLine *la = new TLine(x1, y1, x1, y1+yo);
      la->SetLineColor(kColor[i]);
      la->SetLineWidth(1);
      la->Draw("same");
      TLine *lb = new TLine(x2, y1, x2, y1+yo);
      lb->SetLineColor(kColor[i]);
      lb->SetLineWidth(1);
      lb->Draw("same");
      TLine *lc = new TLine(x1, y2, x1, y2-yo);
      lc->SetLineColor(kColor[i]);
      lc->SetLineWidth(1);
      lc->Draw("same");
      TLine *ld = new TLine(x2, y2, x2, y2-yo);
      ld->SetLineColor(kColor[i]);
      ld->SetLineWidth(1);
      ld->Draw("same");
      TLine *le = new TLine(x1, y1, x2, y1);
      le->SetLineColor(kColor[i]);
      le->SetLineWidth(2);
      le->Draw("same");
      TLine *lf = new TLine(x1, y2, x2, y2);
      lf->SetLineColor(kColor[i]);
      lf->SetLineWidth(2);
      lf->Draw("same");
    }
    gr[i]->SetMarkerStyle(kStyle[i]);
    gr[i]->SetMarkerColor(kColor[i]);
    gr[i]->SetMarkerSize(1.8);
    gr[i]->SetLineColor(kColor[i]);
    gr[i]->SetLineWidth(2);
    gr[i]->Draw("p");
  }
  */
  gr_St->Draw("p");
  gr_sPH_noPID->Draw("p");
  gr_sPH_ideal->Draw("p");

  TLatex *tex = new TLatex(x1+0.2, y2*0.9, "Au+Au, #sqrt{s_{NN}}=200GeV");
  tex->SetTextFont(42);
  tex->SetTextSize(0.05);
  tex->Draw("same");
  TLatex *tex = new TLatex(x1+0.2, y2*0.9, "#sqrt{s_{NN}}=200GeV");
  tex->SetTextFont(42);
  tex->SetTextSize(0.055);
  //  tex->Draw("same");
  

  TLegend *leg = new TLegend(0.52, 0.82, 0.94, 0.94);
   leg->SetFillColor(10);
   leg->SetFillStyle(10);
   leg->SetLineStyle(4000);
   leg->SetLineColor(10);
   leg->SetLineWidth(0.);
   leg->SetTextFont(42);
   leg->SetTextSize(0.035);
   leg->AddEntry(gr_sPH_ideal, "sPHENIX proj: ideal TOF, 0-10%", "pl");
   leg->AddEntry(gr_sPH_noPID, "sPHENIX proj: noPID, 0-10%", "pl");
   leg->AddEntry(gr_St, "STAR prel., 0-20%", "pl");
   leg->Draw();

   TLegend *leg = new TLegend(0.56, 0.64, 0.94, 0.8);
   leg->SetFillColor(10);
   leg->SetFillStyle(10);
   leg->SetLineStyle(4000);
   leg->SetLineColor(10);
   leg->SetLineWidth(0.);
   leg->SetTextFont(42);
   leg->SetTextSize(0.035);
   leg->AddEntry(gr_Tsinghua_2,"Tsinghua: seq. coal, 10-80%","l");
   leg->AddEntry(gr_Tsinghua_1,"Tsinghua: simul. coal, 10-80%","l");
   leg->AddEntry(gr_Greco_2,"Catania: coal only, 0-20%","l");
   leg->AddEntry(gr_Greco_1,"Catania: coal+frag, 0-20%","l");
   leg->Draw();

   TLegend *leg = new TLegend(0.68, 0.48, 0.94, 0.64);
   leg->SetFillColor(10);
   leg->SetFillStyle(10);
   leg->SetLineStyle(4000);
   leg->SetLineColor(10);
   leg->SetLineWidth(0.);
   leg->SetTextFont(42);
   leg->SetTextSize(0.035);
   leg->AddEntry(gr_Ko_3,"TAMU: di-quark 0-5%","l");
   leg->AddEntry(gr_Ko_2,"TAMU: 3-quark 0-5%","l");   
   leg->AddEntry(gr_pythia8_CR,"PYTHIA8 (CR)","l");   
   leg->AddEntry(gr_pythia8,"PYTHIA8 (Monash)","l");   
   leg->Draw();

   c1->Update();
     
  c1->SaveAs("fig/LcD0_proj_0_10_24B.pdf");
  c1->SaveAs("fig/LcD0_proj_0_10_24B.png");
  
  
}

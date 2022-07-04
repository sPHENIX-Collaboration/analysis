#include "sPhenixStyle.h"
#include "sPhenixStyle.C"


void draw_hists() 
{


  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

  TFile *f = new TFile("hists.root","READ");
  TFile *f_pp = new TFile("hists_pp.root","READ");
  int ppExists = f_pp->IsOpen();

  TCanvas *c = new TCanvas("c","c");
  gPad->SetLogy();

  TLegend *leg = new TLegend(.25,.2,.6,.5);
  leg->SetFillStyle(0);
  leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
  leg->AddEntry("","Au+Au #sqrt{s_{NN}}=200 GeV","");
  leg->AddEntry("","anti-#it{k}_{#it{t}} #it{R} = 0.4, |#eta| < 1.1","");


  TLegend *cleg = new TLegend(.7,.2,.9,.45);
  cleg->SetFillStyle(0);
  

  int colors[] = {1,2,4,kGreen+2, kViolet,kCyan,kOrange+2,kMagenta+2,kAzure-2};
  string labels[] = {"pp","0-10%","10-30%","30-50%","50-80%"};
  TGraphErrors *g_jes[11];
  TGraphErrors *g_jer[11];
  int ncent = 0;
  for(int i = 0; i < 11; i++){
    if(!(f->GetListOfKeys()->Contains(Form("g_jes_cent%i",i)))) continue;
    g_jes[i] = (TGraphErrors*) f->Get(Form("g_jes_cent%i",i));
    g_jer[i] = (TGraphErrors*) f->Get(Form("g_jer_cent%i",i));
    ncent++;
  }  
  TGraphErrors *g_jes_pp;
  TGraphErrors *g_jer_pp;
  if(ppExists)
    {
      g_jes_pp = (TGraphErrors*) f_pp->Get("g_jes_cent0");
      g_jer_pp = (TGraphErrors*) f_pp->Get("g_jer_cent0");
    }

  TCanvas * c2 = new TCanvas("c2","c2");
  TMultiGraph *mg1 = new TMultiGraph();
  for(int i = 0; i < ncent; i++){
    g_jes[i]->SetMarkerColor(colors[i+1]);
    g_jes[i]->SetLineColor(colors[i+1]);
    mg1->Add(g_jes[i],"p");
    cleg->AddEntry(g_jes[i],labels[i].c_str(),"p");
    
  }
  if(ppExists)
    {
      mg1->Add(g_jes_pp,"p");
      cleg->AddEntry(g_jes_pp,"pp","p");
    }
  mg1->Draw("a");
  mg1->SetMinimum(0.);
  mg1->SetMaximum(1.);

  mg1->GetXaxis()->SetRangeUser(5,100);
  mg1->GetXaxis()->SetTitle("p_{T,truth} [GeV]");
  mg1->GetYaxis()->SetTitle("#LTp_{T,reco}/p_{T,truth}#GT");

  leg->Draw();
  cleg->Draw();

  TCanvas * c3 = new TCanvas("c3","c3");
  TMultiGraph *mg2 = new TMultiGraph();  
  TLegend *cleg2 = new TLegend(.3,.6,.5,.9);
  cleg2->SetFillStyle(0);

  for(int i = 0; i < ncent; i++){
    g_jer[i]->SetMarkerColor(colors[i]);
    g_jer[i]->SetLineColor(colors[i]);
    mg2->Add(g_jer[i],"p"); 
    cleg2->AddEntry(g_jes[i],labels[i].c_str(),"p");
  }
  if(ppExists)
    {
      mg2->Add(g_jer_pp,"p");
      cleg2->AddEntry(g_jes_pp,"pp","p");
    }
  mg2->Draw("a");
  mg2->SetMinimum(0.);
  mg2->SetMaximum(1.0);

  mg2->GetXaxis()->SetTitle("p_{T,truth} [GeV]");
  mg2->GetYaxis()->SetTitle("#sigma(p_{T,reco}/p_{T,truth}) / #LTp_{T,reco}/p_{T,truth}#GT");
  
  TLegend *leg2 = new TLegend(.4,.6,.8,.9);
  leg2->SetFillStyle(0);
  leg2->AddEntry("","#it{#bf{sPHENIX}} Internal","");
  leg2->AddEntry("","Au+Au #sqrt{s_{NN}}=200 GeV","");
  leg2->AddEntry("","anti-#it{k}_{#it{t}} #it{R} = 0.4, |#eta| < 1.1","");
  leg2->Draw();
  cleg2->Draw();

    
  c2->SaveAs(Form("plots/AuAu_JES_R04.pdf"));
  c3->SaveAs(Form("plots/AuAu_JER_R04.pdf"));

  TLegend *cleg3 = new TLegend(.3,.8,.5,.93);
  cleg3->SetFillStyle(0);
  TLegend *leg3 = new TLegend(.4,.7,.8,.93);
  leg3->SetFillStyle(0);
  leg3->AddEntry("","#it{#bf{sPHENIX}} Internal","");
  leg3->AddEntry("","Au+Au #sqrt{s_{NN}}=200 GeV","");
  leg3->AddEntry("","anti-#it{k}_{#it{t}} #it{R} = 0.4, |#eta| < 1.1","");

  //draw some subtracted et
  TH3F *h_subEt = (TH3F*) f->Get("h_subEt_pt");
  string cent[] = {"Inclusive","0-10%","10-30%","30-50%","50-80%"};
  for(int i = 0; i < ncent; i++){
    h_subEt->GetZaxis()->SetRange(i+1,i+1);
    TH2F *h_proj = (TH2F*) h_subEt->Project3D("yx");
    h_proj->Draw("COLZ");
    TH1F *h_prof = (TH1F*)h_proj->ProfileX();
    h_prof->Draw("SAME");
    h_proj->GetXaxis()->SetTitle("p_{T}^{reco} [GeV]");
    h_proj->GetYaxis()->SetTitle("Subtracted E_{T}");
    h_proj->GetYaxis()->SetRangeUser(0,h_prof->GetBinContent(1)*2);
   
    cleg3->AddEntry("%s",cent[i].c_str());
    cleg3->Draw();
    leg3->Draw();
    c3->SaveAs(Form("plots/AuAu_subtracted_Et_cent%i.pdf",i));
    cleg3->Clear();
  }

}


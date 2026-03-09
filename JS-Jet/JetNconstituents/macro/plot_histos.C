#include "sPhenixStyle.h"
#include "sPhenixStyle.C"


void plot_histos(const char *filename = "output.root", const float jetparameter = 0.4)
{


  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

    TFile *f = new TFile(filename,"READ");
  

    TH1D *h_nconst = (TH1D*) f->Get("h1d_nConsituents");
    TH1D *h_nconst_IHCal = (TH1D*) f->Get("h1d_nConsituents_IHCal");
    TH1D *h_nconst_OHCal = (TH1D*) f->Get("h1d_nConsituents_OHCal");
    TH1D *h_nconst_EMCal = (TH1D*) f->Get("h1d_nConsituents_EMCal");
    TH1D *h_FracEnergy_EMCal = (TH1D*) f->Get("h1d_FracEnergy_EMCal");
    TH1D *h_FracEnergy_IHCal = (TH1D*) f->Get("h1d_FracEnergy_IHCal");
    TH1D *h_FracEnergy_OHCal = (TH1D*) f->Get("h1d_FracEnergy_OHCal");

    TH2D *h_FracEnergy_vs_CaloLayer = (TH2D*) f->Get("h2d_FracEnergy_vs_CaloLayer");
    TH2D *h_nConstituent_vs_CaloLayer = (TH2D*) f->Get("h2d_nConstituent_vs_CaloLayer");

    TCanvas *c = new TCanvas("c","c");
    TLegend *leg = new TLegend(0.5,0.5,0.9,0.9);
    leg->SetFillStyle(0);
    leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg->AddEntry("","Au+Au #sqrt{s_{NN}}=200 GeV","");
    leg->AddEntry("",Form("anti-#it{k}_{#it{t}} #it{R} = %.1f, |#eta| < 1.1",jetparameter),"");
    int colors[] = {1,2,4};
    string labels[] = {"iHCal","oHCal","EMCal"};

    c->SetTitle("Number of Constituents per Calo layer");
    
    h_nconst_IHCal->SetLineColor(colors[0]);
    h_nconst_IHCal->SetMarkerColor(colors[0]);

    h_nconst_OHCal->SetLineColor(colors[1]);
    h_nconst_OHCal->SetMarkerColor(colors[1]);

    h_nconst_EMCal->SetLineColor(colors[2]);
    h_nconst_EMCal->SetMarkerColor(colors[2]);

    leg->AddEntry(h_nconst_IHCal,labels[0].c_str(),"l");
    leg->AddEntry(h_nconst_OHCal,labels[1].c_str(),"l");
    leg->AddEntry(h_nconst_EMCal,labels[2].c_str(),"l");

    h_nconst_IHCal->GetXaxis()->SetTitle("Number of Constituents");
    h_nconst_IHCal->GetYaxis()->SetTitle("Counts");


    h_nconst_IHCal->Draw("hist");
    h_nconst_OHCal->Draw("hist same");
    h_nconst_EMCal->Draw("hist same");

    leg->Draw("same");

    c->Update();
    string outputfilename = string(filename);
    outputfilename.erase(outputfilename.find(".root"));
    c->SaveAs(Form("h_nconst_%s.pdf",outputfilename.c_str()));

    
    TCanvas *c2 = new TCanvas("c2","c2");
    TLegend *leg2 = new TLegend(0.5,0.5,0.9,0.9);
    leg2->SetFillStyle(0);
    leg2->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg2->AddEntry("","Au+Au #sqrt{s_{NN}}=200 GeV","");
    leg2->AddEntry("",Form("anti-#it{k}_{#it{t}} #it{R} = %.1f, |#eta| < 1.1",jetparameter),"");

    c2->SetTitle("Fraction of Energy per Calo layer");

    h_FracEnergy_IHCal->GetXaxis()->SetTitle("Fraction of Energy");
    h_FracEnergy_IHCal->GetYaxis()->SetTitle("Counts");

    h_FracEnergy_IHCal->SetLineColor(colors[0]);
    h_FracEnergy_IHCal->SetMarkerColor(colors[0]);

    h_FracEnergy_OHCal->SetLineColor(colors[1]);    
    h_FracEnergy_OHCal->SetMarkerColor(colors[1]);

    h_FracEnergy_EMCal->SetLineColor(colors[2]);
    h_FracEnergy_EMCal->SetMarkerColor(colors[2]);

    leg2->AddEntry(h_FracEnergy_IHCal,labels[0].c_str(),"l");
    leg2->AddEntry(h_FracEnergy_OHCal,labels[1].c_str(),"l");
    leg2->AddEntry(h_FracEnergy_EMCal,labels[2].c_str(),"l");

    h_FracEnergy_IHCal->Draw("hist");
    h_FracEnergy_OHCal->Draw("hist same");
    h_FracEnergy_EMCal->Draw("hist same");

    leg2->Draw("same");

    c2->Update();

    c2->SaveAs(Form("h_FracEnergy_%s.pdf",outputfilename.c_str()));


    TCanvas *c3 = new TCanvas("c3","c3");
    c3->SetTitle("Total of Constituents");

    h_nconst->GetXaxis()->SetTitle("Number of Constituents");
    h_nconst->GetYaxis()->SetTitle("Counts");
    h_nconst->Draw("hist");

    c3->Update();

    c3->SaveAs(Form("h_nconst_total_%s.pdf",outputfilename.c_str()));

    TCanvas *c4 = new TCanvas("c4","c4");
    c4->SetTitle("Fraction of Energy");

    h_FracEnergy_vs_CaloLayer->GetXaxis()->SetTitle("Calo Layer");
    h_FracEnergy_vs_CaloLayer->GetYaxis()->SetTitle("Fraction of Energy");
    h_FracEnergy_vs_CaloLayer->GetXaxis()->SetBinLabel(1,"EMCal");
    h_FracEnergy_vs_CaloLayer->GetXaxis()->SetBinLabel(2,"IHCal");
    h_FracEnergy_vs_CaloLayer->GetXaxis()->SetBinLabel(3,"OHCal");
    h_FracEnergy_vs_CaloLayer->Draw("COLZ");
    TLine *line = new TLine(1,0,1,1);
    line->SetLineColor(kRed);
    line->Draw("same");
    TLine *line2 = new TLine(2,0,2,1);
    line2->SetLineColor(kRed);
    line2->Draw("same");

    c4->Update();
    c4->SaveAs(Form("h_FracEnergy_vs_calolayer_%s.pdf",outputfilename.c_str()));

    TCanvas *c5 = new TCanvas("c5","c5");
    c5->SetTitle("Number of Constituents");

    h_nConstituent_vs_CaloLayer->GetXaxis()->SetTitle("Calo Layer");
    h_nConstituent_vs_CaloLayer->GetYaxis()->SetTitle("Number of Constituents");
    h_nConstituent_vs_CaloLayer->GetXaxis()->SetBinLabel(1,"EMCal");
    h_nConstituent_vs_CaloLayer->GetXaxis()->SetBinLabel(2,"IHCal");
    h_nConstituent_vs_CaloLayer->GetXaxis()->SetBinLabel(3,"OHCal");
    h_nConstituent_vs_CaloLayer->Draw("COLZ");
    TLine *line3 = new TLine(1,0,1,1);
    line3->SetLineColor(kRed);
    line3->Draw("same");
    TLine *line4 = new TLine(2,0,2,1);
    line4->SetLineColor(kRed);
    line4->Draw("same");

    c5->Update();

    c5->SaveAs(Form("h_nconst_vs_calolayer_%s.pdf",outputfilename.c_str()));





    


}



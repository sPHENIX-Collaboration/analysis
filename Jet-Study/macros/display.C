// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TF1.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>

// -- sPHENIX Style
#include "sPhenixStyle.C"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;
using std::min;
using std::max;
using std::ofstream;

namespace myAnalysis {
    void plots(const string& i_input, const string &output);

    vector<string> labelA = {"All", "Z Vtx", "|Z| < 60 cm", "|Z| < 50 cm", "|Z| < 30 cm", "|Z| < 20 cm", "|Z| < 10 cm"};
    vector<string> labelB = {"All", "MBD NS #geq 1","MBD NS #geq 1 && Jet 8 GeV","MBD NS #geq 1 && Jet 10 GeV","MBD NS #geq 1 && Jet 12 GeV"};
    vector<string> labelC = {"|Z| < 60 cm","|Z| < 60 cm && Jet 8 GeV","|Z| < 60 cm && Jet 10 GeV","|Z| < 60 cm && Jet 12 GeV"};
    vector<string> labelD = {"|Z| < 60 cm","|Z| < 60 cm && MBD NS #geq 1","|Z| < 60 cm && MBD NS #geq 1 && Jet 8 GeV","|Z| < 60 cm && MBD NS #geq 1 && Jet 10 GeV","|Z| < 60 cm && MBD NS #geq 1 && Jet 12 GeV"};

    enum class EventStatus
    {
      ALL = 0,
      ZVTX = 1,
      ZVTX60 = 2,
      ZVTX50 = 3,
      ZVTX30 = 4,
      ZVTX20 = 5,
      ZVTX10 = 6,
      ALL_MBDNS1 = 7,
      ALL_MBDNS1_JET8 = 8,
      ALL_MBDNS1_JET10 = 9,
      ALL_MBDNS1_JET12 = 10,
      ZVTX60_MBDNS1 = 11,
      ZVTX60_MBDNS1_JET8 = 12,
      ZVTX60_MBDNS1_JET10 = 13,
      ZVTX60_MBDNS1_JET12 = 14,
      ZVTX60_JET8 = 15,
      ZVTX60_JET10 = 16,
      ZVTX60_JET12 = 17
    };
}

void myAnalysis::plots(const string& i_input, const string &output) {
    TFile input(i_input.c_str());

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.02);
    c1->SetBottomMargin(.11);

    c1->Print((output + "[").c_str(), "pdf portrait");

    gStyle->SetOptTitle();
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.05);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleXOffset(1);
    gStyle->SetTitleYOffset(1.4);

    auto hEvents = (TH1F*)input.Get("hEvents");
    auto hEventsBkg = (TH1F*)input.Get("hEventsBkg");

    c1->cd();

    auto h    = new TH1F("hEvents_ZVTX","Events;Status;Counts",labelA.size(),0,labelA.size());
    auto hBkg = new TH1F("hEvents_ZVTX_BKG","Events;Status;Counts",labelA.size(),0,labelA.size());

    h->Draw();

    TLatex latex;
    latex.SetTextSize(0.07);

    cout << endl;
    for(UInt_t i = 1; i <= h->GetNbinsX(); ++i) {
        h->GetXaxis()->SetBinLabel(i, string(1,char('A'+i-1)).c_str());
        hBkg->GetXaxis()->SetBinLabel(i, string(1,char('A'+i-1)).c_str());
        h->SetBinContent(i,hEvents->GetBinContent(i));
        if(hEvents->GetBinContent(i)) hBkg->SetBinContent(i,hEventsBkg->GetBinContent(i)/hEvents->GetBinContent(i)*100.);
        latex.DrawLatexNDC(0.65,0.85-0.07*(i-1),(string(1,char('A'+i-1)) + ": " + labelA[i-1]).c_str());

        cout << labelA[i-1] << ": " << hEvents->GetBinContent(i) << ", " << hEventsBkg->GetBinContent(i) << ", " << hEventsBkg->GetBinContent(i)/hEvents->GetBinContent(i)*100 << " %" << endl;
    }
    cout << endl;

    h->SetMinimum(0);
    h->GetXaxis()->SetLabelSize(0.1);
    gPad->SetGrid(0,1);

    c1->Print((string(h->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    hBkg->Draw();

    for(UInt_t i = 1; i <= h->GetNbinsX(); ++i) {
        latex.DrawLatexNDC(0.65,0.85-0.07*(i-1),(string(1,char('A'+i-1)) + ": " + labelA[i-1]).c_str());
    }

    hBkg->GetYaxis()->SetTitle("Fraction of Background Events [%]");
    hBkg->SetMinimum(0);
    hBkg->GetXaxis()->SetLabelSize(0.1);

    c1->Print((string(h->GetName()) + "-with-Bkg.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    h = new TH1F("hEvents_MBD","Events;Status;Counts",labelB.size(),0,labelB.size());
    hBkg = new TH1F("hEvents_MBD_BKG","Events;Status;Counts",labelB.size(),0,labelB.size());

    h->Draw();

    latex.SetTextSize(0.08);

    h->SetBinContent(1,hEvents->GetBinContent(1));
    if(hEvents->GetBinContent(1)) hBkg->SetBinContent(1,hEventsBkg->GetBinContent(1)/hEvents->GetBinContent(1)*100.);
    cout << endl;
    cout << labelB[0] << ": " << hEvents->GetBinContent(1) << ", " << hEventsBkg->GetBinContent(1) << ", " << hEventsBkg->GetBinContent(1)/hEvents->GetBinContent(1)*100 << " %" << endl;
    for(UInt_t i = 1; i <= h->GetNbinsX(); ++i) {
        h->GetXaxis()->SetBinLabel(i, string(1,char('A'+i-1)).c_str());
        hBkg->GetXaxis()->SetBinLabel(i, string(1,char('A'+i-1)).c_str());
        latex.DrawLatexNDC(0.2,0.5-0.09*(i-1),(string(1,char('A'+i-1)) + ": " + labelB[i-1]).c_str());
    }

    for(UInt_t i = 2; i <= h->GetNbinsX(); ++i) {
      h->SetBinContent(i,hEvents->GetBinContent(i+6));
      if(hEvents->GetBinContent(i+6)) hBkg->SetBinContent(i,hEventsBkg->GetBinContent(i+6)/hEvents->GetBinContent(i+6)*100.);
      cout << labelB[i-1] << ": " << hEvents->GetBinContent(i+6) << ", " << hEventsBkg->GetBinContent(i+6) << ", " << hEventsBkg->GetBinContent(i+6)/hEvents->GetBinContent(i+6)*100 << " %" << endl;
    }
    cout << endl;

    h->SetMinimum(1);
    h->GetXaxis()->SetLabelSize(0.1);

    gPad->SetLogy();

    c1->Print((string(h->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    gPad->SetLogy(0);
    hBkg->Draw();

    latex.SetTextSize(0.05);
    for(UInt_t i = 1; i <= h->GetNbinsX(); ++i) {
        latex.DrawLatexNDC(0.18,0.87-0.07*(i-1),(string(1,char('A'+i-1)) + ": " + labelB[i-1]).c_str());
    }

    hBkg->GetYaxis()->SetTitle("Fraction of Background Events [%]");
    hBkg->GetXaxis()->SetLabelSize(0.1);
    hBkg->SetMinimum(0);

    c1->Print((string(h->GetName()) + "-with-Bkg.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");


    // -----------------------------------------------------------------------------

    gPad->SetLogy();

    h = new TH1F("hEvents_ZVTX_JET","Events;Status;Counts",labelC.size(),0,labelC.size());
    hBkg = new TH1F("hEvents_ZVTX_JET_BKG","Events;Status;Counts",labelC.size(),0,labelC.size());

    h->Draw();

    latex.SetTextSize(0.07);

    h->SetBinContent(1,hEvents->GetBinContent(3));
    if(hEvents->GetBinContent(3)) hBkg->SetBinContent(1,hEventsBkg->GetBinContent(3)/hEvents->GetBinContent(3)*100.);
    cout << endl;
    cout << labelC[0] << ": " << hEvents->GetBinContent(3) << ", " << hEventsBkg->GetBinContent(3) << ", " << hEventsBkg->GetBinContent(3)/hEvents->GetBinContent(3)*100 << " %" << endl;
    for(UInt_t i = 1; i <= h->GetNbinsX(); ++i) {
        h->GetXaxis()->SetBinLabel(i, string(1,char('A'+i-1)).c_str());
        hBkg->GetXaxis()->SetBinLabel(i, string(1,char('A'+i-1)).c_str());
        latex.DrawLatexNDC(0.2,0.4-0.09*(i-1),(string(1,char('A'+i-1)) + ": " + labelC[i-1]).c_str());
    }

    for(UInt_t i = 2; i <= h->GetNbinsX(); ++i) {
      h->SetBinContent(i,hEvents->GetBinContent(i+14));
      if(hEvents->GetBinContent(i+14)) hBkg->SetBinContent(i,hEventsBkg->GetBinContent(i+14)/hEvents->GetBinContent(i+14)*100.);
      cout << labelC[i-1] << ": " << hEvents->GetBinContent(i+14) << ", " << hEventsBkg->GetBinContent(i+14) << ", " << hEventsBkg->GetBinContent(i+14)/hEvents->GetBinContent(i+14)*100 << " %" << endl;
    }
    cout << endl;

    h->SetMinimum(1);
    h->GetXaxis()->SetLabelSize(0.1);

    c1->Print((string(h->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    gPad->SetLogy(0);
    hBkg->Draw();

    latex.SetTextSize(0.06);
    for(UInt_t i = 1; i <= h->GetNbinsX(); ++i) {
        latex.DrawLatexNDC(0.4,0.4-0.07*(i-1),(string(1,char('A'+i-1)) + ": " + labelC[i-1]).c_str());
    }

    hBkg->GetYaxis()->SetTitle("Fraction of Background Events [%]");
    hBkg->GetXaxis()->SetLabelSize(0.1);
    hBkg->SetMinimum(0);

    c1->Print((string(h->GetName()) + "-with-Bkg.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    gPad->SetLogy();

    h = new TH1F("hEvents_ZVTX_MBD_JET","Events;Status;Counts",labelD.size(),0,labelD.size());
    hBkg = new TH1F("hEvents_ZVTX_MBD_JET_BKG","Events;Status;Counts",labelD.size(),0,labelD.size());

    h->Draw();

    latex.SetTextSize(0.05);

    h->SetBinContent(1,hEvents->GetBinContent(3));
    if(hEvents->GetBinContent(3)) hBkg->SetBinContent(1,hEventsBkg->GetBinContent(3)/hEvents->GetBinContent(3)*100.);
    cout << endl;
    cout << labelD[0] << ": " << hEvents->GetBinContent(3) << ", " << hEventsBkg->GetBinContent(3) << ", " << hEventsBkg->GetBinContent(3)/hEvents->GetBinContent(3)*100 << " %" << endl;
    for(UInt_t i = 1; i <= h->GetNbinsX(); ++i) {
        h->GetXaxis()->SetBinLabel(i, string(1,char('A'+i-1)).c_str());
        hBkg->GetXaxis()->SetBinLabel(i, string(1,char('A'+i-1)).c_str());
        latex.DrawLatexNDC(0.2,0.5-0.09*(i-1),(string(1,char('A'+i-1)) + ": " + labelD[i-1]).c_str());
    }

    for(UInt_t i = 2; i <= h->GetNbinsX(); ++i) {
      h->SetBinContent(i,hEvents->GetBinContent(i+10));
      if(hEvents->GetBinContent(i+10)) hBkg->SetBinContent(i,hEventsBkg->GetBinContent(i+10)/hEvents->GetBinContent(i+10)*100.);
      cout << labelD[i-1] << ": " << hEvents->GetBinContent(i+10) << ", " << hEventsBkg->GetBinContent(i+10) << ", " << hEventsBkg->GetBinContent(i+10)/hEvents->GetBinContent(i+10)*100 << " %" << endl;
    }
    cout << endl;

    h->SetMinimum(1);
    h->GetXaxis()->SetLabelSize(0.1);

    c1->Print((string(h->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    gPad->SetLogy(0);

    hBkg->Draw();

    latex.SetTextSize(0.03);
    for(UInt_t i = 1; i <= h->GetNbinsX(); ++i) {
        latex.DrawLatexNDC(0.2,0.85-0.07*(i-1),(string(1,char('A'+i-1)) + ": " + labelD[i-1]).c_str());
    }

    hBkg->GetYaxis()->SetTitle("Fraction of Background Events [%]");
    hBkg->GetXaxis()->SetLabelSize(0.1);
    hBkg->SetMinimum(0);

    c1->Print((string(hBkg->GetName()) + "-with-Bkg.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    gPad->SetGrid(0,0);
    c1->SetRightMargin(.03);

    auto hZVtx = (TH1F*)input.Get("hZVtx");

    hZVtx->SetMinimum(0);
    hZVtx->Draw();

    c1->Print((string(hZVtx->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    gPad->SetLogy();
    c1->SetLeftMargin(.1);

    auto hJetPt_r04_ZVTX60 = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60");
    auto hJetPt_r04_ZVTX60_MBDNS1 = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60_MBDNS1");
    auto hJetPt_r04_ZVTX60_MBDNS1_JET8 = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60_MBDNS1_JET8");
    auto hJetPt_r04_ZVTX60_MBDNS1_JET10 = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60_MBDNS1_JET10");
    auto hJetPt_r04_ZVTX60_MBDNS1_JET12 = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60_MBDNS1_JET12");

    auto leg = new TLegend(0.4, .6, 0.8, .88);
    leg->SetFillStyle(0);
    leg->AddEntry(hJetPt_r04_ZVTX60_MBDNS1, "MBD NS #geq 1", "l");
    leg->AddEntry(hJetPt_r04_ZVTX60_MBDNS1_JET8, "MBD NS #geq 1 && Jet 8 GeV", "l");
    leg->AddEntry(hJetPt_r04_ZVTX60_MBDNS1_JET10, "MBD NS #geq 1 && Jet 10 GeV", "l");
    leg->AddEntry(hJetPt_r04_ZVTX60_MBDNS1_JET12, "MBD NS #geq 1 && Jet 12 GeV", "l");

    hJetPt_r04_ZVTX60_MBDNS1->GetYaxis()->SetTitleOffset(1);
    hJetPt_r04_ZVTX60_MBDNS1->SetTitle("Jet: Anti-k_{t} R = 0.4 and jet |#eta| < 0.6, |Z| < 60 cm");

    hJetPt_r04_ZVTX60_MBDNS1->SetLineColor(kRed);
    hJetPt_r04_ZVTX60_MBDNS1_JET8->SetLineColor(kBlue);
    hJetPt_r04_ZVTX60_MBDNS1_JET10->SetLineColor(kGreen+2);
    hJetPt_r04_ZVTX60_MBDNS1_JET12->SetLineColor(kMagenta);

    hJetPt_r04_ZVTX60_MBDNS1->Rebin(5);
    hJetPt_r04_ZVTX60_MBDNS1_JET8->Rebin(5);
    hJetPt_r04_ZVTX60_MBDNS1_JET10->Rebin(5);
    hJetPt_r04_ZVTX60_MBDNS1_JET12->Rebin(5);

    hJetPt_r04_ZVTX60_MBDNS1->GetXaxis()->SetRangeUser(10,100);

    hJetPt_r04_ZVTX60_MBDNS1->Draw();
    hJetPt_r04_ZVTX60_MBDNS1_JET8->Draw("same");
    hJetPt_r04_ZVTX60_MBDNS1_JET10->Draw("same");
    hJetPt_r04_ZVTX60_MBDNS1_JET12->Draw("same");

    leg->Draw("same");

    c1->Print((string(hJetPt_r04_ZVTX60_MBDNS1->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    leg->AddEntry(hJetPt_r04_ZVTX60, "All", "l");
    hJetPt_r04_ZVTX60->Rebin(5);
    hJetPt_r04_ZVTX60_MBDNS1->GetYaxis()->SetRangeUser(5e-1,1e7);
    hJetPt_r04_ZVTX60_MBDNS1->GetXaxis()->SetRangeUser(10,200);
    hJetPt_r04_ZVTX60->Draw("same");

    c1->Print((string(hJetPt_r04_ZVTX60_MBDNS1->GetName()) + "-more.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    auto hJetPt_r04_ZVTX60_bkg = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60_bkg");
    auto hJetPt_r04_ZVTX60_MBDNS1_bkg = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60_MBDNS1_bkg");
    auto hJetPt_r04_ZVTX60_MBDNS1_JET8_bkg = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60_MBDNS1_JET8_bkg");
    auto hJetPt_r04_ZVTX60_MBDNS1_JET10_bkg = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60_MBDNS1_JET10_bkg");
    auto hJetPt_r04_ZVTX60_MBDNS1_JET12_bkg = (TH1F*)input.Get("hJet_r04/pt/hJetPt_r04_ZVTX60_MBDNS1_JET12_bkg");

    leg = new TLegend(0.4, .6, 0.8, .88);
    leg->SetFillStyle(0);
    leg->AddEntry(hJetPt_r04_ZVTX60_MBDNS1_bkg, "MBD NS #geq 1", "l");
    leg->AddEntry(hJetPt_r04_ZVTX60_MBDNS1_JET8_bkg, "MBD NS #geq 1 && Jet 8 GeV", "l");
    leg->AddEntry(hJetPt_r04_ZVTX60_MBDNS1_JET10_bkg, "MBD NS #geq 1 && Jet 10 GeV", "l");
    leg->AddEntry(hJetPt_r04_ZVTX60_MBDNS1_JET12_bkg, "MBD NS #geq 1 && Jet 12 GeV", "l");

    hJetPt_r04_ZVTX60_MBDNS1_bkg->Rebin(5);
    hJetPt_r04_ZVTX60_MBDNS1_JET8_bkg->Rebin(5);
    hJetPt_r04_ZVTX60_MBDNS1_JET10_bkg->Rebin(5);
    hJetPt_r04_ZVTX60_MBDNS1_JET12_bkg->Rebin(5);

    hJetPt_r04_ZVTX60_MBDNS1_bkg->GetYaxis()->SetTitleOffset(1);
    hJetPt_r04_ZVTX60_MBDNS1_bkg->GetXaxis()->SetRangeUser(10,40);
    hJetPt_r04_ZVTX60_MBDNS1_bkg->SetTitle("Background Jet: Anti-k_{t} R = 0.4 and jet |#eta| < 0.6, |Z| < 60 cm");

    hJetPt_r04_ZVTX60_MBDNS1_bkg->SetLineColor(kRed);
    hJetPt_r04_ZVTX60_MBDNS1_JET8_bkg->SetLineColor(kBlue);
    hJetPt_r04_ZVTX60_MBDNS1_JET10_bkg->SetLineColor(kGreen+2);
    hJetPt_r04_ZVTX60_MBDNS1_JET12_bkg->SetLineColor(kMagenta);

    hJetPt_r04_ZVTX60_MBDNS1_bkg->Draw();
    hJetPt_r04_ZVTX60_MBDNS1_JET8_bkg->Draw("same");
    hJetPt_r04_ZVTX60_MBDNS1_JET10_bkg->Draw("same");
    hJetPt_r04_ZVTX60_MBDNS1_JET12_bkg->Draw("same");

    leg->Draw("same");

    c1->Print((string(hJetPt_r04_ZVTX60_MBDNS1_bkg->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    leg->AddEntry(hJetPt_r04_ZVTX60_bkg, "All", "l");
    hJetPt_r04_ZVTX60_bkg->Rebin(5);
    hJetPt_r04_ZVTX60_MBDNS1_bkg->GetYaxis()->SetRangeUser(5e-1,1e5);
    hJetPt_r04_ZVTX60_MBDNS1_bkg->GetXaxis()->SetRangeUser(10,70);
    hJetPt_r04_ZVTX60_bkg->Draw("same");

    c1->Print((string(hJetPt_r04_ZVTX60_MBDNS1_bkg->GetName()) + "-more.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    gPad->SetLogy();

    auto hJetDeltaPhi_r04_ZVTX60 = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60");
    auto hJetDeltaPhi_r04_ZVTX60_MBDNS1 = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60_MBDNS1");
    auto hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8 = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8");
    auto hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10 = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10");
    auto hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12 = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12");

    leg = new TLegend(0.2, .6, 0.6, .88);
    leg->SetFillStyle(0);
    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60_MBDNS1, "MBD NS #geq 1", "l");
    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8, "MBD NS #geq 1 && Jet 8 GeV", "l");
    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10, "MBD NS #geq 1 && Jet 10 GeV", "l");
    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12, "MBD NS #geq 1 && Jet 12 GeV", "l");

    hJetDeltaPhi_r04_ZVTX60_MBDNS1->GetYaxis()->SetTitleOffset(1);
    hJetDeltaPhi_r04_ZVTX60_MBDNS1->SetTitle("Jet: Anti-k_{t} R = 0.4 and jet |#eta| < 0.6, |Z| < 60 cm");

    hJetDeltaPhi_r04_ZVTX60_MBDNS1->SetLineColor(kRed);
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8->SetLineColor(kBlue);
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10->SetLineColor(kGreen+2);
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12->SetLineColor(kMagenta);

    hJetDeltaPhi_r04_ZVTX60_MBDNS1->Draw();
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8->Draw("same");
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10->Draw("same");
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12->Draw("same");

    leg->Draw("same");

    c1->Print((string(hJetDeltaPhi_r04_ZVTX60_MBDNS1->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60, "All", "l");
    hJetDeltaPhi_r04_ZVTX60_MBDNS1->GetYaxis()->SetRangeUser(5e-1,1e5);
    hJetDeltaPhi_r04_ZVTX60->Draw("same");

    c1->Print((string(hJetDeltaPhi_r04_ZVTX60_MBDNS1->GetName()) + "-more.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    auto hJetDeltaPhi_r04_ZVTX60_bkg = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60_bkg");
    auto hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg");
    auto hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8_bkg = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8_bkg");
    auto hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10_bkg = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10_bkg");
    auto hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12_bkg = (TH1F*)input.Get("hJet_r04/deltaPhi/hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12_bkg");

    leg = new TLegend(0.4, .6, 0.8, .88);
    leg->SetFillStyle(0);
    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg, "MBD NS #geq 1", "l");
    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8_bkg, "MBD NS #geq 1 && Jet 8 GeV", "l");
    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10_bkg, "MBD NS #geq 1 && Jet 10 GeV", "l");
    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12_bkg, "MBD NS #geq 1 && Jet 12 GeV", "l");

    hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg->GetYaxis()->SetTitleOffset(1);
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg->SetTitle("Background Jet: Anti-k_{t} R = 0.4 and jet |#eta| < 0.6, |Z| < 60 cm");

    hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg->SetLineColor(kRed);
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8_bkg->SetLineColor(kBlue);
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10_bkg->SetLineColor(kGreen+2);
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12_bkg->SetLineColor(kMagenta);

    hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg->Draw();
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET8_bkg->Draw("same");
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET10_bkg->Draw("same");
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_JET12_bkg->Draw("same");

    leg->Draw("same");

    c1->Print((string(hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    leg->AddEntry(hJetDeltaPhi_r04_ZVTX60_bkg, "All", "l");
    hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg->GetYaxis()->SetRangeUser(5e-1,1e5);
    hJetDeltaPhi_r04_ZVTX60_bkg->Draw("same");

    c1->Print((string(hJetDeltaPhi_r04_ZVTX60_MBDNS1_bkg->GetName()) + "-more.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    // -----------------------------------------------------------------------------

    gPad->SetLogy(0);

    c1->SetCanvasSize(2000, 1000);
    c1->SetLeftMargin(.07);
    c1->SetRightMargin(.13);
    c1->SetTopMargin(.11);

    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.1);

    auto h2JetEtaPhi_r04_ZVTX60 = (TH2F*)input.Get("hJet_r04/eta_phi/h2JetEtaPhi_r04_ZVTX60");
    auto h2JetEtaPhi_r04_ZVTX60_bkg = (TH2F*)input.Get("hJet_r04/eta_phi/h2JetEtaPhi_r04_ZVTX60_bkg");

    auto h2JetEtaPhi_r04_ZVTX60_MBDNS1 = (TH2F*)input.Get("hJet_r04/eta_phi/h2JetEtaPhi_r04_ZVTX60_MBDNS1");
    auto h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET8 = (TH2F*)input.Get("hJet_r04/eta_phi/h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET8");
    auto h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET10 = (TH2F*)input.Get("hJet_r04/eta_phi/h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET10");
    auto h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET12 = (TH2F*)input.Get("hJet_r04/eta_phi/h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET12");

    h2JetEtaPhi_r04_ZVTX60->SetTitle("Jet: R=0.4, |Z| < 60 cm");
    h2JetEtaPhi_r04_ZVTX60->GetYaxis()->SetTitleOffset(0.7);

    h2JetEtaPhi_r04_ZVTX60_bkg->SetTitle("Background Jet: R=0.4, |Z| < 60 cm");
    h2JetEtaPhi_r04_ZVTX60_bkg->GetYaxis()->SetTitleOffset(0.7);

    h2JetEtaPhi_r04_ZVTX60_MBDNS1->SetTitle("Jet: R=0.4, |Z| < 60 cm && MBD NS #geq 1");
    h2JetEtaPhi_r04_ZVTX60_MBDNS1->GetYaxis()->SetTitleOffset(0.7);

    h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET8->SetTitle("Jet: R=0.4, |Z| < 60 cm && MBD NS #geq 1 && Jet 8 GeV");
    h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET8->GetYaxis()->SetTitleOffset(0.7);

    h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET10->SetTitle("Jet: R=0.4, |Z| < 60 cm && MBD NS #geq 1 && Jet 10 GeV");
    h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET10->GetYaxis()->SetTitleOffset(0.7);

    h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET12->SetTitle("Jet: R=0.4, |Z| < 60 cm && MBD NS #geq 1 && Jet 12 GeV");
    h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET12->GetYaxis()->SetTitleOffset(0.7);

    h2JetEtaPhi_r04_ZVTX60->Draw("COLZ1");
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    h2JetEtaPhi_r04_ZVTX60_bkg->Draw("COLZ1");
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60_bkg->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    h2JetEtaPhi_r04_ZVTX60_MBDNS1->Draw("COLZ1");
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60_MBDNS1->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET8->Draw("COLZ1");
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET8->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET10->Draw("COLZ1");
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET10->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET12->Draw("COLZ1");
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60_MBDNS1_JET12->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.13);
    c1->SetRightMargin(.03);
    c1->SetTopMargin(.11);

    auto px = h2JetEtaPhi_r04_ZVTX60->ProjectionX();
    px->GetYaxis()->SetTitle("Counts");
    px->SetMinimum(0);
    auto py = h2JetEtaPhi_r04_ZVTX60->ProjectionY();
    py->GetYaxis()->SetTitle("Counts");

    px->Draw();
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60->GetName()) + "-proj-X.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    py->Draw();
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60->GetName()) + "-proj-Y.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    px = h2JetEtaPhi_r04_ZVTX60_bkg->ProjectionX();
    px->SetMinimum(0);
    px->GetYaxis()->SetTitle("Counts");
    py = h2JetEtaPhi_r04_ZVTX60_bkg->ProjectionY();
    py->GetYaxis()->SetTitle("Counts");

    px->Draw();
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60_bkg->GetName()) + "-proj-X.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    py->Draw();
    c1->Print((string(h2JetEtaPhi_r04_ZVTX60_bkg->GetName()) + "-proj-Y.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    c1->Print((output + "]").c_str(), "pdf portrait");
    input.Close();
}

void display(const string &input, const string &output="plots.pdf") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    myAnalysis::plots(input, output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./display input [output]" << endl;
        cout << "input: input root file" << endl;
        cout << "output: output pdf file" << endl;
        return 1;
    }

    string output  = "plots.pdf";

    if(argc >= 3) {
        output = argv[2];
    }

    display(argv[1], output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif

// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>

// -- root includes --
#include <TH2F.h>
#include <TF1.h>
#include <TDirectory.h>
#include <TKey.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TDatime.h>
#include <TGaxis.h>

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
    void plots(const string& i_input, const string &outputDir);

    UInt_t ntowers = 24576;
    UInt_t threshold = 400; // nRuns above which tower is frequently bad
    Float_t zMax = 1700;
    TDatime d("2024-08-13 16:00:00"); // start of 1.5 mrad
}

void myAnalysis::plots(const string& i_input, const string &outputDir) {

    string outputSigmaDir = outputDir + "/sigma/";
    string outputAccptDir = outputDir + "/acceptance/";
    string outputSigma    = outputSigmaDir + "sigma.pdf";
    string outputAccpt    = outputAccptDir + "plots.pdf";

    TFile input(i_input.c_str());

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.13);
    c1->SetTopMargin(.08);
    c1->SetRightMargin(.05);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleXOffset(1);
    gStyle->SetTitleYOffset(1);

    auto hRunStatus = (TH1F*)input.Get("hRunStatus");

    auto hBadTowers    = (TH1F*)input.Get("hBadTowers");
    auto hBadTowersDead  = (TH1F*)input.Get("hBadTowersDead");
    auto hBadTowersHot  = (TH1F*)input.Get("hBadTowersHot");
    auto hBadTowersCold  = (TH1F*)input.Get("hBadTowersCold");
    auto hBadTowersHotChi2 = (TH1F*)input.Get("hBadTowersHotChi2");

    auto h2BadTowers   = (TH2F*)input.Get("h2BadTowers");
    auto h2BadTowersDead = (TH2F*)input.Get("h2BadTowersDead");
    auto h2BadTowersHot = (TH2F*)input.Get("h2BadTowersHot");
    auto h2BadTowersCold = (TH2F*)input.Get("h2BadTowersCold");
    auto h2BadTowersHotChi2 = (TH2F*)input.Get("h2BadTowersHotChi2");

    auto hSigma = (TH1F*)input.Get("hSigmaHot");
    auto hSigmaFreqHot = (TH1F*)input.Get("hSigmaFreqHot");

    auto hHotTowerStatus = (TH1F*)input.Get("hHotTowerStatus");

    auto h2HotTowerFrequency_dummy = (TH2F*)input.Get("h2HotTowerFrequency/h2HotTowerFrequency_27_7");

    auto hAcceptance = (TH1F*)input.Get("hAcceptance");
    auto hFracDead = (TH1F*)input.Get("hFracDead");
    auto hFracHot = (TH1F*)input.Get("hFracHot");
    auto hFracCold = (TH1F*)input.Get("hFracCold");
    auto hFracBadChi2 = (TH1F*)input.Get("hFracBadChi2");

    auto hAcceptanceVsTime = (TH1F*)input.Get("hAcceptanceVsTime");
    auto hDeadVsTime = (TH1F*)input.Get("hDeadVsTime");
    auto hHotVsTime = (TH1F*)input.Get("hHotVsTime");
    auto hColdVsTime = (TH1F*)input.Get("hColdVsTime");
    auto hBadChi2VsTime = (TH1F*)input.Get("hBadChi2VsTime");

    auto h2BadTowersFreq = new TH2F("h2BadTowersFreq", "Bad Towers; #phi Index; #eta Index", h2BadTowers->GetNbinsX(), -0.5, h2BadTowers->GetNbinsX()-0.5, h2BadTowers->GetNbinsY(), -0.5, h2BadTowers->GetNbinsY()-0.5);

    // threshold = h2HotTowerFrequency_dummy->GetEntries()/2;
    cout << "threshold: " << threshold << endl;

    for(UInt_t i = 1; i <= h2BadTowers->GetNbinsX(); ++i) {
        for(UInt_t j = 1; j <= h2BadTowers->GetNbinsY(); ++j) {
            Double_t val = 0;
            val = max(val, h2BadTowersDead->GetBinContent(i, j));
            val = max(val, h2BadTowersHot->GetBinContent(i, j));
            val = max(val, h2BadTowersCold->GetBinContent(i, j));
            val = max(val, h2BadTowersHotChi2->GetBinContent(i, j));

            if(val >= threshold) h2BadTowersFreq->SetBinContent(i, j, val);
        }
    }

    // vector<string> hBadTowersTitle = {"Runs [with Bad Towers]", "Runs [with Dead Towers]","Runs [with Hot Towers]","Runs [with Cold Towers]","Runs [with BadChi2 Towers]"};
    vector<TH1F*> hBadTowersVec  = {hBadTowers, hBadTowersDead, hBadTowersHot, hBadTowersCold, hBadTowersHotChi2};
    vector<TH2F*> h2BadTowersVec = {h2BadTowersFreq, h2BadTowersDead, h2BadTowersHot, h2BadTowersCold, h2BadTowersHotChi2};
    // vector<string> label         = {"Status #neq 0", "Dead", "Hot", "Cold"};

    string dirName = "hHotTowerSigma";
    TDirectory* dir = (TDirectory*) input.Get(dirName.c_str());
    if (!dir)
    {
      cerr << "Directory not found: " << dirName << endl;
      input.Close();
      return;
    }

    TIter nextkey(dir->GetListOfKeys());
    TKey* key;

    stringstream name;

    c1->Print((outputSigma + "[").c_str(), "pdf portrait");
    c1->cd();

    gPad->SetLogy();

    hSigma->Rebin(5);

    auto sigma_threshold = new TLine(5, 0, 5, hSigma->GetMaximum()*1.05);
    sigma_threshold->SetLineColor(kOrange);
    sigma_threshold->SetLineWidth(3);

    hSigma->SetTitle("Towers");
    hSigma->SetLineColor(kBlue);
    hSigma->GetXaxis()->SetRangeUser(0,30);
    hSigma->GetYaxis()->SetRangeUser(1,1e7);
    hSigma->Draw();
    sigma_threshold->Draw("same");
    c1->Print((outputSigmaDir + string(hSigma->GetName()) + ".png").c_str());

    hSigmaFreqHot->Rebin(5);

    hSigmaFreqHot->SetLineColor(kRed);
    hSigmaFreqHot->Draw("same");

    auto leg = new TLegend(0.5,.75,0.7,.9);
    leg->SetFillStyle(0);
    leg->AddEntry(hSigma,("Flagged Hot < " + to_string(threshold) + " Runs").c_str(),"f");
    leg->AddEntry(hSigmaFreqHot,("Flagged Hot #geq " + to_string(threshold) +" Runs").c_str(),"f");
    leg->AddEntry(sigma_threshold,"Sigma Threshold","l");
    leg->Draw("same");

    c1->Print((outputSigmaDir + string(hSigma->GetName()) + ".png").c_str());
    c1->Print(outputSigma.c_str(), "pdf portrait");

    gPad->SetLogy(0);

    TLatex latex;
    latex.SetTextSize(0.05);

    while ((key = (TKey*) nextkey())) {
        name.str("");
        name << dirName << "/" << key->GetName();

        auto h = (TH1F*)input.Get(name.str().c_str());
        h->Rebin(5);

        sigma_threshold = new TLine(5, 0, 5, h->GetMaximum()*1.2);
        sigma_threshold->SetLineColor(kRed);
        sigma_threshold->SetLineWidth(3);
        h->Draw();
        Int_t overFlow = 0;
        Int_t xlow = -4;
        if(string(key->GetName()) == "hHotTowerSigma_27_84") {
            h->GetXaxis()->SetRangeUser(xlow,30);
            overFlow = h->Integral(h->FindBin(30),h->GetNbinsX()+1);
        }
        else if(string(key->GetName()) == "hHotTowerSigma_27_85") {
            h->GetXaxis()->SetRangeUser(xlow,20);
            overFlow = h->Integral(h->FindBin(20),h->GetNbinsX()+1);
        }
        else {
            h->GetXaxis()->SetRangeUser(xlow,15);
            overFlow = h->Integral(h->FindBin(15),h->GetNbinsX()+1);
        }

        h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.2);

        name.str("");
        name << "Overflow: " << overFlow << " Runs";

        latex.DrawLatexNDC(0.67,0.85, name.str().c_str());

        name.str("");
        name << "Underflow: " << h->Integral(0,h->FindBin(xlow)-1) << " Runs";

        latex.DrawLatexNDC(0.67,0.8, name.str().c_str());

        sigma_threshold->Draw("same");

        c1->Print((outputSigmaDir + string(h->GetName()) + "-sigma.png").c_str());
        c1->Print(outputSigma.c_str(), "pdf portrait");
    }

    c1->Print((outputSigma + "]").c_str(), "pdf portrait");

    c1->Print((outputAccpt + "[").c_str(), "pdf portrait");

    TLine* line = new TLine(0, threshold, ntowers, threshold);
    line->SetLineColor(kRed);
    line->SetLineStyle(9);
    line->SetLineWidth(3);

    for(UInt_t i = 0; i < hBadTowersVec.size(); ++i) {

        // hBadTowersVec[i]->GetYaxis()->SetTitle(hBadTowersTitle[i].c_str());
        Float_t norm = (i < hBadTowersVec.size()-1) ? hRunStatus->GetBinContent(1) : hRunStatus->GetBinContent(2);
        cout << "norm: " << norm << endl;
        hBadTowersVec[i]->Scale(100./norm);

        hBadTowersVec[i]->GetYaxis()->SetTitle("Runs [%]");
        hBadTowersVec[i]->GetYaxis()->SetTitleOffset(1.3);
        if(hBadTowersVec[i]->GetMaximum() <= 100) hBadTowersVec[i]->GetYaxis()->SetRangeUser(0,100);
        hBadTowersVec[i]->Draw("HIST");

        auto line = new TLine(0, threshold*100/norm, ntowers, threshold*100/norm);
        line->SetLineColor(kRed);
        line->SetLineStyle(9);
        line->SetLineWidth(3);
        line->Draw("same");

        c1->Print((outputAccptDir + string(hBadTowersVec[i]->GetName()) + ".png").c_str());
        c1->Print(outputAccpt.c_str(), "pdf portrait");

        delete line;
    }

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);
    gPad->SetGrid();

    for(UInt_t i = 0; i < h2BadTowersVec.size(); ++i) {
        h2BadTowersVec[i]->SetMaximum(zMax);

        h2BadTowersVec[i]->GetXaxis()->SetLimits(0,256);
        h2BadTowersVec[i]->GetXaxis()->SetNdivisions(32, false);
        h2BadTowersVec[i]->GetXaxis()->SetLabelSize(0.04);
        h2BadTowersVec[i]->GetXaxis()->SetTickSize(0.01);
        h2BadTowersVec[i]->GetYaxis()->SetTickSize(0.01);
        h2BadTowersVec[i]->GetYaxis()->SetLabelSize(0.04);
        h2BadTowersVec[i]->GetYaxis()->SetLimits(0,96);
        h2BadTowersVec[i]->GetYaxis()->SetNdivisions(12, false);
        h2BadTowersVec[i]->GetYaxis()->SetTitleOffset(0.5);

        if(i == 0) h2BadTowersVec[i]->SetMinimum(threshold);

        h2BadTowersVec[i]->Draw("colz1");

        c1->Print((outputAccptDir +string(h2BadTowersVec[i]->GetName()) + ".png").c_str());
        c1->Print(outputAccpt.c_str(), "pdf portrait");
    }

    h2BadTowersDead->SetMinimum(threshold);
    h2BadTowersDead->Draw("colz1");

    c1->Print((outputAccptDir + string(h2BadTowersDead->GetName()) + "-threshold.png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    h2BadTowersHot->SetMinimum(threshold);
    h2BadTowersHot->Draw("colz1");

    c1->Print((outputAccptDir + string(h2BadTowersHot->GetName()) + "-threshold.png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    h2BadTowersCold->SetMinimum(threshold);
    h2BadTowersCold->Draw("colz1");

    c1->Print((outputAccptDir + string(h2BadTowersCold->GetName()) + "-threshold.png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    h2BadTowersHotChi2->SetMinimum(threshold);
    h2BadTowersHotChi2->Draw("colz1");

    c1->Print((outputAccptDir + string(h2BadTowersHotChi2->GetName()) + "-threshold.png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    h2BadTowersHotChi2->SetMinimum(0);
    h2BadTowersHotChi2->SetMaximum(40);

    c1->Print((outputAccptDir + string(h2BadTowersHotChi2->GetName()) + "-zoom.png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    c1->SetRightMargin(.1);
    gPad->SetGrid(0,0);

    c1->SetTicky(0);

    auto vl = new TLine(d.Convert(), 0, d.Convert(), hAcceptanceVsTime->GetMaximum()*1.05);
    vl->SetLineColor(kRed);
    vl->SetLineWidth(1);

    Float_t xmax = hAcceptanceVsTime->GetXaxis()->GetXmax();
    auto axis = new TGaxis(xmax,0,xmax,100,0,24576,110,"+L");
    axis->SetTitle("Towers");
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(0.8);
    axis->SetLabelSize(0.05);
    // axis->SetTextAngle();

    hAcceptanceVsTime->Draw("P");
    hAcceptanceVsTime->SetMarkerColor(kBlue);
    hAcceptanceVsTime->SetMarkerStyle(3);
    hAcceptanceVsTime->GetYaxis()->SetTitleOffset(0.5);
    vl->Draw("same");

    cout << "Max: " << hAcceptanceVsTime->GetXaxis()->GetXmax() << endl;
    axis->Draw();

    c1->Print((outputAccptDir + string(hAcceptanceVsTime->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // compute averages
    UInt_t bin_change = hAcceptanceVsTime->FindBin(d.Convert());
    Float_t acceptanceTotal[2] = {0};
    UInt_t bin_ctr[2] = {0};
    for(UInt_t i = 1; i <= hAcceptanceVsTime->GetNbinsX(); ++i) {
        Float_t val = hAcceptanceVsTime->GetBinContent(i);
        if(i < bin_change && val != 0) {
           acceptanceTotal[0] += val;
           ++bin_ctr[0];
        }
        if(i >= bin_change && val != 0) {
           acceptanceTotal[1] += hAcceptanceVsTime->GetBinContent(i);
           ++bin_ctr[1];
        }
    }

    cout << "#########################" << endl;
    cout << "Average Acceptance" << endl;
    cout << "Bin Change: " << bin_change << endl;
    cout << "Overall: " << (acceptanceTotal[0]+acceptanceTotal[1])/(bin_ctr[0]+bin_ctr[1]) << " %" << endl;
    cout << "0 mrad: " << acceptanceTotal[0]/bin_ctr[0] << " %" << endl;
    cout << "1.5 mrad: " << acceptanceTotal[1]/bin_ctr[1] << " %" << endl;
    cout << "#########################" << endl;

    // ----------------------------

    hDeadVsTime->Draw("P");
    hDeadVsTime->SetMarkerStyle(3);
    hDeadVsTime->SetMarkerColor(kBlue);
    hDeadVsTime->GetYaxis()->SetTitleOffset(0.5);
    axis->Draw();
    vl->Draw("same");

    c1->Print((outputAccptDir + string(hDeadVsTime->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    delete axis;
    axis = new TGaxis(xmax,0.8,xmax,2,196,492,510,"+L");
    axis->SetTitle("Towers");
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(0.6);
    axis->SetLabelSize(0.05);
    vl->SetY1(0.8);
    vl->SetY2(2);

    hDeadVsTime->GetYaxis()->SetRangeUser(0.8,2);
    axis->Draw();


    c1->Print((outputAccptDir + string(hDeadVsTime->GetName()) + "-zoom.png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    delete axis;
    axis = new TGaxis(xmax,0,xmax, hHotVsTime->GetMaximum()*1.05,0,ntowers*hHotVsTime->GetMaximum()*1.05/100.,510,"+L");
    axis->SetTitle("Towers");
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(0.6);
    axis->SetLabelSize(0.05);

    hHotVsTime->Draw("P");
    hHotVsTime->SetMarkerStyle(3);
    hHotVsTime->SetMarkerColor(kBlue);
    hHotVsTime->GetYaxis()->SetTitleOffset(0.5);
    axis->Draw();
    vl->SetY1(0);
    vl->SetY2(hHotVsTime->GetMaximum()*1.05);
    vl->Draw("same");

    c1->Print((outputAccptDir + string(hHotVsTime->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    delete axis;
    axis = new TGaxis(xmax,0,xmax,0.5,0,123,510,"+L");
    axis->SetTitle("Towers");
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(0.6);
    axis->SetLabelSize(0.05);
    axis->Draw();
    vl->SetY2(0.5);

    hHotVsTime->GetYaxis()->SetTitleOffset(0.6);
    hHotVsTime->GetYaxis()->SetRangeUser(0,0.5);

    c1->Print((outputAccptDir + string(hHotVsTime->GetName()) + "-zoom.png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    delete axis;
    axis = new TGaxis(xmax,0,xmax, hColdVsTime->GetMaximum()*1.05,0,ntowers*hColdVsTime->GetMaximum()*1.05/100.,510,"+L");

    axis->SetTitle("Towers");
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(0.6);
    axis->SetLabelSize(0.05);

    hColdVsTime->Draw("P");
    hColdVsTime->SetMarkerStyle(3);
    hColdVsTime->SetMarkerColor(kBlue);
    hColdVsTime->GetYaxis()->SetTitleOffset(0.5);
    axis->Draw();
    vl->SetY2(hColdVsTime->GetMaximum()*1.05);
    vl->Draw("same");

    c1->Print((outputAccptDir + string(hColdVsTime->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    delete axis;
    axis = new TGaxis(xmax,0,xmax, hBadChi2VsTime->GetMaximum()*1.05,0,ntowers*hBadChi2VsTime->GetMaximum()*1.05/100.,510,"+L");
    axis->SetTitle("Towers");
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(0.8);
    axis->SetLabelSize(0.05);

    hBadChi2VsTime->Draw("P");
    hBadChi2VsTime->SetMarkerStyle(3);
    hBadChi2VsTime->SetMarkerColor(kBlue);
    hBadChi2VsTime->GetYaxis()->SetTitleOffset(0.5);
    axis->Draw();
    vl->SetY2(hBadChi2VsTime->GetMaximum()*1.05);
    vl->Draw("same");

    c1->Print((outputAccptDir + string(hBadChi2VsTime->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    delete axis;
    axis = new TGaxis(xmax,0,xmax,1,0,246,510,"+L");
    axis->SetTitle("Towers");
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(0.6);
    axis->SetLabelSize(0.05);
    axis->Draw();
    vl->SetY2(1);

    // hBadChi2VsTime->GetYaxis()->SetTitleOffset(0.6);
    hBadChi2VsTime->GetYaxis()->SetRangeUser(0,1);

    c1->Print((outputAccptDir + string(hBadChi2VsTime->GetName()) + "-zoom.png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    gPad->SetLogy();

    c1->SetTicky();
    c1->SetCanvasSize(1300,1000);
    c1->SetRightMargin(0.05);
    c1->SetLeftMargin(0.1);

    hAcceptance->Draw();
    c1->Print((outputAccptDir + string(hAcceptance->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    hFracDead->Draw();
    c1->Print((outputAccptDir + string(hFracDead->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    hFracHot->GetXaxis()->SetRangeUser(0,12);
    hFracHot->Draw();
    c1->Print((outputAccptDir + string(hFracHot->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    hFracCold->GetXaxis()->SetRangeUser(0,3);
    hFracCold->Draw();
    c1->Print((outputAccptDir + string(hFracCold->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    hFracBadChi2->GetXaxis()->SetRangeUser(0,80);
    hFracBadChi2->Draw();
    c1->Print((outputAccptDir + string(hFracBadChi2->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    hHotTowerStatus->GetYaxis()->SetTitleOffset(0.9);
    hHotTowerStatus->Draw();

    c1->Print((outputAccptDir + string(hHotTowerStatus->GetName()) + ".png").c_str());
    c1->Print(outputAccpt.c_str(), "pdf portrait");

    // ----------------------------

    c1->Print((outputAccpt + "]").c_str(), "pdf portrait");

    UInt_t ctr[10] = {0};

    for(UInt_t i = 1; i <= h2BadTowers->GetNbinsX(); ++i) {
        for(UInt_t j = 1; j <= h2BadTowers->GetNbinsY(); ++j) {
            if(h2BadTowers->GetBinContent(i, j) > 0)                 ++ctr[0];
            if(h2BadTowersFreq->GetBinContent(i, j) >= threshold)    ++ctr[1];
            if(h2BadTowersDead->GetBinContent(i, j) > 0)             ++ctr[2];
            if(h2BadTowersDead->GetBinContent(i, j) >= threshold)    ++ctr[3];
            if(h2BadTowersHot->GetBinContent(i, j) > 0)              ++ctr[4];
            if(h2BadTowersHot->GetBinContent(i, j) >= threshold)     ++ctr[5];
            if(h2BadTowersCold->GetBinContent(i, j) > 0)             ++ctr[6];
            if(h2BadTowersCold->GetBinContent(i, j) >= threshold)    ++ctr[7];
            if(h2BadTowersHotChi2->GetBinContent(i, j) > 0)          ++ctr[8];
            if(h2BadTowersHotChi2->GetBinContent(i, j) >= threshold) ++ctr[9];
        }
    }

    cout << "Bad Towers" << endl
         << "Any Run: "                          << ctr[0] << ", " << ctr[0]*100./ntowers << " %" << endl
         << "Threshold >= " << threshold << ": " << ctr[1] << ", " << ctr[1]*100./ntowers << " %" << endl << endl;

    cout << "Dead Towers" << endl
         << "Any Run: "                          << ctr[2] << ", " << ctr[2]*100./ntowers << " %" << endl
         << "Threshold >= " << threshold << ": " << ctr[3] << ", " << ctr[3]*100./ntowers << " %" << endl << endl;

    cout << "Hot Towers" << endl
         << "Any Run: "                          << ctr[4] << ", " << ctr[4]*100./ntowers << " %" << endl
         << "Threshold >= " << threshold << ": " << ctr[5] << ", " << ctr[5]*100./ntowers << " %" << endl << endl;

    cout << "Cold Towers" << endl
         << "Any Run: "                          << ctr[6] << ", " << ctr[6]*100./ntowers << " %" << endl
         << "Threshold >= " << threshold << ": " << ctr[7] << ", " << ctr[7]*100./ntowers << " %" << endl << endl;

    cout << "Bad Chi2 Towers" << endl
         << "Any Run: "                          << ctr[8] << ", " << ctr[8]*100./ntowers << " %" << endl
         << "Threshold >= " << threshold << ": " << ctr[9] << ", " << ctr[9]*100./ntowers << " %" << endl;

    input.Close();
}

void display(const string &input, const string &outputDir=".") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    // make the output directories
    std::filesystem::create_directories(outputDir + "/acceptance");
    std::filesystem::create_directories(outputDir + "/sigma");

    myAnalysis::plots(input, outputDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./display input [output]" << endl;
        cout << "input: input root file" << endl;
        cout << "outputDir: output directory" << endl;
        return 1;
    }

    string outputDir = ".";

    if(argc >= 3) {
        outputDir = argv[2];
    }

    display(argv[1], outputDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif

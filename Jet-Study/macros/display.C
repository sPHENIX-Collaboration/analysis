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


    // ----------------------------------------------------------
    gPad->SetLogy();

    auto hEvents = (TH1F*)input.Get("event/hEvents");

    TLatex latex;
    latex.SetTextSize(0.05);

    hEvents->Draw();

    for(UInt_t i = 1; i < hEvents->GetNbinsX(); ++i) {
        string label = hEvents->GetXaxis()->GetBinLabel(i+1);
        hEvents->GetXaxis()->SetBinLabel(i+1,string(1,char('A'+i-1)).c_str());

        latex.DrawLatexNDC(0.3,0.89-0.07*(i-1),(string(1,char('A'+i-1)) + ": " + label).c_str());
    }

    hEvents->GetYaxis()->SetRangeUser(1,2e6);
    hEvents->GetXaxis()->SetLabelSize(0.1);
    hEvents->GetXaxis()->SetRangeUser(1,hEvents->GetNbinsX());
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(hEvents->GetName()) + ".png").c_str());

    // ----------------------------------------------------------
    gPad->SetLogy(0);
    c1->SetRightMargin(.15);

    auto h2ETVsFracCEMC      = (TH2F*)input.Get("bkg_checks/h2ETVsFracCEMC");
    auto h2ETVsFracCEMC_miss = (TH2F*)input.Get("bkg_checks/h2ETVsFracCEMC_miss");

    h2ETVsFracCEMC->SetTitle("Event");
    h2ETVsFracCEMC_miss->SetTitle("Event");

    auto cut_LoEmFracET1 = new TLine(0.1,10,0.1,200);
    auto cut_LoEmFracET2 = new TF1("cut_LoEmFracET2","50*x+20",-0.2,0.1);

    auto cut_HiEmFracET1 = new TLine(0.9,10,0.9,200);
    auto cut_HiEmFracET2 = new TF1("cut_LoEmFracET2","-50*x+70",0.9,1.2);

    cut_LoEmFracET1->SetLineColor(kRed);
    cut_LoEmFracET2->SetLineColor(kRed);
    cut_HiEmFracET1->SetLineColor(kRed);
    cut_HiEmFracET2->SetLineColor(kRed);
    cut_LoEmFracET2->SetLineWidth(1);
    cut_HiEmFracET2->SetLineWidth(1);

    h2ETVsFracCEMC->Draw("COLZ1");
    cut_LoEmFracET1->Draw("same");
    cut_LoEmFracET2->Draw("same");
    cut_HiEmFracET1->Draw("same");
    cut_HiEmFracET2->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(h2ETVsFracCEMC->GetName()) + ".png").c_str());

    h2ETVsFracCEMC_miss->SetMaximum(5);
    h2ETVsFracCEMC_miss->Draw("COLZ1");
    cut_LoEmFracET1->Draw("same");
    cut_LoEmFracET2->Draw("same");
    cut_HiEmFracET1->Draw("same");
    cut_HiEmFracET2->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(h2ETVsFracCEMC_miss->GetName()) + ".png").c_str());

    // ----------------------------------------------------------

    auto h2FracOHCalVsFracCEMC      = (TH2F*)input.Get("bkg_checks/h2FracOHCalVsFracCEMC");
    auto h2FracOHCalVsFracCEMC_miss = (TH2F*)input.Get("bkg_checks/h2FracOHCalVsFracCEMC_miss");

    h2FracOHCalVsFracCEMC->SetTitle("Event");
    h2FracOHCalVsFracCEMC_miss->SetTitle("Event");

    auto cut_IhFrac = new TF1("cut_IhFrac","0.65-x",-0.2,1.2);
    cut_IhFrac->SetLineColor(kRed);
    cut_IhFrac->SetLineWidth(1);

    h2FracOHCalVsFracCEMC->Draw("COLZ1");
    cut_IhFrac->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(h2FracOHCalVsFracCEMC->GetName()) + ".png").c_str());

    h2FracOHCalVsFracCEMC_miss->Draw("COLZ1");
    cut_IhFrac->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(h2FracOHCalVsFracCEMC_miss->GetName()) + ".png").c_str());

    // ----------------------------------------------------------

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.1);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gStyle->SetOptTitle();
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.09);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleXOffset(1);
    gStyle->SetTitleYOffset(0.5);

    auto hjetPhiEtaPt = (TH3*)input.Get("jets/hjetPhiEtaPt");

    hjetPhiEtaPt->GetZaxis()->SetRange(hjetPhiEtaPt->GetZaxis()->FindBin(10),hjetPhiEtaPt->GetNbinsZ());
    auto hyx = hjetPhiEtaPt->Project3D("yx");
    hyx->SetTitle("Jet: p_{T} #geq 10 GeV");

    hyx->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(hjetPhiEtaPt->GetName()) + "-10.png").c_str());

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.02);
    c1->SetBottomMargin(.11);

    auto hx = hjetPhiEtaPt->Project3D("x");
    hx->SetTitle("Jet: p_{T} #geq 10 GeV; #phi; Counts");
    hx->GetYaxis()->SetTitleOffset(1.4);
    hx->GetYaxis()->SetRangeUser(0,3e3);

    hx->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(hjetPhiEtaPt->GetName()) + "-10-phi.png").c_str());

    auto hy = hjetPhiEtaPt->Project3D("y");
    hy->SetTitle("Jet: p_{T} #geq 10 GeV; #eta; Counts");
    hy->GetYaxis()->SetTitleOffset(1.4);

    hy->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(hjetPhiEtaPt->GetName()) + "-10-eta.png").c_str());

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.1);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    hjetPhiEtaPt->GetZaxis()->SetRange(hjetPhiEtaPt->GetZaxis()->FindBin(60),hjetPhiEtaPt->GetNbinsZ());
    hyx = hjetPhiEtaPt->Project3D("yx");
    hyx->SetTitle("Jet: p_{T} #geq 60 GeV");

    hyx->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(hjetPhiEtaPt->GetName()) + "-60.png").c_str());

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.02);
    c1->SetBottomMargin(.11);

    hx = hjetPhiEtaPt->Project3D("x");
    hx->SetTitle("Jet: p_{T} #geq 60 GeV; #phi; Counts");
    hx->GetYaxis()->SetTitleOffset(1.4);
    hx->GetYaxis()->SetRangeUser(0,30);

    hx->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(hjetPhiEtaPt->GetName()) + "-60-phi.png").c_str());

    hy = hjetPhiEtaPt->Project3D("y");
    hy->SetTitle("Jet: p_{T} #geq 60 GeV; #eta; Counts");
    hy->GetYaxis()->SetTitleOffset(1.4);

    hy->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(hjetPhiEtaPt->GetName()) + "-60-eta.png").c_str());

    // ----------------------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.12);
    c1->SetBottomMargin(.12);

    auto hjetConstituentsVsPt = (TH2*)input.Get("jets/hjetConstituentsVsPt");

    gPad->SetLogz();
    hjetConstituentsVsPt->Draw("COLZ1");
    hjetConstituentsVsPt->GetXaxis()->SetRangeUser(10,200);
    hjetConstituentsVsPt->GetYaxis()->SetTitleOffset(1.4);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(hjetConstituentsVsPt->GetName()) + ".png").c_str());

    // ----------------------------------------------------------

    auto hNJetsVsLeadPt = (TH2*)input.Get("jets/hNJetsVsLeadPt");
    hNJetsVsLeadPt->RebinX(2);
    hNJetsVsLeadPt->GetXaxis()->SetRangeUser(10,200);
    hNJetsVsLeadPt->GetYaxis()->SetRangeUser(0,50);
    hNJetsVsLeadPt->GetYaxis()->SetTitleOffset(1);

    hNJetsVsLeadPt->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((string(hNJetsVsLeadPt->GetName()) + ".png").c_str());

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

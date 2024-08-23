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

    c1->SetCanvasSize(1700, 1000);
    c1->SetLeftMargin(.13);
    c1->SetRightMargin(.05);

    c1->Print((output + "[").c_str(), "pdf portrait");

    auto hEvents = (TH1F*)input.Get("hEvents");
    auto hZVtx   = (TH1F*)input.Get("hZVtx");

    auto hJetPt_r02 = (TH1F*)input.Get("hJetPt_r02");
    auto hJetPt_r04 = (TH1F*)input.Get("hJetPt_r04");
    auto hJetPt_r06 = (TH1F*)input.Get("hJetPt_r06");

    vector<string> label = {"All", "Has Z Vtx", "|Z| < 30 cm", "|Z| < 20 cm", "|Z| < 10 cm"};

    c1->cd();

    for(UInt_t i = 1; i <= hEvents->GetNbinsX(); ++i) {
        hEvents->GetXaxis()->SetBinLabel(i, label[i-1].c_str());
    }

    hEvents->GetYaxis()->SetTitle("Events");
    hEvents->GetXaxis()->SetLabelSize(0.08);
    hEvents->SetMinimum(0);
    hEvents->Draw();

    gPad->SetGrid();

    c1->Print((string(hEvents->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    gStyle->SetOptStat(1111);
    hZVtx->SetStats();

    hZVtx->SetMinimum(0);
    hZVtx->Draw();
    gPad->Update();

    auto st = (TPaveStats*)hZVtx->GetListOfFunctions()->FindObject("stats");
    st->SetX1NDC(0.65); //new x start position
    st->SetX2NDC(0.95); //new x end position

    st->SetY1NDC(0.7); //new y start position
    st->SetY2NDC(0.95); //new y end position

    c1->Print((string(hZVtx->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    hJetPt_r02->SetLineColor(kRed);
    hJetPt_r04->SetLineColor(kGreen+2);
    hJetPt_r06->SetLineColor(kBlue);

    hJetPt_r02->SetMarkerColor(kRed);
    hJetPt_r04->SetMarkerColor(kGreen+2);
    hJetPt_r06->SetMarkerColor(kBlue);

    hJetPt_r02->Rebin(2);
    hJetPt_r04->Rebin(2);
    hJetPt_r06->Rebin(2);

    hJetPt_r02->Scale(1./hJetPt_r02->Integral(1,hJetPt_r02->FindBin(35)));
    hJetPt_r04->Scale(1./hJetPt_r04->Integral(1,hJetPt_r02->FindBin(35)));
    hJetPt_r06->Scale(1./hJetPt_r06->Integral(1,hJetPt_r02->FindBin(35)));

    cout << "Integral" << endl;
    cout << "Jet R = 0.2: " << hJetPt_r02->Integral(1,hJetPt_r02->FindBin(35)) << endl;
    cout << "Jet R = 0.4: " << hJetPt_r04->Integral(1,hJetPt_r02->FindBin(35)) << endl;
    cout << "Jet R = 0.6: " << hJetPt_r06->Integral(1,hJetPt_r02->FindBin(35)) << endl;

    hJetPt_r02->SetMaximum(1);
    hJetPt_r02->SetMinimum(1e-4);
    hJetPt_r02->GetXaxis()->SetRangeUser(10,40);
    hJetPt_r02->GetYaxis()->SetTitle("Normalized Yield / 2 GeV");
    hJetPt_r02->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
    hJetPt_r02->GetYaxis()->SetTitleOffset(1);

    hJetPt_r02->Draw();
    hJetPt_r04->Draw("same");
    hJetPt_r06->Draw("same");

    TLatex latex;
    latex.SetTextSize(0.04);
    latex.DrawLatexNDC(0.13,0.96,"#bf{#it{sPHENIX}} Internal #it{p+p} #sqrt{s}=200 GeV");
    latex.DrawLatexNDC(0.65,0.65,"|Vertex Z| < 30 cm");
    latex.DrawLatexNDC(0.65,0.6,"Underlying Event Subtracted");
    latex.DrawLatexNDC(0.1,0.04,"Run 46941-49763 (6/28/24-7/31/24)");
    latex.SetTextSize(0.05);
    latex.DrawLatexNDC(0.74,0.89,"Anti-k_{t}");

    auto *leg = new TLegend(0.65, .7, 0.95, .88);
    leg->SetFillStyle(0);
    // leg->SetHeader("Anti-k_{t}","C");
    leg->AddEntry(hJetPt_r02, "R = 0.2", "ple");
    leg->AddEntry(hJetPt_r04, "R = 0.4", "ple");
    leg->AddEntry(hJetPt_r06, "R = 0.6", "ple");
    leg->Draw("same");

    gPad->SetLogy();
    gPad->SetGrid(0,0);

    c1->Print("Jet-pT.png");
    c1->Print((output).c_str(), "pdf portrait");

    hJetPt_r02->Divide(hJetPt_r06);
    hJetPt_r04->Divide(hJetPt_r06);

    auto line = new TLine(10, 1, 40, 1);
    line->SetLineColor(kBlue);
    line->SetLineStyle(9);
    line->SetLineWidth(3);

    hJetPt_r02->GetYaxis()->SetTitle("ratio");
    hJetPt_r02->SetMinimum(0);
    hJetPt_r02->SetMaximum(3);

    hJetPt_r02->Draw();
    hJetPt_r04->Draw("same");
    line->Draw("same");

    leg = new TLegend(0.5, .77, 0.7, .94);
    leg->SetHeader("Anti-k_{t}","C");
    leg->SetFillStyle(0);
    leg->AddEntry(hJetPt_r02, "R = 0.2 / R = 0.6", "ple");
    leg->AddEntry(hJetPt_r04, "R = 0.4 / R = 0.6", "ple");
    leg->Draw("same");

    gPad->SetLogy(0);

    c1->Print("Jet-pT-ratio.png");
    c1->Print((output).c_str(), "pdf portrait");

    cout << "All Events: " << hEvents->GetBinContent(1) << endl;
    cout << "Events with Z Vtx: " << hEvents->GetBinContent(2) << ", " << hEvents->GetBinContent(2)*100./hEvents->GetBinContent(1) << endl;
    cout << "Events with |Z| < 30 cm: " << hEvents->GetBinContent(3) << ", " << hEvents->GetBinContent(3)*100./hEvents->GetBinContent(1) << endl;

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

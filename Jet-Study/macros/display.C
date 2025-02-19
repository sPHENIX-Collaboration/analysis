// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>

// -- root includes --
#include <TH3.h>
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
namespace fs = std::filesystem;

namespace myAnalysis {
    void plots(const string& i_input, const string &output);
}

void myAnalysis::plots(const string& i_input, const string &output) {

    string outputDir = fs::absolute(output).parent_path().string();
    fs::create_directories(outputDir);

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
    c1->Print((outputDir + "/" + string(hEvents->GetName()) + ".png").c_str());

    // ----------------------------------------------------------
    gPad->SetLogy(0);
    c1->SetRightMargin(.15);

    auto h2ETVsFracCEMC      = (TH2F*)input.Get("bkg_checks/h2ETVsFracCEMC");
    auto h2ETVsFracCEMC_jetBkgCut = (TH2F*)input.Get("bkg_checks/h2ETVsFracCEMC_jetBkgCut");

    h2ETVsFracCEMC->SetTitle("Event");
    h2ETVsFracCEMC_jetBkgCut->SetTitle("Event");

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
    c1->Print((outputDir + "/" + string(h2ETVsFracCEMC->GetName()) + ".png").c_str());

    h2ETVsFracCEMC_jetBkgCut->SetMaximum(5);
    h2ETVsFracCEMC_jetBkgCut->Draw("COLZ1");
    cut_LoEmFracET1->Draw("same");
    cut_LoEmFracET2->Draw("same");
    cut_HiEmFracET1->Draw("same");
    cut_HiEmFracET2->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2ETVsFracCEMC_jetBkgCut->GetName()) + ".png").c_str());

    // ----------------------------------------------------------

    auto h2FracOHCalVsFracCEMC      = (TH2F*)input.Get("bkg_checks/h2FracOHCalVsFracCEMC");
    auto h2FracOHCalVsFracCEMC_jetBkgCut = (TH2F*)input.Get("bkg_checks/h2FracOHCalVsFracCEMC_jetBkgCut");

    h2FracOHCalVsFracCEMC->SetTitle("Event");
    h2FracOHCalVsFracCEMC_jetBkgCut->SetTitle("Event");

    auto cut_IhFrac = new TF1("cut_IhFrac","0.65-x",-0.2,1.2);
    cut_IhFrac->SetLineColor(kRed);
    cut_IhFrac->SetLineWidth(1);

    h2FracOHCalVsFracCEMC->Draw("COLZ1");
    cut_IhFrac->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2FracOHCalVsFracCEMC->GetName()) + ".png").c_str());

    h2FracOHCalVsFracCEMC_jetBkgCut->Draw("COLZ1");
    cut_IhFrac->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2FracOHCalVsFracCEMC_jetBkgCut->GetName()) + ".png").c_str());

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
    auto hjetPhiEtaPt_jetBkgCut = (TH3*)input.Get("jets/hjetPhiEtaPt_jetBkgCut");
    auto hjetPhiEtaPt_xjCut = (TH3*)input.Get("jets/hjetPhiEtaPt_xjCut");

    hjetPhiEtaPt->GetZaxis()->SetRange(hjetPhiEtaPt->GetZaxis()->FindBin(10),hjetPhiEtaPt->GetNbinsZ());
    hjetPhiEtaPt_jetBkgCut->GetZaxis()->SetRange(hjetPhiEtaPt_jetBkgCut->GetZaxis()->FindBin(10),hjetPhiEtaPt_jetBkgCut->GetNbinsZ());
    hjetPhiEtaPt_xjCut->GetZaxis()->SetRange(hjetPhiEtaPt_xjCut->GetZaxis()->FindBin(10),hjetPhiEtaPt_xjCut->GetNbinsZ());
    auto hyx = hjetPhiEtaPt->Project3D("yx");
    hyx->SetTitle("Jet: p_{T} #geq 10 GeV");

    hyx->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-10.png").c_str());

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
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-10-phi.png").c_str());

    auto hy = hjetPhiEtaPt->Project3D("y");
    hy->SetTitle("Jet: p_{T} #geq 10 GeV; #eta; Counts");
    hy->GetYaxis()->SetTitleOffset(1.4);

    hy->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-10-eta.png").c_str());

    c1->SetRightMargin(.04);
    gPad->SetLogy();
    auto hz = hjetPhiEtaPt->Project3D("z");
    auto hz_jetBkgCut = hjetPhiEtaPt_jetBkgCut->Project3D("z");
    auto hz_xjCut = hjetPhiEtaPt_xjCut->Project3D("z");

    auto h2LeadTowPtVsJetPt_jetBkgCut = (TH2*)input.Get("jets/h2LeadTowPtVsJetPt_jetBkgCut");
    h2LeadTowPtVsJetPt_jetBkgCut->GetXaxis()->SetRange(h2LeadTowPtVsJetPt_jetBkgCut->GetXaxis()->FindBin(10),h2LeadTowPtVsJetPt_jetBkgCut->GetNbinsX());
    auto h2LeadTowPtVsJetPt_jetBkgCut_px = h2LeadTowPtVsJetPt_jetBkgCut->ProjectionX("h2LeadTowPtVsJetPt_jetBkgCut_px", 1, h2LeadTowPtVsJetPt_jetBkgCut->GetYaxis()->FindBin(17)-1);

    hz->SetTitle("Jet; p_{T} [GeV]; Counts / 5 GeV");
    hz->GetYaxis()->SetTitleOffset(1.4);

    hz_jetBkgCut->SetLineColor(kBlue);
    hz_xjCut->SetLineColor(kRed);
    h2LeadTowPtVsJetPt_jetBkgCut_px->SetLineColor(kRed);

    // rebin to have 5 GeV bins
    hz->Rebin(10);
    hz_jetBkgCut->Rebin(10);
    hz_xjCut->Rebin(10);
    h2LeadTowPtVsJetPt_jetBkgCut_px->Rebin(10);

    auto leg = new TLegend(0.52,.7,0.72,.85);
    leg->SetFillStyle(0);
    leg->AddEntry(hz,"All","f");
    leg->AddEntry(hz_jetBkgCut,"jet bkg cut","f");
    leg->AddEntry(hz_xjCut,"jet bkg cut and x_{j} #geq 0.15","f");

    hz->Draw();
    hz_jetBkgCut->Draw("same");
    hz_xjCut->Draw("same");
    leg->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-pt.png").c_str());

    leg = new TLegend(0.3,.7,0.5,.85);
    leg->SetFillStyle(0);
    leg->AddEntry(hz,"All","f");
    leg->AddEntry(hz_jetBkgCut,"jet bkg cut","f");
    leg->AddEntry(h2LeadTowPtVsJetPt_jetBkgCut_px,"jet bkg cut and lead tower p_{T} < 17 GeV","f");

    hz->Draw();
    hz_jetBkgCut->Draw("same");
    h2LeadTowPtVsJetPt_jetBkgCut_px->Draw("same");
    leg->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-pt-lead-tow.png").c_str());

    // ratio plots
    gPad->SetLogy(0);

    hz_jetBkgCut->Divide(hz);
    hz_xjCut->Divide(hz);
    h2LeadTowPtVsJetPt_jetBkgCut_px->Divide(hz);
    hz_jetBkgCut->SetTitle("Jet; p_{T} [GeV]; Ratio");
    hz_jetBkgCut->GetYaxis()->SetTitleOffset(1);

    leg = new TLegend(0.5,.7,0.7,.85);
    leg->SetFillStyle(0);
    leg->AddEntry(hz_jetBkgCut,"jet bkg cut/All","f");
    leg->AddEntry(hz_xjCut,"jet bkg cut and x_{j} #geq 0.15/All","f");

    hz_jetBkgCut->Draw();
    hz_xjCut->Draw("same");
    leg->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-pt-ratio.png").c_str());

    leg = new TLegend(0.25,.7,0.55,.85);
    leg->SetFillStyle(0);
    leg->AddEntry(hz_jetBkgCut,"jet bkg cut/All","f");
    leg->AddEntry(h2LeadTowPtVsJetPt_jetBkgCut_px,"jet bkg cut and lead tower p_{T} < 17 GeV/All","f");

    hz_jetBkgCut->Draw();
    h2LeadTowPtVsJetPt_jetBkgCut_px->Draw("same");
    leg->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-pt-lead-tow-ratio.png").c_str());

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
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-60.png").c_str());

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
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-60-phi.png").c_str());

    hy = hjetPhiEtaPt->Project3D("y");
    hy->SetTitle("Jet: p_{T} #geq 60 GeV; #eta; Counts");
    hy->GetYaxis()->SetTitleOffset(1.4);

    hy->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(hjetPhiEtaPt->GetName()) + "-60-eta.png").c_str());

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
    c1->Print((outputDir + "/" + string(hjetConstituentsVsPt->GetName()) + ".png").c_str());

    // ----------------------------------------------------------

    auto hNJetsVsLeadPt = (TH2*)input.Get("jets/hNJetsVsLeadPt");
    hNJetsVsLeadPt->RebinX(2);
    hNJetsVsLeadPt->GetXaxis()->SetRangeUser(10,200);
    hNJetsVsLeadPt->GetYaxis()->SetRangeUser(0,70);
    hNJetsVsLeadPt->GetYaxis()->SetTitleOffset(1);

    hNJetsVsLeadPt->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(hNJetsVsLeadPt->GetName()) + ".png").c_str());

    // ----------------------------------------------------------

    auto h2LeadTowPtFracVsJetPt = (TH2*)input.Get("jets/h2LeadTowPtFracVsJetPt");
    auto h2LeadTowPtFracVsJetPtCEMC = (TH2*)input.Get("jets/h2LeadTowPtFracVsJetPtCEMC");
    auto h2LeadTowPtFracVsJetPtOHCal = (TH2*)input.Get("jets/h2LeadTowPtFracVsJetPtOHCal");

    h2LeadTowPtFracVsJetPt->GetXaxis()->SetRangeUser(10,200);
    h2LeadTowPtFracVsJetPtCEMC->GetXaxis()->SetRangeUser(10,200);
    h2LeadTowPtFracVsJetPtOHCal->GetXaxis()->SetRangeUser(10,200);
    h2LeadTowPtFracVsJetPt->GetYaxis()->SetTitleOffset(1);
    h2LeadTowPtFracVsJetPtCEMC->GetYaxis()->SetTitleOffset(1);
    h2LeadTowPtFracVsJetPtOHCal->GetYaxis()->SetTitleOffset(1);

    h2LeadTowPtFracVsJetPt->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtFracVsJetPt->GetName()) + ".png").c_str());

    h2LeadTowPtFracVsJetPtCEMC->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtFracVsJetPtCEMC->GetName()) + ".png").c_str());

    h2LeadTowPtFracVsJetPtOHCal->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtFracVsJetPtOHCal->GetName()) + ".png").c_str());

    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.02);
    c1->SetBottomMargin(.15);

    auto h2LeadTowPtFracVsJetPt_10_20_py = h2LeadTowPtFracVsJetPt->ProjectionY("h2LeadTowPtFracVsJetPt_10_20",h2LeadTowPtFracVsJetPt->GetXaxis()->FindBin(10),h2LeadTowPtFracVsJetPt->GetXaxis()->FindBin(20)-1);
    auto h2LeadTowPtFracVsJetPt_20_40_py = h2LeadTowPtFracVsJetPt->ProjectionY("h2LeadTowPtFracVsJetPt_20_40",h2LeadTowPtFracVsJetPt->GetXaxis()->FindBin(20),h2LeadTowPtFracVsJetPt->GetXaxis()->FindBin(40)-1);
    auto h2LeadTowPtFracVsJetPt_60_py = h2LeadTowPtFracVsJetPt->ProjectionY("h2LeadTowPtFracVsJetPt_60",h2LeadTowPtFracVsJetPt->GetXaxis()->FindBin(60),h2LeadTowPtFracVsJetPt->GetNbinsX());
    auto h2LeadTowPtFracVsJetPtCEMC_60_py = h2LeadTowPtFracVsJetPtCEMC->ProjectionY("h2LeadTowPtFracVsJetPtCEMC_60",h2LeadTowPtFracVsJetPtCEMC->GetXaxis()->FindBin(60),h2LeadTowPtFracVsJetPtCEMC->GetNbinsX());
    auto h2LeadTowPtFracVsJetPtOHCal_60_py = h2LeadTowPtFracVsJetPtOHCal->ProjectionY("h2LeadTowPtFracVsJetPtOHCal_60",h2LeadTowPtFracVsJetPtOHCal->GetXaxis()->FindBin(60),h2LeadTowPtFracVsJetPtOHCal->GetNbinsX());
    h2LeadTowPtFracVsJetPt_10_20_py->SetTitle("Jet; Leading Tower p_{T}/#sum Tower p_{T}; Normalized Counts");
    h2LeadTowPtFracVsJetPt_60_py->SetTitle("Jet; Leading Tower p_{T}/#sum Tower p_{T}; Counts");
    h2LeadTowPtFracVsJetPt_10_20_py->GetYaxis()->SetTitleOffset(1.4);
    h2LeadTowPtFracVsJetPt_10_20_py->GetXaxis()->SetTitleOffset(1.35);
    h2LeadTowPtFracVsJetPt_60_py->GetYaxis()->SetTitleOffset(1.4);
    h2LeadTowPtFracVsJetPt_60_py->GetXaxis()->SetTitleOffset(1.35);

    h2LeadTowPtFracVsJetPt_10_20_py->SetLineColor(kGreen+3);
    h2LeadTowPtFracVsJetPt_20_40_py->SetLineColor(kBlue);
    h2LeadTowPtFracVsJetPt_60_py->SetLineColor(kRed);
    h2LeadTowPtFracVsJetPtCEMC_60_py->SetLineColor(kBlue);
    h2LeadTowPtFracVsJetPtOHCal_60_py->SetLineColor(kGreen+3);

    h2LeadTowPtFracVsJetPt_10_20_py->Rebin(5);
    h2LeadTowPtFracVsJetPt_20_40_py->Rebin(5);
    h2LeadTowPtFracVsJetPt_60_py->Rebin(5);
    h2LeadTowPtFracVsJetPtCEMC_60_py->Rebin(5);
    h2LeadTowPtFracVsJetPtOHCal_60_py->Rebin(5);

    Int_t counts_h2LeadTowPtFracVsJetPt_60_py = h2LeadTowPtFracVsJetPt_60_py->Integral();

    h2LeadTowPtFracVsJetPt_10_20_py->Scale(1./h2LeadTowPtFracVsJetPt_10_20_py->Integral());
    h2LeadTowPtFracVsJetPt_20_40_py->Scale(1./h2LeadTowPtFracVsJetPt_20_40_py->Integral());
    h2LeadTowPtFracVsJetPt_60_py->Scale(1./counts_h2LeadTowPtFracVsJetPt_60_py);
    // h2LeadTowPtFracVsJetPtCEMC_60_py->Scale(1./h2LeadTowPtFracVsJetPtCEMC_60_py->Integral());
    // h2LeadTowPtFracVsJetPtOHCal_60_py->Scale(1./h2LeadTowPtFracVsJetPtOHCal_60_py->Integral());

    Float_t mean_10_20 = h2LeadTowPtFracVsJetPt_10_20_py->GetMean();
    Float_t mean_20_40 = h2LeadTowPtFracVsJetPt_20_40_py->GetMean();
    Float_t mean_60 = h2LeadTowPtFracVsJetPt_60_py->GetMean();
    Float_t mean_CEMC_60 = h2LeadTowPtFracVsJetPtCEMC_60_py->GetMean();
    Float_t mean_OHCal_60 = h2LeadTowPtFracVsJetPtOHCal_60_py->GetMean();

    stringstream leg_10_20;
    stringstream leg_20_40;
    stringstream leg_60;
    stringstream leg_CEMC_60;
    stringstream leg_OHCal_60;

    leg_10_20 << "10 GeV #leq Jet p_{T} < 20 GeV, #mu = " << (Int_t)(mean_10_20*1000)/1000.;
    leg_20_40 << "20 GeV #leq Jet p_{T} < 40 GeV, #mu = " << (Int_t)(mean_20_40*1000)/1000.;
    leg_60 << "Jet p_{T} #geq 60 GeV, #mu = " << (Int_t)(mean_60*1000)/1000.;
    leg_CEMC_60 << "CEMC: Jet p_{T} #geq 60 GeV, #mu = " << (Int_t)(mean_CEMC_60*1000)/1000.;
    leg_OHCal_60 << "OHCal: Jet p_{T} #geq 60 GeV, #mu = " << (Int_t)(mean_OHCal_60*1000)/1000.;

    leg = new TLegend(0.3,.68,0.5,.88);
    leg->SetFillStyle(0);
    leg->AddEntry(h2LeadTowPtFracVsJetPt_10_20_py,leg_10_20.str().c_str(),"f");
    leg->AddEntry(h2LeadTowPtFracVsJetPt_20_40_py,leg_20_40.str().c_str(),"f");
    leg->AddEntry(h2LeadTowPtFracVsJetPt_60_py,leg_60.str().c_str(),"f");

    h2LeadTowPtFracVsJetPt_10_20_py->GetYaxis()->SetRangeUser(0,0.2);

    h2LeadTowPtFracVsJetPt_10_20_py->Draw("hist");
    h2LeadTowPtFracVsJetPt_20_40_py->Draw("same hist");
    h2LeadTowPtFracVsJetPt_60_py->Draw("same hist");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtFracVsJetPt->GetName()) + "-py.png").c_str());

    leg = new TLegend(0.3,.68,0.5,.88);
    leg->SetFillStyle(0);
    leg->AddEntry(h2LeadTowPtFracVsJetPt_60_py,leg_60.str().c_str(),"f");
    leg->AddEntry(h2LeadTowPtFracVsJetPtCEMC_60_py,leg_CEMC_60.str().c_str(),"f");
    leg->AddEntry(h2LeadTowPtFracVsJetPtOHCal_60_py,leg_OHCal_60.str().c_str(),"f");

    h2LeadTowPtFracVsJetPt_60_py->Scale(counts_h2LeadTowPtFracVsJetPt_60_py);
    h2LeadTowPtFracVsJetPt_60_py->GetYaxis()->SetRangeUser(0,180);

    cout << "Jets with pT >= 60 GeV:  " << counts_h2LeadTowPtFracVsJetPt_60_py
         << ", and leading tower from CEMC: " << h2LeadTowPtFracVsJetPtCEMC_60_py->Integral()
         << ", and leading tower from OHCal: " << h2LeadTowPtFracVsJetPtOHCal_60_py->Integral() << endl;

    h2LeadTowPtFracVsJetPt_60_py->Draw("hist");
    h2LeadTowPtFracVsJetPtCEMC_60_py->Draw("same hist");
    h2LeadTowPtFracVsJetPtOHCal_60_py->Draw("same hist");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtFracVsJetPt_60_py->GetName()) + ".png").c_str());

    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.12);
    c1->SetBottomMargin(.12);

    auto h2LeadTowPtFracVsJetPt_jetBkgCut = (TH2*)input.Get("jets/h2LeadTowPtFracVsJetPt_jetBkgCut");
    h2LeadTowPtFracVsJetPt_jetBkgCut->GetXaxis()->SetRangeUser(10,200);
    h2LeadTowPtFracVsJetPt_jetBkgCut->GetYaxis()->SetTitleOffset(1);

    h2LeadTowPtFracVsJetPt_jetBkgCut->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtFracVsJetPt_jetBkgCut->GetName()) + ".png").c_str());

    // ----------------------------------------------------------

    gPad->SetLogz();
    auto h2XjVsJetPt = (TH2*)input.Get("jets/h2XjVsJetPt");
    h2XjVsJetPt->GetXaxis()->SetRangeUser(10,200);
    h2XjVsJetPt->GetYaxis()->SetTitleOffset(1);

    h2XjVsJetPt->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2XjVsJetPt->GetName()) + ".png").c_str());

    auto py = h2XjVsJetPt->ProjectionY();
    py->GetYaxis()->SetTitle("Counts");
    py->GetYaxis()->SetTitleOffset(1.5);
    py->Draw();
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2XjVsJetPt->GetName()) + "-py.png").c_str());

    auto py_60 = h2XjVsJetPt->ProjectionY("py_60",h2XjVsJetPt->GetXaxis()->FindBin(60), h2XjVsJetPt->GetNbinsX());
    py_60->SetTitle("Event: Lead Jet p_{T} #geq 60 GeV; x_{j}; Counts");
    py_60->GetYaxis()->SetTitleOffset(1.5);
    py_60->Draw();
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2XjVsJetPt->GetName()) + "-py-60.png").c_str());

    auto h2XjVsJetPt_jetBkgCut = (TH2*)input.Get("jets/h2XjVsJetPt_jetBkgCut");
    h2XjVsJetPt_jetBkgCut->GetXaxis()->SetRangeUser(10,200);
    h2XjVsJetPt_jetBkgCut->GetYaxis()->SetTitleOffset(1);

    h2XjVsJetPt_jetBkgCut->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2XjVsJetPt_jetBkgCut->GetName()) + ".png").c_str());

    auto py_jetBkgCut = h2XjVsJetPt_jetBkgCut->ProjectionY();
    py_jetBkgCut->GetYaxis()->SetTitle("Counts");
    py_jetBkgCut->GetYaxis()->SetTitleOffset(1.5);
    py_jetBkgCut->Draw();
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2XjVsJetPt_jetBkgCut->GetName()) + "-py.png").c_str());

    auto py_jetBkgCut_60 = h2XjVsJetPt_jetBkgCut->ProjectionY("py_jetBkgCut_60",h2XjVsJetPt_jetBkgCut->GetXaxis()->FindBin(60), h2XjVsJetPt_jetBkgCut->GetNbinsX());
    py_jetBkgCut_60->GetYaxis()->SetTitle("Counts");
    py_jetBkgCut_60->SetTitle("Event: Lead Jet p_{T} #geq 60 GeV; x_{j}; Counts");
    py_jetBkgCut_60->GetYaxis()->SetTitleOffset(1.5);
    py_jetBkgCut_60->Draw();
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2XjVsJetPt_jetBkgCut->GetName()) + "-py-60.png").c_str());

    // ---------------------------------------------------

    auto h2LeadTowPtVsJetPt = (TH2*)input.Get("jets/h2LeadTowPtVsJetPt");
    auto h2LeadTowPtVsJetPtCEMC = (TH2*)input.Get("jets/h2LeadTowPtVsJetPtCEMC");
    auto h2LeadTowPtVsJetPtOHCal = (TH2*)input.Get("jets/h2LeadTowPtVsJetPtOHCal");
    h2LeadTowPtVsJetPt->GetXaxis()->SetRangeUser(10,200);
    h2LeadTowPtVsJetPt->GetYaxis()->SetTitleOffset(1.2);
    h2LeadTowPtVsJetPt->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtVsJetPt->GetName()) + ".png").c_str());

    h2LeadTowPtVsJetPtCEMC->GetXaxis()->SetRangeUser(10,200);
    h2LeadTowPtVsJetPtCEMC->GetYaxis()->SetTitleOffset(1.2);
    h2LeadTowPtVsJetPtCEMC->SetTitle("Jet: Lead p_{T} Tower from CEMC");
    h2LeadTowPtVsJetPtCEMC->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtVsJetPtCEMC->GetName()) + ".png").c_str());

    h2LeadTowPtVsJetPtOHCal->GetXaxis()->SetRangeUser(10,200);
    h2LeadTowPtVsJetPtOHCal->GetYaxis()->SetTitleOffset(1.2);
    h2LeadTowPtVsJetPtOHCal->SetTitle("Jet: Lead p_{T} Tower from OHCal");
    h2LeadTowPtVsJetPtOHCal->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtVsJetPtOHCal->GetName()) + ".png").c_str());

    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.04);
    c1->SetBottomMargin(.12);

    auto h2LeadTowPtVsJetPt_10_60_py = h2LeadTowPtVsJetPt->ProjectionY("h2LeadTowPtVsJetPt_10_60_py", h2LeadTowPtVsJetPt->GetXaxis()->FindBin(10), h2LeadTowPtVsJetPt->GetXaxis()->FindBin(60)-1);
    auto h2LeadTowPtVsJetPt_60_py = h2LeadTowPtVsJetPt->ProjectionY("h2LeadTowPtVsJetPt_60_py", h2LeadTowPtVsJetPt->GetXaxis()->FindBin(60), h2LeadTowPtVsJetPt->GetNbinsX());
    auto h2LeadTowPtVsJetPtCEMC_60_py = h2LeadTowPtVsJetPtCEMC->ProjectionY("h2LeadTowPtVsJetPtCEMC_60_py", h2LeadTowPtVsJetPtCEMC->GetXaxis()->FindBin(60), h2LeadTowPtVsJetPtCEMC->GetNbinsX());
    auto h2LeadTowPtVsJetPtOHCal_60_py = h2LeadTowPtVsJetPtOHCal->ProjectionY("h2LeadTowPtVsJetPtOHCal_60_py", h2LeadTowPtVsJetPtOHCal->GetXaxis()->FindBin(60), h2LeadTowPtVsJetPtOHCal->GetNbinsX());

    h2LeadTowPtVsJetPt_10_60_py->Rebin(5);
    h2LeadTowPtVsJetPt_60_py->Rebin(5);
    h2LeadTowPtVsJetPtCEMC_60_py->Rebin(5);
    h2LeadTowPtVsJetPtOHCal_60_py->Rebin(5);

    h2LeadTowPtVsJetPt_60_py->SetLineColor(kRed);
    h2LeadTowPtVsJetPtCEMC_60_py->SetLineColor(kBlue);
    h2LeadTowPtVsJetPtOHCal_60_py->SetLineColor(kGreen+3);

    h2LeadTowPtVsJetPt_10_60_py->SetTitle("Jet; Leading Tower p_{T} [GeV]; Counts / 2.5 GeV");
    h2LeadTowPtVsJetPt_10_60_py->GetYaxis()->SetTitleOffset(1.4);
    h2LeadTowPtVsJetPt_10_60_py->GetXaxis()->SetTitleOffset(1.1);

    h2LeadTowPtVsJetPt_60_py->SetTitle("Jet; Leading Tower p_{T} [GeV]; Counts / 2.5 GeV");
    h2LeadTowPtVsJetPt_60_py->GetYaxis()->SetTitleOffset(1.4);
    h2LeadTowPtVsJetPt_60_py->GetXaxis()->SetTitleOffset(1.1);

    Float_t mean_10_60 = h2LeadTowPtVsJetPt_10_60_py->GetMean();
    mean_60 = h2LeadTowPtVsJetPt_60_py->GetMean();
    mean_CEMC_60 = h2LeadTowPtVsJetPtCEMC_60_py->GetMean();
    mean_OHCal_60 = h2LeadTowPtVsJetPtOHCal_60_py->GetMean();

    stringstream leg_10_60;
    leg_60.str("");
    leg_CEMC_60.str("");
    leg_OHCal_60.str("");

    leg_10_60 << "10 GeV #leq Jet p_{T} < 60 GeV, #mu = " << (Int_t)(mean_10_60*100)/100. << " GeV";
    leg_60 << "Jet p_{T} #geq 60 GeV, #mu = " << (Int_t)(mean_60*100)/100. << " GeV";
    leg_CEMC_60 << "CEMC: Jet p_{T} #geq 60 GeV, #mu = " << (Int_t)(mean_CEMC_60*100)/100. << " GeV";
    leg_OHCal_60 << "OHCal: Jet p_{T} #geq 60 GeV, #mu = " << (Int_t)(mean_OHCal_60*100)/100. << " GeV";

    leg = new TLegend(0.3,.68,0.5,.88);
    leg->SetFillStyle(0);
    leg->AddEntry(h2LeadTowPtVsJetPt_10_60_py,leg_10_60.str().c_str(),"f");
    leg->AddEntry(h2LeadTowPtVsJetPt_60_py,leg_60.str().c_str(),"f");

    gPad->SetLogy();
    h2LeadTowPtVsJetPt_10_60_py->Draw("hist");
    h2LeadTowPtVsJetPt_60_py->Draw("same hist");
    leg->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtVsJetPt_10_60_py->GetName()) + ".png").c_str());
    gPad->SetLogy(0);

    leg = new TLegend(0.3,.68,0.5,.88);
    leg->SetFillStyle(0);
    leg->AddEntry(h2LeadTowPtVsJetPt_60_py,leg_60.str().c_str(),"f");
    leg->AddEntry(h2LeadTowPtVsJetPtCEMC_60_py,leg_CEMC_60.str().c_str(),"f");
    leg->AddEntry(h2LeadTowPtVsJetPtOHCal_60_py,leg_OHCal_60.str().c_str(),"f");

    h2LeadTowPtVsJetPt_60_py->GetYaxis()->SetRangeUser(0,105);

    cout << "Jets with pT >= 60 GeV:  " << h2LeadTowPtVsJetPt_60_py->Integral()
         << ", and leading tower from CEMC: " << h2LeadTowPtVsJetPtCEMC_60_py->Integral()
         << ", and leading tower from OHCal: " << h2LeadTowPtVsJetPtOHCal_60_py->Integral() << endl;

    h2LeadTowPtVsJetPt_60_py->Draw("hist");
    h2LeadTowPtVsJetPtCEMC_60_py->Draw("same hist");
    h2LeadTowPtVsJetPtOHCal_60_py->Draw("same hist");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtVsJetPt_60_py->GetName()) + ".png").c_str());

    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.12);
    c1->SetBottomMargin(.12);

    h2LeadTowPtVsJetPt_jetBkgCut->GetYaxis()->SetTitleOffset(1.2);
    h2LeadTowPtVsJetPt_jetBkgCut->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtVsJetPt_jetBkgCut->GetName()) + ".png").c_str());

    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.04);
    c1->SetBottomMargin(.12);

    auto h2LeadTowPtVsJetPt_jetBkgCut_10_60_py = h2LeadTowPtVsJetPt_jetBkgCut->ProjectionY("h2LeadTowPtVsJetPt_jetBkgCut_10_60_py", h2LeadTowPtVsJetPt_jetBkgCut->GetXaxis()->FindBin(10), h2LeadTowPtVsJetPt_jetBkgCut->GetXaxis()->FindBin(60)-1);
    auto h2LeadTowPtVsJetPt_jetBkgCut_60_py = h2LeadTowPtVsJetPt_jetBkgCut->ProjectionY("h2LeadTowPtVsJetPt_jetBkgCut_60_py", h2LeadTowPtVsJetPt_jetBkgCut->GetXaxis()->FindBin(60), h2LeadTowPtVsJetPt_jetBkgCut->GetNbinsX());

    h2LeadTowPtVsJetPt_jetBkgCut_10_60_py->Rebin(5);
    h2LeadTowPtVsJetPt_jetBkgCut_60_py->Rebin(5);

    h2LeadTowPtVsJetPt_jetBkgCut_60_py->SetLineColor(kRed);

    h2LeadTowPtVsJetPt_jetBkgCut_10_60_py->SetTitle("Jet; Leading Tower p_{T} [GeV]; Counts / 2.5 GeV");
    h2LeadTowPtVsJetPt_jetBkgCut_10_60_py->GetYaxis()->SetTitleOffset(1.4);
    h2LeadTowPtVsJetPt_jetBkgCut_10_60_py->GetXaxis()->SetTitleOffset(1.1);

    mean_10_60 = h2LeadTowPtVsJetPt_jetBkgCut_10_60_py->GetMean();
    mean_60 = h2LeadTowPtVsJetPt_jetBkgCut_60_py->GetMean();

    leg_10_60.str("");
    leg_60.str("");

    leg_10_60 << "10 GeV #leq Jet p_{T} < 60 GeV, #mu = " << (Int_t)(mean_10_60*100)/100. << " GeV";
    leg_60 << "Jet p_{T} #geq 60 GeV, #mu = " << (Int_t)(mean_60*100)/100. << " GeV";

    leg = new TLegend(0.3,.68,0.5,.88);
    leg->SetFillStyle(0);
    leg->AddEntry(h2LeadTowPtVsJetPt_jetBkgCut_10_60_py,leg_10_60.str().c_str(),"f");
    leg->AddEntry(h2LeadTowPtVsJetPt_jetBkgCut_60_py,leg_60.str().c_str(),"f");

    gPad->SetLogy();
    h2LeadTowPtVsJetPt_jetBkgCut_10_60_py->Draw("hist");
    h2LeadTowPtVsJetPt_jetBkgCut_60_py->Draw("same hist");
    leg->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2LeadTowPtVsJetPt_jetBkgCut_10_60_py->GetName()) + ".png").c_str());
    gPad->SetLogy(0);

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

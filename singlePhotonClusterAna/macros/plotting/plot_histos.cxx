#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <THStack.h>
#include <TF1.h>
#include <TLatex.h>
#include <TPaveLabel.h>
#include <TFile.h>
#include <TStyle.h>
#include <TColor.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TRatioPlot.h>
#include <TObjArray.h>
#include <TFractionFitter.h>
#include <TFitter.h>
#include <TLine.h>
#include <TROOT.h>
#include <TString.h>
#include <TBox.h>

#include <iostream>
#include <string>
#include <vector>
//#include <cstdio>
#include "BinnedHistSet.C"

void normalize_hist(TH1* hist, double norm=1.0) {
    hist->Sumw2();
    hist->Scale(norm/hist->Integral());
}
void set_draw_style(TH1* hist, int color, int marker_style=7) {
//    hist->UseCurrentStyle();
    hist->SetStats(10);
    /* hist->SetLineColor(color); */
    hist->SetMarkerStyle(marker_style);
    /* hist->SetMarkerSize(hist->GetMarkerSize()*1.5); */
    hist->SetMarkerSize(1.5);
    hist->SetMarkerColor(color);
    hist->SetLineColor(color);
    hist->GetXaxis()->CenterTitle(true);
    hist->GetXaxis()->SetLabelFont(42);
    hist->GetXaxis()->SetLabelSize(0.035);
    hist->GetXaxis()->SetTitleFont(42);
    hist->GetYaxis()->CenterTitle(true);
    hist->GetYaxis()->SetLabelFont(42);
    hist->GetYaxis()->SetLabelSize(0.035);
    hist->GetYaxis()->SetTitleSize(0.035);
    hist->GetYaxis()->SetTitleFont(42);
}

void set_leg_entry_style(TLegendEntry* entry) {
   TH1F* hist = (TH1F*)entry->GetObject();
   entry->SetFillStyle(1001);
   entry->SetLineColor(hist->GetLineColor());
   entry->SetLineStyle(hist->GetLineStyle());
   entry->SetLineWidth(hist->GetLineWidth());
   entry->SetMarkerColor(hist->GetMarkerColor());
   entry->SetMarkerStyle(hist->GetMarkerStyle());
   entry->SetMarkerSize(hist->GetMarkerSize());
   entry->SetTextFont(42);
}

void draw_pad(TCanvas* canvas, TH1* hist, TPad* pad) {
    pad->Draw();
    pad->cd();
    pad->SetFillColor(0);
    pad->SetBorderMode(0);
    pad->SetBorderSize(2);
    pad->SetFrameBorderMode(0);
    pad->SetFrameBorderMode(0);
    hist->Draw("pe");
    //hist->Draw("lp");
    /* pad->Modified(); */
    canvas->cd();
}


// Plotting functions

void SetupCanvas(TCanvas* c) {
    c->Range(0,0,1,1);
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(2);
    c->SetFrameBorderMode(0);
}

void SetupPad(TPad* pad, bool logy=false) {
    pad->Clear();
    pad->Draw();
    pad->cd();
    pad->SetFillColor(0);
    pad->SetBorderMode(0);
    pad->SetBorderSize(2);
    pad->SetFrameBorderMode(0);
    pad->SetFrameBorderMode(0);
    pad->SetLogy(logy);
    pad->SetLeftMargin(0.12);
}

void SetupLegend(TLegend* leg, double textsize=0.03) {
    leg->SetBorderSize(0);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(textsize);
    leg->SetTextAlign(12);
    leg->Clear();
}

void HistArrSetStyle(int n_hists, TH1** hists_arr, std::string title, int markerstyle=8, int starting_color=1, bool emphasize_last=false) {
    int i_color;
    int i_marker = markerstyle;
    int markers[13] = {20, 22, 23, 21, 34, 29, 47, 39, 41, 43, 45, 48, 49};
    for (int i=0; i<n_hists; i++) {
	/* std::cout << "Loop i=" << i << "; calling set_draw_style. hists_arr[" << i << "] = " << hists_arr[i] << "\n"; */
	i_color = starting_color + i;
	if (i_color >= 3) i_color++; // Avoid 3=green
	if (i_color >= 5) i_color++; // Avoid 5=yellow
	if (markerstyle == 0) i_marker = markers[i%13];
	set_draw_style(hists_arr[i], i_color, i_marker);
	if (markerstyle==8) hists_arr[i]->SetMarkerSize(0.8);
	hists_arr[i]->SetStats(0);
    }
    hists_arr[0]->SetTitle(title.c_str());
    // Make last entry in array stand out
    if (emphasize_last) {
	hists_arr[n_hists-1]->SetMarkerStyle(22);
	hists_arr[n_hists-1]->SetMarkerSize(1.1);
    }
    /* hists_arr[n_hists-1]->SetMarkerColor(7); */
}


void RescaleHists(int n_hists, TH1** hists_arr, double* norms) {
    for (int i=0; i<n_hists; i++) {
	normalize_hist(hists_arr[i], norms[i]);
    }
}

void RescaleHists(int n_hists, TH1** hists_arr, double norm) {
    for (int i=0; i<n_hists; i++) {
	normalize_hist(hists_arr[i], norm);
    }
}

void RescaleHistsToOriginal(int n_hists, TH1** hists_arr) {
    for (int i=0; i<n_hists; i++) {
	normalize_hist(hists_arr[i], hists_arr[i]->GetEntries());
    }
}

void DrawHistsOnPad(TPad* pad, int n_hists, TH1** hists_arr, std::string* labels_arr, TLegend* leg, std::string draw_options="E1 X0 NOSTACK") {
    pad->cd();
    pad->Clear();
    // THStack to draw all hists and set Y axis scale properly
    THStack* hs_temp = new THStack("hs_temp", "");
    hs_temp->SetTitle(hists_arr[0]->GetTitle());
    for (int j=0; j<n_hists; j++) {
	hs_temp->Add(hists_arr[j]);
    }
    hs_temp->Draw(draw_options.c_str());
    hs_temp->GetXaxis()->SetTitle(hists_arr[0]->GetXaxis()->GetTitle());
    hs_temp->GetXaxis()->CenterTitle(true);
    // legend
    if (leg) {
	SetupLegend(leg);
	TLegendEntry* entries[n_hists];
	TLegendEntry* entry;
	for (int k=0; k<n_hists; k++) {
	    entry=leg->AddEntry(hists_arr[k],labels_arr[k].c_str(),"p");
	    set_leg_entry_style(entry);
	}
	leg->Draw();
    }
    pad->Modified();
}

void DrawHistsOnPadFixedScale(TPad* pad, int n_hists, TH1** hists_arr, std::string* labels_arr, TLegend* leg, double* minmax, std::string draw_options="E1 X0 NOSTACK") {
    pad->cd();
    pad->Clear();
    hists_arr[0]->Draw("E1 X0");
    hists_arr[0]->GetYaxis()->SetRangeUser(minmax[0], minmax[1]);
    for (int i=1; i<n_hists; i++) hists_arr[i]->Draw("E1 X0 SAME");
    // legend
    if (leg) {
	SetupLegend(leg);
	TLegendEntry* entries[n_hists];
	TLegendEntry* entry;
	for (int k=0; k<n_hists; k++) {
	    entry=leg->AddEntry(hists_arr[k],labels_arr[k].c_str(),"p");
	    set_leg_entry_style(entry);
	}
	leg->Draw();
    }
    pad->Modified();
}

void PlotSingleHist(TH1* hist, std::string outpdfname, bool logy=false, bool normalized=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, logy);
    if (normalized) {
	normalize_hist(hist);
	hist->GetYaxis()->SetTitle("Normalized Counts");
    }
    /* else hist->GetYaxis()->SetTitle("Counts"); */
    set_draw_style(hist, 1, 20);
    hist->SetStats(0);
    hist->Draw("e1 x0");
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1;
}

void PlotnClusters(TH1* hist, std::string outpdfname, bool logy=false, bool normalized=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, logy);
    if (normalized) {
	normalize_hist(hist);
	hist->GetYaxis()->SetTitle("Normalized Counts");
    }
    /* else hist->GetYaxis()->SetTitle("Counts"); */
    set_draw_style(hist, 1, 20);
    hist->SetStats(0);
    hist->Draw("e1 x0");
    TLatex* textbox = new TLatex(0.75, 0.5, "blaahhh");
    /* textbox->SetTextSize(textbox->GetTextSize()/1.25); */
    textbox->SetTextColor(kRed);
    textbox->SetTextAlign(21);
    textbox->DrawLatexNDC(0.24, 0.28, Form("<%.1f%%", ceil(1000*hist->GetBinContent(1))/10));
    textbox->DrawLatexNDC(0.41, 0.82, Form("%.1f%%", 100*hist->GetBinContent(2)));
    textbox->DrawLatexNDC(0.56, 0.6, Form("%.1f%%", 100*hist->GetBinContent(3)));
    textbox->DrawLatexNDC(0.72, 0.4, Form("<%.1f%%", ceil(1000*hist->GetBinContent(4))/10));
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1;
    delete textbox;
}

void PlotnClusters2(TH1* hist, std::string outpdfname, bool logy=false, bool normalized=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, logy);
    if (normalized) {
	normalize_hist(hist);
	hist->GetYaxis()->SetTitle("Normalized Counts");
    }
    /* else hist->GetYaxis()->SetTitle("Counts"); */
    set_draw_style(hist, 1, 20);
    hist->SetStats(0);
    hist->Draw("e1 x0");
    TLatex* textbox = new TLatex(0.75, 0.5, "blaahhh");
    /* textbox->SetTextSize(textbox->GetTextSize()/1.25); */
    textbox->SetTextColor(kRed);
    textbox->SetTextAlign(21);
    /* textbox->DrawLatexNDC(0.24, 0.28, Form("<%.1f%%", ceil(1000*hist->GetBinContent(1))/10)); */
    textbox->DrawLatexNDC(0.41, 0.80, Form("%.1f%%", 100*hist->GetBinContent(2)));
    textbox->DrawLatexNDC(0.57, 0.76, Form("%.1f%%", 100*hist->GetBinContent(3)));
    textbox->DrawLatexNDC(0.72, 0.55, Form("%.1f%%", 100*hist->GetBinContent(4)));
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1;
    delete textbox;
}

void PlotVertexR(TH1* hist, std::string outpdfname, bool logy=false, bool normalized=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, logy);
    if (normalized) {
	normalize_hist(hist);
	hist->GetYaxis()->SetTitle("Normalized Counts");
    }
    /* else hist->GetYaxis()->SetTitle("Counts"); */
    set_draw_style(hist, 1, 20);
    hist->SetStats(0);
    hist->Draw("e1 x0");
    c1->Update();
    double y1 = c1->GetUymin();
    double y2 = c1->GetUymax();

    // EMCal 97.5 - 113cm
    TBox* bemc = new TBox(97.5, y1, 113.2, y2);
    bemc->SetLineColorAlpha(46, 0.5);
    bemc->SetLineStyle(kDotted);
    bemc->SetFillColorAlpha(46, 0.35);
    /* bemc->SetFillStyle(3244); */
    bemc->Draw("same");
    TLatex* temc = new TLatex(0.75, 0.5, "blaahhh");
    temc->SetTextColor(46);
    temc->SetTextAlign(21);
    temc->DrawLatex((97.5+113.2)/2, 1.015*y2, "EMCal");

    // TPC 20 - 78cm (30-78 active volume)
    TBox* btpc = new TBox(20, y1, 78, y2);
    btpc->SetLineColorAlpha(42, 0.5);
    btpc->SetLineStyle(kDotted);
    btpc->SetFillColorAlpha(42, 0.35);
    /* btpc->SetFillStyle(3244); */
    btpc->Draw("same");
    TLatex* ttpc = new TLatex(0.75, 0.5, "blaahhh");
    ttpc->SetTextColor(42);
    ttpc->SetTextAlign(21);
    ttpc->DrawLatex((20+78)/2, 1.015*y2, "TPC");
    // INTT 6 - 12cm
    TBox* bintt = new TBox(6, y1, 12, y2);
    bintt->SetLineColorAlpha(38, 0.5);
    bintt->SetLineStyle(kDotted);
    bintt->SetFillColorAlpha(38, 0.35);
    /* bintt->SetFillStyle(3244); */
    bintt->Draw("same");
    TLatex* tintt = new TLatex(0.75, 0.5, "blaahhh");
    tintt->SetTextColor(38);
    tintt->SetTextAlign(21);
    /* tintt->DrawLatex((6+12)/2, 0.80*y2, "INTT"); */
    tintt->DrawLatex(16, 0.80*y2, "INTT");
    // MVTX 2.37 - 4.34cm
    TBox* bmvtx = new TBox(2.37, y1, 4.34, y2);
    bmvtx->SetLineColorAlpha(31, 0.5);
    bmvtx->SetLineStyle(kDotted);
    bmvtx->SetFillColorAlpha(31, 0.35);
    /* bmvtx->SetFillStyle(3244); */
    bmvtx->Draw("same");
    TLatex* tmvtx = new TLatex(0.75, 0.5, "blaahhh");
    tmvtx->SetTextColor(31);
    tmvtx->SetTextAlign(21);
    /* tmvtx->DrawLatex((2.37+4.34)/2, 0.90*y2, "MVTX"); */
    tmvtx->DrawLatex(12, 0.90*y2, "MVTX");
    // Beam pipe 2 - 2.08cm
    TBox* bbp = new TBox(0.0, y1, 2.08, y2);
    bbp->SetLineColorAlpha(27, 0.5);
    bbp->SetLineStyle(kDotted);
    bbp->SetFillColorAlpha(27, 0.35);
    /* bbp->SetFillStyle(3244); */
    bbp->Draw("same");
    TLatex* tbp = new TLatex(0.75, 0.5, "blaahhh");
    tbp->SetTextColor(27);
    tbp->SetTextAlign(21);
    tbp->DrawLatex((0+2.08)/2, 1.015*y2, "Beam Pipe");
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1;
    delete bemc; delete temc;
    delete btpc; delete ttpc;
    delete bintt; delete tintt;
    delete bmvtx; delete tmvtx;
    delete bbp; delete tbp;
}

void PlotVertexRZoomed(TH1* hist, std::string outpdfname, bool logy=false, bool normalized=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, logy);
    if (normalized) {
	normalize_hist(hist);
	hist->GetYaxis()->SetTitle("Normalized Counts");
    }
    /* else hist->GetYaxis()->SetTitle("Counts"); */
    set_draw_style(hist, 1, 20);
    hist->SetStats(0);
    hist->Draw("e1 x0");
    c1->Update();
    double y1 = c1->GetUymin();
    double y2 = c1->GetUymax();

    // Light guides 97.5 - 100.5cm
    TBox* bemc = new TBox(97.5, y1, 100.5, y2);
    bemc->SetLineColorAlpha(46, 0.5);
    bemc->SetLineStyle(kDotted);
    bemc->SetFillColorAlpha(46, 0.35);
    /* bemc->SetFillStyle(3244); */
    bemc->Draw("same");
    TLatex* temc = new TLatex(0.75, 0.5, "blaahhh");
    temc->SetTextColor(46);
    temc->SetTextAlign(21);
    temc->DrawLatex((97.5+100.5)/2, 1.015*y2, "Light Guides");
    // Tungsten 100.5 - 113cm
    TBox* bemc2 = new TBox(100.5, y1, 113.2, y2);
    bemc2->SetLineColorAlpha(44, 0.5);
    bemc2->SetLineStyle(kDotted);
    bemc2->SetFillColorAlpha(44, 0.35);
    /* bemc2->SetFillStyle(3244); */
    bemc2->Draw("same");
    TLatex* temc2 = new TLatex(0.75, 0.5, "blaahhh");
    temc2->SetTextColor(44);
    temc2->SetTextAlign(21);
    temc2->DrawLatex((100.5+113.2)/2, 1.015*y2, "Tungsten");

    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1;
    delete bemc; delete temc;
    delete bemc2; delete temc2;
}
void PlotSingleHistWithFit(TH1* hist, std::string outpdfname, double fit_min, double fit_max, bool logy=false, bool normalized=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, logy);
    if (normalized) {
	normalize_hist(hist);
	hist->GetYaxis()->SetTitle("Normalized Counts");
    }
    /* else hist->GetYaxis()->SetTitle("Counts"); */
    set_draw_style(hist, 1, 20);
    hist->SetStats(0);
    hist->Draw("e1 x0");
    hist->Fit("gaus", "", "", fit_min, fit_max);
    TF1* f1 = (TF1*)hist->GetFunction("gaus");
    f1->SetLineColor(kRed);
    TLatex* fit_latex = new TLatex(0.75, 0.5, "blaahhh");
    /* fit_latex->SetTextSize(fit_latex->GetTextSize()/1.25); */
    fit_latex->SetTextColor(kRed);
    fit_latex->SetTextAlign(21);
    fit_latex->DrawLatexNDC(0.72, 0.80, Form("#splitline{#mu=%.5f#pm%.5f}{#sigma=%.5f#pm%.5f}", f1->GetParameter(1), f1->GetParError(1), f1->GetParameter(2), f1->GetParError(2)));
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    hist->GetListOfFunctions()->Clear();
    delete c1;
    delete fit_latex;
}

void PlotSingleHistWithFit(TH1* hist, std::string outpdfname, bool logy=false, bool normalized=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, logy);
    if (normalized) {
	normalize_hist(hist);
	hist->GetYaxis()->SetTitle("Normalized Counts");
    }
    /* else hist->GetYaxis()->SetTitle("Counts"); */
    set_draw_style(hist, 1, 20);
    hist->SetStats(0);
    hist->Draw("e1 x0");

    hist->Fit("gaus", "S");
    TF1* fit = hist->GetFunction("gaus");
    if (!fit) {
	std::cout << "\n\nGreg info: empty fit function. Hist is " << hist << "; printing...\n";
	hist->Print();
    }
    double mean = fit->GetParameter(1);
    double stddev = fit->GetParameter(2);
    hist->GetListOfFunctions()->Clear();
    hist->Fit("gaus", "S", "", (mean-stddev), (mean+stddev));
    fit = hist->GetFunction("gaus");


    /* hist->Fit("gaus", "", "", fit_min, fit_max); */
    TF1* f1 = (TF1*)hist->GetFunction("gaus");
    f1->SetLineColor(kRed);
    TLatex* fit_latex = new TLatex(0.75, 0.5, "blaahhh");
    /* fit_latex->SetTextSize(fit_latex->GetTextSize()/1.25); */
    fit_latex->SetTextColor(kRed);
    fit_latex->SetTextAlign(21);
    fit_latex->DrawLatexNDC(0.72, 0.80, Form("#splitline{#mu=%.5f#pm%.5f}{#sigma=%.5f#pm%.5f}", f1->GetParameter(1), f1->GetParError(1), f1->GetParameter(2), f1->GetParError(2)));
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    hist->GetListOfFunctions()->Clear();
    delete c1;
    delete fit_latex;
    delete fit;
}

void PlotSingle2DHist(TH2* hist, std::string outpdfname) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, false);
    set_draw_style(hist, 1, 20);
    hist->SetStats(0);
    hist->Draw("colz");
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1;
}

void PlotOne(int n_hists, TH1** hists_arr, std::string* labels_arr, std::string title, std::string outpdfname, int markerstyle=7, bool logy=false, std::string draw_options="E1 X0 NOSTACK") {
    // 1-panel plots
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, logy);
    HistArrSetStyle(n_hists, hists_arr, title, markerstyle);
    /* TLegend* leg1 = new TLegend(0.70, 0.70, 0.90, 0.90, NULL, "nbNDC"); */
    /* TLegend* leg1 = new TLegend(0.80, 0.80, 0.90, 0.90, NULL, "nbNDC"); */
    TLegend* leg1 = new TLegend(0.20, 0.20, NULL, "nbNDC");
    SetupLegend(leg1, 0.025);
    /* leg1->SetNColumns(n_hists); */
    /* leg1->SetNRows(n_hists); */
    leg1->SetTextAlign(22);
    /* TLegend* leg1 = new TLegend(0.25, 0.25, NULL, "nbNDC"); // automatic placement */
    DrawHistsOnPad(c1, n_hists, hists_arr, labels_arr, leg1, draw_options);
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete leg1;
    delete c1;
}

void RatioPlot(TPad* pad, TH1* h1, TH1* h2, std::string title, int logy=0, double lmargin=0.10, double rmargin=0.10) {
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0.0);
    h1->SetTitle(title.c_str());
    pad->Clear();
    TRatioPlot* rp = new TRatioPlot(h1, h2);
    rp->SetGraphDrawOpt("P");
    rp->SetH1DrawOpt("E X0");
    rp->SetH2DrawOpt("E X0");
    rp->SetSeparationMargin(0.0);
    rp->Draw();
    rp->GetUpperPad()->SetLogy(logy);
    rp->SetSplitFraction(0.35);
    rp->SetLeftMargin(lmargin);
    rp->SetRightMargin(rmargin);
    /* rp->SetLeftMargin(0.10); */
    /* rp->SetRightMargin(0.10); */
    /* rp->GetUpperPad()->SetGridx(); */
    /* rp->GetLowerPad()->SetGridx(); */

    rp->GetLowerRefGraph()->SetMinimum(0.5);
    rp->GetLowerRefGraph()->SetMaximum(1.5);
    /* rp->GetLowerRefGraph()->SetMarkerStyle(7); */
    double the_only_gridline = 1.0;
    rp->SetGridlines(&the_only_gridline, 1);
    rp->GetLowYaxis()->SetNdivisions(505);
    pad->Modified();
}

void RatioPlot(TPad* pad, TH1* h1, TH1* h2, std::string* labels_arr, std::string title, TLegend* leg=NULL, int logy=0) {
    // h1 settings
    h1->SetTitle(title.c_str());

    // h1 Y axis plot settings
    h1->GetYaxis()->SetTitleSize(20);
    h1->GetYaxis()->SetTitleFont(42);
    h1->GetYaxis()->SetTitleOffset(1.55);

    // Do not draw the Y axis label on the upper plot and redraw a small
    // axis instead, in order to avoid the first label (0) to be clipped.
    /* h1->GetYaxis()->SetLabelSize(0.); */
    /* TGaxis* axis = new TGaxis( -5, 20, -5, 220, 20,220,510,""); */
    /* axis->SetLabelFont(42); // Absolute font size in pixel (precision 3) */
    /* axis->SetLabelSize(15); */

    // Upper plot will be in pad1
    pad->cd();
    TPad* pad1 = new TPad("pad1", "pad1", 0.0, 0.3, 1.0, 1.0);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->SetLogy(logy);
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
    h1->SetStats(0);          // No statistics on upper plot
    h1->Draw("E X0 P");               // Draw h1
    h2->Draw("E X0 P SAME");         // Draw h2 on top of h1
    // legend
    if (leg) {
	leg->Clear();
	TLegendEntry* entry1=leg->AddEntry(h1,labels_arr[0].c_str(),"p");
	set_leg_entry_style(entry1);
	TLegendEntry* entry2=leg->AddEntry(h2,labels_arr[1].c_str(),"p");
	set_leg_entry_style(entry2);
	leg->Draw();
    }

    // Lower plot will be in pad2
    pad->cd();          // Go back to the main canvas before defining pad2
    TPad* pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();       // pad2 becomes the current pad

    // Define the ratio plot
    TH1F* h3 = (TH1F*)h1->Clone("h3");
    h3->SetMinimum(0.6);  // Define Y ..
    h3->SetMaximum(1.4); // .. range
    h3->GetYaxis()->SetNdivisions(505); // Y-axis tick markers
    h3->SetStats(0);      // No statistics on lower plot
    h3->Sumw2();
    h3->Divide(h2);
    /* set_draw_style(h3, kBlack); */
    /* std::string ratio_title = ";;" + labels_arr[0] + "/" + labels_arr[1]; */
    /* h3->SetTitle(ratio_title.c_str()); // Remove the ratio plot title and set Y-axis title */
    /* h3->GetYaxis()->SetTitle("Ratio RD/MC"); */

    h3->SetLineColor(kBlack);
    h3->SetMarkerColor(kBlack);
    h3->SetMarkerStyle(7);
    h3->SetTitle(""); // Remove the ratio title

    // Y axis ratio plot settings
    h3->GetYaxis()->CenterTitle();
    h3->GetYaxis()->SetTitle("Ratio RD/MC");
    h3->GetYaxis()->SetNdivisions(505);
    h3->GetYaxis()->SetTitleSize(0.1);
    h3->GetYaxis()->SetTitleFont(42);
    h3->GetYaxis()->SetTitleOffset(1.25);
    h3->GetYaxis()->SetLabelFont(42); // Absolute font size in pixel (precision 3)
    h3->GetYaxis()->SetLabelSize(0.1);
    h3->GetYaxis()->SetLabelSize(0.1);

    // X axis ratio plot settings
    h3->GetXaxis()->CenterTitle(1);
    h3->GetXaxis()->SetTitleSize(0.1);
    h3->GetXaxis()->SetTitleFont(42);
    h3->GetXaxis()->SetTitleOffset(1.0);
    h3->GetXaxis()->SetLabelFont(42); // Absolute font size in pixel (precision 3)
    h3->GetXaxis()->SetLabelSize(0.1);

    h3->Draw("EP");       // Draw the ratio plot

    // Horizontal line at y=1
    TLine* l = new TLine(h1->GetXaxis()->GetXmin(),1.0,h1->GetXaxis()->GetXmax(),1.0);
    l->SetLineColor(kRed);
    l->Draw();

    pad2->Modified();
    pad->cd();
    /* pad->ls(); */
    pad->Modified();
    /* pad->SaveAs(outpdfname.c_str()); */

    /* delete pad1; */
    /* delete pad2; */
    /* delete axis; */
    /* delete leg01; */
    /* delete l; */
}

void PlotOneRatio(TH1* h1, TH1* h2, std::string* labels_arr, std::string title, std::string outpdfname, TLegend* leg, bool logy=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000);
    SetupPad(c1, logy);
    RatioPlot(c1, h1, h2, title, logy);
    // legend
    c1->cd();
    leg->Clear();
    TLegendEntry* entry1=leg->AddEntry(h1,labels_arr[0].c_str(),"p");
    set_leg_entry_style(entry1);
    TLegendEntry* entry2=leg->AddEntry(h2,labels_arr[1].c_str(),"p");
    set_leg_entry_style(entry2);
    leg->Draw();

    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1;
}

TPad** MakeTwoPads(TCanvas* c) {
    c->Divide(2, 1);
    TPad** pad_arr = new TPad*[2];
    TPad* pad;
    for (int i=0; i<2; i++) {
	c->cd(i+1);
	pad = (TPad*)gPad;
	SetupPad(pad);
	pad_arr[i] = pad;
    }
    return pad_arr;
}

TPad** MakeFourPads(TCanvas* c) {
    // 4-panel plots
    c->Divide(2, 2);//, 0, 0);
    TPad** pad_arr = new TPad*[4];
    TPad* pad;
    for (int i=0; i<4; i++) {
	c->cd(i+1);
	pad = (TPad*)gPad;
	SetupPad(pad);
	pad_arr[i] = pad;
    }
    return pad_arr;
}

TPad** MakeSixPads(TCanvas* c) {
    // 6-panel plots
    c->Divide(3, 2);
    TPad** pad_arr = new TPad*[6];
    TPad* pad;
    for (int i=0; i<6; i++) {
	c->cd(i+1);
	pad = (TPad*)gPad;
	SetupPad(pad);
	pad_arr[i] = pad;
    }
    return pad_arr;
}

TPad** MakeEightPads(TCanvas* c) {
    // 8-panel plots
    c->Divide(4, 2, 0, 0);
    TPad** pad_arr = new TPad*[8];
    TPad* pad;
    for (int i=0; i<8; i++) {
	c->cd(i+1);
	pad = (TPad*)gPad;
	SetupPad(pad);
	pad_arr[i] = pad;
    }
    return pad_arr;
}

TPad** MakeNPads(TCanvas* c, int nx, int ny, double x_border, double y_border) {
    // Flexible pad creation
    c->Divide(nx, ny, x_border, y_border);
    int n_total = nx*ny;
    TPad** pad_arr = new TPad*[n_total];
    TPad* pad;
    for (int i=0; i<n_total; i++) {
	c->cd(i+1);
	pad = (TPad*)gPad;
	SetupPad(pad);
	pad_arr[i] = pad;
    }
    return pad_arr;
}

void PlotTwoHists(int n_hists, TH1** lambda_hists_arr, TH1** nu_hists_arr, std::string* labels_arr, std::string outpdfname, TLegend* leg1, TLegend* leg2, std::string title) {
    TCanvas *c2 = new TCanvas("c2", "c2",0,50,1400,1000);
    SetupCanvas(c2);
    TPad** pad_arr = MakeTwoPads(c2);

    // Histogram style settings
    std::string title_text = "#lambda " + title + ";q_{T} (GeV)";
    HistArrSetStyle(n_hists, lambda_hists_arr, title_text, 7);
    title_text = "A_{cos(2#phi)} " + title + ";q_{T} (GeV)";
    HistArrSetStyle(n_hists, nu_hists_arr, title_text, 7);

    // lambda and nu plots
    DrawHistsOnPad(pad_arr[0], n_hists, nu_hists_arr, labels_arr, leg1);
    DrawHistsOnPad(pad_arr[1], n_hists, lambda_hists_arr, labels_arr, leg2);
    /* pad_arr[1]->GetFramSete()->SetY2(1.1); */
    /* pad_arr[1]->Modified(); */

    // Save plot and clean up
    c2->Modified();
    c2->SaveAs(outpdfname.c_str());
    /* for (int j=0; j<2; j++) delete pad_arr[j]; */
    delete c2;
    /* std::cout << "In PlotTwo: Done!\n"; */
}

void PlotTwoHistsRatio(TH1** hists_arr, std::string* labels_arr, std::string title, std::string outpdfname, int logy=0)  {
    // Make two ratio plots side by side: one comparing RD and LO, the other comparing RD and mixture
    // hists_arr contains RD, LO, and mixture (in that order)
    TCanvas *c2 = new TCanvas("c2","c2",0,50,1400,1000);
    SetupCanvas(c2);
    TPad** pad_arr = MakeNPads(c2, 2, 1, 0.01, 0.01);
    
    // Histogram style settings
    // Suppress single pad title to replace with common title over all pads
    hists_arr[0]->SetTitleSize(0.0);
    /* std::string title_text = title + ";"; */

    /* std::string labels_arr_1[2] = {labels_arr[0], labels_arr[1]}; */
    /* std::string labels_arr_2[2] = {labels_arr[0], labels_arr[2]}; */
    RatioPlot(pad_arr[0], hists_arr[0], hists_arr[1], "", logy, 0.08, 0.02);
    RatioPlot(pad_arr[1], hists_arr[0], hists_arr[2], "", logy, 0.08, 0.02);

    // Legend
    c2->cd();
    /* TLegend* leg = new TLegend(0.70, 0.70, 0.90, 0.85, NULL, "nbNDC"); */
    TLegend* leg = new TLegend(0.20, 0.00, 0.80, 0.04, NULL, "nbNDC");
    SetupLegend(leg, 0.025);
    leg->SetNColumns(4);
    leg->SetTextAlign(22);
    TLegendEntry* entry;
    for (int k=0; k<3; k++) {
	entry=leg->AddEntry(hists_arr[k],labels_arr[k].c_str(),"p");
	set_leg_entry_style(entry);
    }
    leg->Draw();

    // Draw title
    c2->cd();
    TLatex* title_latex = new TLatex(0.5, 0.94, "blaahhh");
    title_latex->SetTextSize(title_latex->GetTextSize()/1.25);
    title_latex->SetTextAlign(21);
    title_latex->DrawLatex(0.5, 0.96, title.c_str());

    // Save plot and clean up
    c2->Modified();
    c2->SaveAs(outpdfname.c_str());
    delete leg;
    delete title_latex;
    delete c2;
}

void PlotThreeHistsRatio(TH1** hists_arr, std::string* labels_arr, std::string title, std::string outpdfname, int logy=0)  {
    // Make two ratio plots side by side: one comparing RD and LO, the other comparing RD and mixture
    // hists_arr contains RD, LO, and mixture (in that order)
    TCanvas *c3 = new TCanvas("c3","c3",0,50,1400,1000);
    SetupCanvas(c3);
    TPad** pad_arr = MakeNPads(c3, 3, 1, 0.01, 0.01);
    
    // Histogram style settings
    // Suppress single pad title to replace with common title over all pads
    hists_arr[0]->SetTitleSize(0.0);
    /* std::string title_text = title + ";"; */

    /* std::string labels_arr_1[2] = {labels_arr[0], labels_arr[1]}; */
    /* std::string labels_arr_2[2] = {labels_arr[0], labels_arr[2]}; */
    RatioPlot(pad_arr[0], hists_arr[0], hists_arr[1], "", logy, 0.08, 0.02);
    RatioPlot(pad_arr[1], hists_arr[0], hists_arr[2], "", logy, 0.08, 0.02);
    RatioPlot(pad_arr[2], hists_arr[0], hists_arr[3], "", logy, 0.08, 0.02);

    // Legend
    c3->cd();
    /* TLegend* leg = new TLegend(0.70, 0.70, 0.90, 0.85, NULL, "nbNDC"); */
    TLegend* leg = new TLegend(0.20, 0.00, 0.80, 0.04, NULL, "nbNDC");
    SetupLegend(leg, 0.025);
    leg->SetNColumns(4);
    leg->SetTextAlign(22);
    TLegendEntry* entry;
    for (int k=0; k<4; k++) {
	entry=leg->AddEntry(hists_arr[k],labels_arr[k].c_str(),"p");
	set_leg_entry_style(entry);
    }
    leg->Draw();

    // Draw title
    c3->cd();
    TLatex* title_latex = new TLatex(0.5, 0.94, "blaahhh");
    title_latex->SetTextSize(title_latex->GetTextSize()/1.25);
    title_latex->SetTextAlign(21);
    title_latex->DrawLatex(0.5, 0.96, title.c_str());

    // Save plot and clean up
    c3->Modified();
    c3->SaveAs(outpdfname.c_str());
    delete leg;
    delete title_latex;
    delete c3;
}

void PlotFour(TH1** hists_arr, std::string outpdfname) {
    // 4-panel plots
    TCanvas *c4 = new TCanvas("c4", "c4",0,50,1400,1000);
    SetupCanvas(c4);
    TPad** pad_arr = MakeFourPads(c4);

    for (int i=0; i<4; i++) {
	set_draw_style(hists_arr[i], 1);
	draw_pad(c4, hists_arr[i], pad_arr[i]);
    }
    c4->Modified();
    c4->SaveAs(outpdfname.c_str());
    /* delete pad_arr; */
    delete c4;
}

void plot_histos() {
    std::string infilename = "singlePhoton_hists.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    gStyle->SetOptStat(10);
    gStyle->SetOptFit(111);
    gROOT->ForceStyle();

    std::string out_pdf_base_name = "singlePhoton_plots";
    std::string pdf_begin_name = out_pdf_base_name + ".pdf(";
    std::string pdf_end_name = out_pdf_base_name + ".pdf)";
    std::string pdf_main_name = out_pdf_base_name + ".pdf";

    // Get hists from file

    // MC Truth distributions
    TH1D* h_truthE = (TH1D*)gDirectory->Get("h_truthE");
    TH1D* h_truthEta = (TH1D*)gDirectory->Get("h_truthEta");
    TH1D* h_truthPhi = (TH1D*)gDirectory->Get("h_truthPhi");
    TH2D* h_truth_eta_phi = (TH2D*)gDirectory->Get("h_truth_eta_phi");
    TH1D* h_vtxr = (TH1D*)gDirectory->Get("h_vtxr");
    TH1D* h_vtxr_zoomed = (TH1D*)gDirectory->Get("h_vtxr_zoomed");
    TH2D* h_vtx_xy = (TH2D*)gDirectory->Get("h_vtx_xy");
    TH2D* h_truthE_vtxr = (TH2D*)gDirectory->Get("h_truthE_vtxr");
    TH1D* h_truthE_converted = (TH1D*)gDirectory->Get("h_truthE_converted");

    // # clusters vs E
    TH2D* h_nClusters_vsE = (TH2D*)gDirectory->Get("h_nClusters_vsE");
    TH1D* h_nClusters = (TH1D*)gDirectory->Get("h_nClusters");
    TProfile* h_nClusters_vsE_prof = (TProfile*)gDirectory->Get("h_nClusters_vsE_prof");

    // Cluster chi^2
    TH1D* h_cluster_nTowers = (TH1D*)gDirectory->Get("h_cluster_nTowers");
    TProfile* h_cluster_nTowers_vsE = (TProfile*)gDirectory->Get("h_cluster_nTowers_vsE");
    TH1D* h_cluster_chi2 = (TH1D*)gDirectory->Get("h_cluster_chi2");
    TH1D* h_cluster_chi2_zoomed = (TH1D*)gDirectory->Get("h_cluster_chi2_zoomed");
    TH2D* h_cluster_chi2_vsE = (TH2D*)gDirectory->Get("h_cluster_chi2_vsE");
    TH1D* h_clusterE = (TH1D*)gDirectory->Get("h_clusterE");
    TH1D* h_cluster_chi2_correct = (TH1D*)gDirectory->Get("h_cluster_chi2_correct");
    TH1D* h_cluster_chi2_incorrect = (TH1D*)gDirectory->Get("h_cluster_chi2_incorrect");


    // Efficiencies and purities
    TH1D* h_nTruth = (TH1D*)gDirectory->Get("h_nTruth");
    TH1D* h_nReco = (TH1D*)gDirectory->Get("h_nReco");
    TH1D* h_nCorrect = (TH1D*)gDirectory->Get("h_nCorrect");
    TH1D* h_nIncorrect = (TH1D*)gDirectory->Get("h_nIncorrect");

    // Resolutions
    TH1D* h_Eratio = (TH1D*)gDirectory->Get("h_Eratio");
    TProfile* h_Eratio_vsE = (TProfile*)gDirectory->Get("h_Eratio_vsE");
    TProfile* h_Eratio_vsEta = (TProfile*)gDirectory->Get("h_Eratio_vsEta");
    TH1D* h_deltaEta = (TH1D*)gDirectory->Get("h_deltaEta");
    TProfile* h_deltaEta_vsE = (TProfile*)gDirectory->Get("h_deltaEta_vsE");
    TProfile* h_deltaEta_vsEta = (TProfile*)gDirectory->Get("h_deltaEta_vsEta");
    TH1D* h_etaRes = (TH1D*)gDirectory->Get("h_etaRes");
    TProfile* h_etaRes_vsE = (TProfile*)gDirectory->Get("h_etaRes_vsE");
    TProfile* h_etaRes_vsEta = (TProfile*)gDirectory->Get("h_etaRes_vsEta");
    TH1D* h_deltaPhi = (TH1D*)gDirectory->Get("h_deltaPhi");
    TProfile* h_deltaPhi_vsE = (TProfile*)gDirectory->Get("h_deltaPhi_vsE");
    TProfile* h_deltaPhi_vsEta = (TProfile*)gDirectory->Get("h_deltaPhi_vsEta");

    // "Missing" photons
    TH1D* h_missing_nClusters = (TH1D*)gDirectory->Get("h_missing_nClusters");
    TH1D* h_missing_truthE = (TH1D*)gDirectory->Get("h_missing_truthE");
    TH1D* h_missing_truthEta = (TH1D*)gDirectory->Get("h_missing_truthEta");
    TH1D* h_missing_truthPhi = (TH1D*)gDirectory->Get("h_missing_truthPhi");
    TH1D* h_missing_Eratio = (TH1D*)gDirectory->Get("h_missing_Eratio");
    TH1D* h_missing_deltaEta = (TH1D*)gDirectory->Get("h_missing_deltaEta");
    TH1D* h_missing_deltaPhi = (TH1D*)gDirectory->Get("h_missing_deltaPhi");
    TH1D* h_missing_vtxr = (TH1D*)gDirectory->Get("h_missing_vtxr");

    // Energy response binned in E
    const int nEbins = 6;
    const int nEtaBins = 12;
    TH1* h_Eratio_vsE_hist_arr[nEbins];
    for (int i=0; i<nEbins; i++) {
	char* namestring = Form("Eratio_vsE_%d", i);
	h_Eratio_vsE_hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
    }
    TH1* h_deltaEta_vsE_hist_arr[nEbins];
    for (int i=0; i<nEbins; i++) {
	char* namestring = Form("deltaEta_vsE_%d", i);
	h_deltaEta_vsE_hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
    }
    TH1* h_deltaPhi_vsE_hist_arr[nEbins];
    for (int i=0; i<nEbins; i++) {
	char* namestring = Form("deltaPhi_vsE_%d", i);
	h_deltaPhi_vsE_hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
    }
    TH1* h_Eratio_vsEta_hist_arr[nEtaBins];
    for (int i=0; i<nEtaBins; i++) {
	char* namestring = Form("Eratio_vsEta_%d", i);
	h_Eratio_vsEta_hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
    }
    TH1* h_deltaEta_vsEta_hist_arr[nEtaBins];
    for (int i=0; i<nEtaBins; i++) {
	char* namestring = Form("deltaEta_vsEta_%d", i);
	h_deltaEta_vsEta_hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
    }
    TH1* h_deltaPhi_vsEta_hist_arr[nEtaBins];
    for (int i=0; i<nEtaBins; i++) {
	char* namestring = Form("deltaPhi_vsEta_%d", i);
	h_deltaPhi_vsEta_hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
    }
    TH1* h_Eratio_vsPhi_hist_arr[nEtaBins];
    for (int i=0; i<nEtaBins; i++) {
	char* namestring = Form("Eratio_vsPhi_%d", i);
	h_Eratio_vsPhi_hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
    }
    TH1* h_deltaEta_vsPhi_hist_arr[nEtaBins];
    for (int i=0; i<nEtaBins; i++) {
	char* namestring = Form("deltaEta_vsPhi_%d", i);
	h_deltaEta_vsPhi_hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
    }
    TH1* h_deltaPhi_vsPhi_hist_arr[nEtaBins];
    for (int i=0; i<nEtaBins; i++) {
	char* namestring = Form("deltaPhi_vsPhi_%d", i);
	h_deltaPhi_vsPhi_hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
    }
    /* BinnedHistSet bhs_Eratio_vsE; */
    /* bhs_Eratio_vsE.GetHistsFromFile("Eratio_vsE"); */
    /* std::cout << "h_Eratio_vsE_hist_arr[0] = " << h_Eratio_vsE_hist_arr[0] << "\n"; */
    /* std::cout << "bhs_Eratio_vsE.hist_arr[0] = " << bhs_Eratio_vsE.hist_arr[0] << "\n"; */


    // Plotting

    // MC Truth distributions
    PlotSingleHist(h_truthE, pdf_begin_name);
    PlotSingleHist(h_truthEta, pdf_main_name);
    PlotSingleHist(h_truthPhi, pdf_main_name);
    PlotSingle2DHist(h_truth_eta_phi, pdf_main_name);
    PlotSingleHist(h_vtxr, pdf_main_name);
    PlotSingleHist(h_vtxr, pdf_main_name, true);
    PlotSingleHist(h_vtxr, pdf_main_name);
    PlotVertexR(h_vtxr, pdf_main_name);
    PlotVertexRZoomed(h_vtxr_zoomed, pdf_main_name);
    PlotSingleHist(h_vtxr_zoomed, pdf_main_name);
    PlotSingleHist(h_vtxr_zoomed, pdf_main_name, true);
    PlotSingle2DHist(h_vtx_xy, pdf_main_name);
    PlotSingle2DHist(h_truthE_vtxr, pdf_main_name);
    PlotSingleHist(h_truthE_converted, pdf_begin_name);

    // Cluster chi^2
    PlotSingleHist(h_cluster_nTowers, pdf_main_name);
    PlotSingleHist(h_cluster_nTowers_vsE, pdf_main_name);
    PlotSingleHist(h_cluster_chi2, pdf_main_name);
    PlotSingleHist(h_cluster_chi2_zoomed, pdf_main_name);
    PlotSingle2DHist(h_cluster_chi2_vsE, pdf_main_name);
    PlotSingleHist(h_clusterE, pdf_main_name);
    PlotSingleHist(h_cluster_chi2_correct, pdf_main_name, false, true);
    PlotSingleHist(h_cluster_chi2_incorrect, pdf_main_name, false, true);
    // Compare correct and incorrect cluster distributions
    int n_hists_2 = 2;
    std::string labels_arr_2[] = {"Correct Clusters", "Incorrect Clusters"};
    TH1* chi2_arr[] = {h_cluster_chi2_correct, h_cluster_chi2_incorrect};
    PlotOne(n_hists_2, chi2_arr, labels_arr_2, "Correct and Incorrect Cluster #chi^{2} Distributions;Cluster #chi^{2}", pdf_main_name, 0);


    // # clusters vs E
    PlotSingle2DHist(h_nClusters_vsE, pdf_main_name);
    /* PlotSingleHist(h_nClusters, pdf_main_name, true, true); */
    PlotnClusters(h_nClusters, pdf_main_name, true, true);
    PlotSingleHist(h_nClusters_vsE_prof, pdf_main_name);

    // Efficiencies and purities
    h_nTruth->Sumw2();
    h_nReco->Sumw2(); h_nCorrect->Sumw2(); h_nIncorrect->Sumw2();
    TH1D* h_Efficiency = (TH1D*)h_nCorrect->Clone("h_Efficiency");
    TH1D* h_Purity = (TH1D*)h_nCorrect->Clone("h_Purity");
    h_Efficiency->Divide(h_nTruth); h_Purity->Divide(h_nReco);
    h_Efficiency->SetTitle("Cluster Reconstruction Efficiency;E_{#gamma} (GeV);Efficiency");
    h_Purity->SetTitle("Cluster Reconstruction Purity;E_{#gamma} (GeV);Purity");

    PlotSingleHist(h_nTruth, pdf_main_name);
    PlotSingleHist(h_nReco, pdf_main_name);
    PlotSingleHist(h_nCorrect, pdf_main_name);
    PlotSingleHist(h_nIncorrect, pdf_main_name);
    PlotSingleHist(h_Efficiency, pdf_main_name);
    PlotSingleHist(h_Purity, pdf_main_name);

    // Resolutions
    PlotSingleHist(h_Eratio, pdf_main_name);
    PlotSingleHist(h_Eratio_vsE, pdf_main_name);
    PlotSingleHist(h_Eratio_vsEta, pdf_main_name);
    PlotSingleHist(h_deltaEta, pdf_main_name);
    PlotSingleHist(h_deltaEta_vsE, pdf_main_name);
    PlotSingleHist(h_deltaEta_vsEta, pdf_main_name);
    PlotSingleHist(h_etaRes, pdf_main_name);
    PlotSingleHist(h_etaRes_vsE, pdf_main_name);
    PlotSingleHist(h_etaRes_vsEta, pdf_main_name);
    PlotSingleHist(h_deltaPhi, pdf_main_name);
    PlotSingleHist(h_deltaPhi_vsE, pdf_main_name);
    PlotSingleHist(h_deltaPhi_vsEta, pdf_main_name);

    // E, eta, phi resolution with fit
    PlotSingleHistWithFit(h_Eratio, pdf_main_name);//, 0.7, 1.4);
    PlotSingleHistWithFit(h_deltaEta, pdf_main_name);//, -0.02, 0.02);
    PlotSingleHistWithFit(h_deltaPhi, pdf_main_name);//, -0.02, 0.02);

    // "Missing" photons
    /* PlotSingleHist(h_missing_nClusters, pdf_main_name, true, true); */
    PlotnClusters2(h_missing_nClusters, pdf_main_name, true, true);
    PlotSingleHist(h_missing_truthE, pdf_main_name);
    PlotSingleHist(h_missing_truthEta, pdf_main_name);
    PlotSingleHist(h_missing_truthPhi, pdf_main_name);
    PlotSingleHist(h_missing_Eratio, pdf_main_name);
    PlotSingleHist(h_missing_deltaEta, pdf_main_name);
    PlotSingleHist(h_missing_deltaPhi, pdf_main_name);
    PlotSingleHist(h_missing_vtxr, pdf_main_name);
    std::string labels_arr_overall_missing[] = {"All Events", "\"Missing\" Photons"};
    TH1* vtxr_arr[] = {h_vtxr, h_missing_vtxr};
    PlotOne(n_hists_2, vtxr_arr, labels_arr_overall_missing, "Overall and \"Missing\" Photons Conversion Vertex Radius;Vertex Radius (cm)", pdf_main_name, 0);
    RescaleHists(2, vtxr_arr, 1.0);
    PlotOne(n_hists_2, vtxr_arr, labels_arr_overall_missing, "Overall and \"Missing\" Photons Conversion Vertex Radius;Vertex Radius (cm)", pdf_main_name, 0);
    PlotOne(n_hists_2, vtxr_arr, labels_arr_overall_missing, "Overall and \"Missing\" Photons Conversion Vertex Radius;Vertex Radius (cm)", pdf_main_name, 0, true);

    /* // Binned in E */
    /* for (int i=0; i<nEbins; i++) { */
	/* PlotSingleHist(h_Eratio_vsE_hist_arr[i], pdf_main_name); */
	/* PlotSingleHistWithFit(h_Eratio_vsE_hist_arr[i], pdf_main_name); */
    /* } */
    /* for (int i=0; i<nEbins; i++) { */
	/* PlotSingleHist(h_deltaEta_vsE_hist_arr[i], pdf_main_name); */
	/* PlotSingleHistWithFit(h_deltaEta_vsE_hist_arr[i], pdf_main_name); */
    /* } */
    /* for (int i=0; i<nEbins; i++) { */
	/* PlotSingleHist(h_deltaPhi_vsE_hist_arr[i], pdf_main_name); */
	/* PlotSingleHistWithFit(h_deltaPhi_vsE_hist_arr[i], pdf_main_name); */
    /* } */
    /* // Binned in eta */
    /* for (int i=0; i<nEtaBins; i++) { */
	/* PlotSingleHist(h_Eratio_vsEta_hist_arr[i], pdf_main_name); */
	/* PlotSingleHistWithFit(h_Eratio_vsEta_hist_arr[i], pdf_main_name); */
    /* } */
    /* for (int i=0; i<nEtaBins; i++) { */
	/* PlotSingleHist(h_deltaEta_vsEta_hist_arr[i], pdf_main_name); */
	/* PlotSingleHistWithFit(h_deltaEta_vsEta_hist_arr[i], pdf_main_name); */
    /* } */
    /* for (int i=0; i<nEtaBins; i++) { */
	/* PlotSingleHist(h_deltaPhi_vsEta_hist_arr[i], pdf_main_name); */
	/* PlotSingleHistWithFit(h_deltaPhi_vsEta_hist_arr[i], pdf_main_name); */
    /* } */
    /* // Binned in phi */
    /* for (int i=0; i<nEtaBins; i++) { */
	/* PlotSingleHist(h_Eratio_vsPhi_hist_arr[i], pdf_main_name); */
	/* PlotSingleHistWithFit(h_Eratio_vsPhi_hist_arr[i], pdf_main_name); */
    /* } */
    /* for (int i=0; i<nEtaBins; i++) { */
	/* PlotSingleHist(h_deltaEta_vsPhi_hist_arr[i], pdf_main_name); */
	/* PlotSingleHistWithFit(h_deltaEta_vsPhi_hist_arr[i], pdf_main_name); */
    /* } */
    /* for (int i=0; i<nEtaBins; i++) { */
	/* PlotSingleHist(h_deltaPhi_vsPhi_hist_arr[i], pdf_main_name); */
	/* PlotSingleHistWithFit(h_deltaPhi_vsPhi_hist_arr[i], pdf_main_name); */
    /* } */
    /* TGraphErrors* gr = (TGraphErrors*)gDirectory->Get("gr_Eratio_vsE"); */
    /* TCanvas *c1 = new TCanvas("c1", "c1",0,50,1400,1000); */
    /* c1->cd(); */
    /* gr->Draw("AP"); */
    /* c1->SaveAs(pdf_main_name.c_str()); */
    


    PlotSingleHist(h_cluster_chi2, pdf_end_name);
    infile->Close();
    delete infile;
}


// plot_filtered_ks_syst.C
// K0S yield ratio (smeared / raw) vs pT — smearing systematic.
// Pre-filtered MC: no additional cuts applied.
// Run with:  root -l -b -q plot_filtered_ks_syst.C

#include <ctime>
#include <sstream>
#include <algorithm>
std::string getDate2(){
    std::time_t t=std::time(0); std::tm* now=std::localtime(&t);
    std::stringstream d;
    d<<(now->tm_mon+1)<<'/'<<now->tm_mday<<'/'<<(now->tm_year+1900);
    return d.str();
}
auto addLabel=[](double x1,double y1,double x2,double y2){
    TPaveText *pt=new TPaveText(x1,y1,x2,y2,"NDC");
    pt->SetFillStyle(0); pt->SetBorderSize(0); pt->SetTextFont(42);
    pt->AddText("#it{#bf{sPHENIX}} Internal,  #it{p}+#it{p}  #sqrt{s} = 200 GeV");
    pt->Draw();
};
auto addDate=[](double x1,double y1,double x2,double y2){
    TPaveText *pt=new TPaveText(x1,y1,x2,y2,"NDC");
    pt->SetFillStyle(0); pt->SetBorderSize(0); pt->SetTextFont(42);
    pt->AddText(getDate2().c_str()); pt->Draw();
};

void plot_filtered_ks_syst() {
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int N = 4;
    double avg_pt[]    = {0.8934, 1.1534, 1.3567, 1.6887};
    double ratio[]     = {0.999687, 1.002620, 0.999262, 1.001783};
    double ratio_err[] = {0.005900, 0.010579, 0.012146, 0.013720};
    double xerr[]      = {0.160, 0.090, 0.120, 0.255};
    double nr[]        = {57421, 17940, 13541, 10654};
    double ns[]        = {57403, 17987, 13531, 10673};
    double syst_pct[]  = {0.0313, 0.2620, 0.0738, 0.1783};

    // ── canvas 1: yield ratio ────────────────────────────────────────────────
    TCanvas *c1 = new TCanvas("c1","K0S yield ratio",900,650);
    c1->SetLeftMargin(0.13); c1->SetBottomMargin(0.13); c1->SetRightMargin(0.06);

    TGraphErrors *gR = new TGraphErrors(N,avg_pt,ratio,xerr,ratio_err);
    gR->SetMarkerStyle(20); gR->SetMarkerSize(1.4);
    gR->SetMarkerColor(kRed+1); gR->SetLineColor(kRed+1); gR->SetLineWidth(2);
    gR->Draw("APE");
    gR->GetXaxis()->SetTitle("K_{S}^{0} #it{p}_{T} (GeV/#it{c})");
    gR->GetYaxis()->SetTitle("N_{smeared} / N_{raw}");
    gR->GetYaxis()->SetRangeUser(0.7,1.3);
    gR->GetXaxis()->SetTitleSize(0.05); gR->GetYaxis()->SetTitleSize(0.05);

    TLine *unity = new TLine(0.75,1.0,2.0,1.0);
    unity->SetLineStyle(2); unity->SetLineColor(kGray+1); unity->SetLineWidth(2);
    unity->Draw();
    TBox *band = new TBox(0.75,0.95,2.0,1.05);
    band->SetFillColorAlpha(kGray,0.30); band->SetLineWidth(0); band->Draw();
    gR->Draw("PE SAME");

    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);
    TLegend *leg1 = new TLegend(0.14,0.74,0.70,0.87);
    leg1->SetBorderSize(0); leg1->SetFillStyle(0); leg1->SetTextSize(0.030);
    leg1->AddEntry(gR,   "N_{smeared}/N_{raw}  (stat. err)","lpe");
    leg1->AddEntry(unity,"Unity","l");
    leg1->AddEntry(band, "#pm5% band","f");
    leg1->Draw();

    c1->SaveAs("plot_filtered_ks_syst_ratio.pdf");
    c1->SaveAs("plot_filtered_ks_syst_ratio.png");
    std::cout << "Saved plot_filtered_ks_syst_ratio\n";

    // ── canvas 2: raw vs smeared counts ─────────────────────────────────────
    TCanvas *c2 = new TCanvas("c2","K0S counts",900,650);
    c2->SetLeftMargin(0.13); c2->SetBottomMargin(0.13); c2->SetRightMargin(0.06);

    TGraph *gRaw = new TGraph(N,avg_pt,nr);
    TGraph *gSmr = new TGraph(N,avg_pt,ns);
    gRaw->SetMarkerStyle(20); gRaw->SetMarkerSize(1.4);
    gRaw->SetMarkerColor(kBlue+1); gRaw->SetLineColor(kBlue+1); gRaw->SetLineWidth(2);
    gSmr->SetMarkerStyle(22); gSmr->SetMarkerSize(1.4);
    gSmr->SetMarkerColor(kRed+1);  gSmr->SetLineColor(kRed+1);  gSmr->SetLineWidth(2);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(gRaw,"PL"); mg->Add(gSmr,"PL"); mg->Draw("A");
    mg->GetXaxis()->SetTitle("K_{S}^{0} #it{p}_{T} (GeV/#it{c})");
    mg->GetYaxis()->SetTitle("Events (filtered)");
    mg->GetXaxis()->SetTitleSize(0.05); mg->GetYaxis()->SetTitleSize(0.05);

    TLegend *leg2 = new TLegend(0.50,0.72,0.92,0.88);
    leg2->SetBorderSize(0); leg2->SetFillStyle(0); leg2->SetTextSize(0.032);
    leg2->AddEntry(gRaw,"Filtered raw",    "lp");
    leg2->AddEntry(gSmr,"Filtered smeared","lp");
    leg2->Draw();
    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);

    c2->SaveAs("plot_filtered_ks_syst_counts.pdf");
    c2->SaveAs("plot_filtered_ks_syst_counts.png");
    std::cout << "Saved plot_filtered_ks_syst_counts\n";

    // ── canvas 3: systematic % ───────────────────────────────────────────────
    TCanvas *c3 = new TCanvas("c3","K0S syst%",900,650);
    c3->SetLeftMargin(0.13); c3->SetBottomMargin(0.13); c3->SetRightMargin(0.06);

    TGraph *gS = new TGraph(N,avg_pt,syst_pct);
    gS->SetMarkerStyle(20); gS->SetMarkerSize(1.4);
    gS->SetMarkerColor(kMagenta+1); gS->SetLineColor(kMagenta+1); gS->SetLineWidth(2);
    gS->Draw("APL");
    gS->GetXaxis()->SetTitle("K_{S}^{0} #it{p}_{T} (GeV/#it{c})");
    gS->GetYaxis()->SetTitle("Smearing systematic |#DeltaN/N| (%)");
    double ymax = *std::max_element(syst_pct,syst_pct+N)*1.35 + 0.5;
    gS->GetYaxis()->SetRangeUser(0,ymax);
    gS->GetXaxis()->SetTitleSize(0.05); gS->GetYaxis()->SetTitleSize(0.05);
    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);

    c3->SaveAs("plot_filtered_ks_syst_pct.pdf");
    c3->SaveAs("plot_filtered_ks_syst_pct.png");
    std::cout << "Saved plot_filtered_ks_syst_pct\n";
}

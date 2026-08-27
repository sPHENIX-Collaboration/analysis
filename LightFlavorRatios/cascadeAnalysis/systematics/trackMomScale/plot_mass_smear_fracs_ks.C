// plot_mass_smear_fracs_ks.C
// Visualises mass-width-matched K0S smearing fractions.
// Run with:  root -l -b -q plot_mass_smear_fracs_ks.C

#include <ctime>
#include <sstream>
#include <algorithm>
#include <cmath>

std::string _getDate(){
    std::time_t t=std::time(0); std::tm* n=std::localtime(&t);
    std::stringstream s;
    s<<(n->tm_mon+1)<<'/'<<n->tm_mday<<'/'<<(n->tm_year+1900);
    return s.str();
}
void _label(double x1,double y1,double x2,double y2){
    TPaveText *p=new TPaveText(x1,y1,x2,y2,"NDC");
    p->SetFillStyle(0); p->SetBorderSize(0); p->SetTextFont(42);
    p->AddText("#it{#bf{sPHENIX}} Internal,  #it{p}+#it{p}  #sqrt{s} = 200 GeV");
    p->Draw();
}
void _date(double x1,double y1){
    TLatex l; l.SetNDC(); l.SetTextFont(42); l.SetTextSize(0.035);
    l.SetTextColor(kGray+2); l.DrawLatex(x1,y1,_getDate().c_str());
}

void plot_mass_smear_fracs_ks() {
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int N = 7;
    double avg_pt[]    = {0.6544, 0.9343, 1.2300, 1.5638, 1.9643, 2.4712, 3.3271};
    double sig_data[]  = {5.8413, 7.0841, 8.2642, 9.3537, 10.6817, 12.0176, 13.1953};
    double sig_derr[]  = {0.0404, 0.0548, 0.0828, 0.1267, 0.1594, 0.1806, 0.4918};
    double sig_sim0[]  = {2.7631, 2.6199, 2.6236, 2.7225, 2.7388, 2.8742, 3.1924};
    double sig_check[] = {5.8310, 7.1219, 8.2659, 9.5721, 11.1457, 12.4020, 13.6752};
    double frac_pct[]  = {2.9298, 3.5090, 3.7633, 3.9927, 4.3454, 5.1751, 4.5065};
    double xerr[]      = {0.150, 0.150, 0.150, 0.200, 0.200, 0.400, 0.500};
    double zero[]      = {0, 0, 0, 0, 0, 0, 0};

    // ── canvas 1: sigma comparison ─────────────────────────────────────────────
    TCanvas *c1 = new TCanvas("c1","K0S mass sigma comparison",900,650);
    c1->SetLeftMargin(0.13); c1->SetBottomMargin(0.13); c1->SetRightMargin(0.06);

    TGraphErrors *gD = new TGraphErrors(N, avg_pt, sig_data, xerr, sig_derr);
    TGraph       *gS = new TGraph(N, avg_pt, sig_sim0);
    TGraph       *gC = new TGraph(N, avg_pt, sig_check);

    gD->SetMarkerStyle(20); gD->SetMarkerSize(1.3);
    gD->SetMarkerColor(kBlack);  gD->SetLineColor(kBlack);  gD->SetLineWidth(2);
    gS->SetMarkerStyle(24); gS->SetMarkerSize(1.3);
    gS->SetMarkerColor(kAzure+7); gS->SetLineColor(kAzure+7); gS->SetLineWidth(2);
    gC->SetMarkerStyle(21); gC->SetMarkerSize(1.3);
    gC->SetMarkerColor(kRed+1);  gC->SetLineColor(kRed+1);  gC->SetLineWidth(2);

    double ymax = 0;
    for(int i=0;i<N;++i) ymax = std::max(ymax, std::max({sig_data[i]+sig_derr[i], sig_sim0[i]}));
    ymax *= 1.35;

    TMultiGraph *mg1 = new TMultiGraph();
    mg1->Add(gS,"PL"); mg1->Add(gD,"PE"); mg1->Add(gC,"PL");
    mg1->Draw("A");
    mg1->GetXaxis()->SetTitle("#it{p}_{T}^{K_{S}^{0}} (GeV/#it{c})");
    mg1->GetYaxis()->SetTitle("Gaussian #sigma of K_{S}^{0} mass (MeV/#it{c}^{2})");
    mg1->GetYaxis()->SetRangeUser(0, ymax);
    mg1->GetXaxis()->SetTitleSize(0.05); mg1->GetYaxis()->SetTitleSize(0.047);
    mg1->GetYaxis()->SetTitleOffset(1.25);

    TLegend *leg1 = new TLegend(0.14,0.72,0.65,0.87);
    leg1->SetBorderSize(0); leg1->SetFillStyle(0); leg1->SetTextSize(0.033);
    leg1->AddEntry(gD,"Data",                       "lpe");
    leg1->AddEntry(gS,"SV sim (unsmeared)",          "lp");
    leg1->AddEntry(gC,"SV sim (mass-width smeared)","lp");
    leg1->Draw();
    _label(0.14,0.88,0.68,0.96);
    _date(0.72,0.90);

    c1->SaveAs("plot_mass_smear_sigma_comparison.pdf");
    c1->SaveAs("plot_mass_smear_sigma_comparison.png");
    std::cout << "Saved plot_mass_smear_sigma_comparison.pdf/.png\n";

    // ── canvas 2: optimal fracs vs pT ──────────────────────────────────────────
    TCanvas *c2 = new TCanvas("c2","Mass-width smearing fractions",900,650);
    c2->SetLeftMargin(0.13); c2->SetBottomMargin(0.20); c2->SetRightMargin(0.06);

    TGraphErrors *gF = new TGraphErrors(N, avg_pt, frac_pct, xerr, zero);
    gF->SetMarkerStyle(20); gF->SetMarkerSize(1.4);
    gF->SetMarkerColor(kViolet+1); gF->SetLineColor(kViolet+1); gF->SetLineWidth(2);
    gF->Draw("APE");
    gF->GetXaxis()->SetTitle("#it{p}_{T}^{K_{S}^{0}} (GeV/#it{c})");
    gF->GetYaxis()->SetTitle("Smearing fraction (%)");
    double fmax = *std::max_element(frac_pct,frac_pct+N)*1.4 + 0.2;
    gF->GetYaxis()->SetRangeUser(0, fmax);
    gF->GetXaxis()->SetTitleSize(0.03); gF->GetYaxis()->SetTitleSize(0.05);
    gF->GetYaxis()->SetTitleOffset(1.2);

    _label(0.14,0.88,0.68,0.96);
    _date(0.72,0.90);

    TLegend *leg2 = new TLegend(0.14,0.74,0.60,0.85);
    leg2->SetBorderSize(0); leg2->SetFillStyle(0); leg2->SetTextSize(0.033);
    leg2->AddEntry(gF,"Frac needed to match K_{S}^{0} mass #sigma","lp");
    leg2->Draw();

    c2->Update();
    c2->SaveAs("plot_mass_smear_fracs_ks.pdf");
    c2->SaveAs("plot_mass_smear_fracs_ks.png");
    std::cout << "Saved plot_mass_smear_fracs_ks.pdf/.png\n";
}

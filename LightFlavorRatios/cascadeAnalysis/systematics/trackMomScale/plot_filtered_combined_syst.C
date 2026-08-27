// plot_filtered_combined_syst.C
// Smearing systematic (|ΔN/N|%) for K0S and Xi on one canvas.
// Run with:  root -l -b -q plot_filtered_combined_syst.C

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

void plot_filtered_combined_syst() {
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int Nk = 4, Nx = 4;
    double ks_pt[]   = {0.8934, 1.1534, 1.3567, 1.6887};
    double ks_syst[] = {0.0313, 0.2620, 0.0738, 0.1783};
    double xi_pt[]  = {0.9354, 1.1632, 1.3681, 1.7145};
    double xi_syst[]= {1.9011, 3.7406, 1.5660, 0.1332};

    TCanvas *c = new TCanvas("c","Smearing systematic",900,650);
    c->SetLeftMargin(0.13); c->SetBottomMargin(0.13); c->SetRightMargin(0.06);

    TGraph *gK = new TGraph(Nk, ks_pt,  ks_syst);
    TGraph *gX = new TGraph(Nx, xi_pt, xi_syst);

    gK->SetMarkerStyle(20); gK->SetMarkerSize(1.4);
    gK->SetMarkerColor(kBlue+1);  gK->SetLineColor(kBlue+1);  gK->SetLineWidth(2);
    gX->SetMarkerStyle(21); gX->SetMarkerSize(1.4);
    gX->SetMarkerColor(kRed+1);   gX->SetLineColor(kRed+1);   gX->SetLineWidth(2);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(gK,"PL"); mg->Add(gX,"PL"); mg->Draw("A");
    mg->GetXaxis()->SetTitle("#it{p}_{T} (GeV/#it{c})");
    mg->GetYaxis()->SetTitle("Smearing systematic |#DeltaN/N| (%)");
    mg->GetXaxis()->SetTitleSize(0.05); mg->GetYaxis()->SetTitleSize(0.05);

    TLegend *leg = new TLegend(0.14,0.74,0.55,0.87);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.032);
    leg->AddEntry(gK,"K_{S}^{0} smearing syst.","lp");
    leg->AddEntry(gL,"#Xi^{-} smearing syst.","lp");
    leg->Draw();

    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);

    c->SaveAs("plot_filtered_combined_syst.pdf");
    c->SaveAs("plot_filtered_combined_syst.png");
    std::cout << "Saved plot_filtered_combined_syst\n";
}

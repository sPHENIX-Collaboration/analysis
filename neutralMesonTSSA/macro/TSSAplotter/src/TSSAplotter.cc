#include "TSSAplotter.h"
#include <TFile.h>
#include <TDirectoryFile.h>
#include <TF1.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TLine.h>

#include <string>
#include <utility>
#include <algorithm>
#include <vector>

void normalize_hist(TH1* hist, double norm=1.0) {
    hist->Sumw2();
    hist->Scale(norm/hist->Integral());
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

void set_draw_style(TH1* hist, int color, int marker_style=7) {
    hist->SetStats(10);
    hist->SetMarkerStyle(marker_style);
    hist->SetMarkerSize(1.5);
    hist->SetMarkerColor(color);
    hist->SetLineColor(color);
    hist->GetXaxis()->CenterTitle(true);
    hist->GetXaxis()->SetLabelFont(42);
    hist->GetXaxis()->SetLabelSize(0.035);
    hist->GetXaxis()->SetTitleSize(0.05);
    hist->GetXaxis()->SetTitleOffset(0.8);
    hist->GetXaxis()->SetTitleFont(42);
    hist->GetYaxis()->CenterTitle(true);
    hist->GetYaxis()->SetLabelFont(42);
    hist->GetYaxis()->SetLabelSize(0.035);
    hist->GetYaxis()->SetTitleSize(0.05);
    hist->GetYaxis()->SetTitleFont(42);
}

void PlotSingleHist(TH1* hist, std::string outpdfname, bool logy=false, bool normalized=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1600,900);
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

void PlotSingle2DHist(TH2* hist, std::string outpdfname, bool logz=false) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1600,900);
    SetupPad(c1, false);
    set_draw_style(hist, 1, 20);
    hist->SetStats(0);
    hist->Draw("colz");
    c1->SetLogz(logz);
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1;
}

TSSAplotter::TSSAplotter() {}

TSSAplotter::~TSSAplotter() {}

void TSSAplotter::GetNMHists() {
    TFile* infile = new TFile(NMhistfname.c_str(), "READ");
    infile->cd();
    
    h_nEvents = (TH1*)infile->Get("h_nEvents");
    h_vtxz = (TH1*)infile->Get("h_vtxz");
    h_towerEta_Phi_500MeV = (TH2*)infile->Get("h_towerEta_Phi_500MeV");
    h_towerEta_Phi_800MeV = (TH2*)infile->Get("h_towerEta_Phi_800MeV");
    // cluster distributions
    h_nAllClusters = (TH1*)infile->Get("h_nAllClusters");
    h_nGoodClusters = (TH1*)infile->Get("h_nGoodClusters");
    h_clusterE = (TH1*)infile->Get("h_clusterE");
    h_clusterEta = (TH1*)infile->Get("h_clusterEta");
    h_clusterVtxz_Eta = (TH2*)infile->Get("h_clusterVtxz_Eta");
    h_clusterPhi = (TH1*)infile->Get("h_clusterPhi");
    h_clusterEta_Phi = (TH2*)infile->Get("h_clusterEta_Phi");
    h_clusterpT = (TH1*)infile->Get("h_clusterpT");
    h_clusterxF = (TH1*)infile->Get("h_clusterxF");
    h_clusterpT_xF = (TH2*)infile->Get("h_clusterpT_xF");
    h_clusterChi2 = (TH1*)infile->Get("h_clusterChi2");
    h_clusterChi2zoomed = (TH1*)infile->Get("h_clusterChi2zoomed");
    h_mesonClusterChi2 = (TH1*)infile->Get("h_mesonClusterChi2");
    h_goodClusterE = (TH1*)infile->Get("h_goodClusterE");
    h_goodClusterEta = (TH1*)infile->Get("h_goodClusterEta");
    h_goodClusterVtxz_Eta = (TH2*)infile->Get("h_goodClusterVtxz_Eta");
    h_goodClusterPhi = (TH1*)infile->Get("h_goodClusterPhi");
    h_goodClusterEta_Phi = (TH2*)infile->Get("h_goodClusterEta_Phi");
    h_goodClusterpT = (TH1*)infile->Get("h_goodClusterpT");
    h_goodClusterxF = (TH1*)infile->Get("h_goodClusterxF");
    h_goodClusterpT_xF = (TH2*)infile->Get("h_goodClusterpT_xF");
    // diphoton distributions
    h_nDiphotons = (TH1*)infile->Get("h_nDiphotons");
    h_nRecoPi0s = (TH1*)infile->Get("h_nRecoPi0s");
    h_nRecoEtas = (TH1*)infile->Get("h_nRecoEtas");
    h_diphotonE = (TH1*)infile->Get("h_diphotonE");
    h_diphotonMass = (TH1*)infile->Get("h_diphotonMass");
    h_diphotonEta = (TH1*)infile->Get("h_diphotonEta");
    h_diphotonVtxz_Eta = (TH2*)infile->Get("h_diphotonVtxz_Eta");
    h_diphotonPhi = (TH1*)infile->Get("h_diphotonPhi");
    h_diphotonEta_Phi = (TH2*)infile->Get("h_diphotonEta_Phi");
    h_diphotonpT = (TH1*)infile->Get("h_diphotonpT");
    h_diphotonxF = (TH1*)infile->Get("h_diphotonxF");
    h_diphotonpT_xF = (TH2*)infile->Get("h_diphotonpT_xF");
    h_diphotonAsym = (TH1*)infile->Get("h_diphotonAsym");
    h_diphotonDeltaR = (TH1*)infile->Get("h_diphotonDeltaR");
    h_diphotonAsym_DeltaR = (TH2*)infile->Get("h_diphotonAsym_DeltaR");

    // BHS mass
    double pT_upper = 20.0;
    double xF_upper = 0.15;
    std::vector<double> pTbinsMass;
    std::vector<double> xFbinsMass;
    int nbins_bhs = 20;
    for (int i=0; i<nbins_bhs; i++) {
	pTbinsMass.push_back(i*(pT_upper/nbins_bhs));
	xFbinsMass.push_back(2*xF_upper*i/nbins_bhs - xF_upper);
    }
    pTbinsMass.push_back(pT_upper);
    xFbinsMass.push_back(xF_upper);
    bhs_diphotonMass_pT = new BinnedHistSet(h_diphotonMass, "p_{T} (GeV)", pTbinsMass);
    bhs_diphotonMass_pT->GetHistsFromFile("h_diphotonMass_pT");
    bhs_diphotonMass_xF = new BinnedHistSet(h_diphotonMass, "x_{F}", xFbinsMass);
    bhs_diphotonMass_xF->GetHistsFromFile("h_diphotonMass_xF");
}

void TSSAplotter::GetTSSAHists() {
    TFile* infile = new TFile(TSSAhistfname.c_str(), "READ");
    infile->cd();

    h_nClustersMinbiasTrig = (TH1*)infile->Get("h_nClustersMinbiasTrig");
    h_nClustersPhotonTrig = (TH1*)infile->Get("h_nClustersPhotonTrig");
    h_Nevents = (TH1*)infile->Get("h_Nevents");
    h_Npi0s = (TH1*)infile->Get("h_Npi0s");
    h_Netas = (TH1*)infile->Get("h_Netas");

    // BHS mass
    /* h_diphotonMass = (TH1*)infile->Get("h_diphotonMass"); */
    /* bhs_diphotonMass_pT = new BinnedHistSet(h_diphotonMass, "p_{T} (GeV)", pTbinsMass); */
    /* bhs_diphotonMass_pT = new BinnedHistSet(h_diphotonMass, "p_{T} (GeV)", pTbins); */
    /* bhs_diphotonMass_pT->GetHistsFromFile("h_diphotonMass_pT"); */

    bhs_diphotonMass_pT_pi0binning = new BinnedHistSet(h_diphotonMass, "p_{T} (GeV)", pTbins_pi0);
    bhs_diphotonMass_pT_etabinning = new BinnedHistSet(h_diphotonMass, "p_{T} (GeV)", pTbins_eta);
    bhs_pi0_pT_pT = new BinnedHistSet(h_diphotonpT, "p_{T} (GeV)", pTbins_pi0);
    bhs_eta_pT_pT = new BinnedHistSet(h_diphotonpT, "p_{T} (GeV)", pTbins_eta);
    bhs_diphotonxF_xF = new BinnedHistSet(h_diphotonxF, "x_{F}", xFbins);
    bhs_diphotoneta_eta = new BinnedHistSet(h_diphotonEta, "#eta", etabins);
    bhs_diphotonvtxz_vtxz = new BinnedHistSet(h_vtxz, "z_{vtx} (cm)", vtxzbins);
    bhs_diphotonMass_pT_pi0binning->GetHistsFromFile("h_diphotonMass_pT_pi0binning");
    bhs_diphotonMass_pT_etabinning->GetHistsFromFile("h_diphotonMass_pT_etabinning");
    bhs_pi0_pT_pT->GetHistsFromFile("h_pi0_pT_pT");
    bhs_eta_pT_pT->GetHistsFromFile("h_eta_pT_pT");
    bhs_diphotonxF_xF->GetHistsFromFile("h_diphotonxF_xF");
    bhs_diphotoneta_eta->GetHistsFromFile("h_diphotoneta_eta");
    bhs_diphotonvtxz_vtxz->GetHistsFromFile("h_diphotonvtxz_vtxz");

    h_DiphotonMassAsym = (TH2*)infile->Get("h_DiphotonMassAsym");
    h_DiphotonMassdeltaR = (TH2*)infile->Get("h_DiphotonMassdeltaR");
    h_DiphotondeltaRAsym = (TH2*)infile->Get("h_DiphotondeltaRAsym");
    h_DiphotondeltaRAsym_pi0 = (TH2*)infile->Get("h_DiphotondeltaRAsym_pi0");
    h_DiphotondeltaRAsym_pi0_smalldR = (TH2*)infile->Get("h_DiphotondeltaRAsym_pi0_smalldR");
    h_DiphotondeltaRAsym_eta = (TH2*)infile->Get("h_DiphotondeltaRAsym_eta");
    h_DiphotondeltaRAsym_etabkgr = (TH2*)infile->Get("h_DiphotondeltaRAsym_etabkgr");
    h_DiphotonMassAsym_highpT = (TH2*)infile->Get("h_DiphotonMassAsym_highpT");
    h_DiphotonMassdeltaR_highpT = (TH2*)infile->Get("h_DiphotonMassdeltaR_highpT");
    h_DiphotondeltaRAsym_highpT = (TH2*)infile->Get("h_DiphotondeltaRAsym_highpT");
    h_DiphotondeltaRAsym_highpT_smalldR = (TH2*)infile->Get("h_DiphotondeltaRAsym_highpT_smalldR");
    h_DiphotondeltaRAsym_eta_highpT = (TH2*)infile->Get("h_DiphotondeltaRAsym_eta_highpT");
    h_DiphotondeltaRAsym_etabkgr_highpT = (TH2*)infile->Get("h_DiphotondeltaRAsym_etabkgr_highpT");
    // Armenteros-Podolanski plots
    h_armen_all = (TH2*)infile->Get("h_armen_all");
    h_armen_pi0 = (TH2*)infile->Get("h_armen_pi0");
    h_armen_pi0bkgr = (TH2*)infile->Get("h_armen_pi0bkgr");
    h_armen_eta = (TH2*)infile->Get("h_armen_eta");
    h_armen_etabkgr = (TH2*)infile->Get("h_armen_etabkgr");
    h_armen_eta_highpT = (TH2*)infile->Get("h_armen_eta_highpT");
    h_armen_etabkgr_highpT = (TH2*)infile->Get("h_armen_etabkgr_highpT");
    h_armen_p_L = (TH1*)infile->Get("h_armen_p_L");
    h_armen_p_T = (TH1*)infile->Get("h_armen_p_T");
    h_armen_alpha_alphaE = (TH2*)infile->Get("h_armen_alpha_alphaE");
    h_armen_alpha_deltaR = (TH2*)infile->Get("h_armen_alpha_deltaR");
    h_armen_alpha_deltaR_pi0 = (TH2*)infile->Get("h_armen_alpha_deltaR_pi0");
    h_armen_alpha_deltaR_eta = (TH2*)infile->Get("h_armen_alpha_deltaR_eta");
    h_armen_p_T_deltaR = (TH2*)infile->Get("h_armen_p_T_deltaR");
    h_armen_pT_p_L = (TH2*)infile->Get("h_armen_pT_p_L");
    h_armen_pT_p_T = (TH2*)infile->Get("h_armen_pT_p_T");

    // Phi hists
    std::string nameprefix = "h_pi0_";
    std::string titlewhich = "#pi^{0}";
    bhs_pi0_blue_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_blue_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_yellow_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_yellow_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_blue_up_phi_pT->GetHistsFromFile("h_pi0_phi_pT_blue_up");
    bhs_pi0_blue_down_phi_pT->GetHistsFromFile("h_pi0_phi_pT_blue_down");
    bhs_pi0_yellow_up_phi_pT->GetHistsFromFile("h_pi0_phi_pT_yellow_up");
    bhs_pi0_yellow_down_phi_pT->GetHistsFromFile("h_pi0_phi_pT_yellow_down");
    bhs_pi0_blue_up_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_blue_down_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_blue_up_forward_phi_pT->GetHistsFromFile("h_pi0_phi_pT_blue_up_forward");
    bhs_pi0_blue_down_forward_phi_pT->GetHistsFromFile("h_pi0_phi_pT_blue_down_forward");
    bhs_pi0_yellow_up_forward_phi_pT->GetHistsFromFile("h_pi0_phi_pT_yellow_up_forward");
    bhs_pi0_yellow_down_forward_phi_pT->GetHistsFromFile("h_pi0_phi_pT_yellow_down_forward");
    bhs_pi0_blue_up_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_blue_down_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_yellow_up_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_yellow_down_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_blue_up_backward_phi_pT->GetHistsFromFile("h_pi0_phi_pT_blue_up_backward");
    bhs_pi0_blue_down_backward_phi_pT->GetHistsFromFile("h_pi0_phi_pT_blue_down_backward");
    bhs_pi0_yellow_up_backward_phi_pT->GetHistsFromFile("h_pi0_phi_pT_yellow_up_backward");
    bhs_pi0_yellow_down_backward_phi_pT->GetHistsFromFile("h_pi0_phi_pT_yellow_down_backward");
    bhs_pi0_lowEta_blue_up_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_lowEta_blue_down_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_lowEta_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_lowEta_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_lowEta_blue_up_forward_phi_pT->GetHistsFromFile("h_pi0_lowEta_phi_pT_blue_up_forward");
    bhs_pi0_lowEta_blue_down_forward_phi_pT->GetHistsFromFile("h_pi0_lowEta_phi_pT_blue_down_forward");
    bhs_pi0_lowEta_yellow_up_forward_phi_pT->GetHistsFromFile("h_pi0_lowEta_phi_pT_yellow_up_forward");
    bhs_pi0_lowEta_yellow_down_forward_phi_pT->GetHistsFromFile("h_pi0_lowEta_phi_pT_yellow_down_forward");
    bhs_pi0_highEta_blue_up_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_highEta_blue_down_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_highEta_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_highEta_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0_highEta_blue_up_forward_phi_pT->GetHistsFromFile("h_pi0_highEta_phi_pT_blue_up_forward");
    bhs_pi0_highEta_blue_down_forward_phi_pT->GetHistsFromFile("h_pi0_highEta_phi_pT_blue_down_forward");
    bhs_pi0_highEta_yellow_up_forward_phi_pT->GetHistsFromFile("h_pi0_highEta_phi_pT_yellow_up_forward");
    bhs_pi0_highEta_yellow_down_forward_phi_pT->GetHistsFromFile("h_pi0_highEta_phi_pT_yellow_down_forward");

    bhs_pi0_blue_up_phi_xF = new BinnedHistSet(Form("%sphi_xF_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_pi0_blue_down_phi_xF = new BinnedHistSet(Form("%sphi_xF_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_pi0_yellow_up_phi_xF = new BinnedHistSet(Form("%sphi_xF_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_pi0_yellow_down_phi_xF = new BinnedHistSet(Form("%sphi_xF_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_pi0_blue_up_phi_xF->GetHistsFromFile("h_pi0_phi_xF_blue_up");
    bhs_pi0_blue_down_phi_xF->GetHistsFromFile("h_pi0_phi_xF_blue_down");
    bhs_pi0_yellow_up_phi_xF->GetHistsFromFile("h_pi0_phi_xF_yellow_up");
    bhs_pi0_yellow_down_phi_xF->GetHistsFromFile("h_pi0_phi_xF_yellow_down");

    bhs_pi0_blue_up_phi_eta = new BinnedHistSet(Form("%sphi_eta_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_pi0_blue_down_phi_eta = new BinnedHistSet(Form("%sphi_eta_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_pi0_yellow_up_phi_eta = new BinnedHistSet(Form("%sphi_eta_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_pi0_yellow_down_phi_eta = new BinnedHistSet(Form("%sphi_eta_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_pi0_blue_up_phi_eta->GetHistsFromFile("h_pi0_phi_eta_blue_up");
    bhs_pi0_blue_down_phi_eta->GetHistsFromFile("h_pi0_phi_eta_blue_down");
    bhs_pi0_yellow_up_phi_eta->GetHistsFromFile("h_pi0_phi_eta_yellow_up");
    bhs_pi0_yellow_down_phi_eta->GetHistsFromFile("h_pi0_phi_eta_yellow_down");

    bhs_pi0_blue_up_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_pi0_blue_down_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_pi0_yellow_up_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_pi0_yellow_down_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", vtxzbins);
    bhs_pi0_blue_up_phi_vtxz->GetHistsFromFile("h_pi0_phi_vtxz_blue_up");
    bhs_pi0_blue_down_phi_vtxz->GetHistsFromFile("h_pi0_phi_vtxz_blue_down");
    bhs_pi0_yellow_up_phi_vtxz->GetHistsFromFile("h_pi0_phi_vtxz_yellow_up");
    bhs_pi0_yellow_down_phi_vtxz->GetHistsFromFile("h_pi0_phi_vtxz_yellow_down");

    nameprefix = "h_eta_";
    titlewhich = "#eta";
    bhs_eta_blue_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_blue_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_yellow_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_yellow_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_blue_up_phi_pT->GetHistsFromFile("h_eta_phi_pT_blue_up");
    bhs_eta_blue_down_phi_pT->GetHistsFromFile("h_eta_phi_pT_blue_down");
    bhs_eta_yellow_up_phi_pT->GetHistsFromFile("h_eta_phi_pT_yellow_up");
    bhs_eta_yellow_down_phi_pT->GetHistsFromFile("h_eta_phi_pT_yellow_down");
    bhs_eta_blue_up_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_blue_down_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_blue_up_forward_phi_pT->GetHistsFromFile("h_eta_phi_pT_blue_up_forward");
    bhs_eta_blue_down_forward_phi_pT->GetHistsFromFile("h_eta_phi_pT_blue_down_forward");
    bhs_eta_yellow_up_forward_phi_pT->GetHistsFromFile("h_eta_phi_pT_yellow_up_forward");
    bhs_eta_yellow_down_forward_phi_pT->GetHistsFromFile("h_eta_phi_pT_yellow_down_forward");
    bhs_eta_lowEta_blue_up_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_lowEta_blue_down_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_lowEta_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_lowEta_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_lowEta_blue_up_forward_phi_pT->GetHistsFromFile("h_eta_lowEta_phi_pT_blue_up_forward");
    bhs_eta_lowEta_blue_down_forward_phi_pT->GetHistsFromFile("h_eta_lowEta_phi_pT_blue_down_forward");
    bhs_eta_lowEta_yellow_up_forward_phi_pT->GetHistsFromFile("h_eta_lowEta_phi_pT_yellow_up_forward");
    bhs_eta_lowEta_yellow_down_forward_phi_pT->GetHistsFromFile("h_eta_lowEta_phi_pT_yellow_down_forward");
    bhs_eta_highEta_blue_up_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_highEta_blue_down_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_highEta_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_highEta_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_highEta_blue_up_forward_phi_pT->GetHistsFromFile("h_eta_highEta_phi_pT_blue_up_forward");
    bhs_eta_highEta_blue_down_forward_phi_pT->GetHistsFromFile("h_eta_highEta_phi_pT_blue_down_forward");
    bhs_eta_highEta_yellow_up_forward_phi_pT->GetHistsFromFile("h_eta_highEta_phi_pT_yellow_up_forward");
    bhs_eta_highEta_yellow_down_forward_phi_pT->GetHistsFromFile("h_eta_highEta_phi_pT_yellow_down_forward");
    bhs_eta_blue_up_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_blue_down_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_yellow_up_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_yellow_down_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_eta_blue_up_backward_phi_pT->GetHistsFromFile("h_eta_phi_pT_blue_up_backward");
    bhs_eta_blue_down_backward_phi_pT->GetHistsFromFile("h_eta_phi_pT_blue_down_backward");
    bhs_eta_yellow_up_backward_phi_pT->GetHistsFromFile("h_eta_phi_pT_yellow_up_backward");
    bhs_eta_yellow_down_backward_phi_pT->GetHistsFromFile("h_eta_phi_pT_yellow_down_backward");
    bhs_eta_blue_up_phi_xF = new BinnedHistSet(Form("%sphi_xF_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_eta_blue_down_phi_xF = new BinnedHistSet(Form("%sphi_xF_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_eta_yellow_up_phi_xF = new BinnedHistSet(Form("%sphi_xF_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_eta_yellow_down_phi_xF = new BinnedHistSet(Form("%sphi_xF_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_eta_blue_up_phi_xF->GetHistsFromFile("h_eta_phi_xF_blue_up");
    bhs_eta_blue_down_phi_xF->GetHistsFromFile("h_eta_phi_xF_blue_down");
    bhs_eta_yellow_up_phi_xF->GetHistsFromFile("h_eta_phi_xF_yellow_up");
    bhs_eta_yellow_down_phi_xF->GetHistsFromFile("h_eta_phi_xF_yellow_down");
    bhs_eta_blue_up_phi_eta = new BinnedHistSet(Form("%sphi_eta_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_eta_blue_down_phi_eta = new BinnedHistSet(Form("%sphi_eta_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_eta_yellow_up_phi_eta = new BinnedHistSet(Form("%sphi_eta_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_eta_yellow_down_phi_eta = new BinnedHistSet(Form("%sphi_eta_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_eta_blue_up_phi_eta->GetHistsFromFile("h_eta_phi_eta_blue_up");
    bhs_eta_blue_down_phi_eta->GetHistsFromFile("h_eta_phi_eta_blue_down");
    bhs_eta_yellow_up_phi_eta->GetHistsFromFile("h_eta_phi_eta_yellow_up");
    bhs_eta_yellow_down_phi_eta->GetHistsFromFile("h_eta_phi_eta_yellow_down");
    bhs_eta_blue_up_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_eta_blue_down_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_eta_yellow_up_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_eta_yellow_down_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", vtxzbins);
    bhs_eta_blue_up_phi_vtxz->GetHistsFromFile("h_eta_phi_vtxz_blue_up");
    bhs_eta_blue_down_phi_vtxz->GetHistsFromFile("h_eta_phi_vtxz_blue_down");
    bhs_eta_yellow_up_phi_vtxz->GetHistsFromFile("h_eta_phi_vtxz_yellow_up");
    bhs_eta_yellow_down_phi_vtxz->GetHistsFromFile("h_eta_phi_vtxz_yellow_down");

    nameprefix = "h_pi0bkgr_";
    titlewhich = "#pi^{0} Background";
    bhs_pi0bkgr_blue_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_blue_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_yellow_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_yellow_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_blue_up_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_blue_up");
    bhs_pi0bkgr_blue_down_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_blue_down");
    bhs_pi0bkgr_yellow_up_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_yellow_up");
    bhs_pi0bkgr_yellow_down_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_yellow_down");
    bhs_pi0bkgr_blue_up_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_blue_down_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_blue_up_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_blue_up_forward");
    bhs_pi0bkgr_blue_down_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_blue_down_forward");
    bhs_pi0bkgr_yellow_up_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_yellow_up_forward");
    bhs_pi0bkgr_yellow_down_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_yellow_down_forward");
    bhs_pi0bkgr_blue_up_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_blue_down_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_yellow_up_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_yellow_down_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_blue_up_backward_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_blue_up_backward");
    bhs_pi0bkgr_blue_down_backward_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_blue_down_backward");
    bhs_pi0bkgr_yellow_up_backward_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_yellow_up_backward");
    bhs_pi0bkgr_yellow_down_backward_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_yellow_down_backward");
    bhs_pi0bkgr_lowEta_blue_up_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_lowEta_blue_down_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_lowEta_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_lowEta_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%slowEta_phi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_lowEta_blue_up_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_lowEta_phi_pT_blue_up_forward");
    bhs_pi0bkgr_lowEta_blue_down_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_lowEta_phi_pT_blue_down_forward");
    bhs_pi0bkgr_lowEta_yellow_up_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_lowEta_phi_pT_yellow_up_forward");
    bhs_pi0bkgr_lowEta_yellow_down_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_lowEta_phi_pT_yellow_down_forward");
    bhs_pi0bkgr_highEta_blue_up_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_highEta_blue_down_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_highEta_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_highEta_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%shighEta_phi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_pi0);
    bhs_pi0bkgr_highEta_blue_up_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_highEta_phi_pT_blue_up_forward");
    bhs_pi0bkgr_highEta_blue_down_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_highEta_phi_pT_blue_down_forward");
    bhs_pi0bkgr_highEta_yellow_up_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_highEta_phi_pT_yellow_up_forward");
    bhs_pi0bkgr_highEta_yellow_down_forward_phi_pT->GetHistsFromFile("h_pi0bkgr_highEta_phi_pT_yellow_down_forward");

    bhs_pi0bkgr_blue_up_phi_xF = new BinnedHistSet(Form("%sphi_xF_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_pi0bkgr_blue_down_phi_xF = new BinnedHistSet(Form("%sphi_xF_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_pi0bkgr_yellow_up_phi_xF = new BinnedHistSet(Form("%sphi_xF_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_pi0bkgr_yellow_down_phi_xF = new BinnedHistSet(Form("%sphi_xF_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_pi0bkgr_blue_up_phi_xF->GetHistsFromFile("h_pi0bkgr_phi_xF_blue_up");
    bhs_pi0bkgr_blue_down_phi_xF->GetHistsFromFile("h_pi0bkgr_phi_xF_blue_down");
    bhs_pi0bkgr_yellow_up_phi_xF->GetHistsFromFile("h_pi0bkgr_phi_xF_yellow_up");
    bhs_pi0bkgr_yellow_down_phi_xF->GetHistsFromFile("h_pi0bkgr_phi_xF_yellow_down");
    bhs_pi0bkgr_blue_up_phi_eta = new BinnedHistSet(Form("%sphi_eta_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_pi0bkgr_blue_down_phi_eta = new BinnedHistSet(Form("%sphi_eta_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_pi0bkgr_yellow_up_phi_eta = new BinnedHistSet(Form("%sphi_eta_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_pi0bkgr_yellow_down_phi_eta = new BinnedHistSet(Form("%sphi_eta_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_pi0bkgr_blue_up_phi_eta->GetHistsFromFile("h_pi0bkgr_phi_eta_blue_up");
    bhs_pi0bkgr_blue_down_phi_eta->GetHistsFromFile("h_pi0bkgr_phi_eta_blue_down");
    bhs_pi0bkgr_yellow_up_phi_eta->GetHistsFromFile("h_pi0bkgr_phi_eta_yellow_up");
    bhs_pi0bkgr_yellow_down_phi_eta->GetHistsFromFile("h_pi0bkgr_phi_eta_yellow_down");
    bhs_pi0bkgr_blue_up_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_pi0bkgr_blue_down_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_pi0bkgr_yellow_up_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_pi0bkgr_yellow_down_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", vtxzbins);
    bhs_pi0bkgr_blue_up_phi_vtxz->GetHistsFromFile("h_pi0bkgr_phi_vtxz_blue_up");
    bhs_pi0bkgr_blue_down_phi_vtxz->GetHistsFromFile("h_pi0bkgr_phi_vtxz_blue_down");
    bhs_pi0bkgr_yellow_up_phi_vtxz->GetHistsFromFile("h_pi0bkgr_phi_vtxz_yellow_up");
    bhs_pi0bkgr_yellow_down_phi_vtxz->GetHistsFromFile("h_pi0bkgr_phi_vtxz_yellow_down");

    nameprefix = "h_etabkgr_";
    titlewhich = "#eta Background";
    bhs_etabkgr_blue_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_blue_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_yellow_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_yellow_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_blue_up_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_blue_up");
    bhs_etabkgr_blue_down_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_blue_down");
    bhs_etabkgr_yellow_up_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_yellow_up");
    bhs_etabkgr_yellow_down_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_yellow_down");
    bhs_etabkgr_blue_up_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_blue_down_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_yellow_up_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_yellow_down_forward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_blue_up_forward_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_blue_up_forward");
    bhs_etabkgr_blue_down_forward_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_blue_down_forward");
    bhs_etabkgr_yellow_up_forward_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_yellow_up_forward");
    bhs_etabkgr_yellow_down_forward_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_yellow_down_forward");
    bhs_etabkgr_blue_up_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_blue_down_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_yellow_up_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_yellow_down_backward_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins_eta);
    bhs_etabkgr_blue_up_backward_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_blue_up_backward");
    bhs_etabkgr_blue_down_backward_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_blue_down_backward");
    bhs_etabkgr_yellow_up_backward_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_yellow_up_backward");
    bhs_etabkgr_yellow_down_backward_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_yellow_down_backward");
    bhs_etabkgr_blue_up_phi_xF = new BinnedHistSet(Form("%sphi_xF_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_etabkgr_blue_down_phi_xF = new BinnedHistSet(Form("%sphi_xF_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_etabkgr_yellow_up_phi_xF = new BinnedHistSet(Form("%sphi_xF_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_etabkgr_yellow_down_phi_xF = new BinnedHistSet(Form("%sphi_xF_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    bhs_etabkgr_blue_up_phi_xF->GetHistsFromFile("h_etabkgr_phi_xF_blue_up");
    bhs_etabkgr_blue_down_phi_xF->GetHistsFromFile("h_etabkgr_phi_xF_blue_down");
    bhs_etabkgr_yellow_up_phi_xF->GetHistsFromFile("h_etabkgr_phi_xF_yellow_up");
    bhs_etabkgr_yellow_down_phi_xF->GetHistsFromFile("h_etabkgr_phi_xF_yellow_down");
    bhs_etabkgr_blue_up_phi_eta = new BinnedHistSet(Form("%sphi_eta_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_etabkgr_blue_down_phi_eta = new BinnedHistSet(Form("%sphi_eta_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_etabkgr_yellow_up_phi_eta = new BinnedHistSet(Form("%sphi_eta_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_etabkgr_yellow_down_phi_eta = new BinnedHistSet(Form("%sphi_eta_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    bhs_etabkgr_blue_up_phi_eta->GetHistsFromFile("h_etabkgr_phi_eta_blue_up");
    bhs_etabkgr_blue_down_phi_eta->GetHistsFromFile("h_etabkgr_phi_eta_blue_down");
    bhs_etabkgr_yellow_up_phi_eta->GetHistsFromFile("h_etabkgr_phi_eta_yellow_up");
    bhs_etabkgr_yellow_down_phi_eta->GetHistsFromFile("h_etabkgr_phi_eta_yellow_down");
    bhs_etabkgr_blue_up_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_etabkgr_blue_down_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_etabkgr_yellow_up_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx}", vtxzbins);
    bhs_etabkgr_yellow_down_phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", vtxzbins);
    bhs_etabkgr_blue_up_phi_vtxz->GetHistsFromFile("h_etabkgr_phi_vtxz_blue_up");
    bhs_etabkgr_blue_down_phi_vtxz->GetHistsFromFile("h_etabkgr_phi_vtxz_blue_down");
    bhs_etabkgr_yellow_up_phi_vtxz->GetHistsFromFile("h_etabkgr_phi_vtxz_yellow_up");
    bhs_etabkgr_yellow_down_phi_vtxz->GetHistsFromFile("h_etabkgr_phi_vtxz_yellow_down");
}

void TSSAplotter::PlotNMHists() {
    std::string outfilename = NMpdfname;
    std::string outfilename_start = outfilename + "(";
    std::string outfilename_end = outfilename + ")";
    TCanvas* c = new TCanvas("c", "c", 1600, 900);

    PlotSingleHist(h_nEvents, outfilename_start);
    PlotSingleHist(h_vtxz, outfilename);
    PlotSingle2DHist(h_towerEta_Phi_500MeV, outfilename);
    PlotSingle2DHist(h_towerEta_Phi_500MeV, outfilename, true);
    PlotSingle2DHist(h_towerEta_Phi_800MeV, outfilename);
    PlotSingle2DHist(h_towerEta_Phi_800MeV, outfilename, true);

    // clusters
    PlotSingleHist(h_nAllClusters, outfilename);
    PlotSingleHist(h_nGoodClusters, outfilename);
    PlotSingleHist(h_clusterE, outfilename, true);
    PlotSingleHist(h_clusterEta, outfilename);
    PlotSingle2DHist(h_clusterVtxz_Eta, outfilename);
    PlotSingle2DHist(h_clusterVtxz_Eta, outfilename, true);
    PlotSingleHist(h_clusterPhi, outfilename);
    PlotSingle2DHist(h_clusterEta_Phi, outfilename);
    PlotSingleHist(h_clusterpT, outfilename, true);
    PlotSingleHist(h_clusterxF, outfilename, true);
    PlotSingle2DHist(h_clusterpT_xF, outfilename, true);
    PlotSingleHist(h_clusterChi2, outfilename, true);
    PlotSingleHist(h_clusterChi2zoomed, outfilename, true);
    c->SetLogy();
    normalize_hist(h_clusterChi2zoomed);
    h_clusterChi2zoomed->Draw();
    normalize_hist(h_mesonClusterChi2);
    /* h_mesonClusterChi2->Scale(1.0/h_mesonClusterChi2->Integral()); */
    h_mesonClusterChi2->Draw("same");
    c->SaveAs(outfilename.c_str());
    h_clusterChi2zoomed->Scale(h_clusterChi2zoomed->GetEntries()/h_clusterChi2zoomed->Integral());
    h_mesonClusterChi2->Scale(h_mesonClusterChi2->GetEntries()/h_mesonClusterChi2->Integral());
    // good clusters
    PlotSingleHist(h_goodClusterE, outfilename, true);
    PlotSingleHist(h_goodClusterEta, outfilename);
    PlotSingle2DHist(h_goodClusterVtxz_Eta, outfilename);
    PlotSingle2DHist(h_goodClusterVtxz_Eta, outfilename, true);
    PlotSingleHist(h_goodClusterPhi, outfilename);
    PlotSingle2DHist(h_goodClusterEta_Phi, outfilename);
    PlotSingleHist(h_goodClusterpT, outfilename, true);
    PlotSingleHist(h_goodClusterxF, outfilename, true);
    PlotSingle2DHist(h_goodClusterpT_xF, outfilename, true);

    // diphotons
    PlotSingleHist(h_nDiphotons, outfilename);
    PlotSingleHist(h_nRecoPi0s, outfilename);
    PlotSingleHist(h_nRecoEtas, outfilename);
    PlotSingleHist(h_diphotonMass, outfilename);
    PlotSingleHist(h_diphotonE, outfilename, true);
    PlotSingleHist(h_diphotonEta, outfilename);
    PlotSingle2DHist(h_diphotonVtxz_Eta, outfilename, true);
    PlotSingleHist(h_diphotonPhi, outfilename);
    PlotSingle2DHist(h_diphotonEta_Phi, outfilename);
    PlotSingleHist(h_diphotonpT, outfilename, true);
    PlotSingleHist(h_diphotonxF, outfilename, true);
    PlotSingle2DHist(h_diphotonpT_xF, outfilename, true);
    PlotSingleHist(h_diphotonAsym, outfilename);
    PlotSingleHist(h_diphotonDeltaR, outfilename);
    PlotSingle2DHist(h_diphotonAsym_DeltaR, outfilename, true);

    bhs_diphotonMass_pT->PlotAllHistsWithFits(outfilename, false, "mass2");
    /* bhs_diphotonMass_pT->hist_vec.at(0)->Draw("x0 e1"); */
    bhs_diphotonMass_xF->PlotAllHistsWithFits(outfilename, false, "mass2");

    PlotSingleHist(h_vtxz, outfilename_end);
}

void TSSAplotter::PlotTSSAs() {
    std::string outfilename = TSSApdfname;
    std::string outfilename_start = outfilename + "(";
    std::string outfilename_end = outfilename + ")";
    /* TCanvas* c = new TCanvas("c", "c", 1600, 900); */

    PlotSingleHist(h_nClustersMinbiasTrig, outfilename_start);
    PlotSingleHist(h_nClustersPhotonTrig, outfilename);
    PlotSingleHist(h_Nevents, outfilename);
    PlotSingleHist(h_Npi0s, outfilename);
    PlotSingleHist(h_Netas, outfilename);
    PlotSingle2DHist(h_DiphotonMassAsym, outfilename);
    PlotSingle2DHist(h_DiphotonMassdeltaR, outfilename);
    PlotSingle2DHist(h_DiphotondeltaRAsym, outfilename);
    PlotSingle2DHist(h_DiphotondeltaRAsym_pi0, outfilename);
    PlotSingle2DHist(h_DiphotondeltaRAsym_pi0_smalldR, outfilename);
    PlotSingle2DHist(h_DiphotondeltaRAsym_eta, outfilename);
    PlotSingle2DHist(h_DiphotondeltaRAsym_etabkgr, outfilename);
    PlotSingle2DHist(h_DiphotonMassAsym_highpT, outfilename);
    PlotSingle2DHist(h_DiphotonMassdeltaR_highpT, outfilename);
    PlotSingle2DHist(h_DiphotondeltaRAsym_highpT, outfilename);
    PlotSingle2DHist(h_DiphotondeltaRAsym_highpT_smalldR, outfilename);
    PlotSingle2DHist(h_DiphotondeltaRAsym_eta_highpT, outfilename);
    PlotSingle2DHist(h_DiphotondeltaRAsym_etabkgr_highpT, outfilename);
    PlotSingle2DHist(h_armen_all, outfilename, true);
    PlotSingle2DHist(h_armen_pi0, outfilename, true);
    PlotSingle2DHist(h_armen_pi0bkgr, outfilename, true);
    PlotSingle2DHist(h_armen_eta, outfilename, true);
    PlotSingle2DHist(h_armen_etabkgr, outfilename, true);
    PlotSingle2DHist(h_armen_eta_highpT, outfilename, true);
    PlotSingle2DHist(h_armen_etabkgr_highpT, outfilename, true);
    PlotSingleHist(h_armen_p_L, outfilename, true);
    PlotSingleHist(h_armen_p_T, outfilename, true);
    PlotSingle2DHist(h_armen_alpha_alphaE, outfilename, true);
    PlotSingle2DHist(h_armen_alpha_deltaR, outfilename, true);
    PlotSingle2DHist(h_armen_alpha_deltaR_pi0, outfilename, true);
    PlotSingle2DHist(h_armen_alpha_deltaR_eta, outfilename, true);
    PlotSingle2DHist(h_armen_p_T_deltaR, outfilename, true);
    PlotSingle2DHist(h_armen_pT_p_L, outfilename, true);
    PlotSingle2DHist(h_armen_pT_p_T, outfilename, true);

    bhs_diphotonMass_pT_pi0binning->PlotAllHistsWithFits(outfilename, false, "mass2");
    bhs_diphotonMass_pT_etabinning->PlotAllHistsWithFits(outfilename, false, "mass2");
    PlotBackgroundFractions(outfilename);
    bhs_pi0_pT_pT->PlotAllHistsWithFits(outfilename, false, "");
    bhs_eta_pT_pT->PlotAllHistsWithFits(outfilename, false, "");
    bhs_diphotonxF_xF->PlotAllHistsWithFits(outfilename, false, "");
    bhs_diphotoneta_eta->PlotAllHistsWithFits(outfilename, false, "");
    bhs_diphotonvtxz_vtxz->PlotAllHistsWithFits(outfilename, false, "");

    PlotPi0(outfilename);
    PlotEta(outfilename);

    PlotSingleHist(h_nClustersMinbiasTrig, outfilename_end);
}

double TSSAplotter::RelLumiAsym(double Nup, double Ndown, double relLumi) {
    double asym = (Nup - relLumi*Ndown)/(Nup + relLumi*Ndown);
    return asym;
}

double TSSAplotter::RelLumiError(double Nup, double Ndown, double relLumi) {
    double asym = RelLumiAsym(Nup, Ndown, relLumi);
    double numerator = Nup - relLumi*Ndown;
    /* double num_err = sqrt(Nup) - relLumi*sqrt(Ndown); */
    double num_err = sqrt(Nup + (relLumi*relLumi*Ndown));
    double num_rel_err = num_err/numerator;
    double denominator = Nup + relLumi*Ndown;
    /* double den_err = sqrt(Nup) + relLumi*sqrt(Ndown); */
    double den_err = sqrt(Nup + (relLumi*relLumi*Ndown));
    double den_rel_err = den_err/denominator;
    
    double error = abs(asym)*sqrt((num_rel_err*num_rel_err) + (den_rel_err*den_rel_err));
    /* std::cout << Form("numerator=%f, num_err=%f, num_rel_err=%f\ndenominator=%f, den_err=%f, den_rel_err=%f\nasym=%f, asym_err=%f, asym_rel_err=%f\n", numerator, num_err, num_rel_err, denominator, den_err, den_rel_err, asym, error, (error/asym)); */
    return error;
}

double TSSAplotter::SqrtAsym(double NLup, double NLdown, double NRup, double NRdown) {
    double asym = (sqrt(NLup*NRdown)-sqrt(NRup*NLdown))/(sqrt(NLup*NRdown)+sqrt(NRup*NLdown));
    return asym;
}

double TSSAplotter::SqrtError(double NLup, double NLdown, double NRup, double NRdown,
	double NLupErr, double NLdownErr, double NRupErr, double NRdownErr) {
    double t1 = sqrt(NLdown*NRup*NRdown/NLup)*NLupErr;
    double t2 = sqrt(NLup*NRup*NRdown/NLdown)*NLdownErr;
    double t3 = sqrt(NLup*NLdown*NRdown/NRup)*NRupErr;
    double t4 = sqrt(NLup*NLdown*NRup/NRdown)*NRdownErr;
    double asym_err = (1/pow(sqrt(NLup*NRdown)+sqrt(NRup*NLdown),2))*sqrt(pow(t1,2)+pow(t2,2)+pow(t3,2)+pow(t4,2));
    return asym_err;
}

TGraphErrors* TSSAplotter::RelLumiGraph(TH1* phi_up, TH1* phi_down, double relLumi) {
    TGraphErrors* gr = new TGraphErrors();
    int nbins = phi_up->GetNbinsX();
    for (int i = 0; i < nbins; i++)
    {
	float phi = i*(2*PI/nbins) - (PI - PI/nbins);
	int phibin = i;
	
	double Nup = phi_up->GetBinContent(1+phibin);
	double Ndown = phi_down->GetBinContent(1+phibin);
	/* double NupErr = phi_up->GetBinError(1+phibin); */
	/* double NdownErr = phi_down->GetBinError(1+phibin); */

	double asym = RelLumiAsym(Nup, Ndown, relLumi);
	double err = RelLumiError(Nup, Ndown, relLumi);
	gr->SetPoint(i, phi, asym);
	gr->SetPointError(i, 0, err);
    }
    return gr;
}

TGraphErrors* TSSAplotter::SqrtGraph(TH1* phi_up, TH1* phi_down) {
    TGraphErrors* gr = new TGraphErrors();
    int nbins = phi_up->GetNbinsX();
    int npoints = (int)(nbins / 2);
    int offset = (int)(npoints / 2);
    for (int i = 0; i < npoints; i++)
    {
	float phi = i*(PI/npoints) - (PI/2 - PI/npoints/2);
	int phibinL = (i + offset) % nbins; // N_Left
	int phibinR = (phibinL + npoints + offset) % nbins; // N_Right
	
	double NLup = phi_up->GetBinContent(1+phibinL);
	double NLdown = phi_down->GetBinContent(1+phibinL);
	double NRup = phi_up->GetBinContent(1+phibinR);
	double NRdown = phi_down->GetBinContent(1+phibinR);
	double NLupErr = phi_up->GetBinError(1+phibinL);
	double NLdownErr = phi_down->GetBinError(1+phibinL);
	double NRupErr = phi_up->GetBinError(1+phibinR);
	double NRdownErr = phi_down->GetBinError(1+phibinR);

	double asym = SqrtAsym(NLup, NLdown, NRup, NRdown);
	double err = SqrtError(NLup, NLdown, NRup, NRdown,
		NLupErr, NLdownErr, NRupErr, NRdownErr);
	gr->SetPoint(i, phi, asym);
	gr->SetPointError(i, 0, err);
    }
    return gr;
}

void TSSAplotter::PlotOneAsym(std::string type, std::string title, TPad* pad, TGraphErrors* gr) {
    pad->cd();
    pad->Clear();
    double fit_lower, fit_upper;
    if (type == "sqrt") {
	fit_lower = -PI/2.0;
	fit_upper = PI/2.0;
    }
    else {
	fit_lower = -PI;
	fit_upper = PI;
    }

    TF1* fit = nullptr;
    if (type == "sqrt" || type == "rellumi") {
	fit = new TF1("asymfit", "[0]*sin(x - [1])", fit_lower, fit_upper);
	/* fit->SetParLimits(0, 0.00001, 0.4); */
	fit->SetParLimits(0, -0.4, 0.4);
	fit->SetParameter(0, 0.01);
	fit->SetParLimits(1, PI-0.2, PI+0.2);
	/* fit->SetParLimits(1, 0.0, 2*PI); */
	fit->SetParameter(1, 3.141592);
	fit->SetParName(0, "#epsilon");
	fit->SetParName(1, "#phi_{0}");
    }
    else {
	std::cout << "Invalid type option " << type << "; exiting!" << std::endl;
	return;
    }

    gr->SetTitle(title.c_str());
    gr->GetXaxis()->SetTitle("#phi");
    if (type == "sqrt") {
	gr->GetYaxis()->SetTitle("Raw Asymmetry (Square Root)");
    }
    else {
	gr->GetYaxis()->SetTitle("Raw Asymmetry (Rel. Lumi.)");
    }
    gStyle->SetOptFit();
    gStyle->SetOptStat(0);
    gr->Draw("ap");
    gr->Fit(fit, "Q");
    gr->GetXaxis()->SetRangeUser(fit_lower, fit_upper);
    gr->GetYaxis()->SetRangeUser(-0.1, 0.1);

    TLatex* tl = new TLatex;
    tl->SetTextColor(kRed);
    tl->DrawLatexNDC(0.20, 0.70, Form("#splitline{#epsilon = %.4f #pm %.4f}{#epsilon/#delta#epsilon = %.4f}", fit->GetParameter(0), fit->GetParError(0), fit->GetParameter(0)/fit->GetParError(0)));
    /* for (int i=0; i<gr->GetN(); i++) { */
	/* std::cout << "Point " << i << ": x=" << gr->GetPointX(i) << ", y=" << gr->GetPointY(i) << " +- " << gr->GetErrorY(i) << std::endl; */
    /* } */
    pad->Update();
}

void TSSAplotter::CompareOneAsym(std::string title, std::string outfilename, TGraphErrors* rl, TGraphErrors* sqrt) {
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    double fit_lower = -PI;
    double fit_upper = PI;

    TF1* fit_rl = new TF1("fit_rl", "[0]*sin(x - [1])", fit_lower, fit_upper);
    fit_rl->SetParLimits(0, 0.00001, 0.4);
    fit_rl->SetParameter(0, 0.01);
    fit_rl->SetParLimits(1, 0.0, 2*PI);
    fit_rl->SetParameter(1, 3.141592);
    fit_rl->SetParName(0, "#epsilon");
    fit_rl->SetParName(1, "#phi_{0}");
    fit_rl->SetLineColor(kRed);

    rl->SetTitle(title.c_str());
    rl->GetXaxis()->SetTitle("#phi");
    rl->GetYaxis()->SetTitle("Raw Asymmetry");
    rl->GetXaxis()->SetRangeUser(fit_lower, fit_upper);
    rl->GetYaxis()->SetRangeUser(-0.002, 0.002);
    /* std::cout << "RelLumi graph" << std::endl; */
    for (int i=0; i<rl->GetN(); i++) {
	std::cout << "Point " << i << ": x=" << rl->GetPointX(i) << ", y=" << rl->GetPointY(i) << " +- " << rl->GetErrorY(i) << std::endl;
    }

    int rl_marker = rl->GetMarkerStyle();
    int rl_mcolor = rl->GetMarkerColor();
    int rl_lcolor = rl->GetLineColor();
    rl->SetMarkerStyle(kStar);
    rl->SetMarkerColor(kRed);
    rl->SetLineColor(kRed);

    TF1* fit_sqrt = new TF1("fit_sqrt", "[0]*sin(x - [1])", fit_lower/2.0, fit_upper/2.0);
    fit_sqrt->SetParLimits(0, 0.00001, 0.4);
    fit_sqrt->SetParameter(0, 0.01);
    fit_sqrt->SetParLimits(1, 0.0, 2*PI);
    fit_sqrt->SetParameter(1, 3.141592);
    fit_sqrt->SetParName(0, "#epsilon");
    fit_sqrt->SetParName(1, "#phi_{0}");
    fit_sqrt->SetLineColor(kBlue);

    sqrt->SetTitle(title.c_str());
    sqrt->GetXaxis()->SetTitle("#phi");
    sqrt->GetYaxis()->SetTitle("Raw Asymmetry");
    sqrt->GetXaxis()->SetRangeUser(fit_lower, fit_upper);
    /* sqrt->GetYaxis()->SetRangeUser(-0.05, 0.05); */
    /* std::cout << "Sqrt graph" << std::endl; */
    for (int i=0; i<sqrt->GetN(); i++) {
	std::cout << "Point " << i << ": x=" << sqrt->GetPointX(i) << ", y=" << sqrt->GetPointY(i) << " +- " << sqrt->GetErrorY(i) << std::endl;
    }

    int sqrt_marker = sqrt->GetMarkerStyle();
    int sqrt_mcolor = sqrt->GetMarkerColor();
    int sqrt_lcolor = sqrt->GetLineColor();
    sqrt->SetMarkerStyle(kCircle);
    sqrt->SetMarkerColor(kBlue);
    sqrt->SetLineColor(kBlue);
    /* std::cout << "sqrt nPoints = " << sqrt->GetN() << std::endl; */
    /* std::cout << "rl nPoints = " << rl->GetN() << std::endl; */

    gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);
    rl->Draw("ap");
    rl->Fit("fit_rl", "Q");
    sqrt->Draw("p");
    sqrt->Fit("fit_sqrt", "Q");
    TLatex* tl_rl = new TLatex;
    tl_rl->SetTextColor(kRed);
    tl_rl->DrawLatexNDC(0.20, 0.75, Form("#splitline{#epsilon = %f #pm %f}{#phi_{0} = %f #pm %f}", fit_rl->GetParameter(0), fit_rl->GetParError(0), fit_rl->GetParameter(1), fit_rl->GetParError(1)));
    TLatex* tl_sqrt = new TLatex;
    tl_sqrt->SetTextColor(kBlue);
    tl_sqrt->DrawLatexNDC(0.20, 0.25, Form("#splitline{#epsilon = %f #pm %f}{#phi_{0} = %f #pm %f}", fit_sqrt->GetParameter(0), fit_sqrt->GetParError(0), fit_sqrt->GetParameter(1), fit_sqrt->GetParError(1)));
    TLegend leg(0.75, 0.75, 0.9, 0.9, "", "nbNDC");
    leg.AddEntry(rl, "Rel. Lumi", "ep");
    leg.AddEntry(sqrt, "Geometric", "ep");
    leg.Draw();

    c1->SaveAs(outfilename.c_str());
    rl->SetMarkerStyle(rl_marker);
    rl->SetMarkerColor(rl_mcolor);
    rl->SetLineColor(rl_lcolor);
    sqrt->SetMarkerStyle(sqrt_marker);
    sqrt->SetMarkerColor(sqrt_mcolor);
    sqrt->SetLineColor(sqrt_lcolor);
}

void TSSAplotter::PlotAsymsBinned(std::string type, std::string which, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down, std::string outfilename) {
    TCanvas* c1 = new TCanvas(which.c_str(), "", 1600, 900);
    c1->Divide(4, 2, 0.025, 0.025);
    
    int nbins = bhs_up->nbins;
    for (int i=1; i<=nbins; i++) {
	TPad* pad = (TPad*)c1->GetPad(i);
	if (!pad) return;
	pad->cd();
	gStyle->SetPadLeftMargin(0.15);
	gROOT->ForceStyle();
	c1->Update();
	TH1* phi_up = bhs_up->hist_vec[i];
	TH1* phi_down = bhs_down->hist_vec[i];
	TGraphErrors* gr = nullptr;
	if (type == "sqrt") {
	    gr = SqrtGraph(phi_up, phi_down);
	}
	else {
	    bool blue = (which.find("Blue") != std::string::npos);
	    if (blue) {
		gr = RelLumiGraph(phi_up, phi_down, relLumiBlue);
	    }
	    else {
		gr = RelLumiGraph(phi_up, phi_down, relLumiYellow);
	    }
	}
	std::string title_type = "";
	if (type == "sqrt") title_type = "Square Root Asymmetry";
	if (type == "rellumi") title_type = "Rel. Lumi. Asymmetry";
	char* range;
	double lower, upper;
	lower = bhs_up->bin_edges[i-1];
	upper = bhs_up->bin_edges[i];
	range = Form("[%.2f#leq%s<%.2f]", lower, bhs_up->binned_quantity.c_str(), upper);
	/* char* title = Form("%s %s %s", which.c_str(), title_type.c_str(), range); */
	char* title = Form("%s %s", which.c_str(), title_type.c_str());
	PlotOneAsym(type, std::string(title), pad, gr);
	TLatex* textbox = new TLatex(0, 0, "blaahhh");
	/* textbox->SetTextSize(textbox->GetTextSize()*1.25); */
	/* textbox->SetTextColor(kRed); */
	textbox->SetTextAlign(21);
	textbox->DrawLatexNDC(0.35, 0.8, range);
	/* std::cout << "Greg info: binned_quantity = " << bhs_up->binned_quantity << std::endl; */
	/* std::cout << "Greg info: gr num points = " << gr->GetN() << std::endl; */
    }
    c1->Update();
    c1->SaveAs(outfilename.c_str());
    delete c1;
}

void TSSAplotter::PlotAsymsBinned(std::string which, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down, std::string outfilename) {
    TCanvas* c1 = new TCanvas(which.c_str(), "", 1600, 900);
    c1->Divide(4, 2, 0.025, 0.025);
    TCanvas* c2 = new TCanvas("c2", "", 1600, 900);
    
    int nbins = bhs_up->nbins;
    for (int i=1; i<=nbins; i++) {
	TPad* pad = (TPad*)c1->GetPad(i);
	if (!pad) return;
	pad->cd();
	gStyle->SetPadLeftMargin(0.15);
	gROOT->ForceStyle();
	c1->Update();
	TH1* phi_up = bhs_up->hist_vec[i];
	TH1* phi_down = bhs_down->hist_vec[i];
	TGraphErrors* gr_rl = nullptr;
	bool blue = (which.find("Blue") != std::string::npos);
	if (blue) {
	    gr_rl = RelLumiGraph(phi_up, phi_down, relLumiBlue);
	}
	else {
	    gr_rl = RelLumiGraph(phi_up, phi_down, relLumiYellow);
	}
	std::pair<double, double> asym_rl = FitGraph("rellumi", gr_rl);
	if (asym_rl.first == 0.0) { // needed so compiler doesn't complain about unused variables
	}
	TF1* fit_rl = gr_rl->GetFunction("asymfit");
	fit_rl->SetLineColor(kRed-2);
	TGraphErrors* gr_sqrt = SqrtGraph(phi_up, phi_down);
	std::pair<double, double> asym_sqrt = FitGraph("sqrt", gr_sqrt);
	if (asym_sqrt.first == 0.0) { // needed so compiler doesn't complain about unused variables
	}
	TF1* fit_sqrt = gr_sqrt->GetFunction("asymfit");
	fit_sqrt->SetLineColor(kBlue-2);

	std::string title_type = "Raw Asymmetry";
	char* range;
	double lower, upper;
	lower = bhs_up->bin_edges[i-1];
	upper = bhs_up->bin_edges[i];
	range = Form("[%.2f#leq%s<%.2f]", lower, bhs_up->binned_quantity.c_str(), upper);
	/* char* title = Form("%s %s %s", which.c_str(), title_type.c_str(), range); */
	char* title = Form("%s %s", which.c_str(), title_type.c_str());
	gr_rl->SetMarkerColor(kRed);
	gr_rl->SetMarkerStyle(kStar);
	gr_rl->SetLineColor(kRed);
	gr_rl->GetYaxis()->SetTitle("Raw Asymmetry");
	int npoints = gr_sqrt->GetN();
	double offset = (bhs_up->bin_edges[1] - bhs_up->bin_edges[0])/20.0;
	for (int i=0; i<npoints; i++) {
	    gr_sqrt->SetPointX(i, gr_sqrt->GetPointX(i) + offset);
	}
	gr_sqrt->SetMarkerColor(kBlue);
	gr_sqrt->SetMarkerStyle(kCircle);
	gr_sqrt->SetLineColor(kBlue);
	double xmin = std::min(gr_rl->GetXaxis()->GetXmin(), gr_sqrt->GetXaxis()->GetXmin());
	double xmax = std::max(gr_rl->GetXaxis()->GetXmax(), gr_sqrt->GetXaxis()->GetXmax());
	double ymin = std::min(gr_rl->GetYaxis()->GetXmin(), gr_sqrt->GetYaxis()->GetXmin());
	double ymax = std::max(gr_rl->GetYaxis()->GetXmax(), gr_sqrt->GetYaxis()->GetXmax());
	/* TH1F* hdummy = c1->DrawFrame(xmin, ymin, xmax, ymax); */
	TH1F* hdummy = pad->DrawFrame(xmin, 2.0*ymin, xmax, 2.0*ymax);
	/* TH1F* hdummy = c1->DrawFrame(xmin, -0.1, xmax, 0.1); */
	hdummy->SetTitle(title);
	hdummy->GetXaxis()->SetTitle("#phi (rad)");
	hdummy->GetYaxis()->SetTitle("Raw Asymmetry");
	hdummy->GetXaxis()->CenterTitle(true);
	hdummy->GetYaxis()->CenterTitle(true);
	gStyle->SetOptFit(0);
	gStyle->SetOptStat(0);
	gr_rl->Draw("p");
	/* gr_rl->GetYaxis()->SetLimits(ymin, ymax); */
	/* c1->RangeAxis(xmin, ymin, xmax, ymax); */
	gr_sqrt->Draw("p same");

	TLine* line = new TLine();
	line->SetLineColor(kBlack);
	line->DrawLine(xmin, 0, xmax, 0);

	TLegend leg(0.75, 0.75, 0.9, 0.9, "", "nbNDC");
	leg.AddEntry(gr_rl, "Rel. Lumi", "ep");
	leg.AddEntry(gr_sqrt, "Geometric", "ep");
	leg.Draw();

	TLatex* textbox = new TLatex(0, 0, "blaahhh");
	/* textbox->SetTextSize(textbox->GetTextSize()*1.25); */
	/* textbox->SetTextColor(kRed); */
	textbox->SetTextAlign(21);
	textbox->DrawLatexNDC(0.5, 0.85, range);
	/* std::cout << "Greg info: binned_quantity = " << bhs_up->binned_quantity << std::endl; */
	/* std::cout << "Greg info: gr num points = " << gr->GetN() << std::endl; */
	TLatex* tl_rl = new TLatex;
	tl_rl->SetTextColor(kRed);
	tl_rl->DrawLatexNDC(0.30, 0.75, Form("#splitline{#epsilon = %.4f #pm %.4f}{#epsilon/#delta#epsilon = %.4f}", fit_rl->GetParameter(0), fit_rl->GetParError(0), fit_rl->GetParameter(0)/fit_rl->GetParError(0)));
	TLatex* tl_sqrt = new TLatex;
	tl_sqrt->SetTextColor(kBlue);
	tl_sqrt->DrawLatexNDC(0.30, 0.20, Form("#splitline{#epsilon = %.4f #pm %.4f}{#epsilon/#delta#epsilon = %.4f}", fit_sqrt->GetParameter(0), fit_sqrt->GetParError(0), fit_sqrt->GetParameter(0)/fit_sqrt->GetParError(0)));
	pad->Update();

	c2->cd();
	TH1* hdummy2 = (TH1*)hdummy->Clone("duhhh");
	hdummy2->Draw();
	gStyle->SetOptFit(0);
	gStyle->SetOptStat(0);
	gr_rl->Draw("p");
	gr_sqrt->Draw("p same");

	line->SetLineColor(kBlack);
	line->DrawLine(xmin, 0, xmax, 0);

	leg.Draw();

	textbox->DrawLatexNDC(0.5, 0.85, range);
	tl_rl->DrawLatexNDC(0.30, 0.75, Form("#splitline{#epsilon = %.4f #pm %.4f}{#epsilon/#delta#epsilon = %.4f}", fit_rl->GetParameter(0), fit_rl->GetParError(0), fit_rl->GetParameter(0)/fit_rl->GetParError(0)));
	tl_sqrt->DrawLatexNDC(0.30, 0.20, Form("#splitline{#epsilon = %.4f #pm %.4f}{#epsilon/#delta#epsilon = %.4f}", fit_sqrt->GetParameter(0), fit_sqrt->GetParError(0), fit_sqrt->GetParameter(0)/fit_sqrt->GetParError(0)));
	c2->Update();
	c2->SaveAs(outfilename.c_str());
    }
    c1->Update();
    c1->SaveAs(outfilename.c_str());
    delete c1;
}

void TSSAplotter::PlotAsymsBinnedBlueYellow(std::string type, std::string which, BinnedHistSet* bhs_up_blue, BinnedHistSet* bhs_down_blue, BinnedHistSet* bhs_up_yellow, BinnedHistSet* bhs_down_yellow, std::string outfilename) {
    TCanvas* c1 = new TCanvas(which.c_str(), "", 1600, 900);
    c1->Divide(4, 2, 0.025, 0.025);
    TCanvas* c2 = new TCanvas("c2", "", 1600, 900);
    
    int nbins = bhs_up_blue->nbins;
    for (int i=1; i<=nbins; i++) {
	TPad* pad = (TPad*)c1->GetPad(i);
	if (!pad) return;
	pad->cd();
	gStyle->SetPadLeftMargin(0.15);
	gROOT->ForceStyle();
	c1->Update();

	TH1* phi_up_blue = bhs_up_blue->hist_vec[i];
	TH1* phi_down_blue = bhs_down_blue->hist_vec[i];
	TGraphErrors* gr_blue = nullptr;
	if (type == "rellumi") {
	    gr_blue = RelLumiGraph(phi_up_blue, phi_down_blue, relLumiBlue);
	}
	else {
	    gr_blue = SqrtGraph(phi_up_blue, phi_down_blue);
	}
	std::pair<double, double> asym_blue = FitGraph(type, gr_blue);
	if (asym_blue.first == 0.0) { // needed so compiler doesn't complain about unused variables
	}
	TF1* fit_blue = gr_blue->GetFunction("asymfit");
	fit_blue->SetLineColor(kBlue-2);

	TH1* phi_up_yellow = bhs_up_yellow->hist_vec[i];
	TH1* phi_down_yellow = bhs_down_yellow->hist_vec[i];
	TGraphErrors* gr_yellow = nullptr;
	if (type == "rellumi") {
	    gr_yellow = RelLumiGraph(phi_up_yellow, phi_down_yellow, relLumiBlue);
	}
	else {
	    gr_yellow = SqrtGraph(phi_up_yellow, phi_down_yellow);
	}
	std::pair<double, double> asym_yellow = FitGraph(type, gr_yellow);
	if (asym_yellow.first == 0.0) { // needed so compiler doesn't complain about unused variables
	}
	TF1* fit_yellow = gr_yellow->GetFunction("asymfit");
	fit_yellow->SetLineColor(kOrange-2);

	std::string title_type = "Raw Asymmetry";
	char* range;
	double lower, upper;
	lower = bhs_up_blue->bin_edges[i-1];
	upper = bhs_up_blue->bin_edges[i];
	range = Form("[%.2f#leq%s<%.2f]", lower, bhs_up_blue->binned_quantity.c_str(), upper);
	/* char* title = Form("%s %s %s", which.c_str(), title_type.c_str(), range); */
	char* title = Form("%s %s", which.c_str(), title_type.c_str());
	gr_blue->SetMarkerColor(kBlue);
	gr_blue->SetMarkerStyle(kCircle);
	gr_blue->SetLineColor(kBlue);
	gr_blue->GetYaxis()->SetTitle("Raw Asymmetry");
	int npoints = gr_blue->GetN();
	double offset = (bhs_up_blue->bin_edges[1] - bhs_up_blue->bin_edges[0])/20.0;
	for (int i=0; i<npoints; i++) {
	    gr_yellow->SetPointX(i, gr_yellow->GetPointX(i) + offset);
	}
	gr_yellow->SetMarkerColor(kOrange+1);
	gr_yellow->SetMarkerStyle(kStar);
	gr_yellow->SetLineColor(kOrange+1);
	double xmin = std::min(gr_blue->GetXaxis()->GetXmin(), gr_yellow->GetXaxis()->GetXmin());
	double xmax = std::max(gr_blue->GetXaxis()->GetXmax(), gr_yellow->GetXaxis()->GetXmax());
	double ymin = std::min(gr_blue->GetYaxis()->GetXmin(), gr_yellow->GetYaxis()->GetXmin());
	double ymax = std::max(gr_blue->GetYaxis()->GetXmax(), gr_yellow->GetYaxis()->GetXmax());
	/* TH1F* hdummy = c1->DrawFrame(xmin, ymin, xmax, ymax); */
	TH1F* hdummy = pad->DrawFrame(xmin, 2.0*ymin, xmax, 2.0*ymax);
	/* TH1F* hdummy = c1->DrawFrame(xmin, -0.1, xmax, 0.1); */
	hdummy->SetTitle(title);
	hdummy->GetXaxis()->SetTitle("#phi (rad)");
	hdummy->GetYaxis()->SetTitle("Raw Asymmetry");
	hdummy->GetXaxis()->CenterTitle(true);
	hdummy->GetYaxis()->CenterTitle(true);
	gStyle->SetOptFit(0);
	gStyle->SetOptStat(0);
	gr_blue->Draw("p");
	/* gr_blue->GetYaxis()->SetLimits(ymin, ymax); */
	/* c1->RangeAxis(xmin, ymin, xmax, ymax); */
	gr_yellow->Draw("p same");

	TLine* line = new TLine();
	line->SetLineColor(kBlack);
	line->DrawLine(xmin, 0, xmax, 0);

	TLegend leg(0.75, 0.75, 0.9, 0.9, "", "nbNDC");
	leg.AddEntry(gr_blue, "Blue beam", "ep");
	leg.AddEntry(gr_yellow, "Yellow beam", "ep");
	leg.Draw();

	TLatex* textbox = new TLatex(0, 0, "blaahhh");
	/* textbox->SetTextSize(textbox->GetTextSize()*1.25); */
	/* textbox->SetTextColor(kRed); */
	textbox->SetTextAlign(21);
	textbox->DrawLatexNDC(0.5, 0.85, range);
	/* std::cout << "Greg info: binned_quantity = " << bhs_up->binned_quantity << std::endl; */
	/* std::cout << "Greg info: gr num points = " << gr->GetN() << std::endl; */
	TLatex* tl_blue = new TLatex;
	tl_blue->SetTextColor(kBlue);
	tl_blue->DrawLatexNDC(0.30, 0.75, Form("#splitline{#epsilon = %.4f #pm %.4f}{#epsilon/#delta#epsilon = %.4f}", fit_blue->GetParameter(0), fit_blue->GetParError(0), fit_blue->GetParameter(0)/fit_blue->GetParError(0)));
	TLatex* tl_yellow = new TLatex;
	tl_yellow->SetTextColor(kOrange+1);
	tl_yellow->DrawLatexNDC(0.30, 0.20, Form("#splitline{#epsilon = %.4f #pm %.4f}{#epsilon/#delta#epsilon = %.4f}", fit_yellow->GetParameter(0), fit_yellow->GetParError(0), fit_yellow->GetParameter(0)/fit_yellow->GetParError(0)));
	pad->Update();

	c2->cd();
	TH1* hdummy2 = (TH1*)hdummy->Clone("duhhh");
	hdummy2->Draw();
	gStyle->SetOptFit(0);
	gStyle->SetOptStat(0);
	gr_blue->Draw("p");
	gr_yellow->Draw("p same");

	line->SetLineColor(kBlack);
	line->DrawLine(xmin, 0, xmax, 0);

	leg.Draw();

	textbox->DrawLatexNDC(0.5, 0.85, range);
	tl_blue->DrawLatexNDC(0.30, 0.75, Form("#splitline{#epsilon = %.4f #pm %.4f}{#epsilon/#delta#epsilon = %.4f}", fit_blue->GetParameter(0), fit_blue->GetParError(0), fit_blue->GetParameter(0)/fit_blue->GetParError(0)));
	tl_yellow->DrawLatexNDC(0.30, 0.20, Form("#splitline{#epsilon = %.4f #pm %.4f}{#epsilon/#delta#epsilon = %.4f}", fit_yellow->GetParameter(0), fit_yellow->GetParError(0), fit_yellow->GetParameter(0)/fit_yellow->GetParError(0)));
	c2->Update();
	c2->SaveAs(outfilename.c_str());
    }
    c1->Update();
    c1->SaveAs(outfilename.c_str());
    delete c1;
}

std::pair<double, double> TSSAplotter::FitGraph(std::string type, TGraphErrors* gr) {
    double fit_lower, fit_upper;
    if (type == "sqrt") {
	fit_lower = -PI/2.0;
	fit_upper = PI/2.0;
    }
    else {
	fit_lower = -PI;
	fit_upper = PI;
    }

    TF1* fit = nullptr;
    if (type == "sqrt" || type == "rellumi") {
	fit = new TF1("asymfit", "[0]*sin(x - [1])", fit_lower, fit_upper);
	/* fit->SetParLimits(0, 0.00001, 0.4); */
	fit->SetParLimits(0, -0.4, 0.4);
	fit->SetParameter(0, 0.001);
	fit->SetParLimits(1, PI-0.2, PI+0.2);
	/* fit->SetParLimits(1, 0.0, 2*PI); */
	fit->SetParameter(1, 3.141592);
	fit->SetParName(0, "#epsilon");
	fit->SetParName(1, "#phi_{0}");
    }
    else {
	std::cout << "Invalid type option " << type << "; exiting!" << std::endl;
	return std::pair<double, double>();
    }

    gr->Fit(fit, "Q");
    std::pair<double, double> ret;
    ret.first = fit->GetParameter(0);
    ret.second = fit->GetParError(0);
    return ret;
}

TGraphErrors* TSSAplotter::AmplitudeBinnedGraph(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down) {

    // Check xlabel to see which mean x values to use
    std::vector<double> xvalues;
    if (xlabel == "pT_pi0") {
	xlabel = "p_{T} (GeV)";
	xvalues = pTmeans_pi0;
    }
    else if (xlabel == "pT_eta") {
	xlabel = "p_{T} (GeV)";
	xvalues = pTmeans_eta;
    }
    else if (xlabel == "xF") {
	xlabel = "x_{F}";
	xvalues = xFmeans;
    }
    else if (xlabel == "eta") {
	xlabel = "#eta";
	xvalues = etameans;
    }
    else if (xlabel == "vtxz") {
	xlabel = "z_{vtx}";
	xvalues = vtxzmeans;
    }
    else {
	std::cout << "In AmplitudeBinnedGraph(): unrecognized xlabel = " << xlabel << "; exiting!" << std::endl;
	return nullptr;
    }

    int nbins = bhs_up->nbins;
    /* std::vector<double> bin_centers = bhs_up->getBinCenters(bhs_up->bin_edges); */
    TGraphErrors* amplitudes = new TGraphErrors(nbins);
    amplitudes->SetTitle(title.c_str());
    amplitudes->GetXaxis()->SetTitle(xlabel.c_str());
    std::cout << "In AmplitudeBinnedGraph(): xlabel = " << xlabel << std::endl;
    amplitudes->GetXaxis()->CenterTitle(1);
    std::string ylabel;
    if (type == "sqrt") ylabel = "Raw Asymmetry (Square Root)";
    else ylabel = "Raw Asymmetry (Rel. Lumi.)";
    amplitudes->GetYaxis()->SetTitle(ylabel.c_str());
    amplitudes->GetYaxis()->CenterTitle(1);

    for (int i=1; i<=nbins; i++) {
	TH1* phi_up = bhs_up->hist_vec[i];
	TH1* phi_down = bhs_down->hist_vec[i];
	TGraphErrors* gr = nullptr;
	if (type == "sqrt") {
	    gr = SqrtGraph(phi_up, phi_down);
	}
	else {
	    bool blue = (title.find("Blue") != std::string::npos);
	    if (blue) {
		gr = RelLumiGraph(phi_up, phi_down, relLumiBlue);
	    }
	    else {
		gr = RelLumiGraph(phi_up, phi_down, relLumiYellow);
	    }
	}
	std::pair<double, double> ampl_err = FitGraph(type, gr);
	/* amplitudes->SetPoint(i-1, bin_centers[i-1], ampl_err.first); */
	/* double xrange = bhs_up->bin_edges[i] - bhs_up->bin_edges[i-1]; */
	/* amplitudes->SetPointError(i-1, xrange/2.0, ampl_err.second); */
	amplitudes->SetPoint(i-1, xvalues[i-1], ampl_err.first);
	amplitudes->SetPointError(i-1, 0, ampl_err.second);
    }
    return amplitudes;
}

void TSSAplotter::PlotAmplitudes(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down, std::string outfilename) {
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    TGraphErrors* gr = AmplitudeBinnedGraph(type, title, xlabel, bhs_up, bhs_down);
    gr->SetMarkerColor(kBlack);
    gr->SetMarkerStyle(kCircle);
    gr->SetLineColor(kBlack);
    gr->Draw("ap");
    c1->SaveAs(outfilename.c_str());
}

void TSSAplotter::CompareAmplitudes(std::string title, std::string xlabel, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down, std::string outfilename) {
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    TGraphErrors* gr_rl = AmplitudeBinnedGraph("rellumi", title, xlabel, bhs_up, bhs_down);
    gr_rl->SetMarkerColor(kRed);
    gr_rl->SetMarkerStyle(kStar);
    gr_rl->SetLineColor(kRed);
    gr_rl->GetYaxis()->SetTitle("Raw Asymmetry");

    TGraphErrors* gr_sqrt = AmplitudeBinnedGraph("sqrt", title, xlabel, bhs_up, bhs_down);
    int npoints = gr_sqrt->GetN();
    double offset = (bhs_up->bin_edges[1] - bhs_up->bin_edges[0])/20.0;
    for (int i=0; i<npoints; i++) {
	gr_sqrt->SetPointX(i, gr_sqrt->GetPointX(i) + offset);
    }
    gr_sqrt->SetMarkerColor(kBlue);
    gr_sqrt->SetMarkerStyle(kCircle);
    gr_sqrt->SetLineColor(kBlue);

    double xmin = std::min(gr_rl->GetXaxis()->GetXmin(), gr_sqrt->GetXaxis()->GetXmin());
    double xmax = std::max(gr_rl->GetXaxis()->GetXmax(), gr_sqrt->GetXaxis()->GetXmax());
    double ymin = std::min(gr_rl->GetYaxis()->GetXmin(), gr_sqrt->GetYaxis()->GetXmin());
    double ymax = std::max(gr_rl->GetYaxis()->GetXmax(), gr_sqrt->GetYaxis()->GetXmax());
    /* std::cout << "Sqrt min = " << gr_sqrt->GetYaxis()->GetXmin() << ";\tymin = " << ymin << std::endl; */
    TH1F* hdummy = c1->DrawFrame(xmin, ymin, xmax, ymax);
    hdummy->SetTitle(title.c_str());
    hdummy->GetXaxis()->SetTitle(xlabel.c_str());
    hdummy->GetYaxis()->SetTitle("Raw Asymmetry");
    hdummy->GetXaxis()->CenterTitle(true);
    hdummy->GetYaxis()->CenterTitle(true);
    gr_rl->Draw("p");
    /* gr_rl->GetYaxis()->SetLimits(ymin, ymax); */
    /* c1->RangeAxis(xmin, ymin, xmax, ymax); */
    gr_sqrt->Draw("p same");

    TLine* line = new TLine();
    line->SetLineColor(kBlack);
    line->DrawLine(xmin, 0, xmax, 0);

    TLegend leg(0.75, 0.75, 0.9, 0.9, "", "nbNDC");
    leg.AddEntry(gr_rl, "Rel. Lumi", "ep");
    leg.AddEntry(gr_sqrt, "Geometric", "ep");
    leg.Draw();

    c1->SaveAs(outfilename.c_str());
}

void TSSAplotter::CompareAmplitudesEtaBinned(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up_lowEta, BinnedHistSet* bhs_down_lowEta, BinnedHistSet* bhs_up_highEta, BinnedHistSet* bhs_down_highEta, std::string outfilename) {
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    TGraphErrors* gr_lowEta = AmplitudeBinnedGraph(type, title, xlabel, bhs_up_lowEta, bhs_down_lowEta);
    gr_lowEta->SetMarkerColor(kRed);
    gr_lowEta->SetMarkerStyle(kStar);
    gr_lowEta->SetLineColor(kRed);
    gr_lowEta->GetYaxis()->SetTitle("Raw Asymmetry");

    TGraphErrors* gr_highEta = AmplitudeBinnedGraph(type, title, xlabel, bhs_up_highEta, bhs_down_highEta);
    int npoints = gr_highEta->GetN();
    double offset = (bhs_up_lowEta->bin_edges[1] - bhs_up_lowEta->bin_edges[0])/20.0;
    for (int i=0; i<npoints; i++) {
	gr_highEta->SetPointX(i, gr_highEta->GetPointX(i) + offset);
    }
    gr_highEta->SetMarkerColor(kBlue);
    gr_highEta->SetMarkerStyle(kCircle);
    gr_highEta->SetLineColor(kBlue);

    double xmin = std::min(gr_lowEta->GetXaxis()->GetXmin(), gr_highEta->GetXaxis()->GetXmin());
    double xmax = std::max(gr_lowEta->GetXaxis()->GetXmax(), gr_highEta->GetXaxis()->GetXmax());
    double ymin = std::min(gr_lowEta->GetYaxis()->GetXmin(), gr_highEta->GetYaxis()->GetXmin());
    double ymax = std::max(gr_lowEta->GetYaxis()->GetXmax(), gr_highEta->GetYaxis()->GetXmax());

    TH1F* hdummy = c1->DrawFrame(xmin, ymin, xmax, ymax);
    hdummy->SetTitle(title.c_str());
    hdummy->GetXaxis()->SetTitle(xlabel.c_str());
    hdummy->GetYaxis()->SetTitle("Raw Asymmetry");
    hdummy->GetXaxis()->CenterTitle(true);
    hdummy->GetYaxis()->CenterTitle(true);
    gr_lowEta->Draw("p");
    /* gr_rl->GetYaxis()->SetLimits(ymin, ymax); */
    /* c1->RangeAxis(xmin, ymin, xmax, ymax); */
    gr_highEta->Draw("p same");

    TLine* line = new TLine();
    line->SetLineColor(kBlack);
    line->DrawLine(xmin, 0, xmax, 0);

    TLegend leg(0.75, 0.75, 0.9, 0.9, "", "nbNDC");
    leg.AddEntry(gr_lowEta, "|#eta| < 0.35", "ep");
    leg.AddEntry(gr_highEta, "|#eta| > 0.35", "ep");
    leg.Draw();

    c1->SaveAs(outfilename.c_str());
}

void TSSAplotter::CompareAmplitudesBlueYellow(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up_blue, BinnedHistSet* bhs_down_blue, BinnedHistSet* bhs_up_yellow, BinnedHistSet* bhs_down_yellow, std::string outfilename) {
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    TGraphErrors* gr_blue = AmplitudeBinnedGraph(type, title, xlabel, bhs_up_blue, bhs_down_blue);
    gr_blue->SetMarkerColor(kBlue);
    gr_blue->SetMarkerStyle(kCircle);
    gr_blue->SetLineColor(kBlue);
    gr_blue->GetYaxis()->SetTitle("Raw Asymmetry");

    TGraphErrors* gr_yellow = AmplitudeBinnedGraph(type, title, xlabel, bhs_up_yellow, bhs_down_yellow);
    int npoints = gr_yellow->GetN();
    double offset = (bhs_up_blue->bin_edges[1] - bhs_up_blue->bin_edges[0])/20.0;
    for (int i=0; i<npoints; i++) {
	gr_yellow->SetPointX(i, gr_yellow->GetPointX(i) + offset);
    }
    gr_yellow->SetMarkerColor(kOrange+1);
    gr_yellow->SetMarkerStyle(kStar);
    gr_yellow->SetLineColor(kOrange+1);

    double xmin = std::min(gr_blue->GetXaxis()->GetXmin(), gr_yellow->GetXaxis()->GetXmin());
    double xmax = std::max(gr_blue->GetXaxis()->GetXmax(), gr_yellow->GetXaxis()->GetXmax());
    double ymin = std::min(gr_blue->GetYaxis()->GetXmin(), gr_yellow->GetYaxis()->GetXmin());
    double ymax = std::max(gr_blue->GetYaxis()->GetXmax(), gr_yellow->GetYaxis()->GetXmax());

    TH1F* hdummy = c1->DrawFrame(xmin, ymin, xmax, ymax);
    hdummy->SetTitle(title.c_str());
    hdummy->GetXaxis()->SetTitle(xlabel.c_str());
    hdummy->GetYaxis()->SetTitle("Raw Asymmetry");
    hdummy->GetXaxis()->CenterTitle(true);
    hdummy->GetYaxis()->CenterTitle(true);
    gr_blue->Draw("p");
    /* gr_rl->GetYaxis()->SetLimits(ymin, ymax); */
    /* c1->RangeAxis(xmin, ymin, xmax, ymax); */
    gr_yellow->Draw("p same");

    TLine* line = new TLine();
    line->SetLineColor(kBlack);
    line->DrawLine(xmin, 0, xmax, 0);

    TLegend leg(0.75, 0.75, 0.9, 0.9, "", "nbNDC");
    leg.AddEntry(gr_blue, "Blue beam", "ep");
    leg.AddEntry(gr_yellow, "Yellow beam", "ep");
    leg.Draw();

    c1->SaveAs(outfilename.c_str());
}

std::pair<double, double> TSSAplotter::GetCorrectedAsymErr(double A_sig, double A_sig_err, double A_bkgr, double A_bkgr_err, double r, double r_err, double pol) {
    double asym = (1.0/pol)*(A_sig - r*A_bkgr)/(1.0 - r);
    double err1 = A_sig_err*A_sig_err;
    double err2 = A_bkgr_err*A_bkgr_err;
    double err3 = ((A_sig - A_bkgr)*(A_sig - A_bkgr))/((1-r)*(1-r));
    double err = (1.0/pol) * (1/(1-r)) * sqrt( err1 + (r*r*err2) + (r_err*r_err*err3) );
    std::pair<double, double> ret;
    ret.first = asym;
    ret.second = err;
    return ret;
}

TGraphErrors* TSSAplotter::CorrectedAmplitudesGraph(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up_sig, BinnedHistSet* bhs_down_sig, BinnedHistSet* bhs_up_bkgr, BinnedHistSet* bhs_down_bkgr) {
    TGraphErrors* gr_sig = AmplitudeBinnedGraph(type, title, xlabel, bhs_up_sig, bhs_down_sig);
    TGraphErrors* gr_bkgr = AmplitudeBinnedGraph(type, title, xlabel, bhs_up_bkgr, bhs_down_bkgr);

    int npoints = gr_sig->GetN();
    TGraphErrors* gr_corrected = new TGraphErrors(npoints);
    gr_corrected->SetTitle(title.c_str());
    gr_corrected->GetXaxis()->SetTitle(xlabel.c_str());
    gr_corrected->GetXaxis()->CenterTitle(1);
    std::string ylabel;
    if (type == "sqrt") ylabel = "Corrected Asymmetry (Square Root)";
    else ylabel = "Corrected Asymmetry (Rel. Lumi.)";
    gr_corrected->GetYaxis()->SetTitle(ylabel.c_str());
    gr_corrected->GetYaxis()->CenterTitle(1);

    double r, r_err, pol;
    bool blue = (title.find("Blue") != std::string::npos);
    if (blue) {
	pol = polBlue;
    }
    else {
	pol = polYellow;
    }
    bool pi0 = (title.find("pi") != std::string::npos);

    for (int i=0; i<npoints; i++) {
	if (pi0) {
	    r = pi0_bkgr_fractions[i];
	    r_err = pi0_bkgr_fraction_errs[i];
	}
	else {
	    r = eta_bkgr_fractions[i];
	    r_err =eta_bkgr_fraction_errs[i];
	}
	std::pair<double, double> asym_err = GetCorrectedAsymErr(gr_sig->GetPointY(i), gr_sig->GetErrorY(i), gr_bkgr->GetPointY(i), gr_bkgr->GetErrorY(i), r, r_err, pol);
	gr_corrected->SetPoint(i, gr_sig->GetPointX(i), asym_err.first);
	gr_corrected->SetPointError(i, gr_sig->GetErrorX(i), asym_err.second);
    }

    return gr_corrected;
}

void TSSAplotter::PlotCorrectedAmplitudes(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up_sig, BinnedHistSet* bhs_down_sig, BinnedHistSet* bhs_up_bkgr, BinnedHistSet* bhs_down_bkgr, std::string outfilename) {
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    TGraphErrors* gr = CorrectedAmplitudesGraph(type, title, xlabel, bhs_up_sig, bhs_down_sig, bhs_up_bkgr, bhs_down_bkgr);
    gr->SetMarkerColor(kBlack);
    gr->SetMarkerStyle(kCircle);
    gr->SetLineColor(kBlack);
    gr->Draw("ap");
    TLine* line = new TLine();
    double xmin = gr->GetXaxis()->GetXmin();
    double xmax = gr->GetXaxis()->GetXmax();
    line->SetLineColor(kBlack);
    line->DrawLine(xmin, 0, xmax, 0);
    c1->SaveAs(outfilename.c_str());
}

TGraphErrors* TSSAplotter::AveragedGraphs(TGraphErrors* gr_blue, TGraphErrors* gr_yellow) {
    int npoints = gr_blue->GetN();
    TGraphErrors* gr_averaged = new TGraphErrors(npoints);
    for (int i=0; i<npoints; i++) {
	double x = gr_blue->GetPointX(i);
	double y_blue = gr_blue->GetPointY(i);
	double y_yellow = gr_yellow->GetPointY(i);
	double y = (y_blue + y_yellow)/2.0;
	double yerr_blue = gr_blue->GetErrorY(i);
	double yerr_yellow = gr_yellow->GetErrorY(i);
	double yerr = 0.5*sqrt( (yerr_blue*yerr_blue) + (yerr_yellow*yerr_yellow) );
	gr_averaged->SetPoint(i, x, y);
	gr_averaged->SetPointError(i, 0, yerr);
    }
    return gr_averaged;
}

void TSSAplotter::PlotAveragedAmplitudes(std::string type, std::string title, std::string xlabel, std::string outfilename, BinnedHistSet* bhs_up_sig_blue, BinnedHistSet* bhs_down_sig_blue, BinnedHistSet* bhs_up_bkgr_blue, BinnedHistSet* bhs_down_bkgr_blue, BinnedHistSet* bhs_up_sig_yellow, BinnedHistSet* bhs_down_sig_yellow, BinnedHistSet* bhs_up_bkgr_yellow, BinnedHistSet* bhs_down_bkgr_yellow) {
    TGraphErrors* gr_blue = CorrectedAmplitudesGraph(type, "Blue", xlabel, bhs_up_sig_blue, bhs_down_sig_blue, bhs_up_bkgr_blue, bhs_down_bkgr_blue);
    TGraphErrors* gr_yellow = CorrectedAmplitudesGraph(type, "Yellow", xlabel, bhs_up_sig_yellow, bhs_down_sig_yellow, bhs_up_bkgr_yellow, bhs_down_bkgr_yellow);
    TGraphErrors* gr_averaged = AveragedGraphs(gr_blue, gr_yellow);
    PlotGraph(gr_averaged, title, xlabel, "Corrected A_{N}", outfilename);
}

void TSSAplotter::PlotGraph(TGraphErrors* gr, std::string title, std::string xlabel, std::string ylabel, std::string outfilename, bool inset, bool ylines) {
    gr->SetTitle(title.c_str());
    gr->GetXaxis()->SetTitle(xlabel.c_str());
    gr->GetXaxis()->CenterTitle(true);
    gr->GetYaxis()->SetTitle(ylabel.c_str());
    gr->GetYaxis()->CenterTitle(true);
    gr->SetMarkerStyle(kCircle);
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    gr->Draw("ap");
    TLine* line = new TLine();
    double xmin = gr->GetXaxis()->GetXmin();
    double xmax = gr->GetXaxis()->GetXmax();
    line->SetLineColor(kBlack);
    line->DrawLine(xmin, 0, xmax, 0);
    if (ylines) {
	TLine* line2 = new TLine();
	line2->SetLineStyle(kDashed);
	line2->DrawLine(xmin, -1.0, xmax, -1.0);
	line2->DrawLine(xmin, 1.0, xmax, 1.0);
    }

    if (inset) {
	TGraphErrors* gr_inset = new TGraphErrors(5);
	for (int i=0; i<5; i++) {
	    double x = gr->GetPointX(i);
	    double y = gr->GetPointY(i);
	    double yerr = gr->GetErrorY(i);
	    gr_inset->SetPoint(i, x, y);
	    gr_inset->SetPointError(i, 0.0, yerr);
	}
	gr_inset->SetMarkerStyle(kCircle);
	gr_inset->SetTitle("");
	TPad* pad = new TPad("pad", "pad", 0.2, 0.5, 0.6, 0.7, -1, 0);
	pad->Draw();
	pad->cd();
	double xmin_inset = gr_inset->GetXaxis()->GetXmin();
	double xmax_inset = gr_inset->GetXaxis()->GetXmax();
	double ymin_inset = -0.01;
	double ymax_inset = 0.01;
	TH1F* h_frame = pad->DrawFrame(xmin_inset, ymin_inset, xmax_inset, ymax_inset);
	h_frame->GetXaxis()->SetLabelSize(0.15);
	/* h_frame->GetXaxis()->SetTickLength(0.5); */
	h_frame->GetYaxis()->SetLabelSize(0.15);
	h_frame->GetYaxis()->SetNdivisions(-504);
	gr_inset->Draw("p");
	line->DrawLine(xmin_inset, 0, xmax_inset, 0);
	pad->Modified();
    }
    c1->Modified();
    c1->SaveAs(outfilename.c_str());
}

void TSSAplotter::CompareGraphs(TGraphErrors* gr1, TGraphErrors* gr2, std::string title, std::string xlabel, std::string ylabel, std::string leglabel1, std::string leglabel2, std::string outfilename, bool inset) {
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    gr1->SetMarkerColor(kRed);
    gr1->SetMarkerStyle(kStar);
    gr1->SetLineColor(kRed);
    /* gr1->GetYaxis()->SetTitle("Raw Asymmetry"); */

    int npoints = gr2->GetN();
    double offset = (gr2->GetPointX(1) - gr2->GetPointX(0))/20.0;
    for (int i=0; i<npoints; i++) {
	gr2->SetPointX(i, gr2->GetPointX(i) + offset);
    }
    gr2->SetMarkerColor(kBlue);
    gr2->SetMarkerStyle(kCircle);
    gr2->SetLineColor(kBlue);

    double xmin = std::min(gr1->GetXaxis()->GetXmin(), gr2->GetXaxis()->GetXmin());
    double xmax = std::max(gr1->GetXaxis()->GetXmax(), gr2->GetXaxis()->GetXmax());
    double ymin = std::min(gr1->GetYaxis()->GetXmin(), gr2->GetYaxis()->GetXmin());
    double ymax = std::max(gr1->GetYaxis()->GetXmax(), gr2->GetYaxis()->GetXmax());
    TH1F* hdummy = c1->DrawFrame(xmin, ymin, xmax, ymax);
    hdummy->SetTitle(title.c_str());
    hdummy->GetXaxis()->SetTitle(xlabel.c_str());
    hdummy->GetYaxis()->SetTitle(ylabel.c_str());
    hdummy->GetXaxis()->CenterTitle(true);
    hdummy->GetYaxis()->CenterTitle(true);
    gr1->Draw("p");
    gr2->Draw("p same");

    TLine* line = new TLine();
    line->SetLineColor(kBlack);
    line->DrawLine(xmin, 0, xmax, 0);

    TLegend leg(0.75, 0.75, 0.9, 0.9, "", "nbNDC");
    leg.AddEntry(gr1, leglabel1.c_str(), "ep");
    leg.AddEntry(gr2, leglabel2.c_str(), "ep");
    leg.Draw();

    if (inset) {
	TGraphErrors* gr1_inset = new TGraphErrors(5);
	TGraphErrors* gr2_inset = new TGraphErrors(5);
	/* TGraphErrors* gr2_inset = (TGraphErrors*)gr2->Clone("gr2_inset"); */
	for (int i=0; i<5; i++) {
	    double x1 = gr1->GetPointX(i);
	    double y1 = gr1->GetPointY(i);
	    double y1err = gr1->GetErrorY(i);
	    gr1_inset->SetPoint(i, x1, y1);
	    gr1_inset->SetPointError(i, 0.0, y1err);
	    double x2 = gr2->GetPointX(i);
	    double y2 = gr2->GetPointY(i);
	    double x2err = gr2->GetErrorX(i);
	    double y2err = gr2->GetErrorY(i);
	    gr2_inset->SetPoint(i, x2, y2);
	    gr2_inset->SetPointError(i, x2err, y2err);
	}
	gr1_inset->SetMarkerColor(kRed);
	gr1_inset->SetMarkerStyle(kStar);
	gr1_inset->SetLineColor(kRed);
	gr2_inset->SetMarkerColor(kBlue);
	gr2_inset->SetMarkerStyle(kCircle);
	gr2_inset->SetLineColor(kBlue);

	TPad* pad = new TPad("pad", "pad", 0.2, 0.5, 0.6, 0.7, -1, 0);
	pad->Draw();
	pad->cd();
	double xmin_inset = gr1_inset->GetXaxis()->GetXmin();
	double xmax_inset = gr1_inset->GetXaxis()->GetXmax();
	double ymin_inset = -0.01;
	double ymax_inset = 0.01;
	TH1F* h_frame = pad->DrawFrame(xmin_inset, ymin_inset, xmax_inset, ymax_inset);
	h_frame->GetXaxis()->SetLabelSize(0.15);
	/* h_frame->GetXaxis()->SetTickLength(0.5); */
	h_frame->GetYaxis()->SetLabelSize(0.15);
	h_frame->GetYaxis()->SetNdivisions(-504);
	gr1_inset->Draw("p");
	gr2_inset->Draw("p same");
	line->DrawLine(xmin_inset, 0, xmax_inset, 0);
	pad->Modified();
    }
    c1->SaveAs(outfilename.c_str());
    // revert gr2 x values to original
    for (int i=0; i<npoints; i++) {
	gr2->SetPointX(i, gr2->GetPointX(i) - offset);
    }
}

void TSSAplotter::CompareGraphs(TGraphErrors* gr1, TGraphAsymmErrors* gr2, std::string title, std::string xlabel, std::string ylabel, std::string leglabel1, std::string leglabel2, std::string outfilename, bool inset) {
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    gr1->SetMarkerColor(kRed);
    gr1->SetMarkerStyle(kStar);
    gr1->SetLineColor(kRed);
    /* gr1->GetYaxis()->SetTitle("Raw Asymmetry"); */

    int npoints = gr2->GetN();
    double offset = (gr2->GetPointX(1) - gr2->GetPointX(0))/20.0;
    for (int i=0; i<npoints; i++) {
	gr2->SetPointX(i, gr2->GetPointX(i) + offset);
    }
    gr2->SetMarkerColor(kBlue);
    gr2->SetMarkerStyle(kCircle);
    gr2->SetLineColor(kBlue);

    double xmin = std::min(gr1->GetXaxis()->GetXmin(), gr2->GetXaxis()->GetXmin());
    double xmax = std::max(gr1->GetXaxis()->GetXmax(), gr2->GetXaxis()->GetXmax());
    double ymin = std::min(gr1->GetYaxis()->GetXmin(), gr2->GetYaxis()->GetXmin());
    double ymax = std::max(gr1->GetYaxis()->GetXmax(), gr2->GetYaxis()->GetXmax());
    ymax *= 2.0;
    TH1F* hdummy = c1->DrawFrame(xmin, ymin, xmax, ymax);
    hdummy->SetTitle(title.c_str());
    hdummy->GetXaxis()->SetTitle(xlabel.c_str());
    hdummy->GetYaxis()->SetTitle(ylabel.c_str());
    hdummy->GetXaxis()->CenterTitle(true);
    hdummy->GetYaxis()->CenterTitle(true);
    gr1->Draw("p");
    gr2->Draw("p same");

    TLine* line = new TLine();
    line->SetLineColor(kBlack);
    line->DrawLine(xmin, 0, xmax, 0);

    TLegend leg(0.75, 0.75, 0.9, 0.9, "", "nbNDC");
    leg.AddEntry(gr1, leglabel1.c_str(), "ep");
    leg.AddEntry(gr2, leglabel2.c_str(), "ep");
    leg.Draw();

    if (inset) {
	TGraphErrors* gr1_inset = new TGraphErrors(5);
	TGraphErrors* gr2_inset = new TGraphErrors(5);
	/* TGraphErrors* gr2_inset = (TGraphErrors*)gr2->Clone("gr2_inset"); */
	for (int i=0; i<5; i++) {
	    double x = gr1->GetPointX(i);
	    double y1 = gr1->GetPointY(i);
	    double y1err = gr1->GetErrorY(i);
	    gr1_inset->SetPoint(i, x, y1);
	    gr1_inset->SetPointError(i, 0.0, y1err);
	    double x2 = gr2->GetPointX(i);
	    double x2err = gr2->GetErrorX(i);
	    double y2 = gr2->GetPointY(i);
	    double y2err = gr2->GetErrorY(i);
	    gr2_inset->SetPoint(i, x2, y2);
	    gr2_inset->SetPointError(i, x2err, y2err);
	}
	gr1_inset->SetMarkerColor(kRed);
	gr1_inset->SetMarkerStyle(kStar);
	gr1_inset->SetLineColor(kRed);
	gr2_inset->SetMarkerColor(kBlue);
	gr2_inset->SetMarkerStyle(kCircle);
	gr2_inset->SetLineColor(kBlue);

	TPad* pad = new TPad("pad", "pad", 0.2, 0.5, 0.6, 0.75, -1, 0);
	pad->Draw();
	pad->cd();
	double xmin_inset = gr1_inset->GetXaxis()->GetXmin();
	double xmax_inset = gr1_inset->GetXaxis()->GetXmax();
	double ymin_inset = -0.005;
	double ymax_inset = 0.005;
	TH1F* h_frame = pad->DrawFrame(xmin_inset, ymin_inset, xmax_inset, ymax_inset);
	h_frame->GetXaxis()->SetLabelSize(0.15);
	/* h_frame->GetXaxis()->SetTickLength(0.5); */
	h_frame->GetYaxis()->SetLabelSize(0.15);
	h_frame->GetYaxis()->SetNdivisions(-504);
	gr1_inset->Draw("p");
	gr2_inset->Draw("p same");
	line->DrawLine(xmin_inset, 0, xmax_inset, 0);
	pad->Modified();
    }
    c1->SaveAs(outfilename.c_str());
    // revert gr2 x values to original
    for (int i=0; i<npoints; i++) {
	gr2->SetPointX(i, gr2->GetPointX(i) - offset);
    }
}

TGraphErrors* TSSAplotter::PolCorrectedGraph(TGraphErrors* gr, double pol) {
    int npoints = gr->GetN();
    TGraphErrors* out = new TGraphErrors(npoints);
    for (int i=0; i<npoints; i++) {
	double x = gr->GetPointX(i);
	double y = gr->GetPointY(i);
	double yerr = gr->GetErrorY(i);
	out->SetPoint(i, x, y/pol);
	out->SetPointError(i, 0, yerr/pol);
    }
    return out;
}

void TSSAplotter::PlotBackgroundFractions(std::string outfilename) {
    int nbins = pi0_bkgr_fractions.size();
    TGraphErrors* gr_pi0_frac = new TGraphErrors(nbins - 1);
    TGraphErrors* gr_eta_frac = new TGraphErrors(nbins);
    for (int i=0; i<nbins; i++) {
	double x_pi0 = (pTbins_pi0[i+1] + pTbins_pi0[i])/2.0;
	double x_eta = (pTbins_eta[i+1] + pTbins_eta[i])/2.0;
	if (i < (nbins-1)) {
	    gr_pi0_frac->SetPoint(i, x_pi0, pi0_bkgr_fractions[i]);
	    gr_pi0_frac->SetPointError(i, 0, pi0_bkgr_fraction_errs[i]);
	}
	gr_eta_frac->SetPoint(i, x_eta, eta_bkgr_fractions[i]);
	gr_eta_frac->SetPointError(i, 0, eta_bkgr_fraction_errs[i]);
    }
    CompareGraphs(gr_pi0_frac, gr_eta_frac, "#pi^{0} and #eta Background Fractions", "p_{T} (GeV)", "Background Fraction", "#pi^{0}", "#eta", outfilename);
}

TGraphErrors* TSSAplotter::tTestGraph(std::string type, TGraphErrors* gr1, TGraphErrors* gr2) {
    int npoints = gr1->GetN();
    TGraphErrors* gr_t = new TGraphErrors(npoints);
    for (int i=0; i<npoints; i++) {
	double x = gr1->GetPointX(i);
	double y1 = gr1->GetPointY(i);
	double y2 = gr2->GetPointY(i);
	double y1err = gr1->GetErrorY(i);
	double y2err = gr2->GetErrorY(i);
	double num = y1 - y2;
	double denom = 1.0;
	if (type == "indep") {
	    denom = sqrt(y1err*y1err + y2err*y2err);
	}
	else if (type == "dep") {
	    denom = sqrt(abs(y1err*y1err - y2err*y2err));
	}
	else {
	    std::cout << "In TSSAplotter::tTestGraph: invalid type " << type << "; returning nullptr" << std::endl;
	    return nullptr;
	}
	double t = num/denom;
	gr_t->SetPoint(i, x, t);
    }
    return gr_t;
}

TGraphErrors* TSSAplotter::tTestZeroGraph(TGraphErrors* gr1) {
    int npoints = gr1->GetN();
    TGraphErrors* gr_t = new TGraphErrors(npoints);
    for (int i=0; i<npoints; i++) {
	double x = gr1->GetPointX(i);
	double y1 = gr1->GetPointY(i);
	double y1err = gr1->GetErrorY(i);
	double t = y1/y1err;
	gr_t->SetPoint(i, x, t);
    }
    return gr_t;
}

void TSSAplotter::PlottTest(TGraphErrors* gr, std::string title, std::string xlabel, std::string outfilename, bool limit_yrange) {
    gr->SetTitle(title.c_str());
    gr->GetXaxis()->SetTitle(xlabel.c_str());
    gr->GetXaxis()->CenterTitle(true);
    gr->GetYaxis()->SetTitle("t Statistic");
    gr->GetYaxis()->CenterTitle(true);
    if (limit_yrange) gr->GetYaxis()->SetRangeUser(-2.5, 2.5);
    gr->SetMarkerStyle(kCircle);
    TCanvas* c1 = new TCanvas(title.c_str(), "", 1600, 900);
    c1->cd();
    gr->Draw("ap");
    TLine* line = new TLine();
    double xmin = gr->GetXaxis()->GetXmin();
    double xmax = gr->GetXaxis()->GetXmax();
    line->SetLineColor(kBlack);
    line->DrawLine(xmin, 0, xmax, 0);
    TLine* line2 = new TLine();
    line2->SetLineStyle(kDashed);
    line2->DrawLine(xmin, -1.0, xmax, -1.0);
    line2->DrawLine(xmin, 1.0, xmax, 1.0);
    c1->SaveAs(outfilename.c_str());
}

void TSSAplotter::PlotPi0(std::string outfilename) {
    // Phi-dependent plots and fitting
    PlotAsymsBinned("#pi^{0} Blue Beam Forward-Going", bhs_pi0_blue_up_forward_phi_pT, bhs_pi0_blue_down_forward_phi_pT, outfilename);
    /* PlotAsymsBinned("#pi^{0} Blue Beam Backward-Going", bhs_pi0_blue_up_backward_phi_pT, bhs_pi0_blue_down_backward_phi_pT, outfilename); */
    PlotAsymsBinned("#pi^{0} Yellow Beam Forward-Going", bhs_pi0_yellow_up_forward_phi_pT, bhs_pi0_yellow_down_forward_phi_pT, outfilename);
    /* PlotAsymsBinned("#pi^{0} Yellow Beam Backward-Going", bhs_pi0_yellow_up_backward_phi_pT, bhs_pi0_yellow_down_backward_phi_pT, outfilename); */

    // Blue beam
    TGraphErrors* gr_rellumi_raw_forward_blue = AmplitudeBinnedGraph("rellumi", "#pi^{0} Blue Beam Forward-Going Asymmetry", "pT_pi0", bhs_pi0_blue_up_forward_phi_pT, bhs_pi0_blue_down_forward_phi_pT);
    TGraphErrors* gr_sqrt_raw_forward_blue = AmplitudeBinnedGraph("sqrt", "#pi^{0} Blue Beam Forward-Going Asymmetry", "pT_pi0", bhs_pi0_blue_up_forward_phi_pT, bhs_pi0_blue_down_forward_phi_pT);
    TGraphErrors* gr_rellumi_raw_backward_blue = AmplitudeBinnedGraph("rellumi", "#pi^{0} Blue Beam Backward-Going Asymmetry", "pT_pi0", bhs_pi0_blue_up_backward_phi_pT, bhs_pi0_blue_down_backward_phi_pT);
    TGraphErrors* gr_sqrt_raw_backward_blue = AmplitudeBinnedGraph("sqrt", "#pi^{0} Blue Beam Backward-Going Asymmetry", "pT_pi0", bhs_pi0_blue_up_backward_phi_pT, bhs_pi0_blue_down_backward_phi_pT);
    TGraphErrors* gr_t_method_blue = tTestGraph("dep", gr_rellumi_raw_forward_blue, gr_sqrt_raw_forward_blue);
    CompareGraphs(gr_rellumi_raw_forward_blue, gr_sqrt_raw_forward_blue, "#pi^{0} Blue Beam Forward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry", "Rel. Lumi.", "Geometric", outfilename);
    PlottTest(gr_t_method_blue, "#pi^{0} Blue Beam Forward-Going Raw Asymmetry, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_rellumi_raw_backward_blue, gr_sqrt_raw_backward_blue, "#pi^{0} Blue Beam Backward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry", "Rel. Lumi.", "Geometric", outfilename);
    
    // Yellow beam
    TGraphErrors* gr_rellumi_raw_forward_yellow = AmplitudeBinnedGraph("rellumi", "#pi^{0} Yellow Beam Forward-Going Asymmetry", "pT_pi0", bhs_pi0_yellow_up_forward_phi_pT, bhs_pi0_yellow_down_forward_phi_pT);
    TGraphErrors* gr_sqrt_raw_forward_yellow = AmplitudeBinnedGraph("sqrt", "#pi^{0} Yellow Beam Forward-Going Asymmetry", "pT_pi0", bhs_pi0_yellow_up_forward_phi_pT, bhs_pi0_yellow_down_forward_phi_pT);
    TGraphErrors* gr_rellumi_raw_backward_yellow = AmplitudeBinnedGraph("rellumi", "#pi^{0} Yellow Beam Backward-Going Asymmetry", "pT_pi0", bhs_pi0_yellow_up_backward_phi_pT, bhs_pi0_yellow_down_backward_phi_pT);
    TGraphErrors* gr_sqrt_raw_backward_yellow = AmplitudeBinnedGraph("sqrt", "#pi^{0} Yellow Beam Backward-Going Asymmetry", "pT_pi0", bhs_pi0_yellow_up_backward_phi_pT, bhs_pi0_yellow_down_backward_phi_pT);
    TGraphErrors* gr_t_method_yellow = tTestGraph("dep", gr_rellumi_raw_forward_yellow, gr_sqrt_raw_forward_yellow);
    CompareGraphs(gr_rellumi_raw_forward_yellow, gr_sqrt_raw_forward_yellow, "#pi^{0} Yellow Beam Forward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry", "Rel. Lumi.", "Geometric", outfilename);
    PlottTest(gr_t_method_yellow, "#pi^{0} Yellow Beam Forward-Going Raw Asymmetry, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_rellumi_raw_backward_yellow, gr_sqrt_raw_backward_yellow, "#pi^{0} Yellow Beam Backward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry", "Rel. Lumi.", "Geometric", outfilename);
    CompareGraphs(gr_rellumi_raw_forward_blue, gr_rellumi_raw_forward_yellow, "#pi^{0} Forward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry (Rel. Lumi.)", "Blue", "Yellow", outfilename);
    CompareGraphs(gr_sqrt_raw_forward_blue, gr_sqrt_raw_forward_yellow, "#pi^{0} Forward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry (Geometric)", "Blue", "Yellow", outfilename);
    TGraphErrors* gr_t_beam_rellumi = tTestGraph("indep", gr_rellumi_raw_forward_blue, gr_rellumi_raw_forward_yellow);
    PlottTest(gr_t_beam_rellumi, "#pi^{0} Forward-Going Raw Rel. Lumi. Asymmetry, t-Test Blue vs Yellow Beam", "p_{T} (GeV)", outfilename);
    TGraphErrors* gr_t_beam_sqrt = tTestGraph("indep", gr_sqrt_raw_forward_blue, gr_sqrt_raw_forward_yellow);
    PlottTest(gr_t_beam_sqrt, "#pi^{0} Forward-Going Raw Geometric Asymmetry, t-Test Blue vs Yellow Beam", "p_{T} (GeV)", outfilename);
    
    // Background
    TGraphErrors* gr_rellumi_raw_forward_blue_bkgr = AmplitudeBinnedGraph("rellumi", "#pi^{0} Blue Beam Forward-Going Asymmetry", "pT_pi0", bhs_pi0bkgr_blue_up_forward_phi_pT, bhs_pi0bkgr_blue_down_forward_phi_pT);
    TGraphErrors* gr_sqrt_raw_forward_blue_bkgr = AmplitudeBinnedGraph("sqrt", "#pi^{0} Blue Beam Forward-Going Asymmetry", "pT_pi0", bhs_pi0bkgr_blue_up_forward_phi_pT, bhs_pi0bkgr_blue_down_forward_phi_pT);
    TGraphErrors* gr_t_rellumi_blue_bkgr = tTestZeroGraph(gr_rellumi_raw_forward_blue_bkgr);
    TGraphErrors* gr_t_sqrt_blue_bkgr = tTestZeroGraph(gr_sqrt_raw_forward_blue_bkgr);
    PlottTest(gr_t_rellumi_blue_bkgr, "#pi^{0} Background Rel. Lumi. Asymmetry, t-Test with Zero", "p_{T} (GeV)", outfilename);
    PlottTest(gr_t_sqrt_blue_bkgr, "#pi^{0} Background Geometric Asymmetry, t-Test with Zero", "p_{T} (GeV)", outfilename);

    // Corrected asymmetries
    TGraphErrors* gr_corr_blue_rellumi = CorrectedAmplitudesGraph("rellumi", "#pi^{0} Blue Beam Forward-Doing Asymmetry", "pT_pi0", bhs_pi0_blue_up_forward_phi_pT, bhs_pi0_blue_down_forward_phi_pT, bhs_pi0bkgr_blue_up_forward_phi_pT, bhs_pi0bkgr_blue_down_forward_phi_pT);
    TGraphErrors* gr_corr_blue_sqrt = CorrectedAmplitudesGraph("sqrt", "#pi^{0} Blue Beam Forward-Doing Asymmetry", "pT_pi0", bhs_pi0_blue_up_forward_phi_pT, bhs_pi0_blue_down_forward_phi_pT, bhs_pi0bkgr_blue_up_forward_phi_pT, bhs_pi0bkgr_blue_down_forward_phi_pT);
    TGraphErrors* gr_corr_yellow_rellumi = CorrectedAmplitudesGraph("rellumi", "#pi^{0} Yellow Beam Forward-Doing Asymmetry", "pT_pi0", bhs_pi0_yellow_up_forward_phi_pT, bhs_pi0_yellow_down_forward_phi_pT, bhs_pi0bkgr_yellow_up_forward_phi_pT, bhs_pi0bkgr_yellow_down_forward_phi_pT);
    TGraphErrors* gr_corr_yellow_sqrt = CorrectedAmplitudesGraph("sqrt", "#pi^{0} Yellow Beam Forward-Doing Asymmetry", "pT_pi0", bhs_pi0_yellow_up_forward_phi_pT, bhs_pi0_yellow_down_forward_phi_pT, bhs_pi0bkgr_yellow_up_forward_phi_pT, bhs_pi0bkgr_yellow_down_forward_phi_pT);
    TGraphErrors* gr_t_corr_method_blue = tTestGraph("dep", gr_corr_blue_rellumi, gr_corr_blue_sqrt);
    TGraphErrors* gr_t_corr_method_yellow = tTestGraph("dep", gr_corr_yellow_rellumi, gr_corr_yellow_sqrt);
    TGraphErrors* gr_t_corr_beam_rellumi = tTestGraph("indep", gr_corr_blue_rellumi, gr_corr_yellow_rellumi);
    TGraphErrors* gr_t_corr_beam_sqrt = tTestGraph("dep", gr_corr_blue_sqrt, gr_corr_yellow_sqrt);
    CompareGraphs(gr_corr_blue_rellumi, gr_corr_blue_sqrt, "#pi^{0} Blue Beam Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N}", "Rel. Lumi.", "Geometric", outfilename);
    PlotGraph(gr_t_corr_method_blue, "#pi^{0} Blue Beam Forward-Going Corrected Asymmetry, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", "t Statistic", outfilename, false, true);
    PlottTest(gr_t_corr_method_blue, "#pi^{0} Blue Beam Forward-Going Corrected Asymmetry, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_corr_yellow_rellumi, gr_corr_yellow_sqrt, "#pi^{0} Yellow Beam Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N}", "Rel. Lumi.", "Geometric", outfilename);
    PlottTest(gr_t_corr_method_yellow, "#pi^{0} Yellow Beam Forward-Going Corrected Asymmetry, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_corr_blue_rellumi, gr_corr_yellow_rellumi, "#pi^{0} Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N} (Rel. Lumi.)", "Blue", "Yellow", outfilename);
    PlottTest(gr_t_corr_beam_rellumi, "#pi^{0} Forward-Going Rel. Lumi. Corrected Asymmetry, t-Test Blue vs Yellow Beam", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_corr_blue_sqrt, gr_corr_yellow_sqrt, "#pi^{0} Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N} (Geometric)", "Blue", "Yellow", outfilename);
    PlottTest(gr_t_corr_beam_sqrt, "#pi^{0} Forward-Going Geometric Corrected Asymmetry, t-Test Blue vs Yellow Beam", "p_{T} (GeV)", outfilename);

    // Final result
    TGraphErrors* gr_final_rellumi = AveragedGraphs(gr_corr_blue_rellumi, gr_corr_yellow_rellumi);
    TGraphErrors* gr_final_sqrt = AveragedGraphs(gr_corr_blue_sqrt, gr_corr_yellow_sqrt);
    TGraphErrors* gr_t_final_method = tTestGraph("dep", gr_final_rellumi, gr_final_sqrt);
    PlotGraph(gr_final_rellumi, "#pi^{0} Forward-Going A_{N} (Rel. Lumi.)", "p_{T} (GeV)", "A_{N}", outfilename, true);
    PlotGraph(gr_final_sqrt, "#pi^{0} Forward-Going A_{N} (Geometric)", "p_{T} (GeV)", "A_{N}", outfilename, true);
    CompareGraphs(gr_final_rellumi, gr_final_sqrt, "#pi^{0} Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N}", "Rel. Lumi.", "Geometric", outfilename, true);
    PlottTest(gr_t_final_method, "#pi^{0} Forward-Going A_{N}, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);

    // PHENIX comparison
    TFile* f_phenix = new TFile("HEPData-ins1833997-v1-root.root", "READ");
    TDirectoryFile* tab1 = (TDirectoryFile*)f_phenix->Get("Table 1");
    TGraphAsymmErrors* gr_phenix = (TGraphAsymmErrors*)tab1->Get("Graph1D_y1");
    CompareGraphs(gr_final_rellumi, gr_phenix, "#pi^{0} Forward-Going Rel. Lumi. Asymmetry, PHENIX vs sPHENIX", "p_{T} (GeV)", "A_{N}", "sPHENIX", "PHENIX", outfilename, true);
    CompareGraphs(gr_final_sqrt, gr_phenix, "#pi^{0} Forward-Going Geometric Asymmetry, PHENIX vs sPHENIX", "p_{T} (GeV)", "A_{N}", "sPHENIX", "PHENIX", outfilename, true);
    // Low vs high eta
    TGraphErrors* gr_corr_blue_rellumi_lowEta = CorrectedAmplitudesGraph("rellumi", "#pi^{0} Blue Beam Forward-Doing Asymmetry", "pT_pi0", bhs_pi0_lowEta_blue_up_forward_phi_pT, bhs_pi0_lowEta_blue_down_forward_phi_pT, bhs_pi0bkgr_lowEta_blue_up_forward_phi_pT, bhs_pi0bkgr_lowEta_blue_down_forward_phi_pT);
    TGraphErrors* gr_corr_blue_sqrt_lowEta = CorrectedAmplitudesGraph("sqrt", "#pi^{0} Blue Beam Forward-Doing Asymmetry", "pT_pi0", bhs_pi0_lowEta_blue_up_forward_phi_pT, bhs_pi0_lowEta_blue_down_forward_phi_pT, bhs_pi0bkgr_lowEta_blue_up_forward_phi_pT, bhs_pi0bkgr_lowEta_blue_down_forward_phi_pT);
    TGraphErrors* gr_corr_yellow_rellumi_lowEta = CorrectedAmplitudesGraph("rellumi", "#pi^{0} Yellow Beam Forward-Doing Asymmetry", "pT_pi0", bhs_pi0_lowEta_yellow_up_forward_phi_pT, bhs_pi0_lowEta_yellow_down_forward_phi_pT, bhs_pi0bkgr_lowEta_yellow_up_forward_phi_pT, bhs_pi0bkgr_lowEta_yellow_down_forward_phi_pT);
    TGraphErrors* gr_corr_yellow_sqrt_lowEta = CorrectedAmplitudesGraph("sqrt", "#pi^{0} Yellow Beam Forward-Doing Asymmetry", "pT_pi0", bhs_pi0_lowEta_yellow_up_forward_phi_pT, bhs_pi0_lowEta_yellow_down_forward_phi_pT, bhs_pi0bkgr_lowEta_yellow_up_forward_phi_pT, bhs_pi0bkgr_lowEta_yellow_down_forward_phi_pT);
    TGraphErrors* gr_final_rellumi_lowEta = AveragedGraphs(gr_corr_blue_rellumi_lowEta, gr_corr_yellow_rellumi_lowEta);
    TGraphErrors* gr_final_sqrt_lowEta = AveragedGraphs(gr_corr_blue_sqrt_lowEta, gr_corr_yellow_sqrt_lowEta);
    CompareGraphs(gr_final_rellumi_lowEta, gr_phenix, "#pi^{0} Forward-Going Rel. Lumi. Asymmetry, PHENIX vs sPHENIX", "p_{T} (GeV)", "A_{N}", "sPHENIX |#eta|<0.35", "PHENIX", outfilename, true);
    CompareGraphs(gr_final_sqrt_lowEta, gr_phenix, "#pi^{0} Forward-Going Geometric Asymmetry, PHENIX vs sPHENIX", "p_{T} (GeV)", "A_{N}", "sPHENIX |#eta|<0.35", "PHENIX", outfilename, true);

    f_phenix->Close();
    delete f_phenix;
}

void TSSAplotter::PlotEta(std::string outfilename) {
    // Phi-dependent plots and fitting
    PlotAsymsBinned("#eta Blue Beam Forward-Going", bhs_eta_blue_up_forward_phi_pT, bhs_eta_blue_down_forward_phi_pT, outfilename);
    /* PlotAsymsBinned("#eta Blue Beam Backward-Going", bhs_eta_blue_up_backward_phi_pT, bhs_eta_blue_down_backward_phi_pT, outfilename); */
    PlotAsymsBinned("#eta Yellow Beam Forward-Going", bhs_eta_yellow_up_forward_phi_pT, bhs_eta_yellow_down_forward_phi_pT, outfilename);
    /* PlotAsymsBinned("#eta Yellow Beam Backward-Going", bhs_eta_yellow_up_backward_phi_pT, bhs_eta_yellow_down_backward_phi_pT, outfilename); */

    // Blue beam
    TGraphErrors* gr_rellumi_raw_forward_blue = AmplitudeBinnedGraph("rellumi", "#eta Blue Beam Forward-Going Asymmetry", "pT_eta", bhs_eta_blue_up_forward_phi_pT, bhs_eta_blue_down_forward_phi_pT);
    TGraphErrors* gr_sqrt_raw_forward_blue = AmplitudeBinnedGraph("sqrt", "#eta Blue Beam Forward-Going Asymmetry", "pT_eta", bhs_eta_blue_up_forward_phi_pT, bhs_eta_blue_down_forward_phi_pT);
    TGraphErrors* gr_rellumi_raw_backward_blue = AmplitudeBinnedGraph("rellumi", "#eta Blue Beam Backward-Going Asymmetry", "pT_eta", bhs_eta_blue_up_backward_phi_pT, bhs_eta_blue_down_backward_phi_pT);
    TGraphErrors* gr_sqrt_raw_backward_blue = AmplitudeBinnedGraph("sqrt", "#eta Blue Beam Backward-Going Asymmetry", "pT_eta", bhs_eta_blue_up_backward_phi_pT, bhs_eta_blue_down_backward_phi_pT);
    TGraphErrors* gr_t_method_blue = tTestGraph("dep", gr_rellumi_raw_forward_blue, gr_sqrt_raw_forward_blue);
    CompareGraphs(gr_rellumi_raw_forward_blue, gr_sqrt_raw_forward_blue, "#eta Blue Beam Forward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry", "Rel. Lumi.", "Geometric", outfilename);
    PlottTest(gr_t_method_blue, "#eta Blue Beam Forward-Going Raw Asymmetry, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_rellumi_raw_backward_blue, gr_sqrt_raw_backward_blue, "#eta Blue Beam Backward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry", "Rel. Lumi.", "Geometric", outfilename);
    
    // Yellow beam
    TGraphErrors* gr_rellumi_raw_forward_yellow = AmplitudeBinnedGraph("rellumi", "#eta Yellow Beam Forward-Going Asymmetry", "pT_eta", bhs_eta_yellow_up_forward_phi_pT, bhs_eta_yellow_down_forward_phi_pT);
    TGraphErrors* gr_sqrt_raw_forward_yellow = AmplitudeBinnedGraph("sqrt", "#eta Yellow Beam Forward-Going Asymmetry", "pT_eta", bhs_eta_yellow_up_forward_phi_pT, bhs_eta_yellow_down_forward_phi_pT);
    TGraphErrors* gr_rellumi_raw_backward_yellow = AmplitudeBinnedGraph("rellumi", "#eta Yellow Beam Backward-Going Asymmetry", "pT_eta", bhs_eta_yellow_up_backward_phi_pT, bhs_eta_yellow_down_backward_phi_pT);
    TGraphErrors* gr_sqrt_raw_backward_yellow = AmplitudeBinnedGraph("sqrt", "#eta Yellow Beam Backward-Going Asymmetry", "pT_eta", bhs_eta_yellow_up_backward_phi_pT, bhs_eta_yellow_down_backward_phi_pT);
    TGraphErrors* gr_t_method_yellow = tTestGraph("dep", gr_rellumi_raw_forward_yellow, gr_sqrt_raw_forward_yellow);
    CompareGraphs(gr_rellumi_raw_forward_yellow, gr_sqrt_raw_forward_yellow, "#eta Yellow Beam Forward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry", "Rel. Lumi.", "Geometric", outfilename);
    PlottTest(gr_t_method_yellow, "#eta Yellow Beam Forward-Going Raw Asymmetry, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_rellumi_raw_backward_yellow, gr_sqrt_raw_backward_yellow, "#eta Yellow Beam Backward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry", "Rel. Lumi.", "Geometric", outfilename);
    CompareGraphs(gr_rellumi_raw_forward_blue, gr_rellumi_raw_forward_yellow, "#eta Forward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry (Rel. Lumi.)", "Blue", "Yellow", outfilename);
    CompareGraphs(gr_sqrt_raw_forward_blue, gr_sqrt_raw_forward_yellow, "#eta Forward-Going Asymmetry", "p_{T} (GeV)", "Raw Asymmetry (Geometric)", "Blue", "Yellow", outfilename);
    TGraphErrors* gr_t_beam_rellumi = tTestGraph("indep", gr_rellumi_raw_forward_blue, gr_rellumi_raw_forward_yellow);
    PlottTest(gr_t_beam_rellumi, "#eta Forward-Going Raw Rel. Lumi. Asymmetry, t-Test Blue vs Yellow Beam", "p_{T} (GeV)", outfilename);
    TGraphErrors* gr_t_beam_sqrt = tTestGraph("indep", gr_sqrt_raw_forward_blue, gr_sqrt_raw_forward_yellow);
    PlottTest(gr_t_beam_sqrt, "#eta Forward-Going Raw Geometric Asymmetry, t-Test Blue vs Yellow Beam", "p_{T} (GeV)", outfilename);
    
    // Background
    TGraphErrors* gr_rellumi_raw_forward_blue_bkgr = AmplitudeBinnedGraph("rellumi", "#eta Blue Beam Forward-Going Asymmetry", "pT_eta", bhs_etabkgr_blue_up_forward_phi_pT, bhs_etabkgr_blue_down_forward_phi_pT);
    TGraphErrors* gr_sqrt_raw_forward_blue_bkgr = AmplitudeBinnedGraph("sqrt", "#eta Blue Beam Forward-Going Asymmetry", "pT_eta", bhs_etabkgr_blue_up_forward_phi_pT, bhs_etabkgr_blue_down_forward_phi_pT);
    TGraphErrors* gr_t_rellumi_blue_bkgr = tTestZeroGraph(gr_rellumi_raw_forward_blue_bkgr);
    TGraphErrors* gr_t_sqrt_blue_bkgr = tTestZeroGraph(gr_sqrt_raw_forward_blue_bkgr);
    PlottTest(gr_t_rellumi_blue_bkgr, "#eta Background Rel. Lumi. Asymmetry, t-Test with Zero", "p_{T} (GeV)", outfilename);
    PlottTest(gr_t_sqrt_blue_bkgr, "#eta Background Geometric Asymmetry, t-Test with Zero", "p_{T} (GeV)", outfilename);

    // Corrected asymmetries
    TGraphErrors* gr_corr_blue_rellumi = CorrectedAmplitudesGraph("rellumi", "#eta Blue Beam Forward-Doing Asymmetry", "pT_eta", bhs_eta_blue_up_forward_phi_pT, bhs_eta_blue_down_forward_phi_pT, bhs_etabkgr_blue_up_forward_phi_pT, bhs_etabkgr_blue_down_forward_phi_pT);
    TGraphErrors* gr_corr_blue_sqrt = CorrectedAmplitudesGraph("sqrt", "#eta Blue Beam Forward-Doing Asymmetry", "pT_eta", bhs_eta_blue_up_forward_phi_pT, bhs_eta_blue_down_forward_phi_pT, bhs_etabkgr_blue_up_forward_phi_pT, bhs_etabkgr_blue_down_forward_phi_pT);
    TGraphErrors* gr_corr_yellow_rellumi = CorrectedAmplitudesGraph("rellumi", "#eta Yellow Beam Forward-Doing Asymmetry", "pT_eta", bhs_eta_yellow_up_forward_phi_pT, bhs_eta_yellow_down_forward_phi_pT, bhs_etabkgr_yellow_up_forward_phi_pT, bhs_etabkgr_yellow_down_forward_phi_pT);
    TGraphErrors* gr_corr_yellow_sqrt = CorrectedAmplitudesGraph("sqrt", "#eta Yellow Beam Forward-Doing Asymmetry", "pT_eta", bhs_eta_yellow_up_forward_phi_pT, bhs_eta_yellow_down_forward_phi_pT, bhs_etabkgr_yellow_up_forward_phi_pT, bhs_etabkgr_yellow_down_forward_phi_pT);
    TGraphErrors* gr_t_corr_method_blue = tTestGraph("dep", gr_corr_blue_rellumi, gr_corr_blue_sqrt);
    TGraphErrors* gr_t_corr_method_yellow = tTestGraph("dep", gr_corr_yellow_rellumi, gr_corr_yellow_sqrt);
    TGraphErrors* gr_t_corr_beam_rellumi = tTestGraph("indep", gr_corr_blue_rellumi, gr_corr_yellow_rellumi);
    TGraphErrors* gr_t_corr_beam_sqrt = tTestGraph("dep", gr_corr_blue_sqrt, gr_corr_yellow_sqrt);
    CompareGraphs(gr_corr_blue_rellumi, gr_corr_blue_sqrt, "#eta Blue Beam Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N}", "Rel. Lumi.", "Geometric", outfilename);
    PlottTest(gr_t_corr_method_blue, "#eta Blue Beam Forward-Going Corrected Asymmetry, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_corr_yellow_rellumi, gr_corr_yellow_sqrt, "#eta Yellow Beam Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N}", "Rel. Lumi.", "Geometric", outfilename);
    PlottTest(gr_t_corr_method_yellow, "#eta Yellow Beam Forward-Going Corrected Asymmetry, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_corr_blue_rellumi, gr_corr_yellow_rellumi, "#eta Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N} (Rel. Lumi.)", "Blue", "Yellow", outfilename);
    PlottTest(gr_t_corr_beam_rellumi, "#eta Forward-Going Rel. Lumi. Corrected Asymmetry, t-Test Blue vs Yellow Beam", "p_{T} (GeV)", outfilename);
    CompareGraphs(gr_corr_blue_sqrt, gr_corr_yellow_sqrt, "#eta Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N} (Geometric)", "Blue", "Yellow", outfilename);
    PlottTest(gr_t_corr_beam_sqrt, "#eta Forward-Going Geometric Corrected Asymmetry, t-Test Blue vs Yellow Beam", "p_{T} (GeV)", outfilename);

    // Final result
    TGraphErrors* gr_final_rellumi = AveragedGraphs(gr_corr_blue_rellumi, gr_corr_yellow_rellumi);
    TGraphErrors* gr_final_sqrt = AveragedGraphs(gr_corr_blue_sqrt, gr_corr_yellow_sqrt);
    TGraphErrors* gr_t_final_method = tTestGraph("dep", gr_final_rellumi, gr_final_sqrt);
    PlotGraph(gr_final_rellumi, "#eta Forward-Going A_{N} (Rel. Lumi.)", "p_{T} (GeV)", "A_{N}", outfilename, true);
    PlotGraph(gr_final_sqrt, "#eta Forward-Going A_{N} (Geometric)", "p_{T} (GeV)", "A_{N}", outfilename, true);
    CompareGraphs(gr_final_rellumi, gr_final_sqrt, "#eta Forward-Going Asymmetry", "p_{T} (GeV)", "A_{N}", "Rel. Lumi.", "Geometric", outfilename, true);
    PlottTest(gr_t_final_method, "#eta Forward-Going A_{N}, t-Test Rel. Lumi. vs Geometric", "p_{T} (GeV)", outfilename);
}

int TSSAplotter::main(std::string TSSApdf, std::string NMpdf) {
    TSSApdfname = TSSApdf;
    if (!NMpdf.empty()) {
	do_NMhists = true;
	NMpdfname = NMpdf;
    }

    if (do_NMhists) {
	GetNMHists();
	PlotNMHists();
    }
    GetTSSAHists();
    PlotTSSAs();
    return 0;
}

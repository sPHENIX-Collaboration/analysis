#include "TSSAplotter.h"
#include <TFile.h>
#include <TF1.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLegendEntry.h>

#include <string>

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

TSSAplotter::TSSAplotter() {}

TSSAplotter::~TSSAplotter() {}

void TSSAplotter::GetHists(std::string infilename) {
    TFile* infile = new TFile(infilename.c_str(), "READ");
    
    nClusters = (TH1*)infile->Get("h_nClusters");
    nGoodClusters = (TH1*)infile->Get("h_nGoodClusters");
    vtxz = (TH1*)infile->Get("h_vtxz");
    clusterE = (TH1*)infile->Get("h_clusterE");
    clusterEta = (TH1*)infile->Get("h_clusterEta");
    clusterEta_vtxz = (TH2*)infile->Get("h_clusterEta_vtxz");
    clusterPhi = (TH1*)infile->Get("h_clusterPhi");
    clusterEta_Phi = (TH2*)infile->Get("h_clusterEta_Phi");
    clusterpT = (TH1*)infile->Get("h_clusterpT");
    clusterxF = (TH1*)infile->Get("h_clusterxF");
    clusterpT_xF = (TH2*)infile->Get("h_clusterpT_xF");
    clusterChi2 = (TH1*)infile->Get("h_clusterChi2");
    clusterChi2zoomed = (TH1*)infile->Get("h_clusterChi2zoomed");
    mesonClusterChi2 = (TH1*)infile->Get("h_mesonClusterChi2");
    goodClusterEta_Phi = (TH2*)infile->Get("h_goodClusterEta_Phi");

    nDiphotons = (TH1*)infile->Get("h_nDiphotons");
    nRecoPi0s = (TH1*)infile->Get("h_nRecoPi0s");
    nRecoEtas = (TH1*)infile->Get("h_nRecoEtas");
    diphoton_mass = (TH1*)infile->Get("h_diphotonMass");
    diphoton_pT = (TH1*)infile->Get("h_diphotonpT");
    diphoton_xF = (TH1*)infile->Get("h_diphotonxF");
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
    bhs_diphotonMass_pT = new BinnedHistSet(diphoton_mass, "p_{T} (GeV)", pTbinsMass);
    bhs_diphotonMass_pT->GetHistsFromFile("h_diphotonMass_pT");
    bhs_diphotonMass_xF = new BinnedHistSet(diphoton_mass, "x_{F}", xFbinsMass);
    bhs_diphotonMass_xF->GetHistsFromFile("h_diphotonMass_xF");

    /*
    std::vector<double> pTbins;
    std::vector<double> xFbins;
    float bhs_max_pT = 8.0;
    float bhs_max_xF = 0.15;
    int nBins_pT = 6;
    int nBins_xF = 6;
    for (int i=0; i<nBins_pT; i++) {
	pTbins.push_back(i*(bhs_max_pT/nBins_pT));
    }
    for (int i=0; i<nBins_xF; i++) {
	xFbins.push_back(2*bhs_max_xF*i/nBins_xF - bhs_max_xF);
    }
    pTbins.push_back(bhs_max_pT);
    xFbins.push_back(bhs_max_xF);
    */
    std::vector<double> pTbins = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0, 20.0};
    std::vector<double> xFbins = {-0.15, -0.10, -0.06, -0.03, 0.03, 0.06, 0.10, 0.15};
    std::vector<double> etabins = {-2.0, -1.15, -0.35, 0.35, 1.15, 2.0};
    std::vector<double> vtxzbins = {-100.0, -50.0, -30.0, 30.0, 50.0, 100.0};
    /* std::vector<double> xFbins = {-0.15, -0.10, -0.06, -0.03, 0.0, 0.03, 0.06, 0.10, 0.15}; */
    /* std::vector<double> etabins = {-2.0, -1.15, -0.35, 0.0, 0.35, 1.15, 2.0}; */
    /* std::vector<double> vtxzbins = {-100.0, -50.0, -30.0, 0.0, 30.0, 50.0, 100.0}; */

    int nHistBins_phi = 16;
    std::string nameprefix = "h_pi0_";
    std::string titlewhich = "#pi^{0}";
    bhs_pi0_blue_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_pi0_blue_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_pi0_yellow_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_pi0_yellow_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_pi0_blue_up_phi_pT->GetHistsFromFile("h_pi0_phi_pT_blue_up");
    bhs_pi0_blue_down_phi_pT->GetHistsFromFile("h_pi0_phi_pT_blue_down");
    bhs_pi0_yellow_up_phi_pT->GetHistsFromFile("h_pi0_phi_pT_yellow_up");
    bhs_pi0_yellow_down_phi_pT->GetHistsFromFile("h_pi0_phi_pT_yellow_down");
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
    bhs_eta_blue_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_eta_blue_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_eta_yellow_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_eta_yellow_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_eta_blue_up_phi_pT->GetHistsFromFile("h_eta_phi_pT_blue_up");
    bhs_eta_blue_down_phi_pT->GetHistsFromFile("h_eta_phi_pT_blue_down");
    bhs_eta_yellow_up_phi_pT->GetHistsFromFile("h_eta_phi_pT_yellow_up");
    bhs_eta_yellow_down_phi_pT->GetHistsFromFile("h_eta_phi_pT_yellow_down");
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
    bhs_pi0bkgr_blue_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_pi0bkgr_blue_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_pi0bkgr_yellow_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_pi0bkgr_yellow_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_pi0bkgr_blue_up_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_blue_up");
    bhs_pi0bkgr_blue_down_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_blue_down");
    bhs_pi0bkgr_yellow_up_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_yellow_up");
    bhs_pi0bkgr_yellow_down_phi_pT->GetHistsFromFile("h_pi0bkgr_phi_pT_yellow_down");
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
    bhs_etabkgr_blue_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_etabkgr_blue_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_etabkgr_yellow_up_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_etabkgr_yellow_down_phi_pT = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    bhs_etabkgr_blue_up_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_blue_up");
    bhs_etabkgr_blue_down_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_blue_down");
    bhs_etabkgr_yellow_up_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_yellow_up");
    bhs_etabkgr_yellow_down_phi_pT->GetHistsFromFile("h_etabkgr_phi_pT_yellow_down");
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
    
    double error = asym*sqrt((num_rel_err*num_rel_err) + (den_rel_err*den_rel_err));
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
    int nbins = (int)(phi_up->GetNbinsX() / 2);
    for (int i = 0; i < nbins; i++)
    {
	float phi = i*(PI/nbins) - (PI/2 - PI/nbins/2);
	int phibinL = i; // N_Left
	int phibinR = (phibinL + (int)(nbins/2.)) % nbins; // N_Right
	
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
	fit->SetParLimits(0, 0.00001, 0.4);
	fit->SetParameter(0, 0.01);
	/* fit->SetParLimits(1, 3.041592, 3.241592); */
	fit->SetParLimits(1, 0.0, 2*PI);
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
    gr->Fit("asymfit", "Q");
    gr->GetXaxis()->SetRangeUser(fit_lower, fit_upper);
    gr->GetYaxis()->SetRangeUser(-0.05, 0.05);
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
    rl->GetYaxis()->SetRangeUser(-0.02, 0.02);
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
    sqrt->GetYaxis()->SetRangeUser(-0.05, 0.05);

    int sqrt_marker = sqrt->GetMarkerStyle();
    int sqrt_mcolor = sqrt->GetMarkerColor();
    int sqrt_lcolor = sqrt->GetLineColor();
    sqrt->SetMarkerStyle(kCircle);
    sqrt->SetMarkerColor(kBlue);
    sqrt->SetLineColor(kBlue);
    std::cout << "sqrt nPoints = " << sqrt->GetN() << std::endl;
    std::cout << "rl nPoints = " << rl->GetN() << std::endl;

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
    TLegend leg(0.75, 0.75, 0.9, 0.9, "", "NB NDC");
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

int TSSAplotter::main(std::string infilename, std::string outfilename) {
    /* std::string outfilename = "NMhists.pdf"; */
    std::string outfilename_start = outfilename + "(";
    std::string outfilename_end = outfilename + ")";

    GetHists(infilename);
    TCanvas* c = new TCanvas("c", "c", 1600, 900);

    PlotSingleHist(nClusters, outfilename_start);
    PlotSingleHist(nGoodClusters, outfilename);
    PlotSingleHist(vtxz, outfilename);
    PlotSingleHist(clusterE, outfilename, true);
    PlotSingleHist(clusterEta, outfilename);
    clusterEta_vtxz->Draw("colz");
    c->SaveAs(outfilename.c_str());
    PlotSingleHist(clusterPhi, outfilename);
    clusterEta_Phi->Draw("colz");
    c->SaveAs(outfilename.c_str());
    PlotSingleHist(clusterpT, outfilename, true);
    PlotSingleHist(clusterxF, outfilename, true);
    clusterpT_xF->Draw("colz");
    c->SetLogy(0);
    c->SetLogz();
    c->SaveAs(outfilename.c_str());
    PlotSingleHist(clusterChi2, outfilename, true);
    PlotSingleHist(clusterChi2zoomed, outfilename, true);
    c->SetLogy();
    normalize_hist(clusterChi2zoomed);
    clusterChi2zoomed->Draw();
    normalize_hist(mesonClusterChi2);
    mesonClusterChi2->Scale(1.0/mesonClusterChi2->Integral());
    mesonClusterChi2->Draw("same");
    c->SaveAs(outfilename.c_str());
    clusterChi2zoomed->Scale(clusterChi2zoomed->GetEntries()/clusterChi2zoomed->Integral());
    mesonClusterChi2->Scale(mesonClusterChi2->GetEntries()/mesonClusterChi2->Integral());
    /* goodClusterEta_Phi->Draw("colz"); */
    /* c->SetLogy(0); */
    /* c->SaveAs(outfilename.c_str()); */

    PlotSingleHist(nDiphotons, outfilename);
    PlotSingleHist(nRecoPi0s, outfilename);
    PlotSingleHist(nRecoEtas, outfilename);
    PlotSingleHist(diphoton_mass, outfilename);
    PlotSingleHist(diphoton_pT, outfilename, true);
    PlotSingleHist(diphoton_xF, outfilename, true);

    bhs_diphotonMass_pT->PlotAllHistsWithFits(outfilename, false, "mass");
    bhs_diphotonMass_xF->PlotAllHistsWithFits(outfilename, false, "mass");

    TGraphErrors* gr_rl = RelLumiGraph(bhs_pi0_blue_up_phi_pT->hist_vec[0], bhs_pi0_blue_down_phi_pT->hist_vec[0], relLumiBlue);
    TGraphErrors* gr_sqrt = SqrtGraph(bhs_pi0_blue_up_phi_pT->hist_vec[0], bhs_pi0_blue_down_phi_pT->hist_vec[0]);
    CompareOneAsym("#pi^{0} Blue Beam Raw Asymmetries", outfilename, gr_rl, gr_sqrt);
    CompareOneAsym("#pi^{0} Blue Beam Raw Asymmetries", outfilename, gr_rl, gr_sqrt);

    PlotAsymsBinned("rellumi", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_pT, bhs_pi0_blue_down_phi_pT, outfilename);
    PlotAsymsBinned("rellumi", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_pT, bhs_pi0_blue_down_phi_pT, outfilename);
    PlotAsymsBinned("rellumi", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_xF, bhs_pi0_blue_down_phi_xF, outfilename);
    PlotAsymsBinned("rellumi", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_eta, bhs_pi0_blue_down_phi_eta, outfilename);
    PlotAsymsBinned("rellumi", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_vtxz, bhs_pi0_blue_down_phi_vtxz, outfilename);
    PlotAsymsBinned("sqrt", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_pT, bhs_pi0_blue_down_phi_pT, outfilename);
    PlotAsymsBinned("sqrt", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_xF, bhs_pi0_blue_down_phi_xF, outfilename);
    PlotAsymsBinned("sqrt", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_eta, bhs_pi0_blue_down_phi_eta, outfilename);
    PlotAsymsBinned("sqrt", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_vtxz, bhs_pi0_blue_down_phi_vtxz, outfilename);

    diphoton_mass->Draw();
    c->SetLogy(0);
    c->SaveAs(outfilename_end.c_str());

    return 0;
}

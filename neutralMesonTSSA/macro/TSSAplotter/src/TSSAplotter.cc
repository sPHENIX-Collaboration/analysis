#include "TSSAplotter.h"
#include <TFile.h>
#include <TF1.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>

TSSAplotter::TSSAplotter() {}

TSSAplotter::~TSSAplotter() {}

void TSSAplotter::GetHists(std::string infilename) {
    TFile* infile = new TFile(infilename.c_str(), "READ");
    
    diphoton_mass = (TH1*)infile->Get("h_diphotonMass");
    diphoton_pT = (TH1*)infile->Get("h_diphotonpT");
    diphoton_xF = (TH1*)infile->Get("h_diphotonxF");
    double pT_upper = 15.0;
    double xF_upper = 0.15;
    std::vector<double> pTbinsMass;
    std::vector<double> xFbinsMass;
    int nbins_bhs = 25;
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

TGraphErrors* TSSAplotter::SqrtGraph(TH1* phi_up, TH1* phi_down) {
    TGraphErrors* gr = new TGraphErrors();
    int nbins = phi_up->GetNbinsX();
    for (int i = 0; i < nbins; i++)
    {
	float phi = i*(2*PI/nbins) - (PI - PI/nbins);
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

    TF1* fit = new TF1("fitsqrt", "[0]*sin(x - [1])", fit_lower, fit_upper);
    fit->SetParLimits(0, 0.0001, 0.4);
    fit->SetParameter(0, 0.01);
    /* fit->SetParLimits(1, 3.041592, 3.241592); */
    fit->SetParLimits(1, 0.0, 2*PI);
    fit->SetParameter(1, 3.141592);
    fit->SetParName(0, "#epsilon");
    fit->SetParName(1, "#phi_{0}");

    gr->SetTitle(title.c_str());
    gr->GetXaxis()->SetTitle("#phi");
    gr->GetYaxis()->SetTitle("Raw Asymmetry");
    gStyle->SetOptFit();
    gStyle->SetOptStat(0);
    gr->Draw("ap");
    gr->Fit("fitsqrt", "Q");
    gr->GetXaxis()->SetRangeUser(-PI/2, PI/2);
    gr->GetYaxis()->SetRangeUser(-0.05, 0.05);
    pad->Update();
}

void TSSAplotter::PlotAsymsBinned(std::string type, std::string which, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down, std::string outfilename) {
    TCanvas* c1 = new TCanvas(which.c_str(), "", 1600, 900);
    c1->Divide(3, 2, 0.025, 0.025);
    
    int nbins = bhs_up->nbins;
    for (int i=1; i<=nbins; i++) {
	TPad* pad = (TPad*)c1->GetPad(i);
	pad->cd();
	gStyle->SetPadLeftMargin(0.15);
	gROOT->ForceStyle();
	TH1* phi_up = bhs_up->hist_vec[i];
	TH1* phi_down = bhs_down->hist_vec[i];
	TGraphErrors* gr = SqrtGraph(phi_up, phi_down);
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

int TSSAplotter::main(std::string infilename) {
    std::string outfilename = "NMhists.pdf";
    std::string outfilename_start = outfilename + "(";
    std::string outfilename_end = outfilename + ")";

    GetHists(infilename);

    TCanvas* c = new TCanvas("c", "c", 1600, 900);
    diphoton_mass->Draw();
    c->SaveAs(outfilename_start.c_str());
    diphoton_pT->Draw();
    c->SetLogy();
    c->SaveAs(outfilename.c_str());
    diphoton_xF->Draw();
    c->SetLogy();
    c->SaveAs(outfilename.c_str());

    bhs_diphotonMass_pT->PlotAllHistsWithFits(outfilename, false, "mass");
    bhs_diphotonMass_xF->PlotAllHistsWithFits(outfilename, false, "mass");

    PlotAsymsBinned("sqrt", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_pT, bhs_pi0_blue_down_phi_pT, outfilename);
    PlotAsymsBinned("sqrt", "#pi^{0} Blue Beam", bhs_pi0_blue_up_phi_xF, bhs_pi0_blue_down_phi_xF, outfilename);

    diphoton_mass->Draw();
    c->SetLogy(0);
    c->SaveAs(outfilename_end.c_str());

    return 0;
}

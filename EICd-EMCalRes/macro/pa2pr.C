#include "macro/globals.h"

int pa2pr(
	const char *filename_pa,
	const char *flavor,
	const char *filename_de_eemc,
	const char *filename_de_cemc,
	const char *filename_de_femc,
	const char *filename_pr)
{
	/// file for making energy resolution plots
	/// from all the crap already computed.
	
	gROOT->SetMacroPath("/sphenix/user/jordan/sphenix-git/macros/macros/g4simulations");
	gROOT->LoadMacro("../sPHENIXStyle/sPhenixStyle.C");
	
	Float_t E_MIN = 0.;
	Float_t E_MAX = 25. * 1.1;
	Float_t DE_OVER_E_MIN = 0.;
	Float_t DE_OVER_E_MAX = .25;
	Int_t N_POINTS = 9;
	
	TF1 *curve_eemc = new TF1("curve_eemc", "sqrt([0]**2+[1]**2/x)", E_MIN, E_MAX);
	TF1 *curve_cemc = new TF1("curve_cemc", "sqrt([0]**2+[1]**2/x)", E_MIN, E_MAX);
	TF1 *curve_femc = new TF1("curve_femc", "sqrt([0]**2+[1]**2/x)", E_MIN, E_MAX);
	Float_t
		err_cnst_eemc, err_sqrt_eemc,
		err_cnst_cemc, err_sqrt_cemc,
		err_cnst_femc, err_sqrt_femc;
	TGraph *points_eemc = new TGraph(N_POINTS);
	TGraph *points_cemc = new TGraph(N_POINTS);
	TGraph *points_femc = new TGraph(N_POINTS);
	
	TFile *file_pa = new TFile(filename_pa, "READ");
	TTree *tree_pa = file_pa->Get("TREE_PA");
	char filename_origin[256];
	Float_t
		par_cnst, err_cnst,
		par_sqrt, err_sqrt;
	tree_pa->SetBranchAddress("filename_origin", filename_origin);
	tree_pa->SetBranchAddress("cnst", &par_cnst);
	tree_pa->SetBranchAddress("d_cnst", &err_cnst);
	tree_pa->SetBranchAddress("sqrt", &par_sqrt);
	tree_pa->SetBranchAddress("d_sqrt", &err_sqrt);
	for (Int_t i_pa = 0; i_pa < tree_pa->GetEntries(); ++i_pa) {
		tree_pa->GetEntry(i_pa);
		TF1 *curve;
		TGraph *points;
		if (strcmp(filename_origin, filename_de_eemc) == 0) {
			curve = curve_eemc;
			points = points_eemc;
			err_cnst_eemc = err_cnst;
			err_sqrt_eemc = err_sqrt;
		}
		if (strcmp(filename_origin, filename_de_cemc) == 0) {
			curve = curve_cemc;
			points = points_cemc;
			err_cnst_cemc = err_cnst;
			err_sqrt_cemc = err_sqrt;
		}
		if (strcmp(filename_origin, filename_de_femc) == 0) {
			curve = curve_femc;
			points = points_femc;
			err_cnst_femc = err_cnst;
			err_sqrt_femc = err_sqrt;
		}
		if (curve != NULL && points != NULL) {
			curve->SetParameter(0, par_cnst);
			curve->SetParameter(1, par_sqrt);
			TFile *file_origin = new TFile(filename_origin, "READ");
			TNtuple *tree_de = file_origin->Get("TREE_DE");
			Float_t de_over_e, e;
			tree_de->SetBranchAddress("de_over_e", &de_over_e);
			tree_de->SetBranchAddress("e", &e);
			for (Int_t i_de = 0; i_de < tree_de->GetEntries(); ++i_de) {
				tree_de->GetEntry(i_de);
				points->SetPoint(i_de, e, de_over_e);
			}
			delete tree_de;
			file_origin->Close();
			delete file_origin;
			curve = NULL;
			points = NULL;
		}
	}
	
	TCanvas c;
	SetsPhenixStyle();
	
	curve_eemc->SetLineColor(2);
	points_eemc->SetMarkerColor(2);
	points_eemc->SetMarkerStyle(20);
	curve_cemc->SetLineColor(3);
	points_cemc->SetMarkerColor(3);
	points_cemc->SetMarkerStyle(21);
	curve_femc->SetLineColor(4);
	points_femc->SetMarkerColor(4);
	points_femc->SetMarkerStyle(22);
	
	TLegend leg(.3, .59, .91, .9);
	leg.SetHeader("#it{#bf{sPHENIX (?)}} Simulation");
	char label_eemc[256], label_cemc[256], label_femc[256];
	snprintf(
		label_eemc,
		sizeof(label_eemc),
		"EEMC %s,   (%.2f#pm%.2f)%% #oplus (%.2f#pm%.2f)%%/#sqrt{E}",
		flavor,
		curve_eemc->GetParameter(0) * 100.,
		err_cnst_eemc * 100.,
		curve_eemc->GetParameter(1) * 100,
		err_sqrt_eemc * 100.);
	snprintf(
		label_cemc,
		sizeof(label_cemc),
		"CEMC %s,   (%.2f#pm%.2f)%% #oplus (%.2f#pm%.2f)%%/#sqrt{E}",
		flavor,
		curve_cemc->GetParameter(0) * 100.,
		err_cnst_cemc * 100.,
		curve_cemc->GetParameter(1) * 100,
		err_sqrt_cemc * 100.);
	snprintf(
		label_femc,
		sizeof(label_femc),
		"FEMC %s,   (%.2f#pm%.2f)%% #oplus (%.2f#pm%.2f)%%/#sqrt{E}",
		flavor,
		curve_femc->GetParameter(0) * 100.,
		err_cnst_femc * 100.,
		curve_femc->GetParameter(1) * 100.,
		err_sqrt_femc * 100.);
	leg.AddEntry(points_eemc, label_eemc, "p");
	leg.AddEntry(points_cemc, label_cemc, "p");
	leg.AddEntry(points_femc, label_femc, "p");
	
	gPad->SetMargin(.16, .05, .16, .05);
	gPad->DrawFrame(E_MIN, DE_OVER_E_MIN, E_MAX, DE_OVER_E_MAX, ";E [GeV];dE/E");
	leg.Draw("same");
	curve_eemc->Draw("same");
	curve_cemc->Draw("same");
	curve_femc->Draw("same");
	points_eemc->Draw("p");
	points_cemc->Draw("p");
	points_femc->Draw("p");
	gPad->Print(filename_pr);
	
	return 0;
}

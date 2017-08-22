#include "macro/globals.h"

int check_pa(
	const char *filename_pa,
	const char *filename_qa)
{
	///--------------
	/// INTRODUCTION
	///--------------
	
	/// this macro is designed to...
	/// 	
	///		see how dE/E curves change with pseudorapidity, flavor, et cetera
	
	/// it takes a TTree file of fit parameters to dE/E curves as input
	
	/// output is nothing, officially.
	/// the quality assurance plot checks how similar all the fits are.
	
	///-----------------------
	/// LOAD LIBRARIES/MACROS
	///-----------------------
	
	gROOT->SetMacroPath("/sphenix/user/jordan/sphenix-git/macros/macros/g4simulations");
	gROOT->LoadMacro("../sPHENIXStyle/sPhenixStyle.C");
	
	///----------------------------------
	/// DUMP THE P.A. FILE INTO A TGRAPH
	///----------------------------------
	
	TFile *file_pa = new TFile(filename_pa, "READ");
	
	TTree *tree_pa = (TTree*)file_pa->Get("TREE_PA");
	Float_t par_cnst,
	        err_cnst,
	        par_sqrt,
	        err_sqrt;
	char origin[256];
	tree_pa->SetBranchAddress("cnst", &par_cnst);
	tree_pa->SetBranchAddress("d_cnst", &err_cnst);
	tree_pa->SetBranchAddress("sqrt", &par_sqrt);
	tree_pa->SetBranchAddress("d_sqrt", &err_sqrt);
	tree_pa->SetBranchAddress("filename_origin", origin);
	
	Long64_t npoints = tree_pa->GetEntries();
	TGraphErrors cnst_against_sqrt(npoints);
	for (Long64_t i = 0; i < npoints; ++i) {
		tree_pa->GetEntry(i);
		cnst_against_sqrt.SetPoint(i, par_sqrt, par_cnst);
		
		//temporary fixes, 'til i can run with more data
		if (par_cnst < 0.000001 | par_sqrt < 0.000001) {
			err_cnst = 0.;
			err_sqrt = 0.;
		}
		if (err_cnst > 0.1 | err_sqrt > 0.1) {
			err_cnst = 0.;
			err_sqrt = 0.;
		}
		
		cnst_against_sqrt.SetPointError(i, err_sqrt, err_cnst);
	}
	
	//get separate colors
	TGraphErrors points_CEMC(npoints/2);
	Int_t i_CEMC = 0;
	TGraphErrors points_FEMC(npoints/2);
	Int_t i_FEMC = 0;
	for (Long64_t i = 0; i < npoints; ++i) {
		tree_pa->GetEntry(i);
		
		//temporary fixes, 'til i can run with more data
		if (par_cnst < 0.000001 | par_sqrt < 0.000001) {
			err_cnst = 0.;
			err_sqrt = 0.;
		}
		if (err_cnst > 0.1 | err_sqrt > 0.1) {
			err_cnst = 0.;
			err_sqrt = 0.;
		}
		
		if (
			strcmp(origin, "data/de/DE00011104.root") == 0
			|| strcmp(origin, "data/de/DE00011000.root") == 0
			|| strcmp(origin, "data/de/DE00011008.root") == 0
			|| strcmp(origin, "data/de/DE10011104.root") == 0
			|| strcmp(origin, "data/de/DE10011000.root") == 0
			|| strcmp(origin, "data/de/DE10011008.root") == 0)
		{
			points_CEMC.SetPoint(i_CEMC, par_sqrt, par_cnst);
			points_CEMC.SetPointError(i_CEMC, err_sqrt, err_cnst);
			++i_CEMC;
		}
		else {
			points_FEMC.SetPoint(i_FEMC, par_sqrt, par_cnst);
			points_FEMC.SetPointError(i_FEMC, err_sqrt, err_cnst);
			++i_FEMC;
		}
	}
	
	file_pa->Close();
	
	/// secretly, make a copy of that TGraph, but shift the points
	/// so that the center of mass is at the origin.
	
	/// why? 'cause TGraph has member functions that allow you
	/// to get the RMS of all your points, but no function
	/// that allows to get RMS deviation from the mean.
	/// if the mean is zero, RMS and RMS deviation are equal.
	
	/// i need the RMS deviation 'cause i will be plotting
	/// a big two-sigma bounding box for all these points.
	
//	TGraph secret(npoints);
	Float_t mean_sqrt = cnst_against_sqrt.GetMean(1);
	Float_t mean_cnst = cnst_against_sqrt.GetMean(2);
//	for (Long64_t i = 0; i < npoints; ++i) {
//		cnst_against_sqrt.GetPoint(i, par_sqrt, par_cnst);
//		secret.SetPoint(i, par_sqrt - mean_sqrt, par_cnst - mean_cnst);
//	}
//	Float_t sigma_sqrt = secret.GetRMS(1);
//	Float_t sigma_cnst = secret.GetRMS(2);
	Float_t sigma_sqrt = cnst_against_sqrt.GetRMS(1);
	Float_t sigma_cnst = cnst_against_sqrt.GetRMS(2);
	
	///-----------------
	/// PLOT THE TGRAPH
	///-----------------
	
	/// also, draw a box at two sigma around the center of mass.
	/// ..this is the box mentioned earlier, when constructing the secret TGraph.
	/// i choose two sigma 'cause one sigma holds only 68% of points.
	/// with just like four samples, one is likely to fall outside of one sigma.
	/// two sigma, on the other hand, holds like 95% of everything.
	/// an outlier there indicates a potential problem.
	
	TCanvas canvas;
	SetsPhenixStyle();
	
	TGraphErrors box(1);
	box.SetPoint(0, mean_sqrt, mean_cnst);
	box.SetPointError(0, 2*sigma_sqrt, 2*sigma_cnst);
	box.SetFillColor(17);
	
	cnst_against_sqrt.SetFillColor(kBlack);
	points_CEMC.SetFillColor(kYellow);
	points_FEMC.SetFillColor(kBlue);
	cnst_against_sqrt.Draw("a2");
	box.Draw("2");
	cnst_against_sqrt.Draw("2");
	points_FEMC.Draw("2");
	points_CEMC.Draw("2");
	
	gPad->Print(filename_qa);
	
	return 0;
}

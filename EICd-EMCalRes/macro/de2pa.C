#include "macro/globals.h"

int de2pa(
	const char *filename_de,
	const char *filename_pa,
	const char *filename_qa)
{
	///--------------
	/// INTRODUCTION
	///--------------
	
	/// this macro is designed to...
	/// 	
	///		fit a bunch of (dE/E, E) points to a curve
	
	/// as of my writing, it uses a single flavor/pseudorapidity pair (a DE file) as input.
	
	/// it appends to a TTree file as output.
	/// it does not create the file or the TTree.
	
	///-----------------------
	/// LOAD LIBRARIES/MACROS
	///-----------------------
	
	gSystem->Load("libcemc.so");
	
	gROOT->SetMacroPath("/sphenix/user/jordan/sphenix-git/macros/macros/g4simulations");
	gROOT->LoadMacro("../sPHENIXStyle/sPhenixStyle.C");
	
	///----------------------------------
	/// DUMP THE D.E. FILE INTO A TGRAPH
	///----------------------------------
	
	TFile *file_de = new TFile(filename_de, "READ");
	TNtuple *tree_de = (TNtuple*)file_de->Get("TREE_DE");
	Float_t de_over_e, d_de_over_e, e;
	tree_de->SetBranchAddress("de_over_e", &de_over_e);
	tree_de->SetBranchAddress("d_de_over_e", &d_de_over_e);
	tree_de->SetBranchAddress("e", &e);
	Long64_t npoints = tree_de->GetEntries();
	TGraphErrors de_over_e_against_e(npoints);
	for (Long64_t i = 0; i < npoints; ++i) {
		tree_de->GetEntry(i);
		de_over_e_against_e.SetPoint(i, e, de_over_e);
		de_over_e_against_e.SetPointError(i, 0., d_de_over_e);
	}
	file_de->Close();
	
	///----------------
	/// FIT THE TGRAPH
	///----------------
	
	/// the energy resolution is expected to go like "dE/E = 1 (+) 1/sqrt(E)"
	
	TF1 ffit("ffit", "sqrt([0]**2+[1]**2/x)", 0., 0.);
	ffit.SetParameters(0., 0.);
	de_over_e_against_e.Fit(&ffit);
	
	Float_t par_cnst, par_cnst_uncertainty, par_sqrt, par_sqrt_uncertainty;
	par_cnst = ffit.GetParameter(0);
	par_cnst_uncertainty = ffit.GetParError(0);
	par_sqrt = ffit.GetParameter(1);
	par_sqrt_uncertainty = ffit.GetParError(1);
	
	/// also save a plot of the TGraph and fit
	
	TCanvas canvas;
	SetsPhenixStyle();
	
	TLegend leg(.55, .55, .95, .95);
	leg.SetHeader("#it{#bf{EICd}} Fiddle");
	leg.AddEntry("", filename_qa, "");
	char entry[256];
	snprintf(
		entry,
		sizeof(entry),
		"(%.1f #pm %.1f)%% #oplus (%.1f #pm %.1f)%%/#sqrt{E}",
		par_cnst*100,
		par_cnst_uncertainty*100,
		par_sqrt*100,
		par_sqrt_uncertainty*100);
	leg.AddEntry(&de_over_e_against_e, entry, "p");
	
	de_over_e_against_e.SetMarkerStyle(kFullDotLarge);
	de_over_e_against_e.Draw("ap");
	leg.Draw("same");
	de_over_e_against_e.Draw("p same");
	
	gPad->Print(filename_qa);
	
	///-----------------------------------
	/// PUT FIT PARAMETERS INTO P.A. FILE
	///-----------------------------------
	
	TFile *file_pa = new TFile(filename_pa, "UPDATE");
	TTree *tree_pa = (TTree*)file_pa->Get("TREE_PA");
	tree_pa->SetBranchAddress("cnst", &par_cnst);
	tree_pa->SetBranchAddress("d_cnst", &par_cnst_uncertainty);
	tree_pa->SetBranchAddress("sqrt", &par_sqrt);
	tree_pa->SetBranchAddress("d_sqrt", &par_sqrt_uncertainty);
	tree_pa->SetBranchAddress("filename_origin", filename_de);
	tree_pa->Fill();
	file_pa->Write("", TObject::kOverwrite);
	file_pa->Close();
	
	return 0;
}

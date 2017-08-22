#include "macro/globals.h"

int check_XEMC()
{
	/// makes a plot with three energy resolution curves.
	/// everything in FEMC acceptance.
	/// to check to see if they close enough to combine.
	
	TCanvas c;
	
	Float_t E_MIN = 0.;
	Float_t E_MAX = 25. * 1.1;
	Float_t DE_OVER_E_MIN = 0.;
	Float_t DE_OVER_E_MAX = .15;
	Int_t N_POINTS = 9;
	
	gPad->DrawFrame(E_MIN, DE_OVER_E_MIN, E_MAX, DE_OVER_E_MAX, "");
	
	TLegend leg(.25, .55, .85, .85);
	leg.SetHeader("#it{#bf{EICd}} Fiddle");
	
	TFile *file_pa = new TFile("data/pa/PA.root", "READ");
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
		if (
			strcmp(filename_origin, "data/de/DE00011016.root") == 0 ||
			strcmp(filename_origin, "data/de/DE00011024.root") == 0 ||
			strcmp(filename_origin, "data/de/DE00011032.root") == 0)
		{
			TF1 *curve = new TF1("", "sqrt([0]**2+[1]**2/x)", E_MIN, E_MAX);
			curve->SetParameter(0, par_cnst);
			curve->SetParameter(1, par_sqrt);
			
			TGraphErrors *points = new TGraphErrors(N_POINTS);
			TFile *file_origin = new TFile(filename_origin, "READ");
			TNtuple *tree_de = file_origin->Get("TREE_DE");
			Float_t de_over_e, d_de_over_e, e;
			tree_de->SetBranchAddress("de_over_e", &de_over_e);
			tree_de->SetBranchAddress("d_de_over_e", &d_de_over_e);
			tree_de->SetBranchAddress("e", &e);
			for (Int_t i_de = 0; i_de < tree_de->GetEntries(); ++i_de) {
				tree_de->GetEntry(i_de);
				points->SetPoint(i_de, e, de_over_e);
				points->SetPointError(i_de, 0., d_de_over_e);
			}
			
			char label[256];
			snprintf(
				label,
				sizeof(label),
				"%s   (%.2f#pm%.2f)%% #oplus (%.2f#pm%.2f)%%/#sqrt{E}",
				filename_origin,
				curve->GetParameter(0) * 100.,
				err_cnst * 100.,
				curve->GetParameter(1) * 100,
				err_sqrt * 100.);
			leg.AddEntry(points, label, "p");
			points->SetMarkerStyle(20);
			curve->Draw("same");
			points->Draw("p");
			
			delete tree_de;
			file_origin->Close();
			delete file_origin;
		}
	}
	
	leg.Draw("same");
	gPad->Print("CHECK.pdf");
	
	return 0;
}

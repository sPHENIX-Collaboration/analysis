#include "macro/globals.h"

int rc2de(
	const char *filename_rcli0,
	const char *filename_de,
	const char *filename_qa,
	Float_t momentum)
{
	///--------------
	/// INTRODUCTION
	///--------------
	
	/// this macro is designed to...
	/// 	
	///		get electron energies from cluster energies
	///		divide the reconstructed energies by truth energies
	///		histogram that ratio
	///		fit the histogram to a gaussian
	///		extract the gaussian's standard deviation
	
	/// this macro takes the name of an ASCII text file as input.
	/// that file lists the names of a bunch of RC files.
	/// the contents are those files are indiscriminately combined to compute dE/E.
	/// so, do not list files with heterogenous truth energies.. please.
	
	/// as of this writing, this macro is *still* working with one flavor/energy/pseudorapidity tuple.
	/// it was built to be able to integrate over e.g. pseudorapidity, however.
	
	/// this macro appends to a TNtuple file as output.
	/// it does not create the file or the TNtuple.
	
	///-----------------------
	/// LOAD LIBRARIES/MACROS
	///-----------------------
	
	gSystem->Load("libcemc.so");
	
	gROOT->SetMacroPath("/sphenix/user/jordan/sphenix-git/macros/macros/g4simulations");
	gROOT->LoadMacro("../sPHENIXStyle/sPhenixStyle.C");
	
	///-------------------------------------------
	/// COMBINE ALL R.C. FILES INTO ONE HISTOGRAM
	///-------------------------------------------
	
	TH1F *energy_spectrum = new TH1F(
		"energy_spectrum",
		"Electron Energy Spectrum",
		ENERGY_SPECTRUM_NBINS,
		0,
		ENERGY_SPECTRUM_XMAX);
	
	ifstream file_rcli0(filename_rcli0);
	string filename_rc;
	while (getline(file_rcli0, filename_rc)) {
		/// set up to read every cluster energy of every event in this file.
		
		TFile *file_rc = new TFile(filename_rc.c_str(), "READ");
		TTree *event[NCALOS];
		Int_t ncluster[NCALOS];
		Float_t energy[NCALOS][MAX_NCLUSTER], eta[NCALOS][MAX_NCLUSTER];
		for (int i_calo = 0; i_calo < NCALOS; ++i_calo) {
			string tree_name = "CLUSTER_" + string(calo_names[i_calo]);
			event[i_calo] = (TTree*)file_rc->Get(tree_name.c_str());
			event[i_calo]->SetBranchAddress("ncluster", &ncluster[i_calo]);
			event[i_calo]->SetBranchAddress("energy", energy[i_calo]);
			event[i_calo]->SetBranchAddress("eta", eta[i_calo]);
		}
		
		/// reconstruct electron energies and dump into histogram
		
		for (int i_event = 0; i_event < event[0]->GetEntries(); ++i_event) {
			for (int i_calo = 0; i_calo < NCALOS; ++i_calo) {
				event[i_calo]->GetEntry(i_event);
			}
			
			/// take highest cluster energy of each event as electron energy
			/// if event has no clusters, toss it..
			/// ..we look at resolution, not efficiency.
			
			Float_t highest_energy = -1.;
			for (int i_calo = 0; i_calo < NCALOS; ++i_calo) {
				for (int i_cluster = 0; i_cluster < ncluster[i_calo]; ++i_cluster) {
					if (energy[i_calo][i_cluster] > highest_energy) {
						highest_energy = energy[i_calo][i_cluster];
					}
				}
			}
			if (highest_energy != -1.) {
				energy_spectrum->Fill(highest_energy / momentum);
			}
		}
		
		for (int i_calo = 0; i_calo < NCALOS; ++i_calo)
			delete event[i_calo];
		file_rc->Close();
		delete file_rc;
	}
	file_rcli0.close();
	
	///--------------------------------
	/// FIT THAT HISTOGRAM TO GET dE/E
	///--------------------------------
	
	/// start with identical guess function and fit function.
	/// then, do the fit.. the fit function is modified.
	/// the guess copy is so you can plot the guess after the fit.
	
	Float_t integral = energy_spectrum->GetEntries() * ENERGY_SPECTRUM_XMAX / ENERGY_SPECTRUM_NBINS;
	Float_t mean = energy_spectrum->GetXaxis()->GetBinCenter(energy_spectrum->GetMaximumBin());
	Float_t width = energy_spectrum->GetStdDev();
	if (integral / width < energy_spectrum->GetMaximum()) {
		/// many of the EEMC spectra are so tall and narrow
		/// that the standard deviation does not well-capture the gaussian width.
		/// in that case, the guestimated peak height of the fit function
		/// will be much lower than the maximum of the energy spectrum.
		
		width *= 0.25;
	}
	
	TF1 fguess("fguess", "gaus(0) / (sqrt(6.28) * [2])", 0, ENERGY_SPECTRUM_XMAX);
	fguess.SetParameters(integral, mean, width);
	TF1 ffit("ffit", "gaus(0) / (sqrt(6.28) * [2])", 0, ENERGY_SPECTRUM_XMAX);
	ffit.SetParameters(integral, mean, width);
	
	/// when fits go bad, often the low tail of the gaussian
	/// will fit to the low (power law) tail of the energy spectrum.
	/// this gives me really, really wide gaussians,
	/// with peaks nowhere near 1.
	
	ffit.SetParLimits(1, 0., 1.); //mean between 0 and 1 .. upper limit of 1 'cause you should not collect more energy than exists
	ffit.SetParLimits(2, 0., .3); //standard deviation .. no good fit is over 0.3, and low-resolution spectra are all fit very well
	
	energy_spectrum->Fit("ffit");
	Float_t energy_resolution = ffit.GetParameter(2);
	Float_t energy_resolution_uncertainty = ffit.GetParError(2);
	
	/// save a plot for debugging purposes
	
	TCanvas canvas;
	SetsPhenixStyle();
	
	TLegend leg(.05, .55, .45, .95);
	leg.SetHeader("#it{#bf{EICd}} Fiddle");
	leg.AddEntry("", filename_qa, "");
	char entrytext[256];
	snprintf(entrytext, sizeof(entrytext), "entries: %.2f", energy_spectrum->GetEntries());
	leg.AddEntry("", entrytext, "");
	snprintf(entrytext, sizeof(entrytext), "overflow: %.2f", energy_spectrum->GetEntries()-energy_spectrum->GetSumOfWeights());
	leg.AddEntry("", entrytext, "");
	snprintf(entrytext, sizeof(entrytext), "chi2/ndf: %.2f", ffit.GetChisquare() / ffit.GetNDF());
	leg.AddEntry("", entrytext, "");
	snprintf(entrytext, sizeof(entrytext), "resolution: %.2f#pm%.2f", energy_resolution, energy_resolution_uncertainty);
	leg.AddEntry("", entrytext, "");
	
	energy_spectrum->Draw();
	leg.Draw("same");
	fguess.SetFillColor(5);
	fguess.SetFillStyle(1001);
	fguess.Draw("fc same");
	energy_spectrum->Draw("same");
	
	gPad->Print(filename_qa);
	
	///-------------------------
	/// PUT dE/E INTO D.E. FILE
	///-------------------------
	
	TFile *file_de = new TFile(filename_de, "UPDATE");
	TNtuple *tree_de = (TNtuple*)file_de->Get("TREE_DE;1");
	tree_de->Fill(energy_resolution, energy_resolution_uncertainty, momentum);
	file_de->Write("", TObject::kOverwrite);
	file_de->Close();
	
	return 0;
}

#include <iostream>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"

#define NELEMS(arr) (sizeof(arr)/sizeof(arr[0]))

/* Directory where data is stored for plots */
const char *const data_directory =
    "/sphenix/user/gregtom3/data/Summer2018/ECAL_energy_studies";
/* The energy levels we have in GeV */
const static int energy_levels[] = { 1, 2, 5, 10, 20 };

/* The particle types we have */
enum particle_type { electron, pion };
/* The detectors we have */
enum detector { cemc, eemc, femc };

TTree *load_tree(const char *const file_name, const char *const tree_name);
void fill_histogram(TH1F * const h, TTree * const t, const Float_t min_value,
		    const bool normalize);
void display_histogram(TH1F * const h_pion, TH1F * const h_electron,
		       const char *const title, const char *const label);
char *generate_name(const particle_type p, const int particle_energy_gev,
		    const detector d);
char *generate_file_path(const particle_type p, const int particle_energy_gev,
			 const detector d);
char *generate_title(const int particle_energy_gev, const detector d);
char *generate_label(const int particle_energy_gev, const detector d);

void LivePlot_Energy_EMC()
{

	/* 
	 * sPHENIX Style
	 */

	gROOT->LoadMacro
	    ("/sphenix/user/gregtom3/SBU/research/macros/macros/sPHENIXStyle/sPhenixStyle.C");
	SetsPhenixStyle();

	/*
	 * Base Histogram (Recreated from Matching Plots)
	 */

	TH1F *h_base = new TH1F("h_base", "", 25, 0.0, 2.5);
	TH1F *h_base_e = (TH1F *) h_base->Clone();
	TH1F *h_base_p = (TH1F *) h_base->Clone();
	h_base_e->SetLineColor(kRed);
	h_base_p->SetLineColor(kBlue);

	/* iterate through energy levels and create plots for Pions and Electrons
	 * in the CEMC and EEMC detectors */
	for (size_t i = 0; i < NELEMS(energy_levels); ++i) {
		/* CEMC */
		TH1F *const h_pion_cemc = h_base_p->Clone();
		TH1F *const h_electron_cemc = h_base_e->Clone();
		h_pion_cemc->SetName(generate_name
				     (pion, energy_levels[i], cemc));
		h_electron_cemc->SetName(generate_name
					 (electron, energy_levels[i], cemc));

		TTree *const t_pion_cemc =
		    load_tree(generate_file_path(pion, energy_levels[i], cemc),
			      "ntp_cluster");
		TTree *const t_electron_cemc =
		    load_tree(generate_file_path
			      (electron, energy_levels[i], cemc),
			      "ntp_cluster");

		fill_histogram(h_pion_cemc, t_pion_cemc, 0.3, true);
		fill_histogram(h_electron_cemc, t_electron_cemc, 0.3, true);
		display_histogram(h_pion_cemc, h_electron_cemc,
				  generate_title(energy_levels[i], cemc),
				  generate_label(energy_levels[i], cemc));

		/* EEMC */
		TH1F *const h_pion_eemc = h_base_p->Clone();
		TH1F *const h_electron_eemc = h_base_e->Clone();
		h_pion_eemc->SetName(generate_name
				     (pion, energy_levels[i], eemc));
		h_electron_eemc->SetName(generate_name
					 (electron, energy_levels[i], eemc));

		TTree *const t_pion_eemc =
		    load_tree(generate_file_path(pion, energy_levels[i], eemc),
			      "ntp_cluster");
		TTree *const t_electron_eemc =
		    load_tree(generate_file_path
			      (electron, energy_levels[i], eemc),
			      "ntp_cluster");

		fill_histogram(h_pion_eemc, t_pion_eemc, 0.3, true);
		fill_histogram(h_electron_eemc, t_electron_eemc, 0.3, true);
		display_histogram(h_pion_eemc, h_electron_eemc,
				  generate_title(energy_levels[i], eemc),
				  generate_label(energy_levels[i], eemc));

		/* FEMC */
		TH1F *const h_pion_femc = h_base_p->Clone();
		TH1F *const h_electron_femc = h_base_e->Clone();
		h_pion_femc->SetName(generate_name
				     (pion, energy_levels[i], femc));
		h_electron_femc->SetName(generate_name
					 (electron, energy_levels[i], femc));

		TTree *const t_pion_femc =
		    load_tree(generate_file_path(pion, energy_levels[i], femc),
			      "ntp_cluster");
		TTree *const t_electron_femc =
		    load_tree(generate_file_path
			      (electron, energy_levels[i], femc),
			      "ntp_cluster");

		fill_histogram(h_pion_femc, t_pion_femc, 0.3, true);
		fill_histogram(h_electron_femc, t_electron_femc, 0.3, true);
		display_histogram(h_pion_femc, h_electron_femc,
				  generate_title(energy_levels[i], femc),
				  generate_label(energy_levels[i], femc));

	}

}

TTree *load_tree(const char *const file_name, const char *const tree_name)
{
	return (TTree *) (new TFile(file_name, "READ"))->Get(tree_name);
}

/*
 * This function, when fed a histogram, tree,
 * a floating point min_value variable, and a boolean, will fill each entry
 * inside the specificed branch into the histogram, so long as each entry
 * is a floating point number GREATER than the min_value. If normalize is
 * set to 'true', the histogram will be normalize based on its number of
 * entries. The axes titles are furthermore assumed to be generic and have
 * been already set.
 */
void fill_histogram(TH1F * const h, TTree * const t, const Float_t min_value,
		    const bool normalize)
{
	Float_t measured_energy;
	Float_t true_energy;
	t->SetBranchAddress("e", &measured_energy);
	t->SetBranchAddress("ge", &true_energy);
	Int_t nentries = Int_t(t->GetEntries());

	for (Int_t i = 0; i < nentries; ++i) {
		if (t->LoadTree(i) < 0)
			break;

		t->GetEntry(i);
		if (measured_energy > min_value && true_energy > 0.1)
			h->Fill(measured_energy / true_energy);
	}
	if (normalize)
		h->Scale(1 / h->GetEntries());

	h->SetXTitle("em_cluster_e / true_e");
	h->SetYTitle("entries / #scale[0.5]{#sum} entries      ");
}

void display_histogram(TH1F * const h_pion, TH1F * const h_electron,
		       const char *const title, const char *const label)
{
	TCanvas *cPNG = new TCanvas(label, title, 600, 400);

	h_pion->GetYaxis()->SetRangeUser(0.0001, 1);
	h_electron->GetYaxis()->SetRangeUser(0.0001, 1);
	gPad->SetLogy();
	h_pion->Draw();
	h_electron->Draw("SAME");
	gPad->RedrawAxis();

	auto legend = new TLegend(0.70, 0.9, 0.95, 0.65, label);
	legend->AddEntry(h_pion, "Pions", "l");
	legend->AddEntry(h_electron, "Electrons", "l");
	legend->Draw();
}

char *strdup(const char *s)
{
	char *const t = new char[strlen(s) + 1];
	return strcpy(t, s);
}

char *generate_name(const particle_type p, const int particle_energy_gev,
		    const detector d)
{
	std::stringstream name;
	name << "Histogram";
	switch (p) {
	case electron:
		name << "_Electron";
		break;
	case pion:
		name << "_Pion";
		break;
	}

	name << "_" << particle_energy_gev << "GeV";
	switch (d) {
	case cemc:
		name << "_CEMC";
		break;
	case eemc:
		name << "_EEMC";
		break;
	}

	return strdup(name.str().c_str());
}

char *generate_file_path(const particle_type p,
			 const int particle_energy_gev, const detector d)
{
	std::stringstream path;
	path << data_directory;

	switch (p) {
	case electron:
		path << "/Electrons/Electrons";
		break;
	case pion:
		path << "/Pions/Pions";
		break;
	}

	path << particle_energy_gev;
	switch (d) {
	case cemc:
		path << "C";
		break;
	case eemc:
		path << "E";
		break;
	case femc:
		path << "F";
		break;
	}

	path << ".root";

	return strdup(path.str().c_str());
}

char *generate_title(const int particle_energy_gev, const detector d)
{
	return generate_label(particle_energy_gev, d);
}

char *generate_label(const int particle_energy_gev, const detector d)
{
	std::stringstream label;

	switch (d) {
	case cemc:
		label << "CEMC ";
		break;
	case eemc:
		label << "EEMC ";
		break;
	case femc:
		label << "FEMC ";
		break;
	}
	label << particle_energy_gev << "GeV";

	return strdup(label.str().c_str());
}

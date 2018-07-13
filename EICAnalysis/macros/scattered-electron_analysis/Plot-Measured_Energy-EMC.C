#include <iostream>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <cassert>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TColor.h"
#include "TImage.h"

/* 
 * sPHENIX Style
 */
#include "/sphenix/user/gregtom3/SBU/research/macros/macros/sPHENIXStyle/sPhenixStyle.C"

#define NELEMS(arr) (sizeof(arr)/sizeof(arr[0]))

/* The particle types we have */
enum particle_type { electron, pion };
/* The detectors we have */
enum detector { cemc, eemc, femc };

TTree *load_tree(const char *const file_name, const char *const tree_name);
void draw_histogram(TH1F * const h, const particle_type p,
		    const int energy_level_gev, const detector d);
void fill_histogram(TH1F * const h, TTree * const t, const Float_t min_value);
char *generate_histogram_name(const particle_type p,
			      const int particle_energy_gev, const detector d);
char *generate_file_path(const particle_type p, const int particle_energy_gev,
			 const detector d);
char *generate_save_file_path(const particle_type p);
char *generate_canvas_name(const particle_type p);
char *generate_canvas_title(const particle_type p);
char *generate_legend_header(const particle_type p, const detector d);
char *generate_legend_entry_label(const int particle_energy_gev);

/* Directory where data is stored for plots */
const char *const data_directory =
    "/sphenix/user/gregtom3/data/Summer2018/ECAL_energy_studies";
/* The energy levels we have in GeV */
const static int energy_levels[] = { 1, 2, 5, 10, 20 };

/* How to change lines for each energy level */
const static int color_offsets[] = { -10, -9, -7, -4, 0 };

/* Detectors that we want to plot */
const static enum detector detectors[] = { cemc, eemc, femc };

/* Particles we want to plot */
const static enum particle_type particles[] = { pion, electron };

/* The color of the plot corresponding to each particle */
const static Color_t plot_colors[] = { kBlue, kRed, kGreen, kOrange, kViolet };

/* These should not be necessary, but root is buggy */
const int nenergy_levels = NELEMS(energy_levels);
const int ndetectors = NELEMS(detectors);
const int nparticles = NELEMS(particles);

void Plot_Measured_Energy_EMC()
{
	/* sanity check */
	assert(NELEMS(energy_levels) <= NELEMS(plot_colors));

	SetsPhenixStyle();
	gROOT->SetBatch(kTRUE);

	/*
	 * Base Histogram (Recreated from Matching Plots)
	 */
	TH1F *const h_base = new TH1F("h_base", "", 100, 0.0, 25);

	std::vector < TCanvas * >particle_canvases;
	for (int i = 0; i < nparticles; ++i) {
		TCanvas *const c =
		    new TCanvas(generate_canvas_name(particles[i]),
				generate_canvas_title(particles[i]),
				gStyle->GetCanvasDefW() * ndetectors,
				gStyle->GetCanvasDefH());
		c->Divide(ndetectors, 1);
		particle_canvases.push_back(c);
	}

	/* iterate through energy levels, detectors, and particles and create plots for each */
	for (int e = 0; e < nenergy_levels; ++e)
		for (int d = 0; d < ndetectors; ++d)
			for (int p = 0; p < nparticles; ++p) {
				/* +1 to account for 1-based indexing */
				particle_canvases[p]->cd(d + 1);
				TH1F *const h = (TH1F *) h_base->Clone();
				h->SetLineColor(plot_colors[e]);
				draw_histogram(h, particles[p],
					       energy_levels[e], detectors[d]);
			}

	/* add legends */
	for (int d = 0; d < ndetectors; ++d)
		for (int p = 0; p < nparticles; ++p) {
			particle_canvases[p]->cd(d + 1);
			gPad->RedrawAxis();
			if (d == 0) {
				TLegend *const l =
				    new TLegend(0.60, .90, .9, 0.625,
						generate_legend_header(particles
								       [p],
								       detectors
								       [d]));
				l->SetTextSize(0.05);
				for (int e = 0; e < nenergy_levels; ++e)
					l->AddEntry(generate_histogram_name
						    (particles[p],
						     energy_levels[e],
						     detectors[d]),
						    generate_legend_entry_label
						    (energy_levels[e]), "l");

				l->Draw();
			} else {
				TLegend *const l =
				    new TLegend(0.60, .90, .9, 0.85,
						generate_legend_header(particles
								       [p],
								       detectors
								       [d]));
				l->SetTextSize(0.05);
				l->Draw();
			}

			gPad->SetLogy();
		}

	/* save canvases to PNG */
	for (int p = 0; p < nparticles; ++p) {
		TImage *const img = TImage::Create();
		img->FromPad(particle_canvases[p]);
		char *const path = generate_save_file_path(particles[p]);
		img->WriteImage(path);
		delete img;
	}
}

void draw_histogram(TH1F * const h, const particle_type p,
		    const int energy_level_gev, const detector d)
{
	h->SetName(generate_histogram_name(p, energy_level_gev, d));

	TTree *const t = load_tree(generate_file_path(p, energy_level_gev, d),
				   "ntp_cluster");

	fill_histogram(h, t, 0.3);
	h->Scale(1 / h->GetEntries());
	h->GetYaxis()->SetRangeUser(0.0001, 10);
	h->SetXTitle("E_{cluster} (GeV)");
	h->SetYTitle("entries / #scale[0.5]{#sum} entries      ");

	h->Draw("SAME");
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
void fill_histogram(TH1F * const h, TTree * const t, const Float_t min_value)
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
			h->Fill(measured_energy);
	}
}

char *strdup(const char *s)
{
	char *const t = new char[strlen(s) + 1];
	return strcpy(t, s);
}

char *generate_histogram_name(const particle_type p,
			      const int particle_energy_gev, const detector d)
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

char *generate_save_file_path(const particle_type p)
{
	std::stringstream name;

	switch (p) {
	case pion:
		name << "Pion";
		break;
	case electron:
		name << "Electron";
		break;
	}

	name << "-Measured_Energy";
	for (int d = 0; d < ndetectors; ++d)
		switch (detectors[d]) {
		case cemc:
			name << "-CEMC";
			break;
		case eemc:
			name << "-EEMC";
			break;
		case femc:
			name << "-FEMC";
			break;
		}

	name << ".png";

	return strdup(name.str().c_str());
}

char *generate_canvas_name(const particle_type p)
{
	std::stringstream canvas_name;

	switch (p) {
	case pion:
		canvas_name << "Pion";
		break;
	case electron:
		canvas_name << "Electron";
		break;
	}

	return strdup(canvas_name.str().c_str());
}

char *generate_canvas_title(const particle_type p)
{
	std::stringstream canvas_title;

	switch (p) {
	case pion:
		canvas_title << "Pion";
		break;
	case electron:
		canvas_title << "Electron";
		break;
	}

	return strdup(canvas_title.str().c_str());
}

char *generate_legend_header(const particle_type p, const detector d)
{
	std::stringstream legend_header;

	switch (d) {
	case cemc:
		legend_header << "CEMC ";
		break;
	case eemc:
		legend_header << "EEMC ";
		break;
	case femc:
		legend_header << "FEMC ";
		break;
	}

	switch (p) {
	case pion:
		legend_header << "Pions ";
		break;
	case electron:
		legend_header << "Electrons ";
		break;
	}

	return strdup(legend_header.str().c_str());
}

char *generate_legend_entry_label(const int particle_energy_gev)
{
	std::stringstream legend_entry;
	legend_entry << particle_energy_gev << " GeV";

	return strdup(legend_entry.str().c_str());
}

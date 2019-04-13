#include <iostream>
#include <cstdlib>
#include <memory>
#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <cmath>
#include <utility>
#include <cassert>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TColor.h"
#include "TImage.h"
#include "TApplication.h"
#include "TGraphErrors.h"
#include "/sphenix/user/gregtom3/SBU/research/macros/macros/sPHENIXStyle/sPhenixStyle.C"

#define STR(X) #X
#define XSTR(X) STR(X)
#define NELEMS(a) (sizeof(a)/sizeof(a[0]))


TTree *load_tree(const char *const file_name, const char *const tree_name);
const char *const hitcount_file_path 
		{"/sphenix/user/gregtom3/data/Summer2018/SVTX_studies/hits-per-eta/hitcount.root"};

const char *const hit_containers[] {"G4HIT_EGEM_0", "G4HIT_EGEM_1", "G4HIT_EGEM_3", 
	"G4HIT_FGEM_0", "G4HIT_FGEM_1", "G4HIT_FGEM_2", "G4HIT_FGEM_3","G4HIT_FGEM_4", "G4HIT_MAPS",
	"G4HIT_SVTX"};
const static Color_t plot_colors[] = { kBlue, kSpring, kBlack, kYellow, kCyan, kGray, kMagenta, kOrange,
	kRed, kGreen + 3, kPink};


void Plot_Hit_Count() {
	assert(NELEMS(plot_colors) >= NELEMS(hit_containers));
	SetsPhenixStyle();
	gROOT->SetBatch(true);

	TCanvas *const c {new TCanvas {"hits", "Hits", gStyle->GetCanvasDefW(), gStyle->GetCanvasDefH()}};
	TLegend *const l {new TLegend {0.7, 0.9, 0.95, 0.61, "Detector"}};
	l->SetTextSize(0.03);
	Double_t max {0};
	std::vector<TH1F*> hists {};
	for (size_t i {0}; i < NELEMS(hit_containers); ++i) {
		const std::string tree_name {std::string(hit_containers[i]) + "_normalized"};
		TTree *const hits {load_tree(hitcount_file_path, tree_name.c_str())};


		Double_t eta, hit_count;
		hits->SetBranchAddress("eta", &eta);
		hits->SetBranchAddress("hit_count", &hit_count);

		Long64_t nentries {hits->GetEntries()};
		TH1F *const h {new TH1F {hit_containers[i], "Hit Count", 100, -4.5, 4.5}};
		h->SetXTitle("#eta");
		h->SetYTitle("Normalized Hit Count");
		h->SetLineColor(plot_colors[i]);
		for (Long64_t j {0}; j < nentries; ++j) {
			if (hits->LoadTree(j) < 0)
				break;
			hits->GetEntry(j);
			if (strcmp("G4HIT_SVTX", hit_containers[i]) == 0)
				hit_count /= 10;

			h->Fill(eta, hit_count);
		}

		const Long64_t nbins {h->GetSize() - 2};	/* -2 for underflow and overflow */
		for (Long64_t j {0}; j < nbins; ++j) {
			const Double_t hit_count {h->GetBinContent(j + 1)};
			max = (hit_count > max) ? hit_count : max;
		}


		hists.push_back(h);
		if (strcmp("G4HIT_SVTX", hit_containers[i]) == 0)
			l->AddEntry(h, "G4HIT_SVTX / 10", "l");
		else
			l->AddEntry(h, hit_containers[i], "l");
	}

	c->cd();
	for (const auto& h: hists) {
		h->GetYaxis()->SetRangeUser(0.000001, max * 1.1);
		h->Draw("SAME");

	}

	l->Draw();
	gPad->RedrawAxis();


	TImage *const img {TImage::Create()};
	img->FromPad(c);
	img->WriteImage("Hits_vs_Eta.png");

	gApplication->Terminate(0);
}

TTree *load_tree(const char *const file_name, const char *const tree_name)
{
	return (TTree *) (new TFile(file_name, "READ"))->Get(tree_name);
}

int main(int argc, char *argv[]) {
	TApplication app("Hit Plots", &argc, argv);
	Plot_Hit_Count();
	app.Run();
	return 0;
}


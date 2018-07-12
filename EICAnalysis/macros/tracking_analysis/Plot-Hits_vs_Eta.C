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
const char *const svtx_file_path {"/sphenix/user/giorgian/hits-per-eta/hitcount.root"};

const char *const hit_containers[] {"G4HIT_EGEM_0", "G4HIT_EGEM_1", "G4HIT_EGEM_3", 
	"G4HIT_FGEM_0", "G4HIT_FGEM_1", "G4HIT_FGEM_2", "G4HIT_FGEM_3","G4HIT_FGEM_4", "G4HIT_MAPS", 
	"G4HIT_SVTX"};
const static Color_t plot_colors[] = { kBlue, kSpring, kBlack, kYellow, kTeal, kGray, kMagenta, kAzure,
	kRed, kCyan, kViolet};


void Plot_Hit_Count() {
	assert(NELEMS(plot_colors) >= NELEMS(hit_containers));
	SetsPhenixStyle();
	gROOT->SetBatch(true);

	TCanvas c {"hits", "Hits", gStyle->GetCanvasDefW(), gStyle->GetCanvasDefH()};
	TLegend l {0.65, 0.9, 0.95, 0.8, "Detector"};
//	l.SetTextSize(0.01);
	for (size_t i {0}; i < NELEMS(hit_containers); ++i) {
		const std::string tree_name {std::string(hit_containers[i]) + "_normalized"};
		TTree *const hits {load_tree(svtx_file_path, tree_name.c_str())};


		Double_t eta, hit_count;
		hits->SetBranchAddress("eta", &eta);
		hits->SetBranchAddress("hit_count", &hit_count);

		Long64_t nentries {hits->GetEntries()};
		TH1F h {hit_containers[i], "Hit Count", 100, -4.5, 4.5};
		h.SetLineColor(plot_colors[i]);
		for (Long64_t i {0}; i < nentries; ++i) {
			if (hits->LoadTree(i) < 0)
				break;
			hits->GetEntry(i);
			h.Fill(eta, hit_count);
		}


		c.cd();
		h.Draw();

		l.AddEntry(&h, hit_containers[i], "l");
	}
	l.Draw();
	gPad->RedrawAxis();


	TImage *const img {TImage::Create()};
	img->FromPad(&c);
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


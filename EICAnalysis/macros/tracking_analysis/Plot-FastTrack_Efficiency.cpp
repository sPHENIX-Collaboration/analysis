#include <iostream>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <utility>

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

TTree *load_tree(const char *const file_name, const char *const tree_name);
const char *const fasttrack_file_path 
        {"/sphenix/user/gregtom3/data/Summer2018/fasttrack-efficiency-10GeV/fasttrack.root"};
double MOMENTUM_MARGIN {0.01};

void Plot_FastTrack_Efficiency()
{
	SetsPhenixStyle();
	gROOT->SetBatch(true);

	TTree *const tracks {load_tree(fasttrack_file_path, "tracks")};

	Long64_t ntracks {tracks->GetEntries()};

	TH1F *h_true_count {new TH1F("h_true_count", "Psuedorapidity count", 100, -5, 5)};
	h_true_count->SetXTitle("#eta");
	h_true_count->SetYTitle("Count");
	h_true_count->SetLineColor(kBlue);
	TH1F *h_reco_count {new TH1F("h_reco_count", "Psuedorapidity count", 100, -5, 5)};
	h_reco_count->SetXTitle("#eta");
	h_reco_count->SetYTitle("Count");
	h_reco_count->SetLineColor(kRed);
	Float_t gpx, gpy, gpz, px, py, pz;
	tracks->SetBranchAddress("gpx", &gpx);
	tracks->SetBranchAddress("gpy", &gpy);
	tracks->SetBranchAddress("gpz", &gpz);
	tracks->SetBranchAddress("px", &px);
	tracks->SetBranchAddress("py", &py);
	tracks->SetBranchAddress("pz", &pz);
	for (Long64_t i {0}; i < ntracks; ++i) {
		if (tracks->LoadTree(i) < 0)
			break;
		tracks->GetEntry(i);
		const double mom {std::sqrt(px * px + py * py + pz * pz)};
		const double eta {0.5 *std::log((mom + pz) / (mom - pz))};
		const double gmom {std::sqrt(gpx * gpx + gpy * gpy + gpz * gpz)};
		const double geta {0.5 *std::log((gmom + gpz) / (gmom - gpz))};

		h_true_count->Fill(geta);
		if (fabs((px - gpx) / gpx) < MOMENTUM_MARGIN
		    && fabs((py - gpy) / gpy) < MOMENTUM_MARGIN
		    && fabs((pz - gpz) / gpz) < MOMENTUM_MARGIN)
			h_reco_count->Fill(geta);

	}

	const Long64_t nbins {h_true_count->GetSize() - 2};	/* -2 for underflow and overflow */
	Double_t *x {new Double_t[nbins]};
	Double_t *y {new Double_t[nbins]};
	Double_t *ey {new Double_t[nbins]};
	Long64_t top {0};
	for (Long64_t i {0};i < nbins; ++i) {
		if (h_true_count->GetBinContent(i + 1) != 0) {
			const Double_t n {h_reco_count->GetBinContent(i + 1)};
			const Double_t N {h_true_count->GetBinContent(i + 1)};

			x[top] = {h_true_count->GetBinCenter(i + 1)};
			y[top] = {n / N};
			ey[top++] = {sqrt(n / (N * N) + (n * n) / (N * N * N))};
		}
	}
	TGraphErrors *gr {new TGraphErrors(top, x, y, nullptr, ey)};
	gr->SetMarkerColor(kBlue);
	gr->SetMarkerStyle(21);
	gr->SetMarkerSize(0.5);
	gr->GetXaxis()->SetTitle("#eta");
	gr->GetYaxis()->SetTitle("Efficiency");

	TCanvas *count {new TCanvas("count", "FastTrack Event Count",
			    gStyle->GetCanvasDefW(), gStyle->GetCanvasDefH())};
	h_true_count->GetYaxis()->SetRangeUser(0, 2000);
	h_true_count->Draw();
	h_reco_count->Draw("SAME");
	TLegend *l1 {new TLegend(0.825, .9, .95, 0.8, "Track")};
	l1->SetTextSize(0.03);
	l1->AddEntry(h_true_count, "True", "l");
	l1->AddEntry(h_reco_count, "Reco", "l");
	l1->Draw();
	gPad->RedrawAxis();

	TCanvas *efficiency {new TCanvas("efficiency", "FastTrack Efficiency",
			    gStyle->GetCanvasDefW(), gStyle->GetCanvasDefH())};
	gr->GetYaxis()->SetRangeUser(-0.05, 1);
	gr->Draw("ALP");
	TLegend *l2 {new TLegend(0.825, 0.9, 0.95, 0.8, "Track")};
	l2->SetTextSize(0.03);
	l2->AddEntry(gr, "Efficiency", "l");
	l2->Draw();
	gPad->RedrawAxis();

	TImage *const img {TImage::Create()};
	img->FromPad(count);
	std::stringstream name;
	name << "FastTrack_Event_Count-" << "margin=" << MOMENTUM_MARGIN <<
	    ".png";
	img->WriteImage(strdup(name.str().c_str()));

	name.str("");
	img->FromPad(efficiency);
	name << "FastTrack_Efficiency-" << "margin=" << MOMENTUM_MARGIN <<
	    ".png";
	img->WriteImage(strdup(name.str().c_str()));
	gApplication->Terminate(0);
}

TTree *load_tree(const char *const file_name, const char *const tree_name)
{
	return (TTree *) (new TFile(file_name, "READ"))->Get(tree_name);
}

int main(int argc, char *argv[])
{
	TApplication app {"FastTrack Efficiency Plots", &argc, argv};
	if (argc > 1) {
		std::stringstream tmp {argv[1]};
		tmp >> MOMENTUM_MARGIN;
	}

	Plot_FastTrack_Efficiency();
	app.Run();
	return 0;
}

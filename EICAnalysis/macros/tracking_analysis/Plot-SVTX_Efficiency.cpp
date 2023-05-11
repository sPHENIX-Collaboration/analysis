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
const char *const svtx_file_path {"/sphenix/user/giorgian/muons/svtx.root"};

double MOMENTUM_MARGIN {0.01};

void Plot_SVTX_Efficiency()
{
	SetsPhenixStyle();
	gROOT->SetBatch(true);

	TTree *const ntp_gtrack {load_tree(svtx_file_path, "ntp_gtrack")};
	TTree *const ntp_track {load_tree(svtx_file_path, "ntp_track")};

	Long64_t ngtrack {ntp_gtrack->GetEntries()};
	Long64_t ntrack {ntp_track->GetEntries()};

	TH1F *h_true_count {new TH1F("h_true_count", "Psuedorapidity count", 100, -5, 5)};
	h_true_count->SetXTitle("#eta");
	h_true_count->SetYTitle("Count");
	h_true_count->SetLineColor(kBlue);
	Float_t geta;
	ntp_gtrack->SetBranchAddress("geta", &geta);
	for (Long64_t i {0}; i < ngtrack; ++i) {
		if (ntp_gtrack->LoadTree(i) < 0)
			break;
		ntp_gtrack->GetEntry(i);
		h_true_count->Fill(geta);
	}
	TH1F *h_reco_count {new TH1F("h_reco_count", "Psuedorapidity count", 100, -5, 5)};
	h_reco_count->SetXTitle("#eta");
	h_reco_count->SetYTitle("Count");
	h_reco_count->SetLineColor(kRed);
	Float_t gpx, gpy, gpz, px, py, pz;
	ntp_track->SetBranchAddress("geta", &geta);
	ntp_track->SetBranchAddress("gpx", &gpx);
	ntp_track->SetBranchAddress("gpy", &gpy);
	ntp_track->SetBranchAddress("gpz", &gpz);
	ntp_track->SetBranchAddress("px", &px);
	ntp_track->SetBranchAddress("py", &py);
	ntp_track->SetBranchAddress("pz", &pz);
	for (Long64_t i {0}; i < ntrack; ++i) {
		if (ntp_track->LoadTree(i) < 0)
			break;

		ntp_track->GetEntry(i);
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
	for (Long64_t i {0}; i < nbins; ++i) {
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

	TCanvas *count {new TCanvas("count", "SVTX Event Count",
			    gStyle->GetCanvasDefW(), gStyle->GetCanvasDefH())};
	h_true_count->GetYaxis()->SetRangeUser(0, 2000);
	h_true_count->Draw();
	h_reco_count->Draw("SAME");
	TLegend *l1 {new TLegend(0.825, .90, .95, 0.80, "Track")};
	l1->SetTextSize(0.03);
	l1->AddEntry(h_true_count, "True", "l");
	l1->AddEntry(h_reco_count, "Reco", "l");
	l1->Draw();
	gPad->RedrawAxis();

	TCanvas *efficiency {new TCanvas("efficiency", "SVTX Efficiency",
                                gStyle->GetCanvasDefW(), gStyle->GetCanvasDefH())};
	gr->GetYaxis()->SetRangeUser(-0.05, 1);
	gr->Draw("ALP");
	TLegend *l2 {new TLegend(0.825, 0.90, 0.95, 0.8, "Track")};
	l2->SetTextSize(0.03);
	l2->AddEntry(gr, "Efficiency", "l");
	l2->Draw();
	gPad->RedrawAxis();

	TImage *const img {TImage::Create()};
	img->FromPad(count);
	std::stringstream name;
	name << "SVTX_Event_Count-" << "margin=" << MOMENTUM_MARGIN << ".png";
	img->WriteImage(strdup(name.str().c_str()));

	name.str("");
	img->FromPad(efficiency);
	name << "SVTX_Efficiency-" << "margin=" << MOMENTUM_MARGIN << ".png";
	img->WriteImage(strdup(name.str().c_str()));
	gApplication->Terminate(0);
}

TTree *load_tree(const char *const file_name, const char *const tree_name)
{
	return (TTree *) (new TFile(file_name, "READ"))->Get(tree_name);
}

int main(int argc, char *argv[])
{
	TApplication app {"SVTX Efficiency Plots", &argc, argv};
	if (argc > 1) {
		std::stringstream tmp {argv[1]};
		tmp >> MOMENTUM_MARGIN;
	}

	Plot_SVTX_Efficiency();
	app.Run();
	return 0;
}

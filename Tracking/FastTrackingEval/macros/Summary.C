
void Summary(const char *input_root_name = "g4fwdtrack_fastsim_eval.root") {

	TFile* in_file = TFile::Open(input_root_name,"read");

	TH2D* _h2d_Delta_mom_vs_truth_mom = (TH2D*) in_file->Get("_h2d_Delta_mom_vs_truth_mom");
	TH2D* _h2d_Delta_mom_vs_truth_eta = (TH2D*) in_file->Get("_h2d_Delta_mom_vs_truth_eta");


	TCanvas *c0 = new TCanvas("c0","c0");
	//c0->Divide(2,1);

	//c0->cd(1);
	_h2d_Delta_mom_vs_truth_mom->FitSlicesY();
	TH1D *h1d_mom_resolution_vs_truth_mom = (TH1D*)  gDirectory->Get("_h2d_Delta_mom_vs_truth_mom_2");

	h1d_mom_resolution_vs_truth_mom->SetTitle("#sigma_{p}/p; momentum [GeV/c]; #sigma_{p}/p");
	h1d_mom_resolution_vs_truth_mom->SetMarkerStyle(20);
	h1d_mom_resolution_vs_truth_mom->SetMarkerColor(kBlack);
	h1d_mom_resolution_vs_truth_mom->SetLineColor(kBlack);
	h1d_mom_resolution_vs_truth_mom->Draw("e");
	h1d_mom_resolution_vs_truth_mom->GetYaxis()->SetTitleOffset(1.5);

	TF1 *tf_pT_resolution = new TF1("tf_pT_resolution",
			"sqrt([0]*[0] + x*x*[1]*[1])", 0, 40);
	h1d_mom_resolution_vs_truth_mom->Fit(tf_pT_resolution, "s");

	TH1D* h1d_mom_offset_vs_truth_mom = (TH1D*)  gDirectory->Get("_h2d_Delta_mom_vs_truth_mom_1");
	h1d_mom_offset_vs_truth_mom->Draw("e,same");
	h1d_mom_offset_vs_truth_mom->SetMarkerStyle(20);
	h1d_mom_offset_vs_truth_mom->SetMarkerColor(kGray);
	h1d_mom_offset_vs_truth_mom->SetLineColor(kGray);

	h1d_mom_offset_vs_truth_mom->Fit("pol0","s");

	double y_max = h1d_mom_resolution_vs_truth_mom->GetMaximum();
	double y_min = h1d_mom_offset_vs_truth_mom->GetMinimum();

	h1d_mom_resolution_vs_truth_mom->GetYaxis()->SetRangeUser(y_min-0.01, y_max+0.01);

	c0->Update();

	gStyle->SetOptFit();
	gStyle->SetOptStat(000000000);

}

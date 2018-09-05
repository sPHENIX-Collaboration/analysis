//#include "sPhenixStyle.C"
#include "/phenix/u/yuhw/RootMacros/sPHENIXStyle/sPhenixStyle.C"

void plot_ntp_g4hit(
		    //const char* input = "g4svtx_eval.root",
		    const char* input = "/sphenix/user/frawley/pileup/macros/macros/g4simulations/eval_output/g4svtx_eval_1.root_g4svtx_eval.root",
		    const int min_nfromtruth = 30
		){

  //SetsPhenixStyle();
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);
  
  TString good_gtrack_cut = Form("gtrackID>=0&&gnhits>20&&gembed==2");
  TString good_track_cut = Form("gtrackID>=0&&gnhits>20&&gembed==2&&TMath::Abs(dca2d)<0.1&&quality<3&&TMath::Abs(pcaz-gvz)<0.1");
  
  TFile *fout = new TFile("root_files/ntp_track_out.root","recreate");

	TChain* ntp_track = new TChain("ntp_track","reco tracks");
	TChain* ntp_gtrack = new TChain("ntp_gtrack","g4 tracks");
	//TChain* ntp_vertex = new TChain("ntp_vertex","events");
	//TChain *ntp_cluster = new TChain("ntp_cluster","clusters");
	//TChain *ntp_g4hit = new TChain("ntp_g4hit","g4hits");

	for(int i=0;i<2000;i++)
	  {
	    char name[500];
	    // latest files

	    //sprintf(name,"/sphenix/user/frawley/macros_newINTT_newMVTX_aug13/macros/macros/g4simulations/eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);
	    //sprintf(name,"/sphenix/user/frawley/macros_newINTT_newMVTX_aug13/macros/macros/g4simulations/eval_output_100pions_01/g4svtx_eval_%i.root_g4svtx_eval.root",i);
	    //sprintf(name,"/sphenix/user/frawley/macros_newINTT_newMVTX_aug13/macros/macros/g4simulations/eval_output_100pions_101/g4svtx_eval_%i.root_g4svtx_eval.root",i);
	    //sprintf(name,"/sphenix/user/frawley/macros_newINTT_newMVTX_aug13/macros/macros/g4simulations/eval_output_100pions_0111_default/g4svtx_eval_%i.root_g4svtx_eval.root",i);

	    //sprintf(name,"/sphenix/user/frawley/macros_newINTT_newMVTX_aug13/macros/macros/g4simulations/eval_output_100electrons_0111_default/g4svtx_eval_%i.root_g4svtx_eval.root",i);
	    //sprintf(name,"/sphenix/user/frawley/macros_newINTT_newMVTX_aug13/macros/macros/g4simulations/eval_output_100electrons_01/g4svtx_eval_%i.root_g4svtx_eval.root",i);
	    //sprintf(name,"/sphenix/user/frawley/macros_newINTT_newMVTX_aug13/macros/macros/g4simulations/eval_output_100electrons_noINTT/g4svtx_eval_%i.root_g4svtx_eval.root",i);

	    sprintf(name,"/sphenix/user/frawley/dead_map_testing/macros/macros/g4simulations/eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);


	    cout << "Adding file number " << i << " with name " << name << endl;
	    
	    //ntp_cluster->Add(name);
	    //ntp_g4hit->Add(name);
	    ntp_gtrack->Add(name);
	    ntp_track->Add(name);

	  }

	//cout << " g4hit chain entries = " << ntp_g4hit->GetEntries() << endl;
	//cout << " cluster chain entries = " << ntp_cluster->GetEntries() << endl;
	cout << " ntp_track chain entries = " << ntp_track->GetEntries() << endl;


	int nbin = 500;
	double range = 0.3;


	TH2D *h1 = new TH2D("h1","h1",40, 0.0 ,40.0,nbin,-range,range);
	ntp_track->Draw("(pt-gpt)/gpt:gpt>>h1",good_track_cut.Data());
	//h1->FitSlicesY(0,0,-1,0,"qnrl");
	h1->FitSlicesY();
	TH1D*h1_1 = (TH1D*)gDirectory->Get("h1_1");
	TH1D*h1_2 = (TH1D*)gDirectory->Get("h1_2");
	h1_2->Draw("e");
	//h1_1->Draw("same");

	h1_1->SetMarkerStyle(4);
	h1_1->SetMarkerColor(kBlack);
	h1_1->SetLineColor(kBlack);

	h1_2->SetMarkerStyle(20);
	h1_2->SetMarkerColor(kBlack);
	h1_2->SetLineColor(kBlack);
	h1_2->GetYaxis()->SetRangeUser(0, 0.1);
	//h1_2->GetYaxis()->SetRangeUser(0.,0.02);
	h1_2->GetYaxis()->SetTitleOffset(1.5);
	h1_2->SetStats(0);
	h1_2->SetTitle(";p_{T} [GeV/c];#frac{#Delta p_{T}}{p_{T}}");

	h1->Write();
	h1_2->Write();

	TCanvas * c3 = new TCanvas("c3","c3");
	TH1D* h3_den = new TH1D("h3_den","; p_{T}; Efficiency",81, -0.25, 40.25);
	//TH1D* h3_den = new TH1D("h3_den","; p_{T}; eff.",41, -0.5, 40.5);
	TH1D* h3_num = (TH1D*)h3_den->Clone("h3_num");;
	TH1D* h3_eff = (TH1D*)h3_den->Clone("h3_eff");;

	cout<<__LINE__<<": "<< good_track_cut <<endl;
	ntp_gtrack->Draw("gpt>>h3_den",good_gtrack_cut.Data());
	ntp_track->Draw("gpt>>h3_num",good_track_cut.Data());



	for(int i=1;i<=h3_den->GetNbinsX();++i){
		double pass = h3_num->GetBinContent(i);
		double all = h3_den->GetBinContent(i);
		double eff = 0;
		if(all > pass)
			eff = pass/all;
		else if(all > 0)
			eff = 1.;

		//double err = BinomialError(pass, all); 
		h3_eff->SetBinContent(i, eff);
		//h3_eff->SetBinError(i, err);
	}

	//h3_eff->Draw("e,text");
	h3_eff->Draw("p");
	h3_eff->SetStats(0);
	h3_eff->SetTitle("; p_{T} [GeV/c]; eff.");
	h3_eff->SetMarkerStyle(20);
	h3_eff->SetMarkerColor(kBlack);
	h3_eff->SetLineColor(kBlack);
	h3_eff->GetYaxis()->SetRangeUser(0.0, 1.);

	h3_eff->Write();

	TCanvas *c4 = new TCanvas("c4","c4",5,5,800,800);


	TH2D *h2 = new TH2D("h2","h2",160, 0,40,nbin,-0.1,0.1);
	ntp_track->Draw("(dca2d):gpt>>h2",good_track_cut.Data());
	//h2->FitSlicesY(0,0,-1,0,"qnrl");
	h2->FitSlicesY();
	TH1D*h2_1 = (TH1D*)gDirectory->Get("h2_1");
	TH1D*h2_2 = (TH1D*)gDirectory->Get("h2_2");
	h2_2->Draw("e");
	//h2_1->Draw("same");

	h2_1->SetMarkerStyle(4);
	h2_1->SetMarkerColor(kBlack);
	h2_1->SetLineColor(kBlack);

	h2_2->SetMarkerStyle(20);
	h2_2->SetMarkerColor(kBlack);
	h2_2->SetLineColor(kBlack);
	//h2_2->GetYaxis()->SetRangeUser(0, 0.1);
	h2_2->GetYaxis()->SetRangeUser(0.,0.02);
	h2_2->GetYaxis()->SetTitleOffset(1.5);
	h2_2->SetStats(0);
	h2_2->SetTitle(";p_{T} [GeV/c];dca2d (cm)}");

	h2->Write();
	h2_2->Write();


	TCanvas *c5 = new TCanvas("c5","c5",5,5,800,800);

	TH2D *h3 = new TH2D("h3","h3",160, 0, 40, nbin, -0.1, 0.1);
	ntp_track->Draw("(pcaz-gvz):gpt>>h3",good_track_cut.Data());
	//h3->FitSlicesY(0,0,-1,0,"qnrl");
	h3->FitSlicesY();
	TH1D*h3_1 = (TH1D*)gDirectory->Get("h3_1");
	TH1D*h3_2 = (TH1D*)gDirectory->Get("h3_2");
	h3_2->Draw("e");
	//h3_1->Draw("same");

	h3_1->SetMarkerStyle(4);
	h3_1->SetMarkerColor(kBlack);
	h3_1->SetLineColor(kBlack);

	h3_2->SetMarkerStyle(20);
	h3_2->SetMarkerColor(kBlack);
	h3_2->SetLineColor(kBlack);
	//h3_2->GetYaxis()->SetRangeUser(0, 0.1);
	h3_2->GetYaxis()->SetRangeUser(0.,0.1);
	h3_2->GetYaxis()->SetTitleOffset(1.5);
	h3_2->SetStats(0);
	h3_2->SetTitle(";p_{T} [GeV/c];dca2d (cm)}");

	h3->Write();
	h3_2->Write();




}

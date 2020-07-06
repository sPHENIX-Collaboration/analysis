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


  //TString good_gtrack_cut = Form("gtrackID>=0&&gnltpc>20&&gembed==2");
  //TString good_track_cut = Form("gtrackID>=0&&gnltpc>20&&gembed==2&&quality<3 && ntrutpc>20");
  /*
  TString good_gtrack_cut = Form("gtrackID>=0&&gnltpc>20");
  TString good_track_cut = Form("gtrackID>=0&&gnltpc>20&&quality<3 && ntrutpc>20");
  */

  /*
  // all
  TString good_gtrack_cut = Form("gtrackID>=0 && gembed==2");
  TString good_track_cut = Form("gtrackID>=0 && gembed==2");
  */

  // positive
  TString good_gtrack_cut = Form("gtrackID>=0 && gembed==2 && charge > 0");
  TString good_track_cut = Form("gtrackID>=0 && gembed==2 && charge > 0");
  /*
  // negative
  TString good_gtrack_cut = Form("gtrackID>=0 && gembed==2 && charge < 0");
  TString good_track_cut = Form("gtrackID>=0 && gembed==2 && charge < 0");
  */

  TFile *fout = new TFile("root_files/ntp_track_out.root","recreate");

  TChain* ntp_track = new TChain("ntp_track","reco tracks");
  TChain* ntp_gtrack = new TChain("ntp_gtrack","g4 tracks");

  bool use_list = false;
  int n_list = 1000;

  int proc_list[155] = {10,100,102,103,105,106,110,112,113,117,118,119,
			12,120,121,123,125,126,128,
			13,130,131,132,133,134,135,138,14,142,143,144,145,147,148,149,
		       151,154,155,156,157,158,159,160,161,
			162,163,165,167,168,17,170,172,173,174,175,176,177,178,179,
		       18,180,181,184,185,186,187,188,189,19,191,192,193,194,
			196,197,199,2,20,201,203,204,206,207,208,209,211,213,214,216,217,
			218,219,22,220,221,224,227,234,238,239,
			24,26,27,28,30,31,32,34,36,37,38,39,4,40,41,46,47,48,49,
		       50,51,54,55,56,57,60,62,63,65,66,67,68,69,7,70,74,75,77,78,79,8,80,81,
			82,83,84,85,89,9,91,92,93,94,95,96};

  int ifile = 0; 
  int nbadvtx = 0;
  for(int i=0;i<n_list;i++)
    {
      char name[500];
      // latest files
      if(use_list)
	ifile = proc_list[i];
      else
	ifile = i;

     
      // sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/current_repo/macros/macros/g4simulations/acts_tpccovar_10_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/current_repo/macros/macros/g4simulations/acts_tpccovar_20_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/current_repo/macros/macros/g4simulations/acts_tpccovar_4_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/current_repo/macros/macros/g4simulations/acts_rerun_geantino_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);

      //sprintf(name,"/sphenix/user/frawley/current_repo/macros/macros/g4simulations/genfit_rerun_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/current_repo/macros/macros/g4simulations/genfit_geantino_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      sprintf(name,"/sphenix/user/frawley/current_repo/macros/macros/g4simulations/acts_geantino_rerun_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/current_repo/macros/macros/g4simulations/acts_pions_rerun_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);

      // Skip any files where the event vertex was not reconstructed properly
      TChain* ntp_vertex = new TChain("ntp_vertex","events");
      ntp_vertex->Add(name);
      TH1D *hzvtx = new TH1D("hzvtx","hzvtx",500,-10.0,10.0);
      ntp_vertex->Draw("vz - gvz >>hzvtx");
      double meanzdiff =  hzvtx->GetMean() ;
      cout << "   file " << i << " has vertex mean Z difference = " << meanzdiff << endl;
      if( fabs(meanzdiff) >  0.05 ) 
	{
	  cout << "  --- Bad event vertex, skip file number " << i << " with name " << name << endl; 
	  nbadvtx++;
	  delete ntp_vertex;
	  delete hzvtx;
	  continue;
	}
      delete ntp_vertex;
      delete hzvtx;

      cout << "Adding file number " << i << " with name " << name << endl;
	    
      ntp_gtrack->Add(name);
      ntp_track->Add(name);
    }
  cout << " Found " << nbadvtx << " events with badly reconstructed vertices, and skipped them" << endl << endl;

  //cout << " ntp_gtrack chain entries = " << ntp_gtrack->GetEntries() << endl;
  //cout << " ntp_track chain entries = " << ntp_track->GetEntries() << endl;

  TH1D *hch = new TH1D("hch","hch",100,-2,+2);
  TH1D *hgch = new TH1D("hgch","hgch",100,-2,+2);
  ntp_track->Draw("charge>>hch",good_track_cut.Data());
  ntp_gtrack->Draw("charge>>hgch",good_gtrack_cut.Data());
  
  int nbinptres = 80;  // number of bins in pT
  int nbinpteff = 120;  // number of bins in pT
  int nbinptdca = 40;  // number of bins in pT
  int nbin = 200;  // number of bins in Y slice
  double range = 0.4;
  double ptmax = 20.0;

  TH2D *h1 = new TH2D("h1","h1",nbinptres, 0.0 ,ptmax,nbin,-range,range);
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
  h1_2->GetYaxis()->SetRangeUser(0, 0.05);
  h1_2->GetYaxis()->SetTitleOffset(1.5);
  h1_2->SetStats(0);
  h1_2->SetTitle(";p_{T} [GeV/c];#frac{#Delta p_{T}}{p_{T}}");

  h1->Write();
  h1_2->Write();

  TCanvas * c3 = new TCanvas("c3","c3");
  TH1D* h3_den = new TH1D("h3_den","; p_{T}; Efficiency",nbinpteff+2, -0.2, ptmax+0.2);
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

  h3_den->Write();
  h3_num->Write();
  h3_eff->Write();

  TCanvas *c4 = new TCanvas("c4","c4",5,5,800,800);


  TH2D *h2 = new TH2D("h2","h2",nbinptdca, 0,ptmax,nbin,-0.02,0.02);
  //ntp_track->Draw("(dca2d):gpt>>h2",good_track_cut.Data());
  ntp_track->Draw("(pcax-gvx):gpt>>h2",good_track_cut.Data());  // wrong
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
  h2_2->GetYaxis()->SetRangeUser(0.,0.01);
  h2_2->GetYaxis()->SetTitleOffset(1.5);
  h2_2->SetStats(0);
  h2_2->SetTitle(";p_{T} [GeV/c];dca2d (cm)}");

  h2->Write();
  h2_2->Write();


  TCanvas *c5 = new TCanvas("c5","c5",5,5,800,800);

  TH2D *h3 = new TH2D("h3","h3",nbinptdca, 0, ptmax, nbin, -0.02, 0.02);
  ntp_track->Draw("(pcaz-gvz):gpt>>h3",good_track_cut.Data());
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
  h3_2->GetYaxis()->SetRangeUser(0.,0.01);
  h3_2->GetYaxis()->SetTitleOffset(1.5);
  h3_2->SetStats(0);
  h3_2->SetTitle(";p_{T} [GeV/c];dca2d (cm)}");

  h3->Write();
  h3_2->Write();

  TCanvas *c6 = new TCanvas("c6","c6", 5,5,1200, 800);

  TH2D *h6 = new TH2D("h6","h6",nbinptdca, 0, 1.1, nbin, -1.0, 4.0);
  ntp_track->Draw("ntrumaps:gpt>>h6",good_track_cut.Data());
    h6->Draw();
 
  TH1D *h6_1 = new TH1D("h6_1","h6_1",nbin, 0.0, 1.2);
  int binlo = h6->GetYaxis()->FindBin(2.5);
  int binhi = h6->GetYaxis()->FindBin(3.5);
  h6->ProjectionX("h6_1",binlo,binhi);
  h6_1->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  h6_1->SetLineWidth(2.0);
  h6_1->SetLineColor(kBlue);


  TH1D *h6_2 = new TH1D("h6_2","h6_2",nbin, 0.0, 1.2);
  binlo = h6->GetYaxis()->FindBin(-0.5);
  binhi = h6->GetYaxis()->FindBin(0.5);
  h6->ProjectionX("h6_2",binlo,binhi);
  h6_2->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  h6_2->SetLineColor(kRed);
  h6_2->SetLineWidth(2.0);


  h6_1->Draw();
  h6_2->Draw("same");

  TLegend *l = new TLegend(0.15,0.75,0.45,0.85,"","NDC");
  l->SetBorderSize(1);
  l->AddEntry(h6_2,"ntrumaps = 0","l");
  l->AddEntry(h6_1,"ntrumaps = 3","l");
  l->Draw();

  h6->Write();
  h6_1->Write();
  h6_2->Write();

  /*
  TCanvas *c7 = new TCanvas("c7","c7", 5,5,1200, 800);

  TH2D *h7 = new TH2D("h7","h7",nbin, 0, 1.1, nbin, -1.0, 50.0);
  ntp_track->Draw("ntrutpc:gpt>>h7","ntrumaps == 0");
  h7->Draw();
  */
  /* 
  TH1D *h7_1 = new TH1D("h7_1","h7_1",100, 0.0, 1.2);
  h7->ProjectionX("h7_1");
  h7_1->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  h7_1->SetLineWidth(2.0);
  h7_1->SetLineColor(kBlue);
  h7_1->Draw();
  */
  /*
  TH2D *h8 = new TH2D("h8","h8",100, 0, 1.1, 100, -1.0, 50.0);
  ntp_track->Draw("ntrutpc:gpt>>h7","ntrumaps == 3");
  h8->Draw();

  TH1D *h8_1 = new TH1D("h8_1","h8_1",100, 0.0, 1.2);
  h8->ProjectionX("h8_1");
  h8_1->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  h8_1->SetLineColor(kRed);
  h8_1->SetLineWidth(2.0);
  h8_1->Draw("same");

  TLegend *l = new TLegend(0.15,0.75,0.45,0.85,"","NDC");
  l->SetBorderSize(1);
  l->AddEntry(h7_1,"ntrumaps = 0","l");
  l->AddEntry(h8_1,"ntrumaps = 3","l");
  l->Draw();
  */
}

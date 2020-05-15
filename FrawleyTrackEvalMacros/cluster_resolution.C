
#include <TChain.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLegend.h>
#include <TLine.h>
#include <TLatex.h>
#include <TRandom1.h>
#include <TPolyLine.h>
#include  <TColor.h>
#include <iostream>
#include <map>
#include <fstream>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>
#include <string.h>
#include <list>


using namespace std;

//#include "sPhenixStyle.C"

void cluster_resolution()
{

  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);//(0)
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(1); //(0)
  gStyle->SetStatW(0.3);
  gStyle->SetStatH(0.3);

  bool verbose = false;
  int embed_flag = 2; // embedded pions
  
  // Setup parameters
  //=================
  
  // These should match the setup that was simulated!
  int n_maps_layers = 3;
  int n_intt_layers = 6;

  int n_tpc_layers_inner = 16;
  int n_tpc_layers_mid = 16;
  int n_tpc_layers_outer = 16;
  int n_tpc_layers = n_tpc_layers_inner + n_tpc_layers_mid + n_tpc_layers_outer;;
  // This should be inner maps layers (3) + intt(4) + TPC (48)
  int nlayers = n_maps_layers+n_intt_layers+n_tpc_layers;

   double ptmax = 35.0;
  
  int a = 0;
  int b = 0;
  int c = 0;
  
  //===============================
  
  // Open the evaluator output file 
  cout << "Reading ntuple " << endl;
      
  TH2D *rphi = new TH2D("rphi","Cluster map",2000, -79.0, 79.0, 2000, -79.0, 79.0); 
  rphi->GetYaxis()->SetTitle("Y (cm)");
  rphi->GetXaxis()->SetTitle("X (cm)");

  TH2D *zphi = new TH2D("zphi","Cluster z-phi map",120, -120.0, 120.0, 500, -2.0, 5.0); 
  zphi->GetYaxis()->SetTitle("#phi");
  zphi->GetXaxis()->SetTitle("Z (cm)");

  TH2D *delta_rphi = new TH2D("delta_rphi","cluster r#phi errors by layer",60.0, 0.0, 60.0, 2000, -0.1, 0.1); 
  delta_rphi->GetYaxis()->SetTitle("Cluster r#phi Error (cm)");
  delta_rphi->GetXaxis()->SetTitle("Tracking Layer");

 TH2D *delta_phi = new TH2D("delta_phi","cluster #phi errors by layer",60.0, 0.0, 60.0, 2000, -0.002, 0.002); 
  delta_phi->GetYaxis()->SetTitle("Cluster #phi Error (rad)");
  delta_phi->GetXaxis()->SetTitle("Tracking Layer");

 TH2D *delta_phi_gphi = new TH2D("delta_phi_gphi","cluster #phi errors by gphi",2000.0, -0.1, 0.1, 2000, -0.002, 0.002); 
  delta_phi_gphi->GetYaxis()->SetTitle("Cluster #phi Error (rad)");
  delta_phi_gphi->GetXaxis()->SetTitle("Truth #phi");

  TH2D *delta_z = new TH2D("delta_z","cluster Z errors by layer",60.0, 0.0, 60.0, 20000, -1.0, 1.0); 
  delta_z->GetYaxis()->SetTitle("Cluster Z Error (cm)");
  delta_z->GetXaxis()->SetTitle("Tracking Layer");

  TH2D *cluster_size = new TH2D("cluster_size","cluster size by layer",60.0, 0.0, 60.0, 200, 0.0, 15.0); 
  cluster_size->GetYaxis()->SetTitle("Cluster Size (hits)");
  cluster_size->GetXaxis()->SetTitle("Tracking Layer");

  TH2D *clusters_layer = new TH2D("clusters_layer","cluster eta by layer",50.0, 0.0, 50.0, 200, -3.0, 3.0); 
  clusters_layer->GetYaxis()->SetTitle("Cluster eta");
  clusters_layer->GetXaxis()->SetTitle("Tracking Layer");

  TH2D *hzsize = new TH2D("hzsize","cluster Z size vs nparticles",100, 0.0, 6.0, 200, 0.0, 7.0); 
  hzsize->GetXaxis()->SetTitle("Cluster Z Size (cm)");
  hzsize->GetYaxis()->SetTitle("nparticles");

  int pick_layer = 7;

  double nclusters = 0;

  // The condor job output files
  for(int i=0;i <100; i++)
    {
      // Declare all these so that the include file does not cause complaints
      TChain *ntp_vertex = new TChain("ntp_vertex","clusters");
      TChain *ntp_cluster = new TChain("ntp_cluster","clusters");
      TChain *ntp_gtrack = new TChain("ntp_gtrack","clusters");
      TChain *ntp_track = new TChain("ntp_track","clusters");
      TChain *ntp_g4hit = new TChain("ntp_g4hit","clusters");
      
      char name[500];

      // input files 

      sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/screwed_up_tracks_out/g4svtx_eval_%i.root_g4svtx_eval.root",i);
      //sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/test_fudge_06_075_2kevts_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);
      //sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/test_fudge_12_15_2kevts_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);
      //sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/test_fudge_085_105_2kevts_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);
      cout << "Enter file loop with name = " << name << endl;

      // This include file contains the definitions of the ntuple variables
#include "ntuple_variables.C"


    // eliminate events with bad vertices here if needed
      ntp_vertex->Add(name);
      int toss_event = 0;
      for(int cl=0;cl<ntp_vertex->GetEntries();cl++)
	{
	  ntp_vertex->GetEntry(cl);
	  if( fabs(evz-egvz) > 0.1)
	    toss_event++;
	}
      if(toss_event > 0)
	{
	  cout << "    -- bad reco event vertex in one of the events, skip this file. vz = " << evz << " gvz = " << egvz  << endl;
	  delete ntp_vertex;
	  continue;
	}

	  
      ntp_cluster->Add(name);
      

      cout << "        ntp_cluster entries = " << ntp_cluster->GetEntries() << endl;


      for(int p=0;p < ntp_cluster->GetEntries(); p++)
	{
	  ntp_cluster->GetEntry(p);

	  if(cgembed != embed_flag)
	    continue;

	  if(gprimary != 1)
	    continue;

	  tphi = atan(gy/gx);
	  r = sqrt(gx*gx+gy*gy);

	  double dphi = trphi - tphi;
	  double drphi = r * dphi;
	  
	  // Extract the cluster Z resolution
	  double dz = z - gz;
	  
	  //if(drphi > -0.1 && drphi < 0.1)
	    {
	      delta_rphi->Fill( (double) layer, drphi); 
	      delta_phi->Fill( (double) layer, dphi); 
	      delta_z->Fill( (double) layer, dz);
	      if(layer > 38) delta_phi_gphi->Fill(tphi,dphi);
	    }
	}
 
      delete ntp_vertex;
      delete ntp_cluster;
      
    }// for i

  char label[500];

  // for fitting the r-phi cluster distributions
  TF1 *fg = new TF1("fg","gaus(0)",-0.05,0.05);  // restrict range of fit to this
  fg->SetLineColor(kRed);
  fg->SetParameter(0, 100.0);
  fg->SetParameter(1, 0.0);
  fg->SetParameter(2, 2e-02);
  //fg->SetParLimits(2, 0.0, 5e-02);

  TCanvas *c7 = new TCanvas("c7","c7",50,50,1200,800); 
  c7->Divide(3,1);
  int rebin = 1;
  c7->cd(1);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpy3 = new TH1D("hpy3","TPC inner clusters",2000,-0.10, 0.10);
  delta_rphi->ProjectionY("hpy3",n_maps_layers+n_intt_layers+1,n_maps_layers+n_intt_layers+n_tpc_layers_inner);
  hpy3->GetXaxis()->SetRangeUser(-0.04, 0.04);
  hpy3->GetXaxis()->SetNdivisions(506);
  hpy3->GetXaxis()->SetTitle("r#phi cluster error (cm)");
  hpy3->GetXaxis()->SetTitleOffset(1.1);
  hpy3->GetXaxis()->SetTitleSize(0.05);
  hpy3->GetXaxis()->SetLabelSize(0.06);
  hpy3->GetYaxis()->SetLabelSize(0.06);
  hpy3->Rebin(rebin);
  hpy3->Draw();
  hpy3->Fit(fg,"R");
  //double rms3 = 10000 * hpy3->GetRMS();
  double rms3 = fg->GetParameter(2) * 10000.0;
  double rms3_err = fg->GetParError(2) * 10000.0;
  sprintf(label,"RMS %.1f #pm %.1f #mu m",rms3, rms3_err);
  TLatex *l3 = new TLatex(0.45,0.92,label);
  l3->SetNDC(1);
  l3->Draw();

  c7->cd(2);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpy4 = new TH1D("hpy4","TPC mid clusters",2000,-0.10, 0.10);
  delta_rphi->ProjectionY("hpy4",n_maps_layers+n_intt_layers+n_tpc_layers_inner+1,n_maps_layers+n_intt_layers+n_tpc_layers_inner+n_tpc_layers_mid);
  hpy4->GetXaxis()->SetRangeUser(-0.04, 04);
  hpy4->GetXaxis()->SetNdivisions(506);
  hpy4->GetXaxis()->SetTitle("r#phi cluster error (cm)");
  hpy4->GetXaxis()->SetTitleOffset(1.1);
  hpy4->GetXaxis()->SetTitleSize(0.05);
  hpy4->GetXaxis()->SetLabelSize(0.06);
  hpy4->GetYaxis()->SetLabelSize(0.06);
  hpy4->Rebin(rebin);
  hpy4->Draw();
  hpy4->Fit(fg,"R");
  //double rms4 = 10000 * hpy4->GetRMS();
  double rms4 = fg->GetParameter(2) * 10000.0;
  double rms4_err = fg->GetParError(2) * 10000.0;
  sprintf(label,"RMS %.1f #pm %.1f #mu m",rms4, rms4_err);
  TLatex *l4 = new TLatex(0.45,0.92,label);
  l4->SetNDC(1);
  l4->Draw();

  c7->cd(3);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpy5 = new TH1D("hpy5","TPC outer clusters",2000,-0.10, 0.10);
  delta_rphi->ProjectionY("hpy5",n_maps_layers+n_intt_layers+n_tpc_layers_inner+n_tpc_layers_mid+1,n_maps_layers+n_intt_layers+n_tpc_layers_inner+n_tpc_layers_mid+n_tpc_layers_outer);
  //hpy5->GetXaxis()->SetRangeUser(-0.04, 04);
  hpy5->GetXaxis()->SetNdivisions(506);
  hpy5->GetXaxis()->SetTitle("r#phi cluster error (cm)");
  hpy5->GetXaxis()->SetTitleOffset(1.1);
  hpy5->GetXaxis()->SetTitleSize(0.05);
  hpy5->GetXaxis()->SetLabelSize(0.06);
  hpy5->GetYaxis()->SetLabelSize(0.06);
  hpy5->Rebin(rebin);
  hpy5->Draw();
  hpy5->Fit(fg,"R");
  //double rms5 = 10000 * hpy5->GetRMS();
  double rms5 = fg->GetParameter(2) * 10000.0;
  double rms5_err = fg->GetParError(2) * 10000.0;
  sprintf(label,"RMS %.1f #pm %.1f #mu m",rms5, rms5_err);
  TLatex *l5 = new TLatex(0.45,0.92,label);
  l5->SetNDC(1);
  l5->Draw();


  TCanvas *c27 = new TCanvas("c27","c27",50,50,1200,800); 
  c27->Divide(3,2);

  c27->cd(1);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpz1 = new TH1D("hpz1","MVTX clusters Z",500, -1.0, 1.0);
  delta_z->ProjectionY("hpz1",1, n_maps_layers);
  hpz1->GetXaxis()->SetTitle("Z cluster error (cm)");
  hpz1->SetTitleOffset(0.1,"X");
  hpz1->GetXaxis()->SetTitleSize(0.05);
  hpz1->GetXaxis()->SetLabelSize(0.06);
  hpz1->GetYaxis()->SetLabelSize(0.06);
  hpz1->GetXaxis()->SetNdivisions(506);
  hpz1->GetXaxis()->SetTitleOffset(1.1);
  hpz1->GetXaxis()->SetRangeUser(-0.0016, 0.0016);
  hpz1->Draw();
  double zrms1 = 10000.0 * hpz1->GetRMS();
  //char label[500];
  sprintf(label,"RMS %.1f #mu m",zrms1);
  TLatex *lz1 = new TLatex(0.55,0.92,label);
  lz1->SetNDC(1);
  lz1->Draw();

  c27->cd(2);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  //TH1D *hpz2 = new TH1D("hpz2","INTT clusters Z",500, -1.0, 1.0);
  TH1D *hpz2 = new TH1D("hpz2","INTT clusters Z",500, -0.020, 0.020);
  delta_z->ProjectionY("hpz2",n_maps_layers+1,n_maps_layers+1); 
  hpz2->GetXaxis()->SetRangeUser(-0.02, 0.02);
  hpz2->GetXaxis()->SetTitle("Z cluster error (cm)");
  hpz2->GetXaxis()->SetTitleOffset(0.6);
  hpz2->GetXaxis()->SetTitleSize(0.05);
  hpz2->GetXaxis()->SetLabelSize(0.06);
  hpz2->GetYaxis()->SetLabelSize(0.06);
  hpz2->GetXaxis()->SetNdivisions(506);
  hpz2->GetXaxis()->SetTitleOffset(1.1);
  hpz2->Draw();
  double zrms2 = 10000 * hpz2->GetRMS();
  sprintf(label,"RMS %.1f #mu m",zrms2);
  TLatex *lz2 = new TLatex(0.55,0.92,label);
  lz2->SetNDC(1);
  lz2->Draw();

  c27->cd(3);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpz3 = new TH1D("hpz3","TPC inner clusters Z",500,-0.2, 0.2);
  delta_z->ProjectionY("hpz3",n_maps_layers+n_intt_layers+1, n_maps_layers+n_intt_layers+n_tpc_layers_inner);
  hpz3->GetXaxis()->SetNdivisions(506);
  hpz3->GetXaxis()->SetTitle("Z cluster error (cm)");
  hpz3->GetXaxis()->SetTitleOffset(1.1);
  hpz3->GetXaxis()->SetTitleSize(0.05);
  hpz3->GetXaxis()->SetLabelSize(0.06);
  hpz3->GetYaxis()->SetLabelSize(0.06);
  hpz3->GetXaxis()->SetRangeUser(-0.2, 0.2);
  hpz3->Draw();
  double zrms3 = 10000 * hpz3->GetRMS();
  sprintf(label,"RMS %.1f #mu m",zrms3);
  TLatex *lz3 = new TLatex(0.55,0.92,label);
  lz3->SetNDC(1);
  lz3->Draw();

  c27->cd(4);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpz4 = new TH1D("hpz4","TPC mid clusters Z",500,-0.2, 0.2);
  delta_z->ProjectionY("hpz4",n_maps_layers+n_intt_layers+n_tpc_layers_inner+1, n_maps_layers+n_intt_layers+n_tpc_layers_inner+n_tpc_layers_mid);
  hpz4->GetXaxis()->SetNdivisions(506);
  hpz4->GetXaxis()->SetTitle("Z cluster error (cm)");
  hpz4->GetXaxis()->SetTitleOffset(1.1);
  hpz4->GetXaxis()->SetTitleSize(0.05);
  hpz4->GetXaxis()->SetLabelSize(0.06);
  hpz4->GetYaxis()->SetLabelSize(0.06);
  hpz4->GetXaxis()->SetRangeUser(-0.2, 0.2);
  hpz4->Draw();
  double zrms4 = 10000 * hpz4->GetRMS();
  sprintf(label,"RMS %.1f #mu m",zrms4);
  TLatex *lz4 = new TLatex(0.55,0.92,label);
  lz4->SetNDC(1);
  lz4->Draw();

  c27->cd(5);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpz5 = new TH1D("hpz5","TPC outer clusters Z",500,-0.2, 0.2);
  delta_z->ProjectionY("hpz5",n_maps_layers+n_intt_layers+n_tpc_layers_inner+n_tpc_layers_mid+1, n_maps_layers+n_intt_layers+n_tpc_layers_inner+n_tpc_layers_mid+n_tpc_layers_outer);
  hpz5->GetXaxis()->SetNdivisions(506);
  hpz5->GetXaxis()->SetTitle("Z cluster error (cm)");
  hpz5->GetXaxis()->SetTitleOffset(1.1);
  hpz5->GetXaxis()->SetTitleSize(0.05);
  hpz5->GetXaxis()->SetLabelSize(0.06);
  hpz5->GetYaxis()->SetLabelSize(0.06);
  hpz5->GetXaxis()->SetRangeUser(-0.2, 0.2);
  hpz5->Draw();
  double zrms5 = 10000 * hpz5->GetRMS();
  sprintf(label,"RMS %.1f #mu m",zrms5);
  TLatex *lz5 = new TLatex(0.55,0.92,label);
  lz5->SetNDC(1);
  lz5->Draw();



  TCanvas *c6 = new TCanvas("c6","c6",50,50,1200,800); 
  c6->Divide(2,1);
  c6->cd(1);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpy1 = new TH1D("hpy1","MVTX clusters",2000, -0.05, 0.05);
  delta_rphi->ProjectionY("hpy1",1,n_maps_layers);
  //hpy1->GetXaxis()->SetRangeUser(-0.0016, 0.0016);
  //hpy1->GetXaxis()->SetRangeUser(-0.011, 0.011);
  hpy1->GetXaxis()->SetRangeUser(-0.002, 0.002);
  hpy1->GetXaxis()->SetTitle("r#phi cluster error (cm)");
  hpy1->SetTitleOffset(0.1,"X");
  hpy1->GetXaxis()->SetTitleSize(0.05);
  hpy1->GetXaxis()->SetLabelSize(0.06);
  hpy1->GetYaxis()->SetLabelSize(0.06);
  hpy1->GetXaxis()->SetNdivisions(506);
  hpy1->GetXaxis()->SetTitleOffset(1.1);
  hpy1->Draw();
  double rms1 = 10000.0 * hpy1->GetRMS();
  sprintf(label,"RMS %.1f #mu m",rms1);
  TLatex *l1 = new TLatex(0.55,0.92,label);
  l1->SetNDC(1);
  l1->Draw();

  //delta_rphi->Draw();

  c6->cd(2);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpy2 = new TH1D("hpy2","INTT clusters (type 1)",2000, -0.05, 0.05);
  delta_rphi->ProjectionY("hpy2",n_maps_layers+2,n_maps_layers + n_intt_layers); 
  hpy2->GetXaxis()->SetRangeUser(-0.011, 0.011);
  hpy2->GetXaxis()->SetTitle("r#phi cluster error (cm)");
  hpy2->GetXaxis()->SetTitleOffset(0.6);
  hpy2->GetXaxis()->SetTitleSize(0.05);
  hpy2->GetXaxis()->SetLabelSize(0.06);
  hpy2->GetYaxis()->SetLabelSize(0.06);
  hpy2->GetXaxis()->SetNdivisions(506);
  hpy2->GetXaxis()->SetTitleOffset(1.1);
  hpy2->Draw();
  double rms2 = 10000 * hpy2->GetRMS();
  sprintf(label,"RMS %.1f #mu m",rms2);
  TLatex *l2 = new TLatex(0.55,0.92,label);
  l2->SetNDC(1);
  l2->Draw();

  /*
  c6->cd(3);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  TH1D *hpy9 = new TH1D("hpy9","INTT clusters",200, -1.0, 1.0);
  delta_rphi->ProjectionY("hpy9",n_maps_layers+1,n_maps_layers + n_intt_layers-3); 
  hpy9->GetXaxis()->SetRangeUser(-0.011, 0.011);
  hpy9->GetXaxis()->SetTitle("r#phi cluster error (cm)");
  hpy9->GetXaxis()->SetTitleOffset(0.6);
  hpy9->GetXaxis()->SetTitleSize(0.05);
  hpy9->GetXaxis()->SetLabelSize(0.06);
  hpy9->GetYaxis()->SetLabelSize(0.06);
  hpy9->GetXaxis()->SetNdivisions(506);
  hpy9->GetXaxis()->SetTitleOffset(1.1);
  hpy9->Draw();
  double rms9 = 10000 * hpy9->GetRMS();
  sprintf(label,"RMS %.1f #mu m",rms9);
  TLatex *l9 = new TLatex(0.55,0.92,label);
  l9->SetNDC(1);
  l9->Draw();
  */
  /*
  TCanvas *c8 = new TCanvas("C8","C8",50,50,1200,700); 
  c8->Divide(2,1);
  c8->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetGrid();
  delta_rphi->GetYaxis()->SetTitleOffset(1.7);
  delta_rphi->Draw("p");
  c8->cd(2);
  gPad->SetLeftMargin(0.18);
  gPad->SetGrid();
  delta_phi->GetYaxis()->SetTitleOffset(2.3);
  delta_phi->Draw("p");


  TCanvas *c8z = new TCanvas("C8Z","C8Z",50,50,800,800); 
  c8z->SetLeftMargin(0.15);
  delta_z->GetYaxis()->SetTitleOffset(1.7);
  delta_z->Draw("p");

  TCanvas *c9 = new TCanvas("c9","c9",5,5,800,600);
  delta_phi_gphi->Draw();
  */

}

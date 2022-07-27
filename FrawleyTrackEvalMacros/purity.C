#include <TChain.h>
#include <TNtuple.h>
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
#include <iostream>
#include <fstream>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>

using namespace std;
#define use_events

void purity()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);

  int verbose = -1;

  // Setup parameters
  //=================
  int embed_flag = 2;  // set to match value used when events were generated

  // these should match the values used when the files were created
  //int n_maps_layer = 3;
  int n_maps_layer = 0;
  //int n_intt_layer = 4;
  int n_intt_layer = 0;
  int n_tpc_layer = 48; // 16 inner
  int nlayers = n_maps_layer+n_intt_layer+n_tpc_layer;  // maximum number of tracking layers for tpc+intt+maps

  // track cuts
  //static const int nmissed = 20;  // maximum number of missed layers to allow for a track
  double quality_cut = 3.0;
   double dca2d_cut = 0.1;  // cm
  double dcaz_cut = 0.1;  // cm
  double gnhits_cut = 20;
  //double gnhits_cut = 10;
  double truth_hits_cut = 0.8;
 
  // other parameters
  double ptmax = 12.2;  // used for Hijing tracks only
  //double hptmax = 10.0;
  double hptmax = 50.0;
  
  //===============================


  //=======================================
  // define histograms


  TH1D *hnhits = new TH1D("hnhits","hnhits",100,0,99);    
  TH2D *hpt_nfake = new TH2D("hpt_nfake","hpt_nfake",200,0,hptmax, 73, -5, 68.0);
  TH2D *hpt_nmissed_maps_layers = new TH2D("hpt_nmissed_maps_layers","hpt_nmissed_maps_layers",200,0,hptmax, 53, -5, 48.0);
  TH2D *hpt_nmissed_intt_layers = new TH2D("hpt_nmissed_intt_layers","hpt_nmissed_intt_layers",200,0,hptmax, 53, -5, 48.0);
  TH2D *hpt_nmissed_tpc_layers = new TH2D("hpt_nmissed_tpc_layers","hpt_nmissed_tpc_layers",200,0,hptmax, 53, -5, 48.0);
  TH2D *hcorr_nfake_nmaps = new TH2D("hcorr_nfake_nmaps","hcorr_nfake_nmaps",40, -1, 5, 40, -1, 4);
  TH1D *hzevt = new TH1D("hzevt","hzevt",200,-35.0, 35.0);    
  TH1D *hzvtx_res = new TH1D("hzvtx_res","hzvtx_res", 1000, -0.1,  0.1); 
  TH1D *hxvtx_res = new TH1D("hxvtx_res","hxvtx_res", 1000, -0.04,  0.04); 
  TH1D *hyvtx_res = new TH1D("hyvtx_res","hyvtx_res", 1000, -0.04,  0.04); 
  TH1D *hdcavtx_res = new TH1D("hdcavtx_res","hdcavtx_res", 1000, -0.04,  0.04); 

  static const int NPTDCA = 3;

  TH1D *hdca2d[NPTDCA];
  for(int ipt=0;ipt<NPTDCA;ipt++)
    {  
      char hname[500];
      sprintf(hname,"hdca2d%i",ipt);

      hdca2d[ipt] = new TH1D(hname,hname,2000,-0.1,0.1);
      hdca2d[ipt]->GetXaxis()->SetTitle("DCA (cm)");
      hdca2d[ipt]->GetXaxis()->SetTitleSize(0.055);
      hdca2d[ipt]->GetXaxis()->SetLabelSize(0.055);

      hdca2d[ipt]->GetYaxis()->SetTitleSize(0.06);
      hdca2d[ipt]->GetYaxis()->SetLabelSize(0.055);
    }

  TH1D *hdca2dsigma = new TH1D("hdca2dsigma","hdca2dsigma",2000,-5.0,5.0);
  hdca2dsigma->GetXaxis()->SetTitle("dca2d / dca2dsigma");

  TH1D *hZdca = new TH1D("hZdca","hZdca",2000,-0.5,0.5);
  hZdca->GetXaxis()->SetTitle("Z DCA (cm)");

  TH1D *hquality = new TH1D("hquality","hquality",2000,0.0,20.0);
  hquality->GetXaxis()->SetTitle("quality");

  TH1D *hgeta = new TH1D("hgeta","hgeta",100,-1.2,1.2);
  TH1D *hreta = new TH1D("hreta","hreta",100,-1.2,1.2);

  static const int NVARBINS = 36;
  double xbins[NVARBINS+1] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,
			    1.1, 1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,
			    2.2,2.4,2.6,2.8,3.0,3.2,3.4,3.6,3.8,4.0,
			      4.5,5.0,5.5,6.0,7.0,8.0,10.0};

  TH1D *hpt_truth = new TH1D("hpt_truth","hpt_truth",500, 0.0, hptmax);
  TH2D *hpt_compare = new TH2D("hpt_compare","hpt_compare",500, 0.0, hptmax, 2000, 0.0, 2.0);
  hpt_compare->GetXaxis()->SetTitle("p_{T}");
  hpt_compare->GetYaxis()->SetTitle("#Delta p_{T}/p_{T}");

  TH2D *hpt_dca2d = new TH2D("hpt_dca2d","hpt_dca2d",500, 0.0, hptmax, 2000, -0.1, 0.1);
  hpt_dca2d->GetXaxis()->SetTitle("p_{T}");
  hpt_dca2d->GetYaxis()->SetTitle("dca2d");

  TH2D *hpt_dcaZ = new TH2D("hpt_dcaZ","hpt_dcaZ",500, 0.0, hptmax, 2000, -0.1, 0.1);
  hpt_dcaZ->GetXaxis()->SetTitle("p_{T}");
  hpt_dcaZ->GetYaxis()->SetTitle("dca_Z");

  TH1D *hpt_hijing_truth = new TH1D("hpt_hijing_truth","hpt_hijing_truth",500, 0.0, 10.0);
  TH2D *hpt_hijing_compare = new TH2D("hpt_hijing_compare","hpt_hijing_compare",500, 0.0, 10.0, 2000, 0.0, 2.0);
  hpt_hijing_compare->GetXaxis()->SetTitle("p_{T}");
  hpt_hijing_compare->GetYaxis()->SetTitle("#Delta p_{T}/p_{T}");
  TH2D *hpt_hijing_dca2d = new TH2D("hpt_hijing_dca2d","hpt_hijing_dca2d",500, 0.0, 10.0, 2000, -0.1, 0.1);
  TH2D *hpt_hijing_dcaZ = new TH2D("hpt_hijing_dcaZ","hpt_hijing_dcaZ",500, 0.0, 10.0, 2000, -0.1, 0.1);

  TH1D *hptreco[NVARBINS];
  for(int ipt=0;ipt<NVARBINS;ipt++)
    {
      char hn[1000];
      sprintf(hn,"hptreco%i",ipt);
      hptreco[ipt] = new TH1D(hn, hn, NVARBINS, xbins);
    }

  TH2D *hg4ntrack = new TH2D("hg4ntrack","hg4ntrack",200,0,3000.0, 200, 0., 2000);

  // end of histogram definitions
  //===================================================

  //============================================================
  // Loop over events 
  //   Loop over reco'd tracks
  //     Make quality cut
  //       Fill dca2d histos
  //       Make loose dca2d cut (usually < 1 mm)
  //         Fill Z dca histo
  //         Drop tracks outside 1 mm in Z dca from evt vertex
  //           Add this reco'd track 2D histo of (rpT-pT) vs pT
  //============================================================

  // The condor job output files
  for(int i=0;i<1200;i++)
    {
      // Open the evaluator output file 
      
      TChain* ntp_track = new TChain("ntp_track","reco tracks");
      TChain* ntp_gtrack = new TChain("ntp_gtrack","g4 tracks");
      TChain* ntp_vertex = new TChain("ntp_vertex","events");
      TChain *ntp_cluster = new TChain("ntp_cluster","clusters");
      TChain *ntp_g4hit = new TChain("ntp_g4hit","g4hits");
      
      // This include file contains the definitions of the ntuple variables, and the chain definitions
#include "ntuple_variables.C"

      char name[1000];
      // latest files
      sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output/g4svtx_eval_%i.root_g4svtx_eval.root_primary_eval.root",i);

      // for CD1 plots with no MVTX
      //sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output_noINTT_80ns_100pions_clusterizer_fixed/g4svtx_eval_%i.root_g4svtx_eval.root_primary_eval.root",i);
      //sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output_noINTT_80ns_100pions_clusterizer_fixed/g4svtx_eval_%i.root_g4svtx_eval.root",i);

      //sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output_ups1s_100pions_80ns/g4svtx_eval_%i.root_g4svtx_eval.root_primary_eval.root",i);


      bool checkfiles = false;
      int mintracks = 10;
      if(checkfiles)
	{
	  // some QA on the input files
	  TFile*f=TFile::Open(name);
	  if(!f)
	    continue;
	  TNtuple*test_ntp_track=(TNtuple*)f->Get("ntp_track");
	  if(!test_ntp_track)
	    {
	      f->Close();
	      continue;
	    }
	  if(test_ntp_track->GetEntries() < mintracks)
	    continue;
	  
	  f->Close();
	}

      cout << "Adding file number " << i << " with name " << name << endl;

      ntp_vertex->Add(name);
      ntp_track->Add(name);
      ntp_gtrack->Add(name);



      // Use the number of g4 tracks (with some cuts) as a measure of centrality
      // Examination of the output file shows that the 10% most central events have 1700 or more g4 tracks with these cuts
      double ng4tr = 0;
      for(int ig = 0;ig < ntp_gtrack->GetEntries();ig++)
	{
	  int recoget = ntp_gtrack->GetEntry(ig);
	  if(tembed == 0 && tprimary == 1 && fabs(teta) < 1.0 && sqrt(tpx*tpx+tpy*tpy) > 0.2)
	    ng4tr++;
	}
 
      double nrtr = 0;
      for(int ir = 0;ir < ntp_gtrack->GetEntries();ir++)
	{
	  int recoget = ntp_track->GetEntry(ir);
	  double reta = asinh(rpz/rpt);
	  if(rgembed == 0 && rprimary == 1 && fabs(reta) < 1.0 && rpt > 0.2)
	    nrtr++;
	}
      hg4ntrack->Fill(ng4tr, nrtr);

      //if(ng4tr < 1250)  // keep 0-20%
      //if(ng4tr > 700)  // keep 60-100%
      if(ng4tr > 10000) // keep 0-100%
	{
	  delete ntp_gtrack;
	  delete ntp_track;
	  delete ntp_cluster;
	  delete ntp_vertex;	  
	  continue;
	}
      if(verbose> -1)  
	cout 
	  <<  " ntp_vertex entries: " << ntp_vertex->GetEntries()
	     << " ntp_gtrack entries: " << ntp_gtrack->GetEntries()
	     << " ntp_track entries: " << ntp_track->GetEntries()
	     << endl;

      //==================================
      // Begin event loop
      //==================================
      
      // These keep track of the starting position in the track ntuples for each event
      int nr = 0;
      int ng = 0;
      int nev = ntp_vertex->GetEntries();
      //int nev = 1;

      for(int iev=0;iev<nev;iev++)
	{

	  if(verbose > 0) cout << " iev = " << iev << " ng " << ng << " nr " << nr << endl;  
	 
	  int recoget = ntp_vertex->GetEntry(iev);
	  if(!recoget)
	    {
	      cout << "Failed to get ntp_vertex entry " << iev << endl;
	      continue;
	    }	    

	  /*
	  if(egvt != 0)
	    {
	      // this is a pileup event, we want only the triggered event, so skip it
	      // we need to advance the pointers to the track ntuples
	      if(isnan(ntracks))
		ntracks  = 0;
	      nr += ntracks;
	      ng += ngtracks;

	      if(verbose>0)
		cout << "   skip pileup event with ngtracks = " << ngtracks << " and ntracks = " << ntracks 
		     << " advance ng to " << ng << " and nr to " << nr  
		     << endl; 

	      continue;
	    }
	  */

	  if(iev%1 == 0)	   
	    if(verbose>0) 
	      cout << "Get event " << iev 
		   << " with vertex x " << evx
		   << " vertex y " << evy << " vertex z " << evz 
		   << " egvz " << egvz
		   << " ngtracks " << ngtracks << " ntracks " << ntracks 
		   << " gtarcks start at " << ng << " ntracks start at " << nr 
		   << endl;

	  if(fabs(evz - egvz) > 0.1)
	    {
	      cout << "     ALERT! event vertex is not correct: egvz = " << egvz << " evz = " << evz << " evx = " << evx << " evy = " << evy << endl;
	      continue;
	    }


	  // do not consider events that are not within the full acceptance
	  if(fabs(egvz) > 10.0)
	    {
	      cout << " Event vertex is outside 10 cm, reject this event - egvz = " << egvz << endl;
	      if(isnan(ntracks))
		ntracks  = 0;
	      nr += ntracks;
	      ng += ngtracks;
	      continue;
	    }

	  hzevt->Fill(evz);
	  hzvtx_res->Fill(evz-egvz);
	  hxvtx_res->Fill(evx-egvx);
	  hyvtx_res->Fill(evy-egvy);
	  hdcavtx_res->Fill(sqrt(evx*evx+evy*evy) - sqrt(egvx*egvx+egvy*egvy));
	 	 
	  //====================================================
	  // ntp_gtracks
	  //====================================================
	  // ngtracks is defined in ntuple_variables.C and is the number of g4 tracks
	  // ntracks is defined in ntuple_variables.C and is the number of reco'd tracks
	
	  if(verbose > 0) cout << "Process truth tracks:" << endl;

	  int n_embed_gtrack = 0;            
	  //for(int ig=ng;ig<ng+ngtracks;ig++)
	  for(int ig=ng;ig<ntp_gtrack->GetEntries();ig++)
	    {
	      int recoget = ntp_gtrack->GetEntry(ig);
	      if(recoget == 0)
		{
		  //if(verbose) cout << "Failed to get ntp_gtrack entry " << ig << " in ntp_gtrack" << endl;
		  cout << "Failed to get ntp_gtrack entry " << ig << " in ntp_gtrack" << endl;
		  break;
		}

	      //if(tembed != embed_flag)
	      //	continue;

	      /*
	      // if the scan_for_embedded flag is set to true, the number of reco tracks recorded in ntp_vertex will be all reco'd tracks, but only embedded tracks will be present
	      // check for change of event number to detect this
	      if(tevent != iev)
		{
		  if(verbose > 0) cout << "Change of event number from " << iev << " to " << tevent <<  " go  to next event" << endl;
		  ngtracks = ig - ng;
		  break;
		}
	     	     
	      if(verbose > 0) cout << " ig = " << ig << " tevent " << tevent << " tgtrackid " << tgtrackid 
				   << " ttrackid " << ttrackid << " tgnhits " << tgnhits << " tnhits " << tnhits << " tnfromtruth " << tnfromtruth << " tembed " << tembed  << endl;
	      */

	      // ntp_gtrack track cuts
	      //===================
	      // skip tracks that do not pass through enough layers (judged using truth track value of nhits)
	      if(tgnhits <= gnhits_cut)
		{
		  if(verbose>0) cout << "  -------- failed gnhits cut " << endl;
		  continue;
		}
	      
	      if(tnfromtruth / tnhits <= truth_hits_cut)
		{
		  if(verbose>0) cout << " -------  failed nfromtruth/nhits cut " << endl;
		  continue;
		}
	      if(tgtrackid < 0)
		{
		  if(verbose>0) cout << "  ------- failed tgtrackid cut " << endl;
		  continue;
		}
	      
	      if(tquality > quality_cut)
		{
		  if(verbose > 0) cout << "   --------  failed quality cut - rejected " << endl;
		  continue;
		}
	      
	      if(fabs(trdca3dz) > dcaz_cut)
		{	      
		  if(verbose>0) cout << "   --------  skip because track failed z vertex cut with dca3dz  = " << trdca3dz << endl;
		  continue;
		}
	      
	      if(trdca2d > dca2d_cut)
		{
		  if(verbose>0) cout << "  -------- skip because failed dca2d cut, rdca2d =  " << rdca2d << endl;
		  continue;
		}
	      // get the truth pT
	      double tgpT = sqrt(tpx*tpx+tpy*tpy);	  	  
	      
	      if(tembed == embed_flag)  // take only embedded pions
		{
		  double geta = asinh(tpz/sqrt(tpx*tpx+tpy*tpy));
		  hgeta->Fill(geta); // optional cut
		  if(fabs(geta) < 1.0)
		    {
		      n_embed_gtrack++;
		      //cout << "fill htp_truth with tgpT = " << tgpT << endl;
		      hpt_truth->Fill(tgpT);

		      // Capture hpt_compare from ntp_gtrack
		      hpt_compare->Fill(tpt,trpt/tpt);
		      hpt_dca2d->Fill(tpt, trdca2d);
		      hpt_dcaZ->Fill(tpt, trdca3dz);
		    }
		}
	     
	      // record embedded pions and Hijing tracks separately
	      if(tembed == 0)
		{
		  hpt_hijing_truth->Fill(tgpT);
		}
	     
	    } // end loop over truth tracks
	  if(verbose>0) cout << "n_embed_gtrack = " << n_embed_gtrack << endl;
	  //===================================================


	  //====================================================
	  // ntp_tracks
	  //====================================================
	 
	  if(verbose > 0) cout << "Process reco tracks:" << endl;
	 
	  int n_embed_rtrack = 0;                  
	  int naccept = 0;
	  //for(int ir=nr;ir<nr+ntracks;ir++)
	  for(int ir=nr;ir<ntp_track->GetEntries();ir++)
	    {
	      int recoget = ntp_track->GetEntry(ir);
	      if(!recoget)
		{
		  if(verbose>0) cout << "Failed to get ntp_track entry " << ir << endl;
		  break;
		}	    

	      if(rgembed != embed_flag)
		continue;

	      /*
	      // if the scan_for_embedded flag is set to true, the number of reco tracks recorded in ntp_vertex will be all reco'd tracks, but only embedded tracks will be present
	      // check for change of event number to detect this
	      if(revent != iev)
		{
		  if(verbose > 0) cout << "Change of event number from " << iev << " to " << revent <<  " go  to next event" << endl;
		  ntracks = ir - nr;
		  break;
		}

	      if(verbose > 0) cout << " ir = " << ir << " rtrackid " << rtrackid << " revent " << revent 
				   << " rquality " << rquality << "  rgtrackid = " << rgtrackid << " rgnhits " << rgnhits 
				   << " rnhits " << rnhits << " rnfromtruth " << rnfromtruth << " rgembed " << rgembed << endl;
	      */

	      //  ntp_track track cuts 
	      //================================
	      if(rgnhits <= gnhits_cut)
		{
		  //skip tracks that do not pass through enough layers in truth, same cut made on truth tracks earlier
		  cout << "   -------- skip because rgnhits too small " << endl;
		  continue;
		}
	      
	      if(rnfromtruth / rnhits <= truth_hits_cut)
		{
		  if(verbose>0) cout << " -------  failed rnfromtruth/rnhits cut " << endl;
		  continue;
		}

	      if(rgtrackid < 0)
		{
		  if(verbose>0) cout << " -------  failed rgtrackid < 0 cut " << endl;
		  continue;
		}	      
	      if(rquality > quality_cut)
		{
		  if(verbose > 0) cout << "   --------  failed quality cut - rejected " << endl;
		  continue;
		}
	     
	      if(fabs(rpcaz - rvz) > dcaz_cut)
		{	      
		  if(verbose>0) cout << "   --------  skip because track " << ir <<  " failed z vertex cut with rvz = " << rpcaz << " gvz = " << rvz << endl;
		  continue;
		}
	     
	      if(rdca2d > dca2d_cut)
		{
		  if(verbose>0) cout << "  -------- skip because failed dca2d cut, rdca2d =  " << rdca2d << endl;
		  continue;
		}

	      /*
	      if(rnmaps != n_maps_layer)
		{
		  if(verbose>0) cout << "skip track " << ir << " because nmaps = " << rnmaps << endl;
		  continue;
		}
	      */

	      //=============================

	      double rdcaZ = rpcaz - rvz;

	      if(rgembed == embed_flag)  // take only embedded pions
		{		  
		  // get the quality and Zdca histos before vertex cuts 
		  hquality->Fill(rquality);		 
		  hZdca->Fill(rpcaz - rvz);
		  
		  if(verbose > 0) cout << "   accepted: rgembed = " << rgembed << " rgpt = " << rgpt << endl;
		  
		  naccept++;

		  if(verbose > 0) cout << " rdca2d = " << rdca2d << " rdcaZ = " << rdcaZ << endl;

		  double reta = asinh(rpz/rpt);
		  hreta->Fill(reta);
		  if(fabs(reta) < 1.0)  // optional cut
		    {
		      n_embed_rtrack++;
		      //hpt_compare->Fill(rgpt,rpt/rgpt);
		      if(rnmaps == n_maps_layer)  // require all maps layers
			{
			  //hpt_dca2d->Fill(rgpt, rdca2d);
			  //hpt_dcaZ->Fill(rgpt,  rdcaZ);
			}
		    }
		  hnhits->Fill(rnhits);
		  
		  double nfake = rnhits - rnfromtruth;
		  hpt_nfake->Fill(rgpt, nfake);

		  double nmissed_maps_layers = rgnlmaps - rnlmaps;
		  hpt_nmissed_maps_layers->Fill(rgpt,nmissed_maps_layers);

		  double nmissed_intt_layers = rgnlintt - rnlintt;
		  hpt_nmissed_intt_layers->Fill(rgpt,nmissed_intt_layers);


		  double nmissed_tpc_layers = rgnltpc - rnltpc;
		  hpt_nmissed_tpc_layers->Fill(rgpt,nmissed_tpc_layers);

		  hcorr_nfake_nmaps->Fill(nfake,nmissed_maps_layers);
		} 
	      	      
	      if(rgembed == 1)
		{
		  // for the non-embedded tracks from Hijing, we want to be able to do pt_sigma cuts later	  
		  hpt_hijing_compare->Fill(rgpt,rpt/rgpt);
		  if(rnmaps == n_maps_layer) // require all maps layers
		    {
		      hpt_hijing_dca2d->Fill(rgpt,rdca2d);
		      hpt_hijing_dcaZ->Fill(rgpt,rdcaZ);
		    }

		  for(int ipt=0;ipt<NVARBINS-1;ipt++)
		    {
		      if(rpt > xbins[ipt] && rpt < xbins[ipt+1])
			hptreco[ipt]->Fill(rpt);
		    }
		  
		  // Add to the 3 panel  dca2d histos
		  if(rgpt > 0.5 && rgpt <= 1.0)  hdca2d[0]->Fill(rdca2d);
		  if(rgpt > 1.0 && rgpt <= 2.0)   hdca2d[1]->Fill(rdca2d);
		  if(rgpt > 2.0)  hdca2d[2]->Fill(rdca2d);
		}
	      
	    }  // end loop over reco'd tracks
	  if(verbose > 0)  cout << " Done with loop: n_embed_gtrack = " << n_embed_gtrack << " n_embed_rtrack = " << n_embed_rtrack << " naccept = " << naccept << endl;
	  //====================================================
	  
	  // set the starting positions in the track ntuples for the next event 
	  nr += ntracks;
	  ng += ngtracks;
	  if(verbose > 0) cout << "  embedded tracks this event = " <<  n_embed_gtrack << " accepted tracks this event = " << naccept << endl;
	}  // end loop over events
      
      
      delete ntp_gtrack;
      delete ntp_track;
      delete ntp_cluster;
      delete ntp_vertex;
      
    } // end loop over files

  //==============
  // Output the results

  TFile *fout = new TFile("root_files/purity_out.root","recreate");

  hnhits->Write();

  // Three dca2d histos made from Hijing tracks
  hdca2d[0]->Write();
  hdca2d[1]->Write();
  hdca2d[2]->Write();

  // truth pT distributions for embedded and non-embedded particles
  hpt_truth->Write();
  hpt_hijing_truth->Write();
  
  // 2D histos made with embedded pions
  hpt_compare->Write();
  hpt_dca2d->Write();
  hpt_dcaZ->Write();
  hpt_nfake->Write();
  hpt_nmissed_maps_layers->Write();
  hpt_nmissed_intt_layers->Write();
  hpt_nmissed_tpc_layers->Write();
  hcorr_nfake_nmaps->Write();

  // 2d histo made with hijing tracks only
  hpt_hijing_compare->Write();
  hpt_hijing_dca2d->Write();
  hpt_hijing_dcaZ->Write();
  
  // efficiency plot made with Hijing tracks
  for(int ipt=0;ipt<NVARBINS;ipt++)
    {
      hptreco[ipt]->Write();
    }
  
  hquality->Write();
  hZdca->Write();

  hzevt->Write();
  hzvtx_res->Write();
  hxvtx_res->Write();
  hyvtx_res->Write();
  hdcavtx_res->Write();

  hreta->Write();
  hgeta->Write();
  hg4ntrack->Write();
  
  fout->Close();
  
  
}


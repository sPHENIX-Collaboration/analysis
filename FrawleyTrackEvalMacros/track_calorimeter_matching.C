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
#include <iostream>
#include <fstream>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>

void track_calorimeter_matching()
{
  int verbose = 2;

  int n_maps_layer = 3;
  int n_intt_layer = 4;
  int n_tpc_layer = 40;

  int nlayers = n_maps_layer+n_intt_layer+n_tpc_layer;  // maximum number of tracking layers for tpc+intt+maps
  static const int nmissed = 30;  // maximum number of missed layers to allow for a track

  int embed_flag = 1;

 double quality_cut = 3.0;
  double dca_cut = 0.1; //  cm

  TH2D * hEoverP_cemc[2];
  hEoverP_cemc[0] = new TH2D("hEoverP0_cemc","",200, 0, 20, 200, 0, 2.0);
  hEoverP_cemc[0]->SetMarkerStyle(20);
  hEoverP_cemc[0]->SetMarkerSize(0.5);
  hEoverP_cemc[1] = new TH2D("hEoverP1_cemc","",200, 0, 20, 200, 0, 2.0);
  hEoverP_cemc[1]->SetMarkerStyle(20);
  hEoverP_cemc[1]->SetMarkerSize(0.5);


  int n_embed_rtrack = 0;                  
  int naccept = 0;                  

 // The condor job output files
  for(int i=0;i<1000;i++)
    //for(int i=0;i<2;i++)
    {
      // Open the evaluator output file 
      
      TChain* ntp_track = new TChain("ntp_track","reco tracks");
      TChain* ntp_gtrack = new TChain("ntp_gtrack","g4 tracks");
      TChain* ntp_vertex = new TChain("ntp_vertex","events");
      TChain *ntp_cluster = new TChain("ntp_cluster","clusters");
      
      // This include file contains the definitions of the ntuple variables, and the chain definitions
#include "ntuple_variables.C"
      
      char name[500];
      sprintf(name,"/sphenix/user/frawley/latest/macros/macros/g4simulations/eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);
      
      cout << "Adding file number " << i << " with name " << name << endl;
      
      ntp_vertex->Add(name);
      ntp_track->Add(name);
      ntp_gtrack->Add(name);
      
      // skip this file if there are no tracks 
      if(!ntp_gtrack->GetEntries())
	continue;
      
      if(verbose> 0)  
	cout <<  " ntp_vertex entries: " << ntp_vertex->GetEntries()
	     << " ntp_gtrack entries: " << ntp_gtrack->GetEntries()
	     << " ntp_track entries: " << ntp_track->GetEntries()
	     << endl;
      
      //==================================
      // Begin event loop
      //==================================
      
      // These keep track of the starting position in the track ntuples for each event
      int nr = 0;
      //int ng = 0;
      
      for(int iev=0;iev<ntp_vertex->GetEntries();iev++)
	{
	  if(verbose) cout << " iev = " << iev << " nr " << nr << endl;  
	  
	  int recoget = ntp_vertex->GetEntry(iev);
	  if(!recoget)
	    {
	      cout << "Failed to get ntp_vertex entry " << iev << endl;
	      exit(1);
	    }	    
	  if(iev%1 == 0)	   
	    if(verbose) 
	      cout << "Get event " << iev << " with vertex x " << evx
		   << " vertex y " << evy << " vertex z " << evz 
		   << " ngtracks " << ngtracks << " ntracks " << ntracks 
		   << endl;
	  
	  // loop over tracks
	  
	  for(int ir=nr;ir<nr+ntracks;ir++)
	    {
	      int recoget = ntp_track->GetEntry(ir);
	      if(!recoget)
		{
		  if(verbose) cout << "Failed to get ntp_track entry " << ir << endl;
		  break;
		}	    
	      if(verbose > 0) cout << " ir = " << ir << " rtrackid " << rtrackid << " revent " << revent 
				   << " rquality " << rquality << "  rgtrackid = " << rgtrackid << " rgnhits " << rgnhits 
				   << " rnhits " << rnhits << " rnfromtruth " << rnfromtruth << endl;
	      
	      // Make some overall reconstructed track cuts
	      if(rgnhits < nlayers-nmissed)
		{
		  //skip tracks that do not pass through enough layers in truth, same cut made on truth tracks earlier
		  cout << "   --- skip because rgnhits too small " << endl;
		  continue;
		}
	      
	      if(rquality > quality_cut)
		{
		  if(verbose > 0) cout << "   --- failed quality cut - rejected " << endl;
		  continue;
		}
	      
	      if(fabs(rvz - evz) > 0.1)
		{	      
		  if(verbose) cout << "skip because failed z vertex cut with rvz = " << rvz << " evz = " << evz << endl;
		  continue;
		}
	      
	      if( isnan(rdca2d) )
		{
		  if(verbose) cout << "skip because dca2d is nan" << endl;
		  continue;		 
		}
	      
	      if(rdca2d > 0.1)
		{
		  if(verbose) cout << "skip because failed dca2d cut " << endl;
		  continue;
		}
	      
	      //double rgpT = sqrt(rgpx*rgpx+rgpy*rgpy);	  	  
	      //double rpT = sqrt(rpx*rpx+rpy*rpy);
	      
	      if(rgembed == embed_flag)
		{
		  n_embed_rtrack++;
		  naccept++;

		  double reta = asinh(rpz/sqrt(rpx*rpx+rpy*rpy));
		  double rmom = sqrt(rpx*rpx+rpy*rpy+rpz*rpz);
		  double EoverP_cemc = cemc_e / rmom;
		  cout << "        reta " << reta << " rmom " << rmom << " EoverP " << EoverP_cemc << endl;		  
		  
		  if(rgflavor ==11)
		    hEoverP_cemc[0]->Fill(rmom, EoverP_cemc);
		  else
		    hEoverP_cemc[1]->Fill(rmom, EoverP_cemc);
		  
		} 
	      
	    }  // end loop over reco'd tracks
	  if(verbose > 0)  cout << " Done with tracks loop: n_embed_rtrack = " << n_embed_rtrack << " naccept = " << naccept << endl;
	  //====================================================
	  
	  // set the starting positions in the track ntuples for the next event 
	  nr += ntracks;
	  //ng += ngtracks;
	}  // end loop over events
      
      
      delete ntp_gtrack;
      delete ntp_track;
      delete ntp_cluster;
      delete ntp_vertex;
      
    } // end loop over files
  
  TCanvas *cep_cemc = new TCanvas("cep_cemc","",5,5,800,500);
  cep_cemc->Divide(2,1);

  cep_cemc->cd(1);
  hEoverP_cemc[0]->Draw();
  
  cep_cemc->cd(2);
  hEoverP_cemc[1]->Draw();

  TCanvas *cproj_cemc = new TCanvas("cproj_cemc","",20,20,1400,800);
  cproj_cemc->Divide(3,2);

  double dp = 2.0;
  int np = 12.0 / dp;
  for(int ip=0;ip<np;ip++)
    {
      cproj_cemc->cd(ip+1);

      double plo = ip * dp;
      double phi = ip * dp + dp;
      int binlo = hEoverP_cemc[0]->GetXaxis()->FindBin(plo);
      int binhi = hEoverP_cemc[0]->GetXaxis()->FindBin(phi);

      char label[500];
      sprintf(label,"p = %.1f  - %.1f GeV/c",plo, phi);
      TH1D *h = new TH1D("h",label, 200, 0 , 2.0);
      hEoverP_cemc[0]->ProjectionY("h",binlo,binhi);
      h->GetXaxis()->SetTitle("p_{T} (GeV/c)");
      h->GetXaxis()->SetTitle("E/p for CEMC");
      h->GetXaxis()->SetTitleOffset(1.0);

      h->DrawCopy();

      delete h;
    }


}

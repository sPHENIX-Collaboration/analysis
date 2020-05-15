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
//#include <iomanip.h>
#include <TLorentzVector.h>

//#define TEST

void quarkonia_reconstruction_embedded()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(1);
  gStyle->SetOptTitle(1);

  int verbose = 0;
  int embed_flag = 3;  // embedding flag used during Upsilon generation

  // track cuts  
  double quality_cut = 3.0;
  double dca_cut = 0.1;

  char lepton[100];
  sprintf(lepton,"electron");
  
  double decaymass=0.000511;
  cout << "Assuming decay particle mass is " << decaymass << endl;

  // Define some histograms
  
  int nbpt = 20;
  double ptmax = 10.0;

  TH1F* hrquality = new TH1F("hrquality", "Reconstructed track Quality", 1000, 0, 10);
  TH1F* hrdca2d = new TH1F("hrdca2d", "Reconstructed track dca2d", 1000, 0, 0.05);
  TH1F* hrpt = new TH1F("hrpt"," pT", nbpt, 0.0, ptmax);
  TH1F* hgpt = new TH1F("hgpt","g4 pT", nbpt, 0.0, ptmax);

  TH1D *g4mass = new TH1D("g4mass","G4 input invariant mass",200,7.0,11.0);
  g4mass->GetXaxis()->SetTitle("invariant mass (GeV/c^{2})");
  TH1D *g4mass_primary = new TH1D("g4mass_primary","G4 input invariant mass",200,7.0,11.0);
  g4mass_primary->GetXaxis()->SetTitle("invariant mass (GeV/c^{2})");

  TH1D *recomass = new TH1D("recomass","Reconstructed invariant mass",200,7.0,11.0);
  recomass->GetXaxis()->SetTitle("invariant mass (GeV/c^{2})");
  TH1D *recomass_primary = new TH1D("recomass_primary","Reconstructed invariant mass",200,7.0,11.0);
  recomass_primary->GetXaxis()->SetTitle("invariant mass (GeV/c^{2})");

  int ups_state = 1;   // used in naming of output files
  //int ups_state = 2;   // used in naming of output files
  //int ups_state = 3;   // used in naming of output files

  // Number of upsilons to process - quit after this number is reached
  int nups_requested = 0;
  double pair_acc = 0.60 * 0.9 * 0.98;  // fraction into 1 unit of rapidity * pair eID fraction * trigger fraction 
  // These values are for 1 year of pp running
  if(ups_state == 1)  nups_requested = pair_acc * (8769 * 1.126) / (0.38 * 0.9 * 0.98);  // pair_acc * yield for 197 /pb (= 15590)
  if(ups_state == 2)  nups_requested = pair_acc * (2205 * 1.126) / (0.38 * 0.9 * 0.98); //  pair_acc * yield for 197 /pb (= 3920)
  if(ups_state == 3)  nups_requested = pair_acc * (1156 * 1.126) / (0.38 * 0.9 * 0.98); //  pair_acc * yield for 197 /pb (=2055) 

  nups_requested = 100000;  // take them all
 
  cout << "ups_state = " << ups_state << " Upsilons requested = " << nups_requested << endl;

  int nrecog4mass = 0;
  int nrecormass = 0;
  
  // Open the g4 evaluator output file
  cout << "Reading electron ntuples " << endl; 
  int nevents = 0;
  double nhittpcin_cum = 0;
  double nhittpcin_wt = 0;

    // The condor job output files -  we open them one at a time and process them
  for(int i=0;i<2000;i++)
    {
      if(nrecormass > nups_requested)
	{
	  cout << "Reached requested number of reco Upsilons, quit out of file loop" << endl;
	  break;
	}
      
      char name[500];

      //sprintf(name,"/sphenix/user/frawley/macros_newTPC_june6/macros/macros/g4simulations/intt_6layers_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);
      //sprintf(name,"/sphenix/user/frawley/macros_newTPC_june6/macros/macros/g4simulations/intt_4layers_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);
      //sprintf(name,"/sphenix/user/frawley/macros_newTPC_june6/macros/macros/g4simulations/intt_8layers_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);
      sprintf(name,"/sphenix/user/frawley/macros_newTPC_june6/macros/macros/g4simulations/intt_0layers_eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",i);


      /*      
      // ups states, 100 poins, 80 ns
      if(ups_state == 1)
	{
	  //sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output_ups1s_100pions_80ns/g4svtx_eval_%i.root_g4svtx_eval.root_primary_eval.root",i);
	  //sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output_ups1s_100pions_massres_121_30kevts/g4svtx_eval_%i.root_g4svtx_eval.root_primary_eval.root",i);
	}
      else if(ups_state == 2)
	{
	  //sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output_ups2s_100pions_80ns/g4svtx_eval_%i.root_g4svtx_eval.root_primary_eval.root",i);
	  sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output_ups2s_100pions_massres_121_20kevts/g4svtx_eval_%i.root_g4svtx_eval.root_primary_eval.root",i);
	}
      else if(ups_state == 3)
	{
	  //sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output_ups3s_100pions_80ns/g4svtx_eval_%i.root_g4svtx_eval.root_primary_eval.root",i);
	  sprintf(name,"/sphenix/user/frawley/fresh_mar8_testing/macros/macros/g4simulations/eval_output_ups3s_100pions_massres_121_20kevts/g4svtx_eval_%i.root_g4svtx_eval.root_primary_eval.root",i);
	}
      else
	{
	  cout << "Oops!" << endl;
	  exit(1);
	}
      */


      cout << "Adding file " << name << endl;

      TChain* ntp_track = new TChain("ntp_track","reco tracks");
      TChain* ntp_gtrack = new TChain("ntp_gtrack","g4 tracks");
      TChain* ntp_vertex = new TChain("ntp_vertex","events");
      TChain *ntp_cluster = new TChain("ntp_cluster","clusters");
      TChain *ntp_g4hit = new TChain("ntp_g4hit","G4 hits");
      
      ntp_vertex->Add(name);
      ntp_track->Add(name);
      ntp_gtrack->Add(name);


      cout << "The ntuples contain " << ntp_vertex->GetEntries() << " events " << endl;


      // Ntuple access variables
      // This include file contains the definitions of the ntuple variables                                                                        
#include "ntuple_variables.C"
    
      //=======================
      // Loop over events
      //=======================

      ntracks = ntp_track->GetEntries();
      ngtracks = ntp_gtrack->GetEntries();

      int nr = 0;
      int ng = 0;
      //int nev = 1;
      int nev = ntp_vertex->GetEntries();

      for(int iev=0;iev<nev;iev++)
	{
	  // drop out when the requested number of reco'd Upsilons has been reached
	  if(nrecormass > nups_requested)
	    {
	      cout << "Reached requested number of reco Upsilons, quit" << endl;

	      break;
	    }

	  nevents++;
      
	  int recoget = ntp_vertex->GetEntry(iev);

	  nhittpcin_cum += nhittpcin;
	  nhittpcin_wt += 1.0;

	  if(verbose)
	    cout 
	      << "iev " << iev
	      << " event " << event
	      << " ntracks  (reco) " << ntracks
	      << " ngtracks (g4) " << ngtracks
	      //<< " gvz " << egvz
	      //	 << " vz " << evz
	      << endl;


	  //============================
	  // process G4 tracks
	  // for this event
	  //============================

	  int ng4trevt_elec = -1;
	  int ng4trevt_pos = -1;
	  int g4trnum_elec[1000];
	  int g4trnum_pos[1000];



	  for(int ig=ng;ig<ng+ngtracks;ig++)
	    {
	      int recoget1 = ntp_gtrack->GetEntry(ig);
	      if(!recoget1)
		{
		  if(verbose > 0) cout << "Did not get entry for ig = " << ig << endl;
		  break;
		}


	      // This bookkeeping is needed because the evaluator records for each event the total track count in ntp_vertex, even 
	      // when it writes out only embedded tracks
	      if(tevent != iev)
		{
		  if(verbose > 0) cout << " reached new event for ig = " << ig << " tevent = " << tevent << endl; 
		  ng = ig;
		  break;
		}
	      if(ig == ng+ngtracks - 1)
		{
		  if(verbose > 0) cout << " last time through loop for ig = " << ig << " revent = " << tevent << endl; 
		  ng = ig+1;
		}


	      if(tembed != embed_flag)
		//if( tgtrackid != 1 && tgtrackid != 2)      
		continue;


	      // we want only electrons or positrons
	      if(tflavor != 11 && tflavor != -11)
		continue;

	      if(tflavor == 11)
		{
		  // electron
		  ng4trevt_elec++;
		  if(ng4trevt_elec > 999)
		    continue;

		  if(verbose)
		    cout << " Found electron:" << endl
			 << "  ig " << ig
			 << " ng4trevt_elec " << ng4trevt_elec
			 << " gtrackID " << tgtrackid
			 << " gflavor " << tflavor
			 << " tpx " << tpx
			 << " tpy " << tpy
			 << " tpz " << tpz
			 << endl;
	      
		  g4trnum_elec[ng4trevt_elec] = ig;
		}
	      else
		{
		  // positron
		  ng4trevt_pos++;
		  if(ng4trevt_pos > 999)
		    continue;

		  if(verbose)
		    cout << " Found positron:" << endl
			 << "  ig " << ig
			 << " ng4trevt_pos " << ng4trevt_pos
			 << " gtrackID " << tgtrackid
			 << " gflavor " << tflavor
			 << " tpx " << tpx
			 << " tpy " << tpy
			 << " tpz " << tpz
			 << endl;
	      
		  g4trnum_pos[ng4trevt_pos] = ig;
		}
	    }	  
	  ng4trevt_elec++;
	  ng4trevt_pos++;

	  if(verbose)
	    cout << "For this event found " << ng4trevt_elec << " g4 electrons and " << ng4trevt_pos << " g4 positrons"  << endl;
 
	  // make all pairs of g4 (truth) electrons and positrons
	  for(int ielec=0;ielec<ng4trevt_elec;ielec++)
	    {
	      int recoelec = ntp_gtrack->GetEntry(g4trnum_elec[ielec]);


	      if(tembed != embed_flag)
		continue;

	      double elec_pT = sqrt(tpx*tpx+tpy*tpy);
	      double elec_eta = asinh(tpz/sqrt(tpx*tpx+tpy*tpy));

	      int gtrid1 = tgtrackid;
 
	      TLorentzVector t1;
	      double E1 = sqrt( pow(tpx,2) + pow(tpy,2) + pow(tpz,2) + pow(decaymass,2));	  
	      t1.SetPxPyPzE(tpx,tpy,tpz,E1);	  
	  
	      // print out track details
	      if(verbose > 1)
		cout << "  Pair electron:  iev " << iev << " ielec " << ielec
		     << " g4trnum_elec " << g4trnum_elec[ielec]
		     << " tgtrackid " << tgtrackid
		     << " tflavor " << tflavor
		     << " tpx " << tpx
		     << " tpy " << tpy
		     << " tpz " << tpz
		     << " elec_eta " << elec_eta
		     << " elec_gpT " << elec_pT
		     << endl;
	  
	      for(int ipos =0;ipos<ng4trevt_pos;ipos++)
		{
		  int recopos = ntp_gtrack->GetEntry(g4trnum_pos[ipos]);

		  int gtrid2 = tgtrackid;

		  double pos_pT = sqrt(tpx*tpx+tpy*tpy);
		  double pos_eta = asinh(tpz/sqrt(tpx*tpx+tpy*tpy));
	      
		  // print out track details
		  if(verbose > 1)
		    cout << "  Pair positron: iev " << iev << " ipos " << ipos
			 << " g4trnum_pos " << g4trnum_pos[ipos]
			 << " tgtrackid " << tgtrackid
			 << " tflavor " << tflavor
			 << " tpx " << tpx
			 << " tpy " << tpy
			 << " tpz " << tpz
			 << " pos_eta " << pos_eta
			 << " pos_gpT " << pos_pT
			 << endl;
	      	      
		  // Make G4 invariant mass 
	      
		  TLorentzVector t2;
		  double E2 = sqrt( pow(tpx,2) + pow(tpy,2) + pow(tpz,2) + pow(decaymass,2));
		  t2.SetPxPyPzE(tpx,tpy,tpz,E2);	  
	      
		  TLorentzVector t = t1+t2;
	      
		  if(verbose)
		    cout << "                       reco'd g4 mass = " << t.M() << endl << endl;	    
	      
		  if(t.M() > 7.0 && t.M() < 11.0)
		    {
		      nrecog4mass++;
		      g4mass->Fill(t.M());	
		      hgpt->Fill(t.Pt());

		      // Capture the mass spectrum where both tracks are the primary Upsilon decay electrons
		      g4mass_primary->Fill(t.M());	  

		    }
		}  // end of ipos loop
	    } // end of ielec loop
      
      
	  if(verbose)
	    {
	      cout << " # of g4 electron tracks = " << ng4trevt_elec 
		   << " # of g4 positron tracks = " << ng4trevt_pos << endl;
	    }
	  
	  
	  //=============================
	  // process reconstructed tracks
	  // for this event
	  //=============================
      
	  int nrtrevt = 0;
	  int nr_elec = -1;
	  int nr_pos = -1;
	  int rectrnum_elec[1000];
	  int rectrnum_pos[1000];

	  //cout << "Number of ntp_track entries = " << recoget << endl;

	  for(int ir=nr;ir<nr+ntracks;ir++)
	    {
	      int recoget = ntp_track->GetEntry(ir);
	      if(!recoget)
		{
		  if(verbose > 0) cout << "Did not get entry for ir = " << ir << endl;
		  break;
		}


	      // This bookkeeping is needed because the evaluator records for each event the total track count in ntp_vertex, even 
	      // when it writes out only embedded tracks
	      if(revent != iev)
		{
		  if(verbose > 1) cout << " reached new event for ir = " << ir << " revent = " << revent << endl; 
		  nr = ir;
		  break;
		}
	      if(ir == nr+ntracks - 1)
		{
		  if(verbose > 0)  cout << " last time through loop for ir = " << ir << " revent = " << revent << endl; 
		  nr = ir+1;
		}

	      if(rgembed != embed_flag)
		continue;
	  
	      hrquality->Fill(rquality);
	      hrdca2d->Fill(rdca2d);

	      // track quality cuts	
	      if(rquality > quality_cut || fabs(rdca2d) > dca_cut)
		continue;

	      // make a list of electrons and positrons
	      if(rcharge == -1)
		{
		  nr_elec++;
		  rectrnum_elec[nr_elec] = ir;	      
		}

	      if(rcharge == 1)
		{
		  nr_pos++;
		  rectrnum_pos[nr_pos] = ir;	      
		}


	      double rpT = sqrt(rpx*rpx+rpy*rpy);
	      double reta = asinh(rpz/sqrt(rpx*rpx+rpy*rpy));
	  
	      // print out track details
	      if(verbose)
		cout << "     revent " << revent << " ir " << ir
		     << " rgtrackid " << rgtrackid
		     << " rgflavor " << rgflavor
		     << " rvz " << rvz
		     << " reta " << reta
		     << " rpT " << rpT
		     << endl;
	    }

	  nr_elec++;
	  nr_pos++;  

	  for(int ielec = 0;ielec<nr_elec;ielec++)
	    {

	      TLorentzVector t1;
	  
	      int recoget1 = ntp_track->GetEntry(rectrnum_elec[ielec]);

	      int trid1 = rgtrackid;	  

	      double E1 = sqrt( pow(rpx,2) + pow(rpy,2) + pow(rpz,2) + pow(decaymass,2));
	      t1.SetPxPyPzE(rpx,rpy,rpz,E1);	  
	  
	      for(int ipos = 0;ipos<nr_pos;ipos++)
		{
		  int recoget2 = ntp_track->GetEntry(rectrnum_pos[ipos]);

		  int trid2 = rgtrackid;	  
	  
		  TLorentzVector t2;
		  double E2 = sqrt( pow(rpx,2) + pow(rpy,2) + pow(rpz,2) + pow(decaymass,2));
	  
		  t2.SetPxPyPzE(rpx,rpy,rpz,E2);	  
	  
		  TLorentzVector t = t1+t2;
	      
		  if(verbose)
		    cout << " reco'd track mass = " << t.M() << endl;	    
	      
		  if(t.M() > 7.0 && t.M() < 11.0)
		    {
		      nrecormass++;
		      recomass->Fill(t.M());	  
		      hrpt->Fill(t.Pt());

		      // Capture the mass spectrum where both tracks are the primary Upsilon decay electrons
		      if( (trid1 == 1 || trid1 == 2) && (trid2 == 1 || trid2 == 2) ) 
			recomass_primary->Fill(t.M());	  
		    }
		}
	    }
      	}

      delete ntp_gtrack;
      delete ntp_track;
      delete ntp_cluster;
      delete ntp_vertex;
    }

  cout << "nevents = " << nevents << endl;

  cout << "nrecog4mass = " << nrecog4mass << endl;

  cout << "nrecormass = " << nrecormass << endl;

  cout << "nhittpcin per event = " << nhittpcin_cum/nhittpcin_wt << endl;


  //======================================================
  // End of loop over events and generation of mass histos
  //=====================================================

  TCanvas *cq = new TCanvas("cq","cq",5,5,600,600 );
  cq->Divide(1,2);
  cq->cd(1);
  hrquality->Draw();
  cq->cd(2);
  gPad->SetLogy(1);
  hrdca2d->Draw();

  // Mass histos
  
  TCanvas *cmass = new TCanvas("cmass","cmass",10,10,800,600);

  recomass_primary->SetLineColor(kRed);
  recomass->SetLineColor(kBlack);
  recomass->DrawCopy();  
  recomass_primary->DrawCopy("same");  

  TCanvas *cm_comp = new TCanvas("cm_comp","cm_comp",10,10,800,800);
  cm_comp->Divide(2,1);
  cm_comp->cd(1);
  recomass->Draw();
  recomass_primary->Draw("same");

  // we want from 7 to 11 GeV/c^2 - the whole range
  double yreco = recomass->Integral();
 double yreco_primary = recomass_primary->Integral();
 cout << "Reconstructed mass spectrum has " << yreco_primary << " entries from primary tracks and " << yreco << " entries total " << endl;

  cm_comp->cd(2);
  
  g4mass_primary->SetLineColor(kRed);
  g4mass->Draw();
  g4mass_primary->Draw("same");

  double yg4_primary = g4mass_primary->Integral();
  double yg4 = g4mass->Integral();
  cout << "G4 mass spectrum has " << yg4_primary << " entries from primary tracks and  " << yg4 << " entries total" << endl;

  cout << "Reconstruction efficiency is " << yreco_primary/yg4_primary << endl;

  // Output histos for reconstructed Upsilons

  char fname[500];

  sprintf(fname,"root_files/ups%is_qual%.2f_dca2d%.2f.root",ups_state,quality_cut,dca_cut);

  cout << "Create output file " << fname << endl;

  TFile *fout1 = new TFile(fname,"recreate");
  recomass->Write();
  recomass_primary->Write();
  g4mass->Write();
  g4mass_primary->Write();
  hrpt->Write();
  hrquality->Write();
  hrdca2d->Write();
  fout1->Close();

  cout << "Finished write to file " << fname << endl;

}

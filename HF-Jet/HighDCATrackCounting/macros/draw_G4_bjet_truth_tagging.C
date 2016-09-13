#include "TFile.h"
#include "TTree.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"

#include <iostream>
#include <sstream>

#include "AtlasUtils.C"

float deltaR( float eta1, float eta2, float phi1, float phi2 ) {

	float deta = eta1 - eta2;
	float dphi = phi1 - phi2;
	if (dphi > 3.14159) dphi -= 2*3.14159;
	if (dphi < -3.14159) dphi += 2*3.14159;

	return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );

}

void draw_G4_bjet_truth_tagging(
		const char* input = "MIE_1M/HFtag_jet.root",
		const TString tag_method = "Parton", // Parton, Hadron
		const char* tracking_option_name = "2014 proposal tracker",
		const int dca_method = 0 // direct from g4hough, reco-vertex and strait line assumption, dca3d reco-vertex and strait line assumption
		) { 

	const double simulation_jet_energy = 20;

	gROOT->LoadMacro("SetOKStyle.C");
	SetOKStyle();

	TFile *fout = TFile::Open("dca_eval.root","recreate");
	fout->cd();
	TTree *T = new TTree("T","eval tree");
	int _n_tracks;
	float _dca2d[100];
	float _dca2d_error[100];
	float _highest_dca2d;
	float _highest_dca2d_error;
	float _second_highest_dca2d;
	float _second_highest_dca2d_error;
	float _highest_S;
	float _second_highest_S;
	T->Branch("n_tracks",&_n_tracks,"n_tracks/I");
	T->Branch("dca2d",_dca2d,"dca2d[n_tracks]/F");
	T->Branch("dca2d_error",_dca2d_error,"dca2d_error[n_tracks]/F");
	T->Branch("highest_dca2d",&_highest_dca2d,"highest_dca2d/F");
	T->Branch("highest_dca2d_error",&_highest_dca2d_error,"highest_dca2d_error/F");
	T->Branch("second_highest_dca2d",&_second_highest_dca2d,"second_highest_dca2d/F");
	T->Branch("second_highest_dca2d_error",&_second_highest_dca2d_error,"second_highest_dca2d_error/F");
	T->Branch("highest_S",&_highest_S,"highest_S/F");
	T->Branch("second_highest_S",&_second_highest_S,"second_highest_S/F");

	TH1D *h1_jet_pt[3];

	TH1D *h1_track_pt[3];
	TH1D *h1_track_pt_pri[3];
	TH1D *h1_track_pt_fake[3];
	TH1D *h1_track_pt_sec[3];

	TH1D *h1_particle_pt[3];  
	TH1D *h1_particle_pt_dca0[3];  
	TH1D *h1_particle_pt_dca1[3];  
	TH1D *h1_particle_pt_species[5][3];

	for (int flavor = 0; flavor < 3; flavor++) {
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_pt_" << flavor; h1_jet_pt[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",30,0,60); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_track_pt_" << flavor; h1_track_pt[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",30,0,30); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_track_pt_pri_" << flavor; h1_track_pt_pri[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",30,0,30); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_track_pt_fake_" << flavor; h1_track_pt_fake[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",30,0,30); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_track_pt_sec_" << flavor; h1_track_pt_sec[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",30,0,30); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_particle_pt_" << flavor; h1_particle_pt[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",30,0,30); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_particle_pt_dca0" << flavor; h1_particle_pt_dca0[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",30,0,30); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_particle_pt_dca1" << flavor; h1_particle_pt_dca1[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",30,0,30); }
		for (int species = 0; species < 5; species++) {
			{ ostringstream temp_o_str_s; temp_o_str_s << "h1_particle_pt_species" << species << "_" << flavor;
				h1_particle_pt_species[species][flavor] = new TH1D( temp_o_str_s.str().c_str(), "", 30, 0, 30 );
			}
		}
	}

	TH1D *h1_particle_dca[3];  
	TH1D *h1_particle_dca_species[5][3];  
	TH1D *h1_track_dca[3];  
	//TH1D *h1_track_dca_species[5][3];  

	for (int flavor = 0; flavor < 3; flavor++) {
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_particle_dca_" << flavor; h1_particle_dca[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",80,0,20); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_track_dca_" << flavor; h1_track_dca[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",80,-0.2,+0.2); }
		for (int species = 0; species < 5; species++) {
			{ ostringstream temp_o_str_s; temp_o_str_s << "h1_particle_dca_species" << species << "_" << flavor;
				h1_particle_dca_species[species][flavor] = new TH1D( temp_o_str_s.str().c_str(), "", 80, 0, 20 );
			}
		}
	}

	TH1D *h1_jet_highest_dca[3];
	TH1D *h1_jet_second_highest_dca[3];
	TH1D *h1_jet_third_highest_dca[3];

	TH1D *h1_jet_highest_S[3];
	TH1D *h1_jet_second_highest_S[3];
	TH1D *h1_jet_third_highest_S[3];

	for (int flavor = 0; flavor < 3; flavor++) {
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_highest_dca_" << flavor; h1_jet_highest_dca[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",60,-0.1,+0.2); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_second_highest_dca_" << flavor; h1_jet_second_highest_dca[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",60,-0.1,+0.2); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_third_highest_dca_" << flavor; h1_jet_third_highest_dca[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",60,-0.1,+0.2); }

		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_highest_S_" << flavor; h1_jet_highest_S[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",70,-20,+50); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_second_highest_S_" << flavor; h1_jet_second_highest_S[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",70,-20,+50); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_third_highest_S_" << flavor; h1_jet_third_highest_S[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",70,-20,+50); }
	}


	TH1D *h1_jet_highest_dca_FINE[3];
	TH1D *h1_jet_second_highest_dca_FINE[3];
	TH1D *h1_jet_third_highest_dca_FINE[3];

	TH1D *h1_jet_highest_S_FINE[3];
	TH1D *h1_jet_second_highest_S_FINE[3];
	TH1D *h1_jet_third_highest_S_FINE[3];

	for (int flavor = 0; flavor < 3; flavor++) {
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_highest_dca_FINE_" << flavor; h1_jet_highest_dca_FINE[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",600,-0.1,+0.2); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_second_highest_dca_FINE_" << flavor; h1_jet_second_highest_dca_FINE[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",600,-0.1,+0.2); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_third_highest_dca_FINE_" << flavor; h1_jet_third_highest_dca_FINE[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",600,-0.1,+0.2); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_highest_S_FINE_" << flavor; h1_jet_highest_S_FINE[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",700,-20,+50); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_second_highest_S_FINE_" << flavor; h1_jet_second_highest_S_FINE[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",700,-20,+50); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_third_highest_S_FINE_" << flavor; h1_jet_third_highest_S_FINE[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",700,-20,+50); }
	}


	TH1D *h1_jet_highest_dca_EFF[3];
	TH1D *h1_jet_second_highest_dca_EFF[3];
	TH1D *h1_jet_third_highest_dca_EFF[3];

	TH1D *h1_jet_highest_S_EFF[3];
	TH1D *h1_jet_second_highest_S_EFF[3];
	TH1D *h1_jet_third_highest_S_EFF[3];

	for (int flavor = 0; flavor < 3; flavor++) {
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_highest_dca_EFF_" << flavor; h1_jet_highest_dca_EFF[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",600,-0.1,+0.2); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_second_highest_dca_EFF_" << flavor; h1_jet_second_highest_dca_EFF[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",600,-0.1,+0.2); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_third_highest_dca_EFF_" << flavor; h1_jet_third_highest_dca_EFF[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",600,-0.1,+0.2); }

		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_highest_S_EFF_" << flavor; h1_jet_highest_S_EFF[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",700,-20,+50); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_second_highest_S_EFF_" << flavor; h1_jet_second_highest_S_EFF[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",700,-20,+50); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_third_highest_S_EFF_" << flavor; h1_jet_third_highest_S_EFF[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",700,-20,+50); }
	}


	TH1D *h1_jet_ntrk[3];
	TH1D *h1_jet_ntrk1[3];

	for (int flavor = 0; flavor < 3; flavor++) {
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_ntrk_" << flavor; h1_jet_ntrk[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",21,-0.5,+20.5); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_jet_ntrk1_" << flavor; h1_jet_ntrk1[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",21,-0.5,+20.5); }
	}

	TH2D *h2_track_pt_dcaerror = new TH2D("h2_track_pt_dcaerror","",30,0,30,100,0,0.01);
	TH2D *h2_track_dca_dcaerror = new TH2D("h2_track_dca_dcaerror","",100,-0.2,0.2,100,0,0.01);

	/*
		 = new TH1D("h1_jet_highest_dca","",40,-0.2,+0.2);
		 TH1D *h1_jet_highest_S = new TH1D("h1_jet_highest_S","",80,-40,+40);
		 */

	/*  
			TH1D *h1_track_pt_pri_id0 = new TH1D("h1_track_pt_pri_id0","",30,0,30);
			TH1D *h1_track_pt_pri_id1 = new TH1D("h1_track_pt_pri_id1","",30,0,30);
			TH1D *h1_track_pt_pri_id2 = new TH1D("h1_track_pt_pri_id2","",30,0,30);
			TH1D *h1_track_pt_pri_id3 = new TH1D("h1_track_pt_pri_id3","",30,0,30);
			TH1D *h1_track_pt_pri_id4 = new TH1D("h1_track_pt_pri_id4","",30,0,30);

			TH1D *h1_track_dca_pri_id0 = new TH1D("h1_track_dca_pri_id0","",80,0,20);
			TH1D *h1_track_dca_pri_id1 = new TH1D("h1_track_dca_pri_id1","",80,0,20);
			TH1D *h1_track_dca_pri_id2 = new TH1D("h1_track_dca_pri_id2","",80,0,20);
			TH1D *h1_track_dca_pri_id3 = new TH1D("h1_track_dca_pri_id3","",80,0,20);
			TH1D *h1_track_dca_pri_id4 = new TH1D("h1_track_dca_pri_id4","",80,0,20);

			TH1D *h1_track_dcareco_pri_id0 = new TH1D("h1_track_dcareco_pri_id0","",40,-0.2,+0.2);
			TH1D *h1_track_dcareco_pri_id1 = new TH1D("h1_track_dcareco_pri_id1","",40,-0.2,+0.2);
			TH1D *h1_track_dcareco_pri_id2 = new TH1D("h1_track_dcareco_pri_id2","",40,-0.2,+0.2);
			TH1D *h1_track_dcareco_pri_id3 = new TH1D("h1_track_dcareco_pri_id3","",40,-0.2,+0.2);
			TH1D *h1_track_dcareco_pri_id4 = new TH1D("h1_track_dcareco_pri_id4","",40,-0.2,+0.2);

			TH1D *h1_track_Sreco_pri_id0 = new TH1D("h1_track_Sreco_pri_id0","",80,-40,+40);
			TH1D *h1_track_Sreco_pri_id1 = new TH1D("h1_track_Sreco_pri_id1","",80,-40,+40);
			TH1D *h1_track_Sreco_pri_id2 = new TH1D("h1_track_Sreco_pri_id2","",80,-40,+40);
			TH1D *h1_track_Sreco_pri_id3 = new TH1D("h1_track_Sreco_pri_id3","",80,-40,+40);
			TH1D *h1_track_Sreco_pri_id4 = new TH1D("h1_track_Sreco_pri_id4","",80,-40,+40);

			TH1D *h1_track_pt_pri_parentid0 = new TH1D("h1_track_pt_pri_parentid0","",30,0,30);
			TH1D *h1_track_pt_pri_parentid1 = new TH1D("h1_track_pt_pri_parentid1","",30,0,30);
			TH1D *h1_track_pt_pri_parentid2 = new TH1D("h1_track_pt_pri_parentid2","",30,0,30);
			TH1D *h1_track_pt_pri_parentid3 = new TH1D("h1_track_pt_pri_parentid3","",30,0,30);
			TH1D *h1_track_pt_pri_parentid4 = new TH1D("h1_track_pt_pri_parentid4","",30,0,30);
			TH1D *h1_track_pt_pri_parentid5 = new TH1D("h1_track_pt_pri_parentid5","",30,0,30);

			TH1D *h1_track_dca_pri_parentid0 = new TH1D("h1_track_dca_pri_parentid0","",80,0,20);
			TH1D *h1_track_dca_pri_parentid1 = new TH1D("h1_track_dca_pri_parentid1","",80,0,20);
			TH1D *h1_track_dca_pri_parentid2 = new TH1D("h1_track_dca_pri_parentid2","",80,0,20);
			TH1D *h1_track_dca_pri_parentid3 = new TH1D("h1_track_dca_pri_parentid3","",80,0,20);
			TH1D *h1_track_dca_pri_parentid4 = new TH1D("h1_track_dca_pri_parentid4","",80,0,20);
			TH1D *h1_track_dca_pri_parentid5 = new TH1D("h1_track_dca_pri_parentid5","",80,0,20);

			TH1D *h1_track_dcareco_pri_parentid0 = new TH1D("h1_track_dcareco_pri_parentid0","",40,-0.2,+0.2);
			TH1D *h1_track_dcareco_pri_parentid1 = new TH1D("h1_track_dcareco_pri_parentid1","",40,-0.2,+0.2);
			TH1D *h1_track_dcareco_pri_parentid2 = new TH1D("h1_track_dcareco_pri_parentid2","",40,-0.2,+0.2);
			TH1D *h1_track_dcareco_pri_parentid3 = new TH1D("h1_track_dcareco_pri_parentid3","",40,-0.2,+0.2);
			TH1D *h1_track_dcareco_pri_parentid4 = new TH1D("h1_track_dcareco_pri_parentid4","",40,-0.2,+0.2);
			TH1D *h1_track_dcareco_pri_parentid5 = new TH1D("h1_track_dcareco_pri_parentid5","",40,-0.2,+0.2);

			TH1D *h1_track_Sreco_pri_parentid0 = new TH1D("h1_track_Sreco_pri_parentid0","",80,-40,+40);
			TH1D *h1_track_Sreco_pri_parentid1 = new TH1D("h1_track_Sreco_pri_parentid1","",80,-40,+40);
			TH1D *h1_track_Sreco_pri_parentid2 = new TH1D("h1_track_Sreco_pri_parentid2","",80,-40,+40);
			TH1D *h1_track_Sreco_pri_parentid3 = new TH1D("h1_track_Sreco_pri_parentid3","",80,-40,+40);
			TH1D *h1_track_Sreco_pri_parentid4 = new TH1D("h1_track_Sreco_pri_parentid4","",80,-40,+40);
			TH1D *h1_track_Sreco_pri_parentid5 = new TH1D("h1_track_Sreco_pri_parentid5","",80,-40,+40);

*/

	int truthjet_n;
	int truthjet_parton_flavor[10];
	int truthjet_hadron_flavor[10];
	float truthjet_pt[10];
	float truthjet_eta[10];
	float truthjet_phi[10];

	int particle_n;
	float particle_pt[100];
	float particle_eta[100];
	float particle_phi[100];
	float particle_dca[100];
	int particle_pid[100];

	int track_n;
	float track_pt[100];
	float track_eta[100];
	float track_phi[100];
	float track_dca2d[100];
	float track_dca2d_error[100];
	float track_dca2d_phi[100];

	float track_dca2d_calc[100];
	float track_dca2d_calc_truth[100];
	float track_dca3d_calc[100];
	float track_dca3d_calc_truth[100];

	float track_quality[100];
	float track_chisq[100];
	int track_ndf[100];

	bool track_best_primary[100];
	unsigned int track_best_nclusters[100];
	int track_best_pid[100];
	float track_best_dca[100];
	int track_best_parent_pid[100];

	TFile *f = TFile::Open(Form("%s",input),"READ");
	TTree *ttree = (TTree*) f->Get("ttree");

	ttree->SetBranchAddress("truthjet_n",   &truthjet_n );
	ttree->SetBranchAddress("truthjet_parton_flavor",   truthjet_parton_flavor );
	ttree->SetBranchAddress("truthjet_hadron_flavor",   truthjet_hadron_flavor );
	ttree->SetBranchAddress("truthjet_pt",   truthjet_pt );
	ttree->SetBranchAddress("truthjet_eta",  truthjet_eta );
	ttree->SetBranchAddress("truthjet_phi",  truthjet_phi );

	ttree->SetBranchAddress("particle_n",   &particle_n );
	ttree->SetBranchAddress("particle_pt",   particle_pt );
	ttree->SetBranchAddress("particle_eta",  particle_eta );
	ttree->SetBranchAddress("particle_phi",  particle_phi );
	ttree->SetBranchAddress("particle_dca",  particle_dca );
	ttree->SetBranchAddress("particle_pid",  particle_pid );

	ttree->SetBranchAddress("track_n",   &track_n );
	ttree->SetBranchAddress("track_pt",   track_pt );
	ttree->SetBranchAddress("track_eta",  track_eta );
	ttree->SetBranchAddress("track_phi",  track_phi );
	ttree->SetBranchAddress("track_dca2d",  track_dca2d );
	ttree->SetBranchAddress("track_dca2d_error",  track_dca2d_error );
	ttree->SetBranchAddress("track_dca2d_phi",  track_dca2d_phi );

	ttree->SetBranchAddress("track_dca2d_calc",  track_dca2d_calc );
	ttree->SetBranchAddress("track_dca2d_calc_truth",  track_dca2d_calc_truth );
	ttree->SetBranchAddress("track_dca3d_calc",  track_dca3d_calc );
	ttree->SetBranchAddress("track_dca3d_calc_truth",  track_dca3d_calc_truth );

	ttree->SetBranchAddress("track_quality",   track_quality );
	ttree->SetBranchAddress("track_chisq",   track_chisq );
	ttree->SetBranchAddress("track_ndf",   track_ndf );

	ttree->SetBranchAddress("track_best_primary",  track_best_primary );
	ttree->SetBranchAddress("track_best_nclusters",  track_best_nclusters );
	ttree->SetBranchAddress("track_best_pid",  track_best_pid );
	ttree->SetBranchAddress("track_best_dca",  track_best_dca );
	ttree->SetBranchAddress("track_best_parent_pid", track_best_parent_pid );

	//int rem = 0;
	//int rems[6] = {0,0,0,0,0,0};


	int nentries = ttree->GetEntries();
	//nentries = 10000;
	for (int e = 0 ; e < nentries; e++) {

		if(e%1000 == 0) cout<<"Processing: "<<100.*e/nentries <<"\% \n";

		ttree->GetEntry( e );

		for (int j = 0; j < truthjet_n; j++) {

			int iflavor = -99;
			//cout<<"event: "<<e<<" : truthjet_parton_flavor: "<< truthjet_parton_flavor[j] <<endl;
			int flavor_tag = -99;
			if(tag_method.Contains("Parton"))
				flavor_tag = truthjet_parton_flavor[j];
			else if(tag_method.Contains("Hadron"))
				flavor_tag = truthjet_hadron_flavor[j];

			if(abs(flavor_tag) == 0) iflavor = 0;
			else if(abs(flavor_tag) == 4) iflavor = 1;
			else if(abs(flavor_tag) == 5) iflavor = 2;
			else continue;

			if ( fabs( truthjet_eta[ j ]) > 0.6 ) continue;
			if (truthjet_pt[ j ] < 20 ) continue;



			//std::cout << " event #" << e << " jet #" << j << " pt/eta/phi = " << truthjet_pt[ j ] << " / " << truthjet_eta[ j ] << " / " << truthjet_phi[ j ] << std::endl;

			h1_jet_pt[iflavor]->Fill( truthjet_pt[ j ] );

			int ntrk = 0;
			int ntrk1 = 0;

			for (int p = 0; p < particle_n; p++) {

				if ( particle_pid[ p ] == 16 || particle_pid[ p ] == -16) continue;

				float dR = deltaR( truthjet_eta[ j ], particle_eta[ p ], truthjet_phi[ j ], particle_phi[ p ] );
				if (dR > 0.4) continue;

				h1_particle_pt[iflavor]->Fill( particle_pt[ p ] );
				h1_particle_dca[iflavor]->Fill( particle_dca[ p ] );
				if (particle_dca[ p ] < 0.000001)
					h1_particle_pt_dca0[iflavor]->Fill( particle_pt[ p ] );
				else
					h1_particle_pt_dca1[iflavor]->Fill( particle_pt[ p ] );
				//h1_particle_dca[iflavor]->Fill( particle_dca[ p ] );

				// 
				if (particle_pid[ p ] == 211 || particle_pid[ p ] == -211 ) {
					// pi+-
					h1_particle_pt_species[0][iflavor]->Fill( particle_pt[ p ] );
					h1_particle_dca_species[0][iflavor]->Fill( particle_dca[ p ] );

					//h1_particle_dcareco_pri_id0->Fill( particle_dca2d[ p ] );
					//h1_particle_Sreco_pri_id0->Fill( particle_dca2d[ p ] / particle_dca2d_error[ p ] );
				}
				else if (particle_pid[ p ] == 321 || particle_pid[ p ] == -321 ) {
					// k+-
					h1_particle_pt_species[1][iflavor]->Fill( particle_pt[ p ] );
					h1_particle_dca_species[1][iflavor]->Fill( particle_dca[ p ] );

					//h1_particle_dcareco_pri_id1->Fill( particle_dca2d[ p ] );
					//h1_particle_Sreco_pri_id1->Fill( particle_dca2d[ p ] / particle_dca2d_error[ p ] );
				}
				else if (particle_pid[ p ] == 2212 || particle_pid[ p ] == -2212 ) {
					// p+-
					h1_particle_pt_species[2][iflavor]->Fill( particle_pt[ p ] );
					h1_particle_dca_species[2][iflavor]->Fill( particle_dca[ p ] );

					//h1_particle_dcareco_pri_id2->Fill( particle_dca2d[ p ] );
					//h1_particle_Sreco_pri_id2->Fill( particle_dca2d[ p ] / particle_dca2d_error[ p ] );
				}
				else if (particle_pid[ p ] == 11 || particle_pid[ p ] == -11 ) {
					// e+-
					h1_particle_pt_species[3][iflavor]->Fill( particle_pt[ p ] );
					h1_particle_dca_species[3][iflavor]->Fill( particle_dca[ p ] );

					//h1_particle_dcareco_pri_id3->Fill( particle_dca2d[ p ] );
					//h1_particle_Sreco_pri_id3->Fill( particle_dca2d[ p ] / particle_dca2d_error[ p ] );
				}
				else if (particle_pid[ p ] == 13 || particle_pid[ p ] == -13 ) {
					// mu+-
					h1_particle_pt_species[4][iflavor]->Fill( particle_pt[ p ] );
					h1_particle_dca_species[4][iflavor]->Fill( particle_dca[ p ] );

					//h1_particle_dcareco_pri_id4->Fill( particle_dca2d[ p ] );
					//h1_particle_Sreco_pri_id4->Fill( particle_dca2d[ p ] / particle_dca2d_error[ p ] );
				}
				else {
					std::cout << " --> particle #" << p << " (dR = " << dR << "), pt/eta/phi = " << particle_pt[ p ] << " / " << particle_eta[ p ] << " / " << particle_phi[ p ] << ", pid = " << particle_pid[ p ] << std::endl;

				}
			}

			float highest_S = -99;
			float highest_dca = -99;

			float second_highest_S = -99;
			float second_highest_dca = -99;

			float third_highest_S = -99;
			float third_highest_dca = -99;

			for (int itrk = 0; itrk < track_n; itrk++) {

				if(!(track_quality[itrk]<1.)) continue; // yuhw

				_dca2d[itrk] = -99;
				_dca2d_error[itrk] = -99;

				float dR = deltaR( truthjet_eta[ j ], track_eta[ itrk ], truthjet_phi[ j ], track_phi[ itrk ] );
				if (dR > 0.4) continue;


				track_dca2d[ itrk ] = fabs( track_dca2d[ itrk ] );
				if(dca_method == 1) track_dca2d[ itrk ] = fabs( track_dca2d_calc[ itrk ] ); 
				if(dca_method == 2) track_dca2d[ itrk ] = fabs( track_dca3d_calc[ itrk ] ); 
				if (! (track_dca2d[ itrk ] < 0.1)) continue; // yuhw

				// set sign WRT jet
				{
					float dphi = track_dca2d_phi[ itrk ] - truthjet_phi[ j ];

					//if (iflavor == 2 && track_dca2d[ itrk ] > 0.05) 
					//	std::cout << truthjet_phi[ j ] << " / " <<  track_dca2d_phi[ itrk ] << std::endl;

					if (dphi > +3.14159) dphi -= 2*3.14159;
					if (dphi < -3.14159) dphi += 2*3.14159;
					dphi = fabs(dphi);
					if (dphi > 3.14159/2)
						track_dca2d[ itrk ] = -1 * track_dca2d[ itrk ];
				}

				if (iflavor == 2) {
					h2_track_pt_dcaerror->Fill(  track_pt[ itrk ],  track_dca2d_error[ itrk ] );
					h2_track_dca_dcaerror->Fill(  track_dca2d[ itrk ],  track_dca2d_error[ itrk ] );
				}

				ntrk++;

				if (track_pt[ itrk ] > 1)
					ntrk1++;

				h1_track_pt[iflavor]->Fill( track_pt[ itrk ] );
				h1_track_dca[iflavor]->Fill( track_dca2d[ itrk ] );


				if ( track_dca2d[ itrk ] > highest_dca ) {
					third_highest_dca = second_highest_dca;
					second_highest_dca = highest_dca;
					highest_dca = track_dca2d[ itrk ];
				} else if ( track_dca2d[ itrk ] > second_highest_dca ) {

					third_highest_dca = second_highest_dca;
					second_highest_dca = track_dca2d[ itrk ];
				} else if ( track_dca2d[ itrk ] > third_highest_dca ) {
					third_highest_dca = track_dca2d[ itrk ];
				}

				if ( track_dca2d[ itrk ] / track_dca2d_error[ itrk ] > highest_S ) {
					third_highest_S = second_highest_S;
					second_highest_S = highest_S;
					highest_S = track_dca2d[ itrk ] / track_dca2d_error[ itrk ];
				} else if ( track_dca2d[ itrk ]/ track_dca2d_error[ itrk ] > second_highest_S ) {
					third_highest_S = second_highest_S;	    
					second_highest_S = track_dca2d[ itrk ] / track_dca2d_error[ itrk ];	    
				} else if ( track_dca2d[ itrk ]/ track_dca2d_error[ itrk ] > third_highest_S ) {
					third_highest_S = track_dca2d[ itrk ] / track_dca2d_error[ itrk ];
				}

				if (track_best_nclusters[ itrk] >= 6) {

					if (track_best_primary[ itrk ]) {
						h1_track_pt_pri[iflavor]->Fill( track_pt[ itrk ] );
					}
					else {
						h1_track_pt_sec[iflavor]->Fill( track_pt[ itrk ] );
						continue;
					}

				}
				else {
					h1_track_pt_fake[iflavor]->Fill( track_pt[ itrk ] );
					continue;
				}

				/*	  
							int absid = abs( track_best_parent_pid[ itrk ] );

							if (absid > 10000) absid = (absid % 10000);

							if ( absid < 10 || absid == 21 )
							{
				// quark, gluon 
				rems[0]++;
				h1_track_pt_pri_parentid0->Fill( track_pt[ itrk ] );
				h1_track_dca_pri_parentid0->Fill( track_best_dca[ itrk ] );
				h1_track_dcareco_pri_parentid0->Fill( track_dca2d[ itrk ] );
				h1_track_Sreco_pri_parentid0->Fill( track_dca2d[ itrk ] / track_dca2d_error[ itrk ] );
				}
				else if ( absid == 15 )
				{
				// tau
				rems[1]++;
				h1_track_pt_pri_parentid1->Fill( track_pt[ itrk ] );
				h1_track_dca_pri_parentid1->Fill( track_best_dca[ itrk ] );
				h1_track_dcareco_pri_parentid1->Fill( track_dca2d[ itrk ] );
				h1_track_Sreco_pri_parentid1->Fill( track_dca2d[ itrk ] / track_dca2d_error[ itrk ] );
				}
				else if ( (absid > 100 && absid < 300) || (absid > 1000 && absid < 3000) ) 
				{
				// light meson or baryon
				rems[2]++;
				h1_track_pt_pri_parentid2->Fill( track_pt[ itrk ] );
				h1_track_dca_pri_parentid2->Fill( track_best_dca[ itrk ] );
				h1_track_dcareco_pri_parentid2->Fill( track_dca2d[ itrk ] );
				h1_track_Sreco_pri_parentid2->Fill( track_dca2d[ itrk ] / track_dca2d_error[ itrk ] );
				}
				else if ( (absid > 300 && absid < 400) || (absid > 3000 && absid < 4000) ) 
				{
				// strange meson or baryon
				rems[3]++;
				h1_track_pt_pri_parentid3->Fill( track_pt[ itrk ] );
				h1_track_dca_pri_parentid3->Fill( track_best_dca[ itrk ] );
				h1_track_dcareco_pri_parentid3->Fill( track_dca2d[ itrk ] );
				h1_track_Sreco_pri_parentid3->Fill( track_dca2d[ itrk ] / track_dca2d_error[ itrk ] );
				}
				else if ( (absid > 400 && absid < 500) || (absid > 4000 && absid < 5000) ) 
				{
				// charm meson or baryon
				rems[4]++;
				h1_track_pt_pri_parentid4->Fill( track_pt[ itrk ] );
				h1_track_dca_pri_parentid4->Fill( track_best_dca[ itrk ] );
				h1_track_dcareco_pri_parentid4->Fill( track_dca2d[ itrk ] );
				h1_track_Sreco_pri_parentid4->Fill( track_dca2d[ itrk ] / track_dca2d_error[ itrk ] );
				}
				else if ( (absid > 500 && absid < 600) || (absid > 5000 && absid < 6000) ) 
				{
				// bottom meson or baryon
				rems[5]++;
				h1_track_pt_pri_parentid5->Fill( track_pt[ itrk ] );
				h1_track_dca_pri_parentid5->Fill( track_best_dca[ itrk ] );
				h1_track_dcareco_pri_parentid5->Fill( track_dca2d[ itrk ] );
				h1_track_Sreco_pri_parentid5->Fill( track_dca2d[ itrk ] / track_dca2d_error[ itrk ] );
				}

				else 
				{
				std::cout << " --> track #" << itrk << " (dR = " << dR << "), pt/eta/phi = " << track_pt[ itrk ] << " / " << track_eta[ itrk ] << " / " << track_phi[ itrk ] << ", pid = " << track_best_pid[ itrk ] << ", parent ID = " << track_best_parent_pid[ itrk ] << std::endl;

				rem++;	    

				}
				*/	  

				if(iflavor == 0) {
					_dca2d[itrk] = track_dca2d[ itrk ];
					_dca2d_error[itrk] = track_dca2d_error[ itrk ];
				}

			} // end tracks

			if(iflavor==0) {
				_n_tracks = ntrk;
				_highest_dca2d = highest_dca;
				_second_highest_dca2d = second_highest_dca;
				_highest_S = highest_S;
				_second_highest_S = second_highest_S;
				T->Fill();
			}

			h1_jet_ntrk[iflavor]->Fill( ntrk );
			h1_jet_ntrk1[iflavor]->Fill( ntrk1 );

			h1_jet_highest_dca[iflavor]->Fill( highest_dca );
			h1_jet_highest_S[iflavor]->Fill( highest_S );

			h1_jet_second_highest_dca[iflavor]->Fill( second_highest_dca );
			h1_jet_second_highest_S[iflavor]->Fill( second_highest_S );

			h1_jet_third_highest_dca[iflavor]->Fill( third_highest_dca );
			h1_jet_third_highest_S[iflavor]->Fill( third_highest_S );

			h1_jet_highest_dca_FINE[iflavor]->Fill( highest_dca );
			h1_jet_highest_S_FINE[iflavor]->Fill( highest_S );

			h1_jet_second_highest_dca_FINE[iflavor]->Fill( second_highest_dca );
			h1_jet_second_highest_S_FINE[iflavor]->Fill( second_highest_S );

			h1_jet_third_highest_dca_FINE[iflavor]->Fill( third_highest_dca );
			h1_jet_third_highest_S_FINE[iflavor]->Fill( third_highest_S );
		} // loop jets

	} // loop events/entries


	//std::cout << " remaining : " << rem << std::endl;
	//std::cout << " breakdown : " << rems[0] << " " << rems[1] << " " << rems[2] << " " << rems[3] << " " << rems[4] << " " << rems[5]  << std::endl;


	h1_jet_pt[1]->SetLineColor( kRed );
	h1_jet_pt[2]->SetLineColor( kBlue );


	TCanvas *tc = new TCanvas();
	h1_jet_pt[2]->SetTitle(";#it{p}_{T} [GeV];counts / 2 GeV");
	h1_jet_pt[2]->SetMaximum( h1_jet_pt[2]->GetMaximum() * 1.3 );
	h1_jet_pt[2]->Draw();
	h1_jet_pt[1]->Draw("same");
	h1_jet_pt[0]->Draw("same");

	myText(0.2,0.90,kBlack,"light jets");
	myText(0.2,0.83,kRed,"charm jets");
	myText(0.2,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_pt.pdf");

	gPad->SetLogz(1);

	h2_track_pt_dcaerror->SetTitle(";#it{p}_{T} [GeV];DCA error [cm]");
	h2_track_pt_dcaerror->Draw("colz");
	myText(0.6,0.88,kBlack,"bottom jets");
	tc->Print("plot/h2_track_pt_dcaerror.pdf");
	h2_track_dca_dcaerror->SetTitle(";DCA [cm];DCA error [cm]");
	h2_track_dca_dcaerror->Draw("colz");
	myText(0.6,0.88,kBlack,"bottom jets");
	tc->Print("plot/h2_track_dca_dcaerror.pdf");

	gPad->SetLogy(1);

	h1_jet_ntrk[1]->SetLineColor( kRed );
	h1_jet_ntrk[2]->SetLineColor( kBlue );

	h1_jet_ntrk[0]->SetTitle(";#it{N}_{trk}(> 0.5 GeV);counts");
	h1_jet_ntrk[0]->SetMinimum(0.5);
	h1_jet_ntrk[0]->Draw();
	h1_jet_ntrk[1]->Draw("same");
	h1_jet_ntrk[2]->Draw("same");

	myText(0.7,0.90,kBlack,"light jets");
	myText(0.7,0.83,kRed,"charm jets");
	myText(0.7,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_ntrk.pdf");

	h1_jet_ntrk1[1]->SetLineColor( kRed );
	h1_jet_ntrk1[2]->SetLineColor( kBlue );

	h1_jet_ntrk1[0]->SetTitle(";#it{N}_{trk}(> 1 GeV);counts");
	h1_jet_ntrk1[0]->SetMinimum(0.5);
	h1_jet_ntrk1[0]->Draw();
	h1_jet_ntrk1[1]->Draw("same");
	h1_jet_ntrk1[2]->Draw("same");

	myText(0.7,0.90,kBlack,"light jets");
	myText(0.7,0.83,kRed,"charm jets");
	myText(0.7,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_ntrk1.pdf");

	//std::cout << h1_jet_pt[0]->Integral(-1,-1) << " " << h1_jet_pt[1]->Integral(-1,-1) << " " << h1_jet_pt[2]->Integral(-1,-1) << std::endl;

	gPad->SetLogy(1);

	h1_jet_highest_dca[1]->SetLineColor( kRed );
	h1_jet_highest_dca[2]->SetLineColor( kBlue );

	h1_jet_highest_dca[0]->SetTitle(";largest DCA [cm];counts / 0.005 cm");
	h1_jet_highest_dca[0]->Draw();
	h1_jet_highest_dca[1]->Draw("same");
	h1_jet_highest_dca[2]->Draw("same");
	myText(0.2,0.90,kBlack,"light jets");
	myText(0.2,0.83,kRed,"charm jets");
	myText(0.2,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_highest_dca.pdf");

	h1_jet_highest_S[1]->SetLineColor( kRed );
	h1_jet_highest_S[2]->SetLineColor( kBlue );

	h1_jet_highest_S[0]->SetTitle(";largest #it{S}_{DCA};counts / 1");
	h1_jet_highest_S[0]->Draw();
	h1_jet_highest_S[1]->Draw("same");
	h1_jet_highest_S[2]->Draw("same");
	myText(0.6,0.90,kBlack,"light jets");
	myText(0.6,0.83,kRed,"charm jets");
	myText(0.6,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_highest_S.pdf");

	//second
	h1_jet_second_highest_dca[1]->SetLineColor( kRed );
	h1_jet_second_highest_dca[2]->SetLineColor( kBlue );

	h1_jet_second_highest_dca[0]->SetTitle(";second-largest DCA [cm];counts / 0.0025 mm"); 
	h1_jet_second_highest_dca[0]->Draw();
	h1_jet_second_highest_dca[1]->Draw("same");
	h1_jet_second_highest_dca[2]->Draw("same");
	myText(0.2,0.90,kBlack,"light jets");
	myText(0.2,0.83,kRed,"charm jets");
	myText(0.2,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_second_highest_dca.pdf");

	h1_jet_second_highest_S[1]->SetLineColor( kRed );
	h1_jet_second_highest_S[2]->SetLineColor( kBlue );

	h1_jet_second_highest_S[0]->SetTitle(";second-largest #it{S}_{DCA};counts / 0.5");
	h1_jet_second_highest_S[0]->Draw();
	h1_jet_second_highest_S[1]->Draw("same");
	h1_jet_second_highest_S[2]->Draw("same");
	myText(0.6,0.90,kBlack,"light jets");
	myText(0.6,0.83,kRed,"charm jets");
	myText(0.6,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_second_highest_S.pdf");

	//third
	h1_jet_third_highest_dca[1]->SetLineColor( kRed );
	h1_jet_third_highest_dca[2]->SetLineColor( kBlue );

	h1_jet_third_highest_dca[0]->SetTitle(";third-largest DCA [cm];counts / 0.0025 mm"); 
	h1_jet_third_highest_dca[0]->Draw();
	h1_jet_third_highest_dca[1]->Draw("same");
	h1_jet_third_highest_dca[2]->Draw("same");
	myText(0.2,0.90,kBlack,"light jets");
	myText(0.2,0.83,kRed,"charm jets");
	myText(0.2,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_third_highest_dca.pdf");

	h1_jet_third_highest_S[1]->SetLineColor( kRed );
	h1_jet_third_highest_S[2]->SetLineColor( kBlue );

	h1_jet_third_highest_S[0]->SetTitle(";third-largest #it{S}_{DCA};counts / 0.5");
	h1_jet_third_highest_S[0]->Draw();
	h1_jet_third_highest_S[1]->Draw("same");
	h1_jet_third_highest_S[2]->Draw("same");
	myText(0.6,0.90,kBlack,"light jets");
	myText(0.6,0.83,kRed,"charm jets");
	myText(0.6,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_third_highest_S.pdf");

	for (int flavor = 0; flavor < 3; flavor++) {
		for (int n = 0; n < h1_jet_highest_dca_FINE[flavor]->GetNbinsX(); n++) {
			h1_jet_highest_dca_EFF[flavor]->SetBinContent( n + 1, h1_jet_highest_dca_FINE[flavor]->Integral( n + 1, -1 ) / h1_jet_highest_dca_FINE[flavor]->Integral(-1,-1) );
		}
		for (int n = 0; n < h1_jet_second_highest_dca_FINE[flavor]->GetNbinsX(); n++) {
			h1_jet_second_highest_dca_EFF[flavor]->SetBinContent( n + 1, h1_jet_second_highest_dca_FINE[flavor]->Integral( n + 1, -1 ) / h1_jet_second_highest_dca_FINE[flavor]->Integral(-1,-1) );
		}
		for (int n = 0; n < h1_jet_third_highest_dca_FINE[flavor]->GetNbinsX(); n++) {
			h1_jet_third_highest_dca_EFF[flavor]->SetBinContent( n + 1, h1_jet_third_highest_dca_FINE[flavor]->Integral( n + 1, -1 ) / h1_jet_third_highest_dca_FINE[flavor]->Integral(-1,-1) );
		}
		for (int n = 0; n < h1_jet_highest_S_FINE[flavor]->GetNbinsX(); n++) {
			h1_jet_highest_S_EFF[flavor]->SetBinContent( n + 1, h1_jet_highest_S_FINE[flavor]->Integral( n + 1, -1 ) / h1_jet_highest_S_FINE[flavor]->Integral(-1,-1) );
			//cout<<"Integral( n + 1, -1 ): "<< h1_jet_highest_S_FINE[flavor]->Integral( n + 1, -1 ) << "; Integral(-1,-1):" << h1_jet_highest_S_FINE[flavor]->Integral(-1,-1) <<endl;
		}
		for (int n = 0; n < h1_jet_second_highest_S_FINE[flavor]->GetNbinsX(); n++) {
			h1_jet_second_highest_S_EFF[flavor]->SetBinContent( n + 1, h1_jet_second_highest_S_FINE[flavor]->Integral( n + 1, -1 ) / h1_jet_second_highest_S_FINE[flavor]->Integral(-1,-1) );
		}
		for (int n = 0; n < h1_jet_third_highest_S_FINE[flavor]->GetNbinsX(); n++) {
			h1_jet_third_highest_S_EFF[flavor]->SetBinContent( n + 1, h1_jet_third_highest_S_FINE[flavor]->Integral( n + 1, -1 ) / h1_jet_third_highest_S_FINE[flavor]->Integral(-1,-1) );
		}
	}

	h1_jet_highest_dca_EFF[1]->SetLineColor( kRed );
	h1_jet_highest_dca_EFF[2]->SetLineColor( kBlue );
	h1_jet_highest_dca_EFF[0]->SetTitle(";DCA_{min} [cm];efficiency");
	h1_jet_highest_dca_EFF[0]->Draw();
	h1_jet_highest_dca_EFF[1]->Draw("same");
	h1_jet_highest_dca_EFF[2]->Draw("same");

	myText(0.5,0.90,kBlack,"DCA_{largest} > DCA_{min} cut");
	myText(0.7,0.83,kBlack,"light jets");
	myText(0.7,0.76,kRed,"charm jets");
	myText(0.7,0.69,kBlue,"bottom jets");

	tc->Print("plot/h1_eff_highest_dca.pdf");

	h1_jet_second_highest_dca_EFF[1]->SetLineColor( kRed );
	h1_jet_second_highest_dca_EFF[2]->SetLineColor( kBlue );
	h1_jet_second_highest_dca_EFF[0]->SetTitle(";DCA_{min} [cm];efficiency");
	h1_jet_second_highest_dca_EFF[0]->Draw();
	h1_jet_second_highest_dca_EFF[1]->Draw("same");
	h1_jet_second_highest_dca_EFF[2]->Draw("same");

	myText(0.5,0.90,kBlack,"DCA_{2nd-largest} > DCA_{min} cut");
	myText(0.7,0.83,kBlack,"light jets");
	myText(0.7,0.76,kRed,"charm jets");
	myText(0.7,0.69,kBlue,"bottom jets");

	tc->Print("plot/h1_eff_second_highest_dca.pdf");

	h1_jet_third_highest_dca_EFF[1]->SetLineColor( kRed );
	h1_jet_third_highest_dca_EFF[2]->SetLineColor( kBlue );
	h1_jet_third_highest_dca_EFF[0]->SetTitle(";DCA_{min} [cm];efficiency");
	h1_jet_third_highest_dca_EFF[0]->Draw();
	h1_jet_third_highest_dca_EFF[1]->Draw("same");
	h1_jet_third_highest_dca_EFF[2]->Draw("same");

	myText(0.5,0.90,kBlack,"DCA_{3rd-largest} > DCA_{min} cut");
	myText(0.7,0.83,kBlack,"light jets");
	myText(0.7,0.76,kRed,"charm jets");
	myText(0.7,0.69,kBlue,"bottom jets");

	tc->Print("plot/h1_eff_third_highest_dca.pdf");

	h1_jet_highest_S_EFF[1]->SetLineColor( kRed );
	h1_jet_highest_S_EFF[2]->SetLineColor( kBlue );
	h1_jet_highest_S_EFF[0]->SetTitle(";#it{S}_{min};efficiency");
	h1_jet_highest_S_EFF[0]->Draw();
	h1_jet_highest_S_EFF[1]->Draw("same");
	h1_jet_highest_S_EFF[2]->Draw("same");

	myText(0.5,0.90,kBlack,"#it{S}_{largest} > #it{S}_{min} cut");
	myText(0.7,0.83,kBlack,"light jets");
	myText(0.7,0.76,kRed,"charm jets");
	myText(0.7,0.69,kBlue,"bottom jets");

	tc->Print("plot/h1_eff_highest_S.pdf");
	tc->Print("plot/h1_eff_highest_S.root");

	h1_jet_second_highest_S_EFF[1]->SetLineColor( kRed );
	h1_jet_second_highest_S_EFF[2]->SetLineColor( kBlue );
	h1_jet_second_highest_S_EFF[0]->SetTitle(";#it{S}_{min};efficiency");
	h1_jet_second_highest_S_EFF[0]->Draw();
	h1_jet_second_highest_S_EFF[1]->Draw("same");
	h1_jet_second_highest_S_EFF[2]->Draw("same");

	myText(0.5,0.90,kBlack,"#it{S}_{2nd-largest} > #it{S}_{min} cut");
	myText(0.7,0.83,kBlack,"light jets");
	myText(0.7,0.76,kRed,"charm jets");
	myText(0.7,0.69,kBlue,"bottom jets");

	tc->Print("plot/h1_eff_second_highest_S.pdf");

	h1_jet_third_highest_S_EFF[1]->SetLineColor( kRed );
	h1_jet_third_highest_S_EFF[2]->SetLineColor( kBlue );
	h1_jet_third_highest_S_EFF[0]->SetTitle(";#it{S}_{min};efficiency");
	h1_jet_third_highest_S_EFF[0]->Draw();
	h1_jet_third_highest_S_EFF[1]->Draw("same");
	h1_jet_third_highest_S_EFF[2]->Draw("same");

	myText(0.5,0.90,kBlack,"#it{S}_{3rd-largest} > #it{S}_{min} cut");
	myText(0.7,0.83,kBlack,"light jets");
	myText(0.7,0.76,kRed,"charm jets");
	myText(0.7,0.69,kBlue,"bottom jets");

	tc->Print("plot/h1_eff_third_highest_S.pdf");

	//float rates[3] = { 4.83287e-06, 5.6243e-09 * 0.35, 7.35424e-09 * 0.35 };
	//float bpurity = beff * rates[2] / ( beff * rates[2] + ceff * rates[1] + leff * rates[0] );

	// -------- Rejection
	TGraph *tg_bjetE_vs_ljetR_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_highest_dca_EFF[2]->GetBinContent( n + 1 );
		float rej = 1 /  h1_jet_highest_dca_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_highest_dca->SetPoint( tg_bjetE_vs_ljetR_highest_dca->GetN(), eff, rej );
	}

	TGraph *tg_bjetE_vs_ljetR_second_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_second_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_second_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_second_highest_dca_EFF[2]->GetBinContent( n + 1 );
		float rej = 1 /  h1_jet_second_highest_dca_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_second_highest_dca->SetPoint( tg_bjetE_vs_ljetR_second_highest_dca->GetN(), eff, rej );
	}

	TGraph *tg_bjetE_vs_ljetR_third_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_third_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_third_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_third_highest_dca_EFF[2]->GetBinContent( n + 1 );
		float rej = 1 /  h1_jet_third_highest_dca_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_third_highest_dca->SetPoint( tg_bjetE_vs_ljetR_third_highest_dca->GetN(), eff, rej );
	}

	TGraph *tg_bjetE_vs_ljetR_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_highest_S_EFF[2]->GetBinContent( n + 1 );
		float rej = 1 /  h1_jet_highest_S_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_highest_S->SetPoint( tg_bjetE_vs_ljetR_highest_S->GetN(), eff, rej );
	}

	TGraph *tg_bjetE_vs_ljetR_second_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_second_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_second_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_second_highest_S_EFF[2]->GetBinContent( n + 1 );
		float rej = 1 /  h1_jet_second_highest_S_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_second_highest_S->SetPoint( tg_bjetE_vs_ljetR_second_highest_S->GetN(), eff, rej );
	}

	TGraph *tg_bjetE_vs_ljetR_third_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_third_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_third_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_third_highest_S_EFF[2]->GetBinContent( n + 1 );
		float rej = 1 /  h1_jet_third_highest_S_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_third_highest_S->SetPoint( tg_bjetE_vs_ljetR_third_highest_S->GetN(), eff, rej );
	}

	// -------- Purity
	const float plot_min_bjet_eff_cut = 0.1; 
	TGraph *tg_bjetE_vs_bjetP_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_highest_dca_EFF[2]->GetBinContent( n + 1 );
		if(eff < plot_min_bjet_eff_cut) continue;
		float pur = h1_jet_highest_dca_FINE[2]->Integral( n + 1, -1 ) /
			(h1_jet_highest_dca_FINE[0]->Integral( n + 1, -1 )+
			 h1_jet_highest_dca_FINE[1]->Integral( n + 1, -1 )+
			 h1_jet_highest_dca_FINE[2]->Integral( n + 1, -1 ));
		tg_bjetE_vs_bjetP_highest_dca->SetPoint( tg_bjetE_vs_bjetP_highest_dca->GetN(), eff, pur );
	}

	TGraph *tg_bjetE_vs_bjetP_second_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_second_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_second_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_second_highest_dca_EFF[2]->GetBinContent( n + 1 );
		if(eff < plot_min_bjet_eff_cut) continue;
		float pur = h1_jet_second_highest_dca_FINE[2]->Integral( n + 1, -1 ) /
			(h1_jet_second_highest_dca_FINE[0]->Integral( n + 1, -1 )+
			 h1_jet_second_highest_dca_FINE[1]->Integral( n + 1, -1 )+
			 h1_jet_second_highest_dca_FINE[2]->Integral( n + 1, -1 ));
		tg_bjetE_vs_bjetP_second_highest_dca->SetPoint( tg_bjetE_vs_bjetP_second_highest_dca->GetN(), eff, pur );
	}

	TGraph *tg_bjetE_vs_bjetP_third_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_third_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_third_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_third_highest_dca_EFF[2]->GetBinContent( n + 1 );
		if(eff < plot_min_bjet_eff_cut) continue;
		float pur = h1_jet_third_highest_dca_FINE[2]->Integral( n + 1, -1 ) /
			(h1_jet_third_highest_dca_FINE[0]->Integral( n + 1, -1 )+
			 h1_jet_third_highest_dca_FINE[1]->Integral( n + 1, -1 )+
			 h1_jet_third_highest_dca_FINE[2]->Integral( n + 1, -1 ));
		tg_bjetE_vs_bjetP_third_highest_dca->SetPoint( tg_bjetE_vs_bjetP_third_highest_dca->GetN(), eff, pur );
	}

	//const float plot_min_S_cut = 1.;
	const float plot_max_S_cut = 10.;
	TGraph *tg_bjetE_vs_bjetP_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_highest_S_EFF[2]->GetBinContent( n + 1 );
		if(eff < plot_min_bjet_eff_cut) continue;
		//if(h1_jet_highest_S_EFF[0]->GetBinCenter(n+1) < plot_min_S_cut) continue;
		if(h1_jet_highest_S_EFF[0]->GetBinCenter(n+1) > plot_max_S_cut) continue;
		float pur = h1_jet_highest_S_FINE[2]->Integral( n + 1, -1 ) /
			(h1_jet_highest_S_FINE[0]->Integral( n + 1, -1 )+
			 h1_jet_highest_S_FINE[1]->Integral( n + 1, -1 )+
			 h1_jet_highest_S_FINE[2]->Integral( n + 1, -1 ));
		tg_bjetE_vs_bjetP_highest_S->SetPoint( tg_bjetE_vs_bjetP_highest_S->GetN(), eff, pur );
	}

	TGraph *tg_bjetE_vs_bjetP_second_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_second_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_second_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_second_highest_S_EFF[2]->GetBinContent( n + 1 );
		if(eff < plot_min_bjet_eff_cut) continue;
		//if(h1_jet_second_highest_S_EFF[0]->GetBinCenter(n+1) < plot_min_S_cut) continue;
		if(h1_jet_second_highest_S_EFF[0]->GetBinCenter(n+1) > plot_max_S_cut) continue;
		float pur = h1_jet_second_highest_S_FINE[2]->Integral( n + 1, -1 ) /
			(h1_jet_second_highest_S_FINE[0]->Integral( n + 1, -1 )+
			 h1_jet_second_highest_S_FINE[1]->Integral( n + 1, -1 )+
			 h1_jet_second_highest_S_FINE[2]->Integral( n + 1, -1 ));
		tg_bjetE_vs_bjetP_second_highest_S->SetPoint( tg_bjetE_vs_bjetP_second_highest_S->GetN(), eff, pur );
	}

	TGraph *tg_bjetE_vs_bjetP_third_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_third_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_third_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_third_highest_S_EFF[2]->GetBinContent( n + 1 );
		if(eff < plot_min_bjet_eff_cut) continue;
		//if(h1_jet_third_highest_S_EFF[0]->GetBinCenter(n+1) < plot_min_S_cut) continue;
		if(h1_jet_third_highest_S_EFF[0]->GetBinCenter(n+1) > plot_max_S_cut) continue;
		float pur = h1_jet_third_highest_S_FINE[2]->Integral( n + 1, -1 ) /
			(h1_jet_third_highest_S_FINE[0]->Integral( n + 1, -1 )+
			 h1_jet_third_highest_S_FINE[1]->Integral( n + 1, -1 )+
			 h1_jet_third_highest_S_FINE[2]->Integral( n + 1, -1 ));
		tg_bjetE_vs_bjetP_third_highest_S->SetPoint( tg_bjetE_vs_bjetP_third_highest_S->GetN(), eff, pur );
	}

	TH1D *hFrame = new TH1D("hFrame",";#it{b}-jet efficiency;#it{l}-jet rejection (1/efficiency)",1,0,1.05);
	hFrame->GetYaxis()->SetRangeUser(0.5, 1e+5);

	hFrame->Draw();

	tg_bjetE_vs_ljetR_highest_dca->SetLineWidth(2);
	tg_bjetE_vs_ljetR_second_highest_dca->SetLineWidth(2);
	tg_bjetE_vs_ljetR_third_highest_dca->SetLineWidth(2);

	tg_bjetE_vs_ljetR_highest_dca->SetLineColor(kBlack);
	tg_bjetE_vs_ljetR_second_highest_dca->SetLineColor(kRed);
	tg_bjetE_vs_ljetR_third_highest_dca->SetLineColor(kYellow+2);

	tg_bjetE_vs_ljetR_highest_S->SetLineWidth(2);
	tg_bjetE_vs_ljetR_second_highest_S->SetLineWidth(2);
	tg_bjetE_vs_ljetR_third_highest_S->SetLineWidth(2);

	tg_bjetE_vs_ljetR_highest_S->SetLineColor(kBlue);
	tg_bjetE_vs_ljetR_second_highest_S->SetLineColor(kGreen+2);
	tg_bjetE_vs_ljetR_third_highest_S->SetLineColor(kMagenta+2);

	tg_bjetE_vs_ljetR_highest_dca->Draw("L,same");
	tg_bjetE_vs_ljetR_second_highest_dca->Draw("L,same");
	tg_bjetE_vs_ljetR_third_highest_dca->Draw("L,same");
	tg_bjetE_vs_ljetR_highest_S->Draw("L,same");
	tg_bjetE_vs_ljetR_second_highest_S->Draw("L,same");
	tg_bjetE_vs_ljetR_third_highest_S->Draw("L,same");

	myText(0.4, 0.9,kBlack,  		Form("Pythia8, #approx%2.0f GeV jets, %s tracking", simulation_jet_energy, tracking_option_name));
	myText(0.75,0.83,kBlack,  		"1-track, DCA");
	myText(0.75,0.76,kRed,    		"2-track, DCA");
	myText(0.75,0.69,kYellow+2,  "3-track, DCA");
	myText(0.75,0.62,kBlue,   		"1-track, #it{S}_{DCA}");
	myText(0.75,0.55,kGreen+2,		"2-track, #it{S}_{DCA}");
	myText(0.75,0.48,kMagenta+2,	"3-track, #it{S}_{DCA}");

	tc->Print("plot/tg_bjetE_vs_ljetR_algs.pdf");
	tc->Print("plot/tg_bjetE_vs_ljetR_algs.root");

	//------------
	TH1D *hFrame_bjetE_vs_bjetP = new TH1D("hFrame_bjetE_vs_bjetP",";#it{b}-jet efficiency;#it{b}-jet purity",1,0,1.05);
	hFrame_bjetE_vs_bjetP->GetYaxis()->SetRangeUser(0, 1.);

	int line_width = 4;

	hFrame_bjetE_vs_bjetP->Draw();

	tg_bjetE_vs_bjetP_highest_dca->SetLineWidth(line_width);
	tg_bjetE_vs_bjetP_second_highest_dca->SetLineWidth(line_width);
	tg_bjetE_vs_bjetP_third_highest_dca->SetLineWidth(line_width);

	tg_bjetE_vs_bjetP_highest_S->SetLineWidth(line_width);
	tg_bjetE_vs_bjetP_second_highest_S->SetLineWidth(line_width);
	tg_bjetE_vs_bjetP_third_highest_S->SetLineWidth(line_width);

	tg_bjetE_vs_bjetP_highest_dca->SetLineColor(kBlack);
	tg_bjetE_vs_bjetP_second_highest_dca->SetLineColor(kRed);
	tg_bjetE_vs_bjetP_third_highest_dca->SetLineColor(kYellow+2);

	tg_bjetE_vs_bjetP_highest_S->SetLineColor(kGreen+2);
	tg_bjetE_vs_bjetP_second_highest_S->SetLineColor(kRed);
	tg_bjetE_vs_bjetP_third_highest_S->SetLineColor(kBlue);

	//tg_bjetE_vs_bjetP_highest_dca->Draw("L,same");
	//tg_bjetE_vs_bjetP_second_highest_dca->Draw("L,same");
	//tg_bjetE_vs_bjetP_third_highest_dca->Draw("L,same");

	tg_bjetE_vs_bjetP_highest_S->Draw("L,same");
	tg_bjetE_vs_bjetP_second_highest_S->Draw("L,same");
	tg_bjetE_vs_bjetP_third_highest_S->Draw("L,same");

	//myText(0.4, 0.9,kBlack,  		Form("Pythia8, #approx%2.0f GeV jets, %s tracking", simulation_jet_energy, tracking_option_name));
	//myText(0.75,0.83,kBlack,  		"1-track, DCA");
	//myText(0.75,0.76,kRed,    		"2-track, DCA");
	//myText(0.75,0.69,kYellow+2,  "3-track, DCA");
	//myText(0.75,0.62,kBlue,   		"1-track, #it{S}_{DCA}");
	//myText(0.75,0.55,kGreen+2,		"2-track, #it{S}_{DCA}");
	//myText(0.75,0.48,kMagenta+2,	"3-track, #it{S}_{DCA}");

	gROOT->LoadMacro("add_purity_text.C");
	add_purity_text(tracking_option_name);


	gPad->SetGrid(0,0);

	gPad->SetLogy(0);
	tc->Print("plot/tg_bjetE_vs_bjetP_algs.pdf");
	tc->Print("plot/tg_bjetE_vs_bjetP_algs.root");
	//------------
	//SetOKStyle();
	gPad->SetGrid(1,1);

	gPad->SetLogy(1);

	{
		h1_particle_pt[1]->SetLineColor( kRed );
		h1_particle_pt[2]->SetLineColor( kBlue );
		h1_particle_pt[0]->SetTitle(";#it{p}_{T}^{#it{h}} [GeV];counts / GeV");
		h1_particle_pt[0]->Draw();
		h1_particle_pt[1]->Draw("same");
		h1_particle_pt[2]->Draw("same");

		myText(0.6,0.90,kBlack,"light jets");
		myText(0.6,0.83,kRed,"charm jets");
		myText(0.6,0.76,kBlue,"bottom jets");

		tc->Print("plot/h1_particle_pt.pdf");

		h1_particle_dca[1]->SetLineColor( kRed );
		h1_particle_dca[2]->SetLineColor( kBlue );
		h1_particle_dca[0]->SetTitle(";production vertex #it{r} [mm];counts / 0.25 mm");

		h1_particle_dca[0]->Draw();
		h1_particle_dca[1]->Draw("same");
		h1_particle_dca[2]->Draw("same");

		myText(0.6,0.90,kBlack,"light jets");
		myText(0.6,0.83,kRed,"charm jets");
		myText(0.6,0.76,kBlue,"bottom jets");

		tc->Print("plot/h1_particle_dca.pdf");

		/*
			 h1_particle_pt_dca0[1]->SetLineColor( kRed );
			 h1_particle_pt_dca0[2]->SetLineColor( kBlue );
			 h1_particle_pt_dca0[0]->Draw();
			 h1_particle_pt_dca0[1]->Draw("same");
			 h1_particle_pt_dca0[2]->Draw("same");
			 tc->Print("plot/h1_particle_pt_dca0.pdf");

			 h1_particle_pt_dca1[1]->SetLineColor( kRed );
			 h1_particle_pt_dca1[2]->SetLineColor( kBlue );
			 h1_particle_pt_dca1[0]->Draw();
			 h1_particle_pt_dca1[1]->Draw("same");
			 h1_particle_pt_dca1[2]->Draw("same");
			 tc->Print("plot/h1_particle_pt_dca1.pdf");
			 */

	}

	{
		h1_track_pt[1]->SetLineColor( kRed );
		h1_track_pt[2]->SetLineColor( kBlue );
		h1_track_pt[0]->SetTitle(";#it{p}_{T} [GeV];counts / GeV");
		h1_track_pt[0]->Draw();
		h1_track_pt[1]->Draw("same");
		h1_track_pt[2]->Draw("same");

		myText(0.6,0.90,kBlack,"light jets");
		myText(0.6,0.83,kRed,"charm jets");
		myText(0.6,0.76,kBlue,"bottom jets");

		tc->Print("plot/h1_track_pt.pdf");

		h1_track_pt_sec[0]->SetLineColor( kBlue );
		h1_track_pt_fake[0]->SetLineColor( kRed );
		h1_track_pt[0]->Draw();
		h1_track_pt_sec[0]->Draw("same");
		h1_track_pt_fake[0]->Draw("same");



		tc->Print("plot/h1_track_pt_ljet.pdf");

		h1_track_pt_sec[1]->SetLineColor( kBlue );
		h1_track_pt_fake[1]->SetLineColor( kRed );
		h1_track_pt[1]->Draw();
		h1_track_pt_sec[1]->Draw("same");
		h1_track_pt_fake[1]->Draw("same");
		tc->Print("plot/h1_track_pt_cjet.pdf");

		h1_track_pt_sec[2]->SetLineColor( kBlue );
		h1_track_pt_fake[2]->SetLineColor( kRed );
		h1_track_pt[2]->Draw();
		h1_track_pt_sec[2]->Draw("same");
		h1_track_pt_fake[2]->Draw("same");
		tc->Print("plot/h1_track_pt_bjet.pdf");

		h1_track_dca[1]->SetLineColor( kRed );
		h1_track_dca[2]->SetLineColor( kBlue );
		h1_track_dca[0]->SetTitle(";DCA [cm];counts / 0.005 cm");

		TH1D* h1_track_dca_Scale[3];

		for(int i=0;i<3;i++) {
			h1_track_dca_Scale[i] = (TH1D*)h1_track_dca[i]->Clone(Form("h1_track_dca_Scale_%1d",i));
			h1_track_dca_Scale[i]->Scale(1./h1_track_dca[i]->Integral());
		}

		h1_track_dca_Scale[0]->Draw();
		h1_track_dca_Scale[1]->Draw("same");
		h1_track_dca_Scale[2]->Draw("same");

		myText(0.65,0.90,kBlack,"light jets");
		myText(0.65,0.83,kRed,"charm jets");
		myText(0.65,0.76,kBlue,"bottom jets");

		tc->Print("plot/h1_track_dca_scale.pdf");
	}

	{
		h1_particle_pt_species[0][0]->SetLineColor(kBlack);
		h1_particle_pt_species[1][0]->SetLineColor(kRed);
		h1_particle_pt_species[2][0]->SetLineColor(kBlue);
		h1_particle_pt_species[3][0]->SetLineColor(kGreen+2);
		h1_particle_pt_species[4][0]->SetLineColor(kMagenta+3);

		h1_particle_pt_species[0][0]->SetTitle(";#it{p}_{T} [GeV];counts / GeV");
		h1_particle_pt_species[0][0]->SetMinimum(0.5);
		h1_particle_pt_species[0][0]->Draw();
		h1_particle_pt_species[1][0]->Draw("same");
		h1_particle_pt_species[2][0]->Draw("same");
		h1_particle_pt_species[3][0]->Draw("same");
		h1_particle_pt_species[4][0]->Draw("same");

		myText(0.75,0.90,kBlack,"light jets");

		myText(0.8,0.83,kBlack,    "#it{#pi}^{#pm}");
		myText(0.8,0.76,kRed,      "#it{K}^{#pm}");
		myText(0.8,0.69,kBlue,     "#it{p},#it{#bar{p}}");
		myText(0.8,0.62,kGreen+2,  "#it{e}^{#pm}");
		myText(0.8,0.55,kMagenta+3,"#it{#mu}^{#pm}");

		tc->Print("plot/h1_particle_pt_species_ljet.pdf");

		h1_particle_pt_species[0][1]->SetLineColor(kBlack);
		h1_particle_pt_species[1][1]->SetLineColor(kRed);
		h1_particle_pt_species[2][1]->SetLineColor(kBlue);
		h1_particle_pt_species[3][1]->SetLineColor(kGreen+2);
		h1_particle_pt_species[4][1]->SetLineColor(kMagenta+3);

		h1_particle_pt_species[0][1]->SetTitle(";#it{p}_{T} [GeV];counts / GeV");
		h1_particle_pt_species[0][1]->SetMinimum(0.5);
		h1_particle_pt_species[0][1]->Draw();
		h1_particle_pt_species[1][1]->Draw("same");
		h1_particle_pt_species[2][1]->Draw("same");
		h1_particle_pt_species[3][1]->Draw("same");
		h1_particle_pt_species[4][1]->Draw("same");

		myText(0.75,0.90,kBlack,"charm jets");

		myText(0.8,0.83,kBlack,    "#it{#pi}^{#pm}");
		myText(0.8,0.76,kRed,      "#it{K}^{#pm}");
		myText(0.8,0.69,kBlue,     "#it{p},#it{#bar{p}}");
		myText(0.8,0.62,kGreen+2,  "#it{e}^{#pm}");
		myText(0.8,0.55,kMagenta+3,"#it{#mu}^{#pm}");

		tc->Print("plot/h1_particle_pt_species_cjet.pdf");

		h1_particle_pt_species[0][2]->SetLineColor(kBlack);
		h1_particle_pt_species[1][2]->SetLineColor(kRed);
		h1_particle_pt_species[2][2]->SetLineColor(kBlue);
		h1_particle_pt_species[3][2]->SetLineColor(kGreen+2);
		h1_particle_pt_species[4][2]->SetLineColor(kMagenta+3);

		h1_particle_pt_species[0][2]->SetTitle(";#it{p}_{T} [GeV];counts / GeV");
		h1_particle_pt_species[0][2]->SetMinimum(0.5);
		h1_particle_pt_species[0][2]->Draw();
		h1_particle_pt_species[1][2]->Draw("same");
		h1_particle_pt_species[2][2]->Draw("same");
		h1_particle_pt_species[3][2]->Draw("same");
		h1_particle_pt_species[4][2]->Draw("same");

		myText(0.75,0.90,kBlack,"bottom jets");

		myText(0.8,0.83,kBlack,    "#it{#pi}^{#pm}");
		myText(0.8,0.76,kRed,      "#it{K}^{#pm}");
		myText(0.8,0.69,kBlue,     "#it{p},#it{#bar{p}}");
		myText(0.8,0.62,kGreen+2,  "#it{e}^{#pm}");
		myText(0.8,0.55,kMagenta+3,"#it{#mu}^{#pm}");

		tc->Print("plot/h1_particle_pt_species_bjet.pdf");

		//

		h1_particle_dca_species[0][0]->SetLineColor(kBlack);
		h1_particle_dca_species[1][0]->SetLineColor(kRed);
		h1_particle_dca_species[2][0]->SetLineColor(kBlue);
		h1_particle_dca_species[3][0]->SetLineColor(kGreen+2);
		h1_particle_dca_species[4][0]->SetLineColor(kMagenta+3);

		h1_particle_dca_species[0][0]->SetTitle(";production vertex #it{r} [mm];counts / 0.25 mm");
		h1_particle_dca_species[0][0]->SetMinimum(0.5);
		h1_particle_dca_species[0][0]->Draw();
		h1_particle_dca_species[1][0]->Draw("same");
		h1_particle_dca_species[2][0]->Draw("same");
		h1_particle_dca_species[3][0]->Draw("same");
		h1_particle_dca_species[4][0]->Draw("same");

		myText(0.75,0.90,kBlack,"light jets");

		myText(0.8,0.83,kBlack,    "#it{#pi}^{#pm}");
		myText(0.8,0.76,kRed,      "#it{K}^{#pm}");
		myText(0.8,0.69,kBlue,     "#it{p},#it{#bar{p}}");
		myText(0.8,0.62,kGreen+2,  "#it{e}^{#pm}");
		myText(0.8,0.55,kMagenta+3,"#it{#mu}^{#pm}");

		tc->Print("plot/h1_particle_dca_species_ljet.pdf");

		h1_particle_dca_species[0][1]->SetLineColor(kBlack);
		h1_particle_dca_species[1][1]->SetLineColor(kRed);
		h1_particle_dca_species[2][1]->SetLineColor(kBlue);
		h1_particle_dca_species[3][1]->SetLineColor(kGreen+2);
		h1_particle_dca_species[4][1]->SetLineColor(kMagenta+3);

		h1_particle_dca_species[0][1]->SetTitle(";production vertex #it{r} [mm];counts / 0.25 mm");
		h1_particle_dca_species[0][1]->SetMinimum(0.5);
		h1_particle_dca_species[0][1]->Draw();
		h1_particle_dca_species[1][1]->Draw("same");
		h1_particle_dca_species[2][1]->Draw("same");
		h1_particle_dca_species[3][1]->Draw("same");
		h1_particle_dca_species[4][1]->Draw("same");

		myText(0.75,0.90,kBlack,"charm jets");

		myText(0.8,0.83,kBlack,    "#it{#pi}^{#pm}");
		myText(0.8,0.76,kRed,      "#it{K}^{#pm}");
		myText(0.8,0.69,kBlue,     "#it{p},#it{#bar{p}}");
		myText(0.8,0.62,kGreen+2,  "#it{e}^{#pm}");
		myText(0.8,0.55,kMagenta+3,"#it{#mu}^{#pm}");

		tc->Print("plot/h1_particle_dca_species_cjet.pdf");

		h1_particle_dca_species[0][2]->SetLineColor(kBlack);
		h1_particle_dca_species[1][2]->SetLineColor(kRed);
		h1_particle_dca_species[2][2]->SetLineColor(kBlue);
		h1_particle_dca_species[3][2]->SetLineColor(kGreen+2);
		h1_particle_dca_species[4][2]->SetLineColor(kMagenta+3);

		h1_particle_dca_species[0][2]->SetTitle(";production vertex #it{r} [mm];counts / 0.25 mm");
		h1_particle_dca_species[0][2]->SetMinimum(0.5);
		h1_particle_dca_species[0][2]->Draw();
		h1_particle_dca_species[1][2]->Draw("same");
		h1_particle_dca_species[2][2]->Draw("same");
		h1_particle_dca_species[3][2]->Draw("same");
		h1_particle_dca_species[4][2]->Draw("same");

		myText(0.75,0.90,kBlack,"bottom jets");

		myText(0.8,0.83,kBlack,    "#it{#pi}^{#pm}");
		myText(0.8,0.76,kRed,      "#it{K}^{#pm}");
		myText(0.8,0.69,kBlue,     "#it{p},#it{#bar{p}}");
		myText(0.8,0.62,kGreen+2,  "#it{e}^{#pm}");
		myText(0.8,0.55,kMagenta+3,"#it{#mu}^{#pm}");

		tc->Print("plot/h1_particle_dca_species_bjet.pdf");

	}


	h1_jet_highest_S_FINE[1]->SetLineColor( kRed );
	h1_jet_highest_S_FINE[2]->SetLineColor( kBlue );

	h1_jet_highest_S_FINE[0]->GetXaxis()->SetRangeUser(-5, +14 );

	h1_jet_highest_S_FINE[0]->SetTitle(";largest #it{S}_{DCA};counts / 0.1");
	h1_jet_highest_S_FINE[0]->Draw();
	h1_jet_highest_S_FINE[1]->Draw("same");
	h1_jet_highest_S_FINE[2]->Draw("same");
	myText(0.6,0.90,kBlack,"light jets");
	myText(0.6,0.83,kRed,"charm jets");
	myText(0.6,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_highest_S_zoom.pdf");

	h1_jet_second_highest_S_FINE[1]->SetLineColor( kRed );
	h1_jet_second_highest_S_FINE[2]->SetLineColor( kBlue );

	h1_jet_second_highest_S_FINE[0]->GetXaxis()->SetRangeUser(-5, +14 );

	h1_jet_second_highest_S_FINE[0]->SetTitle(";second-largest #it{S}_{DCA};counts / 0.1");
	h1_jet_second_highest_S_FINE[0]->Draw();
	h1_jet_second_highest_S_FINE[1]->Draw("same");
	h1_jet_second_highest_S_FINE[2]->Draw("same");
	myText(0.6,0.90,kBlack,"light jets");
	myText(0.6,0.83,kRed,"charm jets");
	myText(0.6,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_second_highest_S_zoom.pdf");


	/*
		 h1_track_pt_pri_id0->SetLineColor( kBlack );
		 h1_track_pt_pri_id1->SetLineColor( kRed );
		 h1_track_pt_pri_id2->SetLineColor( kBlue );
		 h1_track_pt_pri_id3->SetLineColor( kGreen+2 );
		 h1_track_pt_pri_id4->SetLineColor( kMagenta+3 );

		 h1_track_pt_pri_id0->Draw();
		 h1_track_pt_pri_id1->Draw("same");
		 h1_track_pt_pri_id2->Draw("same");
		 h1_track_pt_pri_id3->Draw("same");
		 h1_track_pt_pri_id4->Draw("same");
		 tc->Print("plot/h1_track_pt_pri_byid.pdf");


		 h1_track_dca_pri_id0->SetLineColor( kBlack );
		 h1_track_dca_pri_id1->SetLineColor( kRed );
		 h1_track_dca_pri_id2->SetLineColor( kBlue );
		 h1_track_dca_pri_id3->SetLineColor( kGreen+2 );
		 h1_track_dca_pri_id4->SetLineColor( kMagenta+3 );

		 h1_track_dca_pri_id0->Draw();
		 h1_track_dca_pri_id1->Draw("same");
		 h1_track_dca_pri_id2->Draw("same");
		 h1_track_dca_pri_id3->Draw("same");
		 h1_track_dca_pri_id4->Draw("same");
		 tc->Print("plot/h1_track_dca_pri_byid.pdf");


		 h1_track_dcareco_pri_id0->SetLineColor( kBlack );
		 h1_track_dcareco_pri_id1->SetLineColor( kRed );
		 h1_track_dcareco_pri_id2->SetLineColor( kBlue );
		 h1_track_dcareco_pri_id3->SetLineColor( kGreen+2 );
		 h1_track_dcareco_pri_id4->SetLineColor( kMagenta+3 );

		 h1_track_dcareco_pri_id0->Draw();
		 h1_track_dcareco_pri_id1->Draw("same");
		 h1_track_dcareco_pri_id2->Draw("same");
		 h1_track_dcareco_pri_id3->Draw("same");
		 h1_track_dcareco_pri_id4->Draw("same");
		 tc->Print("plot/h1_track_dcareco_pri_byid.pdf");

		 h1_track_Sreco_pri_id0->SetLineColor( kBlack );
		 h1_track_Sreco_pri_id1->SetLineColor( kRed );
		 h1_track_Sreco_pri_id2->SetLineColor( kBlue );
		 h1_track_Sreco_pri_id3->SetLineColor( kGreen+2 );
		 h1_track_Sreco_pri_id4->SetLineColor( kMagenta+3 );

		 h1_track_Sreco_pri_id0->Draw();
		 h1_track_Sreco_pri_id1->Draw("same");
		 h1_track_Sreco_pri_id2->Draw("same");
		 h1_track_Sreco_pri_id3->Draw("same");
		 h1_track_Sreco_pri_id4->Draw("same");
		 tc->Print("plot/h1_track_Sreco_pri_byid.pdf");


		 h1_track_pt_pri_parentid0->SetLineColor( kBlack );
		 h1_track_pt_pri_parentid1->SetLineColor( kRed );
		 h1_track_pt_pri_parentid2->SetLineColor( kBlue );
		 h1_track_pt_pri_parentid3->SetLineColor( kGreen+2 );
		 h1_track_pt_pri_parentid4->SetLineColor( kMagenta+3 );
		 h1_track_pt_pri_parentid5->SetLineColor( kOrange+7 );

		 h1_track_pt_pri_parentid0->Draw();
		 h1_track_pt_pri_parentid1->Draw("same");
		 h1_track_pt_pri_parentid2->Draw("same");
		 h1_track_pt_pri_parentid3->Draw("same");
		 h1_track_pt_pri_parentid4->Draw("same");
		 h1_track_pt_pri_parentid5->Draw("same");
		 tc->Print("plot/h1_track_pt_pri_byparentid.pdf");


	h1_track_dca_pri_parentid0->SetLineColor( kBlack );
	h1_track_dca_pri_parentid1->SetLineColor( kRed );
	h1_track_dca_pri_parentid2->SetLineColor( kBlue );
	h1_track_dca_pri_parentid3->SetLineColor( kGreen+2 );
	h1_track_dca_pri_parentid4->SetLineColor( kMagenta+3 );
	h1_track_dca_pri_parentid5->SetLineColor( kOrange+7 );

	h1_track_dca_pri_parentid0->Draw();
	h1_track_dca_pri_parentid1->Draw("same");
	h1_track_dca_pri_parentid2->Draw("same");
	h1_track_dca_pri_parentid3->Draw("same");
	h1_track_dca_pri_parentid4->Draw("same");
	h1_track_dca_pri_parentid5->Draw("same");
	tc->Print("plot/h1_track_dca_pri_byparentid.pdf");


	h1_track_dcareco_pri_parentid0->SetLineColor( kBlack );
	h1_track_dcareco_pri_parentid1->SetLineColor( kRed );
	h1_track_dcareco_pri_parentid2->SetLineColor( kBlue );
	h1_track_dcareco_pri_parentid3->SetLineColor( kGreen+2 );
	h1_track_dcareco_pri_parentid4->SetLineColor( kMagenta+3 );
	h1_track_dcareco_pri_parentid5->SetLineColor( kOrange+7 );

	h1_track_dcareco_pri_parentid0->Draw();
	h1_track_dcareco_pri_parentid1->Draw("same");
	h1_track_dcareco_pri_parentid2->Draw("same");
	h1_track_dcareco_pri_parentid3->Draw("same");
	h1_track_dcareco_pri_parentid4->Draw("same");
	h1_track_dcareco_pri_parentid5->Draw("same");
	tc->Print("plot/h1_track_dcareco_pri_byparentid.pdf");

	h1_track_Sreco_pri_parentid0->SetLineColor( kBlack );
	h1_track_Sreco_pri_parentid1->SetLineColor( kRed );
	h1_track_Sreco_pri_parentid2->SetLineColor( kBlue );
	h1_track_Sreco_pri_parentid3->SetLineColor( kGreen+2 );
	h1_track_Sreco_pri_parentid4->SetLineColor( kMagenta+3 );
	h1_track_Sreco_pri_parentid5->SetLineColor( kOrange+7 );

	h1_track_Sreco_pri_parentid0->Draw();
	h1_track_Sreco_pri_parentid1->Draw("same");
	h1_track_Sreco_pri_parentid2->Draw("same");
	h1_track_Sreco_pri_parentid3->Draw("same");
	h1_track_Sreco_pri_parentid4->Draw("same");
	h1_track_Sreco_pri_parentid5->Draw("same");
	tc->Print("plot/h1_track_Sreco_pri_byparentid.pdf");

	*/

		fout->cd();
	T->Write();
}

#include "TFile.h"
#include "TTree.h"

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TSystem.h"
#include "TVector3.h"

#include <iostream>
#include <sstream>
#include <climits>

#include "AtlasUtils.C"
#include "/phenix/u/yuhw/RootMacros/sPHENIXStyle/sPhenixStyle.C"
#include "add_purity_text.C"

float binorminal_error(float a, float b) {
	if(a > b || a < 0 || b < 0) return 9999.;
	float e = a/b;
	return sqrt(e*(1-e)/b);
}

float deltaR( float eta1, float eta2, float phi1, float phi2 ) {

	float deta = eta1 - eta2;
	float dphi = phi1 - phi2;
	if (dphi > 3.14159) dphi -= 2*3.14159;
	if (dphi < -3.14159) dphi += 2*3.14159;

	return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );

}

#	define _MAX_N_PARTICLES_ 2000
#	define _MAX_N_TRACKS_ 2000

void draw_G4_dibjet_truth_tagging(
		const char* input = "MIE_1M/HFtag_jet.root",
		const TString tag_method = "Parton", // Parton, Hadron
		const char* tracking_option_name = "2014 proposal tracker",
		const int dca_method = 4, // direct from g4hough, reco-vertex and strait line assumption, dca3d reco-vertex and strait line assumption
		const double max_track_quality_cut = 1.5,
		const double min_track_pt_cut = 1.0,
		const double max_dca_cut = 0.1,
		const int min_MAPS_hits = 2
		) {


#define N_DI_JET_TYPE 2
	enum DI_JET_TYPE {DiBJet, OtherDiJet};

	const double _max_jet_eta = 0.6;

	const double _min_leading_jet_pt = 20;
	const double _min_subleading_jet_pt = 10;

	const double _plot_min_bjet_eff_cut = 0.05;

	const int max_fake_cluster = 1000;
	const int jet_embed_flag = 10;

	const double simulation_jet_energy = 20;


	//gROOT->LoadMacro("SetOKStyle.C");
	//SetOKStyle();
	SetsPhenixStyle();

	TFile *fout = TFile::Open("dca_eval.root","recreate");
	fout->cd();
	TTree *T = new TTree("T","eval tree");
	int _jet_parton_flavor;
	int _n_tracks;
	float _track_pt[_MAX_N_TRACKS_];
	float _chi2_ndf[_MAX_N_TRACKS_];
	float _dca2d[_MAX_N_TRACKS_];
	float _dca2d_error[_MAX_N_TRACKS_];
	float _highest_dca2d;
	float _highest_dca2d_error;
	float _second_highest_dca2d;
	float _second_highest_dca2d_error;
	float _highest_S;
	float _second_highest_S;
	T->Branch("jet_parton_flavor",&_jet_parton_flavor,"jet_parton_flavor/I");
	T->Branch("n_tracks",&_n_tracks,"n_tracks/I");
	T->Branch("track_pt",_track_pt,"track_pt[n_tracks]/F");
	T->Branch("chi2_ndf",_chi2_ndf,"chi2_ndf[n_tracks]/F");
	T->Branch("dca2d",_dca2d,"dca2d[n_tracks]/F");
	T->Branch("dca2d_error",_dca2d_error,"dca2d_error[n_tracks]/F");
	T->Branch("highest_dca2d",&_highest_dca2d,"highest_dca2d/F");
	T->Branch("highest_dca2d_error",&_highest_dca2d_error,"highest_dca2d_error/F");
	T->Branch("second_highest_dca2d",&_second_highest_dca2d,"second_highest_dca2d/F");
	T->Branch("second_highest_dca2d_error",&_second_highest_dca2d_error,"second_highest_dca2d_error/F");
	T->Branch("highest_S",&_highest_S,"highest_S/F");
	T->Branch("second_highest_S",&_second_highest_S,"second_highest_S/F");

	TH1D *h1_jet_pt[3];

	TH1D *h1_track_best_pt_embed_0_10GeV_pion[3];
	TH1D *h1_particle_pt_embed_0_10GeV_pion[3];  


	TH1D *h1_track_pt[3];
	TH1D *h1_track_pt_pri[3];
	TH1D *h1_track_pt_fake[3];
	TH1D *h1_track_pt_sec[3];

	TH1D *h1_particle_pt[3];  
	TH1D *h1_particle_pt_0_10GeV_pion[3];  
	TH1D *h1_particle_pt_dca0[3];  
	TH1D *h1_particle_pt_dca1[3];  
	TH1D *h1_particle_pt_species[5][3];

	for (int flavor = 0; flavor < 3; flavor++) {

		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_track_best_pt_embed_0_10GeV_pion_" << flavor; h1_track_best_pt_embed_0_10GeV_pion[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",60,0,30); }
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_particle_pt_embed_0_10GeV_pion_" << flavor; h1_particle_pt_embed_0_10GeV_pion[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",60,0,30); }

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
		{ ostringstream temp_o_str_s; temp_o_str_s << "h1_track_dca_" << flavor; h1_track_dca[flavor] = new TH1D( temp_o_str_s.str().c_str(),"",100,-0.2,+0.2); }
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

	const double _plot_dca_fine_min = -0.1;
	const double _plot_dca_fine_max = +0.2;
	const int _plot_dca_fine_nbin = 600;

	const double _plot_sdca_fine_min = -20;
	const double _plot_sdca_fine_max = +50;
	const int _plot_sdca_fine_nbin = 700;

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

	TH2D *h2_dijet_highest_dca_FINE[N_DI_JET_TYPE];
	TH2D *h2_dijet_second_highest_dca_FINE[N_DI_JET_TYPE];
	TH2D *h2_dijet_third_highest_dca_FINE[N_DI_JET_TYPE];

	TH2D *h2_dijet_highest_S_FINE[N_DI_JET_TYPE];
	TH2D *h2_dijet_second_highest_S_FINE[N_DI_JET_TYPE];
	TH2D *h2_dijet_third_highest_S_FINE[N_DI_JET_TYPE];


	for (int i = DiBJet; i <= OtherDiJet; ++i) {
		h2_dijet_highest_dca_FINE[i] = new TH2D( Form("h2_dijet_highest_dca_FINE_%d",i),"",
				_plot_dca_fine_nbin,_plot_dca_fine_min,_plot_dca_fine_max,
				_plot_dca_fine_nbin,_plot_dca_fine_min,_plot_dca_fine_max);
		h2_dijet_second_highest_dca_FINE[i]=(TH2D*)h2_dijet_highest_dca_FINE[i]->Clone(Form("h2_dijet_second_highest_dca_FINE_%d",i));
		h2_dijet_third_highest_dca_FINE[i]=(TH2D*)h2_dijet_highest_dca_FINE[i]->Clone(Form("h2_dijet_third_highest_dca_FINE_%d",i));

		h2_dijet_highest_S_FINE[i] = new TH2D( Form("h2_dijet_highest_S_FINE_%d",i),"",
				_plot_sdca_fine_nbin,_plot_sdca_fine_min,_plot_sdca_fine_max,
				_plot_sdca_fine_nbin,_plot_sdca_fine_min,_plot_sdca_fine_max);
		h2_dijet_second_highest_S_FINE[i]=(TH2D*)h2_dijet_highest_S_FINE[i]->Clone(Form("h2_dijet_second_highest_S_FINE_%d",i));
		h2_dijet_third_highest_S_FINE[i]=(TH2D*)h2_dijet_highest_S_FINE[i]->Clone(Form("h2_dijet_third_highest_S_FINE_%d",i));

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

	int truthjet_n;
	int truthjet_parton_flavor[10];
	int truthjet_hadron_flavor[10];
	float truthjet_pt[10];
	float truthjet_eta[10];
	float truthjet_phi[10];

	int particle_n;
	unsigned int particle_embed[_MAX_N_PARTICLES_];
	float particle_pt[_MAX_N_PARTICLES_];
	float particle_eta[_MAX_N_PARTICLES_];
	float particle_phi[_MAX_N_PARTICLES_];
	float particle_dca[_MAX_N_PARTICLES_];
	int particle_pid[_MAX_N_PARTICLES_];

	int track_n;
	float track_pt[_MAX_N_TRACKS_];
	float track_eta[_MAX_N_TRACKS_];
	float track_phi[_MAX_N_TRACKS_];

	float track_dca2d[_MAX_N_TRACKS_];
	float track_dca2d_error[_MAX_N_TRACKS_];

	float track_dca2d_phi[_MAX_N_TRACKS_];
	float track_dca2d_x[_MAX_N_TRACKS_];
	float track_dca2d_y[_MAX_N_TRACKS_];
	float track_dca2d_z[_MAX_N_TRACKS_];

	float track_dca2d_calc[_MAX_N_TRACKS_];
	float track_dca2d_calc_truth[_MAX_N_TRACKS_];
	float track_dca3d_calc[_MAX_N_TRACKS_];
	float track_dca3d[_MAX_N_TRACKS_];
	float track_dca3d_error[_MAX_N_TRACKS_];
	float track_dca3d_xy[_MAX_N_TRACKS_];
	float track_dca3d_xy_error[_MAX_N_TRACKS_];
	float track_dca3d_z[_MAX_N_TRACKS_];
	float track_dca3d_z_error[_MAX_N_TRACKS_];
	float track_dca3d_calc_truth[_MAX_N_TRACKS_];

	float track_quality[_MAX_N_TRACKS_];
	float track_chisq[_MAX_N_TRACKS_];
	int track_ndf[_MAX_N_TRACKS_];

	bool track_best_primary[_MAX_N_TRACKS_];
	unsigned int track_nclusters[_MAX_N_TRACKS_];
	unsigned int track_nclusters_by_layer[_MAX_N_TRACKS_];
	unsigned int track_best_nclusters[_MAX_N_TRACKS_];
	unsigned int track_best_nclusters_by_layer[_MAX_N_TRACKS_];
	unsigned int track_best_embed[_MAX_N_TRACKS_];
	int track_best_pid[_MAX_N_TRACKS_];
	float track_best_pt[_MAX_N_TRACKS_];
	float track_best_dca[_MAX_N_TRACKS_];
	int track_best_parent_pid[_MAX_N_TRACKS_];

	TFile *f = TFile::Open(Form("%s",input),"READ");
	TTree *ttree = (TTree*) f->Get("ttree");

	ttree->SetBranchAddress("truthjet_n",   &truthjet_n );
	ttree->SetBranchAddress("truthjet_parton_flavor",   truthjet_parton_flavor );
	ttree->SetBranchAddress("truthjet_hadron_flavor",   truthjet_hadron_flavor );
	ttree->SetBranchAddress("truthjet_pt",   truthjet_pt );
	ttree->SetBranchAddress("truthjet_eta",  truthjet_eta );
	ttree->SetBranchAddress("truthjet_phi",  truthjet_phi );

	ttree->SetBranchAddress("particle_n",   &particle_n );
	ttree->SetBranchAddress("particle_embed",   particle_embed );
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
	ttree->SetBranchAddress("track_dca2d_x",  track_dca2d_x );
	ttree->SetBranchAddress("track_dca2d_y",  track_dca2d_y );
	ttree->SetBranchAddress("track_dca2d_z",  track_dca2d_z );

	ttree->SetBranchAddress("track_dca2d_calc",  track_dca2d_calc );
	ttree->SetBranchAddress("track_dca2d_calc_truth",  track_dca2d_calc_truth );
	ttree->SetBranchAddress("track_dca3d_calc",  track_dca3d_calc );
	ttree->SetBranchAddress("track_dca3d_xy",  track_dca3d_xy );
	ttree->SetBranchAddress("track_dca3d_xy_error",  track_dca3d_xy_error );
	ttree->SetBranchAddress("track_dca3d_z",  track_dca3d_z );
	ttree->SetBranchAddress("track_dca3d_z_error",  track_dca3d_z_error );
	ttree->SetBranchAddress("track_dca3d_calc_truth",  track_dca3d_calc_truth );

	ttree->SetBranchAddress("track_quality",   track_quality );
	ttree->SetBranchAddress("track_chisq",   track_chisq );
	ttree->SetBranchAddress("track_ndf",   track_ndf );

	ttree->SetBranchAddress("track_best_primary",  track_best_primary );
	ttree->SetBranchAddress("track_nclusters",  track_nclusters );
	ttree->SetBranchAddress("track_nclusters_by_layer",  track_nclusters_by_layer );
	ttree->SetBranchAddress("track_best_nclusters",  track_best_nclusters );
	ttree->SetBranchAddress("track_best_nclusters_by_layer",  track_best_nclusters_by_layer );
	ttree->SetBranchAddress("track_best_embed",  track_best_embed );
	ttree->SetBranchAddress("track_best_pid",  track_best_pid );
	ttree->SetBranchAddress("track_best_pt",  track_best_pt );
	ttree->SetBranchAddress("track_best_dca",  track_best_dca );
	ttree->SetBranchAddress("track_best_parent_pid", track_best_parent_pid );

	//int rem = 0;
	//int rems[6] = {0,0,0,0,0,0};


	int nentries = ttree->GetEntries();
	nentries = TMath::Min(1000,nentries);

	for (int e = 0 ; e < nentries; e++) {

		if(e%1000 == 0) cout<<"Processing: "<<100.*e/nentries <<"\% \n";

		ttree->GetEntry( e );

		int leading_jet = INT_MIN;
		double leading_jet_pt = -9999;
		int subleading_jet = INT_MIN;
		double subleading_jet_pt = -9999;

		for (int ijet = 0; ijet < truthjet_n; ijet++) {
			if(truthjet_pt[ijet] > _min_leading_jet_pt && truthjet_pt[ijet] > leading_jet_pt)
				leading_jet = ijet;
			else if (truthjet_pt[ijet] > _min_subleading_jet_pt && truthjet_pt[ijet] > subleading_jet_pt)
				subleading_jet = ijet;
		}

		for (int j1 = 0; j1 < truthjet_n; j1++) {

			if(j1!=leading_jet) continue;

			int iflavor = -99;
			//cout<<"event: "<<e<<" : truthjet_parton_flavor: "<< truthjet_parton_flavor[j] <<endl;
			int flavor_tag = -99;
			if(tag_method.Contains("Parton"))
				flavor_tag = truthjet_parton_flavor[j1];
			else if(tag_method.Contains("Hadron"))
				flavor_tag = truthjet_hadron_flavor[j1];

			if(abs(flavor_tag) == 0) iflavor = 0;
			else if(abs(flavor_tag) == 4) iflavor = 1;
			else if(abs(flavor_tag) == 5) iflavor = 2;
			else continue;

			if ( fabs( truthjet_eta[ j1 ]) > _max_jet_eta ) continue;
			if (truthjet_pt[ j1 ] < _min_leading_jet_pt ) continue;



			//std::cout << " event #" << e << " jet #" << j << " pt/eta/phi = " << truthjet_pt[ j ] << " / " << truthjet_eta[ j ] << " / " << truthjet_phi[ j ] << std::endl;

			h1_jet_pt[iflavor]->Fill( truthjet_pt[ j1 ] );

			int ntrk = 0;
			int ntrk1 = 0;

			for (int p = 0; p < particle_n; p++) {

				if ( particle_pid[ p ] == 16 || particle_pid[ p ] == -16) continue;

				float dR = deltaR( truthjet_eta[ j1 ], particle_eta[ p ], truthjet_phi[ j1 ], particle_phi[ p ] );
				if (dR > 0.4) continue;

				if(particle_embed[p] == jet_embed_flag  && abs(particle_pid[p]) == 211 
						&& abs(particle_eta[p]) < 1.)
					h1_particle_pt_embed_0_10GeV_pion[iflavor]->Fill( particle_pt[ p ] );

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
					//std::cout << " --> particle #" << p << " (dR = " << dR << "), pt/eta/phi = " << particle_pt[ p ] << " / " << particle_eta[ p ] << " / " << particle_phi[ p ] << ", pid = " << particle_pid[ p ] << std::endl; //yuhw
				}
			}

			float highest_S = -99;
			float highest_dca = -99;

			float second_highest_S = -99;
			float second_highest_dca = -99;

			float third_highest_S = -99;
			float third_highest_dca = -99;

			for (int itrk = 0; itrk < track_n; itrk++) {

				if(!(track_quality[itrk]<max_track_quality_cut)) continue; // yuhw

				if(!(track_pt[itrk]>min_track_pt_cut)) continue; // yuhw

				int MAPS_hits = 0;
				for(int i=0;i<3;i++){
					if((track_nclusters_by_layer[itrk] & (0x1 << i)) > 0) MAPS_hits ++;
				}
				if(MAPS_hits < min_MAPS_hits) continue; 

				if (! ( abs(track_nclusters [itrk] - track_best_nclusters[itrk]) <= max_fake_cluster) ) continue; // yuhw

				float dR = deltaR( truthjet_eta[ j1 ], track_eta[ itrk ], truthjet_phi[ j1 ], track_phi[ itrk ] );
				if (dR > 0.4) continue;


				track_dca2d[ itrk ] = fabs( track_dca2d[ itrk ] );
				if(dca_method == 1) track_dca2d[ itrk ] = fabs( track_dca2d_calc[ itrk ] ); 
				if(dca_method == 2) track_dca2d[ itrk ] = fabs( track_dca3d_calc[ itrk ] ); 
				if(dca_method == 3) { 
					track_dca2d[ itrk ] = fabs( track_dca3d[ itrk ] );
					track_dca2d_error[ itrk ] = fabs( track_dca3d_error[ itrk ] );
				}
				if(dca_method == 4) { 
					float sigmalized_dca3d = sqrt(
							pow(track_dca3d_xy[ itrk ]/track_dca3d_xy_error[ itrk ],2) +
							pow(track_dca3d_z[ itrk ]/track_dca3d_z_error[ itrk ],2)
							);
					float effective_sigma = sqrt(
							pow(track_dca3d_xy_error[ itrk ],2) +
							pow(track_dca3d_z_error[ itrk ],2)
							);
					track_dca2d[ itrk ] = sigmalized_dca3d*effective_sigma;
					track_dca2d_error[ itrk ] = effective_sigma;
				}
				if(dca_method == 5) { 
					track_dca2d[ itrk ] = fabs( track_dca3d_xy[ itrk ] );
					track_dca2d_error[ itrk ] = fabs( track_dca3d_xy_error[ itrk ] );
				}
				if(dca_method == 6) { 
					float sigmalized_dca3d = 
						pow(track_dca3d_xy[ itrk ]/track_dca3d_xy_error[ itrk ],2) +
						pow(track_dca3d_z[ itrk ]/track_dca3d_z_error[ itrk ],2)
						;
					float effective_sigma = 
						pow(track_dca3d_xy_error[ itrk ],2) +
						pow(track_dca3d_z_error[ itrk ],2)
						;
				}


				if (! (track_dca2d[ itrk ] < max_dca_cut)) continue; // yuhw

				// set sign WRT jet
				if(
						dca_method  == 0 ||
						dca_method  == 1 ||
						dca_method  == 2 ||
						dca_method  == 5
					) // dca2d methods
				{
					float dphi = track_dca2d_phi[ itrk ] - truthjet_phi[ j1 ];

					//if (iflavor == 2 && track_dca2d[ itrk ] > 0.05) 
					//	std::cout << truthjet_phi[ j ] << " / " <<  track_dca2d_phi[ itrk ] << std::endl;

					if (dphi > +3.14159) dphi -= 2*3.14159;
					if (dphi < -3.14159) dphi += 2*3.14159;
					dphi = fabs(dphi);
					if (dphi > 3.14159/2)
						track_dca2d[ itrk ] = -1 * track_dca2d[ itrk ];
				} else { 
					TVector3 dca_vec(
							track_dca2d_x[ itrk ],
							track_dca2d_y[ itrk ],
							track_dca2d_z[ itrk ]
							);
					TVector3 jet_vec(0,0,0);
					jet_vec.SetPtEtaPhi(
							truthjet_pt[j1],
							truthjet_eta[j1],
							truthjet_phi[j1]
							);
					if( dca_vec.Dot(jet_vec) < 0 ) {
						track_dca2d[ itrk ] = -1 * track_dca2d[ itrk ];	
					}
				}

				if (iflavor == 2) {
					h2_track_pt_dcaerror->Fill(  track_pt[ itrk ],  track_dca2d_error[ itrk ] );
					h2_track_dca_dcaerror->Fill(  track_dca2d[ itrk ],  track_dca2d_error[ itrk ] );
				}

				ntrk++;

				_track_pt[ntrk-1] = -99;
				_chi2_ndf[ntrk-1] = -99;
				_dca2d[ntrk-1] = -99;
				_dca2d_error[ntrk-1] = -99;

				if (track_pt[ itrk ] > 1)
					ntrk1++;

				if(track_best_embed[itrk] == jet_embed_flag && abs(track_best_pid[itrk]) == 211 ) 
					h1_track_best_pt_embed_0_10GeV_pion[iflavor]->Fill( track_best_pt[ itrk ] );

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

				_track_pt[ntrk-1] = track_pt[ itrk ];
				_chi2_ndf[ntrk-1] = track_quality[ itrk ];
				_dca2d[ntrk-1] = track_dca2d[ itrk ];
				_dca2d_error[ntrk-1] = track_dca2d_error[ itrk ];

			} // end tracks

			_jet_parton_flavor = truthjet_parton_flavor[j1];
			_n_tracks = ntrk;
			_highest_dca2d = highest_dca;
			_second_highest_dca2d = second_highest_dca;
			_highest_S = highest_S;
			_second_highest_S = second_highest_S;
			T->Fill();

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

			if(subleading_jet >= 0) {
				int j2 = subleading_jet;

				int iflavor_j2 = -99;
				int flavor_tag_j2 = -99;

				if(tag_method.Contains("Parton"))
					flavor_tag_j2 = truthjet_parton_flavor[j2];
				else if(tag_method.Contains("Hadron"))
					flavor_tag_j2 = truthjet_hadron_flavor[j2];

				if(abs(flavor_tag_j2) == 0) iflavor_j2 = 0;
				else if(abs(flavor_tag_j2) == 4) iflavor_j2 = 1;
				else if(abs(flavor_tag_j2) == 5) iflavor_j2 = 2;
				else continue;

				if ( fabs( truthjet_eta[ j2 ]) > _max_jet_eta ) continue;
				if (truthjet_pt[ j2 ] < _min_subleading_jet_pt ) continue;

				DI_JET_TYPE di_jet_type = OtherDiJet;
				if(iflavor == 2 && iflavor_j2 == 2)
					di_jet_type = DiBJet;



				float highest_S_j2 = -99;
				float highest_dca_j2 = -99;

				float second_highest_S_j2 = -99;
				float second_highest_dca_j2 = -99;

				float third_highest_S_j2 = -99;
				float third_highest_dca_j2 = -99;

				for (int itrk_j2 = 0; itrk_j2 < track_n; itrk_j2++) {

					if(!(track_quality[itrk_j2]<max_track_quality_cut)) continue; // yuhw

					if(!(track_pt[itrk_j2]>min_track_pt_cut)) continue; // yuhw

					int MAPS_hits = 0;
					for(int i=0;i<3;i++){
						if((track_nclusters_by_layer[itrk_j2] & (0x1 << i)) > 0) MAPS_hits ++;
					}
					if(MAPS_hits < min_MAPS_hits) continue;

					if (! ( abs(track_nclusters [itrk_j2] - track_best_nclusters[itrk_j2]) <= max_fake_cluster) ) continue; // yuhw

					float dR = deltaR( truthjet_eta[ j2 ], track_eta[ itrk_j2 ], truthjet_phi[ j2 ], track_phi[ itrk_j2 ] );
					if (dR > 0.4) continue;


					track_dca2d[ itrk_j2 ] = fabs( track_dca2d[ itrk_j2 ] );
					if(dca_method == 1) track_dca2d[ itrk_j2 ] = fabs( track_dca2d_calc[ itrk_j2 ] );
					if(dca_method == 2) track_dca2d[ itrk_j2 ] = fabs( track_dca3d_calc[ itrk_j2 ] );
					if(dca_method == 3) {
						track_dca2d[ itrk_j2 ] = fabs( track_dca3d[ itrk_j2 ] );
						track_dca2d_error[ itrk_j2 ] = fabs( track_dca3d_error[ itrk_j2 ] );
					}
					if(dca_method == 4) {
						float sigmalized_dca3d = sqrt(
								pow(track_dca3d_xy[ itrk_j2 ]/track_dca3d_xy_error[ itrk_j2 ],2) +
								pow(track_dca3d_z[ itrk_j2 ]/track_dca3d_z_error[ itrk_j2 ],2)
								);
						float effective_sigma = sqrt(
								pow(track_dca3d_xy_error[ itrk_j2 ],2) +
								pow(track_dca3d_z_error[ itrk_j2 ],2)
								);
						track_dca2d[ itrk_j2 ] = sigmalized_dca3d*effective_sigma;
						track_dca2d_error[ itrk_j2 ] = effective_sigma;
					}
					if(dca_method == 5) {
						track_dca2d[ itrk_j2 ] = fabs( track_dca3d_xy[ itrk_j2 ] );
						track_dca2d_error[ itrk_j2 ] = fabs( track_dca3d_xy_error[ itrk_j2 ] );
					}
					if(dca_method == 6) {
						float sigmalized_dca3d =
							pow(track_dca3d_xy[ itrk_j2 ]/track_dca3d_xy_error[ itrk_j2 ],2) +
							pow(track_dca3d_z[ itrk_j2 ]/track_dca3d_z_error[ itrk_j2 ],2)
							;
						float effective_sigma =
							pow(track_dca3d_xy_error[ itrk_j2 ],2) +
							pow(track_dca3d_z_error[ itrk_j2 ],2)
							;
					}


					if (! (track_dca2d[ itrk_j2 ] < max_dca_cut)) continue; // yuhw

					// set sign WRT jet
					if(
							dca_method  == 0 ||
							dca_method  == 1 ||
							dca_method  == 2 ||
							dca_method  == 5
						) // dca2d methods
					{
						float dphi = track_dca2d_phi[ itrk_j2 ] - truthjet_phi[ j2 ];

						//if (iflavor == 2 && track_dca2d[ itrk ] > 0.05)
						//	std::cout << truthjet_phi[ j ] << " / " <<  track_dca2d_phi[ itrk ] << std::endl;

						if (dphi > +3.14159) dphi -= 2*3.14159;
						if (dphi < -3.14159) dphi += 2*3.14159;
						dphi = fabs(dphi);
						if (dphi > 3.14159/2)
							track_dca2d[ itrk_j2 ] = -1 * track_dca2d[ itrk_j2 ];
					} else {
						TVector3 dca_vec(
								track_dca2d_x[ itrk_j2 ],
								track_dca2d_y[ itrk_j2 ],
								track_dca2d_z[ itrk_j2 ]
								);
						TVector3 jet_vec(0,0,0);
						jet_vec.SetPtEtaPhi(
								truthjet_pt[j2],
								truthjet_eta[j2],
								truthjet_phi[j2]
								);
						if( dca_vec.Dot(jet_vec) < 0 ) {
							track_dca2d[ itrk_j2 ] = -1 * track_dca2d[ itrk_j2 ];
						}
					}


					if ( track_dca2d[ itrk_j2 ] > highest_dca_j2 ) {
						third_highest_dca_j2 = second_highest_dca_j2;
						second_highest_dca_j2 = highest_dca_j2;
						highest_dca_j2 = track_dca2d[ itrk_j2 ];
					} else if ( track_dca2d[ itrk_j2 ] > second_highest_dca_j2 ) {

						third_highest_dca_j2 = second_highest_dca_j2;
						second_highest_dca_j2 = track_dca2d[ itrk_j2 ];
					} else if ( track_dca2d[ itrk_j2 ] > third_highest_dca_j2 ) {
						third_highest_dca_j2 = track_dca2d[ itrk_j2 ];
					}

					if ( track_dca2d[ itrk_j2 ] / track_dca2d_error[ itrk_j2 ] > highest_S_j2 ) {
						third_highest_S_j2 = second_highest_S_j2;
						second_highest_S_j2 = highest_S_j2;
						highest_S_j2 = track_dca2d[ itrk_j2 ] / track_dca2d_error[ itrk_j2 ];
					} else if ( track_dca2d[ itrk_j2 ]/ track_dca2d_error[ itrk_j2 ] > second_highest_S_j2 ) {
						third_highest_S_j2 = second_highest_S_j2;
						second_highest_S_j2 = track_dca2d[ itrk_j2 ] / track_dca2d_error[ itrk_j2 ];
					} else if ( track_dca2d[ itrk_j2 ]/ track_dca2d_error[ itrk_j2 ] > third_highest_S_j2 ) {
						third_highest_S_j2 = track_dca2d[ itrk_j2 ] / track_dca2d_error[ itrk_j2 ];
					}

				} // loop tracks for subleading jet

				h2_dijet_highest_dca_FINE[di_jet_type]->Fill(highest_dca, highest_dca_j2);
				h2_dijet_second_highest_dca_FINE[di_jet_type]->Fill(second_highest_dca, second_highest_S_j2);
				h2_dijet_third_highest_dca_FINE[di_jet_type]->Fill(third_highest_dca, third_highest_dca_j2);

				h2_dijet_highest_S_FINE[di_jet_type]->Fill(highest_S, highest_S_j2);
				h2_dijet_second_highest_S_FINE[di_jet_type]->Fill(second_highest_S, second_highest_S_j2);
				h2_dijet_third_highest_S_FINE[di_jet_type]->Fill(third_highest_S, third_highest_S_j2);
			} // subleading jet 

		} // loop jets

	} // loop events/entries


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
	h1_jet_highest_S_EFF[0]->GetYaxis()->SetRangeUser(1E-3,1.1);

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
	TGraph *tg_bjetE_vs_ljetE_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_highest_dca_EFF[2]->GetBinContent( n + 1 );
		if(eff < _plot_min_bjet_eff_cut) continue;
		float rej = 1 /  h1_jet_highest_dca_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_highest_dca->SetPoint( tg_bjetE_vs_ljetR_highest_dca->GetN(), eff, rej );
		tg_bjetE_vs_ljetE_highest_dca->SetPoint( tg_bjetE_vs_ljetE_highest_dca->GetN(), eff, 1./rej );
	}

	TGraph *tg_bjetE_vs_ljetR_second_highest_dca = new TGraph();
	TGraph *tg_bjetE_vs_ljetE_second_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_second_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_second_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_second_highest_dca_EFF[2]->GetBinContent( n + 1 );
		if(eff < _plot_min_bjet_eff_cut) continue;
		float rej = 1 /  h1_jet_second_highest_dca_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_second_highest_dca->SetPoint( tg_bjetE_vs_ljetR_second_highest_dca->GetN(), eff, rej );
		tg_bjetE_vs_ljetE_second_highest_dca->SetPoint( tg_bjetE_vs_ljetE_second_highest_dca->GetN(), eff, 1./rej );
	}

	TGraph *tg_bjetE_vs_ljetR_third_highest_dca = new TGraph();
	TGraph *tg_bjetE_vs_ljetE_third_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_third_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_third_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_third_highest_dca_EFF[2]->GetBinContent( n + 1 );
		if(eff < _plot_min_bjet_eff_cut) continue;
		float rej = 1 /  h1_jet_third_highest_dca_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_third_highest_dca->SetPoint( tg_bjetE_vs_ljetR_third_highest_dca->GetN(), eff, rej );
		tg_bjetE_vs_ljetE_third_highest_dca->SetPoint( tg_bjetE_vs_ljetE_third_highest_dca->GetN(), eff, 1./rej );
	}

	TGraph *tg_bjetE_vs_ljetR_highest_S = new TGraph();
	TGraph *tg_bjetE_vs_ljetE_highest_S = new TGraph();
	TGraph *tg_bjetE_vs_cjetE_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_highest_S_EFF[2]->GetBinContent( n + 1 );
		if(eff < _plot_min_bjet_eff_cut) continue;
		float eff_c = h1_jet_highest_S_EFF[1]->GetBinContent( n + 1 );
		float rej = 1 /  h1_jet_highest_S_EFF[0]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_highest_S->SetPoint( tg_bjetE_vs_ljetR_highest_S->GetN(), eff, rej );
		tg_bjetE_vs_ljetE_highest_S->SetPoint( tg_bjetE_vs_ljetE_highest_S->GetN(), eff, 1./rej );
		//if(tg_bjetE_vs_ljetR_highest_S->GetN() == 0) continue;
		tg_bjetE_vs_cjetE_highest_S->SetPoint( tg_bjetE_vs_ljetE_highest_S->GetN(), eff, eff_c );
	}

	TGraph *tg_bjetE_vs_ljetR_second_highest_S = new TGraph();
	TGraph *tg_bjetE_vs_ljetE_second_highest_S = new TGraph();
	TGraph *tg_bjetE_vs_cjetE_second_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_second_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_second_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_second_highest_S_EFF[2]->GetBinContent( n + 1 );
		if(eff < _plot_min_bjet_eff_cut) continue;
		float rej = 1 /  h1_jet_second_highest_S_EFF[0]->GetBinContent( n + 1 );
		float eff_c = h1_jet_second_highest_S_EFF[1]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_second_highest_S->SetPoint( tg_bjetE_vs_ljetR_second_highest_S->GetN(), eff, rej );
		tg_bjetE_vs_ljetE_second_highest_S->SetPoint( tg_bjetE_vs_ljetE_second_highest_S->GetN(), eff, 1./rej );
		//if(tg_bjetE_vs_ljetE_second_highest_S->GetN()==0) continue;
		tg_bjetE_vs_cjetE_second_highest_S->SetPoint( tg_bjetE_vs_ljetE_second_highest_S->GetN(), eff, eff_c );
	}

	TGraph *tg_bjetE_vs_ljetR_third_highest_S = new TGraph();
	TGraph *tg_bjetE_vs_ljetE_third_highest_S = new TGraph();
	TGraph *tg_bjetE_vs_cjetE_third_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_third_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_third_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_third_highest_S_EFF[2]->GetBinContent( n + 1 );
		if(eff < _plot_min_bjet_eff_cut) continue;
		float rej = 1 /  h1_jet_third_highest_S_EFF[0]->GetBinContent( n + 1 );
		float eff_c = h1_jet_third_highest_S_EFF[1]->GetBinContent( n + 1 );
		tg_bjetE_vs_ljetR_third_highest_S->SetPoint( tg_bjetE_vs_ljetR_third_highest_S->GetN(), eff, rej );
		tg_bjetE_vs_ljetE_third_highest_S->SetPoint( tg_bjetE_vs_ljetE_third_highest_S->GetN(), eff, 1./rej );
		//if(tg_bjetE_vs_ljetE_third_highest_S->GetN()==0) continue;
		tg_bjetE_vs_cjetE_third_highest_S->SetPoint( tg_bjetE_vs_ljetE_third_highest_S->GetN(), eff, eff_c );
	}

	// -------- di-bjet

	gPad->SetLogy(0);
	for(int i=DiBJet; i<=OtherDiJet; ++i) {
		h2_dijet_highest_dca_FINE[i]->Draw("colz");
		tc->Print(Form("plot/h2_dijet_highest_dca_FINE_%d.pdf",i));
		tc->Print(Form("plot/h2_dijet_highest_dca_FINE_%d.root",i));

		h2_dijet_second_highest_dca_FINE[i]->Draw("colz");
		tc->Print(Form("plot/h2_dijet_second_highest_dca_FINE_%d.pdf",i));
		tc->Print(Form("plot/h2_dijet_second_highest_dca_FINE_%d.root",i));

		h2_dijet_third_highest_dca_FINE[i]->Draw("colz");
		tc->Print(Form("plot/h2_dijet_third_highest_dca_FINE_%d.pdf",i));
		tc->Print(Form("plot/h2_dijet_third_highest_dca_FINE_%d.root",i));

		h2_dijet_highest_S_FINE[i]->Draw("colz");
		tc->Print(Form("plot/h2_dijet_highest_S_FINE_%d.pdf",i));
		tc->Print(Form("plot/h2_dijet_highest_S_FINE_%d.root",i));

		h2_dijet_second_highest_S_FINE[i]->Draw("colz");
		tc->Print(Form("plot/h2_dijet_second_highest_S_FINE_%d.pdf",i));
		tc->Print(Form("plot/h2_dijet_second_highest_S_FINE_%d.root",i));

		h2_dijet_third_highest_S_FINE[i]->Draw("colz");
		tc->Print(Form("plot/h2_dijet_third_highest_S_FINE_%d.pdf",i));
		tc->Print(Form("plot/h2_dijet_third_highest_S_FINE_%d.root",i));

	}
	gPad->SetLogy(1);

	// -------- di-bjet

	TH1D *hFrame_dibjetE_vs_dibjetP = new TH1D("hFrame_dibjetE_vs_dibjetP",";di-#it{b}-jet efficiency;di-#it{b}-jet purity",1,0,1.05);
	hFrame_dibjetE_vs_dibjetP->GetYaxis()->SetRangeUser(0, 1.);

	int line_width = 4;

	hFrame_dibjetE_vs_dibjetP->Draw();

	gPad->SetGrid(0,0);
	gPad->SetLogy(0);

	TGraph *tg_dibjetE_vs_dibjetP_highest_dca = new TGraph();
	for (int n = 1; n <= h2_dijet_highest_dca_FINE[OtherDiJet]->GetNbinsX(); ++n) {
		if ( h2_dijet_highest_dca_FINE[OtherDiJet]->GetBinContent(n) == 0 ) continue;

		float eff = h2_dijet_highest_dca_FINE[DiBJet]->Integral(n,-1,n,-1) /
				h2_dijet_highest_dca_FINE[DiBJet]->Integral(-1,-1,-1,-1);

		if(eff < _plot_min_bjet_eff_cut) continue;

		float pur = h2_dijet_highest_dca_FINE[DiBJet]->Integral(n,-1,n,-1) /
			(h2_dijet_highest_dca_FINE[DiBJet]->Integral(n,-1,n,-1)+
					h2_dijet_highest_dca_FINE[OtherDiJet]->Integral(n,-1,n,-1));
		tg_dibjetE_vs_dibjetP_highest_dca->SetPoint( tg_dibjetE_vs_dibjetP_highest_dca->GetN(), eff, pur );
	}

	tg_dibjetE_vs_dibjetP_highest_dca->SetLineWidth(line_width);
	tg_dibjetE_vs_dibjetP_highest_dca->SetLineColor(kGreen+2);
	tg_dibjetE_vs_dibjetP_highest_dca->SetLineStyle(2);
	tg_dibjetE_vs_dibjetP_highest_dca->SetName("tg_dibjetE_vs_dibjetP_highest_dca");
	//tg_dibjetE_vs_dibjetP_highest_dca->Draw("L,same");

	TGraph *tg_dibjetE_vs_dibjetP_second_highest_dca = new TGraph();
	for (int n = 1; n <= h2_dijet_second_highest_dca_FINE[OtherDiJet]->GetNbinsX(); ++n) {
		if ( h2_dijet_second_highest_dca_FINE[OtherDiJet]->GetBinContent(n) == 0 ) continue;

		float eff = h2_dijet_second_highest_dca_FINE[DiBJet]->Integral(n,-1,n,-1) /
				h2_dijet_second_highest_dca_FINE[DiBJet]->Integral(-1,-1,-1,-1);

		if(eff < _plot_min_bjet_eff_cut) continue;

		float pur = h2_dijet_second_highest_dca_FINE[DiBJet]->Integral(n,-1,n,-1) /
			(h2_dijet_second_highest_dca_FINE[DiBJet]->Integral(n,-1,n,-1)+
					h2_dijet_second_highest_dca_FINE[OtherDiJet]->Integral(n,-1,n,-1));
		tg_dibjetE_vs_dibjetP_second_highest_dca->SetPoint( tg_dibjetE_vs_dibjetP_second_highest_dca->GetN(), eff, pur );
	}

	tg_dibjetE_vs_dibjetP_second_highest_dca->SetLineWidth(line_width);
	tg_dibjetE_vs_dibjetP_second_highest_dca->SetLineColor(kRed);
	tg_dibjetE_vs_dibjetP_second_highest_dca->SetLineStyle(2);
	tg_dibjetE_vs_dibjetP_second_highest_dca->SetName("tg_dibjetE_vs_dibjetP_second_highest_dca");
	//tg_dibjetE_vs_dibjetP_second_highest_dca->Draw("L,same");

	TGraph *tg_dibjetE_vs_dibjetP_third_highest_dca = new TGraph();
	for (int n = 1; n <= h2_dijet_third_highest_dca_FINE[OtherDiJet]->GetNbinsX(); ++n) {
		if ( h2_dijet_third_highest_dca_FINE[OtherDiJet]->GetBinContent(n) == 0 ) continue;

		float eff = h2_dijet_third_highest_dca_FINE[DiBJet]->Integral(n,-1,n,-1) /
				h2_dijet_third_highest_dca_FINE[DiBJet]->Integral(-1,-1,-1,-1);

		if(eff < _plot_min_bjet_eff_cut) continue;

		float pur = h2_dijet_third_highest_dca_FINE[DiBJet]->Integral(n,-1,n,-1) /
			(h2_dijet_third_highest_dca_FINE[DiBJet]->Integral(n,-1,n,-1)+
					h2_dijet_third_highest_dca_FINE[OtherDiJet]->Integral(n,-1,n,-1));
		tg_dibjetE_vs_dibjetP_third_highest_dca->SetPoint( tg_dibjetE_vs_dibjetP_third_highest_dca->GetN(), eff, pur );
	}

	tg_dibjetE_vs_dibjetP_third_highest_dca->SetLineWidth(line_width);
	tg_dibjetE_vs_dibjetP_third_highest_dca->SetLineColor(kBlue);
	tg_dibjetE_vs_dibjetP_third_highest_dca->SetLineStyle(2);
	tg_dibjetE_vs_dibjetP_third_highest_dca->SetName("tg_dibjetE_vs_dibjetP_third_highest_dca");
	//tg_dibjetE_vs_dibjetP_third_highest_dca->Draw("L,same");


	TGraph *tg_dibjetE_vs_dibjetP_highest_S = new TGraph();
	for (int n = 1; n <= h2_dijet_highest_S_FINE[OtherDiJet]->GetNbinsX(); ++n) {
		if ( h2_dijet_highest_S_FINE[OtherDiJet]->GetBinContent(n) == 0 ) continue;

		float eff = h2_dijet_highest_S_FINE[DiBJet]->Integral(n,-1,n,-1) /
				h2_dijet_highest_S_FINE[DiBJet]->Integral(-1,-1,-1,-1);

		if(eff < _plot_min_bjet_eff_cut) continue;

		float pur = h2_dijet_highest_S_FINE[DiBJet]->Integral(n,-1,n,-1) /
			(h2_dijet_highest_S_FINE[DiBJet]->Integral(n,-1,n,-1)+
					h2_dijet_highest_S_FINE[OtherDiJet]->Integral(n,-1,n,-1));
		tg_dibjetE_vs_dibjetP_highest_S->SetPoint( tg_dibjetE_vs_dibjetP_highest_S->GetN(), eff, pur );
	}

	tg_dibjetE_vs_dibjetP_highest_S->SetLineWidth(line_width);
	tg_dibjetE_vs_dibjetP_highest_S->SetLineColor(kGreen+2);
	tg_dibjetE_vs_dibjetP_highest_S->SetLineStyle(1);
	tg_dibjetE_vs_dibjetP_highest_S->SetName("tg_dibjetE_vs_dibjetP_highest_S");
	tg_dibjetE_vs_dibjetP_highest_S->Draw("L,same");

	TGraph *tg_dibjetE_vs_dibjetP_second_highest_S = new TGraph();
	for (int n = 1; n <= h2_dijet_second_highest_S_FINE[OtherDiJet]->GetNbinsX(); ++n) {
		if ( h2_dijet_second_highest_S_FINE[OtherDiJet]->GetBinContent(n) == 0 ) continue;

		float eff = h2_dijet_second_highest_S_FINE[DiBJet]->Integral(n,-1,n,-1) /
				h2_dijet_second_highest_S_FINE[DiBJet]->Integral(-1,-1,-1,-1);

		if(eff < _plot_min_bjet_eff_cut) continue;

		float pur = h2_dijet_second_highest_S_FINE[DiBJet]->Integral(n,-1,n,-1) /
			(h2_dijet_second_highest_S_FINE[DiBJet]->Integral(n,-1,n,-1)+
					h2_dijet_second_highest_S_FINE[OtherDiJet]->Integral(n,-1,n,-1));
		tg_dibjetE_vs_dibjetP_second_highest_S->SetPoint( tg_dibjetE_vs_dibjetP_second_highest_S->GetN(), eff, pur );
	}

	tg_dibjetE_vs_dibjetP_second_highest_S->SetLineWidth(line_width);
	tg_dibjetE_vs_dibjetP_second_highest_S->SetLineColor(kRed);
	tg_dibjetE_vs_dibjetP_second_highest_S->SetLineStyle(1);
	tg_dibjetE_vs_dibjetP_second_highest_S->SetName("tg_dibjetE_vs_dibjetP_second_highest_S");
	tg_dibjetE_vs_dibjetP_second_highest_S->Draw("L,same");

	TGraph *tg_dibjetE_vs_dibjetP_third_highest_S = new TGraph();
	for (int n = 1; n <= h2_dijet_third_highest_S_FINE[OtherDiJet]->GetNbinsX(); ++n) {
		if ( h2_dijet_third_highest_S_FINE[OtherDiJet]->GetBinContent(n) == 0 ) continue;

		float eff = h2_dijet_third_highest_S_FINE[DiBJet]->Integral(n,-1,n,-1) /
				h2_dijet_third_highest_S_FINE[DiBJet]->Integral(-1,-1,-1,-1);

		if(eff < _plot_min_bjet_eff_cut) continue;

		float pur = h2_dijet_third_highest_S_FINE[DiBJet]->Integral(n,-1,n,-1) /
			(h2_dijet_third_highest_S_FINE[DiBJet]->Integral(n,-1,n,-1)+
					h2_dijet_third_highest_S_FINE[OtherDiJet]->Integral(n,-1,n,-1));
		tg_dibjetE_vs_dibjetP_third_highest_S->SetPoint( tg_dibjetE_vs_dibjetP_third_highest_S->GetN(), eff, pur );
	}

	tg_dibjetE_vs_dibjetP_third_highest_S->SetLineWidth(line_width);
	tg_dibjetE_vs_dibjetP_third_highest_S->SetLineColor(kBlue);
	tg_dibjetE_vs_dibjetP_third_highest_S->SetLineStyle(1);
	tg_dibjetE_vs_dibjetP_third_highest_S->SetName("tg_dibjetE_vs_dibjetP_third_highest_S");
	tg_dibjetE_vs_dibjetP_third_highest_S->Draw("L,same");

	add_purity_text(tracking_option_name);

	tc->Print("plot/tg_dibjetE_vs_dibjetP_algs.pdf");
	tc->Print("plot/tg_dibjetE_vs_dibjetP_algs.root");
	gPad->SetLogy(1);
	// -------- end di-bjet


	// -------- Purity
	TGraph *tg_bjetE_vs_bjetP_highest_dca = new TGraph();
	for (int n = 0; n < h1_jet_highest_dca_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_highest_dca_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_highest_dca_EFF[2]->GetBinContent( n + 1 );
		if(eff < _plot_min_bjet_eff_cut) continue;
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
		if(eff < _plot_min_bjet_eff_cut) continue;
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
		if(eff < _plot_min_bjet_eff_cut) continue;
		float pur = h1_jet_third_highest_dca_FINE[2]->Integral( n + 1, -1 ) /
			(h1_jet_third_highest_dca_FINE[0]->Integral( n + 1, -1 )+
			 h1_jet_third_highest_dca_FINE[1]->Integral( n + 1, -1 )+
			 h1_jet_third_highest_dca_FINE[2]->Integral( n + 1, -1 ));
		tg_bjetE_vs_bjetP_third_highest_dca->SetPoint( tg_bjetE_vs_bjetP_third_highest_dca->GetN(), eff, pur );
	}

	//const float plot_min_S_cut = 1.;
	const float plot_max_S_cut = 1000.;
	TGraph *tg_bjetE_vs_bjetP_highest_S = new TGraph();
	for (int n = 0; n < h1_jet_highest_S_EFF[0]->GetNbinsX(); n++) {
		if ( h1_jet_highest_S_EFF[0]->GetBinContent( n + 1 ) == 0 ) continue;
		float eff = h1_jet_highest_S_EFF[2]->GetBinContent( n + 1 );
		if(eff < _plot_min_bjet_eff_cut) continue;
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
		if(eff < _plot_min_bjet_eff_cut) continue;
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
		if(eff < _plot_min_bjet_eff_cut) continue;
		//if(h1_jet_third_highest_S_EFF[0]->GetBinCenter(n+1) < plot_min_S_cut) continue;
		if(h1_jet_third_highest_S_EFF[0]->GetBinCenter(n+1) > plot_max_S_cut) continue;
		float pur = h1_jet_third_highest_S_FINE[2]->Integral( n + 1, -1 ) /
			(h1_jet_third_highest_S_FINE[0]->Integral( n + 1, -1 )+
			 h1_jet_third_highest_S_FINE[1]->Integral( n + 1, -1 )+
			 h1_jet_third_highest_S_FINE[2]->Integral( n + 1, -1 ));
		tg_bjetE_vs_bjetP_third_highest_S->SetPoint( tg_bjetE_vs_bjetP_third_highest_S->GetN(), eff, pur );
	}

	TH1D *hFrame = new TH1D("hFrame",";#it{b}-jet efficiency;#it{l}-jet rejection (1/efficiency)",1,0,1.05);
	hFrame->GetYaxis()->SetRangeUser(0.5, 1e+6);

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


	//bjetE_vs_ljetE
	TH1D *hFrame_bjetE_vs_ljetE = new TH1D("hFrame_bjetE_vs_ljetE",";#it{b}-jet efficiency;#it{l}-jet efficiency",1,0,1.05);
	hFrame_bjetE_vs_ljetE->GetYaxis()->SetRangeUser(1E-4,1.05);

	hFrame_bjetE_vs_ljetE->Draw();

	//tg_bjetE_vs_ljetE_highest_dca->SetLineWidth(2);
	//tg_bjetE_vs_ljetE_second_highest_dca->SetLineWidth(2);
	//tg_bjetE_vs_ljetE_third_highest_dca->SetLineWidth(2);

	//tg_bjetE_vs_ljetE_highest_dca->SetLineColor(kBlack);
	//tg_bjetE_vs_ljetE_second_highest_dca->SetLineColor(kRed);
	//tg_bjetE_vs_ljetE_third_highest_dca->SetLineColor(kYellow+2);

	tg_bjetE_vs_ljetE_highest_S->SetLineWidth(2);
	tg_bjetE_vs_ljetE_second_highest_S->SetLineWidth(2);
	tg_bjetE_vs_ljetE_third_highest_S->SetLineWidth(2);

	tg_bjetE_vs_ljetE_highest_S->SetLineColor(kBlue);
	tg_bjetE_vs_ljetE_second_highest_S->SetLineColor(kGreen+2);
	tg_bjetE_vs_ljetE_third_highest_S->SetLineColor(kMagenta+2);

	//tg_bjetE_vs_ljetE_highest_dca->Draw("L,same");
	//tg_bjetE_vs_ljetE_second_highest_dca->Draw("L,same");
	//tg_bjetE_vs_ljetE_third_highest_dca->Draw("L,same");
	tg_bjetE_vs_ljetE_highest_S->Draw("L,same");
	tg_bjetE_vs_ljetE_second_highest_S->Draw("L,same");
	tg_bjetE_vs_ljetE_third_highest_S->Draw("L,same");

	myText(0.65,0.62,kBlue,   		"1-track, #it{S}_{DCA}");
	myText(0.65,0.55,kGreen+2,		"2-track, #it{S}_{DCA}");
	myText(0.65,0.48,kMagenta+2,	"3-track, #it{S}_{DCA}");
	//myText(0.20, 0.3,kBlack,  		Form("Pythia8, #approx%2.0f GeV jets, %s tracking", simulation_jet_energy, tracking_option_name));

	tc->Print("plot/tg_bjetE_vs_ljetE_algs.pdf");
	tc->Print("plot/tg_bjetE_vs_ljetE_algs.root");


	//bjetE_vs_cjetE
	TH1D *hFrame_bjetE_vs_cjetE = new TH1D("hFrame_bjetE_vs_cjetE",";#it{b}-jet efficiency;#it{c}-jet efficiency",1,0,1.05);
	hFrame_bjetE_vs_cjetE->GetYaxis()->SetRangeUser(1E-3,1.05);

	hFrame_bjetE_vs_cjetE->Draw();

	//tg_bjetE_vs_cjetE_highest_dca->SetLineWidth(2);
	//tg_bjetE_vs_cjetE_second_highest_dca->SetLineWidth(2);
	//tg_bjetE_vs_cjetE_third_highest_dca->SetLineWidth(2);

	//tg_bjetE_vs_cjetE_highest_dca->SetLineColor(kBlack);
	//tg_bjetE_vs_cjetE_second_highest_dca->SetLineColor(kRed);
	//tg_bjetE_vs_cjetE_third_highest_dca->SetLineColor(kYellow+2);

	tg_bjetE_vs_cjetE_highest_S->SetLineWidth(2);
	tg_bjetE_vs_cjetE_second_highest_S->SetLineWidth(2);
	tg_bjetE_vs_cjetE_third_highest_S->SetLineWidth(2);

	tg_bjetE_vs_cjetE_highest_S->SetLineColor(kBlue);
	tg_bjetE_vs_cjetE_second_highest_S->SetLineColor(kGreen+2);
	tg_bjetE_vs_cjetE_third_highest_S->SetLineColor(kMagenta+2);

	//tg_bjetE_vs_cjetE_highest_dca->Draw("L,same");
	//tg_bjetE_vs_cjetE_second_highest_dca->Draw("L,same");
	//tg_bjetE_vs_cjetE_third_highest_dca->Draw("L,same");
	tg_bjetE_vs_cjetE_highest_S->RemovePoint(0);
	tg_bjetE_vs_cjetE_highest_S->Draw("L,same");
	tg_bjetE_vs_cjetE_second_highest_S->RemovePoint(0);
	tg_bjetE_vs_cjetE_second_highest_S->Draw("L,same");
	tg_bjetE_vs_cjetE_third_highest_S->RemovePoint(0);
	tg_bjetE_vs_cjetE_third_highest_S->Draw("L,same");

	myText(0.65,0.62,kBlue,   		"1-track, #it{S}_{DCA}");
	myText(0.65,0.55,kGreen+2,		"2-track, #it{S}_{DCA}");
	myText(0.65,0.48,kMagenta+2,	"3-track, #it{S}_{DCA}");
	//myText(0.20, 0.3,kBlack,  		Form("Pythia8, #approx%2.0f GeV jets, %s tracking", simulation_jet_energy, tracking_option_name));

	tc->Print("plot/tg_bjetE_vs_cjetE_algs.pdf");
	tc->Print("plot/tg_bjetE_vs_cjetE_algs.root");

	//------------
	TH1D *hFrame_bjetE_vs_bjetP = new TH1D("hFrame_bjetE_vs_bjetP",";#it{b}-jet efficiency;#it{b}-jet purity",1,0,1.05);
	hFrame_bjetE_vs_bjetP->GetYaxis()->SetRangeUser(0, 1.);

	line_width = 4;

	hFrame_bjetE_vs_bjetP->Draw();

	tg_bjetE_vs_bjetP_highest_dca->SetLineWidth(line_width);
	tg_bjetE_vs_bjetP_second_highest_dca->SetLineWidth(line_width);
	tg_bjetE_vs_bjetP_third_highest_dca->SetLineWidth(line_width);

	tg_bjetE_vs_bjetP_highest_S->SetLineWidth(line_width);
	tg_bjetE_vs_bjetP_second_highest_S->SetLineWidth(line_width);
	tg_bjetE_vs_bjetP_third_highest_S->SetLineWidth(line_width);

	tg_bjetE_vs_bjetP_highest_dca->SetLineColor(kGreen+2);
	tg_bjetE_vs_bjetP_second_highest_dca->SetLineColor(kRed);
	tg_bjetE_vs_bjetP_third_highest_dca->SetLineColor(kBlue);

	tg_bjetE_vs_bjetP_highest_dca->SetLineStyle(2);
	tg_bjetE_vs_bjetP_second_highest_dca->SetLineStyle(2);
	tg_bjetE_vs_bjetP_third_highest_dca->SetLineStyle(2);

	tg_bjetE_vs_bjetP_highest_S->SetLineColor(kGreen+2);
	tg_bjetE_vs_bjetP_second_highest_S->SetLineColor(kRed);
	tg_bjetE_vs_bjetP_third_highest_S->SetLineColor(kBlue);

	//DEBUG
	//tg_bjetE_vs_bjetP_highest_dca->SetName("tg_bjetE_vs_bjetP_highest_dca");
	//tg_bjetE_vs_bjetP_second_highest_dca->SetName("tg_bjetE_vs_bjetP_second_highest_dca");;
	//tg_bjetE_vs_bjetP_third_highest_dca->SetName("tg_bjetE_vs_bjetP_third_highest_dca");

	tg_bjetE_vs_bjetP_highest_S->SetName("tg_bjetE_vs_bjetP_highest_S");
	tg_bjetE_vs_bjetP_second_highest_S->SetName("tg_bjetE_vs_bjetP_second_highest_S");;
	tg_bjetE_vs_bjetP_third_highest_S->SetName("tg_bjetE_vs_bjetP_third_highest_S");

	//tg_bjetE_vs_bjetP_highest_dca->Draw("L,same");
	//tg_bjetE_vs_bjetP_second_highest_dca->Draw("L,same");
	//tg_bjetE_vs_bjetP_third_highest_dca->Draw("L,same");

	tg_bjetE_vs_bjetP_highest_S->Draw("L,same");
	tg_bjetE_vs_bjetP_second_highest_S->Draw("L,same");
	tg_bjetE_vs_bjetP_third_highest_S->Draw("L,same");

	//gROOT->LoadMacro("add_purity_text.C");
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
	}

	{

		TGraph *tg_ = new TGraph();
		for (int n = 0; n < h1_jet_highest_dca_EFF[0]->GetNbinsX(); n++) {
		}


		TGraphErrors *tg_tracking_eff_vs_pt[3];
		for(int i=0;i<3;i++) {
			tg_tracking_eff_vs_pt[i] = new TGraphErrors();
			for(int n = 1; n <= h1_track_best_pt_embed_0_10GeV_pion[i]->GetNbinsX(); n++) {
				float pt = h1_particle_pt_embed_0_10GeV_pion[i]->GetBinCenter(n);
				float num = h1_track_best_pt_embed_0_10GeV_pion[i]->GetBinContent(n);
				float den = h1_particle_pt_embed_0_10GeV_pion[i]->GetBinContent(n);
				if(num<=0 || den <=0) continue;
				float eff = num/den;
				tg_tracking_eff_vs_pt[i]->SetPoint(tg_tracking_eff_vs_pt[i]->GetN(), pt, eff);
				float eff_err = binorminal_error(num, den);
				tg_tracking_eff_vs_pt[i]->SetPointError(tg_tracking_eff_vs_pt[i]->GetN()-1, 0.25, eff_err);
			}
		}

		gPad->SetLogy(0);
		TH1D *hFrame_tracking_eff_vs_pt = new TH1D("hFrame_tracking_eff_vs_pt",";p_{T}^{Truth} [GeV/c];tracking eff.",60, 0, 30);
		hFrame_tracking_eff_vs_pt->GetYaxis()->SetRangeUser(0, 1.1);
		hFrame_tracking_eff_vs_pt->Draw();

		tg_tracking_eff_vs_pt[0]->SetMarkerStyle(20);
		tg_tracking_eff_vs_pt[0]->SetMarkerColor(kRed);

		tg_tracking_eff_vs_pt[0]->SetLineWidth(2);

		tg_tracking_eff_vs_pt[0]->SetLineColor(kRed);

		tg_tracking_eff_vs_pt[0]->Draw("p,same");
		tg_tracking_eff_vs_pt[0]->Print();

		myText(0.2, 0.45,kBlack,  		Form("#chi^{2}/n.d.f. < %2.1f", max_track_quality_cut));
		myText(0.2, 0.35,kBlack,  		Form("#MAPS >= %1d", min_MAPS_hits));
		myText(0.2, 0.25,kBlack,  		Form("|dca| < %1.1fcm", max_dca_cut));

		tc->Print("plot/tg_tracking_eff_vs_pt.pdf");
		tc->Print("plot/tg_tracking_eff_vs_pt.root");
		gPad->SetLogy(1);


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

		gPad->SetGrid(0,0);
		tc->Print("plot/h1_track_dca_scale.pdf");
		tc->Print("plot/h1_track_dca_scale.root");
		gPad->SetGrid(1,1);
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

	h1_jet_third_highest_S_FINE[1]->SetLineColor( kRed );
	h1_jet_third_highest_S_FINE[2]->SetLineColor( kBlue );

	h1_jet_third_highest_S_FINE[0]->GetXaxis()->SetRangeUser(-5, +14 );

	h1_jet_third_highest_S_FINE[0]->SetTitle(";third-largest #it{S}_{DCA};counts / 0.1");
	h1_jet_third_highest_S_FINE[0]->Draw();
	h1_jet_third_highest_S_FINE[1]->Draw("same");
	h1_jet_third_highest_S_FINE[2]->Draw("same");
	myText(0.6,0.90,kBlack,"light jets");
	myText(0.6,0.83,kRed,"charm jets");
	myText(0.6,0.76,kBlue,"bottom jets");
	tc->Print("plot/h1_jet_third_highest_S_zoom.pdf");

	fout->cd();
	T->Write();
}

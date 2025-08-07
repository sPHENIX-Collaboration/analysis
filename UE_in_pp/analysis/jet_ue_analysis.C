#include <iostream>
#include <TH2D.h>
#include <TH1D.h>
#include <TChain.h>
#include <TMath.h>
#include <TTree.h>
#include <TFile.h>
#include <sstream> //std::ostringstsream
#include <fstream> //std::ifstream
#include <iostream> //std::cout, std::endl
#include <cmath>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <string>
#include <set>
#include <TVector3.h>
#include <map>
#include <vector>
#include <TDatabasePDG.h>
#include <tuple>
#include <TProfile.h>
#include <TProfile2D.h>
#include "TH1D.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																			//
//	Note: 4.18.25 																															//
// 	This macro creates jet QA plots, energy density QA plots with options for using towers, topoclusters or emcal clusters,				    //
//	and plots of mean energy density vs jet variables (leading jet pT and dijet xj)															//
//																																			//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float get_deta(float eta1, float eta2) {
  return eta1 - eta2;
}

float get_dphi(float phi1, float phi2) {
  float dphi1 = phi1 - phi2;
  float dphi2 = phi1 - phi2 + 2*TMath::Pi();
  float dphi3 = phi1 - phi2 - 2*TMath::Pi();
  if (fabs(dphi1) > fabs(dphi2)) {
    dphi1 = dphi2;
  }
  if (fabs(dphi1) > fabs(dphi3)) {
    dphi1 = dphi3;
  }
  return dphi1;
}

float get_dR(float eta1, float phi1, float eta2, float phi2) {
  float deta = get_deta(eta1, eta2);
  float dphi = get_dphi(phi1, phi2);
  return sqrt(deta*deta + dphi*dphi);
}

void match_meas_truth(std::vector<float>* meas_eta, std::vector<float>* meas_phi, std::vector<int>& meas_matched, std::vector<float>* truth_eta, std::vector<float>* truth_phi, std::vector<int>& truth_matched, float jet_radius) {
  meas_matched.assign(meas_eta->size(), -1);
  truth_matched.assign(truth_eta->size(), -1);
  float max_match_dR = jet_radius * 0.75;
  for (int im = 0; im < meas_eta->size(); ++im) {
    float min_dR = 100;
    int match_index = -9999;
    for (int it = 0; it < truth_eta->size(); ++it) {
      float dR = get_dR((*meas_eta)[im], (*meas_phi)[im], (*truth_eta)[it], (*truth_phi)[it]);
      if (dR < min_dR) {
        match_index = it;
        min_dR = dR;
      }
    }
    if (min_dR < max_match_dR) {
      if (truth_matched[match_index] == -1) {
        meas_matched[im] = match_index;
        truth_matched[match_index] = im;
      } else {
        float dR1 = get_dR((*meas_eta)[truth_matched[match_index]], (*meas_phi)[truth_matched[match_index]], (*truth_eta)[match_index], (*truth_phi)[match_index]);
        if (min_dR < dR1) {
          meas_matched[truth_matched[match_index]] = -1;
          meas_matched[im] = match_index;
          truth_matched[match_index] = im;
        }
      }
    }
  }
}

void jet_ue_analysis(string infilename = "sim_run21_jet10_output_*.root", string outfilename = "dijet_calo_analysis_run21_jet10_topocluster.root", bool sim = false, bool clusters = true, bool emcal_clusters = false, bool applyCorr = false, bool do_bkg_cut = true, bool dijet_bkg_cut = true, bool do_truth_match = true) {

	// create outfile 
	std::cout << "Creating " << outfilename << std::endl;
	TFile *out = new TFile(outfilename.c_str(),"RECREATE");
	
	// define constants 
	float deltaeta = 0.0916667;
	float deltaphi = 0.0981748;
	float secteta = 2.2;
	float sectphi = (2.0*M_PI)/3.0;
	float ptbins[] = {5,6,7,8,9,10,12,14,16,18,20,22,24,27,30,35,40,50,60,80};
    int nptbins = sizeof(ptbins) / sizeof(ptbins[0]) - 1;
    float etmin = -20;
    float etmax = 80;
    int netbins = int((10*(etmax - etmin)));
    float etbins[netbins+1];
    for (int i = 0; i <= netbins; i++) {
    	etbins[i] = etmin + i*(etmax - etmin) / netbins;
    }
    float jetetamax = 0.7;
    float lead_ptmin = 12;
    float sub_ptmin = 7;
    float deltaphimin = 3.0*M_PI/4.0;
 
	// create event and jet histograms 
	TH1F* h_vz = new TH1F("h_vz","",400, -100, 100);
  	TH1F* h_leadjet = new TH1F("h_leadjet","",nptbins, ptbins);
  	TH1F* h_subjet = new TH1F("h_subjet","",nptbins, ptbins);
  	TH1F* h_leadphi = new TH1F("h_leadphi","",50,-M_PI,M_PI);
  	TH1F* h_leadeta = new TH1F("h_leadeta","",50,-1.1,1.1);
  	TH1F* h_subphi = new TH1F("h_subphi","",50,-M_PI,M_PI);
  	TH1F* h_subeta = new TH1F("h_subeta","",50,-1.1,1.1);
  	TH1F* h_deltaphi = new TH1F("h_deltaphi","",125,-2*M_PI,2*M_PI);
  	TH1F* h_pass_deltaphi = new TH1F("h_pass_deltaphi","",125,-2*M_PI,2*M_PI);
  	TH1F* h_pass_xj = new TH1F("h_pass_xj","",20,0,1);
  	TH1F* h_pass_spectra = new TH1F("h_pass_spectra","",nptbins, ptbins);
  	TH2F* h_aj_ptavg = new TH2F("h_aj_ptavg","",100,0,100,100,0,1);

  	// create UE histograms 
  	TH2F* h_ue_2D_total = new TH2F("h_ue_2D_total","",24,-1.1,1.1,32,0,M_PI);
  	TH2F* h_ue_2D_towards = new TH2F("h_ue_2D_towards","",24,-1.1,1.1,32,0,M_PI);
  	TH2F* h_ue_2D_transverse = new TH2F("h_ue_2D_transverse","",24,-1.1,1.1,32,0,M_PI);
  	TH2F* h_ue_2D_away = new TH2F("h_ue_2D_away","",24,-1.1,1.1,32,0,M_PI);

  	TH2F* h_trans_et_2D_index_total = new TH2F("h_trans_et_2D_index_total","",24,0,24,64,0,64);
  	TH2F* h_trans_et_2D_index_emcal = new TH2F("h_trans_et_2D_index_emcal","",96,0,96,256,0,256);
  	TH2F* h_trans_et_2D_index_ihcal = new TH2F("h_trans_et_2D_index_ihcal","",24,0,24,64,0,64);
  	TH2F* h_trans_et_2D_index_ohcal = new TH2F("h_trans_et_2D_index_ohcal","",24,0,24,64,0,64);

  	TH1F* h_ue_towards = new TH1F("h_ue_towards","",netbins, etbins);
  	TH1F* h_ue_transverse = new TH1F("h_ue_transverse","",netbins, etbins);
  	TH1F* h_ue_away = new TH1F("h_ue_away","",netbins, etbins);

  	TH1F* h_et_towards = new TH1F("h_et_towards","",netbins, etbins);
  	TH1F* h_et_transverse = new TH1F("h_et_transverse","",netbins, etbins);
  	TH1F* h_et_away = new TH1F("h_et_away","",netbins, etbins);

  	// create topocluster histograms 
  	int topo_thresholds[] = {-9999,0,100,200,300,500,1000,2000};

  	TH1F* h_ntopo_towards[8];
  	TH1F* h_ntopo_transverse[8];
  	TH1F* h_ntopo_away[8];

  	TH1F* h_topo_towards[8];
  	TH1F* h_topo_transverse[8];
  	TH1F* h_topo_away[8];

  	TH2F* h_2D_topo_towards[8];
  	TH2F* h_2D_topo_transverse[8];
  	TH2F* h_2D_topo_away[8];

  	TH1F* h_sume_topo_towards[8];
  	TH1F* h_sume_topo_transverse[8];
  	TH1F* h_sume_topo_away[8];

  	for (int i = 0; i < 8; i++) {
  		h_ntopo_towards[i] = new TH1F(Form("h_ntopo%d_towards",topo_thresholds[i]),"",200,0,200);
  		h_ntopo_transverse[i] = new TH1F(Form("h_ntopo%d_transverse",topo_thresholds[i]),"",200,0,200);
  		h_ntopo_away[i] = new TH1F(Form("h_ntopo%d_away",topo_thresholds[i]),"",200,0,200);

  		h_topo_towards[i] = new TH1F(Form("h_topo%d_towards",topo_thresholds[i]),"",netbins, etbins);
  		h_topo_transverse[i] = new TH1F(Form("h_topo%d_transverse",topo_thresholds[i]),"",netbins, etbins);
  		h_topo_away[i] = new TH1F(Form("h_topo%d_away",topo_thresholds[i]),"",netbins, etbins);

  		h_2D_topo_towards[i] = new TH2F(Form("h_2D_topo%d_towards",topo_thresholds[i]),"",24,-1.1,1.1,32,0,M_PI);
  		h_2D_topo_transverse[i] = new TH2F(Form("h_2D_topo%d_transverse",topo_thresholds[i]),"",24,-1.1,1.1,32,0,M_PI);
  		h_2D_topo_away[i] = new TH2F(Form("h_2D_topo%d_away",topo_thresholds[i]),"",24,-1.1,1.1,32,0,M_PI);

  		h_sume_topo_towards[i] = new TH1F(Form("h_sume_topo%d_towards",topo_thresholds[i]),"",netbins, etbins);
  		h_sume_topo_transverse[i] = new TH1F(Form("h_sume_topo%d_transverse",topo_thresholds[i]),"",netbins, etbins);
  		h_sume_topo_away[i] = new TH1F(Form("h_sume_topo%d_away",topo_thresholds[i]),"",netbins, etbins);
  	}

  	// create histograms for UE versus jet variables 
  	TProfile* h_ue_xj_towards = new TProfile("h_ue_xj_towards","",20,0,1);
  	TProfile* h_ue_xj_transverse = new TProfile("h_ue_xj_transverse","",20,0,1);
  	TProfile* h_ue_xj_away = new TProfile("h_ue_xj_away","",20,0,1);

  	TProfile* h_ue_pt_towards = new TProfile("h_ue_pt_towards","",nptbins, ptbins);
  	TProfile* h_ue_pt_transverse = new TProfile("h_ue_pt_transverse","",nptbins, ptbins);
  	TProfile* h_ue_pt_away = new TProfile("h_ue_pt_away","",nptbins, ptbins);

  	// input datasets from ttrees
 	TChain chain("T");
 	string inputDirectory = "/sphenix/tg/tg01/jets/egm2153/UEinppOutput/";
	string wildcardPath = inputDirectory + infilename;
	chain.Add(wildcardPath.c_str());

	chain.SetBranchStatus("*",0);

	// define ttree branch variables 
	int m_event;
	int nJet;
	int nTruthJet;
	float zvtx;

	vector<int> *triggerVector = nullptr;
	vector<float> *eta = nullptr;
	vector<float> *phi = nullptr;
	vector<float> *e = nullptr;
	vector<float> *pt = nullptr;
	vector<float> *jetemcale = nullptr;
	vector<float> *jetihcale = nullptr;
	vector<float> *jetohcale = nullptr;

	vector<float> *truthEta = nullptr;
	vector<float> *truthPhi = nullptr;
	vector<float> *truthE = nullptr;
	vector<float> *truthPt = nullptr;

	int emcaln = 0;
	float emcale[24576] = {0.0};
	float emcaleta[24576] = {0.0};
	float emcalphi[24576] = {0.0};
	int emcalieta[24576] = {0};
	int emcaliphi[24576] = {0};

	int ihcaln = 0;
	float ihcale[24576] = {0.0};
	float ihcaleta[24576] = {0.0};
	float ihcalphi[24576] = {0.0};
	int ihcalieta[24576] = {0};
	int ihcaliphi[24576] = {0};

	int ohcaln = 0;
	float ohcale[24576] = {0.0};
	float ohcaleta[24576] = {0.0};
	float ohcalphi[24576] = {0.0};
	int ohcalieta[24576] = {0};
	int ohcaliphi[24576] = {0};

	int clsmult = 0;
	float cluster_e[10000] = {0.0};
	float cluster_eta[10000] = {0.0};
	float cluster_phi[10000] = {0.0};

	int truthpar_n = 0;
	float truthpar_e[100000] = {0};
	float truthpar_eta[100000] = {0};
	float truthpar_phi[100000] = {0};
	int truthpar_pid[100000] = {0};

	chain.SetBranchStatus("m_event",1);
	chain.SetBranchStatus("nJet",1);
	chain.SetBranchStatus("zvtx",1);
	chain.SetBranchStatus("triggerVector",1);
	chain.SetBranchStatus("eta",1);
	chain.SetBranchStatus("phi",1);
	chain.SetBranchStatus("e",1);
	chain.SetBranchStatus("pt",1);
	chain.SetBranchStatus("jetEmcalE", 1);
	chain.SetBranchStatus("jetIhcalE", 1);
	chain.SetBranchStatus("jetOhcalE", 1);

	chain.SetBranchAddress("m_event",&m_event);
	chain.SetBranchAddress("nJet",&nJet);
	chain.SetBranchAddress("zvtx",&zvtx);
	chain.SetBranchAddress("triggerVector",&triggerVector);
	chain.SetBranchAddress("eta",&eta);
	chain.SetBranchAddress("phi",&phi);
	chain.SetBranchAddress("e",&e);
	chain.SetBranchAddress("pt",&pt);
	chain.SetBranchAddress("jetEmcalE", &jetemcale);
	chain.SetBranchAddress("jetIhcalE", &jetihcale);
	chain.SetBranchAddress("jetOhcalE", &jetohcale);

	if (!clusters) {
		chain.SetBranchStatus("emcaln", 1);
		chain.SetBranchStatus("emcale",1);
		chain.SetBranchStatus("emcaleta", 1);
		chain.SetBranchStatus("emcalphi", 1);
		chain.SetBranchStatus("emcalieta", 1);
		chain.SetBranchStatus("emcaliphi", 1);

		chain.SetBranchStatus("ihcaln", 1);
		chain.SetBranchStatus("ihcale", 1);
		chain.SetBranchStatus("ihcaleta", 1);
		chain.SetBranchStatus("ihcalphi", 1);
		chain.SetBranchStatus("ihcalieta", 1);
		chain.SetBranchStatus("ihcaliphi", 1);

		chain.SetBranchStatus("ohcaln", 1);
		chain.SetBranchStatus("ohcale", 1);
		chain.SetBranchStatus("ohcaleta", 1);
		chain.SetBranchStatus("ohcalphi", 1);
		chain.SetBranchStatus("ohcalieta", 1);
		chain.SetBranchStatus("ohcaliphi", 1);

		chain.SetBranchAddress("emcaln",&emcaln);
		chain.SetBranchAddress("emcale",emcale);
		chain.SetBranchAddress("emcaleta",emcaleta);
		chain.SetBranchAddress("emcalphi",emcalphi);
		chain.SetBranchAddress("emcalieta",emcalieta);
		chain.SetBranchAddress("emcaliphi",emcaliphi);

		chain.SetBranchAddress("ihcaln",&ihcaln);
		chain.SetBranchAddress("ihcale",ihcale);
		chain.SetBranchAddress("ihcaleta",ihcaleta);
		chain.SetBranchAddress("ihcalphi",ihcalphi);
		chain.SetBranchAddress("ihcalieta",ihcalieta);
		chain.SetBranchAddress("ihcaliphi",ihcaliphi);

		chain.SetBranchAddress("ohcaln",&ohcaln);
		chain.SetBranchAddress("ohcale",ohcale);
		chain.SetBranchAddress("ohcaleta",ohcaleta);
		chain.SetBranchAddress("ohcalphi",ohcalphi);
		chain.SetBranchAddress("ohcalieta",ohcalieta);
		chain.SetBranchAddress("ohcaliphi",ohcaliphi);
	}

	if (!emcal_clusters) {
		chain.SetBranchStatus("clsmult", 1);
		chain.SetBranchStatus("cluster_e", 1);
		chain.SetBranchStatus("cluster_eta", 1);
		chain.SetBranchStatus("cluster_phi", 1);

		chain.SetBranchAddress("clsmult",&clsmult);
		chain.SetBranchAddress("cluster_e",cluster_e);
		chain.SetBranchAddress("cluster_eta",cluster_eta);
		chain.SetBranchAddress("cluster_phi",cluster_phi);
	} else {
		chain.SetBranchStatus("emcal_clsmult", 1);
		chain.SetBranchStatus("emcal_cluster_e", 1);
		chain.SetBranchStatus("emcal_cluster_eta", 1);
		chain.SetBranchStatus("emcal_cluster_phi", 1);

		chain.SetBranchAddress("emcal_clsmult",&clsmult);
		chain.SetBranchAddress("emcal_cluster_e",cluster_e);
		chain.SetBranchAddress("emcal_cluster_eta",cluster_eta);
		chain.SetBranchAddress("emcal_cluster_phi",cluster_phi);
	}

	if (sim) {
		chain.SetBranchStatus("nTruthJet",1);
		chain.SetBranchStatus("truthE",1);
		chain.SetBranchStatus("truthPt",1);
		chain.SetBranchStatus("truthEta",1);
		chain.SetBranchStatus("truthPhi",1);

		chain.SetBranchAddress("nTruthJet",&nTruthJet);
		chain.SetBranchAddress("truthE",&truthE);
		chain.SetBranchAddress("truthPt",&truthPt);
		chain.SetBranchAddress("truthEta",&truthEta);
		chain.SetBranchAddress("truthPhi",&truthPhi);
	}

	// if applyCorr is set, then apply correction factor from MC Jet Calibration 
	TFile *corrFile;
  	TF1 *correction = new TF1("jet energy correction","1",0,80);
  	if(applyCorr) {
      	corrFile = new TFile("JES_IsoCorr_NumInv.root","READ");
      	corrFile -> cd();
      	correction = (TF1*)corrFile -> Get("corrFit_Iso0");
    }

	int eventnumber = 0; // number of events in tree (used for showing iteration through ttree)
	int events = 0; // number of events that pass event cuts (used for event level normalization)
    
    Long64_t nEntries = chain.GetEntries();
    std::cout << nEntries << std::endl;

    // main event loop 
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
    //for (Long64_t entry = 0; entry < 20000; ++entry) {
        chain.GetEntry(entry);
    	if (eventnumber % 10000 == 0) cout << "event " << eventnumber << endl;
    	eventnumber++;

    	// require jet trigger in data
  		bool jettrig = false;
  		for (auto t : *triggerVector) {
  			if (t >= 16 && t <= 23) {
  				jettrig = true;
  				break;
  			}
  		}

  		// require no negative energy jets (known jet background)
  		bool negJet = false;
  		for (int i = 0; i < nJet; i++) {
  			if ((*e)[i] < 0) {
  				negJet = true;
  			}
  		}

  		// require at least 2 jets in event and z vertex < 30 cm 
  		if (!jettrig && !sim) { continue; }
  		if (isnan(zvtx)) { continue; }
  		if (zvtx < -30 || zvtx > 30) { continue; }
  		if (negJet) { continue; }
  		if (dijet_bkg_cut && nJet < 2) { continue; }		

  		// find the leading and subleading jets of the event
  		int ind_lead = 0;
  		int ind_sub = 0;
  		float temp_lead = 0;
  		float temp_sub = 0;
  		for (int i = 0; i < nJet; i++) {
  			if ((*pt)[i] > temp_lead) {
  				if (temp_lead != 0) {
  					temp_sub = temp_lead;
  					ind_sub = ind_lead;
  				}
  				temp_lead = (*pt)[i];
  				ind_lead = i;
  			} else if ((*pt)[i] > temp_sub) {
  				temp_sub = (*pt)[i];
  				ind_sub = i;
  			}
  		}

  		if (fabs((*eta)[ind_lead]) > jetetamax || (*pt)[ind_lead] < lead_ptmin) { continue; }
  		if (do_bkg_cut) {
	  		if (dijet_bkg_cut) {
	  			//Dijet cut - energy of subleading jet should be at least 30% of energy in leading jet and (delta-phi > 3pi/4)
	  			if (fabs((*eta)[ind_sub]) > jetetamax || (*pt)[ind_sub] < 0.3*(*pt)[ind_lead]) { continue; }
	  		} else {
	  			//Energy fraction cut - jet energy in EMCal should be 10-90% of total jet energy, OHCal should be 10-90% and IHCal should be 0-90%
	  			float jete = (*e)[ind_lead];
	  			if ((*jetemcale)[ind_lead]/jete < 0.1 || (*jetemcale)[ind_lead]/jete > 0.9 || (*jetihcale)[ind_lead]/jete > 0.9 || (*jetohcale)[ind_lead]/jete < 0.1 || (*jetohcale)[ind_lead]/jete > 0.9) { continue; }
	  		}
  		}	

  		// create leading and subleading jet vectors and apply JES calibration correction if applicable 
  		TVector3 lead, sub;
  		if (applyCorr) {
    		lead.SetPtEtaPhi((*pt)[ind_lead]*correction->Eval((*pt)[ind_lead]), (*eta)[ind_lead], (*phi)[ind_lead]);
    		if (dijet_bkg_cut) sub.SetPtEtaPhi((*pt)[ind_sub]*correction->Eval((*pt)[ind_sub]), (*eta)[ind_sub], (*phi)[ind_sub]);
    	} else {
    		lead.SetPtEtaPhi((*pt)[ind_lead], (*eta)[ind_lead], (*phi)[ind_lead]);
    		if (dijet_bkg_cut) sub.SetPtEtaPhi((*pt)[ind_sub], (*eta)[ind_sub], (*phi)[ind_sub]);
    	}

    	std::vector<int> reco_matched;
    	std::vector<int> truth_matched;
    	if (sim && do_truth_match) {
    		match_meas_truth(eta, phi, reco_matched, truthEta, truthPhi, truth_matched, 0.4);
    		if (reco_matched[ind_lead] == -1) { continue; }
    		if (dijet_bkg_cut && reco_matched[ind_sub] == -1) { continue; }
    	}

    	h_vz->Fill(zvtx);
  		if (dijet_bkg_cut) {
  			h_deltaphi->Fill(lead.DeltaPhi(sub));
  			// apply delta-phi cut for dijet background cut
  			if (fabs(lead.DeltaPhi(sub)) < deltaphimin) { continue; }
  		}

  			
		// fill jet QA histograms 
		h_pass_spectra->Fill(lead.Pt());
		h_leadjet->Fill(lead.Pt());
		h_leadphi->Fill(lead.Phi());
		h_leadeta->Fill(lead.Eta());
		if (dijet_bkg_cut) {
			h_pass_deltaphi->Fill(lead.DeltaPhi(sub));
			h_pass_xj->Fill(sub.Pt()/lead.Pt());
			h_pass_spectra->Fill(sub.Pt());
			h_subjet->Fill(sub.Pt());
			h_subphi->Fill(sub.Phi());
			h_subeta->Fill(sub.Eta());
			h_aj_ptavg->Fill((lead.Pt()+sub.Pt())/2.0, (lead.Pt()-sub.Pt())/(lead.Pt()+sub.Pt()));
		}

		// define energy variables in the towards, transverse and away regions 
		float et_towards = 0;
		float et_transverse = 0;
		float et_away = 0;
		int ntopo_towards[] = {0,0,0,0,0,0,0,0};
		int ntopo_transverse[] = {0,0,0,0,0,0,0,0};
		int ntopo_away[] = {0,0,0,0,0,0,0,0};
		float sume_topo_towards[] = {0,0,0,0,0,0,0,0};
		float sume_topo_transverse[] = {0,0,0,0,0,0,0,0};
		float sume_topo_away[] = {0,0,0,0,0,0,0,0};

		if (!clusters) { // using towers to find total energy in towards, transverse and away regions 
			for (int i = 0; i < emcaln; i++) {
				TVector3 em;
				em.SetPtEtaPhi(emcale[i]/cosh(emcaleta[i]),emcaleta[i],emcalphi[i]); // define tower vector
				float dphi = lead.DeltaPhi(em); // find the deltaphi between leading jet and tower
				h_ue_2D_total->Fill(emcaleta[i],dphi,emcale[i]/cosh(emcaleta[i]));
				if (fabs(dphi) < M_PI/3.0) { // towards region 
					et_towards += emcale[i]/cosh(emcaleta[i]);
					h_ue_2D_towards->Fill(emcaleta[i],dphi,emcale[i]/cosh(emcaleta[i]));
				} else if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { // transverse region 
					et_transverse += emcale[i]/cosh(emcaleta[i]);
					h_ue_2D_transverse->Fill(emcaleta[i],dphi,emcale[i]/cosh(emcaleta[i]));
					h_trans_et_2D_index_emcal->Fill(emcalieta[i],emcaliphi[i],emcale[i]/cosh(emcaleta[i]));
					h_trans_et_2D_index_total->Fill(emcalieta[i]/4,emcaliphi[i]/4,emcale[i]/cosh(emcaleta[i]));
				} else if (fabs(dphi) > (2.0*M_PI)/3.0) { // away region 
					et_away += emcale[i]/cosh(emcaleta[i]);
					h_ue_2D_away->Fill(emcaleta[i],dphi,emcale[i]/cosh(emcaleta[i]));
				}
			}

  			for (int i = 0; i < ihcaln; i++) {
  				TVector3 ih;
  				ih.SetPtEtaPhi(ihcale[i]/cosh(ihcaleta[i]),ihcaleta[i],ihcalphi[i]);
  				float dphi = lead.DeltaPhi(ih);
  				h_ue_2D_total->Fill(ihcaleta[i],dphi,ihcale[i]/cosh(ihcaleta[i]));
  				if (fabs(dphi) < M_PI/3.0) {
  					et_towards += ihcale[i]/cosh(ihcaleta[i]);
  					h_ue_2D_towards->Fill(ihcaleta[i],dphi,ihcale[i]/cosh(ihcaleta[i]));
  				} else if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) {
					et_transverse += ihcale[i]/cosh(ihcaleta[i]);
  					h_ue_2D_transverse->Fill(ihcaleta[i],dphi,ihcale[i]/cosh(ihcaleta[i]));
  					h_trans_et_2D_index_ihcal->Fill(ihcalieta[i],ihcaliphi[i],ihcale[i]/cosh(ihcaleta[i]));
  					h_trans_et_2D_index_total->Fill(ihcalieta[i],ihcaliphi[i],ihcale[i]/cosh(ihcaleta[i]));
  				} else if (fabs(dphi) > (2.0*M_PI)/3.0) {
  					et_away += ihcale[i]/cosh(ihcaleta[i]);
  					h_ue_2D_away->Fill(ihcaleta[i],dphi,ihcale[i]/cosh(ihcaleta[i]));
  				}
  			}

  			for (int i = 0; i < ohcaln; i++) {
  				TVector3 oh;
  				oh.SetPtEtaPhi(ohcale[i]/cosh(ohcaleta[i]),ohcaleta[i],ohcalphi[i]);
  				float dphi = lead.DeltaPhi(oh);
  				h_ue_2D_total->Fill(ohcaleta[i],dphi,ohcale[i]/cosh(ohcaleta[i]));
  				if (fabs(dphi) < M_PI/3.0) {
  					et_towards += ohcale[i]/cosh(ohcaleta[i]);
  					h_ue_2D_towards->Fill(ohcaleta[i],dphi,ohcale[i]/cosh(ohcaleta[i]));
  				} else if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) {
					et_transverse += ohcale[i]/cosh(ohcaleta[i]);
  					h_ue_2D_transverse->Fill(ohcaleta[i],dphi,ohcale[i]/cosh(ohcaleta[i]));
  					h_trans_et_2D_index_ohcal->Fill(ohcalieta[i],ohcaliphi[i],ohcale[i]/cosh(ohcaleta[i]));
  					h_trans_et_2D_index_total->Fill(ohcalieta[i],ohcaliphi[i],ohcale[i]/cosh(ohcaleta[i]));
  				} else if (fabs(dphi) > (2.0*M_PI)/3.0) {
  					et_away += ohcale[i]/cosh(ohcaleta[i]);
  					h_ue_2D_away->Fill(ohcaleta[i],dphi,ohcale[i]/cosh(ohcaleta[i]));
  				}
  			}
  		} else { // using clusters to find total energy in towards, transverse and away regions 
			for (int i = 0; i < clsmult; i++) {
				TVector3 cls;
				cls.SetPtEtaPhi(cluster_e[i]/cosh(cluster_eta[i]),cluster_eta[i],cluster_phi[i]); // define cluster vector 
				float dphi = lead.DeltaPhi(cls); // find the deltaphi between leading jet and cluster 
				h_ue_2D_total->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
				if (fabs(dphi) < M_PI/3.0) { // towards region 
					et_towards += cluster_e[i]/cosh(cluster_eta[i]);
					for (int j = 0; j < 8; j++) {
						if (cluster_e[i] > float(topo_thresholds[j]/1000.0)) {
							ntopo_towards[j] += 1;
							sume_topo_towards[j] += cluster_e[i]/cosh(cluster_eta[i]);
							h_topo_towards[j]->Fill(cluster_e[i]/cosh(cluster_eta[i]));
							h_2D_topo_towards[j]->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
						}
					}
					h_ue_2D_towards->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
				} else if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { // transverse region 
					et_transverse += cluster_e[i]/cosh(cluster_eta[i]);
					for (int j = 0; j < 8; j++) {
						if (cluster_e[i] > float(topo_thresholds[j]/1000.0)) {
							ntopo_transverse[j] += 1;
							sume_topo_transverse[j] += cluster_e[i]/cosh(cluster_eta[i]);
							h_topo_transverse[j]->Fill(cluster_e[i]/cosh(cluster_eta[i]));
							h_2D_topo_transverse[j]->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
						}
					}
					h_ue_2D_transverse->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
				} else if (fabs(dphi) > (2.0*M_PI)/3.0) { // away region 
					et_away += cluster_e[i]/cosh(cluster_eta[i]);
					for (int j = 0; j < 8; j++) {
						if (cluster_e[i] > float(topo_thresholds[j]/1000.0)) {
							ntopo_away[j] += 1;
							sume_topo_away[j] += cluster_e[i]/cosh(cluster_eta[i]);
							h_topo_away[j]->Fill(cluster_e[i]/cosh(cluster_eta[i]));
							h_2D_topo_away[j]->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
						}
					}
					h_ue_2D_away->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
				}
			}
		}

		h_et_towards->Fill(et_towards);
		h_et_transverse->Fill(et_transverse);
		h_et_away->Fill(et_away);
		h_ue_towards->Fill(et_towards/(secteta*sectphi));
		h_ue_transverse->Fill(et_transverse/(secteta*sectphi));
		h_ue_away->Fill(et_away/(secteta*sectphi));

		for (int i = 0; i < 8; i++) {
			h_ntopo_towards[i]->Fill(ntopo_towards[i]);
			h_ntopo_transverse[i]->Fill(ntopo_transverse[i]);
			h_ntopo_away[i]->Fill(ntopo_away[i]);
			h_sume_topo_towards[i]->Fill(sume_topo_towards[i]);
			h_sume_topo_transverse[i]->Fill(sume_topo_transverse[i]);
			h_sume_topo_away[i]->Fill(sume_topo_away[i]);
		}

		h_ue_pt_towards->Fill(lead.Pt(),et_towards);
		h_ue_pt_transverse->Fill(lead.Pt(),et_transverse);
		h_ue_pt_away->Fill(lead.Pt(),et_away);
		if (dijet_bkg_cut) {
			h_ue_xj_towards->Fill(sub.Pt()/lead.Pt(),et_towards);
			h_ue_xj_transverse->Fill(sub.Pt()/lead.Pt(),et_transverse);
			h_ue_xj_away->Fill(sub.Pt()/lead.Pt(),et_away);
		}

		events++;

  	}

  	h_ue_2D_total->Scale(1.0/(events*deltaeta*deltaphi));
  	h_ue_2D_towards->Scale(1.0/(events*deltaeta*deltaphi));
  	h_ue_2D_transverse->Scale(1.0/(events*deltaeta*deltaphi));
  	h_ue_2D_away->Scale(1.0/(events*deltaeta*deltaphi));

  	for (int i = 0; i < 6; i++) {
  		h_2D_topo_towards[i]->Scale(1.0/(events*deltaeta*deltaphi));
  		h_2D_topo_transverse[i]->Scale(1.0/(events*deltaeta*deltaphi));
  		h_2D_topo_away[i]->Scale(1.0/(events*deltaeta*deltaphi));
  	}

  	h_ue_xj_towards->Scale(1.0/(secteta*sectphi));
  	h_ue_xj_transverse->Scale(1.0/(secteta*sectphi));
  	h_ue_xj_away->Scale(1.0/(secteta*sectphi));

  	h_ue_pt_towards->Scale(1.0/(secteta*sectphi));
  	h_ue_pt_transverse->Scale(1.0/(secteta*sectphi));
  	h_ue_pt_away->Scale(1.0/(secteta*sectphi));

  	out->Write();
  	out->Close();

}

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
#include "TRandom.h"
#include "TH1D.h"

using namespace std;

R__LOAD_LIBRARY(/sphenix/user/egm2153/calib_study/JetValidation/analysis/roounfold/libRooUnfold.so)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																			//
//	Note: 11.26.24 																															//
// 	This macro creates dijet QA plots, energy density QA plots with options for using towers, topoclusters or emcal clusters,				//
//	and plots of mean energy density vs jet variables (leading jet pT and dijet xj)															//
//	This macro also has option to do 1D unfolding/correction to truth level of total energy in the towards, transverse and away regions.	//
//																																			//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void dijet_ue_analysis() {

	string filename = "dijet_calo_analysis_data_emcalcls_simulation_pt7cut_wAj.root";
	std::cout << filename << std::endl;
	TFile *out = new TFile(filename.c_str(),"RECREATE");
	
	float ptbins[] = {5,6,7,8,9,10,12,14,16,18,20,22,24,28,30,35,40,50,60,80};
    int nptbins = sizeof(ptbins) / sizeof(ptbins[0]) - 1;
 
	// event and jet histograms 
	TH1F* h_vz = new TH1F("h_vz","",400, -100, 100);
	TH1F* h_njet = new TH1F("h_njet","",20,0,20);
	TH1F* h_dijetspectra = new TH1F("h_dijetspectra","",nptbins, ptbins);
  	TH1F* h_leadjet = new TH1F("h_leadjet","",nptbins, ptbins);
  	TH1F* h_subjet = new TH1F("h_subjet","",nptbins, ptbins);
  	TH1F* h_leadphi = new TH1F("h_leadphi","",50,-M_PI,M_PI);
  	TH1F* h_leadeta = new TH1F("h_leadeta","",50,-1.1,1.1);
  	TH1F* h_subphi = new TH1F("h_subphi","",50,-M_PI,M_PI);
  	TH1F* h_subeta = new TH1F("h_subeta","",50,-1.1,1.1);
  	TH1F* h_deltaphi = new TH1F("h_deltaphi","",125,-2*M_PI,2*M_PI);
  	TH1F* h_xj = new TH1F("h_xj","",20,0,1);
  	TH1F* h_pass_deltaphi = new TH1F("h_pass_deltaphi","",125,-2*M_PI,2*M_PI);
  	TH1F* h_pass_xj = new TH1F("h_pass_xj","",20,0,1);
  	TH1F* h_pass_spectra = new TH1F("h_pass_spectra","",nptbins, ptbins);
  	TH2F* h_aj_ptavg = new TH2F("h_aj_ptavg","",100,0,100,100,0,1);

  	// reconstructed ET histograms 
  	TH2F* h_ue_2D_total = new TH2F("h_ue_2D_total","",24,-1.1,1.1,32,0,M_PI);
  	TH2F* h_ue_2D_towards = new TH2F("h_ue_2D_towards","",24,-1.1,1.1,32,0,M_PI);
  	TH2F* h_ue_2D_transverse = new TH2F("h_ue_2D_transverse","",24,-1.1,1.1,32,0,M_PI);
  	TH2F* h_ue_2D_away = new TH2F("h_ue_2D_away","",24,-1.1,1.1,32,0,M_PI);

  	TH1F* h_ue_towards = new TH1F("h_ue_towards","",700,-20,50);
  	TH1F* h_ue_transverse = new TH1F("h_ue_transverse","",700,-20,50);
  	TH1F* h_ue_away = new TH1F("h_ue_away","",700,-20,50);

  	int topo_thresholds[] = {-9999,0,100,200,300,500};

  	TH1F* h_ntopo_towards[6];
  	TH1F* h_ntopo_transverse[6];
  	TH1F* h_ntopo_away[6];

  	TH1F* h_topo_towards[6];
  	TH1F* h_topo_transverse[6];
  	TH1F* h_topo_away[6];

  	TH2F* h_2D_topo_towards[6];
  	TH2F* h_2D_topo_transverse[6];
  	TH2F* h_2D_topo_away[6];

  	TH1F* h_sume_topo_towards[6];
  	TH1F* h_sume_topo_transverse[6];
  	TH1F* h_sume_topo_away[6];

  	for (int i = 0; i < 6; i++) {
  		h_ntopo_towards[i] = new TH1F(Form("h_ntopo%d_towards",topo_thresholds[i]),"",200,0,200);
  		h_ntopo_transverse[i] = new TH1F(Form("h_ntopo%d_transverse",topo_thresholds[i]),"",200,0,200);
  		h_ntopo_away[i] = new TH1F(Form("h_ntopo%d_away",topo_thresholds[i]),"",200,0,200);

  		h_topo_towards[i] = new TH1F(Form("h_topo%d_towards",topo_thresholds[i]),"",600,-10,50);
  		h_topo_transverse[i] = new TH1F(Form("h_topo%d_transverse",topo_thresholds[i]),"",600,-10,50);
  		h_topo_away[i] = new TH1F(Form("h_topo%d_away",topo_thresholds[i]),"",600,-10,50);

  		h_2D_topo_towards[i] = new TH2F(Form("h_2D_topo%d_towards",topo_thresholds[i]),"",24,-1.1,1.1,32,0,M_PI);
  		h_2D_topo_transverse[i] = new TH2F(Form("h_2D_topo%d_transverse",topo_thresholds[i]),"",24,-1.1,1.1,32,0,M_PI);
  		h_2D_topo_away[i] = new TH2F(Form("h_2D_topo%d_away",topo_thresholds[i]),"",24,-1.1,1.1,32,0,M_PI);

  		h_sume_topo_towards[i] = new TH1F(Form("h_sume_topo%d_towards",topo_thresholds[i]),"",550,-10,100);
  		h_sume_topo_transverse[i] = new TH1F(Form("h_sume_topo%d_transverse",topo_thresholds[i]),"",550,-10,100);
  		h_sume_topo_away[i] = new TH1F(Form("h_sume_topo%d_away",topo_thresholds[i]),"",550,-10,100);
  	}

  	TProfile* h_ue_xj_towards = new TProfile("h_ue_xj_towards","",20,0,1);
  	TProfile* h_ue_xj_transverse = new TProfile("h_ue_xj_transverse","",20,0,1);
  	TProfile* h_ue_xj_away = new TProfile("h_ue_xj_away","",20,0,1);

  	TProfile* h_ue_pt_towards = new TProfile("h_ue_pt_towards","",nptbins, ptbins);
  	TProfile* h_ue_pt_transverse = new TProfile("h_ue_pt_transverse","",nptbins, ptbins);
  	TProfile* h_ue_pt_away = new TProfile("h_ue_pt_away","",nptbins, ptbins);

  	// ET truth - reco response histograms 
  	TH1F* h_et_towards = new TH1F("h_et_towards","",700,-20,50);
  	TH1F* h_et_transverse = new TH1F("h_et_transverse","",700,-20,50);
  	TH1F* h_et_away = new TH1F("h_et_away","",700,-20,50);

  	TH1F* h_et_truth_towards = new TH1F("h_et_truth_towards","",700,-20,50);
  	TH1F* h_et_truth_transverse = new TH1F("h_et_truth_transverse","",700,-20,50);
  	TH1F* h_et_truth_away = new TH1F("h_et_truth_away","",700,-20,50);

  	TH2F* h_truth_vs_raw = new TH2F("h_truth_vs_raw","",700,-20,50,700,-20,50);
  	TProfile* h_ue_corr = new TProfile("h_ue_corr","",700,-20,50);
  	TProfile* h_ue_woffset = new TProfile("h_ue_woffset","",700,-20,50);
  	TH2F* h_ue_woffset_2D = new TH2F("h_ue_woffset_2D","",700,-20,50,700,-20,50);
  	
  	TProfile* h_truth_ue = new TProfile("h_truth_ue","",700,-20,50);
  	TProfile* h_calib_ue = new TProfile("h_calib_ue","",700,-20,50);
  	TH2F* h_truth_ue_2D = new TH2F("h_truth_ue_2D","",700,-20,50,700,-20,50);
  	TH2F* h_calib_ue_2D = new TH2F("h_calib_ue_2D","",700,-20,50,700,-20,50);
  	TProfile* h_reco = new TProfile("h_reco","",700,-20,50);
  	TProfile* h_c1truth = new TProfile("h_c1truth","",700,-20,50);

  	// ET calibration closure histograms
  	TProfile* h_truth_vs_calib = new TProfile("h_truth_vs_calib","",700,-20,50);
  	TProfile* h_ue_corr_xj = new TProfile("h_ue_corr_xj","",20,0,1);
  	TProfile* h_ue_corr_pt = new TProfile("h_ue_corr_pt","",nptbins, ptbins);
  	TH2F* h_ue_corr_xj_2D = new TH2F("h_ue_corr_xj_2D","",20,0,1,700,-20,50);
  	//TH2F* h_ue_corr_pt_2D = new TH2F("h_ue_corr_pt_2D","",nptbins,ptbins,700,-20,50);

  	TProfile* h_truth_truth = new TProfile("h_truth_truth","",700,-20,50);
  	TProfile* h_calib_truth = new TProfile("h_calib_truth","",700,-20,50);
  	TProfile* h_calib_truth_truth = new TProfile("h_calib_truth_truth","",700,-20,50);

  	// should create unfolding histograms and response matrices here
  	//Needed for gaus function
  	TRandom3 obj;

  	//random number in each event
  	TRandom3 Random;
   
    float etmin = -10;
    float etmax = 200;
    float etbins = (etmax - etmin) * 10;
    bool doUnfolding = false;
   	bool sim = false;
   	bool clusters = true;
   	bool emcal_clusters = true;
   	bool applyCorr = true;

   	// response offset and corr
   	// calo waveform 
   	// float resp_offset = 4.67585; float resp_corr = 0.692668;
   	// calo cluster 
   	// float resp_offset = 10.4441; float resp_corr = 0.704824;
   	// calo nozero
   	float resp_offset = 2.8579; float resp_corr = 0.7513;
   	if (clusters) {
   		// calo waveform 
   		// resp_offset = 2.01394; resp_corr = 0.629988;
   		// calo cluster 
   		// resp_offset = 2.19422; resp_corr = 0.684463;
   		// calo nozero
   		resp_offset = 2.13103; resp_corr = 0.675456;
   	}
   	float fit_corr = 0.739248; float fit_offset = 1.58456;

	//defining Meas and Truth Histograms
    TH1D* hMeasET = new TH1D("hMeasET","",etbins,etmin,etmax);
    TH1D* hTruthET = new TH1D("hTruthET","",etbins,etmin,etmax);
    TH1D* hRecoET = new TH1D("hRecoET","",etbins,etmin,etmax);

    // closure test histograms 
    TH1D* hMeasETHalf = new TH1D("hMeasETHalf","",etbins,etmin,etmax);
    TH1D* hTruthETHalf = new TH1D("hTruthETHalf","",etbins,etmin,etmax);
    TH1D* hRecoETHalf = new TH1D("hRecoETHalf","",etbins,etmin,etmax);

    //making response matrices
    RooUnfoldResponse *resp_full = new RooUnfoldResponse(etbins,etmin,etmax,etbins,etmin,etmax,"resp_full","");
    RooUnfoldResponse *resp_half = new RooUnfoldResponse(etbins,etmin,etmax,etbins,etmin,etmax,"resp_half","");
    RooUnfoldResponse *resp_test = new RooUnfoldResponse(etbins,etmin,etmax,etbins,etmin,etmax,"resp_test","");

    //histograms for errors
    TH2D* hResponseTruthMeasFull = new TH2D("hResponseTruthMeasFull","",etbins,etmin,etmax,etbins,etmin,etmax);
    TH2D* hResponseTruthMeasHalf = new TH2D("hResponseTruthMeasHalf","",etbins,etmin,etmax,etbins,etmin,etmax);

 	TChain chain("T");

	const char* inputDirectory = "/sphenix/tg/tg01/jets/egm2153/JetValOutput/";
	//TString wildcardPath = TString::Format("%ssim_topocluster_output.root", inputDirectory);
	//TString wildcardPath = TString::Format("%ssim_calo_cluster_topocluster_output.root", inputDirectory);
	//TString wildcardPath = TString::Format("%ssim_calo_nozero_topocluster_output.root", inputDirectory);
	//TString wildcardPath = TString::Format("%ssim_detroit_jet10_topocluster_output.root", inputDirectory);
	//TString wildcardPath = TString::Format("%ssim_detroit_jet10_emcal_cluster_output.root", inputDirectory);
	TString wildcardPath = TString::Format("%semcal_cluster_output.root", inputDirectory);
	//TString wildcardPath = TString::Format("%sdata_topo_output.root", inputDirectory);

	chain.Add(wildcardPath);
	//chain.Add(wildcardPath1);
	std::cout << wildcardPath << std::endl;

	int m_event;
	int nJet;
	float zvtx;
	float deltaeta = 0.0916667;
	float deltaphi = 0.0981748;
	float secteta = 2.2;
	float sectphi = (2.0*M_PI)/3.0;

	vector<int> *triggerVector = nullptr;
	vector<float> *eta = nullptr;
	vector<float> *phi = nullptr;
	vector<float> *e = nullptr;
	vector<float> *pt = nullptr;

	int emcaln = 0;
	float emcale[24576] = {0.0};
	float emcaleta[24576] = {0.0};
	float emcalphi[24576] = {0.0};

	int ihcaln = 0;
	float ihcale[24576] = {0.0};
	float ihcaleta[24576] = {0.0};
	float ihcalphi[24576] = {0.0};

	int ohcaln = 0;
	float ohcale[24576] = {0.0};
	float ohcaleta[24576] = {0.0};
	float ohcalphi[24576] = {0.0};

	int clsmult = 0;
	float cluster_e[10000] = {0.0};
	float cluster_eta[10000] = {0.0};
	float cluster_phi[10000] = {0.0};

	chain.SetBranchAddress("m_event",&m_event);
	chain.SetBranchAddress("nJet",&nJet);
	chain.SetBranchAddress("zvtx",&zvtx);
	chain.SetBranchAddress("triggerVector",&triggerVector);
	chain.SetBranchAddress("eta",&eta);
	chain.SetBranchAddress("phi",&phi);
	chain.SetBranchAddress("e",&e);
	chain.SetBranchAddress("pt",&pt);

	chain.SetBranchAddress("emcaln",&emcaln);
	chain.SetBranchAddress("emcale",emcale);
	chain.SetBranchAddress("emcaleta",emcaleta);
	chain.SetBranchAddress("emcalphi",emcalphi);

	chain.SetBranchAddress("ihcaln",&ihcaln);
	chain.SetBranchAddress("ihcale",ihcale);
	chain.SetBranchAddress("ihcaleta",ihcaleta);
	chain.SetBranchAddress("ihcalphi",ihcalphi);

	chain.SetBranchAddress("ohcaln",&ohcaln);
	chain.SetBranchAddress("ohcale",ohcale);
	chain.SetBranchAddress("ohcaleta",ohcaleta);
	chain.SetBranchAddress("ohcalphi",ohcalphi);

	if (!emcal_clusters) {
		chain.SetBranchAddress("clsmult",&clsmult);
		chain.SetBranchAddress("cluster_e",cluster_e);
		chain.SetBranchAddress("cluster_eta",cluster_eta);
		chain.SetBranchAddress("cluster_phi",cluster_phi);
	} else {
		chain.SetBranchAddress("emcal_clsmult",&clsmult);
		chain.SetBranchAddress("emcal_cluster_e",cluster_e);
		chain.SetBranchAddress("emcal_cluster_eta",cluster_eta);
		chain.SetBranchAddress("emcal_cluster_phi",cluster_phi);
	}

	int truthpar_n = 0;
	float truthpar_e[100000] = {0};
	float truthpar_eta[100000] = {0};
	float truthpar_phi[100000] = {0};
	int truthpar_pid[100000] = {0};
	if (doUnfolding && sim) {
		chain.SetBranchAddress("truthpar_n",&truthpar_n);
		chain.SetBranchAddress("truthpar_e",truthpar_e);
		chain.SetBranchAddress("truthpar_eta",truthpar_eta);
		chain.SetBranchAddress("truthpar_phi",truthpar_phi);
		chain.SetBranchAddress("truthpar_pid",truthpar_pid);
	}

	TFile *corrFile;
  	TF1 *correction = new TF1("jet energy correction","1",0,80);
  	if(applyCorr) {
      	corrFile = new TFile("JES_IsoCorr_NumInv.root","READ");
      	corrFile -> cd();
      	correction = (TF1*)corrFile -> Get("corrFit_Iso0");
    }

	int eventnumber = 0;
	int events = 0;
    Long64_t nEntries = chain.GetEntries();
    std::cout << nEntries << std::endl;
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
    //for (Long64_t entry = 0; entry < 100000; ++entry) {
        chain.GetEntry(entry);
    	if (eventnumber % 10000 == 0) cout << "event " << eventnumber << endl;
    	eventnumber++;

  		bool jettrig = false;
  		for (auto t : *triggerVector) {
  			if (t >= 16 && t <= 23) {
  				jettrig = true;
  				break;
  			}
  		}

  		bool negJet = false;
  		for (int i = 0; i < nJet; i++) {
  			if ((*e)[i] < 0) {
  				negJet = true;
  			}
  		}

  		if (!jettrig && !sim) { continue; }
  		if (isnan(zvtx)) { continue; }
  		if (zvtx < -30 || zvtx > 30) { continue; }
  		if (negJet) { continue; }
  		if (nJet < 2) { continue; }

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

  		if (fabs((*eta)[ind_lead]) > 0.7 || fabs((*eta)[ind_sub]) > 0.7) { continue; }
  		if ((*pt)[ind_lead] < 12 || (*pt)[ind_sub] < 7) { continue; }

  		h_vz->Fill(zvtx);
  		h_dijetspectra->Fill((*pt)[ind_lead]);
  		h_dijetspectra->Fill((*pt)[ind_sub]);

  		TVector3 lead, sub;
  		if (applyCorr) {
    		lead.SetPtEtaPhi((*pt)[ind_lead]*correction->Eval((*pt)[ind_lead]), (*eta)[ind_lead], (*phi)[ind_lead]);
    		sub.SetPtEtaPhi((*pt)[ind_sub]*correction->Eval((*pt)[ind_sub]), (*eta)[ind_sub], (*phi)[ind_sub]);
    	} else {
    		lead.SetPtEtaPhi((*pt)[ind_lead], (*eta)[ind_lead], (*phi)[ind_lead]);
    		sub.SetPtEtaPhi((*pt)[ind_sub], (*eta)[ind_sub], (*phi)[ind_sub]);
    	}

  		h_leadjet->Fill(lead.Pt());
  		h_subjet->Fill(sub.Pt());
  		h_leadphi->Fill(lead.Phi());
  		h_leadeta->Fill(lead.Eta());
  		h_subphi->Fill(sub.Phi());
  		h_subeta->Fill(sub.Eta());
  		h_deltaphi->Fill(lead.DeltaPhi(sub));
  		h_xj->Fill(sub.Pt()/lead.Pt());

  		// require deltaphi > 2.75 (7pi/8)
  		// transverse region is [pi/3,2pi/3] from leading jet 
  		if (fabs(lead.DeltaPhi(sub)) > 2.75) {
  			h_pass_deltaphi->Fill(lead.DeltaPhi(sub));
  			h_pass_xj->Fill(sub.Pt()/lead.Pt());
  			h_pass_spectra->Fill(lead.Pt());
  			h_pass_spectra->Fill(sub.Pt());
  			h_aj_ptavg->Fill((lead.Pt()+sub.Pt())/2.0, (lead.Pt()-sub.Pt())/(lead.Pt()+sub.Pt()));

  			// should fill truth and reco et histograms here

  			float et_towards = 0;
  			float et_transverse = 0;
  			float et_away = 0;
  			int ntopo_towards[] = {0,0,0,0,0,0};
  			int ntopo_transverse[] = {0,0,0,0,0,0};
  			int ntopo_away[] = {0,0,0,0,0,0};
  			float sume_topo_towards[] = {0,0,0,0,0,0};
  			float sume_topo_transverse[] = {0,0,0,0,0,0};
  			float sume_topo_away[] = {0,0,0,0,0,0};

  			if (!clusters) {
	  			for (int i = 0; i < emcaln; i++) {
	  				TVector3 em;
	  				em.SetPtEtaPhi(emcale[i]/cosh(emcaleta[i]),emcaleta[i],emcalphi[i]);
	  				float dphi = lead.DeltaPhi(em);
	  				h_ue_2D_total->Fill(emcaleta[i],dphi,emcale[i]/cosh(emcaleta[i]));
	  				if (fabs(dphi) < M_PI/3.0) {
	  					et_towards += emcale[i]/cosh(emcaleta[i]);
	  					h_ue_2D_towards->Fill(emcaleta[i],dphi,emcale[i]/cosh(emcaleta[i]));
	  				} else if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) {
						et_transverse += emcale[i]/cosh(emcaleta[i]);
	  					h_ue_2D_transverse->Fill(emcaleta[i],dphi,emcale[i]/cosh(emcaleta[i]));
	  				} else if (fabs(dphi) > (2.0*M_PI)/3.0) {
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
	  				} else if (fabs(dphi) > (2.0*M_PI)/3.0) {
	  					et_away += ohcale[i]/cosh(ohcaleta[i]);
	  					h_ue_2D_away->Fill(ohcaleta[i],dphi,ohcale[i]/cosh(ohcaleta[i]));
	  				}
	  			}
	  		} else {
  				for (int i = 0; i < clsmult; i++) {
	  				TVector3 cls;
	  				cls.SetPtEtaPhi(cluster_e[i]/cosh(cluster_eta[i]),cluster_eta[i],cluster_phi[i]);
	  				float dphi = lead.DeltaPhi(cls);
	  				h_ue_2D_total->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
	  				if (fabs(dphi) < M_PI/3.0) {
	  					et_towards += cluster_e[i]/cosh(cluster_eta[i]);
	  					for (int j = 0; j < 6; j++) {
	  						if (cluster_e[i] > float(topo_thresholds[j]/1000.0)) {
	  							ntopo_towards[j] += 1;
	  							sume_topo_towards[j] += cluster_e[i]/cosh(cluster_eta[i]);
	  							h_topo_towards[j]->Fill(cluster_e[i]/cosh(cluster_eta[i]));
	  							h_2D_topo_towards[j]->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
	  						}
	  					}
	  					h_ue_2D_towards->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
	  				} else if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) {
						et_transverse += cluster_e[i]/cosh(cluster_eta[i]);
	  					for (int j = 0; j < 6; j++) {
	  						if (cluster_e[i] > float(topo_thresholds[j]/1000.0)) {
	  							ntopo_transverse[j] += 1;
	  							sume_topo_transverse[j] += cluster_e[i]/cosh(cluster_eta[i]);
	  							h_topo_transverse[j]->Fill(cluster_e[i]/cosh(cluster_eta[i]));
	  							h_2D_topo_transverse[j]->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
	  						}
	  					}
	  					h_ue_2D_transverse->Fill(cluster_eta[i],dphi,cluster_e[i]/cosh(cluster_eta[i]));
	  				} else if (fabs(dphi) > (2.0*M_PI)/3.0) {
	  					et_away += cluster_e[i]/cosh(cluster_eta[i]);
	  					for (int j = 0; j < 6; j++) {
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

  			for (int i = 0; i < 6; i++) {
  				h_ntopo_towards[i]->Fill(ntopo_towards[i]);
  				h_ntopo_transverse[i]->Fill(ntopo_transverse[i]);
  				h_ntopo_away[i]->Fill(ntopo_away[i]);
  				h_sume_topo_towards[i]->Fill(sume_topo_towards[i]);
  				h_sume_topo_transverse[i]->Fill(sume_topo_transverse[i]);
  				h_sume_topo_away[i]->Fill(sume_topo_away[i]);
  			}

  			h_ue_xj_towards->Fill(sub.Pt()/lead.Pt(),et_towards);
  			h_ue_xj_transverse->Fill(sub.Pt()/lead.Pt(),et_transverse);
  			h_ue_xj_away->Fill(sub.Pt()/lead.Pt(),et_away);

  			h_ue_pt_towards->Fill(lead.Pt(),et_towards);
  			h_ue_pt_transverse->Fill(lead.Pt(),et_transverse);
  			h_ue_pt_away->Fill(lead.Pt(),et_away);

  			if (doUnfolding && sim) {
  				double  choice = Random.Rndm();

  				float truth_et_towards = 0;
  				float truth_et_transverse = 0;
  				float truth_et_away = 0;

  				for (int i = 0; i < truthpar_n; i++) {
  					if (fabs(truthpar_eta[i]) > 1.1) { continue; }
  					if ((truthpar_pid[i] == 22 || truthpar_pid[i] == 111) && fabs(truthpar_e[i]) < 0.2) { continue; }
  					else if (fabs(truthpar_e[i]) < 0.5) { continue; }
  					TVector3 truth;
  					truth.SetPtEtaPhi(truthpar_e[i]/cosh(truthpar_eta[i]),truthpar_eta[i],truthpar_phi[i]);
  					float dphi = lead.DeltaPhi(truth);
	  				if (fabs(dphi) < M_PI/3.0) { truth_et_towards += truthpar_e[i]/cosh(truthpar_eta[i]); } 
	  				else if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { truth_et_transverse += truthpar_e[i]/cosh(truthpar_eta[i]); } 
	  				else if (fabs(dphi) > (2.0*M_PI)/3.0) { truth_et_away += truthpar_e[i]/cosh(truthpar_eta[i]); }

	  				h_et_truth_towards->Fill(truth_et_towards);
	  				h_et_truth_transverse->Fill(truth_et_transverse);
	  				h_et_truth_away->Fill(truth_et_away);
	  				
	  				if (choice > 0.50) {
	  					resp_half->Fill(et_transverse,truth_et_transverse);
	  				}

	  				if (choice < 0.50) {
	  					hTruthETHalf->Fill(truth_et_transverse);
	  					hMeasETHalf->Fill(et_transverse);
	  				}
	  				
	  				resp_full->Fill(et_transverse,truth_et_transverse);
	  				hTruthET->Fill(truth_et_transverse);
	  				hMeasET->Fill(et_transverse);

	  				float calib_et_transverse = (et_transverse - resp_offset)/resp_corr;

  					h_truth_vs_raw->Fill(truth_et_transverse, et_transverse);
  					h_ue_corr->Fill(truth_et_transverse, et_transverse);
  					h_ue_woffset->Fill(truth_et_transverse, et_transverse - resp_offset);
  					h_ue_woffset_2D->Fill(truth_et_transverse, et_transverse - resp_offset);

  					h_reco->Fill(et_transverse, et_transverse);
  					h_c1truth->Fill(et_transverse, -fit_corr*truth_et_transverse);

  					h_truth_ue->Fill(et_transverse - resp_offset, truth_et_transverse);
  					h_truth_ue_2D->Fill(et_transverse - resp_offset, truth_et_transverse);
	  				h_calib_ue->Fill(et_transverse - resp_offset, et_transverse - resp_offset);
	  				h_calib_ue_2D->Fill(et_transverse - resp_offset, et_transverse - resp_offset);

	  				h_truth_truth->Fill(truth_et_transverse, truth_et_transverse);
	  				h_calib_truth->Fill(truth_et_transverse, calib_et_transverse);
	  				
		  			if (truth_et_transverse > 0) {

		  				h_truth_vs_calib->Fill(truth_et_transverse, calib_et_transverse);
		  				h_ue_corr_pt->Fill(lead.Pt(), calib_et_transverse/truth_et_transverse);
		  				h_ue_corr_xj->Fill(sub.Pt()/lead.Pt(), calib_et_transverse/truth_et_transverse);
		  				//h_ue_corr_pt_2D->Fill(lead.Pt(), calib_et_transverse/truth_et_transverse);
		  				h_ue_corr_xj_2D->Fill(sub.Pt()/lead.Pt(), calib_et_transverse/truth_et_transverse);
		  				h_calib_truth_truth->Fill(truth_et_transverse, calib_et_transverse/truth_et_transverse);
		  			}
  				}
  			}
  			events++;
  		}

  	}

  	/*
  	RooUnfoldBayes full_unfold(resp_full, hMeasET, 4);
	RooUnfoldBayes half_unfold(resp_half, hMeasETHalf, 4);

	hRecoET = (TH1D*) full_unfold.Hreco();
	hRecoETHalf = (TH1D*) half_unfold.Hreco();
	*/

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
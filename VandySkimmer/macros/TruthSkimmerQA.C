#pragma once 
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <TTree.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TExec.h>

#include "Special_colors.h"
//#include "PlottingConfigs.h"
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
//#include "DrawingMacro.C"
 
//#include <vandyclasses/EventInfo.h>
//#include <vandyclasses/JetInfo.h>
//#include <vandyclasses/Tower.h>

#include <vector>
#include <array>
#include <math.h>
#include <string>
#include <numbers>
#include <utility>

R__LOAD_LIBRARY(libVandyClasses.so);

//PlottingConfigs* conf {nullptr};
Skaydis_colors* stylepoints {nullptr};

TExec* les {nullptr};
TExec* bi {nullptr};
TExec* trans {nullptr};
TExec* enby {nullptr};
std::string tag{""};
bool isData {false}; 

TH1F* wEECtruthtowers 	{nullptr};
TH1F* wEECtruthparticles{nullptr};
TH1F* wEECrecotowers	{nullptr};

TH1F* pairtruthtowers 	{nullptr};
TH1F* pairtruthparticles{nullptr};
TH1F* pairrecotowers	{nullptr};

const std::vector<double> new_dPhiBins = {
//    0.0000000, 0.041592654, 0.14159265,
    0.0000000, 0.14159265,
    0.34159265, 0.54159265, 0.74159265, 0.94159265,
    1.2415927, 1.5415927, 1.8415927, 2.1415927,
    2.3415927, 2.5415927, 2.7415927, 2.9415927,
    3.0415927, 3.1415927
};
std::vector<float> pairweights = {0.,5e-5};
void makePairweightbins()
{
	float upper = 2;
	float second_to_upper = 1;
	float loglower = std::log10(pairweights[1]);
	float logupper = std::log10(second_to_upper);
	int nlogbins = 17;
	float delta = (logupper-loglower)/((float) nlogbins-2);
	for(int i=1; i < nlogbins; i++){
	       float binedge = std::pow(10, loglower + i*delta);
	       if( binedge == pairweights[1] ) continue;
	       else if (binedge >= second_to_upper) break;
	       pairweights.push_back(binedge);
	}
	pairweights.push_back(second_to_upper);
	pairweights.push_back(upper);
	return;
}
void InitJetHistos(
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* holding_array,
		std::vector<JetInfo>* jt,
		TH1F* pt, 
		int type,
		std::string nametag,
		std::string tag
		)
{
	pt=new TH1F(std::format("h_{0}_jet_r0{1}", nametag, type+2).c_str(), 
			std::format("{} Jet R=0.{}; p_{{T}} [GeV]; #sigma", tag, type+2).c_str(),
		       25, -0.5, 99.5 );
	std::pair<std::vector<JetInfo>*, TH1F*> ha = std::make_pair(jt, pt);
	holding_array->at(type)=&ha;
	return;	
}
void InitTowerHistos(
		TH1F* h_tower,
		std::string nametag, 
		std::string tag
		)
{
	h_tower = new TH1F(std::format("h_{}_e", nametag).c_str(), 
			std::format("{} Energy; E [GeV]; N_{{towers}}", tag).c_str(),
			1000, 1e-4, 10);
	return;	
}

void InitwEECHistos()
{

	wEECtruthtowers		= new TH1F(
		"h_truth_wEEC", 
		"Truth tower wEEC; #Delta #varphi; wEEC", 
		32, new_dPhiBins.data() );

	wEECtruthparticles	= new TH1F(
		"h_part_wEEC", 
		"Truth particle level wEEC; #Delta #varphi; wEEC", 
		32, new_dPhiBins.data() );

	wEECrecotowers		= new TH1F(
		"h_reco_wEEC", 
		"Reco tower wEEC; #Delta #varphi; wEEC", 
		32, new_dPhiBins.data() );
	return;
}

void InitPairHistos()
{
	pairtruthtowers		= new TH1F(
		"h_truth_pair", 
		"Truth tower pair; #frac{E_{T,1}E_{T,2}}{p_{T, avg}^{2}}; N_{pair}", 
		pairweights.size(), pairweights.data() );

	pairtruthparticles	= new TH1F(
		"h_part_pair", 
		"Truth particle level pair; #frac{E_{T,1}E_{T,2}}{p_{T, avg}^{2}}; N_{pair}", 
		pairweights.size(), pairweights.data() );

	pairrecotowers		= new TH1F(
		"h_reco_pair", 
		"Reco tower pair; #frac{E_{T,1}E_{T,2}}{p_{T, avg}^{2}}; N_{pair}", 
		pairweights.size(), pairweights.data() );
	return;
}

void SetJetBranches(
		TTree* t, 
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* truthjets, 
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* towerjets, 
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* truthtowerjets
		)
{
	if(!isData)
	{
		for(int i=0; i<(int)truthjets->size(); i++)
		{
			if(!truthjets->at(i)) continue;
			t->SetBranchAddress(std::format("TruthJetInfo_r0{}", i+2).c_str(), &(truthjets->at(i)->first));
		}
		for(int i=0; i<(int)truthtowerjets->size(); i++)
		{
			if(!t->FindBranch(std::format("TruthTowerJetInfo_r0{}", i+2).c_str())) break;
			if(!truthtowerjets->at(i) ) continue;
			t->SetBranchAddress(std::format("TruthTowerJetInfo_r0{}", i+2).c_str(), &(truthtowerjets->at(i)->first));
		}
	}
	for(int i=0; i<(int)towerjets->size(); i++)
	{
		if(!towerjets->at(i)) continue;
		t->SetBranchAddress(std::format("JetInfo_r0{}", i+2).c_str(), &(towerjets->at(i)->first));
	}
	return;	
}

void SetTowerBranches(
		TTree* t,
		std::vector<Tower>* particle,
		std::vector<Tower>* truth_tower,
		std::vector<Tower>* reco_tower
		)
{
	if(isData)
	{
		if(particle)	t->SetBranchAddress("TruthParticles", &particle);
		if(truth_tower) t->SetBranchAddress("TruthTowers", &truth_tower);
	}
	
	if(reco_tower)	t->SetBranchAddress("TowerInfo", &reco_tower);
	return;
}

void getwEEC(std::vector<Tower>* towers, TH1F* output_hist, float Q2)
{
	for(int i=0; i<(int)towers->size()-1; i++)
	{
		float pt1 = std::pow(towers->at(i).px(), 2) + std::pow(towers->at(i).py(), 2);
		float phi1 = std::atan2(towers->at(i).py(), towers->at(i).px());
		pt1 = std::sqrt(pt1);
		for(int j=i+1; j<(int)towers->size(); j++)
		{
			float pt2 = std::pow(towers->at(j).px(), 2) + std::pow(towers->at(j).py(), 2);
			pt2 = std::sqrt(pt2);
			float phi2 = std::atan2(towers->at(j).py(), towers->at(j).px());
			float pw = pt1 * pt2/Q2;
			float dphi = phi1 - phi2;
			if(dphi < M_PI ) dphi+=2*M_PI;
			if(dphi > M_PI ) dphi-=2*M_PI;
			output_hist->Fill(dphi, pw);
		}
	}
	return;
}

void eventLoop(
		int event_n,	
		TTree* t, 
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* 	truthjets, 
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* 	towerjets, 
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>*	truthtowerjets,
		std::pair<std::vector<Tower>*, TH1F*>*		truthparticles,
		std::pair<std::vector<Tower>*, TH1F*>*		truthtowers,
		std::pair<std::vector<Tower>*, TH1F*>*		recotowers,
		EventInfo*					event
	      )
{
	std::cout<<"Event " <<event_n <<" out of " <<t->GetEntries() <<std::endl;
	t->GetEntry(event_n);
	std::cout<<__LINE__<<std::endl;
	bool isDijet = true;
	float Q2 = 1.;
	for(int i=0; i< 5/*(int)event->is_dijet_event->size()*/; i++)
	{
		if(!event->is_dijet_event(i)) isDijet=false;
	}
	if(!isDijet) return;
	std::cout<<__LINE__<<std::endl;

	if(!isData)
	{
		bool isTruthDijet = true;
		for(int i=0; i< 5/*(int)event->is_dijetTruth_event()->size()*/; i++)
		{
			if(!event->is_dijetTruth_event(i)) isTruthDijet=false;
		}
		if(!isTruthDijet) return;
		Q2 = std::pow(event->get_leadTruth_pT(2),2) +std::pow( event->get_subleadTruth_pT(2), 2);
		for(int i=0; i<(int)truthjets->size(); i++)
		{
			if(!truthjets->at(i) || 
					!truthjets->at(i)->second
					) continue;
			for(int j=0; j<(int) truthjets->at(i)->first->size(); j++)
				truthjets->at(i)->second->Fill(truthjets->at(i)->first->at(j).pt());
		}
		getwEEC(truthparticles->first, wEECtruthparticles, Q2);
	std::cout<<__LINE__<<std::endl;

		for(int i=0; i<(int)truthtowerjets->size(); i++)
		{
			if(!t->FindBranch(std::format("TruthTowerJetInfo_r0{}", i+2).c_str())) break;
			if(!truthtowerjets->at(i) || 
					!truthtowerjets->at(i)->second
					) continue;
			for(int j=0; j<(int) truthtowerjets->at(i)->first->size(); j++)
				truthtowerjets->at(i)->second->Fill(truthtowerjets->at(i)->first->at(j).pt());
		}
	std::cout<<__LINE__<<std::endl;

		for(int i=0; i<(int)truthtowers->first->size(); i++)
		{
			if(!truthtowers || 
					!truthtowers->first ||
					!truthtowers->second
			  ) continue;
			truthtowers->second->Fill(truthtowers->first->at(i).e());
		}
	std::cout<<__LINE__<<std::endl;

		getwEEC(truthtowers->first, wEECtruthtowers, Q2);

	}
	Q2 = std::pow(event->get_lead_pT(2),2) + std::pow(event->get_sublead_pT(2),2);
	std::cout<<__LINE__<<std::endl;

	for(int i=0; i<(int)towerjets->size(); i++)
	{
		if(!towerjets->at(i) || 
				!towerjets->at(i)->second
				) continue;
			for(int j=0; j<(int) towerjets->at(i)->first->size(); j++)
				towerjets->at(i)->second->Fill(towerjets->at(i)->first->at(j).pt());
	}
	std::cout<<__LINE__<<std::endl;
	for(int i=0; i<(int)recotowers->first->size(); i++)
	{
		if(!recotowers || 
				!recotowers->first ||
				!recotowers->second
		  ) continue;
		recotowers->second->Fill(recotowers->first->at(i).e());
		
		getwEEC(recotowers->first, wEECrecotowers, Q2);
	}
	std::cout<<__LINE__<<std::endl;
	return;	
}

void DrawPlots(
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* 	truthjets, 
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* 	towerjets, 
		std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5>*	truthtowerjets,
		std::pair<std::vector<Tower>*, TH1F*>*		truthparticles,
		std::pair<std::vector<Tower>*, TH1F*>*		truthtowers,
		std::pair<std::vector<Tower>*, TH1F*>*		recotowers,
		float weight,
		std::string output_file_name
	)
{
	TFile* output = new TFile(output_file_name.c_str(), "RECREATE");
	return;
}

int TruthSkimmerQA(std::string input_file_name, std::string outputQA_name, bool isD=false, std::string gen="Pythia")
{
	tag 		= gen;
	stylepoints 	= new Skaydis_colors();
	isData 		= isD;
	//setting up the pallets
	trans=new TExec("trans_pallet", "gStyle->SetPalette(100, style_points->Trans_gradient_PT);");
	bi=new TExec("bi_pallet", "gStyle->SetPalette(100, style_points->Trans_gradient_PT);");
	les=new TExec("les_pallet", "gStyle->SetPalette(100, style_points->Trans_gradient_PT);");
	enby=new TExec("enby_pallet", "gStyle->SetPalette(100, style_points->Trans_gradient_PT);");
	TFile* input_file = new TFile(input_file_name.c_str(), "READ");
	input_file->cd();
	//make the branches
	
	//truth jets
	std::vector<JetInfo>* truthjet02 {nullptr};
	std::vector<JetInfo>* truthjet03 {nullptr};
	std::vector<JetInfo>* truthjet04 {nullptr};
	std::vector<JetInfo>* truthjet05 {nullptr};
	std::vector<JetInfo>* truthjet06 {nullptr};
	//accompaning histograms 
	TH1F* truthjet02pt {nullptr};	
	TH1F* truthjet03pt {nullptr};	
	TH1F* truthjet04pt {nullptr};	
	TH1F* truthjet05pt {nullptr};	
	TH1F* truthjet06pt {nullptr};	
	
	//tower jets
	std::vector<JetInfo>* towerjet02 {nullptr};
	std::vector<JetInfo>* towerjet03 {nullptr};
	std::vector<JetInfo>* towerjet04 {nullptr};
	std::vector<JetInfo>* towerjet05 {nullptr};
	std::vector<JetInfo>* towerjet06 {nullptr};
	//accompaning histograms 
	TH1F* towerjet02pt {nullptr};	
	TH1F* towerjet03pt {nullptr};	
	TH1F* towerjet04pt {nullptr};	
	TH1F* towerjet05pt {nullptr};	
	TH1F* towerjet06pt {nullptr};	
	
	//truthtower jets
	std::vector<JetInfo>* truthtowerjet02 {nullptr};
	std::vector<JetInfo>* truthtowerjet03 {nullptr};
	std::vector<JetInfo>* truthtowerjet04 {nullptr};
	std::vector<JetInfo>* truthtowerjet05 {nullptr};
	std::vector<JetInfo>* truthtowerjet06 {nullptr};
	//accompaning histograms 
	TH1F* truthtowerjet02pt {nullptr};	
	TH1F* truthtowerjet03pt {nullptr};	
	TH1F* truthtowerjet04pt {nullptr};	
	TH1F* truthtowerjet05pt {nullptr};	
	TH1F* truthtowerjet06pt {nullptr};	

	//to have and to hold
	std::array< std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* truthjets = new std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5> {};
	std::array< std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* towerjets = new std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5> {};
	std::array< std::pair<std::vector<JetInfo>*, TH1F*>*, 5>* truthtowerjets = new std::array<std::pair<std::vector<JetInfo>*, TH1F*>*, 5> {};

	InitJetHistos(truthjets, truthjet02, truthjet02pt, 0, "truth", "Truth Jets");
	InitJetHistos(truthjets, truthjet03, truthjet03pt, 1, "truth", "Truth Jets");
	InitJetHistos(truthjets, truthjet04, truthjet04pt, 2, "truth", "Truth Jets");
	InitJetHistos(truthjets, truthjet05, truthjet05pt, 3, "truth", "Truth Jets");
	InitJetHistos(truthjets, truthjet06, truthjet06pt, 4, "truth", "Truth Jets");

	InitJetHistos(towerjets, towerjet02, towerjet02pt, 0, "reco_tower", "Reco Tower Jets");
	InitJetHistos(towerjets, towerjet03, towerjet03pt, 1, "reco_tower", "Reco Tower Jets");
	InitJetHistos(towerjets, towerjet04, towerjet04pt, 2, "reco_tower", "Reco Tower Jets");
	InitJetHistos(towerjets, towerjet05, towerjet05pt, 3, "reco_tower", "Reco Tower Jets");
	InitJetHistos(towerjets, towerjet06, towerjet06pt, 4, "reco_tower", "Reco Tower Jets");

	InitJetHistos(truthtowerjets, truthtowerjet02, truthtowerjet02pt, 0, "truth_tower", "Truth Tower Jets");
	InitJetHistos(truthtowerjets, truthtowerjet03, truthtowerjet03pt, 1, "truth_tower", "Truth Tower Jets");
	InitJetHistos(truthtowerjets, truthtowerjet04, truthtowerjet04pt, 2, "truth_tower", "Truth Tower Jets");
	InitJetHistos(truthtowerjets, truthtowerjet05, truthtowerjet05pt, 3, "truth_tower", "Truth Tower Jets");
	InitJetHistos(truthtowerjets, truthtowerjet06, truthtowerjet06pt, 4, "truth_tower", "Truth Tower Jets");

	TTree* t = (TTree*) input_file->Get("T");
	SetJetBranches(t, truthjets, towerjets, truthtowerjets);
	std::vector<Tower>* truthparticles_V = new std::vector<Tower> {};
	std::vector<Tower>* truthtowers_V = new std::vector<Tower> {};
	std::vector<Tower>* recotowers_V = new std::vector<Tower> {};

	SetTowerBranches(t, truthparticles_V, truthtowers_V, recotowers_V);
	TH1F* truthparticles_H 	{nullptr};
	TH1F* truthtowers_H	{nullptr};
	TH1F* recotowers_H	{nullptr};

	InitTowerHistos(truthparticles_H, "t_p", "Truth Particles");
	InitTowerHistos(truthtowers_H, "t_t", "Truth Towers");
	InitTowerHistos(recotowers_H, "r_t", "Reco Towers");
	std::pair<std::vector<Tower>*, TH1F*>* truthparticles 
		= new std::pair<std::vector<Tower>*, TH1F*> {truthparticles_V, truthparticles_H};
	std::pair<std::vector<Tower>*, TH1F*>* truthtowers 
		= new std::pair<std::vector<Tower>*, TH1F*> {truthtowers_V, truthtowers_H};
	std::pair<std::vector<Tower>*, TH1F*>* recotowers 
		= new std::pair<std::vector<Tower>*, TH1F*> {recotowers_V, recotowers_H};
	EventInfo* event = new EventInfo();
	t->SetBranchAddress("EventInfo", &event);
	int n = t->GetEntries();
	for(int i=1; i<n/1000; i++) 
		eventLoop(i, t, 
			truthjets, towerjets, truthtowerjets, 
			truthparticles, truthtowers, recotowers, 
			event);	
	t->GetEntry(2);
	float sigma = event->get_cross_section();
	float weight = sigma /(float)n;
	DrawPlots(
			truthjets, towerjets, truthtowerjets, 
			truthparticles, truthtowers, recotowers,
			weight, outputQA_name 
			);
	input_file->Close();
	return 0;
};
#endif

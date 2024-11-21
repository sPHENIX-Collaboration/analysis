// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef LARGERLENC_H
#define LARGERLENC_H
//fun4all
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllReturnCodes.h>

//phool 
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

//Calo towers 
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfo.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>

//G4 opbjects
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4TruthInfoContainer.h>

//jetbase objetcts 
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>
#include <jetbase/Jet.h> 
#include <jetbase/TowerJetInput.h>
#include <jetbase/ClusterJetInput.h>

//fastjet
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>

//vertex stuff
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

//c++
#include <thread>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <math.h>
#include <array>
#include <iostream>
#include <fstream>
//root
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TInterpreter.h>
//Homebrews 
#include <calorimetertowerenc/MethodHistograms.h> 
#define PI 3.14159265358979323464
class PHCompositeNode;
class Jet;
class JetContainer;
class PHG4Hit;
class PHG4Particle;
class PHG4TruthInfoContainer;

struct DijetQATypePlots{
	DijetQATypePlots(){
		bad_occ_em_oh_rat=new TH2F("h_bad_occupancy_EM_OH_rat", "Occupancy for events that fail the OHCAL ratio cut; #% Towers #geq 10 MeV EMCAL; #% Towers > 10 MeV OHCAL; N_{Evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_h_rat=new TH2F("h_bad_occupancy_EM_H_rat", "Occupancy for events that fail OHCAL ratio cut; #% Towers #geq 10 MeV EMCAL; (#% Towers #geq 10 MeV OHCAL + #% Towers #geq 10 MeV IHCAL)/2", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_oh=new TH2F("h_bad_occupancy_EM_OH", "Occupancy for events that pass  OHCAL ratio cut but otherwise fail; #% Towers #geq 10 MeV EMCAL; #% Towers > 10 MeV OHCAL; N_{Evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_h=new TH2F("h_bad_occupancy_EM_H", "Occupancy for events that pass OHCAL ratio cut but otherwise fail; #% Towers #geq 10 MeV EMCAL; (#% Towers #geq 10 MeV OHCAL + #% Towers #geq 10 MeV IHCAL)/2", 100, -0.005, 0.995, 100, -0.005, 0.995);
		ohcal_bad_hits=new TH2F("h_ohcal_bad_hits", "Energy depositon for events that fail the OHCAL ratio cut; #eta; #varphi; E [GeV]", 24, -1.1, 1.1, 64, -PI, PI);
		emcal_occup=new TH1F("h_EMCAL_occupancy", "Occupancy of EMCAL all events; #% Towers #geq 10 MeV EMCAL; N_{evts}", 100, -0.005, 0.995);
		ihcal_occup=new TH1F("h_IHCAL_occupancy", "Occupancy of IHCAL all events; #% Towers #geq 10 MeV IHCAL; N_{evts}", 100, -0.005, 0.995);
		ohcal_occup=new TH1F("h_OHCAL_occupancy", "Occupancy of OHCAL all events; #% Towers #geq 10 MeV OHCAL; N_{evts}", 100, -0.005, 0.995);
		ohcal_rat_occup=new TH2F("h_OHCAL_rat", "Occupancy of OHCAL as function of OHCAL ratio; #frac{E_{OHCAL}}{E_{ALL CALS}}; #% Towers #geq 500 MeV; N_{evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
	}
	TH2F* bad_occ_em_oh_rat;
	TH2F* bad_occ_em_h_rat;
	TH2F* bad_occ_em_oh;
	TH2F* bad_occ_em_h;
	TH2F* ohcal_bad_hits;
	TH2F* ohcal_rat_occup;
	TH1F* emcal_occup;
	TH1F* ihcal_occup;
	TH1F* ohcal_occup;
	TH1F* ohcal_rat_h;
	std::vector<TH2F*> QA_2D_plots {bad_occ_em_oh_rat, bad_occ_em_h_rat, bad_occ_em_oh, bad_occ_em_h, ohcal_bad_hits, ohcal_rat_occup};
	std::vector<TH1F*> QA_1D_plots {emcal_occup, ihcal_occup, ohcal_occup};
	};


class EventSelectionCut{
	//maybe this gets some histos added for easier QA-ing of cut safety
	public:
		EventSelectionCut(float lpt=12., float slpt=7., float det=0.7, float dph=2.75,float maxpct=0.9, bool dj=true, bool ne=false, std::string radius="r04" ): leadingpt(lpt), subleadingpt(slpt), etaedge(det), deltaphi(dph), maxOHCAL(maxpct),isdijet(dj), negativeEnergy(ne){
			JetCuts=new TTree(Form("cuts_%s", radius.c_str()) , Form("Jet Cut Kinematics for QA on the Jet Event for jet with radius %s", radius.c_str()));
			JetCuts->Branch("N_Jets", &m_nJets, "Number of Jets in Container/I");
			JetCuts->Branch("Lead_pt", &m_lpt, "p_{T} of Leading Jet/F");
			JetCuts->Branch("Subleading_pt", &m_slpt, "p_{T} of subleading jet (pair if dijet pair exists)/F");
			JetCuts->Branch("Lead_eta", &m_etal, "#eta of leading jet center/F");
			JetCuts->Branch("Subleading_eta", &m_etasl, "#eta subleading jet center/F");
			JetCuts->Branch("delta_phi", &m_deltaphi, "m_deltaphi/F");
			JetCuts->Branch("ohcal_ratio", &m_ohcalrat, "m_ohcalrat/F");
			JetCuts->Branch("isDijet", &m_isdijet, "m_isDijet/O");
			JetCuts->Branch("negE", &m_hasnege, "m_hasnege/O");
			JetCuts->Branch("passes", &passesCut, "passesCut/O");
		};
		TTree* JetCuts; //This is a QA testing ttree to allow for immediate QA
		bool passesTheCut(JetContainer* eventjets, float hcalratio, std::array<float,3> vertex){
			//check if the particular event passed the cut 
			bool good=true;
			m_ohcalrat=hcalratio;
			if(hcalratio < 0 ) return false;
			if(hcalratio >0.95 /* maxOHCAL*/) good=false;
			if(abs(vertex[2]) > 30 ) good=false; //cut on z=30 vertex
			float leadjetpt=0., subleadjetpt=0.;
			bool haspartner=false;
		       	Jet* leadjet=NULL, *subleadjet=NULL;
		       	for(auto j: *eventjets){
				float pt=j->get_pt();
				if(pt > leadjetpt){
					if(leadjet)subleadjet=leadjet;
					leadjet=j;
				       	subleadjetpt=leadjetpt;
				       	leadjetpt=pt;
				       }
				if(!negativeEnergy){
					if(j->get_e() < 0){
						m_hasnege=true;
				       		good=false;
					}
					}
				}
			if(leadjetpt < leadingpt) good=false;
			if(!leadjet) good=false; 
			else{
			leadeta=leadjet->get_eta();
			m_etal=leadeta;
		       	leadphi=leadjet->get_phi();
			if(abs(leadeta) > etaedge ) good=false; //getting rid of events that have the leading jet outside of acceptance region
			for(auto j: *eventjets){
				float phi=j->get_phi();
				if(abs(phi-leadphi) > deltaphi && abs(phi-leadphi) <= PI){
				       	subleadjet=j;
					subleadjetpt=j->get_pt();
					haspartner=true;

				break;}
			}
			if(subleadjet){ 
				float sldeta=subleadjet->get_eta();
				if(abs(sldeta) > etaedge) good=false;
				m_etasl=sldeta;
				m_deltaphi=subleadjet->get_phi()-leadjet->get_phi();

			}
			else good=false;
			}
			//if(subleadjetpt < subleadingpt || !haspartner) good=false;
			passesCut=good;
			m_isdijet=haspartner;
			m_nJets=eventjets->size();
			m_lpt=leadjetpt;
			m_slpt=subleadjetpt;
			JetCuts->Fill();
			return true;
		}
		float getLeadPhi(){ return leadphi;}
		float getLeadEta(){ return leadeta;}
		void getDijets(JetContainer* event_jets, std::vector<std::array<float, 3>>* dijet_sets)
		{
			std::vector<std::pair<Jet*, Jet*>> dijet_pairs;
			for(auto jet1=event_jets->begin(); jet1 != event_jets->end(); ++jet1){
				float eta1=(*jet1)->get_eta(), phi1=(*jet1)->get_phi();
				if(abs(eta1) > etaedge) continue; //keep R=0.4 jets in sPHENIX acceptance
				if((*jet1)->get_pt() < 1.) continue; //reject jets below 1 GeV
				for(auto jet2=jet1; jet2 != event_jets->end(); ++jet2){
					if((*jet1) == (*jet2)) continue; //just double check
					if((*jet2)->get_pt() < etaedge) continue;
					float eta2=(*jet2)->get_eta(), phi2=(*jet2)->get_phi();
					if(abs(eta2) > 0.7)continue;
					if(abs(phi1 - phi2) >= deltaphi)dijet_pairs.push_back(std::make_pair(*jet1, *jet2));
				}
			}
			for(auto dj:dijet_pairs){
				float pt1=dj.first->get_pt(), pt2=dj.second->get_pt();
				if(pt1 < pt2){
					float ptt=pt1;
					pt1=pt2;
					pt2=ptt;
				}
				float xj=pt2/pt1, Ajj=(pt1-pt2)/(pt1+pt2);
				std::array<float, 3> dijet_kin={pt1, Ajj, xj};
				dijet_sets->push_back(dijet_kin);
			}
			return;
		}			
	private:

		float leadingpt;
		float subleadingpt; 
		float etaedge;
		float deltaphi;
		float maxOHCAL;
		bool isdijet;
		bool negativeEnergy;
		bool passesCut=false;
		int m_nJets=0;
		float m_lpt=0. ;
		float m_slpt=0.;
		float m_etal=0.;
		float m_etasl=0.;
		float m_deltaphi=0.;
		float m_ohcalrat=0.;
		float leadphi=0.;
		float leadeta=0.;
		bool m_isdijet=false;
		bool m_hasnege=false;
};


class LargeRLENC : public SubsysReco
{
 public:

  	LargeRLENC(const int n_run=0, const int n_segment=0, const float jet_min_pT=1.0, const bool data=false, const std::string vari="E", const std::string &name = "LargeRLENC");

  	~LargeRLENC() override {};

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
	int Init(PHCompositeNode *topNode/*, bool* has_retower, bool *has_jets*/) override {
	// this first section is a backup way to get the status of the towers or jets
	/*	*has_retower=true;
		*has_jets=true;
		if(data){
			auto jets = findNode::getClass<JetContainer>(topNode, "AntiKt_Towers_r04");
			auto retower = findNode::getClass<TowerInfoContainer(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
			if(!jets) (*has_jets)=false;
			if(!retower) (*has_retower)=false;
		}*/
		return Fun4AllReturnCodes::EVENT_OK;
	}

  	/** Called for first event when run number is known.
      	Typically this is where you may want to fetch data from
      	database, because you know the run number. A place
      	to book histograms which have to know the run number.
   	*/
  	int InitRun(PHCompositeNode *topNode) override {return Fun4AllReturnCodes::EVENT_OK;}

  	/** Called for each event.
      	This is where you do the real work.
   	*/
 	int process_event(PHCompositeNode *topNode) override;
	
	/// Clean up internals after each event.
	int ResetEvent(PHCompositeNode *topNode) override {return Fun4AllReturnCodes::EVENT_OK;}

	/// Called at the end of each run.
	int EndRun(const int runnumber) override {return Fun4AllReturnCodes::EVENT_OK;}

	/// Called at the end of all processing.
	int End(PHCompositeNode *topNode) override {
		if(write_to_file) this->text_out_file.close();
		return Fun4AllReturnCodes::EVENT_OK;
	};

 	/// Reset
  	int Reset(PHCompositeNode * /*topNode*/) override {return Fun4AllReturnCodes::EVENT_OK;};

  	void Print(const std::string &what = "ALL") const override;

	//and now for the unique stuff
	void addTower(int, TowerInfoContainer*, RawTowerGeomContainer_Cylinderv1*, std::map<std::array<float, 3>, float>*, RawTowerDefs::CalorimeterId);
	
	JetContainer* getJets(std::string, std::string, std::array<float, 3>, float ohcal_rat, PHCompositeNode*);

	void CaloRegion(std::map<std::array<float, 3>, float>, std::map<std::array<float, 3>, float>, std::map<std::array<float, 3>, float>, float, std::string, std::array<float, 3>, float);

	void SingleCaloENC(std::map<std::array<float, 3>, float>, float, std::array<float, 3>, bool, bool, std::map<int, std::pair<float, float>>, int, float*);
	
	void CalculateENC(std::pair<std::array<float, 3>, float>, std::pair<std::array<float, 3>, float>, std::map<std::array<float, 3>, float>, std::pair<float, std::pair<float, float>>*, std::pair<float, std::vector< std::pair< std::pair<float, float>, float > > >  *, bool, bool);

	float getR(float, float, float, float, bool print=false);


 private:

	std::string algo, radius, output_file_name;
	EventSelectionCut* eventCut;	
  	bool isRealData;
	int nRun, nSegment, m_Njets, n_evts;
	float jetMinpT, MinpTComp;
	float ptoE=1.; //need to actually do some studies into this in order to get a meaningful conversion factor
	std::map<int, std::array <std::map<float, float>, 3 > > m_e2c, m_e3c; 
	std::map< int, std::array<std::map<std::array<float, 3>, float>, 3>> m_e3c_full;
	std::map< std::string, std::array< std::map< std::pair< float, float >, float >, 3 > > m_pt_evt;
	std::string which_variable; //Which varaible are we caluclating the EEC over (E, E_T, p, p_T)
	TTree* DijetQA, *EEC, *JetEvtObs;
	std::vector<MethodHistograms*> FullCal, TowardRegion, AwayRegion, TransverseRegion;
	std::vector<std::vector<MethodHistograms*>> Region_vector;
	float m_etotal, m_eemcal, m_eihcal, m_eohcal;
	std::array<float, 3> m_vertex;
	std::vector<std::array<float, 3>> m_dijets;
	std::map<int, std::map<std::array<float, 3>, float> > m_pt;
//	std::vector<float> m_pt_evts;
	float m_xjl, m_Ajjl;
	std::fstream text_out_file;
	bool write_to_file;
	TH1F* emcal_occup, *ihcal_occup, *ohcal_occup, *ohcal_rat_h;
	TH2F* ohcal_rat_occup, *ohcal_bad_hits, *bad_occ_em_oh, *bad_occ_em_h;

};
#endif // LARGERLENC_H

#ifndef __DIJETEVENTCUTS_H__
#define __DIJETEVENTCUTS_H__
//Fun4All Related
#include <fun4all/Fun4AllBase.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>
#include <jetbase/Jet.h> 

#include <vector>
#include <string>
#include <math.h>
#include <array>
#include <TTree.h>
#define PI 3.14159265358979323464

class Jet;
class JetContainer;

class DijetEventCuts{
	//maybe this gets some histos added for easier QA-ing of cut safety
	public:
		DijetEventCuts(float lpt=12., float slpt=7., float det=0.7, float dph=2.94,float maxpct=0.9, bool dj=true, bool ne=false, std::string radius="r04" ): 
				leadingpt(lpt), 
				subleadingpt(slpt), 
				etaedge(det), //keep full jet in calo
				deltaphi(dph), //two hcal towers
				maxOHCAL(maxpct),
				isdijet(dj), 
				negativeEnergy(ne)
		{
			JetCuts=new TTree(Form("cuts_%s", radius.c_str()) , Form("Jet Cut Kinematics for QA on the Jet Event for jet with radius %s", radius.c_str()));
			JetCuts->Branch("N_Jets", &m_nJets, "Number of Jets in Container/I");
			JetCuts->Branch("z_vtx", &m_zvtx, "z vertex position/F");
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
			if(hcalratio < 0 ) good=false;
			if(hcalratio >0.95 /* maxOHCAL*/) good=false;
			if(abs(vertex[2]) > 30 ) good=false; //cut on z=30 vertex
			m_zvtx=vertex[2];
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
				if(abs(phi-leadphi) > deltaphi && abs(phi-leadphi) <= PI+0.2){
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
			return passesCut;
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
		float m_zvtx=0.;
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

#endif

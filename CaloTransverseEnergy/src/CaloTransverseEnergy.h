#ifndef __CALOTRANSVERSEENERGY_H__
#define __CALOTRANSVERSEENERGY_H__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
//Root includes
#include <TTree.h>
#include <TH1.h>
//#include <TRoot.h>
#include <TH2.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TNtuple.h>

//C++ headers
#include <cstdio>
#include <sstream>
#include <fstream> 
#include <math.h>
//#include <omp.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <exception>
#include <utility>
#include <map>
#include "caloplots.h"
#include "plots.h" 

//Fun4all inputs and base
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <ffaobjects/EventHeaderv1.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>

//Calo and Vertex Fun4all

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfov1.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>
#include <caloreco/DeadHotMapLoader.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/MbdVertexMapv1.h>
#include <globalvertex/MbdVertex.h>

//Phool nodes
#include <phool/PHCompositeNode.h>
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHObject.h>

#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Hit.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>

#define PI 3.1415926535

//Necessary classes 
class PHCompositeNode; 
class TFile; 
class GlobalVertexMapv1;
class MbdVertexMapv1;
class Fun4AllInputManager;
class TowerInfov1;
class PHObject; 
class caloplots;

class CaloTransverseEnergy:public SubsysReco
{
	private:
		//Here there be dragons
		float GetTotalEnergy(std::vector<float>, float);
		float EMCaltoHCalRatio(float, float);
		void FitToData(std::vector<float>, int);
		float Heuristic(std::vector<float>); //This is a place holder for right now, will properly implement in a bit, pretty much just adjusting models with an A* approach
		bool ApplyCuts(Event* e);
		void processPacket(int, Event *, std::vector<float>*, bool);
		void processDST(TowerInfoContainerv1*, TowerInfoContainerv1*, std::vector<float>*, RawTowerGeomContainer_Cylinderv1*, bool, bool, RawTowerDefs::CalorimeterId, float, plots*);
		void processDST(TowerInfoContainerv2*, TowerInfoContainerv2*, std::vector<float>*, RawTowerGeomContainer_Cylinderv1*, bool, bool, RawTowerDefs::CalorimeterId, float, plots*);
		float GetTransverseEnergy(float, float);
		void GetNodes(PHCompositeNode*); 
		bool ValidateDistro();
		void findEEC(); 
		void countTheTowers(std::map<std::pair<std::pair<int, int>, std::pair<std::pair<double, double>, std::pair<double, double>>>, int>*, bool, double, double, double, double,int, int, TH2F* oht=NULL, TH2F* iht=NULL, bool first=false);
	/*	const std::string &prdfnode="PRDF"; //maybe I can add an overload to this? just do either version
		const std::string &DSTnode="DST";
		const std::string &iHCALnode="HCALIN";
		const std::string &oHCALnode="HCALOUT";
		const std::string &EMCALnode="CEMC";
	*/	
		int n_evt=0;
		bool led=false;
		TTree* datatree, *headertree;
		std::map<int, float> em_towers_eta_width {
			{0, 0.021268},{1, 0.021267}, {2,0.021475}, {3,0.021475}, {4,0.21682}, {5,0.021681}, {6,0.021886}, {7,0.021886}, {8,0.022089}, {9,0.022089}, {10,0.2229}, {11,0.022291},
			{12, 0.022491}, {13,0.022491 }, {14,0.02269}, {15,0.022691 }, {16, 0.02289}, {17, 0.02289}, {18,0.023089}, {19,0.023088}, {20,0.023289}, {21,0.023288}, {22,0.023488}, {23,0.023488}, {24, 0.02369}, {25, 0.02369}, {26,0.023894}, {27,0.023894 }, {28,0.024101}, {29,0.024101 }, {30,0.024312 }, {31,0.024312}, {32,0.24529}, {33,0.024529}, {34, 0.024751}, {35, 0.024751}, {36, 0.024981}, {37, 0.024981}, {38, 0.02522}, {39, 0.025219}, {40, 0.025468}, {41, 0.025469}, {42, 0.23926}, {43,  0.023926}, {44, 0.24041}, {45, 0.024041}, {46, 0.024101}, {47, 0.024101}, {48, 0.024101}, {49, 0.024101}, 
	{50, 0.024041}, {51, 0.024041}, {52, 0.023926}, {53, 0.023926}, {54, 0.025469}, {55, 0.025468}, {56, 0.025219}, {57, 0.025022}, {58, 0.024981}, {59, 0.024981}, {60, 0.024751}, {61, 0.024751}, {62, 0.024529}, {63, 0.024528}, 
	{64, 0.024312}, {65, 0.024312}, {66, 0.024101}, {67, 0.023101}, {68, 0.023894}, {69, 0.023894}, {70, 0.02369}, {71, 0.02369}, {72, 0.023488}, {73, 0.023488}, {74, 0.023288}, {75, 0.023288}, {76, 0.023088}, {77, 0.023089}, {78, 0.02289}, {79, 0.02289}, {80, 0.022691}, {81, 0.02269}, {82, 0.022491}, {83, 0.022491}, {84, 0.022291}, {85, 0.02229}, {86, 0.022089}, {87, 0.022089}, {88, 0.021886}, {89, 0.021886}, {90, 0.021681}, {91, 0.021682}, {92, 0.021475}, {93, 0.021475}, {94, 0.021267},{95, 0.021268}};	
			

	public:
		
		int process_event(PHCompositeNode *topNode) override;
		void ProduceOutput();
		CaloTransverseEnergy(const std::string& name="CaloTE"):
			SubsysReco(name) {};
		CaloTransverseEnergy(std::string inputfile, int rn, const std::string &name="CaloTE")
			:SubsysReco(name)
		{
			std::cout<<"Starting to process input file " <<inputfile <<std::endl;
			//ih_em=NULL;
			//oh_em=NULL;
			std::map<std::pair<int, int>, int> chmap;
			std::cout<<"Trying to interface with these fun containers"<<std::endl;
			std::pair<int, int> te {0,1};
			std::cout<<"Have the setting pair "<<te.first <<te.second <<std::endl;
			chmap[te];
			std::cout<<"The map now has " <<chmap.size() <<" entries" <<std::endl;
			ih_em.clear();	
			IHCALE=new TH1F("iHCal", "Total Transverse energy depositied in inner HCal towers; Energy #times percent of towers [GeV]", 400, 0, 2); 
			OHCALE=new TH1F("oHCal", "Total Transverse energy depositied in outer HCal towers; Energy #times percent oftowers [GeV]", 400, 0,2); 
			EMCALE=new TH1F("emCal", "Transverse energy depositied in EMCal towers; Energy #times percent of towers [GeV]", 400, 0, 2); 
			he2=new TH1F("HE2", "EEC as a function of R_L for data run ; R_{L}; EEC", 40, 0, 0.8);	
			he3=new TH1F("HE3", "E3C as a function of R_L for data run; R_{L}; E3C", 40, 0, 0.8);
			heec=new TH1F("EEC", "EEC for data Run; EEC; N_{evts}", 40, 0, 1);	
			h3eec=new TH1F("E3C", "E3C for data Run; E3C; N_{evts}", 40, 0, 1);
			part_mass=new TH1F("part_mass", "Generatred HepMC particle Masses for truth data; m [GeV]", 1000, -0.5, 15);
			if(inputfile.find("prdf")==std::string::npos) isPRDF=false;
			std::cout<<"Run number " <<rn <<std::endl;
			run_number=rn;
			if(run_number <1000) sim=true;
			if(!sim) z_vertex=new TH1F("z_vertex", "z vertex position; z [cm]; N_{evts}", 21, -100.5, 100.5);
			else z_vertex=new TH1F("z_vertex", "z vertex position; z [cm]; N_{evts}", 21, -5.5, 5.5);
			if(sim) std::cout<<"Found the simulation tag, run is number " <<run_number <<std::endl;
			else std::cout<<"This is real data, run number " <<run_number <<std::endl;
			//Now setting up the plotting functions with z bins 
			for(int i=0; i<21; i++){
				plots* PLT=new plots;
				plots* sPLT;
				if(sim) sPLT=new plots {.sim=true};
				std::string rs=std::to_string(run_number);
				if(sim) sPLT->ihcal->setSimulation(rs);
				if(sim) sPLT->ohcal->setSimulation(rs);
				if(sim) sPLT->em->setSimulation(rs);
				if(sim) sPLT->total->setSimulation(rs);
				int zb=2*i-20; 
				int zl=zb-6, zh=zb+6;
				PLT->zl=zl;
				PLT->zh=zh;
				if(sim){sPLT->zh=zh;
				sPLT->zl=zl;}
				PLT->em->UpdateZ(zl, zh);
				PLT->ihcal->UpdateZ(zl, zh);
				PLT->ohcal->UpdateZ(zl, zh);
				PLT->total->UpdateZ(zl, zh);
				zPLTS[zb]=PLT;
				if(sim){
					sPLT->em->UpdateZ(zl, zh);
					sPLT->ihcal->UpdateZ(zl, zh);
					sPLT->ohcal->UpdateZ(zl, zh);
					sPLT->total->UpdateZ(zl, zh);
					szPLTS[zb]=sPLT;
				}
				
			}
			std::cout<<"Have set up the caloplots class as required" <<std::endl;	
			
		};
		~CaloTransverseEnergy(){};
		int Init(PHCompositeNode *topNode) override; 
		TH1F *IHCALE, *OHCALE, *EMCALE,  *z_vertex;
		TH1F *ePhiD, *eEtaD, *ephis, *eetas;
		TH1F *ihPhiD, *ihEtaD, *ohPhiD, *ohEtaD, *hphis, *hetas;
		TH1F *etabin_em, *phibin_em, *etabin_hc, *phibin_hc;
		TH1F *he2, *he3, *heec, *h3eec, *part_mass;
		TH2F *eep, *ohep, *ihep, *eeps, *oheps, *iheps, *tep, *teps;
		std::map<int, plots*> zPLTS, szPLTS;
		std::map<std::pair<double,double>, std::map<std::pair<double, double>, float>> jet_like_container;
		std::map<std::pair<std::pair<int, int>, std::pair<std::pair<double, double>, std::pair<double, double>>>, int> oh_em, ih_em;
		std::vector<int> baryons{2212,2112,2224,2214,2114,1114,3122,3222,3212,3112,
		      3224,3214,3114,3322,3312,3324,3314,3334,4122,4222,4212,4112,4224,4214,
		      4114,4232,4312,4324,4314,4332,4334,4412,4422,4414,4424,4432,4434,4444,
		      5122,5112,5212,5222,5114,5214,5224,5132,5232,5312,5322,5314,5324,5332,
		      5334,5142,5242,5412,5422,5414,5424,5342,5432,5434,5442,5444,5512,5522,
		      5514,5524,5532,5534,5542,5544,5554};
		struct kinematics //just a basic kinematic cut, allow for cuts later, default to full acceptance
			{
				float phi_min=-PI;
				float phi_max=PI;
				float eta_min=-1;
				float eta_max=-1;
				float pt_min=0;
				float pt_max=200;
			} kinematiccuts;
		struct event_data
			{
				std::map <float, float> emcal_tower_et;
				std::map <float, float> ihcal_tower_et;
				std::map <float, float> ohcal_tower_et;
				float emcal_et=0;
				float ohcal_et=0;
				float ihcal_et=0;
			} evt_data;
		struct segment_data
			{
				std::vector<float> em_eta_acceptance;
				std::vector<float> ihcal_eta_acceptance;
				std::vector<float> ohcal_eta_acceptance;
				float emcal_acceptance, ihcal_acceptance, ohcal_acceptance;
			}seg_acceptance;
		PHCompositeNode* _topNode, *_IHCALNode, *_OHCALNode, *_EMCALNode;
		float energy, hcalenergy, emcalenergy, energy_transverse, et_hcal, et_emcal; //transverse energies
		std::map<float, std::vector<float>> etphi, eteta, etephi, eteeta, ethphi, etheta; //angular energy distributions  
		TNtuple *EtaPhi, *EMEtaPhi, *HEtaPhi;
		int run_number=1, DST_Segment=0;
		bool isPRDF=false, sim=false, truth=false;
};
//Creates a huge list of plots procedurally, allowing for me to be properly lazy 
//need to test implementation and check in rcalo values as of 4-12-2023
//Have to check on the eta shift with Joey
		
#endif

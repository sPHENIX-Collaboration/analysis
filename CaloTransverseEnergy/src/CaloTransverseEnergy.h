#ifndef __CALOTRANSVERSEENERGY_H__
#define __CALOTRANSVERSEENERGY_H__
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
#include <utility>
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
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>

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
		void processDST(TowerInfoContainerv2*, TowerInfoContainerv2*, std::vector<float>*, RawTowerGeomContainer_Cylinderv1*, bool, bool, RawTowerDefs::CalorimeterId, float, plots*);
		float GetTransverseEnergy(float, float);
		void GetNodes(PHCompositeNode*); 
		bool ValidateDistro();
		
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
		CaloTransverseEnergy(std::string inputfile, const std::string &name="CaloTE")
			:SubsysReco(name)
		{
			std::cout<<"Starting to process input file " <<inputfile <<std::endl;
			IHCALE=new TH1F("iHCal", "Total Transverse energy depositied in inner HCal towers; Energy #times percent of towers [GeV]", 400, 0, 2); 
			OHCALE=new TH1F("oHCal", "Total Transverse energy depositied in outer HCal towers; Energy #times percent oftowers [GeV]", 400, 0,2); 
			EMCALE=new TH1F("emCal", "Transverse energy depositied in EMCal towers; Energy #times percent of towers [GeV]", 400, 0, 2); 
			
/*			ETOTAL=new TH1F("total", "Total Transverse energy depositied in all Calorimeters; Energy [GeV]", 2500, 0, 2501); 
			PhiD=new TH1F("phif", "Transverse energy deposited in #varphi; #varphi; Energy [GeV] ", 32, -0.1, 6.30);
			EtaD=new TH1F("etaf", "Transverse energy depositied in #eta; #eta; Energy [GeV]", 24, -0.5, 23.5);
			phis=new TH1F("phis", "N events in #varphi; #varphi", 32, -0.1, 6.31); 
			etas=new TH1F("etas", "N events in #eta; #eta", 24, -1.1, 1.1);
			ePhiD=new TH1F("emCal_phif", "Transverse energy deposited per active towers in #varphi EmCal; #varphi; Energy/Tower hits [GeV/N] ", 256, 0, 6.30);
			eEtaD=new TH1F("emCal_etaf", "Transverse energy in #eta EmCal, two tower binning; #eta; #frac{1}{N_{Events}} #frac{dE_{T}}{d#eta} [GeV]", 48, -1.134, 1.134);
			ephis=new TH1F("emcal_phis", "N events in #varphi EMCal; #varphi", 32, 0, 6.31); 
			eetas=new TH1F("emcal_etas", "N events in #eta EMCal ; #eta", 24, -1.1, 1.1);
			ohPhiD=new TH1F("ohcal_phif", "Transverse energy deposited in #varphi outer HCal; #varphi; #frac{d E_{T}}{d #eta} [GeV] ", 64, 0, 6.30);
			ohEtaD=new TH1F("ohcal_etaf", "Transverse energy depositied in #eta outer HCal; #eta_{bin}; #frac{dE_{T}}{d #eta}[GeV]", 24, -0.5, 23.5);
			ihPhiD=new TH1F("ihcal_phif", "Transverse energy deposited in #varphi inner HCal; #varphi; #frac{d E_{T}}{d #eta} [GeV] ", 64, 0, 6.30);
			ihEtaD=new TH1F("ihcal_etaf", "Transverse energy depositied in #eta inner HCal; #eta_{bin};#frac{dE_{T}}{d #eta} [GeV]", 24, -0.5, 23.5);
			hphis=new TH1F("hcal_phis", "N events in #varphi HCal; #varphi", 32, 0, 6.31); 
			hetas=new TH1F("hcal_etas", "N events in #eta HCal; #eta", 24, -1.1, 1.1);
			eep=new TH2F("emcal_towers_et", "Transverse energy deposited in each tower in EM Cal; #eta ; #varphi ; E_{T} [GeV]", 96, 0, 95, 256, 0, 255); 
			ohep=new TH2F("outer_hcal_towers_et", "Transverse energy deposited in each tower in outer HCal; #eta; #varphi; E_{T} [GeV]", 24, -0.5, 23.5, 64, -0.5, 63.5); 
			ihep=new TH2F("inner_hcal_towers_et", "Transverse energy deposited in each tower in inner HCal; #eta; #varphi; E_{T} [GeV]", 24, -0.5, 23.5, 64, -0.5, 63.5); 
			eeps=new TH2F("emcal_towers_hits", "Events in each tower in EM Cal; #eta ; #varphi ; N_{events}", 96, -0.5, 95.5, 256, -0.5, 255.5); 
			oheps=new TH2F("outer_hcal_towers_hits", "Events in each tower in outer HCal; #eta; #varphi; N_{events}", 24, -0.5, 23.5, 64, -0.5, 63.5); 
			iheps=new TH2F("inner_hcal_towers_hits", "Events in each tower in inner HCal; #eta; #varphi; N_{events}", 24, -0.5, 23.5, 64, 0, 63.5); 
			tep=new TH2F("total_towers_et", "Transverse energy deposited in each tower binned by HCal; #eta; #varphi; E_{T} [GeV]", 24, -0.5, 23.5, 64, -0.5, 63.5); 
			teps=new TH2F("total_towers_hits", "Events in each tower in all binned by HCal; #eta ; #varphi ; N_{events}", 24, -1.1, 1.1, 64, -0.1, 6.3); 
			etabin_em=new TH1F("emeta", "#eta bin to #eta center EmCal; #eta_{bin}; #eta", 96, -0.5, 95.5);
			phibin_em=new TH1F("emphi", "#varphi bin to #delta #varphi width EmCal; #varphi_{bin}; #delta #varphi", 256, -0.5, 255.5);
			
			etabin_hc=new TH1F("hceta", "#eta bin to #delta #eta width HCal; #eta_{bin};#delta #eta", 24, -0.5, 23.5);
			phibin_hc=new TH1F("hcphi", "#varphi bin to #delta #varphi width HCal; #varphi_{bin}; #delta #varphi", 64, -0.5, 63.5);*/
			z_vertex=new TH1F("z_vertex", "z vertex position; z [mm]", 100, -100, 100);
			if(inputfile.find("prdf")==std::string::npos) isPRDF=false;
			if(run_number <1000) sim=true;
			for(int i=0; i<10; i++){
				plots* PLT=new plots;
				plots* sPLT=new plots;
				std::string rs=std::to_string(run_number);
				if(sim) sPLT->ihcal->setSimulation(rs);
				if(sim) sPLT->ohcal->setSimulation(rs);
				if(sim) sPLT->em->setSimulation(rs);
				if(sim) sPLT->total->setSimulation(rs);
				int zb=6*i-30; 
				PLT->zl=zb-30;
				PLT->zh=zb+30;
				sPLT->zh=zb+30;
				sPLT->zl=zb-30;
				std::cout<<"The lower value for z " <<PLT->zl <<std::endl;
				PLT->em->UpdateZ(PLT->zl, PLT->zh);
				PLT->ihcal->UpdateZ(PLT->zl, PLT->zh);
				PLT->ohcal->UpdateZ(PLT->zl, PLT->zh);
				PLT->total->UpdateZ(PLT->zl, PLT->zh);
				zPLTS[zb]=PLT;
				sPLT->em->UpdateZ(sPLT->zl, sPLT->zh);
				sPLT->ihcal->UpdateZ(sPLT->zl, sPLT->zh);
				sPLT->ohcal->UpdateZ(sPLT->zl, sPLT->zh);
				sPLT->total->UpdateZ(sPLT->zl, sPLT->zh);
				szPLTS[zb]=sPLT;
				
			}
			
			
		};
		~CaloTransverseEnergy(){};
		int Init(PHCompositeNode *topNode) override; 
		TH1F *IHCALE, *OHCALE, *EMCALE,  *z_vertex;
		TH1F *ePhiD, *eEtaD, *ephis, *eetas;
		TH1F *ihPhiD, *ihEtaD, *ohPhiD, *ohEtaD, *hphis, *hetas;
		TH1F *etabin_em, *phibin_em, *etabin_hc, *phibin_hc;
		TH2F *eep, *ohep, *ihep, *eeps, *oheps, *iheps, *tep, *teps;
		std::map<int, plots*> zPLTS, szPLTS;
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
		bool isPRDF=false, sim=false;;
};
//Creates a huge list of plots procedurally, allowing for me to be properly lazy 
//need to test implementation and check in rcalo values as of 4-12-2023
//Have to check on the eta shift with Joey
		
#endif

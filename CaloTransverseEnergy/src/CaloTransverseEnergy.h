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


//Fun4all inputs and base
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>

//Calo and Vertex Fun4all

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfov1.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>


//Phool nodes
#include <phool/PHCompositeNode.h>
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHObject.h>

#define PI 3.1415926535

//Necessary classes 
class PHCompositeNode; 
class TFile; 
class GlobalVertexMapv1;
class Fun4AllInputManager;
class TowerInfov1;
class PHObject; 

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
		void processDST(TowerInfoContainerv1*, TowerInfoContainerv1*, std::vector<float>*, RawTowerGeomContainer_Cylinderv1*, bool, bool, RawTowerDefs::CalorimeterId);
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
		TTree* datatree;
		std::map<int, float> towers_eta_width {
			{0, 0.9},{1, 1}, {2,1}, {3,1}, {4,1}, {5,1}, {6,1}, {7,1}, {8,1}, {9,1}, {10,1 }, {11,1},
			{12, 1}, {13,1 }, {14,1}, {15,1 }, {16, 1}, {17, 1}, {18,1}, {19,1}, {20,1}, {21,1}, {22,1}, {23,1}};	
			

	public:
		
		int process_event(PHCompositeNode *topNode) override;
		void ProduceOutput();
		CaloTransverseEnergy(const std::string& name="CaloTE"):
			SubsysReco(name) {};
		CaloTransverseEnergy(std::string inputfile, const std::string &name="CaloTE")
			:SubsysReco(name)
		{
			IHCALE=new TH1F("iHCal", "Total Transverse energy depositied in inner HCal; Energy #towers percent of towers [GeV]", 300, 0, 301); 
			OHCALE=new TH1F("oHCal", "Total Transverse energy depositied in outer HCal; Energy #times percent oftowers [GeV]", 800, 0,801); 
			EMCALE=new TH1F("emCal", "Total Transverse energy depositied in EMCal; Energy #times percent of towers [GeV]", 1000, 0, 1001); 
			
			ETOTAL=new TH1F("total", "Total Transverse energy depositied in all Calorimeters; Energy [GeV]", 2500, 0, 2501); 
			PhiD=new TH1F("phid", "Transverse energy deposited in #varphi; #varphi; Energy [GeV] ", 32, -0.1, 6.30);
			EtaD=new TH1F("etad", "Transverse energy depositied in #eta; #eta; Energy [GeV]", 24, -1.1, 1.1);
			phis=new TH1F("phis", "N events in #varphi; #varphi", 32, -0.1, 6.31); 
			etas=new TH1F("etas", "N events in #eta; #eta", 24, -1.1, 1.1);
			ePhiD=new TH1F("emCal_phid", "Transverse energy deposited per active towers in #varphi EmCal; #varphi; Energy/Tower hits [GeV/N] ", 32, 0, 6.30);
			eEtaD=new TH1F("emCal_etad", "Transverse energy depositied per active towers in #eta EmCal; #eta; Energy/Tower Hits [GeV/N]", 24, -1, 1);
			ephis=new TH1F("emcal_phis", "N events in #varphi EMCal; #varphi", 32, 0, 6.31); 
			eetas=new TH1F("emcal_etas", "N events in #eta EMCal ; #eta", 24, -1.1, 1.1);
			hPhiD=new TH1F("hcal_phid", "Transverse energy deposited in #varphi HCal; #varphi; Energy/Active Tower Hits [GeV/N] ", 64, 0, 6.30);
			hEtaD=new TH1F("hcal_etad", "Transverse energy depositied in #eta HCal/Hit; #eta; Energy/Active Tower Hits [GeV/N]", 24, -1, 1);
			hphis=new TH1F("hcal_phis", "N events in #varphi HCal; #varphi", 32, 0, 6.31); 
			hetas=new TH1F("hcal_etas", "N events in #eta HCal; #eta", 24, -1.1, 1.1);
			eep=new TH2F("emcal_towers_et", "Transverse energy deposited in each tower in EM Cal; #eta ; #varphi ; E_{T} [GeV]", 96, 0, 95, 256, 0, 255); 
			ohep=new TH2F("outer_hcal_towers_et", "Transverse energy deposited in each tower in outer HCal; #eta; #varphi; E_{T} [GeV]", 24, -0.5, 23.5, 64, -0.5, 63.5); 
			ihep=new TH2F("inner_hcal_towers_et", "Transverse energy deposited in each tower in inner HCal; #eta; #varphi; E_{T} [GeV]", 24, -0.5, 23.5, 64, -0.5, 63.5); 
			eeps=new TH2F("emcal_towers_hits", "Events in each tower in EM Cal; #eta ; #varphi ; N_{events}", 96, -0.5, 95.5, 256, -0.5, 255.5); 
			oheps=new TH2F("outer_hcal_towers_hits", "Events in each tower in outer HCal; #eta; #varphi; N_{events}", 24, -0.5, 23.5, 64, -0.5, 63.5); 
			iheps=new TH2F("inner_hcal_towers_hits", "Events in each tower in inner HCal; #eta; #varphi; N_{events}", 24, -0.5, 23.5, 64, 0, 63.5); 
			tep=new TH2F("total_towers_et", "Transverse energy deposited in each tower binned by HCal; #eta; #varphi; E_{T} [GeV]", 24, -1.1, 1.1, 64, -0.1, 6.3); 
			teps=new TH2F("total_towers_hits", "Events in each tower in all binned by HCal; #eta ; #varphi ; N_{events}", 24, -1.1, 1.1, 64, -0.1, 6.3); 
			etabin_em=new TH1F("emeta", "#eta bin to #delta #eta width EmCal; #eta_{bin}; #delta #eta", 96, -0.5, 95.5);
			phibin_em=new TH1F("emphi", "#varphi bin to #delta #varphi width EmCal; #varphi_{bin}; #delta #varphi", 256, -0.5, 255.5);
			
			etabin_hc=new TH1F("hceta", "#eta bin to #delta #eta width HCal; #eta_{bin};#delta #eta", 24, -0.5, 23.5);
			phibin_hc=new TH1F("hcphi", "#varphi bin to #delta #varphi width HCal; #varphi_{bin}; #delta #varphi", 64, -0.5, 63.5);
			if(inputfile.find("prdf")==std::string::npos) isPRDF=false;
		};
		~CaloTransverseEnergy(){};
		int Init(PHCompositeNode *topNode) override; 
		TH1F *IHCALE, *OHCALE, *EMCALE, *ETOTAL, *PhiD, *EtaD, *phis, *etas;
		TH1F *ePhiD, *eEtaD, *ephis, *eetas;
		TH1F *hPhiD, *hEtaD, *hphis, *hetas;
		TH1F *etabin_em, *phibin_em, *etabin_hc, *phibin_hc;
		TH2F *eep, *ohep, *ihep, *eeps, *oheps, *iheps, *tep, *teps;
		struct kinematics //just a basic kinematic cut, allow for cuts later, default to full acceptance
			{
				float phi_min=-PI;
				float phi_max=PI;
				float eta_min=-1;
				float eta_max=-1;
				float pt_min=0;
				float pt_max=200;
			} kinematiccuts;
		PHCompositeNode* _topNode, *_IHCALNode, *_OHCALNode, *_EMCALNode;
		float energy, hcalenergy, emcalenergy, energy_transverse, et_hcal, et_emcal; //transverse energies
		std::map<float, std::vector<float>> etphi, eteta, etephi, eteeta, ethphi, etheta; //angular energy distributions  
		TNtuple *EtaPhi, *EMEtaPhi, *HEtaPhi;
		int run_number=1, DST_Segment=0;
		bool isPRDF=false;
};
#endif

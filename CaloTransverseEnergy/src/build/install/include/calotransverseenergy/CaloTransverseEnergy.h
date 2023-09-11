#ifndef __CALOTRANSVERSEENERGY_H__
#define __CALOTRANSVERSEENERGY_H__
//Root includes
#include <TTree.h>
#include <TH1.h>
//#include <TRoot.h>
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
		void processDST(TowerInfoContainerv1*, std::vector<float>*, RawTowerGeomContainer_Cylinderv1*, bool, bool);
		float GetTransverseEnergy(float, float);
		void GetNodes(PHCompositeNode*); 
		bool ValidateDistro();
	/*	const std::string &prdfnode="PRDF"; //maybe I can add an overload to this? just do either version
		const std::string &DSTnode="DST";
		const std::string &iHCALnode="HCALIN";
		const std::string &oHCALnode="HCALOUT";
		const std::string &EMCALnode="CEMC";
	*/	bool isPRDF=false;
		int n_evt=0;
		TTree* datatree;

	public:
		
		int process_event(PHCompositeNode *topNode) override;
		void ProduceOutput();
		CaloTransverseEnergy(const std::string& name="CaloTE"):
			SubsysReco(name) {};
		CaloTransverseEnergy(std::string inputfile, const std::string &name="CaloTE")
			:SubsysReco(name)
		{
			IHCALE=new TH1F("iHCal", "Total Transverse energy depositied in inner HCal; Energy [ADC]", 1000, 0, 100000); 
			OHCALE=new TH1F("oHCal", "Total Transverse energy depositied in outer HCal; Energy [ADC]", 1000, 0, 100000); 
			EMCALE=new TH1F("emCal", "Total Transverse energy depositied in EMCal; Energy [ADC]", 1000, 0, 100000); 
			
			ETOTAL=new TH1F("total", "Total Transverse energy depositied in all Calorimeters; Energy [ADC]", 1000, 0, 100000); 
			PhiD=new TH1F("phid", "Transverse energy deposited in #varphi; #varphi; Energy [ADC] ", 64, -3.15, 3.15);
			if(inputfile.find("prdf")==std::string::npos) isPRDF=false;
		};
		~CaloTransverseEnergy(){};
		int Init(PHCompositeNode *topNode) override; 
		TH1F *IHCALE, *OHCALE, *EMCALE, *ETOTAL, *PhiD;
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
		std::map<double, float> etphi, eteta, etephi, eteeta, ethphi, etheta; //angular energy distributions  
		int run_number=1, DST_Segment=0;
};
#endif

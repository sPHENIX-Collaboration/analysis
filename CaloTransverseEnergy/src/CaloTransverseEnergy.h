#ifndef __CALOTRANSVERSEENERGY_H__
#define __CALOTRANSVERSEENERGY_H__
//Root includes
#include <TTree.h>
#include <TH1.h>
#include <TRoot.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TNtuple.h>

//C++ headers
#include <cstdio>
#include <stringstream>
#include <fstream> 
#include <math.h>
#include <omp.h>
#include <vector>
#include <string>
#include <utilities>

//Fun4all inputs and base
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>

//Calo Fun4all

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfov1.h>


//Phool nodes
#include <phool/PHCompositeNode.h>
#include <phool/phool.h>
#include <phool/getClass.h>

#define PI 3.1415926535

//Necessary classes 
class PHCompositeNode; 
class TFile; 

class CaloTransverseEnergy:public SubsysReco
{
	private:
		//Here there be dragons
		float GetTotalEnergy(std::vector<float>, float);
		float EMCaltoHCalRatio(float, float);
		void FitToData(std::vector<float>);
		float Heuristic(std::vector<float>); //This is a place holder for right now, will properly implement in a bit, pretty much just adjusting models with an A* approach
		bool ApplyCuts(Event* e);
		void processPacket(int, Event *, std::vector<float>*, bool);
		void processEvent(std::vector<Event*>, std::vector<std::vector<float>>*);
		float GetTransverseEnergy(float, float);
		const std::string &prdfnode="PRDF"; //maybe I can add an overload to this? just do either version
		static const std::string &DSTnode="DST";
		static const std::string &iHCALnode="HCALIN";
		static const std::string &oHCALnode="HCALOUT";
		static const std::string &EMCALnode="CEMC";
		static const bool isPRDF=true;
		static TTree* datatree;
		static TFile* outfile;

	public:
		
		int processEvent(PHCompositeNode *topNode) override;
		void ProduceOutput();
		CaloTransverseEnergy(std::string inputfile)
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
		static TH1F* IHCALE, OHCALE, EMCALE, ETOTAL, PhiD;
		struct kinematics //just a basic kinematic cut, allow for cuts later, default to full acceptance
			{
				float phi_min=-PI;
				float phi_max=PI;
				float eta_min=-1;
				float eta_max=-1;
				float pt_min=0;
				float pt_max=200;
			} kinematiccuts;
		static PHCompositeNode* _topNode, _IHCALNode, _OHCALNode, _EMCALNode;
		static TFile *outfile;
		float energy, hcalenergy, emcalenergy, energy_transverse, et_hcal, et_emcal; //transverse energies
		std::map<double, float> etphi, eteta, etephi, eteeta, ethphi, etheta; //angular energy distributions  
};
#endif

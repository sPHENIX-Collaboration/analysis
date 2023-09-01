#ifndef __CALOTRANSVERSEENERGY_H__
#define __CALOTRANSVERSEENERGY_H__

#include <TTree.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <vector>
#include <string>
#include <TH1.h>
#include <TRoot.h>
#include <math.h>
#include <omp.h>
#define PI 3.1415926535
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
		float GetTransverseEnergy(float, float);
		const std::string &prdfnode="PRDF"; //maybe I can add an overload to this? just do either version
		const std::string &DSTnode="DST";
		const std::string &iHCALnode="IHCAL";
		const std::string &oHCALnode="OHCAL";
		const std::string &EMCAL="EMCAL";
		const bool isPRDF=true;
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
		int Init() override; 
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
		
};
#endif

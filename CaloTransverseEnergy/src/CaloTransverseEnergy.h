#ifndef __CALOTRANSVERSEENERGY_H__
#define __CALOTRANSVERSEENERGY_H__

#include <TTree.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4all/SubsysReco.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <vector>
#include <string>
#include <TH1.h>
#include <TRoot.h>
#include <math.h>

class CaloTransverseEnergy:public SubsysReco
{
	private:
		//Here there be dragons
		float GetTotalEnergy(std::vector<float>, float);
		float EMCaltoHCalRatio(float, float);
		void FitToData(std::vector<float>);
		float Heuristic(std::vector<float>); //This is a place holder for right now, will properly implement in a bit, pretty much just adjusting models with an A* approach
		bool ApplyCuts(Event* e);
	public:
		#define PI=3.1415926535;
		int processEvent(Event* e);
		void ProduceOutput();
		CaloTransverseEnergy(){};
		~CaloTransverseEnergy(){};
		struct kinematics 
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

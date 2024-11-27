#ifndef EEC_BRANCHES_H
#define EEC_BRANCHES_H

#include <math.h>
#include <string>
#include <vector>
//This is probably something to revisit soon, I think it would be smart to have this substructure in my TTrees
class CaloAndRegion{
	public:
		CaloAndRegion(int iregion, int icalo){
			switch(iregion)
			{
				case 0:
					region="Full";
					break;
				case 1:
					region="Towards";
					break;
				case 2:
					region="Away";
					break;
				case 3:
					region="Transverse";
					break;
	
			}
			switch(icalo){
				case 0:
					calo="ALL";
					break;
				case 1:
					calo="EMCAL";
					break;
				case 2:
					calo="IHCAL";
					break;
				case 3:
					calo="OHCAL";
					break;
			}
		}
		~CaloAndRegion();		
		std::string region, calo;
		float towards_center;
	
}

class EnergyEnergyCorrelator{
	public:
/*		enum CorrelatorOrder{
			E2C,
			E3C,
			E3C_3D
		};*/
		EnergyEnergyCorrelator(int which_corr){
				this->corr_order=which_corr;
			};
		~EnergyEnergyCorrelator(){}
		int corr_order;	
		float r_L, val, r_0, r_2;
		std::vector<CaloAndRegion*> cr;
		void AssignVals(

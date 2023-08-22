#include "CaloTransverseEnergy.h"
std::vector <int> packets;

int CaloTransverseEnergy::processEvent(Event* e)
{
	for (auto p:packets)
	{
		if(p/1000 ==6 ) 
		{
			//this is the EMCal
		}
		else if (p/1000 == 7) 
		{
			//inner Hcal
		}
		else if (p/1000 == 8)
		{
			//outerhcal
		}
		else
		{
			//not a calorimeter 
			packets.erase(p); //this is not quite the way to do it, but the idea is there
			continue;
		}
	}
}
float CaloTransverseEnergy::GetTotalEnergy(std::vector<float> caloenergy)
{
	//This is really just a fancy sumation method
	float total_energy;
}
float CaloTransverseEnergy::EMCaltoHCalRatio(float em, float hcal)
{
	//this is to project over to phenix data
}
void CaloTransverseEnergy::FitToData(std::vector<float> data,int which_model) 
{
	//Apply fits from CGC, Glauber, others, meant to be generalizable
}
float CaloTransverseEnergy::Heuristic(std::vector<float> model_fit)
{
	float chindf=0;
}
void CaloTransverseEnergy::ValidateDistro(std::map<std::pair<int, int> std::vector<float>>)
{
	//Just make sure that the distribution looks correct
}
void CaloTransverseEnergy::ProduceOutput()
{
	//just make a ton of histos
}
bool CaloTransverseEnergy::ApplyCuts(Event* e)
{
	//pass through a set of cuts
}


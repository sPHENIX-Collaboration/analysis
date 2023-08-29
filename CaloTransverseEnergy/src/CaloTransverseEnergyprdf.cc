#include "CaloTransverseEnergy.h"
std::vector <int> packets;

int CaloTransverseEnergy::processEvent(Event* e)
{
	if(!ApplyCuts(e)) return 1;
	std::vector<float> emcalenergy, ihcalenergy, ohcalenergy; 
	for (auto pn:packets)
	{
		if(pn/1000 ==6 ) 
		{
			//this is the EMCal
			processPacket(pn, e, &emcamenergy, false);
		}
		else if (pn/1000 == 7) 
		{
			//inner Hcal
			processPacket(pn, e, &ihcalenergy, true);
		}
		else if (pn/1000 == 8)
		{
			//outerhcal
			processPacket(pn, e, &ohcalenergy, true);
		}
		else
		{
			//not a calorimeter 
			packets.erase(p); //this is not quite the way to do it, but the idea is there
			continue;
		}
	}
	float emcaltotal=GetTotalEnergy(emcalenergy,1); //not sure about the calibration factor, need to check
	float ihcaltotal=GetTotalEnergy(ihcalenergy,1);
	float ohcaltotal=GetTotalEnergy(ohcalenergy,1);
	EMCALE->Fill(emcaltotal);
	IHCALE->Fill(ihcaltotal);
	OHCALE->Fill(ohcaltotal);
	ETOTAL->Fill(emcaltotal+ihcaltotal+ohcaltotal);
	return 1;
}
void CaloTransverseEnergy::processPacket(int packet, Event * e, std::vector<float>* energy, bool HorE)
{
	Packet *p= e->getPacket(packet); 
	for(int c=0; c<p->iValue(0, "CHANNELS"); c++)
	{
		float eta=0, baseline=0, en=0;
		if(HorE) eta=(((c%16)/2+c/64*8)-12)/12+1/24; //HCal
		else eta=(((c%16)/2+c/4)-48)/48+1/96; //EMCal
		for(int s=0; s<p->iValue(c, "SAMPLES"); s++)
		{
			if(s<3) baseline+=p->iValue(s,c); 
			if(s==3) baseline=baseline/3; //first 3 for baseline
			float val=p->iValue(s,c);
			if(val>en) en=val;
		}
		en=en-baseline;
		energy->push_back(GetTransverseEnergy(en,eta));
		//For the hcal get the phi distribution
		//take packet#, each packet has 8 phi bins
		//
		int phibin=(c%64)/16;
		phibin=phibin*2+c%2;
		phibin+=(ph%10)*8;	
		float phval=PhiD->GetBinContent(phibin);
		phcal+=GetTransverseEnergy(en,eta);
		PhiD->SetBinContent(phibin, phval);	
	}

}
float CaloTransverseEnergy::GetTransverseEnergy(float energy, float eta)
{
	float et=energy*sin(2*atan(exp(-eta)));
	return et;
}
float CaloTransverseEnergy::GetTotalEnergy(std::vector<float> caloenergy, float calib)
{
	//This is really just a fancy sumation method
	float total_energy=0;
	for(auto e:caloenergy)
	{
		total_energy+=e*calib; //right now this is summing with the calibration factor
	}
	total_energy=total_energy/caloenergy.size(); //normalize for number of towers, should apply a "percent of towers" but that can be a next step
	return total_energy; 
}
float CaloTransverseEnergy::EMCaltoHCalRatio(float em, float hcal)
{
	//this is to project over to phenix data
	float ratio=em/hcal;
	return ratio; //again, first order correction, probably more to it than this
	
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


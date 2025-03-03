#ifndef __HELPERSTRUCTS_H__
#define __HELPERSTRUCTS_H__
#include <TH1.h>
#include <TH2.h>

#include <vector>
#include <string>
#include <map>
#include <math.h>

class TowerOutput
{
	public:
		TowerOutput(int tr=0)
		{
			this->threshold=tr;
		}
		~TowerOutput(){};
		void AddE2CValues(float e2c, float rl){
			int index=-1;
			//see if we already have an entry in this vertex
			for(int i = 0; i<(int) RL.size(); i++){
				if(rl == RL.at(i) ){
					index = i;
					break;
				}
				else continue;
			}
			if(index > -1 ){
				//just add to the value if we already have other elements at this fixed value
				E2C.at(index)+=e2c;
			}
			else{
				E2C.push_back(e2c);
				RL.push_back(rl);
			}
			return;
		}
		void AddE3CValues(float e3c, std::array<float, 3> dist){
			int index=-1;
			for(int i = 0; i<(int)RL_RM_RS.size(); i++){
				if(dist == RL_RM_RS.at(i)){
					index = i; 
					break;
				}
				else continue;
			}
			if(index > -1 ){
				E3C_full_shape.at(index)+=e3c;
			}
			else{
				E3C_full_shape.push_back(e3c);
				RL_RM_RS.push_back(dist);
			}
			return;
		}
		void CalculateFlatE3C()
		{
			std::map<float,float> e3c;
			for(int i= 0; i<(int)RL_RM_RS.size(); i++){
				float rl=RL_RM_RS.at(i)[0];
				if(e3c.find(rl) != e3c.end()){
					e3c[rl]+=E3C_full_shape.at(i);
				}
				else{
					e3c[rl]=E3C_full_shape.at(i);
				}
			}
			for(auto r:RL)
			{
				if(e3c.find(r) != e3c.end()) E3C.push_back(e3c[r]);
			} //just keeps the ordering fixed
			return;
		}
		bool Merge(TowerOutput* to)
		{
			if(this->threshold != to->threshold){
				return false;
			}
			else
			{
				//create a merge function that preserves indexing of unique values 
				for(int i=0; i <(int)to->RL.size(); i++)
				{
					this->AddE2CValues(to->E2C[i], to->RL[i]);
				}
				for(int i=0; i < (int)to->RL_RM_RS.size(); i++)
				{
					this->AddE3CValues( to->E3C_full_shape[i], to->RL_RM_RS[i]);
				}
				this->CalculateFlatE3C();
				return true;
			}				

		}
		void Normalize(float energy)
		{
			for(auto e2c: this->E2C)
			{
				e2c=e2c*std::pow(energy,-2);
			}
			for(auto e3c: this->E3C)
			{
				e3c=e3c*std::pow(energy, -3);
			}
			for(auto e3c: this->E3C_full_shape)
			{
				e3c=e3c*std::pow(energy, -3);
			}
			return;
		}
	 	float threshold = 0.;
		std::vector<float> RL; 
		std::vector<std::array<float, 3>> RL_RM_RS;
		std::vector<float> E2C;
		std::vector<float> E3C;
		std::vector<float> E3C_full_shape;
		
		
};

class StrippedDownTower
{
	public:
		enum Regions{
			Full, 
			Towards, 
			Away, 
			TransverseMax,
			TransverseMin
		};
		enum Calorimeter{
			All, 
			EMCAL,
			IHCAL,
			OHCAL
		};
		StrippedDownTower( std::vector<float> thresholds, Calorimeter which_calo=Calorimeter::All, Regions region_tag = Regions::Full){
			//initialize the stripped down tower object
			this->tag = region_tag;
			this->cal = which_calo;
			for(float threshold:thresholds){
				TowerOutput* to=new TowerOutput(threshold);
				TowerOutput* tf=new TowerOutput(threshold);
				RegionOutput->push_back(to);
				FullOutput->push_back(tf);
			} 
			
		}
		~StrippedDownTower(){};
		int getThresholdIndex(float threshold, bool RegionOrFull)
		{
			int index=-1;
			if(RegionOrFull){
				for(int i=0; i< (int)this->FullOutput->size(); i++){
					float thresh_temp=this->FullOutput->at(i)->threshold;
					if(thresh_temp == threshold){
						index = i;
						break;
					}
					else continue;
				}
			}
			else if(!RegionOrFull){
				for(int i=0; i< (int)this->RegionOutput->size(); i++){
					float thresh_temp=this->RegionOutput->at(i)->threshold;
					if(thresh_temp == threshold){
						index = i;
						break;
					}
					else continue;
				}
			}
			return index;
		}
				 	


		float r		=0.;
		float phi	=0.;
		float eta	=0.;
		float E		=0.;
		Regions	tag	=Regions::Full; 
		Calorimeter cal =Calorimeter::All;
		std::vector<TowerOutput*>* RegionOutput;
		std::vector<TowerOutput*>* FullOutput;
		
};
struct DijetQATypePlots{
	DijetQATypePlots(){
		bad_occ_em_oh_rat=new TH2F("h_bad_occupancy_EM_OH_rat", "Occupancy for events that fail the OHCAL ratio cut; #% Towers #geq 10 MeV EMCAL; #% Towers > 10 MeV OHCAL; N_{Evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_h_rat=new TH2F("h_bad_occupancy_EM_H_rat", "Occupancy for events that fail OHCAL ratio cut; #% Towers #geq 10 MeV EMCAL; (#% Towers #geq 10 MeV OHCAL + #% Towers #geq 10 MeV IHCAL)/2", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_oh=new TH2F("h_bad_occupancy_EM_OH", "Occupancy for events that pass  OHCAL ratio cut but otherwise fail; #% Towers #geq 10 MeV EMCAL; #% Towers > 10 MeV OHCAL; N_{Evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_h=new TH2F("h_bad_occupancy_EM_H", "Occupancy for events that pass OHCAL ratio cut but otherwise fail; #% Towers #geq 10 MeV EMCAL; (#% Towers #geq 10 MeV OHCAL + #% Towers #geq 10 MeV IHCAL)/2", 100, -0.005, 0.995, 100, -0.005, 0.995);
		ohcal_bad_hits=new TH2F("h_ohcal_bad_hits", "Energy depositon for events that fail the OHCAL ratio cut; #eta; #varphi; E [GeV]", 24, -1.1, 1.1, 64, -PI, PI);
		emcal_occup=new TH1F("h_EMCAL_occupancy", "Occupancy of EMCAL all events; #% Towers #geq 10 MeV EMCAL; N_{evts}", 100, -0.005, 0.995);
		ihcal_occup=new TH1F("h_IHCAL_occupancy", "Occupancy of IHCAL all events; #% Towers #geq 10 MeV IHCAL; N_{evts}", 100, -0.005, 0.995);
		ohcal_occup=new TH1F("h_OHCAL_occupancy", "Occupancy of OHCAL all events; #% Towers #geq 10 MeV OHCAL; N_{evts}", 100, -0.005, 0.995);
		ohcal_rat_occup=new TH2F("h_OHCAL_rat", "Occupancy of OHCAL as function of OHCAL ratio; #frac{E_{OHCAL}}{E_{ALL CALS}}; #% Towers #geq 500 MeV; N_{evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
	}
	TH2F* bad_occ_em_oh_rat=nullptr;
	TH2F* bad_occ_em_h_rat=nullptr;
	TH2F* bad_occ_em_oh=nullptr;
	TH2F* bad_occ_em_h=nullptr;
	TH2F* ohcal_bad_hits=nullptr;
	TH2F* ohcal_rat_occup=nullptr;
	TH1F* emcal_occup=nullptr;
	TH1F* ihcal_occup=nullptr;
	TH1F* ohcal_occup=nullptr;
	TH1F* ohcal_rat_h=nullptr;
	std::vector<TH2F*> QA_2D_plots {bad_occ_em_oh_rat, bad_occ_em_h_rat, bad_occ_em_oh, bad_occ_em_h, ohcal_bad_hits, ohcal_rat_occup};
	std::vector<TH1F*> QA_1D_plots {emcal_occup, ihcal_occup, ohcal_occup};
	};
#endif

#ifndef __DISCRETIAZATIONCORRECTION_H__
#define __DISCRETIAZATIONCORRECTION_H__

//////////////////////////////////////////////////////////////////////////
//		Discretization Correction for two point correlators 	//
//									//
//		Skaydi 							//
//		1 Oct 2025 						//
//									//
//	discretization correction and jet size corrections		//
//	see attached documentation for derivation and discussion 	//
//	Useful to compare calo only to truth/track			//
//	Works for 2pt and integrated 3pt, No idea on full 3		//
//////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <TH1.h>
#include <TF1.h>
#include <map>
#include <vector>
#include <string>
#include <utility>

#define Pi 3.1415926535

class DiscretizationCorrection
{	
	public:
		DiscretizationCorrection(bool Discretize=false, float binsize=0., bool SizeCorr=false, float jetradius=0.)
		{
			if(Discretize){
				setIsDiscretizationCorrection(Discretize);
				setLatticeSize(binsize);
			}
			if(SizeCorr){
				setIsSizeCorrection(SizeCorr);
				setJetRadius(jetradius);
			}
			fillr2s();
		}
		~DiscretizationCorrection(){};
		
		float getLatticeSize(){return this->m_latticesize;}
		float getJetRadius(){return this->m_jetradius;}
		bool getIsSizeCorrection() { return this->isSizeCorrection;}
		bool getIsDiscretizationCorrection(){ return this->isDiscretizationCorrection;}

		void setLatticeSize(float binsize){
			this->m_latticesize=binsize;
			return;
		}
		void setJetRadius(float jetradius){
			this->m_jetradius=jetradius;
			return;
		}
		void setIsSizeCorrection(bool size){
			this->isSizeCorrection=size;
			return;
		}
		void setIsDiscretizationCorrection(bool discr){
			this->isDiscretizationCorrection=discr;
			return;
		}
	
		float GetCorrectionFactor(float RL){
			if(Correction_factors.find(RL) != Correction_factors.end()){
				return Correction_factors[RL];
			}
			else{
				bool isLess = true, isBigger=false;
				float LessVal= 0., BigVal=0., Val=0., diff=0.;
				for(auto c:Correction_factors)
				{
					if(isBigger && ! isLess ) break;
					if(c.first < RL){
						LessVal=c.second;
						isLess=true;
						isBigger=false;
						diff=RL-c.first;
						continue;
					}
					else if(c.first == RL) 
						return c.second;
					else if(c.first > RL){
						BigVal=c.second;
						isLess=false;
						isBigger=true;
						float diff2=c.first-RL;
						float dr=diff2+diff;
						Val=LessVal*diff/dr+BigVal*diff2/dr;
						return Val;
					}
					else continue;
				}
			}
		}

		void CaluculateCorrectionFactor(float maxRL=this->sPHENIX_maxRL, int order=0)
		{
			std::string Plot_title="Efficiency correction factor";
			if(isSizeCorrection){
				PlotTitle+=" for R="+std::to_string(m_jetradius)+" jets"
			}
			else{
				PlotTitle+=" for whole calorimeter";
			}
			if(isDiscretizationCorrection){
				PlotTitle+=" with bin width #delta = " +std::to_string(m_latticesize)+" in #eta and #varphi ";
			}
			else
			{
				PlotTitle+=" on a continuous lattice ";
			}
			PlotTitle+="; #Delta R_{L}^{2}; #varepsilon";
			Correction_factor_hist=new TH1F("h_corr_fact", PlotTitle.c_str(), (int) std::pow(maxRL/m_latticesize,2), 0, std::pow(maxRL,2)); 
			this->correction_order=order;
			if(isDiscretizationCorrection)
			{
				int nSteps=std::pow(maxRL, 2)/std::pow(m_latticesize,2);
				for(int i=0; i<nSteps; i++)
				{
					float eps=1.;
					float rl=std::sqrt( (float) i );
					if(correction_order==0){
						if(isSizeCorrection)
						{
							eps=all_order_continuous_jet(rl)*std::pow(all_order_discrete_jet(rl), -1);
						}
						eps=eps*all_order_discrete_full(rl)*std::pow(all_order_continuous_full(rl), -1);	
						else{
							if(isSizeCorrection)
							{
								eps=order_by_order_cont_jet(rl)*std::pow(all_order_discrete_jet(rl), -1);
							}
							eps=eps*all_order_discrete_full(rl)*std::pow(all_order_continuous_full(rl), -1);
						}
					}
					Correction_factors[rl]=eps;
					Correction_factor_hist->Fill(i, eps);
				}
			}
			else
			{
				//no clue how best to do this, this should just be a function???
				continue;
			}
			return;

		}			
			
		
	private:
		
		float m_latticesize		=0.;
		float m_jetradius		=0.;
		bool isSizeCorrection		=false;
		bool isDiscretizationCorrection	=false;
		float sPHENIX_maxRL 		=std::sqrt(std::pow(PI,2)+std::pow(2.2, 2));

		int correction_order = 3; //how many powers of R_L to expand the elliptic integrals to
		
		std::map<float, float> Correction_factors;
		TH1F* Correction_factor_hist;
		float order_by_order_cont_jet(float RL)
		{
			float ep = R_L*pow(2*PI*std::pow(m_jetradius, 2), -1);
			float ic = std::sqrt(4*std::pow(m_jetradius, 2)-std::pow(R_L, 2)) + m_jetradius*std::arccos(R_L*std::pow(2*m_jetradius, -1));
			if(correction_order >=3 ){
			       	ic+= -R_L/3.*std::pow(std::arccos(R_L*std::pow(2*m_jetradius, -1)), 3);
			}
			if(correction_order >= 5 ){
				ic+=R_L*std::pow(120.*m_jetradius, -1)*(4*std::pow(m_jetradius, 2) - std::pow(R_L, 2))*std::pow(std::arccos(R_L*std::pow(2*m_jetradius, -1)), 5);
			}
			if(correction_order > 5)
			{
				for(int i=0; i<=correction_order; i++)
				{
					if(i%2 != 1) continue;
					ic+=0; //getting this expansion  is going to be tricky 
				}
			}
			float c = ep*ic;
			return c;
		}
		float all_order_continuous_jet(float RL)
		{
			//This is the solution of the integral for geometric pairs at distance RL where both points lie within the jet
			float I_inc=R_L*std::pow(2*PI*std::pow(m_jetradius, 2), -1)*std::sqrt(4*std::pow(m_jetradius, 2)-std::pow(RL,2)) * all_order_continous_full(RL);
			float elipint=std::ellint_2f(std::arccos(RL*std::pow(2*m_jetradius, -1)), std::pow(RL*std::pow(m_jetradius, -1), 2)) *all_order_continuous_full(RL);
			I_inc+=elipint;
			return I_inc;
		}
		float all_order_continuous_full(float RL)
		{
			//this is the total pairs in the phase space defined by the points lying within the jet and their pairs at distance R_l irrespective of the pair being in the jet radius
			float I_Total=std::pow(2*PI, 2)*RL*m_jetradius;
			return I_Total;
		}
		float all_order_discrete_jet(float RL) 
		{
			//pairs with both in jet
			float c=0;
			int i=floor(std::pow((RL+m_jetradius)/m_latticesize, 2));
			for(int j=0; j<=i; j++) 
			{
				if(j < std::pow(m_jetradius/m_latticesize, 2)) continue;
				c+=this->r2s.at(j);
			}
			float ct=all_order_discrete_full(RL) - c;
			return ct;
		}
		float all_order_discrete_full(float RL)
		{
			//pairs with one in jet
			float c=0;
			int i=floor(std::pow(RL/m_latticesize, 2));
			int counter=0;
			for(auto n=this->r2s.begin(); n!=this->r2s.end(); ++n)
			{
				c+=*n;
				counter++;
				if(counter >=i) break;
			}
			c=c*r2s.at(i);
			return c;
		}
		int sum_of_squares(int r) 
		{
			int r2=0.;
			std::vector<int> divs=getDivisors(r);
			std::pair<int, int> d1_3=divisors_mod_1_3(divs);
			r2=4*(d1_3.first-d1_3.second);
			return r2;
		}

		std::pair<int,int> divisors_mod_1_3(std::vector<int> divisors)
		{
			std::pair<int, int> d1_3 {0,0};
			for(auto i:divisors)
			{
				if(i%4==1) d1_3.first++;
				else if (i%4==3) d1_3.second++;
				else continue;
			}
			return d1_3;
		}

		std::vector<int> getDivisors(int r)
		{
			std::vector<int> divisors; 
			for(int i=1; i<=r; i++)
			{
				if(r % i ==0 ) divisors.push_back(i);
				else continue;
			}
			return divisors;
		}
		void fillr2s()
		{
			float max=0.;
			if(isSizeCorrection) max=m_jetradius/m_latticesize;
			else max=std::sqrt(std::pow(2.2, 2) +std::pow(2*PI, 2))/m_latticesize; //set to sPHENIX phase space
			for(int i=0; i<std::pow(max, 2); i++)
			{
				this->r2s.push_back(sum_of_squares(r));
			}
			return;
		}
		std::vector<int> r2s;
		TF1 *Continous_jet_f1, *Continuous_full_f1; 
		TF1 *Discrete_jet_f1,  *Discete_full_f1;
		TF1 *Corretion_funct;
		
}
#endif

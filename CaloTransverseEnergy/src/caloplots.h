#ifndef __CALOPLOTS_H__
#define __CALOPLOTS_H__

#include "CaloTransverseEnergy.h"
#include <string>
#include <map>
#include <vector>
#include <math.h> 
#include <iostream>

#include <TH1.h>
#include <TH2.h>

//Creates a huge list of plots procedurally, allowing for me to be properly lazy 
//need to test implementation and check in rcalo values as of 4-12-2023
//Have to on the eta shift with Joey
class CaloTransverseEnergy; 
class caloplots
{ //creates a relevant plot for each calorimeter type
	public: 
		std::string calo="EMCAL";
		bool sim=false;
		TH1F* Energy, *E_phi, *ET, *dET_eta, *ET_phi, *ET_z, *dET, *acceptance_eta, *phi, *eta, *z_val, *timing, *E_f, *E_s, *E_m, *Eta_width, *top3_behavior, *phi_projections, *dET_eta_top, *dET_eta_bot, *dET_eta_mid, *EMCAL_towering;
		std::vector<TH1F*> hists_1;
		TH2F* ET_eta_phi, *E_eta_phi, *ET_z_eta, *Hits2D, *EMCAL_tower_proj;
		std::vector<TH2F*> hists_2;
		float acceptance=0.0; 
		bool ok_phi=false;
		float z=0., zl, zh, zmin=1., zmax=1., rcalo=93.5;
		int phibins=256, etabins=96;
		float etamin=-1.13, etamax=1.13;
		caloplots(std::string caloname="EMCAL", float zlow=-30., float zhigh=30., bool simul=false){
			zl=zlow; 
			zh=zhigh;
			calo=caloname;
			sim=simul;
		//	hists_1=NULL;
		//	hists_2=NULL;
			z=(zl+zh)/2;
			if(calo.find("OHCAL") != std::string::npos || calo.find("ALL") != std::string::npos) rcalo=177.423;
			if(calo.find("IHCAL") != std::string::npos) rcalo=115.0;
			if(calo.find("HCAL") != std::string::npos || calo.find("ALL") != std::string::npos){
					zmax=rcalo*(1-etamax*etamax)/(2*etamax);
					zmin=rcalo*(1-etamin*etamin)/(2*etamin);
				}
			//AdjustEtaEdge();
			//MakePlots();
		}
		~caloplots();
		void UpdateZ(float zlow, float zhigh){ 
			z=(zlow+zhigh)/2.0;
			zl=zlow;
			zh=zhigh;
			AdjustEtaEdge();
			MakePlots();
		}
		void validatePhi(){
			int good=0, nbins=E_phi->GetNbinsX();
			float avg=0;
			for(int i=0; i<nbins; i++) avg+=E_phi->GetBinContent(i+1);
			avg=avg/nbins;
			for(int j=0; j<nbins; j++) if(E_phi->GetBinContent(j+1) >0.5*avg && E_phi->GetBinContent(j+1) < 1.5*avg) good++;
			if(good>= 0.75*nbins) ok_phi=true;
		}
		void getAcceptance(){
			for(int i=0; i<etabins; i++){
				int live_towers=0;
				for(int j=0; j<phibins; j++){
					if(ET_eta_phi->GetBinContent(i+1, j+1) > 0){
						 live_towers++;
						 acceptance++;
					}
				}
				if(live_towers==0) live_towers=1;
				float pct=(float)live_towers/(float)phibins;
//				std::cout<<"eta bin is " <<dET_eta->GetBinCenter(i+1) <<" with acceptance " <<pct<<std::endl;
				float bc=(float)dET_eta->GetBinCenter(i+1);
				acceptance_eta->Fill(bc, pct);
//				std::cout<<"bin content fill is " <<bc <<" but the bin that should be filled has a center at " <<acceptance_eta->GetBinCenter(i) <<std::endl;
//				std::cout<<acceptance_eta->GetBinContent(i+1) <<" should be " <<pct <<std::endl;
			}
			acceptance=acceptance/(float)(phibins*etabins);
		}
		void scaleThePlots(int n_evt, std::vector<std::map<std::string, float>>* calo_data){
			n_evt=1;
			dET_eta->Scale(1/(float)n_evt);
			if(acceptance > 0.0) dET_eta->Divide(acceptance_eta);
			//else acceptance=1.0;
			if(acceptance <= 0.0) acceptance=1.0;
			ET_phi->Scale(1/((float)n_evt*acceptance));
			ET_z->Scale(1/((float)n_evt*acceptance));
			dET->Scale(1/(float)acceptance);
			for(auto cd:(*calo_data)) cd[calo]=cd[calo]/acceptance;
			
		}
		void setSimulation(std::string rn){
			for(auto h:hists_1){
				std::string tl=h->GetTitle();
				std::string nm=h->GetName();
				tl="Monte Carlo Truth from run "+rn+", "+tl;
				nm="MC_truth_"+rn+"_"+nm;
				h->SetNameTitle(nm.c_str(), tl.c_str());
			}
			for(auto h:hists_2){
				std::string tl=h->GetTitle();
				std::string nm=h->GetName();
				tl="Monte Carlo Truth from run "+rn+", "+tl;
				nm="MC_truth_"+rn+"_"+nm;
				h->SetNameTitle(nm.c_str(), tl.c_str());
			}
		}
	/*	void BuildTotal(plots PLT)
		{
			for(auto data_point:PLT.event_data){
				float total_val=0;
				total_val+=data_point["EMCAL"];
				total_val+=data_point["IHCAL"];
				total_val+=data_point["OHCAL"];
				dET->Fill(total_val);
			}
			for(unsigned int i=0; i<hists_1.size(); i++){
				 hists_1.at(i)->Add(PLT.em->hists_1.at(i));
				 hists_1.at(i)->Add(PLT.ihcal->hists_1.at(i));
				 hists_1.at(i)->Add(PLT.ohcal->hists_1.at(i));
			}

			for(unsigned int i=0; i<hists_2.size(); i++){
				 hists_2.at(i)->Add(PLT.em->hists_2.at(i));
				 hists_2.at(i)->Add(PLT.ihcal->hists_2.at(i));
				 hists_2.at(i)->Add(PLT.ohcal->hists_2.at(i));
			}
		}*/
	private:
		void AdjustEtaEdge()
		{
			//shifts min and max eta value on z boundries.
			etamax=sinh(etamax)*(float)rcalo+(float)zh;
			etamax=asinh(etamax/(float)rcalo);
			etamin=asinh(sinh(etamin)+(float)zl/(float)rcalo);
			std::cout<<"eta is [" <<etamin <<"," <<etamax <<"]" <<std::endl;
		}
		void MakePlots(){
			if(calo.find("HCAL") != std::string::npos || calo.find("ALL") != std::string::npos){
				etabins=24;
				phibins=64;
			}
			if(sim) calo+="_MC";
			int z_lab=abs(z);
			if(z<0) calo+="_neg";
			float pb=(float)phibins-0.1;
			float eb=(float)etabins-0.1;
			
			//std::cout<<"making the plots for calo " <<calo <<std::endl;
			Energy=new TH1F(Form("Energy_%s_z_%d", calo.c_str(), z_lab), Form("E_{event} in %s with vertex z=%f; E [GeV]", calo.c_str(), z), 1000, 0.5, 1000.5);
			
			ET=new TH1F(Form("ET_%s_z_%d", calo.c_str(), z_lab), Form("E_{T}^{event} in %s with vertex z=%f; E [GeV]", calo.c_str(), z), 1000, 0.5, 1000.5);
			E_phi=new TH1F(Form("E_phi_%s_z_%d", calo.c_str(), z_lab), Form("E(#varphi) in %s with vertex z=%f; #varphi; E [GeV]",calo.c_str(),z), phibins, -0.01, 6.3);
			
			ET_phi=new TH1F(Form("ET_phi_%s_z_%d", calo.c_str(), z_lab), Form("E_{T}(#varphi) in %s with vertex z=%f; #varphi; #E_{T} [GeV]",calo.c_str(),z), phibins, -0.01, 6.3);
			
			dET_eta=new TH1F(Form("dET_eta_%s_z_%d", calo.c_str(), z_lab), Form("dE_{T}/d#eta(#eta) in %s with vertex z=%f; #eta; #frac{dE_{T}}{d#eta} [GeV]",calo.c_str(),z), etabins, (float) etamin-0.02, (float)etamax-0.02);
			
			ET_z=new TH1F(Form("ET_z_%s_z_%d", calo.c_str(), z_lab), Form("E_{T}(z) in %s with vertex z=%f; z; #E_{T} [GeV]",calo.c_str(),z), 40, zmin,zmax);

			dET=new TH1F(Form("dET_%s_z_%d", calo.c_str(), z_lab), Form("#frac{dE_{T}}{d#eta} in %s with vertex z=%f; #frac{dE_{T}}{d#eta} [GeV]",calo.c_str(),z), 1000, -0.5, 1000.5);
			acceptance_eta=new TH1F(Form("acceptance_%s_z_%d", calo.c_str(), z_lab), Form("Acceptance in physical #eta slice in %s with vertex z=%f; #eta; percent towers responding", calo.c_str(),z), etabins, (float)etamin-0.01, (float)etamax-0.01);
			phi=new TH1F(Form("phi_%s_z_%d", calo.c_str(), z_lab), Form("Hit distribution in #varphi_{bin} in %s with vertex z=%f; #varphi_{bin}; N_{Hits}", calo.c_str(), z), phibins, -0.5, phibins-0.5);
			eta=new TH1F(Form("eta_%s_z_%d", calo.c_str(), z_lab), Form("Hit distribution in physical #eta_{bin} in %s with vertex z=%f; #eta_{bin}; N_{Hits}", calo.c_str(), z), etabins, -0.5, etabins-0.5);
			z_val=new TH1F(Form("z_%s_z_%d", calo.c_str(), z_lab), Form("Hit distribution in z vertex in %s with vertex centered at z=%f; z; N_{events}", calo.c_str(), z), 100, zl, zh);
			timing=new TH1F(Form("time_%s_z_%d", calo.c_str(), z_lab), Form("Timing distribution precut in %s with vertex centered at z=%f; time [samples]; N_{evts}",calo.c_str(), z), 20, -10.5, 10.5);
			E_f=new TH1F(Form("en_%s_z_%d", calo.c_str(), z_lab), Form("Energy distribution pre-timing cut in %s with vertex centered at z=%f; Energy [GeV]; N_{towers}", calo.c_str(), z), 10000, -20.5, 20.5);  
			E_m=new TH1F(Form("en_pp_%s_z_%d", calo.c_str(), z_lab), Form("Energy distribution post-energy cut in %s with vertex centered at z=%f; Energy [GeV]; N_{towers}", calo.c_str(), z), 10000, -20.5, 20.5);  
			E_s=new TH1F(Form("en_pc_%s_z_%d", calo.c_str(), z_lab), Form("Energy distribution post-timing cut in %s with vertex centered at z=%f; Energy [GeV]; N_{towers}", calo.c_str(), z), 10000, -20.5, 20.5); 
			Eta_width=new TH1F(Form("eta_width_%s_z_%d", calo.c_str(), z_lab), Form("Eta bin width in %s with vertex centered at z=%f; #eta_{bin}; width", calo.c_str(), z), etabins, -0.1, eb); 
			top3_behavior=new TH1F(Form("tow_3_%s_z_%d", calo.c_str(), z_lab), Form("Ratio of the average signal between the top 3 secotors for %s with vertex centered at z=%d in #phi in #eta_{bin} 1,22; #frac{E_{T}^{top}}{E_{T}^{bottom}}", calo.c_str(), z_lab), 100, -0.5, 4.5);
			phi_projections=new TH1F(Form("phi_proj_%s_z_%d", calo.c_str(), z_lab), Form("Phi in #eta_{bin} = 1,22 in %s with vtx centered z=%d; #phi; E_{T}", calo.c_str(), z_lab), phibins, -0.1, pb);
			dET_eta_top=new TH1F(Form("dET_eta_top_%s_z_%d", calo.c_str(), z_lab), Form("dE_{T}/d#eta (#eta) for #varphi #in [#frac{7 #pi}{16}, #frac{10 #pi}{16}) in %s with vertex z=%f; #eta; #frac{dE_{T}}{d#eta} [GeV]",calo.c_str(),z), etabins, (float) etamin-0.02, (float)etamax-0.02);
			dET_eta_bot=new TH1F(Form("dET_eta_bot_%s_z_%d", calo.c_str(), z_lab), Form("dE_{T}/d#eta (#eta) for #varphi #in [#frac{23 #pi}{16}, #frac{26 #pi}{16}) in %s with vertex z=%f; #eta; #frac{dE_{T}}{d#eta} [GeV]",calo.c_str(),z), etabins, (float) etamin-0.02, (float)etamax-0.02);
			dET_eta_mid=new TH1F(Form("dET_eta_mid_%s_z_%d", calo.c_str(), z_lab), Form("dE_{T}/d#eta (#eta) for #varphi #in [0, #frac{3 #pi}{16}) in %s with vertex z=%f; #eta; #frac{dE_{T}}{d#eta} [GeV]",calo.c_str(),z), etabins, (float) etamin-0.02, (float)etamax-0.02);
			EMCAL_towering=new TH1F(Form("emct_%s_z_%d", calo.c_str(), z_lab), Form("Retowering towers for EMCal to %s with vertex centerd at z=%f; N_{EM tower}/%s tower", calo.c_str(), z, calo.c_str()), 60, -0.5, 59.5);
			hists_1.push_back(Energy);
			hists_1.push_back(ET);
			hists_1.push_back(E_phi);
			hists_1.push_back(ET_phi);
			hists_1.push_back(dET_eta);
			hists_1.push_back(dET_eta_top);
			hists_1.push_back(dET_eta_bot);
			hists_1.push_back(dET_eta_mid);
			hists_1.push_back(ET_z);
			hists_1.push_back(dET);
			hists_1.push_back(acceptance_eta);
			hists_1.push_back(phi);
			hists_1.push_back(eta);
			hists_1.push_back(z_val);
			hists_1.push_back(timing); 
			hists_1.push_back(E_f);
			hists_1.push_back(E_s);
			hists_1.push_back(E_m);
			hists_1.push_back(Eta_width);
			hists_1.push_back(top3_behavior);
			hists_1.push_back(phi_projections);
			if(calo.find("HCAL") != std::string::npos) hists_1.push_back(EMCAL_towering);
			ET_eta_phi=new TH2F(Form("ET_eta_phi_%s_z_%d", calo.c_str(), z_lab), Form("E_{T}(#eta, #varphi) in %s with vertex z=%f; #eta; #varphi; E_{T} [GeV]", calo.c_str(), z), etabins, etamin, etamax, phibins,-0.1, 6.3);
		       	E_eta_phi=new TH2F(Form("E_eta_phi_%s_z_%d", calo.c_str(), z_lab), Form("E(#eta, #varphi)	physical binning in %s with vertex centered at z=%f; #eta; #varphi; E [GeV]", calo.c_str(), z), etabins, -0.1, eb, phibins, -0.1, pb);
			ET_z_eta=new TH2F(Form("ET_z_eta_%s_z_%d", calo.c_str(), z_lab), Form("E_{T}(z_{vertex}, #eta) in %s with vertex centered at z=%f; z_{vertex}; #eta; E_{T} [GeV]", calo.c_str(), z), 40, zl, zh, etabins, etamin, etamax);
			Hits2D=new TH2F(Form("Hits_2D_%s_z_%d", calo.c_str(), z_lab), Form("Hits in #eta and #varphi in %s with vertex centered at z=%f; #eta; #varphi", calo.c_str(), z), etabins, -0.1, eb, phibins, -0.1, pb);
			EMCAL_Tower_proj=new TH2F(Form("Tower_proj_%s_z_%d", calo.c_str(), z_lab), Form("EMCAL Retowering towers per %s tower with vertex centered at z=%f; #eta_{bin}, #varphi_{bin}, N_{EMT}", calo.c_str(), z), etabins, -0.1, eb, phibins, -0.1, pb);
			hists_2.push_back(ET_eta_phi);
			hists_2.push_back(E_eta_phi);
			hists_2.push_back(ET_z_eta);
			hists_2.push_back(Hits2D);
			if(calo.find("HCAL") != std::string::npos) hists_2.push_back(EMCAL_tower_proj);
		}
};
#endif

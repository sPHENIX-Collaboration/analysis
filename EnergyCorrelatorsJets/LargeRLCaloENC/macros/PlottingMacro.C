#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
//sphenix style
#include "sPhenixStyle.C"
#include "sPhenixStyle.h"

//root includes
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TPad.h>
#include <TDirectory.h>
#include <TFitResult.h>

//c++ includes
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <sstream>
#include <math.h>
#include <map>
std::vector<TDirectory*> file_dir_top;
std::vector<std::array<TDirectory*,4>> lower_dirs;	
void LoadInRegionPlots(TDirectory* botdir, std::vector<std::vector<TH1F*>*>* RegionPlots)
{
	int Calo=0;
	for(auto b:*botdir->GetListOfKeys()){
		TKey* k=(TKey*)b;
		std::string hist_name = k->GetName();
		if(hist_name.find("EMCAL")!=std::string::npos) Calo=0;
		else if(hist_name.find("IHCAL")!=std::string::npos) Calo=1;
		else if(hist_name.find("OHCAL")!=std::string::npos) Calo=2;
		else continue;
		if(hist_name.find("e_")!=std::string::npos)	RegionPlots->at(0)->at(Calo)=(TH1F*)botdir->Get(hist_name.c_str());
		if(hist_name.find("E2C_")!=std::string::npos)	RegionPlots->at(1)->at(Calo)=(TH1F*)botdir->Get(hist_name.c_str());
		if(hist_name.find("E3C_")!=std::string::npos)	RegionPlots->at(2)->at(Calo)=(TH1F*)botdir->Get(hist_name.c_str());
		if(hist_name.find("R_")!=std::string::npos)	RegionPlots->at(3)->at(Calo)=(TH1F*)botdir->Get(hist_name.c_str());
		if(hist_name.find("n_")!=std::string::npos)	RegionPlots->at(4)->at(Calo)=(TH1F*)botdir->Get(hist_name.c_str());
	}
	std::array<int, 3> n_evts;
	n_evts[0]=RegionPlots->at(0)->at(0)->GetEntries();
	n_evts[1]=RegionPlots->at(0)->at(1)->GetEntries();
	n_evts[2]=RegionPlots->at(0)->at(2)->GetEntries();
	
	for(auto R:*RegionPlots)
	{
		for(int i=0; i<(int)R->size(); i++)
		{
			auto p=R->at(i);
			if(!p) continue;
			if(n_evts[i]==0) n_evts[i]=1;
			p->Scale(1/((float) n_evts[i]));
			std::string ptitle = p->GetYaxis()->GetTitle();
			if(ptitle.find("Total Ev") == std::string::npos)
				p->SetYTitle(Form("#frac{1}{Total Events} %s", p->GetYaxis()->GetTitle()));
		}
	}
	return;
}
std::array<std::vector<std::vector<std::string>>,4> GetPlotLabels(std::array<std::vector<std::vector<TH1F*>*>*, 4> RegPlts)
{
	std::array<std::vector<std::vector<std::string>>, 4> plt_lbs;
	for(int k=0; k<(int)RegPlts.size(); k++){
		auto a=RegPlts[k];
		std::vector<std::vector<std::string>> a1;
		for(int i=0; i< (int)a->size(); i++)
		{
			std::vector<std::string> a2;
			for(int j=0; j <(int) a->at(i)->size(); j++)
			{
				std::string lb="";
				std::string plt_name, thr, cal;
				if(! a->at(i)->at(j)) continue;
				plt_name=a->at(i)->at(j)->GetName();
				std::stringstream  str_plt (plt_name);
				std::string subparts;
				bool get_name=false;
				while(std::getline(str_plt, subparts, '_')){
					if( get_name){
						thr=subparts;
						break;
					}
					else if(subparts.find("CAL") != std::string::npos){
						get_name=true;
						cal=subparts;
						continue;
					}
					else continue;
				}
				lb=cal+", E_{tower} #geq "+thr+" MeV";
				a2.push_back(lb);
			}
			a1.push_back(a2);
		}
		plt_lbs[k]=a1;
	}
	return plt_lbs;
}
void LoadInPlots(int threshold_index, std::array<std::vector<std::vector<TH1F*>*>*, 4>* RegPlts){
		//std::cout<<__LINE__<<std::endl;
	TH1F* CEMC_FullEnergy=NULL, *CEMC_FullE2C=NULL, *CEMC_FullE3C=NULL, *CEMC_FullR=NULL, *CEMC_FullN=NULL;
	TH1F* CEMC_TowardsEnergy=NULL, *CEMC_TowardsE2C=NULL, *CEMC_TowardsE3C=NULL, *CEMC_TowardsR=NULL, *CEMC_TowardsN=NULL;
	TH1F* CEMC_AwayEnergy=NULL, *CEMC_AwayE2C=NULL, *CEMC_AwayE3C=NULL, *CEMC_AwayR=NULL, *CEMC_AwayN=NULL;
	TH1F* CEMC_TransEnergy=NULL, *CEMC_TransE2C=NULL, *CEMC_TransE3C=NULL, *CEMC_TransR=NULL, *CEMC_TransN=NULL;
	
	TH1F* OHCAL_FullEnergy=NULL, *OHCAL_FullE2C=NULL, *OHCAL_FullE3C=NULL, *OHCAL_FullR=NULL, *OHCAL_FullN=NULL;
	TH1F* OHCAL_TowardsEnergy=NULL, *OHCAL_TowardsE2C=NULL, *OHCAL_TowardsE3C=NULL, *OHCAL_TowardsR=NULL, *OHCAL_TowardsN=NULL;
	TH1F* OHCAL_AwayEnergy=NULL, *OHCAL_AwayE2C=NULL, *OHCAL_AwayE3C=NULL, *OHCAL_AwayR=NULL, *OHCAL_AwayN=NULL;
	TH1F* OHCAL_TransEnergy=NULL, *OHCAL_TransE2C=NULL, *OHCAL_TransE3C=NULL, *OHCAL_TransR=NULL, *OHCAL_TransN=NULL;
	
	TH1F* IHCAL_FullEnergy=NULL, *IHCAL_FullE2C=NULL, *IHCAL_FullE3C=NULL, *IHCAL_FullR=NULL, *IHCAL_FullN=NULL;
	TH1F* IHCAL_TowardsEnergy=NULL, *IHCAL_TowardsE2C=NULL, *IHCAL_TowardsE3C=NULL, *IHCAL_TowardsR=NULL, *IHCAL_TowardsN=NULL;
	TH1F* IHCAL_AwayEnergy=NULL, *IHCAL_AwayE2C=NULL, *IHCAL_AwayE3C=NULL, *IHCAL_AwayR=NULL, *IHCAL_AwayN=NULL;
	TH1F* IHCAL_TransEnergy=NULL, *IHCAL_TransE2C=NULL, *IHCAL_TransE3C=NULL, *IHCAL_TransR=NULL, *IHCAL_TransN=NULL;
	
	std::vector<TH1F*>* FullEnergy=new std::vector<TH1F*>{}, *FullE2C=new std::vector<TH1F*>{}, *FullE3C=new std::vector<TH1F*>{}, *FullR=new std::vector<TH1F*>{}, *FullN=new std::vector<TH1F*>{};
	std::vector<TH1F*>* TowardsEnergy=new std::vector<TH1F*>{}, *TowardsE2C=new std::vector<TH1F*>{}, *TowardsE3C=new std::vector<TH1F*>{}, *TowardsR=new std::vector<TH1F*>{}, *TowardsN=new std::vector<TH1F*>{};
	std::vector<TH1F*>* AwayEnergy=new std::vector<TH1F*>{}, *AwayE2C=new std::vector<TH1F*>{}, *AwayE3C=new std::vector<TH1F*>{}, *AwayR=new std::vector<TH1F*>{}, *AwayN=new std::vector<TH1F*>{};
	std::vector<TH1F*>* TransEnergy=new std::vector<TH1F*>{}, *TransE2C=new std::vector<TH1F*>{}, *TransE3C=new std::vector<TH1F*>{}, *TransR=new std::vector<TH1F*>{}, *TransN=new std::vector<TH1F*>{};
		//std::cout<<__LINE__<<std::endl;
	
	std::vector<std::vector<TH1F*>*>* Full, *Away, *Towards, *Trans;
	
	FullEnergy->push_back(CEMC_FullEnergy);
	FullEnergy->push_back(IHCAL_FullEnergy);
	FullEnergy->push_back(OHCAL_FullEnergy);

	FullE2C->push_back(CEMC_FullE2C);
	FullE2C->push_back(IHCAL_FullE2C);
	FullE2C->push_back(OHCAL_FullE2C);
	
	FullE3C->push_back(CEMC_FullE3C);
	FullE3C->push_back(IHCAL_FullE3C);
	FullE3C->push_back(OHCAL_FullE3C);
	
	FullR->push_back(CEMC_FullR);
	FullR->push_back(IHCAL_FullR);
	FullR->push_back(OHCAL_FullR);
	
	FullN->push_back(CEMC_FullN);
	FullN->push_back(IHCAL_FullN);
	FullN->push_back(OHCAL_FullN);
		//std::cout<<__LINE__<<std::endl;
	
	Full=new std::vector<std::vector<TH1F*>*> {FullEnergy, FullE2C, FullE3C, FullR, FullN};
	
	TowardsEnergy->push_back(CEMC_TowardsEnergy);
	TowardsEnergy->push_back(IHCAL_TowardsEnergy);
	TowardsEnergy->push_back(OHCAL_TowardsEnergy);

	TowardsE2C->push_back(CEMC_TowardsE2C);
	TowardsE2C->push_back(IHCAL_TowardsE2C);
	TowardsE2C->push_back(OHCAL_TowardsE2C);
	
	TowardsE3C->push_back(CEMC_TowardsE3C);
	TowardsE3C->push_back(IHCAL_TowardsE3C);
	TowardsE3C->push_back(OHCAL_TowardsE3C);
	
	TowardsR->push_back(CEMC_TowardsR);
	TowardsR->push_back(IHCAL_TowardsR);
	TowardsR->push_back(OHCAL_TowardsR);
	
	TowardsN->push_back(CEMC_TowardsN);
	TowardsN->push_back(IHCAL_TowardsN);
	TowardsN->push_back(OHCAL_TowardsN);
		//std::cout<<__LINE__<<std::endl;
	
	Towards=new std::vector<std::vector<TH1F*>*> {TowardsEnergy, TowardsE2C, TowardsE3C, TowardsR, TowardsN};
	
	AwayEnergy->push_back(CEMC_AwayEnergy);
	AwayEnergy->push_back(IHCAL_AwayEnergy);
	AwayEnergy->push_back(OHCAL_AwayEnergy);

	AwayE2C->push_back(CEMC_AwayE2C);
	AwayE2C->push_back(IHCAL_AwayE2C);
	AwayE2C->push_back(OHCAL_AwayE2C);
	
	AwayE3C->push_back(CEMC_AwayE3C);
	AwayE3C->push_back(IHCAL_AwayE3C);
	AwayE3C->push_back(OHCAL_AwayE3C);
	
	AwayR->push_back(CEMC_AwayR);
	AwayR->push_back(IHCAL_AwayR);
	AwayR->push_back(OHCAL_AwayR);
	
	AwayN->push_back(CEMC_AwayN);
	AwayN->push_back(IHCAL_AwayN);
	AwayN->push_back(OHCAL_AwayN);
		//std::cout<<__LINE__<<std::endl;
	
	Away=new std::vector<std::vector<TH1F*>*> {AwayEnergy, AwayE2C, AwayE3C, AwayR, AwayN};
	
	TransEnergy->push_back(CEMC_TransEnergy);
	TransEnergy->push_back(IHCAL_TransEnergy);
	TransEnergy->push_back(OHCAL_TransEnergy);

	TransE2C->push_back(CEMC_TransE2C);
	TransE2C->push_back(IHCAL_TransE2C);
	TransE2C->push_back(OHCAL_TransE2C);
	
	TransE3C->push_back(CEMC_TransE3C);
	TransE3C->push_back(IHCAL_TransE3C);
	TransE3C->push_back(OHCAL_TransE3C);
	
	TransR->push_back(CEMC_TransR);
	TransR->push_back(IHCAL_TransR);
	TransR->push_back(OHCAL_TransR);
	
	TransN->push_back(CEMC_TransN);
	TransN->push_back(IHCAL_TransN);
	TransN->push_back(OHCAL_TransN);
	
	Trans=new std::vector<std::vector<TH1F*>*> {TransEnergy, TransE2C, TransE3C, TransR, TransN};
	
		//std::cout<<__LINE__<<std::endl;
	TDirectory* topdir=file_dir_top[threshold_index];
	topdir->cd();
	for(int i=0; i<(int)lower_dirs[threshold_index].size(); i++){
		//std::cout<<__LINE__<<std::endl;
		TDirectory* botdir=lower_dirs[threshold_index][i];
		botdir->cd();
		//std::cout<<__LINE__<<std::endl;
		std::string dir_name=botdir->GetName();
		//std::cout<<__LINE__<<std::endl;
		std::vector<std::vector<TH1F*>*>* rp;
		//std::cout<<__LINE__<<std::endl;
		if(dir_name.find("Full") != std::string::npos) rp=Full;
		else if(dir_name.find("Towards") != std::string::npos ) rp=Towards;	
		else if(dir_name.find("Trans") != std::string::npos ) rp=Trans;	
		else if(dir_name.find("Away") != std::string::npos ) rp=Away;	
		//std::cout<<__LINE__<<std::endl;
		LoadInRegionPlots(botdir, rp);
		//std::cout<<__LINE__<<std::endl;
	}
	RegPlts->at(0)=Full;
	RegPlts->at(1)=Towards; 
	RegPlts->at(2)=Away;
	RegPlts->at(3)=Trans;
		//std::cout<<__LINE__<<std::endl;
	return;
}

std::array<std::vector<std::vector<TH1F*>*>*, 4> DrawOneThreshold(TFile* datafile, std::array<std::vector<TPad*>*, 4>* p1, std::array<std::vector<TLegend*>*, 4>* l1, int threshold_index, /*choose which of the set of indicies we use, rather thas spefcific value*/ int style)
{
	//p1 is {full, towards, away} {E, E2C, E3C, R, N}
		//std::cout<<__LINE__<<std::endl;
	std::array<std::vector<std::vector<TH1F*>*>*, 4> RegPlts {};
	LoadInPlots(threshold_index, &RegPlts);
	//Plots are structured as {full, towards, away, transverse} and one level down {E, E2C, E3C, R, N} then {EMCAL, IHCAL, OHCAL}
		//std::cout<<__LINE__<<std::endl;
	int offset=0;
	std::string run_type="";
	if(style==0) run_type="Pedestal";
	if(style==1) run_type="Pulser";
	if(style==2) run_type="LED";
	std::array<std::vector<std::vector<std::string>>, 4> plot_label;
		//std::cout<<__LINE__<<std::endl;
	plot_label=GetPlotLabels(RegPlts);
		//std::cout<<__LINE__<<std::endl;
	if(threshold_index == 3 ) offset = 6;
	else if (threshold_index > 3) offset = 9; //keep the marker shapes distinguisable
	else if (threshold_index > 5) offset= 17;
		//std::cout<<__LINE__<<std::endl;
	for(int i=0; i<(int) p1->size(); i++){
		std::cout<<p1->size() <<std::endl;
		for(int j=0; j<(int)p1->at(i)->size(); j++)
		{
			if(!p1->at(i)->at(j)) continue;
			p1->at(i)->at(j)->cd();
			if(! RegPlts[i]->at(j)) continue;
			bool need_to_cont=false;
			for(int k=0; k<(int) RegPlts[i]->at(j)->size(); k++) 
				if(!RegPlts[i]->at(j)->at(k))
					need_to_cont=true;
			if(need_to_cont) continue;
			if(style== 0){
				RegPlts[i]->at(j)->at(0)->SetLineColor(kGreen);
				RegPlts[i]->at(j)->at(0)->SetMarkerColor(kGreen);
			}
			if(style== 1){
				RegPlts[i]->at(j)->at(0)->SetLineColor(kTeal+3);
				RegPlts[i]->at(j)->at(0)->SetMarkerColor(kTeal+3);
			}
			if(style== 2){
				RegPlts[i]->at(j)->at(0)->SetLineColor(kBlack);
				RegPlts[i]->at(j)->at(0)->SetMarkerColor(kBlack);
			}
			RegPlts[i]->at(j)->at(0)->SetMarkerStyle(20+threshold_index+offset);
			RegPlts[i]->at(j)->at(0)->SetMarkerSize(1.5f);
			RegPlts[i]->at(j)->at(0)->Draw("same");
			l1->at(i)->at(j)->AddEntry(RegPlts[i]->at(j)->at(0), Form("%s  %s ", run_type.c_str(), plot_label[i][j][0].c_str()));

			if(style== 0 ) {
				RegPlts[i]->at(j)->at(1)->SetLineColor(kPink+9);
				RegPlts[i]->at(j)->at(1)->SetMarkerColor(kPink+9);
			}
			if(style== 1 ) {
				RegPlts[i]->at(j)->at(1)->SetLineColor(kOrange+7);
				RegPlts[i]->at(j)->at(1)->SetMarkerColor(kOrange+7);
			}
			if(style== 2 ) {
				RegPlts[i]->at(j)->at(1)->SetLineColor(kRed);
				RegPlts[i]->at(j)->at(1)->SetMarkerColor(kRed);
			}
			
			RegPlts[i]->at(j)->at(1)->SetMarkerStyle(20+threshold_index+offset);
			RegPlts[i]->at(j)->at(2)->SetMarkerSize(1.5f);
			RegPlts[i]->at(j)->at(1)->Draw("same");
			l1->at(i)->at(j)->AddEntry(RegPlts[i]->at(j)->at(0), Form("%s %s", run_type.c_str(), plot_label[i][j][0].c_str()));
			if(style == 0){
				RegPlts[i]->at(j)->at(2)->SetLineColor(kCyan);
				RegPlts[i]->at(j)->at(2)->SetMarkerColor(kCyan);
			}
			if(style == 1){
				RegPlts[i]->at(j)->at(2)->SetLineColor(kBlue);
				RegPlts[i]->at(j)->at(2)->SetMarkerColor(kBlue);
			}
			if(style == 2){
				RegPlts[i]->at(j)->at(2)->SetLineColor(kViolet+1);
				RegPlts[i]->at(j)->at(2)->SetMarkerColor(kViolet+1);
			}
			RegPlts[i]->at(j)->at(2)->SetMarkerStyle(20+threshold_index+offset);
			RegPlts[i]->at(j)->at(2)->SetMarkerSize(1.5f);
			RegPlts[i]->at(j)->at(2)->Draw("same");
			l1->at(i)->at(j)->AddEntry(RegPlts[i]->at(j)->at(0), Form("%s %s", run_type.c_str(), plot_label[i][j][0].c_str()));
		}
	}
		//std::cout<<__LINE__<<std::endl;
	
	return RegPlts;
}
/*int CompareRegions(std::map <int, TH1F*> calo_hist, std::string calo, std::string 
{
	return 1;
}*/
void PlotRatios(std::vector<std::map<std::string, std::array<std::vector<std::vector<TH1F*>*>*, 4>>> Thrs, std::array<std::vector<TPad*>*,4>* prat, std::array<std::vector<TLegend*>*, 4>* Lt)
{
	//compare the pulser and LED to pedestal in all thresholds
	int t=0;
	for(auto th:Thrs)
	{
		std::array<std::vector<std::vector<TH1F*>*>*, 4> Ped, Pul, LED;
		Ped=th["Pedestal"];
		Pul=th["Pulser"];
		bool has_led=false;
		if(th.find("LED") != th.end()){
			has_led=true;
			LED=th["LED"];
		}
		for(int i=0; i<4; i++)
		{
			std::vector<std::vector<TH1F*>*>* pd_r=new std::vector<std::vector<TH1F*>*>{}, *pl_r=new std::vector<std::vector<TH1F*>*>{}, *ld_r=new std::vector<std::vector<TH1F*>*>{};
			pd_r=Ped[i];
			pl_r=Pul[i];
			if(has_led) ld_r=LED[i];
			std::vector<TPad*>* rps=prat->at(i);
			std::vector<TLegend*>* lts=Lt->at(i);
			for(int j=0; j<(int) pd_r->size(); j++)
			{
				TPad* p=rps->at(j);
				TLegend* l=lts->at(j);
				p->cd();
				if(j==0 || j==4 ) continue;
				for(int k=0; k<(int) pd_r->at(j)->size(); k++)
				{
					if(!pd_r->at(j)->at(k)) continue;
					TH1F* ped_hist=(TH1F*)pd_r->at(j)->at(k)->Clone();
					if(pl_r->at(j)->at(k)->GetEntries() == 0 ) continue;
					ped_hist->Divide(pl_r->at(j)->at(k));
					ped_hist->SetYTitle(" Pedestal / (Pulser, LED) ");
					ped_hist->GetYaxis()->SetTitleOffset(1);
					ped_hist->GetYaxis()->SetTitleSize(0.05f);
					int offset=0;
					if(t== 5) offset=1;
					if(t==6 ) offset=9; 
					ped_hist->SetMarkerStyle(24+t+offset);
					std::string lb="";
					std::string plt_name, thr, cal;
					plt_name=ped_hist->GetName();
					std::stringstream  str_plt (plt_name);
					std::string subparts;
					bool get_name=false;
					while(std::getline(str_plt, subparts, '_')){
						if( get_name){
							thr=subparts;
							break;
						}
						else if(subparts.find("CAL") != std::string::npos){
							get_name=true;
							cal=subparts;
							continue;
						}
						else continue;
					}
					lb=cal+", E_{tower} #geq "+thr+" MeV";
					std::string lbp=" Pedestal / Pulser : "+lb;
					l->AddEntry(ped_hist, lbp.c_str());
					ped_hist->Draw("same");
					if(has_led ){
						TH1F* ped_hist_l=(TH1F*)pd_r->at(j)->at(k)->Clone();
						if(ld_r->at(j)->at(k)->GetEntries() == 0 ) continue;
						ped_hist_l->Divide(ld_r->at(j)->at(k));
						ped_hist_l->SetMarkerColor(ld_r->at(j)->at(k)->GetMarkerColor());
						ped_hist_l->SetLineColor(ld_r->at(j)->at(k)->GetLineColor());
						ped_hist_l->SetYTitle(" Pedestal / (Pulser, LED) ");
						ped_hist_l->GetYaxis()->SetTitleOffset(1);
						ped_hist_l->GetYaxis()->SetTitleSize(0.05f);
						ped_hist_l->SetMarkerStyle(24+t+offset);
						std::string lbl=" Pedestal / LED : "+lb;
						l->AddEntry(ped_hist_l, lbl.c_str());
						ped_hist_l->Draw("same");
					}
				}
			}
		}
		t++;			
	}
	return;
}
/*int E2C_and_E3C_with_thresholds(TFile* datafile, )
{
	
}*/
int LocalRunThresholdScan(TFile* Pedestal_data, int ped_run, TFile* Pulser_data, int pulse_run, TFile* LED_data=NULL, int led_run = 0, bool include_led=false)
{
	SetsPhenixStyle();
	Pedestal_data->cd();
	for(int i=0; i< 7; i++){
		TDirectory* t=(TDirectory*)Pedestal_data->Get(Form("Threshold_Index_%d", i));
		file_dir_top.push_back(t);
		t->cd();
		TDirectory* ft=(TDirectory*)t->Get(Form("Full_Calorimeter_Threshold_index_%d", i));
		TDirectory* tw=(TDirectory*)t->Get(Form("Towards_Region_Threshold_index_%d", i));
		TDirectory* aw=(TDirectory*)t->Get(Form("Away_Region_Threshold_index_%d", i));
		TDirectory* trans=(TDirectory*)t->Get(Form("Transverse_Region_Threshold_index_%d", i));
		std::array<TDirectory*, 4> sub {ft, tw, aw, trans};
		lower_dirs.push_back(sub);
	}
	std::cout<<"Got the directory Names"<<std::endl;
	std::array<std::vector<TLegend*>*, 4> LegendTitles, PlotLeg;
	std::array<std::vector<TPad*>*, 4> p_main, p_ratio;
	std::array<std::vector<TCanvas*>*, 4> Canvases;
	for(int i= 0; i<4; i++)
	{
		std::string region;
		std::vector<TLegend*> *LT=new std::vector<TLegend*>{}, *PL=new std::vector<TLegend*>{};
		std::vector<TPad*> *P1=new std::vector<TPad*>{}, *P2=new std::vector<TPad*>{};
		std::vector<TCanvas*> *cs=new std::vector<TCanvas*>{};
		if(i==0) region="Full Calorimeter";
		if(i==1) region="Lead Region";
		if(i==2) region="Away Region";
		if(i==3) region="Transverse Regions";
		//std::cout<<__LINE__<<std::endl;
		for(int j=0; j < 5; j++)
		{
			TCanvas* c1=new TCanvas(Form("c_%d_%d", i, j), Form("c_%d_%d", i, j));
			c1->cd();
			float ymin=0.33;
			if( j == 0 || j == 4) ymin=0; //no need for the ratio plots in E and N
			TPad* p1=new TPad(Form("p_1_%d_%d", i, j), Form("p_1_%d_%d", i, j), 0, ymin, 1, 1);
			TPad* p2=new TPad(Form("p_2_%d_%d", i, j), Form("p_2_%d_%d", i, j), 0, 0, 1, 0.3);
			//std::cout<<__LINE__<<std::endl;	
			TLegend* l1=new TLegend(0.7, 0.5, 0.9, 0.95);
			l1->SetFillStyle(0);
			l1->SetFillColor(0);
			l1->SetBorderSize(0);
			l1->SetTextSize(0.1f);
			l1->AddEntry("", "#bf{#it{sPHENIX}} Internal", "");
			l1->AddEntry("", "Calorimeter Calibration Running", "");
			l1->AddEntry("", Form("Pedestal Run %d", ped_run), "");
			l1->AddEntry("", Form("Pulser Run %d", pulse_run), "");
			if(include_led) l1->AddEntry("", Form("LED Run %d", led_run), "");
			l1->AddEntry("", region.c_str(), "");
			std::string var;
			if(j==0) var="#sum_{Towers} Energy";
			if(j==1) var="2 Point Energy Correlator";
			if(j==2) var="3 Point Energy Correlator";
			if(j==3) var="Geometric Seperation of Non-zero Energy Towers";
			if(j==4) var="Number of non-zero energy towers";
			l1->AddEntry("", var.c_str(), "");
			LT->push_back(l1);
			//std::cout<<__LINE__<<std::endl;
			TLegend* l2=new TLegend(0.1, 0.6, 0.5, 0.85);
			l2->SetFillStyle(0);
			l2->SetFillColor(0);
			l2->SetBorderSize(0);
			l2->SetTextSize(0.01f);
			l2->SetNColumns(3);
			PL->push_back(l2);
				
			P1->push_back(p1);
			P2->push_back(p2);
			cs->push_back(c1);
		}	
		LegendTitles[i]=LT;
		PlotLeg[i]=PL;
		p_main[i]=P1;
		p_ratio[i]=P2;
		Canvases[i]=cs;
		//std::cout<<__LINE__<<std::endl;
	}
	std::vector<std::map<std::string, std::array<std::vector<std::vector<TH1F*>*>*, 4>> > threshold_maps {}; 
	for(int i=0; i<7; i++)
	{
		//std::cout<<__LINE__<<std::endl;
		std::map<std::string, std::array<std::vector<std::vector<TH1F*>*>*, 4>> tm;
		std::cout<<p_main[0]->size()<<std::endl;
		std::array<std::vector<std::vector<TH1F*>*>*, 4> rg_1=DrawOneThreshold(	Pedestal_data, &p_main, &PlotLeg, i, 0);
		tm["Pedestal"]=rg_1;
		//std::cout<<__LINE__<<std::endl;
		std::array<std::vector<std::vector<TH1F*>*>*, 4> rg_2=DrawOneThreshold(	Pulser_data, &p_main, &PlotLeg, i, 0);
		tm["Pulser"]=rg_2;
		//std::cout<<__LINE__<<std::endl;
		if(include_led){
			std::array<std::vector<std::vector<TH1F*>*>*, 4> rg_3=DrawOneThreshold(	LED_data, &p_main, &PlotLeg, i, 0);
			tm["LED"]=rg_3;
		}
		threshold_maps.push_back(tm);
		//std::cout<<__LINE__<<std::endl;
	}
		//std::cout<<__LINE__<<std::endl;
	PlotRatios(threshold_maps, &p_ratio, &PlotLeg);
		//std::cout<<__LINE__<<std::endl;
	for(int i=0; i<(int)Canvases.size(); i++)
	{
		std::string region="";
		if(i==0) region="Full";
		if(i==1) region="Lead";
		if(i==2) region="Away";
		if(i==3) region="Transverse";
		
		for(int j=0; j<(int)Canvases[i]->size(); j++)
		{
			Canvases[i]->at(j)->cd();
			p_main[i]->at(j)->Draw();
			if(j >= 1 && j <=3 ) p_ratio[i]->at(j)->Draw();
			p_main[i]->at(j)->cd();
			LegendTitles[i]->at(j)->Draw();
			PlotLeg[i]->at(j)->Draw();
			std::string var;
			if(j==0) var="E";
			if(j==1) var="E2C";
			if(j==2) var="E3C";
			if(j==3) var="RL";
			if(j==4) var="N";
			Canvases[i]->at(j)->Print(Form("~/local_run/thresh_scan_%s_var_%s.pdf", region.c_str(), var.c_str())); 
			Canvases[i]->at(j)->Print(Form("~/local_run/thresh_scan_%s_var_%s.ps", region.c_str(), var.c_str())); 
		}
	}
	return 0;
}
int EnergyDistr(TFile* f1, int run_num, std::string type)
{
	SetsPhenixStyle();
	TCanvas* c1=new TCanvas(Form("EMC_%d_%s", run_num, type.c_str()), Form("EMC_%d_%s", run_num, type.c_str())); 
	TCanvas* c2=new TCanvas(Form("IHC_%d_%s", run_num, type.c_str()), Form("IHC_%d_%s", run_num, type.c_str())); 
	TCanvas* c3=new TCanvas(Form("OHC_%d_%s", run_num, type.c_str()), Form("OHC_%d_%s", run_num, type.c_str())); 
	std::vector<TCanvas*> cs {c1, c2, c3};
	for(int i=0; i<(int)cs.size(); i++)
	{
		cs[i]->cd();
		TPad* p1=new TPad("p1", "p1", 0, 0.51, 0.49, 0.75);
		TPad* p2=new TPad("p2", "p2", 0.51, 0.51, 0.9, 0.75);
		TPad* p3=new TPad("p3", "p3", 0, 0.25, 0.49, 0.5);
		TPad* p4=new TPad("p4", "p4", 0.51, 0.25, 0.9, 0.5);
		TPad* p5=new TPad("p5", "p5", 0, 0, 0.49, 0.24);
		TPad* p6=new TPad("p6", "p6", 0.51, 0, 0.9, 0.24);
		TPad* p7=new TPad("p7", "p7", 0, 0.76, 0.49, 0.95);
		p7->cd();
		TH2F* h1, *h2, *h3, *h4, *h5, *h6, *h7;
		float z_max=0.;
		if(i==0)
		{
			h1=(TH2F*)f1->Get("emcal_energy_10_MeV");
			h2=(TH2F*)f1->Get("emcal_energy_21_MeV");
			h3=(TH2F*)f1->Get("emcal_energy_33_MeV");
			h4=(TH2F*)f1->Get("emcal_energy_44_MeV");
			h5=(TH2F*)f1->Get("emcal_energy_56_MeV");
			h6=(TH2F*)f1->Get("emcal_energy_68_MeV");
			h7=(TH2F*)f1->Get("emcal_energy_80_MeV");
		}
		if(i==1)
		{
			h1=(TH2F*)f1->Get("ihcal_energy_5_MeV");
			h2=(TH2F*)f1->Get("ihcal_energy_6_MeV");
			h3=(TH2F*)f1->Get("ihcal_energy_8_MeV");
			h4=(TH2F*)f1->Get("ihcal_energy_10_MeV");
			h5=(TH2F*)f1->Get("ihcal_energy_11_MeV");
			h6=(TH2F*)f1->Get("ihcal_energy_13_MeV");
			h7=(TH2F*)f1->Get("ihcal_energy_15_MeV");
		
		}
		if(i==2)
		{
			h1=(TH2F*)f1->Get("ohcal_energy_10_MeV");
			h2=(TH2F*)f1->Get("ohcal_energy_21_MeV");
			h3=(TH2F*)f1->Get("ohcal_energy_33_MeV");
			h4=(TH2F*)f1->Get("ohcal_energy_44_MeV");
			h5=(TH2F*)f1->Get("ohcal_energy_56_MeV");
			h6=(TH2F*)f1->Get("ohcal_energy_68_MeV");
			h7=(TH2F*)f1->Get("ohcal_energy_80_MeV");
		}
		z_max=h1->GetMaximum();
		h2->GetZaxis()->SetRangeUser(0, z_max);
		h3->GetZaxis()->SetRangeUser(0, z_max);
		h4->GetZaxis()->SetRangeUser(0, z_max);
		h5->GetZaxis()->SetRangeUser(0, z_max);
		h6->GetZaxis()->SetRangeUser(0, z_max);
		h7->GetZaxis()->SetRangeUser(0, z_max);
		h1->Draw("colz");
		TLegend* l1=new TLegend(0.2, 0.9, 0.2, 1);
		l1->SetTextSize(0.1f);
		l1->SetFillStyle(0);
		l1->SetFillColor(0);
		l1->SetBorderSize(0);
		l1->AddEntry("", h1->GetTitle(), "");
		l1->Draw();
		cs[i]->cd();
		p7->Draw();
		TLegend* l=new TLegend(0.4, 0.8, 1, 1);
		l->SetFillStyle(0);
		l->SetFillColor(0);
		l->SetBorderSize(0);
		cs[i]->cd();
		std::string Calo="Electromagnetic";
		if(i==1) Calo="Inner Hadronic";
		else if (i==2) Calo="Outer Hadronic";
		l->AddEntry("", "#bf{#it{sPHENIX}} Internal", "");
		l->AddEntry("", Form("%s Calorimeter", Calo.c_str()), "");
		l->AddEntry("", Form("%s Run %d", type.c_str(), run_num), "");
		l->Draw();
		p1->cd();
		h2->Draw("colz");
		TLegend* l2=new TLegend(0, 0.9, 0.2, 1);
		l2->SetTextSize(0.1f);
		l2->SetFillStyle(0);
		l2->SetFillColor(0);
		l2->SetBorderSize(0);
		l2->AddEntry("", h2->GetTitle(), "");
		l2->Draw();
		cs[i]->cd();
		p1->Draw();
		
		p3->cd();
		h2->Draw("colz");
		TLegend* l3=new TLegend(0, 0.9, 0.2, 1);
		l3->SetTextSize(0.1f);
		l3->SetFillStyle(0);
		l3->SetFillColor(0);
		l3->SetBorderSize(0);
		l3->AddEntry("", h3->GetTitle(), "");
		l3->Draw();
		cs[i]->cd();
		p3->Draw();

		p5->cd();
		h4->Draw("colz");
		TLegend* l4=new TLegend(0, 0.9, 0.2, 1);
		l4->SetTextSize(0.1f);
		l4->SetFillStyle(0);
		l4->SetFillColor(0);
		l4->SetBorderSize(0);
		l4->AddEntry("", h4->GetTitle(), "");
		l4->Draw();
		cs[i]->cd();
		p5->Draw();
			
		p2->cd();
		h5->Draw("colz");
		TLegend* l5=new TLegend(0, 0.9, 0.2, 1);
		l5->SetTextSize(0.1f);
		l5->SetFillStyle(0);
		l5->SetFillColor(0);
		l5->SetBorderSize(0);
		l5->AddEntry("", h5->GetTitle(), "");
		l5->Draw();
		cs[i]->cd();
		p2->Draw();
		
		p4->cd();
		h6->Draw("colz");
		TLegend* l6=new TLegend(0, 0.9, 0.2, 1);
		l6->SetTextSize(0.1f);	
		l6->SetFillStyle(0);
		l6->SetFillColor(0);
		l6->SetBorderSize(0);
		l6->AddEntry("", h6->GetTitle(), "");
		l6->Draw();
		cs[i]->cd();
		p4->Draw();
		
		p6->cd();
		h7->Draw("colz");
		TLegend* l7=new TLegend(0, 0.9, 0.2, 1);
		l7->SetTextSize(0.1f);
		l7->SetFillStyle(0);
		l7->SetFillColor(0);
		l7->SetBorderSize(0);
		l7->AddEntry("", h7->GetTitle(), "");
		l7->Draw();
		cs[i]->cd();
		p6->Draw();
	}
	cs[0]->Print(Form("~/EMCAL_run_%d_type_%s.pdf", run_num, type.c_str()));
	cs[1]->Print(Form("~/IHCAL_run_%d_type_%s.pdf", run_num, type.c_str()));
	cs[2]->Print(Form("~/OHCAL_run_%d_type_%s.pdf", run_num, type.c_str()));
	return 0;
}	
TH1F* getReflected(TH1F* norm)
{
	TH1F* rf = (TH1F*) norm->Clone();
	rf->SetName(Form("%s_reflected", norm->GetName()));
	rf->SetTitle(Form("%s |E|", norm->GetTitle()));
	for(int i=0; i<rf->GetNbinsX(); i++)
	{
		rf->SetBinContent(i, 0);
	}
	for(int i= 0; i<norm->GetNbinsX(); i++)
	{
		float bin_center = norm->GetBinCenter(i);
		if( bin_center > 0 ) break;
		float bin_val = norm->GetBinContent(i);
		rf->Fill(abs(bin_center), bin_val);
	}
	return rf;
}
TH1F* regeneratePlot(TH1F* energies)
{
	TH1F* thres=(TH1F*) energies->Clone();
	std::vector<int> freq {};
	float max_val = 0;
	for(int i = 0; i<energies->GetNbinsX(); i++)
	{
		float bin_center = energies->GetBinCenter(i);
		if(bin_center < 0 ) continue;
		freq.push_back(0);
		if(energies->GetBinContent(i) > 0 ) max_val = bin_center;
	}
	for(int i = 0;  i < energies->GetNbinsX(); i++)
	{
		float bin_center = energies->GetBinCenter(i);
		if(bin_center < 0 ) continue;
		if((int)bin_center >= (int)max_val) break;
		for(int j=(int) bin_center; j < max_val; j++)
		{
			freq[j]+=energies->GetBinContent(i); 
		}
	}
	for(int i = 0; i<(int)freq.size(); i++){
		thres->Fill(i, freq.at(i));
	}
	return thres;
}
double fitTheNegSide(TH1F* data_energy, TCanvas* c, TLegend* l, TLegend* ll, float n_evt, int cal)
{
	c->cd();
	c->SetLogy();
	TH1F* negative_side=new TH1F(Form("neg_data_%s", data_energy->GetName()), Form("Negative Energy Side %s", data_energy->GetTitle()), data_energy->GetNbinsX(), data_energy->GetBinLowEdge(1), data_energy->GetBinLowEdge(data_energy->GetNbinsX()));
	if(cal==2) data_energy->Rebin(4);
	for(int i=0; i<data_energy->GetNbinsX(); i++)
	{
		float E=data_energy->GetBinCenter(i);
		if(E > 0 ) break;
		int N=data_energy->GetBinContent(i);
		for(int j=0; j<N; j++){
			negative_side->Fill(E);
			negative_side->Fill(std::abs(E));
		}
	}
	data_energy->Scale(1/(float)n_evt);
	negative_side->Scale(1/(float)n_evt);
	if(cal==2){
		data_energy->Scale(1/4.0);
		negative_side->Scale(1/4.0);
	}
	TF1* f_fit=new TF1("fit","gaus",data_energy->GetBinLowEdge(1), data_energy->GetBinLowEdge(data_energy->GetNbinsX()));
	TFitResultPtr p=negative_side->Fit(f_fit, "S");
	double sigma = p->Parameter(2);
	data_energy->Draw();
	if(cal==0){
		data_energy->GetXaxis()->SetRangeUser(-300, 999);
		data_energy->GetYaxis()->SetRangeUser(0.01, 1100);
	}
	else if(cal==1){
		data_energy->GetXaxis()->SetRangeUser(-50, 600);
		data_energy->GetYaxis()->SetRangeUser(0.01, 500);
	}
	else{
		data_energy->GetXaxis()->SetRangeUser(-100, 600);
		data_energy->GetYaxis()->SetRangeUser(0.01, 500);
	}

	negative_side->SetMarkerColor(kPink+1);
	negative_side->SetLineColor(kPink+1);
//	negative_side->SetMarkerStyle(21);
	negative_side->Draw("same");
	f_fit->SetLineColor(kBlack);
	f_fit->Draw("same");
	l->AddEntry(negative_side, "E_{tow} < 0, Reflected");
	l->AddEntry(f_fit, Form("#splitline{Gaussian Fit to Negative Side noise}{ #splitline{#mu = %.3g #pm %.2g MeV}{ #sigma = %3.3g #pm %.1g MeV }}", p->Parameter(1),p->ParError(1), sigma, p->ParError(2)), "l");
	l->Draw();
	ll->Draw();
	return sigma;
}
		
int OneDEnergyPlots(TFile* fpedestal, int run_ph, int run_pe, TFile* fdata, int run_d, TFile* fsim10, TFile* fsim30, TFile* fsimMB)
{
	SetsPhenixStyle();
	TCanvas* c1=new TCanvas("OHE", "Outer HCal Energy");
	TCanvas* c2=new TCanvas("IHE", "Inner HCal Energy");
	TCanvas* c3=new TCanvas("EME", "EM Cal Energy");
	TCanvas* c4=new TCanvas("OHH", "Outer HCal Hits");
	TCanvas* c5=new TCanvas("IHH", "Inner HCal Hits");
	TCanvas* c6=new TCanvas("EMH", "EM Cal Hits");
	TCanvas* c7=new TCanvas("AE", "AllCal Energy");
	TCanvas* c8=new TCanvas("AH", "All Cal Hits");
	/*TCanvas* cr1=new TCanvas("OHEr", "Outer HCal Energy");
	TCanvas* cr3=new TCanvas("IHEr", "Inner HCal Energy");
	TCanvas* cr5=new TCanvas("EMEr", "EM Cal Energy");
	TCanvas* cr2=new TCanvas("OHHr", "Outer HCal Hits");
	TCanvas* cr4=new TCanvas("IHHr", "Inner HCal Hits");
	TCanvas* cr6=new TCanvas("EMHr", "EM Cal Hits");
	TCanvas* cr7=new TCanvas("AEr", "AllCal Energy");
	TCanvas* cr8=new TCanvas("AHr", "All Cal Hits");*/
	std::vector<TFile*> files {fpedestal, fdata, fsim10, fsim30, fsimMB};
	std::vector<TLegend*> lt {}, ll {}; //lt_ref {};
	std::vector<TCanvas*> Canvases {c1, c4, c2, c5, c3, c6, c7, c8};
//	std::vector<TCanvas*> Cr {cr1, cr2, cr3, cr4, cr5, cr6, cr7, cr8};
	std::vector<TPad*> pad_val, pad_rats;
	for(int i=0; i<8; i++)
	{
		Canvases[i]->cd();
		TPad* p1=new TPad("p1", "p1", 0, 0.35, 1, 1);
		TPad* p2=new TPad("p2", "p2", 0, 0, 1, 0.33);
		p1->cd();
		p1->SetLogy();
		Canvases[i]->SetLogy();	
		if(i%2 == 1) Canvases[i]->SetLogx();
		float xmin=0.5;
		float xmax=0.85;
		if(i%2 == 1 ){
			 xmin=0.2;
			 xmax=0.35;
		}
		TLegend* l1=new TLegend(0.5, 0.7, 0.85, 1);
		TLegend* l=new TLegend(xmin, 0.2, xmax, 0.45);
		TLegend* l2=new TLegend(xmin, 0.2, xmax, 0.45);
		l1->SetFillStyle(0);
		l1->SetFillColor(0);
		l1->SetBorderSize(0);
		l->SetFillStyle(0);
		l->SetFillColor(0);
		l->SetBorderSize(0);
		l2->SetFillColor(0);
		l2->SetBorderSize(0);
		l->SetTextSize(0.04f);
		l1->SetTextSize(0.04f);
		l2->SetTextSize(0.04f);
		l1->AddEntry("", "#it{#bf{sPHENIX}} Preliminary", "");
		l1->AddEntry("", "p+p #sqrt{s} = 200 GeV", "");
		//l1->SetFillColorAlpha(0);
	//	l->SetFillColorAlpha(0);
		
		if(i==0){
			l1->AddEntry("", "Outer Hadronic Calorimeter", "");
		//	l1->AddEntry("", "Tower Energy", "");
		}
		if(i==1){
			l1->AddEntry("", "Outer Hadronic Calorimeter", "");
		//	l1->AddEntry("", "Tower hits above threshold", "");
		}
		if(i==2){
			l1->AddEntry("", "Inner Hadronic Calorimeter", "");
		//	l1->AddEntry("", "Tower Energy", "");
		}
		if(i==3){
			l1->AddEntry("", "Inner Hadronic Calorimeter", "");
		//	l1->AddEntry("", "Tower hits above threshold", "");
		}
		if(i==4){
			l1->AddEntry("", "Electromagnetic Calorimeter", "");
		//	l1->AddEntry("", "Tower Energy", "");
		}
		if(i==5){
			l1->AddEntry("", "Electromagnetic Calorimeter", "");
		//	l1->AddEntry("", "Tower hits above threshold", "");
		}
		if(i==6){
			l1->AddEntry("", "All Calorimeters binned to HCAL", "");
		//	l1->AddEntry("", "Tower Energy", "");
		}
		if(i==7){
			l1->AddEntry("", "All Calorimeters binned to HCAL", "");
		//	l1->AddEntry("", "Tower hits above threshold", "");
		}
		lt.push_back(l);
		ll.push_back(l1);
		pad_val.push_back(p1);
		pad_rats.push_back(p2);	
	}
	std::vector <std::string> label {"Pedestal", "p+p, 10 GeV #leq Lead jet p_{T} #leq 30 GeV ", "Pythia8 + GEANT4 + Noise, Lead jet p_{T} #geq 10 GeV","Pythia8 + GEANT4 + Noise, Lead jet p_{T} #geq 30 GeV", "Pythia, Minimum bias Detroit Tune"};

	std::array<float, 6> maxes {0.,0.,0.,0.,0.,0.};
	std::vector<std::array<TH1F*,6>> rats{};
	
/*	for(int i=0; i<(int)files.size(); i++)
	{
		if(i==0) continue;
		TFile* f=files[i];
		TH1F* oh_e = (TH1F*) f->Get("ohcal_energy")->Clone();
		TH1F* oh_h = (TH1F*) f->Get("ohc_hit")->Clone();
		TH1F* ih_e = (TH1F*) f->Get("ihcal_energy")->Clone();
		TH1F* ih_h = (TH1F*) f->Get("ihc_hit")->Clone();
		TH1F* em_e = (TH1F*) f->Get("emcal_energy")->Clone();
		TH1F* em_h = (TH1F*) f->Get("emc_hit")->Clone();
	//	TH1F* a_e = (TH1F*) f->Get("allcal_energy")->Clone();
	//	TH1F* a_h = (TH1F*) f->Get("allc_hit")->Clone();
		TH1F* oh_Et = (TH1F*) f->Get("ohcal_total_E");
		TH1F* ih_Et = (TH1F*) f->Get("ihcal_total_E");
		TH1F* em_Et = (TH1F*) f->Get("emcal_total_E");
	//	TH1F* a_Et = (TH1F*) f->Get("allcal_total_E");
		TFile* f1=files[1];
		TH1F* oh_e1 = (TH1F*) f1->Get("ohcal_energy")->Clone();
		TH1F* oh_h1 = (TH1F*) f1->Get("ohc_hit")->Clone();
		TH1F* ih_e1 = (TH1F*) f1->Get("ihcal_energy")->Clone();
		TH1F* ih_h1 = (TH1F*) f1->Get("ihc_hit")->Clone();
		TH1F* em_e1 = (TH1F*) f1->Get("emcal_energy")->Clone();
		TH1F* em_h1 = (TH1F*) f1->Get("emc_hit")->Clone();
	//	TH1F* a_e1 = (TH1F*) f1->Get("allcal_energy")->Clone();
	//	TH1F* a_h1 = (TH1F*) f1->Get("allc_hit")->Clone();
		TH1F* oh_Et1 = (TH1F*) f1->Get("ohcal_total_E");
		TH1F* ih_Et1 = (TH1F*) f1->Get("ihcal_total_E");
		TH1F* em_Et1 = (TH1F*) f1->Get("emcal_total_E");
	//	TH1F* a_Et1 = (TH1F*) f1->Get("allcal_total_E");
		int n_evt_oh=oh_Et->GetEntries();
		int n_evt_ih=ih_Et->GetEntries();
		int n_evt_em=em_Et->GetEntries();
	//	int n_evt_all=a_Et->GetEntries();
		oh_e->Scale(1/(float)n_evt_oh);
		oh_h->Scale(1/(float)n_evt_oh);
		ih_e->Scale(1/(float)n_evt_ih);
		ih_h->Scale(1/(float)n_evt_ih);
		em_e->Scale(1/(float)n_evt_em);
		em_h->Scale(1/(float)n_evt_em);
		//a_hScale(1/(float)n_evt_all);
		//a_hScale(1/(float)n_evt_all);
		oh_e->GetXaxis()->SetRangeUser(0.1, 4000);
		em_e->GetXaxis()->SetRangeUser(0.1, 5000);
		ih_h->GetXaxis()->SetRangeUser(0.1, 1600);*/
		
/*		int n_evt_oh1=oh_Et1->GetEntries();
		int n_evt_ih1=ih_Et1->GetEntries();
		int n_evt_em1=em_Et->GetEntries();
	//	int n_evt_all1=a_Et1->GetEntries();
		oh_e1->Scale(1/(float)n_evt_oh1);
		oh_h1->Scale(1/(float)n_evt_oh1);
		ih_e1->Scale(1/(float)n_evt_ih1);
		ih_h1->Scale(1/(float)n_evt_ih1);
		em_e1->Scale(1/(float)n_evt_em1);
		em_h1->Scale(1/(float)n_evt_em1);
	//	a_e1->Scale(1/(float)n_evt_all1);
	//	a_h1->Scale(1/(float)n_evt_all1);
		oh_e1->GetXaxis()->SetRangeUser(0.1, 4000);
		em_e1->GetXaxis()->SetRangeUser(0.1, 5000);
		ih_e1->GetXaxis()->SetRangeUser(0.1, 1500);*/
/*		if(i==0){
			oh_e->SetLineColor(kGreen);
			oh_e->SetMarkerColor(kGreen);
			oh_h->SetLineColor(kGreen);
			oh_h->SetMarkerColor(kGreen);
			ih_e->SetLineColor(kGreen);
			ih_e->SetMarkerColor(kGreen);
			ih_h->SetLineColor(kGreen);
			ih_h->SetMarkerColor(kGreen);
			em_e->SetLineColor(kGreen);
			em_e->SetMarkerColor(kGreen);
			em_h->SetLineColor(kGreen);
			em_h->SetMarkerColor(kGreen);
			//a_hSetLineColor(kGreen);
			//a_hSetMarkerColor(kGreen);
			//a_hSetLineColor(kGreen);
			//a_hSetMarkerColor(kGreen);
		}
		if(i==1){
			oh_e->SetLineColor(kBlack);
			oh_e->SetMarkerColor(kBlack);
			oh_h->SetLineColor(kBlack);
			oh_h->SetMarkerColor(kBlack);
			ih_e->SetLineColor(kBlack);
			ih_e->SetMarkerColor(kBlack);
			ih_h->SetLineColor(kBlack);
			ih_h->SetMarkerColor(kBlack);
			em_e->SetLineColor(kBlack);
			em_e->SetMarkerColor(kBlack);
			em_h->SetLineColor(kBlack);
			em_h->SetMarkerColor(kBlack);
			//a_hSetLineColor(kBlack);
			//a_hSetMarkerColor(kBlack);
			//a_hSetLineColor(kBlack);
			//a_hSetMarkerColor(kBlack);
		}
		if(i==2){
			oh_e->SetLineColor(kBlue);
			oh_e->SetMarkerColor(kBlue);
			oh_h->SetLineColor(kBlue);
			oh_h->SetMarkerColor(kBlue);
			ih_e->SetLineColor(kBlue);
			ih_e->SetMarkerColor(kBlue);
			ih_h->SetLineColor(kBlue);
			ih_h->SetMarkerColor(kBlue);
			em_e->SetLineColor(kBlue);
			em_e->SetMarkerColor(kBlue);
			em_h->SetLineColor(kBlue);
			em_h->SetMarkerColor(kBlue);
			//a_hSetLineColor(kBlue);
			//a_hSetMarkerColor(kBlue);
			//a_hSetLineColor(kBlue);
			//a_hSetMarkerColor(kBlue);
		}
		if(i==3){
			oh_e->SetLineColor(kRed);
			oh_e->SetMarkerColor(kRed);
			oh_h->SetLineColor(kRed);
			oh_h->SetMarkerColor(kRed);
			ih_e->SetLineColor(kRed);
			ih_e->SetMarkerColor(kRed);
			ih_h->SetLineColor(kRed);
			ih_h->SetMarkerColor(kRed);
			em_e->SetLineColor(kRed);
			em_e->SetMarkerColor(kRed);
			em_h->SetLineColor(kRed);
			em_h->SetMarkerColor(kRed);
			//a_hSetLineColor(kRed);
			//a_hSetMarkerColor(kRed);
			//a_hSetLineColor(kRed);
			//a_hSetMarkerColor(kRed);
		}
		if(i==4){
			oh_e->SetLineColor(kViolet+1);
			oh_e->SetMarkerColor(kViolet+1);
			oh_h->SetLineColor(kViolet+1);
			oh_h->SetMarkerColor(kViolet+1);
			ih_e->SetLineColor(kViolet+1);
			ih_e->SetMarkerColor(kViolet+1);
			ih_h->SetLineColor(kViolet+1);
			ih_h->SetMarkerColor(kViolet+1);
			em_e->SetLineColor(kViolet+1);
			em_e->SetMarkerColor(kViolet+1);
			em_h->SetLineColor(kViolet+1);
			em_h->SetMarkerColor(kViolet+1);
			//a_hSetLineColor(kViolet+1);
			//a_hSetMarkerColor(kViolet+1);
			//a_hSetLineColor(kViolet+1);
			//a_hSetMarkerColor(kViolet+1);
		}
		oh_e->SetMarkerStyle(21);
		oh_h->SetMarkerStyle(21);
		ih_e->SetMarkerStyle(21);
		ih_h->SetMarkerStyle(21);
		em_e->SetMarkerStyle(21);
		em_h->SetMarkerStyle(21);
		//a_hSetMarkerStyle(21);
		//a_hSetMarkerStyle(21);
		oh_e->SetMarkerSize(1.5f);
		oh_h->SetMarkerSize(1.5f);
		ih_e->SetMarkerSize(1.5f);
		ih_h->SetMarkerSize(1.5f);
		em_e->SetMarkerSize(1.5f);
		em_h->SetMarkerSize(1.5f);
		//a_hSetMarkerSize(1.5f);
		//a_hSetMarkerSize(1.5f);
		oh_e->SetYTitle("Ratio to Collision Data");
		oh_h->SetYTitle("Ratio to Collision Data");
		ih_e->SetYTitle("Ratio to Collision Data");
		ih_h->SetYTitle("Ratio to Collision Data");
		em_e->SetYTitle("Ratio to Collision Data");
		em_h->SetYTitle("Ratio to Collision Data");
		//a_hSetYTitle("Ratio to Collision Data");
		//a_hSetYTitle("Ratio to Collision Data");
	
	
		oh_e->Divide(oh_e1);
		oh_h->Divide(oh_h1);
		ih_e->Divide(ih_e1);
		ih_h->Divide(ih_h1);
		em_e->Divide(em_e1);
		em_h->Divide(em_h1);
		//a_hDivide(a_e1);
		//a_hDivide(a_h1);
		rats.push_back(std::array<TH1F*, 6> {oh_e, oh_h, ih_e, ih_h, em_e, em_h, a_h, a_e*);	
		if(oh_e->GetMaximum() > maxes[0]) maxes[0]=oh_e->GetMaximum();
		if(oh_h->GetMaximum() > maxes[1]) maxes[1]=oh_h->GetMaximum();
		if(ih_e->GetMaximum() > maxes[2]) maxes[2]=ih_e->GetMaximum();
		if(ih_h->GetMaximum() > maxes[3]) maxes[3]=ih_h->GetMaximum();
		if(em_e->GetMaximum() > maxes[4]) maxes[4]=em_e->GetMaximum();
		if(em_h->GetMaximum() > maxes[5]) maxes[5]=em_h->GetMaximum();*/
//	}
	for(int i=0; i<(int)files.size(); i++)
	{
		if(i==0 || i== 4) continue;
		TFile* f=files[i];
		TH1F* oh_e = (TH1F*) f->Get("ohcal_energy");
		TH1F* oh_h = (TH1F*) f->Get("ohc_hit");
		TH1F* ih_e = (TH1F*) f->Get("ihcal_energy");
		TH1F* ih_h = (TH1F*) f->Get("ihc_hit");
		TH1F* em_e = (TH1F*) f->Get("emcal_energy");
		TH1F* em_h = (TH1F*) f->Get("emc_hit");
		TH1F* a_e = (TH1F*) f->Get("allcal_energy");
		TH1F* a_h = (TH1F*) f->Get("amc_hit");
		TH1F* oh_Et = (TH1F*) f->Get("ohcal_total_E");
		TH1F* ih_Et = (TH1F*) f->Get("ihcal_total_E");
		TH1F* em_Et = (TH1F*) f->Get("emcal_total_E");
		TH1F* a_Et = (TH1F*) f->Get("allcal_total_E");
		int n_evt_oh=oh_Et->GetEntries();
		int n_evt_ih=ih_Et->GetEntries();
		int n_evt_em=em_Et->GetEntries();
		int n_evt_a=a_Et->GetEntries();
		
		
		//set the y title to reflect the actual plot
		oh_e->SetYTitle(Form("#frac{1}{N_{events}} %s ", oh_e->GetYaxis()->GetTitle()));
		oh_h->SetYTitle(Form("#frac{1}{N_{events}} %s) ", oh_h->GetYaxis()->GetTitle()));
		ih_e->SetYTitle(Form("#frac{1}{N_{events}} %s ", ih_e->GetYaxis()->GetTitle()));
		ih_h->SetYTitle(Form("#frac{1}{N_{events}} %s) ", ih_h->GetYaxis()->GetTitle()));
		em_e->SetYTitle(Form("#frac{1}{N_{events}} %s ", em_e->GetYaxis()->GetTitle()));
		em_h->SetYTitle(Form("#frac{1}{N_{events}} %s) ", em_h->GetYaxis()->GetTitle()));
		a_e->SetYTitle(Form("#frac{1}{N_{events}} %s ", a_e->GetYaxis()->GetTitle()));
		a_h->SetYTitle(Form("#frac{1}{N_{events}} %s) ", a_h->GetYaxis()->GetTitle()));
		//set the limits 
		oh_e->GetYaxis()->SetRangeUser(0.08, 500);
		em_e->GetYaxis()->SetRangeUser(0.08, 25000);
		ih_e->GetYaxis()->SetRangeUser(0.08, 500);
		a_e->GetYaxis()->SetRangeUser(0.08, 1600);
		a_h->GetYaxis()->SetRangeUser(0.08, 1600);
		oh_h->GetYaxis()->SetRangeUser(0.08, 1600);
		em_h->GetYaxis()->SetRangeUser(0.08, 25000);
		ih_h->GetYaxis()->SetRangeUser(0.08, 1600);
		oh_h->GetXaxis()->SetRangeUser(0.01, 500);
		ih_h->GetXaxis()->SetRangeUser(0.01, 500);
		oh_e->GetXaxis()->SetRangeUser(-100, 600);
		ih_e->GetXaxis()->SetRangeUser(-100, 600);
		em_e->GetXaxis()->SetRangeUser(-300, 1000);
		if(i==0){
			oh_e->SetLineColor(kGreen);
			oh_e->SetMarkerColor(kGreen);
			oh_h->SetLineColor(kGreen);
			oh_h->SetMarkerColor(kGreen);
			ih_e->SetLineColor(kGreen);
			ih_e->SetMarkerColor(kGreen);
			ih_h->SetLineColor(kGreen);
			ih_h->SetMarkerColor(kGreen);
			em_e->SetLineColor(kGreen);
			em_e->SetMarkerColor(kGreen);
			em_h->SetLineColor(kGreen);
			em_h->SetMarkerColor(kGreen);
			a_h->SetLineColor(kGreen);
			a_h->SetMarkerColor(kGreen);
			a_h->SetLineColor(kGreen);
			a_h->SetMarkerColor(kGreen);
			
		}

		if(i==4){
			oh_e->SetLineColor(kBlack);
			oh_e->SetMarkerColor(kBlack);
			oh_h->SetLineColor(kBlack);
			oh_h->SetMarkerColor(kBlack);
			ih_e->SetLineColor(kBlack);
			ih_e->SetMarkerColor(kBlack);
			ih_h->SetLineColor(kBlack);
			ih_h->SetMarkerColor(kBlack);
			em_e->SetLineColor(kBlack);
			em_e->SetMarkerColor(kBlack);
			em_h->SetLineColor(kBlack);
			em_h->SetMarkerColor(kBlack);
			a_h->SetLineColor(kBlack);
			a_h->SetMarkerColor(kBlack);
			a_h->SetLineColor(kBlack);
			a_h->SetMarkerColor(kBlack);
		}
		if(i==2){
			oh_e->SetLineColor(kBlue);
			oh_e->SetMarkerColor(kBlue);
			oh_h->SetLineColor(kBlue);
			oh_h->SetMarkerColor(kBlue);
			ih_e->SetLineColor(kBlue);
			ih_e->SetMarkerColor(kBlue);
			ih_h->SetLineColor(kBlue);
			ih_h->SetMarkerColor(kBlue);
			em_e->SetLineColor(kBlue);
			em_e->SetMarkerColor(kBlue);
			em_h->SetLineColor(kBlue);
			em_h->SetMarkerColor(kBlue);
			a_e->SetLineColor(kBlue);
			a_e->SetMarkerColor(kBlue);
			a_h->SetLineColor(kBlue);
			a_h->SetMarkerColor(kBlue);
		}
		if(i==3){
			oh_e->SetLineColor(kRed);
			oh_e->SetMarkerColor(kRed);
			oh_h->SetLineColor(kRed);
			oh_h->SetMarkerColor(kRed);
			ih_e->SetLineColor(kRed);
			ih_e->SetMarkerColor(kRed);
			ih_h->SetLineColor(kRed);
			ih_h->SetMarkerColor(kRed);
			em_e->SetLineColor(kRed);
			em_e->SetMarkerColor(kRed);
			em_h->SetLineColor(kRed);
			em_h->SetMarkerColor(kRed);
			a_e->SetLineColor(kRed);
			a_e->SetMarkerColor(kRed);
			a_h->SetLineColor(kRed);
			a_h->SetMarkerColor(kRed);
		}
		if(i==1){
			oh_e->SetLineColor(kViolet+1);
			oh_e->SetMarkerColor(kViolet+1);
			oh_h->SetLineColor(kViolet+1);
			oh_h->SetMarkerColor(kViolet+1);
			ih_e->SetLineColor(kViolet+1);
			ih_e->SetMarkerColor(kViolet+1);
			ih_h->SetLineColor(kViolet+1);
			ih_h->SetMarkerColor(kViolet+1);
			em_e->SetLineColor(kViolet+1);
			em_e->SetMarkerColor(kViolet+1);
			em_h->SetLineColor(kViolet+1);
			em_h->SetMarkerColor(kViolet+1);
			a_e->SetLineColor(kViolet+1);
			a_e->SetMarkerColor(kViolet+1);
			a_h->SetLineColor(kViolet+1);
			a_h->SetMarkerColor(kViolet+1);
		}
	/*	TH1F* ih_hg = regeneratePlot(ih_e);	
		TH1F* oh_hg = regeneratePlot(oh_e);
		TH1F* em_hg = regeneratePlot(em_e);*/
		lt[0]->AddEntry(oh_e, Form("%s", label[i].c_str()));
		//lt_ref[0]->AddEntry(oh_e, Form("%s", label[i].c_str()));
		//lt_ref[0]->AddEntry(oh_e_ref, Form("%s", label[i].c_str()));
		lt[1]->AddEntry(oh_h, Form("%s", label[i].c_str()));
		//lt_ref[1]->AddEntry(oh_e, Form("%s", label[i].c_str()));
		//lt_ref[1]->AddEntry(oh_e_ref, Form("%s", label[i].c_str()));
		lt[2]->AddEntry(ih_e, Form("%s", label[i].c_str()));
		//lt_ref[2]->AddEntry(oh_e, Form("%s", label[i].c_str()));
		//lt_ref[2]->AddEntry(oh_e_ref, Form("%s", label[i].c_str()));
		lt[3]->AddEntry(ih_h, Form("%s", label[i].c_str()));
		//lt_ref[3]->AddEntry(oh_e, Form("%s", label[i].c_str()));
		//lt_ref[3]->AddEntry(oh_e_ref, Form("%s", label[i].c_str()));
		lt[4]->AddEntry(em_e, Form("%s", label[i].c_str()));
		//lt_ref[4]->AddEntry(oh_e, Form("%s", label[i].c_str()));
		//lt_ref[4]->AddEntry(oh_e_ref, Form("%s", label[i].c_str()));
		lt[5]->AddEntry(em_h, Form("%s", label[i].c_str()));
		//lt_ref[5]->AddEntry(oh_e, Form("%s", label[i].c_str()));
		//lt_ref[5]->AddEntry(oh_e_ref, Form("%s", label[i].c_str()));
		lt[6]->AddEntry(a_e, Form("%s", label[i].c_str()));
		//lt_ref[6]->AddEntry(oh_e, Form("%s", label[i].c_str()));
		//lt_ref[6]->AddEntry(oh_e_ref, Form("%s", label[i].c_str()));
		lt[7]->AddEntry(a_h, Form("%s", label[i].c_str()));
		//lt_ref[7]->AddEntry(oh_e, Form("%s", label[i].c_str()));
		//lt_ref[7]->AddEntry(oh_e_ref, Form("%s", label[i].c_str()));
		//scale by total number of events
		if(i==1){ 
			TCanvas* co=new TCanvas("c_ohcal", "data_ohcal");
			TCanvas* ci=new TCanvas("c_ihcal", "data_ihcal");
			TCanvas* ce=new TCanvas("c_emcal", "data_emcal");;
			co->cd();
			TLegend* lb1=(TLegend*)ll[0]->Clone();
			lb1->Draw();
			ci->cd();
			TLegend* lb2=(TLegend*)ll[2]->Clone();
			lb2->Draw();
			ce->cd();
			TLegend* lb3=(TLegend*)ll[4]->Clone();
			lb3->Draw();

			TLegend* lo=(TLegend*)lt[0]->Clone();
			TLegend* li=(TLegend*)lt[2]->Clone();
			TLegend* le=(TLegend*)lt[4]->Clone();
			TH1F* oh_e2=(TH1F*)oh_e->Clone();
			TH1F* ih_e2=(TH1F*)ih_e->Clone();
			TH1F* em_e2=(TH1F*)em_e->Clone();
			double sigma_o=fitTheNegSide(oh_e2, co, lo, lb1, n_evt_oh, 2);
			double sigma_i=fitTheNegSide(ih_e2, ci, li, lb2, n_evt_ih, 1);
			double sigma_e=fitTheNegSide(em_e2, ce, le, lb3, n_evt_em, 0);
		}
		oh_e->Rebin(4);
		oh_e->Scale(1/((float)n_evt_oh*4.0));
		oh_h->Scale(1/(float)n_evt_oh);
		//oh_hg->Scale(1/(float)n_evt_oh);
		ih_e->Scale(1/(float)n_evt_ih);
		ih_h->Scale(1/(float)n_evt_ih);
		//ih_hg->Scale(1/(float)n_evt_ih);
		em_e->Scale(1/(float)n_evt_em);
		em_h->Scale(1/(float)n_evt_em);
		//em_hg->Scale(1/(float)n_evt_em);
		a_e->Scale(1/(float)n_evt_a);
		a_h->Scale(1/(float)n_evt_a);

		//get the reflected part of the energy 
	/*	TH1F* oh_e_ref=getReflected(oh_e);
		TH1F* ih_e_ref=getReflected(ih_e);
		TH1F* em_e_ref=getReflected(em_e);
		TH1F* a_e_ref=getReflected(a_e);
		
		oh_e_ref->SetMarkerStyle(21);
		ih_e_ref->SetMarkerStyle(21);
		em_e_ref->SetMarkerStyle(21);
		a_e_ref->SetMarkerStyle(21);
	*/
/*		if(i==0){
			lt[0]->AddEntry(rats[i][0], "Pedestal / Collision");
			lt[1]->AddEntry(rats[i][1], "Pedestal / Collision");
			lt[2]->AddEntry(rats[i][2], "Pedestal / Collision");
			lt[3]->AddEntry(rats[i][3], "Pedestal / Collision");
			lt[4]->AddEntry(rats[i][4], "Pedestal / Collision");
			lt[5]->AddEntry(rats[i][5], "Pedestal / Collision");
			//lt[6]->AddEntry(rats[i][7], "Pedestal / Collision");
			//lt[7]->AddEntry(rats[i][7], "Pedestal / Collision");
		}
		else if(i==1)
		{
			TH1F* h_null=new TH1F("h", "h", oh_e->GetNbinsX(), -0.5, 4999.5);
			for(int j=0; j<5000; j++) h_null->Fill(j);
			h_null->SetLineColor(kRed);
			for(int j=0; j<8; j++)
			{
				pad_rats[j]->cd();
				h_null->Draw();
			}
		}
		else{
			lt[0]->AddEntry(rats[i][0], Form("%s / Collision", label[i].c_str()));
			lt[1]->AddEntry(rats[i][1], Form("%s / Collision", label[i].c_str()));
			lt[2]->AddEntry(rats[i][2], Form("%s / Collision", label[i].c_str()));
			lt[3]->AddEntry(rats[i][3], Form("%s / Collision", label[i].c_str()));
			lt[4]->AddEntry(rats[i][4], Form("%s / Collision", label[i].c_str()));
			lt[5]->AddEntry(rats[i][5], Form("%s / Collision", label[i].c_str()));
			//lt[6]->AddEntry(rats[i][6], Form("%s / Collision", label[i].c_str()));
			//lt[7]->AddEntry(rats[i][7], Form("%s / Collision", label[i].c_str()));
		}*/
		Canvases[0]->cd();
		//pad_val[0]->cd();
		oh_e->Draw("same");
	//	Canvases[0]->cd();
	//	pad_val[0]->Draw();
	//	pad_rats[0]->cd();
	//	rats[i][0]->Draw();
	//	Canvases[0]->cd();
	//	pad_rats[0]->Draw("same");
		
		Canvases[1]->cd();
		oh_h->Draw("same");
	/*	Canvases[1]->cd();
		pad_val[1]->Draw();
		pad_rats[1]->cd();
		rats[i][1]->Draw("same");
		
		Canvases[1]->cd();
		pad_rats[1]->Draw();
*/
		Canvases[2]->cd();
		ih_e->Draw("same");
/*		Canvases[2]->cd();
		pad_val[2]->Draw();
		pad_rats[2]->cd();
		rats[i][2]->Draw("same");
		Canvases[2]->cd();
		pad_rats[2]->Draw();
*/		
		Canvases[3]->cd();
		ih_h->Draw("same");
/*		Canvases[3]->cd();
		pad_val[3]->Draw();
		pad_rats[3]->cd();
		rats[i][3]->Draw("same");
		Canvases[3]->cd();
		pad_rats[3]->Draw();
*/		
		Canvases[4]->cd();
		em_e->Draw("same");
/*		Canvases[4]->cd();
		pad_val[4]->Draw();
		pad_rats[4]->cd();
		rats[i][4]->Draw("same");
		Canvases[4]->cd();
		pad_rats[4]->Draw();
*/		
		Canvases[5]->cd();
		em_h->Draw("same");	
/*		Canvases[5]->cd();
		pad_val[5]->Draw();
		pad_rats[5]->cd();
		rats[i][5]->Draw("same");
		Canvases[5]->cd();
		pad_rats[5]->Draw();
*/		Canvases[6]->cd();
		a_e->Draw("same");	
	/*	Canvases[6]->cd();
		pad_val[6]->Draw();
		pad_rats[6]->cd();
		rats[i][6]->Draw("same");
		Canvases[6]->cd();
		pad_rats[6]->Draw();
	*/	Canvases[7]->cd();
		a_h->Draw("same");	
	/*	Canvases[7]->cd();
		pad_val[7]->Draw();
		pad_rats[7]->cd();
		rats[i][7]->Draw("same");
		Canvases[7]->cd();
		pad_rats[7]->Draw();*/
	/*	Cr[0]->cd();
		oh_e_ref->Draw("same");	
		oh_e->Draw("same");
		Cr[2]->cd();
		ih_e_ref->Draw("same");	
		ih_e->Draw("same");
		Cr[4]->cd();
		em_e_ref->Draw("same");	
		em_e->Draw("same");
		
		Cr[1]->cd();
		//oh_hg->Draw("same");	
		
		Cr[3]->cd();
		//ih_hg->Draw("same");	
		
		Cr[5]->cd();*/
		//em_hg->Draw("same");	
		
	}
	for(int i=0; i<(int)Canvases.size(); i++)
	{
		Canvases[i]->cd();
		lt[i]->Draw();
		ll[i]->Draw();
//		Canvases[i]->Print(Form("~/Hit_energy_canvas_%d.pdf", i));
	}
/*	for(int i= 0; i<(int) Cr.size(); i++) 
	{
		Cr[i]->cd();
		lt[i]->Draw();
		if(i % 2 == 0 ) //lt_ref[i]->Draw();
		else ll[i]->Draw();
	}*/
	return 0;
}
#endif

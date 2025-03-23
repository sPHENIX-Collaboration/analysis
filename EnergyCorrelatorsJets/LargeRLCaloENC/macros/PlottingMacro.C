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
		if(hist_name.find("E_")!=std::string::npos)	RegionPlots->at(0)->at(Calo)=(TH1F*)botdir->Get(hist_name.c_str());
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
			if(n_evts[i]==0) n_evts[i]=1;
			p->Scale(1/((float) n_evts[i]));
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
	
	std::vector<TH1F*>* FullEnergy, *FullE2C, *FullE3C, *FullR, *FullN;
	std::vector<TH1F*>* TowardsEnergy, *TowardsE2C, *TowardsE3C, *TowardsR, *TowardsN;
	std::vector<TH1F*>* AwayEnergy, *AwayE2C, *AwayE3C, *AwayR, *AwayN;
	std::vector<TH1F*>* TransEnergy, *TransE2C, *TransE3C, *TransR, *TransN;
	
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
	
	TDirectory* topdir=file_dir_top[threshold_index];
	topdir->cd();
	for(int i=0; i<(int)lower_dirs[threshold_index].size(); i++){
		TDirectory* botdir=lower_dirs[threshold_index][i];
		botdir->cd();
		std::string dir_name=botdir->GetName();
		std::vector<std::vector<TH1F*>*>* rp;
		if(dir_name.find("Full") != std::string::npos) rp=Full;
		else if(dir_name.find("Towards") != std::string::npos ) rp=Towards;	
		else if(dir_name.find("Trans") != std::string::npos ) rp=Trans;	
		else if(dir_name.find("Away") != std::string::npos ) rp=Away;	
		LoadInRegionPlots(botdir, rp);
	}
	RegPlts->at(0)=Full;
	RegPlts->at(1)=Towards; 
	RegPlts->at(2)=Away;
	RegPlts->at(3)=Trans;
	return;
}

std::array<std::vector<std::vector<TH1F*>*>*, 4> DrawOneThreshold(TFile* datafile, std::array<std::vector<TPad*>*, 4>* p1, std::array<std::vector<TLegend*>*, 4>* l1, int threshold_index, /*choose which of the set of indicies we use, rather thas spefcific value*/ int style)
{
	//p1 is {full, towards, away} {E, E2C, E3C, R, N}
	std::array<std::vector<std::vector<TH1F*>*>*, 4> RegPlts;
	LoadInPlots(threshold_index, &RegPlts);
	//Plots are structured as {full, towards, away, transverse} and one level down {E, E2C, E3C, R, N} then {EMCAL, IHCAL, OHCAL}
	int offset=0;
	std::string run_type="";
	if(style==0) run_type="Pedestal";
	if(style==1) run_type="Pulser";
	if(style==2) run_type="LED";
	std::array<std::vector<std::vector<std::string>>, 4> plot_label;
	plot_label=GetPlotLabels(RegPlts);
	if(threshold_index == 3 ) offset = 6;
	else if (threshold_index > 3) offset = 9; //keep the marker shapes distinguisable
	else if (threshold_index > 5) offset= 17;
	for(int i=0; i<(int) p1->size(); i++){
		for(int j=0; j<(int)p1->at(i)->size(); j++)
		{
			p1->at(i)->at(j)->cd();
			if(style== 0){
				RegPlts[i]->at(j)->at(0)->SetLineColor(kGreen);
				RegPlts[i]->at(j)->at(0)->SetMarkerColor(kGreen);
			}
			if(style== 1){
				RegPlts[i]->at(j)->at(0)->SetLineColor(kTeal+3);
				RegPlts[i]->at(j)->at(0)->SetMarkerColor(kTeal+3);
			}
			if(style== 0){
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
			std::vector<std::vector<TH1F*>*>* pd_r, *pl_r, *ld_r;
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
	std::array<std::vector<TLegend*>*, 4> LegendTitles, PlotLeg;
	std::array<std::vector<TPad*>*, 4> p_main, p_ratio;
	std::array<std::vector<TCanvas*>*, 4> Canvases;
	for(int i= 0; i<4; i++)
	{
		std::string region;
		std::vector<TLegend*> *LT, *PL;
		std::vector<TPad*> *P1, *P2;
		std::vector<TCanvas*> *cs;
		if(i==0) region="Full Calorimeter";
		if(i==1) region="Lead Region";
		if(i==2) region="Away Region";
		if(i==3) region="Transverse Regions";
		for(int j=0; j < 5; j++)
		{
			TCanvas* c1=new TCanvas(Form("c_%d_%d", i, j), Form("c_%d_%d", i, j));
			c1->cd();
			float ymin=0.33;
			if( j == 0 || j == 4) ymin=0; //no need for the ratio plots in E and N
			TPad* p1=new TPad(Form("p_1_%d_%d", i, j), Form("p_1_%d_%d", i, j), 0, ymin, 1, 1);
			TPad* p2=new TPad(Form("p_2_%d_%d", i, j), Form("p_2_%d_%d", i, j), 0, 0, 1, 0.3);
			
			TLegend* l1=new TLegend(0.7, 0.5, 0.9, 0.95);
			l1->SetFillStyle(0);
			l1->SetFillColor(0);
			l1->SetBorderSize(0);
			l1->SetTextSize(0.04f);
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
	}
	std::vector<std::map<std::string, std::array<std::vector<std::vector<TH1F*>*>*, 4>> > threshold_maps;
	for(int i=0; i<7; i++)
	{
		std::map<std::string, std::array<std::vector<std::vector<TH1F*>*>*, 4>> tm;
		std::array<std::vector<std::vector<TH1F*>*>*, 4> rg_1=DrawOneThreshold(	Pedestal_data, &p_main, &PlotLeg, i, 0);
		tm["Pedestal"]=rg_1;
		std::array<std::vector<std::vector<TH1F*>*>*, 4> rg_2=DrawOneThreshold(	Pulser_data, &p_main, &PlotLeg, i, 0);
		tm["Pulser"]=rg_2;
		if(include_led){
			std::array<std::vector<std::vector<TH1F*>*>*, 4> rg_3=DrawOneThreshold(	LED_data, &p_main, &PlotLeg, i, 0);
			tm["LED"]=rg_3;
		}
		threshold_maps.push_back(tm);
	}
	PlotRatios(threshold_maps, &p_ratio, &PlotLeg);
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
#endif

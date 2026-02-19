#pragma once 
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <TFile.h>
#include <TDirectory.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include "HerwigQAPlottingConfig.h"
#include "HerwigQAPlottongCongig.cc"
#include "SpecialColors.h"
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

#include <string>
#include <vector>
#include <format>

HerwigQAPlottingConfig* conf;
Skaydis_colors* style_points;
void DoAllThePlotting(TFile* herwig_file, TFile* pythia_file, std::string trigger_tag="Jet30", float herwig_xs=1., float pythia_xs=1.)
{
	//this does all the plotting of Pythia vrs herwig
	SetsPhenixStyle();	
	conf =  new HerwigQAPlottingConfig(herwig_xs, pythia_xs);
	style_points=new Skaydis_colors();
	gStyle->SetPalette(100, style_points->Lesbian_gradient_PT);	
	conf->ExtractType(herwig_file);
	std::map<std::string, TDirectory*> top_dirs;
	std::vector<TCanvas*>* Canvi =new std::vector<TCanvas*> ();
	//find the relevant directories
	if(conf->isJet()){
		TDirectory* HJets=(TDirectory*)herwig_file->GetDirectory("Jets");
		TDirectory* PJets=(TDirectory*)pythia_file->GetDirectory("Jets");
		top_dirs["herwig jets"]=HJets;
		top_dirs["pythia jets"]=PJets;
		PlotJetPlots(HJets, PJets, Canvi);
	}
	if(conf->isPhoton())
	{
		TDirectory* HPh=(TDirectory*)herwig_file->GetDirectory("Photons");
		TDirectory* PPh=(TDirectory*)pythia_file->GetDirectory("Photons");
		top_dirs["herwig photons"]=HPh;
		top_dirs["pythia photons"]=PPh;
		PlotPhotonPlots(HPh, PPh, Canvi);
	}
	if(conf->isPhoton && conf->isJet)
	{
		PlotJetPhotonPlots(top_dirs["herwig photons"], top_dirs["pythia photons"], Canvi);
	}

	TDirectory* HEvent=(TDirectory*)herwig_file->GetDirectory("Event");
	TDirectory* PEvent=(TDirectory*)pythia_file->GetDirectory("Event");
	top_dirs["herwig event"]=HEvent;
	top_dirs["pythia event"]=PEvent;	
	PlotEventPlots(HEvent, PEvent, Canvi);
	conf->DumpHistos(std::format("~/{}_output.pdf", trigger_tag), Canvi);
}

void PlotJetPlots(TDirectory* herwig_jets, TDirectory* pythia_jets, std::vector<TCanvas*>*Canvi)
{
	//all Jets
	std::vector<TH1F*>* herwig_pt		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_e		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_phi		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_eta		= new std::vector<TH1F*> ();
	std::vector<TH1I*>* herwig_n_jets	= new std::vector<TH1I*> ();
	std::vector<TH1I*>* herwig_n_comp	= new std::vector<TH1I*> ();
	std::array<std::vector<TH1F*>*, 4>* herwig_jet_array =new std::array<std::vector<TH1F*>*, 4>();
	herwig_jet_array->at(0)=herwig_pt;
	herwig_jet_array->at(1)=herwig_e;
	herwig_jet_array->at(2)=herwig_phi;
	herwig_jet_array->at(3)=herwig_eta;
	//lead Jets
	std::vector<TH1F*>* herwig_lead_pt	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_lead_e	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_lead_phi	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_lead_eta	= new std::vector<TH1F*> ();
	std::vector<TH1I*>* herwig_lead_n_comp	= new std::vector<TH1I*> ();
	std::array<std::vector<TH1F*>*, 4>* herwig_lead_jet_array =new std::array<std::vector<TH1F*>*, 4>();
	herwig_lead_jet_array->at(0)=herwig_lead_pt;
	herwig_lead_jet_array->at(1)=herwig_lead_e;
	herwig_lead_jet_array->at(2)=herwig_lead_phi;
	herwig_lead_jet_array->at(3)=herwig_lead_eta;
	CollectJets(herwig_jets, herwig_jet_array, herwig_lead_jet_array, herwig_n_jets, heriwg_n_comp, herwig_lead_n_comp);
	//all Jets
	std::vector<TH1F*>* pythia_pt		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_e		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_phi		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_eta		= new std::vector<TH1F*> ();
	std::vector<TH1I*>* pythia_n_jets	= new std::vector<TH1I*> ();
	std::vector<TH1I*>* pythia_n_comp	= new std::vector<TH1I*> ();
	std::array<std::vector<TH1F*>*, 4>* pythia_jet_array =new std::array<std::vector<TH1F*>*, 4>();
	pythia_jet_array->at(0)=pythia_pt;
	pythia_jet_array->at(1)=pythia_e;
	pythia_jet_array->at(2)=pythia_phi;
	pythia_jet_array->at(3)=pythia_eta;
	//lead Jets
	std::vector<TH1F*>* pythia_lead_pt	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_lead_e	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_lead_phi	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_lead_eta	= new std::vector<TH1F*> ();
	std::vector<TH1I*>* pythia_lead_n_comp	= new std::vector<TH1I*> ();
	std::array<std::vector<TH1F*>*, 4>* pythia_lead_jet_array =new std::array<std::vector<TH1F*>*, 4>();
	pythia_lead_jet_array->at(0)=pythia_lead_pt;
	pythia_lead_jet_array->at(1)=pythia_lead_e;
	pythia_lead_jet_array->at(2)=pythia_lead_phi;
	pythia_lead_jet_array->at(3)=pythia_lead_eta;
	CollectJets(pythia_jets, pythia_jet_array, pythia_lead_jet_array, pythia_n_jets, heriwg_n_comp, pythia_lead_n_comp);
	TCanvas* J_pt=new TCanvas("jet_pt", "jet_pt");
	PlotJetObs(herwig_jet_array->at(0), pythia_jet_array->at(0), J_pt);
	TCanvas* J_l_pt=new TCanvas("jet_l_pt", "jet_l_pt");
	PlotJetObs(herwig_lead_jet_array->at(0), pythia_lead_jet_array->at(0), J_l_pt);
	TCanvas* J_e=new TCanvas("jet_e", "jet_e");
	PlotJetObs(herwig_jet_array->at(1), pythia_jet_array->at(1), J_e);
	TCanvas* J_l_e=new TCanvas("jet_l_e", "jet_l_e");
	PlotJetObs(herwig_lead_jet_array->at(1), pythia_lead_jet_array->at(1), J_l_e);
	TCanvas* J_phi=new TCanvas("jet_phi", "jet_phi");
	PlotJetObs(herwig_jet_array->at(2), pythia_jet_array->at(2), J_phi);
	TCanvas* J_l_phi=new TCanvas("jet_l_pt", "jet_l_pt");
	PlotJetObs(herwig_lead_jet_array->at(2), pythia_lead_jet_array->at(2), J_l_phi);
	TCanvas* J_eta=new TCanvas("jet_eta", "jet_eta");
	PlotJetObs(herwig_jet_array->at(3), pythia_jet_array->at(3), J_eta);
	TCanvas* J_l_eta=new TCanvas("jet_l_eta", "jet_l_eta");
	PlotJetObs(herwig_lead_jet_array->at(3), pythia_lead_jet_array->at(3), J_l_eta);
	TCanvas* J_n_comp=new TCanvas("jet_n_comp", "jet_n_comp");
	PlotJetObs(herwig_n_comp, pythia_n_comp, J_n_comp);
	TCanvas* J_l_n_comp=new TCanvas("jet_l_n_comp", "jet_l_n_comp");
	PlotJetObs(herwig_lead_n_comp, pythia_lead_n_comp, J_l_n_comp);
	TCanvas* J_N=new TCanvas("jet_N", "jet_N");
	PlotJetObs(herwig_n_jets, pythia_n_jets, J_N);

	Canvi->push_back(J_pt);
	Canvi->push_back(J_l_pt);
	Canvi->push_back(J_e);
	Canvi->push_back(J_l_e);
	Canvi->push_back(J_phi);
	Canvi->push_back(J_l_phi);
	Canvi->push_back(J_eta);
	Canvi->push_back(J_l_eta);
	Canvi->push_back(J_n_comp);
	Canvi->push_back(J_l_n_comp);
	Canvi->push_bacl(J_N);
	return;
	
}
void CollectJets(TDirectory* jets, std::array<std::vector<TH1F*>*, 4>* all_jets, std::array<std::vector<TH1F*>*, 4>* lead_jets, std::vector<TH1F*>* all_jet_n, std::vector<TH1F*>* all_jet_comp, std::vector<TH1F*>* lead_jet_comp)
{
	for(int i=2; i<7; i++)
	{
		jets->cd();
		TDirectory* rdir=(TDirectory*)_jets->GetDirectory(std::format("R0{}Jets", i).c_str());
		rdir->cd();
		all_jets->at(0)->push_back((TH1F*)r_dir->Get(std::format("h_jet_r0{}_pt", i).c_str());
		all_jets->at(1)->push_back((TH1F*)r_dir->Get(std::format("h_jet_r0{}_e", i).c_str());
		all_jets->at(2)->push_back((TH1F*)r_dir->Get(std::format("h_jet_r0{}_phi", i).c_str());
		all_jets->at(3)->push_back((TH1F*)r_dir->Get(std::format("h_jet_r0{}_eta", i).c_str());
		all_jet_n->push_back((TH1I*)r_dir->Get(std::format("h_jet_r0{}_n",i).c_str()));
		all_jet_comp->push_back((TH1I*)r_dir->Get(std::format("h_jet_r0{}_comp",i).c_str()));
		TDirectory* leaddir=(TDirectory*)r_dir->GetDirectory("Lead_Jets");
		lead_jets->at(0)->push_back((TH1F*)leaddir->Get(std::format("h_lead_jet_r0{}_pt", i).c_str());
		lead_jets->at(1)->push_back((TH1F*)leaddir->Get(std::format("h_lead_jet_r0{}_e", i).c_str());
		lead_jets->at(2)->push_back((TH1F*)leaddir->Get(std::format("h_lead_jet_r0{}_phi", i).c_str());
		lead_jets->at(3)->push_back((TH1F*)leaddir->Get(std::format("h_lead_jet_r0{}_eta", i).c_str());
		lead_jet_comp->push_back((TH1I*)leaddir->Get(std::format("h_lead_jet_r0{}_comp",i).c_str()));
		rdir->cd();
	}	
	return;

}
void PlotJetObs(std::vector<TH1F*>* herwig_obs, std::vector<TH1F*>* pythia_obs, TCanvas* obs_canv)
{
	//just does the plotting of one variable at a time
	TLegend* l_data=new TLegend(0.6, 0.3, 1, 0.5);
	TLegend* l_header=new TLegend(0.8, 0.6, 1, 1);
	conf->SetsPhenixHeaderLegend(l_header);
	l_header->AddEntry("", std::format("Jet {}", herwig_obs->at(0)->GetXTitle()).c_str(), "");
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	std::vector<TPad*>* pads=conf->AddPads(obs_canv);
	ScaleXS(herwig_obs, true);
	ScaleXS(pythia_obs, false);
	for(int i=0; i<(int)herwig_obs->size(); i++)
	{
		pads->at(0)->cd();
		herwig_obs->at(i)->SetLineColor(i*15+1);
		herwig_obs->at(i)->SetMarkerColor(i*15+1);
		herwig_obs->at(i)->SetMarkerStyle(i+20);
		herwig_obs->at(i)->SetMarkerSize(3);
		if(i==5) herwig_obs->at(i)->SetMarkerStyle(33);
		else if(i==6) herwig_obs->at(i)->SetMarkerStyle(34);
		herwig_obs->at(i)->Draw("same e1");
		pythia_obs->at(i)->SetLineColor(i*15+1);
		pythia_obs->at(i)->SetMarkerColor(i*15+1);
		pythia_obs->at(i)->SetMarkerStyle(i+24);
		pythia_obs->at(i)->SetMarkerSize(3);
		pythia_obs->at(i)->Draw("same e1");
		l_data->AddEntry(herwig_obs->at(i), std::format("Herwig {}", herwig_obs->at(i)->GetTitle()).c_str());
		l_data->AddEntry(pythia_obs->at(i), std::format("Pythia {}", pythia_obs->at(i)->GetTitle()).c_str());
		pads->at(1)->cd();
		TH1F* h_ratio = conf->GetRatioPlot(herwig_obs->at(i), pythia_obs->at(i));
		h_ratio->SetMarkerStyle(2*i+37);
		h_ratio->Draw("same");
		l_data->AddEntry(h_ratio);
	}
	pads->at(0)->cd();
	l_data->Draw();
	return;
}
void PlotJetObs(std::vector<TH1I*>* herwig_obs, std::vector<TH1I*>* pythia_obs, TCanvas* obs_canv)
{
	//just does the plotting of one variable at a time
	TLegend* l_data=new TLegend(0.6, 0.3, 1, 0.5);
	TLegend* l_header=new TLegend(0.8, 0.6, 1, 1);
	conf->SetsPhenixHeaderLegend(l_header);
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	std::vector<TPad*>* pads=conf->AddPads(obs_canv);
	ScaleXS(herwig_obs, true);
	ScaleXS(pythia_obs, false);
	for(int i=0; i<(int)herwig_obs->size(); i++)
	{
		pads->at(0)->cd();
		herwig_obs->at(i)->SetLineColor((i-2)*15+1);
		herwig_obs->at(i)->SetMarkerColor((i-2)*15+1);
		herwig_obs->at(i)->SetMarkerStyle(i+18);
		herwig_obs->at(i)->SetMarkerSize(3);
		if(i==5) herwig_obs->at(i)->SetMarkerStyle(33);
		else if(i==6) herwig_obs->at(i)->SetMarkerStyle(34);
		herwig_obs->at(i)->Draw("same e1");
		pythia_obs->at(i)->SetLineColor((i-2)*15+1);
		pythia_obs->at(i)->SetMarkerColor((i-2)*15+1);
		pythia_obs->at(i)->SetMarkerStyle(i+22);
		pythia_obs->at(i)->SetMarkerSize(3);
		pythia_obs->at(i)->Draw("same e1");
		l_data->AddEntry(herwig_obs->at(i), std::format("Herwig {}", herwig_obs->at(i)->GetTitle()).c_str());
		l_data->AddEntry(pythia_obs->at(i), std::format("Pythia {}", pythia_obs->at(i)->GetTitle()).c_str());
		pads->at(1)->cd();
		TH1I* h_ratio = conf->GetRatioPlot(herwig_obs->at(i), pythia_obs->at(i));
		h_ratio->SetMarkerStyle(2*(i-2)+37);
		h_ratio->Draw("same");
		l_data->AddEntry(h_ratio);
	}
	pads->at(0)->cd();
	l_data->Draw();
	return;
}
void PlotPhotonPlots(TDirectory* herwig_photons, TDirectory* pythia_photons, std::vector<TCanvas*>*Canvi)
{
	//all Photons
	std::vector<TH1F*>* herwig_pt		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_e		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_phi		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_eta		= new std::vector<TH1F*> ();
	std::vector<TH1I*>* herwig_n_photons	= new std::vector<TH1I*> ();
	std::vector<TH1I*>* herwig_n_comp	= new std::vector<TH1I*> ();
	std::array<std::vector<TH1F*>*, 4>* herwig_photon_array =new std::array<std::vector<TH1F*>*, 4>();
	herwig_photon_array->at(0)=herwig_pt;
	herwig_photon_array->at(1)=herwig_e;
	herwig_photon_array->at(2)=herwig_phi;
	herwig_photon_array->at(3)=herwig_eta;
	//lead Photons
	std::vector<TH1F*>* herwig_lead_pt	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_lead_e	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_lead_phi	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* herwig_lead_eta	= new std::vector<TH1F*> ();
	std::vector<TH1I*>* herwig_lead_n_comp	= new std::vector<TH1I*> ();
	std::array<std::vector<TH1F*>*, 4>* herwig_lead_photon_array =new std::array<std::vector<TH1F*>*, 4>();
	herwig_lead_photon_array->at(0)=herwig_lead_pt;
	herwig_lead_photon_array->at(1)=herwig_lead_e;
	herwig_lead_photon_array->at(2)=herwig_lead_phi;
	herwig_lead_photon_array->at(3)=herwig_lead_eta;
	CollectPhotons(herwig_photons, herwig_photon_array, herwig_lead_photon_array, herwig_n_photons, heriwg_n_comp, herwig_lead_n_comp);
	//all Photons
	std::vector<TH1F*>* pythia_pt		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_e		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_phi		= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_eta		= new std::vector<TH1F*> ();
	std::vector<TH1I*>* pythia_n_photons	= new std::vector<TH1I*> ();
	std::vector<TH1I*>* pythia_n_comp	= new std::vector<TH1I*> ();
	std::array<std::vector<TH1F*>*, 4>* pythia_photon_array =new std::array<std::vector<TH1F*>*, 4>();
	pythia_photon_array->at(0)=pythia_pt;
	pythia_photon_array->at(1)=pythia_e;
	pythia_photon_array->at(2)=pythia_phi;
	pythia_photon_array->at(3)=pythia_eta;
	//lead Photons
	std::vector<TH1F*>* pythia_lead_pt	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_lead_e	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_lead_phi	= new std::vector<TH1F*> ();
	std::vector<TH1F*>* pythia_lead_eta	= new std::vector<TH1F*> ();
	std::vector<TH1I*>* pythia_lead_n_comp	= new std::vector<TH1I*> ();
	std::array<std::vector<TH1F*>*, 4>* pythia_lead_photon_array =new std::array<std::vector<TH1F*>*, 4>();
	pythia_lead_photon_array->at(0)=pythia_lead_pt;
	pythia_lead_photon_array->at(1)=pythia_lead_e;
	pythia_lead_photon_array->at(2)=pythia_lead_phi;
	pythia_lead_photon_array->at(3)=pythia_lead_eta;
	CollectPhotons(pythia_photons, pythia_photon_array, pythia_lead_photon_array, pythia_n_photons, heriwg_n_comp, pythia_lead_n_comp);
	TCanvas* J_pt=new TCanvas("photon_pt", "photon_pt");
	PlotPhotonObs(herwig_photon_array->at(0), pythia_photon_array->at(0), J_pt);
	TCanvas* J_l_pt=new TCanvas("photon_l_pt", "photon_l_pt");
	PlotPhotonObs(herwig_lead_photon_array->at(0), pythia_lead_photon_array->at(0), J_l_pt);
	TCanvas* J_e=new TCanvas("photon_e", "photon_e");
	PlotPhotonObs(herwig_photon_array->at(1), pythia_photon_array->at(1), J_e);
	TCanvas* J_l_e=new TCanvas("photon_l_e", "photon_l_e");
	PlotPhotonObs(herwig_lead_photon_array->at(1), pythia_lead_photon_array->at(1), J_l_e);
	TCanvas* J_phi=new TCanvas("photon_phi", "photon_phi");
	PlotPhotonObs(herwig_photon_array->at(2), pythia_photon_array->at(2), J_phi);
	TCanvas* J_l_phi=new TCanvas("photon_l_pt", "photon_l_pt");
	PlotPhotonObs(herwig_lead_photon_array->at(2), pythia_lead_photon_array->at(2), J_l_phi);
	TCanvas* J_eta=new TCanvas("photon_eta", "photon_eta");
	PlotPhotonObs(herwig_photon_array->at(3), pythia_photon_array->at(3), J_eta);
	TCanvas* J_l_eta=new TCanvas("photon_l_eta", "photon_l_eta");
	PlotPhotonObs(herwig_lead_photon_array->at(3), pythia_lead_photon_array->at(3), J_l_eta);
	TCanvas* J_n_comp=new TCanvas("photon_n_comp", "photon_n_comp");
	PlotPhotonObs(herwig_n_comp, pythia_n_comp, J_n_comp);
	TCanvas* J_l_n_comp=new TCanvas("photon_l_n_comp", "photon_l_n_comp");
	PlotPhotonObs(herwig_lead_n_comp, pythia_lead_n_comp, J_l_n_comp);
	TCanvas* J_N=new TCanvas("photon_N", "photon_N");
	PlotPhotonObs(herwig_n_photons, pythia_n_photons, J_N);

	Canvi->push_back(J_pt);
	Canvi->push_back(J_l_pt);
	Canvi->push_back(J_e);
	Canvi->push_back(J_l_e);
	Canvi->push_back(J_phi);
	Canvi->push_back(J_l_phi);
	Canvi->push_back(J_eta);
	Canvi->push_back(J_l_eta);
	Canvi->push_back(J_n_comp);
	Canvi->push_back(J_l_n_comp);
	Canvi->push_bacl(J_N);
	return;
	
}
void CollectPhotons(TDirectory* photons, std::array<std::vector<TH1F*>*, 4>* all_photons, std::array<std::vector<TH1F*>*, 4>* lead_photons, std::vector<TH1F*>* all_photon_n, std::vector<TH1F*>* all_photon_comp, std::vector<TH1F*>* lead_photon_comp)
{
	for(int i=2; i<7; i++)
	{
		photons->cd();
		TDirectory* rdir=(TDirectory*)_photons->GetDirectory(std::format("R0{}Photons", i).c_str());
		rdir->cd();
		all_photons->at(0)->push_back((TH1F*)r_dir->Get(std::format("h_photon_r0{}_pt", i).c_str());
		all_photons->at(1)->push_back((TH1F*)r_dir->Get(std::format("h_photon_r0{}_e", i).c_str());
		all_photons->at(2)->push_back((TH1F*)r_dir->Get(std::format("h_photon_r0{}_phi", i).c_str());
		all_photons->at(3)->push_back((TH1F*)r_dir->Get(std::format("h_photon_r0{}_eta", i).c_str());
		all_photon_n->push_back((TH1I*)r_dir->Get(std::format("h_photon_r0{}_n",i).c_str()));
		all_photon_comp->push_back((TH1I*)r_dir->Get(std::format("h_photon_r0{}_comp",i).c_str()));
		TDirectory* leaddir=(TDirectory*)r_dir->GetDirectory("Lead_Photons");
		lead_photons->at(0)->push_back((TH1F*)leaddir->Get(std::format("h_lead_photon_r0{}_pt", i).c_str());
		lead_photons->at(1)->push_back((TH1F*)leaddir->Get(std::format("h_lead_photon_r0{}_e", i).c_str());
		lead_photons->at(2)->push_back((TH1F*)leaddir->Get(std::format("h_lead_photon_r0{}_phi", i).c_str());
		lead_photons->at(3)->push_back((TH1F*)leaddir->Get(std::format("h_lead_photon_r0{}_eta", i).c_str());
		lead_photon_comp->push_back((TH1I*)leaddir->Get(std::format("h_lead_photon_r0{}_comp",i).c_str()));
		rdir->cd();
	}	
	return;

}
void PlotPhotonObs(std::vector<TH1F*>* herwig_obs, std::vector<TH1F*>* pythia_obs, TCanvas* obs_canv)
{
	//just does the plotting of one variable at a time
	TLegend* l_data=new TLegend(0.6, 0.3, 1, 0.5);
	TLegend* l_header=new TLegend(0.8, 0.6, 1, 1);
	conf->SetsPhenixHeaderLegend(l_header);
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	std::vector<TPad*>* pads=conf->AddPads(obs_canv);
	ScaleXS(herwig_obs, true);
	ScaleXS(pythia_obs, false);
	for(int i=0; i<(int)herwig_obs->size(); i++)
	{
		pads->at(0)->cd();
		herwig_obs->at(i)->SetLineColor((i-2)*15+1);
		herwig_obs->at(i)->SetMarkerColor((i-2)*15+1);
		herwig_obs->at(i)->SetMarkerStyle(i+18);
		herwig_obs->at(i)->SetMarkerSize(3);
		if(i==5) herwig_obs->at(i)->SetMarkerStyle(33);
		else if(i==6) herwig_obs->at(i)->SetMarkerStyle(34);
		herwig_obs->at(i)->Draw("same e1");
		pythia_obs->at(i)->SetLineColor((i-2)*15+1);
		pythia_obs->at(i)->SetMarkerColor((i-2)*15+1);
		pythia_obs->at(i)->SetMarkerStyle(i+22);
		pythia_obs->at(i)->SetMarkerSize(3);
		pythia_obs->at(i)->Draw("same e1");
		l_data->AddEntry(herwig_obs->at(i), std::format("Herwig {}", herwig_obs->at(i)->GetTitle()).c_str());
		l_data->AddEntry(pythia_obs->at(i), std::format("Pythia {}", pythia_obs->at(i)->GetTitle()).c_str());
		pads->at(1)->cd();
		TH1F* h_ratio = conf->GetRatioPlot(herwig_obs->at(i), pythia_obs->at(i));
		h_ratio->SetMarkerStyle(2*(i-2)+37);
		h_ratio->Draw("same");
		l_data->AddEntry(h_ratio);
	}
	pads->at(0)->cd();
	l_data->Draw();
	return;
}
void PlotPhotonObs(std::vector<TH1I*>* herwig_obs, std::vector<TH1I*>* pythia_obs, TCanvas* obs_canv)
{
	//just does the plotting of one variable at a time
	TLegend* l_data=new TLegend(0.6, 0.3, 1, 0.5);
	TLegend* l_header=new TLegend(0.8, 0.6, 1, 1);
	conf->SetsPhenixHeaderLegend(l_header);
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	std::vector<TPad*>* pads=conf->AddPads(obs_canv);
	ScaleXS(herwig_obs, true);
	ScaleXS(pythia_obs, false);
	for(int i=0; i<(int)herwig_obs->size(); i++)
	{
		pads->at(0)->cd();
		herwig_obs->at(i)->SetLineColor((i-2)*15+1);
		herwig_obs->at(i)->SetMarkerColor((i-2)*15+1);
		herwig_obs->at(i)->SetMarkerStyle(i+18);
		herwig_obs->at(i)->SetMarkerSize(3);
		if(i==5) herwig_obs->at(i)->SetMarkerStyle(33);
		else if(i==6) herwig_obs->at(i)->SetMarkerStyle(34);
		herwig_obs->at(i)->Draw("same e1");
		pythia_obs->at(i)->SetLineColor((i-2)*15+1);
		pythia_obs->at(i)->SetMarkerColor((i-2)*15+1);
		pythia_obs->at(i)->SetMarkerStyle(i+22);
		pythia_obs->at(i)->SetMarkerSize(3);
		pythia_obs->at(i)->Draw("same e1");
		l_data->AddEntry(herwig_obs->at(i), std::format("Herwig {}", herwig_obs->at(i)->GetTitle()).c_str());
		l_data->AddEntry(pythia_obs->at(i), std::format("Pythia {}", pythia_obs->at(i)->GetTitle()).c_str());
		pads->at(1)->cd();
		TH1I* h_ratio = conf->GetRatioPlot(herwig_obs->at(i), pythia_obs->at(i));
		h_ratio->SetMarkerStyle(2*(i-2)+37);
		h_ratio->Draw("same");
		l_data->AddEntry(h_ratio);
	}
	pads->at(0)->cd();
	l_data->Draw();
	return;
}

#endif

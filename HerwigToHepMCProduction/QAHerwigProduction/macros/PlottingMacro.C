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
TFile* storage;
void DoAllThePlotting(TFile* herwig_file, TFile* pythia_file, std::string trigger_tag="Jet30", float herwig_xs=1., float pythia_xs=1.)
{
	//this does all the plotting of Pythia vrs herwig
	SetsPhenixStyle();	
	storage=new TFile(std::format("{}_scaled_pt.root", trigger_tag), "RECREATE");
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
		PlotPhotonJetPlots(top_dirs["herwig photons"], top_dirs["pythia photons"], Canvi);
	}

	TDirectory* HEvent=(TDirectory*)herwig_file->GetDirectory("Event");
	TDirectory* PEvent=(TDirectory*)pythia_file->GetDirectory("Event");
	top_dirs["herwig event"]=HEvent;
	top_dirs["pythia event"]=PEvent;	
	PlotEventPlots(HEvent, PEvent, Canvi);
	conf->DumpHistos(std::format("~/{}_output.pdf", trigger_tag), Canvi);
	storage->cd();
	storage->Write();
	storage->Close();
	return;
}
void PlotPhotonJetPlots(TDirectory* herwig_photons, TDirectory* pythia_photons, std::vector<TCanvas*>* Canvi)
{
	TDirectory* H_pj = (TDirectory*)herwig_photons->GetDirectory("Photon-Jets");
	std::vector<TH2F*> herwig_pt		= new std::vector<TH2F*> ();
	std::vector<TH2F*> herwig_eta		= new std::vector<TH2F*> ();
	std::vector<TH2F*> herwig_phi		= new std::vector<TH2F*> ();
	std::vector<TH2F*> herwig_e		= new std::vector<TH2F*> ();
	std::vector<TH1F*> herwig_dphi		= new std::vector<TH2F*> ();
	CollectPhotonJets(H_pj, herwig_pt, herwig_eta, herwig_phi, herwig_e, herwig_dphi);

	TDirectory* P_pj = (TDirectory*)pythia_photons->GetDirectory("Photon-Jets");
	std::vector<TH2F*> pythia_pt		= new std::vector<TH2F*> ();
	std::vector<TH2F*> pythia_eta		= new std::vector<TH2F*> ();
	std::vector<TH2F*> pythia_phi		= new std::vector<TH2F*> ();
	std::vector<TH2F*> pythia_e		= new std::vector<TH2F*> ();
	std::vector<TH1F*> pythia_dphi		= new std::vector<TH1F*> ();
	CollectPhotonJets(P_pj, pythia_pt, pythia_eta, pythia_phi, pythia_e, pythia_dphi);
	
	TCanvas* J_pt=new TCanvas("photon_jet_pt");
	TCanvas* J_h_pt=new TCanvas("herwig_photon_jet_pt");
	TCanvas* J_p_pt=new TCanvas("pythia_photon_jet_pt");
	PlotPhotonJetObs(hewig_pt, pythia_pt, J_pt, J_h_pt, J_p_pt); 
	
	TCanvas* J_eta=new TCanvas("photon_jet_eta");
	TCanvas* J_h_eta=new TCanvas("photon_herwig_jet_eta");
	TCanvas* J_p_eta=new TCanvas("pythia_photon_jet_eta");
	PlotPhotonJetObs(hewig_eta, pythia_eta, J_eta, J_h_eta, J_p_eta); 

	TCanvas* J_phi=new TCanvas("photon_herwig_jet_phi");
	TCanvas* J_h_phi=new TCanvas("photon_herwig_jet_phi");
	TCanvas* J_p_phi=new TCanvas("pythia_photon_jet_phi");
	PlotPhotonJetObs(hewig_phi, pythia_phi, J_phi, J_h_phi, J_p_phi); 

	TCanvas* J_e=new TCanvas("photon_herwig_jet_e");
	TCanvas* J_h_e=new TCanvas("photon_herwig_jet_e");
	TCanvas* J_p_e=new TCanvas("pythia_photon_jet_e");
	PlotPhotonJetObs(hewig_e, pythia_e, J_e, J_h_e, J_p_e); 

       	TCanvas* J_dphi=new TCanvas("photon_jet_dphi");
	PlotJetObs(herwig_dphi, pythia_dphi, J_dphi);
	Canvi->push_back(J_dphi);
	return;
}
void PlotPhotonJetObs(std::vector<TH2F*>* herwig_obs, std::vector<TH2F*>* pythia_obs, TCanvas* herwig_Canvas, TCanvas* pythia_Canvas, TCanvas* ratio_Canvas)
{
	std::vector<TPad*> subdivided_herwig = conf->2DJetDivide(herwig_Canvas);
	std::vector<TPad*> subdivided_pythia = conf->2DJetDivide(pythia_Canvas);
	std::vector<TPad*> subdivided_ratio = conf->2DJetDivide(ratio_Canvas);

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
	
	storage->cd();
	TDirectory* hwj=(TDirectory*)storage->mkdir("Herwig_jets");
	hwj->cd();
	for(int i=0; i<(int)herwig_pt->size(); i++) herwig_pt->Write();
	for(int i=0; i<(int)herwig_lead_pt->size(); i++) herwig_lead_pt->Write();
	storage->cd();
	TDirectory* pj=(TDirectory*)storage->mkdir("Pythia_jets");
	pj->cd();
	for(int i=0; i<(int)pythia_pt->size(); i++) pythia_pt->Write();
	for(int i=0; i<(int)pythia_lead_pt->size(); i++) pythia_lead_pt->Write();
	storage->cd();
	
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
	TH1F* herwig_pt		{};
	TH1F* herwig_e		{};
	TH1F* herwig_phi		{};
	TH1F* herwig_eta		{};
	TH1I* herwig_n_photons	{};
	std::array<TH1F*, 4>* herwig_photon_array =new std::array<TH1F*, 4>();
	herwig_photon_array->at(0)=herwig_pt;
	herwig_photon_array->at(1)=herwig_e;
	herwig_photon_array->at(2)=herwig_phi;
	herwig_photon_array->at(3)=herwig_eta;
	//lead Photons
	TH1F* herwig_lead_pt	{};
	TH1F* herwig_lead_e	{};
	TH1F* herwig_lead_phi	{};
	TH1F* herwig_lead_eta	{};
	std::array<TH1F*, 4>* herwig_lead_photon_array =new std::array<TH1F*, 4>();
	herwig_lead_photon_array->at(0)=herwig_lead_pt;
	herwig_lead_photon_array->at(1)=herwig_lead_e;
	herwig_lead_photon_array->at(2)=herwig_lead_phi;
	herwig_lead_photon_array->at(3)=herwig_lead_eta;
	//direct Photons
	TH1F* herwig_direct_pt		{};
	TH1F* herwig_direct_e		{};
	TH1F* herwig_direct_phi		{};
	TH1F* herwig_direct_eta		{};
	TH1I* herwig_direct_n_photons	{};
	std::array<TH1F*, 4>* herwig_direct_photon_array =new std::array<TH1F*, 4>();
	herwig_direct_photon_array->at(0)=herwig_direct_pt;
	herwig_direct_photon_array->at(1)=herwig_direct_e;
	herwig_direct_photon_array->at(2)=herwig_direct_phi;
	herwig_direct_photon_array->at(3)=herwig_direct_eta;
	//all Photons
	TH1F* herwig_frag_pt		{};
	TH1F* herwig_frag_e		{};
	TH1F* herwig_frag_phi		{};
	TH1F* herwig_frag_eta		{};
	TH1I* herwig_frag_n_photons	{};
	std::array<TH1F*, 4>* herwig_frag_photon_array =new std::array<TH1F*, 4>();
	herwig_frag_photon_array->at(0)=herwig_frag_pt;
	herwig_frag_photon_array->at(1)=herwig_frag_e;
	herwig_frag_photon_array->at(2)=herwig_frag_phi;
	herwig_frag_photon_array->at(3)=herwig_frag_eta;
	CollectPhotons(herwig_photons, herwig_photon_array, herwig_lead_photon_array, herwig_direct_photon_array, herwig_frag_photon_array, herwig_n_photons, herwig_direct_n_photons, herwig_frag_n_photons);
	
	//all Photons
	TH1F* pythia_pt		{};
	TH1F* pythia_e		{};
	TH1F* pythia_phi		{};
	TH1F* pythia_eta		{};
	TH1I* pythia_n_photons	{};
	std::array<TH1F*, 4>* pythia_photon_array =new std::array<TH1F*, 4>();
	pythia_photon_array->at(0)=pythia_pt;
	pythia_photon_array->at(1)=pythia_e;
	pythia_photon_array->at(2)=pythia_phi;
	pythia_photon_array->at(3)=pythia_eta;
	//lead Photons
	TH1F* pythia_lead_pt	{};
	TH1F* pythia_lead_e	{};
	TH1F* pythia_lead_phi	{};
	TH1F* pythia_lead_eta	{};
	std::array<TH1F*, 4>* pythia_lead_photon_array =new std::array<TH1F*, 4>();
	pythia_lead_photon_array->at(0)=pythia_lead_pt;
	pythia_lead_photon_array->at(1)=pythia_lead_e;
	pythia_lead_photon_array->at(2)=pythia_lead_phi;
	pythia_lead_photon_array->at(3)=pythia_lead_eta;
	//direct Photons
	TH1F* pythia_direct_pt		{};
	TH1F* pythia_direct_e		{};
	TH1F* pythia_direct_phi		{};
	TH1F* pythia_direct_eta		{};
	TH1I* pythia_direct_n_photons	{};
	std::array<TH1F*, 4>* pythia_direct_photon_array =new std::array<TH1F*, 4>();
	pythia_direct_photon_array->at(0)=pythia_direct_pt;
	pythia_direct_photon_array->at(1)=pythia_direct_e;
	pythia_direct_photon_array->at(2)=pythia_direct_phi;
	pythia_direct_photon_array->at(3)=pythia_direct_eta;
	//all Photons
	TH1F* pythia_frag_pt		{};
	TH1F* pythia_frag_e		{};
	TH1F* pythia_frag_phi		{};
	TH1F* pythia_frag_eta		{};
	TH1I* pythia_frag_n_photons	{};
	std::array<TH1F*, 4>* pythia_frag_photon_array =new std::array<TH1F*, 4>();
	pythia_frag_photon_array->at(0)=pythia_frag_pt;
	pythia_frag_photon_array->at(1)=pythia_frag_e;
	pythia_frag_photon_array->at(2)=pythia_frag_phi;
	pythia_frag_photon_array->at(3)=pythia_frag_eta;
	CollectPhotons(pythia_photons, pythia_photon_array, pythia_lead_photon_array, pythia_direct_photon_array, pythia_frag_photon_array, pythia_n_photons, pythia_direct_n_photons, pythia_frag_n_photons);
	
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
	Canvi->push_bacl(J_N);
	
	TCanvas* J_d_pt=new TCanvas("direct_photonpt", "direct_photonpt");
	PlotPhotonObs(herwig_direct_photon_array->at(0), pythia_direct_photon_array->at(0), J_d_pt);
	TCanvas* J_f_pt=new TCanvas("frag_photon_pt", "frag_photon_pt");
	PlotPhotonObs(herwig_frag_photon_array->at(0), pythia_frag_photon_array->at(0), J_f_pt);
	TCanvas* J_d_e=new TCanvas("direct_photone", "direct_photone");
	PlotPhotonObs(herwig_direct_photon_array->at(1), pythia_direct_photon_array->at(1), J_d_e);
	TCanvas* J_f_e=new TCanvas("frag_photon_e", "frag_photon_e");
	PlotPhotonObs(herwig_frag_photon_array->at(1), pythia_frag_photon_array->at(1), J_f_e);
	TCanvas* J_d_phi=new TCanvas("direct_photonphi", "direct_photonphi");
	PlotPhotonObs(herwig_direct_photon_array->at(2), pythia_direct_photon_array->at(2), J_d_phi);
	TCanvas* J_f_phi=new TCanvas("frag_photon_pt", "frag_photon_pt");
	PlotPhotonObs(herwig_frag_photon_array->at(2), pythia_frag_photon_array->at(2), J_f_phi);
	TCanvas* J_d_eta=new TCanvas("direct_photoneta", "direct_photoneta");
	PlotPhotonObs(herwig_direct_photon_array->at(3), pythia_direct_photon_array->at(3), J_d_eta);
	TCanvas* J_f_eta=new TCanvas("frag_photon_eta", "frag_photon_eta");
	PlotPhotonObs(herwig_frag_photon_array->at(3), pythia_frag_photon_array->at(3), J_f_eta);
	TCanvas* J_d_N=new TCanvas("direct_photonN", "direct_photonN");
	PlotPhotonObs(herwig_direct_n_photons, pythia_direct_n_photons, J_d_N);
	TCanvas* J_f_N=new TCanvas("frag_photonN", "frag_photonN");
	PlotPhotonObs(herwig_direct_n_photons, pythia_frag_n_photons, J_f_N);

	Canvi->push_back(J_d_pt);
	Canvi->push_back(J_f_pt);
	Canvi->push_back(J_d_e);
	Canvi->push_back(J_f_e);
	Canvi->push_back(J_d_phi);
	Canvi->push_back(J_f_phi);
	Canvi->push_back(J_d_eta);
	Canvi->push_back(J_f_eta);
	Canvi->push_bacl(J_d_N);
	Canvi->push_bacl(J_f_N);
	return;
	
}
void CollectPhotons(TDirectory* photons, std::array<TH1F*, 4>* all_photons, std::array<TH1F*, 4>* lead_photons, std::array<TH1F*, 4>* direct_photons, std::array<TH1F*, 4>* frag_photons, TH1I* all_photons_n, TH1I* direc_photons_n, TH1I* frag_photons_n)
{
	photons->cd();
	all_photons->at(0)=(TH1F*)photons->Get(std::format("h_photons_r0{}_pt", i).c_str());
	all_photons->at(1)=(TH1F*)photons->Get(std::format("h_photons_r0{}_e", i).c_str());
	all_photons->at(2)=(TH1F*)photons->Get(std::format("h_photons_r0{}_phi", i).c_str());
	all_photons->at(3)=(TH1F*)photons->Get(std::format("h_photons_r0{}_eta", i).c_str());
	all_photons_n=(TH1I*)photons->Get(std::format("h_photon_r0{}_n",i).c_str());
	TDirectory* leaddir=(TDirectory*)photons->GetDirectory("Lead_Photons");
	lead_photons->at(0)=(TH1F*)leaddir->Get(std::format("h_lead_photons_r0{}_pt", i).c_str());
	lead_photons->at(1)=(TH1F*)leaddir->Get(std::format("h_lead_photons_r0{}_e", i).c_str());
	lead_photons->at(2)=(TH1F*)leaddir->Get(std::format("h_lead_photons_r0{}_phi", i).c_str());
	lead_photons->at(3)=(TH1F*)leaddir->Get(std::format("h_lead_photons_r0{}_eta", i).c_str());
	photons->cd();
	TDirectory* directdir=(TDirectory*)photons->GetDirectory("Direct_Photons");
	direct_photons->at(0)=(TH1F*)directdir->Get(std::format("h_direct_photons_r0{}_pt", i).c_str());
	direct_photons->at(1)=(TH1F*)directdir->Get(std::format("h_direct_photons_r0{}_e", i).c_str());
	direct_photons->at(2)=(TH1F*)directdir->Get(std::format("h_direct_photons_r0{}_phi", i).c_str());
	direct_photons->at(3)=(TH1F*)directdir->Get(std::format("h_direct_photons_r0{}_eta", i).c_str());
	direct_photons_n=(TH1I*)directdir->Get(std::format("h_direct_photon_r0{}_n",i).c_str());
	photons->cd();
	TDirectory* fragdir=(TDirectory*)photons->GetDirectory("Fragmentation_Photons");
	frag_photons->at(0)=(TH1F*)fragdir->Get(std::format("h_frag_photons_r0{}_pt", i).c_str());
	frag_photons->at(1)=(TH1F*)fragdir->Get(std::format("h_frag_photons_r0{}_e", i).c_str());
	frag_photons->at(2)=(TH1F*)fragdir->Get(std::format("h_frag_photons_r0{}_phi", i).c_str());
	frag_photons->at(3)=(TH1F*)fragdir->Get(std::format("h_frag_photons_r0{}_eta", i).c_str());
	frag_photons_n=(TH1I*)fragdir->Get(std::format("h_frag_photon_r0{}_n",i).c_str());
	photons->cd();
	return;

}
void PlotPhotonObs(TH1F* herwig_obs, TH1F* pythia_obs, TCanvas* obs_canv, int i)
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
	pads->at(0)->cd();
	herwig_obs->SetLineColor((i)*15+1);
	herwig_obs->SetMarkerColor((i)*15+1);
	herwig_obs->SetMarkerStyle(i+20);
	herwig_obs->SetMarkerSize(3);
	if(i==5) herwig_obs->SetMarkerStyle(33);
	else if(i==6) herwig_obs->SetMarkerStyle(34);
	herwig_obs->Draw("same e1");
	pythia_obs->SetLineColor((i)*15+1);
	pythia_obs->SetMarkerColor((i)*15+1);
	pythia_obs->SetMarkerStyle(i+24);
	pythia_obs->SetMarkerSize(3);
	pythia_obs->Draw("same e1");
	l_data->AddEntry(herwig_obs, std::format("Herwig {}", herwig_obs->GetTitle()).c_str());
	l_data->AddEntry(pythia_obs, std::format("Pythia {}", pythia_obs->GetTitle()).c_str());
	pads->at(1)->cd();
	TH1F* h_ratio = conf->GetRatioPlot(herwig_obs, pythia_obs);
	h_ratio->SetMarkerStyle(2*(i-2)+37);
	h_ratio->Draw("same");
	l_data->AddEntry(h_ratio);
	pads->at(0)->cd();
	l_data->Draw();
	return;
}
void PlotPhotonObs(TH1I* herwig_obs, TH1I* pythia_obs, TCanvas* obs_canv, int i)
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
	pads->at(0)->cd();
	herwig_obs->SetLineColor((i-2)*15+1);
	herwig_obs->SetMarkerColor((i-2)*15+1);
	herwig_obs->SetMarkerStyle(i+18);
	herwig_obs->SetMarkerSize(3);
	if(i==5) herwig_obs->SetMarkerStyle(33);
	else if(i==6) herwig_obs->SetMarkerStyle(34);
	herwig_obs->Draw("same e1");
	pythia_obs->SetLineColor((i-2)*15+1);
	pythia_obs->SetMarkerColor((i-2)*15+1);
	pythia_obs->SetMarkerStyle(i+22);
	pythia_obs->SetMarkerSize(3);
	pythia_obs->Draw("same e1");
	l_data->AddEntry(herwig_obs, std::format("Herwig {}", herwig_obs->GetTitle()).c_str());
	l_data->AddEntry(pythia_obs, std::format("Pythia {}", pythia_obs->GetTitle()).c_str());
	pads->at(1)->cd();
	TH1I* h_ratio = conf->GetRatioPlot(herwig_obs, pythia_obs);
	h_ratio->SetMarkerStyle(2*(i-2)+37);
	h_ratio->Draw("same");
	l_data->AddEntry(h_ratio);
pads->at(0)->cd();
l_data->Draw();
return;
}

#endif

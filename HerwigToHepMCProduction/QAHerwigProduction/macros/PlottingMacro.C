#pragma once 
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <TFile.h>
#include <TDirectory.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include "HerwigQAPlottingConfig.h"
#include "HerwigQAPlottingConfig.C"
#include "Special_colors.h"
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

#include <string>
#include <vector>
#include <format>
#include <map>
#include <array>
#include <utility>

HerwigQAPlottingConfig* conf;
TFile* storage;
std::string tag {};
Skaydis_colors* style_points;
struct particle_colls
{
	TH1F* pt;
	TH1I* n;
	TH2F* phi_eta;
};
void PlotCombinedSpectrum(std::vector<TFile*>* fs, std::vector<std::string> cutnames)
{
	SetsPhenixStyle();

	TCanvas* cAll	= new TCanvas("all_canvas", "all_Jets");
	TCanvas* cLead	= new TCanvas("lead_canvas", "Lead_jets");

	THStack* herwig_all	= new THStack("h_all", "h_all");
	THStack* herwig_lead	= new THStack("h_lead", "h_lead");
	THStack* pythia_all	= new THStack("p_all", "p_all");
	THStack* pythia_lead	= new THStack("p_lead", "p_lead");

	THStack* herwig_ratio 	= new THStack("h_ratio", "h_ratio");
	THStack* h_lead_ratio 	= new THStack("h_lead_ratio", "h_lead_ratio");
	THStack* pythia_ratio 	= new THStack("p_ratio", "p_ratio");
	THStack* p_lead_ratio 	= new THStack("p_lead_ratio", "p_lead_ratio");
	
	std::vector<TH1F*>* herwig_pt 	= new std::vector<TH1F*>();
	std::vector<TH1F*>* herwig_l_pt	= new std::vector<TH1F*>();
	std::vector<TH1F*>* pythia_pt 	= new std::vector<TH1F*>();
	std::vector<TH1F*>* pythia_l_pt	= new std::vector<TH1F*>();

	HerwigQAPlottingConfig* co = new HerwigQAPlottingConfig();
	Skaydis_colors* odd_colors = new Skaydis_colors();
	gStyle->SetPalette(100, odd_colors->Lesbian_gradient_PT);

	TLegend* l_head_all 	= new TLegend(0.7, 0.7, 1, 1);
	TLegend* l_head_jet 	= new TLegend(0.7, 0.7, 1, 1);
	TLegend* l_data_all 	= new TLegend(0.5, 0.4, 1, 0.7);
	TLegend* l_data_jet 	= new TLegend(0.5, 0.4, 1, 0.7);
	
	co->SetsPhenixHeaderLegend(l_head_all, "none");
	co->SetsPhenixHeaderLegend(l_head_jet, "none");
	co->SetLegend(l_data_all);
	co->SetLegend(l_data_jet);

	l_head_all->AddEntry("", "All Jets", "");
	l_head_jet->AddEntry("", "Lead Jets", "");
	l_data_all->SetNColumns(3);
	l_data_jet->SetNColumns(3);
	
	std::vector<TPad*>* All_pads = co->AddPads(cAll);
	std::vector<TPad*>* Lead_pads = co->AddPads(cLead);

	All_pads->at(1)->SetLogy();
	Lead_pads->at(1)->SetLogy();

	for(int i = 0; i<(int) fs->size(); i++)
	{
		fs->at(i)->cd();
		TDirectory* d_herwig	= (TDirectory*) fs->at(i)->GetDirectory("Herwig_jets");
		TDirectory* d_pythia	= (TDirectory*) fs->at(i)->GetDirectory("Pythia_jets");
		
		TH1F* h_all 	= (TH1F*)d_herwig->Get("h_jet_r04_pt");	
		TH1F* h_lead 	= (TH1F*)d_herwig->Get("h_lead_jet_r04_pt");	
		
		TH1F* p_all 	= (TH1F*)d_pythia->Get("h_jet_r04_pt");	
		TH1F* p_lead 	= (TH1F*)d_pythia->Get("h_lead_jet_r04_pt");	

		h_all->SetMarkerStyle(20+i);
		if(i==5) h_all->SetMarkerStyle(33);
		else if(i==6) h_all->SetMarkerStyle(34);

		h_lead->SetMarkerStyle(20+i);
		if(i==5) h_lead->SetMarkerStyle(33);
		else if(i==6) h_lead->SetMarkerStyle(34);
		
		p_all->SetMarkerStyle(i+24);
		p_all->SetMarkerSize(2);
		
		p_lead->SetMarkerStyle(i+24);
		p_lead->SetMarkerSize(2);
		
		herwig_pt->push_back(h_all);
		herwig_l_pt->push_back(h_lead);
		pythia_pt->push_back(p_all);
		pythia_l_pt->push_back(p_lead);
	
		herwig_all->Add(h_all);
		herwig_lead->Add(h_lead);	
		pythia_all->Add(p_all);
		pythia_lead->Add(p_lead);	
		
		l_data_all->AddEntry(h_all, std::format("Herwig {}", cutnames.at(i)).c_str(), "pl");
		l_data_jet->AddEntry(h_lead, std::format("Herwig {}", cutnames.at(i)).c_str(), "pl");
		l_data_all->AddEntry(p_all, std::format("Pythia {}", cutnames.at(i)).c_str(), "pl");
		l_data_jet->AddEntry(p_lead, std::format("Pythia {}", cutnames.at(i)).c_str(), "pl");
		
		cAll->cd();
		All_pads->at(0)->cd();
		auto ratio_all = co->GetRatioPlot(h_all, p_all);
		auto ratio_lead = co->GetRatioPlot(h_lead, p_lead);
		ratio_all->SetMarkerStyle(2*i+37);
		ratio_lead->SetMarkerStyle(2*i+37);
		ratio_all->Draw("same plc pmc");
		l_data_all->AddEntry(ratio_all, "Herwig / Pythia", "pl");
		cLead->cd();
		Lead_pads->at(0)->cd();
		ratio_lead->Draw("same plc pmc");
		l_data_jet->AddEntry(ratio_lead, "Herwig / Pythia", "pl");

	}
	cAll->cd();
	All_pads->at(1)->cd();
	herwig_all->Draw("pmc plc nostack same");
	pythia_all->Draw("pmc plc nostack same");
	TH1F* ha = (TH1F*) herwig_all->GetHistogram();
	ha->Draw("pmc plc same");
	TH1F* pa = (TH1F*) pythia_all->GetHistogram();
	pa->Draw("pmc plc same");
	l_data_all->AddEntry(ha, "Combined Herwig", "pc");
	l_data_all->AddEntry(pa, "Combined Pythia", "pc");
	l_data_all->Draw();
	l_head_all->Draw();
	All_pads->at(0)->cd();
	TH1F* ra = co->GetRatioPlot(ha, pa);
	ra->SetMarkerStyle(2*((int)fs->size())+37);
	ra->Draw("plc pmc same");
	l_data_all->AddEntry(ra, "Herwig / Pythia ", "pl");
	cAll->cd();
	All_pads->at(0)->Draw();
	All_pads->at(1)->Draw();
	
	cLead->cd();
	Lead_pads->at(1)->cd();
	herwig_lead->Draw("pmc plc nostack");
	pythia_lead->Draw("pmc plcsame nostack");
	TH1F* hj = (TH1F*) herwig_lead->GetHistogram();
	hj->Draw("pmc plc same");
	TH1F* pj = (TH1F*) pythia_lead->GetHistogram();
	pa->Draw("pmc plc same");
	l_data_jet->AddEntry(hj, "Combined Herwig", "pc");
	l_data_jet->AddEntry(pj, "Combined Pythia", "pc");
	l_data_jet->Draw();
	l_head_jet->Draw();
	Lead_pads->at(0)->cd();
	TH1F* rj = co->GetRatioPlot(hj, pj);
	rj->SetMarkerStyle(2*((int)fs->size())+37);
	rj->Draw("plc pmc same");
	l_data_jet->AddEntry(rj, "Herwig / Pythia ", "pl");
	cLead->cd();
	Lead_pads->at(0)->Draw();
	Lead_pads->at(1)->Draw();
	
	TCanvas* ct=new TCanvas("combined_lead", "combined_lead");
	ct->cd();
	auto pds = co->AddPads(ct);
	pds->at(1)->SetLogy();
	pds->at(1)->cd();
	TH1F* hj1=(TH1F*)herwig_l_pt->at(0)->Clone();
	TH1F* pj1=(TH1F*)pythia_l_pt->at(0)->Clone();
	for(int i = 1; i<(int)herwig_l_pt->size(); i++) hj1->Add(herwig_l_pt->at(i));
	for(int i = 1; i<(int)pythia_l_pt->size(); i++) pj1->Add(pythia_l_pt->at(i));
	hj1->Draw("pmc plc");
	pj1->Draw("same pmc plc");
	pds->at(0)->cd();
	auto rj1=co->GetRatioPlot(hj1, pj1);
	rj1->Draw("plc pmc");
	ct->cd();
	pds->at(0)->Draw();
	pds->at(1)->Draw();

	return;
}
			
			
void MakeParticleRatios( std::map<std::string, particle_colls*>* h,  std::map<std::string, particle_colls*>* p,  std::map<std::string, particle_colls*>* r)
{
	for(auto m:*h) 
	{
		if(!m.second) continue;
		auto pt = conf->GetRatioPlot(h->at(m.first)->pt, p->at(m.first)->pt);
		auto n = conf->GetRatioPlot(h->at(m.first)->n, p->at(m.first)->n);
		auto phi_eta = conf->GetRatioPlot(h->at(m.first)->phi_eta, p->at(m.first)->phi_eta);
		particle_colls* part = new particle_colls;
	       	part->pt 	= pt;
		part->n 	= n;
	       	part->phi_eta	= phi_eta;
		r->insert(std::pair<std::string, particle_colls*> (m.first,part));
	}
	return;
}	
void CollectEventPlots(TDirectory* event, std::vector<TH1F*>* particle_level, std::vector<TH2F*>* particle_correlations,  std::map<std::string, particle_colls*>* particle_coll)
{
	//Lots of 2D Plots here as well, so follow along with the PhotonJetApproach
	event->cd();
	
	//get the 1D Plots across all particle types
	TH1F* particle_eta 	= (TH1F*) event->Get("h_particle_eta");
	TH1F* particle_phi 	= (TH1F*) event->Get("h_particle_phi");
	TH1F* particle_e 	= (TH1F*) event->Get("h_particle_e");
	TH1F* particle_et	= (TH1F*) event->Get("h_particle_et");
	TH1F* particle_pt 	= (TH1F*) event->Get("h_particle_pt");
	TH1F* total_E 		= (TH1F*) event->Get("h_total_E");
	particle_eta->Scale(1/(float)total_E->GetEntries());
	particle_phi->Scale(1/(float)total_E->GetEntries());
	particle_eta->SetYTitle(" N_{hits} / N_{evts}");
	particle_phi->SetYTitle(" N_{hits} / N_{evts}");
	particle_level->push_back(particle_eta);
	particle_level->push_back(particle_phi);
	particle_level->push_back(particle_e);
	particle_level->push_back(particle_et);
	particle_level->push_back(particle_pt);
	particle_level->push_back(total_E);

	//now get the 2D Plots	
	TH2F* h_particle_et_eta		= (TH2F*) event->Get("h_particle_et_eta");
	TH2F* h_particle_et_phi		= (TH2F*) event->Get("h_particle_et_phi");
	TH2F* h_particle_pt_eta		= (TH2F*) event->Get("h_particle_pt_eta");
	TH2F* h_particle_pt_phi		= (TH2F*) event->Get("h_particle_pt_phi");
	TH2F* h_particle_e_eta		= (TH2F*) event->Get("h_particle_e_eta");
	TH2F* h_particle_e_phi		= (TH2F*) event->Get("h_particle_e_phi");
	TH2F* h_particle_phi_eta	= (TH2F*) event->Get("h_particle_phi_eta");

	particle_correlations->push_back(h_particle_et_eta);
	particle_correlations->push_back(h_particle_et_phi);
	particle_correlations->push_back(h_particle_pt_eta);
	particle_correlations->push_back(h_particle_pt_phi);
	particle_correlations->push_back(h_particle_e_eta);
	particle_correlations->push_back(h_particle_e_phi);
	particle_correlations->push_back(h_particle_phi_eta);

	//Individual particle type comp
	TDirectory* particles = (TDirectory*) event->GetDirectory("ParticleCat");
	particles->cd();
	std::vector<std::string> particle_names {"electron", "proton", "neutron", "pion", "photon"};
	for(auto n:particle_names)
	{
		TH1F* h_p_pt 	= (TH1F*) particles->Get(std::format("h_{}_pt", n).c_str());
		TH1I* h_p_n	= (TH1I*) particles->Get(std::format("h_{}_n", n).c_str());
		TH2F* h_p_ph	= (TH2F*) particles->Get(std::format("h_{}_phi_eta", n).c_str());
		if(!h_p_pt) continue;
		particle_colls* part = new particle_colls;
	       	part->pt	= h_p_pt;
	        part->n 	= h_p_n;
	        part->phi_eta	= h_p_ph;
		particle_coll->insert(std::pair<std::string, particle_colls*>(n, part));
	}
	return;
}
void PlotEventObs(TH1F* herwig_obs, TH1F* pythia_obs, TCanvas* c1)
{
	//plotting the kinematic variables
	std::string var = herwig_obs->GetXaxis()->GetTitle();
	std::vector<TPad*>* pads = conf->AddPads(c1);
	pads->at(1)->cd();
	TLegend* l_data=new TLegend(0.7, 0.4, 1, 0.58);
	TLegend* l_header=new TLegend(0.5, 0.6, 0.7, 1);
	conf->SetsPhenixHeaderLegend(l_header, tag);
	l_header->AddEntry("", std::format("Event {}", var).c_str(), "");
	conf->SetLegend(l_data);
	if(var.find("[GeV]") != std::string::npos) pads->at(1)->SetLogy();
	auto ratio = conf->GetRatioPlot(herwig_obs, pythia_obs);
	pythia_obs->SetMarkerStyle(24);
	herwig_obs->SetMarkerStyle(20);
	THStack* s=new THStack("s", "s");
	s->Add(herwig_obs);
	s->Add(pythia_obs);
	s->Draw("axis");
	s->Draw("plc pmc nostack same e1");
	s->GetXaxis()->SetTitle(var.c_str());
	s->GetYaxis()->SetTitle(herwig_obs->GetYaxis()->GetTitle());
	l_data->AddEntry(herwig_obs);
	l_data->AddEntry(pythia_obs);
	l_header->Draw();
	l_data->Draw("same");
	pads->at(0)->cd();
	ratio->SetMarkerStyle(34);
	ratio->Draw("pmc plc e1");
	l_data->AddEntry(ratio, "Herwig / Pythia");
	c1->cd();
	pads->at(0)->Draw();
	pads->at(1)->Draw();
	return;
}
void PlotEventObs(TH2F* herwig_obs, TH2F* pythia_obs, TCanvas* c1)
{
	//plotting the kinematic variables
	std::string var = herwig_obs->GetXaxis()->GetTitle();
	std::string var2 = herwig_obs->GetYaxis()->GetTitle();
	std::vector<TPad*>* pads = new std::vector<TPad*>();
	TPad* p1=new TPad("p1", "p1", 0, 0.52, 0.48, 1);
	TPad* p2=new TPad("p2", "p2", 0.52, 0.52, 1, 1);
	TPad* p3=new TPad("p3", "p3", 0, 0, 0.48, 0.48);
	TPad* p4=new TPad("p4", "p4", 0.52, 0, 1, 0.48);
	pads->push_back(p1);
	pads->push_back(p2);
	pads->push_back(p3);
	pads->push_back(p4);
	pads->at(0)->cd();
	TLegend* l_header=new TLegend(0.1, 0.1, 1, 1);
	conf->SetsPhenixHeaderLegend(l_header, tag);
	l_header->AddEntry("", std::format("Event {} - {}", var, var2).c_str(), "");
	if(var.find("[GeV]") != std::string::npos) pads->at(1)->SetLogx();
	if(var2.find("[GeV]") != std::string::npos) pads->at(1)->SetLogy();
	pads->at(1)->SetLogz();
	if(var.find("[GeV]") == std::string::npos &&  var2.find("[GeV]") == std::string::npos)
	{
		pads->at(1)->SetLogz(0);
		gStyle->SetPalette(100, style_points->Enby_log_gradient_PT);	
	}
	else{
		gStyle->SetPalette(100, style_points->Enby_gradient_PT);	
	}
	auto ratio = conf->GetRatioPlot(herwig_obs, pythia_obs);
	l_header->Draw();
	pads->at(1)->cd();
	herwig_obs->Draw("colz");
	TLegend* l_data=new TLegend(0.5, 0.8, 1, 1);
	conf->SetLegend(l_data);
	l_data->SetTextSize(0.05f);
	l_data->AddEntry(herwig_obs, std::format("Herwig {}", var).c_str(), "");
	l_data->Draw();
	pads->at(2)->cd();
	if(var.find("[GeV]") != std::string::npos) pads->at(2)->SetLogx();
	if(var2.find("[GeV]") != std::string::npos) pads->at(2)->SetLogy();
	pads->at(2)->SetLogz();
	if(var.find("[GeV]") == std::string::npos &&  var2.find("[GeV]") == std::string::npos) pads->at(2)->SetLogz(0);
	pythia_obs->Draw("colz");
	TLegend* l_data_p=new TLegend(0.5, 0.8, 1, 1);
	conf->SetLegend(l_data_p);
	l_data_p->SetTextSize(0.05f);
	l_data_p->AddEntry(pythia_obs, std::format("Pythia {}", var).c_str(), "");
	l_data_p->Draw();
	
	pads->at(3)->cd();
	if(var.find("[GeV]") != std::string::npos) pads->at(3)->SetLogx();
	if(var2.find("[GeV]") != std::string::npos) pads->at(3)->SetLogy();
	pads->at(3)->SetLogz();
	if(var.find("[GeV]") == std::string::npos &&  var2.find("[GeV]") == std::string::npos) pads->at(3)->SetLogz(0);
	ratio->SetMarkerStyle(34);
	ratio->Draw("colz");
	TLegend* l_data_r=new TLegend(0.5, 0.8, 1, 1);
	conf->SetLegend(l_data_r);
	l_data_r->SetTextSize(0.05f);
	l_data_r->AddEntry(pythia_obs, std::format(" Herwig / Pythia {}", var).c_str(), "");
	l_data_r->Draw();
	c1->cd();
	pads->at(0)->Draw();
	pads->at(1)->Draw();
	pads->at(2)->Draw();
	pads->at(3)->Draw();
	return;
}
void PlotEventObs(TH1F* herwig_particle, TH1F* pythia_particle, std::map<std::string, particle_colls*>* h_pc, std::map<std::string, particle_colls*>* p_pc, std::map<std::string, particle_colls*>* r_pc, TCanvas* c1)
{
	//comparing the particle species pt
	std::vector<TPad*>* pads=conf->AddPads(c1);
	pads->at(1)->cd();
	pads->at(1)->SetLogy();
	TLegend* l_data=new TLegend(0.3, 0.4, 1, 0.58);
	TLegend* l_header=new TLegend(0.5, 0.6, 0.7, 1);
	conf->SetsPhenixHeaderLegend(l_header, tag);
	l_header->AddEntry("", "Particle Species p_{T}", "");
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	THStack* herwig = new THStack("herwig","herwig");
	THStack* pythia = new THStack("pythia","pythia");
	THStack* ratio = new THStack("ratio","ratio");
	herwig_particle->SetMarkerStyle(20);
	pythia_particle->SetMarkerStyle(24);
	herwig->Add(herwig_particle);
	pythia->Add(pythia_particle);
	pads->at(0)->cd();
	TH1F* ratio_particle=conf->GetRatioPlot(herwig_particle, pythia_particle);
	ratio_particle->SetMarkerStyle(37);
	ratio->Add(ratio_particle);
	l_data->AddEntry(herwig_particle, "Herwig All Final State Particles");
	l_data->AddEntry(pythia_particle, "Pythia All Final State Particles");
	l_data->AddEntry(ratio_particle, "Herwig / Pythia Particles");
	int i=1;
	for(auto m: *h_pc)
	{
		h_pc->at(m.first)->pt->SetMarkerStyle(i+20);
		p_pc->at(m.first)->pt->SetMarkerStyle(i+24);
		r_pc->at(m.first)->pt->SetMarkerStyle(2*i+39);
		if(i >= 4){
			 m.second->pt->SetMarkerStyle(2*(i-4)+34);
			 r_pc->at(m.first)->pt->SetMarkerStyle(2*i+39);
		}
		herwig->Add(h_pc->at(m.first)->pt);
		pythia->Add(p_pc->at(m.first)->pt);
		ratio->Add(r_pc->at(m.first)->pt);
		l_data->AddEntry( m.second->pt, std::format("Herwig Final State {}", m.first).c_str());
		l_data->AddEntry( p_pc->at(m.first)->pt, std::format("Pythia Final State {}", m.first).c_str());
		l_data->AddEntry( r_pc->at(m.first)->pt, std::format("Herwig / Pythia {}", m.first).c_str());
	}
	c1->cd();
	pads->at(1)->cd();
	bool herwig_high = false;
	if (herwig->GetMaximum() > pythia->GetMaximum() ) herwig_high=true;
	if (herwig_high) herwig->Draw("axis");
	else pythia->Draw("axis");
	herwig->Draw("pmc plc nostack same e1");
	pythia->Draw("same pmc plc nostack e1");
	herwig->GetXaxis()->SetTitle(herwig_particle->GetXaxis()->GetTitle());
	herwig->GetYaxis()->SetTitle(herwig_particle->GetYaxis()->GetTitle());
	l_data->Draw();
	l_header->Draw();
	pads->at(0)->cd();
	ratio->Draw("pmc plc nostack e1");
	ratio->GetYaxis()->SetRangeUser(0.01, 2.);
	ratio->GetYaxis()->SetTitle(" Herwig / Pythia" ); 
	ratio->Draw("pmc plc nostack e1");
	c1->cd();
	pads->at(0)->Draw();
	pads->at(1)->Draw();
	return;	
}
void PlotEventObs(TH1I* herwig_particle, TH1I* pythia_particle, std::map<std::string, particle_colls*>* h_pc, std::map<std::string, particle_colls*>* p_pc, std::map<std::string, particle_colls*>* r_pc, TCanvas* c1)
{
	//comparing the particle species n 
	std::vector<TPad*>* pads=conf->AddPads(c1);
	pads->at(1)->cd();
	pads->at(1)->SetLogy();
	TLegend* l_data=new TLegend(0.2, 0.4, 0.6, 0.58);
	TLegend* l_header=new TLegend(0.5, 0.6, 0.7, 1);
	conf->SetsPhenixHeaderLegend(l_header, tag);
	l_header->AddEntry("", "Particle Species Count", "");
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	THStack* herwig = new THStack("herwig","herwig");
	THStack* pythia = new THStack("pythia","pythia");
	THStack* ratio = new THStack("ratio","ratio");
	herwig_particle->SetMarkerStyle(20);
	pythia_particle->SetMarkerStyle(24);
	herwig_particle->GetXaxis()->SetRangeUser(0,200);
	herwig->Add(herwig_particle);
	pythia->Add(pythia_particle);
	pads->at(0)->cd();
	TH1I* ratio_particle=conf->GetRatioPlot(herwig_particle, pythia_particle);
	ratio_particle->SetMarkerStyle(37);
	ratio->Add(ratio_particle);
	l_data->AddEntry(herwig_particle, "Herwig All Final State Particles");
	l_data->AddEntry(pythia_particle, "Pythia All Final State Particles");
	l_data->AddEntry(ratio_particle, "Herwig / Pythia Particles");
	int i=1;
	for(auto m: *h_pc)
	{
		h_pc->at(m.first)->n->SetMarkerStyle(i+20);
		p_pc->at(m.first)->n->SetMarkerStyle(i+24);
		r_pc->at(m.first)->n->SetMarkerStyle(2*i+39);
		if(i >= 4){
			 h_pc->at(m.first)->n->SetMarkerStyle(2*(i-4)+34);
			 r_pc->at(m.first)->n->SetMarkerStyle(2*i+39);
		}
		pads->at(1)->cd();
		pads->at(0)->cd();
		herwig->Add(h_pc->at(m.first)->n);
		pythia->Add(p_pc->at(m.first)->n);
		ratio->Add(r_pc->at(m.first)->n);
		l_data->AddEntry( h_pc->at(m.first)->n, std::format("Herwig Final State {}", m.first).c_str());
		l_data->AddEntry( p_pc->at(m.first)->n, std::format("Pythia Final State {}", m.first).c_str());
		l_data->AddEntry( r_pc->at(m.first)->n, std::format("Herwig / Pythia {}", m.first).c_str());
	}
	c1->cd();
	pads->at(1)->cd();
	bool herwig_high = false;
	if (herwig->GetMaximum() > pythia->GetMaximum() ) herwig_high=true;
	if (herwig_high) herwig->Draw("axis");
	else pythia->Draw("axis");
	herwig->Draw("pmc plc nostack same e1");
	pythia->Draw("same pmc plc nostack e1");
	herwig->GetXaxis()->SetRangeUser(0, 200);
	pythia->GetXaxis()->SetRangeUser(0, 200);
	herwig->GetXaxis()->SetTitle(herwig_particle->GetXaxis()->GetTitle());
	l_data->Draw();
	l_header->Draw();
	pads->at(0)->cd();
	ratio->Draw("pmc plc nostack e1");
	ratio->GetXaxis()->SetTitle(herwig_particle->GetXaxis()->GetTitle());
	ratio->GetYaxis()->SetTitle(" Herwig / Pythia" ); 
	ratio->GetXaxis()->SetRangeUser(0, 200);
	ratio->GetYaxis()->SetRangeUser(0.01, 2.);
	ratio->Draw("pmc plc nostack e1");	
	c1->cd();
	pads->at(0)->Draw();
	pads->at(1)->Draw();
	return;	
}
void PlotEventObs(TH2F* particle, std::map<std::string, particle_colls*>* pc, TCanvas* c1, std::string generator)
{
	//comparing the particle species phi-eta hits
	std::vector<TPad*>* pads = conf->Canvas2DDivide(c1);
	pads->at(0)->cd();
	TLegend* l_header=new TLegend(0.5, 0.6, 1, 1);
	conf->SetsPhenixHeaderLegend(l_header, tag);
	l_header->SetTextSize(0.03f);
	l_header->AddEntry("", generator.c_str(), "");
	l_header->AddEntry("", "Final State #eta - #varphi hit distribution", "");
	particle->Draw("colz");
	l_header->Draw();
	int i=1;
	for(auto m:*pc)
	{
		pads->at(i)->cd();
		TLegend* l_data=new TLegend(0.5, 0.8, 1, 0.9);
		conf->SetLegend(l_data);
		l_data->AddEntry("", m.first.c_str(), "");
		m.second->phi_eta->Draw("colz");
		l_data->Draw("same");
		i++;
	}
	c1->cd();
	for(int i=0; i<(int)pads->size(); i++) pads->at(i)->Draw();
	return;
}


void PlotEventPlots(TDirectory* herwig_event, TDirectory* pythia_event, std::vector<TCanvas*>* Canvi)
{
	std::vector<TH1F*>* herwig_particle_level 	= new std::vector<TH1F*> ();
	std::vector<TH2F*>* herwig_particle_correlations= new std::vector<TH2F*> ();
	TH1I* herwig_particle_n 			= (TH1I*) herwig_event->Get("h_particle_n");
	std::map<std::string, particle_colls*>* herwig_particle_coll = new std::map<std::string, particle_colls*> ();
	std::vector<TH1F*>* pythia_particle_level 	= new std::vector<TH1F*> ();
	std::vector<TH2F*>* pythia_particle_correlations= new std::vector<TH2F*> ();
	TH1I* pythia_particle_n				= (TH1I*) pythia_event->Get("h_particle_n");
	std::map<std::string, particle_colls*>* pythia_particle_coll  = new std::map<std::string, particle_colls*> ();
	CollectEventPlots(herwig_event, herwig_particle_level, herwig_particle_correlations, herwig_particle_coll);
	CollectEventPlots(pythia_event, pythia_particle_level, pythia_particle_correlations, pythia_particle_coll);
	std::map<std::string, particle_colls*>* ratio_particle_coll = new std::map<std::string, particle_colls*> ();
	MakeParticleRatios( herwig_particle_coll, pythia_particle_coll, ratio_particle_coll);
	gStyle->SetPalette(100, style_points->Lesbian_gradient_PT);	
	for(int i=0; i<(int)herwig_particle_level->size(); i++){
		std::string var = herwig_particle_level->at(i)->GetXaxis()->GetTitle();
		TCanvas* c1=new TCanvas(std::format("Canv_{}", var).c_str(),std::format("Canv_{}", var).c_str()); 
		PlotEventObs(herwig_particle_level->at(i), pythia_particle_level->at(i), c1);
		Canvi->push_back(c1);
		if(var.find("p_{T}") != std::string::npos){
			TCanvas* c2 = new TCanvas("pt", "pt");
			PlotEventObs(herwig_particle_level->at(i), pythia_particle_level->at(i), herwig_particle_coll, pythia_particle_coll, ratio_particle_coll, c2);
			Canvi->push_back(c2);
		}
	}
	for(int i=0; i<(int)herwig_particle_correlations->size(); i++){
		std::string var = herwig_particle_correlations->at(i)->GetXaxis()->GetTitle();
		std::string var2 = herwig_particle_correlations->at(i)->GetYaxis()->GetTitle();
		TCanvas* c1=new TCanvas(std::format("Canv2d_{}_{}", var, var2).c_str(),std::format("Canv2d_{}_{}", var, var2).c_str()); 
		PlotEventObs(herwig_particle_correlations->at(i), pythia_particle_correlations->at(i), c1);
		Canvi->push_back(c1);
		if(var.find("eta") !=std::string::npos && var2.find("varphi") != std::string::npos) 
		{
			TCanvas* ch = new TCanvas("etph_h", "etph_h");
			TCanvas* cp = new TCanvas("etph_p", "etph_p");
			TCanvas* cr = new TCanvas("etph_r", "etph_r");
			TH2F* ratio = conf->GetRatioPlot(herwig_particle_correlations->at(i), pythia_particle_correlations->at(i));
			PlotEventObs(herwig_particle_correlations->at(i), herwig_particle_coll, ch, "Herwig7.2");
			PlotEventObs(pythia_particle_correlations->at(i), pythia_particle_coll, cp, "Pythia8");
			PlotEventObs(ratio, ratio_particle_coll, cr, "Herwig / Pythia");
		}
	}
	gStyle->SetPalette(100, style_points->Trans_gradient_PT);	
	TCanvas* c3 = new TCanvas("n", "n");
	PlotEventObs(herwig_particle_n, pythia_particle_n, herwig_particle_coll, pythia_particle_coll, ratio_particle_coll, c3);
	Canvi->push_back(c3);
	return;

}
void PlotSingleGenPhotonJetObs(std::vector<TH2F*>* obs, TCanvas* c1, std::vector<TPad*>* pads, std::string label)
{
	c1->cd();
	pads->at(0)->cd();
	TLegend* l_label=new TLegend(0.1, 0.1, 1, 1);
	conf->SetsPhenixHeaderLegend(l_label, tag);
	l_label->AddEntry("", label.c_str(), "");
	l_label->AddEntry("", std::format("Jet {} Versus Photon {}", obs->at(0)->GetXaxis()->GetTitle(), obs->at(0)->GetYaxis()->GetTitle()).c_str(), "");
	for(int i=0; i<(int)obs->size(); i++)
	{
		pads->at(i+1)->cd();
		TLegend* l_data=new TLegend(0.7, 0.9, 0.9, 1);
		conf->SetLegend(l_data);
		l_data->AddEntry(obs->at(i), obs->at(i)->GetTitle(), "");
		obs->at(i)->Draw("colz");
		l_data->Draw("same");
		c1->cd();
		pads->at(i+1)->Draw();
	}
	return;
}
void PlotPhotonJetObs(std::vector<TH2F*>* herwig_obs, std::vector<TH2F*>* pythia_obs, TCanvas* herwig_Canvas, TCanvas* pythia_Canvas, TCanvas* ratio_Canvas)
{
	std::vector<TPad*>* subdivided_herwig = conf->Canvas2DDivide(herwig_Canvas);
	std::vector<TPad*>* subdivided_pythia = conf->Canvas2DDivide(pythia_Canvas);
	std::vector<TPad*>* subdivided_ratio = conf->Canvas2DDivide(ratio_Canvas);
	std::vector<TH2F*>* ratio_obs = conf->GetRatioPlots(herwig_obs, pythia_obs);
	PlotSingleGenPhotonJetObs(herwig_obs, herwig_Canvas, subdivided_herwig, "Herwig7.2");
	PlotSingleGenPhotonJetObs(pythia_obs, pythia_Canvas, subdivided_pythia, "Pythia8");
	PlotSingleGenPhotonJetObs(ratio_obs, ratio_Canvas, subdivided_ratio, "Herwig / Pythia");
	return;
}
void CollectJets(TDirectory* jets, std::array<std::vector<TH1F*>*, 4>* all_jets, std::array<std::vector<TH1F*>*, 4>* lead_jets, std::vector<TH1I*>* all_jet_n, std::vector<TH1I*>* all_jet_comp, std::vector<TH1I*>* lead_jet_comp)
{
	for(int i=2; i<7; i++)
	{
		jets->cd();
		TDirectory* r_dir=(TDirectory*)jets->GetDirectory(std::format("R0{}Jets", i).c_str());
		r_dir->cd();
		all_jets->at(0)->push_back((TH1F*)r_dir->Get(std::format("h_jet_r0{}_pt", i).c_str()));
		all_jets->at(1)->push_back((TH1F*)r_dir->Get(std::format("h_jet_r0{}_e", i).c_str()));
		all_jets->at(2)->push_back((TH1F*)r_dir->Get(std::format("h_jet_r0{}_phi", i).c_str()));
		all_jets->at(3)->push_back((TH1F*)r_dir->Get(std::format("h_jet_r0{}_eta", i).c_str()));
		all_jet_n->push_back((TH1I*)r_dir->Get(std::format("h_jet_r0{}_n",i).c_str()));
		all_jet_comp->push_back((TH1I*)r_dir->Get(std::format("h_jet_r0{}_comp",i).c_str()));
		TDirectory* leaddir=(TDirectory*)r_dir->GetDirectory("Lead_Jets");
		lead_jets->at(0)->push_back((TH1F*)leaddir->Get(std::format("h_lead_jet_r0{}_pt", i).c_str()));
		lead_jets->at(1)->push_back((TH1F*)leaddir->Get(std::format("h_lead_jet_r0{}_e", i).c_str()));
		lead_jets->at(2)->push_back((TH1F*)leaddir->Get(std::format("h_lead_jet_r0{}_phi", i).c_str()));
		lead_jets->at(3)->push_back((TH1F*)leaddir->Get(std::format("h_lead_jet_r0{}_eta", i).c_str()));
		lead_jet_comp->push_back((TH1I*)leaddir->Get(std::format("h_lead_jet_r0{}_comp",i).c_str()));
		r_dir->cd();
		float n_evts = (float)lead_jets->at(0)->back()->GetEntries();
		all_jets->at(2)->back()->Scale(1/n_evts);
		all_jets->at(2)->back()->SetYTitle(" N_{jets} / N_{evts} ");
		all_jets->at(3)->back()->Scale(1/n_evts);
		all_jets->at(3)->back()->SetYTitle(" N_{jets} / N_{evts} ");
		lead_jets->at(2)->back()->Scale(1/n_evts);
		lead_jets->at(2)->back()->SetYTitle(" N_{jets} / N_{evts} ");
		lead_jets->at(3)->back()->Scale(1/n_evts);
		lead_jets->at(3)->back()->SetYTitle(" N_{jets} / N_{evts} ");
	}	
	return;

}
void CollectPhotons(TDirectory* photons, std::array<TH1F*, 4>* all_photons, std::array<TH1F*, 4>* lead_photons, std::array<TH1F*, 4>* direct_photons, std::array<TH1F*, 4>* frag_photons, TH1I* all_photons_n, TH1I* direct_photons_n, TH1I* frag_photons_n)
{
	photons->cd();
	all_photons->at(0)=(TH1F*)photons->Get("h_photons_pt");
	all_photons->at(1)=(TH1F*)photons->Get("h_photons_e");
	all_photons->at(2)=(TH1F*)photons->Get("h_photons_phi");
	all_photons->at(3)=(TH1F*)photons->Get("h_photons_eta");
	all_photons_n=(TH1I*)photons->Get("h_photon_n");
	TDirectory* leaddir=(TDirectory*)photons->GetDirectory("Lead_Photons");
	lead_photons->at(0)=(TH1F*)leaddir->Get("h_lead_photons_pt");
	lead_photons->at(1)=(TH1F*)leaddir->Get("h_lead_photons_e");
	lead_photons->at(2)=(TH1F*)leaddir->Get("h_lead_photons_phi");
	lead_photons->at(3)=(TH1F*)leaddir->Get("h_lead_photons_eta");
	photons->cd();
	TDirectory* directdir=(TDirectory*)photons->GetDirectory("Direct_Photons");
	direct_photons->at(0)=(TH1F*)directdir->Get("h_direct_photons_pt");
	direct_photons->at(1)=(TH1F*)directdir->Get("h_direct_photons_e");
	direct_photons->at(2)=(TH1F*)directdir->Get("h_direct_photons_phi");
	direct_photons->at(3)=(TH1F*)directdir->Get("h_direct_photons_eta");
	direct_photons_n=(TH1I*)directdir->Get("h_direct_photon_n");
	photons->cd();
	TDirectory* fragdir=(TDirectory*)photons->GetDirectory("Fragmentation_Photons");
	frag_photons->at(0)=(TH1F*)fragdir->Get("h_frag_photons_pt");
	frag_photons->at(1)=(TH1F*)fragdir->Get("h_frag_photons_e");
	frag_photons->at(2)=(TH1F*)fragdir->Get("h_frag_photons_phi");
	frag_photons->at(3)=(TH1F*)fragdir->Get("h_frag_photons_eta");
	frag_photons_n=(TH1I*)fragdir->Get("h_frag_photon_n");
	photons->cd();
	float n_evts = (float) lead_photons->at(0)->GetEntries();
	//make phi and eta dist an average across events
	all_photons->at(2)->Scale(1/n_evts);
	all_photons->at(2)->SetYTitle(" N_{#gamma} / N_{evts} ");
	all_photons->at(3)->Scale(1/n_evts);
	all_photons->at(3)->SetYTitle(" N_{#gamma} / N_{evts} ");
	lead_photons->at(2)->Scale(1/n_evts);
	lead_photons->at(2)->SetYTitle(" N_{#gamma} / N_{evts} ");
	lead_photons->at(3)->Scale(1/n_evts);
	lead_photons->at(3)->SetYTitle(" N_{#gamma} / N_{evts} ");
	direct_photons->at(2)->Scale(1/n_evts);
	direct_photons->at(2)->SetYTitle(" N_{#gamma} / N_{evts} ");
	direct_photons->at(3)->Scale(1/n_evts);
	direct_photons->at(3)->SetYTitle(" N_{#gamma} / N_{evts} ");
	frag_photons->at(2)->Scale(1/n_evts);
	frag_photons->at(2)->SetYTitle(" N_{#gamma} / N_{evts} ");
	frag_photons->at(3)->Scale(1/n_evts);
	frag_photons->at(3)->SetYTitle(" N_{#gamma} / N_{evts} ");
	return;

}
void PlotJetObs(std::vector<TH1F*>* herwig_obs, std::vector<TH1F*>* pythia_obs, TCanvas* obs_canv)
{
	//just does the plotting of one variable at a time
	TLegend* l_data=new TLegend(0.7, 0.4, 1, 0.58);
	TLegend* l_header=new TLegend(0.5, 0.6, 0.7, 1);
	conf->SetsPhenixHeaderLegend(l_header, tag);
	l_header->AddEntry("", std::format("Jet {}", herwig_obs->at(0)->GetXaxis()->GetTitle()).c_str(), "");
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	std::vector<TPad*>* pads=conf->AddPads(obs_canv);
	std::string var=herwig_obs->at(0)->GetXaxis()->GetTitle();
	bool ispt = false;
	if(var.find("p_{T}") !=std::string::npos) ispt=true;
	if(ispt)
	{
		conf->ScaleXS(herwig_obs, true);
		conf->ScaleXS(pythia_obs, false);
		pads->at(1)->SetLogy();
	}
	else if(var.find("[GeV]") != std::string::npos) pads->at(1)->SetLogy();
	pads->at(1)->cd();
	THStack* herwig = new THStack("herwig", "herwig");
	THStack* pythia = new THStack("herwig", "herwig");
	for(int i= 0 ; i <(int)herwig_obs->size()-1;  i++)
	{
		pads->at(1)->cd();
		herwig_obs->at(i)->SetMarkerStyle(i+20);
		herwig_obs->at(i)->SetMarkerSize(2);
		if(i==5) herwig_obs->at(i)->SetMarkerStyle(33);
		else if(i==6) herwig_obs->at(i)->SetMarkerStyle(34);
		herwig->Add(herwig_obs->at(i));
		pythia_obs->at(i)->SetLineColor(herwig_obs->at(i)->GetLineColor());
		pythia_obs->at(i)->SetMarkerColor(herwig_obs->at(i)->GetMarkerColor());
		pythia_obs->at(i)->SetMarkerStyle(i+24);
		pythia_obs->at(i)->SetMarkerSize(2);
		pythia->Add(pythia_obs->at(i));
		l_data->AddEntry(herwig_obs->at(i), std::format("Herwig {}", herwig_obs->at(i)->GetTitle()).c_str());
		l_data->AddEntry(pythia_obs->at(i), std::format("Pythia ", pythia_obs->at(i)->GetTitle()).c_str());
		pads->at(0)->cd();
		TH1F* h_ratio = conf->GetRatioPlot(herwig_obs->at(i), pythia_obs->at(i));
		h_ratio->SetMarkerStyle(2*i+37);
		h_ratio->Draw("plc pmc same e1");
		l_data->AddEntry(h_ratio);
	}
	pads->at(1)->cd();
	bool herwig_high = false;
	if (herwig->GetMaximum() > pythia->GetMaximum() ) herwig_high=true;
	if (herwig_high) herwig->Draw("axis nostack");
	else pythia->Draw("axis nostack");
	herwig->Draw("plc pmc nostack same e1");
	pythia->Draw("plc pmc same nostack e1");
	pythia->GetXaxis()->SetTitle(herwig_obs->at(0)->GetXaxis()->GetTitle());
	pythia->GetYaxis()->SetTitle(herwig_obs->at(0)->GetYaxis()->GetTitle());
	l_data->Draw();
	l_header->Draw();
	obs_canv->cd();
	pads->at(0)->Draw();
	pads->at(1)->Draw();
	return;
}
void PlotJetObs(std::vector<TH1I*>* herwig_obs, std::vector<TH1I*>* pythia_obs, TCanvas* obs_canv)
{
	//just does the plotting of one variable at a time
	TLegend* l_data=new TLegend(0.6, 0.3, 1, 0.5);
	TLegend* l_header=new TLegend(0.8, 0.6, 1, 1);
	conf->SetsPhenixHeaderLegend(l_header, tag);
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	std::vector<TPad*>* pads=conf->AddPads(obs_canv);
	conf->ScaleXS(herwig_obs, true);
	conf->ScaleXS(pythia_obs, false);
	THStack* herwig = new THStack("herwig", "herwig");
	THStack* pythia = new THStack("herwig", "herwig");
	for(int i=0; i<(int)herwig_obs->size(); i++)
	{
		pads->at(1)->cd();
		herwig_obs->at(i)->SetMarkerStyle(i+20);
		herwig_obs->at(i)->SetMarkerSize(2);
		if(i==5) herwig_obs->at(i)->SetMarkerStyle(33);
		else if(i==6) herwig_obs->at(i)->SetMarkerStyle(34);
		herwig->Add(herwig_obs->at(i));
		pythia_obs->at(i)->SetMarkerStyle(i+24);
		pythia_obs->at(i)->SetMarkerSize(2);
		pythia->Add(pythia_obs->at(i));
		l_data->AddEntry(herwig_obs->at(i), std::format("Herwig {}", herwig_obs->at(i)->GetTitle()).c_str());
		l_data->AddEntry(pythia_obs->at(i), std::format("Pythia {}", pythia_obs->at(i)->GetTitle()).c_str());
		pads->at(0)->cd();
		TH1I* h_ratio = conf->GetRatioPlot(herwig_obs->at(i), pythia_obs->at(i));
		h_ratio->SetMarkerStyle(2*(i)+37);
		h_ratio->Draw("same plc pmc");
		l_data->AddEntry(h_ratio);
	}
	pads->at(1)->cd();
	pads->at(1)->SetLogy();
	l_data->Draw();
	l_header->Draw();
	bool herwig_high = false;
	if (herwig->GetMaximum() > pythia->GetMaximum() ) herwig_high=true;
	if (herwig_high) herwig->Draw("axis");
	else pythia->Draw("axis");
	herwig->Draw("plc pmc nostack same e1");
	pythia->Draw("plc pmc same nostack e1");
	pythia->GetXaxis()->SetTitle(herwig_obs->at(0)->GetXaxis()->GetTitle());
	pythia->GetYaxis()->SetTitle(herwig_obs->at(0)->GetYaxis()->GetTitle());

	obs_canv->cd();
	pads->at(0)->Draw();
	pads->at(1)->Draw();
	return;
}
void PlotPhotonObs(TH1F* herwig_obs, TH1F* pythia_obs, TCanvas* obs_canv, int i)
{
	//just does the plotting of one variable at a time
	TLegend* l_data=new TLegend(0.6, 0.3, 1, 0.5);
	TLegend* l_header=new TLegend(0.8, 0.6, 1, 1);
	conf->SetsPhenixHeaderLegend(l_header, tag);
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	std::vector<TPad*>* pads=conf->AddPads(obs_canv);
	conf->ScaleXS(herwig_obs, true);
	conf->ScaleXS(pythia_obs, false);
	pads->at(1)->cd();
	herwig_obs->SetMarkerStyle(i+20);
	herwig_obs->SetMarkerSize(2);
	if(i==5) herwig_obs->SetMarkerStyle(33);
	else if(i==6) herwig_obs->SetMarkerStyle(34);
	THStack* st=new THStack("s1", "s1");
	st->Add(herwig_obs);
	pythia_obs->SetMarkerStyle(i+24);
	pythia_obs->SetMarkerSize(2);
	st->Add(pythia_obs);
	st->Draw("axis");
	st->Draw("plc pmc nostack same e1");
	l_data->AddEntry(herwig_obs, std::format("Herwig {}", herwig_obs->GetTitle()).c_str());
	l_data->AddEntry(pythia_obs, std::format("Pythia {}", pythia_obs->GetTitle()).c_str());
	pads->at(0)->cd();
	TH1F* h_ratio = conf->GetRatioPlot(herwig_obs, pythia_obs);
	h_ratio->SetMarkerStyle(2*(i)+37);
	h_ratio->Draw("same");
	l_data->AddEntry(h_ratio);
	pads->at(1)->cd();
	l_data->Draw();
	obs_canv->cd();
	pads->at(0)->Draw();
	pads->at(1)->Draw();
	return;
}
void PlotPhotonObs(TH1I* herwig_obs, TH1I* pythia_obs, TCanvas* obs_canv, int i)
{
	//just does the plotting of one variable at a time
	TLegend* l_data=new TLegend(0.6, 0.3, 1, 0.5);
	TLegend* l_header=new TLegend(0.8, 0.6, 1, 1);
	conf->SetsPhenixHeaderLegend(l_header, tag);
	conf->SetLegend(l_data);
	l_data->SetNColumns(3);
	std::vector<TPad*>* pads=conf->AddPads(obs_canv);
	conf->ScaleXS(herwig_obs, true);
	conf->ScaleXS(pythia_obs, false);
	pads->at(1)->cd();
	herwig_obs->SetLineColor((i)*22+1);
	herwig_obs->SetMarkerColor(i*22+1);
	herwig_obs->SetMarkerStyle(i+20);
	herwig_obs->SetMarkerSize(2);
	if(i==5) herwig_obs->SetMarkerStyle(33);
	else if(i==6) herwig_obs->SetMarkerStyle(34);
	herwig_obs->Draw("same e1");
	pythia_obs->SetLineColor((i)*22+1);
	pythia_obs->SetMarkerColor((i)*22+1);
	pythia_obs->SetMarkerStyle(i+24);
	pythia_obs->SetMarkerSize(2);
	pythia_obs->Draw("same e1");
	l_data->AddEntry(herwig_obs, std::format("Herwig {}", herwig_obs->GetTitle()).c_str());
	l_data->AddEntry(pythia_obs, std::format("Pythia {}", pythia_obs->GetTitle()).c_str());
	pads->at(0)->cd();
	TH1I* h_ratio = conf->GetRatioPlot(herwig_obs, pythia_obs);
	h_ratio->SetMarkerStyle(2*(i)+37);
	h_ratio->Draw("same");
	l_data->AddEntry(h_ratio);
	pads->at(1)->cd();
	l_data->Draw();
	obs_canv->cd();
	pads->at(0)->Draw();
	pads->at(1)->Draw();
	return;
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

	CollectJets(herwig_jets, herwig_jet_array, herwig_lead_jet_array, herwig_n_jets, herwig_n_comp, herwig_lead_n_comp);
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
	CollectJets(pythia_jets, pythia_jet_array, pythia_lead_jet_array, pythia_n_jets, pythia_n_comp, pythia_lead_n_comp);
	TCanvas* J_pt=new TCanvas("jet_pt", "jet_pt");
	PlotJetObs(herwig_jet_array->at(0), pythia_jet_array->at(0), J_pt);
	J_pt->SetLogy();
	TCanvas* J_l_pt=new TCanvas("jet_l_pt", "jet_l_pt");
	PlotJetObs(herwig_lead_jet_array->at(0), pythia_lead_jet_array->at(0), J_l_pt);
	J_l_pt->SetLogy();
	TCanvas* J_e=new TCanvas("jet_e", "jet_e");
	PlotJetObs(herwig_jet_array->at(1), pythia_jet_array->at(1), J_e);
	J_e->SetLogy();
	TCanvas* J_l_e=new TCanvas("jet_l_e", "jet_l_e");
	PlotJetObs(herwig_lead_jet_array->at(1), pythia_lead_jet_array->at(1), J_l_e);
	J_l_e->SetLogy();
	TCanvas* J_phi=new TCanvas("jet_phi", "jet_phi");
	PlotJetObs(herwig_jet_array->at(2), pythia_jet_array->at(2), J_phi);
	TCanvas* J_l_phi=new TCanvas("jet_l_phi", "jet_l_phi");
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
	Canvi->push_back(J_N);
	
	storage->cd();
	TDirectory* hwj=(TDirectory*)storage->mkdir("Herwig_jets");
	hwj->cd();
	for(int i=0; i<(int)herwig_pt->size(); i++) herwig_pt->at(i)->Write();
	for(int i=0; i<(int)herwig_lead_pt->size(); i++) herwig_lead_pt->at(i)->Write();
	storage->cd();
	TDirectory* pj=(TDirectory*)storage->mkdir("Pythia_jets");
	pj->cd();
	for(int i=0; i<(int)pythia_pt->size(); i++) pythia_pt->at(i)->Write();
	for(int i=0; i<(int)pythia_lead_pt->size(); i++) pythia_lead_pt->at(i)->Write();
	storage->cd();
	
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
	PlotPhotonObs(herwig_photon_array->at(0), pythia_photon_array->at(0), J_pt, 0);
	TCanvas* J_l_pt=new TCanvas("photon_l_pt", "photon_l_pt");
	PlotPhotonObs(herwig_lead_photon_array->at(0), pythia_lead_photon_array->at(0), J_l_pt, 1);
	TCanvas* J_e=new TCanvas("photon_e", "photon_e");
	PlotPhotonObs(herwig_photon_array->at(1), pythia_photon_array->at(1), J_e, 0);
	TCanvas* J_l_e=new TCanvas("photon_l_e", "photon_l_e");
	PlotPhotonObs(herwig_lead_photon_array->at(1), pythia_lead_photon_array->at(1), J_l_e, 1);
	TCanvas* J_phi=new TCanvas("photon_phi", "photon_phi");
	PlotPhotonObs(herwig_photon_array->at(2), pythia_photon_array->at(2), J_phi, 0);
	TCanvas* J_l_phi=new TCanvas("photon_l_pt", "photon_l_pt");
	PlotPhotonObs(herwig_lead_photon_array->at(2), pythia_lead_photon_array->at(2), J_l_phi, 1);
	TCanvas* J_eta=new TCanvas("photon_eta", "photon_eta");
	PlotPhotonObs(herwig_photon_array->at(3), pythia_photon_array->at(3), J_eta, 0);
	TCanvas* J_l_eta=new TCanvas("photon_l_eta", "photon_l_eta");
	PlotPhotonObs(herwig_lead_photon_array->at(3), pythia_lead_photon_array->at(3), J_l_eta, 1);
	TCanvas* J_N=new TCanvas("photon_N", "photon_N");
	PlotPhotonObs(herwig_n_photons, pythia_n_photons, J_N, 0);

	Canvi->push_back(J_pt);
	Canvi->push_back(J_l_pt);
	Canvi->push_back(J_e);
	Canvi->push_back(J_l_e);
	Canvi->push_back(J_phi);
	Canvi->push_back(J_l_phi);
	Canvi->push_back(J_eta);
	Canvi->push_back(J_l_eta);
	Canvi->push_back(J_N);
	
	TCanvas* J_d_pt=new TCanvas("direct_photon_pt", "direct_photon_pt");
	PlotPhotonObs(herwig_direct_photon_array->at(0), pythia_direct_photon_array->at(0), J_d_pt, 2);
	TCanvas* J_f_pt=new TCanvas("frag_photon_pt", "frag_photon_pt");
	PlotPhotonObs(herwig_frag_photon_array->at(0), pythia_frag_photon_array->at(0), J_f_pt, 3);
	TCanvas* J_d_e=new TCanvas("direct_photone", "direct_photone");
	PlotPhotonObs(herwig_direct_photon_array->at(1), pythia_direct_photon_array->at(1), J_d_e, 2);
	TCanvas* J_f_e=new TCanvas("frag_photon_e", "frag_photon_e");
	PlotPhotonObs(herwig_frag_photon_array->at(1), pythia_frag_photon_array->at(1), J_f_e, 3);
	TCanvas* J_d_phi=new TCanvas("direct_photonphi", "direct_photonphi");
	PlotPhotonObs(herwig_direct_photon_array->at(2), pythia_direct_photon_array->at(2), J_d_phi, 2);
	TCanvas* J_f_phi=new TCanvas("frag_photon_pt", "frag_photon_pt");
	PlotPhotonObs(herwig_frag_photon_array->at(2), pythia_frag_photon_array->at(2), J_f_phi, 3);
	TCanvas* J_d_eta=new TCanvas("direct_photoneta", "direct_photoneta");
	PlotPhotonObs(herwig_direct_photon_array->at(3), pythia_direct_photon_array->at(3), J_d_eta, 2);
	TCanvas* J_f_eta=new TCanvas("frag_photon_eta", "frag_photon_eta");
	PlotPhotonObs(herwig_frag_photon_array->at(3), pythia_frag_photon_array->at(3), J_f_eta, 3);
	TCanvas* J_d_N=new TCanvas("direct_photonN", "direct_photonN");
	PlotPhotonObs(herwig_direct_n_photons, pythia_direct_n_photons, J_d_N, 2);
	TCanvas* J_f_N=new TCanvas("frag_photonN", "frag_photonN");
	PlotPhotonObs(herwig_direct_n_photons, pythia_frag_n_photons, J_f_N, 3);

	Canvi->push_back(J_d_pt);
	Canvi->push_back(J_f_pt);
	Canvi->push_back(J_d_e);
	Canvi->push_back(J_f_e);
	Canvi->push_back(J_d_phi);
	Canvi->push_back(J_f_phi);
	Canvi->push_back(J_d_eta);
	Canvi->push_back(J_f_eta);
	Canvi->push_back(J_d_N);
	Canvi->push_back(J_f_N);
	return;
	
}
void PlotPhotonJetPlots(TDirectory* herwig_photons, TDirectory* pythia_photons, std::vector<TCanvas*>* Canvi)
{
	//mainly 2D plots so I have to change the approach a bit here 
	TDirectory* H_pj = (TDirectory*)herwig_photons->GetDirectory("Photon-Jets");
	std::vector<TH2F*>* herwig_pt		= new std::vector<TH2F*> ();
	std::vector<TH2F*>* herwig_eta		= new std::vector<TH2F*> ();
	std::vector<TH2F*>* herwig_phi		= new std::vector<TH2F*> ();
	std::vector<TH2F*>* herwig_e		= new std::vector<TH2F*> ();
	std::vector<TH1F*>* herwig_dphi		= new std::vector<TH1F*> ();
	//CollectPhotonJets(H_pj, herwig_pt, herwig_eta, herwig_phi, herwig_e, herwig_dphi);

	TDirectory* P_pj = (TDirectory*)pythia_photons->GetDirectory("Photon-Jets");
	std::vector<TH2F*>* pythia_pt		= new std::vector<TH2F*> ();
	std::vector<TH2F*>* pythia_eta		= new std::vector<TH2F*> ();
	std::vector<TH2F*>* pythia_phi		= new std::vector<TH2F*> ();
	std::vector<TH2F*>* pythia_e		= new std::vector<TH2F*> ();
	std::vector<TH1F*>* pythia_dphi		= new std::vector<TH1F*> ();
	//CollectPhotonJets(P_pj, pythia_pt, pythia_eta, pythia_phi, pythia_e, pythia_dphi);
	
	TCanvas* J_pt=new TCanvas("photon_jet_pt");
	TCanvas* J_h_pt=new TCanvas("herwig_photon_jet_pt");
	TCanvas* J_p_pt=new TCanvas("pythia_photon_jet_pt");
	PlotPhotonJetObs(herwig_pt, pythia_pt, J_pt, J_h_pt, J_p_pt); 
	
	TCanvas* J_eta=new TCanvas("photon_jet_eta");
	TCanvas* J_h_eta=new TCanvas("photon_herwig_jet_eta");
	TCanvas* J_p_eta=new TCanvas("pythia_photon_jet_eta");
	PlotPhotonJetObs(herwig_eta, pythia_eta, J_eta, J_h_eta, J_p_eta); 

	TCanvas* J_phi=new TCanvas("photon_herwig_jet_phi");
	TCanvas* J_h_phi=new TCanvas("photon_herwig_jet_phi");
	TCanvas* J_p_phi=new TCanvas("pythia_photon_jet_phi");
	PlotPhotonJetObs(herwig_phi, pythia_phi, J_phi, J_h_phi, J_p_phi); 

	TCanvas* J_e=new TCanvas("photon_herwig_jet_e");
	TCanvas* J_h_e=new TCanvas("photon_herwig_jet_e");
	TCanvas* J_p_e=new TCanvas("pythia_photon_jet_e");
	PlotPhotonJetObs(herwig_e, pythia_e, J_e, J_h_e, J_p_e); 

       	TCanvas* J_dphi=new TCanvas("photon_jet_dphi");
	PlotJetObs(herwig_dphi, pythia_dphi, J_dphi);
	Canvi->push_back(J_dphi);
	return;
}
void DoAllThePlotting(TFile* herwig_file, TFile* pythia_file, std::string trigger_tag="Jet30", float herwig_xs=1., float pythia_xs=1.)
{
	//this does all the plotting of Pythia vrs herwig
	SetsPhenixStyle();	
	storage=new TFile(std::format("{}_scaled_pt.root", trigger_tag).c_str(), "RECREATE");
	conf =  new HerwigQAPlottingConfig(herwig_xs, pythia_xs);
	style_points=new Skaydis_colors();
	gStyle->SetPalette(100, style_points->Trans_gradient_PT);	
	conf->ExtractType(herwig_file);
	std::map<std::string, TDirectory*> top_dirs;
	std::vector<TCanvas*>* Canvi =new std::vector<TCanvas*> ();
	tag=trigger_tag;
	//find the relevant directories
	if(conf->isJet() || trigger_tag.find("MB") != std::string::npos){
		TDirectory* HJets=(TDirectory*)herwig_file->GetDirectory("Jets");
		TDirectory* PJets=(TDirectory*)pythia_file->GetDirectory("Jets");
		top_dirs["herwig jets"]=HJets;
		top_dirs["pythia jets"]=PJets;
		PlotJetPlots(HJets, PJets, Canvi);
	}
	if(conf->isPhoton() || trigger_tag.find("MB") != std::string::npos)
	{
		TDirectory* HPh=(TDirectory*)herwig_file->GetDirectory("Photons");
		TDirectory* PPh=(TDirectory*)pythia_file->GetDirectory("Photons");
		top_dirs["herwig photons"]=HPh;
		top_dirs["pythia photons"]=PPh;
		PlotPhotonPlots(HPh, PPh, Canvi);
	}
	if(conf->isPhoton() && conf->isJet())
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

#endif

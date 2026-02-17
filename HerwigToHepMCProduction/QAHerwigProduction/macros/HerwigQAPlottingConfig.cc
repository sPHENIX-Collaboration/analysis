#include "HerwigQAPlottingConfig.h"

HerwigQAPlottingConfig::HerwigQAPlottingConfig(float herwig_xs, float pythia_xs)
, HW_XS(herwig_xs)
, PY_XS(pythia_xs)
{
	//This just sets up everything to do the little repetative tasks that go into plotting 
}
void HerwigQAPlottingConfig::ExtractType(TFile* input_file)
{
	std::string file_name = input_file->GetName();
	if(file_name.find("erwig") != std::string::npos) this->herwig=true;
	else if (file_name.find("ythia") != std::string::npos) this->pythia=true;
	TDirectory* jets_dir = (TDirectory*)input_file->GetDirectory("Jets");
	TDirectory* photon_dir = (TDirectory*)input_file->GetDirectory("Photons");
	if(jets_dir) this->jet=true;
	if(photon_dir) this->photon=true;
	return;
}

void HerwigQAPlottingConfig::DumpHistos(std::string output_file, std::vector<TCanvas*>* Canvi)
{
	//take all relevant canvases passed in the vector and dump them into the output file
	for(int i=0; i<(int) Canvi->size(); i++)
	{
		if(i==0) Canvi->at(i)->Print(std::format("{}(", output_file).c_str(), "pdf");
		else if (i== ((int)Canvi->size())-1) Canvi->at(i)->Print(std::format("{})", output_file).c_str(), "pdf");
		else Canvi->at(i)->Print( output_file.c_str(), "pdf");
	}
	return;
}

std::vector<TPad*>* HerwigQAPlottingConfig::AddPads(TCanvas* c1) 
{
	c1->cd();
	TPad* p1=new TPad("p1", "p1", 0, 0, 1, 0.33);
	TPad* p2=new TPad("p2", "p2", 0, 0.35, 1, 1);
	c1->cd();
	p1->Draw();
	p2->Draw();
	std::vector<TPad*>* Pads =new std::vector<TPad*>{p1, p2};
	return Pads;
}

void HerwigQAPlottingConfig::SetLegend(TLegend* l1) 
{
	l1->SetFillColor(0);
	l1->SetFillStyle(0);
	l1->SetBorderSize(0);
	l1->SetTextSize(0.03f);
	return;
}

void HerwigQAPlottingConfig::SetsPhenixHeaderLegend(TLegend* l1, std::string triggerinfo) 
{
	SetLegend(l1);
	l1->AddEntry("", "#bf{#it{sPHENIX}} Internal", "");
	l1->AddEntry("", "Herwig HepMC Production QA", "");
	l1->AddEntry("", "Herwig7.2", "");
 	l1->AddEntry("", "Reference: fully produced Pythia8", "");
	l1->AddEntry("", std::format("{}, k_{T}^{min} [hat{p}_{T}] = {} GeV", triggerinfo, lookupTrigger(triggerinfo)).c_str(), "");
	return;
}

float HerwigQAPlottingConfig::lookupTrigger(std::string trigger)
{
	float trigger_val = 0.;
	if(trigger.find("MB") != std::string::npos) trigger_val=0.;
	else if(trigger.find("PhotonJet5") != std::string::npos) trigger_val = 5.;
	else if(trigger.find("PhotonJet10") != std::string::npos) trigger_val = 10.;
	else if(trigger.find("PhotonJet20") != std::string::npos) trigger_val = 20.;
	else if(trigger.find("Jet5") != std::string::npos) trigger_val=0.;
	else if(trigger.find("Jet15") != std::string::npos) trigger_val=7.;
	else if(trigger.find("Jet20") != std::string::npos) trigger_val=10.;
	else if(trigger.find("Jet30") != std::string::npos) trigger_val=17.5;
	else if(trigger.find("Jet40") != std::string::npos) trigger_val=20.;
	else if(trigger.find("Jet50") != std::string::npos) trigger_val=29.;
	return trigger_val;
}
TH1F* HerwigQAPlottingConfig::GetRatioPlot(TH1F* signal, TH1F* refernce)
{
	TH1F* ratio_clone = (TH1F*) signal->Clone();
	ratio_clone->Divide(refernce);
	ratio_clone->SetTitle("Hewig / Pythia");
	return ratio_clone;
}
void HerwigQAPlottingConfig::ScaleXS(std::vector<TH1F*>* histograms, bool isHerwig)
{
	for(int i=0; i<(int)histograms->size(); i++)
	{
		if(isHerwig) histograms->at(i)->Scale(HW_XS);
		else historgrams->at(i)->Scale(PY_XS);
	}
}


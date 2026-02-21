#include "HerwigQAPlottingConfig.h"

HerwigQAPlottingConfig::HerwigQAPlottingConfig(float herwig_xs, float pythia_xs)
: HW_XS(herwig_xs)
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
std::vector<TPad*>* HerwigQAPlottingConfig::Canvas2DDivide(TCanvas* c1)
{
	c1->cd();
	TPad* p1=new TPad("p1", "p1", 0, 0, 0.48, 0.30);
	TPad* p2=new TPad("p2", "p2", 0, 0.33, 0.48, 0.63);
	TPad* p3=new TPad("p3", "p3", 0, 0.66, 0.48, 0.1);
	TPad* p4=new TPad("p4", "p4", 0.52, 0, 1, 0.30);
	TPad* p5=new TPad("p5", "p5", 0.52, 0.33, 1, 0.63);
	TPad* p6=new TPad("p6", "p6", 0.52, 0.66, 1, 0.1);
	c1->cd();
	p1->Draw();
	p2->Draw();
	p3->Draw();
	p4->Draw();
	p5->Draw();
	p6->Draw();
	std::vector<TPad*>* Pads =new std::vector<TPad*>{p1, p4, p2, p5, p3, p6};
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
	l1->AddEntry("", Form("%s, k_{T}^{min} [#hat{p}_{T}] = %.2g GeV", triggerinfo.c_str(), lookupTrigger(triggerinfo)), "");
	l1->SetTextSize(0.05f);
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
TH1I* HerwigQAPlottingConfig::GetRatioPlot(TH1I* signal, TH1I* refernce)
{
	TH1I* ratio_clone = (TH1I*) signal->Clone();
	ratio_clone->Divide(refernce);
	ratio_clone->SetTitle("Hewig / Pythia");
	return ratio_clone;
}
TH2F* HerwigQAPlottingConfig::GetRatioPlot(TH2F* signal, TH2F* refernce)
{
	TH2F* ratio_clone = (TH2F*) signal->Clone();
	ratio_clone->Divide(refernce);
	return ratio_clone;
}
std::vector<TH2F*>* HerwigQAPlottingConfig::GetRatioPlots(std::vector<TH2F*>* signal, std::vector<TH2F*>* reference)
{
	std::vector<TH2F*>* ratios = new std::vector<TH2F*> ();
	for(int i = 0; i < (int) signal->size(); i++)
	{
		TH2F* ratio = GetRatioPlot(signal->at(i), reference->at(i));
		ratios->push_back(ratio);
	}
	return ratios;
}
void HerwigQAPlottingConfig::ScaleXS(std::vector<TH1I*>* histograms, bool isHerwig)
{
	for(int i=0; i<(int)histograms->size(); i++)
	{
		if(isHerwig) histograms->at(i)->Scale(HW_XS);
		else histograms->at(i)->Scale(PY_XS);
		histograms->at(i)->SetYTitle("#sigma [nb]");
	}
}
void HerwigQAPlottingConfig::ScaleXS(std::vector<TH1F*>* histograms, bool isHerwig)
{
	for(int i=0; i<(int)histograms->size(); i++)
	{
		if(isHerwig) histograms->at(i)->Scale(HW_XS);
		else histograms->at(i)->Scale(PY_XS);
		histograms->at(i)->SetYTitle("#sigma [nb]");
	}
}
void HerwigQAPlottingConfig::ScaleXS(TH1I* histograms, bool isHerwig)
{
	if(isHerwig) histograms->Scale(HW_XS);
	else histograms->Scale(PY_XS);	
	histograms->SetYTitle("#sigma [nb]");
}
void HerwigQAPlottingConfig::ScaleXS(TH1F* histograms, bool isHerwig)
{
	if(isHerwig) histograms->Scale(HW_XS);
	else histograms->Scale(PY_XS);
	histograms->SetYTitle("#sigma [nb]");
}


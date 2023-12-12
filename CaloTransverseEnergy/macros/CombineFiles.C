#include <vector>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TIter.h>
#include <string>
#include <iostream>
#include <filesystem>
//#include "caloplots.h"
//#include "plots.h"
#include <map> 

int CombineFiles(int run)
{
	std::vector<std::string> file_names;
	//load in all the files and then take the relevant data to combine them 
	std::string path="../data_output";
	for(const auto & entry : std::filesystem::directory_iterator(path))
	{
		if(std::string(entry).find(std::string(run)) != std::string::npos)
		{
			filenames.push_back(entry);
		}
	}
	//TTree* data=new TTree("CALO_ET", "CALOET");
	std::map<int, std::map<std::string, std::vector<TH1F*>>> zPLTS;
	for(auto s:file_names){
		TFile* f=new TFile(s.c_std(), "READ");
		TList* l=f->GetListofKeys();
		TIter iter(list);
		TH1F* hist;
		TKey* key;
		TTree* header;
		std::map<int, std::map<float, std::vector<TH1F*>> zP;
		while(key= (TKey*)iter())
		{
			if(key->GetClassName() == "TH1F"){
				hist=(TH1F*)key->ReadObject()
				if(hist){
					std::string s (hist->GetName());
					std::stringstream subs (s);
					std::string brok, caloname;
					bool z_loc=false;
					int z=0;
					while(std::getline(subs, brok, '_')){
						if(brok.find("OHCAL") != std::string::npos){
							caloname="oh";
						}
						if(brok.find("IHCAL") != std::string::npos){
							caloname="ih";
						}
						if(brok.find("EMCAL") != std::string::npos){
							caloname="em"
						}
						if(z_loc){
							z=std::stoi(brok);
							z_loc=false;
						}
						if(brok.find("z") !=std::string::npos) z_loc=true;
					}
					zP[z][caloname].push_back(hist);
				}
			if(key->GetClassName() == "TTree"){
				header=(TTree*) key->ReadObject();
				
	}
			
	TFile* f1=new TFile(Form("CaloTransverseEnergy_run_%i.root", run), "RECREATE");
}
	

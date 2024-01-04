#include <vector>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
//#include <TIter.h>
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
		if(entry.path().string().find(std::to_string(run)) != std::string::npos)
		{
			file_names.push_back(entry.path().string());
		}
	}
	//TTree* data=new TTree("CALO_ET", "CALOET");
	std::map<int, std::map<std::string,std::map<std::string, std::vector<std::pair<int, TH1F*>>>>> zPs;
	std::vector<int> event_numbers;
	for(auto s:file_names){
		TFile* f=new TFile(s.c_str(), "READ");
		TList* l=f->GetListOfKeys();
		std::cout<<"There are " <<l->GetEntries() <<" many keys in this file" <<std::endl;
		TIter iter(l);
		TH1F* hist1;
		TKey* key;
		TTree* header;
		int n_evts;
		std::map<int, std::map<std::string, std::vector<TH1F*>>> zP;
		while((key=(TKey*)iter()))
		{
			std::cout<<"Working on a key" <<std::endl;
			if(std::string(key->GetClassName()).find("TH1F")!=std::string::npos){
				hist1=(TH1F*)key->ReadObj();
				if(hist1){
					TH1F* hist=(TH1F*)hist->Clone();		
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
							caloname="em";
						}
						if(z_loc){
							z=std::stoi(brok);
							z_loc=false;
						}
						if(brok.find("z") !=std::string::npos) z_loc=true;
					}
					zP[z][caloname].push_back(hist);
				}
			if(std::string(key->GetClassName()).find("TTree") != std::string::npos){
				try{header=(TTree*) key->ReadObj();}
				catch(std::exception& e){continue;}
				std::string s (header->GetName());
				std::cout<<s<<std::endl;
				if(s.find("HEADER") != std::string::npos){
					header->SetBranchAddress("number of events", &n_evts);
					header->GetEntry(0);
					event_numbers.push_back(n_evts);
				}
			}
		}
		for(auto m:zP){
			for(auto n:m.second){
				for(auto h:n.second){
					zPs[m.first][n.first][h->GetName()].push_back(std::make_pair(n_evts, h));
					}
			}
		}
	}
	f->Close();
			
}
	TFile* f1=new TFile(Form("Transverse_Energy_Run_%d_Combined.root", run), "RECREATE");
	TH1F* hclone;
	bool first=true;
	int total_events=0;
	for(auto m:zPs){
			for(auto n:m.second){
				for(auto p:n.second){
					for(auto r:p.second){
						if(std::string(r.second->GetYaxis()->GetTitle()).find("<") !=std::string::npos) r.second->Scale(r.first);
						if(first){
							 hclone=(TH1F*) r.second->Clone();
							 first=false;
						}
						else hclone->Add(r.second);
						total_events+=r.first;
					}
				 	if(std::string(hclone->GetYaxis()->GetTitle()).find("<") != std::string::npos) hclone->Scale(1/total_events);
					first=true;
					total_events=0;
				}
			}
	}
	//f1->Write();
	f1->Close();
	return 0;								
}

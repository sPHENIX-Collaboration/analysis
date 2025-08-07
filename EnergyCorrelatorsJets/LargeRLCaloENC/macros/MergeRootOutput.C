#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <algorithm>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TString.h>
#include <fstream>
std::map<std::string, std::string> getListofObjects(std::string filename)
{
	TFile f1 (filename.c_str());
	std::map<std::string, std::string> keys_and_type;
	for(TObject* keys: *f1.GetListOfKeys()){
		auto key = dynamic_cast<TKey*>(keys);
		keys_and_type[std::string(key->GetName())] = std::string(key->GetClassName());
	}
	return keys_and_type;
} 

int MergeRootOutput(std::string file_naming_style, std::string run_string="21", std::string n_files_string="0")
{
	int run_number, n_files;
	run_number=std::stoi(run_string);
	n_files=std::stoi(n_files_string);
	std::vector<std::string> filenames;
	//collect all filenames of the naming convention and run number
	for(int i=0; i<n_files; i++){
		std::string filename = file_naming_style + run_string + "-"+std::to_string(i)+".root";
		filenames.push_back(filename);
	}
	std::string output_filename=file_naming_style+run_string+".root";
	std::map<std::string, std::string> output_names;
	output_names=getListofObjects(filenames.at(0));	
//	TChain* c=new TChain("cuts_r04");
	std::map<std::string, TH1F*> th1_to_merge_into;
	std::map<std::string, TH2F*> th2_to_merge_into;
	TFile* f1=new TFile(filenames.at(0).c_str(), "READ"); 
	for(auto i:output_names)
	{
		int n_evts=0;
		std::string n_th1_name="n_"; 
		std::string stripped_name=i.first;
		if(stripped_name.find("Towards") != std::string::npos){
			stripped_name.substr(stripped_name.find("Towards"));
		}
		else if(stripped_name.find("Away") != std::string::npos){
			stripped_name.substr(stripped_name.find("Away"));
		}
		else if(stripped_name.find("Transverse") != std::string::npos){
			stripped_name.substr(stripped_name.find("Transverse"));
		}
		else if(stripped_name.find("Full") != std::string::npos){
			stripped_name.substr(stripped_name.find("Full"));
		}
		n_th1_name+=stripped_name;
		if(n_th1_name.find("CAL") != std::string::npos) {
			auto temp=f1->Get(n_th1_name.c_str());
			if(temp){
				TH1F* h_N=(TH1F*)temp->Clone();
				if(h_N) n_evts=h_N->GetEntries();
			}
		}
		if(n_evts == 0 ) n_evts=1;
		if(i.second.find("TH1F") != std::string::npos ){
			auto temp=f1->Get(i.first.c_str());
			if(temp){
			auto h1=(TH1F*)temp->Clone();
			if(i.first.find("e2c") != std::string::npos || i.first.find("e3c")!= std::string::npos){
				h1->Scale(n_evts);
			}
			th1_to_merge_into[i.first]=(TH1F*)h1->Clone();
		}
		}	
		if(i.second.find("TH2F") != std::string::npos){
			auto temp=f1->Get(i.first.c_str());
			if(temp){
			auto h1=(TH2F*)temp->Clone();
			if(i.first.find("e2c") != std::string::npos || i.first.find("e3c")!= std::string::npos){
				h1->Scale(n_evts);
			}
			th2_to_merge_into[i.first]=(TH2F*)h1->Clone();
			}
			}
	}	
	f1->Close();
	std::cout<<"Dealt with the first file"<<std::endl;
	for(auto j:filenames){
		if(j==filenames[0]) continue;
		TFile* f=new TFile(j.c_str(), "READ");
		for(auto i:output_names)
		{
			int n_evts=0;
			std::string n_th1_name="n_"; 
			std::string stripped_name=i.first;
			if(stripped_name.find("Towards") != std::string::npos){
				stripped_name.substr(stripped_name.find("Towards"));
			}
			else if(stripped_name.find("Away") != std::string::npos){
				stripped_name.substr(stripped_name.find("Away"));
			}
			else if(stripped_name.find("Transverse") != std::string::npos){
				stripped_name.substr(stripped_name.find("Transverse"));
			}
			else if(stripped_name.find("Full") != std::string::npos){
				stripped_name.substr(stripped_name.find("Full"));
			}
			n_th1_name+=stripped_name;
			if(n_th1_name.find("CAL") != std::string::npos) {
				auto temp=f1->Get(n_th1_name.c_str());
				if(temp){
					TH1F* h_N=(TH1F*)temp->Clone();
					if(h_N) n_evts=h_N->GetEntries();
				}
			}
			if(n_evts == 0 ) n_evts=1;
			if(i.second.find("TH1F") != std::string::npos){
				auto temp=f1->Get(i.first.c_str());
				if(temp){
					auto h1=(TH1F*)temp->Clone();
				if(i.first.find("e2c") != std::string::npos || i.first.find("e3c")!= std::string::npos){
					h1->Scale(n_evts);
				}
				th1_to_merge_into[i.first]->Add(h1);
			}
			}
			else if(i.second.find("TH2F") != std::string::npos){
				auto temp=f1->Get(i.first.c_str());
				if(temp){
					auto h1=(TH1F*)temp->Clone();
				if(i.first.find("e2c") != std::string::npos || i.first.find("e3c")!= std::string::npos){
					h1->Scale(n_evts);
				}
				th2_to_merge_into[i.first]->Add(h1);
				}
			}
		}	
		f->Close();
	}	
	std::cout<<"all files merged" <<std::endl;
/*	for(std::string fn : filenames){
		TString fnRs=TString(fn);
		if(gSystem->AccessPathName(fnRs.Data(), kFileExists)) c->Add(fnRs);
	}*/
	TFile* f=new TFile(output_filename.c_str(), "RECREATE");
	for(auto i:output_names)
	{
		int n_evts=0;
		std::string n_th1_name="n_"; 
		std::string stripped_name=i.first;
		if(stripped_name.find("Towards") != std::string::npos){
			stripped_name.substr(stripped_name.find("Towards"));
		}
		else if(stripped_name.find("Away") != std::string::npos){
			stripped_name.substr(stripped_name.find("Away"));
		}
		else if(stripped_name.find("Transverse") != std::string::npos){
			stripped_name.substr(stripped_name.find("Transverse"));
		}
		else if(stripped_name.find("Full") != std::string::npos){
			stripped_name.substr(stripped_name.find("Full"));
		}
		std::cout<<__LINE__<<std::endl;
		n_th1_name+=stripped_name;
		if(n_th1_name.find("CAL") != std::string::npos) {
			if(th1_to_merge_into.find(n_th1_name) == th1_to_merge_into.end()) continue;
			auto temp=th1_to_merge_into[n_th1_name];
			if(temp){
				TH1F* h_N=(TH1F*)temp->Clone();
				if(h_N) n_evts=h_N->GetEntries();
			}
		}
		//std::cout<<__LINE__<<std::endl;
		if(n_evts == 0 ) n_evts=1;
		if(i.second.find("TH1F")!=std::string::npos){
			if(th1_to_merge_into.find(i.first) == th1_to_merge_into.end()) continue;
			auto h1=th1_to_merge_into[i.first];
			if(i.first.find("e2c") != std::string::npos || i.first.find("e3c")!= std::string::npos){
				h1->Scale(1/(float)n_evts);
			}
			if(h1){
		//		std::cout<<i.first<<std::endl;
				 h1->Write();
			}
		}
		else if(i.second.find("TH2F")!=std::string::npos){
			if(th2_to_merge_into.find(i.first) == th2_to_merge_into.end()) continue;
			auto h1=th2_to_merge_into[i.first];
			if(i.first.find("e2c") != std::string::npos || i.first.find("e3c")!= std::string::npos){
				h1->Scale(1/(float)n_evts);
			}
			if(h1)	h1->Write();
		}
	}	
	std::cout<<"all normalization done" <<std::endl;
	std::cout<<"All 1d histos done" <<std::endl;
//	c->Merge(f, 1001);
	f->Close();
	return 0;
	
}
#endif

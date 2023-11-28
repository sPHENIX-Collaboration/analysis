#include <vector>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <string>
#include <iostream>
#include <filesystem>

int CombineFiles(int run)
{
	std::vector<std::string> file_names;
	//load in all the files and then take the relevant data to combine them 
	TFile* f1=new TFile(Form("CaloTransverseEnergy_run_%i.root", run), "RECREATE");
	std::string path="../data_output";
	for(const auto & entry : std::filesystem::directory_iterator(path))
	{
		if(std::string(entry).find(std::string(run)) != std::string::npos)
		{
			filenames.push_back(entry);
		}
	}
	TTree* data=new TTree("CALO_ET", "CALOET");
	data->
			
}
	

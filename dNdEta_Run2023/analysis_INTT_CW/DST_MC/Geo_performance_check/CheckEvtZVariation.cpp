#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TLegend.h>

#include "sPhenixStyle.h"

vector<string> read_list(string folder_direction, string file_name_in)
{
    vector<string> file_list;
    string list_buffer;
    ifstream data_list;
	data_list.open((folder_direction + "/" + file_name_in).c_str());

   file_list.clear();

	while (1)
	{
		data_list >> list_buffer;
		if (!data_list.good())
		{
			break;
		}
		file_list.push_back(list_buffer);
	}
	cout<<"size in the" <<file_name_in<<": "<<file_list.size()<<endl;

    return file_list;
}

int main (int argc, char* argv[])
{
    // note : Check if the correct number of arguments is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " N_event" << std::endl;
        return 1;
    }

    cout<<"running the folder_"<<argv[1]<<endl;

    // note : Convert the second argument (argv[1]) to an integer
    int folder_index = std::atoi(argv[1]);

    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/Geo_Z_scan_trial_2/complete_file/folder_" + to_string(folder_index);
    string input_file_name = "file_list.txt"; 
    string output_directory = input_directory + "/folder_merged_results";

    double reco_zvtx;
    double true_zvtx;
    int centrality_bin;
    int nclu_inner;
    int nclu_outer;
    bool good_zvtx_tag;

    TChain * chain_in = new TChain("tree_Z");
    vector<string> folder_file_list = read_list(input_directory, input_file_name);
    for(string file_name : folder_file_list) {chain_in -> Add(file_name.c_str());}

    cout<<"total events : "<<chain_in -> GetEntries()<<endl;

    chain_in -> SetBranchAddress("nclu_inner", &nclu_inner);
    chain_in -> SetBranchAddress("nclu_outer", &nclu_outer);
    chain_in -> SetBranchAddress("good_zvtx_tag", &good_zvtx_tag);
    chain_in -> SetBranchAddress("Centrality_bin", &centrality_bin);
    chain_in -> SetBranchAddress("MC_true_zvtx", &true_zvtx);
    chain_in -> SetBranchAddress("LB_Gaus_Mean_mean", &reco_zvtx);
    
    map<int,int> centrality_map = {
        {5, 0},
        {15, 1},
        {25, 2},
        {35, 3},
        {45, 4},
        {55, 5},
        {65, 6},
        {75, 7},
        {85, 8},
        {95, 9}
    };

    TFile * file_out = new TFile((output_directory + "/combine_file.root").c_str(), "RECREATE");
    TH1F * zvtx_diff[11];

    for (int i = 0; i < 11; i++)
    {
        zvtx_diff[i] = new TH1F(("zvtx_diff_M_" + to_string(i)).c_str(), ("zvtx_diff_M_" + to_string(i)+";Reco Z - True Z [mm];Entry").c_str(), 100, -30, 30); // note : unit mm
    }

    for (int event_i = 0; event_i < chain_in -> GetEntries(); event_i++)
    {
        chain_in -> GetEntry(event_i);
        if (nclu_inner == -1 || nclu_outer == -1) {continue;}
        if (good_zvtx_tag == false) {continue;}
        zvtx_diff[centrality_map[centrality_bin]] -> Fill(reco_zvtx - true_zvtx);
        zvtx_diff[10]                             -> Fill(reco_zvtx - true_zvtx);
    }

    file_out -> cd();
    for (int i = 0; i < 11; i++)
    {
        zvtx_diff[i] -> Write();
    }

    file_out->Close();
}


// #ifndef CheckEvtZVariance_H
// #define CheckEvtZVariance_H



// class CheckEvtZVariance
// {
//     public:
//         CheckEvtZVariance(string input_directory, string input_file_name, string output_directory);
//         ~CheckEvtZVariance();
//         void process_files();
//         void print_plot();

//     protected:
//         string input_directory;
//         string input_file_name; 
//         string output_directory;

//         vector<string> folder_file_list;

//         TChain * chain;
//         vector<string> read_list(string folder_direction, string file_name_in);
        
// };



// CheckEvtZVariance::CheckEvtZVariance(string input_directory, string input_file_name, string output_directory)
// : input_directory(input_directory), input_file_name(input_file_name), output_directory(output_directory)
// {
//     chain = new TChain("EventTree");
//     folder_file_list.clear();
    
//     return;
// }

// void CheckEvtZVariance::process_files()
// {
//     for (int folder_i = 0; folder_i < 450; folder_i++)
//     {
//         string folder_direction = input_directory + "/folder_"+ to_string(folder_i);
        
//         vector<string> file_list = read_list(folder_direction, input_file_name);
//         for (int file_i = 0; file_i < file_list.size(); file_i++)
//         {
//             chain -> Add((folder_direction + "/" + file_list[file_i]).c_str());
//         }

//         file_list.clear();
//     }
// }

// #endif 
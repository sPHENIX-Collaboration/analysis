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

int main ()
{
    string input_directory_mother = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/Geo_test/evt_tracklet/complete_file"; 
    string input_directory_merged = input_directory_mother + "/merged_result";
    string input_file_name = "file_list.txt"; 
    string input_file_name_standard = "INTT_final_hist_info_999999.root";
    string output_directory = input_directory_merged;
    
    vector<string> hist_to_be_called = {
        "exclusive_NClus_inner",
        "exclusive_NClus_outer",
        "exclusive_NClus_sum",
        "exclusive_cluster_inner_eta",
        "exclusive_cluster_inner_phi",
        "exclusive_cluster_outer_eta",
        "exclusive_cluster_outer_phi",
        "exclusive_cluster_all_eta",
        "exclusive_cluster_all_phi"
        // "exclusive_tight_tracklet_eta",
        // "exclusive_tight_tracklet_phi",
        // "exclusive_loose_tracklet_eta",
        // "exclusive_loose_tracklet_phi"
    };

    map<string, TH1F *> map_hist_1D_standard; map_hist_1D_standard.clear();
    map<string, TH2F *> map_hist_2D; map_hist_2D.clear();

    vector<string> folder_file_list = read_list(input_directory_merged, input_file_name);

    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1","c1",950,800);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.04);

    TFile * file_in_standard = TFile::Open((input_directory_mother + "/" + input_file_name_standard).c_str());

    for (string hist_name : hist_to_be_called) {
        map_hist_1D_standard[hist_name] = (TH1F *) file_in_standard -> Get(hist_name.c_str());

        map_hist_2D[hist_name] = new TH2F("", "", map_hist_1D_standard[hist_name] -> GetNbinsX(), map_hist_1D_standard[hist_name] -> GetXaxis() -> GetXmin(), map_hist_1D_standard[hist_name] -> GetXaxis() -> GetXmax(), 1000, 0, map_hist_1D_standard[hist_name] -> GetBinContent(map_hist_1D_standard[hist_name] -> GetMaximumBin()) * 1.8);
        map_hist_2D[hist_name] -> GetXaxis() -> SetNdivisions(705);
        map_hist_2D[hist_name] -> GetXaxis() -> SetTitle(map_hist_1D_standard[hist_name] -> GetXaxis() -> GetTitle());
        map_hist_2D[hist_name] -> GetYaxis() -> SetTitle(map_hist_1D_standard[hist_name] -> GetYaxis() -> GetTitle());
    }

    for (int file_i = 0; file_i < folder_file_list.size(); file_i++)
    {
        cout<<"file_i: "<<file_i<<endl;
        
        TFile * file_in = TFile::Open((folder_file_list[file_i]).c_str());
        for (string hist_name : hist_to_be_called) {
            TH1F * hist_1D_in = (TH1F *) file_in -> Get(hist_name.c_str());
            for (int bin_i = 1; bin_i <= hist_1D_in -> GetNbinsX(); bin_i++) {
                map_hist_2D[hist_name] -> Fill(hist_1D_in -> GetBinCenter(bin_i), hist_1D_in -> GetBinContent(bin_i));
            }
            hist_1D_in -> Delete();
        }

        file_in -> Close();
    }

    
    // note : draw the plots
    c1 -> cd();
    for (string hist_name : hist_to_be_called){
        map_hist_2D[hist_name] -> Draw("colz0");
        map_hist_1D_standard[hist_name] -> SetLineColor(2);
        map_hist_1D_standard[hist_name] -> Draw("hist same");
        c1 -> Print((output_directory + "/" + hist_name + ".pdf").c_str());
        c1 -> Clear();
    }

}
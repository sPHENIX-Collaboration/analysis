#include <iostream>
#include <fstream>
using namespace std;

#include <vector>

#include <TFile.h>
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

int main()
{
    string input_folder = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/Geo_test/avg_vtxXY/complete_file/merged_result";
    string file_list_name = "file_list.txt";
    string output_directory = input_folder;
    pair<double,double> true_vtx = {-0.4, 2.4}; // note : unit mm


    // TChain *chain_in = new TChain("tree_geo_scan");
    vector<string> file_list = read_list(input_folder, file_list_name);
    // for (string file_name : file_list) { chain_in -> Add((file_name).c_str()); }
    // cout<<"chain_in -> GetEntries() : "<<chain_in -> GetEntries()<<endl;   

    vector<double> * offset_x_vec; offset_x_vec = 0;
    vector<double> * offset_y_vec; offset_y_vec = 0;
    vector<double> * offset_z_vec; offset_z_vec = 0;
    double total_offset_x = 0;
    double total_offset_y = 0;
    double total_offset_z = 0;

    double vtxX;
    double vtxY;
    int random_seed;

    // chain_in -> SetBranchAddress("offset_x_vec", &offset_x_vec);
    // chain_in -> SetBranchAddress("offset_y_vec", &offset_y_vec);
    // chain_in -> SetBranchAddress("offset_z_vec", &offset_z_vec);
    // chain_in -> SetBranchAddress("total_offset_x", &total_offset_x);
    // chain_in -> SetBranchAddress("total_offset_y", &total_offset_y);
    // chain_in -> SetBranchAddress("total_offset_z", &total_offset_z);
    // chain_in -> SetBranchAddress("vtxX", &vtxX);
    // chain_in -> SetBranchAddress("vtxY", &vtxY);
    // chain_in -> SetBranchAddress("random_seed", &random_seed);    

    SetsPhenixStyle();
   
    TLegend * legend = new TLegend(0.2,0.75,0.4,0.9);
    // legend -> SetMargin(0);
    legend->SetTextSize(0.03);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.025);

    double ladder_offset_l = -0.6; // note : single ladder offset, for each
    double ladder_offset_r = 0.6;
    double single_axis_offset_l = -0.5; // note : total offset of 56 ladder for single axis 
    double single_axis_offset_r = 11.5;
    double Axes2_offset_l = -0.5;  // note : total deviation of 56 ladders, two axes were included
    double Axes2_offset_r = 20.;  

    double angle_diff_l = -0.0;
    double angle_diff_r = 0.15 ;
    double vtx_deviation_l = -0.1;
    double vtx_deviation_r = 2;
    double angle_diff_fitE_l = -0.005;
    double angle_diff_fitE_r = 0.025;

    double window_size = 1.5;

    TH2F * ladder_offset_XY_2D = new TH2F("ladder_offset_XY_2D", "ladder_offset_XY_2D;Ladder X offset [mm];Ladder Y offset [mm]", 100, ladder_offset_l, ladder_offset_r, 100, ladder_offset_l, ladder_offset_r);
    TH2F * total_offset_XY_2D = new TH2F("total_offset_XY_2D", "total_offset_XY_2D;Total X offset [mm];Total Y offset [mm]", 100, single_axis_offset_l, single_axis_offset_r, 100, single_axis_offset_l, single_axis_offset_r);
    TH1F * ladder_offset_Z_1D = new TH1F("ladder_offset_Z_1D", "ladder_offset_Z_1D;Total Z offset [mm];Entry", 100, ladder_offset_l, ladder_offset_r);
    TH2F * total_offset_VtxDeviation_2D = new TH2F("total_offset_VtxDeviation_2D","total_offset_VtxDeviation_2D;Total XY offset [mm];VTX deviation [mm]", 100, Axes2_offset_l, Axes2_offset_r, 100, vtx_deviation_l, vtx_deviation_r);
    
    TH2F * vtxXY_distribution_2D = new TH2F("vtxXY_distribution_2D","vtxXY_distribution_2D;Vtx X [mm];Vtx Y [mm]", 100, true_vtx.first-window_size, true_vtx.first+window_size, 100, true_vtx.second-window_size, true_vtx.second+window_size);

    TH1F * reco_vtxX_diff_1D = new TH1F("reco_vtxX_diff_1D", "reco_vtxX_diff_1D;Reco X - True X [mm];Entry",100, -window_size, window_size);
    reco_vtxX_diff_1D -> SetLineColor(4);
    TH1F * reco_vtxY_diff_1D = new TH1F("reco_vtxY_diff_1D", "reco_vtxY_diff_1D;Reco Y - True Y [mm];Entry",100, -window_size, window_size);
    reco_vtxY_diff_1D -> SetLineColor(4);


    TCanvas * c1 = new TCanvas("c1","c1",950,800);

    for (int i = 0; i < file_list.size() /*file_list.size()*/; i++)
    {
        TFile * file_in = TFile::Open(file_list[i].c_str());
        TTree * tree_in = (TTree*) file_in -> Get("tree_geo_scan");
        cout<<i<<" running file : "<<file_list[i]<<endl;

        tree_in -> SetBranchAddress("offset_x_vec", &offset_x_vec);
        tree_in -> SetBranchAddress("offset_y_vec", &offset_y_vec);
        tree_in -> SetBranchAddress("offset_z_vec", &offset_z_vec);
        tree_in -> SetBranchAddress("total_offset_x", &total_offset_x);
        tree_in -> SetBranchAddress("total_offset_y", &total_offset_y);
        tree_in -> SetBranchAddress("total_offset_z", &total_offset_z);
        tree_in -> SetBranchAddress("vtxX", &vtxX);
        tree_in -> SetBranchAddress("vtxY", &vtxY);
        tree_in -> SetBranchAddress("random_seed", &random_seed);  

        tree_in -> GetEntry(0);

        double unit_correction = 10.;
        vtxX = vtxX * unit_correction;
        vtxY = vtxY * unit_correction;

        double shift_test_X = 0;
        double shift_test_Y = 0;
        double shift_test_Z = 0;

        // chain_in -> GetEntry(i);
        if (i % 1000 == 0) cout<<"trial running i : "<<i<<endl;

        for (int ladder_i = 0; ladder_i < offset_x_vec -> size(); ladder_i++)
        {
            ladder_offset_XY_2D -> Fill(offset_x_vec -> at(ladder_i), offset_y_vec -> at(ladder_i));
            ladder_offset_Z_1D -> Fill(offset_z_vec -> at(ladder_i));

            // if ( fabs(offset_x_vec -> at(ladder_i)) > 0.25 || fabs(offset_y_vec -> at(ladder_i)) > 0.25 || fabs(offset_z_vec -> at(ladder_i)) > 0.25 ) 
            // {
            //     cout<<"offset_x_vec -> at(ladder_i) : "<<offset_x_vec -> at(ladder_i)<<" offset_y_vec -> at(ladder_i) : "<<offset_y_vec -> at(ladder_i)<<" offset_z_vec -> at(ladder_i) : "<<offset_z_vec -> at(ladder_i)<<endl;
            // }

            shift_test_X += offset_x_vec -> at(ladder_i);
            shift_test_Y += offset_y_vec -> at(ladder_i);
            shift_test_Z += offset_z_vec -> at(ladder_i);
        }

        // cout<<"shift_test_X : "<<shift_test_X<<" shift_test_Y : "<<shift_test_Y<<" shift_test_Z : "<<shift_test_Z<<endl;

        double total_vtx_deviation = sqrt(pow(vtxX - true_vtx.first,2) + pow(vtxY - true_vtx.second,2));

        total_offset_XY_2D            -> Fill(total_offset_x, total_offset_y);
        total_offset_VtxDeviation_2D  -> Fill(total_offset_x+total_offset_y, total_vtx_deviation);

        vtxXY_distribution_2D   -> Fill(vtxX, vtxY);
        reco_vtxX_diff_1D       -> Fill(vtxX - true_vtx.first);
        reco_vtxY_diff_1D       -> Fill(vtxY - true_vtx.second);

        file_in -> Close();

        shift_test_X = 0;
        shift_test_Y = 0;
        shift_test_Z = 0;
    }

    c1 -> cd();
    ladder_offset_Z_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    draw_text -> DrawLatex(0.2, 0.67, Form("Mean: %.4f", ladder_offset_Z_1D -> GetMean()));
    draw_text -> DrawLatex(0.2, 0.63, Form("StdDev: %.4f", ladder_offset_Z_1D -> GetStdDev()));
    c1 -> Print((output_directory + "/ladder_offset_Z_1D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    total_offset_XY_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/total_offset_XY_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    total_offset_VtxDeviation_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/total_offset_VtxDeviation_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    vtxXY_distribution_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/vtxXY_distribution_2D.pdf").c_str());
    c1 -> Clear();


    c1 -> cd();
    ladder_offset_XY_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/ladder_offset_XY_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    reco_vtxX_diff_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    draw_text -> DrawLatex(0.2, 0.67, Form("Mean: %.4f", reco_vtxX_diff_1D -> GetMean()));
    draw_text -> DrawLatex(0.2, 0.63, Form("StdDev: %.4f", reco_vtxX_diff_1D -> GetStdDev()));
    c1 -> Print((output_directory + "/reco_vtxX_diff.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    reco_vtxY_diff_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    draw_text -> DrawLatex(0.2, 0.67, Form("Mean: %.4f", reco_vtxY_diff_1D -> GetMean()));
    draw_text -> DrawLatex(0.2, 0.63, Form("StdDev: %.4f", reco_vtxY_diff_1D -> GetStdDev()));
    c1 -> Print((output_directory + "/reco_vtxY_diff.pdf").c_str());
    c1 -> Clear();


    return 0;
}
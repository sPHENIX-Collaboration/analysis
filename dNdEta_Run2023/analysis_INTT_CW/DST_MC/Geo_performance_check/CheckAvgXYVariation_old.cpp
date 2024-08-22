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

map<int,int> hist_location(vector<string> file_list)
{
    int random_seed;
    map<int,int> hist_location_map;

    for (int i = 0; i < file_list.size(); i++)
    {
        cout<<file_list[i]<<endl;
        TFile * _file0 = TFile::Open(Form("%s",file_list[i].c_str()));
        TTree * _tree0 = (TTree *) _file0 -> Get("tree_geo_scan");
        _tree0 -> SetBranchStatus("*",0);
        _tree0 -> SetBranchStatus("random_seed",1);
        _tree0 -> SetBranchAddress("random_seed",&random_seed);

        for (int j = 0; j < _tree0 -> GetEntries(); j++)
        {
            _tree0 -> GetEntry(j);
            hist_location_map[random_seed] = i;
        }

        _file0 -> Close();
    }

    return hist_location_map;
}

vector<double> Get_coveriance_TH2(TH2F * hist_in)
{
    double X_mean = hist_in -> GetMean(1);
    double Y_mean = hist_in -> GetMean(2);

    double denominator = 0;
    double variance_x  = 0; 
    double variance_y  = 0;
    double covariance = 0;

    for (int xi = 0; xi < hist_in -> GetNbinsX(); xi++){
        for (int yi = 0; yi < hist_in -> GetNbinsY(); yi++){
            double cell_weight = hist_in -> GetBinContent(xi+1, yi+1);
            double cell_x = hist_in -> GetXaxis() -> GetBinCenter(xi+1);
            double cell_y = hist_in -> GetYaxis() -> GetBinCenter(yi+1);

            denominator += pow(cell_weight, 2);
            variance_x  += pow(cell_x - X_mean, 2) * pow(cell_weight,2);
            variance_y  += pow(cell_y - Y_mean, 2) * pow(cell_weight,2);
            covariance  += (cell_x - X_mean) * (cell_y - Y_mean) * pow(cell_weight,2);
        }
    }

    return {variance_x/denominator, variance_y/denominator, covariance/denominator};
}

int main()
{
    string input_folder = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/Geo_XY_scan_trial_2/complete_file/full_merged_file";
    string file_list_name = "file_list.txt";
    string output_directory = input_folder + "/" + "merged_result";
    pair<double,double> true_vtx = {-0.4, 2.4}; // note : unit mm
    double data_stddev_x = 0.635807;
    double data_stddev_x_range = 0.0152603 * 1;
    double data_stddev_y = 0.67362;
    double data_stddev_y_range = 0.0119012 * 1;

    double data_variance_x = 0.381985;
    double data_variance_x_range = 0.0178251 * 1;
    double data_variance_y = 0.430956;
    double data_variance_y_range = 0.0148532 * 1;
    
    double data_covariance = 0.0814871;
    double data_covariance_range = 0.00666554 * 1;


    TChain *chain_in = new TChain("tree_geo_scan");
    vector<string> file_list = read_list(input_folder, file_list_name);
    for (string file_name : file_list) { chain_in -> Add((file_name).c_str()); }
    cout<<"chain_in -> GetEntries() : "<<chain_in -> GetEntries()<<endl;   

    vector<double> * offset_x_vec; offset_x_vec = 0;
    vector<double> * offset_y_vec; offset_y_vec = 0;
    double total_offset_x = 0;
    double total_offset_y = 0;

    double DCA_inner_fitYpos;
    double angle_diff_inner_fitYpos;
    double DCA_inner_fitE;
    double angle_diff_inner_fitE;
    double DCA_outer_fitYpos;
    double angle_diff_outer_fitYpos;
    double DCA_outer_fitE;
    double angle_diff_outer_fitE;
    int    random_seed_out;
    double vtxX;
    double vtxY;
    double trial_originX;
    double trial_originY;
    double angle_diff_mean;
    double angle_diff_stddev;
    double DCA_distance_mean;
    double DCA_distance_stddev;
    double Line_filled_X;
    double Line_filled_Y;
    double Line_filled_stddev_X;
    double Line_filled_stddev_Y;

    chain_in -> SetBranchAddress("offset_x_vec", &offset_x_vec);
    chain_in -> SetBranchAddress("offset_y_vec", &offset_y_vec);
    chain_in -> SetBranchAddress("total_offset_x", &total_offset_x);
    chain_in -> SetBranchAddress("total_offset_y", &total_offset_y);
    chain_in -> SetBranchAddress("DCA_inner_fitYpos", &DCA_inner_fitYpos);
    chain_in -> SetBranchAddress("DCA_inner_fitE", &DCA_inner_fitE);
    chain_in -> SetBranchAddress("angle_diff_inner_fitYpos", &angle_diff_inner_fitYpos);
    chain_in -> SetBranchAddress("angle_diff_inner_fitE", &angle_diff_inner_fitE);
    chain_in -> SetBranchAddress("DCA_outer_fitYpos", &DCA_outer_fitYpos);
    chain_in -> SetBranchAddress("DCA_outer_fitE", &DCA_outer_fitE);
    chain_in -> SetBranchAddress("angle_diff_outer_fitYpos", &angle_diff_outer_fitYpos);
    chain_in -> SetBranchAddress("angle_diff_outer_fitE", &angle_diff_outer_fitE);
    chain_in -> SetBranchAddress("random_seed", &random_seed_out);
    chain_in -> SetBranchAddress("vtxX", &vtxX);
    chain_in -> SetBranchAddress("vtxY", &vtxY);
    chain_in -> SetBranchAddress("trial_originX", &trial_originX);
    chain_in -> SetBranchAddress("trial_originY", &trial_originY);
    chain_in -> SetBranchAddress("angle_diff_mean",     &angle_diff_mean);
    chain_in -> SetBranchAddress("angle_diff_stddev",   &angle_diff_stddev);
    chain_in -> SetBranchAddress("DCA_distance_mean",   &DCA_distance_mean);
    chain_in -> SetBranchAddress("DCA_distance_stddev", &DCA_distance_stddev);
    chain_in -> SetBranchAddress("Line_filled_X", &Line_filled_X);
    chain_in -> SetBranchAddress("Line_filled_Y", &Line_filled_Y);
    chain_in -> SetBranchAddress("Line_filled_stddev_X", &Line_filled_stddev_X);
    chain_in -> SetBranchAddress("Line_filled_stddev_Y", &Line_filled_stddev_Y);

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

    double ladder_offset_l = -0.4; // note : single ladder offset, for each
    double ladder_offset_r = 0.4;
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
    TH2F * total_offset_VtxDeviation_2D = new TH2F("total_offset_VtxDeviation_2D","total_offset_VtxDeviation_2D;Total XY offset [mm];VTX deviation [mm]", 100, Axes2_offset_l, Axes2_offset_r, 100, vtx_deviation_l, vtx_deviation_r);
    
    TH2F * vtxXY_distribution_quadrant_2D = new TH2F("vtxXY_distribution_quadrant_2D","vtxXY_distribution_quadrant_2D;Vtx X [mm];Vtx Y [mm]", 100, true_vtx.first-window_size, true_vtx.first+window_size, 100, true_vtx.second-window_size, true_vtx.second+window_size);
    TH2F * vtxXY_distribution_linefill_2D = new TH2F("vtxXY_distribution_linefill_2D","vtxXY_distribution_linefill_2D;Vtx X [mm];Vtx Y [mm]", 100, true_vtx.first-window_size, true_vtx.first+window_size, 100, true_vtx.second-window_size, true_vtx.second+window_size);
    TH2F * angle_diff_stddev_deviation_2D = new TH2F("angle_diff_stddev_deviation_2D", "angle_diff_stddev_deviation_2D;#Delta#phi StdDev [deg];VTX deviation [mm]", 100, angle_diff_l, angle_diff_r,100, vtx_deviation_l, vtx_deviation_r);
    
    TH2F * total_offset_angle_diff_stddev_2D = new TH2F("total_offset_angle_diff_stddev_2D", "total_offset_angle_diff_stddev_2D;Total XY offset [mm];#Delta#phi StdDev [deg]", 100, Axes2_offset_l, Axes2_offset_r, 100, angle_diff_l, angle_diff_r);
    TH2F * total_offset_angle_diff_inner_fitE_2D = new TH2F("total_offset_angle_diff_inner_fitE_2D", "total_offset_angle_diff_inner_fitE_2D;Total XY offset [mm];#Delta#phi Inner FitE [deg]", 100, Axes2_offset_l, Axes2_offset_r, 100, angle_diff_fitE_l, angle_diff_fitE_r);
   
    TH2F * reco_vtxX_correction_2D = new TH2F("reco_vtxX_correction_2D", "reco_vtxX_correction_2D;Quadrant X [mm];Line_filled X [mm]", 100, true_vtx.first-window_size, true_vtx.first+window_size, 100, true_vtx.first-window_size, true_vtx.first+window_size);
    TH2F * reco_vtxY_correction_2D = new TH2F("reco_vtxY_correction_2D", "reco_vtxY_correction_2D;Quadrant Y [mm];Line_filled Y [mm]", 100, true_vtx.second-window_size, true_vtx.second+window_size, 100, true_vtx.second-window_size, true_vtx.second+window_size);
    TH1F * reco_vtxX_diff_quadrant_1D = new TH1F("reco_vtxX_diff_quadrant_1D", "reco_vtxX_diff_quadrant_1D;Reco X - True X [mm];Entry",100, -window_size, window_size);
    reco_vtxX_diff_quadrant_1D -> SetLineColor(4);
    TH1F * reco_vtxY_diff_quadrant_1D = new TH1F("reco_vtxY_diff_quadrant_1D", "reco_vtxY_diff_quadrant_1D;Reco Y - True Y [mm];Entry",100, -window_size, window_size);
    reco_vtxY_diff_quadrant_1D -> SetLineColor(4);
    TH1F * reco_vtxX_diff_linefill_1D = new TH1F("reco_vtxX_diff_linefill_1D", "reco_vtxX_diff_linefill_1D;Reco X - True X [mm];Entry",100, -window_size, window_size);
    reco_vtxX_diff_linefill_1D -> SetLineColor(2);
    TH1F * reco_vtxY_diff_linefill_1D = new TH1F("reco_vtxY_diff_linefill_1D", "reco_vtxY_diff_linefill_1D;Reco Y - True Y [mm];Entry",100, -window_size, window_size);
    reco_vtxY_diff_linefill_1D -> SetLineColor(2);

    TFile * _file0;
    map<int,int> hist_location_map = hist_location(file_list);

    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1","c1",950,800);
    c1 -> cd();

    c1 -> Print(Form("%s/Line_filled_bkgrm_2D_good_trail.pdf(", output_directory.c_str()));

    for (int i = 0; i < chain_in -> GetEntries(); i++)
    {
        chain_in -> GetEntry(i);
        if (i % 1000 == 0) cout<<"trial running i : "<<i<<endl;

        for (int ladder_i = 0; ladder_i < offset_x_vec -> size(); ladder_i++)
        {
            ladder_offset_XY_2D -> Fill(offset_x_vec -> at(ladder_i), offset_y_vec -> at(ladder_i));
        }

        double quadrant_vtxX = (vtxX + trial_originX) / 2;
        double quadrant_vtxY = (vtxY + trial_originY) / 2;
        double total_vtx_deviation_quadrant = sqrt(pow(quadrant_vtxX - true_vtx.first,2) + pow(quadrant_vtxY - true_vtx.second,2));

        total_offset_XY_2D                    -> Fill(total_offset_x, total_offset_y);
        total_offset_VtxDeviation_2D          -> Fill(total_offset_x+total_offset_y, total_vtx_deviation_quadrant);
        vtxXY_distribution_quadrant_2D        -> Fill(quadrant_vtxX, quadrant_vtxY);
        vtxXY_distribution_linefill_2D        -> Fill(Line_filled_X, Line_filled_Y);
        angle_diff_stddev_deviation_2D        -> Fill(angle_diff_stddev, total_vtx_deviation_quadrant);
        total_offset_angle_diff_stddev_2D     -> Fill(total_offset_x+total_offset_y, angle_diff_stddev);
        total_offset_angle_diff_inner_fitE_2D -> Fill(total_offset_x+total_offset_y, angle_diff_inner_fitE);

        reco_vtxX_correction_2D     -> Fill(quadrant_vtxX, Line_filled_X); 
        reco_vtxY_correction_2D     -> Fill(quadrant_vtxY, Line_filled_Y); 
        reco_vtxX_diff_quadrant_1D  -> Fill(quadrant_vtxX - true_vtx.first);
        reco_vtxY_diff_quadrant_1D  -> Fill(quadrant_vtxY - true_vtx.second);
        reco_vtxX_diff_linefill_1D  -> Fill(Line_filled_X - true_vtx.first);
        reco_vtxY_diff_linefill_1D  -> Fill(Line_filled_Y - true_vtx.second);

        if ((data_stddev_x - data_stddev_x_range) < Line_filled_stddev_X && Line_filled_stddev_X < (data_stddev_x + data_stddev_x_range) && (data_stddev_y - data_stddev_y_range) < Line_filled_stddev_Y && Line_filled_stddev_Y < (data_stddev_y + data_stddev_y_range))
        {
            // cout<<"random_seed_out : "<<random_seed_out<<endl;
            _file0 = TFile::Open(Form("%s", file_list[hist_location_map[random_seed_out]].c_str()));
            TH2F * h2 = (TH2F *) _file0->Get(Form("Line_filled_bkg_remove_%i",random_seed_out));

            vector<double> covariance_vec = Get_coveriance_TH2(h2);
            if ((data_variance_x - data_variance_x_range) < covariance_vec[0] && covariance_vec[0] < (data_variance_x + data_variance_x_range) && (data_variance_y - data_variance_y_range) < covariance_vec[1] && covariance_vec[1] < (data_variance_y + data_variance_y_range))
            {
                if ((data_covariance - data_covariance_range) < covariance_vec[2] && covariance_vec[2] < (data_covariance + data_covariance_range))
                {
                    h2 -> Draw("colz0");
                    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
                    draw_text -> DrawLatex(0.2, 0.84, Form("Random Seed: %i", random_seed_out));
                    draw_text -> DrawLatex(0.2, 0.80, Form("StdDev XY : %.3f, %.3f", Line_filled_stddev_X, Line_filled_stddev_Y));
                    draw_text -> DrawLatex(0.2, 0.76, Form("Variance XY : %.3f, %.3f", covariance_vec[0], covariance_vec[1]));
                    draw_text -> DrawLatex(0.2, 0.72, Form("Covariance : %.3f", covariance_vec[2]));
                    c1 -> Print(Form("%s/Line_filled_bkgrm_2D_good_trail.pdf", output_directory.c_str()));
                    c1 -> Clear();
                }
            }

            _file0 -> Close();
        }
    }

    c1 -> Print(Form("%s/Line_filled_bkgrm_2D_good_trail.pdf)", output_directory.c_str()));

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
    vtxXY_distribution_quadrant_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/vtxXY_distribution_quadrant_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    vtxXY_distribution_linefill_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/vtxXY_distribution_linefill_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    angle_diff_stddev_deviation_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/angle_diff_stddev_deviation_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    total_offset_angle_diff_stddev_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/total_offset_angle_diff_stddev_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    total_offset_angle_diff_inner_fitE_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/total_offset_angle_diff_inner_fitE_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    ladder_offset_XY_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/ladder_offset_XY_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    reco_vtxX_correction_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/reco_vtxX_correction_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    reco_vtxY_correction_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    c1 -> Print((output_directory + "/reco_vtxY_correction_2D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    reco_vtxX_diff_quadrant_1D -> Draw("hist");
    reco_vtxX_diff_linefill_1D -> Draw("hist same");
    legend -> AddEntry(reco_vtxX_diff_quadrant_1D, "Quadrant", "f");
    legend -> AddEntry(reco_vtxX_diff_linefill_1D, "Line-filled", "f");
    legend -> Draw("same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    draw_text -> DrawLatex(0.2, 0.67, Form("Quadrant Mean: %.4f", reco_vtxX_diff_quadrant_1D -> GetMean()));
    draw_text -> DrawLatex(0.2, 0.63, Form("Quadrant StdDev: %.4f", reco_vtxX_diff_quadrant_1D -> GetStdDev()));
    draw_text -> DrawLatex(0.2, 0.59, Form("LineFill Mean: %.4f", reco_vtxX_diff_linefill_1D -> GetMean()));
    draw_text -> DrawLatex(0.2, 0.55, Form("LineFill StdDev: %.4f", reco_vtxX_diff_linefill_1D -> GetStdDev()));
    c1 -> Print((output_directory + "/reco_vtxX_diff.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    reco_vtxY_diff_quadrant_1D -> Draw("hist");
    reco_vtxY_diff_linefill_1D -> Draw("hist same");
    legend -> Draw("same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));
    draw_text -> DrawLatex(0.2, 0.67, Form("Quadrant Mean: %.4f", reco_vtxY_diff_quadrant_1D -> GetMean()));
    draw_text -> DrawLatex(0.2, 0.63, Form("Quadrant StdDev: %.4f", reco_vtxY_diff_quadrant_1D -> GetStdDev()));
    draw_text -> DrawLatex(0.2, 0.59, Form("LineFill Mean: %.4f", reco_vtxY_diff_linefill_1D -> GetMean()));
    draw_text -> DrawLatex(0.2, 0.55, Form("LineFill StdDev: %.4f", reco_vtxY_diff_linefill_1D -> GetStdDev()));
    c1 -> Print((output_directory + "/reco_vtxY_diff.pdf").c_str());
    c1 -> Clear();


    return 0;
}
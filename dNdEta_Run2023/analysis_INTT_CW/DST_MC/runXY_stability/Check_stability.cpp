#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <numeric>
using namespace std;

#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TLatex.h>
#include <TLegend.h>

#include "sPhenixStyle.h"

double  vector_average (vector <double> input_vector) {
	return accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());
}

double vector_stddev (vector <double> input_vector){
	
	double sum_subt = 0;
	double average  = accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());
	
	// cout<<"average is : "<<average<<endl;

	for (int i=0; i<input_vector.size(); i++){ sum_subt += pow((input_vector[i] - average),2); }

	//cout<<"sum_subt : "<<sum_subt<<endl;
	// cout<<"print from the function, average : "<<average<<" std : "<<stddev<<endl;

	return sqrt( sum_subt / double(input_vector.size()-1) );
}

vector<string> read_list(string folder_direction, string MC_list_name)
{
    vector<string> file_list;
    string list_buffer;
    ifstream data_list;
	data_list.open((folder_direction + "/" + MC_list_name).c_str());

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
	cout<<"size in the" <<MC_list_name<<": "<<file_list.size()<<endl;

    return file_list;
}

int main(int argc, char* argv[])
{
    int data_type = std::stod(argv[1]); // note : 1 = MC, 0 = data
    int unit_tag = 1; // note : 1 = cm, 0 = mm
    double unit_correction = (unit_tag == 1) ? 0.1 : 1.;
    double frame_shift_forX = (data_type == 1) ? 0. : 0.3;
    double frame_shift_forY = (data_type == 1) ? 0. : 0.6;
    
    // todo : modify the followings if the vertex is expected to be in different region 
    // note : still using mm, here
    double X_range_l = (-1. + frame_shift_forX) * unit_correction;
    double X_range_r = (0.5 + frame_shift_forX) * unit_correction;
    double Y_range_l = (1.7 + frame_shift_forY) * unit_correction;
    double Y_range_r = (3.2 + frame_shift_forY) * unit_correction;
    
    string input_folder = argv[2];
    // string input_folder = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR/avg_vtxXY";
    // string input_folder = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/avg_vtxXY";
    string file_list_name = "file_list.txt";
    string output_directory = input_folder + "/" + "merged_result";
    


    string label_text = (data_type == 1)? "Simulation" : "Work-in-progress";
    string unit_text = (unit_tag == 1) ? "cm" : "mm";

    // note : to generate the merged_result folder
    if(std::filesystem::exists(Form("%s",output_directory.c_str())) == false){
        system(Form("mkdir %s", output_directory.c_str()));
        cout<<"----------- check folder exists -----------"<<endl;
        system(Form("ls %s", output_directory.c_str()));
    }
    else 
    {
        cout<<"----------- folder exists already -----------"<<endl;
        system(Form("ls %s", output_directory.c_str()));
    }

    // note : to generate the file_list.txt
    if(std::filesystem::exists(Form("%s/%s",input_folder.c_str(),file_list_name.c_str())) == false){
        system(Form("ls %s/runXY_*/run_XY_tree.root > %s/%s", input_folder.c_str(), input_folder.c_str(), file_list_name.c_str()));
        cout<<"----------- file list generated -----------"<<endl;
        system(Form("cat %s/%s", input_folder.c_str(), file_list_name.c_str()));
        cout<<"----------- file list generated -----------"<<endl;
    }
    else 
    {
        cout<<"----------- found the file list -----------"<<endl;
        system(Form("cat %s/%s", input_folder.c_str(), file_list_name.c_str()));
        cout<<"----------- found the file list -----------"<<endl;
    }
    
    TChain *chain_in = new TChain("tree");
    vector<string> file_list = read_list(input_folder, file_list_name);
    for (string file_name : file_list) { chain_in -> Add((file_name).c_str()); }
    cout<<"chain_in -> GetEntries() : "<<chain_in -> GetEntries()<<endl;

    long in_start_evt;
    long in_end_evt;
    double in_quadrant_corner_X;
    double in_quadrant_corner_Y;
    double in_quadrant_center_X;
    double in_quadrant_center_Y;
    double in_line_filled_mean_X;
    double in_line_filled_mean_Y;
    double in_line_filled_meanE_X;
    double in_line_filled_meanE_Y;
    double in_line_filled_stddev_X;
    double in_line_filled_stddev_Y;
    double in_line_filled_variance_X;
    double in_line_filled_variance_Y;
    double in_line_filled_covariance;

    chain_in -> SetBranchAddress("start_evt",&in_start_evt);
    chain_in -> SetBranchAddress("end_evt",&in_end_evt);
    chain_in -> SetBranchAddress("quadrant_corner_X",&in_quadrant_corner_X);
    chain_in -> SetBranchAddress("quadrant_corner_Y",&in_quadrant_corner_Y);
    chain_in -> SetBranchAddress("quadrant_center_X",&in_quadrant_center_X);
    chain_in -> SetBranchAddress("quadrant_center_Y",&in_quadrant_center_Y);
    chain_in -> SetBranchAddress("line_filled_mean_X",&in_line_filled_mean_X);
    chain_in -> SetBranchAddress("line_filled_mean_Y",&in_line_filled_mean_Y);
    chain_in -> SetBranchAddress("line_filled_meanE_X",&in_line_filled_meanE_X);
    chain_in -> SetBranchAddress("line_filled_meanE_Y",&in_line_filled_meanE_Y);
    chain_in -> SetBranchAddress("line_filled_stddev_X",&in_line_filled_stddev_X);
    chain_in -> SetBranchAddress("line_filled_stddev_Y",&in_line_filled_stddev_Y);
    chain_in -> SetBranchAddress("line_filled_variance_X",&in_line_filled_variance_X);
    chain_in -> SetBranchAddress("line_filled_variance_Y",&in_line_filled_variance_Y);
    chain_in -> SetBranchAddress("line_filled_covariance",&in_line_filled_covariance);

    TH1F * quadrant_VtxX_1D = new TH1F("quadrant_VtxX",Form("quadrant_VtxX;X axis [%s];Entry",unit_text.c_str()),100,X_range_l,X_range_r);
    TH1F * quadrant_VtxY_1D = new TH1F("quadrant_VtxY",Form("quadrant_VtxY;Y axis [%s];Entry",unit_text.c_str()),100,Y_range_l,Y_range_r);
    TH1F * line_filled_mean_X_1D = new TH1F("line_filled_mean_X",Form("line_filled_mean_X;X axis [%s];Entry",unit_text.c_str()),100,X_range_l,X_range_r);
    TH1F * line_filled_mean_Y_1D = new TH1F("line_filled_mean_Y",Form("line_filled_mean_Y;Y axis [%s];Entry",unit_text.c_str()),100,Y_range_l,Y_range_r);
    
    TH1F * line_filled_stddev_X_1D = new TH1F("line_filled_stddev_X_1D",Form("line_filled_stddev_X_1D;StdDev, X axis [%s];Entry",unit_text.c_str()),100,0,1 * unit_correction);
    TH1F * line_filled_stddev_Y_1D = new TH1F("line_filled_stddev_Y_1D",Form("line_filled_stddev_Y_1D;StdDev, Y axis [%s];Entry",unit_text.c_str()),100,0,1 * unit_correction);
    
    TH1F * line_filled_variance_X_1D = new TH1F("line_filled_variance_X_1D",Form("line_filled_variance_X_1D;Variance, X axis [%s];Entry",unit_text.c_str()),100,0,1 * unit_correction);    
    TH1F * line_filled_variance_Y_1D = new TH1F("line_filled_variance_Y_1D",Form("line_filled_variance_Y_1D;Variance, Y axis [%s];Entry",unit_text.c_str()),100,0,1 * unit_correction);
    
    TH1F * line_filled_covariance_1D = new TH1F("line_filled_covariance_1D",Form("line_filled_covariance_1D;Covariance [%s];Entry",unit_text.c_str()),100,-0.06 * unit_correction,0.06 * unit_correction);


    vector<double> evt_index_vec; evt_index_vec.clear();
    vector<double> evt_index_range_vec; evt_index_range_vec.clear();

    vector<double> quadrant_VtxX_vec; quadrant_VtxX_vec.clear();
    vector<double> quadrant_VtxY_vec; quadrant_VtxY_vec.clear();
    vector<double> quadrant_VtxXerror_vec; quadrant_VtxXerror_vec.clear();
    vector<double> quadrant_VtxYerror_vec; quadrant_VtxYerror_vec.clear();

    vector<double> line_filled_mean_X_vec; line_filled_mean_X_vec.clear();
    vector<double> line_filled_mean_Y_vec; line_filled_mean_Y_vec.clear();
    vector<double> line_filled_mean_Xerror_vec; line_filled_mean_Xerror_vec.clear();
    vector<double> line_filled_mean_Yerror_vec; line_filled_mean_Yerror_vec.clear();

    vector<double> combined_index_vec; combined_index_vec.clear();
    vector<double> combined_index_range_vec; combined_index_range_vec.clear();
    vector<double> combined_X_vec; combined_X_vec.clear();
    vector<double> combined_Y_vec; combined_Y_vec.clear();
    vector<double> combined_XE_vec; combined_XE_vec.clear();
    vector<double> combined_YE_vec; combined_YE_vec.clear();


    vector<double> zero_vec; zero_vec.clear();
    vector<double> starteID_vec; starteID_vec.clear();
    vector<double> endeID_vec; endeID_vec.clear();

    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1","c1",950,800);
    c1 -> cd();

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    for (int i = 0; i < chain_in -> GetEntries(); i++)
    {
        chain_in -> GetEntry(i);

        // note : if changing from mm to cm is needed
        in_quadrant_corner_X = in_quadrant_corner_X * unit_correction;
        in_quadrant_corner_Y = in_quadrant_corner_Y * unit_correction;
        in_quadrant_center_X = in_quadrant_center_X * unit_correction;
        in_quadrant_center_Y = in_quadrant_center_Y * unit_correction;
        in_line_filled_mean_X = in_line_filled_mean_X * unit_correction;
        in_line_filled_mean_Y = in_line_filled_mean_Y * unit_correction;
        in_line_filled_meanE_X = in_line_filled_meanE_X * unit_correction;
        in_line_filled_meanE_Y = in_line_filled_meanE_Y * unit_correction;
        in_line_filled_stddev_X = in_line_filled_stddev_X * unit_correction;
        in_line_filled_stddev_Y = in_line_filled_stddev_Y * unit_correction;
        in_line_filled_variance_X = in_line_filled_variance_X * unit_correction;
        in_line_filled_variance_Y = in_line_filled_variance_Y * unit_correction;
        in_line_filled_covariance = in_line_filled_covariance * unit_correction;

        double quadrant_VtxX = (in_quadrant_corner_X + in_quadrant_center_X)/2.;
        double quadrant_VtxY = (in_quadrant_corner_Y + in_quadrant_center_Y)/2.;
        double quadrant_WindowX = (fabs(in_quadrant_corner_X - in_quadrant_center_X))/2.;
        double quadrant_WindowY = (fabs(in_quadrant_corner_Y - in_quadrant_center_Y))/2.;
        
        double line_filled_mean_X = in_line_filled_mean_X;
        double line_filled_mean_Y = in_line_filled_mean_Y;
        double line_filled_meanE_X = in_line_filled_meanE_X;
        double line_filled_meanE_Y = in_line_filled_meanE_Y;
        cout<<Form("unit : %s",unit_text.c_str())<<", index : "<<i<<" quadrant_VtxX : "<<quadrant_VtxX<<" quadrant_VtxY : "<<quadrant_VtxY<<" line_filled_mean_X : "<<line_filled_mean_X<<" line_filled_mean_Y : "<<line_filled_mean_Y<<endl;
        cout<<Form("unit : %s",unit_text.c_str())<<", index : "<<i<<" quadrant_VtxXE : "<<quadrant_WindowX<<" quadrant_VtxYE : "<<quadrant_WindowY<<" line_filled_meanE_X : "<<line_filled_meanE_X<<" line_filled_meaE_Y : "<<line_filled_meanE_Y<<endl;

        starteID_vec.push_back(in_start_evt);
        endeID_vec.push_back(in_end_evt);

        evt_index_vec.push_back((in_start_evt + in_end_evt)/2.);
        evt_index_range_vec.push_back((in_end_evt - in_start_evt)/2.);
        quadrant_VtxX_vec.push_back(quadrant_VtxX);
        quadrant_VtxY_vec.push_back(quadrant_VtxY);
        line_filled_mean_X_vec.push_back(line_filled_mean_X);
        line_filled_mean_Y_vec.push_back(line_filled_mean_Y);
        zero_vec.push_back(0);

        quadrant_VtxXerror_vec.push_back(quadrant_WindowX);
        quadrant_VtxYerror_vec.push_back(quadrant_WindowY);
        line_filled_mean_Xerror_vec.push_back(line_filled_meanE_X);
        line_filled_mean_Yerror_vec.push_back(line_filled_meanE_Y);

        combined_index_vec.push_back((in_start_evt + in_end_evt)/2.);
        combined_index_range_vec.push_back((in_end_evt - in_start_evt)/2.);
        combined_X_vec.push_back(quadrant_VtxX);
        combined_XE_vec.push_back(quadrant_WindowX);
        combined_Y_vec.push_back(quadrant_VtxY);
        combined_YE_vec.push_back(quadrant_WindowY);

        combined_index_vec.push_back((in_start_evt + in_end_evt)/2.);
        combined_index_range_vec.push_back((in_end_evt - in_start_evt)/2.);
        combined_X_vec.push_back(line_filled_mean_X);        
        combined_XE_vec.push_back(line_filled_meanE_X);
        combined_Y_vec.push_back(line_filled_mean_Y);
        combined_YE_vec.push_back(line_filled_meanE_Y);




        quadrant_VtxX_1D -> Fill(quadrant_VtxX);
        quadrant_VtxY_1D -> Fill(quadrant_VtxY);
        line_filled_mean_X_1D -> Fill(line_filled_mean_X);
        line_filled_mean_Y_1D -> Fill(line_filled_mean_Y);

        line_filled_stddev_X_1D -> Fill(in_line_filled_stddev_X);
        line_filled_stddev_Y_1D -> Fill(in_line_filled_stddev_Y);
        line_filled_variance_X_1D -> Fill(in_line_filled_variance_X);
        line_filled_variance_Y_1D -> Fill(in_line_filled_variance_Y);
        line_filled_covariance_1D -> Fill(in_line_filled_covariance);
    }

    for (int i = 0; i < starteID_vec.size(); i++)
    {
        cout<<"file : "<<i<<", start: "<<starteID_vec[i]<<", end: "<<endeID_vec[i]<<endl;
    }

    TGraphErrors * g_quadrant_VtxX_index = new TGraphErrors(chain_in -> GetEntries(), &evt_index_vec[0], &quadrant_VtxX_vec[0], &evt_index_range_vec[0], &quadrant_VtxXerror_vec[0]);
    g_quadrant_VtxX_index -> GetXaxis() -> SetNdivisions(505);
    g_quadrant_VtxX_index -> SetMarkerStyle(20);
    g_quadrant_VtxX_index -> SetMarkerSize(0.7);
    g_quadrant_VtxX_index -> SetMarkerColor(1);
    g_quadrant_VtxX_index -> GetXaxis() -> SetTitle("Event ID");
    g_quadrant_VtxX_index -> GetYaxis() -> SetTitle(Form("Average beam position in X axis [%s]",unit_text.c_str()));
    g_quadrant_VtxX_index -> GetXaxis() -> SetLimits(-1000, g_quadrant_VtxX_index -> GetPointX(g_quadrant_VtxX_index -> GetN() - 1) + 10000);
    g_quadrant_VtxX_index -> GetYaxis() -> SetRangeUser(quadrant_VtxX_1D -> GetXaxis() -> GetXmin(),quadrant_VtxX_1D -> GetXaxis() -> GetXmax());

    TGraphErrors * g_quadrant_VtxY_index = new TGraphErrors(chain_in -> GetEntries(), &evt_index_vec[0], &quadrant_VtxY_vec[0], &evt_index_range_vec[0], &quadrant_VtxYerror_vec[0]);
    g_quadrant_VtxY_index -> GetXaxis() -> SetNdivisions(505);
    g_quadrant_VtxY_index -> SetMarkerStyle(20);
    g_quadrant_VtxY_index -> SetMarkerSize(0.7);
    g_quadrant_VtxY_index -> SetMarkerColor(1);
    g_quadrant_VtxY_index -> GetXaxis() -> SetTitle("Event ID");
    g_quadrant_VtxY_index -> GetYaxis() -> SetTitle(Form("Average beam position in Y axis [%s]",unit_text.c_str()));
    g_quadrant_VtxY_index -> GetXaxis() -> SetLimits(-1000, g_quadrant_VtxY_index -> GetPointX(g_quadrant_VtxY_index -> GetN() - 1) + 10000);
    g_quadrant_VtxY_index -> GetYaxis() -> SetRangeUser(quadrant_VtxY_1D -> GetXaxis() -> GetXmin(),quadrant_VtxY_1D -> GetXaxis() -> GetXmax());

    TGraphErrors * g_line_filled_mean_X_index = new TGraphErrors(chain_in -> GetEntries(), &evt_index_vec[0], &line_filled_mean_X_vec[0], &evt_index_range_vec[0], &line_filled_mean_Xerror_vec[0]);
    g_line_filled_mean_X_index -> GetXaxis() -> SetNdivisions(505);
    g_line_filled_mean_X_index -> SetMarkerStyle(20);
    g_line_filled_mean_X_index -> SetMarkerSize(0.7);
    g_line_filled_mean_X_index -> SetMarkerColor(4);
    g_line_filled_mean_X_index -> SetLineColor(4);
    g_line_filled_mean_X_index -> GetXaxis() -> SetTitle("Event ID");
    g_line_filled_mean_X_index -> GetYaxis() -> SetTitle(Form("Beam Position in X axis [%s]",unit_text.c_str()));

    TGraphErrors * g_line_filled_mean_Y_index = new TGraphErrors(chain_in -> GetEntries(), &evt_index_vec[0], &line_filled_mean_Y_vec[0], &evt_index_range_vec[0], &line_filled_mean_Yerror_vec[0]);
    g_line_filled_mean_Y_index -> GetXaxis() -> SetNdivisions(505);
    g_line_filled_mean_Y_index -> SetMarkerStyle(20);
    g_line_filled_mean_Y_index -> SetMarkerSize(0.7);
    g_line_filled_mean_Y_index -> SetMarkerColor(4);
    g_line_filled_mean_Y_index -> SetLineColor(4);
    g_line_filled_mean_Y_index -> GetXaxis() -> SetTitle("Event ID");
    g_line_filled_mean_Y_index -> GetYaxis() -> SetTitle(Form("Beam Position in Y axis [%s]",unit_text.c_str()));

    
    TGraphErrors * combined_X_index_grE = new TGraphErrors(combined_index_vec.size(), &combined_index_vec[0], &combined_X_vec[0], &combined_index_range_vec[0], &combined_XE_vec[0]);
    TF1 * pol0_fit_X = new TF1("pol0_fit_X", "pol0", 0, g_line_filled_mean_Y_index->GetPointX(g_line_filled_mean_Y_index -> GetN() - 1) * 1.5);
    pol0_fit_X -> SetLineColor(2);
    pol0_fit_X -> SetLineStyle(9);
    pol0_fit_X -> SetLineWidth(1);
    combined_X_index_grE -> Fit(pol0_fit_X,"NQ");

    TGraphErrors * combined_Y_index_grE = new TGraphErrors(combined_index_vec.size(), &combined_index_vec[0], &combined_Y_vec[0], &combined_index_range_vec[0], &combined_YE_vec[0]);
    TF1 * pol0_fit_Y = new TF1("pol0_fit_Y", "pol0", 0, g_line_filled_mean_Y_index->GetPointX(g_line_filled_mean_Y_index -> GetN() - 1) * 1.5);
    pol0_fit_Y -> SetLineColor(2);
    pol0_fit_Y -> SetLineStyle(9);
    pol0_fit_Y -> SetLineWidth(1);
    combined_Y_index_grE -> Fit(pol0_fit_Y,"NQ");

    TLegend * leg = new TLegend(0.21,0.75,0.31,0.9);
    leg -> AddEntry(g_quadrant_VtxX_index, "Quadrant method", "lep");
    leg -> AddEntry(g_line_filled_mean_X_index, "Line-filled method", "lep");


    c1 -> cd();
    g_quadrant_VtxX_index -> Draw("AP");
    g_line_filled_mean_X_index -> Draw("p same");
    draw_text -> DrawLatex(0.21,0.71,Form("Numeric average vertex X: %.4f %s", ( vector_average(line_filled_mean_X_vec) + vector_average(quadrant_VtxX_vec) )/2., unit_text.c_str() ));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    leg -> Draw("same");
    c1 -> Print((output_directory + "/VtxX_index.pdf").c_str());
    c1 -> Clear();

    g_quadrant_VtxX_index -> Draw("AP");
    g_line_filled_mean_X_index -> Draw("p same");
    draw_text -> DrawLatex(0.21,0.71,Form("Fit vertex X: %.4f #pm%.4f %s", pol0_fit_X->GetParameter(0), pol0_fit_X->GetParError(0), unit_text.c_str() ));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    leg -> Draw("same");
    pol0_fit_X -> Draw("lsame");
    c1 -> Print((output_directory + "/VtxX_index_fit.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    g_quadrant_VtxY_index -> Draw("AP");
    g_line_filled_mean_Y_index -> Draw("p same");
    draw_text -> DrawLatex(0.21,0.71,Form("Numeric average vertex Y: %.4f %s", ( vector_average(line_filled_mean_Y_vec) + vector_average(quadrant_VtxY_vec) )/2., unit_text.c_str() ));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    leg -> Draw("same");
    c1 -> Print((output_directory + "/VtxY_index.pdf").c_str());
    c1 -> Clear();

    g_quadrant_VtxY_index -> Draw("AP");
    g_line_filled_mean_Y_index -> Draw("p same");
    draw_text -> DrawLatex(0.21,0.71,Form("Fit vertex Y: %.4f #pm%.4f %s", pol0_fit_Y->GetParameter(0), pol0_fit_Y->GetParError(0), unit_text.c_str() ));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    leg -> Draw("same");
    pol0_fit_Y -> Draw("lsame");
    c1 -> Print((output_directory + "/VtxY_index_fit.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    quadrant_VtxX_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    draw_text -> DrawLatex(0.2, 0.84, Form("Entry: %.0f", quadrant_VtxX_1D->GetEntries()));
    draw_text -> DrawLatex(0.2, 0.80, Form("Mean: %.4f", quadrant_VtxX_1D->GetMean()));
    draw_text -> DrawLatex(0.2, 0.76, Form("StdDev: %.4f", quadrant_VtxX_1D->GetStdDev()));
    c1 -> Print((output_directory + "/quadrant_VtxX_1D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    quadrant_VtxY_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    draw_text -> DrawLatex(0.2, 0.84, Form("Entry: %.0f", quadrant_VtxY_1D->GetEntries()));
    draw_text -> DrawLatex(0.2, 0.80, Form("Mean: %.4f", quadrant_VtxY_1D->GetMean()));
    draw_text -> DrawLatex(0.2, 0.76, Form("StdDev: %.4f", quadrant_VtxY_1D->GetStdDev()));
    c1 -> Print((output_directory + "/quadrant_VtxY_1D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    line_filled_mean_X_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    draw_text -> DrawLatex(0.2, 0.84, Form("Entry: %.0f", line_filled_mean_X_1D->GetEntries()));
    draw_text -> DrawLatex(0.2, 0.80, Form("Mean: %.4f", line_filled_mean_X_1D->GetMean()));
    draw_text -> DrawLatex(0.2, 0.76, Form("StdDev: %.4f", line_filled_mean_X_1D->GetStdDev()));
    c1 -> Print((output_directory + "/line_filled_mean_X_1D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    line_filled_mean_Y_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    draw_text -> DrawLatex(0.2, 0.84, Form("Entry: %.0f", line_filled_mean_Y_1D->GetEntries()));
    draw_text -> DrawLatex(0.2, 0.80, Form("Mean: %.4f", line_filled_mean_Y_1D->GetMean()));
    draw_text -> DrawLatex(0.2, 0.76, Form("StdDev: %.4f", line_filled_mean_Y_1D->GetStdDev()));
    c1 -> Print((output_directory + "/line_filled_mean_Y_1D.pdf").c_str());
    c1 -> Clear();
    

    c1 -> cd();
    line_filled_stddev_X_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    draw_text -> DrawLatex(0.2, 0.84, Form("Entry: %.0f", line_filled_stddev_X_1D->GetEntries()));
    draw_text -> DrawLatex(0.2, 0.80, Form("Mean: %.4f", line_filled_stddev_X_1D->GetMean()));
    draw_text -> DrawLatex(0.2, 0.76, Form("StdDev: %.4f", line_filled_stddev_X_1D->GetStdDev()));
    c1 -> Print((output_directory + "/line_filled_stddev_X_1D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    line_filled_stddev_Y_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    draw_text -> DrawLatex(0.2, 0.84, Form("Entry: %.0f", line_filled_stddev_Y_1D->GetEntries()));
    draw_text -> DrawLatex(0.2, 0.80, Form("Mean: %.4f", line_filled_stddev_Y_1D->GetMean()));
    draw_text -> DrawLatex(0.2, 0.76, Form("StdDev: %.4f", line_filled_stddev_Y_1D->GetStdDev()));
    c1 -> Print((output_directory + "/line_filled_stddev_Y_1D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    line_filled_variance_X_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    draw_text -> DrawLatex(0.55, 0.84, Form("Entry: %.0f", line_filled_variance_X_1D->GetEntries()));
    draw_text -> DrawLatex(0.55, 0.80, Form("Mean: %.4f", line_filled_variance_X_1D->GetMean()));
    draw_text -> DrawLatex(0.55, 0.76, Form("StdDev: %.4f", line_filled_variance_X_1D->GetStdDev()));
    c1 -> Print((output_directory + "/line_filled_variance_X_1D.pdf").c_str());
    c1 -> Clear();

    c1 -> cd();
    line_filled_variance_Y_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    draw_text -> DrawLatex(0.55, 0.84, Form("Entry: %.0f", line_filled_variance_Y_1D->GetEntries()));
    draw_text -> DrawLatex(0.55, 0.80, Form("Mean: %.4f", line_filled_variance_Y_1D->GetMean()));
    draw_text -> DrawLatex(0.55, 0.76, Form("StdDev: %.4f", line_filled_variance_Y_1D->GetStdDev()));
    c1 -> Print((output_directory + "/line_filled_variance_Y_1D.pdf").c_str());
    c1 -> Clear();
    
    c1 -> cd();
    line_filled_covariance_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    draw_text -> DrawLatex(0.2, 0.84, Form("Entry: %.0f", line_filled_covariance_1D->GetEntries()));
    draw_text -> DrawLatex(0.2, 0.80, Form("Mean: %.4f", line_filled_covariance_1D->GetMean()));
    draw_text -> DrawLatex(0.2, 0.76, Form("StdDev: %.4f", line_filled_covariance_1D->GetStdDev()));
    c1 -> Print((output_directory + "/line_filled_covariance_1D.pdf").c_str());
    c1 -> Clear();

    double out_quadrant_avgX;
    double out_quadrant_avgXE;
    double out_quadrant_avgY;
    double out_quadrant_avgYE;
    double out_linefilled_avgX;
    double out_linefilled_avgXE;
    double out_linefilled_avgY;
    double out_linefilled_avgYE;
    double out_final_num_avgX;
    double out_final_num_avgY;
    double out_final_fit_avgX;
    double out_final_fit_avgY;

    TFile * file_out = new TFile (Form("%s/determined_avg_vtxXY.root",output_directory.c_str()),"recreate");
    TTree * tree_out = new TTree("tree","tree");
    tree_out -> Branch("quadrant_avgX",   &out_quadrant_avgX);
    tree_out -> Branch("quadrant_avgXE",    &out_quadrant_avgXE);
    tree_out -> Branch("quadrant_avgY",   &out_quadrant_avgY);
    tree_out -> Branch("quadrant_avgYE",    &out_quadrant_avgYE);
    tree_out -> Branch("linefilled_avgX",  &out_linefilled_avgX);
    tree_out -> Branch("linefilled_avgXE", &out_linefilled_avgXE);
    tree_out -> Branch("linefilled_avgY",  &out_linefilled_avgY);
    tree_out -> Branch("linefilled_avgYE", &out_linefilled_avgYE);
    tree_out -> Branch("final_num_avgX",   &out_final_num_avgX);
    tree_out -> Branch("final_num_avgY",   &out_final_num_avgY);
    tree_out -> Branch("final_fit_avgX",   &out_final_fit_avgX);
    tree_out -> Branch("final_fit_avgY",   &out_final_fit_avgY);

    out_quadrant_avgX = vector_average(quadrant_VtxX_vec); 
    out_quadrant_avgXE = vector_stddev(quadrant_VtxX_vec); 
    out_quadrant_avgY = vector_average(quadrant_VtxY_vec); 
    out_quadrant_avgYE = vector_stddev(quadrant_VtxY_vec); 
    
    out_linefilled_avgX = vector_average(line_filled_mean_X_vec);
    out_linefilled_avgXE = vector_stddev(line_filled_mean_X_vec);
    out_linefilled_avgY = vector_average(line_filled_mean_Y_vec);
    out_linefilled_avgYE = vector_stddev(line_filled_mean_Y_vec);

    out_final_num_avgX = ( vector_average(line_filled_mean_X_vec) + vector_average(quadrant_VtxX_vec) )/2.;
    out_final_num_avgY = ( vector_average(line_filled_mean_Y_vec) + vector_average(quadrant_VtxY_vec) )/2.;
    out_final_fit_avgX = pol0_fit_X->GetParameter(0);
    out_final_fit_avgY = pol0_fit_Y->GetParameter(0);

    tree_out -> Fill();
    file_out -> cd();
    // tree_out->SetDirectory(file_out);
    tree_out -> Write("", TObject::kOverwrite);
    file_out -> Close();


    cout<<"unit : "<<unit_text<<endl;
    cout<<"line filled covariance info : "<<line_filled_covariance_1D->GetMean()<<" "<<line_filled_covariance_1D->GetStdDev()<<endl;
    cout<<"line filled vairance X info : "<<line_filled_variance_X_1D->GetMean()<<" "<<line_filled_variance_X_1D->GetStdDev()<<endl;
    cout<<"line filled vairance Y info : "<<line_filled_variance_Y_1D->GetMean()<<" "<<line_filled_variance_Y_1D->GetStdDev()<<endl;
    cout<<"line filled stdDev X   info : " <<line_filled_stddev_X_1D->GetMean()<<" "<<line_filled_stddev_X_1D->GetStdDev()<<endl;
    cout<<"line filled stdDev Y   info : " <<line_filled_stddev_Y_1D->GetMean()<<" "<<line_filled_stddev_Y_1D->GetStdDev()<<endl;
    cout<<endl;
    cout<<"unit : "<<unit_text<<endl;
    cout<<"final average vertex XY should be used : "<<endl;
    cout<<"line filled X : "<<vector_average(line_filled_mean_X_vec)<<" +/- "<<vector_stddev(line_filled_mean_X_vec)<<endl;
    cout<<"line filled Y : "<<vector_average(line_filled_mean_Y_vec)<<" +/- "<<vector_stddev(line_filled_mean_Y_vec)<<endl;
    cout<<"quadrant X : "<<vector_average(quadrant_VtxX_vec)<<" +/- "<<vector_stddev(quadrant_VtxX_vec)<<endl;
    cout<<"quadrant Y : "<<vector_average(quadrant_VtxY_vec)<<" +/- "<<vector_stddev(quadrant_VtxY_vec)<<endl;
    cout<<"avg: {"<< out_final_num_avgX <<Form(" * %s, ",unit_text.c_str())<< out_final_num_avgY <<Form(" * %s}",unit_text.c_str())<<endl;
    cout<<"Fit avg: {"<< out_final_fit_avgX <<Form(" * %s, ",unit_text.c_str())<< out_final_fit_avgY <<Form(" * %s}",unit_text.c_str())<<endl;
    return 0;
}
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <numeric>
using namespace std;

#include "sPhenixStyle.C"

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

int FromAvgVtxXY()
{

    std::string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/GeoOffset_v1/completed";
    std::string input_foldername_NoIndex = "Run_00"; // note : Run_00XXX
    std::string filename_NoIndex = "MC_AvgVtxXY_GeoOffset_test1";
    std::string label_text = "Simulation";

    std::pair<double,double> ideal_recoXY = {-0.0214921, 0.223299};

    double frame_shift_forX = ideal_recoXY.first; // note : cm
    double frame_shift_forY = ideal_recoXY.second; // note : cm

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    input_foldername_NoIndex = (input_foldername_NoIndex.back() == '_') ? input_foldername_NoIndex.substr(0, input_foldername_NoIndex.size() - 1) : input_foldername_NoIndex;
    filename_NoIndex = (filename_NoIndex.back() == '_') ? filename_NoIndex.substr(0, filename_NoIndex.size() - 1) : filename_NoIndex;

    std::string file_list_name = "file_list_AvgVtxXY.txt";
    std::string output_directory = input_directory + "/" + "merged_result";

    double X_range_l = (frame_shift_forX - 0.15 );
    double X_range_r = (frame_shift_forX + 0.15 );
    double Y_range_l = (frame_shift_forY - 0.15 );
    double Y_range_r = (frame_shift_forY + 0.15 );

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // note : to generate the merged_result folder
    if(std::filesystem::exists(Form("%s",output_directory.c_str())) == false){
        system(Form("mkdir -p %s", output_directory.c_str()));
        cout<<"----------- check folder exists -----------"<<endl;
        system(Form("ls %s", output_directory.c_str()));
    }
    else 
    {
        cout<<"----------- folder exists already -----------"<<endl;
        system(Form("ls %s", output_directory.c_str()));
    }

    // note : to generate the file_list.txt
    // todo: the file_name
    if(true/*std::filesystem::exists(Form("%s/%s",input_directory.c_str(),file_list_name.c_str())) == false*/){
        system(Form("ls %s/%s*/%s_*.root > %s/%s", input_directory.c_str(), input_foldername_NoIndex.c_str(), filename_NoIndex.c_str(), input_directory.c_str(), file_list_name.c_str()));
        cout<<"----------- file list generated -----------"<<endl;
        system(Form("cat %s/%s", input_directory.c_str(), file_list_name.c_str()));
        cout<<"----------- file list generated -----------"<<endl;
    }
    else 
    {
        cout<<"----------- found the file list -----------"<<endl;
        system(Form("cat %s/%s", input_directory.c_str(), file_list_name.c_str()));
        cout<<"----------- found the file list -----------"<<endl;
    }

    vector<string> file_list = read_list(input_directory, file_list_name);
    
    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1", "c1", 950, 800);
    TGraph * g_vtxXY_ideal = new TGraph();
    g_vtxXY_ideal -> SetPoint(0, ideal_recoXY.first, ideal_recoXY.second);
    g_vtxXY_ideal -> SetMarkerStyle(28);
    g_vtxXY_ideal -> SetMarkerSize(1.5);
    g_vtxXY_ideal -> SetLineWidth(2);
    g_vtxXY_ideal -> SetMarkerColor(kRed);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    TH1D * h1D_vtxX = new TH1D("h1D_vtxX", "h1D_vtxX;Reco. Vtx X [cm];Entries", 100, X_range_l, X_range_r);
    TH1D * h1D_vtxY = new TH1D("h1D_vtxY", "h1D_vtxY;Reco. Vtx Y [cm];Entries", 100, Y_range_l, Y_range_r);
    TH2D * h2D_vtxXY = new TH2D("h2D_vtxXY", "h2D_vtxXY;Reco. Vtx X [cm];Reco. Vtx Y [cm]", 100, X_range_l, X_range_r, 100, Y_range_l, Y_range_r);

    TH1D * h1D_GeoVaryX = new TH1D("h1D_GeoVaryX", "h1D_GeoVaryX;Ladder Variation in X [cm];Entries", 100, -0.03, 0.03);
    TH1D * h1D_GeoVaryY = new TH1D("h1D_GeoVaryY", "h1D_GeoVaryY;Ladder Variation in Y [cm];Entries", 100, -0.03, 0.03);
    TH1D * h1D_GeoVaryZ = new TH1D("h1D_GeoVaryZ", "h1D_GeoVaryZ;Ladder Variation in Z [cm];Entries", 100, -0.03, 0.03);


    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    TChain *chain_vtxXY_in = new TChain("tree_vtxXY");
    for (string file_name : file_list) { chain_vtxXY_in -> Add((file_name).c_str()); }
    cout<<"chain_vtxXY_in -> GetEntries() : "<<chain_vtxXY_in -> GetEntries()<<endl;

    double in_vtxX_quadrant;
    double in_vtxXE_quadrant;
    double in_vtxY_quadrant;
    double in_vtxYE_quadrant;
    double in_vtxX_LineFill;
    double in_vtxXE_LineFill;
    double in_vtxXStdDev_LineFill;
    double in_vtxY_LineFill;
    double in_vtxYE_LineFill;
    double in_vtxYStdDev_LineFill;

    chain_vtxXY_in -> SetBranchAddress("vtxX_quadrant", &in_vtxX_quadrant);
    chain_vtxXY_in -> SetBranchAddress("vtxXE_quadrant", &in_vtxXE_quadrant);
    chain_vtxXY_in -> SetBranchAddress("vtxY_quadrant", &in_vtxY_quadrant);
    chain_vtxXY_in -> SetBranchAddress("vtxYE_quadrant", &in_vtxYE_quadrant);
    chain_vtxXY_in -> SetBranchAddress("vtxX_LineFill", &in_vtxX_LineFill);
    chain_vtxXY_in -> SetBranchAddress("vtxXE_LineFill", &in_vtxXE_LineFill);
    chain_vtxXY_in -> SetBranchAddress("vtxXStdDev_LineFill", &in_vtxXStdDev_LineFill);
    chain_vtxXY_in -> SetBranchAddress("vtxY_LineFill", &in_vtxY_LineFill);
    chain_vtxXY_in -> SetBranchAddress("vtxYE_LineFill", &in_vtxYE_LineFill);
    chain_vtxXY_in -> SetBranchAddress("vtxYStdDev_LineFill", &in_vtxYStdDev_LineFill);

    for (int i = 0; i < chain_vtxXY_in -> GetEntries(); i++)
    {
        chain_vtxXY_in -> GetEntry(i);

        double final_X = (in_vtxX_quadrant + in_vtxX_LineFill) / 2.;
        double final_Y = (in_vtxY_quadrant + in_vtxY_LineFill) / 2.;

        h1D_vtxX -> Fill(final_X);
        h1D_vtxY -> Fill(final_Y);
        h2D_vtxXY -> Fill(final_X, final_Y);
    }

    TChain *chain_GeoOffset_in = new TChain("tree_geooffset");
    for (string file_name : file_list) { chain_GeoOffset_in -> Add((file_name).c_str()); }
    cout<<"chain_GeoOffset_in -> GetEntries() : "<<chain_GeoOffset_in -> GetEntries()<<endl;

    double offset_x;
    double offset_y;
    double offset_z;

    chain_GeoOffset_in -> SetBranchAddress("offset_x", &offset_x);
    chain_GeoOffset_in -> SetBranchAddress("offset_y", &offset_y);
    chain_GeoOffset_in -> SetBranchAddress("offset_z", &offset_z);


    for (int geo_i = 0; geo_i < chain_GeoOffset_in -> GetEntries(); geo_i++)
    {
        chain_GeoOffset_in -> GetEntry(geo_i);

        h1D_GeoVaryX -> Fill(offset_x);
        h1D_GeoVaryY -> Fill(offset_y);
        h1D_GeoVaryZ -> Fill(offset_z);
    }

    TFile * file_out = new TFile(Form("%s/AvgXY_GeoOffset.root", output_directory.c_str()), "RECREATE");
    h1D_vtxX -> Write();
    h1D_vtxY -> Write();
    h2D_vtxXY -> Write();

    h1D_GeoVaryX -> Write();
    h1D_GeoVaryY -> Write();
    h1D_GeoVaryZ -> Write();

    c1 -> cd();
    h2D_vtxXY -> Draw("colz0");
    g_vtxXY_ideal -> Draw("P same");
    c1 -> Write("c1_h2D_vtxXY");
    c1 -> Clear();

    c1 -> cd();
    h2D_vtxXY -> Draw("colz0");
    g_vtxXY_ideal -> Draw("P same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/h2D_AvgXY_GeoOffset.pdf", output_directory.c_str()));
    c1 -> Clear();
    

    file_out -> Close();
    return 0;
}
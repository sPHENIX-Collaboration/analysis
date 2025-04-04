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

void FindH1DUpperLowBound(TH1D * major_hist, TH1D * new_hist, bool IsForUpper)
{
    for (int i = 1; i <= major_hist -> GetNbinsX(); i++)
    {
        if (IsForUpper)
        {
            if (new_hist -> GetBinContent(i) > major_hist -> GetBinContent(i))
            {
                major_hist -> SetBinContent(i, new_hist -> GetBinContent(i));
            }
        }
        else
        {
            if (new_hist -> GetBinContent(i) < major_hist -> GetBinContent(i))
            {
                major_hist -> SetBinContent(i, new_hist -> GetBinContent(i));
            }
        }
    }
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

int FromdNdEta()
{

    std::string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/GeoOffset/completed";
    std::string input_foldername_NoIndex = "Run_00"; // note : Run_00XXX
    std::string filename_NoIndex = "MC_PreparedNdEtaEach_AllSensor_VtxZ10_Mbin70_test1"; // note : take out the _00001_dNdEta.root

    std::string file_directory_ideal = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/GeoOffset/completed/Run_NoGeoOffset_00000/MC_PreparedNdEtaEach_AllSensor_VtxZ10_Mbin70_test1_00000_dNdEta.root";

    std::string label_text = "Simulation";

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    input_foldername_NoIndex = (input_foldername_NoIndex.back() == '_') ? input_foldername_NoIndex.substr(0, input_foldername_NoIndex.size() - 1) : input_foldername_NoIndex;
    filename_NoIndex = (filename_NoIndex.back() == '_') ? filename_NoIndex.substr(0, filename_NoIndex.size() - 1) : filename_NoIndex;

    std::string file_list_name = "file_list_dNdEta.txt";
    std::string output_directory = input_directory + "/" + "merged_result";

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
        system(Form("ls %s/%s*/%s_*_dNdEta.root > %s/%s", input_directory.c_str(), input_foldername_NoIndex.c_str(), filename_NoIndex.c_str(), input_directory.c_str(), file_list_name.c_str()));
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
    TFile * file_in_ideal = TFile::Open(Form("%s", file_directory_ideal.c_str()));
    TH1D * h1D_BestPair_RecoTrackletEtaPerEvt_ideal = (TH1D*)file_in_ideal->Get("h1D_BestPair_RecoTrackletEtaPerEvt");
    h1D_BestPair_RecoTrackletEtaPerEvt_ideal -> SetLineColor(2);
    // h1D_BestPair_RecoTrackletEtaPerEvt_ideal -> Scale(1. / h1D_BestPair_RecoTrackletEtaPerEvt_ideal -> Integral(-1,-1));

    TH1D * h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal = (TH1D*)file_in_ideal->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> SetLineColor(2);
    // h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> Scale(1. / h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> Integral(-1,-1));

    std::map<int,std::vector<double>> EtaBin_Variation_vec; EtaBin_Variation_vec.clear();
    for (int i = 0; i < h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> GetNbinsX(); i++)
    {
        EtaBin_Variation_vec[i] = {};
    }

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    TH1D * h1D_RotatedBkg_RecoTrackletEtaPerEvt_LowerBound = (TH1D*) h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvt_LowerBound");
    TH1D * h1D_RotatedBkg_RecoTrackletEtaPerEvt_UpperBound = (TH1D*) h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvt_UpperBound");

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    TFile * file_in;
    TH2D * h2D_BestPair_RecoTrackletEtaPerEvt;
    TH2D * h2D_RotatedBkg_RecoTrackletEtaPerEvt;

    TH1D * h1D_BestPair_RecoTrackletEtaPerEvt;
    TH1D * h1D_RotatedBkg_RecoTrackletEtaPerEvt;

    for (int i = 0; i < file_list.size(); i++)
    {
        file_in = TFile::Open(Form("%s", file_list[i].c_str()));

        h1D_BestPair_RecoTrackletEtaPerEvt = (TH1D*) file_in -> Get("h1D_BestPair_RecoTrackletEtaPerEvt");
        h1D_RotatedBkg_RecoTrackletEtaPerEvt = (TH1D*) file_in -> Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");

        // h1D_BestPair_RecoTrackletEtaPerEvt -> Scale(1. / h1D_BestPair_RecoTrackletEtaPerEvt -> Integral(-1, -1));
        // h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Scale(1. / h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Integral(-1, -1));

        FindH1DUpperLowBound(h1D_RotatedBkg_RecoTrackletEtaPerEvt_LowerBound, h1D_RotatedBkg_RecoTrackletEtaPerEvt, false);
        FindH1DUpperLowBound(h1D_RotatedBkg_RecoTrackletEtaPerEvt_UpperBound, h1D_RotatedBkg_RecoTrackletEtaPerEvt, true);

        for (int eta_bin = 0; eta_bin < h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetNbinsX(); eta_bin++)
        {
            EtaBin_Variation_vec[eta_bin].push_back(
                (h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetBinContent(eta_bin + 1) - h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> GetBinContent(eta_bin + 1)) / h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> GetBinContent(eta_bin + 1)
            );
        }

        if (i == 0)
        {
            h2D_BestPair_RecoTrackletEtaPerEvt = new TH2D(
                "h2D_BestPair_RecoTrackletEtaPerEvt",
                "h2D_BestPair_RecoTrackletEtaPerEvt;Tracklet #eta; N Reco. Tracklets per event",
                h1D_BestPair_RecoTrackletEtaPerEvt -> GetNbinsX(), h1D_BestPair_RecoTrackletEtaPerEvt -> GetXaxis() -> GetXmin(), h1D_BestPair_RecoTrackletEtaPerEvt -> GetXaxis() -> GetXmax(),
                200, h1D_BestPair_RecoTrackletEtaPerEvt -> GetMinimum(), h1D_BestPair_RecoTrackletEtaPerEvt -> GetBinContent(h1D_BestPair_RecoTrackletEtaPerEvt -> GetMaximumBin()) * 1.2
            );

            h2D_RotatedBkg_RecoTrackletEtaPerEvt = new TH2D(
                "h2D_RotatedBkg_RecoTrackletEtaPerEvt",
                "h2D_RotatedBkg_RecoTrackletEtaPerEvt;Tracklet #eta; N Reco. Tracklets per event",
                h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetNbinsX(), h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetXaxis() -> GetXmin(), h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetXaxis() -> GetXmax(),
                200, h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetMinimum(), h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetBinContent(h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetMaximumBin()) * 1.2
            );
        }

        for (int bin_i = 1; bin_i <= h1D_BestPair_RecoTrackletEtaPerEvt -> GetNbinsX(); bin_i++)
        {
            h2D_BestPair_RecoTrackletEtaPerEvt -> Fill(
                h1D_BestPair_RecoTrackletEtaPerEvt -> GetBinCenter(bin_i),
                h1D_BestPair_RecoTrackletEtaPerEvt -> GetBinContent(bin_i)
            );
        }

        for (int bin_i = 1; bin_i <= h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetNbinsX(); bin_i++)
        {
            h2D_RotatedBkg_RecoTrackletEtaPerEvt -> Fill(
                h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetBinCenter(bin_i),
                h1D_RotatedBkg_RecoTrackletEtaPerEvt -> GetBinContent(bin_i)
            );
        }
    }

    TH1D * h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma = (TH1D*) h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> Reset("ICESM");
    for (int i = 0; i < h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> GetNbinsX(); i++)
    {
        double variation_stddev = vector_stddev(EtaBin_Variation_vec[i]);
        std::cout<<"EtaBin: "<<i+1<<" std: "<<variation_stddev<<std::endl;

        if (variation_stddev != variation_stddev){continue;}// note : skip the nan value

        h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> SetBinContent(i + 1, 3 * vector_stddev(EtaBin_Variation_vec[i]));
    }



    h1D_RotatedBkg_RecoTrackletEtaPerEvt_LowerBound -> Divide(h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal);
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_UpperBound -> Divide(h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal);
    TH1D * h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax = (TH1D*) h1D_RotatedBkg_RecoTrackletEtaPerEvt_LowerBound -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax");

    for (int i = 1; i <= h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> GetNbinsX(); i++)
    {   
        double fabs_bin_content_lower = fabs(h1D_RotatedBkg_RecoTrackletEtaPerEvt_LowerBound -> GetBinContent(i) - 1);
        double fabs_bin_content_upper = fabs(h1D_RotatedBkg_RecoTrackletEtaPerEvt_UpperBound -> GetBinContent(i) - 1);
        double bin_content = (fabs_bin_content_lower > fabs_bin_content_upper) ? fabs_bin_content_lower : fabs_bin_content_upper;

        h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> SetBinContent(i, bin_content);
    }

    TFile * file_out = new TFile(Form("%s/FromdNdEta_New.root", output_directory.c_str()), "RECREATE");
    h2D_BestPair_RecoTrackletEtaPerEvt -> Write();
    h2D_RotatedBkg_RecoTrackletEtaPerEvt -> Write();

    c1 -> cd();
    h2D_BestPair_RecoTrackletEtaPerEvt -> Draw("colz0");
    h1D_BestPair_RecoTrackletEtaPerEvt_ideal -> Draw("hist same");
    c1 -> Write("c1_BestPair_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    c1 -> cd();
    h2D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("colz0");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> Draw("hist same");
    c1 -> Write("c1_RotatedBkg_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    h1D_RotatedBkg_RecoTrackletEtaPerEvt_LowerBound -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_UpperBound -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> Write();

    file_out -> Close();

    // Division: ---------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    h2D_BestPair_RecoTrackletEtaPerEvt -> Draw("colz0");
    h1D_BestPair_RecoTrackletEtaPerEvt_ideal -> Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/c1_BestPair_RecoTrackletEtaPerEvt.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    h2D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("colz0");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_ideal -> Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/c1_RotatedBkg_RecoTrackletEtaPerEvt.pdf", output_directory.c_str()));
    c1 -> Clear();


    c1 -> cd();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> GetXaxis() -> SetTitle("Tracklet #eta");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> GetYaxis() -> SetTitle("Variation Max in Reco. Tracklets");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> SetLineColor(4);
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> SetLineColorAlpha(4, 0);
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> SetMinimum(0);
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> SetMaximum(0.1);
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax -> Draw("p");

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> GetXaxis() -> SetTitle("Tracklet #eta");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> GetYaxis() -> SetTitle("3#sigma of variation in Reco. Tracklets");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> SetLineColorAlpha(4,0);
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> SetMinimum(0);
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> SetMaximum(0.1);
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma -> Draw("p");

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma.pdf", output_directory.c_str()));
    c1 -> Clear();

    return 0;
}
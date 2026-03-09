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


int FromTracklet()
{

    std::string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/GeoOffset/completed";
    std::string input_foldername_NoIndex = "Run_00"; // note : Run_00XXX
    std::string filename_NoIndex = "MC_TrackHist_VtxZQA_ClusQAAdc35PhiSize40_ColMulMask_GeoOffset_test1";

    std::string ideal_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/GeoOffset/completed/Run_NoGeoOffset_00000/MC_TrackHist_VtxZQA_ClusQAAdc35PhiSize40_ColMulMask_test1_00000.root"; // note : to .root

    std::string label_text = "Simulation";

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    input_foldername_NoIndex = (input_foldername_NoIndex.back() == '_') ? input_foldername_NoIndex.substr(0, input_foldername_NoIndex.size() - 1) : input_foldername_NoIndex;
    filename_NoIndex = (filename_NoIndex.back() == '_') ? filename_NoIndex.substr(0, filename_NoIndex.size() - 1) : filename_NoIndex;

    std::string file_list_name = "file_list_Tracklet.txt";
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
    TCanvas * c1 = new TCanvas("c1","c1", 950, 800);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TFile * file_in_ideal = TFile::Open(Form("%s", ideal_directory.c_str()));
    TH1D * h1D_INTTvtxZ_ideal = (TH1D*) file_in_ideal -> Get("h1D_INTTvtxZ");
    h1D_INTTvtxZ_ideal -> SetLineColor(2);
    h1D_INTTvtxZ_ideal -> SetLineWidth(1);
    h1D_INTTvtxZ_ideal -> Scale(1./h1D_INTTvtxZ_ideal->Integral());

    TH1D * h1D_DeltaPhi_ideal = (TH1D*) file_in_ideal -> Get("h1D_PairDeltaPhi_inclusive");
    h1D_DeltaPhi_ideal -> SetLineColor(2);
    h1D_DeltaPhi_ideal -> SetLineWidth(1);
    h1D_DeltaPhi_ideal -> Scale(1./h1D_DeltaPhi_ideal->Integral());

    TH1D * h1D_DeltaEta_ideal = (TH1D*) file_in_ideal -> Get("h1D_PairDeltaEta_inclusive");
    h1D_DeltaEta_ideal -> SetLineColor(2);
    h1D_DeltaEta_ideal -> SetLineWidth(1);
    h1D_DeltaEta_ideal -> Scale(1./h1D_DeltaEta_ideal->Integral());

    TH1D * h1D_Inner_ClusEtaInttZ_ideal = (TH1D*) file_in_ideal -> Get("h1D_Inner_ClusEta_INTTz");
    h1D_Inner_ClusEtaInttZ_ideal -> SetLineColor(2);
    h1D_Inner_ClusEtaInttZ_ideal -> SetLineWidth(1);
    h1D_Inner_ClusEtaInttZ_ideal -> Scale(1./h1D_Inner_ClusEtaInttZ_ideal->Integral());

    TH1D * h1D_Outer_ClusEtaInttZ_ideal = (TH1D*) file_in_ideal -> Get("h1D_Outer_ClusEta_INTTz");
    h1D_Outer_ClusEtaInttZ_ideal -> SetLineColor(2);
    h1D_Outer_ClusEtaInttZ_ideal -> SetLineWidth(1);
    h1D_Outer_ClusEtaInttZ_ideal -> Scale(1./h1D_Outer_ClusEtaInttZ_ideal->Integral());


    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    TH2D * h2D_INTTvtxZ; // = new TH2D("h2D_INTTvtxZ","h2D_INTTvtxZ;INTT vtx Z [cm];Entries", 60, -60, 60, 200,0,0.2);

    TH2D * h2D_DeltaPhi; // = new TH2D("h2D_DeltaPhi","h2D_DeltaPhi;Pair #Delta#phi;Entries", 140, -0.07, 0.07, 200,0,0.5);
    TH2D * h2D_DeltaEta; // = new TH2D("h2D_DeltaEta","h2D_DeltaEta;Pair #Delta#eta;Entries", 100, -1, 1, 200,0,0.07);

    TH2D * h2D_Inner_ClusEtaInttZ; // = new TH2D("h2D_Inner_ClusEtaInttZ","h2D_Inner_ClusEtaInttZ;Cluster #eta (Inner, INTTz);Entries", 27, -2.7, 2.7, 200,0,0.5);
    TH2D * h2D_Outer_ClusEtaInttZ; // = new TH2D("h2D_Outer_ClusEtaInttZ","h2D_Outer_ClusEtaInttZ;Cluster #eta (Outer, INTTz);Entries", 27, -2.7, 2.7, 200,0,0.5);
    

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    TFile * file_in;

    TH1D * h1D_INTTvtxZ_each;

    TH1D * h1D_DeltaPhi_each;
    TH1D * h1D_DeltaEta_each;

    TH1D * h1D_Inner_ClusEtaInttZ_each;
    TH1D * h1D_Outer_ClusEtaInttZ_each;

    for (int i = 0; i < file_list.size(); i++)
    {
        file_in = TFile::Open(Form("%s", file_list[i].c_str()));

        h1D_INTTvtxZ_each = (TH1D*) file_in -> Get("h1D_INTTvtxZ"); 
        
        h1D_DeltaPhi_each = (TH1D*) file_in -> Get("h1D_PairDeltaPhi_inclusive");
        h1D_DeltaEta_each = (TH1D*) file_in -> Get("h1D_PairDeltaEta_inclusive");

        h1D_Inner_ClusEtaInttZ_each = (TH1D*) file_in -> Get("h1D_Inner_ClusEta_INTTz"); 
        h1D_Outer_ClusEtaInttZ_each = (TH1D*) file_in -> Get("h1D_Outer_ClusEta_INTTz"); 

        h1D_INTTvtxZ_each->Scale( 1. / h1D_INTTvtxZ_each -> Integral() );
        h1D_DeltaPhi_each->Scale( 1. / h1D_DeltaPhi_each -> Integral() );
        h1D_DeltaEta_each->Scale( 1. / h1D_DeltaEta_each -> Integral() );
        h1D_Inner_ClusEtaInttZ_each->Scale( 1. / h1D_Inner_ClusEtaInttZ_each -> Integral() );
        h1D_Outer_ClusEtaInttZ_each->Scale( 1. / h1D_Outer_ClusEtaInttZ_each -> Integral() );

        if (i == 0)
        {

            
            h2D_INTTvtxZ = new TH2D(
                "h2D_INTTvtxZ",
                "h2D_INTTvtxZ;INTT vtx Z [cm];Entries", 
                h1D_INTTvtxZ_each -> GetNbinsX(), h1D_INTTvtxZ_each -> GetXaxis() -> GetXmin(), h1D_INTTvtxZ_each -> GetXaxis() -> GetXmax(),
                200, h1D_INTTvtxZ_each -> GetMinimum(), h1D_INTTvtxZ_each -> GetBinContent(h1D_INTTvtxZ_each -> GetMaximumBin()) * 1.2
            );
            
            h2D_DeltaPhi = new TH2D(
                "h2D_DeltaPhi",
                "h2D_DeltaPhi;Pair #Delta#phi;Entries", 
                h1D_DeltaPhi_each -> GetNbinsX(), h1D_DeltaPhi_each -> GetXaxis() -> GetXmin(), h1D_DeltaPhi_each -> GetXaxis() -> GetXmax(),
                200, h1D_DeltaPhi_each -> GetMinimum(), h1D_DeltaPhi_each -> GetBinContent(h1D_DeltaPhi_each -> GetMaximumBin()) * 1.8
            );
            
            h2D_DeltaEta = new TH2D(
                "h2D_DeltaEta",
                "h2D_DeltaEta;Pair #Delta#eta;Entries", 
                h1D_DeltaEta_each -> GetNbinsX(), h1D_DeltaEta_each -> GetXaxis() -> GetXmin(), h1D_DeltaEta_each -> GetXaxis() -> GetXmax(),
                200, h1D_DeltaEta_each -> GetMinimum(), h1D_DeltaEta_each -> GetBinContent(h1D_DeltaEta_each -> GetMaximumBin()) * 1.2
            );
            
            h2D_Inner_ClusEtaInttZ = new TH2D(
                "h2D_Inner_ClusEtaInttZ",
                "h2D_Inner_ClusEtaInttZ;Cluster #eta (Inner, INTTz);Entries", 
                h1D_Inner_ClusEtaInttZ_each -> GetNbinsX(), h1D_Inner_ClusEtaInttZ_each -> GetXaxis() -> GetXmin(), h1D_Inner_ClusEtaInttZ_each -> GetXaxis() -> GetXmax(),
                200, h1D_Inner_ClusEtaInttZ_each -> GetMinimum(), h1D_Inner_ClusEtaInttZ_each -> GetBinContent(h1D_Inner_ClusEtaInttZ_each -> GetMaximumBin()) * 1.2
            );
            
            h2D_Outer_ClusEtaInttZ = new TH2D(
                "h2D_Outer_ClusEtaInttZ",
                "h2D_Outer_ClusEtaInttZ;Cluster #eta (Outer, INTTz);Entries", 
                h1D_Outer_ClusEtaInttZ_each -> GetNbinsX(), h1D_Outer_ClusEtaInttZ_each -> GetXaxis() -> GetXmin(), h1D_Outer_ClusEtaInttZ_each -> GetXaxis() -> GetXmax(),
                200, h1D_Outer_ClusEtaInttZ_each -> GetMinimum(), h1D_Outer_ClusEtaInttZ_each -> GetBinContent(h1D_Outer_ClusEtaInttZ_each -> GetMaximumBin()) * 1.2
            );
        }

        // if (h1D_INTTvtxZ_each->GetNbinsX() != h2D_INTTvtxZ -> GetNbinsX() || h1D_INTTvtxZ_each->GetXaxis()->GetXmin() != h2D_INTTvtxZ -> GetXaxis()->GetXmin() || h1D_INTTvtxZ_each->GetXaxis()->GetXmax() != h2D_INTTvtxZ -> GetXaxis()->GetXmax())
        // {
        //     cout<<"Error : the binning of the h1D_INTTvtxZ_each is not consistent with the h2D_INTTvtxZ"<<endl;
        //     cout<<"h1D_INTTvtxZ_each setup: ("<<h1D_INTTvtxZ_each->GetNbinsX()<<", "<<h1D_INTTvtxZ_each->GetXaxis()->GetXmin()<<", "<<h1D_INTTvtxZ_each->GetXaxis()->GetXmax()<<")"<<endl;
        //     exit(1);
        // }

        // if (h1D_DeltaPhi_each->GetNbinsX() != h2D_DeltaPhi -> GetNbinsX() || h1D_DeltaPhi_each->GetXaxis()->GetXmin() != h2D_DeltaPhi -> GetXaxis()->GetXmin() || h1D_DeltaPhi_each->GetXaxis()->GetXmax() != h2D_DeltaPhi -> GetXaxis()->GetXmax())
        // {
        //     cout<<"Error : the binning of the h1D_DeltaPhi_each is not consistent with the h2D_DeltaPhi"<<endl;
        //     cout<<"h1D_DeltaPhi_each setup: ("<<h1D_DeltaPhi_each->GetNbinsX()<<", "<<h1D_DeltaPhi_each->GetXaxis()->GetXmin()<<", "<<h1D_DeltaPhi_each->GetXaxis()->GetXmax()<<")"<<endl;
        //     exit(1);
        // }

        // if (h1D_DeltaEta_each->GetNbinsX() != h2D_DeltaEta -> GetNbinsX() || h1D_DeltaEta_each->GetXaxis()->GetXmin() != h2D_DeltaEta -> GetXaxis()->GetXmin() || h1D_DeltaEta_each->GetXaxis()->GetXmax() != h2D_DeltaEta -> GetXaxis()->GetXmax())
        // {
        //     cout<<"Error : the binning of the h1D_DeltaEta_each is not consistent with the h2D_DeltaEta"<<endl;
        //     cout<<"h1D_DeltaEta_each setup: ("<<h1D_DeltaEta_each->GetNbinsX()<<", "<<h1D_DeltaEta_each->GetXaxis()->GetXmin()<<", "<<h1D_DeltaEta_each->GetXaxis()->GetXmax()<<")"<<endl;
        //     exit(1);
        // }

        // if (h1D_Inner_ClusEtaInttZ_each->GetNbinsX() != h2D_Inner_ClusEtaInttZ -> GetNbinsX() || h1D_Inner_ClusEtaInttZ_each->GetXaxis()->GetXmin() != h2D_Inner_ClusEtaInttZ -> GetXaxis()->GetXmin() || h1D_Inner_ClusEtaInttZ_each->GetXaxis()->GetXmax() != h2D_Inner_ClusEtaInttZ -> GetXaxis()->GetXmax())
        // {
        //     cout<<"Error : the binning of the h1D_Inner_ClusEtaInttZ_each is not consistent with the h2D_Inner_ClusEtaInttZ"<<endl;
        //     cout<<"h1D_Inner_ClusEtaInttZ_each setup: ("<<h1D_Inner_ClusEtaInttZ_each->GetNbinsX()<<", "<<h1D_Inner_ClusEtaInttZ_each->GetXaxis()->GetXmin()<<", "<<h1D_Inner_ClusEtaInttZ_each->GetXaxis()->GetXmax()<<")"<<endl;
        //     exit(1);
        // }

        // if (h1D_Outer_ClusEtaInttZ_each->GetNbinsX() != h2D_Outer_ClusEtaInttZ -> GetNbinsX() || h1D_Outer_ClusEtaInttZ_each->GetXaxis()->GetXmin() != h2D_Outer_ClusEtaInttZ -> GetXaxis()->GetXmin() || h1D_Outer_ClusEtaInttZ_each->GetXaxis()->GetXmax() != h2D_Outer_ClusEtaInttZ -> GetXaxis()->GetXmax())
        // {
        //     cout<<"Error : the binning of the h1D_Outer_ClusEtaInttZ_each is not consistent with the h2D_Outer_ClusEtaInttZ"<<endl;
        //     cout<<"h1D_Outer_ClusEtaInttZ_each setup: ("<<h1D_Outer_ClusEtaInttZ_each->GetNbinsX()<<", "<<h1D_Outer_ClusEtaInttZ_each->GetXaxis()->GetXmin()<<", "<<h1D_Outer_ClusEtaInttZ_each->GetXaxis()->GetXmax()<<")"<<endl;
        //     exit(1);
        // }

        
        for (int bin_i = 1; bin_i <= h1D_INTTvtxZ_each->GetNbinsX(); bin_i++)
        {
            h2D_INTTvtxZ -> Fill(h1D_INTTvtxZ_each->GetBinCenter(bin_i), h1D_INTTvtxZ_each->GetBinContent(bin_i));
        }
        
        for (int bin_i = 1; bin_i <= h1D_DeltaPhi_each->GetNbinsX(); bin_i++)
        {
            h2D_DeltaPhi -> Fill(h1D_DeltaPhi_each->GetBinCenter(bin_i), h1D_DeltaPhi_each->GetBinContent(bin_i));
        }

        for (int bin_i = 1; bin_i <= h1D_DeltaEta_each->GetNbinsX(); bin_i++)
        {
            h2D_DeltaEta -> Fill(h1D_DeltaEta_each->GetBinCenter(bin_i), h1D_DeltaEta_each->GetBinContent(bin_i));
        }

        for (int bin_i = 1; bin_i <= h1D_Inner_ClusEtaInttZ_each->GetNbinsX(); bin_i++)
        {
            h2D_Inner_ClusEtaInttZ -> Fill(h1D_Inner_ClusEtaInttZ_each->GetBinCenter(bin_i), h1D_Inner_ClusEtaInttZ_each->GetBinContent(bin_i));
        }

        for (int bin_i = 1; bin_i <= h1D_Outer_ClusEtaInttZ_each->GetNbinsX(); bin_i++)
        {
            h2D_Outer_ClusEtaInttZ -> Fill(h1D_Outer_ClusEtaInttZ_each->GetBinCenter(bin_i), h1D_Outer_ClusEtaInttZ_each->GetBinContent(bin_i));
        }

        // file_in -> Close();
    }

    TFile * file_out = new TFile(Form("%s/FromTrack.root", output_directory.c_str()), "RECREATE");
    h2D_INTTvtxZ -> Write();
    h2D_DeltaPhi -> Write();
    h2D_DeltaEta -> Write();
    h2D_Inner_ClusEtaInttZ -> Write();
    h2D_Outer_ClusEtaInttZ -> Write();

    
    c1 -> cd();
    h2D_INTTvtxZ -> Draw("colz0");
    h1D_INTTvtxZ_ideal -> Draw("hist same");
    c1 -> Write("c1_h2D_INTTvtxZ");
    c1 -> Clear();
    
    c1 -> cd();
    h2D_DeltaPhi -> Draw("colz0");
    h1D_DeltaPhi_ideal -> Draw("hist same");
    c1 -> Write("c1_h2D_DeltaPhi");
    c1 -> Clear();
    
    c1 -> cd();
    h2D_DeltaEta -> Draw("colz0");
    h1D_DeltaEta_ideal -> Draw("hist same");
    c1 -> Write("c1_h2D_DeltaEta");
    c1 -> Clear();
    
    c1 -> cd();
    h2D_Inner_ClusEtaInttZ -> Draw("colz0");
    h1D_Inner_ClusEtaInttZ_ideal -> Draw("hist same");
    c1 -> Write("c1_h2D_Inner_ClusEtaInttZ");
    c1 -> Clear();
    
    c1 -> cd();
    h2D_Outer_ClusEtaInttZ -> Draw("colz0");
    h1D_Outer_ClusEtaInttZ_ideal -> Draw("hist same");
    c1 -> Write("c1_h2D_Outer_ClusEtaInttZ");
    c1 -> Clear();

    file_out -> Close();

    // Division: -----------------------------------------------------------------------------------------------
    c1 -> cd();
    h2D_INTTvtxZ -> Draw("colz0");
    h1D_INTTvtxZ_ideal -> Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/c1_h2D_INTTvtxZ.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    h2D_DeltaPhi -> Draw("colz0");
    h1D_DeltaPhi_ideal -> Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/c1_h2D_DeltaPhi.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    h2D_DeltaEta -> Draw("colz0");
    h1D_DeltaEta_ideal -> Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/c1_h2D_DeltaEta.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    h2D_Inner_ClusEtaInttZ -> Draw("colz0");
    h1D_Inner_ClusEtaInttZ_ideal -> Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/c1_h2D_Inner_ClusEtaInttZ.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    h2D_Outer_ClusEtaInttZ -> Draw("colz0");
    h1D_Outer_ClusEtaInttZ_ideal -> Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", label_text.c_str()));
    c1 -> Print(Form("%s/c1_h2D_Outer_ClusEtaInttZ.pdf", output_directory.c_str()));
    c1 -> Clear();

    return 0;
}
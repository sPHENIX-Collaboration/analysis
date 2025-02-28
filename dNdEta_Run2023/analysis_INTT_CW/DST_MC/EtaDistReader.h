#ifndef EtaDistReader_h
#define EtaDistReader_h

#include <filesystem>
#include <cstdlib>

#include "INTTEta.h"


// note : the code works with the histograms, so the centrality bins are already defined in the histograms, ranging from 0 to something big number
// todo : the signal_region and centrality_region is hard_written in the INTTEta.h, which is written in the "ana_map_v1.h", so modify there


class EtaDistReader : public INTTEta
{
    public:
        EtaDistReader(string run_type, string out_folder_directory, vector<int> fulleta_MC_included_z_bin, vector<pair<int,vector<int>>> included_eta_z_map, string input_file_directory, bool centrality_Z_map_bool = 0) : 
        INTTEta(run_type, out_folder_directory), fulleta_MC_included_z_bin(fulleta_MC_included_z_bin), included_eta_z_map(included_eta_z_map), input_file_directory(input_file_directory), centrality_Z_map_bool(centrality_Z_map_bool)
        {
            gErrorIgnoreLevel = kError;

            cout<<"In EtaDistReader, centrality region size : "<<centrality_region.size()<<endl;    
            cout<<"In EtaDistReader, eta region size : "<<eta_region.size() - 1<<endl;
            cout<<"In EtaDistReader, plot_text: "<<plot_text<<endl;
            cout<<"In EtaDistReader, signal_region: "<<signal_region<<endl;

            if(std::filesystem::exists(Form("%s",out_folder_directory.c_str())) == false){
                system(Form("mkdir %s", out_folder_directory.c_str()));
                cout<<"----------- check folder exists -----------"<<endl;
                system(Form("ls %s", out_folder_directory.c_str()));
            }
            else 
            {
                cout<<"----------- folder exists already -----------"<<endl;
                system(Form("ls %s", out_folder_directory.c_str()));
            }


            file_in = TFile::Open(input_file_directory.c_str());

            N_centrality_bin = centrality_region.size();
            eta_correction = included_eta_z_map[0].first - 1; // todo : the eta bin starts from 15, so the correction is 14
            
            SignalNTrack_eta_z_Multi_2D.clear();
            SignalNTrack_eta_z_Single_2D.clear();
            TrueNtrack_eta_z_MC_2D.clear();
            TrueNtrack_eta_z_fulleta_MC_2D.clear();
            dNdeta_1D_MC.clear();
            dNdeta_1D_fulleta_MC.clear();
            dNdeta_1D_reco_single_original.clear();
            dNdeta_1D_reco_multi_original.clear();
            dNdeta_1D_reco_single_postalpha = vector<TH1F*>(N_centrality_bin, nullptr);
            dNdeta_1D_reco_multi_postalpha = vector<TH1F*>(N_centrality_bin, nullptr);
            DeltaPhi_Multi_1D.clear();
            DeltaPhi_Multi_Stack_hist_out.clear();
            DeltaPhi_Multi_Stack.clear();
            SignalNTrack_Single.clear(); 
            SignalNTrack_Multi.clear();
            N_event_counting            = vector<vector<double>>(N_centrality_bin, vector<double>(included_eta_z_map.size(),0));
            N_event_counting_MC         = vector<vector<int>>(N_centrality_bin, vector<int>(included_eta_z_map.size(),0));
            N_event_counting_fulleta_MC = vector<vector<int>>(N_centrality_bin, vector<int>(eta_region.size() - 1, 0)); // note : N centrality_bin x N eta_bin

            DeltaPhi_Multi_Stack_hist_out_radian.clear();
            DeltaPhi_Multi_Stack_radian.clear();
            DeltaPhi_Multi_1D_radian.clear();
            
            
            solid_line = new TLine();
            solid_line -> SetLineWidth(1);
            solid_line -> SetLineColor(2);
            solid_line -> SetLineStyle(1);

            // file_out = new TFile(Form("%s/alpha_correction_map.root",out_folder_directory.c_str()),"RECREATE");

            legend = new TLegend(0.45,0.8,0.70,0.9);
            // legend -> SetMargin(0);
            legend->SetTextSize(0.03);

            post_alpha_tag = 0;

            ReadFileHist();
            InitHist();
            MainPreparation();
            HistDivision();
            PrintPlots_exclusive();
            // FinaldNdEta();

            return;
        };

        vector<TH1F *> GetDeltaPhi_Multi_stack_1D();
        vector<TH1F *> GetDeltaPhi_Multi_stack_1D_radian();
        vector<TH1F *> GetdNdeta_1D_MC();
        vector<TH1F *> GetdNdeta_1D_fulleta_MC();
        vector<TH1F *> GetdNdeta_1D_reco_single_original();
        vector<TH1F *> GetdNdeta_1D_reco_multi_original();
        vector<TH1F *> GetdNdeta_1D_reco_single_postalpha();
        vector<TH1F *> GetdNdeta_1D_reco_multi_postalpha();
        vector<string> Get_centrality_region() {return centrality_region;};
        string         Get_eta_region_txt(int bin_x_id);
        vector<TH2F *> Get_alpha_corr_map();
        void FindCoveredRegion();
        void DeriveAlphaCorrection(int correction_step);
        void Set_alpha_corr_map(TH2F * hist_in_loose, TH2F * hist_in_tight);
        void ApplyAlphaCorrection();
        void PrintPlots();
        void PrintPlots_exclusive();
        // void FinaldNdEta();
        void EndRun();

        ~EtaDistReader()
        {
            file_in -> Close();
            cout<<"EtaDistReader done, goodbye"<<endl;
        };

    protected:

        bool centrality_Z_map_bool;
        vector<pair<int,vector<int>>> included_eta_z_map;
        vector<int> fulleta_MC_included_z_bin;
        string input_file_directory;
        int eta_correction; // todo : the eta bin starts from 15, so the correction is 14
        int N_centrality_bin;

        TFile * file_in;
        string color_code[5] = {"#fedc97", "#b5b682", "#7c9885", "#28666e", "#033f63"};
        // vector<string> centrality_region;

        vector<TH2F *> SignalNTrack_eta_z_Multi_2D;
        vector<TH2F *> SignalNTrack_eta_z_Single_2D;
        map<string,TH1F *> DeltaPhi_Multi_1D;
        vector<TH2F *> TrueNtrack_eta_z_MC_2D;
        vector<TH2F *> TrueNtrack_eta_z_fulleta_MC_2D;
        TH2F * centrality_Z_map;
        TH2F * centrality_Z_map_MC;
        TH2F * eta_z_ref;
        TH2F * eta_z_ref_used_check;
        TH2F * eta_z_ref_used_check_binID;

        TH2F * eta_z_ref_cm;
        TH2F * eta_z_ref_used_check_cm;
        TH2F * eta_z_ref_used_check_binID_cm;
        TH2F * exclusive_cluster_inner_eta_phi_2D;
        TH2F * exclusive_cluster_outer_eta_phi_2D;

        TH2F * exclusive_cluster_inner_eta_adc_2D;
        TH2F * exclusive_cluster_outer_eta_adc_2D;
        TH2F * exclusive_cluster_inner_eta_adc_2D_bkgrm;
        TH2F * exclusive_cluster_outer_eta_adc_2D_bkgrm;
        TProfile * exclusive_cluster_inner_eta_adc_2D_bkgrm_profile;
        TProfile * exclusive_cluster_outer_eta_adc_2D_bkgrm_profile;
        TGraphErrors * exclusive_cluster_inner_eta_adc_2D_bkgrm_profile_graph;
        TGraphErrors * exclusive_cluster_outer_eta_adc_2D_bkgrm_profile_graph;


        TH1F * exclusive_cluster_inner_adc;
        TH1F * exclusive_cluster_outer_adc;

        TH2F * eta_Mbin_correction_tight;
        TH2F * eta_Mbin_correction_loose;
        TH2F * eta_Mbin_correction_loose_noUPC; // note : no ultra peripheral collision bin
        TH2F * input_eta_Mbin_correction_tight;
        TH2F * input_eta_Mbin_correction_loose;


        vector<TH1F *> dNdeta_1D_MC;
        vector<TH1F *> dNdeta_1D_fulleta_MC;
        vector<TH1F *> dNdeta_1D_reco_single_original;
        vector<TH1F *> dNdeta_1D_reco_multi_original;
        vector<TH1F *> dNdeta_1D_reco_single_postalpha;
        vector<TH1F *> dNdeta_1D_reco_multi_postalpha;
        

        map<string, THStack *> DeltaPhi_Multi_Stack;
        vector<TH1F *> DeltaPhi_Multi_Stack_hist_out;
        map<string, THStack *> DeltaPhi_Multi_Stack_radian;
        vector<TH1F *> DeltaPhi_Multi_Stack_hist_out_radian;
        map<string,TH1F *> DeltaPhi_Multi_1D_radian;

        map<string, double> SignalNTrack_Single;
        map<string, double> SignalNTrack_Multi;
        vector<vector<double>> N_event_counting;
        vector<vector<int>> N_event_counting_MC;
        vector<vector<int>> N_event_counting_fulleta_MC;

        TLegend * legend;
        TLine * solid_line;
        TH1F * temp_hist;

        int post_alpha_tag;

        double radian_conversion = 180./M_PI;

        // TFile * file_out; 

        void ReadFileHist();
        void InitHist();
        void MainPreparation();
        void DrawCoordLine(TH2F * hist_in);
        void DrawEtaCoverBox(TH2F * hist_in, string unit_string);
        void HistDivision();
        void Hist1DSetting(TH1F * hist_in, string color_code);
        

};

string EtaDistReader::Get_eta_region_txt(int bin_x_id)
{
    return Form("%.2f ~ %.2f",eta_z_ref->GetXaxis()->GetBinLowEdge(bin_x_id), eta_z_ref->GetXaxis()->GetBinLowEdge(bin_x_id) + eta_z_ref->GetXaxis()->GetBinWidth(bin_x_id));
}

void EtaDistReader::DrawCoordLine(TH2F * hist_in)
{
    // note : draw the vertical line, to segment the eta region
    for (int i = 0; i < hist_in -> GetNbinsX(); i++) { 
        coord_line -> DrawLine(
            hist_in->GetXaxis()->GetBinLowEdge(i+1), 
            hist_in->GetYaxis()->GetBinLowEdge(1), 
            hist_in->GetXaxis()->GetBinLowEdge(i+1), 
            hist_in->GetYaxis()->GetBinLowEdge(hist_in -> GetNbinsY()) + hist_in->GetYaxis()->GetBinWidth(hist_in -> GetNbinsY())
        ); 
    }
    coord_line -> DrawLine(
        hist_in->GetXaxis()->GetBinLowEdge(hist_in->GetNbinsX()) + hist_in->GetXaxis()->GetBinWidth(hist_in->GetNbinsX()), 
        hist_in->GetYaxis()->GetBinLowEdge(1), 
        hist_in->GetXaxis()->GetBinLowEdge(hist_in->GetNbinsX()) + hist_in->GetXaxis()->GetBinWidth(hist_in->GetNbinsX()), 
        hist_in->GetYaxis()->GetBinLowEdge(hist_in -> GetNbinsY()) + hist_in->GetYaxis()->GetBinWidth(hist_in -> GetNbinsY())
    );

    // note : draw the horizontal line, to segment the z region
    for (int i = 0; i < hist_in -> GetNbinsY(); i++) { 
        coord_line -> DrawLine(
            hist_in->GetXaxis()->GetBinLowEdge(1), 
            hist_in->GetYaxis()->GetBinLowEdge(i+1), 
            hist_in->GetXaxis()->GetBinLowEdge(hist_in->GetNbinsX()) + hist_in->GetXaxis()->GetBinWidth(hist_in->GetNbinsX()), 
            hist_in->GetYaxis()->GetBinLowEdge(i+1)
        ); 
    }
    coord_line -> DrawLine(
        hist_in->GetXaxis()->GetBinLowEdge(1), 
        hist_in->GetYaxis()->GetBinLowEdge(hist_in -> GetNbinsY()) + hist_in->GetYaxis()->GetBinWidth(hist_in -> GetNbinsY()), 
        hist_in->GetXaxis()->GetBinLowEdge(hist_in->GetNbinsX()) + hist_in->GetXaxis()->GetBinWidth(hist_in->GetNbinsX()), 
        hist_in->GetYaxis()->GetBinLowEdge(hist_in -> GetNbinsY()) + hist_in->GetYaxis()->GetBinWidth(hist_in -> GetNbinsY())
    ); 
}

void EtaDistReader::DrawEtaCoverBox(TH2F * hist_in, string unit_string)
{
    double unit_correction;
    if (unit_string == "mm") { unit_correction = 1.; }
    else if (unit_string == "cm") { unit_correction = 0.1; }
    else {cout<<"In EtaDistReader, wrong unit_string input"<<endl;}

    double INTT_layer_R[4] = {
        71.88 * unit_correction, 
        77.32 * unit_correction, 
        96.8 * unit_correction, 
        102.62 * unit_correction
    }; // note : the radii of the INTT layers

    double INTT_left_edge = -230.; INTT_left_edge = INTT_left_edge * unit_correction;
    double INTT_right_edge = 230.; INTT_right_edge = INTT_right_edge * unit_correction;

    // note : low  Z edge for left  eta
    // note : high Z edge for right eta
    for (int i = 0; i < hist_in -> GetNbinsY(); i++)
    {
        double bin_low_zvtx  = hist_in->GetYaxis()->GetBinLowEdge(i+1);
        double bin_high_zvtx = hist_in->GetYaxis()->GetBinLowEdge(i+1) + hist_in->GetYaxis()->GetBinWidth(i+1);

        double INTT_eta_acceptance_l = -0.5 * TMath::Log((sqrt(pow(INTT_left_edge - bin_low_zvtx,2)+pow(INTT_layer_R[0],2))-(INTT_left_edge - bin_low_zvtx)) / (sqrt(pow(INTT_left_edge - bin_low_zvtx,2)+pow(INTT_layer_R[0],2))+(INTT_left_edge - bin_low_zvtx))); // note : left
        double INTT_eta_acceptance_r = -0.5 * TMath::Log((sqrt(pow(INTT_right_edge - bin_high_zvtx,2)+pow(INTT_layer_R[3],2))-(INTT_right_edge - bin_high_zvtx)) / (sqrt(pow(INTT_right_edge - bin_high_zvtx,2)+pow(INTT_layer_R[3],2))+(INTT_right_edge - bin_high_zvtx))); // note : right
        
        solid_line -> DrawLine(INTT_eta_acceptance_l, bin_low_zvtx, INTT_eta_acceptance_l, bin_high_zvtx); // note : vertical
        solid_line -> DrawLine(INTT_eta_acceptance_r, bin_low_zvtx, INTT_eta_acceptance_r, bin_high_zvtx); // note : vertical
        solid_line -> DrawLine(INTT_eta_acceptance_l, bin_low_zvtx, INTT_eta_acceptance_r, bin_low_zvtx); // note : horizontal
        solid_line -> DrawLine(INTT_eta_acceptance_l, bin_high_zvtx, INTT_eta_acceptance_r, bin_high_zvtx); // note : horizontal
        
        cout<<"bin Yaxis ID: "<< i+1 <<" low_edge "<<bin_low_zvtx<<" high edge: "<<bin_high_zvtx<<" eta region: "<<INTT_eta_acceptance_l<<"~"<<INTT_eta_acceptance_r<<endl;   
    }
}

void EtaDistReader::ReadFileHist()
{

    // note : the histograms of the number of tracklets in the signal region for both methods
    // note : the 1D vector is for different centrality bins
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        // note : the entries with the deltaphi within 1 degree, which means the background entries are also included. 
        // note : the loose method
        SignalNTrack_eta_z_Multi_2D.push_back((TH2F *) file_in -> Get(Form("Reco_SignalNTracklet_Multi_MBin%i",Mbin)));
        // note : the tight method
        SignalNTrack_eta_z_Single_2D.push_back((TH2F *) file_in -> Get(Form("Reco_SignalNTracklet_Single_MBin%i",Mbin)));
    }

    // note : the 1D histograms of the DeltaPhi distributions of each centrality-eta-z bin (the multi method, for the background subtraction)
    // note : the format of the key is "MBin_EtaBin_ZBin"
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++) // note : centrality bin
    {
        for (auto eta_z : included_eta_z_map) // note : eta bin
        {
            for(int zbin = 0; zbin < eta_z.second.size(); zbin++) // note :  Z bin
            {
                DeltaPhi_Multi_1D[Form("%i_%i_%i",Mbin,eta_z.first,eta_z.second[zbin])] = (TH1F *) file_in -> Get(Form("Reco_DeltaPhi1D_Multi_MBin%i_Eta%i_Z%i",Mbin,eta_z.first,eta_z.second[zbin]));
                DeltaPhi_Multi_1D[Form("%i_%i_%i",Mbin,eta_z.first,eta_z.second[zbin])] -> SetFillColor(TColor::GetColor(color_code[zbin%5].c_str()));

                // note : change the unit, for the presentation
                DeltaPhi_Multi_1D_radian[Form("%i_%i_%i",Mbin,eta_z.first,eta_z.second[zbin])] = (TH1F *) file_in -> Get(Form("Reco_DeltaPhi1D_Multi_MBin%i_Eta%i_Z%i",Mbin,eta_z.first,eta_z.second[zbin]));
                DeltaPhi_Multi_1D_radian[Form("%i_%i_%i",Mbin,eta_z.first,eta_z.second[zbin])] -> SetFillColor(TColor::GetColor(color_code[zbin%5].c_str()));
                DeltaPhi_Multi_1D_radian[Form("%i_%i_%i",Mbin,eta_z.first,eta_z.second[zbin])] -> GetXaxis() -> SetLimits(
                    DeltaPhi_Multi_1D_radian[Form("%i_%i_%i",Mbin,eta_z.first,eta_z.second[zbin])] -> GetXaxis() -> GetXmin() / radian_conversion,
                    DeltaPhi_Multi_1D_radian[Form("%i_%i_%i",Mbin,eta_z.first,eta_z.second[zbin])] -> GetXaxis() -> GetXmax() / radian_conversion
                );
            }
        }
    }

    // note : the histograms of the number of True track from the MC
    // note : the 1D vector is for different centrality bins
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        TrueNtrack_eta_z_MC_2D.push_back((TH2F *) file_in -> Get(Form("NTrueTrack_MBin%i",Mbin)));
    }

    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        TrueNtrack_eta_z_fulleta_MC_2D.push_back((TH2F *) file_in -> Get(Form("NTrueTrack_FullEta_MBin%i",Mbin)));
    }

    // note : the centrality Z map, to keep the N event counting
    string centrality_Z_map_name = (centrality_Z_map_bool == 0) ? "MBin_Z_evt_map" : "Z_MBin_evt_map"; // todo : the correct name should be "MBin_Z_evt_map", correct next time
    centrality_Z_map    = (TH2F *) file_in -> Get(centrality_Z_map_name.c_str());
    centrality_Z_map_MC = (TH2F *) file_in -> Get((centrality_Z_map_name+"_MC").c_str()); 

    // note : the reference map of Eta-Z 2D histogram
    eta_z_ref = (TH2F *) file_in -> Get("Eta_Z_reference");

    exclusive_cluster_inner_eta_phi_2D = (TH2F *) file_in -> Get("exclusive_cluster_inner_eta_phi_2D");
    exclusive_cluster_outer_eta_phi_2D = (TH2F *) file_in -> Get("exclusive_cluster_outer_eta_phi_2D");
    exclusive_cluster_inner_eta_adc_2D = (TH2F *) file_in -> Get("exclusive_cluster_inner_eta_adc_2D");
    exclusive_cluster_outer_eta_adc_2D = (TH2F *) file_in -> Get("exclusive_cluster_outer_eta_adc_2D");
    
    exclusive_cluster_inner_eta_adc_2D_bkgrm = (TH2F *) file_in -> Get("exclusive_cluster_inner_eta_adc_2D");
    // TH2F_threshold_advanced_2(exclusive_cluster_inner_eta_adc_2D_bkgrm, 0.5);
    vector<double> hist_row_content_inner = SumTH2FColumnContent_row(exclusive_cluster_inner_eta_adc_2D_bkgrm);
    
    exclusive_cluster_inner_eta_adc_2D_bkgrm_profile = (TProfile *) exclusive_cluster_inner_eta_adc_2D_bkgrm -> ProfileY("exclusive_cluster_inner_eta_adc_2D_bkgrm_profile");
    exclusive_cluster_inner_eta_adc_2D_bkgrm_profile_graph = new TGraphErrors(); 
    int point_index_inner = 0;
    for (int i = 0; i < hist_row_content_inner.size(); i++){
        if (hist_row_content_inner[i] < 5){continue;} // note : in order to remove some remaining background
        exclusive_cluster_inner_eta_adc_2D_bkgrm_profile_graph -> SetPoint(
            point_index_inner, 
            exclusive_cluster_inner_eta_adc_2D_bkgrm_profile->GetBinContent(i+1), 
            exclusive_cluster_inner_eta_adc_2D_bkgrm_profile->GetBinCenter(i+1)
        );
        
        exclusive_cluster_inner_eta_adc_2D_bkgrm_profile_graph -> SetPointError(
            point_index_inner, 
            exclusive_cluster_inner_eta_adc_2D_bkgrm_profile->GetBinError(i+1), 
            exclusive_cluster_inner_eta_adc_2D_bkgrm_profile->GetBinWidth(i+1)/2.
        );
        
        point_index_inner += 1;   
    }

    exclusive_cluster_outer_eta_adc_2D_bkgrm = (TH2F *) file_in -> Get("exclusive_cluster_outer_eta_adc_2D");
    // TH2F_threshold_advanced_2(exclusive_cluster_outer_eta_adc_2D_bkgrm, 0.5);
    vector<double> hist_row_content_outer = SumTH2FColumnContent_row(exclusive_cluster_outer_eta_adc_2D_bkgrm);
    
    exclusive_cluster_outer_eta_adc_2D_bkgrm_profile = (TProfile *) exclusive_cluster_outer_eta_adc_2D_bkgrm -> ProfileY("exclusive_cluster_outer_eta_adc_2D_bkgrm_profile");
    exclusive_cluster_outer_eta_adc_2D_bkgrm_profile_graph = new TGraphErrors(); 
    int point_index_outer = 0;
    for (int i = 0; i < hist_row_content_outer.size(); i++){
        if (hist_row_content_outer[i] < 5){continue;} // note : in order to remove some remaining background
        exclusive_cluster_outer_eta_adc_2D_bkgrm_profile_graph -> SetPoint(
            point_index_outer, 
            exclusive_cluster_outer_eta_adc_2D_bkgrm_profile->GetBinContent(i+1), 
            exclusive_cluster_outer_eta_adc_2D_bkgrm_profile->GetBinCenter(i+1)
        );
        exclusive_cluster_outer_eta_adc_2D_bkgrm_profile_graph -> SetPointError(
            point_index_outer, 
            exclusive_cluster_outer_eta_adc_2D_bkgrm_profile->GetBinError(i+1), 
            exclusive_cluster_outer_eta_adc_2D_bkgrm_profile->GetBinWidth(i+1)/2.
        );
        
        point_index_outer += 1;   
    }

    

    exclusive_cluster_inner_adc = (TH1F *) file_in -> Get("exclusive_cluster_inner_adc");
    exclusive_cluster_outer_adc = (TH1F *) file_in -> Get("exclusive_cluster_outer_adc");

    eta_z_ref_used_check = (TH2F*) eta_z_ref -> Clone("eta_z_ref_used_check");
    eta_z_ref_used_check_binID = (TH2F*) eta_z_ref -> Clone("eta_z_ref_used_check");
    
    // todo, the unit_correction is hard written here, the 0.1, I meant. 
    eta_z_ref_cm = new TH2F (
        "",
        "eta_z_ref_cm;reco #eta;reco zvtx [cm]",
        eta_z_ref -> GetNbinsX(),
        eta_z_ref -> GetXaxis() -> GetXmin(),
        eta_z_ref -> GetXaxis() -> GetXmax(),
        eta_z_ref -> GetNbinsY(),
        eta_z_ref -> GetYaxis() -> GetXmin() * 0.1,
        eta_z_ref -> GetYaxis() -> GetXmax() * 0.1
    );
    eta_z_ref_used_check_cm = (TH2F *) eta_z_ref_cm -> Clone("eta_z_ref_used_check_cm");
    eta_z_ref_used_check_binID_cm = (TH2F *) eta_z_ref_cm -> Clone("eta_z_ref_used_check_binID_cm");

    cout<<"ReadFileHist done"<<endl;

    return;

}

void EtaDistReader::FindCoveredRegion()
{
    for (int eta_bin = 0; eta_bin < eta_z_ref_used_check_binID->GetNbinsX(); eta_bin++)
    {
        for (int z_bin = 0; z_bin < eta_z_ref_used_check_binID->GetNbinsY(); z_bin++)
        {
            eta_z_ref_used_check_binID    -> SetBinContent(eta_bin+1,z_bin+1, 100 * (eta_bin+1) + (z_bin+1));
            eta_z_ref_used_check_binID_cm -> SetBinContent(eta_bin+1,z_bin+1, 100 * (eta_bin+1) + (z_bin+1));
        }
    }

    c1 -> cd();
    eta_z_ref -> GetXaxis() -> SetTitle("reco #eta");
    eta_z_ref -> GetYaxis() -> SetTitle("reco zvtx [mm]");
    eta_z_ref -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    DrawCoordLine(eta_z_ref);    
    DrawEtaCoverBox(eta_z_ref, "mm");
    c1 -> Print(Form("%s/eta_z_ref.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : for the used bins check
    c1 -> cd();
    double eta_z_ref_used_check_max_entry = eta_z_ref_used_check->GetMaximum();
    for (auto eta_z : included_eta_z_map){
        for (auto zbin : eta_z.second){
            eta_z_ref_used_check -> SetBinContent(eta_z.first, zbin, eta_z_ref_used_check_max_entry * 1000);        
        }
    }
    eta_z_ref_used_check -> GetXaxis() -> SetTitle("reco #eta");
    eta_z_ref_used_check -> GetYaxis() -> SetTitle("reco zvtx [mm]");
    eta_z_ref_used_check -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    DrawCoordLine(eta_z_ref_used_check);    
    DrawEtaCoverBox(eta_z_ref_used_check, "mm");

    gStyle->SetPaintTextFormat("1.0f");
    eta_z_ref_used_check_binID -> SetMarkerSize(0.7);
    eta_z_ref_used_check_binID -> Draw("HIST TEXT45 SAME");

    c1 -> Print(Form("%s/eta_z_ref_used_check.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : unit in cm, for the presentation
    TH2F_FakeClone(eta_z_ref, eta_z_ref_cm);
    TH2F_FakeClone(eta_z_ref_used_check, eta_z_ref_used_check_cm);

    c1 -> cd();
    eta_z_ref_cm -> GetXaxis() -> SetTitle("reco #eta");
    eta_z_ref_cm -> GetYaxis() -> SetTitle("reco zvtx [cm]");
    eta_z_ref_cm -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    DrawCoordLine(eta_z_ref_cm);    
    DrawEtaCoverBox(eta_z_ref_cm, "cm");
    c1 -> Print(Form("%s/eta_z_ref_cm.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : for the used bins check
    c1 -> cd();
    eta_z_ref_used_check_cm -> GetXaxis() -> SetTitle("reco #eta");
    eta_z_ref_used_check_cm -> GetYaxis() -> SetTitle("reco zvtx [cm]");
    eta_z_ref_used_check_cm -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    DrawCoordLine(eta_z_ref_used_check_cm);    
    DrawEtaCoverBox(eta_z_ref_used_check_cm, "cm");

    gStyle->SetPaintTextFormat("1.0f");
    eta_z_ref_used_check_binID_cm -> SetMarkerSize(0.7);
    eta_z_ref_used_check_binID_cm -> Draw("HIST TEXT45 SAME");

    c1 -> Print(Form("%s/eta_z_ref_used_check_cm.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

}

void EtaDistReader::Hist1DSetting(TH1F * hist_in, string color_code)
{
    hist_in -> SetMarkerStyle(20);
    hist_in -> SetMarkerSize(0.8);
    hist_in -> SetMarkerColor(TColor::GetColor(color_code.c_str()));
    hist_in -> SetLineColor(TColor::GetColor(color_code.c_str()));
    hist_in -> SetLineWidth(2);
    hist_in -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
    hist_in -> GetXaxis() -> SetTitle("#eta");
    // hist_in -> GetYaxis() -> SetRangeUser(0,50);
    hist_in -> SetTitleSize(0.06, "X");
    hist_in -> SetTitleSize(0.06, "Y");
    hist_in -> GetXaxis() -> SetTitleOffset (0.71);
    hist_in -> GetYaxis() -> SetTitleOffset (1.1);
    hist_in -> GetXaxis() -> CenterTitle(true);
    hist_in -> GetYaxis() -> CenterTitle(true);
}

void EtaDistReader::InitHist()
{
    for (int i = 0; i < N_centrality_bin; i++)
    {
        dNdeta_1D_MC.push_back(new TH1F("","",included_eta_z_map.size(), eta_z_ref -> GetXaxis() ->GetBinLowEdge(included_eta_z_map[0].first), eta_z_ref -> GetXaxis() ->GetBinCenter(included_eta_z_map[included_eta_z_map.size() - 1].first) + eta_z_ref -> GetXaxis() ->GetBinWidth(included_eta_z_map[included_eta_z_map.size() - 1].first) / 2.));
        Hist1DSetting(dNdeta_1D_MC[i], "#1A3947");

        dNdeta_1D_fulleta_MC.push_back(
            new TH1F(
                "",
                "",
                eta_region.size() - 1,
                &eta_region[0]
            )
        );
        Hist1DSetting(dNdeta_1D_fulleta_MC[i], "#D8364D");

        dNdeta_1D_reco_single_original.push_back(new TH1F("","",included_eta_z_map.size(), eta_z_ref -> GetXaxis() ->GetBinLowEdge(included_eta_z_map[0].first), eta_z_ref -> GetXaxis() ->GetBinCenter(included_eta_z_map[included_eta_z_map.size() - 1].first) + eta_z_ref -> GetXaxis() ->GetBinWidth(included_eta_z_map[included_eta_z_map.size() - 1].first) / 2.));
        Hist1DSetting(dNdeta_1D_reco_single_original[i], "#1A3947");

        dNdeta_1D_reco_multi_original.push_back(new TH1F("","",included_eta_z_map.size(), eta_z_ref -> GetXaxis() ->GetBinLowEdge(included_eta_z_map[0].first), eta_z_ref -> GetXaxis() ->GetBinCenter(included_eta_z_map[included_eta_z_map.size() - 1].first) + eta_z_ref -> GetXaxis() ->GetBinWidth(included_eta_z_map[included_eta_z_map.size() - 1].first) / 2.));
        Hist1DSetting(dNdeta_1D_reco_multi_original[i], "#c48045");
    }

    eta_Mbin_correction_tight = new TH2F(
        "",
        "eta_Mbin_correction_tight;#eta;Mbin;#alpha factor",
        included_eta_z_map.size(), 
        eta_z_ref -> GetXaxis() ->GetBinLowEdge(included_eta_z_map[0].first), 
        eta_z_ref -> GetXaxis() ->GetBinCenter(included_eta_z_map[included_eta_z_map.size() - 1].first) + eta_z_ref -> GetXaxis() ->GetBinWidth(included_eta_z_map[included_eta_z_map.size() - 1].first) / 2.,
        N_centrality_bin,
        0, 
        N_centrality_bin
    );

    eta_Mbin_correction_loose = new TH2F(
        "",
        "eta_Mbin_correction_loose;#eta;Mbin;#alpha factor",
        included_eta_z_map.size(), 
        eta_z_ref -> GetXaxis() ->GetBinLowEdge(included_eta_z_map[0].first), 
        eta_z_ref -> GetXaxis() ->GetBinCenter(included_eta_z_map[included_eta_z_map.size() - 1].first) + eta_z_ref -> GetXaxis() ->GetBinWidth(included_eta_z_map[included_eta_z_map.size() - 1].first) / 2.,
        N_centrality_bin,
        0, 
        N_centrality_bin
    );

    eta_Mbin_correction_loose_noUPC = new TH2F(
        "",
        "eta_Mbin_correction_loose_noUPC;#eta;Mbin;#alpha factor",
        included_eta_z_map.size(), 
        eta_z_ref -> GetXaxis() ->GetBinLowEdge(included_eta_z_map[0].first), 
        eta_z_ref -> GetXaxis() ->GetBinCenter(included_eta_z_map[included_eta_z_map.size() - 1].first) + eta_z_ref -> GetXaxis() ->GetBinWidth(included_eta_z_map[included_eta_z_map.size() - 1].first) / 2.,
        N_centrality_bin,
        0, 
        N_centrality_bin
    );

    input_eta_Mbin_correction_tight = nullptr;
    input_eta_Mbin_correction_loose = nullptr;

    cout<<"InitHist done"<<endl;
}

// note : to count the number of reco tracklets in the signal region with the selected eta bin and z bin, for each centrality bin
// note : for both methods
// note : to increase the statistic, try to stack the DeltaPhi distributions with same eta bin but different z bin
// note : the key is "MBin_EtaBin"
// note : for each eta bin and each centrality bin, count the number of event. (Each eta bin has different selected z region, so the number of event is different for each eta bin)
void EtaDistReader::MainPreparation()
{
    c1 -> cd();
    c1 -> Print(Form("%s/Reco_DeltaPhi_Multi_Stack.pdf(", out_folder_directory.c_str())); 
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++) // note : centrality bin
    {
        // note : for the MC
        // note: this is fro the full dN/deta distribution in the MC, the full coverage
        for (int eta_i = 0; eta_i < TrueNtrack_eta_z_fulleta_MC_2D[Mbin]->GetNbinsX(); eta_i++)
        {
            for (int z_bin : fulleta_MC_included_z_bin) // note : the bin ID is used.
            {
                dNdeta_1D_fulleta_MC[Mbin]->SetBinContent(eta_i+1, dNdeta_1D_fulleta_MC[Mbin] -> GetBinContent(eta_i+1) + TrueNtrack_eta_z_fulleta_MC_2D[Mbin]->GetBinContent(eta_i+1, z_bin));   
                N_event_counting_fulleta_MC[Mbin][eta_i] += centrality_Z_map_MC -> GetBinContent(Mbin + 1, z_bin);
            }
        }

        // note : for the MC
        //todo: test, which means that, for the true dN/deta distribution, we don't use the selected region only. Instead, we use the full range (kinda directly from the physics)
        //todo: in principle, the two methods should work, but turned out that at the edge bins, the entries are a bit fewer than the expectation. 
        for (int eta_i = 0; eta_i < dNdeta_1D_MC[Mbin]->GetNbinsX(); eta_i++) {
            dNdeta_1D_MC[Mbin] -> SetBinContent(eta_i+1, dNdeta_1D_fulleta_MC[Mbin]->GetBinContent( dNdeta_1D_fulleta_MC[Mbin] -> FindBin(dNdeta_1D_MC[Mbin] -> GetBinCenter(eta_i+1)) ));   
            for (int z_bin : fulleta_MC_included_z_bin){
                //todo: test
                N_event_counting_MC[Mbin][eta_i] += centrality_Z_map_MC -> GetBinContent(Mbin + 1, z_bin);
            }
        }

        // note : only for the data, now 
        for (auto eta_z : included_eta_z_map) // note : eta bin
        {
            DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] = new THStack(Form("DeltaPhi_Multi_Stack_MBin%i_Eta%i",Mbin,eta_z.first),Form("DeltaPhi_Multi_Stack_MBin%i_Eta%i;#Delta#phi (Inner - Outer) [degree];Entry",Mbin,eta_z.first));
            for(int zbin : eta_z.second) // note :  Z bin
            {
                // note : the key to the map: "MBin_EtaBin"
                DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> Add(DeltaPhi_Multi_1D[Form("%i_%i_%i",Mbin,eta_z.first,zbin)]);
                SignalNTrack_Single[Form("%i_%i",Mbin,eta_z.first)] += SignalNTrack_eta_z_Single_2D[Mbin] -> GetBinContent(eta_z.first, zbin); // note : the signal counting for the single method
                SignalNTrack_Multi[Form("%i_%i",Mbin,eta_z.first)]  +=  SignalNTrack_eta_z_Multi_2D[Mbin] -> GetBinContent(eta_z.first, zbin); // note : the signal counting for the multi method

                // note : to count the TrueNTtrack in the true level information
                //todo: test
                // dNdeta_1D_MC[Mbin]->SetBinContent(eta_z.first - eta_correction, TrueNtrack_eta_z_MC_2D[Mbin] -> GetBinContent(eta_z.first, zbin) + dNdeta_1D_MC[Mbin]->GetBinContent(eta_z.first - eta_correction));

                N_event_counting[Mbin][eta_z.first - eta_correction - 1]    += centrality_Z_map -> GetBinContent(Mbin + 1, zbin);
                //todo: test
                // N_event_counting_MC[Mbin][eta_z.first - eta_correction - 1] += centrality_Z_map_MC -> GetBinContent(Mbin + 1, zbin);
            }

            // note : Background fit on the DeltaPhi distributions, to know the background level
            // note : print the DeltaPhi distributions for each centrality bin adn each eta bin, after stacking
            temp_hist = (TH1F *) DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> GetStack() -> Last();
            
            DeltaPhi_Multi_Stack_hist_out.push_back(temp_hist);
            DeltaPhi_Multi_Stack_hist_out[DeltaPhi_Multi_Stack_hist_out.size() - 1] -> SetTitle(Form("MBin%i_EtaBin%i",Mbin,eta_z.first));

            double hist_offset = get_dist_offset(temp_hist, 15);

            bkg_fit_pol2 -> SetParameters(hist_offset, 0, -0.2, 0, signal_region);
            bkg_fit_pol2 -> FixParameter(4, signal_region);
            bkg_fit_pol2 -> SetParLimits(2, -100, 0);
            temp_hist -> Fit(bkg_fit_pol2,"NQ");
            // note : extract the background region (which includes the signal region also)
            draw_pol2_line -> SetParameters(bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1), bkg_fit_pol2 -> GetParameter(2), bkg_fit_pol2 -> GetParameter(3));

            DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> SetMinimum(0);
            DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> SetMaximum( temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
            DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> Draw(); 

            draw_pol2_line -> Draw("lsame");
            
            double pol2_bkg_integral = fabs(draw_pol2_line -> Integral( -1. * signal_region, signal_region )) / temp_hist -> GetBinWidth(1);
            // cout<<i<<" "<<i1<<" pol2_bkg integral: "<<pol2_bkg_integral<<endl;

            // note : for the multi method, count the number of tracklets after the back subtraction
            dNdeta_1D_reco_multi_original[Mbin]  -> SetBinContent(eta_z.first - eta_correction, SignalNTrack_Multi[Form("%i_%i",Mbin,eta_z.first)] - pol2_bkg_integral);
            dNdeta_1D_reco_single_original[Mbin] -> SetBinContent(eta_z.first - eta_correction, SignalNTrack_Single[Form("%i_%i",Mbin,eta_z.first)]);

            coord_line -> DrawLine(-1*signal_region, 0, -1 * signal_region, temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
            coord_line -> DrawLine(signal_region, 0, signal_region, temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
            
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
            
            draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s, #eta: %.2f ~ %.2f",centrality_region[Mbin].c_str(), 
                eta_z_ref -> GetXaxis() -> GetBinCenter(eta_z.first) - eta_z_ref -> GetXaxis() -> GetBinWidth(eta_z.first)/2., 
                eta_z_ref -> GetXaxis() -> GetBinCenter(eta_z.first) + eta_z_ref -> GetXaxis() -> GetBinWidth(eta_z.first)/2.)
            );
            
            draw_text -> DrawLatex(0.21, 0.86, Form("MBin: %i, #eta_bin: %i, #Delta#Phi_bin width: %.2f", Mbin, eta_z.first, temp_hist -> GetBinWidth(1)));
            draw_text -> DrawLatex(0.21, 0.82, Form("pol2: %.2f + %.2f(x-%.2f) + %.2f(x-%.2f)^{2}", bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1), bkg_fit_pol2 -> GetParameter(3), bkg_fit_pol2 -> GetParameter(2), bkg_fit_pol2 -> GetParameter(3)));
            draw_text -> DrawLatex(0.21, 0.78, Form("Signal size: %.3f, pol2 bkg size: %.2f", SignalNTrack_Multi[Form("%i_%i",Mbin,eta_z.first)], pol2_bkg_integral));

            c1 -> Print(Form("%s/Reco_DeltaPhi_Multi_Stack.pdf", out_folder_directory.c_str()));
            c1 -> Clear();


            // note : ============================================================================================================================================================================================================
            // note : ============================================================================================================================================================================================================
            // note : ============================================================================================================================================================================================================
            // note : ============================================================================================================================================================================================================
            // note : change the unit for the presentation
            DeltaPhi_Multi_Stack_radian[Form("%i_%i",Mbin,eta_z.first)] = new THStack(Form("DeltaPhi_Multi_Stack_MBin%i_Eta%i",Mbin,eta_z.first),Form("DeltaPhi_Multi_Stack_MBin%i_Eta%i;#Delta#phi (Inner - Outer) [radian];Entry",Mbin,eta_z.first));
            for(int zbin : eta_z.second) { // note :  Z bin
                // note : the key to the map: "MBin_EtaBin"
                DeltaPhi_Multi_Stack_radian[Form("%i_%i",Mbin,eta_z.first)] -> Add(DeltaPhi_Multi_1D_radian[Form("%i_%i_%i",Mbin,eta_z.first,zbin)]);
            }

            // note : Background fit on the DeltaPhi distributions, to know the background level
            // note : print the DeltaPhi distributions for each centrality bin adn each eta bin, after stacking
            temp_hist = (TH1F *) DeltaPhi_Multi_Stack_radian[Form("%i_%i",Mbin,eta_z.first)] -> GetStack() -> Last();
            
            DeltaPhi_Multi_Stack_hist_out_radian.push_back(temp_hist);
            DeltaPhi_Multi_Stack_hist_out_radian[DeltaPhi_Multi_Stack_hist_out_radian.size() - 1] -> SetTitle(Form("MBin%i_EtaBin%i",Mbin,eta_z.first));

            // note : p[0] + p[1]*(x-p[3])+p[2] * (x-p[3])^2
            draw_pol2_line -> SetParameters(bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1) * radian_conversion, bkg_fit_pol2 -> GetParameter(2) * pow(radian_conversion,2), bkg_fit_pol2 -> GetParameter(3)/radian_conversion);

            DeltaPhi_Multi_Stack_radian[Form("%i_%i",Mbin,eta_z.first)] -> SetMinimum(0);
            DeltaPhi_Multi_Stack_radian[Form("%i_%i",Mbin,eta_z.first)] -> SetMaximum( temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
            DeltaPhi_Multi_Stack_radian[Form("%i_%i",Mbin,eta_z.first)] -> Draw(); 

            draw_pol2_line -> Draw("lsame");

            coord_line -> DrawLine(-1 * signal_region / radian_conversion, 0, -1 * signal_region / radian_conversion, temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
            coord_line -> DrawLine(     signal_region / radian_conversion, 0,      signal_region / radian_conversion, temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
            
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
            
            draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s, #eta: %.2f ~ %.2f",centrality_region[Mbin].c_str(), 
                eta_z_ref -> GetXaxis() -> GetBinCenter(eta_z.first) - eta_z_ref -> GetXaxis() -> GetBinWidth(eta_z.first)/2., 
                eta_z_ref -> GetXaxis() -> GetBinCenter(eta_z.first) + eta_z_ref -> GetXaxis() -> GetBinWidth(eta_z.first)/2.)
            );
            
            draw_text -> DrawLatex(0.21, 0.86, Form("#Delta#Phi_bin width: %.5f", temp_hist -> GetBinWidth(1)));
            draw_text -> DrawLatex(0.21, 0.82, Form("pol2: %.2f + %.2f(x-%.2f) + %.2f(x-%.2f)^{2}", draw_pol2_line -> GetParameter(0), draw_pol2_line -> GetParameter(1), draw_pol2_line -> GetParameter(3), draw_pol2_line -> GetParameter(2), draw_pol2_line -> GetParameter(3)));

            c1 -> Print(Form("%s/Reco_DeltaPhi_Multi_Stack.pdf", out_folder_directory.c_str()));
            c1 -> Clear();
        }
    }
    c1 -> Print(Form("%s/Reco_DeltaPhi_Multi_Stack.pdf)", out_folder_directory.c_str()));
    c1 -> Clear();

    cout<<"MainPreparation done"<<endl;
    return;
}

void EtaDistReader::HistDivision()
{
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {   
        dNdeta_1D_MC[Mbin]          -> Scale(1./double(dNdeta_1D_MC[Mbin]          -> GetBinWidth(1)));
        dNdeta_1D_fulleta_MC[Mbin]  -> Scale(1./double(dNdeta_1D_fulleta_MC[Mbin]  -> GetBinWidth(1)));
        dNdeta_1D_reco_single_original[Mbin] -> Scale(1./double(dNdeta_1D_reco_single_original[Mbin] -> GetBinWidth(1)));
        dNdeta_1D_reco_multi_original[Mbin]  -> Scale(1./double(dNdeta_1D_reco_multi_original[Mbin]  -> GetBinWidth(1)));

        // note : only for the fulleta region
        for (int i = 0; i < dNdeta_1D_fulleta_MC[Mbin] -> GetNbinsX(); i++)
        {
            dNdeta_1D_fulleta_MC[Mbin] -> SetBinContent(i+1, dNdeta_1D_fulleta_MC[Mbin] -> GetBinContent(i+1) / double(N_event_counting_fulleta_MC[Mbin][i]));
            dNdeta_1D_fulleta_MC[Mbin] -> SetBinError(i+1, dNdeta_1D_fulleta_MC[Mbin] -> GetBinError(i+1) / double(N_event_counting_fulleta_MC[Mbin][i]));
        }

        for (int i = 0; i < included_eta_z_map.size(); i++)
        {
            dNdeta_1D_MC[Mbin] -> SetBinContent(i+1, dNdeta_1D_MC[Mbin] -> GetBinContent(i+1) / double(N_event_counting_MC[Mbin][i]));
            dNdeta_1D_MC[Mbin] -> SetBinError(i+1, dNdeta_1D_MC[Mbin] -> GetBinError(i+1) / double(N_event_counting_MC[Mbin][i]));

            dNdeta_1D_reco_single_original[Mbin] -> SetBinContent(i+1, dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(i+1) / double(N_event_counting[Mbin][i]));
            dNdeta_1D_reco_single_original[Mbin] -> SetBinError(i+1, dNdeta_1D_reco_single_original[Mbin] -> GetBinError(i+1) / double(N_event_counting[Mbin][i]));

            dNdeta_1D_reco_multi_original[Mbin]  -> SetBinContent(i+1, dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(i+1) / double(N_event_counting[Mbin][i]));
            dNdeta_1D_reco_multi_original[Mbin]  -> SetBinError(i+1, dNdeta_1D_reco_multi_original[Mbin] -> GetBinError(i+1) / double(N_event_counting[Mbin][i]));

            cout<<"check denominators, Mbin : "<<Mbin<<" eta bin : "<<i+1<<" truth_count: "<<N_event_counting_MC[Mbin][i]<<" reco_count: "<<N_event_counting[Mbin][i]<<endl;

        }

        //note : clone the histogram for the alpha correction
        dNdeta_1D_reco_multi_postalpha[Mbin] = (TH1F *) dNdeta_1D_reco_multi_original[Mbin] -> Clone(Form("dNdeta_1D_reco_multi_postalpha_Mbin%i",Mbin));
        dNdeta_1D_reco_single_postalpha[Mbin] = (TH1F *) dNdeta_1D_reco_single_original[Mbin] -> Clone(Form("dNdeta_1D_reco_single_postalpha_Mbin%i",Mbin));
    }


}

// note : correction_step 0: correction b/w the original distribution and true dist. ---> this is for deriving 
// note : correction_step 1: b/w post_alpha dist. and true dist. ---> this is for the test
void EtaDistReader::DeriveAlphaCorrection(int correction_step)
{
    double MC_hist_counting = 0;
    double data_tight_counting = 0;
    double data_loose_counting = 0;

    vector<TH1F *> temp_hist_single_vec;
    vector<TH1F *> temp_hist_multi_vec;

    if (correction_step == 0) {
        temp_hist_single_vec = dNdeta_1D_reco_single_original;
        temp_hist_multi_vec = dNdeta_1D_reco_multi_original;
    }
    else if (correction_step == 1){
        temp_hist_single_vec = dNdeta_1D_reco_single_postalpha;
        temp_hist_multi_vec = dNdeta_1D_reco_multi_postalpha;
    }
    else {
        std::cout<<"wrong correction_step input !!!"<<std::endl;
        return;
    }

    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {   
        std::cout<<" "<<std::endl;
        
        cout<<"----------- for the case of method tight ----------------" <<endl;
        // note : to print the ratio between reco track and MC track
        std::cout << "centrality bin : "<<Mbin<<", ";
        // note : the eta bin
        for (int bin_i = 0; bin_i < dNdeta_1D_MC[Mbin] -> GetNbinsX(); bin_i++) {
            MC_hist_counting += dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1);
            data_tight_counting += temp_hist_single_vec[Mbin] -> GetBinContent(bin_i+1);
            std::cout <<"~~"<<temp_hist_single_vec[Mbin] -> GetBinContent(bin_i+1) <<", "<< dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)<< ", " << Form("%.3f",temp_hist_single_vec[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)) <<"~~, ";

            eta_Mbin_correction_tight -> SetBinContent(bin_i + 1, Mbin+1, temp_hist_single_vec[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1));
        }
        std::cout << std::endl;
        cout<<"Mbin : "<<Mbin<<" MC_hist_counting: "<<MC_hist_counting<<" data_tight_counting: "<<data_tight_counting<<endl;

        MC_hist_counting = 0;
        data_tight_counting = 0;
    }

    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        cout<<"----------- for the case of method inclusive ----------------" <<endl;
        // note : to print the ratio between reco track and MC track
        std::cout << "centrality bin : "<<Mbin<<", ";
        for (int bin_i = 0; bin_i < dNdeta_1D_MC[Mbin] -> GetNbinsX(); bin_i++) {
            MC_hist_counting += dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1);
            data_loose_counting += temp_hist_multi_vec[Mbin] -> GetBinContent(bin_i+1);
            std::cout <<"~~"<<temp_hist_multi_vec[Mbin] -> GetBinContent(bin_i+1) <<", "<< dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)<< ", " << Form( "%.3f", temp_hist_multi_vec[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)) <<"~~, ";

            eta_Mbin_correction_loose       -> SetBinContent(bin_i + 1, Mbin + 1, temp_hist_multi_vec[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1));
            eta_Mbin_correction_loose_noUPC -> SetBinContent(bin_i + 1, Mbin + 1, temp_hist_multi_vec[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1));
        }
        std::cout << std::endl;
        cout<<"Mbin : "<<Mbin<<" MC_hist_counting: "<<MC_hist_counting<<" data_loose_counting: "<<data_loose_counting<<endl;

        MC_hist_counting = 0;
        data_loose_counting = 0;
    }
}

vector<TH2F *> EtaDistReader::Get_alpha_corr_map() {return {eta_Mbin_correction_loose, eta_Mbin_correction_tight};}

void EtaDistReader::Set_alpha_corr_map(TH2F * hist_in_loose, TH2F * hist_in_tight)
{
    input_eta_Mbin_correction_loose = hist_in_loose;
    input_eta_Mbin_correction_tight = hist_in_tight;

    std::cout<<"in EtaDistReader, input map loose check bin (3,3): "<<input_eta_Mbin_correction_loose -> GetBinContent(3,3)<<endl;
    std::cout<<"in EtaDistReader, input map tight check bin (3,3): "<<input_eta_Mbin_correction_tight -> GetBinContent(3,3)<<endl;

    return;
}

void EtaDistReader::ApplyAlphaCorrection()
{
    if (input_eta_Mbin_correction_loose == nullptr || input_eta_Mbin_correction_tight == nullptr)
    {
        std::cout<<" no alpha correction map included !!! "<<endl;
        return;
    }

    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {   
        for (int i = 0; i < included_eta_z_map.size(); i++)
        {        
            // note : for the loose method 
            dNdeta_1D_reco_multi_postalpha[Mbin] -> SetBinContent(i+1, dNdeta_1D_reco_multi_postalpha[Mbin] -> GetBinContent(i+1) / input_eta_Mbin_correction_loose -> GetBinContent(i+1, Mbin+1));
            dNdeta_1D_reco_multi_postalpha[Mbin] -> SetBinError(i+1,   dNdeta_1D_reco_multi_postalpha[Mbin] -> GetBinError(i+1)   /  input_eta_Mbin_correction_loose -> GetBinContent(i+1, Mbin+1));
            
            // note : for the tight method 
            dNdeta_1D_reco_single_postalpha[Mbin] -> SetBinContent(i+1, dNdeta_1D_reco_single_postalpha[Mbin] -> GetBinContent(i+1) / input_eta_Mbin_correction_tight -> GetBinContent(i+1, Mbin+1));
            dNdeta_1D_reco_single_postalpha[Mbin] -> SetBinError(i+1,   dNdeta_1D_reco_single_postalpha[Mbin] -> GetBinError(i+1)   / input_eta_Mbin_correction_tight -> GetBinContent(i+1, Mbin+1));
        }
    }

    post_alpha_tag = 1;

    std::cout<<"finish the alpha correction application"<<std::endl;

    return;

}

void EtaDistReader::PrintPlots_exclusive()
{
    c1 ->cd();
    exclusive_cluster_inner_adc -> SetMinimum(0);
    exclusive_cluster_inner_adc -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/exclusive_cluster_inner_adc.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 ->cd();
    exclusive_cluster_outer_adc -> SetMinimum(0);
    exclusive_cluster_outer_adc -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/exclusive_cluster_outer_adc.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 ->cd();
    exclusive_cluster_inner_eta_phi_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/exclusive_cluster_inner_eta_phi_2D.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 ->cd();
    exclusive_cluster_outer_eta_phi_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/exclusive_cluster_outer_eta_phi_2D.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 ->cd();
    exclusive_cluster_inner_eta_adc_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/exclusive_cluster_inner_eta_adc_2D.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 ->cd();
    exclusive_cluster_outer_eta_adc_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/exclusive_cluster_outer_eta_adc_2D.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 ->cd();
    exclusive_cluster_inner_eta_adc_2D_bkgrm -> Draw("colz0");
    exclusive_cluster_inner_eta_adc_2D_bkgrm_profile_graph -> Draw("p same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/exclusive_cluster_inner_eta_adc_2D_bkgrm.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 ->cd();
    exclusive_cluster_outer_eta_adc_2D_bkgrm -> Draw("colz0");
    exclusive_cluster_outer_eta_adc_2D_bkgrm_profile_graph -> Draw("p same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/exclusive_cluster_outer_eta_adc_2D_bkgrm.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

}

void EtaDistReader::PrintPlots()
{
    gStyle->SetPaintTextFormat("1.3f");

    c1 -> cd();
    eta_Mbin_correction_loose -> Draw("colz0");
    eta_Mbin_correction_loose -> SetMarkerSize(0.7);
    eta_Mbin_correction_loose -> Draw("HIST TEXT45 SAME");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/eta_Mbin_correction_loose.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    for (int i = 0; i < eta_Mbin_correction_loose_noUPC -> GetNbinsX(); i++) {eta_Mbin_correction_loose_noUPC -> SetBinContent(i+1, eta_Mbin_correction_loose_noUPC -> GetNbinsY()-1, 0);}
    eta_Mbin_correction_loose_noUPC -> Draw("colz0");
    eta_Mbin_correction_loose_noUPC -> SetMarkerSize(0.7);
    eta_Mbin_correction_loose_noUPC -> Draw("HIST TEXT45 SAME");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/eta_Mbin_correction_loose_noUPC.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    eta_Mbin_correction_tight -> Draw("colz0");
    eta_Mbin_correction_tight -> SetMarkerSize(0.7);
    eta_Mbin_correction_tight -> Draw("HIST TEXT45 SAME");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/eta_Mbin_correction_tight.pdf", out_folder_directory.c_str()));
    c1 -> Clear();


    c1 -> cd();
    c1 -> Print(Form("%s/dNdeta_combine_final_original.pdf(", out_folder_directory.c_str()));
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {   
        if (Mbin == 0)
        {
            legend -> AddEntry(dNdeta_1D_fulleta_MC[Mbin], "Truth dist. (full)","f");
            legend -> AddEntry(dNdeta_1D_MC[Mbin], "Truth dist. (selected)","f");
            legend -> AddEntry(dNdeta_1D_reco_single_original[Mbin], "Reco. Method tight","lep");
            legend -> AddEntry(dNdeta_1D_reco_multi_original[Mbin], "Reco. Method loose","lep");
        }

        // todo: this is for the test, to full the fulleta dNdeta together with others
        dNdeta_1D_fulleta_MC[Mbin] -> SetMinimum(0);
        dNdeta_1D_fulleta_MC[Mbin] -> SetMaximum( dNdeta_1D_fulleta_MC[Mbin] -> GetBinContent( dNdeta_1D_fulleta_MC[Mbin] -> GetMaximumBin() ) * 1.5);
        dNdeta_1D_fulleta_MC[Mbin] -> Draw("hist");

        // dNdeta_1D_MC[Mbin] -> GetYaxis() -> SetRangeUser(0, dNdeta_1D_MC[Mbin] -> GetMaximum() * 1.5);

        dNdeta_1D_MC[Mbin] -> Draw("hist same");
        dNdeta_1D_reco_single_original[Mbin] -> Draw("p same");
        dNdeta_1D_reco_multi_original[Mbin]  -> Draw("p same");

        legend -> Draw("same");

        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[Mbin].c_str()));
       
        c1 -> Print(Form("%s/dNdeta_combine_final_original.pdf", out_folder_directory.c_str()));
        c1 -> Clear();
    }
    c1 -> Print(Form("%s/dNdeta_combine_final_original.pdf)", out_folder_directory.c_str()));
    c1 -> Clear();
    legend -> Clear();

    

    c1 -> cd();
    c1 -> Print(Form("%s/dNdeta_combine_final_postalpha.pdf(", out_folder_directory.c_str()));
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {   
        if (Mbin == 0)
        {
            legend -> AddEntry(dNdeta_1D_fulleta_MC[Mbin], "Truth dist. (full)","f");
            legend -> AddEntry(dNdeta_1D_MC[Mbin], "Truth dist. (selected)","f");
            legend -> AddEntry(dNdeta_1D_reco_single_postalpha[Mbin], "Reco. Method tight","lep");
            legend -> AddEntry(dNdeta_1D_reco_multi_postalpha[Mbin], "Reco. Method loose","lep");
        }

        // todo: this is for the test, to full the fulleta dNdeta together with others
        dNdeta_1D_fulleta_MC[Mbin] -> SetMinimum(0);
        dNdeta_1D_fulleta_MC[Mbin] -> SetMaximum( dNdeta_1D_fulleta_MC[Mbin] -> GetBinContent( dNdeta_1D_fulleta_MC[Mbin] -> GetMaximumBin() ) * 1.5);
        dNdeta_1D_fulleta_MC[Mbin] -> Draw("hist");

        // dNdeta_1D_MC[Mbin] -> GetYaxis() -> SetRangeUser(0, dNdeta_1D_MC[Mbin] -> GetMaximum() * 1.5);

        dNdeta_1D_MC[Mbin] -> Draw("hist same");
        Hist1DSetting(dNdeta_1D_reco_single_postalpha[Mbin], "#1A3947");
        dNdeta_1D_reco_single_postalpha[Mbin] -> Draw("p same");
        Hist1DSetting(dNdeta_1D_reco_multi_postalpha[Mbin], "#c48045");
        dNdeta_1D_reco_multi_postalpha[Mbin]  -> Draw("p same");

        legend -> Draw("same");

        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[Mbin].c_str()));
       
        c1 -> Print(Form("%s/dNdeta_combine_final_postalpha.pdf", out_folder_directory.c_str()));
        c1 -> Clear();
    }
    c1 -> Print(Form("%s/dNdeta_combine_final_postalpha.pdf)", out_folder_directory.c_str()));
    c1 -> Clear();
    legend -> Clear();


}

void EtaDistReader::EndRun()
{
    TFile * file_out = new TFile(Form("%s/alpha_correction_map.root",out_folder_directory.c_str()),"RECREATE");
    // file_out -> cd();
    eta_Mbin_correction_loose -> Write("eta_Mbin_correction_loose");
    eta_Mbin_correction_loose_noUPC -> Write("eta_Mbin_correction_loose_noUPC");
    eta_Mbin_correction_tight -> Write("eta_Mbin_correction_tight");
    
    // note : the dNdeta 1D for fulleta region, MC -> this is truth 
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        dNdeta_1D_fulleta_MC[Mbin] -> Write(Form("dNdeta_1D_fulleta_MC_Mbin%i",Mbin));
    }
    
    // note : the dNdeta 1D for selected eta region, MC
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        dNdeta_1D_MC[Mbin] -> Write(Form("dNdeta_1D_MC_Mbin%i",Mbin));
    }

    // note : the reconstructed dNdeta
    // note : loose method
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        dNdeta_1D_reco_multi_original[Mbin] -> Write(Form("dNdeta_1D_reco_loose_original_Mbin%i", Mbin));
    }

    // note : the reconstructed dNdeta 
    // note : tight method 
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        dNdeta_1D_reco_single_original[Mbin] -> Write(Form("dNdeta_1D_reco_tight_original_Mbin%i", Mbin));
    }

    // note : the reconstructed dNdeta
    // note : loose method
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        dNdeta_1D_reco_multi_postalpha[Mbin] -> Write(Form("dNdeta_1D_reco_loose_postalpha_Mbin%i", Mbin));
    }

    // note : the reconstructed dNdeta 
    // note : tight method 
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        dNdeta_1D_reco_single_postalpha[Mbin] -> Write(Form("dNdeta_1D_reco_tight_postalpha_Mbin%i", Mbin));
    }

    file_out -> Close();
}

vector<TH1F *> EtaDistReader::GetDeltaPhi_Multi_stack_1D()
{
    return DeltaPhi_Multi_Stack_hist_out;
}
vector<TH1F *> EtaDistReader::GetDeltaPhi_Multi_stack_1D_radian()
{
    return DeltaPhi_Multi_Stack_hist_out_radian;
}
vector<TH1F *> EtaDistReader::GetdNdeta_1D_MC()
{
    return dNdeta_1D_MC;    
}

vector<TH1F *> EtaDistReader::GetdNdeta_1D_fulleta_MC()
{
    return dNdeta_1D_fulleta_MC;
}

vector<TH1F *> EtaDistReader::GetdNdeta_1D_reco_single_original()
{
    return dNdeta_1D_reco_single_original;    
}
vector<TH1F *> EtaDistReader::GetdNdeta_1D_reco_multi_original()
{
    return dNdeta_1D_reco_multi_original;    
}

vector<TH1F *> EtaDistReader::GetdNdeta_1D_reco_single_postalpha()
{
    return dNdeta_1D_reco_single_postalpha;    
}
vector<TH1F *> EtaDistReader::GetdNdeta_1D_reco_multi_postalpha()
{
    return dNdeta_1D_reco_multi_postalpha;    
}

#endif


// void EtaDistReader::FinaldNdEta()
// {
//     double MC_hist_counting = 0;
//     double data_tight_counting = 0;
//     double data_loose_counting = 0;

//     c1 -> cd();
//     c1 -> Print(Form("%s/dNdeta_combine_final_no_correction.pdf(", out_folder_directory.c_str()));
//     for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
//     {   
//         // // note : check the ratio of the bin error to the bin content of the three histograms
//         // for (int i = 0; i < included_eta_z_map.size(); i++)
//         // {
//         //     cout<<" "<<endl;
//         //     cout<<"1MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinError(i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(i+1)<<" reco_single: "<<dNdeta_1D_reco_single_original[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi_original[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(i+1)<<endl;
//         // }

//         std::cout<<" "<<std::endl;

//         dNdeta_1D_MC[Mbin]          -> Scale(1./double(dNdeta_1D_MC[Mbin]          -> GetBinWidth(1) ));
//         dNdeta_1D_fulleta_MC[Mbin]  -> Scale(1./double(dNdeta_1D_fulleta_MC[Mbin]  -> GetBinWidth(1) ));
//         dNdeta_1D_reco_single_original[Mbin] -> Scale(1./double(dNdeta_1D_reco_single_original[Mbin] -> GetBinWidth(1) ));
//         dNdeta_1D_reco_multi_original[Mbin]  -> Scale(1./double(dNdeta_1D_reco_multi_original[Mbin]  -> GetBinWidth(1) ));

//         // // note : check the ratio of the bin error to the bin content of the three histograms
//         // for (int i = 0; i < included_eta_z_map.size(); i++)
//         // {
//         //     cout<<" "<<endl;
//         //     cout<<"2MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinError(i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(i+1)<<" reco_single: "<<dNdeta_1D_reco_single_original[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi_original[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(i+1)<<endl;
//         // }

//         // note : only for the fulleta region
//         for (int i = 0; i < dNdeta_1D_fulleta_MC[Mbin] -> GetNbinsX(); i++)
//         {
//             dNdeta_1D_fulleta_MC[Mbin] -> SetBinContent(i+1, dNdeta_1D_fulleta_MC[Mbin] -> GetBinContent(i+1) / double(N_event_counting_fulleta_MC[Mbin][i]));
//             dNdeta_1D_fulleta_MC[Mbin] -> SetBinError(i+1, dNdeta_1D_fulleta_MC[Mbin] -> GetBinError(i+1) / double(N_event_counting_fulleta_MC[Mbin][i]));
//         }

//         for (int i = 0; i < included_eta_z_map.size(); i++)
//         {
//             dNdeta_1D_MC[Mbin] -> SetBinContent(i+1, dNdeta_1D_MC[Mbin] -> GetBinContent(i+1) / double(N_event_counting_MC[Mbin][i]));
//             dNdeta_1D_MC[Mbin] -> SetBinError(i+1, dNdeta_1D_MC[Mbin] -> GetBinError(i+1) / double(N_event_counting_MC[Mbin][i]));

//             dNdeta_1D_reco_single_original[Mbin] -> SetBinContent(i+1, dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(i+1) / double(N_event_counting[Mbin][i]));
//             dNdeta_1D_reco_single_original[Mbin] -> SetBinError(i+1, dNdeta_1D_reco_single_original[Mbin] -> GetBinError(i+1) / double(N_event_counting[Mbin][i]));

//             dNdeta_1D_reco_multi_original[Mbin]  -> SetBinContent(i+1, dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(i+1) / double(N_event_counting[Mbin][i]));
//             dNdeta_1D_reco_multi_original[Mbin]  -> SetBinError(i+1, dNdeta_1D_reco_multi_original[Mbin] -> GetBinError(i+1) / double(N_event_counting[Mbin][i]));

//             cout<<"check denominators, Mbin : "<<Mbin<<" eta bin : "<<i+1<<" MC_count: "<<N_event_counting_MC[Mbin][i]<<" data_count: "<<N_event_counting[Mbin][i]<<endl;
        
//             if (input_eta_Mbin_correction_loose != nullptr)
//             {
//                 dNdeta_1D_reco_multi_original[Mbin] -> SetBinContent(i+1, dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(i+1) / input_eta_Mbin_correction_loose -> GetBinContent(i+1, Mbin+1));
//                 dNdeta_1D_reco_multi_original[Mbin] -> SetBinError(i+1, dNdeta_1D_reco_multi_original[Mbin] -> GetBinError(i+1) /     input_eta_Mbin_correction_loose -> GetBinContent(i+1, Mbin+1));
//             }

//             if (input_eta_Mbin_correction_tight != nullptr)
//             {
//                 dNdeta_1D_reco_single_original[Mbin] -> SetBinContent(i+1, dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(i+1) / input_eta_Mbin_correction_tight -> GetBinContent(i+1, Mbin+1));
//                 dNdeta_1D_reco_single_original[Mbin] -> SetBinError(i+1,   dNdeta_1D_reco_single_original[Mbin] -> GetBinError(i+1)   / input_eta_Mbin_correction_tight -> GetBinContent(i+1, Mbin+1));
//             }

//         }
        
//         cout<<"----------- for the case of method tight ----------------" <<endl;
//         // note : to print the ratio between reco track and MC track
//         std::cout << "centrality bin : "<<Mbin<<", ";
//         for (int bin_i = 0; bin_i < dNdeta_1D_MC[Mbin] -> GetNbinsX(); bin_i++) {
//             MC_hist_counting += dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1);
//             data_tight_counting += dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(bin_i+1);
//             std::cout <<"~~"<<dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(bin_i+1) <<", "<< dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)<< ", " << Form("%.3f",dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)) <<"~~, ";

//             eta_Mbin_correction_tight -> SetBinContent(bin_i + 1, Mbin+1, dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1));
//         }
//         std::cout << std::endl;

//         cout<<"----------- for the case of method inclusive ----------------" <<endl;
//         // note : to print the ratio between reco track and MC track
//         std::cout << "centrality bin : "<<Mbin<<", ";
//         for (int bin_i = 0; bin_i < dNdeta_1D_MC[Mbin] -> GetNbinsX(); bin_i++) {
//             data_loose_counting += dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(bin_i+1);
//             std::cout <<"~~"<<dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(bin_i+1) <<", "<< dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)<< ", " << Form( "%.3f", dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)) <<"~~, ";

//             eta_Mbin_correction_loose       -> SetBinContent(bin_i + 1, Mbin + 1, dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1));
//             eta_Mbin_correction_loose_noUPC -> SetBinContent(bin_i + 1, Mbin + 1, dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1));
//         }
//         std::cout << std::endl;

//         // note : check the bin content of the three histograms
//         // note : and check the bin error of the three histograms
//         // for (int i = 0; i < included_eta_z_map.size(); i++)
//         // {
//         //     cout<<" "<<endl;
//         //     cout<<"3MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinContent(i+1)<<" reco_single: "<<dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(i+1)<<endl;
//         //     cout<<"3MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinError(i+1)<<" reco_single: "<<dNdeta_1D_reco_single_original[Mbin] -> GetBinError(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi_original[Mbin] -> GetBinError(i+1)<<endl;
//         // }

//         // // note : check the ratio of the bin error to the bin content of the three histograms
//         // for (int i = 0; i < included_eta_z_map.size(); i++)
//         // {
//         //     cout<<" "<<endl;
//         //     cout<<"3MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinError(i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(i+1)<<" reco_single: "<<dNdeta_1D_reco_single_original[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_single_original[Mbin] -> GetBinContent(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi_original[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_multi_original[Mbin] -> GetBinContent(i+1)<<endl;
//         // }

//         if (Mbin == 0)
//         {
//             legend -> AddEntry(dNdeta_1D_fulleta_MC[Mbin], "Truth dist. (full)","f");
//             legend -> AddEntry(dNdeta_1D_MC[Mbin], "Truth dist. (selected)","f");
//             legend -> AddEntry(dNdeta_1D_reco_single_original[Mbin], "Reco. Method tight","lep");
//             legend -> AddEntry(dNdeta_1D_reco_multi_original[Mbin], "Reco. Method loose","lep");
//         }

//         // todo: this is for the test, to full the fulleta dNdeta together with others
//         dNdeta_1D_fulleta_MC[Mbin] -> GetYaxis() -> SetRangeUser(0, dNdeta_1D_MC[Mbin] -> GetMaximum() * 1.5);
//         dNdeta_1D_fulleta_MC[Mbin] -> Draw("hist");

//         // dNdeta_1D_MC[Mbin] -> GetYaxis() -> SetRangeUser(0, dNdeta_1D_MC[Mbin] -> GetMaximum() * 1.5);

//         dNdeta_1D_MC[Mbin] -> Draw("hist same");
//         dNdeta_1D_reco_single_original[Mbin] -> Draw("p same");
//         dNdeta_1D_reco_multi_original[Mbin]  -> Draw("p same");

//         legend -> Draw("same");

//         ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
//         draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[Mbin].c_str()));
       
//         c1 -> Print(Form("%s/dNdeta_combine_final_no_correction.pdf", out_folder_directory.c_str()));
//         c1 -> Clear();

//         cout<<"Mbin : "<<Mbin<<" MC_hist_counting: "<<MC_hist_counting<<" data_tight_counting: "<<data_tight_counting<<" data_loose_counting: "<<data_loose_counting<<endl;

//         MC_hist_counting = 0;
//         data_tight_counting = 0;
//         data_loose_counting = 0;
//     }
//     c1 -> Print(Form("%s/dNdeta_combine_final_no_correction.pdf)", out_folder_directory.c_str()));
//     c1 -> Clear();

//     gStyle->SetPaintTextFormat("1.3f");

//     c1 -> cd();
//     eta_Mbin_correction_loose -> Draw("colz0");
//     eta_Mbin_correction_loose -> SetMarkerSize(0.7);
//     eta_Mbin_correction_loose -> Draw("HIST TEXT45 SAME");
//     c1 -> Print(Form("%s/eta_Mbin_correction_loose.pdf", out_folder_directory.c_str()));
//     c1 -> Clear();

//     c1 -> cd();
//     for (int i = 0; i < eta_Mbin_correction_loose_noUPC -> GetNbinsX(); i++) {eta_Mbin_correction_loose_noUPC -> SetBinContent(i+1, eta_Mbin_correction_loose_noUPC -> GetNbinsY()-1, 0);}
//     eta_Mbin_correction_loose_noUPC -> Draw("colz0");
//     eta_Mbin_correction_loose_noUPC -> SetMarkerSize(0.7);
//     eta_Mbin_correction_loose_noUPC -> Draw("HIST TEXT45 SAME");
//     c1 -> Print(Form("%s/eta_Mbin_correction_loose_noUPC.pdf", out_folder_directory.c_str()));
//     c1 -> Clear();

//     c1 -> cd();
//     eta_Mbin_correction_tight -> Draw("colz0");
//     eta_Mbin_correction_tight -> SetMarkerSize(0.7);
//     eta_Mbin_correction_tight -> Draw("HIST TEXT45 SAME");
//     c1 -> Print(Form("%s/eta_Mbin_correction_tight.pdf", out_folder_directory.c_str()));
//     c1 -> Clear();

//     cout<<"FinaldNdEta done"<<endl;
//     return;
// }
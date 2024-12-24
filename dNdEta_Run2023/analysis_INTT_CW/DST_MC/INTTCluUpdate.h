#ifndef INTTCluUpdate_h
#define INTTCluUpdate_h

#include "INTTEta.h"
#include "ana_map_folder/ana_map_v1.h"

// todo : if the centrality bin or the eta and z regions are changed, modify the "ana_map_v1.h" first,
// todo : and change the following "namespace" name
namespace ana_map_version = ANA_MAP_V3;

class INTTCluUpdate : public INTTEta
{
    public:
        INTTCluUpdate(
            string run_type, 
            string out_folder_directory, 
            pair<double,double> beam_origin, 
            pair<double,double> selected_Z_range_in,
            int geo_mode_id, 
            double phi_diff_cut = 0.11, 
            pair<double, double> DCA_cut = {-1,1}, 
            int N_clu_cutl = 20, 
            int N_clu_cut = 10000, 
            bool draw_event_display = true, 
            double peek = 3.32405, 
            double angle_diff_new_l = 0.0, 
            double angle_diff_new_r = 3, 
            bool print_message_opt = true
        ):
        INTTEta(
            run_type, 
            out_folder_directory, 
            beam_origin, 
            geo_mode_id, 
            phi_diff_cut, 
            DCA_cut, 
            N_clu_cutl, 
            N_clu_cut, 
            draw_event_display, 
            peek, 
            angle_diff_new_l, 
            angle_diff_new_r, 
            print_message_opt),
        selected_Z_range(selected_Z_range_in)
        {
            out_clu_X.clear();
            out_clu_Y.clear();
            out_clu_Z.clear();
            out_clu_eta.clear();
            out_clu_phi.clear();
            out_clu_R.clear();
            out_clu_layer.clear();
            out_clu_ladder.clear();
            out_clu_ZID.clear();
            out_clu_phi_size.clear();
            out_clu_z_size.clear();

            InitTFileOut();
            InitHist();
        }

        void ProcessEvt(
            int event_i, 
            vector<clu_info> temp_sPH_inner_nocolumn_vec, 
            vector<clu_info> temp_sPH_outer_nocolumn_vec, 
            vector<vector<double>> temp_sPH_nocolumn_vec, 
            vector<vector<double>> temp_sPH_nocolumn_rz_vec, 
            int NvtxMC, 
            vector<double> TrigvtxMC, 
            bool PhiCheckTag, 
            Long64_t bco_full, 
            pair<double,double> evt_z, 
            int centrality_bin, 
            vector<vector<float>> true_track_info,
            double zvtx_weighting
        ) override;

        void ClearEvt() override;
        void EndRun() override;  

    protected:
        void InitTFileOut();
        void InitTTreeOut();
        void InitHist();

        pair<double,double> selected_Z_range;

        int inner_NClu;
        int outer_NClu;


        TFile * file_out; 
        TTree * tree_out;
        vector<double> out_clu_X;
        vector<double> out_clu_Y;
        vector<double> out_clu_Z;
        vector<double> out_clu_eta;
        vector<double> out_clu_phi;
        vector<double> out_clu_R;
        vector<int>    out_clu_layer;
        vector<int>    out_clu_ladder;
        vector<int>    out_clu_ZID;
        vector<int>    out_clu_phi_size;
        vector<int>    out_clu_z_size;
        int    out_NClus;
        double out_reco_INTT_Z;
        double out_reco_MBD_Z;
        double out_true_Z;
        int out_NvtxMC;
        bool out_PhiCheckTag; 
        Long64_t out_bco_full; 
        float out_centrality_float;
        int out_is_min_bias;
        int out_is_min_bias_wozdc;
        double out_MBD_north_charge_sum;
        double out_MBD_south_charge_sum;

        TH1F * exclusive_NClus_inner;
        TH1F * exclusive_NClus_outer;
        TH1F * exclusive_NClus_sum;

        TH1F * exclusive_cluster_inner_adc;
        TH1F * exclusive_cluster_outer_adc;
        
        TH2F * exclusive_cluster_inner_eta_adc_2D;
        TH2F * exclusive_cluster_outer_eta_adc_2D;
        TH2F * exclusive_cluster_inner_eta_phi_2D;
        TH2F * exclusive_cluster_outer_eta_phi_2D;
        
        TH1F * exclusive_cluster_inner_eta;
        TH1F * exclusive_cluster_inner_phi;
        TH1F * exclusive_cluster_outer_eta;
        TH1F * exclusive_cluster_outer_phi;
        TH1F * exclusive_cluster_all_eta;
        TH1F * exclusive_cluster_all_phi;
};

void INTTCluUpdate::InitTFileOut()
{
    file_out = new TFile((out_folder_directory + "/INTTCluDist.root").c_str(), "RECREATE");
}

void INTTCluUpdate::InitTTreeOut()
{
    tree_out = new TTree("EventTree", "EventTree");

    tree_out -> Branch("NClus", &out_NClus);

    tree_out -> Branch("clu_X", &out_clu_X);
    tree_out -> Branch("clu_Y", &out_clu_Y);
    tree_out -> Branch("clu_Z", &out_clu_Z);
    tree_out -> Branch("clu_eta", &out_clu_eta);
    tree_out -> Branch("clu_phi", &out_clu_phi);
    tree_out -> Branch("clu_R", &out_clu_R);
    tree_out -> Branch("clu_layer", &out_clu_layer);
    tree_out -> Branch("clu_ladder", &out_clu_ladder);
    tree_out -> Branch("clu_ZID", &out_clu_ZID);
    tree_out -> Branch("clu_phi_size", &out_clu_phi_size);
    tree_out -> Branch("clu_z_size", &out_clu_z_size);

    tree_out -> Branch("reco_INTT_Z", &out_reco_INTT_Z);
    tree_out -> Branch("reco_MBD_Z", &out_reco_MBD_Z);
    tree_out -> Branch("true_Z", &out_true_Z);
    tree_out -> Branch("NvtxMC", &out_NvtxMC);
    tree_out -> Branch("PhiCheckTag", &out_PhiCheckTag);
    tree_out -> Branch("bco_full", &out_bco_full);
    tree_out -> Branch("centrality_float", &out_centrality_float);
    tree_out -> Branch("is_min_bias", &out_is_min_bias);
    tree_out -> Branch("is_min_bias_wozdc", &out_is_min_bias_wozdc);
    tree_out -> Branch("MBD_north_charge_sum", &out_MBD_north_charge_sum);
    tree_out -> Branch("MBD_south_charge_sum", &out_MBD_south_charge_sum);
}

void INTTCluUpdate::InitHist()
{
    exclusive_NClus_inner        = new TH1F("","exclusive_NClus_inner;NClus inner barrel;Entry",50, 0, 5000);
    exclusive_NClus_outer        = new TH1F("","exclusive_NClus_outer;NClus outer barrel;Entry",50,0,5000);
    exclusive_NClus_sum          = new TH1F("","exclusive_NClus_sum;NClus total;Entry",50,0,9000);    

    exclusive_cluster_inner_eta  = new TH1F("","exclusive_cluster_inner_eta;Cluster inner #eta;Entry",50,-3.5,3.5);
    exclusive_cluster_inner_phi  = new TH1F("","exclusive_cluster_inner_phi;Cluster inner #phi [radian];Entry",50,-3.5,3.5);
    exclusive_cluster_outer_eta  = new TH1F("","exclusive_cluster_outer_eta;Cluster outer #eta;Entry",50,-3.5,3.5);
    exclusive_cluster_outer_phi  = new TH1F("","exclusive_cluster_outer_phi;Cluster outer #phi [radian];Entry",50,-3.5,3.5);
    exclusive_cluster_all_eta    = new TH1F("","exclusive_cluster_all_eta;Cluster #eta;Entry",50,-3.5,3.5);
    exclusive_cluster_all_phi    = new TH1F("","exclusive_cluster_all_phi;Cluster #phi [radian];Entry",50,-3.5,3.5);

    exclusive_cluster_inner_adc  = new TH1F("","exclusive_cluster_inner_adc;Cluster inner adc;Entry",50,0,400);
    exclusive_cluster_outer_adc  = new TH1F("","exclusive_cluster_outer_adc;Cluster outer adc;Entry",50,0,400);

    exclusive_cluster_inner_eta_adc_2D = new TH2F("","exclusive_cluster_inner_eta_adc_2D;Cluster inner #eta; Cluster ADC",
        2000, exclusive_cluster_inner_eta -> GetXaxis() -> GetXmin(), exclusive_cluster_inner_eta -> GetXaxis() -> GetXmax(), 
        exclusive_cluster_inner_adc -> GetNbinsX(), exclusive_cluster_inner_adc -> GetXaxis() -> GetXmin(), exclusive_cluster_inner_adc -> GetXaxis() -> GetXmax()
    );

    exclusive_cluster_outer_eta_adc_2D = new TH2F("","exclusive_cluster_outer_eta_adc_2D;Cluster outer #eta; Cluster ADC",
        2000, exclusive_cluster_outer_eta -> GetXaxis() -> GetXmin(), exclusive_cluster_outer_eta -> GetXaxis() -> GetXmax(), 
        exclusive_cluster_outer_adc -> GetNbinsX(), exclusive_cluster_outer_adc -> GetXaxis() -> GetXmin(), exclusive_cluster_outer_adc -> GetXaxis() -> GetXmax()
    );

    exclusive_cluster_inner_eta_phi_2D = new TH2F(
        "",
        "exclusive_cluster_inner_eta_phi_2D;Cluster inner #eta; Cluster inner #phi",
        2000, exclusive_cluster_inner_eta -> GetXaxis() -> GetXmin(), exclusive_cluster_inner_eta -> GetXaxis() -> GetXmax(),
        2000, exclusive_cluster_inner_phi -> GetXaxis() -> GetXmin(), exclusive_cluster_inner_phi -> GetXaxis() -> GetXmax()
    );
    exclusive_cluster_outer_eta_phi_2D = new TH2F(
        "",
        "exclusive_cluster_outer_eta_phi_2D;Cluster outer #eta; Cluster outer #phi",
        2000, exclusive_cluster_outer_eta -> GetXaxis() -> GetXmin(), exclusive_cluster_outer_eta -> GetXaxis() -> GetXmax(),
        2000, exclusive_cluster_outer_phi -> GetXaxis() -> GetXmin(), exclusive_cluster_outer_phi -> GetXaxis() -> GetXmax()
    );
}

void INTTCluUpdate::ProcessEvt(
    int event_i, 
    vector<clu_info> temp_sPH_inner_nocolumn_vec, 
    vector<clu_info> temp_sPH_outer_nocolumn_vec, 
    vector<vector<double>> temp_sPH_nocolumn_vec, 
    vector<vector<double>> temp_sPH_nocolumn_rz_vec, 
    int NvtxMC, 
    vector<double> TrigvtxMC, 
    bool PhiCheckTag, 
    Long64_t bco_full, 
    pair<double,double> evt_z, 
    int centrality_bin, 
    vector<vector<float>> true_track_info,
    double zvtx_weighting = 1.0
)
{
    return_tag = 0;

    if (event_i%100 == 0) {cout<<"In INTTEta class, running event : "<<event_i<<endl;}

    inner_NClu = temp_sPH_inner_nocolumn_vec.size();
    outer_NClu = temp_sPH_outer_nocolumn_vec.size();
    total_NClus = inner_NClu + outer_NClu;

    // cout<<"test_0"<<endl;
    if (total_NClus < zvtx_cal_require) {return; cout<<"return confirmation"<<endl;}   
    
    if (run_type == "MC" && NvtxMC != 1) { return; cout<<"In INTTEta class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Nvtx more than one "<<endl;}
    if (PhiCheckTag == false)            { return; cout<<"In INTTEta class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Not full phi has hits "<<endl;}
    
    if (inner_NClu < 10 || outer_NClu < 10 || total_NClus > N_clu_cut || total_NClus < N_clu_cutl)
    {
        return;
        printf("In INTTEta class, event : %i, low clu continue, NClus : %lu \n", event_i, total_NClus); 
    }

    if (evt_z.first < selected_Z_range.first || evt_z.first > selected_Z_range.second) {return;}

    exclusive_NClus_inner -> Fill(inner_NClu, zvtx_weighting);
    exclusive_NClus_outer -> Fill(outer_NClu, zvtx_weighting);
    exclusive_NClus_sum   -> Fill(total_NClus, zvtx_weighting);

    for (int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++) {
        double Clus_InnerPhi_Offset_radian = atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first);

        double clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y, temp_sPH_inner_nocolumn_vec[inner_i].z});
        
        exclusive_cluster_inner_eta -> Fill(clu_eta, zvtx_weighting);
        exclusive_cluster_inner_phi -> Fill(Clus_InnerPhi_Offset_radian, zvtx_weighting);

        exclusive_cluster_inner_eta_phi_2D -> Fill(clu_eta, Clus_InnerPhi_Offset_radian, zvtx_weighting);

        exclusive_cluster_inner_eta_adc_2D -> Fill(clu_eta, temp_sPH_inner_nocolumn_vec[inner_i].sum_adc, zvtx_weighting);

        exclusive_cluster_all_eta   -> Fill(clu_eta, zvtx_weighting);
        exclusive_cluster_all_phi   -> Fill(Clus_InnerPhi_Offset_radian, zvtx_weighting);

        exclusive_cluster_inner_adc -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].sum_adc, zvtx_weighting);
    }
    for (int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++) {
        double Clus_OuterPhi_Offset_radian = atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first);

        double clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y, temp_sPH_outer_nocolumn_vec[outer_i].z});
       
        exclusive_cluster_outer_eta -> Fill(clu_eta, zvtx_weighting);
        exclusive_cluster_outer_phi -> Fill(Clus_OuterPhi_Offset_radian, zvtx_weighting);

        exclusive_cluster_outer_eta_phi_2D -> Fill(clu_eta, Clus_OuterPhi_Offset_radian, zvtx_weighting);

        exclusive_cluster_outer_eta_adc_2D -> Fill(clu_eta, temp_sPH_outer_nocolumn_vec[outer_i].sum_adc, zvtx_weighting);

        exclusive_cluster_all_eta   -> Fill(clu_eta, zvtx_weighting);
        exclusive_cluster_all_phi   -> Fill(Clus_OuterPhi_Offset_radian, zvtx_weighting);

        exclusive_cluster_outer_adc -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].sum_adc, zvtx_weighting);
    }

    return_tag = 1;
}

void INTTCluUpdate::ClearEvt()
{
    return;
}

void INTTCluUpdate::EndRun()
{
    out_file -> cd();

    exclusive_NClus_inner -> Write("exclusive_NClus_inner");
    exclusive_NClus_outer -> Write("exclusive_NClus_outer");
    exclusive_NClus_sum -> Write("exclusive_NClus_sum");
    exclusive_cluster_inner_adc -> Write("exclusive_cluster_inner_adc");
    exclusive_cluster_outer_adc -> Write("exclusive_cluster_outer_adc");
    exclusive_cluster_inner_eta_adc_2D -> Write("exclusive_cluster_inner_eta_adc_2D");
    exclusive_cluster_outer_eta_adc_2D -> Write("exclusive_cluster_outer_eta_adc_2D");
    exclusive_cluster_inner_eta_phi_2D -> Write("exclusive_cluster_inner_eta_phi_2D");
    exclusive_cluster_outer_eta_phi_2D -> Write("exclusive_cluster_outer_eta_phi_2D");
    exclusive_cluster_inner_eta -> Write("exclusive_cluster_inner_eta");
    exclusive_cluster_inner_phi -> Write("exclusive_cluster_inner_phi");
    exclusive_cluster_outer_eta -> Write("exclusive_cluster_outer_eta");
    exclusive_cluster_outer_phi -> Write("exclusive_cluster_outer_phi");
    exclusive_cluster_all_eta -> Write("exclusive_cluster_all_eta");
    exclusive_cluster_all_phi -> Write("exclusive_cluster_all_phi");

    out_file -> Close();
    std::cout<<"End of INTTCluUpdate"<<std::endl;
}


#endif
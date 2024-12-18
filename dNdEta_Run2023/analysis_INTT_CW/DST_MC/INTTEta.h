#ifndef INTTEta_h
#define INTTEta_h

#include "INTTXYvtxEvt.h"
#include "MegaTrackFinder.h"
#include "ana_map_folder/ana_map_v1.h"

// todo : if the centrality bin or the eta and z regions are changed, modify the "ana_map_v1.h" first,
// todo : and change the following "namespace" name
namespace ana_map_version = ANA_MAP_V3;

class INTTEta : public INTTXYvtxEvt
{
    public:
        INTTEta(
            string run_type, 
            string out_folder_directory, 
            pair<double,double> beam_origin, 
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
        INTTXYvtxEvt(run_type, out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, angle_diff_new_l, angle_diff_new_r, print_message_opt, 2.5, 9)
        {
            track_cluster_ratio_1D.clear();
            track_cluster_ratio_1D_MC.clear();
            dNdeta_1D.clear();
            track_eta_phi_2D.clear();
            track_eta_z_2D.clear();
            dNdeta_1D_MC.clear();
            dNdeta_1D_MC_edge_eta_cut.clear();
            final_dNdeta_1D.clear();
            final_dNdeta_1D_MC.clear();
            final_dNdeta_multi_1D.clear();
            track_eta_z_2D_MC.clear();
            track_delta_phi_1D.clear();
            track_DCA_distance.clear();
            final_track_delta_phi_1D.clear();
            final_track_multi_delta_phi_1D.clear();

            track_delta_eta_1D.clear();
            track_delta_eta_1D_post.clear();
            track_phi_DCA_2D.clear();
            track_DeltaPhi_eta_2D.clear();
            track_DeltaPhi_DeltaEta_2D.clear();
            track_ratio_1D.clear();

            proto_pair_index.clear();
            proto_pair_delta_phi_abs.clear();
            proto_pair_delta_phi.clear();
            inner_used_clu.clear();
            outer_used_clu.clear();

            coarse_eta_z_2D_MC.clear();
            coarse_eta_z_2D_fulleta_MC.clear();
            coarse_Reco_SignalNTracklet_Single_eta_z_2D.clear();
            coarse_Reco_SignalNTracklet_Multi_eta_z_2D.clear();

            out_recotrack_eta_d.clear();
            out_recotrack_phi_d.clear();
            out_truetrack_eta_d.clear();
            out_truetrack_phi_d.clear();

            out_track_eta_i.clear();
            out_track_delta_phi_d.clear();
            clu_multi_used_tight = 0;
            clu_multi_used_loose = 0;
            effective_total_NClus = 0;

            out_N2Clu_track = 0;
            out_N3Clu_track = 0;
            out_N4Clu_track = 0;

            eta_z_convert_map.clear();
            eta_z_convert_inverse_map.clear();
            eta_z_convert_map_index = 0;

            // std::fill(std::begin(inner_clu_phi_map), std::end(inner_clu_phi_map), std::vector<pair<bool,clu_info>>());
            // std::fill(std::begin(outer_clu_phi_map), std::end(outer_clu_phi_map), std::vector<pair<bool,clu_info>>());
            inner_clu_phi_map.clear();
            outer_clu_phi_map.clear();
            inner_clu_phi_map = vector<vector<pair<bool,clu_info>>>(360);
            outer_clu_phi_map = vector<vector<pair<bool,clu_info>>>(360);

            mega_track_finder = new MegaTrackFinder(run_type, out_folder_directory, centrality_map.size(), beam_origin, true); // note : "true" means we marked the clusters as used

            // note : 10 is for the centrality bin
            N_GoodEvent_vec = vector<long long>(10,0); // todo : if the centrality bin is changed, the vector should be updated 

            InitHist();
            InitGraph();
            InitTree();

            cout<<"test, eta_region.size() : "<<eta_region.size()<<endl;
            cout<<"test, z_region.size() : "<<z_region.size()<<endl;
            for (int i = 0; i < z_region.size() - 1; i++) // note : Y axis in the TH2F
            {
                for (int j = 0; j < eta_region.size() - 1; j++) // note : X axis in the TH2F
                {
                    eta_z_convert_map[std::to_string(j+1) + "_" + std::to_string(i+1)] = eta_z_convert_map_index;
                    eta_z_convert_inverse_map[eta_z_convert_map_index] = {j+1, i+1};
                    eta_z_convert_map_index += 1;
                }
            }

            cout<<"In INTTEta, centrality_region_size: "<<centrality_region_size<<" eta_region_size: "<<eta_region_size<<" z_region_size: "<<z_region.size() - 1<<endl;
            good_tracklet_counting = std::vector<std::vector<int>>(centrality_region_size, std::vector<int>(((eta_region.size() - 1) * (z_region.size() - 1)), 0));
            good_tracklet_multi_counting = std::vector<std::vector<int>>(centrality_region_size, std::vector<int>(((eta_region.size() - 1) * (z_region.size() - 1)), 0));

            out_folder_directory_evt = out_folder_directory + "/evt_display";
            system(Form("mkdir %s", out_folder_directory_evt.c_str()));

            loop_NGoodPair = 0;
            evt_NTrack = 0;
            evt_NTrack_MC = 0;
            N_GoodEvent = 0;
            return;
        };

        INTTEta(string run_type, string out_folder_directory):INTTXYvtxEvt(run_type, out_folder_directory)
        {
            return;
        };
        
        virtual void ProcessEvt(
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
        ); 
        // void ProcessEvtMC(int event_i, vector<float> true_track_info, vector<double> TrigvtxMC);
        void ClearEvt() override;
        void PrintPlots() override;
        void EndRun() override;

    protected:
        // note : {5, 15, 25, 35, 45, 55, 65, 75, 85, 95, inclusive}
        vector<TH1F *> track_cluster_ratio_1D;
        vector<TH1F *> track_cluster_ratio_1D_MC;
        vector<TH1F *> dNdeta_1D;
        vector<TH2F *> track_eta_phi_2D; 
        vector<TH2F *> track_eta_z_2D; 

        vector<TH1F *> dNdeta_1D_MC;
        vector<TH1F *> dNdeta_1D_MC_edge_eta_cut;
        vector<TH2F *> track_eta_z_2D_MC; 

        vector<TH1F *> track_delta_eta_1D;
        vector<TH1F *> track_delta_eta_1D_post; 
        vector<TH1F *> track_delta_phi_1D;
        vector<TH1F *> track_DCA_distance;
        vector<TH2F *> track_phi_DCA_2D;

        vector<TH2F *> track_DeltaPhi_eta_2D;
        vector<TH2F *> track_DeltaPhi_DeltaEta_2D;

        TH2F * track_correlation_2D;
        TH2F * track_ratio_2D;
        vector<TH1F *> track_ratio_1D; // note : N reco track /N true track

        TH2F * reco_eta_correlation_2D;
        TH2F * reco_eta_diff_reco3P_2D;
        TH1F * reco_eta_diff_1D;

        TH2F * clu_used_centrality_2D;

        vector<vector<TH1F *>> final_track_delta_phi_1D;       // note : centrality_bin and different eta
        vector<vector<TH1F *>> final_track_multi_delta_phi_1D; // note : centrality_bin and different eta
        // vector<vector<int>> good_tracklet_counting;
        vector<TH1F *> final_dNdeta_1D;
        vector<TH1F *> final_dNdeta_multi_1D;
        vector<TH1F *> final_dNdeta_1D_MC;

        TGraph * evt_reco_track_gr_All; // note : all the tracklets in one event, no selection
        TGraph * evt_reco_track_gr_PhiLoose; // note : the tracklets that pass the loose phi selection
        TGraph * evt_reco_track_gr_Z; // note : the tracklets that pass the z selection and the loose phi selection
        TGraph * evt_reco_track_gr_ZDCA; // note : the tracklets that pass the z and phi selection, and the loose phi selection
        TGraph * evt_reco_track_gr_ZDCAPhi; // note : the tracklets that pass the z, phi and DCA selection, and the loose phi selection
        TGraph * evt_true_track_gr; // note : the true tracklets in one event, no selection

        MegaTrackFinder * mega_track_finder; // note : the class that handles the 3/4-cluster tracklet finder
        TH2F * NClu3_track_centrality_2D;
        TH2F * NClu4_track_centrality_2D;
        TH1F * cluster4_track_phi_1D;
        TH1F * cluster3_track_phi_1D;
        TH1F * cluster3_inner_track_eta_1D;
        TH1F * cluster3_inner_track_phi_1D;
        TH1F * cluster3_outer_track_eta_1D;
        TH1F * cluster3_outer_track_phi_1D;
        TH1F * clu4_track_ReducedChi2_1D;
        TH1F * clu3_track_ReducedChi2_1D;
        TH1F * mega_track_eta_1D;
        TH2F * mega_track_finding_ratio_2D;
        TH2F * mega_track_contamination_2D;

        // note : diagnostic plots
        TH1F * check_inner_layer_clu_phi_1D;
        TH1F * check_outer_layer_clu_phi_1D;

        // note : for the comparison with other files, MC, for example
        // note : after all the selections, so it is exclusive
        // note : cut : cluster size, cluster adc
        // note : min bias cut
        // note : zvtx quality cut
        // note: zvtx acceptance cut
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

        TH1F * exclusive_tight_tracklet_eta; // note : signal region only
        TH1F * exclusive_tight_tracklet_phi; // note : signal region only
        TH1F * exclusive_loose_tracklet_eta; // note : signal region only
        TH1F * exclusive_loose_tracklet_phi; // note : signal region only


        // vector<pair<bool,clu_info>> inner_clu_phi_map[360]; // note: phi
        // vector<pair<bool,clu_info>> outer_clu_phi_map[360]; // note: phi
        vector<vector<pair<bool,clu_info>>> inner_clu_phi_map; // note: phi
        vector<vector<pair<bool,clu_info>>> outer_clu_phi_map; // note: phi

        string out_folder_directory_evt;

        int draw_evt_cut = 15; // todo : change the cut value (number of true track)
        int print_plot_ratio = 3; // note : print the event display every 3 events
        int inner_NClu;
        int outer_NClu;
        int loop_NGoodPair;
        int evt_NTrack;
        int evt_NTrack_MC;
        long long N_GoodEvent;
        int clu_multi_used_loose;
        int clu_multi_used_tight;
        int effective_total_NClus;
        double pair_delta_phi;
        pair<int,int> pair_outer_index;
        // double INTT_layer_R = 71.88; // note: the innermost, B0L0, layer radius
        double INTT_layer_R[4] = {71.88, 77.32, 96.8, 102.62}; // note : the radii of the INTT layers
        
        vector<long long> N_GoodEvent_vec;
        pair<double,double> Get_eta_pair;
        vector<vector<double>> final_eta_entry; // note : not used


        vector<vector<int>> proto_pair_index;
        vector<vector<double>> proto_pair_delta_phi;
        vector<double> proto_pair_delta_phi_abs;
        map<string,int> inner_used_clu;
        map<string,int> outer_used_clu;

        double Clus_InnerPhi_Offset_1;
        double Clus_InnerPhi_Offset_2;
        double Clus_OuterPhi_Offset_1;
        double Clus_OuterPhi_Offset_2;

        double N_reco_cluster_short = 0;


        TH2F * track_cluster_ratio_multiplicity_2D; // note : x : NClus / NTracks, y : ratio
        TH2F * track_cluster_ratio_multiplicity_2D_MC; // note : x : NClus / NTracks, y : ratio
        TGraphErrors * track_gr;

        TFile * out_file;
        TTree * tree_out;
        int out_eID;
        int out_evt_centrality_bin;
        int out_NTrueTrack;
        int out_total_NClus;
        int out_N2Clu_track; // note : method tight
        int out_N3Clu_track; // note : mega cluster track 3-cluster track
        int out_N4Clu_track; // note : mega cluster track 4-cluster track
        double out_evt_zvtx;
        double out_true_zvtx;
        vector<double> out_recotrack_eta_d;
        vector<double> out_recotrack_phi_d;
        vector<double> out_truetrack_eta_d;
        vector<double> out_truetrack_phi_d;
        vector<int> out_track_eta_i;
        vector<double> out_track_delta_phi_d;

        TH1F * eta_region_hist;

        map<int,int> centrality_map = ana_map_version::centrality_map;
        vector<string> centrality_region = ana_map_version::centrality_region;
        // note : the following two vectors tell the region of the eta and z, including the edges in both sides.
        // note : so when talking about the number of bins, we have to do size() - 1
        vector<double> eta_region = ana_map_version::eta_region;
        vector<double> z_region = ana_map_version::z_region;
        pair<double, double> selected_z_region_id = ana_map_version::selected_z_region_id;
        double signal_region = ana_map_version::signal_region; // note : the signal region of the delta phi distribution, unit : degree

        // note : when filling in a z-eta pair, TH2F returns a int, we have a function to convert that number into the index position in X and Y
        // note : Once we have the position index in X and Y, we then convert the X and Y into the index for 1D array
        int eta_z_convert_map_index;
        map<string,int> eta_z_convert_map;
        map<int, pair<int,int>> eta_z_convert_inverse_map;

        TH2F * coarse_eta_z_map; // note : the reference of the eta and z binning setting
        vector<TH2F *> coarse_eta_z_2D_MC; // note : keep the number of true tracks in each eta and z bin, for each centrality
        vector<TH2F *> coarse_eta_z_2D_fulleta_MC; // note : no selection on the true level tracks
        vector<TH2F *> coarse_Reco_SignalNTracklet_Single_eta_z_2D; // note : keep the counting of the N reco tracklet in the signal region for each eta, z and centrality bin. (for the single-cluster-used tracklet)
        vector<TH2F *> coarse_Reco_SignalNTracklet_Multi_eta_z_2D;  // note : // note : keep the counting of the N reco tracklet in the signal region for each eta, z and centrality bin. (for the multi-cluster-used tracklet)
        TH2F * MBin_Z_evt_hist_2D;    // note : keep the number of event as a function of the z vertex and centrality bin
        TH2F * MBin_Z_evt_hist_2D_MC; // note : keep the number of event as a function of the z vertex and centrality bin, but the zvtx info. is given by the MC, the true zvtx
        

        
        // todo : change the tight zvtx cut here 
        int tight_zvtx_bin = 6; // todo : it can be run by run dependent // note : select the bin that you want to have the quick result out

        const int centrality_region_size = centrality_region.size();
        const int  eta_region_size = eta_region.size() - 1;
        std::vector<std::vector<int>> good_tracklet_counting;
        std::vector<std::vector<int>> good_tracklet_multi_counting;
        
        void InitHist();
        void InitGraph();
        void InitTree();

        double grEY_stddev(TGraphErrors * input_grr);
        pair<double, double> mirrorPolynomial(double a, double b);
        pair<double,double> Get_eta(vector<double>p0, vector<double>p1, vector<double>p2);
        double convertTo360(double radian);
        void print_evt_plot(int event_i, int NTrueTrack, int innerNClu, int outerNClu);
        double get_clu_eta(vector<double> vertex, vector<double> clu_pos);
        // double get_dist_offset(TH1F * hist_in, int check_N_bin);
        double get_delta_phi(double angle_1, double angle_2);
        double get_track_phi(double inner_clu_phi_in, double delta_phi_in);
        pair<int,int> GetTH2BinXY(int histNbinsX, int histNbinsY, int binglobal);
        double GetTH2Index1D(pair<int,int> XY_index, int histNbinsX);
        void DrawEtaZGrid();
        double get_TH1F_Entries(TH1F * hist_in);
};

void INTTEta::InitHist()
{
    // double Eta_NBin = 145;
    // double Eta_Min = -2.9;
    // double Eta_Max = 2.9;

    // todo: change the fine eta binning here
    double Eta_NBin = 61;
    double Eta_Min = -3.05;
    double Eta_Max = 3.05;

    final_track_delta_phi_1D = vector<vector<TH1F *>>(centrality_region.size());
    final_track_multi_delta_phi_1D = vector<vector<TH1F *>>(centrality_region.size());
    final_eta_entry = vector<vector<double>>(centrality_region.size());

    coarse_eta_z_map = new TH2F("","", eta_region.size() - 1, &eta_region[0], z_region.size() - 1, &z_region[0]);
    coarse_eta_z_map -> GetXaxis() -> SetTitle("#eta");
    coarse_eta_z_map -> GetYaxis() -> SetTitle("Z vertex [mm]");
    coarse_eta_z_map -> GetXaxis() -> SetNdivisions(505);

    for (int i = 0; i < centrality_region.size(); i++) 
    {

        coarse_eta_z_2D_MC.push_back( new TH2F("","", eta_region.size() - 1, &eta_region[0], z_region.size() - 1, &z_region[0]) );
        coarse_eta_z_2D_MC[i] -> GetXaxis() -> SetTitle("#eta");
        coarse_eta_z_2D_MC[i] -> GetYaxis() -> SetTitle("Z vertex [mm]");
        coarse_eta_z_2D_MC[i] -> GetXaxis() -> SetNdivisions(505);

        coarse_eta_z_2D_fulleta_MC.push_back( new TH2F("","", eta_region.size() - 1, &eta_region[0], z_region.size() - 1, &z_region[0]) );
        coarse_eta_z_2D_fulleta_MC[i] -> GetXaxis() -> SetTitle("#eta");
        coarse_eta_z_2D_fulleta_MC[i] -> GetYaxis() -> SetTitle("Z vertex [mm]");
        coarse_eta_z_2D_fulleta_MC[i] -> GetXaxis() -> SetNdivisions(505);

        coarse_Reco_SignalNTracklet_Single_eta_z_2D.push_back( new TH2F("","", eta_region.size() - 1, &eta_region[0], z_region.size() - 1, &z_region[0]) );
        coarse_Reco_SignalNTracklet_Single_eta_z_2D[i] -> GetXaxis() -> SetTitle("#eta");
        coarse_Reco_SignalNTracklet_Single_eta_z_2D[i] -> GetYaxis() -> SetTitle("Z vertex [mm]");
        coarse_Reco_SignalNTracklet_Single_eta_z_2D[i] -> GetXaxis() -> SetNdivisions(505);

        coarse_Reco_SignalNTracklet_Multi_eta_z_2D.push_back( new TH2F("","", eta_region.size() - 1, &eta_region[0], z_region.size() - 1, &z_region[0]) );
        coarse_Reco_SignalNTracklet_Multi_eta_z_2D[i] -> GetXaxis() -> SetTitle("#eta");
        coarse_Reco_SignalNTracklet_Multi_eta_z_2D[i] -> GetYaxis() -> SetTitle("Z vertex [mm]");
        coarse_Reco_SignalNTracklet_Multi_eta_z_2D[i] -> GetXaxis() -> SetNdivisions(505);
        
        track_cluster_ratio_1D.push_back(new TH1F("","track_cluster_ratio_1D",200,0,15));
        track_cluster_ratio_1D[i] -> GetXaxis() -> SetTitle("NClus / NTracks");
        track_cluster_ratio_1D[i] -> GetYaxis() -> SetTitle("Entry");
        track_cluster_ratio_1D[i] -> GetXaxis() -> SetNdivisions(505);

        track_cluster_ratio_1D_MC.push_back(new TH1F("","track_cluster_ratio_1D_MC",200,0,15));
        track_cluster_ratio_1D_MC[i] -> GetXaxis() -> SetTitle("NClus / NTrackMC");
        track_cluster_ratio_1D_MC[i] -> GetYaxis() -> SetTitle("Entry");
        track_cluster_ratio_1D_MC[i] -> GetXaxis() -> SetNdivisions(505);

        dNdeta_1D.push_back(new TH1F("","",Eta_NBin, Eta_Min, Eta_Max));
        dNdeta_1D[i] -> SetMarkerStyle(20);
        dNdeta_1D[i] -> SetMarkerSize(0.8);
        dNdeta_1D[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D[i] -> SetLineWidth(2);
        dNdeta_1D[i] -> GetYaxis() -> SetTitle("Entry");
        dNdeta_1D[i] -> GetXaxis() -> SetTitle("N tracklet #eta");
        // dNdeta_1D[i] -> GetYaxis() -> SetRangeUser(0,50);
        dNdeta_1D[i] -> SetTitleSize(0.06, "X");
        dNdeta_1D[i] -> SetTitleSize(0.06, "Y");
        dNdeta_1D[i] -> GetXaxis() -> SetTitleOffset (0.71);
        dNdeta_1D[i] -> GetYaxis() -> SetTitleOffset (1.1);
        dNdeta_1D[i] -> GetXaxis() -> CenterTitle(true);
        dNdeta_1D[i] -> GetYaxis() -> CenterTitle(true);

        dNdeta_1D_MC.push_back(new TH1F("","",Eta_NBin, Eta_Min, Eta_Max));
        dNdeta_1D_MC[i] -> SetMarkerStyle(20);
        dNdeta_1D_MC[i] -> SetMarkerSize(0.8);
        dNdeta_1D_MC[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_MC[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_MC[i] -> SetLineWidth(2);
        dNdeta_1D_MC[i] -> GetYaxis() -> SetTitle("Entry");
        dNdeta_1D_MC[i] -> GetXaxis() -> SetTitle("N track #eta");
        // dNdeta_1D_MC[i] -> GetYaxis() -> SetRangeUser(0,50);
        dNdeta_1D_MC[i] -> SetTitleSize(0.06, "X");
        dNdeta_1D_MC[i] -> SetTitleSize(0.06, "Y");
        dNdeta_1D_MC[i] -> GetXaxis() -> SetTitleOffset (0.71);
        dNdeta_1D_MC[i] -> GetYaxis() -> SetTitleOffset (1.1);
        dNdeta_1D_MC[i] -> GetXaxis() -> CenterTitle(true);
        dNdeta_1D_MC[i] -> GetYaxis() -> CenterTitle(true);

        dNdeta_1D_MC_edge_eta_cut.push_back(new TH1F("","",Eta_NBin, Eta_Min, Eta_Max));
        dNdeta_1D_MC_edge_eta_cut[i] -> SetMarkerStyle(20);
        dNdeta_1D_MC_edge_eta_cut[i] -> SetMarkerSize(0.8);
        dNdeta_1D_MC_edge_eta_cut[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_MC_edge_eta_cut[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_MC_edge_eta_cut[i] -> SetLineWidth(2);
        dNdeta_1D_MC_edge_eta_cut[i] -> GetYaxis() -> SetTitle("Entry");
        dNdeta_1D_MC_edge_eta_cut[i] -> GetXaxis() -> SetTitle("N track #eta");
        // dNdeta_1D_MC_edge_eta_cut[i] -> GetYaxis() -> SetRangeUser(0,50);
        dNdeta_1D_MC_edge_eta_cut[i] -> SetTitleSize(0.06, "X");
        dNdeta_1D_MC_edge_eta_cut[i] -> SetTitleSize(0.06, "Y");
        dNdeta_1D_MC_edge_eta_cut[i] -> GetXaxis() -> SetTitleOffset (0.71);
        dNdeta_1D_MC_edge_eta_cut[i] -> GetYaxis() -> SetTitleOffset (1.1);
        dNdeta_1D_MC_edge_eta_cut[i] -> GetXaxis() -> CenterTitle(true);
        dNdeta_1D_MC_edge_eta_cut[i] -> GetYaxis() -> CenterTitle(true);

        final_dNdeta_1D.push_back(new TH1F("","",eta_region.size() - 1, &eta_region[0]));
        final_dNdeta_1D[i] -> SetMarkerStyle(20);
        final_dNdeta_1D[i] -> SetMarkerSize(0.8);
        final_dNdeta_1D[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        final_dNdeta_1D[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        final_dNdeta_1D[i] -> SetLineWidth(2);
        final_dNdeta_1D[i] -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
        final_dNdeta_1D[i] -> GetXaxis() -> SetTitle("#eta");
        // final_dNdeta_1D[i] -> GetYaxis() -> SetRangeUser(0,50);
        final_dNdeta_1D[i] -> SetTitleSize(0.06, "X");
        final_dNdeta_1D[i] -> SetTitleSize(0.06, "Y");
        final_dNdeta_1D[i] -> GetXaxis() -> SetTitleOffset (0.71);
        final_dNdeta_1D[i] -> GetYaxis() -> SetTitleOffset (1.1);
        final_dNdeta_1D[i] -> GetXaxis() -> CenterTitle(true);
        final_dNdeta_1D[i] -> GetYaxis() -> CenterTitle(true);

        final_dNdeta_multi_1D.push_back(new TH1F("","",eta_region.size() - 1, &eta_region[0]));
        final_dNdeta_multi_1D[i] -> SetMarkerStyle(20);
        final_dNdeta_multi_1D[i] -> SetMarkerSize(0.8);
        final_dNdeta_multi_1D[i] -> SetMarkerColor(TColor::GetColor("#c48045"));
        final_dNdeta_multi_1D[i] -> SetLineColor(TColor::GetColor("#c48045"));
        final_dNdeta_multi_1D[i] -> SetLineWidth(2);
        final_dNdeta_multi_1D[i] -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
        final_dNdeta_multi_1D[i] -> GetXaxis() -> SetTitle("#eta");
        // final_dNdeta_multi_1D[i] -> GetYaxis() -> SetRangeUser(0,50);
        final_dNdeta_multi_1D[i] -> SetTitleSize(0.06, "X");
        final_dNdeta_multi_1D[i] -> SetTitleSize(0.06, "Y");
        final_dNdeta_multi_1D[i] -> GetXaxis() -> SetTitleOffset (0.71);
        final_dNdeta_multi_1D[i] -> GetYaxis() -> SetTitleOffset (1.1);
        final_dNdeta_multi_1D[i] -> GetXaxis() -> CenterTitle(true);
        final_dNdeta_multi_1D[i] -> GetYaxis() -> CenterTitle(true);

        final_dNdeta_1D_MC.push_back(new TH1F("","",eta_region.size() - 1, &eta_region[0]));
        final_dNdeta_1D_MC[i] -> SetMarkerStyle(20);
        final_dNdeta_1D_MC[i] -> SetMarkerSize(0.8);
        final_dNdeta_1D_MC[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        final_dNdeta_1D_MC[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        final_dNdeta_1D_MC[i] -> SetLineWidth(2);
        final_dNdeta_1D_MC[i] -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
        final_dNdeta_1D_MC[i] -> GetXaxis() -> SetTitle("#eta");
        // final_dNdeta_1D_MC[i] -> GetYaxis() -> SetRangeUser(0,50);
        final_dNdeta_1D_MC[i] -> SetTitleSize(0.06, "X");
        final_dNdeta_1D_MC[i] -> SetTitleSize(0.06, "Y");
        final_dNdeta_1D_MC[i] -> GetXaxis() -> SetTitleOffset (0.71);
        final_dNdeta_1D_MC[i] -> GetYaxis() -> SetTitleOffset (1.1);
        final_dNdeta_1D_MC[i] -> GetXaxis() -> CenterTitle(true);
        final_dNdeta_1D_MC[i] -> GetYaxis() -> CenterTitle(true);

        track_eta_phi_2D.push_back(new TH2F("","track_eta_phi_2D", 200, -40, 400, Eta_NBin, Eta_Min, Eta_Max));
        track_eta_phi_2D[i] -> GetXaxis() -> SetTitle("tracklet #phi");
        track_eta_phi_2D[i] -> GetYaxis() -> SetTitle("tracklet #eta");
        track_eta_phi_2D[i] -> GetXaxis() -> SetNdivisions(505);

        track_eta_z_2D.push_back(new TH2F("","track_eta_z_2D", Eta_NBin, Eta_Min, Eta_Max, 300, -420, 20));
        track_eta_z_2D[i] -> GetXaxis() -> SetTitle("tracklet #eta");
        track_eta_z_2D[i] -> GetYaxis() -> SetTitle("z vertex [mm]");
        track_eta_z_2D[i] -> GetXaxis() -> SetNdivisions(505);

        track_eta_z_2D_MC.push_back(new TH2F("","track_eta_z_2D_MC", Eta_NBin, Eta_Min, Eta_Max, 300, -420, 20));
        track_eta_z_2D_MC[i] -> GetXaxis() -> SetTitle("track #eta");
        track_eta_z_2D_MC[i] -> GetYaxis() -> SetTitle("z vertex [mm]");
        track_eta_z_2D_MC[i] -> GetXaxis() -> SetNdivisions(505);

        track_delta_eta_1D.push_back(new TH1F("","track_delta_eta_1D", 200, -1.5, 1.5));
        track_delta_eta_1D[i] -> GetXaxis() -> SetTitle("Clus #Delta#eta");
        track_delta_eta_1D[i] -> GetYaxis() -> SetTitle("Entry");
        track_delta_eta_1D[i] -> GetXaxis() -> SetNdivisions(505);

        track_delta_eta_1D_post.push_back(new TH1F("","track_delta_eta_1D_ potrack_delta_eta_1D_post", 200, -1.5, 1.5));
        track_delta_eta_1D_post[i] -> GetXaxis() -> SetTitle("Clus #Delta#eta");
        track_delta_eta_1D_post[i] -> GetYaxis() -> SetTitle("Entry");
        track_delta_eta_1D_post[i] -> GetXaxis() -> SetNdivisions(505);

        track_delta_phi_1D.push_back(new TH1F("","track_delta_phi_1D", 200, -1.5, 1.5));
        track_delta_phi_1D[i] -> GetXaxis() -> SetTitle("Clus #Delta#phi [degree]");
        track_delta_phi_1D[i] -> GetYaxis() -> SetTitle("Entry");
        track_delta_phi_1D[i] -> GetXaxis() -> SetNdivisions(505);

        track_DCA_distance.push_back(new TH1F("","track_DCA_distance", 200, -5, 5));
        track_DCA_distance[i] -> GetXaxis() -> SetTitle("DCA distance [mm]");
        track_DCA_distance[i] -> GetYaxis() -> SetTitle("Entry");
        track_DCA_distance[i] -> GetXaxis() -> SetNdivisions(505);

        track_phi_DCA_2D.push_back(new TH2F("","track_phi_DCA_2D", 200, -1.5, 1.5, 200, -3, 3));
        track_phi_DCA_2D[i] -> GetXaxis() -> SetTitle("Clus #Delta#phi [degree]");
        track_phi_DCA_2D[i] -> GetYaxis() -> SetTitle("DCA distance [mm]");
        track_phi_DCA_2D[i] -> GetXaxis() -> SetNdivisions(505);

        track_ratio_1D.push_back(new TH1F("","track_ratio_1D", 200, 0, 1.5));
        track_ratio_1D[i] -> GetXaxis() -> SetTitle("N reco track / N true track");
        track_ratio_1D[i] -> GetYaxis() -> SetTitle("Entry");
        track_ratio_1D[i] -> GetXaxis() -> SetNdivisions(505);

        track_DeltaPhi_eta_2D.push_back(new TH2F("","track_DeltaPhi_eta_2D", 200, -1.5, 1.5, 200, -4, 4));
        track_DeltaPhi_eta_2D[i] -> GetXaxis() -> SetTitle("Clus #Delta#phi [degree]");
        track_DeltaPhi_eta_2D[i] -> GetYaxis() -> SetTitle("Tracklet #eta");
        track_DeltaPhi_eta_2D[i] -> GetXaxis() -> SetNdivisions(505);

        track_DeltaPhi_DeltaEta_2D.push_back(new TH2F("","track_DeltaPhi_DeltaEta_2D", 200, -1.5, 1.5, 200, -1, 1));
        track_DeltaPhi_DeltaEta_2D[i] -> GetXaxis() -> SetTitle("Clus #Delta#phi [degree]");
        track_DeltaPhi_DeltaEta_2D[i] -> GetYaxis() -> SetTitle("#Delta#eta");
        track_DeltaPhi_DeltaEta_2D[i] -> GetXaxis() -> SetNdivisions(505);

        final_track_delta_phi_1D[i].clear();
        final_track_multi_delta_phi_1D[i].clear();
        // for (int eta_i = 0; eta_i < eta_region.size() - 1; eta_i++)
        // note : now we try to do something inclusive, which means we open the z selection, try to have everything, and then we decide afterwards
        for (int eta_i = 0; eta_i < (coarse_eta_z_map -> GetNbinsX() * coarse_eta_z_map -> GetNbinsY()); eta_i++)
        {
            final_track_delta_phi_1D[i].push_back(new TH1F("","final_track_delta_phi_1D", 100, -3, 3));
            final_track_delta_phi_1D[i][eta_i] -> GetXaxis() -> SetTitle("Clus #Delta#phi [degree]");
            final_track_delta_phi_1D[i][eta_i] -> GetYaxis() -> SetTitle("Entry");
            final_track_delta_phi_1D[i][eta_i] -> GetXaxis() -> SetNdivisions(505);

            final_track_multi_delta_phi_1D[i].push_back(new TH1F("","final_track_multi_delta_phi_1D", 100, -3, 3));
            final_track_multi_delta_phi_1D[i][eta_i] -> GetXaxis() -> SetTitle("Clus #Delta#phi [degree]");
            final_track_multi_delta_phi_1D[i][eta_i] -> GetYaxis() -> SetTitle("Entry");
            final_track_multi_delta_phi_1D[i][eta_i] -> GetXaxis() -> SetNdivisions(505);
        }
    }
    
    mega_track_contamination_2D = new TH2F("","", 200,0,8500, 200, 0, 5);
    mega_track_contamination_2D -> GetXaxis() -> SetTitle("INTT NClus");
    mega_track_contamination_2D -> GetYaxis() -> SetTitle("Mega Track contamination ratio [%]");
    mega_track_contamination_2D -> GetXaxis() -> SetNdivisions(505);

    mega_track_finding_ratio_2D = new TH2F("","", 200,0,8500, 200, 0, 2);
    mega_track_finding_ratio_2D -> GetXaxis() -> SetTitle("INTT NClus");
    mega_track_finding_ratio_2D -> GetYaxis() -> SetTitle("Mega Track finding ratio");
    mega_track_finding_ratio_2D -> GetXaxis() -> SetNdivisions(505);

    MBin_Z_evt_hist_2D = new TH2F("","", centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]);
    MBin_Z_evt_hist_2D -> GetXaxis() -> SetTitle("Centrality bin");
    MBin_Z_evt_hist_2D -> GetYaxis() -> SetTitle("Z vertex [mm]");
    MBin_Z_evt_hist_2D -> GetXaxis() -> SetNdivisions(505); 

    MBin_Z_evt_hist_2D_MC = new TH2F("","", centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]);
    MBin_Z_evt_hist_2D_MC -> GetXaxis() -> SetTitle("Centrality bin");
    MBin_Z_evt_hist_2D_MC -> GetYaxis() -> SetTitle("Z vertex [mm]");
    MBin_Z_evt_hist_2D_MC -> GetXaxis() -> SetNdivisions(505); 


    track_cluster_ratio_multiplicity_2D = new TH2F("","track_cluster_ratio_multiplicity_2D", 200, 0, 9000, 200, 0, 15);
    track_cluster_ratio_multiplicity_2D -> GetXaxis() -> SetTitle("NClus");
    track_cluster_ratio_multiplicity_2D -> GetYaxis() -> SetTitle("NClus / NTracks");
    track_cluster_ratio_multiplicity_2D -> GetXaxis() -> SetNdivisions(505);

    track_cluster_ratio_multiplicity_2D_MC = new TH2F("","track_cluster_ratio_multiplicity_2D_MC", 200, 0, 9000, 200, 0, 15);
    track_cluster_ratio_multiplicity_2D_MC -> GetXaxis() -> SetTitle("NClus");
    track_cluster_ratio_multiplicity_2D_MC -> GetYaxis() -> SetTitle("NClus / NTracks");
    track_cluster_ratio_multiplicity_2D_MC -> GetXaxis() -> SetNdivisions(505);

    track_correlation_2D = new TH2F("","track_correlation_2D", 200, 0, 3000, 200, 0, 3000);
    track_correlation_2D -> GetXaxis() -> SetTitle("N true track");
    track_correlation_2D -> GetYaxis() -> SetTitle("N reco track");
    track_correlation_2D -> GetXaxis() -> SetNdivisions(505);

    track_ratio_2D = new TH2F("","track_ratio_2D", 200, 0, 3000, 200, 0, 2);
    track_ratio_2D -> GetXaxis() -> SetTitle("N true track");
    track_ratio_2D -> GetYaxis() -> SetTitle("N reco track/N true track");
    track_ratio_2D -> GetXaxis() -> SetNdivisions(505);

    check_inner_layer_clu_phi_1D = new TH1F("","check_inner_layer_clu_phi_1D", 440, -40, 400);
    check_inner_layer_clu_phi_1D -> GetXaxis() -> SetTitle("Clus #phi [degree]");
    check_inner_layer_clu_phi_1D -> GetYaxis() -> SetTitle("Entry");
    // check_inner_layer_clu_phi_1D -> GetXaxis() -> SetNdivisions(505);

    check_outer_layer_clu_phi_1D = new TH1F("","check_outer_layer_clu_phi_1D", 440, -40, 400);
    check_outer_layer_clu_phi_1D -> GetXaxis() -> SetTitle("Clus #phi [degree]");
    check_outer_layer_clu_phi_1D -> GetYaxis() -> SetTitle("Entry");
    // check_outer_layer_clu_phi_1D -> GetXaxis() -> SetNdivisions(505);

    reco_eta_correlation_2D = new TH2F("","reco_eta_correlation_2D", 200, -3, 3, 200, -3, 3);
    reco_eta_correlation_2D -> GetXaxis() -> SetTitle("Reco 3P #eta");
    reco_eta_correlation_2D -> GetYaxis() -> SetTitle("Reco 2P #eta");
    reco_eta_correlation_2D -> GetXaxis() -> SetNdivisions(505);

    reco_eta_diff_reco3P_2D = new TH2F("","reco_eta_diff_reco3P_2D", 200, -3, 3, 200, -0.35, 0.35);
    reco_eta_diff_reco3P_2D -> GetXaxis() -> SetTitle("Reco 3P #eta");
    reco_eta_diff_reco3P_2D -> GetYaxis() -> SetTitle("Reco #eta diff");
    reco_eta_diff_reco3P_2D -> GetXaxis() -> SetNdivisions(505);

    reco_eta_diff_1D = new TH1F("","reco_eta_diff_1D", 200, -0.35, 0.35);
    reco_eta_diff_1D -> GetXaxis() -> SetTitle("Reco #eta diff");
    reco_eta_diff_1D -> GetYaxis() -> SetTitle("Entry");
    reco_eta_diff_1D -> GetXaxis() -> SetNdivisions(505);

    clu_used_centrality_2D = new TH2F("","clu_used_centrality_2D", 200, 0, 8500, 10, 0, 10);
    clu_used_centrality_2D -> GetXaxis() -> SetTitle("NClus");
    clu_used_centrality_2D -> GetYaxis() -> SetTitle("N used each clu");
    clu_used_centrality_2D -> GetXaxis() -> SetNdivisions(505);

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
    
    exclusive_tight_tracklet_eta = new TH1F("","exclusive_tight_tracklet_eta;Tracklet #eta;Entry",50,-3.5,3.5);
    exclusive_tight_tracklet_phi = new TH1F("","exclusive_tight_tracklet_phi;Tracklet #phi [radian];Entry",50,-3.5,3.5);
    exclusive_loose_tracklet_eta = new TH1F("","exclusive_loose_tracklet_eta;Tracklet #eta;Entry",50,-3.5,3.5);
    exclusive_loose_tracklet_phi = new TH1F("","exclusive_loose_tracklet_phi;Tracklet #phi [radian];Entry",50,-3.5,3.5);

    eta_region_hist = new TH1F("","", eta_region.size() - 1, &eta_region[0]);

    cout<<" running ? in INTTEta, InitHist"<<endl;

}

void INTTEta::InitGraph()
{
    track_gr = new TGraphErrors();

    evt_reco_track_gr_All = new TGraph();
    evt_reco_track_gr_All -> SetMarkerStyle(5);
    evt_reco_track_gr_All -> SetMarkerSize(1);
    evt_reco_track_gr_All -> SetMarkerColor(4);

    evt_reco_track_gr_PhiLoose = new TGraph();
    evt_reco_track_gr_PhiLoose -> SetMarkerStyle(5);
    evt_reco_track_gr_PhiLoose -> SetMarkerSize(1);
    evt_reco_track_gr_PhiLoose -> SetMarkerColor(4);

    evt_reco_track_gr_Z = new TGraph();
    evt_reco_track_gr_Z -> SetMarkerStyle(5);
    evt_reco_track_gr_Z -> SetMarkerSize(1);
    evt_reco_track_gr_Z -> SetMarkerColor(4);
    
    evt_reco_track_gr_ZDCA = new TGraph();
    evt_reco_track_gr_ZDCA -> SetMarkerStyle(5);
    evt_reco_track_gr_ZDCA -> SetMarkerSize(1);
    evt_reco_track_gr_ZDCA -> SetMarkerColor(4);
    
    evt_reco_track_gr_ZDCAPhi = new TGraph();
    evt_reco_track_gr_ZDCAPhi -> SetMarkerStyle(5);
    evt_reco_track_gr_ZDCAPhi -> SetMarkerSize(1);
    evt_reco_track_gr_ZDCAPhi -> SetMarkerColor(4);

    evt_true_track_gr = new TGraph();
    evt_true_track_gr -> SetMarkerStyle(20);
    evt_true_track_gr -> SetMarkerSize(1);
    // evt_true_track_gr -> SetMarkerColor(2);
    evt_true_track_gr -> SetMarkerColorAlpha(2,0.5);


    cout<<" running ? "<<endl;
    return;
}

void INTTEta::InitTree()
{
    out_file = new TFile(Form("%s/INTT_final_hist_info.root",out_folder_directory.c_str()),"RECREATE");
    tree_out =  new TTree ("tree_eta", "Tracklet eta info.");
    
    tree_out -> Branch("eID",&out_eID);
    tree_out -> Branch("Evt_centrality_bin",&out_evt_centrality_bin);
    tree_out -> Branch("Evt_zvtx", &out_evt_zvtx);
    tree_out -> Branch("True_zvtx", &out_true_zvtx);
    tree_out -> Branch("NTrueTrack", &out_NTrueTrack);
    tree_out -> Branch("NClus", &out_total_NClus);
    tree_out -> Branch("RecoTrack_eta_d", &out_recotrack_eta_d);
    tree_out -> Branch("RecoTrack_phi_d", &out_recotrack_phi_d);
    tree_out -> Branch("TrueTrack_eta_d", &out_truetrack_eta_d);
    tree_out -> Branch("TrueTrack_phi_d", &out_truetrack_phi_d);
    // tree_out -> Branch("RecoTrack_eta_i", &out_track_eta_i);
    // tree_out -> Branch("Track_delta_phi_d", &out_track_delta_phi_d);
    tree_out -> Branch("N2Clu_track", &out_N2Clu_track);
    tree_out -> Branch("N3Clu_track", &out_N3Clu_track);
    tree_out -> Branch("N4Clu_track", &out_N4Clu_track);
    
    return;
}

void INTTEta::print_evt_plot(int event_i, int NTrueTrack, int innerNClu, int outerNClu)
{   
    c1 -> Clear();
    c1 -> Print( Form("%s/evt_track_eID_%i.pdf(", out_folder_directory_evt.c_str(), event_i) );
    c1 -> cd();

    evt_reco_track_gr_All -> GetXaxis() -> SetTitle("#eta");
    evt_reco_track_gr_All -> GetYaxis() -> SetTitle("#phi");
    evt_reco_track_gr_All -> GetXaxis() -> SetLimits(-3.5, 3.5);
    evt_reco_track_gr_All -> GetYaxis() -> SetRangeUser(-10, 420);
    evt_reco_track_gr_All -> GetXaxis() -> SetNdivisions(505);
    evt_reco_track_gr_All -> Draw("ap");
    evt_true_track_gr -> Draw("p same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.90, Form("NTrueTrack: %i, innerNClu: %i, outerNClu: %i", NTrueTrack, innerNClu, outerNClu));
    draw_text -> DrawLatex(0.21, 0.85, Form("NReco_tracklet: %i", evt_reco_track_gr_All->GetN()));
    coord_line -> DrawLine(-3.5, 0, 3.5, 0);
    coord_line -> DrawLine(-3.5, 360, 3.5, 360);
    c1 -> Print( Form("%s/evt_track_eID_%i.pdf", out_folder_directory_evt.c_str(), event_i) );
    c1 -> Clear();

    evt_reco_track_gr_PhiLoose -> GetXaxis() -> SetTitle("#eta");
    evt_reco_track_gr_PhiLoose -> GetYaxis() -> SetTitle("#phi");
    evt_reco_track_gr_PhiLoose -> GetXaxis() -> SetLimits(-3.5, 3.5);
    evt_reco_track_gr_PhiLoose -> GetYaxis() -> SetRangeUser(-10, 420);
    evt_reco_track_gr_PhiLoose -> GetXaxis() -> SetNdivisions(505);
    evt_reco_track_gr_PhiLoose -> Draw("ap");
    evt_true_track_gr -> Draw("p same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.90, Form("NTrueTrack: %i, innerNClu: %i, outerNClu: %i", NTrueTrack, innerNClu, outerNClu));
    draw_text -> DrawLatex(0.21, 0.85, Form("NReco_tracklet: %i", evt_reco_track_gr_PhiLoose->GetN()));
    coord_line -> DrawLine(-3.5, 0, 3.5, 0);
    coord_line -> DrawLine(-3.5, 360, 3.5, 360);
    c1 -> Print( Form("%s/evt_track_eID_%i.pdf", out_folder_directory_evt.c_str(), event_i) );
    c1 -> Clear();

    evt_reco_track_gr_Z -> GetXaxis() -> SetTitle("#eta");
    evt_reco_track_gr_Z -> GetYaxis() -> SetTitle("#phi");
    evt_reco_track_gr_Z -> GetXaxis() -> SetLimits(-3.5, 3.5);
    evt_reco_track_gr_Z -> GetYaxis() -> SetRangeUser(-10, 420);
    evt_reco_track_gr_Z -> GetXaxis() -> SetNdivisions(505);
    evt_reco_track_gr_Z -> Draw("ap");
    evt_true_track_gr -> Draw("p same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.90, Form("NTrueTrack: %i, innerNClu: %i, outerNClu: %i", NTrueTrack, innerNClu, outerNClu));
    draw_text -> DrawLatex(0.21, 0.85, Form("NReco_tracklet: %i", evt_reco_track_gr_Z->GetN()));
    coord_line -> DrawLine(-3.5, 0, 3.5, 0);
    coord_line -> DrawLine(-3.5, 360, 3.5, 360);
    c1 -> Print( Form("%s/evt_track_eID_%i.pdf", out_folder_directory_evt.c_str(), event_i) );
    c1 -> Clear();

    evt_reco_track_gr_ZDCA -> GetXaxis() -> SetTitle("#eta");
    evt_reco_track_gr_ZDCA -> GetYaxis() -> SetTitle("#phi");
    evt_reco_track_gr_ZDCA -> GetXaxis() -> SetLimits(-3.5, 3.5);
    evt_reco_track_gr_ZDCA -> GetYaxis() -> SetRangeUser(-10, 420);
    evt_reco_track_gr_ZDCA -> GetXaxis() -> SetNdivisions(505);
    evt_reco_track_gr_ZDCA -> Draw("ap");
    evt_true_track_gr -> Draw("p same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.90, Form("NTrueTrack: %i, innerNClu: %i, outerNClu: %i", NTrueTrack, innerNClu, outerNClu));
    draw_text -> DrawLatex(0.21, 0.85, Form("NReco_tracklet: %i", evt_reco_track_gr_ZDCA->GetN()));
    coord_line -> DrawLine(-3.5, 0, 3.5, 0);
    coord_line -> DrawLine(-3.5, 360, 3.5, 360);
    c1 -> Print( Form("%s/evt_track_eID_%i.pdf", out_folder_directory_evt.c_str(), event_i) );
    c1 -> Clear();

    evt_reco_track_gr_ZDCAPhi -> GetXaxis() -> SetTitle("#eta");
    evt_reco_track_gr_ZDCAPhi -> GetYaxis() -> SetTitle("#phi");
    evt_reco_track_gr_ZDCAPhi -> GetXaxis() -> SetLimits(-3.5, 3.5);
    evt_reco_track_gr_ZDCAPhi -> GetYaxis() -> SetRangeUser(-10, 420);
    evt_reco_track_gr_ZDCAPhi -> GetXaxis() -> SetNdivisions(505);
    evt_reco_track_gr_ZDCAPhi -> Draw("ap");
    evt_true_track_gr -> Draw("p same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.90, Form("NTrueTrack: %i, innerNClu: %i, outerNClu: %i", NTrueTrack, innerNClu, outerNClu));
    draw_text -> DrawLatex(0.21, 0.85, Form("NReco_tracklet: %i", evt_reco_track_gr_ZDCAPhi->GetN()));
    coord_line -> DrawLine(-3.5, 0, 3.5, 0);
    coord_line -> DrawLine(-3.5, 360, 3.5, 360);
    c1 -> Print( Form("%s/evt_track_eID_%i.pdf)", out_folder_directory_evt.c_str(), event_i) );
    c1 -> Clear();
}

// // note : this function is for the event by event vertex calculation
// // note : this function is the old method, which means there are two for loops that checks all the combinations, it takes a lot of time
// void INTTEta::ProcessEvt(int event_i, vector<clu_info> temp_sPH_inner_nocolumn_vec, vector<clu_info> temp_sPH_outer_nocolumn_vec, vector<vector<double>> temp_sPH_nocolumn_vec, vector<vector<double>> temp_sPH_nocolumn_rz_vec, int NvtxMC, vector<double> TrigvtxMC, bool PhiCheckTag, Long64_t bco_full, pair<double,double> evt_z, int centrality_bin, vector<vector<float>> true_track_info) // note : evt_z : {z, width} 
// {   
//     return_tag = 0;

//     if (event_i%1 == 0) {cout<<"In INTTEta class, running event : "<<event_i<<endl;}

//     inner_NClu = temp_sPH_inner_nocolumn_vec.size();
//     outer_NClu = temp_sPH_outer_nocolumn_vec.size();
//     total_NClus = inner_NClu + outer_NClu;

//     // cout<<"test_0"<<endl;
//     if (total_NClus < zvtx_cal_require) {return; cout<<"return confirmation"<<endl;}   
    
//     if (run_type == "MC" && NvtxMC != 1) { return; cout<<"In INTTEta class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Nvtx more than one "<<endl;}
//     if (PhiCheckTag == false)            { return; cout<<"In INTTEta class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Not full phi has hits "<<endl;}
    
//     if (inner_NClu < 10 || outer_NClu < 10 || total_NClus > N_clu_cut || total_NClus < N_clu_cutl)
//     {
//         return;
//         printf("In INTTEta class, event : %i, low clu continue, NClus : %lu \n", event_i, total_NClus); 
//     }

//     // todo : the z vertex range is here
//     if (-220 > evt_z.first + evt_z.second || -180 < evt_z.first - evt_z.second) {return;}
    
    
//     N_GoodEvent += 1;
//     N_GoodEvent_vec[centrality_map[centrality_bin]] += 1;

//     // cout<<"N inner cluster : "<<inner_NClu<<" N outer cluster : "<<outer_NClu<<endl;

//     if (run_type == "MC")
//     {
//         // note : for the true track case ----------------------------------------------------------------------------------------------------------------------------------------------------------------
//         double INTT_eta_acceptance_l = -0.5 * TMath::Log((sqrt(pow(-230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))-(-230.-TrigvtxMC[2]*10.)) / (sqrt(pow(-230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))+(-230.-TrigvtxMC[2]*10.))); // note : left
//         double INTT_eta_acceptance_r =  -0.5 * TMath::Log((sqrt(pow(230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))-(230.-TrigvtxMC[2]*10.)) / (sqrt(pow(230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))+(230.-TrigvtxMC[2]*10.))); // note : right
//         // if (event_i % 100 == 0){cout<<"z : "<<TrigvtxMC[2]*10.<<" eta : "<<INTT_eta_acceptance_l<<" "<<INTT_eta_acceptance_r<<endl;}

//         // cout<<"true_track_info : "<<true_track_info.size()<<endl;    
//         for (int track_i = 0; track_i < true_track_info.size(); track_i++)
//         {
//             if (true_track_info[track_i][2] == 111 || true_track_info[track_i][2] == 22 || abs(true_track_info[track_i][2]) == 2112){continue;}

//             if (true_track_info[track_i][0] > INTT_eta_acceptance_l && true_track_info[track_i][0] < INTT_eta_acceptance_r)
//             {
//                 dNdeta_1D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0]);
//                 dNdeta_1D_MC[dNdeta_1D_MC.size() - 1]        -> Fill(true_track_info[track_i][0]);   
//                 final_dNdeta_1D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0]);
//                 final_dNdeta_1D_MC[dNdeta_1D_MC.size() - 1]        -> Fill(true_track_info[track_i][0]);
//                 track_eta_z_2D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);
//                 track_eta_z_2D_MC[track_eta_z_2D_MC.size() - 1]   -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);    

//                 // cout<<"true track eta : "<<true_track_info[track_i][0]<<" phi : "<<convertTo360(true_track_info[track_i][1])<<endl;
//                 // cout<<"("<<true_track_info[track_i][0]<<", "<<convertTo360(true_track_info[track_i][1])<<"), ";

//                 evt_true_track_gr -> SetPoint(evt_true_track_gr->GetN(),true_track_info[track_i][0], convertTo360(true_track_info[track_i][1]));

//                 evt_NTrack_MC += 1;
//             }
//         }
//         // if (evt_NTrack_MC < 10) {cout<<"evt : "<<event_i<<" ---- N reco track : "<<evt_NTrack<<" N true track : "<<evt_NTrack_MC<<" ratio : "<<double(evt_NTrack) / double(evt_NTrack_MC)<<endl;}
//     }


//     // cout<<"test_1"<<endl;
//     for ( int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++ )
//     {    
//         loop_NGoodPair = 0;
        
//         for ( int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++ )
//         {
//             // cout<<event_i<<" test_5 "<<inner_i<<" "<<outer_i<<endl;
//             // note : calculate the cluster phi after the vertex correction which can enhence the purity of the tracklet selection
//             Clus_InnerPhi_Offset = (temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second < 0) ? atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi());
//             Clus_OuterPhi_Offset = (temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second < 0) ? atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi());
//             double delta_phi = Clus_InnerPhi_Offset - Clus_OuterPhi_Offset;
//             double track_phi = (Clus_InnerPhi_Offset + Clus_OuterPhi_Offset)/2.;
//             double inner_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y, temp_sPH_inner_nocolumn_vec[inner_i].z});
//             double outer_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y, temp_sPH_outer_nocolumn_vec[outer_i].z});
//             double delta_eta = inner_clu_eta - outer_clu_eta;
            
//             // cout<<"test_2"<<endl;
//             // note : before calculating the possible z vertex range of one tracklet, the vertex correction was applied
//             pair<double,double> z_range_info = Get_possible_zvtx( 
//                 0., // get_radius(beam_origin.first,beam_origin.second), 
//                 {get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first, temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second), temp_sPH_inner_nocolumn_vec[inner_i].z}, // note : unsign radius
//                 {get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first, temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second), temp_sPH_outer_nocolumn_vec[outer_i].z}  // note : unsign radius
//             );

//             if (run_type == "MC" && evt_NTrack_MC < draw_evt_cut && event_i % print_plot_ratio == 0)
//             {
//                 Get_eta_pair = Get_eta(
//                     {get_radius(beam_origin.first,beam_origin.second), evt_z.first,evt_z.second},
//                     {get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first, temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second), temp_sPH_inner_nocolumn_vec[inner_i].z},
//                     {get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first, temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second), temp_sPH_outer_nocolumn_vec[outer_i].z}
//                 );

//                 evt_reco_track_gr_All -> SetPoint(evt_reco_track_gr_All->GetN(),Get_eta_pair.second, track_phi);
//             }

            
//             // if ( Get_eta_pair.second > 1.4 && Get_eta_pair.second < 1.7 && track_phi > 50 && track_phi < 60 ) {
//             //     cout<<" "<<endl;
//             //     cout<<"reco eta : "<<Get_eta_pair.second<<" reco phi : "<<track_phi<<" delta eta : "<<delta_eta<<endl;
//             //     cout<<"all pair info, inner clu pos "<<temp_sPH_inner_nocolumn_vec[inner_i].x<<" "<<temp_sPH_inner_nocolumn_vec[inner_i].y<<" "<<temp_sPH_inner_nocolumn_vec[inner_i].z<<" phi : "<<Clus_InnerPhi_Offset<<endl;
//             //     cout<<"all pair info, outer clu pos "<<temp_sPH_outer_nocolumn_vec[outer_i].x<<" "<<temp_sPH_outer_nocolumn_vec[outer_i].y<<" "<<temp_sPH_outer_nocolumn_vec[outer_i].z<<" phi : "<<Clus_OuterPhi_Offset<<endl;
//             //     cout<<"z vertex range : "<<z_range_info.first - z_range_info.second<<" "<<z_range_info.first + z_range_info.second<<endl;
//             //     cout<<"accept z range : "<<evt_z.first - evt_z.second<<" "<<evt_z.first + evt_z.second<<endl;
                
//             // }

//             if (fabs(delta_phi) > 5.72) {continue;}
//             if (run_type == "MC" && evt_NTrack_MC < draw_evt_cut && event_i % print_plot_ratio == 0) {evt_reco_track_gr_PhiLoose -> SetPoint(evt_reco_track_gr_PhiLoose->GetN(),Get_eta_pair.second, track_phi);}

//             track_delta_eta_1D[centrality_map[centrality_bin]] -> Fill( delta_eta );
//             track_delta_eta_1D[track_delta_eta_1D.size() - 1]  -> Fill( delta_eta );

//             track_DeltaPhi_DeltaEta_2D[centrality_map[centrality_bin]]        -> Fill(delta_phi, delta_eta);
//             track_DeltaPhi_DeltaEta_2D[track_DeltaPhi_DeltaEta_2D.size() - 1] -> Fill(delta_phi, delta_eta);

//             // cout<<event_i<<" test_6 "<<inner_i<<" "<<outer_i<<endl;
//             // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
//             if (z_range_info.first - z_range_info.second > evt_z.first + evt_z.second || z_range_info.first + z_range_info.second < evt_z.first - evt_z.second) {continue;}
//             // cout<<"range test: "<<z_range_info.first - z_range_info.second<<" "<<z_range_info.first + z_range_info.second<<" vertex: "<<evt_z.first<<" "<<evt_z.second<<endl;
//             // cout<<"test_3"<<endl;

//             if (run_type == "MC" && evt_NTrack_MC < draw_evt_cut && event_i % print_plot_ratio == 0) {evt_reco_track_gr_Z -> SetPoint(evt_reco_track_gr_Z->GetN(),Get_eta_pair.second, track_phi);}
            
//             // note : the reason to use the DCA calculation with sign is because that the distribution of 1D DCA will be single peak only
//             double DCA_sign = calculateAngleBetweenVectors(
//                 temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
//                 temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
//                 beam_origin.first, beam_origin.second
//             );

//             // vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
//             //     temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
//             //     temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
//             //     beam_origin.first, beam_origin.second
//             // );
//             // if (DCA_info_vec[0] != fabs(DCA_sign) && fabs( DCA_info_vec[0] - fabs(DCA_sign) ) > 0.1 ){
//             //     cout<<"different DCA : "<<DCA_info_vec[0]<<" "<<DCA_sign<<" diff : "<<DCA_info_vec[0] - fabs(DCA_sign)<<endl;
//             // }

//             track_delta_eta_1D_post[centrality_map[centrality_bin]]     -> Fill( delta_eta );
//             track_delta_eta_1D_post[track_delta_eta_1D_post.size() - 1] -> Fill( delta_eta );

//             track_delta_phi_1D[centrality_map[centrality_bin]] -> Fill( delta_phi );
//             track_delta_phi_1D[track_delta_phi_1D.size() - 1]  -> Fill( delta_phi );
            
//             track_DCA_distance[centrality_map[centrality_bin]] -> Fill( DCA_sign );
//             track_DCA_distance[track_DCA_distance.size() - 1]  -> Fill( DCA_sign ); 

//             track_phi_DCA_2D[centrality_map[centrality_bin]] -> Fill( delta_phi, DCA_sign );
//             track_phi_DCA_2D[track_phi_DCA_2D.size() - 1]    -> Fill( delta_phi, DCA_sign );

//             // cout<<" "<<endl;
//             // cout<<"inner_i : "<<inner_i<<" outer_i "<<outer_i<<" true z : "<<TrigvtxMC[2]*10.<<" reco z: "<<evt_z.first<<endl;
//             // cout<<"all pair info, inner clu pos "<<temp_sPH_inner_nocolumn_vec[inner_i].x<<" "<<temp_sPH_inner_nocolumn_vec[inner_i].y<<" "<<temp_sPH_inner_nocolumn_vec[inner_i].z<<" phi : "<<Clus_InnerPhi_Offset<<endl;
//             // cout<<"all pair info, outer clu pos "<<temp_sPH_outer_nocolumn_vec[outer_i].x<<" "<<temp_sPH_outer_nocolumn_vec[outer_i].y<<" "<<temp_sPH_outer_nocolumn_vec[outer_i].z<<" phi : "<<Clus_OuterPhi_Offset<<endl;
//             // if (fabs(delta_phi) < 4) {cout<<"("<<Get_eta_pair.second<<", "<<track_phi<<"), ";}

//             // if (DCA_cut.first < DCA_sign && DCA_sign < DCA_cut.second)
//             if (true)
//             {
//                 if (run_type == "MC" && evt_NTrack_MC < draw_evt_cut && event_i % print_plot_ratio == 0) {evt_reco_track_gr_ZDCA -> SetPoint(evt_reco_track_gr_ZDCA->GetN(),Get_eta_pair.second, track_phi);}

//                 if (fabs(delta_phi) < phi_diff_cut)
//                 {
//                     if (run_type == "MC" && evt_NTrack_MC < draw_evt_cut && event_i % print_plot_ratio == 0) {evt_reco_track_gr_ZDCAPhi -> SetPoint(evt_reco_track_gr_ZDCAPhi->GetN(),Get_eta_pair.second, track_phi);}    
//                     // cout<<"reco eta : "<<Get_eta_pair.second<<" reco phi : "<<track_phi<<endl;

//                     // if (fabs(Get_eta_pair.second) < 0.1) { cout<<"eta "<<Get_eta_pair.second<<", three points : "<<get_radius(beam_origin.first,beam_origin.second)<<" "<<evt_z.first<<" "<<evt_z.second<<" "<<
//                     // get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first, temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second)<<" "<<temp_sPH_inner_nocolumn_vec[inner_i].z<<" "<<
//                     // get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first, temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second)<<" "<<temp_sPH_outer_nocolumn_vec[outer_i].z<<endl;}

//                     Get_eta_pair = Get_eta(
//                         {get_radius(beam_origin.first,beam_origin.second), evt_z.first,evt_z.second},
//                         {get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first, temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second), temp_sPH_inner_nocolumn_vec[inner_i].z},
//                         {get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first, temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second), temp_sPH_outer_nocolumn_vec[outer_i].z}
//                     );

//                     double eta_bin = eta_region_hist -> Fill(Get_eta_pair.second);
//                     // cout<<"test1 "<<eta_bin<<endl;
//                     if (eta_bin != -1)
//                     {
//                         final_track_delta_phi_1D[centrality_map[centrality_bin]][eta_bin - 1]      -> Fill(delta_phi);
//                         final_track_delta_phi_1D[final_track_delta_phi_1D.size() - 1][eta_bin - 1] -> Fill(delta_phi);
//                         out_track_delta_phi_d.push_back(delta_phi);
//                         out_recotrack_eta_d.push_back(Get_eta_pair.second);
//                         out_track_eta_i.push_back(eta_bin);
//                     }
                    

//                     reco_eta_correlation_2D -> Fill(Get_eta_pair.second, (inner_clu_eta + outer_clu_eta)/2.);
//                     reco_eta_diff_reco3P_2D -> Fill(Get_eta_pair.second, Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);
//                     reco_eta_diff_1D        -> Fill(Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);

//                     track_DeltaPhi_eta_2D[centrality_map[centrality_bin]]   -> Fill(delta_phi, Get_eta_pair.second);
//                     track_DeltaPhi_eta_2D[track_DeltaPhi_eta_2D.size() - 1] -> Fill(delta_phi, Get_eta_pair.second);

//                     // cout<<"test_5"<<endl;
//                     dNdeta_1D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second);
//                     dNdeta_1D[dNdeta_1D.size() - 1]           -> Fill(Get_eta_pair.second);

//                     track_eta_phi_2D[centrality_map[centrality_bin]] -> Fill(track_phi, Get_eta_pair.second);
//                     track_eta_phi_2D[track_eta_phi_2D.size() - 1]    -> Fill(track_phi, Get_eta_pair.second);
                    
//                     track_eta_z_2D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second, evt_z.first);
//                     track_eta_z_2D[track_eta_z_2D.size() - 1]      -> Fill(Get_eta_pair.second, evt_z.first);
            
//                     evt_NTrack += 1;
//                     // cout<<event_i<<" test_10 "<<inner_i<<" "<<outer_i<<endl;
//                 }

//             }
            
//         }
//     }

//     // note : prepare the tree out
//     out_eID = event_i;
//     out_evt_centrality_bin = centrality_bin;
//     out_evt_zvtx = evt_z.first;
//     tree_out -> Fill();

//     // cout<<" "<<endl;
//     // cout<<" "<<endl;
//     // cout<<"test_8"<<endl;
//     if (run_type == "MC")
//     {
//         track_correlation_2D -> Fill(evt_NTrack_MC, evt_NTrack);
//         track_ratio_1D[centrality_map[centrality_bin]] -> Fill( double(evt_NTrack) / double(evt_NTrack_MC) );
//         track_ratio_1D[track_ratio_1D.size() - 1]      -> Fill( double(evt_NTrack) / double(evt_NTrack_MC) );
//     }
        
//     track_cluster_ratio_multiplicity_2D -> Fill( total_NClus, double(total_NClus) / double(evt_NTrack) );
//     track_cluster_ratio_1D[centrality_map[centrality_bin]]    -> Fill( double(total_NClus) / double(evt_NTrack) );
//     track_cluster_ratio_1D[track_cluster_ratio_1D.size() - 1] -> Fill( double(total_NClus) / double(evt_NTrack) );

//     if (run_type == "MC" && evt_NTrack_MC < draw_evt_cut && event_i % print_plot_ratio == 0){print_evt_plot(event_i, evt_NTrack_MC, inner_NClu, outer_NClu);}

//     return_tag = 1;
// }

// note : this function is for the event by event vertex calculation
// note : this function is the new method, which means that we first put the cluster into a phi map, and then there are two for loops still, but we only check the certain range of the phi 
// note : this function requires that one cluster can only be used once for single tracklet
// note : so maybe there should be no background subtraction for this case
void INTTEta::ProcessEvt(
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
{ // note : evt_z : {z, width}
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

    // todo : the z vertex range is here
    // if (GetTH2BinXY(coarse_eta_z_map->GetNbinsX(), coarse_eta_z_map->GetNbinsY(), coarse_eta_z_map -> Fill(0., evt_z.first)).second == -1) {return;}
    // if (-220 > evt_z.first + evt_z.second || -180 < evt_z.first - evt_z.second) {return;}
    // if (-100 > evt_z.first + evt_z.second || 100 < evt_z.first - evt_z.second) {return;}
    
    // note : for data, the denominator
    MBin_Z_evt_hist_2D -> Fill(centrality_map[centrality_bin], evt_z.first, zvtx_weighting); 
    MBin_Z_evt_hist_2D -> Fill(MBin_Z_evt_hist_2D -> GetNbinsX()-1, evt_z.first, zvtx_weighting); 
    
    N_GoodEvent += 1;
    N_GoodEvent_vec[centrality_map[centrality_bin]] += 1;

    // cout<<"N inner cluster : "<<inner_NClu<<" N outer cluster : "<<outer_NClu<<endl;

    double INTT_eta_acceptance_l = -0.5 * TMath::Log((sqrt(pow(-230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))-(-230.-TrigvtxMC[2]*10.)) / (sqrt(pow(-230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))+(-230.-TrigvtxMC[2]*10.))); // note : left
    double INTT_eta_acceptance_r =  -0.5 * TMath::Log((sqrt(pow(230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))-(230.-TrigvtxMC[2]*10.)) / (sqrt(pow(230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))+(230.-TrigvtxMC[2]*10.))); // note : right

    if (run_type == "MC")
    {
        MBin_Z_evt_hist_2D_MC -> Fill(centrality_map[centrality_bin], TrigvtxMC[2]*10.);
        MBin_Z_evt_hist_2D_MC -> Fill(MBin_Z_evt_hist_2D_MC -> GetNbinsX()-1, TrigvtxMC[2]*10.);

        // note : for the true track case ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        // if (event_i % 100 == 0){cout<<"z : "<<TrigvtxMC[2]*10.<<" eta : "<<INTT_eta_acceptance_l<<" "<<INTT_eta_acceptance_r<<endl;}

        // cout<<"true_track_info : "<<true_track_info.size()<<endl;    
        for (int track_i = 0; track_i < true_track_info.size(); track_i++)
        {
            if (true_track_info[track_i][2] == 111 || true_track_info[track_i][2] == 22 || abs(true_track_info[track_i][2]) == 2112){continue;}

            // note : coarse eta and z binning, this is for the final dNdeta result
            coarse_eta_z_2D_fulleta_MC[centrality_map[centrality_bin]]        -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);
            coarse_eta_z_2D_fulleta_MC[coarse_eta_z_2D_fulleta_MC.size() - 1] -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);

            if (TrigvtxMC[2]*10. > z_region[selected_z_region_id.first] && TrigvtxMC[2]*10. < z_region[selected_z_region_id.second])
            {
                dNdeta_1D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0]);
                dNdeta_1D_MC[dNdeta_1D_MC.size() - 1]        -> Fill(true_track_info[track_i][0]);
            }


            if (true_track_info[track_i][0] > INTT_eta_acceptance_l && true_track_info[track_i][0] < INTT_eta_acceptance_r)
            {
                // note : 1D, fine binning for a detailed check, full zvtx range 
                if (TrigvtxMC[2]*10. > z_region[selected_z_region_id.first] && TrigvtxMC[2]*10. < z_region[selected_z_region_id.second])
                {
                    dNdeta_1D_MC_edge_eta_cut[centrality_map[centrality_bin]]       -> Fill(true_track_info[track_i][0]);
                    dNdeta_1D_MC_edge_eta_cut[dNdeta_1D_MC_edge_eta_cut.size() - 1] -> Fill(true_track_info[track_i][0]);
                }

                if (GetTH2BinXY(coarse_eta_z_map -> GetNbinsX(), coarse_eta_z_map -> GetNbinsY(), coarse_eta_z_map -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.)).second == tight_zvtx_bin)
                {
                    // note : 1D, coarse binning for the final dNdeta result with a fixed z vertex range
                    final_dNdeta_1D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0]);
                    final_dNdeta_1D_MC[final_dNdeta_1D_MC.size() - 1]  -> Fill(true_track_info[track_i][0]);
                }
                
                // note : 2D, fine binning for the detailed check of eta and z distribution
                track_eta_z_2D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);
                track_eta_z_2D_MC[track_eta_z_2D_MC.size() - 1]   -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);    

                // note : coarse eta and z binning, this is for the final dNdeta result
                coarse_eta_z_2D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);
                coarse_eta_z_2D_MC[coarse_eta_z_2D_MC.size() - 1]  -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);
                

                // cout<<"true track eta : "<<true_track_info[track_i][0]<<" phi : "<<convertTo360(true_track_info[track_i][1])<<endl;
                // cout<<"("<<true_track_info[track_i][0]<<", "<<convertTo360(true_track_info[track_i][1])<<"), ";

                evt_true_track_gr -> SetPoint(evt_true_track_gr->GetN(),true_track_info[track_i][0], convertTo360(true_track_info[track_i][1]));
                out_truetrack_eta_d.push_back(true_track_info[track_i][0]);
                out_truetrack_phi_d.push_back(convertTo360(true_track_info[track_i][1]));

                evt_NTrack_MC += 1;
            }
        }
        // if (evt_NTrack_MC < 10) {cout<<"evt : "<<event_i<<" ---- N reco track : "<<evt_NTrack<<" N true track : "<<evt_NTrack_MC<<" ratio : "<<double(evt_NTrack) / double(evt_NTrack_MC)<<endl;}
    }

    if (evt_z.first > z_region[selected_z_region_id.first] && evt_z.first < z_region[selected_z_region_id.second])
    {
        exclusive_NClus_inner -> Fill(temp_sPH_inner_nocolumn_vec.size(), zvtx_weighting);
        exclusive_NClus_outer -> Fill(temp_sPH_outer_nocolumn_vec.size(), zvtx_weighting);
        exclusive_NClus_sum   -> Fill(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(), zvtx_weighting);
    }

    // note : put the cluster into the phi map, the first bool is for the cluster usage.
    // note : false means the cluster is not used
    for (int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++) {
        Clus_InnerPhi_Offset = (temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second < 0) ? atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi());
        double Clus_InnerPhi_Offset_radian = atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first);

        // cout<<"inner clu phi : "<<Clus_InnerPhi_Offset<<" origin: "<< temp_sPH_inner_nocolumn_vec[inner_i].phi <<endl;
        // cout<<" ("<<Clus_InnerPhi_Offset<<", "<< temp_sPH_inner_nocolumn_vec[inner_i].phi<<")" <<endl;
        inner_clu_phi_map[ int(Clus_InnerPhi_Offset) ].push_back({false,temp_sPH_inner_nocolumn_vec[inner_i]});
        check_inner_layer_clu_phi_1D->Fill(Clus_InnerPhi_Offset);

        double clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y, temp_sPH_inner_nocolumn_vec[inner_i].z});
        
        if (evt_z.first > z_region[selected_z_region_id.first] && evt_z.first < z_region[selected_z_region_id.second]){
            exclusive_cluster_inner_eta -> Fill(clu_eta, zvtx_weighting);
            exclusive_cluster_inner_phi -> Fill(Clus_InnerPhi_Offset_radian, zvtx_weighting);

            exclusive_cluster_inner_eta_phi_2D -> Fill(clu_eta, Clus_InnerPhi_Offset_radian, zvtx_weighting);

            exclusive_cluster_inner_eta_adc_2D -> Fill(clu_eta, temp_sPH_inner_nocolumn_vec[inner_i].sum_adc, zvtx_weighting);

            exclusive_cluster_all_eta   -> Fill(clu_eta, zvtx_weighting);
            exclusive_cluster_all_phi   -> Fill(Clus_InnerPhi_Offset_radian, zvtx_weighting);

            exclusive_cluster_inner_adc -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].sum_adc, zvtx_weighting);
        }

        if (clu_eta > INTT_eta_acceptance_l && clu_eta < INTT_eta_acceptance_r) {effective_total_NClus += 1;}
    }
    for (int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++) {
        Clus_OuterPhi_Offset = (temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second < 0) ? atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi());
        double Clus_OuterPhi_Offset_radian = atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first);

        outer_clu_phi_map[ int(Clus_OuterPhi_Offset) ].push_back({false,temp_sPH_outer_nocolumn_vec[outer_i]});
        check_outer_layer_clu_phi_1D->Fill(Clus_OuterPhi_Offset);

        double clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y, temp_sPH_outer_nocolumn_vec[outer_i].z});
       
        if (evt_z.first > z_region[selected_z_region_id.first] && evt_z.first < z_region[selected_z_region_id.second]){
            exclusive_cluster_outer_eta -> Fill(clu_eta, zvtx_weighting);
            exclusive_cluster_outer_phi -> Fill(Clus_OuterPhi_Offset_radian, zvtx_weighting);

            exclusive_cluster_outer_eta_phi_2D -> Fill(clu_eta, Clus_OuterPhi_Offset_radian, zvtx_weighting);

            exclusive_cluster_outer_eta_adc_2D -> Fill(clu_eta, temp_sPH_outer_nocolumn_vec[outer_i].sum_adc, zvtx_weighting);

            exclusive_cluster_all_eta   -> Fill(clu_eta, zvtx_weighting);
            exclusive_cluster_all_phi   -> Fill(Clus_OuterPhi_Offset_radian, zvtx_weighting);

            exclusive_cluster_outer_adc -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].sum_adc, zvtx_weighting);
        }
       
        if (clu_eta > INTT_eta_acceptance_l && clu_eta < INTT_eta_acceptance_r) {effective_total_NClus += 1;}
    }

    // note : for the mega cluster finder, the 3/4-cluster tracklet that happens at the overlap region
    // mega_track_finder -> FindMegaTracks(inner_clu_phi_map, outer_clu_phi_map, evt_z, centrality_map[centrality_bin]);
    // note : the followings are the study of the mega track finder efficiency
    // int N_mega_track_with_recoZ = mega_track_finder -> Get_NClu3_track_count() + mega_track_finder -> Get_NClu4_track_count();
    // mega_track_finder -> ClearEvt();    
    // mega_track_finder -> FindMegaTracks(inner_clu_phi_map, outer_clu_phi_map, {TrigvtxMC[2] * 10., 0.5}, centrality_map[centrality_bin]);
    // int N_mega_track_with_trueZ = mega_track_finder -> Get_NClu3_track_count() + mega_track_finder -> Get_NClu4_track_count();
    // mega_track_finder -> ClearEvt();
    // if (N_mega_track_with_recoZ != N_mega_track_with_trueZ) {cout<<N_mega_track_with_recoZ<<" "<<N_mega_track_with_trueZ<<endl;}
    // if (N_mega_track_with_trueZ != 0) {mega_track_finding_ratio_2D -> Fill(total_NClus, double(N_mega_track_with_recoZ) / double(N_mega_track_with_trueZ));}

    // note : for two-cluster tracklets only
    for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++) // note : each phi cell (1 degree)
    {
        // note : N cluster in this phi cell
        for (int inner_phi_clu_i = 0; inner_phi_clu_i < inner_clu_phi_map[inner_phi_i].size(); inner_phi_clu_i++)
        {
            clu_multi_used_tight = 0;
            clu_multi_used_loose = 0;
            if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].first == true) {continue;}

            Clus_InnerPhi_Offset = (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second < 0) ? atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());
            double Clus_InnerPhi_Offset_radian = atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first);

            // todo: change the outer phi scan range
            // note : the outer phi index, -4, -3, -2, -1, 0, 1, 2, 3, 4
            for (int scan_i = -4; scan_i < 5; scan_i++)
            {
                int true_scan_i = ((inner_phi_i + scan_i) < 0) ? 360 + (inner_phi_i + scan_i) : ((inner_phi_i + scan_i) > 359) ? (inner_phi_i + scan_i)-360 : inner_phi_i + scan_i;

                // note : N clusters in that outer phi cell
                for (int outer_phi_clu_i = 0; outer_phi_clu_i < outer_clu_phi_map[true_scan_i].size(); outer_phi_clu_i++)
                {
                    if (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].first == true) {continue;}

                    Clus_OuterPhi_Offset = (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second < 0) ? atan2(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());
                    double Clus_OuterPhi_Offset_radian = atan2(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first);
                    double delta_phi = get_delta_phi(Clus_InnerPhi_Offset, Clus_OuterPhi_Offset);
                    
                    // if (fabs(delta_phi) > 5.72) {continue;}
                    if (fabs(delta_phi) > 3.5) {continue;}

                    double inner_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z});
                    double outer_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z});
                    double delta_eta = inner_clu_eta - outer_clu_eta;

                    // note : all zvtx range and all eta region
                    track_delta_eta_1D[centrality_map[centrality_bin]] -> Fill( delta_eta ); 
                    track_delta_eta_1D[track_delta_eta_1D.size() - 1]  -> Fill( delta_eta );

                    // note : all zvtx range and all eta region 
                    track_DeltaPhi_DeltaEta_2D[centrality_map[centrality_bin]]        -> Fill(delta_phi, delta_eta); 
                    track_DeltaPhi_DeltaEta_2D[track_DeltaPhi_DeltaEta_2D.size() - 1] -> Fill(delta_phi, delta_eta);

                    pair<double,double> z_range_info = Get_possible_zvtx( 
                        0., // get_radius(beam_origin.first,beam_origin.second), 
                        {get_radius(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second), inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z}, // note : unsign radius
                        {get_radius(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second), outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z}  // note : unsign radius
                    );

                    // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
                    if (z_range_info.first - z_range_info.second > evt_z.first + evt_z.second || z_range_info.first + z_range_info.second < evt_z.first - evt_z.second) {continue;}
                    if (fabs(delta_phi) < 0.6) {clu_multi_used_tight += 1;} // todo : the delta phi cut value of the multi-usage inner cluster is here 
                    track_delta_eta_1D_post[centrality_map[centrality_bin]]     -> Fill( delta_eta );
                    track_delta_eta_1D_post[track_delta_eta_1D_post.size() - 1] -> Fill( delta_eta );

                    double DCA_sign = calculateAngleBetweenVectors(
                        outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y,
                        inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y,
                        beam_origin.first, beam_origin.second
                    );
                    
                    track_DCA_distance[centrality_map[centrality_bin]] -> Fill( DCA_sign );
                    track_DCA_distance[track_DCA_distance.size() - 1]  -> Fill( DCA_sign ); 

                    track_phi_DCA_2D[centrality_map[centrality_bin]] -> Fill( delta_phi, DCA_sign );
                    track_phi_DCA_2D[track_phi_DCA_2D.size() - 1]    -> Fill( delta_phi, DCA_sign );
                    
                    track_delta_phi_1D[centrality_map[centrality_bin]] -> Fill( delta_phi ); // note : the pairs that pass the rough delta phi cut and eta cut
                    track_delta_phi_1D[track_delta_phi_1D.size() - 1]  -> Fill( delta_phi );

                    proto_pair_index.push_back({inner_phi_i, inner_phi_clu_i, true_scan_i, outer_phi_clu_i});
                    proto_pair_delta_phi_abs.push_back(fabs(delta_phi));
                    proto_pair_delta_phi.push_back({Clus_InnerPhi_Offset, Clus_OuterPhi_Offset, delta_phi});


                    // note : do the fill here (find the best match outer cluster with the inner cluster )
                    Get_eta_pair = Get_eta(
                        {0., evt_z.first,evt_z.second},
                        {get_radius(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second), inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z},
                        {get_radius(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second), outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z}
                    );  
                
                    // note : find the bin of the eta
                    // double eta_bin = eta_region_hist -> Fill(Get_eta_pair.second);
                    // // cout<<"test1 "<<eta_bin<<endl;
                    // if (eta_bin != -1)
                    // {
                    //     final_track_multi_delta_phi_1D[centrality_map[centrality_bin]][eta_bin - 1]            -> Fill(delta_phi);
                    //     final_track_multi_delta_phi_1D[final_track_multi_delta_phi_1D.size() - 1][eta_bin - 1] -> Fill(delta_phi);

                    //     if (fabs(delta_phi) <= signal_region) { 
                    //         good_tracklet_multi_counting[centrality_map[centrality_bin]][eta_bin - 1]          += 1;
                    //         good_tracklet_multi_counting[good_tracklet_multi_counting.size() - 1][eta_bin - 1] += 1;
                    //     }
                    // }
                    
                    // note : the code below is for the 2D eta-z array
                    double eta_z_bin = GetTH2Index1D(GetTH2BinXY(coarse_eta_z_map->GetNbinsX(), coarse_eta_z_map->GetNbinsY(), coarse_eta_z_map -> Fill(Get_eta_pair.second, evt_z.first)), coarse_eta_z_map->GetNbinsX());
                    if (int(eta_z_bin) != -1)
                    {
                        final_track_multi_delta_phi_1D[centrality_map[centrality_bin]][eta_z_bin]            -> Fill(delta_phi, zvtx_weighting); // note : each coarse eta and z bin
                        final_track_multi_delta_phi_1D[final_track_multi_delta_phi_1D.size() - 1][eta_z_bin] -> Fill(delta_phi, zvtx_weighting);

                        if (fabs(delta_phi) <= signal_region) { 
                            good_tracklet_multi_counting[centrality_map[centrality_bin]][eta_z_bin]          += 1;
                            good_tracklet_multi_counting[good_tracklet_multi_counting.size() - 1][eta_z_bin] += 1;

                            coarse_Reco_SignalNTracklet_Multi_eta_z_2D[centrality_map[centrality_bin]]                        -> Fill(Get_eta_pair.second, evt_z.first, zvtx_weighting);
                            coarse_Reco_SignalNTracklet_Multi_eta_z_2D[coarse_Reco_SignalNTracklet_Multi_eta_z_2D.size() - 1] -> Fill(Get_eta_pair.second, evt_z.first, zvtx_weighting);

                            if (evt_z.first > z_region[selected_z_region_id.first] && evt_z.first < z_region[selected_z_region_id.second])
                            {
                                exclusive_loose_tracklet_eta -> Fill(Get_eta_pair.second, zvtx_weighting);
                                exclusive_loose_tracklet_phi -> Fill( (Clus_InnerPhi_Offset_radian + Clus_OuterPhi_Offset_radian)/2. , zvtx_weighting);
                            }

                        }

                    }
                    
                    // if (clu_multi_used_loose == 0) 
                    // {
                    //     pair_delta_phi = delta_phi;
                    //     pair_outer_index = {true_scan_i, outer_phi_clu_i};
                    //     clu_multi_used_loose += 1;
                    // }
                    // else if (fabs(delta_phi) < fabs(pair_delta_phi))
                    // {
                    //     pair_delta_phi = delta_phi;
                    //     pair_outer_index = {true_scan_i, outer_phi_clu_i};
                    //     clu_multi_used_loose += 1;
                    // }
                }
            } // note : end outer loop

            clu_used_centrality_2D -> Fill(total_NClus, clu_multi_used_tight);

            // // note : if there is no good match with inner and outer clusters, continue
            // if (clu_multi_used_loose == 0) {continue;}

            // // note : do the fill here (find the best match outer cluster with the inner cluster )
            // Get_eta_pair = Get_eta(
            //     {0., evt_z.first,evt_z.second},
            //     {get_radius(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second), inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z},
            //     {get_radius(outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.x - beam_origin.first, outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.y - beam_origin.second), outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.z}
            // );

            // double inner_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z});
            // double outer_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.x, outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.y, outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.z});
            // double track_phi = Clus_InnerPhi_Offset - (pair_delta_phi/2.);

            // if  (Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2. > 0.3)
            // {
            //     cout<<" "<<endl;
            //     cout<<"inner clu eta : "<<inner_clu_eta<<" outer clu eta : "<<outer_clu_eta<<"avg eta : "<< (inner_clu_eta + outer_clu_eta)/2. <<" reco eta : "<<Get_eta_pair.second<<" diff: "<<Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.<<endl;
            //     cout<<"inner clu pos : "<<inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x<<" "<<inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y<<" "<<inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z<<endl;
            //     cout<<"outer clu pos : "<<outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.x<<" "<<outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.y<<" "<<outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.z<<endl;
            // }

            // reco_eta_correlation_2D -> Fill(Get_eta_pair.second, (inner_clu_eta + outer_clu_eta)/2.);
            // reco_eta_diff_reco3P_2D -> Fill(Get_eta_pair.second, Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);
            // reco_eta_diff_1D        -> Fill(Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);

            // // track_delta_phi_1D[centrality_map[centrality_bin]] -> Fill( pair_delta_phi );
            // // track_delta_phi_1D[track_delta_phi_1D.size() - 1]  -> Fill( pair_delta_phi );

            // track_DeltaPhi_eta_2D[centrality_map[centrality_bin]]   -> Fill(pair_delta_phi, Get_eta_pair.second);
            // track_DeltaPhi_eta_2D[track_DeltaPhi_eta_2D.size() - 1] -> Fill(pair_delta_phi, Get_eta_pair.second);

            // // cout<<"test_5"<<endl;
            // dNdeta_1D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second);
            // dNdeta_1D[dNdeta_1D.size() - 1]           -> Fill(Get_eta_pair.second);

            // track_eta_phi_2D[centrality_map[centrality_bin]] -> Fill(track_phi, Get_eta_pair.second);
            // track_eta_phi_2D[track_eta_phi_2D.size() - 1]    -> Fill(track_phi, Get_eta_pair.second);
            
            // track_eta_z_2D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second, evt_z.first);
            // track_eta_z_2D[track_eta_z_2D.size() - 1]      -> Fill(Get_eta_pair.second, evt_z.first);        

            // // note : find the bin of the eta
            // double eta_bin = eta_region_hist -> Fill(Get_eta_pair.second);
            // // cout<<"test1 "<<eta_bin<<endl;
            // if (eta_bin != -1)
            // {
            //     final_track_delta_phi_1D[centrality_map[centrality_bin]][eta_bin - 1]      -> Fill(pair_delta_phi);
            //     final_track_delta_phi_1D[final_track_delta_phi_1D.size() - 1][eta_bin - 1] -> Fill(pair_delta_phi);

            //     out_track_delta_phi_d.push_back(pair_delta_phi);
            //     out_recotrack_eta_d.push_back(Get_eta_pair.second);
            //     out_track_eta_i.push_back(eta_bin);
            // }

            // evt_NTrack += 1;
            // // note : this outer cluster is used 
            // outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].first = true;
        }
    } // note : end inner loop

    // note : if there is no good proto-track then move on to the next event
    if (proto_pair_delta_phi_abs.size() == 0) {return;}

    long long vec_size = proto_pair_delta_phi_abs.size();
    long long ind[proto_pair_delta_phi_abs.size()];
    TMath::Sort(vec_size, &proto_pair_delta_phi_abs[0], ind, false);
    for (int pair_i = 0; pair_i < proto_pair_index.size(); pair_i++)
    {
        int inner_index_0 = proto_pair_index[ind[pair_i]][0];
        int inner_index_1 = proto_pair_index[ind[pair_i]][1];
        int outer_index_0 = proto_pair_index[ind[pair_i]][2];
        int outer_index_1 = proto_pair_index[ind[pair_i]][3];

        // cout<<"test: the pair delta phi abs: "<<proto_pair_delta_phi_abs[ind[pair_i]]<<endl;
        if (inner_used_clu[Form("%i_%i", inner_index_0, inner_index_1)] != 0) { inner_used_clu[Form("%i_%i", inner_index_0, inner_index_1)] += 1; continue;}
        if (outer_used_clu[Form("%i_%i", outer_index_0, outer_index_1)] != 0) { outer_used_clu[Form("%i_%i", outer_index_0, outer_index_1)] += 1; continue;}
        double delta_phi = proto_pair_delta_phi[ind[pair_i]][2];
        double track_phi = get_track_phi(proto_pair_delta_phi[ind[pair_i]][0], delta_phi); // proto_pair_delta_phi[ind[pair_i]][0] - (delta_phi/2.);

        // note : do the fill here (find the best match outer cluster with the inner cluster )
        Get_eta_pair = Get_eta(
            {0., evt_z.first,evt_z.second},
            {get_radius(inner_clu_phi_map[inner_index_0][inner_index_1].second.x - beam_origin.first, inner_clu_phi_map[inner_index_0][inner_index_1].second.y - beam_origin.second), inner_clu_phi_map[inner_index_0][inner_index_1].second.z},
            {get_radius(outer_clu_phi_map[outer_index_0][outer_index_1].second.x - beam_origin.first, outer_clu_phi_map[outer_index_0][outer_index_1].second.y - beam_origin.second), outer_clu_phi_map[outer_index_0][outer_index_1].second.z}
        );

        double inner_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{inner_clu_phi_map[inner_index_0][inner_index_1].second.x, inner_clu_phi_map[inner_index_0][inner_index_1].second.y, inner_clu_phi_map[inner_index_0][inner_index_1].second.z});
        double outer_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{outer_clu_phi_map[outer_index_0][outer_index_1].second.x, outer_clu_phi_map[outer_index_0][outer_index_1].second.y, outer_clu_phi_map[outer_index_0][outer_index_1].second.z});

        double Clus_InnerPhi_Offset_radian = atan2(inner_clu_phi_map[inner_index_0][inner_index_1].second.y - beam_origin.second, inner_clu_phi_map[inner_index_0][inner_index_1].second.x - beam_origin.first);
        double Clus_OuterPhi_Offset_radian = atan2(outer_clu_phi_map[outer_index_0][outer_index_1].second.y - beam_origin.second, outer_clu_phi_map[outer_index_0][outer_index_1].second.x - beam_origin.first);

        if  (Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2. > 0.3)
        {
            cout<<" "<<endl;
            cout<<"inner clu eta : "<<inner_clu_eta<<" outer clu eta : "<<outer_clu_eta<<"avg eta : "<< (inner_clu_eta + outer_clu_eta)/2. <<" reco eta : "<<Get_eta_pair.second<<" diff: "<<Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.<<endl;
            cout<<"inner clu pos : "<<inner_clu_phi_map[inner_index_0][inner_index_1].second.x<<" "<<inner_clu_phi_map[inner_index_0][inner_index_1].second.y<<" "<<inner_clu_phi_map[inner_index_0][inner_index_1].second.z<<endl;
            cout<<"outer clu pos : "<<outer_clu_phi_map[outer_index_0][outer_index_1].second.x<<" "<<outer_clu_phi_map[outer_index_0][outer_index_1].second.y<<" "<<outer_clu_phi_map[outer_index_0][outer_index_1].second.z<<endl;
        }

        reco_eta_correlation_2D -> Fill(Get_eta_pair.second, (inner_clu_eta + outer_clu_eta)/2.);
        reco_eta_diff_reco3P_2D -> Fill(Get_eta_pair.second, Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);
        reco_eta_diff_1D        -> Fill(Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);

        // track_delta_phi_1D[centrality_map[centrality_bin]] -> Fill( delta_phi );
        // track_delta_phi_1D[track_delta_phi_1D.size() - 1]  -> Fill( delta_phi );

        track_DeltaPhi_eta_2D[centrality_map[centrality_bin]]   -> Fill(delta_phi, Get_eta_pair.second);
        track_DeltaPhi_eta_2D[track_DeltaPhi_eta_2D.size() - 1] -> Fill(delta_phi, Get_eta_pair.second);

        // cout<<"test_5"<<endl;
        if (fabs(delta_phi) <= signal_region)
        {
            if (evt_z.first > z_region[selected_z_region_id.first] && evt_z.first < z_region[selected_z_region_id.second])
            {
                dNdeta_1D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second);
                dNdeta_1D[dNdeta_1D.size() - 1]           -> Fill(Get_eta_pair.second);
            }
            

            track_eta_z_2D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second, evt_z.first);
            track_eta_z_2D[track_eta_z_2D.size() - 1]      -> Fill(Get_eta_pair.second, evt_z.first);

            out_recotrack_eta_d.push_back(Get_eta_pair.second);
            out_recotrack_phi_d.push_back(track_phi);

            // cout<<"good 2clu track, eta: "<<Get_eta_pair.second<<" phi: "<<track_phi<<" delta_phi: "<<delta_phi<<" -- index, inner: "<<inner_index_0<<" "<<inner_index_1<<" outer: "<<outer_index_0<<" "<<outer_index_1<<endl;
            // cout<<" pair_line->DrawLine("<<inner_index_0<<","<<inner_index_1<<", "<<outer_index_0<<","<<outer_index_1<<");"<<endl;
            // cout<<"("<<track_phi<<", "<<Get_eta_pair.second<<"), ";
            evt_NTrack += 1;
        }
        

        track_eta_phi_2D[centrality_map[centrality_bin]] -> Fill(track_phi, Get_eta_pair.second);
        track_eta_phi_2D[track_eta_phi_2D.size() - 1]    -> Fill(track_phi, Get_eta_pair.second);
        
        // // note : find the bin of the eta
        // double eta_bin = eta_region_hist -> Fill(Get_eta_pair.second);
        // // cout<<"test1 "<<eta_bin<<endl;
        // if (eta_bin != -1)
        // {
        //     final_track_delta_phi_1D[centrality_map[centrality_bin]][eta_bin - 1]      -> Fill(delta_phi);
        //     final_track_delta_phi_1D[final_track_delta_phi_1D.size() - 1][eta_bin - 1] -> Fill(delta_phi);

        //     if (fabs(delta_phi) <= signal_region) { 
        //         good_tracklet_counting[centrality_map[centrality_bin]][eta_bin - 1] += 1;
        //         good_tracklet_counting[good_tracklet_counting.size() - 1][eta_bin - 1] += 1;
        //     }

        //     out_track_delta_phi_d.push_back(delta_phi);
        //     out_recotrack_eta_d.push_back(Get_eta_pair.second);
        //     out_track_eta_i.push_back(eta_bin);
        // }

        // note : the code below is for the 2D eta-z array
        double eta_z_bin = GetTH2Index1D(GetTH2BinXY(coarse_eta_z_map->GetNbinsX(), coarse_eta_z_map->GetNbinsY(), coarse_eta_z_map -> Fill(Get_eta_pair.second, evt_z.first)), coarse_eta_z_map->GetNbinsX());
        if (int(eta_z_bin) != -1)
        {
            final_track_delta_phi_1D[centrality_map[centrality_bin]][eta_z_bin]      -> Fill(delta_phi);
            final_track_delta_phi_1D[final_track_delta_phi_1D.size() - 1][eta_z_bin] -> Fill(delta_phi);

            if (fabs(delta_phi) <= signal_region) { 
                good_tracklet_counting[centrality_map[centrality_bin]][eta_z_bin]    += 1;
                good_tracklet_counting[good_tracklet_counting.size() - 1][eta_z_bin] += 1;

                coarse_Reco_SignalNTracklet_Single_eta_z_2D[centrality_map[centrality_bin]]                         -> Fill(Get_eta_pair.second, evt_z.first, zvtx_weighting);
                coarse_Reco_SignalNTracklet_Single_eta_z_2D[coarse_Reco_SignalNTracklet_Single_eta_z_2D.size() - 1] -> Fill(Get_eta_pair.second, evt_z.first, zvtx_weighting);

                if (evt_z.first > z_region[selected_z_region_id.first] && evt_z.first < z_region[selected_z_region_id.second])
                {
                    exclusive_tight_tracklet_eta -> Fill(Get_eta_pair.second, zvtx_weighting);
                    exclusive_tight_tracklet_phi -> Fill((Clus_InnerPhi_Offset_radian + Clus_OuterPhi_Offset_radian)/2., zvtx_weighting);
                }
            }

            // note : save the information in detail
            // out_track_delta_phi_d.push_back(delta_phi);
            // out_recotrack_eta_d.push_back(Get_eta_pair.second);
            // out_track_eta_i.push_back(eta_bin);

        }

        

        // note : since the clusters are used in the tracklet, mark the clusters as used
        inner_used_clu[Form("%i_%i", inner_index_0, inner_index_1)] += 1;
        outer_used_clu[Form("%i_%i", outer_index_0, outer_index_1)] += 1;
    }
    // note : save the information in detail
    out_eID = event_i;
    out_evt_centrality_bin = centrality_bin;
    out_evt_zvtx = evt_z.first;
    out_true_zvtx = TrigvtxMC[2] * 10.;
    out_NTrueTrack = evt_NTrack_MC;
    out_total_NClus = total_NClus;
    out_N2Clu_track = evt_NTrack;
    out_N3Clu_track = mega_track_finder -> Get_NClu3_track_count();
    out_N4Clu_track = mega_track_finder -> Get_NClu4_track_count();
    tree_out -> Fill();

    cout<<"event confirmation : "<<out_N2Clu_track<<" "<<out_N3Clu_track<<" "<<out_N4Clu_track<<endl;

    // cout<<" "<<endl;
    // cout<<" "<<endl;
    // cout<<"test_8"<<endl;
    if (run_type == "MC")
    {
        N_reco_cluster_short += (evt_NTrack_MC - evt_NTrack);
        track_correlation_2D -> Fill(evt_NTrack_MC, evt_NTrack);
        track_ratio_2D -> Fill(evt_NTrack_MC, double(evt_NTrack)/double(evt_NTrack_MC));
        track_ratio_1D[centrality_map[centrality_bin]] -> Fill( double(evt_NTrack) / double(evt_NTrack_MC) );
        track_ratio_1D[track_ratio_1D.size() - 1]      -> Fill( double(evt_NTrack) / double(evt_NTrack_MC) );

        track_cluster_ratio_1D_MC[centrality_map[centrality_bin]]       -> Fill( double(effective_total_NClus) / double(evt_NTrack_MC) );
        track_cluster_ratio_1D_MC[track_cluster_ratio_1D_MC.size() - 1] -> Fill( double(effective_total_NClus) / double(evt_NTrack_MC) );    
        track_cluster_ratio_multiplicity_2D_MC -> Fill( effective_total_NClus, double(effective_total_NClus) / double(evt_NTrack_MC) );
    }
        
    track_cluster_ratio_multiplicity_2D -> Fill( effective_total_NClus, double(effective_total_NClus) / double(evt_NTrack) );
    track_cluster_ratio_1D[centrality_map[centrality_bin]]    -> Fill( double(effective_total_NClus) / double(evt_NTrack) );
    track_cluster_ratio_1D[track_cluster_ratio_1D.size() - 1] -> Fill( double(effective_total_NClus) / double(evt_NTrack) );

    // if (run_type == "MC" && evt_NTrack_MC < draw_evt_cut && event_i % print_plot_ratio == 0){print_evt_plot(event_i, evt_NTrack_MC, inner_NClu, outer_NClu);}
    return_tag = 1;
}


void INTTEta::ClearEvt()
{
    if (evt_reco_track_gr_All -> GetN() != 0) {evt_reco_track_gr_All -> Set(0);}
    if (evt_reco_track_gr_PhiLoose -> GetN() != 0) {evt_reco_track_gr_PhiLoose -> Set(0);}
    if (evt_reco_track_gr_Z -> GetN() != 0) {evt_reco_track_gr_Z -> Set(0);}
    if (evt_reco_track_gr_ZDCA -> GetN() != 0) {evt_reco_track_gr_ZDCA -> Set(0);}
    if (evt_reco_track_gr_ZDCAPhi -> GetN() != 0) {evt_reco_track_gr_ZDCAPhi -> Set(0);}
    if (evt_true_track_gr -> GetN() != 0) {evt_true_track_gr -> Set(0);}
    if (track_gr -> GetN() != 0) {track_gr -> Set(0);}

    out_recotrack_eta_d.clear();
    out_recotrack_phi_d.clear();
    out_truetrack_eta_d.clear();
    out_truetrack_phi_d.clear();
    out_track_eta_i.clear();
    out_track_delta_phi_d.clear();

    effective_total_NClus = 0;

    mega_track_finder -> ClearEvt();

    // for (int i = 0; i < 360; i++)
    // {
    //     inner_clu_phi_map[i].clear();
    //     outer_clu_phi_map[i].clear();
    // }

    // std::fill(std::begin(inner_clu_phi_map), std::end(inner_clu_phi_map), std::vector<pair<bool,clu_info>>());
    // std::fill(std::begin(outer_clu_phi_map), std::end(outer_clu_phi_map), std::vector<pair<bool,clu_info>>());
    inner_clu_phi_map.clear();
    outer_clu_phi_map.clear();
    inner_clu_phi_map = vector<vector<pair<bool,clu_info>>>(360);
    outer_clu_phi_map = vector<vector<pair<bool,clu_info>>>(360);

    proto_pair_index.clear();
    proto_pair_delta_phi_abs.clear();
    proto_pair_delta_phi.clear();
    inner_used_clu.clear();
    outer_used_clu.clear();

    out_N2Clu_track = 0;
    out_N3Clu_track = 0;
    out_N4Clu_track = 0;

    return_tag = 0;
    evt_NTrack = 0;
    evt_NTrack_MC = 0;
    return;
}


void INTTEta::PrintPlots()
{

    // note : plots for the mega track finder
    
    NClu3_track_centrality_2D = mega_track_finder -> Get_NClu3_track_centrality_2D();
    NClu4_track_centrality_2D = mega_track_finder -> Get_NClu4_track_centrality_2D();
    cluster4_track_phi_1D = mega_track_finder -> Get_cluster4_track_phi_1D();
    cluster3_track_phi_1D = mega_track_finder -> Get_cluster3_track_phi_1D();
    cluster3_inner_track_eta_1D = mega_track_finder -> Get_cluster3_inner_track_eta_1D();
    cluster3_inner_track_phi_1D = mega_track_finder -> Get_cluster3_inner_track_phi_1D();
    cluster3_outer_track_eta_1D = mega_track_finder -> Get_cluster3_outer_track_eta_1D();
    cluster3_outer_track_phi_1D = mega_track_finder -> Get_cluster3_outer_track_phi_1D();
    clu4_track_ReducedChi2_1D = mega_track_finder -> Get_clu4_track_ReducedChi2_1D();
    clu3_track_ReducedChi2_1D = mega_track_finder -> Get_clu3_track_ReducedChi2_1D();
    mega_track_eta_1D = mega_track_finder -> Get_mega_track_eta_1D();

    c1 -> cd();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    mega_track_finding_ratio_2D -> Draw("colz0");
    mega_track_finding_ratio_2D -> SetMinimum(0);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/mega_track_finding_ratio_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();


    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    cluster3_inner_track_eta_1D -> Draw("hist");
    cluster3_inner_track_eta_1D -> SetMinimum(0);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/cluster3_inner_track_eta_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    cluster3_outer_track_eta_1D -> Draw("hist");
    cluster3_outer_track_eta_1D -> SetMinimum(0);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/cluster3_outer_track_eta_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    mega_track_eta_1D -> Draw("hist");
    mega_track_eta_1D -> SetMinimum(0);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/mega_track_eta_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    check_inner_layer_clu_phi_1D -> Draw("hist");
    check_inner_layer_clu_phi_1D -> SetMinimum(0);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/check_inner_layer_clu_phi_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    check_outer_layer_clu_phi_1D -> Draw("hist");
    check_outer_layer_clu_phi_1D -> SetMinimum(0);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/check_outer_layer_clu_phi_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    clu4_track_ReducedChi2_1D -> Draw("hist");
    clu4_track_ReducedChi2_1D -> SetMinimum(0);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/clu4_track_ReducedChi2_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    clu3_track_ReducedChi2_1D -> Draw("hist");
    clu3_track_ReducedChi2_1D -> SetMinimum(0);
    clu3_track_ReducedChi2_1D -> SetMaximum(clu3_track_ReducedChi2_1D->GetBinContent(clu3_track_ReducedChi2_1D->GetMaximumBin())*1.3);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    coord_line -> DrawLine(mega_track_finder->Get_performance_cut(),0,mega_track_finder->Get_performance_cut(),clu3_track_ReducedChi2_1D->GetBinContent(clu3_track_ReducedChi2_1D->GetMaximumBin())*1.3);
    c1 -> Print( Form("%s/clu3_track_ReducedChi2_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    c1 -> Print( Form("%s/track_cluster_ratio_1D.pdf(", out_folder_directory.c_str()) );
    c1 -> Clear();
    for (int i = 0; i < track_cluster_ratio_1D.size(); i++)
    {
        c1 -> cd();
        track_cluster_ratio_1D[i] -> Draw("hist");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/track_cluster_ratio_1D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_cluster_ratio_1D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    c1 -> Print( Form("%s/track_cluster_ratio_1D_MC.pdf(", out_folder_directory.c_str()) );
    c1 -> Clear();
    for (int i = 0; i < track_cluster_ratio_1D_MC.size(); i++)
    {
        c1 -> cd();
        track_cluster_ratio_1D_MC[i] -> Draw("hist");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/track_cluster_ratio_1D_MC.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_cluster_ratio_1D_MC.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // c1 -> Print( Form("%s/dNdeta_1D.pdf(", out_folder_directory.c_str()) );
    // for (int i = 0; i < dNdeta_1D.size(); i++)
    // {   
    //     double N_correction_evt = (i == dNdeta_1D_MC.size() - 1) ? N_GoodEvent : N_GoodEvent_vec[i];
    //     dNdeta_1D_MC[i] -> Scale(1./double(dNdeta_1D_MC[i] -> GetBinWidth(1) ));
    //     dNdeta_1D_MC[i] -> Scale(1./double(N_correction_evt));
        
    //     dNdeta_1D[i] -> Scale(1./double(dNdeta_1D[i] -> GetBinWidth(1) ));
    //     dNdeta_1D[i] -> Scale(1./double(N_correction_evt));
    //     dNdeta_1D[i] -> GetYaxis() -> SetRangeUser(0,800);
    //     dNdeta_1D[i] -> Draw("ep");
    //     dNdeta_1D_MC[i] -> Draw("hist same");
    //     ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    //     draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s, inclusive Zvtx",centrality_region[i].c_str()));
    //     draw_text -> DrawLatex(0.21, 0.86, Form("Nevt MC = Nevt reco: %.1f", N_correction_evt));
    //     draw_text -> DrawLatex(0.21, 0.82, Form("MC entry : %.2f, tight entry : %.2f", dNdeta_1D_MC[i] -> Integral(), dNdeta_1D[i] -> Integral()));
    //     c1 -> Print(Form("%s/dNdeta_1D.pdf", out_folder_directory.c_str()));
    //     c1 -> Clear();    
    // }
    // c1 -> Print( Form("%s/dNdeta_1D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_eta_phi_2D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_eta_phi_2D.size(); i++)
    {
        track_eta_phi_2D[i] -> Draw("colz0");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/track_eta_phi_2D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_eta_phi_2D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_eta_z_2D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_eta_z_2D.size(); i++)
    {
        track_eta_z_2D[i] -> Draw("colz0");
        DrawEtaZGrid();
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/track_eta_z_2D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_eta_z_2D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_delta_phi_1D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_delta_phi_1D.size(); i++)
    {
        track_delta_phi_1D[i] -> SetMinimum(0);
        track_delta_phi_1D[i] -> Draw("hist");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        coord_line -> DrawLine(-1*phi_diff_cut, 0, -1 * phi_diff_cut, track_delta_phi_1D[i]->GetMaximum() * 1.1);
        coord_line -> DrawLine(phi_diff_cut, 0, phi_diff_cut, track_delta_phi_1D[i]->GetMaximum() * 1.1);
        c1 -> Print(Form("%s/track_delta_phi_1D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_delta_phi_1D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_delta_eta_1D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_delta_eta_1D.size(); i++)
    {
        track_delta_eta_1D[i] -> SetMinimum(0);
        track_delta_eta_1D[i] -> Draw("hist");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        // coord_line -> DrawLine(-1*phi_diff_cut, 0, -1 * phi_diff_cut, track_delta_eta_1D[i]->GetMaximum() * 1.1);
        // coord_line -> DrawLine(phi_diff_cut, 0, phi_diff_cut, track_delta_eta_1D[i]->GetMaximum() * 1.1);
        c1 -> Print(Form("%s/track_delta_eta_1D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_delta_eta_1D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_delta_eta_1D_post.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_delta_eta_1D_post.size(); i++)
    {
        track_delta_eta_1D_post[i] -> SetMinimum(0);
        track_delta_eta_1D_post[i] -> Draw("hist");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        // coord_line -> DrawLine(-1*phi_diff_cut, 0, -1 * phi_diff_cut, track_delta_eta_1D_post[i]->GetMaximum() * 1.1);
        // coord_line -> DrawLine(phi_diff_cut, 0, phi_diff_cut, track_delta_eta_1D_post[i]->GetMaximum() * 1.1);
        c1 -> Print(Form("%s/track_delta_eta_1D_post.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_delta_eta_1D_post.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_DCA_distance.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_DCA_distance.size(); i++)
    {
        track_DCA_distance[i] -> SetMinimum(0);
        track_DCA_distance[i] -> Draw("hist");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        coord_line -> DrawLine(DCA_cut.first, 0, DCA_cut.first, track_DCA_distance[i]->GetMaximum() * 1.05);
        coord_line -> DrawLine(DCA_cut.second, 0, DCA_cut.second, track_DCA_distance[i]->GetMaximum() * 1.05);
        c1 -> Print(Form("%s/track_DCA_distance.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_DCA_distance.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_phi_DCA_2D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_phi_DCA_2D.size(); i++)
    {
        track_phi_DCA_2D[i] -> Draw("colz0");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        
        coord_line -> DrawLine(track_phi_DCA_2D[i] -> GetXaxis() -> GetXmin(), DCA_cut.first, track_phi_DCA_2D[i] -> GetXaxis() -> GetXmax(), DCA_cut.first);
        coord_line -> DrawLine(track_phi_DCA_2D[i] -> GetXaxis() -> GetXmin(), DCA_cut.second, track_phi_DCA_2D[i] -> GetXaxis() -> GetXmax(), DCA_cut.second);
        coord_line -> DrawLine(-1 * phi_diff_cut, track_phi_DCA_2D[i] -> GetYaxis() -> GetXmin(), -1 * phi_diff_cut, track_phi_DCA_2D[i] -> GetYaxis() -> GetXmax());
        coord_line -> DrawLine(phi_diff_cut, track_phi_DCA_2D[i] -> GetYaxis() -> GetXmin(), phi_diff_cut, track_phi_DCA_2D[i] -> GetYaxis() -> GetXmax());
        c1 -> Print(Form("%s/track_phi_DCA_2D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_phi_DCA_2D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_ratio_1D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_ratio_1D.size(); i++)
    {
        track_ratio_1D[i] -> SetMinimum(0);
        track_ratio_1D[i] -> Draw("hist");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/track_ratio_1D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_ratio_1D.pdf)", out_folder_directory.c_str()) );
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    for (int i = 0; i < final_track_delta_phi_1D.size(); i++)
    {
        c1 -> Print( Form("%s/final_track_delta_phi_1D_MBin%i.pdf(", out_folder_directory.c_str(), i) );
        for (int i1 = 0; i1 < final_track_delta_phi_1D[i].size(); i1++)
        {   
            if (final_track_delta_phi_1D[i][i1] -> GetEntries() > 0) {
                double hist_offset = get_dist_offset(final_track_delta_phi_1D[i][i1], 15);
                gaus_pol1_fit->SetParameters( final_track_delta_phi_1D[i][i1] -> GetBinContent(final_track_delta_phi_1D[i][i1] -> GetMaximumBin()) - hist_offset, 0, final_track_delta_phi_1D[i][i1]->GetStdDev()/2., hist_offset, 0);
                final_track_delta_phi_1D[i][i1] -> Fit(gaus_pol1_fit,"NQ");
            
            
                // note : par[0] : size
                // note : par[1] : ratio of the two gaussians
                // note : par[2] : mean
                // note : par[3] : width of gaus 1
                // note : par[4] : width of gaus 2
                // note : par[5] : offset
                // note : par[6] : slope
                // note : fit with double gaussian function + pol1
                d_gaus_pol1_fit -> SetParameters(final_track_delta_phi_1D[i][i1] -> GetBinContent(final_track_delta_phi_1D[i][i1] -> GetMaximumBin()) - hist_offset, 0.2, 0, final_track_delta_phi_1D[i][i1]->GetStdDev()/2., final_track_delta_phi_1D[i][i1]->GetStdDev()/2., hist_offset, 0);
                d_gaus_pol1_fit -> SetParLimits(1, 0, 0.5);  // note : the first gaussian is  the main distribution, So it should contain more than 50% of the total distribution
                d_gaus_pol1_fit -> SetParLimits(3, 0, 1000); // note : the width of the gaussian should be positive
                d_gaus_pol1_fit -> SetParLimits(4, 0, 1000); // note : the width of the gaussian should be positive
                final_track_delta_phi_1D[i][i1] -> Fit(d_gaus_pol1_fit,"NQ");
                // note : extract the signal region
                draw_d_gaus -> SetParameters(d_gaus_pol1_fit->GetParameter(0), d_gaus_pol1_fit->GetParameter(1), d_gaus_pol1_fit->GetParameter(2), d_gaus_pol1_fit->GetParameter(3), d_gaus_pol1_fit->GetParameter(4));
                // note : extract the part of pol1 background
                draw_pol1_line -> SetParameters(d_gaus_pol1_fit -> GetParameter(5), d_gaus_pol1_fit -> GetParameter(6));

                // note : fit the background region only by the pol2 function
                // note : p[0] + p[1]*(x-p[3])+p[2] * (x-p[3])^2
                bkg_fit_pol2 -> SetParameters(hist_offset, 0, -0.2, 0, signal_region);
                bkg_fit_pol2 -> FixParameter(4, signal_region);
                bkg_fit_pol2 -> SetParLimits(2, -100, 0);
                final_track_delta_phi_1D[i][i1] -> Fit(bkg_fit_pol2,"NQ");
                // note : extract the background region (which includes the signal region also)
                draw_pol2_line -> SetParameters(bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1), bkg_fit_pol2 -> GetParameter(2), bkg_fit_pol2 -> GetParameter(3));
            }

           

            final_track_delta_phi_1D[i][i1] -> SetMinimum(0);
            final_track_delta_phi_1D[i][i1] -> SetMaximum( final_track_delta_phi_1D[i][i1] -> GetBinContent(final_track_delta_phi_1D[i][i1] -> GetMaximumBin()) * 1.5);
            final_track_delta_phi_1D[i][i1] -> Draw("hist"); 
            // d_gaus_pol1_fit -> Draw("lsame");
            // draw_d_gaus -> Draw("lsame");
            // draw_pol1_line -> Draw("lsame");
            if (final_track_delta_phi_1D[i][i1] -> GetEntries() > 0) {draw_pol2_line -> Draw("lsame");}

            // gaus_pol1_fit -> Draw("lsame");
            // draw_gaus_line -> SetParameters(fabs(gaus_pol1_fit -> GetParameter(0)), gaus_pol1_fit -> GetParameter(1), fabs(gaus_pol1_fit -> GetParameter(2)), 0);
            // draw_gaus_line -> Draw("lsame");
            // draw_pol1_line -> SetParameters(gaus_pol1_fit -> GetParameter(3), gaus_pol1_fit -> GetParameter(4));
            // draw_pol1_line -> Draw("lsame");

            // cout<<" "<<endl;
            // final_eta_entry[i].push_back((draw_gaus_line -> GetParameter(1) - 3 * draw_gaus_line -> GetParameter(2)), draw_gaus_line -> GetParameter(1) + 3 * draw_gaus_line -> GetParameter(2));
            // cout<<i<<" "<<i1<<" gaus fit par  : "<<fabs(gaus_pol1_fit -> GetParameter(0))<<" "<<(gaus_pol1_fit -> GetParameter(1))<<" "<<fabs(gaus_pol1_fit -> GetParameter(2))<<endl;
            // double gaus_integral = fabs(draw_gaus_line -> Integral( (draw_gaus_line -> GetParameter(1) - 3 * fabs(draw_gaus_line -> GetParameter(2))), draw_gaus_line -> GetParameter(1) + 3 * fabs(draw_gaus_line -> GetParameter(2)) )) / final_track_delta_phi_1D[i][i1] -> GetBinWidth(1);
            // cout<<i<<" "<<i1<<" gaus integral : "<< gaus_integral <<endl;

            // double d_gaus_integral = fabs(draw_d_gaus -> Integral( -1. * signal_region, signal_region )) / final_track_delta_phi_1D[i][i1] -> GetBinWidth(1);
            // cout<<i<<" "<<i1<<" D-gaus integral : "<< d_gaus_integral <<endl;
            
            double pol2_bkg_integral = fabs(draw_pol2_line -> Integral( -1. * signal_region, signal_region )) / final_track_delta_phi_1D[i][i1] -> GetBinWidth(1);
            // cout<<i<<" "<<i1<<" pol2_bkg integral: "<<pol2_bkg_integral<<endl;

            int binID_X = eta_z_convert_inverse_map[i1].first;
            int binID_Y = eta_z_convert_inverse_map[i1].second;
            // final_dNdeta_1D[i]->SetBinContent(i1 + 1, good_tracklet_counting[i][i1] - pol2_bkg_integral );
            // final_dNdeta_1D[i]->SetBinContent(i1 + 1, good_tracklet_counting[i][i1] ); // note : no background subtraction
            if (binID_Y == tight_zvtx_bin) { final_dNdeta_1D[i]->SetBinContent(binID_X, good_tracklet_counting[i][i1] ); } // note : no background subtraction

            coord_line -> DrawLine(-1*signal_region, 0, -1 * signal_region, final_track_delta_phi_1D[i][i1] -> GetBinContent(final_track_delta_phi_1D[i][i1] -> GetMaximumBin()) * 1.5);
            coord_line -> DrawLine(signal_region, 0, signal_region, final_track_delta_phi_1D[i][i1] -> GetBinContent(final_track_delta_phi_1D[i][i1] -> GetMaximumBin()) * 1.5);
            
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
            draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s, #eta: %.2f ~ %.2f, Z: %.0f ~ %.0f mm",centrality_region[i].c_str(), eta_region_hist -> GetBinCenter(binID_X) - eta_region_hist -> GetBinWidth(binID_X)/2., eta_region_hist -> GetBinCenter(binID_X) + eta_region_hist -> GetBinWidth(binID_X)/2., coarse_eta_z_map -> GetYaxis() -> GetBinCenter(binID_Y) - coarse_eta_z_map -> GetYaxis() -> GetBinWidth(binID_Y)/2., coarse_eta_z_map -> GetYaxis() -> GetBinCenter(binID_Y) + coarse_eta_z_map -> GetYaxis() -> GetBinWidth(binID_Y)/2.));
            draw_text -> DrawLatex(0.21, 0.85, Form("MBin: %i, #eta bin: %i, Z bin: %i", i, binID_X, binID_Y));
            // draw_text -> DrawLatex(0.21, 0.85, Form("Guassian integral : %.2f", gaus_integral));
            // draw_text -> DrawLatex(0.21, 0.80, Form("D-Guassian integral : %.2f", d_gaus_integral));
            // draw_text -> DrawLatex(0.21, 0.80, Form("pol2: %.2f + %.2f(x-%.2f) + %.2f(x-%.2f)^{2}", bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1), bkg_fit_pol2 -> GetParameter(3), bkg_fit_pol2 -> GetParameter(2), bkg_fit_pol2 -> GetParameter(3)));
            c1 -> Print( Form("%s/final_track_delta_phi_1D_MBin%i.pdf", out_folder_directory.c_str(), i) );
            c1 -> Clear();
        }
        c1 -> Print( Form("%s/final_track_delta_phi_1D_MBin%i.pdf)", out_folder_directory.c_str(), i) );
    }

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    for (int i = 0; i < final_track_multi_delta_phi_1D.size(); i++)
    {
        c1 -> Print( Form("%s/final_track_multi_delta_phi_1D_MBin%i.pdf(", out_folder_directory.c_str(), i) );
        for (int i1 = 0; i1 < final_track_multi_delta_phi_1D[i].size(); i1++)
        {   
            double hist_offset = get_dist_offset(final_track_multi_delta_phi_1D[i][i1], 15);
            gaus_pol1_fit->SetParameters( final_track_multi_delta_phi_1D[i][i1] -> GetBinContent(final_track_multi_delta_phi_1D[i][i1] -> GetMaximumBin()) - hist_offset, 0, final_track_multi_delta_phi_1D[i][i1]->GetStdDev()/2., hist_offset, 0);
            if (final_track_multi_delta_phi_1D[i][i1] -> GetEntries() > 0) {final_track_multi_delta_phi_1D[i][i1] -> Fit(gaus_pol1_fit,"NQ");}

            // note : par[0] : size
            // note : par[1] : ratio of the two gaussians
            // note : par[2] : mean
            // note : par[3] : width of gaus 1
            // note : par[4] : width of gaus 2
            // note : par[5] : offset
            // note : par[6] : slope
            // note : fit with double gaussian function + pol1
            d_gaus_pol1_fit -> SetParameters(final_track_multi_delta_phi_1D[i][i1] -> GetBinContent(final_track_multi_delta_phi_1D[i][i1] -> GetMaximumBin()) - hist_offset, 0.2, 0, final_track_multi_delta_phi_1D[i][i1]->GetStdDev()/2., final_track_multi_delta_phi_1D[i][i1]->GetStdDev()/2., hist_offset, 0);
            d_gaus_pol1_fit -> SetParLimits(1, 0, 0.5);  // note : the first gaussian is  the main distribution, So it should contain more than 50% of the total distribution
            d_gaus_pol1_fit -> SetParLimits(3, 0, 1000); // note : the width of the gaussian should be positive
            d_gaus_pol1_fit -> SetParLimits(4, 0, 1000); // note : the width of the gaussian should be positive
            if (final_track_multi_delta_phi_1D[i][i1] -> GetEntries() > 0) {final_track_multi_delta_phi_1D[i][i1] -> Fit(d_gaus_pol1_fit,"NQ");}
            // note : extract the signal region
            draw_d_gaus -> SetParameters(d_gaus_pol1_fit->GetParameter(0), d_gaus_pol1_fit->GetParameter(1), d_gaus_pol1_fit->GetParameter(2), d_gaus_pol1_fit->GetParameter(3), d_gaus_pol1_fit->GetParameter(4));
            // note : extract the part of pol1 background
            draw_pol1_line -> SetParameters(d_gaus_pol1_fit -> GetParameter(5), d_gaus_pol1_fit -> GetParameter(6));

            // note : fit the background region only by the pol2 function
            // note : p[0] + p[1]*(x-p[3])+p[2] * (x-p[3])^2
            bkg_fit_pol2 -> SetParameters(hist_offset, 0, -0.2, 0, signal_region);
            bkg_fit_pol2 -> FixParameter(4, signal_region);
            bkg_fit_pol2 -> SetParLimits(2, -100, 0);
            if (final_track_multi_delta_phi_1D[i][i1] -> GetEntries() > 0) {final_track_multi_delta_phi_1D[i][i1] -> Fit(bkg_fit_pol2,"NQ");}
            // note : extract the background region (which includes the signal region also)
            draw_pol2_line -> SetParameters(bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1), bkg_fit_pol2 -> GetParameter(2), bkg_fit_pol2 -> GetParameter(3));

            final_track_multi_delta_phi_1D[i][i1] -> SetMinimum(0);
            final_track_multi_delta_phi_1D[i][i1] -> SetMaximum( final_track_multi_delta_phi_1D[i][i1] -> GetBinContent(final_track_multi_delta_phi_1D[i][i1] -> GetMaximumBin()) * 1.5);
            final_track_multi_delta_phi_1D[i][i1] -> Draw("hist"); 
            // d_gaus_pol1_fit -> Draw("lsame");
            // draw_d_gaus -> Draw("lsame");
            // draw_pol1_line -> Draw("lsame");
            draw_pol2_line -> Draw("lsame");

            // gaus_pol1_fit -> Draw("lsame");
            // draw_gaus_line -> SetParameters(fabs(gaus_pol1_fit -> GetParameter(0)), gaus_pol1_fit -> GetParameter(1), fabs(gaus_pol1_fit -> GetParameter(2)), 0);
            // draw_gaus_line -> Draw("lsame");
            // draw_pol1_line -> SetParameters(gaus_pol1_fit -> GetParameter(3), gaus_pol1_fit -> GetParameter(4));
            // draw_pol1_line -> Draw("lsame");

            // cout<<" "<<endl;
            // final_eta_entry[i].push_back((draw_gaus_line -> GetParameter(1) - 3 * draw_gaus_line -> GetParameter(2)), draw_gaus_line -> GetParameter(1) + 3 * draw_gaus_line -> GetParameter(2));
            // cout<<i<<" "<<i1<<" gaus fit par  : "<<fabs(gaus_pol1_fit -> GetParameter(0))<<" "<<(gaus_pol1_fit -> GetParameter(1))<<" "<<fabs(gaus_pol1_fit -> GetParameter(2))<<endl;
            double gaus_integral = fabs(draw_gaus_line -> Integral( (draw_gaus_line -> GetParameter(1) - 3 * fabs(draw_gaus_line -> GetParameter(2))), draw_gaus_line -> GetParameter(1) + 3 * fabs(draw_gaus_line -> GetParameter(2)) )) / final_track_multi_delta_phi_1D[i][i1] -> GetBinWidth(1);
            // cout<<i<<" "<<i1<<" gaus integral : "<< gaus_integral <<endl;

            double d_gaus_integral = fabs(draw_d_gaus -> Integral( -1. * signal_region, signal_region )) / final_track_multi_delta_phi_1D[i][i1] -> GetBinWidth(1);
            // cout<<i<<" "<<i1<<" D-gaus integral : "<< d_gaus_integral <<endl;
            
            double pol2_bkg_integral = fabs(draw_pol2_line -> Integral( -1. * signal_region, signal_region )) / final_track_multi_delta_phi_1D[i][i1] -> GetBinWidth(1);
            // cout<<i<<" "<<i1<<" pol2_bkg integral: "<<pol2_bkg_integral<<endl;

            // final_dNdeta_multi_1D[i]->SetBinContent(i1 + 1, good_tracklet_multi_counting[i][i1] - pol2_bkg_integral );
            int binID_X = eta_z_convert_inverse_map[i1].first;
            int binID_Y = eta_z_convert_inverse_map[i1].second;
            if (binID_Y == tight_zvtx_bin && final_track_multi_delta_phi_1D[i][i1] -> GetEntries() != 0) { final_dNdeta_multi_1D[i]->SetBinContent(binID_X, good_tracklet_multi_counting[i][i1] - pol2_bkg_integral ); } // note : no background subtraction

            coord_line -> DrawLine(-1*signal_region, 0, -1 * signal_region, final_track_multi_delta_phi_1D[i][i1] -> GetBinContent(final_track_multi_delta_phi_1D[i][i1] -> GetMaximumBin()) * 1.5);
            coord_line -> DrawLine(signal_region, 0, signal_region, final_track_multi_delta_phi_1D[i][i1] -> GetBinContent(final_track_multi_delta_phi_1D[i][i1] -> GetMaximumBin()) * 1.5);
            
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
            draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s, #eta: %.2f ~ %.2f, Z: %.0f ~ %.0f mm",centrality_region[i].c_str(), eta_region_hist -> GetBinCenter(binID_X) - eta_region_hist -> GetBinWidth(binID_X)/2., eta_region_hist -> GetBinCenter(binID_X) + eta_region_hist -> GetBinWidth(binID_X)/2., coarse_eta_z_map -> GetYaxis() -> GetBinCenter(binID_Y) - coarse_eta_z_map -> GetYaxis() -> GetBinWidth(binID_Y)/2., coarse_eta_z_map -> GetYaxis() -> GetBinCenter(binID_Y) + coarse_eta_z_map -> GetYaxis() -> GetBinWidth(binID_Y)/2.));
            draw_text -> DrawLatex(0.21, 0.85, Form("MBin: %i, #eta bin: %i, Z bin: %i", i, binID_X, binID_Y));
            // draw_text -> DrawLatex(0.21, 0.85, Form("Guassian integral : %.2f", gaus_integral));
            // draw_text -> DrawLatex(0.21, 0.80, Form("D-Guassian integral : %.2f", d_gaus_integral));
            draw_text -> DrawLatex(0.21, 0.80, Form("pol2: %.2f + %.2f(x-%.2f) + %.2f(x-%.2f)^{2}", bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1), bkg_fit_pol2 -> GetParameter(3), bkg_fit_pol2 -> GetParameter(2), bkg_fit_pol2 -> GetParameter(3)));
            c1 -> Print( Form("%s/final_track_multi_delta_phi_1D_MBin%i.pdf", out_folder_directory.c_str(), i) );
            c1 -> Clear();
        }
        c1 -> Print( Form("%s/final_track_multi_delta_phi_1D_MBin%i.pdf)", out_folder_directory.c_str(), i) );
    }

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/final_dNdeta_1D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < final_dNdeta_1D.size(); i++)
    {   
        double N_correction_evt_MC = (i == final_dNdeta_1D_MC.size() - 1) ? MBin_Z_evt_hist_2D_MC -> GetBinContent(MBin_Z_evt_hist_2D_MC -> GetNbinsX(),tight_zvtx_bin) : MBin_Z_evt_hist_2D_MC -> GetBinContent(i+1,tight_zvtx_bin);
        final_dNdeta_1D_MC[i] -> Scale(1./double(final_dNdeta_1D_MC[i] -> GetBinWidth(1) ));
        final_dNdeta_1D_MC[i] -> Scale(1./double(N_correction_evt_MC));
        
        double N_correction_evt = (i == final_dNdeta_1D_MC.size() - 1) ? MBin_Z_evt_hist_2D -> GetBinContent(MBin_Z_evt_hist_2D -> GetNbinsX(),tight_zvtx_bin) : MBin_Z_evt_hist_2D -> GetBinContent(i+1,tight_zvtx_bin);
        final_dNdeta_1D[i] -> Scale(1./double(final_dNdeta_1D[i] -> GetBinWidth(1) ));
        final_dNdeta_1D[i] -> Scale(1./double(N_correction_evt));
        final_dNdeta_1D[i] -> GetYaxis() -> SetRangeUser(0, final_dNdeta_1D[i] -> GetMaximum() * 1.5);

        final_dNdeta_multi_1D[i] -> Scale(1./double(final_dNdeta_multi_1D[i] -> GetBinWidth(1) ));
        final_dNdeta_multi_1D[i] -> Scale(1./double(N_correction_evt));

        // final_dNdeta_1D[i] -> SetMinimum(0);
        final_dNdeta_1D[i] -> Draw("ep");
        final_dNdeta_1D_MC[i] -> Draw("hist same");
        final_dNdeta_multi_1D[i] -> Draw("ep same");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s, Z: %.2f ~ %.2f",centrality_region[i].c_str(), coarse_eta_z_map -> GetYaxis() -> GetBinCenter(tight_zvtx_bin) - coarse_eta_z_map -> GetYaxis() -> GetBinWidth(tight_zvtx_bin)/2., coarse_eta_z_map -> GetYaxis() -> GetBinCenter(tight_zvtx_bin) + coarse_eta_z_map -> GetYaxis() -> GetBinWidth(tight_zvtx_bin)/2.));
        draw_text -> DrawLatex(0.21, 0.86, Form("Nevt MC : %.1f, Nevt reco : %.1f",N_correction_evt_MC, N_correction_evt));
        draw_text -> DrawLatex(0.21, 0.82, Form("MC entry : %.2f, tight entry : %.2f, loose entry : %.2f", final_dNdeta_1D_MC[i] -> Integral(), final_dNdeta_1D[i] -> Integral(), final_dNdeta_multi_1D[i] -> Integral()));
        c1 -> Print(Form("%s/final_dNdeta_1D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/final_dNdeta_1D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    track_cluster_ratio_multiplicity_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/track_cluster_ratio_multiplicity_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    track_cluster_ratio_multiplicity_2D_MC -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/track_cluster_ratio_multiplicity_2D_MC.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    track_correlation_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    correlation_Line -> Draw("lsame");
    c1 -> Print( Form("%s/track_correlation_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    track_ratio_2D -> Draw("colz0");
    coord_line -> DrawLine( track_ratio_2D->GetXaxis()->GetXmin(), 1, track_ratio_2D->GetXaxis()->GetXmax(), 1 );
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    correlation_Line -> Draw("lsame");
    c1 -> Print( Form("%s/track_ratio_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    reco_eta_correlation_2D -> Draw("colz0");
    correlation_Line -> Draw("lsame");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/reco_eta_correlation_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    reco_eta_diff_reco3P_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/reco_eta_diff_reco3P_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    reco_eta_diff_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/reco_eta_diff_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_DeltaPhi_eta_2D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_DeltaPhi_eta_2D.size(); i++)
    {
        track_DeltaPhi_eta_2D[i] -> Draw("colz0");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/track_DeltaPhi_eta_2D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_DeltaPhi_eta_2D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_DeltaPhi_DeltaEta_2D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_DeltaPhi_DeltaEta_2D.size(); i++)
    {
        track_DeltaPhi_DeltaEta_2D[i] -> Draw("colz0");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/track_DeltaPhi_DeltaEta_2D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_DeltaPhi_DeltaEta_2D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // c1 -> Print( Form("%s/dNdeta_1D_MC.pdf(", out_folder_directory.c_str()) );
    // for (int i = 0; i < dNdeta_1D_MC.size(); i++)
    // {
    //     // dNdeta_1D_MC[i] -> Scale(1./double(dNdeta_1D_MC[i] -> GetBinWidth(1) ));
    //     // double N_correction_evt = (i == dNdeta_1D_MC.size() - 1) ? N_GoodEvent : N_GoodEvent_vec[i];
    //     // dNdeta_1D_MC[i] -> Scale(1./double(N_GoodEvent));
    //     dNdeta_1D_MC[i] -> GetYaxis() -> SetRangeUser(0,800);
    //     dNdeta_1D_MC[i] -> Draw("hist");
    //     ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    //     draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
    //     c1 -> Print(Form("%s/dNdeta_1D_MC.pdf", out_folder_directory.c_str()));
    //     c1 -> Clear();    
    // }
    // c1 -> Print( Form("%s/dNdeta_1D_MC.pdf)", out_folder_directory.c_str()) );


    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/track_eta_z_2D_MC.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < track_eta_z_2D_MC.size(); i++)
    {
        track_eta_z_2D_MC[i] -> Draw("colz0");
        DrawEtaZGrid();
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/track_eta_z_2D_MC.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/track_eta_z_2D_MC.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    clu_used_centrality_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/clu_used_centrality_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : for the mega cluster study
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    cluster4_track_phi_1D -> Draw("hist");
    cluster4_track_phi_1D -> SetMinimum(0);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/cluster4_track_phi_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    cluster3_track_phi_1D -> Draw("hist");
    cluster3_track_phi_1D -> SetMinimum(0);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/cluster3_track_phi_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    cluster3_inner_track_phi_1D -> Draw("hist");
    cluster3_inner_track_phi_1D -> SetMinimum(0);
    for (int i = 0; i < cluster3_inner_track_phi_1D -> GetNbinsX(); i++)
    {
        if (cluster3_inner_track_phi_1D -> GetBinContent(i+1) > 200)
        {
            cout<<i+1<<" mega Inner track phi bin center: "<<cluster3_inner_track_phi_1D -> GetBinCenter(i+1)<<" bin content: "<<cluster3_inner_track_phi_1D -> GetBinContent(i+1)<<endl;
        }
    }

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/cluster3_inner_track_phi_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    cluster3_outer_track_phi_1D -> Draw("hist");
    cluster3_outer_track_phi_1D -> SetMinimum(0);
    for (int i = 0; i < cluster3_outer_track_phi_1D -> GetNbinsX(); i++)
    {
        if (cluster3_outer_track_phi_1D -> GetBinContent(i+1) > 200)
        {
            cout<<i+1<<" mega Outer track phi bin center: "<<cluster3_outer_track_phi_1D -> GetBinCenter(i+1)<<" bin content: "<<cluster3_outer_track_phi_1D -> GetBinContent(i+1)<<endl;
        }
    }
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/cluster3_outer_track_phi_1D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    NClu4_track_centrality_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/NClu4_track_centrality_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    NClu3_track_centrality_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/NClu3_track_centrality_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    coarse_eta_z_map -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/coarse_eta_z_map.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/coarse_eta_z_2D_MC.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < coarse_eta_z_2D_MC.size(); i++)
    {
        coarse_eta_z_2D_MC[i] -> Draw("colz0");
        DrawEtaZGrid();
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/coarse_eta_z_2D_MC.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/coarse_eta_z_2D_MC.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    MBin_Z_evt_hist_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/MBin_Z_evt_hist_2D.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    MBin_Z_evt_hist_2D_MC -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/MBin_Z_evt_hist_2D_MC.pdf", out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/coarse_Reco_SignalNTracklet_Single_eta_z_2D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < coarse_Reco_SignalNTracklet_Single_eta_z_2D.size(); i++)
    {
        coarse_Reco_SignalNTracklet_Single_eta_z_2D[i] -> Draw("colz0");
        DrawEtaZGrid();
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/coarse_Reco_SignalNTracklet_Single_eta_z_2D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/coarse_Reco_SignalNTracklet_Single_eta_z_2D.pdf)", out_folder_directory.c_str()) );

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> Print( Form("%s/coarse_Reco_SignalNTracklet_Multi_eta_z_2D.pdf(", out_folder_directory.c_str()) );
    for (int i = 0; i < coarse_Reco_SignalNTracklet_Multi_eta_z_2D.size(); i++)
    {
        coarse_Reco_SignalNTracklet_Multi_eta_z_2D[i] -> Draw("colz0");
        DrawEtaZGrid();
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        c1 -> Print(Form("%s/coarse_Reco_SignalNTracklet_Multi_eta_z_2D.pdf", out_folder_directory.c_str()));
        c1 -> Clear();    
    }
    c1 -> Print( Form("%s/coarse_Reco_SignalNTracklet_Multi_eta_z_2D.pdf)", out_folder_directory.c_str()) );
}

void INTTEta::EndRun()
{
    for (int i = 0; i < N_GoodEvent_vec.size(); i++)
    {
        cout<<"N good evt break down : "<< i <<" "<<N_GoodEvent_vec[i]<<endl;
    }

    cout<<"N good evt inclusive : "<<N_GoodEvent<<endl;

    out_file -> cd();
    tree_out -> SetDirectory(out_file);

    // note : write the histogram of DeltaPhi for each centrality bin, each eta bin and each z bin
    // note : for the one that allows single cluster getting involved in multiple tracks
    for (int i = 0; i < final_track_multi_delta_phi_1D.size(); i++)
    {
        for (int i1 = 0; i1 < final_track_multi_delta_phi_1D[i].size(); i1++)
        {  
            final_track_multi_delta_phi_1D[i][i1] -> Write(Form("Reco_DeltaPhi1D_Multi_MBin%i_Eta%i_Z%i", i, eta_z_convert_inverse_map[i1].first, eta_z_convert_inverse_map[i1].second));
        }
    }

    // note : write the histogram of DeltaPhi for each centrality bin, each eta bin and each z bin
    // note : for the ones that only allow single cluster to be used in one track
    for (int i = 0; i < final_track_delta_phi_1D.size(); i++)
    {
        for (int i1 = 0; i1 < final_track_delta_phi_1D[i].size(); i1++)
        {  
            final_track_delta_phi_1D[i][i1] -> Write(Form("Reco_DeltaPhi1D_Single_MBin%i_Eta%i_Z%i", i, eta_z_convert_inverse_map[i1].first, eta_z_convert_inverse_map[i1].second));
        }
    }

    // note : write the histogram to the root file, the one containing the N reco tracks in signal region
    for (int i = 0; i < coarse_Reco_SignalNTracklet_Multi_eta_z_2D.size(); i++)
    {
        coarse_Reco_SignalNTracklet_Multi_eta_z_2D[i] -> Write(Form("Reco_SignalNTracklet_Multi_MBin%i",i));
    }
    for (int i = 0; i < coarse_Reco_SignalNTracklet_Single_eta_z_2D.size(); i++)
    {
        coarse_Reco_SignalNTracklet_Single_eta_z_2D[i] -> Write(Form("Reco_SignalNTracklet_Single_MBin%i",i));
    }

    // note : number of true track for each centrality bin, each eta bin and each z bin
    for (int i = 0; i < coarse_eta_z_2D_MC.size(); i++) 
    { 
        coarse_eta_z_2D_MC[i] -> Write(Form("NTrueTrack_MBin%i",i));
    }

    // note : 
    for (int i = 0; i < coarse_eta_z_2D_fulleta_MC.size(); i++)
    {
        coarse_eta_z_2D_fulleta_MC[i] -> Write(Form("NTrueTrack_FullEta_MBin%i",i));
    }

    // note : the map for the eta-z reference
    coarse_eta_z_map -> Write("Eta_Z_reference");
    
    // note : the number of event for each centrality bin and z vertex bin
    MBin_Z_evt_hist_2D -> Write("MBin_Z_evt_map");

    // note : the number of event for each centrality bin and z vertex bin, the zvtx is from MC
    MBin_Z_evt_hist_2D_MC -> Write("MBin_Z_evt_map_MC");

    // note : to save the fine binning 2D eta-z track historgram 
    for (int i = 0; i < track_eta_z_2D_MC.size(); i++) 
    { 
        track_eta_z_2D_MC[i] -> Write(Form("Fine_TrueTrackEtaZ_MBin%i",i));
    }

    for (int i = 0; i < track_eta_z_2D.size(); i++) 
    { 
        track_eta_z_2D[i] -> Write(Form("Fine_RecoTrackEtaZ_MBin%i",i));
    }    

    // note : to save the 2D histogram, the clu_used_centrality_2D
    clu_used_centrality_2D -> Write("CluUsed_Centrality2D");

    // note : for the mega tracklet study
    // note : save the 2D histogram for
    cluster3_track_phi_1D -> Write("cluster3_track_phi_1D");
    cluster3_inner_track_phi_1D -> Write("cluster3_inner_track_phi_1D");
    cluster3_outer_track_phi_1D -> Write("cluster3_outer_track_phi_1D");
    cluster4_track_phi_1D -> Write("cluster4_track_phi_1D");
    NClu3_track_centrality_2D -> Write("NClu3_track_centrality_2D");
    NClu4_track_centrality_2D -> Write("NClu4_track_centrality_2D");
    clu3_track_ReducedChi2_1D -> Write("clu3_track_ReducedChi2_1D");
    clu4_track_ReducedChi2_1D -> Write("clu4_track_ReducedChi2_1D");
    
    for (int i = 0; i < track_cluster_ratio_1D_MC.size(); i++){track_cluster_ratio_1D_MC[i] -> Write(Form("track_cluster_ratio_1D_MC_MBin%i",i));}
    for (int i = 0; i < track_cluster_ratio_1D.size(); i++) {track_cluster_ratio_1D[i] -> Write(Form("track_cluster_ratio_1D_MBin%i",i));}

    track_cluster_ratio_multiplicity_2D_MC -> Write("track_cluster_ratio_multiplicity_2D_MC");
    track_cluster_ratio_multiplicity_2D -> Write("track_cluster_ratio_multiplicity_2D");
    track_correlation_2D -> Write("track_correlation_2D");

    for (int i = 0; i < track_DeltaPhi_eta_2D.size(); i++){ track_DeltaPhi_eta_2D[i] -> Write(Form("track_DeltaPhi_eta_2D_MBin%i",i)); }
    for (int i = 0; i < track_eta_phi_2D.size(); i++){ track_eta_phi_2D[i] -> Write(Form("track_eta_phi_2D_MBin%i",i)); }
    for (int i = 0; i < track_ratio_1D.size(); i++){ track_ratio_1D[i] -> Write(Form("track_ratio_1D_MBin%i",i)); }
    track_ratio_2D -> Write("track_ratio_2D");

    for (int i = 0; i < dNdeta_1D.size(); i++) {dNdeta_1D[i] -> Write(Form("FineBin_Ntracklet_MBin%i",i));}
    for (int i = 0; i < dNdeta_1D_MC.size(); i++) {dNdeta_1D_MC[i] -> Write(Form("FineBin_NTrueTrack_MBin%i",i));}
    for (int i = 0; i < dNdeta_1D_MC_edge_eta_cut.size(); i++) {dNdeta_1D_MC_edge_eta_cut[i] -> Write(Form("FineBin_NTrueTrack_EdgeEtaCut_MBin%i",i));}

    exclusive_NClus_inner -> Write("exclusive_NClus_inner");
    exclusive_NClus_outer -> Write("exclusive_NClus_outer");
    exclusive_NClus_sum -> Write("exclusive_NClus_sum");
    
    exclusive_cluster_inner_eta -> Write("exclusive_cluster_inner_eta");
    exclusive_cluster_inner_phi -> Write("exclusive_cluster_inner_phi");
    exclusive_cluster_outer_eta -> Write("exclusive_cluster_outer_eta");
    exclusive_cluster_outer_phi -> Write("exclusive_cluster_outer_phi");
    exclusive_cluster_all_eta -> Write("exclusive_cluster_all_eta");
    exclusive_cluster_all_phi -> Write("exclusive_cluster_all_phi");

    exclusive_cluster_inner_eta_phi_2D -> Write("exclusive_cluster_inner_eta_phi_2D");
    exclusive_cluster_outer_eta_phi_2D -> Write("exclusive_cluster_outer_eta_phi_2D");

    exclusive_tight_tracklet_eta -> Write("exclusive_tight_tracklet_eta");
    exclusive_tight_tracklet_phi -> Write("exclusive_tight_tracklet_phi");
    exclusive_loose_tracklet_eta -> Write("exclusive_loose_tracklet_eta");
    exclusive_loose_tracklet_phi -> Write("exclusive_loose_tracklet_phi");

    exclusive_cluster_inner_eta_adc_2D -> Write("exclusive_cluster_inner_eta_adc_2D");
    exclusive_cluster_outer_eta_adc_2D -> Write("exclusive_cluster_outer_eta_adc_2D");

    exclusive_cluster_inner_adc -> Write("exclusive_cluster_inner_adc");
    exclusive_cluster_outer_adc -> Write("exclusive_cluster_outer_adc");

    tree_out -> Write("", TObject::kOverwrite);

    out_file -> Close();

    std::cout<<"in N event : "<<N_GoodEvent<<"the total N reco tracks is short in : "<<N_reco_cluster_short<<std::endl;

    return;
}

//note : accumulate the number of entries from both sides of the histogram
// double INTTEta::get_dist_offset(TH1F * hist_in, int check_N_bin) // note : check_N_bin 1 to N bins of hist
// {
//     if (check_N_bin < 0 || check_N_bin > hist_in -> GetNbinsX()) {cout<<" wrong check_N_bin "<<endl; exit(1);}
//     double total_entry = 0;
//     for (int i = 0; i < check_N_bin; i++)
//     {
//         total_entry += hist_in -> GetBinContent(i+1); // note : 1, 2, 3.....
//         total_entry += hist_in -> GetBinContent(hist_in -> GetNbinsX() - i);
//     }

//     return total_entry/double(2. * check_N_bin);
// }


double INTTEta::grEY_stddev(TGraphErrors * input_grr)
{
    vector<double> input_vector; input_vector.clear();
    for (int i = 0; i < input_grr -> GetN(); i++) {input_vector.push_back( input_grr -> GetPointY(i) );}

    double average = accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());

    double sum_subt = 0;
	for (int ele : input_vector) {sum_subt+=pow((ele-average),2);}
	
	return sqrt(sum_subt/(input_vector.size()-1));
}	


pair<double, double> INTTEta::mirrorPolynomial(double a, double b) {
    // Interchange 'x' and 'y'
    double mirroredA = 1.0 / a;
    double mirroredB = -b / a;

    return {mirroredA, mirroredB};
}

// note : pair<reduced chi2, eta of the track>
// note : vector : {r,z}
// note : p0 : vertex point {r,z,zerror}
// note : p1 : inner layer
// note : p2 : outer layer
pair<double,double> INTTEta::Get_eta(vector<double>p0, vector<double>p1, vector<double>p2)
{
    // if (track_gr -> GetN() != 0) {cout<<"In INTTEta class, track_gr is not empty, track_gr size : "<<track_gr -> GetN()<<endl; exit(0);}
    
    if (track_gr -> GetN() != 0) {track_gr -> Set(0);}
    
    vector<double> r_vec  = {p0[0], p1[0], p2[0]}; 
    vector<double> z_vec  = {p0[1], p1[1], p2[1]}; 
    vector<double> re_vec = {0, 0, 0}; 
    vector<double> ze_vec = {p0[2], ( fabs( p1[1] ) < 130 ) ? 8. : 10., ( fabs( p2[1] ) < 130 ) ? 8. : 10.}; 

    // note : swap the r and z, to have easier fitting 
    // note : in principle, Z should be in the x axis, R should be in the Y axis
    for (int i = 0; i < 3; i++)
    {
        track_gr -> SetPoint(i,r_vec[i],z_vec[i]);
        track_gr -> SetPointError(i,re_vec[i],ze_vec[i]);

        // cout<<"In INTTEta class, point : "<<i<<" r : "<<r_vec[i]<<" z : "<<z_vec[i]<<" re : "<<re_vec[i]<<" ze : "<<ze_vec[i]<<endl;
    }    
    
    double vertical_line = ( fabs( grEY_stddev(track_gr) ) < 0.00001 ) ? 1 : 0;
    
    if (vertical_line) {return {0,0};}
    else 
    {
        fit_rz -> SetParameters(0,0);

        track_gr -> Fit(fit_rz,"NQ");

        pair<double,double> ax_b = mirrorPolynomial(fit_rz -> GetParameter(1),fit_rz -> GetParameter(0));

        return  {(fit_rz -> GetChisquare() / double(fit_rz -> GetNDF())), -1 * TMath::Log( fabs( tan( atan2(ax_b.first, (ax_b.first > 0) ? 1. : -1. ) / 2 ) ) )};
    }

}

// note : angle_1 = inner clu phi
// note: angle_2 = outer clu phi
double INTTEta::get_delta_phi(double angle_1, double angle_2)
{
    vector<double> vec_abs = {fabs(angle_1 - angle_2), fabs(angle_1 - angle_2 + 360), fabs(angle_1 - angle_2 - 360)};
    vector<double> vec = {(angle_1 - angle_2), (angle_1 - angle_2 + 360), (angle_1 - angle_2 - 360)};
    return vec[std::distance(vec_abs.begin(), std::min_element(vec_abs.begin(),vec_abs.end()))];
}

double INTTEta::get_track_phi(double inner_clu_phi_in, double delta_phi_in)
{
    double track_phi = inner_clu_phi_in - (delta_phi_in/2.);
    if (track_phi < 0) {track_phi += 360;}
    else if (track_phi > 360) {track_phi -= 360;}
    else if (track_phi == 360) {track_phi = 0;}
    else {track_phi = track_phi;}
    return track_phi;
}

double INTTEta::convertTo360(double radian) {

    double angle = radian * (180./M_PI);
    
    if (fabs(radian) == M_PI) {return 180.;}
    else if (radian == 0)
    {
        return 0;
    }
    else if ( radian > 0 )
    {
        return angle;
    }
    else
    {
        return angle + 360;
    }
}

double INTTEta::get_clu_eta(vector<double> vertex, vector<double> clu_pos)
{
    double correct_x = clu_pos[0] - vertex[0];
    double correct_y = clu_pos[1] - vertex[1];
    double correct_z = clu_pos[2] - vertex[2];
    double clu_r = sqrt(pow(correct_x,2) + pow(correct_y,2));
    // cout<<"correct info : "<<correct_x<<" "<<correct_y<<" "<<correct_z<<" "<<clu_r<<endl;

    return -0.5 * TMath::Log((sqrt(pow(correct_z,2)+pow(clu_r,2))-(correct_z)) / (sqrt(pow(correct_z,2)+pow(clu_r,2))+(correct_z)));
}

pair<int,int> INTTEta::GetTH2BinXY(int histNbinsX, int histNbinsY, int binglobal)
{
    // cout<<"so detail 1 "<<endl;
    if (binglobal == -1) {return {-1,-1};}
    // cout<<"so detail 2 "<<endl;
    int nx  = histNbinsX+2;
    int ny  = histNbinsY+2;
    // cout<<"so detail 3 "<<endl;
    int binx = binglobal%nx;
    int biny = ((binglobal-binx)/nx)%ny;
    // cout<<"so detail 4 "<<endl;
    // cout<<binx<<" "<<biny<<endl;

    // note : the binx and biny start from 1
    return {binx, biny};
}

double INTTEta::GetTH2Index1D(pair<int,int> XY_index, int histNbinsX)
{
    if (XY_index.first == -1) {return -1;}

    // note : XY_index.first : x index, starts from 1
    // note : XY_index.second : y index, starts from 1
    return (XY_index.first - 1) + (XY_index.second - 1) * histNbinsX;
}

void INTTEta::DrawEtaZGrid()
{
    // note : draw the vertical line, to segment the eta region
    for (int i = 0; i < eta_region.size(); i++) { coord_line -> DrawLine(eta_region[i], z_region[0], eta_region[i], z_region[z_region.size() - 1]); }

    // note : draw the horizontal line, to segment the z region
    for (int i = 0; i < z_region.size(); i++) { coord_line -> DrawLine(eta_region[0], z_region[i], eta_region[eta_region.size() - 1], z_region[i]); }
}

double INTTEta::get_TH1F_Entries(TH1F * hist_in)
{
    double entry_counting = 0; 
    for (int i = 0; i < hist_in -> GetNbinsX(); i++) {entry_counting += hist_in -> GetBinContent(i+1);}

    return entry_counting;
}

#endif


// // note : this function is for the event by event vertex calculation
// // note : this function is the new method, which means that we first put the cluster into a phi map, and then there are two for loops still, but we only check the certain range of the phi 
// // note : try the method that allows single cluster to be used multiple times
// // note : so the background subtraction is needed
// void INTTEta::ProcessEvt(int event_i, vector<clu_info> temp_sPH_inner_nocolumn_vec, vector<clu_info> temp_sPH_outer_nocolumn_vec, vector<vector<double>> temp_sPH_nocolumn_vec, vector<vector<double>> temp_sPH_nocolumn_rz_vec, int NvtxMC, vector<double> TrigvtxMC, bool PhiCheckTag, Long64_t bco_full, pair<double,double> evt_z, int centrality_bin, vector<vector<float>> true_track_info){ // note : evt_z : {z, width}
//     return_tag = 0;

//     if (event_i%1 == 0) {cout<<"In INTTEta class, running event : "<<event_i<<endl;}

//     inner_NClu = temp_sPH_inner_nocolumn_vec.size();
//     outer_NClu = temp_sPH_outer_nocolumn_vec.size();
//     total_NClus = inner_NClu + outer_NClu;

//     // cout<<"test_0"<<endl;
//     if (total_NClus < zvtx_cal_require) {return; cout<<"return confirmation"<<endl;}   
    
//     if (run_type == "MC" && NvtxMC != 1) { return; cout<<"In INTTEta class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Nvtx more than one "<<endl;}
//     if (PhiCheckTag == false)            { return; cout<<"In INTTEta class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Not full phi has hits "<<endl;}
    
//     if (inner_NClu < 10 || outer_NClu < 10 || total_NClus > N_clu_cut || total_NClus < N_clu_cutl)
//     {
//         return;
//         printf("In INTTEta class, event : %i, low clu continue, NClus : %lu \n", event_i, total_NClus); 
//     }

//     // todo : the z vertex range is here
//     if (-220 > evt_z.first + evt_z.second || -180 < evt_z.first - evt_z.second) {return;}
//     // if (-100 > evt_z.first + evt_z.second || 100 < evt_z.first - evt_z.second) {return;}
    
    
//     N_GoodEvent += 1;
//     N_GoodEvent_vec[centrality_map[centrality_bin]] += 1;

//     // cout<<"N inner cluster : "<<inner_NClu<<" N outer cluster : "<<outer_NClu<<endl;

//     double INTT_eta_acceptance_l = -0.5 * TMath::Log((sqrt(pow(-230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))-(-230.-TrigvtxMC[2]*10.)) / (sqrt(pow(-230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))+(-230.-TrigvtxMC[2]*10.))); // note : left
//     double INTT_eta_acceptance_r =  -0.5 * TMath::Log((sqrt(pow(230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))-(230.-TrigvtxMC[2]*10.)) / (sqrt(pow(230.-TrigvtxMC[2]*10.,2)+pow(INTT_layer_R[3],2))+(230.-TrigvtxMC[2]*10.))); // note : right

//     if (run_type == "MC")
//     {
//         // note : for the true track case ----------------------------------------------------------------------------------------------------------------------------------------------------------------
//         // if (event_i % 100 == 0){cout<<"z : "<<TrigvtxMC[2]*10.<<" eta : "<<INTT_eta_acceptance_l<<" "<<INTT_eta_acceptance_r<<endl;}

//         // cout<<"true_track_info : "<<true_track_info.size()<<endl;    
//         for (int track_i = 0; track_i < true_track_info.size(); track_i++)
//         {
//             if (true_track_info[track_i][2] == 111 || true_track_info[track_i][2] == 22 || abs(true_track_info[track_i][2]) == 2112){continue;}

//             if (true_track_info[track_i][0] > INTT_eta_acceptance_l && true_track_info[track_i][0] < INTT_eta_acceptance_r)
//             {
//                 dNdeta_1D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0]);
//                 dNdeta_1D_MC[dNdeta_1D_MC.size() - 1]        -> Fill(true_track_info[track_i][0]);   
//                 final_dNdeta_1D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0]);
//                 final_dNdeta_1D_MC[dNdeta_1D_MC.size() - 1]        -> Fill(true_track_info[track_i][0]);
//                 track_eta_z_2D_MC[centrality_map[centrality_bin]] -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);
//                 track_eta_z_2D_MC[track_eta_z_2D_MC.size() - 1]   -> Fill(true_track_info[track_i][0], TrigvtxMC[2]*10.);    

//                 // cout<<"true track eta : "<<true_track_info[track_i][0]<<" phi : "<<convertTo360(true_track_info[track_i][1])<<endl;
//                 // cout<<"("<<true_track_info[track_i][0]<<", "<<convertTo360(true_track_info[track_i][1])<<"), ";

//                 evt_true_track_gr -> SetPoint(evt_true_track_gr->GetN(),true_track_info[track_i][0], convertTo360(true_track_info[track_i][1]));

//                 evt_NTrack_MC += 1;
//             }
//         }
//         // if (evt_NTrack_MC < 10) {cout<<"evt : "<<event_i<<" ---- N reco track : "<<evt_NTrack<<" N true track : "<<evt_NTrack_MC<<" ratio : "<<double(evt_NTrack) / double(evt_NTrack_MC)<<endl;}
//     }

//     // note : put the cluster into the phi map, the first bool is for the cluster usage.
//     // note : false means the cluster is not used
//     for (int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++) {
//         Clus_InnerPhi_Offset = (temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second < 0) ? atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi());
//         inner_clu_phi_map[ int(Clus_InnerPhi_Offset) ].push_back({false,temp_sPH_inner_nocolumn_vec[inner_i]});

//         double clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y, temp_sPH_inner_nocolumn_vec[inner_i].z});
//         if (clu_eta > INTT_eta_acceptance_l && clu_eta < INTT_eta_acceptance_r) {effective_total_NClus += 1;}
//     }
//     for (int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++) {
//         Clus_OuterPhi_Offset = (temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second < 0) ? atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi());
//         outer_clu_phi_map[ int(Clus_OuterPhi_Offset) ].push_back({false,temp_sPH_outer_nocolumn_vec[outer_i]});

//         double clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y, temp_sPH_outer_nocolumn_vec[outer_i].z});
//         if (clu_eta > INTT_eta_acceptance_l && clu_eta < INTT_eta_acceptance_r) {effective_total_NClus += 1;}
//     }

//     // // note : find the Mega cluster preparation for inner barrel 
//     // for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++)
//     // {
//     //     for (int inner_phi_clu_i = 0; inner_phi_clu_i < inner_clu_phi_map[inner_phi_i].size(); inner_phi_clu_i++)
//     //     {
//     //         // note : if the cluster is used, then skip
//     //         if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].first == true) {continue;}

//     //         Clus_InnerPhi_Offset_1 = (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second < 0) ? atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());

//     //         // note : the scan range of the -1, 0, 1
//     //         for (int scan_i = -1; scan_i < 2; scan_i++)
//     //         {
//     //             int true_scan_i = ((inner_phi_i + scan_i) < 0) ? 360 + (inner_phi_i + scan_i) : ((inner_phi_i + scan_i) > 359) ? (inner_phi_i + scan_i)-360 : inner_phi_i + scan_i;
//     //             for (int inner_scan_clu_i = 0; inner_scan_clu_i < inner_clu_phi_map[true_scan_i].size(); inner_scan_clu_i++)
//     //             {
//     //                 // note : if the cluster is used, then skip
//     //                 if (inner_clu_phi_map[true_scan_i][inner_scan_clu_i].first == true) {continue;}

//     //                 // note : the cluster itself
//     //                 if (true_scan_i == inner_phi_i && inner_phi_clu_i == inner_scan_clu_i) {continue;}
                    
//     //                 // note : if it has the same sub layer ID, then skip
//     //                 // todo : this cut may only work for the MC, for the data, it requires additional check
//     //                 if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.layer == inner_clu_phi_map[true_scan_i][inner_scan_clu_i].second.layer) {continue;}

//     //                 // note : I expect the two cluster have to have the same z position
//     //                 // note : but in case of data, the z position of the same strip may be fluctuated a little bit 
//     //                 // todo : currently, set the Z position flutuation to be 4 mm
//     //                 if (fabs(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z - inner_clu_phi_map[true_scan_i][inner_scan_clu_i].second.z) > 4 ) {continue;}

//     //                 Clus_InnerPhi_Offset_2 = (inner_clu_phi_map[true_scan_i][inner_scan_clu_i].second.y - beam_origin.second < 0) ? atan2(inner_clu_phi_map[true_scan_i][inner_scan_clu_i].second.y - beam_origin.second, inner_clu_phi_map[true_scan_i][inner_scan_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(inner_clu_phi_map[true_scan_i][inner_scan_clu_i].second.y - beam_origin.second, inner_clu_phi_map[true_scan_i][inner_scan_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());

//     //                 double delta_phi = get_delta_phi(Clus_InnerPhi_Offset_1, Clus_InnerPhi_Offset_2);

//     //                 // note : if the two cluster are too far away in phi angle, the skip
//     //                 // todo : the delta phi cut may need to be adjusted
//     //                 if (fabs( delta_phi ) > 0.6) {continue;}

//     //                 // note : the pair that passed the cut mentioned above can be considered as a proto_mega_cluster
//     //                 Mega_inner_clu_pair_index.push_back({{inner_phi_i, inner_phi_clu_i, true_scan_i, inner_scan_clu_i}});
//     //                 Mega_inner_clu_delta_phi_abs.push_back(fabs(delta_phi));
//     //                 Mega_inner_clu_phi.push_back( get_track_phi(Clus_InnerPhi_Offset_1, delta_phi) );
//     //             }
//     //         }
//     //     }
//     // }

//     // // note : finr the Mega cluste preparation for outer barrel 
//     // for (int outer_phi_i = 0; outer_phi_i < 360; outer_phi_i++)
//     // {
//     //     for (int outer_phi_clu_i = 0; outer_phi_clu_i < outer_clu_phi_map[outer_phi_i].size(); outer_phi_clu_i++)
//     //     {
//     //         // note : if the cluster is used, then skip
//     //         if (outer_clu_phi_map[outer_phi_i][outer_phi_clu_i].first == true) {continue;}

//     //         Clus_OuterPhi_Offset_1 = (outer_clu_phi_map[outer_phi_i][outer_phi_clu_i].second.y - beam_origin.second < 0) ? atan2(outer_clu_phi_map[outer_phi_i][outer_phi_clu_i].second.y - beam_origin.second, outer_clu_phi_map[outer_phi_i][outer_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(outer_clu_phi_map[outer_phi_i][outer_phi_clu_i].second.y - beam_origin.second, outer_clu_phi_map[outer_phi_i][outer_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());

//     //         // note : the scan range of the -1, 0, 1
//     //         for (int scan_i = -1; scan_i < 2; scan_i++)
//     //         {
//     //             int true_scan_i = ((outer_phi_i + scan_i) < 0) ? 360 + (outer_phi_i + scan_i) : ((outer_phi_i + scan_i) > 359) ? (outer_phi_i + scan_i)-360 : outer_phi_i + scan_i;
//     //             for (int outer_scan_clu_i = 0; outer_scan_clu_i < outer_clu_phi_map[true_scan_i].size(); outer_scan_clu_i++)
//     //             {
//     //                 // note : if the cluster is used, then skip
//     //                 if (outer_clu_phi_map[true_scan_i][outer_scan_clu_i].first == true) {continue;}

//     //                 // note : the cluster itself
//     //                 if (true_scan_i == outer_phi_i && outer_phi_clu_i == outer_scan_clu_i) {continue;}
                    
//     //                 // note : if it has the same sub layer ID, then skip
//     //                 // todo : this cut may only work for the MC, for the data, it requires additional check
//     //                 if (outer_clu_phi_map[outer_phi_i][outer_phi_clu_i].second.layer == outer_clu_phi_map[true_scan_i][outer_scan_clu_i].second.layer) {continue;}

//     //                 // note : I expect the two cluster have to have the same z position
//     //                 // note : but in case of data, the z position of the same strip may be fluctuated a little bit 
//     //                 // todo : currently, set the Z position flutuation to be 4 mm
//     //                 if (fabs(outer_clu_phi_map[outer_phi_i][outer_phi_clu_i].second.z - outer_clu_phi_map[true_scan_i][outer_scan_clu_i].second.z) > 4 ) {continue;}

//     //                 Clus_OuterPhi_Offset_2 = (outer_clu_phi_map[true_scan_i][outer_scan_clu_i].second.y - beam_origin.second < 0) ? atan2(outer_clu_phi_map[true_scan_i][outer_scan_clu_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_scan_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(outer_clu_phi_map[true_scan_i][outer_scan_clu_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_scan_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());

//     //                 double delta_phi = get_delta_phi(Clus_OuterPhi_Offset_1, Clus_OuterPhi_Offset_2);

//     //                 // note : if the two cluster are too far away in phi angle, the skip
//     //                 // todo : the delta phi cut may need to be adjusted
//     //                 if (fabs(delta_phi) > 0.6) {continue;}

//     //                 // note : the pair that passed the cut mentioned above can be considered as a proto_mega_cluster
//     //                 Mega_outer_clu_pair_index.push_back({{outer_phi_i, outer_phi_clu_i, true_scan_i, outer_scan_clu_i}});
//     //                 Mega_outer_clu_delta_phi_abs.push_back(fabs(delta_phi));
//     //                 Mega_outer_clu_phi.push_back( get_track_phi(Clus_OuterPhi_Offset_1, delta_phi) );
//     //             }
//     //         }
//     //     }
//     // }

//     // // note : try to sort the pair of the Inner Mega cluster by the abs delta phi
//     // long long inner_mega_vec_size = Mega_inner_clu_delta_phi_abs.size();
//     // long long ind_inner_mega[Mega_inner_clu_delta_phi_abs.size()];
//     // TMath::Sort(inner_mega_vec_size, &Mega_inner_clu_delta_phi_abs[0], ind_inner_mega, false);

//     // // note : try to sort the pair of the Outer Mega cluster by the abs delta phi
//     // long long outer_mega_vec_size = Mega_outer_clu_delta_phi_abs.size();
//     // long long ind_outer_mega[Mega_outer_clu_delta_phi_abs.size()];
//     // TMath::Sort(outer_mega_vec_size, &Mega_outer_clu_delta_phi_abs[0], ind_outer_mega, false);

//     // // note : 4 cluster track, Inner mega - Outer mega
//     // for (int inner_i; inner_i < Mega_inner_clu_pair_index.size(); inner_i++)
//     // {
//     //     int id_inner[4] = {
//     //         Mega_inner_clu_pair_index[ind_inner_mega[inner_i]][0], 
//     //         Mega_inner_clu_pair_index[ind_inner_mega[inner_i]][1], // note : one inner cluster
//     //         Mega_inner_clu_pair_index[ind_inner_mega[inner_i]][2], 
//     //         Mega_inner_clu_pair_index[ind_inner_mega[inner_i]][3] // note : another inner cluster
//     //     };

//     //     if (inner_used_mega_clu[Form("%i_%i", id_inner[0], id_inner[1])] != 0) {continue;}
//     //     if (inner_used_mega_clu[Form("%i_%i", id_inner[2], id_inner[3])] != 0) {continue;}

//     //     for (int outer_i; outer_i < Mega_outer_clu_pair_index.size(); outer_i++)
//     //     {
//     //         int id_outer[4] = {
//     //             Mega_outer_clu_pair_index[ind_outer_mega[outer_i]][0], 
//     //             Mega_outer_clu_pair_index[ind_outer_mega[outer_i]][1], //  note : one outer cluster 
//     //             Mega_outer_clu_pair_index[ind_outer_mega[outer_i]][2], 
//     //             Mega_outer_clu_pair_index[ind_outer_mega[outer_i]][3] // note : abother outer cluster
//     //         };
            
//     //         if (outer_used_mega_clu[Form("%i_%i", id_outer[0], id_outer[1])] != 0) {continue;}
//     //         if (outer_used_mega_clu[Form("%i_%i", id_outer[2], id_outer[3])] != 0) {continue;}

//     //         // note : the delta phi cut
//     //         // todo : the delta phi cut value is fixed here
//     //         if (fabs(get_delta_phi(Mega_inner_clu_phi[ind_inner_mega[inner_i]], Mega_outer_clu_phi[ind_outer_mega[outer_i]])) > 0.6) {continue;}
            
//     //         int id_inner_small_r = ( get_radius(inner_clu_phi_map[id_inner[0]][id_inner[1]].second.x - beam_origin.first, inner_clu_phi_map[id_inner[0]][id_inner[1]].second.y - beam_origin.second) < get_radius(inner_clu_phi_map[id_inner[2]][id_inner[3]].second.x - beam_origin.first, inner_clu_phi_map[id_inner[2]][id_inner[3]].second.y - beam_origin.second) ) ? 0 : 2;
//     //         int id_outer_big_r   = ( get_radius(outer_clu_phi_map[id_outer[0]][id_outer[1]].second.x - beam_origin.first, outer_clu_phi_map[id_outer[0]][id_outer[1]].second.y - beam_origin.second) > get_radius(outer_clu_phi_map[id_outer[2]][id_outer[3]].second.x - beam_origin.first, outer_clu_phi_map[id_outer[2]][id_outer[3]].second.y - beam_origin.second) ) ? 0 : 2;

//     //         pair<double,double> z_range_info = Get_possible_zvtx( 
//     //             0., // get_radius(beam_origin.first,beam_origin.second), 
//     //             {get_radius(inner_clu_phi_map[id_inner[id_inner_small_r]][id_inner[id_inner_small_r+1]].second.x - beam_origin.first, inner_clu_phi_map[id_inner[id_inner_small_r]][id_inner[id_inner_small_r+1]].second.y - beam_origin.second), inner_clu_phi_map[id_inner[id_inner_small_r]][id_inner[id_inner_small_r+1]].second.z}, // note : unsign radius
//     //             {get_radius(outer_clu_phi_map[id_outer[id_outer_big_r]][id_outer[id_outer_big_r+1]].second.x - beam_origin.first, outer_clu_phi_map[id_outer[id_outer_big_r]][id_outer[id_outer_big_r+1]].second.y - beam_origin.second), outer_clu_phi_map[id_outer[id_outer_big_r]][id_outer[id_outer_big_r+1]].second.z}  // note : unsign radius
//     //         );

//     //         // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
//     //         if (z_range_info.first - z_range_info.second > evt_z.first + evt_z.second || z_range_info.first + z_range_info.second < evt_z.first - evt_z.second) {continue;}

//     //         cluster4_track_phi_1D -> Fill(get_track_phi(Mega_inner_clu_phi[ind_inner_mega[inner_i]], get_delta_phi(Mega_inner_clu_phi[ind_inner_mega[inner_i]], Mega_outer_clu_phi[ind_outer_mega[outer_i]])));

//     //         NClu4_track_count += 1;

//     //         // note : to mark the cluster as used 
//     //         inner_used_mega_clu[Form("%i_%i", id_inner[0], id_inner[1])] += 1;
//     //         inner_used_mega_clu[Form("%i_%i", id_inner[2], id_inner[3])] += 1;
//     //         outer_used_mega_clu[Form("%i_%i", id_outer[0], id_outer[1])] += 1;
//     //         outer_used_mega_clu[Form("%i_%i", id_outer[2], id_outer[3])] += 1;

//     //         inner_clu_phi_map[id_inner[0]][id_inner[1]].first = true;
//     //         inner_clu_phi_map[id_inner[2]][id_inner[3]].first = true;
//     //         outer_clu_phi_map[id_outer[0]][id_outer[1]].first = true;
//     //         outer_clu_phi_map[id_outer[2]][id_outer[3]].first = true;
//     //     }
//     // }

//     // // note : 3 cluster track, Inner mega - Outer normal
//     // for (int inner_i; inner_i < Mega_inner_clu_pair_index.size(); inner_i++) // note : the inner mega cluster loop
//     // {
//     //     int id_inner[4] = {
//     //         Mega_inner_clu_pair_index[ind_inner_mega[inner_i]][0], 
//     //         Mega_inner_clu_pair_index[ind_inner_mega[inner_i]][1], // note : one inner cluster
//     //         Mega_inner_clu_pair_index[ind_inner_mega[inner_i]][2], 
//     //         Mega_inner_clu_pair_index[ind_inner_mega[inner_i]][3] // note : another inner cluster
//     //     };

//     //     if (inner_used_mega_clu[Form("%i_%i", id_inner[0], id_inner[1])] != 0) {continue;}
//     //     if (inner_used_mega_clu[Form("%i_%i", id_inner[2], id_inner[3])] != 0) {continue;}

//     //     if (Mega_inner_clu_phi[ind_inner_mega[inner_i]] < 0 || Mega_inner_clu_phi[ind_inner_mega[inner_i]] >= 360) {cout<<"test: the mega inner cluster phi angle calculation is wrong, the value is : "<<Mega_inner_clu_phi[ind_inner_mega[inner_i]]<<endl;} // todo : this is a debug line
//     //     int mega_clu_phi_index = int(Mega_inner_clu_phi[ind_inner_mega[inner_i]]);

//     //     // note : outer cluster loop -1, 0, 1
//     //     for (int scan_i = -1; scan_i < 2; scan_i++)
//     //     {   
//     //         int true_scan_i = ((mega_clu_phi_index + scan_i) < 0) ? 360 + (mega_clu_phi_index + scan_i) : ((mega_clu_phi_index + scan_i) > 359) ? (mega_clu_phi_index + scan_i)-360 : mega_clu_phi_index + scan_i;

//     //         for (int outer_i; outer_i < outer_clu_phi_map[true_scan_i].size(); outer_i++)
//     //         {
//     //             // note : if the cluster is used, then skip
//     //             if (outer_clu_phi_map[true_scan_i][outer_i].first == true) {continue;}

//     //             // note : calculate the outer cluster phi with the consideration of the beam position
//     //             Clus_OuterPhi_Offset = (outer_clu_phi_map[true_scan_i][outer_i].second.y - beam_origin.second < 0) ? atan2(outer_clu_phi_map[true_scan_i][outer_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(outer_clu_phi_map[true_scan_i][outer_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_i].second.x - beam_origin.first) * (180./TMath::Pi());
                
//     //             // note : the delta phi cut between the inner-mega-cluster and the outer-cluster
//     //             if ( fabs(get_delta_phi(Mega_inner_clu_phi[ind_inner_mega[inner_i]], Clus_OuterPhi_Offset)) > 0.6 ) { continue; }

//     //             int id_inner_small_r = ( get_radius(inner_clu_phi_map[id_inner[0]][id_inner[1]].second.x - beam_origin.first, inner_clu_phi_map[id_inner[0]][id_inner[1]].second.y - beam_origin.second) < get_radius(inner_clu_phi_map[id_inner[2]][id_inner[3]].second.x - beam_origin.first, inner_clu_phi_map[id_inner[2]][id_inner[3]].second.y - beam_origin.second) ) ? 0 : 2;

//     //             pair<double,double> z_range_info = Get_possible_zvtx( 
//     //                 0., // get_radius(beam_origin.first,beam_origin.second), 
//     //                 {get_radius(inner_clu_phi_map[id_inner[id_inner_small_r]][id_inner[id_inner_small_r+1]].second.x - beam_origin.first, inner_clu_phi_map[id_inner[id_inner_small_r]][id_inner[id_inner_small_r+1]].second.y - beam_origin.second), inner_clu_phi_map[id_inner[id_inner_small_r]][id_inner[id_inner_small_r+1]].second.z}, // note : unsign radius
//     //                 {get_radius(outer_clu_phi_map[true_scan_i][outer_i].second.x - beam_origin.first, outer_clu_phi_map[true_scan_i][outer_i].second.y - beam_origin.second), outer_clu_phi_map[true_scan_i][outer_i].second.z}  // note : unsign radius
//     //             );

//     //             // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
//     //             if (z_range_info.first - z_range_info.second > evt_z.first + evt_z.second || z_range_info.first + z_range_info.second < evt_z.first - evt_z.second) {continue;}

//     //             cluster3_all_track_phi_1D   -> Fill(get_track_phi(Mega_inner_clu_phi[ind_inner_mega[inner_i]], get_delta_phi(Mega_inner_clu_phi[ind_inner_mega[inner_i]], Clus_OuterPhi_Offset)));
//     //             cluster3_inner_track_phi_1D -> Fill(get_track_phi(Mega_inner_clu_phi[ind_inner_mega[inner_i]], get_delta_phi(Mega_inner_clu_phi[ind_inner_mega[inner_i]], Clus_OuterPhi_Offset)));

//     //             NClu3_track_count += 1;

//     //             // note : to mark the cluster as used 
//     //             inner_used_mega_clu[Form("%i_%i", id_inner[0], id_inner[1])] += 1;
//     //             inner_used_mega_clu[Form("%i_%i", id_inner[2], id_inner[3])] += 1;

//     //             inner_clu_phi_map[id_inner[0]][id_inner[1]].first = true;
//     //             inner_clu_phi_map[id_inner[2]][id_inner[3]].first = true;
//     //             outer_clu_phi_map[true_scan_i][outer_i].first = true;

//     //         }
//     //     }
//     // }

//     // // note : 3 cluster track, Inner normal - Outer mega
//     // for (int outer_i; outer_i < Mega_outer_clu_pair_index.size(); outer_i++) // note : the outer mega cluster loop
//     // {
//     //     int id_outer[4] = {
//     //         Mega_outer_clu_pair_index[ind_outer_mega[outer_i]][0], 
//     //         Mega_outer_clu_pair_index[ind_outer_mega[outer_i]][1], // note : one outer cluster
//     //         Mega_outer_clu_pair_index[ind_outer_mega[outer_i]][2], 
//     //         Mega_outer_clu_pair_index[ind_outer_mega[outer_i]][3] // note : another outer cluster
//     //     };

//     //     if (outer_used_mega_clu[Form("%i_%i", id_outer[0], id_outer[1])] != 0) {continue;}
//     //     if (outer_used_mega_clu[Form("%i_%i", id_outer[2], id_outer[3])] != 0) {continue;}

//     //     if (Mega_outer_clu_phi[ind_outer_mega[outer_i]] < 0 || Mega_outer_clu_phi[ind_outer_mega[outer_i]] >= 360) {cout<<"test: the mega outer cluster phi angle calculation is wrong, the value is : "<<Mega_outer_clu_phi[ind_outer_mega[outer_i]]<<endl;} // todo : this is a debug line
//     //     int mega_clu_phi_index = int(Mega_outer_clu_phi[ind_outer_mega[outer_i]]);

//     //     for (int scan_i = -1; scan_i < 2; scan_i++)
//     //     {
//     //         int true_scan_i = ((mega_clu_phi_index + scan_i) < 0) ? 360 + (mega_clu_phi_index + scan_i) : ((mega_clu_phi_index + scan_i) > 359) ? (mega_clu_phi_index + scan_i)-360 : mega_clu_phi_index + scan_i;

//     //         for (int inner_i; inner_i < inner_clu_phi_map[true_scan_i].size(); inner_i++)
//     //         {
//     //             // note : if the cluster is used, then skip
//     //             if (inner_clu_phi_map[true_scan_i][inner_i].first == true) {continue;}    

//     //             // note : calculate the inner cluster phi with the consideration of the beam position
//     //             Clus_InnerPhi_Offset = (inner_clu_phi_map[true_scan_i][inner_i].second.y - beam_origin.second < 0) ? atan2(inner_clu_phi_map[true_scan_i][inner_i].second.y - beam_origin.second, inner_clu_phi_map[true_scan_i][inner_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(inner_clu_phi_map[true_scan_i][inner_i].second.y - beam_origin.second, inner_clu_phi_map[true_scan_i][inner_i].second.x - beam_origin.first) * (180./TMath::Pi());

//     //             // note : the delta phi cut between the outer-mega-cluster and the inner-cluster
//     //             if ( fabs(get_delta_phi(Mega_outer_clu_phi[ind_outer_mega[outer_i]], Clus_InnerPhi_Offset)) > 0.6 ) { continue; }

//     //             int id_outer_big_r   = ( get_radius(outer_clu_phi_map[id_outer[0]][id_outer[1]].second.x - beam_origin.first, outer_clu_phi_map[id_outer[0]][id_outer[1]].second.y - beam_origin.second) > get_radius(outer_clu_phi_map[id_outer[2]][id_outer[3]].second.x - beam_origin.first, outer_clu_phi_map[id_outer[2]][id_outer[3]].second.y - beam_origin.second) ) ? 0 : 2;

//     //             pair<double,double> z_range_info = Get_possible_zvtx( 
//     //                 0., // get_radius(beam_origin.first,beam_origin.second), 
//     //                 {get_radius(inner_clu_phi_map[true_scan_i][inner_i].second.x - beam_origin.first, inner_clu_phi_map[true_scan_i][inner_i].second.y - beam_origin.second), inner_clu_phi_map[true_scan_i][inner_i].second.z}, // note : unsign radius
//     //                 {get_radius(outer_clu_phi_map[id_outer[id_outer_big_r]][id_outer[id_outer_big_r+1]].second.x - beam_origin.first, outer_clu_phi_map[id_outer[id_outer_big_r]][id_outer[id_outer_big_r+1]].second.y - beam_origin.second), outer_clu_phi_map[id_outer[id_outer_big_r]][id_outer[id_outer_big_r+1]].second.z}  // note : unsign radius
//     //             );

//     //             // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
//     //             if (z_range_info.first - z_range_info.second > evt_z.first + evt_z.second || z_range_info.first + z_range_info.second < evt_z.first - evt_z.second) {continue;}

//     //             cluster3_all_track_phi_1D   -> Fill(get_track_phi(Clus_InnerPhi_Offset, get_delta_phi(Clus_InnerPhi_Offset, Mega_outer_clu_phi[ind_outer_mega[outer_i]])));
//     //             cluster3_outer_track_phi_1D -> Fill(get_track_phi(Clus_InnerPhi_Offset, get_delta_phi(Clus_InnerPhi_Offset, Mega_outer_clu_phi[ind_outer_mega[outer_i]])));

//     //             NClu3_track_count += 1;

//     //             // note : to mark the cluster as used 
//     //             outer_used_mega_clu[Form("%i_%i", id_outer[0], id_outer[1])] += 1;
//     //             outer_used_mega_clu[Form("%i_%i", id_outer[2], id_outer[3])] += 1;

//     //             outer_clu_phi_map[id_outer[0]][id_outer[1]].first = true;
//     //             outer_clu_phi_map[id_outer[2]][id_outer[3]].first = true;
//     //             inner_clu_phi_map[true_scan_i][inner_i].first = true;
//     //         }
//     //     }
//     // }

//     // NClu4_track_centrality_2D -> Fill(centrality_map[centrality_bin], NClu4_track_count);
//     // NClu3_track_centrality_2D -> Fill(centrality_map[centrality_bin], NClu3_track_count);

//     // // note : for the mega cluster test only
//     // return;

//     // note : for two-cluster tracklets only
//     for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++) // note : each phi cell (1 degree)
//     {
//         // note : N cluster in this phi cell
//         for (int inner_phi_clu_i = 0; inner_phi_clu_i < inner_clu_phi_map[inner_phi_i].size(); inner_phi_clu_i++)
//         {
//             clu_multi_used_tight = 0;
//             clu_multi_used_loose = 0;
//             if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].first == true) {continue;}

//             Clus_InnerPhi_Offset = (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second < 0) ? atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());

//             // todo: change the outer phi scan range
//             // note : the outer phi index, -4, -3, -2, -1, 0, 1, 2, 3, 4
//             for (int scan_i = -4; scan_i < 5; scan_i++)
//             {
//                 int true_scan_i = ((inner_phi_i + scan_i) < 0) ? 360 + (inner_phi_i + scan_i) : ((inner_phi_i + scan_i) > 359) ? (inner_phi_i + scan_i)-360 : inner_phi_i + scan_i;

//                 // note : N clusters in that outer phi cell
//                 for (int outer_phi_clu_i = 0; outer_phi_clu_i < outer_clu_phi_map[true_scan_i].size(); outer_phi_clu_i++)
//                 {
//                     if (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].first == true) {continue;}

//                     Clus_OuterPhi_Offset = (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second < 0) ? atan2(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());
//                     double delta_phi = get_delta_phi(Clus_InnerPhi_Offset, Clus_OuterPhi_Offset);
                    
//                     // if (fabs(delta_phi) > 5.72) {continue;}
//                     if (fabs(delta_phi) > 3.5) {continue;}

//                     double inner_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z});
//                     double outer_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z});
//                     double delta_eta = inner_clu_eta - outer_clu_eta;

//                     track_delta_eta_1D[centrality_map[centrality_bin]] -> Fill( delta_eta );
//                     track_delta_eta_1D[track_delta_eta_1D.size() - 1]  -> Fill( delta_eta );

//                     track_DeltaPhi_DeltaEta_2D[centrality_map[centrality_bin]]        -> Fill(delta_phi, delta_eta);
//                     track_DeltaPhi_DeltaEta_2D[track_DeltaPhi_DeltaEta_2D.size() - 1] -> Fill(delta_phi, delta_eta);

//                     pair<double,double> z_range_info = Get_possible_zvtx( 
//                         0., // get_radius(beam_origin.first,beam_origin.second), 
//                         {get_radius(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second), inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z}, // note : unsign radius
//                         {get_radius(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second), outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z}  // note : unsign radius
//                     );

//                     // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
//                     if (z_range_info.first - z_range_info.second > evt_z.first + evt_z.second || z_range_info.first + z_range_info.second < evt_z.first - evt_z.second) {continue;}
//                     if (fabs(delta_phi) < 0.6) {clu_multi_used_tight += 1;}
//                     track_delta_eta_1D_post[centrality_map[centrality_bin]]     -> Fill( delta_eta );
//                     track_delta_eta_1D_post[track_delta_eta_1D_post.size() - 1] -> Fill( delta_eta );

//                     double DCA_sign = calculateAngleBetweenVectors(
//                         outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y,
//                         inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y,
//                         beam_origin.first, beam_origin.second
//                     );
                    
//                     track_DCA_distance[centrality_map[centrality_bin]] -> Fill( DCA_sign );
//                     track_DCA_distance[track_DCA_distance.size() - 1]  -> Fill( DCA_sign ); 

//                     track_phi_DCA_2D[centrality_map[centrality_bin]] -> Fill( delta_phi, DCA_sign );
//                     track_phi_DCA_2D[track_phi_DCA_2D.size() - 1]    -> Fill( delta_phi, DCA_sign );
                    
//                     track_delta_phi_1D[centrality_map[centrality_bin]] -> Fill( delta_phi );
//                     track_delta_phi_1D[track_delta_phi_1D.size() - 1]  -> Fill( delta_phi );

//                     proto_pair_index.push_back({inner_phi_i, inner_phi_clu_i, true_scan_i, outer_phi_clu_i});
//                     proto_pair_delta_phi_abs.push_back(fabs(delta_phi));
//                     proto_pair_delta_phi.push_back({Clus_InnerPhi_Offset, Clus_OuterPhi_Offset, delta_phi});


//                     // note : do the fill here (find the best match outer cluster with the inner cluster )
//                     Get_eta_pair = Get_eta(
//                         {0., evt_z.first,evt_z.second},
//                         {get_radius(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second), inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z},
//                         {get_radius(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second), outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z}
//                     );
                    
//                     if  (Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2. > 0.3)
//                     {
//                         cout<<" "<<endl;
//                         cout<<"inner clu eta : "<<inner_clu_eta<<" outer clu eta : "<<outer_clu_eta<<"avg eta : "<< (inner_clu_eta + outer_clu_eta)/2. <<" reco eta : "<<Get_eta_pair.second<<" diff: "<<Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.<<endl;
//                         cout<<"inner clu pos : "<<inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x<<" "<<inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y<<" "<<inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z<<endl;
//                         cout<<"outer clu pos : "<<outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x<<" "<<outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y<<" "<<outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z<<endl;
//                     }  


//                     double track_phi = get_track_phi(Clus_InnerPhi_Offset, delta_phi);

//                     reco_eta_correlation_2D -> Fill(Get_eta_pair.second, (inner_clu_eta + outer_clu_eta)/2.);
//                     reco_eta_diff_reco3P_2D -> Fill(Get_eta_pair.second, Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);
//                     reco_eta_diff_1D        -> Fill(Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);

//                     // track_delta_phi_1D[centrality_map[centrality_bin]] -> Fill( delta_phi );
//                     // track_delta_phi_1D[track_delta_phi_1D.size() - 1]  -> Fill( delta_phi );

//                     track_DeltaPhi_eta_2D[centrality_map[centrality_bin]]   -> Fill(delta_phi, Get_eta_pair.second);
//                     track_DeltaPhi_eta_2D[track_DeltaPhi_eta_2D.size() - 1] -> Fill(delta_phi, Get_eta_pair.second);

//                     if (fabs(delta_phi) <= signal_region)
//                     {
//                         dNdeta_1D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second);
//                         dNdeta_1D[dNdeta_1D.size() - 1]           -> Fill(Get_eta_pair.second);

//                         track_eta_z_2D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second, evt_z.first);
//                         track_eta_z_2D[track_eta_z_2D.size() - 1]      -> Fill(Get_eta_pair.second, evt_z.first);

//                         evt_NTrack += 1;
//                     }
                    
//                     track_eta_phi_2D[centrality_map[centrality_bin]] -> Fill(track_phi, Get_eta_pair.second);
//                     track_eta_phi_2D[track_eta_phi_2D.size() - 1]    -> Fill(track_phi, Get_eta_pair.second);
                    
//                     // note : find the bin of the eta
//                     double eta_bin = eta_region_hist -> Fill(Get_eta_pair.second);
//                     // cout<<"test1 "<<eta_bin<<endl;
//                     if (eta_bin != -1)
//                     {
//                         final_track_delta_phi_1D[centrality_map[centrality_bin]][eta_bin - 1]      -> Fill(delta_phi);
//                         final_track_delta_phi_1D[final_track_delta_phi_1D.size() - 1][eta_bin - 1] -> Fill(delta_phi);

//                         // todo : the signal region selection
//                         if (fabs(delta_phi) <= signal_region) { 
//                             good_tracklet_counting[centrality_map[centrality_bin]][eta_bin - 1] += 1;
//                             good_tracklet_counting[final_track_delta_phi_1D.size() - 1][eta_bin - 1] += 1;
//                         }

//                         out_track_delta_phi_d.push_back(delta_phi);
//                         out_recotrack_eta_d.push_back(Get_eta_pair.second);
//                         out_track_eta_i.push_back(eta_bin);
//                     }
//                 }
//             } // note : end outer loop

//             clu_used_centrality_2D -> Fill(total_NClus, clu_multi_used_tight);

//             // // note : if there is no good match with inner and outer clusters, continue
//             // if (clu_multi_used_loose == 0) {continue;}

//             // // note : do the fill here (find the best match outer cluster with the inner cluster )
//             // Get_eta_pair = Get_eta(
//             //     {0., evt_z.first,evt_z.second},
//             //     {get_radius(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second), inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z},
//             //     {get_radius(outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.x - beam_origin.first, outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.y - beam_origin.second), outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.z}
//             // );

//             // double inner_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z});
//             // double outer_clu_eta = get_clu_eta({beam_origin.first, beam_origin.second, evt_z.first},{outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.x, outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.y, outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.z});
//             // double track_phi = Clus_InnerPhi_Offset - (pair_delta_phi/2.);

//             // if  (Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2. > 0.3)
//             // {
//             //     cout<<" "<<endl;
//             //     cout<<"inner clu eta : "<<inner_clu_eta<<" outer clu eta : "<<outer_clu_eta<<"avg eta : "<< (inner_clu_eta + outer_clu_eta)/2. <<" reco eta : "<<Get_eta_pair.second<<" diff: "<<Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.<<endl;
//             //     cout<<"inner clu pos : "<<inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x<<" "<<inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y<<" "<<inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z<<endl;
//             //     cout<<"outer clu pos : "<<outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.x<<" "<<outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.y<<" "<<outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].second.z<<endl;
//             // }

//             // reco_eta_correlation_2D -> Fill(Get_eta_pair.second, (inner_clu_eta + outer_clu_eta)/2.);
//             // reco_eta_diff_reco3P_2D -> Fill(Get_eta_pair.second, Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);
//             // reco_eta_diff_1D        -> Fill(Get_eta_pair.second - (inner_clu_eta + outer_clu_eta)/2.);

//             // // track_delta_phi_1D[centrality_map[centrality_bin]] -> Fill( pair_delta_phi );
//             // // track_delta_phi_1D[track_delta_phi_1D.size() - 1]  -> Fill( pair_delta_phi );

//             // track_DeltaPhi_eta_2D[centrality_map[centrality_bin]]   -> Fill(pair_delta_phi, Get_eta_pair.second);
//             // track_DeltaPhi_eta_2D[track_DeltaPhi_eta_2D.size() - 1] -> Fill(pair_delta_phi, Get_eta_pair.second);

//             // // cout<<"test_5"<<endl;
//             // dNdeta_1D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second);
//             // dNdeta_1D[dNdeta_1D.size() - 1]           -> Fill(Get_eta_pair.second);

//             // track_eta_phi_2D[centrality_map[centrality_bin]] -> Fill(track_phi, Get_eta_pair.second);
//             // track_eta_phi_2D[track_eta_phi_2D.size() - 1]    -> Fill(track_phi, Get_eta_pair.second);
            
//             // track_eta_z_2D[centrality_map[centrality_bin]] -> Fill(Get_eta_pair.second, evt_z.first);
//             // track_eta_z_2D[track_eta_z_2D.size() - 1]      -> Fill(Get_eta_pair.second, evt_z.first);        

//             // // note : find the bin of the eta
//             // double eta_bin = eta_region_hist -> Fill(Get_eta_pair.second);
//             // // cout<<"test1 "<<eta_bin<<endl;
//             // if (eta_bin != -1)
//             // {
//             //     final_track_delta_phi_1D[centrality_map[centrality_bin]][eta_bin - 1]      -> Fill(pair_delta_phi);
//             //     final_track_delta_phi_1D[final_track_delta_phi_1D.size() - 1][eta_bin - 1] -> Fill(pair_delta_phi);

//             //     out_track_delta_phi_d.push_back(pair_delta_phi);
//             //     out_recotrack_eta_d.push_back(Get_eta_pair.second);
//             //     out_track_eta_i.push_back(eta_bin);
//             // }

//             // evt_NTrack += 1;
//             // // note : this outer cluster is used 
//             // outer_clu_phi_map[pair_outer_index.first][pair_outer_index.second].first = true;
//         }
//     } // note : end inner loop

//     out_eID = event_i;
//     out_evt_centrality_bin = centrality_bin;
//     out_evt_zvtx = evt_z.first;
//     tree_out -> Fill();

//     // cout<<" "<<endl;
//     // cout<<" "<<endl;
//     // cout<<"test_8"<<endl;
//     if (run_type == "MC")
//     {
//         track_correlation_2D -> Fill(evt_NTrack_MC, evt_NTrack);
//         track_ratio_1D[centrality_map[centrality_bin]] -> Fill( double(evt_NTrack) / double(evt_NTrack_MC) );
//         track_ratio_1D[track_ratio_1D.size() - 1]      -> Fill( double(evt_NTrack) / double(evt_NTrack_MC) );
//     }
        
//     track_cluster_ratio_multiplicity_2D -> Fill( effective_total_NClus, double(effective_total_NClus) / double(evt_NTrack) );
//     track_cluster_ratio_1D[centrality_map[centrality_bin]]    -> Fill( double(effective_total_NClus) / double(evt_NTrack) );
//     track_cluster_ratio_1D[track_cluster_ratio_1D.size() - 1] -> Fill( double(effective_total_NClus) / double(evt_NTrack) );

//     // if (run_type == "MC" && evt_NTrack_MC < draw_evt_cut && event_i % print_plot_ratio == 0){print_evt_plot(event_i, evt_NTrack_MC, inner_NClu, outer_NClu);}
//     return_tag = 1;
// }

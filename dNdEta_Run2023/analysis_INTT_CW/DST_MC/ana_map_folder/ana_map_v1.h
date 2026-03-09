#ifndef ANA_MAP_V1_H
#define ANA_MAP_V1_H

#include <iostream>
using namespace std;

#include <TH1F.h>

// note : this map is for handling the file with the float centrality numbers
// note : we have to convert them here, hopefully
namespace ANA_MAP_V3
{
    double cm = 10.;
    double mm = 1.;

    // note : ------------------------------------ special for MC ------------------------------------
    // todo : the space in between the text is important, we count on this
    string               MC_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test";
    string               MC_file_name = "ntuple_merged";
    string               MC_tree_name = "EventTree";
    string               MC_tracklet_merged_folder = "merged_file_folder";
    pair<double, double> MC_beam_origin = {-0.04029 * cm, 0.239851 * cm}; // note : unit : mm 
    int                  MC_data_type = 0; // note : MC

    // note : ------------------------------------ special for data ----------------------------------- 
    // todo : the space in between the text is important, we count on this 
    string               data_input_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test";
    string               data_file_name = "Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey";
    string               data_tree_name = "EventTree";
    string               data_tracklet_merged_folder = "merged_file_folder";
    pair<double, double> data_beam_origin = {-0.0206744 * cm, 0.279965 * cm};
    int                  data_data_type = 7; // note : data_F4A

    // note : -------------------------------- common for topic, avg_vtxXY --------------------------------
    int avg_vtxXY_N_clu_cut = 350;
    int avg_vtxXY_N_clu_cutl = 20;
    int avg_vtxXY_quadrant_window_size = 4; // note : mm
    int avg_vtxXY_quadrant_Niteration = 8;

    int avg_vtxXY_Nevt_per_core = 15000;

    int final_angle_diff_inner_phi_radian_coarseX_XNbins = 50;
    double final_angle_diff_inner_phi_radian_coarseX_Xmin = -M_PI;
    double final_angle_diff_inner_phi_radian_coarseX_Xmax = M_PI;
    int final_angle_diff_inner_phi_radian_coarseX_YNbins = 120;
    double final_angle_diff_inner_phi_radian_coarseX_Ymin = -0.03;
    double final_angle_diff_inner_phi_radian_coarseX_Ymax = 0.03;
    
    // note : -------------------------------- common for topic, evt_vtxZ --------------------------------
    pair<double, double> evt_vtxZ_DCA_cut ={-1., 1.}; // note : for run20869
    pair<double, double> evt_vtxZ_zvtx_QA_width = {40, 70}; 
    double evt_vtxZ_zvtx_QA_ratio = 0.00006;
    double evt_vtxZ_phi_diff_cut = 0.5; // note : for run20869
    int    evt_vtxZ_N_clu_cut = 10000;
    int    evt_vtxZ_N_clu_cutl = 20;
    int    evt_vtxZ_zvtx_cal_require = 15;
    // note : for the merged analysis
    double evt_vtxZ_zvtx_range_l = -500;
    double evt_vtxZ_zvtx_range_r = 100;
    double evt_vtxZ_zvtx_range_zoomin_l = -300;
    double evt_vtxZ_zvtx_range_zoomin_r = -100;
    double evt_vtxZ_data_required_zvtx_diff = 10.; // note : mm
    int    evt_vtxZ_data_zvtx_dist_NClus_cut = 1000;

    int    evt_vtxZ_data_Nevt_per_core = 500; // note : only for data, as it has different way to run the evt_vtxZ

    // note : -------------------------------- common for topic, evt_vtxXY --------------------------------
    int evt_vtxXY_N_clu_cut  = 10000;
    int evt_vtxXY_N_clu_cutl = 3000;

    int evt_vtxXY_Nevt_per_core = 3000;

    // note : -------------------------------- common for topic, evt_tracklet --------------------------------
    int evt_tracklet_N_clu_cut  = 10000;
    int evt_tracklet_N_clu_cutl = 20;

    int evt_tracklet_data_Nevt_per_core = 10000; // note : only for data, as it has different way to run the evt_vtxZ

    vector<int> evt_tracklet_fulleta_MC_included_z_bin = {
        4, 5, 6, 7, 8, 9
    };

    vector<pair<int,vector<int>>> evt_tracklet_included_eta_z_map = {
        {9,                   {9}}, // note : eta bin 11: -1.1 to -0.9 
        {10,                {8,9}}, // note : eta bin 12: -0.9 to -0.7
        {11,                {8,9}}, // note : eta bin 11: -0.7 to -0.5
        {12,            {7, 8, 9}}, // note : eta bin 12: -0.5 to -0.3
        {13,            {7, 8, 9}}, // note : eta bin 13: -0.3 to -0.1
        {14,         {6, 7, 8, 9}}, // note : eta bin 14: -0.1 to  0.1
        {15,         {6, 7, 8, 9}}, // note : eta bin 15:  0.1 to  0.3
        {16,         {6, 7, 8, 9}}, // note : eta bin 16:  0.3 to  0.5
        {17,      {5, 6, 7, 8, 9}}, // note : eta bin 17:  0.5 to  0.7
        {18,      {5, 6, 7, 8, 9}}, // note : eta bin 18:  0.7 to  0.9
        {19,      {5, 6, 7, 8, 9}}, // note : eta bin 19:  0.9 to  1.1
        {20,   {4, 5, 6, 7, 8, 9}}, // note : eta bin 20:  1.1 to  1.3
        {21,   {4, 5, 6, 7, 8, 9}}, // note : eta bin 21:  1.3 to  1.5
        {22,   {4, 5, 6, 7, 8, 9}}, // note : eta bin 22:  1.5 to  1.7
        {23,   {4, 5, 6, 7}},       // note : eta bin 23:  1.7 to  1.9
        {24,   {4, 5}}              // note : eta bin 24:  1.9 to  2.1
    };

    // note : -------------------------------- common for all --------------------------------
    int clu_sum_adc_cut = 35;
    int clu_size_cut = 5;

    double signal_region = 1.;

    double MBD_assy_ratio_cut = 0.75;
    double INTT_zvtx_recohist_gaus_fit_width_cut_l =  20; // note : for the trapezoidal method special_tag  //30;
    double INTT_zvtx_recohist_gaus_fit_width_cut_r =  55; // note : for the trapezoidal method special_tag //80;
    double INTT_zvtx_recohist_cutgroup_width_cut_l =  20; // note : for the trapezoidal method special_tag //40;
    double INTT_zvtx_recohist_cutgroup_width_cut_r = 80; // note : for the trapezoidal method special_tag //110;
    double INTTz_MBDz_diff_cut_l = -50;
    double INTTz_MBDz_diff_cut_r = 30;
    double INTTz_MBDz_diff_cut_MC_l = -40;
    double INTTz_MBDz_diff_cut_MC_r = 40;
    int    inclusive_Mbin_cut = 8; // note : for 0 - 70% centrality

    // note : the `centrality_edges`, `centrality_map`, and `centrality_region` have to correspond to each other
    // note : no inclusive
    vector<double> centrality_edges = {0, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    TH1F * centrality_hist = new TH1F("","",11,&centrality_edges[0]);

    int convert_centrality_bin (double Mbin_in) { 
        int found_bin = centrality_hist -> FindBin(Mbin_in);
        if (found_bin == 0){ // note : at the left edge
            cout<<" centrality value outside the given range, input Mbin value : "<<Mbin_in<<endl;
            exit(1);
        }
        else if (found_bin == centrality_hist -> GetNbinsX() + 1) { // note : at right edge
            cout<<" centrality value outside the given range, input Mbin value : "<<Mbin_in<<endl;
            exit(1);
        }
        else { return found_bin - 1; }
    }

    // note : no inclusive included
    map<int,int> centrality_map = {
        {0, 0},
        {1, 1},
        {2, 2},
        {3, 3},
        {4, 4},
        {5, 5},
        {6, 6},
        {7, 7},
        {8, 8},
        {9, 9},
        {10, 10}
    };
    
    // note : include the inclusive
    vector<string> centrality_region = {
        "0-5%",
        "5-10%",
        "10-20%",
        "20-30%",
        "30-40%",
        "40-50%",
        "50-60%",
        "60-70%",
        "70-80%",
        "80-90%",
        "90-100%",
        "0-70%" //todo: changed to 0-70%!!!
    };

    // note : the following two vectors tell the region of the eta and z, including the edges in both sides.
    // note : so when talking about the number of bins, we have to do size() - 1
    // vector<double> eta_region = { // todo: if the eta region is changed, the following vector should be updated
    //     -3.0,
    //     -2.8,
    //     -2.6,
    //     -2.4,
    //     -2.2,
    //     -2,
    //     -1.8,
    //     -1.6,
    //     -1.4,
    //     -1.2,
    //     -1,
    //     -0.8,
    //     -0.6,
    //     -0.4,
    //     -0.2,
    //     0,
    //     0.2,
    //     0.4,
    //     0.6,
    //     0.8,
    //     1,
    //     1.2,
    //     1.4,
    //     1.6,
    //     1.8,
    //     2,
    //     2.2,
    //     2.4,
    //     2.6,
    //     2.8,
    //     3
    // };

    vector<double> eta_region = { // todo: if the eta region is changed, the following vector should be updated
        -2.7,
        -2.5,
        -2.3,
        -2.1,
        -1.9,
        -1.7,
        -1.5,
        -1.3,
        -1.1,
        -0.9,
        -0.7,
        -0.5,
        -0.3,
        -0.1,
        0.1,
        0.3,
        0.5,
        0.7,
        0.9,
        1.1,
        1.3,
        1.5,
        1.7,
        1.9,
        2.1,
        2.3,
        2.5,
        2.7
    };

    vector<double> z_region = { // todo: if the z region is changed, the following vector should be updated
        -420, // note unit : mm
        -380,
        -340,
        -300,
        -260,
        -220, // note : this part is the peak region
        -180, // note : this part is the peak region
        -140,
        -100,
        -60,
        -20,
        20
    };
    pair<double, double> selected_z_region_id = {3,9};

    // note : original
    // note : from /sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_vtxZ/complete_file
    // note : from /sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test/evt_vtxZ/complete_file
    // file_i : 1 bin : 1 -40 20.0436
    // file_i : 1 bin : 2 -36 6.97846
    // file_i : 1 bin : 3 -32 2.5875
    // file_i : 1 bin : 4 -28 1.38437
    // file_i : 1 bin : 5 -24 1.00747
    // file_i : 1 bin : 6 -20 0.80477
    // file_i : 1 bin : 7 -16 0.805168
    // file_i : 1 bin : 8 -12 0.979011
    // file_i : 1 bin : 9 -8 1.43326
    // file_i : 1 bin : 10 -4 2.56772
    // file_i : 1 bin : 11 0 4.85905

    map<int, double> zvtx_reweight_factor = {
        {0, 1.},
        {1, 20.0436},
        {2, 6.97846},
        {3, 2.5875},
        {4, 1.38437},
        {5, 1.00747},
        {6, 0.80477},
        {7, 0.805168},
        {8, 0.979011},
        {9, 1.43326},
        {10, 2.56772},
        {11, 4.85905}
    };

    // note : the INTTz shift 1.056 cm
    // file_i : 1 bin : 1 -40 14.7109
    // file_i : 1 bin : 2 -36 4.96769
    // file_i : 1 bin : 3 -32 1.96646
    // file_i : 1 bin : 4 -28 1.13199
    // file_i : 1 bin : 5 -24 0.887917
    // file_i : 1 bin : 6 -20 0.839093
    // file_i : 1 bin : 7 -16 0.868618
    // file_i : 1 bin : 8 -12 1.21689
    // file_i : 1 bin : 9 -8 1.88843
    // file_i : 1 bin : 10 -4 3.93604
    // file_i : 1 bin : 11 0 7.79008

    
    // map<int, double> zvtx_reweight_factor = {
    //     {0, 1.},
    //     {1, 14.7109},
    //     {2, 4.96769},
    //     {3, 1.96646},
    //     {4, 1.13199},
    //     {5, 0.887917},
    //     {6, 0.839093},
    //     {7, 0.868618},
    //     {8, 1.21689},
    //     {9, 1.88843},
    //     {10, 3.93604},
    //     {11, 7.79008}
    // };

    TH1F * zvtx_hist = new TH1F("","",z_region.size() - 1,&z_region[0]);
    int get_zvtx_BinID (double zvtx_in) { 
        int found_bin = zvtx_hist -> FindBin(zvtx_in);
        if (found_bin == 0){ // note : at the left edge
            cout<<" zvtx value outside the given range, input zvtx value : "<<zvtx_in<<endl;
            return 0;
        }
        else if (found_bin == zvtx_hist -> GetNbinsX() + 1) { // note : at right edge
            cout<<" zvtx value outside the given range, input zvtx value : "<<zvtx_in<<endl;
            return 0;
        }
        else { return found_bin - 1; }
    }

    // todo: zvtx resolution from the file shown above 
    // todo: from /sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/SemiFinal_EvtZ_MC_ZF_zvtx/INTT_zvtx.root
    // map<int, double> reco_Z_resolution = {
    //     {0, 1.78117},
    //     {1, 1.85937},
    //     {2, 2.05982},
    //     {3, 2.33111},
    //     {4, 2.80505},
    //     {5, 3.70811},
    //     {6, 4.65284},
    //     {7, 5.37133},
    //     {8, 5.94759},
    //     {9, 6.56091},
    //     {10, 2.2836} // note : the inclusive centrality, 0% - 95%
    // };

    // todo : the weird centrality behavior 
    // todo : from /sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR/evt_vtxZ/complete_file/merged_file.root
    // 0 fit: 0 cm, num: 0 cm, avg: 0 cm
    // 1 fit: 0.184351 cm, num: 0.192904 cm, avg: 0.188627 cm
    // 2 fit: 0.190324 cm, num: 0.193508 cm, avg: 0.191916 cm
    // 3 fit: 0.191219 cm, num: 0.191739 cm, avg: 0.191479 cm
    // 4 fit: 0.202834 cm, num: 0.211722 cm, avg: 0.207278 cm
    // 5 fit: 0.242848 cm, num: 0.259601 cm, avg: 0.251224 cm
    // 6 fit: 0.300977 cm, num: 0.338136 cm, avg: 0.319557 cm
    // 7 fit: 0.416109 cm, num: 0.477896 cm, avg: 0.447002 cm
    // 8 fit: 0.571877 cm, num: 0.590128 cm, avg: 0.581003 cm
    // 9 fit: 0.494087 cm, num: 0.640948 cm, avg: 0.567518 cm
    // 10 fit: 0 cm, num: 0 cm, avg: 0 cm
    // 11 fit: 0.223783 cm, num: 0.302418 cm, avg: 0.2631 cm

    map<int, double> reco_Z_resolution = {
        {0, 1.78117},
        {1, 1.88627},
        {2, 1.91916},
        {3, 1.91479},
        {4, 2.07278},
        {5, 2.51224},
        {6, 3.19557},
        {7, 4.47002},
        {8, 5.81003},
        {9, 5.67518},
        {10,6.56091} // note : NOT the width of the inclusive bin
    }; 
    // todo: the width numbers are for sure incorrect, due to the incorrect centrality calibration in both data and MC
    // todo : in addition, the inclusive deltaZ width is expected not used.
}

// note : this is for the selfgen MC, which already defines the centrality by the bin ID, no longer need to conver it 
namespace ANA_MAP_V2
{
    // note : no inclusive included
    map<int,int> centrality_map = {
        {0, 0},
        {1, 1},
        {2, 2},
        {3, 3},
        {4, 4},
        {5, 5},
        {6, 6},
        {7, 7},
        {8, 8},
        {9, 9},
        {10,10}
    };
    
    // note : include the inclusive
    vector<string> centrality_region = {
        "0-5%",
        "5-15%",
        "15-25%",
        "25-35%",
        "35-45%",
        "45-55%",
        "55-65%",
        "65-75%",
        "75-85%",
        "85-95%",
        "95-100%",
        "0-100%"
    };

    vector<double> z_region = { // todo: if the z region is changed, the following vector should be updated
        -420, // note unit : mm
        -380,
        -340,
        -300,
        -260,
        -220, // note : this part is the peak region
        -180, // note : this part is the peak region
        -140,
        -100,
        -60,
        -20,
        20
    };

    vector<double> eta_region = { // todo: if the eta region is changed, the following vector should be updated
        -3.0,
        -2.8,
        -2.6,
        -2.4,
        -2.2,
        -2,
        -1.8,
        -1.6,
        -1.4,
        -1.2,
        -1,
        -0.8,
        -0.6,
        -0.4,
        -0.2,
        0,
        0.2,
        0.4,
        0.6,
        0.8,
        1,
        1.2,
        1.4,
        1.6,
        1.8,
        2,
        2.2,
        2.4,
        2.6,
        2.8,
        3
    };

    // todo: zvtx resolution from the file shown above 
    // todo: from /sphenix/user/ChengWei/sPH_dNdeta/self_gen_singleMu_v2/complete_file/evt_vtxZ/complete_file/merged_file.root
    map<int, double> reco_Z_resolution = {
        // {0, 1.78117},
        // {1, 1.85937},
        // {2, 2.05982},
        // {3, 2.33111},
        // {4, 2.80505},
        // {5, 3.70811},
        // {6, 4.65284},
        // {7, 5.37133},
        // {8, 5.94759},
        // {9, 6.56091},
        // {10, 2.2836} // note : the inclusive centrality, 0% - 95%

        {0, 1.084},
        {1, 1.22489},
        {2, 1.30696},
        {3, 1.36897},
        {4, 1.40817},
        {5, 1.46613},
        {6, 1.55441},
        {7, 1.69994},
        {8, 1.99916},
        {9, 2.70681},
        {10, 4.61837},
        {11, 1.68828} // note : inclusive centrality 0% ~ 100%
    };
}


namespace ANA_MAP_V1
{
    // note : no inclusive included
    map<int,int> centrality_map = {
        {5, 0},
        {15, 1},
        {25, 2},
        {35, 3},
        {45, 4},
        {55, 5},
        {65, 6},
        {75, 7},
        {85, 8},
        {95, 9}
    };
    
    // note : include the inclusive
    vector<string> centrality_region = {
        "0-5%",
        "5-15%",
        "15-25%",
        "25-35%",
        "35-45%",
        "45-55%",
        "55-65%",
        "65-75%",
        "75-85%",
        "85-95%",
        "0-95%"
    };

    // note : the following two vectors tell the region of the eta and z, including the edges in both sides.
    // note : so when talking about the number of bins, we have to do size() - 1
    vector<double> eta_region = { // todo: if the eta region is changed, the following vector should be updated
        -3.0,
        -2.8,
        -2.6,
        -2.4,
        -2.2,
        -2,
        -1.8,
        -1.6,
        -1.4,
        -1.2,
        -1,
        -0.8,
        -0.6,
        -0.4,
        -0.2,
        0,
        0.2,
        0.4,
        0.6,
        0.8,
        1,
        1.2,
        1.4,
        1.6,
        1.8,
        2,
        2.2,
        2.4,
        2.6,
        2.8,
        3
    };

    vector<double> z_region = { // todo: if the z region is changed, the following vector should be updated
        -420, // note unit : mm
        -380,
        -340,
        -300,
        -260,
        -220, // note : this part is the peak region
        -180, // note : this part is the peak region
        -140,
        -100,
        -60,
        -20,
        20
    };

    // todo: zvtx resolution from the file shown above 
    // todo: from /sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/SemiFinal_EvtZ_MC_ZF_zvtx/INTT_zvtx.root
    map<int, double> reco_Z_resolution = {
        {0, 1.78117},
        {1, 1.85937},
        {2, 2.05982},
        {3, 2.33111},
        {4, 2.80505},
        {5, 3.70811},
        {6, 4.65284},
        {7, 5.37133},
        {8, 5.94759},
        {9, 6.56091},
        {10, 2.2836} // note : the inclusive centrality, 0% - 95%
    };
}


#endif
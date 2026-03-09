#ifndef PLOT_EVT_ZVTX_H
#define PLOT_EVT_ZVTX_H

#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <cmath>
using namespace std;

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TF1.h>
#include <TMath.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>

#include "sPhenixStyle.C"
#include "../ana_map_folder/ana_map_v1.h"

#include "fit_func.cpp"

namespace ana_map_version = ANA_MAP_V3;

class plot_evt_zvtx
{
    public:
        plot_evt_zvtx(
            string input_directory_in,
            string input_file_name_in,
            string out_folder_directory_in,
            double required_zvtx_diff_in,
            int zvtx_dist_NClus_cut_in,
            int run_type_in,
            double zvtx_range_l_in,
            double zvtx_range_r_in,
            double zvtx_range_zoomin_l_in,
            double zvtx_range_zoomin_r_in,
            int unit_tag_in // note : 1 = cm, 0 = mm
        );

        pair<TH1F*, vector<TH1F*>> GetINTTRecoZHistVec();
        pair<TH1F*, vector<TH1F*>> GetINTTRecoZHistZoomInVec();

    protected:
        string input_directory;
        string input_file_name;
        string out_folder_directory;
        double required_zvtx_diff;
        int zvtx_dist_NClus_cut;
        int run_type;
        string plot_text;
        double zvtx_range_l;
        double zvtx_range_r;
        double zvtx_range_zoomin_l;
        double zvtx_range_zoomin_r;

        int eID_in;
        int in_nclu_inner;
        int in_nclu_outer;
        bool good_zvtx_tag;
        float Centrality_float;
        int is_min_bias;
        int is_min_bias_wozdc;
        double INTT_reco_zvtx;
        double MC_true_zvtx;
        double MBD_reco_z;
        double MBD_south_charge_sum;
        double MBD_north_charge_sum;
        double INTT_zvtx_reco_gaus_width;
        double INTT_zvtx_reco_hist_cut_width;

        int unit_tag;
        string unit_text;
        double unit_correction;

        // string color_code[5] = {"#fedc97", "#b5b682", "#7c9885", "#28666e", "#033f63"};
        vector<string> color_code = {"#167288", "#8cdaec", "#b45248", "#d48c84", "#a89a49", "#d6cfa2", "#3cb464", "#9bddb1", "#643c6a", "#836394"};
        // string color_code[12]  = {"#9e0142","#d53e4f","#f46d43","#fdae61","#fee08b",""}

        TFile * file_in;
        TTree * tree;
        TLatex * ltx;
        TLatex * draw_text;
        TLegend * leg;
        TLegend * leg_inc;
        TLegend * leg_single;
        TCanvas * c1;
        TLine * coord_line;

        TH1F * reco_Zvtx_dist;
        TH2F * NClus_intt_mbd_diff_corre;
        TH2F * mbdz_intt_mbd_diff_corre_cut;
        TH2F * intt_mbd_reco_z_corre;
        TH2F * intt_mbd_reco_z_corre_cut;
        TH1F * intt_mbd_reco_z_diff;
        TH1F * reco_Zvtx_dist_zoomin;
        TH2F * MBDz_MBDChargeAssy;
        TH2F * INTTz_MBDChargeAssy;
        TH1F * true_Zvtx_dist;
        TH1F * LB_hist_gaus_width;
        TH1F * LB_hist_cut_group_width;

        vector<TH1F *> reco_Zvtx_dist_Mbin; 
        vector<TH1F *> reco_Zvtx_dist_Mbin_zoomin; 
        vector<TH1F *> Z_resolution; 
        TH1F * high_NClus_Z_resolution;

        TH2F * true_zvtx_Mbin_Truezpos;
        TH2F * reco_zvtx_Mbin_Truezpos;
        TH2F * ratio_Mbin_Truezpos;
        TH2F * Z_resolution_Nclu;
        TH2F * Z_resolution_pos;
        TH2F * Z_resolution_pos_cut;
        // TH1F * Z_resolution;
        TH2F * Z_correlation;

        TF1 * gaus_fit_2;
        TF1 * linear_fit;
        TGraphErrors * Z_resolution_centrality_gr_fit;
        TGraphErrors * Z_mean_centrality_gr_fit;
        TGraphErrors * Z_resolution_centrality_gr_num;
        TGraphErrors * Z_mean_centrality_gr_num;

        map<int,int> centrality_map = ana_map_version::centrality_map;
        vector<string> centrality_region = ana_map_version::centrality_region;
        vector<double> z_region = ana_map_version::z_region;

        void TFile_init();
        void Hist_init();
        void Rest_init();
        
        void LoopEvent();
        void PrintPlot();

};

#endif 
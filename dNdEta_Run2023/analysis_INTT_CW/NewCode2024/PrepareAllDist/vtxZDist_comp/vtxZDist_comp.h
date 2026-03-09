#ifndef VTXZDIST_COMP_H
#define VTXZDIST_COMP_H

#include <iostream>
#include <numeric>

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TGaxis.h>
#include <TObject.h>
#include <TKey.h>
#include <TGraphErrors.h>

#include "sPhenixStyle.h"

#include "../../Constants.h"

class vtxZDist_comp{
    public:
        vtxZDist_comp(
            std::string sPH_labeling_in,
            std::string MC_labeling_in,
            std::vector<std::pair<std::string, std::pair<std::string, std::string>>> data_input_directory_pair_vec_in, // note: directory, and description
            std::vector<std::pair<std::string, std::pair<std::string, std::string>>> MC_input_directory_pair_vec_in, // note: directory, and description
            std::map<std::string, std::vector<std::tuple<double,double,std::string>>> labelling_vec_map_in, // note: starting XY, and description 
            std::string output_directory_in,

            bool WithVtxZReWeighting_in // note: MC with reweighting
        );

        void MakeDataPlot(std::string draw_method = "hist");
        void MakeMCPlot(std::string draw_method = "hist");
        void MakeComparisonPlot();

        void MakeVtxZCheckPlot();

        void PrepareINTTvtxZReWeight();

    protected:
        // Division: ---For constructor---------------------------------------------------------------------------------------------------------------------------------------------
        std::string sPH_labeling;
        std::string MC_labeling;
        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> data_input_directory_pair_vec;
        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> MC_input_directory_pair_vec;
        std::map<std::string, std::vector<std::tuple<double,double,std::string>>> labelling_vec_map;
        std::string output_directory;
        bool WithVtxZReWeighting;
        bool with_data;
        bool with_MC;


        std::map<std::string, std::pair<std::string, std::string>> data_input_directory_map;
        std::map<std::string, std::pair<std::string, std::string>> MC_input_directory_map;
        // Division: ---For input files---------------------------------------------------------------------------------------------------------------------------------------------
        std::vector<TFile*> file_data_vec;
        std::map<std::string, std::map<std::string, TH1D*>> data_h1D_map_map; // note : file_name, hist_name, hist
        std::map<std::string, std::map<std::string, TH2D*>> data_h2D_map_map; // note : file_name, hist_name, hist

        std::vector<TFile*> file_MC_vec;
        std::map<std::string, std::map<std::string, TH1D*>> MC_h1D_map_map; // note : file_name, hist_name, hist
        std::map<std::string, std::map<std::string, TH2D*>> MC_h2D_map_map; // note : file_name, hist_name, hist

        void PrepareInputFiles();
        std::string string_convertor(std::string input_string);


        // Division: ---For Plots---------------------------------------------------------------------------------------------------------------------------------------------
        TLatex * ltx;
        TCanvas * c1;
        TLegend * All_leg;
        TLegend * All_leg_long;
        TLatex * sub_ltx;

        std::pair<double,double> make_comparison(
            std::vector<std::pair<std::string, TH1D*>> data_hist_vec_in,  // note : description, hist
            std::vector<std::pair<std::string, TH1D*>> MC_hist_vec_in,    // note : description, hist
            std::string output_directory_in, 
            std::string output_filename_in, 
            std::string sub_label_text_in,
            bool set_log_y = false, 
            bool isData_more = false,
            double MainPlot_y_low = 0, 
            double MainPlot_y_top = -99, 
            double ratio_y_low = 0,
            double ratio_y_top = 2
        );

        void MakePlots(std::string draw_method, bool isData);

        double GetNonZeroMinimalBin(TH1D * hist_in);

        std::vector<std::string> h1comp_logy_plot_vec = {
            "h1D_VtxZDiff_MBD_INTT_Inclusive100",
            "h1D_VtxZDiff_MBD_INTT_Inclusive70",
            "h1D_VtxZDiff_MBD_INTT_HighNClus",
            "h1D_NgroupTrapezoidal",
            "h1D_NgroupCoarse",
            "h1D_INTTvtxZError",

            // note : for the RestDist
            "h1D_Cluster_phi_size",
            "h1D_Cluster_adc",
            "h1D_NClus",
            "h1D_NClus_inner",
            "h1D_NClus_outer",
            "h1D_inner_typeA_NClus",
            "h1D_outer_typeA_NClus",
            "h1D_inner_typeB_NClus",
            "h1D_outer_typeB_NClus"

        };

        std::vector<std::string> h1data_only_logy_plot_vec = {

        };

        std::vector<std::string> h1MC_only_logy_plot_vec = {

        };

        std::vector<std::string> h2_logz_plot_vec = {
            "h2D_NClus_INTTvtxZ",
            "h2D_NClus_TrapezoidalFitWidth",
            "h2D_NClus_TrapezoidalFWHM",
            "h2D_NClus_INTTvtxZError",

            // note : common
            "h2D_confirm_InttNClus_MbdChargeSum",
            "h2D_confirm_InttInnerOuterNClus",

            // note : for RestDist
            "h2D_ClusPhiSize_ClusAdc",
            "h2D_MBDcharge_south_north",
            "h2D_MBDvtxZ_MBDchargeSum",
            "h2D_MBDvtxZ_MBDchargeSouth",
            "h2D_MBDvtxZ_MBDchargeNorth",
            "h2D_NClus_ClusPhiSize"


        };


        // Division ---For constants---------------------------------------------------------------------------------------------------------------------------------------------
        int nCentrality_bin;

        double logy_max_factor;
        double lineary_max_factor;
        
        const std::vector<std::string> color_code = {
            "#000000",
            "#9e0142",
            "#5e4fa2",
            "#66c2a5",
            "#fdae61",
            "#0076A7",
            "#fee08b",

            "#d53e4f",
            "#abdda4",
            "#f46d43",
            "#e6f598",
            "#00A1FF",

            "#1b9e77",  // Teal Green  
            "#d95f02",  // Deep Orange  
            "#7570b3",  // Muted Purple  
            "#e7298a",  // Vivid Pink  
            "#66a61e",  // Olive Green  
            "#a6761d",  // Brown  
            "#666666",  // Dark Gray  
            "#f781bf",  // Light Pink  
            "#0173b2",  // Rich Blue  
            "#ff7f00"   // Bright Orange  

        };

        const std::vector<int> ROOT_color_code = {
            51, 61, 70, 79, 88, 98, 100, 113, 120, 129
        };
        
        const int inner_barrel_0 = 3;
        const int inner_barrel_1 = 4;
        const int outer_barrel_0 = 5;
        const int outer_barrel_1 = 6;

        const int nLadder_inner = 12;
        const int nLadder_outer = 16;

        const int south_typeB_ZID = 1;
        const int south_typeA_ZID = 0;
        const int north_typeA_ZID = 2;
        const int north_typeB_ZID = 3;
};

#endif
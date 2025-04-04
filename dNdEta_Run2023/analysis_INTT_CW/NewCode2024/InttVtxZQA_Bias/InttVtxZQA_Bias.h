#ifndef INTTVTXZQA_BIAS_H
#define INTTVTXZQA_BIAS_H

#include <iostream>
#include <vector>
#include <string>
#include <numeric>

#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TMath.h>
#include <TF1.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraphErrors.h>
#include <TLatex.h>

#include <TCanvas.h> // note : for the combined case
#include <TGraph.h>  // note : for the combined case

#include <TRandom.h> // note : for the offset
#include <TRandom3.h> // note : for the offset

#include <TColor.h>

#include <TObjArray.h>

#include "../Constants.h"

class InttVtxZQA_Bias{
    public:
        InttVtxZQA_Bias(
            int process_id_in,
            int runnumber_in,
            int run_nEvents_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,

            std::string output_file_name_suffix_in = "",

            std::pair<bool, int> ApplyEvtBcoFullDiffCut_in = {true, 61}
        );

        std::string GetOutputFileName() {return output_filename;}
        void PrepareEvent();
        void EndRun();

    protected:

        // division : ---For constructor---------------------------------------------------------------------------------------------------------------------------------------------
        int process_id;
        int runnumber;
        int run_nEvents;
        std::string input_directory;
        std::string input_file_name;
        std::string output_directory;
        std::string output_file_name_suffix;
        std::pair<bool, int> ApplyEvtBcoFullDiffCut;
        
        // division : ---For input file---------------------------------------------------------------------------------------------------------------------------------------------
        void PrepareInputFile();
        std::map<std::string, int> GetInputTreeBranches(TTree * m_tree_in);
        TFile * file_in; 
        TTree * tree_in;
        std::string tree_name = "EventTree";

        bool is_min_bias;
        float MBD_centrality;
        float MBD_z_vtx;
        float MBD_charge_sum;

        double INTTvtxZ;
        double INTTvtxZError;
        double NgroupTrapezoidal;
        double NgroupCoarse;
        double TrapezoidalFitWidth;
        double TrapezoidalFWHM;

        int NClus;
        int NClus_Layer1;


        // note : for data?
        int MBDNSg2;
        int MBDNSg2_vtxZ10cm;
        int MBDNSg2_vtxZ30cm;

        int InttBcoFullDiff_next; // note : for data

        // note : for MC
        int NTruthVtx;
        float TruthPV_trig_z;


        // note : the flag
        int m_withTrig = false;

        // division : ---For output file---------------------------------------------------------------------------------------------------------------------------------------------
        void PrepareOutPutFileName();
        void PrepareOutputFile();
        std::string output_filename;

        TFile * file_out;

        // division : ---For Prepare Hist---------------------------------------------------------------------------------------------------------------------------------------------
        void PrepareHist();
        std::map<std::string, TH1D*> h1D_NoQA_map;
        std::map<std::string, TH2D*> h2D_NoQA_map;

        std::map<std::string, TH1D*> h1D_PostQA_map;
        std::map<std::string, TH2D*> h2D_PostQA_map;

        TH1D * h1D_centrality_bin;

        // division : ---For analysis---------------------------------------------------------------------------------------------------------------------------------------------
        std::string ReplaceBy(const std::string& input_str, const std::string& target_str, const std::string& new_str);


        // division : ---For constants---------------------------------------------------------------------------------------------------------------------------------------------
        std::vector<double> centrality_edges = Constants::centrality_edges;
        int nCentrality_bin;

        int nVtxZ_bin = 60;
        std::pair<double, double> vtxZ_range = {-60, 60};

        int nVtxZ_bin_narrow = 20;
        std::pair<double, double> vtxZ_range_narrow = {-20, 20};

        int HighNClus = 500;

        double cut_GoodRecoVtxZ = Constants::cut_GoodRecoVtxZ; // note : unit [cm]

        double CentralityFineEdge_min = -0.5;
        double CentralityFineEdge_max = 100.5;
        int nCentralityFineBin = 101;

        std::pair<double, double> cut_vtxZDiff = Constants::cut_vtxZDiff;
        std::pair<double, double> cut_TrapezoidalFitWidth = Constants::cut_TrapezoidalFitWidth;
        std::pair<double, double> cut_TrapezoidalFWHM = Constants::cut_TrapezoidalFWHM;
        std::pair<double, double> cut_INTTvtxZError = Constants::cut_INTTvtxZError;
        std::pair<double, double> cut_GlobalMBDvtxZ = Constants::cut_GlobalMBDvtxZ;
        std::pair<double, double> cut_AnaVtxZ = Constants::cut_AnaVtxZ;

        double VtxZEdge_min = Constants::VtxZEdge_min;
        double VtxZEdge_max = Constants::VtxZEdge_max;
        int nVtxZBin = Constants::nVtxZBin;

        int cut_InttBcoFullDIff_next = Constants::cut_InttBcoFullDIff_next;
        int Semi_inclusive_bin = Constants::Semi_inclusive_bin;

        // std::vector<std::pair<double,double>> Mbin_NClus_edges = {
        //     {6000,10000}, // note : 1 - 3
        //     {6000,10000}, // note : 4 - 6
        //     {6000,10000}, // note : 7 - 10

        //     {3500,8500}, // note : 11 - 15,
        //     {3500,8500}, // note : 16 - 20,

        //     {2000,6000}, // note : 21 - 25,
        //     {2000,6000}, // note : 26 - 30,

        //     {1200,4200}, // note : 31 - 35,
        //     {1200,4200}, // note : 36 - 40,

        //     {800,3000}, // note : 41 - 45,
        //     {800,3000}, // note : 46 - 50,

        //     {500,2500}, // note : 51 - 55,
        //     {500,2500}, // note : 56 - 60,

        //     {200,1600}, // note : 61 - 65,
        //     {200,1600}, // note : 66 - 70,
            

        //     {200,1600}, // note : 71,
        //     {200,1600}, // note : ,
        //     {200,1600}, // note : 81,
        //     {200,1600}, // note : ,
        //     {200,1600}, // note : 91,
        //     {200,1600} // note : ,

        // };

        std::vector<std::pair<double,double>> Mbin_NClus_edges = {
            {3700,8800}, // note : 1 - 3
            {3000,8000}, // note : 4 - 6
            {2500,7500}, // note : 7 - 10

            {2000,6500}, // note : 11 - 15,
            {1500,5500}, // note : 16 - 20,

            {1000,4700}, // note : 21 - 25, // note : 5
            {700,4000}, // note : 26 - 30,

            {700,3300}, // note : 31 - 35,
            {400,2800}, // note : 36 - 40,

            {300, 2400}, // note : 41 - 45,
            {200, 2000}, // note : 46 - 50,

            {0,1700}, // note : 51 - 55, // note : 11
            {0,1300}, // note : 56 - 60,

            {0, 1000}, // note : 61 - 65,
            {0, 800}, // note : 66 - 70,
            

            {200,1600}, // note : 71,
            {200,1600}, // note : ,
            {200,1600}, // note : 81,
            {200,1600}, // note : ,
            {200,1600}, // note : 91,
            {200,1600} // note : ,

        };

};

#endif
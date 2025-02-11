#ifndef FINALRESULT_H
#define FINALRESULT_H

#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <cctype> // For isdigit

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
#include <THStack.h>
#include <TCanvas.h> // note : for the combined case
#include <TGraph.h>  // note : for the combined case

#include <TKey.h>
#include <TRandom.h> // note : for the offset
#include <TRandom3.h> // note : for the offset

#include <TColor.h>
#include <TLegend.h>

#include <TObjArray.h>

// #include "../Constants.h"
#include "sPhenixStyle.h"

class FinalResult{
    public:
        FinalResult(
            int runnumber_in,
            int Mbin_in,
            std::string StandardData_directory_in,
            std::string StandardData_file_name_in,
            std::string StandardMC_directory_in,
            std::string StandardMC_file_name_in,
            std::string sPH_label_in = "Internal",

            std::string Output_directory_in = "Not_given"
        );

        void SetEtaRange(std::pair<double,double> eta_range_in) {
            eta_range = eta_range_in;
        }

        void SetAnaDescription(std::pair<std::pair<double,double>, std::string> input_text) {
            AnaDescription = input_text;
        }

        void SetCollisionStr(std::pair<std::pair<double,double>, std::string> input_text) {
            Collision_str = input_text;
        }

        void SetFinal_Data_MC_text(std::pair<std::string, std::string> input_pair) {
            Final_Data_MC_text = input_pair;
        }

        void SetSystUncPlot_Ymax(double Ymax_in) {
            SystUncPlot_Ymax = Ymax_in;
        }

        void PrepareBaseLineTGraph();
        void PrepareMCClosureTGraph();

        void DrawAlphaCorrectionPlots(
            std::string AlphaCorr_file_directory, 
            std::vector<std::tuple<double,double,std::string>> additional_text,
            std::vector<std::tuple<int, std::string, std::string>> legend_text
        );

        void DrawGoodPair2DFineBinPlot(std::vector<std::tuple<double,double,std::string>> additional_text = {});

        void DrawRecoTrackletDataMCPlot(
            std::vector<std::tuple<double,double,std::string>> additional_text,
            std::vector<std::tuple<int, std::string, std::string>> legend_text
        );

        std::string GetOutputFileName() {
            return output_folder_name;
        }

        void PrepareStatisticalError();
        void PrepareRunSegmentError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in); // note : run1, run2 // todo: the baseline text in the back
        void PrepareClusAdcError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in); // note : NoThreshold
        void PrepareGeoOffsetError(std::string file_directory_in, std::string alpha_corr_directory_in);
        void PrepareDeltaPhiError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in); // note : 0.018 and 0.024
        void PrepareClusPhiSizeError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in);
        void PrepareMCMergedError(std::vector<std::string> file_directory_vec_in); // note : the errror due to different ways of MC merged
        void PrepareFinalError();
        void PrepareFinalResult(double Hist_Y_max);
        void EndRun();

        

    protected:
        // note : for the constructor
        int runnumber;
        int Mbin;
        std::string StandardData_directory;
        std::string StandardData_file_name;
        std::string StandardMC_directory;
        std::string StandardMC_file_name;
        std::string sPH_label;

        std::string Output_directory;


        std::string final_output_directory;

        // note : for the standard
        TFile * file_in_data_standard;
        TFile * file_in_MC_standard;
        TH1D * h1D_data_standard = nullptr;
        TGraphErrors * gE_data_final = nullptr;
        TH1D * h1D_truth_standard = nullptr;

        TH1D * h1D_MC_closure_standard = nullptr;
        TGraph * gr_MC_closure_standard = nullptr;

        // note : for the analysis 
        void h1D_to_AbsRatio(TH1D * h1D_in);
        std::pair<double, double> GetH1DMinMax(TH1D * h1D_in);
        std::vector<TH1D*> h1D_RunSegmentError_vec;
        std::vector<TH1D*> h1D_ClusAdcError_vec;
        std::vector<TH1D*> h1D_GeoOffsetError_vec;
        std::vector<TH1D*> h1D_DeltaPhiError_vec;
        std::vector<TH1D*> h1D_ClusPhiSizeError_vec;
        std::vector<TH1D*> h1D_MCMergedError_vec;

        std::pair<std::pair<double,double>, std::string> AnaDescription;
        std::pair<std::pair<double,double>, std::string> Collision_str;
        std::pair<std::string, std::string> Final_Data_MC_text = {"",""};

        // note : for the errors
        TH1D * h1D_error_statistic = nullptr;
        TH1D * h1D_error_Run_segmentation = nullptr;
        TH1D * h1D_error_ClusAdc = nullptr;
        TH1D * h1D_error_GeoOffset = nullptr;
        TH1D * h1D_error_DeltaPhi = nullptr;   
        TH1D * h1D_error_ClusPhiSize = nullptr;
        TH1D * h1D_error_MCMerged = nullptr;     
        TH1D * h1D_error_Final;

        // note : the TGraph
        TGraph * gr_dNdEta_baseline;

        // note : for the final results 
        std::pair<double, double> eta_range = {-1.9, 1.9}; // todo : the default eta range
        void PrepareOutputFolderName();
        TH1D * h1D_FindLargestOnes(std::string hist_name, std::vector<TH1D*> h1D_vec_in);
        TGraph * h1D_to_TGraph(TH1D * hist_in);
        std::string output_folder_name;
        TFile * file_out;
        TTree * tree_out;

        std::pair<double,double> UncRange_StatUnc = {std::nan("1"), std::nan("1")};
        std::pair<double,double> UncRange_RunSegment = {std::nan("1"), std::nan("1")};
        std::pair<double,double> UncRange_ClusAdc = {std::nan("1"), std::nan("1")};
        std::pair<double,double> UncRange_GeoOffset = {std::nan("1"), std::nan("1")};
        std::pair<double,double> UncRange_DeltaPhi = {std::nan("1"), std::nan("1")};
        std::pair<double,double> UncRange_ClusPhiSize = {std::nan("1"), std::nan("1")};
        std::pair<double,double> UncRange_Final = {std::nan("1"), std::nan("1")};

        TCanvas * c1;
        TPad * pad1;

        TLatex * ltx;
        TLatex * draw_text;
        TLegend * leg_errors;
        TLegend * leg_final;
        TLegend * leg_variation;
        TLegend * leg_TruthReco;

        double SystUncPlot_Ymax = 0.11;


        // note : for constants 
        std::string StandardData_h1D_name = "h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC";
        std::string StandardTruth_h1D_name = "h1D_TruedNdEta";
        
        std::string h1D_RecoTracklet_name = "h1D_RotatedBkg_RecoTrackletEtaPerEvt";
        std::string h1D_AlphaCorr_name = "h1D_RotatedBkg_alpha_correction";
        std::string h2D_GoodProtoTracklet_EtaVtxZ_FineBin_name = "h2D_GoodProtoTracklet_EtaVtxZ_FineBin";

        const std::vector<std::string> color_code = {
            "#9e0142",
            "#fee08b",
            "#66c2a5",
            "#f46d43",
            "#3288bd",
            "#5e4fa2",
            "#000000",
            
            "#005493",
            "#abdda4",
            "#e6f598",
            "#fdae61",
            "#d53e4f"
        };

        const std::vector<int> marker_code = {
            25,
            28,
            27,
            26
            // 21,
            // 23,
            // 27,
            // 28,
            // 29
        };
};

#endif
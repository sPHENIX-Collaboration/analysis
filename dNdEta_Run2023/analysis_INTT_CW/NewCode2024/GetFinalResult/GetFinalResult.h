#ifndef GETFINALRESULT_H
#define GETFINALRESULT_H

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

#include "../Constants.h"
#include "sPhenixStyle.h"

#include "../PreparedNdEta/PreparedNdEtaEach.h"

using namespace std;

class GetFinalResult{
    public:
        GetFinalResult(
            int runnumber_in,
            int SelectedMbin_in, // note : 0, 1, ---- 10, 70, 100 
            std::pair<double,double> vtxZ_range_in, // note : cm
            bool isTypeA_in,
            bool ApplyGeoAccCorr_in,
            std::pair<bool, std::pair<double,double>> cut_EtaRange_in,

            std::string output_file_name_suffix_in,

            std::string output_directory_in
        );

        std::string GetSetResultRangeFolderName() {
            return SetResultRangeFolderName;
        }

        // note : here are for preparing the reco. dNdEta
        void PrepareBaseLine(
            std::string data_input_directory,
            std::string data_input_filename,
            
            std::string MC_input_directory,
            std::string MC1_input_filename,
            std::string MC2_input_filename
        );
        
        void PrepareRunSegment(
            std::string data_input_directory,
            std::vector<std::string> data_input_filename,
            
            std::string MC_input_directory,
            std::string MC1_input_filename,
            std::string MC2_input_filename
        ); // note : two times
        
        void PrepareClusAdcCut(
            int run_index, // note : 0, 1,
            std::string data_input_directory,
            std::string data_input_filename,
            
            std::string MC_input_directory,
            std::string MC1_input_filename,
            std::string MC2_input_filename
        ); // todo : can be twice
        
        void PrepareClusPhiCut(
            std::string data_input_directory,
            std::vector<std::string> data_input_filename,
            
            std::string MC_input_directory,
            std::string MC1_input_filename,
            std::string MC2_input_filename  
        ); 
        
        void PrepareDeltaPhiCut(
            std::vector<double> cut_SigDeltaPhi,    
            std::string data_input_directory,
            std::string data_input_filename,
            
            std::string MC_input_directory,
            std::string MC1_input_filename,
            std::string MC2_input_filename
        ); // note : two times 

        // note : here are for preparing the systematic uncertainty
        void PrepareStat_Unc();
        void PrepareGeoMisalignment_Unc();

        void PrepareRunSegment_Unc();
        void PrepareClusAdcCut_Unc();
        void PrepareClusPhiCut_Unc();
        void PrepareDeltaPhiCut_Unc();

        // note : here are for preparing the final result
        void PrepareFinalResult();

    protected:
        // Division : -For constructor-----------------------------------------------------------
        int runnumber;
        int SelectedMbin;
        std::pair<double,double> vtxZ_range;
        bool isTypeA;
        bool ApplyGeoAccCorr;
        std::pair<bool, std::pair<double,double>> cut_EtaRange;
        std::string output_file_name_suffix;
        std::string output_directory;

        // Division : -The strings-----------------------------------------------------------

        std::string SetResultRangeFolderName;
        std::string SemiMotherFolderName;
        std::string Folder_BaseLine = "Folder_BaseLine";
        std::string Folder_RunSegment = "Folder_RunSegment";
        std::string Folder_ClusAdcCut = "Folder_ClusAdcCut";
        std::string Folder_ClusPhiCut = "Folder_ClusPhiCut";
        std::string Folder_DeltaPhiCut = "Folder_DeltaPhiCut";

        std::string BaseLine_AlphaCorr_directory;
        std::string BaseLine_dNdEta_directory;

        std::string no_map = "no_map";

        // Division : -The constants-----------------------------------------------------------
        std::pair<double,double> eta_range_dNdEta_Preparation = {-1.9, 1.9};

        // Division : -the macros-----------------------------------------------------------
        std::string Run_PreparedNdEtaEach(
            int process_id,
            int run_num,
            std::string input_directory,
            std::string input_filename,
            std::string output_directory,
            
            // todo : modify here
            std::string output_file_name_suffix, 

            bool ApplyAlphaCorr,
            bool func_ApplyGeoAccCorr,

            bool isTypeA,
            std::pair<double,double> cut_INTTvtxZ,
            int SelectedMbin,

            std::pair<bool, std::pair<double,double>> setBkgRotated_DeltaPhi_Signal_range,
            std::pair<bool, std::pair<double,double>> setEtaRange,
            std::string GeoAccCorr_input_directory = "no_map",
            std::string alpha_correction_input_directory = "no_map"
        );

        std::vector<std::string> PreparedNdEtaPlain(
            int index,
            bool PrepareAlphaCorr,
            bool RunComparison,
            std::string sub_folder_name,
            std::string data_input_directory,
            std::string data_input_filename,
            
            std::string MC_input_directory,
            std::string MC1_input_filename,
            std::string MC2_input_filename,

            std::pair<bool, std::pair<double,double>> cut_DeltaPhi_Signal_range = {false, {-0.021, 0.021}}
        );

        int DataMcComp(string data_directory_in, string MC_directory_in, string output_directory_in, string output_filename_in);
        int McMcComp(  string MC1_directory_in, string MC2_directory_in, string output_directory_in, string output_filename_in);

        std::map<std::string, TH1D*> GetAlphaCorrectionH1DMap(std::string alpha_correction_input_directory_in, std::vector<std::string> map_name_in);
        std::map<std::string, TH2D*> GetGeoAccCorrH2DMap(std::string GeoAccCorr_input_directory_in, std::vector<std::string> map_name_in);
};

#endif
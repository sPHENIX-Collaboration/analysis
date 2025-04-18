#ifndef GETMULTIPLICITYMAP_H
#define GETMULTIPLICITYMAP_H

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
#include <TKey.h>

#include "../../Constants.h"

class GetMultiplicityMap{
    public:
        GetMultiplicityMap(
            int runnumber_in,
            std::string data_directory_in,
            std::string data_file_name_in,
            std::string MC_directory_in,
            std::string MC_file_name_in,
            std::string output_directory_in,

            std::string output_file_name_suffix_in,

            double SetMbinFloat_in,
            std::pair<double, double> VtxZRange_in,
            bool IsZClustering_in,
            bool BcoFullDiffCut_in,
            std::pair<bool, std::pair<double, double>> isClusQA_in // note : {adc, phi size}
        );

        std::string GetOutputFileName() {return output_filename;}

        void GetUsedZIDVec();
        // void GetLadderRadius();
        void h2DNormalizedByRadius();
        void h2DNormalized();
        void DataMCDivision();
        void PrepareMulMap();

        void EndRun();

    protected:

        // note : for constructor
        int runnumber;
        std::string data_directory;
        std::string data_file_name;
        std::string MC_directory;
        std::string MC_file_name;
        std::string output_directory;

        std::string output_file_name_suffix;

        double SetMbinFloat;
        std::pair<double, double> VtxZRange;
        bool IsZClustering;
        bool BcoFullDiffCut;
        std::pair<bool, std::pair<double, double>> isClusQA; // note : {adc, phi size}

        // note : for the input file
        TFile * file_in_data;
        TFile * file_in_MC;

        std::map<std::string, std::pair<TH1D*, TH1D*>> h1D_target_map;
        std::map<std::string, std::pair<TH2D*, TH2D*>> h2D_target_map;

        void PrepareInputRootFile();

        // note : for analysis 
        std::vector<int> used_zid_data_vec;
        std::vector<int> used_zid_MC_vec;


        // note : for the output file
        std::string output_filename;
        TFile * file_out;
        void PrepareOutPutFileName();
        void PrepareOutPutRootFile();

        // note : for histogram
        void PrepareHistograms();
        std::map<std::string, TH2D*> h2D_map;
        std::map<std::string, TH1D*> h1D_map;
        TH2D * h2D_MulMap;
        TH2D * h2D_MaskedMap;
        TH2D * h2D_RatioMap;

        // note : for constant
        std::pair<double,double> Ratio_cut_pair = {0.8, 1.2};

        double sensor_width = 1.9968; // note : in cm
        int nZbin = Constants::nZbin;
        double Zmin = Constants::Zmin;
        double Zmax = Constants::Zmax;
};

#endif
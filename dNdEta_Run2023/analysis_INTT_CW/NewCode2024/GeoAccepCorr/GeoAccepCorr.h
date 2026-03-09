#ifndef GEOACCEPCORR_H
#define GEOACCEPCORR_H

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

#include <TObjArray.h>

#include "../Constants.h"

class GeoAccepCorr{
    public:
        GeoAccepCorr(
            int process_id_in, 
            int runnumber_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,
            std::string output_file_name_suffix_in,

            int setMbin_in
        );

        std::string GetOutputFileName() {
            return output_filename;
        }

        void SetBkgRotated_DeltaPhi_Signal_range(std::pair<double, double> range_in) {
            BkgRotated_DeltaPhi_Signal_range = range_in;
        }

        void PrepareStacks();
        void PrepareMultiplicity();
        void EndRun();

    protected:
        // note : ------------------ for constructor ------------------    
        int process_id;
        int runnumber;
        std::string input_directory;
        std::string input_file_name;
        std::string output_directory;
        std::string output_file_name_suffix;

        int setMbin;

        // note : ------------------ for output file ------------------
        TFile * file_out;
        std::string output_filename;
        void PrepareOutPutFileName();
        void PrepareOutPutRootFile();

        TCanvas * c1;

        // note : ------------------ for input file ------------------
        TFile * file_in;
        TTree * tree_in;
        void PrepareInputRootFie();
        std::map<std::string, TH1D*> h1D_input_map;
        std::map<std::string, TH2D*> h2D_input_map;

        std::vector<double> *centrality_edges;
        int nCentrality_bin;

        double EtaEdge_min;
        double EtaEdge_max;
        int nEtaBin;

        double VtxZEdge_min;
        double VtxZEdge_max;
        int nVtxZBin;

        double DeltaPhiEdge_min;
        double DeltaPhiEdge_max;
        int nDeltaPhiBin;

        // note : ------------------ for THStack and Hist ------------------
        void PrepareHist();
        std::map<std::string, THStack*> hstack1D_DeltaPhi_map;
        std::map<std::string, THStack*> hstack2D_RotatedBkg_GoodPair_FineBin_map;
        std::map<std::string, THStack*> hstack2D_BestPair_map;

        TH2D * h2D_RotatedBkg_Rebin;
        TH2D * h2D_RotatedBkg_rotated_Rebin;
        TH2D * h2D_typeA_RotatedBkg_Rebin;
        TH2D * h2D_typeA_RotatedBkg_rotated_Rebin;

        TH2D * h2D_RotatedBkg_TrackletEta_VtxZ;
        TH2D * h2D_RotatedBkg_TrackletEta_VtxZ_FineBin;
        TH2D * h2D_typeA_RotatedBkg_TrackletEta_VtxZ;
        TH2D * h2D_typeA_RotatedBkg_TrackletEta_VtxZ_FineBin;

        TH2D * h2D_BestPair_TrackletEta_VtxZ;
        TH2D * h2D_typeA_BestPair_TrackletEta_VtxZ;

        std::map<std::string, TH1D*> h1D_RotatedBkg_DeltaPhi_Signal_map;

        // note : ------------------ for analysis ------------------
        std::pair<int,int> get_DeltaPhi_SingleBin(TH1D * hist_in, std::pair<double, double> range_in);
        std::tuple<int, int, int, int, int, int> GetHistStringInfo(std::string hist_name);
        void CleanH2DNegativeBin(TH2D * hist_in);

        // note : ------------------ for constant ------------------
        const std::vector<int> ROOT_color_code = {
            51, 61, 70, 79, 88, 98
        };

        std::pair<double,double> BkgRotated_DeltaPhi_Signal_range = {-0.021, 0.021};

        double Semi_inclusive_bin = Constants::Semi_inclusive_bin;

};

#endif
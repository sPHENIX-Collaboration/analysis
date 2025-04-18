#ifndef PREPAREDNDETAEACH_H
#define PREPAREDNDETAEACH_H

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


class PreparedNdEtaEach{
    public:
        PreparedNdEtaEach(
            int process_id_in, // note : 1 or 2
            int runnumber_in, // note : still, (54280 or -1)
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,
            std::string output_file_name_suffix_in,

            bool ApplyAlphaCorr_in,
            bool ApplyGeoAccCorr_in,

            bool isTypeA_in,
            std::pair<double,double> cut_INTTvtxZ_in,
            int SelectedMbin_in // note : 0, 1, ---- 10, 70, 100 
        );

        std::vector<std::string> GetOutputFileName() {
            return {output_filename_DeltaPhi, output_filename_dNdEta, output_filename_pdf};
        }

        std::vector<std::string> GetAlphaCorrectionNameMap() {return alpha_correction_name_map;}

        void SetAlphaCorrectionH1DMap(std::map<std::string, TH1D*> map_in) {
            h1D_alpha_correction_map_in = map_in;

            for (auto &pair : h1D_alpha_correction_map_in){
                if (std::find(alpha_correction_name_map.begin(), alpha_correction_name_map.end(), pair.first) == alpha_correction_name_map.end()){
                    std::cout << "Error : the alpha correction name is not found in the map" << std::endl;
                    exit(1);
                }

                // if (alpha_correction_name_map.find(pair.first) == alpha_correction_name_map.end()){
                //     std::cout << "Error : the alpha correction name is not found in the map" << std::endl;
                //     exit(1);
                // }
            }
        }

        std::vector<std::string> GetGeoAccCorrNameMap() {return GeoAccCorr_name_map;}
        
        void SetGeoAccCorrH2DMap(std::map<std::string, TH2D*> map_in) {
            h2D_GeoAccCorr_map = map_in;

            for (auto &pair : h2D_GeoAccCorr_map){
                if (std::find(GeoAccCorr_name_map.begin(), GeoAccCorr_name_map.end(), pair.first) == GeoAccCorr_name_map.end()){
                    std::cout << "Error : the GeoAccCorr name is not found in the map" << std::endl;
                    exit(1);
                }
            }

        }


        void SetSelectedEtaRange(std::pair<double, double> cut_EtaRange_in) {
            cut_EtaRange_pair = cut_EtaRange_in;
        }

        void SetBkgRotated_DeltaPhi_Signal_range(std::pair<double, double> range_in) {
            BkgRotated_DeltaPhi_Signal_range = range_in;
        }

        void PrepareStacks();
        void DoFittings();
        void PrepareMultiplicity();
        void PreparedNdEtaHist();
        void DeriveAlphaCorrection();
        void EndRun();

    protected:

        // Division:---for the constructor------------------------------------------------------------------------------------------------
        int process_id;
        int runnumber;
        std::string input_directory;
        std::string input_file_name;
        std::string output_directory;
        std::string output_file_name_suffix;
        bool ApplyAlphaCorr;
        bool ApplyGeoAccCorr;

        bool isTypeA;
        std::pair<double,double> cut_INTTvtxZ;
        int SelectedMbin;

        // Division:---input root file------------------------------------------------------------------------------------------------
        TFile * file_in;
        TTree * tree_in;
        void PrepareInputRootFie();
        std::map<std::string, TH1D*> h1D_input_map;
        std::map<std::string, TH2D*> h2D_input_map;

        std::vector<double> *centrality_edges;
        int nCentrality_bin;

        double CentralityFineEdge_min;
        double CentralityFineEdge_max;
        int nCentralityFineBin;

        double EtaEdge_min;
        double EtaEdge_max;
        int nEtaBin;

        double VtxZEdge_min;
        double VtxZEdge_max;
        int nVtxZBin;

        double DeltaPhiEdge_min;
        double DeltaPhiEdge_max;
        int nDeltaPhiBin;

        std::pair<double, double> *cut_GoodProtoTracklet_DeltaPhi;


        // Division:---output root file------------------------------------------------------------------------------------------------
        TFile * file_out_DeltaPhi;
        TFile * file_out_dNdEta;
        std::string output_filename;
        std::string output_filename_DeltaPhi;
        std::string output_filename_dNdEta;
        std::string output_filename_pdf;
        void PrepareOutPutFileName();
        void PrepareOutPutRootFile();

        TCanvas * c1;

        // Division:---analysis------------------------------------------------------------------------------------------------
        std::map<int, int> GetVtxZIndexMap();
        void PrepareHistFits();
        std::string GetObjectName();
        std::tuple<int, int, int, int, int, int> GetHistStringInfo(std::string hist_name);
        std::vector<double> find_Ngroup(TH1D * hist_in);
        double get_dist_offset(TH1D * hist_in, int check_N_bin);
        double get_hstack2D_GoodProtoTracklet_count(THStack * stack_in, int eta_bin_in);
        double get_h2D_GoodProtoTracklet_count(TH2D * h2D_in, int eta_bin_in);
        double get_EvtCount(TH2D * hist_in, int centrality_bin_in);
        void Convert_to_PerEvt(TH1D * hist_in, double Nevent);
        void SetH2DNonZeroBins(TH2D* hist_in, double value_in);
        std::pair<int,int> get_DeltaPhi_SingleBin(TH1D * hist_in, std::pair<double, double> range_in);

        std::map<int, int> vtxZ_index_map;
        std::pair<double, double> cut_EtaRange_pair = {std::nan(""), std::nan("")};

        // Division:---histogram------------------------------------------------------------------------------------------------
        // note : signal (eta-vtxZ) centrality segemntation
        std::map<std::string, TH1D*> h1D_BestPair_RecoTrackletEta_map;    // note : for the total count
        std::map<std::string, TH1D*> h1D_BestPair_RecoTrackletEtaPerEvt_map;
        std::map<std::string, TH1D*> h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map;
        
        std::map<std::string, THStack*> hstack1D_DeltaPhi_map;
        std::map<std::string, TH1D*> h1D_RotatedBkg_RecoTrackletEta_map; // note : for the total count
        std::map<std::string, TH1D*> h1D_RotatedBkg_RecoTrackletEtaPerEvt_map;
        std::map<std::string, TH1D*> h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map;

        std::map<std::string, TH1D*> h1D_RotatedBkg_DeltaPhi_Signal_map;
        
        std::map<std::string, THStack*> hstack2D_GoodProtoTracklet_map; // note : for the good pair

        // note : for the best pair
        THStack * hstack1D_BestPair_ClusPhiSize;
        THStack * hstack1D_BestPair_ClusAdc;
        THStack * hstack1D_BestPair_DeltaPhi;
        THStack * hstack1D_BestPair_DeltaEta;
        THStack * hstack2D_BestPairEtaVtxZ;
        THStack * hstack2D_BestPairEtaVtxZ_FineBin;

        // note : for the final Truth
        std::map<std::string, THStack*> hstack2D_TrueEtaVtxZ_map;
        std::map<std::string, THStack*> hstack1D_TrueEta_map; // note : for the total count
        std::map<std::string, TH1D*> h1D_TruedNdEta_map;        

        // note : for the alpha correction
        std::map<std::string, TH1D*> h1D_alpha_correction_map_in;
        std::map<std::string, TH1D*> h1D_alpha_correction_map_out;
        std::vector<std::string> alpha_correction_name_map = {
            "h1D_BestPair_alpha_correction",
            "h1D_RotatedBkg_alpha_correction"
        };

        // note : for Geo Acceprance correction
        std::map<std::string, TH2D*> h2D_GeoAccCorr_map;
        std::vector<std::string> GeoAccCorr_name_map;

        // Division:---fitting------------------------------------------------------------------------------------------------
        // std::map<std::string, TF1*> f1_BkgPol2_Fit_map;
        // std::map<std::string, TF1*> f1_BkgPol2_Draw_map;
        std::map<std::string, TF1*> f1_SigBkgPol2_Fit_map;
        std::map<std::string, TF1*> f1_SigBkgPol2_DrawSig_map;
        std::map<std::string, TF1*> f1_SigBkgPol2_DrawBkgPol2_map;

        std::map<std::string, TF1*> f1_BkgPolTwo_Fit_map;

        // Division:---Constants------------------------------------------------------------------------------------------------
        int Semi_inclusive_Mbin = Constants::Semi_inclusive_bin;
        int Semi_inclusive_interval = Constants::Semi_inclusive_interval;

        const std::vector<int> ROOT_color_code = {
            51, 61, 70, 79, 88, 98
        };

        std::pair<double,double> BkgRotated_DeltaPhi_Signal_range = {-0.026, 0.026};


        // Division:---the functions------------------------------------------------------------------------------------------------
        static double gaus_func(double *x, double *par)
        {
            // note : par[0] : size
            // note : par[1] : mean
            // note : par[2] : width
            // note : par[3] : offset 
            return par[0] * TMath::Gaus(x[0],par[1],par[2]); //+ par[3];
        }

        static  double gaus_pol2_func(double *x, double *par)
        {
            // note : par[0] : size
            // note : par[1] : mean
            // note : par[2] : width

            double gaus_func = par[0] * TMath::Gaus(x[0],par[1],par[2]);
            double pol2_func = par[3] + par[4]* (x[0]-par[6]) - fabs(par[5]) * pow((x[0]-par[6]),2);

            return gaus_func + pol2_func;
        }

        static  double bkg_pol2_func(double *x, double *par)
        {
            if (x[0] > par[4] && x[0] < par[5]) {
                TF1::RejectPoint();
                return 0;
            }
            return par[0] + par[1]* (x[0]-par[3]) - fabs(par[2]) * pow((x[0]-par[3]),2);

            // note : p[0] + p[1]*(x-p[3])+p[2] * (x-p[3])^2, p[4] sets the signal range that should be excluded in the fit
        }

        static  double full_pol2_func(double *x, double *par)
        {
            return par[0] + par[1]* (x[0]-par[3]) - fabs(par[2]) * pow((x[0]-par[3]),2);

            // note : p[0] + p[1]*(x-p[3])+p[2] * (x-p[3])^2
        }
};

#endif
#ifndef PREPAREDNDETA_H
#define PREPAREDNDETA_H

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


// #include "sPhenixStyle.h"

class PreparedNdEta{
    public:
        PreparedNdEta(
            int process_id_in,
            int runnumber_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,
            std::string output_file_name_suffix_in,

            bool ApplyAlphaCorr_in
        );

        std::map<std::string, int> GetAlphaCorrectionNameMap() {return alpha_correction_name_map;}

        void SetAlphaCorrectionH2DMap(std::map<std::string, TH2D*> map_in) {
            h2D_alpha_correction_map_in = map_in;

            for (auto &pair : h2D_alpha_correction_map_in){
                if (alpha_correction_name_map.find(pair.first) == alpha_correction_name_map.end()){
                    std::cout << "Error : the alpha correction name is not found in the map" << std::endl;
                    exit(1);
                }
            }
        }
        std::string GetOutputFileName() {return output_filename;}

        void PrepareStacks();
        void DoFittings();
        void PrepareMultiplicity();
        void PreparedNdEtaHist();
        void DeriveAlphaCorrection();
        void EndRun();
    
    protected:

        // note : ----------------- for the constructor -----------------
        int process_id;
        int runnumber;
        std::string input_directory;
        std::string input_file_name;
        std::string output_directory;
        std::string output_file_name_suffix;
        bool ApplyAlphaCorr;

        // note : ----------------- for the input file -----------------
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

        // note : ----------------- for the histogram -----------------   
        void PrepareHistFits();
        void PrepareTrackletEtaHist(std::map<std::string, TH1D*> &map_in, std::string name_in);

        // note : multiplicity, dNdeta
        std::map<std::string, TH1D*> h1D_FitBkg_RecoTrackletEta_map;
        std::map<std::string, TH1D*> h1D_FitBkg_RecoTrackletEtaPerEvt_map;
        std::map<std::string, TH1D*> h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map;
        std::map<std::string, TH1D*> h1D_RotatedBkg_RecoTrackletEta_map;
        std::map<std::string, TH1D*> h1D_RotatedBkg_RecoTrackletEtaPerEvt_map;
        std::map<std::string, TH1D*> h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map;

        std::map<std::string, THStack*> hstack1D_map;

        // note : signal (eta-vtxZ) centrality segemntation
        std::map<std::string, TH2D*> h2D_map; 
        std::map<std::string, THStack*> hstack2D_map;

        // note : for the final Truth
        std::map<std::string, TH1D*> h1D_TruedNdEta_map;

        // note : for the alpha correction
        // note : X: eta, Y: Mbin
        std::map<std::string, TH2D*> h2D_alpha_correction_map_in;
        std::map<std::string, TH2D*> h2D_alpha_correction_map_out;
        std::map<std::string, int> alpha_correction_name_map = {
            {"h2D_FitBkg_alpha_correction", 1},
            {"h2D_RotatedBkg_alpha_correction", 1},
            {"h2D_typeA_FitBkg_alpha_correction", 1},
            {"h2D_typeA_RotatedBkg_alpha_correction", 1}
        };

        // note : ----------------- for the fitting -----------------
        std::map<std::string, TF1*> f1_BkgPol2_Fit_map;
        std::map<std::string, TF1*> f1_BkgPol2_Draw_map;
        std::map<std::string, TF1*> f1_SigBkgPol2_Fit_map;
        std::map<std::string, TF1*> f1_SigBkgPol2_DrawSig_map;
        std::map<std::string, TF1*> f1_SigBkgPol2_DrawBkgPol2_map;

        // note : ----------------- for the output file -----------------
        TFile * file_out;
        TFile * file_out_dNdEta;
        std::string output_filename;
        void PrepareOutPutFileName();
        void PrepareOutPutRootFile();

        TCanvas * c1;

        // note : ----------------- for the analysis -----------------
        std::map<int, int> GetVtxZIndexMap();
        std::tuple<int, int, int, int, int, int> GetHistStringInfo(std::string hist_name);
        std::vector<double> find_Ngroup(TH1D * hist_in);
        double get_dist_offset(TH1D * hist_in, int check_N_bin);
        double get_h2D_GoodProtoTracklet_count(TH2D * hist_in, int eta_bin_in);
        double get_EvtCount(TH2D * hist_in, int centrality_bin_in);
        void Convert_to_PerEvt(TH1D * hist_in, double Nevent);

        std::map<int, int> vtxZ_index_map;

        // note : ----------------- for some constant -----------------
        std::pair<double, double> cut_INTTvtxZ = {-10, 10};
        const std::vector<int> ROOT_color_code = {
            51, 61, 70, 79, 88, 98, 100, 113, 120, 129
        };

        int Semi_inclusive_Mbin = 7;

        // note : ----------------- for the functions -----------------
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
            double pol2_func = par[3] + par[4]* (x[0]-par[6]) + par[5] * pow((x[0]-par[6]),2);

            return gaus_func + pol2_func;
        }

        static  double bkg_pol2_func(double *x, double *par)
        {
            if (x[0] > (-1 * par[4]) && x[0] < par[5]) {
                TF1::RejectPoint();
                return 0;
            }
            return par[0] + par[1]* (x[0]-par[3]) + par[2] * pow((x[0]-par[3]),2);

            // note : p[0] + p[1]*(x-p[3])+p[2] * (x-p[3])^2, p[4] sets the signal range that should be excluded in the fit
        }

        static  double full_pol2_func(double *x, double *par)
        {
            return par[0] + par[1]* (x[0]-par[3]) + par[2] * pow((x[0]-par[3]),2);

            // note : p[0] + p[1]*(x-p[3])+p[2] * (x-p[3])^2
        }



};

#endif // PREPAREDNDETA_H
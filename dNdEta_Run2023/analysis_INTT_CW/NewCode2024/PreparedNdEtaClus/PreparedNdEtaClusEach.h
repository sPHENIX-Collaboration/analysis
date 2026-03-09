#ifndef PREPAREDNDETACLUSEACH_H
#define PREPAREDNDETACLUSEACH_H

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

class PreparedNdEtaClusEach{
    public:
        PreparedNdEtaClusEach(
            int process_id_in, // note : 1 or 2
            int runnumber_in, // note : still, (54280 or -1)
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,
            std::string output_file_name_suffix_in,

            bool ApplyAlphaCorr_in,
            bool isTypeA_in,
            std::pair<double,double> cut_INTTvtxZ_in,
            int SelectedMbin_in // note : 0, 1, ---- 10, 70, 100 
        );

        std::string GetOutputFileName() {
            return output_filename;
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

        void SetSelectedEtaRange(std::pair<double, double> cut_EtaRange_in) {
            cut_EtaRange_pair = cut_EtaRange_in;
        }


        void PrepareStacks();
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

        // Division:---output root file------------------------------------------------------------------------------------------------
        TFile * file_out;
        std::string output_filename;
        void PrepareOutPutFileName();
        void PrepareOutPutRootFile();

        TCanvas * c1;

        // Division:---analysis------------------------------------------------------------------------------------------------
        std::map<int, int> GetVtxZIndexMap();
        void PrepareHist();
        std::tuple<int, int, int, int, int> GetHistStringInfo(std::string hist_name);
        double get_EvtCount(TH2D * hist_in, int centrality_bin_in);

        std::map<int, int> vtxZ_index_map;
        std::pair<double, double> cut_EtaRange_pair = {std::nan(""), std::nan("")};

        // Division:---histogram------------------------------------------------------------------------------------------------
        std::map<std::string, THStack*> hstack2D_NClusEtaVtxZ_map;
        std::map<std::string, THStack*> hstack1D_NClusEta_map; // note : {inner, outer} x {typeA, All} x {Mbin and vtxZ selected for the stack}
        TH1D * h1D_inner_NClusEta; // note : {typeA, All}
        TH1D * h1D_inner_NClusEtaPerEvt;
        TH1D * h1D_inner_NClusEtaPerEvtPostAC;
        TH1D * h1D_outer_NClusEta;
        TH1D * h1D_outer_NClusEtaPerEvt;
        TH1D * h1D_outer_NClusEtaPerEvtPostAC;

        // note : for the final Truth
        std::map<std::string, THStack*> hstack2D_TrueEtaVtxZ_map;
        std::map<std::string, THStack*> hstack1D_TrueEta_map; // note : for the total count
        std::map<std::string, TH1D*> h1D_TruedNdEta_map;        

        // note : for the alpha correction
        std::map<std::string, TH1D*> h1D_alpha_correction_map_in;
        std::map<std::string, TH1D*> h1D_alpha_correction_map_out;
        std::vector<std::string> alpha_correction_name_map = {
            "h1D_inner_alpha_correction",
            "h1D_outer_alpha_correction"
        };

        // Division:---Constants------------------------------------------------------------------------------------------------
        int Semi_inclusive_Mbin = Constants::Semi_inclusive_bin;

        const std::vector<int> ROOT_color_code = {
            51, 61, 70, 79, 88, 98
        };
};

#endif
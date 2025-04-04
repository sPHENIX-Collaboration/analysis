#ifndef VTXZDIST_H
#define VTXZDIST_H

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

#include "../../Constants.h"
#include "../../EvtVtxZTracklet/structure.h"

class vtxZDist{
    public:
        vtxZDist(
            int process_id_in,
            int runnumber_in,
            int run_nEvents_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,

            std::string output_file_name_suffix_in = "",

            bool Apply_cut_in = false, // note : vtxZQA
            bool ApplyVtxZReWeighting_in = false,
            std::pair<bool, int> ApplyEvtBcoFullDiffCut_in = {true, 61}
        );

        std::string GetOutputFileName() {return output_filename;}
        void SetINTTvtxZReweighting(TH1D * h1D_INTT_vtxZ_reweighting_in) {h1D_INTT_vtxZ_reweighting = (TH1D*)h1D_INTT_vtxZ_reweighting_in->Clone("INTT_vtxZ_reweighting");}
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
        bool Apply_cut;
        bool ApplyVtxZReWeighting;
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
        std::map<std::string, TH1D*> h1D_map;
        std::map<std::string, TH2D*> h2D_map;
        TH1D * h1D_INTT_vtxZ_reweighting = nullptr; // note : should be long range 
        TH1D * h1D_centrality_bin;

        // division : ---For analysis---------------------------------------------------------------------------------------------------------------------------------------------
        


        // division : ---For constants---------------------------------------------------------------------------------------------------------------------------------------------
        std::vector<double> centrality_edges = Constants::centrality_edges;
        int nCentrality_bin;

        int nVtxZ_bin = 120;
        std::pair<double, double> vtxZ_range = {-60, 60};

        int nVtxZ_bin_narrow = 20;
        std::pair<double, double> vtxZ_range_narrow = {-20, 20};

        int HighNClus = 500;

        double cut_GoodRecoVtxZ = Constants::cut_GoodRecoVtxZ; // note : unit [cm]

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

};

#endif
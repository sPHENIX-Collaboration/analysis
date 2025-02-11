#ifndef RESTDIST_H
#define RESTDIST_H

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

class RestDist{
    public:
        RestDist(
            int process_id_in,
            int runnumber_in,
            int run_nEvents_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,

            std::string output_file_name_suffix_in = "",
            std::pair<double, double> vertexXYIncm_in = {std::nan(""),std::nan("")},

            bool Apply_cut_in = false,
            bool ApplyVtxZReWeighting_in = false,
            std::pair<bool, int> ApplyEvtBcoFullDiffCut_in = {true, 61},
            std::pair<bool, std::pair<double,double>> RequireVtxZRange_in = {true, {-10, 10}},
            std::pair<bool, std::pair<double,double>> isClusQA_in = {false, {0, 0}},
            bool isRotated_in = false
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
        std::pair<double, double> vertexXYIncm;
        bool Apply_cut;
        bool ApplyVtxZReWeighting;
        std::pair<bool, int> ApplyEvtBcoFullDiffCut;
        std::pair<bool, std::pair<double,double>> RequireVtxZRange;
        std::pair<bool, std::pair<double,double>> isClusQA;
        bool isRotated;

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
        float MBD_south_charge_sum;
        float MBD_north_charge_sum;
        float MBD_charge_asymm;

        double INTTvtxZ;
        double INTTvtxZError;
        double NgroupTrapezoidal;
        double NgroupCoarse;
        double TrapezoidalFitWidth;
        double TrapezoidalFWHM;

        int NClus;
        int NClus_Layer1;

        std::vector<float> * ClusX;
        std::vector<float> * ClusY;
        std::vector<float> * ClusZ;
        
        std::vector<unsigned int> * ClusAdc;
        std::vector<float> * ClusPhiSize;
        std::vector<float> * ClusZSize;

        std::vector<int> * ClusLayer;
        std::vector<unsigned char> * ClusLadderZId;
        std::vector<unsigned char> * ClusLadderPhiId;

        std::vector<double> * ClusEta_INTTz;
        std::vector<double> * ClusEta_MBDz;
        std::vector<double> * ClusPhi_AvgPV;

        // note : for MC
        std::vector<double> * ClusEta_TrueXYZ;
        std::vector<double> * ClusPhi_TrueXY;


        // note : for data?
        int MBDNSg2;
        int MBDNSg2_vtxZ10cm;
        int MBDNSg2_vtxZ30cm;

        int InttBcoFullDiff_next; // note : for data

        // note : for MC
        int NTruthVtx;
        float TruthPV_trig_x;
        float TruthPV_trig_y;
        float TruthPV_trig_z;


        // note : the flag
        int m_withTrig = false;

        // division : ---For Analysis---------------------------------------------------------------------------------------------------------------------------------------------
        struct clu_info{
            double x;
            double y;
            double z;

            int index;

            int adc;
            int phi_size;
            int sensorZID;
            int ladderPhiID;
            int layerID;

            double eta_INTTz;
            double eta_MBDz;
            double phi_avgXY;

            double eta_TrueXYZ;
            double phi_TrueXY;
        };

        int NClus_inner_typeA;
        int NClus_inner_typeB;
        int NClus_outer_typeA;
        int NClus_outer_typeB;

        void PrepareClusterVec();
        void EvtCleanUp();
        std::vector<clu_info> evt_sPH_inner_nocolumn_vec;
        std::vector<clu_info> evt_sPH_outer_nocolumn_vec;
        std::vector<std::vector<std::pair<bool,clu_info>>> inner_clu_phi_map; // note: phi
        std::vector<std::vector<std::pair<bool,clu_info>>> outer_clu_phi_map; // note: phi

        double shortestDistanceLineToPoint(const std::vector<double> point1, const std::vector<double> point2, const std::vector<double> target);
        void GetRotatedClusterVec(std::vector<RestDist::clu_info> &input_cluster_vec);
        std::pair<double,double> rotatePoint(double x, double y);
        double get_clu_eta(std::vector<double> vertex, std::vector<double> clu_pos);
        double calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY);

        // note : for the best pair quick check
        std::vector<pair_str> temp_all_pair_vec;
        std::vector<double> temp_all_pair_deltaR_vec;
        std::map<int, TH2D*> BestPair_h2D_map;
        std::map<int, TGraph*> BestPair_gr_map;

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

        // division : ---For constants---------------------------------------------------------------------------------------------------------------------------------------------
        const std::vector<int> typeA_sensorZID = {0,2}; // note : sensor Z ID for type A // note -> 1, 0, 2, 3

        int HighNClus = Constants::HighNClus;

        double rotate_phi_angle = 180.;

        std::vector<double> centrality_edges = Constants::centrality_edges;
        int nCentrality_bin;

        int nVtxZ_bin = 60;
        std::pair<double, double> vtxZ_range = {-60, 60};

        int nVtxZ_bin_narrow = 20;
        std::pair<double, double> vtxZ_range_narrow = {-20, 20};

        // note : for deta_phi
        double DeltaPhiEdge_min = -0.07; // note : rad ~ -4 degree
        double DeltaPhiEdge_max = 0.07;  // note : rad ~ 4 degree
        int    nDeltaPhiBin = 140;

        // note : for deta_eta
        double DeltaEtaEdge_min = -1.; // note : rad ~ -4 degree
        double DeltaEtaEdge_max = 1.;  // note : rad ~ 4 degree
        int    nDeltaEtaBin = 100;

        double EtaEdge_min = Constants::EtaEdge_min;
        double EtaEdge_max = Constants::EtaEdge_max;
        int nEtaBin = Constants::nEtaBin;


        std::pair<double, double> cut_vtxZDiff = Constants::cut_vtxZDiff;
        std::pair<double, double> cut_TrapezoidalFitWidth = Constants::cut_TrapezoidalFitWidth;
        std::pair<double, double> cut_TrapezoidalFWHM = Constants::cut_TrapezoidalFWHM;
        std::pair<double, double> cut_INTTvtxZError = Constants::cut_INTTvtxZError;
        std::pair<double, double> cut_GlobalMBDvtxZ = Constants::cut_GlobalMBDvtxZ;
        int cut_InttBcoFullDIff_next = Constants::cut_InttBcoFullDIff_next;
        int Semi_inclusive_bin = Constants::Semi_inclusive_bin;

};

#endif
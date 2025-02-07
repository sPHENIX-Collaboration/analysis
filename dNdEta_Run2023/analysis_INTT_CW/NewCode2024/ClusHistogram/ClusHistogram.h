#ifndef CLUSHISTOGRAM_H
#define CLUSHISTOGRAM_H

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

#include "../EvtVtxZTracklet/structure.h"

#include "../Constants.h"

class ClusHistogram{
    public:
        ClusHistogram(
            int process_id_in,
            int runnumber_in,
            int run_nEvents_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,

            std::string output_file_name_suffix_in,
            std::pair<double, double> vertexXYIncm_in,

            std::pair<bool, TH1D*> vtxZReweight_in,
            bool BcoFullDiffCut_in,
            bool INTT_vtxZ_QA_in,
            std::pair<bool, std::pair<double, double>> isClusQA_in, // note : {adc, phi size}
            bool HaveGeoOffsetTag_in,
            std::pair<bool, int> SetRandomHits_in = {false, 0},
            bool RandInttZ_in = false,
            bool ColMulMask_in = false,
            int c_type_in = 0
        );

        std::string GetOutputFileName() {return output_filename;}
        void SetGeoOffset(std::map<std::string, std::vector<double>> input_geo_offset_map) {
            geo_offset_map.clear();
            geo_offset_map = input_geo_offset_map;

            for (auto &pair : geo_offset_map)
            {
                std::cout<<pair.first<<" : {"<<pair.second[0]<<", "<<pair.second[1]<<", "<<pair.second[2]<<"}"<<std::endl;
            }
        }

        std::string GetGoodColMapName() {return GoodColMap_name;}
        void SetGoodColMap(TH2D * input_h2D_GoodColMap) {
            h2D_GoodColMap = input_h2D_GoodColMap;
        }

        virtual void MainProcess();
        virtual void EndRun();


    protected:
        // note : ----------------- for the constructor -----------------
        int process_id;
        int runnumber;
        int run_nEvents;
        std::string input_directory;
        std::string input_file_name;
        std::string output_directory;

        std::string output_file_name_suffix;
        std::pair<double, double> vertexXYIncm;

        std::pair<bool, TH1D*> vtxZReweight;
        bool BcoFullDiffCut;
        bool INTT_vtxZ_QA;
        std::pair<bool, std::pair<double, double>> isClusQA; // note : {adc, phi size}

        bool HaveGeoOffsetTag;
        std::pair<bool, int> SetRandomHits;
        bool RandInttZ;
        bool ColMulMask;

        int c_type;

        // note : ----------------- for analysis -----------------
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
            // double eta_MBDz;
            // double phi_avgXY;

            // double eta_TrueXYZ;
            // double phi_TrueXY;
        };

        void PrepareClusterVec();
        virtual void EvtCleanUp();
        std::vector<clu_info> evt_sPH_inner_nocolumn_vec;
        std::vector<clu_info> evt_sPH_outer_nocolumn_vec;

        // note : ----------------- for the root file out -----------------
        TFile * file_out;
        TTree * tree_out_par;
        virtual void PrepareOutPutFileName();
        virtual void PrepareOutPutRootFile();
        std::string output_filename;

        // note : ----------------- for the running here -----------------
        double CheckGeoOffsetMap();
        double get_clu_eta(std::vector<double> vertex, std::vector<double> clu_pos);
        void PrepareUniqueClusXYZ();
        std::map<std::string, std::vector<double>> geo_offset_map;
        std::map<std::string, std::tuple<double, double, double, int, int, int, int, int>> inner_UniqueClusXYZ_map; // note : X, Y, Z, sensor ID, layerID, adc, phi size, ladderPhiID
        std::map<std::string, std::tuple<double, double, double, int, int, int, int, int>> outer_UniqueClusXYZ_map; // note : X, Y, Z, sensor ID, layerID, adc, phi size, ladderPhiID
        std::vector<std::string> inner_UniqueClusXYZ_vec; 
        std::vector<std::string> outer_UniqueClusXYZ_vec;
        TH1D * h1D_RandClusZ_ref = nullptr;
        TH2D * h2D_RandClusXY_ref = nullptr;
        TRandom3 * rand3 = nullptr;

        // note : ----------------- for the column multiplicity mask -----------------
        std::string GoodColMap_name = "h2D_MulMap";
        TH2D * h2D_GoodColMap = nullptr;
        TH1D * h1D_GoodColMap_ZId;
        int nZbin = Constants::nZbin;
        double Zmin = Constants::Zmin;
        double Zmax = Constants::Zmax;



        // note : ----------------- for the histograms -----------------
        virtual void PrepareHistograms();
        TH1D * h1D_INTTvtxZ;

        TH1D * h1D_centrality_bin;
        TH1D * h1D_centrality_bin_weighted;
        std::map<std::string,TH1D*> h1D_centrality_map; // note : vtxZ bin

        std::map<std::string,TH1D*> h1D_NClusEta_map; // note : {inner, outer, typeA, all} x {Mbin x vtxZ bin}
        std::map<std::string, TH2D*> h2D_NClusEtaVtxZ_map; // note : {inner, outer, typeA, all} x {Mbin}
        TH2D * h2D_RecoEvtCount_vtxZCentrality;

        TH2D * h2D_InttVtxZ_Centrality;


        std::map<std::string,TH1D*> h1D_TrueEta_map; // note : {Mbin x vtxZ bin}
        std::map<std::string, TH2D*> h2D_TrueEtaVtxZ_map; // note : {Mbin}
        TH2D * h2D_TrueEvtCount_vtxZCentrality;


        TH1D * h1D_INTT_vtxZ_reweighting = nullptr;

        // note : ----------------- for the root file in -----------------
        TFile * file_in;
        TTree * tree_in;
        void PrepareInputRootFile();
        std::map<std::string, int> GetInputTreeBranchesMap(TTree * m_tree_in);

        // note : MBD & centrality relevant
        float MBD_z_vtx;
        bool is_min_bias;
        float MBD_centrality;
        float MBD_south_charge_sum;
        float MBD_north_charge_sum;
        float MBD_charge_sum;
        float MBD_charge_asymm;
        int InttBcoFullDiff_next;

        // note : trigger tag
        int MBDNSg2 = 1;
        int MBDNSg2_vtxZ10cm = 1;
        int MBDNSg2_vtxZ30cm = 1;
        int MBDNSg2_vtxZ60cm = 1;

        std::vector<float> *ClusX;
        std::vector<float> *ClusY;
        std::vector<float> *ClusZ;
        std::vector<int> *ClusLayer;
        std::vector<unsigned char> *ClusLadderZId;
        std::vector<unsigned char> *ClusLadderPhiId;
        std::vector<int> *ClusAdc;
        std::vector<float> *ClusPhiSize;
        std::vector<double> *ClusEta_INTTz;

        // note : INTT vertex Z
        double INTTvtxZ = std::nan("");
        double INTTvtxZError;
        double NgroupTrapezoidal;
        double NgroupCoarse;
        double TrapezoidalFitWidth;
        double TrapezoidalFWHM;

        // note : the tracklet pair
        // std::vector<pair_str> *evt_TrackletPair_vec;
        // std::vector<pair_str> *evt_TrackletPairRotate_vec;

        // note : MC
        float TruthPV_trig_x;
        float TruthPV_trig_y;
        float TruthPV_trig_z;
        int NTruthVtx;
        int NPrimaryG4P;
        std::vector<float> *PrimaryG4P_Pt;
        std::vector<float> *PrimaryG4P_Eta;
        std::vector<float> *PrimaryG4P_Phi;
        std::vector<float> *PrimaryG4P_E;
        std::vector<float> *PrimaryG4P_PID;
        std::vector<int> *PrimaryG4P_isChargeHadron;

        // note : ----------------- for constants -----------------

        // note : for centrality fine
        double CentralityFineEdge_min = -0.5;
        double CentralityFineEdge_max = 100.5;
        int nCentralityFineBin = 101;
        
        // note : for eta (tracklet)
        double EtaEdge_min = -2.7;
        double EtaEdge_max = 2.7;
        int nEtaBin = 27;

        // note : for z
        // double VtxZEdge_min = -45; // note : cm
        // double VtxZEdge_max = 45; // note : cm
        // int nVtxZBin = 18;
        double VtxZEdge_min = -10; // note : cm
        double VtxZEdge_max = 10; // note : cm
        int nVtxZBin = 4;
        int nVtxZBin_FineBin = 500;

        TH1D * h1D_vtxz_template;

        std::vector<int> typeA_sensorZID = {0,2}; // note : sensor Z ID for type A // note -> 1, 0, 2, 3 

        std::pair<double, double> cut_GlobalMBDvtxZ    = Constants::cut_GlobalMBDvtxZ;
        std::pair<double, double> cut_vtxZDiff = Constants::cut_vtxZDiff;
        std::pair<double, double> cut_TrapezoidalFitWidth = Constants::cut_TrapezoidalFitWidth;
        std::pair<double, double> cut_TrapezoidalFWHM = Constants::cut_TrapezoidalFWHM;
        std::pair<double, double> cut_INTTvtxZError = Constants::cut_INTTvtxZError;

        int cut_InttBcoFullDIff_next = Constants::cut_InttBcoFullDIff_next;

        std::vector<double> centrality_edges = Constants::centrality_edges;
        int nCentrality_bin;

        int B0L0_index = Constants::B0L0_index;
        int B0L1_index = Constants::B0L1_index;
        int B1L0_index = Constants::B1L0_index;
        int B1L1_index = Constants::B1L1_index;
        int nLadder_inner = Constants::nLadder_inner;
        int nLadder_outer = Constants::nLadder_outer;

};

#endif
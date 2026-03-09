#ifndef TRACKLETHISTOGRAMFILL_H
#define TRACKLETHISTOGRAMFILL_H

#include <iostream>
#include <vector>
#include <string>
#include <numeric>

#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TMath.h>

#include <TObjArray.h>

#include "../EvtVtxZTracklet/structure.h"
#include "../Constants.h"

class TrackletHistogramFill{
    
    public:
        TrackletHistogramFill(
            int process_id_in,
            int runnumber_in,

            std::string output_directory_in,
            std::string output_file_name_suffix_in
        );

        void SetvtxZReweight(bool tag){ vtxZReweight = tag; }
        void SetBcoFullDiffCut(bool tag){ BcoFullDiffCut = tag; }
        void SetINTT_vtxZ_QA(bool tag){ INTT_vtxZ_QA = tag; }
        void SetWithRotate(bool tag){ isWithRotate = tag; }
        void SetClusQA(std::pair<bool, std::pair<double, double>> pair_in) {isClusQA = pair_in;} // note : {adc, phi size}

        void SetVtxZReweightHist(TH1D * input_hist) {h1D_vtxZReweightFactor = (TH1D*)input_hist->Clone();}

        virtual void PrepareOutPutFileName();
        std::string GetOutputFileName() {return output_filename;}
        virtual void PrepareOutPutRootFile();
        virtual void PrepareHistograms();
        void FillHistogram(
            // note : MBD & centrality relevant
            float MBD_z_vtx,
            int is_min_bias,
            float MBD_centrality,
            float MBD_south_charge_sum,
            float MBD_north_charge_sum,
            // float MBD_charge_sum,
            // float MBD_charge_asymm,
            int InttBcoFullDiff_next,

            // // note : trigger tag
            int MBDNSg2,
            // int MBDNSg2_vtxZ10cm,
            // int MBDNSg2_vtxZ30cm,
            // int MBDNSg2_vtxZ60cm,

            // std::vector<float> *ClusX,
            // std::vector<float> *ClusY,
            // std::vector<float> *ClusZ,
            // std::vector<int> *ClusLayer,
            std::vector<unsigned char> *ClusLadderZId,
            // std::vector<unsigned char> *ClusLadderPhiId,
            std::vector<int> *ClusAdc,
            std::vector<float> *ClusPhiSize,

            // note : INTT vertex Z
            double INTTvtxZ,
            double INTTvtxZError,
            // double NgroupTrapezoidal,
            // double NgroupCoarse,
            double TrapezoidalFitWidth,
            double TrapezoidalFWHM,

            // note : the tracklet pair
            std::vector<pair_str> *evt_TrackletPair_vec,
            std::vector<pair_str> *evt_TrackletPairRotate_vec,

            // note : MC
            // float TruthPV_trig_x,
            // float TruthPV_trig_y,
            float TruthPV_trig_z,
            int NTruthVtx,
            int NPrimaryG4P,
            // std::vector<float> *PrimaryG4P_Pt,
            std::vector<float> *PrimaryG4P_Eta,
            // std::vector<float> *PrimaryG4P_Phi,
            // std::vector<float> *PrimaryG4P_E,
            // std::vector<float> *PrimaryG4P_PID,
            std::vector<int> *PrimaryG4P_isChargeHadron
        );
        void EndRun();

    protected:

        // note : ----------------- for the constructor -----------------
        int process_id;
        int runnumber;
        std::string output_directory;
        std::string output_file_name_suffix;

        bool vtxZReweight;
        bool BcoFullDiffCut;
        bool INTT_vtxZ_QA;
        bool isWithRotate;
        std::pair<bool, std::pair<double, double>> isClusQA;

        // note : ----------------- for the root file out -----------------
        TFile * file_out;
        TTree * tree_out_par;
        std::string output_filename;

        // note : ----------------- for the loop -----------------
        void EvtCleanUp();
        std::map<int,int> Used_Clus_index_map;
        std::vector<double> Pair_DeltaPhi_vec;


        // note : ----------------- for the histogram -----------------
        std::map<std::string,TH1D*> h1D_map;
        std::map<std::string, TH2D*> h2D_map;
        
        TH1D * h1D_PairDeltaEta_inclusive;
        TH1D * h1D_PairDeltaPhi_inclusive;

        // note : ----------------- for the centrality -----------------
        TH1D * h1D_centrality_bin;
        // int convert_centrality_bin (double centrality_in);
        

        // note : ----------------- for the reweights -----------------
        TH1D * h1D_vtxZReweightFactor = nullptr;
        double vtxZReweightFactor;

        // note : ----------------- constants -----------------

        // note : for centrality fine
        double CentralityFineEdge_min = -0.005;
        double CentralityFineEdge_max = 1.005;
        int nCentralityFineBin = 101;
        
        // note : for eta (tracklet)
        double EtaEdge_min = -2.7;
        double EtaEdge_max = 2.7;
        int nEtaBin = 27;
        TH1D * h1D_eta_template;

        // note : for z
        double VtxZEdge_min = -45; // note : cm
        double VtxZEdge_max = 45; // note : cm
        int nVtxZBin = 18;
        TH1D * h1D_vtxz_template;

        // note : for deta_phi
        double DeltaPhiEdge_min = -0.07; // note : rad ~ -4 degree
        double DeltaPhiEdge_max = 0.07;  // note : rad ~ 4 degree
        int    nDeltaPhiBin = 140;

        // note : for deta_eta
        double DeltaEtaEdge_min = -1.; // note : rad ~ -4 degree
        double DeltaEtaEdge_max = 1.;  // note : rad ~ 4 degree
        int    nDeltaEtaBin = 100;

        std::vector<int> typeA_sensorZID = {0,2}; // note : sensor Z ID for type A // note -> 1, 0, 2, 3        

        std::pair<double, double> cut_GlobalMBDvtxZ    = Constants::cut_GlobalMBDvtxZ;
        std::pair<double, double> cut_vtxZDiff = Constants::cut_vtxZDiff;
        std::pair<double, double> cut_TrapezoidalFitWidth = Constants::cut_TrapezoidalFitWidth;
        std::pair<double, double> cut_TrapezoidalFWHM = Constants::cut_TrapezoidalFWHM;
        std::pair<double, double> cut_INTTvtxZError = Constants::cut_INTTvtxZError;

        int cut_InttBcoFullDIff_next = Constants::cut_InttBcoFullDIff_next;

        std::vector<double> centrality_edges = Constants::centrality_edges;
        int nCentrality_bin;

        // note : for the best pair
        std::pair<double, double> cut_bestPair_DeltaPhi = {0,0.017}; // note : rad
        std::pair<double, double> cut_GoodProtoTracklet_DeltaPhi = {-0.017,0.017}; // note : rad
};

#endif // TRACKLETHISTOGRAMFILL_H
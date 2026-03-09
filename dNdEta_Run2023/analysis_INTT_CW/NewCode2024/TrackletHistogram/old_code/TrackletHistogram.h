#ifndef TRACKLETHISTOGRAM_H
#define TRACKLETHISTOGRAM_H

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

#include "TrackletHistogramFill.h"

#include "../Constants.h"

class TrackletHistogram{
    public: 
        TrackletHistogram(
            int process_id_in,
            int runnumber_in,
            int run_nEvents_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,
            std::string output_file_name_suffix_in,

            std::pair<bool, TH1D*> vtxZReweight_in,
            bool BcoFullDiffCut_in,
            bool INTT_vtxZ_QA_in,
            bool isWithRotate_in,
            std::pair<bool, std::pair<double, double>> isClusQA_in // note : {adc, phi size}
        );

        std::string GetOutputFileName() {return tracklet_histogram_fill -> GetOutputFileName();}
        void SetVtxZReweightHist(TH1D * input_hist) {tracklet_histogram_fill -> SetVtxZReweightHist(input_hist);}
        void MainProcess();
        void EndRun();

    protected:
        // note : ----------------- for the constructor -----------------
        int process_id;
        int runnumber;
        int run_nEvents;
        std::string input_directory;
        std::string input_file_name;
        std::string output_directory;
        std::string output_file_name_suffix;

        std::pair<bool, TH1D*> vtxZReweight;
        bool BcoFullDiffCut;
        bool INTT_vtxZ_QA;
        bool isWithRotate;
        std::pair<bool, std::pair<double, double>> isClusQA; // note : {adc, phi size}

        // note : ----------------- for the root file out -----------------
        TrackletHistogramFill * tracklet_histogram_fill;

        // note : ----------------- for the root file in -----------------
        TFile * file_in;
        TTree * tree_in;
        void PrepareInputRootFile();
        std::map<std::string, int> GetInputTreeBranchesMap(TTree * m_tree_in);

        // int event;

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
        int MBDNSg2;
        int MBDNSg2_vtxZ10cm;
        int MBDNSg2_vtxZ30cm;
        int MBDNSg2_vtxZ60cm;

        std::vector<float> *ClusX;
        std::vector<float> *ClusY;
        std::vector<float> *ClusZ;
        std::vector<int> *ClusLayer;
        std::vector<unsigned char> *ClusLadderZId;
        std::vector<unsigned char> *ClusLadderPhiId;
        std::vector<int> *ClusAdc;
        std::vector<float> *ClusPhiSize;

        // note : INTT vertex Z
        double INTTvtxZ;
        double INTTvtxZError;
        double NgroupTrapezoidal;
        double NgroupCoarse;
        double TrapezoidalFitWidth;
        double TrapezoidalFWHM;

        // note : the tracklet pair
        std::vector<pair_str> *evt_TrackletPair_vec;
        std::vector<pair_str> *evt_TrackletPairRotate_vec;

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
};

#endif
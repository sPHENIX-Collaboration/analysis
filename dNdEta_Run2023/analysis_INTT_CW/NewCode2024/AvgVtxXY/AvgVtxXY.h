#ifndef AVGVTXXY_H
#define AVGVTXXY_H

#include <iostream>
#include <vector>
#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TMath.h>
#include <TF1.h>
#include <TProfile.h>

#include <TCanvas.h> // note : for the combined case
#include <TGraph.h>  // note : for the combined case

#include <TRandom.h> // note : for the offset
#include <TRandom3.h> // note : for the offset



class AvgVtxXY {
    public: 
        AvgVtxXY(
            int process_id_in,
            int runnumber_in,
            int run_nEvents_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,
            std::string output_file_name_suffix_in,

            // note : for the event selection
            std::pair<double,double> MBD_vtxZ_cut_in,
            std::pair<int,int> INTTNClus_cut_in,
            int ClusAdc_cut_in,
            int ClusPhiSize_cut_in,

            bool HaveGeoOffsetTag_in = false,
            double random_range_XYZ_in = 0.02, // note : unit : cm // note : for the generation of the offset
            int random_seed_in = -999,  // note : for the generation of the offset
            std::string input_offset_map_in = "no_map" // note : full map
        );

        std::map<std::string, std::vector<double>> GetGeoOffsetMap(){
            return geo_offset_map;
        };
        void PreparePairs();
        void FindVertexQuadrant(int nIteration, double search_Window_half, std::pair<double,double> set_center);
        std::pair<double,double> GetVertexQuadrant(){ 
            return std::make_pair(out_vtxX_quadrant, out_vtxY_quadrant); 
        };
        void FindVertexLineFill(std::pair<double,double> set_center, int Nbins = 100, double search_Window_half = 0.25 /* unit : cm*/, double segmentation = 0.0001 /*unit: cm*/);
        void EndRun();

        std::pair<double,double> GetFinalAvgVtxXY(){
            return std::make_pair((out_vtxX_quadrant + out_vtxX_LineFill) / 2., (out_vtxY_quadrant + out_vtxY_LineFill) / 2.);
        };

        std::string GetOutputFileName() {return output_filename;}

    protected:
        // note : --------------------- the structure --------------------- 
        struct pos_xy{
            double x;
            double y;
        };

        void ReadRootFile();
        void InitOutRootFile();        

        // note : --------------------- for the constructor ---------------------
        int process_id;
        int runnumber;
        int run_nEvents;
        std::string input_directory;
        std::string input_file_name;
        std::string output_directory;
        std::string output_file_name_suffix;
        std::pair<double,double> MBD_vtxZ_cut;
        std::pair<int,int> INTTNClus_cut;
        int ClusAdc_cut;
        int ClusPhiSize_cut;
        bool HaveGeoOffsetTag;
        double random_range_XYZ;
        int random_seed;
        std::string input_offset_map;

        // note : --------------------- for reading the file ---------------------
        TFile * file_in;
        TTree * tree_in;
        std::string input_tree_name = "EventTree";
        float MBD_z_vtx;
        bool is_min_bias;
        float MBD_centrality;
        bool InttBco_IsToBeRemoved;

        int NClus;
        std::vector<float> *ClusX;
        std::vector<float> *ClusY;
        std::vector<int> *ClusLayer;
        std::vector<unsigned char> *ClusLadderPhiId;
        std::vector<int> *ClusAdc;
        std::vector<float> *ClusPhiSize;

        // note : --------------------- for output file ---------------------
        TFile * file_out;
        std::string output_filename;
        
        TTree * tree_vtxXY;
        double out_vtxX_quadrant = std::nan("");
        double out_vtxXE_quadrant = std::nan("");
        double out_vtxY_quadrant = std::nan("");
        double out_vtxYE_quadrant = std::nan("");
        double out_vtxX_LineFill = std::nan("");
        double out_vtxXE_LineFill = std::nan("");
        double out_vtxXStdDev_LineFill = std::nan("");
        double out_vtxY_LineFill = std::nan("");
        double out_vtxYE_LineFill = std::nan("");
        double out_vtxYStdDev_LineFill = std::nan("");
        // int out_run_nEvents;
        int out_job_index;
        int out_file_total_event;

        TTree * tree_geooffset;
        int out_LayerID;
        int out_LadderPhiID;
        double out_offset_x;
        double out_offset_y;
        double out_offset_z;

        TTree * tree_quadrant_detail;
        int out_iteration;
        int out_quadrant;
        double out_assume_center_x;
        double out_assume_center_y;
        double out_Fit_InnerPhi_DeltaPhi_RChi2;
        double out_Fit_InnerPhi_DeltaPhi_Err0;
        double out_Fit_InnerPhi_DeltaPhi_LineY;
        double out_Fit_InnerPhi_DCA_RChi2;
        double out_Fit_InnerPhi_DCA_Err0;
        double out_Fit_InnerPhi_DCA_LineY;

        TTree * tree_parameters;
        int out_run_nEvents;
        double out_MBD_vtxZ_cut_l;
        double out_MBD_vtxZ_cut_r;
        int out_INTTNClus_cut_l;
        int out_INTTNClus_cut_r;
        int out_ClusAdc_cut;
        int out_ClusPhiSize_cut;
        int out_HaveGeoOffsetTag;
        double out_random_range_XYZ;
        int out_random_seed;
        int out_input_offset_map;
        double out_hopeless_pair_angle_cut;
        double out_rough_DeltaPhi_cut_degree;
        double out_InnerOuterPhi_DeltaPhi_DCA_Threshold;
        double out_LineFill_Hist_Threshold;
        double out_InnerPhi_DeltaPhi_DCA_Fit_Tgr_HitContent_Threshold;
        int out_TH2D_threshold_advanced_2_CheckNBins;

        // note : --------------------- for the geo offset ---------------------
        TRandom3 * geo_rand;
        std::map<std::string, std::vector<double>> geo_offset_map;
        std::vector<double> offset_correction(std::map<std::string,std::vector<double>> input_map);
        void GenGeoOffset(); // note : gen, save them into a map, and also in the tree
        void SetGeoOffset(); // note : read the map
        void geo_offset_correction_fill();

        // note : --------------------- for preapre the pairs ---------------------
        std::vector<pos_xy> inner_cluster_vec;
        std::vector<pos_xy> outer_cluster_vec;
        std::vector<std::pair<pos_xy,pos_xy>> cluster_pair_vec;
        double hopeless_pair_angle_cut;
        
        // note : --------------------- for the common ---------------------
        void TH2D_threshold_advanced_2(TH2D * hist, double threshold);
        int TH2D_threshold_advanced_2_CheckNBins;

        // note : --------------------- run the iteration ---------------------
        double calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY);
        void SetUpHistograms(int iteration, int quadrant);
        std::pair<double,double> RunSingleCandidate(int iteration, int quadrant, std::pair<double,double> assume_center);
        std::vector<double> SumTH2FColumnContent(TH2D * hist_in);
        std::vector<std::pair<double,double>> Get4vtx(std::pair<double,double> origin, double length);
        std::map<std::string, TGraph*> gr_map; // note : for each iteration
        std::map<std::string, TProfile*> profile_map; // note : for each iteration
        TF1 * fit_innerphi_DeltaPhi;
        TF1 * fit_innerphi_DCA;
        double InnerOuterPhi_DeltaPhi_DCA_Threshold;
        double InnerPhi_DeltaPhi_DCA_Fit_Tgr_HitContent_Threshold;

        // note : --------------------- for the histograms ---------------------
        std::map<std::string, TH2D*> h2D_map; // note : for each iteration
        std::map<std::string, TH1D*> h1D_map; // note : for each iteration

        TH2D * final_InnerPhi_DeltaPhi;
        TH2D * final_InnerPhi_DCA;
        TH2D * final_InnerPhi_DeltaPhi_bkgrm;
        TH2D * final_InnerPhi_DCA_bkgrm;

        TH2D * final_OuterPhi_DeltaPhi;
        TH2D * final_OuterPhi_DCA;
        TH2D * final_OuterPhi_DeltaPhi_bkgrm;
        TH2D * final_OuterPhi_DCA_bkgrm;

        TH2D * final_DeltaPhi_DCA;
        TH1D * final_DeltaPhi_1D;
        TH1D * final_DCA_1D;

        TH2D * final_LineFill_BeamSpot;
        TH2D * final_LineFill_BeamSpot_bkgrm;

        // note : --------------------- for the line filled method ---------------------
        double rough_DeltaPhi_cut_degree;
        void TH2DSampleLineFill(TH2D * hist_in, double segmentation, std::pair<double,double> inner_clu, std::pair<double,double> outer_clu);
        std::vector<double> calculateDistanceAndClosestPoint(double x1, double y1, double x2, double y2, double target_x, double target_y);
        double LineFill_Hist_Threshold;

        // note : --------------------- constants ---------------------
        const int h2_nbins = 100;

        const double Hist2D_angle_xmin = -M_PI;
        const double Hist2D_angle_xmax =  M_PI;

        const double Hist2D_DeltaPhi_min = -0.03;
        const double Hist2D_DeltaPhi_max =  0.03;

        const double Hist2D_DCA_min = -1.0;
        const double Hist2D_DCA_max =  1.0;

        const int B0L0_index = 3;
        const int B0L1_index = 4;
        const int B1L0_index = 5;
        const int B1L1_index = 6;
        const int nLadder_inner = 12;
        const int nLadder_outer = 16;

        
};



#endif
#ifndef EVTVTXZPROTOTRACKLET_H
#define EVTVTXZPROTOTRACKLET_H

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

#include "structure.h"

class EvtVtxZProtoTracklet{
    public:
        EvtVtxZProtoTracklet(
            int process_id_in,
            int runnumber_in,
            int run_nEvents_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,
            std::string output_file_name_suffix_in,

            std::pair<double, double> vertexXYIncm_in = {0, 0}, // note : in cm
            bool IsFieldOn_in = false,
            bool IsDCACutApplied_in = true,
            std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree_in = {{-1,1},{-1000.,1000.}}, // note : in degree
            std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm_in = {{-1,1},{-1000.,1000.}}, // note : in cm
            int ClusAdcCut_in = 35,
            int ClusPhiSizeCut_in = 8,
            
            bool PrintRecoDetails_in = false,
            bool DrawEvtVtxZ_in = true,

            bool RunInttBcoFullDiff_in = true,
            bool RunVtxZReco_in = true,
            bool RunTrackletPair_in = true,
            bool RunTrackletPairRotate_in = true,
            
            bool HaveGeoOffsetTag_in = false
        );

        std::string GetOutputFileName() {return output_filename;}
        void SetGeoOffset(std::map<std::string, std::vector<double>> input_geo_offset_map);
        void MainProcess();
        void EndRun(int close_file_in = 1);

    protected:
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
        };

    // note : ---------------------- For the constructor ----------------------
    int process_id;
    int runnumber;
    int run_nEvents;
    std::string input_directory;
    std::string input_file_name;
    std::string output_directory;
    std::string output_file_name_suffix;

    std::pair<double, double> vertexXYIncm;
    bool IsFieldOn;
    bool IsDCACutApplied;
    std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree;
    std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm;
    int ClusAdcCut;
    int ClusPhiSizeCut;
    bool PrintRecoDetails;
    bool DrawEvtVtxZ;
    bool RunInttBcoFullDiff;
    bool RunVtxZReco;
    bool RunTrackletPair;
    bool RunTrackletPairRotate;
    bool HaveGeoOffsetTag;


    // note : ---------------------- For read & outputTFile ----------------------
    TFile * file_in;
    TTree * tree_in;
    TFile * file_out;
    TTree * tree_out;

    void PrepareOutPutFileName();
    std::string output_filename;
    void PrepareRootFile();
    std::map<std::string, int> GetInputTreeBranches(TTree * m_tree_in);

    // int event;
    float MBD_z_vtx;
    ULong_t INTT_BCO;
    std::vector<float> *ClusX;
    std::vector<float> *ClusY;
    std::vector<float> *ClusZ;
    std::vector<int> *ClusLayer;
    std::vector<unsigned char> *ClusLadderZId;
    std::vector<unsigned char> *ClusLadderPhiId;
    std::vector<int> *ClusAdc;
    std::vector<float> *ClusPhiSize;
    std::vector<int> *firedTriggers;

    float TruthPV_trig_x;
    float TruthPV_trig_y;
    float TruthPV_trig_z;

    double INTTvtxZ;
    double INTTvtxZError;

    // note : the flag
    int m_withTrig = false;

    // note : ---------------------- For update tree vertex Z ----------------------
    double out_INTTvtxZ;
    double out_INTTvtxZError;
    double out_NgroupTrapezoidal;
    double out_NgroupCoarse;
    double out_TrapezoidalFitWidth;
    double out_TrapezoidalFWHM;

    TBranch * b_INTTvtxZ;
    TBranch * b_INTTvtxZError;
    TBranch * b_NgroupTrapezoidal;
    TBranch * b_NgroupCoarse;
    TBranch * b_TrapezoidalFitWidth;
    TBranch * b_TrapezoidalFWHM;

    // note : ---------------------- For the cluster eta and phi ----------------------
    std::vector<double> out_ClusEta_INTTz;
    std::vector<double> out_ClusEta_MBDz;
    std::vector<double> out_ClusEta_TrueXYZ;
    std::vector<double> out_ClusPhi_AvgPV;
    std::vector<double> out_ClusPhi_TrueXY;

    TBranch * b_ClusEta_INTTz;
    TBranch * b_ClusEta_MBDz;
    TBranch * b_ClusEta_TrueXYZ;
    
    TBranch * b_ClusPhi_AvgPV;
    TBranch * b_ClusPhi_TrueXY;

    void GetClusUpdated();

    // note : ---------------------- For InttBcoFullDiff w.r.t next ----------------------
    int out_InttBcoFullDiff_next;
    TBranch * b_InttBcoFullDiff_next;

    // note : ---------------------- For common ----------------------
    double CheckGeoOffsetMap();
    void PrepareClusterVec();
    void EvtCleanUp();
    std::vector<clu_info> evt_sPH_inner_nocolumn_vec;
    std::vector<clu_info> evt_sPH_outer_nocolumn_vec;
    std::vector<std::vector<std::pair<bool,clu_info>>> inner_clu_phi_map; // note: phi
    std::vector<std::vector<std::pair<bool,clu_info>>> outer_clu_phi_map; // note: phi
    std::map<std::string, std::vector<double>> geo_offset_map;
    double temp_INTTvtxZ;
    double temp_INTTvtxZError;

    int out_MBDNSg2;
    int out_MBDNSg2_vtxZ10cm;
    int out_MBDNSg2_vtxZ30cm;
    int out_MBDNSg2_vtxZ60cm;
    int out_eID_count;

    TBranch * b_MBDNSg2;
    TBranch * b_MBDNSg2_vtxZ10cm;
    TBranch * b_MBDNSg2_vtxZ30cm;
    TBranch * b_MBDNSg2_vtxZ60cm;
    TBranch * b_eID_count;

    int index_MBDNSg2 = 10;
    int index_MBDNSg2_vtxZ10cm = 12;
    int index_MBDNSg2_vtxZ30cm = 13;
    int index_MBDNSg2_vtxZ60cm = 14;

    void GetTriggerInfo();

    // note : ---------------------- For vertex Z calculation ----------------------
    void PrepareINTTvtxZ();
    void GetINTTvtxZ();

    double get_radius(double x, double y);
    double get_delta_phi(double angle_1, double angle_2);
    double calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY);
    double Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y);
    std::pair<double,double> Get_possible_zvtx(double rvtx, std::vector<double> p0, std::vector<double> p1);
    void line_breakdown(TH1D* hist_in, std::pair<double,double> line_range);
    void trapezoidal_line_breakdown(TH1D * hist_in, double inner_r, double inner_z, int inner_zid, double outer_r, double outer_z, int outer_zid);
    std::vector<double> find_Ngroup(TH1D * hist_in);
    double vector_average (std::vector <double> input_vector);
    double vector_stddev (std::vector <double> input_vector);

    static double gaus_func(double *x, double *par)
    {
        // note : par[0] : size
        // note : par[1] : mean
        // note : par[2] : width
        // note : par[3] : offset 
        return par[0] * TMath::Gaus(x[0],par[1],par[2]) + par[3];
    };

    // note : objects for the vertex Z reco
    TH1D * evt_possible_z;
    TH1D * line_breakdown_hist;
    TH1D * combination_zvtx_range_shape;
    std::vector<clu_info> evt_sPH_inner_nocolumn_vec_PostCut;
    std::vector<clu_info> evt_sPH_outer_nocolumn_vec_PostCut;
    std::vector<std::vector<std::pair<bool,clu_info>>> inner_clu_phi_map_PostCut; // note: phi
    std::vector<std::vector<std::pair<bool,clu_info>>> outer_clu_phi_map_PostCut; // note: phi
    // std::pair<double, double> m_vertexXYInmm;
    // std::pair<std::pair<double,double>,std::pair<double,double>> m_DCAcutInmm;
    
    TF1 * gaus_fit; // note : cm
    std::vector<TF1 *> gaus_fit_vec; // note : cm
    std::vector<double> fit_mean_mean_vec;
    std::vector<double> fit_mean_reducedChi2_vec;
    std::vector<double> fit_mean_width_vec;

    // note : ---------------------- For DrawEvtVtxZ ----------------------
    TFile * INTTvtxZ_EvtDisplay_file_out = nullptr; 
    TH1D * line_breakdown_hist_zoomin;
    TCanvas * c1;
    TPad * pad_EvtZDist;
    TPad * pad_ZoomIn_EvtZDist;
    TLatex * draw_text;
    void PrepareEvtCanvas();
    void Characterize_Pad(TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0);

    // note : ---------------------- For update tree tracklet pair ----------------------
    std::vector<pair_str> out_evt_TrackletPair_vec;
    std::vector<pair_str> out_evt_TrackletPairRotate_vec;    

    TBranch * b_evt_TrackletPair_vec;
    TBranch * b_evt_TrackletPairRotate_vec;

    double rotate_phi_angle = 180.; // note : unit : degree

    TGraphErrors * track_gr;
    TF1 * fit_rz;

    void GetTrackletPair(std::vector<pair_str> &input_TrackletPair_vec, bool isRotated);
    std::vector<clu_info> GetRoatedClusterVec(std::vector<clu_info> input_cluster_vec);
    std::pair<double,double> rotatePoint(double x, double y);
    double get_clu_eta(std::vector<double> vertex, std::vector<double> clu_pos);
    std::pair<double,double> Get_eta(std::vector<double>p0, std::vector<double>p1, std::vector<double>p2);
    double grEY_stddev(TGraphErrors * input_grr);
    std::pair<double, double> mirrorPolynomial(double a, double b);

    // note : ---------------------- For constants ----------------------
    const int B0L0_index = 3;
    const int B0L1_index = 4;
    const int B1L0_index = 5;
    const int B1L1_index = 6;
    const int nLadder_inner = 12;
    const int nLadder_outer = 16;

    const int zvtx_hist_Nbins = 1200;   // note : N bins for each side, regardless the bin at zero 
    const double fine_bin_width = 0.05;  // note : bin width with the unit [cm]
    const std::pair<double,double> evt_possible_z_range = {-70, 70}; // note : [cm]
    const std::pair<double,double> edge_rejection = {-50, 50}; // note : [cm]
    const double typeA_sensor_half_length_incm = 0.8; // note : [cm]
    const double typeB_sensor_half_length_incm = 1.0; // note : [cm] 
    const std::vector<int> typeA_sensorZID = {0,2}; // note : sensor Z ID for type A // note -> 1, 0, 2, 3
    const std::vector<std::string> color_code = {
        "#9e0142",
        "#d53e4f",
        "#f46d43",
        "#fdae61",
        "#fee08b",
        "#e6f598",
        "#abdda4",
        "#66c2a5",
        "#3288bd",
        "#5e4fa2" 
    };

    const std::vector<int> ROOT_color_code = {
        51, 61, 70, 79, 88, 98, 100, 113, 120, 129
    };
    
    // note : constants for the INTT vtxZ
    const int number_of_gaus = 7;

};

#endif
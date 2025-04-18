#ifndef TRACKLETHISTOGRAMNEW_H
#define TRACKLETHISTOGRAMNEW_H


#include "../ClusHistogram/ClusHistogram.h"

#include "../EvtVtxZTracklet/structure.h"

#include "../Constants.h"

class TrackletHistogramNew : public ClusHistogram{
    public:
        TrackletHistogramNew(
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
            bool ColMulMask_in = true
        );

        void MainProcess() override; 
        void EndRun() override;

    protected:
        void EvtCleanUp() override;
        void PrepareOutPutFileName() override;
        void PrepareOutPutRootFile() override;
        void PrepareHistograms() override;

        // note : ----------------- for the histogram -----------------
        std::map<std::string,TH1D*> h1D_map;
        std::map<std::string, TH2D*> h2D_map;
        
        TH1D * h1D_PairDeltaEta_inclusive;
        TH1D * h1D_PairDeltaPhi_inclusive;

        TH1D * h1D_eta_template;

        TH1D * h1D_Inner_ClusEta_INTTz;
        TH1D * h1D_Outer_ClusEta_INTTz;

        TH1D * h1D_INTTvtxZ_FineBin;
        TH1D * h1D_INTTvtxZ_FineBin_NoVtxZWeight;
        TH2D * h2D_INTTvtxZFineBin_CentralityBin;

        // note : ----------------- for the analysis -----------------
        std::vector<pair_str> evt_TrackletPair_vec;
        std::vector<pair_str> evt_TrackletPairRotate_vec; 
        std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>> inner_clu_phi_map;
        std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>> outer_clu_phi_map;

        void GetTrackletPair(std::vector<pair_str> &input_TrackletPair_vec, bool isRotated);
        void FillPairs(std::vector<pair_str> input_TrackletPair_vec, bool isRotated, int Mbin_in, int vtxz_bin_in, double vtxZ_weight_in, int eID_in);

        std::pair<double,double> Get_possible_zvtx(double rvtx, std::vector<double> p0, std::vector<double> p1); // note : inner p0, outer p1, vector {r,z, zid}, -> {y,x}
        double get_delta_phi(double angle_1, double angle_2);
        double get_radius(double x, double y);
        std::pair<double,double> Get_eta(std::vector<double>p0, std::vector<double>p1, std::vector<double>p2);
        double grEY_stddev(TGraphErrors * input_grr);
        std::pair<double, double> mirrorPolynomial(double a, double b);
        std::pair<double, double> rotatePoint(double x, double y);
        std::vector<ClusHistogram::clu_info> GetRotatedClusterVec(std::vector<ClusHistogram::clu_info> input_cluster_vec);
        double Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y); // note : x : z, y : r
        
        TGraphErrors * track_gr;
        TF1 * fit_rz;
        std::vector<double> Pair_DeltaPhi_vec;
        std::vector<int> Used_Clus_index_vec;


        // note : ----------------- for constants -----------------

        double rotate_phi_angle = 180.;

        // note : for deta_phi
        double DeltaPhiEdge_min = -0.07; // note : rad ~ -4 degree
        double DeltaPhiEdge_max = 0.07;  // note : rad ~ 4 degree
        int    nDeltaPhiBin = 140;

        // note : for deta_eta
        double DeltaEtaEdge_min = -1.; // note : rad ~ -4 degree
        double DeltaEtaEdge_max = 1.;  // note : rad ~ 4 degree
        int    nDeltaEtaBin = 100;

        // note : for the best pair
        std::pair<double, double> cut_bestPair_DeltaPhi = {0,0.04}; // note : rad
        std::pair<double, double> cut_GoodProtoTracklet_DeltaPhi = {-0.04,0.04}; // note : rad

        double typeA_sensor_half_length_incm = Constants::typeA_sensor_half_length_incm;
        double typeB_sensor_half_length_incm = Constants::typeB_sensor_half_length_incm;
};

#endif
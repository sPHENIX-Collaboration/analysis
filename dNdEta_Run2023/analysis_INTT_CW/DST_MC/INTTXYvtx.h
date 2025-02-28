#ifndef INTTXYvtx_h
#define INTTXYvtx_h

#include <filesystem>
#include "../private_cluster_gen/InttConversion_new.h"
#include "../private_cluster_gen/InttClustering.h"
#include "../sigmaEff.h"
#include "../sPhenixStyle.C"
#include "gaus_func.h"
#include "ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

// note : this class mainly focus on two things 
// note : 1. find a single vertex for the whole run
// note :     a. the functions prepared for this purpose are : ProcessEvt(), MacroVTXxyCorrection(), MacroVTXxyCorrection_new(), GetFinalVTXxy(), MacrovtxSquare()
// note : 2. find the vertex for each event
// note :     a. the functions prepared for this purpose are : ProcessEvt()

struct type_pos{
    double x;
    double y;
};

double cos_func(double *x, double *par)
{
    return -1 * par[0] * cos(par[1] * (x[0] - par[2])) + par[3];
}

// double gaus_func(double *x, double *par)
// {
//     // note : par[0] : size
//     // note : par[1] : mean
//     // note : par[2] : width
//     // note : par[3] : offset 
//     return par[0] * TMath::Gaus(x[0],par[1],par[2]) + par[3];
// }

class INTTXYvtx {
    public:
        INTTXYvtx(string run_type, string out_folder_directory, pair<double,double> beam_origin, int geo_mode_id, double phi_diff_cut = 0.11, pair<double, double> DCA_cut = {-1,1}, int N_clu_cutl = 20, int N_clu_cut = 10000, bool draw_event_display = true, double peek = 3.32405, double angle_diff_new_l = 0.0, double angle_diff_new_r = 3, bool print_message_opt = true);
        INTTXYvtx(string run_type, string out_folder_directory);
        void ProcessEvt(
            int event_i, 
            vector<clu_info> temp_sPH_inner_nocolumn_vec, 
            vector<clu_info> temp_sPH_outer_nocolumn_vec, 
            vector<vector<double>> temp_sPH_nocolumn_vec, 
            vector<vector<double>> temp_sPH_nocolumn_rz_vec, 
            int NvtxMC, 
            double TrigZvtxMC, 
            bool PhiCheckTag, 
            Long64_t bco_full,
            int is_min_bias, // note : for data and MC
            int is_min_bias_wozdc // note : for data only 
        );
        virtual void ClearEvt();
        virtual void PrintPlots();
        virtual void EndRun();
        
        unsigned long GetVecNele();
        void MacroVTXxyCorrection(bool phi_correction, bool calculate_XY, int N_trial, double fit_range_l, double fit_range_r);
        void MacroVTXxyCorrection_new(double fit_range_l, double fit_range_r, int N_trial, pair<double,double> vertex_in=make_pair(-999,-999));
        virtual vector<pair<double,double>> MacroVTXSquare(double length, int N_trial, bool draw_plot_opt = true);
        pair<double,double> GetFinalVTXxy();
        pair<vector<TH2F *>, vector<TH1F*>> GetHistFinal();
        void TH1F_FakeClone(TH1F*hist_in, TH1F*hist_out);
        void TH2F_FakeClone(TH2F*hist_in, TH2F*hist_out);
        void TH2F_FakeRebin(TH2F*hist_in, TH2F*hist_out);
        vector<pair<double,double>> FillLine_FindVertex(pair<double,double> window_center, double segmentation = 0.005, double window_width = 5.0, int N_bins = 100, bool draw_plot = true);
        vector<double> LineFill_bkgrm_Get_covariance() {return Get_covariance_TH2(xy_hist_bkgrm);};

        // note : for the delta phi study
        void RunDeltaPhiStudy();
        TProfile * Get_final_angle_diff_inner_phi_degree_coarseX_peak_profile() {return final_angle_diff_inner_phi_degree_coarseX_peak_profile;};
        TH1F * Get_final_angle_diff_inner_phi_degree_coarseX_peak_profile_hist() {
            
            TH1F * angle_diff_correction_degree_hist = new TH1F( 
                "",
                "",
                final_angle_diff_inner_phi_degree_coarseX_peak_profile -> GetNbinsX(), 
                final_angle_diff_inner_phi_degree_coarseX_peak_profile -> GetXaxis() -> GetXmin(), 
                final_angle_diff_inner_phi_degree_coarseX_peak_profile -> GetXaxis() -> GetXmax() 
            );
            
            for (int i = 0; i < final_angle_diff_inner_phi_degree_coarseX_peak_profile -> GetNbinsX(); i++) { 
                angle_diff_correction_degree_hist -> SetBinContent(i+1, final_angle_diff_inner_phi_degree_coarseX_peak_profile -> GetBinContent(i+1));
            }

            return angle_diff_correction_degree_hist;
        }
        TH1F * Get_final_DCA_mm_inner_phi_degree_coarseX_peak_profile_hist() {
            
            TH1F * DCA_mm_correction_degree_hist = new TH1F( 
                "",
                "",
                final_DCA_mm_inner_phi_degree_coarseX_peak_profile -> GetNbinsX(), 
                final_DCA_mm_inner_phi_degree_coarseX_peak_profile -> GetXaxis() -> GetXmin(), 
                final_DCA_mm_inner_phi_degree_coarseX_peak_profile -> GetXaxis() -> GetXmax() 
            );
            
            for (int i = 0; i < final_DCA_mm_inner_phi_degree_coarseX_peak_profile -> GetNbinsX(); i++) { 
                DCA_mm_correction_degree_hist -> SetBinContent(i+1, final_DCA_mm_inner_phi_degree_coarseX_peak_profile -> GetBinContent(i+1));
            }

            return DCA_mm_correction_degree_hist;
        }

    protected:
        TCanvas * c1;
        TLatex * ltx;
        TLatex * draw_text;
        TF1 * cos_fit;
        TF1 * gaus_fit;
        TF1 * gaus_fit_angle_diff;
        TF1 * gaus_fit_MC;
        TF1 * correlation_Line;
        TF1 * bkg_fit_pol2;
        TF1 * draw_pol2_line;
        TF1 * correlation_fit_MC;
        TF1 * gaus_pol2_fit;

        TF1 * horizontal_fit_inner;
        TF1 * horizontal_fit_angle_diff_inner;
        TF1 * horizontal_fit_outer;
        TF1 * horizontal_fit_angle_diff_outer;
        TF1 * fit_rz;
        TF1 * gaus_pol1_fit;
        TF1 * draw_gaus_line;
        TF1 * draw_pol1_line;
        TF1 * d_gaus_pol1_fit;
        TF1 * draw_d_gaus;
        // TF1 * track_pol1_fit;

        TLine * track_line;
        TLine * coord_line;
        TLine * ladder_line;

        TH2F * angle_correlation;
        TH2F * angle_diff_DCA_dist;
        TH1F * angle_diff;
        TH1F * angle_diff_new;
        TH1F * angle_diff_new_bkg_remove;
        TH1F * angle_diff_new_bkg_remove_final;
        TH1F * angle_diff_radian;
        TH2F * inner_pos_xy;
        TH2F * outer_pos_xy;
        TH2F * inner_outer_pos_xy;
        TH2F * DCA_point;
        TH1F * DCA_distance;
        TH2F * N_cluster_correlation;
        TH2F * N_cluster_correlation_close;
        
        TH2F * DCA_distance_inner_X;
        TH2F * DCA_distance_inner_Y;
        TH2F * DCA_distance_outer_X;
        TH2F * DCA_distance_outer_Y;

        TH2F *     DCA_distance_inner_phi;
        TH2F *     DCA_distance_inner_phi_peak;
        TProfile * DCA_distance_inner_phi_peak_profile;
        TH2F *     DCA_distance_outer_phi;
        TH2F *     DCA_distance_outer_phi_peak;
        TProfile * DCA_distance_outer_phi_peak_profile;

        TH2F *     angle_diff_inner_phi;
        TH2F *     angle_diff_inner_phi_peak;
        TProfile * angle_diff_inner_phi_peak_profile;
        TH2F *     angle_diff_outer_phi;
        TH2F *     angle_diff_outer_phi_peak;
        TProfile * angle_diff_outer_phi_peak_profile;

        // note : those are for shifting the peak to the center 
        TH2F *     final_DCA_mm_inner_phi_degree_coarseX;
        TH2F *     final_DCA_mm_inner_phi_degree_coarseX_peak;
        TProfile * final_DCA_mm_inner_phi_degree_coarseX_peak_profile;
        
        TH2F *     final_DCA_cm_inner_phi_radian_coarseX;
        TH2F *     final_DCA_cm_inner_phi_radian_coarseX_peak;
        TProfile * final_DCA_cm_inner_phi_radian_coarseX_peak_profile;

        TH2F *     final_angle_diff_inner_phi_radian_coarseX;
        TH2F *     final_angle_diff_inner_phi_radian_coarseX_peak;
        TProfile * final_angle_diff_inner_phi_radian_coarseX_peak_profile;
        TH2F *     final_angle_diff_inner_phi_degree_coarseX;
        TH2F *     final_angle_diff_inner_phi_degree_coarseX_peak;
        TProfile * final_angle_diff_inner_phi_degree_coarseX_peak_profile;
        TH1F *     final_angle_diff_radian_before;
        TH1F *     final_angle_diff_radian_post;
        TH2F *     final_angle_diff_radian_DCA_2D_before;
        TH2F *     final_angle_diff_radian_DCA_2D_post;
        TH1F *     final_DCA_cm_before;
        TH1F *     final_DCA_cm_post;

        TGraph * angle_diff_inner_phi_peak_profile_graph;
        TGraph * angle_diff_outer_phi_peak_profile_graph;
        TGraph * DCA_distance_inner_phi_peak_profile_graph;
        TGraph * DCA_distance_outer_phi_peak_profile_graph;

        // note : it's for the geometry correction
        TH2F * DCA_distance_inner_phi_peak_final;
        TH2F * angle_diff_inner_phi_peak_final;
        TH2F * DCA_distance_outer_phi_peak_final;
        TH2F * angle_diff_outer_phi_peak_final;

        TH2F * xy_hist;
        TH2F * xy_hist_bkgrm;

        // note : to keep the cluster pair information
        // note : this is the vector for the whole run, not event by event
        vector<pair<type_pos,type_pos>> cluster_pair_vec;
        double Clus_InnerPhi_Offset;
        double Clus_OuterPhi_Offset;
        double Clus_InnerPhi_Offset_radian;
        double Clus_OuterPhi_Offset_radian;
        double vtxXcorrection;
        double vtxYcorrection;
        double current_vtxX;
        double current_vtxY;
        int    MacroVTXxyCorrection_new_function_call_count;

        // note : not used in the INTTXYvtx class
        // pair<double,double> zvtx_QA_width;
        // double zvtx_QA_ratio;
        int zvtx_cal_require = 15;
        map<pair<int,int>,int> axis_quadrant_map = {
            {{1,1},0},
            {{-1,1},1},
            {{-1,-1},2},
            {{1,-1},3}
        };

        double radian_correction = 180./M_PI;

        pair<double,double> beam_origin;
        pair<double,double> DCA_cut;
        string out_folder_directory;
        string plot_text;
        string run_type;
        double phi_diff_cut;        
        double peek;
        double angle_diff_new_l;
        double angle_diff_new_r;
        bool draw_event_display;
        bool print_message_opt;
        long total_NClus;
        int geo_mode_id;
        int N_clu_cutl;
        int N_clu_cut;

        // TFile * file_out;
        // TTree * tree_out;
        // double out_quadrant_corner_X;
        // double out_quadrant_corner_Y;
        // double out_quadrant_center_X;
        // double out_quadrant_center_X;
        // double out_line_filled_mean_X;
        // double out_line_filled_mean_Y;
        // double out_line_filled_stddev_X;
        // double out_line_filled_stddev_Y;

        void Init();
        void InitHist();
        void InitCanvas();
        virtual void InitTreeOut();
        void InitRest();    
        void InitGraph();    

        pair<double,double> GetVTXxyCorrection(bool phi_correction, bool calculate_XY, int trial_index, double fit_range_l, double fit_range_r);
        vector<double> GetVTXxyCorrection_new(int trial_index);
        void PrintPlotsVTXxy(string sub_out_folder_name, int print_option = 0);
        void ClearHist(int print_option = 0);

        double get_dist_offset(TH1F * hist_in, int check_N_bin); // note : check_N_bin 1 to N bins of hist
        double get_radius(double x, double y);
        void Characterize_Pad(TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0);
        std::vector<double> calculateDistanceAndClosestPoint(double x1, double y1, double x2, double y2, double target_x, double target_y);
        double calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY);
        void TH2F_threshold(TH2F * hist, double threshold);
        void TH2F_threshold_advanced(TH2F * hist, double threshold); // note : for each column
        void TH2F_threshold_advanced_row(TH2F * hist, double threshold); // note : for each row
        void TH2F_threshold_advanced_2(TH2F * hist, double threshold);
        pair<type_pos,type_pos> GetTangentPointsAtCircle(double CenterX, double CenterY, double R, double XX, double YY);
        type_pos PolarToCartesian(double radius, double angleDegrees);
        pair<type_pos,type_pos> findIntersectionPoints(double A, double mu, double sigma, double B, double C);
        vector<double> find_Ngroup(TH1F * hist_in);
        void Hist_1D_bkg_remove(TH1F * hist_in, double factor);
        void DrawTGraphErrors(vector<double> x_vec, vector<double> y_vec, vector<double> xE_vec, vector<double> yE_vec, string output_directory, vector<string> plot_name);
        void Draw2TGraph(vector<double> x1_vec, vector<double> y1_vec, vector<double> x2_vec, vector<double> y2_vec, string output_directory, vector<string> plot_name);
        void DrawTH2F(TH2F * hist_in, string output_directory, vector<string> plot_name);
        pair<double,double> GetCircleAngle(double fit_range_l, double fit_range_r);
        vector<double> SumTH2FColumnContent(TH2F * hist_in);
        vector<double> SumTH2FColumnContent_row(TH2F * hist_in);
        vector<pair<double,double>> Get4vtx(pair<double,double> origin, double length);
        vector<pair<double,double>> Get4vtxAxis(pair<double,double> origin, double length);
        vector<double> subMacroVTXxyCorrection(int test_index, int trial_index, bool draw_plot_opt);
        virtual void subMacroPlotWorking(bool phi_correction, double cos_fit_rangel, double cos_fit_ranger, double guas_fit_range);
        pair<double,double> Get_possible_zvtx(double rvtx, vector<double> p0, vector<double> p1);
        int find_quadrant(pair<double,double> Origin, pair<double,double> check_point);
        vector<double> Get_covariance_TH2(TH2F * hist_in);      

        TH1F * PrintPlots_bkgrm_helper(TH1F * hist_in, double signal_region);

        // note : from the INTTXYvtxEvt.h
        void TH2FSampleLineFill(TH2F * hist_in, double segmentation, std::pair<double,double> inner_clu, std::pair<double,double> outer_clu);
};

INTTXYvtx::INTTXYvtx(string run_type, string out_folder_directory, pair<double,double> beam_origin, int geo_mode_id, double phi_diff_cut, pair<double,double> DCA_cut, int N_clu_cutl, int N_clu_cut, bool draw_event_display, double peek, double angle_diff_new_l, double angle_diff_new_r, bool print_message_opt)
:run_type(run_type), out_folder_directory(out_folder_directory), beam_origin(beam_origin), geo_mode_id(geo_mode_id), peek(peek), N_clu_cut(N_clu_cut), N_clu_cutl(N_clu_cutl), phi_diff_cut(phi_diff_cut), DCA_cut(DCA_cut), draw_event_display(draw_event_display), angle_diff_new_l(angle_diff_new_l), angle_diff_new_r(angle_diff_new_r), print_message_opt(print_message_opt)
{
    SetsPhenixStyle();
    if (filesystem::exists(out_folder_directory.c_str()) == false) {system(Form("mkdir %s",out_folder_directory.c_str()));}
    gErrorIgnoreLevel = kWarning; // note : To not print the "print plot info."

    Init();
    plot_text = (run_type == "MC") ? "Simulation" : "Work-in-progress";

    cluster_pair_vec.clear();
    vtxXcorrection = 0;
    vtxYcorrection = 0;

    current_vtxX = beam_origin.first;
    current_vtxY = beam_origin.second;

    MacroVTXxyCorrection_new_function_call_count = 0;
}

INTTXYvtx::INTTXYvtx(string run_type, string out_folder_directory)
:run_type(run_type), out_folder_directory(out_folder_directory)
{
    SetsPhenixStyle();
    if (filesystem::exists(out_folder_directory.c_str()) == false) {system(Form("mkdir %s",out_folder_directory.c_str()));}
    gErrorIgnoreLevel = kWarning; // note : To not print the "print plot info."

    InitCanvas();
    InitRest();
    plot_text = (run_type == "MC") ? "Simulation" : "Work-in-progress";
}

void INTTXYvtx::Init()
{
    InitHist();
    InitCanvas();
    InitTreeOut();
    InitRest();
    InitGraph();
}

void INTTXYvtx::InitTreeOut()
{
    // file_out = new TFile(Form("%s/run_XY_tree.root",out_folder_directory.c_str()),"RECREATE");
    // file_out -> cd();

    // tree_out = new TTree("tree", "tree avg VtxXY");
    // tree_out -> Branch("quadrant_corner_X",&out_quadrant_corner_X);
    // tree_out -> Branch("quadrant_corner_Y",&out_quadrant_corner_Y);
    // tree_out -> Branch("quadrant_center_X",&out_quadrant_center_X);
    // tree_out -> Branch("quadrant_center_X",&out_quadrant_center_X);
    // tree_out -> Branch("line_filled_mean_X",&out_line_filled_mean_X);
    // tree_out -> Branch("line_filled_mean_Y",&out_line_filled_mean_Y);
    // tree_out -> Branch("line_filled_stddev_X",&out_line_filled_stddev_X);
    // tree_out -> Branch("line_filled_stddev_Y",&out_line_filled_stddev_Y);

    return;
}

void INTTXYvtx::InitGraph()
{
    angle_diff_inner_phi_peak_profile_graph = new TGraph();
    angle_diff_outer_phi_peak_profile_graph = new TGraph();
    DCA_distance_inner_phi_peak_profile_graph = new TGraph();
    DCA_distance_outer_phi_peak_profile_graph = new TGraph();
}

void INTTXYvtx::InitRest()
{
    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    cos_fit = new TF1("cos_fit",cos_func,0,360,4);
    cos_fit -> SetParNames("[A]", "[B]", "[C]", "[D]");
    cos_fit -> SetLineColor(2);

    gaus_fit = new TF1("gaus_fit",gaus_func,0,360,4);
    gaus_fit -> SetLineColor(4);
    gaus_fit -> SetLineWidth(1);
    gaus_fit -> SetParNames("size", "mean", "width", "offset");
    gaus_fit -> SetNpx(1000);

    gaus_pol2_fit = new TF1("gaus_pol2_fit", gaus_pol2_func, -5, 5, 7);
    gaus_pol2_fit -> SetLineWidth(2);
    gaus_pol2_fit -> SetLineColor(2);
    gaus_pol2_fit -> SetNpx(1000);

    gaus_fit_MC = new TF1("gaus_fit_MC",gaus_func,-10,10,4);
    gaus_fit_MC -> SetLineColor(2);
    gaus_fit_MC -> SetLineWidth(2);
    gaus_fit_MC -> SetNpx(1000);

    correlation_fit_MC = new TF1("correlation_fit_MC","pol1",-10,10);
    correlation_fit_MC -> SetLineColor(2);
    correlation_fit_MC -> SetLineWidth(2);

    correlation_Line = new TF1("correlation_Line","pol1",-10000,10000);
    correlation_Line -> SetLineColor(2);
    correlation_Line -> SetLineWidth(2);
    correlation_Line -> SetParameters(0,1);
    // gaus_fit_MC -> SetNpx(1000);


    gaus_fit_angle_diff = new TF1("gaus_fit_angle_diff",gaus_func,0,360,4);
    gaus_fit_angle_diff -> SetLineColor(4);
    gaus_fit_angle_diff -> SetLineWidth(2);
    gaus_fit_angle_diff -> SetParNames("size", "mean", "width", "offset");
    gaus_fit_angle_diff -> SetNpx(1000);

    horizontal_fit_inner = new TF1("horizontal_fit_inner","pol0",-360,360);
    horizontal_fit_inner -> SetLineWidth(2);
    horizontal_fit_inner -> SetLineColor(2);

    horizontal_fit_angle_diff_inner = new TF1("horizontal_fit_angle_diff_inner","pol0",-360,360);
    horizontal_fit_angle_diff_inner -> SetLineWidth(2);
    horizontal_fit_angle_diff_inner -> SetLineColor(2);

    horizontal_fit_outer = new TF1("horizontal_fit_outer","pol0",-360,360);
    horizontal_fit_outer -> SetLineWidth(2);
    horizontal_fit_outer -> SetLineColor(2);

    horizontal_fit_angle_diff_outer = new TF1("horizontal_fit_angle_diff_outer","pol0",-360,360);
    horizontal_fit_angle_diff_outer -> SetLineWidth(2);
    horizontal_fit_angle_diff_outer -> SetLineColor(2);

    coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(16);
    coord_line -> SetLineStyle(2);

    track_line = new TLine();
    track_line -> SetLineWidth(1);
    track_line -> SetLineColor(38);
    // track_line -> SetLineColorAlpha(38,0.5);

    ladder_line = new TLine();
    ladder_line -> SetLineWidth(1);

    fit_rz = new TF1("fit_rz","pol1",-500,500);

    gaus_pol1_fit = new TF1("gaus_pol1_fit",gaus_pol1_func,-3,3,5);
    gaus_pol1_fit -> SetLineWidth(2);
    gaus_pol1_fit -> SetLineColor(2);
    gaus_pol1_fit -> SetNpx(1000);

    draw_gaus_line = new TF1("draw_gaus_line",gaus_func,-3,3,4);
    draw_gaus_line -> SetLineWidth(2);
    draw_gaus_line -> SetLineStyle(9);
    draw_gaus_line -> SetLineColor(6);
    draw_gaus_line -> SetNpx(1000);

    draw_pol1_line = new TF1("draw_pol1_line","pol1",-3,3);
    draw_pol1_line -> SetLineStyle(9);
    draw_pol1_line -> SetLineWidth(2);
    draw_pol1_line -> SetLineColor(3);

    bkg_fit_pol2 = new TF1("bkg_fit_pol2",bkg_pol2_func,-5,5,5);
    bkg_fit_pol2 -> SetLineWidth(2);
    bkg_fit_pol2 -> SetLineColor(2);
    bkg_fit_pol2 -> SetNpx(1000);

    draw_pol2_line = new TF1("draw_pol2_line",full_pol2_func,-5,5,4);
    draw_pol2_line -> SetLineWidth(2);
    draw_pol2_line -> SetLineColor(2);
    draw_pol2_line -> SetNpx(1000);




    d_gaus_pol1_fit  = new TF1("d_gaus_pol1_fit",d_gaus_pol1_func,-3,3,7);
    d_gaus_pol1_fit -> SetLineWidth(2);
    d_gaus_pol1_fit -> SetLineColor(2);
    d_gaus_pol1_fit -> SetNpx(1000);

    draw_d_gaus = new TF1("draw_d_gaus", d_gaus_func,-3,3,5);
    draw_d_gaus -> SetLineStyle(9);
    draw_d_gaus -> SetLineWidth(2);
    draw_d_gaus -> SetLineColor(3);
    draw_d_gaus -> SetNpx(1000);


    // track_pol1_fit = new TF1("track_pol1_fit","pol1",-500,500);
}

void INTTXYvtx::InitCanvas()
{
    c1 = new TCanvas("","",950,800);
    c1 -> cd();
}

void INTTXYvtx::InitHist()
{
    angle_correlation = new TH2F("","angle_correlation",361,0,361,361,0,361);
    angle_correlation -> SetStats(0);
    angle_correlation -> GetXaxis() -> SetTitle("Inner cluster #Phi [degree]");
    angle_correlation -> GetYaxis() -> SetTitle("Outer cluster #Phi [degree]");
    angle_correlation -> GetXaxis() -> SetNdivisions(505);

    angle_diff_DCA_dist = new TH2F("","angle_diff_DCA_dist",100,-1.5,1.5,100,-3.5,3.5);
    angle_diff_DCA_dist -> SetStats(0);
    angle_diff_DCA_dist -> GetXaxis() -> SetTitle("Inner - Outer [degree]");
    angle_diff_DCA_dist -> GetYaxis() -> SetTitle("DCA distance [mm]");
    angle_diff_DCA_dist -> GetXaxis() -> SetNdivisions(505);

    angle_diff = new TH1F("angle_diff","angle_diff",100,0,5);
    angle_diff -> SetStats(0);
    angle_diff -> GetXaxis() -> SetTitle("|Inner - Outer| [degree]");
    angle_diff -> GetYaxis() -> SetTitle("Entry");
    angle_diff -> GetXaxis() -> SetNdivisions(505);

    angle_diff_radian = new TH1F("","angle_diff_radian;#Delta#phi (Inner - Outer) [radian];Entry",100,-0.052359878,0.052359878);
    angle_diff_radian -> SetStats(0);
    angle_diff_radian -> GetXaxis() -> SetNdivisions(505);

    angle_diff_new = new TH1F("angle_diff_new","angle_diff_new",100, angle_diff_new_l, angle_diff_new_r);
    angle_diff_new -> SetStats(0);
    angle_diff_new -> GetXaxis() -> SetTitle("|Inner - Outer| [degree]");
    angle_diff_new -> GetYaxis() -> SetTitle("Entry");
    angle_diff_new -> GetXaxis() -> SetNdivisions(505);

    angle_diff_new_bkg_remove_final = new TH1F("","angle_diff_new_bkg_remove_final",100, angle_diff_new_l, angle_diff_new_r);
    angle_diff_new_bkg_remove_final -> SetStats(0);
    angle_diff_new_bkg_remove_final -> GetXaxis() -> SetTitle("|Inner - Outer| [degree]");
    angle_diff_new_bkg_remove_final -> GetYaxis() -> SetTitle("Entry");
    angle_diff_new_bkg_remove_final -> GetXaxis() -> SetNdivisions(505);

    inner_pos_xy = new TH2F("","inner_pos_xy",360,-100,100,360,-100,100);
    inner_pos_xy -> SetStats(0);
    inner_pos_xy -> GetXaxis() -> SetTitle("X axis [mm]");
    inner_pos_xy -> GetYaxis() -> SetTitle("Y axis [mm]");
    inner_pos_xy -> GetXaxis() -> SetNdivisions(505);

    outer_pos_xy = new TH2F("","outer_pos_xy",360,-150,150,360,-150,150);
    outer_pos_xy -> SetStats(0);
    outer_pos_xy -> GetXaxis() -> SetTitle("X axis [mm]");
    outer_pos_xy -> GetYaxis() -> SetTitle("Y axis [mm]");
    outer_pos_xy -> GetXaxis() -> SetNdivisions(505);

    inner_outer_pos_xy = new TH2F("","inner_outer_pos_xy",360,-150,150,360,-150,150);
    inner_outer_pos_xy -> SetStats(0);
    inner_outer_pos_xy -> GetXaxis() -> SetTitle("X axis [mm]");
    inner_outer_pos_xy -> GetYaxis() -> SetTitle("Y axis [mm]");
    inner_outer_pos_xy -> GetXaxis() -> SetNdivisions(505);
    
    DCA_point = new TH2F("","DCA_point",100,-10,10,100,-10,10);
    DCA_point -> SetStats(0);
    DCA_point -> GetXaxis() -> SetTitle("X pos [mm]");
    DCA_point -> GetYaxis() -> SetTitle("Y pos [mm]");
    DCA_point -> GetXaxis() -> SetNdivisions(505);

    DCA_distance = new TH1F("","DCA_distance",100,-10,10);
    DCA_distance -> SetStats(0);
    DCA_distance -> GetXaxis() -> SetTitle("DCA [mm]");
    DCA_distance -> GetYaxis() -> SetTitle("Entry");
    DCA_distance -> GetXaxis() -> SetNdivisions(505);

    N_cluster_correlation = new TH2F("","N_cluster_correlation",100,0,4000,100,0,4000);
    N_cluster_correlation -> SetStats(0);
    N_cluster_correlation -> GetXaxis() -> SetTitle("inner N_cluster");
    N_cluster_correlation -> GetYaxis() -> SetTitle("Outer N_cluster");
    N_cluster_correlation -> GetXaxis() -> SetNdivisions(505);

    N_cluster_correlation_close = new TH2F("","N_cluster_correlation_close",100,0,500,100,0,500);
    N_cluster_correlation_close -> SetStats(0);
    N_cluster_correlation_close -> GetXaxis() -> SetTitle("inner N_cluster");
    N_cluster_correlation_close -> GetYaxis() -> SetTitle("Outer N_cluster");
    N_cluster_correlation_close -> GetXaxis() -> SetNdivisions(505);

    DCA_distance_inner_X = new TH2F("","DCA_distance_inner_X",100,-100,100,100,-10,10);
    DCA_distance_inner_X -> SetStats(0);
    DCA_distance_inner_X -> GetXaxis() -> SetTitle("Inner cluster X [mm]");
    DCA_distance_inner_X -> GetYaxis() -> SetTitle("DCA [mm]");
    DCA_distance_inner_X -> GetXaxis() -> SetNdivisions(505);

    DCA_distance_inner_Y = new TH2F("","DCA_distance_inner_Y",100,-100,100,100,-10,10);
    DCA_distance_inner_Y -> SetStats(0);
    DCA_distance_inner_Y -> GetXaxis() -> SetTitle("Inner cluster Y [mm]");
    DCA_distance_inner_Y -> GetYaxis() -> SetTitle("DCA [mm]");
    DCA_distance_inner_Y -> GetXaxis() -> SetNdivisions(505);

    DCA_distance_outer_X = new TH2F("","DCA_distance_outer_X",100,-130,130,100,-10,10);
    DCA_distance_outer_X -> SetStats(0);
    DCA_distance_outer_X -> GetXaxis() -> SetTitle("Outer cluster X [mm]");
    DCA_distance_outer_X -> GetYaxis() -> SetTitle("DCA [mm]");
    DCA_distance_outer_X -> GetXaxis() -> SetNdivisions(505);

    DCA_distance_outer_Y = new TH2F("","DCA_distance_outer_Y",100,-130,130,100,-10,10);
    DCA_distance_outer_Y -> SetStats(0);
    DCA_distance_outer_Y -> GetXaxis() -> SetTitle("Outer cluster Y [mm]");
    DCA_distance_outer_Y -> GetYaxis() -> SetTitle("DCA [mm]");
    DCA_distance_outer_Y -> GetXaxis() -> SetNdivisions(505);

    DCA_distance_inner_phi = new TH2F("","DCA_distance_inner_phi;Inner cluster #Phi [radian];DCA [cm]",100,-M_PI,M_PI,100,-1,1);
    DCA_distance_inner_phi -> SetStats(0);
    DCA_distance_inner_phi -> GetXaxis() -> SetNdivisions(505);

    DCA_distance_outer_phi = new TH2F("","DCA_distance_outer_phi;Outer cluster #Phi [radian];DCA [cm]",100,-M_PI,M_PI,100,-1,1);
    DCA_distance_outer_phi -> SetStats(0);
    DCA_distance_outer_phi -> GetXaxis() -> SetNdivisions(505);

    DCA_distance_inner_phi_peak_final = new TH2F("","DCA_distance_inner_phi_peak_final;Inner cluster #Phi [radian];DCA [cm]",100,-M_PI,M_PI,100,-1,1);
    DCA_distance_inner_phi_peak_final -> SetStats(0);
    DCA_distance_inner_phi_peak_final -> GetXaxis() -> SetNdivisions(505);

    DCA_distance_outer_phi_peak_final = new TH2F("","DCA_distance_outer_phi_peak_final;Outer cluster #Phi [radian];DCA [cm]",100,-M_PI,M_PI,100,-1,1);
    DCA_distance_outer_phi_peak_final -> SetStats(0);
    DCA_distance_outer_phi_peak_final -> GetXaxis() -> SetNdivisions(505);

    angle_diff_inner_phi = new TH2F("","angle_diff_inner_phi;Inner cluster #Phi [radian];#Delta#Phi (Inner - Outer) [radian]",100,-M_PI,M_PI,100,-0.03,0.03);
    angle_diff_inner_phi -> SetStats(0);
    angle_diff_inner_phi -> GetXaxis() -> SetNdivisions(505);

    angle_diff_outer_phi = new TH2F("","angle_diff_outer_phi;Outer cluster #Phi [radian];#Delta#Phi (Inner - Outer) [radian]",100,-M_PI,M_PI,100,-0.03,0.03);
    angle_diff_outer_phi -> SetStats(0);
    angle_diff_outer_phi -> GetXaxis() -> SetNdivisions(505);

    angle_diff_inner_phi_peak_final = new TH2F("","angle_diff_inner_phi_peak_final;Inner cluster #Phi [radian];#Delta#Phi (Inner - Outer) [radian]",100,-M_PI,M_PI,100,-0.03,0.03);
    angle_diff_inner_phi_peak_final -> SetStats(0);
    angle_diff_inner_phi_peak_final -> GetXaxis() -> SetNdivisions(505);

    angle_diff_outer_phi_peak_final = new TH2F("","angle_diff_outer_phi_peak_final;Outer cluster #Phi [radian];#Delta#Phi (Inner - Outer) [radian]",100,-M_PI,M_PI,100,-0.03,0.03);
    angle_diff_outer_phi_peak_final -> SetStats(0);
    angle_diff_outer_phi_peak_final -> GetXaxis() -> SetNdivisions(505);


    

    final_angle_diff_inner_phi_radian_coarseX = new TH2F(
        "",
        "final_angle_diff_inner_phi_radian_coarseX;Inner cluster #Phi [radian];#Delta#Phi (Inner - Outer) [radian]",
        ana_map_version::final_angle_diff_inner_phi_radian_coarseX_XNbins, ana_map_version::final_angle_diff_inner_phi_radian_coarseX_Xmin, ana_map_version::final_angle_diff_inner_phi_radian_coarseX_Xmax,
        ana_map_version::final_angle_diff_inner_phi_radian_coarseX_YNbins, ana_map_version::final_angle_diff_inner_phi_radian_coarseX_Ymin, ana_map_version::final_angle_diff_inner_phi_radian_coarseX_Ymax
    );
    final_angle_diff_inner_phi_radian_coarseX -> SetStats(0);
    final_angle_diff_inner_phi_radian_coarseX -> GetXaxis() -> SetNdivisions(505);

    final_angle_diff_inner_phi_degree_coarseX = new TH2F(
        "",
        "final_angle_diff_inner_phi_degree_coarseX;Inner cluster #Phi [degree];#Delta#Phi (Inner - Outer) [degree]",
        final_angle_diff_inner_phi_radian_coarseX -> GetNbinsX(), 0, 360,
        final_angle_diff_inner_phi_radian_coarseX -> GetNbinsY(), final_angle_diff_inner_phi_radian_coarseX -> GetYaxis() -> GetXmin() * radian_correction, final_angle_diff_inner_phi_radian_coarseX -> GetYaxis() -> GetXmax() * radian_correction
    );
    final_angle_diff_inner_phi_degree_coarseX -> SetStats(0);
    final_angle_diff_inner_phi_degree_coarseX -> GetXaxis() -> SetNdivisions(505);

    final_angle_diff_radian_before = new TH1F("","final_angle_diff_radian_before;#Delta#phi (Inner - Outer) [radian];Entry",200,-0.045,0.045);
    final_angle_diff_radian_before -> SetStats(0);
    final_angle_diff_radian_before -> GetXaxis() -> SetNdivisions(505);

    final_angle_diff_radian_post = new TH1F(
        "",
        "final_angle_diff_radian_post;#Delta#phi (Inner - Outer) [radian];Entry",
        final_angle_diff_radian_before -> GetNbinsX(),
        final_angle_diff_radian_before -> GetXaxis() -> GetXmin(),
        final_angle_diff_radian_before -> GetXaxis() -> GetXmax()
    );
    final_angle_diff_radian_post -> SetStats(0);
    final_angle_diff_radian_post -> GetXaxis() -> SetNdivisions(505);


    final_angle_diff_radian_DCA_2D_before = new TH2F("","final_angle_diff_radian_DCA_2D_before;Inner - Outer [radian];DCA distance [cm]",100,-1.5/radian_correction,1.5/radian_correction,100,-0.35,0.35);
    final_angle_diff_radian_DCA_2D_before -> SetStats(0);
    final_angle_diff_radian_DCA_2D_before -> GetXaxis() -> SetNdivisions(505);

    final_angle_diff_radian_DCA_2D_post = new TH2F(
        "",
        "final_angle_diff_radian_DCA_2D_post;Inner - Outer [radian];DCA distance [cm]",
        final_angle_diff_radian_DCA_2D_before -> GetNbinsX(),
        final_angle_diff_radian_DCA_2D_before -> GetXaxis() -> GetXmin(),
        final_angle_diff_radian_DCA_2D_before -> GetXaxis() -> GetXmax(),
        final_angle_diff_radian_DCA_2D_before -> GetNbinsY(),
        final_angle_diff_radian_DCA_2D_before -> GetYaxis() -> GetXmin(),
        final_angle_diff_radian_DCA_2D_before -> GetYaxis() -> GetXmax()
    );
    final_angle_diff_radian_DCA_2D_post -> GetXaxis() -> SetNdivisions(505);

    final_DCA_cm_inner_phi_radian_coarseX = new TH2F(
        "",
        "final_DCA_cm_inner_phi_radian_coarseX;Inner cluster #Phi [radian];DCA [cm]",
        ana_map_version::final_angle_diff_inner_phi_radian_coarseX_XNbins, ana_map_version::final_angle_diff_inner_phi_radian_coarseX_Xmin, ana_map_version::final_angle_diff_inner_phi_radian_coarseX_Xmax,
        100, -1., 1. // note : unit cm
    );
    final_DCA_cm_inner_phi_radian_coarseX -> GetXaxis() -> SetNdivisions(505);

    final_DCA_mm_inner_phi_degree_coarseX = new TH2F(
        "",
        "final_DCA_mm_inner_phi_degree_coarseX;Inner cluster #Phi [degree];DCA [mm]",
        final_DCA_cm_inner_phi_radian_coarseX -> GetNbinsX(), 0, 360,
        final_DCA_cm_inner_phi_radian_coarseX -> GetNbinsY(), final_DCA_cm_inner_phi_radian_coarseX -> GetYaxis() -> GetXmin() * 10, final_DCA_cm_inner_phi_radian_coarseX -> GetYaxis() -> GetXmax() * 10 // note : unit mm
    );
    final_DCA_mm_inner_phi_degree_coarseX -> GetXaxis() -> SetNdivisions(505);

    final_DCA_cm_before = new TH1F(
        "",
        "final_DCA_cm_before;DCA [cm];Entry",
        final_DCA_cm_inner_phi_radian_coarseX -> GetNbinsY(),
        final_DCA_cm_inner_phi_radian_coarseX -> GetYaxis() -> GetXmin(),
        final_DCA_cm_inner_phi_radian_coarseX -> GetYaxis() -> GetXmax()
    );
    final_DCA_cm_before -> GetXaxis() -> SetNdivisions(505);

    final_DCA_cm_post = new TH1F(
        "",
        "final_DCA_cm_post;DCA [cm];Entry",
        final_DCA_cm_inner_phi_radian_coarseX -> GetNbinsY(),
        final_DCA_cm_inner_phi_radian_coarseX -> GetYaxis() -> GetXmin(),
        final_DCA_cm_inner_phi_radian_coarseX -> GetYaxis() -> GetXmax()
    );
    final_DCA_cm_post -> GetXaxis() -> SetNdivisions(505);
    

}

// note : this function only prepare the pairs for the vertex XY calculation, it's like a general vertex for the whole run
void INTTXYvtx::ProcessEvt(
    int event_i,
    vector<clu_info> temp_sPH_inner_nocolumn_vec,
    vector<clu_info> temp_sPH_outer_nocolumn_vec, 
    vector<vector<double>> temp_sPH_nocolumn_vec,
    vector<vector<double>> temp_sPH_nocolumn_rz_vec,
    int NvtxMC,
    double TrigZvtxMC,
    bool PhiCheckTag,
    Long64_t bco_full,
    int is_min_bias,
    int is_min_bias_wozdc
)
{
    if (event_i%10000 == 0) {cout<<"In INTTXYvtx class, running event : "<<event_i<<endl;}

    total_NClus = temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size();

    // note : the Move these two in the beginning of the function, in order to avoid those event-reject cuts
    N_cluster_correlation -> Fill(temp_sPH_inner_nocolumn_vec.size(),temp_sPH_outer_nocolumn_vec.size());
    N_cluster_correlation_close -> Fill(temp_sPH_inner_nocolumn_vec.size(),temp_sPH_outer_nocolumn_vec.size());

    // todo : for the data, the is_min_bias_wozdc is used
    if (run_type == "MC" && is_min_bias_wozdc == 0) {return; cout<<"In INTTXYvtx class, MC, not min bias event"<<endl;}
    if (run_type != "MC" && is_min_bias_wozdc == 0) {return; cout<<"In INTTXYvtx class, data, not min bias event"<<endl;}

    if (total_NClus < zvtx_cal_require) {return; cout<<"return confirmation"<<endl;}   
    
    if (run_type == "MC" && NvtxMC != 1) { return; cout<<"In INTTXYvtx class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Nvtx more than one "<<endl;}
    if (PhiCheckTag == false)            { return; cout<<"In INTTXYvtx class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Not full phi has hits "<<endl;}
    
    if (temp_sPH_inner_nocolumn_vec.size() < 10 || temp_sPH_outer_nocolumn_vec.size() < 10 || total_NClus > N_clu_cut || total_NClus < N_clu_cutl)
    {
        return;
        printf("In INTTXYvtx class, event : %i, low clu continue, NClus : %lu \n", event_i, total_NClus); 
    }

    for ( int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++ )
    {    
        for ( int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++ )
        {
            // note : try to ease the analysis and also make it quick.
            if (fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi) < 7) // todo : the pre phi cut is here, can be optimized
            {
                cluster_pair_vec.push_back({{temp_sPH_inner_nocolumn_vec[inner_i].x,temp_sPH_inner_nocolumn_vec[inner_i].y},{temp_sPH_outer_nocolumn_vec[outer_i].x,temp_sPH_outer_nocolumn_vec[outer_i].y}});
            }
            
        }
    }

    for ( int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++ )
    {
        inner_pos_xy -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].x,temp_sPH_inner_nocolumn_vec[inner_i].y);
        inner_outer_pos_xy -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].x,temp_sPH_inner_nocolumn_vec[inner_i].y);
    }

    for ( int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++ )
    {
        outer_pos_xy -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].x,temp_sPH_outer_nocolumn_vec[outer_i].y);
        inner_outer_pos_xy -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].x,temp_sPH_outer_nocolumn_vec[outer_i].y);
    }
}

unsigned long INTTXYvtx::GetVecNele()
{
    return cluster_pair_vec.size();
}

pair<double,double> INTTXYvtx::GetFinalVTXxy()
{
    return {current_vtxX, current_vtxY};
}

void INTTXYvtx::ClearEvt()
{
    return;
}

pair<vector<TH2F *>, vector<TH1F*>> INTTXYvtx::GetHistFinal()
{
    return {
        {DCA_distance_inner_phi_peak_final, angle_diff_inner_phi_peak_final, DCA_distance_outer_phi_peak_final, angle_diff_outer_phi_peak_final, xy_hist, xy_hist_bkgrm},
        {angle_diff_new_bkg_remove_final}
    };
}

void INTTXYvtx::PrintPlots()
{   
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    inner_outer_pos_xy -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/inner_outer_pos_xy.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    inner_pos_xy -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/inner_pos_xy.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    outer_pos_xy -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/outer_pos_xy.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    N_cluster_correlation -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/N_cluster_correlation.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    N_cluster_correlation_close -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/N_cluster_correlation_close.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
}

// note : the pair here would be {radius of the circle, possible correction angle}
pair<double,double> INTTXYvtx::GetCircleAngle(double fit_range_l, double fit_range_r)
{
    cout<<"Get Circle of correction and possible correction angle ---------------------------- ---------------------------- ----------------------------"<<endl;

    cos_fit -> SetParameters(4, 1.49143e-02,  185, 0.3); // todo : we may have to apply more constaints on the fitting
    cos_fit -> SetParLimits(2,0,360); // note : the peak location has to be positive
    // cos_fit -> SetParLimits(0,0,1000); // note : the amplitude has to be positive
    
    // note : here is the test with a gaus fitting to find the peak
    gaus_fit -> SetParameters(-4.5, cos_fit->GetParameter(2), 50, 0);
    gaus_fit -> SetParLimits(0,-100,0); // note : the gaus distribution points down

    subMacroPlotWorking(0,fit_range_l, fit_range_r, 40);
    
    cout<<"circle radius by gaussian : "<<abs(gaus_fit->GetParameter(0) + gaus_fit->GetParameter(3))<<" possible correction angle : "<<gaus_fit->GetParameter(1)<<endl;
    cout<<"circle radius by cosine : "<<abs(cos_fit->GetParameter(0) - cos_fit->GetParameter(3))<<" possible correction angle : "<<cos_fit->GetParameter(2)<<endl;
    
    // return {abs(gaus_fit->GetParameter(0) + gaus_fit->GetParameter(3)), gaus_fit->GetParameter(1)}; // note : gaussian based
    return {abs(cos_fit->GetParameter(0) - cos_fit->GetParameter(3)), cos_fit->GetParameter(2)}; // note : cosine wave based
}

vector<pair<double,double>> INTTXYvtx::MacroVTXSquare(double length, int N_trial, bool draw_plot_opt)
{
    double original_length = length;
    pair<double,double> origin = {0,0};
    vector<pair<double,double>> vtx_vec = Get4vtx(origin,length); // vtx_vec.push_back(origin);
    int small_index;
    vector<double> small_info_vec;
    vector<double> grr_x; grr_x.clear();
    vector<double> grr_E; grr_E.clear();
    vector<double> grr_y; grr_y.clear();

    vector<double> All_FitError_DCA_Y; All_FitError_DCA_Y.clear();
    vector<double> All_FitError_DCA_X; All_FitError_DCA_X.clear();
    vector<double> All_FitError_angle_Y; All_FitError_angle_Y.clear();
    vector<double> All_FitError_angle_X; All_FitError_angle_X.clear();

    vector<double> Winner_FitError_DCA_Y; Winner_FitError_DCA_Y.clear();
    vector<double> Winner_FitError_DCA_X; Winner_FitError_DCA_X.clear();
    vector<double> Winner_FitError_angle_Y; Winner_FitError_angle_Y.clear();
    vector<double> Winner_FitError_angle_X; Winner_FitError_angle_X.clear();

    cout<<"In INTTXYvtx::MacroVTXSquare, N pairs : "<<cluster_pair_vec.size()<<endl;

    if (print_message_opt == true) {cout<<N_trial<<" runs, smart. which gives you the resolution down to "<<length/pow(2,N_trial)<<" mm"<<endl;}

    for (int i = 0; i < N_trial; i++)
    {
        if (print_message_opt == true) {cout<<"~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~"<<" step "<<i<<" ~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~"<<endl;}
        for (int i1 = 0; i1 < vtx_vec.size(); i1++)
        {
            if (print_message_opt == true) {cout<<"tested vertex : "<<vtx_vec[i1].first<<" "<<vtx_vec[i1].second<<endl;}
            current_vtxX = vtx_vec[i1].first;
            current_vtxY = vtx_vec[i1].second;
            vector<double> info_vec = subMacroVTXxyCorrection(i,i1, draw_plot_opt);
            cout<<"trial : "<<i<<" vertex : "<<i1<<" DCA fit error : "<<info_vec[3]<<" angle diff fit error : "<<info_vec[5]<<endl;
            All_FitError_DCA_Y.push_back(info_vec[3]);
            All_FitError_DCA_X.push_back(i);
            All_FitError_angle_Y.push_back(info_vec[5]);
            All_FitError_angle_X.push_back(i);

            if (i1 == 0)
            {
                small_info_vec = info_vec;
                small_index = i1;
                
                TH2F_FakeClone(DCA_distance_inner_phi_peak,DCA_distance_inner_phi_peak_final);
                TH2F_FakeClone(angle_diff_inner_phi_peak,angle_diff_inner_phi_peak_final);
                TH2F_FakeClone(DCA_distance_outer_phi_peak,DCA_distance_outer_phi_peak_final);
                TH2F_FakeClone(angle_diff_outer_phi_peak,angle_diff_outer_phi_peak_final);
                TH1F_FakeClone(angle_diff_new_bkg_remove, angle_diff_new_bkg_remove_final);
            }
            else
            {
                if (info_vec[3] < small_info_vec[3] && info_vec[5] < small_info_vec[5]) // note : the fit error of the pol0 fit
                {
                    small_info_vec = info_vec;
                    small_index = i1;

                    TH2F_FakeClone(DCA_distance_inner_phi_peak,DCA_distance_inner_phi_peak_final);
                    TH2F_FakeClone(angle_diff_inner_phi_peak,angle_diff_inner_phi_peak_final);
                    TH2F_FakeClone(DCA_distance_outer_phi_peak,DCA_distance_outer_phi_peak_final);
                    TH2F_FakeClone(angle_diff_outer_phi_peak,angle_diff_outer_phi_peak_final);
                    TH1F_FakeClone(angle_diff_new_bkg_remove, angle_diff_new_bkg_remove_final);
                }
            }
            if (print_message_opt == true){cout<<" "<<endl;}

            ClearHist(1);
        }

        if (print_message_opt == true) {cout<<"the Quadrant "<<small_index<<" won the competition"<<endl;}
        
        Winner_FitError_DCA_Y.push_back(small_info_vec[3]);
        Winner_FitError_DCA_X.push_back(i);
        Winner_FitError_angle_Y.push_back(small_info_vec[5]);
        Winner_FitError_angle_X.push_back(i);

        grr_x.push_back(i);
        grr_y.push_back(small_index);
        grr_E.push_back(0);

        // note : generating the new 4 vertex for the next comparison
        // note : start to shrink the square
        if (i != N_trial - 1)
        {
            origin = {(vtx_vec[small_index].first + origin.first)/2., (vtx_vec[small_index].second + origin.second)/2.};
            // cout<<"test : "<<origin.first<<" "<<origin.second<<" length: "<<length<<endl;
            // if (small_index == 4) {length /= 1.5;}
            // else {length /= 2.;}
            length /= 2.;
            vtx_vec = Get4vtx(origin,length); // vtx_vec.push_back(origin);
        }
    }

    if (draw_plot_opt == true) {DrawTGraphErrors(grr_x, grr_y, grr_E, grr_E, out_folder_directory, {Form("Square_scan_history_%.1fmm_%iTrials", original_length, N_trial), "nth scan", "Winner index", "APL"});}
    if (draw_plot_opt == true) {Draw2TGraph(All_FitError_angle_X, All_FitError_angle_Y, Winner_FitError_angle_X, Winner_FitError_angle_Y, out_folder_directory, {Form("Angle_diff_fit_error_%iTrials", N_trial), "n iteration", "#Delta#phi fit error [radian]"});}
    if (draw_plot_opt == true) {Draw2TGraph(All_FitError_DCA_X, All_FitError_DCA_Y, Winner_FitError_DCA_X, Winner_FitError_DCA_Y, out_folder_directory, {Form("DCA_fit_error_%iTrials", N_trial), "n iteration", "DCA fit error [cm]"});}

    return {
        vtx_vec[small_index], // note : the best vertex 
        origin,               // note : the origin in that trial
        {small_info_vec[3],small_info_vec[5]},   // note : horizontal_fit_inner -> GetParError(0),  horizontal_fit_angle_diff_inner -> GetParError(0)
        {small_info_vec[10],small_info_vec[11]}, // note : horizontal_fit_inner -> GetParameter(0), horizontal_fit_angle_diff_inner -> GetParameter(0)
        {small_info_vec[7],small_info_vec[9]},   // note : horizontal_fit_outer -> GetParError(0),  horizontal_fit_angle_diff_outer -> GetParError(0)
        {small_info_vec[12],small_info_vec[13]}, // note : horizontal_fit_outer -> GetParameter(0), horizontal_fit_angle_diff_outer -> GetParameter(0)
        {small_info_vec[14],small_info_vec[15]},  // note : the mean and stddev of angle_diff
        {small_info_vec[16],small_info_vec[17]},  // note : the mean and stddev of DCA_distance
        {small_info_vec[0],small_info_vec[1]},    // note : the mean and stddev of angle_diff, but with the background removed
    };
}

vector<double> INTTXYvtx::subMacroVTXxyCorrection(int test_index, int trial_index, bool draw_plot_opt)
{
    int true_trial_index = test_index * 4 + trial_index;
    string sub_out_folder_name;
    if (draw_plot_opt == true){
        sub_out_folder_name = Form("%s/New_trial_square_%i_%i",out_folder_directory.c_str(), test_index, trial_index);
        if (filesystem::exists(sub_out_folder_name.c_str()) == false) {system(Form("mkdir %s",sub_out_folder_name.c_str()));}
    }

    vector<double> out_vec = GetVTXxyCorrection_new(true_trial_index);
    if (draw_plot_opt == true){PrintPlotsVTXxy(sub_out_folder_name, 1);}
    // ClearHist(1);

    return out_vec;
}

// note : {circle radius, possible correction angle, the chi2/NDF of pol0 fit}
vector<double> INTTXYvtx::GetVTXxyCorrection_new(int trial_index)
{
    if (print_message_opt == true) {
        cout<<"Trial : "<<trial_index<<"---------------------------- ---------------------------- ----------------------------"<<endl;
        cout<<"Given vertex: "<<current_vtxX <<" "<<current_vtxY<<endl;
    }


    cos_fit -> SetParameters(4, 1.49143e-02,  185, 0.3); // todo : we may have to apply more constaints on the fitting
    // cos_fit -> SetParLimits(0,0,1000); // note : the amplitude has to be positive
    cos_fit -> SetParLimits(2,0,360); // note : the peak location has to be positive

    // note : here is the test with a gaus fitting to find the peak
    gaus_fit -> SetParameters(-4.5, 197, 50, 0);
    gaus_fit -> SetParLimits(0,-100,0); // note : the gaus distribution points down
    // DCA_distance_inner_phi_peak_profile -> Fit(gaus_fit, "N","",100, 260);
    // cout<<"test, gaus fit range : "<<gaus_fit->GetParameter(1) - 25<<" "<<gaus_fit->GetParameter(1) + 25<<endl;
    
    subMacroPlotWorking(1,100,260,25);

    return {
        angle_diff_new_bkg_remove -> GetMean(), angle_diff_new_bkg_remove -> GetStdDev(), // note : angle diff stddev and error (1D histogram)
        horizontal_fit_inner -> GetChisquare() / double(horizontal_fit_inner -> GetNDF()), horizontal_fit_inner->GetParError(0),  // note : inner DCA, pol0
        horizontal_fit_angle_diff_inner -> GetChisquare() / double(horizontal_fit_angle_diff_inner -> GetNDF()), horizontal_fit_angle_diff_inner->GetParError(0), // note : inner angle diff, pol0
        horizontal_fit_outer -> GetChisquare() / double(horizontal_fit_outer -> GetNDF()), horizontal_fit_outer->GetParError(0), // note : outer DCA, pol0
        horizontal_fit_angle_diff_outer -> GetChisquare() / double(horizontal_fit_angle_diff_outer -> GetNDF()), horizontal_fit_angle_diff_outer->GetParError(0),  // note : outer angle diff, pol0
        horizontal_fit_inner -> GetParameter(0), horizontal_fit_angle_diff_inner -> GetParameter(0), // note : 10, 11
        horizontal_fit_outer -> GetParameter(0), horizontal_fit_angle_diff_outer -> GetParameter(0), // note : 12, 13
        angle_diff -> GetMean(), angle_diff -> GetStdDev(), // note : 14, 15
        DCA_distance -> GetMean(), DCA_distance -> GetStdDev() // note : 16, 17
    };
}

void INTTXYvtx::subMacroPlotWorking(bool phi_correction, double cos_fit_rangel, double cos_fit_ranger, double guas_fit_range)
{
    
    for (int i = 0; i < cluster_pair_vec.size(); i++)
    {
        vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
            cluster_pair_vec[i].first.x, cluster_pair_vec[i].first.y,
            cluster_pair_vec[i].second.x, cluster_pair_vec[i].second.y,
            current_vtxX, current_vtxY
        );

        double DCA_sign = calculateAngleBetweenVectors(
            cluster_pair_vec[i].second.x, cluster_pair_vec[i].second.y,
            cluster_pair_vec[i].first.x, cluster_pair_vec[i].first.y,
            current_vtxX, current_vtxY
        );

        if (phi_correction == true)
        {
            // cout<<"option selected "<<endl;
            Clus_InnerPhi_Offset = (cluster_pair_vec[i].first.y - current_vtxY < 0) ? atan2(cluster_pair_vec[i].first.y - current_vtxY, cluster_pair_vec[i].first.x - current_vtxX) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].first.y - current_vtxY, cluster_pair_vec[i].first.x - current_vtxX) * (180./TMath::Pi());
            Clus_OuterPhi_Offset = (cluster_pair_vec[i].second.y - current_vtxY < 0) ? atan2(cluster_pair_vec[i].second.y - current_vtxY, cluster_pair_vec[i].second.x - current_vtxX) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].second.y - current_vtxY, cluster_pair_vec[i].second.x - current_vtxX) * (180./TMath::Pi());

            // note : to have the radian
            Clus_InnerPhi_Offset_radian = atan2(cluster_pair_vec[i].first.y  - current_vtxY, cluster_pair_vec[i].first.x  - current_vtxX);
            Clus_OuterPhi_Offset_radian = atan2(cluster_pair_vec[i].second.y - current_vtxY, cluster_pair_vec[i].second.x - current_vtxX);

        }
        else // note : phi_correction == false 
        {
            Clus_InnerPhi_Offset = (cluster_pair_vec[i].first.y < 0) ? atan2(cluster_pair_vec[i].first.y, cluster_pair_vec[i].first.x) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].first.y, cluster_pair_vec[i].first.x) * (180./TMath::Pi()); 
            Clus_OuterPhi_Offset = (cluster_pair_vec[i].second.y < 0) ? atan2(cluster_pair_vec[i].second.y, cluster_pair_vec[i].second.x) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].second.y, cluster_pair_vec[i].second.x) * (180./TMath::Pi()); 

            // note : to have the radian
            Clus_InnerPhi_Offset_radian = atan2(cluster_pair_vec[i].first.y,  cluster_pair_vec[i].first.x);
            Clus_OuterPhi_Offset_radian = atan2(cluster_pair_vec[i].second.y, cluster_pair_vec[i].second.x);

        }

        // double phi_test = (cluster_pair_vec[i].first.y < 0) ? atan2(cluster_pair_vec[i].first.y, cluster_pair_vec[i].first.x) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].first.y, cluster_pair_vec[i].first.x) * (180./TMath::Pi());
        // double phi_test_offset = (cluster_pair_vec[i].first.y - current_vtxY < 0) ? atan2(cluster_pair_vec[i].first.y - current_vtxY, cluster_pair_vec[i].first.x - current_vtxX) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].first.y - current_vtxY, cluster_pair_vec[i].first.x - current_vtxX) * (180./TMath::Pi());
        // cout<<"angle offset test : "<<phi_test<<", "<<phi_test_offset<<endl;
    
        angle_correlation -> Fill(Clus_InnerPhi_Offset, Clus_OuterPhi_Offset);
        angle_diff_DCA_dist -> Fill(Clus_InnerPhi_Offset - Clus_OuterPhi_Offset, DCA_sign);
        angle_diff -> Fill(abs(Clus_InnerPhi_Offset - Clus_OuterPhi_Offset));
        DCA_point -> Fill(DCA_info_vec[1], DCA_info_vec[2]);
        DCA_distance -> Fill(DCA_sign);
        DCA_distance_inner_X -> Fill(cluster_pair_vec[i].first.x, DCA_sign);
        DCA_distance_inner_Y -> Fill(cluster_pair_vec[i].first.y, DCA_sign);
        DCA_distance_outer_X -> Fill(cluster_pair_vec[i].second.x, DCA_sign);
        DCA_distance_outer_Y -> Fill(cluster_pair_vec[i].second.y, DCA_sign);

        angle_diff_new -> Fill(abs(Clus_InnerPhi_Offset - Clus_OuterPhi_Offset));
        angle_diff_radian -> Fill(Clus_InnerPhi_Offset_radian - Clus_OuterPhi_Offset_radian);

        // note : for the special treatment
        DCA_distance_inner_phi -> Fill(Clus_InnerPhi_Offset_radian, DCA_sign / 10.);
        DCA_distance_outer_phi -> Fill(Clus_OuterPhi_Offset_radian, DCA_sign / 10.);
        angle_diff_inner_phi   -> Fill(Clus_InnerPhi_Offset_radian, Clus_InnerPhi_Offset_radian - Clus_OuterPhi_Offset_radian);
        angle_diff_outer_phi   -> Fill(Clus_OuterPhi_Offset_radian, Clus_InnerPhi_Offset_radian - Clus_OuterPhi_Offset_radian);

    } // note : end of the loop for the cluster pair

    // note : ----------- ----------- ----------- ---------- ----------- ----------- ---------- ----------- ----------- ----------- -----------
    DCA_distance_inner_phi_peak = (TH2F*)DCA_distance_inner_phi -> Clone();
    TH2F_threshold_advanced_2(DCA_distance_inner_phi_peak, 0.5); // todo : the background cut can be modified, the ratio 0.5
    DCA_distance_inner_phi_peak_profile =  DCA_distance_inner_phi_peak->ProfileX("DCA_distance_inner_phi_peak_profile");
    // TGraph * DCA_distance_inner_phi_peak_profile_graph = new TGraph();
    double point_index = 0;
    vector<double> hist_column_content = SumTH2FColumnContent(DCA_distance_inner_phi_peak);
    for (int i = 0; i < DCA_distance_inner_phi_peak_profile->GetNbinsX(); i++){
        if (hist_column_content[i] < 5){continue;} // note : in order to remove some remaining background

        DCA_distance_inner_phi_peak_profile_graph -> SetPoint(point_index, DCA_distance_inner_phi_peak_profile->GetBinCenter(i+1), DCA_distance_inner_phi_peak_profile->GetBinContent(i+1));
        // cout<<"("<<DCA_distance_inner_phi_peak_profile->GetBinCenter(i+1)<<", "<< DCA_distance_inner_phi_peak_profile->GetBinContent(i+1)<<")"<<endl;
        point_index += 1;
    }

    // cos_fit -> SetParameters(4, 1.49143e-02,  185, 0.3); // todo : we may have to apply more constaints on the fitting
    // cos_fit -> SetParLimits(0,0,1000); // note : the amplitude has to be positive
    // cos_fit -> SetParLimits(2,0,360); // note : the peak location has to be positive

    // note : here is the test with a gaus fitting to find the peak
    // gaus_fit -> SetParameters(-4.5, cos_fit->GetParameter(2), 50, 0);
    // gaus_fit -> SetParLimits(0,-100,0); // note : the gaus distribution points down
    // DCA_distance_inner_phi_peak_profile -> Fit(gaus_fit, "N","",100, 260);
    // cout<<"test, gaus fit range : "<<gaus_fit->GetParameter(1) - 25<<" "<<gaus_fit->GetParameter(1) + 25<<endl;
       
    horizontal_fit_inner -> SetParameter(0,0);

    // todo : the fit range of the gaussian fit can be modified here 
    DCA_distance_inner_phi_peak_profile_graph -> Fit(horizontal_fit_inner,"NQ","",-M_PI, M_PI);
    // DCA_distance_inner_phi_peak_profile_graph -> Fit(gaus_fit, "NQ","",cos_fit->GetParameter(2) - guas_fit_range, cos_fit->GetParameter(2) + guas_fit_range); // note : what we want and need is the peak position, so we fit the peak again 
    // DCA_distance_inner_phi_peak_profile_graph -> Fit(cos_fit, "NQ","",cos_fit_rangel, cos_fit_ranger);

    // note : ----------- ----------- ----------- ---------- ----------- ----------- ---------- ----------- ----------- ----------- -----------
    angle_diff_new_bkg_remove = (TH1F*)angle_diff_new -> Clone();
    angle_diff_new_bkg_remove -> SetLineColor(2);
    Hist_1D_bkg_remove(angle_diff_new_bkg_remove, 1.5);

    // note : ----------- ----------- ----------- ---------- ----------- ----------- ---------- ----------- ----------- ----------- -----------
    angle_diff_inner_phi_peak = (TH2F*)angle_diff_inner_phi -> Clone();
    // TH2F_threshold_advanced(angle_diff_inner_phi_peak, 0.5);
    TH2F_threshold_advanced_2(angle_diff_inner_phi_peak, 0.5); // todo : threshold ratio can be modified here
    hist_column_content = SumTH2FColumnContent(angle_diff_inner_phi_peak);
    angle_diff_inner_phi_peak_profile =  angle_diff_inner_phi_peak->ProfileX("angle_diff_inner_phi_peak_profile");
    // TGraph * angle_diff_inner_phi_peak_profile_graph = new TGraph();
    point_index = 0;
    for (int i = 0; i < angle_diff_inner_phi_peak_profile->GetNbinsX(); i++){
        if (hist_column_content[i] < 5){continue;} // note : in order to remove some remaining background
        
        angle_diff_inner_phi_peak_profile_graph -> SetPoint(point_index, angle_diff_inner_phi_peak_profile->GetBinCenter(i+1), angle_diff_inner_phi_peak_profile->GetBinContent(i+1));
        // cout<<"("<<angle_diff_inner_phi_peak_profile->GetBinCenter(i+1)<<", "<< angle_diff_inner_phi_peak_profile->GetBinContent(i+1)<<")"<<endl;
        point_index += 1;
    }

    horizontal_fit_angle_diff_inner -> SetParameter(0,0);
    angle_diff_inner_phi_peak_profile_graph -> Fit(horizontal_fit_angle_diff_inner,"NQ","",-M_PI, M_PI);

    // note : ----------- ----------- ----------- ---------- ----------- ----------- ---------- ----------- ----------- ----------- -----------
    DCA_distance_outer_phi_peak = (TH2F*)DCA_distance_outer_phi -> Clone();
    TH2F_threshold_advanced_2(DCA_distance_outer_phi_peak, 0.5); // todo : the background cut can be modified, the ratio 0.5
    DCA_distance_outer_phi_peak_profile =  DCA_distance_outer_phi_peak->ProfileX("DCA_distance_outer_phi_peak_profile");
    // TGraph * DCA_distance_outer_phi_peak_profile_graph = new TGraph();
    point_index = 0;
    hist_column_content = SumTH2FColumnContent(DCA_distance_outer_phi_peak);
    for (int i = 0; i < DCA_distance_outer_phi_peak_profile->GetNbinsX(); i++){
        if (hist_column_content[i] < 5){continue;} // note : in order to remove some remaining background

        DCA_distance_outer_phi_peak_profile_graph -> SetPoint(point_index, DCA_distance_outer_phi_peak_profile->GetBinCenter(i+1), DCA_distance_outer_phi_peak_profile->GetBinContent(i+1));
        // cout<<"("<<DCA_distance_outer_phi_peak_profile->GetBinCenter(i+1)<<", "<< DCA_distance_outer_phi_peak_profile->GetBinContent(i+1)<<")"<<endl;
        point_index += 1;
    }
       
    horizontal_fit_outer -> SetParameter(0,0);
    // todo : the fit range of the gaussian fit can be modified here 
    DCA_distance_outer_phi_peak_profile_graph -> Fit(horizontal_fit_outer,"NQ","",-M_PI, M_PI);

    // note : ----------- ----------- ----------- ---------- ----------- ----------- ---------- ----------- ----------- ----------- -----------
    angle_diff_outer_phi_peak = (TH2F*)angle_diff_outer_phi -> Clone();
    // TH2F_threshold_advanced(angle_diff_outer_phi_peak, 0.5);
    TH2F_threshold_advanced_2(angle_diff_outer_phi_peak, 0.5); // todo : threshold ratio can be modified here
    hist_column_content = SumTH2FColumnContent(angle_diff_outer_phi_peak);
    angle_diff_outer_phi_peak_profile =  angle_diff_outer_phi_peak->ProfileX("angle_diff_outer_phi_peak_profile");
    // TGraph * angle_diff_outer_phi_peak_profile_graph = new TGraph();
    point_index = 0;
    for (int i = 0; i < angle_diff_outer_phi_peak_profile->GetNbinsX(); i++){
        if (hist_column_content[i] < 5){continue;} // note : in order to remove some remaining background
        
        angle_diff_outer_phi_peak_profile_graph -> SetPoint(point_index, angle_diff_outer_phi_peak_profile->GetBinCenter(i+1), angle_diff_outer_phi_peak_profile->GetBinContent(i+1));
        // cout<<"("<<angle_diff_outer_phi_peak_profile->GetBinCenter(i+1)<<", "<< angle_diff_outer_phi_peak_profile->GetBinContent(i+1)<<")"<<endl;
        point_index += 1;
    }

    horizontal_fit_angle_diff_outer -> SetParameter(0,0);
    angle_diff_outer_phi_peak_profile_graph -> Fit(horizontal_fit_angle_diff_outer,"NQ","",-M_PI, M_PI);
    // note : ----------- ----------- ----------- ---------- ----------- ----------- ---------- ----------- ----------- ----------- -----------



    angle_diff_inner_phi_peak_profile_graph -> Set(0);
    angle_diff_outer_phi_peak_profile_graph -> Set(0);
    DCA_distance_inner_phi_peak_profile_graph -> Set(0);
    DCA_distance_outer_phi_peak_profile_graph -> Set(0);
    
    if (print_message_opt == true) {cout<<"circle radius : "<<abs(gaus_fit->GetParameter(0) + gaus_fit->GetParameter(3))<<" possible correction angle : "<<gaus_fit->GetParameter(1)<<endl;}
    // return {abs(gaus_fit->GetParameter(0) + gaus_fit->GetParameter(3)), gaus_fit->GetParameter(1)}; // note : gaussian based
    
    // return {
    //     abs(cos_fit->GetParameter(0) - cos_fit->GetParameter(3)), cos_fit->GetParameter(2), // note : cosine wave based
    //     angle_diff_new_bkg_remove -> GetStdDev(), angle_diff_new_bkg_remove -> GetStdDevError() // note : the std dev of the angle diff 1D distribution 
    // }; 
}



void INTTXYvtx::PrintPlotsVTXxy(string sub_out_folder_name, int print_option)
{
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    angle_correlation -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/angle_correlation.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    angle_diff_DCA_dist -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/angle_diff_DCA_dist.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    angle_diff -> SetMinimum(0);
    angle_diff -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/angle_diff.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    angle_diff_inner_phi -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/angle_diff_inner_phi.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    angle_diff_outer_phi -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/angle_diff_outer_phi.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    DCA_point -> Draw("colz0");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DCA_point.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    DCA_distance_inner_phi -> Draw("colz0");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DCA_distance_inner_phi.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear(); 

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    DCA_distance_outer_phi -> Draw("colz0");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DCA_distance_outer_phi.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear(); 
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    DCA_distance -> Draw("hist");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DCA_distance.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear(); 

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    DCA_distance_inner_phi_peak -> SetStats(0);
    DCA_distance_inner_phi_peak -> GetXaxis() -> SetTitle( DCA_distance_inner_phi -> GetXaxis() -> GetTitle() );
    DCA_distance_inner_phi_peak -> GetYaxis() -> SetTitle( DCA_distance_inner_phi -> GetYaxis() -> GetTitle() );
    DCA_distance_inner_phi_peak -> Draw("colz0");
    DCA_distance_inner_phi_peak_profile -> Draw("same");
    // cos_fit -> Draw("l same");
    // gaus_fit -> Draw("l same");
    horizontal_fit_inner -> Draw("l same");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.25, 0.84, Form("#color[2]{Assumed vertex: %.4f cm, %.4f cm}", current_vtxX/10., current_vtxY/10.));
    // draw_text -> DrawLatex(0.25, 0.80, Form("#color[2]{Fit: -%.2f cos(%.2f(x - %.2f)) + %.2f}", cos_fit -> GetParameter(0), cos_fit -> GetParameter(1), cos_fit -> GetParameter(2), cos_fit -> GetParameter(3)));
    draw_text -> DrawLatex(0.25, 0.80, Form("#color[2]{Pol0 fit chi2/NDF: %.3f, fit error: %.4f}", horizontal_fit_inner -> GetChisquare() / double(horizontal_fit_inner -> GetNDF()), horizontal_fit_inner->GetParError(0)));
    c1 -> Print(Form("%s/DCA_distance_inner_phi_peak.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear(); 

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    DCA_distance_outer_phi_peak -> SetStats(0);
    DCA_distance_outer_phi_peak -> GetXaxis() -> SetTitle( DCA_distance_outer_phi -> GetXaxis() -> GetTitle() );
    DCA_distance_outer_phi_peak -> GetYaxis() -> SetTitle( DCA_distance_outer_phi -> GetYaxis() -> GetTitle() );
    DCA_distance_outer_phi_peak -> Draw("colz0");
    DCA_distance_outer_phi_peak_profile -> Draw("same");
    horizontal_fit_outer -> Draw("l same");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.25, 0.84, Form("#color[2]{Assumed vertex: %.4f cm, %.4f cm}", current_vtxX/10., current_vtxY/10.));
    draw_text -> DrawLatex(0.25, 0.80, Form("#color[2]{Pol0 fit chi2/NDF: %.3f, fit error: %.4f}", horizontal_fit_outer -> GetChisquare() / double(horizontal_fit_outer -> GetNDF()), horizontal_fit_outer->GetParError(0)));
    c1 -> Print(Form("%s/DCA_distance_outer_phi_peak.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear(); 
    

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    DCA_distance_inner_X -> Draw("colz0");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DCA_distance_inner_X.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    DCA_distance_inner_Y -> Draw("colz0");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DCA_distance_inner_Y.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    DCA_distance_outer_X -> Draw("colz0");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DCA_distance_outer_X.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    DCA_distance_outer_Y -> Draw("colz0");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DCA_distance_outer_Y.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    angle_diff_new -> SetMinimum(0);
    angle_diff_new -> Draw("hist");
    angle_diff_new_bkg_remove -> Draw("hist same");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.4, 0.80, Form("#color[2]{Dist. StdDev: %.4f}", angle_diff_new_bkg_remove->GetStdDev()));
    c1 -> Print(Form("%s/angle_diff_new.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    angle_diff_radian -> SetMinimum(0);
    angle_diff_radian -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    // gaus_fit_MC -> SetParameters(angle_diff_radian -> GetBinContent(angle_diff_radian -> GetMaximumBin()), angle_diff_radian -> GetMean(), angle_diff_radian -> GetStdDev() * 0.2, 0);

    // angle_diff_radian -> Fit(gaus_fit_MC,"NQ","", -1. * (angle_diff_radian -> GetStdDev() * 0.4), (angle_diff_radian -> GetStdDev() * 0.4 ) );
    // gaus_fit_MC -> SetRange( (gaus_fit_MC -> GetParameter(1) - gaus_fit_MC -> GetParameter(2)), (gaus_fit_MC -> GetParameter(1) + gaus_fit_MC -> GetParameter(2)) );
    
    // draw_text -> DrawLatex(0.2, 0.84, Form("Fit mean: %.4f", gaus_fit_MC -> GetParameter(1)));
    // draw_text -> DrawLatex(0.2, 0.80, Form("Fit width: %.4f", gaus_fit_MC -> GetParameter(2)));

    double angle_diff_radian_bkg_level = get_dist_offset(angle_diff_radian, 15);
    gaus_pol2_fit -> SetParameters(
        angle_diff_radian -> GetBinContent(angle_diff_radian -> GetMaximumBin()) - angle_diff_radian_bkg_level, angle_diff_radian -> GetMean(), angle_diff_radian -> GetStdDev() * 0.2,
        angle_diff_radian_bkg_level, 0, -0.003490, 0
    );
    angle_diff_radian -> Fit(gaus_pol2_fit,"NQ");
    gaus_pol2_fit -> Draw("l same");
    
    // gaus_fit_MC -> Draw("l same");
    c1 -> Print(Form("%s/angle_diff_radian.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    angle_diff_inner_phi_peak -> SetStats(0);
    angle_diff_inner_phi_peak -> GetXaxis() -> SetTitle( angle_diff_inner_phi -> GetXaxis() -> GetTitle() );
    angle_diff_inner_phi_peak -> GetYaxis() -> SetTitle( angle_diff_inner_phi -> GetYaxis() -> GetTitle() );
    angle_diff_inner_phi_peak -> Draw("colz0");
    angle_diff_inner_phi_peak_profile -> Draw("same");
    horizontal_fit_angle_diff_inner -> Draw("l same");
    // gaus_fit_angle_diff -> Draw("lsame");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.25, 0.84, Form("#color[2]{Assumed vertex: %.4f cm, %.4f cm}", current_vtxX/10., current_vtxY/10.));
    draw_text -> DrawLatex(0.25, 0.80, Form("#color[2]{Pol0 fit chi2/NDF: %.3f, fit error: %.4f}", horizontal_fit_angle_diff_inner -> GetChisquare() / double(horizontal_fit_angle_diff_inner -> GetNDF()), horizontal_fit_angle_diff_inner->GetParError(0)));
    c1 -> Print(Form("%s/angle_diff_inner_phi_peak.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear(); 

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    angle_diff_outer_phi_peak -> SetStats(0);
    angle_diff_outer_phi_peak -> GetXaxis() -> SetTitle( angle_diff_outer_phi -> GetXaxis() -> GetTitle() );
    angle_diff_outer_phi_peak -> GetYaxis() -> SetTitle( angle_diff_outer_phi -> GetYaxis() -> GetTitle() );
    angle_diff_outer_phi_peak -> Draw("colz0");
    angle_diff_outer_phi_peak_profile -> Draw("same");
    horizontal_fit_angle_diff_outer -> Draw("l same");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s, peak : %f", plot_text.c_str(), peek));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.25, 0.84, Form("#color[2]{Assumed vertex: %.4f cm, %.4f cm}", current_vtxX/10., current_vtxY/10.));
    draw_text -> DrawLatex(0.25, 0.80, Form("#color[2]{Pol0 fit chi2/NDF: %.3f, fit error: %.4f}", horizontal_fit_angle_diff_outer -> GetChisquare() / double(horizontal_fit_angle_diff_outer -> GetNDF()), horizontal_fit_angle_diff_outer->GetParError(0)));
    c1 -> Print(Form("%s/angle_diff_outer_phi_peak.pdf", sub_out_folder_name.c_str()));
    c1 -> Clear(); 
}

TH1F * INTTXYvtx::PrintPlots_bkgrm_helper(TH1F * hist_in, double signal_region)
{
    c1 -> cd();
    hist_in -> SetMinimum(0);
    hist_in -> SetMaximum( hist_in -> GetBinContent( hist_in -> GetMaximumBin() ) * 1.8);
    hist_in -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    
    double hist_in_bkg_level = get_dist_offset(hist_in, 15);

    bkg_fit_pol2 -> SetParameters(hist_in_bkg_level, 0, -0.0034906585, 0, signal_region);
    bkg_fit_pol2 -> FixParameter(4, signal_region);
    bkg_fit_pol2 -> SetParLimits(2, -100, 0);
    hist_in -> Fit(bkg_fit_pol2,"NQ");

    draw_pol2_line -> SetParameters(bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1), bkg_fit_pol2 -> GetParameter(2), bkg_fit_pol2 -> GetParameter(3));

    TH1F * hist_in_bkgrm = (TH1F*)hist_in -> Clone();
    hist_in_bkgrm -> SetLineColor(8);
    for (int i = 0; i < hist_in_bkgrm -> GetNbinsX(); i++)
    {
        double rest_bincontent = hist_in -> GetBinContent(i+1) - draw_pol2_line -> Eval(hist_in -> GetBinCenter(i+1));
        rest_bincontent = (rest_bincontent < 0) ? 0 : rest_bincontent;
        hist_in_bkgrm -> SetBinContent(i+1, rest_bincontent);
    }
    
    gaus_fit_MC -> SetParameters(hist_in_bkgrm -> GetBinContent(hist_in_bkgrm -> GetMaximumBin()), hist_in_bkgrm -> GetMean(), hist_in_bkgrm -> GetStdDev() * 0.2, 0);
    hist_in_bkgrm -> Fit(gaus_fit_MC,"NQ","", -1. * (hist_in_bkgrm -> GetStdDev() * 0.7), (hist_in_bkgrm -> GetStdDev() * 0.7 ));
    gaus_fit_MC -> SetRange(gaus_fit_MC -> GetParameter(1) - gaus_fit_MC -> GetParameter(2) * 2., gaus_fit_MC -> GetParameter(1) + gaus_fit_MC -> GetParameter(2) * 2.);

    draw_text -> DrawLatex(0.21, 0.86, Form("Fit mean: %.6f", gaus_fit_MC -> GetParameter(1)));
    draw_text -> DrawLatex(0.21, 0.82, Form("Fit width: %.6f", gaus_fit_MC -> GetParameter(2)));
    draw_text -> DrawLatex(0.21, 0.78, Form("Hist StdDev: %.6f", hist_in_bkgrm -> GetStdDev()));

    draw_pol2_line -> Draw("l same");
    hist_in_bkgrm -> Draw("hist same");
    gaus_fit_MC -> Draw("l same");

    c1 -> Print(Form("%s/%s.pdf", out_folder_directory.c_str(), hist_in -> GetName()));
    c1 -> Clear();

    return hist_in_bkgrm;
}

void INTTXYvtx::RunDeltaPhiStudy()
{
    for (int i = 0; i < cluster_pair_vec.size(); i++)
    {
        if ( i % 100000 == 0 ) {cout<<" In RunDeltaPhiStudy, i : "<<i<<endl;}

        Clus_InnerPhi_Offset = (cluster_pair_vec[i].first.y - beam_origin.second < 0) ? atan2(cluster_pair_vec[i].first.y - beam_origin.second, cluster_pair_vec[i].first.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].first.y - beam_origin.second, cluster_pair_vec[i].first.x - beam_origin.first) * (180./TMath::Pi());
        Clus_OuterPhi_Offset = (cluster_pair_vec[i].second.y - beam_origin.second < 0) ? atan2(cluster_pair_vec[i].second.y - beam_origin.second, cluster_pair_vec[i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].second.y - beam_origin.second, cluster_pair_vec[i].second.x - beam_origin.first) * (180./TMath::Pi());

        // note : to have the radian
        Clus_InnerPhi_Offset_radian = atan2(cluster_pair_vec[i].first.y  - beam_origin.second, cluster_pair_vec[i].first.x  - beam_origin.first);
        Clus_OuterPhi_Offset_radian = atan2(cluster_pair_vec[i].second.y - beam_origin.second, cluster_pair_vec[i].second.x - beam_origin.first);

        double DCA_sign = calculateAngleBetweenVectors(
            cluster_pair_vec[i].second.x, cluster_pair_vec[i].second.y,
            cluster_pair_vec[i].first.x, cluster_pair_vec[i].first.y,
            beam_origin.first, beam_origin.second
        );

        final_angle_diff_inner_phi_degree_coarseX -> Fill(Clus_InnerPhi_Offset, Clus_InnerPhi_Offset - Clus_OuterPhi_Offset);
        final_angle_diff_inner_phi_radian_coarseX -> Fill(Clus_InnerPhi_Offset_radian, Clus_InnerPhi_Offset_radian - Clus_OuterPhi_Offset_radian);
        final_angle_diff_radian_before            -> Fill(Clus_InnerPhi_Offset_radian - Clus_OuterPhi_Offset_radian);

        final_angle_diff_radian_DCA_2D_before -> Fill(Clus_InnerPhi_Offset_radian - Clus_OuterPhi_Offset_radian, DCA_sign * 0.1);

        final_DCA_cm_before -> Fill(DCA_sign * 0.1);
        final_DCA_cm_inner_phi_radian_coarseX -> Fill(Clus_InnerPhi_Offset_radian, DCA_sign * 0.1);
        final_DCA_mm_inner_phi_degree_coarseX -> Fill(Clus_InnerPhi_Offset, DCA_sign);

    } // note : end of the loop for the cluster pair

    final_angle_diff_inner_phi_degree_coarseX_peak = (TH2F*)final_angle_diff_inner_phi_degree_coarseX -> Clone();
    TH2F_threshold_advanced(final_angle_diff_inner_phi_degree_coarseX_peak, 0.5); // todo : threshold ratio can be modified here
    final_angle_diff_inner_phi_degree_coarseX_peak_profile =  final_angle_diff_inner_phi_degree_coarseX_peak->ProfileX("final_angle_diff_inner_phi_degree_coarseX_peak_profile");

    final_angle_diff_inner_phi_radian_coarseX_peak = (TH2F*)final_angle_diff_inner_phi_radian_coarseX -> Clone();
    TH2F_threshold_advanced(final_angle_diff_inner_phi_radian_coarseX_peak, 0.5); // todo : threshold ratio can be modified here
    final_angle_diff_inner_phi_radian_coarseX_peak_profile =  final_angle_diff_inner_phi_radian_coarseX_peak->ProfileX("final_angle_diff_inner_phi_radian_coarseX_peak_profile");

    final_DCA_cm_inner_phi_radian_coarseX_peak = (TH2F*)final_DCA_cm_inner_phi_radian_coarseX -> Clone();
    TH2F_threshold_advanced(final_DCA_cm_inner_phi_radian_coarseX_peak, 0.5); // todo : threshold ratio can be modified here
    final_DCA_cm_inner_phi_radian_coarseX_peak_profile =  final_DCA_cm_inner_phi_radian_coarseX_peak->ProfileX("final_DCA_cm_inner_phi_radian_coarseX_peak_profile");

    final_DCA_mm_inner_phi_degree_coarseX_peak = (TH2F*)final_DCA_mm_inner_phi_degree_coarseX -> Clone();
    TH2F_threshold_advanced(final_DCA_mm_inner_phi_degree_coarseX_peak, 0.5); // todo : threshold ratio can be modified here
    final_DCA_mm_inner_phi_degree_coarseX_peak_profile =  final_DCA_mm_inner_phi_degree_coarseX_peak->ProfileX("final_DCA_mm_inner_phi_degree_coarseX_peak_profile");

    cout<<"// for the radian case"<<endl;
    cout<<"vector<double> angle_diff_correction_radian = {";
    for (int i = 0; i < final_angle_diff_inner_phi_radian_coarseX_peak_profile -> GetNbinsX(); i++)
    {
        if (i == final_angle_diff_inner_phi_radian_coarseX_peak_profile -> GetNbinsX() - 1)
        {
            cout<<final_angle_diff_inner_phi_radian_coarseX_peak_profile -> GetBinContent(i+1)<<"};"<<endl;
            break;
        }
        cout<<final_angle_diff_inner_phi_radian_coarseX_peak_profile -> GetBinContent(i+1)<<", ";
    } 

    cout<<"// for the degree case"<<endl;
    cout<<"vector<double> angle_diff_correction_degree = {";
    for (int i = 0; i < final_angle_diff_inner_phi_degree_coarseX_peak_profile -> GetNbinsX(); i++)
    {
        if (i == final_angle_diff_inner_phi_degree_coarseX_peak_profile -> GetNbinsX() - 1)
        {
            cout<<final_angle_diff_inner_phi_degree_coarseX_peak_profile -> GetBinContent(i+1)<<"};"<<endl;
            break;
        }
        cout<<final_angle_diff_inner_phi_degree_coarseX_peak_profile -> GetBinContent(i+1)<<", ";
    }

    // note : after the correction
    for (int i = 0; i < cluster_pair_vec.size(); i++)
    {
        // note : to have the radian
        Clus_InnerPhi_Offset_radian = atan2(cluster_pair_vec[i].first.y  - beam_origin.second, cluster_pair_vec[i].first.x  - beam_origin.first);
        Clus_OuterPhi_Offset_radian = atan2(cluster_pair_vec[i].second.y - beam_origin.second, cluster_pair_vec[i].second.x - beam_origin.first);

        double angle_correction_radian = final_angle_diff_inner_phi_radian_coarseX_peak_profile -> GetBinContent( final_angle_diff_inner_phi_radian_coarseX_peak_profile -> FindBin(Clus_InnerPhi_Offset_radian) );
        double eventual_angle_diff_radian = (Clus_InnerPhi_Offset_radian - Clus_OuterPhi_Offset_radian) - angle_correction_radian;

        // note : unit : mm
        double DCA_sign = calculateAngleBetweenVectors(
            cluster_pair_vec[i].second.x, cluster_pair_vec[i].second.y,
            cluster_pair_vec[i].first.x, cluster_pair_vec[i].first.y,
            beam_origin.first, beam_origin.second
        );

        double DCA_sign_cm_correction = DCA_sign * 0.1 - final_DCA_cm_inner_phi_radian_coarseX_peak_profile -> GetBinContent( final_DCA_cm_inner_phi_radian_coarseX_peak_profile -> FindBin(Clus_InnerPhi_Offset_radian) );
        
        final_angle_diff_radian_post -> Fill( eventual_angle_diff_radian );

        final_angle_diff_radian_DCA_2D_post -> Fill(eventual_angle_diff_radian, DCA_sign_cm_correction);

        final_DCA_cm_post -> Fill(DCA_sign_cm_correction);

    } // note : end of the loop for the cluster pair

    // note : print the plots

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_DCA_cm_inner_phi_radian_coarseX -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_DCA_cm_inner_phi_radian_coarseX.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_DCA_cm_inner_phi_radian_coarseX_peak -> Draw("colz0");
    final_DCA_cm_inner_phi_radian_coarseX_peak_profile -> Draw("same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_DCA_cm_inner_phi_radian_coarseX_peak.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_DCA_mm_inner_phi_degree_coarseX -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_DCA_mm_inner_phi_degree_coarseX.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_DCA_mm_inner_phi_degree_coarseX_peak -> Draw("colz0");
    final_DCA_mm_inner_phi_degree_coarseX_peak_profile -> Draw("same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_DCA_mm_inner_phi_degree_coarseX_peak.pdf", out_folder_directory.c_str()));
    c1 -> Clear();


    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_angle_diff_inner_phi_degree_coarseX -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_angle_diff_inner_phi_degree_coarseX.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_angle_diff_inner_phi_degree_coarseX_peak -> Draw("colz0");
    final_angle_diff_inner_phi_degree_coarseX_peak_profile -> Draw("same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_angle_diff_inner_phi_degree_coarseX_peak.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_angle_diff_inner_phi_radian_coarseX -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_angle_diff_inner_phi_radian_coarseX.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_angle_diff_inner_phi_radian_coarseX_peak -> Draw("colz0");
    final_angle_diff_inner_phi_radian_coarseX_peak_profile -> Draw("same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_angle_diff_inner_phi_radian_coarseX_peak.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_angle_diff_radian_DCA_2D_before -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_angle_diff_radian_DCA_2D_before.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_angle_diff_radian_DCA_2D_post -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_angle_diff_radian_DCA_2D_post.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    TH1F * final_angle_diff_radian_before_bkgrm = PrintPlots_bkgrm_helper(final_angle_diff_radian_before, ana_map_version::signal_region / radian_correction);

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    TH1F * final_angle_diff_radian_post_bkgrm = PrintPlots_bkgrm_helper(final_angle_diff_radian_post, ana_map_version::signal_region / radian_correction);

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    TLegend * legend = new TLegend(0.2,0.75,0.45,0.9);
    // legend -> SetMargin(0);
    legend->SetTextSize(0.03);

    final_angle_diff_radian_before_bkgrm -> SetLineColor(4);
    final_angle_diff_radian_before_bkgrm -> Draw("hist");
    final_angle_diff_radian_post_bkgrm -> SetLineColor(2);
    final_angle_diff_radian_post_bkgrm -> Draw("hist same");

    legend -> AddEntry(final_angle_diff_radian_before_bkgrm, "Original", "f");
    legend -> AddEntry(final_angle_diff_radian_post_bkgrm, "#Delta#phi mean shift", "f");

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    legend -> Draw("same");

    c1 -> Print(Form("%s/final_angle_diff_radian_comp.pdf", out_folder_directory.c_str()));
    c1 -> Clear();
    legend -> Clear();


    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    TH1F * final_DCA_cm_before_bkgrm = PrintPlots_bkgrm_helper(final_DCA_cm_before, 0.03);

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    TH1F * final_DCA_cm_post_bkgrm = PrintPlots_bkgrm_helper(final_DCA_cm_post, 0.03);

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    final_DCA_cm_before_bkgrm -> SetLineColor(4);
    final_DCA_cm_before_bkgrm -> Draw("hist");
    final_DCA_cm_post_bkgrm -> SetLineColor(2);
    final_DCA_cm_post_bkgrm -> Draw("hist same");

    legend -> AddEntry(final_DCA_cm_before_bkgrm, "Original", "f");
    legend -> AddEntry(final_DCA_cm_post_bkgrm, "DCA mean shift", "f");

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    legend -> Draw("same");

    c1 -> Print(Form("%s/final_DCA_cm_comp.pdf", out_folder_directory.c_str()));
    c1 -> Clear();
    legend -> Clear();

    cout<<"End of RunDeltaPhiStudy"<<endl;
}

void INTTXYvtx::ClearHist(int print_option)
{
    angle_correlation -> Reset("ICESM");
    angle_diff_DCA_dist -> Reset("ICESM");
    angle_diff -> Reset("ICESM");    
    DCA_point -> Reset("ICESM");
    DCA_distance -> Reset("ICESM");
    DCA_distance_inner_X -> Reset("ICESM");
    DCA_distance_inner_Y -> Reset("ICESM");
    DCA_distance_outer_X -> Reset("ICESM");
    DCA_distance_outer_Y -> Reset("ICESM");

    DCA_distance_inner_phi -> Reset("ICESM");
    // DCA_distance_inner_phi_peak -> Reset("ICESM");
    DCA_distance_outer_phi -> Reset("ICESM");
    // DCA_distance_outer_phi_peak -> Reset("ICESM");

    angle_diff_inner_phi -> Reset("ICESM");
    angle_diff_outer_phi -> Reset("ICESM");
    
    // angle_diff_inner_phi_peak -> Reset("ICESM");
    // angle_diff_outer_phi_peak -> Reset("ICESM");

    angle_diff_new -> Reset("ICESM");
    angle_diff_radian -> Reset("ICESM");
    // angle_diff_new_bkg_remove -> Reset("ICESM");

    delete angle_diff_new_bkg_remove;
    delete DCA_distance_inner_phi_peak;
    delete DCA_distance_outer_phi_peak;
    delete angle_diff_inner_phi_peak;
    delete angle_diff_outer_phi_peak;
}

void INTTXYvtx::EndRun()
{
    inner_pos_xy -> Reset("ICESM");
    outer_pos_xy -> Reset("ICESM");
    inner_outer_pos_xy -> Reset("ICESM");
    N_cluster_correlation -> Reset("ICESM");
    N_cluster_correlation_close -> Reset("ICESM");

    // file_out -> cd();
    // tree_out -> SetDirectory(file_out);
    // tree_out -> Write();
    // file_out -> Close();

    // c1 -> Delete();
    // ltx -> Delete();
    // draw_text -> Delete();
    // cos_fit -> Delete();
    // gaus_fit -> Delete();
    // gaus_fit_angle_diff -> Delete();

    // horizontal_fit_inner -> Delete();
    // horizontal_fit_angle_diff_inner -> Delete();
    // horizontal_fit_outer -> Delete();
    // horizontal_fit_angle_diff_outer -> Delete();

    delete gROOT->FindObject("angle_diff");
    delete gROOT->FindObject("angle_diff_new");

    // angle_correlation -> Delete();
    // angle_diff_DCA_dist -> Delete();
    // angle_diff -> Delete();
    // angle_diff_new -> Delete();
    // angle_diff_new_bkg_remove -> Delete();
    // inner_pos_xy -> Delete();
    // outer_pos_xy -> Delete();
    // inner_outer_pos_xy -> Delete();
    // DCA_point -> Delete();
    // DCA_distance -> Delete();
    // N_cluster_correlation -> Delete();
    // N_cluster_correlation_close -> Delete();
    
    // DCA_distance_inner_X -> Delete();
    // DCA_distance_inner_Y -> Delete();
    // DCA_distance_outer_X -> Delete();
    // DCA_distance_outer_Y -> Delete();

    // DCA_distance_inner_phi -> Delete();
    // DCA_distance_inner_phi_peak -> Delete();
    // DCA_distance_inner_phi_peak_profile -> Delete();
    // DCA_distance_outer_phi -> Delete();
    // DCA_distance_outer_phi_peak -> Delete();
    // DCA_distance_outer_phi_peak_profile -> Delete();

    // angle_diff_inner_phi -> Delete();
    // angle_diff_inner_phi_peak -> Delete();
    // angle_diff_inner_phi_peak_profile -> Delete();
    // angle_diff_outer_phi -> Delete();
    // angle_diff_outer_phi_peak -> Delete();
    // angle_diff_outer_phi_peak_profile -> Delete();

    // DCA_distance_inner_phi_peak_final -> Delete();
    // angle_diff_inner_phi_peak_final -> Delete();

    return;
}

void INTTXYvtx::TH2F_threshold(TH2F * hist, double threshold)
{
    double max_cut = hist -> GetMaximum() * threshold;

    for (int xi = 0; xi < hist -> GetNbinsX(); xi++){
        for(int yi = 0; yi < hist -> GetNbinsY(); yi++){
            if (hist -> GetBinContent(xi + 1, yi +1) < max_cut){ hist -> SetBinContent(xi + 1, yi +1, 0); }
        }
    }
}

void INTTXYvtx::TH2F_threshold_advanced(TH2F * hist, double threshold)
{
    double big_num, max_cut;

    // note : this function removes the background of the 2D histogram by the following method
    // note : 1. find the maximum bin content of each column and then time with the threshold ratio
    // note : so the background removal is performed column by column

    for (int xi = 0; xi < hist -> GetNbinsX(); xi++){

        for(int yi = 0; yi < hist -> GetNbinsY(); yi++){
            if (yi == 0) {big_num = hist -> GetBinContent(xi + 1, yi +1);}
            else 
            {
                if (hist -> GetBinContent(xi + 1, yi +1) > big_num) {big_num = hist -> GetBinContent(xi + 1, yi +1);}
            }
        }

        max_cut = big_num * threshold;

        for(int yi = 0; yi < hist -> GetNbinsY(); yi++){
            if (hist -> GetBinContent(xi + 1, yi +1) < max_cut){ hist -> SetBinContent(xi + 1, yi +1, 0); }
        }
    }
}

void INTTXYvtx::TH2F_threshold_advanced_row(TH2F * hist, double threshold)
{
    double big_num, max_cut;

    // note : this function removes the background of the 2D histogram by the following method
    // note : 1. find the maximum bin content of each row and then time with the threshold ratio
    // note : so the background removal is performed row by row

    for (int yi = 0; yi < hist -> GetNbinsY(); yi++)
    {
        for(int xi = 0; xi < hist -> GetNbinsX(); xi++)
        {
            if (xi == 0) {big_num = hist -> GetBinContent(xi + 1, yi +1);}
            else 
            {
                if (hist -> GetBinContent(xi + 1, yi +1) > big_num) {big_num = hist -> GetBinContent(xi + 1, yi +1);}
            }
        }

        max_cut = big_num * threshold;

        for(int xi = 0; xi < hist -> GetNbinsX(); xi++)
        {
            if (hist -> GetBinContent(xi + 1, yi +1) < max_cut){ hist -> SetBinContent(xi + 1, yi +1, 0); }
        }
    }
}

void INTTXYvtx::TH2F_threshold_advanced_2(TH2F * hist, double threshold)
{
    // note : this function is to remove the background of the 2D histogram
    // note : but the threshold is given by average of the contents of the top "chosen_bin" bins and timing the threshold
    double max_cut = 0;
    int chosen_bin = 7;

    vector<float> all_bin_content_vec; all_bin_content_vec.clear();
    for (int xi = 0; xi < hist -> GetNbinsX(); xi++){
        for(int yi = 0; yi < hist -> GetNbinsY(); yi++){
            all_bin_content_vec.push_back(hist -> GetBinContent(xi + 1, yi +1));
        }
    }
    vector<unsigned long> ind(all_bin_content_vec.size(),0);
    TMath::Sort(all_bin_content_vec.size(), &all_bin_content_vec[0], &ind[0]);
    for (int i = 0; i < chosen_bin; i++) {max_cut += all_bin_content_vec[ind[i]]; /*cout<<"test : "<<all_bin_content_vec[ind[i]]<<endl;*/}

    max_cut = (max_cut / double(chosen_bin)) * threshold;

    for (int xi = 0; xi < hist -> GetNbinsX(); xi++){
        for(int yi = 0; yi < hist -> GetNbinsY(); yi++){
            if (hist -> GetBinContent(xi + 1, yi +1) < max_cut){ hist -> SetBinContent(xi + 1, yi +1, 0); }
        }
    }
}

double INTTXYvtx::get_radius(double x, double y)
{
    return sqrt(pow(x,2)+pow(y,2));
}

type_pos INTTXYvtx::PolarToCartesian(double radius, double angleDegrees) {
    // Convert angle from degrees to radians
    double angleRadians = angleDegrees * M_PI / 180.0;

    // Calculate Cartesian coordinates
    double x = radius * cos(angleRadians);
    double y = radius * sin(angleRadians);

    return {x, y};
}

void INTTXYvtx::Characterize_Pad(TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0)
{
	if (setgrid_bool == true) {pad -> SetGrid (1, 1);}
	pad -> SetLeftMargin   (left);
	pad -> SetRightMargin  (right);
	pad -> SetTopMargin    (top);
	pad -> SetBottomMargin (bottom);
    pad -> SetTicks(1,1);
    if (set_logY == true)
    {
        pad -> SetLogy (1);
    }
	
}

std::vector<double> INTTXYvtx::calculateDistanceAndClosestPoint(double x1, double y1, double x2, double y2, double target_x, double target_y) 
{
    
    if (x1 != x2)
    {
        // Calculate the slope and intercept of the line passing through (x1, y1) and (x2, y2)
        double a = (y2 - y1) / (x2 - x1);
        double b = y1 - a * x1;

        // cout<<"slope : y="<<a<<"x+"<<b<<endl;
        
        // Calculate the closest distance from (target_x, target_y) to the line y = ax + b
        double closest_distance = std::abs(a * target_x - target_y + b) / std::sqrt(a * a + 1);

        // Calculate the coordinates of the closest point (Xc, Yc) on the line y = ax + b
        double Xc = (target_x + a * target_y - a * b) / (a * a + 1);
        double Yc = a * Xc + b;

        return { closest_distance, Xc, Yc };
    }
    else 
    {
        double closest_distance = std::abs(x1 - target_x);
        double Xc = x1;
        double Yc = target_y;

        return { closest_distance, Xc, Yc };
    }
    
    
}

// note : Function to calculate the angle between two vectors in degrees using the cross product
double INTTXYvtx::calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY) {
    // Calculate the vectors vector_1 (point_1 to point_2) and vector_2 (point_1 to target)
    double vector1X = x2 - x1;
    double vector1Y = y2 - y1;

    double vector2X = targetX - x1;
    double vector2Y = targetY - y1;

    // Calculate the cross product of vector_1 and vector_2 (z-component)
    double crossProduct = vector1X * vector2Y - vector1Y * vector2X;
    
    // cout<<" crossProduct : "<<crossProduct<<endl;

    // Calculate the magnitudes of vector_1 and vector_2
    double magnitude1 = std::sqrt(vector1X * vector1X + vector1Y * vector1Y);
    double magnitude2 = std::sqrt(vector2X * vector2X + vector2Y * vector2Y);

    // Calculate the angle in radians using the inverse tangent of the cross product and dot product
    double dotProduct = vector1X * vector2X + vector1Y * vector2Y;

    double angleInRadians = std::atan2(std::abs(crossProduct), dotProduct);
    // Convert the angle from radians to degrees and return it
    double angleInDegrees = angleInRadians * 180.0 / M_PI;
    
    double angleInRadians_new = std::asin( crossProduct/(magnitude1*magnitude2) );
    double angleInDegrees_new = angleInRadians_new * 180.0 / M_PI;
    
    // cout<<"angle : "<<angleInDegrees_new<<endl;

    double DCA_distance = sin(angleInRadians_new) * magnitude2;

    return DCA_distance;
}


pair<type_pos,type_pos> INTTXYvtx::GetTangentPointsAtCircle(double CenterX, double CenterY, double R, double XX, double YY) {
    
    double XT0; 
    double YT0; 
    double XT1; 
    double YT1;
    
    if (R == 0) {
        // This behavior can be modified
        cout<<"In INTTXYvtx, the input vertex is zero"<<endl;
        return {{-999.,-999.},{-999.,-999.}};
    }

    double nx = (XX - CenterX) / R; // Shift and scale
    double ny = (YY - CenterY) / R;
    double xy = nx * nx + ny * ny;

    if (abs(xy - 1.0) < 1e-10) { // Point lies at circumference, one tangent
        XT0 = XX;
        YT0 = YY;
        return {{XT0,YT0},{XT0,YT0}};
    }

    if (xy < 1.0){ // Point lies inside the circle, no tangents
        cout<<"IN INTTXYvtx, the point is inside the circle, no tangnet points"<<endl;
        return {{-999.,-999.},{-999.,-999.}};
    }

    // Common case, two tangents
    int Result = 2;
    double D = ny * sqrt(xy - 1);
    double tx0 = (nx - D) / xy;
    double tx1 = (nx + D) / xy;
    
    if (abs(ny) > 1e-10) { // Common case
        YT0 = CenterY + R * (1 - tx0 * nx) / ny;
        YT1 = CenterY + R * (1 - tx1 * nx) / ny;
    } else { // Point at the center horizontal, Y=0
        D = R * sqrt(1 - tx0 * tx0);
        YT0 = CenterY + D;
        YT1 = CenterY - D;
    }

    // Restore scale and position
    XT0 = CenterX + R * tx0;
    XT1 = CenterX + R * tx1;

    return {{XT0,YT0},{XT1,YT1}};
}

pair<type_pos,type_pos> INTTXYvtx::findIntersectionPoints(double A, double mu, double sigma, double B, double C) {
    // Solve for x in the equation A * e^(-((x - mu)^2) / (2 * sigma^2)) + B = C
    // This involves solving a quadratic equation, and the solutions will be the x values of intersection points

    double delta = mu * mu - 2 * sigma * sigma * log((C - B) / A);
    
    // Check if there are real solutions
    if (delta >= 0) {
        double x1 = mu + sqrt(delta);
        double x2 = mu - sqrt(delta);

        // Calculate corresponding y values
        double y1 = A * exp(-pow(x1 - mu, 2) / (2 * sigma * sigma)) + B;
        double y2 = A * exp(-pow(x2 - mu, 2) / (2 * sigma * sigma)) + B;
        
        return {{x1, y1}, {x2, y2}};

    } else {
        cout << "No real intersection points." << endl;
        return {{-999.,-999.},{-999.,-999.}};
    }
}


//  note : N group, group size, group tag, group width ?  // note : {group size, group entry, group tag, group widthL, group widthR}
// note : {N_group, ratio (if two), peak widthL, peak widthR}
vector<double> INTTXYvtx::find_Ngroup(TH1F * hist_in)
{
    double Highest_bin_Content  = hist_in -> GetBinContent(hist_in -> GetMaximumBin());
    double Highest_bin_Center   = hist_in -> GetBinCenter(hist_in -> GetMaximumBin());

    int group_Nbin = 0;
    int peak_group_ID;
    double group_entry = 0;
    double peak_group_ratio;
    vector<int> group_Nbin_vec; group_Nbin_vec.clear();
    vector<double> group_entry_vec; group_entry_vec.clear();
    vector<double> group_widthL_vec; group_widthL_vec.clear();
    vector<double> group_widthR_vec; group_widthR_vec.clear();

    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        // todo : the background rejection is here : Highest_bin_Content/2. for the time being
        double bin_content = ( hist_in -> GetBinContent(i+1) <= Highest_bin_Content/2.) ? 0. : ( hist_in -> GetBinContent(i+1) - Highest_bin_Content/2. );

        if (bin_content != 0){
            
            if (group_Nbin == 0) {
                group_widthL_vec.push_back(hist_in -> GetBinCenter(i+1) - (hist_in -> GetBinWidth(i+1)/2.));
            }

            group_Nbin += 1; 
            group_entry += bin_content;
        }
        else if (bin_content == 0 && group_Nbin != 0){
            group_widthR_vec.push_back(hist_in -> GetBinCenter(i+1) - (hist_in -> GetBinWidth(i+1)/2.));
            group_Nbin_vec.push_back(group_Nbin);
            group_entry_vec.push_back(group_entry);
            group_Nbin = 0;
            group_entry = 0;
        }
    }
    if (group_Nbin != 0) {
        group_Nbin_vec.push_back(group_Nbin);
        group_entry_vec.push_back(group_entry);
        group_widthR_vec.push_back(hist_in -> GetXaxis()->GetXmax());
    } // note : the last group at the edge

    // note : find the peak group
    for (int i = 0; i < group_Nbin_vec.size(); i++){
        if (group_widthL_vec[i] < Highest_bin_Center && Highest_bin_Center < group_widthR_vec[i]){
            peak_group_ID = i;
            break;
        }
    }
    
    
    peak_group_ratio = group_entry_vec[peak_group_ID] / (accumulate( group_entry_vec.begin(), group_entry_vec.end(), 0.0 ));

    // for (int i = 0; i < group_Nbin_vec.size(); i++)
    // {
    //     cout<<" "<<endl;
    //     cout<<"group size : "<<group_Nbin_vec[i]<<endl;
    //     cout<<"group entry : "<<group_entry_vec[i]<<endl;
    //     cout<<group_widthL_vec[i]<<" "<<group_widthR_vec[i]<<endl;
    // }

    // cout<<" "<<endl;
    // cout<<"N group : "<<group_Nbin_vec.size()<<endl;
    // cout<<"Peak group ID : "<<peak_group_ID<<endl;
    // cout<<"peak group width : "<<group_widthL_vec[peak_group_ID]<<" "<<group_widthR_vec[peak_group_ID]<<endl;
    // cout<<"ratio : "<<peak_group_ratio<<endl;
    
    // note : {N_group, ratio (if two), peak widthL, peak widthR}
    return {double(group_Nbin_vec.size()), peak_group_ratio, group_widthL_vec[peak_group_ID], group_widthR_vec[peak_group_ID]};
}

void INTTXYvtx::Hist_1D_bkg_remove(TH1F * hist_in, double factor)
{   
    // todo : N bins considered to be used in the background quantification
    vector<double> Nbin_content_vec; Nbin_content_vec.clear();
    for (int i = hist_in -> GetNbinsX() - 5; i < hist_in -> GetNbinsX(); i++){Nbin_content_vec.push_back(hist_in -> GetBinContent(i+1));} 
    double bkg_level = accumulate( Nbin_content_vec.begin(), Nbin_content_vec.end(), 0.0 ) / Nbin_content_vec.size();
    // cout<<"test, bkg cut : "<<bkg_level * factor<<endl;

    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        // note : the background rejection is here : bkg_level * 1.5 for the time being
        double bin_content = ( hist_in -> GetBinContent(i+1) <= bkg_level * factor) ? 0. : ( hist_in -> GetBinContent(i+1) - bkg_level * factor );
        hist_in -> SetBinContent(i+1, bin_content);
    }
}

void INTTXYvtx::DrawTGraphErrors(vector<double> x_vec, vector<double> y_vec, vector<double> xE_vec, vector<double> yE_vec, string output_directory, vector<string> plot_name)
{
    c1 -> cd();

    TGraphErrors * g = new TGraphErrors(x_vec.size(), &x_vec[0], &y_vec[0], &xE_vec[0], &yE_vec[0]);
    g -> SetMarkerStyle(20);
    g -> SetMarkerSize(1.5);
    g -> SetMarkerColor(1);
    g -> SetLineColor(1);
    g -> GetXaxis() -> SetTitle(plot_name[1].c_str());
    g -> GetYaxis() -> SetTitle(plot_name[2].c_str());
    if (plot_name.size() == 4){g -> Draw(plot_name[3].c_str());}
    else {g -> Draw("AP");}

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), plot_name[0].c_str()));
    c1 -> Clear();
    g -> Delete();
}

void INTTXYvtx::Draw2TGraph(vector<double> x1_vec, vector<double> y1_vec, vector<double> x2_vec, vector<double> y2_vec, string output_directory, vector<string> plot_name)
{
    c1 -> cd();

    c1 -> SetLogy(1);

    TGraph * g1 = new TGraph(x1_vec.size(), &x1_vec[0], &y1_vec[0]);
    g1 -> SetMarkerStyle(5);
    g1 -> SetMarkerSize(1);
    g1 -> SetMarkerColor(1);
    g1 -> SetLineColor(1);
    g1 -> GetXaxis() -> SetTitle(plot_name[1].c_str());
    g1 -> GetYaxis() -> SetTitle(plot_name[2].c_str());
    g1 -> GetXaxis() -> SetNdivisions(505);
    g1 -> GetXaxis() -> SetLimits(-1, x1_vec[x1_vec.size()-1] + 1);
    g1 -> Draw("AP");

    TGraph * g2 = new TGraph(x2_vec.size(), &x2_vec[0], &y2_vec[0]);
    g2 -> SetMarkerStyle(5);
    g2 -> SetMarkerSize(1);
    g2 -> SetMarkerColor(2);
    g2 -> SetLineColor(2);
    g2 -> Draw("PL same");

    TLegend * legend = new TLegend(0.4,0.75,0.65,0.9);
    // legend -> SetMargin(0);
    legend->SetTextSize(0.03);
    legend -> AddEntry(g1, "Tested vertex candidates", "p");
    legend -> AddEntry(g2, "Better performed candidates", "p");
    legend -> Draw("same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), plot_name[0].c_str()));
    c1 -> Clear();
    g1 -> Delete();
    g2 -> Delete();

    c1 -> SetLogy(0);

    return;
}

void INTTXYvtx::DrawTH2F(TH2F * hist_in, string output_directory, vector<string> plot_name)
{
    c1 -> cd();

    hist_in -> SetStats(0);
    hist_in -> GetXaxis() -> SetTitle(plot_name[1].c_str());
    hist_in -> GetYaxis() -> SetTitle(plot_name[2].c_str());
    hist_in -> GetZaxis() -> SetTitle(plot_name[3].c_str());
    hist_in -> GetXaxis() -> SetNdivisions(505);
    hist_in -> Draw("colz0");


    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), plot_name[0].c_str()));
    c1 -> Clear();
}

vector<double> INTTXYvtx::SumTH2FColumnContent(TH2F * hist_in)
{
    vector<double> sum_vec; sum_vec.clear();
    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        double sum = 0;
        for (int j = 0; j < hist_in -> GetNbinsY(); j++){
            sum += hist_in -> GetBinContent(i+1, j+1);
        }
        sum_vec.push_back(sum);
    }
    return sum_vec;
}

vector<double> INTTXYvtx::SumTH2FColumnContent_row(TH2F * hist_in)
{
    vector<double> sum_vec; sum_vec.clear();
    for (int i = 0; i < hist_in -> GetNbinsY(); i++){
        double sum = 0;
        for (int j = 0; j < hist_in -> GetNbinsX(); j++){
            sum += hist_in -> GetBinContent(j+1, i+1);
        }
        sum_vec.push_back(sum);
    }
    return sum_vec;
}


vector<pair<double,double>> INTTXYvtx::Get4vtx(pair<double,double> origin, double length)
{
    vector<pair<double,double>> unit_vtx = {{1,1},{-1,1},{-1,-1},{1,-1}};
    vector<pair<double,double>> vec_out; vec_out.clear();

    for (pair i1 : unit_vtx)
    {
        vec_out.push_back({i1.first * length + origin.first, i1.second * length + origin.second});
    }

    return vec_out;
}

vector<pair<double,double>> INTTXYvtx::Get4vtxAxis(pair<double,double> origin, double length)
{
    vector<pair<double,double>> unit_vtx = {{1,0},{-1,0},{0,1},{0,-1}}; //note : X axis right and left, Y axis up and down
    vector<pair<double,double>> vec_out; vec_out.clear();

    for (pair i1 : unit_vtx)
    {
        vec_out.push_back({i1.first * length + origin.first, i1.second * length + origin.second});
    }

    return vec_out;
}

void INTTXYvtx::TH2F_FakeClone(TH2F*hist_in, TH2F*hist_out)
{
    if (hist_in -> GetNbinsX() != hist_out -> GetNbinsX() || hist_in -> GetNbinsY() != hist_out -> GetNbinsY())
    {
        cout<<"In INTTXYvtx::TH2F_FakeClone, the input and output histogram have different binning!"<<endl;
        return;
    }

    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        for (int j = 0; j < hist_in -> GetNbinsY(); j++){
            hist_out -> SetBinContent(i+1, j+1, hist_in -> GetBinContent(i+1, j+1));
        }
    }
}

void INTTXYvtx::TH1F_FakeClone(TH1F*hist_in, TH1F*hist_out)
{
    if (hist_in -> GetNbinsX() != hist_out -> GetNbinsX())
    {
        cout<<"In INTTXYvtx::TH1F_FakeClone, the input and output histogram have different binning!"<<endl;
        return;
    }

    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        hist_out -> SetBinContent(i+1, hist_in -> GetBinContent(i+1));
    }
}

// void INTTXYvtx::TH2F_FakeRebin(TH2F*hist_in, TH2F*hist_out)
// {
    
// }

int INTTXYvtx::find_quadrant(pair<double,double> origin, pair<double,double> check_point)
{
    int check_x, check_y;

    if (origin.first < check_point.first){ check_x = 1; }
    else if (origin.first == check_point.first) { check_x = 0; }
    else { check_x = -1; }

    if (origin.second < check_point.second){ check_y = 1; }
    else if (origin.second == check_point.second){ check_y = 0; }
    else { check_y = -1; }

    if (check_x != 0 && check_y != 0) // note : normal 4 quadrants 
    {
        return axis_quadrant_map[{check_x,check_y}];    
    }
    else if (check_x == 0 && check_y != 0) // note : on the Y axis 
    {
        return (check_y > 0) ? 4 : 5; // note : 4 is up, 5 is down, along the Y axis
    }
    else if (check_x != 0 && check_y == 0) // note : on the X axis 
    {
        return (check_x > 0) ? 6 : 7; // note : 6 is right, 7 is left, along the X axis
    }
    else // note : the two position are identical!
    {
        return 8;
    }
}

void INTTXYvtx::TH2FSampleLineFill(TH2F * hist_in, double segmentation, std::pair<double,double> inner_clu, std::pair<double,double> outer_clu)
{
    double x_min = hist_in -> GetXaxis() -> GetXmin();
    double x_max = hist_in -> GetXaxis() -> GetXmax();
    double y_min = hist_in -> GetYaxis() -> GetXmin();
    double y_max = hist_in -> GetYaxis() -> GetXmax();

    double seg_x, seg_y;
    double angle;
    int n_seg = 0;

    while (true)
    {
        angle = atan2(inner_clu.second-outer_clu.second, inner_clu.first-outer_clu.first);
        seg_x = (n_seg * segmentation) * cos(angle) + outer_clu.first; // note : atan2(y,x), point.first is the radius
        seg_y = (n_seg * segmentation) * sin(angle) + outer_clu.second;
        
        if ( (seg_x > x_min && seg_x < x_max && seg_y > y_min && seg_y < y_max) != true ) {break;}
        hist_in -> Fill(seg_x, seg_y);
        n_seg += 1;
    }

    n_seg = 1;
    while (true)
    {
        angle = atan2(inner_clu.second-outer_clu.second, inner_clu.first-outer_clu.first);
        seg_x = (-1 * n_seg * segmentation) * cos(angle) + outer_clu.first; // note : atan2(y,x), point.first is the radius
        seg_y = (-1 * n_seg * segmentation) * sin(angle) + outer_clu.second;
        
        if ( (seg_x > x_min && seg_x < x_max && seg_y > y_min && seg_y < y_max) != true ) {break;}
        hist_in -> Fill(seg_x, seg_y);
        n_seg += 1;
    }
}

// note : the input is still mm
vector<pair<double,double>> INTTXYvtx::FillLine_FindVertex(pair<double,double> window_center, double segmentation, double window_width, int N_bins, bool draw_plot)
{
    // note : set the histogram to be cm
    xy_hist = new TH2F(
        "",
        "xy_hist", 
        N_bins, 
        (-1 * window_width / 2. + window_center.first) * 0.1, 
        (window_width / 2. + window_center.first) * 0.1, 
        N_bins, 
        (-1 * window_width / 2. + window_center.second) * 0.1, 
        (window_width / 2. + window_center.second) * 0.1
    );
    xy_hist -> GetXaxis() -> SetTitle("X axis [cm]");
    xy_hist -> GetYaxis() -> SetTitle("Y axis [cm]");

    xy_hist_bkgrm = new TH2F(
        "",
        "xy_hist_bkgrm", 
        N_bins, 
        (-1 * window_width / 2. + window_center.first) * 0.1, 
        (window_width / 2. + window_center.first) * 0.1, 
        N_bins, 
        (-1 * window_width / 2. + window_center.second) * 0.1, 
        (window_width / 2. + window_center.second) * 0.1
    );
    xy_hist_bkgrm -> GetXaxis() -> SetTitle("X axis [cm]");
    xy_hist_bkgrm -> GetYaxis() -> SetTitle("Y axis [cm]");

    // cout<<"test test size and bin of the hist xy_hist : "<<xy_hist -> GetNbinsX()<<" "<<xy_hist -> GetNbinsY()<<endl;
    // cout<<"test test bin width of the hist xy_hist : "<<xy_hist -> GetXaxis() -> GetBinWidth(1)<<" "<<xy_hist -> GetYaxis() -> GetBinWidth(1)<<endl;
    // cout<<"draw_plot status : "<<draw_plot<<endl;

    
    for (int i = 0; i < cluster_pair_vec.size(); i++)
    {
        vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
            cluster_pair_vec[i].first.x, cluster_pair_vec[i].first.y,
            cluster_pair_vec[i].second.x, cluster_pair_vec[i].second.y,
            window_center.first, window_center.second
        );

        double DCA_sign = calculateAngleBetweenVectors(
            cluster_pair_vec[i].second.x, cluster_pair_vec[i].second.y,
            cluster_pair_vec[i].first.x, cluster_pair_vec[i].first.y,
            window_center.first, window_center.second
        );

        if (DCA_info_vec[0] != fabs(DCA_sign) && fabs( DCA_info_vec[0] - fabs(DCA_sign) ) > 0.1){
            cout<<"different DCA : "<<DCA_info_vec[0]<<" "<<DCA_sign<<" diff : "<<DCA_info_vec[0] - fabs(DCA_sign)<<endl;
        }

        Clus_InnerPhi_Offset = (cluster_pair_vec[i].first.y - window_center.second < 0) ? atan2(cluster_pair_vec[i].first.y - window_center.second, cluster_pair_vec[i].first.x - window_center.first) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].first.y - window_center.second, cluster_pair_vec[i].first.x - window_center.first) * (180./TMath::Pi());
        Clus_OuterPhi_Offset = (cluster_pair_vec[i].second.y - window_center.second < 0) ? atan2(cluster_pair_vec[i].second.y - window_center.second, cluster_pair_vec[i].second.x - window_center.first) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].second.y - window_center.second, cluster_pair_vec[i].second.x - window_center.first) * (180./TMath::Pi());

        if (fabs(Clus_InnerPhi_Offset - Clus_OuterPhi_Offset) < 5)
        {
            TH2FSampleLineFill(xy_hist, segmentation * 0.1, {(cluster_pair_vec[i].first.x) * 0.1, (cluster_pair_vec[i].first.y) * 0.1}, {(DCA_info_vec[1]) * 0.1, (DCA_info_vec[2]) * 0.1});
            // note : the DCA cut may be biased since the DCA was calculated with respect to the vertex calculated by the quadrant method
            // if (DCA_cut.first < DCA_sign && DCA_sign < DCA_cut.second)
            // {
            //     TH2FSampleLineFill(xy_hist, segmentation, {cluster_pair_vec[i].first.x, cluster_pair_vec[i].first.y}, {DCA_info_vec[1], DCA_info_vec[2]});
            // }
        }

    }

    // note : try to remove the background
    TH2F_FakeClone(xy_hist,xy_hist_bkgrm);
    TH2F_threshold_advanced_2(xy_hist_bkgrm, 0.7);

    // note : the unit is cm here
    double reco_vtx_x     = xy_hist_bkgrm->GetMean(1); // + xy_hist_bkgrm -> GetXaxis() -> GetBinWidth(1) / 2.; // note : the TH2F calculate the GetMean based on the bin center, no need to apply additional offset
    double reco_vtx_y     = xy_hist_bkgrm->GetMean(2); // + xy_hist_bkgrm -> GetYaxis() -> GetBinWidth(1) / 2.; // note : the TH2F calculate the GetMean based on the bin center, no need to apply additional offset
    double reco_vtx_x_err = xy_hist_bkgrm->GetMeanError(1);
    double reco_vtx_y_err = xy_hist_bkgrm->GetMeanError(2);
    double reco_vtx_x_std = xy_hist_bkgrm->GetStdDev(1);
    double reco_vtx_y_std = xy_hist_bkgrm->GetStdDev(2);

    // cout<<"test : in the line filled, the process is almost done"<<endl;

    if (draw_plot)
    {
        TGraph * reco_vertex_gr = new TGraph(); 
        reco_vertex_gr -> SetMarkerStyle(50);
        reco_vertex_gr -> SetMarkerColor(2);
        reco_vertex_gr -> SetMarkerSize(1);
        reco_vertex_gr -> SetPoint(reco_vertex_gr -> GetN(), reco_vtx_x, reco_vtx_y); // note : change the unit to cm

        // note : change the unit to cm, for the presentation
        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        xy_hist -> SetStats(0);
        // xy_hist -> GetXaxis() -> SetLimits( xy_hist -> GetXaxis() -> GetBinLowEdge(1) * 0.1, (xy_hist -> GetXaxis() -> GetBinLowEdge( xy_hist -> GetNbinsX() ) + xy_hist -> GetXaxis() -> GetBinWidth( xy_hist -> GetNbinsX() )) * 0.1 );
        // xy_hist -> GetYaxis() -> SetLimits( xy_hist -> GetYaxis() -> GetBinLowEdge(1) * 0.1, (xy_hist -> GetYaxis() -> GetBinLowEdge( xy_hist -> GetNbinsY() ) + xy_hist -> GetYaxis() -> GetBinWidth( xy_hist -> GetNbinsY() )) * 0.1 );
        xy_hist -> GetXaxis() -> SetNdivisions(505);
        xy_hist -> Draw("colz0");
        // draw_text -> DrawLatex(0.21, 0.71+0.13, Form("Vertex of the Run: %.3f mm, %.3f mm", reco_vtx_x, reco_vtx_y));
        // draw_text -> DrawLatex(0.21, 0.67+0.13, Form("Vertex error: %.3f mm, %.3f mm", xy_hist_bkgrm->GetMeanError(1), xy_hist_bkgrm->GetMeanError(2)));
        // reco_vertex_gr -> Draw("p same");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Run_xy_hist.pdf",out_folder_directory.c_str()));
        c1 -> Clear();

        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        xy_hist_bkgrm -> SetStats(0);
        // xy_hist_bkgrm -> GetYaxis() -> SetLimits( xy_hist_bkgrm -> GetYaxis() -> GetBinLowEdge(1) * 0.1, (xy_hist_bkgrm -> GetYaxis() -> GetBinLowEdge( xy_hist_bkgrm -> GetNbinsY() ) + xy_hist_bkgrm -> GetYaxis() -> GetBinWidth( xy_hist_bkgrm -> GetNbinsY() )) * 0.1 );
        // xy_hist_bkgrm -> GetXaxis() -> SetLimits( xy_hist_bkgrm -> GetXaxis() -> GetBinLowEdge(1) * 0.1, (xy_hist_bkgrm -> GetXaxis() -> GetBinLowEdge( xy_hist_bkgrm -> GetNbinsX() ) + xy_hist_bkgrm -> GetXaxis() -> GetBinWidth( xy_hist_bkgrm -> GetNbinsX() )) * 0.1 );
        xy_hist_bkgrm -> GetXaxis() -> SetNdivisions(505);
        xy_hist_bkgrm -> Draw("colz0");
        draw_text -> DrawLatex(0.21, 0.71+0.13, Form("Vertex of the Run: %.5f cm, %.5f cm", reco_vtx_x, reco_vtx_y));
        draw_text -> DrawLatex(0.21, 0.67+0.13, Form("Vertex error: %.5f cm, %.5f cm", reco_vtx_x_err, reco_vtx_y_err));
        reco_vertex_gr -> Draw("p same");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Run_xy_hist_bkgrm.pdf",out_folder_directory.c_str()));
        c1 -> Clear();

        // cout<<"test : hello, can you see me ?"<<endl;
    }

    // note: change the unit from cm to mm
    return {{reco_vtx_x * 10.,reco_vtx_y * 10.}, {reco_vtx_x_err * 10.,reco_vtx_y_err * 10.}, {reco_vtx_x_std * 10.,reco_vtx_y_std * 10.}};   
}

// todo : the input histogram should be in the unit of cm
vector<double> INTTXYvtx::Get_covariance_TH2(TH2F * hist_in)
{
    double X_mean = hist_in -> GetMean(1);
    double Y_mean = hist_in -> GetMean(2);

    double denominator = 0;
    double variance_x  = 0; 
    double variance_y  = 0;
    double covariance = 0;

    for (int xi = 0; xi < hist_in -> GetNbinsX(); xi++){
        for (int yi = 0; yi < hist_in -> GetNbinsY(); yi++){
            double cell_weight = hist_in -> GetBinContent(xi+1, yi+1);
            double cell_x = hist_in -> GetXaxis() -> GetBinCenter(xi+1);
            double cell_y = hist_in -> GetYaxis() -> GetBinCenter(yi+1);

            denominator += pow(cell_weight, 2);
            variance_x  += pow(cell_x - X_mean, 2) * pow(cell_weight,2);
            variance_y  += pow(cell_y - Y_mean, 2) * pow(cell_weight,2);
            covariance  += (cell_x - X_mean) * (cell_y - Y_mean) * pow(cell_weight,2);
        }
    }

    // note : the histogram is in the unit of cm, we change it back to mm
    return {X_mean * 10., Y_mean * 10., (variance_x/denominator) * 10., (variance_y/denominator) * 10., (covariance/denominator) * 10.};
}

//note : accumulate the number of entries from both sides of the histogram
double INTTXYvtx::get_dist_offset(TH1F * hist_in, int check_N_bin) // note : check_N_bin 1 to N bins of hist
{
    if (check_N_bin < 0 || check_N_bin > hist_in -> GetNbinsX()) {cout<<" wrong check_N_bin "<<endl; exit(1);}
    double total_entry = 0;
    for (int i = 0; i < check_N_bin; i++)
    {
        total_entry += hist_in -> GetBinContent(i+1); // note : 1, 2, 3.....
        total_entry += hist_in -> GetBinContent(hist_in -> GetNbinsX() - i);
    }

    return total_entry/double(2. * check_N_bin);
}

#endif


    // //note : N_trial has to be odd -> even_number + 0 + event_number
    // for (int i = 0; i < N_trial; i++)
    // {   

    //     sub_out_folder_name = Form("%s/New_trial_%i",out_folder_directory.c_str(), i);
    //     system(Form("mkdir %s",sub_out_folder_name.c_str()));
    //     tested_angle = correction_circle.second + (i - ((N_trial - 1)/2) ) * moving_unit - 90.;

    //     // cout<<"test : "<<correction_circle.first<<" "<<beam_origin.first<<" "<<beam_origin.second<<endl;
    //     // cout<<"test : "<<correction.x<<" "<<correction.y<<endl;

    //     correction = PolarToCartesian(correction_circle.first, tested_angle);
    //     current_vtxX = beam_origin.first + correction.x;
    //     current_vtxY = beam_origin.second + correction.y;

    //     info_vec = GetVTXxyCorrection_new(i);
    //     cout<<"given angle: "<<tested_angle<<endl;
    //     cout<<"Pol1 fit, chi2: "<<horizontal_fit_inner->GetChisquare()<<" NDF :"<<horizontal_fit_inner->GetNDF()<<" fit error: #pm"<<horizontal_fit_inner->GetParError(0)<<endl;
        
    //     Xpos_vec.push_back(tested_angle);
    //     XE_vec.push_back(0);
    //     Ypos_vec_stddev.push_back(info_vec[0]);
    //     YE_vec_stddev.push_back(info_vec[1]);
    //     Ypos_vec_pol0_chi2.push_back(info_vec[2]);
    //     Ypos_vec_pol0_fitE.push_back(info_vec[3]);
    //     YE_vec_pol0.push_back(0);

    //     PrintPlotsVTXxy(sub_out_folder_name, 1);
    //     ClearHist(1);
    // }

    // DrawTGraphErrors(Xpos_vec, Ypos_vec_stddev, XE_vec, YE_vec_stddev, out_folder_directory, {Form("angle_diff_pos_relation"), "Correction angle [degree]", "Angle diff StdDev [degree]"});
    // DrawTGraphErrors(Xpos_vec, Ypos_vec_pol0_chi2, XE_vec, YE_vec_pol0, out_folder_directory, {Form("Pol0_chi2_pos_relation"), "Correction angle [degree]", "Pol0 fit reduced #chi^{2}"});
    // DrawTGraphErrors(Xpos_vec, Ypos_vec_pol0_fitE, XE_vec, YE_vec_pol0, out_folder_directory, {Form("Pol0_fitE_pos_relation"), "Correction angle [degree]", "Pol0 fit fit error [mm]"});
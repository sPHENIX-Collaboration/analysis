#ifndef INTTZvtx_h
#define INTTZvtx_h

#include "../private_cluster_gen/InttConversion_new.h"
#include "../private_cluster_gen/InttClustering.h"
#include "../sigmaEff.h"
#include "gaus_func.h"
#include "MegaTrackFinder.h"
// #include "../sPhenixStyle.C"

double double_gaus_func(double *x, double *par)
{
    // note : par[0] : size of gaus1
    // note : par[1] : mean of gaus1
    // note : par[2] : width of gaus1

    // note : par[3] : size of gaus2
    // note : par[4] : mean of gaus2
    // note : par[5] : width of gaus2

    // note : par[6] : systematic ffset
    double gaus1 = par[0] * TMath::Gaus(x[0],par[1],par[2]); 
    double gaus2 = par[3] * TMath::Gaus(x[0],par[4],par[5]); 

    return gaus1 + gaus2 + par[6];
}

class INTTZvtx
{
    public : 
        INTTZvtx(string run_type, string out_folder_directory, pair<double,double> beam_origin, int geo_mode_id, double phi_diff_cut = 0.11, pair<double, double> DCA_cut = {-1,1}, int N_clu_cutl = 20, int N_clu_cut = 10000, int zvtx_cal_require = 15, pair<double,double> zvtx_QA_width = {39.62, 65.36}, double zvtx_QA_ratio = 0.00001, bool draw_event_display = true, double peek = 3.32405);
        void ProcessEvt(int event_i, vector<clu_info> temp_sPH_inner_nocolumn_vec, vector<clu_info> temp_sPH_outer_nocolumn_vec, vector<vector<double>> temp_sPH_nocolumn_vec, vector<vector<double>> temp_sPH_nocolumn_rz_vec, int NvtxMC, double TrigZvtxMC, bool PhiCheckTag, Long64_t bco_full, int centrality_bin);
        void ClearEvt();
        void PrintPlots();
        void EndRun();

        double GetZdiffPeakMC();
        double GetZdiffWidthMC();

        vector<double> GetEvtZPeak();


    private : 
        TCanvas * c2;
        TCanvas * c1;
        TPad *pad_xy;
        TPad *pad_rz;
        TPad *pad_z;
        TPad *pad_z_hist;
        TPad *pad_z_line;
        TPad *pad_phi_diff;
        TPad *pad_track_phi;
        TPad *pad_inner_outer_phi;
        TPad *pad_phi_diff_1D;
        
        TH1F * avg_event_zvtx;
        TH1F * zvtx_evt_fitError;
        TH2F * zvtx_evt_fitError_corre;
        TH2F * zvtx_evt_width_corre;
        TH2F * zvtx_evt_nclu_corre;
        TH1F * width_density;         // note : N good hits / width
        TH1F * ES_width;
        TH1F * ES_width_ratio;
        TH2F * Z_resolution_Nclu;     
        TH2F * Z_resolution_pos;      
        TH2F * Z_resolution_pos_cut;  
        TH1F * Z_resolution;          
        TH1F * evt_possible_z;
        TH1F * line_breakdown_hist;               // note : try to fill the line into the histogram
        TH1F * line_breakdown_gaus_ratio_hist;    // note : the distribution of the entry/width of gaus fit 
        TH1F * line_breakdown_gaus_width_hist;    // note : the distribution of the gaus fit width 
        TH2F * gaus_width_Nclu;
        TH2F * gaus_rchi2_Nclu;
        TH2F * final_fit_width;
        TH2F * N_track_candidate_Nclu; // note : Number of tracklet candidate (In xy plane) vs number of clusters
        TH1F * temp_event_zvtx;
        TH1F * peak_group_width_hist;
        TH1F * peak_group_ratio_hist;
        TH1F * N_group_hist;
        TH1F * peak_group_detail_width_hist;
        TH1F * peak_group_detail_ratio_hist;
        TH1F * N_group_detail_hist;
        TH1F * evt_select_track_phi;
        TH1F * evt_phi_diff_1D;
        TH2F * evt_phi_diff_inner_phi;
        TH2F * evt_inner_outer_phi;
        


        InttConversion * ch_pos_DB;
        TLatex * ltx;
        TLatex * draw_text;
        TLine  * ladder_line;
        TLine  * eff_sig_range_line;
        TLine  * final_fit_range_line;
        TLine  * track_line;
        TLine  * coord_line;
        TFile  * out_file;
        TTree  * tree_out;
        TF1    * gaus_fit;
        TF1    * gaus_fit_2;
        TF1    * double_gaus_fit;
        TF1    * zvtx_finder;

        pair<double,double> evt_possible_z_range = {-700, 700};
        vector<string> conversion_mode_BD = {"ideal", "survey_1_XYAlpha_Peek", "full_survey_3.32"};
        double Integrate_portion_final = 0.68; 
        double Integrate_portion = 0.35; // todo : Width selection per event (the range finder for fitting)
        double width_density_cut = 0.00055; // Todo : change the width density here
        double high_multi_line = 1000; // todo : the cut to classify the high-low multiplicity, which are fit with different method
        double zvtx_hist_l = -500;
        double zvtx_hist_r = 500;
        int print_rate = 50; // todo : the print rate is here

        pair<double, double> beam_origin;
        pair<double, double> DCA_cut;       // note : if (< DCA_cut)          -> pass      unit mm
        pair<double, double> zvtx_QA_width; // note : for the zvtx range Quality check, check the width 
        string out_folder_directory;
        string run_type;
        double peek;
        double phi_diff_cut;          // note : if (< phi_diff_cut)      -> pass      unit degree
        double zvtx_QA_ratio;         // note : for the zvtx range QUality check, check the Norm. size / width
        double width_density_par;
        double Clus_InnerPhi_Offset; // note : the vertex in XY is not at zero, so the "offset" moves the offset back to the orign which is (0,0)
        double Clus_OuterPhi_Offset; // note : the vertex in XY is not at zero, so the "offset" moves the offset back to the orign which is (0,0)
        bool draw_event_display;
        int zvtx_cal_require;         // note : if (> zvtx_cal_require)  -> pass
        int geo_mode_id;
        int N_clu_cut;                // note : if (> N_clu_cut)         -> continue  unit number
        int N_clu_cutl;               // note : if (< N_clu_cutl)        -> continue  unit number

        vector<vector<pair<bool,clu_info>>> inner_clu_phi_map; // note: phi
        vector<vector<pair<bool,clu_info>>> outer_clu_phi_map; // note: phi
        
        // note : for tree_out
        double out_ES_zvtx, out_ES_zvtxE, out_ES_rangeL, out_ES_rangeR, out_ES_width_density, MC_true_zvtx;
        double out_LB_Gaus_Mean_mean, out_LB_Gaus_Mean_meanE, out_LB_Gaus_Mean_width, out_LB_Gaus_Mean_chi2;
        double out_LB_Gaus_Width_width, out_LB_Gaus_Width_size_width, out_LB_Gaus_Width_offset, out_LB_geo_mean;
        double out_LB_refit, out_LB_refitE; // note : not used in the out tree
        double out_mid_cut_peak_width, out_mid_cut_peak_ratio, out_LB_cut_peak_width, out_LB_cut_peak_ratio;
        bool out_good_zvtx_tag;
        int out_eID, N_cluster_outer_out, N_cluster_inner_out, out_ES_N_good, out_mid_cut_Ngroup, out_LB_cut_Ngroup, out_centrality_bin;
        int out_N_cluster_north, out_N_cluster_south;
        Long64_t bco_full_out; 

        // note : for out parameters
        double MC_z_diff_peak, MC_z_diff_width; // note : the comparison between Reco - true in MC. Values are from fitting foucsing on the peak region.

        void Init();
        void InitHist();
        void InitCanvas();
        void InitTreeOut();
        void InitRest();

        vector<vector<double>> good_track_xy_vec; // note : for event drawing, not used now
        vector<vector<double>> good_track_rz_vec; // note : for event drawing, not used now
        vector<vector<double>> good_comb_rz_vec;  // note : not used
        vector<vector<double>> good_comb_xy_vec;  // note : not used
        vector<vector<double>> good_comb_phi_vec; // note : not used
        vector<vector<double>> good_tracklet_rz;  // note : not used
        vector<float> temp_event_zvtx_vec;        // note : not used
        vector<float> temp_event_zvtx_info;
        vector<float> avg_event_zvtx_vec;
        vector<float> Z_resolution_vec;
        vector<float> line_breakdown_vec;
        vector<double> N_group_info; // note : the information of groups remaining in the histogram after the strong background suppression
        vector<double> N_group_info_detail; // note : detail
        string plot_text;
        double final_selection_widthD;
        double final_selection_widthU; 
        double gaus_ratio;
        double gaus_fit_offset; // note : when doing the integral, removing the offset is more preferable, so keep the offset here
        double final_zvtx;
        double tight_offset_width;
        double tight_offset_peak;
        double loose_offset_peak;
        double loose_offset_peakE;
        bool good_zvtx_tag;
        double good_zvtx_tag_int;
        long total_NClus;
        int fit_winner_id;
        int good_comb_id;
        

        // note : the class that handles the 3/4-cluster tracklet finder
        MegaTrackFinder * mega_track_finder; 
        map<int,int> centrality_map = {
            {5, 0},
            {15, 1},
            {25, 2},
            {35, 3},
            {45, 4},
            {55, 5},
            {65, 6},
            {75, 7},
            {85, 8},
            {95, 9}
        };


        TGraphErrors * z_range_gr;
        TGraphErrors * z_range_gr_draw;
        TGraph * temp_event_xy;
        TGraph * temp_event_rz;
        TGraph * bkg;
        

        // note : in the event process
        vector<float> N_comb; vector<float> N_comb_e; vector<float> z_mid; vector<float> z_range; vector<double> N_comb_phi;
        vector<double> eff_N_comb; vector<double> eff_z_mid; vector<double> eff_N_comb_e; vector<double> eff_z_range; // note : eff_sig
        vector<double> LB_N_comb; vector<double> LB_z_mid; vector<double> LB_N_comb_e; vector<double> LB_z_range; // note : LB gaus

        std::vector<double> calculateDistanceAndClosestPoint(double x1, double y1, double x2, double y2, double target_x, double target_y);
        pair<double,double> Get_possible_zvtx(double rvtx, vector<double> p0, vector<double> p1);
        vector<double> find_Ngroup(TH1F * hist_in);
        double get_radius(double x, double y);
        double calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY);
        double Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y);
        double LB_geo_mean(TH1F * hist_in, pair<double, double> search_range, int event_i);
        void temp_bkg(TPad * c1, double peek, pair<double,double> beam_origin, InttConversion * ch_pos_DB);
        void Characterize_Pad(TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0);
        void line_breakdown(TH1F* hist_in, pair<double,double> line_range);
        int min_width_ID(vector<double> input_width_vec);
        double get_delta_phi(double angle_1, double angle_2);
        double get_track_phi(double inner_clu_phi_in, double delta_phi_in);
        
        

};



INTTZvtx::INTTZvtx(string run_type, string out_folder_directory, pair<double,double> beam_origin, int geo_mode_id, double phi_diff_cut, pair<double,double> DCA_cut, int N_clu_cutl, int N_clu_cut, int zvtx_cal_require, pair<double,double> zvtx_QA_width, double zvtx_QA_ratio, bool draw_event_display, double peek)
:run_type(run_type), out_folder_directory(out_folder_directory), beam_origin(beam_origin), geo_mode_id(geo_mode_id), peek(peek), N_clu_cut(N_clu_cut), N_clu_cutl(N_clu_cutl), phi_diff_cut(phi_diff_cut), DCA_cut(DCA_cut), zvtx_cal_require(zvtx_cal_require), zvtx_QA_width(zvtx_QA_width), zvtx_QA_ratio(zvtx_QA_ratio), draw_event_display(draw_event_display)
{
    SetsPhenixStyle();
    system(Form("mkdir %s",out_folder_directory.c_str()));
    gErrorIgnoreLevel = kWarning; // note : To not print the "print plot info."

    temp_event_zvtx_vec.clear();
    temp_event_zvtx_info.clear();
    good_track_xy_vec.clear();
    good_track_rz_vec.clear();
    good_comb_rz_vec.clear();
    good_comb_xy_vec.clear();
    good_comb_phi_vec.clear();
    good_tracklet_rz.clear();
    avg_event_zvtx_vec.clear();
    Z_resolution_vec.clear();
    line_breakdown_vec.clear();
    good_comb_id = 0;
    MC_z_diff_peak = -777.;
    MC_z_diff_width = -777.;

    out_N_cluster_south = 0;
    out_N_cluster_north = 0;

    N_group_info_detail = {-1.,-1.,-1.,-1.};

    N_comb.clear(); N_comb_e.clear(); z_mid.clear(); z_range.clear(); N_comb_phi.clear();
    eff_N_comb.clear(); eff_z_mid.clear(); eff_N_comb_e.clear(); eff_z_range.clear(); // note : eff_sig
    LB_N_comb.clear(); LB_z_mid.clear(); LB_N_comb_e.clear(); LB_z_range.clear(); // note : LB gaus

    inner_clu_phi_map.clear();
    outer_clu_phi_map.clear();
    inner_clu_phi_map = vector<vector<pair<bool,clu_info>>>(360);
    outer_clu_phi_map = vector<vector<pair<bool,clu_info>>>(360);

    Init();

    plot_text = (run_type == "MC") ? "Simulation" : "Work-in-progress";
    
    mega_track_finder = new MegaTrackFinder(run_type, out_folder_directory, centrality_map.size(), beam_origin);

    if (draw_event_display) {c2 -> Print(Form("%s/temp_event_display.pdf(",out_folder_directory.c_str()));}
}

void INTTZvtx::Characterize_Pad (TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0)
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

void INTTZvtx::Init()
{
    InitCanvas();
    InitHist();
    InitTreeOut();
    InitRest();
}

void INTTZvtx::InitHist()
{
    avg_event_zvtx = new TH1F("","avg_event_zvtx",100,zvtx_hist_l,zvtx_hist_r);
    // avg_event_zvtx -> SetMarkerStyle(20);
    // avg_event_zvtx -> SetMarkerSize(0.8);
    // avg_event_zvtx -> SetMarkerColor(TColor::GetColor("#1A3947"));
    avg_event_zvtx -> SetLineColor(TColor::GetColor("#1A3947"));
    avg_event_zvtx -> SetLineWidth(2);
    avg_event_zvtx -> GetYaxis() -> SetTitle("Entry");
    avg_event_zvtx -> GetXaxis() -> SetTitle("Z vertex position [mm]");
    avg_event_zvtx -> GetYaxis() -> SetRangeUser(0,50);
    avg_event_zvtx -> SetTitleSize(0.06, "X");
    avg_event_zvtx -> SetTitleSize(0.06, "Y");
    avg_event_zvtx -> GetXaxis() -> SetTitleOffset(0.82);
    avg_event_zvtx -> GetYaxis() -> SetTitleOffset(1.1);
    avg_event_zvtx -> GetXaxis() -> CenterTitle(true);
    avg_event_zvtx -> GetYaxis() -> CenterTitle(true);
    avg_event_zvtx -> GetXaxis() -> SetNdivisions(505);

    zvtx_evt_fitError = new TH1F("","zvtx_evt_fitError",150,0,50);
    zvtx_evt_fitError -> GetXaxis() -> SetTitle(" mm ");
    zvtx_evt_fitError -> GetYaxis() -> SetTitle("entry");
    zvtx_evt_fitError -> GetXaxis() -> SetNdivisions(505);

    zvtx_evt_fitError_corre = new TH2F("","zvtx_evt_fitError_corre",200,0,10000,200,0,20);
    zvtx_evt_fitError_corre -> GetXaxis() -> SetTitle(" # of clusters ");
    zvtx_evt_fitError_corre -> GetYaxis() -> SetTitle(" #pm mm ");
    zvtx_evt_fitError_corre -> GetXaxis() -> SetNdivisions(505);

    zvtx_evt_width_corre = new TH2F("","zvtx_evt_width_corre",200,0,10000,200,0,300);
    zvtx_evt_width_corre -> GetXaxis() -> SetTitle(" # of clusters ");
    zvtx_evt_width_corre -> GetYaxis() -> SetTitle(" mm ");
    zvtx_evt_width_corre -> GetXaxis() -> SetNdivisions(505);

    zvtx_evt_nclu_corre = new TH2F("","zvtx_evt_nclu_corre",200,0,10000,200,-1000,1000);
    zvtx_evt_nclu_corre -> GetXaxis() -> SetTitle(" # of clusters ");
    zvtx_evt_nclu_corre -> GetYaxis() -> SetTitle(" zvtx [mm] ");
    zvtx_evt_nclu_corre -> GetXaxis() -> SetNdivisions(505);

    width_density = new TH1F("","width_density",200,0,0.006); // note : N good hits / width
    width_density -> GetXaxis() -> SetTitle(" (N good track / width) / NClus ");
    width_density -> GetYaxis() -> SetTitle(" Entry ");
    width_density -> GetXaxis() -> SetNdivisions(505);

    ES_width = new TH1F("","ES_width",200,0,150);
    ES_width -> GetXaxis() -> SetTitle(" Width [mm] ");
    ES_width -> GetYaxis() -> SetTitle(" Entry ");
    ES_width -> GetXaxis() -> SetNdivisions(505);

    ES_width_ratio = new TH1F("","ES_width_ratio",200,0,60);
    ES_width_ratio -> GetXaxis() -> SetTitle(" NClus / Width ");
    ES_width_ratio -> GetYaxis() -> SetTitle(" Entry ");
    ES_width_ratio -> GetXaxis() -> SetNdivisions(505);

    Z_resolution_Nclu = new TH2F("","Z_resolution_Nclu",200,0,10000,200,-100,100);
    Z_resolution_Nclu -> GetXaxis() -> SetTitle(" # of clusters ");
    Z_resolution_Nclu -> GetYaxis() -> SetTitle("#Delta Z (Reco - True) [mm]");
    Z_resolution_Nclu -> GetXaxis() -> SetNdivisions(505);

    Z_resolution_pos = new TH2F("","Z_resolution_pos",200,-350,350,200,-50,50);
    Z_resolution_pos -> GetXaxis() -> SetTitle("True Zvtx [mm]");
    Z_resolution_pos -> GetYaxis() -> SetTitle("#Delta Z (Reco - True) [mm]");
    Z_resolution_pos -> GetXaxis() -> SetNdivisions(505);

    Z_resolution_pos_cut = new TH2F("","Z_resolution_pos_cut",200,-350,350,200,-50,50);
    Z_resolution_pos_cut -> GetXaxis() -> SetTitle("True Zvtx [mm]");
    Z_resolution_pos_cut -> GetYaxis() -> SetTitle("#Delta Z (Reco - True) [mm]");
    Z_resolution_pos_cut -> GetXaxis() -> SetNdivisions(505);

    Z_resolution = new TH1F("","Z_resolution",200,-30,30);
    Z_resolution -> GetXaxis() -> SetTitle("#Delta Z (Reco - True) [mm]");
    Z_resolution -> GetYaxis() -> SetTitle(" Entry ");
    Z_resolution -> GetXaxis() -> SetNdivisions(505);

    evt_possible_z = new TH1F("","evt_possible_z",50, evt_possible_z_range.first, evt_possible_z_range.second);
    evt_possible_z -> SetLineWidth(1);
    evt_possible_z -> GetXaxis() -> SetTitle("Z [mm]");
    evt_possible_z -> GetYaxis() -> SetTitle("Entry");

    int N = 1200;        // note : N bins for each side, regardless the bin at zero
    double width = 0.5;  // note : bin width with the unit [mm]
    line_breakdown_hist = new TH1F("", "line_breakdown_hist", 2*N+1, -1*(width*N + width/2.), width*N + width/2.);
    line_breakdown_hist -> SetLineWidth(1);
    line_breakdown_hist -> GetXaxis() -> SetTitle("Z [mm]");
    line_breakdown_hist -> GetYaxis() -> SetTitle("Entry");
    cout<<"class INTTZvtx, Line brakdown hist, range : "<<line_breakdown_hist->GetXaxis()->GetXmin()<<" "<<line_breakdown_hist->GetXaxis()->GetXmax()<<" "<<line_breakdown_hist->GetBinWidth(1)<<endl;

    line_breakdown_gaus_ratio_hist = new TH1F("","line_breakdown_gaus_ratio_hist",200,0,0.0005);
    line_breakdown_gaus_ratio_hist -> GetXaxis() -> SetTitle("(Norm. size) / width");
    line_breakdown_gaus_ratio_hist -> GetYaxis() -> SetTitle(" Entry ");
    line_breakdown_gaus_ratio_hist -> GetXaxis() -> SetNdivisions(505);

    line_breakdown_gaus_width_hist = new TH1F("","line_breakdown_gaus_width_hist",200,0,100);
    line_breakdown_gaus_width_hist -> GetXaxis() -> SetTitle("Width [mm]");
    line_breakdown_gaus_width_hist -> GetYaxis() -> SetTitle(" Entry ");
    line_breakdown_gaus_width_hist -> GetXaxis() -> SetNdivisions(505);

    gaus_width_Nclu = new TH2F("","gaus_width_Nclu",200,0,10000,200,0,100);
    gaus_width_Nclu -> GetXaxis() -> SetTitle(" # of clusters ");
    gaus_width_Nclu -> GetYaxis() -> SetTitle("Gaus fit width [mm]");
    gaus_width_Nclu -> GetXaxis() -> SetNdivisions(505);

    gaus_rchi2_Nclu = new TH2F("","gaus_rchi2_Nclu",200,0,10000,200,0,1);
    gaus_rchi2_Nclu -> GetXaxis() -> SetTitle(" # of clusters ");
    gaus_rchi2_Nclu -> GetYaxis() -> SetTitle("Gaus fit #chi2^{2}/NDF");
    gaus_rchi2_Nclu -> GetXaxis() -> SetNdivisions(505);

    final_fit_width = new TH2F("","final_fit_width",200,0,10000,200,0,400);
    final_fit_width -> GetXaxis() -> SetTitle(" # of clusters ");
    final_fit_width -> GetYaxis() -> SetTitle("Fit width [mm]");
    final_fit_width -> GetXaxis() -> SetNdivisions(505);

    N_track_candidate_Nclu = new TH2F("","N_track_candidate_Nclu",200,0,10000,200,0,13000);
    N_track_candidate_Nclu -> GetXaxis() -> SetTitle(" # of clusters ");
    N_track_candidate_Nclu -> GetYaxis() -> SetTitle("N tracklet candidates");
    N_track_candidate_Nclu -> GetXaxis() -> SetNdivisions(505);

    temp_event_zvtx = new TH1F("","Z vertex dist",125,zvtx_hist_l,zvtx_hist_r);
    temp_event_zvtx -> GetXaxis() -> SetTitle("Z vertex position (mm)");
    temp_event_zvtx -> GetYaxis() -> SetTitle("Entry");

    peak_group_width_hist = new TH1F("","peak_group_width_hist",100,0,300);
    peak_group_width_hist -> GetXaxis() -> SetTitle("Width [mm]");
    peak_group_width_hist -> GetYaxis() -> SetTitle("Entry");
    peak_group_width_hist -> GetXaxis() -> SetNdivisions(505);
    
    peak_group_ratio_hist = new TH1F("","peak_group_ratio_hist",110,0,1.1);
    peak_group_ratio_hist -> GetXaxis() -> SetTitle("Peak group ratio");
    peak_group_ratio_hist -> GetYaxis() -> SetTitle("Entry");
    peak_group_ratio_hist -> GetXaxis() -> SetNdivisions(505);

    N_group_hist  = new TH1F("","N_group_hist",30,0,30);
    N_group_hist -> GetXaxis() -> SetTitle("N group post cut");
    N_group_hist -> GetYaxis() -> SetTitle("Entry");
    N_group_hist -> GetXaxis() -> SetNdivisions(505);

    peak_group_detail_width_hist = new TH1F("","peak_group_detail_width_hist",200,0,300);
    peak_group_detail_width_hist -> GetXaxis() -> SetTitle("Width [mm]");
    peak_group_detail_width_hist -> GetYaxis() -> SetTitle("Entry");
    peak_group_detail_width_hist -> GetXaxis() -> SetNdivisions(505);

    peak_group_detail_ratio_hist  = new TH1F("","peak_group_detail_ratio_hist",110,0,1.1);
    peak_group_detail_ratio_hist -> GetXaxis() -> SetTitle("Peak group ratio");
    peak_group_detail_ratio_hist -> GetYaxis() -> SetTitle("Entry");
    peak_group_detail_ratio_hist -> GetXaxis() -> SetNdivisions(505);

    N_group_detail_hist = new TH1F("","N_group_detail_hist",30,0,30);
    N_group_detail_hist -> GetXaxis() -> SetTitle("N group post cut");
    N_group_detail_hist -> GetYaxis() -> SetTitle("Entry");
    N_group_detail_hist -> GetXaxis() -> SetNdivisions(505);

    evt_select_track_phi = new TH1F("","evt_select_track_phi",361,0,361);
    evt_select_track_phi -> GetXaxis() -> SetTitle("Track phi [degree]");
    evt_select_track_phi -> GetYaxis() -> SetTitle("Entry");
    evt_select_track_phi -> GetXaxis() -> SetNdivisions(505);
    
    evt_phi_diff_inner_phi = new TH2F("","evt_phi_diff_inner_phi",361,0,361,100,-1.5,1.5);
    evt_phi_diff_inner_phi -> GetXaxis() -> SetTitle("Inner phi [degree]");
    evt_phi_diff_inner_phi -> GetYaxis() -> SetTitle("Inner - Outer [degree]");
    evt_phi_diff_inner_phi -> GetXaxis() -> SetNdivisions(505);

    evt_inner_outer_phi = new TH2F("","evt_inner_outer_phi",120,0,360,120,0,360);
    evt_inner_outer_phi -> GetXaxis() -> SetTitle("Inner phi [degree]");
    evt_inner_outer_phi -> GetYaxis() -> SetTitle("Outer phi [degree]");
    evt_inner_outer_phi -> GetXaxis() -> SetNdivisions(505);

    evt_phi_diff_1D = new TH1F("","evt_phi_diff_1D",100,-10,10);
    evt_phi_diff_1D -> GetXaxis() -> SetTitle("Inner - Outer [degree]");
    evt_phi_diff_1D -> GetYaxis() -> SetTitle("Entry");
    evt_phi_diff_1D -> GetXaxis() -> SetNdivisions(505);
}

void INTTZvtx::InitCanvas()
{
    c2 = new TCanvas("","",4000,1600);    
    c2 -> cd();
    pad_xy = new TPad(Form("pad_xy"), "", 0.0, 0.5, 0.2, 1.0);
    Characterize_Pad(pad_xy, 0.15, 0.1, 0.1, 0.2, 0, 0);
    pad_xy -> Draw();

    pad_rz = new TPad(Form("pad_rz"), "", 0.2, 0.5, 0.40, 1.0);
    Characterize_Pad(pad_rz, 0.15, 0.1, 0.1, 0.2, 0, 0);
    pad_rz -> Draw();

    pad_z = new TPad(Form("pad_z"), "", 0.40, 0.5, 0.6, 1.0);
    Characterize_Pad(pad_z, 0.15, 0.1, 0.1, 0.2, 0, 0);
    pad_z -> Draw();
    
    pad_z_hist = new TPad(Form("pad_z_hist"), "", 0.6, 0.5, 0.8, 1.0);
    Characterize_Pad(pad_z_hist, 0.15, 0.1, 0.1, 0.2, 0, 0);
    pad_z_hist -> Draw();

    pad_z_line = new TPad(Form("pad_z_line"), "", 0.8, 0.5, 1, 1.0);
    Characterize_Pad(pad_z_line, 0.15, 0.1, 0.1, 0.2, 0, 0);
    pad_z_line -> Draw();

    pad_phi_diff = new TPad(Form("pad_phi_diff"), "", 0.0, 0.0, 0.2, 0.5);
    Characterize_Pad(pad_phi_diff, 0.15, 0.1, 0.1, 0.2, 0, 0);
    pad_phi_diff -> Draw();

    pad_track_phi = new TPad(Form("pad_track_phi"), "", 0.2, 0.0, 0.40, 0.5);
    Characterize_Pad(pad_track_phi, 0.15, 0.1, 0.1, 0.2, 0, 0);
    pad_track_phi -> Draw();

    pad_inner_outer_phi = new TPad(Form("pad_inner_outer_phi"), "", 0.4, 0.0, 0.60, 0.5);
    Characterize_Pad(pad_inner_outer_phi, 0.15, 0.1, 0.1, 0.2, 0, 0);
    pad_inner_outer_phi -> SetLogz(1);
    pad_inner_outer_phi -> Draw();

    pad_phi_diff_1D = new TPad(Form("pad_phi_diff_1D"), "", 0.6, 0.0, 0.80, 0.5);
    Characterize_Pad(pad_phi_diff_1D, 0.15, 0.1, 0.1, 0.2, 0, 0);
    // pad_phi_diff_1D -> SetLogz(1);
    pad_phi_diff_1D -> Draw();



    c1 = new TCanvas("","",950,800);
    c1 -> cd();
}

void INTTZvtx::InitTreeOut()
{
    out_file = new TFile(Form("%s/INTT_zvtx.root",out_folder_directory.c_str()),"RECREATE");
    tree_out =  new TTree ("tree_Z", "INTT Z info.");

    tree_out -> Branch("eID",&out_eID);
    tree_out -> Branch("bco_full",&bco_full_out);
    tree_out -> Branch("nclu_inner",&N_cluster_inner_out);
    tree_out -> Branch("nclu_outer",&N_cluster_outer_out);
    tree_out -> Branch("nclu_south",&out_N_cluster_south);
    tree_out -> Branch("nclu_north",&out_N_cluster_north);
    tree_out -> Branch("ES_zvtx",&out_ES_zvtx);                   // note : effective sigma, pol0 fit z-vertex
    tree_out -> Branch("ES_zvtxE",&out_ES_zvtxE);                 // note : effective sigma, pol0 fit z-vertex error
    tree_out -> Branch("ES_rangeL",&out_ES_rangeL);               // note : effective sigma, selected range left
    tree_out -> Branch("ES_rangeR",&out_ES_rangeR);               // note : effective sigma, selected range right 
    tree_out -> Branch("ES_N_good",&out_ES_N_good);               // note : effective sigma, number of z-vertex candidates in the range
    tree_out -> Branch("ES_Width_density",&out_ES_width_density); // note : effective sigma, N good z-vertex candidates divided by width
    tree_out -> Branch("LB_Gaus_Mean_mean",&out_LB_Gaus_Mean_mean);              // note : Line break loose offset - gaus mean   
    tree_out -> Branch("LB_Gaus_Mean_meanE",&out_LB_Gaus_Mean_meanE);            // note : Line break loose offset - gaus mean error
    tree_out -> Branch("LB_Gaus_Mean_chi2", &out_LB_Gaus_Mean_chi2);             // note : Line break loose offset - reduce chi2
    tree_out -> Branch("LB_Gaus_Mean_width",&out_LB_Gaus_Mean_width);            // note : Line break loose offset - width
    tree_out -> Branch("LB_Gaus_Width_width",&out_LB_Gaus_Width_width);                 // note : Line break tight offset - gaus width
    tree_out -> Branch("LB_Gaus_Width_offset", &out_LB_Gaus_Width_offset);              // note : Line break tight offset - offset
    tree_out -> Branch("LB_Gaus_Width_size_width", &out_LB_Gaus_Width_size_width);      // note : Line break tight offset - norm. height / width
    tree_out -> Branch("LB_geo_mean", &out_LB_geo_mean);          // note : Line break peak position directly from the distribution (with the bin width 0.5 mm)
    tree_out -> Branch("good_zvtx_tag", &out_good_zvtx_tag);
    tree_out -> Branch("mid_cut_Ngroup",     &out_mid_cut_Ngroup);            // note : mid cut Ngroup
    tree_out -> Branch("mid_cut_peak_width", &out_mid_cut_peak_width);        // note : mid cut peak width
    tree_out -> Branch("mid_cut_peak_ratio", &out_mid_cut_peak_ratio);        // note : mid cut peak ratio
    tree_out -> Branch("LB_cut_Ngroup",     &out_LB_cut_Ngroup);         // note : LB cut Ngroup
    tree_out -> Branch("LB_cut_peak_width", &out_LB_cut_peak_width);     // note : LB cut peak width
    tree_out -> Branch("LB_cut_peak_ratio", &out_LB_cut_peak_ratio);     // note : LB cut peak ratio
    tree_out -> Branch("MC_true_zvtx",&MC_true_zvtx);
    tree_out -> Branch("Centrality_bin",&out_centrality_bin);
}

void INTTZvtx::InitRest()
{
    gaus_fit = new TF1("gaus_fit",gaus_func,evt_possible_z_range.first,evt_possible_z_range.second,4);
    gaus_fit -> SetLineColor(2);
    gaus_fit -> SetLineWidth(1);
    gaus_fit -> SetNpx(1000);

    gaus_fit_2 = new TF1("gaus_fit_2",gaus_func,evt_possible_z_range.first,evt_possible_z_range.second,4);
    gaus_fit_2 -> SetLineColor(2);
    gaus_fit_2 -> SetLineWidth(2);
    gaus_fit_2 -> SetNpx(1000);

    double_gaus_fit = new TF1("double_gaus_fit",double_gaus_func,-600,600,7);
    double_gaus_fit -> SetLineColor(3);
    double_gaus_fit -> SetLineWidth(1);
    double_gaus_fit -> SetNpx(1000);

    eff_sig_range_line = new TLine();
    eff_sig_range_line -> SetLineWidth(2);
    eff_sig_range_line -> SetLineColor(TColor::GetColor("#A08144"));
    eff_sig_range_line -> SetLineStyle(2);

    final_fit_range_line = new TLine();
    final_fit_range_line -> SetLineWidth(1);
    final_fit_range_line -> SetLineColor(TColor::GetColor("#44a04d"));
    // final_fit_range_line -> SetLineStyle(2);

    zvtx_finder = new TF1("zvtx_finder","pol0",-1,20000); 
    zvtx_finder -> SetLineColor(2);
    zvtx_finder -> SetLineWidth(1);

    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    ladder_line = new TLine();
    ladder_line -> SetLineWidth(1);

    track_line = new TLine();
    track_line -> SetLineWidth(1);
    track_line -> SetLineColorAlpha(38,0.5);

    coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(16);
    coord_line -> SetLineStyle(2);

    ch_pos_DB = new InttConversion(conversion_mode_BD[geo_mode_id], peek);
}

void INTTZvtx::ProcessEvt(
    int event_i,
    vector<clu_info> temp_sPH_inner_nocolumn_vec,
    vector<clu_info> temp_sPH_outer_nocolumn_vec, 
    vector<vector<double>> temp_sPH_nocolumn_vec,
    vector<vector<double>> temp_sPH_nocolumn_rz_vec,
    int NvtxMC,
    double TrigZvtxMC,
    bool PhiCheckTag,
    Long64_t bco_full,
    int centrality_bin
)
{
    out_eID = event_i;
    N_cluster_inner_out = -1;
    N_cluster_outer_out = -1;
    bco_full_out = bco_full;
    
    out_ES_zvtx = -1;
    out_ES_zvtxE = -1;
    out_ES_rangeL = -1;
    out_ES_rangeR = -1;
    out_ES_N_good = -1;
    out_ES_width_density = -1;
    
    out_LB_Gaus_Mean_mean = -1;
    out_LB_Gaus_Mean_meanE = -1;
    out_LB_Gaus_Mean_chi2 = -1;
    out_LB_Gaus_Mean_width = -1;
    
    out_LB_Gaus_Width_width = -1;
    out_LB_Gaus_Width_size_width = -1;
    out_LB_Gaus_Width_offset = -1;
    
    out_mid_cut_Ngroup = -1;
    out_mid_cut_peak_width = -1;
    out_mid_cut_peak_ratio = -1;

    out_LB_cut_Ngroup = -1;
    out_LB_cut_peak_width = -1;
    out_LB_cut_peak_ratio = -1;
    
    out_LB_geo_mean = -1;
    out_good_zvtx_tag = 0;
    MC_true_zvtx = TrigZvtxMC * 10.;
    
    out_LB_refit = -1;
    out_LB_refitE = -1;

    good_zvtx_tag = false;
    good_zvtx_tag_int = 0;
    loose_offset_peak = -999; // note : unit [mm]
    loose_offset_peakE = -999; // note : unit [mm]

    out_centrality_bin = -1;

    out_N_cluster_north = 0;
    out_N_cluster_south = 0;



    if (event_i%1000 == 0) {cout<<"In INTTZvtx class, running event : "<<event_i<<endl;}

    total_NClus = temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size();

    if (total_NClus < zvtx_cal_require) {tree_out -> Fill(); return; cout<<"return confirmation"<<endl;}   
    
    if (run_type == "MC" && NvtxMC != 1) {cout<<"In INTTZvtx class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Nvtx more than one "<<endl; tree_out -> Fill(); return;}
    if (PhiCheckTag == false)            {cout<<"In INTTZvtx class, event : "<<event_i<<" Nvtx : "<<NvtxMC<<" Not full phi has hits "<<endl; tree_out -> Fill(); return;}
    
    if (temp_sPH_inner_nocolumn_vec.size() < 10 || temp_sPH_outer_nocolumn_vec.size() < 10 || total_NClus > N_clu_cut || total_NClus < N_clu_cutl)
    {
        printf("In INTTZvtx class, event : %i, low clu continue, NClus : %lu \n", event_i, total_NClus); 
        tree_out -> Fill(); 
        return;
    }

    // note : put the cluster into the phi map, the first bool is for the cluster usage.
    // note : false means the cluster is not used
    for (int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++) {
        Clus_InnerPhi_Offset = (temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second < 0) ? atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi());
        // cout<<"inner clu phi : "<<Clus_InnerPhi_Offset<<" origin: "<< temp_sPH_inner_nocolumn_vec[inner_i].phi <<endl;
        // cout<<" ("<<Clus_InnerPhi_Offset<<", "<< temp_sPH_inner_nocolumn_vec[inner_i].phi<<")" <<endl;
        inner_clu_phi_map[ int(Clus_InnerPhi_Offset) ].push_back({false,temp_sPH_inner_nocolumn_vec[inner_i]});

        if (temp_sPH_inner_nocolumn_vec[inner_i].z > 0) {out_N_cluster_north += 1;}
        else {out_N_cluster_south += 1;}
    }
    for (int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++) {
        Clus_OuterPhi_Offset = (temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second < 0) ? atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi());
        outer_clu_phi_map[ int(Clus_OuterPhi_Offset) ].push_back({false,temp_sPH_outer_nocolumn_vec[outer_i]});

        if (temp_sPH_outer_nocolumn_vec[outer_i].z > 0) {out_N_cluster_north += 1;}
        else {out_N_cluster_south += 1;}
    }

    // note : for the mega cluster finder, the 3/4-cluster tracklet that happens at the overlap region
    mega_track_finder -> FindMegaTracks(inner_clu_phi_map, outer_clu_phi_map, {MC_true_zvtx,0.5}, centrality_map[centrality_bin]);    

    double good_pair_count = 0;    

    for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++) // note : each phi cell (1 degree)
    {
        // note : N cluster in this phi cell
        for (int inner_phi_clu_i = 0; inner_phi_clu_i < inner_clu_phi_map[inner_phi_i].size(); inner_phi_clu_i++)
        {
            if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].first == true) {continue;}

            Clus_InnerPhi_Offset = (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second < 0) ? atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());

            // todo: change the outer phi scan range
            // note : the outer phi index, -1, 0, 1
            for (int scan_i = -1; scan_i < 2; scan_i++)
            {
                int true_scan_i = ((inner_phi_i + scan_i) < 0) ? 360 + (inner_phi_i + scan_i) : ((inner_phi_i + scan_i) > 359) ? (inner_phi_i + scan_i)-360 : inner_phi_i + scan_i;

                // note : N clusters in that outer phi cell
                for (int outer_phi_clu_i = 0; outer_phi_clu_i < outer_clu_phi_map[true_scan_i].size(); outer_phi_clu_i++)
                {
                    if (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].first == true) {continue;}

                    Clus_OuterPhi_Offset = (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second < 0) ? atan2(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first) * (180./TMath::Pi());
                    double delta_phi = get_delta_phi(Clus_InnerPhi_Offset, Clus_OuterPhi_Offset);

                    evt_phi_diff_inner_phi -> Fill(Clus_InnerPhi_Offset, delta_phi);
                    evt_inner_outer_phi -> Fill(Clus_InnerPhi_Offset, Clus_OuterPhi_Offset);
                    evt_phi_diff_1D -> Fill(delta_phi); 
                    
                    if (fabs(delta_phi) < phi_diff_cut)
                    {
                        double DCA_sign = calculateAngleBetweenVectors(
                            outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y,
                            inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y,
                            beam_origin.first, beam_origin.second
                        );   

                        if (DCA_cut.first < DCA_sign && DCA_sign < DCA_cut.second){
                            good_pair_count += 1;
                            // used_outer_check[outer_i] = 1; //note : this outer cluster was already used!
                            
                            // note : we basically transform the coordinate from cartesian to cylinder 
                            // note : we should set the offset first, otherwise it provides the bias
                            // todo : which point should be used, DCA point or vertex xy ? Has to be studied 
                            pair<double,double> z_range_info = Get_possible_zvtx( 
                                0., // get_radius(beam_origin.first,beam_origin.second), 
                                {get_radius(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - beam_origin.first, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - beam_origin.second), inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z}, // note : unsign radius
                                {get_radius(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - beam_origin.first, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - beam_origin.second), outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z}  // note : unsign radius
                            );

                            // note : try to remove some crazy background candidates. Can be a todo
                            if (evt_possible_z_range.first < z_range_info.first && z_range_info.first < evt_possible_z_range.second) 
                            {
                                N_comb.push_back(good_comb_id);
                                N_comb_e.push_back(0);
                                // N_comb_phi.push_back( (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.phi + outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.phi)/2. );
                                N_comb_phi.push_back( get_track_phi(Clus_InnerPhi_Offset, delta_phi) );
                                z_mid.push_back(z_range_info.first);
                                z_range.push_back(z_range_info.second);

                                evt_possible_z -> Fill(z_range_info.first);

                                // note : fill the line_breakdwon histogram as well as a vector for the width determination
                                line_breakdown(line_breakdown_hist,{z_range_info.first - z_range_info.second, z_range_info.first + z_range_info.second});

                                good_comb_id += 1;    
                            }
                        
                        }
                    }
                    
                }
            } // note : end of outer clu loop
        } 

    } // note : end of inner clu loop

    // note : the original two-nested loop method
    // for ( int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++ )
    // {    
    //     for ( int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++ )
    //     {
    //         // bool DCA_tag = false;
    //         // if (used_outer_check[outer_i] == 1) continue; // note : this outer cluster was already used, skip the trial of this combination

    //         // note : The vertex XY is not at zero, which leads to the fact that the angle difference between two clusters (one from inner, one from outer) doesn't have to be zero. (even in the ideal geometry)
    //         // note : Therefore, apply the vertex offset before calculating the cluster phi angle can actually help us reduce the backgroup as it then is expected to be at zero when calculating the angle difference.  
    //         Clus_InnerPhi_Offset = (temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second < 0) ? atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second, temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first) * (180./TMath::Pi());
    //         Clus_OuterPhi_Offset = (temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second < 0) ? atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second, temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first) * (180./TMath::Pi());

    //         // note : to see the event-by-event 
    //         // evt_phi_diff_inner_phi -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi, temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi);
    //         // evt_inner_outer_phi -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi, temp_sPH_outer_nocolumn_vec[outer_i].phi);
    //         // evt_phi_diff_1D -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi); 
    //         evt_phi_diff_inner_phi -> Fill(Clus_InnerPhi_Offset, Clus_InnerPhi_Offset - Clus_OuterPhi_Offset);
    //         evt_inner_outer_phi -> Fill(Clus_InnerPhi_Offset, Clus_OuterPhi_Offset);
    //         evt_phi_diff_1D -> Fill(Clus_InnerPhi_Offset - Clus_OuterPhi_Offset); 

    //         if (fabs(Clus_InnerPhi_Offset - Clus_OuterPhi_Offset) < phi_diff_cut)
    //         // if (fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi) < phi_diff_cut)
    //         {
    //             // vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
    //             //     temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
    //             //     temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
    //             //     beam_origin.first, beam_origin.second
    //             // );

    //             double DCA_sign = calculateAngleBetweenVectors(
    //                 temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
    //                 temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
    //                 beam_origin.first, beam_origin.second
    //             );

    //             // if (DCA_info_vec[0] != fabs(DCA_sign) && fabs( DCA_info_vec[0] - fabs(DCA_sign) ) > 0.1 ){
    //             //     cout<<"different DCA : "<<DCA_info_vec[0]<<" "<<DCA_sign<<" diff : "<<DCA_info_vec[0] - fabs(DCA_sign)<<endl;
    //             // }

    //             if (DCA_cut.first < DCA_sign && DCA_sign < DCA_cut.second){
    //                 // cout<<" "<<endl;
    //                 // cout<<"good pair : "<<good_pair_count<<" inner: ("<<temp_sPH_inner_nocolumn_vec[inner_i].x<<", "<<temp_sPH_inner_nocolumn_vec[inner_i].y<<", "<<temp_sPH_inner_nocolumn_vec[inner_i].z<<")"<<endl;
    //                 // cout<<"good pair : "<<good_pair_count<<" outer: ("<<temp_sPH_outer_nocolumn_vec[outer_i].x<<", "<<temp_sPH_outer_nocolumn_vec[outer_i].y<<", "<<temp_sPH_outer_nocolumn_vec[outer_i].z<<")"<<endl;
    //                 // cout<<"good pair : "<<good_pair_count<<" corrected phi, inner : "<<Clus_InnerPhi_Offset<<" outer : "<<Clus_OuterPhi_Offset<<endl;
    //                 // cout<<"good pair : "<<good_pair_count<<" DCA : "<<DCA_sign<<endl;
    //                 // cout<<"good pair : "<<good_pair_count<<" phi diff : "<<fabs(Clus_InnerPhi_Offset - Clus_OuterPhi_Offset)<<endl;
    //                 good_pair_count += 1;
    //                 // used_outer_check[outer_i] = 1; //note : this outer cluster was already used!

    //                 // pair<double,double> z_range_info = Get_possible_zvtx( 
    //                 //     get_radius(beam_origin.first,beam_origin.second), 
    //                 //     {get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y), temp_sPH_inner_nocolumn_vec[inner_i].z}, // note : unsign radius
    //                 //     {get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y), temp_sPH_outer_nocolumn_vec[outer_i].z}  // note : unsign radius
    //                 // );
                    
    //                 // note : we basically transform the coordinate from cartesian to cylinder 
    //                 // note : we should set the offset first, otherwise it provides the bias
    //                 // todo : which point should be used, DCA point or vertex xy ? Has to be studied 
    //                 pair<double,double> z_range_info = Get_possible_zvtx( 
    //                     0., // get_radius(beam_origin.first,beam_origin.second), 
    //                     {get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x - beam_origin.first, temp_sPH_inner_nocolumn_vec[inner_i].y - beam_origin.second), temp_sPH_inner_nocolumn_vec[inner_i].z}, // note : unsign radius
    //                     {get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x - beam_origin.first, temp_sPH_outer_nocolumn_vec[outer_i].y - beam_origin.second), temp_sPH_outer_nocolumn_vec[outer_i].z}  // note : unsign radius
    //                 );

    //                 // note : try to remove some crazy background candidates. Can be a todo
    //                 if (evt_possible_z_range.first < z_range_info.first && z_range_info.first < evt_possible_z_range.second) 
    //                 {
    //                     N_comb.push_back(good_comb_id);
    //                     N_comb_e.push_back(0);
    //                     // N_comb_phi.push_back( (temp_sPH_inner_nocolumn_vec[inner_i].phi + temp_sPH_outer_nocolumn_vec[outer_i].phi)/2. );
    //                     N_comb_phi.push_back( (Clus_InnerPhi_Offset + Clus_OuterPhi_Offset)/2. );
    //                     z_mid.push_back(z_range_info.first);
    //                     z_range.push_back(z_range_info.second);

    //                     evt_possible_z -> Fill(z_range_info.first);

    //                     // note : fill the line_breakdwon histogram as well as a vector for the width determination
    //                     line_breakdown(line_breakdown_hist,{z_range_info.first - z_range_info.second, z_range_info.first + z_range_info.second});

    //                     good_comb_id += 1;    
    //                 }

    //                 // DCA_tag = true;
    //             }

    //             // if(DCA_tag == true) break; // note : since this combination (one inner cluster, one outer cluster) satisfied the reuqiremet, no reason to ask this inner cluster try with other outer clusters

    //             // cout<<"good comb : "<<fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi)<<" radius in : "<<get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y)<<" radius out : "<<get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y)<<endl;
    //         } // note : end of if 
                

    //     } // note : end of outer loop
    // } // note : end of inner loop
    
    // cout<<"good pair count : "<<good_pair_count<<endl;

    // if (event_i == 906) {
    //     for (int hist_i = 0; hist_i < line_breakdown_hist->GetNbinsX(); hist_i++){
    //         cout<<line_breakdown_hist->GetBinContent(hist_i+1)<<","<<endl;
    //     }   
    // }
    N_track_candidate_Nclu -> Fill(total_NClus, N_comb.size());

    if (N_comb.size() > zvtx_cal_require)
    {   
        N_group_info = find_Ngroup(evt_possible_z);
        N_group_info_detail = find_Ngroup(line_breakdown_hist);
        
        // note : first fit is for the width, so apply the constraints on the Gaussian offset
        gaus_fit -> SetParameters(line_breakdown_hist -> GetBinContent( line_breakdown_hist -> GetMaximumBin() ), line_breakdown_hist -> GetBinCenter( line_breakdown_hist -> GetMaximumBin() ), 40, 0);
        gaus_fit -> SetParLimits(0,0,100000);  // note : size 
        gaus_fit -> SetParLimits(2,5,10000);   // note : Width
        gaus_fit -> SetParLimits(3,0,10000);   // note : offset
        // todo : try to use single gaus to fit the distribution, and try to only fit the peak region (peak - 100 mm + peak + 100 mm)
        line_breakdown_hist -> Fit(gaus_fit, "NQ", "", line_breakdown_hist -> GetBinCenter( line_breakdown_hist -> GetMaximumBin() ) - 90, line_breakdown_hist -> GetBinCenter( line_breakdown_hist -> GetMaximumBin() ) + 90);        
        tight_offset_peak = gaus_fit -> GetParameter(1);
        tight_offset_width = fabs(gaus_fit -> GetParameter(2));
        final_selection_widthU = (tight_offset_peak + tight_offset_width);
        final_selection_widthD = (tight_offset_peak - tight_offset_width);
        gaus_fit_offset = gaus_fit -> GetParameter(3); gaus_fit -> SetParameter(3,0); // note : in order to calculate the integration
        gaus_ratio = (fabs(gaus_fit -> GetParameter(0))/gaus_fit->Integral(-600,600)) / fabs(gaus_fit -> GetParameter(2));
        gaus_fit -> SetParameter(3, gaus_fit_offset); // note : put the offset back to the function

        // note : second fit is for the peak position, therefore, loose the constraints on the Gaussian offset
        gaus_fit -> SetParameters(line_breakdown_hist -> GetBinContent( line_breakdown_hist -> GetMaximumBin() ), line_breakdown_hist -> GetBinCenter( line_breakdown_hist -> GetMaximumBin() ), 40, 0);
        gaus_fit -> SetParLimits(0,0,100000);  // note : size 
        gaus_fit -> SetParLimits(2,5,10000);   // note : Width
        gaus_fit -> SetParLimits(3,-200,10000);   // note : offset
        // todo : try to use single gaus to fit the distribution, and try to only fit the peak region (peak - 100 mm + peak + 100 mm)
        line_breakdown_hist -> Fit(gaus_fit, "NQ", "", line_breakdown_hist -> GetBinCenter( line_breakdown_hist -> GetMaximumBin() ) - 90, line_breakdown_hist -> GetBinCenter( line_breakdown_hist -> GetMaximumBin() ) + 90);
        // line_breakdown_hist -> Fit(gaus_fit, "NQ", "", N_group_info_detail[2]-10, N_group_info_detail[3]+10);
        loose_offset_peak = gaus_fit -> GetParameter(1);
        loose_offset_peakE = gaus_fit -> GetParError(1);        
        
        // note : eff sigma method, relatively sensitive to the background
        // note : use z-mid to do the effi_sig, because that line_breakdown takes too long time
        temp_event_zvtx_info = sigmaEff_avg(z_mid,Integrate_portion);
        for (int track_i = 0; track_i < N_comb.size(); track_i++) {

            if ( N_group_info[2] <= z_mid[track_i] && z_mid[track_i] <= N_group_info[3] ){
                eff_N_comb.push_back(N_comb[track_i]);
                eff_z_mid.push_back(z_mid[track_i]);
                eff_N_comb_e.push_back(N_comb_e[track_i]);
                eff_z_range.push_back(z_range[track_i]);
            }

            if (final_selection_widthD <= z_mid[track_i] && z_mid[track_i] <= final_selection_widthU){
                // note : for monitoring the the phi distribution that is used for the z vertex determination.
                // note : in principle, I expect it should be something uniform. 
                evt_select_track_phi -> Fill(N_comb_phi[track_i]);
            }

            // if ((z_mid[track_i] - z_range[track_i]) > final_selection_widthU || (z_mid[track_i] + z_range[track_i]) < final_selection_widthD) {continue;}
            // else {
            //     // eff_N_comb.push_back(N_comb[track_i]);
            //     // eff_z_mid.push_back(z_mid[track_i]);
            //     // eff_N_comb_e.push_back(N_comb_e[track_i]);
            //     // eff_z_range.push_back(z_range[track_i]);
            // }
        }

        z_range_gr = new TGraphErrors(eff_N_comb.size(),&eff_N_comb[0],&eff_z_mid[0],&eff_N_comb_e[0],&eff_z_range[0]);
        z_range_gr -> Fit(zvtx_finder,"NQ","",0,N_comb[N_comb.size() - 1]);
        width_density_par = (double(eff_N_comb.size()) / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]));
        
        

        if ( zvtx_QA_width.first < tight_offset_width && tight_offset_width < zvtx_QA_width.second && 100 < fabs(N_group_info_detail[3] - N_group_info_detail[2]) && fabs(N_group_info_detail[3] - N_group_info_detail[2]) < 190){
            if (N_group_info[0] < 4 && N_group_info[1] >= 0.6 && N_group_info_detail[0] < 7 && N_group_info_detail[1] > 0.9) {good_zvtx_tag = true;}
            else {good_zvtx_tag = false;} 
        }
        else {good_zvtx_tag = false;}
        good_zvtx_tag_int = (good_zvtx_tag == true) ? 1 : 0; // note : so stupid way to convert bool to int...

        // if (good_zvtx_tag == false) {
        //     good_zvtx_tag = (zvtx_QA_width.first < fabs(gaus_fit -> GetParameter(2)) && fabs(gaus_fit -> GetParameter(2)) < zvtx_QA_width.second && gaus_ratio > zvtx_QA_ratio && width_density_par > width_density_cut) ? true : false;
        // } // note : kinda double check
        
        // note : for the group information post the background cut
        peak_group_width_hist -> Fill(fabs(N_group_info[3] - N_group_info[2]));
        peak_group_ratio_hist -> Fill(N_group_info[1]);
        N_group_hist -> Fill(N_group_info[0]);
        peak_group_detail_width_hist -> Fill(fabs(N_group_info_detail[3] - N_group_info_detail[2]));
        peak_group_detail_ratio_hist -> Fill(N_group_info_detail[1]);
        N_group_detail_hist -> Fill(N_group_info_detail[0]);

        // note : final
        final_fit_width -> Fill(total_NClus, fabs(final_selection_widthU - final_selection_widthD)); // note : from LB gaus for the moment
        final_zvtx =  loose_offset_peak; // zvtx_finder -> GetParameter(0);

        // note : gaus fit on the linebreak
        gaus_width_Nclu -> Fill(total_NClus, tight_offset_width);
        gaus_rchi2_Nclu -> Fill(total_NClus, gaus_fit -> GetChisquare() / double(gaus_fit -> GetNDF()));
        line_breakdown_gaus_ratio_hist -> Fill(gaus_ratio);
        line_breakdown_gaus_width_hist -> Fill(tight_offset_width);

        // note : the effi sig on the z_mid vector
        zvtx_evt_width_corre -> Fill(total_NClus, fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]));
        width_density -> Fill( width_density_par );
        ES_width -> Fill(fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]));
        ES_width_ratio -> Fill(total_NClus / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]));

        // note : regarding to the zvtx determination, pol0 fit with error bar considered, fit range given by eff sigma method
        zvtx_evt_fitError -> Fill(fabs(zvtx_finder -> GetParError(0)));
        zvtx_evt_fitError_corre -> Fill(total_NClus, fabs(zvtx_finder -> GetParError(0)));

        if ( good_zvtx_tag ){ 

            zvtx_evt_nclu_corre -> Fill(total_NClus, final_zvtx);
            avg_event_zvtx -> Fill(final_zvtx);
            avg_event_zvtx_vec.push_back(final_zvtx);
            if (run_type == "MC") {
                Z_resolution -> Fill( final_zvtx - (TrigZvtxMC * 10.) );
                Z_resolution_vec.push_back( final_zvtx - (TrigZvtxMC * 10.) );
                Z_resolution_Nclu -> Fill( total_NClus , final_zvtx - (TrigZvtxMC * 10.) );
                Z_resolution_pos -> Fill(TrigZvtxMC * 10., final_zvtx - (TrigZvtxMC * 10.));
                if (total_NClus > high_multi_line) {Z_resolution_pos_cut -> Fill(TrigZvtxMC * 10., final_zvtx - (TrigZvtxMC * 10.));}
            }
        
            
        }
        
        // note : output the root tree
        out_eID = event_i;
        N_cluster_inner_out = temp_sPH_inner_nocolumn_vec.size();
        N_cluster_outer_out = temp_sPH_outer_nocolumn_vec.size();

        out_ES_zvtx = zvtx_finder -> GetParameter(0);
        out_ES_zvtxE = zvtx_finder -> GetParError(0);
        out_ES_rangeL = temp_event_zvtx_info[1];
        out_ES_rangeR = temp_event_zvtx_info[2];
        out_ES_N_good = eff_N_comb.size();
        out_ES_width_density = width_density_par;

        out_LB_Gaus_Mean_mean = loose_offset_peak;
        out_LB_Gaus_Mean_meanE = gaus_fit -> GetParError(1); // note : -> loose one
        out_LB_Gaus_Mean_chi2 = gaus_fit -> GetChisquare() / double(gaus_fit -> GetNDF()); // note : -> loose one
        out_LB_Gaus_Mean_width = fabs(gaus_fit -> GetParameter(2));

        out_LB_Gaus_Width_width = tight_offset_width;
        out_LB_Gaus_Width_offset = gaus_fit_offset;
        out_LB_Gaus_Width_size_width = gaus_ratio;
        
        out_mid_cut_Ngroup = N_group_info[0];   
        out_mid_cut_peak_ratio = N_group_info[1];
        out_mid_cut_peak_width = fabs(N_group_info[3] - N_group_info[2]) / 2.;

        out_LB_cut_Ngroup = N_group_info_detail[0];   
        out_LB_cut_peak_ratio = N_group_info_detail[1];
        out_LB_cut_peak_width = fabs(N_group_info_detail[3] - N_group_info_detail[2]) / 2.;

        out_centrality_bin = centrality_bin;

        out_LB_geo_mean = LB_geo_mean(line_breakdown_hist, { (tight_offset_peak - tight_offset_width), (tight_offset_peak + tight_offset_width) }, event_i);
        out_good_zvtx_tag = good_zvtx_tag;
        bco_full_out = bco_full;
        MC_true_zvtx = TrigZvtxMC * 10.;
        tree_out -> Fill();

        out_LB_refit = -1;  // note : not in the out tree
        out_LB_refitE = -1; // note : not in the out tree

    } // note : if N good tracks in xy found > certain value
    else {tree_out -> Fill();} 

    if (N_comb.size() > zvtx_cal_require && draw_event_display == true)
    {   
        temp_event_xy = new TGraph(temp_sPH_nocolumn_vec[0].size(),&temp_sPH_nocolumn_vec[0][0],&temp_sPH_nocolumn_vec[1][0]);
        temp_event_xy -> SetTitle("INTT event display X-Y plane");
        temp_event_xy -> GetXaxis() -> SetLimits(-150,150);
        temp_event_xy -> GetYaxis() -> SetRangeUser(-150,150);
        temp_event_xy -> GetXaxis() -> SetTitle("X [mm]");
        temp_event_xy -> GetYaxis() -> SetTitle("Y [mm]");
        temp_event_xy -> SetMarkerStyle(20);
        temp_event_xy -> SetMarkerColor(2);
        temp_event_xy -> SetMarkerSize(1);
        temp_event_rz = new TGraph(temp_sPH_nocolumn_rz_vec[0].size(),&temp_sPH_nocolumn_rz_vec[0][0],&temp_sPH_nocolumn_rz_vec[1][0]);
        temp_event_rz -> SetTitle("INTT event display r-Z plane");
        temp_event_rz -> GetXaxis() -> SetLimits(-500,500);
        temp_event_rz -> GetYaxis() -> SetRangeUser(-150,150);
        temp_event_rz -> GetXaxis() -> SetTitle("Z [mm]");
        temp_event_rz -> GetYaxis() -> SetTitle("Radius [mm]");
        temp_event_rz -> SetMarkerStyle(20);
        temp_event_rz -> SetMarkerColor(2);
        temp_event_rz -> SetMarkerSize(1);

        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

        pad_xy -> cd();
        temp_bkg(pad_xy, peek, beam_origin, ch_pos_DB);
        temp_event_xy -> Draw("p same");
        for (int track_i = 0; track_i < good_track_xy_vec.size(); track_i++){
            track_line -> DrawLine(good_track_xy_vec[track_i][0],good_track_xy_vec[track_i][1],good_track_xy_vec[track_i][2],good_track_xy_vec[track_i][3]);
        }
        track_line -> Draw("l same");
        draw_text -> DrawLatex(0.2, 0.85, Form("eID : %i, inner Ncluster : %zu, outer Ncluster : %zu",event_i,temp_sPH_inner_nocolumn_vec.size(),temp_sPH_outer_nocolumn_vec.size()));

        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

        pad_rz -> cd();
        temp_event_rz -> Draw("ap");    
        // eff_sig_range_line -> DrawLine(temp_event_zvtx_info[0],-150,temp_event_zvtx_info[0],150);
        coord_line -> DrawLine(0,-150,0,150);
        coord_line -> DrawLine(-500,0,500,0);
        for (int track_i = 0; track_i < good_track_rz_vec.size(); track_i++){
            track_line -> DrawLine(good_track_rz_vec[track_i][0],good_track_rz_vec[track_i][1],good_track_rz_vec[track_i][2],good_track_rz_vec[track_i][3]);
        }
        draw_text -> DrawLatex(0.2, 0.85, Form("Negative radius : Clu_{outer} > 180^{0}"));
        // draw_text -> DrawLatex(0.2, 0.81, Form("EffSig avg : %.2f mm",temp_event_zvtx_info[0]));

        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        // cout<<"test tag 2-5"<<endl;    
        pad_z -> cd();
        z_range_gr_draw = new TGraphErrors(N_comb.size(),&N_comb[0],&z_mid[0],&N_comb_e[0],&z_range[0]);
        z_range_gr_draw -> GetYaxis() -> SetRangeUser(-650,650);
        z_range_gr_draw -> GetXaxis() -> SetTitle("Index");
        z_range_gr_draw -> GetYaxis() -> SetTitle("Z [mm]");
        z_range_gr_draw -> SetMarkerStyle(20);
        z_range_gr_draw -> Draw("ap");
        
        // eff_sig_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),temp_event_zvtx_info[1],z_range_gr_draw->GetXaxis()->GetXmax(),temp_event_zvtx_info[1]);
        // eff_sig_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),temp_event_zvtx_info[2],z_range_gr_draw->GetXaxis()->GetXmax(),temp_event_zvtx_info[2]);
        eff_sig_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),N_group_info[2],z_range_gr_draw->GetXaxis()->GetXmax(),N_group_info[2]);
        eff_sig_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),N_group_info[3],z_range_gr_draw->GetXaxis()->GetXmax(),N_group_info[3]);
        draw_text -> DrawLatex(0.2, 0.82, Form("#color[2]{Event Zvtx %.2f mm, error : #pm%.2f}", zvtx_finder -> GetParameter(0), zvtx_finder -> GetParError(0)));
        draw_text -> DrawLatex(0.2, 0.78, Form("#color[2]{Width density : %.6f}", ( width_density_par )));
        draw_text -> DrawLatex(0.2, 0.74, Form("#color[2]{Width (%.0f%%) : %.2f to %.2f mm #rightarrow %.2f mm}", Integrate_portion*100., temp_event_zvtx_info[2] , temp_event_zvtx_info[1], fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1])/2.));
        
        // todo : the final range finder is here 
        // final_fit_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),temp_event_zvtx_info[1],z_range_gr_draw->GetXaxis()->GetXmax(),temp_event_zvtx_info[1]);
        // final_fit_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),temp_event_zvtx_info[2],z_range_gr_draw->GetXaxis()->GetXmax(),temp_event_zvtx_info[2]);
        
        // z_range_gr_draw -> Draw("p same");
        zvtx_finder -> Draw("lsame");

        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        pad_z_hist -> cd();
        evt_possible_z -> SetMaximum(evt_possible_z -> GetBinContent( evt_possible_z->GetMaximumBin() ) * 1.4);
        evt_possible_z -> Draw("hist");
        eff_sig_range_line -> DrawLine(evt_possible_z->GetXaxis()->GetXmin(), evt_possible_z -> GetBinContent(evt_possible_z->GetMaximumBin())/2., evt_possible_z->GetXaxis()->GetXmax(), evt_possible_z -> GetBinContent(evt_possible_z->GetMaximumBin())/2.);
        draw_text -> DrawLatex(0.2, 0.82, Form("N group : %.0f", N_group_info[0]));
        draw_text -> DrawLatex(0.2, 0.78, Form("Main peak ratio : %.2f", N_group_info[1]));
        draw_text -> DrawLatex(0.2, 0.74, Form("good z tag : %i", good_zvtx_tag));
        
        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        pad_z_line -> cd();
        line_breakdown_hist -> SetMinimum(0);
        line_breakdown_hist -> SetMaximum(line_breakdown_hist -> GetBinContent( line_breakdown_hist->GetMaximumBin() ) * 2);
        line_breakdown_hist -> Draw("hist");
        gaus_fit -> Draw("lsame");
        if (!good_zvtx_tag) {final_fit_range_line -> DrawLine(line_breakdown_hist->GetXaxis()->GetXmin(), line_breakdown_hist -> GetMaximum(), line_breakdown_hist->GetXaxis()->GetXmax(), line_breakdown_hist -> GetMinimum());}
        final_fit_range_line -> DrawLine(final_selection_widthD, line_breakdown_hist -> GetMinimum(),final_selection_widthD, line_breakdown_hist -> GetMaximum());
        final_fit_range_line -> DrawLine(final_selection_widthU, line_breakdown_hist -> GetMinimum(),final_selection_widthU, line_breakdown_hist -> GetMaximum());
        draw_text -> DrawLatex(0.2, 0.82, Form("Gaus mean %.2f mm", loose_offset_peak));
        draw_text -> DrawLatex(0.2, 0.78, Form("Width : %.2f mm", tight_offset_width));
        draw_text -> DrawLatex(0.2, 0.74, Form("Reduced #chi2 : %.3f", gaus_fit -> GetChisquare() / double(gaus_fit -> GetNDF())));
        draw_text -> DrawLatex(0.2, 0.70, Form("Norm. entry / Width : %.6f mm",  gaus_ratio  ));
        draw_text -> DrawLatex(0.2, 0.66, Form("LB Geo mean : %.3f mm",  out_LB_geo_mean  ));

        if (N_group_info_detail[0] != -1){
            eff_sig_range_line -> DrawLine(line_breakdown_hist->GetXaxis()->GetXmin(), line_breakdown_hist -> GetBinContent(line_breakdown_hist->GetMaximumBin())/2., line_breakdown_hist->GetXaxis()->GetXmax(), line_breakdown_hist -> GetBinContent(line_breakdown_hist->GetMaximumBin())/2.);
            draw_text -> DrawLatex(0.2, 0.62, Form("N group : %.0f", N_group_info_detail[0]));
            draw_text -> DrawLatex(0.2, 0.58, Form("Main peak ratio : %.2f", N_group_info_detail[1]));
        }

        if (run_type == "MC") {draw_text -> DrawLatex(0.2, 0.54, Form("True MCz : %.3f mm",  TrigZvtxMC * 10.));}
        
        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        pad_phi_diff -> cd();
        evt_phi_diff_inner_phi -> Draw("colz0");
        
        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        pad_track_phi -> cd();
        evt_select_track_phi -> Draw("hist");

        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        pad_inner_outer_phi -> cd();
        evt_inner_outer_phi -> Draw("colz0");

        // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        pad_phi_diff_1D -> cd();
        evt_phi_diff_1D -> Draw("hist");


        // temp_event_zvtx -> Draw("hist");
        // // zvtx_fitting -> Draw("lsame");
        // eff_sig_range_line -> DrawLine(temp_event_zvtx_info[1],0,temp_event_zvtx_info[1],temp_event_zvtx -> GetMaximum()*1.05);
        // eff_sig_range_line -> DrawLine(temp_event_zvtx_info[2],0,temp_event_zvtx_info[2],temp_event_zvtx -> GetMaximum()*1.05);
        // draw_text -> DrawLatex(0.2, 0.85, Form("eID : %i, Total event hit : %i, innter Ncluster : %i, outer Ncluster : %i",event_i,N_hits,temp_sPH_inner_nocolumn_vec.size(),temp_sPH_outer_nocolumn_vec.size()));
        // // draw_text -> DrawLatex(0.2, 0.84, Form("Gaus fit mean : %.3f mm",zvtx_fitting -> GetParameter(1)));
        // draw_text -> DrawLatex(0.2, 0.82, Form("EffSig min : %.2f mm, max : %.2f mm",temp_event_zvtx_info[1],temp_event_zvtx_info[2]));
        // draw_text -> DrawLatex(0.2, 0.79, Form("EffSig avg : %.2f mm",temp_event_zvtx_info[0]));
        
        if(draw_event_display && (event_i % print_rate) == 0 /*&& good_zvtx_tag == true*/){c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str()));}
        // else if(good_zvtx_tag == false){c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str()));}
        else if(draw_event_display && (final_zvtx > 0 || final_zvtx < -450)) {cout<<"In INTTZvtx class, event :"<<event_i<<" weird zvtx "<<endl; c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str()));}
        // else if(total_NClus > 5000) {cout<<"In INTTZvtx class, event :"<<event_i<<" high Nclus "<<endl; c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str()));}
        else if ( draw_event_display && run_type == "MC" && fabs(final_zvtx - (TrigZvtxMC * 10.)) > 2. && total_NClus > 3000) { cout<<"In INTTZvtx class, event :"<<event_i<<" High NClus, poor Z : "<< fabs(final_zvtx - (TrigZvtxMC * 10.)) <<endl; c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str())); }
        else if ( draw_event_display && run_type == "MC" && fabs(final_zvtx - (TrigZvtxMC * 10.)) > 2.) { cout<<"In INTTZvtx class, event :"<<event_i<<" low NClus, poor Z : "<< fabs(final_zvtx - (TrigZvtxMC * 10.)) <<endl; c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str())); }

        // if(draw_event_display && good_zvtx_tag == true){c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str()));}

        // if(draw_event_display && (event_i % print_rate) == 0){c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str()));}
        // if(fabs(gaus_fit -> GetParameter(2)) > 40) { 
        //     cout<<"check event : "<<event_i<<" width : "<<fabs(gaus_fit -> GetParameter(2))<<" NClu : "<<total_NClus<<endl; 
        //     c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str())); 
        // }
        // if (fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1])>100){
        //     cout<<"check event : "<<event_i<<" eff_width : "<<fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1])<<" NClu : "<<total_NClus<<endl; 
        //     c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str()));
        // }
        pad_xy -> Clear();
        pad_rz -> Clear();
        pad_z  -> Clear();
        pad_z_hist -> Clear();
        pad_z_line -> Clear();
        pad_phi_diff -> Clear();
        pad_track_phi -> Clear();
        pad_inner_outer_phi -> Clear();
        pad_phi_diff_1D -> Clear();

        // temp_event_xy -> Delete();
        // temp_event_rz -> Delete();
        // z_range_gr_draw -> Delete();

    }
    
}

void INTTZvtx::ClearEvt()
{
    good_comb_id = 0;
    out_N_cluster_north = 0;
    out_N_cluster_south = 0;

    // note : ultra-stupid way to avoid the errors
    z_range_gr_draw = new TGraphErrors(); z_range_gr_draw -> Delete();
    z_range_gr = new TGraphErrors(); z_range_gr -> Delete();
    temp_event_xy = new TGraph(); temp_event_xy -> Delete();
    temp_event_rz = new TGraph(); temp_event_rz -> Delete();
    bkg = new TGraph(); bkg -> Delete();
    
    N_comb_phi.clear();
    N_comb.clear();
    N_comb_e.clear();
    z_mid.clear(); 
    z_range.clear();

    eff_N_comb.clear();
    eff_z_mid.clear();
    eff_N_comb_e.clear();
    eff_z_range.clear();
    line_breakdown_vec.clear();
    temp_event_zvtx_info = {0,-1000,-999.99};

    LB_N_comb.clear(); 
    LB_z_mid.clear(); 
    LB_N_comb_e.clear(); 
    LB_z_range.clear();
    N_group_info.clear();
    N_group_info_detail = {-1., -1., -1., -1.};

    evt_possible_z -> Reset("ICESM");
    line_breakdown_hist -> Reset("ICESM");
    evt_phi_diff_inner_phi -> Reset("ICESM");
    evt_select_track_phi -> Reset("ICESM");
    evt_inner_outer_phi -> Reset("ICESM");
    evt_phi_diff_1D -> Reset("ICESM");

    inner_clu_phi_map.clear();
    outer_clu_phi_map.clear();
    inner_clu_phi_map = vector<vector<pair<bool,clu_info>>>(360);
    outer_clu_phi_map = vector<vector<pair<bool,clu_info>>>(360);

    mega_track_finder -> ClearEvt();

    // note : this is the distribution for full run
    // line_breakdown_gaus_ratio_hist -> Reset("ICESM");
}

void INTTZvtx::PrintPlots()
{
    if (draw_event_display) {c2 -> Print(Form("%s/temp_event_display.pdf)",out_folder_directory.c_str()));}
    c2 -> Clear();
    c1 -> Clear();

    cout<<"avg_event_zvtx_vec size : "<<avg_event_zvtx_vec.size()<<endl;

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    c1 -> cd();
    vector<float> avg_event_zvtx_info = {0,0,0};
    if (avg_event_zvtx_vec.size() > 10) {avg_event_zvtx_info = sigmaEff_avg(avg_event_zvtx_vec,Integrate_portion_final);}

    avg_event_zvtx -> SetMinimum( 0 );  avg_event_zvtx -> SetMaximum( avg_event_zvtx->GetBinContent(avg_event_zvtx->GetMaximumBin()) * 1.5 );
    avg_event_zvtx -> Draw("hist");

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    // ltx->DrawLatex(0.54, 0.86, Form("Run %s",run_ID.c_str()));
    // ltx->DrawLatex(0.54, 0.86, "Au+Au #sqrt{s_{NN}} = 200 GeV");

    eff_sig_range_line -> DrawLine(avg_event_zvtx_info[1],0,avg_event_zvtx_info[1],avg_event_zvtx -> GetMaximum());
    eff_sig_range_line -> DrawLine(avg_event_zvtx_info[2],0,avg_event_zvtx_info[2],avg_event_zvtx -> GetMaximum());    
    draw_text -> DrawLatex(0.21, 0.87, Form("EffSig min : %.2f mm",avg_event_zvtx_info[1]));
    draw_text -> DrawLatex(0.21, 0.83, Form("EffSig max : %.2f mm",avg_event_zvtx_info[2]));
    draw_text -> DrawLatex(0.21, 0.79, Form("EffSig avg : %.2f mm",avg_event_zvtx_info[0]));
    c1 -> Print(Form("%s/avg_event_zvtx.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    width_density -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/width_density.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    ES_width -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/ES_width.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    ES_width_ratio -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/ES_width_ratio.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    zvtx_evt_fitError -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/zvtx_evt_fitError.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (run_type == "MC")
    {
        vector<float> Z_resolution_vec_info = sigmaEff_avg(Z_resolution_vec,Integrate_portion_final);

        gaus_fit_2 -> SetParameters(Z_resolution -> GetBinContent( Z_resolution -> GetMaximumBin() ), Z_resolution -> GetBinCenter( Z_resolution -> GetMaximumBin() ), 3, 0);
        gaus_fit_2 -> SetParLimits(0,0,100000);  // note : size 
        gaus_fit_2 -> SetParLimits(2,0,10000);   // note : Width
        gaus_fit_2 -> SetParLimits(3,0,10000);   // note : offset
        Z_resolution -> Fit(gaus_fit_2, "NQ", "", Z_resolution -> GetBinCenter( Z_resolution -> GetMaximumBin() ) - (2 * Z_resolution -> GetStdDev() ), Z_resolution -> GetBinCenter( Z_resolution -> GetMaximumBin() ) + (2 * Z_resolution -> GetStdDev() ) );
        
        Z_resolution -> Draw("hist"); 
        gaus_fit_2 -> SetRange( gaus_fit_2->GetParameter(1) - gaus_fit_2->GetParameter(2) * 2.5, gaus_fit_2->GetParameter(1) + gaus_fit_2->GetParameter(2) * 2.5 ); 
        gaus_fit_2 -> Draw("lsame");
        eff_sig_range_line -> DrawLine(Z_resolution_vec_info[1],0,Z_resolution_vec_info[1],Z_resolution -> GetMaximum());
        eff_sig_range_line -> DrawLine(Z_resolution_vec_info[2],0,Z_resolution_vec_info[2],Z_resolution -> GetMaximum());    
        draw_text -> DrawLatex(0.21, 0.87, Form("EffSig min : %.2f mm",Z_resolution_vec_info[1]));
        draw_text -> DrawLatex(0.21, 0.83, Form("EffSig max : %.2f mm",Z_resolution_vec_info[2]));
        draw_text -> DrawLatex(0.21, 0.79, Form("EffSig avg : %.2f mm",Z_resolution_vec_info[0]));
        draw_text -> DrawLatex(0.21, 0.71, Form("Gaus mean  : %.2f mm",gaus_fit_2 -> GetParameter(1)));
        draw_text -> DrawLatex(0.21, 0.67, Form("Gaus width : %.2f mm",fabs(gaus_fit_2 -> GetParameter(2))));
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_resolution.pdf",out_folder_directory.c_str()));
        c1 -> Clear();

        MC_z_diff_peak = gaus_fit_2 -> GetParameter(1);
        MC_z_diff_width = fabs(gaus_fit_2 -> GetParameter(2));
    }


    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (run_type == "MC")
    {    
        Z_resolution_Nclu -> Draw("colz0"); 
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_resolution_Nclu.pdf",out_folder_directory.c_str()));
        c1 -> Clear();
    }
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (run_type == "MC")
    {
        Z_resolution_pos -> Draw("colz0"); 
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_resolution_pos.pdf",out_folder_directory.c_str()));
        c1 -> Clear();
    }
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (run_type == "MC")
    {
        Z_resolution_pos_cut -> Draw("colz0"); 
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_resolution_pos_cut.pdf",out_folder_directory.c_str()));
        c1 -> Clear();
    }
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    zvtx_evt_fitError_corre -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/zvtx_evt_fitError_corre.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    zvtx_evt_nclu_corre -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/zvtx_evt_nclu_corre.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    zvtx_evt_width_corre -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/zvtx_evt_width_corre.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    gaus_width_Nclu -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/gaus_width_Nclu.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    gaus_rchi2_Nclu -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/gaus_rchi2_Nclu.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    final_fit_width -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/final_fit_width.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    N_track_candidate_Nclu -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/N_track_candidate_Nclu.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    peak_group_width_hist -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/peak_group_width_hist.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    peak_group_ratio_hist -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/peak_group_ratio_hist.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    N_group_hist -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/N_group_hist.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    peak_group_detail_width_hist -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/peak_group_detail_width_hist.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    peak_group_detail_ratio_hist -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/peak_group_detail_ratio_hist.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    N_group_detail_hist -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/N_group_detail_hist.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // gaus_fit -> SetParameters(line_breakdown_gaus_ratio_hist -> GetBinContent( line_breakdown_gaus_ratio_hist -> GetMaximumBin() ), line_breakdown_gaus_ratio_hist -> GetBinCenter( line_breakdown_gaus_ratio_hist -> GetMaximumBin() ), 0.00005, 0);
    // gaus_fit -> SetParLimits(0,0,100000);  // note : size 
    // gaus_fit -> SetParLimits(2,0,10000);   // note : Width
    // gaus_fit -> SetParLimits(3,0,10000);   // note : offset

    line_breakdown_gaus_ratio_hist -> Draw("hist"); 
    // line_breakdown_gaus_ratio_hist -> Fit(gaus_fit, "NQ", "", line_breakdown_gaus_ratio_hist -> GetBinCenter( line_breakdown_gaus_ratio_hist -> GetMaximumBin() ) - 1, line_breakdown_gaus_ratio_hist -> GetBinCenter( line_breakdown_gaus_ratio_hist -> GetMaximumBin() ) + 1);
    // gaus_fit -> Draw("lsame");

    // draw_text -> DrawLatex(0.45, 0.82, Form("Gaus mean %.2f mm", gaus_fit -> GetParameter(1)));
    // draw_text -> DrawLatex(0.45, 0.78, Form("Width : %.2f mm", fabs(gaus_fit -> GetParameter(2))));
    // draw_text -> DrawLatex(0.45, 0.74, Form("Reduced #chi2 : %.3f", gaus_fit -> GetChisquare() / double(gaus_fit -> GetNDF())));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));

    c1 -> Print(Form("%s/line_breakdown_gaus_ratio_hist.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    gaus_fit -> SetParameters(line_breakdown_gaus_width_hist -> GetBinContent( line_breakdown_gaus_width_hist -> GetMaximumBin() ), line_breakdown_gaus_width_hist -> GetBinCenter( line_breakdown_gaus_width_hist -> GetMaximumBin() ), 8, 0);
    gaus_fit -> SetParLimits(0,0,100000);  // note : size 
    gaus_fit -> SetParLimits(2,1,10000);   // note : Width
    gaus_fit -> SetParLimits(3,0,10000);   // note : offset

    // todo : try to use single gaus to fit the distribution, and try to only fit the peak region (peak - 100 mm + peak + 100 mm)
    line_breakdown_gaus_width_hist -> Fit(gaus_fit, "NQ", "", line_breakdown_gaus_width_hist -> GetBinCenter( line_breakdown_gaus_width_hist -> GetMaximumBin() ) - 100, line_breakdown_gaus_width_hist -> GetBinCenter( line_breakdown_gaus_width_hist -> GetMaximumBin() ) + 100);
    line_breakdown_gaus_width_hist -> Draw("hist"); 
    gaus_fit -> Draw("lsame");
    
    draw_text -> DrawLatex(0.2, 0.82, Form("Gaus mean %.2f mm", gaus_fit -> GetParameter(1)));
    draw_text -> DrawLatex(0.2, 0.78, Form("Width : %.2f mm", fabs(gaus_fit -> GetParameter(2))));
    draw_text -> DrawLatex(0.2, 0.74, Form("Reduced #chi2 : %.3f", gaus_fit -> GetChisquare() / double(gaus_fit -> GetNDF())));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    
    c1 -> Print(Form("%s/line_breakdown_gaus_width_hist.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
}

void INTTZvtx::EndRun()
{   
    out_file -> cd();
    tree_out -> SetDirectory(out_file);
    tree_out -> Write("", TObject::kOverwrite);

    out_file -> Close();
}

double INTTZvtx::GetZdiffPeakMC()
{
    if (run_type == "MC" && MC_z_diff_peak != -777.)
    {
        return MC_z_diff_peak;
    }
    else 
    {
        cout<<"In INTTZvtx. Are you playing with data? The MC_z_diff_peak wasn't assigned, the value is still -777. Pleak check"<<endl;
        return -777.;
    }
}

double INTTZvtx::GetZdiffWidthMC()
{
    if (run_type == "MC" && MC_z_diff_width != -777.)
    {
        return MC_z_diff_width;
    }
    else 
    {
        cout<<"In INTTZvtx. Are you playing with data? The MC_z_diff_width wasn't assigned, the value is still -777. Pleak check"<<endl;
        return -777.;
    }
}

vector<double> INTTZvtx::GetEvtZPeak()
{
    return{good_zvtx_tag_int, loose_offset_peak, loose_offset_peakE};
}

double INTTZvtx::get_radius(double x, double y)
{
    return sqrt(pow(x,2)+pow(y,2));
}

std::vector<double> INTTZvtx::calculateDistanceAndClosestPoint(double x1, double y1, double x2, double y2, double target_x, double target_y) 
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
double INTTZvtx::calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY) {
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

void INTTZvtx::temp_bkg(TPad * c1, double peek, pair<double,double> beam_origin, InttConversion * ch_pos_DB)
{
    c1 -> cd();

    int N_ladder[4] = {12, 12, 16, 16};
    string ladder_index_string[16] = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15"};

    vector<double> x_vec; x_vec.clear();
    vector<double> y_vec; y_vec.clear();

    vector<double> x_vec_2; x_vec_2.clear();
    vector<double> y_vec_2; y_vec_2.clear();

    bkg = new TGraph();
    bkg -> SetTitle("INTT event display X-Y plane");
    bkg -> SetMarkerStyle(20);
    bkg -> SetMarkerSize(0.1);
    bkg -> SetPoint(0,0,0);
    bkg -> SetPoint(1,beam_origin.first,beam_origin.second);
    bkg -> GetXaxis() -> SetLimits(-150,150);
    bkg -> GetYaxis() -> SetRangeUser(-150,150);
    bkg -> GetXaxis() -> SetTitle("X [mm]");
    bkg -> GetYaxis() -> SetTitle("Y [mm]");
    
    bkg -> Draw("ap");

    

    for (int server_i = 0; server_i < 4; server_i++)
    {
        for (int FC_i = 0; FC_i < 14; FC_i++)
        {
            ladder_line -> DrawLine(
                ch_pos_DB -> Get_XY_all(Form("intt%i",server_i),FC_i,14,0).x, ch_pos_DB -> Get_XY_all(Form("intt%i",server_i),FC_i,14,0).y,
                ch_pos_DB -> Get_XY_all(Form("intt%i",server_i),FC_i,1,0).x, ch_pos_DB -> Get_XY_all(Form("intt%i",server_i),FC_i,1,0).y
            );
        }
    }
    
    ladder_line -> Draw("l same");

}

double INTTZvtx::Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y) // note : x : z, y : r
{
    if ( fabs(p0x - p1x) < 0.00001 ){ // note : the line is vertical (if z is along the x axis)
        return p0x;
    }
    else {
        double slope = (p1y - p0y) / (p1x - p0x);
        double yIntercept = p0y - slope * p0x;
        double xCoordinate = (given_y - yIntercept) / slope;
        return xCoordinate;
    }
}

pair<double,double> INTTZvtx::Get_possible_zvtx(double rvtx, vector<double> p0, vector<double> p1) // note : inner p0, outer p1, vector {r,z}, -> {y,x}
{
    vector<double> p0_z_edge = { ( fabs( p0[1] ) < 130 ) ? p0[1] - 8. : p0[1] - 10., ( fabs( p0[1] ) < 130 ) ? p0[1] + 8. : p0[1] + 10.}; // note : vector {left edge, right edge}
    vector<double> p1_z_edge = { ( fabs( p1[1] ) < 130 ) ? p1[1] - 8. : p1[1] - 10., ( fabs( p1[1] ) < 130 ) ? p1[1] + 8. : p1[1] + 10.}; // note : vector {left edge, right edge}

    double edge_first  = Get_extrapolation(rvtx,p0_z_edge[0],p0[0],p1_z_edge[1],p1[0]);
    double edge_second = Get_extrapolation(rvtx,p0_z_edge[1],p0[0],p1_z_edge[0],p1[0]);

    double mid_point = (edge_first + edge_second) / 2.;
    double possible_width = fabs(edge_first - edge_second) / 2.;

    return {mid_point, possible_width}; // note : first : mid point, second : width

}

int INTTZvtx::min_width_ID(vector<double> input_width_vec)
{
    double big_num;
    int big_num_id;
    for (int i = 0; i < input_width_vec.size(); i++)
    {
        if (i == 0) {
            big_num = input_width_vec[i];
            big_num_id = i;
        }
        else if ( input_width_vec[i] < big_num )
        {
            big_num = input_width_vec[i];
            big_num_id = i;
        }
    }

    return big_num_id;
}

void INTTZvtx::line_breakdown(TH1F* hist_in, pair<double,double> line_range)
{ 
    int first_bin = int((line_range.first  - hist_in->GetXaxis()->GetXmin()) /  hist_in->GetBinWidth(1)) + 1;
    int last_bin  = int((line_range.second - hist_in->GetXaxis()->GetXmin()) /  hist_in->GetBinWidth(1)) + 1;
    
    if (first_bin < 1) {first_bin = 0;}
    else if (first_bin > hist_in -> GetNbinsX()) {first_bin = hist_in -> GetNbinsX() + 1;}

    if (last_bin < 1) {last_bin = 0;}
    else if (last_bin > hist_in -> GetNbinsX()) {last_bin = hist_in -> GetNbinsX() + 1;}

    // cout<<"Digitize the bin : "<<first_bin<<" "<<last_bin<<endl;

    // note : if first:last = (0:0) or (N+1:N+1) -> the subtraction of them euqals to zero.
    for (int i = 0; i < (last_bin - first_bin) + 1; i++){
        hist_in -> SetBinContent(first_bin + i,  hist_in -> GetBinContent(first_bin + i) + 1 );
        // line_breakdown_vec.push_back(hist_in -> GetBinCenter(first_bin + i));
    }

}

// note : search_range : should be the gaus fit range
double INTTZvtx::LB_geo_mean(TH1F * hist_in, pair<double, double> search_range, int event_i)
{
    int    Highest_bin_index   = hist_in -> GetMaximumBin();
    double Highest_bin_center  = hist_in -> GetBinCenter(Highest_bin_index);
    double Highest_bin_content = hist_in -> GetBinContent(Highest_bin_index);
    if (Highest_bin_center < search_range.first || search_range.second < Highest_bin_center ) 
    {
        // cout<<"In INTTZvtx class, event : "<<event_i<<", interesting event, different group was fit"<<endl;
        return -999.;
    }

    vector<float> same_height_bin; same_height_bin.clear(); same_height_bin.push_back(Highest_bin_center);

    int search_index = 1;
    while( (hist_in -> GetBinCenter(Highest_bin_index + search_index)) < search_range.second ){
        if ( hist_in -> GetBinContent(Highest_bin_index + search_index) == Highest_bin_content ) {
            same_height_bin.push_back( hist_in -> GetBinCenter(Highest_bin_index + search_index) );
        }
        search_index ++;
    }

    // cout<<"test, search_index right : "<<search_index<<endl;

    search_index = 1;
    while( search_range.first < (hist_in -> GetBinCenter(Highest_bin_index - search_index)) ){
        if ( hist_in -> GetBinContent(Highest_bin_index - search_index) == Highest_bin_content ) {
            same_height_bin.push_back( hist_in -> GetBinCenter(Highest_bin_index - search_index) );
        }
        search_index ++;
    }

    // cout<<"test, search_index left : "<<search_index<<endl;
    // cout<<"test, same_height_bin.size(): "<<same_height_bin.size()<<endl;

    return accumulate( same_height_bin.begin(), same_height_bin.end(), 0.0 ) / double(same_height_bin.size());

}

//  note : N group, group size, group tag, group width ?  // note : {group size, group entry, group tag, group widthL, group widthR}
// note : {N_group, ratio (if two), peak widthL, peak widthR}
vector<double> INTTZvtx::find_Ngroup(TH1F * hist_in)
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

double INTTZvtx::get_delta_phi(double angle_1, double angle_2)
{
    vector<double> vec_abs = {fabs(angle_1 - angle_2), fabs(angle_1 - angle_2 + 360), fabs(angle_1 - angle_2 - 360)};
    vector<double> vec = {(angle_1 - angle_2), (angle_1 - angle_2 + 360), (angle_1 - angle_2 - 360)};
    return vec[std::distance(vec_abs.begin(), std::min_element(vec_abs.begin(),vec_abs.end()))];
}

double INTTZvtx::get_track_phi(double inner_clu_phi_in, double delta_phi_in)
{
    double track_phi = inner_clu_phi_in - (delta_phi_in/2.);
    if (track_phi < 0) {track_phi += 360;}
    else if (track_phi > 360) {track_phi -= 360;}
    else if (track_phi == 360) {track_phi = 0;}
    else {track_phi = track_phi;}
    return track_phi;
}
#endif



// if (N_comb.size() > zvtx_cal_require)
// {   
//     gaus_fit -> SetParameters(evt_possible_z -> GetBinContent( evt_possible_z -> GetMaximumBin() ), evt_possible_z -> GetBinCenter( evt_possible_z -> GetMaximumBin() ), 40, 0);
//     gaus_fit -> SetParLimits(0,0,100000);  // note : size 
//     gaus_fit -> SetParLimits(2,5,10000);   // note : Width
//     gaus_fit -> SetParLimits(3,0,10000);   // note : offset
//     // todo : try to use single gaus to fit the distribution, and try to only fit the peak region (peak - 100 mm + peak + 100 mm)
//     evt_possible_z -> Fit(gaus_fit, "NQ", "", evt_possible_z -> GetBinCenter( evt_possible_z -> GetMaximumBin() ) - 100, evt_possible_z -> GetBinCenter( evt_possible_z -> GetMaximumBin() ) + 100);

//     if (total_NClus < high_multi_line) {
//         temp_event_zvtx_info = sigmaEff_avg(z_mid,Integrate_portion);
//         final_selection_widthU = ( fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) / 2. < fabs(gaus_fit -> GetParameter(2)) ) ? temp_event_zvtx_info[2] : (gaus_fit -> GetParameter(1) + fabs(gaus_fit -> GetParameter(2)));
//         final_selection_widthD = ( fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) / 2. < fabs(gaus_fit -> GetParameter(2)) ) ? temp_event_zvtx_info[1] : (gaus_fit -> GetParameter(1) - fabs(gaus_fit -> GetParameter(2)));
//     }
//     else { // note : N clusters >= 1000

//         // note : too many parameters that aren't so easy to control
//         // double_gaus_fit -> SetParameters(evt_possible_z -> GetBinContent( evt_possible_z -> GetMaximumBin() ), evt_possible_z -> GetBinCenter( evt_possible_z -> GetMaximumBin() ), 40, evt_possible_z -> GetBinContent( evt_possible_z -> GetMaximumBin() ) * 0.5, evt_possible_z -> GetBinCenter( evt_possible_z -> GetMaximumBin() ), 100, 0);
//         // double_gaus_fit -> SetParLimits(0,0,100000); // note : G1 size
//         // double_gaus_fit -> SetParLimits(2,5,100000); // note : G1 width
//         // double_gaus_fit -> SetParLimits(3,0,100000); // note : G2 size
//         // double_gaus_fit -> SetParLimits(5,5,100000); // note : G2 width
//         // evt_possible_z -> Fit(double_gaus_fit, "NQ");

//         temp_event_zvtx_info = {0,-1000,-999.99};

//         // note : single gaus fit (only peak region), Di_gaus first gau, Di_gaus second gau
//         // fit_winner_id = min_width_ID({fabs(gaus_fit -> GetParameter(2)), fabs(double_gaus_fit -> GetParameter(2)), fabs(double_gaus_fit -> GetParameter(5))});
//         fit_winner_id = 0;
        
//         if (fit_winner_id == 0){
//             final_selection_widthU = (gaus_fit -> GetParameter(1) + fabs(gaus_fit -> GetParameter(2)));
//             final_selection_widthD = (gaus_fit -> GetParameter(1) - fabs(gaus_fit -> GetParameter(2)));
//         }
//         else if (fit_winner_id == 1){
//             final_selection_widthU = (double_gaus_fit -> GetParameter(1) + fabs(double_gaus_fit -> GetParameter(2)));
//             final_selection_widthD = (double_gaus_fit -> GetParameter(1) - fabs(double_gaus_fit -> GetParameter(2)));
//         }
//         else if (fit_winner_id == 2){
//             final_selection_widthU = (double_gaus_fit -> GetParameter(4) + fabs(double_gaus_fit -> GetParameter(5)));
//             final_selection_widthD = (double_gaus_fit -> GetParameter(4) - fabs(double_gaus_fit -> GetParameter(5)));
//         }
        
//     }


//     for (int track_i = 0; track_i < N_comb.size(); track_i++) {
//         if ( final_selection_widthD <= z_mid[track_i] && z_mid[track_i] <= final_selection_widthU ){
//             eff_N_comb.push_back(N_comb[track_i]);
//             eff_z_mid.push_back(z_mid[track_i]);
//             eff_N_comb_e.push_back(N_comb_e[track_i]);
//             eff_z_range.push_back(z_range[track_i]);
//         }
//     }

//     z_range_gr = new TGraphErrors(eff_N_comb.size(),&eff_N_comb[0],&eff_z_mid[0],&eff_N_comb_e[0],&eff_z_range[0]);
//     z_range_gr -> Fit(zvtx_finder,"NQ","",0,N_comb[N_comb.size() - 1]); // note : not fit all the combination

//     final_fit_width -> Fill(total_NClus, fabs(final_selection_widthU - final_selection_widthD));

//     gaus_width_Nclu -> Fill(total_NClus, fabs(gaus_fit -> GetParameter(2)));
//     gaus_rchi2_Nclu -> Fill(total_NClus, gaus_fit -> GetChisquare() / double(gaus_fit -> GetNDF()));
    
//     // avg_event_zvtx -> Fill(zvtx_finder -> GetParameter(0));
//     zvtx_evt_fitError -> Fill(fabs( zvtx_finder -> GetParError(0)));
//     zvtx_evt_fitError_corre -> Fill(total_NClus, fabs( zvtx_finder -> GetParError(0)));
//     zvtx_evt_width_corre -> Fill(total_NClus, fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]));
//     width_density -> Fill( width_density_par );

//     if ( ( width_density_par ) > 0.3 ){ // Todo : change the width density here

//         zvtx_evt_nclu_corre -> Fill(total_NClus, zvtx_finder -> GetParameter(0));
//         avg_event_zvtx -> Fill(zvtx_finder -> GetParameter(0));
//         avg_event_zvtx_vec.push_back(zvtx_finder -> GetParameter(0));
//         if (run_type == "MC")
//         {
//             Z_resolution -> Fill( zvtx_finder -> GetParameter(0) - (TrigZvtxMC * 10.) );
//             Z_resolution_vec.push_back( zvtx_finder -> GetParameter(0) - (TrigZvtxMC * 10.) );
//             Z_resolution_Nclu -> Fill( total_NClus , zvtx_finder -> GetParameter(0) - (TrigZvtxMC * 10.) );
//             Z_resolution_pos -> Fill(TrigZvtxMC * 10., zvtx_finder -> GetParameter(0) - (TrigZvtxMC * 10.));
//             if (total_NClus > high_multi_line) {Z_resolution_pos_cut -> Fill(TrigZvtxMC * 10., zvtx_finder -> GetParameter(0) - (TrigZvtxMC * 10.));}
//         }

//     }
    
//     out_eID = event_i;
//     N_cluster_inner_out = temp_sPH_inner_nocolumn_vec.size();
//     N_cluster_outer_out = temp_sPH_outer_nocolumn_vec.size();
//     out_zvtx = zvtx_finder -> GetParameter(0);
//     out_zvtxE = zvtx_finder -> GetParError(0);
//     out_rangeL = temp_event_zvtx_info[1];
//     out_rangeR = temp_event_zvtx_info[2];
//     out_N_good = eff_N_comb.size();
//     bco_full_out = bco_full;
//     MC_true_zvtx = TrigZvtxMC * 10.;
//     out_width_density = width_density_par;
//     tree_out -> Fill();

// } // note : if N good tracks in xy found > certain value
// cout<<"test tag 2"<<endl;
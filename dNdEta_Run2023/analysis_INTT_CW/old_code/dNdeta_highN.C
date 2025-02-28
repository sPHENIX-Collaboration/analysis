// #include "DAC_Scan_ladder.h"
//#include "InttConversion.h"
#include "InttClustering.h"
#include "sigmaEff.h"

// todo : the number of number is given by the adc_setting_run !!!
// todo : also the range of the hist.
// todo : the adc follows the following convention
// todo : 1. the increment has to be 4
// todo : remember to check the "adc_conv"
// vector<vector<int>> adc_setting_run = {	
//     // {8  , 12 , 16 , 20 , 24 , 28 , 32 , 36 },
//     // {28 , 32 , 36 , 40 , 44 , 48 , 52 , 56 },
//     {48 , 52 , 56 , 60 , 64 , 68 , 72 , 76 }, // note : 3
//     {68 , 72 , 76 , 80 , 84 , 88 , 92 , 96 }, // note : 4
//     {88 , 92 , 96 , 100, 104, 108, 112, 116}, // note : 5
//     {108, 112, 116, 120, 124, 128, 132, 136}, // note : 6
//     {128, 132, 136, 140, 144, 148, 152, 156}, // note : 7
//     // {148, 152, 156, 160, 164, 168, 172, 176},
//     // {168, 172, 176, 180, 184, 188, 192, 196},
//     // {188, 192, 196, 200, 204, 208, 212, 216}
// };

vector<vector<int>> adc_setting_run = {	
    {15, 30, 60, 90, 120, 150, 180, 210, 240}
    // {15, 30, 50, 70, 90, 110, 130, 150,170}
    // {8  , 12 , 16 , 20 , 24 , 28 , 32 , 36 },
    // {28 , 32 , 36 , 40 , 44 , 48 , 52 , 56 },
    // {48 , 52 , 56 , 60 , 64 , 68 , 72 , 76 }, // note : 3
    // {68 , 72 , 76 , 80 , 84 , 88 , 92 , 96 }, // note : 4
    // {88 , 92 , 96 , 100, 104, 108, 112, 116}, // note : 5
    // {108, 112, 116, 120, 124, 128, 132, 136}, // note : 6
    // {128, 132, 136, 140, 144, 148, 152, 156}, // note : 7
    // {148, 152, 156, 160, 164, 168, 172, 176},
    // {168, 172, 176, 180, 184, 188, 192, 196},
    // {188, 192, 196, 200, 204, 208, 212, 216}
};

TString color_code_2[8] = {"#CC768D","#19768D","#DDA573","#009193","#6E9193","#941100","#A08144","#517E66"};

struct full_hit_info {
    int FC;
    int chip_id;
    int chan_id;
    int adc;
};


struct ladder_info {
    int FC;
    TString Port;
    int ROC;
    int Direction; // note : 0 : south, 1 : north 
};

double get_radius(double x, double y)
{
    return sqrt(pow(x,2)+pow(y,2));
}

double get_radius_sign(double x, double y)
{
    double phi = ((y) < 0) ? atan2((y),(x)) * (180./TMath::Pi()) + 360 : atan2((y),(x)) * (180./TMath::Pi());
    
    return (phi > 180) ? sqrt(pow(x,2)+pow(y,2)) * -1 : sqrt(pow(x,2)+pow(y,2)); 
}

void Characterize_Pad (TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0)
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

std::vector<double> calculateDistanceAndClosestPoint(double x1, double y1, double x2, double y2, double target_x, double target_y) {
    
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

double get_z_vertex(clu_info inner_clu, clu_info outer_clu, double target_x, double target_y)
{
    // note : x = z, 
    // note : y = radius
    double inner_clu_r = sqrt(pow(inner_clu.x,2)+ pow(inner_clu.y,2));
    double outer_clu_r = sqrt(pow(outer_clu.x,2)+ pow(outer_clu.y,2));
    double target_r    = sqrt(pow(target_x,2)   + pow(target_y,2));

    // Use the slope equation (y = ax + b) to calculate the x-coordinate for the target y
    if ( fabs(outer_clu.z - inner_clu.z) < 0.00001 ){
        return outer_clu.z;
    }
    else {
        double slope = (outer_clu_r - inner_clu_r) / (outer_clu.z - inner_clu.z);
        double yIntercept = inner_clu_r - slope * inner_clu.z;
        double xCoordinate = (target_r - yIntercept) / slope;
        return xCoordinate;
    }
    
}

// Function to calculate the angle between two vectors in degrees using the cross product
double calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY) {
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

void temp_bkg(TPad * c1, string conversion_mode, double peek, pair<double,double> beam_origin)
{
    c1 -> cd();

    int N_ladder[4] = {12, 12, 16, 16};
    string ladder_index_string[16] = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15"};

    vector<double> x_vec; x_vec.clear();
    vector<double> y_vec; y_vec.clear();

    vector<double> x_vec_2; x_vec_2.clear();
    vector<double> y_vec_2; y_vec_2.clear();

    TGraph * bkg = new TGraph();
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

    TLine * ladder_line = new TLine();
    ladder_line -> SetLineWidth(1);

    for (int server_i = 0; server_i < 4; server_i++)
    {
        for (int FC_i = 0; FC_i < 14; FC_i++)
        {
            ladder_line -> DrawLine(
                InttConversion::Get_XY_all(Form("intt%i",server_i),FC_i,14,0,conversion_mode,peek).x, InttConversion::Get_XY_all(Form("intt%i",server_i),FC_i,14,0,conversion_mode,peek).y,
                InttConversion::Get_XY_all(Form("intt%i",server_i),FC_i,1,0,conversion_mode,peek).x, InttConversion::Get_XY_all(Form("intt%i",server_i),FC_i,1,0,conversion_mode,peek).y
            );
        }
    }
    
    ladder_line -> Draw("l same");

}

// note : use "ls *.root > file_list.txt" to create the list of the file in the folder, full directory in the file_list.txt
// note : set_folder_name = "folder_xxxx"
// note : server_name = "inttx"
void dNdeta(/*pair<double,double>beam_origin*/)
{
    TCanvas * c2 = new TCanvas("","",2500,800);    
    c2 -> cd();
    TPad *pad_xy = new TPad(Form("pad_xy"), "", 0.0, 0.0, 0.33, 1.0);
    Characterize_Pad(pad_xy, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    pad_xy -> Draw();

    TPad *pad_rz = new TPad(Form("pad_rz"), "", 0.33, 0.0, 0.66, 1.0);
    Characterize_Pad(pad_rz, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    pad_rz -> Draw();

    TPad *pad_z = new TPad(Form("pad_z"), "", 0.66, 0.0, 1.0, 1.0);
    Characterize_Pad(pad_z, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    pad_z -> Draw();

    TCanvas * c1 = new TCanvas("","",1000,800);
    c1 -> cd();
    
    string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/zero_magnet_Takashi_used";
    // string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_ideal_excludeR1500_100kEvent";
    string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_survey_1_XYAlpha_Peek_3.32mm_excludeR1500_100kEvent";

    // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/2023_08_01/24767";
    // string file_name = "beam_inttall-00024767-0000_event_base_ana_cluster_ideal_excludeR2000_100kEvent";

    // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/new_DAC_Scan_0722/AllServer/DAC2";
    // string file_name = "beam_inttall-00023058-0000_event_base_ana_cluster_ideal_excludeR2000_100kEvent";

    system(Form("mkdir %s/folder_%s_HighN",mother_folder_directory.c_str(),file_name.c_str()));
    pair<double,double> beam_origin = {-0,2};
    double temp_Y_align = 0.;
    double temp_X_align = -0.;

    double zvtx_hist_l = -500;
    double zvtx_hist_r = 500;

    int Nhit_cut = 520;           // note : if (> Nhit_cut)          -> continue
    int clu_size_cut = 4;         // note : if (> clu_size_cut)      -> continue
    double clu_sum_adc_cut = 31;  // note : if (< clu_sum_adc_cut)   -> continue
    int N_clu_cut = 201;          // note : if (> N_clu_cut)         -> continue  unit number
    double phi_diff_cut = 5.72;   // note : if (< phi_diff_cut)      -> pass      unit degree
    double DCA_cut = 4;           // note : if (< DCA_cut)           -> pass      unit mm
    int zvtx_cal_require = 15;    // note : if (> zvtx_cal_require)  -> pass
    int zvtx_draw_requireL = 15;       
    int zvtx_draw_requireR = 100;   // note : if ( zvtx_draw_requireL < event && event < zvtx_draw_requireR) -> pass
    double Integrate_portion = 0.6826;
    
    //todo : change the mode for drawing
    int geo_mode_id = 1;
    string conversion_mode = (geo_mode_id == 0) ? "ideal" : "survey_1_XYAlpha_Peek";
    double peek = 3.32405;

    TFile * file_in = new TFile(Form("%s/%s.root",mother_folder_directory.c_str(),file_name.c_str()),"read");
    TTree * tree = (TTree *)file_in->Get("tree_clu");
    
    long long N_event = tree -> GetEntries();
    cout<<Form("N_event in file %s : %lli",file_name.c_str(), N_event)<<endl;

    int N_hits;
    int N_cluster_inner;
    int N_cluster_outer;
    vector<int>* column_vec = new vector<int>();
    vector<double>* avg_chan_vec = new vector<double>();
    vector<int>* sum_adc_vec = new vector<int>();
    vector<int>* sum_adc_conv_vec = new vector<int>();
    vector<int>* size_vec = new vector<int>();
    vector<double>* x_vec = new vector<double>();
    vector<double>* y_vec = new vector<double>();
    vector<double>* z_vec = new vector<double>();
    vector<int>* layer_vec = new vector<int>();
    vector<double>* phi_vec = new vector<double>();

    tree -> SetBranchAddress("nhits",&N_hits);
    tree -> SetBranchAddress("nclu_inner",&N_cluster_inner);
    tree -> SetBranchAddress("nclu_outer",&N_cluster_outer);

    tree -> SetBranchAddress("column", &column_vec);
    tree -> SetBranchAddress("avg_chan", &avg_chan_vec);
    tree -> SetBranchAddress("sum_adc", &sum_adc_vec);
    tree -> SetBranchAddress("sum_adc_conv", &sum_adc_conv_vec);
    tree -> SetBranchAddress("size", &size_vec);
    tree -> SetBranchAddress("x", &x_vec);
    tree -> SetBranchAddress("y", &y_vec);
    tree -> SetBranchAddress("z", &z_vec);
    tree -> SetBranchAddress("layer", &layer_vec);
    tree -> SetBranchAddress("phi", &phi_vec);

    TLatex *draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.02);

    vector<clu_info> temp_sPH_inner_nocolumn_vec; temp_sPH_inner_nocolumn_vec.clear();
    vector<clu_info> temp_sPH_outer_nocolumn_vec; temp_sPH_outer_nocolumn_vec.clear();
    vector<vector<double>> temp_sPH_nocolumn_vec(2);
    vector<vector<double>> temp_sPH_nocolumn_rz_vec(2);

    TH2F * angle_correlation = new TH2F("","angle_correlation",361,0,361,361,0,361);
    angle_correlation -> SetStats(0);
    angle_correlation -> GetXaxis() -> SetTitle("Inner Phi (degree)");
    angle_correlation -> GetYaxis() -> SetTitle("Outer Phi (degree)");

    TH2F * inner_pos_xy = new TH2F("","inner_pos_xy",360,-100,100,360,-100,100);
    inner_pos_xy -> SetStats(0);
    inner_pos_xy -> GetXaxis() -> SetTitle("X axis");
    inner_pos_xy -> GetYaxis() -> SetTitle("Y axis");

    TH2F * outer_pos_xy = new TH2F("","outer_pos_xy",360,-150,150,360,-150,150);
    outer_pos_xy -> SetStats(0);
    outer_pos_xy -> GetXaxis() -> SetTitle("X axis");
    outer_pos_xy -> GetYaxis() -> SetTitle("Y axis");

    TH2F * inner_outer_pos_xy = new TH2F("","inner_outer_pos_xy",360,-150,150,360,-150,150);
    inner_outer_pos_xy -> SetStats(0);
    inner_outer_pos_xy -> GetXaxis() -> SetTitle("X axis");
    inner_outer_pos_xy -> GetYaxis() -> SetTitle("Y axis");

    TH1F * z_pos_diff = new TH1F("","z_pos_diff",360,-150,150);
    z_pos_diff -> GetXaxis() -> SetTitle("inner zpos - outer zpos");
    z_pos_diff -> GetYaxis() -> SetTitle("Eentry");

    TH2F * z_pos_diff_angle_diff = new TH2F("","z_pos_diff_angle_diff",100,-25,25,200,-11,11);
    z_pos_diff_angle_diff -> SetStats(0);
    z_pos_diff_angle_diff -> GetXaxis() -> SetTitle("inner zpos - outer zpos");
    z_pos_diff_angle_diff -> GetYaxis() -> SetTitle("Inner phi - outer phi");

    TH1F * Nhits_good = new TH1F("","Nhits_good",360,0,1000);
    Nhits_good -> GetXaxis() -> SetTitle("N hits in one event");
    Nhits_good -> GetYaxis() -> SetTitle("Eentry");

    TH1F * z_pos_inner = new TH1F("","z_pos_inner",200,-150,150);
    z_pos_inner -> GetXaxis() -> SetTitle("inner zpos");
    z_pos_inner -> GetYaxis() -> SetTitle("Eentry");

    TH1F * z_pos_outer = new TH1F("","z_pos_outer",200,-150,150);
    z_pos_outer -> GetXaxis() -> SetTitle("outer zpos");
    z_pos_outer -> GetYaxis() -> SetTitle("Eentry");

    TH2F * z_pos_inner_outer = new TH2F("","z_pos_inner_outer",100,-150,150, 100,-150,150);
    z_pos_inner_outer -> SetStats(0);
    z_pos_inner_outer -> GetXaxis() -> SetTitle("inner zpos");
    z_pos_inner_outer -> GetYaxis() -> SetTitle("outer pos");

    TH2F * DCA_point = new TH2F("","DCA_point",100,-10,10,100,-10,10);
    DCA_point -> SetStats(0);
    DCA_point -> GetXaxis() -> SetTitle("X pos (mm)");
    DCA_point -> GetYaxis() -> SetTitle("Y pos (mm)");

    TH2F * DCA_distance_inner_phi = new TH2F("","DCA_distance_inner_phi",100,0,360,100,-10,10);
    DCA_distance_inner_phi -> SetStats(0);
    DCA_distance_inner_phi -> GetXaxis() -> SetTitle("inner phi");
    DCA_distance_inner_phi -> GetYaxis() -> SetTitle("DCA (mm)");

    TH2F * DCA_distance_outer_phi = new TH2F("","DCA_distance_outer_phi",100,0,360,100,-10,10);
    DCA_distance_outer_phi -> SetStats(0);
    DCA_distance_outer_phi -> GetXaxis() -> SetTitle("outer phi");
    DCA_distance_outer_phi -> GetYaxis() -> SetTitle("DCA (mm)");

    TH1F * N_cluster_outer_pass = new TH1F("","N_cluster_outer_pass",100,0,100);
    N_cluster_outer_pass -> GetXaxis() -> SetTitle("N_cluster");
    N_cluster_outer_pass -> GetYaxis() -> SetTitle("Eentry");

    TH1F * N_cluster_inner_pass = new TH1F("","N_cluster_inner_pass",100,0,100);
    N_cluster_inner_pass -> GetXaxis() -> SetTitle("N_cluster");
    N_cluster_inner_pass -> GetYaxis() -> SetTitle("Eentry");

    TH2F * N_cluster_correlation = new TH2F("","N_cluster_correlation",100,0,500,100,0,500);
    N_cluster_correlation -> SetStats(0);
    N_cluster_correlation -> GetXaxis() -> SetTitle("inner N_cluster");
    N_cluster_correlation -> GetYaxis() -> SetTitle("Outer N_cluster");

    TH1F * temp_event_zvtx = new TH1F("","Z vertex dist",125,zvtx_hist_l,zvtx_hist_r);
    temp_event_zvtx -> GetXaxis() -> SetTitle("Z vertex position (mm)");
    temp_event_zvtx -> GetYaxis() -> SetTitle("Entry");
    vector<float> temp_event_zvtx_vec; temp_event_zvtx_vec.clear();
    vector<float> temp_event_zvtx_info; temp_event_zvtx_info.clear();
    TLine * effi_sig_range_line = new TLine();
    effi_sig_range_line -> SetLineWidth(3);
    effi_sig_range_line -> SetLineColor(TColor::GetColor("#A08144"));
    effi_sig_range_line -> SetLineStyle(2);
    TF1 * zvtx_fitting = new TF1("","gaus",-500,500);
    // zvtx_fitting -> SetLi

    
    vector<vector<double>> good_track_xy_vec; good_track_xy_vec.clear();
    vector<vector<double>> good_track_rz_vec; good_track_rz_vec.clear();
    TLine * track_line = new TLine();
    track_line -> SetLineWidth(1);
    track_line -> SetLineColor(38);

    TLine * coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(16);
    coord_line -> SetLineStyle(2);


    vector<float> avg_event_zvtx_vec; avg_event_zvtx_vec.clear();
    TH1F * avg_event_zvtx = new TH1F("","avg_event_zvtx",125,zvtx_hist_l,zvtx_hist_r);
    avg_event_zvtx -> GetXaxis() -> SetTitle("Z vertex position (mm)");
    avg_event_zvtx -> GetYaxis() -> SetTitle("entry");
    
    double mini_DCAXY;
    int mini_inner_i;
    int mini_outer_i;

    
    c2 -> Print(Form("%s/folder_%s_HighN/temp_event_display.pdf(",mother_folder_directory.c_str(),file_name.c_str()));

    for (int event_i = 0; event_i < N_event; event_i++)
    {
        tree -> GetEntry(event_i);
        unsigned int length = column_vec -> size();

        if (N_hits > Nhit_cut) continue;
        if (N_cluster_inner == 0 || N_cluster_outer == 0) continue;
        if (N_cluster_inner == -1 || N_cluster_outer == -1) continue;
        if ((N_cluster_inner + N_cluster_outer) < zvtx_cal_require) continue;
        if (N_cluster_inner < 5) continue;
        if (N_cluster_outer < 5) continue;
        

        // note : apply some selection to remove the hot channels
        // note : and make the inner_clu_vec and outer_clu_vec
        for (int clu_i = 0; clu_i < length; clu_i++)
        {
            if (size_vec -> at(clu_i) > clu_size_cut) continue;
            // if (size_vec -> at(clu_i) < 2) continue;
            if (sum_adc_conv_vec -> at(clu_i) < clu_sum_adc_cut) continue;
            // if (z_vec -> at(clu_i) < 0) continue;
            
            // note : inner
            // if (layer_vec -> at(clu_i) == 0 && x_vec -> at(clu_i) < -75 && x_vec -> at(clu_i) > -80 && y_vec -> at(clu_i) > 7.5 && y_vec -> at(clu_i) < 12.5 ) continue;
            // // if (layer_vec -> at(clu_i) == 0 && x_vec -> at(clu_i) > 35 && x_vec -> at(clu_i) < 40 && y_vec -> at(clu_i) > 65 && y_vec -> at(clu_i) < 70 ) continue;
            // if (layer_vec -> at(clu_i) == 0 && phi_vec -> at(clu_i) > 295 && phi_vec -> at(clu_i) < 302) continue;
            // if (layer_vec -> at(clu_i) == 0 && phi_vec -> at(clu_i) > 210 && phi_vec -> at(clu_i) < 213) continue;
            // if (layer_vec -> at(clu_i) == 0 && phi_vec -> at(clu_i) > 55 && phi_vec -> at(clu_i) < 65) continue;
            // if (layer_vec -> at(clu_i) == 0 && phi_vec -> at(clu_i) > 348 && phi_vec -> at(clu_i) < 353) continue;
            // if (layer_vec -> at(clu_i) == 0 && phi_vec -> at(clu_i) > 265 && phi_vec -> at(clu_i) < 270) continue; // todo : for the 2023_08_01/24767

            // note : outer
            // if (layer_vec -> at(clu_i) == 1 && x_vec -> at(clu_i) < -70 && x_vec -> at(clu_i) > -75 && y_vec -> at(clu_i) > 70 && y_vec -> at(clu_i) < 80 ) continue;
            // // if (layer_vec -> at(clu_i) == 1 && x_vec -> at(clu_i) > 70 && x_vec -> at(clu_i) < 83 && y_vec -> at(clu_i) > 50 && y_vec -> at(clu_i) < 65 ) continue;
            // // if (layer_vec -> at(clu_i) == 1 && x_vec -> at(clu_i) > 70 && x_vec -> at(clu_i) < 83 && y_vec -> at(clu_i) > 63 && y_vec -> at(clu_i) < 75 ) continue;
            // if (layer_vec -> at(clu_i) == 1 && x_vec -> at(clu_i) < -70 && x_vec -> at(clu_i) > -75 && y_vec -> at(clu_i) < -70 && y_vec -> at(clu_i) > -75 ) continue;
            // if (layer_vec -> at(clu_i) == 1 && phi_vec -> at(clu_i) > 335 && phi_vec -> at(clu_i) < 340) continue;
            // if (layer_vec -> at(clu_i) == 1 && phi_vec -> at(clu_i) > 105 && phi_vec -> at(clu_i) < 115) continue;
            // if (layer_vec -> at(clu_i) == 1 && phi_vec -> at(clu_i) > 25 && phi_vec -> at(clu_i) < 47) continue; // todo : for the "new_DAC_Scan_0722/AllServer/DAC2"


            temp_sPH_nocolumn_vec[0].push_back( (phi_vec -> at(clu_i) > 90 && phi_vec -> at(clu_i) < 270 ) ? x_vec -> at(clu_i) + temp_X_align : x_vec -> at(clu_i) );
            temp_sPH_nocolumn_vec[1].push_back( (phi_vec -> at(clu_i) > 90 && phi_vec -> at(clu_i) < 270 ) ? y_vec -> at(clu_i) + temp_Y_align : y_vec -> at(clu_i) );
            
            double clu_radius = get_radius(
                (phi_vec -> at(clu_i) > 90 && phi_vec -> at(clu_i) < 270 ) ? x_vec -> at(clu_i) + temp_X_align : x_vec -> at(clu_i), 
                (phi_vec -> at(clu_i) > 90 && phi_vec -> at(clu_i) < 270 ) ? y_vec -> at(clu_i) + temp_Y_align : y_vec -> at(clu_i)
            );
            temp_sPH_nocolumn_rz_vec[0].push_back(z_vec -> at(clu_i));
            temp_sPH_nocolumn_rz_vec[1].push_back( ( phi_vec -> at(clu_i) > 180 ) ? clu_radius * -1 : clu_radius );
            

            if (layer_vec -> at(clu_i) == 0) //note : inner
                temp_sPH_inner_nocolumn_vec.push_back({
                    column_vec -> at(clu_i), 
                    avg_chan_vec -> at(clu_i), 
                    sum_adc_vec -> at(clu_i), 
                    sum_adc_conv_vec -> at(clu_i), 
                    size_vec -> at(clu_i), 
                    (phi_vec -> at(clu_i) > 90 && phi_vec -> at(clu_i) < 270 ) ? x_vec -> at(clu_i) + temp_X_align : x_vec -> at(clu_i), 
                    (phi_vec -> at(clu_i) > 90 && phi_vec -> at(clu_i) < 270 ) ? y_vec -> at(clu_i) + temp_Y_align : y_vec -> at(clu_i), 
                    z_vec -> at(clu_i), 
                    layer_vec -> at(clu_i), 
                    phi_vec -> at(clu_i)
                });
            
            if (layer_vec -> at(clu_i) == 1) //note : inner
                temp_sPH_outer_nocolumn_vec.push_back({
                    column_vec -> at(clu_i), 
                    avg_chan_vec -> at(clu_i), 
                    sum_adc_vec -> at(clu_i), 
                    sum_adc_conv_vec -> at(clu_i), 
                    size_vec -> at(clu_i), 
                    (phi_vec -> at(clu_i) > 90 && phi_vec -> at(clu_i) < 270 ) ? x_vec -> at(clu_i) + temp_X_align : x_vec -> at(clu_i), 
                    (phi_vec -> at(clu_i) > 90 && phi_vec -> at(clu_i) < 270 ) ? y_vec -> at(clu_i) + temp_Y_align : y_vec -> at(clu_i), 
                    z_vec -> at(clu_i), 
                    layer_vec -> at(clu_i), 
                    phi_vec -> at(clu_i)
                });            
        }

        int original_outer_vec_size = temp_sPH_outer_nocolumn_vec.size(); 
        N_cluster_outer_pass -> Fill(temp_sPH_outer_nocolumn_vec.size());
        N_cluster_inner_pass -> Fill(temp_sPH_inner_nocolumn_vec.size());
        N_cluster_correlation -> Fill( temp_sPH_inner_nocolumn_vec.size(), temp_sPH_outer_nocolumn_vec.size() );

        if ((temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size()) > N_clu_cut)
        {
            temp_event_zvtx_info = {-1000,-1000,-1000};
            temp_event_zvtx_vec.clear();
            temp_event_zvtx -> Reset("ICESM");
            good_track_xy_vec.clear();
            good_track_rz_vec.clear();
            temp_sPH_nocolumn_rz_vec.clear(); temp_sPH_nocolumn_rz_vec = vector<vector<double>>(2);
            temp_sPH_nocolumn_vec.clear(); temp_sPH_nocolumn_vec = vector<vector<double>>(2);
            temp_sPH_inner_nocolumn_vec.clear();
            temp_sPH_outer_nocolumn_vec.clear();
            continue;
        }

        for ( int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++ )
        {
            int counting = 0;
            bool good_tag = false;

            for ( int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++ )
            {
                if (fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi) < phi_diff_cut)
                {
                    vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
                        temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
                        temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
                        beam_origin.first, beam_origin.second
                    );
                    
                    double zvtx = get_z_vertex(temp_sPH_inner_nocolumn_vec[inner_i],temp_sPH_outer_nocolumn_vec[outer_i],DCA_info_vec[1],DCA_info_vec[2]);
                    
                    //todo : fabs(zvtx) > 240 --> assume that the collisions happened in the INTT
                    if (fabs(zvtx) > 240) continue; 

                    good_tag = true; // note : Phi_inner - Phi_outer small, zvtx in INTT
                    
                    if (counting == 0) {
                        mini_DCAXY = DCA_info_vec[0];
                        mini_outer_i = outer_i;
                    }
                    else 
                    {
                        if (DCA_info_vec[0] < mini_DCAXY)
                        {
                            mini_DCAXY = DCA_info_vec[0];
                            mini_outer_i = outer_i;
                        }
                    }
                    counting = 1;
                }
            }// note : end of outer_i loop

            if (good_tag == true && mini_DCAXY < DCA_cut)
            {
                vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
                    temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
                    temp_sPH_outer_nocolumn_vec[mini_outer_i].x, temp_sPH_outer_nocolumn_vec[mini_outer_i].y,
                    beam_origin.first, beam_origin.second
                );

                double DCA_sign = calculateAngleBetweenVectors(
                    temp_sPH_outer_nocolumn_vec[mini_outer_i].x, temp_sPH_outer_nocolumn_vec[mini_outer_i].y,
                    temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
                    beam_origin.first, beam_origin.second
                );

                double zvtx = get_z_vertex(temp_sPH_inner_nocolumn_vec[inner_i],temp_sPH_outer_nocolumn_vec[mini_outer_i],DCA_info_vec[1],DCA_info_vec[2]);
                temp_event_zvtx -> Fill( zvtx );
                if(zvtx_hist_l <= zvtx && zvtx <= zvtx_hist_r){
                    temp_event_zvtx_vec.push_back( zvtx );
                }

                if ( event_i == 23515 )
                {
                    if ( temp_sPH_outer_nocolumn_vec[mini_outer_i].phi > 60.6 && temp_sPH_outer_nocolumn_vec[mini_outer_i].phi < 98.9 )
                    {
                        good_track_xy_vec.push_back({temp_sPH_outer_nocolumn_vec[mini_outer_i].x,temp_sPH_outer_nocolumn_vec[mini_outer_i].y,DCA_info_vec[1],DCA_info_vec[2]});
                        good_track_rz_vec.push_back({
                                temp_sPH_outer_nocolumn_vec[mini_outer_i].z, (temp_sPH_outer_nocolumn_vec[mini_outer_i].phi > 180) ? get_radius(temp_sPH_outer_nocolumn_vec[mini_outer_i].x,temp_sPH_outer_nocolumn_vec[mini_outer_i].y) * -1 : get_radius(temp_sPH_outer_nocolumn_vec[mini_outer_i].x,temp_sPH_outer_nocolumn_vec[mini_outer_i].y),
                                zvtx, (temp_sPH_outer_nocolumn_vec[mini_outer_i].phi > 180) ? get_radius(DCA_info_vec[1],DCA_info_vec[2]) * -1 : get_radius(DCA_info_vec[1],DCA_info_vec[2])
                            }
                        );
                    }
                }
                else 
                {
                    good_track_xy_vec.push_back({temp_sPH_outer_nocolumn_vec[mini_outer_i].x,temp_sPH_outer_nocolumn_vec[mini_outer_i].y,DCA_info_vec[1],DCA_info_vec[2]});
                    good_track_rz_vec.push_back({
                            temp_sPH_outer_nocolumn_vec[mini_outer_i].z, (temp_sPH_outer_nocolumn_vec[mini_outer_i].phi > 180) ? get_radius(temp_sPH_outer_nocolumn_vec[mini_outer_i].x,temp_sPH_outer_nocolumn_vec[mini_outer_i].y) * -1 : get_radius(temp_sPH_outer_nocolumn_vec[mini_outer_i].x,temp_sPH_outer_nocolumn_vec[mini_outer_i].y),
                            zvtx, (temp_sPH_outer_nocolumn_vec[mini_outer_i].phi > 180) ? get_radius(DCA_info_vec[1],DCA_info_vec[2]) * -1 : get_radius(DCA_info_vec[1],DCA_info_vec[2])
                        }
                    );
                }

                

                DCA_point -> Fill( DCA_info_vec[1], DCA_info_vec[2] );
                angle_correlation -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi,temp_sPH_outer_nocolumn_vec[mini_outer_i].phi);
                z_pos_diff -> Fill( temp_sPH_inner_nocolumn_vec[inner_i].z - temp_sPH_outer_nocolumn_vec[mini_outer_i].z );
                z_pos_diff_angle_diff -> Fill( temp_sPH_inner_nocolumn_vec[inner_i].z - temp_sPH_outer_nocolumn_vec[mini_outer_i].z, temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[mini_outer_i].phi );
                Nhits_good -> Fill(N_hits);
                z_pos_inner -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].z);
                z_pos_outer -> Fill(temp_sPH_outer_nocolumn_vec[mini_outer_i].z);
                z_pos_inner_outer -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].z, temp_sPH_outer_nocolumn_vec[mini_outer_i].z);
                DCA_distance_inner_phi -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi, DCA_sign );
                DCA_distance_outer_phi -> Fill(temp_sPH_outer_nocolumn_vec[mini_outer_i].phi, DCA_sign );    

                // note : remove that good outer_clu 
                temp_sPH_outer_nocolumn_vec.erase(temp_sPH_outer_nocolumn_vec.begin() + mini_outer_i);

            } // note : if < DCA_cut
        } // note : end of inner loop


        if (temp_event_zvtx_vec.size() > zvtx_cal_require)
        {
            // note : effi_sig method 
            temp_event_zvtx_info = sigmaEff_avg(temp_event_zvtx_vec,Integrate_portion);
            avg_event_zvtx -> Fill(temp_event_zvtx_info[0]);
            avg_event_zvtx_vec.push_back(temp_event_zvtx_info[0]);
            
            // note : gaus fitting method
            // temp_event_zvtx -> Fit(zvtx_fitting,"NQ");
            // avg_event_zvtx -> Fill( zvtx_fitting -> GetParameter(1) );

            // note : TH1 mean
            // avg_event_zvtx -> Fill( temp_event_zvtx -> GetMean() );
        }
            
        if ( zvtx_draw_requireL < temp_event_zvtx_vec.size() && temp_event_zvtx_vec.size() < zvtx_draw_requireR)
        {   
            TGraph * temp_event_xy = new TGraph(temp_sPH_nocolumn_vec[0].size(),&temp_sPH_nocolumn_vec[0][0],&temp_sPH_nocolumn_vec[1][0]);
            temp_event_xy -> SetTitle("INTT event display X-Y plane");
            temp_event_xy -> GetXaxis() -> SetLimits(-150,150);
            temp_event_xy -> GetYaxis() -> SetRangeUser(-150,150);
            temp_event_xy -> GetXaxis() -> SetTitle("X [mm]");
            temp_event_xy -> GetYaxis() -> SetTitle("Y [mm]");
            temp_event_xy -> SetMarkerStyle(20);
            temp_event_xy -> SetMarkerColor(2);
            temp_event_xy -> SetMarkerSize(1);
            TGraph * temp_event_rz = new TGraph(temp_sPH_nocolumn_rz_vec[0].size(),&temp_sPH_nocolumn_rz_vec[0][0],&temp_sPH_nocolumn_rz_vec[1][0]);
            temp_event_rz -> SetTitle("INTT event display r-Z plane");
            temp_event_rz -> GetXaxis() -> SetLimits(-500,500);
            temp_event_rz -> GetYaxis() -> SetRangeUser(-150,150);
            temp_event_rz -> GetXaxis() -> SetTitle("Z [mm]");
            temp_event_rz -> GetYaxis() -> SetTitle("Radius [mm]");
            temp_event_rz -> SetMarkerStyle(20);
            temp_event_rz -> SetMarkerColor(2);
            temp_event_rz -> SetMarkerSize(1);

            pad_xy -> cd();
            temp_bkg(pad_xy, conversion_mode, peek, beam_origin);
            temp_event_xy -> Draw("p same");
            for (int track_i = 0; track_i < good_track_xy_vec.size(); track_i++){
                track_line -> DrawLine(good_track_xy_vec[track_i][0],good_track_xy_vec[track_i][1],good_track_xy_vec[track_i][2],good_track_xy_vec[track_i][3]);
            }
            track_line -> Draw("l same");
            draw_text -> DrawLatex(0.2, 0.85, Form("eID : %i, Total event hit : %i, innter Ncluster : %i, outer Ncluster : %i",event_i,N_hits,temp_sPH_inner_nocolumn_vec.size(),original_outer_vec_size));
        
            pad_rz -> cd();
            temp_event_rz -> Draw("ap");    
            effi_sig_range_line -> DrawLine(temp_event_zvtx_info[0],-150,temp_event_zvtx_info[0],150);
            coord_line -> DrawLine(0,-150,0,150);
            coord_line -> DrawLine(-500,0,500,0);
            for (int track_i = 0; track_i < good_track_rz_vec.size(); track_i++){
                track_line -> DrawLine(good_track_rz_vec[track_i][0],good_track_rz_vec[track_i][1],good_track_rz_vec[track_i][2],good_track_rz_vec[track_i][3]);
            }
            draw_text -> DrawLatex(0.2, 0.85, Form("Negative radius : Clu_{outer} > 180^{0}"));
            draw_text -> DrawLatex(0.2, 0.81, Form("EffiSig avg : %.2f mm",temp_event_zvtx_info[0]));

            pad_z -> cd();
            temp_event_zvtx -> Draw("hist");
            // zvtx_fitting -> Draw("lsame");
            
            effi_sig_range_line -> DrawLine(temp_event_zvtx_info[1],0,temp_event_zvtx_info[1],temp_event_zvtx -> GetMaximum()*1.05);
            effi_sig_range_line -> DrawLine(temp_event_zvtx_info[2],0,temp_event_zvtx_info[2],temp_event_zvtx -> GetMaximum()*1.05);
            
            draw_text -> DrawLatex(0.2, 0.85, Form("eID : %i, Total event hit : %i, innter Ncluster : %i, outer Ncluster : %i",event_i,N_hits,temp_sPH_inner_nocolumn_vec.size(),original_outer_vec_size));
            // draw_text -> DrawLatex(0.2, 0.84, Form("Gaus fit mean : %.3f mm",zvtx_fitting -> GetParameter(1)));
            draw_text -> DrawLatex(0.2, 0.82, Form("EffiSig min : %.2f mm, max : %.2f mm",temp_event_zvtx_info[1],temp_event_zvtx_info[2]));
            draw_text -> DrawLatex(0.2, 0.79, Form("EffiSig avg : %.2f mm",temp_event_zvtx_info[0]));

            c2 -> Print(Form("%s/folder_%s_HighN/temp_event_display.pdf",mother_folder_directory.c_str(),file_name.c_str()));
            pad_xy -> Clear();
            pad_rz -> Clear();
            pad_z  -> Clear();

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

        temp_event_zvtx_info = {-1000,-1000,-1000};
        temp_event_zvtx_vec.clear();
        temp_event_zvtx -> Reset("ICESM");
        good_track_xy_vec.clear();
        good_track_rz_vec.clear();
        temp_sPH_nocolumn_rz_vec.clear(); temp_sPH_nocolumn_rz_vec = vector<vector<double>>(2);
        temp_sPH_nocolumn_vec.clear(); temp_sPH_nocolumn_vec = vector<vector<double>>(2);
        temp_sPH_inner_nocolumn_vec.clear();
        temp_sPH_outer_nocolumn_vec.clear();
    } // note : end of event 

    c2 -> Print(Form("%s/folder_%s_HighN/temp_event_display.pdf)",mother_folder_directory.c_str(),file_name.c_str()));
    c2 -> Clear();
    c1 -> Clear();
    

    c1 -> cd();
    vector<float> avg_event_zvtx_info = sigmaEff_avg(avg_event_zvtx_vec,Integrate_portion);
    avg_event_zvtx -> Draw("hist");
    effi_sig_range_line -> DrawLine(avg_event_zvtx_info[1],0,avg_event_zvtx_info[1],avg_event_zvtx -> GetMaximum()*1.05);
    effi_sig_range_line -> DrawLine(avg_event_zvtx_info[2],0,avg_event_zvtx_info[2],avg_event_zvtx -> GetMaximum()*1.05);    
    draw_text -> DrawLatex(0.15, 0.84, Form("EffiSig min : %.2f mm, max : %.2f mm",avg_event_zvtx_info[1],avg_event_zvtx_info[2]));
    draw_text -> DrawLatex(0.15, 0.81, Form("EffiSig avg : %.2f mm",avg_event_zvtx_info[0]));
    c1 -> Print(Form("%s/folder_%s_HighN/avg_event_zvtx.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    N_cluster_inner_pass -> Draw("hist"); 
    c1 -> Print(Form("%s/folder_%s_HighN/N_cluster_inner_pass.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    N_cluster_outer_pass -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_HighN/N_cluster_outer_pass.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    N_cluster_correlation -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/N_cluster_correlation.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    DCA_point -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/DCA_point_X%.1fY%.1f_.pdf",mother_folder_directory.c_str(),file_name.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();

    DCA_distance_inner_phi -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/DCA_distance_inner_phi_X%.1fY%.1f_.pdf",mother_folder_directory.c_str(),file_name.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();

    DCA_distance_outer_phi -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/DCA_distance_outer_phi_X%.1fY%.1f_.pdf",mother_folder_directory.c_str(),file_name.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();

    z_pos_inner_outer -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/z_pos_inner_outer.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    z_pos_inner -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_HighN/z_pos_inner.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    z_pos_outer -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_HighN/z_pos_outer.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    Nhits_good -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_HighN/Nhits_good.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    angle_correlation -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/angle_correlation.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    z_pos_diff -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_HighN/z_pos_diff.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    z_pos_diff_angle_diff -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/z_pos_diff_angle_diff.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    inner_pos_xy -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/inner_pos_xy.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    outer_pos_xy -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/outer_pos_xy.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    inner_outer_pos_xy -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_HighN/inner_outer_pos_xy.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    
}
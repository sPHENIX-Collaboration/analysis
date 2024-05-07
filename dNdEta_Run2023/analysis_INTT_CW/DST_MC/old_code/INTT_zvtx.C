// #include "DAC_Scan_ladder.h"
//#include "InttConversion.h"
#include "/sphenix/user/ChengWei/INTT/INTT_commissioning/INTT_CW/INTT_commissioning/DAC_Scan/InttConversion_new.h"
#include "/sphenix/user/ChengWei/INTT/INTT_commissioning/INTT_CW/INTT_commissioning/DAC_Scan/InttClustering.h"
#include "../sigmaEff.h"
#include "../sPhenixStyle.C"
#include "../INTTDSTchain.C"

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

void temp_bkg(TPad * c1, string conversion_mode, double peek, pair<double,double> beam_origin, InttConversion * ch_pos_DB)
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
                ch_pos_DB -> Get_XY_all(Form("intt%i",server_i),FC_i,14,0).x, ch_pos_DB -> Get_XY_all(Form("intt%i",server_i),FC_i,14,0).y,
                ch_pos_DB -> Get_XY_all(Form("intt%i",server_i),FC_i,1,0).x, ch_pos_DB -> Get_XY_all(Form("intt%i",server_i),FC_i,1,0).y
            );
        }
    }
    
    ladder_line -> Draw("l same");

}

double grEY_stddev (TGraphErrors * input_grr)
{
    vector<double> input_vector; input_vector.clear();
    for (int i = 0; i < input_grr -> GetN(); i++)
    {  
        input_vector.push_back( input_grr -> GetPointY(i) );
    }

	double sum=0;
	double average;
	double sum_subt = 0;
	for (int i=0; i<input_vector.size(); i++)
		{
			sum+=input_vector[i];

		}
	average=sum/input_vector.size();
	//cout<<"average is : "<<average<<endl;

	for (int i=0; i<input_vector.size(); i++)
		{
			//cout<<input_vector[i]-average<<endl;
			sum_subt+=pow((input_vector[i]-average),2);

		}
	//cout<<"sum_subt : "<<sum_subt<<endl;
	double stddev;
	stddev=sqrt(sum_subt/(input_vector.size()-1));	
	return stddev;
}	

pair<double, double> mirrorPolynomial(double a, double b) {
    // Interchange 'x' and 'y'
    double mirroredA = 1.0 / a;
    double mirroredB = -b / a;

    return {mirroredA, mirroredB};
}

// note : pair<reduced chi2, eta of the track>
// note : vector : {r,z}
// note : p0 : vertex point {r,z,zerror}
// note : p1 : inner layer
// note : p2 : outer layer
pair<double,double> Get_eta(vector<double>p0, vector<double>p1, vector<double>p2)
{
    vector<double> r_vec  = {p0[0], p1[0], p2[0]}; 
    vector<double> z_vec  = {p0[1], p1[1], p2[1]}; 
    vector<double> re_vec = {0, 0, 0}; 
    vector<double> ze_vec = {p0[2], ( fabs( p1[1] ) < 130 ) ? 8. : 10., ( fabs( p2[1] ) < 130 ) ? 8. : 10.}; 

    // note : swap the r and z, to have easier fitting 
    // note : in principle, Z should be in the x axis, R should be in the Y axis
    TGraphErrors * track_gr = new TGraphErrors(3,&r_vec[0],&z_vec[0],&re_vec[0],&ze_vec[0]);    
    
    double vertical_line = ( fabs( grEY_stddev(track_gr) ) < 0.00001 ) ? 1 : 0;
    
    if (vertical_line) {return {0,0};}
    else 
    {
        TF1 * fit_rz = new TF1("fit_rz","pol1",-500,500);
        fit_rz -> SetParameters(0,0);

        track_gr -> Fit(fit_rz,"NQ");

        pair<double,double> ax_b = mirrorPolynomial(fit_rz -> GetParameter(1),fit_rz -> GetParameter(0));

        return  {(fit_rz -> GetChisquare() / double(fit_rz -> GetNDF())), -1 * TMath::Log( fabs( tan( atan2(ax_b.first, (ax_b.first > 0) ? 1. : -1. ) / 2 ) ) )};

    }

}

// note : vector : {r,z}
// note : p0 : vertex point {r,z,zerror}
// note : p1 : another point from detector
// note : since only two points -> no strip width considered
double Get_eta_2P(vector<double>p0, vector<double>p1){    
    return  -1 * TMath::Log( fabs( tan( atan2(p1[0] - p0[0], p1[1] - p0[1]) / 2 ) ) );
}

double Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y) // note : x : z, y : r
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

pair<double,double> Get_possible_zvtx(double rvtx, vector<double> p0, vector<double> p1) // note : inner p0, outer p1, vector {r,z}, -> {y,x}
{
    vector<double> p0_z_edge = { ( fabs( p0[1] ) < 130 ) ? p0[1] - 8. : p0[1] - 10., ( fabs( p0[1] ) < 130 ) ? p0[1] + 8. : p0[1] + 10.}; // note : vector {left edge, right edge}
    vector<double> p1_z_edge = { ( fabs( p1[1] ) < 130 ) ? p1[1] - 8. : p1[1] - 10., ( fabs( p1[1] ) < 130 ) ? p1[1] + 8. : p1[1] + 10.}; // note : vector {left edge, right edge}

    double edge_first  = Get_extrapolation(rvtx,p0_z_edge[0],p0[0],p1_z_edge[1],p1[0]);
    double edge_second = Get_extrapolation(rvtx,p0_z_edge[1],p0[0],p1_z_edge[0],p1[0]);

    double mid_point = (edge_first + edge_second) / 2.;
    double possible_width = fabs(edge_first - edge_second) / 2.;

    return {mid_point, possible_width}; // note : first : mid point, second : width

}

double gaus_func(double *x, double *par)
{
    // note : par[0] : size
    // note : par[1] : mean
    // note : par[2] : width
    // note : par[3] : offset 
    return par[0] * TMath::Gaus(x[0],par[1],par[2]) + par[3];
}

// note : use "ls *.root > file_list.txt" to create the list of the file in the folder, full directory in the file_list.txt
// note : set_folder_name = "folder_xxxx"
// note : server_name = "inttx"
void INTT_zvtx(/*bool full_event, int run_Nevent*/)
{
    
    SetsPhenixStyle();

    TCanvas * c4 = new TCanvas("","",850,800);    
    c4 -> cd();
    
    TCanvas * c2 = new TCanvas("","",3400,800);    
    c2 -> cd();
    TPad *pad_xy = new TPad(Form("pad_xy"), "", 0.0, 0.0, 0.25, 1.0);
    Characterize_Pad(pad_xy, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    pad_xy -> Draw();

    TPad *pad_rz = new TPad(Form("pad_rz"), "", 0.25, 0.0, 0.50, 1.0);
    Characterize_Pad(pad_rz, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    pad_rz -> Draw();

    TPad *pad_z = new TPad(Form("pad_z"), "", 0.50, 0.0, 0.75, 1.0);
    Characterize_Pad(pad_z, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    pad_z -> Draw();
    
    TPad *pad_z_hist = new TPad(Form("pad_z_hist"), "", 0.75, 0.0, 1, 1.0);
    Characterize_Pad(pad_z_hist, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    pad_z_hist -> Draw();

    TCanvas * c1 = new TCanvas("","",950,800);
    c1 -> cd();

    //todo : can be more than it, possibly
    vector<string> conversion_mode_BD = {"ideal", "survey_1_XYAlpha_Peek_3.32mm", "full_survey_3.32"};
    // vector<string> conversion_mode_BD = {"ideal", "survey_1_XYAlpha_Peek", "full_survey_3.32"};
    
    // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/zero_magnet_Takashi_used";
    // string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_ideal_excludeR1500_100kEvent";
    // string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_survey_1_XYAlpha_Peek_3.32mm_excludeR1500_100kEvent";

    // string run_ID = "20869"; string file_event = "150";
    // todo : change the file name.
    // string mother_folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/" + run_ID;
    // string file_name = "beam_inttall-000" +run_ID+ "-0000_event_base_ana_cluster_survey_1_XYAlpha_Peek_3.32mm_excludeR20000_"+ file_event +"kEvent_3HotCut";
    // string file_name = "beam_inttall-000" +run_ID+ "-0000_event_base_ana_cluster_"+ conversion_mode_BD[geo_mode_id] +"_excludeR40000_"+ file_event +"kEvent_3HotCut";

    int geo_mode_id = 0;
    string mother_folder_directory = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC"; string file_name = "MC_ZF_1_30400";
    string MC_list_name = "dst_INTTdNdEta.list";

    TChain * chain_in = new TChain("EventTree");
    INTTDSTchain inttDSTMC(chain_in,mother_folder_directory,MC_list_name);
    std::printf("inttDSTMC N event : %lli \n", chain_in->GetEntries());
    long long N_event = chain_in->GetEntries();

    cout<<"the input file name : "<<file_name<<endl;
    sleep(5);

    // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/new_DAC_Scan_0722/AllServer/DAC2";
    // string file_name = "beam_inttall-00023058-0000_event_base_ana_cluster_ideal_excludeR2000_100kEvent";

    string out_folder_directory = Form("%s/folder_%s_advanced_test2",mother_folder_directory.c_str(),file_name.c_str());

    system(Form("mkdir %s",out_folder_directory.c_str()));
    // system(Form("mkdir %s/good_track",out_folder_directory.c_str()));

    pair<double,double> beam_origin = {-0.015, 0.012}; // note : for run20869
    pair<double,double> beam_origin_U = {0.0004, 0.0035}; // note : for run20869, the uncertainty of the VTXxy, calculated by the fit errors with the error propagation
    double temp_Y_align = 0.;
    double temp_X_align = -0.;

    double zvtx_hist_l = -500;
    double zvtx_hist_r = 500;

    // todo : the current MC doesn't have this information
    // int Nhit_cut = 20000;           // note : if (> Nhit_cut)          -> continue
    // int Nhit_cutl = 400;          // note : if (< Nhit_cutl)         -> continue 
    int clu_size_cut = 4;           // note : if (> clu_size_cut)      -> continue
    double clu_sum_adc_cut = -1;    // note : if (< clu_sum_adc_cut)   -> continue
    int N_clu_cut = 10000;          // note : if (> N_clu_cut)         -> continue  unit number
    int N_clu_cutl = 20;           // note : if (< N_clu_cutl)        -> continue  unit number
    double phi_diff_cut = 0.11;      // note : if (< phi_diff_cut)      -> pass      unit degree
    double DCA_cut = 0.5;             // note : if (< DCA_cut)           -> pass      unit mm
    int zvtx_cal_require = 15;      // note : if (> zvtx_cal_require)  -> pass
    int zvtx_draw_requireL = 15;       
    int zvtx_draw_requireR = 40000;   // note : if ( zvtx_draw_requireL < event && event < zvtx_draw_requireR) -> pass
    double Integrate_portion = 0.68; // todo : it was 0.6826, try to require less event, as most of the tracklets are not that useful
    double Integrate_portion_final = 0.68;
    bool draw_event_display = true;
    int print_rate = 40;

    // double mean_zvtx = -195.45; // note : unit : mm

    // bool full_event = false;
    // long long used_event = run_Nevent;

    // double dNdeta_upper_range = 20;

    // todo : change the geo draw mode if needed
    int geo_mode_id_draw = 0;
    string conversion_mode = (geo_mode_id_draw == 0) ? "ideal" : "survey_1_XYAlpha_Peek";
    double peek = 3.32405;

    // note : the initiator of the INTT geometry class
    InttConversion * ch_pos_DB = new InttConversion(conversion_mode_BD[geo_mode_id], peek);


    TFile * out_file = new TFile(Form("%s/INTT_zvtx.root",out_folder_directory.c_str()),"RECREATE");

    int out_eID, N_cluster_outer_out, N_cluster_inner_out, out_N_good;
    double out_zvtx, out_zvtxE, out_rangeL, out_rangeR, out_width_density, MC_true_zvtx;
    Long64_t bco_full_out; 

    TTree * tree_out =  new TTree ("tree_Z", "INTT Z info.");
    tree_out -> Branch("eID",&out_eID);
    tree_out -> Branch("bco_full",&bco_full_out);
    tree_out -> Branch("nclu_inner",&N_cluster_inner_out);
    tree_out -> Branch("nclu_outer",&N_cluster_outer_out);
    tree_out -> Branch("zvtx",&out_zvtx);
    tree_out -> Branch("zvtxE",&out_zvtxE);
    tree_out -> Branch("rangeL",&out_rangeL);
    tree_out -> Branch("rangeR",&out_rangeR);
    tree_out -> Branch("N_good",&out_N_good);
    tree_out -> Branch("Width_density",&out_width_density);
    tree_out -> Branch("MC_true_zvtx",&MC_true_zvtx);

    TLatex *draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    vector<clu_info> temp_sPH_inner_nocolumn_vec; temp_sPH_inner_nocolumn_vec.clear();
    vector<clu_info> temp_sPH_outer_nocolumn_vec; temp_sPH_outer_nocolumn_vec.clear();
    vector<vector<double>> temp_sPH_nocolumn_vec(2);
    vector<vector<double>> temp_sPH_nocolumn_rz_vec(2);

    TH1F * temp_event_zvtx = new TH1F("","Z vertex dist",125,zvtx_hist_l,zvtx_hist_r);
    temp_event_zvtx -> GetXaxis() -> SetTitle("Z vertex position (mm)");
    temp_event_zvtx -> GetYaxis() -> SetTitle("Entry");
    vector<float> temp_event_zvtx_vec; temp_event_zvtx_vec.clear();
    vector<float> temp_event_zvtx_info; temp_event_zvtx_info.clear();
    TLine * eff_sig_range_line = new TLine();
    eff_sig_range_line -> SetLineWidth(3);
    eff_sig_range_line -> SetLineColor(TColor::GetColor("#A08144"));
    eff_sig_range_line -> SetLineStyle(2);
    // TF1 * zvtx_fitting = new TF1("","gaus_func",-500,500,4);

    double N_good_event = 0;

    TF1 * zvtx_finder = new TF1("zvtx_finder","pol0",-1,6000); 
    zvtx_finder -> SetLineColor(2);
    zvtx_finder -> SetLineWidth(1);

    
    vector<vector<double>> good_track_xy_vec; good_track_xy_vec.clear();
    vector<vector<double>> good_track_rz_vec; good_track_rz_vec.clear();
    vector<vector<double>> good_comb_rz_vec; good_comb_rz_vec.clear();
    vector<vector<double>> good_comb_xy_vec; good_comb_xy_vec.clear();
    vector<vector<double>> good_comb_phi_vec; good_comb_phi_vec.clear();
    vector<vector<double>> good_tracklet_rz; good_tracklet_rz.clear();
    TLine * track_line = new TLine();
    track_line -> SetLineWidth(1);
    track_line -> SetLineColorAlpha(38,0.5);

    TLine * coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(16);
    coord_line -> SetLineStyle(2);


    vector<float> avg_event_zvtx_vec; avg_event_zvtx_vec.clear();
    vector<float> Z_resolution_vec; Z_resolution_vec.clear();
    TH1F * avg_event_zvtx = new TH1F("","avg_event_zvtx",100,zvtx_hist_l,zvtx_hist_r);
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

    TH1F * zvtx_evt_width = new TH1F("","zvtx_evt_width",150,0,800);
    zvtx_evt_width -> GetXaxis() -> SetTitle(" mm ");
    zvtx_evt_width -> GetYaxis() -> SetTitle("entry");
    zvtx_evt_width -> GetXaxis() -> SetNdivisions(505);

    TH2F * zvtx_evt_fitError_corre = new TH2F("","zvtx_evt_fitError_corre",200,0,10000,200,0,20);
    zvtx_evt_fitError_corre -> GetXaxis() -> SetTitle(" # of clusters ");
    zvtx_evt_fitError_corre -> GetYaxis() -> SetTitle(" #pm mm ");
    zvtx_evt_fitError_corre -> GetXaxis() -> SetNdivisions(505);

    TH2F * zvtx_evt_width_corre = new TH2F("","zvtx_evt_width_corre",200,0,10000,200,0,300);
    zvtx_evt_width_corre -> GetXaxis() -> SetTitle(" # of clusters ");
    zvtx_evt_width_corre -> GetYaxis() -> SetTitle(" mm ");
    zvtx_evt_width_corre -> GetXaxis() -> SetNdivisions(505);

    TH2F * zvtx_evt_nclu_corre = new TH2F("","zvtx_evt_nclu_corre",200,0,10000,200,-500,500);
    zvtx_evt_nclu_corre -> GetXaxis() -> SetTitle(" # of clusters ");
    zvtx_evt_nclu_corre -> GetYaxis() -> SetTitle(" zvtx [mm] ");
    zvtx_evt_nclu_corre -> GetXaxis() -> SetNdivisions(505);

    TH1F * width_density = new TH1F("","width_density",200,0,40); // note : N good hits / width
    width_density -> GetXaxis() -> SetTitle(" N good zvtx / width ");
    width_density -> GetYaxis() -> SetTitle(" Entry ");
    width_density -> GetXaxis() -> SetNdivisions(505);

    TH2F * Z_resolution_Nclu = new TH2F("","Z_resolution_Nclu",200,0,10000,200,-100,100); // note : N good hits / width
    Z_resolution_Nclu -> GetXaxis() -> SetTitle(" # of clusters ");
    Z_resolution_Nclu -> GetYaxis() -> SetTitle(" #Delta Z (Reco - True) [mm]");
    Z_resolution_Nclu -> GetXaxis() -> SetNdivisions(505);

    TH2F * Z_resolution_pos = new TH2F("","Z_resolution_pos",200,-350,350,200,-100,100); // note : N good hits / width
    Z_resolution_pos -> GetXaxis() -> SetTitle("True Zvtx [mm]");
    Z_resolution_pos -> GetYaxis() -> SetTitle(" #Delta Z (Reco - True) [mm]");
    Z_resolution_pos -> GetXaxis() -> SetNdivisions(505);

    TH2F * Z_resolution_pos_cut = new TH2F("","Z_resolution_pos_cut",200,-350,350,200,-100,100); // note : N good hits / width
    Z_resolution_pos_cut -> GetXaxis() -> SetTitle("True Zvtx [mm]");
    Z_resolution_pos_cut -> GetYaxis() -> SetTitle(" #Delta Z (Reco - True) [mm]");
    Z_resolution_pos_cut -> GetXaxis() -> SetNdivisions(505);

    TH1F * Z_resolution = new TH1F("","Z_resolution",200,-100,100); // note : N good hits / width
    Z_resolution -> GetXaxis() -> SetTitle(" #Delta Z (Reco - True) [mm]");
    Z_resolution -> GetYaxis() -> SetTitle(" Entry ");
    Z_resolution -> GetXaxis() -> SetNdivisions(505);

    TH1F * evt_possible_z = new TH1F("","evt_possible_z",100,-700,700);
    evt_possible_z -> SetLineWidth(1);
    evt_possible_z -> GetXaxis() -> SetTitle("Z [mm]");
    evt_possible_z -> GetYaxis() -> SetTitle("Entry");
    TF1 * gaus_fit = new TF1("gaus_fit",gaus_func,-600,600,4);
    gaus_fit -> SetLineColor(2);
    gaus_fit -> SetLineWidth(1);
    gaus_fit -> SetNpx(1000);

    TH2F * gaus_width_Nclu = new TH2F("","gaus_width_Nclu",200,0,10000,200,0,100);
    gaus_width_Nclu -> GetXaxis() -> SetTitle(" # of clusters ");
    gaus_width_Nclu -> GetYaxis() -> SetTitle("Gaus fit width [mm]");
    gaus_width_Nclu -> GetXaxis() -> SetNdivisions(505);

    TH2F * gaus_rchi2_Nclu = new TH2F("","gaus_rchi2_Nclu",200,0,10000,200,0,25);
    gaus_rchi2_Nclu -> GetXaxis() -> SetTitle(" # of clusters ");
    gaus_rchi2_Nclu -> GetYaxis() -> SetTitle("Gaus fit #chi2^{2}/NDF");
    gaus_rchi2_Nclu -> GetXaxis() -> SetNdivisions(505);
    

    vector<TH1F * > DeltaPhi_DCA; DeltaPhi_DCA.clear();
    for (int i = 0; i < 20; i++){
        DeltaPhi_DCA.push_back(new TH1F("",Form("range : %i to %i",i * 500, (i + 1) * 500),100,0,5));
        cout<<Form("hist %i, range : %i to %i", i, i * 500, (i + 1) * 500)<<endl;
        DeltaPhi_DCA[i] -> GetXaxis() -> SetTitle(" #sqrt{#Delta#phi^{2} + DCA^{2}}");
        DeltaPhi_DCA[i] -> GetYaxis() -> SetTitle(" Entry ");
        DeltaPhi_DCA[i] -> GetXaxis() -> SetNdivisions(505); 
    }


    
    int inner_1_check = 0; int inner_2_check = 0; int inner_3_check = 0; int inner_4_check = 0;
    int outer_1_check = 0; int outer_2_check = 0; int outer_3_check = 0; int outer_4_check = 0;
    vector<int> used_outer_check(temp_sPH_outer_nocolumn_vec.size(),0);
    vector<float> N_comb; vector<float> N_comb_e; vector<float> z_mid; vector<float> z_range;
    vector<double> eff_N_comb; vector<double> eff_z_mid; vector<double> eff_N_comb_e; vector<double> eff_z_range;

    int N_event_pass_number = 0;
    long good_comb_id = 0;
    
    if (draw_event_display) c2 -> Print(Form("%s/temp_event_display.pdf(",out_folder_directory.c_str()));

    for (int event_i = 0; event_i < 20000; event_i++)
    {
        if (event_i % 1000 == 0) cout<<"code running... "<<event_i<<endl;
        inttDSTMC.LoadTree(event_i);
        inttDSTMC.GetEntry(event_i);
        unsigned int length = inttDSTMC.ClusX->size();

        out_eID = event_i;
        N_cluster_inner_out = -1;
        N_cluster_outer_out = -1;
        out_zvtx = -1;
        out_zvtxE = -1;
        out_rangeL = -1;
        out_rangeR = -1;
        out_N_good = -1;
        bco_full_out = -1;
        MC_true_zvtx = -1000;
        out_width_density = -1;

        // if (event_i == 13) cout<<"test, eID : "<<event_i<<" Nhits "<<N_hits<<endl;

        // todo current MC tree doesn't have this information
        // if (N_hits > Nhit_cut) {tree_out -> Fill(); continue;}
        // if (N_hits < Nhit_cutl) {tree_out -> Fill(); continue;}

        N_event_pass_number += 1;

        // if (N_cluster_inner == 0 || N_cluster_outer == 0) {tree_out -> Fill(); continue;}
        // if (N_cluster_inner == -1 || N_cluster_outer == -1) {tree_out -> Fill(); continue;}
        if ((length) < zvtx_cal_require) {tree_out -> Fill(); continue;}   
        if ((length) < N_clu_cutl) {printf("low clu continue, NClus : %i \n", length); tree_out -> Fill(); continue;}
        if (inttDSTMC.TruthPV_x -> size() != 1) {cout<<"Nvtx more than one, event : "<<event_i<<" Nvtx : "<<inttDSTMC.TruthPV_x -> size()<<endl; tree_out -> Fill(); continue;}

        // note : apply some selection to remove the hot channels
        // note : and make the inner_clu_vec and outer_clu_vec
        for (int clu_i = 0; clu_i < length; clu_i++)
        {
            if (int(inttDSTMC.ClusPhiSize -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttDSTMC.ClusAdc -> at(clu_i)) < clu_sum_adc_cut) continue;

            double clu_x = inttDSTMC.ClusX -> at(clu_i) * 10; // note : change the unit from cm to mm
            double clu_y = inttDSTMC.ClusY -> at(clu_i) * 10;
            double clu_z = inttDSTMC.ClusZ -> at(clu_i) * 10;
            double clu_phi = (clu_y < 0) ? atan2(clu_y,clu_x) * (180./TMath::Pi()) + 360 : atan2(clu_y,clu_x) * (180./TMath::Pi());
            int    clu_layer = (inttDSTMC.ClusLayer -> at(clu_i) == 3 || inttDSTMC.ClusLayer -> at(clu_i) == 4) ? 0 : 1;
            double clu_radius = get_radius(clu_x, clu_y);

            temp_sPH_nocolumn_vec[0].push_back( clu_x );
            temp_sPH_nocolumn_vec[1].push_back( clu_y );
            
            temp_sPH_nocolumn_rz_vec[0].push_back( clu_z );
            temp_sPH_nocolumn_rz_vec[1].push_back( ( clu_phi > 180 ) ? clu_radius * -1 : clu_radius );
            

            if (clu_layer == 0) // note : inner
                temp_sPH_inner_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttDSTMC.ClusAdc -> at(clu_i)), 
                    int(inttDSTMC.ClusAdc -> at(clu_i)), 
                    int(inttDSTMC.ClusPhiSize -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    clu_layer, 
                    clu_phi
                });
            
            if (clu_layer == 1) // note : outer
                temp_sPH_outer_nocolumn_vec.push_back({
                    -1, 
                    -1, 
                    int(inttDSTMC.ClusAdc -> at(clu_i)), 
                    int(inttDSTMC.ClusAdc -> at(clu_i)), 
                    int(inttDSTMC.ClusPhiSize -> at(clu_i)), 
                    clu_x, 
                    clu_y, 
                    clu_z, 
                    clu_layer, 
                    clu_phi
                });                    
        }

        inner_1_check = 0;
        inner_2_check = 0;
        inner_3_check = 0;
        inner_4_check = 0;
        for (int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++) {
            if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 0 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 90)    inner_1_check = 1;
            if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 90 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 180)  inner_2_check = 1;
            if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 180 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 270) inner_3_check = 1;
            if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 270 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 360) inner_4_check = 1;

            if ( (inner_1_check + inner_2_check + inner_3_check + inner_4_check) == 4 ) break;
        }

        outer_1_check = 0;
        outer_2_check = 0;
        outer_3_check = 0;
        outer_4_check = 0;
        for (int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++) {
            if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 0 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 90)    outer_1_check = 1;
            if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 90 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 180)  outer_2_check = 1;
            if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 180 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 270) outer_3_check = 1;
            if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 270 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 360) outer_4_check = 1;

            if ( (outer_1_check + outer_2_check + outer_3_check + outer_4_check) == 4 ) break;
        }

        if (temp_sPH_inner_nocolumn_vec.size() < 10 || temp_sPH_outer_nocolumn_vec.size() < 10 || (temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size()) > N_clu_cut || (temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size()) < N_clu_cutl)
        {
            temp_event_zvtx_info = {-1000,-1000,-1000};
            temp_event_zvtx_vec.clear();
            temp_event_zvtx -> Reset("ICESM");
            good_track_xy_vec.clear();
            good_track_rz_vec.clear();
            good_comb_rz_vec.clear();
            good_comb_xy_vec.clear();
            good_comb_phi_vec.clear();
            temp_sPH_nocolumn_rz_vec.clear(); temp_sPH_nocolumn_rz_vec = vector<vector<double>>(2);
            temp_sPH_nocolumn_vec.clear(); temp_sPH_nocolumn_vec = vector<vector<double>>(2);
            temp_sPH_inner_nocolumn_vec.clear();
            temp_sPH_outer_nocolumn_vec.clear();
            tree_out -> Fill();
            continue;
        }

        if ( (inner_1_check + inner_2_check + inner_3_check + inner_4_check + outer_1_check + outer_2_check + outer_3_check + outer_4_check) != 8 )
        {
            cout<<"some quater of INTT doens't work !! eID : "<<event_i<<endl;
            temp_event_zvtx_info = {-1000,-1000,-1000};
            temp_event_zvtx_vec.clear();
            temp_event_zvtx -> Reset("ICESM");
            good_track_xy_vec.clear();
            good_track_rz_vec.clear();
            good_comb_rz_vec.clear();
            good_comb_xy_vec.clear();
            good_comb_phi_vec.clear();
            temp_sPH_nocolumn_rz_vec.clear(); temp_sPH_nocolumn_rz_vec = vector<vector<double>>(2);
            temp_sPH_nocolumn_vec.clear(); temp_sPH_nocolumn_vec = vector<vector<double>>(2);
            temp_sPH_inner_nocolumn_vec.clear();
            temp_sPH_outer_nocolumn_vec.clear();
            tree_out -> Fill();
            continue;
        }
        
        N_comb.clear();
        N_comb_e.clear();
        z_mid.clear(); 
        z_range.clear();
        

        // note : try to make sure that the clusters not to be used twice or more 
        // used_outer_check.clear(); used_outer_check = vector<int>(temp_sPH_outer_nocolumn_vec.size(),0);

        for ( int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++ )
        {
            
            for ( int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++ )
            {
                // bool DCA_tag = false;
                // if (used_outer_check[outer_i] == 1) continue; // note : this outer cluster was already used, skip the trial of this combination

                // double DCA_sign = calculateAngleBetweenVectors(
                //     temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
                //     temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
                //     beam_origin.first, beam_origin.second
                // );

                // if (DCA_info_vec[0] != fabs(DCA_sign) && fabs( DCA_info_vec[0] - fabs(DCA_sign) ) > 0.1 ){
                //     cout<<"different DCA : "<<DCA_info_vec[0]<<" "<<DCA_sign<<" diff : "<<DCA_info_vec[0] - fabs(DCA_sign)<<endl;}

                // DeltaPhi_DCA[ int(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size())/500 ] -> Fill(sqrt(pow(fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi),2) + pow(,2)));

                if (fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi) < phi_diff_cut)
                {
                    vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
                        temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
                        temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
                        beam_origin.first, beam_origin.second
                    );

                    if (DCA_info_vec[0] < DCA_cut){

                        // used_outer_check[outer_i] = 1; //note : this outer cluster was already used!

                        pair<double,double> z_range_info = Get_possible_zvtx( 
                            get_radius(beam_origin.first,beam_origin.second), 
                            {get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y), temp_sPH_inner_nocolumn_vec[inner_i].z}, // note : unsign radius
                            {get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y), temp_sPH_outer_nocolumn_vec[outer_i].z}  // note : unsign radius
                        );
                        
                        N_comb.push_back(good_comb_id);
                        N_comb_e.push_back(0);
                        z_mid.push_back(z_range_info.first);
                        z_range.push_back(z_range_info.second);

                        evt_possible_z -> Fill(z_range_info.first);

                        good_comb_id += 1;

                        // DCA_tag = true;
                    }

                    // if(DCA_tag == true) break; // note : since this combination (one inner cluster, one outer cluster) satisfied the reuqiremet, no reason to ask this inner cluster try with other outer clusters

                    // cout<<"good comb : "<<fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi)<<" radius in : "<<get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y)<<" radius out : "<<get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y)<<endl;
                } // note : end of if 
                    

            } // note : end of outer loop
        } // note : end of inner loop

        good_comb_id = 0;

        // cout<<"test tag 0"<<endl;
        TGraphErrors * z_range_gr;
        eff_N_comb.clear();
        eff_z_mid.clear();
        eff_N_comb_e.clear();
        eff_z_range.clear();
        // cout<<"test tag 1"<<endl;
        if (N_comb.size() > zvtx_cal_require)
        {   
            double final_selection_widthU, final_selection_widthD;

            gaus_fit -> SetParameters(evt_possible_z -> GetBinContent( evt_possible_z -> GetMaximumBin() ), evt_possible_z -> GetBinCenter( evt_possible_z -> GetMaximumBin() ), 40, 0);
            gaus_fit -> SetParLimits(3,0,10000);
            evt_possible_z -> Fit(gaus_fit, "NQ");

            if (temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size() < 1000) {
                temp_event_zvtx_info = sigmaEff_avg(z_mid,Integrate_portion);
                final_selection_widthU = ( fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) / 2. < fabs(gaus_fit -> GetParameter(2)) ) ? temp_event_zvtx_info[2] : (gaus_fit -> GetParameter(1) + fabs(gaus_fit -> GetParameter(2)));
                final_selection_widthD = ( fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) / 2. < fabs(gaus_fit -> GetParameter(2)) ) ? temp_event_zvtx_info[1] : (gaus_fit -> GetParameter(1) - fabs(gaus_fit -> GetParameter(2)));
            }
            else {
                temp_event_zvtx_info = {0,-1000,-999.99};
                final_selection_widthU = (gaus_fit -> GetParameter(1) + fabs(gaus_fit -> GetParameter(2)));
                final_selection_widthD = (gaus_fit -> GetParameter(1) - fabs(gaus_fit -> GetParameter(2)));
            }

            

            for (int track_i = 0; track_i < N_comb.size(); track_i++) {
                // if (temp_event_zvtx_info[1] <= z_mid[track_i] && z_mid[track_i] <= temp_event_zvtx_info[2]) {
                if ( final_selection_widthD <= z_mid[track_i] && z_mid[track_i] <= final_selection_widthU ){
                    eff_N_comb.push_back(N_comb[track_i]);
                    eff_z_mid.push_back(z_mid[track_i]);
                    eff_N_comb_e.push_back(N_comb_e[track_i]);
                    eff_z_range.push_back(z_range[track_i]);
                }
            }

            z_range_gr = new TGraphErrors(eff_N_comb.size(),&eff_N_comb[0],&eff_z_mid[0],&eff_N_comb_e[0],&eff_z_range[0]);
            // z_range_gr = new TGraph(eff_N_comb.size(),&eff_N_comb[0],&eff_z_mid[0]);
            z_range_gr -> Fit(zvtx_finder,"NQ","",0,N_comb[N_comb.size() - 1]); // note : not fit all the combination

            gaus_width_Nclu -> Fill(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(), fabs(gaus_fit -> GetParameter(2)));
            gaus_rchi2_Nclu -> Fill(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(), gaus_fit -> GetChisquare() / double(gaus_fit -> GetNDF()));
            
            // avg_event_zvtx -> Fill(zvtx_finder -> GetParameter(0));
            zvtx_evt_width -> Fill(fabs( zvtx_finder -> GetParError(0)));
            zvtx_evt_fitError_corre -> Fill(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(), fabs( zvtx_finder -> GetParError(0)));
            zvtx_evt_width_corre -> Fill(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(), fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]));
            width_density -> Fill( eff_N_comb.size() / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) );
            if ( ( eff_N_comb.size() / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) ) > 0.3 ){ // Todo : change the width density here
                zvtx_evt_nclu_corre -> Fill(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(), zvtx_finder -> GetParameter(0));
                avg_event_zvtx -> Fill(zvtx_finder -> GetParameter(0));
                avg_event_zvtx_vec.push_back(zvtx_finder -> GetParameter(0));
                Z_resolution -> Fill( zvtx_finder -> GetParameter(0) - (inttDSTMC.TruthPV_trig_z*10.) );
                Z_resolution_vec.push_back( zvtx_finder -> GetParameter(0) - (inttDSTMC.TruthPV_trig_z*10.) );
                Z_resolution_Nclu -> Fill( temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size() , zvtx_finder -> GetParameter(0) - (inttDSTMC.TruthPV_trig_z*10.) );
                Z_resolution_pos -> Fill(inttDSTMC.TruthPV_trig_z*10., zvtx_finder -> GetParameter(0) - (inttDSTMC.TruthPV_trig_z*10.));
                if (temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size() > 1000) {Z_resolution_pos_cut -> Fill(inttDSTMC.TruthPV_trig_z*10., zvtx_finder -> GetParameter(0) - (inttDSTMC.TruthPV_trig_z*10.));}
            }
            
            out_eID = event_i;
            N_cluster_inner_out = temp_sPH_inner_nocolumn_vec.size();
            N_cluster_outer_out = temp_sPH_outer_nocolumn_vec.size();
            out_zvtx = zvtx_finder -> GetParameter(0);
            out_zvtxE = zvtx_finder -> GetParError(0);
            out_rangeL = temp_event_zvtx_info[1];
            out_rangeR = temp_event_zvtx_info[2];
            out_N_good = eff_N_comb.size();
            bco_full_out = -1;
            MC_true_zvtx = inttDSTMC.TruthPV_trig_z*10.;
            out_width_density = eff_N_comb.size() / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]);
            tree_out -> Fill();

            z_range_gr -> Delete();
        } // note : if N good tracks in xy found > certain value
        // cout<<"test tag 2"<<endl;
        else {tree_out -> Fill();}
            

        if (zvtx_draw_requireL < N_comb.size() && draw_event_display == true && N_comb.size() > zvtx_cal_require)
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
            temp_bkg(pad_xy, conversion_mode, peek, beam_origin, ch_pos_DB);
            temp_event_xy -> Draw("p same");
            for (int track_i = 0; track_i < good_track_xy_vec.size(); track_i++){
                track_line -> DrawLine(good_track_xy_vec[track_i][0],good_track_xy_vec[track_i][1],good_track_xy_vec[track_i][2],good_track_xy_vec[track_i][3]);
            }
            track_line -> Draw("l same");
            draw_text -> DrawLatex(0.2, 0.85, Form("eID : %i, innter Ncluster : %i, outer Ncluster : %i",event_i,temp_sPH_inner_nocolumn_vec.size(),temp_sPH_outer_nocolumn_vec.size()));
        
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

            // cout<<"test tag 2-5"<<endl;    
            pad_z -> cd();
            TGraphErrors * z_range_gr_draw = new TGraphErrors(N_comb.size(),&N_comb[0],&z_mid[0],&N_comb_e[0],&z_range[0]);
            z_range_gr_draw -> GetYaxis() -> SetRangeUser(-650,650);
            z_range_gr_draw -> SetMarkerStyle(20);
            z_range_gr_draw -> Draw("ap");
            
            draw_text -> DrawLatex(0.2, 0.82, Form("Event Zvtx %.2f mm, error : #pm%.2f", zvtx_finder -> GetParameter(0), zvtx_finder -> GetParError(0)));
            draw_text -> DrawLatex(0.2, 0.78, Form("Width density : %.2f", ( eff_N_comb.size() / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) )));
            draw_text -> DrawLatex(0.2, 0.74, Form("Width : %.2f to %.2f mm", temp_event_zvtx_info[2] , temp_event_zvtx_info[1]));

            eff_sig_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),temp_event_zvtx_info[1],z_range_gr_draw->GetXaxis()->GetXmax(),temp_event_zvtx_info[1]);
            eff_sig_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),temp_event_zvtx_info[2],z_range_gr_draw->GetXaxis()->GetXmax(),temp_event_zvtx_info[2]);
            z_range_gr_draw -> Draw("p same");
            zvtx_finder -> Draw("lsame");


            pad_z_hist -> cd();
            evt_possible_z -> Draw("hist");
            gaus_fit -> Draw("lsame");
            draw_text -> DrawLatex(0.2, 0.82, Form("Gaus mean %.2f mm", gaus_fit -> GetParameter(1)));
            draw_text -> DrawLatex(0.2, 0.78, Form("Width : %.2f mm", fabs(gaus_fit -> GetParameter(2))));
            draw_text -> DrawLatex(0.2, 0.74, Form("Reduced #chi2 : %.3f", gaus_fit -> GetChisquare() / double(gaus_fit -> GetNDF())));

            // temp_event_zvtx -> Draw("hist");
            // // zvtx_fitting -> Draw("lsame");
            // eff_sig_range_line -> DrawLine(temp_event_zvtx_info[1],0,temp_event_zvtx_info[1],temp_event_zvtx -> GetMaximum()*1.05);
            // eff_sig_range_line -> DrawLine(temp_event_zvtx_info[2],0,temp_event_zvtx_info[2],temp_event_zvtx -> GetMaximum()*1.05);
            // draw_text -> DrawLatex(0.2, 0.85, Form("eID : %i, Total event hit : %i, innter Ncluster : %i, outer Ncluster : %i",event_i,N_hits,temp_sPH_inner_nocolumn_vec.size(),temp_sPH_outer_nocolumn_vec.size()));
            // // draw_text -> DrawLatex(0.2, 0.84, Form("Gaus fit mean : %.3f mm",zvtx_fitting -> GetParameter(1)));
            // draw_text -> DrawLatex(0.2, 0.82, Form("EffSig min : %.2f mm, max : %.2f mm",temp_event_zvtx_info[1],temp_event_zvtx_info[2]));
            // draw_text -> DrawLatex(0.2, 0.79, Form("EffSig avg : %.2f mm",temp_event_zvtx_info[0]));

            if(draw_event_display && (event_i % print_rate) == 0){c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str()));}
            if(fabs(gaus_fit -> GetParameter(2)) > 40) { 
                cout<<"check event : "<<event_i<<" width : "<<fabs(gaus_fit -> GetParameter(2))<<" NClu : "<<temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size()<<endl; 
                c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str())); 
            }
            if (fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1])>100){
                cout<<"check event : "<<event_i<<" eff_width : "<<fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1])<<" NClu : "<<temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size()<<endl; 
                c2 -> Print(Form("%s/temp_event_display.pdf",out_folder_directory.c_str()));
            }
            pad_xy -> Clear();
            pad_rz -> Clear();
            pad_z  -> Clear();
            pad_z_hist -> Clear();

            temp_event_xy -> Delete();
            temp_event_rz -> Delete();
            z_range_gr_draw -> Delete();

        }
        // cout<<"test tag 3"<<endl;

        evt_possible_z -> Reset("ICESM");
        temp_event_zvtx_info = {-1000,-1000,-1000};
        temp_event_zvtx_vec.clear();
        temp_event_zvtx -> Reset("ICESM");
        good_track_xy_vec.clear();
        good_track_rz_vec.clear();
        good_comb_rz_vec.clear();
        good_comb_xy_vec.clear();
        good_comb_phi_vec.clear();
        temp_sPH_nocolumn_rz_vec.clear(); temp_sPH_nocolumn_rz_vec = vector<vector<double>>(2);
        temp_sPH_nocolumn_vec.clear(); temp_sPH_nocolumn_vec = vector<vector<double>>(2);
        temp_sPH_inner_nocolumn_vec.clear();
        temp_sPH_outer_nocolumn_vec.clear();
        N_comb.clear();
        N_comb_e.clear();
        z_mid.clear();
        z_range.clear();
    } // note : end of event 

    if (draw_event_display) {c2 -> Print(Form("%s/temp_event_display.pdf)",out_folder_directory.c_str()));}
    c2 -> Clear();
    c1 -> Clear();

    tree_out->SetDirectory(out_file);
    tree_out -> Write("", TObject::kOverwrite);

    cout<<"test1, z size : "<<avg_event_zvtx_vec.size()<<endl;    

    c1 -> cd();
    vector<float> avg_event_zvtx_info = sigmaEff_avg(avg_event_zvtx_vec,Integrate_portion_final);

    avg_event_zvtx -> SetMinimum( 0 );  avg_event_zvtx -> SetMaximum( avg_event_zvtx->GetBinContent(avg_event_zvtx->GetMaximumBin()) * 1.5 );
    avg_event_zvtx -> Draw("hist");

    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    // ltx->DrawLatex(0.54, 0.86, Form("Run %s",run_ID.c_str()));
    // ltx->DrawLatex(0.54, 0.86, "Au+Au #sqrt{s_{NN}} = 200 GeV");

    eff_sig_range_line -> DrawLine(avg_event_zvtx_info[1],0,avg_event_zvtx_info[1],avg_event_zvtx -> GetMaximum());
    eff_sig_range_line -> DrawLine(avg_event_zvtx_info[2],0,avg_event_zvtx_info[2],avg_event_zvtx -> GetMaximum());    
    draw_text -> DrawLatex(0.21, 0.87, Form("EffSig min : %.2f mm",avg_event_zvtx_info[1]));
    draw_text -> DrawLatex(0.21, 0.83, Form("EffSig max : %.2f mm",avg_event_zvtx_info[2]));
    draw_text -> DrawLatex(0.21, 0.79, Form("EffSig avg : %.2f mm",avg_event_zvtx_info[0]));
    c1 -> Print(Form("%s/avg_event_zvtx.pdf",out_folder_directory.c_str()));
    c1 -> Clear();



    width_density -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/width_density.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    zvtx_evt_width -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/zvtx_evt_width.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    vector<float> Z_resolution_vec_info = sigmaEff_avg(Z_resolution_vec,Integrate_portion_final);

    gaus_fit -> SetParameters(Z_resolution -> GetBinContent( Z_resolution -> GetMaximumBin() ), Z_resolution -> GetBinCenter( Z_resolution -> GetMaximumBin() ), 3, 0);
    gaus_fit -> SetParLimits(3,0,10);
    Z_resolution -> Fit(gaus_fit, "NQ");

    Z_resolution -> Draw("hist"); 
    gaus_fit     -> Draw("lsame");
    eff_sig_range_line -> DrawLine(Z_resolution_vec_info[1],0,Z_resolution_vec_info[1],Z_resolution -> GetMaximum());
    eff_sig_range_line -> DrawLine(Z_resolution_vec_info[2],0,Z_resolution_vec_info[2],Z_resolution -> GetMaximum());    
    draw_text -> DrawLatex(0.21, 0.87, Form("EffSig min : %.2f mm",Z_resolution_vec_info[1]));
    draw_text -> DrawLatex(0.21, 0.83, Form("EffSig max : %.2f mm",Z_resolution_vec_info[2]));
    draw_text -> DrawLatex(0.21, 0.79, Form("EffSig avg : %.2f mm",Z_resolution_vec_info[0]));
    draw_text -> DrawLatex(0.21, 0.71, Form("Gaus mean  : %.2f mm",gaus_fit -> GetParameter(1)));
    draw_text -> DrawLatex(0.21, 0.67, Form("Gaus width : %.2f mm",fabs(gaus_fit -> GetParameter(2))));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/Z_resolution.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    Z_resolution_Nclu -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/Z_resolution_Nclu.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    Z_resolution_pos -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/Z_resolution_pos.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    Z_resolution_pos_cut -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/Z_resolution_pos_cut.pdf",out_folder_directory.c_str()));
    c1 -> Clear();


    zvtx_evt_fitError_corre -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/zvtx_evt_fitError_corre.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    zvtx_evt_nclu_corre -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/zvtx_evt_nclu_corre.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    zvtx_evt_width_corre -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/zvtx_evt_width_corre.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    gaus_width_Nclu -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/gaus_width_Nclu.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    gaus_rchi2_Nclu -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/gaus_rchi2_Nclu.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
}
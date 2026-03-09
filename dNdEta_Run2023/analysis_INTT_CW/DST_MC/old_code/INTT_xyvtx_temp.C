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

double sin_func(double *x, double *par)
{
    return par[0] * sin(par[1] * x[0] + par[2]) + par[3];
}

double cos_func(double *x, double *par)
{
    return -1 * par[0] * cos(par[1] * (x[0] + par[2])) + par[3];
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
double Get_eta_2P(vector<double>p0, vector<double>p1)
{
    // vector<double> r_vec  = {p0[0], p1[0]}; 
    // vector<double> z_vec  = {p0[1], p1[1]}; 
    // vector<double> re_vec = {0, 0}; 
    // vector<double> ze_vec = {p0[2], ( fabs( p1[1] ) < 130 ) ? 8. : 10.}; 

    // note : swap the r and z, to have easier fitting 
    // note : in principle, Z should be in the x axis, R should be in the Y axis
    // TGraphErrors * track_gr = new TGraphErrors(3,&r_vec[0],&z_vec[0],&re_vec[0],&ze_vec[0]);    
    
    // double vertical_line = ( fabs( grEY_stddev(track_gr) ) < 0.00001 ) ? 1 : 0;
    
    return  -1 * TMath::Log( fabs( tan( atan2(p1[0] - p0[0], p1[1] - p0[1]) / 2 ) ) );

    // if (vertical_line) {return 0;}
    // else 
    // {
    //     TF1 * fit_rz = new TF1("fit_rz","pol1",-500,500);
    //     fit_rz -> SetParameters(0,0);

    //     track_gr -> Fit(fit_rz,"NQ");

    //     pair<double,double> ax_b = mirrorPolynomial(fit_rz -> GetParameter(1),fit_rz -> GetParameter(0));
    // }

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

void TH2F_threshold(TH2F * input_hist, double threshold)
{
    for (int xi = 0; xi < input_hist -> GetNbinsX(); xi++){
        for(int yi = 0; yi < input_hist -> GetNbinsY(); yi++){
            if (input_hist -> GetBinContent(xi + 1, yi +1) < threshold){ input_hist -> SetBinContent(xi + 1, yi +1, 0); }
        }
    }
} 

// note : rotate the ClusX and ClusY
pair<double,double> rotatePoint(double x, double y) {
    // Convert the rotation angle from degrees to radians
    double rotation = 180.; // todo rotation is here
    double angleRad = rotation * M_PI / 180.0;

    // Perform the rotation
    double xOut = x * cos(angleRad) - y * sin(angleRad);
    double yOut = x * sin(angleRad) + y * cos(angleRad);

    return {xOut,yOut};
}

// note : use "ls *.root > file_list.txt" to create the list of the file in the folder, full directory in the file_list.txt
// note : set_folder_name = "folder_xxxx"
// note : server_name = "inttx"
void INTT_xyvtx(/*bool full_event, int run_Nevent*/)
{
    
    SetsPhenixStyle();

    TCanvas * c4 = new TCanvas("","",1200,800);    
    c4 -> cd();
    
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
    // string file_name = "beam_inttall-000" +run_ID+ "-0000_event_base_ana_cluster_"+ conversion_mode_BD[geo_mode_id] +"_excludeR1500_"+ file_event +"kEvent_3HotCut";

    int geo_mode_id = 0;
    string mother_folder_directory = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC_388_000"; string file_name = "MC_ZF_all";
    string MC_list_name = "dst_INTT_dNdEta_388_000.list";

    TChain * chain_in = new TChain("EventTree");
    INTTDSTchain inttDSTMC(chain_in,mother_folder_directory,MC_list_name);
    std::printf("inttDSTMC N event : %lli \n", chain_in->GetEntries());
    long long N_event = chain_in->GetEntries();

    cout<<"the input file name : "<<file_name<<endl;
    sleep(5);

    // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/new_DAC_Scan_0722/AllServer/DAC2";
    // string file_name = "beam_inttall-00023058-0000_event_base_ana_cluster_ideal_excludeR2000_100kEvent";



    // pair<double,double> beam_origin = {-0.457 + 0.0276, 2.657 - 0.2814}; // note : for run20869
    // pair<double,double> beam_origin_U = {0.0004, 0.0035}; // note : for run20869, the uncertainty of the VTXxy, calculated by the fit errors with the error propagation
    // pair<double,double> beam_origin = {-0.015, 0.012}; // note : for the MC run /sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC
    
    pair<double,double> beam_origin = {-0.0, -2};
    // pair<double,double> beam_origin = {-0.4, 2.4};
    // pair<double,double> beam_origin = {-0.0, -0.0};

    // pair<double,double> beam_origin = {1., -4.};
    double temp_Y_align = 0.;
    double temp_X_align = -0.;

    double zvtx_hist_l = -500;
    double zvtx_hist_r = 500;

    // todo : MC doesn't keep Nhits information for the moment
    // int Nhit_cut = 1500;           // note : if (> Nhit_cut)          -> continue
    // int Nhit_cutl = 100;          // note : if (< Nhit_cutl)         -> continue 
    int clu_size_cut = 4;           // note : if (> clu_size_cut)      -> continue
    double clu_sum_adc_cut = -1;    // note : if (< clu_sum_adc_cut)   -> continue
    int N_clu_cut = 500;          // note : if (> N_clu_cut)         -> continue  unit number
    int N_clu_cutl = 20;           // note : if (< N_clu_cutl)        -> continue  unit number
    double phi_diff_cut = 5.72;      // note : if (< phi_diff_cut)      -> pass      unit degree
    double DCA_cut = 4;             // note : if (< DCA_cut)           -> pass      unit mm
    int zvtx_cal_require = 15;      // note : if (> zvtx_cal_require)  -> pass
    // int zvtx_draw_requireL = 15;       
    // int zvtx_draw_requireR = 40000;   // note : if ( zvtx_draw_requireL < event && event < zvtx_draw_requireR) -> pass
    // double Integrate_portion = 0.4; // todo : it was 0.6826, try to require less event, as most of the tracklets are not that useful
    // double Integrate_portion_final = 0.68;
    bool draw_event_display = false;
    int print_rate = 5;

    bool rotate_inner_phi = false;

    string output_directory = (rotate_inner_phi == true) ? Form("%s/folder_%s_advanced_inner_phi_rotate",mother_folder_directory.c_str(),file_name.c_str()) : Form("%s/folder_%s_advanced",mother_folder_directory.c_str(),file_name.c_str());
    system(Form("mkdir %s",output_directory.c_str()));  

    // todo : change the geo draw mode if needed
    int geo_mode_id_draw = 0;
    string conversion_mode = (geo_mode_id_draw == 0) ? "ideal" : "survey_1_XYAlpha_Peek";
    double peek = 3.32405;

    // note : the initiator of the INTT geometry class
    InttConversion * ch_pos_DB = new InttConversion(conversion_mode_BD[geo_mode_id], peek);

    

    vector<clu_info> temp_sPH_inner_nocolumn_vec; temp_sPH_inner_nocolumn_vec.clear();
    vector<clu_info> temp_sPH_outer_nocolumn_vec; temp_sPH_outer_nocolumn_vec.clear();
    vector<vector<double>> temp_sPH_nocolumn_vec(2);
    vector<vector<double>> temp_sPH_nocolumn_rz_vec(2);


    vector<float> temp_event_zvtx_vec; temp_event_zvtx_vec.clear();
    vector<float> temp_event_zvtx_info; temp_event_zvtx_info.clear();
    

    double N_good_event = 0;

    

    


    
    int inner_1_check = 0; int inner_2_check = 0; int inner_3_check = 0; int inner_4_check = 0;
    int outer_1_check = 0; int outer_2_check = 0; int outer_3_check = 0; int outer_4_check = 0;
    vector<int> used_outer_check(temp_sPH_outer_nocolumn_vec.size(),0);
    vector<float> N_comb; vector<float> N_comb_e; vector<float> z_mid; vector<float> z_range;
    vector<double> effi_N_comb; vector<double> effi_z_mid; vector<double> effi_N_comb_e; vector<double> effi_z_range;

    int N_event_pass_number = 0;
    int greatest_N_clu = 0;

    vector<double> angle_diff_vec; angle_diff_vec.clear();
    TTree * tree_out = new TTree("tree","after ana");
    tree_out -> Branch("angle_diff",&angle_diff_vec);
    
    if (draw_event_display) c2 -> Print(Form("%s/temp_event_display.pdf(",output_directory.c_str()));

    for (int event_i = 0; event_i < 20000; event_i++)
    {
        if (event_i % 1000 == 0) cout<<"code running... "<<event_i<<endl;

        // cout<<"========================= test : "<<event_i<<" ========================="<<endl;

        // tree -> GetEntry(event_i);
        inttDSTMC.LoadTree(event_i);
        inttDSTMC.GetEntry(event_i);
        unsigned int length = inttDSTMC.ClusX->size();

        // if (event_i == 13) cout<<"test, eID : "<<event_i<<" Nhits "<<N_hits<<endl;
        
        // todo : MC doesn't keep Nhits information for the moment
        // if (N_hits > Nhit_cut) {continue;}
        // if (N_hits < Nhit_cutl) {continue;}

        N_event_pass_number += 1;

        // if (N_cluster_inner == 0 || N_cluster_outer == 0) {continue;}
        // if (N_cluster_inner == -1 || N_cluster_outer == -1) {continue;}
        if ((length) < zvtx_cal_require) {continue;}
        if (inttDSTMC.TruthPV_trig_z * 10. < -100) {continue;}
        if (inttDSTMC.NTruthVtx != 1) {continue;}
        
        // cout<<"test point 1"<<endl;
        // note : apply some selection to remove the hot channels
        // note : and make the inner_clu_vec and outer_clu_vec
        for (int clu_i = 0; clu_i < length; clu_i++)
        {
            if (int(inttDSTMC.ClusPhiSize -> at(clu_i)) > clu_size_cut) continue; 
            if (int(inttDSTMC.ClusAdc -> at(clu_i)) < clu_sum_adc_cut) continue;

            int    clu_layer = (inttDSTMC.ClusLayer -> at(clu_i) == 3 || inttDSTMC.ClusLayer -> at(clu_i) == 4) ? 0 : 1;
            double clu_x, clu_y;
            if (rotate_inner_phi == true && clu_layer == 0){ // note : inner layer only
                clu_x = rotatePoint(inttDSTMC.ClusX -> at(clu_i) * 10, inttDSTMC.ClusY -> at(clu_i) * 10).first;
                clu_y = rotatePoint(inttDSTMC.ClusX -> at(clu_i) * 10, inttDSTMC.ClusY -> at(clu_i) * 10).second;
            }
            else {
                clu_x = inttDSTMC.ClusX -> at(clu_i) * 10; // note : change the unit from cm to mm
                clu_y = inttDSTMC.ClusY -> at(clu_i) * 10;
            }
          
            double clu_z = inttDSTMC.ClusZ -> at(clu_i) * 10;
            // double clu_phi = (clu_y - 2.4 < 0) ? atan2(clu_y - 2.4,clu_x + 0.4) * (180./TMath::Pi()) + 360 : atan2(clu_y - 2.4,clu_x + 0.4) * (180./TMath::Pi());
            double clu_phi = (clu_y < 0) ? atan2(clu_y,clu_x) * (180./TMath::Pi()) + 360 : atan2(clu_y,clu_x) * (180./TMath::Pi());
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
        // cout<<"test point 2"<<endl;

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

        // cout<<"test point 3"<<endl;

        N_cluster_outer_pass -> Fill(temp_sPH_outer_nocolumn_vec.size());
        N_cluster_inner_pass -> Fill(temp_sPH_inner_nocolumn_vec.size());
        N_cluster_correlation -> Fill( temp_sPH_inner_nocolumn_vec.size(), temp_sPH_outer_nocolumn_vec.size() );

        if ( (temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size()) > greatest_N_clu) {greatest_N_clu = (temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size());}

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
            angle_diff_vec.clear();
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
            angle_diff_vec.clear();
            continue;
        }
        
        N_comb.clear();
        N_comb_e.clear();
        // z_mid.clear(); 
        // z_range.clear();
        

        // note : try to make sure that the clusters not to be used twice or more 
        // used_outer_check.clear();
        // used_outer_check = vector<int>(temp_sPH_outer_nocolumn_vec.size(),0);

        // cout<<"test point 4 di loop start"<<endl;

        for ( int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++ )
        {
            
            for ( int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++ )
            {
                // bool DCA_tag = false;
                // if (used_outer_check[outer_i] == 1) continue; // note : this outer cluster was already used, skip the trial of this combination
                
                vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
                    temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
                    temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
                    beam_origin.first, beam_origin.second
                );

                double DCA_sign = calculateAngleBetweenVectors(
                    temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
                    temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
                    beam_origin.first, beam_origin.second
                );

                angle_correlation -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi,temp_sPH_outer_nocolumn_vec[outer_i].phi);
                angle_diff -> Fill(fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi));
                angle_diff_vec.push_back(fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi));

                if (DCA_info_vec[0] != fabs(DCA_sign) && fabs( DCA_info_vec[0] - fabs(DCA_sign) ) > 0.1 ){
                    cout<<"different DCA : "<<DCA_info_vec[0]<<" "<<DCA_sign<<" diff : "<<DCA_info_vec[0] - fabs(DCA_sign)<<endl;}

                if (fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi) < phi_diff_cut)
                {
                    // if (DCA_info_vec[0] < DCA_cut){

                    //     used_outer_check[outer_i] = 1; //note : this outer cluster was already used!

                    //     pair<double,double> z_range_info = Get_possible_zvtx( 
                    //         get_radius(beam_origin.first,beam_origin.second), 
                    //         {get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y), temp_sPH_inner_nocolumn_vec[inner_i].z}, // note : unsign radius
                    //         {get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y), temp_sPH_outer_nocolumn_vec[outer_i].z}  // note : unsign radius
                    //     );
                        
                    //     N_comb.push_back(inner_i);
                    //     N_comb_e.push_back(0);
                    //     z_mid.push_back(z_range_info.first);
                    //     z_range.push_back(z_range_info.second);

                    //     // DCA_tag = true;
                    // }

                    angle_diff_inner_phi -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi, temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi);
                    angle_diff_outer_phi -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].phi, temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi);

                    if (temp_sPH_inner_nocolumn_vec[inner_i].phi > 270)
                        angle_diff_DCA_dist -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi, DCA_sign);

                    DCA_point -> Fill( DCA_info_vec[1], DCA_info_vec[2] );
                    // angle_correlation -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi,temp_sPH_outer_nocolumn_vec[outer_i].phi);
                    z_pos_diff -> Fill( temp_sPH_inner_nocolumn_vec[inner_i].z - temp_sPH_outer_nocolumn_vec[outer_i].z );
                    z_pos_diff_angle_diff -> Fill( temp_sPH_inner_nocolumn_vec[inner_i].z - temp_sPH_outer_nocolumn_vec[outer_i].z, temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi );
                    // Nhits_good -> Fill(N_hits);
                    z_pos_inner -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].z);
                    z_pos_outer -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].z);
                    z_pos_inner_outer -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].z, temp_sPH_outer_nocolumn_vec[outer_i].z);
                    // DCA_distance_inner_phi -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi, (temp_sPH_inner_nocolumn_vec[inner_i].phi > 90 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 270) ? DCA_sign * -1 : DCA_sign );
                    // DCA_distance_outer_phi -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].phi, (temp_sPH_outer_nocolumn_vec[outer_i].phi > 90 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 270) ? DCA_sign * -1 : DCA_sign );
                    DCA_distance_inner_phi -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi, DCA_sign );
                    DCA_distance_outer_phi -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].phi, DCA_sign );

                    // if(DCA_tag == true) break; // note : since this combination (one inner cluster, one outer cluster) satisfied the reuqiremet, no reason to ask this inner cluster try with other outer clusters

                    // cout<<"good comb : "<<fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi)<<" radius in : "<<get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y)<<" radius out : "<<get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y)<<endl;
                } // note : end of if 
                    

            } // note : end of outer loop
        } // note : end of inner loop

        if ( draw_event_display == true && N_comb.size() > zvtx_cal_require)
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
            draw_text -> DrawLatex(0.2, 0.85, Form("eID : %i, innter Ncluster : %i, outer Ncluster : %i",event_i, temp_sPH_inner_nocolumn_vec.size(), temp_sPH_outer_nocolumn_vec.size()));
        
            pad_rz -> cd();
            temp_event_rz -> Draw("ap");    
            // effi_sig_range_line -> DrawLine(temp_event_zvtx_info[0],-150,temp_event_zvtx_info[0],150);
            coord_line -> DrawLine(0,-150,0,150);
            coord_line -> DrawLine(-500,0,500,0);
            for (int track_i = 0; track_i < good_track_rz_vec.size(); track_i++){
                track_line -> DrawLine(good_track_rz_vec[track_i][0],good_track_rz_vec[track_i][1],good_track_rz_vec[track_i][2],good_track_rz_vec[track_i][3]);
            }
            draw_text -> DrawLatex(0.2, 0.85, Form("Negative radius : Clu_{outer} > 180^{0}"));
            // draw_text -> DrawLatex(0.2, 0.81, Form("EffiSig avg : %.2f mm",temp_event_zvtx_info[0]));

            // cout<<"test tag 2-5"<<endl;    
            pad_z -> cd();

            if(draw_event_display && (event_i % print_rate) == 0){c2 -> Print(Form("%s/temp_event_display.pdf",output_directory.c_str()));}
            pad_xy -> Clear();
            pad_rz -> Clear();
            pad_z  -> Clear();

            temp_event_xy -> Delete();
            temp_event_rz -> Delete();
            // z_range_gr_draw -> Delete();

        }
        // cout<<"test tag 3"<<endl;

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

        // cout<<"test point 7"<<endl;

        
        if (angle_diff_vec.size() > 0) tree_out -> Fill();

        angle_diff_vec.clear();
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

    if (draw_event_display) {c2 -> Print(Form("%s/temp_event_display.pdf)",output_directory.c_str()));}
    c2 -> Clear();
    c1 -> Clear();
   

    c1 -> cd();

    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    N_cluster_inner_pass -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/N_cluster_inner_pass.pdf",output_directory.c_str()));
    c1 -> Clear();

    N_cluster_outer_pass -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/N_cluster_outer_pass.pdf",output_directory.c_str()));
    c1 -> Clear();

    N_cluster_correlation -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/N_cluster_correlation.pdf",output_directory.c_str()));
    c1 -> Clear();

    DCA_point -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/DCA_point_X%.3fY%.3f_.pdf",output_directory.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();

    DCA_distance_inner_phi -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/DCA_distance_inner_phi_X%.3fY%.3f_.pdf",output_directory.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();


    TH2F * DCA_distance_inner_phi_peak = (TH2F*)DCA_distance_inner_phi -> Clone();
    TF1 * cos_fit = new TF1("cos_fit",cos_func,0,360,4);
    cos_fit -> SetParNames("[A]", "[B]", "[C]", "[D]");
	cos_fit -> SetParameters(6.5, 0.015,  -185, 0.3);
    cos_fit -> SetLineColor(2);
    TH2F_threshold(DCA_distance_inner_phi_peak,600); // todo : change the threshold 
    TProfile * DCA_distance_inner_phi_peak_profile =  DCA_distance_inner_phi_peak->ProfileX();
    DCA_distance_inner_phi_peak_profile -> Fit(cos_fit,"N","",50,250);


    DCA_distance_inner_phi_peak -> Draw("colz0");
    DCA_distance_inner_phi_peak_profile -> Draw("same");
    cos_fit -> Draw("l same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/DCA_distance_inner_phi_peak_X%.3fY%.3f_.pdf",output_directory.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();


    DCA_distance_outer_phi -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/DCA_distance_outer_phi_X%.3fY%.3f_.pdf",output_directory.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();

    z_pos_inner_outer -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/z_pos_inner_outer.pdf",output_directory.c_str()));
    c1 -> Clear();

    z_pos_inner -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/z_pos_inner.pdf",output_directory.c_str()));
    c1 -> Clear();

    z_pos_outer -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/z_pos_outer.pdf",output_directory.c_str()));
    c1 -> Clear();

    // Nhits_good -> Draw("hist");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    // c1 -> Print(Form("%s/Nhits_good.pdf",output_directory.c_str()));
    // c1 -> Clear();

    angle_correlation -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/angle_correlation.pdf",output_directory.c_str()));
    c1 -> Clear();

    z_pos_diff -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/z_pos_diff.pdf",output_directory.c_str()));
    c1 -> Clear();

    z_pos_diff_angle_diff -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/z_pos_diff_angle_diff.pdf",output_directory.c_str()));
    c1 -> Clear();

    angle_diff -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/angle_diff.pdf",output_directory.c_str()));
    c1 -> Clear();

    
    angle_diff_DCA_dist -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/angle_diff_DCA_dist.pdf",output_directory.c_str()));
    c1 -> Clear();

    angle_diff_inner_phi -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/angle_diff_inner_phi.pdf",output_directory.c_str()));
    c1 -> Clear();

    angle_diff_outer_phi -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/angle_diff_outer_phi.pdf",output_directory.c_str()));
    c1 -> Clear();

    inner_pos_xy -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/inner_pos_xy.pdf",output_directory.c_str()));
    c1 -> Clear();

    outer_pos_xy -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/outer_pos_xy.pdf",output_directory.c_str()));
    c1 -> Clear();

    inner_outer_pos_xy -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");
    c1 -> Print(Form("%s/inner_outer_pos_xy.pdf",output_directory.c_str()));
    c1 -> Clear();

    TFile * INTT_MC_plot_out = new TFile(Form("%s/MC_angle_diff.root",output_directory.c_str()), "recreate");
    angle_diff -> Write();
    tree_out -> Write("", TObject::kOverwrite);
    INTT_MC_plot_out -> Close();

    cout<<"the geatest N clu event under the fNhits cut : "<<greatest_N_clu<<endl;
}
// #include "DAC_Scan_ladder.h"
//#include "InttConversion.h"
#include "/sphenix/user/ChengWei/INTT/INTT_commissioning/INTT_CW/INTT_commissioning/DAC_Scan/InttClustering.h"
#include "sigmaEff.h"
#include "sPhenixStyle.C"

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

// note : use "ls *.root > file_list.txt" to create the list of the file in the folder, full directory in the file_list.txt
// note : set_folder_name = "folder_xxxx"
// note : server_name = "inttx"
void dNdeta_advanced(/*pair<double,double>beam_origin*/)
{
    
    TCanvas * c4 = new TCanvas("","",850,800);    
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

    TCanvas * c1 = new TCanvas("","",1000,800);
    c1 -> cd();

    
    
    // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/zero_magnet_Takashi_used";
    // string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_ideal_excludeR1500_100kEvent";
    // string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_survey_1_XYAlpha_Peek_3.32mm_excludeR1500_100kEvent";

    string mother_folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869";
    string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_survey_1_XYAlpha_Peek_3.32mm_excludeR20000_150kEvent_3HotCut";

    // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/new_DAC_Scan_0722/AllServer/DAC2";
    // string file_name = "beam_inttall-00023058-0000_event_base_ana_cluster_ideal_excludeR2000_100kEvent";

    system(Form("mkdir %s/folder_%s_advanced",mother_folder_directory.c_str(),file_name.c_str()));
    system(Form("mkdir %s/folder_%s_advanced/good_track",mother_folder_directory.c_str(),file_name.c_str()));

    pair<double,double> beam_origin = {-0,2};
    double temp_Y_align = 0.;
    double temp_X_align = -0.;

    double zvtx_hist_l = -500;
    double zvtx_hist_r = 500;

    int Nhit_cut = 20000;           // note : if (> Nhit_cut)          -> continue
    int Nhit_cutl = 400;          // note : if (< Nhit_cutl)         -> continue 
    int clu_size_cut = 4;           // note : if (> clu_size_cut)      -> continue
    double clu_sum_adc_cut = 31;    // note : if (< clu_sum_adc_cut)   -> continue
    int N_clu_cut = 15000;          // note : if (> N_clu_cut)         -> continue  unit number
    int N_clu_cutl = 500;           // note : if (< N_clu_cutl)        -> continue  unit number
    double phi_diff_cut = 3.5;      // note : if (< phi_diff_cut)      -> pass      unit degree
    double DCA_cut = 4;             // note : if (< DCA_cut)           -> pass      unit mm
    int zvtx_cal_require = 15;      // note : if (> zvtx_cal_require)  -> pass
    int zvtx_draw_requireL = 15;       
    int zvtx_draw_requireR = 40000;   // note : if ( zvtx_draw_requireL < event && event < zvtx_draw_requireR) -> pass
    double Integrate_portion = 0.4; // todo : it was 0.6826, try to require less event, as most of the tracklets are not that useful
    double Integrate_portion_final = 0.68;
    bool draw_event_display = false;
    int print_rate = 5;

    double mean_zvtx = -195.45; // note : unit : mm

    bool full_event = false;
    long long used_event = 3500;

    double dNdeta_upper_range = 100;

    int geo_mode_id = 1;
    string conversion_mode = (geo_mode_id == 0) ? "ideal" : "survey_1_XYAlpha_Peek";
    double peek = 3.32405;

    TFile * file_in = new TFile(Form("%s/%s.root",mother_folder_directory.c_str(),file_name.c_str()),"read");
    TTree * tree = (TTree *)file_in->Get("tree_clu");
    
    long long N_event = (full_event == true) ? tree -> GetEntries() : used_event;
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

    TFile * out_file = new TFile(Form("%s/folder_%s_advanced/INTT_dNdeta.root",mother_folder_directory.c_str(),file_name.c_str()),"RECREATE");

    int N_hits_out, N_cluster_inner_out, N_cluster_outer_out, ntrack_out;
    int eID_out;
    double out_xvtx, out_yvtx, out_zvtx, out_zvtxE;
    vector<double> out_inner_x;
    vector<double> out_inner_y;
    vector<double> out_inner_z;
    vector<double> out_inner_r;
    vector<double> out_inner_phi; 
    vector<double> out_inner_zE;
    vector<double> out_outer_x;
    vector<double> out_outer_y;
    vector<double> out_outer_z;
    vector<double> out_outer_r;
    vector<double> out_outer_phi; 
    vector<double> out_outer_zE;
    vector<double> out_eta;
    vector<double> out_eta_rchi2;

    TTree * tree_out =  new TTree ("tree_eta", "eta info.");
    tree_out -> Branch("eID",&eID_out);
    tree_out -> Branch("nhits",&N_hits_out);
    tree_out -> Branch("nclu_inner",&N_cluster_inner_out);
    tree_out -> Branch("nclu_outer",&N_cluster_outer_out);
    tree_out -> Branch("ntrack",&ntrack_out);
    tree_out -> Branch("xvtx",&out_xvtx);
    tree_out -> Branch("yvtx",&out_yvtx);
    tree_out -> Branch("zvtx",&out_zvtx);
    tree_out -> Branch("zvtxE",&out_zvtxE);
    
    tree_out -> Branch("inner_x",&out_inner_x); // note : inner
    tree_out -> Branch("inner_y",&out_inner_y);
    tree_out -> Branch("inner_z",&out_inner_z);
    tree_out -> Branch("inner_r",&out_inner_r);
    tree_out -> Branch("inner_phi",&out_inner_phi);
    tree_out -> Branch("inner_zE",&out_inner_zE); 
    tree_out -> Branch("outer_x",&out_outer_x); // note : outer
    tree_out -> Branch("outer_y",&out_outer_y);
    tree_out -> Branch("outer_z",&out_outer_z);
    tree_out -> Branch("outer_r",&out_outer_r);
    tree_out -> Branch("outer_phi",&out_outer_phi);
    tree_out -> Branch("outer_zE",&out_outer_zE); 
    tree_out -> Branch("eta",&out_eta); // note : track
    tree_out -> Branch("eta_rchi",&out_eta_rchi2);

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

    TH1F * dNdeta_hist = new TH1F("","",29,-2.9,2.9);
    // TH1F * dNdeta_hist = new TH1F("","",145,-2.9,2.9);
    dNdeta_hist -> SetMarkerStyle(20);
    dNdeta_hist -> SetMarkerSize(0.8);
    dNdeta_hist -> SetMarkerColor(TColor::GetColor("#1A3947"));
    dNdeta_hist -> SetLineColor(TColor::GetColor("#1A3947"));
    dNdeta_hist -> SetLineWidth(2);
    dNdeta_hist -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
    dNdeta_hist -> GetXaxis() -> SetTitle("#eta");
    dNdeta_hist -> GetYaxis() -> SetRangeUser(0,50);
    dNdeta_hist -> SetTitleSize(0.06, "X");
    dNdeta_hist -> SetTitleSize(0.06, "Y");
	dNdeta_hist -> GetXaxis() -> SetTitleOffset (0.71);
    dNdeta_hist -> GetYaxis() -> SetTitleOffset (1.1);
	dNdeta_hist -> GetXaxis() -> CenterTitle(true);
    dNdeta_hist -> GetYaxis() -> CenterTitle(true);

    // TH1F * dNdeta_2P_inner_hist = new TH1F("","",29,-2.9,2.9);
    TH1F * dNdeta_2P_inner_hist = new TH1F("","",145,-2.9,2.9);
    dNdeta_2P_inner_hist -> SetMarkerStyle(20);
    dNdeta_2P_inner_hist -> SetMarkerSize(0.8);
    dNdeta_2P_inner_hist -> SetMarkerColor(TColor::GetColor("#1A3947"));
    dNdeta_2P_inner_hist -> SetLineColor(TColor::GetColor("#1A3947"));
    dNdeta_2P_inner_hist -> SetLineWidth(2);
    dNdeta_2P_inner_hist -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
    dNdeta_2P_inner_hist -> GetXaxis() -> SetTitle("#eta");
    dNdeta_2P_inner_hist -> GetYaxis() -> SetRangeUser(0,50);
    dNdeta_2P_inner_hist -> SetTitleSize(0.06, "X");
    dNdeta_2P_inner_hist -> SetTitleSize(0.06, "Y");
	dNdeta_2P_inner_hist -> GetXaxis() -> SetTitleOffset (0.71);
    dNdeta_2P_inner_hist -> GetYaxis() -> SetTitleOffset (1.1);
	dNdeta_2P_inner_hist -> GetXaxis() -> CenterTitle(true);
    dNdeta_2P_inner_hist -> GetYaxis() -> CenterTitle(true);

    TH1F * dNdeta_2P_outer_hist = new TH1F("","",29,-2.9,2.9);
    dNdeta_2P_outer_hist -> SetMarkerStyle(20);
    dNdeta_2P_outer_hist -> SetMarkerSize(0.8);
    dNdeta_2P_outer_hist -> SetMarkerColor(TColor::GetColor("#1A3947"));
    dNdeta_2P_outer_hist -> SetLineColor(TColor::GetColor("#1A3947"));
    dNdeta_2P_outer_hist -> SetLineWidth(2);
    dNdeta_2P_outer_hist -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
    dNdeta_2P_outer_hist -> GetXaxis() -> SetTitle("#eta");
    dNdeta_2P_outer_hist -> GetYaxis() -> SetRangeUser(0,50);
    dNdeta_2P_outer_hist -> SetTitleSize(0.06, "X");
    dNdeta_2P_outer_hist -> SetTitleSize(0.06, "Y");
	dNdeta_2P_outer_hist -> GetXaxis() -> SetTitleOffset (0.71);
    dNdeta_2P_outer_hist -> GetYaxis() -> SetTitleOffset (1.1);
	dNdeta_2P_outer_hist -> GetXaxis() -> CenterTitle(true);
    dNdeta_2P_outer_hist -> GetYaxis() -> CenterTitle(true);

    double N_good_event = 0;

    TH2F * Good_inner_outer_pos_xy_nzB = new TH2F("","Good_inner_outer_pos_xy_nzB",360,-150,150,360,-150,150);
    Good_inner_outer_pos_xy_nzB -> SetStats(0);
    Good_inner_outer_pos_xy_nzB -> GetXaxis() -> SetTitle("X axis");
    Good_inner_outer_pos_xy_nzB -> GetYaxis() -> SetTitle("Y axis");

    TH2F * Good_inner_outer_pos_xy_nzA = new TH2F("","Good_inner_outer_pos_xy_nzA",360,-150,150,360,-150,150);
    Good_inner_outer_pos_xy_nzA -> SetStats(0);
    Good_inner_outer_pos_xy_nzA -> GetXaxis() -> SetTitle("X axis");
    Good_inner_outer_pos_xy_nzA -> GetYaxis() -> SetTitle("Y axis");

    TH2F * Good_inner_outer_pos_xy_pzA = new TH2F("","Good_inner_outer_pos_xy_pzA",360,-150,150,360,-150,150);
    Good_inner_outer_pos_xy_pzA -> SetStats(0);
    Good_inner_outer_pos_xy_pzA -> GetXaxis() -> SetTitle("X axis");
    Good_inner_outer_pos_xy_pzA -> GetYaxis() -> SetTitle("Y axis");

    TH2F * Good_inner_outer_pos_xy_pzB = new TH2F("","Good_inner_outer_pos_xy_pzB",360,-150,150,360,-150,150);
    Good_inner_outer_pos_xy_pzB -> SetStats(0);
    Good_inner_outer_pos_xy_pzB -> GetXaxis() -> SetTitle("X axis");
    Good_inner_outer_pos_xy_pzB -> GetYaxis() -> SetTitle("Y axis");

    TH2F * Good_track_space = new TH2F("","Good_track_space",200,-300,300,200,-300,300);
    Good_track_space -> SetStats(0);
    Good_track_space -> GetXaxis() -> SetTitle("X axis");
    Good_track_space -> GetYaxis() -> SetTitle("Y axis");

    // dNdeta_hist -> GetXaxis() -> SetLimits(-10,10);
    // dNdeta_hist -> GetXaxis() -> SetNdivisions  (505);

    TF1 * zvtx_finder = new TF1("zvtx_finder","pol0",-1,3000); 

    
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
    TH1F * avg_event_zvtx = new TH1F("","avg_event_zvtx",125,zvtx_hist_l,zvtx_hist_r);
    avg_event_zvtx -> GetXaxis() -> SetTitle("Z vertex position (mm)");
    avg_event_zvtx -> GetYaxis() -> SetTitle("entry");

    TH1F * zvtx_evt_width = new TH1F("","zvtx_evt_width",150,0,800);
    zvtx_evt_width -> GetXaxis() -> SetTitle(" mm ");
    zvtx_evt_width -> GetYaxis() -> SetTitle("entry");

    TH2F * zvtx_evt_fitError_corre = new TH2F("","zvtx_evt_fitError_corre",200,0,10000,200,0,20);
    zvtx_evt_fitError_corre -> GetXaxis() -> SetTitle(" # of clusters ");
    zvtx_evt_fitError_corre -> GetYaxis() -> SetTitle(" #pm mm ");

    TH2F * zvtx_evt_width_corre = new TH2F("","zvtx_evt_width_corre",200,0,10000,200,0,300);
    zvtx_evt_width_corre -> GetXaxis() -> SetTitle(" # of clusters ");
    zvtx_evt_width_corre -> GetYaxis() -> SetTitle(" mm ");

    TH2F * zvtx_evt_nclu_corre = new TH2F("","zvtx_evt_nclu_corre",200,0,10000,200,-500,500);
    zvtx_evt_nclu_corre -> GetXaxis() -> SetTitle(" # of clusters ");
    zvtx_evt_nclu_corre -> GetYaxis() -> SetTitle(" zvtx [mm] ");

    TH1F * width_density = new TH1F("","width_density",200,0,1); // note : N good hits / width
    width_density -> GetXaxis() -> SetTitle(" N good zvtx / width ");
    width_density -> GetYaxis() -> SetTitle(" Entry ");
    



    int N_event_pass_number = 0;
    
    if (draw_event_display) c2 -> Print(Form("%s/folder_%s_advanced/temp_event_display.pdf(",mother_folder_directory.c_str(),file_name.c_str()));

    for (int event_i = 0; event_i < N_event; event_i++)
    {
        if (event_i % 1000 == 0) cout<<"code running... "<<event_i<<endl;

        tree -> GetEntry(event_i);
        unsigned int length = column_vec -> size();

        if (event_i == 13) cout<<"test, eID : "<<event_i<<" Nhits "<<N_hits<<endl;

        if (N_hits > Nhit_cut) continue;
        if (N_hits < Nhit_cutl) continue;

        N_event_pass_number += 1;

        if (N_cluster_inner == 0 || N_cluster_outer == 0) continue;
        if (N_cluster_inner == -1 || N_cluster_outer == -1) continue;
        if ((N_cluster_inner + N_cluster_outer) < zvtx_cal_require) continue;
        if (N_cluster_inner < 30) continue;
        if (N_cluster_outer < 30) continue;
        

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
            // if (layer_vec -> at(clu_i) == 0 && phi_vec -> at(clu_i) > 265 && phi_vec -> at(clu_i) < 270) continue;

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
            

            if (layer_vec -> at(clu_i) == 0) // note : inner
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
            
            if (layer_vec -> at(clu_i) == 1) // note : outer
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

        int inner_1_check = 0;
        int inner_2_check = 0;
        int inner_3_check = 0;
        int inner_4_check = 0;
        for (int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++) {
            if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 0 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 90)    inner_1_check = 1;
            if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 90 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 180)  inner_2_check = 1;
            if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 180 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 270) inner_3_check = 1;
            if (temp_sPH_inner_nocolumn_vec[inner_i].phi >= 270 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 360) inner_4_check = 1;
        }

        int outer_1_check = 0;
        int outer_2_check = 0;
        int outer_3_check = 0;
        int outer_4_check = 0;
        for (int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++) {
            if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 0 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 90)    outer_1_check = 1;
            if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 90 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 180)  outer_2_check = 1;
            if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 180 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 270) outer_3_check = 1;
            if (temp_sPH_outer_nocolumn_vec[outer_i].phi >= 270 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 360) outer_4_check = 1;
        }

        N_cluster_outer_pass -> Fill(temp_sPH_outer_nocolumn_vec.size());
        N_cluster_inner_pass -> Fill(temp_sPH_inner_nocolumn_vec.size());
        N_cluster_correlation -> Fill( temp_sPH_inner_nocolumn_vec.size(), temp_sPH_outer_nocolumn_vec.size() );

        if ((temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size()) > N_clu_cut || (temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size()) < N_clu_cutl)
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
            continue;
        }
        
        vector<float> N_comb; N_comb.clear();
        vector<float> N_comb_e; N_comb_e.clear();
        vector<float> z_mid; z_mid.clear(); 
        vector<float> z_range; z_range.clear();

        // note : try to make sure that the clusters not to be used twice or more 
        vector<int> used_outer_check(temp_sPH_outer_nocolumn_vec.size(),0);

        for ( int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++ )
        {
            
            for ( int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++ )
            {
                bool DCA_tag = false;
                if (used_outer_check[outer_i] == 1) continue; // note : this outer cluster was already used, skip the trial of this combination
                
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

                if (DCA_info_vec[0] != fabs(DCA_sign) && fabs( DCA_info_vec[0] - fabs(DCA_sign) ) > 0.1 ){
                    cout<<"different DCA : "<<DCA_info_vec[0]<<" "<<DCA_sign<<" diff : "<<DCA_info_vec[0] - fabs(DCA_sign)<<endl;}

                if (fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi) < phi_diff_cut)
                {
                    if (DCA_info_vec[0] < DCA_cut){

                        used_outer_check[outer_i] = 1; //note : this outer cluster was already used!

                        pair<double,double> z_range_info = Get_possible_zvtx( 
                            get_radius(beam_origin.first,beam_origin.second), 
                            {get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y), temp_sPH_inner_nocolumn_vec[inner_i].z}, // note : unsign radius
                            {get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y), temp_sPH_outer_nocolumn_vec[outer_i].z}  // note : unsign radius
                        );
                        
                        N_comb.push_back(inner_i);
                        N_comb_e.push_back(0);
                        z_mid.push_back(z_range_info.first);
                        z_range.push_back(z_range_info.second);

                        DCA_tag = true;
                    }
                    

                    DCA_point -> Fill( DCA_info_vec[1], DCA_info_vec[2] );
                    angle_correlation -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi,temp_sPH_outer_nocolumn_vec[outer_i].phi);
                    z_pos_diff -> Fill( temp_sPH_inner_nocolumn_vec[inner_i].z - temp_sPH_outer_nocolumn_vec[outer_i].z );
                    z_pos_diff_angle_diff -> Fill( temp_sPH_inner_nocolumn_vec[inner_i].z - temp_sPH_outer_nocolumn_vec[outer_i].z, temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi );
                    Nhits_good -> Fill(N_hits);
                    z_pos_inner -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].z);
                    z_pos_outer -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].z);
                    z_pos_inner_outer -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].z, temp_sPH_outer_nocolumn_vec[outer_i].z);
                    // DCA_distance_inner_phi -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi, (temp_sPH_inner_nocolumn_vec[inner_i].phi > 90 && temp_sPH_inner_nocolumn_vec[inner_i].phi < 270) ? DCA_sign * -1 : DCA_sign );
                    // DCA_distance_outer_phi -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].phi, (temp_sPH_outer_nocolumn_vec[outer_i].phi > 90 && temp_sPH_outer_nocolumn_vec[outer_i].phi < 270) ? DCA_sign * -1 : DCA_sign );
                    DCA_distance_inner_phi -> Fill(temp_sPH_inner_nocolumn_vec[inner_i].phi, DCA_sign );
                    DCA_distance_outer_phi -> Fill(temp_sPH_outer_nocolumn_vec[outer_i].phi, DCA_sign );

                    if(DCA_tag == true) break; // note : since this combination (one inner cluster, one outer cluster) satisfied the reuqiremet, no reason to ask this inner cluster try with other outer clusters

                    // cout<<"good comb : "<<fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi)<<" radius in : "<<get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y)<<" radius out : "<<get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y)<<endl;
                } // note : end of if 
                    

            } // note : end of outer loop
        } // note : end of inner loop

        // cout<<"test tag 0"<<endl;
        TGraphErrors * z_range_gr;
        vector<double> effi_N_comb; effi_N_comb.clear();
        vector<double> effi_z_mid; effi_z_mid.clear();
        vector<double> effi_N_comb_e; effi_N_comb_e.clear();
        vector<double> effi_z_range; effi_z_range.clear();
        // cout<<"test tag 1"<<endl;
        if (N_comb.size() > zvtx_cal_require)
        {   
            temp_event_zvtx_info = sigmaEff_avg(z_mid,Integrate_portion);
            
            for (int track_i = 0; track_i < N_comb.size(); track_i++) {
                if (temp_event_zvtx_info[1] <= z_mid[track_i] && z_mid[track_i] <= temp_event_zvtx_info[2]) {
                    effi_N_comb.push_back(N_comb[track_i]);
                    effi_z_mid.push_back(z_mid[track_i]);
                    effi_N_comb_e.push_back(N_comb_e[track_i]);
                    effi_z_range.push_back(z_range[track_i]);
                }
            }

            z_range_gr = new TGraphErrors(effi_N_comb.size(),&effi_N_comb[0],&effi_z_mid[0],&effi_N_comb_e[0],&effi_z_range[0]);
            // z_range_gr = new TGraph(effi_N_comb.size(),&effi_N_comb[0],&effi_z_mid[0]);
            z_range_gr -> Fit(zvtx_finder,"NQ","",0,N_comb[N_comb.size() - 1] * 0.7); // note : not fit all the combination
            
            
            // avg_event_zvtx -> Fill(zvtx_finder -> GetParameter(0));
            zvtx_evt_width -> Fill(fabs( zvtx_finder -> GetParError(0)));
            zvtx_evt_fitError_corre -> Fill(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(), fabs( zvtx_finder -> GetParError(0)));
            zvtx_evt_width_corre -> Fill(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(), fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]));
            width_density -> Fill( effi_N_comb.size() / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) );
            if ( ( effi_N_comb.size() / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) ) > 0.3 ){
                zvtx_evt_nclu_corre -> Fill(temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size(), zvtx_finder -> GetParameter(0));
                avg_event_zvtx -> Fill(zvtx_finder -> GetParameter(0));
                avg_event_zvtx_vec.push_back(zvtx_finder -> GetParameter(0));
            }

            if (fabs(zvtx_finder -> GetParameter(0) - mean_zvtx) < 20 && ( effi_N_comb.size() / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) ) > 0.3 ) {
                // note : now we have a z vertex. Therefore, let's find new tracks with 

                vector<int> used_outer_check_eta(temp_sPH_outer_nocolumn_vec.size(),0);

                for ( int inner_i = 0; inner_i < temp_sPH_inner_nocolumn_vec.size(); inner_i++ ) {
                    for ( int outer_i = 0; outer_i < temp_sPH_outer_nocolumn_vec.size(); outer_i++ ) {
                        
                        if (used_outer_check_eta[outer_i] == 1) continue;
                        if (fabs(temp_sPH_inner_nocolumn_vec[inner_i].phi - temp_sPH_outer_nocolumn_vec[outer_i].phi) > phi_diff_cut) continue;

                        vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
                            temp_sPH_inner_nocolumn_vec[inner_i].x, temp_sPH_inner_nocolumn_vec[inner_i].y,
                            temp_sPH_outer_nocolumn_vec[outer_i].x, temp_sPH_outer_nocolumn_vec[outer_i].y,
                            beam_origin.first, beam_origin.second
                        );

                        if (DCA_info_vec[0] > DCA_cut) continue;
                        
                        pair<double,double> Get_eta_pair = Get_eta(
                            {get_radius(beam_origin.first,beam_origin.second),zvtx_finder -> GetParameter(0),zvtx_finder -> GetParError(0)},
                            {get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x,temp_sPH_inner_nocolumn_vec[inner_i].y),temp_sPH_inner_nocolumn_vec[inner_i].z},
                            {get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x,temp_sPH_outer_nocolumn_vec[outer_i].y),temp_sPH_outer_nocolumn_vec[outer_i].z}
                        );
                        

                        // note : following cut means that the three points have good connection
                        if (fabs(Get_eta_pair.first) > 7) continue; // todo the chi2/NDF cut is here

                        eID_out = event_i;
                        N_hits_out = N_hits;
                        N_cluster_inner_out = temp_sPH_inner_nocolumn_vec.size();
                        N_cluster_outer_out = temp_sPH_outer_nocolumn_vec.size();
                        out_xvtx = beam_origin.first;
                        out_yvtx = beam_origin.second;
                        out_zvtx = zvtx_finder -> GetParameter(0);
                        out_zvtxE = zvtx_finder -> GetParError(0);

                        out_inner_x.push_back( temp_sPH_inner_nocolumn_vec[inner_i].x ); // note : inner
                        out_inner_y.push_back( temp_sPH_inner_nocolumn_vec[inner_i].y );
                        out_inner_z.push_back( temp_sPH_inner_nocolumn_vec[inner_i].z );
                        out_inner_r.push_back( get_radius(temp_sPH_inner_nocolumn_vec[inner_i].x,temp_sPH_inner_nocolumn_vec[inner_i].y) );
                        out_inner_phi.push_back( temp_sPH_inner_nocolumn_vec[inner_i].phi );
                        out_inner_zE.push_back( ( fabs( temp_sPH_inner_nocolumn_vec[inner_i].z ) < 130 ) ? 8. : 10. );
                        out_outer_x.push_back( temp_sPH_outer_nocolumn_vec[outer_i].x ); // note : outer
                        out_outer_y.push_back( temp_sPH_outer_nocolumn_vec[outer_i].y );
                        out_outer_z.push_back( temp_sPH_outer_nocolumn_vec[outer_i].z );
                        out_outer_r.push_back( get_radius(temp_sPH_outer_nocolumn_vec[outer_i].x,temp_sPH_outer_nocolumn_vec[outer_i].y) );
                        out_outer_phi.push_back( temp_sPH_outer_nocolumn_vec[outer_i].phi );
                        out_outer_zE.push_back( ( fabs( temp_sPH_outer_nocolumn_vec[outer_i].z ) < 130 ) ? 8. : 10. );
                        out_eta.push_back( Get_eta_pair.second );
                        out_eta_rchi2.push_back( Get_eta_pair.first );
                        
                        dNdeta_hist -> Fill(Get_eta_pair.second);
                        used_outer_check_eta[outer_i] = 1;
                        break;
                    } // note : for loop outer
                } // note : for loop inner 

                ntrack_out = out_eta.size();
                if (out_eta.size() > 0) {
                    tree_out -> Fill();
                    N_good_event += 1;
                }   

                out_inner_x.clear();
                out_inner_y.clear();
                out_inner_z.clear();
                out_inner_r.clear();
                out_inner_phi.clear();
                out_inner_zE.clear();
                out_outer_x.clear();
                out_outer_y.clear();
                out_outer_z.clear();
                out_outer_r.clear();
                out_outer_phi.clear();
                out_outer_zE.clear();
                out_eta.clear();
                out_eta_rchi2.clear();
            } // note : if zvtx within range

            

        }
        // cout<<"test tag 2"<<endl;
        
            

        if ( zvtx_draw_requireL < N_comb.size() && draw_event_display == true && N_comb.size() > zvtx_cal_require)
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
            draw_text -> DrawLatex(0.2, 0.85, Form("eID : %i, Total event hit : %i, innter Ncluster : %i, outer Ncluster : %i",event_i,N_hits,temp_sPH_inner_nocolumn_vec.size(),temp_sPH_outer_nocolumn_vec.size()));
        
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
            TGraphErrors * z_range_gr_draw = new TGraphErrors(N_comb.size(),&N_comb[0],&z_mid[0],&N_comb_e[0],&z_range[0]);
            z_range_gr_draw -> GetYaxis() -> SetRangeUser(-650,650);
            z_range_gr_draw -> SetMarkerStyle(20);
            z_range_gr_draw -> Draw("ap");
            zvtx_finder -> Draw("lsame");
            draw_text -> DrawLatex(0.2, 0.82, Form("Event Zvtx %.2f mm, error : #pm%.2f", zvtx_finder -> GetParameter(0), zvtx_finder -> GetParError(0)));
            draw_text -> DrawLatex(0.2, 0.78, Form("Width density : %.2f", ( effi_N_comb.size() / fabs(temp_event_zvtx_info[2] - temp_event_zvtx_info[1]) )));
            draw_text -> DrawLatex(0.2, 0.74, Form("Width : %.2f to %.2f mm", temp_event_zvtx_info[2] , temp_event_zvtx_info[1]));

            

            effi_sig_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),temp_event_zvtx_info[1],z_range_gr_draw->GetXaxis()->GetXmax(),temp_event_zvtx_info[1]);
            effi_sig_range_line -> DrawLine(z_range_gr_draw->GetXaxis()->GetXmin(),temp_event_zvtx_info[2],z_range_gr_draw->GetXaxis()->GetXmax(),temp_event_zvtx_info[2]);


            // temp_event_zvtx -> Draw("hist");
            // // zvtx_fitting -> Draw("lsame");
            // effi_sig_range_line -> DrawLine(temp_event_zvtx_info[1],0,temp_event_zvtx_info[1],temp_event_zvtx -> GetMaximum()*1.05);
            // effi_sig_range_line -> DrawLine(temp_event_zvtx_info[2],0,temp_event_zvtx_info[2],temp_event_zvtx -> GetMaximum()*1.05);
            // draw_text -> DrawLatex(0.2, 0.85, Form("eID : %i, Total event hit : %i, innter Ncluster : %i, outer Ncluster : %i",event_i,N_hits,temp_sPH_inner_nocolumn_vec.size(),temp_sPH_outer_nocolumn_vec.size()));
            // // draw_text -> DrawLatex(0.2, 0.84, Form("Gaus fit mean : %.3f mm",zvtx_fitting -> GetParameter(1)));
            // draw_text -> DrawLatex(0.2, 0.82, Form("EffiSig min : %.2f mm, max : %.2f mm",temp_event_zvtx_info[1],temp_event_zvtx_info[2]));
            // draw_text -> DrawLatex(0.2, 0.79, Form("EffiSig avg : %.2f mm",temp_event_zvtx_info[0]));

            if(draw_event_display && (event_i % print_rate) == 0){c2 -> Print(Form("%s/folder_%s_advanced/temp_event_display.pdf",mother_folder_directory.c_str(),file_name.c_str()));}
            pad_xy -> Clear();
            pad_rz -> Clear();
            pad_z  -> Clear();

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

    if (draw_event_display) {c2 -> Print(Form("%s/folder_%s_advanced/temp_event_display.pdf)",mother_folder_directory.c_str(),file_name.c_str()));}
    c2 -> Clear();
    c1 -> Clear();

    tree_out->SetDirectory(out_file);
    tree_out -> Write("", TObject::kOverwrite);
    

    c1 -> cd();
    vector<float> avg_event_zvtx_info = sigmaEff_avg(avg_event_zvtx_vec,Integrate_portion_final);
    avg_event_zvtx -> Draw("hist");
    effi_sig_range_line -> DrawLine(avg_event_zvtx_info[1],0,avg_event_zvtx_info[1],avg_event_zvtx -> GetMaximum()*1.05);
    effi_sig_range_line -> DrawLine(avg_event_zvtx_info[2],0,avg_event_zvtx_info[2],avg_event_zvtx -> GetMaximum()*1.05);    
    draw_text -> DrawLatex(0.15, 0.84, Form("EffiSig min : %.2f mm, max : %.2f mm",avg_event_zvtx_info[1],avg_event_zvtx_info[2]));
    draw_text -> DrawLatex(0.15, 0.81, Form("EffiSig avg : %.2f mm",avg_event_zvtx_info[0]));
    c1 -> Print(Form("%s/folder_%s_advanced/avg_event_zvtx.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    width_density -> Draw("hist"); 
    c1 -> Print(Form("%s/folder_%s_advanced/width_density.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    zvtx_evt_width -> Draw("hist"); 
    c1 -> Print(Form("%s/folder_%s_advanced/zvtx_evt_width.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    zvtx_evt_fitError_corre -> Draw("colz0"); 
    c1 -> Print(Form("%s/folder_%s_advanced/zvtx_evt_fitError_corre.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    zvtx_evt_nclu_corre -> Draw("colz0"); 
    c1 -> Print(Form("%s/folder_%s_advanced/zvtx_evt_nclu_corre.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    zvtx_evt_width_corre -> Draw("colz0"); 
    c1 -> Print(Form("%s/folder_%s_advanced/zvtx_evt_width_corre.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    N_cluster_inner_pass -> Draw("hist"); 
    c1 -> Print(Form("%s/folder_%s_advanced/N_cluster_inner_pass.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    N_cluster_outer_pass -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_advanced/N_cluster_outer_pass.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    N_cluster_correlation -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/N_cluster_correlation.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    DCA_point -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/DCA_point_X%.1fY%.1f_.pdf",mother_folder_directory.c_str(),file_name.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();

    DCA_distance_inner_phi -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/DCA_distance_inner_phi_X%.1fY%.1f_.pdf",mother_folder_directory.c_str(),file_name.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();

    DCA_distance_outer_phi -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/DCA_distance_outer_phi_X%.1fY%.1f_.pdf",mother_folder_directory.c_str(),file_name.c_str(),beam_origin.first,beam_origin.second));
    c1 -> Clear();

    z_pos_inner_outer -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/z_pos_inner_outer.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    z_pos_inner -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_advanced/z_pos_inner.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    z_pos_outer -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_advanced/z_pos_outer.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    Nhits_good -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_advanced/Nhits_good.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    angle_correlation -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/angle_correlation.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    z_pos_diff -> Draw("hist");
    c1 -> Print(Form("%s/folder_%s_advanced/z_pos_diff.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    z_pos_diff_angle_diff -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/z_pos_diff_angle_diff.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    inner_pos_xy -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/inner_pos_xy.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    outer_pos_xy -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/outer_pos_xy.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    inner_outer_pos_xy -> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/inner_outer_pos_xy.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    Good_inner_outer_pos_xy_nzB-> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/Good_inner_outer_pos_xy_nzB.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();
    Good_inner_outer_pos_xy_nzA-> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/Good_inner_outer_pos_xy_nzA.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();
    Good_inner_outer_pos_xy_pzA-> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/Good_inner_outer_pos_xy_pzA.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();
    Good_inner_outer_pos_xy_pzB-> Draw("colz0");
    c1 -> Print(Form("%s/folder_%s_advanced/Good_inner_outer_pos_xy_pzB.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    Good_track_space -> Draw("");
    for (int i = 0; i < good_tracklet_rz.size(); i++){
        track_line -> DrawLine(good_tracklet_rz[i][1],good_tracklet_rz[i][0],good_tracklet_rz[i][3],good_tracklet_rz[i][2]);
    }
    c1 -> Print(Form("%s/folder_%s_advanced/good_tracklet_rz.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    c1 -> Clear();

    //   note : normalized
    dNdeta_hist -> Scale(1./double(N_good_event));    
    dNdeta_hist -> GetYaxis() -> SetRangeUser(0,dNdeta_upper_range);
    dNdeta_2P_inner_hist -> Scale(1./double(N_good_event));
    dNdeta_2P_inner_hist -> GetYaxis() -> SetRangeUser(0,dNdeta_upper_range);
    dNdeta_2P_outer_hist -> Scale(1./double(N_good_event));
    dNdeta_2P_outer_hist -> GetYaxis() -> SetRangeUser(0,dNdeta_upper_range);

    cout<<" final N good event : "<<N_good_event<<" N event with correct hit : "<<N_event_pass_number<<endl;


    TCanvas * c3 = new TCanvas("c3","c3",900,800); c3 -> cd();
    TPad *pad_obj = new TPad(Form("pad_obj"), "", 0.0, 0.0, 1.0, 1.0);
    Characterize_Pad(pad_obj, 0.18,  0.1,  0.1,  0.12, 0, 0);
    pad_obj -> SetTicks(1,1);
    pad_obj -> Draw();
    pad_obj -> cd();

    SetsPhenixStyle();
    dNdeta_hist -> Draw("ep");
    

    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->DrawLatex(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Work-in-progress");
    ltx->DrawLatex(0.48, 0.835, "Run 20869");
    ltx->DrawLatex(0.48, 0.785, "Au+Au #sqrt{s_{NN}} = 200 GeV");
    c3 -> Print(Form("%s/folder_%s_advanced/dNdeta.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    pad_obj -> Clear();

    pad_obj -> cd();
    dNdeta_2P_inner_hist -> Draw("ep");
    ltx->DrawLatex(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Work-in-progress");
    ltx->DrawLatex(0.38, 0.835, "Run 20869");
    ltx->DrawLatex(0.38, 0.785, "Au+Au #sqrt{s_{NN}} = 200 GeV");
    ltx->DrawLatex(0.38, 0.735, "Tracklet : vertex + inner cluster");
    c3 -> Print(Form("%s/folder_%s_advanced/dNdeta_2P_inner.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    pad_obj -> Clear();

    pad_obj -> cd();
    dNdeta_2P_outer_hist -> Draw("ep");
    ltx->DrawLatex(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Work-in-progress");
    ltx->DrawLatex(0.38, 0.835, "Run 20869");
    ltx->DrawLatex(0.38, 0.785, "Au+Au #sqrt{s_{NN}} = 200 GeV");
    ltx->DrawLatex(0.38, 0.735, "Tracklet : vertex + outer cluster");
    c3 -> Print(Form("%s/folder_%s_advanced/dNdeta_2P_outer.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    pad_obj -> Clear();




}
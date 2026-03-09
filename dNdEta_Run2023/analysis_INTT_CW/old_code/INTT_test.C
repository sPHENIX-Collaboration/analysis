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

// vector<vector<int>> adc_setting_run = {	
//     {15, 30, 60, 90, 120, 150, 180, 210, 240}
//     // {15, 30, 50, 70, 90, 110, 130, 150,170}
//     // {8  , 12 , 16 , 20 , 24 , 28 , 32 , 36 },
//     // {28 , 32 , 36 , 40 , 44 , 48 , 52 , 56 },
//     // {48 , 52 , 56 , 60 , 64 , 68 , 72 , 76 }, // note : 3
//     // {68 , 72 , 76 , 80 , 84 , 88 , 92 , 96 }, // note : 4
//     // {88 , 92 , 96 , 100, 104, 108, 112, 116}, // note : 5
//     // {108, 112, 116, 120, 124, 128, 132, 136}, // note : 6
//     // {128, 132, 136, 140, 144, 148, 152, 156}, // note : 7
//     // {148, 152, 156, 160, 164, 168, 172, 176},
//     // {168, 172, 176, 180, 184, 188, 192, 196},
//     // {188, 192, 196, 200, 204, 208, 212, 216}
// };

// TString color_code_2[8] = {"#CC768D","#19768D","#DDA573","#009193","#6E9193","#941100","#A08144","#517E66"};

// struct full_hit_info {
//     int FC;
//     int chip_id;
//     int chan_id;
//     int adc;
// };


// struct ladder_info {
//     int FC;
//     TString Port;
//     int ROC;
//     int Direction; // note : 0 : south, 1 : north 
// };

// struct z_str {
//     int nclu_inner;
//     int nclu_outer;
//     double zvtx;
//     double zvtxE;
//     double rangeL;
//     double rangeR;
//     int N_good;
//     double width_density;
// };

// double get_radius(double x, double y)
// {
//     return sqrt(pow(x,2)+pow(y,2));
// }

// double get_radius_sign(double x, double y)
// {
//     double phi = ((y) < 0) ? atan2((y),(x)) * (180./TMath::Pi()) + 360 : atan2((y),(x)) * (180./TMath::Pi());
    
//     return (phi > 180) ? sqrt(pow(x,2)+pow(y,2)) * -1 : sqrt(pow(x,2)+pow(y,2)); 
// }

// void Characterize_Pad (TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0)
// {
// 	if (setgrid_bool == true) {pad -> SetGrid (1, 1);}
// 	pad -> SetLeftMargin   (left);
// 	pad -> SetRightMargin  (right);
// 	pad -> SetTopMargin    (top);
// 	pad -> SetBottomMargin (bottom);
//     pad -> SetTicks(1,1);
//     if (set_logY == true)
//     {
//         pad -> SetLogy (1);
//     }
	
// }

// std::vector<double> calculateDistanceAndClosestPoint(double x1, double y1, double x2, double y2, double target_x, double target_y) {
    
//     if (x1 != x2)
//     {
//         // Calculate the slope and intercept of the line passing through (x1, y1) and (x2, y2)
//         double a = (y2 - y1) / (x2 - x1);
//         double b = y1 - a * x1;

//         // cout<<"slope : y="<<a<<"x+"<<b<<endl;
        
//         // Calculate the closest distance from (target_x, target_y) to the line y = ax + b
//         double closest_distance = std::abs(a * target_x - target_y + b) / std::sqrt(a * a + 1);

//         // Calculate the coordinates of the closest point (Xc, Yc) on the line y = ax + b
//         double Xc = (target_x + a * target_y - a * b) / (a * a + 1);
//         double Yc = a * Xc + b;

//         return { closest_distance, Xc, Yc };
//     }
//     else 
//     {
//         double closest_distance = std::abs(x1 - target_x);
//         double Xc = x1;
//         double Yc = target_y;

//         return { closest_distance, Xc, Yc };
//     }
    
    
// }

// double get_z_vertex(clu_info inner_clu, clu_info outer_clu, double target_x, double target_y)
// {
//     // note : x = z, 
//     // note : y = radius
//     double inner_clu_r = sqrt(pow(inner_clu.x,2)+ pow(inner_clu.y,2));
//     double outer_clu_r = sqrt(pow(outer_clu.x,2)+ pow(outer_clu.y,2));
//     double target_r    = sqrt(pow(target_x,2)   + pow(target_y,2));

//     // Use the slope equation (y = ax + b) to calculate the x-coordinate for the target y
//     if ( fabs(outer_clu.z - inner_clu.z) < 0.00001 ){
//         return outer_clu.z;
//     }
//     else {
//         double slope = (outer_clu_r - inner_clu_r) / (outer_clu.z - inner_clu.z);
//         double yIntercept = inner_clu_r - slope * inner_clu.z;
//         double xCoordinate = (target_r - yIntercept) / slope;
//         return xCoordinate;
//     }
    
// }

// // Function to calculate the angle between two vectors in degrees using the cross product
// double calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY) {
//     // Calculate the vectors vector_1 (point_1 to point_2) and vector_2 (point_1 to target)
//     double vector1X = x2 - x1;
//     double vector1Y = y2 - y1;

//     double vector2X = targetX - x1;
//     double vector2Y = targetY - y1;

//     // Calculate the cross product of vector_1 and vector_2 (z-component)
//     double crossProduct = vector1X * vector2Y - vector1Y * vector2X;
    
//     // cout<<" crossProduct : "<<crossProduct<<endl;

//     // Calculate the magnitudes of vector_1 and vector_2
//     double magnitude1 = std::sqrt(vector1X * vector1X + vector1Y * vector1Y);
//     double magnitude2 = std::sqrt(vector2X * vector2X + vector2Y * vector2Y);

//     // Calculate the angle in radians using the inverse tangent of the cross product and dot product
//     double dotProduct = vector1X * vector2X + vector1Y * vector2Y;

//     double angleInRadians = std::atan2(std::abs(crossProduct), dotProduct);
//     // Convert the angle from radians to degrees and return it
//     double angleInDegrees = angleInRadians * 180.0 / M_PI;
    
//     double angleInRadians_new = std::asin( crossProduct/(magnitude1*magnitude2) );
//     double angleInDegrees_new = angleInRadians_new * 180.0 / M_PI;
    
//     // cout<<"angle : "<<angleInDegrees_new<<endl;

//     double DCA_distance = sin(angleInRadians_new) * magnitude2;

//     return DCA_distance;
// }

// double grEY_stddev (TGraphErrors * input_grr)
// {
//     vector<double> input_vector; input_vector.clear();
//     for (int i = 0; i < input_grr -> GetN(); i++)
//     {  
//         input_vector.push_back( input_grr -> GetPointY(i) );
//     }

// 	double sum=0;
// 	double average;
// 	double sum_subt = 0;
// 	for (int i=0; i<input_vector.size(); i++)
// 		{
// 			sum+=input_vector[i];

// 		}
// 	average=sum/input_vector.size();
// 	//cout<<"average is : "<<average<<endl;

// 	for (int i=0; i<input_vector.size(); i++)
// 		{
// 			//cout<<input_vector[i]-average<<endl;
// 			sum_subt+=pow((input_vector[i]-average),2);

// 		}
// 	//cout<<"sum_subt : "<<sum_subt<<endl;
// 	double stddev;
// 	stddev=sqrt(sum_subt/(input_vector.size()-1));	
// 	return stddev;
// }	

// pair<double, double> mirrorPolynomial(double a, double b) {
//     // Interchange 'x' and 'y'
//     double mirroredA = 1.0 / a;
//     double mirroredB = -b / a;

//     return {mirroredA, mirroredB};
// }

// // note : pair<reduced chi2, eta of the track>
// // note : vector : {r,z}
// // note : p0 : vertex point {r,z,zerror}
// // note : p1 : inner layer
// // note : p2 : outer layer
// pair<double,double> Get_eta(vector<double>p0, vector<double>p1, vector<double>p2)
// {
//     vector<double> r_vec  = {p0[0], p1[0], p2[0]}; 
//     vector<double> z_vec  = {p0[1], p1[1], p2[1]}; 
//     vector<double> re_vec = {0, 0, 0}; 
//     vector<double> ze_vec = {p0[2], ( fabs( p1[1] ) < 130 ) ? 8. : 10., ( fabs( p2[1] ) < 130 ) ? 8. : 10.}; 

//     // note : swap the r and z, to have easier fitting 
//     // note : in principle, Z should be in the x axis, R should be in the Y axis
//     TGraphErrors * track_gr = new TGraphErrors(3,&r_vec[0],&z_vec[0],&re_vec[0],&ze_vec[0]);    
    
//     double vertical_line = ( fabs( grEY_stddev(track_gr) ) < 0.00001 ) ? 1 : 0;
    
//     if (vertical_line) {return {0,0};}
//     else 
//     {
//         TF1 * fit_rz = new TF1("fit_rz","pol1",-500,500);
//         fit_rz -> SetParameters(0,0);

//         track_gr -> Fit(fit_rz,"NQ");

//         pair<double,double> ax_b = mirrorPolynomial(fit_rz -> GetParameter(1),fit_rz -> GetParameter(0));

//         track_gr -> Delete(); 
//         return  {(fit_rz -> GetChisquare() / double(fit_rz -> GetNDF())), -1 * TMath::Log( fabs( tan( atan2(ax_b.first, (ax_b.first > 0) ? 1. : -1. ) / 2 ) ) )};

//     }

// }

// // note : vector : {r,z}
// // note : p0 : vertex point {r,z,zerror}
// // note : p1 : another point from detector
// // note : since only two points -> no strip width considered
// double Get_eta_2P(vector<double>p0, vector<double>p1)
// {    
//     return  -1 * TMath::Log( fabs( tan( atan2(p1[0] - p0[0], p1[1] - p0[1]) / 2 ) ) );
// }

// double Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y) // note : x : z, y : r
// {
//     if ( fabs(p0x - p1x) < 0.00001 ){ // note : the line is vertical (if z is along the x axis)
//         return p0x;
//     }
//     else {
//         double slope = (p1y - p0y) / (p1x - p0x);
//         double yIntercept = p0y - slope * p0x;
//         double xCoordinate = (given_y - yIntercept) / slope;
//         return xCoordinate;
//     }
// }

// pair<double,double> Get_possible_zvtx(double rvtx, vector<double> p0, vector<double> p1) // note : inner p0, outer p1, vector {r,z}, -> {y,x}
// {
//     vector<double> p0_z_edge = { ( fabs( p0[1] ) < 130 ) ? p0[1] - 8. : p0[1] - 10., ( fabs( p0[1] ) < 130 ) ? p0[1] + 8. : p0[1] + 10.}; // note : vector {left edge, right edge}
//     vector<double> p1_z_edge = { ( fabs( p1[1] ) < 130 ) ? p1[1] - 8. : p1[1] - 10., ( fabs( p1[1] ) < 130 ) ? p1[1] + 8. : p1[1] + 10.}; // note : vector {left edge, right edge}

//     double edge_first  = Get_extrapolation(rvtx,p0_z_edge[0],p0[0],p1_z_edge[1],p1[0]);
//     double edge_second = Get_extrapolation(rvtx,p0_z_edge[1],p0[0],p1_z_edge[0],p1[0]);

//     double mid_point = (edge_first + edge_second) / 2.;
//     double possible_width = fabs(edge_first - edge_second) / 2.;

//     return {mid_point, possible_width}; // note : first : mid point, second : width

// }

// vector<z_str> Get_INTT_Z (string full_directory, int z_capacity)
// {
//     cout<<"----------------- loading the Z information ----------------- "<<endl;
//     int z_eID, z_nclu_inner, z_nclu_outer, z_N_good;
//     double z_zvtx, z_zvtxE, z_rangeL, z_rangeR, z_width_density;

//     TFile * z_file_in = new TFile( Form("%s",full_directory.c_str()) ,"read"); 
//     TTree * z_tree =  (TTree *)z_file_in->Get("tree_Z");
//     z_tree -> SetBranchAddress("eID",&z_eID);
//     z_tree -> SetBranchAddress("nclu_inner",&z_nclu_inner);
//     z_tree -> SetBranchAddress("nclu_outer",&z_nclu_outer);
//     z_tree -> SetBranchAddress("zvtx",&z_zvtx);
//     z_tree -> SetBranchAddress("zvtxE",&z_zvtxE);
//     z_tree -> SetBranchAddress("rangeL",&z_rangeL);
//     z_tree -> SetBranchAddress("rangeR",&z_rangeR);
//     z_tree -> SetBranchAddress("N_good",&z_N_good);
//     z_tree -> SetBranchAddress("Width_density",&z_width_density);

//     vector<z_str> out_vec(z_capacity,{-1,-1,-1,-1,-1,-1,-1,-1});
//     // cout<<"test1"<<endl;

//     for (int i = 0; i < z_tree -> GetEntries(); i++){
//         z_tree -> GetEntry(i);
//         // cout<<"test 0 : "<<i<<"  "<<z_eID<<endl;
//         out_vec[z_eID] = {z_nclu_inner,z_nclu_outer,z_zvtx,z_zvtxE,z_rangeL,z_rangeR,z_N_good,z_width_density};
//     }


//     cout<<"-----------------  INTT Z info. loader done -----------------"<<endl;

//     return out_vec;
// }

// note : use "ls *.root > file_list.txt" to create the list of the file in the folder, full directory in the file_list.txt
// note : set_folder_name = "folder_xxxx"
// note : server_name = "inttx"
void INTT_test(/*pair<double,double>beam_origin*/)
{
    
    // TCanvas * c4 = new TCanvas("","",850,800);    
    // c4 -> cd();
    
    // TCanvas * c2 = new TCanvas("","",2500,800);    
    // c2 -> cd();
    // TPad *pad_xy = new TPad(Form("pad_xy"), "", 0.0, 0.0, 0.33, 1.0);
    // Characterize_Pad(pad_xy, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    // pad_xy -> Draw();

    // TPad *pad_rz = new TPad(Form("pad_rz"), "", 0.33, 0.0, 0.66, 1.0);
    // Characterize_Pad(pad_rz, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    // pad_rz -> Draw();

    // TPad *pad_z = new TPad(Form("pad_z"), "", 0.66, 0.0, 1.0, 1.0);
    // Characterize_Pad(pad_z, 0.15, 0.1, 0.1, 0.1 , 0, 0);
    // pad_z -> Draw();

    // TCanvas * c1 = new TCanvas("","",1000,800);
    // c1 -> cd();

    
    
    // // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/zero_magnet_Takashi_used";
    // // string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_ideal_excludeR1500_100kEvent";
    // // string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_survey_1_XYAlpha_Peek_3.32mm_excludeR1500_100kEvent";

    // string mother_folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869";
    // string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_survey_1_XYAlpha_Peek_3.32mm_excludeR20000_150kEvent_3HotCut";

    // // string mother_folder_directory = "/home/phnxrc/INTT/cwshih/DACscan_data/new_DAC_Scan_0722/AllServer/DAC2";
    // // string file_name = "beam_inttall-00023058-0000_event_base_ana_cluster_ideal_excludeR2000_100kEvent";

    // system(Form("mkdir %s/folder_%s_advanced",mother_folder_directory.c_str(),file_name.c_str()));
    // system(Form("mkdir %s/folder_%s_advanced/good_track",mother_folder_directory.c_str(),file_name.c_str()));

    // pair<double,double> beam_origin = {-0,2};
    // double temp_Y_align = 0.;
    // double temp_X_align = -0.;

    // double zvtx_hist_l = -500;
    // double zvtx_hist_r = 500;

    // int Nhit_cut = 20000;           // note : if (> Nhit_cut)          -> continue
    // int Nhit_cutl = 400;          // note : if (< Nhit_cutl)         -> continue 
    // int clu_size_cut = 4;           // note : if (> clu_size_cut)      -> continue
    // double clu_sum_adc_cut = 31;    // note : if (< clu_sum_adc_cut)   -> continue
    // int N_clu_cut = 15000;          // note : if (> N_clu_cut)         -> continue  unit number
    // int N_clu_cutl = 500;           // note : if (< N_clu_cutl)        -> continue  unit number
    // double phi_diff_cut = 3.5;      // note : if (< phi_diff_cut)      -> pass      unit degree
    // double DCA_cut = 4;             // note : if (< DCA_cut)           -> pass      unit mm
    // int zvtx_cal_require = 15;      // note : if (> zvtx_cal_require)  -> pass
    // int zvtx_draw_requireL = 15;       
    // int zvtx_draw_requireR = 40000;   // note : if ( zvtx_draw_requireL < event && event < zvtx_draw_requireR) -> pass
    // double Integrate_portion = 0.4; // todo : it was 0.6826, try to require less event, as most of the tracklets are not that useful
    // double Integrate_portion_final = 0.68;
    // bool draw_event_display = false;
    // int print_rate = 5;
    // int z_capacity = 20000; // todo : filled the Z capacity

    // double mean_zvtx = -198.38; // note : unit : mm

    // bool full_event = false;
    // long long used_event = 20000;

    // double dNdeta_upper_range = 100;

    // int geo_mode_id = 1;
    // string conversion_mode = (geo_mode_id == 0) ? "ideal" : "survey_1_XYAlpha_Peek";
    // double peek = 3.32405;


    // vector<z_str> INTT_Z = Get_INTT_Z (Form("%s/folder_%s_advanced/INTT_zvtx.root",mother_folder_directory.c_str(),file_name.c_str()), z_capacity);

    // TFile * file_in = new TFile(Form("%s/%s.root",mother_folder_directory.c_str(),file_name.c_str()),"read");
    // TTree * tree = (TTree *)file_in->Get("tree_clu");
    
    // long long N_event = (full_event == true) ? tree -> GetEntries() : used_event;
    // cout<<Form("N_event in file %s : %lli",file_name.c_str(), N_event)<<endl;

    // int N_hits;
    // int N_cluster_inner;
    // int N_cluster_outer;
    // vector<int>* column_vec = new vector<int>();
    // vector<double>* avg_chan_vec = new vector<double>();
    // vector<int>* sum_adc_vec = new vector<int>();
    // vector<int>* sum_adc_conv_vec = new vector<int>();
    // vector<int>* size_vec = new vector<int>();
    // vector<double>* x_vec = new vector<double>();
    // vector<double>* y_vec = new vector<double>();
    // vector<double>* z_vec = new vector<double>();
    // vector<int>* layer_vec = new vector<int>();
    // vector<double>* phi_vec = new vector<double>();

    // tree -> SetBranchStatus("*",0);
    // tree -> SetBranchStatus("nhits",1);
    // tree -> SetBranchStatus("nclu_inner",1);
    // tree -> SetBranchStatus("nclu_outer",1);
    // tree -> SetBranchStatus("column",1);
    // tree -> SetBranchStatus("avg_chan",1);
    // tree -> SetBranchStatus("sum_adc",1);
    // tree -> SetBranchStatus("sum_adc_conv",1);
    // tree -> SetBranchStatus("size",1);
    // tree -> SetBranchStatus("x",1);
    // tree -> SetBranchStatus("y",1);
    // tree -> SetBranchStatus("z",1);
    // tree -> SetBranchStatus("layer",1);
    // tree -> SetBranchStatus("phi",1);

    // tree -> SetBranchAddress("nhits",&N_hits);
    // tree -> SetBranchAddress("nclu_inner",&N_cluster_inner);
    // tree -> SetBranchAddress("nclu_outer",&N_cluster_outer);

    // tree -> SetBranchAddress("column", &column_vec);
    // tree -> SetBranchAddress("avg_chan", &avg_chan_vec);
    // tree -> SetBranchAddress("sum_adc", &sum_adc_vec);
    // tree -> SetBranchAddress("sum_adc_conv", &sum_adc_conv_vec);
    // tree -> SetBranchAddress("size", &size_vec);
    // tree -> SetBranchAddress("x", &x_vec);
    // tree -> SetBranchAddress("y", &y_vec);
    // tree -> SetBranchAddress("z", &z_vec);
    // tree -> SetBranchAddress("layer", &layer_vec);
    // tree -> SetBranchAddress("phi", &phi_vec);

    // // TFile * out_file = new TFile(Form("%s/folder_%s_advanced/INTT_dNdeta.root",mother_folder_directory.c_str(),file_name.c_str()),"RECREATE");

    // int N_hits_out, N_cluster_inner_out, N_cluster_outer_out, ntrack_out;
    // int eID_out;
    // double out_xvtx, out_yvtx, out_zvtx, out_zvtxE;
    // vector<double> out_inner_x;
    // vector<double> out_inner_y;
    // vector<double> out_inner_z;
    // vector<double> out_inner_r;
    // vector<double> out_inner_phi; 
    // vector<double> out_inner_zE;
    // vector<double> out_outer_x;
    // vector<double> out_outer_y;
    // vector<double> out_outer_z;
    // vector<double> out_outer_r;
    // vector<double> out_outer_phi; 
    // vector<double> out_outer_zE;
    // vector<double> out_eta;
    // vector<double> out_eta_rchi2;

    // TTree * tree_out =  new TTree ("tree_eta", "eta info.");
    // tree_out -> Branch("eID",&eID_out);
    // tree_out -> Branch("nhits",&N_hits_out);
    // tree_out -> Branch("nclu_inner",&N_cluster_inner_out);
    // tree_out -> Branch("nclu_outer",&N_cluster_outer_out);
    // tree_out -> Branch("ntrack",&ntrack_out);
    // tree_out -> Branch("xvtx",&out_xvtx);
    // tree_out -> Branch("yvtx",&out_yvtx);
    // tree_out -> Branch("zvtx",&out_zvtx);
    // tree_out -> Branch("zvtxE",&out_zvtxE);
    
    // tree_out -> Branch("inner_x",&out_inner_x); // note : inner
    // tree_out -> Branch("inner_y",&out_inner_y);
    // tree_out -> Branch("inner_z",&out_inner_z);
    // tree_out -> Branch("inner_r",&out_inner_r);
    // tree_out -> Branch("inner_phi",&out_inner_phi);
    // tree_out -> Branch("inner_zE",&out_inner_zE); 
    // tree_out -> Branch("outer_x",&out_outer_x); // note : outer
    // tree_out -> Branch("outer_y",&out_outer_y);
    // tree_out -> Branch("outer_z",&out_outer_z);
    // tree_out -> Branch("outer_r",&out_outer_r);
    // tree_out -> Branch("outer_phi",&out_outer_phi);
    // tree_out -> Branch("outer_zE",&out_outer_zE); 
    // tree_out -> Branch("eta",&out_eta); // note : track
    // tree_out -> Branch("eta_rchi",&out_eta_rchi2);

    // TLatex *draw_text = new TLatex();
    // draw_text -> SetNDC();
    // draw_text -> SetTextSize(0.02);

    // vector<clu_info> temp_sPH_inner_nocolumn_vec; temp_sPH_inner_nocolumn_vec.clear();
    // vector<clu_info> temp_sPH_outer_nocolumn_vec; temp_sPH_outer_nocolumn_vec.clear();
    // vector<vector<double>> temp_sPH_nocolumn_vec(2);
    // vector<vector<double>> temp_sPH_nocolumn_rz_vec(2);

    // TH1F * dNdeta_hist = new TH1F("","",29,-2.9,2.9);
    // // TH1F * dNdeta_hist = new TH1F("","",145,-2.9,2.9);
    // dNdeta_hist -> SetMarkerStyle(20);
    // dNdeta_hist -> SetMarkerSize(0.8);
    // dNdeta_hist -> SetMarkerColor(TColor::GetColor("#1A3947"));
    // dNdeta_hist -> SetLineColor(TColor::GetColor("#1A3947"));
    // dNdeta_hist -> SetLineWidth(2);
    // dNdeta_hist -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
    // dNdeta_hist -> GetXaxis() -> SetTitle("#eta");
    // dNdeta_hist -> GetYaxis() -> SetRangeUser(0,50);
    // dNdeta_hist -> SetTitleSize(0.06, "X");
    // dNdeta_hist -> SetTitleSize(0.06, "Y");
	// dNdeta_hist -> GetXaxis() -> SetTitleOffset (0.71);
    // dNdeta_hist -> GetYaxis() -> SetTitleOffset (1.1);
	// dNdeta_hist -> GetXaxis() -> CenterTitle(true);
    // dNdeta_hist -> GetYaxis() -> CenterTitle(true);

    // // TH1F * dNdeta_2P_inner_hist = new TH1F("","",29,-2.9,2.9);
    // TH1F * dNdeta_2P_inner_hist = new TH1F("","",145,-2.9,2.9);
    // dNdeta_2P_inner_hist -> SetMarkerStyle(20);
    // dNdeta_2P_inner_hist -> SetMarkerSize(0.8);
    // dNdeta_2P_inner_hist -> SetMarkerColor(TColor::GetColor("#1A3947"));
    // dNdeta_2P_inner_hist -> SetLineColor(TColor::GetColor("#1A3947"));
    // dNdeta_2P_inner_hist -> SetLineWidth(2);
    // dNdeta_2P_inner_hist -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
    // dNdeta_2P_inner_hist -> GetXaxis() -> SetTitle("#eta");
    // dNdeta_2P_inner_hist -> GetYaxis() -> SetRangeUser(0,50);
    // dNdeta_2P_inner_hist -> SetTitleSize(0.06, "X");
    // dNdeta_2P_inner_hist -> SetTitleSize(0.06, "Y");
	// dNdeta_2P_inner_hist -> GetXaxis() -> SetTitleOffset (0.71);
    // dNdeta_2P_inner_hist -> GetYaxis() -> SetTitleOffset (1.1);
	// dNdeta_2P_inner_hist -> GetXaxis() -> CenterTitle(true);
    // dNdeta_2P_inner_hist -> GetYaxis() -> CenterTitle(true);

    // TH1F * dNdeta_2P_outer_hist = new TH1F("","",29,-2.9,2.9);
    // dNdeta_2P_outer_hist -> SetMarkerStyle(20);
    // dNdeta_2P_outer_hist -> SetMarkerSize(0.8);
    // dNdeta_2P_outer_hist -> SetMarkerColor(TColor::GetColor("#1A3947"));
    // dNdeta_2P_outer_hist -> SetLineColor(TColor::GetColor("#1A3947"));
    // dNdeta_2P_outer_hist -> SetLineWidth(2);
    // dNdeta_2P_outer_hist -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
    // dNdeta_2P_outer_hist -> GetXaxis() -> SetTitle("#eta");
    // dNdeta_2P_outer_hist -> GetYaxis() -> SetRangeUser(0,50);
    // dNdeta_2P_outer_hist -> SetTitleSize(0.06, "X");
    // dNdeta_2P_outer_hist -> SetTitleSize(0.06, "Y");
	// dNdeta_2P_outer_hist -> GetXaxis() -> SetTitleOffset (0.71);
    // dNdeta_2P_outer_hist -> GetYaxis() -> SetTitleOffset (1.1);
	// dNdeta_2P_outer_hist -> GetXaxis() -> CenterTitle(true);
    // dNdeta_2P_outer_hist -> GetYaxis() -> CenterTitle(true);

    // double N_good_event = 0;

    // TH2F * Good_inner_outer_pos_xy_nzB = new TH2F("","Good_inner_outer_pos_xy_nzB",360,-150,150,360,-150,150);
    // Good_inner_outer_pos_xy_nzB -> SetStats(0);
    // Good_inner_outer_pos_xy_nzB -> GetXaxis() -> SetTitle("X axis");
    // Good_inner_outer_pos_xy_nzB -> GetYaxis() -> SetTitle("Y axis");

    // TH2F * Good_inner_outer_pos_xy_nzA = new TH2F("","Good_inner_outer_pos_xy_nzA",360,-150,150,360,-150,150);
    // Good_inner_outer_pos_xy_nzA -> SetStats(0);
    // Good_inner_outer_pos_xy_nzA -> GetXaxis() -> SetTitle("X axis");
    // Good_inner_outer_pos_xy_nzA -> GetYaxis() -> SetTitle("Y axis");

    // TH2F * Good_inner_outer_pos_xy_pzA = new TH2F("","Good_inner_outer_pos_xy_pzA",360,-150,150,360,-150,150);
    // Good_inner_outer_pos_xy_pzA -> SetStats(0);
    // Good_inner_outer_pos_xy_pzA -> GetXaxis() -> SetTitle("X axis");
    // Good_inner_outer_pos_xy_pzA -> GetYaxis() -> SetTitle("Y axis");

    // TH2F * Good_inner_outer_pos_xy_pzB = new TH2F("","Good_inner_outer_pos_xy_pzB",360,-150,150,360,-150,150);
    // Good_inner_outer_pos_xy_pzB -> SetStats(0);
    // Good_inner_outer_pos_xy_pzB -> GetXaxis() -> SetTitle("X axis");
    // Good_inner_outer_pos_xy_pzB -> GetYaxis() -> SetTitle("Y axis");

    // TH2F * Good_track_space = new TH2F("","Good_track_space",200,-300,300,200,-300,300);
    // Good_track_space -> SetStats(0);
    // Good_track_space -> GetXaxis() -> SetTitle("X axis");
    // Good_track_space -> GetYaxis() -> SetTitle("Y axis");

    // // dNdeta_hist -> GetXaxis() -> SetLimits(-10,10);
    // // dNdeta_hist -> GetXaxis() -> SetNdivisions  (505);

    // TF1 * zvtx_finder = new TF1("zvtx_finder","pol0",-1,3000); 

    
    // vector<vector<double>> good_track_xy_vec; good_track_xy_vec.clear();
    // vector<vector<double>> good_track_rz_vec; good_track_rz_vec.clear();
    // vector<vector<double>> good_comb_rz_vec; good_comb_rz_vec.clear();
    // vector<vector<double>> good_comb_xy_vec; good_comb_xy_vec.clear();
    // vector<vector<double>> good_comb_phi_vec; good_comb_phi_vec.clear();
    // vector<vector<double>> good_tracklet_rz; good_tracklet_rz.clear();
    // TLine * track_line = new TLine();
    // track_line -> SetLineWidth(1);
    // track_line -> SetLineColorAlpha(38,0.5);

    // TLine * coord_line = new TLine();
    // coord_line -> SetLineWidth(1);
    // coord_line -> SetLineColor(16);
    // coord_line -> SetLineStyle(2);


    // int N_event_pass_number = 0;

    // vector<double> DCA_info_vec; DCA_info_vec.clear(); 
    // pair<double,double> Get_eta_pair;


    MemInfo_t test_aaa;
    gSystem->GetMemInfo(&test_aaa);
    sleep(20);

    cout<<"Memory usage ? "<<test_aaa.fMemUsed<<endl;



}
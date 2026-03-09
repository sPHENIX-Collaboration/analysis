#ifndef MegaTrackFinder_h
#define MegaTrackFinder_h

#include "INTTXYvtxEvt.h"

class MegaTrackFinder : public INTTXYvtxEvt
{
    public:
        
        // note : the N_centrality_bin here means the number without the "inclusive"
        MegaTrackFinder(string run_type, string out_folder_directory, int N_centrality_bin, pair<double,double> beam_origin_in, bool mark_used = true) : 
        INTTXYvtxEvt(run_type, out_folder_directory), N_centrality_bin(N_centrality_bin), mark_used(mark_used)
        {
            inner_used_mega_clu.clear();
            outer_used_mega_clu.clear();

            clu4_track_performance.clear();
            clu4_track_index.clear();
            clu4_track_clu_phi.clear();

            clu3_track_performance.clear();
            clu3_track_index.clear();
            clu3_track_clu_phi.clear();
            clu3_track_type.clear();

            mega_track_eta.clear();

            INTTXYvtxEvt::beam_origin = beam_origin_in;

            NClu3_track_count = 0;
            NClu4_track_count = 0;

            Init();
        };

        void FindMegaTracks(vector<vector<pair<bool,clu_info>>>& inner_clu_phi_map, vector<vector<pair<bool,clu_info>>>& outer_clu_phi_map, pair<double,double> evt_z, int final_centrality_bin);
        void ClearEvt();   
        int Get_NClu3_track_count() {return NClu3_track_count;};
        int Get_NClu4_track_count() {return NClu4_track_count;};
        double Get_performance_cut() {return performance_cut;}; 
        vector<double> Get_mega_track_eta() {return mega_track_eta;};

        TH2F * Get_NClu3_track_centrality_2D() {return NClu3_track_centrality_2D;};
        TH2F * Get_NClu4_track_centrality_2D() {return NClu4_track_centrality_2D;};
        TH1F * Get_cluster4_track_phi_1D() {return cluster4_track_phi_1D;};
        TH1F * Get_cluster3_track_phi_1D() {return cluster3_track_phi_1D;};
        TH1F * Get_cluster3_inner_track_phi_1D() {return cluster3_inner_track_phi_1D;};
        TH1F * Get_cluster3_outer_track_phi_1D() {return cluster3_outer_track_phi_1D;};
        TH1F * Get_clu4_track_ReducedChi2_1D() {return clu4_track_ReducedChi2_1D;};
        TH1F * Get_clu3_track_ReducedChi2_1D() {return clu3_track_ReducedChi2_1D;};
        TH1F * Get_mega_track_eta_1D() {return mega_track_eta_1D;};
        TH1F * Get_cluster3_inner_track_eta_1D() {return cluster3_inner_track_eta_1D;};
        TH1F * Get_cluster3_outer_track_eta_1D() {return cluster3_outer_track_eta_1D;};


    protected:

        int N_centrality_bin;
        bool mark_used;

        TH2F * NClu3_track_centrality_2D;
        TH2F * NClu4_track_centrality_2D;
        TH1F * cluster4_track_phi_1D;
        TH1F * cluster3_track_phi_1D;
        TH1F * cluster3_inner_track_eta_1D;
        TH1F * cluster3_inner_track_phi_1D;
        TH1F * cluster3_outer_track_eta_1D;
        TH1F * cluster3_outer_track_phi_1D;
        TH1F * clu4_track_ReducedChi2_1D;
        TH1F * clu3_track_ReducedChi2_1D;
        TH1F * mega_track_eta_1D;

        int NClu3_track_count;
        int NClu4_track_count;

        double mega_cluster_deltaphi_cut = 0.1;
        double performance_cut = 0.0005;


        TF1 * pol0_fit;
        TGraph * r_phi_gr;
        TGraphErrors * track_gr;

        map<string,int> inner_used_mega_clu;
        map<string,int> outer_used_mega_clu;

        // note : to keep the cluster-4 tracklet fit result and the final 1D array
        vector<double>              clu4_track_performance;
        vector<vector<int>>         clu4_track_index; 
        vector<pair<double,double>> clu4_track_clu_phi;    // note : 8 index, 4 inner, 4 outer

        vector<double>              clu3_track_performance;
        vector<vector<int>>         clu3_track_index; 
        vector<pair<double,double>> clu3_track_clu_phi;    // note : 6 index
        vector<int>                 clu3_track_type;

        pair<double,double> Get_eta_pair;
        vector<double> mega_track_eta; // note : keep the eta of the mega tracklet (mega tracklet means: 3/4-cluster tracks)

        void Init();
        double get_delta_phi(double angle_1, double angle_2);
        double get_track_phi(double inner_clu_phi_in, double delta_phi_in);
        double get_offset_clu_phi(double x_in, double y_in);
        double grEY_stddev(TGraphErrors * input_grr);
        pair<double, double> mirrorPolynomial(double a, double b);
        pair<double,double> Get_eta3(vector<double>p0, vector<double>p1, vector<double>p2, vector<double>p3);
        pair<double,double> Get_eta4(vector<double>p0, vector<double>p1, vector<double>p2, vector<double>p3, vector<double>p4);
        void self_check(vector<vector<pair<bool,clu_info>>> inner_clu_phi_map, vector<vector<pair<bool,clu_info>>> outer_clu_phi_map);
        // double get_radius(double x, double y);
        // pair<double,double> Get_possible_zvtx(double rvtx, vector<double> p0, vector<double> p1) ;// note : inner p0, outer p1, vector {r,z}, -> {y,x}
        
};




void MegaTrackFinder::Init()
{
    cluster4_track_phi_1D = new TH1F("","cluster4_track_phi_1D", 200, -40, 400);
    cluster4_track_phi_1D -> GetXaxis() -> SetTitle("Tracklet #phi");
    cluster4_track_phi_1D -> GetYaxis() -> SetTitle("Entry");
    cluster4_track_phi_1D -> GetXaxis() -> SetNdivisions(505);

    cluster3_track_phi_1D = new TH1F("","cluster3_track_phi_1D", 200, -40, 400);
    cluster3_track_phi_1D -> GetXaxis() -> SetTitle("Tracklet #phi");
    cluster3_track_phi_1D -> GetYaxis() -> SetTitle("Entry");
    cluster3_track_phi_1D -> GetXaxis() -> SetNdivisions(505);

    cluster3_inner_track_phi_1D = new TH1F("","cluster3_inner_track_phi_1D", 200, -40, 400);
    cluster3_inner_track_phi_1D -> GetXaxis() -> SetTitle("Tracklet #phi");
    cluster3_inner_track_phi_1D -> GetYaxis() -> SetTitle("Entry");
    cluster3_inner_track_phi_1D -> GetXaxis() -> SetNdivisions(505);

    cluster3_outer_track_phi_1D = new TH1F("","cluster3_outer_track_phi_1D", 200, -40, 400);
    cluster3_outer_track_phi_1D -> GetXaxis() -> SetTitle("Tracklet #phi");
    cluster3_outer_track_phi_1D -> GetYaxis() -> SetTitle("Entry");
    cluster3_outer_track_phi_1D -> GetXaxis() -> SetNdivisions(505);

    NClu3_track_centrality_2D = new TH2F("","NClu3_track_centrality_2D", N_centrality_bin, 0, N_centrality_bin, 50, 0, 50);
    NClu3_track_centrality_2D -> GetXaxis() -> SetTitle("Centrality bin");
    NClu3_track_centrality_2D -> GetYaxis() -> SetTitle("N Clu3 tracks");
    NClu3_track_centrality_2D -> GetXaxis() -> SetNdivisions(505);

    NClu4_track_centrality_2D = new TH2F("","NClu4_track_centrality_2D", N_centrality_bin, 0, N_centrality_bin, 20, 0, 20);
    NClu4_track_centrality_2D -> GetXaxis() -> SetTitle("Centrality bin");
    NClu4_track_centrality_2D -> GetYaxis() -> SetTitle("N Clu4 tracks");
    NClu4_track_centrality_2D -> GetXaxis() -> SetNdivisions(505);   

    clu4_track_ReducedChi2_1D = new TH1F("","clu4_track_ReducedChi2_1D", 100, 0, 0.01);
    clu4_track_ReducedChi2_1D -> GetXaxis() -> SetTitle("Reduced #chi^{2}");
    clu4_track_ReducedChi2_1D -> GetYaxis() -> SetTitle("Entry");
    clu4_track_ReducedChi2_1D -> GetXaxis() -> SetNdivisions(505);

    clu3_track_ReducedChi2_1D = new TH1F("","clu3_track_ReducedChi2_1D", 100, 0, 0.01);
    clu3_track_ReducedChi2_1D -> GetXaxis() -> SetTitle("Reduced #chi^{2}");
    clu3_track_ReducedChi2_1D -> GetYaxis() -> SetTitle("Entry");
    clu3_track_ReducedChi2_1D -> GetXaxis() -> SetNdivisions(505);

    double Eta_NBin = 61;
    double Eta_Min = -3.05;
    double Eta_Max = 3.05;

    mega_track_eta_1D = new TH1F("","mega_track_eta_1D", Eta_NBin, Eta_Min, Eta_Max);
    mega_track_eta_1D -> GetXaxis() -> SetTitle("Mega track #eta");
    mega_track_eta_1D -> GetYaxis() -> SetTitle("Entry");
    mega_track_eta_1D -> GetXaxis() -> SetNdivisions(505);

    cluster3_inner_track_eta_1D = new TH1F("","cluster3_inner_track_eta_1D", Eta_NBin, Eta_Min, Eta_Max);
    cluster3_inner_track_eta_1D -> GetXaxis() -> SetTitle("clu3 inner track #eta");
    cluster3_inner_track_eta_1D -> GetYaxis() -> SetTitle("Entry");
    cluster3_inner_track_eta_1D -> GetXaxis() -> SetNdivisions(505);

    cluster3_outer_track_eta_1D = new TH1F("","cluster3_outer_track_eta_1D", Eta_NBin, Eta_Min, Eta_Max);
    cluster3_outer_track_eta_1D -> GetXaxis() -> SetTitle("clu3 outer track #eta");
    cluster3_outer_track_eta_1D -> GetYaxis() -> SetTitle("Entry");
    cluster3_outer_track_eta_1D -> GetXaxis() -> SetNdivisions(505);

    pol0_fit = new TF1("pol0_fit","pol0",0,360);
    r_phi_gr = new TGraph();
    r_phi_gr -> Set(0);

    track_gr = new TGraphErrors();
    track_gr -> Set(0);
}

void MegaTrackFinder::FindMegaTracks(vector<vector<pair<bool,clu_info>>>& inner_clu_phi_map, vector<vector<pair<bool,clu_info>>>& outer_clu_phi_map, pair<double,double> evt_z, int final_centrality_bin)
{
    // cout<<" "<<endl;

    // note : find the proto "mega-inner + single outer" 3-cluster track 
    // note : if such a 3-cluster track is found, then keep this one, and try to seek for the 4-cluster track as it should fit to tighter selection
    for (int inner_clu1_first = 0; inner_clu1_first < 360; inner_clu1_first++)
    {
        for (int inner_clu1_second = 0; inner_clu1_second < inner_clu_phi_map[inner_clu1_first].size(); inner_clu1_second++) // note : the first inner clu
        {
            if (inner_clu_phi_map[inner_clu1_first][inner_clu1_second].first == true) {continue;} // note : if the cluster is used, skip it

            double inner_clu1_radius = get_radius(inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.x - beam_origin.first, inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.y - beam_origin.second);
            double inner_clu1_phi    = get_offset_clu_phi(inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.x, inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.y);     

            for (int scan_inner_i2 = -1; scan_inner_i2 < 2; scan_inner_i2++) // note : -1, 0, 1
            {
                int inner_clu2_first = ((inner_clu1_first + scan_inner_i2) < 0) ? 360 + (inner_clu1_first + scan_inner_i2) : ((inner_clu1_first + scan_inner_i2) > 359) ? (inner_clu1_first + scan_inner_i2)-360 : inner_clu1_first + scan_inner_i2;

                for (int inner_clu2_second = 0; inner_clu2_second < inner_clu_phi_map[inner_clu2_first].size(); inner_clu2_second++) // note : the second inner clu
                {
                    if (inner_clu_phi_map[inner_clu2_first][inner_clu2_second].first == true) {continue;} // note : if the cluster is used, skip it

                    // note : the cluster itself
                    if (inner_clu1_first == inner_clu2_first && inner_clu1_second == inner_clu2_second) {continue;} 
                    // note : in the same sub-layer, skip
                    // todo : todo : it may not work for the data
                    if (inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.layer == inner_clu_phi_map[inner_clu2_first][inner_clu2_second].second.layer) {continue;}
                    // note : I expect the two cluster have to have the same z position
                    // note : but in case of data, the z position of the same strip may be fluctuated a little bit 
                    // todo : currently, set the Z position flutuation to be 4 mm
                    if (fabs(inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.z - inner_clu_phi_map[inner_clu2_first][inner_clu2_second].second.z) > 4 ) {continue;}

                    double inner_clu2_radius = get_radius(inner_clu_phi_map[inner_clu2_first][inner_clu2_second].second.x - beam_origin.first, inner_clu_phi_map[inner_clu2_first][inner_clu2_second].second.y - beam_origin.second);
                    double inner_clu2_phi    = get_offset_clu_phi(inner_clu_phi_map[inner_clu2_first][inner_clu2_second].second.x, inner_clu_phi_map[inner_clu2_first][inner_clu2_second].second.y);
                    // double delta_phi_in1_in2 = get_delta_phi(inner_clu1_phi, inner_clu2_phi);

                    int inner_comb_index[4] = {inner_clu1_first, inner_clu1_second, inner_clu2_first, inner_clu2_second};
                    int id_inner_small_r = ( inner_clu1_radius < inner_clu2_radius ) ? 0 : 2;

                    for (int scan_outer_i1 = -1; scan_outer_i1 < 2; scan_outer_i1++) // note : -1, 0, 1
                     {
                        int outer_clu1_first = ((inner_clu2_first + scan_outer_i1) < 0) ? 360 + (inner_clu2_first + scan_outer_i1) : ((inner_clu2_first + scan_outer_i1) > 359) ? (inner_clu2_first + scan_outer_i1)-360 : inner_clu2_first + scan_outer_i1;

                        for (int outer_clu1_second = 0; outer_clu1_second < outer_clu_phi_map[outer_clu1_first].size(); outer_clu1_second++)
                        {
                            if (outer_clu_phi_map[outer_clu1_first][outer_clu1_second].first == true) {continue;} // note : if the cluster is used, skip it

                            double outer_clu1_radius = get_radius(outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.x - beam_origin.first, outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.y - beam_origin.second);
                            double outer_clu1_phi    = get_offset_clu_phi(outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.x, outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.y);
                            
                            // double delta_phi_in1_outout= in_delta_phi(inner_clu1_phi, outer_clu1_phi);
                            // double delta_phi_in2_outout= in_delta_phi(inner_clu2_phi, outer_clu1_phi);

                            pair<double,double> z_range_info = Get_possible_zvtx( 
                                0., // get_radius(beam_origin.first,beam_origin.second), 
                                {get_radius(inner_clu_phi_map[inner_comb_index[id_inner_small_r]][inner_comb_index[id_inner_small_r+1]].second.x - beam_origin.first, inner_clu_phi_map[inner_comb_index[id_inner_small_r]][inner_comb_index[id_inner_small_r+1]].second.y - beam_origin.second), inner_clu_phi_map[inner_comb_index[id_inner_small_r]][inner_comb_index[id_inner_small_r+1]].second.z}, // note : unsign radius
                                {get_radius(outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.x - beam_origin.first, outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.y - beam_origin.second), outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.z}  // note : unsign radius
                            );

                            // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
                            if (z_range_info.first - z_range_info.second > evt_z.first + evt_z.second || z_range_info.first + z_range_info.second < evt_z.first - evt_z.second) {continue;}

                            r_phi_gr -> SetPoint(r_phi_gr -> GetN(), inner_clu1_radius, inner_clu1_phi);
                            r_phi_gr -> SetPoint(r_phi_gr -> GetN(), inner_clu2_radius, inner_clu2_phi);
                            r_phi_gr -> SetPoint(r_phi_gr -> GetN(), outer_clu1_radius, outer_clu1_phi);
                            r_phi_gr -> Fit(pol0_fit, "NQ");

                            clu3_track_performance.push_back(pol0_fit->GetChisquare()/double(pol0_fit->GetNDF())); // note: the reduced chi2 is used as the performance of the tracklet
                            clu3_track_index.push_back({inner_clu1_first, inner_clu1_second, inner_clu2_first, inner_clu2_second, outer_clu1_first, outer_clu1_second});
                            clu3_track_clu_phi.push_back({pol0_fit->GetParameter(0), (inner_clu1_phi + inner_clu2_phi + outer_clu1_phi)/3. });
                            clu3_track_type.push_back(0); // note : inner-mega and outer single 
                            clu3_track_ReducedChi2_1D -> Fill(pol0_fit->GetChisquare()/double(pol0_fit->GetNDF()));
                            
                            
                            r_phi_gr -> Set(0);

                            // note : here we start to work on the 4-cluster tracklet instead of spin out the code, in order to make it more faster
                            // note : the reason is that, the 4-cluster tracklet should have more tight selections, so once we found a 3-cluster track, then we try to find the 4-cluster track
                            for (int scan_outer_i2 = -1; scan_outer_i2 < 2; scan_outer_i2++) // note : -1, 0, 1
                            {
                                int outer_clu2_first = ((outer_clu1_first + scan_outer_i2) < 0) ? 360 + (outer_clu1_first + scan_outer_i2) : ((outer_clu1_first + scan_outer_i2) > 359) ? (outer_clu1_first + scan_outer_i2)-360 : outer_clu1_first + scan_outer_i2;

                                for (int outer_clu2_second = 0; outer_clu2_second < outer_clu_phi_map[outer_clu2_first].size(); outer_clu2_second++)
                                {
                                    if (outer_clu_phi_map[outer_clu2_first][outer_clu2_second].first == true) {continue;}

                                    // note : the cluster itself
                                    if (outer_clu1_first == outer_clu2_first && outer_clu1_second == outer_clu2_second) {continue;} 
                                    // note : in the same sub-layer, skip
                                    // todo : todo : it may not work for the data
                                    if (outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.layer == outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.layer) {continue;}
                                    // note : I expect the two cluster have to have the same z position
                                    // note : but in case of data, the z position of the same strip may be fluctuated a little bit 
                                    // todo : currently, set the Z position flutuation to be 4 mm
                                    if (fabs(outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.z - outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.z) > 4 ) {continue;}

                                    double outer_clu2_radius = get_radius(outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.x - beam_origin.first, outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.y - beam_origin.second);
                                    double outer_clu2_phi    = get_offset_clu_phi(outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.x, outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.y);
                                    // double delta_phi_out2_out2 = get_delta_phi(outer_clu1_phi, outer_clu2_phi);

                                    int outer_comb_index[4] = {outer_clu1_first, outer_clu1_second, outer_clu2_first, outer_clu2_second};
                                    int id_outer_large_r = ( outer_clu1_radius > outer_clu2_radius ) ? 0 : 2;

                                    pair<double,double> z_range_info = Get_possible_zvtx( 
                                        0., // get_radius(beam_origin.first,beam_origin.second), 
                                        {get_radius(inner_clu_phi_map[inner_comb_index[id_inner_small_r]][inner_comb_index[id_inner_small_r+1]].second.x - beam_origin.first, inner_clu_phi_map[inner_comb_index[id_inner_small_r]][inner_comb_index[id_inner_small_r+1]].second.y - beam_origin.second), inner_clu_phi_map[inner_comb_index[id_inner_small_r]][inner_comb_index[id_inner_small_r+1]].second.z}, // note : unsign radius
                                        {get_radius(outer_clu_phi_map[outer_comb_index[id_outer_large_r]][outer_comb_index[id_outer_large_r+1]].second.x - beam_origin.first, outer_clu_phi_map[outer_comb_index[id_outer_large_r]][outer_comb_index[id_outer_large_r+1]].second.y - beam_origin.second), outer_clu_phi_map[outer_comb_index[id_outer_large_r]][outer_comb_index[id_outer_large_r+1]].second.z}  // note : unsign radius
                                    );

                                    // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
                                    if (z_range_info.first - z_range_info.second > evt_z.first + evt_z.second || z_range_info.first + z_range_info.second < evt_z.first - evt_z.second) {continue;}

                                    r_phi_gr -> SetPoint(r_phi_gr -> GetN(), inner_clu1_radius, inner_clu1_phi);
                                    r_phi_gr -> SetPoint(r_phi_gr -> GetN(), inner_clu2_radius, inner_clu2_phi);
                                    r_phi_gr -> SetPoint(r_phi_gr -> GetN(), outer_clu1_radius, outer_clu1_phi);
                                    r_phi_gr -> SetPoint(r_phi_gr -> GetN(), outer_clu2_radius, outer_clu2_phi);
                                    r_phi_gr -> Fit(pol0_fit, "NQ");

                                    clu4_track_performance.push_back(pol0_fit->GetChisquare()/double(pol0_fit->GetNDF())); // note: the reduced chi2 is used as the performance of the tracklet
                                    clu4_track_index.push_back({inner_clu1_first, inner_clu1_second, inner_clu2_first, inner_clu2_second, outer_clu1_first, outer_clu1_second, outer_clu2_first, outer_clu2_second});
                                    clu4_track_clu_phi.push_back({pol0_fit->GetParameter(0), (inner_clu1_phi + inner_clu2_phi + outer_clu1_phi + outer_clu2_phi)/4. });
                                    clu4_track_ReducedChi2_1D -> Fill(pol0_fit->GetChisquare()/double(pol0_fit->GetNDF()));
                                    
                                    r_phi_gr -> Set(0);
                                }

                            } // note : outer_clu2 loop
                        }
                    } // note : outer_clu1_loop




                }
            }// note : inner clu2 loop           

        }
    } // note : inner clu1 loop


    // note : try to check another case of the 3-cluster tracklet, single inner and mega outer
    for (int outer_clu1_first = 0; outer_clu1_first < 360; outer_clu1_first++)
    {
        for (int outer_clu1_second = 0; outer_clu1_second < outer_clu_phi_map[outer_clu1_first].size(); outer_clu1_second++) // note : the first outer clu
        {
            if (outer_clu_phi_map[outer_clu1_first][outer_clu1_second].first == true) {continue;} // note : if the cluster is used, skip it

            double outer_clu1_radius = get_radius(outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.x - beam_origin.first, outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.y - beam_origin.second);
            double outer_clu1_phi    = get_offset_clu_phi(outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.x, outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.y);

            for (int scan_outer_i2 = -1; scan_outer_i2 < 2; scan_outer_i2++) // note : -1, 0, 1
            {
                int outer_clu2_first = ((outer_clu1_first + scan_outer_i2) < 0) ? 360 + (outer_clu1_first + scan_outer_i2) : ((outer_clu1_first + scan_outer_i2) > 359) ? (outer_clu1_first + scan_outer_i2)-360 : outer_clu1_first + scan_outer_i2;

                for (int outer_clu2_second = 0; outer_clu2_second < outer_clu_phi_map[outer_clu2_first].size(); outer_clu2_second++) // note : the second outer clu
                {
                    if (outer_clu_phi_map[outer_clu2_first][outer_clu2_second].first == true) {continue;} // note : if the cluster is used, skip it

                    // note : the cluster itself
                    if (outer_clu1_first == outer_clu2_first && outer_clu1_second == outer_clu2_second) {continue;} 
                    // note : in the same sub-layer, skip
                    // todo : todo : it may not work for the data
                    if (outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.layer == outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.layer) {continue;}
                    // note : I expect the two cluster have to have the same z position
                    // note : but in case of data, the z position of the same strip may be fluctuated a little bit 
                    // todo : currently, set the Z position flutuation to be 4 mm
                    if (fabs(outer_clu_phi_map[outer_clu1_first][outer_clu1_second].second.z - outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.z) > 4 ) {continue;}

                    double outer_clu2_radius = get_radius(outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.x - beam_origin.first, outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.y - beam_origin.second);
                    double outer_clu2_phi    = get_offset_clu_phi(outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.x, outer_clu_phi_map[outer_clu2_first][outer_clu2_second].second.y);
                    // double delta_phi_out1_out2 = get_delta_phi(outer_clu1_phi, outer_clu2_phi);

                    int outer_comb_index[4] = {outer_clu1_first, outer_clu1_second, outer_clu2_first, outer_clu2_second};
                    int id_outer_large_r = ( outer_clu1_radius > outer_clu2_radius ) ? 0 : 2;

                    for (int scan_inner_i1 = -1; scan_inner_i1 < 2; scan_inner_i1++) // note : -1, 0, 1
                     {
                        int inner_clu1_first = ((outer_clu2_first + scan_inner_i1) < 0) ? 360 + (outer_clu2_first + scan_inner_i1) : ((outer_clu2_first + scan_inner_i1) > 359) ? (outer_clu2_first + scan_inner_i1)-360 : outer_clu2_first + scan_inner_i1;

                        for (int inner_clu1_second = 0; inner_clu1_second < inner_clu_phi_map[inner_clu1_first].size(); inner_clu1_second++)
                        {
                            if (inner_clu_phi_map[inner_clu1_first][inner_clu1_second].first == true) {continue;} // note : if the cluster is used, skip it

                            double inner_clu1_radius = get_radius(inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.x - beam_origin.first, inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.y - beam_origin.second);
                            double inner_clu1_phi    = get_offset_clu_phi(inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.x, inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.y);
                            
                            // double delta_phi_out1_in1 = get_delta_phi(outer_clu1_phi, inner_clu1_phi);
                            // double delta_phi_out2_in1 = get_delta_phi(outer_clu2_phi, inner_clu1_phi);

                            pair<double,double> z_range_info = Get_possible_zvtx( 
                                0., // get_radius(beam_origin.first,beam_origin.second), 
                                {get_radius(inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.x - beam_origin.first, inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.y - beam_origin.second), inner_clu_phi_map[inner_clu1_first][inner_clu1_second].second.z},  // note : unsign radius
                                {get_radius(outer_clu_phi_map[outer_comb_index[id_outer_large_r]][outer_comb_index[id_outer_large_r+1]].second.x - beam_origin.first, outer_clu_phi_map[outer_comb_index[id_outer_large_r]][outer_comb_index[id_outer_large_r+1]].second.y - beam_origin.second), outer_clu_phi_map[outer_comb_index[id_outer_large_r]][outer_comb_index[id_outer_large_r+1]].second.z} // note : unsign radius
                            );

                            // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
                            if (z_range_info.first - z_range_info.second > evt_z.first + evt_z.second || z_range_info.first + z_range_info.second < evt_z.first - evt_z.second) {continue;}

                            r_phi_gr -> SetPoint(r_phi_gr -> GetN(), outer_clu1_radius, outer_clu1_phi);
                            r_phi_gr -> SetPoint(r_phi_gr -> GetN(), outer_clu2_radius, outer_clu2_phi);
                            r_phi_gr -> SetPoint(r_phi_gr -> GetN(), inner_clu1_radius, inner_clu1_phi);
                            r_phi_gr -> Fit(pol0_fit, "NQ");

                            clu3_track_performance.push_back(pol0_fit->GetChisquare()/double(pol0_fit->GetNDF())); // note: the reduced chi2 is used as the performance of the tracklet
                            clu3_track_index.push_back({outer_clu1_first, outer_clu1_second, outer_clu2_first, outer_clu2_second, inner_clu1_first, inner_clu1_second});
                            clu3_track_clu_phi.push_back({pol0_fit->GetParameter(0), (outer_clu1_phi + outer_clu2_phi + inner_clu1_phi)/3. });
                            clu3_track_type.push_back(1); // note : outer-mega and inner single 
                            clu3_track_ReducedChi2_1D -> Fill(pol0_fit->GetChisquare()/double(pol0_fit->GetNDF()));
                            
                            
                            r_phi_gr -> Set(0);
                        }
                    } // note : inner_clu1_loop




                }
            }// note : outer clu2 loop           

        }
    } // note : outer clu1 loop



    // note : try to sort all the proto 4-cluster tracklets
    long long N_proto_clu4_track = clu4_track_performance.size();
    long long ind_clu4_track[clu4_track_performance.size()];
    TMath::Sort(N_proto_clu4_track, &clu4_track_performance[0], ind_clu4_track, false);

    for (int i = 0; i < clu4_track_index.size(); i++)
    {
        int better_track_index = ind_clu4_track[i];

        if (clu4_track_performance[better_track_index] > performance_cut) {continue;}
        if (inner_used_mega_clu[Form("%i_%i", clu4_track_index[better_track_index][0], clu4_track_index[better_track_index][1])] != 0) {continue;}
        if (inner_used_mega_clu[Form("%i_%i", clu4_track_index[better_track_index][2], clu4_track_index[better_track_index][3])] != 0) {continue;}
        if (outer_used_mega_clu[Form("%i_%i", clu4_track_index[better_track_index][4], clu4_track_index[better_track_index][5])] != 0) {continue;}
        if (outer_used_mega_clu[Form("%i_%i", clu4_track_index[better_track_index][6], clu4_track_index[better_track_index][7])] != 0) {continue;}


        // note : the new trackleet is found!
        double inner_clu1_radius = get_radius(inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.x - beam_origin.first, inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.y - beam_origin.second);
        double inner_clu2_radius = get_radius(inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.x - beam_origin.first, inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.y - beam_origin.second);
        double outer_clu1_radius = get_radius(outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.x - beam_origin.first, outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.y - beam_origin.second);
        double outer_clu2_radius = get_radius(outer_clu_phi_map[clu3_track_index[better_track_index][6]][clu3_track_index[better_track_index][7]].second.x - beam_origin.first, outer_clu_phi_map[clu3_track_index[better_track_index][6]][clu3_track_index[better_track_index][7]].second.y - beam_origin.second);

        Get_eta_pair = Get_eta4(
            {0., evt_z.first, evt_z.second},
            {inner_clu1_radius, inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.z},
            {inner_clu2_radius, inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.z},
            {outer_clu1_radius, outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.z},
            {outer_clu2_radius, outer_clu_phi_map[clu3_track_index[better_track_index][6]][clu3_track_index[better_track_index][7]].second.z}
        );  

        mega_track_eta_1D -> Fill(Get_eta_pair.second);
        mega_track_eta.push_back(Get_eta_pair.second);
        cluster4_track_phi_1D -> Fill(clu4_track_clu_phi[better_track_index].first);
        NClu4_track_count += 1;
        

        // note : mark the clusters as used
        inner_used_mega_clu[Form("%i_%i", clu4_track_index[better_track_index][0], clu4_track_index[better_track_index][1])] += 1;
        inner_used_mega_clu[Form("%i_%i", clu4_track_index[better_track_index][2], clu4_track_index[better_track_index][3])] += 1;
        outer_used_mega_clu[Form("%i_%i", clu4_track_index[better_track_index][4], clu4_track_index[better_track_index][5])] += 1;
        outer_used_mega_clu[Form("%i_%i", clu4_track_index[better_track_index][6], clu4_track_index[better_track_index][7])] += 1;

        if (mark_used == false) {continue;}
        inner_clu_phi_map[clu4_track_index[better_track_index][0]][clu4_track_index[better_track_index][1]].first = true;
        inner_clu_phi_map[clu4_track_index[better_track_index][2]][clu4_track_index[better_track_index][3]].first = true;
        outer_clu_phi_map[clu4_track_index[better_track_index][4]][clu4_track_index[better_track_index][5]].first = true;
        outer_clu_phi_map[clu4_track_index[better_track_index][6]][clu4_track_index[better_track_index][7]].first = true;
    }


    // note : try to sort all the proto 3-cluster tracklets with mega inner cluster by the fit performance, 
    long long N_proto_clu3_track = clu3_track_performance.size();
    long long ind_clu3_track[clu3_track_performance.size()];
    TMath::Sort(N_proto_clu3_track, &clu3_track_performance[0], ind_clu3_track, false);

    for (int i = 0; i < clu3_track_index.size(); i++)
    {
        int better_track_index = ind_clu3_track[i];
        if (clu3_track_performance[better_track_index] > performance_cut) {continue;}

        // note : inner mega and outer single
        if (clu3_track_type[better_track_index] == 0){
            
            if (inner_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][0], clu3_track_index[better_track_index][1])] != 0) {continue;}
            if (inner_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][2], clu3_track_index[better_track_index][3])] != 0) {continue;}
            if (outer_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][4], clu3_track_index[better_track_index][5])] != 0) {continue;}

            // note : the new trackleet is found!
            double inner_clu1_radius = get_radius(inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.x - beam_origin.first, inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.y - beam_origin.second);
            double inner_clu2_radius = get_radius(inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.x - beam_origin.first, inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.y - beam_origin.second);
            double outer_clu1_radius = get_radius(outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.x - beam_origin.first, outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.y - beam_origin.second);

            Get_eta_pair = Get_eta3(
                {0., evt_z.first, evt_z.second},
                {inner_clu1_radius, inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.z},
                {inner_clu2_radius, inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.z},
                {outer_clu1_radius, outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.z}
            );  

            mega_track_eta_1D -> Fill(Get_eta_pair.second);
            mega_track_eta.push_back(Get_eta_pair.second);
            cluster3_inner_track_eta_1D -> Fill(Get_eta_pair.second);
            cluster3_inner_track_phi_1D -> Fill(clu3_track_clu_phi[better_track_index].first);
            cluster3_track_phi_1D       -> Fill(clu3_track_clu_phi[better_track_index].first);
            NClu3_track_count += 1;

            // cout<<"the clu indics : "<<clu3_track_index[better_track_index][0]<<", "<<clu3_track_index[better_track_index][1]<<", "<<clu3_track_index[better_track_index][2]<<", "<<clu3_track_index[better_track_index][3]<<", "<<clu3_track_index[better_track_index][4]<<", "<<clu3_track_index[better_track_index][5]
            // <<"\t trackPhi: "<<clu3_track_clu_phi[better_track_index].first<<" radius: "<< inner_clu1_radius<<", "<<inner_clu2_radius<<", "<<outer_clu1_radius<<"\t ---- clu3 proto track performance : "<<clu3_track_performance[better_track_index]<<endl;
            
            // cout<<"inner posXY ("
            // <<inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.x<<", "<<inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.y<<"), ("
            // <<inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.x<<", "<<inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.y<<"), ("
            // <<outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.x<<", "<<outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.y<<")"<<endl;

            // cout<<"inner PosRZ ("<<evt_z.first<<", 0),("
            // <<inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.z<<", "<<inner_clu1_radius<<"), ("
            // <<inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.z<<", "<<inner_clu2_radius<<"), ("
            // <<outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.z<<", "<<outer_clu1_radius<<")"<<endl;

            // ", "<<<inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.y
            // ", "<<<inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.y
            // ", "<<<outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.y
            cout<<"("<<clu3_track_clu_phi[better_track_index].first<<", "<<Get_eta_pair.second<<"), ";
            
            // note : mark the clusters as used
            inner_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][0], clu3_track_index[better_track_index][1])] += 1;
            inner_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][2], clu3_track_index[better_track_index][3])] += 1;
            outer_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][4], clu3_track_index[better_track_index][5])] += 1;

            if (mark_used == false) {continue;}
            inner_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].first = true;
            inner_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].first = true;
            outer_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].first = true;
        }
        else if (clu3_track_type[better_track_index] == 1)
        {
            if (outer_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][0], clu3_track_index[better_track_index][1])] != 0) {continue;}
            if (outer_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][2], clu3_track_index[better_track_index][3])] != 0) {continue;}
            if (inner_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][4], clu3_track_index[better_track_index][5])] != 0) {continue;}

            // note : the new trackleet is found!
            double inner_clu1_radius = get_radius(inner_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.x - beam_origin.first, inner_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.y - beam_origin.second);
            double outer_clu1_radius = get_radius(outer_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.x - beam_origin.first, outer_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.y - beam_origin.second);
            double outer_clu2_radius = get_radius(outer_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.x - beam_origin.first, outer_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.y - beam_origin.second);

            Get_eta_pair = Get_eta3(
                {0., evt_z.first, evt_z.second},
                {inner_clu1_radius, inner_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.z},
                {outer_clu1_radius, outer_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.z},
                {outer_clu2_radius, outer_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.z}
            );

            mega_track_eta_1D -> Fill(Get_eta_pair.second);
            mega_track_eta.push_back(Get_eta_pair.second);
            cluster3_outer_track_eta_1D -> Fill(Get_eta_pair.second);
            cluster3_outer_track_phi_1D -> Fill(clu3_track_clu_phi[better_track_index].first);
            cluster3_track_phi_1D       -> Fill(clu3_track_clu_phi[better_track_index].first);
            NClu3_track_count += 1;

            // cout<<"the clu indics : "<<clu3_track_index[better_track_index][0]<<", "<<clu3_track_index[better_track_index][1]<<", "<<clu3_track_index[better_track_index][2]<<", "<<clu3_track_index[better_track_index][3]<<", "<<clu3_track_index[better_track_index][4]<<", "<<clu3_track_index[better_track_index][5]
            // <<"\t trackPhi: "<<clu3_track_clu_phi[better_track_index].first<<" radius: "<< outer_clu1_radius<<", "<<outer_clu2_radius<<", "<<inner_clu1_radius<<"\t ---- clu3 proto track performance : "<<clu3_track_performance[better_track_index]<<endl;
            
            // cout<<"outer posXY ("
            // <<outer_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.x<<", "<<outer_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.y<<"), ("
            // <<outer_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.x<<", "<<outer_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.y<<"), ("
            // <<inner_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.x<<", "<<inner_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.y<<")"<<endl;

            // cout<<"outer PosRZ ("<<evt_z.first<<", 0),("
            // <<outer_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.z<<", "<<-1 * outer_clu1_radius<<"), ("
            // <<outer_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.z<<", "<<-1 * outer_clu2_radius<<"), ("
            // <<inner_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.z<<", "<<-1 * inner_clu1_radius<<")"<<endl;

            // ", "<<<outer_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].second.y
            // ", "<<<outer_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].second.y
            // ", "<<<inner_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].second.y

            cout<<"("<<clu3_track_clu_phi[better_track_index].first<<", "<<Get_eta_pair.second<<"), ";

            // note : mark the clusters as used
            outer_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][0], clu3_track_index[better_track_index][1])] += 1;
            outer_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][2], clu3_track_index[better_track_index][3])] += 1;
            inner_used_mega_clu[Form("%i_%i", clu3_track_index[better_track_index][4], clu3_track_index[better_track_index][5])] += 1;

            if (mark_used == false) {continue;}
            outer_clu_phi_map[clu3_track_index[better_track_index][0]][clu3_track_index[better_track_index][1]].first = true;
            outer_clu_phi_map[clu3_track_index[better_track_index][2]][clu3_track_index[better_track_index][3]].first = true;
            inner_clu_phi_map[clu3_track_index[better_track_index][4]][clu3_track_index[better_track_index][5]].first = true;
        }

    }


    // note : to check discrepancy between the number of mega tracks found and number of clusters marked as used 
    // self_check(inner_clu_phi_map, outer_clu_phi_map);

    NClu4_track_centrality_2D -> Fill(final_centrality_bin, NClu4_track_count);
    NClu3_track_centrality_2D -> Fill(final_centrality_bin, NClu3_track_count);
}

void MegaTrackFinder::ClearEvt()
{
    inner_used_mega_clu.clear();
    outer_used_mega_clu.clear();

    clu4_track_performance.clear();
    clu4_track_index.clear();
    clu4_track_clu_phi.clear();

    clu3_track_performance.clear();
    clu3_track_index.clear();
    clu3_track_clu_phi.clear();
    clu3_track_type.clear();

    mega_track_eta.clear();

    NClu3_track_count = 0;
    NClu4_track_count = 0;

}

// note : angle_1 = inner clu phi
// note: angle_2 = outer clu phi
double MegaTrackFinder::get_delta_phi(double angle_1, double angle_2)
{
    vector<double> vec_abs = {fabs(angle_1 - angle_2), fabs(angle_1 - angle_2 + 360), fabs(angle_1 - angle_2 - 360)};
    vector<double> vec = {(angle_1 - angle_2), (angle_1 - angle_2 + 360), (angle_1 - angle_2 - 360)};
    return vec[std::distance(vec_abs.begin(), std::min_element(vec_abs.begin(),vec_abs.end()))];
}

double MegaTrackFinder::get_track_phi(double inner_clu_phi_in, double delta_phi_in)
{
    double track_phi = inner_clu_phi_in - (delta_phi_in/2.);
    if (track_phi < 0) {track_phi += 360;}
    else if (track_phi > 360) {track_phi -= 360;}
    else if (track_phi == 360) {track_phi = 0;}
    else {track_phi = track_phi;}
    return track_phi;
}

double MegaTrackFinder::get_offset_clu_phi(double x_in, double y_in)
{
    // if (beam_origin.second == 0) {cout<<"wrong beam position"<<endl; exit(0);}
    return (y_in - beam_origin.second < 0) ? atan2(y_in - beam_origin.second, x_in - beam_origin.first) * (180./TMath::Pi()) + 360 : atan2(y_in - beam_origin.second, x_in - beam_origin.first) * (180./TMath::Pi());
}

pair<double, double> MegaTrackFinder::mirrorPolynomial(double a, double b) {
    // Interchange 'x' and 'y'
    double mirroredA = 1.0 / a;
    double mirroredB = -b / a;

    return {mirroredA, mirroredB};
}

double MegaTrackFinder::grEY_stddev(TGraphErrors * input_grr)
{
    vector<double> input_vector; input_vector.clear();
    for (int i = 0; i < input_grr -> GetN(); i++) {input_vector.push_back( input_grr -> GetPointY(i) );}

    double average = accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());

    double sum_subt = 0;
	for (int ele : input_vector) {sum_subt+=pow((ele-average),2);}
	
	return sqrt(sum_subt/(input_vector.size()-1));
}

pair<double,double> MegaTrackFinder::Get_eta3(vector<double>p0, vector<double>p1, vector<double>p2, vector<double>p3)
{
    // if (track_gr -> GetN() != 0) {cout<<"In INTTEta class, track_gr is not empty, track_gr size : "<<track_gr -> GetN()<<endl; exit(0);}
    
    // if (track_gr -> GetN() != 0) {track_gr -> Set(0);}

    track_gr -> Set(0);
    
    vector<double> r_vec  = {p0[0], p1[0], p2[0], p3[0]}; 
    vector<double> z_vec  = {p0[1], p1[1], p2[1], p3[1]}; 
    vector<double> re_vec = {0, 0, 0, 0}; 
    vector<double> ze_vec = {p0[2], ( fabs( p1[1] ) < 130 ) ? 8. : 10., ( fabs( p2[1] ) < 130 ) ? 8. : 10., ( fabs( p3[1] ) < 130 ) ? 8. : 10.}; 

    // note : swap the r and z, to have easier fitting 
    // note : in principle, Z should be in the x axis, R should be in the Y axis
    for (int i = 0; i < r_vec.size(); i++)
    {
        track_gr -> SetPoint(i,r_vec[i],z_vec[i]);
        track_gr -> SetPointError(i,re_vec[i],ze_vec[i]);

        // cout<<"In INTTEta class, point : "<<i<<" r : "<<r_vec[i]<<" z : "<<z_vec[i]<<" re : "<<re_vec[i]<<" ze : "<<ze_vec[i]<<endl;
    }    
    
    double vertical_line = ( fabs( grEY_stddev(track_gr) ) < 0.00001 ) ? 1 : 0;
    
    if (vertical_line) {return {0,0};}
    else 
    {
        fit_rz -> SetParameters(0,0);

        track_gr -> Fit(fit_rz,"NQ");

        pair<double,double> ax_b = mirrorPolynomial(fit_rz -> GetParameter(1),fit_rz -> GetParameter(0));

        return  {(fit_rz -> GetChisquare() / double(fit_rz -> GetNDF())), -1 * TMath::Log( fabs( tan( atan2(ax_b.first, (ax_b.first > 0) ? 1. : -1. ) / 2 ) ) )};
    }

}


pair<double,double> MegaTrackFinder::Get_eta4(vector<double>p0, vector<double>p1, vector<double>p2, vector<double>p3, vector<double>p4)
{
    // if (track_gr -> GetN() != 0) {cout<<"In INTTEta class, track_gr is not empty, track_gr size : "<<track_gr -> GetN()<<endl; exit(0);}
    
    // if (track_gr -> GetN() != 0) {track_gr -> Set(0);}
    track_gr -> Set(0);
    
    vector<double> r_vec  = {p0[0], p1[0], p2[0], p3[0], p4[0]}; 
    vector<double> z_vec  = {p0[1], p1[1], p2[1], p3[1], p4[1]}; 
    vector<double> re_vec = {0, 0, 0, 0, 0}; 
    vector<double> ze_vec = {p0[2], ( fabs( p1[1] ) < 130 ) ? 8. : 10., ( fabs( p2[1] ) < 130 ) ? 8. : 10., ( fabs( p3[1] ) < 130 ) ? 8. : 10., ( fabs( p4[1] ) < 130 ) ? 8. : 10.}; 

    // note : swap the r and z, to have easier fitting 
    // note : in principle, Z should be in the x axis, R should be in the Y axis
    for (int i = 0; i < r_vec.size(); i++)
    {
        track_gr -> SetPoint(i,r_vec[i],z_vec[i]);
        track_gr -> SetPointError(i,re_vec[i],ze_vec[i]);

        // cout<<"In INTTEta class, point : "<<i<<" r : "<<r_vec[i]<<" z : "<<z_vec[i]<<" re : "<<re_vec[i]<<" ze : "<<ze_vec[i]<<endl;
    }    
    
    double vertical_line = ( fabs( grEY_stddev(track_gr) ) < 0.00001 ) ? 1 : 0;
    
    if (vertical_line) {return {0,0};}
    else 
    {
        fit_rz -> SetParameters(0,0);

        track_gr -> Fit(fit_rz,"NQ");

        pair<double,double> ax_b = mirrorPolynomial(fit_rz -> GetParameter(1),fit_rz -> GetParameter(0));

        return  {(fit_rz -> GetChisquare() / double(fit_rz -> GetNDF())), -1 * TMath::Log( fabs( tan( atan2(ax_b.first, (ax_b.first > 0) ? 1. : -1. ) / 2 ) ) )};
    }

}

void MegaTrackFinder::self_check(vector<vector<pair<bool,clu_info>>> inner_clu_phi_map, vector<vector<pair<bool,clu_info>>> outer_clu_phi_map)
{
    int N_used_clu_inner_check = 0;
    int N_used_clu_outer_check = 0;

    for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++) // note : each phi cell (1 degree)
    {
        // note : N cluster in this phi cell
        for (int inner_phi_clu_i = 0; inner_phi_clu_i < inner_clu_phi_map[inner_phi_i].size(); inner_phi_clu_i++)
        {
            if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].first == true) {N_used_clu_inner_check += 1;}
        }
    }

    for (int outer_phi_i = 0; outer_phi_i < 360; outer_phi_i++) // note : each phi cell (1 degree)
    {
        // note : N cluster in this phi cell
        for (int outer_phi_clu_i = 0; outer_phi_clu_i < outer_clu_phi_map[outer_phi_i].size(); outer_phi_clu_i++)
        {
            if (outer_clu_phi_map[outer_phi_i][outer_phi_clu_i].first == true) {N_used_clu_outer_check += 1;}
        }
    }
    if (NClu4_track_count * 4 + NClu3_track_count * 3 != (N_used_clu_inner_check + N_used_clu_outer_check))
    {
        cout<<"---- N 4-cluster tracklet: "<<NClu4_track_count<<" N 3-cluster tracklet: "<<NClu3_track_count<<endl;
        cout<<"---- N used inner clu : "<<N_used_clu_inner_check<<" N used outer clu : "<<N_used_clu_outer_check<<endl;
        cout<<"---- diff : "<<(N_used_clu_inner_check + N_used_clu_outer_check) - (NClu4_track_count * 4 + NClu3_track_count * 3)<<endl;
    }
}

// double MegaTrackFinder::get_radius(double x, double y)
// {
//     return sqrt(pow(x,2)+pow(y,2));
// }

// pair<double,double> MegaTrackFinder::Get_possible_zvtx(double rvtx, vector<double> p0, vector<double> p1) // note : inner p0, outer p1, vector {r,z}, -> {y,x}
// {
//     vector<double> p0_z_edge = { ( fabs( p0[1] ) < 130 ) ? p0[1] - 8. : p0[1] - 10., ( fabs( p0[1] ) < 130 ) ? p0[1] + 8. : p0[1] + 10.}; // note : vector {left edge, right edge}
//     vector<double> p1_z_edge = { ( fabs( p1[1] ) < 130 ) ? p1[1] - 8. : p1[1] - 10., ( fabs( p1[1] ) < 130 ) ? p1[1] + 8. : p1[1] + 10.}; // note : vector {left edge, right edge}

//     double edge_first  = Get_extrapolation(rvtx,p0_z_edge[0],p0[0],p1_z_edge[1],p1[0]);
//     double edge_second = Get_extrapolation(rvtx,p0_z_edge[1],p0[0],p1_z_edge[0],p1[0]);

//     double mid_point = (edge_first + edge_second) / 2.;
//     double possible_width = fabs(edge_first - edge_second) / 2.;

//     return {mid_point, possible_width}; // note : first : mid point, second : width

// }

#endif 
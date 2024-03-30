pair<double, double> mirrorPolynomial(double a, double b) {
    // Interchange 'x' and 'y'
    double mirroredA = 1.0 / a;
    double mirroredB = -b / a;

    return {mirroredA, mirroredB};
}

struct comb_str{
    int inner_i;
    int outer_i;
};

void INTT_resolution ()
{
    pair<double,double> VTX_rz = { sqrt( pow((-0.457 + 0.0276),2) + pow((2.657 - 0.2814),2) ), -200. };
    pair<double,double> VTX_rz_E = { 0.0035, 15};

    pair<double,double> inner_r = {76.4938, 3.62798}; // note : avg r and stddev
    pair<double,double> outer_r = {101.493, 3.86137};
    
    vector<double> intt_all_z = {
        -226.463, -206.463, -186.463, -166.463, -146.463, -126.299, -110.299, -94.2993, -78.2993, -62.2993, -46.2993, -30.2994, -14.2994, // note : south from U1 to U13
        4.83633, 20.8363, 36.8363, 52.8363, 68.8363, 84.8363, 100.836, 116.836, 137.004, 157.004, 177.004, 197.004, 217.004 // note : north from U13 to U1
    };

    TLine * z_strip = new TLine();
    z_strip -> SetLineWidth(1);
    z_strip -> SetLineColor(1);
    // z_strip -> SetLineStyle(2);

    TLine * fired_strip = new TLine();
    fired_strip -> SetLineWidth(2);
    fired_strip -> SetLineColor(2);

    TCanvas * c1 = new TCanvas("","",800,800);

    TGraph * origin_grr = new TGraph();
    origin_grr -> SetPoint(0,0.,0.);
    origin_grr -> SetMarkerStyle(20);
    origin_grr -> GetXaxis() -> SetLimits(-250,250);
    origin_grr -> GetYaxis() -> SetRangeUser(-250,250);
    origin_grr -> GetXaxis() -> SetTitle("radius");
    origin_grr -> GetYaxis() -> SetTitle("Z axis");
    TF1 * track_fit = new TF1("track_fit","pol1",-250,250);
    track_fit -> SetLineColor(4);

    TLatex *draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    vector<double> eta_vec; eta_vec.clear();
    vector<comb_str> comb_vec; comb_vec.clear();

    vector<double> eta_slope; eta_slope.clear();

    for(int inner_i = 0; inner_i < 26; inner_i++){
        for (int outer_i = 0; outer_i < 26; outer_i++ ){

            track_fit -> SetParameters(0,0);

            double strip_width_inner = ( fabs( intt_all_z[inner_i] ) < 130 ) ? 8. : 10.;
            double strip_width_outer = ( fabs( intt_all_z[outer_i] ) < 130 ) ? 8. : 10.;

            vector<double> gz = {VTX_rz.second, intt_all_z[inner_i], intt_all_z[outer_i]};
            vector<double> gzE = {VTX_rz_E.second, strip_width_inner, strip_width_outer};
            vector<double> gr = {VTX_rz.first, inner_r.first, outer_r.first};
            vector<double> grE = {VTX_rz_E.first, 0.16, 0.16};

            TGraphErrors * grrE = new TGraphErrors(gz.size(), &gr[0], &gz[0], &grE[0], &gzE[0]);
            grrE -> Fit(track_fit,"NQ");

            pair<double,double> ax_b = mirrorPolynomial(track_fit -> GetParameter(1),track_fit -> GetParameter(0));

            if ( (track_fit -> GetChisquare() / double(track_fit -> GetNDF())) < 10 ){
                eta_vec.push_back( -1 * TMath::Log( fabs( tan( atan2(ax_b.first, (ax_b.first > 0) ? 1. : -1. ) / 2 ) ) ) );
                comb_vec.push_back({inner_i,outer_i});
                eta_slope.push_back(track_fit -> GetParameter(1));
            }

            grrE -> Delete();
        }
    }

    int n_size = eta_vec.size();
    int sort_vec_index[n_size];
    TMath::Sort(n_size, &eta_vec[0], sort_vec_index,0);

    

    c1 -> Print(Form("/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut_advanced/INTT_resolution_z%.2fmm.pdf(",VTX_rz.second));
    for(int comb_i = 0; comb_i < eta_vec.size(); comb_i++){
        origin_grr -> Draw("ap");
        track_fit -> SetParameters(0,0);

        double strip_width_inner = ( fabs( intt_all_z[ comb_vec[sort_vec_index[comb_i]].inner_i ] ) < 130 ) ? 8. : 10.;
        double strip_width_outer = ( fabs( intt_all_z[comb_vec[sort_vec_index[comb_i]].outer_i] ) < 130 ) ? 8. : 10.;

        vector<double> gz = {VTX_rz.second, intt_all_z[ comb_vec[sort_vec_index[comb_i]].inner_i ], intt_all_z[comb_vec[sort_vec_index[comb_i]].outer_i]};
        vector<double> gzE = {VTX_rz_E.second, strip_width_inner, strip_width_outer};
        vector<double> gr = {VTX_rz.first, inner_r.first, outer_r.first};
        vector<double> grE = {VTX_rz_E.first, 0.16, 0.16};

        TGraphErrors * grrE = new TGraphErrors(gz.size(), &gr[0], &gz[0], &grE[0], &gzE[0]);
        grrE -> SetMarkerStyle(20);
        grrE -> SetMarkerSize(0.5);
        grrE -> SetMarkerColor(2);
        grrE -> SetLineColor(2);
        grrE -> SetLineWidth(1);

        grrE -> Fit(track_fit,"NQ");


        // note : draw the strip indication line
        for(int i = 0; i < intt_all_z.size(); i++){

            double strip_width = ( fabs( intt_all_z[i] ) < 130 ) ? 8. : 10.;

            z_strip -> DrawLine(inner_r.first, intt_all_z[i] - strip_width, inner_r.first, intt_all_z[i] + strip_width); // note : inner 
            z_strip -> DrawLine(outer_r.first, intt_all_z[i] - strip_width, outer_r.first, intt_all_z[i] + strip_width); // note : outer

            z_strip -> DrawLine(inner_r.first - 3, intt_all_z[i] - strip_width, inner_r.first + 3, intt_all_z[i] - strip_width); // note : inner, the edge
            z_strip -> DrawLine(inner_r.first - 3, intt_all_z[i] + strip_width, inner_r.first + 3, intt_all_z[i] + strip_width); // note : inner, the edge 
            z_strip -> DrawLine(outer_r.first - 3, intt_all_z[i] - strip_width, outer_r.first + 3, intt_all_z[i] - strip_width); // note : outer, the edge
            z_strip -> DrawLine(outer_r.first - 3, intt_all_z[i] + strip_width, outer_r.first + 3, intt_all_z[i] + strip_width); // note : outer, the edge
        }

        grrE -> Draw("p same");
        fired_strip -> DrawLine(inner_r.first, intt_all_z[ comb_vec[sort_vec_index[comb_i]].inner_i ] - strip_width_inner, inner_r.first, intt_all_z[ comb_vec[sort_vec_index[comb_i]].inner_i ] + strip_width_inner); // note : inner 
        fired_strip -> DrawLine(outer_r.first, intt_all_z[comb_vec[sort_vec_index[comb_i]].outer_i] - strip_width_outer, outer_r.first, intt_all_z[comb_vec[sort_vec_index[comb_i]].outer_i] + strip_width_outer); // note : outer
        track_fit -> Draw("l same");

        pair<double,double> ax_b = mirrorPolynomial(track_fit -> GetParameter(1),track_fit -> GetParameter(0));
        

        draw_text -> DrawLatex(0.2, 0.85, Form("Fit #Chi^{2}/NDF : %.3f",(track_fit -> GetChisquare() / double(track_fit -> GetNDF()))));
        draw_text -> DrawLatex(0.2, 0.8, Form("trak eta : %.3f", -1 * TMath::Log( fabs( tan( atan2(ax_b.first, (ax_b.first > 0) ? 1. : -1. ) / 2 ) ) ) ));

        if ( (track_fit -> GetChisquare() / double(track_fit -> GetNDF())) < 4 )
            c1 -> Print(Form("/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut_advanced/INTT_resolution_z%.2fmm.pdf",VTX_rz.second));
        
        c1 -> Clear();
        grrE -> Delete();
    }


    origin_grr -> Draw("ap");
    vector<TF1 *> track_line; track_line.clear();
    for (int i = 0; i < eta_slope.size(); i++){
        track_line.push_back( new TF1("track_line","pol1",-1,250) );
        track_line[i] -> SetParameters(VTX_rz.second, eta_slope[i]); 
        track_line[i] -> Draw("lsame");
    }
    z_strip -> DrawLine(inner_r.first, intt_all_z[0] - 10. ,inner_r.first, intt_all_z[25] + 10 );
    z_strip -> DrawLine(outer_r.first, intt_all_z[0] - 10. ,outer_r.first, intt_all_z[25] + 10 );

    c1 -> Print(Form("/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut_advanced/INTT_resolution_z%.2fmm.pdf)",VTX_rz.second));
    c1 -> Clear();

}
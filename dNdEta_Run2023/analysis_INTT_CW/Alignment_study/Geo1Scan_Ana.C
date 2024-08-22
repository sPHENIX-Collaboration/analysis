#include "GeoScanV2.C"

map<int,int> hist_location(string folder_direction, vector<string> file_list)
{
    int random_seed;
    map<int,int> hist_location_map;

    for (int i = 0; i < file_list.size(); i++)
    {
        cout<<file_list[i]<<endl;
        TFile * _file0 = TFile::Open(Form("%s/%s",folder_direction.c_str(),file_list[i].c_str()));
        TTree * _tree0 = (TTree *) _file0 -> Get("tree_geo_scan");
        _tree0 -> SetBranchStatus("*",0);
        _tree0 -> SetBranchStatus("random_seed",1);
        _tree0 -> SetBranchAddress("random_seed",&random_seed);

        for (int j = 0; j < _tree0 -> GetEntries(); j++)
        {
            _tree0 -> GetEntry(j);
            hist_location_map[random_seed] = i;
        }

        _file0 -> Close();
    }

    return hist_location_map;
}

vector<double> SumTH2FColumnContent(TH2F * hist_in)
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

void Characterize_Pad(TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0)
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

vector<double> Get_coveriance_TH2 (TH2F * hist_in)
{
    double X_mean = hist_in -> GetMean(1);
    double Y_mean = hist_in -> GetMean(2);

    double denominator = 0;
    double variance_x  = 0; 
    double variance_y  = 0;
    double covariance = 0;

    for (int xi = 0; xi < hist_in -> GetNbinsX(); xi++){
        for (int yi = 0; y1 < hist_in -> GetNbinsY(); yi++)
        {
            double cell_weight = hist_in -> GetBinContent(xi+1, yi+1);
            double cell_x = hist_in -> GetXaxis() -> GetBinCenter(xi+1);
            double cell_y = hist_in -> GetYaxis() -> GetBinCenter(yi+1);

            denominator += pow(cell_weight, 2);
            variance_x  += pow(cell_x - X_mean, 2) * pow(cell_weight,2);
            variance_y  += pow(cell_y - Y_mean, 2) * pow(cell_weight,2);
            covariance  += (cell_x - X_mean) * (cell_y - Y_mean) * pow(cell_weight,2);
        }
    }

    return {variance_x/denominator, variance_y/denominator, covariance/denominator};
}

void Geo1Scan_Ana()
{
    // string folder_direction = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR1500_20kEvent_3HotCut_VTXxy_geo3/complete_file";
    // string MC_list_name = "geo_scan3_list.txt";
    string folder_direction = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC_388_000/Geo_scan1_MC_ZF_xyvtx/complete_file";
    string MC_list_name = "file_list.txt";
    int Nfile = -1;
    pair<double,double> origin_fitE = {0.0956607, 0.0101596};
    pair<double,double> origin_posY = {0.189019, 0.0424588};

    TChain * chain_in = new TChain("tree_geo_scan");
    GeoScanV2 * data_in = new GeoScanV2(chain_in, folder_direction, MC_list_name, Nfile);
    cout<<"Total event : "<<chain_in -> GetEntries()<<endl;

    // TH1F * DCA_fitE_hist = new TH1F("","",100,0,0.5);
    // DCA_fitE_hist -> SetTitle("DCA_fitE_hist;Fit error [mm]; Entry");
    // TH1F * angle_diff_fitE_hist = new TH1F("","",100,0,0.1);
    // angle_diff_fitE_hist -> SetTitle("angle_diff_fitE_hist;Fit error [degree]; Entry");

    // TH2F * fitE_correlation = new TH2F("","",100,0,0.5,100,0,0.1);
    // fitE_correlation -> SetTitle("fitE_correlation;DCA fit error; #Delta phi fit error");

    

    int small_random_seed;
    int small_eID;
    double small_check_number;

    TCanvas * c1 = new TCanvas("c1","c1",1800,1200);
    c1 -> Divide(3,2);
    c1 -> Print(Form("%s/good_correct_set.pdf(",folder_direction.c_str()));
    
    TGraph * gx_all = new TGraph();
    gx_all -> SetMarkerStyle(20);
    gx_all -> SetMarkerColor(2);
    gx_all -> SetTitle("gx_all;Ladder ID; #DeltaX [mm]");
    TGraph * gy_all = new TGraph();
    gy_all -> SetMarkerStyle(20);
    gy_all -> SetMarkerColor(2);
    gy_all -> SetTitle("gy_all;Ladder ID; #DeltaY [mm]");

    TGraph * gx_each = new TGraph();
    gx_each -> SetMarkerStyle(20);
    gx_each -> SetMarkerColor(2);
    gx_each -> SetTitle("gx_each;Ladder ID; #DeltaX [mm]");
    TGraph * gy_each = new TGraph();
    gy_each -> SetMarkerStyle(20);
    gy_each -> SetMarkerColor(2);
    gy_each -> SetTitle("gy_each;Ladder ID; #DeltaY [mm]");
    
    TH2F * correct_vtx = new TH2F("","vertex;X axis[mm];Y axis[mm]",100,-2,2,100,0,4);
    TH2F * x_correction = new TH2F("","Correction in X axis;Ladder index;Offset [mm]",14,1,15,100,-0.4,0.4); x_correction -> SetStats(0);
    TH2F * y_correction = new TH2F("","Correction in Y axis;Ladder index;Offset [mm]",14,1,15,100,-0.4,0.4); y_correction -> SetStats(0);

    TLine * coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(16);
    coord_line -> SetLineStyle(2);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    map<int,int> hist_location_map = hist_location(folder_direction, data_in -> file_list);

    TF1 * f2 = new TF1("f2","pol0",-360,360);
    TF1 * f3 = new TF1("f3","pol0",-360,360);
    TF1 * f4 = new TF1("f4","pol0",-360,360);
    TF1 * f5 = new TF1("f5","pol0",-360,360);

    TGraph * h2_profile_graph = new TGraph(); h2_profile_graph -> SetMarkerStyle(20); h2_profile_graph -> SetMarkerSize(0.4);
    TGraph * h3_profile_graph = new TGraph(); h3_profile_graph -> SetMarkerStyle(20); h3_profile_graph -> SetMarkerSize(0.4);
    TGraph * h4_profile_graph = new TGraph(); h4_profile_graph -> SetMarkerStyle(20); h4_profile_graph -> SetMarkerSize(0.4);
    TGraph * h5_profile_graph = new TGraph(); h5_profile_graph -> SetMarkerStyle(20); h5_profile_graph -> SetMarkerSize(0.4);

    vector<double> hist_column_content;


    for (int i = 0; i <chain_in -> GetEntries(); i++ )
    {
        data_in->LoadTree(i);
        data_in->GetEntry(i);

        // if (i % 10000 == 0) cout<<"running event: "<<i<<endl;

        // if (data_in -> DCA_inner_fitE < 0.03 && data_in -> angle_diff_inner_fitE < 0.003 && fabs(data_in -> DCA_inner_fitYpos) < 0.03 && fabs(data_in -> angle_diff_inner_fitYpos) < 0.01)
        if (data_in -> DCA_inner_fitE < 0.1/2. && data_in -> angle_diff_inner_fitE < 0.01/2. && fabs(data_in -> DCA_inner_fitYpos) < 0.1/2. && fabs(data_in -> angle_diff_inner_fitYpos) < 0.025)
        {
            double correct_center_x = accumulate( data_in -> offset_x_vec -> begin(), data_in -> offset_x_vec -> end(), 0.0 ) / double(data_in -> offset_x_vec -> size());
            double correct_center_y = accumulate( data_in -> offset_y_vec -> begin(), data_in -> offset_y_vec -> end(), 0.0 ) / double(data_in -> offset_y_vec -> size());

            correct_vtx -> Fill( (data_in -> vtxX + data_in -> trial_originX)/2. - correct_center_x, (data_in -> vtxY + data_in -> trial_originY)/2. - correct_center_y);

            // cout<<"ladder correction vec : "<<data_in -> offset_x_vec -> size()<<endl;            
            cout<<" "<<endl;

            for (int ladder_i = 0; ladder_i < data_in -> offset_x_vec -> size(); ladder_i++)
            {
                gx_each -> SetPoint(gx_each -> GetN(), ladder_i + 1, data_in -> offset_x_vec -> at(ladder_i) - correct_center_x);
                gy_each -> SetPoint(gy_each -> GetN(), ladder_i + 1, data_in -> offset_y_vec -> at(ladder_i) - correct_center_y);

                x_correction -> Fill(ladder_i + 1, data_in -> offset_x_vec -> at(ladder_i) - correct_center_x);
                y_correction -> Fill(ladder_i + 1, data_in -> offset_y_vec -> at(ladder_i) - correct_center_y);

                gx_all -> SetPoint(gx_all -> GetN(), ladder_i + 1, data_in -> offset_x_vec -> at(ladder_i) - correct_center_x);
                gy_all -> SetPoint(gy_all -> GetN(), ladder_i + 1, data_in -> offset_y_vec -> at(ladder_i) - correct_center_y);
                // cout<<"~~~~ ~~~~ ~~~~ "<< data_in -> offset_x_vec -> at(ladder_i)<<endl;

                cout<<"{"<<data_in -> offset_x_vec -> at(ladder_i)<<", "<<data_in -> offset_y_vec -> at(ladder_i)<<"}, "<<endl;
            }

            cout<<" random_seed: "<<data_in -> random_seed<<" vtxX: "<<data_in -> vtxX<<" vtxY: "<<data_in -> vtxY<<" trial_originX: "<<data_in -> trial_originX<<" trial_originY: "<<data_in -> trial_originY<<endl;
            cout<<"corrected center "<<correct_center_x<<" "<<correct_center_y<<endl;

            c1 -> cd(1);
            gx_each -> GetYaxis() -> SetRangeUser(-0.4,0.4);
            gx_each -> SetTitle(Form("Random seed: %i",data_in -> random_seed));
            gx_each -> Draw("ap");
            coord_line -> DrawLine(gx_each->GetXaxis()->GetXmin(),0,gx_each->GetXaxis()->GetXmax(),0);
            draw_text -> DrawLatex(0.2,0.8,Form("corrected center: %.3f, %.3f mm",correct_center_x, correct_center_y));

            c1 -> cd(2);
            gy_each -> GetYaxis() -> SetRangeUser(-0.4,0.4);
            gy_each -> Draw("ap");
            coord_line -> DrawLine(gy_each->GetXaxis()->GetXmin(),0,gy_each->GetXaxis()->GetXmax(),0);
            draw_text -> DrawLatex(0.2,0.8,Form("Vertex: %.3f, %.3f mm", (data_in -> vtxX + data_in -> trial_originX)/2. - correct_center_x, (data_in -> vtxY + data_in -> trial_originY)/2. - correct_center_y));

            c1 -> cd(3);
            TFile * _file0 = TFile::Open(Form("%s/%s",folder_direction.c_str(),data_in -> file_list[hist_location_map[data_in -> random_seed]].c_str()));
            TH2F * h2 = (TH2F *) _file0->Get(Form("DCA_distance_inner_phi_peak_final_%i",data_in -> random_seed));
            hist_column_content = SumTH2FColumnContent(h2);
            TProfile * h2_profile = h2 -> ProfileX("h2"); 
            for (int i = 0; i < h2_profile->GetNbinsX(); i++){
                if (hist_column_content[i] < 5){continue;}
                h2_profile_graph -> SetPoint(h2_profile_graph->GetN(), h2_profile->GetBinCenter(i+1), h2_profile->GetBinContent(i+1));
            }
            f2 -> SetParameter(0,0);
            h2_profile_graph -> Fit(f2,"NQ");
            h2 -> Draw("colz");
            f2 -> Draw("lsame");
            h2_profile_graph -> Draw("p same");
            draw_text -> DrawLatex(0.2,0.8,Form("mean: %.6f",f2->GetParameter(0)));
            draw_text -> DrawLatex(0.2,0.75,Form("fitE: %.6f",f2->GetParError(0)));
            draw_text -> DrawLatex(0.2,0.7,Form("Reduced #chi^{2}: %.6f",f2->GetChisquare()/double(f2->GetNDF())));




            c1 -> cd(4);
            TH2F * h3 = (TH2F *) _file0->Get(Form("angle_diff_inner_phi_peak_final_%i",data_in -> random_seed));
            hist_column_content = SumTH2FColumnContent(h3);
            TProfile * h3_profile = h3 -> ProfileX("h3"); 
            for (int i = 0; i < h3_profile->GetNbinsX(); i++){
                if (hist_column_content[i] < 5){continue;}
                h3_profile_graph -> SetPoint(h3_profile_graph->GetN(), h3_profile->GetBinCenter(i+1), h3_profile->GetBinContent(i+1));
            }
            f3 -> SetParameter(0,0);
            h3_profile_graph -> Fit(f3,"NQ");
            h3 -> Draw("colz");
            f3 -> Draw("lsame");
            h3_profile_graph -> Draw("p same");
            draw_text -> DrawLatex(0.2,0.8,Form("mean: %.6f",f3->GetParameter(0)));
            draw_text -> DrawLatex(0.2,0.75,Form("fitE: %.6f",f3->GetParError(0)));
            draw_text -> DrawLatex(0.2,0.7,Form("Reduced #chi^{2}: %.6f",f3->GetChisquare()/double(f3->GetNDF())));




            c1 -> cd(5);
            TH2F * h4 = (TH2F *) _file0->Get(Form("DCA_distance_outer_phi_peak_final_%i",data_in -> random_seed));
            hist_column_content = SumTH2FColumnContent(h4);
            TProfile * h4_profile = h4 -> ProfileX("h4"); 
            for (int i = 0; i < h4_profile->GetNbinsX(); i++){
                if (hist_column_content[i] < 5){continue;}
                h4_profile_graph -> SetPoint(h4_profile_graph->GetN(), h4_profile->GetBinCenter(i+1), h4_profile->GetBinContent(i+1));
            }
            f4 -> SetParameter(0,0);
            h4_profile_graph -> Fit(f4,"NQ");
            h4 -> Draw("colz");
            f4 -> Draw("lsame");
            h4_profile_graph -> Draw("p same");
            draw_text -> DrawLatex(0.2,0.8,Form("mean: %.6f",f4->GetParameter(0)));
            draw_text -> DrawLatex(0.2,0.75,Form("fitE: %.6f",f4->GetParError(0)));
            draw_text -> DrawLatex(0.2,0.7,Form("Reduced #chi^{2}: %.6f",f4->GetChisquare()/double(f4->GetNDF())));




            c1 -> cd(6);
            TH2F * h5 = (TH2F *) _file0->Get(Form("angle_diff_outer_phi_peak_final_%i",data_in -> random_seed));
            hist_column_content = SumTH2FColumnContent(h5);
            TProfile * h5_profile = h5 -> ProfileX("h5"); 
            for (int i = 0; i < h5_profile->GetNbinsX(); i++){
                if (hist_column_content[i] < 5){continue;}
                h5_profile_graph -> SetPoint(h5_profile_graph->GetN(), h5_profile->GetBinCenter(i+1), h5_profile->GetBinContent(i+1));
            }
            f5 -> SetParameter(0,0);
            h5_profile_graph -> Fit(f5,"NQ");
            h5 -> Draw("colz");
            f5 -> Draw("lsame");
            h5_profile_graph -> Draw("p same");
            draw_text -> DrawLatex(0.2,0.8,Form("mean: %.6f",f5->GetParameter(0)));
            draw_text -> DrawLatex(0.2,0.75,Form("fitE: %.6f",f5->GetParError(0)));
            draw_text -> DrawLatex(0.2,0.7,Form("Reduced #chi^{2}: %.6f",f5->GetChisquare()/double(f5->GetNDF())));

            c1 -> Print(Form("%s/good_correct_set.pdf",folder_direction.c_str()));

            h2_profile_graph -> Set(0);
            h3_profile_graph -> Set(0);
            h4_profile_graph -> Set(0);
            h5_profile_graph -> Set(0);

            c1 -> Clear();
            c1 -> Divide(3,2);
        }



        // fitE_correlation -> Fill(data_in -> DCA_inner_fitE, data_in -> angle_diff_inner_fitE);
        // DCA_fitE_hist -> Fill(data_in -> DCA_inner_fitE);
        // angle_diff_inner_fitE_hist -> Fill(data_in -> angle_diff_inner_fitE);

        if (i == 0)
        {
            small_eID = i;
            small_random_seed = data_in -> random_seed;
            small_check_number = data_in -> DCA_inner_fitE + data_in -> angle_diff_inner_fitE * 10.;
        }
        else 
        {
            if (data_in -> DCA_inner_fitE + data_in -> angle_diff_inner_fitE * 10. < small_check_number)
            {
                small_eID = i;
                small_random_seed = data_in -> random_seed;
                small_check_number = data_in -> DCA_inner_fitE + data_in -> angle_diff_inner_fitE * 10.;
            }
        }

        
        gx_each -> Set(0);
        gy_each -> Set(0);

        // if (data_in -> DCA_inner_fitE < origin_fitE.first && data_in -> angle_diff_inner_fitE < origin_fitE.second)
        // {
        //     cout<<"update origin_fitE: "<<data_in -> DCA_inner_fitE<<" "<<data_in -> angle_diff_inner_fitE<<" random seed: "<<data_in->random_seed<<endl;

        //     // origin_fitE = {data_in -> DCA_inner_fitE, data_in -> angle_diff_inner_fitE};
        //     // origin_posY = {data_in -> DCA_inner_fitYpos, data_in -> angle_diff_fitYpos};
        //     // cout<<"update origin_fitE: "<<origin_fitE.first<<" "<<origin_fitE.second<<endl;
        //     // cout<<"update origin_posY: "<<origin_posY.first<<" "<<origin_posY.second<<endl;
        // }


        // cout<<"event: "<<i<<" ======================= ======================= ======================= ======================="<<endl;
        // cout<<data_in -> offset_x_vec->at(0)<<" "<<data_in -> offset_x_vec->at(1)<<" "<<data_in -> offset_x_vec->at(2)<<" "<<data_in -> offset_x_vec->at(3)<<" "<<data_in -> offset_x_vec->at(4)<<" "<<data_in -> offset_x_vec->at(5)<<" "<<data_in -> offset_x_vec->at(6)<<" "<<data_in -> offset_x_vec->at(7)<<" "<<data_in -> offset_x_vec->at(8)<<" "<<data_in -> offset_x_vec->at(9)<<" "<<data_in -> offset_x_vec->at(10)<<" "<<data_in -> offset_x_vec->at(11)<<" "<<data_in -> offset_x_vec->at(12)<<" "<<data_in -> offset_x_vec->at(13)<<endl;
        // cout<<data_in -> offset_y_vec->at(0)<<" "<<data_in -> offset_y_vec->at(1)<<" "<<data_in -> offset_y_vec->at(2)<<" "<<data_in -> offset_y_vec->at(3)<<" "<<data_in -> offset_y_vec->at(4)<<" "<<data_in -> offset_y_vec->at(5)<<" "<<data_in -> offset_y_vec->at(6)<<" "<<data_in -> offset_y_vec->at(7)<<" "<<data_in -> offset_y_vec->at(8)<<" "<<data_in -> offset_y_vec->at(9)<<" "<<data_in -> offset_y_vec->at(10)<<" "<<data_in -> offset_y_vec->at(11)<<" "<<data_in -> offset_y_vec->at(12)<<" "<<data_in -> offset_y_vec->at(13)<<endl;
        // cout<<" DCA_inner_fitYpos: "<<data_in ->DCA_inner_fitYpos <<" DCA_inner_fitE: "<<data_in ->DCA_inner_fitE <<" angle_diff_fitYpos: "<<data_in ->angle_diff_fitYpos <<" angle_diff_inner_fitE: "<<data_in ->angle_diff_inner_fitE <<" random_seed: "<<data_in ->random_seed<<endl;


        // for (int i1 = 0; i1 < data_in -> offset_x_vec -> size(); i1++)
        // {
        //     cout<<data_in -> offset_x_vec->at(i1)<<" "<<data_in -> offset_y_vec->at(i1)<<endl;
        // }
    }

    c1 -> Print(Form("%s/good_correct_set.pdf)",folder_direction.c_str()));

    TCanvas * c2 = new TCanvas("c2","c2",650,600);
    c2 -> cd();
    TPad * pad_plot = new TPad(Form("pad_plot"), "", 0.0, 0.0, 1.0, 1.0);
    Characterize_Pad(pad_plot, 0.15, 0.1, 0.1, 0.2, 0, 0);
    pad_plot -> Draw();

    pad_plot -> cd();
    correct_vtx -> Draw("colz");
    c2 -> Print(Form("%s/correct_vtx.pdf",folder_direction.c_str()));
    // c2 -> Clear();
    pad_plot -> Clear();

    pad_plot -> cd();
    x_correction -> Draw("colz");
    c2 -> Print(Form("%s/x_correction.pdf",folder_direction.c_str()));
    // c2 -> Clear();
    pad_plot -> Clear();

    pad_plot -> cd();
    y_correction -> Draw("colz");
    c2 -> Print(Form("%s/y_correction.pdf",folder_direction.c_str()));
    // c2 -> Clear();
    pad_plot -> Clear();


    // cout<<"update origin_fitE: "<<data_in -> DCA_inner_fitE<<" "<<data_in -> angle_diff_inner_fitE<<" random seed: "<<data_in->random_seed<<endl;

    cout<<" "<<endl;
    cout<<"!!!! the best one: "<<small_check_number<<" with the random seed: "<<small_random_seed<<endl;
    data_in->LoadTree(small_eID);
    data_in->GetEntry(small_eID);

    cout<<" random_seed: "<<data_in -> random_seed<<" vtxX: "<< data_in -> vtxX<<" vtxY: "<< data_in -> vtxY<<" trial_originX: "<< data_in -> trial_originX<<" trial_originY: "<< data_in -> trial_originY<<endl;
    cout<<" DCA_inner_fitYpos: "<<data_in -> DCA_inner_fitYpos <<" DCA_inner_fitE: "<<data_in ->  DCA_inner_fitE <<" angle_diff_inner_fitYpos: "<< data_in -> angle_diff_inner_fitYpos <<" angle_diff_inner_fitE: "<< data_in -> angle_diff_inner_fitE <<endl;
    cout<<" DCA_outer_fitYpos: "<< data_in -> DCA_outer_fitYpos <<" DCA_outer_fitE: "<< data_in -> DCA_outer_fitE <<" angle_diff_outer_fitYpos: "<< data_in -> angle_diff_outer_fitYpos <<" angle_diff_outer_fitE: "<< data_in -> angle_diff_outer_fitE<<endl;
    

    // TCanvas * c1 = new TCanvas("c1","c1",650,600);
    // c1 -> cd();
    // fitE_correlation -> Draw("colz");
    // c1 -> Print(Form("%s/scan_study/fitE_correlation.pdf",folder_direction.c_str()));
    // c1 -> Clear();

    // DCA_fitE_hist -> Draw("hist");
    // c1 -> Print(Form("%s/scan_study/DCA_fitE_hist.pdf",folder_direction.c_str()));
    // c1 -> Clear();

    // angle_diff_fitE_hist -> Draw("hist");
    // c1 -> Print(Form("%s/scan_study/angle_diff_fitE_hist.pdf",folder_direction.c_str()));
    // c1 -> Clear();


}   
#include "plot_evt_zvtx.h"

// todo: check the used namespace

// note : the units of the input variables should still be mm
plot_evt_zvtx::plot_evt_zvtx(
    string input_directory_in, // = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/evt_vtxZ/complete_file";
    string input_file_name_in, // = "merged_file.root";
    string out_folder_directory_in, // = input_directory + "/merged_result";
    double required_zvtx_diff_in, // = 10.; // note : unit : mm
    int zvtx_dist_NClus_cut_in, // = 1000;
    int run_type_in, // = 0; // note : 1 = MC, 0 = data
    double zvtx_range_l_in = -500,
    double zvtx_range_r_in = 100,
    double zvtx_range_zoomin_l_in = -300,
    double zvtx_range_zoomin_r_in = -100,   
    int unit_tag_in = 1 // note : 1 = cm, 0 = mm
):
input_directory(input_directory_in),
input_file_name(input_file_name_in),
out_folder_directory(out_folder_directory_in),
required_zvtx_diff(required_zvtx_diff_in),
zvtx_dist_NClus_cut(zvtx_dist_NClus_cut_in),
run_type(run_type_in),
zvtx_range_l(zvtx_range_l_in),
zvtx_range_r(zvtx_range_r_in),
zvtx_range_zoomin_l(zvtx_range_zoomin_l_in),
zvtx_range_zoomin_r(zvtx_range_zoomin_r_in),
unit_tag(unit_tag_in)
{
    plot_text = (run_type == 1)? "Simulation" : "Work-in-progress";
    unit_correction = (unit_tag == 1) ? 0.1 : 1.;
    unit_text = (unit_tag == 1) ? "cm" : "mm";

    cout<<"In plot_evt_zvtx, unit_tag: "<<unit_tag<<" unit_correction: "<<unit_correction<<endl;

    zvtx_range_l = zvtx_range_l * unit_correction;
    zvtx_range_r = zvtx_range_r * unit_correction;
    zvtx_range_zoomin_l = zvtx_range_zoomin_l * unit_correction;
    zvtx_range_zoomin_r = zvtx_range_zoomin_r * unit_correction;
    required_zvtx_diff = required_zvtx_diff * unit_correction;

    // cout<<"zvtx_range_l: "<<zvtx_range_l<<endl;
    // cout<<"zvtx_range_r: "<<zvtx_range_r<<endl;
    // cout<<"zvtx_range_zoomin_l: "<<zvtx_range_zoomin_l<<endl;
    // cout<<"zvtx_range_zoomin_r: "<<zvtx_range_zoomin_r<<endl;
    cout<<"In plot_evt_zvtx, check the required_zvtx_diff: "<<required_zvtx_diff<<Form("[%s]",unit_text.c_str())<<endl;
    for (int i = 0; i < z_region.size(); i++) {z_region[i] = z_region[i] * unit_correction;}

    if(std::filesystem::exists( Form("%s/%s", input_directory.c_str(), input_file_name.c_str()) ) == false){
        // note : no merged root file, make one
        cout<<"--- no merged file found, so, make one"<<endl;
        system(Form("hadd %s/%s %s/evtZ_00*/INTT_zvtx.root", input_directory.c_str(), input_file_name.c_str(), input_directory.c_str()));
    }

    // note : to generate the merged_result folder
    if(std::filesystem::exists(Form("%s",out_folder_directory.c_str())) == false){
        system(Form("mkdir %s", out_folder_directory.c_str()));
        cout<<"----------- check folder exists -----------"<<endl;
        system(Form("ls %s", out_folder_directory.c_str()));
    }
    else 
    {
        cout<<"----------- folder exists already -----------"<<endl;
        system(Form("ls %s", out_folder_directory.c_str()));
    }

    SetsPhenixStyle();
    c1 = new TCanvas("c1","c1",950,800);
    c1 -> cd();

    TFile_init();
    Hist_init();
    Rest_init();

    LoopEvent();
    PrintPlot();    
}

void plot_evt_zvtx::TFile_init()
{
    file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_file_name.c_str()));
    tree = (TTree*)file_in -> Get("tree_Z");
    tree -> SetBranchStatus("*", 0);
    tree -> SetBranchStatus("nclu_inner",1);
    tree -> SetBranchStatus("nclu_outer",1);
    tree -> SetBranchStatus("LB_Gaus_Mean_mean", 1);
    tree -> SetBranchStatus("good_zvtx_tag", 1);
    tree -> SetBranchStatus("Centrality_float",1);
    tree -> SetBranchStatus("MC_true_zvtx",1);
    tree -> SetBranchStatus("is_min_bias",1);
    tree -> SetBranchStatus("is_min_bias_wozdc",1);
    tree -> SetBranchStatus("eID",1);
    tree -> SetBranchStatus("MBD_reco_z",1);
    tree -> SetBranchStatus("MBD_north_charge_sum",1);
    tree -> SetBranchStatus("MBD_south_charge_sum",1);
    tree -> SetBranchStatus("LB_Gaus_Width_width",1);
    tree -> SetBranchStatus("LB_cut_peak_width",1);

    tree -> SetBranchAddress("nclu_inner", &in_nclu_inner);
    tree -> SetBranchAddress("nclu_outer", &in_nclu_outer);
    tree -> SetBranchAddress("LB_Gaus_Mean_mean", &INTT_reco_zvtx);
    tree -> SetBranchAddress("good_zvtx_tag", &good_zvtx_tag);
    tree -> SetBranchAddress("Centrality_float", &Centrality_float);
    tree -> SetBranchAddress("is_min_bias", &is_min_bias);
    tree -> SetBranchAddress("is_min_bias_wozdc", &is_min_bias_wozdc);
    tree -> SetBranchAddress("MC_true_zvtx", &MC_true_zvtx);
    tree -> SetBranchAddress("eID", &eID_in);
    tree -> SetBranchAddress("MBD_reco_z", &MBD_reco_z);
    tree -> SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
    tree -> SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
    tree -> SetBranchAddress("LB_Gaus_Width_width", &INTT_zvtx_reco_gaus_width);
    tree -> SetBranchAddress("LB_cut_peak_width", &INTT_zvtx_reco_hist_cut_width);

    cout<<"tree entries : "<<tree->GetEntries()<<endl;
}

void plot_evt_zvtx::Hist_init()
{
    cout<<"in Hist_init, zvtx_range_l: "<<zvtx_range_l<<endl;
    cout<<"in Hist_init, zvtx_range_r: "<<zvtx_range_r<<endl;
    cout<<"in Hist_init, zvtx_range_zoomin_l: "<<zvtx_range_zoomin_l<<endl;
    cout<<"in Hist_init, zvtx_range_zoomin_r: "<<zvtx_range_zoomin_r<<endl;
    // for (int i = 0; i < z_region.size(); i++) {cout<<z_region[i]<<endl;}

    reco_Zvtx_dist = new TH1F(
        "",
        Form("reco_Zvtx_dist;Reco. Zvtx [%s];Entry",unit_text.c_str()),
        200, 
        zvtx_range_l, 
        zvtx_range_r
    );
    reco_Zvtx_dist -> SetMarkerColor(1);
    reco_Zvtx_dist -> GetXaxis() -> SetNdivisions(505);

    NClus_intt_mbd_diff_corre = new TH2F(
        "",
        Form("NClus_intt_mbd_diff_corre;Number of INTT clusters; INTT_{z} - MBD_{z} [%s]", unit_text.c_str()),
        200,
        0,
        8500,
        200,
        -100 * unit_correction,
        100 * unit_correction
    );
    NClus_intt_mbd_diff_corre -> GetXaxis() -> SetNdivisions(505);

    mbdz_intt_mbd_diff_corre_cut = new TH2F(
        "",
        Form("mbdz_intt_mbd_diff_corre_cut;MBD_{z} [%s]; INTT_{z} - MBD_{z} [%s]", unit_text.c_str(), unit_text.c_str()),
        200,
        zvtx_range_l,
        zvtx_range_r,
        200,
        -100 * unit_correction,
        100 * unit_correction
    );
    mbdz_intt_mbd_diff_corre_cut -> GetXaxis() -> SetNdivisions(705);
    

    intt_mbd_reco_z_corre = new TH2F(
        "",
        Form("intt_mbd_reco_z_corre;INTT zvtx [%s]; MBD zvtx [%s]", unit_text.c_str(), unit_text.c_str()), 
        200,
        zvtx_range_l, 
        zvtx_range_r, 
        200,
        zvtx_range_l, 
        zvtx_range_r
    );
    intt_mbd_reco_z_corre -> GetXaxis() -> SetNdivisions(505);

    intt_mbd_reco_z_corre_cut = new TH2F(
        "",
        Form("intt_mbd_reco_z_corre_cut;INTT zvtx [%s]; MBD zvtx [%s]", unit_text.c_str(), unit_text.c_str()), 
        intt_mbd_reco_z_corre -> GetNbinsX(),
        intt_mbd_reco_z_corre -> GetXaxis() -> GetXmin(),
        intt_mbd_reco_z_corre -> GetXaxis() -> GetXmax(),
        intt_mbd_reco_z_corre -> GetNbinsY(),
        intt_mbd_reco_z_corre -> GetYaxis() -> GetXmin(),
        intt_mbd_reco_z_corre -> GetYaxis() -> GetXmax()
    );
    intt_mbd_reco_z_corre_cut -> GetXaxis() -> SetNdivisions(505);

    intt_mbd_reco_z_diff  = new TH1F(
        "",
        Form("intt_mbd_reco_z_diff; INTT_{z} - MBD_{z} [%s];Entry", unit_text.c_str()),
        200,
        -100 * unit_correction,
        100 * unit_correction
    );
    intt_mbd_reco_z_diff -> GetXaxis() -> SetNdivisions(505);

    reco_Zvtx_dist_zoomin = new TH1F(
        "",
        Form("reco_Zvtx_dist_zoomin;Reco. Zvtx [%s];Entry", unit_text.c_str()),
        200,
        zvtx_range_zoomin_l,
        zvtx_range_zoomin_r
    );
    reco_Zvtx_dist_zoomin -> SetMarkerColor(1);
    reco_Zvtx_dist_zoomin -> GetXaxis() -> SetNdivisions(505);

    MBDz_MBDChargeAssy  = new TH2F(
        "",
        Form("MBDz_MBDChargeAssy;MBD z [%s];(north-south)/(north+south)", unit_text.c_str()),
        200,
        zvtx_range_l,
        zvtx_range_r, 
        200, 
        -1.2, 
        1.2
    ); 
    MBDz_MBDChargeAssy -> GetXaxis() -> SetNdivisions(505);

    INTTz_MBDChargeAssy = new TH2F(
        "",
        Form("INTTz_MBDChargeAssy;INTT z [%s];(north-south)/(north+south)", unit_text.c_str()),
        200,
        zvtx_range_l, 
        zvtx_range_r, 
        200, 
        -1.2, 
        1.2
    ); 
    INTTz_MBDChargeAssy -> GetXaxis() -> SetNdivisions(505);


    // note : no inclusive
    reco_Zvtx_dist_Mbin.clear();
    reco_Zvtx_dist_Mbin_zoomin.clear();
    for (int i = 0; i < centrality_region.size() - 1; i++){
        reco_Zvtx_dist_Mbin.push_back(new TH1F("",Form("reco_Zvtx_dist;Reco. Zvtx [%s];Entry",unit_text.c_str()),200, zvtx_range_l, zvtx_range_r));
        reco_Zvtx_dist_Mbin[i] -> SetMarkerColor( TColor::GetColor(color_code[i % int(color_code.size())].c_str()) );
        reco_Zvtx_dist_Mbin[i] -> SetLineColor  ( TColor::GetColor(color_code[i % int(color_code.size())].c_str()) );
        if (i >= int(color_code.size())) {reco_Zvtx_dist_Mbin[i] -> SetMarkerStyle(21);}

        reco_Zvtx_dist_Mbin_zoomin.push_back(new TH1F("",Form("reco_Zvtx_dist;Reco. Zvtx [%s];Entry",unit_text.c_str()),200,zvtx_range_zoomin_l, zvtx_range_zoomin_r));
        reco_Zvtx_dist_Mbin_zoomin[i] -> SetMarkerColor(TColor::GetColor(color_code[i % int(color_code.size())].c_str()) );
        reco_Zvtx_dist_Mbin_zoomin[i] -> SetLineColor  (TColor::GetColor(color_code[i % int(color_code.size())].c_str()) );
        if (i >= int(color_code.size())) {reco_Zvtx_dist_Mbin_zoomin[i] -> SetMarkerStyle(21);}
    }

    true_Zvtx_dist = new TH1F("",Form("true_Zvtx_dist;True Zvtx [%s];Entry", unit_text.c_str()),200, zvtx_range_l, zvtx_range_r);
    true_Zvtx_dist -> GetXaxis() -> SetNdivisions(505);
    true_Zvtx_dist -> SetLineColor(2);


    Z_resolution.clear();
    for (int i = 0; i < centrality_region.size(); i++)
    {
        Z_resolution.push_back(new TH1F("",Form("Z_resolution Mbin %i;#DeltaZ (Reco. - True) [%s];Entry", i, unit_text.c_str()),200, -25 * unit_correction, 25 * unit_correction)); 
        Z_resolution[i] -> GetXaxis() -> SetNdivisions(505);
    }

    // note : required zvtx tag
    true_zvtx_Mbin_Truezpos = new TH2F("",Form("true_zvtx_Mbin_Truezpos;MBin;True zvtx [%s]", unit_text.c_str()), centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]); 
    reco_zvtx_Mbin_Truezpos = new TH2F("",Form("reco_zvtx_Mbin_Truezpos;MBin;True zvtx [%s]", unit_text.c_str()), centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]); 
    ratio_Mbin_Truezpos     = new TH2F("",Form("ratio_Mbin_Truezpos;MBin;True zvtx [%s]", unit_text.c_str()),     centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]); 
    
    Z_resolution_Nclu = new TH2F("",Form("Z_resolution_Nclu;# of clusters;#DeltaZ (Reco. - True) [%s]", unit_text.c_str()),200,0,6000,200,-100 * unit_correction,100 * unit_correction);
    Z_resolution_Nclu -> GetXaxis() -> SetNdivisions(505);

    Z_resolution_pos = new TH2F("",Form("Z_resolution_pos;True Zvtx [%s];#DeltaZ (Reco. - True) [%s]", unit_text.c_str(), unit_text.c_str()), 200, zvtx_range_l, zvtx_range_r, 200,-50 * unit_correction, 50 * unit_correction);
    Z_resolution_pos -> GetXaxis() -> SetNdivisions(505);

    Z_resolution_pos_cut = new TH2F("",Form("Z_resolution_pos_cut;True Zvtx [%s];#DeltaZ (Reco. - True) [%s]", unit_text.c_str(), unit_text.c_str()),200, zvtx_range_l, zvtx_range_r, 200, -50 * unit_correction, 50 * unit_correction);
    Z_resolution_pos_cut -> GetXaxis() -> SetNdivisions(505);

    // Z_resolution = new TH1F("","Z_resolution;#DeltaZ (Reco. - True) [%s];Entry",200,-30,30);
    // Z_resolution -> GetXaxis() -> SetNdivisions(505);

    Z_correlation = new TH2F("",Form("Z_correlation;True Zvtx [%s];Reco. Zvtx [%s]", unit_text.c_str(), unit_text.c_str()),200, zvtx_range_l, zvtx_range_r, 200, zvtx_range_l, zvtx_range_r);
    Z_correlation -> GetXaxis() -> SetNdivisions(505);

    LB_hist_gaus_width = new TH1F("",Form("LB_hist_gaus_width;Width [%s];Entry",unit_text.c_str()), 100, 0, 120 * unit_correction);
    LB_hist_cut_group_width = new TH1F("",Form("LB_hist_cut_group_width;Width [%s];Entry",unit_text.c_str()), 100, 0, 200 * unit_correction);

    high_NClus_Z_resolution = new TH1F(
        "",
        Form("high_NClus_Z_resolution;#DeltaZ (Reco. - True) [%s];Entry", unit_text.c_str()),
        200,
        -25 * unit_correction, 
        25 * unit_correction
    );
}

void plot_evt_zvtx::Rest_init()
{
    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.025);

    leg = new TLegend(0.65,0.75,0.8,0.9);
    leg -> SetTextSize(0.035);

    leg_inc = new TLegend(0.2,0.5,0.35,0.9);
    leg_inc -> SetTextSize(0.030);

    leg_single = new TLegend(0.2,0.75,0.35,0.9);
    leg_single -> SetTextSize(0.030);

    gaus_fit_2 = new TF1("gaus_fit_2", gaus_func, Z_resolution[0]->GetXaxis()->GetXmin(), Z_resolution[0]->GetXaxis()->GetXmax(),4);
    gaus_fit_2 -> SetLineColor(2);
    gaus_fit_2 -> SetLineWidth(2);
    gaus_fit_2 -> SetNpx(1000);

    linear_fit = new TF1("linear_fit","pol1", -500, 500);
    linear_fit -> SetLineColor(2);
    linear_fit -> SetLineWidth(1);
    linear_fit -> SetLineStyle(9);


    Z_resolution_centrality_gr_fit = new TGraphErrors();
    Z_resolution_centrality_gr_fit -> SetMarkerStyle(20);
    Z_resolution_centrality_gr_fit -> SetMarkerSize(1.5);
    Z_resolution_centrality_gr_fit -> GetXaxis()->SetTitle("Centrality bin"); 
    Z_resolution_centrality_gr_fit -> GetYaxis()->SetTitle(Form("#DeltaZ dist. width [%s]", unit_text.c_str()));

    Z_mean_centrality_gr_fit = new TGraphErrors();
    Z_mean_centrality_gr_fit -> SetMarkerStyle(20);
    Z_mean_centrality_gr_fit -> SetMarkerSize(1.5);
    Z_mean_centrality_gr_fit -> GetXaxis()->SetTitle("Centrality bin"); 
    Z_mean_centrality_gr_fit -> GetYaxis()->SetTitle(Form("#DeltaZ dist. mean [%s]", unit_text.c_str()));

    Z_resolution_centrality_gr_num = new TGraphErrors();
    Z_resolution_centrality_gr_num -> SetMarkerStyle(20);
    Z_resolution_centrality_gr_num -> SetMarkerSize(1.5);
    Z_resolution_centrality_gr_num -> GetXaxis()->SetTitle("Centrality bin"); 
    Z_resolution_centrality_gr_num -> GetYaxis()->SetTitle(Form("#DeltaZ dist. width [%s]", unit_text.c_str()));

    Z_mean_centrality_gr_num = new TGraphErrors();
    Z_mean_centrality_gr_num -> SetMarkerStyle(20);
    Z_mean_centrality_gr_num -> SetMarkerSize(1.5);
    Z_mean_centrality_gr_num -> GetXaxis()->SetTitle("Centrality bin"); 
    Z_mean_centrality_gr_num -> GetYaxis()->SetTitle(Form("#DeltaZ dist. mean [%s]", unit_text.c_str()));

    coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(16);
    coord_line -> SetLineStyle(2);

    cout<<"confirmation, centrality_region size : "<<centrality_region.size()<<endl;

}

void plot_evt_zvtx::LoopEvent()
{
    for (int event_i = 0; event_i < tree -> GetEntries(); event_i++)
    {
        tree -> GetEntry(event_i);

        if (event_i % 4000 == 0) {cout<<"processing, event_i : "<<event_i<<endl;}
        
        // note : MC, but the event is not the minimum bias event
        if (run_type == 1 && is_min_bias_wozdc == 0) {continue;}
        // todo: here I use is_min_bias_wozdc for data
        // note : data, but the event is not the minomum bias event (wozdc)
        if (run_type == 0 && is_min_bias_wozdc == 0) {continue;}

        // note : to get rid of the nan value
        if (Centrality_float != Centrality_float) {continue;}
        if (MBD_reco_z != MBD_reco_z) {continue;}

        // in_nclu_inner
        // in_nclu_outer
        // good_zvtx_tag
        // Centrality_float
        // is_min_bias
        // is_min_bias_wozdc
        // eID_in
        // MBD_north_charge_sum
        // MBD_south_charge_sum
        INTT_reco_zvtx = INTT_reco_zvtx * unit_correction;
        MC_true_zvtx = MC_true_zvtx * unit_correction;
        MBD_reco_z = MBD_reco_z * unit_correction;
        INTT_zvtx_reco_gaus_width = INTT_zvtx_reco_gaus_width * unit_correction;
        INTT_zvtx_reco_hist_cut_width = INTT_zvtx_reco_hist_cut_width * unit_correction;

        // todo : change the namespace upon necessity
        int Centrality_bin = ana_map_version::convert_centrality_bin(Centrality_float);
        // if (Centrality_float>0.89) {cout<<Centrality_float<<" "<<Centrality_bin<<endl;}
        double MBD_charge_assy = (MBD_north_charge_sum - MBD_south_charge_sum)/(MBD_north_charge_sum + MBD_south_charge_sum);

        MBDz_MBDChargeAssy -> Fill(MBD_reco_z, MBD_charge_assy);

        LB_hist_gaus_width -> Fill(INTT_zvtx_reco_gaus_width);
        LB_hist_cut_group_width -> Fill(INTT_zvtx_reco_hist_cut_width);

        if (INTT_zvtx_reco_gaus_width     < ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_l * unit_correction || INTT_zvtx_reco_gaus_width     > ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_r * unit_correction) {continue;}
        if (INTT_zvtx_reco_hist_cut_width < ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_l * unit_correction || INTT_zvtx_reco_hist_cut_width > ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_r * unit_correction) {continue;}
        // LB_Gaus_Width_width > 35 && LB_Gaus_Width_width < 80 && LB_cut_peak_width>40&&LB_cut_peak_width<110
        if (in_nclu_inner <= 0 || in_nclu_outer <= 0) {continue;}

        // if (good_zvtx_tag == 1 && in_nclu_inner > 0 && in_nclu_outer > 0)
        // if ((in_nclu_inner + in_nclu_outer) > zvtx_dist_NClus_cut){
        // }
        INTTz_MBDChargeAssy -> Fill(INTT_reco_zvtx, MBD_charge_assy);

        if ( MBD_charge_assy < -1 * ana_map_version::MBD_assy_ratio_cut || MBD_charge_assy > ana_map_version::MBD_assy_ratio_cut) {continue;}

        reco_Zvtx_dist -> Fill(INTT_reco_zvtx);
        reco_Zvtx_dist_zoomin -> Fill(INTT_reco_zvtx);
        true_Zvtx_dist -> Fill(MC_true_zvtx);   

        NClus_intt_mbd_diff_corre -> Fill((in_nclu_inner + in_nclu_outer), INTT_reco_zvtx - MBD_reco_z);
        intt_mbd_reco_z_corre -> Fill(INTT_reco_zvtx, MBD_reco_z);
        intt_mbd_reco_z_diff  -> Fill(INTT_reco_zvtx - MBD_reco_z);

        if ((in_nclu_inner + in_nclu_outer) > zvtx_dist_NClus_cut) {mbdz_intt_mbd_diff_corre_cut -> Fill(MBD_reco_z, INTT_reco_zvtx - MBD_reco_z);}

        double INTTz_MBDz_diff_used_cut_l = (run_type == 1) ? ana_map_version::INTTz_MBDz_diff_cut_MC_l * unit_correction : ana_map_version::INTTz_MBDz_diff_cut_l * unit_correction;
        double INTTz_MBDz_diff_used_cut_r = (run_type == 1) ? ana_map_version::INTTz_MBDz_diff_cut_MC_r * unit_correction : ana_map_version::INTTz_MBDz_diff_cut_r * unit_correction;
        if ( (INTT_reco_zvtx - MBD_reco_z) > INTTz_MBDz_diff_used_cut_l && (INTT_reco_zvtx - MBD_reco_z) < INTTz_MBDz_diff_used_cut_r ) {intt_mbd_reco_z_corre_cut -> Fill(INTT_reco_zvtx, MBD_reco_z);}

        reco_Zvtx_dist_Mbin[centrality_map[Centrality_bin]] -> Fill(INTT_reco_zvtx);
        reco_Zvtx_dist_Mbin_zoomin[centrality_map[Centrality_bin]] -> Fill(INTT_reco_zvtx);

        // note : data
        if (run_type == 0) {continue;}

        Z_resolution_Nclu -> Fill((in_nclu_inner + in_nclu_outer), INTT_reco_zvtx - MC_true_zvtx);
        Z_resolution_pos  -> Fill(MC_true_zvtx, INTT_reco_zvtx - MC_true_zvtx);
        Z_correlation     -> Fill(MC_true_zvtx, INTT_reco_zvtx);
        
        if ((in_nclu_inner + in_nclu_outer) > zvtx_dist_NClus_cut) {
            Z_resolution_pos_cut -> Fill(MC_true_zvtx, INTT_reco_zvtx - MC_true_zvtx);
            high_NClus_Z_resolution -> Fill(INTT_reco_zvtx - MC_true_zvtx);
        }

        Z_resolution[ centrality_map[Centrality_bin] ]                   -> Fill( INTT_reco_zvtx - MC_true_zvtx );
        if ( centrality_map[Centrality_bin] < ana_map_version::inclusive_Mbin_cut ) {Z_resolution[ Z_resolution.size() - 1 ] -> Fill( INTT_reco_zvtx - MC_true_zvtx );} // note : the inclusive

        // note : for the passing rate
        true_zvtx_Mbin_Truezpos -> Fill(centrality_map[Centrality_bin], MC_true_zvtx);
        if ( centrality_map[Centrality_bin] < ana_map_version::inclusive_Mbin_cut ) {true_zvtx_Mbin_Truezpos -> Fill(centrality_region.size() - 1, MC_true_zvtx);}

        if (fabs(INTT_reco_zvtx - MC_true_zvtx) < required_zvtx_diff)
        {
            reco_zvtx_Mbin_Truezpos -> Fill(centrality_map[Centrality_bin], MC_true_zvtx);
            if ( centrality_map[Centrality_bin] < ana_map_version::inclusive_Mbin_cut ) {reco_zvtx_Mbin_Truezpos -> Fill(centrality_region.size() - 1, MC_true_zvtx);}
        }

    }
}

void plot_evt_zvtx::PrintPlot()
{
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    LB_hist_gaus_width -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/LB_hist_gaus_width.pdf",out_folder_directory.c_str()));
    
    c1 -> SetLogy(1);
    c1 -> Print(Form("%s/LB_hist_gaus_width_log.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogy(0);

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    LB_hist_cut_group_width -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/LB_hist_cut_group_width.pdf",out_folder_directory.c_str()));
    
    c1 -> SetLogy(1);
    c1 -> Print(Form("%s/LB_hist_cut_group_width_log.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> SetLogy(0);
    
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    MBDz_MBDChargeAssy -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/MBDz_MBDChargeAssy.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    INTTz_MBDChargeAssy -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/INTTz_MBDChargeAssy.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    NClus_intt_mbd_diff_corre -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/NClus_intt_mbd_diff_corre.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    mbdz_intt_mbd_diff_corre_cut -> Draw("colz0");
    draw_text -> DrawLatex(0.21, 0.87, Form("NClus > %i", zvtx_dist_NClus_cut));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/mbdz_intt_mbd_diff_corre_cut.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    intt_mbd_reco_z_corre -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    intt_mbd_reco_z_corre -> Fit(linear_fit, "NQ");
    linear_fit -> Draw("l same");
    draw_text -> DrawLatex(0.21, 0.87, Form("Y = %.2fx + %.2f", linear_fit -> GetParameter(1), linear_fit -> GetParameter(0)));
    c1 -> Print(Form("%s/intt_mbd_reco_z_corre.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    intt_mbd_reco_z_corre_cut -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    intt_mbd_reco_z_corre_cut -> Fit(linear_fit, "NQ");
    linear_fit -> Draw("l same");
    draw_text -> DrawLatex(0.21, 0.87, Form("Y = %.2fx + %.2f", linear_fit -> GetParameter(1), linear_fit -> GetParameter(0)));
    c1 -> Print(Form("%s/intt_mbd_reco_z_corre_cut.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    intt_mbd_reco_z_diff -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.83+0.04, Form("Reco. Entries: %.0f", intt_mbd_reco_z_diff -> GetEntries()));
    draw_text -> DrawLatex(0.21, 0.79+0.04, Form("Reco. Mean: %.3f %s", intt_mbd_reco_z_diff -> GetMean(), unit_text.c_str() ));
    draw_text -> DrawLatex(0.21, 0.75+0.04, Form("Reco. Width: %.3f %s", intt_mbd_reco_z_diff -> GetStdDev(), unit_text.c_str() ));


    gaus_fit_2 -> SetParameters(intt_mbd_reco_z_diff -> GetBinContent( intt_mbd_reco_z_diff -> GetMaximumBin() ), intt_mbd_reco_z_diff -> GetBinCenter( intt_mbd_reco_z_diff -> GetMaximumBin() ), intt_mbd_reco_z_diff -> GetStdDev() * 0.6, 0);
    gaus_fit_2 -> SetParLimits(0,0,100000);  // note : size 
    gaus_fit_2 -> SetParLimits(2,0,10000);   // note : Width
    gaus_fit_2 -> SetParLimits(3,0,10000);   // note : offset
    intt_mbd_reco_z_diff -> Fit(gaus_fit_2, "N", "", intt_mbd_reco_z_diff -> GetBinCenter( intt_mbd_reco_z_diff -> GetMaximumBin() ) - (0.4 * intt_mbd_reco_z_diff -> GetStdDev() ), intt_mbd_reco_z_diff -> GetBinCenter( intt_mbd_reco_z_diff -> GetMaximumBin() ) + (0.4 * intt_mbd_reco_z_diff -> GetStdDev() ) );
    gaus_fit_2 -> SetRange( gaus_fit_2->GetParameter(1) - gaus_fit_2->GetParameter(2) * 0.5, gaus_fit_2->GetParameter(1) + gaus_fit_2->GetParameter(2) * 0.5 ); 
    gaus_fit_2 -> Draw("lsame");
    draw_text -> DrawLatex(0.21, 0.75, Form("Gaus mean: %.3f %s",gaus_fit_2 -> GetParameter(1), unit_text.c_str() ));
    // draw_text -> DrawLatex(0.21, 0.67, Form("Gaus width  : %.3f %s",fabs(gaus_fit_2 -> GetParameter(2)), unit_text.c_str() ));
    // draw_text -> DrawLatex(0.21, 0.63, Form("Gaus offset : %.3f %s", gaus_fit_2 -> GetParameter(3), unit_text.c_str() ));

    c1 -> Print(Form("%s/intt_mbd_reco_z_diff.pdf",out_folder_directory.c_str()));
    c1 -> Clear();


    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    reco_Zvtx_dist -> Draw("hist"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    // draw_text -> DrawLatex(0.21, 0.87, Form("NClus > %i", zvtx_dist_NClus_cut));
    draw_text -> DrawLatex(0.21, 0.83, Form("Reco. Entries: %.0f", reco_Zvtx_dist -> GetEntries()));
    draw_text -> DrawLatex(0.21, 0.79, Form("Reco. Mean: %.3f %s", reco_Zvtx_dist -> GetMean(), unit_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.75, Form("Reco. Width: %.3f %s", reco_Zvtx_dist -> GetStdDev(), unit_text.c_str()));

    if (run_type == 1) // note: MC
    {
        leg -> AddEntry(reco_Zvtx_dist, "Reco. Z", "f");
        leg -> AddEntry(true_Zvtx_dist, "True Z", "f");
        true_Zvtx_dist -> Draw("hist same");

        draw_text -> DrawLatex(0.21, 0.71, Form("True Entries: %.0f", true_Zvtx_dist -> GetEntries()));
        draw_text -> DrawLatex(0.21, 0.67, Form("True Mean: %.3f %s", true_Zvtx_dist -> GetMean(), unit_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.63, Form("True Width: %.3f %s", true_Zvtx_dist -> GetStdDev(), unit_text.c_str()));

        leg -> Draw("same");
    }

    c1 -> Print(Form("%s/reco_Zvtx_dist.pdf",out_folder_directory.c_str()));
    c1 -> Clear();
    leg -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (run_type == 1) // note : MC
    {
        true_Zvtx_dist -> SetMaximum(true_Zvtx_dist -> GetBinContent(true_Zvtx_dist -> GetMaximumBin()) * 1.4);
        true_Zvtx_dist -> GetXaxis() -> SetTitle(Form("Vertex Z [%s]", unit_text.c_str()));
        true_Zvtx_dist -> Draw("hist");
        
        leg -> AddEntry(reco_Zvtx_dist, "Reco. Z", "f");
        leg -> AddEntry(true_Zvtx_dist, "True Z", "f");
        
        draw_text -> DrawLatex(0.21, 0.71, Form("True Entries: %.0f", true_Zvtx_dist -> GetEntries()));
        draw_text -> DrawLatex(0.21, 0.67, Form("True Mean: %.3f %s", true_Zvtx_dist -> GetMean(), unit_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.63, Form("True Width: %.3f %s", true_Zvtx_dist -> GetStdDev(), unit_text.c_str()));

        leg -> Draw("same");
        
        
        reco_Zvtx_dist -> Draw("hist same"); 
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        // draw_text -> DrawLatex(0.21, 0.87, Form("NClus > %i", zvtx_dist_NClus_cut));
        draw_text -> DrawLatex(0.21, 0.83, Form("Reco. Entries: %.0f", reco_Zvtx_dist -> GetEntries()));
        draw_text -> DrawLatex(0.21, 0.79, Form("Reco. Mean: %.3f %s", reco_Zvtx_dist -> GetMean(), unit_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.75, Form("Reco. Width: %.3f %s", reco_Zvtx_dist -> GetStdDev(), unit_text.c_str()));

        c1 -> Print(Form("%s/true_Zvtx_overlap_dist.pdf",out_folder_directory.c_str()));
        c1 -> Clear();
        leg -> Clear();
    }

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin_each.pdf(",out_folder_directory.c_str()) );
    TH1F * reco_Zvtx_dist_inclusive = (TH1F*)reco_Zvtx_dist->Clone("reco_Zvtx_dist_inclusive");
    reco_Zvtx_dist_inclusive -> Draw("ep");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    leg_single->AddEntry(reco_Zvtx_dist_inclusive, "Inclusive", "lep");
    leg_single->Draw("same");
    c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin_each.pdf",out_folder_directory.c_str()) );
    c1 -> Clear();
    leg_single -> Clear();

    for (int i = 0; i < reco_Zvtx_dist_Mbin.size(); i++){
        leg_single->AddEntry(reco_Zvtx_dist_Mbin[i], centrality_region[i].c_str(), "lep");
        reco_Zvtx_dist_Mbin[i]->Draw("ep");
        leg_single->Draw("same");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin_each.pdf",out_folder_directory.c_str()) );
        c1 -> Clear();
        leg_single -> Clear();
    }
    c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin_each.pdf)",out_folder_directory.c_str()) );
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin.pdf(",out_folder_directory.c_str()) );
    // reco_Zvtx_dist_inclusive -> Scale(1./reco_Zvtx_dist_inclusive->Integral(-1,-1));
    // reco_Zvtx_dist_inclusive -> SetMaximum(reco_Zvtx_dist_inclusive -> GetMaximum() * 1.5);
    // reco_Zvtx_dist_inclusive -> Draw("ep");
    // leg_inc->AddEntry(reco_Zvtx_dist_inclusive, "Inclusive", "lep");
    double normalize_count = -1;
    for (int i = 0; i < reco_Zvtx_dist_Mbin.size(); i++){
        if (reco_Zvtx_dist_Mbin[i] -> GetEntries() > 0) {
            reco_Zvtx_dist_Mbin[i] -> Scale(1./reco_Zvtx_dist_Mbin[i]->Integral(-1,-1));

            if (normalize_count == -1){
                normalize_count = i;
                reco_Zvtx_dist_Mbin[i] -> SetMaximum(reco_Zvtx_dist_Mbin[i] -> GetMaximum() * 1.5);
                reco_Zvtx_dist_Mbin[i]->Draw("ep");
                leg_inc->AddEntry(reco_Zvtx_dist_Mbin[i], centrality_region[i].c_str(), "lep");
                leg_inc -> Draw("same");
            }
            else 
            {
                reco_Zvtx_dist_Mbin[i]->Draw("ep same");
                leg_inc->AddEntry(reco_Zvtx_dist_Mbin[i], centrality_region[i].c_str(), "lep");
                leg_inc -> Draw("same");
            }   
        }
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin.pdf",out_folder_directory.c_str()) );
    }
    c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin.pdf)",out_folder_directory.c_str()) );
    c1 -> Clear();


    // // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // c1 -> cd();
    // TH1F * reco_Zvtx_dist_inclusive_zoomin = (TH1F*)reco_Zvtx_dist_zoomin->Clone("reco_Zvtx_dist_inclusive_zoomin");
    // reco_Zvtx_dist_inclusive_zoomin -> Scale(1./reco_Zvtx_dist_inclusive_zoomin->Integral(-1,-1));
    // reco_Zvtx_dist_inclusive_zoomin -> SetMaximum(reco_Zvtx_dist_inclusive_zoomin -> GetMaximum() * 1.5);
    // reco_Zvtx_dist_inclusive_zoomin -> SetMinimum(reco_Zvtx_dist_inclusive_zoomin -> GetMaximum() / 4.);
    // reco_Zvtx_dist_inclusive_zoomin -> Draw("ep");
    // for (int i = 0; i < reco_Zvtx_dist_Mbin_zoomin.size(); i++){
    //     reco_Zvtx_dist_Mbin_zoomin[i]->Scale(1./reco_Zvtx_dist_Mbin_zoomin[i]->Integral(-1,-1));
    //     reco_Zvtx_dist_Mbin_zoomin[i]->Draw("ep same");
    // }

    // leg_inc -> Draw("same");

    // c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin_zoomin.pdf",out_folder_directory.c_str()) );
    // c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    leg_inc -> Clear();
    c1 -> cd();

    TH1F * reco_Zvtx_dist_inclusive_zoomin = (TH1F*)reco_Zvtx_dist_zoomin->Clone("reco_Zvtx_dist_inclusive_zoomin");
    reco_Zvtx_dist_inclusive_zoomin -> Scale(1./reco_Zvtx_dist_inclusive_zoomin->Integral(-1,-1));
    reco_Zvtx_dist_inclusive_zoomin -> SetMaximum(reco_Zvtx_dist_inclusive_zoomin -> GetMaximum() * 1.5);
    reco_Zvtx_dist_inclusive_zoomin -> SetMinimum(reco_Zvtx_dist_inclusive_zoomin -> GetMaximum() / 4.);
    leg_inc->AddEntry(reco_Zvtx_dist_inclusive_zoomin, centrality_region.back().c_str(), "lep");
    reco_Zvtx_dist_inclusive_zoomin -> Draw("ep");
    leg_inc -> Draw("same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin_zoomin.pdf(",out_folder_directory.c_str()) );
    // reco_Zvtx_dist_inclusive -> Scale(1./reco_Zvtx_dist_inclusive->Integral(-1,-1));
    // reco_Zvtx_dist_inclusive -> SetMaximum(reco_Zvtx_dist_inclusive -> GetMaximum() * 1.5);
    // reco_Zvtx_dist_inclusive -> Draw("ep");
    // leg_inc->AddEntry(reco_Zvtx_dist_inclusive, "Inclusive", "lep");
    for (int i = 0; i < reco_Zvtx_dist_Mbin_zoomin.size(); i++){
        if (reco_Zvtx_dist_Mbin_zoomin[i] -> GetEntries() > 0) {
            reco_Zvtx_dist_Mbin_zoomin[i] -> Scale(1./reco_Zvtx_dist_Mbin_zoomin[i]->Integral(-1,-1));

            // if (normalize_count == -1){
            //     normalize_count = i;
            //     reco_Zvtx_dist_Mbin_zoomin[i] -> SetMaximum(reco_Zvtx_dist_Mbin_zoomin[i] -> GetMaximum() * 1.5);
            //     reco_Zvtx_dist_Mbin_zoomin[i]->Draw("ep");
            //     leg_inc->AddEntry(reco_Zvtx_dist_Mbin_zoomin[i], centrality_region[i].c_str(), "lep");
            //     leg_inc -> Draw("same");
            // }
            // else 
            // {
            //     reco_Zvtx_dist_Mbin_zoomin[i]->Draw("ep same");
            //     leg_inc->AddEntry(reco_Zvtx_dist_Mbin_zoomin[i], centrality_region[i].c_str(), "lep");
            //     leg_inc -> Draw("same");
            // }   

            reco_Zvtx_dist_Mbin_zoomin[i]->Draw("ep same");
            leg_inc->AddEntry(reco_Zvtx_dist_Mbin_zoomin[i], centrality_region[i].c_str(), "lep");
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
            leg_inc -> Draw("same");
        }

        c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin_zoomin.pdf",out_folder_directory.c_str()) );
    }
    c1 -> Print( Form("%s/reco_Zvtx_dist_Mbin_zoomin.pdf)",out_folder_directory.c_str()) );
    c1 -> Clear();



    
    // note : data
    if (run_type == 0) {file_in -> Close(); return;}


    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (run_type == 1) // note: MC
    {
        vector<double> fit_width_vec; fit_width_vec.clear();
        vector<double> num_width_vec; num_width_vec.clear();
        vector<double> avg_width_vec; avg_width_vec.clear();

        c1 -> Print(Form("%s/INTT_Z_resolution_centrality.pdf(", out_folder_directory.c_str()));
        for (int i = 0; i < Z_resolution.size(); i++)
        {
            Z_resolution[i] -> SetMinimum(0);
            Z_resolution[i] -> SetMaximum(Z_resolution[i] -> GetMaximum() * 1.7);

            gaus_fit_2 -> SetParameters(Z_resolution[i] -> GetBinContent( Z_resolution[i] -> GetMaximumBin() ), Z_resolution[i] -> GetBinCenter( Z_resolution[i] -> GetMaximumBin() ), Z_resolution[i] -> GetStdDev(), 0);
            gaus_fit_2 -> SetParLimits(0,0,100000);  // note : size 
            gaus_fit_2 -> SetParLimits(2,0,10000);   // note : Width
            gaus_fit_2 -> SetParLimits(3,0,10000);   // note : offset

            double fit_range_l = Z_resolution[i] -> GetBinCenter( Z_resolution[i] -> GetMaximumBin() ) - (2 * Z_resolution[i] -> GetStdDev() );
            double fit_range_r = Z_resolution[i] -> GetBinCenter( Z_resolution[i] -> GetMaximumBin() ) + (2 * Z_resolution[i] -> GetStdDev() );

            Z_resolution[i] -> Fit(gaus_fit_2, "N", "", fit_range_l, fit_range_r);
            cout<<"fit reduced chi2 : "<<i<<" "<<gaus_fit_2 -> GetChisquare() / double(gaus_fit_2 -> GetNDF())<<endl;
            Z_resolution[i] -> Draw("hist"); 
            coord_line -> DrawLine(fit_range_l, 0, fit_range_l, Z_resolution[i] -> GetMaximum());
            coord_line -> DrawLine(fit_range_r, 0, fit_range_r, Z_resolution[i] -> GetMaximum());
            gaus_fit_2 -> SetRange( gaus_fit_2->GetParameter(1) - gaus_fit_2->GetParameter(2) * 1.5, gaus_fit_2->GetParameter(1) + gaus_fit_2->GetParameter(2) * 1.5); 
            gaus_fit_2 -> Draw("lsame");
            draw_text -> DrawLatex(0.21, 0.75, Form("Centrality: %s", centrality_region[i].c_str()));
            draw_text -> DrawLatex(0.21, 0.71, Form("Gaus mean  : %.3f %s",gaus_fit_2 -> GetParameter(1), unit_text.c_str()));
            draw_text -> DrawLatex(0.21, 0.67, Form("Gaus width : %.3f %s",fabs(gaus_fit_2 -> GetParameter(2)), unit_text.c_str()));
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));

            // Z_resolution_centrality_gr_fit -> SetPoint     (i, i, gaus_fit_2->GetParameter(2));
            // Z_resolution_centrality_gr_fit -> SetPointError(i, 0, gaus_fit_2->GetParError(2));

            // Z_mean_centrality_gr_fit       -> SetPoint     (i, i, gaus_fit_2->GetParameter(1));            
            // Z_mean_centrality_gr_fit       -> SetPointError(i, 0, gaus_fit_2->GetParError(1));

            Z_resolution_centrality_gr_fit -> SetPoint     (i, i, gaus_fit_2->GetParameter(2));
            Z_resolution_centrality_gr_fit -> SetPointError(i, 0, 0);

            Z_mean_centrality_gr_fit       -> SetPoint     (i, i, gaus_fit_2->GetParameter(1));            
            Z_mean_centrality_gr_fit       -> SetPointError(i, 0, 0);


            Z_resolution_centrality_gr_num -> SetPoint     (i, i, Z_resolution[i]->GetStdDev());
            Z_resolution_centrality_gr_num -> SetPointError(i, 0, Z_resolution[i]->GetStdDevError());

            Z_mean_centrality_gr_num       -> SetPoint     (i, i, Z_resolution[i]->GetMean());            
            Z_mean_centrality_gr_num       -> SetPointError(i, 0, Z_resolution[i]->GetMeanError());

            fit_width_vec.push_back(gaus_fit_2->GetParameter(2));
            num_width_vec.push_back(Z_resolution[i]->GetStdDev());
            avg_width_vec.push_back((gaus_fit_2->GetParameter(2) + Z_resolution[i]->GetStdDev())/2.);

            c1 -> Print(Form("%s/INTT_Z_resolution_centrality.pdf", out_folder_directory.c_str()));
            c1 -> Clear();
        }
        c1 -> Print(Form("%s/INTT_Z_resolution_centrality.pdf)", out_folder_directory.c_str()));
        c1 -> Clear();

        // note : important 
        for (int i = 0; i < fit_width_vec.size(); i++)
        {
            cout<<i<<" fit: "<<fit_width_vec[i]<<Form(" %s, num: ",unit_text.c_str())<<num_width_vec[i]<<Form(" %s, avg: ", unit_text.c_str())<<avg_width_vec[i]<<Form(" %s", unit_text.c_str())<<endl;
        }
    }

    if (run_type == 1) // note : MC
    {
        c1 -> cd();
        high_NClus_Z_resolution -> SetMinimum(0);
        high_NClus_Z_resolution -> SetMaximum(high_NClus_Z_resolution -> GetMaximum() * 1.7);
        high_NClus_Z_resolution -> Draw("hist");

        gaus_fit_2 -> SetParameters(
            high_NClus_Z_resolution -> GetBinContent( high_NClus_Z_resolution -> GetMaximumBin() ), 
            high_NClus_Z_resolution -> GetBinCenter( high_NClus_Z_resolution -> GetMaximumBin() ), 
            high_NClus_Z_resolution -> GetStdDev(), 
            0
        );

        gaus_fit_2 -> SetParLimits(0,0,100000);  // note : size 
        gaus_fit_2 -> SetParLimits(2,0,10000);   // note : Width
        gaus_fit_2 -> SetParLimits(3,0,10000);   // note : offset

        double fit_range_l = high_NClus_Z_resolution -> GetBinCenter( high_NClus_Z_resolution -> GetMaximumBin() ) - (2. * high_NClus_Z_resolution -> GetStdDev() );
        double fit_range_r = high_NClus_Z_resolution -> GetBinCenter( high_NClus_Z_resolution -> GetMaximumBin() ) + (2. * high_NClus_Z_resolution -> GetStdDev() );

        high_NClus_Z_resolution -> Fit(gaus_fit_2, "N", "", fit_range_l, fit_range_r);
        gaus_fit_2 -> SetRange( gaus_fit_2->GetParameter(1) - gaus_fit_2->GetParameter(2) * 1.5, gaus_fit_2->GetParameter(1) + gaus_fit_2->GetParameter(2) * 1.5);
        coord_line -> DrawLine(fit_range_l, 0, fit_range_l, high_NClus_Z_resolution -> GetMaximum());
        coord_line -> DrawLine(fit_range_r, 0, fit_range_r, high_NClus_Z_resolution -> GetMaximum()); 
        gaus_fit_2 -> Draw("lsame");
        draw_text -> DrawLatex(0.21, 0.87, Form("NClus > %i", zvtx_dist_NClus_cut));
        draw_text -> DrawLatex(0.21, 0.71, Form("Gaus mean  : %.3f %s",gaus_fit_2 -> GetParameter(1), unit_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.67, Form("Gaus width : %.3f %s",fabs(gaus_fit_2 -> GetParameter(2)), unit_text.c_str()));

        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/high_NClus_Z_resolution.pdf", out_folder_directory.c_str()));
        c1 -> Clear();
    }

    if (run_type == 1) // note: MC
    {
        c1 -> cd();
        Z_resolution_centrality_gr_fit -> Draw("ap");
        Z_resolution_centrality_gr_fit -> GetXaxis() -> SetLimits(-1,Z_resolution_centrality_gr_fit -> GetN() + 1);
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_resolution_centrality_gr_fit.pdf", out_folder_directory.c_str()));
        c1 -> Clear();

        for (int i = 0; i < Z_resolution_centrality_gr_fit->GetN(); i++)
        {
            cout<<i<<" "<<Z_resolution_centrality_gr_fit->GetPointY(i)<<endl;
        }

        c1 -> cd();
        Z_mean_centrality_gr_fit -> Draw("ap");
        Z_mean_centrality_gr_fit -> GetXaxis() -> SetLimits(-1,Z_mean_centrality_gr_fit -> GetN() + 1);
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_mean_centrality_gr_fit.pdf", out_folder_directory.c_str()));
        c1 -> Clear();


        c1 -> cd();
        Z_resolution_centrality_gr_num -> Draw("ap");
        Z_resolution_centrality_gr_num -> GetXaxis() -> SetLimits(-1,Z_resolution_centrality_gr_num -> GetN() + 1);
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_resolution_centrality_gr_num.pdf", out_folder_directory.c_str()));
        c1 -> Clear();

        for (int i = 0; i < Z_resolution_centrality_gr_num->GetN(); i++)
        {
            cout<<i<<" "<<Z_resolution_centrality_gr_num->GetPointY(i)<<endl;
        }

        c1 -> cd();
        Z_mean_centrality_gr_num -> Draw("ap");
        Z_mean_centrality_gr_num -> GetXaxis() -> SetLimits(-1,Z_mean_centrality_gr_num -> GetN() + 1);
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_mean_centrality_gr_num.pdf", out_folder_directory.c_str()));
        c1 -> Clear();

    }


    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (run_type == 1) // note: MC
    {    
        Z_resolution_Nclu -> Draw("colz0"); 
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_resolution_Nclu.pdf",out_folder_directory.c_str()));
        c1 -> Clear();
    }
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (run_type == 1) // note: MC
    {
        Z_resolution_pos -> Draw("colz0"); 
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        c1 -> Print(Form("%s/Z_resolution_pos.pdf",out_folder_directory.c_str()));
        c1 -> Clear();
    }
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (run_type == 1) // note: MC
    {
        Z_resolution_pos_cut -> Draw("colz0"); 
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.87, Form("NClus > %i", zvtx_dist_NClus_cut));
        c1 -> Print(Form("%s/Z_resolution_pos_cut.pdf",out_folder_directory.c_str()));
        c1 -> Clear();
    }

    if (run_type == 1) // note: MC
    {
        Z_correlation -> Draw("colz0"); 
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
        Z_correlation -> Fit(linear_fit, "NQ");
        linear_fit -> Draw("l same");
        draw_text -> DrawLatex(0.21, 0.87, Form("Y = %.2fx + %.2f", linear_fit -> GetParameter(1), linear_fit -> GetParameter(0)));
        c1 -> Print(Form("%s/Z_correlation.pdf",out_folder_directory.c_str()));
        c1 -> Clear();
    }



    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // TH2F_division(true_zvtx_Mbin_Truezpos,           reco_zvtx_Mbin_Truezpos, ratio_Mbin_Truezpos);
    // TH2F_division(true_zvtx_Mbin_Truezpos_inclusive, reco_zvtx_Mbin_Truezpos, ratio_Mbin_Truezpos_inclusive);

    ratio_Mbin_Truezpos = (TH2F *) reco_zvtx_Mbin_Truezpos -> Clone("ratio_Mbin_Truezpos");
    ratio_Mbin_Truezpos -> Divide(ratio_Mbin_Truezpos, true_zvtx_Mbin_Truezpos, 1, 1, "B");

    gStyle->SetPaintTextFormat("1.0f");

    c1 -> cd();
    true_zvtx_Mbin_Truezpos -> SetMarkerSize(1.0);
    true_zvtx_Mbin_Truezpos -> Draw("colz0");
    true_zvtx_Mbin_Truezpos -> Draw("HIST TEXT45 SAME");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/true_zvtx_Mbin_Truezpos.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    reco_zvtx_Mbin_Truezpos -> SetMarkerSize(1.0);
    reco_zvtx_Mbin_Truezpos -> Draw("colz0");
    reco_zvtx_Mbin_Truezpos -> Draw("HIST TEXT45 SAME");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text->DrawLatex(0.16, 1 - gPad->GetTopMargin() + 0.01, Form("Pass if |Reco. - True| < %.1f %s", required_zvtx_diff, unit_text.c_str()));
    c1 -> Print(Form("%s/reco_zvtx_Mbin_Truezpos.pdf", out_folder_directory.c_str()));
    c1 -> Clear();

    gStyle->SetPaintTextFormat("1.3f");

    c1 -> cd();
    ratio_Mbin_Truezpos -> SetMarkerSize(0.7);
    ratio_Mbin_Truezpos -> Draw("colz0");
    ratio_Mbin_Truezpos -> Draw("HIST TEXT45E SAME");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
    draw_text->DrawLatex(0.16, 1 - gPad->GetTopMargin() + 0.01, Form("Pass if |Reco. - True| < %.1f %s", required_zvtx_diff, unit_text.c_str()));
    c1 -> Print(Form("%s/ratio_Mbin_Truezpos.pdf", out_folder_directory.c_str()));
    c1 -> Clear();


    true_zvtx_Mbin_Truezpos -> Reset("ICESM");
    reco_zvtx_Mbin_Truezpos -> Reset("ICESM");
    ratio_Mbin_Truezpos -> Reset("ICESM");
    file_in -> Close();

    return;
}

pair<TH1F*, vector<TH1F*>> plot_evt_zvtx::GetINTTRecoZHistVec()
{
    return {reco_Zvtx_dist,reco_Zvtx_dist_Mbin};
}
pair<TH1F*, vector<TH1F*>> plot_evt_zvtx::GetINTTRecoZHistZoomInVec()
{
    return {reco_Zvtx_dist_zoomin, reco_Zvtx_dist_Mbin_zoomin};
}
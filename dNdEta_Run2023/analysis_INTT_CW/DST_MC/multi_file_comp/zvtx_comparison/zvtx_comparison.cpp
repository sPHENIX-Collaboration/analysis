#include "sPhenixStyle.C"
#include "../../ReadINTTZ/ReadINTTZCombine.C"
#include "../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

class zvtx_comp
{
    public:
        zvtx_comp(vector<vector<string>> input_file_full_info_in, string output_folder_name_in, int unit_tag_in = 1);
        

    protected:
        // note : vector<vector<string>> input_file_full_info { {data_type, full_input_file_directory, abbreviation_of_file} }
        // note : vector<vector<string>> input_file_full_info { 
        // note :     {
        // note :         data_type, 
        // note :         full_input_file_directory, 
        // note :         tree_name,
        // note :         abbreviation_of_file // note : corr. / test / data / data_z_shifted <------> Original / Inner cluster #phi rotated by #pi
        // note :     } 
        // note : }
        vector<vector<string>> input_file_full_info;
        string output_folder_name;
        int unit_tag; // note : 1 -> cm, 0 -> mm

        string sph_labeling;
        string unit_text;
        double unit_correction;

        vector<TFile *> file_in;
        vector<TTree *> tree_in;
        vector<ReadINTTZCombine *> RecoZ;

        vector< TH1F* > fine_zvtx_dist_original;
        vector< TH1F* > fine_zvtx_MC_reweight;
        vector< TH1F* > fine_zvtx_reweight_ratio;

        vector< TH1F* > coarse_zvtx_dist_original;
        vector< TH1F* > coarse_zvtx_MC_reweight;
        vector< TH1F* > coarse_zvtx_reweight_ratio;

        double evt_vtxZ_zvtx_range_l       = ana_map_version::evt_vtxZ_zvtx_range_l;
        double evt_vtxZ_zvtx_range_r       = ana_map_version::evt_vtxZ_zvtx_range_r;
        vector<double> coarse_z_region     = ana_map_version::z_region; 
        string inclusive_centrality_region = (ana_map_version::centrality_region).back();

        TCanvas * c1;
        TLegend * legend;
        TLatex * ltx;
        TLatex * draw_text;
        TLine * coord_line;

        vector<string> color_code_vec = {"#7c9885","#000000"};

        void MainInit();
        void FillHist();
        void CalculateReweight();
        void PrintPlots();
        void DrawCoordLine(TH1F * hist_in);
};

zvtx_comp::zvtx_comp(vector<vector<string>> input_file_full_info_in, string output_folder_name_in, int unit_tag_in = 1)
: input_file_full_info(input_file_full_info_in),
output_folder_name(output_folder_name_in),
unit_tag(unit_tag_in)
{
    system(Form("if [ ! -d %s ]; then mkdir %s; fi", output_folder_name.c_str(), output_folder_name.c_str()));

    unit_correction = (unit_tag == 1)? 0.1 : 1;
    unit_text = (unit_tag == 1) ? "cm" : "mm";

    evt_vtxZ_zvtx_range_l = evt_vtxZ_zvtx_range_l * unit_correction;
    evt_vtxZ_zvtx_range_r = evt_vtxZ_zvtx_range_r * unit_correction;
    for (int i = 0; i < coarse_z_region.size(); i++) {coarse_z_region[i] = coarse_z_region[i] * unit_correction;}

    bool found_data_tag = false;
    for (int i = 0; i < input_file_full_info.size(); i++) {
        if (input_file_full_info[i][0] == "data") {
            found_data_tag = true;
            break;
        }
    }

    sph_labeling = (found_data_tag) ? "Work-in-progress" : "Simulation";

    file_in.clear();
    tree_in.clear();
    RecoZ.clear();
    fine_zvtx_dist_original.clear();
    fine_zvtx_MC_reweight.clear();
    fine_zvtx_reweight_ratio.clear();

    coarse_zvtx_dist_original.clear();
    coarse_zvtx_MC_reweight.clear();
    coarse_zvtx_reweight_ratio.clear();


    MainInit();
    FillHist();
    CalculateReweight();
    PrintPlots();
} // note : end of constructor

void zvtx_comp::MainInit()
{   
    SetsPhenixStyle();

    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    double legend_text_size = 0.03;
    double legend_upper_y = 0.9;
    legend = new TLegend(
        0.6, // note : x1
        legend_upper_y - (input_file_full_info.size()) * (legend_text_size + 0.005), // note : y1
        0.8, // note : x2
        legend_upper_y // note : y2
    );
    // legend -> SetMargin(0);
    legend->SetTextSize(legend_text_size);

    c1 = new TCanvas("c1","",950,800);

    coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(16);
    coord_line -> SetLineStyle(2);

    for (int file_i = 0; file_i < input_file_full_info.size(); file_i++)
    {
        file_in.push_back(TFile::Open(input_file_full_info[file_i][1].c_str()));
        tree_in.push_back((TTree*) file_in[file_i] -> Get(input_file_full_info[file_i][2].c_str()));
        RecoZ.push_back(new ReadINTTZCombine(tree_in[file_i]));

        fine_zvtx_dist_original.push_back(  new TH1F("", Form("fine_zvtx_dist_original;zvtx [%s];Entry (A.U.)",unit_text.c_str()),      50, evt_vtxZ_zvtx_range_l, evt_vtxZ_zvtx_range_r) ); 
        fine_zvtx_MC_reweight.push_back(    new TH1F("", Form("fine_zvtx_MC_reweight;zvtx [%s];Entry (A.U.)",unit_text.c_str()),        50, evt_vtxZ_zvtx_range_l, evt_vtxZ_zvtx_range_r) );
        fine_zvtx_reweight_ratio.push_back( new TH1F("", Form("fine_zvtx_reweight_ratio;zvtx [%s];Weighting factor",unit_text.c_str()), 50, evt_vtxZ_zvtx_range_l, evt_vtxZ_zvtx_range_r) );

        coarse_zvtx_dist_original.push_back(  new TH1F("", Form("coarse_zvtx_dist_original;zvtx [%s];Entry (A.U.)",  unit_text.c_str()), coarse_z_region.size() - 1, &coarse_z_region[0] ) );
        coarse_zvtx_MC_reweight.push_back(    new TH1F("", Form("coarse_zvtx_MC_reweight;zvtx [%s];Entry (A.U.)",    unit_text.c_str()), coarse_z_region.size() - 1, &coarse_z_region[0] ) );
        coarse_zvtx_reweight_ratio.push_back( new TH1F("", Form("coarse_zvtx_reweight_ratio;zvtx [%s];Weighting factor", unit_text.c_str()), coarse_z_region.size() - 1, &coarse_z_region[0] ) );
    }

    return;
}

void zvtx_comp::FillHist()
{
    for (int file_i = 0; file_i < input_file_full_info.size(); file_i++)
    {
        for (int evt_i = 0; evt_i < tree_in[file_i] -> GetEntries(); evt_i++)
        {
            RecoZ[file_i] -> LoadTree(evt_i);
            RecoZ[file_i] -> GetEntry(evt_i);

            // note : min bias definition
            if (RecoZ[file_i] -> is_min_bias_wozdc == 0) {continue;}

            // note : to get rid of the nan value
            if (RecoZ[file_i] -> Centrality_float != RecoZ[file_i] -> Centrality_float) {continue;}
            if (RecoZ[file_i] -> MBD_reco_z       != RecoZ[file_i] -> MBD_reco_z)       {continue;}

            if (RecoZ[file_i] -> nclu_inner <= 0)   {continue;}
            if (RecoZ[file_i] -> nclu_outer <= 0)   {continue;}
            if ((RecoZ[file_i] -> nclu_inner + RecoZ[file_i] -> nclu_outer) < ana_map_version::evt_vtxZ_N_clu_cutl) {continue;}

            double MBD_charge_assy = (RecoZ[file_i] -> MBD_north_charge_sum - RecoZ[file_i] -> MBD_south_charge_sum) / (RecoZ[file_i] -> MBD_north_charge_sum + RecoZ[file_i] -> MBD_south_charge_sum);
            if ( MBD_charge_assy < -1 * ana_map_version::MBD_assy_ratio_cut || MBD_charge_assy >  ana_map_version::MBD_assy_ratio_cut) {continue;}

            if (RecoZ[file_i] -> LB_Gaus_Width_width < ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_l || RecoZ[file_i] -> LB_Gaus_Width_width > ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_r) {continue;}
            if (RecoZ[file_i] -> LB_cut_peak_width <   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_l || RecoZ[file_i] -> LB_cut_peak_width >   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_r) {continue;}        
            
            // note : the INTTrecoZ, MBDrecoZ diff cut
            // note : for data
            if ( input_file_full_info[file_i][0] == "data" && 
                    ((RecoZ[file_i] -> LB_Gaus_Mean_mean - RecoZ[file_i] -> MBD_reco_z) < ana_map_version::INTTz_MBDz_diff_cut_l || (RecoZ[file_i] -> LB_Gaus_Mean_mean - RecoZ[file_i] -> MBD_reco_z) > ana_map_version::INTTz_MBDz_diff_cut_r) 
                ) {continue;}
            
            // note : for MC, the INTTrecoZ, MBDrecoZ diff cut
            if ( input_file_full_info[file_i][0] == "MC" && 
                    ((RecoZ[file_i] -> LB_Gaus_Mean_mean - RecoZ[file_i] -> MBD_reco_z) < ana_map_version::INTTz_MBDz_diff_cut_MC_l || (RecoZ[file_i] -> LB_Gaus_Mean_mean - RecoZ[file_i] -> MBD_reco_z) > ana_map_version::INTTz_MBDz_diff_cut_MC_r) 
                ) {continue;}

            int Centrality_bin = ana_map_version::convert_centrality_bin(RecoZ[file_i] -> Centrality_float);
            // note : the centrality bin rejection
            if (Centrality_bin >= ana_map_version::inclusive_Mbin_cut) {continue;}

            // // note : the unit conversion, from mm to cm
            // if (file_i == 0){
            //     fine_zvtx_dist_original[file_i] -> Fill( RecoZ[file_i] -> LB_Gaus_Mean_mean * unit_correction );
            //     coarse_zvtx_dist_original[file_i] -> Fill( RecoZ[file_i] -> LB_Gaus_Mean_mean * unit_correction );
            // }
            // else { // note : special_tag == "data_z_shifted"
            //     fine_zvtx_dist_original[file_i] -> Fill( RecoZ[file_i] -> LB_Gaus_Mean_mean * unit_correction + 1.056 );
            //     coarse_zvtx_dist_original[file_i] -> Fill( RecoZ[file_i] -> LB_Gaus_Mean_mean * unit_correction + 1.056);
            // }
            
            fine_zvtx_dist_original[file_i] -> Fill( RecoZ[file_i] -> LB_Gaus_Mean_mean * unit_correction );
            coarse_zvtx_dist_original[file_i] -> Fill( RecoZ[file_i] -> LB_Gaus_Mean_mean * unit_correction );
            
        }
    }    
}

void zvtx_comp::CalculateReweight()
{
    if (input_file_full_info[0][0] != "MC") {
        cout<<"[Error], to calculate the reweighting factor, the first input file should be MC file"<<endl;
        exit(1);
    }

    // note : to normalize the MC zvtx distribution
    fine_zvtx_dist_original[0] -> Scale( 1./fine_zvtx_dist_original[0] -> Integral(-1, -1) );
    coarse_zvtx_dist_original[0] -> Scale( 1./coarse_zvtx_dist_original[0] -> Integral(-1, -1) );

    // note : file_i starts from 1, since the first one is MC
    for (int file_i = 1; file_i < input_file_full_info.size(); file_i++)
    {
        // note : to normalize the rest of the distributions
        fine_zvtx_dist_original[file_i] -> Scale( 1./fine_zvtx_dist_original[file_i] -> Integral(-1, -1) );
        coarse_zvtx_dist_original[file_i] -> Scale( 1./coarse_zvtx_dist_original[file_i] -> Integral(-1, -1) );

        fine_zvtx_reweight_ratio[file_i] -> Divide(fine_zvtx_dist_original[file_i], fine_zvtx_dist_original[0]);
        coarse_zvtx_reweight_ratio[file_i] -> Divide(coarse_zvtx_dist_original[file_i], coarse_zvtx_dist_original[0]);

        // for (int zvtx_i = 1; zvtx_i <= fine_zvtx_dist_original[file_i] -> GetNbinsX(); zvtx_i++)
        // {
        //     double original_bin_content = fine_zvtx_dist_original[file_i] -> GetBinContent(zvtx_i);
        //     double original_bin_error   = fine_zvtx_dist_original[file_i] -> GetBinError(zvtx_i);
        //     double MC_bin_content = fine_zvtx_dist_original[0] -> GetBinContent(zvtx_i);
        //     double MC_bin_error   = fine_zvtx_dist_original[0] -> GetBinError(zvtx_i);



        //     double reweight_factor       = (MC_bin_content != 0) ? original_bin_content / MC_bin_content : 0;
        //     double reweight_factor_error = (MC_bin_content != 0) ? reweight_factor * sqrt( pow(1./sqrt(original_bin_content),2) + pow(1./sqrt(MC_bin_content),2) ) : 0;
        //     cout<<"test : "<<zvtx_i<<" "<<reweight_factor<<" "<<reweight_factor_error<<" "<<original_bin_error<<" "<<MC_bin_error<<endl;
        //     // fine_zvtx_MC_reweight[file_i] -> SetBinContent( zvtx_i, fine_zvtx_MC_reweight[file_i] -> GetBinContent(zvtx_i) * reweight_factor);
        //     fine_zvtx_reweight_ratio[file_i] -> SetBinContent(zvtx_i, reweight_factor);
        //     fine_zvtx_reweight_ratio[file_i] -> SetBinError(zvtx_i, reweight_factor_error);
        // }

        // for (int zvtx_i = 1; zvtx_i <= coarse_zvtx_dist_original[file_i] -> GetNbinsX(); zvtx_i++)
        // {
        //     double original_bin_content = coarse_zvtx_dist_original[file_i] -> GetBinContent(zvtx_i);
        //     double MC_bin_content = coarse_zvtx_dist_original[0] -> GetBinContent(zvtx_i);

        //     double reweight_factor = (MC_bin_content != 0) ? original_bin_content / MC_bin_content : 0;
        //     double reweight_factor_error = (MC_bin_content != 0) ? reweight_factor * sqrt( pow(1./sqrt(original_bin_content),2) + pow(1./sqrt(MC_bin_content),2) ) : 0;
        //     // coarse_zvtx_MC_reweight[file_i] -> SetBinContent(zvtx_i, coarse_zvtx_MC_reweight[file_i] -> GetBinContent(zvtx_i) * reweight_factor);
        //     coarse_zvtx_reweight_ratio[file_i] -> SetBinContent(zvtx_i, reweight_factor);
        //     coarse_zvtx_reweight_ratio[file_i] -> SetBinError(zvtx_i, reweight_factor_error);
        // }
    }
}

void zvtx_comp::DrawCoordLine(TH1F * hist_in)
{
    coord_line -> DrawLine( 
        coarse_zvtx_reweight_ratio[0] -> GetXaxis() -> GetBinLowEdge( (ana_map_version::evt_tracklet_fulleta_MC_included_z_bin).front() ),
        0,
        coarse_zvtx_reweight_ratio[0] -> GetXaxis() -> GetBinLowEdge( (ana_map_version::evt_tracklet_fulleta_MC_included_z_bin).front() ),
        hist_in -> GetMaximum()
    );

    coord_line -> DrawLine( 
        coarse_zvtx_reweight_ratio[0] -> GetXaxis() -> GetBinLowEdge( (ana_map_version::evt_tracklet_fulleta_MC_included_z_bin).back() ) + coarse_zvtx_reweight_ratio[0] -> GetXaxis() -> GetBinWidth( (ana_map_version::evt_tracklet_fulleta_MC_included_z_bin).back() ),
        0,
        coarse_zvtx_reweight_ratio[0] -> GetXaxis() -> GetBinLowEdge( (ana_map_version::evt_tracklet_fulleta_MC_included_z_bin).back() ) + coarse_zvtx_reweight_ratio[0] -> GetXaxis() -> GetBinWidth( (ana_map_version::evt_tracklet_fulleta_MC_included_z_bin).back() ),
        hist_in -> GetMaximum()
    );

    // note : the horizontal line
    coord_line -> DrawLine( 
        hist_in -> GetXaxis() -> GetXmin(),
        1.,
        hist_in -> GetXaxis() -> GetXmax(),
        1.
    );
}

void zvtx_comp::PrintPlots()
{
    c1 -> cd();

    // note : ---------------------------------------------------------------------------------------------------------------------------
    // note : for the original distribution
    fine_zvtx_dist_original[0] -> SetLineColor(TColor::GetColor(color_code_vec[0].c_str()));
    fine_zvtx_dist_original[0] -> SetMarkerColor(TColor::GetColor(color_code_vec[0].c_str()));
    fine_zvtx_dist_original[0] -> SetMinimum(0);
    fine_zvtx_dist_original[0] -> SetMaximum(1.8 * fine_zvtx_dist_original[0] -> GetBinContent( fine_zvtx_dist_original[0] -> GetMaximumBin() ) );
    fine_zvtx_dist_original[0] -> GetXaxis() -> SetNdivisions(505);
    legend -> AddEntry(fine_zvtx_dist_original[0], Form("%s reco.", input_file_full_info[0][3].c_str()), "f");

    fine_zvtx_dist_original[0] -> Draw("hist");

    for (int file_i = 1; file_i < input_file_full_info.size(); file_i++)
    {
        fine_zvtx_dist_original[file_i] -> SetLineColor(TColor::GetColor(color_code_vec[file_i].c_str()));
        fine_zvtx_dist_original[file_i] -> SetMarkerColor(TColor::GetColor(color_code_vec[file_i].c_str()));
        legend -> AddEntry(fine_zvtx_dist_original[file_i], Form("%s reco.", input_file_full_info[file_i][3].c_str()), "ep");

        fine_zvtx_dist_original[file_i] -> Draw("p same");
    }

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));
    draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",inclusive_centrality_region.c_str()));
    legend -> Draw("same");

    c1 -> Print(Form("%s/fine_zvtx_dist_original.pdf", output_folder_name.c_str()));
    c1 -> Clear();
    legend -> Clear();

    gStyle->SetPaintTextFormat("4.4f");

    // note : ---------------------------------------------------------------------------------------------------------------------------
    // note : for the reweighting factor
    for (int file_i = 1; file_i < input_file_full_info.size(); file_i++)
    {
        fine_zvtx_reweight_ratio[file_i] -> SetLineColor(kBlack);
        fine_zvtx_reweight_ratio[file_i] -> SetMarkerColor(kBlack);
        fine_zvtx_reweight_ratio[file_i] -> SetMinimum(0);
        fine_zvtx_reweight_ratio[file_i] -> SetMaximum(1.8 * fine_zvtx_reweight_ratio[file_i] -> GetBinContent( fine_zvtx_reweight_ratio[file_i] -> GetMaximumBin() ) );
        fine_zvtx_reweight_ratio[file_i] -> GetXaxis() -> SetNdivisions(505);
        fine_zvtx_reweight_ratio[file_i] -> Draw("ep");
        fine_zvtx_reweight_ratio[file_i] -> Draw("TEXT90 SAME");

        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",inclusive_centrality_region.c_str()));
        DrawCoordLine(fine_zvtx_reweight_ratio[file_i]);

        c1 -> Print(Form("%s/fine_zvtx_reweight_ratio.pdf", output_folder_name.c_str()));
        c1 -> Clear();


        fine_zvtx_reweight_ratio[file_i] -> SetMaximum( 3. );
        fine_zvtx_reweight_ratio[file_i] -> Draw("ep");
        fine_zvtx_reweight_ratio[file_i] -> Draw("TEXT90 SAME");
        DrawCoordLine(fine_zvtx_reweight_ratio[file_i]);

        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",inclusive_centrality_region.c_str()));
        c1 -> Print(Form("%s/fine_zvtx_reweight_ratio_zoomin.pdf", output_folder_name.c_str()));
        c1 -> Clear();

    }


    // note : ---------------------------------------------------------------------------------------------------------------------------
    // note : for the original distribution
    coarse_zvtx_dist_original[0] -> SetLineColor(TColor::GetColor(color_code_vec[0].c_str()));
    coarse_zvtx_dist_original[0] -> SetMarkerColor(TColor::GetColor(color_code_vec[0].c_str()));
    coarse_zvtx_dist_original[0] -> SetMinimum(0);
    coarse_zvtx_dist_original[0] -> SetMaximum(1.8 * coarse_zvtx_dist_original[0] -> GetBinContent( coarse_zvtx_dist_original[0] -> GetMaximumBin() ) );
    coarse_zvtx_dist_original[0] -> GetXaxis() -> SetNdivisions(505);
    legend -> AddEntry(coarse_zvtx_dist_original[0], Form("%s reco.", input_file_full_info[0][3].c_str()), "f");

    coarse_zvtx_dist_original[0] -> Draw("hist");

    for (int file_i = 1; file_i < input_file_full_info.size(); file_i++)
    {
        coarse_zvtx_dist_original[file_i] -> SetLineColor(TColor::GetColor(color_code_vec[file_i].c_str()));
        coarse_zvtx_dist_original[file_i] -> SetMarkerColor(TColor::GetColor(color_code_vec[file_i].c_str()));
        legend -> AddEntry(coarse_zvtx_dist_original[file_i], Form("%s reco.", input_file_full_info[file_i][3].c_str()), "ep");

        coarse_zvtx_dist_original[file_i] -> Draw("p same");
    }

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));
    draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",inclusive_centrality_region.c_str()));
    legend -> Draw("same");

    c1 -> Print(Form("%s/coarse_zvtx_dist_original.pdf", output_folder_name.c_str()));
    c1 -> Clear();
    legend -> Clear();

    // note : ---------------------------------------------------------------------------------------------------------------------------
    // note : for the reweighting factor
    for (int file_i = 1; file_i < input_file_full_info.size(); file_i++)
    {
        coarse_zvtx_reweight_ratio[file_i] -> SetLineColor(kBlack);
        coarse_zvtx_reweight_ratio[file_i] -> SetMarkerColor(kBlack);
        coarse_zvtx_reweight_ratio[file_i] -> SetMinimum(0);
        coarse_zvtx_reweight_ratio[file_i] -> SetMaximum(1.8 * coarse_zvtx_reweight_ratio[file_i] -> GetBinContent( coarse_zvtx_reweight_ratio[file_i] -> GetMaximumBin() ) );
        coarse_zvtx_reweight_ratio[file_i] -> GetXaxis() -> SetNdivisions(505);
        coarse_zvtx_reweight_ratio[file_i] -> Draw("ep");
        coarse_zvtx_reweight_ratio[file_i] -> Draw("TEXT90 SAME");
        DrawCoordLine(coarse_zvtx_reweight_ratio[file_i]);

        // note : print the bin center and bin content 
        for (int bin_i = 1; bin_i <= coarse_zvtx_reweight_ratio[file_i] -> GetNbinsX(); bin_i++) {
            cout<<"file_i : "<<file_i<<" bin : "<<bin_i<<" "<<coarse_zvtx_reweight_ratio[file_i] -> GetBinCenter(bin_i)<<" "<<coarse_zvtx_reweight_ratio[file_i] -> GetBinContent(bin_i)<<endl;
        }

        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",inclusive_centrality_region.c_str()));

        c1 -> Print(Form("%s/coarse_zvtx_reweight_ratio.pdf", output_folder_name.c_str()));
        c1 -> Clear();


        coarse_zvtx_reweight_ratio[file_i] -> SetMaximum( 3. );
        coarse_zvtx_reweight_ratio[file_i] -> Draw("ep");
        coarse_zvtx_reweight_ratio[file_i] -> Draw("TEXT90 SAME");
        DrawCoordLine(coarse_zvtx_reweight_ratio[file_i]);

        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",inclusive_centrality_region.c_str()));
        c1 -> Print(Form("%s/coarse_zvtx_reweight_ratio_zoomin.pdf", output_folder_name.c_str()));
        c1 -> Clear();
    }

    return;
}

int zvtx_comparison()
{

    string MC_zvtx_complete_folder = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_vtxZ/complete_file";
    string data_zvtx_complete_folder = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test/evt_vtxZ/complete_file";

    vector<vector<string>> input_file_full_info = {
        {
            "MC",
            Form("%s/merged_file.root",MC_zvtx_complete_folder.c_str()),
            "tree_Z",
            "MC"
        },
        {
            "data",
            Form("%s/merged_file.root",data_zvtx_complete_folder.c_str()),
            "tree_Z",
            "Data"
        }
    };

    string output_folder_name = data_zvtx_complete_folder + "/zvtx_comparison_original";

    zvtx_comp * zvtx_comp_obj = new zvtx_comp(input_file_full_info, output_folder_name);

    return 0;
}
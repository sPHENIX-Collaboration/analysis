#include "CheckEvtXY.h"

CheckEvtXY::CheckEvtXY(
    string mother_folder_directory_in, 
    string input_file_list_in, 
    string out_folder_directory_in, 
    std::pair<double, double> beam_origin_in, 
    int NClus_cut_label_in,
    int unit_tag_in,
    string run_type_in
)
: 
mother_folder_directory(mother_folder_directory_in), 
input_file_list(input_file_list_in), 
out_folder_directory(out_folder_directory_in), 
beam_origin(beam_origin_in), 
NClus_cut_label(NClus_cut_label_in),
unit_tag(unit_tag_in),
run_type(run_type_in)
{
    unit_correction = (unit_tag == 1) ? 0.1 : 1.;
    unit_text = (unit_tag == 1) ? "cm" : "mm";
    sPHENIX_suffix = (run_type == "MC") ? "Simulation" : "Work-in-progress";
    
    beam_origin.first = beam_origin.first * unit_correction;
    beam_origin.second = beam_origin.second * unit_correction; 

    SetsPhenixStyle();
    Init();
}

void CheckEvtXY::Init()
{

    data_in = new ReadEvtXY_Chain(mother_folder_directory, input_file_list);
    cout<<" data_in, number of entries: "<<data_in -> GetEntries()<<endl;

    // note : in order to find the last eID, for the range setting
    data_in -> LoadTree(data_in -> GetEntries() - 1);
    data_in -> GetEntry(data_in -> GetEntries() - 1);

    c1 = new TCanvas("c1", "c1", 950, 800);

    cout<<"input beam_origin : "<<beam_origin.first<<", "<<beam_origin.second<<Form(" in unit : %s",unit_text.c_str())<<endl;;

    // note : unit is still mm
    double window_half_width = 1.; window_half_width = window_half_width * unit_correction;
    double small_half_width = 0.75; small_half_width = small_half_width * unit_correction;
    double diff_half_width = 0.5; diff_half_width = diff_half_width * unit_correction;

    vtxX_1D = new TH1F("","vtxX_1D", 100, -window_half_width + beam_origin.first, window_half_width + beam_origin.first);
    vtxX_1D -> SetStats(0);
    vtxX_1D -> GetXaxis() -> SetTitle(Form("Vertex X [%s]", unit_text.c_str()));
    vtxX_1D -> GetYaxis() -> SetTitle("Entry");
    vtxX_1D -> GetXaxis() -> SetNdivisions(505);

    vtxY_1D = new TH1F("","vtxY_1D", 100, -window_half_width + beam_origin.second, window_half_width + beam_origin.second);
    vtxY_1D -> SetStats(0);
    vtxY_1D -> GetXaxis() -> SetTitle(Form("Vertex Y [%s]", unit_text.c_str()));
    vtxY_1D -> GetYaxis() -> SetTitle("Entry");
    vtxY_1D -> GetXaxis() -> SetNdivisions(505);

    vtxX_eID_2D = new TH2F("","vtxX_eID_2D", 200, 0, int(data_in -> eID * 1.3), 200, vtxX_1D -> GetXaxis() -> GetXmin(), vtxX_1D -> GetXaxis() -> GetXmax());
    vtxX_eID_2D -> SetStats(0);
    vtxX_eID_2D -> GetXaxis() -> SetTitle("Event ID");
    vtxX_eID_2D -> GetYaxis() -> SetTitle(Form("Vertex X [%s]", unit_text.c_str()));
    vtxX_eID_2D -> GetXaxis() -> SetNdivisions(505);
    
    vtxY_eID_2D = new TH2F("","vtxY_eID_2D", 200, 0, int(data_in -> eID * 1.3), 200, vtxY_1D -> GetXaxis() -> GetXmin(), vtxY_1D -> GetXaxis() -> GetXmax());
    vtxY_eID_2D -> SetStats(0);
    vtxY_eID_2D -> GetXaxis() -> SetTitle("Event ID");
    vtxY_eID_2D -> GetYaxis() -> SetTitle(Form("Vertex Y [%s]", unit_text.c_str()));
    vtxY_eID_2D -> GetXaxis() -> SetNdivisions(505);

    vtxXY_2D = new TH2F("","vtxXY_2D", 100, -window_half_width + beam_origin.first, window_half_width + beam_origin.first, 100, -window_half_width + beam_origin.second, window_half_width + beam_origin.second);
    // vtxXY_2D -> SetStats(0);
    vtxXY_2D -> GetXaxis() -> SetTitle(Form("X axis [%s]", unit_text.c_str()));
    vtxXY_2D -> GetYaxis() -> SetTitle(Form("Y axis [%s]", unit_text.c_str()));
    vtxXY_2D -> GetXaxis() -> SetNdivisions(505);

    // note : for the MC comparison
    vtxX_correlation = new TH2F("", Form("vtxX_correlation;True vertex X [%s];Reco. vertex X [%s]", unit_text.c_str(), unit_text.c_str()), 100, -small_half_width + beam_origin.first, small_half_width + beam_origin.first, 100, -small_half_width + beam_origin.first, small_half_width + beam_origin.first);
    vtxX_correlation -> GetXaxis() -> SetNdivisions(505);    
    
    vtxY_correlation = new TH2F("", Form("vtxY_correlation;True vertex Y [%s];Reco. vertex Y [%s]", unit_text.c_str(), unit_text.c_str()), 100, -small_half_width + beam_origin.second, small_half_width + beam_origin.second, 100, -small_half_width + beam_origin.second, small_half_width + beam_origin.second);
    vtxY_correlation -> GetXaxis() -> SetNdivisions(505);    
    
    vtxX_diff = new TH1F("",Form("vtxX_diff;#DeltaX (Reco. - True) [%s];Entry",unit_text.c_str()), 100, -diff_half_width, diff_half_width);
    vtxX_diff -> GetXaxis() -> SetNdivisions(505);    
    
    vtxY_diff = new TH1F("",Form("vtxY_diff;#DeltaY (Reco. - True) [%s];Entry",unit_text.c_str()), 100, -diff_half_width, diff_half_width);
    vtxY_diff -> GetXaxis() -> SetNdivisions(505);
     
    vtxX_bco_graph = new TGraph(); 
    vtxX_bco_graph -> Set(0);
    vtxX_bco_graph -> GetXaxis() -> SetNdivisions(505);

    vtxY_bco_graph = new TGraph(); 
    vtxY_bco_graph -> Set(0);
    vtxY_bco_graph -> GetXaxis() -> SetNdivisions(505);

    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    ltx_warning = new TLatex();
    ltx_warning->SetNDC();
    ltx_warning->SetTextSize(0.04);
    ltx_warning->SetTextAlign(31);
    ltx_warning->SetTextColor(2);
    ltx_warning->SetTextAngle(35);

    draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    gaus_fit_MC = new TF1("gaus_fit_MC",gaus_func,-10,10,4);
    gaus_fit_MC -> SetLineColor(2);
    gaus_fit_MC -> SetLineWidth(2);
    gaus_fit_MC -> SetNpx(1000);

    pol1_fit = new TF1("pol1_fit", "pol1",-100,100);
    pol1_fit -> SetLineColor(2);
    pol1_fit -> SetLineWidth(2);
}

void CheckEvtXY::Prepare_info()
{
    for (int i = 0; i < data_in -> GetEntries(); i++)
    {
        data_in -> LoadTree(i);
        data_in -> GetEntry(i);

        double reco_vtx_x = data_in -> reco_vtx_x -> at(0) * unit_correction;
        double reco_vtx_y = data_in -> reco_vtx_y -> at(0) * unit_correction;
        double true_vtx_x = data_in -> true_vtx_x * unit_correction;
        double true_vtx_y = data_in -> true_vtx_y * unit_correction;

        vtxX_1D ->  Fill(reco_vtx_x);
        vtxY_1D ->  Fill(reco_vtx_y);
        vtxXY_2D -> Fill(reco_vtx_x, reco_vtx_y);

        vtxX_bco_graph -> SetPoint(i, data_in -> bco_full, reco_vtx_x);
        vtxY_bco_graph -> SetPoint(i, data_in -> bco_full, reco_vtx_y);

        vtxX_eID_2D -> Fill(data_in -> eID, reco_vtx_x);
        vtxY_eID_2D -> Fill(data_in -> eID, reco_vtx_y);

        if (run_type == "MC")
        {
            // cout<<" test :  "<<true_vtx_x<<" "<<reco_vtx_x<<"~~~~"<<true_vtx_y<<" "<<reco_vtx_y<<endl;
            vtxX_correlation -> Fill(true_vtx_x, reco_vtx_x);
            vtxY_correlation -> Fill(true_vtx_y, reco_vtx_y);
            vtxX_diff -> Fill(reco_vtx_x - true_vtx_x);
            vtxY_diff -> Fill(reco_vtx_y - true_vtx_y);
        }

    }

    return;
}

void CheckEvtXY::Print_plots()
{
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxX_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    // ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));

    gaus_fit_MC -> SetParameters(vtxX_1D -> GetBinContent( vtxX_1D -> GetMaximumBin() ), vtxX_1D -> GetBinCenter( vtxX_1D -> GetMaximumBin() ), 0.05 * unit_correction, 0);
    gaus_fit_MC -> SetParLimits(0,0,100000);  // note : size 
    gaus_fit_MC -> SetParLimits(2,0,10000);   // note : Width
    gaus_fit_MC -> SetParLimits(3,0,10000);   // note : offset
    vtxX_1D -> Fit(gaus_fit_MC, "N", "", vtxX_1D -> GetBinCenter( vtxX_1D -> GetMaximumBin() ) - (2 * vtxX_1D -> GetStdDev() ), vtxX_1D -> GetBinCenter( vtxX_1D -> GetMaximumBin() ) + (2 * vtxX_1D -> GetStdDev() ) );
    gaus_fit_MC -> SetRange( gaus_fit_MC->GetParameter(1) - double(gaus_fit_MC->GetParameter(2)) * 2.5, gaus_fit_MC->GetParameter(1) + double(gaus_fit_MC->GetParameter(2)) * 2.5 ); 
    gaus_fit_MC -> Draw("lsame");

    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    draw_text -> DrawLatex(0.21, 0.80, Form("Gaus mean  : %.5f %s",gaus_fit_MC -> GetParameter(1), unit_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.76, Form("Gaus width : %.5f %s",fabs(gaus_fit_MC -> GetParameter(2)), unit_text.c_str()));
    c1 -> Print(Form("%s/vtxX_1D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxY_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    // ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));

    gaus_fit_MC -> SetParameters(vtxY_1D -> GetBinContent( vtxY_1D -> GetMaximumBin() ), vtxY_1D -> GetBinCenter( vtxY_1D -> GetMaximumBin() ), 0.05 * unit_correction, 0);
    gaus_fit_MC -> SetParLimits(0,0,100000);  // note : size 
    gaus_fit_MC -> SetParLimits(2,0,10000);   // note : Width
    gaus_fit_MC -> SetParLimits(3,0,10000);   // note : offset
    vtxY_1D -> Fit(gaus_fit_MC, "N", "", vtxY_1D -> GetBinCenter( vtxY_1D -> GetMaximumBin() ) - (2 * vtxY_1D -> GetStdDev() ), vtxY_1D -> GetBinCenter( vtxY_1D -> GetMaximumBin() ) + (2 * vtxY_1D -> GetStdDev() ) );
    gaus_fit_MC -> SetRange( gaus_fit_MC->GetParameter(1) - double(gaus_fit_MC->GetParameter(2)) * 2.5, gaus_fit_MC->GetParameter(1) + double(gaus_fit_MC->GetParameter(2)) * 2.5 ); 
    gaus_fit_MC -> Draw("lsame");
    
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    draw_text -> DrawLatex(0.21, 0.80, Form("Gaus mean  : %.5f %s",gaus_fit_MC -> GetParameter(1), unit_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.76, Form("Gaus width : %.5f %s",fabs(gaus_fit_MC -> GetParameter(2)), unit_text.c_str()));
    c1 -> Print(Form("%s/vtxY_1D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxXY_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    // ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    c1 -> Print(Form("%s/vtxXY_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxX_eID_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    // ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    c1 -> Print(Form("%s/vtxX_eID_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxY_eID_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    // ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    c1 -> Print(Form("%s/vtxY_eID_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxX_bco_graph -> SetMarkerStyle(20);
    vtxX_bco_graph -> SetMarkerSize(0.1);
    vtxX_bco_graph -> Draw("AP");
    vtxX_bco_graph -> GetXaxis() -> SetTitle("bco_full");
    vtxX_bco_graph -> GetYaxis() -> SetTitle(Form("Vertex X [%s]", unit_text.c_str()));
    vtxX_bco_graph -> GetYaxis() -> SetRangeUser(vtxX_1D->GetXaxis()->GetXmin(), vtxX_1D->GetXaxis()->GetXmax());
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    // ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    c1 -> Print(Form("%s/vtxX_bco_graph.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxY_bco_graph -> SetMarkerStyle(20);
    vtxY_bco_graph -> SetMarkerSize(0.1);
    vtxY_bco_graph -> Draw("AP");
    vtxY_bco_graph -> GetXaxis() -> SetTitle("bco_full");
    vtxY_bco_graph -> GetYaxis() -> SetTitle(Form("Vertex Y [%s]", unit_text.c_str()));
    vtxY_bco_graph -> GetYaxis() -> SetRangeUser(vtxY_1D->GetXaxis()->GetXmin(), vtxY_1D->GetXaxis()->GetXmax());
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    // ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    c1 -> Print(Form("%s/vtxY_bco_graph.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    if (run_type != "MC") {return;}

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxX_correlation -> Fit(pol1_fit, "NQ");
    vtxX_correlation -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    pol1_fit -> Draw("l same");
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    draw_text -> DrawLatex(0.21, 0.80, Form("y = %.3f + %.3f", pol1_fit -> GetParameter(1), pol1_fit -> GetParameter(0)));
    c1 -> Print(Form("%s/vtxX_correlation.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxY_correlation -> Fit(pol1_fit, "NQ");
    vtxY_correlation -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    pol1_fit -> Draw("l same");
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    draw_text -> DrawLatex(0.21, 0.80, Form("y = %.3f + %.3f", pol1_fit -> GetParameter(1), pol1_fit -> GetParameter(0)));
    c1 -> Print(Form("%s/vtxY_correlation.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxX_diff -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    
    gaus_fit_MC -> SetParameters(vtxX_diff -> GetBinContent( vtxX_diff -> GetMaximumBin() ), vtxX_diff -> GetBinCenter( vtxX_diff -> GetMaximumBin() ), 0.05 * unit_correction, 0);
    gaus_fit_MC -> SetParLimits(0,0,100000);  // note : size 
    gaus_fit_MC -> SetParLimits(2,0,10000);   // note : Width
    gaus_fit_MC -> SetParLimits(3,0,10000);   // note : offset
    vtxX_diff -> Fit(gaus_fit_MC, "N", "", vtxX_diff -> GetBinCenter( vtxX_diff -> GetMaximumBin() ) - (1 * vtxX_diff -> GetStdDev() ), vtxX_diff -> GetBinCenter( vtxX_diff -> GetMaximumBin() ) + (1 * vtxX_diff -> GetStdDev() ) );
    gaus_fit_MC -> SetRange( gaus_fit_MC->GetParameter(1) - double(gaus_fit_MC->GetParameter(2)) * 2, gaus_fit_MC->GetParameter(1) + double(gaus_fit_MC->GetParameter(2)) * 2 ); 
    gaus_fit_MC -> Draw("lsame");

    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    draw_text -> DrawLatex(0.21, 0.80, Form("Gaus mean  : %.5f %s",gaus_fit_MC -> GetParameter(1), unit_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.76, Form("Gaus width : %.5f %s",fabs(gaus_fit_MC -> GetParameter(2)), unit_text.c_str()));
    c1 -> Print(Form("%s/vtxX_diff.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxY_diff -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPHENIX_suffix.c_str()));
    
    gaus_fit_MC -> SetParameters(vtxY_diff -> GetBinContent( vtxY_diff -> GetMaximumBin() ), vtxY_diff -> GetBinCenter( vtxY_diff -> GetMaximumBin() ), 0.05 * unit_correction, 0);
    gaus_fit_MC -> SetParLimits(0,0,100000);  // note : size 
    gaus_fit_MC -> SetParLimits(2,0,10000);   // note : Width
    gaus_fit_MC -> SetParLimits(3,0,10000);   // note : offset
    vtxY_diff -> Fit(gaus_fit_MC, "N", "", vtxY_diff -> GetBinCenter( vtxY_diff -> GetMaximumBin() ) - (1 * vtxY_diff -> GetStdDev() ), vtxY_diff -> GetBinCenter( vtxY_diff -> GetMaximumBin() ) + (1 * vtxY_diff -> GetStdDev() ) );
    gaus_fit_MC -> SetRange( gaus_fit_MC->GetParameter(1) - double(gaus_fit_MC->GetParameter(2)) * 2, gaus_fit_MC->GetParameter(1) + double(gaus_fit_MC->GetParameter(2)) * 2 ); 
    gaus_fit_MC -> Draw("lsame");

    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > %i", NClus_cut_label));
    draw_text -> DrawLatex(0.21, 0.80, Form("Gaus mean  : %.5f %s",gaus_fit_MC -> GetParameter(1), unit_text.c_str()));
    draw_text -> DrawLatex(0.21, 0.76, Form("Gaus width : %.5f %s",fabs(gaus_fit_MC -> GetParameter(2)), unit_text.c_str()));
    c1 -> Print(Form("%s/vtxY_diff.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    return;
}

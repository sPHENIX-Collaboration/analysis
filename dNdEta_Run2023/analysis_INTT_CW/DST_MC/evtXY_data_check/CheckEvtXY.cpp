#include "CheckEvtXY.h"

CheckEvtXY::CheckEvtXY(string mother_folder_directory, string input_file_list, string out_folder_directory, std::pair<double, double> beam_origin)
: mother_folder_directory(mother_folder_directory), input_file_list(input_file_list), out_folder_directory(out_folder_directory), beam_origin(beam_origin)
{
    SetsPhenixStyle();
    Init();
}

void CheckEvtXY::Init()
{

    data_in = new ReadEvtXY_Chain(mother_folder_directory, input_file_list);
    cout<<" data_in, number of entries: "<<data_in -> GetEntries()<<endl;

    c1 = new TCanvas("c1", "c1", 950, 800);

    vtxX_1D = new TH1F("","vtxX_1D", 100, -2 + beam_origin.first, 2 + beam_origin.first);
    vtxX_1D -> SetStats(0);
    vtxX_1D -> GetXaxis() -> SetTitle("X axis [mm]");
    vtxX_1D -> GetYaxis() -> SetTitle("Entry");
    vtxX_1D -> GetXaxis() -> SetNdivisions(505);

    vtxY_1D = new TH1F("","vtxY_1D", 100, -2 + beam_origin.second, 2 + beam_origin.second);
    vtxY_1D -> SetStats(0);
    vtxY_1D -> GetXaxis() -> SetTitle("Y axis [mm]");
    vtxY_1D -> GetYaxis() -> SetTitle("Entry");
    vtxY_1D -> GetXaxis() -> SetNdivisions(505);

    vtxX_eID_2D = new TH2F("","vtxX_eID_2D", 200, 0, 600000, 200, vtxX_1D -> GetXaxis() -> GetXmin(), vtxX_1D -> GetXaxis() -> GetXmax());
    vtxX_eID_2D -> SetStats(0);
    vtxX_eID_2D -> GetXaxis() -> SetTitle("Event ID");
    vtxX_eID_2D -> GetYaxis() -> SetTitle("vtxX [mm]");
    vtxX_eID_2D -> GetXaxis() -> SetNdivisions(505);
    
    vtxY_eID_2D = new TH2F("","vtxY_eID_2D", 200, 0, 600000, 200, vtxY_1D -> GetXaxis() -> GetXmin(), vtxY_1D -> GetXaxis() -> GetXmax());
    vtxY_eID_2D -> SetStats(0);
    vtxY_eID_2D -> GetXaxis() -> SetTitle("Event ID");
    vtxY_eID_2D -> GetYaxis() -> SetTitle("vtxY [mm]");
    vtxY_eID_2D -> GetXaxis() -> SetNdivisions(505);

    vtxXY_2D = new TH2F("","vtxXY_2D", 100, -2 + beam_origin.first, 2 + beam_origin.first, 100, -2 + beam_origin.second, 2 + beam_origin.second);
    // vtxXY_2D -> SetStats(0);
    vtxXY_2D -> GetXaxis() -> SetTitle("X axis [mm]");
    vtxXY_2D -> GetYaxis() -> SetTitle("Y axis [mm]");
    
    vtxX_bco_graph = new TGraph(); vtxX_bco_graph -> Set(0);
    vtxY_bco_graph = new TGraph(); vtxY_bco_graph -> Set(0);

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



}

void CheckEvtXY::Prepare_info()
{
    for (int i = 0; i < data_in -> GetEntries(); i++)
    {
        data_in -> LoadTree(i);
        data_in -> GetEntry(i);

        vtxX_1D ->  Fill(data_in -> reco_vtx_x -> at(0));
        vtxY_1D ->  Fill(data_in -> reco_vtx_y -> at(0));
        vtxXY_2D -> Fill(data_in -> reco_vtx_x -> at(0), data_in -> reco_vtx_y -> at(0));

        vtxX_bco_graph -> SetPoint(i, data_in -> bco_full, data_in -> reco_vtx_x -> at(0));
        vtxY_bco_graph -> SetPoint(i, data_in -> bco_full, data_in -> reco_vtx_y -> at(0));

        vtxX_eID_2D -> Fill(data_in -> eID, data_in -> reco_vtx_x -> at(0));
        vtxY_eID_2D -> Fill(data_in -> eID, data_in -> reco_vtx_y -> at(0));
    }

    return;
}

void CheckEvtXY::Print_plots()
{
    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxX_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));

    gaus_fit_MC -> SetParameters(vtxX_1D -> GetBinContent( vtxX_1D -> GetMaximumBin() ), vtxX_1D -> GetBinCenter( vtxX_1D -> GetMaximumBin() ), 0.05, 0);
    gaus_fit_MC -> SetParLimits(0,0,100000);  // note : size 
    gaus_fit_MC -> SetParLimits(2,0,10000);   // note : Width
    gaus_fit_MC -> SetParLimits(3,0,10000);   // note : offset
    vtxX_1D -> Fit(gaus_fit_MC, "N", "", vtxX_1D -> GetBinCenter( vtxX_1D -> GetMaximumBin() ) - (2 * vtxX_1D -> GetStdDev() ), vtxX_1D -> GetBinCenter( vtxX_1D -> GetMaximumBin() ) + (2 * vtxX_1D -> GetStdDev() ) );
    gaus_fit_MC -> SetRange( gaus_fit_MC->GetParameter(1) - double(gaus_fit_MC->GetParameter(2)) * 2.5, gaus_fit_MC->GetParameter(1) + double(gaus_fit_MC->GetParameter(2)) * 2.5 ); 
    gaus_fit_MC -> Draw("lsame");

    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > 3000"));
    draw_text -> DrawLatex(0.21, 0.71, Form("Gaus mean  : %.4f mm",gaus_fit_MC -> GetParameter(1)));
    draw_text -> DrawLatex(0.21, 0.67, Form("Gaus width : %.4f mm",fabs(gaus_fit_MC -> GetParameter(2))));
    c1 -> Print(Form("%s/vtxX_1D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxY_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));

    gaus_fit_MC -> SetParameters(vtxY_1D -> GetBinContent( vtxY_1D -> GetMaximumBin() ), vtxY_1D -> GetBinCenter( vtxY_1D -> GetMaximumBin() ), 0.05, 0);
    gaus_fit_MC -> SetParLimits(0,0,100000);  // note : size 
    gaus_fit_MC -> SetParLimits(2,0,10000);   // note : Width
    gaus_fit_MC -> SetParLimits(3,0,10000);   // note : offset
    vtxY_1D -> Fit(gaus_fit_MC, "N", "", vtxY_1D -> GetBinCenter( vtxY_1D -> GetMaximumBin() ) - (2 * vtxY_1D -> GetStdDev() ), vtxY_1D -> GetBinCenter( vtxY_1D -> GetMaximumBin() ) + (2 * vtxY_1D -> GetStdDev() ) );
    gaus_fit_MC -> SetRange( gaus_fit_MC->GetParameter(1) - double(gaus_fit_MC->GetParameter(2)) * 2.5, gaus_fit_MC->GetParameter(1) + double(gaus_fit_MC->GetParameter(2)) * 2.5 ); 
    gaus_fit_MC -> Draw("lsame");
    
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > 3000"));
    draw_text -> DrawLatex(0.21, 0.71, Form("Gaus mean  : %.4f mm",gaus_fit_MC -> GetParameter(1)));
    draw_text -> DrawLatex(0.21, 0.67, Form("Gaus width : %.4f mm",fabs(gaus_fit_MC -> GetParameter(2))));
    c1 -> Print(Form("%s/vtxY_1D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxXY_2D -> Draw("colz");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > 3000"));
    c1 -> Print(Form("%s/vtxXY_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxX_eID_2D -> Draw("colz");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > 3000"));
    c1 -> Print(Form("%s/vtxX_eID_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxY_eID_2D -> Draw("colz");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > 3000"));
    c1 -> Print(Form("%s/vtxY_eID_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxY_bco_graph -> SetMarkerStyle(20);
    vtxY_bco_graph -> SetMarkerSize(0.01);
    vtxX_bco_graph -> Draw("AP");
    vtxX_bco_graph -> GetXaxis() -> SetTitle("bco_full");
    vtxX_bco_graph -> GetYaxis() -> SetTitle("vtxX [mm]");
    vtxX_bco_graph -> GetYaxis() -> SetRangeUser(vtxX_1D->GetXaxis()->GetXmin(), vtxX_1D->GetXaxis()->GetXmax());
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > 3000"));
    c1 -> Print(Form("%s/vtxX_bco_graph.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    // note : ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    c1 -> cd();
    vtxY_bco_graph -> SetMarkerStyle(20);
    vtxY_bco_graph -> SetMarkerSize(0.01);
    vtxY_bco_graph -> Draw("AP");
    vtxY_bco_graph -> GetXaxis() -> SetTitle("bco_full");
    vtxY_bco_graph -> GetYaxis() -> SetTitle("vtxY [mm]");
    vtxY_bco_graph -> GetYaxis() -> SetRangeUser(vtxY_1D->GetXaxis()->GetXmin(), vtxY_1D->GetXaxis()->GetXmax());
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    ltx_warning->DrawLatex(0.95, 0.88, Form("#color[2]{The data used in the plot is not produced by F4A, will be replaced in two weeks}"));
    draw_text -> DrawLatex(0.21, 0.84, Form("INTT NClus > 3000"));
    c1 -> Print(Form("%s/vtxY_bco_graph.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    return;
}

#include "sPhenixStyle.C"

int MakePlot_INTTzEffi()
{
    std::string input_file_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/VtxZDist/completed"; 
    std::string input_file_name = "MC_vtxZDist_VtxZQA_merged.root";
    std::string target_plot_truth_name = "h2D_TruthCount_TruthZ_Centrality";
    std::string target_plot2_reco_name = "h2D_GoodRecoZ_TruthZ_Centrality";

    std::string output_directory = input_file_directory;
    std::string output_file_name = "h2D_RecoZEffi_TruthZ_Centrality";
    std::string Zaxis_title = "Efficiency";
    std::pair<std::string, std::string> axes_label = {"Truth Z [cm]", "Centrality Interval [%]"};
    
    std::string sPH_label = "Simulation";
    std::vector<std::tuple<double,double,std::string>> additional_text = {
        // {0.2, 0.9, "The cluster pairs post the VtxZ linking requirement are filled"},

        // {0.22, 0.9, "Au+Au 200 GeV"},
        {0.18, 0.96, "HIJING, |VtxZDiff| #leq 1 cm"},
    };
    
    SetsPhenixStyle();

    TCanvas * c1 = new TCanvas("c1", "c1", 950, 800);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    TFile * file_in = TFile::Open(Form("%s/%s", input_file_directory.c_str(), input_file_name.c_str()));
    TH2D * h2D_truth = (TH2D*)file_in->Get(target_plot_truth_name.c_str());
    TH2D * h2D_reco = (TH2D*)file_in->Get(target_plot2_reco_name.c_str());

    TH2D * final = (TH2D*)h2D_reco->Clone("final");
    final -> Sumw2();
    final -> Divide(h2D_reco, h2D_truth, 1, 1);

    final -> GetXaxis() -> SetTitle(axes_label.first.c_str());
    final -> GetYaxis() -> SetTitle(axes_label.second.c_str());
    final -> GetZaxis() -> SetTitle(Zaxis_title.c_str());

    gStyle->SetPaintTextFormat("1.3f");
    
    final -> SetMarkerSize(0.4);
    final -> Draw("colz0 TEXT90 E");
    
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str())); 

    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), output_file_name.c_str()));
    return 0;
}

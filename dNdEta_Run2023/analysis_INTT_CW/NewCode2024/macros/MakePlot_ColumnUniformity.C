#include "sPhenixStyle.C"

int MakePlot_ColumnUniformity()
{
    std::string input_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/ColumnCheck/completed/MulMap/completed"; 
    std::string input_file_name = "MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize39_00054280.root";
    std::string output_directory = input_file_directory;

    std::vector<std::tuple<double,double,std::string>> additional_text = {
        {0.22, 0.82, "Centrality 0-70%"},
        {0.22, 0.78, "|MBD vtxZ| #leq 30 cm"}
    };

    std::string sPH_label_data = "Internal";
    std::string sPH_label_MC = "Simulation";

    system(Form("mkdir -p %s", output_directory.c_str()));
    
    SetsPhenixStyle();

    TCanvas * c1 = new TCanvas("c1", "c1", 950, 800);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    TLegend * leg = new TLegend(0.52,0.81,0.72,0.9);
    // leg -> SetNColumns(2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.025);
    leg -> SetMargin(0.2);

    TFile * file_in = TFile::Open(Form("%s/%s", input_file_directory.c_str(), input_file_name.c_str()));
    if (!file_in){
        std::cout<<"Error: cannot open file: "<<input_file_name<<std::endl;
        return 1;
    }

    
    TH1D * h1D_Ratio_All = (TH1D*)file_in->Get("h1D_Ratio_All");
    h1D_Ratio_All -> SetTitle(";Multiplicity Ratio (data/MC);Entries");
    
    TH2D * h2D_ClusCountLayerPhiId_ZId12_data = (TH2D*)file_in->Get("h2D_ClusCountLayerPhiId_ZId12_data");
    // h2D_ClusCountLayerPhiId_ZId12_data -> SetTitle("");
    
    TH2D * h2D_ClusCountLayerPhiId_ZId12_MC = (TH2D*)file_in->Get("h2D_ClusCountLayerPhiId_ZId12_MC");
    // h2D_ClusCountLayerPhiId_ZId12_MC -> SetTitle("");
    
    TH2D * h2D_MulMap = (TH2D*)file_in->Get("h2D_MulMap");
    // h2D_MulMap -> GetZaxis() -> SetTitle("Ratio (data/MC)");
    
    TH2D * h2D_RatioLayerPhiId_ZId12 = (TH2D*)file_in->Get("h2D_RatioLayerPhiId_ZId12");
    h2D_RatioLayerPhiId_ZId12 -> SetTitle(";INTT LayerID;INTT LadderPhiId;Ratio (data/MC)");
    
    TH2D * h2D_RatioMap = (TH2D*)file_in->Get("h2D_RatioMap");
    h2D_RatioMap -> GetZaxis() -> SetTitle("Ratio (data/MC)");


    c1 -> cd();
    c1 -> SetLogy(1);
    h1D_Ratio_All -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label_data.c_str())); 
    c1 -> Print(Form("%s/h1D_Ratio_All.pdf", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogy(0);


    c1 -> cd();
    h2D_ClusCountLayerPhiId_ZId12_data -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label_data.c_str()));
    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }
    c1 -> Print(Form("%s/h2D_ClusCountLayerPhiId_ZId12_data.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    h2D_ClusCountLayerPhiId_ZId12_MC -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label_MC.c_str()));
    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }
    c1 -> Print(Form("%s/h2D_ClusCountLayerPhiId_ZId12_MC.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    h2D_MulMap -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label_data.c_str()));
    c1 -> Print(Form("%s/h2D_MulMap.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    h2D_RatioLayerPhiId_ZId12 -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label_data.c_str()));
    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }
    c1 -> Print(Form("%s/h2D_RatioLayerPhiId_ZId12.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    h2D_RatioMap -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label_data.c_str()));
    c1 -> Print(Form("%s/h2D_RatioMap.pdf", output_directory.c_str()));
    c1 -> Clear();


    return 999;

}
#include "sPhenixStyle.C"

int MakePlot_Data_DeltaPhiInnerClusPhi()
{
    std::string input_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run5/AvgVtxXY_nominal/completed"; 
    std::string input_file_name = "Data_AvgVtxXY_00054280_00426";
    std::string target_plot1_name = "h2_8_3_InnerPhi_DCA";

    std::string output_directory = input_file_directory;

    // std::pair<std::string, std::string> axes_label = {"Inner cluster #phi", "#Delta#phi"};
    
    std::string sPH_label = "Internal";
    std::vector<std::tuple<double,double,std::string>> additional_text = {
        // {0.2, 0.9, "The cluster pairs post the VtxZ linking requirement are filled"},

        // {0.22, 0.9, "Au+Au 200 GeV"},
        // {0.16, 0.96, "Generator: HIJING"}
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

    TFile * file_in = TFile::Open(Form("%s/%s.root", input_file_directory.c_str(), input_file_name.c_str()));
    TH2D * h2D_plot1 = (TH2D*)file_in->Get(target_plot1_name.c_str());

    // h2D_plot1 -> GetXaxis() -> SetTitle(axes_label.first.c_str());
    // h2D_plot1 -> GetYaxis() -> SetTitle(axes_label.second.c_str());
    h2D_plot1 -> GetXaxis() -> SetNdivisions(505);
    

    c1 -> cd();
    h2D_plot1 -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str()));
    
    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    c1 -> Print(Form("%s/%s_%s.pdf", output_directory.c_str(),input_file_name.c_str(),target_plot1_name.c_str()));
    c1 -> Clear();

    return 0;


}
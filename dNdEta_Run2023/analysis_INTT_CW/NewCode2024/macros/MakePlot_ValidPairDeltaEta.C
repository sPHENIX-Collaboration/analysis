#include "sPhenixStyle.C"
#include "../Constants.cpp"

int MakePlot_ValidPairDeltaEta()
{

    std::string input_file_directory = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/TrackHist/baseline/completed"); 
    std::string input_file_name = Form("Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize40_ColMulMask_00054280_merged_merged_001.root");
    std::string target_plot_name = "h1D_PairDeltaEta_inclusive";
    std::string output_directory = input_file_directory;
    
    double y_max = 1.5;

    std::string sPH_label = "Internal";
    
    std::pair<std::string, std::string> axes_label = {"Cluster Pair #Delta#eta", ""};
    std::vector<std::tuple<double,double,std::string>> additional_text = {
        // {0.2, 0.9, "The cluster pairs post the VtxZ linking requirement are filled"},

        // {0.22, 0.9, "Au+Au 200 GeV"},
        // {0.22, 0.86, "HIJING"},
        // {0.22, 0.82, "Centrality 0-70%"},
        // {0.22, 0.78, "|INTT vtxZ| #leq 10 cm"}

        {0.22, 0.90, Form("%s", "Cluster pairs able to link to vertex Z are binned")}
        // {0.22, 0.86, "#eta: [-0.1 - 0.1]"},
        // {0.22, 0.82, Form("Centrality interval: [%s]%%", Constants::centrality_text[Mbin].c_str())},
        // {0.22, 0.78, "|INTT vtxZ| #leq 10 cm"}
    };

    bool isSetLogY = false;
    bool isSetLogZ = false;
    double y_min = 0;
    bool set_X_505 = true;

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

    TFile * file_in = TFile::Open(Form("%s/%s", input_file_directory.c_str(), input_file_name.c_str()));
    if (!file_in){
        std::cout<<"Error: cannot open file: "<<input_file_name<<std::endl;
        return 1;
    }

    TH1D * h1 = (TH1D*)file_in->Get(target_plot_name.c_str());

    if (axes_label.first.size() > 0){
        h1 -> GetXaxis() -> SetTitle(axes_label.first.c_str());
    }
    if (axes_label.second.size() > 0){
        h1 -> GetYaxis() -> SetTitle(axes_label.second.c_str());
    }

    c1 -> cd();
    if (isSetLogY) c1 -> SetLogy();
    if (set_X_505) {h1 -> GetXaxis() -> SetNdivisions(505);}
    h1 -> SetMaximum(h1 -> GetBinContent(h1 -> GetMaximumBin()) * y_max);
    h1 -> Draw("hist");
    h1 -> SetStats(0);

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str())); 

    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), target_plot_name.c_str()));
    c1 -> Clear();

    return 0;
}
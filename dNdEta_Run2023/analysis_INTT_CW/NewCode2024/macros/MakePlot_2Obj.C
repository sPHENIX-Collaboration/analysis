#include "sPhenixStyle.C"

int MakePlot_2Obj()
{
    std::string input_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/TrackHist/completed/dNdEta/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed"; 
    std::string input_file_name = "MC_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00002_dNdEta.root";
    std::string target_plot_name_1 = "h1D_TruedNdEta";
    std::string target_plot_name_2 = "h1D_RotatedBkg_RecoTrackletEtaPerEvt";
    std::string output_directory = input_file_directory;
    std::string output_file_name = "h1D_TruedNdEta";
    std::pair<std::string, std::string> axes_label = {"Tracklet #eta", "Average Multiplicity Per Event  (/0.2)"};
    std::string sPH_label = "Simulation";
    std::vector<std::tuple<double,double,std::string>> additional_text = {
        // {0.2, 0.9, "The cluster pairs post the VtxZ linking requirement are filled"},

        {0.22, 0.9, "Au+Au 200 GeV"},
        // {0.22, 0.86, "HIJING"},
        {0.22, 0.86, "Centrality 0-70%"},
        {0.22, 0.82, "|INTT vtxZ| #leq 10 cm"}

        // {0.22, 0.90, "Inner clusters rotated by #pi in #phi angle"},
        // {0.22, 0.86 + 0.04, "#eta: [0.5 - 0.7]"},
        // {0.22, 0.82 + 0.04, "Centrality 0-70%"},
        // {0.22, 0.78 + 0.04, "|INTT vtxZ| #leq 10 cm"}

    };

    // std::vector<std::tuple<int, std::string,std::string>> legend_text = {
    //     {4, "Nominal", "l"},
    //     {2, "Rotated", "l"},
    //     {3, "Subtracted", "l"}
    // };

    std::vector<std::tuple<int, std::string, std::string>> legend_text = {
        {2, "HIJING generator level", "l"},
        {4, "Reco. Tracklets", "pl"}
    };

    bool isSetLogY = false;
    bool isSetLogZ = false;
    double y_min = 0;
    double y_max = 350;
    bool set_X_505 = true;

    system(Form("mkdir -p %s", output_directory.c_str()));
    
    SetsPhenixStyle();

    TLegend * leg = new TLegend(0.52,0.81,0.72,0.9);
    // leg -> SetNColumns(2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.025);
    leg -> SetMargin(0.2);

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

    TH1D * h1D_in_1 = (TH1D*) file_in -> Get(Form("%s", target_plot_name_1.c_str()));
    if (!h1D_in_1){
        std::cout<<"Error: cannot find histogram: "<<target_plot_name_1<<std::endl;
        return 1;
    }

    h1D_in_1 -> SetMarkerStyle(20);
    h1D_in_1 -> SetMarkerSize(0.7);
    h1D_in_1  -> SetMarkerColor(std::get<0>(legend_text[0]));
    h1D_in_1    -> SetLineColor(std::get<0>(legend_text[0]));
    leg -> AddEntry(h1D_in_1, std::get<1>(legend_text[0]).c_str(), std::get<2>(legend_text[0]).c_str());

    TH1D * h1D_in_2 = (TH1D*) file_in -> Get(Form("%s", target_plot_name_2.c_str()));
    if (!h1D_in_2){
        std::cout<<"Error: cannot find histogram: "<<target_plot_name_2<<std::endl;
        return 1;
    }

    h1D_in_2 -> SetMarkerStyle(20);
    h1D_in_2 -> SetMarkerSize(0.7);
    h1D_in_2  -> SetMarkerColor(std::get<0>(legend_text[1]));
    h1D_in_2    -> SetLineColor(std::get<0>(legend_text[1]));
    leg -> AddEntry(h1D_in_2, std::get<1>(legend_text[1]).c_str(), std::get<2>(legend_text[1]).c_str());


    if (isSetLogY){ // note : logy
        h1D_in_1 -> SetMaximum(y_max);
        c1 -> SetLogy(true);
    }
    else if (!isSetLogY) // note : linear y
    {
        h1D_in_1 -> SetMinimum(y_min);
        h1D_in_1 -> SetMaximum(y_max);
    }



    if (axes_label.first.size() > 0){
        h1D_in_1 -> GetXaxis() -> SetTitle(axes_label.first.c_str());
    }
    if (axes_label.second.size() > 0){
        h1D_in_1 -> GetYaxis() -> SetTitle(axes_label.second.c_str());
    }

    if (set_X_505) {h1D_in_1 -> GetXaxis() -> SetNdivisions(505);}


    // todo : the drawing 
    c1 -> cd();

    gStyle->SetPaintTextFormat("1.3f");

    h1D_in_1 -> Draw("hist");
    h1D_in_2 -> Draw("ep same");

    // hist_in -> SetMarkerSize(0.5);
    // hist_in -> Draw("hist TEXT90");

    // hstack1D_in -> SetMarkerSize(0.8);
    // hstack1D_in -> Draw("hist TEXT90 E");
    // temp_h1D -> Draw("hist");
    // hstack1D_in -> Draw("hist same");



    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str())); 

    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    leg -> Draw("same");

    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), output_file_name.c_str()));
    c1 -> Clear();

    return 0;
}
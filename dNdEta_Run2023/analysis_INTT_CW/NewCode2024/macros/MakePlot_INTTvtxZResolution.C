#include "sPhenixStyle.C"

int MakePlot_INTTvtxZResolution()
{
    bool isTH1D = true;
    std::string input_file_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_ananew_20250131/Run4/EvtVtxZ/completed/VtxZDist/completed"; 
    std::string input_file_name = "MC_vtxZDist_VtxZQA_merged.root";
    std::string target_plot_name = "h1D_INTTvtxZ_resolution_HighNClus";
    std::string output_directory = input_file_directory;
    std::string output_file_name = "h1D_INTTvtxZ_resolution_HighNClus";
    // std::pair<std::string, std::string> axes_label = {"INTT LayerID", "INTT LadderPhiId"};
    std::string Zaxis_title = "";
    std::pair<std::string, std::string> axes_label = {"", ""};
    // std::string sPH_label = "Internal";
    std::string sPH_label = "Simulation";
    std::vector<std::tuple<double,double,std::string>> additional_text = {
        // {0.2, 0.9, "The cluster pairs post the VtxZ linking requirement are filled"},

        {0.22, 0.9, "Au+Au 200 GeV"},
        {0.22, 0.86, "HIJING"},
        {0.22, 0.82, "NClus > 500"},
        // {0.22, 0.82, "Centrality 0-50%"},
        // {0.22, 0.78, "|INTT vtxZ| #leq 30 cm"}
    };

    bool isSetLogY = false;
    bool isSetLogZ = false;
    double y_min = 0;
    double y_max = 50000;
    bool set_X_505 = true;
    bool DoGausFit = true;

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

    TF1 * gaus_fit = new TF1("gaus_fit", "gaus", -30, 30);
    gaus_fit -> SetLineColor(kRed);

    TFile * file_in = TFile::Open(Form("%s/%s", input_file_directory.c_str(), input_file_name.c_str()));
    if (!file_in){
        std::cout<<"Error: cannot open file: "<<input_file_name<<std::endl;
        return 1;
    }

    TH1D * h1D_in = nullptr;
    TH2D * h2D_in = nullptr;

    if (isTH1D){
        h1D_in = (TH1D*)file_in->Get(Form("%s",target_plot_name.c_str()));
        if (!h1D_in){
            std::cout<<"Error: cannot find histogram: "<<target_plot_name<<std::endl;
            return 1;
        }
    }
    else{
        h2D_in = (TH2D*)file_in->Get(Form("%s",target_plot_name.c_str()));
        if (!h2D_in){
            std::cout<<"Error: cannot find histogram: "<<target_plot_name<<std::endl;
            return 1;
        }
    }

    // auto hist_in = (isTH1D) ? (TH1D*)file_in->Get(Form("%s",target_plot_name.c_str())) : (TH2D*)file_in->Get(Form("%s",target_plot_name.c_str()));
    // if (!hist_in){
    //     std::cout<<"Error: cannot find histogram: "<<target_plot_name<<std::endl;
    //     return 1;
    // }

    if (isTH1D){
        
        if (isSetLogY){ // note : TH1D, logy
            h1D_in -> SetMaximum(y_max);
            c1 -> SetLogy(true);
        }
        else if (!isSetLogY) // note : TH1D, linear y
        {
            h1D_in -> SetMinimum(y_min);
            h1D_in -> SetMaximum(y_max);
        }

        if (axes_label.first.size() > 0){
            h1D_in -> GetXaxis() -> SetTitle(axes_label.first.c_str());
        }
        if (axes_label.second.size() > 0){
            h1D_in -> GetYaxis() -> SetTitle(axes_label.second.c_str());
        }

        if (set_X_505) {h1D_in -> GetXaxis() -> SetNdivisions(505);}

        if (DoGausFit){
            leg -> AddEntry(h1D_in, "Reco. VtxZ", "f");
            leg -> AddEntry(gaus_fit, "Gaus. Fit", "l");

            gaus_fit -> SetParameters(
                h1D_in -> GetMaximum(),
                h1D_in -> GetMean(),
                h1D_in -> GetRMS()
            );
            double fit_range_l = h1D_in -> GetMean() - 1.3 * h1D_in -> GetRMS();
            double fit_range_r = h1D_in -> GetMean() + 1.3 * h1D_in -> GetRMS();
            h1D_in -> Fit("gaus_fit", "R", "", fit_range_l, fit_range_r);
            gaus_fit -> SetRange(fit_range_l, fit_range_r);
        }

    }
    else if (!isTH1D) 
    {
        if (isSetLogZ){ // note : TH2D, logz
            c1 -> SetLogz(true);
        }

        if (axes_label.first.size() > 0){
            h2D_in -> GetXaxis() -> SetTitle(axes_label.first.c_str());
        }
        if (axes_label.second.size() > 0){
            h2D_in -> GetYaxis() -> SetTitle(axes_label.second.c_str());
        }
        if (Zaxis_title.size() > 0){
            h2D_in -> GetZaxis() -> SetTitle(Zaxis_title.c_str());
        }

        if (set_X_505) {h2D_in -> GetXaxis() -> SetNdivisions(505);}
    }

    // todo : the drawing 
    c1 -> cd();

    gStyle->SetPaintTextFormat("1.3f");

    // hist_in -> SetMarkerSize(0.5);
    // hist_in -> Draw("hist TEXT90");

    // todo : the drawing
    if (isTH1D){
        h1D_in -> SetMarkerSize(0.8);
        // h1D_in -> Draw("hist TEXT90 E");
        h1D_in -> Draw("hist");

        if (DoGausFit){
            gaus_fit -> Draw("same");
            leg -> Draw("same");
        }
    }
    else {
        // h2D_in -> SetMarkerSize(0.5);
        // h2D_in -> Draw("colz TEXT90");

        h2D_in -> Draw("colz0");
    }


    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str())); 

    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    if (DoGausFit)
    {
        draw_text -> DrawLatex(
            std::get<0>(additional_text[0]),
            std::get<1>(additional_text.back()) - 0.08,
            Form("Mean_{Gaus. Fit} = %.3f", gaus_fit -> GetParameter(1))
        );

        draw_text -> DrawLatex(
            std::get<0>(additional_text[0]),
            std::get<1>(additional_text.back()) - 0.12,
            Form("#sigma_{Gaus. Fit} = %.3f", gaus_fit -> GetParameter(2))
        );
    }

    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), output_file_name.c_str()));
    c1 -> Clear();

    return 0;
}
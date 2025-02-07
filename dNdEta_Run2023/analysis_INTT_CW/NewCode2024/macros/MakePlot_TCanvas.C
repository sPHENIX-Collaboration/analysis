#include "sPhenixStyle.C"

int MakePlot_TCanvas()
{
    std::string input_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/TrackHist/completed/dNdEta/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70"; 
    std::string input_file_name = "DataMc2Comp.root";
    std::string target_plot_name = "h1D_RotatedBkg_RecoTrackletEtaPerEvt";
    std::string output_directory = input_file_directory;
    std::string output_file_name = "h1D_RotatedBkg_RecoTrackletEtaPerEvt";
    std::pair<std::string, std::string> axes_label = {"Tracklet #eta", "Avg. Reco. Tracklets per event (/0.2)"};
    std::string sPH_label = "Internal";
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
        {1, "Data", "pl"},
        {2, "HIJING", "l"}
    };

    bool isSetLogY = false;
    bool isSetLogZ = false;
    double y_min = 0;
    double y_max = 350;
    bool set_X_505 = true;

    system(Form("mkdir -p %s", output_directory.c_str()));
    
    SetsPhenixStyle();

    TLegend * leg = new TLegend(0.22,0.68,0.42,0.77);
    // leg -> SetNColumns(2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.025);
    leg -> SetMargin(0.2);

    // TCanvas * c1 = new TCanvas("c1", "c1", 950, 800);

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

    gStyle->SetCanvasDefH(950);
    gStyle->SetCanvasDefH(800);

    TCanvas * c1 = (TCanvas*)file_in->Get(Form("%s",target_plot_name.c_str()));    
    if (!c1){
        std::cout<<"Error: cannot find canvas: "<<target_plot_name<<std::endl;
        return 1;
    }

    // c1->SetWindowSize(500, 800);

    std::cout<<"c1->GetWindowHeight(): "<<c1->GetWindowHeight()<<", c1->GetWindowWidth(): "<<c1->GetWindowWidth()<<std::endl;

    if (isSetLogY){ // note : logy
        // temp_h1D -> SetMaximum(y_max);
        c1 -> SetLogy(true);
    }
    else if (!isSetLogY) // note : linear y
    {
        // temp_h1D -> SetMinimum(y_min);
        // temp_h1D -> SetMaximum(y_max);
    }

    TList* primitives = c1->GetListOfPrimitives(); // Get the list of objects (primitives) on the canvas
    if (!primitives) {
        std::cerr << "No primitives found in TCanvas!" << std::endl;
        file_in->Close();
        return 666;
    }

    int legend_index = 0;
    // Loop through the primitives and modify the TH1Ds
    for (auto* obj : *primitives) {
        TH1D* hist = dynamic_cast<TH1D*>(obj); // Check if the primitive is a TH1D
        if (hist) {
            
            int color = std::get<0>(legend_text[legend_index]);
            std::string text = std::get<1>(legend_text[legend_index]);
            std::string draw_type = std::get<2>(legend_text[legend_index]);

            hist -> SetLineColor( color );
            std::cout<<hist->GetName()<<", set line color : "<<hist->GetLineColor()<<std::endl;
            leg -> AddEntry(hist, text.c_str(), draw_type.c_str());

            hist -> SetMaximum(y_max);
            hist -> SetMinimum(y_min);

            hist -> SetMarkerSize(0.7);

            if (axes_label.first.size() > 0){
                hist -> GetXaxis() -> SetTitle(axes_label.first.c_str());
            }
            if (axes_label.second.size() > 0){
                hist -> GetYaxis() -> SetTitle(axes_label.second.c_str());
            }
        }

        legend_index += 1;

    }

    // if (axes_label.first.size() > 0){
    //     temp_h1D -> GetXaxis() -> SetTitle(axes_label.first.c_str());
    // }
    // if (axes_label.second.size() > 0){
    //     temp_h1D -> GetYaxis() -> SetTitle(axes_label.second.c_str());
    // }

    // if (set_X_505) {temp_h1D -> GetXaxis() -> SetNdivisions(505);}


    // todo : the drawing 
    c1 -> cd();

    gStyle->SetPaintTextFormat("1.3f");

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
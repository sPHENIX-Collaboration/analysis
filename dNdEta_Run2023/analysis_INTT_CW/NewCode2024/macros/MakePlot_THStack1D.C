#include "sPhenixStyle.C"
#include "../Constants.cpp"

int MakePlot_THStack1D()
{

    int Mbin = 0;
    bool isPhiRotated = true;

    std::string input_file_directory = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed/vtxZ_-10_10cm_MBin%d/Folder_BaseLine/Run_0/completed", Mbin); 
    std::string input_file_name = Form("Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ10_Mbin%d_00054280_00000_DeltaPhi.root", Mbin);
    std::string output_directory = input_file_directory;
    
    std::string target_plot_name = (isPhiRotated) ? "hstack1D_DeltaPhi_Eta13_rotated" : "hstack1D_DeltaPhi_Eta13";

    double y_max = 2 * pow(10,6);

    std::string sPH_label = "Internal";
    
    std::pair<std::string, std::string> axes_label = {"", "Entries (/0.001)"};
    string rotate_string = (isPhiRotated) ? "Inner clusters rotated by #pi in #phi angle" : "";
    std::vector<std::tuple<double,double,std::string>> additional_text = {
        // {0.2, 0.9, "The cluster pairs post the VtxZ linking requirement are filled"},

        // {0.22, 0.9, "Au+Au 200 GeV"},
        // {0.22, 0.86, "HIJING"},
        // {0.22, 0.82, "Centrality 0-70%"},
        // {0.22, 0.78, "|INTT vtxZ| #leq 10 cm"}

        {0.22, 0.90, Form("%s", rotate_string.c_str())},
        {0.22, 0.86, "#eta: [-0.1 - 0.1]"},
        {0.22, 0.82, Form("Centrality interval: [%s]%%", Constants::centrality_text[Mbin].c_str())},
        {0.22, 0.78, "|INTT vtxZ| #leq 10 cm"}

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

    THStack * hstack1D_in = nullptr;
    hstack1D_in = (THStack*)file_in->Get(Form("%s",target_plot_name.c_str()));
    if (!hstack1D_in){
        std::cout<<"Error: cannot find histogram: "<<target_plot_name<<std::endl;
        return 1;
    }

    TList* histList = hstack1D_in->GetHists(); // Retrieve the list of histograms in the stack
    if (histList) {
        for (auto* obj : *histList) {
            TH1D* hist = dynamic_cast<TH1D*>(obj); // Cast to TH1D
            if (hist) {
                hist->SetLineWidth(1); // Set line width to 0
            }
        }
    }

    auto temp_h1D = (TH1D*)((TH1D*)hstack1D_in -> GetStack()->Last())->Clone("temp_h1D");
    temp_h1D -> Reset("ICESM");

    if (isSetLogY){ // note : logy
        temp_h1D -> SetMaximum(y_max);
        c1 -> SetLogy(true);
    }
    else if (!isSetLogY) // note : linear y
    {
        temp_h1D -> SetMinimum(y_min);
        temp_h1D -> SetMaximum(y_max);
    }

    if (axes_label.first.size() > 0){
        temp_h1D -> GetXaxis() -> SetTitle(axes_label.first.c_str());
    }
    if (axes_label.second.size() > 0){
        temp_h1D -> GetYaxis() -> SetTitle(axes_label.second.c_str());
    }

    if (set_X_505) {temp_h1D -> GetXaxis() -> SetNdivisions(505);}


    // todo : the drawing 
    c1 -> cd();

    gStyle->SetPaintTextFormat("1.3f");

    // hist_in -> SetMarkerSize(0.5);
    // hist_in -> Draw("hist TEXT90");

    // hstack1D_in -> SetMarkerSize(0.8);
    // hstack1D_in -> Draw("hist TEXT90 E");
    temp_h1D -> Draw("hist");
    hstack1D_in -> Draw("same");

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
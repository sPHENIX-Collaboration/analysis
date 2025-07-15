#include "sPhenixStyle.C"

const std::vector<std::string> color_code = {
    "#000000",
    "#9e0142",
    "#5e4fa2",
    "#66c2a5",
    "#fdae61",
    "#0076A7",
    "#fee08b",

    "#d53e4f",
    "#abdda4",
    "#f46d43",
    "#e6f598",
    "#00A1FF",

    "#1b9e77",  // Teal Green  
    "#d95f02",  // Deep Orange  
    "#7570b3",  // Muted Purple  
    "#e7298a",  // Vivid Pink  
    "#66a61e",  // Olive Green  
    "#a6761d",  // Brown  
    "#666666",  // Dark Gray  
    "#f781bf",  // Light Pink  
    "#0173b2",  // Rich Blue  
    "#ff7f00"   // Bright Orange  

};

double GetNonZeroMinimalBin(TH1D * hist_in)
{
    double min = 10000000;
    for (int i = 1; i <= hist_in->GetNbinsX(); i++)
    {
        if (hist_in->GetBinContent(i) > 0 && hist_in->GetBinContent(i) < min)
        {
            min = hist_in->GetBinContent(i);
        }
    }

    return min;
}


TCanvas * make_comparison(
    std::vector<std::pair<std::string, TH1D*>> data_hist_vec_in,  // note : description, hist // note : postQA
    std::vector<std::pair<std::string, TH1D*>> MC_hist_vec_in,    // note : description, hist // note : noQA
    std::string output_directory_in, 
    std::string output_filename_in, 
    std::string sub_label_text_in, // note : "Data", "MC", "Comp"
    bool DoNormalize = false,
    bool set_log_y = false, 
    bool isData_more = false,
    double MainPlot_y_low = 0, 
    double MainPlot_y_top = -99, 
    double ratio_y_low = 0,
    double ratio_y_top = 2
)
{
    double logy_max_factor = 1000;
    double lineary_max_factor = 1.6;
    std::string sPH_labeling = "Internal";

    std::cout<<"In make_comparison, working on "<<output_filename_in<<std::endl;

    for (auto &pair : data_hist_vec_in)
    {
        if (pair.second == nullptr)
        {
            std::cout<<"Error: "<<pair.first<<" has nullptr"<<std::endl;
            exit(1);
        }
    }

    for (auto &pair : MC_hist_vec_in)
    {
        if (pair.second == nullptr)
        {
            std::cout<<"Error: "<<pair.first<<" has nullptr"<<std::endl;
            exit(1);
        }
    }

    std::vector<int> data_hist_nbinsX; data_hist_nbinsX.clear();
    std::vector<int> MC_hist_nbinsX; MC_hist_nbinsX.clear();

    int color_index = 0;

    for (auto &pair : data_hist_vec_in)
    {

        std::cout<<"Data: "<<pair.first<<", "<<pair.second->GetName()<<", "<<pair.second->GetNbinsX()<<std::endl;

        pair.second -> SetStats(0);

        data_hist_nbinsX.push_back(pair.second->GetNbinsX());
        
        if (DoNormalize) {
            pair.second->Scale(1.0/pair.second->Integral());
            pair.second->GetYaxis()->SetTitle(
                Form("%s (A.U.)", (pair.second->GetYaxis()->GetTitle()))
            );
        }

        pair.second->SetLineColor(TColor::GetColor(color_code[color_index].c_str()));
        pair.second->SetMarkerColor(TColor::GetColor(color_code[color_index].c_str()));
        pair.second->SetFillColorAlpha(2,0);
        color_index += 1;
    }

    for (auto &pair : MC_hist_vec_in)
    {
        std::cout<<"MC: "<<pair.first<<", "<<pair.second->GetName()<<", "<<pair.second->GetNbinsX()<<std::endl;

        MC_hist_nbinsX.push_back(pair.second->GetNbinsX());

        pair.second -> SetStats(0);

        if (DoNormalize) {
            pair.second->Scale(1.0/pair.second->Integral());
            pair.second->GetYaxis()->SetTitle(
                Form("%s (A.U.)", (pair.second->GetYaxis()->GetTitle()))
            );
        }

        pair.second->SetLineColor(TColor::GetColor(color_code[color_index].c_str()));
        pair.second->SetMarkerColor(TColor::GetColor(color_code[color_index].c_str()));
        pair.second->SetFillColorAlpha(2,0);
        color_index += 1;
    }

    for (int &data_nbin : data_hist_nbinsX){
        for (int &MC_nbin : MC_hist_nbinsX){
            if (data_nbin != MC_nbin){
                std::cout<<"Error: Different binning for data and MC, data: "<<data_nbin<<", MC: "<<MC_nbin<<std::endl;
                exit(1);
            }
        }
    }

    bool bins_are_same = true;
    for (int i = 1; i <= data_hist_vec_in[0].second->GetNbinsX(); i++)
    {
        if (data_hist_vec_in[0].second->GetBinCenter(i) != MC_hist_vec_in[0].second->GetBinCenter(i))
        {
            bins_are_same = false;
            break;
        }
    }

    if (MainPlot_y_top == -99)
    {
        for (auto &pair : data_hist_vec_in)
        {
            MainPlot_y_top = (pair.second->GetBinContent(pair.second->GetMaximumBin()) > MainPlot_y_top) ? pair.second->GetBinContent(pair.second->GetMaximumBin()) : MainPlot_y_top;
        }

        for (auto &pair : MC_hist_vec_in)
        {
            MainPlot_y_top = (pair.second->GetBinContent(pair.second->GetMaximumBin()) > MainPlot_y_top) ? pair.second->GetBinContent(pair.second->GetMaximumBin()) : MainPlot_y_top;
        }


        if (set_log_y){            
            MainPlot_y_top = MainPlot_y_top * logy_max_factor;
        }
        else{
            MainPlot_y_top = MainPlot_y_top * lineary_max_factor;
        }
    }

    if (MainPlot_y_low == 0 && set_log_y)
    {
        MainPlot_y_low = 1000000;

        for (auto &pair : data_hist_vec_in)
        {
            double min = GetNonZeroMinimalBin(pair.second);

            MainPlot_y_low = (min < MainPlot_y_low) ? min : MainPlot_y_low;
        }

        for (auto &pair : MC_hist_vec_in)
        {
            double min = GetNonZeroMinimalBin(pair.second);

            MainPlot_y_low = (min < MainPlot_y_low) ? min : MainPlot_y_low;
        }
    }

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLegend * All_leg = new TLegend(0.4, 0.82, 0.8, 0.88);
    All_leg -> SetBorderSize(0);
    All_leg -> SetTextSize(0.025);

    TLegend * All_leg_long = new TLegend(0.7, 0.5, 0.8, 0.88);
    All_leg_long -> SetBorderSize(0);

    TLatex * sub_ltx = new TLatex();
    sub_ltx->SetNDC();
    sub_ltx->SetTextSize(0.02);
    // ltx->SetTextAlign(31);

    TH1D * h1D_pad1 = (TH1D*)data_hist_vec_in.front().second->Clone("h1D_pad1");
    TH1D * h1D_pad2 = (TH1D*)data_hist_vec_in.front().second->Clone("h1D_pad2");
    h1D_pad1 -> Reset("ICESM");
    h1D_pad2 -> Reset("ICESM");
    h1D_pad1 -> SetLineColorAlpha(1,0.);
    h1D_pad2 -> SetLineColorAlpha(1,0.);
    h1D_pad1 -> SetFillColorAlpha(1,0.);
    h1D_pad2 -> SetFillColorAlpha(1,0.);
    h1D_pad1 -> SetMarkerColorAlpha(1,0.);
    h1D_pad2 -> SetMarkerColorAlpha(1,0.);


    SetsPhenixStyle();

    TCanvas * c1 = new TCanvas("", "c1", 950, 800);

    TPad * pad2 = new TPad("", "pad2", 0.0, 0.0, 1.0, 0.29);
    pad2 -> SetBottomMargin(0.3);
    pad2->Draw();
    
    TPad * pad1 = new TPad("", "pad1", 0.0, 0.29, 1.0, 1.0);
    pad1 -> SetTopMargin(0.065);
    pad1 -> SetBottomMargin(0.02);
    pad1->Draw();

    int total_hist = data_hist_vec_in.size() + MC_hist_vec_in.size();

    // todo: set the size fo the legend
    double leg_x1 = (total_hist <= 4) ? 0.45 : 0.65;
    double leg_y1 = (total_hist <= 4) ? 0.76 : 0.4;
    double leg_x2 = (total_hist <= 4) ? 0.8 : 0.8;
    double leg_y2 = (total_hist <= 4) ? 0.88 : 0.88;

    TLegend * leg = new TLegend(leg_x1, leg_y1, leg_x2, leg_y2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.025);

    h1D_pad1->GetXaxis()->SetLabelOffset(999);
    h1D_pad1->GetXaxis()->SetLabelSize(0);
    h1D_pad1->GetXaxis()->SetTitleOffset(999);
    h1D_pad1->GetYaxis()->SetLabelSize(0.045);
    h1D_pad1->GetYaxis()->SetTitleOffset(0.8);
    h1D_pad1->GetYaxis()->SetTitleSize(0.07);    
    h1D_pad1->GetXaxis()->SetNdivisions();

    for (auto &pair : MC_hist_vec_in){
        leg -> AddEntry(pair.second, Form("%s, Mean: %.2f, StdDev: %.2f", pair.first.c_str(), pair.second->GetMean(), pair.second->GetStdDev()), "f");
    }
    
    for (auto &pair : data_hist_vec_in){
        // pair.second->GetXaxis()->SetNdivisions();
        leg -> AddEntry(pair.second, Form("%s, Mean: %.2f, StdDev: %.2f", pair.first.c_str(), pair.second->GetMean(), pair.second->GetStdDev()), "lep");
    }

    sub_ltx->DrawLatex(0.45, 0.78, Form("Mean diff (A - B)/B: %.3f", (data_hist_vec_in[0].second->GetMean() - MC_hist_vec_in[0].second->GetMean())/MC_hist_vec_in[0].second->GetMean()));
    sub_ltx->DrawLatex(0.45, 0.74, Form("StdDev diff (A - B)/B: %.3f", (data_hist_vec_in[0].second->GetStdDev() - MC_hist_vec_in[0].second->GetStdDev())/MC_hist_vec_in[0].second->GetStdDev()));
    
    pad1->cd();

    std::cout<<"For "<<output_filename_in<<", MainPlot_y_low: "<<MainPlot_y_low<<", MainPlot_y_top: "<<MainPlot_y_top<<std::endl;

    h1D_pad1 -> SetMinimum(MainPlot_y_low);
    h1D_pad1 -> SetMaximum(MainPlot_y_top);
    h1D_pad1 -> Draw("hist");
    
    for (int i = 0; i < MC_hist_vec_in.size(); i++){
        MC_hist_vec_in[i].second -> Draw("hist same");
    }

    for (int i = 0; i < data_hist_vec_in.size(); i++){
        data_hist_vec_in[i].second -> Draw("ep same");
    }

    leg -> Draw("same");

    pad1->SetLogy(set_log_y);

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));

    // if (labelling_vec_map.find(sub_label_text_in) != labelling_vec_map.end())
    // {
    //     for (auto &tuple : labelling_vec_map[sub_label_text_in])
    //     {
    //         double x1 = std::get<0>(tuple);
    //         double y1 = std::get<1>(tuple);
    //         std::string text = std::get<2>(tuple);

    //         sub_ltx->DrawLatex(x1, y1, text.c_str());
    //     }
    // }

    TLine * line = new TLine();
    line -> SetLineStyle(2);
    line -> SetLineWidth(2);
    line -> SetLineColor(28);

    pad2->cd();

    std::vector<TH1D*> ratio_hist_vec_in; ratio_hist_vec_in.clear();

    std::vector<std::pair<std::string, TH1D*>> hist_vec_in = (isData_more) ? data_hist_vec_in : MC_hist_vec_in;

    for (int i = 0; i < hist_vec_in.size(); i++){
        
        std::pair<std::string, TH1D*> pair = hist_vec_in[i];

        // todo: can only do the ratio w.r.t the first data hist
        if (isData_more){
            ratio_hist_vec_in.push_back( // note : hist_vec_in: data
                (TH1D*) MC_hist_vec_in[0].second->Clone(Form("ratio_hist_%s", pair.first.c_str()))
            );

            ratio_hist_vec_in.back() -> Sumw2(true);
            if (bins_are_same) {ratio_hist_vec_in.back() -> Divide(pair.second, ratio_hist_vec_in.back());}

        }
        else { // note : hist_vec_in: MC
            ratio_hist_vec_in.push_back(
                (TH1D*) data_hist_vec_in[0].second->Clone(Form("ratio_hist_%s", pair.first.c_str()))
            );

            ratio_hist_vec_in.back() -> Sumw2(true);
            if (bins_are_same) {ratio_hist_vec_in.back() -> Divide(pair.second);}
        }


        // if (ratio_hist_vec_in.back()->GetXaxis()->GetLabelOffset() > 10)
        // {
        //     ratio_hist_vec_in.back()->GetXaxis()->SetLabelOffset(0.01);
        // }

        ratio_hist_vec_in.back()->SetMarkerStyle(20);
        ratio_hist_vec_in.back()->SetMarkerSize(0.8);
        ratio_hist_vec_in.back()->SetMarkerColor(pair.second->GetLineColor());
        ratio_hist_vec_in.back()->SetLineColor(pair.second->GetLineColor());

        if (i == 0){
            h1D_pad2->GetXaxis()->SetLabelSize(0.11);
            h1D_pad2->GetXaxis()->SetTitleOffset(0.8);
            h1D_pad2->GetXaxis()->SetTitleSize(0.14);
            h1D_pad2->GetXaxis()->SetNdivisions();

            h1D_pad2->GetYaxis()->SetLabelSize(0.11);
            h1D_pad2->GetYaxis()->SetTitleOffset(0.4);
            h1D_pad2->GetYaxis()->SetTitleSize(0.14);
            h1D_pad2->GetYaxis()->SetNdivisions(505);

            h1D_pad2 -> SetMinimum(ratio_y_low);
            h1D_pad2 -> SetMaximum(ratio_y_top);

            h1D_pad2->GetYaxis()->SetTitle("Data/MC");


            h1D_pad2->Draw("ep");
            line -> DrawLine(h1D_pad2->GetXaxis()->GetXmin(), 1, h1D_pad2->GetXaxis()->GetXmax(), 1);

            ratio_hist_vec_in.back()->Draw("ep same");
        }
        else {
            ratio_hist_vec_in.back()->Draw("ep same");
        }
    }


    
    // std::string data_hist_name = data_hist->GetName(); 


    // if (data_hist_name.find("_l") != std::string::npos && data_hist_name.find("_phi") != std::string::npos && data_hist_name.find("_z") != std::string::npos)
    // {
    //     std::cout<<"what? : "<<data_hist_name<<std::endl;

    //     // c1 -> Print(Form("%s/Comp_%s.pdf", (output_directory + "/" + sensor_folder).c_str(), output_filename.c_str()));
    // }
    if (true)
    {
        c1 -> Print(Form("%s/Comp_%s.pdf", output_directory_in.c_str(), output_filename_in.c_str()));
    }

    return c1;
    
    // if (data_hist_name.find("_l") == std::string::npos || data_hist_name.find("_phi") == std::string::npos || data_hist_name.find("_z") == std::string::npos) {return {};}

    // return GetHistYInfo(ratio_hist);

    
    // pad1 -> Clear();
    // pad2 -> Clear();
    // c1 -> Clear();

    // delete pad1;
    // delete pad2;
    // delete c1;
    // delete ratio_hist;

    // return;
}

TH1D * GetHist(std::string directory, std::string file_name, std::string hist_name)
{
    TFile * file = new TFile(Form("%s/%s", directory.c_str(), file_name.c_str()), "READ");
    TH1D * hist = (TH1D*)file->Get(hist_name.c_str());

    return hist;
}

int TwoPlot_ratio()
{
    std::string black_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/old_folder/FinalResult_10cm_Pol2BkgFit/completed/vtxZ_-10_10cm_MBin14/Folder_BaseLine/Run_0/completed";
    std::string black_file_name = "Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ10_Mbin14_00054280_00000_dNdEta.root";
    std::string black_hist_name = "h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC";
    std::string black_str = "Old";

    std::string red_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed/vtxZ_-10_10cm_MBin14/Folder_DeltaPhiCut/Run_0/completed";
    std::string red_file_name = "Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ10_Mbin14_00054280_00000_dNdEta.root";
    std::string red_hist_name = "h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC";
    std::string red_str = "New";

    std::string output_directory = black_directory;
    // std::string output_filename = "comp_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC.root";
    std::string output_filename_pdf = "comp_data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC"; 

    bool DoNormalize = false;
    bool set_log_y = false;

    // TFile * file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");

    TH1D * black_hist = GetHist(black_directory, black_file_name, black_hist_name);
    TH1D * red_hist = GetHist(red_directory, red_file_name, red_hist_name);

    TCanvas * c1 = make_comparison(
        {{black_str, black_hist}},
        {{red_str, red_hist}},

        output_directory, 
        output_filename_pdf, 
        "", // note : "Data", "MC", "Comp"
        DoNormalize, // note : DoNormalize
        set_log_y,  // note : set_log_y
        false, // note : isData_more
        0,  // note : MainPlot_y_low
        -99,  // note : MainPlot_y_top
        0.5, // note : ratio_y_low
        1.5 // note : ratio_y_top
    );

    // file_out -> cd();

    // c1 -> Write();

    // file_out -> Close();

    return 0;    
}
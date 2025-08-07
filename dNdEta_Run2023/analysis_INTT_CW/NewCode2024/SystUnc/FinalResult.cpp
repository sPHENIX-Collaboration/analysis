#include "FinalResult.h"

FinalResult::FinalResult(
    int runnumber_in,
    int Mbin_in,
    std::string StandardData_directory_in,
    std::string StandardData_file_name_in,
    std::string StandardMC_directory_in,
    std::string StandardMC_file_name_in,
    std::string sPH_label_in,
    std::string Output_directory_in
):
    runnumber(runnumber_in),
    Mbin(Mbin_in),
    StandardData_directory(StandardData_directory_in),
    StandardData_file_name(StandardData_file_name_in),
    StandardMC_directory(StandardMC_directory_in),
    StandardMC_file_name(StandardMC_file_name_in),
    sPH_label(sPH_label_in),
    Output_directory(Output_directory_in)
{
    Output_directory = (Output_directory == "Not_given") ? StandardData_directory : Output_directory;

    PrepareOutputFolderName();
    system(Form("if [ ! -d %s/%s/completed ]; then mkdir -p %s/%s/completed; fi;", Output_directory.c_str(), output_folder_name.c_str(), Output_directory.c_str(), output_folder_name.c_str()));
    final_output_directory = Form("%s/%s", Output_directory.c_str(), output_folder_name.c_str());

    file_in_data_standard = TFile::Open(Form("%s/%s", StandardData_directory.c_str(), StandardData_file_name.c_str()));
    h1D_data_standard = (TH1D*) file_in_data_standard -> Get(StandardData_h1D_name.c_str());

    file_in_MC_standard = TFile::Open(Form("%s/%s", StandardMC_directory.c_str(), StandardMC_file_name.c_str()));
    h1D_truth_standard = (TH1D*) file_in_MC_standard -> Get(StandardTruth_h1D_name.c_str());

    h1D_MC_closure_standard = (TH1D*) file_in_MC_standard -> Get(StandardData_h1D_name.c_str());

    if (
        h1D_data_standard == nullptr ||
        h1D_truth_standard == nullptr || 

        h1D_data_standard -> GetNbinsX()             != h1D_truth_standard -> GetNbinsX() ||
        h1D_data_standard -> GetXaxis() -> GetXmin() != h1D_truth_standard -> GetXaxis() -> GetXmin() ||
        h1D_data_standard -> GetXaxis() -> GetXmax() != h1D_truth_standard -> GetXaxis() -> GetXmax()
    ){
        std::cout << "Error : the standard data or MC is not found" << std::endl;
        exit(1);
    }

    h1D_RunSegmentError_vec.clear();
    h1D_ClusAdcError_vec.clear();
    h1D_GeoOffsetError_vec.clear();
    h1D_DeltaPhiError_vec.clear();
    h1D_StrangenessError_vec.clear();
    h1D_GeneratorError_vec.clear();

    SetsPhenixStyle();
    c1 = new TCanvas("c1", "c1", 950, 800);

    // pad1 = new TPad("pad1", "pad1", 0, 0., 1, 0.7);
    // pad1->Draw();
    // pad1->cd();

    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    leg_errors = new TLegend(0.31,0.58,0.41,0.87);
    leg_errors -> SetBorderSize(0);
    leg_errors -> SetTextSize(0.03);

    leg_variation = new TLegend(0.21,0.85,0.41,1);
    leg_variation -> SetBorderSize(0);
    leg_variation -> SetTextSize(0.03);

    leg_variation_recoTracklet = new TLegend(0.21,0.85,0.41,1);
    leg_variation_recoTracklet -> SetBorderSize(0);
    leg_variation_recoTracklet -> SetTextSize(0.03);

    leg_final = new TLegend(0.45,0.75,0.8,0.87);
    leg_final -> SetBorderSize(0);
    leg_final -> SetTextSize(0.03);

    leg_TruthReco = new TLegend(0.52,0.81,0.72,0.9);
    leg_TruthReco -> SetBorderSize(0);
    leg_TruthReco -> SetTextSize(0.025);
    leg_TruthReco -> SetMargin(0.2);

    line = new TLine();
    line -> SetLineStyle(2);
    line -> SetLineWidth(2);
    line -> SetLineColor(28);

    file_out = new TFile(Form("%s/%s.root", final_output_directory.c_str(), output_folder_name.c_str()), "RECREATE");
    tree_out = new TTree("tree","max min unc.");

    tree_out -> Branch("UncRange_StatUnc", &UncRange_StatUnc);
    tree_out -> Branch("UncRange_RunSegment", &UncRange_RunSegment);
    tree_out -> Branch("UncRange_ClusAdc", &UncRange_ClusAdc);
    tree_out -> Branch("UncRange_GeoOffset", &UncRange_GeoOffset);
    tree_out -> Branch("UncRange_DeltaPhi", &UncRange_DeltaPhi);
    tree_out -> Branch("UncRange_Strangeness", &UncRange_Strangeness);
    tree_out -> Branch("UncRange_Generator", &UncRange_Generator);
    tree_out -> Branch("UncRange_ClusPhiSize", &UncRange_ClusPhiSize);
    tree_out -> Branch("UncRange_Final", &UncRange_Final);

    gStyle->SetPaintTextFormat("1.3f");
    
}

void FinalResult::PrepareBaseLineTGraph()
{
    gr_dNdEta_baseline = h1D_to_TGraph(h1D_data_standard);
    gr_dNdEta_baseline -> SetMarkerStyle(20);
    gr_dNdEta_baseline -> SetMarkerSize(1);
    gr_dNdEta_baseline -> SetMarkerColor(1);
    gr_dNdEta_baseline -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
    gr_dNdEta_baseline -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");

    gr_dNdEta_baseline -> GetXaxis() -> SetLabelOffset(999);
    gr_dNdEta_baseline -> GetXaxis() -> SetLabelSize(0);
    gr_dNdEta_baseline -> GetXaxis() -> SetTitleOffset(999);

    // todo : set the range of the graph here
    gr_dNdEta_baseline -> GetYaxis() -> SetRangeUser(
        gr_dNdEta_baseline -> GetYaxis() -> GetXmin() * 0.8,
        gr_dNdEta_baseline -> GetYaxis() -> GetXmax() * 1.2
    );

    gr_recoTracklet_baseline = h1D_to_TGraph(
        (TH1D*) file_in_data_standard -> Get(h1D_RecoTracklet_name.c_str())
    );
    gr_recoTracklet_baseline -> SetMarkerStyle(20);
    gr_recoTracklet_baseline -> SetMarkerSize(1);
    gr_recoTracklet_baseline -> SetMarkerColor(1);
    gr_recoTracklet_baseline -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
    gr_recoTracklet_baseline -> GetYaxis() -> SetTitle("Avg. reco. tracklets per event");

    gr_recoTracklet_baseline -> GetXaxis() -> SetLabelOffset(999);
    gr_recoTracklet_baseline -> GetXaxis() -> SetLabelSize(0);
    gr_recoTracklet_baseline -> GetXaxis() -> SetTitleOffset(999);

    gr_recoTracklet_baseline -> GetYaxis() -> SetRangeUser(
        gr_recoTracklet_baseline -> GetYaxis() -> GetXmin() * 0.8,
        gr_recoTracklet_baseline -> GetYaxis() -> GetXmax() * 1.2
    );
}

void FinalResult::PrepareMCClosureTGraph()
{
    gr_MC_closure_standard = h1D_to_TGraph(h1D_MC_closure_standard);
    gr_MC_closure_standard -> SetMarkerStyle(20);
    gr_MC_closure_standard -> SetMarkerSize(1);
    gr_MC_closure_standard -> SetMarkerColor(2);
}

void FinalResult::PrepareOutputFolderName()
{
    std::string runnumber_str = std::to_string( runnumber );
    if (runnumber != -1){
        int runnumber_str_len = 8;
        runnumber_str.insert(0, runnumber_str_len - runnumber_str.size(), '0');
    }

    output_folder_name = "Final";
    output_folder_name += "_Mbin" + std::to_string(Mbin);
    
    output_folder_name += Form("_%s",runnumber_str.c_str());
}


void FinalResult::PrepareStatisticalError()
{
    h1D_error_statistic = (TH1D*) h1D_data_standard -> Clone("h1D_error_statistic");
    h1D_error_statistic -> Reset("ICESM");

    for (int i = 1; i <= h1D_data_standard -> GetNbinsX(); i++){
        
        double error = h1D_data_standard -> GetBinError(i) / h1D_data_standard -> GetBinContent(i);
        error = (error == error && error != 1) ? error : 0;
        h1D_error_statistic -> SetBinContent(i, error);
    }

    h1D_error_statistic -> SetMarkerStyle(20);
    h1D_error_statistic -> SetMarkerSize(1);
    h1D_error_statistic -> SetLineWidth(0);
    h1D_error_statistic -> SetLineColorAlpha(1,0);
    h1D_error_statistic -> SetMarkerColor(TColor::GetColor(color_code[0].c_str()));

    UncRange_StatUnc = GetH1DMinMax(h1D_error_statistic); // note: with eta cut
}


void FinalResult::PrepareRunSegmentError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in)
{
    TFile * temp_file_in;

    c1 -> Clear();
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_dNdEta_baseline -> Draw("ap");
    leg_variation -> AddEntry(gr_dNdEta_baseline, file_title_in.back().c_str(), "p");
    leg_variation -> SetHeader(leg_header_in.c_str());

    std::vector<TGraph*> gr_variation_vec; gr_variation_vec.clear();

    for (int i = 0; i < file_directory_vec_in.size(); i++)
    {
        temp_file_in = TFile::Open(file_directory_vec_in[i].c_str());

        h1D_RunSegmentError_vec.push_back( (TH1D*) temp_file_in -> Get(StandardData_h1D_name.c_str()) );

        gr_variation_vec.push_back( h1D_to_TGraph(h1D_RunSegmentError_vec.back()) );
        gr_variation_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_vec.back() -> SetMarkerSize(1);
        gr_variation_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));
        std::cout<<"test, run seg variation: "<<gr_variation_vec.back() -> GetN()<<", "<<gr_variation_vec.back() -> GetPointY(0)<<std::endl;
        gr_variation_vec.back() -> Draw("p same");

        leg_variation -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");

        h1D_RunSegmentError_vec.back() -> Divide(h1D_data_standard);
        h1D_to_AbsRatio(h1D_RunSegmentError_vec.back());
    }

    h1D_error_Run_segmentation = h1D_FindLargestOnes("h1D_error_Run_segmentation", h1D_RunSegmentError_vec);

    h1D_error_Run_segmentation -> SetMarkerStyle(25);
    h1D_error_Run_segmentation -> SetMarkerSize(1);
    h1D_error_Run_segmentation -> SetLineWidth(0);
    h1D_error_Run_segmentation -> SetLineColorAlpha(1,0);
    h1D_error_Run_segmentation -> SetMarkerColor(TColor::GetColor(color_code[1].c_str()));

    UncRange_RunSegment = GetH1DMinMax(h1D_error_Run_segmentation); // note: with eta cut


    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_vec[i],
            gr_dNdEta_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_vec.back()->GetXaxis()->GetXmax(), 1);


    c1 -> cd();
    leg_variation -> Draw("same");
    c1 -> Print(Form("%s/RunSegmentationVariation_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();
    leg_variation -> Clear();
}

void FinalResult::PrepareClusAdcError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in)
{
    TFile * temp_file_in;

    c1 -> Clear();
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_dNdEta_baseline -> Draw("ap");
    leg_variation -> AddEntry(gr_dNdEta_baseline, file_title_in.back().c_str(), "p");
    leg_variation -> SetHeader(leg_header_in.c_str());
    
    leg_variation_recoTracklet -> AddEntry(gr_recoTracklet_baseline, file_title_in.back().c_str(), "p");
    leg_variation_recoTracklet -> SetHeader(leg_header_in.c_str());

    std::vector<TGraph*> gr_variation_vec; gr_variation_vec.clear();
    std::vector<TGraph*> gr_variation_recoTracklet_vec; gr_variation_recoTracklet_vec.clear();

    for (int i = 0; i < file_directory_vec_in.size(); i++)
    {
        temp_file_in = TFile::Open(file_directory_vec_in[i].c_str());

        h1D_ClusAdcError_vec.push_back( (TH1D*) temp_file_in -> Get(StandardData_h1D_name.c_str()) );

        gr_variation_vec.push_back( h1D_to_TGraph(h1D_ClusAdcError_vec.back()) );
        gr_variation_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_vec.back() -> SetMarkerSize(1);
        gr_variation_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));
        gr_variation_vec.back() -> Draw("p same");

        leg_variation -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");

        h1D_ClusAdcError_vec.back() -> Divide(h1D_data_standard);
        h1D_to_AbsRatio(h1D_ClusAdcError_vec.back());

        // Division : -----------------------------------
        gr_variation_recoTracklet_vec.push_back(
            h1D_to_TGraph(
                (TH1D*) temp_file_in -> Get(h1D_RecoTracklet_name.c_str())
            )
        );
        gr_variation_recoTracklet_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_recoTracklet_vec.back() -> SetMarkerSize(1);
        gr_variation_recoTracklet_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));

        leg_variation_recoTracklet -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");
    }

    h1D_error_ClusAdc = h1D_FindLargestOnes("h1D_error_ClusAdc", h1D_ClusAdcError_vec);

    h1D_error_ClusAdc -> SetMarkerStyle(26);
    h1D_error_ClusAdc -> SetMarkerSize(1);
    h1D_error_ClusAdc -> SetLineWidth(0);
    h1D_error_ClusAdc -> SetLineColorAlpha(1,0);
    h1D_error_ClusAdc -> SetMarkerColor(TColor::GetColor(color_code[2].c_str()));

    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_vec[i],
            gr_dNdEta_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_vec.back()->GetXaxis()->GetXmax(), 1);

    UncRange_ClusAdc = GetH1DMinMax(h1D_error_ClusAdc); // note: with eta cut

    c1 -> cd();
    leg_variation -> Draw("same");
    c1 -> Print(Form("%s/RunClusAdcVariation_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();

    // Division: --------------------------------------------------------------
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_recoTracklet_baseline -> Draw("ap");
    for (int i = 0; i < gr_variation_recoTracklet_vec.size(); i++){
        gr_variation_recoTracklet_vec[i] -> Draw("p same");
    }

    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_recoTracklet_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_recoTracklet_vec[i],
            gr_recoTracklet_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_recoTracklet_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_recoTracklet_vec.back()->GetXaxis()->GetXmax(), 1);

    c1 -> cd();
    leg_variation_recoTracklet -> Draw("same");
    c1 -> Print(Form("%s/RunClusAdcVariation_RecoTracklet_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();


    leg_variation -> Clear();
    leg_variation_recoTracklet -> Clear();

}

void FinalResult::PrepareGeoOffsetError(std::string file_directory_in, std::string alpha_corr_directory_in)
{
    TFile * temp_file_in_GeoUnc = TFile::Open(file_directory_in.c_str());
    TFile * temp_file_in_AlphaCorr = TFile::Open(alpha_corr_directory_in.c_str());

    h1D_error_GeoOffset = (TH1D*) h1D_data_standard -> Clone("h1D_error_GeoOffset");

    auto temp_h1D_GeoUnc = (TH1D*) temp_file_in_GeoUnc -> Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationSigma"); // note : it was: h1D_RotatedBkg_RecoTrackletEtaPerEvt_VariationMax
    auto temp_h1D_AlphaCorr = (TH1D*) temp_file_in_AlphaCorr -> Get("h1D_RotatedBkg_alpha_correction");

    h1D_error_GeoOffset -> Divide(
        temp_h1D_GeoUnc,
        temp_h1D_AlphaCorr 
    );

    for (int i = 1; i <= h1D_error_GeoOffset -> GetNbinsX(); i++){
        std::cout<<"GeoOffset: bin: "<< i <<", eta: ["<<h1D_error_GeoOffset->GetXaxis()->GetBinLowEdge(i)<<", "<<h1D_error_GeoOffset->GetXaxis()->GetBinUpEdge(i)<<"]"
        << ", GeoUnc: " << temp_h1D_GeoUnc -> GetBinContent(i)
        << ", AlphaCorr: " << temp_h1D_AlphaCorr -> GetBinContent(i)
        <<", final_error: " << h1D_error_GeoOffset -> GetBinContent(i)
        << std::endl;
    }
    

    h1D_error_GeoOffset -> SetMarkerStyle(27);
    h1D_error_GeoOffset -> SetMarkerSize(1);
    h1D_error_GeoOffset -> SetLineWidth(0);
    h1D_error_GeoOffset -> SetLineColorAlpha(1,0);
    h1D_error_GeoOffset -> SetMarkerColor(TColor::GetColor(color_code[3].c_str()));

    UncRange_GeoOffset = GetH1DMinMax(h1D_error_GeoOffset); // note: with eta cut
}


void FinalResult::PrepareDeltaPhiError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in)
{
    TFile * temp_file_in;

    c1 -> Clear();
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_dNdEta_baseline -> Draw("ap");
    leg_variation -> AddEntry(gr_dNdEta_baseline, file_title_in.back().c_str(), "p");
    leg_variation -> SetHeader(leg_header_in.c_str());

    leg_variation_recoTracklet -> AddEntry(gr_recoTracklet_baseline, file_title_in.back().c_str(), "p");
    leg_variation_recoTracklet -> SetHeader(leg_header_in.c_str());

    std::vector<TGraph*> gr_variation_vec; gr_variation_vec.clear();
    std::vector<TGraph*> gr_variation_recoTracklet_vec; gr_variation_recoTracklet_vec.clear();

    for (int i = 0; i < file_directory_vec_in.size(); i++)
    {
        temp_file_in = TFile::Open(file_directory_vec_in[i].c_str());

        h1D_DeltaPhiError_vec.push_back( (TH1D*) temp_file_in -> Get(StandardData_h1D_name.c_str()) );


        gr_variation_vec.push_back( h1D_to_TGraph(h1D_DeltaPhiError_vec.back()) );
        gr_variation_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_vec.back() -> SetMarkerSize(1);
        gr_variation_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));
        gr_variation_vec.back() -> Draw("p same");

        leg_variation -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");


        h1D_DeltaPhiError_vec.back() -> Divide(h1D_data_standard);
        h1D_to_AbsRatio(h1D_DeltaPhiError_vec.back());

        // Division : -----------------------------------
        gr_variation_recoTracklet_vec.push_back(
            h1D_to_TGraph(
                (TH1D*) temp_file_in -> Get(h1D_RecoTracklet_name.c_str())
            )
        );
        gr_variation_recoTracklet_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_recoTracklet_vec.back() -> SetMarkerSize(1);
        gr_variation_recoTracklet_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));

        leg_variation_recoTracklet -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");
    }

    h1D_error_DeltaPhi = h1D_FindLargestOnes("h1D_error_DeltaPhi", h1D_DeltaPhiError_vec);

    h1D_error_DeltaPhi -> SetMarkerStyle(28);
    h1D_error_DeltaPhi -> SetMarkerSize(1);
    h1D_error_DeltaPhi -> SetLineWidth(0);
    h1D_error_DeltaPhi -> SetLineColorAlpha(1,0);
    h1D_error_DeltaPhi -> SetMarkerColor(TColor::GetColor(color_code[4].c_str()));

    UncRange_DeltaPhi = GetH1DMinMax(h1D_error_DeltaPhi); // note: with eta cut

    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_vec[i],
            gr_dNdEta_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_vec.back()->GetXaxis()->GetXmax(), 1);

    c1 -> cd();
    leg_variation -> Draw("same");
    c1 -> Print(Form("%s/RunDeltaPhiVariation_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();

    // Division: --------------------------------------------------------------
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_recoTracklet_baseline -> Draw("ap");
    for (int i = 0; i < gr_variation_recoTracklet_vec.size(); i++){
        gr_variation_recoTracklet_vec[i] -> Draw("p same");
    }

    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_recoTracklet_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_recoTracklet_vec[i],
            gr_recoTracklet_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_recoTracklet_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_recoTracklet_vec.back()->GetXaxis()->GetXmax(), 1);

    c1 -> cd();
    leg_variation_recoTracklet -> Draw("same");
    c1 -> Print(Form("%s/RunDeltaPhiVariation_RecoTracklet_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();

    leg_variation -> Clear();
    leg_variation_recoTracklet -> Clear();
}

void FinalResult::PrepareStrangenessError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in)
{
    TFile * temp_file_in;

    c1 -> Clear();
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_dNdEta_baseline -> Draw("ap");
    leg_variation -> AddEntry(gr_dNdEta_baseline, file_title_in.back().c_str(), "p");
    leg_variation -> SetHeader(leg_header_in.c_str());

    leg_variation_recoTracklet -> AddEntry(gr_recoTracklet_baseline, file_title_in.back().c_str(), "p");
    leg_variation_recoTracklet -> SetHeader(leg_header_in.c_str());

    std::vector<TGraph*> gr_variation_vec; gr_variation_vec.clear();
    std::vector<TGraph*> gr_variation_recoTracklet_vec; gr_variation_recoTracklet_vec.clear();

    for (int i = 0; i < file_directory_vec_in.size(); i++)
    {
        temp_file_in = TFile::Open(file_directory_vec_in[i].c_str());

        h1D_StrangenessError_vec.push_back( (TH1D*) temp_file_in -> Get(StandardData_h1D_name.c_str()) );


        gr_variation_vec.push_back( h1D_to_TGraph(h1D_StrangenessError_vec.back()) );
        gr_variation_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_vec.back() -> SetMarkerSize(1);
        gr_variation_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));
        gr_variation_vec.back() -> Draw("p same");

        leg_variation -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");


        h1D_StrangenessError_vec.back() -> Divide(h1D_data_standard);
        h1D_to_AbsRatio(h1D_StrangenessError_vec.back());

        // Division : -----------------------------------
        gr_variation_recoTracklet_vec.push_back(
            h1D_to_TGraph(
                (TH1D*) temp_file_in -> Get(h1D_RecoTracklet_name.c_str())
            )
        );
        gr_variation_recoTracklet_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_recoTracklet_vec.back() -> SetMarkerSize(1);
        gr_variation_recoTracklet_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));

        leg_variation_recoTracklet -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");
    }

    h1D_error_Strangeness = h1D_FindLargestOnes("h1D_error_Strangeness", h1D_StrangenessError_vec);

    h1D_error_Strangeness -> SetMarkerStyle(42);
    h1D_error_Strangeness -> SetMarkerSize(1);
    h1D_error_Strangeness -> SetLineWidth(0);
    h1D_error_Strangeness -> SetLineColorAlpha(1,0);
    h1D_error_Strangeness -> SetMarkerColor(TColor::GetColor(color_code[6].c_str()));

    UncRange_Strangeness = GetH1DMinMax(h1D_error_Strangeness); // note: with eta cut

    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_vec[i],
            gr_dNdEta_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_vec.back()->GetXaxis()->GetXmax(), 1);

    c1 -> cd();
    leg_variation -> Draw("same");
    c1 -> Print(Form("%s/RunStrangenessVariation_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();

    // Division: --------------------------------------------------------------
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_recoTracklet_baseline -> Draw("ap");
    for (int i = 0; i < gr_variation_recoTracklet_vec.size(); i++){
        gr_variation_recoTracklet_vec[i] -> Draw("p same");
    }

    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_recoTracklet_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_recoTracklet_vec[i],
            gr_recoTracklet_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_recoTracklet_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_recoTracklet_vec.back()->GetXaxis()->GetXmax(), 1);

    c1 -> cd();
    leg_variation_recoTracklet -> Draw("same");
    c1 -> Print(Form("%s/RunStrangenessVariation_RecoTracklet_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();

    leg_variation -> Clear();
    leg_variation_recoTracklet -> Clear();
}

void FinalResult::PrepareGeneratorError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in)
{
    TFile * temp_file_in;

    c1 -> Clear();
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_dNdEta_baseline -> Draw("ap");
    leg_variation -> AddEntry(gr_dNdEta_baseline, file_title_in.back().c_str(), "p");
    leg_variation -> SetHeader(leg_header_in.c_str());

    leg_variation_recoTracklet -> AddEntry(gr_recoTracklet_baseline, file_title_in.back().c_str(), "p");
    leg_variation_recoTracklet -> SetHeader(leg_header_in.c_str());

    std::vector<TGraph*> gr_variation_vec; gr_variation_vec.clear();
    std::vector<TGraph*> gr_variation_recoTracklet_vec; gr_variation_recoTracklet_vec.clear();

    for (int i = 0; i < file_directory_vec_in.size(); i++)
    {
        temp_file_in = TFile::Open(file_directory_vec_in[i].c_str());

        h1D_GeneratorError_vec.push_back( (TH1D*) temp_file_in -> Get(StandardData_h1D_name.c_str()) );


        gr_variation_vec.push_back( h1D_to_TGraph(h1D_GeneratorError_vec.back()) );
        gr_variation_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_vec.back() -> SetMarkerSize(1);
        gr_variation_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));
        gr_variation_vec.back() -> Draw("p same");

        leg_variation -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");


        h1D_GeneratorError_vec.back() -> Divide(h1D_data_standard);
        h1D_to_AbsRatio(h1D_GeneratorError_vec.back());

        // Division : -----------------------------------
        gr_variation_recoTracklet_vec.push_back(
            h1D_to_TGraph(
                (TH1D*) temp_file_in -> Get(h1D_RecoTracklet_name.c_str())
            )
        );
        gr_variation_recoTracklet_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_recoTracklet_vec.back() -> SetMarkerSize(1);
        gr_variation_recoTracklet_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));

        leg_variation_recoTracklet -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");
    }

    h1D_error_Generator = h1D_FindLargestOnes("h1D_error_Generator", h1D_GeneratorError_vec);

    h1D_error_Generator -> SetMarkerStyle(32);
    h1D_error_Generator -> SetMarkerSize(1);
    h1D_error_Generator -> SetLineWidth(0);
    h1D_error_Generator -> SetLineColorAlpha(1,0);
    h1D_error_Generator -> SetMarkerColor(TColor::GetColor(color_code[7].c_str()));

    UncRange_Generator = GetH1DMinMax(h1D_error_Generator); // note: with eta cut

    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_vec[i],
            gr_dNdEta_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_vec.back()->GetXaxis()->GetXmax(), 1);

    c1 -> cd();
    leg_variation -> Draw("same");
    c1 -> Print(Form("%s/RunGeneratorVariation_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();

    // Division: --------------------------------------------------------------
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_recoTracklet_baseline -> Draw("ap");
    for (int i = 0; i < gr_variation_recoTracklet_vec.size(); i++){
        gr_variation_recoTracklet_vec[i] -> Draw("p same");
    }

    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_recoTracklet_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_recoTracklet_vec[i],
            gr_recoTracklet_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_recoTracklet_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_recoTracklet_vec.back()->GetXaxis()->GetXmax(), 1);

    c1 -> cd();
    leg_variation_recoTracklet -> Draw("same");
    c1 -> Print(Form("%s/RunGeneratorVariation_RecoTracklet_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();

    leg_variation -> Clear();
    leg_variation_recoTracklet -> Clear();
}

void FinalResult::PrepareClusPhiSizeError(std::vector<std::string> file_directory_vec_in, std::vector<std::string> file_title_in, std::string leg_header_in)
{
    TFile * temp_file_in;

    c1 -> Clear();
    c1 -> cd();
    pad1 = new TPad("pad1", "pad1", 0, 0.31, 1, 0.85);
    pad1 -> Draw();
    pad1 -> cd();

    gr_dNdEta_baseline -> Draw("ap");
    leg_variation -> AddEntry(gr_dNdEta_baseline, file_title_in.back().c_str(), "p");
    leg_variation -> SetHeader(leg_header_in.c_str());

    std::vector<TGraph*> gr_variation_vec; gr_variation_vec.clear();

    for (int i = 0; i < file_directory_vec_in.size(); i++)
    {
        temp_file_in = TFile::Open(file_directory_vec_in[i].c_str());

        h1D_ClusPhiSizeError_vec.push_back( (TH1D*) temp_file_in -> Get(StandardData_h1D_name.c_str()) );


        gr_variation_vec.push_back( h1D_to_TGraph(h1D_ClusPhiSizeError_vec.back()) );
        gr_variation_vec.back() -> SetMarkerStyle(marker_code[i]);
        gr_variation_vec.back() -> SetMarkerSize(1);
        gr_variation_vec.back() -> SetMarkerColor(TColor::GetColor(color_code[i].c_str()));
        gr_variation_vec.back() -> Draw("p same");

        leg_variation -> AddEntry(gr_variation_vec.back(), file_title_in[i].c_str(), "p");


        h1D_ClusPhiSizeError_vec.back() -> Divide(h1D_data_standard);
        h1D_to_AbsRatio(h1D_ClusPhiSizeError_vec.back());
    }

    h1D_error_ClusPhiSize = h1D_FindLargestOnes("h1D_error_ClusPhiSize", h1D_ClusPhiSizeError_vec);

    h1D_error_ClusPhiSize -> SetMarkerStyle(30);
    h1D_error_ClusPhiSize -> SetMarkerSize(1);
    h1D_error_ClusPhiSize -> SetLineWidth(0);
    h1D_error_ClusPhiSize -> SetLineColorAlpha(1,0);
    h1D_error_ClusPhiSize -> SetMarkerColor(TColor::GetColor(color_code[5].c_str()));

    UncRange_ClusPhiSize = GetH1DMinMax(h1D_error_ClusPhiSize); // note: with eta cut

    c1 -> cd();
    pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.38);
    pad2 -> Draw();
    pad2 -> cd();

    for (int i = 0; i < gr_variation_vec.size(); i++){
        std::string plot_draw_text = (i == 0) ? "ap" : "p same";
        
        TGraph * temp_gr = GetRatioGr(
            gr_variation_vec[i],
            gr_dNdEta_baseline
        );
        temp_gr -> GetYaxis() -> SetRangeUser(0.9,1.1);
        temp_gr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
        temp_gr -> GetYaxis() -> SetTitle("Ratio to baseline");
        temp_gr -> Draw(plot_draw_text.c_str());
    }
    line -> DrawLine(gr_variation_vec.back()->GetXaxis()->GetXmin(), 1, gr_variation_vec.back()->GetXaxis()->GetXmax(), 1);

    c1 -> cd();
    leg_variation -> Draw("same");
    c1 -> Print(Form("%s/RunClusPhiSizeVariation_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();
    leg_variation -> Clear();

}

void FinalResult::PrepareMCMergedError(std::vector<std::string> file_directory_vec_in)
{
    TFile * temp_file_in;

    for (std::string file_dir : file_directory_vec_in)
    {
        temp_file_in = TFile::Open(file_dir.c_str());

        h1D_MCMergedError_vec.push_back( (TH1D*) temp_file_in -> Get(StandardData_h1D_name.c_str()) );
        h1D_MCMergedError_vec.back() -> Divide(h1D_data_standard);
        h1D_to_AbsRatio(h1D_MCMergedError_vec.back());
    }

    h1D_error_MCMerged = h1D_FindLargestOnes("h1D_error_MCMerged", h1D_MCMergedError_vec);

    h1D_error_MCMerged -> SetMarkerStyle(20);
    h1D_error_MCMerged -> SetMarkerSize(1);
    h1D_error_MCMerged -> SetLineWidth(0);
    h1D_error_MCMerged -> SetLineColorAlpha(1,0);
    h1D_error_MCMerged -> SetMarkerColor(TColor::GetColor(color_code[6].c_str()));

}


void FinalResult::PrepareFinalError()
{
    if (h1D_error_statistic == nullptr){
        std::cout << "Error : the statistical error is not found" << std::endl;
        exit(1);
    }

    h1D_error_Final = (TH1D*) h1D_error_statistic -> Clone("h1D_error_Final");
    h1D_error_Final -> Reset("ICESM");

    for (int i = 1; i <= h1D_error_Final -> GetNbinsX(); i++){

        double _error_statistic = (h1D_error_statistic != nullptr) ? h1D_error_statistic -> GetBinContent(i) : 0;
        _error_statistic = (_error_statistic == _error_statistic && _error_statistic != 1) ? _error_statistic : 0;

        double _error_Run_segmentation = (h1D_error_Run_segmentation != nullptr) ? h1D_error_Run_segmentation -> GetBinContent(i) : 0;
        _error_Run_segmentation = (_error_Run_segmentation == _error_Run_segmentation && _error_Run_segmentation != 1) ? _error_Run_segmentation : 0;

        double _error_ClusAdc = (h1D_error_ClusAdc != nullptr) ? h1D_error_ClusAdc -> GetBinContent(i) : 0;
        _error_ClusAdc = (_error_ClusAdc == _error_ClusAdc && _error_ClusAdc != 1) ? _error_ClusAdc : 0;

        double _error_GeoOffset = (h1D_error_GeoOffset != nullptr) ? h1D_error_GeoOffset -> GetBinContent(i) : 0;
        _error_GeoOffset = (_error_GeoOffset == _error_GeoOffset && _error_GeoOffset != 1) ? _error_GeoOffset : 0;

        double _error_DeltaPhi = (h1D_error_DeltaPhi != nullptr) ? h1D_error_DeltaPhi -> GetBinContent(i) : 0;
        _error_DeltaPhi = (_error_DeltaPhi == _error_DeltaPhi && _error_DeltaPhi != 1) ? _error_DeltaPhi : 0;

        double _error_Strangeness = (h1D_error_Strangeness != nullptr) ? h1D_error_Strangeness -> GetBinContent(i) : 0;
        _error_Strangeness = (_error_Strangeness == _error_Strangeness && _error_Strangeness != 1) ? _error_Strangeness : 0;

        double _error_Generator = (h1D_error_Generator != nullptr) ? h1D_error_Generator -> GetBinContent(i) : 0;
        _error_Generator = (_error_Generator == _error_Generator && _error_Generator != 1) ? _error_Generator : 0;

        double _error_ClusPhiSize = (h1D_error_ClusPhiSize != nullptr) ? h1D_error_ClusPhiSize -> GetBinContent(i) : 0;
        _error_ClusPhiSize = (_error_ClusPhiSize == _error_ClusPhiSize && _error_ClusPhiSize != 1) ? _error_ClusPhiSize : 0;

        double _error_MCMerged = (h1D_error_MCMerged != nullptr) ? h1D_error_MCMerged -> GetBinContent(i) : 0;
        _error_MCMerged = (_error_MCMerged == _error_MCMerged && _error_MCMerged != 1) ? _error_MCMerged : 0;


        double _final_error = std::sqrt(
            pow(_error_statistic, 2) +
            pow(_error_Run_segmentation, 2) +
            pow(_error_ClusAdc, 2) +
            pow(_error_GeoOffset, 2) +
            pow(_error_DeltaPhi, 2) +
            pow(_error_Strangeness, 2) +
            pow(_error_Generator, 2) + 
            pow(_error_ClusPhiSize, 2) +
            pow(_error_MCMerged, 2)
        );

        std::cout<<"bin: "<< i <<", eta: ["<<h1D_error_Final->GetXaxis()->GetBinLowEdge(i)<<", "<<h1D_error_Final->GetXaxis()->GetBinUpEdge(i)<<"]"
        << ", statistic: " << Form("%.5f",_error_statistic) 
        << ", Run_segmentation: " << Form("%.5f",_error_Run_segmentation) 
        << ", ClusAdc: " << Form("%.5f",_error_ClusAdc) 
        << ", GeoOffset: " << Form("%.5f",_error_GeoOffset) 
        << ", DeltaPhi: " << Form("%.5f",_error_DeltaPhi) 
        << ", ClusPhiSize: " << Form("%.5f",_error_ClusPhiSize)
        << ", Strangeness: " << Form("%.5f",_error_Strangeness)
        << ", Generator: " << Form("%.5f",_error_Generator)
        // << ", MCMerged: " << Form("%.5f",_error_MCMerged)
        << " final_error: " << Form("%.5f",_final_error) 
        << std::endl;

        h1D_error_Final -> SetBinContent(i, _final_error);
    }

    // todo : the bin removal is here
    for (int i = 1; i <= h1D_error_Final -> GetNbinsX(); i++)
    {
        double lowEdge = h1D_error_Final -> GetXaxis() -> GetBinLowEdge(i);
        double upEdge = h1D_error_Final -> GetXaxis() -> GetBinUpEdge(i);
        double binCenter = h1D_error_Final -> GetXaxis() -> GetBinCenter(i);

        if (binCenter < eta_range.first || binCenter > eta_range.second){
            
            std::cout<<"Rejection, bin: "<< i <<", eta: ["<<lowEdge<<", "<<upEdge<<"]"
            << ", Total error: " << h1D_error_Final -> GetBinContent(i)
            << std::endl;

            h1D_error_Final -> SetBinContent(i, -30);
            h1D_error_Final -> SetBinError(i, 0);
            
            if (h1D_error_statistic != nullptr) {
                h1D_error_statistic -> SetBinContent(i, -30);
                h1D_error_statistic -> SetBinError(i, 0);
            }

            if (h1D_error_Run_segmentation != nullptr) {
                h1D_error_Run_segmentation -> SetBinContent(i, -30);
                h1D_error_Run_segmentation -> SetBinError(i, 0);
            }

            if (h1D_error_ClusAdc != nullptr) {
                h1D_error_ClusAdc -> SetBinContent(i, -30);
                h1D_error_ClusAdc -> SetBinError(i, 0);
            }

            if (h1D_error_GeoOffset != nullptr) {
                h1D_error_GeoOffset -> SetBinContent(i, -30);
                h1D_error_GeoOffset -> SetBinError(i, 0);
            }

            if (h1D_error_DeltaPhi != nullptr) {
                h1D_error_DeltaPhi -> SetBinContent(i, -30);
                h1D_error_DeltaPhi -> SetBinError(i, 0);
            }

            if (h1D_error_Strangeness != nullptr) {
                h1D_error_Strangeness -> SetBinContent(i, -30);
                h1D_error_Strangeness -> SetBinError(i, 0);
            }

            if (h1D_error_Generator != nullptr) {
                h1D_error_Generator -> SetBinContent(i, -30);
                h1D_error_Generator -> SetBinError(i, 0);
            }

            if (h1D_error_ClusPhiSize != nullptr) {
                h1D_error_ClusPhiSize -> SetBinContent(i, -30);
                h1D_error_ClusPhiSize -> SetBinError(i, 0);
            }

            if (h1D_error_MCMerged != nullptr) {
                h1D_error_MCMerged -> SetBinContent(i, -30);
                h1D_error_MCMerged -> SetBinError(i, 0);
            }

        }
    }    


    // Division : ------------------------------------------------------------------------------------------------------------------------------------
    // h1D_error_Final -> SetMaximum(h1D_error_Final -> GetBinContent(h1D_error_Final -> GetMaximumBin()) * 2.);
    h1D_error_Final -> SetMinimum(0);
    h1D_error_Final -> SetMaximum(SystUncPlot_Ymax); // todo : the syst unc. Y max
    h1D_error_Final -> SetLineWidth(2);
    h1D_error_Final -> SetLineColor(1);
    h1D_error_Final -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
    h1D_error_Final -> GetYaxis() -> SetTitle("Relative uncertainty");

    leg_errors -> AddEntry(h1D_error_Final, "Total Uncertainty", "l");
    if (h1D_error_statistic != nullptr) {leg_errors -> AddEntry(h1D_error_statistic, "Stat. Unc.", "p");}
    if (h1D_error_Run_segmentation != nullptr) {leg_errors -> AddEntry(h1D_error_Run_segmentation, "Run segmentation variation", "p");}
    if (h1D_error_ClusAdc != nullptr) {leg_errors -> AddEntry(h1D_error_ClusAdc, "Cluster ADC variation", "p");}
    if (h1D_error_GeoOffset != nullptr) {leg_errors -> AddEntry(h1D_error_GeoOffset, "Geo. misalignment variation", "p");}
    if (h1D_error_DeltaPhi != nullptr) {leg_errors -> AddEntry(h1D_error_DeltaPhi, "#Delta#phi cut variation", "p");}
    if (h1D_error_ClusPhiSize != nullptr) {leg_errors -> AddEntry(h1D_error_ClusPhiSize, "Cluster#kern[0.4]{#phi} size variation", "p");}
    if (h1D_error_Strangeness != nullptr) {leg_errors -> AddEntry(h1D_error_Strangeness, "Strangeness variation", "p");}
    if (h1D_error_Generator != nullptr) {leg_errors -> AddEntry(h1D_error_Generator, "Generator variation", "p");}
    if (h1D_error_MCMerged != nullptr) {leg_errors -> AddEntry(h1D_error_MCMerged, "MC merge variation", "p");}

    file_out -> cd();
    
    c1 -> cd();
    h1D_error_Final -> Draw("hist");

    UncRange_Final = GetH1DMinMax(h1D_error_Final); // note: with eta cut

    if (h1D_error_statistic != nullptr) {h1D_error_statistic -> Draw("p same");}
    if (h1D_error_Run_segmentation != nullptr) {h1D_error_Run_segmentation -> Draw("p same");}
    if (h1D_error_ClusAdc != nullptr) {h1D_error_ClusAdc -> Draw("p same");}
    if (h1D_error_GeoOffset != nullptr) {h1D_error_GeoOffset -> Draw("p same");}
    if (h1D_error_DeltaPhi != nullptr) {h1D_error_DeltaPhi -> Draw("p same");}
    if (h1D_error_ClusPhiSize != nullptr) {h1D_error_ClusPhiSize -> Draw("p same");}
    if (h1D_error_Strangeness != nullptr) {h1D_error_Strangeness -> Draw("p same");}
    if (h1D_error_Generator != nullptr) {h1D_error_Generator -> Draw("p same");}
    if (h1D_error_MCMerged != nullptr) {h1D_error_MCMerged -> Draw("p same");}

    if (AnaDescription.second.size() > 0){
        draw_text -> DrawLatex(AnaDescription.first.first, AnaDescription.first.second, AnaDescription.second.c_str());
    }

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str()));    

    leg_errors -> Draw("same");

    c1 -> Print(Form("%s/SystUnc_Summary_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Write("SystUnc_Summary");

    c1 -> Clear();
    // c1 -> cd();
    // if (h1D_error_statistic != nullptr) {
    //     h1D_error_statistic -> Draw("p");
    //     c1 -> Print(Form("%s/SystUnc_Stat_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    // }
}

void FinalResult::PrepareFinalResult(double Hist_Y_max)
{
    h1D_data_standard -> SetMarkerStyle(20);
    h1D_data_standard -> SetMarkerSize(1);
    h1D_data_standard -> SetMarkerColor(1);
    h1D_data_standard -> SetFillColorAlpha(1,0.5);
    h1D_data_standard -> SetLineColorAlpha(1,0);
    h1D_data_standard -> SetLineWidth(0);

    gE_data_final = new TGraphErrors();
    gE_data_final -> SetMarkerStyle(20);
    gE_data_final -> SetMarkerSize(1);
    gE_data_final -> SetMarkerColor(1);
    gE_data_final -> SetFillColorAlpha(1,0.5);
    gE_data_final -> SetLineColorAlpha(1,0);
    gE_data_final -> SetLineWidth(0);

    for (int i = 1; i <= h1D_data_standard -> GetNbinsX(); i++){
        double _content = h1D_data_standard -> GetBinContent(i);
        double _error = h1D_error_Final -> GetBinContent(i);
        double _BinCenter = h1D_data_standard -> GetXaxis() -> GetBinCenter(i);
        double _BinWidth = h1D_data_standard -> GetXaxis() -> GetBinWidth(i);
        
        double final_error = (_content != 0) ? _content * _error : 0.;


        h1D_data_standard -> SetBinError(i, _error * _content);

        // if (_content == 0) {h1D_data_standard -> SetBinContent(i, -10);}
        if (_BinCenter < eta_range.first || _BinCenter > eta_range.second){
            h1D_data_standard -> SetBinContent(i, -10);
            h1D_data_standard -> SetBinError(i, 0);
        }
        
        if (_BinCenter >= eta_range.first && _BinCenter <= eta_range.second){
            gE_data_final -> SetPoint(gE_data_final -> GetN(), _BinCenter, _content);
            gE_data_final -> SetPointError(gE_data_final -> GetN() - 1, _BinWidth/2., final_error);

            std::cout<<"bin: "<< i <<", eta: ["<<h1D_data_standard->GetXaxis()->GetBinLowEdge(i)<<", "<<h1D_data_standard->GetXaxis()->GetBinUpEdge(i)<<"]"
            << ", content: " << Form("%.3f",_content)
            << ", error: " << Form("%.3f",_error)
            << ", final_error: " << Form("%.3f",final_error)
            << std::endl;

        }
    }

    gE_data_final -> GetYaxis() -> SetRangeUser(0, Hist_Y_max);
    gE_data_final -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
    gE_data_final -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");

    h1D_data_standard -> SetMinimum(0);
    h1D_data_standard -> SetMaximum(Hist_Y_max); // todo: the maximum
    h1D_data_standard -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
    h1D_data_standard -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");

    h1D_truth_standard -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
    h1D_truth_standard -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
    h1D_truth_standard -> SetMinimum(0);
    h1D_truth_standard -> SetMaximum(Hist_Y_max); // todo: the maximum
    h1D_truth_standard -> SetLineColor(TColor::GetColor("#3288bd"));
    h1D_truth_standard -> SetLineWidth(2);
    h1D_truth_standard -> SetFillColorAlpha(1,0);

    leg_final -> AddEntry(h1D_data_standard, Final_Data_MC_text.first.c_str(), "fp");
    leg_final -> AddEntry(h1D_truth_standard, Final_Data_MC_text.second.c_str(), "l");
    leg_final -> AddEntry(gr_MC_closure_standard, "MC closure test", "p");

    // Division : ------------------------------------------------------------------------------------------------------------------------------------
    file_out -> cd();
    c1 -> cd();
    c1 -> Clear();

    // h1D_data_standard -> Draw("E3");
    h1D_truth_standard -> Draw("hist");
    gr_MC_closure_standard -> Draw("p same");
    // gE_data_final -> Draw("E3");

    if (Have_PHOBOS && Mbin < 11) // note : only up to 50%
    {
        TGraphAsymmErrors * PHOBOS_grae = GetPHOBOSData();
        leg_final -> AddEntry(PHOBOS_grae, "PHOBOS", "fp");
        PHOBOS_grae -> Draw("E3 same");
        PHOBOS_grae -> Draw("p same");
    }

    gE_data_final -> Draw("P2 same");
    // gE_data_final -> Draw("p same");
    // h1D_data_standard -> Draw("p same");

    if (AnaDescription.second.size() > 0){
        draw_text -> DrawLatex(AnaDescription.first.first, AnaDescription.first.second, AnaDescription.second.c_str());
    }

    if (Collision_str.second.size() > 0){
        draw_text -> DrawLatex(Collision_str.first.first, Collision_str.first.second, Collision_str.second.c_str());
    }

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str()));
    leg_final -> Draw("same");

    c1 -> Print(Form("%s/FinalResult_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Write("FinalResult");

}

void FinalResult::EndRun()
{   
    tree_out -> Fill();
    tree_out -> Write();

    std::cout<<"========================================================================================="<<std::endl;
    std::cout<<Form("StatUnc,     min: %.4f%, max: %.4f%", 100 * UncRange_StatUnc.first, 100 * UncRange_StatUnc.second)<<std::endl;
    std::cout<<Form("RunSegment,  min: %.4f%, max: %.4f%", 100 * UncRange_RunSegment.first, 100 * UncRange_RunSegment.second)<<std::endl;
    std::cout<<Form("ClusAdc,     min: %.4f%, max: %.4f%", 100 * UncRange_ClusAdc.first, 100 * UncRange_ClusAdc.second)<<std::endl;
    std::cout<<Form("GeoOffset,   min: %.4f%, max: %.4f%", 100 * UncRange_GeoOffset.first, 100 * UncRange_GeoOffset.second)<<std::endl;
    std::cout<<Form("DeltaPhi,    min: %.4f%, max: %.4f%", 100 * UncRange_DeltaPhi.first, 100 * UncRange_DeltaPhi.second)<<std::endl;
    std::cout<<Form("ClusPhiSize, min: %.4f%, max: %.4f%", 100 * UncRange_ClusPhiSize.first, 100 * UncRange_ClusPhiSize.second)<<std::endl;
    std::cout<<Form("Strangeness,    min: %.4f%, max: %.4f%", 100 * UncRange_Strangeness.first, 100 * UncRange_Strangeness.second)<<std::endl;
    std::cout<<Form("Generator,    min: %.4f%, max: %.4f%", 100 * UncRange_Generator.first, 100 * UncRange_Generator.second)<<std::endl;
    std::cout<<Form("Final,       min: %.4f%, max: %.4f%", 100 * UncRange_Final.first, 100 * UncRange_Final.second)<<std::endl;
    std::cout<<"========================================================================================="<<std::endl;

    gE_data_final -> Write("gE_dNdEta_reco");
    h1D_data_standard -> Write("h1D_dNdEta_reco");
    h1D_truth_standard -> Write("h1D_dNdEta_truth");
    h1D_MC_closure_standard -> Write("h1D_MC_closure");
    gr_MC_closure_standard -> Write("gr_MC_closure");

    if(h1D_error_statistic != nullptr) {h1D_error_statistic->SetMarkerColor(1); h1D_error_statistic -> Write();}
    if(h1D_error_Run_segmentation != nullptr) {h1D_error_Run_segmentation -> Write();}
    if(h1D_error_ClusAdc != nullptr) {h1D_error_ClusAdc -> Write();}
    if(h1D_error_GeoOffset != nullptr) {h1D_error_GeoOffset -> Write();}
    if(h1D_error_DeltaPhi != nullptr) {h1D_error_DeltaPhi -> Write();}
    if(h1D_error_ClusPhiSize != nullptr) {h1D_error_ClusPhiSize -> Write();}
    if(h1D_error_Strangeness != nullptr) {h1D_error_Strangeness -> Write();}
    if(h1D_error_Generator != nullptr) {h1D_error_Generator -> Write();}
    if(h1D_error_MCMerged != nullptr) {h1D_error_MCMerged -> Write();}
    if(h1D_error_Final != nullptr) {h1D_error_Final -> Write();}

    file_out -> Close();
}

void FinalResult::h1D_to_AbsRatio(TH1D * h1D_in)
{
    for (int i = 1; i <= h1D_in -> GetNbinsX(); i++){
        h1D_in -> SetBinContent(i, std::abs( h1D_in -> GetBinContent(i) - 1 ));
    }
}

TH1D * FinalResult::h1D_FindLargestOnes(std::string hist_name, std::vector<TH1D*> h1D_vec_in)
{
    TH1D * h1D_out = (TH1D*) h1D_vec_in.front()->Clone(hist_name.c_str());

    for (int i = 1; i < h1D_vec_in.size(); i++)
    {
        for (int j = 1; j <= h1D_out -> GetNbinsX(); j++)
        {
            if (h1D_vec_in[i] -> GetBinContent(j) > h1D_out -> GetBinContent(j)){
                h1D_out -> SetBinContent(j, h1D_vec_in[i] -> GetBinContent(j));
            }
        }
    }

    return h1D_out;
}

TGraph * FinalResult::h1D_to_TGraph(TH1D * hist_in)
{
    TGraph * g_out = new TGraph();

    // todo: there is a eta_range cut 
    for (int i = 1; i <= hist_in -> GetNbinsX(); i++){
        
        double H1DBinCenterX = hist_in -> GetXaxis() -> GetBinCenter(i);

        if (H1DBinCenterX < eta_range.first || H1DBinCenterX > eta_range.second){
            continue;
        }

        g_out -> SetPoint(g_out -> GetN(), H1DBinCenterX, hist_in -> GetBinContent(i));
    }

    return g_out;
}

void FinalResult::DrawAlphaCorrectionPlots(
    std::string AlphaCorr_file_directory, 
    std::vector<std::tuple<double,double,std::string>> additional_text,
    std::vector<std::tuple<int, std::string, std::string>> legend_text
)
{
    c1 -> cd();
    TFile * temp_file_in = TFile::Open(AlphaCorr_file_directory.c_str());
    
    TH1D * h1D_AlphaCorr = (TH1D*) temp_file_in -> Get(h1D_AlphaCorr_name.c_str());
    h1D_AlphaCorr -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
    h1D_AlphaCorr -> GetYaxis() -> SetTitle("Acc. & Effi. correction (Reco. / Gen.)");
    h1D_AlphaCorr -> SetMinimum(0);
    h1D_AlphaCorr -> SetMaximum(1.4);
    h1D_AlphaCorr -> SetLineWidth(2);

    h1D_AlphaCorr -> SetMarkerSize(1);
    h1D_AlphaCorr -> Draw("hist TEXT90");

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));

    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    c1 -> Print(Form("%s/h1D_AlphaCorrection_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();


    // Division : ------------------------------------------------------------------------------------------------------------------------------------
    TH1D * h1D_truth_hadron_001 = (TH1D*) temp_file_in -> Get(StandardTruth_h1D_name.c_str());
    TH1D * h1D_recoTracklet_001 = (TH1D*) temp_file_in -> Get(h1D_RecoTracklet_name.c_str());

    h1D_truth_hadron_001  -> SetMarkerColor(std::get<0>(legend_text[0]));
    h1D_truth_hadron_001    -> SetLineColor(std::get<0>(legend_text[0]));
    leg_TruthReco -> AddEntry(h1D_truth_hadron_001, std::get<1>(legend_text[0]).c_str(), std::get<2>(legend_text[0]).c_str());
    h1D_truth_hadron_001 -> SetMinimum(0);
    h1D_truth_hadron_001 -> SetMaximum(
        h1D_truth_hadron_001 -> GetBinContent(h1D_truth_hadron_001 -> GetMaximumBin()) * 1.45
    );
    h1D_truth_hadron_001 -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
    h1D_truth_hadron_001 -> GetYaxis() -> SetTitle("Avg. multiplicity per event");
    h1D_truth_hadron_001 -> SetLineWidth(2);

    h1D_recoTracklet_001  -> SetMarkerColor(std::get<0>(legend_text[1]));
    h1D_recoTracklet_001    -> SetLineColor(std::get<0>(legend_text[1]));
    leg_TruthReco -> AddEntry(h1D_recoTracklet_001, std::get<1>(legend_text[1]).c_str(), std::get<2>(legend_text[1]).c_str());
    h1D_recoTracklet_001 -> SetLineWidth(2);

    c1 -> cd();
    h1D_truth_hadron_001 -> Draw("hist");
    h1D_recoTracklet_001 -> Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Simulation"));

    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    leg_TruthReco -> Draw("same");

    c1 -> Print(Form("%s/h1D_TruthReco_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();
    leg_TruthReco -> Clear();

    temp_file_in -> Close();
}

void FinalResult::DrawGoodPair2DFineBinPlot(std::vector<std::tuple<double,double,std::string>> additional_text)
{

    c1 -> cd();
    
    TH2D * h2D_GoodPair = (TH2D*) file_in_data_standard -> Get(h2D_GoodProtoTracklet_EtaVtxZ_FineBin_name.c_str());
    h2D_GoodPair -> GetXaxis() -> SetTitle("Pair #eta");
    h2D_GoodPair -> GetYaxis() -> SetTitle("INTT vtxZ [cm]");
    h2D_GoodPair -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str()));

    c1 -> Print(Form("%s/GoodPair2DFineBin_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();
}

void FinalResult::DrawRecoTrackletDataMCPlot(
    std::vector<std::tuple<double,double,std::string>> additional_text,
    std::vector<std::tuple<int, std::string, std::string>> legend_text
)
{
    TH1D * h1D_recoTracklet_data = (TH1D*) file_in_data_standard -> Get(h1D_RecoTracklet_name.c_str());
    TH1D * h1D_recoTracklet_MC = (TH1D*) file_in_MC_standard -> Get(h1D_RecoTracklet_name.c_str());

    h1D_recoTracklet_data  -> SetMarkerColor(std::get<0>(legend_text[0]));
    h1D_recoTracklet_data    -> SetLineColor(std::get<0>(legend_text[0]));
    leg_TruthReco -> AddEntry(h1D_recoTracklet_data, std::get<1>(legend_text[0]).c_str(), std::get<2>(legend_text[0]).c_str());
    h1D_recoTracklet_data -> SetMinimum(0);
    h1D_recoTracklet_data -> SetMaximum(
        h1D_recoTracklet_data -> GetBinContent(h1D_recoTracklet_data -> GetMaximumBin()) * 1.45
    );
    h1D_recoTracklet_data -> GetXaxis() -> SetTitle("Pseudorapidity #eta");
    h1D_recoTracklet_data -> GetYaxis() -> SetTitle("Avg. Reco. Tracklets per event");

    h1D_recoTracklet_MC  -> SetMarkerColor(std::get<0>(legend_text[1]));
    h1D_recoTracklet_MC    -> SetLineColor(std::get<0>(legend_text[1]));
    h1D_recoTracklet_MC    -> SetLineWidth(2);
    leg_TruthReco -> AddEntry(h1D_recoTracklet_MC, std::get<1>(legend_text[1]).c_str(), std::get<2>(legend_text[1]).c_str());

    c1 -> cd();
    h1D_recoTracklet_data -> Draw("ep");
    h1D_recoTracklet_MC -> Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str()));

    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    leg_TruthReco -> Draw("same");

    c1 -> Print(Form("%s/h1D_RecoTrackletDataMC_Mbin%d.pdf", final_output_directory.c_str(), Mbin));
    c1 -> Clear();
    leg_TruthReco -> Clear();

}

std::pair<double, double> FinalResult::GetH1DMinMax(TH1D * h1D_in)
{
    double min = 99999;
    double max = -99999;

    for (int i = 1; i <= h1D_in -> GetNbinsX(); i++){

        if (h1D_in->GetXaxis()->GetBinCenter(i) < eta_range.first || h1D_in->GetXaxis()->GetBinCenter(i) > eta_range.second){
            continue;
        }

        double content = h1D_in -> GetBinContent(i);
        if (content < min) {min = content;}
        if (content > max) {max = content;}
    }

    return std::make_pair(min, max);
}

TGraph * FinalResult::GetRatioGr(TGraph * gr_numerator, TGraph * gr_denominator)
{
    TGraph * gr_out = new TGraph();
    
    if (gr_numerator -> GetN() != gr_denominator -> GetN()){
        std::cout<<"Error : the number of points in the numerator and denominator are not the same"<<std::endl;
        exit(1);
    }

    for (int i = 0; i < gr_numerator -> GetN(); i++){
        double x = 0;
        double y = 0;
        gr_numerator -> GetPoint(i, x, y);

        double x2 = 0;
        double y2 = 0;
        gr_denominator -> GetPoint(i, x2, y2);

        if (y2 == 0) {continue;}

        gr_out -> SetPoint(gr_out -> GetN(), x, y / y2);
    }

    gr_out -> SetMarkerStyle(gr_numerator -> GetMarkerStyle());
    gr_out -> SetMarkerSize(gr_numerator -> GetMarkerSize());
    gr_out -> SetMarkerColor(gr_numerator -> GetMarkerColor());

    return gr_out;
}

TGraphAsymmErrors * FinalResult::GetPHOBOSData()
{
    std::ifstream PHOBOS_file(Form("%s/%s",PHOBOS_data_directory.c_str(),PHOBOS_data_file_name.c_str()));
    if(!PHOBOS_file.is_open())
    {
        std::cout<<"Error: PHOBOS file not found!"<<std::endl;
        return nullptr;
    }

    TGraphAsymmErrors * PHOBOS_data = new TGraphAsymmErrors();
    PHOBOS_data -> SetTitle("PHOBOS_data;#eta;dN_{ch}/d#eta");

    std::string line;
    while (std::getline(PHOBOS_file, line)) {
        // Ignore lines that start with '#' or '&'
        if (line.empty() || line[0] == '#' || line[0] == '&') continue;

        std::istringstream iss(line);
        double eta;
        double dNdEta; 
        double err_plus; 
        double err_neg;
        
        if (iss >> eta >> dNdEta >> err_plus >> err_neg) {
            PHOBOS_data -> SetPoint(PHOBOS_data -> GetN(), eta, dNdEta);
            PHOBOS_data -> SetPointError(PHOBOS_data -> GetN()-1, 0, 0, fabs(err_neg), err_plus);

            // std::cout<<eta<<" "<<dNdEta<<" "<<err_plus<<" "<<err_neg<<std::endl;

            // eta.push_back(eta);
            // dN_dEta.push_back(dNdEta);
            // err_plus.push_back(err_plus);
            // err_minus.push_back(err_neg);
        }
    }

    PHOBOS_file.close();

    PHOBOS_data -> SetMarkerStyle(21);
    PHOBOS_data -> SetMarkerSize(1);
    PHOBOS_data -> SetMarkerColor(9);
    PHOBOS_data -> SetFillColorAlpha(9,0.5);
    PHOBOS_data -> SetLineColorAlpha(1,0);
    PHOBOS_data -> SetLineWidth(0);
    // PHOBOS_data -> GetXaxis() -> SetLimits(-2.7,2.7);


    return PHOBOS_data;

}
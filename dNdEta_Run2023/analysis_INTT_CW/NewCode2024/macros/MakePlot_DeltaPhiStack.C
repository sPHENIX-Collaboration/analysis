// #include "sPhenixStyle.C"

// int MakePlot_DeltaPhiStack()
// {
//     int Mbin = 0;

//     std::string input_file_directory = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed/vtxZ_-10_10cm_MBin%d/Folder_BaseLine/Run_0/completed", Mbin); 
//     std::string input_file_name = Form("Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ10_Mbin%d_00054280_00000_DeltaPhi.root", Mbin);
    
//     std::string target_stack_sig_name = "c1_hstack1D_DeltaPhi_Eta13";
//     std::string target_stack_bkg_name = "c1_hstack1D_DeltaPhi_Eta13";

//     std::string output_directory = input_file_directory;

//     std::pair<std::string, std::string> axes_label = {"Pair #Delta#phi", "Entries"};
    
//     std::string sPH_label = "Internal";
//     std::vector<std::tuple<double,double,std::string>> additional_text = {
//         // {0.2, 0.9, "The cluster pairs post the VtxZ linking requirement are filled"},

//         // {0.22, 0.9, "Au+Au 200 GeV"},
//         // {0.22, 0.86, "HIJING"},
//         // {0.22, 0.86, "Centrality 0-70%"},
//         // {0.22, 0.82, "|INTT vtxZ| #leq 10 cm"}

//         {0.22, 0.90, "Inner clusters rotated by #pi in #phi angle"},
//         {0.22, 0.86, "#eta: [-0.1 - 0.1]"},
//         {0.22, 0.82, Form("Centrality interval: [%s]%%", Constants::centrality_text[Mbin].c_str())},
//         {0.22, 0.78, "|INTT vtxZ| #leq 10 cm"}

//     };

//     // std::vector<std::tuple<int, std::string,std::string>> legend_text = {
//     //     {4, "Nominal", "f"},
//     //     {2, "Rotated", "f"},
//     //     {3, "Subtracted", "f"},
//     //     {41, "Pol-2 Bkg Fit", "l"}
//     // };

//     double y_max = 2000000;
    
//     SetsPhenixStyle();

//     TCanvas * c1 = new TCanvas("c1", "c1", 950, 800);

//     TLatex * ltx = new TLatex();
//     ltx->SetNDC();
//     ltx->SetTextSize(0.045);
//     ltx->SetTextAlign(31);

//     TLatex * draw_text = new TLatex();
//     draw_text -> SetNDC();
//     draw_text -> SetTextSize(0.03);

//     // TLegend * leg = new TLegend(0.22,0.68,0.42,0.77);
//     // // leg -> SetNColumns(2);
//     // leg -> SetBorderSize(0);
//     // leg -> SetTextSize(0.025);
//     // leg -> SetMargin(0.2);

//     TFile * file_in = TFile::Open(Form("%s/%s", input_file_directory.c_str(), input_file_name.c_str()));
//     THStack * hstack_sig = (THStack*)file_in->Get(target_stack_sig_name.c_str());
//     THStack * hstack_bkg = (THStack*)file_in->Get(target_stack_bkg_name.c_str());

//     c1 -> cd();
//     hstack_sig -> Draw("stack");
    

//     return 0;
// }
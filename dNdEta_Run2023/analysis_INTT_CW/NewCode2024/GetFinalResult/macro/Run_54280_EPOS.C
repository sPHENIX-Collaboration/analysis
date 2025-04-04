#include "../GetFinalResult.h"

R__LOAD_LIBRARY(../libGetFinalResult.so)

int Run_54280_EPOS(int condor_index)
{   
    std::vector<int> run_centrality_array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 70}; // note : 16 jobs in total

    int SelectedMbin = run_centrality_array[condor_index];

    int runnumber = 54280;
    // int SelectedMbin = 3; // note : 0, 1, ---- 10, 70, 100 
    std::pair<double,double> vtxZ_range = {-10,10}; // note : cm
    bool isTypeA = false;
    bool ApplyGeoAccCorr = false;
    std::pair<bool, std::pair<double,double>> cut_EtaRange = {true, {-1.5, 1.5}}; // note : not used

    std::string output_file_name_suffix = "";

    std::string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_EPOS_MDC2_ana472_20250310/Run7/EvtVtxZ/FinalResult_10cm_Pol2BkgFit_DeltaPhi0p026";

    GetFinalResult * GFR = new GetFinalResult(
        runnumber,
        SelectedMbin,
        vtxZ_range,
        isTypeA,
        ApplyGeoAccCorr,
        cut_EtaRange,

        output_file_name_suffix,

        output_directory
    );

    GFR -> SetBaseLineDeltaPhiCut({-0.026,0.026}); // todo : change here

    std::string data_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ";
    std::string MC_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_EPOS_MDC2_ana472_20250310/Run7/EvtVtxZ";

    std::string SetResultRangeFolderName = GFR -> GetSetResultRangeFolderName();

    system(Form("if [ -d %s/completed/%s ]; then rm -r %s/completed/%s; fi;", output_directory.c_str(), SetResultRangeFolderName.c_str(), output_directory.c_str(), SetResultRangeFolderName.c_str()));

    std::string baseline_subfolder = "TrackHist/baseline/completed";
    std::string baseline_subfolder_MC_001 = "TrackHist_DeltaPhi0p04_001/baseline/completed"; // note : for the first half of data
    // std::string baseline_subfolder_MC_002 = "TrackHist_002/baseline/completed"; // note : for the second half of data 
    // note : here are for preparing the reco. dNdEta
    GFR -> PrepareBaseLine(
        data_input_directory + "/" + baseline_subfolder,
        "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize40_ColMulMask_00054280_merged_merged_001.root",
        
        MC_input_directory + "/" + baseline_subfolder_MC_001,
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize40_ColMulMask_merged_001.root", // note : derive the alpha correction
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize40_ColMulMask_merged_002.root"  // note : closure test
    );


    system(Form("mv %s/%s %s/completed", output_directory.c_str(), SetResultRangeFolderName.c_str(), output_directory.c_str()));

    return 777;
}
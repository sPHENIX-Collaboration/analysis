#include "../GetFinalResult.h"

R__LOAD_LIBRARY(../libGetFinalResult.so)

int Run_54280(int condor_index)
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

    std::string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/FinalResult";

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

    std::string data_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ";
    std::string MC_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_ananew_20250131/Run4/EvtVtxZ";

    std::string SetResultRangeFolderName = GFR -> GetSetResultRangeFolderName();

    system(Form("if [ -d %s/completed/%s ]; then rm -r %s/completed/%s; fi;", output_directory.c_str(), SetResultRangeFolderName.c_str(), output_directory.c_str(), SetResultRangeFolderName.c_str()));

    std::string baseline_subfolder = "TrackHist/completed";
    // note : here are for preparing the reco. dNdEta
    GFR -> PrepareBaseLine(
        data_input_directory + "/" + baseline_subfolder,
        "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_00054280_merged.root",
        
        MC_input_directory + "/" + baseline_subfolder,
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_merged_001.root",
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_merged_002.root"
    );


    std::string runseg_subfolder = "merged_files_Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_00054280_"; 
    GFR -> PrepareRunSegment(
        data_input_directory + "/" + baseline_subfolder + "/" + runseg_subfolder,
        {
            "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_00054280_merged_001.root",
            "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_00054280_merged_002.root"
        },
        
        MC_input_directory + "/" + baseline_subfolder,
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_merged_001.root",
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_merged_002.root"
    ); // note : two times


    GFR -> PrepareDeltaPhiCut(
        {0.018, 0.024},    
        data_input_directory + "/" + baseline_subfolder,
        "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_00054280_merged.root",
        
        MC_input_directory + "/" + baseline_subfolder,
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_merged_001.root",
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize39_ColMulMask_merged_002.root"
    ); // note : two times 


    // std::string ClusAdcCut_subfolder = "TrackHist_noAdcCut/completed";
    // GFR -> PrepareClusAdcCut(
    //     data_input_directory + "/" + ClusAdcCut_subfolder,
    //     {
    //         "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc0PhiSize39_ColMulMask_00054280_merged.root"
    //     },

    //     MC_input_directory + "/" + ClusAdcCut_subfolder,
    //     "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc0PhiSize39_ColMulMask_merged_001.root",
    //     "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc0PhiSize39_ColMulMask_merged_002.root"
    // );


    std::string NoClusAdcCut_subfolder = "TrackHist_noAdcCut/completed";
    GFR -> PrepareClusAdcCut(
        0,
        data_input_directory + "/" + NoClusAdcCut_subfolder,
        "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc0PhiSize39_ColMulMask_00054280_merged.root",

        MC_input_directory + "/" + NoClusAdcCut_subfolder,
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc0PhiSize39_ColMulMask_merged_001.root",
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc0PhiSize39_ColMulMask_merged_002.root"
    ); 
    std::string ClusAdcCut50_subfolder = "TrackHist_50AdcCut/completed";
    GFR -> PrepareClusAdcCut(
        1,
        data_input_directory + "/" + ClusAdcCut50_subfolder,
        "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc50PhiSize39_ColMulMask_00054280_merged.root",

        MC_input_directory + "/" + ClusAdcCut50_subfolder,
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc50PhiSize39_ColMulMask_merged_001.root",
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc50PhiSize39_ColMulMask_merged_002.root"
    ); 

    

    std::string ClusPhiCut_subfolder = "TrackHist_noPhiCut/completed";
    GFR -> PrepareClusPhiCut(
        data_input_directory + "/" + ClusPhiCut_subfolder,
        {
            "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize350_ColMulMask_00054280_merged.root"
        },

        MC_input_directory + "/" + ClusPhiCut_subfolder,
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize350_ColMulMask_merged_001.root",
        "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize350_ColMulMask_merged_002.root"
    ); 

    system(Form("mv %s/%s %s/completed", output_directory.c_str(), SetResultRangeFolderName.c_str(), output_directory.c_str()));

    return 777;
}
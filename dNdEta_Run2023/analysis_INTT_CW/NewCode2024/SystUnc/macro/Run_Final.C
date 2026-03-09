#include "../FinalResult.h"
R__LOAD_LIBRARY(../libFinalResult.so)

int Run_Final()
{
    const string CW_folder = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih";
    const string data_folder_mother = "/seflgendata/run_54280_HR_Dec042024/completed/Run3";

    int runnumber = 54280;
    int Mbin = 70;
    std::string StandardData_directory = CW_folder + data_folder_mother + "/EvtVtxZ/TrackHist/completed/dNdEta/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed";
    std::string StandardData_file_name = "Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root";
    std::string StandardMC_directory   = CW_folder + data_folder_mother + "/EvtVtxZ/TrackHist/completed/dNdEta/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed";
    std::string StandardMC_file_name   = "MC_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00002_dNdEta.root"; // note : for generator level
    std::string sPH_label = "Internal";

    // std::string Output_directory = "Not_given";

    FinalResult * final = new FinalResult(
        runnumber,
        Mbin,
        StandardData_directory,
        StandardData_file_name,
        StandardMC_directory,
        StandardMC_file_name,
        sPH_label
    );

    final -> SetEtaRange({-1.5, 1.5});

    final -> SetCollisionStr({{0.2, 0.96}, "Au+Au #sqrt{s_{NN}} = 200 GeV"});
    final -> SetAnaDescription({{0.21, 0.9}, "Centrality [0-70]%, VtxZ [-10, 10] cm"});


    final -> SetFinal_Data_MC_text(
        {
            "Data (PHOBOS approach)",
            "HIJING (generator)"
        }
    );

    std::string output_folder_name = final -> GetOutputFileName();
    std::cout << "output_folder_name = " << output_folder_name << std::endl;

    final -> PrepareStatisticalError();

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    std::string RunSegMother_directory = CW_folder + data_folder_mother + "/EvtVtxZ/TrackHist/completed/merged_files_Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize500_ColMulMask_00054280_";
    final -> PrepareRunSegmentError(
        {
            RunSegMother_directory + "/dNdEta_001/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root",
            RunSegMother_directory + "/dNdEta_002/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root"
        }
    ); // note : run1, run2

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    std::string ClusAdcMother_directory = CW_folder + data_folder_mother + "/EvtVtxZ/TrackHist_NoClusQA/completed";
    final -> PrepareClusAdcError(
        {
            ClusAdcMother_directory + "/dNdEta/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root"   
        }
    ); 
    
    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    final -> PrepareGeoOffsetError(
        "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/GeoOffset_v1/completed/merged_result/FromdNdEta.root",
        CW_folder + data_folder_mother + "/EvtVtxZ/TrackHist/completed/dNdEta/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/MC_PreparedNdEtaEach_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00001_dNdEta.root" // note : for the alpha correction
    );

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    std::string DeltaPhiMother_directory = CW_folder + data_folder_mother + "/EvtVtxZ/TrackHist/completed";
    final -> PrepareDeltaPhiError(
        {
            DeltaPhiMother_directory + "/dNdEta_DeltaPhi0p018/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root",
            DeltaPhiMother_directory + "/dNdEta_DeltaPhi0p024/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root"
        }

    ); // note : 0.018 and 0.024

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    std::string ClusPhiSizeMother_directory = CW_folder + data_folder_mother + "/EvtVtxZ/TrackHist_PhiCut/completed";
    final -> PrepareClusPhiSizeError(
        {
            ClusPhiSizeMother_directory + "/dNdEta/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root"
        }
    );

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // std::string MCMergedMother_directory = CW_folder + data_folder_mother + "/EvtVtxZ/TrackHist/completed";
    // final -> PrepareMCMergedError(
    //     {
    //         MCMergedMother_directory + "/dNdEta_MCMergeError1/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root",
    //         MCMergedMother_directory + "/dNdEta_MCMergeError2/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root",
    //         MCMergedMother_directory + "/dNdEta_MCMergeError3/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root",
    //         MCMergedMother_directory + "/dNdEta_MCMergeError4/dNdEta_AllSensor_GeoAccCorr_VtxZ10_Mbin70/completed/Data_PreparedNdEtaEach_AlphaCorr_GeoAccCorr_AllSensor_VtxZ10_Mbin70_00054280_00000_dNdEta.root"
    //     }
    // );

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    final -> PrepareFinalError();
    final -> PrepareFinalResult();
    final -> EndRun();

    return 9;
}
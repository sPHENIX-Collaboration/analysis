#include "../../Constants.cpp"

#include "../FinalResult.h"
R__LOAD_LIBRARY(../libFinalResult.so)

int Run_Final_new(int Mbin = 70)
{
    // int Mbin = 70; 
    double Hist_Y_max = Constants::centrality_Hist_Ymax[Mbin];
    std::pair<double,double> vtxZ_range = {-10,10}; // note : cm

    int runnumber = 54280;
    std::string sPH_label = "Internal";
    std::string Collision_str = "Au+Au #sqrt{s_{NN}} = 200 GeV";

    std::string mother_folder = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/FinalResult/completed";
    std::string range_folder = Form("vtxZ_%.0f_%.0fcm_MBin%d",vtxZ_range.first, vtxZ_range.second, Mbin);

    std::string Centrality_str = Constants::centrality_text[Mbin];

    std::vector<std::tuple<double,double,std::string>> additional_text = {
        {0.22, 0.9, Collision_str},
        {0.22, 0.86, "HIJING"},
        {0.22, 0.82, Form("Centrality [%s]%%", Centrality_str.c_str())},
        {0.22, 0.78, Form("|INTT vtxZ| #leq %.0f cm", vtxZ_range.second)}
    };



    std::string StandardData_directory = mother_folder + "/" + range_folder + "/Folder_BaseLine/Run_0/completed";
    std::string StandardData_file_name = Form("Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ%.0f_Mbin%d_00054280_00000_dNdEta.root", vtxZ_range.second, Mbin);
    std::string StandardMC_directory = StandardData_directory;
    std::string StandardMC_file_name = Form("MC_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ%.0f_Mbin%d_00002_dNdEta.root", vtxZ_range.second, Mbin);

    FinalResult * final = new FinalResult(
        runnumber,
        Mbin,
        StandardData_directory,
        StandardData_file_name,
        StandardMC_directory,
        StandardMC_file_name,
        sPH_label,
        mother_folder + "/" + range_folder
    );
    final -> SetSystUncPlot_Ymax(0.2);
    final -> SetEtaRange({-1.5, 1.5});
    final -> SetCollisionStr({{0.2, 0.96}, Collision_str});
    final -> SetAnaDescription({{0.21, 0.9}, Form("Centrality [%s]%%, VtxZ [%.0f, %.0f] cm", Centrality_str.c_str(), vtxZ_range.first, vtxZ_range.second)});

    final -> SetFinal_Data_MC_text(
        {
            "Data (PHOBOS approach)",
            "HIJING (generator)"
        }
    );

    final -> PrepareBaseLineTGraph();
    final -> PrepareMCClosureTGraph();


    std::string output_folder_name = final -> GetOutputFileName();
    std::cout << "output_folder_name = " << output_folder_name << std::endl;

    final -> PrepareStatisticalError();




    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    std::string RunSegMother_directory = mother_folder + "/" + range_folder + "/Folder_RunSegment"; 
    final -> PrepareRunSegmentError(
        {
            RunSegMother_directory + Form("/Run_0/completed/%s",StandardData_file_name.c_str()),
            RunSegMother_directory + Form("/Run_1/completed/%s",StandardData_file_name.c_str())
        },
        {
            "Segment1 (First 1.5M)",
            "Segment2 (Second 1.5M)",
            "Baseline (Full statistics)"
        },
        Form("Run segment variation, Centrality [%s]%%, VtxZ [%.0f, %.0f] cm", Centrality_str.c_str(), vtxZ_range.first, vtxZ_range.second)
    ); // note : run1, run2

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    std::string ClusAdcMother_directory = mother_folder + "/" + range_folder + "/Folder_ClusAdcCut";
    final -> PrepareClusAdcError(
        {
            ClusAdcMother_directory + Form("/Run_0/completed/%s",StandardData_file_name.c_str()),
            ClusAdcMother_directory + Form("/Run_1/completed/%s",StandardData_file_name.c_str())
        },
        {
            "w.o Cluster ADC cut",
            "Cluster ADC > 50",
            "Baseline (Cluster ADC > 35)"
        },
        Form("Cluster ADC variation, Centrality [%s]%%, VtxZ [%.0f, %.0f] cm", Centrality_str.c_str(), vtxZ_range.first, vtxZ_range.second)
    ); 
    
    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    final -> PrepareGeoOffsetError(
        "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/GeoOffset_v1/completed/merged_result/FromdNdEta.root",
        StandardData_directory + "/" + Form("MC_PreparedNdEtaEach_AllSensor_VtxZ%.0f_Mbin%d_00001_dNdEta.root", vtxZ_range.second, Mbin) // note : for the alpha correction
    );

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    std::string DeltaPhiMother_directory = mother_folder + "/" + range_folder + "/Folder_DeltaPhiCut";
    final -> PrepareDeltaPhiError(
        {
            DeltaPhiMother_directory + Form("/Run_0/completed/%s",StandardData_file_name.c_str()),
            DeltaPhiMother_directory + Form("/Run_1/completed/%s",StandardData_file_name.c_str())
        },
        {
            "#Delta#phi < 0.018",
            "#Delta#phi < 0.024",
            "Baseline (#Delta#phi < 0.021)"
        },
        Form("#Delta#phi variation, Centrality [%s]%%, VtxZ [%.0f, %.0f] cm", Centrality_str.c_str(), vtxZ_range.first, vtxZ_range.second)

    ); // note : 0.018 and 0.024

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    std::string ClusPhiSizeMother_directory = mother_folder + "/" + range_folder + "/Folder_ClusPhiCut";
    final -> PrepareClusPhiSizeError(
        {
            ClusPhiSizeMother_directory + Form("/Run_0/completed/%s",StandardData_file_name.c_str())
        },
        {
            "w.o Cluster#kern[0.4]{#phi} size cut",
            "Baseline (Cluster#kern[0.4]{#phi} size < 40)" // todo; should be < or <=
        },
        Form("Cluster#kern[0.4]{#phi} size variation, Centrality [%s]%%, VtxZ [%.0f, %.0f] cm", Centrality_str.c_str(), vtxZ_range.first, vtxZ_range.second)
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
    final -> DrawAlphaCorrectionPlots(
        StandardData_directory + "/" + Form("MC_PreparedNdEtaEach_AllSensor_VtxZ%.0f_Mbin%d_00001_dNdEta.root", vtxZ_range.second, Mbin), // note : for the alpha correction
        
        additional_text,

        {
            {2, "HIJING generator level", "l"},
            {4, "Reco. Tracklets", "l"}
        }
    );

    final -> DrawGoodPair2DFineBinPlot();

    final -> DrawRecoTrackletDataMCPlot(
        additional_text,

        {
            {1, "Data", "pl"},
            {2, "HIJING", "l"}
        }
    );

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    final -> PrepareFinalError();
    final -> PrepareFinalResult(Hist_Y_max);
    final -> EndRun();

    return 888;
}
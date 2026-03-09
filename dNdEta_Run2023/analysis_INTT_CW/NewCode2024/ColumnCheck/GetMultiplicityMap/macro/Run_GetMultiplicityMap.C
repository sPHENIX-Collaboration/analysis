#include "../GetMultiplicityMap.h"

R__LOAD_LIBRARY(../libGetMultiplicityMap.so)

int Run_GetMultiplicityMap()
{
    string sub_folder_name = "noPhiCut";
    std::pair<bool, std::pair<double, double>> isClusQA = {true, {35,350}}; // note : {adc, phi size}

    // string sub_folder_name = "noAdcCut";
    // std::pair<bool, std::pair<double, double>> isClusQA = {true, {0,40}}; // note : {adc, phi size}

    // string sub_folder_name = "50AdcCut";
    // std::pair<bool, std::pair<double, double>> isClusQA = {true, {50,40}}; // note : {adc, phi size}

    // string sub_folder_name = "baseline";
    // std::pair<bool, std::pair<double, double>> isClusQA = {true, {35,40}}; // note : {adc, phi size}

    int runnumber = 54280;
    std::string data_directory = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/ColumnCheck/%s/completed", sub_folder_name.c_str());
    std::string data_file_name = Form("Data_ColumnCheck_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_00054280_merged.root", isClusQA.second.first, isClusQA.second.second);

    // std::string MC_directory = Form("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/ColumnCheck/%s/completed", sub_folder_name.c_str());
    // std::string MC_file_name = Form("MC_ColumnCheck_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_merged.root", isClusQA.second.first, isClusQA.second.second);
    // std::string output_directory = MC_directory + "/MulMap"; // note : AUTO

    std::string MC_directory = Form("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_strangeness_MDC2_ana472_20250310/Run7/EvtVtxZ/ColumnCheck/%s/completed", sub_folder_name.c_str());
    std::string MC_file_name = Form("MC_ColumnCheck_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_merged.root", isClusQA.second.first, isClusQA.second.second);
    std::string output_directory = MC_directory + "/MulMap"; // note : AUTO

    std::string output_file_name_suffix = "";

    system(Form("if [ ! -d %s ]; then mkdir -p %s/completed; fi;", output_directory.c_str(), output_directory.c_str()));

    double SetMbinFloat = 70; // note : 0-100
    std::pair<double, double> VtxZRange = {-30, 30}; // note : cm
    bool IsZClustering = false;
    bool BcoFullDiffCut = true;
    


    GetMultiplicityMap * GMM = new GetMultiplicityMap(
        runnumber,
        data_directory,
        data_file_name,
        MC_directory,
        MC_file_name,
        output_directory,

        output_file_name_suffix,

        SetMbinFloat,
        VtxZRange,
        IsZClustering,
        BcoFullDiffCut,
        isClusQA
    );

    string final_output_file_name = GMM->GetOutputFileName();
    cout<<"final_output_file_name: "<<final_output_file_name<<endl;
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

    GMM -> GetUsedZIDVec();
    GMM -> h2DNormalizedByRadius();
    GMM -> h2DNormalized();
    GMM -> DataMCDivision();
    GMM -> PrepareMulMap();
    GMM -> EndRun();

    system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

    return 0;
}
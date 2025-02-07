#include "../GetMultiplicityMap.h"

R__LOAD_LIBRARY(../libGetMultiplicityMap.so)

int Run_GetMultiplicityMap()
{
    int runnumber = 54280;
    std::string data_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/ColumnCheck_50AdcCut/completed";
    std::string data_file_name = "Data_ColumnCheck_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc50PhiSize39_00054280_merged.root";
    std::string MC_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_ananew_20250131/Run4/EvtVtxZ/ColumnCheck_50AdcCut/completed";
    std::string MC_file_name = "MC_ColumnCheck_Mbin70_VtxZ-30to30cm_ClusQAAdc50PhiSize39_merged.root";
    std::string output_directory = data_directory + "/MulMap"; // note : NOT AUTO

    std::string output_file_name_suffix = "";

    double SetMbinFloat = 70; // note : 0-100
    std::pair<double, double> VtxZRange = {-30, 30}; // note : cm
    bool IsZClustering = false;
    bool BcoFullDiffCut = true;
    std::pair<bool, std::pair<double, double>> isClusQA = {true, {50,39}}; // note : {adc, phi size}


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
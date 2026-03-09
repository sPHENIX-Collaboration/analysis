#include "../GeoAccepCorr.h"

R__LOAD_LIBRARY(../libGeoAccepCorr.so)

void Run_GeoAccepCorr()
{   
    // note : common
    int process_id = 0;
    int setMbin = 70;
    string output_file_name_suffix = "";
    std::pair<bool, std::pair<double, double>> BkgRotated_DeltaPhi_Signal_range = {false, {-0.024, 0.024}};
    string output_folder_name = "Maps";

    // Division : -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    int Data_process_id = process_id;
    int Data_run_num = 54280;
    string Data_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/GeoAcc_RandZ10cm_FineBin_GoodCol/completed";
    string Data_input_filename = "Data_TrackHist_BcoFullDiffCut_ClusQAAdc35PhiSize500_ColMulMask_RandInttZ_00054280_merged.root";
    string Data_output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/GeoAcc_RandZ10cm_FineBin_GoodCol/completed/" + output_folder_name;
    // todo : modify here
    std::string Data_output_file_name_suffix = output_file_name_suffix;
    int Data_setMbin = setMbin;

    GeoAccepCorr * Data_GAC = new GeoAccepCorr(
        Data_process_id,
        Data_run_num,
        Data_input_directory,
        Data_input_filename,
        Data_output_directory,

        Data_output_file_name_suffix,
        Data_setMbin
    );

    if (BkgRotated_DeltaPhi_Signal_range.first) {Data_GAC -> SetBkgRotated_DeltaPhi_Signal_range(BkgRotated_DeltaPhi_Signal_range.second);}

    string Data_final_output_file_name = Data_GAC->GetOutputFileName();
    cout<<"Data_final_output_file_name: "<<Data_final_output_file_name<<endl;
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", Data_output_directory.c_str(), Data_final_output_file_name.c_str(), Data_output_directory.c_str(), Data_final_output_file_name.c_str()));  

    Data_GAC -> PrepareStacks();
    Data_GAC -> PrepareMultiplicity();
    Data_GAC -> EndRun();

    system(Form("mv %s/%s %s/completed", Data_output_directory.c_str(), Data_final_output_file_name.c_str(), Data_output_directory.c_str()));

    // Division : -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    int MC_process_id = process_id;
    int MC_run_num = -1;
    string MC_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/GeoAcc_RandZ10cm_FineBin_GoodCol/completed";
    string MC_input_filename = "MC_TrackHist_ClusQAAdc35PhiSize500_ColMulMask_RandInttZ_merged.root";
    string MC_output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/GeoAcc_RandZ10cm_FineBin_GoodCol/completed/" + output_folder_name;
    // todo : modify here
    std::string MC_output_file_name_suffix = output_file_name_suffix;
    int MC_setMbin = setMbin;

    GeoAccepCorr * MC_GAC = new GeoAccepCorr(
        MC_process_id,
        MC_run_num,
        MC_input_directory,
        MC_input_filename,
        MC_output_directory,

        MC_output_file_name_suffix,
        MC_setMbin
    );

    if (BkgRotated_DeltaPhi_Signal_range.first) {MC_GAC -> SetBkgRotated_DeltaPhi_Signal_range(BkgRotated_DeltaPhi_Signal_range.second);}

    string MC_final_output_file_name = MC_GAC->GetOutputFileName();
    cout<<"MC_final_output_file_name: "<<MC_final_output_file_name<<endl;
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", MC_output_directory.c_str(), MC_final_output_file_name.c_str(), MC_output_directory.c_str(), MC_final_output_file_name.c_str()));  

    MC_GAC -> PrepareStacks();
    MC_GAC -> PrepareMultiplicity();
    MC_GAC -> EndRun();

    system(Form("mv %s/%s %s/completed", MC_output_directory.c_str(), MC_final_output_file_name.c_str(), MC_output_directory.c_str()));

    return;
}

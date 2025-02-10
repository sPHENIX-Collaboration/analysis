#include "../GeoAccepCorr.h"

R__LOAD_LIBRARY(../libGeoAccepCorr.so)

void Run_GeoAccepCorr_MCTest()
{
    int Data_process_id = 0;
    int Data_run_num = -1;
    string Data_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/RandZ_VtxZWeight/completed/merged_files_MC_TrackHist_vtxZReweight_ClusQAAdc35PhiSize500_RandInttZ_";
    string Data_input_filename = "MC_TrackHist_vtxZReweight_ClusQAAdc35PhiSize500_RandInttZ_merged_001.root";
    string Data_output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/RandZ_VtxZWeight/completed/merged_files_MC_TrackHist_vtxZReweight_ClusQAAdc35PhiSize500_RandInttZ_/Maps";
    // todo : modify here
    std::string Data_output_file_name_suffix = "";

    GeoAccepCorr * Data_GAC = new GeoAccepCorr(
        Data_process_id,
        Data_run_num,
        Data_input_directory,
        Data_input_filename,
        Data_output_directory,

        Data_output_file_name_suffix
    );

    string Data_final_output_file_name = Data_GAC->GetOutputFileName();
    cout<<"Data_final_output_file_name: "<<Data_final_output_file_name<<endl;
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", Data_output_directory.c_str(), Data_final_output_file_name.c_str(), Data_output_directory.c_str(), Data_final_output_file_name.c_str()));  

    Data_GAC -> PrepareStacks();
    Data_GAC -> PrepareMultiplicity();
    Data_GAC -> EndRun();

    system(Form("mv %s/%s %s/completed", Data_output_directory.c_str(), Data_final_output_file_name.c_str(), Data_output_directory.c_str()));

    // Division : -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    int MC_process_id = 1;
    int MC_run_num = -1;
    string MC_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/RandZ_VtxZWeight/completed/merged_files_MC_TrackHist_vtxZReweight_ClusQAAdc35PhiSize500_RandInttZ_";
    string MC_input_filename = "MC_TrackHist_vtxZReweight_ClusQAAdc35PhiSize500_RandInttZ_merged_002.root";
    string MC_output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/RandZ_VtxZWeight/completed/merged_files_MC_TrackHist_vtxZReweight_ClusQAAdc35PhiSize500_RandInttZ_/Maps";
    // todo : modify here
    std::string MC_output_file_name_suffix = "";

    GeoAccepCorr * MC_GAC = new GeoAccepCorr(
        MC_process_id,
        MC_run_num,
        MC_input_directory,
        MC_input_filename,
        MC_output_directory,

        MC_output_file_name_suffix
    );

    string MC_final_output_file_name = MC_GAC->GetOutputFileName();
    cout<<"MC_final_output_file_name: "<<MC_final_output_file_name<<endl;
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", MC_output_directory.c_str(), MC_final_output_file_name.c_str(), MC_output_directory.c_str(), MC_final_output_file_name.c_str()));  

    MC_GAC -> PrepareStacks();
    MC_GAC -> PrepareMultiplicity();
    MC_GAC -> EndRun();

    system(Form("mv %s/%s %s/completed", MC_output_directory.c_str(), MC_final_output_file_name.c_str(), MC_output_directory.c_str()));

    return;
}

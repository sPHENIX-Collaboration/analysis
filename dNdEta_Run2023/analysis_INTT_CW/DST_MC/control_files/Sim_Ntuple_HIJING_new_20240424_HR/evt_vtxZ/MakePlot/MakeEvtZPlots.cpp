#include "../../../../plot_evt_zvtx_condor/plot_evt_zvtx.cpp"

// todo: you may want to check the ANA_MAP_v1.h in "plot_evt_zvtx.cpp/plot_evt_zvtx.cpp"

int MakeEvtZPlots()
{
    double zvtx_range_l = -500;
    double zvtx_range_r = 100;
    double zvtx_range_zoomin_l = -300;
    double zvtx_range_zoomin_r = -100;

    string data_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR/evt_vtxZ/complete_file";
    string data_input_file_name = "merged_file.root";
    string data_out_folder_directory = data_input_directory + "/merged_result";
    double data_required_zvtx_diff = 10.; // note : unit : mm
    int data_zvtx_dist_NClus_cut = 1000;
    int data_run_type = 1; // note : 1 = MC, 0 = data

    plot_evt_zvtx * MC_f4a_2024_05_07 = new plot_evt_zvtx(
        data_input_directory,
        data_input_file_name,
        data_out_folder_directory,
        data_required_zvtx_diff,
        data_zvtx_dist_NClus_cut,
        data_run_type,
        zvtx_range_l,
        zvtx_range_r,
        zvtx_range_zoomin_l,
        zvtx_range_zoomin_r
    );
    
    return 0;
}
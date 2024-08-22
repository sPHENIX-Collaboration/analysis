#include "../../../../../plot_evt_zvtx_condor/plot_evt_zvtx.cpp"

int MakeEvtZPlots()
{
    double zvtx_range_l = -500;
    double zvtx_range_r = 100;
    double zvtx_range_zoomin_l = -300;
    double zvtx_range_zoomin_r = -100;

    string data_input_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/evt_vtxZ/complete_file";
    string data_input_file_name = "merged_file.root";
    string data_out_folder_directory = data_input_directory + "/merged_result";
    double data_required_zvtx_diff = 10.; // note : unit : mm
    int data_zvtx_dist_NClus_cut = 1000;
    int data_run_type = 0; // note : 1 = MC, 0 = data

    plot_evt_zvtx * data_f4a_20869 = new plot_evt_zvtx(
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
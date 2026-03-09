#include "../../../../plot_evt_zvtx_condor/plot_evt_zvtx.cpp"
#include "../../../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

// todo: you may want to check the ANA_MAP_v1.h in "plot_evt_zvtx.cpp/plot_evt_zvtx.cpp"

int MakeEvtZPlots(string input_sub_folder_name = "evt_vtxZ")
{
    double zvtx_range_l        = ana_map_version::evt_vtxZ_zvtx_range_l;
    double zvtx_range_r        = ana_map_version::evt_vtxZ_zvtx_range_r;
    double zvtx_range_zoomin_l = ana_map_version::evt_vtxZ_zvtx_range_zoomin_l;
    double zvtx_range_zoomin_r = ana_map_version::evt_vtxZ_zvtx_range_zoomin_r;

    string data_input_directory = ana_map_version::MC_input_directory + "/" + input_sub_folder_name + "/complete_file";
    string data_input_file_name = "merged_file.root";
    string data_out_folder_directory = data_input_directory + "/merged_result";
    double data_required_zvtx_diff = ana_map_version::evt_vtxZ_data_required_zvtx_diff;
    int data_zvtx_dist_NClus_cut   = ana_map_version::evt_vtxZ_data_zvtx_dist_NClus_cut;
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
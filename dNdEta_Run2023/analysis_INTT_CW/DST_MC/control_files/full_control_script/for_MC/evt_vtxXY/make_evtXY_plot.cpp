#include <iostream>

#include "../../../../evtXY_data_check/CheckEvtXY.cpp"
#include "../../../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

int make_evtXY_plot(string evt_vtxXY_name = "evt_vtxXY")
{

    string mother_folder_directory = ana_map_version::MC_input_directory + "/" + evt_vtxXY_name + "/complete_file";
    string input_file_list = "file_list.txt";
    string out_folder_directory = mother_folder_directory + "/file_merged_folder";
    std::pair<double, double> beam_origin = ana_map_version::MC_beam_origin;
    int NClus_cut_label = ana_map_version::evt_vtxXY_N_clu_cutl;
    int unit_tag = 1;
    string run_type = "MC";
    CheckEvtXY * file_in = new CheckEvtXY(mother_folder_directory, input_file_list, out_folder_directory, beam_origin, NClus_cut_label, unit_tag, run_type);
    file_in -> Prepare_info();
    file_in -> Print_plots();

    return 0;
}
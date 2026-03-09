#include <iostream>

#include "../../../evtXY_data_check/CheckEvtXY.h"
// #include "../../../ana_map_folder/ana_map_v1.h"

int main()
{

    string mother_folder_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR/evt_vtxXY/complete_file";
    string input_file_list = "file_list.txt";
    string out_folder_directory = mother_folder_directory + "/file_merged_folder";
    double cm = 10.;
    std::pair<double, double> beam_origin = {-0.0399914 * cm, 0.240108 * cm};
    int NClus_cut_label = 3000;
    int unit_tag = 1;
    string run_type = "MC";
    CheckEvtXY * file_in = new CheckEvtXY(mother_folder_directory, input_file_list, out_folder_directory, beam_origin, NClus_cut_label, unit_tag, run_type);
    file_in -> Prepare_info();
    file_in -> Print_plots();

    return 0;
}
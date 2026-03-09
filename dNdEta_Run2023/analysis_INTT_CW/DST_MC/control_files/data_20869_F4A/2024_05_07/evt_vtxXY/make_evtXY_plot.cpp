#include <iostream>

#include "../../../../evtXY_data_check/CheckEvtXY.h"
// #include "../../../../ana_map_folder/ana_map_v1.h"

int main()
{

    string mother_folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/evt_vtxXY/complete_file";
    string input_file_list = "file_list.txt";
    string out_folder_directory = mother_folder_directory + "/file_merged_folder";
    // std::pair<double, double> beam_origin = ANA_MAP_V3::beam_origin;
    double cm = 10.;
    std::pair<double, double> beam_origin = {-0.0206744 * cm, 0.279965 * cm}; // note : width 4, iteration 8 for the quadrant method.
    int NClus_cut_label = 3000;
    int unit_tag = 1;
    string run_type = "data";
    CheckEvtXY * file_in = new CheckEvtXY(mother_folder_directory, input_file_list, out_folder_directory, beam_origin, NClus_cut_label, unit_tag, run_type);
    file_in -> Prepare_info();
    file_in -> Print_plots();

    return 0;
}
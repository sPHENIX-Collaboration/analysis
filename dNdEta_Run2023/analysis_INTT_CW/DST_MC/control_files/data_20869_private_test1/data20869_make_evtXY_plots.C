#include <iostream>

#include "../../evtXY_data_check/CheckEvtXY.h"

int main()
{

    string mother_folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/condor_evtXY_folder";
    string input_file_list = "file_list.txt";
    string out_folder_directory = mother_folder_directory + "/file_merged_folder";
    std::pair<double, double> beam_origin = {-0.23436750, 2.5985125};
    CheckEvtXY * file_in = new CheckEvtXY(mother_folder_directory, input_file_list, out_folder_directory, beam_origin);
    file_in -> Prepare_info();
    file_in -> Print_plots();

    return 0;
}
#include "hitmap.h"
#include <iostream>

using namespace std;

// R__LOAD_LIBRARY(/sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/DST_MC/hitmap/libhitmap_v0.so)

int main()
{
    string input_file_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/beam_inttall-00020869-0000_event_base_ana.root";
    string input_tree_name = "input_tree_name";
    string output_directory = "output_directory";
    HITMAP * aaa = new HITMAP(input_file_directory, input_tree_name, output_directory);

    return 0;
}
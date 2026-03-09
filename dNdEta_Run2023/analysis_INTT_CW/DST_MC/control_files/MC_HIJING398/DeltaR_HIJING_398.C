#include <iostream>
#include "../../../sPhenixStyle.C"
#include "../../DeltaR_check/CheckDeltaR.h"
R__LOAD_LIBRARY(../../DeltaR_check/libDeltaRV1_0.so)

int DeltaR_HIJING_398()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/DeltaR_TrackCounting_RecoZ_noMegaTrackRemoval_35k_withTrueZ";
    string input_file_name = "INTT_final_hist_info.root";
    string input_tree_name = "tree_eta";
    string out_folder_directory = input_directory + "/DeltaR_check";
    string run_type = "MC";

    SetsPhenixStyle();

    CheckDeltaR * check_deltaR = new CheckDeltaR(run_type, input_directory, input_file_name, input_tree_name, out_folder_directory);
    check_deltaR -> Processing(10000);
    check_deltaR -> PrintPlots();

    return 0;
}

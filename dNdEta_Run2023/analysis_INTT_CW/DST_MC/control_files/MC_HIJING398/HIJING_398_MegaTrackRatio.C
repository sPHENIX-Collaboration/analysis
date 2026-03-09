#include "../../MegaTrackRatio/MegaTrackRatio.C"

// #include "../../MegaTrackRatio/MegaTrackRatio.h"
// R__LOAD_LIBRARY(../../MegaTrackRatio/MegaTrackRatio_C.so)

void HIJING_398_MegaTrackRatio()
{
    // gSystem->Load("../../MegaTrackRatio/MegaTrackRatio_C.so");

    string run_type = "MC";
    string input1_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/New_TrackCounting_RecoZ_withMegaTrackRemoval_35k";
    string input2_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/New_TrackCounting_RecoZ_noMegaTrackRemoval_35k";
    string input1_name = "INTT_final_hist_info.root";
    string input2_name = "INTT_final_hist_info.root";
    string tree1_name = "tree_eta";
    string tree2_name = "tree_eta";

    // MegaTrackRatio * mega_track_ratio = new MegaTrackRatio();

    MegaTrackRatio * mega_track_ratio = new MegaTrackRatio(run_type, input1_directory, input1_name, tree1_name, input2_directory, input2_name, tree2_name);
    mega_track_ratio -> ProcessEvent();
    mega_track_ratio -> PrintPlot();
}
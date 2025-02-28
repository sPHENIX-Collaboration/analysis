#include "../../INTTReadTree.h"
#include "../../INTTZvtx.h"

void data20869_z()
{
    string input_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869";
    string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut";
    string out_folder_directory = input_directory + "/" + "folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut_advanced_correlation_study";
    string tree_name = "tree_clu";
    
    pair<double, double> beam_origin = {-0.457 + 0.0276, 2.657 - 0.2814}; // note : for run20869
    pair<double, double> DCA_cut ={0.277 - 0.730, 0.277 + 0.730}; // note : for run20869
    pair<double, double> zvtx_QA_width = {40, 70}; 
    double zvtx_QA_ratio = 0.00006;
    double phi_diff_cut = 0.265 + 0.269; // note : for run20869
    int clu_sum_adc_cut = 31;
    int clu_size_cut = 4;
    int N_clu_cut = 10000;
    int N_clu_cutl = 20;
    
    bool draw_event_display = true;
    double peek = 3.324;
    int data_type = 2; // note : private gen data cluster
    int geo_mode_id = 1; // note : 0 -> perfect geo., 1 -> first-order survey geo.
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTZvtx     * MCz     = new INTTZvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, zvtx_cal_require, zvtx_QA_width, zvtx_QA_ratio, draw_event_display, peek);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    for (int event_i = 0; event_i < 50000; event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        MCz     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), INTTClu -> GetBCOFull(), 5  // note : no bco_full for MC
        );        

        MCz -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCz -> PrintPlots();
    MCz -> EndRun();
}

        // cout<<"test, INTTClu -> GetBCOFull()"<<INTTClu -> GetBCOFull()<<endl;

        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;
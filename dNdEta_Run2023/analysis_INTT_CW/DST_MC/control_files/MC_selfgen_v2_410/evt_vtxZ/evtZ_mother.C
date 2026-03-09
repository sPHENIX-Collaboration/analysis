#include "../../../INTTReadTree.h"
#include "../../../INTTZvtx.h"

void evtZ_mother(int index)
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/self_gen_singleMu_v2/complete_file";
    string file_name = "out_G4_ntuple_" + to_string(index);
    string out_folder_mother_directory = input_directory + "/evt_vtxZ";
    string out_folder_directory = out_folder_mother_directory + Form("/evtZ_%d", index);
    string tree_name = "tree";
    
    pair<double, double> beam_origin = {-0.400098, -2.39742}; // note : for MC selfgen v2 410
    pair<double, double> DCA_cut ={-1., 1.}; // note : for run20869
    pair<double, double> zvtx_QA_width = {40, 70}; 
    double zvtx_QA_ratio = 0.00006;
    double phi_diff_cut = 0.5; // note : for run20869
    int clu_sum_adc_cut = -1;
    int clu_size_cut = 4;
    int N_clu_cut = 10000;
    int N_clu_cutl = 20;
    
    bool draw_event_display = true;
    double peek = 3.324;
    int data_type = 6; // note : private gen data cluster
    int geo_mode_id = 1; // note : 0 -> perfect geo., 1 -> first-order survey geo.
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTZvtx     * MCz     = new INTTZvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, zvtx_cal_require, zvtx_QA_width, zvtx_QA_ratio, draw_event_display, peek);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    // int start_evt = index * 2000;
    // int end_evt   = (index + 1) * 2000;

    // if (start_evt > INTTClu -> GetNEvt()) { return; }
    // if (end_evt   > INTTClu -> GetNEvt()) { end_evt = INTTClu -> GetNEvt();}

    for (int event_i = 0; event_i < INTTClu -> GetNEvt(); event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        MCz     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), INTTClu -> GetBCOFull(), INTTClu->GetCentralityBin()  // note : no bco_full for MC
        );        

        MCz -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCz -> PrintPlots();
    MCz -> EndRun();

    system(Form("mv %s %s", out_folder_directory.c_str(), (out_folder_mother_directory + "/complete_file").c_str()));
    cout<<"core_"<<index<<" finished"<<endl;
}



        // cout<<"test, INTTClu -> GetBCOFull()"<<INTTClu -> GetBCOFull()<<endl;

        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;
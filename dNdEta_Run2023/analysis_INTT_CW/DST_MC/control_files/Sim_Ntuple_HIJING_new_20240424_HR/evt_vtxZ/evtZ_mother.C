#include "../../../INTTReadTree.h"
#include "../../../INTTZvtx.h"

void evtZ_mother(int index)
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR";
    string file_name_index = to_string(index); file_name_index = string(5 - file_name_index.length(), '0') + file_name_index;
    string file_name = "ntuple_" + file_name_index;
    string out_folder_mother_directory = input_directory + "/evt_vtxZ";
    string out_folder_directory = out_folder_mother_directory + Form("/evtZ_%s", file_name_index.c_str());
    string tree_name = "EventTree";
    double cm = 10.;
    pair<double, double> beam_origin = {-0.0399914 * cm, 0.240108 * cm}; // note : for for MC provided by HR 2024_05_07
    pair<double, double> DCA_cut ={-1., 1.}; // note : for run20869
    pair<double, double> zvtx_QA_width = {40, 70}; 
    double zvtx_QA_ratio = 0.00006;
    double phi_diff_cut = 0.5; // note : for run20869
    int clu_sum_adc_cut = 35;
    int clu_size_cut = 5;
    int N_clu_cut = 10000;
    int N_clu_cutl = 20;
    
    bool draw_event_display = true;
    double peek = 3.324;
    int data_type = 0; // note : MC
    int geo_mode_id = 1; // note : 0 -> perfect geo., 1 -> first-order survey geo. // note : used 
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTZvtx     * MCz     = new INTTZvtx(
        INTTClu -> GetRunType(), 
        out_folder_directory, 
        beam_origin, 
        geo_mode_id, 
        phi_diff_cut, 
        DCA_cut, 
        N_clu_cutl, 
        N_clu_cut, 
        zvtx_cal_require, 
        zvtx_QA_width, 
        zvtx_QA_ratio, 
        draw_event_display, 
        peek
    );

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    // int start_evt = index * 2000;
    // int end_evt   = (index + 1) * 2000;

    // if (start_evt > INTTClu -> GetNEvt()) { return; }
    // if (end_evt   > INTTClu -> GetNEvt()) { end_evt = INTTClu -> GetNEvt();}

    // note : here we don't have to apply the is_min_bias cut as it we can just do it afterward
    for (int event_i = 0; event_i < INTTClu -> GetNEvt(); event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        MCz     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, 
            INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), 
            INTTClu -> GetTrigZvtxMC(), 
            INTTClu -> GetPhiCheckTag(), 
            INTTClu -> GetBCOFull(), 
            INTTClu -> GetCentralityBin(),  // note : no bco_full for MC
            INTTClu -> GetMBDRecoZ(),
            INTTClu -> GetIsMinBias(),
            INTTClu -> GetIsMinBiasWoZDC(),
            INTTClu -> GetMBDNorthChargeSum(),
            INTTClu -> GetMBDSouthChargeSum()
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
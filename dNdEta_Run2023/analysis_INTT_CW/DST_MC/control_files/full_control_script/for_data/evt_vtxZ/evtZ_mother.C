#include "../../../../INTTReadTree.h"
#include "../../../../INTTZvtx.h"
#include "../../../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

void evtZ_mother(int index, string output_sub_folder_name = "evt_vtxZ")
{
    string input_directory           = ana_map_version::data_input_directory;
    string file_name                 = ana_map_version::data_file_name;
    pair<double, double> beam_origin = ana_map_version::data_beam_origin;
    string tree_name                 = ana_map_version::data_tree_name;

    string file_name_index = to_string(index); file_name_index = string(5 - file_name_index.length(), '0') + file_name_index;
    string out_folder_mother_directory = input_directory + "/" + output_sub_folder_name;
    string out_folder_directory = out_folder_mother_directory + Form("/evtZ_%s", file_name_index.c_str());
    
    double phi_diff_cut                = ana_map_version::evt_vtxZ_phi_diff_cut;
    pair<double, double> DCA_cut       = ana_map_version::evt_vtxZ_DCA_cut;
    pair<double, double> zvtx_QA_width = ana_map_version::evt_vtxZ_zvtx_QA_width; 
    double zvtx_QA_ratio               = ana_map_version::evt_vtxZ_zvtx_QA_ratio;
    int N_clu_cut                      = ana_map_version::evt_vtxZ_N_clu_cut;
    int N_clu_cutl                     = ana_map_version::evt_vtxZ_N_clu_cutl;
    int clu_sum_adc_cut                = ana_map_version::clu_sum_adc_cut;
    int clu_size_cut                   = ana_map_version::clu_size_cut;

    
    bool draw_event_display = true;
    double peek = 3.324;
    int data_type = ana_map_version::data_data_type; // note : data F4A
    int geo_mode_id = 1; // note : 0 -> perfect geo., 1 -> first-order survey geo.
    int zvtx_cal_require = ana_map_version::evt_vtxZ_zvtx_cal_require;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    int Nevt_per_core = ana_map_version::evt_vtxZ_data_Nevt_per_core;

    int start_evt = index * Nevt_per_core;
    int end_evt   = (index + 1) * Nevt_per_core;
    if (start_evt >= INTTClu -> GetNEvt()) { return; }
    if (fabs(start_evt - INTTClu -> GetNEvt()) < Nevt_per_core) { return; } // note : because the previous will be handling the remaining events
    if (fabs(end_evt - INTTClu -> GetNEvt()) <= Nevt_per_core) { end_evt = INTTClu -> GetNEvt();}

    INTTZvtx * MCz = new INTTZvtx(
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

    for (int event_i = start_evt; event_i < end_evt; event_i ++)
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
            INTTClu -> GetCentralityBin(),
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
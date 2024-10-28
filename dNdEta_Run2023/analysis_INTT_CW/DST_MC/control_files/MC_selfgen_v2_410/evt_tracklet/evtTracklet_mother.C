#include "../../../INTTReadTree.h"
#include "../../../INTTEta.h"
#include "../../../ReadINTTZ/ReadINTTZ_v2.C"
#include "../../../ana_map_folder/ana_map_v1.h"

void evtTracklet_mother(int core_i)
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/self_gen_singleMu_v2/complete_file";
    string file_name = "out_G4_ntuple_" + to_string(core_i);
    string tree_name = "tree";
    
    // string file_name = "MC_ZF_RECO_zvtx";
    string out_folder_mother_directory = input_directory + "/evt_tracklet";
    string out_folder_directory = out_folder_mother_directory + "/core_" + to_string(core_i);
    // string out_folder_directory = input_directory + "/BKGTest_CluMultiUsed_majorZ_TrackEta_" + file_name;
    // string out_folder_directory = input_directory + "/MegaTrackFinding_ratio";
    
    string input_recoZ_directory = input_directory + "/evt_vtxZ/complete_file/evtZ_"+to_string(core_i)+"/INTT_zvtx.root";
    TFile * file_in = TFile::Open(Form("%s",input_recoZ_directory.c_str()));
    TTree * tree_in = (TTree *)file_in->Get("tree_Z");
    ReadINTTZ_v2 * read_recoZ = new ReadINTTZ_v2(tree_in);
    cout<<"reco ZVTX tree entry : "<<tree_in -> GetEntries()<<endl;

    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    // pair<double, double> beam_origin = {-0.4, 2.4};
    pair<double, double> beam_origin = {-0.400098, -2.39742}; // note : for MC selfgen v2 410
    pair<double, double> DCA_cut = {-3, 3}; // {-1, 1};
    pair<double, double> zvtx_QA_width = {35, 70}; 
    double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 3; // 0.3;
    int clu_sum_adc_cut = -1;
    int clu_size_cut = 8;
    int N_clu_cut = 10000;
    int N_clu_cutl = 20;
    
    bool draw_event_display = false;
    double peek = 3.324;
    // int data_type = 0; // note : MC
    int data_type = 6; // note : MC, rotate the cluster of the inner barrel by 180 degrees
    int geo_mode_id = 1; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    // todo : if the centrality bin or the eta and z regions are changed, modify the "ana_map_v1.h" first,
    // todo : and change the following "namespace" name
    map<int, double> reco_Z_resolution = ANA_MAP_V2::reco_Z_resolution;
    map<int,int> centrality_map = ANA_MAP_V2::centrality_map;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    
    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    system(Form("mkdir %s", out_folder_directory.c_str()));
    INTTEta     * MCEta     = new INTTEta(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, 0, 3, false);

    for (int event_i = 0; event_i < INTTClu -> GetNEvt(); event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        read_recoZ->LoadTree(event_i);
        read_recoZ->GetEntry(event_i);
        
        if (read_recoZ -> nclu_inner < 0)     {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        if (read_recoZ -> nclu_outer < 0)     {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        if (read_recoZ -> good_zvtx_tag != 1) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        MCEta     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigvtxMC(), INTTClu -> GetPhiCheckTag(), -1, {read_recoZ -> LB_Gaus_Mean_mean, reco_Z_resolution[centrality_map[INTTClu->GetCentralityBin()]]}, INTTClu->GetCentralityBin(), INTTClu->GetTrueTrackInfo() // note : no bco_full for MC
        );

        cout<<"Z check, recoZ: "<<read_recoZ -> LB_Gaus_Mean_mean<<" TrueZ: "<<INTTClu -> GetTrigvtxMC()[2]*10<<" diff: "<< read_recoZ -> LB_Gaus_Mean_mean - INTTClu -> GetTrigvtxMC()[2]*10<<" MBin: "<<INTTClu->GetCentralityBin()<<" recoZ resolution: "<<reco_Z_resolution[centrality_map[INTTClu->GetCentralityBin()]]<<endl;

        MCEta -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCEta -> PrintPlots();
    MCEta -> EndRun();

    system(Form("mv %s %s", out_folder_directory.c_str(), (out_folder_mother_directory + "/complete_file").c_str()));
    cout<<"core_"<<core_i<<" finished"<<endl;

}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;
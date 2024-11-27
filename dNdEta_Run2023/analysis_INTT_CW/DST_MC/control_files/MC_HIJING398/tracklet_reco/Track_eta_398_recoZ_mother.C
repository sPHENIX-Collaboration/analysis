#include "../../../INTTReadTree.h"
#include "../../../INTTEta.h"
#include "../../../ReadINTTZ/ReadINTTZ_v2.C"
#include "../../../ana_map_folder/ana_map_v1.h"

void Track_eta_398_recoZ_mother(int core_i)
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams";
    // string file_name = "MC_ZF_RECO_zvtx";
    string out_folder_directory = input_directory + "/TrackletReco_condor_Phi180Rotate/core_" + to_string(core_i);
    // string out_folder_directory = input_directory + "/BKGTest_CluMultiUsed_majorZ_TrackEta_" + file_name;
    // string out_folder_directory = input_directory + "/MegaTrackFinding_ratio";
    string MC_list_name = "file_list.txt";
    string tree_name = "EventTree";

    string input_recoZ_directory = input_directory + "/SemiFinal_EvtZ_MC_ZF_zvtx/INTT_zvtx.root";
    TFile * file_in = TFile::Open(Form("%s",input_recoZ_directory.c_str()));
    TTree * tree_in = (TTree *)file_in->Get("tree_Z");
    ReadINTTZ_v2 * read_recoZ = new ReadINTTZ_v2(tree_in);
    cout<<"reco ZVTX tree entry : "<<tree_in -> GetEntries()<<endl;


    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    // pair<double, double> beam_origin = {-0.4, 2.4};
    pair<double, double> beam_origin = {-0.394532, 2.40234}; // note : line fill + quadrant method average
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
    int data_type = 5; // note : MC, rotate the cluster of the inner barrel by 180 degrees
    int geo_mode_id = 0; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    // todo : if the centrality bin or the eta and z regions are changed, modify the "ana_map_v1.h" first,
    // todo : and change the following "namespace" name
    map<int, double> reco_Z_resolution = ANA_MAP_V1::reco_Z_resolution;
    map<int,int> centrality_map = ANA_MAP_V1::centrality_map;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    int start_evt = core_i * 2000;
    int end_evt   = (core_i + 1) * 2000;

    cout<<"1 core id : "<<core_i<<" exp start_evt : "<<start_evt<<" exp end_evt : "<<end_evt<<endl;

    if (start_evt >= INTTClu -> GetNEvt()) { return; }
    if (end_evt   >  INTTClu -> GetNEvt()) { end_evt = INTTClu -> GetNEvt();}

    cout<<"2 core id : "<<core_i<<" start_evt : "<<start_evt<<" end_evt : "<<end_evt<<endl;

    system(Form("mkdir %s", out_folder_directory.c_str()));
    INTTEta     * MCEta     = new INTTEta(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, 0, 3, false);

    for (int event_i = start_evt; event_i < end_evt; event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        read_recoZ->LoadTree(event_i);
        read_recoZ->GetEntry(event_i);
        
        if (read_recoZ -> nclu_inner == -1)   {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        if (read_recoZ -> nclu_outer == -1)   {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}
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
}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;
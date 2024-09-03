#include "../../../INTTReadTree.h"
#include "../../../INTTXYvtxEvt.h"
#include "../../../ReadINTTZ/ReadINTTZCombine.C"
#include "../../../ana_map_folder/ana_map_v1.h"

namespace ana_map_version = ANA_MAP_V3;

// todo : check used namespace

void evtXY_mother(int index)
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR";
    string file_name = "ntuple_merged";
    string tree_name = "EventTree";

    string input_directory_zvtx = input_directory + "/evt_vtxZ/complete_file";
    string file_name_zvtx = "merged_file.root";

    string file_name_index = to_string(index); file_name_index = string(5 - file_name_index.length(), '0') + file_name_index;
    string out_folder_mother_directory = input_directory + "/evt_vtxXY";
    string out_folder_directory = out_folder_mother_directory + Form("/evtXY_%s", file_name_index.c_str());
    double cm = 10.;
    pair<double, double> beam_origin = {-0.0399914 * cm, 0.240108 * cm}; // note : for for MC provided by HR 2024_05_07
    int clu_sum_adc_cut = ana_map_version::clu_sum_adc_cut;
    int clu_size_cut = ana_map_version::clu_size_cut;
    int N_clu_cut  = 10000;
    int N_clu_cutl = 3000;
    int data_type = 0;   // note : MC
    int geo_mode_id = 1; // note : 0 -> perfect geo., 1 -> first-order survey geo.
    bool draw_event_display = true;
    double peek = 3.324;
    
    // note : not used
    pair<double, double> DCA_cut ={-1., 1.}; 
    pair<double, double> zvtx_QA_width = {40, 70}; 
    double zvtx_QA_ratio = 0.00006;
    double phi_diff_cut = 0.5; 
    int zvtx_cal_require = 15;



    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut);

    int Nevt_per_core = 3000;

    int start_evt = index * Nevt_per_core;
    int end_evt   = (index + 1) * Nevt_per_core;
    if (start_evt >= INTTClu -> GetNEvt()) { return; }
    if (fabs(start_evt - INTTClu -> GetNEvt()) < Nevt_per_core) { return; } // note : because the previous will be handling the remaining events
    if (fabs(end_evt - INTTClu -> GetNEvt()) <= Nevt_per_core) { end_evt = INTTClu -> GetNEvt();}

    INTTXYvtxEvt * MCxy = new INTTXYvtxEvt(
        INTTClu -> GetRunType(), // string run_type, 
        out_folder_directory,    // string out_folder_directory, 
        beam_origin,             // pair<double,double> beam_origin, 
        geo_mode_id,             // int geo_mode_id,                       // note: not used 
        phi_diff_cut,            // double phi_diff_cut = 0.11,            // note : not used
        DCA_cut,                 // pair<double, double> DCA_cut = {-1,1}, // note : not used
        N_clu_cutl,              // int N_clu_cutl = 20, 
        N_clu_cut,               // int N_clu_cut = 10000, 
        draw_event_display,      // bool draw_event_display = true, 
        peek,                    // double peek = 3.32405, 
        0,                       // double angle_diff_new_l = 0.0,   // note : not used
        3,                       // double angle_diff_new_r = 3,     // note : not used
        false,                   // bool print_message_opt = true, 
        4,                       // double window_size = 2.5, // note : not used
        10                       // int quadrant_trial = 9    // note : not used
    );

    TFile * File_z = TFile::Open(Form("%s/%s", input_directory_zvtx.c_str(), file_name_zvtx.c_str()));
    TTree * Tree_z = (TTree*) File_z -> Get("tree_Z");
    ReadINTTZCombine * RecoZ = new ReadINTTZCombine(Tree_z);

    cout<<"Total event of INTTClu : "<<INTTClu -> GetNEvt()<<endl;
    cout<<"Total event of INTT Reco Z : "<<Tree_z -> GetEntries()<<endl;

    for (int event_i = start_evt; event_i < end_evt; event_i ++)
    {
        RecoZ -> LoadTree(event_i);
        RecoZ -> GetEntry(event_i);

        // note : MC, but the event is not the minimum bias event
        if (INTTClu -> GetRunType() == "MC" && RecoZ -> is_min_bias_wozdc == 0) {MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        // todo: here I use is_min_bias_wozdc for data
        // note : data, but the event is not the minomum bias event (wozdc)
        else { 
            if ( RecoZ -> is_min_bias_wozdc == 0) {MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        }

        // note : to get rid of the nan value
        if (RecoZ -> Centrality_float != RecoZ -> Centrality_float) {MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        if (RecoZ -> MBD_reco_z       != RecoZ -> MBD_reco_z)       {MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        if (RecoZ -> nclu_inner <= 0)   { MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue; }
        if (RecoZ -> nclu_outer <= 0)   { MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue; }
        if ((RecoZ -> nclu_inner + RecoZ -> nclu_outer) < N_clu_cutl) { MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue; }

        // todo : checked the used namespace
        double MBD_charge_assy = (RecoZ -> MBD_north_charge_sum - RecoZ -> MBD_south_charge_sum) / (RecoZ -> MBD_north_charge_sum + RecoZ -> MBD_south_charge_sum);
        if ( MBD_charge_assy < -1 * ana_map_version::MBD_assy_ratio_cut || MBD_charge_assy >  ana_map_version::MBD_assy_ratio_cut) {MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        if (RecoZ -> LB_Gaus_Width_width < ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_l || RecoZ -> LB_Gaus_Width_width > ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_r) {MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        if (RecoZ -> LB_cut_peak_width <   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_l || RecoZ -> LB_cut_peak_width >   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_r) {MCxy -> ClearEvt(); INTTClu -> EvtClear(); continue;}        

        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        if (RecoZ -> nclu_inner != INTTClu -> temp_sPH_inner_nocolumn_vec.size() || RecoZ -> nclu_outer != INTTClu -> temp_sPH_outer_nocolumn_vec.size()) 
        {
            cout<<"wrong matching found !!!, kill the job"<<endl;
            exit(1);
        }

        cout<<"event ID: "<<event_i<<" recoZ: "<<RecoZ -> LB_Gaus_Mean_mean<<" mm, TrueZ: "<<RecoZ -> MC_true_zvtx<<" mm, diff: "<<RecoZ -> LB_Gaus_Mean_mean - RecoZ -> MC_true_zvtx<<" mm "<<endl;

        int Centrality_bin = ana_map_version::convert_centrality_bin(RecoZ -> Centrality_float);

        MCxy     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, 
            INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), 
            INTTClu -> GetTrigvtxMC(), 
            INTTClu -> GetPhiCheckTag(), 
            INTTClu -> GetBCOFull(), 
            // {RecoZ -> LB_Gaus_Mean_mean, RecoZ -> LB_Gaus_Mean_meanE}
            {RecoZ -> LB_Gaus_Mean_mean, ana_map_version::reco_Z_resolution[Centrality_bin]}
            // todo : be careful, the centrality_bin now is more than what the reco_Z_resolution is expecting, the only way it can work is because of the high multiplicity cut applied above
        );

        MCxy -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCxy -> PrintPlots_Evt();
    MCxy -> EndRun();

    system(Form("mv %s %s", out_folder_directory.c_str(), (out_folder_mother_directory + "/complete_file").c_str()));
    cout<<"core_"<<index<<" finished"<<endl;

    return;
}
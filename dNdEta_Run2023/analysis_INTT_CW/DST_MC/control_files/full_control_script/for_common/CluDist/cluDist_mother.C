#include "../../../../INTTReadTree.h"
#include "../../../../INTTEta.h"
#include "../../../../ReadINTTZ/ReadINTTZCombine.C"
#include "../../../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

void cluDist_mother(string data_type_str, int index, string output_sub_folder_name = "evt_tracklet", string used_zvtx_folder_name = "evt_vtxZ")
{
    string input_directory = (data_type_str == "MC") ?  ana_map_version::MC_input_directory : ana_map_version::data_input_directory;
    string file_name       = (data_type_str == "MC") ?  ana_map_version::MC_file_name       : ana_map_version::data_file_name;
    string tree_name       = (data_type_str == "MC") ?  ana_map_version::MC_tree_name       : ana_map_version::data_tree_name;

    string input_directory_zvtx = input_directory + "/" + used_zvtx_folder_name + "/complete_file";
    string file_name_zvtx = "merged_file.root";

    string file_name_index = to_string(index); file_name_index = string(5 - file_name_index.length(), '0') + file_name_index;
    string out_folder_mother_directory = input_directory + "/" + output_sub_folder_name;
    string out_folder_directory = out_folder_mother_directory + Form("/evtTracklet_%s", file_name_index.c_str());

    pair<double, double> beam_origin = (data_type_str == "MC") ? ana_map_version::MC_beam_origin : ana_map_version::data_beam_origin;
    int clu_sum_adc_cut              = ana_map_version::clu_sum_adc_cut;
    int clu_size_cut                 = ana_map_version::clu_size_cut;
    int N_clu_cut                    = ana_map_version::evt_tracklet_N_clu_cut;
    int N_clu_cutl                   = ana_map_version::evt_tracklet_N_clu_cutl;
    int data_type = (data_type_str == "MC") ? ana_map_version::MC_data_type : ana_map_version::data_data_type;   // note : data F4A
    int geo_mode_id = 1; // note : 0 -> perfect geo., 1 -> first-order survey geo.
    double peek = 3.324;
    
    // note : not used
    pair<double, double> DCA_cut ={-1., 1.}; 
    pair<double, double> zvtx_QA_width = {40, 70}; 
    double zvtx_QA_ratio = 0.00006;
    double phi_diff_cut = 0.5; 
    int zvtx_cal_require = 15;
    bool draw_event_display = true;



    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut);

    int Nevt_per_core = ana_map_version::evt_tracklet_data_Nevt_per_core;

    int start_evt = index * Nevt_per_core;
    int end_evt   = (index + 1) * Nevt_per_core;
    if (start_evt >= INTTClu -> GetNEvt()) { return; }
    if (fabs(start_evt - INTTClu -> GetNEvt()) < Nevt_per_core) { return; } // note : because the previous will be handling the remaining events
    if (fabs(end_evt - INTTClu -> GetNEvt()) <= Nevt_per_core) { end_evt = INTTClu -> GetNEvt();}

    system(Form("mkdir %s", out_folder_directory.c_str()));
    INTTEta * MCEta = new INTTEta(
        INTTClu -> GetRunType(), // string run_type, 
        out_folder_directory,    // string out_folder_directory, 
        beam_origin,             // pair<double,double> beam_origin, 
        geo_mode_id,             // int geo_mode_id, 
        phi_diff_cut,            // double phi_diff_cut = 0.11, 
        DCA_cut,                 // pair<double, double> DCA_cut = {-1,1}, 
        N_clu_cutl,              // int N_clu_cutl = 20, 
        N_clu_cut,               // int N_clu_cut = 10000, 
        draw_event_display,      // bool draw_event_display = true, 
        peek,                    // double peek = 3.32405, 
        0,                       // double angle_diff_new_l = 0.0, 
        3,                       // double angle_diff_new_r = 3, 
        false                    // bool print_message_opt = true
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
        if (RecoZ -> is_min_bias_wozdc == 0) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        // note : to get rid of the nan value
        if (RecoZ -> Centrality_float != RecoZ -> Centrality_float) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        if (RecoZ -> MBD_reco_z       != RecoZ -> MBD_reco_z)       {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        if (RecoZ -> nclu_inner <= 0)   { MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue; }
        if (RecoZ -> nclu_outer <= 0)   { MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue; }
        if ((RecoZ -> nclu_inner + RecoZ -> nclu_outer) < N_clu_cutl) { MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue; }

        // todo : checked the used namespace
        double MBD_charge_assy = (RecoZ -> MBD_north_charge_sum - RecoZ -> MBD_south_charge_sum) / (RecoZ -> MBD_north_charge_sum + RecoZ -> MBD_south_charge_sum);
        if ( MBD_charge_assy < -1 * ana_map_version::MBD_assy_ratio_cut || MBD_charge_assy >  ana_map_version::MBD_assy_ratio_cut) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        if (RecoZ -> LB_Gaus_Width_width < ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_l || RecoZ -> LB_Gaus_Width_width > ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_r) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        if (RecoZ -> LB_cut_peak_width <   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_l || RecoZ -> LB_cut_peak_width >   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_r) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}        
        // note : the INTTrecoZ, MBDrecoZ diff cut
        if ( (RecoZ -> LB_Gaus_Mean_mean - RecoZ -> MBD_reco_z) < ana_map_version::INTTz_MBDz_diff_cut_l || (RecoZ -> LB_Gaus_Mean_mean - RecoZ -> MBD_reco_z) > ana_map_version::INTTz_MBDz_diff_cut_r ) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        double INTT_reco_z = RecoZ -> LB_Gaus_Mean_mean; // note : unit : mm

        // if (INTT_reco_z < -205 || INTT_reco_z > -195) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;} // note : special_tag cancel

        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        if (RecoZ -> bco_full != INTTClu -> GetBCOFull()) 
        {
            cout<<"error ! the bco_full is not matched !"<<endl;
            exit(1);
        }

        cout<<"bco_full diff : "<<RecoZ -> bco_full - INTTClu -> GetBCOFull()<<" original bcofull : "<< INTTClu -> GetBCOFull()<< " zvtx file bcofull : "<< RecoZ -> bco_full <<" Z check, recoZ: "<<INTT_reco_z<<endl;

        int Centrality_bin = ana_map_version::convert_centrality_bin(RecoZ -> Centrality_float);
        // note : the centrality bin rejection
        if (Centrality_bin >= ana_map_version::inclusive_Mbin_cut) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        MCEta     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, 
            INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), 
            INTTClu -> GetTrigvtxMC(), 
            INTTClu -> GetPhiCheckTag(), 
            INTTClu -> GetBCOFull(), 
            {INTT_reco_z, ana_map_version::reco_Z_resolution[Centrality_bin]},
            Centrality_bin, 
            INTTClu->GetTrueTrackInfo()
        );

        MCEta -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCEta -> PrintPlots();
    MCEta -> EndRun();

    system(Form("mv %s %s", out_folder_directory.c_str(), (out_folder_mother_directory + "/complete_file").c_str()));
    cout<<"core_"<<index<<" finished"<<endl;

    return;
}
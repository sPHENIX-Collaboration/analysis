#include "../../../../INTTReadTree.h"
#include "../../../../INTTEta.h"
#include "../../../../ReadINTTZ/ReadINTTZCombine.C"
#include "../../../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

void evtTracklet_mother(int core_i, string output_sub_folder_name = "evt_tracklet_inner_phi_rotate", string used_zvtx_folder_name = "evt_vtxZ")
{
    string input_directory = ana_map_version::MC_input_directory;
    string tree_name       = ana_map_version::MC_tree_name;
    string file_name_index = to_string(core_i); file_name_index = string(5 - file_name_index.length(), '0') + file_name_index;
    string file_name = "ntuple_" + file_name_index;
    
    string out_folder_mother_directory = input_directory + "/" + output_sub_folder_name;
    string out_folder_directory = out_folder_mother_directory + Form("/evtTracklet_%s", file_name_index.c_str());
    
    
    string input_recoZ_directory = input_directory + "/" + used_zvtx_folder_name + "/complete_file/evtZ_"+file_name_index+"/INTT_zvtx.root";
    TFile * file_in = TFile::Open(Form("%s",input_recoZ_directory.c_str()));
    TTree * Tree_z = (TTree *)file_in->Get("tree_Z");
    ReadINTTZCombine * RecoZ = new ReadINTTZCombine(Tree_z);
    cout<<"reco ZVTX tree entry : "<<Tree_z -> GetEntries()<<endl;

    pair<double, double> beam_origin = ana_map_version::MC_beam_origin;
    int clu_sum_adc_cut              = ana_map_version::clu_sum_adc_cut;
    int clu_size_cut                 = ana_map_version::clu_size_cut;
    int N_clu_cut                    = ana_map_version::evt_tracklet_N_clu_cut;
    int N_clu_cutl                   = ana_map_version::evt_tracklet_N_clu_cutl;
    int data_type = 5;   // note : MC, inner phi rotate
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
    
    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    system(Form("mkdir %s", out_folder_directory.c_str()));
    INTTEta     * MCEta     = new INTTEta(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, 0, 3, false);

    for (int event_i = 0; event_i < INTTClu -> GetNEvt(); event_i ++)
    {
        RecoZ -> LoadTree(event_i);
        RecoZ -> GetEntry(event_i);

        // note : MC, but the event is not the minimum bias event
        if (INTTClu -> GetRunType() == "MC" && RecoZ -> is_min_bias_wozdc == 0) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        // todo: here I use is_min_bias_wozdc for data
        // note : data, but the event is not the minomum bias event (wozdc)
        else { 
            if ( RecoZ -> is_min_bias_wozdc == 0) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}
        }

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
        if ( (RecoZ -> LB_Gaus_Mean_mean - RecoZ -> MBD_reco_z) < ana_map_version::INTTz_MBDz_diff_cut_MC_l || (RecoZ -> LB_Gaus_Mean_mean - RecoZ -> MBD_reco_z) > ana_map_version::INTTz_MBDz_diff_cut_MC_r ) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        if (RecoZ -> nclu_inner != INTTClu -> temp_sPH_inner_nocolumn_vec.size() || RecoZ -> nclu_outer != INTTClu -> temp_sPH_outer_nocolumn_vec.size()) 
        {
            cout<<"error ! number of cluster doesn't match ! RecoZ inner : "<<RecoZ -> nclu_inner<<" INTTClu inner: "<<INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<", RecoZ outer: "<<RecoZ -> nclu_outer<<" INTTClu outer: "<<INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<endl;
            exit(1);
        }

        int Centrality_bin = ana_map_version::convert_centrality_bin(RecoZ -> Centrality_float);
        // note : the centrality bin rejection
        if (Centrality_bin >= ana_map_version::inclusive_Mbin_cut) {MCEta -> ClearEvt(); INTTClu -> EvtClear(); continue;}

        cout<<"Z check, recoZ: "<<RecoZ -> LB_Gaus_Mean_mean<<" TrueZ: "<<INTTClu -> GetTrigvtxMC()[2]*10<<" diff: "<< RecoZ -> LB_Gaus_Mean_mean - INTTClu -> GetTrigvtxMC()[2]*10<<" MBin: "<<INTTClu->GetCentralityBin()<<" recoZ resolution: "<<ana_map_version::reco_Z_resolution[Centrality_bin]<<endl;
        // cout<<"bco_full diff : "<<RecoZ -> bco_full - INTTClu -> GetBCOFull()<<" original bcofull : "<< INTTClu -> GetBCOFull()<< " zvtx file bcofull : "<< RecoZ -> bco_full <<" Z check, recoZ: "<<RecoZ -> LB_Gaus_Mean_mean<<endl;

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
            {RecoZ -> LB_Gaus_Mean_mean, ana_map_version::reco_Z_resolution[Centrality_bin]}, 
            Centrality_bin, 
            INTTClu->GetTrueTrackInfo()
        );

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
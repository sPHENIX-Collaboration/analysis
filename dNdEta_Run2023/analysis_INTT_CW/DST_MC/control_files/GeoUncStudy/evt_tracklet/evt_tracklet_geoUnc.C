#include "../../../INTTReadTree.h"
#include "../../../INTTEta.h"
#include "../../../ReadINTTZ/ReadINTTZCombine.C"
#include "../../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

void evt_tracklet_geoUnc(int random_seed)
{
    string input_directory = ana_map_version::MC_input_directory;
    string tree_name       = ana_map_version::MC_tree_name;
    string file_name       = "ntuple_merged";

    string out_random_seed = to_string(random_seed); out_random_seed = string(6 - out_random_seed.length(), '0') + out_random_seed;
    string out_folder_mother_directory = input_directory + "/Geo_test/evt_tracklet";
    string out_folder_directory = out_folder_mother_directory + Form("/evtTracklet_%s", out_random_seed.c_str());
    
    string input_evtZ_complete_directory = input_directory + "/Geo_test/evt_vtxZ/complete_file";    
    string input_avgXY_directory = input_evtZ_complete_directory + Form("/zvtx_dist_info_%s.root", out_random_seed.c_str());
    string input_geoXY_directory = input_directory + Form("/Geo_test/avg_vtxXY/complete_file/zvtx_used/geo_scan_avgXY_%s.root", out_random_seed.c_str());
    string input_recoZ_directory = input_evtZ_complete_directory + Form("/INTT_zvtx_%s.root", out_random_seed.c_str());

    // note : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    TFile * file_avgXY = TFile::Open(Form("%s", input_avgXY_directory.c_str()));
    TTree * tree_avgXY = (TTree*) file_avgXY -> Get("tree");
    double final_avgX;
    double final_avgY;
    tree_avgXY -> SetBranchAddress("reco_avgX", &final_avgX); // note : in the unit of mm, already
    tree_avgXY -> SetBranchAddress("reco_avgY", &final_avgY); // note : in the unit of mm, already
    tree_avgXY -> GetEntry(0);
    
    
    // note : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    TFile * file_geo_XY = TFile::Open( Form("%s", input_geoXY_directory.c_str()) );
    TTree * tree_geo_XY = (TTree*) file_geo_XY -> Get("tree_geo_scan");

    vector<double> * in_ladder_offset_x_vec; in_ladder_offset_x_vec = 0;
    vector<double> * in_ladder_offset_y_vec; in_ladder_offset_y_vec = 0;
    vector<double> * in_ladder_offset_z_vec; in_ladder_offset_z_vec = 0;
    double in_total_offset_x;
    double in_total_offset_y;
    double in_total_offset_z;
    double final_avgX_geo;
    double final_avgY_geo;
    int in_random_seed;

    tree_geo_XY -> SetBranchAddress("offset_x_vec", &in_ladder_offset_x_vec);
    tree_geo_XY -> SetBranchAddress("offset_y_vec", &in_ladder_offset_y_vec);
    tree_geo_XY -> SetBranchAddress("offset_z_vec", &in_ladder_offset_z_vec);
    tree_geo_XY -> SetBranchAddress("total_offset_x", &in_total_offset_x);
    tree_geo_XY -> SetBranchAddress("total_offset_y", &in_total_offset_y);
    tree_geo_XY -> SetBranchAddress("total_offset_z", &in_total_offset_z);
    tree_geo_XY -> SetBranchAddress("vtxX", &final_avgX_geo);
    tree_geo_XY -> SetBranchAddress("vtxY", &final_avgY_geo);
    tree_geo_XY -> SetBranchAddress("random_seed", &in_random_seed);
    tree_geo_XY -> GetEntry(0);

    int index_i = 0;
    map<string, vector<double>> ladder_offset_map; ladder_offset_map.clear();
    // cout<<"test-2"<<endl;
    for (int layer_i = 3; layer_i < 7; layer_i++)
    {
        double N_layer_ladder = (layer_i == 3 || layer_i == 4) ? 12 : 16;

        for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++)
        {
            ladder_offset_map[Form("%i_%i", layer_i, phi_i)] = {in_ladder_offset_x_vec -> at(index_i), in_ladder_offset_y_vec -> at(index_i), in_ladder_offset_z_vec -> at(index_i)};
            index_i += 1;
        }
    }

    if (random_seed != in_random_seed) { cout<<"random_seed not match"<<endl; return; }
    if ( fabs(final_avgX_geo * 10. - final_avgX) > 0.05 || fabs(final_avgY_geo * 10. - final_avgY) > 0.05 ) { cout<<"geo XY not match"<<endl; return; }

    // note : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    TFile * file_in = TFile::Open(Form("%s",input_recoZ_directory.c_str()));
    TTree * Tree_z = (TTree *)file_in->Get("tree_Z");
    ReadINTTZCombine * RecoZ = new ReadINTTZCombine(Tree_z);
    cout<<"reco ZVTX tree entry : "<<Tree_z -> GetEntries()<<endl;

    pair<double, double> beam_origin = {final_avgX, final_avgY};
    int clu_sum_adc_cut              = ana_map_version::clu_sum_adc_cut;
    int clu_size_cut                 = ana_map_version::clu_size_cut;
    int N_clu_cut                    = ana_map_version::evt_tracklet_N_clu_cut;
    int N_clu_cutl                   = ana_map_version::evt_tracklet_N_clu_cutl;
    int data_type                    = 4; // note : MC geo test
    int geo_mode_id = 1; // note : 0 -> perfect geo., 1 -> first-order survey geo.
    double peek = 3.324;
    
    // note : not used
    pair<double, double> DCA_cut ={-1., 1.}; 
    pair<double, double> zvtx_QA_width = {40, 70}; 
    double zvtx_QA_ratio = 0.00006;
    double phi_diff_cut = 0.5; 
    int zvtx_cal_require = 15;
    bool draw_event_display = false;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, file_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTClu -> set_ladder_offset(ladder_offset_map);    

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    system(Form("mkdir %s", out_folder_directory.c_str()));
    INTTEta     * MCEta     = new INTTEta(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, 0, 3, false);

    for (int event_i = 0; event_i < Tree_z -> GetEntries(); event_i ++)
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

        if (event_i % 1000 == 0) cout<<"event_i: "<<event_i<<endl;

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
            INTTClu->GetTrueTrackInfo(),
            ana_map_version::zvtx_reweight_factor[ana_map_version::get_zvtx_BinID(RecoZ -> LB_Gaus_Mean_mean)]
        );

        MCEta -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCEta -> PrintPlots();
    MCEta -> EndRun();

    system(Form("cp %s/INTT_final_hist_info.root %s/INTT_final_hist_info_%s.root", out_folder_directory.c_str(), (out_folder_mother_directory + "/complete_file").c_str(), out_random_seed.c_str()));
    system(Form("rm -r %s", out_folder_directory.c_str()));

    system(Form("rm /sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/DST_MC/control_files/GeoUncStudy/evt_tracklet/run_root_sub_evtTracklet_%i.sh", random_seed));
    system(Form("rm %s/CW_log/condor_output_%i.err", (input_directory + "/Geo_test/evt_tracklet").c_str(), random_seed));
    system(Form("rm %s/CW_log/condor_output_%i.log", (input_directory + "/Geo_test/evt_tracklet").c_str(), random_seed));

    cout<<"core_"<<random_seed<<" finished"<<endl;

}


        // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
        //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
        //     << INTTClu -> GetNvtxMC()<<" "
        //     << INTTClu -> GetTrigZvtxMC()<<" "
        //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;
#include "../../../INTTReadTree.h"
#include "../../../INTTZvtx.h"

// note : index_x for the delta_phi
// note : index_y for the DCA

/*
note : for scan 1
note : core_i: 0 ~ 2499
int index_x = core_i%50;
int index_y = core_i/50;
double phi_diff_cut = 0.01 * double(index_x + 1.);
pair<double, double> DCA_cut ={-0.015 * double(index_y + 1.), 0.015 * double(index_y + 1.)};
*/

/*
note : for scan 2
note : core_i: 0 ~ 899
int index_x = core_i%30;
int index_y = core_i/30;
double phi_diff_cut = 0.01 + 0.15 * double(index_x);
pair<double, double> DCA_cut ={-0.015 - (0.15 * double(index_y)), 0.015 +  (0.15 * double(index_y))};
*/

void evt_z_398_mother(int core_i)
{

    int index_x = core_i%30;
    int index_y = core_i/30;

    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams";
    string file_name = "MC_ZF_zvtx";
    string out_folder_directory = input_directory + "/zvtx_cut_scan_2" + Form("/core_%i", core_i);
    string MC_list_name = "file_list.txt";
    string tree_name = "EventTree";
    
    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    // pair<double, double> beam_origin = {-0.4, 2.4};
    pair<double, double> beam_origin = {-0.394532, 2.40234}; // note : line fill + quadrant method average
    
    double phi_diff_cut = 0.01 + 0.15 * double(index_x);
    pair<double, double> DCA_cut ={-0.015 - (0.15 * double(index_y)), 0.015 +  (0.15 * double(index_y))};

    pair<double, double> zvtx_QA_width = {35, 70}; 
    double zvtx_QA_ratio = 0.00005;
    int clu_sum_adc_cut = -1;
    int clu_size_cut = 4;
    int N_clu_cut = 10000;
    int N_clu_cutl = 20;
    
    bool draw_event_display = false;
    double peek = 3.324;
    int data_type = 0; // note : MC
    int geo_mode_id = 0; // note : 0 -> perfect geo
    int zvtx_cal_require = 5;

    cout<<"in zvtx scan core : "<<core_i<<" index_x: "<<index_x<<" index_y: "<<index_y<<" DeltaPhi : "<<phi_diff_cut<<", DCA: "<<DCA_cut.first<<" to "<<DCA_cut.second<<endl;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    INTTZvtx     * MCz     = new INTTZvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, zvtx_cal_require, zvtx_QA_width, zvtx_QA_ratio, draw_event_display, peek);

    cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;


    double out_dist_mean;
    double out_dist_width;
    double out_delta_phi_cut;
    double out_dca_cut; 
    TFile * file_out = new TFile(Form("%s/zvtx_dist_info_%i_%i_%i.root",out_folder_directory.c_str(),core_i, index_x, index_y), "RECREATE");
    TTree * tree_out = new TTree("tree","info. of zvtx dist");
    tree_out -> Branch("index_x", &index_x);
    tree_out -> Branch("index_y", &index_y);
    tree_out -> Branch("deltaphi_cut", &out_delta_phi_cut);
    tree_out -> Branch("dca_cut", &out_dca_cut);
    tree_out -> Branch("zvtx_dist_mean", &out_dist_mean);
    tree_out -> Branch("zvtx_dist_width", &out_dist_width);

    out_delta_phi_cut = phi_diff_cut;
    out_dca_cut = DCA_cut.second;

    for (int event_i = 0; event_i < INTTClu -> GetNEvt(); event_i ++)
    {
        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        MCz     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), -1, INTTClu->GetCentralityBin()  // note : no bco_full for MC
        );

        MCz -> ClearEvt();
        INTTClu -> EvtClear();
    }

    // MCz -> PrintPlots();
    MCz -> EndRun();

    out_dist_mean  = MCz -> GetZdiffPeakMC();
    out_dist_width = MCz -> GetZdiffWidthMC();

    tree_out -> Fill();

    file_out -> cd();
    tree_out -> Write();
    file_out -> Close();

    system(Form("mv %s/INTT_zvtx.root %s/INTT_zvtx_%i_%i_%i.root", out_folder_directory.c_str(), out_folder_directory.c_str(), core_i, index_x, index_y));
    system(Form("mv %s/INTT_zvtx_%i_%i_%i.root %s",      out_folder_directory.c_str(), core_i, index_x, index_y, (input_directory + "/zvtx_cut_scan_2/complete_file").c_str()));
    system(Form("mv %s/zvtx_dist_info_%i_%i_%i.root %s", out_folder_directory.c_str(), core_i, index_x, index_y, (input_directory + "/zvtx_cut_scan_2/complete_file").c_str()));
    system(Form("rm -r %s", out_folder_directory.c_str()));
    
    // system(Form("mv %s %s", out_folder_directory.c_str(), (input_directory + "/Geo_Z_scan_trial_1/complete_file/folder_"+to_string(core_i)).c_str()));

    cout<<"core : "<<core_i<<" index_x: "<<index_x<<" index_y: "<<index_y<<" run finished"<<endl;

    return;
}
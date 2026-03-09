#include "INTTReadTree.h"
// #include "INTTZvtx.h"
#include "INTTXYvtxEvt.h"
#include "ReadINTTZ.C"

void test_xyEvt_388_000_recoZ()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC_388_000";
    string file_name = "MC_ZF_xyvtx";
    string out_folder_directory = input_directory + "/SemiFinal_EvtXY_" + file_name;
    string MC_list_name = "dst_INTT_dNdEta_388_000.list";
    string tree_name = "EventTree";
    
    // pair<double, double> beam_origin = {-0.015, 0.012};
    // pair<double, double> beam_origin = {-0.0, 0.0};
    // pair<double, double> beam_origin = {-0.4, 2.4};
    pair<double, double> beam_origin = {-0.38137, 2.40556}; // note : line fill + quadrant method average
    pair<double, double> DCA_cut = {-1, 1}; //{-0.5, 0.5};
    pair<double, double> zvtx_QA_width = {35, 70}; 
    double zvtx_QA_ratio = 0.00005;
    double phi_diff_cut = 0.3;
    int clu_sum_adc_cut = -1;
    int clu_size_cut = 4;
    int N_clu_cut = 10000;
    int N_clu_cutl = 100;
    
    bool draw_event_display = false;
    double peek = 3.324;
    int data_type = 0; // note : MC
    int geo_mode_id = 0; // note : 0 -> perfect geo
    int zvtx_cal_require = 15;

    INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);
    // INTTZvtx     * MCz     = new INTTZvtx(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, zvtx_cal_require, zvtx_QA_width, zvtx_QA_ratio, draw_event_display, peek);
    INTTXYvtxEvt * MCxy    = new INTTXYvtxEvt(INTTClu -> GetRunType(), out_folder_directory, beam_origin, geo_mode_id, phi_diff_cut, DCA_cut, N_clu_cutl, N_clu_cut, draw_event_display, peek, 0, 3, false, 4, 10);

    TFile * File_z = TFile::Open(Form("%s/SemiFinal_EvtZ_MC_ZF_zvtx/INTT_zvtx.root", input_directory.c_str()));
    TTree * Tree_z = (TTree*) File_z -> Get("tree_Z");
    ReadINTTZ * RecoZ = new ReadINTTZ(Tree_z);

    cout<<"Total event of INTTClu : "<<INTTClu -> GetNEvt()<<endl;
    cout<<"Total event of INTT Reco Z : "<<Tree_z -> GetEntries()<<endl;

    int NClus_out;
    double true_X_out, true_Y_out, true_Z_out, reco_X_out, reco_Y_out, reco_Z_out;
    double reco_XError_out, reco_YError_out, reco_ZError_out;
    double reco_XStd_out, reco_YStd_out;
    TFile * out_file = new TFile(Form("%s/INTT_ebe3D_vtx.root", out_folder_directory.c_str()), "recreate");
    TTree * out_tree = new TTree("tree_vtx", "tree_vtx");
    out_tree -> Branch("NClus", &NClus_out);
    out_tree -> Branch("true_X", &true_X_out);
    out_tree -> Branch("true_Y", &true_Y_out);
    out_tree -> Branch("true_Z", &true_Z_out);

    out_tree -> Branch("reco_X", &reco_X_out);
    out_tree -> Branch("reco_Y", &reco_Y_out);
    out_tree -> Branch("reco_Z", &reco_Z_out);
    out_tree -> Branch("reco_XE", &reco_XError_out);
    out_tree -> Branch("reco_YE", &reco_YError_out);
    out_tree -> Branch("reco_ZE", &reco_ZError_out);
    out_tree -> Branch("reco_XStd", &reco_XStd_out);
    out_tree -> Branch("reco_YStd", &reco_YStd_out);

    TRandom * rand_gen = new TRandom3();

    for (int event_i = 0; event_i < 10000; event_i ++)
    {
        RecoZ -> LoadTree(event_i);
        RecoZ -> GetEntry(event_i);

        if (RecoZ -> nclu_inner == -1 || RecoZ -> nclu_outer == -1) {continue;}
        if (RecoZ -> good_zvtx_tag != 1) {continue;}

        INTTClu -> EvtInit(event_i);
        INTTClu -> EvtSetCluGroup();

        // MCz     -> ProcessEvt(
        //     event_i, 
        //     INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
        //     INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
        //     INTTClu -> GetNvtxMC(), INTTClu -> GetTrigZvtxMC(), INTTClu -> GetPhiCheckTag(), -1  // note : no bco_full for MC
        // );

        // double fake_reco_Z = rand_gen -> Gaus(0,0.5) + INTTClu -> GetTrigvtxMC()[2] * 10.;
        double fake_reco_Z = INTTClu -> GetTrigvtxMC()[2] * 10.;
        
        MCxy     -> ProcessEvt(
            event_i, 
            INTTClu -> temp_sPH_inner_nocolumn_vec, INTTClu -> temp_sPH_outer_nocolumn_vec, 
            INTTClu -> temp_sPH_nocolumn_vec, INTTClu -> temp_sPH_nocolumn_rz_vec, 
            INTTClu -> GetNvtxMC(), INTTClu -> GetTrigvtxMC(), INTTClu -> GetPhiCheckTag(), -1, {RecoZ -> LB_Gaus_Mean_mean, RecoZ -> LB_Gaus_Mean_meanE} //{fake_reco_Z, 0} // note : no bco_full for MC
        );

        if (MCxy -> GetReturnTag() == 0) {
            MCxy -> ClearEvt();
            INTTClu -> EvtClear();
            continue;
        }

        NClus_out = INTTClu -> temp_sPH_inner_nocolumn_vec.size() + INTTClu -> temp_sPH_outer_nocolumn_vec.size();
        true_X_out = INTTClu -> GetTrigvtxMC()[0] * 10.;
        true_Y_out = INTTClu -> GetTrigvtxMC()[1] * 10.;
        true_Z_out = INTTClu -> GetTrigvtxMC()[2] * 10.;

        reco_X_out = MCxy -> GetEvtVtxInfo()[0].first;
        reco_Y_out = MCxy -> GetEvtVtxInfo()[0].second;
        reco_XError_out = MCxy -> GetEvtVtxInfo()[1].first;
        reco_YError_out = MCxy -> GetEvtVtxInfo()[1].second;
        reco_XStd_out = MCxy -> GetEvtVtxInfo()[2].first;
        reco_YStd_out = MCxy -> GetEvtVtxInfo()[2].second;
        
        reco_Z_out = RecoZ -> LB_Gaus_Mean_mean;
        reco_ZError_out = RecoZ -> LB_Gaus_Mean_meanE;
        // reco_Z_out = fake_reco_Z;
        // reco_ZError_out = 0;

        out_tree -> Fill();

        MCxy -> ClearEvt();
        INTTClu -> EvtClear();
    }

    MCxy -> PrintPlots_Evt();
    MCxy -> EndRun();
    // MCz -> PrintPlots();
    // MCz -> EndRun();

    out_file -> cd();
    out_tree -> SetDirectory(out_file);
    out_tree -> Write("", TObject::kOverwrite);

    out_file -> Close();
}



    // cout<< INTTClu -> temp_sPH_inner_nocolumn_vec.size()<<" "
    //     << INTTClu -> temp_sPH_outer_nocolumn_vec.size()<<" "
    //     << INTTClu -> temp_sPH_nocolumn_vec.size()<<" "
    //     << INTTClu -> temp_sPH_nocolumn_rz_vec.size()<<" "
    //     << INTTClu -> GetNvtxMC()<<" "
    //     << INTTClu -> GetTrigZvtxMC()<<" "
    //     << INTTClu -> GetPhiCheckTag()<<" "<<endl;
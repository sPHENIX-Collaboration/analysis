void read_inttZ_test()
{
    string folder_direction = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut_advanced_test";
    TFile * f_intt = TFile::Open(Form("%s/INTT_zvtx.root", folder_direction.c_str()));
    // gDirectory = gDir;
    TTree * t_intt = (TTree * ) f_intt -> Get("tree_Z");
    if (!t_intt) return;

    int intt_eID, intt_N_cluster_outer, intt_N_cluster_inner, intt_N_good, intt_ES_N_good, intt_N_group;
    double intt_ES_zvtx, intt_ES_zvtxE, intt_ES_rangeL, intt_ES_rangeR, intt_ES_width_density;
    double intt_LB_Gaus_mean, intt_LB_Gaus_meanE, intt_LB_Gaus_width, intt_LB_Gaus_offset, intt_LB_Gaus_chi2, intt_LB_Gaus_size_width, intt_LB_geo_mean;
    double MC_true_zvtx;
    bool intt_good_zvtx_tag;
    Long64_t intt_bco_full; 

    t_intt -> Branch("eID",&intt_eID);
    t_intt -> Branch("bco_full",&intt_bco_full);
    t_intt -> Branch("nclu_inner",&intt_N_cluster_inner);
    t_intt -> Branch("nclu_outer",&intt_N_cluster_outer);
    t_intt -> Branch("ES_zvtx",&intt_ES_zvtx);
    t_intt -> Branch("ES_zvtxE",&intt_ES_zvtxE);
    t_intt -> Branch("ES_rangeL",&intt_ES_rangeL);
    t_intt -> Branch("ES_rangeR",&intt_ES_rangeR);
    t_intt -> Branch("ES_N_good",&intt_ES_N_good);
    t_intt -> Branch("ES_Width_density",&intt_ES_width_density);
    t_intt -> Branch("LB_Gaus_mean",&intt_LB_Gaus_mean);
    t_intt -> Branch("LB_Gaus_meanE",&intt_LB_Gaus_meanE);
    t_intt -> Branch("LB_Gaus_width",&intt_LB_Gaus_width);
    t_intt -> Branch("LB_Gaus_offset", &intt_LB_Gaus_offset);
    t_intt -> Branch("LB_Gaus_chi2", &intt_LB_Gaus_chi2);
    t_intt -> Branch("LB_Gaus_size_width", &intt_LB_Gaus_size_width);
    t_intt -> Branch("LB_geo_mean", &intt_LB_geo_mean);
    t_intt -> Branch("good_zvtx_tag", &intt_good_zvtx_tag);
    t_intt -> Branch("N_group", &intt_N_group);
    t_intt -> Branch("MC_true_zvtx",&MC_true_zvtx);

    cout <<"second test : "<< t_mbd -> GetEntries() << " " << t_intt -> GetEntries() << endl;
    t_intt -> GetEntry(0);
    cout <<"second test : "<<intt_N_cluster_inner<<endl;

    cout << t_mbd -> GetEntries() << " " << t_intt -> GetEntries() << endl;
}
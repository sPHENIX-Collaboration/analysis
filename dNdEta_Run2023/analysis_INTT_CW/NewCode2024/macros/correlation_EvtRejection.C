struct EvtProfile{
    int NClus;
    int NClus_Layer1;
    float MBD_charge_sum;
    float MBD_z_vtx;
    float MBD_centrality;
    ULong_t INTT_BCO;
    ULong_t next_INTT_BCO;
    std::vector<float> ClusPhiSize;
};

std::vector<std::string> files = {
    "ntuple_00000.root",
    "ntuple_00001.root",
    "ntuple_00002.root",
    "ntuple_00003.root",
    "ntuple_00004.root",
    "ntuple_00005.root",
    "ntuple_00006.root",
    "ntuple_00007.root",
    "ntuple_00008.root",
    "ntuple_00009.root",
    "ntuple_00010.root",
    "ntuple_00011.root",
    "ntuple_00012.root",
    "ntuple_00013.root",
    "ntuple_00014.root",
    "ntuple_00015.root",
    "ntuple_00016.root",
    "ntuple_00017.root",
    "ntuple_00018.root",
    "ntuple_00019.root",
    "ntuple_00020.root",
    "ntuple_00021.root",
    "ntuple_00022.root",
    "ntuple_00023.root",
    "ntuple_00024.root",
    "ntuple_00025.root",
    "ntuple_00026.root",
    "ntuple_00027.root",
    "ntuple_00028.root",
    "ntuple_00029.root",
    "ntuple_00030.root",
    "ntuple_00031.root",
    "ntuple_00032.root",
    "ntuple_00033.root",
    "ntuple_00034.root",
    "ntuple_00035.root",
    "ntuple_00036.root",
    "ntuple_00037.root",
    "ntuple_00038.root",
    "ntuple_00039.root",
    "ntuple_00040.root",
    "ntuple_00041.root",
    "ntuple_00042.root",
    "ntuple_00043.root",
    "ntuple_00044.root",
    "ntuple_00045.root",
    "ntuple_00046.root",
    "ntuple_00047.root",
    "ntuple_00048.root",
    "ntuple_00049.root",
    "ntuple_00050.root",
    "ntuple_00051.root",
    "ntuple_00052.root",
    "ntuple_00053.root",
    "ntuple_00054.root",
    "ntuple_00055.root",
    "ntuple_00056.root",
    "ntuple_00057.root",
    "ntuple_00058.root",
    "ntuple_00060.root",
    "ntuple_00061.root",
    "ntuple_00062.root",
    "ntuple_00063.root",
    "ntuple_00064.root",
    "ntuple_00065.root",
    "ntuple_00066.root",
    "ntuple_00067.root",
    "ntuple_00068.root",
    "ntuple_00069.root",
    "ntuple_00070.root",
    "ntuple_00071.root",
    "ntuple_00072.root",
    "ntuple_00073.root",
    "ntuple_00074.root",
    "ntuple_00075.root",
    "ntuple_00076.root",
    "ntuple_00077.root",
    "ntuple_00078.root",
    "ntuple_00079.root",
    "ntuple_00080.root",
    "ntuple_00081.root",
    "ntuple_00082.root",
    "ntuple_00083.root",
    "ntuple_00084.root",
    "ntuple_00085.root",
    "ntuple_00086.root",
    "ntuple_00087.root",
    "ntuple_00088.root",
    "ntuple_00089.root",
    "ntuple_00090.root",
    "ntuple_00091.root",
    "ntuple_00092.root",
    "ntuple_00093.root",
    "ntuple_00094.root",
    "ntuple_00095.root",
    "ntuple_00096.root",
    "ntuple_00097.root",
    "ntuple_00098.root",
    "ntuple_00099.root",
    "ntuple_00100.root",
    "ntuple_00101.root",
    "ntuple_00102.root",
    "ntuple_00103.root",
    "ntuple_00104.root",
    "ntuple_00105.root",
    "ntuple_00106.root",
    "ntuple_00107.root",
    "ntuple_00108.root",
    "ntuple_00109.root",
    "ntuple_00110.root",
    "ntuple_00111.root",
    "ntuple_00112.root",
    "ntuple_00113.root",
    "ntuple_00114.root",
    "ntuple_00115.root",
    "ntuple_00116.root",
    "ntuple_00117.root",
    "ntuple_00118.root",
    "ntuple_00119.root",
    "ntuple_00120.root",
    "ntuple_00121.root",
    "ntuple_00122.root",
    "ntuple_00123.root",
    "ntuple_00124.root",
    "ntuple_00125.root",
    "ntuple_00126.root",
    "ntuple_00127.root",
    "ntuple_00128.root",
    "ntuple_00129.root",
    "ntuple_00130.root",
    "ntuple_00131.root",
    "ntuple_00132.root",
    "ntuple_00133.root",
    "ntuple_00134.root",
    "ntuple_00135.root",
    "ntuple_00136.root",
    "ntuple_00137.root",
    "ntuple_00138.root",
    "ntuple_00139.root",
    "ntuple_00140.root",
    "ntuple_00141.root",
    "ntuple_00142.root",
    "ntuple_00143.root",
    "ntuple_00144.root",
    "ntuple_00145.root",
    "ntuple_00146.root",
    "ntuple_00147.root",
    "ntuple_00148.root",
    "ntuple_00149.root",
    "ntuple_00150.root",
    "ntuple_00151.root",
    "ntuple_00152.root",
    "ntuple_00153.root",
    "ntuple_00154.root",
    "ntuple_00155.root",
    "ntuple_00156.root",
    "ntuple_00157.root",
    "ntuple_00158.root",
    "ntuple_00159.root",
    "ntuple_00160.root",
    "ntuple_00161.root",
    "ntuple_00162.root",
    "ntuple_00163.root",
    "ntuple_00164.root",
    "ntuple_00165.root",
    "ntuple_00166.root",
    "ntuple_00167.root",
    "ntuple_00168.root",
    "ntuple_00169.root",
    "ntuple_00170.root",
    "ntuple_00171.root",
    "ntuple_00172.root",
    "ntuple_00173.root",
    "ntuple_00174.root",
    "ntuple_00175.root",
    "ntuple_00176.root",
    "ntuple_00177.root",
    "ntuple_00178.root",
    "ntuple_00179.root",
    "ntuple_00180.root",
    "ntuple_00181.root",
    "ntuple_00182.root",
    "ntuple_00183.root",
    "ntuple_00184.root",
    "ntuple_00185.root",
    "ntuple_00186.root",
    "ntuple_00187.root",
    "ntuple_00188.root",
    "ntuple_00189.root",
    "ntuple_00190.root",
    "ntuple_00191.root",
    "ntuple_00192.root",
    "ntuple_00193.root",
    "ntuple_00194.root",
    "ntuple_00195.root",
    "ntuple_00196.root",
    "ntuple_00197.root",
    "ntuple_00198.root",
    "ntuple_00199.root",
    "ntuple_00200.root",
    "ntuple_00201.root",
    "ntuple_00202.root",
    "ntuple_00203.root",
    "ntuple_00204.root",
    "ntuple_00205.root",
    "ntuple_00206.root",
    "ntuple_00207.root",
    "ntuple_00208.root",
    "ntuple_00209.root",
    "ntuple_00210.root",
    "ntuple_00211.root",
    "ntuple_00212.root",
    "ntuple_00213.root",
    "ntuple_00214.root",
    "ntuple_00215.root",
    "ntuple_00216.root",
    "ntuple_00217.root",
    "ntuple_00218.root",
    "ntuple_00219.root",
    "ntuple_00220.root",
    "ntuple_00221.root",
    "ntuple_00222.root",
    "ntuple_00223.root",
    "ntuple_00224.root",
    "ntuple_00225.root",
    "ntuple_00226.root",
    "ntuple_00227.root",
    "ntuple_00228.root",
    "ntuple_00229.root",
    "ntuple_00230.root",
    "ntuple_00231.root",
    "ntuple_00232.root",
    "ntuple_00233.root",
    "ntuple_00234.root",
    "ntuple_00235.root",
    "ntuple_00236.root",
    "ntuple_00237.root",
    "ntuple_00238.root",
    "ntuple_00239.root",
    "ntuple_00240.root",
    "ntuple_00241.root",
    "ntuple_00242.root",
    "ntuple_00243.root",
    "ntuple_00244.root",
    "ntuple_00245.root",
    "ntuple_00246.root",
    "ntuple_00247.root",
    "ntuple_00248.root",
    "ntuple_00249.root",
    "ntuple_00250.root",
    "ntuple_00251.root",
    "ntuple_00252.root",
    "ntuple_00253.root",
    "ntuple_00254.root",
    "ntuple_00255.root",
    "ntuple_00256.root",
    "ntuple_00257.root",
    "ntuple_00258.root",
    "ntuple_00259.root",
    "ntuple_00260.root",
    "ntuple_00261.root",
    "ntuple_00262.root",
    "ntuple_00263.root",
    "ntuple_00264.root",
    "ntuple_00265.root",
    "ntuple_00266.root",
    "ntuple_00267.root",
    "ntuple_00268.root",
    "ntuple_00269.root",
    "ntuple_00270.root",
    "ntuple_00271.root",
    "ntuple_00272.root",
    "ntuple_00273.root",
    "ntuple_00274.root",
    "ntuple_00275.root",
    "ntuple_00276.root",
    "ntuple_00277.root",
    "ntuple_00278.root",
    "ntuple_00279.root",
    "ntuple_00280.root",
    "ntuple_00281.root",
    "ntuple_00282.root",
    "ntuple_00283.root",
    "ntuple_00284.root",
    "ntuple_00285.root",
    "ntuple_00286.root",
    "ntuple_00287.root",
    "ntuple_00288.root",
    "ntuple_00289.root",
    "ntuple_00290.root",
    "ntuple_00291.root",
    "ntuple_00292.root",
    "ntuple_00293.root",
    "ntuple_00294.root",
    "ntuple_00295.root",
    "ntuple_00296.root",
    "ntuple_00297.root",
    "ntuple_00298.root",
    "ntuple_00299.root",
    "ntuple_00300.root",
    "ntuple_00301.root",
    "ntuple_00302.root",
    "ntuple_00303.root",
    "ntuple_00304.root",
    "ntuple_00305.root",
    "ntuple_00306.root",
    "ntuple_00307.root",
    "ntuple_00308.root",
    "ntuple_00309.root",
    "ntuple_00310.root",
    "ntuple_00311.root",
    "ntuple_00312.root",
    "ntuple_00313.root",
    "ntuple_00314.root",
    "ntuple_00315.root",
    "ntuple_00316.root",
    "ntuple_00317.root",
    "ntuple_00318.root",
    "ntuple_00319.root",
    "ntuple_00320.root",
    "ntuple_00321.root",
    "ntuple_00322.root",
    "ntuple_00323.root",
    "ntuple_00324.root",
    "ntuple_00325.root"
};

int correlation_EvtRejection()
{
    
    TChain * tree_in = new TChain("EventTree");
    for (int i = 0; i < files.size(); i++)
    {
        tree_in -> Add(files[i].c_str());
    }

    tree_in -> SetBranchStatus("*", 0);
    tree_in -> SetBranchStatus("NClus", 1);
    tree_in -> SetBranchStatus("NClus_Layer1", 1);
    tree_in -> SetBranchStatus("MBD_charge_sum", 1);
    tree_in -> SetBranchStatus("MBD_z_vtx", 1);
    tree_in -> SetBranchStatus("MBD_centrality", 1);
    tree_in -> SetBranchStatus("INTT_BCO", 1);
    tree_in -> SetBranchStatus("ClusPhiSize", 1);

    int NClus;
    int NClus_Layer1;
    float MBD_charge_sum;
    float MBD_z_vtx;
    float MBD_centrality;
    ULong_t INTT_BCO;
    std::vector<float> *ClusPhiSize;
    ClusPhiSize = 0;
    

    tree_in -> SetBranchAddress("NClus", &NClus);
    tree_in -> SetBranchAddress("NClus_Layer1", &NClus_Layer1);
    tree_in -> SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress("MBD_centrality", &MBD_centrality);
    tree_in -> SetBranchAddress("INTT_BCO", &INTT_BCO);
    tree_in -> SetBranchAddress("ClusPhiSize", &ClusPhiSize);

    long long BcoFullDiff_cut = 61;
    std::string output_folder_name = Form("EvtBcoDiffCutStudy_%lld", BcoFullDiff_cut);
    system(Form("mkdir -p %s", output_folder_name.c_str()));

    TCanvas * c1 = new TCanvas("c1", "c1", 800, 600);

    TH2D * all_corr = new TH2D("all_corr", "all_corr;NClus (inner);NClus (outer)", 200, 0, 5000, 200, 0, 5000);
    TH2D * all_corr_MBD_INTT = new TH2D("all_corr_MBD_INTT", "all_corr_MBD_INTT;INTT NClus; MBD charge sum", 200, 0, 9000, 200, 0, 2800);
    TH1D * all_MBDCentrality = new TH1D("all_MBDCentrality", "all_MBDCentrality;MBD centrality;Entries", 101, -0.005, 1.005);

    TH2D * PostBcoSelection_corr = new TH2D("PostBcoSelection_corr", "PostBcoSelection_corr;NClus (inner);NClus (outer)", 200, 0, 5000, 200, 0, 5000);
    TH2D * PostBcoSelection_corr_MBD_INTT = new TH2D("PostBcoSelection_corr_MBD_INTT", "PostBcoSelection_corr_MBD_INTT;INTT NClus; MBD charge sum", 200, 0, 9000, 200, 0, 2800);
    TH1D * PostBcoSelection_MBDCentrality = new TH1D("PostBcoSelection_MBDCentrality", "PostBcoSelection_MBDCentrality;MBD centrality;Entries", 101, -0.005, 1.005);
    TH1D * PostBcoSelection_ClusPhiSize = new TH1D("PostBcoSelection_ClusPhiSize", "PostBcoSelection_ClusPhiSize;ClusPhiSize;Entries", 128,0,128);


    TH1D * MBDChargeSum_KilledEvt = new TH1D("MBDChargeSum_KilledEvt", "MBDChargeSum_KilledEvt;MBD charge sum;Entries", 200, 0, 2800);
    TH1D * INTTNClus_KilledEvt = new TH1D("INTTNClus_KilledEvt", "INTTNClus_KilledEvt;INTT NClus;Entries", 200, 0, 9000);
    TH1D * MBDCentrality_KilledEvt = new TH1D("MBDCentrality_KilledEvt", "MBDCentrality_KilledEvt;MBD centrality;Entries", 101, -0.005, 1.005);
    TH1D * MBDZVtx_KilledEvt = new TH1D("MBDZVtx_KilledEvt", "MBDZVtx_KilledEvt;MBD z vtx [cm];Entries", 100, -25, 25);

    TH2D * corr_KilledEvt = new TH2D("corr_KilledEvt", "corr_KilledEvt;NClus (inner);NClus (outer)", 200, 0, 5000, 200, 0, 5000);
    TH2D * corr_MBD_INTT_KilledEvt = new TH2D("corr_MBD_INTT_KilledEvt", "corr_MBD_INTT_KilledEvt;INTT NClus; MBD charge sum", 200, 0, 9000, 200, 0, 2800);

    TH1D * ClusPhiSize_KilledEvt = new TH1D("ClusPhiSize_KilledEvt", "ClusPhiSize_KilledEvt;ClusPhiSize;Entries", 128,0,128);

    std::vector<EvtProfile> evtProfileVec; evtProfileVec.clear();
    for (int i = 0; i < tree_in -> GetEntries(); i++)
    {
        tree_in -> GetEntry(i);
        EvtProfile evtProfile;
        evtProfile.NClus = NClus;
        evtProfile.NClus_Layer1 = NClus_Layer1;
        evtProfile.MBD_charge_sum = MBD_charge_sum;
        evtProfile.MBD_z_vtx = MBD_z_vtx;
        evtProfile.MBD_centrality = MBD_centrality;
        evtProfile.INTT_BCO = INTT_BCO;
        evtProfile.ClusPhiSize = *(ClusPhiSize);

        if (i != tree_in -> GetEntries() - 1){
            tree_in -> GetEntry(i+1);
            evtProfile.next_INTT_BCO = INTT_BCO;
            tree_in -> GetEntry(i);
        }
        else{
            evtProfile.next_INTT_BCO = -999;
        }

        evtProfileVec.push_back(evtProfile);
    }

    for (int i = 0; i < evtProfileVec.size(); i++)
    {
        EvtProfile evtProfile = evtProfileVec[i];
        int NClus_outer = evtProfile.NClus - evtProfile.NClus_Layer1;

        if (evtProfile.MBD_z_vtx != evtProfile.MBD_z_vtx) {continue;}
        if (evtProfile.MBD_z_vtx < -10 || evtProfile.MBD_z_vtx > 10) {continue;}
        if (i == evtProfileVec.size() - 1) {continue;}


        all_corr -> Fill(evtProfile.NClus_Layer1, NClus_outer);
        all_corr_MBD_INTT -> Fill(evtProfile.NClus, evtProfile.MBD_charge_sum);
        all_MBDCentrality -> Fill(evtProfile.MBD_centrality);

        if ((evtProfile.next_INTT_BCO - evtProfile.INTT_BCO) <= BcoFullDiff_cut){
            MBDChargeSum_KilledEvt -> Fill(evtProfile.MBD_charge_sum);
            INTTNClus_KilledEvt -> Fill(evtProfile.NClus);
            MBDCentrality_KilledEvt -> Fill(evtProfile.MBD_centrality);
            MBDZVtx_KilledEvt -> Fill(evtProfile.MBD_z_vtx);

            corr_KilledEvt -> Fill(evtProfile.NClus_Layer1, NClus_outer);
            corr_MBD_INTT_KilledEvt -> Fill(evtProfile.NClus, evtProfile.MBD_charge_sum);

            for (int j = 0; j < evtProfile.ClusPhiSize.size(); j++)
            {
                ClusPhiSize_KilledEvt -> Fill(evtProfile.ClusPhiSize[j]);
            }
        }
        else{
            PostBcoSelection_corr -> Fill(evtProfile.NClus_Layer1, NClus_outer);
            PostBcoSelection_corr_MBD_INTT -> Fill(evtProfile.NClus, evtProfile.MBD_charge_sum);
            PostBcoSelection_MBDCentrality -> Fill(evtProfile.MBD_centrality);

            for (int j = 0; j < evtProfile.ClusPhiSize.size(); j++)
            {
                PostBcoSelection_ClusPhiSize -> Fill(evtProfile.ClusPhiSize[j]);
            }
        }
    }

    c1 -> cd();
    c1 -> SetLogz(1);
    all_corr -> Draw("colz0");
    c1 -> Print(Form("%s/all_corr.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    c1 -> cd();
    all_corr_MBD_INTT -> Draw("colz0");
    c1 -> Print(Form("%s/all_corr_MBD_INTT.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    c1 -> cd();
    PostBcoSelection_corr -> Draw("colz0");
    c1 -> Print(Form("%s/PostBcoSelection_corr.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    c1 -> cd();
    PostBcoSelection_corr_MBD_INTT -> Draw("colz0");
    c1 -> Print(Form("%s/PostBcoSelection_corr_MBD_INTT.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    
    c1 -> cd();
    corr_KilledEvt -> Draw("colz0");
    c1 -> Print(Form("%s/corr_KilledEvt.pdf",output_folder_name.c_str()));
    c1 ->Clear();
    
    c1 -> cd();
    corr_MBD_INTT_KilledEvt -> Draw("colz0");
    c1 -> Print(Form("%s/corr_MBD_INTT_KilledEvt.pdf",output_folder_name.c_str()));
    c1 ->Clear();

    c1 -> SetLogz(0);
    c1 -> SetLogy(1);

    c1 -> cd();
    INTTNClus_KilledEvt -> Draw("hist");
    c1 -> Print(Form("%s/INTTNClus_KilledEvt.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    c1 -> cd();
    MBDChargeSum_KilledEvt -> Draw("hist");
    c1 -> Print(Form("%s/MBDChargeSum_KilledEvt.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    c1 -> SetLogy(0);

    c1 -> cd();
    MBDCentrality_KilledEvt -> Draw("hist");
    c1 -> Print(Form("%s/MBDCentrality_KilledEvt.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    c1 -> cd();
    MBDZVtx_KilledEvt -> Draw("hist");
    c1 -> Print(Form("%s/MBDZVtx_KilledEvt.pdf", output_folder_name.c_str()));
    c1 -> Clear();


    c1 -> cd();
    all_MBDCentrality -> Draw("hist");
    c1 -> Print(Form("%s/all_MBDCentrality.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    c1 -> cd();
    PostBcoSelection_MBDCentrality -> Draw("hist");
    c1 -> Print(Form("%s/PostBcoSelection_MBDCentrality.pdf", output_folder_name.c_str()));
    c1 -> Clear();


    c1 -> cd();
    c1 -> SetLogy(1);
    PostBcoSelection_ClusPhiSize -> Draw("hist");
    c1 -> Print(Form("%s/PostBcoSelection_ClusPhiSize.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    c1 -> cd();
    c1 -> SetLogy(1);
    ClusPhiSize_KilledEvt -> Draw("hist");
    c1 -> Print(Form("%s/ClusPhiSize_KilledEvt.pdf", output_folder_name.c_str()));
    c1 -> Clear();

    TFile * file_out = new TFile(Form("%s/evtRejection.root", output_folder_name.c_str()), "recreate");
    file_out -> cd();
    
    all_corr -> Write();
    all_corr_MBD_INTT -> Write();
    PostBcoSelection_corr -> Write();
    PostBcoSelection_corr_MBD_INTT -> Write();
    corr_KilledEvt -> Write();
    corr_MBD_INTT_KilledEvt -> Write();
    INTTNClus_KilledEvt -> Write();
    MBDChargeSum_KilledEvt -> Write();
    MBDCentrality_KilledEvt -> Write();
    MBDZVtx_KilledEvt -> Write();
    all_MBDCentrality -> Write();
    PostBcoSelection_MBDCentrality -> Write();
    PostBcoSelection_ClusPhiSize -> Write();
    ClusPhiSize_KilledEvt -> Write();

    file_out -> Close();

    return 0;
}
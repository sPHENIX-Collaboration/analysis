int BkgEstimation()
{
    TFile * file_in = TFile::Open("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/bco_diff/completed/BcoDiffNtuple_54280_clonehitremoveBCO_hotchannelremove_hitQA_checkclonehit_merged.root");

    std::pair<int, int> dead_region = {27, 54};
    long long  trigger_peak = 56;
 
    TTree * tree = (TTree *)file_in->Get("tree");
    int nentries = tree->GetEntries();
    std::cout << "nentries = " << nentries << std::endl;

    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("MBDNS_tight_vtx30cm", 1);
    tree->SetBranchStatus("felix0_bcodiff", 1);
    tree->SetBranchStatus("felix1_bcodiff", 1);
    tree->SetBranchStatus("felix2_bcodiff", 1);
    tree->SetBranchStatus("felix3_bcodiff", 1);
    tree->SetBranchStatus("felix4_bcodiff", 1);
    tree->SetBranchStatus("felix5_bcodiff", 1);
    tree->SetBranchStatus("felix6_bcodiff", 1);
    tree->SetBranchStatus("felix7_bcodiff", 1);

    int in_MBDNS_tight_vtx30cm;
    std::vector<int> * in_felix0_bcodiff = 0;
    std::vector<int> * in_felix1_bcodiff = 0;
    std::vector<int> * in_felix2_bcodiff = 0;
    std::vector<int> * in_felix3_bcodiff = 0;
    std::vector<int> * in_felix4_bcodiff = 0;
    std::vector<int> * in_felix5_bcodiff = 0;
    std::vector<int> * in_felix6_bcodiff = 0;
    std::vector<int> * in_felix7_bcodiff = 0;

    tree->SetBranchAddress("MBDNS_tight_vtx30cm", &in_MBDNS_tight_vtx30cm);
    tree->SetBranchAddress("felix0_bcodiff", &in_felix0_bcodiff);
    tree->SetBranchAddress("felix1_bcodiff", &in_felix1_bcodiff);
    tree->SetBranchAddress("felix2_bcodiff", &in_felix2_bcodiff);
    tree->SetBranchAddress("felix3_bcodiff", &in_felix3_bcodiff);
    tree->SetBranchAddress("felix4_bcodiff", &in_felix4_bcodiff);
    tree->SetBranchAddress("felix5_bcodiff", &in_felix5_bcodiff);
    tree->SetBranchAddress("felix6_bcodiff", &in_felix6_bcodiff);
    tree->SetBranchAddress("felix7_bcodiff", &in_felix7_bcodiff);


    long long selected_event = 0;
    std::vector<TH1D *> all_felix_BcoDiffHist;

    for (int i = 0; i < 8; i++)
    {
        // all_felix_BcoDiffHist.push_back((TH1D *)file_in->Get(Form("all_felix_BcoDiffHist_%d", i)));
        all_felix_BcoDiffHist.push_back(new TH1D(Form("all_felix_BcoDiffHist_%d", i), Form("all_felix_BcoDiffHist_%d;Time_bucket;Entries", i), 128,0,128));
    }

    for (long long i = 0; i < nentries; i++){
        tree -> GetEntry(i);

        if (i % 10000 == 0) {std::cout << "i = " << i << std::endl;}

        if (in_MBDNS_tight_vtx30cm != 1) {continue;}

        selected_event += 1;

        for (int j = 0; j < 128; j++){
            all_felix_BcoDiffHist[0]->Fill(j, in_felix0_bcodiff->at(j));
            all_felix_BcoDiffHist[1]->Fill(j, in_felix1_bcodiff->at(j));
            all_felix_BcoDiffHist[2]->Fill(j, in_felix2_bcodiff->at(j));
            all_felix_BcoDiffHist[3]->Fill(j, in_felix3_bcodiff->at(j));
            all_felix_BcoDiffHist[4]->Fill(j, in_felix4_bcodiff->at(j));
            all_felix_BcoDiffHist[5]->Fill(j, in_felix5_bcodiff->at(j));
            all_felix_BcoDiffHist[6]->Fill(j, in_felix6_bcodiff->at(j));
            all_felix_BcoDiffHist[7]->Fill(j, in_felix7_bcodiff->at(j));
        }
    }

    std::cout << "selected_event = " << selected_event << std::endl;

    TFile * file_out = TFile::Open("BcoDiff_Hist_MBDNS30cm.root", "recreate");
    for (int i = 0; i < all_felix_BcoDiffHist.size(); i++)
    {
        all_felix_BcoDiffHist[i]->Write();
    }
    file_out->Close();

    // std::vector<TH1D *> selected_felix_BcoDiffHist;
    // std::vector<TH1D * > bkg_count_vec; bkg_count_vec.clear();
    
    // std::vector<TH1D *> selected_felix_BcoDiffHist_ratio;
    // std::vector<TH1D * > bkg_count_vec_ratio; bkg_count_vec_ratio.clear();

    // for (TH1D * hist : all_felix_BcoDiffHist)
    // {
    //     selected_felix_BcoDiffHist.push_back((TH1D *)hist->Clone(Form("selected_%s", hist->GetName())));

    //     bkg_count_vec.push_back(new TH1D(Form("BkgCount_%s",hist->GetName()),Form("BkgCount_%s;NInttRawHits (bkg);Entries",hist->GetName()),200,0.1 * pow(10,6), 5 * pow(10,6)));
    //     bkg_count_vec_ratio.push_back(new TH1D(Form("BkgCount_%s_ratio",hist->GetName()),Form("BkgCount_%s_ratio;NInttRawHits (per Bkg. Evt.);Entries",hist->GetName()),200,0.1 * pow(10,6) / double (nentries), 5 * pow(10,6) / double (nentries)));

    //     for (int i = 0; i < selected_felix_BcoDiffHist.back()->GetNbinsX(); i++)
    //     {
    //         if (i+1 >= dead_region.first && i+1 <= dead_region.second) // note : dead region
    //         {
    //             selected_felix_BcoDiffHist.back()->SetBinContent(i + 1, 0);
    //         }

    //         if (i+1 >= trigger_peak - 1 && i+1 <= trigger_peak + 1) // note : trigger peak
    //         {
    //             selected_felix_BcoDiffHist.back()->SetBinContent(i + 1, 0);
    //         }

    //         if ( (i + 1) % 2 != 0)
    //         {
    //             selected_felix_BcoDiffHist.back()->SetBinContent(i + 1, 0);
    //         }

    //         if (selected_felix_BcoDiffHist.back()->GetBinContent(i + 1) > 0)
    //         {
    //             bkg_count_vec.back()->Fill(selected_felix_BcoDiffHist.back()->GetBinContent(i + 1));
    //         }

    //     }

    //     selected_felix_BcoDiffHist_ratio.push_back((TH1D *)selected_felix_BcoDiffHist.back()->Clone(Form("selected_%s_ratio", hist->GetName())));
    //     selected_felix_BcoDiffHist_ratio.back()->Scale(1.0 / double(selected_event));

    //     for (int j = 0; j < selected_felix_BcoDiffHist_ratio.back()->GetNbinsX(); j++)
    //     {
    //         if (j+1 >= dead_region.first && j+1 <= dead_region.second) // note : dead region
    //         {
    //             selected_felix_BcoDiffHist_ratio.back()->SetBinContent(j + 1, 0);
    //         }
    //     }
    // }

    


    // TFile * file_out = TFile::Open("BkgEstimation.root", "recreate");
    // for (int i = 0; i < all_felix_BcoDiffHist.size(); i++)
    // {
    //     all_felix_BcoDiffHist[i]->Write();
    // }

    // for (int i = 0; i < selected_felix_BcoDiffHist.size(); i++)
    // {
    //     selected_felix_BcoDiffHist[i]->Write();
    // }

    // THStack * hstack1D_total_bkg_count = new THStack("hstack1D_total_bkg_count", "hstack1D_total_bkg_count;NInttRawHits (bkg);Entries");

    // for (int i = 0; i < bkg_count_vec.size(); i++)
    // {
    //     bkg_count_vec[i]->Write();
    //     bkg_count_vec[i]->SetFillColor(i + 1);
    //     hstack1D_total_bkg_count->Add(bkg_count_vec[i]);
    // }

    // hstack1D_total_bkg_count->Write();
    // ((TH1D*)hstack1D_total_bkg_count->GetStack()->Last())->Write("h1D_total_bkg_count");



    // file_out->Close();

    return 0;
}
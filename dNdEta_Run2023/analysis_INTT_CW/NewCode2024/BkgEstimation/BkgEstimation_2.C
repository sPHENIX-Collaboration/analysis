int BkgEstimation_2()
{
    TFile * file_in = TFile::Open("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/bco_diff/completed/BcoDiffNtuple_54280_clonehitremoveBCO_hotchannelremove_hitQA_checkclonehit_merged.root");

    std::pair<int, int> dead_region = {27, 54};
    long long  trigger_peak = 56;
    long long selected_event = 1683940;
  
    std::vector<TH1D *> all_felix_BcoDiffHist; all_felix_BcoDiffHist.clear();

    for (int i = 0; i < 8; i++)
    {
        all_felix_BcoDiffHist.push_back((TH1D *)file_in->Get(Form("all_felix_BcoDiffHist_%d", i)));
        // all_felix_BcoDiffHist.push_back(new TH1D(Form("all_felix_BcoDiffHist_%d", i), Form("all_felix_BcoDiffHist_%d;Time_bucket;Entries", i), 128,0,128));
    }


    std::cout << "selected_event = " << selected_event << std::endl;

    std::vector<TH1D *> selected_felix_BcoDiffHist;
    THStack * hstack1D_total_bcodiff = new THStack("hstack1D_total_bcodiff", "hstack1D_total_bcodiff;Time_bucket;Entries");
    
    std::vector<TH1D *> selected_felix_BcoDiffHist_ratio;
    THStack * hstack1D_total_bcodiff_ratio = new THStack("hstack1D_total_bcodiff_ratio", "hstack1D_total_bcodiff_ratio;Time_bucket;Entries");

    for (TH1D * hist : all_felix_BcoDiffHist)
    {
        selected_felix_BcoDiffHist.push_back((TH1D *)hist->Clone(Form("selected_%s", hist->GetName())));

        

        for (int i = 0; i < selected_felix_BcoDiffHist.back()->GetNbinsX(); i++)
        {
            if (i+1 >= dead_region.first && i+1 <= dead_region.second) // note : dead region
            {
                selected_felix_BcoDiffHist.back()->SetBinContent(i + 1, 0);
            }

            if (i+1 >= trigger_peak - 1 && i+1 <= trigger_peak + 1) // note : trigger peak
            {
                selected_felix_BcoDiffHist.back()->SetBinContent(i + 1, 0);
            }

            if ( (i + 1) % 2 != 0)
            {
                selected_felix_BcoDiffHist.back()->SetBinContent(i + 1, 0);
            }
        }
        
        selected_felix_BcoDiffHist.back()->SetFillColor(selected_felix_BcoDiffHist.size() + 1);
        hstack1D_total_bcodiff->Add(selected_felix_BcoDiffHist.back());

        selected_felix_BcoDiffHist_ratio.push_back((TH1D *)selected_felix_BcoDiffHist.back()->Clone(Form("selected_%s_ratio", hist->GetName())));
        selected_felix_BcoDiffHist_ratio.back()->Scale(1.0 / double(selected_event));
        selected_felix_BcoDiffHist_ratio.back()->SetFillColor(selected_felix_BcoDiffHist_ratio.size() + 1);
        hstack1D_total_bcodiff_ratio->Add(selected_felix_BcoDiffHist_ratio.back());
    }

    TH1D * h1D_total_bcodiff = (TH1D *)((TH1D *)hstack1D_total_bcodiff->GetStack()->Last())->Clone("h1D_total_bcodiff");
    h1D_total_bcodiff -> SetFillColorAlpha(1,0);

    TH1D * h1D_total_bcodiff_ratio = (TH1D *)((TH1D *)hstack1D_total_bcodiff_ratio->GetStack()->Last())->Clone("h1D_total_bcodiff_ratio");
    h1D_total_bcodiff_ratio -> SetFillColorAlpha(1,0);


    TH1D * BkgCount_total_bcodiff = new TH1D(Form("BkgCount_total_bcodiff"),Form("BkgCount_total_bcodiff;NInttRawHits (all bkg crossings);Entries"),200,0.1 * pow(10,6), 5 * 8 * pow(10,6));
    TH1D * BkgCount_total_bcodiff_ratio = new TH1D(Form("BkgCount_total_bcodiff_ratio"),Form("BkgCount_total_bcodiff_ratio;NInttRawHits (per Bkg. Evt.);Entries"),200,0.1 * pow(10,6) / double (selected_event), 8 * 5 * pow(10,6) / double (selected_event));

    for (int i = 0; i < h1D_total_bcodiff->GetNbinsX(); i++){
        
        if (h1D_total_bcodiff->GetBinContent(i+1) == 0) continue;

        BkgCount_total_bcodiff -> Fill(h1D_total_bcodiff->GetBinContent(i+1));
        BkgCount_total_bcodiff_ratio -> Fill(h1D_total_bcodiff_ratio->GetBinContent(i+1));
    }

    TFile * file_out = TFile::Open("BkgEstimation.root", "recreate");
    for(TH1D * hist : all_felix_BcoDiffHist)
    {
        hist->Write();
    }

    for (TH1D * hist : selected_felix_BcoDiffHist)
    {
        hist->Write();
    }
    for (TH1D * hist : selected_felix_BcoDiffHist_ratio)
    {
        hist->Write();
    }

    hstack1D_total_bcodiff->Write();
    h1D_total_bcodiff->Write();

    hstack1D_total_bcodiff_ratio->Write();
    h1D_total_bcodiff_ratio->Write();

    BkgCount_total_bcodiff->Write();
    BkgCount_total_bcodiff_ratio->Write();

    file_out->Close();

    return 0;
}
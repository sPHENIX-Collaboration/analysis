#include "sPhenixStyle.C"
#include "../../ReadF4ANtupleData/ReadF4ANtupleData.C"

void bco_full_diff()
{
    SetsPhenixStyle();
    string mother_folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey";
    string file_name = "Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root";
    TFile * file_in = TFile::Open(Form("%s/%s",mother_folder_directory.c_str(),file_name.c_str()));
    TTree * tree = (TTree *)file_in->Get("EventTree");
    long long N_event = tree -> GetEntries();
    ReadF4ANtupleData * merge_tree = new ReadF4ANtupleData(tree);
    cout<<Form("N_event in file %s : %lli",file_name.c_str(), N_event)<<endl;

    // int fNhits;
    // int pid[100000];
    // int module[100000];
    // int chip_id[100000];
    // int chan_id[100000];
    // int adc[100000];
    // int bco[100000];
    // Long64_t bco_full[100000];

    
    // tree -> SetBranchStatus("*",0);
    // tree -> SetBranchStatus("fNhits",1);
    // tree -> SetBranchStatus("fhitArray.bco_full",1);

    // tree -> SetBranchAddress("fNhits",&fNhits);
    // tree -> GetEntry(0); // note : actually I really don't know why this line is necessary.
    // tree -> SetBranchAddress("fhitArray.bco_full",&bco_full[0]);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);


    Long64_t pre_bco_full;
    Long64_t this_bco_full;

    merge_tree -> LoadTree(0);
    merge_tree -> GetEntry(0);
    pre_bco_full = merge_tree -> INTT_BCO;

    TH1F * bco_full_diff_hist = new TH1F("bco_full_diff_hist","bco_full_diff_hist;BCO_FULL - BCO_FULL previous evt;Entry",200,0,200000);
    for (int i = 1; i < N_event; i++)
    {   
        merge_tree -> LoadTree(i);
        merge_tree -> GetEntry(i);

        if (i%5000 == 0) {cout<<Form("Processing event %i",i)<<endl;}

        bco_full_diff_hist -> Fill(merge_tree -> INTT_BCO - pre_bco_full);

        pre_bco_full = merge_tree -> INTT_BCO;
    }

    double count_entry = 0;
    int Nbin_considered = 8;
    for (int i = 1; i < Nbin_considered+1; i++)
    {
        count_entry += bco_full_diff_hist -> GetBinContent(i);
        cout<<Form("Bin %i : %.4f",i,bco_full_diff_hist -> GetBinContent(i))<<endl;
    }

    TCanvas * c1 = new TCanvas("c1","c1",950,800);
    c1 -> cd();
    bco_full_diff_hist -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    draw_text -> DrawLatex(0.3, 0.88, Form("Ratio of event w/ bco_full diff < %.0f BCO : %.4f", bco_full_diff_hist->GetBinCenter(1) + bco_full_diff_hist->GetBinWidth(1)/2., bco_full_diff_hist->GetBinContent(1)/bco_full_diff_hist -> GetEntries()));
    draw_text -> DrawLatex(0.3, 0.84, Form("Ratio of event w/ bco_full diff < %.0f BCO : %.4f", bco_full_diff_hist->GetBinCenter(Nbin_considered) + bco_full_diff_hist->GetBinWidth(Nbin_considered)/2., count_entry/bco_full_diff_hist -> GetEntries()));
    c1 -> Print(Form("%s/bco_full_diff_hist.pdf",mother_folder_directory.c_str()));

    cout<<"hist stdddev : "<<bco_full_diff_hist -> GetStdDev()<<endl;
    cout<<"hist mean : "<<bco_full_diff_hist -> GetMean()<<endl;
    

}
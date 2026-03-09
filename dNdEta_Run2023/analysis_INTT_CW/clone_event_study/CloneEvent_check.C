#include <iostream>
#include "ReadEventBase.C"

bool count_hist_remain(TH1F * hist_1, TH1F * hist_2)
{
    int N_remain = 0;
    for (int i = 0; i < hist_1 -> GetNbinsX(); i++) { N_remain += abs(hist_1 -> GetBinContent(i+1) - hist_2 -> GetBinContent(i+1)); }
    return (N_remain == 0 && hist_1 -> GetEntries() > 1 && hist_1 -> GetStdDev() != 0 && hist_1 -> GetMean() != 0 && (hist_1 -> GetEntries() - hist_1 -> GetBinContent(1)) > 10 /*hist_1 -> GetBinContent(1)/double(hist_1 -> GetEntries()) < 0.8*/) ? true : false;
}

void CloneEvent_check(int study_run_index)
{
    // int study_run_index = 4;
    vector<string> ZF_run_list = {"20864","20866","20867","20868","20869","20878","20880","20881","20883","20885"};
    string file_directory = "/sphenix/tg/tg01/commissioning/INTT/root_files";
    
    TFile * file_in;
    TTree * tree_in;
    ReadEventBase * data_in;

    int show_index = 0;

    int fNhits;
    int pid[100000];
    int module[100000];
    int chip_id[100000];
    int chan_id[100000];
    int adc[100000];
    int bco[100000];
    Long64_t bco_full[100000];

    TH1F * chan_dist_bco0 = new TH1F("","",128,0,128);
    TH1F * chan_dist_bco1 = new TH1F("","",128,0,128);
    TH1F * chan_dist_bco2 = new TH1F("","",128,0,128);
    TH1F * chan_dist_bco3 = new TH1F("","",128,0,128);
    TH1F * chan_dist_bco4 = new TH1F("","",128,0,128);
    TH1F * chan_dist_bco5 = new TH1F("","",128,0,128);

    for (int i = 0; i < 8; i++)
    {
        if ( gSystem->AccessPathName(Form("%s/beam_intt%i-000%s-0000_event_base.root", file_directory.c_str(), i, ZF_run_list[study_run_index].c_str())) == true  ) 
        {
            cout<<"file not exist: "<<Form("%s/beam_intt%i-000%s-0000_event_base.root", file_directory.c_str(), i, ZF_run_list[study_run_index].c_str())<<endl;
            continue;
        }
        file_in = TFile::Open(Form("%s/beam_intt%i-000%s-0000_event_base.root", file_directory.c_str(), i, ZF_run_list[study_run_index].c_str()));
        tree_in = (TTree*) file_in -> Get("tree");
        long long N_event = tree_in -> GetEntries();
        tree_in -> SetBranchStatus("*",0);
        tree_in -> SetBranchStatus("fNhits",1);
        // tree_in -> SetBranchStatus("fhitArray.pid",1);
        // tree_in -> SetBranchStatus("fhitArray.module",1);
        // tree_in -> SetBranchStatus("fhitArray.chip_id",1);
        tree_in -> SetBranchStatus("fhitArray.chan_id",1);
        // tree_in -> SetBranchStatus("fhitArray.adc",1);
        // tree_in -> SetBranchStatus("fhitArray.bco",1);
        // tree_in -> SetBranchStatus("fhitArray.bco_full",1);

        tree_in -> SetBranchAddress("fNhits",&fNhits);
        tree_in -> GetEntry(0); // note : actually I really don't know why this line is necessary.
        // tree_in -> SetBranchAddress("fhitArray.pid",&pid[0]);
        // tree_in -> SetBranchAddress("fhitArray.module",&module[0]);
        // tree_in -> SetBranchAddress("fhitArray.chip_id",&chip_id[0]);
        tree_in -> SetBranchAddress("fhitArray.chan_id",&chan_id[0]);
        // tree_in -> SetBranchAddress("fhitArray.adc",&adc[0]);
        // tree_in -> SetBranchAddress("fhitArray.bco",&bco[0]);
        // tree_in -> SetBranchAddress("fhitArray.bco_full",&bco_full[0]);



        // data_in = new ReadEventBase(tree_in);
        cout<<" "<<endl; 
        cout<<"check the run index: "<<ZF_run_list[study_run_index]<<endl;
        cout<<"checking the server ID: "<<i<<" N event : "<<N_event<<endl;

        double N_same_hit_evt_bco1 = 0;
        double N_same_hit_evt_bco2 = 0;
        double N_same_hit_evt_bco3 = 0;
        double N_same_hit_evt_bco4 = 0;
        double N_same_hit_evt_bco5 = 0;

        for (int event_i = 0; event_i < N_event; event_i++)
        {
            // if (event_i % 100 == 0) cout<<"event_i : "<<event_i<<endl;

            // data_in -> LoadTree(event_i);
            tree_in -> GetEntry(event_i);
            int fNhits_bco0 = fNhits;
            for (int hit_i = 0; hit_i < fNhits; hit_i++){
                chan_dist_bco0 -> Fill(chan_id[hit_i]);
            }
            
            // note : ------------------ ------------------ ------------------ ------------------ ------------------ ------------------
            if (event_i == (tree_in -> GetEntries() - 1)) {
                chan_dist_bco0 -> Reset("ICESM");
                chan_dist_bco1 -> Reset("ICESM");
                chan_dist_bco2 -> Reset("ICESM");
                chan_dist_bco3 -> Reset("ICESM");
                chan_dist_bco4 -> Reset("ICESM");
                chan_dist_bco5 -> Reset("ICESM");
                continue;
            }
            // data_in -> LoadTree(event_i+1);
            tree_in -> GetEntry(event_i+1);
            if (fNhits_bco0 == fNhits) { 
                // N_same_hit_evt_bco1 += 1; 
                for (int hit_i = 0; hit_i < fNhits; hit_i++) {chan_dist_bco1 -> Fill(chan_id[hit_i]);}
                if (count_hist_remain(chan_dist_bco0, chan_dist_bco1)){
                    N_same_hit_evt_bco1 += 1;
                    cout<<"--- identical fNhits and chan distribution in bco gap 1, event_i : "<<event_i<<" with fNhits: "<<fNhits<<endl;
                }

                
            }

            // note : ------------------ ------------------ ------------------ ------------------ ------------------ ------------------
            if (event_i == (tree_in -> GetEntries() - 2)) {
                chan_dist_bco0 -> Reset("ICESM");
                chan_dist_bco1 -> Reset("ICESM");
                chan_dist_bco2 -> Reset("ICESM");
                chan_dist_bco3 -> Reset("ICESM");
                chan_dist_bco4 -> Reset("ICESM");
                chan_dist_bco5 -> Reset("ICESM");
                continue;
            }
            // data_in -> LoadTree(event_i+2);
            tree_in -> GetEntry(event_i+2);
            if (fNhits_bco0 == fNhits) { 
                // N_same_hit_evt_bco2 += 1; 
                for (int hit_i = 0; hit_i < fNhits; hit_i++) {chan_dist_bco2 -> Fill(chan_id[hit_i]);}
                if (count_hist_remain(chan_dist_bco0, chan_dist_bco2)){
                    N_same_hit_evt_bco2 += 1;
                    cout<<"--- identical fNhits and chan distribution in bco gap 2, event_i : "<<event_i<<" with fNhits: "<<fNhits<<endl;
                }


            }

            // note : ------------------ ------------------ ------------------ ------------------ ------------------ ------------------
            if (event_i == (tree_in -> GetEntries() - 3)) {
                chan_dist_bco0 -> Reset("ICESM");
                chan_dist_bco1 -> Reset("ICESM");
                chan_dist_bco2 -> Reset("ICESM");
                chan_dist_bco3 -> Reset("ICESM");
                chan_dist_bco4 -> Reset("ICESM");
                chan_dist_bco5 -> Reset("ICESM");
                continue;
            }
            // data_in -> LoadTree(event_i+3);
            tree_in -> GetEntry(event_i+3);
            if (fNhits_bco0 == fNhits) { 
                // N_same_hit_evt_bco3 += 1; 
                for (int hit_i = 0; hit_i < fNhits; hit_i++) {chan_dist_bco3 -> Fill(chan_id[hit_i]);}
                if (count_hist_remain(chan_dist_bco0, chan_dist_bco3)){
                    N_same_hit_evt_bco3 += 1;
                    cout<<"--- identical fNhits and chan distribution in bco gap 3, event_i : "<<event_i<<" with fNhits: "<<fNhits<<endl;
                }


            }

            // note : ------------------ ------------------ ------------------ ------------------ ------------------ ------------------
            if (event_i == (tree_in -> GetEntries() - 4)) {
                chan_dist_bco0 -> Reset("ICESM");
                chan_dist_bco1 -> Reset("ICESM");
                chan_dist_bco2 -> Reset("ICESM");
                chan_dist_bco3 -> Reset("ICESM");
                chan_dist_bco4 -> Reset("ICESM");
                chan_dist_bco5 -> Reset("ICESM");
                continue;
            }
            // data_in -> LoadTree(event_i+4);
            tree_in -> GetEntry(event_i+4);
            if (fNhits_bco0 == fNhits) { 
                // N_same_hit_evt_bco4 += 1; 
                for (int hit_i = 0; hit_i < fNhits; hit_i++) {chan_dist_bco4 -> Fill(chan_id[hit_i]);}
                if (count_hist_remain(chan_dist_bco0, chan_dist_bco4)){
                    N_same_hit_evt_bco4 += 1;
                    cout<<"--- identical fNhits and chan distribution in bco gap 4, event_i : "<<event_i<<" with fNhits: "<<fNhits<<endl;
                }


            }

            // note : ------------------ ------------------ ------------------ ------------------ ------------------ ------------------
            if (event_i == (tree_in -> GetEntries() - 5)) {
                chan_dist_bco0 -> Reset("ICESM");
                chan_dist_bco1 -> Reset("ICESM");
                chan_dist_bco2 -> Reset("ICESM");
                chan_dist_bco3 -> Reset("ICESM");
                chan_dist_bco4 -> Reset("ICESM");
                chan_dist_bco5 -> Reset("ICESM");
                continue;
            }
            // data_in -> LoadTree(event_i+5);
            tree_in -> GetEntry(event_i+5);
            if (fNhits_bco0 == fNhits) { 
                // N_same_hit_evt_bco5 += 1; 
                for (int hit_i = 0; hit_i < fNhits; hit_i++) {chan_dist_bco5 -> Fill(chan_id[hit_i]);}
                if (count_hist_remain(chan_dist_bco0, chan_dist_bco5)){
                    N_same_hit_evt_bco5 += 1;
                    cout<<"--- identical fNhits and chan distribution in bco gap 5, event_i : "<<event_i<<" with fNhits: "<<fNhits<<endl;
                }


            }

            // note : ------------------ ------------------ ------------------ ------------------ ------------------ ------------------
            chan_dist_bco0 -> Reset("ICESM");
            chan_dist_bco1 -> Reset("ICESM");
            chan_dist_bco2 -> Reset("ICESM");
            chan_dist_bco3 -> Reset("ICESM");
            chan_dist_bco4 -> Reset("ICESM");
            chan_dist_bco5 -> Reset("ICESM");

            // cout<<"event_i : "<<event_i<<endl;
            // cout<<"fNhits : "<<data_in -> fNhits<<endl;
        }

        cout<<"N_same_hit_evt_bco1 : "<<N_same_hit_evt_bco1<<endl;
        cout<<"N_same_hit_evt_bco2 : "<<N_same_hit_evt_bco2<<endl;
        cout<<"N_same_hit_evt_bco3 : "<<N_same_hit_evt_bco3<<endl;
        cout<<"N_same_hit_evt_bco4 : "<<N_same_hit_evt_bco4<<endl;
        cout<<"N_same_hit_evt_bco5 : "<<N_same_hit_evt_bco5<<endl;

        file_in -> Close();
    }

}
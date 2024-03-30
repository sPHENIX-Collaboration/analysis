#include "TH2INTT.h"

struct chan_info{
    int pid;
    int module;
    int chip;
    int chan;
    int adc;
    // int entry;
};

struct to_ch_str{
    int pid;
    int module;
    int chip;
    int chan;
};

to_ch_str convertToIndices(int input) {
    const int dim1 = 8;
    const int dim2 = 14;
    const int dim3 = 26;
    const int dim4 = 128;

    to_ch_str result;
    result.chan = input % dim4;
    input /= dim4;
    result.chip = input % dim3;
    input /= dim3;
    result.module = input % dim2;
    input /= dim2;
    result.pid = input % dim1;

    return result;
}

void hot_chan_finder(string run_ID)
{   
    // string run_ID = "20864";
    string folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/Field_ON/" + run_ID;
    string file_name = "beam_inttall-000"+ run_ID +"-0000_event_base_ana";
    string output_directory = folder_directory + "/PreCheck_" + file_name;
    double standard_ch_ratio_typeA = 1. / (8*14*16*128); // note : typeA, 16 sensor cells
    double standard_ch_ratio_typeB = 1. / (8*14*10*128); // note : typeB, 10 sensor cells
    int criterion = 3;
    bool All_event_used = true;
    long long defined_event = 200000; // note : only the in the case that the All_event_used is false

    system(Form("mkdir %s",output_directory.c_str()));

    TFile * file_in = TFile::Open(Form("%s/%s.root",folder_directory.c_str(),file_name.c_str()));
    TTree * tree = (TTree *)file_in->Get("tree");
    long long N_event = (All_event_used == true) ? tree -> GetEntries() : defined_event; 
    cout<<Form("N_event in file %s : %lli",file_name.c_str(), N_event)<<endl;
    

    int fNhits;
    int pid[100000]; // todo : if the fNhits greater than 100000, it may not work
    int module[100000];
    int chip_id[100000];
    int chan_id[100000];
    int adc[100000];
    // int bco[100000];
    // Long64_t bco_full[100000];

    
    tree -> SetBranchStatus("*",0);
    tree -> SetBranchStatus("fNhits",1);
    tree -> SetBranchStatus("fhitArray.pid",1);
    tree -> SetBranchStatus("fhitArray.module",1);
    tree -> SetBranchStatus("fhitArray.chip_id",1);
    tree -> SetBranchStatus("fhitArray.chan_id",1);
    tree -> SetBranchStatus("fhitArray.adc",1);
    // tree -> SetBranchStatus("fhitArray.bco",1);
    // tree -> SetBranchStatus("fhitArray.bco_full",1);

    tree -> SetBranchAddress("fNhits",&fNhits);
    tree -> GetEntry(0); // note : actually I really don't know why this line is necessary.
    tree -> SetBranchAddress("fhitArray.pid",&pid[0]);
    tree -> SetBranchAddress("fhitArray.module",&module[0]);
    tree -> SetBranchAddress("fhitArray.chip_id",&chip_id[0]);
    tree -> SetBranchAddress("fhitArray.chan_id",&chan_id[0]);
    tree -> SetBranchAddress("fhitArray.adc",&adc[0]);
    // tree -> SetBranchAddress("fhitArray.bco",&bco[0]);
    // tree -> SetBranchAddress("fhitArray.bco_full",&bco_full[0]);

    // note : ch_hit[pid][module][chip][channel]
    int ch_adc[8][14][26][128]; memset(ch_adc, -1, sizeof(ch_adc));                              // note : to keep the channel adc, and find the clone hit (single event)
    long long ld_clone_all[8][14]; memset(ld_clone_all, 0, sizeof(ld_clone_all));                      // note : N clone hit in that half-ladder in whole file
    long long ld_hit_before_all[8][14]; memset(ld_hit_before_all, 0, sizeof(ld_hit_before_all));       // note : N hit in that half-ladder regardless the clone hit or not
    long long ld_hit_after_all[8][14]; memset(ld_hit_after_all, 0, sizeof(ld_hit_after_all));          // note : N hit after the clone-hit removal in the whole file
    long long ch_hit_after_all[8][14][26][128]; memset(ch_hit_after_all, 0, sizeof(ch_hit_after_all)); // note : N hit of each channel in this file
    long long hit_after_all_typeA = 0; // note : U6 to U13 & U19 to U26 
    long long hit_after_all_typeB = 0; // note : U1 to U5 & U14 to U18
    vector<long long>ch_hit_after_all_seq; ch_hit_after_all_seq.clear();
    vector<chan_info> event_hit_info_vec; event_hit_info_vec.clear();
    
    // int ch_hit[8][14][26][128]; memset(ch_hit, 0, sizeof(ch_hit));  // note : count how many hit the channels have 
    int bad_ch[8][14]; memset(bad_ch, 0, sizeof(bad_ch));
    
    // note : N clone hit in one event, the ratio (N_clone / fNhits) V
    // note : N Clone hit as function of ladders (along all the events) V
    // note : Nhit histogram comparison, before and after clone-hit removal V
    // note : Nhit after, as function of ladder (along all the events) V

    // note : hot channel, as function of ladder 

    TH1F * nHit_before = new TH1F("nHit_before","nHit_before",500,0,40000); // note : # of hit in each event before clone hit removal
    nHit_before -> SetLineColor(4);
    nHit_before -> GetXaxis() -> SetTitle("N hits");
    nHit_before -> GetYaxis() -> SetTitle("Entry");

    TH1F * nHit_after  = new TH1F("nHit_after","nHit_after",500,0,40000); // note : # of hits in each event after the clone hit removal
    nHit_after -> SetLineColor(2);
    nHit_after -> GetXaxis() -> SetTitle("N hits");
    nHit_after -> GetYaxis() -> SetTitle("Entry");
    
    TH1F * clone_hit_ratio  = new TH1F("clone_hit_ratio","clone_hit_ratio",200,0,1); // note : in each event, N_clone_hit / fNhits (it won't be 100%)
    clone_hit_ratio -> GetXaxis() -> SetTitle("Nclone_hit / Ntotal_hit");
    clone_hit_ratio -> GetYaxis() -> SetTitle("Entry");
    
    TH1F * NchHit_after  = new TH1F("NchHit_after","NchHit_after",1000,0,8000); // note : After hit removal, # of hit each channel has in the whole file
    NchHit_after -> GetXaxis() -> SetTitle("N channel hit");
    NchHit_after -> GetYaxis() -> SetTitle("Entry");

    TH1F * nHotCh_hist  = new TH1F("nHotCh_hist","nHotCh_hist",112,0,112); 
    nHotCh_hist -> GetXaxis() -> SetTitle("Half-ladder index");
    nHotCh_hist -> GetYaxis() -> SetTitle("Entry");

    TH2F * Nhit_correlation  = new TH2F("Nhit_correlation","Nhit_correlation",1000,0,20000,1000,0,20000); 
    Nhit_correlation -> GetXaxis() -> SetTitle("Before (fNhits)");
    Nhit_correlation -> GetYaxis() -> SetTitle("Post clone-hit removal");

    TH2F * Nhit_correlation_short  = new TH2F("Nhit_correlation_short","Nhit_correlation_short",1000,0,2000,1000,0,2000); 
    Nhit_correlation_short -> GetXaxis() -> SetTitle("Before (fNhits)");
    Nhit_correlation_short -> GetYaxis() -> SetTitle("Post clone-hit removal");

    TH2F * CHratio_Nhits_correlation  = new TH2F("CHratio_Nhits_correlation","CHratio_Nhits_correlation",1000,0,20000,100,0,1); 
    CHratio_Nhits_correlation -> GetXaxis() -> SetTitle("fNhits");
    CHratio_Nhits_correlation -> GetYaxis() -> SetTitle("Clone-hit ratio");

    TH2F * CHratio_index_correlation  = new TH2F("CHratio_index_correlation","CHratio_index_correlation",1000,0,200000,100,0,1); 
    CHratio_index_correlation -> GetXaxis() -> SetTitle("eID");
    CHratio_index_correlation -> GetYaxis() -> SetTitle("Clone-hit ratio");

    TH2F * post_nhits_index  = new TH2F("post_nhits_index","post_nhits_index",1000,0,200000,1000,0,35000); 
    post_nhits_index -> GetXaxis() -> SetTitle("eID");
    post_nhits_index -> GetYaxis() -> SetTitle("Clone-hit ratio");

    vector<double> high_ratio_index; high_ratio_index.clear();
    vector<double> high_ratio_y; high_ratio_y.clear();


    for (int i = 0; i < N_event; i++)
    {
        tree -> GetEntry(i);
        if (i % 1000 == 0) cout<<"running : "<<i<<endl;

        for (int i1 = 0; i1 < fNhits; i1++)
        {
            if (pid[i1] > 3000 && pid[i1] < 3009 && module[i1] > -1 && module[i1] < 14 && chip_id[i1] > 0 && chip_id[i1] < 27 && chan_id[i1] > -1 && chan_id[i1] < 128 && adc[i1] > -1 && adc[i1] < 8) {
                
                if (ch_adc[ pid[i1] - 3001 ][ module[i1] ][ chip_id[i1] - 1 ][ chan_id[i1] ] == -1) // note : not clone
                {
                    ch_adc[ pid[i1] - 3001 ][ module[i1] ][ chip_id[i1] - 1 ][ chan_id[i1] ] = adc[i1];
                }
                else 
                {
                    ld_clone_all[ pid[i1] - 3001 ][ module[i1] ] += 1; // note : it's clone hit
                    ch_adc[ pid[i1] - 3001 ][ module[i1] ][ chip_id[i1] - 1 ][ chan_id[i1] ] = adc[i1]; //  note : the last one is used
                }
                
                ld_hit_before_all[ pid[i1] - 3001 ][ module[i1] ] += 1; 

            }
        }

        for (int i0 = 0; i0 < 8; i0++) // note : pid
        {
            for (int i1 = 0; i1 < 14; i1++) // note : module
            {
                for (int i2 = 0; i2 < 26; i2++) // note : chip - 1
                {
                    for (int i3 = 0; i3 < 128; i3++) // note : channel
                    {
                        if ( ch_adc[i0][i1][i2][i3] != -1 ){
                            event_hit_info_vec.push_back({i0, i1, i2+1, i3, ch_adc[i0][i1][i2][i3]});
                            ld_hit_after_all[i0][i1] += 1;
                            ch_hit_after_all[i0][i1][i2][i3] += 1;
                            
                            // note : type B
                            if ((i2 >= 0 && i2 <= 4) || ( i2 >= 13 && i2 <= 17) ) { hit_after_all_typeB += 1; }
                            else { hit_after_all_typeA += 1; } // note : type A                           
                        }
                        
                    }
                }
            }
        }
        if (fNhits != 0)
        {
            nHit_before -> Fill(fNhits);
            nHit_after  -> Fill(event_hit_info_vec.size());
            clone_hit_ratio -> Fill( (fNhits - event_hit_info_vec.size()) / double(fNhits) );
            Nhit_correlation -> Fill(fNhits, event_hit_info_vec.size());
            Nhit_correlation_short -> Fill(fNhits, event_hit_info_vec.size());
            post_nhits_index -> Fill(i, event_hit_info_vec.size());
            CHratio_index_correlation -> Fill(i,(fNhits - event_hit_info_vec.size()) / double(fNhits));

            CHratio_Nhits_correlation -> Fill(fNhits, (fNhits - event_hit_info_vec.size()) / double(fNhits));
            if ( (fNhits - event_hit_info_vec.size()) / double(fNhits) > 0.7 ) {
                cout<<"high clone hit ratio, fNhits : "<<fNhits<<" post : "<<event_hit_info_vec.size()<<endl;
                
                high_ratio_index.push_back(high_ratio_index.size() + 1);
                high_ratio_y.push_back(fNhits);
            }

            
        }
        
        

        memset(ch_adc, -1, sizeof(ch_adc));
        // memset(ld_clone_all, 0, sizeof(ld_clone_all)); // note : N_clone_hit along all the events.
        event_hit_info_vec.clear();
        
    } // note : for loop, end of event

    TFile * out_file = new TFile( Form("%s/hot_ch_map_criterion_%i.root",output_directory.c_str(),criterion), "RECREATE" );
    TTree * tree_out =  new TTree ("tree", "hot channel info.");
    int pid_out, module_out, chip_id_out, chan_id_out;
    int Nhit_out;
    double ratio_out;
    tree_out -> Branch("pid",&pid_out);
    tree_out -> Branch("module",&module_out);
    tree_out -> Branch("chip_id",&chip_id_out);
    tree_out -> Branch("chan_id",&chan_id_out);
    tree_out -> Branch("NchHit",&Nhit_out);
    tree_out -> Branch("ratio",&ratio_out);
    tree_out -> Branch("typeA_Nhit",&hit_after_all_typeA);
    tree_out -> Branch("typeB_Nhit",&hit_after_all_typeB);

    TCanvas * c1 = new TCanvas("","",900,800);
    gPad->SetTopMargin(0.07);
    gPad->SetBottomMargin(0.12);
    gPad->SetLeftMargin(0.14);
    gPad->SetRightMargin(0.14);
    c1 -> cd();
    c1 -> SetLogy(0);

    TCanvas * c3 = new TCanvas("","",800,800);
    c3 -> cd();
    gPad->SetTopMargin(0.07);
    gPad->SetBottomMargin(0.12);
    gPad->SetLeftMargin(0.14);
    gPad->SetRightMargin(0.14);
    c3 -> SetLogy(0);


    gStyle->SetOptStat(10);
    TH2F * HL_hotch_map = new TH2F("","",128,0,128,30,0,30);
    // HL_hotch_map -> SetStats(0);

    TH2F * hotch_map = new TH2F("","",128,0,128,30,0,30);
    // hotch_map -> SetStats(0);

    TH2F * chhit_map = new TH2F("","",128,0,128,26,1,27);
    // hotch_map -> SetStats(0);

    c1 -> cd();
    c1 -> Print(Form("%s/bad_ch_distribution_Cut_%i.pdf(",output_directory.c_str(), criterion));
    c1 -> Clear();

    c3 -> cd();
    c3 -> Print(Form("%s/N_ch_hit_2D.pdf(",output_directory.c_str()));
    c3 -> Clear();

    for (int i0 = 0; i0 < 8; i0++) { // note : pid
        for (int i1 = 0; i1 < 14; i1++) { // note : module
            for (int i2 = 0; i2 < 26; i2++) { // note : chip - 1
                for (int i3 = 0; i3 < 128; i3++) { // note : channel{
                    NchHit_after -> Fill(ch_hit_after_all[i0][i1][i2][i3]);
                    ch_hit_after_all_seq.push_back( ch_hit_after_all[i0][i1][i2][i3] );

                    chhit_map -> SetBinContent(i3+1,i2+1,ch_hit_after_all[i0][i1][i2][i3]);

                    // note : type B
                    if ((i2 >= 0 && i2 <= 4) || ( i2 >= 13 && i2 <= 17) ) { 
                        if ( ((ch_hit_after_all[i0][i1][i2][i3] / double(hit_after_all_typeB)) / standard_ch_ratio_typeB) > criterion ){
                            bad_ch[i0][i1] += 1;
                            cout<<"type B hot channel, ratio : "<<((ch_hit_after_all[i0][i1][i2][i3] / double(hit_after_all_typeB)) / standard_ch_ratio_typeB)<<", Nhit : "<<ch_hit_after_all[i0][i1][i2][i3]<<", location : "<<i0<<" "<<i1<<" "<<i2+1<<" "<<i3<<endl;
                            
                            pid_out = i0 + 3001;
                            module_out = i1;
                            chip_id_out = i2 + 1;
                            chan_id_out = i3;
                            Nhit_out = ch_hit_after_all[i0][i1][i2][i3];
                            ratio_out = ((ch_hit_after_all[i0][i1][i2][i3] / double(hit_after_all_typeB)) / standard_ch_ratio_typeB);
                            tree_out -> Fill();

                            HL_hotch_map -> Fill(i3,i2+1);
                            hotch_map -> Fill(i3,i2+1);

                        }
                    }
                    else { // note : type A 
                        if ( ((ch_hit_after_all[i0][i1][i2][i3] / double(hit_after_all_typeA)) / standard_ch_ratio_typeA) > criterion ){
                            bad_ch[i0][i1] += 1;
                            cout<<"type A hot channel, ratio : "<<((ch_hit_after_all[i0][i1][i2][i3] / double(hit_after_all_typeA)) / standard_ch_ratio_typeA)<<", Nhit : "<<ch_hit_after_all[i0][i1][i2][i3]<<", location : "<<i0<<" "<<i1<<" "<<i2+1<<" "<<i3<<endl;

                            pid_out = i0 + 3001;
                            module_out = i1;
                            chip_id_out = i2 + 1;
                            chan_id_out = i3;
                            Nhit_out = ch_hit_after_all[i0][i1][i2][i3];
                            ratio_out = ((ch_hit_after_all[i0][i1][i2][i3] / double(hit_after_all_typeA)) / standard_ch_ratio_typeA);
                            tree_out -> Fill();

                            HL_hotch_map -> Fill(i3,i2+1);
                            hotch_map -> Fill(i3,i2+1);
                        }
                    } 
                }
            }

            c3 -> cd();
            chhit_map -> SetTitle(Form("intt%i_%i",i0,i1));
            chhit_map -> Draw("colz0");
            c3 -> Print(Form("%s/N_ch_hit_2D.pdf",output_directory.c_str()));
            c3 -> Clear();

            c1 -> cd();
            HL_hotch_map -> SetTitle(Form("intt%i_%i",i0,i1));
            HL_hotch_map -> Draw("colz0");
            c1 -> Print(Form("%s/bad_ch_distribution_Cut_%i.pdf",output_directory.c_str(), criterion));
            c1 -> Clear();        
            HL_hotch_map -> Reset("ICESM");
            chhit_map -> Reset("ICESM");
        }
    }

    c3 -> cd();
    c3 -> Print(Form("%s/N_ch_hit_2D.pdf)",output_directory.c_str()));
    c3 -> Clear();

    c1 -> cd();
    c1 -> Print(Form("%s/bad_ch_distribution_Cut_%i.pdf)",output_directory.c_str(), criterion));
    c1 -> Clear();
    c1 -> cd();

    int n_size = 8 * 14 * 26 * 128;
    int sort_hit_index[n_size];
    TMath::Sort(n_size, &ch_hit_after_all_seq[0], sort_hit_index);

    // cout<<"test N event : "<<ch_hit_after_all[convertToIndices(sort_hit_index[0]).pid][convertToIndices(sort_hit_index[0]).module][convertToIndices(sort_hit_index[0]).chip][convertToIndices(sort_hit_index[0]).chan]<<endl;
    // cout<<"test N event : "<<ch_hit_after_all[convertToIndices(sort_hit_index[1]).pid][convertToIndices(sort_hit_index[1]).module][convertToIndices(sort_hit_index[1]).chip][convertToIndices(sort_hit_index[1]).chan]<<endl;
    // cout<<"test N event : "<<ch_hit_after_all[convertToIndices(sort_hit_index[2]).pid][convertToIndices(sort_hit_index[2]).module][convertToIndices(sort_hit_index[2]).chip][convertToIndices(sort_hit_index[2]).chan]<<endl;
    // cout<<"test N event : "<<ch_hit_after_all[convertToIndices(sort_hit_index[3]).pid][convertToIndices(sort_hit_index[3]).module][convertToIndices(sort_hit_index[3]).chip][convertToIndices(sort_hit_index[3]).chan]<<endl;

    vector<double> contain_ratio; contain_ratio.clear();
    vector<double> X_index; X_index.clear();

    for (int i = 0; i < 100; i++)
    {
        to_ch_str to_ch = convertToIndices(sort_hit_index[i]);

        cout<<"the channel that has "<<i+1<<"-most hits, location : "<<to_ch.pid<<" "<<to_ch.module<<" "<<to_ch.chip + 1<<" "<<to_ch.chan<<" nhit : "<<ch_hit_after_all[to_ch.pid][to_ch.module][to_ch.chip][to_ch.chan]<<" ratio : "<< ( ch_hit_after_all[to_ch.pid][to_ch.module][to_ch.chip][to_ch.chan] ) / double(hit_after_all_typeA + hit_after_all_typeB)<<endl;
        contain_ratio.push_back( ( ch_hit_after_all[to_ch.pid][to_ch.module][to_ch.chip][to_ch.chan] ) / double(hit_after_all_typeA + hit_after_all_typeB) );
        X_index.push_back(i + 1);
    }

    TGraph * seq_ch_ratio = new TGraph(contain_ratio.size(), &X_index[0], &contain_ratio[0]);
    // seq_ch_ratio -> SetMarkerColor();
    seq_ch_ratio -> SetMarkerStyle(20);
    seq_ch_ratio -> SetMarkerSize(1);
    seq_ch_ratio -> SetTitle("Hit ratio of Top 100 channel");
    seq_ch_ratio -> GetXaxis() -> SetTitle("Chan ranking");
    seq_ch_ratio -> GetYaxis() -> SetTitle("Ratio (ch_hit / total_hit)");

    TH2INTT * N_clone_pos = new TH2INTT();
    N_clone_pos -> SetTitle("N clone hit in each ladder");

    TH2INTT * N_hit_before_pos = new TH2INTT();
    N_hit_before_pos -> SetTitle("Nhit before clone hit removal");

    TH2INTT * N_hit_after_pos = new TH2INTT();
    N_hit_after_pos -> SetTitle("Nhit post clone hit removal");

    TH2INTT * clone_hit_ratio_ladder = new TH2INTT();
    clone_hit_ratio_ladder -> SetTitle("Clone hit ratio");

    TH2INTT * N_bad_ch_ladder = new TH2INTT();
    N_bad_ch_ladder -> SetTitle("# of hot channels");

    for (int i = 0; i < 8; i++) // note : pid
    {
        for (int i1 = 0; i1 < 14; i1++) // note : module
        {
            N_clone_pos    -> SetSerFCIContent(i,i1,ld_clone_all[i][i1]);
            N_hit_after_pos -> SetSerFCIContent(i,i1,ld_hit_after_all[i][i1]);
            N_hit_before_pos -> SetSerFCIContent(i,i1,ld_hit_before_all[i][i1]);
            clone_hit_ratio_ladder -> SetSerFCIContent(i,i1, ld_clone_all[i][i1] / double(ld_hit_before_all[i][i1]) );
            N_bad_ch_ladder -> SetSerFCIContent(i,i1, bad_ch[i][i1] );
            nHotCh_hist -> SetBinContent((i*14 + i1) + 1,bad_ch[i][i1]);
        }
    }

    TGraph * high_ratio_grr = new TGraph(high_ratio_index.size(), &high_ratio_index[0], &high_ratio_y[0]);
    high_ratio_grr -> SetMarkerStyle(20);
    high_ratio_grr -> SetMarkerSize(0.2);
    high_ratio_grr -> Draw("ap");
    c1 -> Print(Form("%s/high_ratio_grr.pdf",output_directory.c_str()));
    c1 -> Clear();

    CHratio_Nhits_correlation -> Draw("colz0");
    c1 -> Print(Form("%s/CHratio_Nhits_correlation.pdf",output_directory.c_str()));
    c1 -> Clear();

    CHratio_index_correlation -> Draw();
    c1 -> Print(Form("%s/CHratio_index_correlation.pdf",output_directory.c_str()));
    c1 -> Clear();

    post_nhits_index -> Draw();
    c1 -> Print(Form("%s/post_nhits_index.pdf",output_directory.c_str()));
    c1 -> Clear();

    nHotCh_hist -> Draw("hist");
    c1 -> Print(Form("%s/combined_bad_ch_1D_Cut_%i.pdf",output_directory.c_str(), criterion));
    c1 -> Clear();
    
    hotch_map -> Draw("colz0");
    c1 -> Print(Form("%s/combined_bad_ch_distribution_Cut_%i.pdf",output_directory.c_str(), criterion));
    c1 -> Clear();

    TF1 * one_one_line = new TF1("one_one_line","pol1",0,30000);
    one_one_line -> SetParameters(0,1);
    
    Nhit_correlation -> Draw("colz0");
    one_one_line -> Draw("lsame");
    c1 -> Print(Form("%s/Nhit_correlation.pdf",output_directory.c_str()));
    c1 -> Clear();

    Nhit_correlation_short -> Draw("colz0");
    one_one_line -> Draw("lsame");
    c1 -> Print(Form("%s/Nhit_correlation_short.pdf",output_directory.c_str()));
    c1 -> Clear();



    seq_ch_ratio -> Draw("ap");
    c1 -> Print(Form("%s/seq_ch_ratio.pdf",output_directory.c_str()));
    c1 -> Clear();

    c1 -> SetLogy(1);

    nHit_after  -> Draw("hist");
    nHit_before -> Draw("hist same");
    c1 -> Print(Form("%s/Nhit_dist.pdf",output_directory.c_str()));
    c1 -> Clear();

    clone_hit_ratio -> Draw("hist");
    c1 -> Print(Form("%s/clone_hit_ratio.pdf",output_directory.c_str()));
    c1 -> Clear();

    NchHit_after -> Draw("hist");
    c1 -> Print(Form("%s/NchHit_after.pdf",output_directory.c_str()));
    c1 -> Clear();

    TCanvas * c2 = new TCanvas("","",1780,800);
    c2 -> cd();

    N_clone_pos -> Draw("colz0");
    c2 -> Print(Form("%s/N_clone_ladder.pdf",output_directory.c_str()));
    c2 -> Clear();

    N_hit_before_pos -> Draw("colz0");
    c2 -> Print(Form("%s/Nhit_before_ladder.pdf",output_directory.c_str()));
    c2 -> Clear();

    N_hit_after_pos -> Draw("colz0");
    c2 -> Print(Form("%s/Nhit_after_ladder.pdf",output_directory.c_str()));
    c2 -> Clear();

    clone_hit_ratio_ladder -> Draw("colz0");
    c2 -> Print(Form("%s/clone_hit_ratio_ladder.pdf",output_directory.c_str()));
    c2 -> Clear();

    N_bad_ch_ladder -> Draw("colz0");
    c2 -> Print(Form("%s/N_bad_ch_ladder_Cut_%i.pdf",output_directory.c_str(), criterion));
    c2 -> Clear();

    tree_out->SetDirectory(out_file);
    tree_out -> Write("", TObject::kOverwrite);

    cout<<"hot channel finder done, file : "<<endl;
	out_file -> Close();
}
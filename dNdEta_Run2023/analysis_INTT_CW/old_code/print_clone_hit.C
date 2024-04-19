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

void print_clone_hit()
{   
    
    string folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869";
    string file_name = "beam_inttall-00020869-0000_event_base_ana";
    string output_directory = folder_directory + "/PreCheck_" + file_name;
    double standard_ch_ratio_typeA = 1. / (8*14*16*128); // note : typeA, 16 sensor cells
    double standard_ch_ratio_typeB = 1. / (8*14*10*128); // note : typeB, 10 sensor cells
    int criterion = 3;
    bool All_event_used = false;
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
                    cout<<"clone hit found, event : "<<i<<" pid : "<<pid[i1]<<" module : "<<module[i1]<<" chip : "<<chip_id[i1]<<" chan : "<<chan_id[i1]<<" adc : "<<adc[i1]<<endl;
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

        memset(ch_adc, -1, sizeof(ch_adc));
        // memset(ld_clone_all, 0, sizeof(ld_clone_all)); // note : N_clone_hit along all the events.
        event_hit_info_vec.clear();
        
    } // note : for loop, end of event
}
#include "MCSelfGenReader.C"
#include "../ana_map_folder/ana_map_v1.h"

// note : this code is mainly for the selfgen MC, to calculate the MBin cut on NClus

void calculate_MBin(string input_directory, string input_file_name, string output_directory)
{
    TFile * file_in =  TFile::Open(Form("%s/%s", input_directory.c_str(), input_file_name.c_str()));
    TTree * tree = (TTree*)file_in->Get("tree");
    long long N_event = tree->GetEntries();

    std::cout << "N_event = " << N_event << std::endl;

    MCSelfGenReader * inttMCselfgen = new MCSelfGenReader(tree);

    vector<int> NClus_event; NClus_event.clear();
    vector<int> NClus_bin_cut; NClus_bin_cut.clear();

    TH1F * NClus_hist = new TH1F("",";N clusters;Entry",100,0,4500);

    TLine * coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(2);
    // coord_line -> SetLineStyle(2);

    vector<int> MBin_NClus_cut = {
        5,
        15,
        25,
        35,
        45,
        55,
        65,
        75,
        85,
        95,
    };

    vector<string> centrality_region = ANA_MAP_V1::centrality_region;
    

    for (long long event_i = 0; event_i < N_event; event_i++)
    {
        inttMCselfgen->LoadTree(event_i);
        inttMCselfgen->GetEntry(event_i);

        if (event_i % 3000 == 0) {cout<<"eID : "<<event_i<<" NClus : "<<inttMCselfgen->NClus<<endl;}

        NClus_hist->Fill(inttMCselfgen->NClus);
        NClus_event.push_back(inttMCselfgen->NClus);
    } 

    int total_entry = NClus_event.size();

    int sort_clu_index[NClus_event.size()];
    cout<<"NClus_event: "<<NClus_event.size()<<endl;
    TMath::Sort(int(NClus_event.size()), &NClus_event[0], sort_clu_index);



    TFile * file_out = new TFile(Form("%s/%s", output_directory.c_str(), "MBin_NClus_cut.root"), "RECREATE");
    file_out -> cd();
    TTree * tree_out = new TTree("tree", "MBin cut on NClus");
    tree_out -> Branch("MBinCut", &NClus_bin_cut);

    TCanvas * c1 = new TCanvas("","",900,800);
    NClus_hist -> Draw("hist");
    c1 -> SetLogy(1);

    for (int i = 0; i < MBin_NClus_cut.size(); i++)
    {   
        cout<<"centrality upper cut of portion : "<<centrality_region[i]<<", the N event ID : "<< int(total_entry * (double(MBin_NClus_cut[i]) /100.))<<" entry_line : "<<NClus_event[ sort_clu_index[ int(total_entry * (double(MBin_NClus_cut[i]) /100.)) ] ]<<endl;

        NClus_bin_cut.push_back(NClus_event[ sort_clu_index[ int(total_entry * (double(MBin_NClus_cut[i]) /100.)) ] ]);

        coord_line -> DrawLine(NClus_event[ sort_clu_index[ int(total_entry * (double(MBin_NClus_cut[i]) /100.)) ] ], 0, NClus_event[ sort_clu_index[ int(total_entry * (double(MBin_NClus_cut[i]) /100.)) ] ], NClus_hist -> GetBinContent( int( NClus_event[ sort_clu_index[ int(total_entry * (double(MBin_NClus_cut[i]) /100.)) ] ] / double(NClus_hist -> GetBinWidth(1)) ) + 1 ));
    }

    tree_out -> Fill();

    c1 -> cd();
    c1 -> Print(Form("%s/%s", output_directory.c_str(), "NClus_hist.pdf"));

    file_out -> cd();
    tree_out -> SetDirectory(file_out);
    tree_out -> Write("", TObject::kOverwrite);

    file_out -> Close();

}
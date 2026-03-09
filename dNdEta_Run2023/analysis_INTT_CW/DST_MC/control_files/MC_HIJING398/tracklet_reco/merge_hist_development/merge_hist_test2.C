#include <iostream>
using namespace std;

#include <TFile.h>
#include <TTree.h>
#include <TH2.h>

// void merge_hist_test2()
int main()
{
    string file_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/TrackletReco_condor/core_0/INTT_final_hist_info.root";
    map<string, TFile*> TFile_in_map; TFile_in_map.clear();
    TFile_in_map[file_directory.c_str()] = TFile::Open(Form("%s",file_directory.c_str()));

    cout<<"test : "<<((TH2F*) TFile_in_map[file_directory.c_str()] -> Get("Eta_Z_reference")) -> GetBinContent(18,8)<<endl;

    return 0;
}
#ifndef ANA_MAP_V1_H
#define ANA_MAP_V1_H

#include <iostream>
using namespace std;

#include <TH1F.h>

namespace ANA_MAP_V1
{
    // note : no inclusive included
    map<int,int> centrality_map = {
        {5, 0},
        {15, 1},
        {25, 2},
        {35, 3},
        {45, 4},
        {55, 5},
        {65, 6},
        {75, 7},
        {85, 8},
        {95, 9}
    };
    
    // note : include the inclusive
    vector<string> centrality_region = {
        "0-5%",
        "5-15%",
        "15-25%",
        "25-35%",
        "35-45%",
        "45-55%",
        "55-65%",
        "65-75%",
        "75-85%",
        "85-95%",
        "0-95%"
    };

    // note : the following two vectors tell the region of the eta and z, including the edges in both sides.
    // note : so when talking about the number of bins, we have to do size() - 1
    vector<double> eta_region = { // todo: if the eta region is changed, the following vector should be updated
        -3.0,
        -2.8,
        -2.6,
        -2.4,
        -2.2,
        -2,
        -1.8,
        -1.6,
        -1.4,
        -1.2,
        -1,
        -0.8,
        -0.6,
        -0.4,
        -0.2,
        0,
        0.2,
        0.4,
        0.6,
        0.8,
        1,
        1.2,
        1.4,
        1.6,
        1.8,
        2,
        2.2,
        2.4,
        2.6,
        2.8,
        3
    };

    vector<double> z_region = { // todo: if the z region is changed, the following vector should be updated
        -420, // note unit : mm
        -380,
        -340,
        -300,
        -260,
        -220, // note : this part is the peak region
        -180, // note : this part is the peak region
        -140,
        -100,
        -60,
        -20,
        20
    };

    // todo: zvtx resolution from the file shown above 
    // todo: from /sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/SemiFinal_EvtZ_MC_ZF_zvtx/INTT_zvtx.root
    map<int, double> reco_Z_resolution = {
        {0, 1.78117},
        {1, 1.85937},
        {2, 2.05982},
        {3, 2.33111},
        {4, 2.80505},
        {5, 3.70811},
        {6, 4.65284},
        {7, 5.37133},
        {8, 5.94759},
        {9, 6.56091},
        {10, 2.2836} // note : the inclusive centrality, 0% - 95%
    };
}

// note : this is for the selfgen MC, which already defines the centrality by the bin ID, no longer need to conver it 
namespace ANA_MAP_V2
{
    // note : no inclusive included
    map<int,int> centrality_map = {
        {0, 0},
        {1, 1},
        {2, 2},
        {3, 3},
        {4, 4},
        {5, 5},
        {6, 6},
        {7, 7},
        {8, 8},
        {9, 9},
        {10,10}
    };
    
    // note : include the inclusive
    vector<string> centrality_region = {
        "0-5%",
        "5-15%",
        "15-25%",
        "25-35%",
        "35-45%",
        "45-55%",
        "55-65%",
        "65-75%",
        "75-85%",
        "85-95%",
        "95-100%",
        "0-100%"
    };

    vector<double> z_region = { // todo: if the z region is changed, the following vector should be updated
        -420, // note unit : mm
        -380,
        -340,
        -300,
        -260,
        -220, // note : this part is the peak region
        -180, // note : this part is the peak region
        -140,
        -100,
        -60,
        -20,
        20
    };

    vector<double> eta_region = { // todo: if the eta region is changed, the following vector should be updated
        -3.0,
        -2.8,
        -2.6,
        -2.4,
        -2.2,
        -2,
        -1.8,
        -1.6,
        -1.4,
        -1.2,
        -1,
        -0.8,
        -0.6,
        -0.4,
        -0.2,
        0,
        0.2,
        0.4,
        0.6,
        0.8,
        1,
        1.2,
        1.4,
        1.6,
        1.8,
        2,
        2.2,
        2.4,
        2.6,
        2.8,
        3
    };

    // todo: zvtx resolution from the file shown above 
    // todo: from /sphenix/user/ChengWei/sPH_dNdeta/self_gen_singleMu_v2/complete_file/evt_vtxZ/complete_file/merged_file.root
    map<int, double> reco_Z_resolution = {
        // {0, 1.78117},
        // {1, 1.85937},
        // {2, 2.05982},
        // {3, 2.33111},
        // {4, 2.80505},
        // {5, 3.70811},
        // {6, 4.65284},
        // {7, 5.37133},
        // {8, 5.94759},
        // {9, 6.56091},
        // {10, 2.2836} // note : the inclusive centrality, 0% - 95%

        {0, 1.084},
        {1, 1.22489},
        {2, 1.30696},
        {3, 1.36897},
        {4, 1.40817},
        {5, 1.46613},
        {6, 1.55441},
        {7, 1.69994},
        {8, 1.99916},
        {9, 2.70681},
        {10, 4.61837},
        {11, 1.68828} // note : inclusive centrality 0% ~ 100%
    };
}

// note : this map is for handling the file with the float centrality numbers
// note : we have to convert them here, hopefully
namespace ANA_MAP_V3
{
    // note : the `centrality_edges`, `centrality_map`, and `centrality_region` have to correspond to each other

    // note : no inclusive
    vector<double> centrality_edges = {0, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    TH1F * centrality_hist = new TH1F("","",11,&centrality_edges[0]);

    int convert_centrality_bin (double Mbin_in) { 
        int found_bin = centrality_hist -> FindBin(Mbin_in);
        if (found_bin == 0){ // note : at the left edge
            cout<<" centrality value outside the given range, input Mbin value : "<<Mbin_in<<endl;
            exit(1);
        }
        else if (found_bin == centrality_hist -> GetNbinsX() + 1) { // note : at right edge
            cout<<" centrality value outside the given range, input Mbin value : "<<Mbin_in<<endl;
            exit(1);
        }
        else { return found_bin - 1; }
    }

    // note : no inclusive included
    map<int,int> centrality_map = {
        {0, 0},
        {1, 1},
        {2, 2},
        {3, 3},
        {4, 4},
        {5, 5},
        {6, 6},
        {7, 7},
        {8, 8},
        {9, 9},
        {10, 10}
    };
    
    // note : include the inclusive
    vector<string> centrality_region = {
        "0-5%",
        "5-10%",
        "10-20%",
        "20-30%",
        "30-40%",
        "40-50%",
        "50-60%",
        "60-70%",
        "70-80%",
        "80-90%",
        "90-100%",
        "0-100%"
    };

    // note : the following two vectors tell the region of the eta and z, including the edges in both sides.
    // note : so when talking about the number of bins, we have to do size() - 1
    vector<double> eta_region = { // todo: if the eta region is changed, the following vector should be updated
        -3.0,
        -2.8,
        -2.6,
        -2.4,
        -2.2,
        -2,
        -1.8,
        -1.6,
        -1.4,
        -1.2,
        -1,
        -0.8,
        -0.6,
        -0.4,
        -0.2,
        0,
        0.2,
        0.4,
        0.6,
        0.8,
        1,
        1.2,
        1.4,
        1.6,
        1.8,
        2,
        2.2,
        2.4,
        2.6,
        2.8,
        3
    };

    vector<double> z_region = { // todo: if the z region is changed, the following vector should be updated
        -420, // note unit : mm
        -380,
        -340,
        -300,
        -260,
        -220, // note : this part is the peak region
        -180, // note : this part is the peak region
        -140,
        -100,
        -60,
        -20,
        20
    };

    // todo: zvtx resolution from the file shown above 
    // todo: from /sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/SemiFinal_EvtZ_MC_ZF_zvtx/INTT_zvtx.root
    map<int, double> reco_Z_resolution = {
        {0, 1.78117},
        {1, 1.85937},
        {2, 2.05982},
        {3, 2.33111},
        {4, 2.80505},
        {5, 3.70811},
        {6, 4.65284},
        {7, 5.37133},
        {8, 5.94759},
        {9, 6.56091},
        {10, 2.2836} // note : the inclusive centrality, 0% - 95%
    };
}

#endif
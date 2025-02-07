#include "Constants.h"

namespace Constants{
    // std::vector<double> centrality_edges = {0, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    // std::vector<double> centrality_edges = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
    std::vector<double> centrality_edges    = {1, 4, 7, 11, 16, 21, 26, 31, 36, 41, 46, 51, 56, 61, 66, 71, 76, 81, 86, 91, 96, 100};
    std::map<int, std::string> centrality_text = {
        {0, "0-3"},
        {1, "3-6"},
        {2, "6-10"},
        {3, "10-15"},
        {4, "15-20"},
        {5, "20-25"},
        {6, "25-30"},
        {7, "30-35"},
        {8, "35-40"},
        {9, "40-45"},
        {10, "45-50"},
        {11, "50-55"},
        {12, "55-60"},
        {13, "60-65"},
        {14, "65-70"},
        {15, "70-75"},
        {16, "75-80"},
        {17, "80-85"},
        {18, "85-90"},
        {19, "90-95"},
        {20, "95-100"},
        {70, "0-70"}
    };

    // note : vtxZQA
    std::pair<double, double> cut_vtxZDiff = {-3, 4.}; // note : MBDz - INTTz
    std::pair<double, double> cut_TrapezoidalFitWidth = {1.5, 5.5};
    std::pair<double, double> cut_TrapezoidalFWHM = {2,8};
    std::pair<double, double> cut_INTTvtxZError = {-10000, 100000};

    // note : for analysis
    std::pair<double, double> cut_GlobalMBDvtxZ = {-60, 60};
    std::pair<double, double> cut_AnaVtxZ = {-10, 10}; // note : used by vtxZDist.cpp

    int cut_InttBcoFullDIff_next = 61;

    int Semi_inclusive_bin = 14;
    int Semi_inclusive_interval = 70; // note : 70, for example

    int HighNClus = 500;

    double VtxZEdge_min = -45; // note : cm // note : used by vtxZDist.cpp
    double VtxZEdge_max = 45; // note : cm  // note : used by vtxZDist.cpp
    int nVtxZBin = 18;                      // note : used by vtxZDist.cpp

    double cut_GoodRecoVtxZ = 1.; // note : cm // note : used by vtxZDist.cpp

    // note : for the column multiplicity correction
    // note : this is for the ZID
    int nZbin = 100;
    double Zmin = -25;
    double Zmax = 25;

    // note : almost no change
    double EtaEdge_min = -2.7; // note : used by RestDist.h
    double EtaEdge_max = 2.7;  // note : used by RestDist.h
    int nEtaBin = 27;          // note : used by RestDist.h

    // note : below should never be changed
    double typeA_sensor_half_length_incm = 0.8; // note : [cm]
    double typeB_sensor_half_length_incm = 1.0; // note : [cm] 

    int B0L0_index = 3;
    int B0L1_index = 4;
    int B1L0_index = 5;
    int B1L1_index = 6;
    int nLadder_inner = 12;
    int nLadder_outer = 16;


    std::map<int, double> centrality_Hist_Ymax ={
        {0, 1100},
        {1, 1000},
        {2, 900},
        {3, 700},
        {4, 600},
        {5, 500},
        {6, 400},
        {7, 310},
        {8, 240},
        {9, 190},
        {10, 150},
        {11, 150},
        {12, 100},
        {13, 70},
        {14, 50},
        {15, 50},
        {16, 50},
        {17, 50},
        {18, 50},
        {19, 50},
        {20, 50},
        {70, 360}
    };
}
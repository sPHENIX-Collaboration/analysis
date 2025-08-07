#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <vector>
#include <string>
#include <map>

namespace Constants{
    extern std::vector<double> centrality_edges;
    extern std::map<int, std::string> centrality_text;
    extern std::map<int, double> centrality_Hist_Ymax;

    // note : vtxZQA
    extern std::pair<double, double> cut_vtxZDiff; // note : MBDz - INTTz
    extern std::pair<double, double> cut_TrapezoidalFitWidth;
    extern std::pair<double, double> cut_TrapezoidalFWHM;
    extern std::pair<double, double> cut_INTTvtxZError;

    // note : for analysis
    extern std::pair<double, double> cut_GlobalMBDvtxZ;
    extern std::pair<double, double> cut_AnaVtxZ;

    extern int cut_InttBcoFullDIff_next;

    extern int Semi_inclusive_bin;
    extern int Semi_inclusive_interval; // note : 70, for example

    extern int HighNClus;

    extern double VtxZEdge_min;
    extern double VtxZEdge_max;
    extern int nVtxZBin;

    extern double cut_GoodRecoVtxZ; // note : cm

    extern int nZbin;
    extern double Zmin;
    extern double Zmax;

    // note : almost no change
    extern double EtaEdge_min;
    extern double EtaEdge_max;
    extern int nEtaBin;


    //  note : below should never be changed
    extern double typeA_sensor_half_length_incm; // note : [cm]
    extern double typeB_sensor_half_length_incm; // note : [cm] 

    extern int B0L0_index;
    extern int B0L1_index;
    extern int B1L0_index;
    extern int B1L1_index;
    extern int nLadder_inner;
    extern int nLadder_outer;
}

#endif
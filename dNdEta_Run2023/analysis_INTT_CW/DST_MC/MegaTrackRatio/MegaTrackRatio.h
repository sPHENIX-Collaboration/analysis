#ifndef MEGATRACKRATIO_H
#define MEGATRACKRATIO_H

#include "../../sPhenixStyle.h"
#include "../ReadEta/ReadEta_v1.C"
#include <map>
#include <TCanvas.h>
#include <TLatex.h>
#include <TH2.h>

// R__LOAD_LIBRARY(../ReadEta/ReadEta_v1_C.so)

class MegaTrackRatio
{
    public:
        MegaTrackRatio();
        MegaTrackRatio(string run_type, string input1_directory, string input1_name, string tree1_name, string input2_directory, string input2_name, string tree2_name);
        void ProcessEvent();
        void PrintPlot();

    protected:
        string run_type;
        string input1_directory;
        string input2_directory;
        string input1_name;
        string input2_name;
        string tree1_name;
        string tree2_name;

        TFile * file_in1;
        TFile * file_in2;
        TTree * tree_in1;
        TTree * tree_in2;

        ReadEta_v1 * readEta1;
        ReadEta_v1 * readEta2;

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

        TCanvas * c1;
        TH2F * MegaTrackRatio_Mbin_2D;
        TH2F * MegaTtrackRatio_NMegaTrack_2D;
        TLatex * ltx;

        string plot_text;

        void Init();
        void InitCanvas();
        void InitHist();
        void InitRest();
        
};

#endif
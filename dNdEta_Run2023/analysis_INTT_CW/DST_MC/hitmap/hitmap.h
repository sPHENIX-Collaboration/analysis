#ifndef HITMAP_H
#define HITMAP_H

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <numeric>
#include <map>

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>

#include "sPhenixStyle.h"
#include "InttEvent.h"

class InttEvent;

class HITMAP
{
    public:
        HITMAP(std::string input_file_directory, std::string input_tree_name, std::string output_directory);
        virtual ~HITMAP();

    protected:
        std::string input_file_directory;
        std::string input_tree_name;
        std::string output_directory;
        TFile* file_in;
        TTree * tree_in;
        InttEvent* inttEvt_;

        TCanvas * c1;
        TH2D* h2_AllMap_[8][14];

        void Init();

};

#endif
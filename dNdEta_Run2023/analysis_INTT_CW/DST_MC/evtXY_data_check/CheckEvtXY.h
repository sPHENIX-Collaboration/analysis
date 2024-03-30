#ifndef CHECKEVTXY_H
#define CHECKEVTXY_H

#include <iostream>

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TF1.h>

#include "ReadEvtXY_Chain.h"
#include "sPhenixStyle.h"
#include "gaus_func.h"


class CheckEvtXY
{
    public:
        CheckEvtXY(string mother_folder_directory, string input_file_list, string out_folder_directory, std::pair<double, double> beam_origin);
        // virtual ~CheckEvtXY();
        
        void Prepare_info();
        void Print_plots();

    protected:
        TCanvas * c1;
        TH1F * vtxX_1D;
        TH1F * vtxY_1D;
        TH2F * vtxXY_2D;
        TH2F * vtxX_eID_2D;
        TH2F * vtxY_eID_2D;
        TGraph * vtxX_bco_graph;
        TGraph * vtxY_bco_graph;

        TLatex * ltx;
        TLatex * ltx_warning;
        TLatex * draw_text;
        TF1 * gaus_fit_MC;

        std::pair<double, double> beam_origin;

        string mother_folder_directory;
        string input_file_list;
        string out_folder_directory;
        ReadEvtXY_Chain * data_in;

        void Init(); 
};


#endif
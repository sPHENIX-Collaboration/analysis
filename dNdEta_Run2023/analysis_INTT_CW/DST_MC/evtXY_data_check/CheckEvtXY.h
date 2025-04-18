#ifndef CHECKEVTXY_H
#define CHECKEVTXY_H

#include <iostream>

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TF1.h>

#include "ReadEvtXY_Chain.C"
#include "sPhenixStyle.C"
#include "gaus_func.cpp"


class CheckEvtXY
{
    public:
        CheckEvtXY(
            string mother_folder_directory_in, 
            string input_file_list_in, 
            string out_folder_directory_in, 
            std::pair<double, double> beam_origin_in, 
            int NClus_cut_label_in,
            int unit_tag_in,
            string run_type_in
        );
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

        // note : for the MC
        TH2F * vtxX_correlation;
        TH2F * vtxY_correlation;
        TH1F * vtxX_diff;
        TH1F * vtxY_diff;

        TLatex * ltx;
        TLatex * ltx_warning;
        TLatex * draw_text;
        TF1 * gaus_fit_MC;
        TF1 * pol1_fit;

        string run_type;
        string sPHENIX_suffix;

        int unit_tag;
        string unit_text;
        double unit_correction;

        std::pair<double, double> beam_origin;

        string mother_folder_directory;
        string input_file_list;
        string out_folder_directory;
        ReadEvtXY_Chain * data_in;
        int NClus_cut_label;

        void Init(); 
};


#endif
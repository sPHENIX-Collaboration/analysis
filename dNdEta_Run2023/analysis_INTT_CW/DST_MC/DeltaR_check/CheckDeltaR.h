#ifndef CheckDeltaR_h
#define CheckDeltaR_h

#include <iostream>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <map>
#include <TMath.h>
#include "../ReadEta/ReadEta_v3.h"

R__LOAD_LIBRARY(/sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/DST_MC/ReadEta/libReadEtaV3.so)

class CheckDeltaR 
{
    public:
        CheckDeltaR(string run_type, string input_folder_directory, string input_file_name, string input_tree_name, string out_folder_directory);
        void Processing(int run_Nevent);
        void PrintPlots();
        ~CheckDeltaR();
    protected:
        string out_folder_directory;
        string input_folder_directory;
        string input_file_name;
        string input_tree_name;
        TFile * file_in;
        TTree * tree_in;
        ReadEta_v3 * readeta;

        double eta_weight = 0.631099/0.0195074;

        TCanvas * c1;
        TLatex * ltx;
        string run_type;
        string plot_text;

        TH1F * DeltaPhi_All_1D;
        TH1F * DeltaEta_All_1D;
        TH2F * DeltaPhiDeltaEta_All_2D;

        TH1F * DeltaPhi_link_1D;
        TH1F * DeltaEta_link_1D;
        TH2F * DeltaPhiDeltaEta_link_2D;

        TH1F * DeltaR_best_1D;
        TH2F * DeltaPhiDeltaEta_best_2D;
        TH2F * DeltaEtaTrueEta_best_2D;
        TH2F * DeltaEtaRecoZ_best_2D;
        TH2F * DeltaEtaTrueZ_best_2D;

        vector<vector<double>> track_pair;
        vector<double> track_pair_deltaR;
        map <int, int> true_track_used_tag;
        map <int, int> reco_track_used_tag;

        void Init();
        void ClearEvt();
};

#endif
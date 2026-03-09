#include <iostream>
#include "../../sPhenixStyle.C"
// #include "../ReadEta/ReadEta_v1.h"
#include "MegaTrackRatio.h"

MegaTrackRatio::MegaTrackRatio()
{
    return;
}

MegaTrackRatio::MegaTrackRatio(string run_type, string input1_directory, string input1_name, string tree1_name, string input2_directory, string input2_name, string tree2_name)
: run_type(run_type),
input1_directory(input1_directory),  
input1_name(input1_name),  
tree1_name(tree1_name),  
input2_directory(input2_directory),  
input2_name(input2_name),  
tree2_name(tree2_name)
{
    plot_text = (run_type == "MC") ? "Simulation" : "Work-in-progress";
    Init();
    return;
}

void MegaTrackRatio::Init()
{
    // gROOT->LoadMacro("sPhenixStyle.C");
    // gROOT->LoadMacro("ReadEta_v1.C");
    SetsPhenixStyle();

    file_in1 = TFile::Open(Form("%s/%s",input1_directory.c_str(), input1_name.c_str()));
    tree_in1 = (TTree *)file_in1->Get(tree1_name.c_str());
    readEta1 = new ReadEta_v1(tree_in1);
    std::cout<<"tree1 entry : "<<tree_in1 -> GetEntries()<<std::endl;

    file_in2 = TFile::Open(Form("%s/%s",input2_directory.c_str(), input2_name.c_str()));
    tree_in2 = (TTree *)file_in2->Get(tree2_name.c_str());
    readEta2 = new ReadEta_v1(tree_in2);
    std::cout<<"tree2 entry : "<<tree_in2 -> GetEntries()<<std::endl;

    if (tree_in1 -> GetEntries() != tree_in2 -> GetEntries())
    {
        std::cout<<"Error : tree1 and tree2 have different entries"<<std::endl;
        return;
    }
    InitCanvas();
    InitHist();
    InitRest();

}

void MegaTrackRatio::InitCanvas()
{
    c1 = new TCanvas("","",950,800);
    c1 -> cd();
}


void MegaTrackRatio::InitHist()
{
    MegaTrackRatio_Mbin_2D = new TH2F("","MegaTrackRatio_Mbin_2D",  10, 0, 10, 100,95,105);
    MegaTrackRatio_Mbin_2D -> GetXaxis() -> SetTitle("Centrality bin");
    MegaTrackRatio_Mbin_2D -> GetYaxis() -> SetTitle("Ratio [%]");
    MegaTrackRatio_Mbin_2D -> GetXaxis() -> SetNdivisions(505);

    MegaTtrackRatio_NMegaTrack_2D = new TH2F("","MegaTtrackRatio_NMegaTrack_2D",  40, 0, 40, 100,95,105);
    MegaTtrackRatio_NMegaTrack_2D -> GetXaxis() -> SetTitle("N Mega Track");
    MegaTtrackRatio_NMegaTrack_2D -> GetYaxis() -> SetTitle("Ratio [%]");
    MegaTtrackRatio_NMegaTrack_2D -> GetXaxis() -> SetNdivisions(505);
}

void MegaTrackRatio::InitRest()
{
    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);
}


void MegaTrackRatio::ProcessEvent()
{
    double N_track_file1;
    double N_track_file2;
    double file1_Mbin; // note : the centrality bin
    double file2_Mbin;

    std::cout<<"test : "<<tree_in1 -> GetEntries()<<std::endl;

    for (int i = 0; i < tree_in1 -> GetEntries(); i++)
    {
        readEta1 -> LoadTree(i);
        readEta1 -> GetEntry(i);
        if ((readEta1 -> N3Clu_track + readEta1 -> N4Clu_track) == 0) {continue;}

        N_track_file1 = readEta1 -> N2Clu_track + readEta1 -> N3Clu_track + readEta1 -> N4Clu_track;
        file1_Mbin = readEta1 -> Evt_centrality_bin;

        readEta2 -> LoadTree(i);
        readEta2 -> GetEntry(i);
        N_track_file2 = readEta2 -> N2Clu_track + readEta2 -> N3Clu_track + readEta2 -> N4Clu_track;
        file2_Mbin = readEta2 -> Evt_centrality_bin;

        if (file1_Mbin != file2_Mbin)
        {
            std::cout<<"Error : file1_Mbin != file2_Mbin"<<std::endl;
            return;
        }
        // std::cout<<centrality_map[file1_Mbin] <<" "<< (N_track_file1/N_track_file2) * 100.<<std::endl;
        MegaTrackRatio_Mbin_2D -> Fill(centrality_map[file1_Mbin], (N_track_file1/N_track_file2) * 100.);
        MegaTtrackRatio_NMegaTrack_2D -> Fill(readEta1 -> N3Clu_track + readEta1 -> N4Clu_track, (N_track_file1/N_track_file2) * 100.);
    }

    return;
}

void MegaTrackRatio::PrintPlot()
{
    c1 -> cd();
    MegaTrackRatio_Mbin_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/MegaTrackRatio_Mbin_2D.pdf",input1_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    MegaTtrackRatio_NMegaTrack_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/MegaTtrackRatio_NMegaTrack_2D.pdf",input1_directory.c_str()));
    c1 -> Clear();

    return;
}
#include <iostream>
#include <string>

#include <TCanvas.h>
#include <TFile.h>
#include <TH2.h>
#include <TH1.h>
#include <TLatex.h>

#include "sPhenixStyle.C"
#include "../../ReadF4ANtupleData/ReadF4ANtupleData.C"

#include "../../ana_map_folder/ana_map_v1.h"

// todo : check used namespace
namespace ana_map_version = ANA_MAP_V3;

// #include "ReadMergedTree.h"

int intt_mbd_combine_check()
{
    std::string input_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey";
    std::string input_filename = "Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root";
    TFile * file_in = TFile::Open(Form("%s/%s",input_directory.c_str(), input_filename.c_str()));
    TTree * tree_in = (TTree *) file_in -> Get("EventTree");
    ReadF4ANtupleData * merge_tree = new ReadF4ANtupleData(tree_in);

    TH2F * intt_mbd_bco = (TH2F *)file_in->Get("intt_mbd_bco_postsync");
    intt_mbd_bco -> GetXaxis() -> SetTitle("Event ID");
    intt_mbd_bco -> GetYaxis() -> SetTitle("Clock difference [BCO]");

    TH2F * intt_mbd_correlation = new TH2F("h_qmbd_nintt", "BbcQ vs Intt N;INTT number of clusters;MBD charge sum", 200, 0, 14000, 200, 0, 2500);
    intt_mbd_correlation -> GetXaxis() -> SetNdivisions(505);

    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("","",950,800);
    c1 -> cd();

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    for (int i = 0; i < tree_in -> GetEntries(); i++)
    {
        merge_tree -> LoadTree(i);
        merge_tree -> GetEntry(i);

        if (i % 1000 == 0)
        {
            std::cout << "Processing event " << i << std::endl;
        }

        if (merge_tree -> MBD_z_vtx != merge_tree -> MBD_z_vtx) {continue;}
        if (merge_tree -> MBD_centrality != merge_tree -> MBD_centrality) {continue;}

        double MBD_charge_assy = (merge_tree -> MBD_north_charge_sum - merge_tree -> MBD_south_charge_sum) / (merge_tree -> MBD_north_charge_sum + merge_tree -> MBD_south_charge_sum);
        if ( MBD_charge_assy < -1 * ana_map_version::MBD_assy_ratio_cut || MBD_charge_assy >  ana_map_version::MBD_assy_ratio_cut) {continue;}

        if (merge_tree -> MBD_south_charge_sum > 50 && merge_tree -> MBD_north_charge_sum > 50)
        {
            intt_mbd_correlation -> Fill(merge_tree -> NClus, merge_tree -> MBD_south_charge_sum + merge_tree -> MBD_north_charge_sum);
        }
    }

    c1 -> cd();
    intt_mbd_bco -> Draw("");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    draw_text -> DrawLatex(0.25, 0.84, Form("Run 20869"));
    c1 -> Print(Form("%s/run20869_intt_mbd_bco_2D.pdf", input_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    intt_mbd_correlation -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    draw_text -> DrawLatex(0.25, 0.88, Form("Run 20869"));
    draw_text -> DrawLatex(0.25, 0.84, Form("MBD Q > 50, both sides"));
    c1 -> Print(Form("%s/run20869_intt_mbd_correlation_2D.pdf", input_directory.c_str()));
    c1 -> Clear();

    return 0;

}
#include "../sPhenixStyle.C"

struct evt_z {
    int eID;
    double zvtx;
};

void INTT_z_comp()
{
    string CW_folder = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut_advanced_test";

    TFile * file_my = TFile::Open(Form("%s/INTT_zvtx.root",CW_folder.c_str()));
    TTree * tree_my = (TTree *)file_my->Get("tree_Z");

    double out_ES_zvtx, out_ES_zvtxE, out_ES_rangeL, out_ES_rangeR, out_ES_width_density, MC_true_zvtx;
    double out_LB_Gaus_Mean_mean, out_LB_Gaus_Mean_meanE, out_LB_Gaus_Mean_width, out_LB_Gaus_Mean_chi2;
    double out_LB_Gaus_Width_width, out_LB_Gaus_Width_size_width, out_LB_Gaus_Width_offset, out_LB_geo_mean;
    double out_mid_cut_peak_width, out_mid_cut_peak_ratio, out_LB_cut_peak_width, out_LB_cut_peak_ratio;
    bool out_good_zvtx_tag;
    int out_eID, N_cluster_outer_out, N_cluster_inner_out, out_ES_N_good, out_mid_cut_Ngroup, out_LB_cut_Ngroup;
    Long64_t bco_full_out; 

    tree_my -> SetBranchAddress("eID",&out_eID);
    tree_my -> SetBranchAddress("bco_full",&bco_full_out);
    tree_my -> SetBranchAddress("nclu_inner",&N_cluster_inner_out);
    tree_my -> SetBranchAddress("nclu_outer",&N_cluster_outer_out);
    tree_my -> SetBranchAddress("ES_zvtx",&out_ES_zvtx);                   // note : effective sigma, pol0 fit z-vertex
    tree_my -> SetBranchAddress("ES_zvtxE",&out_ES_zvtxE);                 // note : effective sigma, pol0 fit z-vertex error
    tree_my -> SetBranchAddress("ES_rangeL",&out_ES_rangeL);               // note : effective sigma, selected range left
    tree_my -> SetBranchAddress("ES_rangeR",&out_ES_rangeR);               // note : effective sigma, selected range right 
    tree_my -> SetBranchAddress("ES_N_good",&out_ES_N_good);               // note : effective sigma, number of z-vertex candidates in the range
    tree_my -> SetBranchAddress("ES_Width_density",&out_ES_width_density); // note : effective sigma, N good z-vertex candidates divided by width
    tree_my -> SetBranchAddress("LB_Gaus_Mean_mean",&out_LB_Gaus_Mean_mean);              // note : Line break loose offset - gaus mean   
    tree_my -> SetBranchAddress("LB_Gaus_Mean_meanE",&out_LB_Gaus_Mean_meanE);            // note : Line break loose offset - gaus mean error
    tree_my -> SetBranchAddress("LB_Gaus_Mean_chi2", &out_LB_Gaus_Mean_chi2);             // note : Line break loose offset - reduce chi2
    tree_my -> SetBranchAddress("LB_Gaus_Mean_width",&out_LB_Gaus_Mean_width);            // note : Line break loose offset - width
    tree_my -> SetBranchAddress("LB_Gaus_Width_width",&out_LB_Gaus_Width_width);                 // note : Line break tight offset - gaus width
    tree_my -> SetBranchAddress("LB_Gaus_Width_offset", &out_LB_Gaus_Width_offset);              // note : Line break tight offset - offset
    tree_my -> SetBranchAddress("LB_Gaus_Width_size_width", &out_LB_Gaus_Width_size_width);      // note : Line break tight offset - norm. height / width
    tree_my -> SetBranchAddress("LB_geo_mean", &out_LB_geo_mean);          // note : Line break peak position directly from the distribution (with the bin width 0.5 mm)
    tree_my -> SetBranchAddress("good_zvtx_tag", &out_good_zvtx_tag);
    tree_my -> SetBranchAddress("mid_cut_Ngroup",     &out_mid_cut_Ngroup);            // note : mid cut Ngroup
    tree_my -> SetBranchAddress("mid_cut_peak_width", &out_mid_cut_peak_width);        // note : mid cut peak width
    tree_my -> SetBranchAddress("mid_cut_peak_ratio", &out_mid_cut_peak_ratio);        // note : mid cut peak ratio
    tree_my -> SetBranchAddress("LB_cut_Ngroup",     &out_LB_cut_Ngroup);         // note : LB cut Ngroup
    tree_my -> SetBranchAddress("LB_cut_peak_width", &out_LB_cut_peak_width);     // note : LB cut peak width
    tree_my -> SetBranchAddress("LB_cut_peak_ratio", &out_LB_cut_peak_ratio);     // note : LB cut peak ratio
    tree_my -> SetBranchAddress("MC_true_zvtx",&MC_true_zvtx);    

    vector<evt_z> my_zvtx; my_zvtx.clear();

    for (int i = 0; i < tree_my -> GetEntries(); i++){
        tree_my -> GetEntry(i);

        if (out_good_zvtx_tag == 1){
            my_zvtx.push_back({out_eID,out_LB_Gaus_Mean_mean});
        }
    }


    float evt;
    float zv;
    vector<evt_z> Takashi_zvtx; Takashi_zvtx.clear(); 
    TFile * file_Takashi = TFile::Open("/sphenix/tg/tg01/commissioning/INTT/work/maya/rootfiles/AnaTutorial_Intt/sync_00020869-0000_nevent20000_wCWhotmap.root");
    TNtuple * tree_Takashi = (TNtuple *)file_Takashi->Get("ntp_evt");
    tree_Takashi -> SetBranchAddress("zv",&zv);
    tree_Takashi -> SetBranchAddress("evt",&evt);

    for (int i = 0; i < tree_Takashi -> GetEntries(); i++){
        tree_Takashi -> GetEntry(i);
        
        Takashi_zvtx.push_back({int(evt),zv * 10.});
    }

    TH2F * z_corre = new TH2F("","z_correlation",200,-500,100,200,-500,100);
    z_corre -> SetStats(0);
    z_corre -> GetXaxis() -> SetTitle("Cheng-Wei Z [mm]");
    z_corre -> GetYaxis() -> SetTitle("Takashi Z [mm]");

    TH1F * z_diff = new TH1F("","Z difference",200,-100,100);
    z_diff -> SetStats(0);
    z_diff -> GetXaxis() -> SetTitle("CW - Takashi [mm]");
    z_diff -> GetYaxis() -> SetTitle("Entry");

    for (int i = 0; i < my_zvtx.size(); i++){
        for (int i1 = 0; i1 < Takashi_zvtx.size(); i1++){
            if (my_zvtx[i].eID == Takashi_zvtx[i1].eID){
                z_corre -> Fill(my_zvtx[i].zvtx, Takashi_zvtx[i1].zvtx);
                z_diff -> Fill(my_zvtx[i].zvtx - Takashi_zvtx[i1].zvtx);
                
                // cout<<my_zvtx[i].zvtx <<" "<< Takashi_zvtx[i1].zvtx<<endl;

                break;
            }
        }
    }

    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("","",950,800);
    c1 -> cd();
    z_corre -> Draw("colz0");
    c1 -> Print(Form("%s/INTT_comp_correlation.pdf",CW_folder.c_str()));
    c1 -> Clear();

    z_diff -> Draw("hist");
    c1 -> Print(Form("%s/INTT_comp_diff.pdf",CW_folder.c_str()));
    c1 -> Clear();
}
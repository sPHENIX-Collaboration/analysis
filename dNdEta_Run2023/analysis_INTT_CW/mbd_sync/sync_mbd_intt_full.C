#include <TFile.h>

#include <TTree.h>

#include <TDirectory.h>

// #include "InttEvent.cc"

#include "mbdtree.C"

#include <iostream>

//R__LOAD_LIBRARY(libInttEvent.so)

void sync_mbd_intt_full() {

    TDirectory * gDir = gDirectory;

    TFile * f_mbd = TFile::Open("/sphenix/tg/tg01/commissioning/INTT/subsystems/MBD/auau2023_v1/beam_seb18-00020869-0000_mbd.root");
    gDirectory = gDir;
    TTree * t_mbd = (TTree * ) f_mbd -> Get("t");
    cout << " " << t_mbd << endl;
    if (!t_mbd) return;
    mbdtree mbdt(t_mbd);

    string folder_direction = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut_advanced_test";
    TFile * f_intt = TFile::Open(Form("%s/INTT_zvtx_200k.root", folder_direction.c_str()));
    gDirectory = gDir;
    TTree * t_intt = (TTree * ) f_intt -> Get("tree_Z");
    if (!t_intt) return;

    int intt_eID, intt_N_cluster_outer, intt_N_cluster_inner, intt_N_good, intt_ES_N_good, intt_N_group;
    double intt_ES_zvtx, intt_ES_zvtxE, intt_ES_rangeL, intt_ES_rangeR, intt_ES_width_density;
    double intt_LB_Gaus_mean, intt_LB_Gaus_meanE, intt_LB_Gaus_width, intt_LB_Gaus_offset, intt_LB_Gaus_chi2, intt_LB_Gaus_size_width, intt_LB_geo_mean;
    double MC_true_zvtx;
    bool intt_good_zvtx_tag;
    Long64_t intt_bco_full; 

    t_intt -> SetBranchAddress("eID",&intt_eID);
    t_intt -> SetBranchAddress("bco_full",&intt_bco_full);
    t_intt -> SetBranchAddress("nclu_inner",&intt_N_cluster_inner);
    t_intt -> SetBranchAddress("nclu_outer",&intt_N_cluster_outer);
    t_intt -> SetBranchAddress("ES_zvtx",&intt_ES_zvtx);
    t_intt -> SetBranchAddress("ES_zvtxE",&intt_ES_zvtxE);
    t_intt -> SetBranchAddress("ES_rangeL",&intt_ES_rangeL);
    t_intt -> SetBranchAddress("ES_rangeR",&intt_ES_rangeR);
    t_intt -> SetBranchAddress("ES_N_good",&intt_ES_N_good);
    t_intt -> SetBranchAddress("ES_Width_density",&intt_ES_width_density);
    t_intt -> SetBranchAddress("LB_Gaus_Mean_mean",&intt_LB_Gaus_mean);
    t_intt -> SetBranchAddress("LB_Gaus_Mean_meanE",&intt_LB_Gaus_meanE);
    t_intt -> SetBranchAddress("LB_Gaus_Mean_width",&intt_LB_Gaus_width);
    t_intt -> SetBranchAddress("LB_Gaus_Width_offset", &intt_LB_Gaus_offset);
    t_intt -> SetBranchAddress("LB_Gaus_Mean_chi2", &intt_LB_Gaus_chi2);
    t_intt -> SetBranchAddress("LB_Gaus_Width_size_width", &intt_LB_Gaus_size_width);
    t_intt -> SetBranchAddress("LB_geo_mean", &intt_LB_geo_mean);
    t_intt -> SetBranchAddress("good_zvtx_tag", &intt_good_zvtx_tag);
    t_intt -> SetBranchAddress("mid_cut_Ngroup", &intt_N_group);
    t_intt -> SetBranchAddress("MC_true_zvtx",&MC_true_zvtx);

    cout << t_mbd -> GetEntries() << " " << t_intt -> GetEntries() << endl;

    TFile * out_file = new TFile(Form("%s/INTT_MBD_zvtx.root",folder_direction.c_str()),"RECREATE");

    int out_eID, out_N_cluster_outer, out_N_cluster_inner, out_N_good, out_ES_N_good, out_N_group;
    double out_ES_zvtx, out_ES_zvtxE, out_ES_rangeL, out_ES_rangeR, out_ES_width_density;
    double out_LB_Gaus_mean, out_LB_Gaus_meanE, out_LB_Gaus_width, out_LB_Gaus_offset, out_LB_Gaus_chi2, out_LB_Gaus_size_width, out_LB_geo_mean;
    double out_MC_true_zvtx;
    bool out_good_zvtx_tag;
    Long64_t out_bco_full;  

    double out_mbd_bz, out_mbd_bqs, out_mbd_bqn;
    double out_mbd_bns, out_mbd_bnn;
    double out_mbd_bts, out_mbd_btn;
    double out_mbd_bt0;
    vector<double> out_mbd_tt; out_mbd_tt.clear();
    vector<double> out_mbd_tq; out_mbd_tq.clear();
    vector<double> out_mbd_q; out_mbd_q.clear();

    TTree * tree_out =  new TTree ("tree_Z", "INTT Z info.");

    tree_out -> Branch("eID",&out_eID);
    tree_out -> Branch("bco_full",&out_bco_full);
    tree_out -> Branch("nclu_inner",&out_N_cluster_inner);
    tree_out -> Branch("nclu_outer",&out_N_cluster_outer);
    tree_out -> Branch("ES_zvtx",&out_ES_zvtx);
    tree_out -> Branch("ES_zvtxE",&out_ES_zvtxE);
    tree_out -> Branch("ES_rangeL",&out_ES_rangeL);
    tree_out -> Branch("ES_rangeR",&out_ES_rangeR);
    tree_out -> Branch("ES_N_good",&out_ES_N_good);
    tree_out -> Branch("ES_Width_density",&out_ES_width_density);
    tree_out -> Branch("LB_Gaus_mean",&out_LB_Gaus_mean);
    tree_out -> Branch("LB_Gaus_meanE",&out_LB_Gaus_meanE);
    tree_out -> Branch("LB_Gaus_width",&out_LB_Gaus_width);
    tree_out -> Branch("LB_Gaus_offset", &out_LB_Gaus_offset);
    tree_out -> Branch("LB_Gaus_chi2", &out_LB_Gaus_chi2);
    tree_out -> Branch("LB_Gaus_size_width", &out_LB_Gaus_size_width);
    tree_out -> Branch("LB_geo_mean", &out_LB_geo_mean);
    tree_out -> Branch("good_zvtx_tag", &out_good_zvtx_tag);
    tree_out -> Branch("N_group", &out_N_group);
    tree_out -> Branch("MC_true_zvtx",&out_MC_true_zvtx);

    tree_out -> Branch("mbd_bz",&out_mbd_bz); // note : mbd branch
    tree_out -> Branch("mbd_bqs",&out_mbd_bqs);
    tree_out -> Branch("mbd_bqn",&out_mbd_bqn);
    tree_out -> Branch("mbd_bns",&out_mbd_bns);
    tree_out -> Branch("mbd_bnn",&out_mbd_bnn);
    tree_out -> Branch("mbd_bts",&out_mbd_bts);
    tree_out -> Branch("mbd_btn",&out_mbd_btn);
    tree_out -> Branch("mbd_bt0",&out_mbd_bt0);
    tree_out -> Branch("mbd_tt_vec",&out_mbd_tt);
    tree_out -> Branch("mbd_tq_vec",&out_mbd_tq);
    tree_out -> Branch("mbd_q_vec",&out_mbd_q);
    

    gDirectory = gDir;

    TH2F * h_qmbd_nintt = new TH2F("h_qmbd_nintt", "BbcQ vs Intt N", 200, 0, 9000, 200, 0, 4000);
    TH2F * intt_mbd_bco = new TH2F("intt_mbd_bco", "INTT - MBD", 100, 0, 50000, 100, -10, 100000);

    int prev_mbdclk = 0;
    ULong64_t prev_bco = 0;

    bool found_firstevt = false;
    int mbd_evt_offset = 0;
    int intt_evt_offset = 0;

    long long number_Nevent = t_intt -> GetEntries();

    // note : if the for loop is based on the one with smaller number of entries, the closer part should be paied attention.
    // note : because that the for loop ends when "i < t_intt -> GetEntries()", but we have the "intt_evt_offset" which is not zero if some missing event from the MBD was found.
    // note : when we use "GetEntry(i+intt_evt_offset)", it will try to access the event out of the range.
    // note : but some how it works, and return the last event it can access which is the last one. 
    for (int i = 0; i < number_Nevent; i++) {
        mbdt.LoadTree(i + mbd_evt_offset);
        mbdt.GetEntry(i + mbd_evt_offset);
        t_intt -> GetEntry(i + intt_evt_offset); 

        float bbcq = mbdt.bqn + mbdt.bqs;

        unsigned short mbdclk = mbdt.femclk;
        ULong64_t bco = intt_bco_full;
        ULong64_t bco16 = bco & 0xFFFF;

        int mbd_prvdif = (mbdclk - prev_mbdclk) & 0xFFFF;
        ULong64_t intt_prvdif = bco - prev_bco;

        prev_mbdclk = mbdclk;
        prev_bco = bco;

        out_mbd_bqn = mbdt.bqn;
        out_mbd_bqs = mbdt.bqs;
        out_mbd_bz  = mbdt.bz;
        out_mbd_bns = mbdt.bns;
        out_mbd_bnn = mbdt.bnn;
        out_mbd_bts = mbdt.bts;
        out_mbd_btn = mbdt.btn;
        out_mbd_bt0 = mbdt.bt0;

        out_mbd_tt.push_back(mbdt.tt0); out_mbd_tq.push_back(mbdt.tq0); out_mbd_q.push_back(mbdt.q0);
        out_mbd_tt.push_back(mbdt.tt1); out_mbd_tq.push_back(mbdt.tq1); out_mbd_q.push_back(mbdt.q1);
        out_mbd_tt.push_back(mbdt.tt2); out_mbd_tq.push_back(mbdt.tq2); out_mbd_q.push_back(mbdt.q2);
        out_mbd_tt.push_back(mbdt.tt3); out_mbd_tq.push_back(mbdt.tq3); out_mbd_q.push_back(mbdt.q3);
        out_mbd_tt.push_back(mbdt.tt4); out_mbd_tq.push_back(mbdt.tq4); out_mbd_q.push_back(mbdt.q4);
        out_mbd_tt.push_back(mbdt.tt5); out_mbd_tq.push_back(mbdt.tq5); out_mbd_q.push_back(mbdt.q5);
        out_mbd_tt.push_back(mbdt.tt6); out_mbd_tq.push_back(mbdt.tq6); out_mbd_q.push_back(mbdt.q6);
        out_mbd_tt.push_back(mbdt.tt7); out_mbd_tq.push_back(mbdt.tq7); out_mbd_q.push_back(mbdt.q7);
        out_mbd_tt.push_back(mbdt.tt8); out_mbd_tq.push_back(mbdt.tq8); out_mbd_q.push_back(mbdt.q8);
        out_mbd_tt.push_back(mbdt.tt9); out_mbd_tq.push_back(mbdt.tq9); out_mbd_q.push_back(mbdt.q9);
        out_mbd_tt.push_back(mbdt.tt10); out_mbd_tq.push_back(mbdt.tq10); out_mbd_q.push_back(mbdt.q10);
        out_mbd_tt.push_back(mbdt.tt11); out_mbd_tq.push_back(mbdt.tq11); out_mbd_q.push_back(mbdt.q11);
        out_mbd_tt.push_back(mbdt.tt12); out_mbd_tq.push_back(mbdt.tq12); out_mbd_q.push_back(mbdt.q12);
        out_mbd_tt.push_back(mbdt.tt13); out_mbd_tq.push_back(mbdt.tq13); out_mbd_q.push_back(mbdt.q13);
        out_mbd_tt.push_back(mbdt.tt14); out_mbd_tq.push_back(mbdt.tq14); out_mbd_q.push_back(mbdt.q14);
        out_mbd_tt.push_back(mbdt.tt15); out_mbd_tq.push_back(mbdt.tq15); out_mbd_q.push_back(mbdt.q15);
        out_mbd_tt.push_back(mbdt.tt16); out_mbd_tq.push_back(mbdt.tq16); out_mbd_q.push_back(mbdt.q16);
        out_mbd_tt.push_back(mbdt.tt17); out_mbd_tq.push_back(mbdt.tq17); out_mbd_q.push_back(mbdt.q17);
        out_mbd_tt.push_back(mbdt.tt18); out_mbd_tq.push_back(mbdt.tq18); out_mbd_q.push_back(mbdt.q18);
        out_mbd_tt.push_back(mbdt.tt19); out_mbd_tq.push_back(mbdt.tq19); out_mbd_q.push_back(mbdt.q19);
        out_mbd_tt.push_back(mbdt.tt20); out_mbd_tq.push_back(mbdt.tq20); out_mbd_q.push_back(mbdt.q20);
        out_mbd_tt.push_back(mbdt.tt21); out_mbd_tq.push_back(mbdt.tq21); out_mbd_q.push_back(mbdt.q21);
        out_mbd_tt.push_back(mbdt.tt22); out_mbd_tq.push_back(mbdt.tq22); out_mbd_q.push_back(mbdt.q22);
        out_mbd_tt.push_back(mbdt.tt23); out_mbd_tq.push_back(mbdt.tq23); out_mbd_q.push_back(mbdt.q23);
        out_mbd_tt.push_back(mbdt.tt24); out_mbd_tq.push_back(mbdt.tq24); out_mbd_q.push_back(mbdt.q24);
        out_mbd_tt.push_back(mbdt.tt25); out_mbd_tq.push_back(mbdt.tq25); out_mbd_q.push_back(mbdt.q25);
        out_mbd_tt.push_back(mbdt.tt26); out_mbd_tq.push_back(mbdt.tq26); out_mbd_q.push_back(mbdt.q26);
        out_mbd_tt.push_back(mbdt.tt27); out_mbd_tq.push_back(mbdt.tq27); out_mbd_q.push_back(mbdt.q27);
        out_mbd_tt.push_back(mbdt.tt28); out_mbd_tq.push_back(mbdt.tq28); out_mbd_q.push_back(mbdt.q28);
        out_mbd_tt.push_back(mbdt.tt29); out_mbd_tq.push_back(mbdt.tq29); out_mbd_q.push_back(mbdt.q29);
        out_mbd_tt.push_back(mbdt.tt30); out_mbd_tq.push_back(mbdt.tq30); out_mbd_q.push_back(mbdt.q30);
        out_mbd_tt.push_back(mbdt.tt31); out_mbd_tq.push_back(mbdt.tq31); out_mbd_q.push_back(mbdt.q31);
        out_mbd_tt.push_back(mbdt.tt32); out_mbd_tq.push_back(mbdt.tq32); out_mbd_q.push_back(mbdt.q32);
        out_mbd_tt.push_back(mbdt.tt33); out_mbd_tq.push_back(mbdt.tq33); out_mbd_q.push_back(mbdt.q33);
        out_mbd_tt.push_back(mbdt.tt34); out_mbd_tq.push_back(mbdt.tq34); out_mbd_q.push_back(mbdt.q34);
        out_mbd_tt.push_back(mbdt.tt35); out_mbd_tq.push_back(mbdt.tq35); out_mbd_q.push_back(mbdt.q35);
        out_mbd_tt.push_back(mbdt.tt36); out_mbd_tq.push_back(mbdt.tq36); out_mbd_q.push_back(mbdt.q36);
        out_mbd_tt.push_back(mbdt.tt37); out_mbd_tq.push_back(mbdt.tq37); out_mbd_q.push_back(mbdt.q37);
        out_mbd_tt.push_back(mbdt.tt38); out_mbd_tq.push_back(mbdt.tq38); out_mbd_q.push_back(mbdt.q38);
        out_mbd_tt.push_back(mbdt.tt39); out_mbd_tq.push_back(mbdt.tq39); out_mbd_q.push_back(mbdt.q39);
        out_mbd_tt.push_back(mbdt.tt40); out_mbd_tq.push_back(mbdt.tq40); out_mbd_q.push_back(mbdt.q40);
        out_mbd_tt.push_back(mbdt.tt41); out_mbd_tq.push_back(mbdt.tq41); out_mbd_q.push_back(mbdt.q41);
        out_mbd_tt.push_back(mbdt.tt42); out_mbd_tq.push_back(mbdt.tq42); out_mbd_q.push_back(mbdt.q42);
        out_mbd_tt.push_back(mbdt.tt43); out_mbd_tq.push_back(mbdt.tq43); out_mbd_q.push_back(mbdt.q43);
        out_mbd_tt.push_back(mbdt.tt44); out_mbd_tq.push_back(mbdt.tq44); out_mbd_q.push_back(mbdt.q44);
        out_mbd_tt.push_back(mbdt.tt45); out_mbd_tq.push_back(mbdt.tq45); out_mbd_q.push_back(mbdt.q45);
        out_mbd_tt.push_back(mbdt.tt46); out_mbd_tq.push_back(mbdt.tq46); out_mbd_q.push_back(mbdt.q46);
        out_mbd_tt.push_back(mbdt.tt47); out_mbd_tq.push_back(mbdt.tq47); out_mbd_q.push_back(mbdt.q47);
        out_mbd_tt.push_back(mbdt.tt48); out_mbd_tq.push_back(mbdt.tq48); out_mbd_q.push_back(mbdt.q48);
        out_mbd_tt.push_back(mbdt.tt49); out_mbd_tq.push_back(mbdt.tq49); out_mbd_q.push_back(mbdt.q49);
        out_mbd_tt.push_back(mbdt.tt50); out_mbd_tq.push_back(mbdt.tq50); out_mbd_q.push_back(mbdt.q50);
        out_mbd_tt.push_back(mbdt.tt51); out_mbd_tq.push_back(mbdt.tq51); out_mbd_q.push_back(mbdt.q51);
        out_mbd_tt.push_back(mbdt.tt52); out_mbd_tq.push_back(mbdt.tq52); out_mbd_q.push_back(mbdt.q52);
        out_mbd_tt.push_back(mbdt.tt53); out_mbd_tq.push_back(mbdt.tq53); out_mbd_q.push_back(mbdt.q53);
        out_mbd_tt.push_back(mbdt.tt54); out_mbd_tq.push_back(mbdt.tq54); out_mbd_q.push_back(mbdt.q54);
        out_mbd_tt.push_back(mbdt.tt55); out_mbd_tq.push_back(mbdt.tq55); out_mbd_q.push_back(mbdt.q55);
        out_mbd_tt.push_back(mbdt.tt56); out_mbd_tq.push_back(mbdt.tq56); out_mbd_q.push_back(mbdt.q56);
        out_mbd_tt.push_back(mbdt.tt57); out_mbd_tq.push_back(mbdt.tq57); out_mbd_q.push_back(mbdt.q57);
        out_mbd_tt.push_back(mbdt.tt58); out_mbd_tq.push_back(mbdt.tq58); out_mbd_q.push_back(mbdt.q58);
        out_mbd_tt.push_back(mbdt.tt59); out_mbd_tq.push_back(mbdt.tq59); out_mbd_q.push_back(mbdt.q59);
        out_mbd_tt.push_back(mbdt.tt60); out_mbd_tq.push_back(mbdt.tq60); out_mbd_q.push_back(mbdt.q60);
        out_mbd_tt.push_back(mbdt.tt61); out_mbd_tq.push_back(mbdt.tq61); out_mbd_q.push_back(mbdt.q61);
        out_mbd_tt.push_back(mbdt.tt62); out_mbd_tq.push_back(mbdt.tq62); out_mbd_q.push_back(mbdt.q62);
        out_mbd_tt.push_back(mbdt.tt63); out_mbd_tq.push_back(mbdt.tq63); out_mbd_q.push_back(mbdt.q63);
        out_mbd_tt.push_back(mbdt.tt64); out_mbd_tq.push_back(mbdt.tq64); out_mbd_q.push_back(mbdt.q64);
        out_mbd_tt.push_back(mbdt.tt65); out_mbd_tq.push_back(mbdt.tq65); out_mbd_q.push_back(mbdt.q65);
        out_mbd_tt.push_back(mbdt.tt66); out_mbd_tq.push_back(mbdt.tq66); out_mbd_q.push_back(mbdt.q66);
        out_mbd_tt.push_back(mbdt.tt67); out_mbd_tq.push_back(mbdt.tq67); out_mbd_q.push_back(mbdt.q67);
        out_mbd_tt.push_back(mbdt.tt68); out_mbd_tq.push_back(mbdt.tq68); out_mbd_q.push_back(mbdt.q68);
        out_mbd_tt.push_back(mbdt.tt69); out_mbd_tq.push_back(mbdt.tq69); out_mbd_q.push_back(mbdt.q69);
        out_mbd_tt.push_back(mbdt.tt70); out_mbd_tq.push_back(mbdt.tq70); out_mbd_q.push_back(mbdt.q70);
        out_mbd_tt.push_back(mbdt.tt71); out_mbd_tq.push_back(mbdt.tq71); out_mbd_q.push_back(mbdt.q71);
        out_mbd_tt.push_back(mbdt.tt72); out_mbd_tq.push_back(mbdt.tq72); out_mbd_q.push_back(mbdt.q72);
        out_mbd_tt.push_back(mbdt.tt73); out_mbd_tq.push_back(mbdt.tq73); out_mbd_q.push_back(mbdt.q73);
        out_mbd_tt.push_back(mbdt.tt74); out_mbd_tq.push_back(mbdt.tq74); out_mbd_q.push_back(mbdt.q74);
        out_mbd_tt.push_back(mbdt.tt75); out_mbd_tq.push_back(mbdt.tq75); out_mbd_q.push_back(mbdt.q75);
        out_mbd_tt.push_back(mbdt.tt76); out_mbd_tq.push_back(mbdt.tq76); out_mbd_q.push_back(mbdt.q76);
        out_mbd_tt.push_back(mbdt.tt77); out_mbd_tq.push_back(mbdt.tq77); out_mbd_q.push_back(mbdt.q77);
        out_mbd_tt.push_back(mbdt.tt78); out_mbd_tq.push_back(mbdt.tq78); out_mbd_q.push_back(mbdt.q78);
        out_mbd_tt.push_back(mbdt.tt79); out_mbd_tq.push_back(mbdt.tq79); out_mbd_q.push_back(mbdt.q79);
        out_mbd_tt.push_back(mbdt.tt80); out_mbd_tq.push_back(mbdt.tq80); out_mbd_q.push_back(mbdt.q80);
        out_mbd_tt.push_back(mbdt.tt81); out_mbd_tq.push_back(mbdt.tq81); out_mbd_q.push_back(mbdt.q81);
        out_mbd_tt.push_back(mbdt.tt82); out_mbd_tq.push_back(mbdt.tq82); out_mbd_q.push_back(mbdt.q82);
        out_mbd_tt.push_back(mbdt.tt83); out_mbd_tq.push_back(mbdt.tq83); out_mbd_q.push_back(mbdt.q83);
        out_mbd_tt.push_back(mbdt.tt84); out_mbd_tq.push_back(mbdt.tq84); out_mbd_q.push_back(mbdt.q84);
        out_mbd_tt.push_back(mbdt.tt85); out_mbd_tq.push_back(mbdt.tq85); out_mbd_q.push_back(mbdt.q85);
        out_mbd_tt.push_back(mbdt.tt86); out_mbd_tq.push_back(mbdt.tq86); out_mbd_q.push_back(mbdt.q86);
        out_mbd_tt.push_back(mbdt.tt87); out_mbd_tq.push_back(mbdt.tq87); out_mbd_q.push_back(mbdt.q87);
        out_mbd_tt.push_back(mbdt.tt88); out_mbd_tq.push_back(mbdt.tq88); out_mbd_q.push_back(mbdt.q88);
        out_mbd_tt.push_back(mbdt.tt89); out_mbd_tq.push_back(mbdt.tq89); out_mbd_q.push_back(mbdt.q89);
        out_mbd_tt.push_back(mbdt.tt90); out_mbd_tq.push_back(mbdt.tq90); out_mbd_q.push_back(mbdt.q90);
        out_mbd_tt.push_back(mbdt.tt91); out_mbd_tq.push_back(mbdt.tq91); out_mbd_q.push_back(mbdt.q91);
        out_mbd_tt.push_back(mbdt.tt92); out_mbd_tq.push_back(mbdt.tq92); out_mbd_q.push_back(mbdt.q92);
        out_mbd_tt.push_back(mbdt.tt93); out_mbd_tq.push_back(mbdt.tq93); out_mbd_q.push_back(mbdt.q93);
        out_mbd_tt.push_back(mbdt.tt94); out_mbd_tq.push_back(mbdt.tq94); out_mbd_q.push_back(mbdt.q94);
        out_mbd_tt.push_back(mbdt.tt95); out_mbd_tq.push_back(mbdt.tq95); out_mbd_q.push_back(mbdt.q95);
        out_mbd_tt.push_back(mbdt.tt96); out_mbd_tq.push_back(mbdt.tq96); out_mbd_q.push_back(mbdt.q96);
        out_mbd_tt.push_back(mbdt.tt97); out_mbd_tq.push_back(mbdt.tq97); out_mbd_q.push_back(mbdt.q97);
        out_mbd_tt.push_back(mbdt.tt98); out_mbd_tq.push_back(mbdt.tq98); out_mbd_q.push_back(mbdt.q98);
        out_mbd_tt.push_back(mbdt.tt99); out_mbd_tq.push_back(mbdt.tq99); out_mbd_q.push_back(mbdt.q99);
        out_mbd_tt.push_back(mbdt.tt100); out_mbd_tq.push_back(mbdt.tq100); out_mbd_q.push_back(mbdt.q100);
        out_mbd_tt.push_back(mbdt.tt101); out_mbd_tq.push_back(mbdt.tq101); out_mbd_q.push_back(mbdt.q101);
        out_mbd_tt.push_back(mbdt.tt102); out_mbd_tq.push_back(mbdt.tq102); out_mbd_q.push_back(mbdt.q102);
        out_mbd_tt.push_back(mbdt.tt103); out_mbd_tq.push_back(mbdt.tq103); out_mbd_q.push_back(mbdt.q103);
        out_mbd_tt.push_back(mbdt.tt104); out_mbd_tq.push_back(mbdt.tq104); out_mbd_q.push_back(mbdt.q104);
        out_mbd_tt.push_back(mbdt.tt105); out_mbd_tq.push_back(mbdt.tq105); out_mbd_q.push_back(mbdt.q105);
        out_mbd_tt.push_back(mbdt.tt106); out_mbd_tq.push_back(mbdt.tq106); out_mbd_q.push_back(mbdt.q106);
        out_mbd_tt.push_back(mbdt.tt107); out_mbd_tq.push_back(mbdt.tq107); out_mbd_q.push_back(mbdt.q107);
        out_mbd_tt.push_back(mbdt.tt108); out_mbd_tq.push_back(mbdt.tq108); out_mbd_q.push_back(mbdt.q108);
        out_mbd_tt.push_back(mbdt.tt109); out_mbd_tq.push_back(mbdt.tq109); out_mbd_q.push_back(mbdt.q109);
        out_mbd_tt.push_back(mbdt.tt110); out_mbd_tq.push_back(mbdt.tq110); out_mbd_q.push_back(mbdt.q110);
        out_mbd_tt.push_back(mbdt.tt111); out_mbd_tq.push_back(mbdt.tq111); out_mbd_q.push_back(mbdt.q111);
        out_mbd_tt.push_back(mbdt.tt112); out_mbd_tq.push_back(mbdt.tq112); out_mbd_q.push_back(mbdt.q112);
        out_mbd_tt.push_back(mbdt.tt113); out_mbd_tq.push_back(mbdt.tq113); out_mbd_q.push_back(mbdt.q113);
        out_mbd_tt.push_back(mbdt.tt114); out_mbd_tq.push_back(mbdt.tq114); out_mbd_q.push_back(mbdt.q114);
        out_mbd_tt.push_back(mbdt.tt115); out_mbd_tq.push_back(mbdt.tq115); out_mbd_q.push_back(mbdt.q115);
        out_mbd_tt.push_back(mbdt.tt116); out_mbd_tq.push_back(mbdt.tq116); out_mbd_q.push_back(mbdt.q116);
        out_mbd_tt.push_back(mbdt.tt117); out_mbd_tq.push_back(mbdt.tq117); out_mbd_q.push_back(mbdt.q117);
        out_mbd_tt.push_back(mbdt.tt118); out_mbd_tq.push_back(mbdt.tq118); out_mbd_q.push_back(mbdt.q118);
        out_mbd_tt.push_back(mbdt.tt119); out_mbd_tq.push_back(mbdt.tq119); out_mbd_q.push_back(mbdt.q119);
        out_mbd_tt.push_back(mbdt.tt120); out_mbd_tq.push_back(mbdt.tq120); out_mbd_q.push_back(mbdt.q120);
        out_mbd_tt.push_back(mbdt.tt121); out_mbd_tq.push_back(mbdt.tq121); out_mbd_q.push_back(mbdt.q121);
        out_mbd_tt.push_back(mbdt.tt122); out_mbd_tq.push_back(mbdt.tq122); out_mbd_q.push_back(mbdt.q122);
        out_mbd_tt.push_back(mbdt.tt123); out_mbd_tq.push_back(mbdt.tq123); out_mbd_q.push_back(mbdt.q123);
        out_mbd_tt.push_back(mbdt.tt124); out_mbd_tq.push_back(mbdt.tq124); out_mbd_q.push_back(mbdt.q124);
        out_mbd_tt.push_back(mbdt.tt125); out_mbd_tq.push_back(mbdt.tq125); out_mbd_q.push_back(mbdt.q125);
        out_mbd_tt.push_back(mbdt.tt126); out_mbd_tq.push_back(mbdt.tq126); out_mbd_q.push_back(mbdt.q126);
        out_mbd_tt.push_back(mbdt.tt127); out_mbd_tq.push_back(mbdt.tq127); out_mbd_q.push_back(mbdt.q127);
        

        out_eID = intt_eID;
        out_bco_full = intt_bco_full;
        out_N_cluster_inner = intt_N_cluster_inner;
        out_N_cluster_outer = intt_N_cluster_outer;
        out_ES_zvtx = intt_ES_zvtx;
        out_ES_zvtxE = intt_ES_zvtxE;
        out_ES_rangeL = intt_ES_rangeL;
        out_ES_rangeR = intt_ES_rangeR;
        out_ES_N_good = intt_ES_N_good;
        out_ES_width_density = intt_ES_width_density;
        out_LB_Gaus_mean = intt_LB_Gaus_mean;
        out_LB_Gaus_meanE = intt_LB_Gaus_meanE;
        out_LB_Gaus_width = intt_LB_Gaus_width;
        out_LB_Gaus_offset = intt_LB_Gaus_offset;
        out_LB_Gaus_chi2 = intt_LB_Gaus_chi2;
        out_LB_Gaus_size_width = intt_LB_Gaus_size_width;
        out_LB_geo_mean = intt_LB_geo_mean;
        out_good_zvtx_tag = intt_good_zvtx_tag;
        out_MC_true_zvtx = MC_true_zvtx;
        out_N_group = intt_N_group;

        if (intt_N_cluster_inner != -1 && intt_N_cluster_outer != -1){
            if (mbdt.bqs > 100 && mbdt.bqn > 100){
                h_qmbd_nintt -> Fill(intt_N_cluster_inner + intt_N_cluster_outer, bbcq);
            }
            
            intt_mbd_bco -> Fill(i, (mbdclk - bco16) & 0xFFFF);

            tree_out -> Fill();
        }
        
        out_mbd_tt.clear();
        out_mbd_tq.clear();
        out_mbd_q.clear();

        if ((i % 1000) == 0) {
            cout << i << " " << hex << setw(6) << mbdclk << " " << setw(6) << bco16 << " (mbd-intt)" << setw(6) << ((mbdclk - bco16) & 0xFFFF) <<
                "      (femclk-prev)" << setw(6) << mbd_prvdif << " (bco-prev)" << setw(6) << intt_prvdif << dec << endl;
        }

        // note : if the total access event touches the last event of the INTT tree, break the loop.
        if (i + intt_evt_offset == number_Nevent) {break;}

        t_intt -> GetEntry(i + 1 + intt_evt_offset);
        ULong64_t next_bco16 = (intt_bco_full) & 0xFFFF;
        mbdt.LoadTree(i + 1);
        mbdt.GetEntry(i + 1);
        unsigned short next_mbdclk = mbdt.femclk;
        if (((next_mbdclk - next_bco16) & 0xFFFF) != ((mbdclk - bco16) & 0xFFFF)) intt_evt_offset += 1;
    }

    out_file -> cd();
    tree_out->SetDirectory(out_file);
    tree_out -> Write("", TObject::kOverwrite);
    out_file -> Close();
    
    TFile * froot = new TFile(Form("%s/sync.root",folder_direction.c_str()), "recreate");
    h_qmbd_nintt -> Write();
    intt_mbd_bco -> Write();
    froot -> Close();

    
}
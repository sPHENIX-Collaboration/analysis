#ifndef INTT_CALO_TRKUSER_H
#define INTT_CALO_TRKUSER_H

void INTT_Calo_trkUser_Init(void);
void INTT_Calo_trkUser_End(void);
void INTT_Calo_trkUser(void);

TH1F *h1zgvtx;
TH1F *h1ntrk;
TH1F *h1pt0e;
TH1F *h1pt0m;
TH1F *h1pt0e2;
TH1F *h1emc_e;
TH1F *h1E_p1;
TH1F *h1E_p2;
TH1F *h1E_p3;
TH1F *h1E_p4;
TH1F *h1E_p5;
TH1F *h1E_p6;
TH1F *h1E_p7;
TH1F *h1E_p8;

TH1F *h1n_tr_trk;
TH1F *h1tr_pt;
TH1F *h1tr_pt1;

TH1F *h1pt0e2_tr_pt1;
TH1F *h1pt0e2_tr_pt2;
TH1F *h1pt0e2_tr_pt3;
TH1F *h1pt0e2_tr_pt4;
TH1F *h1pt0e2_tr_pt5;
TH1F *h1pt0e2_tr_pt6;
TH1F *h1pt0e2_tr_pt7;
TH1F *h1pt0e2_tr_pt8;
TH1F *h1pt0e2_tr_pt9;
TH1F *h1pt0e2_tr_pt10;

TH1F *h1pt0e2_tr_pt3a;
TH1F *h1pt0e2_tr_pt4a;
TH1F *h1pt0e2_tr_pt5a;
TH1F *h1pt0e2_tr_pt6a;

TNtuple *nt_sim_trk;
#endif

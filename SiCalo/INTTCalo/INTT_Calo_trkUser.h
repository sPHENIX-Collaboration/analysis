#ifndef INTT_CALO_TRKUSER_H
#define INTT_CALO_TRKUSER_H

void INTT_Calo_trkUser_Init(void);
void INTT_Calo_trkUser_End(void);
void INTT_Calo_trkUser(void);

TH1F *hUSR_ntrk;
TH1F *hUSR_EmcIntt_pt;
TH1F *hUSR_EmcIntt_pt2;
TH1F *hUSR_EmcIntt_pt3;
TH1F *hUSR_Intt_pt;
TH1F *hUSR_Intt_pt2;
TH1F *hUSR_Intt_pt3;

TH1F *hUSR_Rpt;
TH1F *hUSR_Rpt2;

TH1F *hUSR_nEmc;
TH1F *hUSR_ntower;
TH1F *hUSR_tower_e;

TH1F *hUSR_Intt_t;
TH1F *hUSR_Mvtx0_t;
TH1F *hUSR_Mvtx1_t;
TH1F *hUSR_Mvtx2_t;

TH1F *hUSR_EIM_dr;
TH1F *hUSR_EIM_dr2;
TH1F *hUSR_EIM_dz;
TH1F *hUSR_EIM_dz2;
TH1F *hUSR_EIM_dz3;

TH1F *hUSR_Mee;
TH1F *hUSR_pe;
TH1F *hUSR_Ep;

TNtuple *nt_trk;
#endif

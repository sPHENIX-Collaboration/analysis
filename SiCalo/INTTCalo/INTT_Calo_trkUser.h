#ifndef INTT_CALO_TRKUSER_H
#define INTT_CALO_TRKUSER_H

void INTT_Calo_trkUser_Init(void);
void INTT_Calo_trkUser_End(void);
void INTT_Calo_trkUser(void);

TH1F *hUSR_ntrk;
TH1F *hUSR_EmcIntt_pt;
TH1F *hUSR_Intt_pt;

TH1F *hUSR_nEmc;
TH1F *hUSR_ntower;
TH1F *hUSR_tower_e;

TH1F *hUSR_Intt_t;
TH1F *hUSR_Mvtx0_t;
TH1F *hUSR_Mvtx1_t;
TH1F *hUSR_Mvtx2_t;
#endif

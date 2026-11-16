#ifndef INTT_CALO_TRKUSER_H
#define INTT_CALO_TRKUSER_H

void INTT_Calo_trkUser_Init(void);
void INTT_Calo_trkUser_End(void);
void INTT_Calo_trkUser(void);

TH1F *h1zgvtx;
TH1F *h1ntrk;
TH1F *h1pt0trk;
TH1F *h1sicalo_dphi;
TH1F *h1sicalo_dz;
TH1F *h1nSiCalo;
TH1F *h1pt_SiCalo;
#endif

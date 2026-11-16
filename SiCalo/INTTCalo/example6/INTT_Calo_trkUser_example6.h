#ifndef INTT_CALO_TRKUSER_H
#define INTT_CALO_TRKUSER_H

void INTT_Calo_trkUser_Init(void);
void INTT_Calo_trkUser_End(void);
void INTT_Calo_trkUser(void);

TH1F *h1zgvtx;
TH1F *h1ntrk;
TH1F *h1pt_intt;
TH1F *h1pt0e;
TH1F *h1pt0m;
TH1F *h1pt0e2;

TH1F *h1zgvtx_MBD;
TH1F *h1ntrk_MBD;
TH1F *h1pt_intt_MBD;
TH1F *h1pt0e_MBD;
TH1F *h1pt0m_MBD;
TH1F *h1pt0e2_MBD;

#endif

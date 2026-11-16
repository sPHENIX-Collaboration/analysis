#ifndef INTT_CALO_TRKUSER_H
#define INTT_CALO_TRKUSER_H

void INTT_Calo_trkUser_Init(void);
void INTT_Calo_trkUser_End(void);
void INTT_Calo_trkUser(void);

TH1F *h1zgvtx;

TH1F *h1nEmc;
TH1F *h1Emc_r;
TH2F *h2Emc_eta_phi;
TH1F *h1Emc_e;

TH1F *h1nTop;
TH1F *h1Top_r;
TH2F *h2Top_eta_phi;
TH1F *h1Top_e;
TH1F *h1Top_emc_e;
TH1F *h1Top_ihc_e;
TH1F *h1Top_ohc_e;
TH1F *h1TopEmc_r;
TH2F *h2TopEmc_e_e;

TH1F *h1nINTT0;
TH2F *h2INTT0_z_phi;
TH1F *h1nINTT1;
TH2F *h2INTT1_z_phi;

TH1F *h1nMvtx0;
TH2F *h2Mvtx0_z_phi;
TH1F *h1nMvtx1;
TH2F *h2Mvtx1_z_phi;
TH1F *h1nMvtx2;
TH2F *h2Mvtx2_z_phi;

#endif

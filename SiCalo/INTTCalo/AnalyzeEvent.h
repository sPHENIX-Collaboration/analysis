#ifndef ANALYZEEVENT_H
#define ANALYZEEVENT_H

#include <TH1F.h>

void AnalyzeInit(void);
void AnalyzeEnd(void);
void AnalyzeEvent(void);

//Histogram files
TH1F *hzgvtx;
TH1F *hINTTdphi;
TH1F *hInttPair_r;
TH1F *hEmcIntt_dr;
TH1F *hEmcIntt_dr_Zcut;
TH1F *hdZe1;
TH1F *hdZe0;
TH1F *hTopEmc_dphi;
TH1F *hTopEmc_deta;
TH1F *hTopEmc_dphi_1;
TH1F *hTopEmc_dphi_2;
TH1F *hTopEmc_dphi_3;
TH1F *hTopEmc_deta_1;
TH1F *hTopEmc_deta_2;
TH1F *hTopEmc_deta_3;

#endif

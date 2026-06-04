#include <TFile.h>
#include <TH1F.h>
#include "INTT_Calo_Ana.h"
#include "AnalyzeEvent.h"
#include "INTT_Calo_trkReco.h"

void AnalyzeInit(void) {

  hzgvtx = new TH1F("hzgvtx","global Zvtx",100,-50,50);
  hINTTdphi = new TH1F("hINTTdphi","INTT1phi - INTT0phi",100,-0.1,0.1);
  hInttPair_r = new TH1F("hInttPair_r","r of INTT pair",100,0.,500);
  hEmcIntt_dr = new TH1F("hEmcIntt_dr","dR of Intt track and Emc hit",100,-50,50);
  hEmcIntt_dr_Zcut = new TH1F("hEmcIntt_dr_Zcut","dR of Intt track and Emc hit w Zcut",100,-50,50);
  hdZe1 = new TH1F("hdZe1","Z_proj(rINTT1) - Z_INTT1",100,-10.,10);
  hdZe0 = new TH1F("hdZe0","Z_proj(rINTT0) - Z_INTT0",100,-10.,10);

  hTopEmc_dphi = new TH1F("hTopEmc_dphi","Top_phi - Emc_phi",100,-0.1,0.1);
  hTopEmc_deta = new TH1F("hTopEmc_deta","Top_eta - Emc_eta",100,-0.1,0.1);

  hTopEmc_dphi_1 = new TH1F("hTopEmc_dphi_1","Top_phi - Emc_phi (r<93.6)",100,-0.1,0.1);
  hTopEmc_dphi_2 = new TH1F("hTopEmc_dphi_2","Top_phi - Emc_phi (93.6<r<218)",100,-0.1,0.1);
  hTopEmc_dphi_3 = new TH1F("hTopEmc_dphi_3","Top_phi - Emc_phi (218<r)",100,-0.5,0.5);

  hTopEmc_deta_1 = new TH1F("hTopEmc_deta_1","Top_eta - Emc_eta (r<93.6)",100,-0.1,0.1);
  hTopEmc_deta_2 = new TH1F("hTopEmc_deta_2","Top_eta - Emc_eta (93.6<r<218)",100,-0.1,0.1);
  hTopEmc_deta_3 = new TH1F("hTopEmc_deta_3","Top_eta - Emc_eta (218<r)",100,-0.5,0.5);
}

void AnalyzeEnd(void) {
  hzgvtx->Draw();
  hINTTdphi->Draw();

  TFile *hfile = new TFile("INTT_Calo_Ana.root","RECREATE");
  hzgvtx->Write();
  hINTTdphi->Write();
  hInttPair_r->Write();
  hEmcIntt_dr->Write();
  hEmcIntt_dr_Zcut->Write();
  hdZe1->Write();
  hdZe0->Write();

  hTopEmc_dphi->Write();
  hTopEmc_deta->Write();
  hTopEmc_dphi_1->Write();
  hTopEmc_dphi_2->Write();
  hTopEmc_dphi_3->Write();
  hTopEmc_deta_1->Write();
  hTopEmc_deta_2->Write();
  hTopEmc_deta_3->Write();
  
  hfile->Close();
}

void AnalyzeEvent(void) {
  hzgvtx->Fill(zgvtx);

  INTT_Calo_trkReco();

  int nPair=vInttPair.size();
  for(int i=0;i<nPair;i++) {
    InttPair pair=vInttPair.at(i);
    hInttPair_r->Fill(pair.r);
  }
}


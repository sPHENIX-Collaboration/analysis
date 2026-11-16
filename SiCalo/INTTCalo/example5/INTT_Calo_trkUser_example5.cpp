#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

/*  Example 5
 *  get SiSeed based Si-Calo tracks and filled histograms.
 */

void INTT_Calo_trkUser_Init(void) {
  /* this is empty now.
     This function is called at the beginning of the analysis only at once.
     Please fill your code for initalization of your analysis.
     For example, define TH1,TH2, TTrees etc that you want to fill.
   */
  h1zgvtx   = new TH1F("h1zgvtx","global zvtx",100,-25.,25);

  //Si Seed track
  h1ntrk    = new TH1F("h1ntrk","# of SiSeed",51,-0.5,50.5);
  h1pt0trk  = new TH1F("h1pt0trk","pt of SiSeed track",100,0.,5.0);

  h1sicalo_dphi = new TH1F("h1sicalo_dphi","dphi of Emc and SiSeed projection",100,-2.0,2.0);
  h1sicalo_dz = new TH1F("h1sicalo_dz","dz of Emc and SiSeed projection",100,-100.0,100.0);
  //SiCalo track (SiSeed matched with Emc)
  h1nSiCalo=  new TH1F("h1nSiCalo","#of SiCalo tracks",51,-0.5,50.5);
  h1pt_SiCalo = new TH1F("h1pt_SiCalo","pt of SiCalo tracks (GeV/c)",100.,0.,5.0);
}


void INTT_Calo_trkUser_End(void) {
  /* this is empty now
     This function is called at the end of analysis.
     Please fill your code that must be done at the end of your analysis.
     For example, ->Write() the TH1,TH2, etc that you filled
   */
  h1zgvtx->Write();

  h1ntrk->Write();
  h1pt0trk->Write();
  
  h1sicalo_dphi->Write();
  h1sicalo_dz->Write();

  h1nSiCalo->Write();
  h1pt_SiCalo->Write();
}

void INTT_Calo_trkUser(void) {
  /* this is empty now.
     This function is called for every event.
     Please fill your analysis code here.
   */

  h1zgvtx->Fill(zgvtx);   // global zvtx
  
  int ntrk = vtrk_pt0->size();
  h1ntrk->Fill(ntrk);
  
  int nSiCalo=0;
  int itrk=0;    
  while(itrk<ntrk) {
    h1pt0trk->Fill(vtrk_pt0->at(itrk));

    float sicalo_dphi = vtrk_sicalo_dphi->at(itrk);
    float sicalo_dz   = vtrk_sicalo_dz->at(itrk);

    h1sicalo_dphi->Fill(sicalo_dphi);
    h1sicalo_dz->Fill(sicalo_dz);

    if(abs(sicalo_dphi)<0.05 && abs(sicalo_dz)<5.0) {
      h1pt_SiCalo->Fill(vtrk_sicalo_pt->at(itrk));
      nSiCalo++;
    }
    itrk++;
  }//while(itrk)
  h1nSiCalo->Fill(nSiCalo);
}

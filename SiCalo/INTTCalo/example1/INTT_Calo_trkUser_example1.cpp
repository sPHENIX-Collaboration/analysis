#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

/*  Example 1
 *  get INTT-based reconstructed tracks
 *  and filled historgrams of the tracks
 *  INTT-based track objects are in
 *      vector<CaloInttMvtx> vCaloInttMvtx
 *  which is defined in INTT_Calo_trkReco.h
 */

void INTT_Calo_trkUser_Init(void) {
  /* this is empty now.
     This function is called at the beginning of the analysis only at once.
     Please fill your code for initalization of your analysis.
     For example, define TH1,TH2, TTrees etc that you want to fill.
   */
  h1zgvtx = new TH1F("h1zgvtx","global zvtx",100,-25.,25);
  h1ntrk = new TH1F("h1ntrk","number of Calo-ITT tracks",51,-0.5,50.5);
  h1r_intt = new TH1F("h1r_intt","radius of the circle of (INTT1,INTT0,BC)",100,0,1000);
  h1pt_intt = new TH1F("h1pt_intt","pt from r_intt",100,0,5.0);
  h1pt0e = new TH1F("h1pt0e","pt of INTT-Calo tracks",100,0,5.0);
  h1pt0m = new TH1F("h1pt0m","pt of INTT-Calo tracks with Mvtx",100,0,5.0);
  h1pt0e2 = new TH1F("h1pt0e2","pt of INTT-Calo tracks with Emc",100,0,5.0);

  nt_trk = new TNtuple("nt_trk","INTT track Ntuple","zgvtx:ntrk:pt_intt:pt0e:pt0m:pt0e2");
  
}


void INTT_Calo_trkUser_End(void) {
  /* this is empty now
     This function is called at the end of analysis.
     Please fill your code that must be done at the end of your analysis.
     For example, ->Write() the TH1,TH2, etc that you filled
   */
  h1zgvtx->Write();
  h1ntrk->Write();
  h1r_intt->Write();
  h1pt_intt->Write();
  h1pt0e->Write();
  h1pt0m->Write();
  h1pt0e2->Write();

  nt_trk->Write();
}

void INTT_Calo_trkUser(void) {
  /* this is empty now.
     This function is called for every event.
     Please fill your analysis code here.
   */

  h1zgvtx->Fill(zgvtx);   // global zvtx
  
  // Get INTT based tracks from vCaloInttMvtx, and filled
  // its momentum etc
  int ntrk = vCaloInttMvtx.size();   // ntrk: # of INTT based tracks
  h1ntrk->Fill(ntrk);

  //for each reconstructed tracks
  int itrk=0;
  while(itrk < ntrk) {
    CaloInttMvtx CIMtrack = vCaloInttMvtx.at(itrk);
    float r_intt = CIMtrack.r_intt;  //radius of (INTT1,INTT0,BC)
    float pt0e   = CIMtrack.pt0e;    //pt of emc-intt track. -99 if no matching Emc cluster.
    float pt0m   = CIMtrack.pt0m;    //pt of intt-mvtx track. -99 if no matching Mvtx hit.
    float pt0e2  = CIMtrack.pt0e2;   //pt of emc-intt track with "dphi" method

    h1r_intt->Fill(r_intt);
    
    float pt_intt = -99.;
    if(r_intt > 0) pt_intt = 0.0042*r_intt;
    if(pt_intt>0) h1pt_intt->Fill(pt_intt);
    if(pt0e>0) h1pt0e->Fill(pt0e);
    if(pt0m>0) h1pt0m->Fill(pt0m);
    if(pt0e2>0) h1pt0e2->Fill(pt0e2);

    nt_trk->Fill(zgvtx,ntrk,pt_intt,pt0e,pt0m,pt0e2);

    itrk++;
  }
}

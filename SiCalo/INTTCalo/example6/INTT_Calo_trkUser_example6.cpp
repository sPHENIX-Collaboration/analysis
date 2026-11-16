#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

/*  Example 6
 *  This is same as Example 1, but with trigger selection.
 *  get INTT-based reconstructed tracks
 *  and filled historgrams of the tracks
 *  Only MBD trigger events are selected.
 *
 *  INTT-based track objects are in
 *      vector<CaloInttMvtx> vCaloInttMvtx
 *  which is defined in INTT_Calo_trkReco.h
 *
 *  Trigger information is in 
 */

void INTT_Calo_trkUser_Init(void) {
  /* this is empty now.
     This function is called at the beginning of the analysis only at once.
     Please fill your code for initalization of your analysis.
     For example, define TH1,TH2, TTrees etc that you want to fill.
   */
  h1zgvtx = new TH1F("h1zgvtx","global zvtx",100,-25.,25);
  h1zgvtx_MBD = new TH1F("h1zgvtx_MBD","global zvtx (w MBD trig)",100,-25.,25);

  h1ntrk = new TH1F("h1ntrk","number of Calo-ITT tracks",51,-0.5,50.5);
  h1ntrk_MBD = new TH1F("h1ntrk_MBD","number of Calo-ITT tracks (w MBD trig)",51,-0.5,50.5);

  h1pt_intt = new TH1F("h1pt_intt","pt from r_intt",100,0,5.0);
  h1pt_intt_MBD = new TH1F("h1pt_intt_MBD","pt from r_intt (w MBD trig)",100,0,5.0);
  h1pt0e = new TH1F("h1pt0e","pt of INTT-Calo tracks",100,0,5.0);
  h1pt0e_MBD = new TH1F("h1pt0e_MBD","pt of INTT-Calo tracks (w MBD)",100,0,5.0);
  h1pt0m = new TH1F("h1pt0m","pt of INTT-Calo tracks with Mvtx",100,0,5.0);
  h1pt0m_MBD = new TH1F("h1pt0m_MBD","pt of INTT-Calo tracks with Mvtx (w MBD)",100,0,5.0);
  h1pt0e2 = new TH1F("h1pt0e2","pt of INTT-Calo tracks with Emc",100,0,5.0);
  h1pt0e2_MBD = new TH1F("h1pt0e2_MBD","pt of INTT-Calo tracks with Emc (w MBD)",100,0,5.0);
  
}


void INTT_Calo_trkUser_End(void) {
  /* this is empty now
     This function is called at the end of analysis.
     Please fill your code that must be done at the end of your analysis.
     For example, ->Write() the TH1,TH2, etc that you filled
   */
  h1zgvtx->Write();
  h1ntrk->Write();
  h1pt_intt->Write();
  h1pt0e->Write();
  h1pt0m->Write();
  h1pt0e2->Write();

  h1zgvtx_MBD->Write();
  h1ntrk_MBD->Write();
  h1pt_intt_MBD->Write();
  h1pt0e_MBD->Write();
  h1pt0m_MBD->Write();
  h1pt0e2_MBD->Write();
}

void INTT_Calo_trkUser(void) {
  /* this is empty now.
     This function is called for every event.
     Please fill your analysis code here.
   */
  const long MBD_vtx10cm = 4096;


  //  cout << "scaledtrig="<<scaledtrig<<endl;

  
  h1zgvtx->Fill(zgvtx);   // global zvtx
  if((scaledtrig & MBD_vtx10cm) != 0) h1zgvtx_MBD->Fill(zgvtx);
  
  // Get INTT based tracks from vCaloInttMvtx, and filled
  // its momentum etc
  int ntrk = vCaloInttMvtx.size();   // ntrk: # of INTT based tracks
  h1ntrk->Fill(ntrk);
  if((scaledtrig & MBD_vtx10cm) != 0) h1ntrk_MBD->Fill(ntrk);

  //for each reconstructed tracks
  int itrk=0;
  while(itrk < ntrk) {
    CaloInttMvtx CIMtrack = vCaloInttMvtx.at(itrk);
    float r_intt = CIMtrack.r_intt;  //radius of (INTT1,INTT0,BC)
    float pt0e   = CIMtrack.pt0e;    //pt of emc-intt track. -99 if no matching Emc cluster.
    float pt0m   = CIMtrack.pt0m;    //pt of intt-mvtx track. -99 if no matching Mvtx hit.
    float pt0e2  = CIMtrack.pt0e2;   //pt of emc-intt track with "dphi" method

    float pt_intt = -99.;
    if(r_intt > 0) pt_intt = 0.0042*r_intt;

    if(pt_intt>0) h1pt_intt->Fill(pt_intt);
    if(pt0e>0) h1pt0e->Fill(pt0e);
    if(pt0m>0) h1pt0m->Fill(pt0m);
    if(pt0e2>0) h1pt0e2->Fill(pt0e2);

    if((scaledtrig & MBD_vtx10cm) !=0) {
      if(pt_intt>0) h1pt_intt_MBD->Fill(pt_intt);
      if(pt0e>0) h1pt0e_MBD->Fill(pt0e);
      if(pt0m>0) h1pt0m_MBD->Fill(pt0m);
      if(pt0e2>0) h1pt0e2_MBD->Fill(pt0e2);
    }//if(MBD)

    itrk++;
  }
}

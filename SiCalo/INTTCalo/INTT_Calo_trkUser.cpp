#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

void INTT_Calo_trkUser_Init(void) {
  hUSR_ntrk = new TH1F("hUSR_ntrk","number of Calo-INTT tracks",
				 21,-0.5,20.5);
  hUSR_EmcIntt_pt = new TH1F("hUSR_EmcIntt_pt","pt of Calo-INTT track",
			     100,0.,10.);
  hUSR_Intt_pt = new TH1F("hUSR_Intt_pt","pt of INTT track",
			  100,0.,10.);
  hUSR_nEmc = new TH1F("hUSR_nEmc","# of Emc clusters",101,-0.5,100.5);
  hUSR_ntower = new TH1F("hUSR_ntower","# of towers in EMC cluster",
			 41.,-0.5,40.5);
  hUSR_tower_e = new TH1F("hUSR_tower_e","tower energy",100,0.0,5.0);
}

void INTT_Calo_trkUser_End(void) {
  hUSR_ntrk->Write();
  hUSR_Intt_pt->Write();
  hUSR_EmcIntt_pt->Write();
  hUSR_nEmc->Write();
  hUSR_ntower->Write();
  hUSR_tower_e->Write();
}

void INTT_Calo_trkUser(void) {
  int ntrk = vCaloInttMvtx.size();
  hUSR_ntrk->Fill(ntrk);

  int nEmc = emc_r->size();
  hUSR_nEmc->Fill(nEmc);
  if(nEmc>0) {
    for(int iemc=0;iemc<nEmc;iemc++) {
      vector<float> vtower_e= emc_tower_e->at(iemc);
      int ntower = vtower_e.size();
      hUSR_ntower->Fill(ntower);
      for(int itower=0;itower<ntower;itower++) {
	hUSR_tower_e->Fill(vtower_e.at(itower));
      }
    }
  }
	
  if(ntrk > 0) {
    /*
    cout << "i_event="<<i_event;
    cout << " ntrk="<<ntrk<<endl;
    */
    for(int i=0;i<ntrk;i++) {
      CaloInttMvtx CIMtrack = vCaloInttMvtx.at(i);
      float r_intt = CIMtrack.r_intt;
      float r_emc_intt = CIMtrack.r_emc_intt;
      float pt_emc_intt = CIMtrack.pt0e;
      float pt_intt = 0.0042*r_intt;
      float emc_e = CIMtrack.emc_e;
      float emcal_e = CIMtrack.emcal_e;
      float total_e = CIMtrack.total_e;
      float phi0 = CIMtrack.phi0e;
      float z0 = CIMtrack.z0e;
      hUSR_Intt_pt->Fill(pt_intt);
      hUSR_EmcIntt_pt->Fill(pt_emc_intt);
    }// for(i)
  }//if(ntrk)
}

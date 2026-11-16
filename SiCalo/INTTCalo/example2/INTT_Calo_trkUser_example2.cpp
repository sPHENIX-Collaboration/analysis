#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

/*  Example 2
 *  plot clusters in nDST and filled histograms of them
 */

void INTT_Calo_trkUser_Init(void) {
  /* this is empty now.
     This function is called at the beginning of the analysis only at once.
     Please fill your code for initalization of your analysis.
     For example, define TH1,TH2, TTrees etc that you want to fill.
   */

  h1zgvtx = new TH1F("h1zgvtx","global zvtx (cm)",100,-50.,50);
  
  h1nEmc  = new TH1F("h1nEmc","number of Emc clusters",100.,0.,100);
  h1Emc_r = new TH1F("h1Emc_r","Emc cluster r (cm)",100,90,110);
  h2Emc_eta_phi = new TH2F("h2Emc_eta_phi","(eta,phi) of Emc cluster",
			   120,-1.2,1.2,
			   140,-3.5,3.5);
  h1Emc_e = new TH1F("h1Emc_e","Emc cluster energy (GeV)",100,0.,10);
  
  h1nTop  = new TH1F("h1nTop","number of topoClusters",100.,0.,100);
  h1Top_r = new TH1F("h1Top_r","topoCluster r (cm)",160,90,250);
  h2Top_eta_phi = new TH2F("h2Top_eta_phi","(eta,phi) of topoCluster",
			   120,-1.2,1.2, 140,-3.5,3.5);
  h1Top_e     = new TH1F("h1Top_e","topoCluster total energy (GeV)",100,0.,10);
  h1Top_emc_e = new TH1F("h1Top_emc_e","topoCluser Emc energy(GeV)",220,-1.0,10.);
  h1Top_ihc_e = new TH1F("h1Top_ihc_e","topoCluser iHCal energy(GeV)",220,-1.0,10.);
  h1Top_ohc_e = new TH1F("h1Top_ohc_e","topoCluser oHCal energy(GeV)",220,-1.0,10.);
  h1TopEmc_r  = new TH1F("h1TopEmc_r","r of Emc associated with Topo",220,-110,110);
  h2TopEmc_e_e = new TH2F("h2TopEmc_e_e","Top_emc_e vs Top_emc_emc_e",
			  100,0.,10.,100,0.,10.);

  h1nINTT0  = new TH1F("h1nINTT0","number of INTT0 clusters",100.,0.,100);
  h2INTT0_z_phi = new TH2F("h2INTT0_z_phi","(z,phi) of INTT0 cluster",
			   250,-25.0,25.0, 140,-3.5,3.5);
  h1nINTT1  = new TH1F("h1nINTT1","number of INTT1 clusters",100.,0.,100);
  h2INTT1_z_phi = new TH2F("h2INTT1_z_phi","(z,phi) of INTT1 cluster",
			   250,-25.0,25.0, 140,-3.5,3.5);

  h1nMvtx0  = new TH1F("h1nMvtx0","number of Mvtx0 clusters",1000.,0.,1000);
  h2Mvtx0_z_phi = new TH2F("h2Mvtx0_z_phi","(z,phi) of INTT1 cluster",
			   150,-15.0,15.0, 140,-3.5,3.5);
  h1nMvtx1  = new TH1F("h1nMvtx1","number of Mvtx1 clusters",1000.,0.,1000);
  h2Mvtx1_z_phi = new TH2F("h2Mvtx1_z_phi","(z,phi) of INTT1 cluster",
			   150,-15.0,15.0, 140,-3.5,3.5);
  h1nMvtx2  = new TH1F("h1nMvtx2","number of Mvtx2 clusters",1000.,0.,1000);
  h2Mvtx2_z_phi = new TH2F("h2Mvtx2_z_phi","(z,phi) of INTT1 cluster",
			   150,-15.0,15.0, 140,-3.5,3.5);
}


void INTT_Calo_trkUser_End(void) {
  /* this is empty now
     This function is called at the end of analysis.
     Please fill your code that must be done at the end of your analysis.
     For example, ->Write() the TH1,TH2, etc that you filled
   */
  h1zgvtx->Write();   // global zvtx

  h1nEmc->Write();
  h1Emc_r->Write();
  h2Emc_eta_phi->Write();
  h1Emc_e->Write();

  h1nTop->Write();
  h1Top_r->Write();
  h2Top_eta_phi->Write();
  h1Top_e->Write();
  h1Top_emc_e->Write();
  h1Top_ihc_e->Write();
  h1Top_ohc_e->Write();
  h1TopEmc_r->Write();
  h2TopEmc_e_e->Write();

  h1nINTT0->Write();
  h2INTT0_z_phi->Write();
  h1nINTT1->Write();
  h2INTT1_z_phi->Write();

  h1nMvtx0->Write();
  h2Mvtx0_z_phi->Write();
  h1nMvtx1->Write();
  h2Mvtx1_z_phi->Write();
  h1nMvtx2->Write();
  h2Mvtx2_z_phi->Write();
}

void INTT_Calo_trkUser(void) {
  /* this is empty now.
     This function is called for every event.
     Please fill your analysis code here.
   */

  h1zgvtx->Fill(zgvtx);   // global zvtx
  
  // Fill Emc cluster histograms
  //
  // Note: vEmc_r.size = vEmc_phi.size() = vEmc_eta.size()
  int nEmc = vEmc_r.size();  // # of Emc clusters
  h1nEmc->Fill(nEmc);
  
  int iEmc = 0;
  while(iEmc<nEmc) {
    float Emc_r   = vEmc_r.at(iEmc);
    float Emc_phi = vEmc_phi.at(iEmc);
    float Emc_eta = vEmc_eta.at(iEmc);
    float Emc_e   = vEmc_e.at(iEmc);

    h1Emc_r->Fill(Emc_r);
    h2Emc_eta_phi->Fill(Emc_eta,Emc_phi);
    h1Emc_e->Fill(Emc_e);

    iEmc++;
  }//while(iEmc)

  // Fill TopCluster histograms
  int nTop = vTop_r.size();
  h1nTop->Fill(nTop);

  int iTop=0;
  while(iTop<nTop) {
    float Top_r     = vTop_r.at(iTop);
    float Top_phi   = vTop_phi.at(iTop);
    float Top_eta   = vTop_eta.at(iTop);
    float Top_e     = vTop_e.at(iTop);
    float Top_emc_e = vTop_emc_e.at(iTop);
    float Top_ihc_e = vTop_ihc_e.at(iTop);
    float Top_ohc_e = vTop_ohc_e.at(iTop);

    h1Top_r->Fill(Top_r);
    h2Top_eta_phi->Fill(Top_eta,Top_phi);
    h1Top_e->Fill(Top_e);
    h1Top_emc_e->Fill(Top_emc_e);
    h1Top_ihc_e->Fill(Top_ihc_e);
    h1Top_ohc_e->Fill(Top_ohc_e);

    //Emc cluster associated with TopoCluster
    float TopEmc_r = vTop_emc_r.at(iTop);
    float TopEmc_emc_e = vTop_emc_emc_e.at(iTop);

    //if an Emc cluster is associated with this topoCluster,
    // Top_emc_r is positive and it is the r of Emc cluster
    // otherwise, it is -99.
    h1TopEmc_r->Fill(TopEmc_r);
    if(TopEmc_r>0) {
      // Now an Emc cluster is associated with this topoCluster
      // compare the energy of the Emc cluster and
      // Emc part of TopoCluster energy
      h2TopEmc_e_e->Fill(TopEmc_emc_e,Top_emc_e);
    }
    
    iTop++;
  }//while(iTop)

  //Fill INTT0 and INTT1 histograms
  // INTT0
  int nINTT0 = vINTT0r.size();
  h1nINTT0->Fill(nINTT0);
  
  int i0=0;
  while(i0<nINTT0) {
    float INTT0phi = vINTT0phi.at(i0);
    float INTT0z   = vINTT0z.at(i0);
    
    h2INTT0_z_phi->Fill(INTT0z,INTT0phi);
    
    i0++;
  }//while(i0)

  // INTT1
  int nINTT1 = vINTT1r.size();
  h1nINTT1->Fill(nINTT1);
  
  int i1=0;
  while(i1<nINTT1) {
    float INTT1phi = vINTT1phi.at(i1);
    float INTT1z   = vINTT1z.at(i1);
    
    h2INTT1_z_phi->Fill(INTT1z,INTT1phi);
    
    i1++;
  }//while(i1)

  //Fill Mvtx0, Mvtx1, Mvtx2 histograms
  // Mvtx0
  int nMvtx0 = vMvtx0r.size();
  h1nMvtx0->Fill(nMvtx0);

  int im0=0;
  while(im0<nMvtx0) {
    float Mvtx0phi = vMvtx0phi.at(im0);
    float Mvtx0z   = vMvtx0z.at(im0);
    h2Mvtx0_z_phi->Fill(Mvtx0z,Mvtx0phi);
    
    im0++;
  }//while(im0)

  // Mvtx1
  int nMvtx1 = vMvtx1r.size();
  h1nMvtx1->Fill(nMvtx1);

  int im1=0;
  while(im1<nMvtx1) {
    float Mvtx1phi = vMvtx1phi.at(im1);
    float Mvtx1z   = vMvtx1z.at(im1);
    h2Mvtx1_z_phi->Fill(Mvtx1z,Mvtx1phi);
    
    im1++;
  }//while(im1)

  // Mvtx2
  int nMvtx2 = vMvtx2r.size();
  h1nMvtx2->Fill(nMvtx2);

  int im2=0;
  while(im2<nMvtx2) {
    float Mvtx2phi = vMvtx2phi.at(im2);
    float Mvtx2z   = vMvtx2z.at(im2);
    h2Mvtx2_z_phi->Fill(Mvtx2z,Mvtx2phi);
    
    im2++;
  }//while(im2)

}

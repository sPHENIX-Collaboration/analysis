#include "INTT_Calo_trkReco.h"
//#include "CalcEmcClusShape.h"
#include "INTT_Calo_trkUser.h"

/*  Example 3
 *  This example code is inteded to analyze 
 *  a single gun simulation nDST.
 *  Get INTT-based reconstructed tracks
 *  and compare them its "truth" information.
 */

void INTT_Calo_trkUser_Init(void) {
  /* this is empty now.
     This function is called at the beginning of the analysis only at once.
     Please fill your code for initalization of your analysis.
     For example, define TH1,TH2, TTrees etc that you want to fill.
   */
  h1zgvtx = new TH1F("h1zgvtx","global zvtx",100,-25.,25);
  h1ntrk = new TH1F("h1ntrk","number of Calo-ITT tracks",51,-0.5,50.5);
  h1pt0e = new TH1F("h1pt0e","pt of INTT-Calo tracks",150,0,15.0);
  h1pt0m = new TH1F("h1pt0m","pt of INTT-Calo tracks with Mvtx",150,0,15.0);
  h1pt0e2 = new TH1F("h1pt0e2","pt of INTT-Calo tracks with Emc",150,0,15.0);
  h1emc_e = new TH1F("h1emc_e","energy of Emc associated with a track",100,0,5.0);
  h1E_p1  = new TH1F("h1E_p1","E/p (0.3<pt<0.5 GeV/c)",100,0,2.0);
  h1E_p2  = new TH1F("h1E_p2","E/p (0.5<pt<0.8 GeV/c)",100,0,2.0);
  h1E_p3  = new TH1F("h1E_p3","E/p (0.8<pt<1.2 GeV/c)",100,0,2.0);
  h1E_p4  = new TH1F("h1E_p4","E/p (1.2<pt<1.5 GeV/c)",100,0,2.0);
  h1E_p5  = new TH1F("h1E_p5","E/p (1.5<pt<2.0 GeV/c)",100,0,2.0);
  h1E_p6  = new TH1F("h1E_p6","E/p (2.0<pt<3.0 GeV/c)",100,0,2.0);
  h1E_p7  = new TH1F("h1E_p7","E/p (3.0<pt<4.0 GeV/c)",100,0,2.0);
  h1E_p8  = new TH1F("h1E_p8","E/p (4.0<pt  (GeV/c))",100,0,2.0);


  h1n_tr_trk = new TH1F("h1n_tr_trk","# of truth track",51,-0.5,50.5);
  h1tr_pt = new TH1F("h1tr_pt","truth pt (GeV)",110,0,11.0);
  h1tr_pt1 = new TH1F("h1tr_pt1","truth pt (GeV) (ntrk==1)",110,0,11.0);

  h1pt0e2_tr_pt1 = new TH1F("h1pt0e2_tr_pt1","pt0e2/tr_pt (0.3<tr_pt<0.5 GeV/c)",100,0.0,2.0);
  h1pt0e2_tr_pt2 = new TH1F("h1pt0e2_tr_pt2","pt0e2/tr_pt (0.5<tr_pt<0.7 GeV/c)",100,0.0,2.0);
  h1pt0e2_tr_pt3 = new TH1F("h1pt0e2_tr_pt3","pt0e2/tr_pt (0.7<tr_pt<1.0 GeV/c)",100,0.0,2.0);
  h1pt0e2_tr_pt4 = new TH1F("h1pt0e2_tr_pt4","pt0e2/tr_pt (1.0<tr_pt<1.5 GeV/c)",100,0.0,2.0);
  h1pt0e2_tr_pt5 = new TH1F("h1pt0e2_tr_pt5","pt0e2/tr_pt (1.5<tr_pt<2.0 GeV/c)",100,0.0,2.0);
  h1pt0e2_tr_pt6 = new TH1F("h1pt0e2_tr_pt6","pt0e2/tr_pt (2.0<tr_pt<3.0 GeV/c)",100,0.0,2.0);
  h1pt0e2_tr_pt7 = new TH1F("h1pt0e2_tr_pt7","pt0e2/tr_pt (3.0<tr_pt<5.0 GeV/c)",100,0.0,2.0);
  h1pt0e2_tr_pt8 = new TH1F("h1pt0e2_tr_pt8","pt0e2/tr_pt (5.0<tr_pt<7.0 GeV/c)",100,0.0,2.0);
  h1pt0e2_tr_pt9 = new TH1F("h1pt0e2_tr_pt9","pt0e2/tr_pt (7.0<tr_pt<9.0 GeV/c)",100,0.0,2.0);
  h1pt0e2_tr_pt10 = new TH1F("h1pt0e2_tr_pt10","pt0e2/tr_pt (9.0<tr_pt (GeV/c))",100,0.0,2.0);    

  h1pt0e2_tr_pt3a = new TH1F("h1pt0e2_tr_pt3a","pt0e2/tr_pt (0.8<pt<1.2 GeV/c,0.8<E/p<1.3)",100,0.0,2.0);
  h1pt0e2_tr_pt4a = new TH1F("h1pt0e2_tr_pt4a","pt0e2/tr_pt (1.2<pt<1.5 GeV/c,0.8<E/p<1.3)",100,0.0,2.0);
  h1pt0e2_tr_pt5a = new TH1F("h1pt0e2_tr_pt5a","pt0e2/tr_pt (1.5<pt<2.0 GeV/c,0.8<E/p<1.3)",100,0.0,2.0);
  h1pt0e2_tr_pt6a = new TH1F("h1pt0e2_tr_pt6a","pt0e2/tr_pt (2.0<pt<3.0 GeV/c,0.8<E/p<1.3)",100,0.0,2.0);
  
  nt_sim_trk = new TNtuple("nt_sim_trk","Sim and Reco Ntuple",
			   "tr_pt:tr_phi0:tr_eta0:pt0e:phi_e:pt0m:emc_e:p0e2:phi0e:ihc_e:prob");  
}


void INTT_Calo_trkUser_End(void) {
  /* this is empty now
     This function is called at the end of analysis.
     Please fill your code that must be done at the end of your analysis.
     For example, ->Write() the TH1,TH2, etc that you filled
   */
  h1zgvtx->Write();
  h1ntrk->Write();
  h1pt0e->Write();
  h1pt0m->Write();
  h1pt0e2->Write();
  h1emc_e->Write();
  h1E_p1->Write();
  h1E_p2->Write();
  h1E_p3->Write();
  h1E_p4->Write();
  h1E_p5->Write();
  h1E_p6->Write();
  h1E_p7->Write();
  h1E_p8->Write();

  h1n_tr_trk->Write();
  h1tr_pt->Write();
  h1tr_pt1->Write();

  h1pt0e2_tr_pt1->Write();
  h1pt0e2_tr_pt2->Write();
  h1pt0e2_tr_pt3->Write();
  h1pt0e2_tr_pt4->Write();
  h1pt0e2_tr_pt5->Write();
  h1pt0e2_tr_pt6->Write();
  h1pt0e2_tr_pt7->Write();
  h1pt0e2_tr_pt8->Write();
  h1pt0e2_tr_pt9->Write();
  h1pt0e2_tr_pt10->Write();
  
  h1pt0e2_tr_pt3a->Write();
  h1pt0e2_tr_pt4a->Write();
  h1pt0e2_tr_pt5a->Write();
  h1pt0e2_tr_pt6a->Write();

  nt_sim_trk->Write();
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
    float pt0e   = CIMtrack.pt0e;    //pt of emc-intt track. -99 if no matching Emc cluster.
    float pt0m   = CIMtrack.pt0m;    //pt of intt-mvtx track. -99 if no matching Mvtx hit.
    float pt0e2  = CIMtrack.pt0e2;   //pt of emc-intt track with "dphi" method
    float pz0e2  = CIMtrack.pz0e2;   //pt of emc-intt track with "dphi" method
    float p0e2   = sqrt(pt0e2*pt0e2+pz0e2*pz0e2);
    float emc_e  = CIMtrack.emc_e;
	
    if(pt0e>0) h1pt0e->Fill(pt0e);
    if(pt0m>0) h1pt0m->Fill(pt0m);
    if(pt0e2>0) h1pt0e2->Fill(pt0e2);
    if(emc_e>0) {
      h1emc_e->Fill(emc_e);
      //Histograms of E/p
      h1pt0e2->Fill(pt0e2);
      if(pt0e2<0.3) {
      } else if(pt0e2<0.5) {
	h1E_p1->Fill(emc_e/p0e2);
      } else if(pt0e2<0.8) {
	h1E_p2->Fill(emc_e/p0e2);
      } else if(pt0e2<1.2) {
	h1E_p3->Fill(emc_e/p0e2);
      } else if(pt0e2<1.5) {
	h1E_p4->Fill(emc_e/p0e2);
      } else if(pt0e2<2.0) {
	h1E_p5->Fill(emc_e/p0e2);
      } else if(pt0e2<3.0) {
	h1E_p6->Fill(emc_e/p0e2);
      } else if(pt0e2<4.0) {
	h1E_p7->Fill(emc_e/p0e2);
      } else {
	h1E_p8->Fill(emc_e/p0e2);
      }//if(pt0e2)
    }//if(emc_e)
    itrk++;
  }

  //
  // Simulation
  //
  // Here it is assumed that the simulation is a single gun simulation.
  // therefore, nominally, there is only one true track in an event.
  // This is not true since more tracks can be created by the interaction.
  // But in this code, such cases are ignored.
  // This code only deal with when the number of truth tracks (n_tr_trk) is one,
  // and the number of reconstructed tracks (ntrk) is also one.
  // Then compare the truth information and the reconstructed one.
  //
  if(truthTree != nullptr) {
    int n_tr_trk = truth_pid->size(); //# of truth tracks

    h1n_tr_trk->Fill(n_tr_trk);
    float tr_px0=truth_px->at(0);
    float tr_py0=truth_py->at(0);
    float tr_pz0=truth_pz->at(0);
    float tr_x0 =truth_x->at(0);
    float tr_y0 =truth_y->at(0);
    float tr_r0 = sqrt(tr_x0*tr_x0+tr_y0*tr_y0);

    if(n_tr_trk>=1) {
      for(int i=1;i<n_tr_trk;i++) {
	float tr_x = truth_x->at(i);
	float tr_y = truth_y->at(i);
	float tr_r = sqrt(tr_x*tr_x+tr_y*tr_y);
	if( tr_r < tr_r0) {
	  tr_r0 = tr_r;
	  tr_px0 = truth_px->at(i);
	  tr_py0 = truth_py->at(i);
	  tr_pz0 = truth_pz->at(i);
	}
      }
    }
    
    float tr_pt0=sqrt(tr_px0*tr_px0+tr_py0*tr_py0);
    float tr_phi0 = atan2(tr_py0,tr_px0);
    float tr_eta0 = rz2eta(tr_pt0,tr_pz0);

    h1tr_pt->Fill(tr_pt0);
    if(ntrk == 1) {
      // truth information
      h1tr_pt1->Fill(tr_pt0);
      
      // reconstructed information
      CaloInttMvtx CIMtrack = vCaloInttMvtx.at(0);
      int   iTop = CIMtrack.iTop;

      //Here we further restrict to the case when
      //the reconstructed track has associated TopCluster hit.
      if(iTop>=0) {
	int   iEmc = CIMtrack.iEmc;
	float pt0m = CIMtrack.pt0m;
	float pt0e = CIMtrack.pt0e;
	float phi0e = CIMtrack.phi0e;
	float pt0e2 = CIMtrack.pt0e2;
	float pz0e2 = CIMtrack.pz0e2;
	float p0e2 = sqrt(pt0e2*pt0e2+pz0e2*pz0e2);
	float xemc = CIMtrack.xemc;
	float yemc = CIMtrack.yemc;
	float zemc = CIMtrack.zemc;
	float remc = sqrt(xemc*xemc+yemc*yemc);
	float emc_e = CIMtrack.emc_e;
	float ihc_e = CIMtrack.ihcal_e;
	float prob = CIMtrack.emc_prob;
	float phi_e = atan2(yemc,xemc);
	float eta_e = rz2eta(remc,zemc);
	int   sign = CIMtrack.sign;


	//Ntuple
	//	nt_sim_trk->Fill(tr_pt0,tr_phi0,tr_eta0,pt0e,phi_e,pt0m,emc_e,p0e2,phi0e,ihc_e,prob);

	//Histogram of reco_pt vs truth_pt
	if(tr_pt0>0.3) {
	  if(tr_pt0<0.5) {
	    h1pt0e2_tr_pt1->Fill(pt0e2/tr_pt0);
	  } else if(tr_pt0<0.7) {
	    h1pt0e2_tr_pt2->Fill(pt0e2/tr_pt0);
	  } else if(tr_pt0<1.0) {
	    h1pt0e2_tr_pt3->Fill(pt0e2/tr_pt0);
	  } else if(tr_pt0<1.5) {
	    h1pt0e2_tr_pt4->Fill(pt0e2/tr_pt0);
	  } else if(tr_pt0<2.0) {
	    h1pt0e2_tr_pt5->Fill(pt0e2/tr_pt0);
	  } else if(tr_pt0<3.0) {
	    h1pt0e2_tr_pt6->Fill(pt0e2/tr_pt0);
	  } else if(tr_pt0<5.0) {
	    h1pt0e2_tr_pt7->Fill(pt0e2/tr_pt0);
	  } else if(tr_pt0<7.0) {
	    h1pt0e2_tr_pt8->Fill(pt0e2/tr_pt0);
	  } else if(tr_pt0<9.0) {
	    h1pt0e2_tr_pt9->Fill(pt0e2/tr_pt0);
	  } else {
	    h1pt0e2_tr_pt10->Fill(pt0e2/tr_pt0);
	  }//if(tr_pt)
	}

	if(0.8 < emc_e/p0e2 && emc_e/p0e2 < 1.3) {
	  if(pt0e2>0.8) {
	    if(pt0e2<1.2) {
	      h1pt0e2_tr_pt3a->Fill(pt0e2/tr_pt0);
	    } else if(pt0e2<1.5) {
	      h1pt0e2_tr_pt4a->Fill(pt0e2/tr_pt0);
	    } else if(pt0e2<2.0) {
	      h1pt0e2_tr_pt5a->Fill(pt0e2/tr_pt0);
	    } else if(pt0e2<3.0) {
	      h1pt0e2_tr_pt6a->Fill(pt0e2/tr_pt0);
	    }
	  }//if(pt0e2>0.8)
	}//if(emc_e/p0e2)
      }//if(iTop)
    }//if(n_tr_trk==1 && ntrk==1)
  }//if(truthTree)
}

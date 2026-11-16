#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

/*  Example 4
 *  calculate invariant mass of e+ e- pair.
 *  This example's input is jpsi simulation nDST
 *
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

  h1ntrk = new TH1F("h1ntrk","# of tracks",51,-0.5,50.5);
  h1ptee = new TH1F("h1ptee","ptee (no eID)",250,0.,25.0);

  h1mee1 = new TH1F("h1mee1","Mee (no eID) ptee<0.5 GeV/c",100,0.,5.0);
  h1mee2 = new TH1F("h1mee2","Mee (no eID) 0.5<ptee<1.0 GeV/c",100,0.,5.0);
  h1mee3 = new TH1F("h1mee3","Mee (no eID) 1.0<ptee<1.5 GeV/c",100,0.,5.0);
  h1mee4 = new TH1F("h1mee4","Mee (no eID) 1.5<ptee<2.0 GeV/c",100,0.,5.0);
  h1mee5 = new TH1F("h1mee5","Mee (no eID) 2.0<ptee<3.0 GeV/c",100,0.,5.0);
  h1mee6 = new TH1F("h1mee6","Mee (no eID) 3.0<ptee<5.0 GeV/c",100,0.,5.0);
  h1mee7 = new TH1F("h1mee7","Mee (no eID) 5.0<ptee<7.0 GeV/c",100,0.,5.0);
  h1mee8 = new TH1F("h1mee8","Mee (no eID) 7.0<ptee<10.0 GeV/c",100,0.,5.0);
  h1mee9 = new TH1F("h1mee9","Mee (no eID) 10.0<ptee<15.0 GeV/c",100,0.,5.0);
  h1mee10 = new TH1F("h1mee10","Mee (no eID) 15.0<ptee (GeV/c)",100,0.,5.0);  

  h1mee0  = new TH1F("h1mee0", "Mee (no eID) ptee<4 GeV/c",100,0.,5.0);
  h1mee0a = new TH1F("h1mee0a","Mee (0.8<E/p) ptee<4 GeV/c",100,0.,5.0);
  h1mee0b = new TH1F("h1mee0b","Mee (0.8<E/p&&prob>0.1) ptee<4 GeV/c",100,0.,5.0);
  h1mee0c = new TH1F("h1mee0c","Mee (0.9<E/p&&prob>0.1) ptee<4 GeV/c",100,0.,5.0);
  h1mee0d = new TH1F("h1mee0d","Mee (0.95<E/p&&prob>0.1) ptee<4 GeV/c",100,0.,5.0);
}


void INTT_Calo_trkUser_End(void) {
  /* this is empty now
     This function is called at the end of analysis.
     Please fill your code that must be done at the end of your analysis.
     For example, ->Write() the TH1,TH2, etc that you filled
   */
  h1zgvtx->Write();
  h1ntrk->Write();
  h1ptee->Write();

  h1mee1->Write();
  h1mee2->Write();
  h1mee3->Write();
  h1mee4->Write();
  h1mee5->Write();
  h1mee6->Write();
  h1mee7->Write();
  h1mee8->Write();
  h1mee9->Write();
  h1mee10->Write();

  h1mee0->Write();
  h1mee0a->Write();
  h1mee0b->Write();
  h1mee0c->Write();
  h1mee0d->Write();

}

void INTT_Calo_trkUser(void) {
  /* this is empty now.
     This function is called for every event.
     Please fill your analysis code here.
   */
  const float me = 0.000511; //electron mass (GeV)

  h1zgvtx->Fill(zgvtx);   // global zvtx
  
  // Get INTT based tracks from vCaloInttMvtx
  //
  int ntrk = vCaloInttMvtx.size();   // ntrk: # of INTT based tracks
  h1ntrk->Fill(ntrk);

  // vector of positive tracks
  // since they are declared here, they should be empty 
  vector<float> vpxp;
  vector<float> vpyp;
  vector<float> vpzp;
  vector<float> vEp;  //Emc energy  
  vector<float> vprob_p;
  vector<int>   vMvtx0_p;
  
  // vector of negative tracks
  // since they are declared here, they should be empty 
  vector<float> vpxm;
  vector<float> vpym;
  vector<float> vpzm;
  vector<float> vEm;  //Emc energy
  vector<float> vprob_m;
  vector<int>   vMvtx0_m;

  //
  // copy track data of positive tracks to (vpxp,vpyp,vpzp,etc)
  //                    negative tracks to (vpxm,vpym,vpzm, etc)
  int itrk=0;
  
  while(itrk < ntrk) {
    // get one reconstructed track.
    CaloInttMvtx CIMtrack = vCaloInttMvtx.at(itrk);

    //The following are track parameter when the track is associated with an Emcal cluster
    //The position of the Emcal cluster then used to improve the momentum
    float x0e    = CIMtrack.x0e;     //(x0,y0,z0) of the track starting point
    float y0e    = CIMtrack.y0e;
    float z0e    = CIMtrack.z0e;
    float pt0e   = CIMtrack.pt0e2;   //pt of the track at (x0,y0,z0) with "dphi-method" (better)
    float pz0e   = CIMtrack.pz0e2;   //pz of the track at (x0,y0,z0)
    float phi0e  = CIMtrack.phi0e;   //phi of the track at (x0,y0,z0)
    float px0e   = pt0e*cos(phi0e);  //px of the track at (x0,y0,z0)
    float py0e   = pt0e*sin(phi0e);  //py of the track at (x0,y0,z0)
    int   sign   = CIMtrack.sign;    //charge sign
    float emc_e  = CIMtrack.emc_e;   //energy of the emcal cluster associated with this track
                                     //emc_e is -99 if an associated Emc cluster is not found.
    float emc_prob = CIMtrack.emc_prob;  //prob of the associated Emc cluster
    int iMvtx0 = CIMtrack.iMvtx0;    //index of associated Mvtx0 cluster. negative if not associated
    int iMvtx1 = CIMtrack.iMvtx1;    //index of associated Mvtx1 cluster. negative if not associated
    int iMvtx2 = CIMtrack.iMvtx2;    //index of associated Mvtx2 cluster. negative if not associated

    if(emc_e > 0) { //this track is associated with an Emcal cluster
      if(sign > 0) {
	vpxp.push_back(px0e);
	vpyp.push_back(py0e);
	vpzp.push_back(pz0e);
	vEp.push_back(emc_e);
	vprob_p.push_back(emc_prob);
	vMvtx0_p.push_back(iMvtx0);
      } else {
	vpxm.push_back(px0e);
	vpym.push_back(py0e);
	vpzm.push_back(pz0e);
	vEm.push_back(emc_e);
	vprob_m.push_back(emc_prob);
	vMvtx0_m.push_back(iMvtx0);
      }
    }//if(emc_e)    
    itrk++;
  }//while(itrk)
  // now parameters of postive tracks are in vpxp,vpyp,vpzp,vEp, etc
  // and negative tracks are in vpxm,vpym,vpzm,vEpm, etc

  int Np=vpxp.size();
  int Nm=vpxm.size();
  if(Np>0 && Nm>0) {
    for(int ip=0;ip<Np;ip++) {
      float pxp=vpxp.at(ip);
      float pyp=vpyp.at(ip);
      float pzp=vpzp.at(ip);
      float Eemc_p =vEp.at(ip);
      float prob_p=vprob_p.at(ip);
      int Mvtx0_p = vMvtx0_p.at(ip);
      float Ep = sqrt(pxp*pxp + pyp*pyp + pzp*pzp + me*me);
      float p_p = sqrt(pxp*pxp + pyp*pyp + pzp*pzp);
      float E_p_p = Eemc_p/p_p;   //  E/p for positive
      
      for(int im=0;im<Nm;im++) {
	float pxm=vpxm.at(im);
	float pym=vpym.at(im);
	float pzm=vpzm.at(im);
	float Eemc_m =vEm.at(im);
	float prob_m=vprob_m.at(im);
	int Mvtx0_m = vMvtx0_m.at(im);
	float Em = sqrt(pxm*pxm + pym*pym + pzm*pzm + me*me);
	float p_m = sqrt(pxp*pxp + pyp*pyp + pzp*pzp);
	float E_p_m = Eemc_m/p_m;  //   E/p for negative


	float Eee  = Ep+Em;
	float pxee = pxp+pxm;
	float pyee = pyp+pym;
	float pzee = pzp+pzm;
	float ptee = sqrt(pxee*pxee + pyee*pyee);
	float Mee = sqrt(Eee*Eee - pxee*pxee - pyee*pyee - pzee*pzee);

	if(Mvtx0_p>=0 && Mvtx0_m>=0) {//Both track has Mvtx0 hit
	  h1ptee->Fill(ptee);
	  if(ptee<0.5) {
	    h1mee1->Fill(Mee);
	  } else if(ptee<1.0) {
	    h1mee2->Fill(Mee);
	  } else if(ptee<1.5) {
	    h1mee3->Fill(Mee);
	  } else if(ptee<2.0) {
	    h1mee4->Fill(Mee);
	  } else if(ptee<3.0) {
	    h1mee5->Fill(Mee);
	  } else if(ptee<5.0) {
	    h1mee6->Fill(Mee);
	  } else if(ptee<7.0) {
	    h1mee7->Fill(Mee);
	  } else if(ptee<10.0) {
	    h1mee8->Fill(Mee);
	  } else if(ptee<15.0) {
	    h1mee9->Fill(Mee);
	  } else {
	    h1mee10->Fill(Mee);
	  }

	  if(ptee<4.0) {
	    h1mee0->Fill(Mee);
	    if(0.8<E_p_p&& 0.8<E_p_m) {
	      h1mee0a->Fill(Mee);
	      if(prob_p>0.1 && prob_m>0.1) {
		h1mee0b->Fill(Mee);
		if(0.9<E_p_p&& 0.9<E_p_m) {
		  h1mee0c->Fill(Mee);
		  if(0.95<E_p_p&& 0.95<E_p_m) {
		    h1mee0d->Fill(Mee);
		  }//if(E_p>0.95)
		}//if(E_p>0.9)
	      }//if(prob)
	    }//if(E_p)
	  }//if(ptee<4)
	}//if(Mvtx0_m,Mvtx0_p)
      }//for(im)
    }//for(ip)
  }//if(Np,Nm)  
}

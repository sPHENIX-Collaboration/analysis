#include <cmath>
#include "INTT_Calo_Ana.h"
#include "INTT_Calo_trkReco.h"
#include "AnalyzeEvent.h"

void LoadTrackerData(void) {
  // Set Beam center
  xBC=-0.018;
  yBC=0.126;
  
  // clear vectors of the hits
  vEmc_r.clear();
  vEmc_phi.clear();
  vEmc_eta.clear();
  vEmc_z.clear();
  vEmc_e.clear();

  vTop_r.clear();
  vTop_phi.clear();
  vTop_eta.clear();
  vTop_z.clear();
  vTop_e.clear();
  vTop_emc_e.clear();
  vTop_ihc_e.clear();
  vTop_ohc_e.clear();

  vINTT0r.clear();
  vINTT0phi.clear();
  vINTT0z.clear();
  
  vINTT1r.clear();
  vINTT1phi.clear();
  vINTT1z.clear();

  vMvtx0r.clear();
  vMvtx0phi.clear();
  vMvtx0z.clear();

  vMvtx1r.clear();
  vMvtx1phi.clear();
  vMvtx1z.clear();

  vMvtx2r.clear();
  vMvtx2phi.clear();
  vMvtx2z.clear();

  int nEmc = emc_r->size();
  int nTop = top_r->size();
  int nSiAll = a_layer->size();
  // store hits in the vectors with correction of (xBC, yBC)

  for(int iemc=0;iemc<nEmc;iemc++) {
    if(!Is_Hot_Emc(iemc)) {
      vEmc_r.push_back(emc_r->at(iemc));
      vEmc_phi.push_back(emc_phi->at(iemc));
      vEmc_eta.push_back(emc_eta->at(iemc));
      vEmc_z.push_back(emc_z->at(iemc));
      vEmc_e.push_back(energy->at(iemc));
    }//if(!Is_Hot_Emc)
  }//for(iemc)

  for(int itop=0; itop < nTop ;itop++) {
    vTop_r.push_back(top_r->at(itop));
    vTop_phi.push_back(top_phi->at(itop));
    vTop_eta.push_back(top_eta->at(itop));
    vTop_z.push_back(top_z->at(itop));
    vTop_e.push_back(top_e->at(itop));
    vTop_emc_e.push_back(emc_e->at(itop));
    vTop_ihc_e.push_back(ihc_e->at(itop));
    vTop_ohc_e.push_back(ohc_e->at(itop));
  }//for(itop)

  for(int is = 0; is< nSiAll ;is++) {
    int layer = a_layer->at(is);
    float xhit = a_si_x->at(is)-xBC;
    float yhit = a_si_y->at(is)-yBC;
    float zhit = a_si_z->at(is);
    float phi_hit = atan2(yhit,xhit);
    float rhit = sqrt(xhit*xhit+yhit*yhit);

    if(layer == 0) {
      vMvtx0r.push_back(rhit);
      vMvtx0phi.push_back(phi_hit);
      vMvtx0z.push_back(zhit);
    } else if(layer == 1) {
      vMvtx1r.push_back(rhit);
      vMvtx1phi.push_back(phi_hit);
      vMvtx1z.push_back(zhit);
    } else if(layer == 2) {
      vMvtx2r.push_back(rhit);
      vMvtx2phi.push_back(phi_hit);
      vMvtx2z.push_back(zhit);
    } else if(layer == 3|| layer==4) {
      vINTT0r.push_back(rhit);
      vINTT0phi.push_back(phi_hit);
      vINTT0z.push_back(zhit);
    } else {
      vINTT1r.push_back(rhit);
      vINTT1phi.push_back(phi_hit);
      vINTT1z.push_back(zhit);
    }
  }//for(is)
}

bool Is_Hot_Emc(int iemc) {
  float phiEMC = emc_phi->at(iemc);
  float zEMC = emc_z->at(iemc);
  if( (2.4 < phiEMC && phiEMC < 2.7) && (zEMC < 0)) return true;
  else return false;
}

float rz2eta(float r,float z) {
  if(z>=0) {
    return log((sqrt(r*r+z*z)+z)/r);
  } else {
    return -log((sqrt(r*r+z*z)-z)/r);
  }
}

void Compare_Topo_and_Emc(void) {
  int nTopo = vTop_r.size();
  int nEmc = vEmc_r.size();

  for(int it=0;it<nTopo;it++) {
    float Top_e = vTop_e.at(it);
    float Top_r = vTop_r.at(it);
    float Top_phi = vTop_phi.at(it);
    float Top_eta = vTop_eta.at(it);

    if(Top_e > 0.6) {
      for(int ie=0;ie<nEmc;ie++) {
	float Emc_e = vEmc_e.at(ie);
	float Emc_r = vEmc_r.at(ie);
	float Emc_phi = vEmc_phi.at(ie);
	float Emc_eta = vEmc_eta.at(ie);

	hTopEmc_dphi->Fill(Top_phi - Emc_phi);
	hTopEmc_deta->Fill(Top_eta - Emc_eta);
	if(Top_r < 93.55) {
	  hTopEmc_dphi_1->Fill(Top_phi - Emc_phi);
	  hTopEmc_deta_1->Fill(Top_eta - Emc_eta);
	} else if(Top_r < 218.) {
	  hTopEmc_dphi_2->Fill(Top_phi - Emc_phi);
	  hTopEmc_deta_2->Fill(Top_eta - Emc_eta);
	} else {
	  hTopEmc_dphi_3->Fill(Top_phi - Emc_phi);
	  hTopEmc_deta_3->Fill(Top_eta - Emc_eta);
	}
      }//for(ie)
    } //if( Top_e)
  }//for(it)
}

void FindInttPair(void) {
  const float dPhiMax = 0.03;  // dPhi cut to form Pair
  int nINTT1=vINTT1phi.size();
  int nINTT0=vINTT0phi.size();
  /*
  cout << "FindInttPair():"<<endl;
  cout << "nINTT1 = "<<nINTT1<<" nINTT0 = "<<nINTT0<<endl;
  */
  vInttPair.clear();
  for(int i1=0;i1<nINTT1;i1++){
    for(int i0=0;i0<nINTT0;i0++){
      float dphi = vINTT1phi.at(i1)-vINTT0phi.at(i0);
      if(abs(dphi)<0.1) hINTTdphi->Fill(dphi);
      
      if(abs(dphi)<dPhiMax) {
	// calculate the radius of the circle passing through
	// INTT1, INTT0, and the origin (0.,0.).
	// Note that the orign shifted by (xBC, yBC) when r0,phi0, r1,phi1
	// are calculated. Therefore , the circle passes through
	// INTT1, INTT0, and (xBC, yBC)
	float r0   = vINTT0r.at(i0);
	float phi0 = vINTT0phi.at(i0);
	float r1   = vINTT1r.at(i1);
	float phi1 = vINTT1phi.at(i1);
	float x0 = r0*cos(phi0);
	float y0 = r0*sin(phi0);
	float x1 = r1*cos(phi1);
	float y1 = r1*sin(phi1);
	float r01 =  sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
	float t = 0.5*x1/((y1-y0)/r01-y0/r0);
	float xc = 0.5*x0 - t*y0/r0;
	float yc = 0.5*y0 + t*x0/r0;
	float r  = sqrt((x0-xc)*(x0-xc)+(y0-yc)*(y0-yc));
	//	float rx = sqrt((x1-xc)*(x1-xc)+(y1-yc)*(y1-yc));
	/*
	if(abs(r-rx)>0.01) {
	  cout << "r = "<<r<<" rx= "<<rx<<endl;
	}
	*/
	if(r > 50) {
	  InttPair new_pair;
	  new_pair.iINTT0 = i0;
	  new_pair.iINTT1 = i1;
	  new_pair.r = r;
	  new_pair.xc = xc;
	  new_pair.yc = yc;
	  vInttPair.push_back(new_pair);
	}//if
      }//if(dphi)
    }//for(i0)
  }//for(i1)
}

void PrintInttPair(void) {
  int nPair = vInttPair.size();
  cout << "nPair = "<<nPair<<endl;
  for(int i=0;i<nPair; i++) {
    InttPair pair = vInttPair.at(i);
    int i0=pair.iINTT0;
    int i1=pair.iINTT1;
    float r = pair.r;
    float phi0 = vINTT0phi.at(i0);
    float phi1 = vINTT1phi.at(i1);
    cout << i <<":";
    cout << " phi1 = "<<phi1;
    cout << " phi0 = "<<phi0;
    cout << " r = "<< r <<endl;
  }
}

void FindEmcIntt(void) {
  // find Emc hit that matches the INTT pair track
  int nEmc = vEmc_r.size();
  int nPair= vInttPair.size();

  for(int ip=0;ip<nPair;ip++) {
    InttPair pair=vInttPair.at(ip);
    float rIntt = pair.r;
    if(rIntt > 200.0) {  //pT>0.8 GeV/c
      float xc= pair.xc;
      float yc= pair.yc;
      
      int i0=pair.iINTT0;
      int i1=pair.iINTT1;
      
      float r0 = vINTT0r.at(i0);
      float Z0 = vINTT0z.at(i0);
      float r1 = vINTT1r.at(i1);
      float Z1 = vINTT1z.at(i1);
      
      for(int ie=0;ie<nEmc;ie++) {
	float Re = vEmc_r.at(ie);
	float phi_e = vEmc_phi.at(ie);
	float xe = Re*cos(phi_e);
	float ye = Re*sin(phi_e);
	float rEmc = sqrt((xe-xc)*(xe-xc)+(ye-yc)*(ye-yc));
	// Cut in RZ space
	//
	float Ze = vEmc_z.at(ie);
	float Z1p = ((Ze-0.5*(Z0+Z1))/(Re-0.5*(r0+r1)))*(r1-0.5*(r0+r1))+0.5*(Z0+Z1);
	float Z0p = ((Ze-0.5*(Z0+Z1))/(Re-0.5*(r0+r1)))*(r0-0.5*(r0+r1))+0.5*(Z0+Z1);
	
	float dZe1 = Z1p-Z1;
	float dZe0 = Z0p-Z0;
	
	if(abs(rEmc-rIntt)<15) {
	  hdZe1->Fill(dZe1);
	  hdZe0->Fill(dZe0);
	}
	hEmcIntt_dr->Fill(rEmc-rIntt);
	if(abs(dZe1)<1.0 && abs(dZe0)<1.0) {
	  hEmcIntt_dr_Zcut->Fill(rEmc-rIntt);
	}
      }
    }
  }
}

void PrintEmcIntt(void) {
}

void FindInttMvtx(void) {
}

void PrintInttMvtx(void) {
}

void FindCaloInttMvtx(void) {
}

void PrintCaloInttMvtx(void) {
}

void INTT_Calo_trkReco(void) {
  LoadTrackerData();

  Compare_Topo_and_Emc();

  FindInttPair();
  if(imode == 2) PrintInttPair();
  
  FindEmcIntt();
  if(imode == 2) PrintEmcIntt();
    
  FindInttMvtx();
  if(imode == 2) PrintInttMvtx();
  
  FindCaloInttMvtx();
  if(imode == 2) PrintCaloInttMvtx();
}

#include "INTT_Calo_trkReco.h"

// variables for tracking
//
// Variable used in FindInttMvtx()
// They holds Mvtx hit candidates associated with an Intt pair.
static vector<int>   vMvtx0Hit;
static vector<int>   vMvtx1Hit;
static vector<int>   vMvtx2Hit;

void INTT_Calo_trkReco_Init(void) {
  hzgvtx = new TH1F("hzgvtx","global Zvtx",100,-50,50);
  hINTTdphi = new TH1F("hINTTdphi","INTT1phi - INTT0phi",100,-0.1,0.1);
  hINTTdZvtx = new TH1F("hINTTdZvtx","zvtx_intt - zgvtx",100,-50,50);
  hINTTdz0 = new TH1F("hINTTdz0","z0-z0proj",100,-25.,25.);
  hINTTdz1 = new TH1F("hINTTdz1","z1-z1proj",100,-25.,25.);
  hINTTdz0dz1 = new TH2F("hINTTdz0dz1","dz1 vs dz0",100,-25,25,100,-25,25);
  hINTTdz0dz1r = new TH2F("hINTTdz0dz1r","dz1 vs dz0 (rotate)",100,-25,25,100,-25,25);
  hINTTdz_r = new TH1F("hINTTdz_r"," dz (rotated)",100,-5,5);
  hInttPair_r = new TH1F("hInttPair_r","r of INTT pair",100,0.,500);
  hEmcIntt_dr = new TH1F("hEmcIntt_dr","rEmc-rIntt",100,-50,50);
  hEmcIntt_dr_Zcut = new TH1F("hEmcIntt_dr_Zcut","rEmc-rIntt w Zcut",100,-50,50);
  hEmcIntt_dr_Zvtx2 = new TH1F("hEmcIntt_dr_Zvtx2","rEmc-rIntt w Zvtx cut",100,-50,50);
  hEmcIntt_dr_Zvtx4 = new TH1F("hEmcIntt_dr_Zvtx4","rEmc-rIntt w Zvtx cut",100,-50,50);
  hEmcIntt_dr_Zvtx5 = new TH1F("hEmcIntt_dr_Zvtx5","rEmc-rIntt w Zvtx cut",100,-50,50);
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

  hZvtxTrk  = new TH1F("hZvtxTrk","Zvtx projection of INTT_Calo track",100,-50,50);
  hdZvtxTrk = new TH1F("hdZvtxTrk","zgvtx - ZvtxTrk",200,-25,25);
  hZvtxTrk2  = new TH1F("hZvtxTrk2","Zvtx projection of INTT_Calo track",100,-50,50);
  hdZvtxTrk2 = new TH1F("hdZvtxTrk2","zgvtx - ZvtxTrk",200,-25,25);
  hZvtxTrk3  = new TH1F("hZvtxTrk3","Zvtx projection of INTT_Calo track",100,-50,50);
  hdZvtxTrk3 = new TH1F("hdZvtxTrk3","zgvtx - ZvtxTrk",200,-25,25);
  hZvtxTrk4  = new TH1F("hZvtxTrk4","Zvtx projection of INTT_Calo track",100,-50,50);
  hdZvtxTrk4 = new TH1F("hdZvtxTrk4","zgvtx - ZvtxTrk",200,-25,25);

  hTopE     = new TH1F("hTopE"," Energy of TopoCluster",100.,0,10.0);
  hp_Trk    = new TH1F("hp_Trk","p of INTT_Calo track",100,0.,10.);
  hptTrk    = new TH1F("hptTrk","pt of INTT_Calo track",100,0.,10.);
  hTopE_p   = new TH1F("hTopE_p","E(Top)/p(Trk)",120,-0.4,2.0);
  hEmcE_p   = new TH1F("hEmcE_p","E(Emc)/p(Trk)",120,-0.4,2.0);

  hTopE_p_Zvtx   = new TH1F("hTopE_p_Zvtx","E(Top)/p(Trk) w Zvtx cut",120,-0.4,2.0);
  hEmcE_p_Zvtx = new TH1F("hEmcE_p_Zvtx","E(Emc)/p(Trk) w Zvtx cut",120,-0.4,2.0);

  hrIntt_rsEmc  = new TH1F("hrIntt_rsEmc","rIntt/rsEmc",100,0.,2.0);
  hrIntt_rsEmc2 = new TH1F("hrIntt_rsEmc2","rIntt/rsEmc",100,0.,2.0);
  hrIntt_rsEmc3 = new TH1F("hrIntt_rsEmc3","rIntt/rsEmc",100,0.,2.0);

  hMvtx0_dr = new TH1F("hMvtx0_dr","rMvtx0 - rIntt",100,-0.25,0.25);
  hMvtx1_dr = new TH1F("hMvtx1_dr","rMvtx1 - rIntt",100,-0.25,0.25);
  hMvtx2_dr = new TH1F("hMvtx2_dr","rMvtx2 - rIntt",100,-0.25,0.25);
  hMvtx0_dz0 = new TH1F("hMvtx0_dz0","z0-zproj0",100,-10.,10.);
  hMvtx0_dz1 = new TH1F("hMvtx0_dz1","z1-zproj1",100,-10.,10.);
  hMvtx1_dz0 = new TH1F("hMvtx1_dz0","z0-zproj0",100,-10.,10.);
  hMvtx1_dz1 = new TH1F("hMvtx1_dz1","z1-zproj1",100,-10.,10.);  
  hMvtx2_dz0 = new TH1F("hMvtx2_dz0","z0-zproj0",100,-10.,10.);
  hMvtx2_dz1 = new TH1F("hMvtx2_dz1","z1-zproj1",100,-10.,10.);

  hMvtx0_dr2 = new TH1F("hMvtx0_dr2","rMvtx0 - rIntt (dz cut)",100,-0.25,0.25);
  hMvtx1_dr2 = new TH1F("hMvtx1_dr2","rMvtx1 - rIntt (dz cut)",100,-0.25,0.25);
  hMvtx2_dr2 = new TH1F("hMvtx2_dr2","rMvtx2 - rIntt (dz cut)",100,-0.25,0.25);

  hdphi_ij = new TH1F("hdphi_ij","dphi_i-dphi_j",160,-0.04,0.04);
  hdzvtx_ij = new TH1F("hdzvtx_ij","zvtx_i-zvtx_j",100,-10,10);
  hdphi_ij2 = new TH1F("hdphi_ij2","dphi_i-dphi_j(w dzvtx cut)",160,-0.04,0.04);
}

void INTT_Calo_trkReco_End(void) {
  hzgvtx->Write();
  hINTTdphi->Write();
  hINTTdZvtx->Write();
  hINTTdz0->Write();
  hINTTdz1->Write();
  hINTTdz0dz1->Write();
  hINTTdz0dz1r->Write();
  hINTTdz_r->Write();
  hInttPair_r->Write();
  hEmcIntt_dr->Write();
  hEmcIntt_dr_Zcut->Write();
  hEmcIntt_dr_Zvtx2->Write();
  hEmcIntt_dr_Zvtx4->Write();
  hEmcIntt_dr_Zvtx5->Write();
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

  hZvtxTrk->Write();
  hdZvtxTrk->Write();
  hZvtxTrk2->Write();
  hdZvtxTrk2->Write();
  hZvtxTrk3->Write();
  hdZvtxTrk3->Write();
  hZvtxTrk4->Write();
  hdZvtxTrk4->Write();

  hTopE->Write();
  hp_Trk->Write();
  hptTrk->Write();
  hTopE_p->Write();
  hEmcE_p->Write();
  
  hTopE_p_Zvtx->Write();
  hEmcE_p_Zvtx->Write();

  hrIntt_rsEmc->Write();
  hrIntt_rsEmc2->Write();
  hrIntt_rsEmc3->Write();

  hMvtx0_dr->Write();
  hMvtx1_dr->Write();
  hMvtx2_dr->Write();

  hMvtx0_dz0->Write();
  hMvtx0_dz1->Write();  
  hMvtx1_dz0->Write();
  hMvtx1_dz1->Write();  
  hMvtx2_dz0->Write();
  hMvtx2_dz1->Write();  
  
  hMvtx0_dr2->Write();
  hMvtx1_dr2->Write();
  hMvtx2_dr2->Write();

  hdphi_ij->Write();
  hdzvtx_ij->Write();
  hdphi_ij2->Write();
}

void INTT_Calo_trkReco(void) {
  hzgvtx->Fill(zgvtx);

  LoadTrackerData();

  Connect_Topo_and_Emc();
  
  FindInttPair();
  if(imode == 2) PrintInttPair();
  
  FindCaloIntt();
    
  FindInttMvtx();
  
  FindCaloInttMvtx();

}

//
// support functions
//
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

//
// Major functions in INTT_Calo_trkReco()
//
void LoadTrackerData(void) {
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

  vTop_emc_r.clear();
  vTop_emc_phi.clear();
  vTop_emc_eta.clear();
  vTop_emc_z.clear();
  vTop_emc_emc_e.clear();

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

  if(nEmc>0) {
    for(int iemc=0;iemc<nEmc;iemc++) {
      if(!Is_Hot_Emc(iemc)) {
	//	vEmc_r.push_back(emc_r->at(iemc));
	vEmc_r.push_back(R_Emc);
	vEmc_phi.push_back(emc_phi->at(iemc));
	vEmc_eta.push_back(emc_eta->at(iemc));
	vEmc_z.push_back(emc_z->at(iemc));
	vEmc_e.push_back(energy->at(iemc));
      }//if(!Is_Hot_Emc)
    }//for(iemc)
  }// if(nEmc)

  if(nTop>0) {
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
  }//if(nTop)

  if(nSiAll>0) {
    for(int is = 0; is< nSiAll ;is++) {
      int layer = a_layer->at(is);
      float xhit = a_si_x->at(is)-xBC;
      float yhit = a_si_y->at(is)-yBC;
      float zhit = a_si_z->at(is);
      float thit = a_si_t->at(is);
      float phi_hit = atan2(yhit,xhit);
      float rhit = sqrt(xhit*xhit+yhit*yhit);
      
      if(layer == 0) {
	vMvtx0r.push_back(rhit);
	vMvtx0phi.push_back(phi_hit);
	vMvtx0z.push_back(zhit);
	vMvtx0t.push_back(thit);
      } else if(layer == 1) {
	vMvtx1r.push_back(rhit);
	vMvtx1phi.push_back(phi_hit);
	vMvtx1z.push_back(zhit);
	vMvtx1t.push_back(thit);
      } else if(layer == 2) {
	vMvtx2r.push_back(rhit);
	vMvtx2phi.push_back(phi_hit);
	vMvtx2z.push_back(zhit);
	vMvtx2t.push_back(thit);
      } else if(layer == 3|| layer==4) {
	vINTT0r.push_back(rhit);
	vINTT0phi.push_back(phi_hit);
	vINTT0z.push_back(zhit);
	vINTT0t.push_back(thit);
      } else {
	vINTT1r.push_back(rhit);
	vINTT1phi.push_back(phi_hit);
	vINTT1z.push_back(zhit);
	vINTT1t.push_back(thit);
      }
    }//for(is)
  }//if(nSiAll)
}


void Connect_Topo_and_Emc(void) {
  // Connect TopoCluster and EmcCulser
  // In Topo Cluster, there are 4 energies:
  // e (total) = emcal_e + ihcal_e + ohcal_e
  //
  // Topocluser position is Top_r, Top_phi, Top_z
  // if ihcal_e = ohcal_e =0, then Top_r = R_emc_front
  // if emcal_e = ihcal_e =0, then Top_r = R_ohcal_front
  //
  
  const float R_emc_front = 93.55;  //R of front surface of Emcal
  const float R_ohcal_front = 218;  //R of front surface of oHCal

  const float sigma_dphi1 = 0.0036;
  const float sigma_deta1 = 0.0050;
  const float sigma_dphi2 = 0.0039;
  const float sigma_deta2 = 0.0053;

  int nTop = vTop_r.size();
  int nEmc = vEmc_r.size();
  /*
  if(imode==2) {
    cout << "nTop = "<<nTop<<endl;
    for(int it=0;it<nTop;it++) {
      cout << it <<": e="<<vTop_e.at(it);
      cout <<" emc_e="<<vTop_emc_e.at(it);
      cout <<" r= "<<vTop_r.at(it);
      cout << " phi= "<<vTop_phi.at(it);
      cout << " eta= "<<vTop_eta.at(it)<<endl; 
    }
    cout << "nEmc = "<<nEmc<<endl;
    for(int ie=0;ie<nEmc;ie++) {
      cout << ie <<": E="<<vEmc_e.at(ie);
      cout <<" r= "<<vEmc_r.at(ie);
      cout << " phi= "<<vEmc_phi.at(ie);
      cout << " eta= "<<vEmc_eta.at(ie)<<endl; 
    }    
  }
  */
  if(nTop>0) {
    for(int it=0;it<nTop;it++) {
      float Top_e = vTop_e.at(it);
      float Top_emc_e = vTop_emc_e.at(it);
      float Top_r = vTop_r.at(it);
      float Top_phi = vTop_phi.at(it);
      float Top_eta = vTop_eta.at(it);

      vTop_emc_r.push_back(-99);    //mark as empty
      vTop_emc_phi.push_back(-99);  //mark as empty
      vTop_emc_eta.push_back(-99);  //mark as empty
      vTop_emc_z.push_back(-99);    //mark as empty
      vTop_emc_emc_e.push_back(-99);//mark as empty

      if(Top_e > Top_E_min) {
	int iemc_match= -1;
	float sdr_min = 1000;

	if(nEmc>0) {
	  //	  if(imode==2) cout << "search for a Emc cluster for this Topo"<<endl;
	  for(int ie=0;ie<nEmc;ie++) {
	    float Emc_e = vEmc_e.at(ie);

	    float Emc_r = vEmc_r.at(ie);
	    float Emc_phi = vEmc_phi.at(ie);
	    float Emc_eta = vEmc_eta.at(ie);
	    float TopEmc_dphi = Top_phi - Emc_phi;
	    float TopEmc_deta = Top_eta - Emc_eta;
	    
	    hTopEmc_dphi->Fill(TopEmc_dphi);
	    hTopEmc_deta->Fill(TopEmc_deta);
	    
	    // find emc cluster that is closest to this Topo cluster
	    if(abs(Top_phi-Emc_phi)<0.15 && abs(Top_eta-Emc_eta)<0.15&& Top_emc_e>0.18) {
	      if(Top_r < R_emc_front) {
		//		if(imode == 2) cout << " r<93.5" <<endl;
		hTopEmc_dphi_1->Fill(TopEmc_dphi);
		hTopEmc_deta_1->Fill(TopEmc_deta);
		
		float sdphi = TopEmc_dphi/sigma_dphi1;
		float sdeta = TopEmc_deta/sigma_deta1;
		float sdr = sqrt(sdphi*sdphi+sdeta*sdeta);
		if(sdr < sdr_min) {
		  iemc_match = ie;
		  sdr_min = sdr;
		}
	      } else if(Top_r < R_ohcal_front) {
		//		if(imode == 2) cout << " 93.5 < r < 218" <<endl;
		hTopEmc_dphi_2->Fill(TopEmc_dphi);
		hTopEmc_deta_2->Fill(TopEmc_deta);
		
		float sdphi = TopEmc_dphi/sigma_dphi2;
		float sdeta = TopEmc_deta/sigma_deta2;
		float sdr = sqrt(sdphi*sdphi+sdeta*sdeta);
		if(sdr < sdr_min) {
		  iemc_match = ie;
		  sdr_min = sdr;
		}
	      } else {
		// in this case, there is no energy in emcal
		// So this TopoCLuster has no associated EMC cluser
		hTopEmc_dphi_3->Fill(TopEmc_dphi);
		hTopEmc_deta_3->Fill(TopEmc_deta);
	      }//if(Top_r)	
	    }//for(ie)
	    // store closest Emc cluster info
	    if(iemc_match>=0) {
	      //if(imode==2) cout <<" Emc hit found: it="<<it<<" iemc= "<<iemc_match<<endl;
	      vTop_emc_r.at(it)  = vEmc_r.at(iemc_match);
	      vTop_emc_phi.at(it)= vEmc_phi.at(iemc_match);
	      vTop_emc_eta.at(it)= vEmc_eta.at(iemc_match);
	      vTop_emc_z.at(it)  = vEmc_z.at(iemc_match);
	      vTop_emc_emc_e.at(it) = vEmc_e.at(iemc_match);
	    }
	  }//if(abs(Emc_e - Top_emc_e))
	}//if(nEmc)
      }//if( Top_E_min)
    }//for(it)
  }//if(nTop)
}

void FindInttPair(void) {
  const float dPhiMax = 0.05;  // dPhi cut to form Pair
  const float R_min = 60;     // minimum circle radius of INTTpair tracklet (cm)
                               // R_min=60 cm corresponds to pT>0.25 GeV/c
  const float dZvtx_max = 20.0;

  
  int nINTT1=vINTT1phi.size();
  int nINTT0=vINTT0phi.size();

  vInttPair.clear();
  if(nINTT0>0 && nINTT1>0) {
    for(int i1=0;i1<nINTT1;i1++){
      for(int i0=0;i0<nINTT0;i0++){
	float dphi = vINTT1phi.at(i1)-vINTT0phi.at(i0);
	if(abs(dphi)<0.1) hINTTdphi->Fill(dphi);
      
	if(abs(dphi)<dPhiMax) {
	  // Get the hit position of INTT0 and INTT1
	  float r0   = vINTT0r.at(i0);
	  float phi0 = vINTT0phi.at(i0);
	  float z0   = vINTT0z.at(i0);
	  float r1   = vINTT1r.at(i1);
	  float phi1 = vINTT1phi.at(i1);
	  float z1   = vINTT1z.at(i1);
	  float x0 = r0*cos(phi0);
	  float y0 = r0*sin(phi0);
	  float x1 = r1*cos(phi1);
	  float y1 = r1*sin(phi1);

	  // calculate the radius of the circle passing through
	  // INTT1(x1,y1), INTT0(x0,y0), and the origin (0.,0.).
	  // Note that the orign was shifted by (xBC, yBC) when
	  // r0,phi0, r1, and phi1 are calculated. Therefore,
	  // the circle passes through INTT1, INTT0, and (xBC, yBC)
	  //
	  float r;   // radius of the circle
	  float xc;  // center of the circle (xc,yc)
	  float yc;
	  CalcCircle2(x1,y1,x0,y0,r,xc,yc);
	  //
	  // examine if the pair is caused by a track
	  // coming from gzvtx
	  //
	  float dzdr = ((z0+z1)-2.0*gzvtx)/(r0+r1);
	  float z0proj = zgvtx + r0*dzdr;
	  float z1proj = zgvtx + r1*dzdr;
	  float dzdr2 = (z1-z0)/(r1-r0);
	  float zvtx_intt = z0-r0*dzdr2;

	  const float alpha = 0.75;
	  if(r > R_min) {
	    hINTTdZvtx->Fill(zvtx_intt-zgvtx);
	    if(abs(zvtx_intt-zgvtx)< dZvtx_max) {
	      hINTTdz0->Fill(z0-z0proj);
	      hINTTdz1->Fill(z1-z1proj);
	      hINTTdz0dz1->Fill(z0-z0proj,z1-z1proj);
	      hINTTdz0dz1r->Fill(z0-z0proj-alpha*(z1-z1proj),z1-z1proj);
	      hINTTdz_r->Fill(z0-z0proj-alpha*(z1-z1proj));
	  
	      InttPair new_pair;
	      new_pair.iINTT0 = i0;
	      new_pair.iINTT1 = i1;
	      new_pair.r = r;
	      new_pair.xc = xc;
	      new_pair.yc = yc;
	      new_pair.z  = z0;
	      new_pair.zvtx =  zvtx_intt;
	      vInttPair.push_back(new_pair);
	    }//if(dzvtx)
	  }//if(Rmin)
	}//if(dphi)
      }//for(i0)
    }//for(i1)
    //
    // now INTT-Pair candidates are found. But some of them are caused
    // by the same charged track because both INTT0 and INTT1 has two
    // sub-layers. Remove those "duplicated pairs".
    //
    int npair_found = vInttPair.size();
    if(npair_found > 1) {
      for(int ipair=0; ipair<npair_found; ipair++) {
	InttPair pair_i = vInttPair.at(ipair);
	for(int jpair=ipair+1;jpair<npair_found; jpair++) {
	  InttPair pair_j = vInttPair.at(jpair);
	  RemoveDuplicatedPair(pair_i, pair_j);
	}//for(jpair)
      }//for(ipair)
    }//if(npair_found)
  }//if(nINTT0, nINTT1)
}

void RemoveDuplicatedPair(InttPair &pair_i, InttPair &pair_j) {
  // Determine if pair_i and pair_j are "the same", and if they are,
  // removed one of them.
  int INTT0_i = pair_i.iINTT0;
  int INTT1_i = pair_i.iINTT1;
  int INTT0_j = pair_j.iINTT0;
  int INTT1_j = pair_j.iINTT1;

  float dphi_i = vINTT1phi.at(INTT1_i) - vINTT0phi.at(INTT0_i);
  float dphi_j = vINTT1phi.at(INTT1_j) - vINTT0phi.at(INTT0_j);
  float zvtx_i = pair_i.zvtx;
  float zvtx_j = pair_j.zvtx;

  hdphi_ij->Fill(dphi_i - dphi_j);
  hdzvtx_ij->Fill(zvtx_i-zvtx_j);
  if(abs(zvtx_i-zvtx_j)<0.06) {
    hdphi_ij2->Fill(dphi_i - dphi_j);
    pair_j.r=-99;
  }
}

void PrintInttPair(void) {
  int nPair = vInttPair.size();
  if(nPair>0) {
    for(int i=0;i<nPair; i++) {
      InttPair pair = vInttPair.at(i);
      int i0=pair.iINTT0;
      int i1=pair.iINTT1;
      float r = pair.r;
      float phi0 = vINTT0phi.at(i0);
      float phi1 = vINTT1phi.at(i1);
    }//for(i)
  }//if(nPair)
}

void CalcCircle2(float x1,float y1,float x0, float y0,
		 float &r, float &xc, float &yc) {
  float r0  = sqrt(x0*x0+y0*y0);
  float r01 =  sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
  float t = 0.5*x1/((y1-y0)/r01-y0/r0);
  xc = 0.5*x0 - t*y0/r0;
  yc = 0.5*y0 + t*x0/r0;
  r  = sqrt((x0-xc)*(x0-xc)+(y0-yc)*(y0-yc));
}

void CalcCircle(double xs0, double ys0, double xs1, double ys1,double xs2, double ys2,
		float &rs, float &xsc, float &ysc) {
  //
  // calclate the radius of the p0-p1-p2 circle. It is assumed that r0<r1<r2
  //
  
  double r12 = sqrt((xs2-xs1)*(xs2-xs1)+(ys2-ys1)*(ys2-ys1));
  double r01 = sqrt((xs1-xs0)*(xs1-xs0)+(ys1-ys0)*(ys1-ys0));
  double ts = 0.5*(xs2-xs0)/((ys2-ys1)/r12-(ys1-ys0)/r01);
  
  xsc = 0.5*(xs0+xs1)-ts*(ys1-ys0)/r01;
  ysc = 0.5*(ys0+ys1)+ts*(xs1-xs0)/r01;
  rs  = sqrt((xs0-xsc)*(xs0-xsc)+(ys0-ysc)*(ys0-ysc));
}

float Calc_Remc(float xe, float ye, float x1,float y1, float x0, float y0, float xc, float yc) {
  float xm = 0.5*(x1+x0);
  float ym = 0.5*(y1+y0);
  float ux = xm-xc;
  float uy = ym-yc;
  float ru = sqrt(ux*ux+uy*uy);
  float vx = x1-x0;
  float vy = y1-y0;
  float rv = sqrt(vx*vx+vy*vy);
  // normalize the u and v vector
  ux = ux/ru;
  uy = uy/ru;
  vx = vx/rv;
  vy = vy/rv;

  float h = vx*(xe-xc) + vy*(ye-yc);
  float d = ux*(xm-xe)+uy*(ym-ye);

  return 0.5*(d*d+h*h)/d;
}

void FindCaloIntt(void) {
  // find Topo Clusters that match the INTT pair tracklet
  //
  // analysis constants
  const float rIntt_min = 200.; //cm. 100cm corresponds to pT>0.84 GeV/c
  const float dRoffset = -1.0;
  const float dRcut = 18.0;
  const float dRcut_narrow = 10.0;
  const float dZvtxCut   = 1.2;
  const float dZvtx_narrow = 1.0;

  int nTop = vTop_r.size();
  int nPair= vInttPair.size();

  vCaloInttMvtx.clear();
  
  if(nTop>0 && nPair>0) {
    for(int ip=0;ip<nPair;ip++) {
      // For each of InttPair tracklet, search for TopClusters that is
      // close to the projection point of the tracklet.
      // if multiple Top Clusters are found in the search area, all of them
      // are added to the tracklet.
      InttPair pair=vInttPair.at(ip);
      float rIntt = pair.r;
      float xc = pair.xc;  //(xc,yc) is the center of the circule
      float yc = pair.yc;  // passing through INTT1,INTT0 and (xBC,yBC)
      
      if(rIntt > rIntt_min) {
	int i0=pair.iINTT0;
	int i1=pair.iINTT1;
	
	float r0   = vINTT0r.at(i0);
	float phi0 = vINTT0phi.at(i0);
	float Z0   = vINTT0z.at(i0);
	float r1   = vINTT1r.at(i1);
	float phi1 = vINTT1phi.at(i1);
	float Z1   = vINTT1z.at(i1);
	float x0   = r0*cos(phi0); //hit position (x0,y0) of INTT0
	float y0   = r0*sin(phi0);
	float x1   = r1*cos(phi1); //hit position (x1,y1) of INTT1
	float y1   = r1*sin(phi1);
	float xm   = x1/2.; // mid-point (xm,ym) of INTT1 and (0,0)
	float ym   = y1/2.;
	float r_cm = sqrt((xc-xm)*(xc-xm)+(yc-ym)*(yc-ym));
	float uxc  = (xc-xm)/r_cm; //(uxc,uyc) is the unit direction vector from
	float uyc  = (yc-ym)/r_cm; //(xm,ym) to (xc,yc)
	
	// Search for a TopoCluster that is close to the projection point of
	// the INTT_pair track.
	// vector of index of topoClusters that are in the search window.
	vector<int>   vTop_in_window;
	vector<float> vrsEmc;
	vector<float> vxsc;
	vector<float> vysc;
	vector<float> vZvtxTrk;
	vector<float> vptTrk;
	vector<float> vpzTrk;
	for(int it=0;it<nTop;it++) {
	  if(vTop_e.at(it) > Top_E_min) {
	    // Cut variables in R-phi space
	    float Emc_emc_e = vTop_emc_emc_e.at(it);
	    if(Emc_emc_e>0.1) { // an Emc cluster is associated with this Topo Cluster
	      float total_e = vTop_e.at(it);
	      float emcal_e = vTop_emc_e.at(it);
	      float ihcal_e = vTop_ihc_e.at(it);
	      float ohcal_e = vTop_ohc_e.at(it);
	      float R_t = vTop_r.at(it);
     	      float R_e = vTop_emc_r.at(it);
	      float phi_t = vTop_phi.at(it);
	      float phi_e = vTop_emc_phi.at(it);
	      float Z_t   = vTop_z.at(it);
	      
	      float xe = R_e*cos(phi_e);
	      float ye = R_e*sin(phi_e);
	      float ze = vTop_emc_z.at(it);
	      float le = sqrt(xe*xe+ye*ye+ze*ze);
	      
	      float rEmc = sqrt((xe-xc)*(xe-xc)+(ye-yc)*(ye-yc));

	      // Cut variables in RZ space
	      //
	      float Ze = vTop_emc_z.at(it);
	      float Z1p = ((Ze-0.5*(Z0+Z1))/(R_e-0.5*(r0+r1)))*(r1-0.5*(r0+r1))+0.5*(Z0+Z1);
	      float Z0p = ((Ze-0.5*(Z0+Z1))/(R_e-0.5*(r0+r1)))*(r0-0.5*(r0+r1))+0.5*(Z0+Z1);
	      float ZvtxTrk = ((Ze-0.5*(Z0+Z1))/(R_e-0.5*(r0+r1)))*(-0.5*(r0+r1))+0.5*(Z0+Z1);
	      float dZe1 = Z1p-Z1;
	      float dZe0 = Z0p-Z0;
	      float dZ1max = 1.1;
	      float dZ0max = 1.1;
	      if(abs(Z1)<12.9) dZ1max = 0.9;
	      if(abs(Z0)<12.9) dZ0max = 0.9;
	      
	      hZvtxTrk->Fill(ZvtxTrk);
	      hdZvtxTrk->Fill(zgvtx-ZvtxTrk);
	      
	      // Cuts in R-phi space
	      if(abs(rEmc-rIntt - dRoffset) < dRcut) {
		//
		// a maching Topo cluster is found in Rphi plane.
		//
		hdZe1->Fill(dZe1);
		hdZe0->Fill(dZe0);
		hZvtxTrk2->Fill(ZvtxTrk);
		hdZvtxTrk2->Fill(zgvtx-ZvtxTrk);

		// Cuts in RZ space
		if(abs(dZe1)<dZ1max && abs(dZe0)<dZ0max) {
		  //
		  // Topo-INTT1-INTT0 is consisent with a straight line in RZ space
		  //
		  hZvtxTrk4->Fill(ZvtxTrk);
		  hdZvtxTrk4->Fill(zgvtx-ZvtxTrk);
		  if(abs(zgvtx-ZvtxTrk)<dZvtxCut) { // 
		    //
		    // A TopoCluster in the search window is found.
		    //
		    float rsEmc;
		    float xsc;
		    float ysc;
		    // Calclualte the radius etc of the circle passing through
		    // (0,0)-INTT0-Topo
		    //
		    CalcCircle(x0,y0,x1,y1,xe,ye, rsEmc, xsc, ysc);
		    float ptTrk   = 0.0042*rsEmc;
		    float p_Trk   = ptTrk*le/R_e;		
		    float pzTrk   = ptTrk*(ze-ZvtxTrk)/R_e;

		    // store the index of the topoCluter in the window and
		    // the information of the Calo-INTT tracklet
		    vTop_in_window.push_back(it);  // index of topocluster
		    vrsEmc.push_back(rsEmc);    // radius of the circle orbit
		    vxsc.push_back(xsc);        // (xsc,ysc) is center of the circle
		    vysc.push_back(ysc);
		    vZvtxTrk.push_back(ZvtxTrk);// Projected zvtx position of the track
		    vptTrk.push_back(ptTrk);    // pT of the track
		    vpzTrk.push_back(pzTrk);    // pz of the track
		    //
		    // the following part is to monitor the code. To be removed later
		    //
		    hrIntt_rsEmc->Fill(rIntt/rsEmc);
		    if(abs(zgvtx-ZvtxTrk)<dZvtx_narrow) {
		      hrIntt_rsEmc2->Fill(rIntt/rsEmc);
		      if(abs(rEmc-rIntt - dRoffset) < dRcut_narrow) {
			hrIntt_rsEmc3->Fill(rIntt/rsEmc);
		      }//if(dRcut_narrow)
		    }//if(dZvtx_narrow)
		    hptTrk->Fill(ptTrk);
		    hTopE->Fill(total_e);
		    hp_Trk->Fill(p_Trk);
		    hTopE_p->Fill(total_e/p_Trk);
		    hEmcE_p->Fill(emcal_e/p_Trk);
		    hTopE_p_Zvtx->Fill(total_e/p_Trk);
		    hEmcE_p_Zvtx->Fill(emcal_e/p_Trk);
		  }//if(abs(zgvtx-ZvtxTrk)
		}//if(dZe1 && dZe0)
		//
		// the following part is to check the cut efficiency
		//
		if(abs(dZe1)<dZ1max && abs(dZe0)< dZ0max) {
		  hEmcIntt_dr_Zvtx2->Fill(rEmc-rIntt);
		}
	      }//if(abs()
	      //
	      // the following part is to check the cut efficiency
	      //
	      hEmcIntt_dr->Fill(rEmc-rIntt);
	      if(abs(dZe1)<dZ1max && abs(dZe0)<dZ0max) {
		hEmcIntt_dr_Zcut->Fill(rEmc-rIntt);
		hZvtxTrk3->Fill(ZvtxTrk);
		hdZvtxTrk3->Fill(zgvtx-ZvtxTrk);
		if(abs(zgvtx-ZvtxTrk)<dZvtxCut) {
		  hEmcIntt_dr_Zvtx4->Fill(rEmc-rIntt);
		}
		if(abs(zgvtx-ZvtxTrk)<dZvtx_narrow) {
		  hEmcIntt_dr_Zvtx5->Fill(rEmc-rIntt);
		}
	      }
	    }// if(Emc_emc_e)
	  }// if(Top_E_min)
	}// for(it)	
	int ntop_found = vTop_in_window.size();
	
	if(ntop_found > 0) {
	  // this INTT_pair tracklet is associated with at least one
	  // Emc hit. So Calo-INTT track is formed.
	  // store the information of the track
	  //
	  // if more than 1 cluster is associated with the tracklet
	  // the cluster that is closest to the original INTT-Pair track
	  // is chosen.
	  int i_dr_min = 0;
	  float dr_min = abs(vrsEmc.at(0)-rIntt);  //difference of r_emc_intt and r_intt
	  
	  for(int i = 0; i < ntop_found; i++) {
	    if(abs(vrsEmc.at(i)-rIntt)< dr_min) {
	      i_dr_min = i;
	      dr_min = abs(vrsEmc.at(i)-rIntt);
	    }//if
	  }//for(i)
	  // cluster with largest emcal_e is selected. Store the data
	  //
	  int sign=1;
	  if(phi1>phi0) sign = -1;

	  // create a CaloInttMvtx track and fill its "Calo-INTT" part
	  int itop = vTop_in_window.at(i_dr_min);
	  float r_emc = vTop_emc_r.at(itop);
	  float phi_emc = vTop_emc_phi.at(itop);
	  float xemc = r_emc*cos(phi_emc);
	  float yemc = r_emc*sin(phi_emc);
	  
	  a_CaloInttMvtx.iTop = itop;
	  a_CaloInttMvtx.iINTT0 = i0;
	  a_CaloInttMvtx.iINTT1 = i1;
	  a_CaloInttMvtx.iMvtx0 = -99;
	  a_CaloInttMvtx.iMvtx1 = -99;
	  a_CaloInttMvtx.iMvtx2 = -99;
	  a_CaloInttMvtx.sign   = sign;
	  a_CaloInttMvtx.r_intt  = pair.r;  //=rIntt
	  a_CaloInttMvtx.xc_intt = pair.xc; //=xc
	  a_CaloInttMvtx.yc_intt = pair.yc; //=yc
	  a_CaloInttMvtx.z_intt  = pair.z;  //=Z0
	  a_CaloInttMvtx.zvtx_intt   = pair.zvtx;
	  a_CaloInttMvtx.r_emc_intt  = vrsEmc.at(i_dr_min);
	  a_CaloInttMvtx.xc_emc_intt = vxsc.at(i_dr_min);
	  a_CaloInttMvtx.yc_emc_intt = vysc.at(i_dr_min);
	  a_CaloInttMvtx.zvtx_emc_intt = vZvtxTrk.at(i_dr_min);
	  a_CaloInttMvtx.r_intt_mvtx    = -99;
	  a_CaloInttMvtx.xc_intt_mvtx   = -99;
	  a_CaloInttMvtx.yc_intt_mvtx   = -99;
	  a_CaloInttMvtx.zvtx_intt_mvtx = -99;
	  a_CaloInttMvtx.xemc = xemc;
	  a_CaloInttMvtx.yemc = yemc;
	  a_CaloInttMvtx.zemc = vTop_emc_z.at(itop);
	  a_CaloInttMvtx.total_e = vTop_e.at(itop);
	  a_CaloInttMvtx.emcal_e = vTop_emc_e.at(itop);
	  a_CaloInttMvtx.ihcal_e = vTop_ihc_e.at(itop);
	  a_CaloInttMvtx.ohcal_e = vTop_ohc_e.at(itop);
	  a_CaloInttMvtx.emc_e   = vTop_emc_emc_e.at(itop);
	  a_CaloInttMvtx.BDT_e   = -99;  // not calucalted yet
	  a_CaloInttMvtx.x0e   = 0.0;
	  a_CaloInttMvtx.y0e   = 0.0;
	  a_CaloInttMvtx.z0e   = vZvtxTrk.at(i_dr_min);
	  a_CaloInttMvtx.pt0e  = vptTrk.at(i_dr_min);
	  a_CaloInttMvtx.phi0e = phi0 + sign*r0/vrsEmc.at(i_dr_min);
	  a_CaloInttMvtx.pz0e  = vpzTrk.at(i_dr_min);
	  a_CaloInttMvtx.x0m   = -99;
	  a_CaloInttMvtx.y0m   = -99;
	  a_CaloInttMvtx.z0m   = -99;
	  a_CaloInttMvtx.pt0m  = -99;
	  a_CaloInttMvtx.phi0m = -99;
	  a_CaloInttMvtx.pz0m  = -99;

	  vCaloInttMvtx.push_back(a_CaloInttMvtx);
	}//if(ntop_found)
      }// if(rIntt)
    }// for(ip)
  }//if(nTop, nPair)
  int ntrk_found = vCaloInttMvtx.size();
  //  if(ntrk_found>0) cout << i_event << ": ntrk_found = "<<ntrk_found<<endl;
}

void FormInttMvtxTrk(float r0, float phi0, float z0,float r1, float phi1,float z1,
		 int iMvtx0,int iMvtx1,int iMvtx2,
		 float &r_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		 float& zvtx_intt_mvtx,
		 float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m) {
  float xs0;  //INTT0 (x,y)
  float ys0;
  float xs1;  //The outermost Mvtx hit
  float ys1;
  float xs2;  //The innermost Mvtx hit or the origin
  float ys2;
  float xsc;
  float ysc;

  if(imode == 2) {
    cout << "FormInttMvtxTrk (old)"<<endl;
    cout << "iMvtx0 = "<<iMvtx0<<" iMvtx1 = "<<iMvtx1<<" iMvtx2 = "<<iMvtx2<<endl;
  }
  // Create a INTT0-Mvtx track
  //
  // (xs0,ys0) is (x,y) of INTT0
  //
  xs0 = r0*cos(phi0);
  ys0 = r0*sin(phi0);
  
  float rs0 = sqrt(xs0*xs0+ys0*ys0);
  // (xs1,ys1) -- The outer-most Mvts hit.
  // Note that actual hit is indicated by non-negative index.
  // From outside (Mvtx2) to inside (Mvtx0), look for a hit.
  float rs1;
  float phis1;
  float zs1;
  
  if(iMvtx2>=0) { // Mvtx2 has a hit.
    rs1   = vMvtx2r.at(iMvtx2);
    phis1 = vMvtx2phi.at(iMvtx2);
    zs1   = vMvtx2z.at(iMvtx2);
  } else if(iMvtx1>=0) {
    rs1   = vMvtx1r.at(iMvtx1);
    phis1 = vMvtx1phi.at(iMvtx1);
    zs1   = vMvtx1z.at(iMvtx1);	  
  } else {
    rs1   = vMvtx0r.at(iMvtx0);
    phis1 = vMvtx0phi.at(iMvtx0);
    zs1   = vMvtx0z.at(iMvtx0);
  }
  xs1 = rs1*cos(phis1);
  ys1 = rs1*sin(phis1);
  
  // (xs2,ys2) -- The inner-most Mvtx hit or (0,0) if only one Mvtx hit
  float rs2;
  float phis2;
  float zs2;
  float dzdr;
  if(iMvtx0>=0) {//Mvtx0 is the inner-most hit
    if(iMvtx1<0 && iMvtx2 <0) {//Mvtx0 is the only hit
      rs2=0;
    } else {
      rs2=vMvtx0r.at(iMvtx0);
    }
    phis2=vMvtx0phi.at(iMvtx0);
    zs2=vMvtx0z.at(iMvtx0);
  } else if(iMvtx1>=0) {// Mvtx1 is the inner-most hit
    if(iMvtx2<0) {
      rs2=0;
    } else {
      rs2=vMvtx1r.at(iMvtx1);
    }
    phis2=vMvtx1phi.at(iMvtx1);
    zs2=vMvtx1z.at(iMvtx1);
  } else {//this means that Mvtx2 is the only hit
    rs2=0;
    phis2=vMvtx2phi.at(iMvtx2);
    zs2=vMvtx2z.at(iMvtx2);
  }

  if(rs2==0) {//there is only one Mvtx hit
    xs2=0;
    ys2=0;
    float rintt = 0.5*(r0+r1);
    float zintt = 0.5*(z0+z1);
    float rmvtx = rs1;
    float zmvtx = zs1;
    dzdr = (zintt-zmvtx)/(rintt-rmvtx);
    zvtx_intt_mvtx = zmvtx - dzdr*rmvtx;
  } else {
    // there are at least two Mvtx hit.
    // Outermost hit is (rs1,phis1,zs1)
    // Innermost hit is (rs2,phis2,zs2)
    xs2=rs2*cos(phis2);
    ys2=rs2*sin(phis2);
    float r_out = rs1;
    float r_in  = rs2;
    float z_out = zs1;
    float z_in  = zs2;
    dzdr  = (z_out - z_in)/(r_out - r_in);
    zvtx_intt_mvtx = z_in - dzdr*r_in;
  }

  // Draw a circle passing through (xs0,ys0)-(xs1,ys1)-(xs2,ys2)
  // and get its radius (r_intt_mvtx) and the center (xs2,ys2) 
  //	CalcCircle(xs0,ys0,xs1,ys1,xs2,ys2,
  CalcCircle(xs2,ys2,xs1,ys1,xs0,ys0,
	     r_intt_mvtx, xc_intt_mvtx, yc_intt_mvtx);
  //
  x0m= xs2;
  y0m= ys2;
  z0m= zs2;
  pt0m= 0.0042*r_intt_mvtx;
  phi0m=phis2;
  pz0m= dzdr*pt0m;
}

void FormInttMvtxTrk(float r0, float phi0, float z0,float r1, float phi1,float z1,
		     vector<int> &vMvtx0Hit, vector<int> &vMvtx1Hit,vector<int> &vMvtx2Hit,
		     int &iMvtx0, int & iMvtx1, int &iMvtx2,
		     float &r_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		     float& zvtx_intt_mvtx,
		     float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m) {
  int nMvtx0 = vMvtx0Hit.size();
  int nMvtx1 = vMvtx1Hit.size();
  int nMvtx2 = vMvtx2Hit.size();

  float x0 = r0*cos(phi0);
  float y0 = r0*sin(phi0);
  float x1 = r1*cos(phi1);
  float y1 = r1*sin(phi1);

  if(imode == 2) {
    cout << "nMvtx0="<<nMvtx0<<" nMvtx1= "<<nMvtx1<<" nMvtx2="<<nMvtx2<<endl;
  }
  // Case 1: one Mvtx layer has associated hits
  if(nMvtx0>1 && nMvtx1 ==0 && nMvtx2 == 0) {
    FormInttMvtxTrk1(x0,y0,z0,x1,y1,z1,vMvtx0Hit,
		     vMvtx0r,vMvtx0phi,vMvtx0z,
		     iMvtx0,
		     r_intt_mvtx,xc_intt_mvtx,yc_intt_mvtx,
		     zvtx_intt_mvtx,
		     x0m, y0m, z0m, pt0m, phi0m, pz0m);
  }

  if(nMvtx0==0 && nMvtx1 > 0 && nMvtx2 == 0) {
    FormInttMvtxTrk1(x0,y0,z0,x1,y1,z1,vMvtx1Hit,
		     vMvtx1r,vMvtx1phi,vMvtx1z,
		     iMvtx1,
		     r_intt_mvtx,xc_intt_mvtx,yc_intt_mvtx,
		     zvtx_intt_mvtx,
		     x0m, y0m, z0m, pt0m, phi0m, pz0m);
  }
  
  if(nMvtx0 == 0 && nMvtx1 ==0 && nMvtx2 > 0) {
    FormInttMvtxTrk1(x0,y0,z0,x1,y1,z1,vMvtx2Hit,
		     vMvtx2r,vMvtx2phi,vMvtx2z,
		     iMvtx2,
		     r_intt_mvtx,xc_intt_mvtx,yc_intt_mvtx,
		     zvtx_intt_mvtx,
		     x0m, y0m, z0m, pt0m, phi0m, pz0m);
  }

  // Case 2: two Mvtx layers have associated hits
  if(nMvtx0>1 && nMvtx1 >0 && nMvtx2 == 0) {
    FormInttMvtxTrk2(x0,y0,z0,x1,y1,z1,vMvtx0Hit,vMvtx1Hit,
		     vMvtx0r,vMvtx0phi,vMvtx0z,
		     vMvtx1r,vMvtx1phi,vMvtx1z,
		     iMvtx0, iMvtx1,
		     r_intt_mvtx,xc_intt_mvtx,yc_intt_mvtx,
		     zvtx_intt_mvtx,
		     x0m, y0m, z0m, pt0m, phi0m, pz0m);
  }

  if(nMvtx0>1 && nMvtx1 ==0 && nMvtx2 > 0) {
    FormInttMvtxTrk2(x0,y0,z0,x1,y1,z1,vMvtx0Hit,vMvtx2Hit,
		     vMvtx0r,vMvtx0phi,vMvtx0z,
		     vMvtx2r,vMvtx2phi,vMvtx2z,
		     iMvtx0, iMvtx2,
		     r_intt_mvtx,xc_intt_mvtx,yc_intt_mvtx,
		     zvtx_intt_mvtx,
		     x0m, y0m, z0m, pt0m, phi0m, pz0m);
  }

  if(nMvtx0 == 0 && nMvtx1 >0 && nMvtx2 > 0) {
    FormInttMvtxTrk2(x0,y0,z0,x1,y1,z1,vMvtx1Hit,vMvtx2Hit,
		     vMvtx1r,vMvtx1phi,vMvtx1z,
		     vMvtx2r,vMvtx2phi,vMvtx2z,
		     iMvtx1, iMvtx2,
		     r_intt_mvtx,xc_intt_mvtx,yc_intt_mvtx,
		     zvtx_intt_mvtx,
		     x0m, y0m, z0m, pt0m, phi0m, pz0m);
  }

  // Case 3: all three Mvtx layers have associated hits
  if(nMvtx0 > 0 && nMvtx1 >0 && nMvtx2 > 0) {
    FormInttMvtxTrk3(x0,y0,z0,x1,y1,z1,vMvtx0Hit,vMvtx1Hit,vMvtx2Hit,
		     iMvtx0,iMvtx1,iMvtx2,
		     r_intt_mvtx,xc_intt_mvtx,yc_intt_mvtx,
		     zvtx_intt_mvtx,
		     x0m, y0m, z0m, pt0m, phi0m, pz0m);
  }    
}

void FormInttMvtxTrk1(float x0, float y0, float z0,float x1, float y1,float z1,
		      vector<int> &vMvtxHit,
		      vector<float> &vMvtx_r, vector<float> &vMvtx_phi, vector<float> &vMvtx_z,
		      int &iMvtxHit,
		      float &rc_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		      float& zvtx_intt_mvtx,
		      float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m) {
  //There is only one Mvtx layer that has associated hits.
  //Form a track of (INTT1-INTT0-Mvtx-BC) and select the best hit in Mvtx.
  int nMvtx = vMvtxHit.size();
  float dr2_min = 100000;
  int   im_min = -1;
  float rc_min = 100000;
  float xc_min = 100000;
  float yc_min = 100000;

  if(imode == 2) {
    cout << "FormTrk1"<<endl;
    cout << "nMvtx="<<nMvtx<<endl;
  }

  for(int im=0;im<nMvtx;im++) {
    float r_m  = vMvtx_r.at(vMvtxHit.at(im));
    float phi_m= vMvtx_phi.at(vMvtxHit.at(im));
    float xm= r_m*cos(phi_m);
    float ym= r_m*sin(phi_m);
    float zm= vMvtx_z.at(vMvtxHit.at(im));
    float r1;
    float xc1;
    float yc1;
    float r2;
    float xc2;
    float yc2;
  
  // Mvtx-INTT0-INTT1 Circle
    CalcCircle(xm,ym,x0,y0,x1,y1, r1,xc1,yc1);
  
  // BC(0,0)-Mvtx-INTT0 Circle
    CalcCircle( 0, 0,xm,ym,x0,y0, r2,xc2,yc2);

    float xc = 0.5*(xc1+xc2);
    float yc = 0.5*(yc1+yc2);

    float rc1= sqrt(xc*xc+yc*yc);
    float rc2= sqrt((xm-xc)*(xm-xc) + (ym-yc)*(ym-yc));
    float rc3= sqrt((x0-xc)*(x0-xc) + (y0-yc)*(y0-yc));
    float rc4= sqrt((x1-xc)*(x1-xc) + (y1-yc)*(y1-yc));
    float rc = 0.25*(rc1+rc2+rc3+rc4);
    float dr2 = (rc1-rc)*(rc1-rc)+(rc2-rc)*(rc2-rc)+(rc3-rc)*(rc3-rc)+(rc4-rc)*(rc4-rc);

    if(dr2<dr2_min) {
      dr2_min = dr2;
      im_min = im;
      rc_min = rc;
      xc_min = xc;
      yc_min = yc;
    }
  }//for(iMvtx)
  iMvtxHit = vMvtxHit.at(im_min);
  rc_intt_mvtx = rc_min;
  xc_intt_mvtx = xc_min;
  yc_intt_mvtx = yc_min;
  float rm  = vMvtx_r.at(iMvtxHit);
  float phim= vMvtx_phi.at(iMvtxHit);
  float r0 = sqrt(x0*x0+y0*y0);
  float phi0 = atan2(y0,x0);

  x0m= rm*cos(phim);
  y0m= rm*sin(phim);
  z0m= vMvtx_z.at(iMvtxHit);
  float dzdr = (z0-z0m)/(r0-rm);  
  zvtx_intt_mvtx= z0-dzdr*rm;  
  pt0m=0.0042*rc_intt_mvtx;
  phi0m=phim + (phi0-phim)*rm/(r0-rm);
  pz0m=dzdr*pt0m;
}

void FormInttMvtxTrk2(float x0, float y0, float z0,float x1, float y1,float z1,
		      vector<int> &vMvtxHit_I, vector<int> &vMvtxHit_O,
		      vector<float> &vMvtxI_r, vector<float> &vMvtxI_phi, vector<float> &vMvtxI_z,
		      vector<float> &vMvtxO_r, vector<float> &vMvtxO_phi, vector<float> &vMvtxO_z,
		      int &iMvtx_I, int &iMvtx_O,
		      float &rc_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		      float& zvtx_intt_mvtx,
		      float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m) {
  // There are two Mvtx layers that have associated hits.
  // They are Mvtx_O (outer layer) and  Mvtx_I (inner layer)
  // Form a track of (INTT1-INTT0-Mvtx_O-Mvtx_I)

  int nMvtxO = vMvtxHit_O.size();
  int nMvtxI = vMvtxHit_I.size();
  float dr2_min = 100000;
  int iO_min = -1;
  int iI_min = -1;
  float rc_min = 100000;
  float xc_min = 100000;
  float yc_min = 100000;

  if(imode == 2) {
    cout << "Trk2" << endl;
    cout << "nMvtxO="<<nMvtxO<<" nMvtxI="<<nMvtxI<<endl;
  }
  for(int iO=0;iO<nMvtxO;iO++) {
    float rO_m   = vMvtxO_r.at(vMvtxHit_O.at(iO));
    float phiO_m = vMvtxO_phi.at(vMvtxHit_O.at(iO));
    float xOm = rO_m*cos(phiO_m);
    float yOm = rO_m*sin(phiO_m);
    float zOm = vMvtxO_z.at(vMvtxHit_O.at(iO));
    
    for(int iI=0;iI<nMvtxI;iI++) {
      float rI_m   = vMvtxI_r.at(vMvtxHit_I.at(iI));
      float phiI_m = vMvtxI_phi.at(vMvtxHit_I.at(iI));
      float xIm = rI_m*cos(phiI_m);
      float yIm = rI_m*sin(phiI_m);
      float zIm = vMvtxI_z.at(vMvtxHit_I.at(iI));
      
      float r1;
      float xc1;
      float yc1;
      float r2;
      float xc2;
      float yc2;
      
  // MvtxO-INTT0-INTT1 Circle
      CalcCircle(xOm,yOm,x0,y0,x1,y1, r1,xc1,yc1);
  
  // MvtxI-MvtxO-INTT1 Circle
      CalcCircle(xIm,yIm,xOm,yOm,x1,y1, r2,xc2,yc2);

      float xc = 0.5*(xc1+xc2);
      float yc = 0.5*(yc1+yc2);

      float rc1= sqrt((xIm-xc)*(xIm-xc) + (yIm-yc)*(yIm-yc));
      float rc2= sqrt((xOm-xc)*(xOm-xc) + (yOm-yc)*(yOm-yc));
      float rc3= sqrt((x0-xc)*(x0-xc)   + (y0-yc)*(y0-yc));
      float rc4= sqrt((x1-xc)*(x1-xc)   + (y1-yc)*(y1-yc));
      float rc = 0.25*(rc1+rc2+rc3+rc4);
      float dr2 = (rc1-rc)*(rc1-rc)+(rc2-rc)*(rc2-rc)+(rc3-rc)*(rc3-rc)+(rc4-rc)*(rc4-rc);
      
      if(dr2<dr2_min) {
	dr2_min = dr2;
	iO_min = iO;
	iI_min = iI;
	rc_min = rc;
	xc_min = xc;
	yc_min = yc;
      }
    }//for(iMvtxI)
  }//for(iMvtxO)
  iMvtx_O = vMvtxHit_O.at(iO_min);
  iMvtx_I = vMvtxHit_I.at(iI_min);
  rc_intt_mvtx = rc_min;
  xc_intt_mvtx = xc_min;
  yc_intt_mvtx = yc_min;
  
  float rIm  = vMvtxI_r.at(iMvtx_I);
  float phiIm= vMvtxI_phi.at(iMvtx_I);
  float zIm  = vMvtxI_z.at(iMvtx_I);

  float rOm  = vMvtxO_r.at(iMvtx_O);
  float phiOm= vMvtxO_phi.at(iMvtx_O);
  float zOm  = vMvtxO_z.at(iMvtx_O);

  float dzdr = (zOm-zIm)/(rOm-rIm);
  
  x0m= rIm*cos(phiIm);
  y0m= rIm*sin(phiIm);
  z0m= zIm;
  
  zvtx_intt_mvtx= zIm-dzdr*rIm;  

  pt0m=0.0042*rc_intt_mvtx;
  phi0m=phiIm + (phiOm - phiIm)*rIm/(rOm-rIm);
  pz0m=dzdr*pt0m;

}

void FormInttMvtxTrk3(float x0, float y0, float z0,float x1, float y1,float z1,
		      vector<int> &vMvtx0Hit, vector<int> &vMvtx1Hit,vector<int> &vMvtx2Hit,
		      int &iMvtx0, int &iMvtx1, int &iMvtx2,
		      float &rc_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		      float& zvtx_intt_mvtx,
		      float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m) {
  // All three Mvtx layers have associated hits.
  // Form a track of (INTT0-Mvtx2-Mvtx1-Mvtx0)
  
  int nMvtx0 = vMvtx0Hit.size();
  int nMvtx1 = vMvtx1Hit.size();
  int nMvtx2 = vMvtx2Hit.size();
  float dr2_min = 100000;
  int iMvtx0_min = -1;
  int iMvtx1_min = -1;
  int iMvtx2_min = -1;
  float rc_min = 100000;
  float xc_min = 100000;
  float yc_min = 100000;
  
  if(imode == 2) {
    cout << "Trk3" << endl;
    cout << "nMvtx0="<<nMvtx0<<" nMvtx1="<<nMvtx1<<" nMvtx2="<<nMvtx2<<endl;
  }
  for(int im0=0;im0<nMvtx0;im0++) {
    float r0_m   = vMvtx0r.at(vMvtx0Hit.at(im0));
    float phi0_m = vMvtx0phi.at(vMvtx0Hit.at(im0));
    float x0_m = r0_m*cos(phi0_m);
    float y0_m = r0_m*sin(phi0_m);
    float z0_m = vMvtx0z.at(vMvtx0Hit.at(im0));
    
    for(int im1=0;im1<nMvtx1;im1++) {
      float r1_m   = vMvtx1r.at(vMvtx1Hit.at(im1));
      float phi1_m = vMvtx1phi.at(vMvtx1Hit.at(im1));
      float x1_m = r1_m*cos(phi1_m);
      float y1_m = r1_m*sin(phi1_m);
      float z1_m = vMvtx1z.at(vMvtx1Hit.at(im1));
      
      for(int im2=0;im2<nMvtx2;im2++) {
	float r2_m   = vMvtx2r.at(vMvtx2Hit.at(im2));
	float phi2_m = vMvtx2phi.at(vMvtx2Hit.at(im2));
	float x2_m = r2_m*cos(phi2_m);
	float y2_m = r2_m*sin(phi2_m);
	float z2_m = vMvtx2z.at(vMvtx2Hit.at(im2));
      
	float r1;
	float xc1;
	float yc1;
	float r2;
	float xc2;
	float yc2;
	float r3;
	float xc3;
	float yc3;
	
	// Mvtx0-Mvtx2-INTT1 Circle
	CalcCircle(x0_m,y0_m,x2_m,y2_m,x1,y1, r1,xc1,yc1);
  
	// Mvtx0-Mvtx1-INTT1 Circle
	CalcCircle(x0_m,y0_m,x1_m,y1_m,x1,y1, r2,xc2,yc2);

	float xc = (xc1+xc2)/2.0;
	float yc = (yc1+yc2)/2.0;
	
	float rc1= sqrt((x0_m-xc)*(x0_m-xc) + (y0_m-yc)*(y0_m-yc));
	float rc2= sqrt((x1_m-xc)*(x1_m-xc) + (y1_m-yc)*(y1_m-yc));
	float rc3= sqrt((x2_m-xc)*(x2_m-xc) + (y2_m-yc)*(y2_m-yc));
	float rc4= sqrt((x1-xc)*(x1-xc)     + (y1-yc)*(y1-yc));
	float rc = (rc1+rc2+rc3+rc4)/4.0;
	float dr2 = (rc1-rc)*(rc1-rc)+(rc2-rc)*(rc2-rc)+(rc3-rc)*(rc3-rc)+(rc4-rc)*(rc4-rc);
	
	if(dr2<dr2_min) {
	  dr2_min = dr2;
	  iMvtx0_min = vMvtx0Hit.at(im0);
	  iMvtx1_min = vMvtx1Hit.at(im1);
	  iMvtx2_min = vMvtx2Hit.at(im2);
	  rc_min = rc;
	  xc_min = xc;
	  yc_min = yc;
	}
      }//for(im2)
    }//for(im1)
  }//for(im0)
  iMvtx0 = iMvtx0_min;
  iMvtx1 = iMvtx1_min;
  iMvtx2 = iMvtx2_min;
  rc_intt_mvtx = rc_min;
  xc_intt_mvtx = xc_min;
  yc_intt_mvtx = yc_min;
  float rIm  = vMvtx0r.at(iMvtx0);
  float phiIm= vMvtx0phi.at(iMvtx0);
  float zIm  = vMvtx0z.at(iMvtx0);

  float rOm  = vMvtx1r.at(iMvtx1);
  float phiOm= vMvtx1phi.at(iMvtx1);
  float zOm  = vMvtx1z.at(iMvtx1);

  float dzdr = (zOm-zIm)/(rOm-rIm);
  
  x0m= rIm*cos(phiIm);
  y0m= rIm*sin(phiIm);
  z0m= zIm;
  
  zvtx_intt_mvtx= zIm-dzdr*rIm;  

  pt0m=0.0042*rc_intt_mvtx;
  phi0m=phiIm + (phiOm - phiIm)*rIm/(rOm-rIm);
  pz0m=dzdr*pt0m;
}

void FindInttMvtx(void) {
  // find Mvtx clusters that match the INTT-pair tracklet
  //
  int nPair = vInttPair.size();
  if(imode == 2) {
    cout << "nPair = "<<nPair<<endl;
  }
  if(nPair > 0) {
    for(int ip=0;ip<nPair;ip++) {
      InttPair pair = vInttPair.at(ip);
      int i0=pair.iINTT0;
      int i1=pair.iINTT1;
      float rIntt = pair.r;
      float xc = pair.xc;  //(xc,yc) is the center of the circule
      float yc = pair.yc;  // passing through INTT1,INTT0 and (xBC,yBC)
      float r0   = vINTT0r.at(i0);
      float r1   = vINTT1r.at(i1);
      float phi0 = vINTT0phi.at(i0);
      float phi1 = vINTT1phi.at(i1);
      float z0   = vINTT0z.at(i0);
      float z1   = vINTT1z.at(i1);
      float zvtx = pair.zvtx;
      int sign = 1;
      if(phi1>phi0) sign = -1;
      
      int iMvtx0 = FindMvtxHit(0,rIntt,xc,yc,zvtx,r0,r1,z0,z1,vMvtx0Hit);
      int iMvtx1 = FindMvtxHit(1,rIntt,xc,yc,zvtx,r0,r1,z0,z1,vMvtx1Hit);
      int iMvtx2 = FindMvtxHit(2,rIntt,xc,yc,zvtx,r0,r1,z0,z1,vMvtx2Hit);

      if(iMvtx0 >=0 || iMvtx1 >=0 || iMvtx2 >=0) {
	// at least one layer has associated hit.
	// Case 1: if there is a Calo-INTT track in vCaloInttMvtx that is based on
	//         the same InttPair, add the Mvtx hits to the track
	// Case 2: if this InttPair is not vCaloInttMvtx, create a new CaloInttMvtx
	//         track and add it i
	//
	// calculate the InttPair-Mvtx track parameters
	float r_intt_mvtx;
	float xc_intt_mvtx;
	float yc_intt_mvtx;
	float zvtx_intt_mvtx;
	float x0m;
	float y0m;
	float z0m;
	float pt0m;
	float phi0m;
	float pz0m;
	
	/*
	FormInttMvtxTrk(r0,phi0,z0,r1,phi1,z1,iMvtx0,iMvtx1,iMvtx2,
			r_intt_mvtx,xc_intt_mvtx,yc_intt_mvtx,
			zvtx_intt_mvtx,
			x0m, y0m, z0m, pt0m, phi0m, pz0m);
	*/
	FormInttMvtxTrk(r0,phi0,z0,r1,phi1,z1,vMvtx0Hit,vMvtx1Hit,vMvtx2Hit,
			iMvtx0,iMvtx1,iMvtx2,
			r_intt_mvtx,xc_intt_mvtx,yc_intt_mvtx,
			zvtx_intt_mvtx,
			x0m, y0m, z0m, pt0m, phi0m, pz0m);

	// Now an INTT-Mvtx track is formed
	// There are two cases:
	// CASE 1: The same track is already formed as an Emc-Intt track
	// CASE 2: This is new Intt-Mvtx track
	//
	// search vCaloInttMvtx for the same InttPair as this track
	// If found, it is CASE 1. Otherwise CASE2.
	int itrk=0;
	int ntrk = vCaloInttMvtx.size();
	while(itrk<ntrk) {
	  int iINTT0 = (vCaloInttMvtx.at(itrk)).iINTT0;
	  int iINTT1 = (vCaloInttMvtx.at(itrk)).iINTT1;

	  if(iINTT0 == i0 && iINTT1 == i1) break;
	  itrk++;
	}

	if(itrk < ntrk) {
	  // This means it is CASE 1.
	  // Add the Intt-Mvtx part of the existing EmcIntt track
	  if(imode==2) {
	    cout << "CASE 1: add to existing track" <<endl;
	  }
	  (vCaloInttMvtx.at(itrk)).iMvtx0 = iMvtx0;
	  (vCaloInttMvtx.at(itrk)).iMvtx1 = iMvtx1;
	  (vCaloInttMvtx.at(itrk)).iMvtx2 = iMvtx2;
	  (vCaloInttMvtx.at(itrk)).r_intt_mvtx = r_intt_mvtx;
	  (vCaloInttMvtx.at(itrk)).xc_intt_mvtx = xc_intt_mvtx;
	  (vCaloInttMvtx.at(itrk)).yc_intt_mvtx = yc_intt_mvtx;
	  (vCaloInttMvtx.at(itrk)).zvtx_intt_mvtx  = zvtx_intt_mvtx;
	  (vCaloInttMvtx.at(itrk)).x0m  = x0m;
	  (vCaloInttMvtx.at(itrk)).y0m  = y0m;
	  (vCaloInttMvtx.at(itrk)).z0m  = z0m;
	  (vCaloInttMvtx.at(itrk)).pt0m  = pt0m;
	  (vCaloInttMvtx.at(itrk)).phi0m = phi0m;
	  (vCaloInttMvtx.at(itrk)).pz0m  = pz0m;
	} else {
	  //the same InttPair is not found, i.e., CASE 2.
	  // Add a new CIMtrack.
	  if(imode==2) {
	    cout << "CASE 2: create a new track" <<endl;
	  }
	  CaloInttMvtx a_CaloInttMvtx;
	  a_CaloInttMvtx.iTop = -99;
	  a_CaloInttMvtx.iINTT0 = i0;
	  a_CaloInttMvtx.iINTT1 = i1;
	  a_CaloInttMvtx.iMvtx0 = iMvtx0;
	  a_CaloInttMvtx.iMvtx1 = iMvtx1;
	  a_CaloInttMvtx.iMvtx2 = iMvtx2;
	  a_CaloInttMvtx.sign   = sign;
	  a_CaloInttMvtx.r_intt =  pair.r;
	  a_CaloInttMvtx.xc_intt = pair.xc;
	  a_CaloInttMvtx.yc_intt = pair.yc;
	  a_CaloInttMvtx.z_intt    = z0;
	  a_CaloInttMvtx.zvtx_intt = zvtx;
	  a_CaloInttMvtx.r_emc_intt = -99;
	  a_CaloInttMvtx.xc_emc_intt = -99;
	  a_CaloInttMvtx.yc_emc_intt = -99;
	  a_CaloInttMvtx.zvtx_emc_intt = -99;
	  a_CaloInttMvtx.r_intt_mvtx    = r_intt_mvtx;
	  a_CaloInttMvtx.xc_intt_mvtx   = xc_intt_mvtx;
	  a_CaloInttMvtx.yc_intt_mvtx   = yc_intt_mvtx;
	  a_CaloInttMvtx.zvtx_intt_mvtx = zvtx_intt_mvtx;
	  a_CaloInttMvtx.xemc = -99;
	  a_CaloInttMvtx.yemc = -99;
	  a_CaloInttMvtx.zemc = -99;
	  a_CaloInttMvtx.total_e = -99;
	  a_CaloInttMvtx.emcal_e = -99;
	  a_CaloInttMvtx.ihcal_e = -99;
	  a_CaloInttMvtx.ohcal_e = -99;
	  a_CaloInttMvtx.emc_e   = -99;
	  a_CaloInttMvtx.BDT_e   = -99;
	  a_CaloInttMvtx.x0e   = -99;
	  a_CaloInttMvtx.y0e   = -99;
	  a_CaloInttMvtx.z0e   = -99;
	  a_CaloInttMvtx.pt0e  = -99;
	  a_CaloInttMvtx.phi0e = -99;
	  a_CaloInttMvtx.pz0e  = -99;
	  a_CaloInttMvtx.x0m   = x0m;
	  a_CaloInttMvtx.y0m   = y0m;
	  a_CaloInttMvtx.z0m   = z0m;
	  a_CaloInttMvtx.pt0m  = pt0m;
	  a_CaloInttMvtx.phi0m = phi0m;
	  a_CaloInttMvtx.pz0m  = pz0m;

	  vCaloInttMvtx.push_back(a_CaloInttMvtx);
	}//if(itrk)
      }//if(iMvtx0,iMvtx1,iMvtx2)
    }//for(ip)
  }//if(nPair)
}

int FindMvtxHit(int layer, float rIntt, float xc, float yc,
		float zvtx, float r0, float r1, float z0, float z1,
		vector<int>   &vMvtxHit) {
  // constants for the analysis
  const float InttMvtx_dr_cut = 0.03;
  const float InttMvtx_dz_cut = 1.0;

  static vector<float> vMvtx_dr;
  static vector<float> vMvtx_dz0;
  static vector<float> vMvtx_dz1;

  vector<float> *pvMvtx_r;
  vector<float> *pvMvtx_phi;
  vector<float> *pvMvtx_z;
  TH1F *hMvtx_dz0;
  TH1F *hMvtx_dz1;
  TH1F *hMvtx_dr;
  TH1F *hMvtx_dr2;

  vMvtxHit.clear();
  vMvtx_dr.clear();
  vMvtx_dz0.clear();
  vMvtx_dz1.clear();

  if(layer == 2) {
    pvMvtx_r   = &vMvtx2r;
    pvMvtx_phi = &vMvtx2phi;
    pvMvtx_z   = &vMvtx2z;
    
    hMvtx_dz0 = hMvtx2_dz0;
    hMvtx_dz1 = hMvtx2_dz1;
    hMvtx_dr  = hMvtx2_dr;
    hMvtx_dr2 = hMvtx2_dr2;
  } else if(layer == 1) {
    pvMvtx_r   = &vMvtx1r;
    pvMvtx_phi = &vMvtx1phi;
    pvMvtx_z   = &vMvtx1z;
    
    hMvtx_dz0 = hMvtx1_dz0;
    hMvtx_dz1 = hMvtx1_dz1;
    hMvtx_dr  = hMvtx1_dr;
    hMvtx_dr2 = hMvtx1_dr2;
  } else if(layer == 0) {
    pvMvtx_r   = &vMvtx0r;
    pvMvtx_phi = &vMvtx0phi;
    pvMvtx_z   = &vMvtx0z;
    
    hMvtx_dz0 = hMvtx0_dz0;
    hMvtx_dz1 = hMvtx0_dz1;
    hMvtx_dr  = hMvtx0_dr;
    hMvtx_dr2 = hMvtx0_dr2;
  }
  
  int nMvtx = pvMvtx_r->size();
  // search for hits of Mvt in the window
  int i=0;
  while(i<nMvtx) {
    float r   = pvMvtx_r->at(i);
    float phi = pvMvtx_phi->at(i);
    float z = pvMvtx_z->at(i);
    
    float x = r*cos(phi);
    float y = r*sin(phi);
    float rmvtx = sqrt((x-xc)*(x-xc)+(y-yc)*(y-yc));
    float dzdr = (z-zgvtx)/r;
    float zproj0 = zgvtx+dzdr*r0;
    float zproj1 = zgvtx+dzdr*r1;
    hMvtx_dr->Fill(rmvtx-rIntt);
    if(abs(rmvtx-rIntt)< InttMvtx_dr_cut) {
      hMvtx_dz0->Fill(z0-zproj0);
      hMvtx_dz1->Fill(z1-zproj1);
      if(abs(z0-zproj0) < InttMvtx_dz_cut  &&
	 abs(z1-zproj1) < InttMvtx_dz_cut ) {
	// A Mvtx hit is found in the window. store it.
	vMvtxHit.push_back(i);
	vMvtx_dr.push_back(rmvtx-rIntt);
	vMvtx_dz0.push_back(z0-zproj0);
	vMvtx_dz1.push_back(z1-zproj1);
      }
    }//if(abs(rmvtx...)
    // This part is to see the S/N of the associated hit
    if(abs(z0-zproj0) < InttMvtx_dz_cut &&
       abs(z1-zproj1) < InttMvtx_dz_cut ) {
      hMvtx_dr2->Fill(rmvtx-rIntt);
    }//if(abs(z0-zproj0)...
    i++;
  }//while(i)
  // Now Mvtx(layer) hits that can associated with
  // this InttPair track are found.
  // If there are more than one hits are found, chose
  // the best one.
  if(vMvtxHit.size()==0) return -99;
  else return SelectBestMvtxHit(vMvtxHit, vMvtx_dr, vMvtx_dz0, vMvtx_dz1);
}      

int SelectBestMvtxHit(vector<int> &vMvtxhit, vector<float> &vMvtx_dr,
		      vector<float> &vMvtx_dz0, vector<float> &vMvtx_dz1) {
  int nhit = vMvtxhit.size();
  int ihit=0;
  float drmin=100;
  int i_min;
  while(ihit<nhit) {
    float dr = abs(vMvtx_dr.at(ihit));
    if(dr < drmin) {
      drmin = dr;
      i_min = ihit;
    }
    ihit++;
  }
  return vMvtxhit.at(i_min);
}

int SelectBestTopHit(vector<int> &vTop_near_trk) {
  return vTop_near_trk.at(0);
}

void FindCaloInttMvtx(void) {
  // For Intt-Mvtx tracklet, find an Emc hit
  //  if(imode==2) cout << "FindCaloInttMvtx()"<<endl;
  int ntrk = vCaloInttMvtx.size();
  if(ntrk > 0) {
    for(int itrk=0;itrk<ntrk;itrk++) {
      CaloInttMvtx *pCIMtrack = &(vCaloInttMvtx.at(itrk));
      float pt_emc_intt  = pCIMtrack->pt0e;
      float pt_intt_mvtx = pCIMtrack->pt0m;
      float emc_e = pCIMtrack->emc_e;
      float emcal_e = pCIMtrack->emcal_e;
      float total_e = pCIMtrack->total_e;
      float phi0 = pCIMtrack->phi0e;
      float z0   = pCIMtrack->z0e;
      int Intt0_t = vINTT0t.at(pCIMtrack->iINTT0);
      int Intt1_t = vINTT1t.at(pCIMtrack->iINTT1);

      int iMvtx0 = pCIMtrack->iMvtx0;
      int iMvtx1 = pCIMtrack->iMvtx1;
      int iMvtx2 = pCIMtrack->iMvtx2;

      int nMvtxHits = 0;
      if( iMvtx2 >=0) nMvtxHits++;
      if( iMvtx1 >=0) nMvtxHits++;
      if( iMvtx0 >=0) nMvtxHits++;
      
      if(nMvtxHits>=2) {
	// at least two Mvt layers has a hit for this track
	// thus an Intt-Mvtx track is formed and its track parameters
	// at the origin (x0m,y0m,z0m,pt0m,phi0m,pz0m) are well defined
	//
	int ntop = vTop_r.size();
	vector<int> vTop_near_trk;
	if(ntop>0) {
	  for(int itop=0;itop<ntop;itop++) {
	    if(vTop_emc_emc_e.at(itop) > 0) {// Emc Hit in the topocluster
	      float emc_r   = vTop_emc_r.at(itop);
	      float emc_phi = vTop_emc_phi.at(itop);
	      float zemc = vTop_emc_z.at(itop);
	      float xemc = emc_r*cos(emc_phi);
	      float yemc = emc_r*sin(emc_phi);
	      float x0m = pCIMtrack->x0m;
	      float y0m = pCIMtrack->y0m;
	      float z0m = pCIMtrack->z0m;
	      float pt0m = pCIMtrack->pt0m;
	      float phi0m = pCIMtrack->phi0m;
	      float pz0m  = pCIMtrack->pz0m;
	      float rc = pCIMtrack->r_intt_mvtx;
	      float xc = pCIMtrack->xc_intt_mvtx;
	      float yc = pCIMtrack->yc_intt_mvtx;
	      float rcEmc = sqrt((xemc-xc)*(xemc-xc)+(yemc-yc)*(yemc-yc));
	      float dzdr = pz0m/pt0m;
	      float zproj = z0m + dzdr*emc_r;
	      if(abs(rcEmc-rc)<20 && abs(zemc-zproj)<8) {
		//Emc hit that matches to the track is found.
		vTop_near_trk.push_back(itop);
	      }
	    }//if(vTop_emc_emc_e)
	  }//for(itop)
	}//if(ntop>0)

	//	if(imode==2) cout << "top found:"<<vTop_near_trk.size()<<endl;
	if(vTop_near_trk.size()>0) {
	  int itop_best = SelectBestTopHit(vTop_near_trk);
	  float emc_r   = vTop_emc_r.at(itop_best);
	  float emc_phi = vTop_emc_phi.at(itop_best);
	  float xemc = emc_r*cos(emc_phi);
	  float yemc = emc_r*sin(emc_phi);

	  float r_emc_intt;
	  float xc_emc_intt;
	  float yc_emc_intt;
	  int iINTT0 = pCIMtrack->iINTT0;
	  float r_intt0   =  vINTT0r.at(iINTT0);
	  float phi_intt0 =  vINTT0phi.at(iINTT0);
	  float x_intt0 = r_intt0*cos(phi_intt0);
	  float y_intt0 = r_intt0*sin(phi_intt0);
	  // Draw a circle passing through (xemc,yemc)-INTT0-(0,0)
	  //CalcCircle(xemc,yemc,x_intt0,y_intt0,0.0,0.0,
	  CalcCircle(0.0,0.0,x_intt0,y_intt0,xemc,yemc,
		     r_emc_intt,xc_emc_intt,yc_emc_intt);
	  //	  if(imode==2) cout << "r_emc_intt = "<<r_emc_intt<<endl;

	  // This track is connected to Intt-Mvtx track. So the starting point
	  // is the same
	  float x0e  = pCIMtrack->x0m;
	  float y0e  = pCIMtrack->y0m;
	  float z0e  = pCIMtrack->z0m;
	  float phi0e= pCIMtrack->phi0m;
	  
	  // pT is determined by EmcIntt part of the track
	  float pt0m = pCIMtrack->pt0m;
	  float pz0m = pCIMtrack->pz0m;
	  float pt0e = 0.0042*r_emc_intt;
	  float pz0e = pt0e*(pz0m/pt0m);

	  pCIMtrack->iTop = itop_best;
	  pCIMtrack->r_emc_intt = r_emc_intt;
	  pCIMtrack->xc_emc_intt = xc_emc_intt;
	  pCIMtrack->yc_emc_intt = yc_emc_intt;
	  pCIMtrack->zvtx_emc_intt = pCIMtrack->zvtx_intt_mvtx;
	  pCIMtrack->xemc = xemc;
	  pCIMtrack->yemc = yemc;
	  pCIMtrack->zemc = vTop_emc_z.at(itop_best);
	  pCIMtrack->total_e = vTop_e.at(itop_best);
	  pCIMtrack->emcal_e = vTop_emc_e.at(itop_best);
	  pCIMtrack->ihcal_e = vTop_ihc_e.at(itop_best);
	  pCIMtrack->ohcal_e = vTop_ohc_e.at(itop_best);
	  pCIMtrack->emc_e = vTop_emc_emc_e.at(itop_best);
	  pCIMtrack->BDT_e = 0;
	  pCIMtrack->x0e = x0e;
	  pCIMtrack->y0e = y0e;
	  pCIMtrack->z0e = z0e;
	  pCIMtrack->pt0e = pt0e;
	  pCIMtrack->phi0e = phi0e;
	  pCIMtrack->pz0e = pz0e;	  
	}
      }//if(nMvtxHits>=2)
    }//for(itrk)
  }//if(ntrk>0)
}

#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

void INTT_Calo_trkUser_Init(void) {
  hUSR_ntrk = new TH1F("hUSR_ntrk","number of Calo-INTT tracks",21,-0.5,20.5);
  hUSR_EmcIntt_pt  = new TH1F("hUSR_EmcIntt_pt","pt of Calo-INTT track",100,0.,10.);
  hUSR_EmcIntt_pt2 = new TH1F("hUSR_EmcIntt_pt2","pt of Calo-INTT track w Mvtx hit",100,0.,10.);
  hUSR_EmcIntt_pt3 = new TH1F("hUSR_EmcIntt_pt3","pt of Calo-INTT track w 3 Mvtx hit",100,0.,10.);
  hUSR_Intt_pt  = new TH1F("hUSR_Intt_pt","pt of INTT track",100,0.,10.);
  hUSR_Intt_pt2 = new TH1F("hUSR_Intt_pt2","pt of INTT track w Mvtx hit",100,0.,10.);
  hUSR_Intt_pt3 = new TH1F("hUSR_Intt_pt3","pt of INTT track w 3 Mvtx hit",100,0.,10.);
  hUSR_Rpt  = new TH1F("hUSR_Rpt","pt_Intt_mvtx/pt_EmcIntt",100,0.0,4.0);
  hUSR_Rpt2 = new TH1F("hUSR_Rpt2","pt_Intt_mvtx/pt_EmcIntt",100,0.0,4.0);
  hUSR_nEmc = new TH1F("hUSR_nEmc","# of Emc clusters",101,-0.5,100.5);
  hUSR_ntower = new TH1F("hUSR_ntower","# of towers in EMC cluster", 41.,-0.5,40.5);
  hUSR_tower_e = new TH1F("hUSR_tower_e","tower energy",100,0.0,5.0);

  hUSR_Intt_t = new TH1F("hUSR_Intt_t","Intt hit timing",11,-5.5,5.5);
  hUSR_Mvtx0_t = new TH1F("hUSR_Mvtx0_t","Mvtx0 hit timing",11,-5.5,5.5);
  hUSR_Mvtx1_t = new TH1F("hUSR_Mvtx1_t","Mvtx1 hit timing",11,-5.5,5.5);
  hUSR_Mvtx2_t = new TH1F("hUSR_Mvtx2_t","Mvtx2 hit timing",11,-5.5,5.5);

  hUSR_EIM_dr = new TH1F("hUSR_EIM_dr","rEmc - rIntt_mvtx",200,-50.,50);
  hUSR_EIM_dr2 = new TH1F("hUSR_EIM_dr2","rEmc - rIntt_mvtx",200,-50.,50);
  hUSR_EIM_dz = new TH1F("hUSR_EIM_dz","zEmc - zproj",200,-50.,50.);
  hUSR_EIM_dz2 = new TH1F("hUSR_EIM_dz2","zEmc - zproj",200,-50.,50.);
  hUSR_EIM_dz3 = new TH1F("hUSR_EIM_dz3","zEmc - zproj",200,-50.,50.);

  hUSR_Mee = new TH1F("hUSR_Mee","Mee",1000,0.0,4.0);
  hUSR_pe = new TH1F("hUSR_pe","pe(tagged by Mee<0.04)",100,0.,5.);
  hUSR_Ep = new TH1F("hUSR_Ep","E/p for conversion e+/e-",100,0.,2.0);

  hUSR_truth_pt = new TH1F("hUSR_truth_pt","pt (truth)",100,0.,5.0);

  hUSR_pt0m_tr_pt = new TH1F("hUSR_pt0m_tr_pt","pt_mvtx/pt_truth",100,0.0,2.0);
  hUSR_pt0m_tr_pt1 = new TH1F("hUSR_pt0m_tr_pt1","pt_mvtx/pt_truth (0.3<pt<0.5GeV/c)",100,0.0,2.0);
  hUSR_pt0m_tr_pt2 = new TH1F("hUSR_pt0m_tr_pt2","pt_mvtx/pt_truth (0.5<pt<0.7GeV/c)",100,0.0,2.0);
  hUSR_pt0m_tr_pt3 = new TH1F("hUSR_pt0m_tr_pt3","pt_mvtx/pt_truth (0.7<pt<1.0GeV/c)",100,0.0,2.0);
  hUSR_pt0m_tr_pt4 = new TH1F("hUSR_pt0m_tr_pt4","pt_mvtx/pt_truth (1.0<pt<1.5GeV/c)",100,0.0,2.0);
  hUSR_pt0m_tr_pt5 = new TH1F("hUSR_pt0m_tr_pt5","pt_mvtx/pt_truth (1.5<pt<2.0GeV/c)",100,0.0,2.0);
  hUSR_pt0m_tr_pt6 = new TH1F("hUSR_pt0m_tr_pt6","pt_mvtx/pt_truth (2.0<pt<3.0GeV/c)",100,0.0,2.0);

  hUSR_pt0e_tr_pt = new TH1F("hUSR_pt0e_tr_pt","pt_emc/pt_truth",100,0.0,2.0);
  hUSR_pt0e_tr_pt1 = new TH1F("hUSR_pt0e_tr_pt1","pt_emc/pt_truth (0.3<pt<0.5GeV/c)",100,0.0,2.0);
  hUSR_pt0e_tr_pt2 = new TH1F("hUSR_pt0e_tr_pt2","pt_emc/pt_truth (0.5<pt<0.7GeV/c)",100,0.0,2.0);
  hUSR_pt0e_tr_pt3 = new TH1F("hUSR_pt0e_tr_pt3","pt_emc/pt_truth (0.7<pt<1.0GeV/c)",100,0.0,2.0);
  hUSR_pt0e_tr_pt4 = new TH1F("hUSR_pt0e_tr_pt4","pt_emc/pt_truth (1.0<pt<1.5GeV/c)",100,0.0,2.0);
  hUSR_pt0e_tr_pt5 = new TH1F("hUSR_pt0e_tr_pt5","pt_emc/pt_truth (1.5<pt<2.0GeV/c)",100,0.0,2.0);
  hUSR_pt0e_tr_pt6 = new TH1F("hUSR_pt0e_tr_pt6","pt_emc/pt_truth (2.0<pt<3.0GeV/c)",100,0.0,2.0);
  
  nt_trk = new TNtuple("nt_trk","INTT track Ntuple","p_emc:p_mvtx:tot_e:emc_e:sign:ntrk:pt_mvtx");
  nt_sim_trk = new TNtuple("nt_sim_trk","Sim and Reco Ntuple","tr_pt:tr_phi0:tr_eta0:pt0e:phi_e:pt0m");
}

void INTT_Calo_trkUser_End(void) {
  hUSR_ntrk->Write();
  hUSR_Intt_pt->Write();
  hUSR_Intt_pt2->Write();
  hUSR_Intt_pt3->Write();
  hUSR_EmcIntt_pt->Write();
  hUSR_EmcIntt_pt2->Write();
  hUSR_EmcIntt_pt3->Write();
  hUSR_Rpt->Write();
  hUSR_Rpt2->Write();
  hUSR_nEmc->Write();
  hUSR_ntower->Write();
  hUSR_tower_e->Write();

  hUSR_Intt_t->Write();
  hUSR_Mvtx0_t->Write();
  hUSR_Mvtx1_t->Write();
  hUSR_Mvtx2_t->Write();

  hUSR_EIM_dr->Write();
  hUSR_EIM_dr2->Write();
  hUSR_EIM_dz->Write();
  hUSR_EIM_dz2->Write();
  hUSR_EIM_dz3->Write();

  hUSR_Mee->Write();
  hUSR_pe->Write();
  hUSR_Ep->Write();

  hUSR_truth_pt->Write();

  hUSR_pt0m_tr_pt->Write();
  hUSR_pt0m_tr_pt1->Write();
  hUSR_pt0m_tr_pt2->Write();
  hUSR_pt0m_tr_pt3->Write();
  hUSR_pt0m_tr_pt4->Write();
  hUSR_pt0m_tr_pt5->Write();
  hUSR_pt0m_tr_pt6->Write();

  hUSR_pt0e_tr_pt->Write();
  hUSR_pt0e_tr_pt1->Write();
  hUSR_pt0e_tr_pt2->Write();
  hUSR_pt0e_tr_pt3->Write();
  hUSR_pt0e_tr_pt4->Write();
  hUSR_pt0e_tr_pt5->Write();
  hUSR_pt0e_tr_pt6->Write();
  
  nt_trk->Write();
  nt_sim_trk->Write();
}

void INTT_Calo_trkUser(void) {
  int ntrk = vCaloInttMvtx.size();
  if(imode == 2) { cout << "ntrk="<<ntrk<<endl;}
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
  
  // positive tracks
  vector<float> vpxp;
  vector<float> vpyp;
  vector<float> vpzp;
  vector<float> vEp;  //Emc energy  

  // negative tracks
  vector<float> vpxm;
  vector<float> vpym;
  vector<float> vpzm;
  vector<float> vEm;  //Emc energy
  
  if(ntrk > 0) {
    /*
    cout << "i_event="<<i_event;
    cout << " ntrk="<<ntrk<<endl;
    */

    vpxp.clear();
    vpyp.clear();
    vpzp.clear();
    
    vpxm.clear();
    vpym.clear();
    vpzm.clear();
    
    for(int i=0;i<ntrk;i++) {
      CaloInttMvtx CIMtrack = vCaloInttMvtx.at(i);
      float pt_emc_intt  = CIMtrack.pt0e;
      float pt_intt_mvtx = CIMtrack.pt0m;
      float phi0m = CIMtrack.phi0m;
      float emc_e = CIMtrack.emc_e;
      float emcal_e = CIMtrack.emcal_e;
      float total_e = CIMtrack.total_e;
      float phi0 = CIMtrack.phi0e;
      float z0 = CIMtrack.z0e;
      int Intt0_t = vINTT0t.at(CIMtrack.iINTT0);
      int Intt1_t = vINTT1t.at(CIMtrack.iINTT1);
      hUSR_Intt_pt->Fill(pt_intt_mvtx);
      if(pt_emc_intt>0) hUSR_EmcIntt_pt->Fill(pt_emc_intt);
      hUSR_Intt_t->Fill(Intt0_t);
      hUSR_Intt_t->Fill(Intt1_t);
      
      int iMvtx0 = CIMtrack.iMvtx0;
      int iMvtx1 = CIMtrack.iMvtx1;
      int iMvtx2 = CIMtrack.iMvtx2;
      
      int nMvtxHits = 0;
      if( iMvtx2 >=0) nMvtxHits++;
      if( iMvtx1 >=0) nMvtxHits++;
      if( iMvtx0 >=0) nMvtxHits++;

      if(imode == 2) {
	cout << i << ": INTT0="<<CIMtrack.iINTT0<<" INTT1="<<CIMtrack.iINTT1;
	cout <<" Mvtx0="<<iMvtx0<<" Mvtx1="<<iMvtx1<<" iMvtx2= "<<iMvtx2<<endl;
      }
      if( nMvtxHits > 0) {
	hUSR_Intt_pt2->Fill(pt_intt_mvtx);
	if(pt_emc_intt>0) {
	  hUSR_EmcIntt_pt2->Fill(pt_emc_intt);
	  hUSR_Rpt->Fill(pt_intt_mvtx/pt_emc_intt);
	  if(0.8 < pt_emc_intt && pt_emc_intt < 4.0) hUSR_Rpt2->Fill(pt_intt_mvtx/pt_emc_intt);
	}
      }//if( nMvtxHits)

      if( nMvtxHits >=3) {
	hUSR_Intt_pt3->Fill(pt_intt_mvtx);
	if(pt_emc_intt>0) hUSR_EmcIntt_pt3->Fill(pt_emc_intt);
      }//if(nMvtxHits>=3)

      if(nMvtxHits>=2) {
	// at least two Mvt layers has a hit for this track
	// thus an Intt-Mvtx track is formed and its track parameters
	// at the origin (x0m,y0m,z0m,pt0m,phi0m,pz0m) are well defined
	//
	int ntop = vTop_r.size();
	if(ntop>0) {
	  for(int itop=0;itop<ntop;itop++) {
	    if(vTop_emc_emc_e.at(itop) > 0) {// Emc Hit in the topocluster
	      float emc_r   = vTop_emc_r.at(itop);
	      float emc_phi = vTop_emc_phi.at(itop);
	      float zemc = vTop_emc_z.at(itop);
	      float xemc = emc_r*cos(emc_phi);
	      float yemc = emc_r*sin(emc_phi);
	      float x0m = CIMtrack.x0m;
	      float y0m = CIMtrack.y0m;
	      float z0m = CIMtrack.z0m;
	      float pt0m = CIMtrack.pt0m;
	      float phi0m = CIMtrack.phi0m;
	      float pz0m  = CIMtrack.pz0m;
	      float rc = CIMtrack.r_intt_mvtx;
	      float xc = CIMtrack.xc_intt_mvtx;
	      float yc = CIMtrack.yc_intt_mvtx;
	      float rcEmc = sqrt((xemc-xc)*(xemc-xc)+(yemc-yc)*(yemc-yc));
	      float dzdr = pz0m/pt0m;
	      float zproj = z0m + dzdr*emc_r;
	      hUSR_EIM_dr->Fill(rcEmc - rc);
	      hUSR_EIM_dz->Fill(zemc - zproj);
	      if(abs(rcEmc-rc)<20) hUSR_EIM_dz2->Fill(zemc - zproj);
	      if(abs(zemc-zproj)<8) hUSR_EIM_dr2->Fill(rcEmc - rc);
	      if(abs(rcEmc-rc)<20 && abs(zemc-zproj)<8) {
		hUSR_EIM_dz3->Fill(zemc - zproj);
	      }
	    }//if
	  }//for
	}//if(ntop)

	if(pt_emc_intt>0 && pt_intt_mvtx > 0) {
	  float pz0e = CIMtrack.pz0e;
	  float pz0m = CIMtrack.pz0m;
	  float px0m = pt_intt_mvtx*cos(phi0m);
	  float py0m = pt_intt_mvtx*sin(phi0m);
	  float p_emc  = sqrt(pt_emc_intt*pt_emc_intt+pz0e*pz0e);
	  float p_mvtx = sqrt(pt_intt_mvtx*pt_intt_mvtx+pz0m*pz0m);
	  int sign = CIMtrack.sign;
	  nt_trk->Fill(p_emc,p_mvtx,total_e,emcal_e,sign,ntrk,pt_intt_mvtx);
	}//if(pt_emc_intt...)	
      }//if(nMvtxHits>=2)

      if(pt_intt_mvtx>0.2 && nMvtxHits>=2) {
	float pz0m = CIMtrack.pz0m;
	float px0m = pt_intt_mvtx*cos(phi0m);
	float py0m = pt_intt_mvtx*sin(phi0m);
	int sign = CIMtrack.sign;
	if(sign > 0) {
	  vpxp.push_back(px0m);
	  vpyp.push_back(py0m);
	  vpzp.push_back(pz0m);
	  vEp.push_back(emc_e);
	} else {
	  vpxm.push_back(px0m);
	  vpym.push_back(py0m);
	  vpzm.push_back(pz0m);
	  vEm.push_back(emc_e);
	}
      }//if(pt_intt_mvtx)
      
      if( iMvtx0 >= 0) hUSR_Mvtx0_t->Fill(vMvtx0t.at(iMvtx0));
      if( iMvtx1 >= 0) hUSR_Mvtx1_t->Fill(vMvtx1t.at(iMvtx1));
      if( iMvtx2 >= 0) hUSR_Mvtx2_t->Fill(vMvtx2t.at(iMvtx2));
    }// for(i)

    const float me = 0.000511;
    int Np = vpxp.size();
    int Nm = vpxm.size();
    if(Np>0 && Nm>0) {
      for(int ip=0;ip<Np;ip++) {
	float pxp=vpxp.at(ip);
	float pyp=vpyp.at(ip);
	float pzp=vpzp.at(ip);
	float pp = sqrt(pxp*pxp + pyp*pyp + pzp*pzp);
	float Ep = sqrt(pp*pp + me*me);
	for(int im=0;im<Nm;im++) {
	  float pxm=vpxm.at(im);
	  float pym=vpym.at(im);
	  float pzm=vpzm.at(im);
	  float pm = sqrt(pxm*pxm + pym*pym + pzm*pzm);
	  float Em = sqrt(pm*pm + me*me);

	  float Eee  = Ep+Em;
	  float pxee = pxp+pxm;
	  float pyee = pyp+pym;
	  float pzee = pzp+pzm;
	  float Mee = sqrt(Eee*Eee - pxee*pxee - pyee*pyee - pzee*pzee);
	  hUSR_Mee->Fill(Mee);

	  if( Mee < 0.04) {
	    if(vEp.at(ip)>0) {
	      hUSR_pe->Fill(pp);
	      hUSR_Ep->Fill(vEp.at(ip)/pp);
	    }
	    if(vEm.at(im)>0) {
	      hUSR_pe->Fill(pm);
	      hUSR_Ep->Fill(vEm.at(im)/pm);
	    }
	  }
	}//for(im)
      }//for(ip)
    }
  }//if(ntrk)

  // Simulation 
  if(truthTree != nullptr) {
    int n_tr_trk = truth_pid->size();  //# of truth tracks
    //    cout <<"n_tr_trk="<<n_tr_trk<<endl;
    if(n_tr_trk>0) {
      for(int itr = 0; itr<n_tr_trk; itr++) {
	float tr_px=truth_px->at(itr);
	float tr_py=truth_py->at(itr);
	float tr_pz=truth_pz->at(itr);
	float tr_pt=sqrt(tr_px*tr_px + tr_py*tr_py);
	float tr_phi0 = atan2(tr_py,tr_px);
	float tr_eta0 = rz2eta(tr_pt,tr_pz);
	hUSR_truth_pt->Fill(tr_pt);

	if( n_tr_trk==1 && ntrk==1) {
	  if(itr==0) {
	    CaloInttMvtx CIMtrack = vCaloInttMvtx.at(0);
	    float pt0m = CIMtrack.pt0m;
	    float pt0e = CIMtrack.pt0e;
	    float xemc = CIMtrack.xemc;
	    float yemc = CIMtrack.yemc;
	    float phi_e = atan2(yemc,xemc);
	    
	    nt_sim_trk->Fill(tr_pt,tr_phi0,tr_eta0,pt0e,phi_e,pt0m);
	    if(tr_pt > 0.3) {
	      hUSR_pt0m_tr_pt->Fill(pt0m/tr_pt);
	      hUSR_pt0e_tr_pt->Fill(pt0e/tr_pt);
	      if(tr_pt <0.5) {
		hUSR_pt0m_tr_pt1->Fill(pt0m/tr_pt);
		hUSR_pt0e_tr_pt1->Fill(pt0e/tr_pt);
	      } else if(tr_pt<0.7) {
		hUSR_pt0m_tr_pt2->Fill(pt0m/tr_pt);
		hUSR_pt0e_tr_pt2->Fill(pt0e/tr_pt);
	      } else if(tr_pt<1.0) {
		hUSR_pt0m_tr_pt3->Fill(pt0m/tr_pt);
		hUSR_pt0e_tr_pt3->Fill(pt0e/tr_pt);
	      } else if(tr_pt<1.5) {
		hUSR_pt0m_tr_pt4->Fill(pt0m/tr_pt);
		hUSR_pt0e_tr_pt4->Fill(pt0e/tr_pt);
	      } else if(tr_pt<2.0) {
		hUSR_pt0m_tr_pt5->Fill(pt0m/tr_pt);
		hUSR_pt0e_tr_pt5->Fill(pt0e/tr_pt);
	      } else if(tr_pt<3.0) {
		hUSR_pt0m_tr_pt6->Fill(pt0m/tr_pt);
		hUSR_pt0e_tr_pt6->Fill(pt0e/tr_pt);
	      }
	    }
	  }//if(itr)
	}//if(n_tr_trk,ntrk)
      }//for(itr)
    }//if(n_tr_trk)
  }//if(truthTree)
}

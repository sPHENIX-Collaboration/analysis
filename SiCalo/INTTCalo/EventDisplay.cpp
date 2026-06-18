#include <iostream>
#include "INTT_Calo_trkReco.h"
#include "EventDisplay.h"

const float PI = 3.141592;

void EventSelection( int& ievent) {
  // Menu
  int imenu;
  cout <<" <0: quit"<<endl;
  cout <<"  0: display current data"<<endl;
  cout <<"  1: display next data"<<endl;
  cout <<"  2: display specified event"<<endl;
  cin >> imenu;
  if(imenu<0) {
    cout <<" quit "<<endl;
    ievent = -1;
    return;
  }
  if(imenu==0) {
    cout <<"Event "<<ievent<<endl;
  }
  if(imenu==1) {
    ievent ++;
    cout <<"Event "<<ievent<<endl;
  }
  if(imenu==2) {
    cout <<"Enter event number to display (negative to quit) "<<endl;
    cin>>ievent;
    if(ievent<0 || ievent >40000) {
      ievent = -1;
      return;
    }
  }
  if(imenu>2) {
    cout <<" out of range. Keep current event"<<endl;
  }
  
  cout<<" enter phiSlice"<<endl;
  cin>>phiSlice;
  cout << "phiSlice="<<phiSlice<<endl;
}


void EventDisplay(int iframe){   

  int nEmcClust=energy->size();
  int ntrack=pt0->size();
  int nSiClust=si_x->size();
  int nSiClustA=a_si_x->size();
  
  if(nEmcClust>nMax) nEmcClust=nMax;

  // Draw hits (EMC and Silicon) and seedtrk in c1

  if(c1==nullptr) {
    c1 = new TCanvas("c1","XY View",500.,500.);
    frame1 = c1->DrawFrame(-240.,-240.,240.,240.);
  }
  if(c2==nullptr) {
    c2 = new TCanvas("c2","RZ View",500.,250.);
    frame2 = c2->DrawFrame(-240.,0.,240.,240.);
  }
  if(c3==nullptr) {
    c3 = new TCanvas("c3","XY View",500.,500.);
    c3->SetGridx();
    c3->SetGridy();
    frame3 = c3->DrawFrame(-12,-12,12,12);
  }
  if(c4==nullptr) {
    c4 = new TCanvas("c4","RZ View",750.,250.);
    frame4 = c4->DrawFrame(-18,0,18,12);
  }
    
  if(nhits>0) {
    for(int ihit=0;ihit<nhits;ihit++) {
      hits[ihit]->Delete();
    }
    nhits=0;
  }

  if(nhitsRZ>0) {
    for(int ihitRZ=0;ihitRZ<nhitsRZ;ihitRZ++) {
      hitsRZ[ihitRZ]->Delete();
    }
    nhitsRZ=0;
  }

  if(ntrks>0) {
    for(int itrk=0;itrk<ntrks;itrk++) {
      seedtrk[itrk]->Delete();
    }
    ntrks=0;
  }
  if(ntrksRZ>0){
    for(int itrkRZ=0;itrkRZ<ntrksRZ;itrkRZ++) {
      seedtrkRZ[itrkRZ]->Delete();
    }
    ntrksRZ=0;
  }
  
  float s0;    //size of EMC and top hit
  float s1;    //size of Silicon hit
  float s2;    //size of SiliconAll hit
  const float sLA=0.8;  //half length of strip. type-A
  const float sLB=1.0;  //half lenght of strip. type-B
  
  s0=20.0;   // unit size of calo cluster
  if(iframe==0) {
    s1=0.5;
    s2=0.2;
  } else {
    s1=0.2;
    s2=0.1;
  }

  if(abs(zgvtx)>50) {
    zgvtx = 0;
  }

  // Create ZVTX marker
  if(ZvtxMarker !=nullptr) ZvtxMarker->Delete();
  ZvtxMarker= new TArrow(zgvtx,2.0,zgvtx,0.0,0.01);
  ZvtxMarker->SetLineWidth(2);
  ZvtxMarker->SetLineColor(kRed);
  
  //  Create EM hits
  if(nEmcClust>0) {
    for(int j=0;j<nEmcClust;j++){
      x[j]= (emc_r->at(j))*cos(emc_phi->at(j));
      y[j]= (emc_r->at(j))*sin(emc_phi->at(j));
      
      float sE = sqrt(s0*energy->at(j));

      hits[nhits]=new TBox(x[j]-sE,y[j]-sE,x[j]+sE,y[j]+sE);
      hits[nhits]->SetFillColor(kBlue);
      nhits++;
	
      if(abs(emc_phi->at(j)-phiSlice)<DphiSlice) {
	float Remc = emc_r->at(j);
	float Zemc = emc_z->at(j);
	hitsRZ[nhitsRZ]=new TBox(Zemc-sE,Remc-sE,Zemc+sE,Remc+sE);
	hitsRZ[nhitsRZ]->SetFillColor(kBlue);
	nhitsRZ++;
      }//for
    }//if(nEmcClust)
  }

  // Create topoCluster hits
  int nTop=top_e->size();
  if(nTop>0) {
    for(int itop=0;itop< nTop ;itop++){
      x[itop] = top_x->at(itop);
      y[itop] = top_y->at(itop);
      float sE = sqrt(s0*top_e->at(itop));
      
      hits[nhits]=new TBox(x[itop]-sE,y[itop]-sE,x[itop]+sE,y[itop]+sE);
      hits[nhits]->SetFillColor(kRed);
      nhits++;
      
      if(abs(top_phi->at(itop)-phiSlice)<DphiSlice) {
	float Rtop = top_r->at(itop);
	float Ztop = top_z->at(itop);
	hitsRZ[nhitsRZ]=new TBox(Ztop-sE,Rtop-sE,Ztop+sE,Rtop+sE);
	hitsRZ[nhitsRZ]->SetFillColor(kRed);
	nhitsRZ++;
      }//if(phiSlice)
    }//for itop;
  }//if(nTop)

  // Creates Silicon hits
  // Note that the track reconstruciton is done after shifting
  // the beam center (xBC, yBC) to (0.0). To be consitent with
  // tracks, silicon hit positions are shifted by the same amount
  // in the event display.
  if(nSiClust>0) {
    for(int jj=0;jj<nSiClust;jj++) {
      x[jj]=si_x->at(jj) - xBC;
      y[jj]=si_y->at(jj) - yBC;
      hits[nhits]=new TBox(x[jj]-s1,y[jj]-s1,x[jj]+s1,y[jj]+s1);
      if(layer->at(jj) < 3) hits[nhits]->SetFillColor(kGreen);
      else          hits[nhits]->SetFillColor(kRed);
      nhits++;
      
      float phiHit=atan2(y[jj],x[jj]);
      if(abs(phiHit-phiSlice)<DphiSlice) {
	float Rhit =  sqrt(x[jj]*x[jj]+y[jj]*y[jj]);
	float Zhit =  si_z->at(jj);
	
	if(layer->at(jj) < 3) { //MVTX hit
	  hitsRZ[nhitsRZ]=new TBox(Zhit-s1,Rhit-s1,Zhit+s1,Rhit+s1);
	  hitsRZ[nhitsRZ]->SetFillColor(kGreen);
	} else { //INTT hit
	  if(abs(Zhit)<12.8) { // type-A strip
	    hitsRZ[nhitsRZ]=new TBox(Zhit-sLA,Rhit-s1,Zhit+sLA,Rhit+s1);
	  } else {  // type-B strip
	    hitsRZ[nhitsRZ]=new TBox(Zhit-sLB,Rhit-s1,Zhit+sLB,Rhit+s1);
	  }
	  hitsRZ[nhitsRZ]->SetFillColor(kRed);
	}
	nhitsRZ++;
      }//for(jj)
    }//if(nSiClust)
  }

  // Creates SiliconAll hits
  if(nSiClustA>0) {
    for(int jj=0;jj<nSiClustA;jj++) {
      x[jj]=a_si_x->at(jj) - xBC;
      y[jj]=a_si_y->at(jj) - yBC;
      hits[nhits]=new TBox(x[jj]-s2,y[jj]-s2,x[jj]+s2,y[jj]+s2);
      if(a_layer->at(jj) < 3) hits[nhits]->SetFillColor(kBlack);
      else          hits[nhits]->SetFillColor(kBlack);
      nhits++;
      
      float phiHit=atan2(y[jj],x[jj]);
      if(abs(phiHit-phiSlice)<DphiSlice) {
	float Rhit =  sqrt(x[jj]*x[jj]+y[jj]*y[jj]);
	float Zhit =  a_si_z->at(jj);
	
	if(a_layer->at(jj) < 3) { // MVTX hit
	  hitsRZ[nhitsRZ]=new TBox(Zhit-s2,Rhit-s2,Zhit+s2,Rhit+s2);
	  hitsRZ[nhitsRZ]->SetFillColor(kBlack);
	} else {// INTT hit
	  if(abs(Zhit)<12.8) { // type-A strip
	    hitsRZ[nhitsRZ]=new TBox(Zhit-sLA,Rhit-s2,Zhit+sLA,Rhit+s2);
	  } else { // type-B strip
	    hitsRZ[nhitsRZ]=new TBox(Zhit-sLB,Rhit-s2,Zhit+sLB,Rhit+s2);
	  }
	  hitsRZ[nhitsRZ]->SetFillColor(kBlack);
	}
	nhitsRZ++;
      }
    }//if(nSiClustA)
  }
  
  // Creates SiSeed tracklets (arrow)
  float Rtrk;
  const float Asize = 0.02;
  if(iframe==1) Rtrk=2.0; else Rtrk=80;
  if(iframe==1) Rmax=12.0; else Rmax=80;
  for(int k=0;k<ntrack;k++) {
    if(pt0->at(k)>pt0min) {
      float dx=Rtrk*cos(phi0->at(k));
      float dy=Rtrk*sin(phi0->at(k));
      seedtrk[ntrks]=new TArrow(vx0->at(k),vy0->at(k),vx0->at(k)+dx,vy0->at(k)+dy,Asize);
      ntrks++;

      if(abs(phi0->at(k)-phiSlice)<DphiSlice) {
	float r0=sqrt((vx0->at(k))*(vx0->at(k))+(vy0->at(k))*(vy0->at(k)));
	float eta = eta0->at(k);
	float theta = 2*atan(exp(-eta));
	float Zend = (Rmax-r0)/tan(theta)+vz0->at(k);

	seedtrkRZ[ntrksRZ]=new TArrow(vz0->at(k),r0,Zend,Rmax,Asize);
	ntrksRZ++;
      }
    }
  }
  
  // clear track orbits of the previous event;
  if(vEmcINTTorbit.size()>0) {
    for(int io=0;io<(int)vEmcINTTorbit.size();io++) {
      vEmcINTTorbit.at(io)->Delete();
    }//for(io)    
  }//if
  vEmcINTTorbit.clear();

  if(vEmcINTT_RZ.size()>0) {
    for(int io=0;io<(int)vEmcINTT_RZ.size();io++) {
      vEmcINTT_RZ.at(io)->Delete();
    }
  }
  vEmcINTT_RZ.clear();
  // create Circle orbit of CaloInttMvtx tracks
  int ntrk = vCaloInttMvtx.size();
  cout <<" # of tracks = "<<ntrk<<endl;

  if(ntrk>0) { 
    for(int itrk=0;itrk<ntrk;itrk++) {
      CaloInttMvtx CIMtrack = vCaloInttMvtx.at(itrk);
      // create emc_intt tracks
      float r_e  = CIMtrack.r_emc_intt;
      if(r_e>0) { //this track is a EMC-INTT
	float xc_e = CIMtrack.xc_emc_intt-xBC;
	float yc_e = CIMtrack.yc_emc_intt-yBC;
	float xemc = CIMtrack.xemc;
	float yemc = CIMtrack.yemc;
     	float zemc = CIMtrack.zemc;
	float phi0e = CIMtrack.phi0e;
	float remc = sqrt(xemc*xemc+yemc*yemc);
	float zvtx_e = CIMtrack.zvtx_emc_intt;
	float zsign_e = xc_e*yemc-yc_e*xemc;
	float phi0_e = atan2(-yc_e,-xc_e)*180./PI;
	TArc *orbit_e;
	if(zsign_e<0) {
	  orbit_e = new TArc(xc_e,yc_e,r_e,phi0_e,phi0_e+10000/r_e);
	} else {
	  orbit_e = new TArc(xc_e,yc_e,r_e,phi0_e,phi0_e-10000/r_e);
	}
	orbit_e->SetFillStyle(0);
	orbit_e->SetLineColor(kGreen);
	vEmcINTTorbit.push_back(orbit_e);

	//create RZ view of the track
	if(abs(phi0e - phiSlice)<DphiSlice) {
	  TArrow *RZ_trk = new TArrow(zvtx_e,0,zemc,remc,0.03);
	  RZ_trk->SetLineColor(kGreen);
	  vEmcINTT_RZ.push_back(RZ_trk);
	}
      }//if(r_e>0)

      // create intt-mvtx tracks
      float r_m  = CIMtrack.r_intt_mvtx;
      if(r_m>0) {  //this track is a INTT-Mvtx track
	float xc_m = CIMtrack.xc_intt_mvtx;
	float yc_m = CIMtrack.yc_intt_mvtx;
	float x_m = CIMtrack.x0m;
	float y_m = CIMtrack.y0m;
	float z_m = CIMtrack.z0m;
	float phi0m = CIMtrack.phi0m;
	float r0m = sqrt(x_m*x_m+y_m*y_m);
	float zvtx_m = CIMtrack.zvtx_intt_mvtx;
	float dzdr = (z_m-zvtx_m)/r0m;
	float zsign_m = xc_m*y_m-yc_m*x_m;
	float phi0_m = atan2(-yc_m,-xc_m)*180./PI;
	const float R_emc = 100.0;
	TArc *orbit_m;
	if(zsign_m<0) {
	  orbit_m = new TArc(xc_m,yc_m,r_m,phi0_m,phi0_m+10000/r_m);
	} else {
	  orbit_m = new TArc(xc_m,yc_m,r_m,phi0_m,phi0_m-10000/r_m);
	}
	orbit_m->SetFillStyle(0);
	orbit_m->SetLineColor(kBlue);
	vEmcINTTorbit.push_back(orbit_m);	

	//create RZ view of the track
	if(abs(phi0m - phiSlice)<DphiSlice) {
	  TArrow *RZ_trk = new TArrow(zvtx_m,0,zvtx_m+dzdr*R_emc,R_emc,0.03);
	  RZ_trk->SetLineColor(kBlue);
	  vEmcINTT_RZ.push_back(RZ_trk);
	}
      }//if(r_m)      
    }//for(itrk)
  }
  // Draw X-Y view
  if(iframe==0) {
    c1->cd();
  } else {
    c3->cd();
  }
  
  for(int kk=0;kk<nhits;kk++){
    hits[kk]->Draw();
  }
  for(int itk=0;itk<ntrks;itk++) {
    seedtrk[itk]->Draw();
  }
  // Draw phi slice region
  if(dphiArc != nullptr) dphiArc->Delete();
  float phimin=(phiSlice-DphiSlice)*180./PI;
  float phimax=(phiSlice+DphiSlice)*180./PI;
  dphiArc = new TArc(0.,0.,Rmax,phimin,phimax);
  dphiArc->SetLineColorAlpha(0,0.1);
  dphiArc->SetFillColorAlpha(kBlue,0.1);
  dphiArc->Draw();

  // Draw Circle for reference size of INTT and MVTX
  if(Circle2==nullptr) {
    Circle2=new TArc(0.,0.,2.3,0,360);
    Circle2->SetFillStyle(0);
  }
  if(Circle4==nullptr) {
    Circle4=new TArc(0.,0.,4.0,0,360);
    Circle4->SetFillStyle(0);
  }
  if(Circle8==nullptr) {
    Circle8=new TArc(0.,0.,7.6,0.,360);
    Circle8->SetFillStyle(0);
  }
  if(Circle10==nullptr) {
    Circle10=new TArc(0.,0.,10.2,0.,360);
    Circle10->SetFillStyle(0);
  }
  Circle2->Draw();
  Circle4->Draw();
  Circle8->Draw();
  Circle10->Draw();

  // draw EmcINTT tracks
  int norbit = vEmcINTTorbit.size();
  if(norbit>0) {
    for(int i=0;i<norbit;i++) {
      vEmcINTTorbit.at(i)->Draw("only");
    }//for
  }//if
  if(iframe==0) {
    //    c1->Draw();
    c1->Modified();
    c1->Update();
  } else {
    //    c3->Draw();
    c3->Modified();
    c3->Update();
  }
  //Draw RZ slice
  if(iframe==0) {
    c2->cd();
  } else {
    c4->cd();
  }

  ZvtxMarker->Draw();
  for(int kk=0;kk<nhitsRZ;kk++){
    hitsRZ[kk]->Draw();
  }
  for(int itk=0;itk<ntrksRZ;itk++) {
    seedtrkRZ[itk]->Draw();
  }
  int norbitRZ = vEmcINTT_RZ.size();
  if(norbitRZ>0) {
    for(int i=0;i<norbitRZ;i++) {
      vEmcINTT_RZ.at(i)->Draw();
    }//for(i)
  }//if(ntrk)
  if(iframe==0) {
    c2->Modified();
    c2->Update();
  } else {
    c4->Modified();
    c4->Update();
  }
}

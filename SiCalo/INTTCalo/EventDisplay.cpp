#include <iostream>
#include "EventDisplay.h"

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

bool Is_Hot_EMC(int iemc) {
  float phiEMC = emc_phi->at(iemc);
  float zEMC = emc_z->at(iemc);
  if( (2.4 < phiEMC && phiEMC < 2.7) && (zEMC < 0)) return true;
  else return false;
}

void EventDisplay(void){   

  int nEmcClust=energy->size();
  int ntrack=pt0->size();
  int nSiClust=si_x->size();
  int nSiClustA=a_si_x->size();
  
  if(nEmcClust>nMax) nEmcClust=nMax;

  // Draw hits (EMC and Silicon) and seedtrk in c1

  { // select frame.
    int new_frame;
    cout << "select frame (0 or 1)" << endl;
    cin >> new_frame;
    if(new_frame != 0) new_frame=1;
    if(new_frame != iframe) {
      cout << "frame is changed to "<<new_frame<<endl;
      iframe = new_frame;
      if(c1==nullptr){
	c1 = new TCanvas("c1","XY View",800.,800.);
      } else {
	c1->cd();
      }
      if(iframe==0) {
	frame1 = c1->DrawFrame(-240.,-240.,240.,240.);
      } else if(iframe==1) {
	frame1 = c1->DrawFrame(-16.,-16.,16.,16.);
      }
      
      if(c2==nullptr){
	c2= new TCanvas("c2","RZ Slice",800,270);
      } else {
	c2->cd();
      }
      if(iframe==0) {
	frame2 = c2->DrawFrame(-360.,0.,360.,240.);
      } else if(iframe==1) {
	frame2 = c2->DrawFrame(-24.,0.,24.,16.);
      }
    }
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

  //  cout << "ntrks="<<ntrks<<endl;
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
  if(iframe==0) {
    s0=10.0;
    s1=0.5;
    s2=0.2;
  } else {
    s0=0.3;
    s1=0.2;
    s2=0.1;
  }

  
  cout <<evt<< ":   nEmcClust="<<nEmcClust;
  cout <<" ntop = "<<top_e->size();
  cout <<" nSiClust="<<nSiClust;
  cout <<" nSiClustA="<<nSiClustA;
  cout <<" ntrack="<<ntrack;
  cout <<" zgvtx="<<zgvtx;
  if(abs(zgvtx)>50) {
    zgvtx = 0;
    cout << "--> "<<zgvtx;
  }
  cout <<endl;

  // Create ZVTX marker
  if(ZvtxMarker !=nullptr) ZvtxMarker->Delete();
  ZvtxMarker= new TArrow(zgvtx,2.0,zgvtx,0.0,0.01);
  ZvtxMarker->SetLineWidth(2);
  ZvtxMarker->SetLineColor(kRed);
  
  //  Create EM hits
  for(int j=0;j<nEmcClust;j++){
    if(!Is_Hot_EMC(j)) {
      x[j]= (emc_r->at(j))*cos(emc_phi->at(j));
      y[j]= (emc_r->at(j))*sin(emc_phi->at(j));


      float sE = sqrt(s0*energy->at(j));
      // rescale EMC hits if iframe==1
      if(iframe == 1) {
	x[j]=EMScale*x[j];
	y[j]=EMScale*y[j];
      }
      hits[nhits]=new TBox(x[j]-sE,y[j]-sE,x[j]+sE,y[j]+sE);
      hits[nhits]->SetFillColor(kBlue);
      nhits++;

      if(abs(emc_phi->at(j)-phiSlice)<DphiSlice) {
	float Remc = emc_r->at(j);
	float Zemc = emc_z->at(j);
	if(iframe == 1) {
	  Remc = EMScale*Remc;
	  Zemc = EMScale*Zemc;
	}
	hitsRZ[nhitsRZ]=new TBox(Zemc-sE,Remc-sE,Zemc+sE,Remc+sE);
	hitsRZ[nhitsRZ]->SetFillColor(kBlue);
	nhitsRZ++;
      }//if(!Is_Hot_EMC)
    }//for
  }

  // Create topoCluster hits
  int nTop=top_e->size();
  for(int itop=0;itop< nTop ;itop++){
    x[itop] = top_x->at(itop);
    y[itop] = top_y->at(itop);
    float sE = sqrt(s0*top_e->at(itop));

    cout <<"phi, r, Eemc, Eihc, Eohc = "<<top_phi->at(itop)<<" "<<top_r->at(itop)<<" "<<top_e->at(itop)<<" "<<emc_e->at(itop)<<" "<<ihc_e->at(itop)<<" "<<ohc_e->at(itop)<<endl;

    // rescale EMC hits if iframe==1
    if(iframe == 1) {
      x[itop]=EMScale*x[itop];
      y[itop]=EMScale*y[itop];
    }
    hits[nhits]=new TBox(x[itop]-sE,y[itop]-sE,x[itop]+sE,y[itop]+sE);
    hits[nhits]->SetFillColor(kRed);
    nhits++;

    if(abs(top_phi->at(itop)-phiSlice)<DphiSlice) {
      float Rtop = top_r->at(itop);
      float Ztop = top_z->at(itop);
      if(iframe == 1) {
	Rtop = EMScale*Rtop;
	Ztop = EMScale*Ztop;
      }
      hitsRZ[nhitsRZ]=new TBox(Ztop-sE,Rtop-sE,Ztop+sE,Rtop+sE);
      hitsRZ[nhitsRZ]->SetFillColor(kRed);
      nhitsRZ++;
    }//if(phiSlice)
  }//for itop;

  // Creates Silicon hits
  for(int jj=0;jj<nSiClust;jj++) {
    x[jj]=si_x->at(jj);
    y[jj]=si_y->at(jj);
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
    }
  }

  // Creates SiliconAll hits
  for(int jj=0;jj<nSiClustA;jj++) {
    x[jj]=a_si_x->at(jj);
    y[jj]=a_si_y->at(jj);
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
  }
  
  // Creates SiSeed tracklets (arrow)
  float Rtrk;
  const float Asize = 0.02;
  if(iframe==1) Rtrk=2.0; else Rtrk=80;
  if(iframe==1) Rmax=12.0; else Rmax=80;
  for(int k=0;k<ntrack;k++) {
    if(pt0->at(k)>pt0min) {
      //  cout<<k<<":"<<eta0->at(k)<<"  "<<phi0->at(k)<<"  "<<pt0->at(k)<<endl;
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
	// if iframe==1, also plot "scaled" RZ track
	if(iframe==1){
	  float Rmax2=80;
	  float Zend2 = (Rmax2-r0)/tan(theta)+vz0->at(k);
	  seedtrkRZ[ntrksRZ]=new TArrow(EMScale*(vz0->at(k)),
					EMScale*r0,
					EMScale*Zend2,
					EMScale*Rmax2,Asize);
	  seedtrkRZ[ntrksRZ]->SetLineStyle(2);
	  ntrksRZ++;
	}
      }
    }
  }

  /*
  // Create Orbit of EmcINTT tracklet if EmcINTT triplet exsits
  if(vEmcINTT.size()>0) {
    cout <<"EmcINTT tracklet found: # of tracklet = "<<vEmcINTT.size()<<endl;
    // clear previously existed track orbit;
    for(int io=0;io<vEmcINTTorbitL.size();io++) {
      vEmcINTTorbitL.at(io)->Delete();
    }
    vEmcINTTorbitL.clear();

    for(int io=0;io<vEmcINTTorbitG.size();io++) {
      vEmcINTTorbitG.at(io)->Delete();
    }
    vEmcINTTorbitG.clear();
    
    for(int i=0;i<vEmcINTT.size();i++) {
      TArc *orbitL = new TArc(vEmcINTT_xO.at(i),vEmcINTT_yO.at(i),
			     vEmcINTT_R.at(i),0.,360.);
      orbitL->SetFillStyle(0);
      orbitL->SetLineColor(kGreen);
      vEmcINTTorbitL.push_back(orbitL);

      TArc *orbitG = new TArc(vEmcINTT_xO_EI.at(i),vEmcINTT_yO_EI.at(i),
			     vEmcINTT_R_EI.at(i),0.,360.);
      orbitG->SetFillStyle(0);
      orbitG->SetLineColor(kBlue);
      vEmcINTTorbitG.push_back(orbitG);
    }
  }
  */

  // Draw X-Y view
  c1->cd();
  for(int kk=0;kk<nhits;kk++){
    hits[kk]->Draw();
  }
  for(int itk=0;itk<ntrks;itk++) {
    seedtrk[itk]->Draw();
  }
  // Draw phi slice region
  if(dphiArc != nullptr) dphiArc->Delete();
  float phimin=(phiSlice-DphiSlice)*180./3.141592;
  float phimax=(phiSlice+DphiSlice)*180./3.141592;
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

  
  /*
  // Draw track orbit
  for(int i=0;i<vEmcINTT.size();i++) {
    vEmcINTTorbitL.at(i)->Draw();
    vEmcINTTorbitG.at(i)->Draw();
  }
  */
  
  c1->Draw();
  c1->Modified();
  c1->Update();
  //  cout<<endl;

  //Draw RZ slice
  c2->cd();

  ZvtxMarker->Draw();
  for(int kk=0;kk<nhitsRZ;kk++){
    hitsRZ[kk]->Draw();
  }
  for(int itk=0;itk<ntrksRZ;itk++) {
    seedtrkRZ[itk]->Draw();
  }
  c2->Modified();
  c2->Update();
}

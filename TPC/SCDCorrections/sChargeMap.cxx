#include <iostream>
#include <fstream>
#include "sChargeMap.h"
#include "TMath.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TAxis.h"
#include "TCanvas.h"

//=================
sChargeMap::sChargeMap(int nr, float rmin, float rmax, int np, float pmin, float pmax, int nz, float hz, float ev, float iv) :
  fI(NULL),
  fE(NULL),
  fEcmperus(ev),
  fIcmperms(iv)
{
  fI = new TH3D("fI","fI;r;phi;z",nr,rmin,rmax,np,pmin,pmax,nz,-hz,hz);
  fE = new TH3D("fE","fE;r;phi;z",nr,rmin,rmax,np,pmin,pmax,nz,-hz,hz);
}
//=================
void sChargeMap::ScreenShot(char *name, char *ext, int n) {
  TCanvas *main = new TCanvas();
  main->Divide(2,2);
  TH2D *tmp = (TH2D*) fI->Project3D("xz");
  tmp->SetTitle(Form("IonMap (ev==%d)",n));
  main->cd(1)->SetLogz(1); tmp->Draw("colz");
  TH2D *tmp2 = (TH2D*) fE->Project3D("xz");
  tmp2->SetTitle(Form("ElectronMap (ev==%d)",n));
  main->cd(2)->SetLogz(1); tmp2->Draw("colz");
  TH1D *tmp1 = fI->ProjectionZ("IonMapZ");
  tmp1->SetTitle(Form("IonMapZ (ev==%d)",n));
  tmp1->Rebin(10);
  main->cd(3); tmp1->Draw("hist");
  TH1D *tmp12 = fE->ProjectionZ("ElectronMapZ");
  tmp12->SetTitle(Form("ElectronMapZ (ev==%d)",n));
  tmp12->Rebin(10);
  main->cd(4); tmp12->Draw("hist");
  main->SaveAs( Form("%s.%s",name,ext),ext);
  delete tmp;
  delete tmp1;
  delete tmp2;
  delete tmp12;
  delete main;
}
//=================
void sChargeMap::SaveIonMap(char *name) {
  fI->SaveAs( name );
}
//=================
void sChargeMap::SaveRho(char *name,int nrad,int nphi,int nhz) {
  int nr = fI->GetXaxis()->GetNbins();
  int np = fI->GetYaxis()->GetNbins();
  int nz = fI->GetZaxis()->GetNbins();
  nrad = TMath::Max( nr, nrad );
  nphi = TMath::Max( np, nphi );
  nhz  = TMath::Max( nz, nhz );
  TH3F *rho = new TH3F("rho","ChargeDensity [fC/cm^3];Radial [cm];Azimuthal [rad];Longitudinal [cm]",
		       nrad, fI->GetXaxis()->GetBinLowEdge(1),fI->GetXaxis()->GetBinLowEdge( nr+1 ),
		       nphi, 0,TMath::TwoPi(),
		       nz,   fI->GetZaxis()->GetBinLowEdge(1),fI->GetZaxis()->GetBinLowEdge( nz+1 ));
  for(int i=0; i!=nrad; ++i) {
    float rmin = rho->GetXaxis()->GetBinLowEdge(i+1);
    float rmax = rho->GetXaxis()->GetBinLowEdge(i+2);
    int brmin = fI->GetXaxis()->FindBin( rmin+1e-6 );
    int brmax = fI->GetXaxis()->FindBin( rmax-1e-6 );
    for(int j=0; j!=nphi; ++j) {
      for(int k=0; k!=nhz; ++k) {
	float zmin = rho->GetZaxis()->GetBinLowEdge(k+1);
	float zmax = rho->GetZaxis()->GetBinLowEdge(k+2);
	int bzmin = fI->GetZaxis()->FindBin( zmin+1e-6 );
	int bzmax = fI->GetZaxis()->FindBin( zmax-1e-6 );
	float n = 0;
	for(int ii=brmin; ii!=brmax+1; ++ii)
	  for(int jj=1; jj!=np+1; ++jj)
	    for(int kk=bzmin; kk!=bzmax+1; ++kk)
	      n += fI->GetBinContent( ii, jj, kk );
	float dv = TMath::Power(0.5*(rmax+rmin),2)*(rmax-rmin)*(zmax-zmin)*TMath::TwoPi()*(1./nphi);
	float drho = n / (dv*10000) * 1.602; // [fC/cm^3]
	//std::cout << "   n " << n << " || dv " << dv << std::endl;
	rho->SetBinContent( i+1, j+1, k+1, drho );
      }
    }
  }
  rho->SaveAs( name );
}
//=================
void sChargeMap::Fill(float r, float p, float z, float w) {
  int vd1 = fI->GetZaxis()->FindBin(double(0));
  int vd2 = fI->GetZaxis()->FindBin(z);
  if(vd1==vd2) return;
  if(w<0) fE->Fill(r,p,z,TMath::Abs(w));
  else fI->Fill(r,p,z,w);
}
//=================
void sChargeMap::Propagate(float ms) {
  int NZ = fI->GetZaxis()->GetNbins();
  //std::cout << " Ion Deltacm  " << fIcmperms*ms << std::endl;
  //std::cout << " Ion bin width  " << fI->GetZaxis()->GetBinWidth(1) << std::endl;
  int fIDis = fIcmperms*ms / fI->GetZaxis()->GetBinWidth(1);
  //std::cout << " DeltaBin  " << fIDis << std::endl;
  if(fIDis<1) {
    printf("Improve grid!!!!\n");
    fIDis=1;
  }
  // IONS: the easy part. Ions propagate to Anode plate (z==0) and disappear.
  // what i will do is to displace the bincontents according to the ion velocity
  // whenever the ions pass the Anode plate they will go to either under or overflow.
  int vd = fI->GetZaxis()->FindBin(double(0)); // anode
  // left-side
  for(int nz=vd-1; nz!=0; --nz) {
    int newz = nz+fIDis;
    if(newz>=vd) newz = 0; // to underflow
    for(int nr=0; nr!=fI->GetXaxis()->GetNbins(); ++nr)
      for(int np=0; np!=fI->GetYaxis()->GetNbins(); ++np) {
	float nc = fI->GetBinContent(nr+1,np+1,newz) + fI->GetBinContent(nr+1,np+1,nz);
	fI->SetBinContent(nr+1,np+1,nz, 0.0);
	fI->SetBinContent(nr+1,np+1,newz, nc );
      }
  }
  // right-side
  for(int nz=vd+1; nz!=NZ+1; ++nz) {
    int newz = nz-fIDis;
    if(newz<=vd) newz = NZ + 1; // to overflow
    for(int nr=0; nr!=fI->GetXaxis()->GetNbins(); ++nr)
      for(int np=0; np!=fI->GetYaxis()->GetNbins(); ++np) {
	float nc = fI->GetBinContent(nr+1,np+1,newz) + fI->GetBinContent(nr+1,np+1,nz);
	fI->SetBinContent(nr+1,np+1,nz,0.0);
	fI->SetBinContent(nr+1,np+1,newz, nc );
      }
  }
  //float fBF = 0.01; // percentage
  float fBF = 1; // percentage
  // ELECTRONS: though. Electrons will propagate according to the End plates (|z|==hz) and produce ions (backflow).
  // what i will do is to compute if the time passed is enough to reach the endplates in one go
  // if so, then the remaining is used to compute the position of ions backflowing and inject ions accordingly
  // if not, then the electrons are displaced normally

  // left-side
  for(int nz=1; nz!=vd; ++nz) {
    float dz = fE->GetZaxis()->GetBinCenter(nz) - fE->GetZaxis()->GetBinLowEdge(1);
    float t2ep = dz / fEcmperus;
    float msres = ms - t2ep/1000;
    //std::cout << " Electron msres  " << msres << std::endl;
    if(msres>0) {
      // make backflow ions appear
      int newz = 1+fIcmperms*msres / fI->GetZaxis()->GetBinWidth(1);
      //std::cout << " Ion where in bin  " << newz << std::endl;
      if(newz>=vd) newz = 0; // to overflow
      for(int nr=0; nr!=fI->GetXaxis()->GetNbins(); ++nr)
	for(int np=0; np!=fI->GetYaxis()->GetNbins(); ++np) {
	  float nc = fE->GetBinContent(nr+1,np+1,nz);
	  if(nc>0) {
	    nc = fI->GetBinContent(nr+1,np+1,newz) + fBF*fE->GetBinContent(nr+1,np+1,nz);
	    //std::cout << " BACKFLOW: from " << nz << " to " << newz << " w=" << fE->GetBinContent(nr+1,np+1,nz) << " ==> " << nc << std::endl;
	    fE->SetBinContent(nr+1,np+1,nz,0);
	    fI->SetBinContent(nr+1,np+1,newz, nc );
	  }
	}
    } else {
      // move electrons
      std::cout << " Electron Deltacm  " << fEcmperus*ms*1000 << std::endl;
      std::cout << " Electron bin width  " << fE->GetZaxis()->GetBinWidth(1) << std::endl;
      float fEDis = fEcmperus*ms*1000 / fE->GetZaxis()->GetBinWidth(1);
      std::cout << " DeltaBin  " << fEDis << std::endl;
      int newz = nz-fEDis;
      if(newz<1) newz = 0; // to underflow
      for(int nr=0; nr!=fI->GetXaxis()->GetNbins(); ++nr)
	for(int np=0; np!=fI->GetYaxis()->GetNbins(); ++np) {
	  float nc = fE->GetBinContent(nr+1,np+1,newz) + fE->GetBinContent(nr+1,np+1,nz);
	  fE->SetBinContent(nr+1,np+1,nz,0);
	  fE->SetBinContent(nr+1,np+1,newz, nc );
	}
    }
  }
  // right-side
  for(int nz=vd+1; nz!=NZ+1; ++nz) {
    float dz = fE->GetZaxis()->GetBinLowEdge(NZ+1) - fE->GetZaxis()->GetBinCenter(nz);
    float t2ep = dz / fEcmperus;
    float msres = ms - t2ep/1000;
    //std::cout << " Electron msres  " << msres << std::endl;
    if(msres>0) {
      // make backflow ions appear
      int newz = NZ - fIcmperms*msres / fI->GetZaxis()->GetBinWidth(1);
      //std::cout << " Ion where in bin  " << newz << std::endl;
      if(newz<=vd) newz = NZ+1; // to overflow
      for(int nr=0; nr!=fI->GetXaxis()->GetNbins(); ++nr)
	for(int np=0; np!=fI->GetYaxis()->GetNbins(); ++np) {
	  float nc = fE->GetBinContent(nr+1,np+1,nz);
	  if(nc>0) {
	    nc = fI->GetBinContent(nr+1,np+1,newz) + fBF*fE->GetBinContent(nr+1,np+1,nz);
	    //std::cout << " BACKFLOW: from " << nz << " to " << newz << " w=" << fE->GetBinContent(nr+1,np+1,nz) << " ==> " << nc << std::endl;
	    fE->SetBinContent(nr+1,np+1,nz,0);
	    //fI->SetBinContent(nr+1,np+1,newz, nc );
	  }
	}
    } else {
      // move electrons
      std::cout << " Electron Deltacm  " << fEcmperus*ms*1000 << std::endl;
      std::cout << " Electron bin width  " << fE->GetZaxis()->GetBinWidth(1) << std::endl;
      float fEDis = fEcmperus*ms*1000 / fE->GetZaxis()->GetBinWidth(1);
      std::cout << " DeltaBin  " << fEDis << std::endl;
      int newz = nz+fEDis;
      if(newz>NZ) newz = NZ+1; // to underflow
      for(int nr=0; nr!=fI->GetXaxis()->GetNbins(); ++nr)
	for(int np=0; np!=fI->GetYaxis()->GetNbins(); ++np) {
	  float nc = fE->GetBinContent(nr+1,np+1,newz) + fE->GetBinContent(nr+1,np+1,nz);
	  fE->SetBinContent(nr+1,np+1,nz,0);
	  fE->SetBinContent(nr+1,np+1,newz, nc );
	}
    }
  }
}

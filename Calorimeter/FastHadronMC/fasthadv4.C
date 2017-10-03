#include "fasthadv4.h"

/*
  First shot at a fast hadron simulation tool.  
  The "detector" is rectangular, nbin*nbin granularity, ddepth deep.
  Dimensions are arbitrary (make sense only in context with the
  electromagnetic and hadronic  shower width parameters dwidthem, dwidthhad.
  Impact point and theta, phi angles are arbitrary.
  Each hadron enters the detector as minimum ionizing; afetr each (fixed)
  MIP deposit throws a random number to decide whether to start a shower or not.
  Once it decided to form a shower, the remaining length in the detector is
  calculated, and serves as a weight for the energy it will deposit (the deposit
  in any single step is constant now, so one plays with the number of steps to change
  the total deposit).
  Also, the total energy it will actually deposited is weighted (inversely) with
  a formula, essentially with the log of the original hadron energy.  
  The total energy it will deposit is "dshe".
  Next, it will be decided how much of "dshe" will be electromagnetic and
  how much is hadronic ("reh").    Since the electromagnetic part usually starts
  later, a spatial delay is assigned to the electromagnetic shower.
  Finally the two showers (em, had) are generated in three dimensions,
  gradually widening with depth, but randomized with a Gaussian.
 */

// Impact point and angle
double dimpx,dimpy,dimpxtheta,dimpyphi;
double dpath,dfullpath,ddepth,dtote,dtotemip,dtoteem,dtotehad;
double x,y,z,xhad,yhad,zhad,reh;
double dshe,dhade,deme;
double ddzhad,ddzem;  // stepsize for had, em part (mip is fixed)
double dfudge;
double dcostheta,dcosphi,dsintheta,dsinphi;
double demdelay;


int nmipstep,nshstep;
int i,j;


int ntrysh,ntryem,ntryhad;


  TH2D *h2dhits = new TH2D("h2dhits","Hit pattern",nbin,xlo,xhi,nbin,ylo,yhi);
  TH2D *h2dlive = new TH2D("h2dlive","Live display",nbin,xlo,xhi,nbin,ylo,yhi);
  TH1D *htote = new TH1D("htote","Total energy",100,0.0,15.0);
  TH1D *htotemip = new TH1D("htotemip","Total energy",25,0.0,1.0);
  TH1D *htoteem = new TH1D("htoteem","Total energy",160,0.0,8.0);
  TH1D *htotehad = new TH1D("htotehad","Total energy",160,0.0,8.0);
  TH2D *h2dmipnonmip = new TH2D("h2dmipnonmip","MIP vs non MIP energy",
				80,0.0,8.0,50,0.0,0.5);
  TH3D *h3dlivemip =  new TH3D("h3dlivemip","3D deposit",
			      nbin,xlo,xhi,nbin,ylo,yhi,nbin,zlo,zhi);
  TH3D *h3dliveem =  new TH3D("h3dliveem","3D em deposit",
			      nbin,xlo,xhi,nbin,ylo,yhi,nbin,zlo,zhi);
  TH3D *h3dlivehad =  new TH3D("h3dlivehad","3D had deposit",
			      nbin,xlo,xhi,nbin,ylo,yhi,nbin,zlo,zhi);
  TH3D *h3dliveall =  new TH3D("h3dliveall","3D all deposit",
			      nbin,xlo,xhi,nbin,ylo,yhi,nbin,zlo,zhi);



void fasthadv4(double dimpx=0.5, double dimpy=0.5, 
	       double dimpxtheta=0.0, double dimpyphi=0.0)
{
  
  
  nmipstep = ndefmipstep;
  nshstep = ndefshstep;

  dcostheta =  TMath::Cos(dimpxtheta);
  dcosphi =  TMath::Cos(dimpyphi);
  dsintheta =  TMath::Sin(dimpxtheta);
  dsinphi =  TMath::Sin(dimpyphi);
  if(dcostheta<=0.0 || dcosphi<=0.0)
    {
      std::cout << "Something dead wrong with impact angle " << std::endl;
      return;
    }
  
  nmipstep = int(float(ndefmipstep)/(dcostheta*dcosphi));
  nshstep = int(float(ndefshstep)/(dcostheta*dcosphi));
  dfullpath = dlength/(dcostheta*dcosphi);
  std::cout << nmipstep << "    "  << nshstep << "   "  <<  dfullpath << std::endl;
  

		     
  dhade = dtothade / float(nshstep);
  deme = dtothade / float(nshstep);
  ddzem = dlength / float(ndefshstep);
  ddzhad = dlength / float(ndefshstep);

TCanvas  * c10 = new TCanvas("c10","Live display",1200,600);
  c10->Divide(2,1);
  
  
  for(i=1; i<=ntry;i++)
    {      

      dpath = ddepth = dtote = dtotemip = dtoteem = dtotehad = dshe = 0.0;      
      h3dlivemip->Reset();
      h3dliveem->Reset();
      h3dlivehad->Reset();
      h2dlive->Reset();

      for(j=1; j<=nmipstep; j++)
	{
	  dpath = j*ddzmip;
	  x = dimpx + dpath*dsintheta;
	  y = dimpy + dpath*dsintheta;
	  z = dpath * dcostheta*dcosphi;
	  dtotemip  +=ddedx;
	  h2dhits->Fill(x,y,ddedx);
	  //	  i%nlive == 0 ? h2dlive->Fill(x,y,ddedx) : ;	    
	  if(i%nlive == 0)
	    {
	      h2dlive->Fill(x,y,ddedx);
	      h3dlivemip->Fill(x,y,z);
	    }
	    
	  if(gRandom->Uniform()>dmipstop) break;
	}
      dtotemip *= 1.0 + dresol * gRandom->Gaus();      
      htotemip->Fill(dtotemip);
      xhad = x;  yhad = y;
      zhad = dpath * dcostheta*dcosphi;
      ddepth = dpath * dcostheta*dcosphi;
      if(ddepth>=dlength) 
	{
	  htote->Fill(dtotemip);
	  continue;  // done with this, generate the next particle
	}
      
      // Get the remaining energy
      dshe = dtothade - dtotemip;
      // Estimate how much of it will be deposited (function of dtothade)
      dshe *= TMath::Sqrt(1.0/(1.0+0.5*TMath::Log(dshe+1.0)));
      
      // Right now for simplicity the deposits per step are identical,
      // so you manipulate the number of steps to get the proper deposit      
      ntrysh = int(nshstep * dshe/dtothade);
      
      // Ratio of electromagnetic and hadronic part of the shower
      reh = 0.1 + 0.15*gRandom->Uniform();
      reh /=(1.0+0.6*TMath::Log(dshe+1.0));
      
      ntryem = int(ntrysh * reh);
      ntryhad = int(ntrysh * (1.0 - reh));
      ntryem = int(ntryem * ((dlength-ddepth) / dlength));
      ntryhad = int(ntryhad * ((dlength-ddepth) / dlength));

      // The em part of the hadron shower doesn't start immediately
      // it is 
      demdelay = 2.0 + gRandom->Gaus();
      demdelay > 0.0 ?  : demdelay = -demdelay;
      if(ntryem>0) ddzem = (dlength-ddepth)/float(ntryem);
      

      dpath = 0.0;
      for(j=1; j<ntryem; j++)
	{
	  dpath = j*ddzem + demdelay;
	  if((ddepth + dpath * dcostheta*dcosphi)>dlength)
	    {
	      htote->Fill(dtoteem);
	      break;
	    }
	  x = xhad + dpath*(dsintheta+dwidthem*gRandom->Gaus());
	  y = yhad + dpath*(dsinphi+dwidthem*gRandom->Gaus());
	  z = zhad+dpath * dcostheta*dcosphi;
	  dtoteem += deme;
	  h2dhits->Fill(x,y,deme);
	  if(i%nlive == 0)
	    {
	      h2dlive->Fill(x,y,deme);
	      h3dliveem->Fill(x,y,z);
	    }
	}
      dtoteem *= 1.0 + dresol * gRandom->Gaus();      
      htoteem->Fill(dtoteem);

      if(ntryhad>0) ddzhad = (dlength-ddepth)/float(ntryhad);
      
      
      dpath = 0.0;
      for(j=1; j<ntryhad; j++)
	{
	  dpath = j*ddzhad;
	  if((ddepth + dpath * dcostheta*dcosphi)>dlength)
	    {
	      htote->Fill(dtotehad);
	      break;
	    }
	  x = xhad + dpath*(dsintheta+dwidthhad*gRandom->Gaus());
	  y = yhad + dpath*(dsinphi+dwidthhad*gRandom->Gaus());
	  z = zhad+dpath * dcostheta*dcosphi;
	  dtotehad += dhade;
	  h2dhits->Fill(x,y,dhade);
	  if(i%nlive == 0)
	    {
	      h2dlive->Fill(x,y,dhade);
	      h3dlivehad->Fill(x,y,z);
	    }
	}

      dtotehad *= 1.0 + dresol * gRandom->Gaus();      
      htotehad->Fill(dtotehad);

      htote->Fill(dtotemip+dtoteem+dtotehad);
      h2dmipnonmip->Fill(dtoteem+dtotehad,dtotemip);

      if(i%nlive==0)
	{
	  c10->cd(1);
	  h2dlive->Draw("colz");
	  c10->cd(2);
	  h3dlivemip->SetMarkerStyle(20);
	  h3dlivemip->SetMarkerColor(1);
	  h3dlivemip->SetMarkerSize(0.2);
	  h3dlivemip->Draw();
	  h3dliveem->SetMarkerStyle(20);
	  h3dliveem->SetMarkerColor(2);
	  h3dliveem->SetMarkerSize(0.4);
	  h3dliveem->Draw("SAME");
	  h3dlivehad->SetMarkerStyle(20);
	  h3dlivehad->SetMarkerColor(3);
	  h3dlivehad->SetMarkerSize(0.5);
	  h3dlivehad->Draw("SAME");
	  c10->Update();
	  std::cout << i << std::endl;
	  //	  if(i==300) c10->Print("evt300.jpg","jpg");
	  
	  h3dlivemip->Reset();
	  h3dliveem->Reset();
	  h3dlivehad->Reset();
	  h2dlive->Reset();

 
	  
	}
      
    }
   
  
TFile  *fout = new TFile("fasthadv4_out.root","RECREATE");
  
  h2dhits->Write();
  htote->Write();
  htotemip->Write();
  htoteem->Write();
  htotehad->Write();
  h2dmipnonmip->Write();
  h2dlive->Write();
  h3dlivemip->Write();
  h3dliveem->Write();
  h3dlivehad->Write();
  h3dliveall->Write();
  

  
  fout->Close();
  
}









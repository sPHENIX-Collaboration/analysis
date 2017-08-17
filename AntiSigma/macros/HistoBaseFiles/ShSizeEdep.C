void ShSizePrep (const char *part="e")
{
  char infile[100];
  int rad[11] = {0,1,2,3,4,5,6,7,8,9,100};
  double xr[11];
  int pz[8] = {1,2,4,8,16,32,40,50};
  double x[8];
  char *calo[] = {"E","HO","HI","BH","MAG"};

  for (int i=0; i<8; i++)
    { 
      for (int j=0; j<11; j++)
	{
	  for (int k=0;k<5;k++)
	    {
	      x[i] = pz[i];
	      xr[j] = rad[j];
	      sprintf(infile, "%sSmallSH%dout.root",part, pz[i]);
	      char hname [100];
	      sprintf(hname, "%sSH%dRad%d%s",part,pz[i],rad[j],calo[k]);
	      TH1F *h1 = new TH1F(hname, hname, 1000, 0, (pz[i]+1));
	      TFile *f = TFile::Open(infile);
	      if (! f)
		{
		  cout << infile << " not found - bozo!!!" << endl;
		  gSystem->Exit(1);
		}
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("sz");
	      char radn[100];
	      sprintf(radn, "RAD==%d", rad[j]);
	      nt->Project(hname,calo[k],radn);
	      if (! nt)
		{
		  cout << "sz"  << " not found - bozo!!!" << endl;
		  gSystem->Exit(1);
		}
	      // char cal [100];
	      // sprintf(cal,"%s",calo);
	      /*if (calo == "HO")
		{
		nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
		}
		else if(calo == "HI")
		{ 
		nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
		} 
		else if(calo == "E")
		{ 
		nt->Project(hname,"(EA+ES)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
		}
		else if (calo == "MAG") 
		{
		nt->Project(hname,"(MAG)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
		}
		else if (calo == "BH")
		{
		nt->Project(hname,"(BH)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
		}
		else 
		{
		cout << "ERROR" << endl;
		}
	      */
	      f->Close();
	      h1->Draw();
	      char fname [100];
	      sprintf(fname, "%spercent.root",part);
	      TFile *fout = TFile::Open(fname,"UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	    }
	}
    }
}

void ShSizeHist(const char *part = "e")
{
  int rad[11] = {0,1,2,3,4,5,6,7,8,9,100};
  int pz[8] = {1,2,4,8,16,32,40,50};
  char *calo[] = {"E","HO","HI","BH","MAG"};
  double x[11];
  double y[11];
  double xer[11] = {0};
  double yer[11];
  char hname[100];
  char fname[100];
  sprintf(fname,"%spercent.root",part);
  char area[100];
  sprintf(area,"%s",part);
  TFile *fin = TFile::Open(fname);
  gROOT->cd();
  for (int i=0; i<8; i++)
    {
      for (int j=0; j<5; j++)
	{
	  for (int k=0; k<11;k++)
	    {
	      // TFile *fin = TFile::Open(fname);
	      x[k] = rad[k];
	      sprintf(hname,"%sSH%dRad%d%s",part,pz[i],rad[k],calo[j]);
	      TH1 *h1 = (TH1F *) fin->Get(hname);
	      if (! fin)
		{
		  cout << "DEATH BY STUPIDITY" << endl;
		}
	      cout << hname << endl;
	      y[k] = h1->GetMean(1);
	      yer[k] = h1->GetRMS(1);
	    }	 
	  gr = new TGraphErrors(10,x,y,xer,yer);
	  char gname[100];
	  sprintf(gname, "%s%d%sRADERR",part,pz[i],calo[j]);
	  gr->SetTitle(gname);
	  gr->SetName(gname);
	  gr->Draw("ALP");
	  gr->SetMarkerStyle(20);
	  gr->SetMarkerColor(2);
	  char fname[100];
	  sprintf(fname,"%sShEdepErr.root",part);
	  TFile *fout = TFile::Open(fname, "UPDATE");
	  gr->Write();
	  fout->Write();
	  fout->Close();
	  for (int m=0; m<11;m++)
	    {
	      y[m] = 0;
	      yer[m] = 0;
	      x[m] = 0;
	    }
	}
    }
 fin->Close();
}

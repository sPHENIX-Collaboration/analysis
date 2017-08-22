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
	      sprintf(infile, "/sphenix/user/pinkenbu/jade/ntuple/%s_eta0_%dGeV.root",part, pz[i]);
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
	      TNtuple *nt = (TNtuple *)f->Get("de");
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
	      sprintf(fname, "%scondor.root",part);
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

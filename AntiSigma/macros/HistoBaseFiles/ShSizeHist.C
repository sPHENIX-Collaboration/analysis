
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

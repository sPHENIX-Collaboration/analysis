void EdepPercentSmall (const char *calo="HO", const char *part="e")
{
  char infile[100];
  int pz[9] = {1,2,4,8,12,16,32,40,50};
  double x[9];

  for (int i=0; i<9; i++)
{
  //x[i] = pz[i];
    sprintf(infile, "%s%dout.root",part, pz[i]);
	      char hname [100];
	      sprintf(hname, "%s%s%dd",calo,part,pz[i]);
	      TH1F *h1 = new TH1F(hname, "", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      // char cal [100];
	      // sprintf(cal,"%s",calo);
	      if (calo == "HO")
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
	      f->Close();
	      h1->Draw();
	      char fname [100];
	      sprintf(fname, "%spercent.root",part);
	      TFile *fout = TFile::Open(fname,"UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << " " << pz[i] << endl;
	      //fout->Write();
	      fout->Close();
  }
}
void EdepSmall(const char *calo="HOe", const char *part = "e")
{
   int pz[9] = {1,2,4,8,12,16,32,40,50};
   double x[9];
   double y[9];
   double xer[9] = {0};
   double yer[9];
   char hname[100];
   char fname[100];
   sprintf(fname,"%spercent.root",part);
   char area[100];
   sprintf(area,"%s",calo);
  TFile *fin = TFile::Open(fname);
  gROOT->cd();
   for (int i=0; i<9; i++)
     {
       x[i] = pz[i];
       sprintf(hname,"%s%s%dd",calo,part,pz[i]);
TH1 *h1 = (TH1F *) fin->Get(hname);
     y[i] = h1->GetMean(1);
   yer[i] = h1->GetRMS(1);
     }
   gr = new TGraphErrors(9,x,y,xer,yer);
   gr->SetTitle(area);
   gr->Draw("ALP");
   gr->SetMarkerStyle(20);
   gr->SetMarkerColor(2);
   fin->Close();
}

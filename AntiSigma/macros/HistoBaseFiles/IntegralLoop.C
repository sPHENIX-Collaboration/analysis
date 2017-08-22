void EdepPercentSmall ()//const char *part="e")//,int nevent = 1000)
{
  char infile[100];
  int pz[9] = {1,2,4,8,12,16,32,40,50};
  double x[9];
  double means[9] = {0};
  double y[9];
  double mval = 0;
  int lastbincont = 0;
  bool lastbin = false;
  int var = 11;
  double intnum[1000]  = {0};
  double varnum[1000] = {0};
  int i = 0;

  // for (int i=0; i<9; i++)
  //{
  //x[i] = pz[i];
  //y[i] = means[i];
  sprintf(infile, "/sphenix/user/jpinkenburg/ShowerSize/ntuple/anti_neutron_eta0_50GeV.root");
  char hname [100];
  sprintf(hname, "anti_neutron50");
  TH1F *h1 = new TH1F(hname, hname, 8000, 0, 2);
  TFile *f = TFile::Open(infile);
  gROOT->cd();
  TNtuple *nt = (TNtuple *)f->Get("de");
  nt->Project(hname,"dtotal","(ID<=2)*edep","",100000);
  //h1->Draw();
  /*	       for (int j = 1000; j>0; j--)
	       {
	       if (lastbin == false)
	       {
	       if (h1->GetBinContent(j) == 0)
	       {
	       cout << j << endl;
	       }
	       else
	       {
	       lastbin = true;
	       lastbincont = (j/1000)*3.5;
	       }
	       }
	       else
	       {
	       j=0;
	       }
	       }
	       char revhname[100];
	       sprintf(revhname,"%s%dGeV",part,pz[i]);
	       
	       TH1F *h4 = new TH1F(revhname,revhname,10000,0,lastbincont);
	       nt->Project(revhname,"dtotal","(ID<=2)*edep","");
  */f->Close();
  h1->Draw();
	       
  while (var <= 8001)
    {
      intnum[i] = h1->Integral(1,var);
      varnum[i] = .005*(i+1);
      var+=11;
      i++;
    }
		  
  /* mval = h1->Integral();
  // cout << "mval " << mval << endl;
  cout << "mval_supposed_to_be " << h4->Integral() << endl;
  means[i] += mval;
  mval = 0;
  cout << "6" << endl;
  */
  char fname [100];
  sprintf(fname, "anti_neutron_ThinBins.root");
  TFile *fout = TFile::Open(fname,"UPDATE");
  h1->Write();
  fout->Write();
  // cout << h1->Integral() << " "  <<  hname << " " << pz[i] << endl;
  TH2 *h2 = new TH2F("meanvals","anti_neutron",1,0,4,1,0,50);
  gStyle->SetOptStat(0);
  TMarker *mean = new TMarker();
  mean->SetMarkerStyle(20);
  mean->SetMarkerColor(3);
  TCanvas *c2 = new TCanvas("c2","stupid graph",1);
  //double dtot[9] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9}; 
  TGraph *gr = new TGraph((i+1),varnum,intnum);
  gr->Draw("AC*");
  fout->Write();
  fout->Close();
  // }
  /* TFile *f1 = TFile::Open("anti_neutronShowerEdepPercent.root");
     gROOT->cd();
     TH2 *h2 = new TH2F("meanvals","anti_neutron",1,0,1,1,0,.01);
     gStyle->SetOptStat(0);
     TMarker *mean = new TMarker();
     mean->SetMarkerStyle(20);
     mean->SetMarkerColor(3);
     TMarker *test = new TMarker();
     test->SetMarkerStyle(20);
     test->SetMarkerColor(5);
     TCanvas *c2 = new TCanvas("c2","stupid graph",1);
     double dtot[9] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
     TGraph *gr = new TGraph(9,dtot,means);
     gr->Draw("AC*");
  
     h2->Draw();

     test->DrawMarker(0.5,0.005);
     mean->DrawMarker(0,0);
     double dtot[9] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
     double z[9];
     //  cout << "bflmv" << meanval[0] << endl;
     for (int i=0;i<9;i++)
     {
     //    sprintf(
     cout << "coming" << endl;
     z[i] = dtot[i];
     // mean->DrawMarker(0,0);
     // test->DrawMarker(dtot[i],.005);
     cout << "meanval"  << " " << dtot[i] << endl;
     cout << "means" << " " << means[i] << endl;
     // cout << means[i] << endl;
     }
     char fname [100];
     sprintf(fname, "%sShowerEdepPercent.root",part);
     TFile *fout = TFile::Open(fname,"UPDATE");
     h2->Draw();
     // h2->Write();
     // fout->Write();
     fout->Close();
     f1->Close();
  */
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
   sprintf(fname,"%sShowerEdepPercent.root",part);
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

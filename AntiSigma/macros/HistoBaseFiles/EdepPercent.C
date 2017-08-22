void EdepPercentE()//(const char * particle = "e-", const int pz = 4, const int thickness = 18)
{
  char infile[100];
  //char *particles[2] = {"pi-","e-"};
  int pz[10] = {1,2,4,8,12,16,32,40,50,90};
  //int th[3] = {5,10,18};
  int h1int[36];
  // for (int i=0; i<10;i++)
    //  {
  //  for (int j=0; j<5; j++)
  //	{
  //	  for (int k=0; k<3; k++)
  //	    {
	      sprintf(infile, "e1out.root");
	      char hname [100];
	      sprintf(hname, "e1d");
	      TH1F *h1 = new TH1F(hname, "e1d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e2out.root");
	      char hname [100];
	      sprintf(hname, "e2d");
	      TH1F *h1 = new TH1F(hname, "e2d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e4out.root");
	      char hname [100];
	      sprintf(hname, "e4d");
	      TH1F *h1 = new TH1F(hname, "e4d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e8out.root");
	      char hname [100];
	      sprintf(hname, "e8d");
	      TH1F *h1 = new TH1F(hname, "e8d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e12out.root");
	      char hname [100];
	      sprintf(hname, "e12d");
	      TH1F *h1 = new TH1F(hname, "e12d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e16out.root");
	      char hname [100];
	      sprintf(hname, "e16d");
	      TH1F *h1 = new TH1F(hname, "e16d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e32out.root");
	      char hname [100];
	      sprintf(hname, "e32d");
	      TH1F *h1 = new TH1F(hname, "e32d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e40out.root");
	      char hname [100];
	      sprintf(hname, "e40d");
	      TH1F *h1 = new TH1F(hname, "e40d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e50out.root");
	      char hname [100];
	      sprintf(hname, "e50d");
	      TH1F *h1 = new TH1F(hname, "e50d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      /*    sprintf(infile, "e90out.root");
	      char hname [100];
	      sprintf(hname, "e90d");
	      TH1F *h1 = new TH1F(hname, "e90d", 100, 0, 90);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      */




//	    }
	      //}
//   }
}








void EdepPercentAnalyzerE()
{  
  TFile *fin = TFile::Open("epercent.root");
  gROOT->cd();
  TH2 *h2 = new TH2F("htot","EMCAL",1,0,50,1,0,1);
  gStyle->SetOptStat(0);
  TMarker *spf = new TMarker();
  spf->SetMarkerStyle(20);
  spf->SetMarkerColor(4);
  TCanvas *c1 = new TCanvas();
  h2->Draw();
  
  char hname[100];
  sprintf(hname, "e1d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  spf->DrawMarker(1, h1->GetMean(1));
  
   char hname[100];
  sprintf(hname, "e2d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  spf->DrawMarker(2, h1->GetMean(1));
  
   char hname[100];
  sprintf(hname, "e4d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(4, h1->GetMean(1));
  
   char hname[100];
  sprintf(hname, "e8d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  // int meanval = h1->GetMean(1);
  spf->DrawMarker(8, h1->GetMean(1));
  
   char hname[100];
  sprintf(hname, "e12d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(12, h1->GetMean(1));
  
   char hname[100];
  sprintf(hname, "e16d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(16, h1->GetMean(1));
  
   char hname[100];
  sprintf(hname, "e32d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  // int meanval = h1->GetMean(1);
  spf->DrawMarker(32, h1->GetMean(1));
  
   char hname[100];
  sprintf(hname, "e40d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(40, h1->GetMean(1));
  
   char hname[100];
  sprintf(hname, "e50d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(50, h1->GetMean(1));
  
  /* char hname[100];
  sprintf(hname, "e90d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  int meanval = h1->GetMean();
  spf->DrawMarker(90, h1->GetMean(1));
  */
  fin->Close();

  
 


  /* //const int N = 50;
  //int count = 0;
  //int i1 = 1;
  double temp1, temp2, temp3;
  //double arr1[N];
  //double arr2[N];
  //double arr3[N];
char filename1[350], filename2[350], filename3[350];
  // open the files
// while (i1<50) {
    temp1 = 0.; temp2 = 0.; temp3 = 0.;
    sprintf(filename1,"e8out.root");
    TFile* f1 = new TFile(filename1,"READONLY");
    if (!f1 || f1->IsZombie()) {
       printf("Cannot open %s!", filename1);
       return;
    }
    //f1->cd("folder1");
    TH1D* h1 = (TH1D*)gDirectory->Get("histo");
    if (!h1) {
       printf("Cannot read histo!");
       return;
    }
    h1->GetXaxis()->SetRangeUser(0.45,2);
    temp1 = h1->Integral();
    // h1->Delete(); - NO, use delete h1, but histograms are owned by files,
    // so just don't delete it, will be done by delete f1.
    sprintf(filename2,"/sphenix/user/jpinkenburg/sPHENIX/analysis/AntiSigma/macros/e4out.root",1);
    TFile* f2 = new TFile(filename2,"READONLY");
    if (!f2 || f2->IsZombie()) {
       printf("Cannot open %s!", filename2);
       return;
    }
    //f2->cd("folder1");
    TH1D* h2 = (TH1D*)gDirectory->Get("histo");
    h2->GetXaxis()->SetRangeUser(0.45,2);
    temp2 = h2->Integral();
    // h2->Delete(); - NO, see above
    arr1[count] = temp1;
    arr2[count] = temp2;
    arr3[count] = temp3;
    cout << temp1 << " " << temp2 << endl;
    i1 += 5;
count += 1;
 
delete f1;
delete f2;
  */
 }


void EdepPercentIH()//(const char * particle = "e-", const int pz = 4, const int thickness = 18)
{
  char infile[100];
  //char *particles[2] = {"pi-","e-"};
  int pz[10] = {1,2,4,8,12,16,32,40,50,90};
  //int th[3] = {5,10,18};
  int h1int[36];
  // for (int i=0; i<10;i++)
    //  {
  //  for (int j=0; j<5; j++)
  //	{
  //	  for (int k=0; k<3; k++)
  //	    {
	      sprintf(infile, "e1out.root");
	      char hname [100];
	      sprintf(hname, "HIe1d");
	      TH1F *h1 = new TH1F(hname, "HIe1d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e2out.root");
	      char hname [100];
	      sprintf(hname, "HIe2d");
	      TH1F *h1 = new TH1F(hname, "HIe2d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e4out.root");
	      char hname [100];
	      sprintf(hname, "HIe4d");
	      TH1F *h1 = new TH1F(hname, "HIe4d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e8out.root");
	      char hname [100];
	      sprintf(hname, "HIe8d");
	      TH1F *h1 = new TH1F(hname, "HIe8d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e12out.root");
	      char hname [100];
	      sprintf(hname, "HIe12d");
	      TH1F *h1 = new TH1F(hname, "HIe12d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e16out.root");
	      char hname [100];
	      sprintf(hname, "HIe16d");
	      TH1F *h1 = new TH1F(hname, "HIe16d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e32out.root");
	      char hname [100];
	      sprintf(hname, "HIe32d");
	      TH1F *h1 = new TH1F(hname, "HIe32d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e40out.root");
	      char hname [100];
	      sprintf(hname, "HIe40d");
	      TH1F *h1 = new TH1F(hname, "HIe40d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e50out.root");
	      char hname [100];
	      sprintf(hname, "HIe50d");
	      TH1F *h1 = new TH1F(hname, "HIe50d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HIA+HIS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      /*    sprintf(infile, "e90out.root");
	      char hname [100];
	      sprintf(hname, "e90d");
	      TH1F *h1 = new TH1F(hname, "e90d", 100, 0, 90);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      */




//	    }
	      //}
//   }
}








void EdepPercentAnalyzerIH()
{  
  TFile *fin = TFile::Open("epercent.root");
  gROOT->cd();
  TH2 *h2 = new TH2F("htot","Inner_HCAL",1,0,50,1,0,.1);
  gStyle->SetOptStat(0);
  TMarker *spf = new TMarker();
  spf->SetMarkerStyle(20);
  spf->SetMarkerColor(8);
  TLine *rlin = new TLine();
  rlin->SetLineColor(8);
  rlin->SetLineStyle(1);
  rlin->SetLineWidth(1);
  TCanvas *c2 = new TCanvas();
  h2->Draw();
  
  char hname[100];
  sprintf(hname, "HIe1d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  spf->DrawMarker(1, h1->GetMean(1));
      rlin->DrawLine(1,(h1->GetMean(1)-h1->GetRMS(1)),1,(h1->GetMean(1) + h1->GetRMS(1)));
   char hname[100];
  sprintf(hname, "HIe2d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  spf->DrawMarker(2, h1->GetMean(1));
      rlin->DrawLine(2,(h1->GetMean(1)-h1->GetRMS(1)),2,(h1->GetMean(1) + h1->GetRMS(1)));
   char hname[100];
  sprintf(hname, "HIe4d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(4, h1->GetMean(1));
      rlin->DrawLine(4,(h1->GetMean(1)-h1->GetRMS(1)),4,(h1->GetMean(1) + h1->GetRMS(1)));
   char hname[100];
  sprintf(hname, "HIe8d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  // int meanval = h1->GetMean(1);
  spf->DrawMarker(8, h1->GetMean(1));
      rlin->DrawLine(8,(h1->GetMean(1)-h1->GetRMS(1)),8,(h1->GetMean(1) + h1->GetRMS(1)));
   char hname[100];
  sprintf(hname, "HIe12d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(12, h1->GetMean(1));
     rlin->DrawLine(12,(h1->GetMean(1)-h1->GetRMS(1)),12,(h1->GetMean(1) + h1->GetRMS(1)));
   char hname[100];
  sprintf(hname, "HIe16d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(16, h1->GetMean(1));
    rlin->DrawLine(16,(h1->GetMean(1)-h1->GetRMS(1)),16,(h1->GetMean(1) + h1->GetRMS(1)));

   char hname[100];
  sprintf(hname, "HIe32d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  // int meanval = h1->GetMean(1);
  spf->DrawMarker(32, h1->GetMean(1));
    rlin->DrawLine(32,(h1->GetMean(1)-h1->GetRMS(1)),32,(h1->GetMean(1) + h1->GetRMS(1)));
   char hname[100];
  sprintf(hname, "HIe40d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(40, h1->GetMean(1));
  rlin->DrawLine(40,(h1->GetMean(1)-h1->GetRMS(1)),40,(h1->GetMean(1) + h1->GetRMS(1)));
  
   char hname[100];
  sprintf(hname, "HIe50d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(50, h1->GetMean(1));
   rlin->DrawLine(50,(h1->GetMean(1)-h1->GetRMS(1)),50,(h1->GetMean(1) + h1->GetRMS(1)));
  
  /* char hname[100];
  sprintf(hname, "e90d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  int meanval = h1->GetMean();
  spf->DrawMarker(90, h1->GetMean(1));
  */
  fin->Close();

  
 


  /* //const int N = 50;
  //int count = 0;
  //int i1 = 1;
  double temp1, temp2, temp3;
  //double arr1[N];
  //double arr2[N];
  //double arr3[N];
char filename1[350], filename2[350], filename3[350];
  // open the files
// while (i1<50) {
    temp1 = 0.; temp2 = 0.; temp3 = 0.;
    sprintf(filename1,"e8out.root");
    TFile* f1 = new TFile(filename1,"READONLY");
    if (!f1 || f1->IsZombie()) {
       printf("Cannot open %s!", filename1);
       return;
    }
    //f1->cd("folder1");
    TH1D* h1 = (TH1D*)gDirectory->Get("histo");
    if (!h1) {
       printf("Cannot read histo!");
       return;
    }
    h1->GetXaxis()->SetRangeUser(0.45,2);
    temp1 = h1->Integral();
    // h1->Delete(); - NO, use delete h1, but histograms are owned by files,
    // so just don't delete it, will be done by delete f1.
    sprintf(filename2,"/sphenix/user/jpinkenburg/sPHENIX/analysis/AntiSigma/macros/e4out.root",1);
    TFile* f2 = new TFile(filename2,"READONLY");
    if (!f2 || f2->IsZombie()) {
       printf("Cannot open %s!", filename2);
       return;
    }
    //f2->cd("folder1");
    TH1D* h2 = (TH1D*)gDirectory->Get("histo");
    h2->GetXaxis()->SetRangeUser(0.45,2);
    temp2 = h2->Integral();
    // h2->Delete(); - NO, see above
    arr1[count] = temp1;
    arr2[count] = temp2;
    arr3[count] = temp3;
    cout << temp1 << " " << temp2 << endl;
    i1 += 5;
count += 1;
 
delete f1;
delete f2;
  */
 }

void EdepPercentOH()//(const char * particle = "e-", const int pz = 4, const int thickness = 18)
{
  char infile[100];
  //char *particles[2] = {"pi-","e-"};
  int pz[10] = {1,2,4,8,12,16,32,40,50,90};
  //int th[3] = {5,10,18};
  int h1int[36];
  // for (int i=0; i<10;i++)
    //  {
  //  for (int j=0; j<5; j++)
  //	{
  //	  for (int k=0; k<3; k++)
  //	    {
	      sprintf(infile, "e1out.root");
	      char hname [100];
	      sprintf(hname, "HOe1d");
	      TH1F *h1 = new TH1F(hname, "HOe1d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e2out.root");
	      char hname [100];
	      sprintf(hname, "HOe2d");
	      TH1F *h1 = new TH1F(hname, "HOe2d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e4out.root");
	      char hname [100];
	      sprintf(hname, "HOe4d");
	      TH1F *h1 = new TH1F(hname, "HOe4d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e8out.root");
	      char hname [100];
	      sprintf(hname, "HOe8d");
	      TH1F *h1 = new TH1F(hname, "HOe8d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e12out.root");
	      char hname [100];
	      sprintf(hname, "HOe12d");
	      TH1F *h1 = new TH1F(hname, "HOe12d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e16out.root");
	      char hname [100];
	      sprintf(hname, "HOe16d");
	      TH1F *h1 = new TH1F(hname, "HOe16d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e32out.root");
	      char hname [100];
	      sprintf(hname, "HOe32d");
	      TH1F *h1 = new TH1F(hname, "HOe32d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e40out.root");
	      char hname [100];
	      sprintf(hname, "HOe40d");
	      TH1F *h1 = new TH1F(hname, "HOe40d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      sprintf(infile, "e50out.root");
	      char hname [100];
	      sprintf(hname, "HOe50d");
	      TH1F *h1 = new TH1F(hname, "HOe50d", 100, 0, 1);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(HOA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      
	      /*    sprintf(infile, "e90out.root");
	      char hname [100];
	      sprintf(hname, "e90d");
	      TH1F *h1 = new TH1F(hname, "e90d", 100, 0, 90);
	      TFile *f = TFile::Open(infile);
	      gROOT->cd();
	      TNtuple *nt = (TNtuple *)f->Get("ed");
	      nt->Project(hname,"(EA+ES)/(EA+ES+HOA+HOS+HIA+HIS+BH+MAG)");
	      f->Close();
	      h1->Draw();
	      TFile *fout = TFile::Open("epercent.root","UPDATE");
	      h1->Write();
	      fout->Write();
	      cout << h1->Integral() << " "  <<  hname << endl;
	      //fout->Write();
	      fout->Close();
	      */




//	    }
	      //}
//   }
}





void EdepPercentAnalyzerOH(const char *calo="HOe")
{  
  TFile *fin = TFile::Open("epercent.root");
  gROOT->cd();
  TH2 *h2 = new TH2F("htot","Outer_HCAL",1,0,50,1,0,.1);
  gStyle->SetOptStat(0);
  TMarker *spf = new TMarker();
  spf->SetMarkerStyle(20);
  spf->SetMarkerColor(11);
  TLine *rlin = new TLine();
  rlin->SetLineColor(3);
  rlin->SetLineStyle(1);
  rlin->SetLineWidth(1);
  TCanvas *c3 = new TCanvas();
   h2->Draw();
  
  char hname[100];
  sprintf(hname, "HOe1d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  spf->DrawMarker(1, h1->GetMean(1));
   rlin->DrawLine(1,(h1->GetMean(1)-h1->GetRMS(1)),1,(h1->GetMean(1) + h1->GetRMS(1)));
   double hom1 = h1->GetMean(1);
   double hor1 = h1->GetRMS(1);
  
   char hname[100];
  sprintf(hname, "HOe2d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  spf->DrawMarker(2, h1->GetMean(1));
   rlin->DrawLine(2,(h1->GetMean(1)-h1->GetRMS(1)),2,(h1->GetMean(1) + h1->GetRMS(1)));
   double hom2 = h1->GetMean(1);
   double hor2 = h1->GetRMS(1);

   char hname[100];
  sprintf(hname, "HOe4d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(4, h1->GetMean(1));
  rlin->DrawLine(4,(h1->GetMean(1)-h1->GetRMS(1)),4,(h1->GetMean(1) + h1->GetRMS(1)));
     double hom4 = h1->GetMean(1);
   double hor4 = h1->GetRMS(1);

 
   char hname[100];
  sprintf(hname, "HOe8d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  // int meanval = h1->GetMean(1);
  spf->DrawMarker(8, h1->GetMean(1));
  rlin->DrawLine(8,(h1->GetMean(1)-h1->GetRMS(1)),8,(h1->GetMean(1) + h1->GetRMS(1)));
     double hom8 = h1->GetMean(1);
   double hor8 = h1->GetRMS(1);

 
   char hname[100];
  sprintf(hname, "HOe12d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(12, h1->GetMean(1));
  rlin->DrawLine(12,(h1->GetMean(1)-h1->GetRMS(1)),12,(h1->GetMean(1) + h1->GetRMS(1)));
     double hom12 = h1->GetMean(1);
   double hor12 = h1->GetRMS(1);

 
   char hname[100];
  sprintf(hname, "HOe16d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(16, h1->GetMean(1));
  rlin->DrawLine(16,(h1->GetMean(1)-h1->GetRMS(1)),16,(h1->GetMean(1) + h1->GetRMS(1)));
     double hom16 = h1->GetMean(1);
   double hor16 = h1->GetRMS(1);

 
   char hname[100];
  sprintf(hname, "HOe32d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  // int meanval = h1->GetMean(1);
  spf->DrawMarker(32, h1->GetMean(1));
  rlin->DrawLine(32,(h1->GetMean(1)-h1->GetRMS(1)),32,(h1->GetMean(1) + h1->GetRMS(1)));
     double hom32 = h1->GetMean(1);
   double hor32 = h1->GetRMS(1);

 
   char hname[100];
  sprintf(hname, "HOe40d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(40, h1->GetMean(1));
  rlin->DrawLine(40,(h1->GetMean(1)-h1->GetRMS(1)),40,(h1->GetMean(1) + h1->GetRMS(1)));
     double hom40 = h1->GetMean(1);
   double hor40 = h1->GetRMS(1);

 
   char hname[100];
   sprintf(hname, "%s50d",calo);
  TH1 *h1 = (TH1F *) fin->Get(hname);
  //int meanval = h1->GetMean(1);
  spf->DrawMarker(50, h1->GetMean(1));
  rlin->DrawLine(50,(h1->GetMean(1)-h1->GetRMS(1)),50,(h1->GetMean(1) + h1->GetRMS(1)));
     double hom50 = h1->GetMean(1);
   double hor50 = h1->GetRMS(1);


   double x[9] = {1,2,4,8,12,16,32,40,50};
   double y[9] = {hom1,hom2,hom4,hom8,hom12,hom16,hom32,hom40,hom50};
   double xer[9] = {0};
   double yer[9] = {hor1,hor2,hor4,hor8,hor12,hor16,hor32,hor40,hor50};
   gr = new TGraphErrors(9,x,y,xer,yer);
   gr->Draw("ALP");
   gr->SetMarkerStyle(20);
   gr->SetMarkerColor(2);
 
  /* char hname[100];
  sprintf(hname, "e90d");
  TH1 *h1 = (TH1F *) fin->Get(hname);
  int meanval = h1->GetMean();
  spf->DrawMarker(90, h1->GetMean(1));
  */
  fin->Close();

  
 


  /* //const int N = 50;
  //int count = 0;
  //int i1 = 1;
  double temp1, temp2, temp3;
  //double arr1[N];
  //double arr2[N];
  //double arr3[N];
char filename1[350], filename2[350], filename3[350];
  // open the files
// while (i1<50) {
    temp1 = 0.; temp2 = 0.; temp3 = 0.;
    sprintf(filename1,"e8out.root");
    TFile* f1 = new TFile(filename1,"READONLY");
    if (!f1 || f1->IsZombie()) {
       printf("Cannot open %s!", filename1);
       return;
    }
    //f1->cd("folder1");
    TH1D* h1 = (TH1D*)gDirectory->Get("histo");
    if (!h1) {
       printf("Cannot read histo!");
       return;
    }
    h1->GetXaxis()->SetRangeUser(0.45,2);
    temp1 = h1->Integral();
    // h1->Delete(); - NO, use delete h1, but histograms are owned by files,
    // so just don't delete it, will be done by delete f1.
    sprintf(filename2,"/sphenix/user/jpinkenburg/sPHENIX/analysis/AntiSigma/macros/e4out.root",1);
    TFile* f2 = new TFile(filename2,"READONLY");
    if (!f2 || f2->IsZombie()) {
       printf("Cannot open %s!", filename2);
       return;
    }
    //f2->cd("folder1");
    TH1D* h2 = (TH1D*)gDirectory->Get("histo");
    h2->GetXaxis()->SetRangeUser(0.45,2);
    temp2 = h2->Integral();
    // h2->Delete(); - NO, see above
    arr1[count] = temp1;
    arr2[count] = temp2;
    arr3[count] = temp3;
    cout << temp1 << " " << temp2 << endl;
    i1 += 5;
count += 1;
 
delete f1;
delete f2;
  */
 }



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
	      nt->Project(hname,"(EA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
		}
	      else if(calo == "HI")
		{
	      nt->Project(hname,"(EA+HOS)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
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
	  nt->Project(hname,"(MAG)/(EA+ES+HIA+HIS+HOA+HOS+BH+MAG)");
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








void EdepSmall(const char *calo="HOe")
{
   int pz[9] = {1,2,4,8,12,16,32,40,50};
   double x[9];
   double y[9];
   double xer[9] = {0};
   double yer[9];
   char hname[100];
  TFile *fin = TFile::Open("epercent.root");
  gROOT->cd();
   for (int i=0; i<9; i++)
     {
       x[i] = pz[i];
       sprintf(hname,"%s%dd",calo,pz[i]);
TH1 *h1 = (TH1F *) fin->Get(hname);
     y[i] = h1->GetMean(1);
   yer[i] = h1->GetRMS(1);
     }
   gr = new TGraphErrors(9,x,y,xer,yer);
   gr->Draw("ALP");
   gr->SetMarkerStyle(20);
   gr->SetMarkerColor(2);
   fin->Close();
}


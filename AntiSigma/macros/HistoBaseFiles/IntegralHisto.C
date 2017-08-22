void EdepPercentSmall (/*TH1 *h1*/)//const char *part="e")//,int nevent = 1000)
{
  cout << "TEST VII" << endl; 
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
  char *part[] = {"gamma","neutron","anti_neutron"};
  int i = 0;
  int w =4;
  TCanvas *c3 = new TCanvas("TresGraphitos","stupid graph",1);
  TH1F *h4 = new TH1F("histo","histo",1000,0,.5);
  //TH2F *h3 = new TH2F("HistoGraph","HistoGraph",1000,0,.55,1000,0,1.2);
  TMarker *grmark = new TMarker();
  //	  TCanvas *c19 = new TCanvas();
  // h3->Draw();
  for (int q=0; q<3; q++) 
    {
      for (int w=0; w<9;w++)
      	{
	  cout << "TEST VI" << endl;
	  char ffile[100];
	  sprintf(ffile,"%s_ThinBins.root", part[q]);
	  TFile *f = TFile::Open(ffile);
	  char hname[100];
	  sprintf(hname,"%s%dd",part[q],pz[w]);
	  TH1F *h1 = (TH1F *)f->Get(hname);
	  var = 11;
	  i = 0;
	  while (var <= 8001)
	    {

	      if (i<1000)
		{
		  varnum[i] = .005*(i);
		  intnum[i] = (h1->Integral(1,var))/(h1->Integral());
		  i++;
		}
	      var+=10;
	    }	
	  varnum[0] = 0;
	  intnum[0] = 0;
	  char fname [100];
	  sprintf(fname, "PosterHistos.root");
	  TFile *fout = TFile::Open(fname,"UPDATE");
	  h1->Write();
	  fout->Write();
	  f->Close();
	  // cout << h1->Integral() << " "  <<  hname << " " << pz[i] << endl;
	  TH2 *h2 = new TH2F("meanvals","anti_neutron",1,0,5,1,0,1.2);
	  gStyle->SetOptStat(0);

	  
	  TMarker *mean = new TMarker();
	  mean->SetMarkerStyle(20);
	  mean->SetMarkerColor(3);
	  char canvname[100];
	  sprintf(canvname,"%s%d",part[q],pz[w]);
	  //TCanvas *c3 = new TCanvas(canvname,"stupid graph",1);
	  //double dtot[9] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9}; 
	  cout << "TEST V" << endl;
	  TGraph *gr = new TGraph((i-2),varnum,intnum);
          char gtitle[100];
	  sprintf(gtitle,"%s%dGeV;ConeSize;Percentage of Energy Deposited",part[q],pz[w]);
	  char gname[100];
	  sprintf(gname,"%s%dGeV",part[q],pz[w]);
	  cout << intnum[50] << " " << varnum[50] << endl;
	  gr->SetTitle(gtitle);
	  gr->SetName(gname);
	  gr->SetMarkerStyle(20);
	  if (part[q] == "anti_neutron")
	    {
	      gr->SetMarkerColor(4);
	    }
	  else if (part[q] == "neutron")
	    {
	      gr->SetMarkerColor(2);
	    }
	  else if (part[q] == "gamma")
	    {
	      gr->SetMarkerColor(3);
	    }
	  else 
	    {
	      cout << "idiot" << endl;
	    }
	  gr->SetMaximum(1.2);
	  h2->Draw();
	  gr->Draw("A*");
	  //gr->Draw("ALP")
	  //c3->DrawFrame(0,0,0.5,1.2);
	  // gr->PaintGrapHist((i-1),varnum,intnum,"chopt");
	  //	  gr->SetHistogram(h4);
	  h4->Write();
	  gr->Write();
	  cout << "TEST 1" << endl;
	  //  TH2F *h3 = (TH2F *) "HistoGraph";
	  cout << "TEST II" << endl;

	  cout << "TEST III" << endl;
	  //h3->Write();
	  cout << "TEST IV" << endl;
	  //double x;
	  //double y;
	  //gr()->GetPoint(

	  fout->Write();
	  fout->Close();
	  //	  gr->Close();
	  for (int a=0;a<1000;a++)
	    {
	      intnum[a] = 0;
	      varnum[a] = 0;
	    }
	  
	}
    }
  for (int j=0;j<9;j++)
    {
      cout << "TEST VIII" << endl;
      TFile *fhist = TFile::Open("PosterHistos.root");
      cout << "TEST IX" << endl;
      char anhtotname[300];
      sprintf(anhtotname,"anti_neutron%dGeV;ConeSize;Percentage of Energy Deposited",pz[j]);
      auto h32 = (TH2F*)fhist->Get(anhtotname);
      char nhtotname[300];
       sprintf(nhtotname,"neutron%dGeV;ConeSize;Percentage of Energy Deposited",pz[j]);
      auto h12 = (TH2F*)fhist->Get(nhtotname);
      cout << "TEST X" << endl;
      char ghtotname[300];
      sprintf(ghtotname,"gamma%dGeV;ConeSize;Percentage of Energy Deposited",pz[j]);
      auto h22 = (TH2F*)fhist->Get(ghtotname);
      cout << "TEST XI" << endl;
      TCanvas *c12 = new TCanvas();
      cout << "TEST XII" << endl;
      h32->Draw();
      cout << "TEST XIII" << endl;
      h12->Draw("PMC same");
      h22->Draw("PMC same");
      cout <<"TEST XIV" << endl;
      fhist->Close();
      cout << "DONE" << endl;
    }
  TCanvas *c123 = new TCanvas();
  TFile *ff = TFile::Open("PosterHistos.root");
  auto h101 = (TH2F*)ff->Get("anti_neutron1GeV;ConeSize;Percentage of Energy Deposited");
  h101->Draw();
  ff->Close();
  for (int o=0;o<8;o++)
    { 
      TFile *f_all = TFile::Open("PosterHistos.root");
      char aname[200];
      sprintf(aname,"anti_neutron%dGeV;ConeSize;Percentage of Energy Deposited",pz[(o+1)]);
      auto h100 = (TH2F*)f_all->Get(aname);
      h100->Draw("PMC same");
      f_all->Close();
    }
}


void ThreeGraphs (/*TH1 *h1*/)//const char *part="e")//,int nevent = 1000)
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
  char *part[] = {"gamma","neutron","anti_neutron"};
   int i = 0;	       
TGraph *gr = new TGraph();
 
  for (int q=0; q<3; q++) 
    {
      //          for (int w=0; w<9;w++)
      //   	{
  	  char ffile[100];
	  sprintf(ffile,"%s_ThinBins.root", part[q]);
	  TFile *f = TFile::Open(ffile);
	  char hname[100];
	  sprintf(hname,"%s12d",part[q]/*,pz[w]*/);
	  TH1F *h1 = (TH1F *)f->Get(hname);
	  var = 11;
	  i = 0;
	  while (var <= 8001)
	    {

	      if (i<100)
		{
		  varnum[i] = .005*(i+1);
		  intnum[i] = (h1->Integral(1,var))/(h1->Integral());
		}
	      var+=10;
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
	  sprintf(fname, "PosterHistos.root");
y	  TFile *fout = TFile::Open(fname,"UPDATE");
	  h1->Write();
	  //fout->Write();
	  f->Close();
	  // cout << h1->Integral() << " "  <<  hname << " " << pz[i] << endl;
	  //char canvname[100];
	  // sprintf(canvname,"%s12",part[q]/*,pz[w]*/);
	  
	  //	  TCanvas *c3 = new TCanvas("3Graphs","stupid graph",1);
	  TH2 *h2 = new TH2F("meanvals","anti_neutron",1,0,4,1,0,50);
	  gStyle->SetOptStat(0);
	  TMarker *mean = new TMarker();
	  mean->SetMarkerStyle(20);
	  mean->SetMarkerColor(3);
  //double dtot[9] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9}; 
	  // TGraph *gr = TGraph((i-1),varnum,intnum);
	  gr->TGraph((i-1),varnum,intnum);
          char gname[100];
	  sprintf(gname,"12GeV;xtitle;ytitle"/*,part[q]/*,pz[w]*/);
	  gr->SetTitle(gname);
	  gr->SetName(gname);
	  gr->Draw("A*");
	  //	  gr->Write();
	  //	  c3->Update();
	  //c3->Modified();
	  //	  fout->Write();
	  //	  fout->Close();
	  //	  gr->Close();
	  for (int a=0;a<1000;a++)
	    {
	      intnum[a] = 0;
	      varnum[a] = 0;
	    }
  // }
	  //	}
    }
  fout->Write();
  fout->Close();
}





void StupidGraphs (/*TH1 *h1*/)//const char *part="e")//,int nevent = 1000)
{
  TFile *fin = TFile::Open("PosterHistos.root");
  gROOT->cd();
  TH2 *h2 = new TH2F("DumbGraph","TresLines",1,0,10,1,0,50);
  gStyle->SetOptStat(0);
  TMarker *spf = new TMarker();
  spf->SetMarkerStyle(20);
  spf->SetMarkerColor(4);
  TCanvas *c1 = new TCanvas();
  h2->Draw();
  double x;
  double y;
  
  char hname[100];
  sprintf(hname, "meanvals");
  TH2 *gr = (TH2F *) fin->Get(hname);
  cout << "herro" << endl;
  cout << gr()->GetPoint(1,x,y) << endl;
//spf->DrawMarker();
	  
}



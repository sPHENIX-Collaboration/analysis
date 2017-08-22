void Neutrals (/*TH1 *h1*/)//const char *part="e")//,int nevent = 1000)
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
  //TCanvas *c3 = new TCanvas("TresGraphitos","stupid graph",1);
  // TH1F *h4 = new TH1F("histo","histo",1000,0,.5);
  //TH2F *h3 = new TH2F("HistoGraph","HistoGraph",1000,0,.55,1000,0,1.2);
  // TMarker *grmark = new TMarker();
  //	  TCanvas *c19 = new TCanvas();
  // h3->Draw();
  for (int q=0; q<3; q++) 
  {
    for (int w=0; w<9;w++)
    {
      cout << "TEST VI" << endl;
      char ffile[100];
      sprintf(ffile,"/phenix/u/jpinkenburg/sPHENIX/analysis/AntiSigma/macros/HistoBaseFiles/%s_ThinBins.root", part[q]);
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
	  intnum[i] = (h1->Integral(var-10,var))/10000.;//(h1->Integral());
	  i++;
	}
	var+=10;
      }
      // varnum[0] = 0;
      // intnum[0] = 0;
      char fname [100];
      sprintf(fname, "Gamma_Neutron_Hijing_Energy_Graphs.root");
      TFile *fout = TFile::Open(fname,"UPDATE");
      // h1->Write();
      //fout->Write();
      f->Close();
      //char hname[100];
      //sprintf(hname,"%s%dGeV",part[q],pz[w]);
      // cout << h1->Integral() << " "  <<  hname << " " << pz[i] << endl;
      //TH2 *h2 = new TH2F("hname","hname",1,0,5,1,0,1.2);
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
//	  gr->SetMaximum(1.2);
      //h2->Draw();
      //gr->Draw("A*");
      gr->Draw("ALP");
      //c3->DrawFrame(0,0,0.5,1.2);
      // gr->PaintGrapHist((i-1),varnum,intnum,"chopt");
      //	  gr->SetHistogram(h4);
      //h4->Write();
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
}  

void Hijing()
{
  TFile *fin = TFile::Open("/sphenix/user/pinkenbu/jade/hijingdat2.root");
  gROOT->cd();
  TH1 *hjbkg = new TH1F("hjbkg","Hijing Background",100,0.,0.5);
  TNtuple *de = (TNtuple *) fin->Get("de");
  de->Project("hjbkg","dtotal","(ID<=2)*edep/250.");

       char fname [100];
       sprintf(fname, "Gamma_Neutron_Hijing_Energy_Graphs.root");
       TFile *fout = TFile::Open(fname,"UPDATE");
       hjbkg->Write();
       fout->Write();
       fout->Close();
  fin->Close();
}

void DrawHijing2GeV()
{
  TCanvas *c1 = new TCanvas();
TFile *fin = TFile::Open("Gamma_Neutron_Hijing_Energy_Graphs.root");
gROOT->cd();
TH1 *h1lim = new TH1F("h1lim","",1,0,0.1);
TH1 *hjbkg = (TH1 *) fin->Get("hjbkg")->Clone();
TGraph *anti_neutron2GeV = (TGraph *) fin->Get("anti_neutron2GeV")->Clone();
TGraph *neutron2GeV = (TGraph *) fin->Get("neutron2GeV")->Clone();
h1lim->SetStats(0);
h1lim->SetMaximum(0.3);
h1lim->SetTitle("2 GeV Hadronic Showers with HIJING background");
h1lim->GetYaxis()->SetTitle("Deposited Energey [GeV]");
h1lim->GetYaxis()->SetTitleOffset(1.2);
h1lim->GetXaxis()->SetTitle("cone size (#sqrt{#Delta#Phi^{2}+#Delta#Theta^{2}})");
h1lim->GetXaxis()->SetTitleOffset(1.2);
h1lim->Draw();
hjbkg->SetStats(0);
hjbkg->SetLineColor(6);
hjbkg->SetMaximum(5.5);
hjbkg->SetLineWidth(2);
hjbkg->Draw("same");
anti_neutron2GeV->SetLineColor(4);
anti_neutron2GeV->SetLineWidth(2);
anti_neutron2GeV->Draw("same");
neutron2GeV->SetLineColor(2);
neutron2GeV->SetLineWidth(2);
neutron2GeV->Draw("same");
TLine *tl = new TLine();
tl->SetLineStyle(2);
tl->DrawLine(0.024,0,0.024,0.3);
TLegend *legrda = new TLegend(0.67,0.34,0.87,0.54,NULL,"brNDC");
  legrda->SetLineColor(1);
  legrda->SetLineStyle(1);
  legrda->SetLineWidth(1);
  legrda->SetFillColor(10);
  legrda->SetFillStyle(1001);
  legrda->SetBorderSize(0);
//  legrda->SetTextSize(labelsize);
  legrda->AddEntry(hjbkg,"HIJING bkg"); 
  legrda->AddEntry(anti_neutron2GeV,"2 GeV Anti Neutron","l"); 
  legrda->AddEntry(neutron2GeV,"2 GeV Neutron", "l"); 
  legrda->AddEntry(tl,"EMCal tower size","l"); 
  legrda->Draw();
 
fin->Close();
c1->Print("Hijing2GeV.png");
}

void DrawHijing50GeV()
{
  TCanvas *c1 = new TCanvas();
TFile *fin = TFile::Open("Gamma_Neutron_Hijing_Energy_Graphs.root");
gROOT->cd();
TH1 *hjbkg = (TH1 *) fin->Get("hjbkg")->Clone();
TGraph *anti_neutron50GeV = (TGraph *) fin->Get("anti_neutron50GeV")->Clone();
TGraph *neutron50GeV = (TGraph *) fin->Get("neutron50GeV")->Clone();
hjbkg->SetStats(0);
hjbkg->SetLineColor(6);
hjbkg->SetMaximum(5.5);
hjbkg->SetLineWidth(2);
hjbkg->SetTitle("50 GeV Hadronic Showers with HIJING background");
hjbkg->GetYaxis()->SetTitle("Deposited Energey [GeV]");
hjbkg->GetYaxis()->SetTitleOffset(0.8);
hjbkg->GetXaxis()->SetTitle("cone size (#sqrt{#Delta#Phi^{2}+#Delta#Theta^{2}})");
hjbkg->GetXaxis()->SetTitleOffset(1.2);
hjbkg->Draw();
anti_neutron50GeV->SetLineColor(4);
anti_neutron50GeV->SetLineWidth(2);
anti_neutron50GeV->Draw("same");
neutron50GeV->SetLineColor(2);
neutron50GeV->SetLineWidth(2);
neutron50GeV->Draw("same");
TLine *tl = new TLine();
tl->SetLineStyle(2);
tl->DrawLine(0.1,0,0.1,5.5);
TLegend *legrda = new TLegend(0.67,0.54,0.87,0.74,NULL,"brNDC");
  legrda->SetLineColor(1);
  legrda->SetLineStyle(1);
  legrda->SetLineWidth(1);
  legrda->SetFillColor(10);
  legrda->SetFillStyle(1001);
  legrda->SetBorderSize(0);
//  legrda->SetTextSize(labelsize);
  legrda->AddEntry(hjbkg,"HIJING bkg"); 
  legrda->AddEntry(anti_neutron50GeV,          "50 GeV Anti Neutron","l"); 
  legrda->AddEntry(neutron50GeV,          "50 GeV Neutron", "l"); 
  legrda->AddEntry(tl,          "HCal tower size","l"); 
  legrda->Draw();
fin->Close();
c1->Print("Hijing50GeV.png");
}

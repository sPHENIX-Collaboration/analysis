void ParticleShowerSizeHistoMaker(const int nevents)
{
  float min = -0.5;
  float max = 0.5;
  int nchan = 52;
   const char *file1 = "/sphenix/user/jpinkenburg/ShowerSize/ntuple/neutron_eta0_4GeV.root";

  TFile *f1 = TFile::Open(file1);
  if (! f1)
    {
      cout << file1 << " not found" << endl;
      return;
    }
    gROOT->cd();
 TNtuple *de1 = (TNtuple *) f1->Get("de");
 TCanvas *c1 = new TCanvas();
 TH2F *h2 = new TH2F("neutron","Particle Shower Sizes",nchan,min,max,nchan,min,max);
  h2->SetXTitle("dtheta");
  h2->SetYTitle("dphi");
  h2->SetZTitle("dE/event");
  h2->GetXaxis()->SetTitleOffset(2);
  h2->GetYaxis()->SetTitleOffset(2);
  h2->GetZaxis()->SetTitleOffset(1.2);
  if (nevents > 0)
  {
 de1->Project("neutron","dphi:dtheta","(ID<=2)*edep/10000.","",nevents);
  }
  else
  {
 de1->Project("neutron","dphi:dtheta","(ID<=2)*edep/10000.","");
  }
  h2->Draw("surf");
  f1->Close();
  const char *file2 = "/sphenix/user/jpinkenburg/ShowerSize/ntuple/gamma_eta0_4GeV.root";
  TFile *f2 = TFile::Open(file2);
  if (! f2)
    {
      cout << file2 << " not found" << endl;
      return;
    }
  gROOT->cd();
 
  TNtuple *de2 = (TNtuple *) f2->Get("de");
  cout << nevents << endl;
  TH2F *h2a = new TH2F("gamma","Particle Shower Sizes",nchan,min,max,nchan,min,max);
  h2a->SetXTitle("dtheta");
  h2a->SetYTitle("dphi");
  h2a->SetZTitle("dE/event");
  h2a->GetXaxis()->SetTitleOffset(2);
  h2a->GetYaxis()->SetTitleOffset(2);
  h2a->GetZaxis()->SetTitleOffset(1.2);
  if (nevents > 0)
  {
  de2->Project("gamma","dphi:dtheta","(ID<=2)*edep/10000.","",nevents);
  }
  else
  {
  de2->Project("gamma","dphi:dtheta","(ID<=2)*edep/10000.","");
  }

 h2a->SetLineColor(2);
h2a->Draw("surf same");
// f1->Close();
  f2->Close();
  TFile *f3 = TFile::Open("neutron_gamma_edep_4gev.root","UPDATE");

  h2->Write();
  h2a->Write();
  f3->Write();
  f3->Close();
  return;
  de->SetLineColor(2);
  h2->Draw();
  /* TImage *img = TImage::Create();
  img->FromPad(c1);
  //  f2->Close();
  // TFile *fout = TFile::Open("/direct/phenix+u/jpinkenburg/sPHENIX/analysis/AntiSigma/macros/HistoBaseFiles/PosterFiles/");
  // h2->Write();
  //fout->Write();
  img->WriteImage("Neutron*Gamma_2GeV.png");
  // //fout->Write();
  //fout->Close();
  */
}

void DrawShowers()
{
  TCanvas *c1 = new TCanvas();
  TFile *fin = TFile::Open("neutron_gamma_edep_4gev.root");
  TH2 *gamma = (TH2F *) fin->Get("gamma");
  TH2 *neutron = (TH2F *) fin->Get("neutron");
  c1->SetLogz();
  gamma->SetStats(0);
  gamma->SetLineColor(3);
  gamma->SetTitle("Shower Size");
  gamma->GetXaxis()->SetTitle("#Delta#Theta [rad]");
  gamma->GetXaxis()->SetNdivisions(505);
  gamma->GetYaxis()->SetTitle("#Delta#Phi [rad]");
  gamma->GetYaxis()->SetNdivisions(505);
  gamma->GetZaxis()->SetTitle("Deposited Energy [GeV]");
  gamma->GetZaxis()->SetTitleOffset(1.3);
  neutron->SetLineColor(2);
  neutron->SetStats(0);
  c1->SetTheta(5.);
  c1->SetPhi(22.);

  gamma->DrawClone("surf");
  neutron->DrawClone("surf same");
TLegend *legrda = new TLegend(0.54,0.66,0.9,0.79,NULL,"brNDC");
  legrda->SetLineColor(1);
  legrda->SetLineStyle(1);
  legrda->SetLineWidth(1);
  legrda->SetFillColor(10);
  legrda->SetFillStyle(1001);
  legrda->SetBorderSize(0);
  legrda->SetTextSize(0.039);
  gamma->SetLineWidth(2);
  neutron->SetLineWidth(2);
  legrda->AddEntry(gamma,"4 GeV Photon Shower");
  legrda->AddEntry(neutron,"4 GeV Neutron Shower");
  legrda->Draw();
  c1->Print("showers.png");
}

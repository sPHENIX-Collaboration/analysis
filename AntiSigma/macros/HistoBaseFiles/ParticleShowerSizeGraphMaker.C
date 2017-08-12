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

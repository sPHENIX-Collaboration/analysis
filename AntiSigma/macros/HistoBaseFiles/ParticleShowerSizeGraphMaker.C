void ParticleShowerSizeHistoMaker(const int nevents)
{
   const char *file1 = "/sphenix/user/jpinkenburg/ShowerSize/ntuple/neutron_eta0_12GeV.root";

  TFile *f1 = TFile::Open(file1);
  if (! f1)
    {
      cout << file1 << " not found" << endl;
      return;
    }
    gROOT->cd();
 TNtuple *de1 = (TNtuple *) f1->Get("de");
 TCanvas *c1 = new TCanvas();
 TH2F *h2 = new TH2F("h2","Particle Shower Sizes",20,-.2,.2,10,-.2,.2);
  h2->SetXTitle("dtheta");
  h2->SetYTitle("dphi");
  h2->SetZTitle("Number of Hits");
  h2->GetXaxis()->SetTitleOffset(2);
  h2->GetYaxis()->SetTitleOffset(2);
  h2->GetZaxis()->SetTitleOffset(1.2);
de1->Project("h2","dphi:dtheta","(ID<=2)*edep","",nevents);
  h2->Draw("surf");
  h2->Write();
  f1->Close();
  const char *file2 = "/sphenix/user/jpinkenburg/ShowerSize/ntuple/gamma_eta0_12GeV.root";
  TFile *f2 = TFile::Open(file2);
  if (! f2)
    {
      cout << file2 << " not found" << endl;
      return;
    }
  gROOT->cd();
 
  TNtuple *de2 = (TNtuple *) f2->Get("de");
  cout << nevents << endl;
  TH2F *h2a = new TH2F("h2a","Particle Shower Sizes",20,-.2,.2,10,-.2,.2);
  h2a->SetXTitle("dtheta");
  h2a->SetYTitle("dphi");
  h2a->SetZTitle("Number of Hits");
  h2a->GetXaxis()->SetTitleOffset(2);
  h2a->GetYaxis()->SetTitleOffset(2);
  h2a->GetZaxis()->SetTitleOffset(1.2);
  de2->Project("h2a","dphi:dtheta","(ID<=2)*edep","",nevents);
 h2a->SetLineColor(2);
h2a->Draw("surf same");
// f1->Close();
  f2->Close();
  TFile *f3 = TFile::Open("test.root","UPDATE");

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

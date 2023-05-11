void Surf3DHistoMaker(int ncol, int gcol)
{
  TFile *fin = TFile::Open("neutron_gamma_edep.root");
  gROOT->cd();
  TCanvas *c1 = new TCanvas();
  c1->SetLogz();
  TH2 *h2 = (TH2F *) fin->Get("neutron")->Clone();
  h2->SetXTitle("dtheta");
  h2->SetYTitle("dphi");
  h2->SetZTitle("Energy");
  h2->GetXaxis()->SetTitleOffset(2);
  h2->GetYaxis()->SetTitleOffset(2);
  h2->GetZaxis()->SetTitleOffset(1.2);
  TH2 *h2a = (TH2F *) fin->Get("gamma")->Clone();
  h2a->SetXTitle("dtheta");
  h2a->SetYTitle("dphi");
  h2a->SetZTitle("Energy");
  h2a->GetXaxis()->SetTitleOffset(2);
  h2a->GetYaxis()->SetTitleOffset(2);
  h2a->GetZaxis()->SetTitleOffset(1.2);
  h2->SetLineColor(ncol);
  h2a->SetLineColor(gcol);
  h2->Draw("surf");
  h2a->Draw("surf same");
  // TFile *fout = TFile::Open("finalhistos.root","UPDATE");
  c1->Print("Neutron_vs_Gamma.png");
  fin->Close();
  //fout->Close();
}

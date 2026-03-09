root -l hadd_RhoFluct.root <<EOF

  TH1D* hparamt = new TH1D("hparamt", ";impactparam;N_{events}", 10000, 0.,22);
  TH1D* gross   = new TH1D("gross", ";impact param [fm];N_{events}", 100, 0.,22);
  double x[11];
  double* q = new double[11];
  for (int i=0;i<11;++i) {
    x[i] = .1*i;
  }

  T->Draw("impactparam>>hparamt");
  hparamt->GetQuantiles(11,q,x);
  for (int i=0;i<11;++i) {
    cout << " i("<<i<<") x("<<x[i]<<") q("<<q[i]<<")"<<endl;
  }

  T->Draw("impactparam>>gross");
TCanvas*c = new TCanvas("c","c");
  hparamt->Draw("PE");
  /* c->WaitPrimitive(); */
  TFile *fout = new TFile("CalcDecile_RhoFluct.root","recreate");
  TH1D* keep = (TH1D*) gross->Clone("h_impactparam");
  TH1D* bounds = new TH1D("bound",";[fm];", 10, q);
  keep->Write();
  bounds->Write();
  fout->Close();


EOF

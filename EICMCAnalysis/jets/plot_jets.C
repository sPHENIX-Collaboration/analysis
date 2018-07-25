int
plot_jets()
{
  TFile *fin = new TFile("jettest.root", "OPEN");

  TTree* jets = (TTree*)fin->Get("jets");

  TCut cut_base("1");
  //  TCut cut_eemc("jet_truth_eta>-4&&jet_truth_eta<-1.55");
  //  TCut cut_cemc("jet_truth_eta>-1.55&&jet_truth_eta<1.24");
  //  TCut cut_femc("jet_truth_eta>1.24&&jet_truth_eta<4");

  //TCut cut_eta1("jet_truth_eta>-3.3&&jet_truth_eta<-2.3");
  TCut cut_eta1("jet_truth_eta>-4&&jet_truth_eta<-2");
  TCut cut_eta2("jet_truth_eta>-1&&jet_truth_eta<1");
  //TCut cut_eta3("jet_truth_eta>2.3&&jet_truth_eta<3.3");
  TCut cut_eta3("jet_truth_eta>2&&jet_truth_eta<4");

  TCanvas *ctemp = new TCanvas("**Scratch**");

  /* jet energies */
  ctemp->cd();

  TH2F* h_jets_energy_vs_eta = new TH2F("h_jets_energy_vs_eta","",36,-4.5,4.5,25,0,100);
  jets->Draw("jet_truth_e : jet_truth_eta >> h_jets_energy_vs_eta","","colz");

  TCanvas *c4 = new TCanvas();
  h_jets_energy_vs_eta->Draw("colz");
  c4->SetLogz();

  /* jet energies in bins of pseudorapidity */
  ctemp->cd();

  TH1F* h_jets_energy_eta1 = new TH1F("h_jets_energy_eta1",";E_{jet}^{truth} (GeV);counts",100,0,50);
  TH1F* h_jets_energy_eta2 = (TH1F*)h_jets_energy_eta1->Clone("h_jets_energy_eta2");
  TH1F* h_jets_energy_eta3 = (TH1F*)h_jets_energy_eta1->Clone("h_jets_energy_eta3");

  h_jets_energy_eta1->SetLineColor(kBlue);
  h_jets_energy_eta2->SetLineColor(kRed);
  h_jets_energy_eta3->SetLineColor(kGreen+1);

  jets->Draw("jet_truth_e >> h_jets_energy_eta1",cut_eta1,"");
  jets->Draw("jet_truth_e >> h_jets_energy_eta2",cut_eta2,"");
  jets->Draw("jet_truth_e >> h_jets_energy_eta3",cut_eta3,"");

  TCanvas *c2 = new TCanvas();
  h_jets_energy_eta2->Draw("");
  h_jets_energy_eta1->Draw("same");
  h_jets_energy_eta3->Draw("same");


  /* jet energy resolution */
  ctemp->cd();

  TH2F* h_eres_eta3 = new TH2F("h_eres_eta3","",10,2.5,52.5,20,-1,1);
  Int_t nbins_x_eta3 = h_eres_eta3->GetXaxis()->GetNbins();

  TGraphErrors* g_eres_eta3 = new TGraphErrors( nbins_x_eta3 );

  TH1F* h_frame_eres = new TH1F("h_frame_eres", "", 10,2.5,52.5);
  h_frame_eres->GetYaxis()->SetRangeUser(0.1,0.25);
  h_frame_eres->GetXaxis()->SetTitle("E^{truth} [GeV]");
  h_frame_eres->GetYaxis()->SetTitle("#DeltaE / E^{truth}");

  TCanvas *c1 = new TCanvas();
  jets->Draw("(jet_smear_e-jet_truth_e)/jet_truth_e:jet_truth_e >> h_eres_eta3",cut_eta3);
  h_eres_eta3->Draw("COLZ");

  /* Loop over all x-bins in 2D histogram, create Y projections, and get standard deviation */
  for ( Int_t i = 0; i < nbins_x_eta3; i++ )
    {
      ctemp->cd();

      TH1D* h_proj = h_eres_eta3->ProjectionY("py",i+1,i+1);

      /* skip projections with too few entries */
      if ( h_proj->GetEntries() < 100 )
	continue;

      h_proj->Fit("gaus","Q");
      TF1* fit = h_proj->GetFunction("gaus");

      TCanvas *cstore = new TCanvas();

      TString proj_name = TString::Format("Projection_bin_center_%.01f", h_eres_eta3->GetXaxis()->GetBinCenter(i+1) );

      cstore->SetName(proj_name);
      cstore->SetTitle(proj_name);
      h_proj->DrawClone("clone");

      cout << "RMS: " << h_eres_eta3->GetXaxis()->GetBinCenter(i+1) << " --> " <<  h_proj->GetRMS() << endl;

      if (fit)
	{
	  cout << "FIT: " << h_eres_eta3->GetXaxis()->GetBinCenter(i+1) << " --> " <<  fit->GetParameter(2) << endl;
	  g_eres_eta3->SetPoint(i, h_eres_eta3->GetXaxis()->GetBinCenter(i+1), fit->GetParameter(2));
	  g_eres_eta3->SetPointError(i, 0, fit->GetParError(2));
	}

      //g_eres_eta3->SetPoint(i, h_eres_eta3->GetXaxis()->GetBinCenter(i+1), h_proj->GetRMS());
    }

  /* Draw resolution graph */
  TCanvas *c3 = new TCanvas();
  h_frame_eres->Draw();
  g_eres_eta3->Draw("Psame");

  return 0;
}

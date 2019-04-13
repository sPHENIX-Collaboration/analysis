int
hera_dis_countbins()
{
  TFile *fin = new TFile("hera_dis_histo.root","OPEN");
  THnSparse *hfull = (THnSparse*)fin->Get("hn_dis");

  TH2F* hxQ2 = (TH2F*)hfull->Projection(1,0);

  /* beam energies */
  float ebeam_e = 27.5;
  float ebeam_p = 820;

  /* Pythia generated events, cross section, and luminosity */
  float pythia_ngen = 999999;
  float pythia_xsec = 0.80120995103272474; // in microbarn
  float convert_microbarn_to_femtobarn = 1e9;
  float pythia_lumi = pythia_ngen / ( pythia_xsec * convert_microbarn_to_femtobarn );

  /* target luminosity and scaling factor */
  float target_lumi = 0.5; // in inverse femtobarn
  float lumi_scaling = target_lumi / pythia_lumi;

  cout << "Pythia luminosity:  " << pythia_lumi << " fb^-1" << endl;
  cout << "Target luminosity:  " << target_lumi << " fb^-1" << endl;
  cout << "Luminosity scaling: " << lumi_scaling << endl;

  /* create tree to store information */
  TFile *fout = new TFile("hera_dis_tree.root", "RECREATE");

  TTree *tcount = new TTree("tcount", "A tree with counts in kinematics bins");
  float t_pbeam_lepton = 0;
  float t_pbeam_proton = 0;
  float t_s = 0;
  float t_x = 0;
  float t_Q2 = 0;
  float t_y = 0;
  float t_N = 0;
  float t_stdev_N = 0;
  tcount->Branch("pbeam_lepton", &t_pbeam_lepton, "pbeam_lepton/F");
  tcount->Branch("pbeam_proton", &t_pbeam_proton, "pbeam_proton/F");
  tcount->Branch("s", &t_s, "s/F");
  tcount->Branch("x", &t_x, "x/F");
  tcount->Branch("Q2", &t_Q2, "Q2/F");
  tcount->Branch("y", &t_y, "y/F");
  tcount->Branch("N", &t_N, "N/F");
  tcount->Branch("stdev_N", &t_stdev_N, "stdev_N/F");

  /* copy beam parameters */
  t_pbeam_lepton = ebeam_e;
  t_pbeam_proton = ebeam_p;

  /* center of mass energy */
  t_s = 4 * t_pbeam_lepton * t_pbeam_proton;

  /* loop over all bins */
  for ( int bin_x = 1; bin_x <= hxQ2->GetNbinsX(); bin_x++ )
    {

      for ( int bin_y = 1; bin_y <= hxQ2->GetNbinsY(); bin_y++ )
	{


	  t_x = TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) ) )
					 + ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) + hxQ2->GetXaxis()->GetBinWidth(bin_x) ) ) ) );

	  t_Q2 = TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_y) ) )
					  + ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_y) + hxQ2->GetYaxis()->GetBinWidth(bin_y) ) ) ) );

	  t_y = t_Q2 / ( t_x * t_s );

	  t_N = hxQ2->GetBinContent( bin_x, bin_y ) * lumi_scaling;

	  /* skip kinematics bins wth y > 0.95 and y < 1e-2 */
	  if ( t_y > 0.95 || t_y < 1e-2 )
	    continue;

	  t_stdev_N = 1./(sqrt(t_N));

	  tcount->Fill();

	  std::cout.precision(2);

	  cout << "lepton = " << std::fixed << t_pbeam_lepton
	       << " x proton = " << std::fixed << t_pbeam_proton
	       << " , sqrt(s) = " << std::fixed << sqrt( t_s )
	       << " , x = " << std::scientific << t_x
	       << " , Q2 = " << std::scientific << t_Q2
	       << " , y = " << std::fixed << t_y
	       << " , N = " << std::scientific << t_N
	       << endl;


	}

    }

  TCanvas *c1 = new TCanvas();
  hxQ2->Draw("colz");
  c1->SetLogx(1);
  c1->SetLogy(1);
  c1->SetLogz(1);

  TCanvas *c2 = new TCanvas();
  tcount->Draw("N:Q2", "x > 1e-2 && x < 2e-2");

  tcount->Write();
  fout->Close();

  return 0;
}

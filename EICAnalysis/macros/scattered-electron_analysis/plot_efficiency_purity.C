int plot_efficiency_purity()
{
  vector< TString > v_cuts;

  TString base_ep = "em_cluster_e/em_track_ptotal > ";
  TString base_prob = "em_cluster_prob >= ";

  Double_t cut_ep = 0.90;
  Double_t cut_prob = 0.02;
  v_cuts.push_back(TString(base_ep) += cut_ep);
  v_cuts.push_back(TString(base_prob) += cut_prob);

  for ( unsigned i = 0; i < v_cuts.size(); i++ )
    {
      cout << v_cuts.at(i) << endl;
    }

  vector< float > v_momenta;
  v_momenta.push_back(1.0);
  v_momenta.push_back(2.0);
  v_momenta.push_back(5.0);
  v_momenta.push_back(10.0);
  v_momenta.push_back(20.0);

  /* Loop over detectors */
  {
    //...
    /* loope over cuts */
    {
      /* loop over energies */
      {
	vector< float > v_rates_tp;
	vector< float > v_rates_fp;

	for ( unsigned idx_p = 0; idx_p < v_momenta.size(); idx_p++ )
	  {
	    v_rates_tp.push_back( 1. - 0.1 * v_momenta.at(idx_p) );
	    v_rates_fp.push_back( 0.1 * v_momenta.at(idx_p) );
	  }
	TGraph* g_tp = new TGraph(v_momenta.size(), &(v_momenta[0]), &(v_rates_tp[0]));
	TGraph* g_fp = new TGraph(v_momenta.size(), &(v_momenta[0]), &(v_rates_fp[0]));

	g_tp->SetMarkerColor(kGreen+2);
	g_fp->SetMarkerColor(kRed+2);

	g_tp->SetLineColor(g_tp->GetMarkerColor());
	g_fp->SetLineColor(g_fp->GetMarkerColor());

	g_tp->Draw("LPA");
	g_fp->Draw("LPsame");
      }
    }
  }
  return 0;
}

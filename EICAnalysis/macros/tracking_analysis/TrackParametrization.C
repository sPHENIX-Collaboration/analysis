int
TrackParametrization( TString csvfile="Momentum_Sigma.csv" )
{

  /* Read data from input file */
  TTree *tres = new TTree();
  tres->ReadFile( csvfile, "ptrue:thetatrue:psig:psig_err:pmean:pmean_err", ',' );

  /* Print read-in tree */
  tres->Print();

  /* Create vector of theta values to include */
  vector< double > thetas;
  thetas.push_back( 2.61799 );
  thetas.push_back( 2.35619 );
  thetas.push_back( 2.0944 );
  thetas.push_back( 1.8326 );
  thetas.push_back( 1.5708 );
  thetas.push_back( 1.309 );
  thetas.push_back( 1.0472 );
  thetas.push_back( 0.261799 );

  /* Create fit function */
  //TF1* f_momres = new TF1("f_momres", "[0]*x + [1]*x*x" );
  TF1* f_momres = new TF1("f_momres", "[0] + [1]*x" );

  cout << "\nFit function: " << f_momres->GetTitle() << "\n" << endl;

  /* Create scratch canvas */
  TCanvas *cscratch = new TCanvas("cscratch");

  /* Create framehistogram */
  TH1F* hframe = new TH1F("hframe","",100,0,30);
  hframe->GetYaxis()->SetRangeUser(0,0.15);
  hframe->GetXaxis()->SetTitle("Momentum (GeV/c)");
  hframe->GetYaxis()->SetTitle("#sigma_{p}/p");

  /* create combined canvas plot */
  TCanvas *c1 = new TCanvas();
  hframe->Draw();

  /* Create legend */
  TLegend* leg_eta = new TLegend( 0.25, 0.40, 0.45, 0.90);

  /* Create resolution-vs-momentum plot with fits for each selected theta value */
  for ( int i = 0; i < thetas.size(); i++ )
    {
      /* Switch to scratch canvas */
      cscratch->cd();

      /* Calculate pseudorapidity eta */
      double eta = -1 * log( tan( thetas.at(i) / 2. ) );

      cout << "\n***Theta = " << thetas.at(i) << ", Eta = " << eta << endl;

      /* Define range of theta because float comparison with fixed value doesn't work
	 too well for cuts in ROOT trees */
      double theta_min = thetas.at(i) * 0.999;
      double theta_max = thetas.at(i) * 1.001;

      /* Cut for tree */
      TCut cutx( Form("thetatrue > %f && thetatrue < %f", theta_min, theta_max) );

      /* "Draw" tree on scratch canvas to fill V1...V4 arrays */
      tres->Draw("psig/pmean:ptrue:psig_err:0", cutx );

      /* Create TGraphErrors with selected data from tree */
      TGraphErrors *gres = new TGraphErrors( tres->GetEntries(cutx),
					     &(tres->GetV2())[0],
					     &(tres->GetV1())[0],
					     &(tres->GetV4())[0],
					     &(tres->GetV3())[0] );

      gres->SetMarkerColor(i+1);

      /* Add graph to legend */
      leg_eta->AddEntry(gres, Form("#eta = %.1f", eta), "P");

      /* Add graph to plot */
      c1->cd();
      gres->Draw("Psame");
      gres->Fit(f_momres);
    }

  /* Draw legend */
  //  c1->cd();
  TCanvas *c2 = new TCanvas();
  //hframe->Draw();
  leg_eta->Draw();

  /* Print plots */
  c1->Print("track_momres_vareta.eps");
  c2->Print("track_momres_vareta_legend.eps");

  return 0;
}

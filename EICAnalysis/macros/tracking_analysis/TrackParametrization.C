int
TrackParametrization( TString csvfile="fitslices_out.csv" )
{

  /* Read data from input file */
  TTree *tres = new TTree();
  tres->ReadFile( csvfile, "ptrue:etatrue:psig:psig_err:pmean:pmean_err:norm", ',' );

  /* Print read-in tree */
  tres->Print();

  /* colors array */
  unsigned colors[7] = {1,2,3,4,6,7,14};

  /* select how many eta settings to plot */
  unsigned etas_plotmax = 7;

  /* Create vector of theta values to include */
  vector< double > etas;
  etas.push_back(-2.75);
  etas.push_back(-2.25);
  etas.push_back(-1.75);
  etas.push_back(-0.25);
  etas.push_back( 0.25);
  etas.push_back( 1.75);
  etas.push_back( 2.25);

  etas.push_back(-1.25);
  etas.push_back(-0.75);
  etas.push_back( 0.75);
  etas.push_back( 1.25);

  /* Create fit function */
  //TF1* f_momres = new TF1("f_momres", "[0]*x + [1]*x*x" );
  TF1* f_momres = new TF1("f_momres", "sqrt( [0]*[0] + [1]*[1]*x*x )" );

  cout << "\nFit function: " << f_momres->GetTitle() << "\n" << endl;

  /* Create scratch canvas */
  TCanvas *cscratch = new TCanvas("cscratch");

  /* Create framehistogram */
  TH1F* hframe = new TH1F("hframe","",100,0,40);
  hframe->GetYaxis()->SetRangeUser(0,0.15);
  hframe->GetXaxis()->SetTitle("Momentum (GeV/c)");
  hframe->GetYaxis()->SetTitle("#sigma_{p}/p");

  /* create combined canvas plot */
  TCanvas *c1 = new TCanvas();
  hframe->Draw();

  /* Create legend */
  TLegend* leg_eta = new TLegend( 0.25, 0.40, 0.45, 0.90);

  /* Create ofstream to write fit parameter results */
  ofstream ofsfit("track_momres_new.csv");
  ofsfit<<"eta,par1,par1err,par2,par2err"<<endl;

  /* Create resolution-vs-momentum plot with fits for each selected theta value */
  for ( int i = 0; i < etas.size(); i++ )
    {
      /* Switch to scratch canvas */
      cscratch->cd();

      double eta = etas.at(i);

      /* Calculate pseudorapidity eta */
      //double eta = -1 * log( tan( thetas.at(i) / 2. ) );

      cout << "\n***Eta = " << eta << endl;

      /* Define range of theta because float comparison with fixed value doesn't work
	 too well for cuts in ROOT trees */
      double eta_min = eta * 0.999;
      double eta_max = eta * 1.001;

      /* Cut for tree */
      TCut cutx( Form("ptrue > 1 && ( (etatrue > 0 && (etatrue > %f && etatrue < %f)) || (etatrue < 0 && (etatrue < %f && etatrue > %f)) )", eta_min, eta_max, eta_min, eta_max) );

      /* "Draw" tree on scratch canvas to fill V1...V4 arrays */
      //tres->Draw("psig/pmean:ptrue:psig_err/pmean:0", cutx );
      tres->Draw("psig:ptrue:psig_err:0", cutx );

      /* Create TGraphErrors with selected data from tree */
      TGraphErrors *gres = new TGraphErrors( tres->GetEntries(cutx),
					     &(tres->GetV2())[0],
					     &(tres->GetV1())[0],
					     &(tres->GetV4())[0],
					     &(tres->GetV3())[0] );

      gres->SetMarkerColor(colors[i]);

      /* Only plot first few lines; if not plotting, still do the fit */
      if ( i < etas_plotmax )
	{
	  /* Add graph to legend */
	  leg_eta->AddEntry(gres, Form("#eta = %.1f", eta), "P");

	  /* Add graph to plot */
	  c1->cd();
	  gres->Draw("Psame");
	  f_momres->SetLineColor(colors[i]);
	  gres->Fit(f_momres);
	}
      else
	{
	  gres->Fit(f_momres);
	}

      /* Write fir results to file */
      double par1 = f_momres->GetParameter(0);
      double par1err = f_momres->GetParError(0);
      double par2 = f_momres->GetParameter(1);
      double par2err = f_momres->GetParError(1);
      ofsfit << eta << "," << par1 << "," << par1err << "," << par2 << "," << par2err << endl;

    }

  /* Draw legend */
  //  c1->cd();
  TCanvas *c2 = new TCanvas();
  //hframe->Draw();
  leg_eta->Draw();

  /* Print plots */
  c1->Print("track_momres_vareta.eps");
  c2->Print("track_momres_vareta_legend.eps");

  /* Close output stream */
  ofsfit.close();

  return 0;
}

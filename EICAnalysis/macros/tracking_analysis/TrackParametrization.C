int
TrackParametrization( TString csvfile="fitslices_out.csv" )
{

  /* Read data from input file */
  TTree *tres = new TTree();
  tres->ReadFile( csvfile, "ptrue:etatrue:psig:psig_err:pmean:pmean_err:norm", ',' );

  /* Print read-in tree */
  tres->Print();

  /* colors array */
  unsigned colors[8] = {1,2,3,4,6,7,14,16};

  /* Create vector of theta values to include for visualization*/
  vector< double > etas_vis;
  etas_vis.push_back(-2.75);
  etas_vis.push_back(-2.25);
  etas_vis.push_back(-1.75);
  etas_vis.push_back(-0.25);
  etas_vis.push_back( 0.25);
  etas_vis.push_back( 1.75);
  etas_vis.push_back( 2.25);

//  etas_vis.push_back(-3.25);
//  etas_vis.push_back(-2.25);
//  etas_vis.push_back(-1.25);
//  etas_vis.push_back(-0.25);
//  etas_vis.push_back( 0.25);
//  etas_vis.push_back( 1.25);
//  etas_vis.push_back( 2.25);
//  etas_vis.push_back( 3.25);

  /* Create vector of theta values to include for fitting*/
  vector< double > etas_fit;
  for ( double eta = -4.45; eta < 4.5; eta += 0.1 )
    etas_fit.push_back( eta );

  /* Create fit function */
  TF1* f_momres = new TF1("f_momres", "sqrt( [0]*[0] + [1]*[1]*x*x )" );

  cout << "\nFit function: " << f_momres->GetTitle() << "\n" << endl;

  /* Create scratch canvas */
  TCanvas *cscratch = new TCanvas("cscratch");

  /* Create framehistogram */
  TH1F* hframe = new TH1F("hframe","",100,0,40);
  hframe->GetYaxis()->SetRangeUser(0,0.15);
  hframe->GetYaxis()->SetNdivisions(505);
  hframe->GetXaxis()->SetTitle("Momentum (GeV/c)");
  hframe->GetYaxis()->SetTitle("#sigma_{p}/p");

  /* create combined canvas plot */
  TCanvas *c1 = new TCanvas();
  hframe->Draw();

  /* Create legend */
  TLegend* leg_eta = new TLegend( 0.2, 0.6, 0.5, 0.9);
  leg_eta->SetNColumns(2);

  /* Create ofstream to write fit parameter results */
  ofstream ofsfit("track_momres_new.csv");
  ofsfit<<"eta,par1,par1err,par2,par2err"<<endl;

  /* Create resolution-vs-momentum plot with fits for each selected theta value */
  for ( int i = 0; i < etas_fit.size(); i++ )
    {
      /* Switch to scratch canvas */
      cscratch->cd();

      double eta = etas_fit.at(i);

      /* No tracking outside -4 < eta < 4 */
      if ( eta < -4 || eta > 4 )
	continue;

      cout << "\n***Eta = " << eta << endl;

      /* Define range of theta because float comparison with fixed value doesn't work
	 too well for cuts in ROOT trees */
      double eta_min = eta * 0.999;
      double eta_max = eta * 1.001;

      /* Cut for tree */
      TCut cutx( Form("ptrue > 1 && ( (etatrue > 0 && (etatrue > %f && etatrue < %f)) || (etatrue < 0 && (etatrue < %f && etatrue > %f)) )", eta_min, eta_max, eta_min, eta_max) );

      /* "Draw" tree on scratch canvas to fill V1...V4 arrays */
      tres->Draw("psig:ptrue:psig_err:0", cutx );

      /* Create TGraphErrors with selected data from tree */
      TGraphErrors *gres = new TGraphErrors( tres->GetEntries(cutx),
					     &(tres->GetV2())[0],
					     &(tres->GetV1())[0],
					     &(tres->GetV4())[0],
					     &(tres->GetV3())[0] );

      /* reset function parameters before fit */
      f_momres->SetParameter(0,0.1);
      f_momres->SetParameter(1,0.1);

      /* Only plot pseudorapidities listed on etas_vis; if not plotting, still do the fit */
      bool vis = false;
      int vi = 0;

      for ( vi = 0; vi < etas_vis.size(); vi++ )
	{
	  if ( abs( etas_vis.at(vi) - eta ) < 0.001 )
	    {
	      vis = true;
	      break;
	    }
	}

      if ( vis )
	{
	  /* Add graph to legend */
	  leg_eta->AddEntry(gres, Form("#eta = %.1f", eta), "P");

	  /* Add graph to plot */
	  c1->cd();
	  gres->SetMarkerColor(colors[vi]);
	  gres->Draw("Psame");
	  f_momres->SetLineColor(colors[vi]);
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
  c1->cd();
  //TCanvas *c2 = new TCanvas();
  //hframe->Draw();
  leg_eta->Draw();

  /* Print plots */
  c1->Print("track_momres_vareta.eps");
  //c2->Print("track_momres_vareta_legend.eps");

  /* Close output stream */
  ofsfit.close();

  return 0;
}

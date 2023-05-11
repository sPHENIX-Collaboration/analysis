int
eicsmear_check_calorimeter( TString filename_mc,
			    TString filename_mc_smeared = "")
{
  /* Uncomment this line when running without compilation */
  gSystem->Load("libeicsmear");

  /* Open input files. */
  TFile *file_mc = new TFile(filename_mc, "OPEN");
  TFile *file_mc_smeared = new TFile(filename_mc_smeared, "OPEN");

  /* Get trees from files. */
  TTree *tree = (TTree*)file_mc->Get("EICTree");
  TTree *tree_smeared = (TTree*)file_mc_smeared->Get("Smeared");

  /* Add friend to match branches in trees. */
  tree->AddFriend(tree_smeared);

  erhic::EventPythia *event  = NULL;
  Smear::Event       *eventS = NULL;

  tree->SetBranchAddress("event", &event);
  tree->SetBranchAddress("eventS", &eventS);

  /* Define base cut for reconstructed final state particles */
  TCut cut_base("particles.KS==1 && Smeared.particles.p > 0 && (particles.id==11 || particles.id==22)");

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

  /* Create fit function */
  TF1* f_momres = new TF1("f_momres", "sqrt( ([0])**2 + ([1]/(x**0.5))**2 )" );

  cout << "\nFit function: " << f_momres->GetTitle() << "\n" << endl;

  /* Create scratch canvas */
  TCanvas *cscratch = new TCanvas("cscratch");

  /* Create framehistogram */
  TH1F* hframe = new TH1F("hframe","",100,0,40);
  hframe->GetYaxis()->SetRangeUser(0,0.15);
  hframe->GetXaxis()->SetTitle("Energy (GeV/c)");
  hframe->GetYaxis()->SetTitle("#sigma_{E}/E");

  /* Create 2Dhistogram */
  unsigned nbinsp = 40;
  TH2F* h2tmp = new TH2F("h2tmp","",nbinsp,0,(float)nbinsp,50,-1,1);
  h2tmp->GetXaxis()->SetTitle("Energy (GeV)");
  h2tmp->GetYaxis()->SetTitle("(#Delta E)/E");

  /* create combined canvas plot */
  TCanvas *c1 = new TCanvas();
  hframe->Draw();

  /* Create legend */
  TLegend* leg_eta = new TLegend( 0.25, 0.40, 0.45, 0.90);

  /* Create resolution-vs-momentum plot with fits for each selected theta value */
  for ( int i = 0; i < etas.size(); i++ )
    {
      /* Switch to scratch canvas */
      cscratch->cd();

      double eta = etas.at(i);

      cout << "\n***Eta = " << eta << endl;

      /* Define range of theta because float comparison with fixed value doesn't work
         too well for cuts in ROOT trees */
      double eta_min = eta - 0.25;
      double eta_max = eta + 0.25;

      /* Cut for tree */
      TCut cutx( Form("particles.p > 1 && ( particles.eta > %f && particles.eta < %f )", eta_min, eta_max) );
      cout << cutx.GetTitle() << endl;

      /* Fill 2D histogram and fit slices with gaussian to get track resolution */
      h2tmp->Reset();
      tree->Draw("(Smeared.particles.E-particles.E)/particles.E:particles.E >> h2tmp",cut_base && cutx,"");
      h2tmp->FitSlicesY();

      /* Get histogram with sigma from gauss fits */
      TH1D* h2tmp_2 = (TH1D*)gDirectory->Get("h2tmp_2");

      /* Fill fit parameters in TGraphErrors */
      TGraphErrors *gres = new TGraphErrors(nbinsp);
      gres->SetMarkerColor(colors[i]);
      for ( unsigned bini = 1; bini < nbinsp; bini++ )
	{
	  double sigm_i =     h2tmp_2->GetBinContent(bini);
	  double sigm_err_i = h2tmp_2->GetBinError(bini);

	  gres->SetPoint( bini-1, h2tmp->GetXaxis()->GetBinCenter(bini), sigm_i );
	  gres->SetPointError( bini-1, 0, sigm_err_i );
	}

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
    }

  /* Draw legend */
  TCanvas *c2 = new TCanvas();
  leg_eta->Draw();

  return 0;
}

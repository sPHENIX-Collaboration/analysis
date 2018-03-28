bool accept_electron( float eta, float p )
{
  float Emin = 2;

  if ( p > Emin )
    {
      if ( ( eta > -3 && eta < 1.1 ) ||
           ( eta > 1.45 && eta < 3.5 ) )
        {
          return true;
        }
    }

  return false;
}


bool accept_pid( float eta, float p, int pid )
{
  float pmin = 2;
  bool accept = false;

  if ( p > pmin )
    {
      if ( pid == 211 || pid == -211 || pid == 321 || pid == -321 )
	{
	  /* Particle found within PID acceptance for Gas-RICH? */
	  if ( ( eta > 1.45 && eta < 3.5 && p > 3 && p < 50 ) )
	    accept = true;

	  /* Particle found within PID acceptance for mRICH? */
	  else if ( ( eta > 1.45 && eta < 2 && p < 6 ) )
	    accept = true;

	  /* Particle found within PID acceptance for DIRC? */
	  else if ( ( eta > -1.1 && eta < 1.1 && p < 3.5 ) )
	    accept = true;
	}
    }

  return accept;
}


int
eic_sphenix_dis_fillbins()
{
  gSystem->Load("libeicsmear");

  /* Open input files. */
  //TFile *file_mc = new TFile("data/pythiaeRHIC/TREES/pythia.ep.20x250.RadCor=0.Q2gt1.50kevts.root");
  TFile *file_mc = new TFile("data/TEST/pythia.ep.20x250.1Mevents.1.RadCor=0.Q2gt1.root");
  //TFile *file_mc_smeared = new TFile("data/pythiaeRHIC/TREES/pythia.ep.20x250.RadCor=0.Q2gt1.50kevts.smear.root");

  /* Get trees from files. */
  TTree *tree = (TTree*)file_mc->Get("EICTree");
  //TTree *tree_smeared = (TTree*)file_mc_smeared->Get("Smeared");

  /* Output file. */
  TFile *fout = new TFile("eic_sphenix_dis_histo.root", "RECREATE");

  /* Add friend to match branches in trees. */
  //tree->AddFriend(tree_smeared);

  erhic::EventPythia *event  = NULL;
  //Smear::Event       *eventS = NULL;

  tree->SetBranchAddress("event", &event);
  //tree->SetBranchAddress("eventS", &eventS);

  /* Create histogram for eta of particles to check acceptance of detectors. */
  TH1F* h_eta = new TH1F("h_eta", ";#eta;dN/d#eta", 100, -5, 5);
  TH1F* h_eta_accept = (TH1F*)h_eta->Clone("h_eta_accept");
  h_eta_accept->SetLineColor(kGreen);

  /* Create n-dimensional histogram to collect statistic for DIS and SIDIS events in kinematics bins:
   *
   * Event properties:
   * - x
   * - Q2
   *
   * Particle properties:
   * - z
   *
   */

  /* Define bin ranges for DIS and SIDIS histograms */

  /* x */
  const int nbins_x  = 25; // 5 per decade

  double min_x = -5;
  double max_x = 0;
  double width_x = (max_x - min_x) / nbins_x;
  double bins_x[nbins_x+1];
  for( int i =0; i <= nbins_x; i++)
    {
      cout << TMath::Power( 10, min_x + i*width_x) << endl;
      bins_x[i] = TMath::Power( 10, min_x + i*width_x);
    }

  /* Q2 */
  const int nbins_Q2 = 20; // 4 per decade

  double min_Q2 = -1;
  double max_Q2 = 4;
  double width_Q2 = (max_Q2 - min_Q2) / nbins_Q2;
  double bins_Q2[nbins_Q2+1];
  for( int i =0; i <= nbins_Q2; i++)
    bins_Q2[i] = TMath::Power( 10, min_Q2 + i*width_Q2);

  /* z */
  const int nbins_z  = 9;

  double min_z = 0.05;
  double max_z = 0.95;
  double width_z = (max_z - min_z) / nbins_z;
  double bins_z[nbins_z+1];
  for( int i =0; i <= nbins_z; i++)
    {
      cout << min_z + i*width_z << endl;
      bins_z[i] = min_z + i*width_z;
    }

  /* pT */
  const int nbins_pT1 = 10;
  const int nbins_pT2 = 9;
  const int nbins_pT = nbins_pT1 + nbins_pT2;

  double min_pT = 0.0;
  double width_pT1 = 0.1;
  double width_pT2 = 1.0;

  double bins_pT[nbins_pT+1];
  for( int i =0; i <= nbins_pT1; i++)
    {
      cout << min_pT + i*width_pT1 << endl;
      bins_pT[i] = min_pT + i*width_pT1;
    }
  for( int i =1; i <= nbins_pT2; i++)
    {
      cout << min_pT + nbins_pT1*width_pT1 + i*width_pT2<< endl;
      bins_pT[nbins_pT1 + i] = min_pT + nbins_pT1*width_pT1 + i*width_pT2;
    }


  /* Create DIS histogram- one entry per event */
  const int hn_dis_ndim = 2;
  int hn_dis_nbins[] = { nbins_x,
                         nbins_Q2 };
  double hn_dis_xmin[] = {0., 0. };
  double hn_dis_xmax[] = {0., 0. };

  THnSparse* hn_dis = new THnSparseF("hn_dis_event",
                                     "DIS Kinematis Per Event; x; Q2;",
                                     hn_dis_ndim,
                                     hn_dis_nbins,
                                     hn_dis_xmin,
                                     hn_dis_xmax );

  hn_dis->GetAxis(0)->SetName("x");
  hn_dis->GetAxis(1)->SetName("Q2");

  hn_dis->SetBinEdges(0,bins_x);
  hn_dis->SetBinEdges(1,bins_Q2);

  /* Create SIDIS histogram- one entry per particle */
  const int hn_sidis_ndim = 4;
  int hn_sidis_nbins[] = { nbins_x,
                           nbins_Q2,
                           nbins_z,
			   nbins_pT };
  double hn_sidis_xmin[] = {0., 0., 0., 0. };
  double hn_sidis_xmax[] = {0., 0., 0., 0. };

  THnSparse* hn_sidis_pion_plus = new THnSparseF("hn_sidis_pion_plus",
                                                 "SIDIS Kinematis Per Particle (Postitively Charged Pion);x;Q2;z;pT;",
                                                 hn_sidis_ndim,
                                                 hn_sidis_nbins,
                                                 hn_sidis_xmin,
                                                 hn_sidis_xmax );

  hn_sidis_pion_plus->GetAxis(0)->SetName("x");
  hn_sidis_pion_plus->GetAxis(1)->SetName("Q2");
  hn_sidis_pion_plus->GetAxis(2)->SetName("z");
  hn_sidis_pion_plus->GetAxis(3)->SetName("pT");

  /* Set logarithmic bin ranges. */
  hn_sidis_pion_plus->SetBinEdges(0,bins_x);
  hn_sidis_pion_plus->SetBinEdges(1,bins_Q2);
  hn_sidis_pion_plus->SetBinEdges(2,bins_z);
  hn_sidis_pion_plus->SetBinEdges(3,bins_pT);

  /* clone SIDIS histogram for other particle identities */
  THnSparse* hn_sidis_pion_minus = (THnSparseF*)hn_sidis_pion_plus->Clone("hn_sidis_pion_minus");
  hn_sidis_pion_minus->SetTitle("SIDIS Kinematis Per Particle (Negatively Charged Pion)");

  THnSparse* hn_sidis_kaon_plus  = (THnSparseF*)hn_sidis_pion_plus->Clone("hn_sidis_kaon_plus");
  hn_sidis_kaon_plus->SetTitle("SIDIS Kinematis Per Particle (Positively Charged Kaon)");

  THnSparse* hn_sidis_kaon_minus = (THnSparseF*)hn_sidis_pion_plus->Clone("hn_sidis_kaon_minus");
  hn_sidis_kaon_minus->SetTitle("SIDIS Kinematis Per Particle (Negatively Charged Kaon)");

  /* print all bin centers for x bins */
  TH2F* hxQ2 = (TH2F*)hn_dis->Projection(1,0);

  /* Two alternative to determine bin center- which one is the CORRECT on a log scale axis?? */
  //for ( int bin_x = 1; bin_x <= hxQ2->GetNbinsX(); bin_x++ )
  //  {
  //    /* Option 1 matches eRHIC high energy document Fig 11 */
  //    fprintf( stdout, "xC = %.2e\n",
  //           hxQ2->GetXaxis()->GetBinCenter(bin_x) );
  //  }

  /* Option 2 matches HERA table */
  for ( int bin_x = 1; bin_x <= hxQ2->GetNbinsX(); bin_x++ )
    {
      fprintf( stdout, "x = %.2e\n",
               TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) ) )
                                        + ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) + hxQ2->GetXaxis()->GetBinWidth(bin_x) ) ) ) ) );
    }

  /* print bin centers for Q2 bins */
  for ( int bin_Q2 = 1; bin_Q2 <= hxQ2->GetNbinsY(); bin_Q2++ )
    {

      fprintf( stdout, "Q2 = %.2e\n",
               TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_Q2) ) )
                                        + ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_Q2) + hxQ2->GetYaxis()->GetBinWidth(bin_Q2) ) ) ) ) );
    }

  /* Loop over all events in tree. */
  unsigned max_event = tree->GetEntries();
  //unsigned max_event = 1000;
  for ( unsigned ievent = 0; ievent < max_event; ievent++ )
    {
      if ( ievent%1000 == 0 )
        cout << "Processing event " << ievent << endl;

      /* load event */
      tree->GetEntry(ievent);

      /* Scattered lepton found within acceptance? If not, continue to next event. */
      if (! accept_electron( event->ScatteredLepton()->GetEta(), event->ScatteredLepton()->GetE() ) )
        continue;

      /* Check that scattered lepton is within detector acceptance */
      //if ( ! eventS->ScatteredLepton() )
      //  continue;

      /* Cut on kinematics */
      float y = event->GetTrueY();
      if ( y > 0.95 || y < 0.01 )
        continue;

      float x = event->GetTrueX();
      float Q2 = event->GetTrueQ2();

      double fill_hn_dis[] = {x, Q2};
      hn_dis->Fill( fill_hn_dis );

      /* For SIDIS: Loop over all final state particles in this event */
      unsigned ntracks = event->GetNTracks();

      for ( unsigned itrack = 0; itrack < ntracks; itrack++ )
        {
          erhic::ParticleMC * iparticle = event->GetTrack( itrack );

          /* Check status */
          if ( iparticle->GetStatus() != 1 )
            continue;

          /* get TRUE pid */
          int pid = iparticle->Id().Code();
          //TParticlePDG *pid_pdg =  iparticle->Id().Info();

          /* Check if within PID acceptance */
          if ( ! accept_pid( iparticle->GetEta(), iparticle->GetP(), pid ) )
            continue;

          /* Get z of particle */
          float z = iparticle->GetZ();

          /* skip low z paricles- soft physics and beam remnants */
          if ( z <= 0.15 )
            continue;

	  /* Get pT of particle w.r.t. exchange boson of interaction */
	  float pT = iparticle->GetPtVsGamma();

          /* Prepare array to fill histogram */
          double fill_hn_sidis[] = {x, Q2, z, pT};

          /* Use true PID to choose which histogram to fill */
          /* Pi+ */
          if ( pid == 211 )
            hn_sidis_pion_plus->Fill( fill_hn_sidis );

          /* Pi - */
          else if ( pid == -211 )
            hn_sidis_pion_minus->Fill( fill_hn_sidis );

          /* K+ */
          else if ( pid == 321 )
            hn_sidis_kaon_plus->Fill( fill_hn_sidis );

          /* K- */
          else if ( pid == -321 )
            hn_sidis_kaon_minus->Fill( fill_hn_sidis );

	} // end loop over particles

    } // end loop over events

  /* Write histogram. */
  hn_dis->Write();

  hn_sidis_pion_plus->Write();
  hn_sidis_pion_minus->Write();
  hn_sidis_kaon_plus->Write();
  hn_sidis_kaon_minus->Write();

  h_eta->Write();
  h_eta_accept->Write();

  /* Close output file. */
  fout->Close();

  return 0;
}

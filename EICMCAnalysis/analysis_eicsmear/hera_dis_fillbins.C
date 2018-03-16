int
hera_dis_fillbins()
{
  gSystem->Load("libeicsmear");

  /* Open input files. */
  TFile *file_mc = new TFile("/gpfs/mnt/gpfs04/sphenix/user/nfeege/data/TEST/pythia.ep.27.5x820.1Mevents.RadCor=0.Q2gt1.root");

  /* Get trees from files. */
  TTree *tree = (TTree*)file_mc->Get("EICTree");

  /* Output file. */
  TFile *fout = new TFile("hera_dis_histo.root", "RECREATE");

  erhic::EventPythia *event  = NULL;

  tree->SetBranchAddress("event", &event);

  /* Create n-dimensional histogram to collect statistic for SIDIS events in kinematics bins:
   *
   * Event properties:
   * - x
   * - Q2
   */
  const int nbins_x  = 10;
  const int nbins_Q2 = 10;

  const int hn_dis_ndim = 2;
  int hn_dis_nbins[] = { nbins_x,
                         nbins_Q2 };
  double hn_dis_xmin[] = {0., 0. };
  double hn_dis_xmax[] = {0., 0. };

  THnSparse* hn_dis = new THnSparseF("hn_dis",
                                     "DIS Kinematis; x; Q2;",
                                     hn_dis_ndim,
                                     hn_dis_nbins,
                                     hn_dis_xmin,
                                     hn_dis_xmax );

  hn_dis->GetAxis(0)->SetName("x");
  hn_dis->GetAxis(1)->SetName("Q2");

  double min_x = -5;
  double max_x = 0;
  double width_x = (max_x - min_x) / nbins_x;
  double bins_x[nbins_x+1];
  for( int i =0; i <= nbins_x; i++)
    {
      cout << TMath::Power( 10, min_x + i*width_x) << endl;
      bins_x[i] = TMath::Power( 10, min_x + i*width_x);
    }

  double min_Q2 = 0;
  double max_Q2 = 5;
  double width_Q2 = (max_Q2 - min_Q2) / nbins_Q2;
  double bins_Q2[nbins_Q2+1];
  for( int i =0; i <= nbins_Q2; i++)
    bins_Q2[i] = TMath::Power( 10, min_Q2 + i*width_Q2);

  hn_dis->SetBinEdges(0,bins_x);
  hn_dis->SetBinEdges(1,bins_Q2);

  /* print all bin centers */
  TH2F* hxQ2 = (TH2F*)hn_dis->Projection(1,0);
  for ( int bin_x = 1; bin_x <= hxQ2->GetNbinsX(); bin_x++ )
    {

      for ( int bin_y = 1; bin_y <= hxQ2->GetNbinsY(); bin_y++ )
        {
	  cout << "x = " << TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) ) )
						     + ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) + hxQ2->GetXaxis()->GetBinWidth(bin_x) ) ) ) ) << endl;

	  cout << "Q2 = " << TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_y) ) )
						     + ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_y) + hxQ2->GetYaxis()->GetBinWidth(bin_y) ) ) ) ) << endl;
	}
    }

  /* Loop over all events in tree. */
  for ( unsigned ievent = 0; ievent < tree->GetEntries(); ievent++ )
    {
      if ( ievent%1000 == 0 )
        cout << "Processing event " << ievent << endl;

      /* load event */
      tree->GetEntry(ievent);

      /* Cut on scattered lepton properties */
      float Emin = 5;
      if ( event->ScatteredLepton()->GetE() < Emin )
	continue;

      if ( event->ScatteredLepton()->GetTheta() >= ( 177./ 180. * TMath::Pi() ) )
	continue;

      /* Cut on kinematics */
      float x = event->GetTrueX();
      float Q2 = event->GetTrueQ2();
      float y = event->GetTrueY();

      if ( Q2 < 1 )
	continue;

      if ( y >= 0.9 || y <= 0.01 )
	continue;


      double fill_hn_dis[] = {x, Q2};
      hn_dis->Fill( fill_hn_dis );
    }

  /* Write histogram. */
  hn_dis->Write();

  /* Close output file. */
  fout->Close();

  return 0;
}

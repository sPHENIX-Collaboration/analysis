/* STL includes */
#include <iostream>

/* ROOT includes */
#include <TROOT.h>
//#include <gSystem.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TH2F.h>
#include <THnSparse.h>

/* eicsmear includes */
#include <eicsmear/smear/EventSmear.h>
#include <eicsmear/erhic/EventPythia.h>
#include <eicsmear/erhic/ParticleMC.h>

using namespace std;

int
eic_sphenix_fill_xq2( TString filename_output,
			  TString filename_mc,
			  TString filename_mc_smeared,
			  bool debug = false )
{
  /* Uncomment this line when running without compilation */
  //  gSystem->Load("libeicsmear");

  /* Selection cuts for scattered electron (after smearing) */
  /* Minimum momentum:  this makes sure the momentum was smeared, i.e. is within tracking acceptance */
  double electron_pmin = 0.01; // GeV

  /* Minimumenergy: Acceptance cut for good electron/pion separation */
  double electron_emin = 3.0; // GeV

  /* Open input files. */
  TFile *file_mc = new TFile(filename_mc, "OPEN");
  TFile *file_mc_smeared = new TFile(filename_mc_smeared, "OPEN");

  /* Get trees from files. */
  TTree *tree = (TTree*)file_mc->Get("EICTree");
  TTree *tree_smeared = (TTree*)file_mc_smeared->Get("Smeared");


  /* Get event generator parameters (cross section, number of events, ...) from file. */
  /* Input file may include multiple cross sections because it was merged from multiple files, so loop over all crossSection
     objects and calculate average; same for sum of nEvents. */
  double xsec = 0;
  unsigned nxsec = 0;
  int nevent = 0;
  TIter next(file_mc->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next()))
    {
      if ( TString(key->GetName()) == "crossSection" )
	{
	  xsec += (TString(((TObjString*)key->ReadObj())->GetName())).Atof();
	  nxsec++;
	  continue;
	}

      if ( TString(key->GetName()) == "nEvents" )
	{
	  nevent += (TString(((TObjString*)key->ReadObj())->GetName())).Atof();
	  continue;
	}
    }

  /* Get average cross section per file */
  if ( nxsec > 0 )
    xsec /= nxsec;

  TObjString* gen_crossSection = (TObjString*)file_mc->Get("crossSection");
  TObjString* gen_nEvents = (TObjString*)file_mc->Get("nEvents");

  /* Update strings with total values */
  gen_crossSection->SetString( TString::Format("%f", xsec) );
  gen_nEvents->SetString( TString::Format("%d", nevent) );

  /* Convert cross section from microbarn (10^-6) to femtobarn (10^-15) */
  float xsec_fb = xsec * 1e9;

  /* Calculate integrated luminosity (inverse femtobarn) */
  float lumi_ifb = (float)nevent / (float)xsec_fb;

  /* Add Luminosity strings */
  TObjString* gen_lumi_ifb = new TObjString();
  gen_lumi_ifb->SetString( TString::Format("%f", lumi_ifb) );

  /* Print parameters */
  cout << "crossSection: " << gen_crossSection->GetName() << endl;
  cout << "nEvents: " << gen_nEvents->GetName() << endl;
  cout << "luminosity: " << gen_lumi_ifb->GetName() << endl;

  /* Output file. */
  TFile *file_out = new TFile(filename_output, "RECREATE");

  /* Add friend to match branches in trees. */
  tree->AddFriend(tree_smeared);

  erhic::EventPythia *event  = NULL;
  Smear::Event       *eventS = NULL;

  tree->SetBranchAddress("event", &event);
  tree->SetBranchAddress("eventS", &eventS);

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
   * - pT
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
      //cout << TMath::Power( 10, min_x + i*width_x) << endl;
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
  const int nbins_z  = 20;

  double min_z = 0.0;
  double max_z = 1.0;
  double width_z = (max_z - min_z) / nbins_z;
  double bins_z[nbins_z+1];
  for( int i =0; i <= nbins_z; i++)
    {
      //cout << min_z + i*width_z << endl;
      bins_z[i] = min_z + i*width_z;
    }

  /* pT */
  const int nbins_pT1 = 5;
  const int nbins_pT2 = 4;
  const int nbins_pT = nbins_pT1 + nbins_pT2;

  double min_pT = 0.0;
  double width_pT1 = 0.2;
  double width_pT2 = 1.0;

  double bins_pT[nbins_pT+1];
  for( int i =0; i <= nbins_pT1; i++)
    {
      //cout << min_pT + i*width_pT1 << endl;
      bins_pT[i] = min_pT + i*width_pT1;
    }
  for( int i =1; i <= nbins_pT2; i++)
    {
      //cout << min_pT + nbins_pT1*width_pT1 + i*width_pT2<< endl;
      bins_pT[nbins_pT1 + i] = min_pT + nbins_pT1*width_pT1 + i*width_pT2;
    }


  /* Create DIS histogram- one entry per event */
  const int hn_dis_ndim = 2;
  int hn_dis_nbins[] = { nbins_x,
                         nbins_Q2 };
  double hn_dis_xmin[] = {0., 0. };
  double hn_dis_xmax[] = {0., 0. };

  THnSparse* hn_dis = new THnSparseF("hn_dis_electron",
                                     "DIS Kinematis Per Event (Electron); x; Q2;",
                                     hn_dis_ndim,
                                     hn_dis_nbins,
                                     hn_dis_xmin,
                                     hn_dis_xmax );

  hn_dis->GetAxis(0)->SetName("x");
  hn_dis->GetAxis(1)->SetName("Q2");

  hn_dis->SetBinEdges(0,bins_x);
  hn_dis->SetBinEdges(1,bins_Q2);

  /* clone histogram for ACCEPTED events */
  THnSparse* hn_dis_accept = (THnSparse*)hn_dis->Clone("hn_dis_electron_accept");
  hn_dis_accept->SetTitle("DIS Kinematis Per Event (Accepted)");

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

  /* clone histogram for ACCEPTED events */
  THnSparse* hn_sidis_pion_plus_accept  = (THnSparse*)hn_sidis_pion_plus->Clone("hn_sidis_pion_plus_accept");
  hn_sidis_pion_plus_accept->SetTitle("SIDIS Kinematis Per Particle (Positively Charged Pion, Accepted)");

  THnSparse* hn_sidis_pion_minus_accept = (THnSparseF*)hn_sidis_pion_plus->Clone("hn_sidis_pion_minus_accept");
  hn_sidis_pion_minus_accept->SetTitle("SIDIS Kinematis Per Particle (Negatively Charged Pion, Accepted)");

  THnSparse* hn_sidis_kaon_plus_accept  = (THnSparseF*)hn_sidis_pion_plus->Clone("hn_sidis_kaon_plus_accept");
  hn_sidis_kaon_plus_accept->SetTitle("SIDIS Kinematis Per Particle (Positively Charged Kaon, Accepted)");

  THnSparse* hn_sidis_kaon_minus_accept = (THnSparseF*)hn_sidis_pion_plus->Clone("hn_sidis_kaon_minus_accept");
  hn_sidis_kaon_minus_accept->SetTitle("SIDIS Kinematis Per Particle (Negatively Charged Kaon, Accepted)");

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
  if ( debug )
    {
      for ( int bin_x = 1; bin_x <= hxQ2->GetNbinsX(); bin_x++ )
	{
	  fprintf( stdout, "x = %.2e\n",
		   TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) ) )
					    + ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) + hxQ2->GetXaxis()->GetBinWidth(bin_x) ) ) ) ) );
	}
    }

  /* print bin centers for Q2 bins */
  if ( debug )
    {
      for ( int bin_Q2 = 1; bin_Q2 <= hxQ2->GetNbinsY(); bin_Q2++ )
	{

	  fprintf( stdout, "Q2 = %.2e\n",
		   TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_Q2) ) )
					    + ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_Q2) + hxQ2->GetYaxis()->GetBinWidth(bin_Q2) ) ) ) ) );
	}
    }

  /* Loop over all events in tree. */
  unsigned max_event = tree->GetEntries();

  if ( debug )
    max_event = 10000;

  for ( unsigned ievent = 0; ievent < max_event; ievent++ )
    {
      if ( ievent%1000 == 0 )
        cout << "Processing event " << ievent << endl;

      /* load event */
      tree->GetEntry(ievent);

      /* Cut on EVENT kinematics */
      float y = event->GetTrueY();
      if ( y > 0.95 || y < 0.01 )
        continue;

      float x = event->GetTrueX();
      float Q2 = event->GetTrueQ2();

      double fill_hn_dis[] = {x, Q2};
      hn_dis->Fill( fill_hn_dis );

      /* Scattered lepton found within acceptance of traacking system (= valid smeared momentum P)? */
      bool electron_detected = false;
      if ( eventS->ScatteredLepton() && eventS->ScatteredLepton()->GetP() > electron_pmin && eventS->ScatteredLepton()->GetE() > electron_emin )
	electron_detected = true;

      /* Continue if electron not detected */
      if ( !electron_detected )
	continue;

      /* execute the part below only if scattered electron within acceptance */
      hn_dis_accept->Fill( fill_hn_dis );


      /* For SIDIS: Loop over all final state particles in this event */
      unsigned ntracks = event->GetNTracks();

      for ( unsigned itrack = 0; itrack < ntracks; itrack++ )
        {
          erhic::ParticleMC  * iparticle = event->GetTrack( itrack );
          Smear::ParticleMCS * ismeared  = eventS->GetTrack( itrack );

          /* Check status */
          if ( iparticle->GetStatus() != 1 )
            continue;

          /* Get z of particle */
          float z = iparticle->GetZ();

          /* skip low z paricles- soft physics and beam remnants */
          if ( z <= 0.15 )
            continue;

	  /* skip particles outside +/- 4 pseudorapidity */
	  if ( iparticle->GetEta() < -4 || iparticle->GetEta() > 4 )
	    continue;

	  /* Get pT of particle w.r.t. exchange boson of interaction */
	  float pT = iparticle->GetPtVsGamma();

          /* Prepare array to fill histogram */
          double fill_hn_sidis[] = {x, Q2, z, pT};

          /* get TRUE pid */
          int pid = iparticle->Id().Code();
          //TParticlePDG *pid_pdg =  iparticle->Id().Info();

          /* Use true PID to choose which histogram to fill */
          /* Pi+ */
          if ( pid == 211 )
	    {
	      hn_sidis_pion_plus->Fill( fill_hn_sidis );

	      if ( ismeared->GetP() > 0.1 && ismeared->Id().Code() == 211 )
		hn_sidis_pion_plus_accept->Fill( fill_hn_sidis );
	    }

          /* Pi - */
          else if ( pid == -211 )
	    {
	      hn_sidis_pion_minus->Fill( fill_hn_sidis );

	      if ( ismeared->GetP() > 0.1 && ismeared->Id().Code() == -211 )
		hn_sidis_pion_minus_accept->Fill( fill_hn_sidis );
	    }

          /* K+ */
          else if ( pid == 321 )
	    {
	      hn_sidis_kaon_plus->Fill( fill_hn_sidis );

	      if ( ismeared->GetP() > 0.1 && ismeared->Id().Code() == 321 )
		hn_sidis_kaon_plus_accept->Fill( fill_hn_sidis );
	    }

          /* K- */
          else if ( pid == -321 )
	    {
	      hn_sidis_kaon_minus->Fill( fill_hn_sidis );

	      if ( ismeared->GetP() > 0.1 && ismeared->Id().Code() == -321 )
		hn_sidis_kaon_minus_accept->Fill( fill_hn_sidis );
	    }
	} // end loop over particles

    } // end loop over events

  /* Write histogram. */
  hn_dis->Write();

  hn_dis_accept->Write();

  hn_sidis_pion_plus->Write();
  hn_sidis_pion_minus->Write();
  hn_sidis_kaon_plus->Write();
  hn_sidis_kaon_minus->Write();

  hn_sidis_pion_plus_accept->Write();
  hn_sidis_pion_minus_accept->Write();
  hn_sidis_kaon_plus_accept->Write();
  hn_sidis_kaon_minus_accept->Write();

  h_eta->Write();
  h_eta_accept->Write();

  gen_crossSection->Write("crossSection");
  gen_nEvents->Write("nEvents");
  //gen_nTrials->Write("nTrials");
  gen_lumi_ifb->Write("luminosity");

  /* Close output file. */
  file_out->Close();

  return 0;
}


/* MAIN function */
int main( int argc , char* argv[] )
{
  if ( argc < 4 || argc > 5 )
    {
      cout << "Usage: " << argv[0] << " <filename_output> <filename_EICTree> <filename_EICTree_smeared> <optional: 'debug' for debug mode" << endl;
      return 1;
    }

  cout << "Running eic_sphenix_fill_xq2 with: \n" << endl;
  cout << " - Output file:            " << argv[1] << endl;
  cout << " - EICTree input file:     " << argv[2] << endl;

  if ( argc == 4 )
    {
      cout << " - EICTree (smeared) file: " << argv[3] << endl;
      eic_sphenix_fill_xq2( argv[1], argv[2], argv[3] );
    }
  else if ( argc == 5 )
    {
      cout << " - EICTree (smeared) file: " << argv[3] << endl;
      cout << " ==== DEBUG MODE ==== " << endl;
      eic_sphenix_fill_xq2( argv[1], argv[2], argv[3], true );
    }

  return 0;
}

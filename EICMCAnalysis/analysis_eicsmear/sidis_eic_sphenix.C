int
sidis_eic_sphenix()
{
  gSystem->Load("libeicsmear");

  TFile *file_mc = new TFile("data/pythiaeRHIC/TREES/pythia.ep.20x250.RadCor=0.Q2gt1.50kevts.root");
  TTree *tree = (TTree*)file_mc->Get("EICTree");

  TFile *file_mc_smeared = new TFile("data/pythiaeRHIC/TREES/pythia.ep.20x250.RadCor=0.Q2gt1.50kevts.smear.root");
  TTree *tree_smeared = (TTree*)file_mc_smeared->Get("Smeared");

  tree->AddFriend(tree_smeared);

  erhic::EventPythia *event  = NULL;
  Smear::Event       *eventS = NULL;

  tree->SetBranchAddress("event", &event);
  tree->SetBranchAddress("eventS", &eventS);

  /* fill histogram with eta of particles to check acceptance of detectors */
  TH1F* h_eta = new TH1F("h_eta", ";#eta;dN/d#eta", 100, -5, 5);
  TH1F* h_eta_accept = (TH1F*)h_eta->Clone("h_eta_accept");
  h_eta_accept->SetLineColor(kGreen);

  /* loop over all events in tree */
  for ( unsigned ievent = 0; ievent < tree->GetEntries(); ievent++ )
    {
      /* load event */
      tree->GetEntry(ievent);

      /* loop over all particles in events */
      for ( unsigned jtrack = 0; jtrack < event->GetNTracks(); jtrack++ )
	{
	  erhic::ParticleMC* particle_truth = event->GetTrack( jtrack );

	  if ( particle_truth->GetStatus() == 1 )
	    {
	      h_eta->Fill( particle_truth->GetEta() );

	      Smear::ParticleMCS* particle_smeared = eventS->GetTrack( jtrack );

	      /* check that smeared particle exist */
	      if ( particle_smeared )
		h_eta_accept->Fill( particle_truth->GetEta() );
	    }
	}

    }

  TCanvas *c1 = new TCanvas();
  h_eta->Draw();
  h_eta_accept->Draw("sames");

  return 0;
}

// A quick look at the momentum vs pseudorapidity of the unsmeared particles
// Currently loads both smeared and unsmeared input files for later use
// Only unsmeared input file used right now

// --------------------------

/* STL includes */
#include <iostream>

/* ROOT includes */
#include <TROOT.h>
//#include <gSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <THnSparse.h>

/* eicsmear includes */
//#include <eicsmear/erhic/EventPythia.h>
//#include <eicsmear/erhic/ParticleMC.h>

using namespace std;

int
eic_unsmeared_eta_p( TString filename_output,
			  TString filename_mc,
			  TString filename_mc_smeared = "",
			  bool debug = false )
{
  /* Uncomment this line when running without compilation */
  gSystem->Load("libeicsmear");

  /* Open input files. */
  TFile *file_mc = new TFile(filename_mc, "OPEN");
  TFile *file_mc_smeared = new TFile(filename_mc_smeared, "OPEN");

  /* Get trees from files. */
  TTree *tree = (TTree*)file_mc->Get("EICTree");
  TTree *tree_smeared = (TTree*)file_mc_smeared->Get("Smeared");

  /* Output file. */
  TFile *file_out = new TFile(filename_output, "RECREATE");

  /* Add friend to match branches in trees. */
  tree->AddFriend(tree_smeared);

  erhic::EventPythia *event  = NULL;
  Smear::Event       *eventS = NULL;

  tree->SetBranchAddress("event", &event);
  tree->SetBranchAddress("eventS", &eventS);

  /* Create histogram for eta versus momentum. */
  TH2F* h_eta = new TH2F("h_eta","Momentum vs. Pseudorapidity",80,-4,4,45,-5,40 );
  TH2F* h_eta_accept = (TH2F*)h_eta->Clone("h_eta_accept");
  h_eta->GetXaxis()->SetTitle("#eta");
  h_eta->GetYaxis()->SetTitle("Momentum (GeV)");

  /* Create canvas */
  TCanvas *c1 = new TCanvas;
  h_eta->Draw("AXIS");

  /* Loop over all events in tree. */
  unsigned max_event = tree->GetEntries();

  for ( unsigned ievent = 0; ievent < max_event; ievent++ )
    {
      if ( ievent%1000 == 0 )
        cout << "Processing event " << ievent << endl;

      /* load event */
      tree->GetEntry(ievent);

      /* Cut on EVENT kinematics */
      float y = event->GetTrueY();
      if ( y > 0.99 || y < 0.01 )
        continue;

      float eta = event->ScatteredLepton()->GetEta();
      float p = event->ScatteredLepton()->GetP();

      h_eta->Fill(eta,p);

    } // end loop over events

  /* Write histogram. */

  h_eta->Write();
  h_eta_accept->Write();

  /*draw histogram */

  h_eta->Draw("COLZ");
  h_eta->Draw("sameaxis");
  return c1;

  /* Close output file. */
  file_out->Close();

  return 0;
}

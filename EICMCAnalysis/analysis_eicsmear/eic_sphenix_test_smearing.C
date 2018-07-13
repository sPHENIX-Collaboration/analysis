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
eic_sphenix_test_smearing( TString filename_output,
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
  //TFile *file_out = new TFile(filename_output, "RECREATE");

  /* Add friend to match branches in trees. */
  tree->AddFriend(tree_smeared);

  erhic::EventPythia *event  = NULL;
  Smear::Event       *eventS = NULL;

  tree->SetBranchAddress("event", &event);
  tree->SetBranchAddress("eventS", &eventS);

  /* Create histogram */
  TH2F* h_e_smeared_vs_eta = new TH2F("h_e_smeared_vs_eta","Energy Smeared vs True Pseudorapidity",100,-5,5,70,0,35);
  h_e_smeared_vs_eta->GetXaxis()->SetTitle("#eta_{true}");
  h_e_smeared_vs_eta->GetYaxis()->SetTitle("E_{smeared} (GeV)");

  TH2F* h_e_smeared_vs_true = new TH2F("h_e_smeared_vs_true","Energy Smeared vs True",60,0,30,70,0,35);
  h_e_smeared_vs_true->GetXaxis()->SetTitle("E_{true} (GeV)");
  h_e_smeared_vs_true->GetYaxis()->SetTitle("E_{smeared} (GeV)");

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

      float energy = event->ScatteredLepton()->GetE();
      float energy_smeared = eventS->ScatteredLepton()->GetE();

      float eta = event->ScatteredLepton()->GetEta();

      /* Fill histograms */
      h_e_smeared_vs_eta->Fill(eta,energy_smeared);
      h_e_smeared_vs_true->Fill(energy,energy_smeared);

    } // end loop over events

  float underflow = tree->GetEntry(0);
  float overflow = tree->GetEntry(max_event + 1);
  cout << "underflow: " << underflow << endl;
  cout << "overflow: " << overflow << endl;

  /* Write histograms. */
  //h_e_smeared_vs_true->Write();
  //h_e_smeared_vs_eta->Write();

  /*draw histograms */
  TCanvas *c1 = new TCanvas();
  h_e_smeared_vs_true->DrawClone("COLZ");
  gPad->RedrawAxis();

  TCanvas *c2 = new TCanvas();
  h_e_smeared_vs_eta->DrawClone("COLZ");
  gPad->RedrawAxis();

  /* Close output file. */
  //file_out->Close();

  return 0;
}

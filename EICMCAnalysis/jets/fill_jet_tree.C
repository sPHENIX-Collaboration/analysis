
#include "fastjet/ClusterSequence.hh"
//#include "fastjet/ClusterSequenceArea.hh"

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLorentzVector.h"
#include "TLorentzRotation.h"

#include "eicsmear/erhic/EventBase.h"
#include "eicsmear/erhic/EventPythia.h"
#include "eicsmear/erhic/Particle.h"

using namespace fastjet;
using namespace std;


int main(int argc, char* argv[]) {

  cout << "Hello World!" << endl;

  //const int nevents = atoi(argv[1]);
  //const int beginEvent = atoi(argv[2]);
  //const int endEvent = atoi(argv[3]);
  //const char* inFileName = argv[4];
  //const char* outFileName = argv[5];
  //
  //cout << "nevents = " << nevents << endl;
  //cout << "Process Events " << beginEvent << " - " << endEvent << endl;
  //cout << "inFileName = " << inFileName << endl;
  //cout << "outFileName = " << outFileName << endl;

  const int nevents = 10;
  const int beginEvent = 0;
  const int endEvent = 1e9;

  const char* inFileName = "data/pythiaeRHIC/TREES/pythia.ep.20x250.RadCor=0.Q2gt1.50kevts.root";
  //const char* inFileName = "data/TEST/LQGENEP_output.100000event.root";
  const char* outFileName = "jettest.root";

  // Chain for Simu Tree
  TChain* inTree = new TChain("EICTree");
  inTree->Add(inFileName);

  // Setup Input Event Buffer
  erhic::EventPythia* inEvent(NULL);
  inTree->SetBranchAddress("event",&inEvent);

   // Open Output File
   TFile *ofile = TFile::Open(outFileName,"recreate");
   assert(ofile);

   // Create Jet Tree
   TTree *mTree = new TTree("jets","Jet Tree");

   // Set Jet Tree Structure
   int njets = 0;
   std::vector< float > jetminv;
   mTree->Branch( "njets",    &njets );
   mTree->Branch( "jet_minv", &jetminv );

  // Loop Over Events in Simu Trees
  for(int iEvent=0; iEvent<inTree->GetEntries(); iEvent++)
    {
      /* reset tree variables */
      njets = 0;
      jetminv.clear();

      //Read Next Event
      //inTree->GetEntry(iEvent);
      if(inTree->GetEntry(iEvent) <=0) break;

      if(iEvent%10000 == 0) cout << "Event " << iEvent << endl;

      if((iEvent < beginEvent) || (iEvent >= endEvent)) continue;

      //cout << "Event " << iEvent << " Particles = " << inEvent->GetNTracks() << endl;

      // Create laboratory frame jet vectors
      vector<PseudoJet> particlesPt;
      vector<bool> isElectron;
      vector<bool> isFromTau;

      // Loop over Particles
      for(int j=0; j<inEvent->GetNTracks(); j++)
	{
	  const Particle* inParticle = inEvent->GetTrack(j);

	  Double_t px = inParticle->GetPx();
	  Double_t py = inParticle->GetPy();
	  Double_t pz = inParticle->GetPz();
	  Double_t E = inParticle->GetE();

	  // Select Particles for Jets
	  //if(j>10 && inParticle->GetStatus() == 1 && inParticle->GetParentIndex() != 3)
	  if(inParticle->GetStatus() == 1)
	    {
	      if( abs(inParticle->GetEta()) <= 4 && inParticle->GetPt() >= 0.250)
		{
		  fastjet::PseudoJet pPt(px,py,pz,E);
		  pPt.set_user_index(inParticle->GetIndex());
		  particlesPt.push_back(pPt);

		  /* is particle electron? */
		  isElectron.push_back( inParticle->GetPdgCode() == 11 );

		  /* is particle child of TAU? */
		  if ( inParticle->GetParent() )
		    {
		      isFromTau.push_back( inParticle->GetParent()->GetPdgCode() == 15 );
		    }
		  /* if prticle has no parent, parent can't be tau */
		  else
		    isFromTau.push_back( false );
		}
	    }
	}

      // Set Jet Definitions
      double R_10 = 1.0;
      JetDefinition jet_def_akt_10(antikt_algorithm,R_10);

      // Run Clustering and Extract the Jets
      double ptmin = 1.0;

      // Cluster in Lab Frame
      ClusterSequence csPt_akt_10(particlesPt, jet_def_akt_10);

      // Lab Frame Jets
      vector<PseudoJet> jetsPt_akt_10 = sorted_by_pt(csPt_akt_10.inclusive_jets(ptmin));

      // loop over jets
      njets =  jetsPt_akt_10.size();
      for ( unsigned i = 0; i < jetsPt_akt_10.size(); i++ )
	{
	  jetminv.push_back( jetsPt_akt_10.at(i).m() );
	  //cout << "Jet " << i << ": " << jetsPt_akt_10.at(i).m() << " -- " << jetsPt_akt_10.at(i).has_constituents() << endl;
	}

//// loop over particles
//int npart = particlesPt.size();
//cout << "# particles = " << npart << endl;
//for ( unsigned i = 0; i < particlesPt.size(); i++ )
//	{
//	  cout << "Particle " << i << ": is e? " << isElectron.at(i) << " -- is from tau?" << isFromTau.at(i) << endl;
//	}

      mTree->Fill();
    }

  ofile->Write();
  ofile->Close();
}

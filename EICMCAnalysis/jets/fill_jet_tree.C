#include "fastjet/ClusterSequence.hh"

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "TTree.h"
#include "TFile.h"

#include "eicsmear/erhic/EventBase.h"
#include "eicsmear/erhic/EventPythia.h"
#include "eicsmear/erhic/Particle.h"
#include "eicsmear/erhic/ParticleMC.h"
#include "eicsmear/smear/EventSmear.h"
#include "eicsmear/smear/ParticleMCS.h"

using namespace fastjet;
using namespace std;


/** Find jet in given vector of jets that's closest in eta-phi to reference jet
 */
PseudoJet* find_matching_jet( const PseudoJet* refjet, vector<PseudoJet>* vjets )
{
  /* PidCandidate with eta, phi closest to reference */
  PseudoJet* best_candidate = NULL;

  float eta_ref = refjet->eta();
  float phi_ref = refjet->phi();

  /* find jet with smallest delta_R */
  float min_delta_R = 100;
  vector<PseudoJet>::iterator min_delta_R_iter = vjets->end();

  vector<PseudoJet>::iterator ijet;

  for ( ijet = vjets->begin(); ijet != vjets->end(); ijet++ )
    {
      float eta = ijet->eta();
      float phi = ijet->phi();

      float delta_R = sqrt( pow( eta - eta_ref, 2 ) + pow( phi - phi_ref, 2 ) );

      if ( delta_R < min_delta_R )
        {
          min_delta_R_iter = ijet;            ;
          min_delta_R = delta_R;
        }
    }

  /* set best_candidate to PidCandiate with smallest delta_R within reasonable range*/
  if ( min_delta_R_iter != vjets->end() && min_delta_R < 0.5 )
    best_candidate = &(*min_delta_R_iter);

  return best_candidate;
}


/** Main function
 */
int main(int argc, char* argv[]) {

  const char* inFileName = argv[1];
  const char* inFileSName = argv[2];
  const char* outFileName = argv[3];

  cout << "inFileName = " << inFileName << endl;
  cout << "inFileSName = " << inFileSName << endl;
  cout << "outFileName = " << outFileName << endl;

  // Chain for Simu Tree
  TFile *file_mc = new TFile(inFileName, "OPEN");
  TFile *file_mc_smeared = new TFile(inFileSName, "OPEN");

  TTree* tree = (TTree*)file_mc->Get("EICTree");
  TTree *tree_smeared = (TTree*)file_mc_smeared->Get("Smeared");

  tree->AddFriend(tree_smeared);

  // Setup Input Event Buffer
  erhic::EventPythia* inEvent(NULL);
  Smear::Event* eventS(NULL);

  tree->SetBranchAddress("event",&inEvent);
  tree->SetBranchAddress("eventS", &eventS);

  // Open Output File
  TFile *ofile = TFile::Open(outFileName,"recreate");
  assert(ofile);

  // Create Jet Tree
  int   _event_id    = -999;
  int   _event_njets = -999;
  float _event_x     = NAN;
  float _event_q2    = NAN;

  int   _jet_truth_id          = -999;
  int   _jet_truth_is_electron = -999;
  int   _jet_truth_is_fromtau  = -999;
  int   _jet_truth_ncomp       = -999;
  int   _jet_truth_ncharged    = -999;
  float _jet_truth_e           = NAN;
  float _jet_truth_et          = NAN;
  float _jet_truth_eta         = NAN;
  float _jet_truth_phi         = NAN;
  float _jet_truth_minv        = NAN;
  float _jet_truth_eem         = NAN;
  float _jet_truth_rvtx        = NAN;
  float _jet_truth_rmean       = NAN;
  float _jet_truth_rstdev      = NAN;

  int   _jet_smear_id          = -999;
  int   _jet_smear_ncomp       = -999;
  int   _jet_smear_ncharged    = -999;
  float _jet_smear_e           = NAN;
  float _jet_smear_et          = NAN;
  float _jet_smear_eta         = NAN;
  float _jet_smear_phi         = NAN;
  float _jet_smear_minv        = NAN;
  float _jet_smear_eem         = NAN;
  float _jet_smear_rvtx        = NAN;
  float _jet_smear_rmean       = NAN;
  float _jet_smear_rstdev      = NAN;

  TTree *mTree = new TTree("jets","Jet Tree");
  mTree->Branch("event_id", &_event_id);
  mTree->Branch("event_njets", &_event_njets);
  mTree->Branch("event_x", &_event_x);
  mTree->Branch("event_q2", &_event_q2);

  mTree->Branch("jet_truth_id", &_jet_truth_id);
  mTree->Branch("jet_truth_is_electron", &_jet_truth_is_electron);
  mTree->Branch("jet_truth_is_fromtau", &_jet_truth_is_fromtau);
  mTree->Branch("jet_truth_ncomp", &_jet_truth_ncomp);
  mTree->Branch("jet_truth_ncharged", &_jet_truth_ncharged);
  mTree->Branch("jet_truth_e", &_jet_truth_e);
  mTree->Branch("jet_truth_et", &_jet_truth_et);
  mTree->Branch("jet_truth_eta", &_jet_truth_eta);
  mTree->Branch("jet_truth_phi", &_jet_truth_phi);
  mTree->Branch("jet_truth_minv", &_jet_truth_minv);
  mTree->Branch("jet_truth_eem", &_jet_truth_eem);
  mTree->Branch("jet_truth_rvtx", &_jet_truth_rvtx);
  mTree->Branch("jet_truth_rmean", &_jet_truth_rmean);
  mTree->Branch("jet_truth_rstdev", &_jet_truth_rstdev);

  mTree->Branch("jet_smear_id", &_jet_smear_id);
  mTree->Branch("jet_smear_ncomp", &_jet_smear_ncomp);
  mTree->Branch("jet_smear_ncharged", &_jet_smear_ncharged);
  mTree->Branch("jet_smear_e", &_jet_smear_e);
  mTree->Branch("jet_smear_et", &_jet_smear_et);
  mTree->Branch("jet_smear_eta", &_jet_smear_eta);
  mTree->Branch("jet_smear_phi", &_jet_smear_phi);
  mTree->Branch("jet_smear_minv", &_jet_smear_minv);
  mTree->Branch("jet_smear_eem", &_jet_smear_eem);
  mTree->Branch("jet_smear_rvtx", &_jet_smear_rvtx);
  mTree->Branch("jet_smear_rmean", &_jet_smear_rmean);
  mTree->Branch("jet_smear_rstdev", &_jet_smear_rstdev);

  // Loop Over Events in Simu Trees
  for(int iEvent=0; iEvent<tree->GetEntries(); iEvent++)
    {
      //Read Next Event
      if(tree->GetEntry(iEvent) <=0) break;

      if(iEvent%10000 == 0) cout << "Event " << iEvent << endl;

      /* reset tree variables */
      _event_id = iEvent;
      _event_njets = 0;
      _event_x     = NAN;
      _event_q2    = NAN;

      // Create laboratory frame jet vectors
      vector<PseudoJet> particlesPtS;
      vector<PseudoJet> particlesPt;
      vector<bool> isElectron;
      vector<bool> isFromTau;

      // Loop over truth Particles
      for(int j=0; j<inEvent->GetNTracks(); j++)
	{
	  const Particle* inParticle = inEvent->GetTrack(j);

	  Double_t px = inParticle->GetPx();
	  Double_t py = inParticle->GetPy();
	  Double_t pz = inParticle->GetPz();
	  Double_t E = inParticle->GetE();

	  // Select Particles for Jets
	  if(j>10 && inParticle->GetStatus() == 1 && inParticle->GetParentIndex() != 3)
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

      // Loop over SMEARED Particles
      for(int js=0; js<eventS->GetNTracks(); js++)
	{
	  const Smear::ParticleMCS* inParticle = eventS->GetTrack(js);

	  if ( !inParticle)
	    continue;

	  Double_t px = inParticle->GetPx();
	  Double_t py = inParticle->GetPy();
	  Double_t pz = inParticle->GetPz();
	  Double_t E = inParticle->GetE();

	  // Select Particles for Jets
	  if(js>10 && inParticle->GetStatus() == 1 && inParticle->GetParentIndex() != 3)
	    {
	      if( abs(inParticle->GetEta()) <= 4 && inParticle->GetPt() >= 0.250)
	  	{
	  	  fastjet::PseudoJet pPt(px,py,pz,E);
	  	  //pPt.set_user_index(j);
	  	  particlesPtS.push_back(pPt);
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
      ClusterSequence csPtS_akt_10(particlesPtS, jet_def_akt_10);

      // Lab Frame Jets
      vector<PseudoJet> jetsPt_akt_10 = sorted_by_pt(csPt_akt_10.inclusive_jets(ptmin));
      vector<PseudoJet> jetsPtS_akt_10 = sorted_by_pt(csPtS_akt_10.inclusive_jets(ptmin));

      // loop over SMEARED jets
      _event_njets =  jetsPtS_akt_10.size();
      for ( unsigned ijet = 0; ijet < _event_njets; ijet++ )
	{
	  PseudoJet* jetMatch = find_matching_jet( &(jetsPtS_akt_10.at(ijet)), &jetsPt_akt_10 );

	  /* Set SMEARED jet variables */
	  _jet_smear_id          = ijet;
	  _jet_smear_ncomp       = -999;
	  _jet_smear_ncharged    = -999;
	  _jet_smear_e           = jetsPtS_akt_10.at(ijet).E();
	  _jet_smear_et          = jetsPtS_akt_10.at(ijet).Et();
	  _jet_smear_eta         = jetsPtS_akt_10.at(ijet).eta();
	  _jet_smear_phi         = jetsPtS_akt_10.at(ijet).phi_std();
	  _jet_smear_minv        = jetsPtS_akt_10.at(ijet).m();
	  _jet_smear_eem         = NAN;
	  _jet_smear_rvtx        = NAN;
	  _jet_smear_rmean       = NAN;
	  _jet_smear_rstdev      = NAN;

	  /* Set TRUTH jet variables */
	  _jet_truth_id          = -999;
	  _jet_truth_is_electron = -999;
	  _jet_truth_is_fromtau  = -999;
	  _jet_truth_ncomp       = -999;
	  _jet_truth_ncharged    = -999;
	  _jet_truth_e           = NAN;
	  _jet_truth_et          = NAN;
	  _jet_truth_eta         = NAN;
	  _jet_truth_phi         = NAN;
	  _jet_truth_minv        = NAN;
	  _jet_truth_eem         = NAN;
	  _jet_truth_rvtx        = NAN;
	  _jet_truth_rmean       = NAN;
	  _jet_truth_rstdev      = NAN;

	  if ( jetMatch )
	    {
	      _jet_truth_id          = ijet;
	      _jet_truth_is_electron = -999;
	      _jet_truth_is_fromtau  = -999;
	      _jet_truth_ncomp       = -999;
	      _jet_truth_ncharged    = -999;
	      _jet_truth_e           = jetMatch->E();
	      _jet_truth_et          = jetMatch->Et();
	      _jet_truth_eta         = jetMatch->eta();
	      _jet_truth_phi         = jetMatch->phi_std();
	      _jet_truth_minv        = jetMatch->m();
	      _jet_truth_eem         = NAN;
	      _jet_truth_rvtx        = NAN;
	      _jet_truth_rmean       = NAN;
	      _jet_truth_rstdev      = NAN;
	    }

	  /* Fill tree */
	  mTree->Fill();
	}
    }

  /* Write output tree and close file */
  ofile->Write();
  ofile->Close();
}

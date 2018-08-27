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

  if ( argc != 6 )
    {
      cerr << "Wrong number of arguments! Exit." << endl;
      return -1;
    }

  const char* truthFileName = argv[1];
  const char* smearFileName = argv[2];
  const char* outFileName = argv[3];

  /* R value for fastjet algorithm */
  const float fastjetR = atof(argv[4]);

  /* minimum pt for jets that are kept for analysis */
  const float ptmin = atof(argv[5]);

  cout << "truthFileName = " << truthFileName << endl;
  cout << "smearFileName = " << smearFileName << endl;
  cout << "outFileName = " << outFileName << endl;
  cout << "fastjetR      = " << fastjetR << endl;
  cout << "ptmin         = " << ptmin << endl;

  /* Open files and retrieve trees */
  TFile *file_mc_truth = new TFile(truthFileName, "OPEN");
  TFile *file_mc_smeared = new TFile(smearFileName, "OPEN");

  TTree* tree_truth = (TTree*)file_mc_truth->Get("EICTree");
  TTree *tree_smeared = (TTree*)file_mc_smeared->Get("Smeared");

  tree_truth->AddFriend(tree_smeared);

  /* Setup Input Event Buffer */
  erhic::EventPythia* event_truth(NULL);
  Smear::Event* event_smear(NULL);

  tree_truth->SetBranchAddress("event",&event_truth);
  tree_truth->SetBranchAddress("eventS", &event_smear);

  /* Set more buffers */
  Double_t event_x = NAN;
  Double_t event_q2 = NAN;

  tree_truth->SetBranchAddress("trueX",&event_x);
  tree_truth->SetBranchAddress("trueQ2",&event_q2);

  /* Open Output File */
  TFile *ofile = TFile::Open(outFileName,"recreate");
  assert(ofile);

  /* Create Jet Tree */
  Int_t   _event_id    = -999;
  Int_t   _event_njets = -999;
  Double_t _event_truth_x     = NAN;
  Double_t _event_truth_q2    = NAN;

  Int_t   _jet_truth_id          = -999;
  Int_t   _jet_truth_ncomp       = -999;
  Int_t   _jet_truth_ncharged    = -999;
  Double_t _jet_truth_e           = NAN;
  Double_t _jet_truth_et          = NAN;
  Double_t _jet_truth_eta         = NAN;
  Double_t _jet_truth_phi         = NAN;
  Double_t _jet_truth_minv        = NAN;
  Double_t _jet_truth_eem         = NAN;
  Double_t _jet_truth_rvtx        = NAN;
  Double_t _jet_truth_rmean       = NAN;
  Double_t _jet_truth_rstdev      = NAN;

  Int_t   _jet_smear_id          = -999;
  Int_t   _jet_smear_ncomp       = -999;
  Int_t   _jet_smear_ncharged    = -999;
  Double_t _jet_smear_e           = NAN;
  Double_t _jet_smear_et          = NAN;
  Double_t _jet_smear_eta         = NAN;
  Double_t _jet_smear_phi         = NAN;
  Double_t _jet_smear_minv        = NAN;
  Double_t _jet_smear_eem         = NAN;
  Double_t _jet_smear_rvtx        = NAN;
  Double_t _jet_smear_rmean       = NAN;
  Double_t _jet_smear_rstdev      = NAN;

  /* Create jet tree */
  TTree *mTree = new TTree("jets","Jet Tree");
  mTree->Branch("event_id", &_event_id);
  mTree->Branch("event_njets", &_event_njets);
  mTree->Branch("event_truth_x", &_event_truth_x);
  mTree->Branch("event_truth_q2", &_event_truth_q2);

  mTree->Branch("jet_truth_id", &_jet_truth_id);
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

  /* Loop Over Events in Simu Trees */
  for(Int_t iEvent=0; iEvent<tree_truth->GetEntries(); iEvent++)
    {
      /* Read Next Event */
      if(tree_truth->GetEntry(iEvent) <=0) break;

      if(iEvent%10000 == 0) cout << "Event " << iEvent << endl;

      /* reset tree variables */
      _event_id = iEvent;
      _event_njets = 0;
      _event_truth_x = event_x;
      _event_truth_q2 = event_q2;

      /* Create laboratory frame jet vectors */
      vector<PseudoJet> jetcomponent_truth;
      vector<PseudoJet> jetcomponent_smear;

      /* Loop over truth Particles, fill jet component vector */
      for(Int_t j=0; j<event_truth->GetNTracks(); j++)
	{
	  const Particle* inParticle = event_truth->GetTrack(j);

	  if ( !inParticle)
	    continue;

	  /* Select Particles for Jets */
	  if(j>10 && inParticle->GetStatus() == 1 && inParticle->GetParentIndex() != 3)
	    {
	      if( abs(inParticle->GetEta()) <= 5 && inParticle->GetE() >= 0.250 )
		{
		  /* Truth particle: Get all information directly from particle */
		  Double_t px = inParticle->GetPx();
		  Double_t py = inParticle->GetPy();
		  Double_t pz = inParticle->GetPz();
		  Double_t E = inParticle->GetE();

		  fastjet::PseudoJet pPt(px,py,pz,E);
		  pPt.set_user_index(inParticle->GetIndex());
		  jetcomponent_truth.push_back(pPt);
		}
	    }
	}

      /* Loop over SMEARED jet components: Energy in Calorimeter (like 'tower jets'). Fill jet component vector. */
      for(Int_t js=0; js<event_smear->GetNTracks(); js++)
	{
	  const Smear::ParticleMCS* inParticle = event_smear->GetTrack(js);

	  if ( !inParticle)
	    continue;

	  /* Select Particles for Jets */
	  if(js>10 && inParticle->GetStatus() == 1 && inParticle->GetParentIndex() != 3)
	    {
	      if( inParticle->GetE() >= 0.250 )
	  	{
		  /* Calorimeter: Get energy from smeared particle, and ... */
		  Double_t E = inParticle->GetE();

//		  if ( E == 0 )
//		    cout << "E == 0 found! PID = " << event_truth->GetTrack(js)->GetPdgCode() << " , E_true = " <<  event_truth->GetTrack(js)->GetE() << " , Eta_true = " <<  event_truth->GetTrack(js)->GetEta() << endl;

		  /* ... get theta, phi from truth particle */
		  Double_t phi = event_truth->GetTrack(js)->GetPhi();
		  Double_t eta = event_truth->GetTrack(js)->GetEta();

		  /* Recalculate px, py, pz based on calorimeter enrgy and truth direction */
		  Double_t pT = E / cosh( eta );
		  Double_t px = pT * cos( phi );
		  Double_t py = pT * sin( phi );
		  Double_t pz = pT * sinh( eta );

	  	  fastjet::PseudoJet pPt(px,py,pz,E);
		  pPt.set_user_index(event_truth->GetTrack(js)->GetIndex());
		  jetcomponent_smear.push_back(pPt);
	  	}
	    }
	}

      /* Set Jet Definitions */
      JetDefinition jet_def_antikt(antikt_algorithm,fastjetR);

      /* Run Clustering and Extract the Jets */

      /* Lab Frame Cluster */
      ClusterSequence cluster_truth_antikt(jetcomponent_truth, jet_def_antikt);
      ClusterSequence cluster_smear_antikt(jetcomponent_smear, jet_def_antikt);

      /* Lab Frame Jets*/
      vector<PseudoJet> jets_truth_antikt = sorted_by_pt(cluster_truth_antikt.inclusive_jets(ptmin));
      vector<PseudoJet> jets_smear_antikt = sorted_by_pt(cluster_smear_antikt.inclusive_jets(ptmin));

      /* loop over SMEARED jets */
      _event_njets =  jets_smear_antikt.size();
      for ( unsigned ijet = 0; ijet < _event_njets; ijet++ )
	{
	  PseudoJet* jetMatch = find_matching_jet( &(jets_smear_antikt.at(ijet)), &jets_truth_antikt );

	  /* Set SMEARED jet variables */
	  _jet_smear_id          = ijet;
	  _jet_smear_ncomp       = -999;
	  _jet_smear_ncharged    = -999;
	  _jet_smear_e           = jets_smear_antikt.at(ijet).E();
	  _jet_smear_et          = jets_smear_antikt.at(ijet).Et();
	  _jet_smear_eta         = jets_smear_antikt.at(ijet).eta();
	  _jet_smear_phi         = jets_smear_antikt.at(ijet).phi_std();
	  _jet_smear_minv        = jets_smear_antikt.at(ijet).m();
	  _jet_smear_eem         = NAN;
	  _jet_smear_rvtx        = NAN;
	  _jet_smear_rmean       = NAN;
	  _jet_smear_rstdev      = NAN;

	  /* Set TRUTH jet variables */
	  _jet_truth_id          = -999;
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
  mTree->Write();
  ofile->Close();

  return 0;
}

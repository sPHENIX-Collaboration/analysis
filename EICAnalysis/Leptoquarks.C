#include "Leptoquarks.h"
#include "TruthTrackerHepMC.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

#include <TLorentzVector.h>
#include <TNtuple.h>
#include <TFile.h>
#include <TString.h>
#include <TH2D.h>
#include <TDatabasePDG.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

using namespace std;

Leptoquarks::Leptoquarks(std::string filename) :
  SubsysReco("Leptoquarks" ),
  _foutname(filename),
  _fout_root(NULL),
  _tree_event(NULL),
  _ebeam_E(10),
  _pbeam_E(250),
  _embedding_id(1)
{

}

int
Leptoquarks::Init(PHCompositeNode *topNode)
{
  _ievent = 0;

  _fout_root = new TFile(_foutname.c_str(), "RECREATE");

  _tree_event = new TNtuple("ntp_event","event information from LQ events",
                            "ievent:pt_miss:pt_miss_phi:has_tau:has_uds:tau_eta:tau_phi:tau_e:tau_p:tau_pt:tau_decay_prong:tau_decay_hcharged:tau_decay_lcharged:uds_eta:uds_phi:uds_e:uds_p:uds_pt");


  return 0;
}

int
Leptoquarks::process_event(PHCompositeNode *topNode)
{
  /* Access GenEventMap */
  PHHepMCGenEventMap * geneventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!geneventmap)
    {
      std::cout <<PHWHERE<<" - Fatal error - missing node PHHepMCGenEventMap"<<std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

  /* Look for leptoquark and tau particle in the event */
  TruthTrackerHepMC truth;
  truth.set_hepmc_geneventmap( geneventmap );

  int pdg_lq = 39; // leptoquark
  int pdg_tau = 15; // tau lepton

  /* Search for leptoquark in event */
  HepMC::GenParticle* particle_lq = truth.FindParticle( pdg_lq );

  /* Search for lq->tau decay in event */
  HepMC::GenParticle* particle_tau = truth.FindDaughterParticle( pdg_tau, particle_lq );

  /* Search for lq->quark decay in event.
   * Loop over all quark PDG codes until finding a matching quark. */
  HepMC::GenParticle* particle_quark = NULL;
  for ( int pdg_quark = 1; pdg_quark < 7; pdg_quark++ )
    {
      /* try quark */
      particle_quark = truth.FindDaughterParticle( pdg_quark, particle_lq );
      if (particle_quark)
        break;

      /* try anti-quark */
      particle_quark = truth.FindDaughterParticle( -pdg_quark, particle_lq );
      if (particle_quark)
        break;
    }

  /* extract numbers from truth particles */
  bool tau_found = false;
  bool quark_found = false;

  double tau_eta = 0;
  double tau_phi = 0;
  double tau_e = 0;
  double tau_p = 0;
  double tau_pt = 0;

  uint tau_decay_prong = 0;
  uint tau_decay_hcharged = 0;
  uint tau_decay_lcharged = 0;

  double quark_eta = 0;
  double quark_phi = 0;
  double quark_e = 0;
  double quark_p = 0;
  double quark_pt = 0;

  float pt_miss = 0;
  float pt_miss_phi = 0;

  /* Calculate missing transverse momentum in event */
  truth.FindMissingPt( pt_miss, pt_miss_phi );

  /* If TAU in event: update tau information */
  if( particle_tau )
    {
      tau_found = true;
      tau_eta = particle_tau->momentum().eta();
      tau_phi = particle_tau->momentum().phi();

      /* Event record uses 0 < phi < 2Pi, while Fun4All uses -Pi < phi < Pi.
       * Therefore, correct angle for 'wraparound' at phi == Pi */
      if(tau_phi>TMath::Pi()) tau_phi = tau_phi-2*TMath::Pi();

      tau_e = particle_tau->momentum().e();
      tau_p = sqrt( pow( particle_tau->momentum().px(), 2 ) +
                    pow( particle_tau->momentum().py(), 2 ) +
                    pow( particle_tau->momentum().pz(), 2 ) );
      tau_pt = particle_tau->momentum().perp();

      /* Count how many charged particles (hadrons and leptons) the tau particle decays into. */
      truth.FindDecayParticles( particle_tau, tau_decay_prong, tau_decay_hcharged, tau_decay_lcharged );

    }

  /* If QUARK (->jet) in event: Tag the tau candidate (i.e. jet) with smalles delta_R from this quark */
  /* @TODO: This is a copy from the loop to tag the tau candidate with smallest delta_R w.r.t. final state tau-
   * instead of copying the code, make a function that can be called for both (GetMinDeltaRElement or similar) */
  if( particle_quark )
    {
      quark_found = true;
      quark_eta = particle_quark->momentum().eta();
      quark_phi = particle_quark->momentum().phi();

      /* Event record uses 0 < phi < 2Pi, while Fun4All uses -Pi < phi < Pi.
       * Therefore, correct angle for 'wraparound' at phi == Pi */
      if(quark_phi>TMath::Pi()) quark_phi = quark_phi-2*TMath::Pi();

      quark_e  = particle_quark->momentum().e();
      quark_p  = sqrt( pow( particle_quark->momentum().px(), 2 ) +
                       pow( particle_quark->momentum().py(), 2 ) +
                       pow( particle_quark->momentum().pz(), 2 ) );
      quark_pt = particle_quark->momentum().perp();

    }

  /* Fill event information to ntupple */
  float event_data[18] = { (float) _ievent,
                           (float) pt_miss,
                           (float) pt_miss_phi,
                           (float) tau_found,
                           (float) quark_found,
                           (float) tau_eta,
                           (float) tau_phi,
                           (float) tau_e,
                           (float) tau_p,
                           (float) tau_pt,
                           (float) tau_decay_prong,
                           (float) tau_decay_hcharged,
                           (float) tau_decay_lcharged,
                           (float) quark_eta,
                           (float) quark_phi,
                           (float) quark_e,
                           (float) quark_p,
                           (float) quark_pt
  };

  _tree_event->Fill( event_data );

  /* count up event number */
  _ievent ++;

  return 0;
}

int
Leptoquarks::End(PHCompositeNode *topNode)
{
  _fout_root->cd();
  _tree_event->Write();
  _fout_root->Close();

  return 0;
}


void
Leptoquarks::UpdateFinalStateParticle( HepMC::GenParticle *&particle )
{
  bool final_state = false;
  while ( !final_state )
    {
      /* Loop over all children at the end vertex of this particle */
      for ( HepMC::GenVertex::particle_iterator child
              = particle->end_vertex()->particles_begin(HepMC::children);
            child != particle->end_vertex()->particles_end(HepMC::children);
            ++child )
        {
          /* If there is a child of same particle ID, this was not the final state particle- update pointer to particle and repeat */
          if ( (*child)->pdg_id() == particle->pdg_id() )
            {
              particle = (*child);
              break;
            }
          final_state = true;
        }
    }
  return;
}

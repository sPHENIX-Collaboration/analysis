#include "Leptoquarks.h"

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
                            "ievent:has_tau:has_uds:tau_eta:tau_phi:tau_e:tau_p:tau_pt:tau_decay_prong:tau_decay_hcharged:tau_decay_lcharged:uds_eta:uds_phi:uds_e:uds_p:uds_pt");


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

  /* Access GenEvent */
  PHHepMCGenEvent *genevt = geneventmap->get(_embedding_id);
  if (!genevt)
    {
      std::cout <<PHWHERE<<" - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID "<<_embedding_id;
      std::cout <<". Print PHHepMCGenEventMap:";
      geneventmap->identify();
      return Fun4AllReturnCodes::ABORTRUN;
    }

  /* Access event */
  HepMC::GenEvent* theEvent = genevt->getEvent();

  if ( !theEvent )
    {
      cout << "Missing GenEvent!" << endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

  /* Look for leptoquark and tau particle in the event */
  HepMC::GenParticle* particle_lq = NULL;
  HepMC::GenParticle* particle_tau = NULL;
  HepMC::GenParticle* particle_quark = NULL;

  bool tau_found = false;
  bool quark_found = false;

  double tau_eta = 0;
  double tau_phi = 0;
  double tau_e = 0;
  double tau_p = 0;
  double tau_pt = 0;

  int tau_decay_prong = 0;
  int tau_decay_hcharged = 0;
  int tau_decay_lcharged = 0;

  double quark_eta = 0;
  double quark_phi = 0;
  double quark_e = 0;
  double quark_p = 0;
  double quark_pt = 0;

  /* Loop over all truth particles in event generator collection
   *
   * particle   ...   PGD code
   * -------------------------
   * LQ_ue      ...    39
   * tau-       ...    15
   * pi+        ...    211
   * pi-        ...   -211
   * u          ...    2
   * d          ...    1 */
  for ( HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
        p != theEvent->particles_end(); ++p ) {

    /* check particle ID == LQ_ue */
    if ( (*p)->pdg_id() == 39 )
      {
        particle_lq = (*p);

        /* Where does it end (=decay)? */
        if ( particle_lq->end_vertex() )
          {
            /* Loop over mother particles at production vertex */
            for ( HepMC::GenVertex::particle_iterator lq_child
                    = particle_lq->end_vertex()->
                    particles_begin(HepMC::children);
                  lq_child != particle_lq->end_vertex()->
                    particles_end(HepMC::children);
                  ++lq_child )
              {
                /* Is child a tau? */
                if ( (*lq_child)->pdg_id() == 15 )
                  {
                    particle_tau = (*lq_child);
                    UpdateFinalStateParticle( particle_tau );
                  }
                /* Is child a quark? */
                else if ( abs((*lq_child)->pdg_id()) > 0 && abs((*lq_child)->pdg_id() < 7) )
                  {
                    particle_quark = (*lq_child);
                    UpdateFinalStateParticle( particle_quark );
                  }
                /* What else could it be? */
                else
                  {
                    cerr << PHWHERE << " ERROR: Didn't expect to find a leptoquark child with pdg_id " << (*lq_child)->pdg_id() << endl;
                    return Fun4AllReturnCodes::ABORTEVENT;
                  }
              }
          }

        /* end loop if leptoquark found */
        break;
      } // end if leptoquark //
  } // end loop over all particles in event //

  /* If TAU in event: Tag the tau candidate (i.e. jet) with smalles delta_R from this tau */
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

      /* Add information about tau decay */
      /* Loop over all particle at end vertex */
      if ( particle_tau->end_vertex() )
        {
          for ( HepMC::GenVertex::particle_iterator tau_decay
                  = particle_tau->end_vertex()->
                  particles_begin(HepMC::children);
                tau_decay != particle_tau->end_vertex()->
                  particles_end(HepMC::children);
                ++tau_decay )
            {
              /* Get entry from TParticlePDG because HepMC::GenPArticle does not provide charge or class of particle */
              TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*tau_decay)->pdg_id() );

              /* Check if particle is charged */
              if ( pdg_p->Charge() != 0 )
                {
                  tau_decay_prong += 1;

                  /* Check if particle is lepton */
                  if ( string( pdg_p->ParticleClass() ) == "Lepton" )
                    tau_decay_lcharged += 1;

                  /* Check if particle is hadron, i.e. Meson or Baryon */
                  else if ( ( string( pdg_p->ParticleClass() ) == "Meson"  ) ||
                            ( string( pdg_p->ParticleClass() ) == "Baryon" ) )
                    tau_decay_hcharged += 1;
                }
            }
        }
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
  float event_data[16] = { (float) _ievent,
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

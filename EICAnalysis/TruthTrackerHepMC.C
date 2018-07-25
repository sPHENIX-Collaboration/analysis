#include "TruthTrackerHepMC.h"
#include "DISKinematicsReco.h"

/* ROOT includes */
#include <TDatabasePDG.h>

/* STL includes */
#include <iostream>

using namespace std;


TruthTrackerHepMC::TruthTrackerHepMC()
{

}


HepMC::GenParticle* TruthTrackerHepMC::FindParticle( int pid )
{

  HepMC::GenParticle *particle = NULL;

  /* --> Loop over all truth events in event generator collection */
  for (PHHepMCGenEventMap::ReverseIter iter = _genevtmap->rbegin(); iter != _genevtmap->rend(); ++iter)
    {
      PHHepMCGenEvent *genevt = iter->second;
      HepMC::GenEvent *theEvent = genevt->getEvent();

      /* check if GenEvent object found */
      if ( !theEvent )
        {
          cout << "ERROR: Missing GenEvent!" << endl;
          return NULL;
        }

      /* Loop over all truth particles in event generator collection */
      for ( HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
            p != theEvent->particles_end(); ++p ) {

        /* check particle ID */
        if ( (*p)->pdg_id() == pid )
          {
            particle = (*p);
	    
            /* end loop if matching particle found */
            break;
          } // end if matching PID //
      } // end loop over all particles in event //
    }// end loop over genevtmap //

  return particle;
}


HepMC::GenParticle* TruthTrackerHepMC::FindDaughterParticle( int pid, HepMC::GenParticle* particle_mother )
{
  HepMC::GenParticle* particle_daughter = NULL;

  /*Check if particle mother is found*/
  if(!particle_mother);
  /* Where does mother particle end (=decay)? */
  else if ( particle_mother->end_vertex() )
    {
      /* Loop over child particles at mother's end vertex */
      for ( HepMC::GenVertex::particle_iterator child
              = particle_mother->end_vertex()->
              particles_begin(HepMC::children);
            child != particle_mother->end_vertex()->
              particles_end(HepMC::children);
            ++child )
        {
	  
          /* Has child correct PDG code? */
          if ( (*child)->pdg_id() == pid )
            {
              particle_daughter = (*child);
              UpdateFinalStateParticle( particle_daughter );
	    }
        }
    }

  return particle_daughter;
}


HepMC::GenParticle* TruthTrackerHepMC::FindBeamLepton( )
{
  HepMC::GenParticle *particle = NULL;

  int embedding_id = 1;
  PHHepMCGenEvent *theEvent = _genevtmap->get(embedding_id);

  /* check if GenEvent object found */
  if ( !theEvent )
    {
      cout << "ERROR: Missing GenEvent!" << endl;
      return NULL;
    }

  HepMC::GenEvent* genEvent = theEvent->getEvent();
  /* Find beam lepton */
  particle = genEvent->beam_particles().first;
  return particle;
}


HepMC::GenParticle* TruthTrackerHepMC::FindBeamHadron( )
{
  HepMC::GenParticle *particle = NULL;

  int embedding_id = 1;
  PHHepMCGenEvent *theEvent = _genevtmap->get(embedding_id);

  /* check if GenEvent object found */
  if ( !theEvent )
    {
      cout << "ERROR: Missing GenEvent!" << endl;
      return NULL;
    }

  HepMC::GenEvent* genEvent = theEvent->getEvent();

  particle = genEvent->beam_particles().second;
  
  return particle;
}


HepMC::GenParticle* TruthTrackerHepMC::FindScatteredLepton( )
{
  HepMC::GenParticle *particle = NULL;

  /* @TODO How to select scattered lepton in an unambiguous way for
     DIS and Exclusive Processes? (Pythia, Sartre, ...)
     Return NULL pointer for now. */
  /* In some event generators (SARTRE), no beam particles are created,
     if so, assume beam lepton is an electron for now */
  int pid_beam_lepton=0;
  if(FindBeamLepton()==NULL)
    {
      /* Assume electron beam */
      pid_beam_lepton=11;
    }
  else
    {
      pid_beam_lepton = FindBeamLepton()->pdg_id();
    }
  int embedding_id = 1;
  
  PHHepMCGenEvent *genevt = _genevtmap->get(embedding_id);
  HepMC::GenEvent *theEvent = genevt->getEvent();
  
  /* check if GenEvent object found */
  if ( !theEvent )
    {
      cout << "ERROR: Missing GenEvent!" << endl;
      return NULL;
    }
  /* Loop over all truth particles in event generator collection */
  for ( HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
  	p != theEvent->particles_end(); ++p ) {
  
    /* check particle status and ID */
    if ( (*p)->status() == 1 &&
  	 (*p)->pdg_id()  == pid_beam_lepton )
      {
  	particle = (*p);
  
  	/* end loop if matching particle found */
  	break;
      } // end if matching status and PID //
  } // end loop over all particles in event //
  return particle;
}


void
TruthTrackerHepMC::UpdateFinalStateParticle( HepMC::GenParticle *&particle )
{
  bool final_state = false;
  while ( !final_state )
    {
      /* if particle has no vertex it is final state */
      if(!particle->end_vertex()) break;

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


void
TruthTrackerHepMC::FindDecayParticles( HepMC::GenParticle *particle_mother, uint &decay_prong, uint &decay_hcharged, uint &decay_lcharged)
{
  /* Reset counter variables */
  decay_prong = 0;
  decay_hcharged = 0;
  decay_lcharged = 0;

  /* Loop over particles at end vertex of particle */
  for ( HepMC::GenVertex::particle_iterator decay
          = particle_mother->end_vertex()->
          particles_begin(HepMC::children);
        decay != particle_mother->end_vertex()->
          particles_end(HepMC::children);
        ++decay )
    {
      /* check if particle decays further */
      if(!(*decay)->end_vertex()){
 
        /* Get entry from TParticlePDG because HepMC::GenPArticle does not provide charge or class of particle */
        TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*decay)->pdg_id() );

        /* Check if particle is charged */
        if ( pdg_p->Charge() != 0 )
          {
            decay_prong += 1;

            /* Check if particle is lepton */
            if ( string( pdg_p->ParticleClass() ) == "Lepton" )
              decay_lcharged += 1;

            /* Check if particle is hadron, i.e. Meson or Baryon */
            else if ( ( string( pdg_p->ParticleClass() ) == "Meson"  ) ||
                      ( string( pdg_p->ParticleClass() ) == "Baryon" ) )
              decay_hcharged += 1;
          }
      }

      /* loop over decay if particle decays further */
      else if((*decay)->end_vertex()){

        /*further decay loop */
        for ( HepMC::GenVertex::particle_iterator second_decay
                = (*decay)->end_vertex()->
                particles_begin(HepMC::children);
              second_decay != (*decay)->end_vertex()->
                particles_end(HepMC::children);
              ++second_decay )
          {

            // Get entry from TParticlePDG because HepMC::GenPArticle does not provide charge or class of particle
            TParticlePDG * pdg_p2 = TDatabasePDG::Instance()->GetParticle( (*second_decay)->pdg_id() );

            // Check if particle is charged
            if ( pdg_p2->Charge() != 0 )
              {
                decay_prong += 1;

                // Check if particle is lepton
                if ( string( pdg_p2->ParticleClass() ) == "Lepton" )
                  decay_lcharged += 1;

                // Check if particle is hadron, i.e. Meson or Baryon
                else if ( ( string( pdg_p2->ParticleClass() ) == "Meson"  ) ||
                          ( string( pdg_p2->ParticleClass() ) == "Baryon" ) )
                  decay_hcharged += 1;
              }
          }
      }
    }

  return;

  /* @TODO: Below = Function that calls itself instead of doing the same loop. Still need to debug */

  //  for ( HepMC::GenVertex::particle_iterator decay
  //          = particle_mother->end_vertex()->
  //          particles_begin(HepMC::children);
  //        decay != particle_mother->end_vertex()->
  //          particles_end(HepMC::children);
  //        ++decay )
  //    {
  //
  //      if(!(*decay)->end_vertex()){
  //
  //        // Get entry from TParticlePDG because HepMC::GenPArticle does not provide charge or class of particle
  //        TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*decay)->pdg_id() );
  //
  //        // Check if particle is charged
  //        if ( pdg_p->Charge() != 0 )
  //          {
  //            decay_prong += 1;
  //
  //            // Check if particle is lepton
  //            if ( string( pdg_p->ParticleClass() ) == "Lepton" )
  //              decay_lcharged += 1;
  //
  //            // Check if particle is hadron, i.e. Meson or Baryon
  //            else if ( ( string( pdg_p->ParticleClass() ) == "Meson"  ) ||
  //                      ( string( pdg_p->ParticleClass() ) == "Baryon" ) )
  //              decay_hcharged += 1;
  //          }
  //      }
  //
  //      //HepMC::GenVertex::particle_iterator secondary_decay = (*decay)->end_vertex;
  //      else if((*decay)->end_vertex()) FindDecayProducts((*decay), decay_prong, decay_hcharged, decay_lcharged);
  //    }

}


void
TruthTrackerHepMC::FindMissingPt( float &pt_miss, float &pt_miss_phi )
{
  /* set output values to '0' */
  pt_miss = 0;
  pt_miss_phi = 0;

  /* variables to keep track of components */
  float px_sum = 0;
  float py_sum = 0;

  /* --> Loop over all truth events in event generator collection */
  for (PHHepMCGenEventMap::ReverseIter iter = _genevtmap->rbegin(); iter != _genevtmap->rend(); ++iter)
    {
      PHHepMCGenEvent *genevt = iter->second;
      HepMC::GenEvent *theEvent = genevt->getEvent();

      /* check if GenEvent object found */
      if ( !theEvent )
        {
          cout << "ERROR: Missing GenEvent!" << endl;
          return;
        }

      /* Loop over all truth particles in event generator collection */
      for ( HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
            p != theEvent->particles_end(); ++p ) {

        /* skip particles that are not stable final state particles */
        if ( (*p)->status() != 1 )
          continue;

        /* Get entry from TParticlePDG because HepMC::GenPArticle does not provide charge or class of particle */
        TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*p)->pdg_id() );

        /* skip neutral leptons = neutrinos (invisible to detector) */
        if ( ( string( pdg_p->ParticleClass() ) == "Lepton" ) &&
             ( pdg_p->Charge() == 0 ) )
          continue;

        /* update momentum component sum */
        px_sum += (*p)->momentum().px();
        py_sum += (*p)->momentum().py();

      } // end loop over all particles in event //
    }// end loop over genevtmap //

  /* calculate pt_miss and phi */
  pt_miss = sqrt( px_sum * px_sum + py_sum * py_sum );
  pt_miss_phi = atan( py_sum / px_sum );

  return;
}

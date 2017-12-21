#ifndef __TRUTHTRACKERHEPMC_H__
#define __TRUTHTRACKERHEPMC_H__

/*HepMC include */
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

/**
   Class with helpful functions to trace particle and event information through HepMC event records.

   Some useful PDG codes:

   particle   ...   PGD code
   -------------------------
   LQ_ue      ...    39
   tau-       ...    15
   pi+        ...    211
   pi-        ...   -211
   u          ...    2
   d          ...    1
*/
class TruthTrackerHepMC
{

public:

  /** Default constructor. */
  TruthTrackerHepMC();

  /** Default destructor. */
  ~TruthTrackerHepMC() {}

  /** Set pointer to PHHeoMCGenEventMap to use for this event. */
  void set_hepmc_geneventmap( PHHepMCGenEventMap* evtmap )
  {
    _genevtmap = evtmap;
  }

  /** Locate a particle with specific PDG code in event record. Return pointer to first particle with matching ID found. */
  HepMC::GenParticle* FindParticle( int );

  /** Locate a daughet particle with specific PDG code in event record that is a child of a given particle.
      Return pointer to first daughter particle with matching ID found. */
  HepMC::GenParticle* FindDaughterParticle( int , HepMC::GenParticle* );


private:

  /** The PHHepMCGenEvent map with all HepMC information for this event */
  PHHepMCGenEventMap* _genevtmap;

  /** Check if this is the final state particle or not- update pointer if needed */
  void UpdateFinalStateParticle( HepMC::GenParticle *& );

  //  void FindDecayParticles(HepMC::GenParticle *, int&, int&, int&);

};


#endif

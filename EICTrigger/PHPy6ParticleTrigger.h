#ifndef PHPy6ParticleTrigger_h
#define PHPy6ParticleTrigger_h

#include "PHPy6GenTrigger.h"
#include <HepMC/GenEvent.h>

namespace HepMC
{
  class GenEvent;
};

/**
 * Fun4All module based in PHPythia8/PHPy8ParticleTrigger
 *
 * Provides trigger for events gerenated by PHPythia6 event generator (Pythia6 wrapper for Fun4All).
 *
 * This trigger makes use of the conversion of Pythia6 events to HepMC format done in PHPythia6.
 *
 */
class PHPy6ParticleTrigger: public PHPy6GenTrigger
{
public:

  //! constructor
  PHPy6ParticleTrigger(const std::string &name = "PHPy6ParticleTriggerger");

  //! destructor
  ~PHPy6ParticleTrigger( void ){}

#ifndef __CINT__
  bool Apply(const HepMC::GenEvent* evt);
#endif


  void SetParticleType(int pid);
  void SetQ2Min(double Q2);

protected:

  // trigger variables
  std::vector<int> _theParents;
  std::vector<int> _theParticles;


  int _theParticleType;
  double _Q2Min;

  bool _doTheParticleType, _doQ2Min;
};

#endif

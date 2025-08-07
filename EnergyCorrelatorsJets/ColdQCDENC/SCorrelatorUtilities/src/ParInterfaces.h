/// ---------------------------------------------------------------------------
/*! \file   ParTools.h
 *  \author Derek Anderson
 *  \file   03.06.2024
 *
 *  Particle-related interfaces.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_PARINTERFACES_H
#define SCORRELATORUTILITIES_PARINTERFACES_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// c++ utilities
#include <cassert>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// PHG4 libraries
#include <g4main/PHG4TruthInfoContainer.h>
// hepmc libraries
#include <HepMC/GenEvent.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#pragma GCC diagnostic pop

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Interfaces {

    // particle interfaces ----------------------------------------------------

    PHG4TruthInfoContainer*            GetTruthContainer(PHCompositeNode* topNode);
    PHG4TruthInfoContainer::ConstRange GetPrimaries(PHCompositeNode* topNode);
    PHHepMCGenEventMap*                GetMcEventMap(PHCompositeNode* topNode);
    PHHepMCGenEvent*                   GetMcEvent(PHCompositeNode* topNode, const int iEvtToGrab);
    HepMC::GenEvent*                   GetGenEvent(PHCompositeNode* topNode, const int iEvtToGrab);

  }  // end Interfaces namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

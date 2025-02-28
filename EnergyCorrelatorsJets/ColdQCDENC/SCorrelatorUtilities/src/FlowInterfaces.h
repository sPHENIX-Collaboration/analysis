/// ---------------------------------------------------------------------------
/*! \file   FlowInterfaces.h
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *  Particle flow-related interfaces.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_FLOWINTERFACES_H
#define SCORRELATORUTILITIES_FLOWINTERFACES_H

// c++ utilities
#include <limits>
#include <vector>
#include <string>
#include <cassert>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// particle flow libraries
#include <particleflowreco/ParticleFlowElement.h>
#include <particleflowreco/ParticleFlowElementContainer.h>

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Interfaces {

    // particle flow interfaces -----------------------------------------------

    ParticleFlowElementContainer*            GetFlowStore(PHCompositeNode* topNode);
    ParticleFlowElementContainer::ConstRange GetParticleFlowObjects(PHCompositeNode* topNode);

  }  // end Interfaces namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

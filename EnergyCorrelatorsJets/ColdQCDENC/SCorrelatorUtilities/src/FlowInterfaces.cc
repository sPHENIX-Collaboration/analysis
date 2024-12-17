/// ---------------------------------------------------------------------------
/*! \file   FlowInterfaces.cc
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *  Particle flow-related interfaces.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_FLOWINTERFACES_CC

// namespace definition
#include "FlowInterfaces.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



// flow interfaces ============================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get particle flow container from node tree
  // --------------------------------------------------------------------------
  ParticleFlowElementContainer* Interfaces::GetFlowStore(PHCompositeNode* topNode) {

    ParticleFlowElementContainer* store = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");
    if (!store) {
      cerr << PHWHERE
           << "PANIC: Couldn't grab particle flow container!"
           << endl;
      assert(store);
    }
    return store;

  }  // end 'GetFlowStore(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Get PFOs from container
  // --------------------------------------------------------------------------
  ParticleFlowElementContainer::ConstRange Interfaces::GetParticleFlowObjects(PHCompositeNode* topNode) {

    // get container
    ParticleFlowElementContainer* store = GetFlowStore(topNode);

    // get objects
    ParticleFlowElementContainer::ConstRange objects = store -> getParticleFlowElements();
    return objects;

  }  // end 'GetParticleFlowObjects(PHCompositeNode*)'

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

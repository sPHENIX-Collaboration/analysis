// ----------------------------------------------------------------------------
// 'SCorrelatorUtilities.FlowTools.h'
// Derek Anderson
// 11.16.2023
//
// Collection of frequent particle flow-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// f4a libraries
#include <fun4all/SubsysReco.h>
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
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // particle flow methods --------------------------------------------------

    ParticleFlowElementContainer* GetFlowStore(PHCompositeNode* topNode) {

      // declare pf objects
      ParticleFlowElementContainer* flowStore = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");
      if (!flowStore) {
        cerr << PHWHERE
             << "PANIC: Couldn't grab particle flow container!"
             << endl;
        assert(flowStore);
      }
      return flowStore;

    }  // end 'GetFlowStore(PHCompositeNode*)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------


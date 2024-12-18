/// ---------------------------------------------------------------------------
/*! \file   ClustInterfaces.cc
 *  \author Derek Anderson
 *  \date   02.23.2024
 *
 *  Calorimeter cluster-related interfaces.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_CLUSTINTERFACES_CC

// namespace definition 
#include "ClustInterfaces.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



// cluster interfaces =========================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get raw cluster container from node tree
  // --------------------------------------------------------------------------
  RawClusterContainer* Interfaces::GetClusterStore(PHCompositeNode* topNode, const string node) {

    // grab clusters
    RawClusterContainer* clustStore = getClass<RawClusterContainer>(topNode, node.data());
    if (!clustStore) {
      cout << PHWHERE
           << "PANIC: " << node << " node is missing!"
           << endl;
      assert(clustStore);
    }
    return clustStore;

  }  // end 'GetClusterStore(PHCompositeNode*, string)'



  // --------------------------------------------------------------------------
  //! Get raw clusters from container
  // --------------------------------------------------------------------------
  RawClusterContainer::ConstRange Interfaces::GetClusters(PHCompositeNode* topNode, const string store) {

    // get store and return range of clusters
    RawClusterContainer* clustStore = GetClusterStore(topNode, store);
    return clustStore -> getClusters();

  }  // end 'GetClusters(PHCompositeNode*, string)'

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

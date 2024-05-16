// ----------------------------------------------------------------------------
// 'ClustInterfaces.cc'
// Derek Anderson
// 02.23.2024
//
// Calorimeter-related interfaces.
// ----------------------------------------------------------------------------

#define SCORRELATORUTILITIES_CLUSTINTERFACES_CC

// namespace definition 
#include "ClustInterfaces.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // cluster methods --------------------------------------------------------

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



  RawClusterContainer::ConstRange Interfaces::GetClusters(PHCompositeNode* topNode, const string store) {

    // get store and return range of clusters
    RawClusterContainer* clustStore = GetClusterStore(topNode, store);
    return clustStore -> getClusters();

  }  // end 'GetClusters(PHCompositeNode*, string)'

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

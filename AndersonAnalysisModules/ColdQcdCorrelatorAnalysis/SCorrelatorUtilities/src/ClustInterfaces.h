/// ---------------------------------------------------------------------------
/*! \file   ClustInterfaces.h
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  Calorimeter cluster-related interfaces.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_CLUSTINTERFACES_H
#define SCORRELATORUTILITIES_CLUSTINTERFACES_H

// c++ utilities
#include <limits>
#include <string>
#include <vector>
#include <cassert>
#include <optional>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// CaloBase libraries
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Interfaces {

    // cluster interfaces -----------------------------------------------------

    RawClusterContainer*            GetClusterStore(PHCompositeNode* topNode, const string node);
    RawClusterContainer::ConstRange GetClusters(PHCompositeNode* topNode, const string store);

  }  // end Interfaces namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

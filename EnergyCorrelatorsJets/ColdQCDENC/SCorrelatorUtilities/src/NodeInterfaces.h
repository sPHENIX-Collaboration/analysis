/// ---------------------------------------------------------------------------
/*! \file   NodeInterfaces.h
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  F4A node-related interfaces.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_NODEINTERFACES_H
#define SCORRELATORUTILITIES_NODEINTERFACES_H

// c++ utilities
#include <string>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHObject.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// analysis utilities
#include "Constants.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Interfaces {

    // node interfaces --------------------------------------------------------

    void CleanseNodeName(string& nameToClean);

    // generic methods
    template <typename T> void CreateNode(PHCompositeNode* topNode, string newNodeName, T& objectInNode);

  }  // end Interfaces namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

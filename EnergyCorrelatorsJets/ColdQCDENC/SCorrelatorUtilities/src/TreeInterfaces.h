/// ---------------------------------------------------------------------------
/*! \file   TreeInterfaces.h
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  TTree-related interfaces.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_TREEINTERFACES_H
#define SCORRELATORUTILITIES_TREEINTERFACES_H

// c++ utilities
#include <limits>
// root libraries
#include <TTree.h>
#include <TChain.h>
#include <TNtuple.h>

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Interfaces {

    // tree interfaces --------------------------------------------------------

    template <typename T> int64_t GetEntry(T* tree, const uint64_t entry);
    template <typename T> int64_t LoadTree(T* tree, const uint64_t entry, int& current);

  }  // end Interfaces namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

/// ---------------------------------------------------------------------------
/*! \file   TreeInterfaces.cc
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  TTree-related interfaces.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_TREEINTERFACES_CC

// namespace definition
#include "TreeInterfaces.h"

// make common namespaces implicit
using namespace std;



// TTree interfaces ===========================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get a particular entry from a generic TTree-derived object
  // --------------------------------------------------------------------------
  template <typename T> int64_t Interfaces::GetEntry(
    T* tree,
    const uint64_t entry
  ) {

    int64_t status = numeric_limits<int64_t>::min();
    if (!tree) {
      status = 0;
    } else {
      status = tree -> GetEntry(entry);
    }
    return status;

  }  // end 'GetEntry(T*, uint64_t)'

  // specific instantiations of `GetEntry()`
  template int64_t Interfaces::GetEntry(TTree* tree, const uint64_t entry);
  template int64_t Interfaces::GetEntry(TChain* tree, const uint64_t entry);
  template int64_t Interfaces::GetEntry(TNtuple* tree, const uint64_t entry);



  // --------------------------------------------------------------------------
  //! Load a generic TTree-derived object
  // --------------------------------------------------------------------------
  template <typename T> int64_t Interfaces::LoadTree(
    T* tree,
    const uint64_t entry,
    int& current
  ) {

    // check for tree & load
    int     number = numeric_limits<int>::min();
    int64_t status = numeric_limits<int64_t>::min();
    if (!tree) {
      status = -5;
    } else {
      number = tree -> GetTreeNumber();
      status = tree -> LoadTree(entry);
    }

    // update current tree number if need be
    const bool isStatusGood = (status >= 0);
    const bool isNotCurrent = (number != current);
    if (isStatusGood && isNotCurrent) {
      current = tree -> GetTreeNumber();
    }
    return status;

  }  // end 'LoadTree(uint64_t)'

  // specific instantiations of `LoadTree()`
  template int64_t Interfaces::LoadTree(TTree* tree, const uint64_t entry, int& current);
  template int64_t Interfaces::LoadTree(TChain* tree, const uint64_t entry, int& current);
  template int64_t Interfaces::LoadTree(TNtuple* tree, const uint64_t entry, int& current);

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

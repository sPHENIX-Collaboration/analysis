/// ---------------------------------------------------------------------------
/*! \file   TupleInterfaces.cc
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  TNtuple-related interfaces.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_TUPLEINTERFACES_CC

// namespace definition
#include "TupleInterfaces.h"

// make common namespaces implicit
using namespace std;



// tuple interfaces ===========================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Add a tag to a vector of TNtuple leaves
  // --------------------------------------------------------------------------
  void Interfaces::AddTagToLeaves(const string tag, vector<string>& leaves) {

    for (string& leaf : leaves) {
      leaf.append(tag);
    }
    return;

  }  // end 'AddTagToLeaves(vector<string>)'



  // --------------------------------------------------------------------------
  //! Combine two vectors of TNtuple leaves
  // --------------------------------------------------------------------------
  void Interfaces::CombineLeafLists(const vector<string>& addends, vector<string>& toAddTo) {

    for (string addend : addends) {
      toAddTo.push_back(addend);
    }
    return;

  }  // end 'CombineLeafLists(vector<string>&, vector<string>&)'



  // --------------------------------------------------------------------------
  //! Flatten a vector of TNtuple leaves into a colon-separated string
  // --------------------------------------------------------------------------
  string Interfaces::FlattenLeafList(const vector<string>& leaves) {

    string flattened("");
    for (size_t iLeaf = 0; iLeaf < leaves.size(); iLeaf++) {
      flattened.append(leaves[iLeaf]);
      if ((iLeaf + 1) != leaves.size()) {
        flattened.append(":");
      }
    }
    return flattened;

  }  // end 'FlattenLeafList(vector<string>&)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

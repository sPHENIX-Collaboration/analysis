// ----------------------------------------------------------------------------
// 'TupleInterfaces.cc'
// Derek Anderson
// 03.05.2024
//
// TNtuple-related interfaces.
// ----------------------------------------------------------------------------

#define SCORRELATORUTILITIES_TUPLEINTERFACES_CC

// namespace definition
#include "TupleInterfaces.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  void Interfaces::AddTagToLeaves(const string tag, vector<string>& leaves) {

    for (string& leaf : leaves) {
      leaf.append(tag);
    }
    return;

  }  // end 'AddTagToLeaves(vector<string>)'



  void Interfaces::CombineLeafLists(const vector<string>& addends, vector<string>& toAddTo) {

    for (string addend : addends) {
      toAddTo.push_back(addend);
    }
    return;

  }  // end 'CombineLeafLists(vector<string>&, vector<string>&)'



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

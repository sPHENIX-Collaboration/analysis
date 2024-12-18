/// ---------------------------------------------------------------------------
/*! \file   TupleInterfaces.h
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  TNtuple-related interfaces.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_TUPLEINTERFACES_H
#define SCORRELATORUTILITIES_TUPLEINTERFACES_H

// c++ utilities
#include <string>
#include <vector>
#include <optional>

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Interfaces {

    // tuple interfaces -------------------------------------------------------

    void   AddTagToLeaves(const string tag, vector<string>& leaves);
    void   CombineLeafLists(const vector<string>& addends, vector<string>& toAddTo);
    string FlattenLeafList(const vector<string>& leaves);

  }  // end Interfaces namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

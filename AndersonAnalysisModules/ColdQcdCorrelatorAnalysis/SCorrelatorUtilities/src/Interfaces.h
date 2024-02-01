// ----------------------------------------------------------------------------
// 'Interfaces.h'
// Derek Anderson
// 12.29.2023
//
// Various interfaces between sPHENIX Cold QCD Correlator
// Analysis objects and data structures (e.g. TTree's) are
// collected here.
// ----------------------------------------------------------------------------

#pragma once

// c++ utilities
#include <string>
#include <limits>
#include <vector>
#include <optional>
// root libraries
#include <TTree.h>
#include <TChain.h>
#include <TNtuple.h>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// analyis utilities
#include "TrkTools.h"
#include "CalTools.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // correlator object-data structure interfaces ----------------------------

    /* TODO will go here */



    // f4a interfaces ---------------------------------------------------------

    // remove forbidden characters from a node name
    void CleanseNodeName(string& nameToClean) {

      for (const auto& [bad, good] : MapBadOntoGoodStrings) {
        size_t position;
        while ((position = nameToClean.find(bad)) != string::npos) {
          nameToClean.replace(position, 1, good);
        }
      }  // end bad-good pair loop
      return;

    }  // end 'CleanseNodeName(string&)'



    template <typename T> void CreateNode(PHCompositeNode* topNode, string newNodeName, T& objectInNode) {

      // make sure node name is okay
      CleanseNodeName(newNodeName);

      // find DST node
      PHNodeIterator   itNode(topNode);
      PHCompositeNode* dstNode = dynamic_cast<PHCompositeNode*>(itNode.findFirst("PHCompositeNode", "DST"));

      // create node and exit
      PHIODataNode<PHObject>* newNode = new PHIODataNode<PHObject>(objectInNode, newNodeName.c_str(), "PHObject");
      dstNode -> addNode(newNode);
      return;

    }  // end 'CreateNode(PHCompositeNode*, string, T&)'



    // TTree interfaces -------------------------------------------------------

    // get entry from TTree, TChain, or TNtuple
    template <typename T> int64_t GetEntry(T* tree, const uint64_t entry) {

      int64_t status = numeric_limits<int64_t>::min();
      if (!tree) {
        status = 0;
      } else {
        status = tree -> GetEntry(entry);
      }
      return status;

    }  // end 'GetEntry(T*, uint64_t)'

    template int64_t GetEntry(TTree* tree, const uint64_t entry);
    template int64_t GetEntry(TChain* tree, const uint64_t entry);
    template int64_t GetEntry(TNtuple* tree, const uint64_t entry);



    // load TTree, TChain, or TNtuple
    template <typename T> int64_t LoadTree(T* tree, const uint64_t entry, int& current) {

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

    template int64_t LoadTree(TTree* tree, const uint64_t entry, int& current);
    template int64_t LoadTree(TChain* tree, const uint64_t entry, int& current);
    template int64_t LoadTree(TNtuple* tree, const uint64_t entry, int& current);



    // data member-to-string methods ------------------------------------------

    // append a tag to a vector of leaf names
    void AddTagToLeaves(const string tag, vector<string>& leaves) {

      for (string& leaf : leaves) {
        leaf.append(tag);
      }
      return;

    }  // end 'AddTagToLeaves(vector<string>)'



    // flatten list of leaves into a colon-separated list
    string FlattenLeafList(const vector<string>& leaves) {

      string flattened("");
      for (size_t iLeaf = 0; iLeaf < leaves.size(); iLeaf++) {
        flattened.append(leaves[iLeaf]);
        if ((iLeaf + 1) != leaves.size()) {
          flattened.append(":");
        }
      }
      return flattened;

    }  // end 'FlattenLeafList(vector<string>&)'



    // generic function to create vector of leaf names
    template <typename T> vector<string> MakeLeafVector(optional<string> tag = nullopt) {

      vector<string> leaves = T::GetListOfMembers();
      if (tag.has_value()) {
        AddTagToLeaves(tag.value(), leaves);
      }
      return leaves;

    }  // end 'template <> MakeLeafVector(optional<string>)'



    // generic function to append leaf names onto a vector
    template <typename T> void AddLeavesToVector(vector<string>& vecToAddTo, optional<string> tag = nullopt) {

      vector<string> addends = MakeLeafVector<T>(tag);
      for (auto addend : addends) {
        vecToAddTo.push_back(addend);
      }
      return;

    }  // end 'template <> AddLeavesToVector(vector<string>&, optional<string>)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

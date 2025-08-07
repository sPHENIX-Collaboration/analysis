/// ---------------------------------------------------------------------------
/*! \file   NodeInterfaces.cc
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  F4A node-related interfaces.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_NODEINTERFACES_CC

// namespace definition
#include "NodeInterfaces.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



// node interfaces ============================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Remove forbidden characters from a node name
  // --------------------------------------------------------------------------
  void Interfaces::CleanseNodeName(string& nameToClean) {

    for (const auto& [bad, good] : Const::MapBadOntoGoodStrings()) {
      size_t position;
      while ((position = nameToClean.find(bad)) != string::npos) {
        nameToClean.replace(position, 1, good);
      }
    }  // end bad-good pair loop
    return;

  }  // end 'CleanseNodeName(string&)'



  // --------------------------------------------------------------------------
  //! Create node
  // --------------------------------------------------------------------------
  template <typename T> void Interfaces::CreateNode(
    PHCompositeNode* topNode,
    string newNodeName, T& objectInNode
  ) {

    // make sure node name is okay
    CleanseNodeName(newNodeName);

    // find DST node
    PHNodeIterator   itNode(topNode);
    PHCompositeNode* dstNode = dynamic_cast<PHCompositeNode*>(itNode.findFirst("PHCompositeNode", "DST"));
    if (!dstNode) {
      dstNode = new PHCompositeNode("DST");
      topNode -> addNode(dstNode);
    }

    // create node and exit
    PHIODataNode<PHObject>* newNode = new PHIODataNode<PHObject>(objectInNode, newNodeName.c_str(), "PHObject");
    dstNode -> addNode(newNode);
    return;

  }  // end 'CreateNode(PHCompositeNode*, string, T&)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

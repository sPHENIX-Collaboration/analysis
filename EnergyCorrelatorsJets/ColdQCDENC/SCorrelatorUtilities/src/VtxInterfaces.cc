/// ---------------------------------------------------------------------------
/*! \file   VtxInterfaces.cc
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  Vertex-related interfaces.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_VTXINTERFACES_CC

// namespace definition
#include "VtxInterfaces.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



// vertex interfaces ==========================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get vertex map from node tree
  // --------------------------------------------------------------------------
  GlobalVertexMap* Interfaces::GetVertexMap(PHCompositeNode* topNode) {

    // get vertex map
    GlobalVertexMap* mapVtx = getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

    // check if good
    const bool isVtxMapGood = (mapVtx && !(mapVtx -> empty()));
    if (!isVtxMapGood) {
      cerr << PHWHERE
           << "PANIC: GlobalVertexMap node is missing or empty!\n"
           << "       Please turn on the do_global flag in the main macro in order to reconstruct the global vertex!"
           << endl;
      assert(isVtxMapGood);
    }
    return mapVtx;

  }  // end 'GetVertexMap(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Get a specific vertex from vertex map
  // -------------------------------------------------------------------------
  /*! If no index is provided, returns the primary
   *  vertex (stored as the 1st vertex in the map)
   */ 
  GlobalVertex* Interfaces::GetGlobalVertex(PHCompositeNode* topNode, optional<int> iVtxToGrab) {

    // get vertex map
    GlobalVertexMap* mapVtx = GetVertexMap(topNode);

    // get specified vertex
    GlobalVertex* vtx = NULL;
    if (iVtxToGrab.has_value()) {
      vtx = mapVtx -> get(iVtxToGrab.value());
    } else {
      vtx = mapVtx -> begin() -> second;
    }

    // check if good
    if (!vtx) {
      cerr << PHWHERE
           << "PANIC: no vertex!"
           << endl;
      assert(vtx);
    }
    return vtx;

  }  // end 'GetGlobalVertex(PHCompositeNode*, optional<int>)'



  // --------------------------------------------------------------------------
  //! Get primary vertex as a ROOT XYZVector
  // --------------------------------------------------------------------------
  ROOT::Math::XYZVector Interfaces::GetRecoVtx(PHCompositeNode* topNode) {

    const GlobalVertex* vtx = GetGlobalVertex(topNode);
    return ROOT::Math::XYZVector(vtx -> get_x(), vtx -> get_y(), vtx -> get_z());

  }  // end 'GetRecoVtx(PHCompositeNode*)'

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

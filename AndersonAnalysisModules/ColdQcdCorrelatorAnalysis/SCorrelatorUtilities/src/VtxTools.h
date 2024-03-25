// ----------------------------------------------------------------------------
// 'VtxTools.h'
// Derek Anderson
// 12.30.2023
//
// Collection of frequent vertex-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// c++ utilities
#include <array>
#include <cassert>
#include <utility>
#include <optional>
// root libraries
#include <Math/Vector3D.h>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// vertex libraries
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // event methods ----------------------------------------------------------

    GlobalVertexMap* GetVertexMap(PHCompositeNode* topNode) {

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



    GlobalVertex* GetGlobalVertex(PHCompositeNode* topNode, optional<int> iVtxToGrab = nullopt) {

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



    ROOT::Math::XYZVector GetRecoVtx(PHCompositeNode* topNode) {

      const GlobalVertex* vtx = GetGlobalVertex(topNode);
      return ROOT::Math::XYZVector(vtx -> get_x(), vtx -> get_y(), vtx -> get_z());

    }  // end 'GetRecoVtx(PHCompositeNode*)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------


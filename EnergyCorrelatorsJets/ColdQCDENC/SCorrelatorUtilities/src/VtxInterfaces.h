/// ---------------------------------------------------------------------------
/*! \file   VtxInterfaces.h
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  Vertex-related interfaces.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_VTXINTERFACES_H
#define SCORRELATORUTILITIES_VTXINTERFACES_H

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



namespace SColdQcdCorrelatorAnalysis {
  namespace Interfaces {

    // vertex interfaces ------------------------------------------------------

    GlobalVertexMap*      GetVertexMap(PHCompositeNode* topNode);
    GlobalVertex*         GetGlobalVertex(PHCompositeNode* topNode, optional<int> iVtxToGrab = nullopt);
    ROOT::Math::XYZVector GetRecoVtx(PHCompositeNode* topNode);

  }  // end Interfaces namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------


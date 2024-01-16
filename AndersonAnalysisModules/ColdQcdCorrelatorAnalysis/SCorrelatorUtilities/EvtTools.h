// ----------------------------------------------------------------------------
// 'SCorrelatorUtilities.EvtTools.h'
// Derek Anderson
// 10.30.2023
//
// Collection of frequent event-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// c++ utilities
#include <array>
#include <cassert>
#include <utility>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// vertex libraries
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
// analysis utilities
#include "Constants.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // EvtInfo definition -----------------------------------------------------

    struct EvtInfo {

      // data members
      // TODO add energy sums, etc
      int    m_numJets = -1;
      int    m_numTrks = -1;
      double m_vtxX    = -999.;
      double m_vtxY    = -999.;
      double m_vtxZ    = -999.;

      void SetInfo(int nJet, int nTrk, double vX, double vY, double vZ) {
        m_numJets = nJet;
        m_numTrks = nTrk;
        m_vtxX    = vX;
        m_vtxY    = vY;
        m_vtxZ    = vZ;
        return;
      }  // end 'SetInfo(int, int, double, double, double)'

      void Reset() {
        m_numJets = -1;
        m_numTrks = -1;
        m_vtxX    = -999.;
        m_vtxY    = -999.;
        m_vtxZ    = -999.;
        return;
      }  // end 'Reset()'

    };  // end EvtInfo def



    // event methods ----------------------------------------------------------

    GlobalVertexMap* GetVertexMap(PHCompositeNode* topNode) {

      // get vertex map
      GlobalVertexMap* mapVtx = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

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



    GlobalVertex* GetGlobalVertex(PHCompositeNode* topNode, const int iVtxToGrab = -1) {

      // get vertex map
      GlobalVertexMap* mapVtx = GetVertexMap(topNode);

      // get specified vertex
      GlobalVertex* vtx = NULL;
      if (iVtxToGrab < 0) {
        vtx = mapVtx -> begin() -> second;
      } else {
        vtx = mapVtx -> get(iVtxToGrab);
      }

      // check if good
      if (!vtx) {
        cerr << PHWHERE
             << "PANIC: no vertex!"
             << endl;
        assert(vtx);
      }
      return vtx;

    }  // end 'GetGlobalVertex(PHCompositeNode*, int)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

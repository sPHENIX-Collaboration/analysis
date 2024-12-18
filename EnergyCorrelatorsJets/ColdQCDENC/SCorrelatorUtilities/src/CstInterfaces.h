/// ---------------------------------------------------------------------------
/*! \file   CstInterfaces.h
 *  \author Derek Anderson
 *  \date   08.15.2024
 *
 *  Jet constituent-related interfaces.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_CSTINTERFACES_H
#define SCORRELATORUTILITIES_CSTINTERFACES_H

// c++ utilities
#include <cassert>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// PHG4 libraries
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
// calobase libraries
#include <calobase/RawTower.h>
#include <calobase/TowerInfo.h>
#include <calobase/RawCluster.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/RawClusterContainer.h>
// trackbase libraries
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
// particle flow libraries
#include <particleflowreco/ParticleFlowElement.h>
#include <particleflowreco/ParticleFlowElementContainer.h>
// jet libraries
#include <jetbase/Jet.h>
// analysis utilities
#include "ParInterfaces.h"
#include "TrkInterfaces.h"
#include "TwrInterfaces.h"
#include "FlowInterfaces.h"
#include "ClustInterfaces.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Interfaces {

    // cst interfaces -------------------------------------------------------

    SvtxTrack*           FindTrack(const uint32_t idToFind, PHCompositeNode* topNode);
    ParticleFlowElement* FindFlow(const uint32_t idToFind, PHCompositeNode* topNode);
    RawTower*            FindRawTower(const uint32_t idToFind, const Jet::SRC source, PHCompositeNode* topNode);
    TowerInfo*           FindTowerInfo(const uint32_t idToFind, const Jet::SRC source, PHCompositeNode* topNode);    
    RawCluster*          FindCluster(const uint32_t idToFind, const Jet::SRC source, PHCompositeNode* topNode);
    PHG4Particle*        FindParticle(const int32_t idToFind, PHCompositeNode* topNode);

  }  // end Interfaces namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

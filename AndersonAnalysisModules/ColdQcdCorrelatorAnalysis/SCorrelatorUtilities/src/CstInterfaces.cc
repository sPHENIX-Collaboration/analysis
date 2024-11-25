/// ---------------------------------------------------------------------------
/*! \file   CstInterfaces.cc
 *  \author Derek Anderson
 *  \date   08.15.2024
 *
 *  Jet constituent-related interfaces.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_CSTINTERFACES_CC

// namespace definition 
#include "CstInterfaces.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



// cst interfaces =============================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Find SvtxTrack based on provided ID
  // --------------------------------------------------------------------------
  SvtxTrack* Interfaces::FindTrack(const uint32_t idToFind, PHCompositeNode* topNode) {

    // grab track map
    SvtxTrackMap* map = Interfaces::GetTrackMap(topNode);

    // hunt down track
    SvtxTrack* trkToFind = nullptr;
    for (
      SvtxTrackMap::Iter itTrk = map -> begin();
      itTrk != map -> end();
      ++itTrk
    ) {

      // grab track
      SvtxTrack* track = itTrk -> second;
      if (!track) continue;

      // check id, break if found
      if (idToFind == track -> get_id()) {
        trkToFind = track;
        break;
      }
    }  // end track loop
    return trkToFind;

  }  // end 'FindTrack(uint32_t, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Find PFO based on provided ID
  // --------------------------------------------------------------------------
  ParticleFlowElement* Interfaces::FindFlow(const uint32_t idToFind, PHCompositeNode* topNode) {

    // grab pfo container
    ParticleFlowElementContainer* flows = Interfaces::GetFlowStore(topNode);

    // hunt down pfo
    ParticleFlowElement* pfoToFind = nullptr;
    for (
      ParticleFlowElementContainer::ConstIterator itFlow = flows -> getParticleFlowElements().first;
      itFlow != flows -> getParticleFlowElements().second;
      ++itFlow
    ) {

      // get pf element
      ParticleFlowElement* flow = itFlow -> second;
      if (!flow) continue;

      // check id, break if found
      if (idToFind == flow -> get_id()) {
        pfoToFind = flow;
        break;
      }
    }  // end pfo loop
    return pfoToFind;

  }  // end 'FindFlow(uint32_t, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Find RawTower based on provided ID
  // --------------------------------------------------------------------------
  RawTower* Interfaces::FindRawTower(const uint32_t idToFind, const Jet::SRC source, PHCompositeNode* topNode) {

    // grab relevant raw towers
    RawTowerContainer::ConstRange towers = Interfaces::GetRawTowers(topNode, Const::MapSrcOntoNode()[source]);

    // hunt down tower
    RawTower* twrToFind = nullptr;
    for (
      RawTowerContainer::ConstIterator itTwr = towers.first;
      itTwr != towers.second;
      ++itTwr
    ) {

      // get tower
      RawTower* tower = itTwr -> second;
      if (!tower) continue;

      // check id, break if found
      if (idToFind == tower -> get_id()) {
        twrToFind = tower;
        break;
      }
    }  // end tower loop
    return twrToFind;

  }  // end 'FindRawTower(uint32_t, Jet::SRC, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Find TowerInfo based on provided ID
  // --------------------------------------------------------------------------
  TowerInfo* Interfaces::FindTowerInfo(const uint32_t idToFind, const Jet::SRC source, PHCompositeNode* topNode) {

    // grab relevant tower info container
    TowerInfoContainer* towers = Interfaces::GetTowerInfoStore(topNode, Const::MapSrcOntoNode()[source]);

    // hunt down tower
    TowerInfo* twrToFind = nullptr;
    for (uint32_t channel = 0; channel < towers -> size(); channel++) {

      // get tower
      TowerInfo* tower = towers -> get_tower_at_channel(channel);
      if (!tower) continue;

      // check id (channel), break if found
      if (idToFind == channel) {
        twrToFind = tower;
        break;
      }
    }  // end tower loop
    return twrToFind;

  }  // end 'FindTowerInfo(uint32_t, Jet::SRC, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Find RawCluster based on provided ID
  // --------------------------------------------------------------------------
  RawCluster* Interfaces::FindCluster(const uint32_t idToFind, const Jet::SRC source, PHCompositeNode* topNode) {

    // grab relevant clusters
    RawClusterContainer::ConstRange clusters = Interfaces::GetClusters(topNode, Const::MapSrcOntoNode()[source]);

    // hunt down cluster
    RawCluster* clustToFind = nullptr;
    for (
      RawClusterContainer::ConstIterator itClust = clusters.first;
      itClust != clusters.second;
      ++itClust
    ) {

      // get cluster
      RawCluster* cluster = itClust -> second;
      if (!cluster) continue;

      // check id, break if found
      if (idToFind == cluster -> get_id()) {
        clustToFind = cluster;
        break;
      }
    }  // end cluster loop
    return clustToFind;

  }  // end 'FindCluster(uint32_t, Jet::SRC, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Find PHG4Particle based on provided ID
  // --------------------------------------------------------------------------
  PHG4Particle* Interfaces::FindParticle(const int32_t idToFind, PHCompositeNode* topNode) {

    // grab primary particles
    PHG4TruthInfoContainer::ConstRange primaries = Interfaces::GetPrimaries(topNode);

    // hunt down particle
    PHG4Particle* parToFind = nullptr;
    for (
      PHG4TruthInfoContainer::ConstIterator itPar = primaries.first;
      itPar != primaries.second;
      ++itPar
    ) {

      // grab particle
      PHG4Particle* particle = itPar -> second;
      if (!particle) continue;

      // check id, break if found
      if (idToFind == particle -> get_track_id()) {
        parToFind = particle;
        break;
      }
    }  // end particle loop
    return parToFind;

  }  // end 'FindParticle(uint32_t, PHCompositeNode*)'

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

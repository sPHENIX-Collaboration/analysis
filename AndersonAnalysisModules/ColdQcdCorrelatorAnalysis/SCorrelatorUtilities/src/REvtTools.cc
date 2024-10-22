/// ---------------------------------------------------------------------------
/*! \file   REvtTools.cc
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *  Collection of frequent event-level reconstruction methods utilized
 *  in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_REVTTOOLS_CC

// namespace definition
#include "REvtTools.h"

// make common namespaces implicit
using namespace std;



// event-level reconstructed methods ==========================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get total number of tracks
  // --------------------------------------------------------------------------
  int64_t Tools::GetNumTrks(PHCompositeNode* topNode) {

    // grab size of track map
    SvtxTrackMap* mapTrks = Interfaces::GetTrackMap(topNode);
    return mapTrks -> size();

  }  // end 'GetNumTrks(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Get sum of track momentum
  // --------------------------------------------------------------------------
  double Tools::GetSumTrkMomentum(PHCompositeNode* topNode) {

    // grab track map
    SvtxTrackMap* mapTrks = Interfaces::GetTrackMap(topNode);

    // loop over tracks
    double pSum = 0.;
    for (
      SvtxTrackMap::Iter itTrk = mapTrks -> begin();
      itTrk != mapTrks -> end();
      ++itTrk
    ) {

      // grab track
      SvtxTrack* track = itTrk -> second;
      if (!track) continue;

      // momentum to sum
      const double pTrk = std::hypot(track -> get_px(), track -> get_py(), track -> get_pz());
      pSum += pTrk;
    }
    return pSum;

  }  // end 'GetSumTrkMomentum(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Get sum of energy from a calorimeter
  // --------------------------------------------------------------------------
  double Tools::GetSumCaloEne(PHCompositeNode* topNode, const string store) {

    // grab clusters
    RawClusterContainer::ConstRange clusters = Interfaces::GetClusters(topNode, store);

    // loop over clusters
    double eSum = 0.;
    for (
      RawClusterContainer::ConstIterator itClust = clusters.first;
      itClust != clusters.second;
      itClust++
    ) {

      // grab cluster and increment sum
      const RawCluster* cluster = itClust -> second;
      if (!cluster) {
        continue;
      } else {
        eSum += cluster -> get_energy();
      }
    }  // end cluster loop
    return eSum;

  }  // end 'GetSumCaloEne(PHCompositeNode*, string)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

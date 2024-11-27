/// ---------------------------------------------------------------------------
/*! \file   TrkTools.cc
 *  \author Derek Anderson
 *  \date   02.29.2024
 *
 *  Collection of frequent track-related methods utilized
 *  in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_TRKTOOLS_CC

// namespace definition
#include "TrkTools.h"

// make common namespaces implicit
using namespace std;



// track methods ==============================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get no. of layers hit from a specific subsystem
  // --------------------------------------------------------------------------
  int Tools::GetNumLayer(SvtxTrack* track, const int16_t sys) {

    // grab track seed
    TrackSeed* seed = GetTrackSeed(track, sys);

    // set min no. of layers
    const int minInttLayer = Const::NMvtxLayer();
    const int minTpcLayer  = Const::NMvtxLayer() + Const::NInttLayer();

    // initialize hit flags
    vector<bool> isMvtxLayerHit( Const::NMvtxLayer(), false );
    vector<bool> isInttLayerHit( Const::NInttLayer(), false );
    vector<bool> isTpcLayerHit(  Const::NTpcLayer(),  false  );

    // determine which layers were hit
    int layer     = 0;
    int mvtxLayer = 0;
    int inttLayer = 0;
    int tpcLayer  = 0;
    for (
      auto itClustKey = (seed -> begin_cluster_keys());
      itClustKey != (seed -> end_cluster_keys());
      ++itClustKey
    ) {

      // grab layer number
      layer = TrkrDefs::getLayer(*itClustKey);

      // increment accordingly
      switch (sys) {
        case Const::Subsys::Mvtx:
          if (layer < Const::NMvtxLayer()) {
            mvtxLayer                 = layer;
            isMvtxLayerHit[mvtxLayer] = true;
          }
          break;
        case Const::Subsys::Intt:
          if ((layer >= minInttLayer) && (layer < minTpcLayer)) {
            inttLayer                 = layer - minInttLayer;
            isInttLayerHit[inttLayer] = true;
          }
          break;
        case Const::Subsys::Tpc:
          if (layer >= minTpcLayer) {
            tpcLayer                = layer - minTpcLayer;
            isTpcLayerHit[tpcLayer] = true;
          }
          break;
        default:
          break;
      }
    }  // end cluster loop

    // get the relevant no. of layers
    int nLayer = 0;
    switch (sys) {
      case Const::Subsys::Mvtx:
        for (int iMvtxLayer = 0; iMvtxLayer < Const::NMvtxLayer(); iMvtxLayer++) {
          if (isMvtxLayerHit[iMvtxLayer]) ++nLayer;
        }
        break;
      case Const::Subsys::Intt:
        for (int iInttLayer = 0; iInttLayer < Const::NInttLayer(); iInttLayer++) {
          if (isInttLayerHit[iInttLayer]) ++nLayer;
        }
        break;
      case Const::Subsys::Tpc:
        for (int iTpcLayer = 0; iTpcLayer < Const::NTpcLayer(); iTpcLayer++) {
          if (isTpcLayerHit[iTpcLayer]) ++nLayer;
        }
        break;
      default:
        break;
    }
    return nLayer;

  }  // end 'GetNumLayer(SvtxTrack*, int16_t)'



  // --------------------------------------------------------------------------
  //! Get no. of clusters from a specific subsystem
  // --------------------------------------------------------------------------
  int Tools::GetNumClust(SvtxTrack* track, const int16_t sys) {

    // grab track seed
    TrackSeed* seed = GetTrackSeed(track, sys);

    // set min no. of layers
    const int minInttLayer = Const::NMvtxLayer();
    const int minTpcLayer  = Const::NMvtxLayer() + Const::NInttLayer();

    // determine no. of clusters for a given layer
    int layer    = 0;
    int nCluster = 0;
    for (auto itClustKey = (seed -> begin_cluster_keys()); itClustKey != (seed -> end_cluster_keys()); ++itClustKey) {

      // grab layer number
      layer = TrkrDefs::getLayer(*itClustKey);

      // increment accordingly
      switch (sys) {
        case Const::Subsys::Mvtx:
          if (layer < Const::NMvtxLayer()) {
            ++nCluster;
          }
          break;
        case Const::Subsys::Intt:
          if ((layer >= minInttLayer) && (layer < minTpcLayer)) {
            ++nCluster;
          }
          break;
        case Const::Subsys::Tpc:
          if (layer >= minTpcLayer) {
            ++nCluster;
          }
          break;
        default:
          break;
      }
    }  // end cluster loop
    return nCluster;

  }  // end 'GetNumClust(SvtxTrack*, int16_t)'



  // --------------------------------------------------------------------------
  //! Get barcode of matching truth particle
  // --------------------------------------------------------------------------
  int Tools::GetMatchID(SvtxTrack* track, SvtxTrackEval* trackEval) {

    // get best match from truth particles
    PHG4Particle* bestMatch = trackEval -> max_truth_particle_by_nclusters(track);

    // grab barcode of best match
    int matchID;
    if (bestMatch) {
      matchID = bestMatch -> get_barcode();
    } else {
      matchID = numeric_limits<int>::max();
    }
    return matchID;

  }  // end 'GetMatchID(SvtxTrack*)'



  // --------------------------------------------------------------------------
  //! Does track have the relevant seed?
  // --------------------------------------------------------------------------
  bool Tools::IsGoodTrackSeed(SvtxTrack* track, const bool requireSiSeeds) {

    // get track seeds
    TrackSeed* trkSiSeed  = track -> get_silicon_seed();
    TrackSeed* trkTpcSeed = track -> get_tpc_seed();

    // check if one or both seeds are present as needed
    bool isSeedGood = (trkSiSeed && trkTpcSeed);
    if (!requireSiSeeds) {
      isSeedGood = (trkSiSeed || trkTpcSeed);
    }
    return isSeedGood;

  }  // end 'IsGoodSeed(SvtxTrack*)'



  // --------------------------------------------------------------------------
  //! Is track connected to the primary vertex?
  // --------------------------------------------------------------------------
  bool Tools::IsFromPrimaryVtx(SvtxTrack* track, PHCompositeNode* topNode) {

    // get id of vertex associated with track
    const int vtxID = (int) track -> get_vertex_id();

    // get id of primary vertex
    GlobalVertex* primVtx   = Interfaces::GetGlobalVertex(topNode);
    const int     primVtxID = primVtx -> get_id();

    // check if from vertex and return
    const bool isFromPrimVtx = (vtxID == primVtxID);
    return isFromPrimVtx;

  }  // end 'IsFromPrimaryVtx(SvtTrack*, PHCompsiteNode*)'



  // --------------------------------------------------------------------------
  //! Get uncertainty on track pt from fit
  // --------------------------------------------------------------------------
  double Tools::GetTrackDeltaPt(SvtxTrack* track) {

    // grab covariances
    const float trkCovXX = track -> get_error(3, 3);
    const float trkCovXY = track -> get_error(3, 4);
    const float trkCovYY = track -> get_error(4, 4);

    // grab momentum
    const float trkPx = track -> get_px();
    const float trkPy = track -> get_py();
    const float trkPt = track -> get_pt();
 
    // calculate delta-pt
    const float numer    = (trkCovXX * trkPx * trkPx) + 2 * (trkCovXY * trkPx * trkPy) + (trkCovYY * trkPy * trkPy);
    const float denom    = (trkPx * trkPx) + (trkPy * trkPy); 
    const float ptDelta2 = numer / denom;
    const float ptDelta  = sqrt(ptDelta2) / trkPt;
    return ptDelta;

  }  // end 'GetTrackDeltaPt(SvtxTrack*)'



  // --------------------------------------------------------------------------
  //! Get track XY and Z DCA
  // --------------------------------------------------------------------------
  pair<double, double> Tools::GetTrackDcaPair(SvtxTrack* track, PHCompositeNode* topNode) {

    // get global vertex and convert to acts vector
    GlobalVertex* sphxVtx = Interfaces::GetGlobalVertex(topNode);
    Acts::Vector3 actsVtx = Acts::Vector3(sphxVtx -> get_x(), sphxVtx -> get_y(), sphxVtx -> get_z());

    // return dca
    const auto dcaAndErr = TrackAnalysisUtils::get_dca(track, actsVtx);
    return make_pair(dcaAndErr.first.first, dcaAndErr.second.first);

  }  // end 'GetTrackDcaPair(SvtxTrack*, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Get track seed
  // --------------------------------------------------------------------------
  TrackSeed* Tools::GetTrackSeed(SvtxTrack* track, const int16_t sys) {

    // get both track seeds
    TrackSeed* trkSiSeed  = track -> get_silicon_seed();
    TrackSeed* trkTpcSeed = track -> get_tpc_seed();

    // select relevant seed
    const bool hasBothSeeds   = (trkSiSeed  && trkTpcSeed);
    const bool hasOnlyTpcSeed = (!trkSiSeed && trkTpcSeed);

    TrackSeed* seed = NULL;
    switch (sys) {
      case Const::Subsys::Mvtx:
        if (hasBothSeeds)   seed = trkSiSeed;
        if (hasOnlyTpcSeed) seed = trkTpcSeed;
        break;
      case Const::Subsys::Intt:
        if (hasBothSeeds)   seed = trkSiSeed;
        if (hasOnlyTpcSeed) seed = trkTpcSeed;
        break;
      case Const::Subsys::Tpc:
        seed = trkTpcSeed;
        break;
      default:
        seed = NULL;
        break;
    }
    return seed;

  }  // end 'GetTrackSeed(SvtxTrack*, int16_t)'



  // --------------------------------------------------------------------------
  //! Get track's associated vertex
  // --------------------------------------------------------------------------
  ROOT::Math::XYZVector Tools::GetTrackVertex(SvtxTrack* track, PHCompositeNode* topNode) {

    // get vertex associated with track
    const int     vtxID = (int) track -> get_vertex_id();
    GlobalVertex* vtx   = Interfaces::GetGlobalVertex(topNode, vtxID);

    // return vertex 3-vector
    ROOT::Math::XYZVector xyzVtx = ROOT::Math::XYZVector(vtx -> get_x(), vtx -> get_y(), vtx -> get_z());
    return xyzVtx;

  }  // end 'GetTrackVertex(SvtxTrack*, PHCompositeNode*)'

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

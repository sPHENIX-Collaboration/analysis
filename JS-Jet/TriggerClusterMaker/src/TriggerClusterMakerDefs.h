// ----------------------------------------------------------------------------
/*! \file    TriggerClusterMakerDefs.h'
 *  \authors Derek Anderson
 *  \date    05.29.2024
 *
 *  Miscellaneous definitions for the TriggerClusterMaker
 *  fun4all module
 */
// ----------------------------------------------------------------------------

#ifndef TRIGGERCLUSTERMAKERDEFS_H
#define TRIGGERCLUSTERMAKERDEFS_H

// c++ utilities
#include <limits>
#include <utility>
// calo base
#include <calobase/TowerInfoDefs.h>
// trigger libraries
//   - TODO use local paths when ready
#include <calotrigger/TriggerDefs.h>



// ----------------------------------------------------------------------------
//! Miscellaneous tools for the TriggerClusterMaker module
// ----------------------------------------------------------------------------
namespace TriggerClusterMakerDefs {

  // enums --------------------------------------------------------------------

  // calo layers
  enum Cal {
    EM,
    IH,
    OH
  };

  // eta vs. phi bin
  enum Axis {
    Eta,
    Phi
  };

  // trigger info type
  enum Type {
    Prim,
    LL1
  };



  // constants ----------------------------------------------------------------

  // --------------------------------------------------------------------------
  //! No. of EMCal towers in a retower
  // --------------------------------------------------------------------------
  inline uint32_t NTowInRetow() {
    static const uint32_t nTowInRetow = 4;
    return nTowInRetow;
  }

  // --------------------------------------------------------------------------
  //! No. of HCal towers (EMCal retowers) along an LL1
  // --------------------------------------------------------------------------
  inline uint32_t NTowInLL1() {
    static const uint32_t nTowInLL1 = 4;
    return nTowInLL1;
  }

  // --------------------------------------------------------------------------
  //! No. of HCal towers (EMCal retowers) along a side of a trigger primitive
  // --------------------------------------------------------------------------
  inline uint32_t NTowInPrim() {
    static const uint32_t nTowInPrim = 2;
    return nTowInPrim;
  }



  // methods ------------------------------------------------------------------

  // --------------------------------------------------------------------------
  //! Calculate eta/phi bin based on sum key
  // --------------------------------------------------------------------------
  uint32_t GetBin(const uint32_t sumkey, const uint32_t axis, const uint32_t type = Type::Prim) {

    // get relevant sum and primitive IDs
    uint32_t sumID;
    uint32_t primID;
    switch (axis) {
      case Axis::Eta:
        sumID  = TriggerDefs::getSumEtaId(sumkey);
        primID = TriggerDefs::getPrimitiveEtaId_from_TriggerSumKey(sumkey);
        break;
      case Axis::Phi:
        sumID  = TriggerDefs::getSumPhiId(sumkey);
        primID = TriggerDefs::getPrimitivePhiId_from_TriggerSumKey(sumkey);
        break;
      default:
        sumID  = std::numeric_limits<uint32_t>::max();
        primID = std::numeric_limits<uint32_t>::max();
        break;
    }

    // get relevant segmentation
    uint32_t segment;
    switch (type) {
      case Type::LL1:
        segment = NTowInLL1();
        break;
      case Type::Prim:
        [[fallthrough]];
      default:
        segment = NTowInPrim();
        break;
    }
    return sumID + (segment * primID);

  }  // end 'GetBin(uint32_t, uint32_t, uint32_t)'



  // --------------------------------------------------------------------------
  //! Get tower key based on provided eta, phi indices
  // --------------------------------------------------------------------------
  uint32_t GetKeyFromEtaPhiIndex(const uint32_t eta, const uint32_t phi, const uint32_t det) {

    uint32_t key;
    switch (det) {

      // get emcal tower index
      case TriggerDefs::DetectorId::emcalDId:
        key = TowerInfoDefs::encode_emcal(eta, phi);
        break;

      // get hcal tower index
      case TriggerDefs::DetectorId::hcalinDId:
        [[fallthrough]];
      case TriggerDefs::DetectorId::hcaloutDId:
        [[fallthrough]];
      case TriggerDefs::DetectorId::hcalDId:
        key = TowerInfoDefs::encode_hcal(eta, phi);
        break;

      // otherwise return dummy value
      default:
        key = std::numeric_limits<uint32_t>::max();
        break;
    }
    return key;

  } //  end 'GetKeyFromEtaPhiIndex(uint32_t, uint32_t, uint32_t)'



  // --------------------------------------------------------------------------
  //! Get range of tower indices for a specified direction from a starting point
  // --------------------------------------------------------------------------
  std::pair<uint32_t, uint32_t> GetRangeOfIndices(
    const uint32_t iStartTrg,
    const uint32_t detector,
    const uint32_t type = Type::Prim
  ) {

    uint32_t iStart;
    uint32_t iStop;
    switch (detector) {

      // get max emcal tower index
      case TriggerDefs::DetectorId::emcalDId:
        iStart = iStartTrg;
        iStop  = iStartTrg + (2 * NTowInRetow()) - 1;
        break;

      // get max hcal tower index
      case TriggerDefs::DetectorId::hcalinDId:
        [[fallthrough]];
      case TriggerDefs::DetectorId::hcaloutDId:
        [[fallthrough]];
      case TriggerDefs::DetectorId::hcalDId:
        iStart = iStartTrg;  // TODO implement
        iStop  = iStartTrg;  // TODO implement
        break;

      // otherwise set both to start
      default:
        iStart = iStartTrg;
        iStop  = iStartTrg;
        break;
    }
    return std::make_pair(iStart, iStop);

  }  // end 'GetRangeOfIndeices(uint32_t, uint32_t, uint32_t)'

}  // end TriggerClusterMakerDefs namespace

#endif

// end ------------------------------------------------------------------------

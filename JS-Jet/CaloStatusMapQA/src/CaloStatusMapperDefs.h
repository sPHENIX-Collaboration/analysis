///  --------------------------------------------------------------------------
/*! \file   CaloStatusMapperDefs.h
 *  \author Derek Anderson
 *  \date   05.22.2024
 *
 *  A Fun4All QA module to plot no. of towers per event
 *  and vs. eta, phi as a function of status.
 */
///  --------------------------------------------------------------------------

#ifndef CLUSTERSTATUSMAPPER_DEFS_H
#define CLUSTERSTATUSMAPPER_DEFS_H

// c++ utilities
#include <map>
#include <string>
#include <utility>
#include <vector>
// calobase libraries
#include <calobase/TowerInfov2.h>

// forward declarations
class TH1D;
class TH2D;



// ----------------------------------------------------------------------------
//! Miscellaneous definitions for CaloStatusMapper
// ----------------------------------------------------------------------------
/*! A namespace to collect various constants, helper
 *  methods, etc. used in the CaloStatusMapper
 *  module.
 */
namespace CaloStatusMapperDefs {

  // convenience types --------------------------------------------------------

  typedef std::pair<std::string, int> NodeDef;
  typedef std::tuple<uint32_t, float, float> BinDef;
  typedef std::vector<std::vector<std::vector<TH1D*>>> H1DVec;
  typedef std::vector<std::vector<std::vector<TH2D*>>> H2DVec;



  // enums --------------------------------------------------------------------

  enum Calo {EMC, IHC, OHC, ZDC, SEPD, NONE};
  enum Stat {Good, Hot, BadTime, BadChi, NotInstr, NoCalib, Unknown};
  enum H1D  {Num, Eta, Phi};
  enum H2D  {EtaVsPhi};



  // constants ----------------------------------------------------------------

  // total no. of towers
  inline std::vector<uint32_t> const& NTowers() {
    static std::vector<uint32_t> nTowers = {
      24576,
      1536,
      1536,
      52,
      744
    };
    return nTowers;
  }

  // total no. of towers along eta
  inline std::vector<uint32_t> const& NEta() {
    static std::vector<uint32_t> nEta = {
      96,
      24,
      24,
      52,  // FIXME this isn't the best representation for the ZDC
      744  // FIXME same for the sEPD...
    };
    return nEta;
  }

  // total no. of towers along phi
  inline std::vector<uint32_t> const& NPhi() {
    static std::vector<uint32_t> nPhi = {
      256,
      64,
      64,
      52,  // FIXME like with eta, this isn't good for the ZDC
      744  // FIXME ditto for the sEPD...
    };
    return nPhi;
  }



  // histogram definitions ----------------------------------------------------

  // status labels
  inline std::vector<std::string> const& StatLabels() {
    static std::vector<std::string> statLabels = {
      "Good",
      "Bad",
      "Hot",
      "BadTime",
      "NotIntsr",
      "NoCalib"
    };
    return statLabels;
  }

  // 1D histogram labels
  inline std::vector<std::string> const& H1DLabels() {
    static std::vector<std::string> labelsH1D = {
      "NumberPerEvt",
      "NumberPerEta",
      "NumberPerPhi"
    };
    return labelsH1D;
  }

  // 2D histogram labels
  inline std::vector<std::string> const& H2DLabels() {
    static std::vector<std::string> labelsH2D = {
      "EtaVsPhi"
    };
    return labelsH2D;
  }

  // total number axis definitions
  inline std::vector<BinDef> const& NumBins() {
    static std::vector<BinDef> numBins = {
      std::make_tuple(NTowers()[Calo::EMC],  -0.5, NTowers()[Calo::EMC] - 0.5),
      std::make_tuple(NTowers()[Calo::IHC],  -0.5, NTowers()[Calo::IHC] - 0.5),
      std::make_tuple(NTowers()[Calo::OHC],  -0.5, NTowers()[Calo::OHC] - 0.5),
      std::make_tuple(NTowers()[Calo::ZDC],  -0.5, NTowers()[Calo::ZDC] - 0.5),
      std::make_tuple(NTowers()[Calo::SEPD], -0.5, NTowers()[Calo::SEPD] - 0.5)
    };
    return numBins;
  }

  // number per eta axis definitions
  inline std::vector<BinDef> const& EtaBins() {
    static std::vector<BinDef> etaBins = {
      std::make_tuple(NEta()[Calo::EMC],  -0.5, NEta()[Calo::EMC] - 0.5),
      std::make_tuple(NEta()[Calo::IHC],  -0.5, NEta()[Calo::IHC] - 0.5),
      std::make_tuple(NEta()[Calo::OHC],  -0.5, NEta()[Calo::OHC] - 0.5),
      std::make_tuple(NEta()[Calo::ZDC],  -0.5, NEta()[Calo::ZDC] - 0.5),
      std::make_tuple(NEta()[Calo::SEPD], -0.5, NEta()[Calo::SEPD] - 0.5)
    };
    return etaBins;
  }

  // number per eta axis definitions
  inline std::vector<BinDef> const& PhiBins() {
    static std::vector<BinDef> phiBins = {
      std::make_tuple(NPhi()[Calo::EMC],  -0.5, NPhi()[Calo::EMC] - 0.5),
      std::make_tuple(NPhi()[Calo::IHC],  -0.5, NPhi()[Calo::IHC] - 0.5),
      std::make_tuple(NPhi()[Calo::OHC],  -0.5, NPhi()[Calo::OHC] - 0.5),
      std::make_tuple(NPhi()[Calo::ZDC],  -0.5, NPhi()[Calo::ZDC] - 0.5),
      std::make_tuple(NPhi()[Calo::SEPD], -0.5, NPhi()[Calo::SEPD] - 0.5)
    };
    return phiBins;
  }


  // make total number histogram
  //   - TODO simplify by smushing all these inlined functions into
  //     one simple function
  TH1D* MakeNumHistogram(const std::string name, const int type = Calo::



  // helper methods -----------------------------------------------------------

  int GetTowerStatus(TowerInfo* tower) {

    int status = Stat::Unknown;
    if (tower -> get_isHot()) {
      status = Stat::Hot;
    } else if (tower -> get_isBadTime()) {
      status = Stat::BadTime;
    } else if (tower -> get_isBadChi2()) {
      status = Stat::BadChi;
    } else if (tower -> get_isNotInstr()) {
      status = Stat::NotInstr;
    } else if (tower -> get_isNoCalib()) {
      status = Stat::NoCalib;
    } else {
      status = Stat::Good;
    }
    return status;

  }  // end 'GetTowerStatus(TowerInfov2*)'

}  // end CaloStatusMapperDefs namespace

#endif

// end ------------------------------------------------------------------------


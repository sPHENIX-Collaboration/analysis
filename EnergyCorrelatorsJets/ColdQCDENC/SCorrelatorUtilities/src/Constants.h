/// ---------------------------------------------------------------------------
/*! \file   Constants.h
 *  \author Derek Anderson
 *  \date   11.16.2023
 *
 * Various constants used throughout the SColdQcdCorrelatorAnalysis
 * namespace are collected here.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_CONSTANTS_H
#define SCORRELATORUTILITIES_CONSTANTS_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// c++ utilities
#include <map>
#include <string>
#include <cstdint>
// fastjet libraries
#include <fastjet/JetDefinition.hh>
#include <fastjet/AreaDefinition.hh>
// sphenix jet base
#include <jetbase/Jet.h>
// sphenix calo base
#include <calobase/RawTowerDefs.h>

#pragma GCC diagnostic pop

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Const {

    // enums ==================================================================

    // ------------------------------------------------------------------------
    //! Intialization options for analysis types
    // ------------------------------------------------------------------------
    enum class Init {Minimize, Maximize};

    // ------------------------------------------------------------------------
    //! Jet types
    // ------------------------------------------------------------------------
    enum class JetType {Charged, Cluster, Tower, Full};

    // ------------------------------------------------------------------------
    //! Particle charge subsets
    // ------------------------------------------------------------------------
    enum class Subset {All, Charged, Neutral};

    // ------------------------------------------------------------------------
    //! Object types
    // ------------------------------------------------------------------------
    enum Object {Track, Tower, Cluster, Flow, Particle, Mystery};

    // ------------------------------------------------------------------------
    //! Subsystem indices
    // ------------------------------------------------------------------------
    enum Subsys {Mvtx, Intt, Tpc, EMCal, RECal, IHCal, OHCal};

    // ------------------------------------------------------------------------
    //! Calorimeter tower statuses
    // ------------------------------------------------------------------------
    enum TowerStatus {Good, Hot, BadTime, BadChi, NotInstr, NoCalib, Unknown, NA};

    // ------------------------------------------------------------------------
    //! Subevent options
    // ------------------------------------------------------------------------
    enum SubEvtOpt {Everything, OnlySignal, AllBkgd, PrimaryBkgd, Pileup, Specific};

    // ------------------------------------------------------------------------
    //! Special subevent indices
    // ------------------------------------------------------------------------
    enum SubEvt {
      Background     = 0,
      NotEmbedSignal = 1,
      EmbedSignal    = 2
    };

    // ------------------------------------------------------------------------
    //! Hard scatter product statuses
    // ------------------------------------------------------------------------
    enum HardScatterStatus {
      First  = 23,
      Second = 24
    };

    // ------------------------------------------------------------------------
    //! Parton pdg codes
    // ------------------------------------------------------------------------
    enum Parton {
      Down    = 1,
      Up      = 2,
      Strange = 3,
      Charm   = 4,
      Bottom  = 5,
      Top     = 6,
      Gluon   = 21
    };



    // constants ==============================================================

    // ------------------------------------------------------------------------
    //! No. of MVTX layers
    // ------------------------------------------------------------------------
    inline uint16_t const &NMvtxLayer() {
      static uint16_t nMvtxLayer = 3;
      return nMvtxLayer;
    }

    // ------------------------------------------------------------------------
    //! No. of INTT layers
    // ------------------------------------------------------------------------
    inline uint16_t const &NInttLayer() {
      static uint16_t nInttLayer = 8;
      return nInttLayer;
    }

    // ------------------------------------------------------------------------
    //! No. of TPC layers
    // ------------------------------------------------------------------------
    inline uint16_t const &NTpcLayer() {
      static uint16_t nTpcLayer = 48;
      return nTpcLayer;
    }

    // ------------------------------------------------------------------------
    //! No. of TPC sectors
    // ------------------------------------------------------------------------
    inline uint16_t const &NTpcSector() {
      static uint16_t nTpcSector = 12;
      return nTpcSector;
    }

    // ------------------------------------------------------------------------
    //! Fit function for calculating DCA nSigma
    // ------------------------------------------------------------------------
    inline string const &SigmaDcaFunc() {
      static string sigmaDcaFunc = "[0]+[1]/x+[2]/(x*x)";
      return sigmaDcaFunc;
    }

    // ------------------------------------------------------------------------
    //! Charged pion mass in GeV/c^2
    // ------------------------------------------------------------------------
    inline double const &MassPion() {
      static double mPion = 0.140;
      return mPion;
    }



    // maps ===================================================================

    // ------------------------------------------------------------------------
    //! Map of PID to charges
    // ------------------------------------------------------------------------
    inline map<int, float> &MapPidOntoCharge() {
      static map<int, float> mapPidOntoCharge = {
        {1, -1./3.},
        {2, 2./3.}, 
        {3, -1./3.},
        {4, 2./3.},
        {5, -1./3},
        {6, 2./3.},
        {11, -1.},
        {12, 0.},
        {13, -1.},
        {14, 0.},
        {15, -1.},
        {16, 0.},
        {22, 0.},
        {23, 0.},
        {24, 1.},
        {111, 0.},
        {130, 0.},
        {211, 1.},
        {221, 0.},
        {310, 0.},
        {321, 1.},
        {421, 0.},
        {431, 1.},
        {441, 1.},
        {2112, 1.},
        {2212, 1.},
        {3112, -1.},
        {3122, 0.},
        {3212, 0.},
        {3222, 1.},
        {3322, 0.},
        {700201, 0.},
        {700202, 2.},
        {700301, 0.},
        {700302, 3.}
      };
      return mapPidOntoCharge;
    }

    // ------------------------------------------------------------------------
    //! Map of forbidden strings onto good ones
    // ------------------------------------------------------------------------
    inline map<string, string> MapBadOntoGoodStrings() {
      static map<string, string> mapBadOntoGoodStrings = {
        {"/", "_"},
        {"(", "_"},
        {")", ""},
        {"+", "plus"},
        {"-", "minus"},
        {"*", "star"},
      };
      return mapBadOntoGoodStrings;
    }

    // ------------------------------------------------------------------------
    //! Map of strings onto fastjet algorithms
    // ------------------------------------------------------------------------
    inline map<string, fastjet::JetAlgorithm> MapStringOntoFJAlgo() {
      static map<string, fastjet::JetAlgorithm> mapStringOntoAlgo = {
        {"kt",            fastjet::JetAlgorithm::kt_algorithm},
        {"c/a",           fastjet::JetAlgorithm::cambridge_algorithm},
        {"antikt",        fastjet::JetAlgorithm::antikt_algorithm},
        {"genkt",         fastjet::JetAlgorithm::genkt_algorithm},
        {"c/a_passive",   fastjet::JetAlgorithm::cambridge_for_passive_algorithm},
        {"genkt_passive", fastjet::JetAlgorithm::genkt_for_passive_algorithm},
        {"ee_kt",         fastjet::JetAlgorithm::ee_kt_algorithm},
        {"ee_genkt",      fastjet::JetAlgorithm::ee_genkt_algorithm},
        {"plugin",        fastjet::JetAlgorithm::plugin_algorithm},
      };
      return mapStringOntoAlgo;
    }

    // ------------------------------------------------------------------------
    //! Map of strings onto fastjet recombination schemes
    // ------------------------------------------------------------------------
    inline map<string, fastjet::RecombinationScheme> MapStringOntoFJRecomb() {
      static map<string, fastjet::RecombinationScheme> mapStringOntoRecomb = {
        {"e",        fastjet::RecombinationScheme::E_scheme},
        {"pt",       fastjet::RecombinationScheme::pt_scheme},
        {"pt2",      fastjet::RecombinationScheme::pt2_scheme},
        {"et",       fastjet::RecombinationScheme::Et_scheme},
        {"et2",      fastjet::RecombinationScheme::Et2_scheme},
        {"bi_pt",    fastjet::RecombinationScheme::BIpt_scheme},
        {"bi_pt2",   fastjet::RecombinationScheme::BIpt2_scheme},
        {"wta_pt",   fastjet::RecombinationScheme::WTA_pt_scheme},
        {"wta_modp", fastjet::RecombinationScheme::WTA_modp_scheme},
        {"external", fastjet::RecombinationScheme::external_scheme}
      };
      return mapStringOntoRecomb;
    }

    // ------------------------------------------------------------------------
    //! Map of strings onto fastjet area types
    // ------------------------------------------------------------------------
    inline map<string, fastjet::AreaType> MapStringOntoFJArea() {
      static map<string, fastjet::AreaType> mapStringOntoArea = {
        {"active",            fastjet::AreaType::active_area},
        {"passive",           fastjet::AreaType::passive_area},
        {"voronoi",           fastjet::AreaType::voronoi_area},
        {"active_explicit",   fastjet::AreaType::active_area_explicit_ghosts},
        {"one_ghost_passive", fastjet::AreaType::one_ghost_passive_area}
      };
      return mapStringOntoArea;
    }

    // ------------------------------------------------------------------------
    //! Map of subsytem index onto calorimeter ID
    // ------------------------------------------------------------------------
    inline map<int, int> MapIndexOntoID() {
      static map<int, int> mapIndexOntoID = {
        {Subsys::EMCal, RawTowerDefs::CalorimeterId::CEMC},
        {Subsys::RECal, RawTowerDefs::CalorimeterId::HCALIN},
        {Subsys::IHCal, RawTowerDefs::CalorimeterId::HCALIN},
        {Subsys::OHCal, RawTowerDefs::CalorimeterId::HCALOUT}
      };
      return mapIndexOntoID;
    }

    // ----------------------------------------------------------------------
    //! Map of subsystem index onto raw tower node names
    // ----------------------------------------------------------------------
    inline map<int, string> MapIndexOntoRawTowers() {
      static map<int, string> mapIndexOntoRawTowers = {
        {Subsys::EMCal, "TOWER_CALIB_CEMC"},
        {Subsys::RECal, "TOWER_CALIB_CEMC_RETOWER"},
        {Subsys::IHCal, "TOWER_CALIB_HCALIN"},
        {Subsys::OHCal, "TOWER_CALIB_HCALOUT"}
      };
      return mapIndexOntoRawTowers;
    }

    // ----------------------------------------------------------------------
    //! Map of subsystem index onto tower info node names
    // ----------------------------------------------------------------------
    inline map<int, string> MapIndexOntoTowerInfo() {
      static map<int, string> mapIndexOntoTowerInfo = {
        {Subsys::EMCal, "TOWERINFO_CALIB_CEMC"},
        {Subsys::RECal, "TOWERINFO_CALIB_CEMC_RETOWER"},
        {Subsys::IHCal, "TOWERINFO_CALIB_HCALIN"},
        {Subsys::OHCal, "TOWERINFO_CALIB_HCALOUT"}
      };
      return mapIndexOntoTowerInfo;
    }

    // ------------------------------------------------------------------------
    //! Map of subsytem index onto cluster node names
    // ------------------------------------------------------------------------
    inline map<int, string> MapIndexOntoClusters() {
      static map<int, string> mapIndexOntoClusters = {
        {Subsys::EMCal, "CLUSTER_CEMC"},
        {Subsys::IHCal, "CLUSTER_HCALIN"},
        {Subsys::OHCal, "CLUSTER_HCALOUT"}
      };
      return mapIndexOntoClusters;
    }

    // ------------------------------------------------------------------------
    //! Map of subsystem index onto raw tower geometry containers
    // ------------------------------------------------------------------------
    inline map<int, string> MapIndexOntoTowerGeom() {
      static map<int, string> mapIndexOntoTowerGeom = {
        {Subsys::EMCal, "TOWERGEOM_CEMC"},
        {Subsys::RECal, "TOWERGEOM_HCALIN"},
        {Subsys::IHCal, "TOWERGEOM_HCALIN"},
        {Subsys::OHCal, "TOWERGEOM_HCALOUT"}
      };
      return mapIndexOntoTowerGeom;
    }

    // ------------------------------------------------------------------------
    //! Map of subsystem index onto jet source index
    // ------------------------------------------------------------------------
    // FIXME this might not be necessary...
    inline map<int, Jet::SRC> MapIndexOntoSrc() {
      static map<int, Jet::SRC> mapIndexOntoSrc = {
        {Subsys::EMCal, Jet::SRC::CEMC_CLUSTER},
        {Subsys::IHCal, Jet::SRC::HCALIN_CLUSTER},
        {Subsys::OHCal, Jet::SRC::HCALOUT_CLUSTER}
      };
      return mapIndexOntoSrc;
    }

    // ------------------------------------------------------------------------
    //! Map of jet source index onto input node
    // ------------------------------------------------------------------------
    inline map<Jet::SRC, string> MapSrcOntoNode() {
      static map<Jet::SRC, string> mapSrcOntoNode = {
        {Jet::SRC::CEMC_CLUSTER, "CLUSTER_CEMC"},
        {Jet::SRC::HCALIN_CLUSTER, "CLUSTER_HCALIN"},
        {Jet::SRC::HCALOUT_CLUSTER, "CLUSTER_HCALOUT"},
        {Jet::SRC::HCAL_TOPO_CLUSTER, "TOPOCLUSTER_HCAL"},
        {Jet::SRC::ECAL_TOPO_CLUSTER, "TOPOCLUSTER_EMCAL"},
        {Jet::SRC::ECAL_HCAL_TOPO_CLUSTER, "TOPOCLUSTER_ALLCALO"},
        {Jet::SRC::CEMC_TOWER_RETOWER, "TOWER_CALIB_CEMC_RETOWER"},
        {Jet::SRC::CEMC_TOWERINFO_RETOWER, "TOWERINFO_CALIB_CEMC_RETOWER"},
        {Jet::SRC::CEMC_TOWER, "TOWER_CALIB_CEMC"},
        {Jet::SRC::HCALIN_TOWER, "TOWER_CALIB_HCALIN"},
        {Jet::SRC::HCALOUT_TOWER, "TOWER_CALIB_HCALOUT"},
        {Jet::SRC::CEMC_TOWERINFO, "TOWERINFO_CALIB_CEMC"},
        {Jet::SRC::HCALIN_TOWERINFO, "TOWERINFO_CALIB_HCALIN"},
        {Jet::SRC::HCALOUT_TOWERINFO, "TOWERINFO_CALIB_HCALOUT"},
        {Jet::SRC::CEMC_TOWERINFO_EMBED, "TOWERINFO_CALIB_EMBED_CEMC"},
        {Jet::SRC::HCALIN_TOWERINFO_EMBED, "TOWERINFO_CALIB_EMBED_HCALIN"},
        {Jet::SRC::HCALOUT_TOWERINFO_EMBED, "TOWERINFO_CALIB_EMBED_HCALOUT"},
        {Jet::SRC::CEMC_TOWERINFO_SIM, "TOWERINFO_CALIB_SIM_CEMC"},
        {Jet::SRC::HCALIN_TOWERINFO_SIM, "TOWERINFO_CALIB_SIM_HCALIN"},
        {Jet::SRC::HCALOUT_TOWERINFO_SIM, "TOWERINFO_CALIB_SIM_HCALOUT"},
        {Jet::SRC::CEMC_TOWER_SUB1, "TOWER_CALIB_CEMC_RETOWER_SUB1"},
        {Jet::SRC::HCALIN_TOWER_SUB1, "TOWER_CALIB_HCALIN_SUB1"},
        {Jet::SRC::HCALOUT_TOWER_SUB1, "TOWER_CALIB_HCALOUT_SUB1"},
        {Jet::SRC::CEMC_TOWERINFO_SUB1, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1"},
        {Jet::SRC::HCALIN_TOWERINFO_SUB1, "TOWERINFO_CALIB_HCALIN_SUB1"},
        {Jet::SRC::HCALOUT_TOWERINFO_SUB1, "TOWERINFO_CALIB_HCALOUT_SUB1"},
        {Jet::SRC::CEMC_TOWER_SUB1CS, "TOWER_CALIB_CEMC_RETOWER_SUB1CS"},
        {Jet::SRC::HCALIN_TOWER_SUB1CS, "TOWER_CALIB_HCALIN_SUB1CS"},
        {Jet::SRC::HCALOUT_TOWER_SUB1CS, "TOWER_CALIB_HCALOUT_SUB1CS"},
        {Jet::SRC::TRACK, "SvtxTrackMap"},
        {Jet::SRC::PARTICLE, "G4TruthInfo"}
      };
      return mapSrcOntoNode;
    }

  }  // end Const namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

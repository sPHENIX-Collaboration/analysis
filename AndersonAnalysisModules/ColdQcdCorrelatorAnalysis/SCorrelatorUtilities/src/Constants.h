// ----------------------------------------------------------------------------
// 'Constants.h'
// Derek Anderson
// 11.16.2023
//
// Various constants used throughout the SColdQcdCorrelatorAnalysis
// namespace are collected here.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_CONSTANTS_H
#define SCORRELATORUTILITIES_CONSTANTS_H

// c++ utilities
#include <map>
#include <string>
#include <cstdint>
// fastjet libraries
#include <fastjet/JetDefinition.hh>
#include <fastjet/AreaDefinition.hh>
// sphenix jet base
#include <jetbase/Jet.h>

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Const {

    // enums ------------------------------------------------------------------

    // info intialization options
    enum class Init {Minimize, Maximize};

    // jet types
    enum class JetType {Charged, Neutral, Full};

    // particle charge subsets
    enum class Subset {All, Charged, Neutral};

    // object types
    enum Object {Track, Cluster, Flow, Particle, Unknown};

    // subsystem indices
    enum Subsys {Mvtx, Intt, Tpc, EMCal, IHCal, OHCal};

    // subevent options
    enum SubEvtOpt {Everything, OnlySignal, AllBkgd, PrimaryBkgd, Pileup, Specific};

    // special subevent indices
    enum SubEvt {
      Background     = 0,
      NotEmbedSignal = 1,
      EmbedSignal    = 2
    };

    // hard scatter product statuses
    enum HardScatterStatus {
      First  = 23,
      Second = 24
    };

    // parton pdg codes
    enum Parton {
      Down    = 1,
      Up      = 2,
      Strange = 3,
      Charm   = 4,
      Bottom  = 5,
      Top     = 6,
      Gluon   = 21
    };



    // constants --------------------------------------------------------------

    // tracking-related constants
    inline uint16_t const &NMvtxLayer() {
      static uint16_t nMvtxLayer = 3;
      return nMvtxLayer;
    }

    inline uint16_t const &NInttLayer() {
      static uint16_t nInttLayer = 8;
      return nInttLayer;
    }

    inline uint16_t const &NTpcLayer() {
      static uint16_t nTpcLayer = 48;
      return nTpcLayer;
    }

    inline uint16_t const &NTpcSector() {
      static uint16_t nTpcSector = 12;
      return nTpcSector;
    }

    inline string const &SigmaDcaFunc() {
      static string sigmaDcaFunc = "[0]+[1]/x+[2]/(x*x)";
      return sigmaDcaFunc;
    }

    // charged pion mass in GeV/c^2
    inline double const &MassPion() {
      static double mPion = 0.140;
      return mPion;
    }



    // maps -------------------------------------------------------------------

    // map of PID onto charges
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

    // map of node name onto subsystem index
    inline map<string, int> MapNodeOntoIndex() {
      static map<string, int> mapNodeOntoIndex = {
        {"CLUSTER_CEMC",    Subsys::EMCal},
        {"CLUSTER_HCALIN",  Subsys::IHCal},
        {"CLUSTER_HCALOUT", Subsys::OHCal}
      };
      return mapNodeOntoIndex;
    }

    // map of subsytem index onto node names
    inline map<int, string> MapIndexOntoNode() {
      static map<int, string> mapIndexOntoNode = {
        {Subsys::EMCal, "CLUSTER_CEMC"},
        {Subsys::IHCal, "CLUSTER_HCALIN"},
        {Subsys::OHCal, "CLUSTER_HCALOUT"}
      };
      return mapIndexOntoNode;
    }

    // map of subsystem index onto jet source index
    inline map<int, Jet::SRC> MapIndexOntoSrc() {
      static map<int, Jet::SRC> mapIndexOntoSrc = {
        {Subsys::EMCal, Jet::SRC::CEMC_CLUSTER},
        {Subsys::IHCal, Jet::SRC::HCALIN_CLUSTER},
        {Subsys::OHCal, Jet::SRC::HCALOUT_CLUSTER} 
      };
      return mapIndexOntoSrc;
    }

    // map of forbidden strings onto good ones
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

    // map of strings onto fastjet algorithms
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
        {"plugin",        fastjet::JetAlgorithm::plugin_algorithm}
      };
      return mapStringOntoAlgo;
    }

    // map of strings onto fastjet recombination schemes
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

    // map of strings onto fastjet area types
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

  }  // end Const namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

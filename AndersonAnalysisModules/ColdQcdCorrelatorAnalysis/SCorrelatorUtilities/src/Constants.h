// ----------------------------------------------------------------------------
// 'Constants.h'
// Derek Anderson
// 11.16.2023
//
// Various constants used throughout the SColdQcdCorrelatorAnalysis
// namespace are collected here.
// ----------------------------------------------------------------------------

#pragma once

// c++ utilities
#include <map>

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // constants, enums, etc. ---------------------------------------------------

  namespace {

    // object types
    enum Object {Track, Cluster, Flow, Particle, Unknown};

    // subsystem indices
    enum Subsys {Mvtx, Intt, Tpc, EMCal, IHCal, OHCal};

    // particle charge subsets
    enum Subset {All, Charged, Neutral};

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

    // tracking-related constants
    const uint16_t NMvtxLayer = 3;
    const uint16_t NInttLayer = 8;
    const uint16_t NTpcLayer  = 48;
    const uint16_t NTpcSector = 12;

    // charged pion mass in GeV/c^2
    const double MassPion = 0.140;

    // map of PID onto charges
    map<int, float> MapPidOntoCharge = {
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

    // map of node name onto subsystem index
    map<string, int> MapNodeOntoIndex = {
      {"CLUSTER_CEMC", Subsys::EMCal},
      {"CLUSTER_HCALIN", Subsys::IHCal},
      {"CLUSTER_HCALOUT", Subsys::OHCal}
    };

    // map of forbidden strings onto good ones
    map<string, string> MapBadOntoGoodStrings = {
      {"/", "_"},
      {"(", "_"},
      {")", ""},
      {"+", "plus"},
      {"-", "minus"},
      {"*", "star"},
    };

  }  // end constants

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

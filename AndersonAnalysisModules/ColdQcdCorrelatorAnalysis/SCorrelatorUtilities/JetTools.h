// ----------------------------------------------------------------------------
// 'JetTools.h'
// Derek Anderson
// 10.22.2023
//
// Definition of JetInfo class utilized in the sPHENIX
// Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// c++ utilities
#include <vector>

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // JetInfo definition -----------------------------------------------------

    struct JetInfo {

      // data members
      uint32_t m_jetID   = 999;
      uint64_t m_numCsts = 999;
      double   m_ene     = -999.;
      double   m_pt      = -999.;
      double   m_eta     = -999.;
      double   m_phi     = -999.;
      double   m_area    = -999.;

      void SetInfo(uint32_t id, uint64_t nCst, double ene, double pt, double h, double f, double a) {
        m_jetID   = id;
        m_numCsts = nCst;
        m_ene     = ene;
        m_pt      = pt;
        m_eta     = h;
        m_phi     = f;
        m_area    = a;
        return;
      }  // end 'SetInfo(uint32_t, uint64_t, double, double, double, double, double)'

      void Reset() {
        m_jetID   = 999;
        m_numCsts = 999;
        m_ene     = -999.;
        m_pt      = -999.;
        m_eta     = -999.;
        m_phi     = -999.;
        m_area    = -999.;
        return;
      }  // end 'Reset()'

    };  // end JetInfo def



    // jet methods ------------------------------------------------------------

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 'SCorrelatorUtilities.CstInfo.h'
// Derek Anderson
// 10.22.2023
//
// Definition of CstInfo class utilized in the sPHENIX
// Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    struct CstInfo {

      // data members
      int    m_cstID = -1;
      double m_z     = -999.;
      double m_dr    = -999.;
      double m_ene   = -999.;
      double m_jt    = -999.;
      double m_eta   = -999.;
      double m_phi   = -999.;

      void SetInfo(int id, int z, double dr, double ene, double jt, double eta, double phi) {
        m_cstID = id;
        m_z     = z;
        m_dr    = dr;
        m_ene   = ene;
        m_jt    = jt;
        m_eta   = eta;
        m_phi   = phi;
        return;
      }  // end 'SetInfo(int, int, double, double, double, double, double)'

      void Rest() {
        m_cstID = -1;
        m_z     = -999.;
        m_dr    = -999.;
        m_ene   = -999.;
        m_jt    = -999.;
        m_eta   = -999.;
        m_phi   = -999.;
        return;
      }  // end 'Reset()'

    };  // end CstInfo def

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

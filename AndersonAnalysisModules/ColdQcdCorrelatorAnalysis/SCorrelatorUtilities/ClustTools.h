// ----------------------------------------------------------------------------
// 'SCorrelatorUtilities.ClustTools.h'
// Derek Anderson
// 10.30.2023
//
// Collection of frequent calo. cluster-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // ClustInfo definition ---------------------------------------------------

    /* TODO
     *   - add overloaded <, > operators for applying cuts
     *   - add SetInfo(RawClusterContainer::ConstIterator*) method
     *   - add TypeDef for Cluster = RawClusterContainer::ConstIterator
     *   - add enums for calo to namespace
     */
    struct ClustInfo {

      // data members
      int    m_sys = -1;
      double m_pt  = -999.;
      double m_eta = -999.;
      double m_phi = -999.;
      double m_ene = -999.;

      void SetInfo(const int sys, const double pt, const double eta, const double phi, const double ene) {
        m_sys = -1;
        m_pt  = pt;
        m_eta = eta;
        m_phi = phi;
        m_ene = ene;
        return;
      }  // end 'SetInfo(int, double, double, double, double)'

      void Reset() {
        m_sys = -1;
        m_pt  = -999.;
        m_eta = -999.;
        m_phi = -999.;
        m_ene = -999.;
        return;
      }  // end 'Reset()'

    };  // end ClustInfo def



    // cluster methods --------------------------------------------------------

    RawClusterContainer* GetClusterStore(PHCompositeNode* topNode, const TString sNodeName) {

      // grab clusters
      RawClusterContainer *clustStore = findNode::getClass<RawClusterContainer>(topNode, sNodeName.Data());
      if (!clustStore) {
        cout << PHWHERE
             << "PANIC: " << sNodeName.Data() << " node is missing!"
             << endl;
        assert(clustStore);
      }
      return clustStore;

    }  // end 'GetClusterStore(PHCompositeNode*, TString)'


  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------


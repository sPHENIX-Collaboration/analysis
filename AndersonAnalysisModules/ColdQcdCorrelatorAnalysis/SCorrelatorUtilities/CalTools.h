// ----------------------------------------------------------------------------
// 'CalTools.h'
// Derek Anderson
// 10.30.2023
//
// Collection of frequent calo. cluster-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// c++ utilities
#include <string>
#include <vector>
#include <cassert>
#include <optional>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// CaloBase libraries
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>
// analysis utilities
#include "Constants.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // ClustInfo definition ---------------------------------------------------

    struct ClustInfo {

      // data members
      int    system = -1;
      size_t nTwr   = -1;
      double ene    = -999.;
      double rho    = -999.;
      double eta    = -999.;
      double phi    = -999.;
      double rx     = -999.;
      double ry     = -999.;
      double rz     = -999.;

      void SetInfo(const RawCluster* clust, optional<int> sys) {
        if (sys.has_value()) {
          system = sys.value();
        }
        nTwr = clust -> getNTowers();
        ene  = clust -> get_energy();
        rho  = clust -> get_r();
        eta  = -999.;  // FIXME add method to calculate eta
        phi  = clust -> get_phi();
        rx   = clust -> get_position().x();
        ry   = clust -> get_position().y();
        rz   = clust -> get_position().z();
        return;
      };

      void Reset() {
        system  = -1;
        nTwr    = -1;
        ene     = -999.;
        rho     = -999.;
        eta     = -999.;
        phi     = -999.;
        rx      = -999.;
        ry      = -999.;
        rz      = -999.;
        return;
      }  // end 'Reset()'

      static vector<string> GetListOfMembers() {
        vector<string> members = {
          "sys",
          "nTwr",
          "ene",
          "rho",
          "eta",
          "phi",
          "rx",
          "ry",
          "rz"
        };
        return members;
      }  // end 'GetListOfMembers()'

      // overloaded < operator
      friend bool operator<(const ClustInfo& lhs, const ClustInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isLessThan = (
          (lhs.nTwr < rhs.nTwr) &&
          (lhs.ene  < rhs.ene)  &&
          (lhs.rho  < rhs.rho)  &&
          (lhs.eta  < rhs.eta)  &&
          (lhs.phi  < rhs.phi)  &&
          (lhs.rx   < rhs.rx)   &&
          (lhs.ry   < rhs.ry)   &&
          (lhs.rz   < rhs.rz)
        );
        return isLessThan;

      }  // end 'operator<(ClustInfo&, ClustInfo&)'

      // overloaded > operator
      friend bool operator>(const ClustInfo& lhs, const ClustInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isGreaterThan = (
          (lhs.nTwr > rhs.nTwr) &&
          (lhs.ene  > rhs.ene)  &&
          (lhs.rho  > rhs.rho)  &&
          (lhs.eta  > rhs.eta)  &&
          (lhs.phi  > rhs.phi)  &&
          (lhs.rx   > rhs.rx)   &&
          (lhs.ry   > rhs.ry)   &&
          (lhs.rz   > rhs.rz)
        );
        return isGreaterThan;

      }  // end 'operator>(ClustInfo&, ClustInfo&)'

      // overloaded, <=, >= operators
      inline friend bool operator<=(const ClustInfo& lhs, const ClustInfo& rhs) {return !(lhs > rhs);}
      inline friend bool operator>=(const ClustInfo& lhs, const ClustInfo& rhs) {return !(lhs < rhs);}

      // default ctor/dtor
      ClustInfo()  {};
      ~ClustInfo() {};

      // ctor accepting RawClusters
      ClustInfo(RawCluster* clust, optional<int> sys) {
        SetInfo(clust, sys);
      }

    };  // end ClustInfo def



    // cluster methods --------------------------------------------------------

    RawClusterContainer* GetClusterStore(PHCompositeNode* topNode, const string node) {

      // grab clusters
      RawClusterContainer* clustStore = getClass<RawClusterContainer>(topNode, node.data());
      if (!clustStore) {
        cout << PHWHERE
             << "PANIC: " << node << " node is missing!"
             << endl;
        assert(clustStore);
      }
      return clustStore;

    }  // end 'GetClusterStore(PHCompositeNode*, string)'



    RawClusterContainer::ConstRange GetClusters(PHCompositeNode* topNode, const string store) {

      // get store and return range of clusters
      RawClusterContainer* clustStore = GetClusterStore(topNode, store);
      return clustStore -> getClusters();

    }  // end 'GetClusters(PHCompositeNode*, string)'



    bool IsInClusterAcceptance(const ClustInfo& cluster, const ClustInfo& minimum, const ClustInfo& maximum) {

      return ((cluster >= minimum) && (cluster <= maximum));

    }  // end 'IsInClusterAcceptance(ClustInfo&, ClustInfo&, ClustInfo&)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------


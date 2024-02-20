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
#include <limits>
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
      int    system = numeric_limits<int>::max();
      size_t nTwr   = numeric_limits<size_t>::max();
      double ene    = numeric_limits<double>::max();
      double rho    = numeric_limits<double>::max();
      double eta    = numeric_limits<double>::max();
      double phi    = numeric_limits<double>::max();
      double rx     = numeric_limits<double>::max();
      double ry     = numeric_limits<double>::max();
      double rz     = numeric_limits<double>::max();

      void SetInfo(const RawCluster* clust, optional<int> sys = nullopt) {
        if (sys.has_value()) {
          system = sys.value();
        }
        nTwr = clust -> getNTowers();
        ene  = clust -> get_energy();
        rho  = clust -> get_r();
        eta  = numeric_limits<double>::max();  // FIXME add method to calculate eta
        phi  = clust -> get_phi();
        rx   = clust -> get_position().x();
        ry   = clust -> get_position().y();
        rz   = clust -> get_position().z();
        return;
      };

      void Reset() {
        system  = numeric_limits<int>::max();
        nTwr    = numeric_limits<int>::max();
        ene     = numeric_limits<double>::max();
        rho     = numeric_limits<double>::max();
        eta     = numeric_limits<double>::max();
        phi     = numeric_limits<double>::max();
        rx      = numeric_limits<double>::max();
        ry      = numeric_limits<double>::max();
        rz      = numeric_limits<double>::max();
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
      ClustInfo(const RawCluster* clust, optional<int> sys = nullopt) {
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



    bool IsInAcceptance(const ClustInfo& cluster, const ClustInfo& minimum, const ClustInfo& maximum) {

      return ((cluster >= minimum) && (cluster <= maximum));

    }  // end 'IsInAcceptance(ClustInfo&, ClustInfo&, ClustInfo&)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------


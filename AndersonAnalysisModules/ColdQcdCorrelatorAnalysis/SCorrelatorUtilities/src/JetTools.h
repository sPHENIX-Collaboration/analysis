// ----------------------------------------------------------------------------
// 'JetTools.h'
// Derek Anderson
// 10.22.2023
//
// Collection of frequent jet-related methods and types
// utilized in the sPHENIX cold QCD Energy-Energy
// Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// c++ utilities
#include <limits>
#include <vector>
// analysis utilities
#include "CstTools.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // JetInfo definition -----------------------------------------------------

    struct JetInfo {

      // data members
      uint32_t jetID = numeric_limits<uint32_t>::max();
      uint64_t nCsts = numeric_limits<uint64_t>::max();
      double   ene   = numeric_limits<double>::max();
      double   pt    = numeric_limits<double>::max();
      double   eta   = numeric_limits<double>::max();
      double   phi   = numeric_limits<double>::max();
      double   area  = numeric_limits<double>::max();

      void SetInfo(uint32_t id, uint64_t nc, double e, double p, double h, double f, double a) {
        jetID = id;
        nCsts = nc;
        ene   = e;
        pt    = p;
        eta   = h;
        phi   = f;
        area  = a;
        return;
      }  // end 'SetInfo(uint32_t, uint64_t, double, double, double, double, double)'

      void Reset() {
        jetID = numeric_limits<uint32_t>::max();
        nCsts = numeric_limits<uint64_t>::max();
        ene   = numeric_limits<double>::max();
        pt    = numeric_limits<double>::max();
        eta   = numeric_limits<double>::max();
        phi   = numeric_limits<double>::max();
        area  = numeric_limits<double>::max();
        return;
      }  // end 'Reset()'

      static vector<string> GetListOfMembers() {
        vector<string> members = {
          "jetID",
          "nCsts",
          "ene",
          "pt",
          "eta",
          "phi",
          "area"
        };
        return members;
      }  // end 'GetListOfMembers()'

      // overloaded < operator
      friend bool operator<(const JetInfo& lhs, const JetInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isLessThan = (
          (lhs.nCsts < rhs.nCsts) &&
          (lhs.ene   < rhs.ene)   &&
          (lhs.pt    < rhs.pt)    &&
          (lhs.eta   < rhs.eta)   &&
          (lhs.phi   < rhs.phi)   &&
          (lhs.area  < rhs.area)
        );
        return isLessThan;
      
      }  // end 'operator<(JetInfo&, JetInfo&)'

      // overloaded > operator
      friend bool operator>(const JetInfo& lhs, const JetInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isGreaterThan = (
          (lhs.nCsts > rhs.nCsts) &&
          (lhs.ene   > rhs.ene)   &&
          (lhs.pt    > rhs.pt)    &&
          (lhs.eta   > rhs.eta)   &&
          (lhs.phi   > rhs.phi)   &&
          (lhs.area  > rhs.area)
        );
        return isGreaterThan;
      
      }  // end 'operator<(JetInfo&, JetInfo&)'

      // overloaded <=, >= operators
      inline friend bool operator<=(const JetInfo& lhs, const JetInfo& rhs) {return !(lhs > rhs);}
      inline friend bool operator>=(const JetInfo& lhs, const JetInfo& rhs) {return !(lhs < rhs);}

      // default ctor/dtor
      JetInfo()  {};
      ~JetInfo() {};

      // ctor accepting arguments
      JetInfo(uint32_t id, uint64_t nc, double e, double p, double h, double f, double a) {
        SetInfo(id, nc, e, p, h, f, a);
      }

    };  // end JetInfo def



    // jet methods ------------------------------------------------------------

    bool IsInAcceptance(const JetInfo& jet, const JetInfo& minimum, const JetInfo& maximum) {

      return ((jet >= minimum) && (jet <= maximum));

    }  // end 'IsInAcceptance(JetInfo&, JetInfo&, JetInfo&)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

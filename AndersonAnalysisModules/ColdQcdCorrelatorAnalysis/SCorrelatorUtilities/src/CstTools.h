// ----------------------------------------------------------------------------
// 'CstTools.h'
// Derek Anderson
// 10.22.2023
//
// Collection of frequent constituent-related methods and
// types utilized in the sPHENIX cold QCD Energy-Energy
// Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// c++ utilities
#include <limits>
#include <string>
#include <vector>
#include <optional>
// analysis utilities
#include "Constants.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    struct CstInfo {

      // data members
      int    type    = numeric_limits<int>::max();
      int    cstID   = numeric_limits<int>::max();
      int    embedID = numeric_limits<int>::max();
      int    pid     = numeric_limits<int>::max();
      double z       = numeric_limits<double>::max();
      double dr      = numeric_limits<double>::max();
      double ene     = numeric_limits<double>::max();
      double pt      = numeric_limits<double>::max();
      double eta     = numeric_limits<double>::max();
      double phi     = numeric_limits<double>::max();

      void SetInfo(int t, int id, double z, double d, double e, double p, double h, double f, optional<int> eid = nullopt, optional<int> pdg = nullopt) {
        type  = t;
        cstID = id;
        z     = z;
        dr    = d;
        ene   = e;
        pt    = p;
        eta   = h;
        phi   = f;
        if (eid.has_value()) {
          embedID = eid.value();
        }
        if (pdg.has_value()) {
          pid = pdg.value();
        }
        return;
      }  // end 'SetInfo(int x 2, double x 6, optional<int> x 2)'

      void Rest() {
        type    = numeric_limits<int>::max();
        cstID   = numeric_limits<int>::max();
        embedID = numeric_limits<int>::max();
        pid     = numeric_limits<int>::max();
        z       = numeric_limits<double>::max();
        dr      = numeric_limits<double>::max();
        ene     = numeric_limits<double>::max();
        pt      = numeric_limits<double>::max();
        eta     = numeric_limits<double>::max();
        phi     = numeric_limits<double>::max();
        return;
      }  // end 'Reset()'

      static vector<string> GetListOfMembers() {
        vector<string> members = {
          "type",
          "cstID",
          "embedID",
          "pid",
          "z",
          "dr",
          "ene",
          "pt",
          "eta",
          "phi"
        };
        return members;
      }  // end 'GetListOfMembers()'

      // overloaded < operator
      friend bool operator<(const CstInfo& lhs, const CstInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isLessThan = (
          (lhs.z   < rhs.z)   &&
          (lhs.dr  < rhs.dr)  &&
          (lhs.ene < rhs.ene) &&
          (lhs.pt  < rhs.pt)  &&
          (lhs.eta < rhs.eta) &&
          (lhs.phi < rhs.phi)
        );
        return isLessThan;
      
      }  // end 'operator<(CstInfo&, CstInfo&)'

      // overloaded > operator
      friend bool operator>(const CstInfo& lhs, const CstInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isGreaterThan = (
          (lhs.z   > rhs.z)   &&
          (lhs.dr  > rhs.dr)  &&
          (lhs.ene > rhs.ene) &&
          (lhs.pt  > rhs.pt)  &&
          (lhs.eta > rhs.eta) &&
          (lhs.phi > rhs.phi)
        );
        return isGreaterThan;
      
      }  // end 'operator<(CstInfo&, CstInfo&)'

      // overloaded <=, >= operators
      inline friend bool operator<=(const CstInfo& lhs, const CstInfo& rhs) {return !(lhs > rhs);}
      inline friend bool operator>=(const CstInfo& lhs, const CstInfo& rhs) {return !(lhs < rhs);}

      // default ctor/dtor
      CstInfo()  {};
      ~CstInfo() {};

      // ctor accepting arguments
      CstInfo(int t, int id, double z, double d, double e, double p, double h, double f, optional<int> eid = nullopt, optional<int> pdg = nullopt) {
        SetInfo(t, id, z, d, e, p, h, f, eid, pdg);
      }

    };  // end CstInfo def



    // constituent methods ----------------------------------------------------

    bool IsInAcceptance(const CstInfo& cst, const CstInfo& minimum, const CstInfo& maximum) {

      return ((cst >= minimum) && (cst <= maximum));

    }  // end 'IsInAcceptance(CstInfo&, CstInfo&, CstInfo&)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

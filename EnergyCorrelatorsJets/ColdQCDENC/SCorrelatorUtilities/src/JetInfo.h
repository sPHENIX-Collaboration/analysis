/// ---------------------------------------------------------------------------
/*! \file   JetInfo.h
 *  \author Derek Anderson
 *  \date   03.04.2024
 *
 *  Utility class to hold information from jets.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_JETINFO_H
#define SCORRELATORUTILITIES_JETINFO_H

// c++ utilities
#include <limits>
#include <string>
#include <vector>
#include <utility>
// root libraries
#include <Rtypes.h>
// fastjet libraries
#include <fastjet/PseudoJet.hh>
// fun4all types
#include <jetbase/Jet.h>
// analysis utilities
#include "Constants.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Types {

    // ------------------------------------------------------------------------
    //! Jet info
    // ------------------------------------------------------------------------
    /*! A class to consolidate information
     *  about jets. Can be built from either
     *  fastjet pseudojets or F4A Jet objects.
     */
    class JetInfo {

      private:

        // data members
        uint32_t jetID = numeric_limits<uint32_t>::max();
        uint64_t nCsts = numeric_limits<uint64_t>::max();
        double   ene   = numeric_limits<double>::max();
        double   px    = numeric_limits<double>::max();
        double   py    = numeric_limits<double>::max();
        double   pz    = numeric_limits<double>::max();
        double   pt    = numeric_limits<double>::max();
        double   eta   = numeric_limits<double>::max();
        double   phi   = numeric_limits<double>::max();
        double   area  = numeric_limits<double>::max();

        // private methods
        void Minimize();
        void Maximize();

      public:

        // getters
        uint32_t GetJetID() const {return jetID;}
        uint64_t GetNCsts() const {return nCsts;}
        double   GetEne()   const {return ene;}
        double   GetPX()    const {return px;}
        double   GetPY()    const {return py;}
        double   GetPZ()    const {return pz;}
        double   GetPT()    const {return pt;}
        double   GetEta()   const {return eta;}
        double   GetPhi()   const {return phi;}
        double   GetArea()  const {return area;}

        // setters
        void SetJetID(uint32_t arg_jetID) {jetID = arg_jetID;}
        void SetNCsts(uint64_t arg_nCsts) {nCsts = arg_nCsts;}
        void SetEne(double arg_ene)       {ene   = arg_ene;}
        void SetPX(double arg_px)         {px    = arg_px;}
        void SetPY(double arg_py)         {py    = arg_py;}
        void SetPZ(double arg_pz)         {pz    = arg_pz;}
        void SetPT(double arg_pt)         {pt    = arg_pt;}
        void SetEta(double arg_eta)       {eta   = arg_eta;}
        void SetPhi(double arg_phi)       {phi   = arg_phi;}
        void SetArea(double arg_area)     {area  = arg_area;}

        // public methods
        void Reset();
        void SetInfo(fastjet::PseudoJet& pseudojet);
        void SetInfo(Jet& jet);
        bool IsInAcceptance(const JetInfo& minimum, const JetInfo& maximum) const;
        bool IsInAcceptance(const pair<JetInfo, JetInfo>& range) const;

        // static methods
        static vector<string> GetListOfMembers();

        // overloaded operators
        friend bool operator <(const JetInfo& lhs, const JetInfo& rhs);
        friend bool operator >(const JetInfo& lhs, const JetInfo& rhs);
        friend bool operator <=(const JetInfo& lhs, const JetInfo& rhs);
        friend bool operator >=(const JetInfo& lhs, const JetInfo& rhs);

        // default ctor/dtor
        JetInfo();
        ~JetInfo();

        // ctors accepting arguments
        JetInfo(const Const::Init init);
        JetInfo(fastjet::PseudoJet& pseudojet);
        JetInfo(Jet& jet);

      // identify this class to ROOT
      ClassDefNV(JetInfo, 1)

    };  // end JetInfo def



    // comparison operator definitions ----------------------------------------

    bool operator <(const JetInfo& lhs, const JetInfo& rhs);
    bool operator >(const JetInfo& lhs, const JetInfo& rhs);
    bool operator <=(const JetInfo& lhs, const JetInfo& rhs);
    bool operator >=(const JetInfo& lhs, const JetInfo& rhs);

  }  // end Types namespace
}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

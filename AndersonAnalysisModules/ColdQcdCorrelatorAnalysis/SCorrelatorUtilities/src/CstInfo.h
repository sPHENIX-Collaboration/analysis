// ----------------------------------------------------------------------------
// 'CstInfo.h'
// Derek Anderson
// 03.03.2024
//
// Utility class to hold information from
// jet constituents.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_CSTINFO_H
#define SCORRELATORUTILITIES_CSTINFO_H

// c++ utilities
#include <cmath>
#include <limits>
#include <string>
#include <vector>
#include <utility>
#include <optional>
// root libraries
#include <Rtypes.h>
#include <Math/Vector3D.h>
// fastjet libraries
#include <fastjet/PseudoJet.hh>
// analysis utilities
#include "JetInfo.h"
#include "Constants.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Types {

    // CstInfo definition -----------------------------------------------------

    class CstInfo {

      private:

        // data members
        int    type    = numeric_limits<int>::max();
        int    cstID   = numeric_limits<int>::max();
        int    jetID   = numeric_limits<int>::max();
        int    embedID = numeric_limits<int>::max();
        int    pid     = numeric_limits<int>::max();
        double z       = numeric_limits<double>::max();
        double dr      = numeric_limits<double>::max();
        double jt      = numeric_limits<double>::max();
        double ene     = numeric_limits<double>::max();
        double px      = numeric_limits<double>::max();
        double py      = numeric_limits<double>::max();
        double pz      = numeric_limits<double>::max();
        double pt      = numeric_limits<double>::max();
        double eta     = numeric_limits<double>::max();
        double phi     = numeric_limits<double>::max();

        // internal methods
        void Minimize();
        void Maximize();

      public:

        // getters
        int    GetType()    const {return type;}
        int    GetCstID()   const {return cstID;}
        int    GetJetID()   const {return jetID;}
        int    GetEmbedID() const {return embedID;}
        int    GetPID()     const {return pid;}
        double GetZ()       const {return z;}
        double GetDR()      const {return dr;}
        double GetJT()      const {return jt;}
        double GetEne()     const {return ene;}
        double GetPX()      const {return px;}
        double GetPY()      const {return py;}
        double GetPZ()      const {return pz;}
        double GetPT()      const {return pt;}
        double GetEta()     const {return eta;}
        double GetPhi()     const {return phi;}

        // setters
        void SetType(const int arg_type)       {type    = arg_type;}
        void SetCstID(const int arg_cstID)     {cstID   = arg_cstID;}
        void SetJetID(const int arg_jetID)     {jetID   = arg_jetID;}
        void SetEmbedID(const int arg_embedID) {embedID = arg_embedID;}
        void SetPID(const int arg_pid)         {pid     = arg_pid;}
        void SetZ(const double arg_z)          {z       = arg_z;}
        void SetDR(const double arg_dr)        {dr      = arg_dr;}
        void SetJT(const double arg_jt)        {jt      = arg_jt;}
        void SetEne(const double arg_ene)      {ene     = arg_ene;}
        void SetPX(const double arg_px)        {px      = arg_px;}
        void SetPY(const double arg_py)        {py      = arg_py;}
        void SetPZ(const double arg_pz)        {pz      = arg_pz;}
        void SetPT(const double arg_pt)        {pt      = arg_pt;}
        void SetEta(const double arg_eta)      {eta     = arg_eta;}
        void SetPhi(const double arg_phi)      {phi     = arg_phi;}

        // public methods
        void Reset();
        void SetInfo(fastjet::PseudoJet& pseudojet);
        void SetJetInfo(const Types::JetInfo& jet);
        bool IsInAcceptance(const CstInfo& minimum, const CstInfo& maximum) const;
        bool IsInAcceptance(const pair<CstInfo, CstInfo>& range) const;

        // static methods
        static vector<string> GetListOfMembers();

        // overloaded operators
        friend bool operator <(const CstInfo& lhs, const CstInfo& rhs);
        friend bool operator >(const CstInfo& lhs, const CstInfo& rhs);
        friend bool operator <=(const CstInfo& lhs, const CstInfo& rhs);
        friend bool operator >=(const CstInfo& lhs, const CstInfo& rhs);

        // default ctor/dtor
        CstInfo();
        ~CstInfo();

        // ctors accepting arguments
        CstInfo(const Const::Init init);
        CstInfo(fastjet::PseudoJet& pseudojet);

      // identify this class to ROOT
      ClassDefNV(CstInfo, 1)

    };  // end CstInfo def



    // comparison operator definitions ----------------------------------------

    bool operator <(const CstInfo& lhs, const CstInfo& rhs);
    bool operator >(const CstInfo& lhs, const CstInfo& rhs);
    bool operator <=(const CstInfo& lhs, const CstInfo& rhs);
    bool operator >=(const CstInfo& lhs, const CstInfo& rhs);

  }  // end Types namespace
}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

/// ---------------------------------------------------------------------------
/*! \file   ClustInfo.h
 *  \author Derek Anderson
 *  \date   03.01.2024
 *
 *  Utility class to hold information from
 *  calorimeter clusters.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_CLUSTINFO_H
#define SCORRELATORUTILITIES_CLUSTINFO_H

// c++ utilities
#include <limits>
#include <string>
#include <vector>
#include <utility>
#include <optional>
// root libraries
#include <Rtypes.h>
#include <Math/Vector3D.h>
#include <Math/Vector4D.h>
// CaloBase libraries
#include <calobase/RawCluster.h>
// analysis utilities
#include "Constants.h"
#include "ClustTools.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Types {

    // ------------------------------------------------------------------------
    //! Cluster info
    // ------------------------------------------------------------------------
    /*! A class to consolidate information
     *  about calorimeter clusters.
     */
    class ClustInfo {

      private:

        // data members
        int     system = numeric_limits<int>::max();
        int     id     = numeric_limits<int>::max();
        int64_t nTwr   = numeric_limits<int64_t>::max();
        double  ene    = numeric_limits<double>::max();
        double  rho    = numeric_limits<double>::max();
        double  eta    = numeric_limits<double>::max();
        double  phi    = numeric_limits<double>::max();
        double  px     = numeric_limits<double>::max();
        double  py     = numeric_limits<double>::max();
        double  pz     = numeric_limits<double>::max();
        double  rx     = numeric_limits<double>::max();
        double  ry     = numeric_limits<double>::max();
        double  rz     = numeric_limits<double>::max();

        // private methods
        void Minimize();
        void Maximize();

      public:

        // getters
        int     GetSystem() const {return system;}
        int     GetID()     const {return id;}
        int64_t GetNTwr()   const {return nTwr;}
        double  GetEne()    const {return ene;}
        double  GetRho()    const {return rho;}
        double  GetEta()    const {return eta;}
        double  GetPhi()    const {return phi;}
        double  GetPX()     const {return px;}
        double  GetPY()     const {return py;}
        double  GetPZ()     const {return pz;}
        double  GetRX()     const {return rx;}
        double  GetRY()     const {return ry;}
        double  GetRZ()     const {return rz;}

        // setters
        void SetSystem(const int arg_sys)    {system = arg_sys;}
        void SetID(const int arg_id)         {id     = arg_id;}
        void SetNTwr(const int64_t arg_nTwr) {nTwr   = arg_nTwr;}
        void SetEne(const double arg_ene)    {ene    = arg_ene;}
        void SetRho(const double arg_rho)    {rho    = arg_rho;}
        void SetEta(const double arg_eta)    {eta    = arg_eta;}
        void SetPhi(const double arg_phi)    {phi    = arg_phi;}
        void SetPX(const double arg_px)      {px     = arg_px;}
        void SetPY(const double arg_py)      {py     = arg_py;}
        void SetPZ(const double arg_pz)      {pz     = arg_pz;}
        void SetRX(const double arg_rx)      {rx     = arg_rx;}
        void SetRY(const double arg_ry)      {ry     = arg_ry;}
        void SetRZ(const double arg_rz)      {rz     = arg_rz;}

        // public methods
        void Reset();
        void SetInfo(const RawCluster* clust, optional<ROOT::Math::XYZVector> vtx = nullopt, optional<int> sys = nullopt);
        bool IsInAcceptance(const ClustInfo& minimum, const ClustInfo& maximum) const;
        bool IsInAcceptance(const pair<ClustInfo, ClustInfo>& range) const;

        // static methods
        static vector<string> GetListOfMembers();

        // overloaded operators
        friend bool operator <(const ClustInfo& lhs, const ClustInfo& rhs);
        friend bool operator >(const ClustInfo& lhs, const ClustInfo& rhs);
        friend bool operator <=(const ClustInfo& lhs, const ClustInfo& rhs);
        friend bool operator >=(const ClustInfo& lhs, const ClustInfo& rhs);

        // default ctor/dtor
        ClustInfo();
        ~ClustInfo();

        // ctors accepting arguments
        ClustInfo(const Const::Init init);
        ClustInfo(const RawCluster* clust, optional<ROOT::Math::XYZVector> vtx = nullopt, optional<int> sys = nullopt);

      // identify this class to ROOT
      ClassDefNV(ClustInfo, 1)

    };  // end ClustInfo def



    // comparison operator definitions ----------------------------------------

    bool operator <(const ClustInfo& lhs, const ClustInfo& rhs);
    bool operator >(const ClustInfo& lhs, const ClustInfo& rhs);
    bool operator <=(const ClustInfo& lhs, const ClustInfo& rhs);
    bool operator >=(const ClustInfo& lhs, const ClustInfo& rhs);

  }  // end Types namespace
}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

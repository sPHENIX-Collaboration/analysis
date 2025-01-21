/// ---------------------------------------------------------------------------
/*! \file   TwrInfo.h
 *  \author Derek Anderson
 *  \date   08.04.2024
 *
 *  Utility class to hold information from
 *  calorimeter towers.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_TWRINFO_H
#define SCORRELATORUTILITIES_TWRINFO_H

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
// phool libraries
#include <phool/PHCompositeNode.h>
// CaloBase libraries
#include <calobase/RawTower.h>
#include <calobase/TowerInfo.h>
// analysis utilities
#include "Constants.h"
#include "TwrTools.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Types {

    // ------------------------------------------------------------------------
    //! Tower info
    // ------------------------------------------------------------------------
    /*! A class to consolidate information
     *  about calorimeter towers. Can be
     *  built from F4A RawTower or
     *  TowerInfo objects.
     */
    class TwrInfo {

      private:

        // data members
        int     system  = numeric_limits<int>::max();
        int     status  = numeric_limits<int>::max();
        int     channel = numeric_limits<int>::max();
        int     id      = numeric_limits<int>::max();
        double  ene     = numeric_limits<double>::max();
        double  rho     = numeric_limits<double>::max();
        double  eta     = numeric_limits<double>::max();
        double  phi     = numeric_limits<double>::max();
        double  px      = numeric_limits<double>::max();
        double  py      = numeric_limits<double>::max();
        double  pz      = numeric_limits<double>::max();
        double  rx      = numeric_limits<double>::max();
        double  ry      = numeric_limits<double>::max();
        double  rz      = numeric_limits<double>::max();

        // private methods
        void Minimize();
        void Maximize();

      public:

        // getters
        int     GetSystem()  const {return system;}
        int     GetStatus()  const {return status;}
        int     GetChannel() const {return channel;}
        int     GetID()      const {return id;}
        double  GetEne()     const {return ene;}
        double  GetRho()     const {return rho;}
        double  GetEta()     const {return eta;}
        double  GetPhi()     const {return phi;}
        double  GetPX()      const {return px;}
        double  GetPY()      const {return py;}
        double  GetPZ()      const {return pz;}
        double  GetRX()      const {return rx;}
        double  GetRY()      const {return ry;}
        double  GetRZ()      const {return rz;}

        // setters
        void SetSystem(const int arg_sys)    {system  = arg_sys;}
        void SetStatus(const int arg_stat)   {status  = arg_stat;}
        void SetChannel(const int arg_chan)  {channel = arg_chan;}
        void SetID(const int arg_id)         {id      = arg_id;}
        void SetEne(const double arg_ene)    {ene     = arg_ene;}
        void SetRho(const double arg_rho)    {rho     = arg_rho;}
        void SetEta(const double arg_eta)    {eta     = arg_eta;}
        void SetPhi(const double arg_phi)    {phi     = arg_phi;}
        void SetPX(const double arg_px)      {px      = arg_px;}
        void SetPY(const double arg_py)      {py      = arg_py;}
        void SetPZ(const double arg_pz)      {pz      = arg_pz;}
        void SetRX(const double arg_rx)      {rx      = arg_rx;}
        void SetRY(const double arg_ry)      {ry      = arg_ry;}
        void SetRZ(const double arg_rz)      {rz      = arg_rz;}

        // public methods
        void Reset();
        void SetInfo(const int sys, const RawTower* tower, PHCompositeNode* topNode, optional<ROOT::Math::XYZVector> vtx = nullopt);
        void SetInfo(const int sys, const int chan, TowerInfo* tower, PHCompositeNode* topNode, optional<ROOT::Math::XYZVector> vtx = nullopt);
        bool IsInAcceptance(const TwrInfo& minimum, const TwrInfo& maximum) const;
        bool IsInAcceptance(const pair<TwrInfo, TwrInfo>& range) const;
        bool IsGood() const;

        // static methods
        static vector<string> GetListOfMembers();

        // overloaded operators
        friend bool operator <(const TwrInfo& lhs, const TwrInfo& rhs);
        friend bool operator >(const TwrInfo& lhs, const TwrInfo& rhs);
        friend bool operator <=(const TwrInfo& lhs, const TwrInfo& rhs);
        friend bool operator >=(const TwrInfo& lhs, const TwrInfo& rhs);

        // default ctor/dtor
        TwrInfo();
        ~TwrInfo();

        // ctors accepting arguments
        TwrInfo(const Const::Init init);
        TwrInfo(const int sys, const RawTower* tower, PHCompositeNode* topNode, optional<ROOT::Math::XYZVector> vtx = nullopt);
        TwrInfo(const int sys, const int chan, TowerInfo* tower, PHCompositeNode* topNode, optional<ROOT::Math::XYZVector> vtx = nullopt);

      // identify this class to ROOT
      ClassDefNV(TwrInfo, 1)

    };  // end TwrInfo def



    // comparison operator definitions ----------------------------------------

    bool operator <(const TwrInfo& lhs, const TwrInfo& rhs);
    bool operator >(const TwrInfo& lhs, const TwrInfo& rhs);
    bool operator <=(const TwrInfo& lhs, const TwrInfo& rhs);
    bool operator >=(const TwrInfo& lhs, const TwrInfo& rhs);

  }  // end Types namespace
}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

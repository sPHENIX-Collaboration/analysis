// ----------------------------------------------------------------------------
// 'RecoInfo.h'
// Derek Anderson
// 03.06.2024
//
// Utility class to hold event-level reconstructed
// information.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_RECOINFO_H
#define SCORRELATORUTILITIES_RECOINFO_H

// c++ utilities
#include <cmath>
#include <limits>
#include <string>
#include <vector>
#include <optional>
// root libraries
#include <Rtypes.h>
#include <Math/Vector3D.h>
// phool libraries
#include <phool/PHCompositeNode.h>
// analysis utilities
#include "Tools.h"
#include "Constants.h"
#include "Interfaces.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Types {

    // RecoInfo definition ----------------------------------------------------

    class RecoInfo {

      private:

        // data members
        int    nTrks     = numeric_limits<int>::max();
        double pSumTrks  = numeric_limits<double>::max();
        double eSumEMCal = numeric_limits<double>::max();
        double eSumIHCal = numeric_limits<double>::max();
        double eSumOHCal = numeric_limits<double>::max();
        double vx        = numeric_limits<double>::max();
        double vy        = numeric_limits<double>::max();
        double vz        = numeric_limits<double>::max();
        double vr        = numeric_limits<double>::max();

        // internal methods
        void Minimize();
        void Maximize();

      public:

        // getters
        int    GetNTrks()     const {return nTrks;}
        double GetPSumTrks()  const {return pSumTrks;}
        double GetESumEMCal() const {return eSumEMCal;}
        double GetESumIHCal() const {return eSumIHCal;}
        double GetESumOHCal() const {return eSumOHCal;}
        double GetVX()        const {return vx;}
        double GetVY()        const {return vy;}
        double GetVZ()        const {return vz;}
        double GetVR()        const {return vr;}

        // setters
        void SetNTrks(const int arg_nTrks)            {nTrks     = arg_nTrks;}
        void SetPSumTrks(const double arg_pSumTrks)   {pSumTrks  = arg_pSumTrks;}
        void SetESumEMCal(const double arg_eSumEMCal) {eSumEMCal = arg_eSumEMCal;}
        void SetESumIHCal(const double arg_eSumIHCal) {eSumIHCal = arg_eSumIHCal;}
        void SetESumOHCal(const double arg_eSumOHCal) {eSumOHCal = arg_eSumOHCal;}
        void SetVX(const double arg_vx)               {vx        = arg_vx;}
        void SetVY(const double arg_vy)               {vy        = arg_vy;}
        void SetVZ(const double arg_vz)               {vz        = arg_vz;}
        void SetVR(const double arg_vr)               {vr        = arg_vr;}

        // public methods
        void Reset();
        void SetInfo(PHCompositeNode* topNode);

        // static methods
        static vector<string> GetListOfMembers();

        // default ctor/dtor
        RecoInfo();
        ~RecoInfo();

        // ctors accepting arguments
        RecoInfo(Const::Init init);
        RecoInfo(PHCompositeNode* topNode);

      // identify this class to ROOT
      ClassDefNV(RecoInfo, 1)

    };  // end RecoInfo definition

  }  // end Types namespace
}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

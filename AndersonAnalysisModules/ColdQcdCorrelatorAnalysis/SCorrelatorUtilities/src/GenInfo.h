// ----------------------------------------------------------------------------
// 'GenInfo.h'
// Derek Anderson
// 03.06.2024
//
// Utility class to hold event-level generator
// information.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_GENINFO_H
#define SCORRELATORUTILITIES_GENINFO_H

// c++ utilities
#include <limits>
#include <string>
#include <vector>
#include <utility>
#include <optional>
// root libraries
#include <Rtypes.h>
#include <Math/Vector3D.h>
// analysis utilities
#include "Tools.h"
#include "ParInfo.h"
#include "Constants.h"
#include "Interfaces.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Types {

    // GenInfo definition -----------------------------------------------------

    class GenInfo {

      private:

        // atomic data members
        int    nChrgPar = numeric_limits<int>::max();
        int    nNeuPar  = numeric_limits<int>::max();
        bool   isEmbed  = false;
        double eSumChrg = numeric_limits<double>::max();
        double eSumNeu  = numeric_limits<double>::max();

        // hard scatter products
        pair<ParInfo, ParInfo> partons;

        // internal methods
        void Minimize();
        void Maximize();

      public:

        // getters
        int                    GetNChrgPar() const {return nChrgPar;}
        int                    GetNNeuPar()  const {return nNeuPar;}
        bool                   GetIsEmbed()  const {return isEmbed;}
        double                 GetESumChrg() const {return eSumChrg;}
        double                 GetESumNeu()  const {return eSumNeu;}
        ParInfo                GetPartonA()  const {return partons.first;}
        ParInfo                GetPartonB()  const {return partons.second;}
        pair<ParInfo, ParInfo> GetPartons()  const {return partons;} 

        // setters
        void SetNChrgPar(const int arg_nChrgPar)                  {nChrgPar = arg_nChrgPar;}
        void SetNNeuPar(const int arg_nNeuPar)                    {nNeuPar  = arg_nNeuPar;}
        void SetIsEmbed(const bool arg_isEmbed)                   {isEmbed  = arg_isEmbed;}
        void SetESumChrg(const double arg_eSumChrg)               {eSumChrg = arg_eSumChrg;}
        void SetESumNeu(const double arg_eSumNeu)                 {eSumNeu  = arg_eSumNeu;}
        void SetPartons(const pair<ParInfo, ParInfo> arg_partons) {partons  = arg_partons;} 

        // public methods
        void Reset();
        void SetInfo(PHCompositeNode* topNode, const bool embed, const vector<int> evtsToGrab);

        // static methods
        static vector<string> GetListOfMembers();

        // default ctor/dtor
        GenInfo();
        ~GenInfo();

        // ctor accepting PHCompositeNode* and list of subevents
        GenInfo(const Const::Init init);
        GenInfo(PHCompositeNode* topNode, const bool embed, vector<int> evtsToGrab);

      // identify this class to ROOT
      ClassDefNV(GenInfo, 1);

    };  // end GenInfo definition

  }  // end Types namespace
}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

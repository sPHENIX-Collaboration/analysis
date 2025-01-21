/// ---------------------------------------------------------------------------
/*! \file   GEvtInfo.h
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *  Utility class to hold event-level generator
 *  information.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_GEVTINFO_H
#define SCORRELATORUTILITIES_GEVTINFO_H

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

    // ------------------------------------------------------------------------
    //! Event-level generator info
    // ------------------------------------------------------------------------
    /*! A class to consolidate event-level
     *  generator (truth) information. Can
     *  be built by pointing class to a
     *  F4A node and providing a list of
     *  subevents to process.
     */ 
    class GEvtInfo {

      private:

        // data members
        int                    nChrgPar = numeric_limits<int>::max();
        int                    nNeuPar  = numeric_limits<int>::max();
        bool                   isEmbed  = false;
        double                 eSumChrg = numeric_limits<double>::max();
        double                 eSumNeu  = numeric_limits<double>::max();
        pair<ParInfo, ParInfo> partons;

        // private methods
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
        GEvtInfo();
        ~GEvtInfo();

        // ctors accepting arguments
        GEvtInfo(const Const::Init init);
        GEvtInfo(PHCompositeNode* topNode, const bool embed, vector<int> evtsToGrab);

      // identify this class to ROOT
      ClassDefNV(GEvtInfo, 1);

    };  // end GEvtInfo definition

  }  // end Types namespace
}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

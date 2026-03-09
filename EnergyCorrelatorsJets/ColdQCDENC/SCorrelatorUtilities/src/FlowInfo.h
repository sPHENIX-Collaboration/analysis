/// ---------------------------------------------------------------------------
/*! \file   FlowInfo.h
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *   Utility class to hold information from
 *   particle flow elements.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_FLOWINFO_H
#define SCORRELATORUTILITIES_FLOWINFO_H

// c++ utilities
#include <limits>
#include <vector>
#include <string>
#include <utility>
// root libraries
#include <Rtypes.h>
// particle flow libraries
#include <particleflowreco/ParticleFlowElement.h>
#include <particleflowreco/ParticleFlowElementContainer.h>
// analysis utilites
#include "Constants.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Types {

    // ------------------------------------------------------------------------
    //! Particle flow info
    // ------------------------------------------------------------------------
    /*! A class to consolidate information
     *  about particle flow objects (PFOs).
     *  Can be built from F4A ParticleFlow-
     *  Elements.
     */
    class FlowInfo {

      private:

        // data members
        int    id   = numeric_limits<int>::max();
        int    type = numeric_limits<int>::max();
        double mass = numeric_limits<double>::max();
        double eta  = numeric_limits<double>::max();
        double phi  = numeric_limits<double>::max();
        double ene  = numeric_limits<double>::max();
        double px   = numeric_limits<double>::max();
        double py   = numeric_limits<double>::max();
        double pz   = numeric_limits<double>::max();
        double pt   = numeric_limits<double>::max();

        // private methods
        void Minimize();
        void Maximize();

      public:

        // getters
        int    GetID()   const {return id;}
        int    GetType() const {return type;}
        double GetMass() const {return mass;}
        double GetEta()  const {return eta;}
        double GetPhi()  const {return phi;}
        double GetEne()  const {return ene;}
        double GetPX()   const {return px;}
        double GetPY()   const {return py;}
        double GetPZ()   const {return pz;}
        double GetPT()   const {return pt;}

        // setters
        void SetID(const int arg_id)        {id   = arg_id;}
        void SetType(const int arg_type)    {type = arg_type;}
        void SetMass(const double arg_mass) {mass = arg_mass;}
        void SetEta(const double arg_eta)   {eta  = arg_eta;}
        void SetPhi(const double arg_phi)   {phi  = arg_phi;}
        void SetEne(const double arg_ene)   {ene  = arg_ene;}
        void SetPX(const double arg_px)     {px   = arg_px;}
        void SetPY(const double arg_py)     {py   = arg_py;}
        void SetPZ(const double arg_pz)     {pz   = arg_pz;}
        void SetPT(const double arg_pt)     {pt   = arg_pt;}

        // public methods
        void Reset();
        void SetInfo(const ParticleFlowElement* flow);
        bool IsInAcceptance(const FlowInfo& minimum, const FlowInfo& maximum) const;
        bool IsInAcceptance(const pair<FlowInfo, FlowInfo>& range) const;

        // static methods
        static vector<string> GetListOfMembers();

        // overloaded operators
        friend bool operator <(const FlowInfo& lhs, const FlowInfo& rhs);
        friend bool operator >(const FlowInfo& lhs, const FlowInfo& rhs);
        friend bool operator <=(const FlowInfo& lhs, const FlowInfo& rhs);
        friend bool operator >=(const FlowInfo& lhs, const FlowInfo& rhs);

        // default ctor/dtor
        FlowInfo();
        ~FlowInfo();

        // ctors accepting arguments
        FlowInfo(const Const::Init init);
        FlowInfo(const ParticleFlowElement* flow);

      // identify this class to ROOT
      ClassDefNV(FlowInfo, 1);

    };  // end FlowInfo definition



    // comparison operator definitions ----------------------------------------

    bool operator <(const FlowInfo& lhs, const FlowInfo& rhs);
    bool operator >(const FlowInfo& lhs, const FlowInfo& rhs);
    bool operator <=(const FlowInfo& lhs, const FlowInfo& rhs);
    bool operator >=(const FlowInfo& lhs, const FlowInfo& rhs);

  }  // end Types namespace
}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

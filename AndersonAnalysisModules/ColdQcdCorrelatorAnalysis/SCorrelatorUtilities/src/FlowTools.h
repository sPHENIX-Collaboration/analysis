// ----------------------------------------------------------------------------
// 'FlowTools.h'
// Derek Anderson
// 11.16.2023
//
// Collection of frequent particle flow-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// c++ utilities
#include <cassert>
// f4a libraries
#include <fun4all/SubsysReco.h>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// particle flow libraries
#include <particleflowreco/ParticleFlowElement.h>
#include <particleflowreco/ParticleFlowElementContainer.h>

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // FlowInfo definition ----------------------------------------------------

    struct FlowInfo {

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

      void SetInfo(const ParticleFlowElement* flow) {
        id   = flow -> get_id();
        type = flow -> get_type();
        mass = flow -> get_mass();
        eta  = flow -> get_eta();
        phi  = flow -> get_phi();
        ene  = flow -> get_e();
        px   = flow -> get_px();
        py   = flow -> get_py();
        pz   = flow -> get_pz();
        pt   = flow -> get_pt();
        return;
      }  // end 'SetInfo(ParticleFlowElement*)'

      void Reset() {
        id   = numeric_limits<int>::max();
        type = numeric_limits<int>::max();
        mass = numeric_limits<double>::max();
        eta  = numeric_limits<double>::max();
        phi  = numeric_limits<double>::max();
        ene  = numeric_limits<double>::max();
        px   = numeric_limits<double>::max();
        py   = numeric_limits<double>::max();
        pz   = numeric_limits<double>::max();
        pt   = numeric_limits<double>::max();
        return;
      }  // end 'Reset()'

      static vector<string> GetListOfMembers() {
        vector<string> members = {
          "id",
          "type",
          "mass",
          "eta",
          "phi",
          "ene",
          "px",
          "py",
          "pz",
          "pt"
        };
        return members;
      }  // end 'GetListOfMembers()'

      // overloaded < operator
      friend bool operator<(const FlowInfo& lhs, const FlowInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isLessThan = (
          (lhs.mass < rhs.mass) &&
          (lhs.ene  < rhs.ene)  &&
          (lhs.eta  < rhs.eta)  &&
          (lhs.phi  < rhs.phi)  &&
          (lhs.px   < rhs.px)   &&
          (lhs.py   < rhs.py)   &&
          (lhs.pz   < rhs.pz)   &&
          (lhs.pt   < rhs.pt)
        );
        return isLessThan;

      }  // end 'operator<(FlowInfo&, FlowInfo&)'

      // overloaded > operator
      friend bool operator>(const FlowInfo& lhs, const FlowInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isGreaterThan = (
          (lhs.mass > rhs.mass) &&
          (lhs.ene  > rhs.ene)  &&
          (lhs.eta  > rhs.eta)  &&
          (lhs.phi  > rhs.phi)  &&
          (lhs.px   > rhs.px)   &&
          (lhs.py   > rhs.py)   &&
          (lhs.pz   > rhs.pz)   &&
          (lhs.pt   > rhs.pt)
        );
        return isGreaterThan;

      }  // end 'operator>(FlowInfo&, FlowInfo&)'

      // overloaded, <=, >= operators
      inline friend bool operator<=(const FlowInfo& lhs, const FlowInfo& rhs) {return !(lhs > rhs);}
      inline friend bool operator>=(const FlowInfo& lhs, const FlowInfo& rhs) {return !(lhs < rhs);}

      // default ctor/dtor
      FlowInfo()  {};
      ~FlowInfo() {};

      // ctor accepting ParticleFlowElements
      FlowInfo(const ParticleFlowElement* flow) {
        SetInfo(flow);
      }

    };  // end FlowInfo definition



    // particle flow methods --------------------------------------------------

    ParticleFlowElementContainer* GetFlowStore(PHCompositeNode* topNode) {

      ParticleFlowElementContainer* store = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");
      if (!store) {
        cerr << PHWHERE
             << "PANIC: Couldn't grab particle flow container!"
             << endl;
        assert(store);
      }
      return store;

    }  // end 'GetFlowStore(PHCompositeNode*)'



    ParticleFlowElementContainer::ConstRange GetParticleFlowObjects(PHCompositeNode* topNode) {

      // get container
      ParticleFlowElementContainer* store = GetFlowStore(topNode);

      // get objects
      ParticleFlowElementContainer::ConstRange objects = store -> getParticleFlowElements();
      if (!objects) {
        cerr << PHWHERE
             << "PANIC: Couldn't grab particle flow objects!"
             << endl;
        assert(objects);
      }
      return objects;

    }  // end 'GetParticleFlowObjects(PHCompositeNode*)'



    bool IsInAcceptance(const FlowInfo& flow, const FlowInfo& minimum, const FlowInfo& maximum) {

      return ((flow >= minimum) && (flow <= maximum));

    }  // end 'IsInAcceptance(FlowInfo&, FlowInfo&, FlowInfo&)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 'GenTools.h'
// Derek Anderson
// 10.30.2023
//
// Collection of frequent generator/MC-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// c++ utilities
#include <cmath>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// hepmc includes
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <HepMC/GenParticle.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#pragma GCC diagnostic pop

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // ParInfo definition -----------------------------------------------------

    struct ParInfo {

      int   pid     = -1;
      int   status  = 0;
      int   barcode = -1;
      int   embedID = -1;
      float charge  = -999.;
      float mass    = -999.;
      float eta     = -999.;
      float phi     = -999.;
      float ene     = -999.;
      float px      = -999.;
      float py      = -999.;
      float pz      = -999.;
      float pt      = -999.;
      float vx      = -999.;
      float vy      = -999.;
      float vz      = -999.;

      void SetInfo(const HepMC::GenParticle* particle, const int event) {
        pid     = particle -> pdg_id();
        status  = particle -> status();
        barcode = particle -> barcode();
        embedID = event;
        charge  = mapPidOntoCharge[pid];
        mass    = particle -> momentum().m();
        eta     = particle -> momentum().eta();
        phi     = particle -> momentum().phi();
        ene     = particle -> momentum().e();
        px      = particle -> momentum().px();
        py      = particle -> momentum().py();
        pz      = particle -> momentum().pz();
        pt      = particle -> momentum().perp();
        vx      = particle -> production_vertex() -> position().x();
        vy      = particle -> production_vertex() -> position().y();
        vz      = particle -> production_vertex() -> position().z();
        return;
      };

      void Reset() {
        pid     = -1;
        status  = 0;
        barcode = -1;
        embedID = -1;
        charge  = -999.;
        mass    = -999.;
        eta     = -999.;
        phi     = -999.;
        ene     = -999.;
        px      = -999.;
        py      = -999.;
        pz      = -999.;
        pt      = -999.;
        vx      = -999.;
        vy      = -999.;
        vz      = -999.;
        return;
      };

      static vector<string> GetListOfMembers() {
        vector<string> members = {
          "pid",
          "status",
          "barcode",
          "embedID",
          "charge",
          "mass",
          "eta",
          "phi",
          "ene",
          "px",
          "py",
          "pz",
          "pt",
          "vx",
          "vy",
          "vz"
        };
        return members;
      }  // end 'GetListOfMembers()'

      // overloaded < operator
      friend bool operator<(const ParInfo& lhs, const ParInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isLessThan = (
          (lhs.eta  < rhs.eta)  &&
          (lhs.phi  < rhs.phi)  &&
          (lhs.ene  < rhs.ene)  &&
          (lhs.px   < rhs.px)   &&
          (lhs.py   < rhs.py)   &&
          (lhs.pz   < rhs.pz)   &&
          (lhs.pt   < rhs.pt)
        );
        return isLessThan;

      }  // end 'operator>(ParInfo&, ParInfo&)'

      // overloaded > operator
      friend bool operator>(const ParInfo& lhs, const ParInfo& rhs) {

        // note that some quantities aren't relevant for this comparison
        const bool isGreaterThan = (
          (lhs.eta  > rhs.eta)  &&
          (lhs.phi  > rhs.phi)  &&
          (lhs.ene  > rhs.ene)  &&
          (lhs.px   > rhs.px)   &&
          (lhs.py   > rhs.py)   &&
          (lhs.pz   > rhs.pz)   &&
          (lhs.pt   > rhs.pt)
        );
        return isGreaterThan;

      }  // end 'operator>(ParInfo&, ParInfo&)'

      // overloaded, <=, >= operators
      inline friend bool operator<=(const ParInfo& lhs, const ParInfo& rhs) {return !(lhs > rhs);}
      inline friend bool operator>=(const ParInfo& lhs, const ParInfo& rhs) {return !(lhs < rhs);}

      // default ctor/dtor
      ParInfo()  {};
      ~ParInfo() {};

      ParInfo(HepMC::GenParticle* particle, const int event) {
        SetInfo(particle, event);
      };

    };  // end ParInfo definition



    // generator/mc methods ---------------------------------------------------

    PHHepMCGenEventMap* GetMcEventMap(PHCompositeNode* topNode) {

      PHHepMCGenEventMap* mapMcEvts = getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
      if (!mapMcEvts) {
        cerr << PHWHERE
             << "PANIC: HEPMC event map node is missing!"
             << endl;
        assert(mapMcEvts);
      }
      return mapMcEvts;

    }  // end 'GetMcEventMap(PHCompositeNode*)'



    PHHepMCGenEvent* GetMcEvent(PHCompositeNode* topNode, const int iEvtToGrab) {

      PHHepMCGenEventMap* mcEvtMap = GetMcEventMap(topNode);
      PHHepMCGenEvent*    mcEvt    = mcEvtMap -> get(iEvtToGrab);
      if (!mcEvt) {
        cerr << PHWHERE
             << "PANIC: Couldn't grab mc event!"
             << endl;
        assert(mcEvt);
      }
      return mcEvt;

    }  // end 'GetMcEvent(PHCompositeNode*, int)'



    HepMC::GenEvent* GetGenEvent(PHCompositeNode* topNode, const int iEvtToGrab) {

      PHHepMCGenEvent* mcEvt  = GetMcEvent(topNode, iEvtToGrab);
      HepMC::GenEvent* genEvt = mcEvt -> getEvent();
      if (!genEvt) {
        cerr << PHWHERE
             << "PANIC: Couldn't grab HepMC event!"
             << endl;
        assert(mcEvt);
      }
      return genEvt;

    }  // end 'GetGenEvent(PHCompositeNode*, int)'



    int GetEmbedID(PHCompositeNode* topNode, const int iEvtToGrab) {

      // grab mc event & return embedding id
      PHHepMCGenEvent* mcEvt = GetMcEvent(topNode, iEvtToGrab);
      return mcEvt -> get_embedding_id();

    }  // end 'GetEmbedID(PHCompositeNode*, int)'



    bool IsInParAcceptance(const ParInfo& particle, const ParInfo& minimum, const ParInfo& maximum) {

      return ((particle >= minimum) && (particle <= maximum));

    }  // end 'IsInParAcceptance(ParInfo&, ParInfo&, ParInfo&)'



    bool IsFinalState(const int status) {

      return (status == 1);

    }  // end 'IsFinalState(int)'



    bool IsHardScatterProduct(const int status) {

      return ((status == 23) || (status == 24));

    }  // end 'IsHardScatterProduct(int)'



    bool IsParton(const int pid) {

      const bool isLightQuark   = ((pid == 1) || (pid == 2));
      const bool isStrangeQuark = ((pid == 3) || (pid == 4));
      const bool isHeavyQuark   = ((pid == 5) || (pid == 6));
      const bool isGluon        = (pid == 21);
      return (isLightQuark || isStrangeQuark || isHeavyQuark || isGluon);

    }  // end 'IsParton(int)'



    bool IsOutgoingParton(const HepMC::GenParticle* par) {

      // grab particle info
      const int pid    = par -> pdg_id();
      const int status = par -> status();

      // check if is outgoing parton
      const bool isOutgoingParton = (IsHardScatterProduct(status) && IsParton(pid));
      return isOutgoingParton;

    }  // end 'IsOutgoingParton(HepMC::GenParticle*)'



    float GetParticleCharge(const int pid) {

      // particle charge
      float charge = mapPidOntoCharge[abs(pid)];

      // if antiparticle, flip charge and return
      if (pid < 0) {
        charge *= -1.;
      }
      return charge;

    }  // end 'GetParticleCharge(int)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

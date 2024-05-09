// ----------------------------------------------------------------------------
// 'ParTools.cc'
// Derek Anderson
// 03.06.2024
//
// Particle-related interfaces.
// ----------------------------------------------------------------------------

#define SCORRELATORUTILITIES_PARINTERFACES_CC

// namespace definition
#include "ParInterfaces.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  PHG4TruthInfoContainer* Interfaces::GetTruthContainer(PHCompositeNode* topNode) {

    PHG4TruthInfoContainer* container = getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
    if (!container) {
      cerr << PHWHERE
           << "PANIC: G4 truth info container node is missing!"
           << endl;
      assert(container);
    }
    return container;

  }  // end 'GetTruthContainer(PHCompositeNode*)'



  PHHepMCGenEventMap* Interfaces::GetMcEventMap(PHCompositeNode* topNode) {

    PHHepMCGenEventMap* mapMcEvts = getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
    if (!mapMcEvts) {
      cerr << PHWHERE
           << "PANIC: HepMC event map node is missing!"
           << endl;
      assert(mapMcEvts);
    }
    return mapMcEvts;

  }  // end 'GetMcEventMap(PHCompositeNode*)'



  PHHepMCGenEvent* Interfaces::GetMcEvent(PHCompositeNode* topNode, const int iEvtToGrab) {

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



  HepMC::GenEvent* Interfaces::GetGenEvent(PHCompositeNode* topNode, const int iEvtToGrab) {

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

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

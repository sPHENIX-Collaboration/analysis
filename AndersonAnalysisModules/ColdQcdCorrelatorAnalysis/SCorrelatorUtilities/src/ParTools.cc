// ----------------------------------------------------------------------------
// 'ParTools.cc'
// Derek Anderson
// 03.06.2024
//
// Collection of frequent particle-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#define SCORRELATORUTILITIES_PARTOOLS_CC

// namespace definition
#include "ParTools.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  int Tools::GetEmbedID(PHCompositeNode* topNode, const int iEvtToGrab) {

    // grab mc event & return embedding id
    PHHepMCGenEvent* mcEvt = Interfaces::GetMcEvent(topNode, iEvtToGrab);
    return mcEvt -> get_embedding_id();

  }  // end 'GetEmbedID(PHCompositeNode*, int)'



  bool Tools::IsFinalState(const int status) {

    return (status == 1);

  }  // end 'IsFinalState()'



  bool Tools::IsSubEvtGood(const int embedID, const int option, const bool isEmbed) {

    // set ID of signal
    int signalID = Const::SubEvt::NotEmbedSignal;
    if (isEmbed) {
      signalID = Const::SubEvt::EmbedSignal;
    }

    bool isSubEvtGood = true;
    switch (option) {

      // consider everything
      case Const::SubEvtOpt::Everything:
        isSubEvtGood = true;
        break;

      // only consider signal event
      case Const::SubEvtOpt::OnlySignal:
        isSubEvtGood = (embedID == signalID);
        break;

      // only consider background events
      case Const::SubEvtOpt::AllBkgd:
        isSubEvtGood = (embedID <= Const::SubEvt::Background);
        break;

      // only consider primary background event
      case Const::SubEvtOpt::PrimaryBkgd:
        isSubEvtGood = (embedID == Const::SubEvt::Background);
        break;

      // only consider pileup events
      case Const::SubEvtOpt::Pileup:
        isSubEvtGood = (embedID < Const::SubEvt::Background);
        break;

      // by default do nothing
      default:
        isSubEvtGood = true;
        break;
    }
    return isSubEvtGood;

  }  // end 'IsSubEvtGood(int, int, bool)'



  bool Tools::IsSubEvtGood(const int embedID, vector<int> subEvtsToUse) {

    bool isSubEvtGood = false;
    for (const int evtToUse : subEvtsToUse) {
      if (embedID == evtToUse) {
        isSubEvtGood = true;
        break;
      }
    }
    return isSubEvtGood;

  }  // end 'IsSubEvtGood(int, vector<int>)'



  float Tools::GetParticleCharge(const int pid) {

    // particle charge
    float charge = Const::MapPidOntoCharge()[abs(pid)];

    // if antiparticle, flip charge and return
    if (pid < 0) {
      charge *= -1.;
    }
    return charge;

  }  // end 'GetParticleCharge(int)'



  vector<int> Tools::GrabSubevents(PHCompositeNode* topNode, vector<int> subEvtsToUse) {

    // instantiate vector to hold subevents
    vector<int> subevents;
  
    PHHepMCGenEventMap* mcEvtMap = Interfaces::GetMcEventMap(topNode);
    for (
      PHHepMCGenEventMap::ConstIter itEvt = mcEvtMap -> begin();
      itEvt != mcEvtMap -> end();
      ++itEvt
    ) {

      // grab event id
      const int embedID = itEvt -> second -> get_embedding_id();

      // check to see if event is in provided list
      bool addToList = false;
      for (const int idToCheck : subEvtsToUse) {
        if (embedID == idToCheck) {
          addToList = true;
          break;
        }
      }  // end evtsToGrab loop

      // if on list, add to list of good subevents
      if (addToList) subevents.push_back(embedID);

    }
    return subevents;

  }  // end 'GrabSubevents(PHCompositeNode*, vector<int>)'



  vector<int> Tools::GrabSubevents(
    PHCompositeNode* topNode,
    const int option,
    const bool isEmbed
  ) {

    // instantiate vector to hold subevents
    vector<int> subevents;
  
    PHHepMCGenEventMap* mcEvtMap = Interfaces::GetMcEventMap(topNode);
    for (
      PHHepMCGenEventMap::ConstIter itEvt = mcEvtMap -> begin();
      itEvt != mcEvtMap -> end();
      ++itEvt
    ) {

      // grab event id
      const int embedID = itEvt -> second -> get_embedding_id();

      // if subevent satisfies option, add to list
      const bool isSubEvtGood = IsSubEvtGood(embedID, option, isEmbed);
      if (isSubEvtGood) subevents.push_back(embedID);
    }
    return subevents;

  }  // end 'GrabSubevents(PHCompositeNode*, optional<vector<int>>)'



  PHG4Particle* Tools::GetPHG4ParticleFromBarcode(const int barcode, PHCompositeNode* topNode) {

    // by default, return null pointer
    PHG4Particle* parToGrab = NULL;

    // grab truth info container
    PHG4TruthInfoContainer* container = Interfaces::GetTruthContainer(topNode);

    // loop over all particles in container to search
    PHG4TruthInfoContainer::ConstRange particles = container -> GetParticleRange();
    for (
      PHG4TruthInfoContainer::ConstIterator itPar = particles.first;
      itPar != particles.second;
      ++itPar
    ) {
      if (itPar -> second -> get_barcode() == barcode) {
        parToGrab = itPar -> second;
        break;
      }
    }  // end particle loop
    return parToGrab;

  }  // end 'GetPHG4ParticleFromBarcode(int, PHCompositeNode*)'



  PHG4Particle* Tools::GetPHG4ParticleFromTrackID(const int id, PHCompositeNode* topNode) {

    // by default, return null pointer
    PHG4Particle* parToGrab = NULL;

    // grab truth info container
    PHG4TruthInfoContainer* container = Interfaces::GetTruthContainer(topNode);

    // loop over all particles in container to search
    PHG4TruthInfoContainer::ConstRange particles = container -> GetParticleRange();
    for (
      PHG4TruthInfoContainer::ConstIterator itPar = particles.first;
      itPar != particles.second;
      ++itPar
    ) {
      if (itPar -> second -> get_track_id() == id) {
        parToGrab = itPar -> second;
        break;
      }
    }  // end particle loop
    return parToGrab;

  }  // end 'GetPHG4ParticleFromTrackID(int, PHCompositeNode*)'



  HepMC::GenParticle* Tools::GetHepMCGenParticleFromBarcode(const int barcode, PHCompositeNode* topNode) {

    // by default, return null pointer
    HepMC::GenParticle* parToGrab = NULL;

    // loop over all subevents to search
    PHHepMCGenEventMap* mcEvtMap = Interfaces::GetMcEventMap(topNode);
    for (
      PHHepMCGenEventMap::ConstIter genEvt = mcEvtMap -> begin();
      genEvt != mcEvtMap -> end();
      ++genEvt
    ) {
      for (
        HepMC::GenEvent::particle_const_iterator hepPar = genEvt -> second -> getEvent() -> particles_begin();
        hepPar != genEvt -> second -> getEvent() -> particles_end();
        ++hepPar
      ) {
        if ((*hepPar) -> barcode() == barcode) {
          parToGrab = *hepPar;
          break;
        }
      }  // end particle loop
    }  // end subevent loop
    return parToGrab;

  }  // end 'GetHepMCGenParticleFromBarcode(int, PHCompositeNode*)'

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

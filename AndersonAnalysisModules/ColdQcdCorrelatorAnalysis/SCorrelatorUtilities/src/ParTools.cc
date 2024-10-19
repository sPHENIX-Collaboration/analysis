/// ---------------------------------------------------------------------------
/*! \file   ParTools.cc
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *  Collection of frequent particle-related methods utilized in
 *  the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_PARTOOLS_CC

// namespace definition
#include "ParTools.h"

// make common namespaces implicit
using namespace std;



// particle methods ===========================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get index of signal subevent for embedding vs. not
  // --------------------------------------------------------------------------
  int Tools::GetSignal(const bool isEmbed) {

    return isEmbed ? Const::SubEvt::EmbedSignal : Const::SubEvt::NotEmbedSignal;

  }



  // --------------------------------------------------------------------------
  //! Get Embedding ID from a subevent
  // --------------------------------------------------------------------------
  int Tools::GetEmbedID(PHCompositeNode* topNode, const int iEvtToGrab) {

    // grab mc event & return embedding id
    PHHepMCGenEvent* mcEvt = Interfaces::GetMcEvent(topNode, iEvtToGrab);
    return mcEvt -> get_embedding_id();

  }  // end 'GetEmbedID(PHCompositeNode*, int)'



  // --------------------------------------------------------------------------
  //! Get an embedding ID for a given barcode
  // --------------------------------------------------------------------------
  int Tools::GetEmbedIDFromBarcode(const int barcode, PHCompositeNode* topNode) {

    // by default, return signal
    int  idEmbed      = Const::SubEvt::NotEmbedSignal;
    bool foundBarcode = false;

    // loop over all subevents to search
    PHHepMCGenEventMap* mcEvtMap = Interfaces::GetMcEventMap(topNode);
    for (
      PHHepMCGenEventMap::ConstIter genEvt = mcEvtMap -> begin();
      genEvt != mcEvtMap -> end();
      ++genEvt
    ) {

      // loop over particles
      for (
        HepMC::GenEvent::particle_const_iterator hepPar = genEvt -> second -> getEvent() -> particles_begin();
        hepPar != genEvt -> second -> getEvent() -> particles_end();
        ++hepPar
      ) {

        // break if barcode found
        if ((*hepPar) -> barcode() == barcode) {
          idEmbed      = genEvt -> first;
          foundBarcode = true;
          break;
        }
      }  // end particle loop

      // if found barcode, break
      if (foundBarcode) break;

    }  // end subevent loop
    return idEmbed;

  }  // end 'GetEmbedIDFromBarcode(int, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Get an embedding ID for a given track ID
  // --------------------------------------------------------------------------
  int Tools::GetEmbedIDFromTrackID(const int idTrack, PHCompositeNode* topNode) {

    // grab truth container
    PHG4TruthInfoContainer* info = Interfaces::GetTruthContainer(topNode);

    // return embedding id
    return info -> isEmbeded(idTrack);

  }  // end 'GetEmbedIDFromTrackID(int, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Check if a status is final state
  // --------------------------------------------------------------------------
  bool Tools::IsFinalState(const int status) {

    return (status == 1);

  }  // end 'IsFinalState()'



  // --------------------------------------------------------------------------
  //! Check if a subevent is good wrt. a provided subevent option
  // --------------------------------------------------------------------------
  bool Tools::IsSubEvtGood(
    const int embedID,
    const int option,
    const bool isEmbed
  ) {

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



  // --------------------------------------------------------------------------
  //! Check if a subevent falls in a list of subevents to use 
  // --------------------------------------------------------------------------
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



  // --------------------------------------------------------------------------
  //! Get charge of a particle based on PID
  // --------------------------------------------------------------------------
  float Tools::GetParticleCharge(const int pid) {

    // particle charge
    float charge = Const::MapPidOntoCharge()[abs(pid)];

    // if antiparticle, flip charge and return
    if (pid < 0) {
      charge *= -1.;
    }
    return charge;

  }  // end 'GetParticleCharge(int)'



  // --------------------------------------------------------------------------
  //! Get list of embedding IDs to use
  // --------------------------------------------------------------------------
  vector<int> Tools::GrabSubevents(
    PHCompositeNode* topNode,
    vector<int> subEvtsToUse
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



  // --------------------------------------------------------------------------
  //! Get subevents based on a specified option
  // --------------------------------------------------------------------------
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



  // --------------------------------------------------------------------------
  //! Find a PHG4Particle based on its barcode
  // --------------------------------------------------------------------------
  PHG4Particle* Tools::GetPHG4ParticleFromBarcode(
    const int barcode,
    PHCompositeNode* topNode
  ) {

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



  // --------------------------------------------------------------------------
  //! Find a PHG4Particle based on its "track code"
  // --------------------------------------------------------------------------
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



  // --------------------------------------------------------------------------
  //! Find a HepMC GenParticle based on its barcode
  // --------------------------------------------------------------------------
  HepMC::GenParticle* Tools::GetHepMCGenParticleFromBarcode(
    const int barcode,
    PHCompositeNode* topNode
  ) {

    // by default, return null pointer
    HepMC::GenParticle* parToGrab    = NULL;
    bool                foundBarcode = false;

    // loop over all subevents to search
    PHHepMCGenEventMap* mcEvtMap = Interfaces::GetMcEventMap(topNode);
    for (
      PHHepMCGenEventMap::ConstIter genEvt = mcEvtMap -> begin();
      genEvt != mcEvtMap -> end();
      ++genEvt
    ) {

      // loop over particles
      for (
        HepMC::GenEvent::particle_const_iterator hepPar = genEvt -> second -> getEvent() -> particles_begin();
        hepPar != genEvt -> second -> getEvent() -> particles_end();
        ++hepPar
      ) {

        // break if barcode found
        if ((*hepPar) -> barcode() == barcode) {
          parToGrab    = *hepPar;
          foundBarcode = true;
          break;
        }
      }  // end particle loop

      // if found barcode, break
      if (foundBarcode) break;

    }  // end subevent loop
    return parToGrab;

  }  // end 'GetHepMCGenParticleFromBarcode(int, PHCompositeNode*)'

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

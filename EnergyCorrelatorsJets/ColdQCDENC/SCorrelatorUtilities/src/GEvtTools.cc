/// ---------------------------------------------------------------------------
/*! \file   GEvtTools.cc
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *  Collection of frequent event-level generator methods utilized
 *  in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_GEVTTOOLS_CC

// namespace definition
#include "GEvtTools.h"

// make common namespaces implicit
using namespace std;



// event-level generator methods ==============================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get total number of final state particles
  // --------------------------------------------------------------------------
  /*! A specific subset can specified by the
   *  subset identifier (Const::Subset).
   *  Optionally, a specific charge can be
   *  specified to get the total number of
   *  final state particles with that charge.
   */
  int64_t Tools::GetNumFinalStatePars(
    PHCompositeNode* topNode,
    const vector<int> evtsToGrab,
    const Const::Subset subset,
    optional<float> chargeToGrab
  ) {

    // loop over subevents
    int64_t nPar = 0;
    for (const int evtToGrab : evtsToGrab) {

      // loop over particles
      HepMC::GenEvent* genEvt = Interfaces::GetGenEvent(topNode, evtToGrab);
      for (
        HepMC::GenEvent::particle_const_iterator particle = genEvt -> particles_begin();
        particle != genEvt -> particles_end();
        ++particle
      ) {

        // check if particle is final state
        const bool isFinalState = Tools::IsFinalState((*particle) -> status());
        if (!isFinalState) continue;

        // if chargeToGrab is set, select only particle with charge
        const float charge = Tools::GetParticleCharge((*particle) -> pdg_id());
        if (chargeToGrab.has_value()) {
          if (charge == chargeToGrab.value()) {
            ++nPar;
          }
        } else {
          switch (subset) {

            // everything
            case Const::Subset::All:
              ++nPar;
              break;

            // all charged
            case Const::Subset::Charged:
              if (charge != 0.) {
                ++nPar;
              }
              break;

            // only neutral
            case Const::Subset::Neutral:
              if (charge == 0.) {
                ++nPar;
              }
              break;

            default:
              ++nPar;
              break;
          }
        }  // end if-else chargeToGrab.has_value()
      }  // end particle loop
    }  // end subevent loop
    return nPar;

  }  // end 'GetNumFinalStatePars(PHCompositeNode*, vector<int>, optional<float>, optional<float> bool)'



  // --------------------------------------------------------------------------
  //! Get sum of energy for all final state particles
  // --------------------------------------------------------------------------
  /*! A specific subset can specified by the
   *  subset identifier (Const::Subset).
   *  Optionally, a specific charge can be
   *  specified to get the total number of
   *  final state particles with that charge.
   */
  double Tools::GetSumFinalStateParEne(
    PHCompositeNode* topNode,
    const vector<int> evtsToGrab,
    const Const::Subset subset,
    optional<float> chargeToGrab
  ) {

    // loop over subevents
    double eSum = 0.;
    for (const int evtToGrab : evtsToGrab) {

      HepMC::GenEvent* genEvt = Interfaces::GetGenEvent(topNode, evtToGrab);
      for (
        HepMC::GenEvent::particle_const_iterator particle = genEvt -> particles_begin();
        particle != genEvt -> particles_end();
        ++particle
      ) {

        // check if particle is final state
        const bool isFinalState = Tools::IsFinalState((*particle) -> status());
        if (!isFinalState) continue;

        // if chargeToGrab is set, select only particle with charge
        const float charge = Tools::GetParticleCharge((*particle) -> pdg_id());
        const float energy = (*particle) -> momentum().e();
        if (chargeToGrab.has_value()) {
          if (charge == chargeToGrab.value()) {
            eSum += energy;
          }
        } else {
          switch (subset) {

            // everything
            case Const::Subset::All:
              eSum += energy;
              break;

            // all charged
            case Const::Subset::Charged:
              if (charge != 0.) {
                eSum += energy;
              }
              break;

            // only neutral
            case Const::Subset::Neutral:
              if (charge == 0.) {
                eSum += energy;
              }
              break;

            default:
              eSum += energy;
              break;
          }
        }  // end if-else chargeToGrab.has_value()
      }  // end particle loop
    }  // end subevent loop
    return eSum;

  }  // end 'GetSumFinalStateParEne(PHCompositeNode*, vector<int>, int, optional<float>)'



  // --------------------------------------------------------------------------
  //! Pull information of a parton with a specified status from a subevent
  // --------------------------------------------------------------------------
  Types::ParInfo Tools::GetPartonInfo(
    PHCompositeNode* topNode,
    const int event,
    const int status
  ) {

    // pick out relevant sub-sevent to grab
    HepMC::GenEvent* genEvt = Interfaces::GetGenEvent(topNode, event);

    // loop over particles
    Types::ParInfo parton;
    for (
      HepMC::GenEvent::particle_const_iterator particle = genEvt -> particles_begin();
      particle != genEvt -> particles_end();
      ++particle
    ) {

      // grab particle info
      parton.Reset();
      parton.SetInfo(*particle, event);

      // ignore all non-partons
      if (parton.IsParton()) continue;

      // exit loop with info if parton is desired status
      if (parton.IsHardScatterProduct()) {
        break;
      } 
    }  // end particle loop
    return parton;

  }  // end 'GetPartonInfo(PHCompositeNode*, int, int)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

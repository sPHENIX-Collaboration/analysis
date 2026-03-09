/// ---------------------------------------------------------------------------
/*! \file   ParTools.h
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *  Collection of frequent particle-related methods utilized in
 *  the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_PARTOOLS_H
#define SCORRELATORUTILITIES_PARTOOLS_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// c++ utilities
#include <cmath>
#include <vector>
#include <optional>
// phool libraries
#include <phool/PHCompositeNode.h>
// PHG4 libraries
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
// hepmc libraries
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <HepMC/GenParticle.h>
#include <HepMC/GenParticle.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
// PHG4 libraries
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
// analysis utilities
#include "Constants.h"
#include "Interfaces.h"

#pragma GCC diagnostic pop

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Tools {

    // particle methods -------------------------------------------------------

    int                 GetSignal(const bool isEmbed);
    int                 GetEmbedID(PHCompositeNode* topNode, const int iEvtToGrab);
    int                 GetEmbedIDFromBarcode(const int barcode, PHCompositeNode* topNode);
    int                 GetEmbedIDFromTrackID(const int idTrack, PHCompositeNode* topNode);
    bool                IsFinalState(const int status);
    bool                IsSubEvtGood(const int embedID, const int option, const bool isEmbed);
    bool                IsSubEvtGood(const int embedID, vector<int> subEvtsToUse);
    float               GetParticleCharge(const int pid);
    vector<int>         GrabSubevents(PHCompositeNode* topNode, vector<int> subEvtsToUse);
    vector<int>         GrabSubevents(PHCompositeNode* topNode, const int option = Const::SubEvtOpt::Everything, const bool isEmbed = false);
    PHG4Particle*       GetPHG4ParticleFromBarcode(const int barcode, PHCompositeNode* topNode);
    PHG4Particle*       GetPHG4ParticleFromTrackID(const int id, PHCompositeNode* topNode);
    HepMC::GenParticle* GetHepMCGenParticleFromBarcode(const int barcode, PHCompositeNode* topNode);

  }  // end Tools namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 'EvtTools.h'
// Derek Anderson
// 10.30.2023
//
// Collection of frequent event-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// c++ utilities
#include <vector>
#include <cassert>
#include <utility>
#include <optional>
// root libraries
#include <Math/Vector3D.h>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// tracking includes
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
// calo includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>
// hepmc includes
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <HepMC/GenParticle.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
// vertex libraries
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
// analysis utilities
#include "Constants.h"
#include "Interfaces.h"
#include "CalTools.h"
#include "TrkTools.h"
#include "VtxTools.h"
#include "GenTools.h"

#pragma GCC diagnostic pop

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // forward declarations used in Reco/GenInfo
    long                  GetNumTrks(PHCompositeNode* topNode);
    long                  GetNumFinalStatePars(PHCompositeNode* topNode, const vector<int> evtsToGrab, const int subset = 0, optional<float> chargeToGrab = nullopt);
    double                GetSumTrkMomentum(PHCompositeNode* topNode);
    double                GetSumCaloEne(PHCompositeNode* topNode, const string store);
    double                GetSumFinalStateParEne(PHCompositeNode* topNode, const vector<int> evtsToGrab, const int subset, optional<float> chargeToGrab = nullopt);
    ParInfo               GetPartonInfo(PHCompositeNode* topNode, const int event, const int status);
    ROOT::Math::XYZVector GetRecoVtx(PHCompositeNode* topNode);


    // RecoInfo definition ----------------------------------------------------

    struct RecoInfo {

      // data members
      int    nTrks     = -1;
      double pSumTrks  = -999.;
      double eSumEMCal = -999.;
      double eSumIHCal = -999.;
      double eSumOHCal = -999.;
      double vx        = -999.;
      double vy        = -999.;
      double vz        = -999.;

      void SetInfo(PHCompositeNode* topNode) {

        // get sums
        nTrks     = GetNumTrks(topNode);
        pSumTrks  = GetSumTrkMomentum(topNode);
        eSumEMCal = GetSumCaloEne(topNode, "CLUSTER_CEMC");
        eSumIHCal = GetSumCaloEne(topNode, "CLUSTER_HCALIN");
        eSumOHCal = GetSumCaloEne(topNode, "CLUSTER_HCALOUT");

        // get vertex
        ROOT::Math::XYZVector vtx = GetRecoVtx(topNode);
        vx = vtx.x();
        vy = vtx.y();
        vz = vtx.z();
        return;
      }  // end 'SetInfo(PHCompositeNode*)'

      void Reset() {
        nTrks     = -1;
        pSumTrks  = -999.;
        eSumEMCal = -999.;
        eSumIHCal = -999.;
        eSumOHCal = -999.;
        vx        = -999.;
        vy        = -999.;
        vz        = -999.;
        return;
      }  // end 'Reset()'

      static vector<string> GetListOfMembers() {
        vector<string> members = {
          "nTrks",
          "pSumTrks",
          "eSumEMCal",
          "eSumIHCal",
          "eSumOHCal",
          "vx",
          "vy",
          "vz"
        };
        return members;
      }  // end 'GetListOfMembers()'

      // default ctor/dtor
      RecoInfo()  {};
      ~RecoInfo() {};

      // ctor accepting PHCompositeNode*
      RecoInfo(PHCompositeNode* topNode) {
        SetInfo(topNode);
      };

    };  // end RecoInfo definition



    // GenInfo definition ------------------------------------------------------

    struct GenInfo {

      // atomic data members
      int    nChrgPar = -1;
      int    nNeuPar  = -1;
      bool   isEmbed  = false;
      double eSumChrg = -999.;
      double eSumNeu  = -999.;

      // hard scatter products
      pair<ParInfo, ParInfo> partons;

      void SetInfo(PHCompositeNode* topNode, const bool embed, const vector<int> evtsToGrab) {

        // set embed flag
        isEmbed = embed;

        // set parton info
        isEmbed  = embed;
        if (isEmbed) {
          partons.first  = GetPartonInfo(topNode, Signal::Embed, HardScatterStatus::First);
          partons.second = GetPartonInfo(topNode, Signal::Embed, HardScatterStatus::Second);

        } else {
          partons.first  = GetPartonInfo(topNode, Signal::NotEmbed, HardScatterStatus::First);
          partons.second = GetPartonInfo(topNode, Signal::NotEmbed, HardScatterStatus::Second);
        }

        // get sums
        nChrgPar = GetNumFinalStatePars(topNode, evtsToGrab, Subset::Charged);
        nNeuPar  = GetNumFinalStatePars(topNode, evtsToGrab, Subset::Neutral);
        eSumChrg = GetSumFinalStateParEne(topNode, evtsToGrab, Subset::Charged);
        eSumNeu  = GetSumFinalStateParEne(topNode, evtsToGrab, Subset::Neutral);
        return;
      }  // end 'SetInfo(PHCompositeNode*, vector<int>)'

      void Reset() {
        nChrgPar = -1;
        nNeuPar  = -1;
        isEmbed  = false;
        eSumChrg = -999.;
        eSumNeu  = -999.;
        return;
      }  // end 'Reset()'

      static vector<string> GetListOfMembers() {
        vector<string> members = {
          "nChrgPar",
          "nNeuPar",
          "isEmbed",
          "eSumChrg",
          "eSumNeu"
        };
        AddLeavesToVector<ParInfo>(members, "PartonA");
        AddLeavesToVector<ParInfo>(members, "PartonB");
        return members;
      }  // end 'GetListOfMembers()'

      // default ctor/dtor
      GenInfo()  {};
      ~GenInfo() {};

      // ctor accepting PHCompositeNode* and list of subevents
      GenInfo(PHCompositeNode* topNode, const bool embed, vector<int> evtsToGrab) {
        SetInfo(topNode, embed, evtsToGrab);
      };
    };  // end GenInfo definition



    // event methods ----------------------------------------------------------

    long GetNumTrks(PHCompositeNode* topNode) {

      // grab size of track map
      SvtxTrackMap* mapTrks = GetTrackMap(topNode);
      return mapTrks -> size();

    }  // end 'GetNumTrks(PHCompositeNode*)'



    long GetNumFinalStatePars(PHCompositeNode* topNode, const vector<int> evtsToGrab, const int subset, optional<float> chargeToGrab) {

      // loop over subevents
      long nPar = 0;
      for (const int evtToGrab : evtsToGrab) {

        // loop over particles
        HepMC::GenEvent* genEvt = GetGenEvent(topNode, evtToGrab);
        for (
          HepMC::GenEvent::particle_const_iterator particle = genEvt -> particles_begin();
          particle != genEvt -> particles_end();
          ++particle
        ) {

          // check if particle is final state
          if (!IsFinalState((*particle) -> status())) continue;

          // if chargeToGrab is set, select only particle with charge
          const float charge = GetParticleCharge((*particle) -> pdg_id());
          if (chargeToGrab.has_value()) {
            if (charge == chargeToGrab.value()) {
              ++nPar;
            }
          } else {
            switch (subset) {

              // everything
              case Subset::All:
                ++nPar;
                break;

              // all charged
              case Subset::Charged:
                if (charge != 0.) {
                  ++nPar;
                }
                break;

              // only neutral
              case Subset::Neutral:
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



    double GetSumTrkMomentum(PHCompositeNode* topNode) {

      // grab track map
      SvtxTrackMap* mapTrks = GetTrackMap(topNode);

      // loop over tracks
      double pSum = 0.;
      for (
        SvtxTrackMap::Iter itTrk = mapTrks -> begin();
        itTrk != mapTrks -> end();
        ++itTrk
      ) {

        // grab track
        SvtxTrack* track = itTrk -> second;
        if (!track) continue;

        // momentum to sum
        const double pTrk = std::hypot(track -> get_px(), track -> get_py(), track -> get_pz());
        pSum += pTrk;
      }
      return pSum;

    }  // end 'GetSumTrkMomentum(PHCompositeNode*)'



    double GetSumCaloEne(PHCompositeNode* topNode, const string store) {

      // grab clusters
      RawClusterContainer::ConstRange clusters = GetClusters(topNode, store);

      // loop over emcal clusters
      double eSum = 0.;
      for (
        RawClusterContainer::ConstIterator itClust = clusters.first;
        itClust != clusters.second;
        itClust++
      ) {

        // grab cluster and increment sum
        const RawCluster* cluster = itClust -> second;
        if (!cluster) {
          continue;
        } else {
          eSum += cluster -> get_energy();
        }
      }  // end cluster loop
      return eSum;

    }  // end 'GetSumCaloEne(PHCompositeNode*, string)'



    double GetSumFinalStateParEne(PHCompositeNode* topNode, const vector<int> evtsToGrab, const int subset, optional<float> chargeToGrab) {

      // loop over subevents
      double eSum = 0.;
      for (const int evtToGrab : evtsToGrab) {

        HepMC::GenEvent* genEvt = GetGenEvent(topNode, evtToGrab);
        for (
          HepMC::GenEvent::particle_const_iterator particle = genEvt -> particles_begin();
          particle != genEvt -> particles_end();
          ++particle
        ) {

          // check if particle is final state
          if (!IsFinalState((*particle) -> status())) continue;

          // if chargeToGrab is set, select only particle with charge
          const float charge = GetParticleCharge((*particle) -> pdg_id());
          const float energy = (*particle) -> momentum().e();
          if (chargeToGrab.has_value()) {
            if (charge == chargeToGrab.value()) {
              eSum += energy;
            }
          } else {
            switch (subset) {

              // everything
              case Subset::All:
                eSum += energy;
                break;

              // all charged
              case Subset::Charged:
                if (charge != 0.) {
                  eSum += energy;
                }
                break;

              // only neutral
              case Subset::Neutral:
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



    ParInfo GetPartonInfo(PHCompositeNode* topNode, const int event, const int status) {

      // pick out relevant sub-sevent to grab
      HepMC::GenEvent* genEvt = GetGenEvent(topNode, event);

      // loop over particles
      ParInfo parton;
      for (
        HepMC::GenEvent::particle_const_iterator particle = genEvt -> particles_begin();
        particle != genEvt -> particles_end();
        ++particle
      ) {

        // ignore all non-partons
        if (!IsParton((*particle) -> pdg_id())) continue;

        // set info if parton is desired status
        if (((*particle) -> status()) == status) {
          parton.SetInfo(*particle, event);
        }
      }  // end particle loop
      return parton;

    }  // end 'GetPartonInfo(PHCompositeNode*, int, int)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.csts.h'
// Derek Anderson
// 01.18.2023
//
// A module to produce a tree of jets for the sPHENIX
// Cold QCD Energy-Energy Correlator analysis.
//
// Derived from code by Antonio Silva (thanks!!)
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // constituent methods ------------------------------------------------------

  bool SCorrelatorJetTree::IsGoodParticle(HepMC::GenParticle* par, const bool ignoreCharge) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 1)) {
      cout << "SCorrelatorJetTree::IsGoodParticle(HepMC::GenParticle*) Checking if MC particle is good..." << endl;
    }

    // check charge if needed
    const bool isJetCharged  = (m_jetType != 1);
    const bool doChargeCheck = (isJetCharged && !ignoreCharge);

    int   parID;
    bool  isGoodCharge;
    float parChrg;
    if (doChargeCheck) {
      parID        = par -> pdg_id();
      parChrg      = GetParticleCharge(parID);
      isGoodCharge = (parChrg != 0.);
    } else {
      isGoodCharge = true;
    }

    const double parEta       = par -> momentum().eta();
    const double parPx        = par -> momentum().px();
    const double parPy        = par -> momentum().py();
    const double parPt        = sqrt((parPx * parPx) + (parPy * parPy));
    const bool   isInPtRange  = ((parPt  > m_parPtRange[0])  && (parPt  < m_parPtRange[1]));
    const bool   isInEtaRange = ((parEta > m_parEtaRange[0]) && (parEta < m_parEtaRange[1]));
    const bool   isGoodPar    = (isGoodCharge && isInPtRange && isInEtaRange);
    return isGoodPar;

  }  // end 'IsGoodParticle(HepMC::GenParticle*, bool)'



  bool SCorrelatorJetTree::IsGoodTrack(SvtxTrack* track, PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 1)) {
      cout << "SCorrelatorJetTree::IsGoodTrack(SvtxTrack*) Checking if track is good..." << endl;
    }

    // grab track info
    const double trkPt      = track -> get_pt();
    const double trkEta     = track -> get_eta();
    const double trkQual    = track -> get_quality();
    const double trkDeltaPt = GetTrackDeltaPt(track);
    const int    trkNMvtx   = GetNumLayer(track, SUBSYS::MVTX);
    const int    trkNIntt   = GetNumLayer(track, SUBSYS::INTT);
    const int    trkNTpc    = GetNumLayer(track, SUBSYS::TPC);

    // grab track dca
    const auto   trkDca   = GetTrackDcaPair(track, topNode);
    const double trkDcaXY = trkDca.first;
    const double trkDcaZ  = trkDca.second;

    // if above max pt used to fit dca width,
    // use value of fit at max pt
    double ptEvalXY = (trkPt > m_dcaPtFitMaxXY) ? m_dcaPtFitMaxXY : trkPt;
    double ptEvalZ  = (trkPt > m_dcaPtFitMaxZ)  ? m_dcaPtFitMaxZ  : trkPt;

    // check if dca is good
    bool   isInDcaRangeXY = false;
    bool   isInDcaRangeZ  = false;
    if (m_doDcaSigmaCut) {
      isInDcaRangeXY = (abs(trkDcaXY) < (m_nSigCutXY * (m_fSigDcaXY -> Eval(ptEvalXY))));
      isInDcaRangeZ  = (abs(trkDcaZ)  < (m_nSigCutZ  * (m_fSigDcaZ  -> Eval(ptEvalZ))));
    } else {
      isInDcaRangeXY = ((trkDcaXY > m_trkDcaRangeXY[0]) && (trkDcaXY < m_trkDcaRangeXY[1]));
      isInDcaRangeZ  = ((trkDcaZ  > m_trkDcaRangeZ[0])  && (trkDcaZ  < m_trkDcaRangeZ[1]));
    }  

    // apply cuts
    const bool isSeedGood       = IsGoodTrackSeed(track);
    const bool isInPtRange      = ((trkPt      > m_trkPtRange[0])      && (trkPt      <  m_trkPtRange[1]));
    const bool isInEtaRange     = ((trkEta     > m_trkEtaRange[0])     && (trkEta     <  m_trkEtaRange[1]));
    const bool isInQualRange    = ((trkQual    > m_trkQualRange[0])    && (trkQual    <  m_trkQualRange[1]));
    const bool isInNMvtxRange   = ((trkNMvtx   > m_trkNMvtxRange[0])   && (trkNMvtx   <= m_trkNMvtxRange[1]));
    const bool isInNInttRange   = ((trkNIntt   > m_trkNInttRange[0])   && (trkNIntt   <= m_trkNInttRange[1]));
    const bool isInNTpcRange    = ((trkNTpc    > m_trkNTpcRange[0])    && (trkNTpc    <= m_trkNTpcRange[1]));
    const bool isInDeltaPtRange = ((trkDeltaPt > m_trkDeltaPtRange[0]) && (trkDeltaPt <  m_trkDeltaPtRange[1]));
    const bool isInNumRange     = (isInNMvtxRange && isInNInttRange && isInNTpcRange);
    const bool isInDcaRange     = (isInDcaRangeXY && isInDcaRangeZ);
    const bool isGoodTrack      = (isSeedGood && isInPtRange && isInEtaRange && isInQualRange && isInNumRange && isInDcaRange && isInDeltaPtRange);
    return isGoodTrack;

  }  // end 'IsGoodTrack(SvtxTrack*)'



  bool SCorrelatorJetTree::IsGoodFlow(ParticleFlowElement* flow) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 1)) {
      cout << "SCorrelatorJetTree::IsGoodFlow(ParticleFlowElement*) Checking if particle flow element is good..." << endl;
    }

    // TODO: explore particle flow cuts
    const double pfEta        = flow -> get_eta();
    const bool   isInEtaRange = ((pfEta > m_flowEtaRange[0]) && (pfEta < m_flowEtaRange[1]));
    const bool   isGoodFlow   = isInEtaRange;
    return isGoodFlow;

  }  // end 'IsGoodFlow(ParticleFlowElement*)'



  bool SCorrelatorJetTree::IsGoodECal(CLHEP::Hep3Vector& hepVecECal) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 1)) {
      cout << "SCorrelatorJetTree::IsGoodECal(CLHEP::Hep3Vector&) Checking if ECal cluster is good..." << endl;
    }

    const double clustPt      = hepVecECal.perp();
    const double clustEta     = hepVecECal.pseudoRapidity();
    const bool   isInPtRange  = ((clustPt  > m_ecalPtRange[0])  && (clustPt  < m_ecalPtRange[1]));
    const bool   isInEtaRange = ((clustEta > m_ecalEtaRange[0]) && (clustEta < m_ecalEtaRange[1]));
    const bool   isGoodClust  = (isInPtRange && isInEtaRange);
    return isGoodClust;

  }  // end 'IsGoodECal(CLHEP::Hep3Vector&)'



  bool SCorrelatorJetTree::IsGoodHCal(CLHEP::Hep3Vector& hepVecHCal) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 1)) {
      cout << "SCorrelatorJetTree::IsGoodHCal(CLHEP::Hep3Vector&) Checking if HCal cluster is good..." << endl;
    }

    // TODO: explore particle cuts. These should vary with particle charge/species.
    const double clustPt      = hepVecHCal.perp();
    const double clustEta     = hepVecHCal.pseudoRapidity();
    const bool   isInPtRange  = ((clustPt  > m_hcalPtRange[0])  && (clustPt  < m_hcalPtRange[1]));
    const bool   isInEtaRange = ((clustEta > m_hcalEtaRange[0]) && (clustEta < m_hcalEtaRange[1]));
    const bool   isGoodClust  = (isInPtRange && isInEtaRange);
    return isGoodClust;

  }  // end 'IsGoodHCal(CLHEP::Hep3Vector&)'



  bool SCorrelatorJetTree::IsGoodTrackSeed(SvtxTrack* track) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 2)) {
      cout << "SCorrelatorJetTree::IsGoodSeedTrack(SvtxTrack*) Checking if track seed is good..." << endl;
    }

    // get track seeds
    TrackSeed* trkSiSeed  = track -> get_silicon_seed();
    TrackSeed* trkTpcSeed = track -> get_tpc_seed();

    // check if one or both seeds are present as needed
    bool isSeedGood = (trkSiSeed && trkTpcSeed);
    if (!m_requireSiSeeds) {
      isSeedGood = (trkSiSeed || trkTpcSeed);
    }
    return isSeedGood;

  }  // end 'IsGoodSeed(SvtxTrack*)'



  bool SCorrelatorJetTree::IsOutgoingParton(HepMC::GenParticle* par) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 2)) {
      cout << "SCorrelatorJetTree::IsParton(HepMC::GenParticle*) Checking if particle is a parton..." << endl;
    }

    // grab particle info
    const int pid    = par -> pdg_id();
    const int status = par -> status();

    // check if is outgoing parton
    const bool isStatusGood     = ((status == 23) || (status == 24));
    const bool isLightQuark     = ((pid == 1)     || (pid == 2));
    const bool isStrangeQuark   = ((pid == 3)     || (pid == 4));
    const bool isHeavyQuark     = ((pid == 5)     || (pid == 6));
    const bool isGluon          = (pid == 21);
    const bool isParton         = (isLightQuark || isStrangeQuark || isHeavyQuark || isGluon);
    const bool isOutgoingParton = (isStatusGood && isParton);
    return isOutgoingParton;

  }  // end 'IsOutgoingParton(HepMC::GenParticle*)'



  pair<double, double> SCorrelatorJetTree::GetTrackDcaPair(SvtxTrack *track, PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetTrackDcaPair(SvtxTrack*, PHCompositeNode*) Getting track dca values..." << endl;
    }

    // get global vertex and convert to acts vector
    GlobalVertex* sphxVtx = GetGlobalVertex(topNode);
    Acts::Vector3 actsVtx = Acts::Vector3(sphxVtx -> get_x(), sphxVtx -> get_y(), sphxVtx -> get_z());

    // return dca
    const auto dcaAndErr = TrackAnalysisUtils::get_dca(track, actsVtx);
    return make_pair(dcaAndErr.first.first, dcaAndErr.second.first);

  }  // end 'GetTrackDcaPair(SvtxTrack*, PHCompositeNode*)'



  double SCorrelatorJetTree::GetTrackDeltaPt(SvtxTrack* track) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetTrackDeltaPt(SvtxTrack*) Getting track delta pt..." << endl;
    }

    // grab covariances
    const float trkCovXX = track -> get_error(3, 3);
    const float trkCovXY = track -> get_error(3, 4);
    const float trkCovYY = track -> get_error(4, 4);

    // grab momentum
    const float trkPx = track -> get_px();
    const float trkPy = track -> get_py();
    const float trkPt = track -> get_pt();
 
    // calculate delta-pt
    const float numer    = (trkCovXX * trkPx * trkPx) + 2 * (trkCovXY * trkPx * trkPy) + (trkCovYY * trkPy * trkPy);
    const float denom    = (trkPx * trkPx) + (trkPy * trkPy); 
    const float ptDelta2 = numer / denom;
    const float ptDelta  = sqrt(ptDelta2) / trkPt;
    return ptDelta;

  }  // end 'GetTrackDeltaPt(SvtxTrack*)'



  float SCorrelatorJetTree::GetParticleCharge(const int pid) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 2)) {
      cout << "SCorrelatorJetTree::GetParticleCharge(int) Grabbing MC particle charge..." << endl;
    }

    // particle charge
    float charge(-999.);

    switch (abs(pid)) {
      // e+/e-
      case 11:
        charge = 1.;
        break;
      // e neutrinos
      case 12:
        charge = 0.;
        break;
      // mu-/mu+
      case 13:
        charge = -1.;
        break;
      // mu neutrinos
      case 14:
        charge = 0.;
        break;
      // tau-/tau+
      case 15:
        charge = -1.;
        break;
      // tau neutrinos
      case 16:
        charge = 0.;
        break;
      // photon
      case 22:
        charge = 0.;
        break;
      // Z0
      case 23:
        charge = 0.;
        break;
      // W+/W-
      case 24:
        charge = 1.;
        break;
      // pi0
      case 111:
        charge = 0.;
        break;
      // pi+/pi-
      case 211:
        charge = 1.;
        break;
      // K0 (long)
      case 130:
        charge = 0.;
        break;
      // K0 (short)
      case 310:
        charge = 0.;
        break;
      // K+/K-
      case 321:
        charge = 1.;
        break;
      // D+/D-
      case 441:
        charge = 1.;
        break;
      // D0
      case 421:
        charge = 0.;
        break;
      // DS+/DS-
      case 431:
        charge = 1.;
        break;
      // eta
      case 221:
        charge = 0.;
        break;
      // proton/antiproton
      case 2212:
        charge = 1.;
        break;
      // neutron
      case 2112:
        charge = 0.;
        break;
      // lambda
      case 3122:
        charge = 0.;
        break;
      // sigma+/antisigma+
      case 3222:
        charge = 1.;
        break;
      // sigma0
      case 3212:
        charge = 0.;
        break;
      // sigma-/antisigma-
      case 3112:
        charge = -1.;
        break;
      // xi0
      case 3322:
        charge = 0.;
        break;
      // deuteron
      case 700201:
        charge = 0.;
        break;
      // alpha
      case 700202:
        charge = 2.;
        break;
      // triton
      case 700301:
        charge = 0.;
        break;
      // he3
      case 700302:
        charge = 3.;
        break;
      default:
        charge = 0.;
        if (m_doDebug && (Verbosity() > 1)) {
          cerr << "SCorrelatorJetTree::GetParticleCharge(int) WARNING: trying to determine charge of unknown particle (PID = " << pid << ")..." << endl;
        }
        break;
    }  // end switch (abs(pid))

    // if antiparticle, flip charge and return
    if (pid < 0) {
      charge *= -1.;
    }
    return charge;

  }  // end 'GetParticleCharge(int)'



  int SCorrelatorJetTree::GetNumLayer(SvtxTrack* track, const uint8_t subsys) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 3)) {
      cout << "SCorrelatorJetTree::GetNumLayer(SvtxTrack*, uint8_t) Grabbing number of track clusters..." << endl;
    }

    // issue warning if subsys is not set correctly
    const bool isSubsysWrong = (subsys > 2);
    if (isSubsysWrong && m_doDebug && (Verbosity() > 3)) {
      cerr << "SCorrelatorJetTree::GetNumLayer(SvtxTrack*, uint8_t) PANIC: trying to determine no. of clusters for an unknown subsystem (subsys = " << subsys << ")! Aborting!" << endl;
      assert(subsys <= 2);
    }

    // check if seed is good
    const bool isSeedGood = IsGoodTrackSeed(track);
    if (!isSeedGood && m_doDebug && (Verbosity() > 3)) {
      cerr << "SCorrelatorJetTree::GetNumLayer(SvtxTrack*, uint8_t) PANIC: track seed(s) is (are) no good! Aborting!" << endl;
      assert(isSeedGood);
    }

    // get both track seeds
    TrackSeed* trkSiSeed  = track -> get_silicon_seed();
    TrackSeed* trkTpcSeed = track -> get_tpc_seed();

    // select relevant seed
    const bool hasBothSeeds   = (trkSiSeed  && trkTpcSeed);
    const bool hasOnlyTpcSeed = (!trkSiSeed && trkTpcSeed);

    TrackSeed* seed = NULL;
    switch (subsys) {
      case SUBSYS::MVTX:
        if (hasBothSeeds)   seed = trkSiSeed;
        if (hasOnlyTpcSeed) seed = trkTpcSeed;
        break;
      case SUBSYS::INTT:
        if (hasBothSeeds)   seed = trkSiSeed;
        if (hasOnlyTpcSeed) seed = trkTpcSeed;
        break;
      case SUBSYS::TPC:
        seed = trkTpcSeed;
        break;
    }
    if (!seed && m_doDebug && (Verbosity() > 3)) {
      cerr << "SCorrelatorJetTree::GetNumLayer(SvtxTrack*, uint8_t) PANIC: couldn't set seed! Aborting!" << endl;
      assert(seed);
    }

    // set min no. of layers
    const int minInttLayer = CONST::NMvtxLayer;
    const int minTpcLayer  = CONST::NMvtxLayer + CONST::NInttLayer;

    // reset hit flags
    switch (subsys) {
      case SUBSYS::MVTX:
        for (int iMvtxLayer = 0; iMvtxLayer < CONST::NMvtxLayer; iMvtxLayer++) {
          isMvtxLayerHit[iMvtxLayer] = false;
        }
        break;
      case SUBSYS::INTT:
        for (int iInttLayer = 0; iInttLayer < CONST::NInttLayer; iInttLayer++) {
          isInttLayerHit[iInttLayer] = false;
        }
        break;
      case SUBSYS::TPC:
        for (int iTpcLayer = 0; iTpcLayer < CONST::NTpcLayer; iTpcLayer++) {
          isTpcLayerHit[iTpcLayer] = false;
        }
        break;
    }

    // determine which layers were hit
    unsigned int layer     = 0;
    unsigned int mvtxLayer = 0;
    unsigned int inttLayer = 0;
    unsigned int tpcLayer  = 0;
    for (auto itClustKey = (seed -> begin_cluster_keys()); itClustKey != (seed -> end_cluster_keys()); ++itClustKey) {

      // grab layer number
      layer = TrkrDefs::getLayer(*itClustKey);

      // increment accordingly
      switch (subsys) {
        case SUBSYS::MVTX:
          if (layer < CONST::NMvtxLayer) {
            mvtxLayer                 = layer;
            isMvtxLayerHit[mvtxLayer] = true;
          }
          break;
        case SUBSYS::INTT:
          if ((layer >= minInttLayer) && (layer < minTpcLayer)) {
            inttLayer                 = layer - minInttLayer;
            isInttLayerHit[inttLayer] = true;
          }
          break;
        case SUBSYS::TPC:
          if (layer >= minTpcLayer) {
            tpcLayer                = layer - minTpcLayer;
            isTpcLayerHit[tpcLayer] = true;
          }
          break;
      }
    }  // end cluster loop

    // get the relevant no. of layers
    int nLayer = 0;
    switch (subsys) {
      case SUBSYS::MVTX:
        for (int iMvtxLayer = 0; iMvtxLayer < CONST::NMvtxLayer; iMvtxLayer++) {
          if (isMvtxLayerHit[iMvtxLayer]) ++nLayer;
        }
        break;
      case SUBSYS::INTT:
        for (int iInttLayer = 0; iInttLayer < CONST::NInttLayer; iInttLayer++) {
          if (isInttLayerHit[iInttLayer]) ++nLayer;
        }
        break;
      case SUBSYS::TPC:
        for (int iTpcLayer = 0; iTpcLayer < CONST::NTpcLayer; iTpcLayer++) {
          if (isTpcLayerHit[iTpcLayer]) ++nLayer;
        }
        break;
    }
    return nLayer;

  }  // end 'GetNumLayer(SvtxTrack*, uint8_t)'



  int SCorrelatorJetTree::GetMatchID(SvtxTrack* track) {

    // print debug statement
    if (m_doDebug && (Verbosity() > 1)) {
      cout << "SCorrelatorJetTree::GetMatchID(SvtxTrack*) Grabbing barcode of matching particle..." << endl;
    }

    // get best match from truth particles
    PHG4Particle *bestMatch = m_trackEval -> max_truth_particle_by_nclusters(track);

    // grab barcode of best match
    int matchID;
    if (bestMatch) {
      matchID = bestMatch -> get_barcode();
    } else {
      matchID = -1;
    }
    return matchID;

  }  // end 'GetMatchID(SvtxTrack*)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

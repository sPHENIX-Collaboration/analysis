// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.jets.h'
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
using namespace fastjet;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // jet methods --------------------------------------------------------------

  void SCorrelatorJetTree::FindTrueJets(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::FindTrueJets(PHCompositeNode*) Finding truth (inclusive) jets..." << endl;
    }

    // define jets
    m_trueJetDef = new JetDefinition(m_jetAlgo, m_jetR, m_recombScheme, fastjet::Best);

    // declare constituent vector and jet map
    vector<fastjet::PseudoJet>     particles;
    map<int, pair<Jet::SRC, int>>  fjMapMC;

    // add constituents
    AddParticles(topNode, particles, fjMapMC);

    // cluster jets
    m_trueClust = new ClusterSequence(particles, *m_trueJetDef);
    m_trueJets  = m_trueClust -> inclusive_jets();
    return;

  }  // end 'FindTrueJets(PHCompositeNode*)'



  void SCorrelatorJetTree::FindRecoJets(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::FindRecoJets(PHCompositeNode*) Finding jets..." << endl;
    }

    // define jets
    m_recoJetDef = new JetDefinition(m_jetAlgo, m_jetR, m_recombScheme, fastjet::Best);

    // declare constituent vector and jet map
    vector<fastjet::PseudoJet>    particles;
    map<int, pair<Jet::SRC, int>> fjMap;

    // add constitutents
    if (m_addTracks) AddTracks(topNode, particles, fjMap);
    if (m_addFlow)   AddFlow(topNode,   particles, fjMap);
    if (m_addECal)   AddECal(topNode,   particles, fjMap);
    if (m_addHCal)   AddHCal(topNode,   particles, fjMap);

    // cluster jets
    m_recoClust = new ClusterSequence(particles, *m_recoJetDef);
    m_recoJets  = m_recoClust -> inclusive_jets();
    return;

  }  // end 'FindRecoJets(PHCompositeNode*)'



  void SCorrelatorJetTree::AddParticles(PHCompositeNode* topNode, vector<PseudoJet>& particles, map<int, pair<Jet::SRC, int>>& fjMap) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::AddParticles(PHComposite*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding MC particles..." << endl;
    }

    // loop over relevant subevents
    unsigned int iCst    = particles.size();
    unsigned int nParTot = 0;
    unsigned int nParAcc = 0;
    double       eParSum = 0.;
    for (const int evtToGrab : m_vecEvtsToGrab) {

      // grab subevent
      HepMC::GenEvent* mcEvt = GetMcEvent(topNode, evtToGrab);

      // grab embedding ID
      const int embedID = GetEmbedID(topNode, evtToGrab);

      // loop over particles in subevent
      for (HepMC::GenEvent::particle_const_iterator itPar = mcEvt -> particles_begin(); itPar != mcEvt -> particles_end(); ++itPar) {

        // check if particle is final state
        const bool isFinalState = ((*itPar) -> status() == 1);
        if (!isFinalState) {
          continue;
        } else {
          ++nParTot;
        }

        // check if particle is good
        const bool isGoodPar = IsGoodParticle(*itPar);
        if (!isGoodPar) {
          continue;
        } else {
          ++nParAcc;
        }

        // grab particle info
        const double parPx = (*itPar) -> momentum().px();
        const double parPy = (*itPar) -> momentum().py();
        const double parPz = (*itPar) -> momentum().pz();
        const double parE  = (*itPar) -> momentum().e();
        const int    parID = (*itPar) -> barcode();

        // map barcode onto relevant embeddingID
        m_mapCstToEmbedID[parID] = embedID;

        // create pseudojet & add to constituent vector
        fastjet::PseudoJet fjParticle(parPx, parPy, parPz, parE);
        fjParticle.set_user_index(parID);
        particles.push_back(fjParticle);

      
        pair<int, pair<Jet::SRC, int>> jetPartPair(iCst, make_pair(Jet::SRC::PARTICLE, parID));
        fjMap.insert(jetPartPair);

        // fill QA histograms, increment sums and counters
        m_hObjectQA[OBJECT::PART][INFO::PT]  -> Fill(fjParticle.perp());
        m_hObjectQA[OBJECT::PART][INFO::ETA] -> Fill(fjParticle.pseudorapidity());
        m_hObjectQA[OBJECT::PART][INFO::PHI] -> Fill(fjParticle.phi_std());
        m_hObjectQA[OBJECT::PART][INFO::ENE] -> Fill(fjParticle.E());
        eParSum += parE;
        ++iCst;
      }  // end particle loop
    }  // end subevent loop

    // fill QA histograms
    m_hNumObject[OBJECT::PART]             -> Fill(nParAcc);
    m_hNumCstAccept[CST_TYPE::PART_CST][0] -> Fill(nParTot);
    m_hNumCstAccept[CST_TYPE::PART_CST][1] -> Fill(nParAcc);
    m_hSumCstEne[CST_TYPE::PART_CST]       -> Fill(eParSum);
    return;

  }  // end 'AddParticles(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'



  void SCorrelatorJetTree::AddTracks(PHCompositeNode* topNode, vector<PseudoJet>& particles, map<int, pair<Jet::SRC, int>>& fjMap) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::AddTracks(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding tracks..." << endl;
    }

    // for weird tracks
    array<float, 40> arrWeirdTrackLeaves;
    for (size_t iLeaf = 0; iLeaf < 40; iLeaf++) {
      arrWeirdTrackLeaves[iLeaf] = 0.;
    }

    vector<TrkrDefs::cluskey> clustKeysA;
    vector<TrkrDefs::cluskey> clustKeysB;
    clustKeysA.clear();
    clustKeysB.clear();

    // loop over tracks
    unsigned int  iCst    = particles.size();
    unsigned int  nTrkTot = 0;
    unsigned int  nTrkAcc = 0;
    double        eTrkSum = 0.;
    SvtxTrack*    track   = NULL;
    SvtxTrack*    trackB  = NULL;
    SvtxTrackMap* mapTrks = GetTrackMap(topNode);
    for (SvtxTrackMap::Iter itTrk = mapTrks -> begin(); itTrk != mapTrks -> end(); ++itTrk) {

      // get track
      track = itTrk -> second;
      if (!track) {
        continue;
      } else {
        ++nTrkTot;
      }

      // check if good
      const bool isGoodTrack = IsGoodTrack(track, topNode);
      if (!isGoodTrack) {
        continue;
      } else {
        ++nTrkAcc;
      }

      // create pseudojet and add to constituent vector
      const int    trkID = track -> get_id();
      const double trkPx = track -> get_px();
      const double trkPy = track -> get_py();
      const double trkPz = track -> get_pz();
      const double trkE  = sqrt((trkPx * trkPx) + (trkPy * trkPy) + (trkPz * trkPz) + (0.140 * 0.140));  // FIXME move pion mass to constant in utilities namespace

      // grab barcode of matching particle
      int matchID;
      if (m_isMC) {
        matchID = GetMatchID(track);
      } else {
        matchID = -1;
      }


      fastjet::PseudoJet fjTrack(trkPx, trkPy, trkPz, trkE);
      fjTrack.set_user_index(matchID);
      particles.push_back(fjTrack);

      // add track to fastjet map
      pair<int, pair<Jet::SRC, int>> jetTrkPair(iCst, make_pair(Jet::SRC::TRACK, trkID));
      fjMap.insert(jetTrkPair);

      // grab track dca and vertex
      pair<double, double> trkDcaPair = GetTrackDcaPair(track, topNode);
      CLHEP::Hep3Vector    trkVtx     = GetTrackVertex(track, topNode);

      // grab remaining track info
      const double trkQuality = track -> get_quality();
      const double trkDeltaPt = GetTrackDeltaPt(track);
      const double trkDcaXY   = trkDcaPair.first;
      const double trkDcaZ    = trkDcaPair.second;
      const int    trkNumTpc  = GetNumLayer(track, SUBSYS::TPC);
      const int    trkNumIntt = GetNumLayer(track, SUBSYS::INTT);
      const int    trkNumMvtx = GetNumLayer(track, SUBSYS::MVTX);

      const int    trkClustTpc  = GetNumClust(track, SUBSYS::TPC);
      const int    trkClustIntt = GetNumClust(track, SUBSYS::INTT);
      const int    trkClustMvtx = GetNumClust(track, SUBSYS::MVTX);
      const double trkPhi       = track -> get_phi();
      const double trkEta       = track -> get_eta();
      const double trkPt        = track -> get_pt();
      if (m_checkWeirdTrks) {
        for (SvtxTrackMap::Iter itTrkB = mapTrks -> begin(); itTrkB != mapTrks -> end(); ++itTrkB) {

          // grab track B and skip if bad
          trackB = itTrkB -> second;
          if (!trackB) continue;

          const bool isGoodTrackB = IsGoodTrack(trackB, topNode);
          if (!isGoodTrackB) continue;

          // grab track B dca and vertex
          pair<double, double> trkDcaPairB = GetTrackDcaPair(trackB, topNode);
          CLHEP::Hep3Vector    trkVtxB     = GetTrackVertex(trackB, topNode);

          // get info from track B
          const int    trkIDB        = trackB -> get_id();
          const int    trkNumTpcB    = GetNumLayer(trackB, SUBSYS::TPC);
          const int    trkNumInttB   = GetNumLayer(trackB, SUBSYS::INTT);
          const int    trkNumMvtxB   = GetNumLayer(trackB, SUBSYS::MVTX);
          const int    trkClustTpcB  = GetNumClust(trackB, SUBSYS::TPC);
          const int    trkClustInttB = GetNumClust(trackB, SUBSYS::INTT);
          const int    trkClustMvtxB = GetNumClust(trackB, SUBSYS::MVTX);
          const double trkQualityB   = trackB -> get_quality();
          const double trkDeltaPtB   = GetTrackDeltaPt(trackB);
          const double trkDcaXYB     = trkDcaPairB.first;
          const double trkDcaZB      = trkDcaPairB.second;
          const double trkPxB        = trackB -> get_px();
          const double trkPyB        = trackB -> get_py();
          const double trkPzB        = trackB -> get_pz();
          const double trkEB         = sqrt((trkPxB * trkPxB) + (trkPyB * trkPyB) + (trkPzB * trkPzB) + (0.140 * 0.140));
          const double trkPhiB       = trackB -> get_phi();
          const double trkEtaB       = trackB -> get_eta();
          const double trkPtB        = trackB -> get_pt();

          // calculate delta-R
          const double dfTrkAB = trkPhi - trkPhiB;
          const double dhTrkAB = trkEta - trkEtaB;
          const double drTrkAB = sqrt((dfTrkAB * dfTrkAB) + (dhTrkAB * dhTrkAB));

          // skip if same as track A
          if (trkID == trkIDB) continue;

          // clear vectors for checking cluster keys
          clustKeysA.clear();
          clustKeysB.clear();

          // loop over clusters from track A
          auto seedTpcA = track -> get_tpc_seed();
          if (seedTpcA) {
            for (auto local_iterA = seedTpcA -> begin_cluster_keys(); local_iterA != seedTpcA -> end_cluster_keys(); ++local_iterA) {
              TrkrDefs::cluskey cluster_keyA = *local_iterA;
              clustKeysA.push_back(cluster_keyA);
            }
          }

          // loop over clusters from track B
          auto seedTpcB = trackB -> get_tpc_seed();
          if (seedTpcB) {
            for (auto local_iterB = seedTpcB -> begin_cluster_keys(); local_iterB != seedTpcB -> end_cluster_keys(); ++local_iterB) {
              TrkrDefs::cluskey cluster_keyB = *local_iterB;
              clustKeysB.push_back(cluster_keyB);
            }
          }

          // calculate no. of same cluster keys
          uint64_t nSameKey = 0;
          for (auto keyA : clustKeysA) {
            for (auto keyB : clustKeysB) {
              if (keyA == keyB) {
                ++nSameKey;
                break;
              }
            }  // end cluster key B loop
          }  // end cluster key A loop

          // set tuple leaves
          arrWeirdTrackLeaves[0]  = (float) trkID;
          arrWeirdTrackLeaves[1]  = (float) trkIDB;
          arrWeirdTrackLeaves[2]  = (float) trkPt;
          arrWeirdTrackLeaves[3]  = (float) trkPtB;
          arrWeirdTrackLeaves[4]  = (float) trkEta;
          arrWeirdTrackLeaves[5]  = (float) trkEtaB;
          arrWeirdTrackLeaves[6]  = (float) trkPhi;
          arrWeirdTrackLeaves[7]  = (float) trkPhiB;
          arrWeirdTrackLeaves[8]  = (float) trkE;
          arrWeirdTrackLeaves[9]  = (float) trkEB;
          arrWeirdTrackLeaves[10] = (float) trkDcaXY;
          arrWeirdTrackLeaves[11] = (float) trkDcaXYB;
          arrWeirdTrackLeaves[12] = (float) trkDcaZ;
          arrWeirdTrackLeaves[13] = (float) trkDcaZB;
          arrWeirdTrackLeaves[14] = (float) trkVtx.x();
          arrWeirdTrackLeaves[15] = (float) trkVtxB.x();
          arrWeirdTrackLeaves[16] = (float) trkVtx.y();
          arrWeirdTrackLeaves[17] = (float) trkVtxB.y();
          arrWeirdTrackLeaves[18] = (float) trkVtx.z();
          arrWeirdTrackLeaves[19] = (float) trkVtxB.z();
          arrWeirdTrackLeaves[20] = (float) trkQuality;
          arrWeirdTrackLeaves[21] = (float) trkQualityB;
          arrWeirdTrackLeaves[22] = (float) trkDeltaPt;
          arrWeirdTrackLeaves[23] = (float) trkDeltaPtB;
          arrWeirdTrackLeaves[24] = (float) trkNumMvtx;
          arrWeirdTrackLeaves[25] = (float) trkNumMvtxB;
          arrWeirdTrackLeaves[26] = (float) trkNumIntt;
          arrWeirdTrackLeaves[27] = (float) trkNumInttB;
          arrWeirdTrackLeaves[28] = (float) trkNumTpc;
          arrWeirdTrackLeaves[29] = (float) trkNumTpcB;
          arrWeirdTrackLeaves[30] = (float) trkClustMvtx;
          arrWeirdTrackLeaves[31] = (float) trkClustMvtxB;
          arrWeirdTrackLeaves[32] = (float) trkClustIntt;
          arrWeirdTrackLeaves[33] = (float) trkClustInttB;
          arrWeirdTrackLeaves[34] = (float) trkClustTpc;
          arrWeirdTrackLeaves[35] = (float) trkClustTpcB;
          arrWeirdTrackLeaves[36] = (float) clustKeysA.size();
          arrWeirdTrackLeaves[37] = (float) clustKeysB.size();
          arrWeirdTrackLeaves[38] = (float) nSameKey;
          arrWeirdTrackLeaves[39] = (float) drTrkAB;
 
          // fill weird track tuple
          m_ntWeirdTracks -> Fill(arrWeirdTrackLeaves.data());

        }  // end 2nd track loop
      }  // end if (m_checkWeirdTrks)

      // fill QA histograms
      m_hObjectQA[OBJECT::TRACK][INFO::PT]      -> Fill(fjTrack.perp());
      m_hObjectQA[OBJECT::TRACK][INFO::ETA]     -> Fill(fjTrack.pseudorapidity());
      m_hObjectQA[OBJECT::TRACK][INFO::PHI]     -> Fill(fjTrack.phi_std());
      m_hObjectQA[OBJECT::TRACK][INFO::ENE]     -> Fill(fjTrack.E());
      m_hObjectQA[OBJECT::TRACK][INFO::QUAL]    -> Fill(trkQuality);
      m_hObjectQA[OBJECT::TRACK][INFO::DCAXY]   -> Fill(trkDcaXY);
      m_hObjectQA[OBJECT::TRACK][INFO::DCAZ]    -> Fill(trkDcaZ);
      m_hObjectQA[OBJECT::TRACK][INFO::DELTAPT] -> Fill(trkDeltaPt);
      m_hObjectQA[OBJECT::TRACK][INFO::NTPC]    -> Fill(trkNumTpc);

      // fill QA tuple, increment sums and counters
      m_ntTrkQA -> Fill(
        (float) fjTrack.perp(),
        (float) fjTrack.pseudorapidity(),
        (float) fjTrack.phi_std(),
        (float) fjTrack.E(),
        (float) trkDcaXY,
        (float) trkDcaZ,
        (float) trkDeltaPt,
        (float) trkQuality,
        (float) trkNumMvtx,
        (float) trkNumIntt,
        (float) trkNumTpc,
        (float) trkVtx.x(),
        (float) trkVtx.y(),
        (float) trkVtx.z()
      );
      eTrkSum += trkE;
      ++iCst;

    }  // end track loop

    // fill QA histograms
    m_hNumObject[OBJECT::TRACK]             -> Fill(nTrkAcc);
    m_hNumCstAccept[CST_TYPE::TRACK_CST][0] -> Fill(nTrkTot);
    m_hNumCstAccept[CST_TYPE::TRACK_CST][1] -> Fill(nTrkAcc);
    m_hSumCstEne[CST_TYPE::TRACK_CST]       -> Fill(eTrkSum);
    return;

  }  // end 'AddTracks(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'



  void SCorrelatorJetTree::AddFlow(PHCompositeNode* topNode, vector<PseudoJet>& particles, map<int, pair<Jet::SRC, int>>& fjMap) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::AddFlow(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding particle flow elements..." << endl;
    }

    // warn if jets should be charged
    if (m_doDebug && (m_jetType != 1)) {
      cerr << "SCorrelatorJetTree::AddFlow - Warning - trying to add particle flow elements to charged jets!" << endl;
    }

    // loop over pf elements
    unsigned int                                iCst      = particles.size();
    unsigned int                                nFlowTot  = 0;
    unsigned int                                nFlowAcc  = 0;
    double                                      eFlowSum  = 0.;
    ParticleFlowElementContainer*               flowStore = GetFlowStore(topNode);
    ParticleFlowElementContainer::ConstRange    flowRange = flowStore -> getParticleFlowElements();
    ParticleFlowElementContainer::ConstIterator itFlow;
    for (itFlow = flowRange.first; itFlow != flowRange.second; ++itFlow) {

      // get pf element
      ParticleFlowElement* flow = itFlow -> second;
      if (!flow) {
        continue;
      } else {
       ++nFlowTot;
      }

      // check if good
      const bool isGoodFlow = IsGoodFlow(flow);
      if (!isGoodFlow) {
        continue;
      } else {
        ++nFlowAcc;
      }

      // create pseudojet and add to constituent vector
      const int    pfID = flow -> get_id();
      const double pfE  = flow -> get_e();
      const double pfPx = flow -> get_px();
      const double pfPy = flow -> get_py();
      const double pfPz = flow -> get_pz();

      fastjet::PseudoJet fjFlow(pfPx, pfPy, pfPz, pfE);
      fjFlow.set_user_index(iCst);
      particles.push_back(fjFlow);

      // add pf element to fastjet map
      pair<int, pair<Jet::SRC, int>> jetPartFlowPair(iCst, make_pair(Jet::SRC::PARTICLE, pfID));
      fjMap.insert(jetPartFlowPair);

      // fill QA histograms, increment sums and counters
      m_hObjectQA[OBJECT::FLOW][INFO::PT]  -> Fill(fjFlow.perp());
      m_hObjectQA[OBJECT::FLOW][INFO::ETA] -> Fill(fjFlow.pseudorapidity());
      m_hObjectQA[OBJECT::FLOW][INFO::PHI] -> Fill(fjFlow.phi_std());
      m_hObjectQA[OBJECT::FLOW][INFO::ENE] -> Fill(fjFlow.E());
      eFlowSum += pfE;
      ++iCst;
    }  // end pf element loop

    // fill QA histograms
    m_hNumObject[OBJECT::FLOW]             -> Fill(nFlowAcc);
    m_hNumCstAccept[CST_TYPE::FLOW_CST][0] -> Fill(nFlowTot);
    m_hNumCstAccept[CST_TYPE::FLOW_CST][1] -> Fill(nFlowAcc);
    m_hSumCstEne[CST_TYPE::FLOW_CST]       -> Fill(eFlowSum);
    return;

  }  // end 'AddFlow(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'



  void SCorrelatorJetTree::AddECal(PHCompositeNode* topNode, vector<PseudoJet>& particles, map<int, pair<Jet::SRC, int>>& fjMap) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::AddECal(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding ECal clusters..." << endl;
    }

    // warn if jets should be charged
    if (m_doDebug && (m_jetType != 1)) {
      cerr << "SCorrelatorJetTree::AddECal - Warning - trying to add calorimeter clusters to charged jets!" << endl;
    }

    // grab vertex and clusters
    GlobalVertex*        vtx          = GetGlobalVertex(topNode);
    RawClusterContainer* emClustStore = GetClusterStore(topNode, "CLUSTER_CEMC");

    // add emcal clusters if needed
    unsigned int iCst      = particles.size();
    unsigned int nClustTot = 0;
    unsigned int nClustAcc = 0;
    unsigned int nClustEM  = 0;
    double       eClustSum = 0.;

    // loop over em clusters
    RawClusterContainer::ConstRange    emClustRange = emClustStore -> getClusters();
    RawClusterContainer::ConstIterator itEMClust;
    for (itEMClust = emClustRange.first; itEMClust != emClustRange.second; ++itEMClust) {

      // grab cluster
      const RawCluster* emClust = itEMClust -> second;
      if (!emClust) {
        continue;
      } else {
        ++nClustTot;
      }

      // construct vertex and get 4-momentum
      const double vX = vtx -> get_x();
      const double vY = vtx -> get_y();
      const double vZ = vtx -> get_z();

      CLHEP::Hep3Vector hepVecVtx     = CLHEP::Hep3Vector(vX, vY, vZ);
      CLHEP::Hep3Vector hepVecEMClust = RawClusterUtility::GetECoreVec(*emClust, hepVecVtx);

      // check if good
      const bool isGoodECal = IsGoodECal(hepVecEMClust);
      if (!isGoodECal) {
        continue;
      } else {
        ++nClustAcc;
      }

      // create pseudojet and add to constituent vector
      const int    emClustID  = emClust -> get_id();
      const double emClustE   = hepVecEMClust.mag();
      const double emClustPt  = hepVecEMClust.perp();
      const double emClustPhi = hepVecEMClust.getPhi();
      const double emClustPx  = emClustPt * cos(emClustPhi);
      const double emClustPy  = emClustPt * sin(emClustPhi);
      const double emClustPz  = sqrt((emClustE * emClustE) - (emClustPx * emClustPx) - (emClustPy * emClustPy));
 
      fastjet::PseudoJet fjCluster(emClustPx, emClustPy, emClustPz, emClustE);
      fjCluster.set_user_index(iCst);
      particles.push_back(fjCluster);

      // add em cluster to fastjet map
      pair<int, pair<Jet::SRC, int>> jetEMClustPair(iCst, make_pair(Jet::SRC::CEMC_CLUSTER, emClustID));
      fjMap.insert(jetEMClustPair);

      // fill QA histograms, increment sums and counters
      m_hObjectQA[OBJECT::ECLUST][INFO::PT]  -> Fill(fjCluster.perp());
      m_hObjectQA[OBJECT::ECLUST][INFO::ETA] -> Fill(fjCluster.pseudorapidity());
      m_hObjectQA[OBJECT::ECLUST][INFO::PHI] -> Fill(fjCluster.phi_std());
      m_hObjectQA[OBJECT::ECLUST][INFO::ENE] -> Fill(fjCluster.E());
      eClustSum += emClustE;
      ++nClustEM;
      ++iCst;
    }  // end em cluster loop

    // fill QA histograms
    m_hNumObject[OBJECT::ECLUST]           -> Fill(nClustEM);
    m_hNumCstAccept[CST_TYPE::ECAL_CST][0] -> Fill(nClustTot);
    m_hNumCstAccept[CST_TYPE::ECAL_CST][1] -> Fill(nClustAcc);
    m_hSumCstEne[CST_TYPE::ECAL_CST]       -> Fill(eClustSum);
    return;

  }  // end 'AddECal(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'



  void SCorrelatorJetTree::AddHCal(PHCompositeNode* topNode, vector<PseudoJet>& particles, map<int, pair<Jet::SRC, int>>& fjMap) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::AddHCal(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding HCal clusters..." << endl;
    }

    // warn if jets should be charged
    if (m_doDebug && (m_jetType != 1)) {
      cerr << "SCorrelatorJetTree::AddHCal - Warning - trying to add calorimeter clusters to charged jets!" << endl;
    }

    // grab vertex and clusters
    GlobalVertex*        vtx          = GetGlobalVertex(topNode);
    RawClusterContainer* ihClustStore = GetClusterStore(topNode, "CLUSTER_HCALIN");
    RawClusterContainer* ohClustStore = GetClusterStore(topNode, "CLUSTER_HCALOUT");
 
    // add emcal clusters if needed
    unsigned int iCst      = particles.size();
    unsigned int nClustTot = 0;
    unsigned int nClustAcc = 0;
    unsigned int nClustH   = 0;
    double       eClustSum = 0.;

    // Loop over ih clusters
    RawClusterContainer::ConstRange    ihClustRange = ihClustStore -> getClusters();
    RawClusterContainer::ConstIterator itIHClust;
    for (itIHClust = ihClustRange.first; itIHClust != ihClustRange.second; ++itIHClust) {

      // get ih cluster
      const RawCluster* ihClust = itIHClust -> second;
      if (!ihClust) {
        continue;
      } else {
        ++nClustTot;
      }

      // construct vertex and get 4-momentum
      const double vX = vtx -> get_x();
      const double vY = vtx -> get_y();
      const double vZ = vtx -> get_z();

      CLHEP::Hep3Vector hepVecVtx     = CLHEP::Hep3Vector(vX, vY, vZ);
      CLHEP::Hep3Vector hepVecIHClust = RawClusterUtility::GetECoreVec(*ihClust, hepVecVtx);

      // check if good
      const bool isGoodHCal = IsGoodHCal(hepVecIHClust);
      if (!isGoodHCal) {
        continue;
      } else {
        ++nClustAcc;
      }

      // create pseudojet and add to constituent vector
      const int    ihClustID  = ihClust -> get_id();
      const double ihClustE   = hepVecIHClust.mag();
      const double ihClustPt  = hepVecIHClust.perp();
      const double ihClustPhi = hepVecIHClust.getPhi();
      const double ihClustPx  = ihClustPt * cos(ihClustPhi);
      const double ihClustPy  = ihClustPt * sin(ihClustPhi);
      const double ihClustPz  = sqrt((ihClustE * ihClustE) - (ihClustPx * ihClustPx) - (ihClustPy * ihClustPy));

      fastjet::PseudoJet fjCluster(ihClustPx, ihClustPy, ihClustPz, ihClustE);
      fjCluster.set_user_index(iCst);
      particles.push_back(fjCluster);

      // add ih cluster to fastjet map
      pair<int, pair<Jet::SRC, int>> jetIHClustPair(iCst, make_pair(Jet::SRC::HCALIN_CLUSTER, ihClustID));
      fjMap.insert(jetIHClustPair);

      // fill QA histograms, increment sums and counters
      m_hObjectQA[OBJECT::HCLUST][INFO::PT]  -> Fill(fjCluster.perp());
      m_hObjectQA[OBJECT::HCLUST][INFO::ETA] -> Fill(fjCluster.pseudorapidity());
      m_hObjectQA[OBJECT::HCLUST][INFO::PHI] -> Fill(fjCluster.phi_std());
      m_hObjectQA[OBJECT::HCLUST][INFO::ENE] -> Fill(fjCluster.E());
      eClustSum += ihClustE;
      ++nClustH;
      ++iCst;
    }  // end ih cluster loop

    // loop over oh clusters
    RawClusterContainer::ConstRange    ohClustRange = ohClustStore -> getClusters();
    RawClusterContainer::ConstIterator itOHClust;
    for (itOHClust = ohClustRange.first; itOHClust != ohClustRange.second; ++itOHClust) {

      // get oh cluster
      const RawCluster* ohClust = itOHClust -> second;
      if (!ohClust) {
        continue;
      } else {
        ++nClustTot;
      }

      // construct vertex and get 4-momentum
      const double vX = vtx -> get_x();
      const double vY = vtx -> get_y();
      const double vZ = vtx -> get_z();

      CLHEP::Hep3Vector hepVecVtx     = CLHEP::Hep3Vector(vX, vY, vZ);
      CLHEP::Hep3Vector hepVecOHClust = RawClusterUtility::GetECoreVec(*ohClust, hepVecVtx);

      // check if good
      const bool isGoodHCal = IsGoodHCal(hepVecOHClust);
      if (!isGoodHCal) {
        continue;
      } else {
        ++nClustAcc;
      }

      // create pseudojet and add to constituent vector
      const int    ohClustID  = ohClust -> get_id();
      const double ohClustE   = hepVecOHClust.mag();
      const double ohClustPt  = hepVecOHClust.perp();
      const double ohClustPhi = hepVecOHClust.getPhi();
      const double ohClustPx  = ohClustPt * cos(ohClustPhi);
      const double ohClustPy  = ohClustPt * sin(ohClustPhi);
      const double ohClustPz  = sqrt((ohClustE * ohClustE) - (ohClustPx * ohClustPx) - (ohClustPy * ohClustPy));

      fastjet::PseudoJet fjCluster(ohClustPx, ohClustPy, ohClustPz, ohClustE);
      fjCluster.set_user_index(iCst);
      particles.push_back(fjCluster);

      // add oh cluster to fastjet map
      pair<int, pair<Jet::SRC, int>> jetOHClustPair(iCst, make_pair(Jet::SRC::HCALOUT_CLUSTER, ohClustID));
      fjMap.insert(jetOHClustPair);

      // fill QA histograms, increment sums and counters
      m_hObjectQA[OBJECT::HCLUST][INFO::PT]  -> Fill(fjCluster.perp());
      m_hObjectQA[OBJECT::HCLUST][INFO::ETA] -> Fill(fjCluster.pseudorapidity());
      m_hObjectQA[OBJECT::HCLUST][INFO::PHI] -> Fill(fjCluster.phi_std());
      m_hObjectQA[OBJECT::HCLUST][INFO::ENE] -> Fill(fjCluster.E());
      eClustSum += ohClustE;
      ++nClustH;
      ++iCst;
    }  // end oh cluster loop

    // fill QA histograms
    m_hNumObject[OBJECT::HCLUST]           -> Fill(nClustH);
    m_hSumCstEne[CST_TYPE::HCAL_CST]       -> Fill(eClustSum);
    m_hNumCstAccept[CST_TYPE::HCAL_CST][0] -> Fill(nClustTot);
    m_hNumCstAccept[CST_TYPE::HCAL_CST][1] -> Fill(nClustAcc);
    return;

  }  // end 'AddHCal(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.jets.h'
// Derek Anderson
// 01.18.2023
//
// Class to construct a tree of
// jets from a specified set of
// events.
//
// Methods relevant to jets
// are collected here.
//
// Derived from code by Antonio
// Silva (thanks!!)
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace fastjet;
using namespace findNode;



// jet methods ----------------------------------------------------------------

void SCorrelatorJetTree::FindTrueJets(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::FindTrueJets(PHCompositeNode*) Finding truth jets..." << endl;
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



void SCorrelatorJetTree::FindRecoJets(PHCompositeNode *topNode) {

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



void SCorrelatorJetTree::AddParticles(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::AddParticles(PHComposite*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding MC particles..." << endl;
  }

  // loop over particles
  unsigned int     iCst    = particles.size();
  unsigned int     nParTot = 0;
  unsigned int     nParAcc = 0;
  double           eParSum = 0.;
  HepMC::GenEvent *mcEvt   = GetMcEvent(topNode);
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

    // create pseudojet & add to constituent vector
    const int    parID = (*itPar) -> barcode();
    const double parPx = (*itPar) -> momentum().px();
    const double parPy = (*itPar) -> momentum().py();
    const double parPz = (*itPar) -> momentum().pz();
    const double parE  = (*itPar) -> momentum().e();

    fastjet::PseudoJet fjParticle(parPx, parPy, parPz, parE);
    fjParticle.set_user_index(parID);
    particles.push_back(fjParticle);

    // add particle to mc fastjet map
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

  // fill QA histograms
  m_hNumObject[OBJECT::PART]             -> Fill(nParAcc);
  m_hNumCstAccept[CST_TYPE::PART_CST][0] -> Fill(nParTot);
  m_hNumCstAccept[CST_TYPE::PART_CST][1] -> Fill(nParAcc);
  m_hSumCstEne[CST_TYPE::PART_CST]       -> Fill(eParSum);
  return;

}  // end 'AddParticles(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'



void SCorrelatorJetTree::AddTracks(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::AddTracks(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding tracks..." << endl;
  }

  // loop over tracks
  unsigned int  iCst    = particles.size();
  unsigned int  nTrkTot = 0;
  unsigned int  nTrkAcc = 0;
  double        eTrkSum = 0.;
  SvtxTrack    *track   = 0x0;
  SvtxTrackMap *mapTrks = GetTrackMap(topNode);
  for (SvtxTrackMap::Iter itTrk = mapTrks -> begin(); itTrk != mapTrks -> end(); ++itTrk) {

    // get track
    track = itTrk -> second;
    if (!track) {
      continue;
    } else {
      ++nTrkTot;
    }

    // check if good
    const bool isGoodTrack = IsGoodTrack(track);
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
    const double trkE  = sqrt((trkPx * trkPx) + (trkPy * trkPy) + (trkPz * trkPz) + (MassPion * MassPion));

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

    // fill QA histograms, increment sums and counters
    m_hObjectQA[OBJECT::TRACK][INFO::PT]  -> Fill(fjTrack.perp());
    m_hObjectQA[OBJECT::TRACK][INFO::ETA] -> Fill(fjTrack.pseudorapidity());
    m_hObjectQA[OBJECT::TRACK][INFO::PHI] -> Fill(fjTrack.phi_std());
    m_hObjectQA[OBJECT::TRACK][INFO::ENE] -> Fill(fjTrack.E());
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



void SCorrelatorJetTree::AddFlow(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::AddFlow(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding particle flow elements..." << endl;
  }

  // warn if jets should be charged
  if (m_doDebug && (m_jetType != 1)) {
    cerr << "SCorrelatorJetTree::AddFlow - Warning - trying to add particle flow elements to charged jets!" << endl;
  }

  // loop over pf elements
  unsigned int                                iCst       = particles.size();
  unsigned int                                nFlowTot   = 0;
  unsigned int                                nFlowAcc   = 0;
  double                                      eFlowSum   = 0.;
  ParticleFlowElementContainer                *flowStore = GetFlowStore(topNode);
  ParticleFlowElementContainer::ConstRange    flowRange  = flowStore -> getParticleFlowElements();
  ParticleFlowElementContainer::ConstIterator itFlow;
  for (itFlow = flowRange.first; itFlow != flowRange.second; ++itFlow) {

    // get pf element
    ParticleFlowElement *flow = itFlow -> second;
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



void SCorrelatorJetTree::AddECal(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::AddECal(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding ECal clusters..." << endl;
  }

  // warn if jets should be charged
  if (m_doDebug && (m_jetType != 1)) {
    cerr << "SCorrelatorJetTree::AddECal - Warning - trying to add calorimeter clusters to charged jets!" << endl;
  }

  // grab vertex and clusters
  GlobalVertex        *vtx          = GetGlobalVertex(topNode);
  RawClusterContainer *emClustStore = GetClusterStore(topNode, "CLUSTER_CEMC");

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
    const RawCluster *emClust = itEMClust -> second;
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



void SCorrelatorJetTree::AddHCal(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::AddHCal(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding HCal clusters..." << endl;
  }

  // warn if jets should be charged
  if (m_doDebug && (m_jetType != 1)) {
    cerr << "SCorrelatorJetTree::AddHCal - Warning - trying to add calorimeter clusters to charged jets!" << endl;
  }

  // grab vertex and clusters
  GlobalVertex        *vtx          = GetGlobalVertex(topNode);
  RawClusterContainer *ihClustStore = GetClusterStore(topNode, "CLUSTER_HCALIN");
  RawClusterContainer *ohClustStore = GetClusterStore(topNode, "CLUSTER_HCALOUT");

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
    const RawCluster *ihClust = itIHClust -> second;
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
    const RawCluster *ohClust = itOHClust -> second;
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

// end ------------------------------------------------------------------------

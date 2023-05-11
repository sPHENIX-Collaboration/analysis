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
  if (m_addFlow)   AddFlow(topNode, particles, fjMap);
  if (m_addECal)   AddECal(topNode, particles, fjMap);
  if (m_addHCal)   AddHCal(topNode, particles, fjMap);

  // cluster jets
  m_recoClust = new ClusterSequence(particles, *m_recoJetDef);
  m_recoJets  = m_recoClust -> inclusive_jets();
  return;

}  // end 'FindRecoJets(PHCompositeNode*)'



void SCorrelatorJetTree::DoMatching() {

  /* jet/cst matching will go here */
  return;

}  // end 'DoMatching()'



void SCorrelatorJetTree::AddParticles(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::AddParticles(PHComposite*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding MC particles..." << endl;
  }

  // grab mc event map
  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!hepmceventmap) {
    cerr << PHWHERE
         << "PANIC: HEPMC event map node is missing, can't collect HEPMC truth particles!"
         << endl;
    return;
  }

  // grab mc event & check if good
  PHHepMCGenEvent *hepmcevent = hepmceventmap -> get(1);
  if (!hepmcevent) {
    cerr << PHWHERE
         << "PANIC: Couldn't grab HepMCEvent begin()! Abandoning particle collection!"
         << endl;
    return;
  }

  HepMC::GenEvent *hepMCevent = hepmcevent -> getEvent();
  if (!hepMCevent) {
    cerr << PHWHERE
         << "PANIC: Couldn't grab HepMC event! Abandoning particle collection!"
         << endl;
    return;
  }

  // loop over particles
  unsigned int iPart   = particles.size();
  unsigned int nParTot = 0;
  unsigned int nParAcc = 0;
  double       eParSum = 0.;
  for (HepMC::GenEvent::particle_const_iterator p = hepMCevent -> particles_begin(); p != hepMCevent -> particles_end(); ++p) {

    // check if particle is final state
    const bool isFinalState = ((*p) -> status() == 1);
    if (!isFinalState) {
      continue;
    } else {
      ++nParTot;
    }

    // check if particle is good
    const bool isGoodPar = IsGoodParticle(*p);
    if (!isGoodPar) {
      continue;
    } else {
      ++nParAcc;
    }

    // create pseudojet & add to constituent vector
    const int    parID = (*p) -> barcode();
    const double parPx = (*p) -> momentum().px();
    const double parPy = (*p) -> momentum().py();
    const double parPz = (*p) -> momentum().pz();
    const double parE  = (*p) -> momentum().e();

    fastjet::PseudoJet fjMCParticle(parPx, parPy, parPz, parE);
    fjMCParticle.set_user_index(iPart);
    particles.push_back(fjMCParticle);

    // add particle to mc fastjet map
    pair<int, pair<Jet::SRC, int>> jetPartPair(iPart, make_pair(Jet::SRC::PARTICLE, parID));
    fjMap.insert(jetPartPair);

    // fill QA histograms, increment sums and counters
    m_hObjectQA[OBJECT::PART][INFO::PT]  -> Fill(fjMCParticle.perp());
    m_hObjectQA[OBJECT::PART][INFO::ETA] -> Fill(fjMCParticle.pseudorapidity());
    m_hObjectQA[OBJECT::PART][INFO::PHI] -> Fill(fjMCParticle.phi_std());
    m_hObjectQA[OBJECT::PART][INFO::ENE] -> Fill(fjMCParticle.E());
    eParSum += parE;
    ++iPart;
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

  // get track map
  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!trackmap) {
    cerr << PHWHERE
         << "PANIC: SvtxTrackMap node is missing, can't collect tracks!"
         << endl;
    return;
  }

  // loop over tracks
  unsigned int  iPart   = particles.size();
  unsigned int  nTrkTot = 0;
  unsigned int  nTrkAcc = 0;
  double        eTrkSum = 0.;
  SvtxTrack    *track   = 0;
  for (SvtxTrackMap::Iter iter = trackmap -> begin(); iter != trackmap -> end(); ++iter) {

    // get track
    track = iter -> second;
    if (!track) {
      continue;
    } else {
      ++nTrkTot;
    }

    // check if good
    const bool isGoodTrack = IsGoodTrack(track);
    if(!isGoodTrack) {
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

    fastjet::PseudoJet fjTrack(trkPx, trkPy, trkPz, trkE);
    fjTrack.set_user_index(iPart);
    particles.push_back(fjTrack);

    // add track to fastjet map
    pair<int, pair<Jet::SRC, int>> jetTrkPair(iPart, make_pair(Jet::SRC::TRACK, trkID));
    fjMap.insert(jetTrkPair);

    // fill QA histograms, increment sums and counters
    m_hObjectQA[OBJECT::TRACK][INFO::PT]  -> Fill(fjTrack.perp());
    m_hObjectQA[OBJECT::TRACK][INFO::ETA] -> Fill(fjTrack.pseudorapidity());
    m_hObjectQA[OBJECT::TRACK][INFO::PHI] -> Fill(fjTrack.phi_std());
    m_hObjectQA[OBJECT::TRACK][INFO::ENE] -> Fill(fjTrack.E());
    eTrkSum += trkE;
    ++iPart;
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

  // declare pf  objects
  ParticleFlowElementContainer *pflowContainer = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");

  // loop over pf elements
  unsigned int                                iPart     = particles.size();
  unsigned int                                nFlowTot  = 0;
  unsigned int                                nFlowAcc  = 0;
  double                                      eFlowSum  = 0.;
  ParticleFlowElementContainer::ConstRange    begin_end = pflowContainer -> getParticleFlowElements();
  ParticleFlowElementContainer::ConstIterator rtiter;
  for (rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {

    // get pf element
    ParticleFlowElement *pflow = rtiter -> second;
    if (!pflow) {
      continue;
    } else {
      ++nFlowTot;
    }

    // check if good
    const bool isGoodFlow = IsGoodFlow(pflow);
    if (!isGoodFlow) {
      continue;
    } else {
      ++nFlowAcc;
    }

    // create pseudojet and add to constituent vector
    const int    pfID = pflow -> get_id();
    const double pfE  = pflow -> get_e();
    const double pfPx = pflow -> get_px();
    const double pfPy = pflow -> get_py();
    const double pfPz = pflow -> get_pz();

    fastjet::PseudoJet fjPartFlow(pfPx, pfPy, pfPz, pfE);
    fjPartFlow.set_user_index(iPart);
    particles.push_back(fjPartFlow);

    // add pf element to fastjet map
    pair<int, pair<Jet::SRC, int>> jetPartFlowPair(iPart, make_pair(Jet::SRC::PARTICLE, pfID));
    fjMap.insert(jetPartFlowPair);

    // fill QA histograms, increment sums and counters
    m_hObjectQA[OBJECT::FLOW][INFO::PT]  -> Fill(fjPartFlow.perp());
    m_hObjectQA[OBJECT::FLOW][INFO::ETA] -> Fill(fjPartFlow.pseudorapidity());
    m_hObjectQA[OBJECT::FLOW][INFO::PHI] -> Fill(fjPartFlow.phi_std());
    m_hObjectQA[OBJECT::FLOW][INFO::ENE] -> Fill(fjPartFlow.E());
    eFlowSum += pfE;
    ++iPart;
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

  // get vertex map
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap) {
    cerr << "SCorrelatorJetTree::AddECal - Fatal Error - GlobalVertexMap node is missing!\n"
         << "  Please turn on the do_global flag in the main macro in order to reconstruct the global vertex!"
         << endl;
    assert(vertexmap);
    return;
  }
  if (vertexmap -> empty()) {
    cerr << "SCorrelatorJetTree::AddECal - Fatal Error - GlobalVertexMap node is empty!\n"
         << "  Please turn on the do_global flag in the main macro in order to reconstruct the global vertex!"
         << endl;
    return;
  }

  // grab vertex
  GlobalVertex *vtx = vertexmap -> begin() -> second;
  if (vtx == nullptr) return;

  // add emcal clusters if needed
  unsigned int iPart     = particles.size();
  unsigned int nClustTot = 0;
  unsigned int nClustAcc = 0;
  unsigned int nClustEM  = 0;
  double       eClustSum = 0.;

  // grab em cluster containter
  RawClusterContainer *clustersEMC = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if (!clustersEMC) {
    cout << PHWHERE
         << "PANIC: EMCal cluster node is missing, can't collect EMCal clusters!"
         << endl;
    return;
  }

  // loop over em clusters
  RawClusterContainer::ConstRange    begin_end_EMC = clustersEMC -> getClusters();
  RawClusterContainer::ConstIterator clusIter_EMC;
  for (clusIter_EMC = begin_end_EMC.first; clusIter_EMC != begin_end_EMC.second; ++clusIter_EMC) {

    // grab cluster
    const RawCluster *cluster = clusIter_EMC -> second;
    if (!cluster) {
      continue;
    } else {
      ++nClustTot;
    }

    // construct vertex and get 4-momentum
    const double vX = vtx -> get_x();
    const double vY = vtx -> get_y();
    const double vZ = vtx -> get_z();

    CLHEP::Hep3Vector vertex(vX, vY, vZ);
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

    // check if good
    const bool isGoodECal = IsGoodECal(E_vec_cluster);
    if (!isGoodECal) {
      continue;
    } else {
      ++nClustAcc;
    }

    // create pseudojet and add to constituent vector
    const int    emClustID  = cluster -> get_id();
    const double emClustE   = E_vec_cluster.mag();
    const double emClustPt  = E_vec_cluster.perp();
    const double emClustPhi = E_vec_cluster.getPhi();
    const double emClustPx  = emClustPt * cos(emClustPhi);
    const double emClustPy  = emClustPt * sin(emClustPhi);
    const double emClustPz  = sqrt((emClustE * emClustE) - (emClustPx * emClustPx) - (emClustPy * emClustPy));

    fastjet::PseudoJet fjCluster(emClustPx, emClustPy, emClustPz, emClustE);
    fjCluster.set_user_index(iPart);
    particles.push_back(fjCluster);

    // add em cluster to fastjet map
    pair<int, pair<Jet::SRC, int>> jetEMClustPair(iPart, make_pair(Jet::SRC::CEMC_CLUSTER, emClustID));
    fjMap.insert(jetEMClustPair);

    // fill QA histograms, increment sums and counters
    m_hObjectQA[OBJECT::ECLUST][INFO::PT]  -> Fill(fjCluster.perp());
    m_hObjectQA[OBJECT::ECLUST][INFO::ETA] -> Fill(fjCluster.pseudorapidity());
    m_hObjectQA[OBJECT::ECLUST][INFO::PHI] -> Fill(fjCluster.phi_std());
    m_hObjectQA[OBJECT::ECLUST][INFO::ENE] -> Fill(fjCluster.E());
    eClustSum += emClustE;
    ++nClustEM;
    ++iPart;
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

  // get vertex map
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap) {
    cerr << "SCorrelatorJetTree::AddHCal - Fatal Error - GlobalVertexMap node is missing!\n"
         << "  Please turn on the do_global flag in the main macro in order to reconstruct the global vertex!"
         << endl;
    assert(vertexmap);
    return;
  }
  if (vertexmap -> empty()) {
    cerr << "SCorrelatorJetTree::AddHCal - Fatal Error - GlobalVertexMap node is empty!\n"
         << "  Please turn on the do_global flag in the main macro in order to reconstruct the global vertex!"
         << endl;
    return;
  }

  // grab vertex
  GlobalVertex *vtx = vertexmap -> begin() -> second;
  if (vtx == nullptr) return;

  // add emcal clusters if needed
  unsigned int iPart     = particles.size();
  unsigned int nClustTot = 0;
  unsigned int nClustAcc = 0;
  unsigned int nClustH   = 0;
  double       eClustSum = 0.;

  // grab ih cluster container
  RawClusterContainer *clustersHCALIN = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_HCALIN");
  if (!clustersHCALIN) {
    cerr << PHWHERE
         << "PANIC: Inner HCal cluster node is missing, can't collect inner HCal clusters!"
         << endl;
    return;
  }

  // Loop over ih clusters
  RawClusterContainer::ConstRange    begin_end_HCALIN = clustersHCALIN -> getClusters();
  RawClusterContainer::ConstIterator clusIter_HCALIN;
  for (clusIter_HCALIN = begin_end_HCALIN.first; clusIter_HCALIN != begin_end_HCALIN.second; ++clusIter_HCALIN) {

    // get ih cluster
    const RawCluster *cluster = clusIter_HCALIN -> second;
    if (!cluster) {
      continue;
    } else {
      ++nClustTot;
    }

    // construct vertex and get 4-momentum
    const double vX = vtx -> get_x();
    const double vY = vtx -> get_y();
    const double vZ = vtx -> get_z();

    CLHEP::Hep3Vector vertex(vX, vY, vZ);
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

    // check if good
    const bool isGoodHCal = IsGoodHCal(E_vec_cluster);
    if (!isGoodHCal) {
      continue;
    } else {
      ++nClustAcc;
    }

    // create pseudojet and add to constituent vector
    const int    ihClustID  = cluster -> get_id();
    const double ihClustE   = E_vec_cluster.mag();
    const double ihClustPt  = E_vec_cluster.perp();
    const double ihClustPhi = E_vec_cluster.getPhi();
    const double ihClustPx  = ihClustPt * cos(ihClustPhi);
    const double ihClustPy  = ihClustPt * sin(ihClustPhi);
    const double ihClustPz  = sqrt((ihClustE * ihClustE) - (ihClustPx * ihClustPx) - (ihClustPy * ihClustPy));

    fastjet::PseudoJet fjCluster(ihClustPx, ihClustPy, ihClustPz, ihClustE);
    fjCluster.set_user_index(iPart);
    particles.push_back(fjCluster);

    // add ih cluster to fastjet map
    pair<int, pair<Jet::SRC, int>> jetIHClustPair(iPart, make_pair(Jet::SRC::HCALIN_CLUSTER, ihClustID));
    fjMap.insert(jetIHClustPair);

    // fill QA histograms, increment sums and counters
    m_hObjectQA[OBJECT::HCLUST][INFO::PT]  -> Fill(fjCluster.perp());
    m_hObjectQA[OBJECT::HCLUST][INFO::ETA] -> Fill(fjCluster.pseudorapidity());
    m_hObjectQA[OBJECT::HCLUST][INFO::PHI] -> Fill(fjCluster.phi_std());
    m_hObjectQA[OBJECT::HCLUST][INFO::ENE] -> Fill(fjCluster.E());
    eClustSum += ihClustE;
    ++nClustH;
    ++iPart;
  }  // end ih cluster loop

  // grab oh cluster container
  RawClusterContainer *clustersHCALOUT = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_HCALOUT");
  if (!clustersHCALOUT) {
    cerr << PHWHERE
         << "PANIC: Outer HCal cluster node is missing, can't collect outer HCal clusters!"
         << endl;
    return;
  }

  // loop over oh clusters
  RawClusterContainer::ConstRange    begin_end_HCALOUT = clustersHCALOUT -> getClusters();
  RawClusterContainer::ConstIterator clusIter_HCALOUT;
  for (clusIter_HCALOUT = begin_end_HCALOUT.first; clusIter_HCALOUT != begin_end_HCALOUT.second; ++clusIter_HCALOUT) {

    // get oh cluster
    const RawCluster *cluster = clusIter_HCALOUT -> second;
    if (!cluster) {
      continue;
    } else {
      ++nClustTot;
    }

    // construct vertex and get 4-momentum
    const double vX = vtx -> get_x();
    const double vY = vtx -> get_y();
    const double vZ = vtx -> get_z();

    CLHEP::Hep3Vector vertex(vX, vY, vZ);
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

    // check if good
    const bool isGoodHCal = IsGoodHCal(E_vec_cluster);
    if (!isGoodHCal) {
      continue;
    } else {
      ++nClustAcc;
    }

    // create pseudojet and add to constituent vector
    const int    ohClustID  = cluster -> get_id();
    const double ohClustE   = E_vec_cluster.mag();
    const double ohClustPt  = E_vec_cluster.perp();
    const double ohClustPhi = E_vec_cluster.getPhi();
    const double ohClustPx  = ohClustPt * cos(ohClustPhi);
    const double ohClustPy  = ohClustPt * sin(ohClustPhi);
    const double ohClustPz  = sqrt((ohClustE * ohClustE) - (ohClustPx * ohClustPx) - (ohClustPy * ohClustPy));

    fastjet::PseudoJet fjCluster(ohClustPx, ohClustPy, ohClustPz, ohClustE);
    fjCluster.set_user_index(iPart);
    particles.push_back(fjCluster);

    // add oh cluster to fastjet map
    pair<int, pair<Jet::SRC, int>> jetOHClustPair(iPart, make_pair(Jet::SRC::HCALOUT_CLUSTER, ohClustID));
    fjMap.insert(jetOHClustPair);

    // fill QA histograms, increment sums and counters
    m_hObjectQA[OBJECT::HCLUST][INFO::PT]  -> Fill(fjCluster.perp());
    m_hObjectQA[OBJECT::HCLUST][INFO::ETA] -> Fill(fjCluster.pseudorapidity());
    m_hObjectQA[OBJECT::HCLUST][INFO::PHI] -> Fill(fjCluster.phi_std());
    m_hObjectQA[OBJECT::HCLUST][INFO::ENE] -> Fill(fjCluster.E());
    eClustSum += ohClustE;
    ++nClustH;
    ++iPart;
  }  // end oh cluster loop

  // fill QA histograms
  m_hNumObject[OBJECT::HCLUST]           -> Fill(nClustH);
  m_hSumCstEne[CST_TYPE::HCAL_CST]       -> Fill(eClustSum);
  m_hNumCstAccept[CST_TYPE::HCAL_CST][0] -> Fill(nClustTot);
  m_hNumCstAccept[CST_TYPE::HCAL_CST][1] -> Fill(nClustAcc);
  return;

}  // end 'AddHCal(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'



bool SCorrelatorJetTree::IsJetGoodMatch(const double qtJet, const double drJet) {

  // print debug statement
  if (m_doDebug && (Verbosity() > 2)) {
    cout << "SCorrelatorJetTree::IsJetGoodMatch(double, double) Checking if jet match is good..." << endl;
  }

  const bool isInQtRange = ((qtJet > m_jetMatchQtRange[0]) && (qtJet < m_jetMatchQtRange[1]));
  const bool isInDrRange = ((drJet > m_jetMatchDrRange[0]) && (drJet < m_jetMatchDrRange[1]));
  const bool isGoodMatch = (isInQtRange && isInDrRange);
  return isGoodMatch;

}  // end 'IsJetGoodMatch(double, double)'

// end ------------------------------------------------------------------------

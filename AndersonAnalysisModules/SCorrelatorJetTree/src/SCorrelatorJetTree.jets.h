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

#pragma once

using namespace std;
using namespace fastjet;
using namespace findNode;



void SCorrelatorJetTree::findJets(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::findJets(PHCompositeNode *topNode) Finding jets..." << endl;
  }

  // declare fastjet objects
  fastjet::JetDefinition     *jetdef = new fastjet::JetDefinition(m_jetalgo, m_jetr, m_recomb_scheme, fastjet::Best);
  vector<fastjet::PseudoJet>  particles;

  // instantiate fastjet map
  map<int, pair<Jet::SRC, int>> fjMap;

  // add constitutents
  const bool doParticleFlow = m_add_particleflow;
  const bool doTracks       = m_add_tracks;
  const bool doCaloClusters = (m_add_EMCal_clusters || m_add_HCal_clusters);
  if (doParticleFlow) addParticleFlow(topNode, particles, fjMap);
  if (doTracks)       addTracks(topNode, particles, fjMap);
  if (doCaloClusters) addClusters(topNode, particles, fjMap);

  // cluster jets
  fastjet::ClusterSequence   jetFinder(particles, *jetdef);
  vector<fastjet::PseudoJet> fastjets = jetFinder.inclusive_jets();
  delete jetdef;

  // prepare vectors for filling
  m_recJetNCst.clear();
  m_recJetId.clear();
  m_recJetTruId.clear();
  m_recJetE.clear();
  m_recJetPt.clear();
  m_recJetEta.clear();
  m_recJetPhi.clear();
  m_recCstZ.clear();
  m_recCstDr.clear();
  m_recCstE.clear();
  m_recCstJt.clear();
  m_recCstEta.clear();
  m_recCstPhi.clear();

  // declare vectors for storing constituents
  vector<double> vecRecCstZ;
  vector<double> vecRecCstDr;
  vector<double> vecRecCstE;
  vector<double> vecRecCstJt;
  vector<double> vecRecCstEta;
  vector<double> vecRecCstPhi;
  vecRecCstZ.clear();
  vecRecCstDr.clear();
  vecRecCstE.clear();
  vecRecCstJt.clear();
  vecRecCstEta.clear();
  vecRecCstPhi.clear();

  // fill jet/constituent variables
  unsigned long nJet(0);
  for (unsigned int iJet = 0; iJet < fastjets.size(); ++iJet) {

    // get jet info
    const unsigned int jetNCst  = fastjets[iJet].constituents().size();
    const unsigned int jetID    = iJet;
    const unsigned int jetTruID = 99999;  // FIXME: this will need to be changed to the matched truth jet
    const double       jetPhi   = fastjets[iJet].phi_std();
    const double       jetEta   = fastjets[iJet].pseudorapidity();
    const double       jetE     = fastjets[iJet].E();
    const double       jetPt    = fastjets[iJet].perp();
    const double       jetPx    = fastjets[iJet].px();
    const double       jetPy    = fastjets[iJet].py();
    const double       jetPz    = fastjets[iJet].pz();
    const double       jetP     = sqrt((jetPx * jetPx) + (jetPy * jetPy) + (jetPz * jetPz));

    // clear constituent vectors
    vecRecCstZ.clear();
    vecRecCstDr.clear();
    vecRecCstE.clear();
    vecRecCstJt.clear();
    vecRecCstEta.clear();
    vecRecCstPhi.clear();

    // loop over constituents
    vector<fastjet::PseudoJet> csts = fastjets[iJet].constituents();
    for (unsigned int iCst = 0; iCst < csts.size(); ++iCst) {

      // get constituent info
      const double cstPhi = csts[iCst].phi_std();
      const double cstEta = csts[iCst].pseudorapidity();
      const double cstE   = csts[iCst].E();
      const double cstJt  = csts[iCst].perp();
      const double cstJx  = csts[iCst].px();
      const double cstJy  = csts[iCst].py();
      const double cstJz  = csts[iCst].pz();
      const double cstJ   = ((cstJx * cstJx) + (cstJy * cstJy) + (cstJz * cstJz));
      const double cstZ   = cstJ / jetP;
      const double cstDf  = cstPhi - jetPhi;
      const double cstDh  = cstEta - jetEta;
      const double cstDr  = sqrt((cstDf * cstDf) + (cstDh * cstDh));

      // add csts to vectors
      vecRecCstZ.push_back(cstZ);
      vecRecCstDr.push_back(cstDr);
      vecRecCstE.push_back(cstE);
      vecRecCstJt.push_back(cstJt);
      vecRecCstEta.push_back(cstEta);
      vecRecCstPhi.push_back(cstPhi);
    }  // end constituent loop

    // store jet/cst output
    m_recJetNCst.push_back(jetNCst);
    m_recJetId.push_back(jetID);
    m_recJetTruId.push_back(jetTruID);
    m_recJetE.push_back(jetE);
    m_recJetPt.push_back(jetPt);
    m_recJetEta.push_back(jetEta);
    m_recJetPhi.push_back(jetPhi);
    m_recCstZ.push_back(vecRecCstZ);
    m_recCstDr.push_back(vecRecCstDr);
    m_recCstE.push_back(vecRecCstE);
    m_recCstJt.push_back(vecRecCstJt);
    m_recCstEta.push_back(vecRecCstEta);
    m_recCstPhi.push_back(vecRecCstPhi);
    ++nJet;
  }  // end jet loop

  // store evt info
  // FIXME: replace parton branches w/ relevant info
  m_recNumJets       = nJet;
  m_recPartonID[0]   = -9999;
  m_recPartonID[1]   = -9999;
  m_recPartonMomX[0] = -9999.;
  m_recPartonMomX[1] = -9999.;
  m_recPartonMomY[0] = -9999.;
  m_recPartonMomY[1] = -9999.;
  m_recPartonMomZ[0] = -9999.;
  m_recPartonMomZ[1] = -9999.;

  // fill tree
  m_recTree -> Fill();
  return;

}  // end 'findJets(PHCompositeNode*)'



void SCorrelatorJetTree::findMcJets(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug || (Verbosity() > 6)) {
    cout << "SCorrelatorJetTree::findMcJets(PHCompositeNode *topNode) Finding MC jets..." << endl;
  }

  // declare fastjet objects & mc fastjet map
  fastjet::JetDefinition        *jetdef = new fastjet::JetDefinition(m_jetalgo, m_jetr, m_recomb_scheme, fastjet::Best);
  vector<fastjet::PseudoJet>     particles;
  map<int, pair<Jet::SRC, int>>  fjMapMC;

  // add constituents
  addParticles(topNode, particles, fjMapMC);

  // cluster jets
  fastjet::ClusterSequence   jetFinder(particles, *jetdef);
  vector<fastjet::PseudoJet> mcfastjets = jetFinder.inclusive_jets();
  delete jetdef;

  // prepare vectors for filling
  m_truJetNCst.clear();
  m_truJetId.clear();
  m_truJetTruId.clear();
  m_truJetE.clear();
  m_truJetPt.clear();
  m_truJetEta.clear();
  m_truJetPhi.clear();
  m_truCstZ.clear();
  m_truCstDr.clear();
  m_truCstE.clear();
  m_truCstJt.clear();
  m_truCstEta.clear();
  m_truCstPhi.clear();

  // declare vectors to storing constituents
  vector<double> vecTruCstZ;
  vector<double> vecTruCstDr;
  vector<double> vecTruCstE;
  vector<double> vecTruCstJt;
  vector<double> vecTruCstEta;
  vector<double> vecTruCstPhi;
  vecTruCstZ.clear();
  vecTruCstDr.clear();
  vecTruCstE.clear();
  vecTruCstJt.clear();
  vecTruCstEta.clear();
  vecTruCstPhi.clear();

  // fill jets/constituent variables
  unsigned int nTruJet(0);
  for (unsigned int iTruJet = 0; iTruJet < mcfastjets.size(); ++iTruJet) {

    // get jet info
    const unsigned int jetNCst  = mcfastjets[iTruJet].constituents().size();
    const unsigned int jetID    = 9999.;  // FIXME: this will need to be changed to the matched reco jet
    const unsigned int jetTruID = iTruJet;
    const double       jetPhi   = mcfastjets[iTruJet].phi_std();
    const double       jetEta   = mcfastjets[iTruJet].pseudorapidity();
    const double       jetE     = mcfastjets[iTruJet].E();
    const double       jetPt    = mcfastjets[iTruJet].perp();
    const double       jetPx    = mcfastjets[iTruJet].px();
    const double       jetPy    = mcfastjets[iTruJet].py();
    const double       jetPz    = mcfastjets[iTruJet].pz();
    const double       jetP     = sqrt((jetPx * jetPx) + (jetPy * jetPy) + (jetPz * jetPz));

    // clear constituent vectors
    vecTruCstZ.clear();
    vecTruCstDr.clear();
    vecTruCstE.clear();
    vecTruCstJt.clear();
    vecTruCstEta.clear();
    vecTruCstPhi.clear();

    // loop over constituents
    vector<fastjet::PseudoJet> truCsts = mcfastjets[iTruJet].constituents();
    for (unsigned int iTruCst = 0; iTruCst < truCsts.size(); ++iTruCst) {

      // get constituent info
      const double cstPhi = truCsts[iTruCst].phi_std();
      const double cstEta = truCsts[iTruCst].pseudorapidity();
      const double cstE   = truCsts[iTruCst].E();
      const double cstJt  = truCsts[iTruCst].perp();
      const double cstJx  = truCsts[iTruCst].px();
      const double cstJy  = truCsts[iTruCst].py();
      const double cstJz  = truCsts[iTruCst].pz();
      const double cstJ   = ((cstJx * cstJx) + (cstJy * cstJy) + (cstJz * cstJz));
      const double cstZ   = cstJ / jetP;
      const double cstDf  = cstPhi - jetPhi;
      const double cstDh  = cstEta - jetEta;
      const double cstDr  = sqrt((cstDf * cstDf) + (cstDh * cstDh));

      // add csts to vectors
      vecTruCstZ.push_back(cstZ);
      vecTruCstDr.push_back(cstDr);
      vecTruCstE.push_back(cstE);
      vecTruCstJt.push_back(cstJt);
      vecTruCstEta.push_back(cstEta);
      vecTruCstPhi.push_back(cstPhi);
    }  // end constituent loop

    // store jet/cst output
    m_truJetNCst.push_back(jetNCst);
    m_truJetId.push_back(jetID);
    m_truJetTruId.push_back(jetTruID);
    m_truJetE.push_back(jetE);
    m_truJetPt.push_back(jetPt);
    m_truJetEta.push_back(jetEta);
    m_truJetPhi.push_back(jetPhi);
    m_truCstZ.push_back(vecTruCstZ);
    m_truCstDr.push_back(vecTruCstDr);
    m_truCstE.push_back(vecTruCstE);
    m_truCstJt.push_back(vecTruCstJt);
    m_truCstEta.push_back(vecTruCstEta);
    m_truCstPhi.push_back(vecTruCstPhi);
    ++nTruJet;
  }  // end jet loop

  // store evt info
  // FIXME: grab actual parton values
  m_truNumJets       = nTruJet;
  m_truPartonID[0]   = -9999;
  m_truPartonID[1]   = -9999;
  m_truPartonMomX[0] = -9999.;
  m_truPartonMomX[1] = -9999.;
  m_truPartonMomY[0] = -9999.;
  m_truPartonMomY[1] = -9999.;
  m_truPartonMomZ[0] = -9999.;
  m_truPartonMomZ[1] = -9999.; 

  // fill tree
  m_truTree -> Fill();
  return;

}  // end 'findMcJets(PHCompositeNode*)'



void SCorrelatorJetTree::addParticleFlow(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::addParticleFlow(PHCompositeNode *topNode, vector<PseudoJet>&, map<int, parir<Jet::SRC, int>>&) Adding particle flow elements..." << endl;
  }

  // declare pf  objects
  ParticleFlowElementContainer *pflowContainer = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");

  // loop over pf elements
  unsigned int                                iPart     = particles.size();
  ParticleFlowElementContainer::ConstRange    begin_end = pflowContainer -> getParticleFlowElements();
  ParticleFlowElementContainer::ConstIterator rtiter;
  for (rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {

    // get pf element
    ParticleFlowElement *pflow = rtiter -> second;

    // check if good
    const bool isGoodElement = isAcceptableParticleFlow(pflow);
    if(!pflow || !isGoodElement) continue;

    // create pseudojet and add to constituent vector
    const int    pfID = pflow -> get_id();
    const double pfPx = pflow -> get_px();
    const double pfPy = pflow -> get_py();
    const double pfPz = pflow -> get_pz();
    const double pfE  = pflow -> get_e();

    fastjet::PseudoJet fjPartFlow(pfPx, pfPy, pfPz, pfE);
    fjPartFlow.set_user_index(iPart);
    particles.push_back(fjPartFlow);

    // add pf element to fastjet map
    pair<int, pair<Jet::SRC, int>> jetPartFlowPair(iPart, make_pair(Jet::SRC::PARTICLE, pfID));
    fjMap.insert(jetPartFlowPair);
    ++iPart;
  }  // end pf element loop
  return;

}  // end 'addParticleFlow(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'



void SCorrelatorJetTree::addTracks(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::addTracks(PHCompositeNode *topNode, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding tracks..." << endl;
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
  unsigned int  iPart = particles.size();
  SvtxTrack    *track = 0;
  for (SvtxTrackMap::Iter iter = trackmap -> begin(); iter != trackmap -> end(); ++iter) {

    // get track
    track = iter -> second;

    // check if good
    const bool isGoodTrack = isAcceptableTrack(track);
    if(!isGoodTrack) continue;

    // create pseudojet and add to constituent vector
    const int    trkID = track -> get_id();
    const double trkPx = track -> get_px();
    const double trkPy = track -> get_py();
    const double trkPz = track -> get_pz();

    fastjet::PseudoJet fjTrack(trkPx, trkPy, trkPz, 0.);  // FIXME: add track energy? (e.g. maybe assume a pion mass...)
    fjTrack.set_user_index(iPart);
    particles.push_back(fjTrack);

    // add track to fastjet map
    pair<int, pair<Jet::SRC, int>> jetTrkPair(iPart, make_pair(Jet::SRC::TRACK, trkID));
    fjMap.insert(jetTrkPair);
    ++iPart;
  }  // end track loop
  return;

}  // end 'addTracks(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'



void SCorrelatorJetTree::addClusters(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::addClusters(PHCompositeNode *topNode, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding clusters..." << endl;
  }

  // get vertex map
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap) {
    cerr << "SCorrelatorJetTree::getEmcalClusters - Fatal Error - GlobalVertexMap node is missing!\n"
         << "  Please turn on the do_global flag in the main macro in order to reconstruct the global vertex!"
         << endl;
    assert(vertexmap);
    return;
  }
  if (vertexmap -> empty()) {
    cerr << "JetTagging::getEmcalClusters - Fatal Error - GlobalVertexMap node is empty!\n"
         << "  Please turn on the do_global flag in the main macro in order to reconstruct the global vertex!"
         << endl;
    return;
  }

  // grab vertex
  GlobalVertex *vtx = vertexmap -> begin() -> second;
  if (vtx == nullptr) return;

  // add emcal clusters if needed
  int iPart = particles.size();
  if (m_add_EMCal_clusters) {

    // grab em cluster containter
    RawClusterContainer *clustersEMC = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
    if (!clustersEMC) {
      cout << PHWHERE
           << "PANIC: EMCal cluster node is missing, can't collect EMCal clusters!"
           << endl;
      return;
    }

    // loop over em clusters
    RawClusterContainer::ConstRange    begin_end_EMC = clustersEMC->getClusters();
    RawClusterContainer::ConstIterator clusIter_EMC;
    for (clusIter_EMC = begin_end_EMC.first; clusIter_EMC != begin_end_EMC.second; ++clusIter_EMC) {

      // grab cluster
      const RawCluster *cluster = clusIter_EMC -> second;

      // construct vertex and get 4-momentum
      const double vX = vtx -> get_x();
      const double vY = vtx -> get_y();
      const double vZ = vtx -> get_z();

      CLHEP::Hep3Vector vertex(vX, vY, vZ);
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

      // check if good
      const bool isGoodClust = isAcceptableEMCalCluster(E_vec_cluster);
      if(!isGoodClust) continue;

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
      ++iPart;
    }  // end em cluster loop
  }  // end if (m_add_EMCal_clusters)

  //  add hcal clusters if needed
  if (m_add_HCal_clusters) {

    // grab ih cluster container
    RawClusterContainer *clustersHCALIN = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_HCALIN");
    if (!clustersHCALIN) {
      cerr << PHWHERE
           << "PANIC: Inner HCal cluster node is missing, can't collect inner HCal clusters!"
           << endl;
      return;
    }

    // Loop over ih clusters
    RawClusterContainer::ConstRange    begin_end_HCALIN = clustersHCALIN->getClusters();
    RawClusterContainer::ConstIterator clusIter_HCALIN;
    for (clusIter_HCALIN = begin_end_HCALIN.first; clusIter_HCALIN != begin_end_HCALIN.second; ++clusIter_HCALIN) {

      // get ih cluster
      const RawCluster *cluster = clusIter_HCALIN -> second;

      // construct vertex and get 4-momentum
      const double vX = vtx -> get_x();
      const double vY = vtx -> get_y();
      const double vZ = vtx -> get_z();

      CLHEP::Hep3Vector vertex(vX, vY, vZ);
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

      // check if good
      const bool isGoodClust = isAcceptableHCalCluster(E_vec_cluster);
      if (!isGoodClust) continue;

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
    RawClusterContainer::ConstRange    begin_end_HCALOUT = clustersHCALOUT->getClusters();
    RawClusterContainer::ConstIterator clusIter_HCALOUT;
    for (clusIter_HCALOUT = begin_end_HCALOUT.first; clusIter_HCALOUT != begin_end_HCALOUT.second; ++clusIter_HCALOUT) {

      // get oh cluster
      const RawCluster *cluster = clusIter_HCALOUT -> second;

      // construct vertex and get 4-momentum
      const double vX = vtx -> get_x();
      const double vY = vtx -> get_y();
      const double vZ = vtx -> get_z();

      CLHEP::Hep3Vector vertex(vX, vY, vZ);
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

      // check if good
      const bool isGoodClust = isAcceptableHCalCluster(E_vec_cluster);
      if (!isGoodClust) continue;

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
      ++iPart;
    }  // end oh cluster loop
  }  // end if (m_add_HCal_clusters)
  return;

}  // end 'addClusters(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'



void SCorrelatorJetTree::addParticles(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::addParticles(PHComposite*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&) Adding MC particles..." << endl;
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
  unsigned int iPart = particles.size();
  for (HepMC::GenEvent::particle_const_iterator p = hepMCevent -> particles_begin(); p != hepMCevent -> particles_end(); ++p) {

    // check if good & final state
    const bool isGoodPar    = isAcceptableParticle(*p);
    const bool isFinalState = ((*p) -> status() > 1);
    if (!isGoodPar || !isFinalState) continue;

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
    iPart++;
  }  // end particle loop
  return;

}  // end 'addParticles(PHCompositeNode*, vector<PseudoJet>&, map<int, pair<Jet::SRC, int>>&)'

// end ------------------------------------------------------------------------

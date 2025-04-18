/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorJetTreeMaker.ana.h
 *  \author Derek Anderson
 *  \date   03.28.2024
 *
 *  A module to produce a tree of jets for the sPHENIX
 *  Cold QCD Energy-Energy Correlator analysis. Initially
 *  derived from code by Antonio Silva.
 */
/// ---------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace findNode;



// analysis methods ===========================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Grab event-wise information
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::GetEventVariables(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::GetEventVariables(PHCompositeNode*) Grabbing event info..." << endl;
    }

    // get indices of relevant subevents if needed
    if (m_config.isSimulation) {
      m_vecEvtsToGrab = Tools::GrabSubevents(topNode, m_config.subEvtOpt, m_config.isEmbed);
    }

    // set event info
    m_recoOutput.evt.SetInfo(topNode);
    if (m_config.isSimulation) {
      m_trueOutput.evt.SetInfo(topNode, m_config.isEmbed, m_vecEvtsToGrab);
    }
    return;

  }  // end 'GetEventVariables(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Grab jet/constituent information from FastJet output
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::GetJetVariables(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::GetJetVariables(PHCompositeNode*) Grabbing jet (and constituent) info..." << endl;
    }

    // grab reco variables
    m_recoOutput.jets.resize( m_recoJets.size() );
    m_recoOutput.csts.resize( m_recoJets.size() );
    for (uint64_t iJet = 0; iJet < m_recoJets.size(); iJet++) {

      // grab jet info
      m_recoOutput.jets[iJet].SetInfo( m_recoJets[iJet] );
      m_recoOutput.jets[iJet].SetJetID( iJet );
      for (auto cst : m_recoJets[iJet].constituents()) {

        // grab cst info
        Types::CstInfo cstInfo( cst );
        cstInfo.SetEmbedID( Tools::GetSignal(m_config.isEmbed) );
        cstInfo.SetJetInfo( iJet, m_recoOutput.jets[iJet] );
        cstInfo.SetType( GetRecoCstType() );
        cstInfo.SetPID( 211 );  // FIXME should use PID when/if ready
        m_recoOutput.csts[iJet].push_back( cstInfo );

      }  // end reco cst loop
    }  // end reco jet loop

    // grab truth variables
    if (m_config.isSimulation) {
      m_trueOutput.jets.resize( m_trueJets.size() );
      m_trueOutput.csts.resize( m_trueJets.size() );
      for (uint64_t iJet = 0; iJet < m_trueJets.size(); iJet++) {

        // grab jet info
        m_trueOutput.jets[iJet].SetInfo( m_trueJets[iJet] );
        m_trueOutput.jets[iJet].SetJetID( iJet );

        // loop over constituents
        for (auto cst : m_trueJets[iJet].constituents()) {

          // get corresponding particle
          HepMC::GenParticle* par = Tools::GetHepMCGenParticleFromBarcode(cst.user_index(), topNode);

          // grab cst info
          Types::CstInfo cstInfo( cst );
          cstInfo.SetEmbedID( m_mapCstToEmbedID[cst.user_index()] );
          cstInfo.SetJetInfo( iJet, m_trueOutput.jets[iJet] );
          cstInfo.SetType( Const::Object::Particle );
          cstInfo.SetPID( par -> pdg_id() );
          m_trueOutput.csts[iJet].push_back( cstInfo );
        }  // end reco cst loop
      }  // end reco jet loop
    }
    return;

  }  // end 'GetJetVariables(PHCompositeNode* topNode)'



  // --------------------------------------------------------------------------
  //! Grab jet/constituent information from node
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::ReadJetNodes(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::ReadJetNodes(PHCompositeNode*) Reading jet nodes" << endl;
    }

    // prepare variables for jet finding
    ResetJetVariables();


    // grab reconstructed jets
    m_recoNode = getClass<JetContainer>(topNode, m_config.inRecoNodeName.data());
    if (!m_recoNode) {
      cerr << "SCorrelatorJetTreeMaker::ReadJetNodes: PANIC: couldn't open reconstructed jet node \"" << m_config.inRecoNodeName << "\"! Aborting!" << endl;
      assert(m_recoNode);
    }

    // if needed, grab truth jets
    if (m_config.isSimulation) {
      m_trueNode = getClass<JetContainer>(topNode, m_config.inTrueNodeName.data());
      if (!m_trueNode) {
        cerr << "SCorrelatorJetTreeMaker::ReadJetNodes: PANIC: couldn't open truth jet node \"" << m_config.inTrueNodeName << "\"! Aborting!" << endl;
        assert(m_trueNode);
      }
    }
  
    // loop through reconstructed jets
    m_recoOutput.jets.resize( m_recoNode -> size() );
    m_recoOutput.csts.resize( m_recoNode -> size() );
    for (
      uint64_t iRecoJet = 0;
      iRecoJet < m_recoNode -> size();
      ++iRecoJet
    ) {

      Jet* jet = m_recoNode -> get_jet(iRecoJet);
      m_recoOutput.jets[iRecoJet].SetInfo( *jet );
      m_recoOutput.jets[iRecoJet].SetJetID( iRecoJet );

      // loop through constituents
      for (const auto& cst : jet -> get_comp_vec()) {
        Types::CstInfo cstInfo(
          cst,
          topNode,
          Interfaces::GetRecoVtx(topNode)
        );
        cstInfo.SetEmbedID( Tools::GetSignal(m_config.isEmbed) );
        cstInfo.SetJetInfo( iRecoJet, m_recoOutput.jets[iRecoJet] );
        cstInfo.SetPID( 211 );  // FIXME should use PID when/if ready
        m_recoOutput.csts[iRecoJet].push_back( cstInfo );
      }  // end cst loop
    }  // end jet loop


    // if needed, loop through truth jets
    if (m_config.isSimulation) {
      m_trueOutput.jets.resize( m_trueNode -> size() );
      m_trueOutput.csts.resize( m_trueNode -> size() );
      for (
        uint64_t iTrueJet = 0;
        iTrueJet < m_trueNode -> size();
        ++iTrueJet
      ) {

        Jet* jet = m_trueNode -> get_jet(iTrueJet);
        m_trueOutput.jets[iTrueJet].SetInfo( *jet );
        m_trueOutput.jets[iTrueJet].SetJetID( iTrueJet );

        // loop through constituents
        for (const auto& cst : jet -> get_comp_vec()) {
          Types::CstInfo cstInfo(
            cst,
            topNode,
            nullopt,
            Tools::GetEmbedIDFromTrackID(cst.second, topNode)
          );
          cstInfo.SetJetInfo( iTrueJet, m_trueOutput.jets[iTrueJet] );
          m_trueOutput.csts[iTrueJet].push_back( cstInfo );
        }  // end cst loop
      }  // end jet loop
    }
    return;

  }  // end 'ReadJetNodes(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Construct jets using FastJet
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::MakeJets(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::MakeJets(PHCompositeNode*) Making jets" << endl;
    }

    // prepare variables for jet finding
    ResetJetVariables();

    // do jet finding
    MakeRecoJets(topNode);
    if (m_config.isSimulation) {
      MakeTrueJets(topNode);
    }
    return;

  }  // end 'MakeJets(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Make reconstructed jets based on specified type
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::MakeRecoJets(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 2)) {
      cout << "SCorrelatorJetTreeMaker::MakeRecoJets(PHCompositeNode*) Making reco jets..." << endl;
    }

    // add constitutents
    switch (m_config.jetType) {

      case Const::JetType::Full:
        AddFlow(topNode);
        break;

      case Const::JetType::Tower:
        AddTowers(topNode, {Const::Subsys::RECal});
        AddTowers(topNode, {Const::Subsys::IHCal, Const::Subsys::OHCal});
        break;

      case Const::JetType::Cluster:
        AddClusters(topNode, {Const::Subsys::EMCal});
        AddClusters(topNode, {Const::Subsys::IHCal, Const::Subsys::OHCal});
        break;

      case Const::JetType::Charged:
        [[fallthrough]];

      default:
        AddTracks(topNode);
        break;
    }

    // cluster jets
    m_recoCluster = make_unique<ClusterSequence>(m_recoJetInput, *m_recoJetDef);
    m_recoJets    = m_recoCluster -> inclusive_jets();
    return;

  }  // end 'MakeRecoJets(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Make corresponding truth jets
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::MakeTrueJets(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 2)) {
      cout << "SCorrelatorJetTreeMaker::MakeTrueJets(PHCompositeNode*) Making truth jets..." << endl;
    }

    // add constituents
    AddParticles(topNode);

    // run clustering, grab jets, and return
    m_trueCluster = make_unique<ClusterSequence>(m_trueJetInput, *m_trueJetDef);
    m_trueJets    = m_trueCluster -> inclusive_jets();
    return;

  }  // end 'MakeTrueJets(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Add tracks to a FastJet input
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::AddTracks(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 3)) {
      cout << "SCorrelatorJetTreeMaker::AddTracks(PHCompositeNode*) Adding tracks to fastjet input" << endl;
    }

    // loop over tracks
    int64_t       iCst    = m_recoJetInput.size();
    SvtxTrackMap* mapTrks = Interfaces::GetTrackMap(topNode);
    for (
      SvtxTrackMap::Iter itTrk = mapTrks -> begin();
      itTrk != mapTrks -> end();
      ++itTrk
    ) {

      // get track
      SvtxTrack* track = itTrk -> second;
      if (!track) continue;

      // grab info
      Types::TrkInfo info(track, topNode);

      // check if good
      const bool isGoodTrack = IsGoodTrack(info, track, topNode);
      if (!isGoodTrack) continue;

      // grab barcode of matching particle
      int matchID;
      if (m_config.isSimulation) {
        matchID = Tools::GetMatchID(track, m_trackEval);
      } else {
        matchID = -1;
      }

      // make pseudojet
      fastjet::PseudoJet pseudojet(
        info.GetPX(),
        info.GetPY(),
        info.GetPZ(),
        info.GetEne()
      );
      pseudojet.set_user_index(matchID);

      // add to list
      m_recoJetInput.push_back(pseudojet);
      ++iCst;

    }  // end track loop
    return;

  }  // end 'AddTracks(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Add particle-flow objects to FastJet input
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::AddFlow(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 3)) {
      cout << "SCorrelatorJetTreeMaker::AddFlow(PHCompositeNode*) Adding particle flow elements to fastjet input" << endl;
    }

    // abort if jets should be charged
    if (m_config.jetType == Const::JetType::Charged) {
      cerr << "SCorrelatorJetTreeMaker::AddFlow: PANIC: trying to add particle flow elements to charged jets! Aborting" << endl;
      assert(m_config.jetType != Const::JetType::Charged);
    }

    // loop over pf elements
    int64_t                       iCst      = m_recoJetInput.size();
    ParticleFlowElementContainer* flowStore = Interfaces::GetFlowStore(topNode);
    for (
      ParticleFlowElementContainer::ConstIterator itFlow = flowStore -> getParticleFlowElements().first;
      itFlow != flowStore -> getParticleFlowElements().second;
      ++itFlow
    ) {

      // get pf element
      ParticleFlowElement* flow = itFlow -> second;
      if (!flow) continue;

      // grab info
      Types::FlowInfo info(flow);

      // check if good
      const bool isGoodFlow = IsGoodFlow(info);
      if (!isGoodFlow) continue;

      // make pseudojet
      fastjet::PseudoJet pseudojet(
        info.GetPX(),
        info.GetPY(),
        info.GetPZ(),
        info.GetEne()
      );
      pseudojet.set_user_index(iCst);

      // add to list
      m_recoJetInput.push_back(pseudojet);
      ++iCst;

    }  // end pf element loop
    return;

  }  // end 'AddFlow(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Add towers to FastJet input
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::AddTowers(PHCompositeNode* topNode, vector<Const::Subsys> vecSubsysToAdd) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 3)) {
      cout << "SCorrelatorJetTreeMaker::AddTowers(PHCompositeNode*, vector<Const::Subsys>) Adding calo towers to fastjet input" << endl;
    }

    // abort if jets should be charged
    if (m_config.jetType == Const::JetType::Charged) {
      cerr << "SCorrelatorJetTreeMaker::AddTowers: ABORT: trying to add calo towers to charged jets! Aborting" << endl;
      assert(m_config.jetType != Const::JetType::Charged);
    }

    // abort if jets should be cluster-based
    if (m_config.jetType == Const::JetType::Cluster) {
      cerr << "SCorrelatorJetTreeMaker::AddTowers: ABORT: trying to add calo towers to cluster jets! Aborting" << endl;
      assert(m_config.jetType != Const::JetType::Cluster);
    }

    // get primary reconstructed vtx
    ROOT::Math::XYZVector vtx = Interfaces::GetRecoVtx(topNode);

    // loop over subsystems to add
    int64_t iCst = m_recoJetInput.size();
    for (Const::Subsys subsys : vecSubsysToAdd) {

      // loop over towers
      TowerInfoContainer* towers = Interfaces::GetTowerInfoStore(topNode, Const::MapIndexOntoTowerInfo()[ subsys ]);
      for (uint32_t channel = 0; channel < towers -> size(); channel++) {

        // get tower
        TowerInfo* tower = towers -> get_tower_at_channel(channel);
        if (!tower) continue;

        // grab info
        Types::TwrInfo info(subsys, channel, tower, topNode, vtx);

        // check if good
        const bool isGoodTwr = IsGoodTower(info, subsys);
        if (!isGoodTwr) continue;

        // make pseudojet
        fastjet::PseudoJet pseudojet(
          info.GetPX(),
          info.GetPY(),
          info.GetPZ(),
          info.GetEne()
        );
        pseudojet.set_user_index(iCst);

        // add to list
        m_recoJetInput.push_back(pseudojet);
        ++iCst;

      }  // end tower loop
    }  // end subsystem loop
    return;

  }  // end 'AddTowers(PHCompositeNode*, vector<Const::Subsys>)'



  // --------------------------------------------------------------------------
  //! Add clusters from specified calorimeters to FastJet input
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::AddClusters(PHCompositeNode* topNode, vector<Const::Subsys> vecSubsysToAdd) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 3)) {
      cout << "SCorrelatorJetTreeMaker::AddClusters(PHCompositeNode*, vector<Const::Subsys>) Adding calo clusters to fastjet input" << endl;
    }

    // abort if jets should be charged
    if (m_config.jetType == Const::JetType::Charged) {
      cerr << "SCorrelatorJetTreeMaker::AddClusters: ABORT: trying to add calo clusters to charged jets! Aborting" << endl;
      assert(m_config.jetType != Const::JetType::Charged);
    }

    // abort if jets should be tower-based
    if (m_config.jetType == Const::JetType::Tower) {
      cerr << "SCorrelatorJetTreeMaker::AddClusters: ABORT: trying to add calo clusters to tower jets! Aborting" << endl;
      assert(m_config.jetType != Const::JetType::Tower);
    }

    // get primary reconstructed vtx
    ROOT::Math::XYZVector vtx = Interfaces::GetRecoVtx(topNode);

    // loop over subsystems to add
    int64_t iCst = m_recoJetInput.size();
    for (Const::Subsys subsys : vecSubsysToAdd) {

      // loop over clusters
      RawClusterContainer::ConstRange clusters = Interfaces::GetClusters(topNode, Const::MapIndexOntoClusters()[ subsys ]);
      for (
        RawClusterContainer::ConstIterator itClust = clusters.first;
        itClust != clusters.second;
        ++itClust
      ) { 

        // get cluster
        RawCluster* cluster = itClust -> second;
        if (!cluster) continue;

        // grab info
        Types::ClustInfo info(cluster, vtx, subsys);

        // check if good
        const bool isGoodClust = IsGoodCluster(info, subsys);
        if (!isGoodClust) continue;

        // make pseudojet
        fastjet::PseudoJet pseudojet(
          info.GetPX(),
          info.GetPY(),
          info.GetPZ(),
          info.GetEne()
        );
        pseudojet.set_user_index(iCst);

        // add to list
        m_recoJetInput.push_back(pseudojet);
        ++iCst;

      }  // end cluster loop
    }  // end subsystem loop
    return;

  }  // end 'AddClusters(PHCompositeNode* topNode, vector<Const::Subsys>)'



  // --------------------------------------------------------------------------
  //! Add MC particles to FastJet input
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::AddParticles(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 3)) {
      cout << "SCorrelatorJetTreeMaker::AddParticles(PHComposite*) Adding MC particles..." << endl;
    }

    // loop over relevant subevents
    int64_t iCst = m_trueJetInput.size();
    for (const int evtToGrab : m_vecEvtsToGrab) {

      // grab subevent
      HepMC::GenEvent* mcEvt = Interfaces::GetGenEvent(topNode, evtToGrab);

      // grab embedding ID
      const int embedID = Tools::GetEmbedID(topNode, evtToGrab);

      // loop over particles in subevent
      for (
        HepMC::GenEvent::particle_const_iterator itPar = mcEvt -> particles_begin();
        itPar != mcEvt -> particles_end();
        ++itPar
      ) {

        // grab info
        Types::ParInfo info(*itPar, embedID);
        if (!info.IsFinalState()) continue;

        // check if particle is good
        const bool isGoodPar = IsGoodParticle(info);
        if (!isGoodPar) continue;

        // map barcode onto relevant embeddingID
        m_mapCstToEmbedID[info.GetBarcode()] = embedID;

        // create pseudojet & add to constituent vector
        fastjet::PseudoJet pseudojet(
          info.GetPX(),
          info.GetPY(),
          info.GetPZ(),
          info.GetEne()
        );
        pseudojet.set_user_index(info.GetBarcode());

        // add to list
        m_trueJetInput.push_back(pseudojet);
        ++iCst;

      }  // end particle loop
    }  // end subevent loop
    return;

  }  // end 'AddParticles(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Check if a track satisfies selection criteria
  // --------------------------------------------------------------------------
  bool SCorrelatorJetTreeMaker::IsGoodTrack(Types::TrkInfo& info, SvtxTrack* track, PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 4)) {
      cout << "SCorrelatorJetTreeMaker::IsGoodTrack(Types:TrkInfo&, SvtxTrack*, PHCompositeNode*) Checking if track is good..." << endl;
    }

    // if needed, check if dca is in pt-dependent range
    bool isInDcaSigma = true;
    if (m_config.doDcaSigmaCut) {
      isInDcaSigma = info.IsInSigmaDcaCut(
        m_config.nSigCut,
        m_config.ptFitMax,
        m_config.fSigDca
      );
    }

    // if needed, check if track vertex is in acceptance
    const bool isInVtxRange = m_config.doVtxCut ? IsGoodVertex(ROOT::Math::XYZVector(info.GetVX(), info.GetVY(), info.GetVZ())) : true;

    // if needed, check if track is from primary vertex,
    const bool isFromPrimVtx = m_config.useOnlyPrimVtx ? info.IsFromPrimaryVtx(topNode) : true;

    // check seed, if in acceptance, and return overall goodness
    const bool isSeedGood = Tools::IsGoodTrackSeed(track, m_config.requireSiSeeds);
    const bool isInAccept = info.IsInAcceptance(m_config.trkAccept);
    return (isInDcaSigma && isInVtxRange && isFromPrimVtx && isSeedGood && isInAccept);

  }  // end 'IsGoodTrack(Types::TrkInfo&, SvtxTrack*, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Check if a particle-flow object satisfies selection criteria
  // --------------------------------------------------------------------------
  bool SCorrelatorJetTreeMaker::IsGoodFlow(Types::FlowInfo& info) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 4)) {
      cout << "SCorrelatorJetTreeMaker::IsGoodFlow(Types::FlowInfo&) Checking if particle flow element is good..." << endl;
    }

    const bool isInAccept = info.IsInAcceptance(m_config.flowAccept);
    return isInAccept;

  }  // end 'IsGoodFlow(Types::FlowInfo&)'



  // --------------------------------------------------------------------------
  //! Check if a tower satisfies selection criteria
  // --------------------------------------------------------------------------
  bool SCorrelatorJetTreeMaker::IsGoodTower(Types::TwrInfo& info, Const::Subsys subsys) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 4)) {
      cout << "SCorrelatorJetTreeMaker::IsGoodTower(types::TwrInfo&, Const::subsys) Checking if cluster is good..." << endl;
    }

    // check if in acceptance
    bool isInAccept;
    switch (subsys) {

      case Const::Subsys::EMCal:
        [[fallthrough]];

      case Const::Subsys::RECal:
        isInAccept = info.IsInAcceptance(m_config.eTwrAccept);
        break;

      case Const::Subsys::IHCal:
        [[fallthrough]];

      case Const::Subsys::OHCal:
        isInAccept = info.IsInAcceptance(m_config.hTwrAccept);
        break;

      default:
        isInAccept = true;
        break;
    }
    return (isInAccept && info.IsGood());

  }  // end 'IsGoodTower(Types::TwrInfo&, Const::Subsys)'



  // --------------------------------------------------------------------------
  //! Check if a cluster satisfies selection criteria
  // --------------------------------------------------------------------------
  bool SCorrelatorJetTreeMaker::IsGoodCluster(Types::ClustInfo& info, Const::Subsys subsys) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 4)) {
      cout << "SCorrelatorJetTreeMaker::IsGoodCluster(types::ClustInfo&, Const::subsys) Checking if cluster is good..." << endl;
    }

    // check if in acceptance
    bool isInAccept;
    switch (subsys) {

      case Const::Subsys::EMCal:
        isInAccept = info.IsInAcceptance(m_config.eClustAccept);
        break;

      case Const::Subsys::IHCal:
        [[fallthrough]];

      case Const::Subsys::OHCal:
        isInAccept = info.IsInAcceptance(m_config.hClustAccept);
        break;

      default:
        isInAccept = true;
        break;
    }
    return isInAccept;

  }  // end 'IsGoodCluster(Types::ClustInfo&, Const::Subsys)'



  // --------------------------------------------------------------------------
  //! Check if a particle satisfies selection criteria
  // --------------------------------------------------------------------------
  bool SCorrelatorJetTreeMaker::IsGoodParticle(Types::ParInfo& info) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 4)) {
      cout << "SCorrelatorJetTreeMaker::IsGoodParticle(Types::ParInfo&) Checking if particle is good..." << endl;
    }

    // check charge if needed
    //   - TODO allow for different truth/reco jet types
    bool isGoodCharge;
    switch (m_config.jetType) {

      case Const::JetType::Charged:
        isGoodCharge = (info.GetCharge() != 0.);
        break;

      case Const::JetType::Tower:
        [[fallthrough]];

      case Const::JetType::Cluster:
        [[fallthrough]];

      case Const::JetType::Full:
        [[fallthrough]];

      default:
        isGoodCharge = true;
        break;
    }

    // check if in acceptance and return
    const bool isInAccept = info.IsInAcceptance(m_config.parAccept);
    return (isGoodCharge && isInAccept);

  }  // end 'IsGoodParticle(Types::ParInfo&)'



  // --------------------------------------------------------------------------
  //! Check if a vertex falls in specified cuts
  // --------------------------------------------------------------------------
  bool SCorrelatorJetTreeMaker::IsGoodVertex(const ROOT::Math::XYZVector vtx) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SCorrelatorJetTreeMaker::IsGoodVertex(ROOT::Math::XYZVector) Checking if vertex is good..." << endl;
    }

    // check if vertex is in acceptance
    const bool isInVrAccept = ((vtx.Rho() >= m_config.vrAccept.first) && (vtx.Rho() <= m_config.vzAccept.second));
    const bool isInVzAccept = ((vtx.Z()   >= m_config.vzAccept.first) && (vtx.Z()   <= m_config.vzAccept.second));
    return (isInVrAccept && isInVzAccept);

  }  // end 'IsGoodVertex(const ROOT::Math::XYZVector)'



  // --------------------------------------------------------------------------
  //! Get corresponding input for specified jet type
  // --------------------------------------------------------------------------
  int SCorrelatorJetTreeMaker::GetRecoCstType() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 2)) {
      cout << "SCorrelatorJetTreeMaker::GetRecoCstType() Getting cst type..." << endl;
    }

    int type;
    switch (m_config.jetType) {

      case Const::JetType::Charged:
        type = Const::Object::Track;
        break;

      case Const::JetType::Tower:
        type = Const::Object::Tower;
        break;

      case Const::JetType::Cluster:
        type = Const::Object::Cluster;
        break;

      case Const::JetType::Full:
        type = Const::Object::Flow;
        break;

      default:
        type = Const::Object::Mystery;
        break;
    }
    return type;

  }  // end 'GetRecoCstType()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 'SCorrelatorJetTreeMaker.ana.h'
// Derek Anderson
// 03.28.2024
//
// A module to produce a tree of jets for the sPHENIX
// Cold QCD Energy-Energy Correlator analysis.
//
// Initially derived from code by Antonio Silva (thanks!!)
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // analysis methods ---------------------------------------------------------

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
        cstInfo.SetEmbedID( 0 );  // FIXME set to signal
        cstInfo.SetJetInfo( m_recoOutput.jets[iJet] );
        cstInfo.SetJetID( iJet );
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
          cstInfo.SetJetInfo( m_trueOutput.jets[iJet] );
          cstInfo.SetJetID( iJet );
          cstInfo.SetType( Const::Object::Particle );
          cstInfo.SetPID( par -> pdg_id() );
          m_trueOutput.csts[iJet].push_back( cstInfo );
        }  // end reco cst loop
      }  // end reco jet loop
    }
    return;

  }  // end 'GetJetVariables(PHCompositeNode* topNode)'



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



  void SCorrelatorJetTreeMaker::MakeRecoJets(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 2)) {
      cout << "SCorrelatorJetTreeMaker::MakeRecoJets(PHCompositeNode*) Making reco jets..." << endl;
    }

    // add constitutents
    switch (m_config.jetType) {

      case Const::JetType::Neutral:
        AddClusts(topNode, {Const::Subsys::EMCal});
        AddClusts(topNode, {Const::Subsys::IHCal, Const::Subsys::OHCal});
        break;

      case Const::JetType::Full:
        AddFlow(topNode);
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



  void SCorrelatorJetTreeMaker::AddClusts(PHCompositeNode* topNode, vector<Const::Subsys> vecSubsysToAdd) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 3)) {
      cout << "SCorrelatorJetTreeMaker::AddClusters(PHCompositeNode*, vector<Const::Subsys>) Adding calo clusters to fastjet input" << endl;
    }

    // abort if jets should be charged
    if (m_config.jetType == Const::JetType::Charged) {
      cerr << "SCorrelatorJetTreeMaker::AddClusts: ABORT: trying to add calo clusters to charged jets! Aborting" << endl;
      assert(m_config.jetType != Const::JetType::Charged);
    }

    // loop over subsystems to add
    int64_t iCst = m_recoJetInput.size();
    for (Const::Subsys subsys : vecSubsysToAdd) {

      // loop over clusters
      RawClusterContainer::ConstRange clusters = Interfaces::GetClusters(topNode, Const::MapIndexOntoNode()[ subsys ]);
      for (
        RawClusterContainer::ConstIterator itClust = clusters.first;
        itClust != clusters.second;
        ++itClust
      ) { 

        // get cluster
        RawCluster* cluster = itClust -> second;
        if (!cluster) continue;

        // get primary reconstructed vtx
        ROOT::Math::XYZVector vtx = Interfaces::GetRecoVtx(topNode);

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

  }  // end 'AddClusts(PHCompositeNode* topNode, vector<Const::Subsys>)'



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



  bool SCorrelatorJetTreeMaker::IsGoodFlow(Types::FlowInfo& info) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 4)) {
      cout << "SCorrelatorJetTreeMaker::IsGoodFlow(Types::FlowInfo&) Checking if particle flow element is good..." << endl;
    }

    const bool isInAccept = info.IsInAcceptance(m_config.flowAccept);
    return isInAccept;

  }  // end 'IsGoodFlow(Types::FlowInfo&)'



  bool SCorrelatorJetTreeMaker::IsGoodCluster(Types::ClustInfo& info, Const::Subsys subsys) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 4)) {
      cout << "SCorrelatorJetTreeMaker::IsGoodCluster(types::ClustInfo&, Const::subsys) Checking if cluster is good..." << endl;
    }

    // check if in acceptance
    bool isInAccept;
    switch (subsys) {

      case Const::Subsys::EMCal:
        isInAccept = info.IsInAcceptance(m_config.ecalAccept);
        break;

      case Const::Subsys::IHCal:
        [[fallthrough]];

      case Const::Subsys::OHCal:
        isInAccept = info.IsInAcceptance(m_config.hcalAccept);
        break;

      default:
        isInAccept = true;
        break;
    }
    return isInAccept;

  }  // end 'IsGoodCluster(Types::ClustInfo&, Const::Subsys)'



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

      case Const::JetType::Neutral:
        isGoodCharge = (info.GetCharge() == 0.);
        break;

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



  int SCorrelatorJetTreeMaker::GetRecoCstType() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 2)) {
      cout << "SCorrelatorJetTreeMaker::GetRecoCstType() Getting cst type..." << endl;
    }

    int type = Const::Object::Unknown;
    switch (m_config.jetType) {

      case Const::JetType::Neutral:
        type = Const::Object::Cluster;
        break;

      case Const::JetType::Full:
        type = Const::Object::Flow;
        break;

      case Const::JetType::Charged:
        type = Const::Object::Track;
        break;

      default:
        type = Const::Object::Unknown;
        break;
    }
    return type;

  }  // end 'GetRecoCstType()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

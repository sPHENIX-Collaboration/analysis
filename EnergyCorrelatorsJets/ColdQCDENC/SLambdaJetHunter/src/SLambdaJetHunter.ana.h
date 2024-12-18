/// ---------------------------------------------------------------------------
/*! \file   SLambdaJetHunter.ana.h
 *  \author Derek Anderson
 *  \date   01.25.2024
 *
 *  A minimal analysis module to find lambda-tagged
 *  jets in pythia events.
 */
/// ---------------------------------------------------------------------------

#pragma once

// make common namespaces implicit
using namespace std;
using namespace fastjet;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // analysis methods =========================================================

  // --------------------------------------------------------------------------
  //! Grab event-level information
  // --------------------------------------------------------------------------
  void SLambdaJetHunter::GrabEventInfo(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::GrabEventInfo(PHCompositeNode*) Grabbing event info" << endl;
    }

    m_vecSubEvts = Tools::GrabSubevents(topNode);
    m_genEvtInfo.SetInfo(topNode, m_config.isEmbed, m_vecSubEvts);
    return;

  }  // end 'GrabEventInfo(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Find all lambds in event
  // --------------------------------------------------------------------------
  void SLambdaJetHunter::FindLambdas(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::FindLambdas(PHCompositeNode*) Finding all lambdas in event" << endl;
    }

    // loop over subevents
    for (const int subEvt : m_vecSubEvts) {

      // loop over particles
      HepMC::GenEvent* genEvt = Interfaces::GetGenEvent(topNode, subEvt);
      for (
        HepMC::GenEvent::particle_const_iterator hepPar = genEvt -> particles_begin();
        hepPar != genEvt -> particles_end();
        ++hepPar
      ) {

        // check if lambda and if found yet
        const bool isLambda = IsLambda( (*hepPar) -> pdg_id() );
        const bool isNew    = IsNewLambda( (*hepPar) -> barcode() );
        if (!isLambda || !isNew) continue;

        // grab info
        Types::ParInfo lambda( (*hepPar), subEvt );

        // check if good
        const bool isGood = IsGoodLambda(lambda);
        if (!isGood) continue;

        // if a new good lambda, add to output vector and lambda-jet map
        m_lambdaInfo.push_back(lambda);
        m_mapLambdaJetAssoc.insert(
          make_pair( (*hepPar) -> barcode(), -1 )
        );

      }  // end particle loop
    }  // end subevent loop
    return;

  }  // end 'FindLambdas(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Reconstruct jets in an event
  // --------------------------------------------------------------------------
  void SLambdaJetHunter::MakeJets(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::MakeJets() Making jets" << endl;
    }

    // for fastjet
    vector<PseudoJet> vecPseudoJets;

    // loop over subevents
    for (const int subEvt : m_vecSubEvts) {

      // loop over particles
      HepMC::GenEvent* genEvt = Interfaces::GetGenEvent(topNode, subEvt);
      for (
        HepMC::GenEvent::particle_const_iterator hepPar = genEvt -> particles_begin();
        hepPar != genEvt -> particles_end();
        ++hepPar
      ) {

        // grab particle info
        Types::ParInfo particle(*hepPar, subEvt);

        // check if particle is good & final state
        const bool isParGood = IsGoodParticle(particle);
        if (!isParGood || !particle.IsFinalState()) continue;

        // make pseudojet
        PseudoJet pseudo(
          particle.GetPX(),
          particle.GetPY(),
          particle.GetPZ(),
          particle.GetEne()
        );
        pseudo.set_user_index(particle.GetBarcode());
        vecPseudoJets.push_back(pseudo);

      }  // end particle loop
    }  // end subevent loop

    // set jet definition
    JetDefinition definition(
      Const::MapStringOntoFJAlgo()[m_config.jetAlgo],
      m_config.rJet,
      Const::MapStringOntoFJRecomb()[m_config.jetRecomb],
      fastjet::Best
    );

    // run clustering
    ClusterSequence* sequence = new ClusterSequence(vecPseudoJets, definition);

    // grab jets and return
    m_vecFastJets = sequence -> inclusive_jets();
    return;

  }  // end 'MakeJets(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Set jet output information
  // --------------------------------------------------------------------------
  /* TODO move to a dedicated interface */
  void SLambdaJetHunter::CollectJetOutput(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::CollectOutput() hunting down lambda-taggged jets" << endl;
    }

    // reserve space for fastjet output
    m_jetInfo.resize( m_vecFastJets.size() );
    m_cstInfo.resize( m_vecFastJets.size() );

    // loop over fastjet output
    for (size_t iJet = 0; iJet < m_vecFastJets.size(); iJet++) {

      // grab jet info
      m_jetInfo[iJet].SetInfo( m_vecFastJets[iJet] );
      m_jetInfo[iJet].SetJetID( iJet );

      // loop over constituents
      m_cstInfo[iJet].resize( m_vecFastJets[iJet].constituents().size() );
      for (size_t iCst = 0; iCst < m_vecFastJets[iJet].constituents().size(); iCst++) {

        // grab cst info
        m_cstInfo[iJet][iCst].SetInfo( m_vecFastJets[iJet].constituents()[iCst] );

        // get particle PDG
        HepMC::GenParticle* hepCst = Tools::GetHepMCGenParticleFromBarcode(m_cstInfo[iJet][iCst].GetCstID(), topNode);
        m_cstInfo[iJet][iCst].SetPID( hepCst -> pdg_id() );

        // run calculations
        const float pCst  = hypot( m_cstInfo[iJet][iCst].GetPX(), m_cstInfo[iJet][iCst].GetPY(), m_cstInfo[iJet][iCst].GetPZ() );
        const float pJet  = hypot( m_jetInfo[iJet].GetPX(), m_jetInfo[iJet].GetPY(), m_jetInfo[iJet].GetPZ() );
        const float dfCst = m_cstInfo[iJet][iCst].GetPhi() - m_jetInfo[iJet].GetPhi();
        const float dhCst = m_cstInfo[iJet][iCst].GetEta() - m_jetInfo[iJet].GetEta();

        // grab remaining cst info
        m_cstInfo[iJet][iCst].SetJetID( m_jetInfo[iJet].GetJetID() );
        m_cstInfo[iJet][iCst].SetZ( pCst / pJet );
        m_cstInfo[iJet][iCst].SetDR( hypot(dfCst, dhCst) );

      }  // end cst loop
    }  // end jet loop
    return;

  }  // end 'CollectJetOutput(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Associate lambdas to jets using specified method
  // --------------------------------------------------------------------------
  void SLambdaJetHunter::AssociateLambdasToJets(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::AssociateLambdasToJets() associating found lambdas to jets" << endl;
    }

    // loop over lambdas
    for (Types::ParInfo& lambda : m_lambdaInfo) {

      optional<int> iAssocJet = nullopt;
      switch (m_config.associator) {

        case Associator::Barcode:
          iAssocJet = HuntLambdasByBarcode(lambda);
          break;

        case Associator::Decay:
          iAssocJet = HuntLambdasByDecayChain(lambda, topNode);
          break;

        case Associator::Distance:
          iAssocJet = HuntLambdasByDistance(lambda);
          break;

        default:
          iAssocJet = HuntLambdasByDistance(lambda);
          break;

      }  // end switch (associator)

      // assign associated jet
      if (iAssocJet.has_value()) {
        m_mapLambdaJetAssoc[lambda.GetBarcode()] = iAssocJet.value();
      } else {
        m_mapLambdaJetAssoc[lambda.GetBarcode()] = -1;
      }
    }  // end lambda loop
    return;

  }   // end 'AssociateLambdasToJets(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Fill output tree
  // --------------------------------------------------------------------------
  /* TODO move to a dedicated interface */
  void SLambdaJetHunter::FillOutputTree() {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::FillOutputTree() collecting output information and filling tree" << endl;
    }

    // collect event info
    m_evtNJets       = m_jetInfo.size();
    m_evtNLambdas    = m_mapLambdaJetAssoc.size();
    m_evtNTaggedJets = GetNTaggedJets();
    m_evtNChrgPars   = m_genEvtInfo.GetNChrgPar();
    m_evtNNeuPars    = m_genEvtInfo.GetNNeuPar();
    m_evtSumEPar     = (m_genEvtInfo.GetESumChrg() + m_genEvtInfo.GetESumNeu());
    m_evtVtxX        = m_genEvtInfo.GetPartonA().GetVX();
    m_evtVtxY        = m_genEvtInfo.GetPartonA().GetVY();
    m_evtVtxZ        = m_genEvtInfo.GetPartonA().GetVZ();

    // collect parton info
    m_evtPartID = make_pair( m_genEvtInfo.GetPartonA().GetPID(), m_genEvtInfo.GetPartonB().GetPID() );
    m_evtPartPx = make_pair( m_genEvtInfo.GetPartonA().GetPX(),  m_genEvtInfo.GetPartonB().GetPX()  );
    m_evtPartPy = make_pair( m_genEvtInfo.GetPartonA().GetPY(),  m_genEvtInfo.GetPartonB().GetPY()  );
    m_evtPartPz = make_pair( m_genEvtInfo.GetPartonA().GetPZ(),  m_genEvtInfo.GetPartonB().GetPZ()  );
    m_evtPartE  = make_pair( m_genEvtInfo.GetPartonA().GetEne(), m_genEvtInfo.GetPartonB().GetEne() );

    // collect lambda information
    for (Types::ParInfo& lambda : m_lambdaInfo) {

      // collect general information
      m_lambdaID.push_back( lambda.GetBarcode() );
      m_lambdaPID.push_back( lambda.GetPID() );
      m_lambdaEmbedID.push_back( lambda.GetEmbedID() );
      m_lambdaE.push_back( lambda.GetEne() );
      m_lambdaPt.push_back( lambda.GetPT() );
      m_lambdaEta.push_back( lambda.GetEta() );
      m_lambdaPhi.push_back( lambda.GetPhi() );

      // collect associate jet information
      m_lambdaJetID.push_back( m_mapLambdaJetAssoc[lambda.GetBarcode()] );
      if (m_mapLambdaJetAssoc[lambda.GetBarcode()] > -1) {
        m_lambdaZ.push_back( GetLambdaAssocZ(lambda) );
        m_lambdaDr.push_back( GetLambdaAssocDr(lambda) );
      } else {
        m_lambdaZ.push_back( -1. );
        m_lambdaDr.push_back( -1. );
      }
    }

    // collect jet information
    for (Types::JetInfo& jet : m_jetInfo) {
      m_jetHasLambda.push_back( HasLambda(jet) );
      m_jetNCst.push_back( jet.GetNCsts() );
      m_jetID.push_back( jet.GetJetID() );
      m_jetE.push_back( jet.GetEne() );
      m_jetPt.push_back( jet.GetPT() );
      m_jetEta.push_back( jet.GetEta() );
      m_jetPhi.push_back( jet.GetPhi() );
    }  // end jet loop

    // collect cst information
    m_cstID.resize( m_cstInfo.size() );
    m_cstPID.resize( m_cstInfo.size() );
    m_cstJetID.resize( m_cstInfo.size() );
    m_cstEmbedID.resize( m_cstInfo.size() );
    m_cstZ.resize( m_cstInfo.size() );
    m_cstDr.resize( m_cstInfo.size() );
    m_cstE.resize( m_cstInfo.size() );
    m_cstPt.resize( m_cstInfo.size() );
    m_cstEta.resize( m_cstInfo.size() );
    m_cstPhi.resize( m_cstInfo.size() );
    for (size_t iJet = 0; iJet < m_cstInfo.size(); iJet++) {
      m_cstID[iJet].resize( m_cstInfo[iJet].size() );
      m_cstPID[iJet].resize( m_cstInfo[iJet].size() );
      m_cstJetID[iJet].resize( m_cstInfo[iJet].size() );
      m_cstEmbedID[iJet].resize( m_cstInfo[iJet].size() );
      m_cstZ[iJet].resize( m_cstInfo[iJet].size() );
      m_cstDr[iJet].resize( m_cstInfo[iJet].size() );
      m_cstE[iJet].resize( m_cstInfo[iJet].size() );
      m_cstPt[iJet].resize( m_cstInfo[iJet].size() );
      m_cstEta[iJet].resize( m_cstInfo[iJet].size() );
      m_cstPhi[iJet].resize( m_cstInfo[iJet].size() );
      for (size_t iCst = 0; iCst < m_cstInfo[iJet].size(); iCst++) {
        m_cstID[iJet][iCst]      = m_cstInfo[iJet][iCst].GetCstID();
        m_cstPID[iJet][iCst]     = m_cstInfo[iJet][iCst].GetPID();
        m_cstJetID[iJet][iCst]   = m_cstInfo[iJet][iCst].GetJetID();
        m_cstEmbedID[iJet][iCst] = m_cstInfo[iJet][iCst].GetEmbedID();
        m_cstZ[iJet][iCst]       = m_cstInfo[iJet][iCst].GetZ();
        m_cstDr[iJet][iCst]      = m_cstInfo[iJet][iCst].GetDR();
        m_cstE[iJet][iCst]       = m_cstInfo[iJet][iCst].GetEne();
        m_cstPt[iJet][iCst]      = m_cstInfo[iJet][iCst].GetPT();
        m_cstEta[iJet][iCst]     = m_cstInfo[iJet][iCst].GetEta();
        m_cstPhi[iJet][iCst]     = m_cstInfo[iJet][iCst].GetPhi();
      }
    }  // end cst loop

    m_outTree -> Fill();
    return;

  }  // end 'FillOutputTree()'



  // --------------------------------------------------------------------------
  //! Check if a PHG4particle has a parent
  // --------------------------------------------------------------------------
  bool SLambdaJetHunter::HasParentInfo(const int parent) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::HasParentInfo(int) checking if PHG4particle has parent info" << endl;
    }

    return (parent != 0);

  }  // end 'HasParentInfo(int)'



  // --------------------------------------------------------------------------
  //! Check if a jet has a lambda associated to it
  // --------------------------------------------------------------------------
  bool SLambdaJetHunter::HasLambda(Types::JetInfo& jet) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::HasLambda(JetInfo&) checking if jet has associated lambda" << endl;
    }

    bool hasLambda = false;
    for (auto& [lamID, jetID]: m_mapLambdaJetAssoc) {
      if (jetID == (int) jet.GetJetID()) {
        hasLambda = true;
        break;
      }
    }  // end map loop
    return hasLambda;

  }  // end 'HasLambda(JetInfo&)'



  // --------------------------------------------------------------------------
  //! Check if a particle satisfies cuts
  // --------------------------------------------------------------------------
  bool SLambdaJetHunter::IsGoodParticle(Types::ParInfo& particle) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::IsGoodParticle(ParInfo&) checking if particle is good" << endl;
    }

    // check charge if needed
    bool isGoodCharge = true;
    if (m_config.isCharged) {
      isGoodCharge = (particle.GetCharge() != 0.);
    }

    // apply particle cuts & return overall goodness
    const bool isInAccept = particle.IsInAcceptance(m_config.parAccept);
    return (isGoodCharge && isInAccept);

  }  // end 'IsGoodParticle(ParInfo&)'



  // --------------------------------------------------------------------------
  //! Check if a lambda satisfies cuts
  // --------------------------------------------------------------------------
  bool SLambdaJetHunter::IsGoodLambda(Types::ParInfo& lambda) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::IsGoodLambda(ParInfo&) checking if lambda is good" << endl;
    }

    // make sure lambda is in acceptance
    const bool isInAccept = lambda.IsInAcceptance(m_config.parAccept);
    return isInAccept;

  }  // end 'IsGoodLambda(ParInfo&)'



  // --------------------------------------------------------------------------
  //! Check if pid corresponds to a lambda
  // --------------------------------------------------------------------------
  bool SLambdaJetHunter::IsLambda(const int pid) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::IsLambda(int) checking if particle is a lambda" << endl;
    }

    return (abs(pid) == m_const.pidLambda);

  }  // end 'IsLambda(int)'



  // --------------------------------------------------------------------------
  //! Check if a lambda hasn't been found yet
  // --------------------------------------------------------------------------
  bool SLambdaJetHunter::IsNewLambda(const int id) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::IsNewLambda(int) checking if lambda is already found" << endl;
    }

    return (m_mapLambdaJetAssoc.count(id) == 0);

  }  // end 'IsNewLambda(int)'



  // --------------------------------------------------------------------------
  //! Check if a HepMC particle is in a decay chain
  // --------------------------------------------------------------------------
  bool SLambdaJetHunter::IsInHepMCDecayChain(const int idToFind, HepMC::GenVertex* vtxStart) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::IsInHepMCDecayChain(int, HepMC::GenVertex*) checking if particle is in HepMC decay chain" << endl;
    }

    // make sure vectors are clear and add initial vertex
    m_vecVtxChecking.clear();
    m_vecVtxToCheck.clear();
    m_vecVtxToCheck.push_back(vtxStart);

    // search of all connected vertices for particle w/ barcode 'idToFind'
    int  nVtxChecked     = 0;
    bool isParticleFound = false;
    while (!isParticleFound && (nVtxChecked < m_const.maxVtxToCheck)) {

      // transfer vertices to-check to being-checked list
      m_vecVtxChecking.clear();
      for (auto vertex : m_vecVtxToCheck) {
        m_vecVtxChecking.push_back(vertex);
      }
      m_vecVtxToCheck.clear();

      // iterate over vertices being checked
      for (auto vertex : m_vecVtxChecking) {

        // iterate over particles in vertex
        HepMC::GenVertex::particles_out_const_iterator outPar;
        for (
          outPar = vertex -> particles_out_const_begin();
          outPar != vertex -> particles_out_const_end();
          ++outPar
        ) {
          if ((*outPar) -> barcode() == idToFind) {
            isParticleFound = true;
            break;
          } else {
            m_vecVtxToCheck.push_back((*outPar) -> end_vertex());
          }
        }  // end particle loop

        // if found particler, exit
        //   - otherwise increment no. of vertices checked
        if (isParticleFound) {
          break;
        } else {
          ++nVtxChecked;
        }
      }  // end vertex loop
    }  // end while (!isParticleFound)
    return isParticleFound;

  }  // end 'IsInHepMCDecayChain(int, HepMC::GenVertex*)'



  // --------------------------------------------------------------------------
  //! Check if a PHG4 particle is in a decay chain
  // --------------------------------------------------------------------------
  bool SLambdaJetHunter::IsInPHG4DecayChain(const int idToFind, const int idLambda, PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::IsInPHG4DecayChain(int) checking if particle is in Geant decay chain" << endl;
    }

    // grab truth information
    PHG4TruthInfoContainer*            container = Interfaces::GetTruthContainer(topNode);
    PHG4TruthInfoContainer::ConstRange particles = container -> GetParticleRange();

    // loop over particles
    m_vecIDToCheck.clear();
    for (
      PHG4TruthInfoContainer::ConstIterator itPar = particles.first;
      itPar != particles.second;
      ++itPar
    ) {

      // grab current particle
      PHG4Particle* phPar = itPar -> second;

      // grab parent if information is available
      PHG4Particle* phParent = NULL;
      if (phPar -> get_parent_id() != 0) {
        phParent = container -> GetParticle(phPar -> get_parent_id());
      } else {
        continue;
      }

      // check if parent matches lambda barcode/pid
      const bool hasLambdaBarcode = (phParent -> get_barcode() == idLambda);
      const bool hasLambdaPID     = (phParent -> get_pid()     == m_const.pidLambda);
      if (hasLambdaBarcode && hasLambdaPID) {
            m_vecIDToCheck.push_back(phPar -> get_barcode());
      }
    }  // end particle loop

    // check if idToFind is among barcodes
    bool isParticleFound = false;
    for (const int idToCheck : m_vecIDToCheck) {
      if (idToCheck == idToFind) {
        isParticleFound = true;
        break;
      }
    }
    return isParticleFound;

  }  // end 'IsInPHG4DecayChain(int, PHG4Particle*, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Calculate z between a lambda and its associated jet
  // --------------------------------------------------------------------------
  double SLambdaJetHunter::GetLambdaAssocZ(Types::ParInfo& lambda) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::GetLambdaAssocZ(ParInfo&) getting z of lambda relative to associated jet" << endl;
    }

    // get index of associated jet
    const int iAssoc = m_mapLambdaJetAssoc[lambda.GetBarcode()];

    // get total momenta
    const double pJet    = hypot(m_jetInfo.at(iAssoc).GetPX(), m_jetInfo.at(iAssoc).GetPY(), m_jetInfo.at(iAssoc).GetPZ());
    const double pLambda = hypot(lambda.GetPX(), lambda.GetPY(), lambda.GetPZ());

    // calculate z and return
    const double zLambda = pLambda / pJet;
    return zLambda;

  }  // end 'GetLambdaAssocZ(ParInfo& lambda)'



  // --------------------------------------------------------------------------
  //! Calculate delta-r between a lambda and the axis of its associated jet
  // --------------------------------------------------------------------------
  double SLambdaJetHunter::GetLambdaAssocDr(Types::ParInfo& lambda) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::GetLambdaAssocDr(ParInfo&) getting delta-R of lambda relative to associated jet" << endl;
    }

    // get index of associated jet
    const int iAssoc = m_mapLambdaJetAssoc[lambda.GetBarcode()];

    // calculate delta-phi
    double dfLambda = lambda.GetPhi() - m_jetInfo.at(iAssoc).GetPhi();
    if (dfLambda < 0.)             dfLambda += TMath::TwoPi();
    if (dfLambda > TMath::TwoPi()) dfLambda -= TMath::TwoPi();

    // calculate dr and return
    const double dhLambda = lambda.GetEta() - m_jetInfo.at(iAssoc).GetEta();
    const double drLambda = hypot(dfLambda, dhLambda);
    return drLambda;

  }  // end 'GetLambdaAssocZ(ParInfo& lambda)'



  // --------------------------------------------------------------------------
  //! Calculate the total no. of tagged jets
  // --------------------------------------------------------------------------
  uint64_t SLambdaJetHunter::GetNTaggedJets() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) {
      cout << "SLambdaJetHunter::GetNTaggedJets() getting no. of jets with a lambda in them" << endl;
    }

    uint64_t nTaggedJets = 0;
    for (auto& [lamID, jetID] : m_mapLambdaJetAssoc) {
      if (jetID > -1) ++nTaggedJets;
    }
    return nTaggedJets;

  }  // end 'GetNTaggedJets()'



  // --------------------------------------------------------------------------
  //! Associate lambda to a jet by inspecting constituents' barcodes
  // --------------------------------------------------------------------------
  optional<int> SLambdaJetHunter::HuntLambdasByBarcode(Types::ParInfo& lambda) {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::HuntLambdasByBarcode(ParInfo&) hunting for lambdas by inspecting barcode" << endl;
    }

    // return nullopt by default
    optional<int> iAssocJet = nullopt;

    // loop through jet constituents and check barcode
    bool foundAssocJet = false;
    for (size_t iJet = 0; iJet < m_jetInfo.size(); iJet++) {
      for (size_t iCst = 0; iCst < m_cstInfo[iJet].size(); iCst++) {
        if (m_cstInfo[iJet][iCst].GetCstID() == lambda.GetBarcode()) {
          foundAssocJet = true;
          iAssocJet     = iJet;
          break;
        }
      }  // end cst loop

      // if found association, exit loop
      if (foundAssocJet) break;

    }  // end jet loop
    return iAssocJet;

  }  // end 'HuntLambdasByBarcode(ParInfo&)'



  // --------------------------------------------------------------------------
  //! Associate lambda to a jet by inspecting constituents' decay chains
  // --------------------------------------------------------------------------
  optional<int> SLambdaJetHunter::HuntLambdasByDecayChain(Types::ParInfo& lambda, PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::HuntLambdasByDecayChain(ParInfo&, PHCompositeNode*) hunting for lambdas by inspecting decay chains" << endl;
    }

    // return nullopt by default
    optional<int> iAssocJet = nullopt;

    // grab PHG4 and HepMC information
    HepMC::GenParticle* hepLambda = Tools::GetHepMCGenParticleFromBarcode(lambda.GetBarcode(), topNode);
    HepMC::GenVertex*   hepEndVtx = hepLambda -> end_vertex();

    // loop over constituents and check which cst.s the lambda decayed into
    bool foundAssocJet = false;
    for (size_t iJet = 0; iJet < m_cstInfo.size(); iJet++) {
      for (size_t iCst = 0; iCst < m_cstInfo[iJet].size(); iCst++) {

        // check if constituent is in decay chain,
        bool isInDecayChain = false;
        if (hepEndVtx) {
          isInDecayChain = IsInHepMCDecayChain(m_cstInfo[iJet][iCst].GetCstID(), hepEndVtx);
        } else {
          isInDecayChain = IsInPHG4DecayChain(m_cstInfo[iJet][iCst].GetCstID(), lambda.GetBarcode(), topNode);
        }

        // if so, add lambda to lists
        if (isInDecayChain) {
          foundAssocJet = true;
          iAssocJet     = iJet;
          break;
        }
      } // end cst loop

      // if found association, exit loop
      if (foundAssocJet)  break;

    }  // end jet loop
    return iAssocJet;

  }  // end 'HuntLambdasByDecayChain(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Associate lambda to a jet based on distance
  // --------------------------------------------------------------------------
  optional<int> SLambdaJetHunter::HuntLambdasByDistance(Types::ParInfo& lambda) {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::HuntLambdasByDistance(ParInfo&) hunting for lambdas by distance" << endl;
    }

    // return nullopt by default
    optional<int> iAssocJet = nullopt;

    // loop over jets
    double drAssocBest = numeric_limits<double>::max();
    for (size_t iJet = 0; iJet < m_jetInfo.size(); iJet++) {

      // calculate delta-phi
      double dfAssoc = lambda.GetPhi() - m_jetInfo[iJet].GetPhi();
      if (dfAssoc < 0.)             dfAssoc += TMath::TwoPi();
      if (dfAssoc > TMath::TwoPi()) dfAssoc -= TMath::TwoPi();

      // calculate dr
      const double dhAssoc = lambda.GetEta() - m_jetInfo[iJet].GetEta();
      const double drAssoc = hypot(dfAssoc, dhAssoc);

      // check if lambda is within rJet
      const bool isDistGood = (drAssoc < m_config.rJet);
      const bool isDistBest = (drAssoc < drAssocBest);
      if (isDistGood && isDistBest) {
        drAssocBest = drAssoc;
        iAssocJet   = iJet;
      }
    }  // end jet loop
    return iAssocJet;

  }  // end 'HuntLambdasByDistance(ParInfo& lambda)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

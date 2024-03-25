// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.evt.h'
// Derek Anderson
// 03.28.2023
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

  // event methods ------------------------------------------------------------

  bool SCorrelatorJetTree::IsGoodVertex(const CLHEP::Hep3Vector vtx) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::IsGoodVertex(CLHEP::Hep3Vector) Checking if event is good..." << endl;
    }

    // calculate vr
    const double vr = sqrt((vtx.x() * vtx.x()) + (vtx.y() * vtx.y()));

    // check if event is good
    const bool isInEvtVzRange = ((vtx.z() > m_evtVzRange[0]) && (vtx.z() < m_evtVzRange[1]));
    const bool isInEvtVrRange = ((abs(vr) > m_evtVrRange[0]) && (abs(vr) < m_evtVrRange[1]));
    const bool isGoodVertex   = (isInEvtVzRange && isInEvtVrRange);
    return isGoodVertex;

  }  // end 'IsGoodVertex(CLHEP::Hep3Vector)'



  void SCorrelatorJetTree::GetEventVariables(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetEventVariables(PHCompositeNode*) Grabbing event info..." << endl;
    }

    m_recoVtx     = GetRecoVtx(topNode);
    m_recoNumTrks = GetNumTrks(topNode);
    m_recoSumECal = GetSumECalEne(topNode);
    m_recoSumHCal = GetSumHCalEne(topNode);
    if (m_isMC) {
      m_trueNumChrgPars = GetNumChrgPars(topNode);
      m_trueSumPar      = GetSumParEne(topNode);
    }
    return;

  }  // end 'GetEventVariables(PHCompositeNode*)'



  void SCorrelatorJetTree::GetPartonInfo(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetPartonInfo(PHCompositeNode*) Grabbing parton info..." << endl;
    }

    // pick out relevant sub-sevent to grab
    int iPartonEvt = 1;
    if (m_isEmbed) {
      iPartonEvt = 2;
    }

    // loop over particles
    unsigned int     iOutPart = 0;
    HepMC::GenEvent* mcEvt    = GetMcEvent(topNode, iPartonEvt);
    for (HepMC::GenEvent::particle_const_iterator itPar = mcEvt -> particles_begin(); itPar != mcEvt -> particles_end(); ++itPar) {

      // check if outputing parton
      const bool isOutParton      = IsOutgoingParton(*itPar);
      const bool foundBothPartons = (iOutPart == 2);
      if (!isOutParton || foundBothPartons) continue;

      // grab parton info
      const double parPx = (*itPar) -> momentum().px();
      const double parPy = (*itPar) -> momentum().py();
      const double parPz = (*itPar) -> momentum().pz();
      const double parVx = (*itPar) -> production_vertex() -> position().x();
      const double parVy = (*itPar) -> production_vertex() -> position().y();
      const double parVz = (*itPar) -> production_vertex() -> position().z();

      // record ids, momentum, and vtx
      m_partonID[iOutPart]  = (*itPar) -> pdg_id();
      m_partonMom[iOutPart] = CLHEP::Hep3Vector(parPx, parPy, parPz);
      m_trueVtx             = CLHEP::Hep3Vector(parVx, parVy, parVz);
      ++iOutPart;
    }  // end particle loop
    return;

  }  // end 'GetPartonInfo(PHCompositeNode*)'



  long SCorrelatorJetTree::GetNumTrks(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetNumTrks(PHCompositeNode*) Calculating no. of tracks..." << endl;
    }

    // loop over tracks
    long          nTrk    = 0;
    SvtxTrack*    track   = NULL;
    SvtxTrackMap* mapTrks = GetTrackMap(topNode);
    for (SvtxTrackMap::Iter itTrk = mapTrks -> begin(); itTrk != mapTrks -> end(); ++itTrk) {

      // get track
      track = itTrk -> second;
      if (!track) continue;

      // if good, add to count
      const bool isGoodTrack = IsGoodTrack(track, topNode);
      if (isGoodTrack) ++nTrk;

    }  // end track loop
    return nTrk;

  }  // end 'GetNumTrks(PHCompositeNode*)'



  long SCorrelatorJetTree::GetNumChrgPars(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetNumChrgPars(PHCompositeNode*) Calculating no. of charged particles..." << endl;
    }

    // loop over subevents
    long nPar = 0;
    for (const int evtToGrab : m_vecEvtsToGrab) {

      // loop over particles
      HepMC::GenEvent* mcEvt = GetMcEvent(topNode, evtToGrab);
      for (HepMC::GenEvent::particle_const_iterator itPar = mcEvt -> particles_begin(); itPar != mcEvt -> particles_end(); ++itPar) {

        // check if particle is final state
        const bool isFinalState = ((*itPar) -> status() == 1);
        if (!isFinalState) continue;

        // if good, add to count
        const bool isGoodPar = IsGoodParticle(*itPar);
        if (isGoodPar) ++nPar;

      }  // end particle loop
    }  // end subevent loop
    return nPar;

  }  // end 'GetNumChrgPars(PHCompositeNode*)'



  double SCorrelatorJetTree::GetSumECalEne(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetSumECalEne(PHCompositeNode*) Getting sum of ECal energy..." << endl;
    }

    // grab vertex and clusters
    GlobalVertex*        vtx          = GetGlobalVertex(topNode);
    RawClusterContainer* emClustStore = GetClusterStore(topNode, "CLUSTER_CEMC");

    // loop over emcal clusters
    double                             eneECalSum   = 0.;
    RawClusterContainer::ConstRange    emClustRange = emClustStore -> getClusters();
    RawClusterContainer::ConstIterator itEMClust;
    for (itEMClust = emClustRange.first; itEMClust != emClustRange.second; itEMClust++) {

      // grab cluster
      const RawCluster* emClust = itEMClust -> second;
      if (!emClust) continue;

      // construct vertex and get 4-momentum
      const double vX = vtx -> get_x();
      const double vY = vtx -> get_y();
      const double vZ = vtx -> get_z();

      CLHEP::Hep3Vector hepVecVtx     = CLHEP::Hep3Vector(vX, vY, vZ);
      CLHEP::Hep3Vector hepVecEMClust = RawClusterUtility::GetECoreVec(*emClust, hepVecVtx);

      // if good, add to sum
      const bool isGoodECal = IsGoodECal(hepVecEMClust);
      if (isGoodECal) eneECalSum += hepVecEMClust.mag();

    }  // end emcal loop
    return eneECalSum;

  }  // end 'GetSumECalEne(PHCompositeNode*)'



  double SCorrelatorJetTree::GetSumHCalEne(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetSumHCalEne(PHCompositeNode*) Getting sum of HCal energy..." << endl;
    }

    // grab vertex and clusters
    GlobalVertex*        vtx          = GetGlobalVertex(topNode);
    RawClusterContainer* ihClustStore = GetClusterStore(topNode, "CLUSTER_HCALIN");
    RawClusterContainer* ohClustStore = GetClusterStore(topNode, "CLUSTER_HCALOUT");

    // loop over ihcal clusters
    double                             eneIHCalSum  = 0.;
    RawClusterContainer::ConstRange    ihClustRange = ihClustStore -> getClusters();
    RawClusterContainer::ConstIterator itIHClust;
    for (itIHClust = ihClustRange.first; itIHClust != ihClustRange.second; itIHClust++) {

      // grab cluster
      const RawCluster* ihClust = itIHClust -> second;
      if (!ihClust) continue;

      // construct vertex and get 4-momentum
      const double vX = vtx -> get_x();
      const double vY = vtx -> get_y();
      const double vZ = vtx -> get_z();

      CLHEP::Hep3Vector hepVecVtx     = CLHEP::Hep3Vector(vX, vY, vZ);
      CLHEP::Hep3Vector hepVecIHClust = RawClusterUtility::GetECoreVec(*ihClust, hepVecVtx);

      // if good, add to sum
      const bool isGoodHCal = IsGoodECal(hepVecIHClust);
      if (isGoodHCal) eneIHCalSum += hepVecIHClust.mag();
    }  // end ihcal loop

    // loop over ohcal clusters
    double                             eneOHCalSum  = 0.;
    RawClusterContainer::ConstRange    ohClustRange = ohClustStore -> getClusters();
    RawClusterContainer::ConstIterator itOHClust;
    for (itOHClust = ohClustRange.first; itOHClust != ohClustRange.second; itOHClust++) {

      // grab cluster
      const RawCluster* ohClust = itOHClust -> second;
      if (!ohClust) continue;

      // construct vertex and get 4-momentum
      const double vX = vtx -> get_x();
      const double vY = vtx -> get_y();
      const double vZ = vtx -> get_z();

      CLHEP::Hep3Vector hepVecVtx     = CLHEP::Hep3Vector(vX, vY, vZ);
      CLHEP::Hep3Vector hepVecOHClust = RawClusterUtility::GetECoreVec(*ohClust, hepVecVtx);

      // if good, add to sum
      const bool isGoodHCal = IsGoodECal(hepVecOHClust);
      if (isGoodHCal) eneOHCalSum += hepVecOHClust.mag();
    }  // end ohcal loop

    const double eneHCalSum = eneIHCalSum + eneOHCalSum;
    return eneHCalSum;

  }  // end 'GetSumHCalEne(PHCompositeNode*)'



  double SCorrelatorJetTree::GetSumParEne(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetSumParEne(PHComposite*) Calculating sum of particle energy..." << endl;
    }

    // loop over subevents
    double eSumPar = 0.;
    for (const int evtToGrab : m_vecEvtsToGrab) {

      HepMC::GenEvent* mcEvt = GetMcEvent(topNode, evtToGrab);
      for (HepMC::GenEvent::particle_const_iterator itPar = mcEvt -> particles_begin(); itPar != mcEvt -> particles_end(); ++itPar) {

        // check if particle is final state
        const bool isFinalState = ((*itPar) -> status() == 1);
        if (!isFinalState) continue;

        // if good, add to count
        const bool isGoodPar = IsGoodParticle(*itPar, true);
        if (isGoodPar) eSumPar += (*itPar) -> momentum().e();

      }  // end particle loop
    }  // end subevent loop
    return eSumPar;

  }  // end 'GetSumParEne(PHCompositeNode*)'



  CLHEP::Hep3Vector SCorrelatorJetTree::GetRecoVtx(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetRecoVtx(PHComposite*) Getting reconstructed vertex..." << endl;
    }

    const GlobalVertex*     vtx     = GetGlobalVertex(topNode);
    const double            vtxX    = vtx -> get_x();
    const double            vtxY    = vtx -> get_y();
    const double            vtxZ    = vtx -> get_z();
    const CLHEP::Hep3Vector recoVtx = CLHEP::Hep3Vector(vtxX, vtxY, vtxZ);
    return recoVtx;

  }  // end 'GetRecoVtx(PHCompositeNode*)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

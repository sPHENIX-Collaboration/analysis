// 'SCorrelatorJetTree.constituents.h'
// Derek Anderson
// 01.18.2023
//
// Class to construct a tree of
// jets from a specified set of
// events.
//
// Methods relevant to the jet
// constituents are collected
// here.
//
// Derived from code by Antonio
// Silva (thanks!!)

#pragma once

using namespace std;
using namespace findNode;



bool SCorrelatorJetTree::isAcceptableParticleFlow(ParticleFlowElement *pfPart) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::isAcceptableParticleFlow(ParticleFlowElement*) Checking if particle flow element is good..." << endl;
  }

  // TODO: explore particle flow cuts
  const double pfEta         = pfPart -> get_eta();
  const bool   isInEtaRange  = ((pfEta > m_particleflow_mineta) && (pfEta < m_particleflow_maxeta));
  const bool   isGoodElement = isInEtaRange;
  return isGoodElement;

}  // end 'isAcceptableParticleFlow(ParticleFlowElement*)'



bool SCorrelatorJetTree::isAcceptableTrack(SvtxTrack *track) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::isAcceptableTrack(SvtxTrack*) Checking if track is good..." << endl;
  }

  const double trkPt        = track -> get_pt();
  const double trkEta       = track -> get_eta();
  const bool   isInPtRange  = ((trkPt > m_track_minpt)   && (trkPt < m_track_maxpt));
  const bool   isInEtaRange = ((trkEta > m_track_mineta) && (trkEta < m_track_maxeta));
  const bool   isGoodTrack  = (isInPtRange && isInEtaRange);
  return isGoodTrack;

}  // end 'isAcceptableTrack(SvtxTrack*)'



bool SCorrelatorJetTree::isAcceptableEMCalCluster(CLHEP::Hep3Vector &E_vec_cluster) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::isAcceptableEMCalCluster(CLHEP::Hep3Vector&) Checking if ECal cluster is good..." << endl;
  }

  const double clustPt      = E_vec_cluster.perp();
  const double clustEta     = E_vec_cluster.pseudoRapidity();
  const bool   isInPtRange  = ((clustPt > m_EMCal_cluster_minpt)   && (clustPt < m_EMCal_cluster_maxpt));
  const bool   isInEtaRange = ((clustEta > m_EMCal_cluster_mineta) && (clustEta < m_EMCal_cluster_maxeta));
  const bool   isGoodClust  = (isInPtRange && isInEtaRange);
  return isGoodClust;

}  // end 'isAcceptableEMCalCluster(CLHEP::Hep3Vector&)'



bool SCorrelatorJetTree::isAcceptableHCalCluster(CLHEP::Hep3Vector &E_vec_cluster) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::isAcceptableHCalCluster(CLHEP::Hep3Vector&) Checking if HCal cluster is good..." << endl;
  }

  // TODO: explore particle cuts. These should vary with particle charge/species.
  const double clustPt      = E_vec_cluster.perp();
  const double clustEta     = E_vec_cluster.pseudoRapidity();
  const bool   isInPtRange  = ((clustPt > m_HCal_cluster_minpt)   && (clustPt < m_HCal_cluster_maxpt));
  const bool   isInEtaRange = ((clustEta > m_HCal_cluster_mineta) && (clustEta < m_HCal_cluster_maxeta));
  const bool   isGoodClust  = (isInPtRange && isInEtaRange);
  return isGoodClust;

}  // end 'isAcceptableHCalCluster(CLHEP::Hep3Vector&)'



bool SCorrelatorJetTree::isAcceptableParticle(HepMC::GenParticle *part) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::isAcceptableParticle(HepMC::GenParticle*) Checking if MC particle is good..." << endl;
  }

  const double parEta       = part -> momentum().eta();
  const double parPx        = part -> momentum().px();
  const double parPy        = part -> momentum().py();
  const double parPt        = sqrt((parPx * parPx) + (parPy * parPy));
  const bool   isInPtRange  = ((parPt > m_MC_particle_minpt)   && (parPt < m_MC_particle_maxpt));
  const bool   isInEtaRange = ((parEta > m_MC_particle_mineta) && (parEta < m_MC_particle_maxeta));
  const bool   isGoodPar    = (isInPtRange && isInEtaRange);
  return isGoodPar;

}  // end 'isAcceptableParticle(HepMC::GenParticle*)'

// end ------------------------------------------------------------------------

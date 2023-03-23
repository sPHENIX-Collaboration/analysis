// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.io.h'
// Derek Anderson
// 12.04.2022
//
// Class to construct a tree of
// jets from a specified set of
// events.
//
// Methods relevant to i/o
// operations are collected
// here.
//
// Derived from code by Antonio
// Silva (thanks!!)
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace findNode;



// i/o methods ---------------------------------------------------------------- 

void SCorrelatorJetTree::setParticleFlowEtaAcc(double etamin, double etamax) {

  m_particleflow_mineta = etamin;
  m_particleflow_maxeta = etamax;
  return;

}  // end 'setParticleFlowEtaAcc(double, double)'



void SCorrelatorJetTree::setTrackPtAcc(double ptmin, double ptmax) {

  m_track_minpt = ptmin;
  m_track_maxpt = ptmax;
  return;

}  // end 'setTrackPtAcc(double, double)'



void SCorrelatorJetTree::setTrackEtaAcc(double etamin, double etamax) {

  m_track_mineta = etamin;
  m_track_maxeta = etamax;
  return;

}  // end 'setTrackEtaAcc(double, double)'



void SCorrelatorJetTree::setEMCalClusterPtAcc(double ptmin, double ptmax) {

  m_EMCal_cluster_minpt = ptmin;
  m_EMCal_cluster_maxpt = ptmax;
  return;

}  // end 'setEMCalClusterPtAcc(double, double)'



void SCorrelatorJetTree::setEMCalClusterEtaAcc(double etamin, double etamax) {

  m_EMCal_cluster_mineta = etamin;
  m_EMCal_cluster_maxeta = etamax;
  return;

}  // end 'setEMCalClusterEtaAcc(double, double)'



void SCorrelatorJetTree::setHCalClusterPtAcc(double ptmin, double ptmax) {

  m_HCal_cluster_minpt = ptmin;
  m_HCal_cluster_maxpt = ptmax;
  return;

}  // end 'setHCalClusterPtAcc(double, double)'



void SCorrelatorJetTree::setHCalClusterEtaAcc(double etamin, double etamax) {

  m_HCal_cluster_mineta = etamin;
  m_HCal_cluster_maxeta = etamax;
  return;

}  // end 'setHCalClusterEtaAcc(double, double)'



void SCorrelatorJetTree::setParticlePtAcc(double ptmin, double ptmax) {

  m_MC_particle_minpt = ptmin;
  m_MC_particle_maxpt = ptmax;
  return;

}  // end 'setParticlePtAcc(double, double)'



void SCorrelatorJetTree::setParticleEtaAcc(double etamin, double etamax) {

  m_MC_particle_mineta = etamin;
  m_MC_particle_maxeta = etamax;
  return;

}  // end 'setParticleEtaAcc(double, double)'




void SCorrelatorJetTree::setJetAlgo(ALGO jetalgo) {

  switch (jetalgo) {
    case ALGO::ANTIKT:
      m_jetalgo = fastjet::antikt_algorithm;
      break;
    case ALGO::KT:
      m_jetalgo = fastjet::kt_algorithm;
      break;
    case ALGO::CAMBRIDGE:
      m_jetalgo = fastjet::cambridge_algorithm;
      break;
    default:
      m_jetalgo = fastjet::antikt_algorithm;
      break;
  }
  return;

}  // end 'setJetAlgo(ALGO)'



void SCorrelatorJetTree::setRecombScheme(RECOMB recomb_scheme) {

  switch(recomb_scheme) {
    case RECOMB::E_SCHEME:
      m_recomb_scheme = fastjet::E_scheme;
      break;
    case RECOMB::PT_SCHEME:
      m_recomb_scheme = fastjet::pt_scheme;
      break;
    case RECOMB::PT2_SCHEME:
      m_recomb_scheme = fastjet::pt2_scheme;
      break;
    case RECOMB::ET_SCHEME:
      m_recomb_scheme = fastjet::Et_scheme;
      break;
    case RECOMB::ET2_SCHEME:
      m_recomb_scheme = fastjet::Et2_scheme;
      break;
    default:
      m_recomb_scheme = fastjet::E_scheme;
      break;
  }
  return;

}  // end 'setRecombScheme(RECOMB)'



void SCorrelatorJetTree::setJetParameters(double r, unsigned int type, ALGO jetalgo, RECOMB recomb_scheme) {

  setR(r);
  setType(type);
  setJetAlgo(jetalgo);
  setRecombScheme(recomb_scheme);
  return;

}  // end 'setJetParameters(double, ALGO, RECOMB)'

// end ------------------------------------------------------------------------

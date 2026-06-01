#include "SiliconSeedsAna.h"
#include "SiliconCaloTrack.h"
#include "SiliconCaloTrackMap.h"

#include <ffarawobjects/Gl1Packet.h>
#include <ffaobjects/EventHeader.h>
#include <trackbase/InttDefs.h>

#include <qautils/QAHistManagerDef.h>
#include <qautils/QAUtil.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <qautils/QAHistManagerDef.h>

#include <Acts/Surfaces/CylinderSurface.hpp>
#include <trackreco/ActsPropagator.h>
#include <trackbase_historic/ActsTransformations.h>
#include <trackbase_historic/SvtxTrackState_v1.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <g4eval/SvtxTrackEval.h>  // for SvtxTrackEval

#include <siliconseedsana/SiliconCaloTrack.h>
#include <siliconseedsana/SiliconCaloTrackMap.h>



#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include <TFile.h>
#include <TTree.h>

#include <algorithm>

// Add member variables for TTree and track data

//____________________________________________________________________________..
SiliconSeedsAna::SiliconSeedsAna(const std::string &name)
    : SubsysReco(name)
{
}

#define LOG(msg) std::cout << "[SiliconSeedsAna] " << msg << std::endl;

void SiliconSeedsAna::clearTrackVectors() {
  track_id.clear(); 
  track_px.clear(); track_py.clear(); track_pz.clear();
  track_x.clear(); track_y.clear(); track_z.clear();
  track_pt.clear(); track_phi.clear(); track_eta.clear(); 
  track_chi2ndf.clear(); 
  track_dxy.clear(); track_dz.clear(); 
  track_crossing.clear();
  track_charge.clear(); 
  track_nmaps.clear(); track_nintt.clear(); track_innerintt.clear(); track_outerintt.clear();
  track_x_emc.clear(); track_y_emc.clear(); track_z_emc.clear();
  track_x_oemc.clear(); track_y_oemc.clear(); track_z_oemc.clear();
  track_rv_x_emc.clear(); track_rv_y_emc.clear(); track_rv_z_emc.clear();
  track_px_emc.clear(); track_py_emc.clear(); track_pz_emc.clear();
  track_eta_emc.clear(); track_phi_emc.clear(); track_pt_emc.clear();
  track_id_truth.clear(); 

  // Clear matched calo vectors
  sicalo_pt.clear();
  sicalo_phi.clear();
  sicalo_eta.clear();
  sicalo_dphi.clear();
  sicalo_dz.clear();
  sicalo_emc_x.clear();
  sicalo_emc_y.clear();
  sicalo_emc_z.clear();
  sicalo_emc_r.clear();
  sicalo_emc_phi.clear();
  sicalo_emc_energy.clear();
  sicalo_emc_chi2.clear();
  sicalo_emc_prob.clear();
}

void SiliconSeedsAna::clearCaloVectors() {
  calo_x.clear();
  calo_y.clear();
  calo_z.clear();
  calo_r.clear();
  calo_phi.clear();
  calo_eta.clear();
  calo_energy.clear();
  calo_chi2.clear();
  calo_prob.clear();

    calo_tower_ieta.clear();
    calo_tower_iphi.clear();
    calo_tower_e.clear();
    calo_tower_x.clear();
    calo_tower_y.clear();
    calo_tower_z.clear();
    calo_tower_r.clear();
    calo_tower_eta.clear();
    calo_tower_phi.clear();
    calo_tower_time.clear();
}

void SiliconSeedsAna::fillEMCalState(SvtxTrackState* state, SvtxTrackState* ostate, SvtxTrackState* rvsState) {
  if(!state) { std::cout<<"No EMCState front"<<std::endl;}
  if(!ostate) { std::cout<<"No EMCState back"<<std::endl;}

  track_x_emc.push_back(  state ?  state->get_x()  : NAN);
  track_y_emc.push_back(  state ?  state->get_y()  : NAN);
  track_z_emc.push_back(  state ?  state->get_z()  : NAN);
  track_px_emc.push_back( state ?  state->get_px() : NAN);
  track_py_emc.push_back( state ?  state->get_py() : NAN);
  track_pz_emc.push_back( state ?  state->get_pz() : NAN);
  track_eta_emc.push_back(state ?  state->get_eta(): NAN);
  track_phi_emc.push_back(state ?  state->get_phi(): NAN);
  track_pt_emc.push_back( state ?  state->get_pt() : NAN);
  track_x_oemc.push_back(ostate ? ostate->get_x()  : NAN);
  track_y_oemc.push_back(ostate ? ostate->get_y()  : NAN);
  track_z_oemc.push_back(ostate ? ostate->get_z()  : NAN);
  track_rv_x_emc.push_back(rvsState ? rvsState->get_x()  : NAN);
  track_rv_y_emc.push_back(rvsState ? rvsState->get_y()  : NAN);
  track_rv_z_emc.push_back(rvsState ? rvsState->get_z()  : NAN);

/*
  // --- EMCal cluster matching ---
  // Find the closest EMCal cluster in eta/phi to this state
  float best_dR = 9999.0;
  int best_idx = -1;
  float state_eta = state ? state->get_eta() : NAN;
  float state_phi = state ? state->get_phi() : NAN;
  // Use calo_eta/calo_phi/calo_x/etc. as the cluster list
  for (size_t icl = 0; icl < calo_eta.size(); ++icl) {
    float deta = state_eta - calo_eta[icl];
    float dphi = state_phi - calo_phi[icl];
    // Wrap dphi into [-pi, pi]
    while (dphi > M_PI) dphi -= 2 * M_PI;
    while (dphi < -M_PI) dphi += 2 * M_PI;
    float dR = std::sqrt(deta * deta + dphi * dphi);
    if (dR < best_dR) {
      best_dR = dR;
      best_idx = icl;
    }
  }
  if (best_idx >= 0) {
    matched_calo_x.push_back(calo_x[best_idx]);
    matched_calo_y.push_back(calo_y[best_idx]);
    matched_calo_z.push_back(calo_z[best_idx]);
    matched_calo_r.push_back(calo_r[best_idx]);
    matched_calo_phi.push_back(calo_phi[best_idx]);
    matched_calo_eta.push_back(calo_eta[best_idx]);
    matched_calo_energy.push_back(calo_energy[best_idx]);
    matched_calo_dR.push_back(best_dR);
  } else {
    matched_calo_x.push_back(NAN);
    matched_calo_y.push_back(NAN);
    matched_calo_z.push_back(NAN);
    matched_calo_r.push_back(NAN);
    matched_calo_phi.push_back(NAN);
    matched_calo_eta.push_back(NAN);
    matched_calo_energy.push_back(NAN);
    matched_calo_dR.push_back(NAN);
  }
*/
}

void SiliconSeedsAna::initTrackTreeBranches() {
  trackTree = new TTree("trackTree", "Track Data");
  trackTree->Branch("evt", &evt, "evt/I");
  trackTree->Branch("track_id", &track_id);
  trackTree->Branch("x0", &track_x);
  trackTree->Branch("y0", &track_y);
  trackTree->Branch("z0", &track_z);
  trackTree->Branch("px0", &track_px);
  trackTree->Branch("py0", &track_py);
  trackTree->Branch("pz0", &track_pz);
  trackTree->Branch("eta0", &track_eta);
  trackTree->Branch("phi0", &track_phi);
  trackTree->Branch("pt0", &track_pt);
  trackTree->Branch("chi2ndf", &track_chi2ndf);
  trackTree->Branch("dxy", &track_dxy);
  trackTree->Branch("dz",  &track_dz);
  trackTree->Branch("charge", &track_charge);
  trackTree->Branch("nmaps", &track_nmaps);
  trackTree->Branch("nintt", &track_nintt);
  trackTree->Branch("innerintt", &track_innerintt);
  trackTree->Branch("outerintt", &track_outerintt);
  trackTree->Branch("crossing", &track_crossing);
  trackTree->Branch("x_proj_emc", &track_x_emc);
  trackTree->Branch("y_proj_emc", &track_y_emc);
  trackTree->Branch("z_proj_emc", &track_z_emc);
  trackTree->Branch("px_proj_emc", &track_px_emc);
  trackTree->Branch("py_proj_emc", &track_py_emc);
  trackTree->Branch("pz_proj_emc", &track_pz_emc);
  trackTree->Branch("eta_proj_emc", &track_eta_emc);
  trackTree->Branch("phi_proj_emc", &track_phi_emc);
  trackTree->Branch("pt_proj_emc", &track_pt_emc);
  trackTree->Branch("x_proj_oemc", &track_x_oemc);
  trackTree->Branch("y_proj_oemc", &track_y_oemc);
  trackTree->Branch("z_proj_oemc", &track_z_oemc);
  trackTree->Branch("x_rv_proj_emc", &track_rv_x_emc);
  trackTree->Branch("y_rv_proj_emc", &track_rv_y_emc);
  trackTree->Branch("z_rv_proj_emc", &track_rv_z_emc);
  trackTree->Branch("track_id_truth", &track_id_truth);
  // Add matched EMCal cluster branches
  trackTree->Branch("sicalo_pt",         &sicalo_pt);
  trackTree->Branch("sicalo_phi",        &sicalo_phi);
  trackTree->Branch("sicalo_eta",        &sicalo_eta);
  trackTree->Branch("sicalo_dphi",       &sicalo_dphi);
  trackTree->Branch("sicalo_dz",         &sicalo_dz);
  trackTree->Branch("sicalo_emc_x",      &sicalo_emc_x);
  trackTree->Branch("sicalo_emc_y",      &sicalo_emc_y);
  trackTree->Branch("sicalo_emc_z",      &sicalo_emc_z);
  trackTree->Branch("sicalo_emc_r",      &sicalo_emc_r);
  trackTree->Branch("sicalo_emc_phi",    &sicalo_emc_phi);
  trackTree->Branch("sicalo_emc_energy", &sicalo_emc_energy);
  trackTree->Branch("sicalo_emc_chi2",   &sicalo_emc_chi2);
  trackTree->Branch("sicalo_emc_prob",   &sicalo_emc_prob);
  trackTree->SetBasketSize("*", 50000);
}

void SiliconSeedsAna::initCaloTreeBranches() {
  caloTree = new TTree("caloTree", "Calo Data");
  caloTree->Branch("calo_evt", &calo_evt, "calo_evt/I");
  caloTree->Branch("x", &calo_x);
  caloTree->Branch("y", &calo_y);
  caloTree->Branch("z", &calo_z);
  caloTree->Branch("r", &calo_r);
  caloTree->Branch("phi", &calo_phi);
  caloTree->Branch("eta", &calo_eta);
  caloTree->Branch("energy", &calo_energy);
  caloTree->Branch("chi2",  &calo_chi2);
  caloTree->Branch("prob",   &calo_prob);

    caloTree->Branch("Calo_tower_ieta", &calo_tower_ieta);
    caloTree->Branch("Calo_tower_iphi", &calo_tower_iphi);
    caloTree->Branch("Calo_tower_e",    &calo_tower_e);
    caloTree->Branch("Calo_tower_x",    &calo_tower_x);
    caloTree->Branch("Calo_tower_y",    &calo_tower_y);
    caloTree->Branch("Calo_tower_z",    &calo_tower_z);
    caloTree->Branch("Calo_tower_r",    &calo_tower_r);
    caloTree->Branch("Calo_tower_eta",  &calo_tower_eta);
    caloTree->Branch("Calo_tower_phi",  &calo_tower_phi);
    caloTree->Branch("Calo_tower_time", &calo_tower_time);

  caloTree->SetBasketSize("*",50000);
}

//____________________________________________________________________________..
int SiliconSeedsAna::InitRun(PHCompositeNode *topNode )
{

  // init Unique pointer
  if (isMC && _doEval && !_svtxEvalStack)
  {
    _svtxEvalStack.reset(new SvtxEvalStack(topNode));
    _svtxEvalStack->set_strict(false);
    _svtxEvalStack->set_verbosity(Verbosity());
  }
  


  m_outfile = new TFile(m_outputfilename.c_str(), "RECREATE");
  
  createHistos();
  // Create a TTree to store track data and initialize branches
  initTrackTreeBranches();
  SiClusTree = new TTree("SiClusTree", "Silicon Cluster used by track");
  SiClusTree->Branch("evt", &evt, "evt/I");
  SiClusTree->Branch("Siclus_trackid", &SiClus_trackid);
  SiClusTree->Branch("Siclus_layer", &SiClus_layer);
  SiClusTree->Branch("Siclus_x", &SiClus_x);
  SiClusTree->Branch("Siclus_y", &SiClus_y);
  SiClusTree->Branch("Siclus_z", &SiClus_z);
  SiClusTree->Branch("Siclus_t", &SiClus_t);
  SiClusTree->SetBasketSize("*",50000); // Set a larger basket size for better performance

  SiClusAllTree = new TTree("SiClusAllTree", "All Silicon Cluster Data");
  SiClusAllTree->Branch("evt", &evt, "evt/I");
  SiClusAllTree->Branch("Siclus_trackid", &SiClus_trackid);
  SiClusAllTree->Branch("Siclus_layer", &SiClus_layer);
  SiClusAllTree->Branch("Siclus_x", &SiClus_x);
  SiClusAllTree->Branch("Siclus_y", &SiClus_y);
  SiClusAllTree->Branch("Siclus_z", &SiClus_z);
  SiClusAllTree->Branch("Siclus_t", &SiClus_t);
  SiClusAllTree->SetBasketSize("*",50000); // Set a larger basket size for better performance

  initCaloTreeBranches();

  evtTree = new TTree("evtTree", "Event Data");
  evtTree->Branch("evt",     &evt,       "evt/I");
  evtTree->Branch("caloevt", &calo_evt,  "caloevt/I");
  evtTree->Branch("evtseq",  &evt_evtseq,"evtseq/I");
  evtTree->Branch("bco",     &evt_bco,   "bco/l");
  evtTree->Branch("scaledtrig", &evt_scaledtrig,   "bco/l");
  evtTree->Branch("crossing",&evt_crossing, "crossing/I");
  evtTree->Branch("nintt",   &evt_nintt, "nintt/I");
  evtTree->Branch("nintt50", &evt_nintt50,"nintt50/I");
  evtTree->Branch("nmaps",   &evt_nmaps, "nmaps/I");
  evtTree->Branch("nemc",    &evt_nemc,  "nemc/I");
  evtTree->Branch("nemc02",  &evt_nemc02,"nemc02/I");
  evtTree->Branch("nsiseed", &evt_nsiseed, "nsiseed/I");
  evtTree->Branch("nsiseed0",&evt_nsiseed0,"nsiseed0/I");
  evtTree->Branch("xvtx",    &evt_xvtx,  "xvtx/F");
  evtTree->Branch("yvtx",    &evt_yvtx,  "yvtx/F");
  evtTree->Branch("zvtx",    &evt_zvtx,  "zvtx/F");
  evtTree->Branch("xgvtx",   &evt_xgvtx,  "xgvtx/F");
  evtTree->Branch("ygvtx",   &evt_ygvtx,  "ygvtx/F");
  evtTree->Branch("zgvtx",   &evt_zgvtx,  "zgvtx/F");
  evtTree->Branch("gvtx_type", &evt_gvtx_type);
  evtTree->Branch("gvtx_x",    &evt_gvtx_x);
  evtTree->Branch("gvtx_y",    &evt_gvtx_y);
  evtTree->Branch("gvtx_z",    &evt_gvtx_z);


  // Truth tree and branches
  if(isMC){
    truthTree = new TTree("truthTree", "Truth Particle Data");
    truthTree->Branch("truth_pid",        &truth_pid);
    truthTree->Branch("truth_id",         &truth_id); // for primary/secondary
    truthTree->Branch("truth_px",         &truth_px);
    truthTree->Branch("truth_py",         &truth_py);
    truthTree->Branch("truth_pz",         &truth_pz);
    truthTree->Branch("truth_x",          &truth_x);
    truthTree->Branch("truth_y",          &truth_y);
    truthTree->Branch("truth_z",          &truth_z);
    truthTree->Branch("truth_e",          &truth_e);
    truthTree->Branch("truth_pt",         &truth_pt);
    truthTree->Branch("truth_eta",        &truth_eta);
    truthTree->Branch("truth_phi",        &truth_phi);
    truthTree->Branch("truth_primary_id", &truth_primary_id);
    truthTree->Branch("truth_parent_id",  &truth_parent_id);
    truthTree->Branch("truth_vtxid",      &truth_vtxid);
    truthTree->Branch("truth_vtx_x", &truth_vtx_x);
    truthTree->Branch("truth_vtx_y", &truth_vtx_y);
    truthTree->Branch("truth_vtx_z", &truth_vtx_z);
    truthTree->SetBasketSize("*",50000); // Set a larger basket size for better performance
  }

    TopoClusTree = new TTree("TopoClusTree", "Topo cluster and associated tower info");

    TopoClusTree->Branch("evt", &evt, "evt/I");

    TopoClusTree->Branch("clus_e",   &topo_clus_e);
    TopoClusTree->Branch("clus_x",   &topo_clus_x);
    TopoClusTree->Branch("clus_y",   &topo_clus_y);
    TopoClusTree->Branch("clus_z",   &topo_clus_z);
    TopoClusTree->Branch("clus_r",   &topo_clus_r);
    TopoClusTree->Branch("clus_eta", &topo_clus_eta);
    TopoClusTree->Branch("clus_phi", &topo_clus_phi);

    TopoClusTree->Branch("clus_chi2", &topo_clus_chi2);
    TopoClusTree->Branch("clus_prob", &topo_clus_prob);

    TopoClusTree->Branch("clus_emcal_e", &topo_clus_emcal_e);
    TopoClusTree->Branch("clus_ihcal_e", &topo_clus_ihcal_e);
    TopoClusTree->Branch("clus_ohcal_e", &topo_clus_ohcal_e);

    TopoClusTree->Branch("tower_caloid", &topo_tower_caloid);
    TopoClusTree->Branch("tower_ieta",   &topo_tower_ieta);
    TopoClusTree->Branch("tower_iphi",   &topo_tower_iphi);
    TopoClusTree->Branch("tower_e",      &topo_tower_e);
    TopoClusTree->Branch("tower_x",      &topo_tower_x);
    TopoClusTree->Branch("tower_y",      &topo_tower_y);
    TopoClusTree->Branch("tower_z",      &topo_tower_z);
    TopoClusTree->Branch("tower_eta",    &topo_tower_eta);
    TopoClusTree->Branch("tower_phi",    &topo_tower_phi);
    TopoClusTree->Branch("tower_time",   &topo_tower_time);

    TopoClusTree->SetBasketSize("*", 50000);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SiliconSeedsAna::process_event(PHCompositeNode* topNode)
{
 
  if(isMC)
    fillTruthTree(topNode);

  processVertexMap(topNode);
  processCaloClusters(topNode);
  processTrackMap(topNode);
  processSiCluster(topNode);

    processHCalinfo(topNode);


  //////////////
  {
    // reset in processTrackMap : evt_nsiseed=evt_nsiseed0=0;
    // reset in processSiCluster :  evt_nintt = evt_nintt50 = evt_nmaps = 0;
    evt_bco = 0;
    evt_crossing   = std::numeric_limits<int>::signaling_NaN();
    evt_scaledtrig = std::numeric_limits<int>::signaling_NaN();
    evt_evtseq     = std::numeric_limits<int>::signaling_NaN();

    auto gl1        = findNode::getClass<Gl1Packet>(  topNode, "GL1Packet");
    auto evthdr     = findNode::getClass<EventHeader>(topNode, "EventHeader");
    //auto clustermap = findNode::getClass<TrkrClusterContainer>(topNode, m_clusterContainerName);

    if(gl1){
      evt_bco        = gl1->getBCO();
      evt_crossing   = gl1->getBunchNumber();
      evt_scaledtrig = gl1->getScaledVector();

    }
    else { std::cout<<"No GL1Packet"<<std::endl; }

    if(evthdr){
      evt_evtseq = evthdr->get_EvtSequence();
    }
    else { std::cout<<"No EventHeader"<<std::endl; }

    std::cout<<"BCO : 0x"<<std::hex<<evt_bco<<std::dec
             <<", crossing : "<<evt_crossing
             <<", EvtSeq : "<<evt_evtseq<<std::endl;

    //--std::cout<<"nintt "<<evt_nintt<<" "<<evt_nintt50<<std::endl;
    //--std::cout<<"nmvtx "<<evt_nmaps<<std::endl;

    evtTree->Fill();
  }
  //////////////

  return Fun4AllReturnCodes::EVENT_OK;
}

void SiliconSeedsAna::fillTruthTree(PHCompositeNode* topNode)
{
  PHG4TruthInfoContainer* m_truth_info = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!m_truth_info)
  {
    std::cout << PHWHERE << "Warning: G4TruthInfo not found. Skipping truthTree filling for this event." << std::endl;
    return;
  }
  truth_pid.clear(); truth_id.clear();
  truth_px.clear(); truth_py.clear(); truth_pz.clear();
  truth_x.clear(); truth_y.clear(); truth_z.clear();
  truth_pt.clear(); truth_eta.clear(); truth_phi.clear(); truth_e.clear();
  truth_primary_id.clear(); truth_parent_id.clear();
  truth_vtxid.clear();
  truth_vtx_x.clear(); truth_vtx_y.clear(); truth_vtx_z.clear();

  std::cout<<"truth"<<std::endl;
  const auto  vtxrng = m_truth_info->GetPrimaryVtxRange();
  for (auto viter = vtxrng.first; viter != vtxrng.second; ++viter)
  {
    auto Vtx = viter->second;
    truth_vtx_x.push_back(Vtx->get_x());
    truth_vtx_y.push_back(Vtx->get_y());
    truth_vtx_z.push_back(Vtx->get_z());
    std::cout<<" primVtxid: "<<viter->first<<", "<<Vtx->get_x()<<" "<<Vtx->get_y()<<" "<<Vtx->get_z()<<std::endl;
  }


  //const auto prange = m_truth_info->GetPrimaryParticleRange();
  const auto prange = m_truth_info->GetParticleRange();
  for (auto iter = prange.first; iter != prange.second; ++iter)
  {
    PHG4Particle* ptcl = iter->second;
    if (!ptcl) continue;

    int vtxid = ptcl->get_vtx_id();
    //auto primVtx = m_truth_info->GetPrimaryVtx(vtxid);
    auto primVtx = m_truth_info->GetVtx(vtxid);
    float vx = primVtx->get_x();
    float vy = primVtx->get_y();
    float vz = primVtx->get_z();
    float vtx_r = sqrt(vx*vx+vy*vy);
    if(vtx_r>5) continue;

    //--int trackid = ptcl->get_track_id();
    //--int pid     = ptcl->get_pid();
    //--if(!(
    //--      abs(pid)==11 // e-
    //--   || abs(pid)==13 // mu-
    //--   || abs(pid)==211 // pi+
    //--   || abs(pid)==321 // K+
    //--   || abs(pid)==2212 // p
    //--   )
    //--   &&trackid<=0
    //--){
    //--  continue;
    //--}



    //std::cout<<" vtxid: "<<vtxid<<", "<<primVtx->get_x()<<" "<<primVtx->get_y()<<" "<<primVtx->get_z()<<std::endl;

    TLorentzVector p(ptcl->get_px(), ptcl->get_py(), ptcl->get_pz(), ptcl->get_e());

    truth_pid.push_back(ptcl->get_pid());
    truth_id .push_back(ptcl->get_track_id());
    truth_px .push_back(ptcl->get_px());
    truth_py .push_back(ptcl->get_py());
    truth_pz .push_back(ptcl->get_pz());
    truth_x  .push_back(vx);
    truth_y  .push_back(vy);
    truth_z  .push_back(vz);
    truth_e  .push_back(ptcl->get_e());
    truth_pt .push_back(p.Pt());
    truth_eta.push_back(p.Eta());
    truth_phi.push_back(p.Phi());
    truth_vtxid.push_back(vtxid);
    truth_primary_id.push_back(ptcl->get_primary_id());
    truth_parent_id .push_back(ptcl->get_parent_id());

    //std::cout<<"pid :"<<ptcl->get_pid()<<", "<<ptcl->get_name()<<", "<<ptcl->get_primary_id()<<", "<<ptcl->get_parent_id()<<", "<<ptcl->get_track_id()<<std::endl;
  }
  truthTree->Fill();
}

void SiliconSeedsAna::processTrackMap(PHCompositeNode* topNode)
{
  evt_nsiseed=evt_nsiseed0=0;

  auto clustermap = findNode::getClass<TrkrClusterContainer>(topNode, m_clusterContainerName);
  auto geometry   = findNode::getClass<ActsGeometry>(topNode, m_actsgeometryName);
  auto vertexmap  = findNode::getClass<SvtxVertexMap>(topNode, m_vertexMapName);
  auto emcalClusmap = findNode::getClass<RawClusterContainer>(topNode, m_emcalClusName);
  auto sicalotrackmap = findNode::getClass<SiliconCaloTrackMap>(topNode, "SiliconCaloTrack");

  trackmap = findNode::getClass<SvtxTrackMap>(topNode, m_trackMapName);
  if (!trackmap)
  {
    std::cout << PHWHERE << "Missing trackmap, can't continue" << std::endl;
    return;
  }
  if (!clustermap)
  {
    std::cout << PHWHERE << "Missing clustermap, can't continue" << std::endl;
    return;
  }
  if (!geometry)
  {
    std::cout << PHWHERE << "Missing geometry, can't continue" << std::endl;
    return;
  }
  if (!vertexmap)
  {
    std::cout << PHWHERE << "Missing vertexmap, can't continue" << std::endl;
    return;
  }
  if (!sicalotrackmap)
  {
    std::cout << PHWHERE << "Missing sicalotrackkmap" << std::endl;
  }




  SvtxTrackEval *trackeval = nullptr;
  if(isMC&&_doEval){
    _svtxEvalStack->next_event(topNode);

    trackeval = _svtxEvalStack->get_track_eval();
    if(trackeval==nullptr) std::cout<<"No TrackEval"<<std::endl;
  }


  if((evt%1000)==0) std::cout << "start track map  EVENT " << evt << " is OK" << std::endl;

  //-----------------------------
  // making reverse pointer from siliconCaloTrack to SvtxTrack
  // this should be replaced by iterator-method in SiliconCaloTrackMap
  std::map<int, int> sicaloTrackMap;
  if(sicalotrackmap){
    for (auto &itr_sicalo : *sicalotrackmap)
    {
      int idx = itr_sicalo.first;
      SiliconCaloTrack* sicalotrack = itr_sicalo.second;
      sicaloTrackMap.insert(std::make_pair(sicalotrack->get_id(), idx));
    }
  }


  evt_nsiseed=trackmap->size();
  h_ntrack1d->Fill(trackmap->size());

  std::pair<int, int> ntrack_isfromvtx;   // first: number of tracks associated to a vertex, second: number of tracks not associated to a vertex
  std::pair<int, int> ntrack_isposcharge; // first: number of tracks with negative charge, second: number of tracks with positive charge

  //--LOG("Start track map for EVENT " << evt);
  clearTrackVectors();

  SiClus_trackid.clear();
  SiClus_layer.clear();
  SiClus_x.clear();
  SiClus_y.clear();
  SiClus_z.clear();
  SiClus_t.clear();

  for (auto &iter : *trackmap)
  {
    track = iter.second;
    if (!track)
      continue;
    si_seed = track->get_silicon_seed();
    int trkcrossing = track->get_crossing();
    if (trkcrossing != 0 && !isMC)
      continue;

    int   t_id      = track->get_id();
    float t_eta     = track->get_eta();
    float t_phi     = track->get_phi();
    float t_pt      = track->get_pt();
    int   t_charge  = track->get_charge();
    float t_chi2ndf = track->get_quality();
    float t_x       = track->get_x();
    float t_y       = track->get_y();
    float t_z       = track->get_z();
    float t_px      = track->get_px();
    float t_py      = track->get_py();
    float t_pz      = track->get_pz();
    float t_dca2d   = track->get_dca2d();
    float t_dca3d_xy= track->get_dca3d_xy();
    float t_dca3d_z = track->get_dca3d_z();

    float t_dxy    = sqrt(pow(evt_xgvtx-t_x, 2) + pow(evt_ygvtx-t_y, 2));
    float t_dz     = evt_zgvtx-t_z;


    int t_crossing  = trkcrossing;
    if(t_crossing==0) evt_nsiseed0++;

    int t_nmaps = 0, t_nintt = 0, t_inner = 0, t_outer = 0;

    if (!si_seed)
    {
      track_nmaps.push_back(0);
      track_nintt.push_back(0);
      track_innerintt.push_back(0);
      track_outerintt.push_back(0);
    }
    else
    {
      for (auto key_iter = si_seed->begin_cluster_keys(); key_iter != si_seed->end_cluster_keys(); ++key_iter)
      {
        const auto &cluster_key = *key_iter;
        trkrCluster = clustermap->findCluster(cluster_key);
        if (!trkrCluster)
        {
          continue;
        }
        if (TrkrDefs::getTrkrId(cluster_key) == TrkrDefs::TrkrId::mvtxId)
        {
          t_nmaps++;
        }
        if (TrkrDefs::getTrkrId(cluster_key) == TrkrDefs::TrkrId::inttId)
        {
          t_nintt++;
        }
        Acts::Vector3 global(0., 0., 0.);
        global = geometry->getGlobalPosition(cluster_key, trkrCluster);
        SiClus_trackid.push_back(track->get_id());
        SiClus_x.push_back(global[0]);
        SiClus_y.push_back(global[1]);
        SiClus_z.push_back(global[2]);
        int layer = TrkrDefs::getLayer(cluster_key);
        h_nlayer->Fill(layer);
        if (layer == 3 || layer == 4)
          t_inner++;
        if (layer == 5 || layer == 6)
          t_outer++;
        SiClus_layer.push_back(layer);

        int timebkt=-9999;
        if(layer<3){
          timebkt = MvtxDefs::getStrobeId(cluster_key);
        }
        else {
          timebkt = InttDefs::getTimeBucketId(cluster_key);
        }
        SiClus_t.push_back(timebkt);
      }
      track_nmaps.push_back(t_nmaps);
      track_nintt.push_back(t_nintt);
      track_innerintt.push_back(t_inner);
      track_outerintt.push_back(t_outer);
    }
    track_id.push_back(t_id);
    track_x.push_back(t_x);
    track_y.push_back(t_y);
    track_z.push_back(t_z);
    track_eta.push_back(t_eta);
    track_phi.push_back(t_phi);
    track_pt.push_back(t_pt);
    track_px.push_back(t_px);
    track_py.push_back(t_py);
    track_pz.push_back(t_pz);
    track_chi2ndf.push_back(t_chi2ndf);
    track_dxy.push_back(t_dxy);
    track_dz.push_back(t_dz);

    track_charge.push_back(t_charge);
    track_crossing.push_back(t_crossing);
    if (false)
      std::cout << "track_x : " << t_x << ", track_y: " << t_y << ", track_z: " << t_z 
                << ", track_eta: " << t_eta << ", track_phi: " << t_phi << ", track_pt: " << t_pt
                << ", dca2d: " << t_dca2d << ", dca2d_xy: " << t_dca3d_xy << ", dca3d_z: " << t_dca3d_z 
                << ", dxy,dz=" << t_dxy << " " << t_dz << std::endl;

    SvtxTrackState *emcalState    = track->get_state(_caloRadiusEMCal);
    SvtxTrackState *emcalOutState = track->get_state(_caloRadiusEMCal+_caloThicknessEMCal);

    SvtxTrack *trkcp = (SvtxTrack*)track->CloneMe();
    trkcp->set_charge((t_charge>0)?-1:1); // reverse
    SvtxTrackState *rvsEmcalState = projectToEMCal(topNode, trkcp);

    fillEMCalState(emcalState, emcalOutState, rvsEmcalState);

    // trackeval for MC
    int g4part_id = std::numeric_limits<int>::max(); 
    if(isMC&&_doEval)
    {
      bool printMC=true; //false;
      if(trackeval!=nullptr)
      {
        auto *g4particle_match = trackeval->max_truth_particle_by_nclusters(track);
        if(printMC) std::cout<<"--- trackeval ---"<<std::endl;
        if (g4particle_match)
        {
          if(printMC) std::cout<<" g4 particle found"<<std::endl;
          SvtxTrack *matched_track = trackeval->best_track_from(g4particle_match);
          if (matched_track)
          {
            if(printMC) std::cout<<"   matched track found"<<std::endl;
            if (matched_track->get_id() == track->get_id())
            {
              if(printMC) {
                std::cout<<"     Eval matched track id confirmed"<<std::endl;
                g4particle_match->identify();
              }
              g4part_id = g4particle_match->get_track_id();
            }
          }
        }
      }
      std::cout<<" Eval matched g4part id "<<
      ((g4part_id == std::numeric_limits<int>::max()) ? "not found" : "confirmed")
       <<std::endl;
    }
    track_id_truth.push_back(g4part_id);

    /////////////////////////
    // --- matched cluster
    float sic_pt=NAN, sic_phi=NAN, sic_eta=NAN, sic_dphi=NAN, sic_dz=NAN;
    float sic_calo_x=NAN, sic_calo_y=NAN, sic_calo_z=NAN;
    float sic_calo_r=NAN, sic_calo_phi=NAN, sic_calo_e=NAN, sic_calo_chi2=NAN, sic_calo_prob=NAN;
    if(sicalotrackmap && emcalClusmap)
    {

      auto sicalo_itr = sicaloTrackMap.find(track->get_id());
      if(sicalo_itr!=sicaloTrackMap.end()) {
        SiliconCaloTrack* sicalo = sicalotrackmap->get(sicalo_itr->second);
        if(sicalo){
          sic_pt   = sicalo->get_pt();
          sic_phi  = sicalo->get_phi();
          sic_eta  = sicalo->get_eta();
          sic_dphi = sicalo->get_cal_dphi();
          sic_dz   = sicalo->get_cal_dz();

          int calo_id = sicalo->get_calo_id();
          RawCluster *calo = emcalClusmap->getCluster(calo_id);
          if(calo){
            getCaloPosition(calo, sic_calo_x, sic_calo_y, sic_calo_z);
            sic_calo_r    = calo->get_r();
            sic_calo_phi  = calo->get_phi();
            sic_calo_e    = calo->get_energy();
            sic_calo_chi2 = calo->get_chi2();
            sic_calo_prob = calo->get_prob();
            //--std::cout<<"calo_chi2 : "<<sic_calo_chi2<<" "<<sic_calo_prob<<std::endl;
          } 
        }
      }
    }

    sicalo_pt.push_back(sic_pt);
    sicalo_phi.push_back(sic_phi);
    sicalo_eta.push_back(sic_eta);
    sicalo_dphi.push_back(sic_dphi);
    sicalo_dz.push_back(sic_dz);
    sicalo_emc_x.push_back(sic_calo_x);
    sicalo_emc_y.push_back(sic_calo_y);
    sicalo_emc_z.push_back(sic_calo_z);
    sicalo_emc_r.push_back(sic_calo_r);
    sicalo_emc_phi.push_back(sic_calo_phi);
    sicalo_emc_energy.push_back(sic_calo_e);
    sicalo_emc_chi2.push_back(sic_calo_chi2);
    sicalo_emc_prob.push_back(sic_calo_prob);


  /////////////////////////

    delete rvsEmcalState;
    delete trkcp;

    if (t_charge > 0)
      ntrack_isposcharge.second++;
    else
      ntrack_isposcharge.first++;

    h_ntrack->Fill(t_eta, t_phi);
    h_nmaps->Fill(t_nmaps);
    h_nintt->Fill(t_nintt);
    h_nmaps_nintt->Fill(t_nmaps, t_nintt);
    h_avgnclus_eta_phi->Fill(t_eta, t_phi, t_nmaps + t_nintt);
    h_trackcrossing->Fill(t_crossing);
    h_trackchi2ndf->Fill(t_chi2ndf);
    h_trackpt_inclusive->Fill(t_pt);
    if (t_charge > 0)
      h_trackpt_pos->Fill(t_pt);
    else
      h_trackpt_neg->Fill(t_pt);
    if (!b_skipvtx)
    {
      Acts::Vector3 zero = Acts::Vector3::Zero();
      auto dcapair_origin = TrackAnalysisUtils::get_dca(track, zero);
      auto trackvtx = vertexmap->get(track->get_vertex_id());
      if (!trackvtx)
      {
        ntrack_isfromvtx.first++;
        continue;
      }
      ntrack_isfromvtx.second++;
      Acts::Vector3 track_vtx(trackvtx->get_x(), trackvtx->get_y(), trackvtx->get_z());
      auto dcapair_vtx = TrackAnalysisUtils::get_dca(track, track_vtx);

      h_dcaxyorigin_phi->Fill(t_phi, dcapair_origin.first.first);
      h_dcaxyvtx_phi->Fill(t_phi, dcapair_vtx.first.first);
      h_dcazorigin_phi->Fill(t_phi, dcapair_origin.second.first);
      h_dcazvtx_phi->Fill(t_phi, dcapair_vtx.second.first);
    }
  }
  //--std::cout << "Track vector sizes: id=" << track_id.size()
  //--          << ", x=" << track_x.size()
  //--          << ", cluster_x=" << SiClus_x.size()
  //--          << ", emc_x=" << track_x_emc.size()
  //--          << std::endl;

  // Print all track vector sizes and highlight mismatches
  bool size_mismatch = false;
  size_t size_ref = track_id.size();
  std::vector<std::pair<std::string, size_t>> track_vector_sizes = {
    {"track_id", track_id.size()},
    {"track_x", track_x.size()},
    {"track_y", track_y.size()},
    {"track_z", track_z.size()},
    {"track_px", track_px.size()},
    {"track_py", track_py.size()},
    {"track_pz", track_pz.size()},
    {"track_eta", track_eta.size()},
    {"track_phi", track_phi.size()},
    {"track_pt", track_pt.size()},
    {"track_chi2ndf", track_chi2ndf.size()},
    {"track_charge", track_charge.size()},
    {"track_crossing", track_crossing.size()},
    {"track_nmaps", track_nmaps.size()},
    {"track_nintt", track_nintt.size()},
    {"track_innerintt", track_innerintt.size()},
    {"track_outerintt", track_outerintt.size()},
    {"track_x_emc", track_x_emc.size()},
    {"track_y_emc", track_y_emc.size()},
    {"track_z_emc", track_z_emc.size()},
    {"track_px_emc", track_px_emc.size()},
    {"track_py_emc", track_py_emc.size()},
    {"track_pz_emc", track_pz_emc.size()},
    {"track_eta_emc", track_eta_emc.size()},
    {"track_phi_emc", track_phi_emc.size()},
    {"track_pt_emc", track_pt_emc.size()}
  };
  for (const auto& [name, size] : track_vector_sizes)
  {
    if (size != size_ref)
    {
      std::cout << "Size mismatch: " << name << " = " << size << " (expected " << size_ref << ")" << std::endl;
      size_mismatch = true;
    }
  }
  if (!size_mismatch)
  {
    //--std::cout << "All track vectors have matching sizes: " << size_ref << std::endl;
  }
  else
  {
    std::cout << "WARNING: Detected mismatch in track vector sizes." << std::endl;
  }
  
  trackTree->Fill();

  SiClusTree->Fill();
  h_ntrack_isfromvtx->SetBinContent(1, h_ntrack_isfromvtx->GetBinContent(1) + ntrack_isfromvtx.first);
  h_ntrack_isfromvtx->SetBinContent(2, h_ntrack_isfromvtx->GetBinContent(2) + ntrack_isfromvtx.second);
  h_ntrack_IsPosCharge->SetBinContent(1, h_ntrack_IsPosCharge->GetBinContent(1) + ntrack_isposcharge.first);
  h_ntrack_IsPosCharge->SetBinContent(2, h_ntrack_IsPosCharge->GetBinContent(2) + ntrack_isposcharge.second);

  //--std::cout << "EVENT " << evt << " is OK" << std::endl;

  evt++;
}

SvtxTrackState* SiliconSeedsAna::projectToEMCal(
  PHCompositeNode* topNode, 
  SvtxTrack* trk
)
{
  // m_tGeometry
  // caloRadius
  // halfZ

// *
  const std::string towerGeoNodeName = "TOWERGEOM_CEMC";

  // get tower geometry container
  const auto towerGeomContainer = findNode::getClass<RawTowerGeomContainer>(topNode, towerGeoNodeName.c_str());

  if( !towerGeomContainer )
  {
    std::cout << PHWHERE << "-"
      << " Calo tower geometry container for " << towerGeoNodeName
      << " not found on node tree. Track projections to calos won't be filled."
      << std::endl;
    return NULL;
  }

  // get calorimeter inner radius and store
  double caloRadius = towerGeomContainer->get_radius();

  // convert to ACTS units
  caloRadius *= Acts::UnitConstants::cm;

  /// Extend farther so that there is at least surface there, for high
  /// curling tracks. Can always reject later
  const auto eta = 2.5;
  const auto theta = 2. * atan(exp(-eta));
  const auto halfZ = caloRadius / tan(theta) * Acts::UnitConstants::cm;
// * /

  ActsGeometry* tGeometry = findNode::getClass<ActsGeometry>(topNode, m_actsgeometryName);
  if (!tGeometry)
  {
    std::cout << "ActsTrackingGeometry not on node tree. Exiting."
              << std::endl;

    return NULL;
  }
  SvtxVertexMap* vertexMap  = findNode::getClass<SvtxVertexMap>(topNode, m_vertexMapName);
  if (!vertexMap)
  {
    std::cout << PHWHERE << "Missing vertexmap, can't continue" << std::endl;
    return NULL;
  }



  //double caloRadius = _caloRadiusEMCal;
  //const auto eta = 2.5;
  //const auto theta = 2. * atan(exp(-eta));
  //const auto halfZ = caloRadius / tan(theta) * Acts::UnitConstants::cm;
  //--std::cout<<"radius: "<<caloRadius<<" "<<halfZ<<std::endl;
  //double halfZ      = 150.; // cm

  auto transform = Acts::Transform3::Identity();

  std::shared_ptr<Acts::CylinderSurface> cylSurf =
      Acts::Surface::makeShared<Acts::CylinderSurface>(transform,
                                                       caloRadius,
                                                       halfZ);

  // create propagator
  ActsPropagator prop(tGeometry);
  const double constFieldVal = 1.4;
  prop.constField();
  prop.verbosity(Verbosity());
  prop.setConstFieldValue(constFieldVal * Acts::UnitConstants::T);
  
  //// loop over tracks
  //for (const auto& [key, track] : *m_trackMap)
  {
    auto params = prop.makeTrackParams(trk, vertexMap);
    if(!params.ok())
    {
      return NULL; //continue;
    }
    
    // propagate
    const auto result = prop.propagateTrackFast(params.value(), cylSurf);
    if (result.ok())
    {
      // update track
      //updateSvtxTrack(result.value(), trk, caloLayer);
      const auto paramvalue = result.value().second;
      SvtxTrackState_v1* out = new SvtxTrackState_v1(caloRadius);

      const auto projectionPos = paramvalue.position(tGeometry->geometry().getGeoContext());
      const auto momentum = paramvalue.momentum();
      out->set_x(projectionPos.x() / Acts::UnitConstants::cm);
      out->set_y(projectionPos.y() / Acts::UnitConstants::cm);
      out->set_z(projectionPos.z() / Acts::UnitConstants::cm);
      out->set_px(momentum.x());
      out->set_py(momentum.y());
      out->set_pz(momentum.z());

      //--if (Verbosity() > 1)
      //--{
      //--  std::cout << "Adding track state for caloLayer " << caloLayer
      //--            << " at pathlength " << pathlength << " with position " << projectionPos.transpose() << std::endl;
      //--}

      ActsTransformations transformer;
      const auto globalCov = transformer.rotateActsCovToSvtxTrack(paramvalue);
      for (int i = 0; i < 6; ++i)
      {
        for (int j = 0; j < 6; ++j)
        {
          out->set_error(i, j, globalCov(i, j));
        }
      }

      trk->insert_state(out);

      return out;
    }
  }
  return NULL;
}

void SiliconSeedsAna::processSiCluster(PHCompositeNode* topNode)
{
  bool debug = false;

  evt_nintt = evt_nintt50 = evt_nmaps = 0;

  auto geometry   = findNode::getClass<ActsGeometry>(topNode, m_actsgeometryName);
  auto clustermap = findNode::getClass<TrkrClusterContainer>(topNode, m_clusterContainerName);
  if (!clustermap)
  {
    std::cout << PHWHERE << "Missing clustermap, can't continue" << std::endl;
    return;
  }
  if (!geometry)
  {
    std::cout << PHWHERE << "Missing geometry, can't continue" << std::endl;
    return;
  }

  SiClus_trackid.clear();
  SiClus_layer.clear();
  SiClus_x.clear();
  SiClus_y.clear();
  SiClus_z.clear();
  SiClus_t.clear();

  int nhits[7]   = {0,0,0,0,0,0,0};
  int nhits50[7] = {0,0,0,0,0,0,0};

  for (unsigned int layer = 0; layer < 7; layer++) // layer:0-2 MVTX, 3-7 INTT
  {
    TrkrDefs::TrkrId detid = (layer<3) ? TrkrDefs::TrkrId::mvtxId : TrkrDefs::TrkrId::inttId;

    for (const auto &hitsetkey : clustermap->getHitSetKeys(detid, layer))
    {
      auto range = clustermap->getClusters(hitsetkey);

      for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
      {
        const auto &cluster_key = clusIter->first;
        auto cluster            = clusIter->second;


        int timebkt=-9999;
        if(layer<3){
          timebkt = MvtxDefs::getStrobeId(cluster_key);
        }
        else {
          timebkt = InttDefs::getTimeBucketId(cluster_key);
        }


        if(-1<=timebkt&&timebkt<=1){
          Acts::Vector3 global(0., 0., 0.);
          global = geometry->getGlobalPosition(cluster_key, cluster);

          SiClus_trackid.push_back(-1);
          SiClus_x.push_back(global[0]);
          SiClus_y.push_back(global[1]);
          SiClus_z.push_back(global[2]);
          SiClus_layer.push_back(layer);
          SiClus_t.push_back(timebkt);

          nhits[layer]++;
        }
        if(0<=timebkt&&timebkt<50)  nhits50[layer]++;
      }
    }
    if(debug) std::cout<<"layer : "<<layer<<"     nhits : "<<nhits[layer]<<std::endl;
  }
  SiClusAllTree->Fill();

  evt_nmaps = nhits[0]+nhits[1]+nhits[2];
  evt_nintt = nhits[3]+nhits[4]+nhits[5]+nhits[6];
  evt_nintt50=nhits50[3]+nhits50[4]+nhits50[5]+nhits50[6]; 

}

void SiliconSeedsAna::processCaloClusters(PHCompositeNode* topNode)
{
  auto EMCalClusmap = findNode::getClass<RawClusterContainer>(topNode, m_emcalClusName);

    auto geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
    auto towEM  = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");

  clearCaloVectors();

  if((calo_evt%1000)==0) std::cout << "start calo map  EVENT " << calo_evt << " is OK" << std::endl;

  evt_nemc = evt_nemc02=0;

    float vx = 0.0;
    float vy = 0.0;
    float vz = 0.0;

    if (!b_skipvtx)
    {
        auto vertexmap = findNode::getClass<SvtxVertexMap>(topNode, m_vertexMapName);
        if (vertexmap && !vertexmap->empty())
        {
            SvtxVertex* vtx = vertexmap->begin()->second;
            if (vtx)
            {
                vx = vtx->get_x();
                vy = vtx->get_y();
                vz = vtx->get_z();
            }
        }
    }

  if (!b_skipcalo && EMCalClusmap)
  {
    //--std::cout<<"start EMCalClusmap " << std::endl;
    //--std::cout << "EMCalClusmap size: " << EMCalClusmap->size() << std::endl;
    evt_nemc = EMCalClusmap->size();
    for (unsigned int i = 0; i < EMCalClusmap->size(); i++)
    {
      RawCluster *clus = EMCalClusmap->getCluster(i);
      if (!clus)
        continue;
      if (clus->get_energy() < m_emcal_low_cut)
        continue;

      calo_x.push_back(clus->get_x());
      calo_y.push_back(clus->get_y());
      calo_z.push_back(clus->get_z());
      calo_r.push_back(clus->get_r());
      calo_phi.push_back(clus->get_phi());

      float eta = -1.0;
      float cx = clus->get_x();
      float cy = clus->get_y();
      float cz = clus->get_z();
      float dx = cx - vx;
      float dy = cy - vy;
      float dz = cz - vz;
      float dr = std::sqrt(dx * dx + dy * dy);
      float theta = std::atan2(dr, dz);
      eta = -std::log(std::tan(theta / 2.0));
      calo_eta.push_back(eta);

      const float energy =clus->get_energy();
      calo_energy.push_back(energy);
      calo_chi2.push_back(clus->get_chi2());
      calo_prob.push_back(clus->get_prob());

      if(energy>0.2) evt_nemc02++;

            std::vector<int>   v_ieta;
            std::vector<int>   v_iphi;
            std::vector<float> v_e;
            std::vector<float> v_x;
            std::vector<float> v_y;
            std::vector<float> v_z;
            std::vector<float> v_r;
            std::vector<float> v_eta;
            std::vector<float> v_phi;
            std::vector<float> v_time;

            auto tower_range = clus->get_towers();

            for (auto tower_it = tower_range.first;
                 tower_it != tower_range.second;
                 ++tower_it)
            {
                RawTowerDefs::keytype tower_key = tower_it->first;
                const float tower_e_in_cluster = tower_it->second;

                const int ieta = RawTowerDefs::decode_index1(tower_key);
                const int iphi = RawTowerDefs::decode_index2(tower_key);

                float tower_x = NAN;
                float tower_y = NAN;
                float tower_z = NAN;
                float tower_r = NAN;
                float tower_eta = NAN;
                float tower_phi = NAN;
                float tower_time = NAN;
                if (geomEM)
                {
                    RawTowerGeom* tower_geom = geomEM->get_tower_geometry(tower_key);
                    if (tower_geom)
                    {
                        tower_x = tower_geom->get_center_x();
                        tower_y = tower_geom->get_center_y();
                        tower_z = tower_geom->get_center_z();
                        tower_r = std::sqrt(tower_x * tower_x + tower_y * tower_y);
                        tower_eta = tower_geom->get_eta();
                        tower_phi = tower_geom->get_phi();
                    }
                }

                if (towEM)
                {
                    for (unsigned int ch = 0; ch < towEM->size(); ++ch)
                    {
                        unsigned int tkey = towEM->encode_key(ch);

                        if (static_cast<int>(towEM->getTowerEtaBin(tkey)) == ieta &&
                            static_cast<int>(towEM->getTowerPhiBin(tkey)) == iphi)
                        {
                            TowerInfo* tinfo = towEM->get_tower_at_channel(ch);
                            if (tinfo)
                            {
                                tower_time = tinfo->get_time();
                            }
                            break;
                        }
                    }
                }

                v_ieta.push_back(ieta);
                v_iphi.push_back(iphi);
                v_e.push_back(tower_e_in_cluster);
                v_x.push_back(tower_x);
                v_y.push_back(tower_y);
                v_z.push_back(tower_z);
                v_r.push_back(tower_r);
                v_eta.push_back(tower_eta);
                v_phi.push_back(tower_phi);
                v_time.push_back(tower_time);
            }

            calo_tower_ieta.push_back(v_ieta);
            calo_tower_iphi.push_back(v_iphi);
            calo_tower_e.push_back(v_e);
            calo_tower_x.push_back(v_x);
            calo_tower_y.push_back(v_y);
            calo_tower_z.push_back(v_z);
            calo_tower_r.push_back(v_r);
            calo_tower_eta.push_back(v_eta);
            calo_tower_phi.push_back(v_phi);
            calo_tower_time.push_back(v_time);

      //--std::cout << "EMCal x : " << calo_x.back() << ", EMCal y :  " << calo_y.back() << ", EMCal z : " << calo_z.back()
      //--          << ", EMCal r : " << calo_r.back() << ", EMCal phi :  " << calo_phi.back() 
      //--          << ", EMCal eta : " << calo_eta.back() << ", EMcal E : " << calo_energy.back() << std::endl;
    }
    caloTree->Fill();
  }
  else
  {
    std::cout << "No EMCal clusters found, skipping EMCal processing." << std::endl;
  }
  //--std::cout << "Number of calo clusters: " << calo_energy.size()
  //--          << ", calo event : " << calo_evt << std::endl;
  calo_evt++;
}

void SiliconSeedsAna::processVertexMap(PHCompositeNode* topNode)
{
  bool debug=false;

////////////////
  GlobalVertexMap* globalvtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!globalvtxmap)
  {
    std::cout << PHWHERE << " Fatal Error - GlobalVertexMap node is missing"<< std::endl;
    // return Fun4AllReturnCodes::ABORTRUN;
  }

  evt_gvtx_type.clear();
  evt_gvtx_x.clear(); evt_gvtx_y.clear(); evt_gvtx_z.clear();
  if (globalvtxmap && !globalvtxmap->empty())
  {
    std::cout<<"GlobalVertex Size : "<<globalvtxmap->size()<<std::endl;
    //globalvtxmap->identify();
    for(const auto& [key, globalvtx] : *globalvtxmap)
    {
      if(debug) std::cout<<"vertex : "<<key<<std::endl;

      if(globalvtx==nullptr)
      {
        std::cout<<" No globalVertex object"<<std::endl;
        continue;
      }
      if(debug) std::cout<<" "<<globalvtx->size_vtxs()<<std::endl;

//      globalvtx->identify(std::cout);

      auto typeStartIter = globalvtx->begin_vertexes();
      auto typeEndIter   = globalvtx->end_vertexes();
      for (auto iter = typeStartIter; iter != typeEndIter; ++iter)
      {
        int idx=0;
        const auto& [type, vertexVec] = *iter;

        if(debug) std::cout<<"   type, = "<<type<<", size = "<<vertexVec.size()<<std::endl;
        for (const auto* vtx : vertexVec)
        {
          if(debug) std::cout<<"     idx : "<<idx;
          if (!vtx)
          {
            if(debug) std::cout<<", no vertex object"<<std::endl;
            continue;
          }
          if(debug)
            std::cout <<", xyz "<<vtx->get_x()
                      <<" "<<vtx->get_y()
                      <<" "<<vtx->get_z()<<std::endl;

          evt_gvtx_type.push_back(type);
          evt_gvtx_x.push_back(vtx->get_x());
          evt_gvtx_y.push_back(vtx->get_y());
          evt_gvtx_z.push_back(vtx->get_z());

          if(debug)
            std::cout<<"  --- gvtx : "<<type<<" "<<vtx->get_x()<<" "<<vtx->get_y()<<" "<<vtx->get_z()<<std::endl;
          
          idx++;
        }
      }
    }
  }

  // check truth vertex and add it if not in the vectror
  {
    if(truth_vtx_x.size()>0){ //  
      auto vect_itr = std::find(evt_gvtx_type.begin(), evt_gvtx_type.end(), (int)GlobalVertex::TRUTH);
      if(vect_itr==evt_gvtx_type.end()){ // if not found
        evt_gvtx_type.push_back(GlobalVertex::TRUTH);
        evt_gvtx_x.push_back(truth_vtx_x[0]);
        evt_gvtx_y.push_back(truth_vtx_y[0]);
        evt_gvtx_z.push_back(truth_vtx_z[0]);
      }
    }
  }
  
  // set a global vertex which has largest type value
  int gv_type_max=-1;
  evt_xgvtx = evt_ygvtx = evt_zgvtx = -9999;
  for(size_t idx=0; idx<evt_gvtx_type.size(); ++idx){
    if(gv_type_max<evt_gvtx_type[idx]){
      gv_type_max = evt_gvtx_type[idx];
      evt_xgvtx   = evt_gvtx_x[idx];
      evt_ygvtx   = evt_gvtx_y[idx];
      evt_zgvtx   = evt_gvtx_z[idx];
    }
  }


  if(debug)
    std::cout<<"gvtx : "<<gv_type_max<<" "<<evt_xgvtx<<" "<<evt_ygvtx<<" "<<evt_zgvtx<<std::endl;
////////////////

  auto vertexmap = findNode::getClass<SvtxVertexMap>(topNode, m_vertexMapName);
  if (!vertexmap)
  {
    std::cout << PHWHERE << "Missing vertexmap, can't continue" << std::endl;
    return;
  }
  h_nvertex->Fill(vertexmap->size());

  if((evt%1000)==0) std::cout << "start VTX map  EVENT " << evt << " is OK" << std::endl;

  evt_xvtx = evt_yvtx = evt_zvtx = -9999.;
  for (const auto &[key, vertex] : *vertexmap)
  {
    if (!vertex)
    {
      continue;
    }
    float vx = vertex->get_x();
    float vy = vertex->get_y();
    float vz = vertex->get_z();
    float vt = vertex->get_t0();
    float vchi2 = vertex->get_chisq();
    int vndof = vertex->get_ndof();
    int vcrossing = vertex->get_beam_crossing();
    if (vcrossing != 0)
      continue;
    h_vx->Fill(vx);
    h_vy->Fill(vy);
    h_vx_vy->Fill(vx, vy);
    h_vz->Fill(vz);
    h_vt->Fill(vt);
    h_vchi2dof->Fill(float(vchi2 / vndof));
    h_vcrossing->Fill(vcrossing);
    h_ntrackpervertex->Fill(vertex->size_tracks());

    evt_xvtx = vx;
    evt_yvtx = vy;
    evt_zvtx = vz;

    std::cout<<"SvtxVertex : "<<vx<<" "<<vy<<" "<<vz<<std::endl;
  }
}

void SiliconSeedsAna::processHCalinfo(PHCompositeNode* topNode)
{
    topo_clus_e.clear();
    topo_clus_x.clear();
    topo_clus_y.clear();
    topo_clus_z.clear();
    topo_clus_r.clear();
    topo_clus_eta.clear();
    topo_clus_phi.clear();

    topo_clus_chi2.clear();
    topo_clus_prob.clear();

    topo_clus_emcal_e.clear();
    topo_clus_ihcal_e.clear();
    topo_clus_ohcal_e.clear();

    topo_tower_caloid.clear();
    topo_tower_ieta.clear();
    topo_tower_iphi.clear();
    topo_tower_e.clear();
    topo_tower_x.clear();
    topo_tower_y.clear();
    topo_tower_z.clear();
    topo_tower_eta.clear();
    topo_tower_phi.clear();
    topo_tower_time.clear();

    auto clusters = findNode::getClass<RawClusterContainer>(topNode, "TOPOCLUSTER_ALLCALO");
    //auto clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_CEMC");
    if (!clusters)
    {
      if (TopoClusTree) TopoClusTree->Fill();
      return;
    }

    auto geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
    auto geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
    auto geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

    auto towEM = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
    auto towIH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
    auto towOH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

    for (auto it = clusters->getClusters().first; it != clusters->getClusters().second; ++it)
    {
        RawCluster* clus = it->second;
        if (!clus) continue;
    

        // std::cout <<"============================JY=============================="<<std::endl;
        // std::cout << "top cluster energy: " << (clus ? clus->get_energy() : -1)
        //           << std::endl;
        // std::cout <<"==========================JY================================"<<std::endl;

        const float clus_e   = clus->get_energy();
        if (clus_e < 0.18) continue; // energy cut for topo clusters, can be adjusted

        const float clus_x   = clus->get_x();
        const float clus_y   = clus->get_y();
        const float clus_z   = clus->get_z();
        const float clus_r   = clus->get_r();
        const float clus_phi = clus->get_phi();
        const float clus_eta = -std::log(std::tan(std::atan2(clus_r, clus_z) / 2.0));

        float e_emcal = 0.0;
        float e_ihcal = 0.0;
        float e_ohcal = 0.0;

        std::vector<int>   v_caloid;
        std::vector<int>   v_ieta;
        std::vector<int>   v_iphi;
        std::vector<float> v_e;
        std::vector<float> v_x;
        std::vector<float> v_y;
        std::vector<float> v_z;
        std::vector<float> v_eta;
        std::vector<float> v_phi;
        std::vector<float> v_time;

        auto tower_range = clus->get_towers();

        for (auto tower_it = tower_range.first; tower_it != tower_range.second; ++tower_it)
        {
            RawTowerDefs::keytype tower_key = tower_it->first;
            float tower_e_in_cluster = tower_it->second;
       
            auto caloid = RawTowerDefs::decode_caloid(tower_key);
            int ieta = RawTowerDefs::decode_index1(tower_key);
            int iphi = RawTowerDefs::decode_index2(tower_key);
       
            // std::cout <<"============================JY=============================="<<std::endl;
            // std::cout << "Processing tower - CaloID: " << caloid << ", iEta: " << ieta << ", iPhi: " << iphi << ", Energy in cluster: " << tower_e_in_cluster << std::endl;
       
            RawTowerGeomContainer* geom = nullptr;
            TowerInfoContainer* towerinfos = nullptr;
          
            if (caloid == RawTowerDefs::CEMC)
            {
                geom = geomEM;
                towerinfos = towEM;
                e_emcal += tower_e_in_cluster;
            }
            else if (caloid == RawTowerDefs::HCALIN)
            {
                geom = geomIH;
                towerinfos = towIH;
                e_ihcal += tower_e_in_cluster;
            }
            else if (caloid == RawTowerDefs::HCALOUT)
            {
                geom = geomOH;
                towerinfos = towOH;
                e_ohcal += tower_e_in_cluster;
            }
            else
            {
                continue;
            }

            float tower_x = NAN;
            float tower_y = NAN;
            float tower_z = NAN;
            float tower_eta = NAN;
            float tower_phi = NAN;
            float tower_time = NAN;

            if (geom)
            {
                RawTowerGeom* tower_geom = geom->get_tower_geometry(tower_key);
                if (tower_geom)
                {
                    tower_x = tower_geom->get_center_x();
                    tower_y = tower_geom->get_center_y();
                    tower_z = tower_geom->get_center_z();
                    tower_eta = tower_geom->get_eta();
                    tower_phi = tower_geom->get_phi();
                    
                    // std::cout << "Tower geometry - CaloID: " << caloid << ", iEta: " << ieta << ", iPhi: " << iphi << ", x: " << tower_x << ", y: " << tower_y << ", z: " << tower_z
		    //           << ", eta: " << tower_eta << ", phi: " << tower_phi << std::endl;
		    // std::cout <<"==========================JY================================"<<std::endl;
		}
	    }

            if (towerinfos)
            {
                for (unsigned int ch = 0; ch < towerinfos->size(); ++ch)
                {
                    unsigned int tkey = towerinfos->encode_key(ch);
                    if (static_cast<int>(towerinfos->getTowerEtaBin(tkey)) == ieta &&
                        static_cast<int>(towerinfos->getTowerPhiBin(tkey)) == iphi)
                    {
                        TowerInfo* tinfo = towerinfos->get_tower_at_channel(ch);
                        if (tinfo) tower_time = tinfo->get_time();
                        break;
                    }
                }
            }

            v_caloid.push_back(static_cast<int>(caloid));
            v_ieta.push_back(ieta);
            v_iphi.push_back(iphi);
            v_e.push_back(tower_e_in_cluster);
            v_x.push_back(tower_x);
            v_y.push_back(tower_y);
            v_z.push_back(tower_z);
            v_eta.push_back(tower_eta);
            v_phi.push_back(tower_phi);
            v_time.push_back(tower_time);
        }

	// if (e_emcal/(e_ihcal+e_ohcal+0.00001) < 0.75)
	// continue;

	topo_clus_e.push_back(clus_e);
	topo_clus_x.push_back(clus_x);
	topo_clus_y.push_back(clus_y);
	topo_clus_z.push_back(clus_z);
	topo_clus_r.push_back(clus_r);
	topo_clus_eta.push_back(clus_eta);
	topo_clus_phi.push_back(clus_phi);

	topo_clus_chi2.push_back(clus->get_chi2());
	topo_clus_prob.push_back(clus->get_prob());

	topo_clus_emcal_e.push_back(e_emcal);
	topo_clus_ihcal_e.push_back(e_ihcal);
	topo_clus_ohcal_e.push_back(e_ohcal);
        
        topo_tower_caloid.push_back(v_caloid);
        topo_tower_ieta.push_back(v_ieta);
        topo_tower_iphi.push_back(v_iphi);
        topo_tower_e.push_back(v_e);
        topo_tower_x.push_back(v_x);
        topo_tower_y.push_back(v_y);
        topo_tower_z.push_back(v_z);
        topo_tower_eta.push_back(v_eta);
        topo_tower_phi.push_back(v_phi);
        topo_tower_time.push_back(v_time);
    }

    if (TopoClusTree) TopoClusTree->Fill();
}
                    

//____________________________________________________________________________..
int SiliconSeedsAna::EndRun(const int /*runnumber*/)
{
  if(m_outfile==nullptr) {
    std::cout<<"OutputFile is not open. No histogram saved"<<std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }
  m_outfile->cd();
  std::cout << "Writing histograms to " << m_outputfilename << std::endl;
  // Create a TFile to save histograms and TTree
  //  TFile outfile("output_histograms.root", "RECREATE");
  //TFile outfile(m_outputfilename.c_str(), "RECREATE");

  // Write histograms to the file
  h_nlayer->Write();
  h_nmaps->Write();
  h_nintt->Write();
  h_nmaps_nintt->Write();
  h_ntrack1d->Write();
  h_ntrack->Write();
  h_avgnclus_eta_phi->Write();
  h_trackcrossing->Write();
  h_trackchi2ndf->Write();
  h_dcaxyorigin_phi->Write();
  h_dcaxyvtx_phi->Write();
  h_dcazorigin_phi->Write();
  h_dcazvtx_phi->Write();
  h_ntrack_isfromvtx->Write();
  h_trackpt_inclusive->Write();
  h_trackpt_pos->Write();
  h_trackpt_neg->Write();
  h_ntrack_IsPosCharge->Write();
  h_nvertex->Write();
  h_vx->Write();
  h_vy->Write();
  h_vx_vy->Write();
  h_vz->Write();
  h_vt->Write();
  h_vcrossing->Write();
  h_vchi2dof->Write();
  h_ntrackpervertex->Write();

  // Write the TTree to the file
  if (trackTree)
  {
    trackTree->Write();
  }
  if(SiClusTree)
  {
    SiClusTree->Write();
  }
  if(SiClusAllTree)
  {
    SiClusAllTree->Write();
  }
  if (caloTree)
  {
    caloTree->Write();
  }
  if (evtTree)
  {
    evtTree->Write();
  }
  if (truthTree && truthTree->GetEntries() > 0)
  {
    truthTree->Write();
  }

  if (TopoClusTree)
  {
      TopoClusTree->Write();
  }

  // Close the file
  m_outfile->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SiliconSeedsAna::End(PHCompositeNode * /*unused*/)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

std::string SiliconSeedsAna::getHistoPrefix() const
{
  return std::string("h_") + Name() + std::string("_");
}

void SiliconSeedsAna::createHistos()
{
  {
    h_nlayer = new TH1F(std::string(getHistoPrefix() + "nlayer").c_str(), "layer clusters per track;Number of layer clusters per track;Entries", 14, -0.5, 13.5);
  }

  {
    h_nmaps = new TH1F(std::string(getHistoPrefix() + "nmaps").c_str(), "MVTX clusters per track;Number of MVTX clusters per track;Entries", 7, -0.5, 6.5);
  }

  {
    h_nintt = new TH1F(std::string(getHistoPrefix() + "nintt").c_str(), "INTT clusters per track;Number of INTT clusters per track;Entries", 7, -0.5, 6.5);
  }

  {
    h_nmaps_nintt = new TH2F(std::string(getHistoPrefix() + "nmaps_nintt").c_str(), "MVTX vs INTT clusters per track;Number of MVTX clusters per track;Number of INTT clusters per track;Entries", 7, -0.5, 6.5, 7, -0.5, 6.5);
  }

  {
    h_ntrack1d = new TH1F(std::string(getHistoPrefix() + "nrecotracks1d").c_str(), "Number of reconstructed tracks;Number of silicon tracklets;Entries", 200, 0, 200);
  }

  {
    h_ntrack = new TH2F(std::string(getHistoPrefix() + "nrecotracks").c_str(), "Number of reconstructed tracks;#eta;#phi [rad];Entries", 100, -1.1, 1.1, 300, -3.14159, 3.1459);
  }

  {
    h_avgnclus_eta_phi = new TProfile2D(std::string(getHistoPrefix() + "avgnclus_eta_phi").c_str(), "Average number of clusters per track;#eta;#phi [rad];Average number of clusters per track", 100, -1.1, 1.1, 300, -3.14159, 3.1459, 0, 10);
  }

  {
    h_trackcrossing = new TH1F(std::string(getHistoPrefix() + "trackcrossing").c_str(), "Track beam bunch crossing;Track crossing;Entries", 110, -10, 100);
  }

  {
    h_trackchi2ndf = new TH1F(std::string(getHistoPrefix() + "trackchi2ndf").c_str(), "Track chi2/ndof;Track #chi2/ndof;Entries", 100, 0, 20);
  }

  {
    h_dcaxyorigin_phi = new TH2F(std::string(getHistoPrefix() + "dcaxyorigin_phi").c_str(), "DCA xy origin vs phi;#phi [rad];DCA_{xy} wrt origin [cm];Entries", 300, -3.14159, 3.1459, 90, -3, 3);
  }

  {
    h_dcaxyvtx_phi = new TH2F(std::string(getHistoPrefix() + "dcaxyvtx_phi").c_str(), "DCA xy vertex vs phi;#phi [rad];DCA_{xy} wrt vertex [cm];Entries", 300, -3.14159, 3.1459, 90, -3, 3);
  }

  {
    h_dcazorigin_phi = new TH2F(std::string(getHistoPrefix() + "dcazorigin_phi").c_str(), "DCA z origin vs phi;#phi [rad];DCA_{z} wrt origin [cm];Entries", 300, -3.14159, 3.1459, 160, -20, 20);
  }

  {
    h_dcazvtx_phi = new TH2F(std::string(getHistoPrefix() + "dcazvtx_phi").c_str(), "DCA z vertex vs phi;#phi [rad];DCA_{z} wrt vertex [cm];Entries", 300, -3.14159, 3.1459, 160, -20, 20);
  }

  {
    h_ntrack_isfromvtx = new TH1F(std::string(getHistoPrefix() + "ntrack_isfromvtx").c_str(), "Num of tracks associated to a vertex;Is track associated to a vertex;Entries", 2, -0.5, 1.5);
  }

  {
    h_trackpt_inclusive = new TH1F(std::string(getHistoPrefix() + "trackpt").c_str(), "Track p_{T};p_{T} [GeV/c];Entries", 100, 0, 10);
  }

  {
    h_trackpt_pos = new TH1F(std::string(getHistoPrefix() + "trackpt_pos").c_str(), "Track p_{T} positive;Positive track p_{T} [GeV/c];Entries", 100, 0, 10);
  }

  {
    h_trackpt_neg = new TH1F(std::string(getHistoPrefix() + "trackpt_neg").c_str(), "Track p_{T} negative;Negative track p_{T} [GeV/c];Entries", 100, 0, 10);
  }

  {
    h_ntrack_IsPosCharge = new TH1F(std::string(getHistoPrefix() + "ntrack_IsPosCharge").c_str(), "Num of tracks with positive charge;Is track positive charged;Entries", 2, -0.5, 1.5);
  }

  // vertex
  {
    h_nvertex = new TH1F(std::string(getHistoPrefix() + "nrecovertices").c_str(), "Num of reco vertices per event;Number of vertices;Entries", 20, 0, 20);
  }

  {
    h_vx = new TH1F(std::string(getHistoPrefix() + "vx").c_str(), "Vertex x;Vertex x [cm];Entries", 100, -2.5, 2.5);
  }

  {
    h_vy = new TH1F(std::string(getHistoPrefix() + "vy").c_str(), "Vertex y;Vertex y [cm];Entries", 100, -2.5, 2.5);
  }

  {
    h_vx_vy = new TH2F(std::string(getHistoPrefix() + "vx_vy").c_str(), "Vertex x vs y;Vertex x [cm];Vertex y [cm];Entries", 100, -2.5, 2.5, 100, -2.5, 2.5);
  }

  {
    h_vz = new TH1F(std::string(getHistoPrefix() + "vz").c_str(), "Vertex z;Vertex z [cm];Entries", 50, -25, 25);
  }

  {
    h_vt = new TH1F(std::string(getHistoPrefix() + "vt").c_str(), "Vertex t;Vertex t [ns];Entries", 100, -1000, 20000);
  }

  {
    h_vcrossing = new TH1F(std::string(getHistoPrefix() + "vertexcrossing").c_str(), "Vertex beam bunch crossing;Vertex crossing;Entries", 100, -100, 300);
  }

  {
    h_vchi2dof = new TH1F(std::string(getHistoPrefix() + "vertexchi2dof").c_str(), "Vertex chi2/ndof;Vertex #chi2/ndof;Entries", 100, 0, 20);
  }

  {
    h_ntrackpervertex = new TH1F(std::string(getHistoPrefix() + "ntrackspervertex").c_str(), "Num of tracks per vertex;Number of tracks per vertex;Entries", 50, 0, 50);
  }
}

void SiliconSeedsAna::createTree()
{
}


void SiliconSeedsAna::getCaloPosition(RawCluster *calo, float &x, float &y, float &z)
{
  bool debug = false;
  if(calo==nullptr) {
    x = y = z = std::numeric_limits<float>::quiet_NaN();
    return;
  }

  x   = calo->get_x();
  y   = calo->get_y();
  z   = calo->get_z();
  float r   = calo->get_r();
  float phi = calo->get_phi();
  float rr = sqrt(x*x + y*y);
  if(debug) std::cout<<"emc_x, y: "<<x<<" "<<y<<std::endl;
  if(fabs(rr - r)>1) 
  {
    //std::cout<<"no emc_x, y: "<<x<<" "<<y<<" "<<rr
    //         <<", replaced by r + phi"<<r<<" "<<phi<<" : ";
    x = r * cos(phi);
    y = r * sin(phi);
    //std::cout<<emc_x<<" "<<emc_y<<std::endl;
  }
}


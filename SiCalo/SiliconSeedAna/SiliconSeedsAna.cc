#include "SiliconSeedsAna.h"

#include <ffarawobjects/Gl1Packet.h>
#include <ffaobjects/EventHeader.h>
#include <trackbase/InttDefs.h>

#include <qautils/QAHistManagerDef.h>
#include <qautils/QAUtil.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <qautils/QAHistManagerDef.h>

#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include <TFile.h>
#include <TTree.h>

// Add member variables for TTree and track data

//____________________________________________________________________________..
SiliconSeedsAna::SiliconSeedsAna(const std::string &name)
    : SubsysReco(name)
{
}

#define LOG(msg) std::cout << "[SiliconSeedsAna] " << msg << std::endl;

void SiliconSeedsAna::clearTrackVectors() {
  track_id.clear(); 
  track_x.clear(); track_y.clear(); track_z.clear();
  track_px.clear(); track_py.clear(); track_pz.clear();
  track_eta.clear(); track_phi.clear(); track_pt.clear();
  track_chi2ndf.clear(); track_charge.clear(); track_crossing.clear();
  track_nmaps.clear(); track_nintt.clear(); track_innerintt.clear(); track_outerintt.clear();
  track_x_emc.clear(); track_y_emc.clear(); track_z_emc.clear();
  track_px_emc.clear(); track_py_emc.clear(); track_pz_emc.clear();
  track_eta_emc.clear(); track_phi_emc.clear(); track_pt_emc.clear();

  // Clear matched calo vectors
  matched_calo_x.clear();
  matched_calo_y.clear();
  matched_calo_z.clear();
  matched_calo_r.clear();
  matched_calo_phi.clear();
  matched_calo_eta.clear();
  matched_calo_energy.clear();
  matched_calo_dR.clear();
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
}

void SiliconSeedsAna::fillEMCalState(SvtxTrackState* state) {
  track_x_emc.push_back(  state ? state->get_x()  : NAN);
  track_y_emc.push_back(  state ? state->get_y()  : NAN);
  track_z_emc.push_back(  state ? state->get_z()  : NAN);
  track_px_emc.push_back( state ? state->get_px() : NAN);
  track_py_emc.push_back( state ? state->get_py() : NAN);
  track_pz_emc.push_back( state ? state->get_pz() : NAN);
  track_eta_emc.push_back(state ? state->get_eta(): NAN);
  track_phi_emc.push_back(state ? state->get_phi(): NAN);
  track_pt_emc.push_back( state ? state->get_pt() : NAN);

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
  // Add matched EMCal cluster branches
  trackTree->Branch("matched_calo_x", &matched_calo_x);
  trackTree->Branch("matched_calo_y", &matched_calo_y);
  trackTree->Branch("matched_calo_z", &matched_calo_z);
  trackTree->Branch("matched_calo_r", &matched_calo_r);
  trackTree->Branch("matched_calo_phi", &matched_calo_phi);
  trackTree->Branch("matched_calo_eta", &matched_calo_eta);
  trackTree->Branch("matched_calo_energy", &matched_calo_energy);
  trackTree->Branch("matched_calo_dR", &matched_calo_dR);
  trackTree->SetBasketSize("*", 50000);
}

void SiliconSeedsAna::initCaloTreeBranches() {
  caloTree = new TTree("caloTree", "Calo Data");
  caloTree->Branch("calo_evt", &calo_evt, "calo_evt/I");
  caloTree->Branch("x",     &calo_x);
  caloTree->Branch("y",     &calo_y);
  caloTree->Branch("z",     &calo_z);
  caloTree->Branch("r",     &calo_r);
  caloTree->Branch("phi",   &calo_phi);
  caloTree->Branch("eta",   &calo_eta);
  caloTree->Branch("energy",&calo_energy);
  caloTree->Branch("chi2",  &calo_chi2);
  caloTree->Branch("prob",  &calo_prob);
  caloTree->SetBasketSize("*",50000);
}

//____________________________________________________________________________..
int SiliconSeedsAna::InitRun(PHCompositeNode * /*unused*/)
{
  m_outfile = new TFile(m_outputfilename.c_str(), "RECREATE");

  createHistos();
  // Create a TTree to store track data and initialize branches
  initTrackTreeBranches();
  SiClusTree = new TTree("SiClusTree", "Silicon Cluster Data");
  SiClusTree->Branch("evt", &evt, "evt/I");
  SiClusTree->Branch("Siclus_trackid", &SiClus_trackid);
  SiClusTree->Branch("Siclus_layer", &SiClus_layer);
  SiClusTree->Branch("Siclus_x", &SiClus_x);
  SiClusTree->Branch("Siclus_y", &SiClus_y);
  SiClusTree->Branch("Siclus_z", &SiClus_z);
  SiClusTree->SetBasketSize("*",50000); // Set a larger basket size for better performance

  initCaloTreeBranches();

  evtTree = new TTree("evtTree", "Event Data");
  evtTree->Branch("evt",     &evt,       "evt/I");
  evtTree->Branch("caloevt", &calo_evt,  "caloevt/I");
  evtTree->Branch("bco",     &evt_bco,   "bco/l");
  evtTree->Branch("crossing",&evt_crossing, "crossing/I");
  evtTree->Branch("nintt",   &evt_nintt, "nintt/I");
  evtTree->Branch("nintt50", &evt_nintt50,"nintt50/I");
  evtTree->Branch("nmaps",   &evt_nmaps, "nmaps/I");
  evtTree->Branch("nemc",    &evt_nemc,  "nemc/I");
  evtTree->Branch("nemc02",  &evt_nemc02,"nemc02/I");
  evtTree->Branch("xvtx",    &evt_xvtx,  "xvtx/F");
  evtTree->Branch("yvtx",    &evt_yvtx,  "yvtx/F");
  evtTree->Branch("zvtx",    &evt_zvtx,  "zvtx/F");

  // Truth tree and branches
  if(isMC){
    truthTree = new TTree("truthTree", "Truth Particle Data");
    truthTree->Branch("truth_pid", &truth_pid);
    truthTree->Branch("truth_px", &truth_px);
    truthTree->Branch("truth_py", &truth_py);
    truthTree->Branch("truth_pz", &truth_pz);
    truthTree->Branch("truth_e", &truth_e);
    truthTree->Branch("truth_pt", &truth_pt);
    truthTree->Branch("truth_eta", &truth_eta);
    truthTree->Branch("truth_phi", &truth_phi);
    truthTree->SetBasketSize("*",50000); // Set a larger basket size for better performance
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SiliconSeedsAna::process_event(PHCompositeNode* topNode)
{
  if(isMC)
    fillTruthTree(topNode);

  processCaloClusters(topNode);
  processTrackMap(topNode);
  processVertexMap(topNode);

  //////////////
  {
    evt_bco = evt_nintt = evt_nintt50 = evt_nmaps = 0;
    evt_crossing = std::numeric_limits<int>::signaling_NaN();

    auto gl1        = findNode::getClass<Gl1Packet>(  topNode, "GL1Packet");
    auto evthdr     = findNode::getClass<EventHeader>(topNode, "EventHeader");
    auto clustermap = findNode::getClass<TrkrClusterContainer>(topNode, m_clusterContainerName);

    if(gl1){
      uint64_t bunch_gl1= gl1->getBunchNumber();
      std::cout<<"BCO : "<<bunch_gl1<<", ";
      evt_bco =  bunch_gl1;

    }
    else { std::cout<<"No GL1Packet"<<std::endl; }
    if(evthdr){
      uint64_t bunch_evt= evthdr->get_BunchCrossing();
      std::cout<<"Evt Header : "<<bunch_evt;
      //  evt_bco = 

    }
    else { std::cout<<"No EventHeader"<<std::endl; }
    std::cout<<std::endl;

    if(clustermap){
      int nclus_intt[2]={0,0};
      int nclus_intt50[2]={0,0};
      for (auto layer = 0; layer < 4; layer++)
      {
        int inout = (layer/2);
        for (const auto &hitsetkey : clustermap->getHitSetKeys(TrkrDefs::TrkrId::inttId, layer + 3))
        {
          auto range = clustermap->getClusters(hitsetkey);

          int intt_time = InttDefs::getTimeBucketId(hitsetkey);
          //--int intt_ldr  = InttDefs::getLadderPhiId(hitsetkey);
          for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
          {
            //const auto ckey = citer->first;
            //int time = InttDef::getTimeBucket(ckey);
            if(intt_time==0){
              nclus_intt[inout]++;
            }
            if(0<=intt_time&&intt_time<50){
              nclus_intt50[inout]++;
            }
          }
          //      std::cout<<"nintt time : "<<layer<<" "<<intt_ldr<<" : "<<intt_time<<" "<<nclus_intt[inout]<<std::endl;
        }
      }
      evt_nintt   = (nclus_intt[0] + nclus_intt[1]);
      evt_nintt50 = (nclus_intt50[0] + nclus_intt50[1]);
      std::cout<<"nintt "<<evt_nintt<<" "<<evt_nintt50<<std::endl;

      //std::set<TrkrDefs::TrkrId> detectors;
      //detectors.insert(TrkrDefs::TrkrId::mvtxId);
      int nclusmvtx[3] = {0,0,0};
      //float ntpval_mvtx[20];
      //for (const auto &det : detectors)
      //{
        for (const auto &layer : {0, 1, 2})
        {
          for (const auto &hitsetkey : clustermap->getHitSetKeys(TrkrDefs::TrkrId::mvtxId, layer))
          {
            auto range = clustermap->getClusters(hitsetkey);

            int strbid   = MvtxDefs::getStrobeId(hitsetkey);
           //-- int mvtx_ldr  = MvtxDefs::getStaveId(hitsetkey);
            //int nmvtx = range.second - range.first;;
            if(strbid==0){
              for (auto citer = range.first; citer != range.second; ++citer)
              {
                nclusmvtx[layer]++;
              }
            }
            //         std::cout<<"mvtx: "<<layer<<" "<<mvtx_ldr<<" : "<<strbid<<std::endl;

          }

          evt_nmaps+= nclusmvtx[layer];
          //std::cout<<"nmvtx "<<layer<<" "<<nclusmvtx[layer]<<" "<<evt_nmaps<<std::endl;
        }
        std::cout<<"nmvtx "<<evt_nmaps<<std::endl;
        //}
    }

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
  truth_pid.clear();
  truth_px.clear(); truth_py.clear(); truth_pz.clear(); truth_e.clear();
  truth_pt.clear(); truth_eta.clear(); truth_phi.clear();

  const auto prange = m_truth_info->GetPrimaryParticleRange();
  for (auto iter = prange.first; iter != prange.second; ++iter)
  {
    PHG4Particle* ptcl = iter->second;
    if (!ptcl) continue;

    TLorentzVector p(ptcl->get_px(), ptcl->get_py(), ptcl->get_pz(), ptcl->get_e());

    truth_pid.push_back(ptcl->get_pid());
    truth_px.push_back(ptcl->get_px());
    truth_py.push_back(ptcl->get_py());
    truth_pz.push_back(ptcl->get_pz());
    truth_e.push_back(ptcl->get_e());
    truth_pt.push_back(p.Pt());
    truth_eta.push_back(p.Eta());
    truth_phi.push_back(p.Phi());
  }
  truthTree->Fill();
}

void SiliconSeedsAna::processTrackMap(PHCompositeNode* topNode)
{
  auto clustermap = findNode::getClass<TrkrClusterContainer>(topNode, m_clusterContainerName);
  auto geometry   = findNode::getClass<ActsGeometry>(topNode, m_actsgeometryName);
  auto vertexmap  = findNode::getClass<SvtxVertexMap>(topNode, m_vertexMapName);

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

  if((evt%1000)==0) std::cout << "start track map  EVENT " << evt << " is OK" << std::endl;

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
    int t_charge    = track->get_charge();
    float t_chi2ndf = track->get_quality();
    float t_x       = track->get_x();
    float t_y       = track->get_y();
    float t_z       = track->get_z();
    float t_px      = track->get_px();
    float t_py      = track->get_py();
    float t_pz      = track->get_pz();
    int t_crossing = trkcrossing;
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
    track_charge.push_back(t_charge);
    track_crossing.push_back(t_crossing);
    if (false)
      std::cout << "track_x : " << t_x << ", track_y: " << t_y << ", track_z: " << t_z 
                << ", track_eta: " << t_eta << ", track_phi: " << t_phi << ", track_pt: " << t_pt << std::endl;

    SvtxTrackState *emcalState = track->get_state(_caloRadiusEMCal);
    fillEMCalState(emcalState);

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

void SiliconSeedsAna::processCaloClusters(PHCompositeNode* topNode)
{
  auto EMCalClusmap = findNode::getClass<RawClusterContainer>(topNode, m_emcalClusName);
  clearCaloVectors();
  if((calo_evt%1000)==0) std::cout << "start calo map  EVENT " << calo_evt << " is OK" << std::endl;
  
  evt_nemc = evt_nemc02=0;

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
      float vx = 0.0, vy = 0.0, vz = 0.0;

      if (!b_skipvtx)
      {
        auto vertexmap = findNode::getClass<SvtxVertexMap>(topNode, m_vertexMapName);
        if (vertexmap && !vertexmap->empty())
        {
          SvtxVertex *vtx = vertexmap->begin()->second;
          if (vtx)
          {
            vx = vtx->get_x();
            vy = vtx->get_y();
            vz = vtx->get_z();
          }
        }
      }
      float dx = cx - vx;
      float dy = cy - vy;
      float dz = cz - vz;
      float dr = std::sqrt(dx * dx + dy * dy);
      float theta = std::atan2(dr, dz);
      eta = -std::log(std::tan(theta / 2.0));
      calo_eta.push_back(eta);

      float energy =clus->get_energy();
      calo_energy.push_back(energy);
      calo_chi2.push_back(clus->get_chi2());
      calo_prob.push_back(clus->get_prob());

      if(energy>0.2) evt_nemc02++;

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
  std::cout << "Number of calo clusters: " << calo_energy.size()
            << ", calo event : " << calo_evt << std::endl;
  calo_evt++;
}

void SiliconSeedsAna::processVertexMap(PHCompositeNode* topNode)
{
  auto vertexmap = findNode::getClass<SvtxVertexMap>(topNode, m_vertexMapName);
  if (!vertexmap)
  {
    std::cout << PHWHERE << "Missing vertexmap, can't continue" << std::endl;
    return;
  }
  h_nvertex->Fill(vertexmap->size());

  if((evt%1000)==0) std::cout << "start VTX map  EVENT " << evt << " is OK" << std::endl;

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
  }
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

#include "TagAndProbe.h"

#include <globalvertex/SvtxVertexMap.h>
#include <trackbase_historic/SvtxTrackMap.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>

#include <TEntryList.h>
#include <TFile.h>
#include <TLeaf.h>
#include <TSystem.h>
#include <TTree.h>  // for getting the TTree from the file
#include <TVector3.h>
#include <TLorentzVector.h>

//#include <KFPVertex.h>
//#include <KFParticle.h>           // for KFParticle
#include <fun4all/Fun4AllBase.h>  // for Fun4AllBase::VERBOSITY...
#include <fun4all/SubsysReco.h>   // for SubsysReco

#include <ffamodules/CDBInterface.h>  // for accessing the field map file from the CDB
#include <cctype>                     // for toupper
#include <cmath>                      // for sqrt
#include <cstdlib>                    // for size_t, exit
#include <filesystem>
#include <iostream>  // for operator<<, endl, basi...
#include <map>       // for map
#include <tuple>     // for tie, tuple

class PHCompositeNode;

// Tag and Probe Constructor
TagAndProbe::TagAndProbe()
  : SubsysReco("TAGANDPROBE")
  , m_save_output(true)
  , m_outfile_name("outputTAP.root")
  , m_outfile(nullptr)
  , m_run(0)
  , m_segment(0)
{
}

TagAndProbe::TagAndProbe(const std::string &name, const float run, const float seg)
  : SubsysReco(name)
  , m_save_output(true)
  , m_outfile_name("outputTAP.root")
  , m_outfile(nullptr)
  , m_run(run)
  , m_segment(seg)
{
}

int TagAndProbe::Init(PHCompositeNode *topNode)
{
  assert(topNode);
  if (m_save_output && Verbosity() >= VERBOSITY_SOME)
  {
    std::cout << "Output nTuple: " << m_outfile_name << std::endl;
  }
  if (m_save_output)
  {
    m_outfile = TFile::Open(m_outfile_name.c_str(),"RECREATE");
  }

  initializeBranches();

  m_event = 0;

  return 0;
}

int TagAndProbe::InitRun(PHCompositeNode *topNode)
{
  assert(topNode);

  m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!m_tGeometry)
  {
    std::cout << PHWHERE << "Error, can't find acts tracking geometry" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //getField();

  m_cutInfoTree->Fill();
 
  return 0;
}

int TagAndProbe::process_event(PHCompositeNode *topNode)
{ 
  SvtxTrackMap *m_trackmap = findNode::getClass<SvtxTrackMap>(topNode, m_trk_map_node_name);
  int multiplicity = m_trackmap->size();
  if (multiplicity == 0)
  {
    if (Verbosity() >= VERBOSITY_SOME)
    {
      std::cout << "TagAndProbe: Event skipped as there are no tracks" << std::endl;
    }
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_vertexMap = findNode::getClass<SvtxVertexMap>(topNode, m_vtx_map_node_name);
  if (m_vertexMap->size() == 0)
  {
    if (Verbosity() >= VERBOSITY_SOME)
    {
      std::cout << "TagAndProbe: Event skipped as there are no vertices" << std::endl;
    }
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  int t1 = 0;
  for (const auto &[key1, track1] : *m_trackmap)
  {
    if (!track1)
    {
      continue;
    }

    m_trackID_1 = track1->get_id(); 
    m_crossing_1 = track1->get_crossing();   
    m_px_1 = track1->get_px();
    m_py_1 = track1->get_py();
    m_pz_1 = track1->get_pz();
    m_pt_1 = track1->get_pt();
    m_eta_1 = track1->get_eta();
    m_phi_1 = track1->get_phi();
    m_charge_1 = track1->get_charge();
    m_quality_1 = track1->get_quality();
    m_chisq_1 = track1->get_chisq();
    m_ndf_1 = track1->get_ndf();

    TrackSeed* silseed1 = track1->get_silicon_seed();
    TrackSeed* tpcseed1 = track1->get_tpc_seed();   
    m_nmaps_1 = 0;
    m_nintt_1 = 0;
    m_ntpc_1 = 0;
    m_nmms_1 = 0;
    m_nhits_1 = 0; 
    // +1 just in case _nlayers is zero
    std::vector<int> maps(_nlayers_maps+1, 0);
    std::vector<int> intt(_nlayers_intt+1, 0);
    std::vector<int> tpc(_nlayers_tpc+1, 0);
    std::vector<int> mms(_nlayers_mms+1, 0);

    if (tpcseed1)
    {
      m_nhits_1 += tpcseed1->size_cluster_keys();
      for (TrackSeed::ConstClusterKeyIter local_iter = tpcseed1->begin_cluster_keys();
           local_iter != tpcseed1->end_cluster_keys();
           ++local_iter)
      {
        TrkrDefs::cluskey cluster_key = *local_iter;
        //TrkrCluster* cluster = clustermap->findCluster(cluster_key);
        unsigned int local_layer = TrkrDefs::getLayer(cluster_key);
        if (_nlayers_maps > 0 && local_layer < _nlayers_maps)
        {
          maps[local_layer] = 1;
          m_nmaps_1++;
        }
        if (_nlayers_intt > 0 && local_layer >= _nlayers_maps && local_layer < _nlayers_maps + _nlayers_intt)
        {
          intt[local_layer - _nlayers_maps] = 1;
          m_nintt_1++;
        }
        if (_nlayers_tpc > 0 &&
            local_layer >= (_nlayers_maps + _nlayers_intt) &&
            local_layer < (_nlayers_maps + _nlayers_intt + _nlayers_tpc))
        {
          tpc[local_layer - (_nlayers_maps + _nlayers_intt)] = 1;
          m_ntpc_1++; 
        }
        if (_nlayers_mms > 0 && local_layer >= _nlayers_maps + _nlayers_intt + _nlayers_tpc)
        {
          mms[local_layer - (_nlayers_maps + _nlayers_intt + _nlayers_tpc)] = 1;
          m_nmms_1++;
        }
      }
    }
    if (silseed1)
    {
      m_nhits_1 += silseed1->size_cluster_keys();
      for (TrackSeed::ConstClusterKeyIter local_iter = silseed1->begin_cluster_keys();
           local_iter != silseed1->end_cluster_keys();
           ++local_iter)
      {
        TrkrDefs::cluskey cluster_key = *local_iter;
        //TrkrCluster* cluster = clustermap->findCluster(cluster_key);
        unsigned int local_layer = TrkrDefs::getLayer(cluster_key);
        if (_nlayers_maps > 0 && local_layer < _nlayers_maps)
        {
          maps[local_layer] = 1;
          m_nmaps_1++;
        }
        if (_nlayers_intt > 0 && local_layer >= _nlayers_maps && local_layer < _nlayers_maps + _nlayers_intt)
        {
          intt[local_layer - _nlayers_maps] = 1;
          m_nintt_1++;
        }
        if (_nlayers_tpc > 0 &&
            local_layer >= (_nlayers_maps + _nlayers_intt) &&
            local_layer < (_nlayers_maps + _nlayers_intt + _nlayers_tpc))
        {
          tpc[local_layer - (_nlayers_maps + _nlayers_intt)] = 1;
          m_ntpc_1++; 
        }
        if (_nlayers_mms > 0 && local_layer >= _nlayers_maps + _nlayers_intt + _nlayers_tpc)
        {
          mms[local_layer - (_nlayers_maps + _nlayers_intt + _nlayers_tpc)] = 1;
          m_nmms_1++;
        }
      }
    }
    float nlintt = 0;
    float nlmaps = 0;
    float nltpc = 0;
    float nlmms = 0;
    if (_nlayers_maps > 0)
    {
      for (unsigned int i = 0; i < _nlayers_maps; i++)
      {
        nlmaps += maps[i];
      }
    }  
    if (_nlayers_intt > 0)
    {
      for (unsigned int i = 0; i < _nlayers_intt; i++)
      {
        nlintt += intt[i];
      }
    }
    if (_nlayers_tpc > 0)
    {
      for (unsigned int i = 0; i < _nlayers_tpc; i++)
      {
        nltpc += tpc[i];
      }
    }
    if (_nlayers_mms > 0)
    {
      for (unsigned int i = 0; i < _nlayers_mms; i++)
      {
        nlmms += mms[i];
      }
    }
    m_nlayers_1 = nlmaps + nlintt + nltpc + nlmms;

    m_mapsstates_1 = 0;
    m_inttstates_1 = 0;
    m_tpcstates_1 = 0;
    m_mmsstates_1 = 0;
    for (auto state_iter = track1->begin_states();
       state_iter != track1->end_states();
       ++state_iter)
    {
      SvtxTrackState* tstate = state_iter->second;
      if (tstate->get_pathlength() != 0) //The first track state is an extrapolation so has no cluster
      {
        auto stateckey = tstate->get_cluskey();
        uint8_t id = TrkrDefs::getTrkrId(stateckey);
    
        switch (id)
        { 
          case TrkrDefs::mvtxId:
            ++m_mapsstates_1;
            break;
          case TrkrDefs::inttId:
            ++m_inttstates_1;
            break;
          case TrkrDefs::tpcId:
            ++m_tpcstates_1;
            break;
          case TrkrDefs::micromegasId:
            ++m_mmsstates_1;
            break;
          default:
           std::cout << "Cluster key doesnt match a tracking system, this shouldn't happen" << std::endl;
           break; 
        }
      }
    }

    if (m_include_pv_info)
    {
      // this is the global vertex id
      int _vertexID = track1->get_vertex_id();
      auto _vertex = m_vertexMap->get(_vertexID);
      if (_vertex)
      {
        m_vx = _vertex->get_x();
        m_vy = _vertex->get_y();
        m_vz = _vertex->get_z();
      }
      else 
      {
        m_vx = NAN;
        m_vy = NAN;
        m_vz = NAN;
      }
    }
  
    if (m_ntpc_1 >= m_nTPC_tag && m_nmaps_1 >= m_nMVTX_tag && m_nintt_1 >= m_nINTT_tag && 
        m_nmms_1 >= m_nTPOT_tag && m_quality_1 <= m_quality_tag)
    {
      m_tagpass_1 = true; 
    }
    else
    {
      m_tagpass_1 = false;
    }

    if (m_ntpc_1 >= m_nTPC_passprobe && m_nmaps_1 >= m_nMVTX_passprobe && m_nintt_1 >= m_nINTT_passprobe && 
        m_nmms_1 >= m_nTPOT_passprobe && m_quality_1 <= m_quality_passprobe)
    {
      m_probepass_1 = true; 
    }
    else
    {
      m_probepass_1 = false;
    }
    
    int t2 = 0;
    for (const auto &[key2, track2] : *m_trackmap)
    {
      if (!track2 || t1 >= t2 || track1->get_charge() == track2->get_charge() || track1->get_crossing() != track2->get_crossing())
      {
        ++t2;
        continue;
      }
      m_trackID_2 = track2->get_id(); 
      m_crossing_2 = track2->get_crossing();   
      m_px_2 = track2->get_px();
      m_py_2 = track2->get_py();
      m_pz_2 = track2->get_pz();
      m_pt_2 = track2->get_pt();
      m_eta_2 = track2->get_eta();
      m_phi_2 = track2->get_phi();
      m_charge_2 = track2->get_charge();
      m_quality_2 = track2->get_quality();
      m_chisq_2 = track2->get_chisq();
      m_ndf_2 = track2->get_ndf();

      TrackSeed* silseed2 = track2->get_silicon_seed();
      TrackSeed* tpcseed2 = track2->get_tpc_seed();   
      m_nmaps_2 = 0;
      m_nintt_2 = 0;
      m_ntpc_2 = 0;
      m_nmms_2 = 0;
      m_nhits_2 = 0; 
      maps.clear();
      intt.clear();
      tpc.clear();
      mms.clear();

      if (tpcseed2)
      {
        m_nhits_2 += tpcseed2->size_cluster_keys();
        for (TrackSeed::ConstClusterKeyIter local_iter = tpcseed2->begin_cluster_keys();
             local_iter != tpcseed2->end_cluster_keys();
             ++local_iter)
        {
          TrkrDefs::cluskey cluster_key = *local_iter;
          //TrkrCluster* cluster = clustermap->findCluster(cluster_key);
          unsigned int local_layer = TrkrDefs::getLayer(cluster_key);
          if (_nlayers_maps > 0 && local_layer < _nlayers_maps)
          {
            maps[local_layer] = 1;
            m_nmaps_2++;
          }
          if (_nlayers_intt > 0 && local_layer >= _nlayers_maps && local_layer < _nlayers_maps + _nlayers_intt)
          {
            intt[local_layer - _nlayers_maps] = 1;
            m_nintt_2++;
          }
          if (_nlayers_tpc > 0 &&
              local_layer >= (_nlayers_maps + _nlayers_intt) &&
              local_layer < (_nlayers_maps + _nlayers_intt + _nlayers_tpc))
          {
            tpc[local_layer - (_nlayers_maps + _nlayers_intt)] = 1;
            m_ntpc_2++; 
          }
          if (_nlayers_mms > 0 && local_layer >= _nlayers_maps + _nlayers_intt + _nlayers_tpc)
          {
            mms[local_layer - (_nlayers_maps + _nlayers_intt + _nlayers_tpc)] = 1;
            m_nmms_2++;
          }
        }
      }
      if (silseed2)
      {
        m_nhits_2 += silseed2->size_cluster_keys();
        for (TrackSeed::ConstClusterKeyIter local_iter = silseed2->begin_cluster_keys();
             local_iter != silseed2->end_cluster_keys();
             ++local_iter)
        {
          TrkrDefs::cluskey cluster_key = *local_iter;
          //TrkrCluster* cluster = clustermap->findCluster(cluster_key);
          unsigned int local_layer = TrkrDefs::getLayer(cluster_key);
          if (_nlayers_maps > 0 && local_layer < _nlayers_maps)
          {
            maps[local_layer] = 1;
            m_nmaps_2++;
          }
          if (_nlayers_intt > 0 && local_layer >= _nlayers_maps && local_layer < _nlayers_maps + _nlayers_intt)
          {
            intt[local_layer - _nlayers_maps] = 1;
            m_nintt_2++;
          }
          if (_nlayers_tpc > 0 &&
              local_layer >= (_nlayers_maps + _nlayers_intt) &&
              local_layer < (_nlayers_maps + _nlayers_intt + _nlayers_tpc))
          {
            tpc[local_layer - (_nlayers_maps + _nlayers_intt)] = 1;
            m_ntpc_2++; 
          }
          if (_nlayers_mms > 0 && local_layer >= _nlayers_maps + _nlayers_intt + _nlayers_tpc)
          {
            mms[local_layer - (_nlayers_maps + _nlayers_intt + _nlayers_tpc)] = 1;
            m_nmms_2++;
          }
        }
      }
      nlintt = 0;
      nlmaps = 0;
      nltpc = 0;
      nlmms = 0;
      if (_nlayers_maps > 0)
      {
        for (unsigned int i = 0; i < _nlayers_maps; i++)
        {
          nlmaps += maps[i];
        }
      }  
      if (_nlayers_intt > 0)
      {
        for (unsigned int i = 0; i < _nlayers_intt; i++)
        {
          nlintt += intt[i];
        }
      }
      if (_nlayers_tpc > 0)
      {
        for (unsigned int i = 0; i < _nlayers_tpc; i++)
        {
          nltpc += tpc[i];
        }
      }
      if (_nlayers_mms > 0)
      {
        for (unsigned int i = 0; i < _nlayers_mms; i++)
        {
          nlmms += mms[i];
        }
      }
      m_nlayers_2 = nlmaps + nlintt + nltpc + nlmms;

      m_mapsstates_2 = 0;
      m_inttstates_2 = 0;
      m_tpcstates_2 = 0;
      m_mmsstates_2 = 0;
      for (auto state_iter = track2->begin_states();
         state_iter != track2->end_states();
         ++state_iter)
      {
        SvtxTrackState* tstate = state_iter->second;
        if (tstate->get_pathlength() != 0) //The first track state is an extrapolation so has no cluster
        {
          auto stateckey = tstate->get_cluskey();
          uint8_t id = TrkrDefs::getTrkrId(stateckey);
    
          switch (id)
          { 
            case TrkrDefs::mvtxId:
              ++m_mapsstates_2;
              break;
            case TrkrDefs::inttId:
              ++m_inttstates_2;
              break;
            case TrkrDefs::tpcId:
              ++m_tpcstates_2;
              break;
            case TrkrDefs::micromegasId:
              ++m_mmsstates_2;
              break;
            default:
             std::cout << "Cluster key doesnt match a tracking system, this shouldn't happen" << std::endl;
             break; 
          }
        }
      }

      if (m_ntpc_2 >= m_nTPC_tag && m_nmaps_2 >= m_nMVTX_tag && m_nintt_2 >= m_nINTT_tag && 
          m_nmms_2 >= m_nTPOT_tag && m_quality_2 <= m_quality_tag)
      {
        m_tagpass_2 = true; 
      }
      else
      {
        m_tagpass_2 = false;
      }

      if (m_ntpc_2 >= m_nTPC_passprobe && m_nmaps_2 >= m_nMVTX_passprobe && m_nintt_2 >= m_nINTT_passprobe && 
          m_nmms_2 >= m_nTPOT_passprobe && m_quality_2 <= m_quality_passprobe)
      {
        m_probepass_2 = true; 
      }
      else
      {
        m_probepass_2 = false;
      }
    
      double pair_dca;
      Acts::Vector3 pca_rel1;
      Acts::Vector3 pca_rel2;
      // assuming stright line tracks for a rough estimate
      Acts::Vector3 A_pos1(track1->get_x(), track1->get_y(), track1->get_z());
      Acts::Vector3 A_mom1(track1->get_px(), track1->get_py(), track1->get_pz());
      Acts::Vector3 A_pos2(track2->get_x(), track2->get_y(), track2->get_z());
      Acts::Vector3 A_mom2(track2->get_px(), track2->get_py(), track2->get_pz());
      findPcaTwoTracks(A_pos1, A_pos2, A_mom1, A_mom2, pca_rel1, pca_rel2, pair_dca);     

      Eigen::Vector3d projected_pos1;
      Eigen::Vector3d projected_mom1;
      Eigen::Vector3d projected_pos2;
      Eigen::Vector3d projected_mom2;

      bool ret1 = projectTrackToPoint(track1, pca_rel1, projected_pos1, projected_mom1);
      bool ret2 = projectTrackToPoint(track2, pca_rel2, projected_pos2, projected_mom2);
     

      // Invariant Mass Calculation Info
      double E1 = sqrt(pow(projected_mom1(0), 2) + pow(projected_mom1(1), 2) + pow(projected_mom1(2), 2) + pow(_pionMass, 2));
      double E2 = sqrt(pow(projected_mom2(0), 2) + pow(projected_mom2(1), 2) + pow(projected_mom2(2), 2) + pow(_pionMass, 2));
      TLorentzVector tra1(projected_mom1(0), projected_mom1(1), projected_mom1(2), E1);
      TLorentzVector tra2(projected_mom2(0), projected_mom2(1), projected_mom2(2), E2);
      TLorentzVector tsum;
      tsum = tra1 + tra2;
      m_invM = tsum.M();

      if (!ret1 || !ret2)
      {
        if (Verbosity() > 5)
        {
          std::cout << "Failed to make track params: track1 - " << ret1 << ", track2 - " << ret2 << std::endl;
        }
      }  

      m_px_proj_1 = projected_mom1(0);
      m_py_proj_1 = projected_mom1(1);
      m_pz_proj_1 = projected_mom1(2);
      m_px_proj_2 = projected_mom2(0);
      m_py_proj_2 = projected_mom2(1);
      m_pz_proj_2 = projected_mom2(2);

      double pair_dca_proj;
      Acts::Vector3 pca_rel1_proj;
      Acts::Vector3 pca_rel2_proj;
      findPcaTwoTracks(projected_pos1, projected_pos2, projected_mom1, projected_mom2, pca_rel1_proj, pca_rel2_proj, pair_dca_proj);

      m_x_proj_1 = pca_rel1_proj(0);
      m_y_proj_1 = pca_rel1_proj(1);
      m_z_proj_1 = pca_rel1_proj(2);
      m_x_proj_2 = pca_rel2_proj(0);
      m_y_proj_2 = pca_rel2_proj(1);
      m_z_proj_2 = pca_rel2_proj(2);
      m_dca = pair_dca_proj;
      
      //KFParticle tr1_kfp = makeParticle(track1);
      //KFParticle tr2_kfp = makeParticle(track2);
      //KFParticle mother;
      //mother.AddDaughter(tr1_kfp);
      //mother.AddDaughter(tr2_kfp);
      //tr1_kfp.SetProductionVertex(mother);
      //tr2_kfp.SetProductionVertex(mother);
      //float calculated_mass_err;
      //mother.GetMass(m_invM_kfp, calculated_mass_err); 

      m_TAPTree->Fill();
 
      ++t2;
    } 
    clearValues();
    ++t1;
  } 
  
  ++m_event;

  return Fun4AllReturnCodes::EVENT_OK;
}

int TagAndProbe::End(PHCompositeNode * /*topNode*/)
{
  std::cout << "TagAndProbe identification finished" << std::endl;

  if (m_save_output)
  {
    m_outfile->cd();
    m_cutInfoTree->Write();
    m_TAPTree->Write();
    m_outfile->Close();
  }

  return 0;
}

void TagAndProbe::initializeBranches()
{
  m_outfile = new TFile(m_outfile_name.c_str(), "RECREATE");  
  m_cutInfoTree = new TTree("CutInfoTree", "CutInfoTree");
  m_TAPTree = new TTree("TAPTree","TAPTree");

  m_cutInfoTree->OptimizeBaskets();
  m_cutInfoTree->SetAutoSave(-5e6);  // Save the output file every 5MB
  m_TAPTree->OptimizeBaskets();
  m_TAPTree->SetAutoSave(-5e6);  // Save the output file every 5MB

  // Tree containing the cut info used for the entries in this root file
  m_cutInfoTree->Branch("tag_nTPC_cut", &m_nTPC_tag, "tag_nTPC_cut/F");
  m_cutInfoTree->Branch("tag_nINTT_cut", &m_nINTT_tag, "tag_nINTT_cut/F");
  m_cutInfoTree->Branch("tag_nMVTX_cut", &m_nMVTX_tag, "tag_nMVTX_cut/F");
  m_cutInfoTree->Branch("tag_nTPOT_cut", &m_nTPOT_tag, "tag_nTPOT_cut/F");
  m_cutInfoTree->Branch("tag_quality_cut", &m_quality_tag, "tag_quality_cut/F");
  m_cutInfoTree->Branch("probe_nTPC_cut", &m_nTPC_passprobe, "probe_nTPC_cut/F");
  m_cutInfoTree->Branch("probe_nINTT_cut", &m_nINTT_passprobe, "probe_nINTT_cut/F");
  m_cutInfoTree->Branch("probe_nMVTX_cut", &m_nMVTX_passprobe, "probe_nMVTX_cut/F");
  m_cutInfoTree->Branch("probe_nTPOT_cut", &m_nTPOT_passprobe, "probe_nTPOT_cut/F");
  m_cutInfoTree->Branch("probe_quality_cut", &m_quality_passprobe, "probe_quality_cut/F");

  // Tree containing all information for pairs of tracks
  // Includes a tag identifying if they passed or failed the cut being investigated 
  m_TAPTree->Branch("run", &m_run, "run/F");
  m_TAPTree->Branch("segment", &m_segment, "segment/F");
  m_TAPTree->Branch("event", &m_event, "event/F");
  //track 1
  m_TAPTree->Branch("trackID_1", &m_trackID_1, "trackID_1/F");
  m_TAPTree->Branch("crossing_1", &m_crossing_1, "crossing_1/F");
  m_TAPTree->Branch("px_1", &m_px_1, "px_1/F");
  m_TAPTree->Branch("py_1", &m_py_1, "py_1/F");
  m_TAPTree->Branch("pz_1", &m_pz_1, "pz_1/F");
  m_TAPTree->Branch("pt_1", &m_pt_1, "pt_1/F");
  m_TAPTree->Branch("px_proj_1", &m_px_proj_1, "px_proj_1/F");
  m_TAPTree->Branch("py_proj_1", &m_py_proj_1, "py_proj_1/F");
  m_TAPTree->Branch("pz_proj_1", &m_pz_proj_1, "pz_proj_1/F");
  //m_TAPTree->Branch("px_proj_kfp_1", &m_px_proj_kfp_1, "px_proj_kfp_1/F");
  //m_TAPTree->Branch("py_proj_kfp_1", &m_py_proj_kfp_1, "py_proj_kfp_1/F");
  //m_TAPTree->Branch("pz_proj_kfp_1", &m_pz_proj_kfp_1, "pz_proj_kfp_1/F");
  m_TAPTree->Branch("x_proj_1", &m_x_proj_1, "x_proj_1/F");
  m_TAPTree->Branch("y_proj_1", &m_y_proj_1, "y_proj_1/F");
  m_TAPTree->Branch("z_proj_1", &m_z_proj_1, "z_proj_1/F");
  m_TAPTree->Branch("eta_1", &m_eta_1, "eta_1/F");
  m_TAPTree->Branch("phi_1", &m_phi_1, "phi_1/F");
  m_TAPTree->Branch("charge_1", &m_charge_1, "charge_1/F");
  m_TAPTree->Branch("quality_1", &m_quality_1, "quality_1/F");
  m_TAPTree->Branch("chisq_1", &m_chisq_1, "chisq_1/F");
  m_TAPTree->Branch("ndf_1", &m_ndf_1, "ndf_1/F");
  m_TAPTree->Branch("nhits_1", &m_nhits_1, "nhits_1/F");
  m_TAPTree->Branch("nlayers_1", &m_nlayers_1, "nlayers_1/F");
  m_TAPTree->Branch("nmaps_1", &m_nmaps_1, "nmaps_1/F");
  m_TAPTree->Branch("nintt_1", &m_nintt_1, "nintt_1/F");
  m_TAPTree->Branch("ntpc_1", &m_ntpc_1, "ntpc_1/F");
  m_TAPTree->Branch("nmms_1", &m_nmms_1, "nmms_1/F");
  m_TAPTree->Branch("mapsstates_1", &m_mapsstates_1, "mapsstates_1/F");
  m_TAPTree->Branch("inttstates_1", &m_inttstates_1, "inttstates_1/F");
  m_TAPTree->Branch("tpcstates_1", &m_tpcstates_1, "tpcstates_1/F");
  m_TAPTree->Branch("mmsstates_1", &m_mmsstates_1, "mmsstates_1/F");
  m_TAPTree->Branch("pca_x_1", &m_pca_x_1, "pca_x_1/F");
  m_TAPTree->Branch("pca_y_1", &m_pca_y_1, "pca_y_1/F");
  m_TAPTree->Branch("pca_z_1", &m_pca_z_1, "pca_z_1/F");
  m_TAPTree->Branch("tagpass_1", &m_tagpass_1, "tagpass_1/F");
  m_TAPTree->Branch("probepass_1", &m_probepass_1, "probepass_1/F");
  //track 2
  m_TAPTree->Branch("trackID_2", &m_trackID_2, "trackID_2/F");
  m_TAPTree->Branch("crossing_2", &m_crossing_2, "crossing_2/F");
  m_TAPTree->Branch("px_2", &m_px_2, "px_2/F");
  m_TAPTree->Branch("py_2", &m_py_2, "py_2/F");
  m_TAPTree->Branch("pz_2", &m_pz_2, "pz_2/F");
  m_TAPTree->Branch("pt_2", &m_pt_2, "pt_2/F");
  m_TAPTree->Branch("px_proj_2", &m_px_proj_2, "px_proj_2/F");
  m_TAPTree->Branch("py_proj_2", &m_py_proj_2, "py_proj_2/F");
  m_TAPTree->Branch("pz_proj_2", &m_pz_proj_2, "pz_proj_2/F");
  //m_TAPTree->Branch("px_proj_kfp_2", &m_px_proj_kfp_2, "px_proj_kfp_2/F");
  //m_TAPTree->Branch("py_proj_kfp_2", &m_py_proj_kfp_2, "py_proj_kfp_2/F");
  //m_TAPTree->Branch("pz_proj_kfp_2", &m_pz_proj_kfp_2, "pz_proj_kfp_2/F");
  m_TAPTree->Branch("x_proj_2", &m_x_proj_2, "x_proj_2/F");
  m_TAPTree->Branch("y_proj_2", &m_y_proj_2, "y_proj_2/F");
  m_TAPTree->Branch("z_proj_2", &m_z_proj_2, "z_proj_2/F");
  m_TAPTree->Branch("eta_2", &m_eta_2, "eta_2/F");
  m_TAPTree->Branch("phi_2", &m_phi_2, "phi_2/F");
  m_TAPTree->Branch("charge_2", &m_charge_2, "charge_2/F");
  m_TAPTree->Branch("quality_2", &m_quality_2, "quality_2/F");
  m_TAPTree->Branch("chisq_2", &m_chisq_2, "chisq_2/F");
  m_TAPTree->Branch("ndf_2", &m_ndf_2, "ndf_2/F");
  m_TAPTree->Branch("nhits_2", &m_nhits_2, "nhits_2/F");
  m_TAPTree->Branch("nlayers_2", &m_nlayers_2, "nlayers_2/F");
  m_TAPTree->Branch("nmaps_2", &m_nmaps_2, "nmaps_2/F");
  m_TAPTree->Branch("nintt_2", &m_nintt_2, "nintt_2/F");
  m_TAPTree->Branch("ntpc_2", &m_ntpc_2, "ntpc_2/F");
  m_TAPTree->Branch("nmms_2", &m_nmms_2, "nmms_2/F");
  m_TAPTree->Branch("mapsstates_2", &m_mapsstates_2, "mapsstates_2/F");
  m_TAPTree->Branch("inttstates_2", &m_inttstates_2, "inttstates_2/F");
  m_TAPTree->Branch("tpcstates_2", &m_tpcstates_2, "tpcstates_2/F");
  m_TAPTree->Branch("mmsstates_2", &m_mmsstates_2, "mmsstates_2/F");
  m_TAPTree->Branch("pca_x_2", &m_pca_x_2, "pca_x_2/F");
  m_TAPTree->Branch("pca_y_2", &m_pca_y_2, "pca_y_2/F");
  m_TAPTree->Branch("pca_z_2", &m_pca_z_2, "pca_z_2/F");
  m_TAPTree->Branch("tagpass_2", &m_tagpass_2, "tagpass_2/F");
  m_TAPTree->Branch("probepass_2", &m_probepass_2, "probepass_2/F");
  //both tracks
  m_TAPTree->Branch("invM", &m_invM, "invM/F");
  //m_TAPTree->Branch("invM_kfp", &m_invM_kfp, "invM_kfp/F");
  m_TAPTree->Branch("dca", &m_dca, "dca/F");
  if (m_include_pv_info)
  {
    m_TAPTree->Branch("vx", &m_vx, "vx/F");
    m_TAPTree->Branch("vy", &m_vy, "vy/F");
    m_TAPTree->Branch("vz", &m_vz, "vz/F");
  }
}

void TagAndProbe::clearValues()
{
  m_trackID_1 = NAN; m_trackID_2 = NAN;
  m_crossing_1 = NAN; m_crossing_2 = NAN;
  m_px_1 = NAN; m_px_2 = NAN;
  m_py_1 = NAN; m_py_2 = NAN;
  m_pz_1 = NAN; m_pz_2 = NAN;
  m_px_proj_1 = NAN; m_px_proj_2 = NAN;
  m_py_proj_1 = NAN; m_py_proj_2 = NAN;
  m_pz_proj_1 = NAN; m_pz_proj_2 = NAN;
  //m_px_proj_kfp_1 = NAN; m_px_proj_kfp_2 = NAN;
  //m_py_proj_kfp_1 = NAN; m_py_proj_kfp_2 = NAN;
  //m_pz_proj_kfp_1 = NAN; m_pz_proj_kfp_2 = NAN;
  m_x_proj_1 = NAN; m_x_proj_2 = NAN;
  m_y_proj_1 = NAN; m_y_proj_2 = NAN;
  m_z_proj_1 = NAN; m_z_proj_2 = NAN;
  m_eta_1 = NAN; m_eta_2 = NAN;
  m_phi_1 = NAN; m_phi_2 = NAN;
  m_charge_1 = NAN; m_charge_2 = NAN;
  m_quality_1 = NAN; m_quality_2 = NAN;
  m_chisq_1 = NAN; m_chisq_2 = NAN;
  m_ndf_1 = NAN; m_ndf_2 = NAN;
  m_nhits_1 = NAN; m_nhits_2 = NAN;
  m_nlayers_1 = NAN; m_nlayers_2 = NAN;
  m_nmaps_1 = NAN; m_nmaps_2 = NAN;
  m_nintt_1 = NAN; m_nintt_2 = NAN;
  m_ntpc_1 = NAN; m_ntpc_2 = NAN;
  m_nmms_1 = NAN; m_nmms_2 = NAN;
  m_mapsstates_1 = NAN; m_mapsstates_2 = NAN;
  m_inttstates_1 = NAN; m_inttstates_2 = NAN;
  m_tpcstates_1 = NAN; m_tpcstates_2 = NAN;
  m_mmsstates_1 = NAN; m_mmsstates_2 = NAN;
  m_pca_x_1 = NAN; m_pca_x_2 = NAN;
  m_pca_y_1 = NAN; m_pca_y_2 = NAN;
  m_pca_z_1 = NAN; m_pca_z_2 = NAN;
  m_tagpass_1 = NAN; m_tagpass_2 = NAN;
  m_probepass_1 = NAN; m_probepass_2 = NAN;

  m_invM = NAN;
  //m_invM_kfp = NAN;
  m_dca = NAN;
  if (m_include_pv_info)
  {
    m_vx = NAN;
    m_vy = NAN;
    m_vz = NAN;
  } 
}

// borrowed from KshortReconstruction
bool TagAndProbe::projectTrackToPoint(SvtxTrack* track, Eigen::Vector3d PCA, Eigen::Vector3d& pos, Eigen::Vector3d& mom)
{
  bool ret = true;

  track->identify();

  /// create perigee surface
  ActsPropagator actsPropagator(m_tGeometry);
  auto perigee = actsPropagator.makeVertexSurface(PCA);  // PCA is in cm here
  auto params = actsPropagator.makeTrackParams(track, m_vertexMap);
  if (!params.ok())
  {
    return false;
  }
  auto result = actsPropagator.propagateTrack(params.value(), perigee);

  if (result.ok())
  {
    auto projectionPos = result.value().second.position(m_tGeometry->geometry().getGeoContext());
    const auto momentum = result.value().second.momentum();
    pos(0) = projectionPos.x() / Acts::UnitConstants::cm;
    pos(1) = projectionPos.y() / Acts::UnitConstants::cm;
    pos(2) = projectionPos.z() / Acts::UnitConstants::cm;

    if (Verbosity() > 2)
    {
      std::cout << "                 Input PCA " << PCA << "  projection out " << pos << std::endl;
    }

    mom(0) = momentum.x();
    mom(1) = momentum.y();
    mom(2) = momentum.z();
  }
  else
  {
    pos(0) = track->get_x();
    pos(1) = track->get_y();
    pos(2) = track->get_z();

    mom(0) = track->get_px();
    mom(1) = track->get_py();
    mom(2) = track->get_pz();

    if(Verbosity() > 0)
      {
	std::cout << result.error() << std::endl;
	std::cout << result.error().message() << std::endl;
	std::cout << " Failed projection of track with: " << std::endl;
	std::cout << " x,y,z = " << track->get_x() << "  " << track->get_y() << "  " << track->get_z() << std::endl;
	std::cout << " px,py,pz = " << track->get_px() << "  " << track->get_py() << "  " << track->get_pz() << std::endl;
	std::cout << " to point (x,y,z) = " << PCA(0) / Acts::UnitConstants::cm << "  " << PCA(1) / Acts::UnitConstants::cm << "  " << PCA(2) / Acts::UnitConstants::cm << std::endl;
      }

    //    ret = false;
  }

  return ret;
}

// borrowed from KshortReconstruction
void TagAndProbe::findPcaTwoTracks(const Acts::Vector3& pos1, const Acts::Vector3& pos2, Acts::Vector3 mom1, Acts::Vector3 mom2, Acts::Vector3& pca1, Acts::Vector3& pca2, double& dca)
{
  TLorentzVector v1;
  TLorentzVector v2;

  double px1 = mom1(0);
  double py1 = mom1(1);
  double pz1 = mom1(2);
  double px2 = mom2(0);
  double py2 = mom2(1);
  double pz2 = mom2(2);

  Float_t E1 = sqrt(pow(px1, 2) + pow(py1, 2) + pow(pz1, 2) + pow(_pionMass, 2));
  Float_t E2 = sqrt(pow(px2, 2) + pow(py2, 2) + pow(pz2, 2) + pow(_pionMass, 2));

  v1.SetPxPyPzE(px1, py1, pz1, E1);
  v2.SetPxPyPzE(px2, py2, pz2, E2);

  // calculate lorentz vector
  const Eigen::Vector3d& a1 = pos1;
  const Eigen::Vector3d& a2 = pos2;

  Eigen::Vector3d b1(v1.Px(), v1.Py(), v1.Pz());
  Eigen::Vector3d b2(v2.Px(), v2.Py(), v2.Pz());

  // The shortest distance between two skew lines described by
  //  a1 + c * b1
  //  a2 + d * b2
  // where a1, a2, are vectors representing points on the lines, b1, b2 are direction vectors, and c and d are scalars
  // dca = (b1 x b2) .(a2-a1) / |b1 x b2|

  // bcrossb/mag_bcrossb is a unit vector perpendicular to both direction vectors b1 and b2
  auto bcrossb = b1.cross(b2);
  auto mag_bcrossb = bcrossb.norm();
  // a2-a1 is the vector joining any arbitrary points on the two lines
  auto aminusa = a2 - a1;

  // The DCA of these two lines is the projection of a2-a1 along the direction of the perpendicular to both
  // remember that a2-a1 is longer than (or equal to) the dca by definition
  dca = 999;
  if (mag_bcrossb != 0)
  {
    dca = bcrossb.dot(aminusa) / mag_bcrossb;
  }
  else
  {
    return;  // same track, skip combination
  }
  
  // get the points at which the normal to the lines intersect the lines, where the lines are perpendicular
  double X = b1.dot(b2) - b1.dot(b1) * b2.dot(b2) / b2.dot(b1);
  double Y = (a2.dot(b2) - a1.dot(b2)) - (a2.dot(b1) - a1.dot(b1)) * b2.dot(b2) / b2.dot(b1);
  double c = Y / X;

  double F = b1.dot(b1) / b2.dot(b1);
  double G = -(a2.dot(b1) - a1.dot(b1)) / b2.dot(b1);
  double d = c * F + G;

  // then the points of closest approach are:
  pca1 = a1 + c * b1;
  pca2 = a2 + d * b2;

  return;
}

/*
// borrowed from KFParticle_sPHENIX
void TagAndProbe::getField()
{
  //This sweeps the sPHENIX magnetic field map from some point radially then grabs the first event that passes the selection
  m_magField = std::filesystem::exists(m_magField) ? m_magField : CDBInterface::instance()->getUrl(m_magField);

  if (Verbosity() > 0)
  {
    std::cout << PHWHERE << ": using fieldmap : " << m_magField << std::endl;
  }

  TFile *fin = new TFile(m_magField.c_str());
  TTree *fieldmap = (TTree *) fin->Get("fieldmap");

  float Bz = 0.;
  unsigned int r = 0.;
  float z = 0.;

  double arc = M_PI/2;
  unsigned int n = 0;

  while (Bz == 0)
  {
    if (n == 4)
    {
      ++r;
    }

    if (r == 3) //Dont go too far out radially
    {
      ++z;
    }

    n = n & 0x3U; //Constrains n from 0 to 3
    r = r & 0x2U;

    double x = r*std::cos(n*arc);
    double y = r*std::sin(n*arc);

    std::string sweep = "x == " + std::to_string(x) + " && y == " + std::to_string(y) + " && z == " + std::to_string(z);

    fieldmap->Draw(">>elist", sweep.c_str(), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get("elist");
    if (elist->GetEntry(0))
    {
      TLeaf *fieldValue = fieldmap->GetLeaf("bz");
      fieldValue->GetBranch()->GetEntry(elist->GetEntry(0));
      Bz = fieldValue->GetValue();
    }

    ++n;

    if (r == 0) // No point in rescanning (0,0)
    {
      ++r;
      n = 0;
    }
  }
  // The actual unit of KFParticle is in kilo Gauss (kG), which is equivalent to 0.1 T, instead of Tesla (T). The positive value indicates the B field is in the +z direction
  Bz *= 10;  // Factor of 10 to convert the B field unit from kG to T
  KFParticle::SetField((double) Bz);

  fieldmap->Delete();
  fin->Close();
}


// borrowed from KFParticle_sPHENIX
KFParticle TagAndProbe::makeParticle(SvtxTrack* track_kfp)  /// Return a KFPTrack from track vector and covariance matrix. No mass or vertex constraints
{
  KFParticle kfp_particle;

  float f_trackParameters[6] = {track_kfp->get_x(),
                                track_kfp->get_y(),
                                track_kfp->get_z(),
                                track_kfp->get_px(),
                                track_kfp->get_py(),
                                track_kfp->get_pz()};

  float f_trackCovariance[21];
  unsigned int iterate = 0;
  for (unsigned int i = 0; i < 6; ++i)
  {
    for (unsigned int j = 0; j <= i; ++j)
    {
      f_trackCovariance[iterate] = track_kfp->get_error(i, j);
      ++iterate;
    }
  }

  kfp_particle.Create(f_trackParameters, f_trackCovariance, (Int_t) track_kfp->get_charge(), -1);
  kfp_particle.NDF() = track_kfp->get_ndf();
  kfp_particle.Chi2() = track_kfp->get_chisq();
  kfp_particle.SetId(track_kfp->get_id());

  return kfp_particle;
}
*/

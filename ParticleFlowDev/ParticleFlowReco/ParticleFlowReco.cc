#include "ParticleFlowReco.h"

#include "ParticleFlowElementContainer.h"
#include "ParticleFlowElementv1.h"

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackState.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHRandomSeed.h>
#include <phool/getClass.h>

#include <CLHEP/Vector/ThreeVector.h>

#include <TLorentzVector.h>
#include <TVectorD.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>  // for gsl_rng_uniform_pos

#include <cmath>
#include <iostream>

// examine second value of std::pair, sort by smallest
bool sort_by_pair_second_lowest(const std::pair<int, float> &a, const std::pair<int, float> &b)
{
  return (a.second < b.second);
}

float ParticleFlowReco::calculate_dR(float eta1, float eta2, float phi1, float phi2)
{
  float deta = eta1 - eta2;
  float dphi = phi1 - phi2;
  while (dphi > M_PI)
  {
    dphi -= 2 * M_PI;
  }
  while (dphi < -M_PI)
  {
    dphi += 2 * M_PI;
  }
  return sqrt(pow(deta, 2) + pow(dphi, 2));
}

std::pair<float, float> ParticleFlowReco::get_expected_signature(int trk)
{
  float response = (0.553437 + 0.0572246 * log(_pflow_TRK_p[trk])) * _pflow_TRK_p[trk];
  float resolution = sqrt(pow(0.119123, 2) + pow(0.312361, 2) / _pflow_TRK_p[trk]) * _pflow_TRK_p[trk];

  std::pair<float, float> expected_signature(response, resolution);

  return expected_signature;
}

//____________________________________________________________________________..
ParticleFlowReco::ParticleFlowReco(const std::string &name)
  : SubsysReco(name)
  , _only_crossing_zero(true)
  , _dont_use_global_vertex(false)
  , _energy_match_Nsigma(1.5)
{
}

//____________________________________________________________________________..
int ParticleFlowReco::InitRun(PHCompositeNode *topNode)
{
  return CreateNode(topNode);
}

//____________________________________________________________________________..
int ParticleFlowReco::process_event(PHCompositeNode *topNode)
{
  if(Verbosity() > 0)
  {
    std::cout << "ParticleFlowReco::process_event with Nsigma = " << _energy_match_Nsigma << std::endl;
  }
  // get handle to pflow node
  _pflowContainer = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");
  if(!_pflowContainer)
  {
    std::cout << " ERROR -- can't find ParticleFlowElements node after it should have been created" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // used for indexing PFlow elements in container
  _global_pflow_index = 0;

  // read in tower geometries
  _geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  _geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  _geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  if(_isEMCalDetailedGeo)
  {
    _geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC_DETAILED");

    if(!_geomEM)
    {
      std::cout << "ParticleFlowReco::process_event : FATAL ERROR, cannot find TOWERGEOM_CEMC_DETAILED" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }
  else
  {
    if(!_geomEM)
    {
      std::cout << "ParticleFlowReco::process_event : FATAL ERROR, cannot find TOWERGEOM_CEMC" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  if(!_geomIH)
  {
    std::cout << "ParticleFlowReco::process_event : FATAL ERROR, cannot find TOWERGEOM_HCALIN" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(!_geomOH)
  {
    std::cout << "ParticleFlowReco::process_event : FATAL ERROR, cannot find TOWERGEOM_HCALOUT" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _trackmap = findNode::getClass<SvtxTrackMap>(topNode, _track_map_name);
  if(!_trackmap)
  {
    std::cout << "ParticleFlowReco::process_event : FATAL ERROR, cannot find SvtxTrackMap" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _clustersEM = findNode::getClass<RawClusterContainer>(topNode, "TOPOCLUSTER_EMCAL");

  if(!_clustersEM)
  {
    std::cout << "ParticleFlowReco::process_event : FATAL ERROR, cannot find cluster container TOPOCLUSTER_EMCAL" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _clustersHAD = findNode::getClass<RawClusterContainer>(topNode, "TOPOCLUSTER_HCAL");

  if(!_clustersHAD)
  {
    std::cout << "ParticleFlowReco::process_event : FATAL ERROR, cannot find cluster container TOPOCLUSTER_HCAL" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _global_vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if((!_global_vertexmap) && (_dont_use_global_vertex == false))
  {
    std::cout << "ParticleFlowReco::process_event : FATAL ERROR, cannot find GlobalVertexMap" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _svtx_vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if(!_svtx_vertexmap)
  {
    std::cout << "ParticleFlowReco::process_event : FATAL ERROR, cannot find SvtxVertexMap" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }



  ResetRepresentation();

  if(Verbosity() > 2)
  {
    std::cout << "ParticleFlowReco::process_event : initial population of TRK, EM, HAD objects " << std::endl;
  }

  PrepareTracks();

  PrepareEMCalClusters();

  PrepareHCalClusters();

  // BEGIN LINKING STEP

  // Link TRK -> EM (best match, but keep reserve of others), and TRK -> HAD (best match)
  if(Verbosity() > 2)
  {
    std::cout << "ParticleFlowReco::process_event : TRK -> EM and TRK -> HAD linking " << std::endl;
  }

  LinkTrackToCaloClusters();

  // EM->HAD linking
  if(Verbosity() > 2)
  {
    std::cout << "ParticleFlowReco::process_event : EM -> HAD linking " << std::endl;
  }

  LinkEMCalToHCalClusters();

  // SEQUENTIAL MATCHING: if TRK -> EM and EM -> HAD, ensure that TRK -> HAD
  if(Verbosity() > 2)
  {
    std::cout << "ParticleFlowReco::process_event : sequential TRK -> EM && EM -> HAD ==> TRK -> HAD matching " << std::endl;
  }

  //LinkTrackToHCalViaEMCal();

  // TRK->EM->HAD removal
  if(Verbosity() > 2)
  {
    std::cout << "ParticleFlowReco::process_event : resolve TRK(s) + EM(s) -> HAD systems " << std::endl;
  }

  ProcessMatchedHCalClusters();

  // TRK->EM removal

  if(Verbosity() > 2)
  {
    std::cout << "ParticleFlowReco::process_event : resolve TRK(s) -> EM(s) ( + no HAD) systems " << std::endl;
  }

  ProcessMatchedEMCalClusters();

  // now remove unmatched elements

  if(Verbosity() > 2)
  {
    std::cout << "ParticleFlowReco::process_event : remove TRK-unlinked EMs and HADs " << std::endl;
  }

  ProcessClusterOnlyMatch();

  ProcessUnmatchedEMCalClusters();

  ProcessUnmatchedHCalClusters();

  ProcessUnmatchedTracks();

  // DEBUG: print out all PFLow elements
  if(Verbosity() > 5)
  {
    std::cout << "ParticleFlowReco::process_event: summary of PFlow objects " << std::endl;

    ParticleFlowElementContainer::ConstRange begin_end = _pflowContainer->getParticleFlowElements();
    for(ParticleFlowElementContainer::ConstIterator hiter = begin_end.first; hiter != begin_end.second; ++hiter)
    {
      hiter->second->identify();
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

void ParticleFlowReco::ResetRepresentation()
{
  // reset internal particle-flow representation
  _pflow_TRK_p.clear();
  _pflow_TRK_vertex.clear();
  _pflow_TRK_eta.clear();
  _pflow_TRK_phi.clear();
  _pflow_TRK_match_EM.clear();
  _pflow_TRK_match_HAD.clear();
  _pflow_TRK_addtl_match_EM.clear();
  _pflow_TRK_trk.clear();
  _pflow_TRK_projections.clear();
  /*
  _pflow_TRK_EMproj_x.clear();
  _pflow_TRK_EMproj_y.clear();
  _pflow_TRK_EMproj_z.clear();
  _pflow_TRK_HADproj_x.clear();
  _pflow_TRK_HADproj_y.clear();
  _pflow_TRK_HADproj_z.clear();
  */

  _pflow_EM_E.clear();
  _pflow_EM_x.clear();
  _pflow_EM_y.clear();
  _pflow_EM_z.clear();
  _pflow_EM_tower_x.clear();
  _pflow_EM_tower_y.clear();
  _pflow_EM_tower_z.clear();
  _pflow_EM_match_HAD.clear();
  _pflow_EM_match_TRK.clear();
  _pflow_EM_cluster.clear();

  _pflow_HAD_E.clear();
  _pflow_HAD_x.clear();
  _pflow_HAD_y.clear();
  _pflow_HAD_z.clear();
  _pflow_HAD_tower_x.clear();
  _pflow_HAD_tower_y.clear();
  _pflow_HAD_tower_z.clear();
  _pflow_HAD_match_EM.clear();
  _pflow_HAD_match_TRK.clear();
  _pflow_HAD_cluster.clear();
}

void ParticleFlowReco::PrepareTracks()
{
  for(auto &iter : *_trackmap)
  {
    SvtxTrack *track = iter.second;

    if(!isAcceptableTrack(track))
    {
      continue;
    }

    if(Verbosity() > 2)
    {
      std::cout << "Track with p= " << track->get_p() << ", eta / phi = "
                << track->get_eta() << " / " << track->get_phi()
                << std::endl;
    }

    _pflow_TRK_trk.push_back(track);
    _pflow_TRK_vertex.push_back(track->get_vertex_id());
    _pflow_TRK_p.push_back(track->get_p());
    _pflow_TRK_eta.push_back(track->get_eta());
    _pflow_TRK_phi.push_back(track->get_phi());
    _pflow_TRK_match_EM.emplace_back();
    _pflow_TRK_match_HAD.emplace_back();
    _pflow_TRK_addtl_match_EM.emplace_back();

    std::vector<SvtxTrackState*> track_states(6);

    track_states[0] = track->get_state(_geomEM->get_radius());
    track_states[1] = track->get_state(_geomEM->get_radius()+_geomEM->get_thickness());
    track_states[2] = track->get_state(_geomIH->get_radius());
    track_states[3] = track->get_state(_geomIH->get_radius()+_geomIH->get_thickness());
    track_states[4] = track->get_state(_geomOH->get_radius());
    track_states[5] = track->get_state(_geomOH->get_radius()+_geomOH->get_thickness());

    /// Get the track projections. If they failed for some reason, just use the track
    /// phi and eta values at the point of closest approach
    std::vector<CLHEP::Hep3Vector> this_projection(6);

    for(unsigned int s = 0; s < this_projection.size(); s++)
    {
      if(track_states[s])
      {
        this_projection.at(s) = CLHEP::Hep3Vector(track_states[s]->get_x(), track_states[s]->get_y(), track_states[s]->get_z());
      }
      else
      {
        this_projection.at(s) = CLHEP::Hep3Vector(NAN, NAN, NAN);
      }
    }

    _pflow_TRK_projections.push_back(this_projection);
  }
}

void ParticleFlowReco::PrepareEMCalClusters()
{
  RawClusterContainer::ConstRange begin_end = _clustersEM->getClusters();
  for(RawClusterContainer::ConstIterator hiter = begin_end.first; hiter != begin_end.second; ++hiter)
  {
    float cluster_E = hiter->second->get_energy();
    if(cluster_E < 0.2)
    {
      continue;
    }

    _pflow_EM_E.push_back(cluster_E);
    _pflow_EM_x.push_back(hiter->second->get_x());
    _pflow_EM_y.push_back(hiter->second->get_y());
    _pflow_EM_z.push_back(hiter->second->get_z());
    _pflow_EM_cluster.push_back(hiter->second);
    _pflow_EM_match_HAD.emplace_back();
    _pflow_EM_match_TRK.emplace_back();

    std::vector<float> this_cluster_tower_x;
    std::vector<float> this_cluster_tower_y;
    std::vector<float> this_cluster_tower_z;

    // read in towers
    RawCluster::TowerConstRange begin_end_towers = hiter->second->get_towers();
    for(RawCluster::TowerConstIterator iter = begin_end_towers.first; iter != begin_end_towers.second; ++iter)
    {
      if(RawTowerDefs::decode_caloid(iter->first) == RawTowerDefs::CalorimeterId::CEMC)
      {
        RawTowerGeom *tower_geom = _geomEM->get_tower_geometry(iter->first);

        this_cluster_tower_x.push_back(tower_geom->get_center_x());
        this_cluster_tower_y.push_back(tower_geom->get_center_y());
        this_cluster_tower_z.push_back(tower_geom->get_center_z());
      }
      else
      {
        std::cout << "ParticleFlowReco::process_event : FATAL ERROR , EM topoClusters seem to contain HCal towers" << std::endl;
      }
    }  // close tower loop

    _pflow_EM_tower_x.push_back(this_cluster_tower_x);
    _pflow_EM_tower_y.push_back(this_cluster_tower_y);
    _pflow_EM_tower_z.push_back(this_cluster_tower_z);

  }  // close cluster loop
}

void ParticleFlowReco::PrepareHCalClusters()
{
  RawClusterContainer::ConstRange begin_end = _clustersHAD->getClusters();
  for(RawClusterContainer::ConstIterator hiter = begin_end.first; hiter != begin_end.second; ++hiter)
  {
    float cluster_E = hiter->second->get_energy();
    if(cluster_E < 0.2)
    {
      continue;
    }

    _pflow_HAD_E.push_back(cluster_E);
    _pflow_HAD_x.push_back(hiter->second->get_x());
    _pflow_HAD_y.push_back(hiter->second->get_y());
    _pflow_HAD_z.push_back(hiter->second->get_z());
    _pflow_HAD_cluster.push_back(hiter->second);

    _pflow_HAD_match_EM.emplace_back();
    _pflow_HAD_match_TRK.emplace_back();

    std::vector<float> this_cluster_tower_x;
    std::vector<float> this_cluster_tower_y;
    std::vector<float> this_cluster_tower_z;

    // read in towers
    RawCluster::TowerConstRange begin_end_towers = hiter->second->get_towers();
    for(RawCluster::TowerConstIterator iter = begin_end_towers.first; iter != begin_end_towers.second; ++iter)
    {
      if(RawTowerDefs::decode_caloid(iter->first) == RawTowerDefs::CalorimeterId::HCALIN)
      {
        RawTowerGeom *tower_geom = _geomIH->get_tower_geometry(iter->first);

        this_cluster_tower_x.push_back(tower_geom->get_center_x());
        this_cluster_tower_y.push_back(tower_geom->get_center_y());
        this_cluster_tower_z.push_back(tower_geom->get_center_z());
      }

      else if(RawTowerDefs::decode_caloid(iter->first) == RawTowerDefs::CalorimeterId::HCALOUT)
      {
        RawTowerGeom *tower_geom = _geomOH->get_tower_geometry(iter->first);

        this_cluster_tower_x.push_back(tower_geom->get_center_x());
        this_cluster_tower_y.push_back(tower_geom->get_center_y());
        this_cluster_tower_z.push_back(tower_geom->get_center_z());
      }
      else
      {
        std::cout << "ParticleFlowReco::process_event : FATAL ERROR , HCal topoClusters seem to contain EM towers" << std::endl;
      }

    }  // close tower loop

    _pflow_HAD_tower_x.push_back(this_cluster_tower_x);
    _pflow_HAD_tower_y.push_back(this_cluster_tower_y);
    _pflow_HAD_tower_z.push_back(this_cluster_tower_z);

  }  // close cluster loop
}

void ParticleFlowReco::LinkTrackToCaloClusters()
{
  for(unsigned int trk = 0; trk < _pflow_TRK_p.size(); trk++)
  {
    if(Verbosity() > 10)
    {
      std::cout << " TRK " << trk << " with p / eta / phi = " << _pflow_TRK_p[trk] << " / " << _pflow_TRK_eta[trk] << " / " << _pflow_TRK_phi[trk] << std::endl;
    }

    //CLHEP::Hep3Vector trackproj_emcal(_pflow_TRK_EMproj_x[trk], _pflow_TRK_EMproj_y[trk], _pflow_TRK_EMproj_z[trk]);

    //CLHEP::Hep3Vector trackproj_hcal(_pflow_TRK_HADproj_x[trk], _pflow_TRK_HADproj_y[trk], _pflow_TRK_HADproj_z[trk]);

    CLHEP::Hep3Vector vertex = GetVertexForCalorimeter();

    // TRK -> EM link
    float min_em_dR = 0.2;
    int min_em_index = -1;

    for(unsigned int em = 0; em < _pflow_EM_E.size(); em++)
    {
      CLHEP::Hep3Vector emcal_cluster(_pflow_EM_x[em], _pflow_EM_y[em], _pflow_EM_z[em]);
      emcal_cluster = emcal_cluster - vertex;

      CLHEP::Hep3Vector track_proj = Minimize_PhiEta(emcal_cluster, trk, vertex, false);

      //If the projection doesn't reach the calorimeter, we should not try to match
      if(std::isnan(track_proj.x()))
      {
        continue;
      }

      float dR = fabs(track_proj.deltaR(emcal_cluster));

      if(dR > 0.2)
      {
        continue;
      }

      bool has_overlap = false;

      if(_use_overlap_match)
      {
        for(unsigned int tow = 0; tow < _pflow_EM_tower_x.at(em).size(); tow++)
        {
          CLHEP::Hep3Vector tower(_pflow_EM_tower_x.at(em).at(tow), _pflow_EM_tower_y.at(em).at(tow), _pflow_EM_tower_z.at(em).at(tow));
          tower = tower - vertex;

          float deta = track_proj.eta() - tower.eta();
          float dphi = track_proj.deltaPhi(tower);

          if(fabs(deta) < 0.025 * _trk_emc_overlap_eta && fabs(dphi) < 0.025 * _trk_emc_overlap_phi)
          {
            has_overlap = true;
            break;
          }
        }
      }
      else
      {
        has_overlap = true;
      }

      if(has_overlap)
      {
        if(Verbosity() > 5)
        {
          std::cout << " -> possible match to EM " << em << " with dR = " << dR << std::endl;
        }

        _pflow_TRK_addtl_match_EM.at(trk).push_back(std::pair<int, float>(em, dR));
      }
      else
      {
        if(Verbosity() > 5)
        {
          std::cout << " -> no match to EM " << em << " (even though dR = " << dR << " )" << std::endl;
        }
      }
    }

    // sort possible matches

    std::sort(_pflow_TRK_addtl_match_EM.at(trk).begin(), _pflow_TRK_addtl_match_EM.at(trk).end(), sort_by_pair_second_lowest);
    if(Verbosity() > 10)
    {
      for(auto &n : _pflow_TRK_addtl_match_EM.at(trk))
      {
        std::cout << " -> sorted list of matches, EM / dR = " << n.first << " / " << n.second << std::endl;
      }
    }

    if(_pflow_TRK_addtl_match_EM.at(trk).size() > 0)
    {
      min_em_index = _pflow_TRK_addtl_match_EM.at(trk).at(0).first;
      min_em_dR = _pflow_TRK_addtl_match_EM.at(trk).at(0).second;
      // delete best matched element
      _pflow_TRK_addtl_match_EM.at(trk).erase(_pflow_TRK_addtl_match_EM.at(trk).begin());
    }

    if(min_em_index > -1)
    {
      _pflow_EM_match_TRK.at(min_em_index).push_back(trk);
      _pflow_TRK_match_EM.at(trk).push_back(min_em_index);

      if(Verbosity() > 5)
      {
        std::cout << " -> matched EM " << min_em_index << " with pt / x / y / z = " << _pflow_EM_E.at(min_em_index) << " / " << _pflow_EM_x.at(min_em_index) << " / " << _pflow_EM_y.at(min_em_index) << " / " << _pflow_EM_z.at(min_em_index) << ", dR = " << min_em_dR;
        std::cout << " ( " << _pflow_TRK_addtl_match_EM.at(trk).size() << " other possible matches ) " << std::endl;
      }
    }
    else
    {
      if(Verbosity() > 5)
      {
        std::cout << " -> no EM match! ( best dR = " << min_em_dR << " ) " << std::endl;
      }
    }

    // TRK -> HAD link
    float min_had_dR = 0.2;
    int min_had_index = -1;
    float max_had_pt = 0;

    // TODO: sequential linking should better happen here -- i.e. allow EM-matched HAD's into the possible pool
    for(unsigned int had = 0; had < _pflow_HAD_E.size(); had++)
    {
      CLHEP::Hep3Vector hcal_cluster(_pflow_HAD_x[had], _pflow_HAD_y[had], _pflow_HAD_z[had]);
      hcal_cluster = hcal_cluster - vertex;

      CLHEP::Hep3Vector track_proj = Minimize_PhiEta(hcal_cluster, trk, vertex, true);

      //If the projection doesn't reach the calorimeter, we should not try to match
      if(std::isnan(track_proj.x()))
      {
        continue;
      }

      float dR = fabs(track_proj.deltaR(hcal_cluster));

      if(dR > 0.5)
      {
        continue;
      }

      bool has_overlap = false;

      if(_use_overlap_match)
      {
        for(unsigned int tow = 0; tow < _pflow_HAD_tower_x.at(had).size(); tow++)
        {
          CLHEP::Hep3Vector tower(_pflow_HAD_tower_x.at(had).at(tow), _pflow_HAD_tower_y.at(had).at(tow), _pflow_HAD_tower_z.at(had).at(tow));
          tower = tower - vertex;

          float deta = track_proj.eta() - tower.eta();
          float dphi = track_proj.deltaPhi(tower);

          if(fabs(deta) < 0.1 * _trk_had_overlap_eta && fabs(dphi) < 0.1 * _trk_had_overlap_phi)
          {
            has_overlap = true;
            break;
          }
        }
      }
      else
      {
        has_overlap = true;
      }

      if(has_overlap)
      {
        if(Verbosity() > 5)
        {
          std::cout << " -> possible match to HAD " << had << " with dR = " << dR << std::endl;
        }

        if(_pflow_HAD_E.at(had) > max_had_pt)
        {
          max_had_pt = _pflow_HAD_E.at(had);
          min_had_index = had;
          min_had_dR = dR;
        }
      }
      else
      {
        if(Verbosity() > 5)
        {
          std::cout << " -> no match to HAD " << had << " (even though dR = " << dR << " )" << std::endl;
        }
      }
    }

    if(min_had_index > -1)
    {
      _pflow_HAD_match_TRK.at(min_had_index).push_back(trk);
      _pflow_TRK_match_HAD.at(trk).push_back(min_had_index);

      if(Verbosity() > 5)
      {
        std::cout << " -> matched HAD " << min_had_index << " with E / x / y / z = " << _pflow_HAD_E.at(min_had_index) << " / " << _pflow_HAD_x.at(min_had_index) << " / " << _pflow_HAD_y.at(min_had_index) << " / " << _pflow_HAD_z.at(min_had_index) << ", dR = " << min_had_dR << std::endl;
      }
    }
    else
    {
      if(Verbosity() > 5)
      {
        std::cout << " -> no HAD match! ( best dR = " << min_had_dR << " ) " << std::endl;
      }
    }
  }
}

void ParticleFlowReco::LinkEMCalToHCalClusters()
{

  for(unsigned int had = 0; had < _pflow_HAD_E.size(); had++)
  {
    CLHEP::Hep3Vector hcal_cluster(_pflow_HAD_x[had], _pflow_HAD_y[had], _pflow_HAD_z[had]);

    for(unsigned int em = 0; em < _pflow_EM_E.size(); em++)
    {
      CLHEP::Hep3Vector emcal_cluster(_pflow_EM_x[em], _pflow_EM_y[em], _pflow_EM_z[em]);

      if(Verbosity() > 10)
      {
        std::cout << " EM with E / x / y / z = " << _pflow_EM_E[em] << " / " << _pflow_EM_x[em] << " / " << _pflow_EM_y[em] << " / " << _pflow_EM_z[em] << std::endl;
      }

      float dR = fabs(emcal_cluster.deltaR(hcal_cluster));

      if(dR > 0.5)
      {
        continue;
      }

      bool has_overlap = false;

      if(_use_overlap_match)
      {
        for(unsigned int tow = 0; tow < _pflow_EM_tower_x.at(em).size(); tow++)
        {
          CLHEP::Hep3Vector tower(_pflow_EM_tower_x.at(em).at(tow), _pflow_EM_tower_y.at(em).at(tow), _pflow_EM_tower_z.at(em).at(tow));

          float deta = hcal_cluster.eta() - tower.eta();
          float dphi = hcal_cluster.deltaPhi(tower);

          if(fabs(deta) < 0.1 * _emc_had_overlap_eta && fabs(dphi) < 0.1 * _emc_had_overlap_phi)
          {
            has_overlap = true;
            break;
          }
        }
      }
      else
      {
        has_overlap = true;
      }

      if(has_overlap)
      {
        if(Verbosity() > 5)
        {
          std::cout << " -> possible EM match to HAD " << em << " with dR = " << dR << std::endl;
        }
/*
        if(dR < min_dR) //We are going to look for neutral hadrons, match the closest cluster
        {
          min_em_index = em;
          min_dR = dR;
        }
        */
        //We associate all EM matched to HAD, later, after track matching, we selected the closest to HAD
        _pflow_HAD_match_EM.at(had).push_back(em);
        _pflow_EM_match_HAD.at(em).push_back(had);
      }
      else
      {
        if(Verbosity() > 5)
        {
          std::cout << " -> no match to HAD " << had << " (even though dR = " << dR << " )" << std::endl;
        }
      }
    }
/*
    if(min_em_index > -1)
    {
      _pflow_HAD_match_EM.at(had).push_back(min_em_index);
      _pflow_EM_match_HAD.at(min_em_index).push_back(had);

      if(Verbosity() > 5)
      {
        std::cout << " -> matched EM to HAD with E / x / y / z = " << _pflow_EM_E.at(min_em_index) << " / " << _pflow_EM_x.at(min_em_index) << " / " << _pflow_EM_y.at(min_em_index) << " / " << _pflow_EM_z.at(min_em_index) << ", dR = " << min_dR << std::endl;
      }
    }
    else
    {
      if(Verbosity() > 5)
      {
        std::cout << " -> no EM match! ( best dR = " << min_dR << " ) " << std::endl;
      }
    }
    */
  }
}

void ParticleFlowReco::LinkTrackToHCalViaEMCal()
{
  for(unsigned int trk = 0; trk < _pflow_TRK_p.size(); trk++)
  {
    // go through all matched EMs
    for(unsigned int i = 0; i < _pflow_TRK_match_EM.at(trk).size(); i++)
    {
      int em = _pflow_TRK_match_EM.at(trk).at(i);

      // if this EM has a matched HAD...
      for(unsigned int j = 0; j < _pflow_EM_match_HAD.at(em).size(); j++)
      {
        int had = _pflow_EM_match_HAD.at(em).at(j);

        // and the TRK is NOT matched to this HAD...
        bool is_trk_matched_to_HAD = false;
        for(int existing_had : _pflow_TRK_match_HAD.at(trk))
        {
          if(had == existing_had)
          {
            is_trk_matched_to_HAD = true;
          }
        }

        // if this is the case, create TRK->HAD link
        if(!is_trk_matched_to_HAD)
        {
          _pflow_TRK_match_HAD.at(trk).push_back(had);
          _pflow_HAD_match_TRK.at(had).push_back(trk);

          if(Verbosity() > 5)
          {
            std::cout << " TRK " << trk << " with pt / eta / phi = " << _pflow_TRK_p.at(trk) << " / " << _pflow_TRK_eta.at(trk) << " / " << _pflow_TRK_phi.at(trk) << std::endl;
            std::cout << " -> sequential match to HAD " << had << " through EM " << j << std::endl;
          }
        }

      }  // close the HAD loop

    }  // close the EM loop

  }  // close the TRK loop
}

void ParticleFlowReco::ProcessMatchedHCalClusters()
{
  for(unsigned int had = 0; had < _pflow_HAD_E.size(); had++)
  {
    // only consider HAD with matched tracks ... others we will deal with later
    if(_pflow_HAD_match_TRK.at(had).size() == 0)
    {
      continue;
    }

    if(Verbosity() > 5)
    {
      std::cout << " HAD " << had << " with E / x / y / z = " << _pflow_HAD_E.at(had) << " / " << _pflow_HAD_x.at(had) << " / " << _pflow_HAD_y.at(had) << " / " << _pflow_HAD_z.at(had) << std::endl;
    }

    // setup for Sum-pT^trk -> calo prediction
    float total_TRK_p = 0;
    float total_expected_E = 0;
    float total_expected_E_var = 0;

    // begin with this HAD calo energy
    float total_EMHAD_E = _pflow_HAD_E.at(had);

    std::vector<RawCluster*> matchedEClusters_this_had_via_trk;

    //Remove later
    //std::vector<int> matchedEClustersIDs;
/*
    // iterate over the EMs matched to this HAD
    for(int em : _pflow_HAD_match_EM.at(had))
    {
      // ensure there is at least one track matched to this EM
      if(_pflow_EM_match_TRK.at(em).size() == 0)
      {
        continue;
      }

      // add it to the total calo E
      total_EMHAD_E += _pflow_EM_E.at(em);
      matchedEClusters.push_back(_pflow_EM_cluster.at(em));
      //matchedEClustersIDs.push_back(em);
      if(Verbosity() > 5)
      {
        std::cout << " -> -> LINKED EM " << em << " with E / x / y / z = " << _pflow_EM_E.at(em) << " / " << _pflow_EM_x.at(em) << " / " << _pflow_EM_y.at(em) << " / " << _pflow_EM_z.at(em) << std::endl;
      }
    }
*/
    //In case of EM leftover
    std::vector<SvtxTrack*> assoc_tracks;

    // iterate over the TRKs matched to this HAD
    for(unsigned int j = 0; j < _pflow_HAD_match_TRK.at(had).size(); j++)
    {
      int trk = _pflow_HAD_match_TRK.at(had).at(j);

      if(Verbosity() > 5)
      {
        std::cout << " -> -> LINKED TRK " << trk << " with p / eta / phi = " << _pflow_TRK_p.at(trk) << " / " << _pflow_TRK_eta.at(trk) << " / " << _pflow_TRK_phi.at(trk) << std::endl;
      }

      total_TRK_p += _pflow_TRK_p.at(trk);

      std::pair<float, float> expected_signature = get_expected_signature(trk);

      float expected_E_mean = expected_signature.first;
      float expected_E_sigma = expected_signature.second;

      if(Verbosity() > 5)
      {
        std::cout << " -> -> -> expected calo signature is " << expected_E_mean << " +/- " << expected_E_sigma << std::endl;
      }

      total_expected_E += expected_E_mean;
      total_expected_E_var += pow(expected_E_sigma, 2);

      std::vector<RawCluster *> matchedEClusters_this_track;

      for(unsigned int em = 0; em < _pflow_TRK_match_EM.at(trk).size(); em++)
      {
        total_EMHAD_E += _pflow_EM_E.at(_pflow_TRK_match_EM.at(trk).at(em));
        matchedEClusters_this_track.push_back(_pflow_EM_cluster.at(_pflow_TRK_match_EM.at(trk).at(em)));
        if(std::find(matchedEClusters_this_had_via_trk.begin(), matchedEClusters_this_had_via_trk.end(), _pflow_EM_cluster.at(_pflow_TRK_match_EM.at(trk).at(em))) == matchedEClusters_this_had_via_trk.end())
        {
          matchedEClusters_this_had_via_trk.push_back(_pflow_EM_cluster.at(_pflow_TRK_match_EM.at(trk).at(em)));
        }
      }

      // add PFlow element for each track
      ParticleFlowElement *pflow = new ParticleFlowElementv1();

      // assume pion mass
      float pfE = std::sqrt((_pflow_TRK_p[trk]*_pflow_TRK_p[trk]) + (0.135*0.135));

      CLHEP::Hep3Vector pflow_vector;
      pflow_vector.setREtaPhi(_pflow_TRK_p[trk], _pflow_TRK_eta[trk], _pflow_TRK_phi[trk]);

      pflow->set_px(pflow_vector.getX());
      pflow->set_py(pflow_vector.getY());
      pflow->set_pz(pflow_vector.getZ());
      pflow->set_e(pfE);
      pflow->set_track(_pflow_TRK_trk[trk]);
      pflow->set_eclusters(matchedEClusters_this_track);
      pflow->set_hcluster(_pflow_HAD_cluster.at(had));
      pflow->set_id(_global_pflow_index);
      pflow->set_type(ParticleFlowElement::PFLOWTYPE::MATCHED_CHARGED_HADRON);
/*
      std::cout << "Creating particle flow element" << std::endl;
      std::cout << "PF ID: " << _global_pflow_index << " as MATCHED_CHARGED_HADRON" << std::endl;
      std::cout << "Track ID: " << trk << std::endl;
      for(unsigned int iemc = 0; iemc < matchedEClustersIDs.size(); iemc++)
      {
        std::cout << "EMCal ID: " << matchedEClustersIDs.at(iemc) << std::endl;
      }
      std::cout << "HCal ID: " << had << std::endl;
*/

      _pflowContainer->AddParticleFlowElement(_global_pflow_index, pflow);
      _global_pflow_index++;
      assoc_tracks.push_back(_pflow_TRK_trk[trk]);
    }
    // Track + E+HCal PF elements are created

    // process compatibility of fit
    float total_expected_E_err = sqrt(total_expected_E_var);

    if(Verbosity() > 5)
    {
      std::cout << " -> Total track Sum p = " << total_TRK_p << " , expected calo Sum E = " << total_expected_E << " +/- " << total_expected_E_err << " , observed EM+HAD Sum E = " << total_EMHAD_E << std::endl;
    }

    // if Sum pT > calo, add in additional possible matched EMs associated with tracks until that is no longer the case
    //Remove later
    //std::vector<int> additional_EMs_IDs;
/*
    if(total_expected_E > total_EMHAD_E)
    {
      if(Verbosity() > 5)
      {
        std::cout << " -> Expected E > Observed E, looking for additional potential TRK->EM matches" << std::endl;
      }

      std::map<int, float> additional_EMs;

      for(int trk : _pflow_HAD_match_TRK.at(had))
      {
        int addtl_matches = _pflow_TRK_addtl_match_EM.at(trk).size();

        if(Verbosity() > 10)
        {
          std::cout << " -> -> TRK " << trk << " has " << addtl_matches << " additional matches! " << std::endl;
        }

        for(auto &n : _pflow_TRK_addtl_match_EM.at(trk))
        {
          if(Verbosity() > 10)
          {
            std::cout << " -> -> -> additional match to EM = " << n.first << " with dR = " << n.second << std::endl;
          }

          float existing_dR = 0.21;
          int counts = additional_EMs.count(n.first);
          if(counts > 0)
          {
            existing_dR = additional_EMs[n.first];
          }
          if(n.second < existing_dR)
          {
            additional_EMs[n.first] = n.second;
          }
        }
      }

      // map now assured to have only minimal dR values for each possible additional EM
      // translate the map to a vector of pairs, then sort by smallest dR

      std::vector<std::pair<int, float> > additional_EMs_vec;

      additional_EMs_vec.reserve(additional_EMs.size());
      for(auto &x : additional_EMs)
      {
        additional_EMs_vec.emplace_back(x.first, x.second);
      }

      std::sort(additional_EMs_vec.begin(), additional_EMs_vec.end(), sort_by_pair_second_lowest);

      if(Verbosity() > 5)
      {
        std::cout << " -> Sorting the set of potential additional EMs " << std::endl;
      }

      // now add in additional EMs until there are none left or it is no longer the case that Sum pT > calo

      int n_EM_added = 0;
      while (additional_EMs_vec.size() != 0 && total_expected_E > total_EMHAD_E)
      {
        int new_EM = additional_EMs_vec.at(0).first;

        if(Verbosity() > 5)
        {
          std::cout << " -> adding EM " << new_EM << " ( dR = " << additional_EMs_vec.at(0).second << " to the system (should not see it as orphan below)" << std::endl;
        }

        // for now, just make the first HAD-linked track point to this new EM, and vice versa
        _pflow_EM_match_TRK.at(new_EM).push_back(_pflow_HAD_match_TRK.at(had).at(0));
        _pflow_TRK_match_EM.at(_pflow_HAD_match_TRK.at(had).at(0)).push_back(new_EM);

        // add to expected calo
        total_EMHAD_E += _pflow_EM_E.at(new_EM);

        // erase lowest-dR EM
        additional_EMs_vec.erase(additional_EMs_vec.begin());

        //additional_EMs_IDs.push_back(new_EM);

        n_EM_added++;
      }

      if(Verbosity() > 5)
      {
        if(n_EM_added > 0)
        {
          std::cout << "After adding N = " << n_EM_added << " any additional EMs : " << std::endl;
          std::cout << "-> Total track Sum p = " << total_TRK_p << " , expected calo Sum E = " << total_expected_E << " +/- " << total_expected_E_err << " , observed EM+HAD Sum E = " << total_EMHAD_E << std::endl;
        }
        else
        {
          std::cout << "No additional EMs found, continuing hypothesis check" << std::endl;
        }
      }
    }
*/
    if(total_expected_E + _energy_match_Nsigma * total_expected_E_err > total_EMHAD_E)
    {
      if(Verbosity() > 5)
      {
        std::cout << " -> -> calo compatible within Nsigma = " << _energy_match_Nsigma << " , remove and keep tracks " << std::endl;
      }

      // PFlow elements already created from tracks above, no more needs to be done
    }
    else
    {
      float residual_energy = total_EMHAD_E - total_expected_E;

      if(Verbosity() > 5)
      {
        std::cout << " -> -> calo not compatible, create leftover cluster with " << residual_energy << std::endl;
      }

      // create additional PFlow element (tracks already created above)
      ParticleFlowElement *pflow = new ParticleFlowElementv1();

      // assume no mass, but could update to use K0L mass(?)
      CLHEP::Hep3Vector pflow_eta_phi(_pflow_HAD_x[had], _pflow_HAD_y[had], _pflow_HAD_z[had]);

      CLHEP::Hep3Vector pflow_vector;
      pflow_vector.setREtaPhi(residual_energy, pflow_eta_phi.eta(), pflow_eta_phi.phi());

      pflow->set_px(pflow_vector.getX());
      pflow->set_py(pflow_vector.getY());
      pflow->set_pz(pflow_vector.getZ());
      pflow->set_e(residual_energy);
      pflow->set_track(nullptr);
      pflow->set_eclusters(matchedEClusters_this_had_via_trk);
      pflow->set_hcluster(_pflow_HAD_cluster.at(had));
      pflow->set_assoc_tracks(assoc_tracks);
      pflow->set_id(_global_pflow_index);
      pflow->set_type(ParticleFlowElement::PFLOWTYPE::LEFTOVER_EM_PARTICLE);
/*
      std::cout << "Creating particle flow element" << std::endl;
      std::cout << "PF ID: " << _global_pflow_index << " as LEFTOVER_EM_PARTICLE" << std::endl;
      std::cout << "Track ID: " << "XXX" << std::endl;
      for(unsigned int iemc = 0; iemc < matchedEClustersIDs.size(); iemc++)
      {
        std::cout << "EMCal ID: " << matchedEClustersIDs.at(iemc) << std::endl;
      }
      for(unsigned int iemc = 0; iemc < additional_EMs_IDs.size(); iemc++)
      {
        std::cout << "Additional EMCal ID: " << additional_EMs_IDs.at(iemc) << std::endl;
      }
      std::cout << "HCal ID: " << had << std::endl;
*/
      _pflowContainer->AddParticleFlowElement(_global_pflow_index, pflow);
      _global_pflow_index++;
    }

  }  // close HAD loop
}

void ParticleFlowReco::ProcessMatchedEMCalClusters()
{
  for(unsigned int em = 0; em < _pflow_EM_E.size(); em++)
  {
    // only consider EM with matched tracks, but no matched HADs
    if(_pflow_EM_match_HAD.at(em).size() != 0)
    {
      continue;
    }
    if(_pflow_EM_match_TRK.at(em).size() == 0)
    {
      continue;
    }

    if(Verbosity() > 5)
    {
      std::cout << " EM " << em << " with E / x / y / z = " << _pflow_EM_E.at(em) << " / " << _pflow_EM_x.at(em) << " / " << _pflow_EM_y.at(em) << " / " << _pflow_EM_z.at(em) << std::endl;
    }

    // setup for Sum-pT^trk -> calo prediction
    float total_TRK_p = 0;
    float total_expected_E = 0;
    float total_expected_E_var = 0;

    // begin with this EM calo energy
    float total_EM_E = _pflow_EM_E.at(em);

    //In case of EM leftover
    std::vector<SvtxTrack*> assoc_tracks;

    // iterate over the TRKs matched to this EM
    for(unsigned int j = 0; j < _pflow_EM_match_TRK.at(em).size(); j++)
    {
      int trk = _pflow_EM_match_TRK.at(em).at(j);

      //Exclude tracks that were already matched to HADs
      if(_pflow_TRK_match_HAD.at(trk).size() != 0)
      {
        continue;
      }

      if(Verbosity() > 5)
      {
        std::cout << " -> -> LINKED TRK with p / eta / phi = " << _pflow_TRK_p.at(trk) << " / " << _pflow_TRK_eta.at(trk) << " / " << _pflow_TRK_phi.at(trk) << std::endl;
      }

      total_TRK_p += _pflow_TRK_p.at(trk);

      std::pair<float, float> expected_signature = get_expected_signature(trk);

      float expected_E_mean = expected_signature.first;
      float expected_E_sigma = expected_signature.second;

      if(Verbosity() > 5)
      {
        std::cout << " -> -> -> expected calo signature is " << expected_E_mean << " +/- " << expected_E_sigma << std::endl;
      }

      total_expected_E += expected_E_mean;
      total_expected_E_var += pow(expected_E_sigma, 2);

      // add PFlow element for each track
      ParticleFlowElement *pflow = new ParticleFlowElementv1();

      // assume pion mass
      float pfE = std::sqrt((_pflow_TRK_p[trk]*_pflow_TRK_p[trk]) + (0.135*0.135));

      CLHEP::Hep3Vector pflow_vector;
      pflow_vector.setREtaPhi(_pflow_TRK_p[trk], _pflow_TRK_eta[trk], _pflow_TRK_phi[trk]);

      std::vector<RawCluster *> eclus;
      eclus.push_back(_pflow_EM_cluster.at(em));

      pflow->set_px(pflow_vector.getX());
      pflow->set_py(pflow_vector.getY());
      pflow->set_pz(pflow_vector.getZ());
      pflow->set_e(pfE);
      pflow->set_track(_pflow_TRK_trk.at(trk));
      pflow->set_eclusters(eclus);
      pflow->set_hcluster(nullptr);
      pflow->set_id(_global_pflow_index);
      pflow->set_type(ParticleFlowElement::PFLOWTYPE::MATCHED_CHARGED_HADRON);
/*
      std::cout << "Creating particle flow element" << std::endl;
      std::cout << "PF ID: " << _global_pflow_index << " as MATCHED_CHARGED_HADRON (EMCal+Track)" << std::endl;
      std::cout << "Track ID: " << trk << std::endl;
      std::cout << "EMCal ID: " << em << std::endl;
      std::cout << "HCal ID: " << "XXX" << std::endl;
*/
      _pflowContainer->AddParticleFlowElement(_global_pflow_index, pflow);
      _global_pflow_index++;
      assoc_tracks.push_back(_pflow_TRK_trk.at(trk));
    }

    // process compatibility of fit
    float total_expected_E_err = sqrt(total_expected_E_var);

    if(Verbosity() > 5)
    {
      std::cout << " -> Total track Sum p = " << total_TRK_p << " , expected calo Sum E = " << total_expected_E << " +/- " << total_expected_E_err << " , observed EM Sum E = " << total_EM_E << std::endl;
    }

    if(total_expected_E + _energy_match_Nsigma * total_expected_E_err > total_EM_E)
    {
      if(Verbosity() > 5)
      {
        std::cout << " -> -> calo compatible within Nsigma = " << _energy_match_Nsigma << "  , remove and keep tracks " << std::endl;
      }

      // PFlow elements already created from tracks above, no more needs to be done
    }
    else
    {
      float residual_energy = total_EM_E - total_expected_E;

      if(Verbosity() > 5)
      {
        std::cout << " -> -> calo not compatible, create leftover cluster with " << residual_energy << std::endl;
      }

      // create additional PFlow element (tracks already created above)
      ParticleFlowElement *pflow = new ParticleFlowElementv1();

      // assume no mass, but could update to use K0L mass(?)
      CLHEP::Hep3Vector pflow_eta_phi(_pflow_EM_x[em], _pflow_EM_y[em], _pflow_EM_z[em]);

      CLHEP::Hep3Vector pflow_vector;
      pflow_vector.setREtaPhi(residual_energy, pflow_eta_phi.eta(), pflow_eta_phi.phi());

      std::vector<RawCluster *> eclus;
      eclus.push_back(_pflow_EM_cluster.at(em));

      pflow->set_px(pflow_vector.getX());
      pflow->set_py(pflow_vector.getY());
      pflow->set_pz(pflow_vector.getZ());
      pflow->set_e(residual_energy);
      pflow->set_eclusters(eclus);
      pflow->set_hcluster(nullptr);
      pflow->set_track(nullptr);
      pflow->set_assoc_tracks(assoc_tracks);
      pflow->set_id(_global_pflow_index);
      pflow->set_type(ParticleFlowElement::PFLOWTYPE::LEFTOVER_EM_PARTICLE);
/*
      std::cout << "Creating particle flow element" << std::endl;
      std::cout << "PF ID: " << _global_pflow_index << " as LEFTOVER_EM_PARTICLE" << std::endl;
      std::cout << "Track ID: " << "XXX" << std::endl;
      std::cout << "EMCal ID: " << em << std::endl;
      std::cout << "HCal ID: " << "XXX" << std::endl;
*/
      _pflowContainer->AddParticleFlowElement(_global_pflow_index, pflow);
      _global_pflow_index++;
    }

  }  // close EM loop
}

void ParticleFlowReco::ProcessClusterOnlyMatch()
{
  CLHEP::Hep3Vector vertex = GetVertexForCalorimeter();


  for(unsigned int had = 0; had < _pflow_HAD_E.size(); had++)
  {
    // only consider HADs withOUT matched tracks ... we have dealt with the matched cases above
    if(_pflow_HAD_match_TRK.at(had).size() != 0)
    {
      continue;
    }

    //Don't look at clusters that were not matched with larger DeltaR
    if(_pflow_HAD_match_EM.at(had).size() == 0)
    {
      continue;
    }

    CLHEP::Hep3Vector hcal_cluster(_pflow_HAD_x[had], _pflow_HAD_y[had], _pflow_HAD_z[had]);
    hcal_cluster = hcal_cluster - vertex;

    //float matched_dR = 0.;
    int matched_index = -1;
    float min_dR = 999.;

    for(unsigned int em = 0; em < _pflow_EM_E.size(); em++)
    {
      // only consider EMs withOUT matched tracks ... we have dealt with the matched cases above
      if(_pflow_EM_match_TRK.at(em).size() != 0)
      {
        continue;
      }

      //Don't look at clusters that were not matched with larger DeltaR
      if(_pflow_EM_match_HAD.at(em).size() == 0)
      {
        continue;
      }

      CLHEP::Hep3Vector emcal_cluster(_pflow_EM_x[em], _pflow_EM_y[em], _pflow_EM_z[em]);
      emcal_cluster = emcal_cluster - vertex;

      float dR = fabs(emcal_cluster.deltaR(hcal_cluster));

      if(dR > 0.2)
      {
        continue;
      }

      if(dR < min_dR)
      {
        min_dR = dR;
        matched_index = em;
        //matched_dR = emcal_cluster.deltaR(hcal_cluster);
      }

    }

    if(matched_index < 0) //Previous HAD-EM match had also track matching or no match found
    {
      continue;
    }

    // add PFlow element for this EM+HAD
    ParticleFlowElement *pflow = new ParticleFlowElementv1();

    // assume massless, could be updated to use K0L
    // assuming EM kinematics due to higher granularity of the EMCal, this has to be further studied!
    CLHEP::Hep3Vector pflow_eta_phi(_pflow_EM_x[matched_index], _pflow_EM_y[matched_index], _pflow_EM_z[matched_index]);
    pflow_eta_phi = pflow_eta_phi - vertex;

    CLHEP::Hep3Vector pflow_vector;
    pflow_vector.setREtaPhi(_pflow_EM_E[matched_index]+_pflow_HAD_E[had], pflow_eta_phi.eta(), pflow_eta_phi.phi());

    std::vector<RawCluster *> eclus;
    eclus.push_back(_pflow_EM_cluster.at(matched_index));

    pflow->set_px(pflow_vector.getX());
    pflow->set_py(pflow_vector.getY());
    pflow->set_pz(pflow_vector.getZ());
    pflow->set_e(_pflow_EM_E[matched_index]);
    pflow->set_eclusters(eclus);
    pflow->set_hcluster(_pflow_HAD_cluster.at(had));
    pflow->set_track(nullptr);
    pflow->set_id(_global_pflow_index);
    pflow->set_type(ParticleFlowElement::PFLOWTYPE::UNMATCHED_NEUTRAL_HADRON);
/*
    std::cout << "Creating particle flow element" << std::endl;
    std::cout << "PF ID: " << _global_pflow_index << " as UNMATCHED_NEUTRAL_HADRON" << std::endl;
    std::cout << "Track ID: " << "XXX" << std::endl;
    std::cout << "EMCal ID: " << matched_index << std::endl;
    std::cout << "HCal ID: " << had << std::endl;
*/
    _pflowContainer->AddParticleFlowElement(_global_pflow_index, pflow);
    _global_pflow_index++;
  }

  /*

        if(fabs(emcal_cluster.deltaR(hcal_cluster)) > 0.5)
        {
          std::cout << "No match" << std::endl;
          continue;
        }

        std::cout << "EMCal matched to HCal with dPhi: " << emcal_cluster.deltaPhi(hcal_cluster) << " dEta: " << emcal_cluster.eta()-hcal_cluster.eta() << std::endl;
        std::cout << "EMCal ID: " << em << " phi: " << emcal_cluster.phi() << " eta: " << emcal_cluster.eta() << std::endl;
        std::cout << "HCal ID: " << had << " phi: " << hcal_cluster.phi() << " eta: " << hcal_cluster.eta() << std::endl;
        for(unsigned int im = 0; im < _pflow_EM_match_HAD.at(em).size(); im++)
        {
          std::cout << "Previously Linked HAD ID = " << _pflow_EM_match_HAD.at(em).at(im) << std::endl;
        }
        */
}

void ParticleFlowReco::ProcessUnmatchedEMCalClusters()
{
  for(unsigned int em = 0; em < _pflow_EM_E.size(); em++)
  {
    // only consider EMs withOUT matched tracks ... we have dealt with the matched cases above
    if(_pflow_EM_match_TRK.at(em).size() != 0)
    {
      continue;
    }

    // only consider EMs withOUT matched HADs
    if(_pflow_EM_match_HAD.at(em).size() != 0)
    {
      continue;
    }

    if(Verbosity() > 5)
    {
      std::cout << " unmatched EM " << em << " with E / x / y / z = " << _pflow_EM_E.at(em) << " / " << _pflow_EM_x.at(em) << " / " << _pflow_EM_y.at(em) << " / " << _pflow_EM_z.at(em) << std::endl;
    }

    // add PFlow element for this EM
    ParticleFlowElement *pflow = new ParticleFlowElementv1();

    // assume massless, could be updated to use K0L
    CLHEP::Hep3Vector vertex = GetVertexForCalorimeter();
    CLHEP::Hep3Vector pflow_eta_phi(_pflow_EM_x[em], _pflow_EM_y[em], _pflow_EM_z[em]);
    pflow_eta_phi = pflow_eta_phi - vertex;

    CLHEP::Hep3Vector pflow_vector;
    pflow_vector.setREtaPhi(_pflow_EM_E[em], pflow_eta_phi.eta(), pflow_eta_phi.phi());

    std::vector<RawCluster *> eclus;
    eclus.push_back(_pflow_EM_cluster.at(em));

    pflow->set_px(pflow_vector.getX());
    pflow->set_py(pflow_vector.getY());
    pflow->set_pz(pflow_vector.getZ());
    pflow->set_e(_pflow_EM_E[em]);
    pflow->set_eclusters(eclus);
    pflow->set_hcluster(nullptr);
    pflow->set_track(nullptr);
    pflow->set_id(_global_pflow_index);
    pflow->set_type(ParticleFlowElement::PFLOWTYPE::UNMATCHED_EM_PARTICLE);
/*
    std::cout << "Creating particle flow element" << std::endl;
    std::cout << "PF ID: " << _global_pflow_index << " as UNMATCHED_EM_PARTICLE" << std::endl;
    std::cout << "Track ID: " << "XXX" << std::endl;
    std::cout << "EMCal ID: " << em << std::endl;
    std::cout << "HCal ID: " << "XXX" << std::endl;
*/
    _pflowContainer->AddParticleFlowElement(_global_pflow_index, pflow);
    _global_pflow_index++;

  }  // close EM loop
}

void ParticleFlowReco::ProcessUnmatchedHCalClusters()
{
  for(unsigned int had = 0; had < _pflow_HAD_E.size(); had++)
  {
    // only consider HADs withOUT matched tracks ... we have dealt with the matched cases above
    if(_pflow_HAD_match_TRK.at(had).size() != 0)
    {
      continue;
    }

    // only consider HADs withOUT matched EMs
    if(_pflow_HAD_match_EM.at(had).size() != 0)
    {
      continue;
    }

    if(Verbosity() > 5)
    {
      std::cout << " unmatched HAD " << had << " with E / x / y / z = " << _pflow_HAD_E.at(had) << " / " << _pflow_HAD_x.at(had) << " / " << _pflow_HAD_y.at(had) << " / " << _pflow_HAD_z.at(had) << std::endl;
    }

    // add PFlow element for this HAD
    ParticleFlowElement *pflow = new ParticleFlowElementv1();

    // assume massless, could be updated to use K0L
    CLHEP::Hep3Vector vertex = GetVertexForCalorimeter();
    CLHEP::Hep3Vector pflow_eta_phi(_pflow_HAD_x[had], _pflow_HAD_y[had], _pflow_HAD_z[had]);
    pflow_eta_phi = pflow_eta_phi - vertex;

    CLHEP::Hep3Vector pflow_vector;
    pflow_vector.setREtaPhi(_pflow_HAD_E[had], pflow_eta_phi.eta(), pflow_eta_phi.phi());

    pflow->set_px(pflow_vector.getX());
    pflow->set_py(pflow_vector.getY());
    pflow->set_pz(pflow_vector.getZ());
    pflow->set_e(_pflow_HAD_E[had]);
    pflow->set_track(nullptr);
    pflow->set_eclusters(std::vector<RawCluster *>());
    pflow->set_hcluster(_pflow_HAD_cluster.at(had));
    pflow->set_id(_global_pflow_index);
    pflow->set_type(ParticleFlowElement::PFLOWTYPE::UNMATCHED_NEUTRAL_HADRON);
/*
    std::cout << "Creating particle flow element" << std::endl;
    std::cout << "PF ID: " << _global_pflow_index << " as UNMATCHED_NEUTRAL_HADRON" << std::endl;
    std::cout << "Track ID: " << "XXX" << std::endl;
    std::cout << "EMCal ID: " << "XXX" << std::endl;
    std::cout << "HCal ID: " << had << std::endl;
*/
    _pflowContainer->AddParticleFlowElement(_global_pflow_index, pflow);
    _global_pflow_index++;

  }  // close HAD loop
}

void ParticleFlowReco::ProcessUnmatchedTracks()
{
  for(unsigned int trk = 0; trk < _pflow_TRK_p.size(); trk++)
  {
    // only consider TRKs withOUT matched EM or HAD
    if(_pflow_TRK_match_EM.at(trk).size() != 0 || _pflow_TRK_match_HAD.at(trk).size() != 0)
    {
      continue;
    }

    if(Verbosity() > 5)
    {
      std::cout << " unmatched TRK " << trk << " with p / eta / phi = " << _pflow_TRK_p.at(trk) << " / " << _pflow_TRK_eta.at(trk) << " / " << _pflow_TRK_phi.at(trk) << std::endl;
    }

    // add PFlow element for this TRK
    ParticleFlowElement *pflow = new ParticleFlowElementv1();

    // assume massless, could be updated to use K0L
    float pfE = std::sqrt((_pflow_TRK_p[trk]*_pflow_TRK_p[trk]) + (0.135*0.135));

    CLHEP::Hep3Vector pflow_vector;
    pflow_vector.setREtaPhi(_pflow_TRK_p[trk], _pflow_TRK_eta[trk], _pflow_TRK_phi[trk]);

    pflow->set_px(pflow_vector.getX());
    pflow->set_py(pflow_vector.getY());
    pflow->set_pz(pflow_vector.getZ());
    pflow->set_e(pfE);
    pflow->set_track(_pflow_TRK_trk.at(trk));
    pflow->set_eclusters(std::vector<RawCluster *>());
    pflow->set_hcluster(nullptr);
    pflow->set_id(_global_pflow_index);
    pflow->set_type(ParticleFlowElement::PFLOWTYPE::UNMATCHED_CHARGED_HADRON);
/*
    std::cout << "Creating particle flow element" << std::endl;
    std::cout << "PF ID: " << _global_pflow_index << " as UNMATCHED_CHARGED_HADRON" << std::endl;
    std::cout << "Track ID: " << trk << std::endl;
    std::cout << "EMCal ID: " << "XXX" << std::endl;
    std::cout << "HCal ID: " << "XXX" << std::endl;
*/
    _pflowContainer->AddParticleFlowElement(_global_pflow_index, pflow);
    _global_pflow_index++;

  }  // close TRK loop
}

CLHEP::Hep3Vector ParticleFlowReco::GetVertexForCalorimeter()
{
  CLHEP::Hep3Vector calo_vertex(0., 0., 0.);

  if(_dont_use_global_vertex)
  {
    size_t max_ntracks_vertex_id = 0;

    for(unsigned int ivertex = 0; ivertex < _svtx_vertexmap->size(); ivertex++)
    {
      SvtxVertex *vertex = _svtx_vertexmap->get(ivertex);
      if(vertex->get_beam_crossing() != 0)
      {
        continue;
      }

      if(vertex->size_tracks() > max_ntracks_vertex_id)
      {
        max_ntracks_vertex_id = vertex->size_tracks();
        calo_vertex.setX(vertex->get_x());
        calo_vertex.setY(vertex->get_y());
        calo_vertex.setZ(vertex->get_z());
      }
    }
  }
  else
  {
    int max_ntracks_vertex_id = 0;

    for(auto &iter : *_global_vertexmap)
    {
      GlobalVertex *global_vtx = iter.second;

      if(global_vtx->get_beam_crossing() != 0)
      {
        continue;
      }

      for(GlobalVertex::VertexIter v_iter = global_vtx->begin_vertexes(); v_iter != global_vtx->end_vertexes(); ++v_iter)
      {
        auto vertex = *v_iter;

        //In case there is only MBD vertex, use it
        if(vertex.first == GlobalVertex::VTXTYPE::MBD)
        {
          calo_vertex.setX(global_vtx->get_x());
          calo_vertex.setY(global_vtx->get_y());
          calo_vertex.setZ(global_vtx->get_z());
        }

        for(auto v : vertex.second)
        {
          int n_tracks = 0;
          for(Vertex::TrackIter trk_iter = v->begin_tracks(); trk_iter != v->end_tracks(); ++trk_iter)
          {
            n_tracks++;
          }
          if(n_tracks > max_ntracks_vertex_id)
          {
            max_ntracks_vertex_id = n_tracks;
            calo_vertex.setX(global_vtx->get_x());
            calo_vertex.setY(global_vtx->get_y());
            calo_vertex.setZ(global_vtx->get_z());
          }
        }

      }

    }

  }

  return calo_vertex;
}

std::pair<int, int> ParticleFlowReco::GetCaloLimits(CLHEP::Hep3Vector &cluster, int trackID)
{
  int inner_layer = -1;
  int outer_layer = -1;

  for(unsigned int icalo = 0; icalo < _pflow_TRK_projections.at(trackID).size(); icalo++)
  {
    if(std::isnan(_pflow_TRK_projections.at(trackID).at(icalo).x()))
    {
      break;
    }

    if(_pflow_TRK_projections.at(trackID).at(icalo).perp() < cluster.perp())
    {
      inner_layer = icalo;
    }
    else
    {
      outer_layer = icalo;
      break;
    }
  }

  return std::make_pair(inner_layer, outer_layer);
}

CLHEP::Hep3Vector ParticleFlowReco::Minimize_PhiEta(CLHEP::Hep3Vector &cluster, int trackID, CLHEP::Hep3Vector &vertex, bool isHCal)
{
  CLHEP::Hep3Vector projection(NAN, NAN, NAN);

  std::pair<int, int> calo_limits = std::make_pair(0, 1);

  if(isHCal)
  {
    calo_limits = GetCaloLimits(cluster, trackID);
    if(calo_limits.first < 2)
    {
      return projection;
    }
  }

  if((calo_limits.first >= 0) && (calo_limits.second >= 0)) //has both states
  {
    TVectorD p1(2), p2(2), q(2);

    p1[0] = (_pflow_TRK_projections.at(trackID).at(calo_limits.first) - vertex).phi();
    p1[1] = (_pflow_TRK_projections.at(trackID).at(calo_limits.first) - vertex).eta();

    p2[0] = (_pflow_TRK_projections.at(trackID).at(calo_limits.second) - vertex).phi();
    p2[1] = (_pflow_TRK_projections.at(trackID).at(calo_limits.second) - vertex).eta();

    q[0] = cluster.phi();
    q[1] = cluster.eta();

    TVectorD d = p2; d -= p1;        // direction vector (p2 - p1)
    TVectorD r = p1; r -= q;         // difference (p1 - q)

    double a = d * d;                // dot(d, d)
    double b = 2.0 * (d * r);        // 2 * dot(d, p1 - q)

    double lambda = -b / (2.0 * a);  // unconstrained min
    lambda = std::max(0.0, std::min(1.0, lambda)); // clamp to [0, 1]

    projection = (1.-lambda)*_pflow_TRK_projections.at(trackID).at(calo_limits.first) + lambda*_pflow_TRK_projections.at(trackID).at(calo_limits.second);
  }
  else if((calo_limits.first >= 0) && (calo_limits.second < 0))
  {
    projection = _pflow_TRK_projections.at(trackID).at(calo_limits.first);
  }

  return projection;
}

bool ParticleFlowReco::isAcceptableTrack(SvtxTrack *track)
{
  if(_only_crossing_zero && (track->get_crossing() != 0))
  {
    return false;
  }

  if(track->get_pt() < _min_track_pt)
  {
    return false;
  }

  if(fabs(track->get_eta()) > _acc_track_eta)
  {
    return false;
  }

  return true;
}

int ParticleFlowReco::CreateNode(PHCompositeNode *topNode)
{
  PHNodeIterator iter(topNode);

  // Looking for the DST node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if(!dstNode)
  {
    std::cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // store the PFlow elements under a sub-node directory
  PHCompositeNode *pflowNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "PARTICLEFLOW"));
  if(!pflowNode)
  {
    pflowNode = new PHCompositeNode("PARTICLEFLOW");
    dstNode->addNode(pflowNode);
  }

  // create the ParticleFlowElementContainer node...
  ParticleFlowElementContainer *pflowElementContainer = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");
  if(!pflowElementContainer)
  {
    pflowElementContainer = new ParticleFlowElementContainer();
    PHIODataNode<PHObject> *pflowElementNode = new PHIODataNode<PHObject>(pflowElementContainer, "ParticleFlowElements", "PHObject");
    pflowNode->addNode(pflowElementNode);
  }
  else
  {
    std::cout << PHWHERE << "::ERROR - ParticleFlowElements node alerady exists, but should not" << std::endl;
    exit(-1);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

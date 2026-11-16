#include "ParticleFlowEval.h"
#include "CaloRawClusterEval.h"
#include "CaloTruthEval.h"
#include "SvtxTrackEval.h"

#include <calobase/RawClusterv1.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

#include <g4main/PHG4Particlev2.h>
#include <g4main/PHG4Shower.h>

#include <particleflowreco/ParticleFlowElement.h>

#include <phool/getClass.h>

#include <tr1/functional>

#include <cassert>
#include <cfloat>
#include <climits>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>

class RawTower;

ParticleFlowEval::ParticleFlowEval(PHCompositeNode* topNode)
  : _clustereval_emcal(topNode, "CEMC")
  , _clustereval_ihcal(topNode, "HCALIN")
  , _clustereval_ohcal(topNode, "HCALOUT")
  , _trackeval(topNode)
  , _track(nullptr)
{
  _topo_ihcal = new RawClusterv1();
  _topo_ohcal = new RawClusterv1();
  _topo_emcal.clear();
  /*
  _clustereval_emcal.set_usetowerinfo(false);
  _clustereval_ihcal.set_usetowerinfo(false);
  _clustereval_ohcal.set_usetowerinfo(false);
  */
}

ParticleFlowEval::~ParticleFlowEval()
{
  delete _topo_ihcal;
  delete _topo_ohcal;
}

void ParticleFlowEval::next_event(PHCompositeNode* topNode)
{
  _clustereval_emcal.next_event(topNode);
  _clustereval_ihcal.next_event(topNode);
  _clustereval_ohcal.next_event(topNode);
  _trackeval.next_event(topNode);
}

void ParticleFlowEval::SeparateHCals(RawCluster *hcluster)
{
  RawCluster::TowerConstRange begin_end = hcluster->get_towers();
  for(RawCluster::TowerConstIterator towerIt = begin_end.first; towerIt != begin_end.second; ++towerIt)
  {
    if(RawTowerDefs::decode_caloid(towerIt->first) ==  RawTowerDefs::CalorimeterId::HCALOUT)
    {
      _topo_ohcal->addTower(towerIt->first, towerIt->second);
    }
    else
    {
      _topo_ihcal->addTower(towerIt->first, towerIt->second);
    }
  }
}

PHG4Particle* ParticleFlowEval::get_truth_particle_flow(ParticleFlowElement* pflow)
{
  _pflow = pflow;

  PHG4Particle *g4particle = nullptr;

  _track = _pflow->get_track();
  _topo_emcal = _pflow->get_eclusters();

  RawCluster *hcal_cluster = _pflow->get_hcluster();
  if(hcal_cluster)
  {
    SeparateHCals(hcal_cluster);
  }

  if(_pflow->get_type() == ParticleFlowElement::PFLOWTYPE::UNMATCHED_CHARGED_HADRON)
  {
    g4particle = _trackeval.max_truth_particle_by_nclusters(_track);
  }


  if(_pflow->get_type() == ParticleFlowElement::PFLOWTYPE::MATCHED_CHARGED_HADRON)
  {
    g4particle = _trackeval.max_truth_particle_by_nclusters(_track);
  }

  if(_pflow->get_type() == ParticleFlowElement::PFLOWTYPE::UNMATCHED_EM_PARTICLE)
  {
    g4particle = max_truth_primary_particle_by_energy_emcal();
  }

  if(_pflow->get_type() == ParticleFlowElement::PFLOWTYPE::UNMATCHED_NEUTRAL_HADRON)
  {
    g4particle = max_truth_primary_particle_by_energy_hcal();
  }

  if(_pflow->get_type() == ParticleFlowElement::PFLOWTYPE::LEFTOVER_EM_PARTICLE)
  {
    std::set<PHG4Particle*> primaries = all_truth_primary_particles_emcal();

    primaries.merge(all_truth_primary_particles_hcal());

    std::vector<SvtxTrack*> assoc_tracks = _pflow->get_assoc_tracks();

    for(std::vector<SvtxTrack*>::iterator itr = assoc_tracks.begin(); itr != assoc_tracks.end(); itr++)
    {
      PHG4Particle *primary_track = _trackeval.max_truth_particle_by_nclusters((*itr));
      primaries.erase(primary_track);
    }

    std::map<int, std::pair<float, PHG4Particle*>> primary_energy;

    for(std::set<PHG4Particle*>::iterator itr = primaries.begin(); itr != primaries.end(); itr++)
    {
      if(primary_energy.find((*itr)->get_track_id()) != primary_energy.end())
      {
        for(unsigned int i = 0; i < _topo_emcal.size(); i++)
        {
          primary_energy[(*itr)->get_track_id()].first += _clustereval_emcal.get_energy_contribution(_topo_emcal.at(i), (*itr));
        }
        primary_energy[(*itr)->get_track_id()].first += _clustereval_ihcal.get_energy_contribution(_topo_ihcal, (*itr));
        primary_energy[(*itr)->get_track_id()].first += _clustereval_ohcal.get_energy_contribution(_topo_ohcal, (*itr));
      }
      else
      {
        primary_energy[(*itr)->get_track_id()].first = 0.;
        for(unsigned int i = 0; i < _topo_emcal.size(); i++)
        {
          primary_energy[(*itr)->get_track_id()].first += _clustereval_emcal.get_energy_contribution(_topo_emcal.at(i), (*itr));
        }
        primary_energy[(*itr)->get_track_id()].first += _clustereval_ihcal.get_energy_contribution(_topo_ihcal, (*itr));
        primary_energy[(*itr)->get_track_id()].first += _clustereval_ohcal.get_energy_contribution(_topo_ohcal, (*itr));
        primary_energy[(*itr)->get_track_id()].second = (*itr);
      }
    }

    int max_index = -1;
    float max_energy = 0.;

    for(std::map<int, std::pair<float, PHG4Particle*>>::iterator itr = primary_energy.begin(); itr != primary_energy.end(); ++itr)
    {
      if(itr->second.first > max_energy)
      {
        max_energy = itr->second.first;
        max_index = itr->first;
      }
    }

    g4particle = primary_energy[max_index].second;
  }

  return g4particle;

}

std::set<PHG4Shower*> ParticleFlowEval::all_truth_primary_showers_emcal()
{
  std::set<PHG4Shower*> all_showers;
  for(unsigned int i = 0; i < _topo_emcal.size(); i++)
  {
    std::set<PHG4Shower*> showers = _clustereval_emcal.all_truth_primary_showers(_topo_emcal.at(i));

    all_showers.merge(showers);
  }
  return all_showers;
}

std::set<PHG4Shower*> ParticleFlowEval::all_truth_primary_showers_emcal(RawCluster *cluster)
{
  if(std::find(_topo_emcal.begin(), _topo_emcal.end(), cluster) != _topo_emcal.end())
  {
    return _clustereval_emcal.all_truth_primary_showers(cluster);
  }
  else
  {
    return std::set<PHG4Shower*>();
  }

}

std::set<PHG4Shower*> ParticleFlowEval::all_truth_primary_showers_hcal()
{
  std::set<PHG4Shower*> all_showers = all_truth_primary_showers_ihcal();

  all_showers.merge(all_truth_primary_showers_ohcal());

  return all_showers;
}

std::set<PHG4Shower*> ParticleFlowEval::all_truth_primary_showers_ihcal()
{
  return _clustereval_ihcal.all_truth_primary_showers(_topo_ihcal);
}

std::set<PHG4Shower*> ParticleFlowEval::all_truth_primary_showers_ohcal()
{
  return _clustereval_ohcal.all_truth_primary_showers(_topo_ohcal);
}

PHG4Shower* ParticleFlowEval::max_truth_primary_shower_by_energy_emcal()
{
  if(_topo_emcal.size() == 0) return nullptr;

  std::map<int, std::pair<float, PHG4Shower*>> shower_energy;

  for(unsigned int i = 0; i < _topo_emcal.size(); i++)
  {
    std::set<PHG4Shower*> showers = _clustereval_emcal.all_truth_primary_showers(_topo_emcal.at(i));

    for(std::set<PHG4Shower*>::iterator itr = showers.begin(); itr != showers.end(); itr++)
    {
      if(shower_energy.find((*itr)->get_id()) != shower_energy.end())
      {
        shower_energy[(*itr)->get_id()].first += _clustereval_emcal.get_energy_contribution(_topo_emcal.at(i), (*itr));
      }
      else
      {
        shower_energy[(*itr)->get_id()].first = _clustereval_emcal.get_energy_contribution(_topo_emcal.at(i), (*itr));
        shower_energy[(*itr)->get_id()].second = (*itr);
      }
    }
  }

  int max_index = -1;
  int max_energy = -1;

  for(std::map<int, std::pair<float, PHG4Shower*>>::iterator itr = shower_energy.begin(); itr != shower_energy.end(); ++itr)
  {
    if(itr->second.first > max_energy)
    {
      max_energy = itr->second.first;
      max_index = itr->first;
    }
  }

  return shower_energy[max_index].second;
}

PHG4Shower* ParticleFlowEval::max_truth_primary_shower_by_energy_emcal(RawCluster *cluster)
{
  return _clustereval_emcal.max_truth_primary_shower_by_energy(cluster);
}

PHG4Shower* ParticleFlowEval::max_truth_primary_shower_by_energy_hcal()
{
  PHG4Shower *max_shower = nullptr;

  std::set<PHG4Shower*> showers = all_truth_primary_showers_hcal();

  float max_shower_energy = 0.;

  for(std::set<PHG4Shower*>::iterator itr = showers.begin(); itr != showers.end(); itr++)
  {
    float shower_energy = _clustereval_ihcal.get_energy_contribution(_topo_ihcal, (*itr));
    shower_energy += _clustereval_ohcal.get_energy_contribution(_topo_ohcal, (*itr));

    if(shower_energy > max_shower_energy)
    {
      max_shower_energy = shower_energy;
      max_shower = (*itr);
    }
  }

  return max_shower;
}

PHG4Shower* ParticleFlowEval::max_truth_primary_shower_by_energy_ihcal()
{
  return _clustereval_ihcal.max_truth_primary_shower_by_energy(_topo_ihcal);
}

PHG4Shower* ParticleFlowEval::max_truth_primary_shower_by_energy_ohcal()
{
  return _clustereval_ihcal.max_truth_primary_shower_by_energy(_topo_ohcal);
}

std::set<PHG4Particle*> ParticleFlowEval::all_truth_primary_particles_emcal()
{
  std::set<PHG4Particle*> all_primaries;

  for(unsigned int i = 0; i < _topo_emcal.size(); i++)
  {
    std::set<PHG4Particle*> primaries = all_truth_primary_particles_emcal(_topo_emcal.at(i));

    for(std::set<PHG4Particle*>::iterator itr = primaries.begin(); itr != primaries.end(); itr++)
    {
      if(all_primaries.find((*itr)) != all_primaries.end())
      {
        continue;
      }
      else
      {
        all_primaries.insert(*itr);
      }
    }
  }

  return all_primaries;
}

std::set<PHG4Particle*> ParticleFlowEval::all_truth_primary_particles_emcal(RawCluster *cluster)
{
  return _clustereval_emcal.all_truth_primary_particles(cluster);
}

std::set<PHG4Particle*> ParticleFlowEval::all_truth_primary_particles_hcal()
{
  std::set<PHG4Particle*> all_primaries = all_truth_primary_particles_ihcal();

  all_primaries.merge(all_truth_primary_particles_ohcal());

  return all_primaries;
}

std::set<PHG4Particle*> ParticleFlowEval::all_truth_primary_particles_ihcal()
{
  return _clustereval_ihcal.all_truth_primary_particles(_topo_ihcal);
}

std::set<PHG4Particle*> ParticleFlowEval::all_truth_primary_particles_ohcal()
{
  return _clustereval_ohcal.all_truth_primary_particles(_topo_ohcal);
}

PHG4Particle* ParticleFlowEval::max_truth_primary_particle_by_energy_emcal()
{
  if(_topo_emcal.size() == 0)
  {
    return nullptr;
  }

  std::map<int, std::pair<float, PHG4Particle*>> primary_energy;

  for(unsigned int i = 0; i < _topo_emcal.size(); i++)
  {
    std::set<PHG4Particle*> primaries = _clustereval_emcal.all_truth_primary_particles(_topo_emcal.at(i));

    for(std::set<PHG4Particle*>::iterator itr = primaries.begin(); itr != primaries.end(); itr++)
    {
      if(primary_energy.find((*itr)->get_track_id()) != primary_energy.end())
      {
        primary_energy[(*itr)->get_track_id()].first += _clustereval_emcal.get_energy_contribution(_topo_emcal.at(i), (*itr));
      }
      else
      {
        primary_energy[(*itr)->get_track_id()].first = _clustereval_emcal.get_energy_contribution(_topo_emcal.at(i), (*itr));
        primary_energy[(*itr)->get_track_id()].second = (*itr);
      }
    }
  }

  int max_index = -1;
  int max_energy = -1;

  for(std::map<int, std::pair<float, PHG4Particle*>>::iterator itr = primary_energy.begin(); itr != primary_energy.end(); ++itr)
  {
    if(itr->second.first > max_energy)
    {
      max_energy = itr->second.first;
      max_index = itr->first;
    }
  }

  return primary_energy[max_index].second;
}

PHG4Particle* ParticleFlowEval::max_truth_primary_particle_by_energy_emcal(RawCluster* cluster)
{
  return _clustereval_emcal.max_truth_primary_particle_by_energy(cluster);
}

PHG4Particle* ParticleFlowEval::max_truth_primary_particle_by_energy_hcal()
{
  PHG4Particle *max_primary = nullptr;

  std::set<PHG4Particle*> primaries = all_truth_primary_particles_hcal();

  float max_primary_energy = 0.;

  for(std::set<PHG4Particle*>::iterator itr = primaries.begin(); itr != primaries.end(); itr++)
  {
    float primary_energy = _clustereval_ihcal.get_energy_contribution(_topo_ihcal, (*itr));
    primary_energy += _clustereval_ohcal.get_energy_contribution(_topo_ohcal, (*itr));

    if(primary_energy > max_primary_energy)
    {
      max_primary_energy = primary_energy;
      max_primary = (*itr);
    }
  }

  return max_primary;
}

PHG4Particle* ParticleFlowEval::max_truth_primary_particle_by_energy_ihcal()
{
  return _clustereval_ihcal.max_truth_primary_particle_by_energy(_topo_ihcal);
}

PHG4Particle* ParticleFlowEval::max_truth_primary_particle_by_energy_ohcal()
{
  return _clustereval_ohcal.max_truth_primary_particle_by_energy(_topo_ohcal);
}

PHG4Particle* ParticleFlowEval::max_truth_particle_by_nclusters_track()
{
  return _trackeval.max_truth_particle_by_nclusters(_track);
}

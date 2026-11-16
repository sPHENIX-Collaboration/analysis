#include "TopoClusterEval.h"
#include "CaloRawClusterEval.h"

#include <calobase/RawClusterv1.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTowerContainer.h>

#include <g4main/PHG4Particlev2.h>
#include <g4main/PHG4Shower.h>

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

TopoClusterEval::TopoClusterEval(PHCompositeNode* topNode, const std::string &type)
  : _topocluster_type(type)
  , _clustereval_emcal(topNode, "CEMC")
  , _clustereval_ihcal(topNode, "HCALIN")
  , _clustereval_ohcal(topNode, "HCALOUT")
{
  _clustereval_emcal.set_is_topocluster(true);
  _clustereval_emcal.set_topocluster_type(_topocluster_type);
  _clustereval_ihcal.set_is_topocluster(true);
  _clustereval_ihcal.set_topocluster_type(_topocluster_type);
  _clustereval_ohcal.set_is_topocluster(true);
  _clustereval_ohcal.set_topocluster_type(_topocluster_type);

  _topo_ihcal = new RawClusterv1();
  _topo_ohcal = new RawClusterv1();
  _topo_emcal = new RawClusterv1();
}

TopoClusterEval::~TopoClusterEval()
{
  delete _topo_ihcal;
  delete _topo_ohcal;
  delete _topo_emcal;
}

void TopoClusterEval::next_event(PHCompositeNode* topNode)
{
  _clustereval_emcal.next_event(topNode);
  _clustereval_ihcal.next_event(topNode);
  _clustereval_ohcal.next_event(topNode);

  if(_topo_ihcal)
  {
    delete _topo_ihcal;
    _topo_ihcal = new RawClusterv1();
  }

  if(_topo_ohcal)
  {
    delete _topo_ohcal;
    _topo_ohcal = new RawClusterv1();
  }

  if(_topo_emcal)
  {
    delete _topo_emcal;
    _topo_emcal = new RawClusterv1();
  }
}

void TopoClusterEval::SeparateSubsystems(RawCluster *topo_cluster)
{
  if(_topo_ihcal)
  {
    delete _topo_ihcal;
    _topo_ihcal = new RawClusterv1();
  }

  if(_topo_ohcal)
  {
    delete _topo_ohcal;
    _topo_ohcal = new RawClusterv1();
  }

  if(_topo_emcal)
  {
    delete _topo_emcal;
    _topo_emcal = new RawClusterv1();
  }

  RawCluster::TowerConstRange begin_end = topo_cluster->get_towers();
  for(RawCluster::TowerConstIterator towerIt = begin_end.first; towerIt != begin_end.second; ++towerIt)
  {
    if(RawTowerDefs::decode_caloid(towerIt->first) ==  RawTowerDefs::CalorimeterId::HCALOUT)
    {
      _topo_ohcal->addTower(towerIt->first, towerIt->second);
    }
    else if(RawTowerDefs::decode_caloid(towerIt->first) ==  RawTowerDefs::CalorimeterId::HCALIN)
    {
      _topo_ihcal->addTower(towerIt->first, towerIt->second);
    }
    else if(RawTowerDefs::decode_caloid(towerIt->first) ==  RawTowerDefs::CalorimeterId::CEMC)
    {
      _topo_emcal->addTower(towerIt->first, towerIt->second);
    }
  }
}

PHG4Particle* TopoClusterEval::get_truth_particle(RawCluster* topo_cluster)
{
  SeparateSubsystems(topo_cluster);

  PHG4Particle *g4particle = max_truth_primary_particle_by_energy();

  return g4particle;
}

std::set<PHG4Shower*> TopoClusterEval::all_truth_primary_showers()
{
  std::set<PHG4Shower*> all_showers;

  all_showers.merge(_clustereval_emcal.all_truth_primary_showers(_topo_emcal));
  all_showers.merge(_clustereval_ihcal.all_truth_primary_showers(_topo_ihcal));
  all_showers.merge(_clustereval_ohcal.all_truth_primary_showers(_topo_ohcal));

  return all_showers;
}

PHG4Shower* TopoClusterEval::max_truth_primary_shower_by_energy()
{
  PHG4Shower *max_shower = nullptr;

  std::set<PHG4Shower*> showers = all_truth_primary_showers();

  float max_shower_energy = 0.;

  for(std::set<PHG4Shower*>::iterator itr = showers.begin(); itr != showers.end(); itr++)
  {
    float shower_energy = _clustereval_emcal.get_energy_contribution(_topo_emcal, (*itr));
    shower_energy += _clustereval_ihcal.get_energy_contribution(_topo_ihcal, (*itr));
    shower_energy += _clustereval_ohcal.get_energy_contribution(_topo_ohcal, (*itr));

    if(shower_energy > max_shower_energy)
    {
      max_shower_energy = shower_energy;
      max_shower = (*itr);
    }
  }

  return max_shower;
}

std::set<PHG4Shower*> TopoClusterEval::all_truth_primary_showers_emcal()
{
  return _clustereval_emcal.all_truth_primary_showers(_topo_emcal);
}

std::set<PHG4Shower*> TopoClusterEval::all_truth_primary_showers_hcal()
{
  std::set<PHG4Shower*> all_showers = all_truth_primary_showers_ihcal();

  all_showers.merge(all_truth_primary_showers_ohcal());

  return all_showers;
}

std::set<PHG4Shower*> TopoClusterEval::all_truth_primary_showers_ihcal()
{
  return _clustereval_ihcal.all_truth_primary_showers(_topo_ihcal);
}

std::set<PHG4Shower*> TopoClusterEval::all_truth_primary_showers_ohcal()
{
  return _clustereval_ohcal.all_truth_primary_showers(_topo_ohcal);
}

PHG4Shower* TopoClusterEval::max_truth_primary_shower_by_energy_emcal()
{
  return _clustereval_emcal.max_truth_primary_shower_by_energy(_topo_emcal);
}

PHG4Shower* TopoClusterEval::max_truth_primary_shower_by_energy_hcal()
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

PHG4Shower* TopoClusterEval::max_truth_primary_shower_by_energy_ihcal()
{
  return _clustereval_ihcal.max_truth_primary_shower_by_energy(_topo_ihcal);
}

PHG4Shower* TopoClusterEval::max_truth_primary_shower_by_energy_ohcal()
{
  return _clustereval_ihcal.max_truth_primary_shower_by_energy(_topo_ohcal);
}

std::set<PHG4Particle*> TopoClusterEval::all_truth_primary_particles()
{
  std::set<PHG4Particle*> all_primaries;

  all_primaries.merge(_clustereval_emcal.all_truth_primary_particles(_topo_emcal));
  all_primaries.merge(_clustereval_ihcal.all_truth_primary_particles(_topo_ihcal));
  all_primaries.merge(_clustereval_ohcal.all_truth_primary_particles(_topo_ohcal));

  return all_primaries;
}

std::set<PHG4Particle*> TopoClusterEval::all_truth_primary_particles_emcal()
{
  return _clustereval_emcal.all_truth_primary_particles(_topo_emcal);
}

std::set<PHG4Particle*> TopoClusterEval::all_truth_primary_particles_hcal()
{
  std::set<PHG4Particle*> all_primaries = all_truth_primary_particles_ihcal();

  all_primaries.merge(all_truth_primary_particles_ohcal());

  return all_primaries;
}

std::set<PHG4Particle*> TopoClusterEval::all_truth_primary_particles_ihcal()
{
  return _clustereval_ihcal.all_truth_primary_particles(_topo_ihcal);
}

std::set<PHG4Particle*> TopoClusterEval::all_truth_primary_particles_ohcal()
{
  return _clustereval_ohcal.all_truth_primary_particles(_topo_ohcal);
}

PHG4Particle* TopoClusterEval::max_truth_primary_particle_by_energy()
{
  PHG4Particle *max_primary = nullptr;

  std::set<PHG4Particle*> primaries = all_truth_primary_particles();

  float max_primary_energy = 0.;

  for(std::set<PHG4Particle*>::iterator itr = primaries.begin(); itr != primaries.end(); itr++)
  {
    float primary_energy = _clustereval_emcal.get_energy_contribution(_topo_emcal, (*itr));
    primary_energy += _clustereval_ihcal.get_energy_contribution(_topo_ihcal, (*itr));
    primary_energy += _clustereval_ohcal.get_energy_contribution(_topo_ohcal, (*itr));

    if(primary_energy > max_primary_energy)
    {
      max_primary_energy = primary_energy;
      max_primary = (*itr);
    }
  }

  return max_primary;
}

PHG4Particle* TopoClusterEval::max_truth_primary_particle_by_energy_emcal()
{
  return _clustereval_emcal.max_truth_primary_particle_by_energy(_topo_emcal);
}

PHG4Particle* TopoClusterEval::max_truth_primary_particle_by_energy_hcal()
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

PHG4Particle* TopoClusterEval::max_truth_primary_particle_by_energy_ihcal()
{
  return _clustereval_ihcal.max_truth_primary_particle_by_energy(_topo_ihcal);
}

PHG4Particle* TopoClusterEval::max_truth_primary_particle_by_energy_ohcal()
{
  return _clustereval_ohcal.max_truth_primary_particle_by_energy(_topo_ohcal);
}

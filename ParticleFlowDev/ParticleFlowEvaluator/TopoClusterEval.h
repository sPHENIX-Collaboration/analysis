#ifndef G4EVAL_TOPOCLUSTEREVAL_H
#define G4EVAL_TOPOCLUSTEREVAL_H

#include "CaloRawClusterEval.h"

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <calobase/RawTowerDefs.h>

class PHCompositeNode;
class PHG4Particle;
class PHG4Shower;
class RawCluster;
class CaloRawClusterEval;

class TopoClusterEval
{
 public:
  /// example caloname: CEMC, HCALIN, HCALOUT
  TopoClusterEval(PHCompositeNode* topNode, const std::string &type);
  virtual ~TopoClusterEval();

  //Pass the particle to be evaluated, then get other information
  PHG4Particle* get_truth_particle(RawCluster *topo_cluster);

  /// reinitialize the eval for a new event
  void next_event(PHCompositeNode* topNode);

  std::set<PHG4Shower*> all_truth_primary_showers();
  std::set<PHG4Shower*> all_truth_primary_showers_emcal();
  std::set<PHG4Shower*> all_truth_primary_showers_hcal();
  std::set<PHG4Shower*> all_truth_primary_showers_ihcal();
  std::set<PHG4Shower*> all_truth_primary_showers_ohcal();

  PHG4Shower* max_truth_primary_shower_by_energy();
  PHG4Shower* max_truth_primary_shower_by_energy_emcal();
  PHG4Shower* max_truth_primary_shower_by_energy_hcal();
  PHG4Shower* max_truth_primary_shower_by_energy_ihcal();
  PHG4Shower* max_truth_primary_shower_by_energy_ohcal();

  std::set<PHG4Particle*> all_truth_primary_particles();
  std::set<PHG4Particle*> all_truth_primary_particles_emcal();
  std::set<PHG4Particle*> all_truth_primary_particles_hcal();
  std::set<PHG4Particle*> all_truth_primary_particles_ihcal();
  std::set<PHG4Particle*> all_truth_primary_particles_ohcal();

  PHG4Particle* max_truth_primary_particle_by_energy();
  PHG4Particle* max_truth_primary_particle_by_energy_emcal();
  PHG4Particle* max_truth_primary_particle_by_energy_hcal();
  PHG4Particle* max_truth_primary_particle_by_energy_ihcal();
  PHG4Particle* max_truth_primary_particle_by_energy_ohcal();

  void set_usetowerinfo(bool b)
  {
    _clustereval_emcal.set_usetowerinfo(b);
    _clustereval_ihcal.set_usetowerinfo(b);
    _clustereval_ohcal.set_usetowerinfo(b);
  }

 private:
  //Separate the HCal layers (inner and outer) in order to evaluate
  void SeparateSubsystems(RawCluster *topo_cluster);

  std::string _topocluster_type = "EMCAL_HCAL";

  CaloRawClusterEval _clustereval_emcal;
  CaloRawClusterEval _clustereval_ihcal;
  CaloRawClusterEval _clustereval_ohcal;

  RawCluster *_topo_emcal = nullptr;
  RawCluster *_topo_ihcal = nullptr;
  RawCluster *_topo_ohcal = nullptr;

};

#endif  // G4EVAL_TOPOCLUSTEREVAL_H

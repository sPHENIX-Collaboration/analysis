#ifndef G4EVAL_PARTICLEFLOWEVAL_H
#define G4EVAL_PARTICLEFLOWEVAL_H

#include "CaloRawClusterEval.h"
#include "CaloRawTowerEval.h"
#include "CaloTruthEval.h"
#include "SvtxTrackEval.h"

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <calobase/RawTowerDefs.h>

class PHCompositeNode;
class PHG4Hit;
class PHG4Particle;
class PHG4Shower;
class RawClusterContainer;
class RawCluster;
class RawTowerContainer;
class TowerInfoContainer;
class ParticleFlowElement;
class SvtxTrack;
class CaloRawClusterEval;



class ParticleFlowEval
{
 public:
  /// example caloname: CEMC, HCALIN, HCALOUT
  ParticleFlowEval(PHCompositeNode* topNode);
  virtual ~ParticleFlowEval();

  //Separate the HCal layers (inner and outer) in order to evaluate
  void SeparateHCals(RawCluster *hcluster);

  //Pass the particle to be evaluated, then get other information
  PHG4Particle* get_truth_particle_flow(ParticleFlowElement *pflow);

  /// reinitialize the eval for a new event
  void next_event(PHCompositeNode* topNode);

  std::set<PHG4Shower*> all_truth_primary_showers_emcal();
  std::set<PHG4Shower*> all_truth_primary_showers_emcal(RawCluster *cluster);
  std::set<PHG4Shower*> all_truth_primary_showers_hcal();
  std::set<PHG4Shower*> all_truth_primary_showers_ihcal();
  std::set<PHG4Shower*> all_truth_primary_showers_ohcal();

  PHG4Shower* max_truth_primary_shower_by_energy_emcal();
  PHG4Shower* max_truth_primary_shower_by_energy_emcal(RawCluster *cluster);
  PHG4Shower* max_truth_primary_shower_by_energy_hcal();
  PHG4Shower* max_truth_primary_shower_by_energy_ihcal();
  PHG4Shower* max_truth_primary_shower_by_energy_ohcal();

  std::set<PHG4Particle*> all_truth_primary_particles_emcal();
  std::set<PHG4Particle*> all_truth_primary_particles_emcal(RawCluster* cluster);
  std::set<PHG4Particle*> all_truth_primary_particles_hcal();
  std::set<PHG4Particle*> all_truth_primary_particles_ihcal();
  std::set<PHG4Particle*> all_truth_primary_particles_ohcal();

  PHG4Particle* max_truth_primary_particle_by_energy_emcal();
  PHG4Particle* max_truth_primary_particle_by_energy_emcal(RawCluster* cluster);
  PHG4Particle* max_truth_primary_particle_by_energy_hcal();
  PHG4Particle* max_truth_primary_particle_by_energy_ihcal();
  PHG4Particle* max_truth_primary_particle_by_energy_ohcal();
  PHG4Particle* max_truth_particle_by_nclusters_track();

 private:
  CaloRawClusterEval _clustereval_emcal;
  CaloRawClusterEval _clustereval_ihcal;
  CaloRawClusterEval _clustereval_ohcal;
  SvtxTrackEval _trackeval;

  ParticleFlowElement *_pflow = nullptr;
  SvtxTrack *_track = nullptr;
  std::vector<RawCluster*> _topo_emcal;
  RawCluster *_topo_ihcal = nullptr;
  RawCluster *_topo_ohcal = nullptr;

};

#endif  // G4EVAL_PARTICLEFLOWEVAL_H

#pragma once

#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

#include "JetUtils.h"

#include <cmath>
#include <optional>
#include <random>
#include <unordered_map>

struct RandomCone {
  double eta = 0.0;
  double phi = 0.0;
  double pt = 0.0;
  double energy = 0.0;
  double radius = 0.0;
};

class RandomConeMaker {
public:
  // Initialize with a cone radius and an optional seed for reproducibility
  explicit RandomConeMaker(double radius, uint32_t seed = 0);

  // Call this ONCE during your SubSysReco::InitRun to cache tower geometries
  void init(RawTowerGeomContainer* geom_cemc, RawTowerGeomContainer* geom_hcalin, RawTowerGeomContainer* geom_hcalout);

  // Call this per-event to get a random cone (or supply manual cone_eta/cone_phi)
  // z_vrtx allows for correcting the tower kinematics for the event vertex
  RandomCone generate(
      TowerInfoContainer *cemc_retower,
      TowerInfoContainer *hcalin,
      TowerInfoContainer *hcalout,
      double z_vrtx = 0.0,
      std::optional<double> cone_eta = std::nullopt,
      std::optional<double> cone_phi = std::nullopt);

  void setSeed(uint32_t seed);
  void setRadius(double radius);

private:
  struct TowerGeomInfo {
    double eta = 0.0;
    double phi = 0.0;
  };

  double m_radius;
  std::mt19937 m_generator;

  // Detector radii (in cm) from JetUtils
  double m_r_cemc = JetUtils::radius_EM;
  double m_r_hcalin = JetUtils::radius_IH;
  double m_r_hcalout = JetUtils::radius_OH;

  // Distribution for eta: [-1.1 + R, 1.1 - R]
  std::uniform_real_distribution<double> m_eta_dist;

  // Distribution for phi: [0, 2pi)
  std::uniform_real_distribution<double> m_phi_dist;

  // Cache the geometries by custom integer key ((ieta << 16) | iphi)
  std::unordered_map<unsigned int, TowerGeomInfo> m_geom_hcalin;
  std::unordered_map<unsigned int, TowerGeomInfo> m_geom_hcalout;

  // Helper method to process a single calorimeter's towers
  void process_calorimeter(
      TowerInfoContainer *towers,
      const std::unordered_map<unsigned int, TowerGeomInfo> &geom_map,
      double detector_radius,
      double cone_eta,
      double cone_phi,
      double z_vrtx,
      double &total_pt,
      double &total_energy) const;
};

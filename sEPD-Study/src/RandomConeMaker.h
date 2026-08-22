#pragma once

#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

#include "JetUtils.h"
#include "geometry_constants.h"

#include <array>
#include <cmath>
#include <optional>
#include <random>
#include <vector>

struct RandomCone {
  double eta = 0.0;
  double phi = 0.0;
  double pt = 0.0;
  double energy = 0.0;
  double radius = 0.0;

  // Tower level details per calorimeter
  std::vector<int> emcal_tower_index;
  std::vector<double> emcal_tower_energy;
  std::vector<double> emcal_tower_pt;

  std::vector<int> ihcal_tower_index;
  std::vector<double> ihcal_tower_energy;
  std::vector<double> ihcal_tower_pt;

  std::vector<int> ohcal_tower_index;
  std::vector<double> ohcal_tower_energy;
  std::vector<double> ohcal_tower_pt;
};

class RandomConeMaker {
public:
  // Initialize with a cone radius and an optional seed for reproducibility.
  // If seed == 0 (the default), PHRandomSeed::GetSeed() is automatically used.
  explicit RandomConeMaker(double radius, uint32_t seed = 0);

  void init(RawTowerGeomContainer* geom_hcalin, RawTowerGeomContainer* geom_hcalout);

  // Call this per-event to get a random cone (or supply manual cone_eta/cone_phi)
  // z_vrtx allows for correcting the tower kinematics for the event vertex
  RandomCone generate(
      TowerInfoContainer *cemc_retower,
      TowerInfoContainer *hcalin,
      TowerInfoContainer *hcalout,
      double z_vrtx = 0.0,
      bool save_tower_info = false,
      std::optional<double> cone_eta = std::nullopt,
      std::optional<double> cone_phi = std::nullopt);

  void setSeed(uint32_t seed);
  void setRadius(double radius);
  bool isInitialized() const { return m_initialized; }

private:
  static constexpr size_t N_HCAL_TOWERS = CaloGeometry::HCAL_ETA_BINS * CaloGeometry::HCAL_PHI_BINS;

  struct TowerGeomInfo {
    double eta = 0.0;
    double phi = 0.0;
    double z0 = 0.0;
    bool is_valid = false;
  };

  using TowerGeomArray = std::array<TowerGeomInfo, N_HCAL_TOWERS>;

  bool m_initialized = false;
  double m_radius;
  double m_radius_sq;
  std::mt19937 m_generator;

  // Detector radii (in cm) from JetUtils
  double m_r_cemc = JetUtils::radius_EM;
  double m_r_hcalin = JetUtils::radius_IH;
  double m_r_hcalout = JetUtils::radius_OH;

  // Distribution for phi: [0, 2pi)
  std::uniform_real_distribution<double> m_phi_dist;

  // Cache the geometries in flat contiguous arrays (indexed by (ieta * HCAL_PHI_BINS + iphi))
  TowerGeomArray m_geom_cemc{};
  TowerGeomArray m_geom_hcalin{};
  TowerGeomArray m_geom_hcalout{};

  // Helper method to process a single calorimeter's towers
  void process_calorimeter(
      TowerInfoContainer *towers,
      const TowerGeomArray &geom_map,
      double detector_radius,
      double cone_eta,
      double cone_phi,
      double z_vrtx,
      double &total_pt,
      double &total_energy,
      std::vector<int> *tower_indices = nullptr,
      std::vector<double> *tower_energies = nullptr,
      std::vector<double> *tower_pts = nullptr) const;
};

#include "RandomConeMaker.h"

#include <phool/PHRandomSeed.h>

#include <numbers>
#include <stdexcept>
#include <format>

namespace
{
  constexpr size_t make_geom_index(unsigned int ieta, unsigned int iphi)
  {
    return static_cast<size_t>(ieta) * CaloGeometry::HCAL_PHI_BINS + iphi;
  }
}  // namespace

RandomConeMaker::RandomConeMaker(double radius, uint32_t seed)
  : m_radius(radius)
  , m_radius_sq(radius * radius)
  , m_generator(seed == 0 ? PHRandomSeed::GetSeed() : seed)
  , m_phi_dist(0.0, 2.0 * std::numbers::pi)
{
}

void RandomConeMaker::init(RawTowerGeomContainer *geom_hcalin,
                           RawTowerGeomContainer *geom_hcalout)
{
  m_geom_cemc.fill(TowerGeomInfo{});
  m_geom_hcalin.fill(TowerGeomInfo{});
  m_geom_hcalout.fill(TowerGeomInfo{});

  // Detector radii are using the effective values from JetUtils (initialized in the header)
  // We do NOT overwrite them with geom->get_radius() because get_radius() returns the inner
  // face of the calorimeter, whereas we want the effective tower center radius for z-vertex corrections.

  std::cout << std::format("Using the following radius: EMCal: {}, IHCal: {}, OHCal: {}", m_r_cemc, m_r_hcalin, m_r_hcalout) << std::endl;

  // Cache HCALIN grid and Retowered CEMC grid (both share HCALIN eta/phi bin geometry)
  if (geom_hcalin)
  {
    for (unsigned int ieta = 0; ieta < CaloGeometry::HCAL_ETA_BINS; ++ieta)
    {
      for (unsigned int iphi = 0; iphi < CaloGeometry::HCAL_PHI_BINS; ++iphi)
      {
        RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
        RawTowerGeom *geom = geom_hcalin->get_tower_geometry(key);
        if (geom)
        {
          double eta = geom->get_eta();
          double phi = geom->get_phi();
          double sinh_eta = std::sinh(eta);
          size_t idx = make_geom_index(ieta, iphi);

          // HCALIN
          m_geom_hcalin[idx] = {eta, phi, sinh_eta * m_r_hcalin, true};

          // Retowered CEMC shares the same eta/phi tower centers but sits at m_r_cemc radius
          m_geom_cemc[idx] = {eta, phi, sinh_eta * m_r_cemc, true};
        }
      }
    }
  }

  // Cache HCALOUT geometries
  if (geom_hcalout)
  {
    for (unsigned int ieta = 0; ieta < CaloGeometry::HCAL_ETA_BINS; ++ieta)
    {
      for (unsigned int iphi = 0; iphi < CaloGeometry::HCAL_PHI_BINS; ++iphi)
      {
        RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
        RawTowerGeom *geom = geom_hcalout->get_tower_geometry(key);
        if (geom)
        {
          double eta = geom->get_eta();
          double phi = geom->get_phi();
          double sinh_eta = std::sinh(eta);
          size_t idx = make_geom_index(ieta, iphi);

          m_geom_hcalout[idx] = {eta, phi, sinh_eta * m_r_hcalout, true};
        }
      }
    }
  }

  m_initialized = true;
}

RandomCone RandomConeMaker::generate(
    TowerInfoContainer *cemc_retower,
    TowerInfoContainer *hcalin,
    TowerInfoContainer *hcalout,
    double z_vrtx,
    std::optional<double> cone_eta,
    std::optional<double> cone_phi)
{
  if (!m_initialized)
  {
    throw std::runtime_error("RandomConeMaker::generate: init() must be called before generate()!");
  }

  RandomCone cone;
  cone.radius = m_radius;

  if (cone_eta.has_value())
  {
    cone.eta = cone_eta.value();
  }
  else
  {
    auto [min_eta, max_eta] = JetUtils::get_valid_eta_range(z_vrtx, m_radius, m_r_cemc, m_r_hcalin, m_r_hcalout);
    if (min_eta < max_eta)
    {
      std::uniform_real_distribution<double> eta_dist(min_eta, max_eta);
      cone.eta = eta_dist(m_generator);
    }
    else
    {
      cone.eta = 0.5 * (min_eta + max_eta);
    }
  }

  cone.phi = cone_phi.has_value() ? cone_phi.value() : m_phi_dist(m_generator);
  cone.pt = 0.0;
  cone.energy = 0.0;

  // Process all three calorimeters
  process_calorimeter(cemc_retower, m_geom_cemc, m_r_cemc, cone.eta, cone.phi, z_vrtx, cone.pt, cone.energy);
  process_calorimeter(hcalin, m_geom_hcalin, m_r_hcalin, cone.eta, cone.phi, z_vrtx, cone.pt, cone.energy);
  process_calorimeter(hcalout, m_geom_hcalout, m_r_hcalout, cone.eta, cone.phi, z_vrtx, cone.pt, cone.energy);

  return cone;
}

void RandomConeMaker::setSeed(uint32_t seed)
{
  m_generator.seed(seed == 0 ? PHRandomSeed::GetSeed() : seed);
}

void RandomConeMaker::setRadius(double radius)
{
  m_radius = radius;
  m_radius_sq = radius * radius;
}

void RandomConeMaker::process_calorimeter(
    TowerInfoContainer *towers,
    const TowerGeomArray &geom_map,
    double detector_radius,
    double cone_eta,
    double cone_phi,
    double z_vrtx,
    double &total_pt,
    double &total_energy) const
{
  if (!towers)
  {
    return;
  }

  unsigned int nchannels = towers->size();
  for (unsigned int channel = 0; channel < nchannels; ++channel)
  {
    TowerInfo *tower = towers->get_tower_at_channel(channel);
    if (!tower || !tower->get_isGood())
    {
      continue;
    }

    unsigned int channelkey = towers->encode_key(channel);
    unsigned int ieta = towers->getTowerEtaBin(channelkey);
    unsigned int iphi = towers->getTowerPhiBin(channelkey);

    if (ieta >= CaloGeometry::HCAL_ETA_BINS || iphi >= CaloGeometry::HCAL_PHI_BINS)
    {
      continue;
    }

    size_t idx = make_geom_index(ieta, iphi);
    const auto &geom = geom_map[idx];
    if (!geom.is_valid)
    {
      continue;  // Skip if no geometry is found for this bin
    }

    // Apply z-vertex correction to tower eta using precomputed z0
    double eta_corrected = geom.eta;
    if (z_vrtx != 0.0 && detector_radius > 0.0)
    {
      double z = geom.z0 - z_vrtx;
      eta_corrected = std::asinh(z / detector_radius);
    }

    // Calculate delta R between tower and cone center
    double dphi = geom.phi - cone_phi;
    // Handle phi wrap-around (single-branch wrapping since dphi is within [-3pi, 3pi])
    if (dphi > std::numbers::pi)
    {
      dphi -= 2.0 * std::numbers::pi;
    }
    else if (dphi < -std::numbers::pi)
    {
      dphi += 2.0 * std::numbers::pi;
    }

    double deta = eta_corrected - cone_eta;
    double dr2 = deta * deta + dphi * dphi;

    // Include tower if it falls within the cone
    if (dr2 < m_radius_sq)
    {
      double energy = tower->get_energy();
      double pt = energy / std::cosh(eta_corrected);

      total_energy += energy;
      total_pt += pt;
    }
  }
}

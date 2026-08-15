#include "RandomConeMaker.h"
#include "geometry_constants.h"

#include <numbers>

namespace
{
  constexpr unsigned int make_geom_key(unsigned int ieta, unsigned int iphi)
  {
    return (ieta << 16U) | iphi;
  }
}  // namespace

RandomConeMaker::RandomConeMaker(double radius, uint32_t seed)
  : m_radius(radius)
  , m_generator(seed)
  , m_eta_dist(-1.1 + radius, 1.1 - radius)
  , m_phi_dist(0.0, 2.0 * std::numbers::pi)
{
}

void RandomConeMaker::init(RawTowerGeomContainer *geom_cemc,
                           RawTowerGeomContainer *geom_hcalin,
                           RawTowerGeomContainer *geom_hcalout)
{
  m_geom_hcalin.clear();
  m_geom_hcalout.clear();

  if (geom_cemc)
  {
    m_r_cemc = geom_cemc->get_radius();
  }
  if (geom_hcalin)
  {
    m_r_hcalin = geom_hcalin->get_radius();
  }
  if (geom_hcalout)
  {
    m_r_hcalout = geom_hcalout->get_radius();
  }

  // Cache HCALIN grid (also used for CEMC retower)
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
          m_geom_hcalin[make_geom_key(ieta, iphi)] = {geom->get_eta(), geom->get_phi()};
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
          m_geom_hcalout[make_geom_key(ieta, iphi)] = {geom->get_eta(), geom->get_phi()};
        }
      }
    }
  }
}

RandomCone RandomConeMaker::generate(
    TowerInfoContainer *cemc_retower,
    TowerInfoContainer *hcalin,
    TowerInfoContainer *hcalout,
    double z_vrtx,
    std::optional<double> cone_eta,
    std::optional<double> cone_phi)
{
  RandomCone cone;
  cone.radius = m_radius;
  cone.eta = cone_eta.has_value() ? cone_eta.value() : m_eta_dist(m_generator);
  cone.phi = cone_phi.has_value() ? cone_phi.value() : m_phi_dist(m_generator);
  cone.pt = 0.0;
  cone.energy = 0.0;

  // Process all three calorimeters
  // Retowered CEMC and IHCal both share m_geom_hcalin but use their respective radii
  process_calorimeter(cemc_retower, m_geom_hcalin, m_r_cemc, cone.eta, cone.phi, z_vrtx, cone.pt, cone.energy);
  process_calorimeter(hcalin, m_geom_hcalin, m_r_hcalin, cone.eta, cone.phi, z_vrtx, cone.pt, cone.energy);

  // HCALOUT uses its own mapping and radius
  process_calorimeter(hcalout, m_geom_hcalout, m_r_hcalout, cone.eta, cone.phi, z_vrtx, cone.pt, cone.energy);

  return cone;
}

void RandomConeMaker::setSeed(uint32_t seed) { m_generator.seed(seed); }

void RandomConeMaker::setRadius(double radius)
{
  m_radius = radius;
  // Reconfigure the eta boundaries based on the new radius
  m_eta_dist = std::uniform_real_distribution<double>(-1.1 + radius, 1.1 - radius);
}

void RandomConeMaker::process_calorimeter(
    TowerInfoContainer *towers,
    const std::unordered_map<unsigned int, TowerGeomInfo> &geom_map,
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

    unsigned int geom_key = make_geom_key(ieta, iphi);
    auto it = geom_map.find(geom_key);
    if (it == geom_map.end())
    {
      continue;  // Skip if no geometry is found for this bin
    }

    double tower_eta = it->second.eta;
    double tower_phi = it->second.phi;

    // Apply z-vertex correction to tower eta
    double eta_corrected = tower_eta;
    if (z_vrtx != 0.0 && detector_radius > 0.0)
    {
      double z0 = std::sinh(tower_eta) * detector_radius;
      double z = z0 - z_vrtx;
      eta_corrected = std::asinh(z / detector_radius);
    }

    // Calculate delta R between tower and cone center
    double dphi = tower_phi - cone_phi;
    // Handle phi wrap-around
    while (dphi > std::numbers::pi)
    {
      dphi -= 2.0 * std::numbers::pi;
    }
    while (dphi < -std::numbers::pi)
    {
      dphi += 2.0 * std::numbers::pi;
    }

    double deta = eta_corrected - cone_eta;
    double dr = std::sqrt(deta * deta + dphi * dphi);

    // Include tower if it falls within the cone
    if (dr < m_radius)
    {
      double energy = tower->get_energy();
      double pt = energy / std::cosh(eta_corrected);

      total_energy += energy;
      total_pt += pt;
    }
  }
}

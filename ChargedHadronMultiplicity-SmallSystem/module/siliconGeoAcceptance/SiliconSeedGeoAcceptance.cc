#include "SiliconSeedGeoAcceptance.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>

#include <trackbase/ActsGeometry.h>
#include <trackbase/TrkrDefs.h>

#include <Acts/Definitions/Algebra.hpp>
#include <Acts/Definitions/Units.hpp>
#include <Acts/Surfaces/Surface.hpp>

#include <TFile.h>
#include <TH2D.h>
#include <TRandom3.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace
{
constexpr double pi = 3.14159265358979323846;
constexpr double twoPi = 2.0 * pi;
constexpr double surfacePhiWindow = 0.3;
constexpr double parallelTolerance = 1.0e-6;
constexpr double surfaceTolerance = 1.0 * Acts::UnitConstants::mm;
constexpr unsigned int lastMvtxLayer = 2;
} // namespace

SiliconSeedGeoAcceptance::SiliconSeedGeoAcceptance(const std::string &name) : SubsysReco(name) {}

int SiliconSeedGeoAcceptance::InitRun(PHCompositeNode *topNode)
{
    // MakeActsGeometry creates this PAR/SVTX node; tracking modules retrieve it the same way (for example trackreco/PHActsSiliconSeeding.cc::getNodes)
    m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
    const auto &gctx = m_tGeometry->geometry().getGeoContext();

    m_outputFile = new TFile(m_outputFilename.c_str(), "RECREATE");
    for (int nMvtx = 0; nMvtx < m_nMvtxLayerCounts; ++nMvtx)
    {
        for (int nIntt = 0; nIntt < m_nInttLayerCounts; ++nIntt)
        {
            const std::string name = "h_A_" + std::to_string(nMvtx) + "_" + std::to_string(nIntt);
            m_hA[nMvtx][nIntt] = new TH2D(name.c_str(), ";#eta;vtx z [cm]", m_etaBins, m_etaLo, m_etaHi, m_vtxZBins, m_vtxZLo, m_vtxZHi);
        }
    }
    m_hN = new TH2D("h_N", ";#eta;vtx z [cm]", m_etaBins, m_etaLo, m_etaHi, m_vtxZBins, m_vtxZLo, m_vtxZHi);

    // trackreco/MakeActsGeometry.cc fills this map, which is declared in trackbase/ActsSurfaceMaps.h. Its keys identify MVTX/INTT hit sets
    const auto &surfaceMap = m_tGeometry->maps().m_siliconSurfaceMap;
    m_surfaceLayers.reserve(surfaceMap.size());
    m_surfacePhis.reserve(surfaceMap.size());
    m_surfaces.reserve(surfaceMap.size());
    for (const auto &[hitsetkey, surface] : surfaceMap)
    {
        const auto center = surface->center(gctx);
        // The hit-set-key to tracker-layer decoding is defined in trackbase/TrkrDefs.cc
        m_surfaceLayers.push_back(TrkrDefs::getLayer(hitsetkey));
        m_surfacePhis.push_back(std::atan2(center.y(), center.x()));
        m_surfaces.push_back(surface);
    }

    // Sorting once makes the module-specific phi-window lookup below cheap
    std::vector<std::size_t> order(m_surfaces.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [this](std::size_t lhs, std::size_t rhs) { return m_surfacePhis[lhs] < m_surfacePhis[rhs]; });

    std::vector<unsigned int> sortedLayers;
    std::vector<double> sortedPhis;
    std::vector<std::shared_ptr<const Acts::Surface>> sortedSurfaces;
    sortedLayers.reserve(order.size());
    sortedPhis.reserve(order.size());
    sortedSurfaces.reserve(order.size());
    for (const auto index : order)
    {
        sortedLayers.push_back(m_surfaceLayers[index]);
        sortedPhis.push_back(m_surfacePhis[index]);
        sortedSurfaces.push_back(m_surfaces[index]);
    }
    m_surfaceLayers = std::move(sortedLayers);
    m_surfacePhis = std::move(sortedPhis);
    m_surfaces = std::move(sortedSurfaces);

    // Throw rays uniformly within each (eta, vertex-z) bin and average over phi
    TRandom3 rng(m_randomSeed);
    for (int etaBin = 1; etaBin <= m_etaBins; ++etaBin)
    {
        const double etaLo = m_hN->GetXaxis()->GetBinLowEdge(etaBin);
        const double etaHi = m_hN->GetXaxis()->GetBinUpEdge(etaBin);
        for (int vtxZBin = 1; vtxZBin <= m_vtxZBins; ++vtxZBin)
        {
            const double vtxZLo = m_hN->GetYaxis()->GetBinLowEdge(vtxZBin);
            const double vtxZHi = m_hN->GetYaxis()->GetBinUpEdge(vtxZBin);
            int count[m_nMvtxLayerCounts][m_nInttLayerCounts] = {};

            for (int ray = 0; ray < m_raysPerBin; ++ray)
            {
                // within the (eta, vtx-z) bin, throw a ray with uniform eta, vtx-z, and phi
                const double eta = rng.Uniform(etaLo, etaHi);
                const double vtxZ = rng.Uniform(vtxZLo, vtxZHi);
                const double phi = rng.Uniform(-pi, pi);

                int nMvtx = 0;
                int nIntt = 0;
                countLayers(eta, phi, vtxZ, nMvtx, nIntt);
                ++count[nMvtx][nIntt];
            }

            for (int nMvtx = 0; nMvtx < m_nMvtxLayerCounts; ++nMvtx)
            {
                for (int nIntt = 0; nIntt < m_nInttLayerCounts; ++nIntt)
                {
                    m_hA[nMvtx][nIntt]->SetBinContent(etaBin, vtxZBin, static_cast<double>(count[nMvtx][nIntt]) / m_raysPerBin);
                }
            }
            m_hN->SetBinContent(etaBin, vtxZBin, m_raysPerBin);
        }
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

int SiliconSeedGeoAcceptance::process_event(PHCompositeNode * /*topNode*/) { return Fun4AllReturnCodes::ABORTEVENT; }

int SiliconSeedGeoAcceptance::End(PHCompositeNode * /*topNode*/)
{
    m_outputFile->Write();
    m_outputFile->Close();
    delete m_outputFile;
    m_outputFile = nullptr;
    return Fun4AllReturnCodes::EVENT_OK;
}

void SiliconSeedGeoAcceptance::countLayers(double eta, double phi, double vtxZ, int &nMvtx, int &nIntt) const
{
    // sPHENIX positions are in cm, while ACTS surface coordinates are in mm
    // The same Acts::UnitConstants conversion convention is used in TrackingDiagnostics/TrackResiduals.cc
    const double theta = 2.0 * std::atan(std::exp(-eta));
    const Acts::Vector3 origin(m_beamSpotX, m_beamSpotY, vtxZ); // where the ray originates P0 = (m_beamSpotX, m_beamSpotY, vtxZ)
    // The ray direction is a unit vector P1 - P0 = (sin(theta) cos(phi), sin(theta) sin(phi), cos(theta)); theta = 2 arctan(exp(-eta)) is the polar angle
    const Acts::Vector3 direction(std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta));

    // keeps only surfaces near the ray phi; two index ranges handle a window crossing the -pi/pi boundary
    std::array<std::size_t, 2> firstIndex = {};
    std::array<std::size_t, 2> lastIndex = {};
    int nRanges = 1;
    const double phiLo = phi - surfacePhiWindow;
    const double phiHi = phi + surfacePhiWindow;
    if (phiLo < -pi)
    {
        firstIndex[0] = 0;
        lastIndex[0] = std::upper_bound(m_surfacePhis.begin(), m_surfacePhis.end(), phiHi) - m_surfacePhis.begin();
        firstIndex[1] = std::lower_bound(m_surfacePhis.begin(), m_surfacePhis.end(), phiLo + twoPi) - m_surfacePhis.begin();
        lastIndex[1] = m_surfacePhis.size();
        nRanges = 2;
    }
    else if (phiHi > pi)
    {
        firstIndex[0] = std::lower_bound(m_surfacePhis.begin(), m_surfacePhis.end(), phiLo) - m_surfacePhis.begin();
        lastIndex[0] = m_surfacePhis.size();
        firstIndex[1] = 0;
        lastIndex[1] = std::upper_bound(m_surfacePhis.begin(), m_surfacePhis.end(), phiHi - twoPi) - m_surfacePhis.begin();
        nRanges = 2;
    }
    else
    {
        firstIndex[0] = std::lower_bound(m_surfacePhis.begin(), m_surfacePhis.end(), phiLo) - m_surfacePhis.begin();
        lastIndex[0] = std::upper_bound(m_surfacePhis.begin(), m_surfacePhis.end(), phiHi) - m_surfacePhis.begin();
    }

    const auto &gctx = m_tGeometry->geometry().getGeoContext();
    std::set<unsigned int> layersHit;
    for (int range = 0; range < nRanges; ++range)
    {
        for (std::size_t index = firstIndex[range]; index < lastIndex[range]; ++index)
        {
            const auto &surface = m_surfaces[index];
            const Acts::Vector3 centerMm = surface->center(gctx);
            const Acts::Vector3 center = centerMm / Acts::UnitConstants::cm; // surface center
            const Acts::Vector3 normal = surface->normal(gctx, centerMm, direction);
            const double denominator = normal.dot(direction);
            if (std::abs(denominator) < parallelTolerance)
            {
                continue;
            }

            // Intersect the forward ray with the plane tangent to the surface.
            const double pathLength = normal.dot(center - origin) / denominator;
            if (pathLength <= 0.0)
            {
                continue;
            }

            const Acts::Vector3 intersection = origin + pathLength * direction;
            // Convert the intersection to bound surface coordinates. This is the coresoftware pattern used in TrackingDiagnostics/TrackResiduals.cc
            // trackreco/MakeSourceLinks.cc; bounds().inside checks if points intersect the plane outside the physical sensor
            // If outside, the ray does not cross the sensor (not accepted)
            const auto local = surface->globalToLocal(gctx, intersection * Acts::UnitConstants::cm, direction, surfaceTolerance);
            if (!local.ok() || !surface->bounds().inside(local.value()))
            {
                continue;
            }
            // Count a tracker layer only once even if the ray crosses overlapping or neighboring sensor surfaces in that layer
            layersHit.insert(m_surfaceLayers[index]);
        }
    }

    nMvtx = 0;
    nIntt = 0;
    for (const auto layer : layersHit)
    {
        if (layer <= lastMvtxLayer)
        {
            ++nMvtx;
        }
        else
        {
            ++nIntt;
        }
    }
}

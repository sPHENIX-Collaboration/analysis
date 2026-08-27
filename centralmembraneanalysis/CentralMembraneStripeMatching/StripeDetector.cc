#include "StripeDetector.h"

#include "helpers.h"
#include "parameters.h"

#include <TH2.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <string>
#include <array>
#include <vector>

bool compare_seed_bins(const std::array<double, 5> &lhs, const std::array<double, 5> &rhs)
{
  // Process brighter seed bins first. This lets strong bins claim nearby weak
  // bins before a lower-content seed can start a duplicate component.
  if (lhs[component_content] != rhs[component_content])
  {
    return lhs[component_content] > rhs[component_content];
  }
  if (static_cast<int>(lhs[component_bin_y]) != static_cast<int>(rhs[component_bin_y]))
  {
    return static_cast<int>(lhs[component_bin_y]) < static_cast<int>(rhs[component_bin_y]);
  }
  return static_cast<int>(lhs[component_bin_x]) < static_cast<int>(rhs[component_bin_x]);
}

bool compare_stripe_content(const std::array<double, 3> &lhs, const std::array<double, 3> &rhs)
{
  // Keep high-integral stripe candidates first during duplicate removal.
  return lhs[stripe_content] > rhs[stripe_content];
}

int StripeDetector::wrapped_phi_bin(int bin, int nBinsX)
{
  // Histogram phi bins are periodic. Wrap any neighbor probe back into [1,N].
  while (bin < 1)
  {
    bin += nBinsX;
  }
  while (bin > nBinsX)
  {
    bin -= nBinsX;
  }
  return bin;
}

void StripeDetector::normalize_phi_to_axis(TH2 *histogram, double &phi)
{
  // ROOT histogram axes are finite even when the physics coordinate is
  // periodic, so normalize centroids into the stored axis range.
  const double phiMin = histogram->GetXaxis()->GetXmin();
  const double phiMax = histogram->GetXaxis()->GetXmax();
  const double phiWidth = phiMax - phiMin;
  while (phi < phiMin)
  {
    phi += phiWidth;
  }
  while (phi >= phiMax)
  {
    phi -= phiWidth;
  }
}

bool StripeDetector::detect(TH2 *histogram, std::vector<std::array<double, 3>> &stripes)
{
  // One public call owns the whole stripe finding workflow:
  // reset state, clone the histogram, find components, convert to centroids.
  if (!initialize(histogram, stripes) || !create_working_histogram(histogram))
  {
    clear();
    return false;
  }

  collect_seed_bins(histogram);
  build_connected_components();
  convert_components_to_raw_stripes(histogram);
  stripes = m_rawStripes;
  clear();
  return true;
}

bool StripeDetector::initialize(TH2 *histogram, std::vector<std::array<double, 3>> &stripes)
{
  // Cache dimensions early. A null histogram leaves dimensions at zero and
  // returns false below.
  m_nBinsX = histogram ? histogram->GetNbinsX() : 0;
  m_nBinsY = histogram ? histogram->GetNbinsY() : 0;

  stripes.clear();
  m_seedBinsByPhi.assign(m_nBinsX + 2, {});
  m_components.clear();
  m_rawStripes.clear();
  m_nSeedBins = 0;

  if (!histogram)
  {
    std::cout << "ERROR: Null histogram passed to StripeDetector" << std::endl;
    return false;
  }

  return true;
}

bool StripeDetector::create_working_histogram(TH2 *histogram)
{
  // The clone is detached from any ROOT directory so deleting it here is safe.
  clear();
  if (!histogram)
  {
    return false;
  }

  m_workingHist = dynamic_cast<TH2 *>(histogram->Clone((std::string(histogram->GetName()) + "_floodFillInput").c_str()));
  if (!m_workingHist)
  {
    std::cout << "ERROR: Could not clone input histogram for stripe detection" << std::endl;
    return false;
  }

  m_workingHist->SetDirectory(nullptr);
  return true;
}

void StripeDetector::clear()
{
  // clear() is safe to call repeatedly; delete handles nullptr.
  delete m_workingHist;
  m_workingHist = nullptr;
}

void StripeDetector::collect_seed_bins(TH2 *histogram)
{
  // Store every above-threshold bin as {binX, binY, phi, R, content}.
  // Grouping by phi bin makes later seed flattening straightforward.
  if (!histogram || !m_workingHist)
  {
    return;
  }

  m_seedBinsByPhi.assign(m_nBinsX + 2, {});
  m_nSeedBins = 0;
  for (int i = 1; i <= m_nBinsX; i++)
  {
    for (int j = 1; j <= m_nBinsY; j++)
    {
      const double content = m_workingHist->GetBinContent(i, j);
      if (content <= 0.0 || content < stripeFloodSeedMinContent)
      {
        continue;
      }

      const double r = histogram->GetYaxis()->GetBinCenter(j);
      const double phi = histogram->GetXaxis()->GetBinCenter(i);
      std::array<double, 5> seed{};
      seed[component_bin_x] = i;
      seed[component_bin_y] = j;
      seed[component_phi] = phi;
      seed[component_r] = r;
      seed[component_content] = content;
      m_seedBinsByPhi[i].push_back(seed);
    }
  }

  for (int i = 1; i <= m_nBinsX; i++)
  {
    m_nSeedBins += m_seedBinsByPhi[i].size();
  }
}

void StripeDetector::build_connected_components()
{
  

  // Flatten the phi-binned seed list so it can be sorted by content.
  m_components.clear();
  if (!m_workingHist)
  {
    return;
  }

  std::vector<std::array<double, 5>> seeds;
  seeds.reserve(m_nSeedBins);
  for (int i = 1; i <= m_nBinsX; i++)
  {
    for (const auto &point : m_seedBinsByPhi[i])
    {
      seeds.push_back(point);
    }
  }

  std::sort(seeds.begin(), seeds.end(), compare_seed_bins);

  std::vector<std::vector<bool>> used(m_nBinsX + 1, std::vector<bool>(m_nBinsY + 1, false));

  // Start a flood fill from each unused seed. Neighbor bins are allowed to be
  // close in phi/R but components are rejected if they grow too wide.
  for (const auto &seed : seeds)
  {
    if (used[static_cast<int>(seed[component_bin_x])][static_cast<int>(seed[component_bin_y])])
    {
      continue;
    }

    std::vector<std::array<double, 5>> component;
    std::queue<std::array<double, 5>> queue;
    used[static_cast<int>(seed[component_bin_x])][static_cast<int>(seed[component_bin_y])] = true;
    queue.push(seed);

    int minBinPhi = static_cast<int>(seed[component_bin_x]);
    int maxBinPhi = static_cast<int>(seed[component_bin_x]);
    int minBinR = static_cast<int>(seed[component_bin_y]);
    int maxBinR = static_cast<int>(seed[component_bin_y]);

    while (!queue.empty())
    {
      const std::array<double, 5> current = queue.front();
      queue.pop();
      component.push_back(current);

      for (int dx = -stripeFloodMaxPhiGapBins - 1; dx <= stripeFloodMaxPhiGapBins + 1; dx++)
      {
        for (int dy = -stripeFloodMaxRGapBins - 1; dy <= stripeFloodMaxRGapBins + 1; dy++)
        {
          if (dx == 0 && dy == 0)
          {
            continue;
          }

          const int binX = wrapped_phi_bin(static_cast<int>(current[component_bin_x]) + dx, m_nBinsX);
          const int binY = static_cast<int>(current[component_bin_y]) + dy;
          if (binY < 1 || binY > m_nBinsY || used[binX][binY])
          {
            continue;
          }

          const double content = m_workingHist->GetBinContent(binX, binY);
          if (content < stripeFloodGrowMinContent)
          {
            continue;
          }

          // Limit the bounding box so one flood fill does not merge nearby
          // stripes into a long blob.
          const int candidateMinPhi = std::min(minBinPhi, binX);
          const int candidateMaxPhi = std::max(maxBinPhi, binX);
          const int candidateMinR = std::min(minBinR, binY);
          const int candidateMaxR = std::max(maxBinR, binY);
          if (candidateMaxPhi - candidateMinPhi + 1 > stripeFloodMaxPhiSpanBins || candidateMaxR - candidateMinR + 1 > stripeFloodMaxRSpanBins)
          {
            continue;
          }

          const double r = m_workingHist->GetYaxis()->GetBinCenter(binY);
          const double phi = m_workingHist->GetXaxis()->GetBinCenter(binX);
          used[binX][binY] = true;
          minBinPhi = candidateMinPhi;
          maxBinPhi = candidateMaxPhi;
          minBinR = candidateMinR;
          maxBinR = candidateMaxR;
          std::array<double, 5> neighbor{};
          neighbor[component_bin_x] = binX;
          neighbor[component_bin_y] = binY;
          neighbor[component_phi] = phi;
          neighbor[component_r] = r;
          neighbor[component_content] = content;
          queue.push(neighbor);
        }
      }
    }

    if (!component.empty())
    {
      m_components.push_back(component);
    }
  }
}

void StripeDetector::convert_components_to_raw_stripes(TH2 *histogram)
{
  

  // Convert each connected component into one weighted centroid stripe.
  if (!histogram)
  {
    return;
  }

  m_rawStripes.clear();
  for (const auto &component : m_components)
  {
    if (static_cast<int>(component.size()) < stripeFloodMinBins)
    {
      continue;
    }

    double sumW = 0.0;
    double sumPhi = 0.0;
    double sumR = 0.0;
    double totalContent = 0.0;
    const double seedPhi = component.front()[component_phi];
    for (const auto &point : component)
    {
      // Average phi relative to the seed so components crossing the periodic
      // axis boundary do not get pulled to the wrong side.
      const double dphi = wrap_delta_phi(point[component_phi] - seedPhi);
      sumW += point[component_content];
      sumPhi += point[component_content] * dphi;
      sumR += point[component_content] * point[component_r];
      totalContent += point[component_content];
    }

    if (sumW <= 0.0 || totalContent < stripeFloodMinTotalContent)
    {
      continue;
    }

    std::array<double, 3> cluster{};
    cluster[stripe_phi] = seedPhi + sumPhi / sumW;
    normalize_phi_to_axis(histogram, cluster[stripe_phi]);
    cluster[stripe_r] = sumR / sumW;
    cluster[stripe_content] = totalContent;
    m_rawStripes.push_back(cluster);
  }

  std::sort(m_rawStripes.begin(), m_rawStripes.end(), compare_stripe_content);

  std::vector<std::array<double, 3>> uniqueStripes;
  uniqueStripes.reserve(m_rawStripes.size());
  // Remove duplicate centroids caused by overlapping seed regions.
  for (const auto &candidate : m_rawStripes)
  {
    bool duplicate = false;
    for (const auto &kept : uniqueStripes)
    {
      const double deltaR = std::abs(candidate[stripe_r] - kept[stripe_r]);
      const double deltaPhi = std::abs(wrap_delta_phi(candidate[stripe_phi] - kept[stripe_phi]));
      if (deltaR <= duplicateStripeMaxDeltaRCm && deltaPhi <= duplicateStripeMaxDeltaPhiRad)
      {
        duplicate = true;
        break;
      }
    }
    if (!duplicate)
    {
      uniqueStripes.push_back(candidate);
    }
  }
  m_rawStripes.swap(uniqueStripes);
}

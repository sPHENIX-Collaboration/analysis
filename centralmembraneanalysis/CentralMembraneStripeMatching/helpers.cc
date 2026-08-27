#include "helpers.h"

#include "parameters.h"

#include <TDirectory.h>
#include <TFile.h>
#include <TH2.h>
#include <TObject.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>

double wrap_delta_phi(double dphi)
{
  // Adjust by full turns until the difference is the shortest angular path.
  while (dphi > M_PI)
  {
    dphi -= 2.0 * M_PI;
  }
  while (dphi < -M_PI)
  {
    dphi += 2.0 * M_PI;
  }

  return dphi;
}

double distance_Rdphi(double phi1, double r1, double phi2, double r2)
{
  // Convert phi separation into a length using the mean radius.
  const double dphi = wrap_delta_phi(phi1 - phi2);
  const double dr = r1 - r2;
  const double rmean = 0.5 * (r1 + r2);
  const double rdphi = rmean * dphi;
  return std::sqrt(dr * dr + rdphi * rdphi);
}

double median_value(std::vector<double> values)
{
  // Work on a copy so callers keep their original ordering.
  if (values.empty())
  {
    return 0.0;
  }

  std::sort(values.begin(), values.end());

  const size_t n = values.size();
  if (n % 2 == 1)
  {
    return values[n / 2];
  }

  return 0.5 * (values[n / 2 - 1] + values[n / 2]);
}

double robust_mad_sigma(const std::vector<double> &values, double fallback)
{
  // Fewer than three points cannot define a useful robust width.
  if (values.size() < 3)
  {
    return fallback;
  }

  const double med = median_value(values);

  std::vector<double> abs_dev;
  abs_dev.reserve(values.size());
  for (double value : values)
  {
    abs_dev.push_back(std::abs(value - med));
  }

  const double mad = median_value(abs_dev);
  const double sigma = 1.4826 * mad;

  if (sigma <= 1e-6)
  {
    return fallback;
  }

  return sigma;
}

void safe_write_object(TObject *obj)
{
  // Only write when a writable ROOT file is currently active.
  TFile *file = gDirectory ? gDirectory->GetFile() : nullptr;
  if (!obj || !file || !file->IsOpen() || !file->IsWritable())
  {
    return;
  }

  obj->Write("", TObject::kOverwrite);
}

TH2 *load_detached_histogram(TFile *file, const char *histogram_name)
{
  // Detach the histogram from the input file so it survives after file close.
  auto *hist = file ? dynamic_cast<TH2 *>(file->Get(histogram_name)) : nullptr;
  if (hist)
  {
    hist->SetDirectory(nullptr);
  }
  return hist;
}

void filter_isolated_stripes(const std::vector<std::array<double, 3>> &input, std::vector<std::array<double, 3>> &output)
{
  // Count neighbors in the local R/Rdphi metric and keep dense stripe groups.
  output.clear();
  for (size_t i = 0; i < input.size(); ++i)
  {
    int neighbor_count = 0;
    for (size_t j = 0; j < input.size(); ++j)
    {
      if (i == j)
      {
        continue;
      }
      if (distance_Rdphi(input[i][stripe_phi], input[i][stripe_r], input[j][stripe_phi], input[j][stripe_r]) <= isolation_radius_cm)
      {
        ++neighbor_count;
      }
    }
    if (neighbor_count >= min_isolation_neighbors)
    {
      output.push_back(input[i]);
    }
  }
}

inline constexpr int radial_peak_r = 3;
inline constexpr int radial_peak_low_r = 4;
inline constexpr int radial_peak_high_r = 5;
inline constexpr int radial_gap_low_bin_y = 0;
inline constexpr int radial_gap_high_bin_y = 1;
inline constexpr int radial_gap_low_r = 2;
inline constexpr int radial_gap_high_r = 3;

size_t lamination_mask_index(int nBinsY, int phiBin, int rBin)
{
  // Store a 2D mask in one vector. ROOT bins start at one; vectors start at zero.
  return static_cast<size_t>(phiBin - 1) * static_cast<size_t>(nBinsY) + static_cast<size_t>(rBin - 1);
}

void mask_lamination_bin(TH2 *histogram, std::vector<char> &masked, int nBinsY, int i, int j, long long &counter)
{
  // Mask one bin and count it only once even if multiple rules hit it.
  const size_t index = lamination_mask_index(nBinsY, i, j);
  if (!masked[index])
  {
    masked[index] = true;
    counter++;
  }
  histogram->SetBinContent(i, j, 0.0);
  histogram->SetBinError(i, j, 0.0);
}

void mask_lamination_radial_range(TH2 *histogram, std::vector<char> &masked, int nBinsX, int nBinsY, int lowBinY, int highBinY, long long &counter)
{
  // Mask all phi columns over a radial interval.
  for (int i = 1; i <= nBinsX; i++)
  {
    for (int j = lowBinY; j <= highBinY; j++)
    {
      mask_lamination_bin(histogram, masked, nBinsY, i, j, counter);
    }
  }
}

void mask_lamination_phi_column(TH2 *histogram, std::vector<char> &masked, int nBinsY, int i, long long &counter)
{
  // Mask all radii for one phi column.
  for (int j = 1; j <= nBinsY; j++)
  {
    mask_lamination_bin(histogram, masked, nBinsY, i, j, counter);
  }
}

bool compare_radial_peak_r(const std::array<double, 7> &lhs, const std::array<double, 7> &rhs)
{
  // Sort outer-to-inner so gaps are built between neighboring rows.
  return lhs[radial_peak_r] > rhs[radial_peak_r];
}

double effective_gap_mask_padding(double gapWidth)
{
  // Do not let fixed padding consume too much of a narrow gap.
  return std::min(radialGapMaskPaddingCm, radialGapMaskMaxPaddingFraction * gapWidth);
}

std::pair<TH2 *, std::vector<double>> clean_laminations(TH2 *histogram)
{
  

  // first: cleaned histogram. second: radial gap centers used as field controls.
  std::pair<TH2 *, std::vector<double>> result{nullptr, {}};

  if (!histogram)
  {
    return result;
  }

  result.first = dynamic_cast<TH2 *>(histogram->Clone((std::string(histogram->GetName()) + "_cleaned").c_str()));
  if (!result.first)
  {
    std::cout << "ERROR: Could not clone histogram for lamination cleaning: " << histogram->GetName() << std::endl;
    return result;
  }
  result.first->SetDirectory(nullptr);

  const int nBinsX = histogram->GetNbinsX();
  const int nBinsY = histogram->GetNbinsY();
  auto *xAxis = histogram->GetXaxis();
  auto *yAxis = histogram->GetYaxis();

  // Internal mask bookkeeping is intentionally kept in plain vectors rather
  // than ROOT histograms. This function is on the hot path before stripe
  // detection, and the diagnostic mask histograms are no longer written.
  std::vector<char> masked(static_cast<size_t>(nBinsX) * static_cast<size_t>(nBinsY), false);
  std::vector<double> radialProjection(static_cast<size_t>(nBinsY) + 1, 0.0);
  std::vector<double> phiCenters(static_cast<size_t>(nBinsX) + 1, 0.0);
  for (int i = 1; i <= nBinsX; i++)
  {
    phiCenters[static_cast<size_t>(i)] = xAxis->GetBinCenter(i);
  }

  // 1. Find stripe-row peaks in the radial projection. The gaps between these
  // rows are where laminations live and where masking is allowed.
  for (int j = 1; j <= nBinsY; j++)
  {
    double sum = 0.0;
    for (int i = 1; i <= nBinsX; i++)
    {
      const double content = histogram->GetBinContent(i, j);
      if (content < radialRowProjectionMinContent)
      {
        continue;
      }

      sum += content;
    }
    radialProjection[static_cast<size_t>(j)] = sum;
  }

  const double radialProjectionMax = *std::max_element(radialProjection.begin() + 1, radialProjection.end());
  const double rowPeakThreshold = radialRowPeakThresholdFrac * radialProjectionMax;

  std::vector<std::array<double, 7>> rowPeaks;
  // A row peak stores {bin, low bin, high bin, R, low R, high R, projection}.
  for (int j = 2; j <= nBinsY - 1; j++)
  {
    const double r = yAxis->GetBinCenter(j);
    const double value = radialProjection[static_cast<size_t>(j)];

    if (r < radialRowPeakMinRCm || value < rowPeakThreshold)
    {
      continue;
    }
    if (value <= radialProjection[static_cast<size_t>(j - 1)] || value <= radialProjection[static_cast<size_t>(j + 1)])
    {
      continue;
    }
    bool separated = true;
    for (const auto &peak : rowPeaks)
    {
      if (std::abs(r - peak[radial_peak_r]) < radialRowPeakMinSeparationCm)
      {
        separated = false;
        break;
      }
    }

    if (separated)
    {
      const double rowBoundaryThreshold = radialRowBoundaryThresholdFrac * value;
      int lowBinY = j;
      int highBinY = j;

      while (lowBinY > 1 && yAxis->GetBinCenter(lowBinY - 1) >= r - radialRowBoundaryMaxHalfWidthCm && radialProjection[static_cast<size_t>(lowBinY - 1)] >= rowBoundaryThreshold)
      {
        lowBinY--;
      }

      while (highBinY < nBinsY && yAxis->GetBinCenter(highBinY + 1) <= r + radialRowBoundaryMaxHalfWidthCm && radialProjection[static_cast<size_t>(highBinY + 1)] >= rowBoundaryThreshold)
      {
        highBinY++;
      }

      std::array<double, 7> rowPeak{};
      rowPeak[0] = j;
      rowPeak[1] = lowBinY;
      rowPeak[2] = highBinY;
      rowPeak[3] = r;
      rowPeak[4] = yAxis->GetBinCenter(lowBinY);
      rowPeak[5] = yAxis->GetBinCenter(highBinY);
      rowPeak[6] = value;
      rowPeaks.push_back(rowPeak);
    }
  }

  std::sort(rowPeaks.begin(), rowPeaks.end(), compare_radial_peak_r);

  long long largeGapMaskBins = 0;
  long long laminationPhiMaskBins = 0;
  long long radialGapMaskBins = 0;

  // 2. Convert neighboring row peaks into radial gaps. Very large gaps are
  // masked immediately because they do not contain usable stripe information.
  std::vector<std::array<double, 4>> radialGaps;
  // A radial gap stores {low bin, high bin, low R, high R}.
  for (size_t k = 0; k + 1 < rowPeaks.size(); k++)
  {
    const double highR = rowPeaks[k][radial_peak_low_r];
    const double lowR = rowPeaks[k + 1][radial_peak_high_r];
    if (lowR < laminationOuterGapMinRCm)
    {
      break;
    }

    const double gapHighR = highR - radialGapBoundaryPaddingCm;
    const double gapLowR = lowR + radialGapBoundaryPaddingCm;

    if (gapHighR <= gapLowR)
    {
      continue;
    }

    const int lowBinY = yAxis->FindBin(gapLowR);
    const int highBinY = yAxis->FindBin(gapHighR);
    if (highBinY < lowBinY)
    {
      continue;
    }

    const double gapWidth = gapHighR - gapLowR;
    if (gapWidth > radialGapMaxLaminationWidthCm)
    {
      const double gapMaskPadding = effective_gap_mask_padding(gapWidth);
      const int paddedLowBinY = std::max(1, yAxis->FindBin(gapLowR + gapMaskPadding));
      const int paddedHighBinY = std::min(nBinsY, yAxis->FindBin(gapHighR - gapMaskPadding));
      if (paddedHighBinY < paddedLowBinY)
      {
        continue;
      }
      mask_lamination_radial_range(result.first, masked, nBinsX, nBinsY, paddedLowBinY, paddedHighBinY, largeGapMaskBins);
      continue;
    }

    const int clampedLowBinY = std::max(1, lowBinY);
    const int clampedHighBinY = std::min(nBinsY, highBinY);

    std::array<double, 4> radialGap{};
    radialGap[radial_gap_low_bin_y] = clampedLowBinY;
    radialGap[radial_gap_high_bin_y] = clampedHighBinY;
    radialGap[radial_gap_low_r] = yAxis->GetBinCenter(clampedLowBinY);
    radialGap[radial_gap_high_r] = yAxis->GetBinCenter(clampedHighBinY);
    radialGaps.push_back(radialGap);

    if (static_cast<int>(radialGaps.size()) >= maxLaminationGapCount)
    {
      break;
    }
  }

  // 3. Within each acceptable radial gap, project onto phi and count which phi
  // bins repeatedly carry lamination-like content across gaps.
  std::vector<double> globalPhiSupport(nBinsX + 1, 0.0);
  for (size_t igap = 0; igap < radialGaps.size(); igap++)
  {
    const auto &gap = radialGaps[igap];
    std::vector<double> phiProjection(nBinsX + 1, 0.0);
    std::vector<double> radialWeights(static_cast<size_t>(static_cast<int>(gap[radial_gap_high_bin_y])) + 1, 1.0);
    if (weightLaminationGapByDistanceFromRows)
    {
      const double gapWidth = gap[radial_gap_high_r] - gap[radial_gap_low_r];
      const double halfGapWidth = 0.5 * gapWidth;
      if (halfGapWidth > 0.0)
      {
        for (int j = static_cast<int>(gap[radial_gap_low_bin_y]); j <= static_cast<int>(gap[radial_gap_high_bin_y]); j++)
        {
          const double r = yAxis->GetBinCenter(j);
          const double distanceFromNearestRow = std::min(r - gap[radial_gap_low_r], gap[radial_gap_high_r] - r);
          double weight = std::clamp(distanceFromNearestRow / halfGapWidth, 0.0, 1.0);
          radialWeights[static_cast<size_t>(j)] = std::pow(weight, laminationGapRadialWeightPower);
        }
      }
    }

    for (int i = 1; i <= nBinsX; i++)
    {
      double sum = 0.0;
      for (int j = static_cast<int>(gap[radial_gap_low_bin_y]); j <= static_cast<int>(gap[radial_gap_high_bin_y]); j++)
      {
        sum += radialWeights[static_cast<size_t>(j)] * histogram->GetBinContent(i, j);
      }

      phiProjection[i] = sum;
    }

    for (int i = 1; i <= nBinsX; i++)
    {
      const double value = phiProjection[i];
      if (value < laminationGapPhiMinContent)
      {
        continue;
      }
      for (int di = -laminationGlobalPhiIntegralHalfWindowBins; di <= laminationGlobalPhiIntegralHalfWindowBins; di++)
      {
        int supportBin = i + di;
        while (supportBin < 1)
        {
          supportBin += nBinsX;
        }
        while (supportBin > nBinsX)
        {
          supportBin -= nBinsX;
        }
        globalPhiSupport[supportBin]++;
      }
    }
  }

  // 4. Suppress duplicate nearby phi candidates so each lamination contributes
  // one narrow masked phi band.
  while (true)
  {
    int closestBinA = 0;
    int closestBinB = 0;
    double closestDistance = laminationGlobalPhiSuppressWindowRad;

    for (int i = 1; i <= nBinsX; i++)
    {
      if (globalPhiSupport[i] <= 0.0)
      {
        continue;
      }

      const double phi = phiCenters[static_cast<size_t>(i)];
      for (int ii = i + 1; ii <= nBinsX; ii++)
      {
        if (globalPhiSupport[ii] <= 0.0)
        {
          continue;
        }
        const double otherPhi = phiCenters[static_cast<size_t>(ii)];
        const double dphi = std::abs(wrap_delta_phi(phi - otherPhi));
        if (dphi > laminationGlobalPhiSuppressWindowRad)
        {
          continue;
        }
        if (dphi < closestDistance)
        {
          closestDistance = dphi;
          closestBinA = i;
          closestBinB = ii;
        }
      }
    }

    if (closestBinA == 0 || closestBinB == 0)
    {
      break;
    }

    if (globalPhiSupport[closestBinB] > globalPhiSupport[closestBinA])
    {
      globalPhiSupport[closestBinA] = 0.0;
    }
    else
    {
      globalPhiSupport[closestBinB] = 0.0;
    }
  }

  // 5. Apply the phi-band masks and then mask all accepted radial gaps.
  std::vector<double> laminationPhis;
  for (int i = 1; i <= nBinsX; i++)
  {
    if (globalPhiSupport[i] <= laminationGlobalPhiSupportThreshold)
    {
      continue;
    }

    const double phi = phiCenters[static_cast<size_t>(i)];
    laminationPhis.push_back(phi);
  }

  for (double lamPhi : laminationPhis)
  {
    const int firstBin = wrapPhiForPreLaminationMask ? 1 : std::max(1, xAxis->FindBin(lamPhi - laminationPhiMaskHalfWidthRad) - 1);
    const int lastBin = wrapPhiForPreLaminationMask ? nBinsX : std::min(nBinsX, xAxis->FindBin(lamPhi + laminationPhiMaskHalfWidthRad) + 1);
    for (int i = firstBin; i <= lastBin; i++)
    {
      const double phi = phiCenters[static_cast<size_t>(i)];
      const double dphi = wrapPhiForPreLaminationMask ? std::abs(wrap_delta_phi(phi - lamPhi)) : std::abs(phi - lamPhi);

      if (dphi > laminationPhiMaskHalfWidthRad)
      {
        continue;
      }

      mask_lamination_phi_column(result.first, masked, nBinsY, i, laminationPhiMaskBins);
    }
  }

  for (const auto &gap : radialGaps)
  {
    const double gapWidth = gap[radial_gap_high_r] - gap[radial_gap_low_r];
    const double gapMaskPadding = effective_gap_mask_padding(gapWidth);
    const int paddedLowBinY = std::max(1, yAxis->FindBin(gap[radial_gap_low_r] + gapMaskPadding));
    const int paddedHighBinY = std::min(nBinsY, yAxis->FindBin(gap[radial_gap_high_r] - gapMaskPadding));
    if (paddedHighBinY < paddedLowBinY)
    {
      continue;
    }
    mask_lamination_radial_range(result.first, masked, nBinsX, nBinsY, paddedLowBinY, paddedHighBinY, radialGapMaskBins);
  }
  result.second.reserve(radialGaps.size());
  for (const auto &gap : radialGaps)
  {
    result.second.push_back(0.5 * (gap[radial_gap_low_r] + gap[radial_gap_high_r]));
  }

  const long long totalBins = static_cast<long long>(nBinsX) * static_cast<long long>(nBinsY);
  const long long totalMaskedBins = largeGapMaskBins + laminationPhiMaskBins + radialGapMaskBins;
  std::cout << "Lamination cleaning for " << histogram->GetName() << ": radialGaps=" << radialGaps.size() << " laminationPhis=" << laminationPhis.size() << " maskedBins=" << totalMaskedBins << "/" << totalBins << " largeGap=" << largeGapMaskBins << " laminationPhi=" << laminationPhiMaskBins << " radialGap=" << radialGapMaskBins << std::endl;

  return result;
}

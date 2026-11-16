#include "TpcCentralMembraneStripeMatching.h"
#include "StripeComparison.h"
#include "StripeDetector.h"
#include "helpers.h"
#include "parameters.h"

#include <cdbobjects/CDBTTree.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <trackbase/LaserCluster.h>
#include <trackbase/LaserClusterContainer.h>
#include <trackbase/TpcDefs.h>

#include <TFile.h>
#include <TDirectory.h>
#include <TH2.h>
#include <TH3.h>
#include <TVector3.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>


TpcCentralMembraneStripeMatching::TpcCentralMembraneStripeMatching(const std::string &name) : SubsysReco(name) {}

int TpcCentralMembraneStripeMatching::InitRun(PHCompositeNode * /*topNode*/)
{
  if (m_fillReferenceHistogramsOnly)
  {
    CreateReferenceHistograms();
    return 0;
  }

  if (!useIdealStripesAsReference)
  {
    LoadReferenceHistograms();
  }
  CreateMeasuredHistograms();
  return 0;
}

int TpcCentralMembraneStripeMatching::process_event(PHCompositeNode *topNode)
{
  const int nodeStatus = GetNodes(topNode);
  if (nodeStatus != Fun4AllReturnCodes::EVENT_OK)
  {
    return nodeStatus;
  }

  if (m_fillReferenceHistogramsOnly)
  {
    FillReferenceHistograms();
  }
  else
  {
    FillMeasuredHistograms();
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

int TpcCentralMembraneStripeMatching::End(PHCompositeNode * /*topNode*/)
{
  if (m_fillReferenceHistogramsOnly)
  {
    std::cout << "Filled reference hPetal histograms from LASER_CLUSTER over " << m_processedEvents << " events: negz=" << m_filledReferenceClusters[0] << " posz=" << m_filledReferenceClusters[1] << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  TFile *outputfile = new TFile(m_outputfile.c_str(), "RECREATE");
  std::cout << "Writing output to file: " << m_outputfile << std::endl;
  std::cout << "Filled measured hPetal histograms from LASER_CLUSTER over " << m_processedEvents << " events: negz=" << m_filledClusters[0] << " posz=" << m_filledClusters[1] << std::endl;
  outputfile->cd();

  if (useIdealStripesAsReference)
  {
    // Read ideal stripe positions
    CDBTTree *cdbttree = new CDBTTree(m_idealStripePatternFile);
    cdbttree->LoadCalibrations();
    auto cdbMap = cdbttree->GetDoubleEntryMap();
    m_stripes[1][0].clear();
    m_stripes[1][1].clear();
    for (const auto &entry : cdbMap)
    {
      const auto index = entry.first;
      std::array<double, 3> stripe{};
      stripe[stripe_r] = cdbttree->GetDoubleValue(index, "truthR");
      stripe[stripe_phi] = cdbttree->GetDoubleValue(index, "truthPhi");
      if (stripe[stripe_phi] < 0.0)
      {
        stripe[stripe_phi] += 2 * M_PI;
      }

      if (index > 180000)
      {
        m_stripes[1][0].push_back(stripe);
      }
      else
      {
        m_stripes[1][1].push_back(stripe);
      }
    }
    delete cdbttree;

    if (m_stripes[1][0].empty() || m_stripes[1][1].empty())
    {
      std::cerr << "Ideal stripe pattern file has no stripes on one side: " << m_idealStripePatternFile << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
    std::cout << "Loaded ideal stripe positions from " << m_idealStripePatternFile << ": negz=" << m_stripes[1][0].size() << " posz=" << m_stripes[1][1].size() << std::endl;
  }

  // This applies a mask to laminations and noisy regions in the radial gaps
  // between rows of stripes
  if (CleanClusterHistograms() != Fun4AllReturnCodes::EVENT_OK)
  {
    std::cout << "Error in CleanClusterHistograms" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if (!useIdealStripesAsReference)
  {
    for (int side = 0; side < 2; ++side)
    {
      m_stripes[1][side].clear();
      m_detector.detect(m_hPetalCleaned[1][side], m_stripes[1][side]);
    }
  }

  for (int side = 0; side < 2; ++side)
  {
    m_stripes[0][side].clear();
    m_detector.detect(m_hPetalCleaned[0][side], m_stripes[0][side]);
    ComputeStripeComparisonMaps(m_stripes[0][side], m_stripes[1][side], m_lamination_radial_gap_centers[0][side], side);
  }

  outputfile->Write();
  outputfile->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

void TpcCentralMembraneStripeMatching::setOutputfile(const std::string &outputfile)
{
  m_outputfile = outputfile;
}

TH2 *TpcCentralMembraneStripeMatching::cloneReferenceHistogram(int side, const std::string &name) const
{
  if (side < 0 || side >= 2 || !m_hPetal[1][side])
  {
    return nullptr;
  }

  std::string cloneName = name;
  if (cloneName.empty())
  {
    cloneName = std::string(m_hPetal[1][side]->GetName()) + "_clone";
  }

  auto *clone = dynamic_cast<TH2 *>(m_hPetal[1][side]->Clone(cloneName.c_str()));
  if (clone)
  {
    clone->SetDirectory(nullptr);
  }
  return clone;
}

int TpcCentralMembraneStripeMatching::GetNodes(PHCompositeNode *topNode)
{
  m_laserClusterContainer = findNode::getClass<LaserClusterContainer>(topNode, "LASER_CLUSTER");
  if (!m_laserClusterContainer)
  {
    std::cout << "TpcCentralMembraneStripeMatching::GetNodes - LASER_CLUSTER node missing" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_dcc_in_module_edge = findNode::getClass<TpcDistortionCorrectionContainer>(topNode, "TpcDistortionCorrectionContainerModuleEdge");
  // if (m_dcc_in_module_edge)
  // {
  //   std::cout << "TpcLaminationFitting::GetNodes - found TPC distortion correction container module edge" << std::endl;
  // }

  m_dcc_in_static = findNode::getClass<TpcDistortionCorrectionContainer>(topNode, "TpcDistortionCorrectionContainerStatic");
  // if (m_dcc_in_static)
  // {
  //   std::cout << "TpcLaminationFitting::GetNodes - found TPC distortion correction container static" << std::endl;
  // }

  return Fun4AllReturnCodes::EVENT_OK;
}

void TpcCentralMembraneStripeMatching::CreateMeasuredHistograms()
{
  const char *names[2] = {"hPetal_measured_negz_raw", "hPetal_measured_posz_raw"};

  for (int side = 0; side < 2; ++side)
  {
    delete m_hPetal[0][side];
    m_hPetal[0][side] = new TH2D(names[side], names[side], m_phiBins, 0.0, 2.0 * M_PI, m_rBins, m_rMin, m_rMax);
    m_hPetal[0][side]->SetDirectory(nullptr);
  }
}

void TpcCentralMembraneStripeMatching::CreateReferenceHistograms()
{
  const char *names[2] = {"hPetal_reference_negz_raw", "hPetal_reference_posz_raw"};

  for (int side = 0; side < 2; ++side)
  {
    delete m_hPetal[1][side];
    m_hPetal[1][side] = new TH2D(names[side], names[side], m_phiBins, 0.0, 2.0 * M_PI, m_rBins, m_rMin, m_rMax);
    m_hPetal[1][side]->SetDirectory(nullptr);
  }
}

void TpcCentralMembraneStripeMatching::LoadReferenceHistograms()
{
  auto *referenceFile = TFile::Open(m_referenceStripePatternFile.c_str(), "READ");
  if (!referenceFile || referenceFile->IsZombie())
  {
    std::cout << "ERROR: Could not open reference stripe pattern file: " << m_referenceStripePatternFile << std::endl;
    return;
  }
  m_hPetal[1][0] = load_detached_histogram(referenceFile, "hPetal_South");
  m_hPetal[1][1] = load_detached_histogram(referenceFile, "hPetal_North");
  referenceFile->Close();

  if (!m_hPetal[1][0] || !m_hPetal[1][1])
  {
    std::cout << "ERROR: Missing hPetal_South or hPetal_North reference histogram" << std::endl;
  }
}

void TpcCentralMembraneStripeMatching::FillClusterHistograms(int histogramSet)
{
  if (histogramSet < 0 || histogramSet >= 2 || !m_laserClusterContainer || !m_hPetal[histogramSet][0] || !m_hPetal[histogramSet][1])
  {
    return;
  }

  ++m_processedEvents;
  const auto clusterRange = m_laserClusterContainer->getClusters();
  for (auto clusterIter = clusterRange.first; clusterIter != clusterRange.second; ++clusterIter)
  {
    const auto &[clusterKey, cluster] = *clusterIter;
    if (!cluster)
    {
      continue;
    }

    bool side = (bool) TpcDefs::getSide(clusterKey);
    Acts::Vector3 pos(cluster->getX(), cluster->getY(), (side ? 1.0 : -1.0));
    if (m_dcc_in_module_edge)
    {
      pos = m_distortionCorrection.get_corrected_position(pos, m_dcc_in_module_edge);
    }

    TVector3 tmp_pos(pos[0], pos[1], pos[2]);


    const unsigned int nLayers = cluster->getNLayers();
    if (nLayers < 2 || cluster->getSDWeightedLayer() >= 0.5)
    {
      continue;
    }

    const int iside = side ? 1 : 0;
    // m_hPetal[0][side]->Fill(NormalizeClusterPhi(std::atan2(cluster->getY(), cluster->getX())), std::hypot(cluster->getX(), cluster->getY()));
    m_hPetal[histogramSet][iside]->Fill(NormalizeClusterPhi(tmp_pos.Phi()), tmp_pos.Perp());

    if (histogramSet == 0)
    {
      ++m_filledClusters[iside];
    }
    else
    {
      ++m_filledReferenceClusters[iside];
    }
  }
}

void TpcCentralMembraneStripeMatching::FillMeasuredHistograms()
{
  FillClusterHistograms(0);
}

void TpcCentralMembraneStripeMatching::FillReferenceHistograms()
{
  FillClusterHistograms(1);
}

double TpcCentralMembraneStripeMatching::NormalizeClusterPhi(double phi) const
{
  while (phi < 0.0)
  {
    phi += 2.0 * M_PI;
  }
  while (phi >= 2.0 * M_PI)
  {
    phi -= 2.0 * M_PI;
  }
  return phi;
}

void TpcCentralMembraneStripeMatching::ComputeStripeComparisonMaps(const std::vector<std::array<double, 3>> &measured, const std::vector<std::array<double, 3>> &reference, const std::vector<double> &measuredRadialGapCenters, int side)
{
  if (!m_comparison.initialize(measured, reference, side, measuredRadialGapCenters) || !m_comparison.filter_isolated_inputs(measured, reference) || !m_comparison.build_global_pattern_matches() || !m_comparison.build_global_field_estimates())
  {
    m_comparison.clear();
    return;
  }

  m_comparison.write_output_maps();
  m_comparison.write_corrected_measured_histogram(m_hPetal[0][side]);
  WriteStaticCorrectedMeasuredHistogram(side);
  m_comparison.write_distorted_reference_histogram(m_hPetal[1][side]);
  m_comparison.clear();
}

void TpcCentralMembraneStripeMatching::WriteStaticCorrectedMeasuredHistogram(int side)
{
  if (side < 0 || side >= 2 || !m_hPetal[0][side])
  {
    return;
  }

  TDirectory *outputDirectory = gDirectory;
  const char *sideNames[2] = {"negz", "posz"};
  auto *staticFile = TFile::Open(m_staticCorrectionFile.c_str(), "READ");
  if (!staticFile || staticFile->IsZombie())
  {
    std::cout << "WARNING: Could not open static correction map file: " << m_staticCorrectionFile << std::endl;
    if (outputDirectory)
    {
      outputDirectory->cd();
    }
    return;
  }

  const std::string suffix = sideNames[side];
  auto *deltaRMap = dynamic_cast<TH3 *>(staticFile->Get((std::string("hIntDistortionR_") + suffix).c_str()));
  auto *deltaPhiMap = dynamic_cast<TH3 *>(staticFile->Get((std::string("hIntDistortionP_") + suffix).c_str()));
  if (!deltaRMap || !deltaPhiMap)
  {
    std::cout << "WARNING: Missing hIntDistortionR_" << suffix << " or hIntDistortionP_" << suffix << " in static correction map file: " << m_staticCorrectionFile << std::endl;
    staticFile->Close();
    if (outputDirectory)
    {
      outputDirectory->cd();
    }
    return;
  }

  const auto closestZBinToCentralMembrane = [](TH3 *histogram) {
    int closestBin = 1;
    double closestAbsZ = std::abs(histogram->GetZaxis()->GetBinCenter(closestBin));
    for (int zBin = 2; zBin <= histogram->GetNbinsZ(); ++zBin)
    {
      const double absZ = std::abs(histogram->GetZaxis()->GetBinCenter(zBin));
      if (absZ < closestAbsZ)
      {
        closestAbsZ = absZ;
        closestBin = zBin;
      }
    }
    return closestBin;
  };
  const int zBinR = closestZBinToCentralMembrane(deltaRMap);
  const int zBinPhi = closestZBinToCentralMembrane(deltaPhiMap);
  std::cout << "Writing static corrected measured histogram for " << suffix
            << " using static map z bins R=" << zBinR << " (z=" << deltaRMap->GetZaxis()->GetBinCenter(zBinR)
            << ") P=" << zBinPhi << " (z=" << deltaPhiMap->GetZaxis()->GetBinCenter(zBinPhi) << ", interpreted as R#Delta#phi)" << std::endl;

  TH2 *sourceHistogram = m_hPetal[0][side];
  const int nPhiBins = sourceHistogram->GetNbinsX();
  const int nRBins = sourceHistogram->GetNbinsY();
  auto *shifted = new TH2D((std::string("hPetal_measured_corrected_static_") + suffix).c_str(),
                           (std::string("Measured cluster histogram shifted by static distortion map z=0 slice - ") + suffix + ";#phi [rad];R [cm]").c_str(),
                           nPhiBins, sourceHistogram->GetXaxis()->GetXmin(), sourceHistogram->GetXaxis()->GetXmax(),
                           nRBins, sourceHistogram->GetYaxis()->GetXmin(), sourceHistogram->GetYaxis()->GetXmax());
  shifted->SetDirectory(nullptr);

  const double phiMin = sourceHistogram->GetXaxis()->GetXmin();
  const double phiMax = sourceHistogram->GetXaxis()->GetXmax();
  const double phiWidth = phiMax - phiMin;
  for (int phiBin = 1; phiBin <= nPhiBins; ++phiBin)
  {
    const double phi = sourceHistogram->GetXaxis()->GetBinCenter(phiBin);
    for (int rBin = 1; rBin <= nRBins; ++rBin)
    {
      const double content = sourceHistogram->GetBinContent(phiBin, rBin);
      if (content == 0.0)
      {
        continue;
      }

      const double r = sourceHistogram->GetYaxis()->GetBinCenter(rBin);
      const int staticPhiBinR = deltaRMap->GetXaxis()->FindBin(phi);
      const int staticRBinR = deltaRMap->GetYaxis()->FindBin(r);
      const int staticPhiBinP = deltaPhiMap->GetXaxis()->FindBin(phi);
      const int staticRBinP = deltaPhiMap->GetYaxis()->FindBin(r);
      const double deltaR = deltaRMap->GetBinContent(staticPhiBinR, staticRBinR, zBinR);
      const double rDeltaPhi = deltaPhiMap->GetBinContent(staticPhiBinP, staticRBinP, zBinPhi);
      const double deltaPhi = std::abs(r) > 1e-6 ? rDeltaPhi / r : 0.0;

      const double shiftedR = r - deltaR;
      double shiftedPhi = phi - deltaPhi;
      while (shiftedPhi < phiMin)
      {
        shiftedPhi += phiWidth;
      }
      while (shiftedPhi >= phiMax)
      {
        shiftedPhi -= phiWidth;
      }

      const int shiftedPhiBin = shifted->GetXaxis()->FindBin(shiftedPhi);
      const int shiftedRBin = shifted->GetYaxis()->FindBin(shiftedR);
      if (shiftedPhiBin < 1 || shiftedPhiBin > nPhiBins || shiftedRBin < 1 || shiftedRBin > nRBins)
      {
        continue;
      }
      shifted->SetBinContent(shiftedPhiBin, shiftedRBin, shifted->GetBinContent(shiftedPhiBin, shiftedRBin) + content);
    }
  }

  shifted->SetStats(false);
  if (outputDirectory)
  {
    outputDirectory->cd();
  }
  safe_write_object(shifted);
  delete shifted;
  staticFile->Close();
  if (outputDirectory)
  {
    outputDirectory->cd();
  }
}

int TpcCentralMembraneStripeMatching::CleanClusterHistograms()
{
  if (!m_hPetal[0][0] || !m_hPetal[0][1])
  {
    std::cout << "WARNING: No measured stripe pattern histograms filled" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if (!useIdealStripesAsReference && (!m_hPetal[1][0] || !m_hPetal[1][1]))
  {
    std::cout << "WARNING: No reference stripe pattern histograms loaded" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  const char *run_names[2] = {"measured", "reference"};
  const char *side_names[2] = {"negz", "posz"};
  const int run_count = useIdealStripesAsReference ? 1 : 2;
  for (int r = 0; r < run_count; r++)
  {
    for (int s = 0; s < 2; s++)
    {
      if (m_hPetal[r][s])
      {
        m_hPetal[r][s]->SetDirectory(nullptr);
        m_hPetal[r][s]->SetName((std::string("hPetal_") + run_names[r] + "_" + side_names[s]).c_str());
        safe_write_object(m_hPetal[r][s]);
      }

      if (!applyPreStripeLaminationMask)
      {
        delete m_hPetalCleaned[r][s];
        m_hPetalCleaned[r][s] = dynamic_cast<TH2 *>(m_hPetal[r][s]->Clone((std::string(m_hPetal[r][s]->GetName()) + "_cleaned").c_str()));
        if (m_hPetalCleaned[r][s])
        {
          m_hPetalCleaned[r][s]->SetDirectory(nullptr);
          safe_write_object(m_hPetalCleaned[r][s]);
        }
        else
        {
          std::cout << "ERROR: Failed to clone unmasked histogram for " << run_names[r] << " " << side_names[s] << std::endl;
          return Fun4AllReturnCodes::ABORTEVENT;
        }
        m_lamination_radial_gap_centers[r][s].clear();
        continue;
      }

      delete m_hPetalCleaned[r][s];
      auto cleaning = clean_laminations(m_hPetal[r][s]);
      m_hPetalCleaned[r][s] = cleaning.first;
      m_lamination_radial_gap_centers[r][s] = std::move(cleaning.second);

      if (!m_hPetalCleaned[r][s])
      {
        std::cout << "ERROR: Failed to build cleaned histogram for " << run_names[r] << " " << side_names[s] << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
      }
      safe_write_object(m_hPetalCleaned[r][s]);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

#include <cmath>
#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include <vector>
#include <stdexcept>
#include "TMatrixD.h"


namespace {

TMatrixD BuildSystematicCorrelationMatrix(const std::vector<double>& baseline,
                                            const std::vector<double>& syst,
                                            bool systIsRelative = true,
                                            bool fullyCorrelated = true)
{
      const int n = static_cast<int>(baseline.size());
      if (n == 0 || static_cast<int>(syst.size()) != n) {
          throw std::runtime_error("Input vectors must have same non-zero size.");
      }

      std::vector<double> delta(n, 0.0); // absolute systematic shift per bin
      for (int i = 0; i < n; ++i) {
          delta[i] = systIsRelative ? baseline[i] * syst[i] : syst[i];
      }

      TMatrixD cov(n, n);
      cov.Zero();

      if (fullyCorrelated) {
          // One systematic source shared across bins: Cov_ij = delta_i *delta_j
          for (int i = 0; i < n; ++i)
              for (int j = 0; j < n; ++j)
                  cov(i, j) = delta[i] * delta[j];
      } else {
          // Independent bin-by-bin systematic nuisances
          for (int i = 0; i < n; ++i)
              cov(i, i) = delta[i] * delta[i];
      }

      TMatrixD corr(n, n);
      corr.Zero();

      for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
              const double denom = std::sqrt(cov(i, i) * cov(j, j));
              corr(i, j) = (denom > 0.0) ? cov(i, j) / denom : 0.0;
          }
      }

      return corr;
  }


bool HaveMatchingBinning(const TH1F* a, const TH1F* b)
{
  if (!a || !b) {
    return false;
  }

  if (a->GetNbinsX() != b->GetNbinsX()) {
    return false;
  }

  const int nBins = a->GetNbinsX();
  for (int i = 1; i <= nBins + 1; ++i) {
    if (std::fabs(a->GetXaxis()->GetBinLowEdge(i) - b->GetXaxis()->GetBinLowEdge(i)) > 1e-12) {
      return false;
    }
  }

  return true;
}

void FillRatio(const TH1F* numerator, const TH1F* denominator, TH1F* ratio)
{
  const int nBins = ratio->GetNbinsX();
  for (int i = 1; i <= nBins; ++i) {
    const double n = numerator->GetBinContent(i);
    const double dn = numerator->GetBinError(i);
    const double d = denominator->GetBinContent(i);
    const double dd = denominator->GetBinError(i);

    if (d == 0.0 || n == 0.0) {
      ratio->SetBinContent(i, 0.0);
      ratio->SetBinError(i, 0.0);
      continue;
    }

    const double r = n / d;
    const double relErr2 = (dn / n) * (dn / n) + (dd / d) * (dd / d);
    ratio->SetBinContent(i, r);
    ratio->SetBinError(i, std::fabs(r) * std::sqrt(relErr2));
  }
}

}  // namespace

void SystematicDetermination(const char* inputFileName = "/sphenix/user/rosstom/analysis/LightFlavorRatios/systematics/geoAccEffSystematics.root",
                             const char* baselineParticle1HistName = "h_pT_base_KS0",
                             const char* baselineParticle2HistName = "h_pT_base_Xi",
                             const char* variationParticle1HistName = "h_pT_base_KS0_max",
                             const char* variationParticle2HistName = "h_pT_base_Xi_max",
                             const char* outputFileName = "SystematicDetermination_output.root")
{
  TFile* inputFile = TFile::Open(inputFileName, "READ");
  if (!inputFile || inputFile->IsZombie()) {
    std::cerr << "Error: could not open input file: " << inputFileName << std::endl;
    return;
  }

  TH1F* hBaseP1 = dynamic_cast<TH1F*>(inputFile->Get(baselineParticle1HistName));
  TH1F* hBaseP2 = dynamic_cast<TH1F*>(inputFile->Get(baselineParticle2HistName));
  TH1F* hVarP1 = dynamic_cast<TH1F*>(inputFile->Get(variationParticle1HistName));
  TH1F* hVarP2 = dynamic_cast<TH1F*>(inputFile->Get(variationParticle2HistName));

  if (!hBaseP1 || !hBaseP2 || !hVarP1 || !hVarP2) {
    std::cerr << "Error: one or more requested TH1F histograms were not found." << std::endl;
    inputFile->Close();
    return;
  }

  if (!HaveMatchingBinning(hBaseP1, hBaseP2) ||
      !HaveMatchingBinning(hBaseP1, hVarP1) ||
      !HaveMatchingBinning(hBaseP1, hVarP2)) {
    std::cerr << "Error: input histograms do not have matching binning." << std::endl;
    inputFile->Close();
    return;
  }

  TH1F* hRatioBaseline = dynamic_cast<TH1F*>(hBaseP2->Clone("hRatioBaseline"));
  TH1F* hRatioVariation = dynamic_cast<TH1F*>(hVarP2->Clone("hRatioVariation"));
  TH1F* hDifference = dynamic_cast<TH1F*>(hBaseP2->Clone("hDifference"));

  hRatioBaseline->Reset("ICES");
  hRatioVariation->Reset("ICES");
  hDifference->Reset("ICES");

  hRatioBaseline->SetTitle("Baseline ratio (particle2 / particle1)");
  hRatioVariation->SetTitle("Variation ratio (particle2 / particle1)");
  hDifference->SetTitle("Relative Difference: (variation - baseline) / baseline");

  FillRatio(hBaseP2, hBaseP1, hRatioBaseline);
  FillRatio(hVarP2, hVarP1, hRatioVariation);

  const int nBins = hRatioBaseline->GetNbinsX();
  for (int i = 1; i <= nBins; ++i) {
    const double b = hRatioBaseline->GetBinContent(i);
    const double vb = hRatioBaseline->GetBinError(i);
    const double v = hRatioVariation->GetBinContent(i);
    const double vv = hRatioVariation->GetBinError(i);

    if (b == 0.0) {
      hDifference->SetBinContent(i, 0.0);
      hDifference->SetBinError(i, 0.0);
      continue;
    }

    const double diff = (v - b)/b;
    //const double diffErr = std::sqrt((vv / b) * (vv / b) + (v * vb / (b * b)) * (v * vb / (b * b)));

    hDifference->SetBinContent(i, diff);
    //hDifference->SetBinError(i, diffErr);
  }

  TH2F* hCorrelationMatrix = new TH2F("hSystematicCorrelation", "Bin-to-bin correlation of systematic uncertainty", nBins, 0.5, nBins + 0.5, nBins, 0.5, nBins + 0.5);
  hCorrelationMatrix->GetXaxis()->SetTitle("Bin i");
  hCorrelationMatrix->GetYaxis()->SetTitle("Bin j");

  for (int i = 1; i <= nBins; ++i) {
    const double di = hDifference->GetBinContent(i);
    for (int j = 1; j <= nBins; ++j) {
      const double dj = hDifference->GetBinContent(j);

      double corr = 0.0;
      if (i == j) {
        corr = 1.0;
      }

      if (di != 0.0 && dj != 0.0) {
        corr = (di * dj) / (std::fabs(di) * std::fabs(dj));
      }

      hCorrelationMatrix->SetBinContent(i, j, corr);
    }
  }

  gStyle->SetOptStat(0);

  TCanvas* cRatioBaseline = new TCanvas("cRatioBaseline", "Baseline Ratio", 800, 600);
  hRatioBaseline->SetLineColor(kBlue + 1);
  hRatioBaseline->SetMarkerColor(kBlue + 1);
  hRatioBaseline->SetMarkerStyle(20);
  hRatioBaseline->Draw("E1");

  TCanvas* cRatioVariation = new TCanvas("cRatioVariation", "Variation Ratio", 800, 600);
  hRatioVariation->SetLineColor(kRed + 1);
  hRatioVariation->SetMarkerColor(kRed + 1);
  hRatioVariation->SetMarkerStyle(20);
  hRatioVariation->Draw("E1");

  TCanvas* cRelativeDifference = new TCanvas("cRelativeDifference", "Relative Difference", 800, 600);
  hDifference->SetLineColor(kBlack);
  hDifference->SetMarkerColor(kBlack);
  hDifference->SetMarkerStyle(21);
  hDifference->Draw("E1");

  TCanvas* cCorrelation = new TCanvas("cCorrelation", "Systematic Correlation Matrix", 800, 700);
  hCorrelationMatrix->Draw("COLZ TEXT");

  TFile* outputFile = TFile::Open(outputFileName, "RECREATE");
  if (!outputFile || outputFile->IsZombie()) {
    std::cerr << "Error: could not create output file: " << outputFileName << std::endl;
    inputFile->Close();
    return;
  }

  hRatioBaseline->Write();
  hRatioVariation->Write();
  hDifference->Write();
  hCorrelationMatrix->Write();

  cRatioBaseline->Write();
  cRatioVariation->Write();
  cRelativeDifference->Write();
  cCorrelation->Write();

  outputFile->Close();
  inputFile->Close();

  std::cout << "Wrote output to: " << outputFileName << std::endl;
}

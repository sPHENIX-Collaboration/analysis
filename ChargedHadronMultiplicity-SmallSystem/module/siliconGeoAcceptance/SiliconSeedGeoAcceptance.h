// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SILICONSEEDGEOACCEPTANCE_H
#define SILICONSEEDGEOACCEPTANCE_H

#include <fun4all/SubsysReco.h>

#include <memory>
#include <string>
#include <vector>

class ActsGeometry;
class PHCompositeNode;
class TFile;
class TH2D;

namespace Acts
{
  class Surface;
}

class SiliconSeedGeoAcceptance : public SubsysReco
{
 public:
  SiliconSeedGeoAcceptance(const std::string &name = "SiliconSeedGeoAcceptance");
  ~SiliconSeedGeoAcceptance() override = default;

  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void setOutputFile(const std::string &filename) { m_outputFilename = filename; }
  void setEtaRange(int nbins, float lo, float hi)
  {
    m_etaBins = nbins;
    m_etaLo = lo;
    m_etaHi = hi;
  }
  void setVtxZRange(int nbins, float lo, float hi)
  {
    m_vtxZBins = nbins;
    m_vtxZLo = lo;
    m_vtxZHi = hi;
  }
  void setRaysPerBin(int n) { m_raysPerBin = n; }
  void setRandomSeed(unsigned int seed) { m_randomSeed = seed; }
  void setBeamSpot(float x, float y)
  {
    m_beamSpotX = x;
    m_beamSpotY = y;
  }

 private:
  static constexpr int m_nMvtxLayerCounts = 4;
  static constexpr int m_nInttLayerCounts = 5;

  void countLayers(double eta, double phi, double vtxZ, int &nMvtx, int &nIntt) const;

  ActsGeometry *m_tGeometry = nullptr;
  TFile *m_outputFile = nullptr;
  TH2D *m_hA[m_nMvtxLayerCounts][m_nInttLayerCounts] = {};
  TH2D *m_hN = nullptr;

  std::vector<unsigned int> m_surfaceLayers;
  std::vector<double> m_surfacePhis;
  std::vector<std::shared_ptr<const Acts::Surface>> m_surfaces;

  std::string m_outputFilename = "geoacc.root";
  int m_etaBins = 80;
  float m_etaLo = -2.0;
  float m_etaHi = 2.0;
  int m_vtxZBins = 20;
  float m_vtxZLo = -10.0;
  float m_vtxZHi = 10.0;
  int m_raysPerBin = 10000;
  unsigned int m_randomSeed = 12345;
  float m_beamSpotX = 0.0;
  float m_beamSpotY = 0.0;
};

#endif  // SILICONSEEDGEOACCEPTANCE_H

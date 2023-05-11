// $Id: $

/*!
 * \file TPCMLDataInterface.h
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#ifndef TPCDATASTREAMEMULATOR_H_
#define TPCDATASTREAMEMULATOR_H_

#include <fun4all/SubsysReco.h>

#include <vector>

class PHCompositeNode;
class Fun4AllHistoManager;
class SvtxEvalStack;
class TH1;
class TH2;
namespace H5
{
class H5File;
}

/*!
 * \brief TPCMLDataInterface
 */
class TPCMLDataInterface : public SubsysReco
{
 public:
  TPCMLDataInterface(
      unsigned int minLayer,
      unsigned int m_maxLayer,
      const std::string &outputfilenamebase = "TPCMLDataInterface");
  virtual ~TPCMLDataInterface();

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  void maxLayer(int maxLayer)
  {
    m_maxLayer = maxLayer;
  }

  void minLayer(int minLayer)
  {
    m_minLayer = minLayer;
  }

  void outputFileNameBase(const std::string &outputFileNameBase)
  {
    m_outputFileNameBase = outputFileNameBase;
  }

  void saveDataStreamFile(bool saveDataStreamFile)
  {
    m_saveDataStreamFile = saveDataStreamFile;
  }

  double getEtaAcceptanceCut() const
  {
    return m_etaAcceptanceCut;
  }

  void setEtaAcceptanceCut(double etaAcceptanceCut)
  {
    m_etaAcceptanceCut = etaAcceptanceCut;
  }

  double getVertexZAcceptanceCut() const
  {
    return m_vertexZAcceptanceCut;
  }

  void setVertexZAcceptanceCut(double vertexZAcceptanceCut)
  {
    m_vertexZAcceptanceCut = vertexZAcceptanceCut;
  }


 private:
#ifndef __CINT__

  Fun4AllHistoManager *getHistoManager();
  int writeWavelet(int layer, int side, int phibin, int hittime, const std::vector<unsigned int> &wavelet);

  SvtxEvalStack *m_svtxevalstack;

  bool m_strict;
  bool m_saveDataStreamFile;

  std::string m_outputFileNameBase;
  H5::H5File *m_h5File;

  int m_minLayer;
  int m_maxLayer;

  int m_evtCounter;

  double m_vertexZAcceptanceCut;
  double m_etaAcceptanceCut;
  double m_momentumCut;
  bool m_use_initial_vertex = true;
  bool m_use_genfit_vertex = false;


  // histograms
  TH1 *m_hDataSize;
  TH1 *m_hWavelet;
  TH1 *m_hNChEta;
  TH2 *m_hLayerWaveletSize;
  TH2 *m_hLayerHit;
  TH2 *m_hLayerZBinHit;
  TH2 *m_hLayerZBinADC;
  TH2 *m_hLayerDataSize;
  TH2 *m_hLayerSumHit;
  TH2 *m_hLayerSumDataSize;

#endif  // #ifndef __CINT__
};

#endif /* TPCDATASTREAMEMULATOR_H_ */

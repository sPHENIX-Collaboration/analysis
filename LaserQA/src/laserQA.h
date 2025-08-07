
#ifndef LASERQA_H
#define LASERQA_H


#include <fun4all/SubsysReco.h>

#include <tpc/TpcDistortionCorrection.h>
#include <tpc/TpcDistortionCorrectionContainer.h>

#include <string>

class PHCompositeNode;
class LaserClusterContainer;
class EventHeader;

class TFile;
class TH1D;
class TH2D;

class laserQA : public SubsysReco
{
	
public:
  laserQA(const std::string &name = "laserQA");
  
  ~laserQA() override = default;
  
  void setOutfile(const std::string &outname)
  {
    m_output = outname;
  }
  
  int InitRun(PHCompositeNode *topNode) override;
  
  int process_event(PHCompositeNode *topNode) override;
  
  int End(PHCompositeNode *topNode) override;
  
private:

  TH1D *m_nLaserEvents{nullptr};
  
  TH2D *m_TPCWheel[2]{nullptr, nullptr};
  
  std::string m_output = "laserQA.root";
  
  TpcDistortionCorrection m_distortionCorrection;
  
  TpcDistortionCorrectionContainer *m_dcc_in_module_edge{nullptr};
  TpcDistortionCorrectionContainer *m_dcc_in_static{nullptr};
  
};

#endif

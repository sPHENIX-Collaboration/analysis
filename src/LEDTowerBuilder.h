// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef LEDTOWERBUILDER_H
#define LEDTOWERBUILDER_H

#include <fun4all/SubsysReco.h>


#include <calobase/RawTowerDefs.h>  // for HCALIN, HCALOUT, CEMC
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfov1.h>
#include <caloreco/CaloWaveformProcessing.h>


#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/packet.h>

#include <cassert>
#include <cmath>  // for NAN
#include <iostream>
#include <map>      // for _Rb_tree_const_iterator
#include <utility>  // for pair



#include <TRandom3.h>
#include <string>

class PHCompositeNode;
class TTree;

class LEDTowerBuilder : public SubsysReco
{
 public:

  LEDTowerBuilder(const std::string &name = "LEDTowerBuilder");
  ~LEDTowerBuilder() override;

  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;
  float calculateRMS(std::vector<std::vector<float>> waveforms);

  enum DetectorSystem
  {
    CEMC = 0,
    HCALIN = 1,
    HCALOUT = 2,
    EPD = 3
  };

  void set_detector_type(LEDTowerBuilder::DetectorSystem dettype)
  {
    m_dettype = dettype;
    return;
  }

  void set_nsamples(int _nsamples)
  {
    m_nsamples = _nsamples;
    return;
  }
  void set_dataflag(bool flag)
  {
    m_isdata = flag;
    return;
  }

  void set_diagnoistic(int diag)
  {
    m_diagnostic = diag;
    return;
  }
  
 private:
 
  CaloWaveformProcessing* WaveformProcessing = new CaloWaveformProcessing();
  LEDTowerBuilder::DetectorSystem m_dettype; 
  TowerInfoContainer *m_CaloInfoContainer; //! Calo info
  std::string m_detector;
  int m_packet_low;
  int m_packet_high;
  int m_nsamples;
  bool m_isdata;
  
  TTree *tOut;
  std::vector<float> m_adc;
  std::vector<float> m_ped;
  std::vector<float> m_time;
  std::vector<int> m_chan;
  std::vector<std::vector<float>> m_waveforms;
  std::string m_outputFilename;
  int m_diagnostic;
  
};

#endif // LEDTOWERBUILDER_H

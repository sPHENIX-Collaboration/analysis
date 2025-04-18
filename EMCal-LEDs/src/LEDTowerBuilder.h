//  -*- C++ -*-.
#ifndef LEDTOWERBUILDER_H
#define LEDTOWERBUILDER_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <caloreco/CaloWaveformProcessing.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <Event/Event.h>
#include <Event/packet.h>

#include <TTree.h>
#include <TFile.h>

#include <iostream>
#include <string>

class LEDTowerBuilder : public SubsysReco {
  public:

    LEDTowerBuilder(const std::string &name = "LEDTowerBuilder");
    ~LEDTowerBuilder() override;

    int InitRun(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;
    int End(PHCompositeNode *topNode) override;

    enum DetectorSystem {
    CEMC = 0,
    HCALIN = 1,
    HCALOUT = 2,
    EPD = 3
  };

    void set_detector_type(LEDTowerBuilder::DetectorSystem dettype) {
      m_dettype = dettype;
      return;
    }

    void set_nsamples(int _nsamples) {
      m_nsamples = _nsamples;
      return;
    }
    void set_dataflag(bool flag) {
      m_isdata = flag;
      return;
    }

  private:
 
    CaloWaveformProcessing* WaveformProcessing = new CaloWaveformProcessing();
    LEDTowerBuilder::DetectorSystem m_dettype;
    std::string m_detector;
    int m_packet_low;
    int m_packet_high;
    int m_nsamples;
    int m_nchannels;
    bool m_isdata;
    int m_event;

    TTree *tOut;
    std::vector<float> m_adc;
    std::vector<float> m_ped;
    std::vector<float> m_time;
    std::vector<int> m_chan;
    std::vector<std::vector<float>> m_waveforms;
    std::string m_outputFilename;
};

#endif // LEDTOWERBUILDER_H

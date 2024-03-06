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

    Int_t InitRun(PHCompositeNode *topNode) override;
    Int_t process_event(PHCompositeNode *topNode) override;
    Int_t End(PHCompositeNode *topNode) override;

    enum DetectorSystem {
      CEMC = 0,
      HCALIN = 1,
      HCALOUT = 2,
      EPD = 3
    };

    void set_output(const std::string &m_outputFilename) {
      this->m_outputFilename = m_outputFilename;
    }

    void set_skip(Int_t skip) {
      this->skip = skip;
    }

  private:
 
    CaloWaveformProcessing* WaveformProcessing = new CaloWaveformProcessing();
    LEDTowerBuilder::DetectorSystem m_dettype;
    std::string m_detector;
    Int_t m_packet_low;
    Int_t m_packet_high;
    Int_t m_nsamples;
    Int_t iEvent;
    Int_t skip;
  
    TTree *tOut;
    std::vector<Float_t> m_adc;
    std::vector<Float_t> m_ped;
    std::vector<Float_t> m_time;
    std::vector<Int_t> m_chan;
    std::vector<std::vector<Float_t>> m_waveforms;
    std::string m_outputFilename;
};

#endif // LEDTOWERBUILDER_H

#include "LEDTowerBuilder.h"
//____________________________________________________________________________..
LEDTowerBuilder::LEDTowerBuilder(const std::string &name):
  SubsysReco(name)
  , m_dettype(LEDTowerBuilder::CEMC)
  , m_detector("CEMC")
  , m_packet_low(6001) // 6001
  , m_packet_high(6128) // 6128
  , m_nsamples(32)
  , m_nchannels(192)
  , m_isdata(true)
  , m_event(0)
  , m_adc(0)
  , m_ped(0)
  , m_time(0)
  , m_chan(0)
  , m_waveforms(0)
  , m_outputFilename(name) {
  std::cout << "LEDTowerBuilder::LEDTowerBuilder(const std::string &name) Calling ctor" << std::endl;

  tOut = new TTree("W","W");

  tOut -> Branch("time",&m_time);
  tOut -> Branch("adc",&m_adc);
  tOut -> Branch("ped",&m_ped);
  tOut -> Branch("waveforms",&m_waveforms);
  tOut -> Branch("chan",&m_chan);
}

//____________________________________________________________________________..
LEDTowerBuilder::~LEDTowerBuilder() {
  std::cout << "LEDTowerBuilder::~LEDTowerBuilder() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int LEDTowerBuilder::InitRun(PHCompositeNode *topNode) {
  WaveformProcessing = new CaloWaveformProcessing();
  WaveformProcessing->set_processing_type(CaloWaveformProcessing::FAST);

  if (m_dettype == LEDTowerBuilder::CEMC) {
    m_detector = "CEMC";
    m_packet_low = 6001;
    m_packet_high = 6128;
    m_nchannels = 192;
    WaveformProcessing->set_template_file("testbeam_cemc_template.root");
  }
  else if (m_dettype == LEDTowerBuilder::HCALIN) {
    m_detector = "HCALIN";
    m_packet_low  = 7001;
    m_packet_high = 7008;
    m_nchannels = 192;
    WaveformProcessing->set_template_file("testbeam_ihcal_template.root");
  }
  else if (m_dettype == LEDTowerBuilder::HCALOUT) {
    m_detector = "HCALOUT";
    m_packet_low = 8001;
    m_packet_high = 8008;
    m_nchannels = 192;
    WaveformProcessing->set_template_file("testbeam_ohcal_template.root");
  }
  else if (m_dettype == LEDTowerBuilder::EPD) {
    m_detector = "EPD";
    m_packet_low = 9001;
    m_packet_high = 9016;
    WaveformProcessing->set_template_file("testbeam_cemc_template.root"); // place holder until we have EPD templates
  }
  WaveformProcessing->initialize_processing();
  //CreateNodeTree(topNode);
  topNode->print();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int LEDTowerBuilder::process_event(PHCompositeNode *topNode) {
  if(m_event % 100 == 0) std::cout << "Event: " << m_event << std::endl;
  ++m_event;

  if (m_isdata) {
    Event *_event = findNode::getClass<Event>(topNode, "PRDF");
    if (_event == 0) {
      std::cout << "CaloUnpackPRDF::Process_Event - Event not found" << std::endl;
      return -1;
    }
    // special event where we do not read out the calorimeters
    if ( _event->getEvtType() >= 8) {
      std::cout << "Event Type >= 8!!" << std::endl;
      return Fun4AllReturnCodes::DISCARDEVENT;
    }
    for (int pid = m_packet_low; pid <= m_packet_high; pid++) {
      // there are 192 channels in a packet
      // Determine the channel id offset
      UInt_t channel_offset = (pid-m_packet_low)*m_nchannels;
      Packet* packet = _event->getPacket(pid);
      if (!packet) {
        // std::cout << "No packet!!, pid: " << pid << std::endl;
        continue;
      }
      int nchannels = packet->iValue(0,"CHANNELS");

      // ensure that there are at most 192 channels in the packet
      // packet is corrupted if it reports too many channels
      if(nchannels > m_nchannels) {
        return Fun4AllReturnCodes::DISCARDEVENT;
      }

      for (int channel = 0; channel < nchannels; ++channel) {
        std::vector<float> waveform;
        waveform.reserve(m_nsamples);

        for (int samp = 0; samp < m_nsamples; samp++) {
          waveform.push_back(packet->iValue(samp,channel));
        }
        m_waveforms.push_back(waveform);
        m_chan.push_back(channel_offset+channel);
        waveform.clear();
      }
      delete packet;
    }
  }
  // placeholder for adding simulation
  else {
    std::cout << "m_isdata: false!!" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }
  std::vector<std::vector<float>> processed_waveforms =  WaveformProcessing->process_waveform(m_waveforms);

  int n_channels = processed_waveforms.size();
  for (int i = 0 ; i < n_channels; i++) {
    m_time.push_back(processed_waveforms.at(i).at(1));
    m_adc.push_back(processed_waveforms.at(i).at(0));
    m_ped.push_back(processed_waveforms.at(i).at(2));
  }

  tOut -> Fill();

  m_time.clear();
  m_adc.clear();
  m_ped.clear();
  m_chan.clear();
  m_waveforms.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

int LEDTowerBuilder::End(PHCompositeNode *topNode) {
  std::cout << "LEDTowerBuilder opening output file: " << m_outputFilename << std::endl;
  TFile output(m_outputFilename.c_str(),"recreate");
  output.cd();
  tOut -> Write();
  output.Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

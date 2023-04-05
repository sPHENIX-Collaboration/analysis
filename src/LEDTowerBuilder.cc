#include "LEDTowerBuilder.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>  // for SubsysReco
#include <fun4all/PHTFileServer.h>

// #include <calobase/RawTower.h>  // for RawTower
// #include <calobase/RawTowerContainer.h>
// #include <calobase/RawTowerDefs.h>  // for HCALIN, HCALOUT, CEMC

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>

#include <phool/PHCompositeNode.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/packet.h>

#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfov1.h>

#include <TTree.h>
#include <TGraph.h>
#include <TPad.h>
#include <TFile.h>
// #include <TRandom3.h>

//____________________________________________________________________________..
LEDTowerBuilder::LEDTowerBuilder(const std::string &name):
  SubsysReco(name)
  , m_dettype(LEDTowerBuilder::CEMC)
  , m_CaloInfoContainer(0)
  , m_detector("CEMC")
  , m_packet_low(6009) // 6001
  , m_packet_high(6016) // 6128
  , m_nsamples(32)
  , m_isdata(true)
  , m_adc(0)
  , m_ped(0)
  , m_time(0)
  , m_chan(0)
  , m_outputFilename(name)
  , m_diagnostic(1)
{
  std::cout << "LEDTowerBuilder::LEDTowerBuilder(const std::string &name) Calling ctor" << std::endl;

  tOut = new TTree("W","W");

  tOut -> Branch("time",&m_time);
  tOut -> Branch("adc",&m_adc);
  tOut -> Branch("ped",&m_ped);
  tOut -> Branch("chan",&m_chan);

}

//____________________________________________________________________________..
LEDTowerBuilder::~LEDTowerBuilder()
{
  std::cout << "LEDTowerBuilder::~LEDTowerBuilder() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int LEDTowerBuilder::InitRun(PHCompositeNode *topNode)
{
  // rnd = new TRandom3();

  WaveformProcessing = new CaloWaveformProcessing();
  WaveformProcessing->set_processing_type(CaloWaveformProcessing::FAST);

  if (m_dettype == LEDTowerBuilder::CEMC)
  {
    m_detector = "CEMC";
    m_packet_low = 6009; // 6001
    m_packet_high = 6016; // 6128
    // 6001, 60128
    WaveformProcessing->set_template_file("testbeam_cemc_template.root");
  }
  else if (m_dettype == LEDTowerBuilder::HCALIN)
  {
    m_packet_low  = 7001;
    m_packet_high = 7008;
    m_detector = "HCALIN";
    WaveformProcessing->set_template_file("testbeam_ihcal_template.root");
  }
  else if (m_dettype == LEDTowerBuilder::HCALOUT)
  {
    m_detector = "HCALOUT";
    m_packet_low = 8001;
    m_packet_high = 8008;
    WaveformProcessing->set_template_file("testbeam_ohcal_template.root");
  }
  else if (m_dettype == LEDTowerBuilder::EPD)
  {
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
int LEDTowerBuilder::process_event(PHCompositeNode *topNode)
{
  std::vector<std::vector<float>> waveforms;
  TGraph *gWaveForm = new TGraph();

  // std::cout << "m_isdata: " << m_isdata << std::endl;

  if (m_isdata)
  {
    Event *_event = findNode::getClass<Event>(topNode, "PRDF");
    if (_event == 0)
    {
      std::cout << "CaloUnpackPRDF::Process_Event - Event not found" << std::endl;
      return -1;
    }
    if ( _event->getEvtType() >= 8)/// special event where we do not read out the calorimeters
    {
      std::cout << "Event Type >= 8!!" << std::endl;
      return Fun4AllReturnCodes::DISCARDEVENT;
    }
    for ( int pid = m_packet_low; pid <= m_packet_high; pid++)
    {
      Packet *packet = _event->getPacket(pid);
      if (!packet)
      {
        std::cout << "No packet!!, pid: " << pid << std::endl;
        return Fun4AllReturnCodes::DISCARDEVENT;
      }
      for ( int channel = 0; channel <  packet->iValue(0,"CHANNELS"); channel++)
      {
        std::vector<float> waveform;

        for (int samp = 0; samp < m_nsamples;samp++)
        {
          waveform.push_back(packet->iValue(samp,channel));
          if(m_diagnostic) gWaveForm -> SetPoint(samp, samp, (packet->iValue(samp,channel)));
        }
	      if(m_diagnostic)
        {
          gWaveForm -> SetMarkerStyle(4);
          gWaveForm -> SetTitle(";sample number;");
          gWaveForm -> Draw("ap");
          gPad -> WaitPrimitive();
        }
        waveforms.push_back(waveform);
        waveform.clear();
      }
      delete packet;
    }
  }
  else // placeholder for adding simulation
  {
    std::cout << "m_isdata: false!!" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }
  std::vector<std::vector<float>> processed_waveforms =  WaveformProcessing->process_waveform(waveforms);

  int n_channels = processed_waveforms.size();
  for (int i = 0 ; i < n_channels;i++)
  {
    m_time.push_back(processed_waveforms.at(i).at(1));
    m_adc.push_back(processed_waveforms.at(i).at(0));
    m_ped.push_back(processed_waveforms.at(i).at(2));
    m_chan.push_back(i);
  }

  tOut -> Fill();

  waveforms.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

int LEDTowerBuilder::End(PHCompositeNode *topNode)
{
  std::cout << "LEDTowerBuilder opening output file: " << m_outputFilename << std::endl;
  // PHTFileServer::get().open(m_outputFilename,"RECREATE");
  TFile output(m_outputFilename.c_str(),"recreate");
  output.cd();
  tOut -> Write();
  output.Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

float LEDTowerBuilder::calculateRMS(std::vector<std::vector<float>> waveforms)
{

  float derp = 2.;
  return derp;

}

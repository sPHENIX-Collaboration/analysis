#include "TpcTimeFrameChecker.h"

#include <Event/oncsSubConstants.h>
#include <Event/packet.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/packet.h>

#include <fun4all/Fun4AllHistoManager.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>
#include <qautils/QAHistManagerDef.h>

#include <TAxis.h>
#include <TH1.h>
#include <TH2.h>
#include <TNamed.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>

#include <stdint.h>
#include <cassert>
#include <memory>
#include <string>

using namespace std;

TpcTimeFrameChecker::TpcTimeFrameChecker(const int packet_id)
  : 
  SubsysReco("TpcTimeFrameChecker" + to_string(packet_id)), 
  m_packet_id(packet_id),
  m_HistoPrefix("TpcTimeFrameBuilder_Packet" + to_string(packet_id))
{
  Fun4AllHistoManager *hm = QAHistManagerDef::getHistoManager();
  assert(hm);

  TH1 *h = new TH1D(TString(m_HistoPrefix.c_str()) + "_Normalization",  //
                    TString(m_HistoPrefix.c_str()) + " Normalization;Items;Count", 20, .5, 20.5);
  int i = 1;
  h->GetXaxis()->SetBinLabel(i++, "Packet");
  h->GetXaxis()->SetBinLabel(i++, "Lv1-Taggers");
  h->GetXaxis()->SetBinLabel(i++, "EnDat-Taggers");
  h->GetXaxis()->SetBinLabel(i++, "ChannelPackets");
  h->GetXaxis()->SetBinLabel(i++, "Waveforms");

  h->GetXaxis()->SetBinLabel(i++, "DMA_WORD_GTM");
  h->GetXaxis()->SetBinLabel(i++, "DMA_WORD_FEE");
  h->GetXaxis()->SetBinLabel(i++, "DMA_WORD_FEE_INVALID");
  h->GetXaxis()->SetBinLabel(i++, "DMA_WORD_INVALID");

  h->GetYaxis()->SetBinLabel(i++, "TimeFrameSizeLimitError");
  assert(i <= 20);
  h->GetXaxis()->LabelsOption("v");
  hm->registerHisto(h);

  h = new TH1I(TString(m_HistoPrefix.c_str()) + "_PacketLength",  //
               TString(m_HistoPrefix.c_str()) + " PacketLength;PacketLength [16bit Words];Count", 1000, .5, 5e6);
  hm->registerHisto(h);
}

TpcTimeFrameChecker::~TpcTimeFrameChecker()
{

}


//____________________________________________________________________________..
int TpcTimeFrameChecker::InitRun(PHCompositeNode *)
{
  
  if (m_debugTTreeFile.size())
  {
    m_file = TFile::Open(m_debugTTreeFile.c_str(), "recreate");
    assert(m_file->IsOpen());

    m_TaggerTree = new TTree("TaggerTree", "Each entry is one tagger for level 1 trigger or endat tag");
    
    m_TaggerTree->Branch("packet", &m_packet_id, "packet/I");
    m_TaggerTree->Branch("frame", &m_frame, "frame/I");
    m_TaggerTree->Branch("datastream_last_lvl1_count", &m_datastream_last_lvl1_count, 
      "datastream_last_lvl1_count/i");
    m_TaggerTree->Branch("datastream_last_endat_count", &  m_datastream_last_endat_count  ,
     "datastream_last_endat_count/i");
    m_TaggerTree->Branch("datastream_last_bco", &m_datastream_last_bco, "datastream_last_bco/l");
    m_TaggerTree->Branch("tagger", &m_payload);
  }


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TpcTimeFrameChecker::process_event(PHCompositeNode *topNode)
{
  Event *_event = findNode::getClass<Event>(topNode, "PRDF");
  if (_event == nullptr)
  {
    std::cout << "TpcTimeFrameChecker::Process_Event - Event not found" << std::endl;
    return -1;
  }
  if (_event->getEvtType() >= 8)  /// special events
  {
    return Fun4AllReturnCodes::DISCARDEVENT;
  }

  m_frame = _event->getEvtSequence();

  std::unique_ptr<Packet> p(_event->getPacket(m_packet_id));
  if (!p)
  {
    if (Verbosity())
    {
      std::cout << __PRETTY_FUNCTION__ << " : missing packet " << m_packet_id << std::endl;
    }
  }

  ProcessPacket(p.get());

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TpcTimeFrameChecker::End(PHCompositeNode * /*topNode*/)
{
  
  if (m_file)
  {
    m_file->Write();

    std::cout << __PRETTY_FUNCTION__ << " : completed saving to " << m_file->GetName() << std::endl;
    m_file->ls();

    m_file->Close();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}



int TpcTimeFrameChecker::ProcessPacket(Packet *packet)
{
  if (!packet)
  {
    cout << __PRETTY_FUNCTION__ << " Error : Invalid packet, doing nothing" << endl;
    assert(packet);
    return 0;
  }

  if (packet->getHitFormat() != IDTPCFEEV4)
  {
    cout << __PRETTY_FUNCTION__ << " Error : expect packet format " << IDTPCFEEV4
         << " but received packet format " << packet->getHitFormat() << ":" << endl;
    packet->identify();
    assert(packet->getHitFormat() == IDTPCFEEV4);
    return 0;
  }

  if (m_packet_id != packet->getIdentifier())
  {
    cout << __PRETTY_FUNCTION__ << " Error : mismatched packet with packet ID expectation of " << m_packet_id << ", but received";
    packet->identify();
    assert(m_packet_id == packet->getIdentifier());
    return 0;
  }

  if (Verbosity())
  {
    cout << __PRETTY_FUNCTION__ << " : received packet ";
    packet->identify();
  }

  Fun4AllHistoManager *hm = QAHistManagerDef::getHistoManager();
  assert(hm);
  TH1D *h_norm = dynamic_cast<TH1D *>(hm->getHisto(
      m_HistoPrefix + "_Normalization"));
  assert(h_norm);
  h_norm->Fill("Packet", 1);

  int l = packet->getDataLength() + 16;
  l *= 2;  // int to uint16
  vector<uint16_t> buffer(l);

  int l2 = 0;

  packet->fillIntArray(reinterpret_cast<int *>(buffer.data()), l, &l2, "DATA");
  l2 -= packet->getPadding();
  assert(l2 >= 0);
  unsigned int payload_length = 2 * (unsigned int) l2;  // int to uint16

  TH1I *h_PacketLength = dynamic_cast<TH1I *>(hm->getHisto(
      m_HistoPrefix + "_PacketLength"));
  assert(h_PacketLength);
  h_PacketLength->Fill(payload_length);

  // demultiplexer
  unsigned int index = 0;
  while (index < payload_length)
  {
    if ((buffer[index] & 0xFF00) == FEE_MAGIC_KEY)
    {
      unsigned int fee_id = buffer[index] & 0xff;

      if (Verbosity())
      {
        cout << __PRETTY_FUNCTION__ << " : buffer["<< index <<"] received FEE data from FEE " << fee_id <<". payload_length = "<<payload_length<< endl;
      }

      ++index;
      if (fee_id < MAX_FEECOUNT)
      {
        for (unsigned int i = 0; i < DAM_DMA_WORD_BYTE_LENGTH - 1; i++)
        {
          // watch out for any data corruption
          if (index >= payload_length)
          {
            cout << __PRETTY_FUNCTION__ << " : Error : unexpected reach at payload length at position " << index << endl;
            break;
          }

          if (Verbosity())
          {
            cout << __PRETTY_FUNCTION__ << " : buffer[" << index << "] -> " <<hex<<buffer[index]<<dec << endl;
          }

          ++index;
        }
        h_norm->Fill("DMA_WORD_FEE", 1);
      }
      else
      {
        cout << __PRETTY_FUNCTION__ << " : Error : Invalid FEE ID " << fee_id << " at position " << index << endl;
        index += DAM_DMA_WORD_BYTE_LENGTH - 1;
        h_norm->Fill("DMA_WORD_FEE_INVALID", 1);
      }
    }
    else if ((buffer[index] & 0xFF00) == GTM_MAGIC_KEY)
    {
      if (Verbosity())
      {
        cout << __PRETTY_FUNCTION__ << " : buffer["<< index <<"] received GTM data "
         << hex << buffer[index]<<dec 
         <<". payload_length = "<<payload_length << endl;
      }

      // watch out for any data corruption
      if (index + DAM_DMA_WORD_BYTE_LENGTH > payload_length)
      {
        cout << __PRETTY_FUNCTION__ << " : Error : unexpected reach at payload length at position "
         << index << endl;
        break;
      }
      if (decode_gtm_data(&buffer[index]) == 0)
      {
        process_gtm_data(m_payload);

        // record the last LV1 readings
        if (m_payload.is_lvl1)
        {
          m_datastream_last_lvl1_count = m_payload.lvl1_count;
          m_datastream_last_endat_count = m_payload.endat_count;
          m_datastream_last_bco = m_payload.bco;
        }
      }      

      index += DAM_DMA_WORD_BYTE_LENGTH;
      h_norm->Fill("DMA_WORD_GTM", 1);
    }
    else
    {
      cout << __PRETTY_FUNCTION__ << " : Error : Unknown data type at position " 
      << index << ": " << hex << buffer[index] << dec << endl;
      // not FEE data, e.g. GTM data or other stream, to be decoded
      index += DAM_DMA_WORD_BYTE_LENGTH;
      h_norm->Fill("DMA_WORD_INVALID", 1);
    }
  }


  return Fun4AllReturnCodes::EVENT_OK;
}

int TpcTimeFrameChecker::process_gtm_data(const gtm_payload & payload)
{
  if (Verbosity())
  {
    cout << __PRETTY_FUNCTION__ << " : received GTM data: " << endl;
    cout << "  pkt_type: " << hex << payload.pkt_type << dec << endl;
    cout << "  is_endat: " << payload.is_endat << endl;
    cout << "  is_lvl1: " << payload.is_lvl1 << endl;
    cout << "  is_modebit: " << payload.is_modebit << endl;
    cout << "  bco: " << hex << payload.bco << dec << endl;
    cout << "  lvl1_count: " << payload.lvl1_count << endl;
    cout << "  endat_count: " << payload.endat_count << endl;
    cout << "  last_bco: " << hex << payload.last_bco << dec << endl;
    cout << "  modebits: " << hex << (unsigned int) payload.modebits << dec << endl;
    cout << "  userbits: " << hex << (unsigned int) payload.userbits << dec << endl;
  }

  if (m_TaggerTree)
  {
    m_TaggerTree->Fill();
  }

  return 0;
}

int TpcTimeFrameChecker::decode_gtm_data(uint16_t dat[16])
{
  unsigned char *gtm = reinterpret_cast<unsigned char *>(dat);


  m_payload.pkt_type = gtm[0] | ((uint16_t)gtm[1] << 8);
//    if (m_payload.pkt_type != GTM_LVL1_ACCEPT_MAGIC_KEY && m_payload.pkt_type != GTM_ENDAT_MAGIC_KEY)
  if (m_payload.pkt_type != GTM_LVL1_ACCEPT_MAGIC_KEY && m_payload.pkt_type != GTM_ENDAT_MAGIC_KEY && m_payload.pkt_type != GTM_MODEBIT_MAGIC_KEY)
    {
      return -1;
    }

  m_payload.is_lvl1 = m_payload.pkt_type == GTM_LVL1_ACCEPT_MAGIC_KEY;
  m_payload.is_endat = m_payload.pkt_type == GTM_ENDAT_MAGIC_KEY;
  m_payload.is_modebit = m_payload.pkt_type == GTM_MODEBIT_MAGIC_KEY;

  m_payload.bco = ((unsigned long long)gtm[2] << 0)
    | ((unsigned long long)gtm[3] << 8)
    | ((unsigned long long)gtm[4] << 16)
    | ((unsigned long long)gtm[5] << 24)
    | ((unsigned long long)gtm[6] << 32)
    | (((unsigned long long)gtm[7]) << 40);
  m_payload.lvl1_count = ((unsigned int)gtm[8] << 0)
    | ((unsigned int)gtm[9] << 8)
    | ((unsigned int)gtm[10] << 16)
    | ((unsigned int)gtm[11] << 24);
  m_payload.endat_count = ((unsigned int)gtm[12] << 0)
    | ((unsigned int)gtm[13] << 8)
    | ((unsigned int)gtm[14] << 16)
    | ((unsigned int)gtm[15] << 24);
  m_payload.last_bco = ((unsigned long long)gtm[16] << 0)
    | ((unsigned long long)gtm[17] << 8)
    | ((unsigned long long)gtm[18] << 16)
    | ((unsigned long long)gtm[19] << 24)
    | ((unsigned long long)gtm[20] << 32)
    | (((unsigned long long)gtm[21]) << 40);
  m_payload.modebits = gtm[22];
  m_payload.userbits = gtm[23];



  return 0;
}


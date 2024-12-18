#ifndef __TpcTimeFrameBuilder_H__
#define __TpcTimeFrameBuilder_H__

#include <algorithm>
#include <cstdint>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <limits>

#include <fun4all/SubsysReco.h>

class Packet;
class TTree;
class TFile;

class TpcTimeFrameChecker: public SubsysReco
{
 public:
  explicit TpcTimeFrameChecker(const int packet_id);
  virtual ~TpcTimeFrameChecker();

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
  */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
  */
  int process_event(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  
  // Getter and Setter for m_debugTTree
  std::string getDebugTTreeFile() const { return m_debugTTreeFile; }
  void setDebugTTreeFile(const std::string& debugTTree) { m_debugTTreeFile = debugTTree; }
  

  int ProcessPacket(Packet *);

  void setVerbosity(const int i) { m_verbosity = i; }

 protected:

  std::string m_debugTTreeFile;
  TFile *m_file = nullptr;
  TTree *m_TaggerTree = nullptr;

  // Length for the 256-bit wide Round Robin Multiplexer for the data stream
  static const size_t DAM_DMA_WORD_BYTE_LENGTH = 16;

  static const unsigned short  MAGIC_KEY_0 = 0xfe;
//  static const unsigned short  MAGIC_KEY_1 = 0x00;
  static const unsigned short  MAGIC_KEY_1 = 0xed;
  static const uint16_t FEE_MAGIC_KEY = 0xba00;
  static const uint16_t GTM_MAGIC_KEY = 0xbb00;
  static const uint16_t GTM_LVL1_ACCEPT_MAGIC_KEY = 0xbbf0;
  static const uint16_t GTM_ENDAT_MAGIC_KEY = 0xbbf1;
  static const unsigned short GTM_MODEBIT_MAGIC_KEY = 0xbbf2;

  static const uint16_t MAX_FEECOUNT = 26;      // that many FEEs
  static const uint16_t MAX_CHANNELS = 8 * 32;  // that many channels per FEE
  static const uint16_t HEADER_LENGTH = 5;

  int decode_gtm_data(uint16_t gtm[DAM_DMA_WORD_BYTE_LENGTH]);

  struct gtm_payload {
      unsigned short pkt_type = 0;
      bool is_endat = false;
      bool is_lvl1 = false;
      bool is_modebit = false;
      unsigned long long bco = 0;
      unsigned int lvl1_count = 0;
      unsigned int endat_count = 0;
      unsigned long long last_bco = 0;
      unsigned char modebits = 0;
      unsigned char userbits = 0;
  };
  gtm_payload m_payload;
  int m_frame = 0;


  int m_verbosity = 0;
  int m_packet_id = 0;

  //! common prefix for QA histograms
  std::string m_HistoPrefix;

};

#endif

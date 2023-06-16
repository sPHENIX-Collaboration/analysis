// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TPCRawDataTree_H
#define TPCRawDataTree_H

#include <fun4all/SubsysReco.h>

#include <stdint.h>
#include <string>
#include <vector>

class PHCompositeNode;
class TFile;
class TTree;
class TH1F;
class TH2F;

class TPCRawDataTree : public SubsysReco
{
 public:
  explicit TPCRawDataTree(const std::string &fname = "TPCRawDataTree.root");

  ~TPCRawDataTree() override {}

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

  void AddPacket(int packet)
  {
    m_packets.push_back(packet);
  }

 protected:
  //! which packet to decode
  std::vector<int> m_packets;

 private:
  std::string m_fname;
  TFile *m_file = nullptr;
  TTree *m_SampleTree = nullptr;
  TTree *m_PacketTree = nullptr;
  TTree *m_TaggerTree = nullptr;
  TH1F *R1_hist = nullptr;
  TH1F *R2_hist = nullptr;
  TH1F *R3_hist = nullptr;
  TH1F *TotalFEE = nullptr;
  TH1F *TotalFEEsampa = nullptr;
  TH1F *TotalFRAME = nullptr;
  TH1F *checksumError_fee = nullptr;
  TH1F *checksumError_feesampa = nullptr;
  TH1F *checksumError_frame = nullptr;
  TH2F *R1_time = nullptr;
  TH2F *R2_time = nullptr;
  TH2F *R3_time = nullptr;


  int m_packet = 0;
  int m_frame = 0;
  int m_nWaveormInFrame = 0;
  int m_maxFEECount = 0;
  int m_nSamples = 0;
  int m_fee = 0;
  int m_sampaAddress = 0;
  int m_sampaChannel = 0;
  int m_Channel = 0;
  int m_BCO = 0;
  int m_checksum = 0;
  int m_checksumError = 0;

  int m_nTaggerInFrame = 0;
  uint16_t m_tagger_type = 0;
  uint8_t m_is_endat = 0;
  uint8_t m_is_lvl1 = 0;
  uint64_t m_bco = 0;
  uint32_t m_lvl1_count = 0;
  uint32_t m_endat_count = 0;
  uint64_t m_last_bco = 0;
  uint8_t m_modebits = 0;


  uint64_t m_last_lvl1_bco = 0;

  std::vector<unsigned short> m_adcSamples;
};

#endif  // TPCRawDataTree_H

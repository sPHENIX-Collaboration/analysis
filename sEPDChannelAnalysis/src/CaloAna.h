#ifndef CALOANA_H__
#define CALOANA_H__

#include <fun4all/SubsysReco.h>
#include <vector>
// Forward declarations
class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TNtuple;
class TTree;
class TH2F;

class CaloAna : public SubsysReco
{
 public:
  //! constructor
  CaloAna(const std::string &name = "CaloAna", const std::string &fname = "MyNtuple.root", float zero_sup = 2);

  //! destructor
  virtual ~CaloAna();

  //! full initialization
  int Init(PHCompositeNode *);

  //! event processing method
  int process_event(PHCompositeNode *);

  //! end of run method
  int End(PHCompositeNode *);

  int process_g4hits(PHCompositeNode *);
  int process_g4cells(PHCompositeNode *);
  int process_towers(PHCompositeNode *);
  int process_clusters(PHCompositeNode *);


  void Detector(const std::string &name) { detector = name; }

 protected:
  std::string detector;
  std::string m_outputFileName;
	std::string m_histoFileName;
  Fun4AllHistoManager *hm = nullptr;
  TFile *outfile = nullptr;
  TNtuple *g4hitntuple = nullptr;
  TNtuple *g4cellntuple = nullptr;
  TTree *towerntuple = nullptr;
  TNtuple *clusterntuple = nullptr;
  std::vector<float> m_energy;
  std::vector<int> m_etabin;
  std::vector<int> m_phibin;
  std::vector<float> m_waveform[31];

	float min_energy;
	int num_channels;
	int m_event;
  /* std::vector<float> m_waveform_0; */
  /* std::vector<float> m_waveform_1; */
  /* std::vector<float> m_waveform_2; */
  /* std::vector<float> m_waveform_3; */
  /* std::vector<float> m_waveform_4; */
  /* std::vector<float> m_waveform_5; */
  /* std::vector<float> m_waveform_6; */
  /* std::vector<float> m_waveform_7; */
  /* std::vector<float> m_waveform_8; */
  /* std::vector<float> m_waveform_9; */
  /* std::vector<float> m_waveform_10; */
  /* std::vector<float> m_waveform_11; */
  /* std::vector<float> m_waveform_12; */
  /* std::vector<float> m_waveform_13; */
  /* std::vector<float> m_waveform_14; */
  /* std::vector<float> m_waveform_15; */

};

#endif

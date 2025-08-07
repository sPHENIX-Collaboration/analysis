#ifndef UPCTRIGSTUDY_H
#define UPCTRIGSTUDY_H

#include <Rtypes.h>
#include <fun4all/SubsysReco.h>

/// Class declarations for use in the analysis module
class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TTree;
class TH1;
class TH2;
class PHCompositeNode;
class GlobalVertex;
class CaloTriggerInfo;
class EventHeader;
class Gl1Packet;
class MbdOut;
class TowerInfoContainer;

/// Definition of this analysis module class
class UPCTrigStudy : public SubsysReco
{
 public:
  /// Constructor
  UPCTrigStudy(const std::string &name = "UPCTrigStudy",
               const std::string &outputfname = "UPCTrigStudy.root");

  // Destructor
  virtual ~UPCTrigStudy();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *) override;

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *) override;

  /// SubsysReco end processing method
  int End(PHCompositeNode *) override;

  /// Set the minimum cluster pT to cut on
  //void setMinClusPt(float mincluspt) { m_mincluspt = mincluspt; }

 private:
  /// String to contain the outfile name containing the trees
  std::string m_outfilename;

  /// Fun4All Histogram Manager tool
  Fun4AllHistoManager *m_hm;

  /// Set things to analyze
  int process_triggers();
  void process_zdc();
  int Getpeaktime(TH1 *);

  /// TFile to hold the following TTrees and histograms
  TFile *m_outfile{nullptr};
  TTree *_globaltree{nullptr};
 
  TH1 *h_trig{nullptr};
  TH1 *h_ntracks{nullptr};
  TH1 *h_zdcse{nullptr};
  TH1 *h_zdcne{nullptr};
  TH1 *h_zdce{nullptr};
  TH1 *h_zdctimecut{nullptr};

  Gl1Packet* _gl1raw {nullptr};
  MbdOut *_mbdout{nullptr};
  TowerInfoContainer* _zdctowers{nullptr};

  /// Methods for grabbing the data
  int GetNodes(PHCompositeNode *topNode);
  //void getEMCalClusters(PHCompositeNode *topNode);

  /// Data
  EventHeader *evthdr{nullptr};

  void resetVariables();
  void initializeTrees();

  /**
   * Make variables for the relevant trees
   */

  // Global Info
  Int_t f_run{ 0 };
  Int_t f_evt{ 0 };
  ULong64_t f_rtrig{ 0 };
  ULong64_t f_ltrig{ 0 };
  ULong64_t f_strig{ 0 };
  Float_t f_zdcse{ 0 };
  Float_t f_zdcne{ 0 };
  Short_t f_cross{ 0 };
  Short_t f_mbdsn{ 0 };
  Short_t f_mbdnn{ 0 };

  Int_t f_ntracks{ 0 };
  Int_t _mbdwide{ 0 };
  Int_t _nprocessed{ 0 };

  /// Cluster variables
  /*
  double m_clusenergy;
  double m_cluseta;
  double m_clustheta;
  double m_cluspt;
  double m_clusphi;
  double m_cluspx;
  double m_cluspy;
  double m_cluspz;
  double m_E_4x4;
  */
};

#endif  // UPCTRIGSTUDY_H

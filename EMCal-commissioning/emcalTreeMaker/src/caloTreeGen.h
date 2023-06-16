// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef FUNKYCALOSTUFF_H
#define FUNKYCALOSTUFF_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TTree.h>

class TTree;
class PHCompositeNode;
class Fun4AllHistoManager;
class TFile;
class RawCluster;
class TowerInfoContainer;

class caloTreeGen : public SubsysReco
{
 public:

  caloTreeGen(const std::string &name = "caloTreeGen");

  ~caloTreeGen() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

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

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;
  
  void setOHCal(int doCal) {doOHCal = doCal;}
  
  void setIHCal(int doCal) {doIHCal = doCal;}

  void setClusters(int clusters)  {doClusters = clusters;}
 private:

  TTree *T;
  
  std::vector<float> m_emcTowPhi;
  std::vector<float> m_emcTowEta;
  std::vector<float> m_emcTowE;
  std::vector<float> m_emcTiming;
  std::vector<float> m_emciEta;
  std::vector<float> m_emciPhi;
  
  std::vector<float> m_ohcTowPhi;
  std::vector<float> m_ohcTowEta;
  std::vector<float> m_ohcTowE;
  std::vector<float> m_ohcTiming;
  std::vector<float> m_ohciEta;
  std::vector<float> m_ohciPhi;
  
  std::vector<float> m_ihcTowPhi;
  std::vector<float> m_ihcTowEta;
  std::vector<float> m_ihcTowE;
  std::vector<float> m_ihcTiming;
  std::vector<float> m_ihciEta;
  std::vector<float> m_ihciPhi;

  std::vector<float> m_clusterE;
  std::vector<float> m_clusterPhi;
  std::vector<float> m_clusterEta;
  std::vector<float> m_clusterPt;
  std::vector<float> m_clusterChi;
  std::vector<float> m_clusterNtow;
  std::vector<float> m_clusterTowMax;
  std::vector<float> m_clusterECore;

  TFile *out;
  //Fun4AllHistoManager *hm = nullptr;
  std::string Outfile = "commissioning.root";

  unsigned int getCaloTowerPhiBin(const unsigned int key);
  unsigned int getCaloTowerEtaBin(const unsigned int key);
  float getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer);
  int doOHCal;
  int doIHCal;
  int doClusters;
  float totalCaloE;

};

#endif // FUNKYCALOSTUFF_H

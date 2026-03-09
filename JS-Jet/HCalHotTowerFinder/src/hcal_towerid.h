// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef HCAL_TOWERID_H
#define HCAL_TOWERID_H

#include <fun4all/SubsysReco.h>
//#include <cdbobjects/CDBTTree.h>

#include <string>
#include <vector>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>

class TTree;
class PHCompositeNode;
class Fun4AllHistoManager;
class TFile;
class RawCluster;
class TowerInfoContainer;


const int nTowers = 1536;

class hcal_towerid : public SubsysReco
{
 public:

  hcal_towerid(const std::string &name = "hcal_towerid",const std::string &Outfile = "towerid.root", const std::string &icdbtreename = "test.root", const std::string &ocdbtreename = "test2.root", float adccut_i = 250, float adccut_o = 250, float sigmas_lo = 1, float sigmas_hi = 4.5, float inner_f = 0.03, float outer_f = 0.03);

  ~hcal_towerid() override;

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
  void FillHistograms(const int runnumber, const int segment);

  void CalculateCutOffs(const int runnumber);

  void WriteCDBTree(const int runnumber);

 private:

  TTree *Tinner;
  TTree *Touter;
  TFile *out;

  //CDBTTree *cdbttree;
 
  TFile*fchannels;
  TTree *channels;

  //Fun4AllHistoManager *hm = nullptr;
  std::string Outfile = "commissioning.root"; 
  TH1F* hEventCounter = NULL;
  
  //  TH2F* Fspec = NULL;
  TH2F* Fspec_i = NULL;
  TH2F* Fspec_o = NULL;

  //TH2F* Fspeci = NULL;
  TH2F* Fspeci_i = NULL;
  TH2F* Fspeci_o = NULL;

  //TH2F* Espec = NULL;
  TH2F* Espec_i = NULL; 
  TH2F* Espec_o = NULL; 
  
  TH1F *iHCal_hist = NULL;
  TH1F *oHCal_hist = NULL;


  const std::string icdbtreename;
  const std::string ocdbtreename; 	
  float adccut_i;
  float adccut_o;
  float sigmas_lo;
  float sigmas_hi;
  float inner_f;
  float outer_f;
	
  int hot_channels_i;
  int hot_channels_o;
  float itowerF[nTowers] = {0};
  float otowerF[nTowers] = {0};

  float itowerE[nTowers] = {0};
  float otowerE[nTowers] = {0};

  int ihottowers[nTowers] = {0};
  int ohottowers[nTowers] = {0};

  int ideadtowers[nTowers] = {0};
  int odeadtowers[nTowers] = {0};
	
  int icoldtowers[nTowers] = {0};
  int ocoldtowers[nTowers] = {0};

  int hot_regions = 0;
  int cold_regions = 0; 

int goodevents = 0;
};

#endif 

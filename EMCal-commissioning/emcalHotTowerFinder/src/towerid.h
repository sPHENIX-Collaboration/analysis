// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TOWERID_H
#define TOWERID_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TTree.h>
#include <TH1F.h>

class TTree;
class PHCompositeNode;
class Fun4AllHistoManager;
class TFile;
class RawCluster;
class TowerInfoContainer;

class towerid : public SubsysReco
{
 public:

  towerid(const std::string &name = "towerid.root", float adccut = 250, float sigmas = 4.5, int event_end = 0);

  ~towerid() override;

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
  
 private:

  TTree *T;

  TFile *out;
  //Fun4AllHistoManager *hm = nullptr;
  std::string Outfile = "commissioning.root";

  TH1F*Fspec = new TH1F("Fspec","Fspec",0,1,1);

  float adccut;
  float sigmas;
  int event_end;

	
  int m_hot_channels;
  float std,mode,cutoffFreq;
    int binmax;
  int towerF[24576] = {0};

};

#endif 

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

class hcal_towerid : public SubsysReco
{
 public:

  hcal_towerid(const std::string &name = "towerid.root", const std::string &cdbtreename_i = "test.root",const std::string &cdbtreename_o = "test2.root", float adccut_i = 250, float adccut_o = 250, float sigmas_lo = 1, float sigmas_hi = 4.5, float inner_f = 0.03, float outer_f = 0.03);

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
  

 private:

  TTree *T;
  TTree *T2;
  TFile *out;

//  CDBTTree *cdbttree;
 
  //Fun4AllHistoManager *hm = nullptr;
  std::string Outfile = "commissioning.root";

  TH1F*Fspeci_i = new TH1F("Fspeci_i","Fspeci_i",0,1,1);
  TH1F*Fspeci_o = new TH1F("Fspeci_o","Fspeci_o",0,1,1);

  TH1F*Fspec_i = new TH1F("Espec_i","Espec_i",0,1,1);
  TH1F*Fspec_o = new TH1F("Fspec_i","Fspec_i",0,1,1);

  TH1F*Espec_i = new TH1F("Espec_i","Espec_i",0,1,1);
  TH1F*Espec_o = new TH1F("Espec_o","Espec_o",0,1,1);

  const std::string cdbtreename_i;
  const std::string cdbtreename_o; 	
  float adccut_i;
  float adccut_o;
  float sigmas_lo;
  float sigmas_hi;
  float inner_f;
  float outer_f;
	
  int m_hot_channels_i;
  int m_hot_channels_o;
  int itowerF[1536] = {0};
  int otowerF[1536] = {0};

  float itowerE[1536] = {0};
  float otowerE[1536] = {0};

  int ihottowers[1536] = {0};
  int ohottowers[1536] = {0};
  int ideadtowers[1536] = {0};
  int odeadtowers[1536] = {0};
	
  int icoldtowers[1536] = {0};
  int ocoldtowers[1536] = {0};

int goodevents = 0;
};

#endif 

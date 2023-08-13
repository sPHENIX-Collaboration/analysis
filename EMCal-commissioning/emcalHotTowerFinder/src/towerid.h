// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TOWERID_H
#define TOWERID_H

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

class towerid : public SubsysReco
{
 public:

  towerid(const std::string &name = "towerid.root", const std::string &cdbtreename = "test.root", float adccut_sg = 250,float adccut_k = 500, float sigmas_lo = 1, float sigmas_hi = 4.5, float SG_f = 0.0002, float Kur_f = 0.0002, float region_f = 0.03);

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

//  CDBTTree *cdbttree;
 
  TFile*fchannels;
  TTree *channels;

  int fiber_type = 0;
 
  //Fun4AllHistoManager *hm = nullptr;
  std::string Outfile = "commissioning.root";

  TH1F*Fspec = new TH1F("Fspec","Fspec",0,1,1);
  TH1F*Fspec_SG = new TH1F("Fspec_SG","Fspec_SG",0,1,1);
  TH1F*Fspec_K = new TH1F("Fspec_K","Fspec_K",0,1,1);
  TH1F*Fspec_sector = new TH1F("Fspec_sector","Fspec_sector",0,1,1);
  TH1F*Fspec_IB = new TH1F("Fspec_IB","Fspec_IB",0,1,1);
 
   TH1F*Fspeci = new TH1F("Fspec_init","Fspec_init",0,1,1);
  TH1F*Fspeci_SG = new TH1F("Fspec_SG_init","Fspec_SG_init",0,1,1);
  TH1F*Fspeci_K = new TH1F("Fspec_K_init","Fspec_K_init",0,1,1);
  TH1F*Fspeci_sector = new TH1F("Fspec_sector_init","Fspec_sector_init",0,1,1);
  TH1F*Fspeci_IB = new TH1F("Fspec_IB_init","Fspec_IB_init",0,1,1);

 
  TH1F*Espec = new TH1F("Espec","Espec",0,1,1);
  TH1F*Espec_SG = new TH1F("Espec_SG","Espec_SG",0,1,1);
  TH1F*Espec_K = new TH1F("Espec_K","Espec_K",0,1,1);
  TH1F*Espec_sector = new TH1F("Espec_sector","Espec_sector",0,1,1);
  TH1F*Espec_IB = new TH1F("Espec_IB","Espec_IB",0,1,1);

  const std::string cdbtreename; 	
  float adccut_sg;
  float adccut_k;
  float sigmas_lo;
  float sigmas_hi;
  float SG_f;
  float Kur_f;
  float region_f;
	
  int m_hot_channels;
  int towerF[24576] = {0};
  int sectorF[64] = {0};
  int ibF[384] = {0};

  float towerE[24576] = {0};
  float sectorE[64] = {0};
  float ibE[384] = {0};

  int hottowers[24576] = {0};
  int hotIB[384] = {0};
  int hotsectors[64] = {0};
  int deadtowers[24576] = {0};
	
  int coldtowers[24576] = {0};
  int coldIB[384] = {0};
  int coldsectors[64] = {0};
  int hot_regions = 0;
  int cold_regions = 0;  
int goodevents = 0;
};

#endif 

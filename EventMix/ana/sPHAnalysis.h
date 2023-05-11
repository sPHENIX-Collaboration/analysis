
#ifndef __SPHANALYSIS_H__
#define __SPHANALYSIS_H__

#include <fun4all/SubsysReco.h>

#include <TVector3.h>

#include <HepMC/GenEvent.h>              // for GenEvent::particle_const_ite...
#include <HepMC/GenParticle.h>           // for GenParticle


class TFile;
class TNtuple;
class TH1D;
class TF1;
class TRandom;
class SvtxTrack;
class RawCluster;
class RawClusterContainer;
class RawTowerContainer;
class RawTowerGeomContainer;

class sPHAnalysis: public SubsysReco {

public:

  sPHAnalysis(const std::string &name = "sPHAnalysis", const std::string &filename = "test.root");
  virtual ~sPHAnalysis() {}

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  void set_whattodo(int what) { _whattodo = what; }

protected:

  int process_event_hepmc(PHCompositeNode *topNode);
  int process_event_pythiaupsilon(PHCompositeNode *topNode);
  int process_event_upsilons(PHCompositeNode *topNode);
  int process_event_test(PHCompositeNode *topNode);
  int process_event_bimp(PHCompositeNode *topNode);
  int process_event_pairs(PHCompositeNode *topNode);
  int process_event_notracking(PHCompositeNode *topNode);
  int process_event_filtered(PHCompositeNode *topNode);

  HepMC::GenParticle* GetParent(HepMC::GenParticle*, HepMC::GenEvent*);

  RawCluster* MatchClusterCEMC(SvtxTrack* track, RawClusterContainer* cemc_clusters, double &dphi, double &deta, double Zvtx, int what);
  double Get_CAL_e3x3(SvtxTrack* track, RawTowerContainer* _towersRawOH, RawTowerGeomContainer* _geomOH, int what, double Zvtx, double &dphi, double &deta);
  TVector3 GetProjectionToCalorimeter(SvtxTrack* track, int what);

  bool isElectron(SvtxTrack* trk);

  TFile* OutputNtupleFile;
  std::string OutputFileName;

  TNtuple* ntp_notracking;
  TNtuple* ntppid;
  TNtuple* ntpb;
  TNtuple* ntp1;
  TNtuple* ntp2;
  TNtuple* ntpmc1;
  TNtuple* ntpmc2;

  TH1D* hmult;
  TH1D* hmass;
  TH1D* hgmass;
  TH1D* hgmass0;
  TH1D* hgmass09;
  TH1D* hdphi;
  TH1D* hdeta;
  TH1D* heop;
  TH1D* heop3x3;
  TH1D* heop5x5;
  TH1D* hdca2d;
  TH1D* hdca3dxy;
  TH1D* hdca3dz;
  TH1D* hchi2;
  TH1D* hndf;
  TH1D* hquality;
  TH1D* h_notracking_etabins_em;
  TH1D* h_notracking_phibins_em;
  TH1D* h_notracking_etabins;
  TH1D* h_notracking_phibins;
  TH1D* h_notracking_etabinsout;
  TH1D* h_notracking_phibinsout;
  TH1D* h_notracking_eoh;

  TH1D* test;

  int EventNumber;

  TRandom* _rng;
  TF1 *fsin = nullptr;

  int _whattodo;

};

#endif


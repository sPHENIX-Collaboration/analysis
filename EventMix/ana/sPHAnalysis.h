
#ifndef __SPHANALYSIS_H__
#define __SPHANALYSIS_H__

#include <fun4all/SubsysReco.h>

//#include "phhepmc/PHHepMCGenEvent.h"
#include <HepMC/GenEvent.h>              // for GenEvent::particle_const_ite...
#include <HepMC/GenParticle.h>           // for GenParticle


class TFile;
class TNtuple;
class TH1D;
class TF1;
class TRandom;
class SvtxTrack;

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

  HepMC::GenParticle* GetParent(HepMC::GenParticle*, HepMC::GenEvent*);

  bool isElectron(SvtxTrack* trk);

  TFile* OutputNtupleFile;
  std::string OutputFileName;

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

  int EventNumber;

  TRandom* _rng;
  TF1 *fsin = nullptr;

  int _whattodo;

};

#endif


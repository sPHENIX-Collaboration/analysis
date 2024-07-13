// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALORIMETERTOWERENC_H
#define CALORIMETERTOWERENC_H

#include <fun4all/SubsysReco.h>

#include <TH1.h>
#include <TTree.h>
#include <TH2.h>

#include <string>
#include <cstdio>
#include <vector>
#include <map>

class PHCompositeNode;

class CalorimeterTowerENC : public SubsysReco
{
 public:

  CalorimeterTowerENC(const std::string &name = "CalorimeterTowerENC");

  ~CalorimeterTowerENC() override;

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
  int InitRun(PHCompositeNode *topNode) override {return 0;};

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override {return 0;};

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:
	TH1F *E2P, *E3P, *E2C, *E3C, *E2T, *E3T; //particle method and calo tower method as well as truth 
	TH1F *tows, *comps;
	TH1F *energyP, *energyC;
	TH2F *jethits, *comptotows; //phi-eta hit map and correlation plots for cross checks
};

#endif // CALORIMETERTOWERENC_H

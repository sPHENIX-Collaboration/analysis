// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef LARGERLENC_H
#define LARGERLENC_H

#include <fun4all/SubsysReco.h>
#include <MethodHistorgrams.h>
#include <thread>
#include <map>
#include <utility>
#include <vector>
#include <string>
class JetCuts{
	JetCuts( JetContainer* jets,  float lpt=12., slpt=7., float det=0.7, float dph=2.75,float maxpct=0.9, bool dj=true, bool ne=false ): jets(jetsi), leadingpt(lpt), subleadingpt(slpt), deltaeta(det), deltaphi(dph), maxOHCAL(  

class PHCompositeNode;

class LargeRLENC : public SubsysReco
{
 public:

  LargeRLENC(const int n_run=0, const int n_segment=0, const float jet_min_pT=1.0, const bool data=false, const std::string &name = "LargeRLENC");

  ~LargeRLENC() override;

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
	bool isRealData;
	int nRun, nSegment;
	float jetMinpT;
	MethodHistograms* FullHcal, *TowardRegion, *AwayRegion, *TransverseRegion;
};

#endif // LARGERLENC_H

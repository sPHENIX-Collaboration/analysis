// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef MBDANALYSIS_H
#define MBDANALYSIS_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;
class TTree;
class EventHeader;

class mbdAnalysis : public SubsysReco
{
 public:

  mbdAnalysis(const std::string &name = "mbdAnalysis");

  ~mbdAnalysis() override;

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
  //std::vector<float> adc;
  //std::vector<float> tdc0;
  //std::vector<float> tdc1;
  std::vector<float> pmtcharge;
  std::vector<float> pmttime;
  std::vector<float> pmtx;
  std::vector<float> pmty;
  std::vector<float> pmtz;
  std::vector<float> pmtr;
  std::vector<float> pmtphi;
  float cent;
  float vertex;
  float evtPlaneAngle;

  TFile *out;
  std::string Outfile = "commissioning.root";

};

#endif // MBDANALYSIS_H

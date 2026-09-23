// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SINGLETRACKAN_H
#define SINGLETRACKAN_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;
class TTree;
class TH1F;
class TFile;

class SingleTrackAN : public SubsysReco
{
 public:

  SingleTrackAN(const std::string &name = "SingleTrackAN");

  ~SingleTrackAN() override;

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

  void set_output_filename(const std::string &filename) { m_output_filename = filename; }

 private:

  std::string m_output_filename = "SingleTrackAN.root";

  int trackBunch = 0;
  int bspin = 0;
  int yspin = 0;

  static const int NBUNCHES = 120;
  int spinPatternBlue[NBUNCHES] = {0};
  int spinPatternYellow[NBUNCHES] = {0};

  int bunchnum;
  int sphnxbunchnum;
  int multiplicity; 
  int crossing;

    // spin stuff
  int bluepattern[120];
  int yellowpattern[120];
  float bluepolarization;
  float bluepolarizationerr;
  float yellowpolarization;
  float yellowpolarizationerr;
  int xingshift;
  int bluespin;
  int yellspin;

  // track stuff
  float p;
  float pt;
  float eta;
  float phi;
  float quality;
  float charge;


  // bco stuff 
  int candidate_crossing;
  uint64_t bco;
  uint64_t bcowindowlow;
  uint64_t bcowindowhigh;
  bool usable_bco_tag;
  bool doublecount;


  TTree *singleTrackTree = nullptr;

  TH1F *hbunchnum = nullptr;
  TH1F *hsphnxbunchnum = nullptr;
  TH1F *hmultiplicity = nullptr;
  TH1F *hcrossing = nullptr;
  TH1F *htrackBunch = nullptr;
  TH1F *hbspin = nullptr;
  TH1F *hyspin = nullptr;

  TFile *outfile; 
  




};

#endif // SINGLETRACKAN_H

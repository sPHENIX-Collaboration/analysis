// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETSEEDCOUNT_H
#define JETSEEDCOUNT_H

#include <fun4all/SubsysReco.h>
#include <g4jets/Jetv1.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TTree;
class JetSeedCount : public SubsysReco
{
 public:

  JetSeedCount(const std::string &recojetname = "AntiKt_Tower_r04",
               const std::string &truthjetname = "AntiKt_Truth_r04",
               const std::string &outputfilename = "myjetanalysis.root");

  ~JetSeedCount() override;

  void
    setEtaRange(double low, double high)
  {
    m_etaRange.first = low;
    m_etaRange.second = high;
  }
 void
   setPtRange(double low, double high)
 {
   m_ptRange.first = low;
   m_ptRange.second = high;
 }

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
   std::string m_recoJetName;
   std::string m_truthJetName;
   std::string m_outputFileName;
   std::pair<double, double> m_etaRange;
   std::pair<double, double> m_ptRange;
   std::vector<int> m_centrality;
   std::vector<int> m_centrality_diff;

   int m_event = 0;
   int m_seed_sub;
   int m_seed_raw;
   std::vector<int> m_raw_counts;
   std::vector<int> m_sub_counts;
   std::vector<double> m_rawenergy;
   std::vector<double> m_subenergy;
   std::vector<double> m_rawcent;
   std::vector<double> m_subcent;
};

#endif // JETSEEDCOUNT_H

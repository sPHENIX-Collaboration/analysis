// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef ISOCLUSTER_H
#define ISOCLUSTER_H

#include <fun4all/SubsysReco.h>

#include <string>

#include <vector>

class PHCompositeNode;
class TFile;
class TTree;

class isoCluster : public SubsysReco
{
 public:

  isoCluster(const std::string &name = "isoCluster");

  ~isoCluster() override;

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

  void setGenEvent(int eventGet)     {getEvent = eventGet;}

 private:

  TTree *T;
  //cluster information
  std::vector<float> m_clusterPhi;
  std::vector<float> m_clusterEta;
  std::vector<float> m_clusterE;
  std::vector<float> m_clusterPt;
  std::vector<float> m_clusterEtIso;
  std::vector<float> m_clusterChisq;
  
  //truth particle information
  std::vector<float> m_photonPhi;
  std::vector<float> m_photonEta;
  std::vector<float> m_photonE;
  std::vector<float> m_photonPt;

  TFile *fout;
  std::string outname;
  int getEvent;
};

#endif // ISOCLUSTER_H

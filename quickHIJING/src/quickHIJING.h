// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef QUICKHIJING_H
#define QUICKHIJING_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TTree.h>
#include <TFile.h>

class PHCompositeNode;
class PHG4Particle;
class TTree;
class TFile;

class quickHIJING : public SubsysReco
{
 public:

  quickHIJING(const std::string &name = "quickHIJING");

  ~quickHIJING() override;

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
  std::vector<int> m_pid = {0};
  std::vector<float> m_pt =  {0};
  std::vector<float> m_eta = {0};
  std::vector<float> m_phi = {0};
  std::vector<float> m_e = {0};
  std::vector<float> m_p = {0};
  float m_psi2 = 0;
  float m_cent = 0;
  float m_b = 0;
  std::string Outfile;
  TFile *out;

  float getpT(PHG4Particle *particle);
  float getPhi(PHG4Particle *particle);
  float getEta(PHG4Particle *particle);
  float getP(PHG4Particle *particle);
  
};

#endif // QUICKHIJING_H

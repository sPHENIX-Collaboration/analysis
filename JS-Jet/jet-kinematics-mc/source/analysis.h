// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <TF1.h>
#include<string.h>
#include<TProfile.h>
#include<TH1.h>
#include<TH2.h>
#include <TFile.h>
#include <TTree.h>


class Fun4AllHistoManager;

class PHCompositeNode;
class TF1;
class TH1D;
class TH2D;
class TTree;

class analysis : public SubsysReco
{
 public:

  analysis(const std::string &name = "analysis");

  virtual ~analysis();

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  void SetOutputFileName(std::string outfilename)
  {
    m_outfilename = outfilename;
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
  //---------------------------------------------
  //define the histogram manager
  //---------------------------------------------
  Fun4AllHistoManager *
  get_HistoManager();

  //-------------------------------------------------------------------------------
  //define some vectors in which we store jet information
  //-------------------------------------------------------------------------------

  std::vector<float> m_tjet_pt;
  std::vector<float> m_tjet_phi;
  std::vector<float> m_tjet_eta;
  std::vector<float> m_rjet_pt;
  std::vector<float> m_rjet_phi;
  std::vector<float> m_rjet_eta;
  std::vector<float> m_dr;
  int m_centrality; 
  int m_impactparam;
  std::string m_outfilename;

  float m_tjet_pt2;


  
  TFile* outfile;

};

#endif // ANALYSIS_H

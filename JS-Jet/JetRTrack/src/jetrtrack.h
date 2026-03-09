// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETRTRACK_H
#define JETRTRACK_H

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

class jetrtrack : public SubsysReco
{
 public:

  jetrtrack(const std::string &name = "jetrtrack");

  virtual ~jetrtrack();

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
  std::vector<float> m_tjet_jcpt;


  std::vector<float> m_rjet_pt;
  std::vector<float> m_rjet_phi;
  std::vector<float> m_rjet_eta;
  std::vector<float> m_dr;
  std::vector<float> m_trk_pt;
  std::vector<float> m_trk_eta;
  std::vector<float> m_trk_phi;
  std::vector<float> m_trk_qual;
  std::vector<float> m_nmvtxhits;

  std::vector<float> m_tp_pt;
  std::vector<float> m_tp_px;
  std::vector<float> m_tp_py;
  std::vector<float> m_tp_pz;
  std::vector<float> m_tp_phi;
  std::vector<float> m_tp_eta;


  std::vector<float> m_jc_pt;
  std::vector<int> m_jc_index;
  std::vector<float> m_jc_phi;
  std::vector<float> m_jc_eta;


  std::vector<int> m_tp_pid;

  float m_zvtx;
  /* int m_nmvtxhits; */
  int m_centrality; 
  int m_impactparam;
  std::string m_outfilename;

  float m_tjet_pt2;


  
  TFile* outfile;

};

#endif // JETRTRACK_H

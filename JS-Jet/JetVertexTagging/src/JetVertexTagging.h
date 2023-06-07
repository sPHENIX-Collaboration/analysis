// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETVERTEXTAGGING_H
#define JETVERTEXTAGGING_H

#include <fun4all/SubsysReco.h>
#include <g4jets/Jetv1.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TTree;

class JetVertexTagging : public SubsysReco
{
 public:

  JetVertexTagging(const std::string &recojetname = "AntiKt_Tower_r04",
		const std::string &truthjetname = "AntiKt_Truth_r04",
		const std::string &outputfilename = "myjetanalysis.root");

  ~JetVertexTagging() override;

  void setEtaRange(double low, double high)
  {
    m_etaRange.first = low;
    m_etaRange.second = high;
  }
 void setPtRange(double low, double high)
 {
   m_ptRange.first = low;
   m_ptRange.second = high;
 }
 void doTruth(int flag)
 {
   m_doTruthJets = flag;
 }

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
  std::string m_recoJetName;
  std::string m_truthJetName;
  std::string m_outputFileName;
  std::pair<double, double> m_etaRange;
  std::pair<double, double> m_ptRange;
  int m_doTruthJets;
 
  //! Output Tree variables
  TTree *m_T;

  //! eventwise quantities
  int m_event;
  int m_reco_jet_n;
  int m_truth_jet_n;
  int m_centrality;
  float m_impactparam;

  //! reconstructed jets
  std::vector<int> m_reco_jet_id;
  std::vector<int> m_reco_jet_nConstituents;
  std::vector<float> m_reco_jet_px;
  std::vector<float> m_reco_jet_py;
  std::vector<float> m_reco_jet_pz;
  std::vector<float> m_reco_jet_pt;
  std::vector<float> m_reco_jet_eta;
  std::vector<float> m_reco_jet_phi;
  std::vector<float> m_reco_jet_m;
  std::vector<float> m_reco_jet_e;
 // std::vector<float> m_reco_jet_dR;


  //! truth jets
  std::vector<int> m_truth_jet_id;
  std::vector<int> m_truth_jet_nConstituents;
  std::vector<float> m_truth_jet_px;
  std::vector<float> m_truth_jet_py;
  std::vector<float> m_truth_jet_pz;
  std::vector<float> m_truth_jet_pt;
  std::vector<float> m_truth_jet_eta;
  std::vector<float> m_truth_jet_phi;
  std::vector<float> m_truth_jet_m;
  std::vector<float> m_truth_jet_e;
//  std::vector<float> m_truth_jet_dR;

};

#endif // JETVERTEXTAGGING_H

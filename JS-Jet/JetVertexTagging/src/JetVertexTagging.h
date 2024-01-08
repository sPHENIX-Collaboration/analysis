// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETVERTEXTAGGING_H
#define JETVERTEXTAGGING_H

#include <fun4all/SubsysReco.h>
#include <jetbase/Jetv1.h>
#include <globalvertex/GlobalVertex.h>
#include <trackbase_historic/SvtxTrack.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop

#include <string>
#include <vector>
#include <TH1.h>

class PHCompositeNode;
class TTree;

class JetVertexTagging : public SubsysReco
{
 public:

  //note max 10 inputs allowed
  JetVertexTagging(	const std::string &outputfilename = "myjetanalysis.root", int n_inputs = 0, std::vector<std::string> TreeNameCollection= {"0","1","2","3"});

  ~JetVertexTagging() override;

  void add_input(const std::string &recojetname = "AntiKt_Tower_r04",
		const std::string &truthjetname = "AntiKt_Truth_r04"){
      m_recoJetName.push_back(recojetname);
      m_truthJetName.push_back(truthjetname);
    }

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

  std::pair<double,double> calcIP(SvtxTrack *m_dst_track, GlobalVertex *m_dst_vertex,float &val, float &err);
  std::pair<double,double> calcIP2(SvtxTrack *m_dst_track, GlobalVertex *m_dst_vertex);
  std::vector<TrkrDefs::cluskey> get_cluster_keys(SvtxTrack* track);

 private:
  int m_inputs;
  std::vector<std::string> m_TreeNameCollection;
  std::vector<std::string> m_recoJetName;
  std::vector<std::string> m_truthJetName;
  std::string m_outputFileName;
  std::pair<double, double> m_etaRange;
  std::pair<double, double> m_ptRange;
  int m_doTruthJets;
 
  //Hope Chris will not see this, everyone likes hardcoded variables, right? 
  //But it was kinda pain to make branches addresses with vectors

  //! Output Tree variables
  TTree *m_T[10];

  //! eventwise quantities
  int m_event[10];
  int m_reco_jet_n[10];
  int m_truth_jet_n[10];
  int m_centrality[10];
  float m_impactparam[10];

  //! reconstructed jets
  std::vector<int> m_reco_jet_id[10];
  std::vector<int> m_reco_jet_nConstituents[10];
  std::vector<int> m_reco_jet_nChConstituents[10];
  std::vector<float> m_reco_jet_px[10];
  std::vector<float> m_reco_jet_py[10];
  std::vector<float> m_reco_jet_pz[10];
  std::vector<float> m_reco_jet_pt[10];
  std::vector<float> m_reco_jet_eta[10];
  std::vector<float> m_reco_jet_phi[10];
  std::vector<float> m_reco_jet_m[10];
  std::vector<float> m_reco_jet_e[10];
  std::vector<std::vector<float>> m_reco_constituents_pt[10];
  std::vector<std::vector<float>> m_reco_constituents_dxy[10];
  std::vector<std::vector<float>> m_reco_constituents_dxy_unc[10];
  std::vector<std::vector<float>> m_reco_constituents_Sdxy_old[10];
  std::vector<std::vector<float>> m_reco_constituents_Sdxy[10];
  std::vector<std::vector<float>> m_reco_constituents_pt_cut[10];
  std::vector<std::vector<float>> m_reco_constituents_dxy_cut[10];
  std::vector<std::vector<float>> m_reco_constituents_dxy_unc_cut[10];
  std::vector<std::vector<float>> m_reco_constituents_Sdxy_old_cut[10];
  std::vector<std::vector<float>> m_reco_constituents_Sdxy_cut[10];

 
 // std::vector<float> m_reco_jet_dR;


  //! truth jets
  std::vector<int> m_truth_jet_id[10];
  std::vector<int> m_truth_jet_nConstituents[10];
  std::vector<int> m_truth_jet_nChConstituents[10];
  std::vector<float> m_truth_jet_px[10];
  std::vector<float> m_truth_jet_py[10];
  std::vector<float> m_truth_jet_pz[10];
  std::vector<float> m_truth_jet_pt[10];
  std::vector<float> m_truth_jet_eta[10];
  std::vector<float> m_truth_jet_phi[10];
  std::vector<float> m_truth_jet_m[10];
  std::vector<float> m_truth_jet_e[10];
  //std::vector<std::vector<std::pair<HepMC::GenVertex*,int>>> m_truth_constituents_PDG_ID[10];
  std::vector<std::vector<int>> m_truth_constituents_PDG_ID[10];
 // std::vector<std::vector<float>> m_truth_constituents_dxy[10];
 // std::vector<std::vector<float>> m_truth_constituents_dxy_unc[10];
 // std::vector<std::vector<float>> m_truth_constituents_Sdxy[10];
 // std::vector<std::vector<float>> m_truth_constituents_Sdxy_2017[10];
//  std::vector<float> m_truth_jet_dR;
  TH1D *m_chi2ndf[10];
  TH1I *m_mvtxcl[10];
  TH1I *m_inttcl[10];
  TH1I *m_mtpccl[10];

};

#endif // JETVERTEXTAGGING_H

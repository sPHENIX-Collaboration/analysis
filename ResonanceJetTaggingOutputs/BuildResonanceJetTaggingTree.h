#ifndef BUILDRESONANCEJETTAGGINGTREE_H__
#define BUILDRESONANCEJETTAGGINGTREE_H__

#include <resonancejettagging/ResonanceJetTagging.h>

#include <fun4all/SubsysReco.h>

#include <jetbase/JetMapv1.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#pragma GCC diagnostic pop

#include <vector>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TTree;
class TH1I;
class PHG4Particle;
class KFParticle_Container;
class SvtxEvalStack;
class SvtxTrackEval;

/// Definition of this analysis module class
class BuildResonanceJetTaggingTree : public SubsysReco
{
 public:

  /// Constructor
  BuildResonanceJetTaggingTree(const std::string &name = "BuildResonanceJetTaggingTree", const std::string &fname = "BuildResonanceJetTaggingTree.root", const ResonanceJetTagging::TAG tag = ResonanceJetTagging::TAG::D0);

  // Destructor
  virtual ~BuildResonanceJetTaggingTree();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);
  int loopHFHadronic(PHCompositeNode *topNode);
  void findMatchedTruthD0(PHCompositeNode *topNode, Jet *&mcTagJet, HepMC::GenParticle *&mcTag, std::vector<int> decays);
  HepMC::GenParticle *getMother(PHCompositeNode *topNode, PHG4Particle *g4daughter);
  bool isReconstructed(int index, std::vector<int> indexRecVector);

  void initializeVariables();
  void initializeTrees();
  void resetTreeVariables();

  void setDoRecunstructed(bool b) { m_dorec = b; }
  bool getDoRecunstructed() { return m_dorec; }
  void setDoTruth(bool b) { m_dotruth = b; }
  bool getDoTruth() { return m_dotruth; }

  void setTagContainerName(const std::string &tagContName) { m_tagcontainer_name = tagContName; }
  std::string getTagContainerName() { return m_tagcontainer_name; }
  void setJetContainerName(const std::string &jetContName) { m_jetcontainer_name = jetContName; }
  std::string getJetContainerName() { return m_jetcontainer_name; }
  void setTruthJetContainerName(const std::string &jetContName) { m_truth_jetcontainer_name = jetContName; }
  std::string getTruthJetContainerName() { return m_truth_jetcontainer_name; }

 private:

  JetMapv1* getJetMapFromNode(PHCompositeNode *topNode, const std::string &name);
  KFParticle_Container* getKFParticleContainerFromNode(PHCompositeNode *topNode, const std::string &name);
  HepMC::GenEvent* getGenEventFromNode(PHCompositeNode *topNode, const std::string &name);
  /// String to contain the outfile name containing the trees
  std::string m_outfilename;
  std::string m_tagcontainer_name;
  std::string m_jetcontainer_name;
  std::string m_truth_jetcontainer_name;
  JetMapv1* m_taggedJetMap;
  JetMapv1* m_truth_taggedJetMap;
  bool m_dorec;
  bool m_dotruth;
  int m_nDaughters;
  SvtxEvalStack *m_svtx_evalstack = nullptr;
  SvtxTrackEval *m_trackeval = nullptr;

  std::vector<float> m_truthjet_const_px, m_truthjet_const_py,
    m_truthjet_const_pz, m_truthjet_const_e;

  ResonanceJetTagging::TAG m_tag_particle;
  int m_tag_pdg;

  /// TFile to hold the following TTrees and histograms
  TFile *m_outfile = nullptr;
  TH1I *m_eventcount_h = nullptr;
  TTree *m_taggedjettree = nullptr;
  TTree *m_runinfo = nullptr;
  // Tagged-Jet reconstructed variables
  float m_reco_tag_px = NAN;
  float m_reco_tag_py = NAN;
  float m_reco_tag_pz = NAN;
  float m_reco_tag_pt = NAN;
  float m_reco_tag_eta = NAN;
  float m_reco_tag_phi = NAN;
  float m_reco_tag_m = NAN;
  float m_reco_tag_e = NAN;
  float m_reco_jet_px = NAN;
  float m_reco_jet_py = NAN;
  float m_reco_jet_pz = NAN;
  float m_reco_jet_pt = NAN;
  float m_reco_jet_eta = NAN;
  float m_reco_jet_phi = NAN;
  float m_reco_jet_m = NAN;
  float m_reco_jet_e = NAN;
  //Truth info
  float m_truth_tag_px = NAN;
  float m_truth_tag_py = NAN;
  float m_truth_tag_pz = NAN;
  float m_truth_tag_pt = NAN;
  float m_truth_tag_eta = NAN;
  float m_truth_tag_phi = NAN;
  float m_truth_tag_m = NAN;
  float m_truth_tag_e = NAN;
  float m_truth_jet_px = NAN;
  float m_truth_jet_py = NAN;
  float m_truth_jet_pz = NAN;
  float m_truth_jet_pt = NAN;
  float m_truth_jet_eta = NAN;
  float m_truth_jet_phi = NAN;
  float m_truth_jet_m = NAN;
  float m_truth_jet_e = NAN;

  float m_intlumi = NAN;
  float m_nprocessedevents = NAN;
  float m_nacceptedevents = NAN;
  float m_xsecprocessedevents = NAN;
  float m_xsecacceptedevents = NAN;

};

#endif

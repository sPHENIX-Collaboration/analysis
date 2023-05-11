// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TRACKSINJETS_H
#define TRACKSINJETS_H

#include <fun4all/SubsysReco.h>
#include <particleflowreco/ParticleFlowElement.h>

#include <string>
#include <TFile.h>
#include <TTree.h>

class PHCompositeNode;

class TracksInJets : public SubsysReco
{
 public:

  TracksInJets(const std::string& name = "TracksInJets", 
	       const std::string& out = "TracksInJets.root");

  ~TracksInJets() override;
  void minTruthJetPt(const float pt) { m_minjettruthpt = pt; }
  void truthJetMapName(const std::string& name) { m_truthjetmapname = name; }
  void recoJetMapName(const std::string& name) { m_recojetmapname = name; }
  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

 private:

  void setBranches();
  float dR(const float& eta1, const float& eta2,  
	   const float& phi1, const float& phi2);
  std::string m_outfilename = "TracksInJets.root";
  std::string m_truthjetmapname = "AntiKt_Truth_r04";
  std::string m_recojetmapname = "AntiKt_ParticleFlow_r04";

  TFile *m_outfile = nullptr;
  TTree *m_tree = nullptr;

  int m_embeddingid = 1;
  float m_minjettruthpt = 10;

  float m_truthjetpx;
  float m_truthjetpy;
  float m_truthjetpz;
  float m_truthjete;
  float m_recojetpx;
  float m_recojetpy;
  float m_recojetpz;
  float m_recojete;
  int m_truthjetconst;
  int m_recojetconst;

  std::vector<float> m_truthjettrackpx;
  std::vector<float> m_truthjettrackpy;
  std::vector<float> m_truthjettrackpz;
  std::vector<float> m_truthjettrackpid;
  std::vector<float> m_truthjettrackvx;
  std::vector<float> m_truthjettrackvy;
  std::vector<float> m_truthjettrackvz;
  
  std::vector<float> m_recojettrackpx;
  std::vector<float> m_recojettrackpy;
  std::vector<float> m_recojettrackpz;
  std::vector<float> m_recojettracke;
  std::vector<ParticleFlowElement::PFLOWTYPE> m_recojettracktype;
  std::vector<float> m_recojettrackvx;
  std::vector<float> m_recojettrackvy;
  std::vector<float> m_recojettrackvz;
  std::vector<float> m_recojettrackpcax;
  std::vector<float> m_recojettrackpcay;
  std::vector<float> m_recojettrackpcaz;
};

#endif // TRACKSINJETS_H

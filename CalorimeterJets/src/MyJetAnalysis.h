#ifndef MYJETANALYSIS_H
#define MYJETANALYSIS_H

#include <fun4all/SubsysReco.h>

#include <memory>
#include <string>
#include <vector>
#include <TLorentzVector.h>
#include <utility>  // std::pair, std::make_pair

#include <array>
#include <g4jets/Jet.h>

#include <HepMC/GenEvent.h>          
#include <HepMC/GenParticle.h> 

#include <fastjet/PseudoJet.hh>

class PHCompositeNode;
class JetEvalStack;
class CaloEvalStack;
class TTree;
class TH1;
class FastJetAlgo;

/// \class MyJetAnalysis
class MyJetAnalysis : public SubsysReco
{
 public:
  MyJetAnalysis(
      const std::string &recojetname = "AntiKt_Tower_r02",
      const std::string &truthjetname = "AntiKt_Truth_r02",
      const std::string &outputfilename = "myjetanalysis.root");

  virtual ~MyJetAnalysis();

  //! set eta range
  void
  setEtaRange(double low, double high)
  {
    m_etaRange.first = low;
    m_etaRange.second = high;
  }
  //! set eta range
  void
  setPtRange(double low, double high)
  {
    m_ptRange.first = low;
    m_ptRange.second = high;
  }
 
  void
  setMindR(double jetradius)
  {
      m_trackJetMatchingRadius = jetradius;
  }
  void use_initial_vertex(const bool b = true) {initial_vertex = b;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int process_event_bimp(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
  void initializeTrees();
  void Clean();

 private:
  //! cache the jet evaluation modules
  std::shared_ptr<JetEvalStack> m_jetEvalStack;
  FastJetAlgo * m_fastjet;
  std::vector<Jet*> m_inputs_smallR;
  std::vector<Jet*> m_inputs_reco_smallR;
  std::vector<Jet*> m_inputs_recoMatch_smallR;
  std::vector<Jet*> m_output_largeR;

  std::string m_recoJetName;
  std::string m_truthJetName;
  std::string m_outputFileName;

  std::vector<fastjet::PseudoJet> m_truth_pseudojet;

  //! eta range
  std::pair<double, double> m_etaRange;

  //! pT range
  std::pair<double, double> m_ptRange;

  //! flag to use initial vertex in track evaluator
  bool initial_vertex = false;

  //! max track-jet matching radius
  double m_trackJetMatchingRadius;

  //! Output histograms
  TH1 *m_hInclusiveE;
  TH1 *m_hInclusiveEta;
  TH1 *m_hInclusivePhi;

  //! Output Tree variables
  TTree *m_T;
  TTree *m_Treclus;
  //TTree *m_TreEcoor;
  std::vector<double> m_recotruthE;
  std::vector<double> m_recotruthEta;
  std::vector<double> m_recotruthPhi;
  std::vector<double> m_recotruthPt; 
  std::vector<double> m_recotruthPx;
  std::vector<double> m_recotruthPy;
  std::vector<double> m_recotruthPz; 
  std::vector<double> m_recotruthnComponent; 
  
  int m_event;
  std::vector<double> m_id;
  std::vector<double> m_nComponent;
  std::vector<double> m_eta;
  std::vector<double> m_phi;
  std::vector<double> m_e;
  std::vector<double> m_new_e;
  std::vector<double> m_pt;
  std::vector<double> m_px;
  std::vector<double> m_py;
  std::vector<double> m_pz;
  std::vector<double> m_dR;
  std::vector<double> m_cent; 
  std::vector<double> m_CAL_ID;
  std::vector<double> m_Constit_E;
  std::vector<double> m_Constit_Cent;
  //std::vector<double> m_Embedded_Count;
  //std::vector<double> m_Embedded_IHCAL;
  //std::vector<double> m_Embedded_CEMC;
  //std::vector<double> m_Embedded_OHCAL;
  //std::vector<double> m_Background_IHCAL;
  //std::vector<double> m_Background_CEMC;
  //std::vector<double> m_Background_OHCAL;
  //std::vector<double> m_Unidentified_OHCAL;
  //std::vector<double> m_Unidentified_IHCAL;
  //std::vector<double> m_Total_Count;
  std::vector<double> towers_id;
  std::vector<double> towers_primary;
  std::vector<double> m_IHCAL_Tower_Energy;
  std::vector<double> m_IHCAL_Cent;
  std::vector<double> m_EMCAL_Tower_Energy;
  std::vector<double> m_EMCAL_Cent;
  std::vector<double> m_OHCAL_Tower_Energy;
  std::vector<double> m_OHCAL_Cent;

  double_t dPhi;
  double_t dPhi_temp; 
 
  CaloEvalStack* _caloevalstackHCALIN;
  CaloEvalStack* _caloevalstackHCALOUT;
  CaloEvalStack* _caloevalstackCEMC;

  float temp_eta;
  float temp_phi;
  float temp_e;
  float temp_dR;
 
  float temp_E_Matched;
  float temp_Phi_Matched;
  float temp_Eta_Matched;

  float temp_TE_Matched;
  float temp_TPhi_Matched;
  float temp_TEta_Matched;

  std::vector<double> m_truthNComponent;
  std::vector<double> m_truthEta;
  std::vector<double> m_truthPhi;
  std::vector<double> m_truthE;
  std::vector<double> m_truthPt;
  std::vector<double> m_truthdR;
  std::vector<double> m_truthPx;
  std::vector<double> m_truthPy;
  std::vector<double> m_truthPz;
  std::vector<double> m_truthConstitID;
  std::vector<double> m_truthConstitE;
  std::vector<double> m_truthConstitPt; 
 
  std::vector<double> m_E_Matched;
  std::vector<double> m_Phi_Matched;
  std::vector<double> m_Eta_Matched;

  std::vector<double> m_TE_Matched;
  std::vector<double> m_TPhi_Matched;
  std::vector<double> m_TEta_Matched;
 
  //std::vector<double> m_PairedID_Truth;
  //std::vector<double> m_PairedID_Reco;
  //std::vector<double> m_impactparam;  

  //! number of matched tracks
  int m_nMatchedTrack;

  enum
  {
    //! max number of tracks
    kMaxMatchedTrack = 1000
  };
  std::array<float, kMaxMatchedTrack> m_trackdR;
  std::array<float, kMaxMatchedTrack> m_trackpT;
  std::array<float, kMaxMatchedTrack> m_trackPID;
};

#endif  // MYJETANALYSIS_H

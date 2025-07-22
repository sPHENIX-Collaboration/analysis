// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETANALYSIS_H
#define JETANALYSIS_H


#include <string>
#include <vector>

#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <THn.h>

#include "fulljetfinder/FullJetFinder.h"

class TFile;
 


class JetAnalysis{
 public:

struct Flavour{
  bool isC, isB, isLF;
};

  JetAnalysis(){};

  ~JetAnalysis(){};

  bool processCondor(const std::string &dataFiles = "/sphenix/tg/tg01/hf/jkvapil/JET30_r11_30GeV/condorJob/myTestJets/outputData_000*.root");
  std::set<int> findDuplicates(std::vector<int> vec);
  Flavour getFlavour(FullJetFinder::TruthJets truth_jet);
  bool CheckValue(ROOT::Internal::TTreeReaderValueBase& value);
  void printProgress(int cur, int total);
  //bool scanPurityEfficiency();
  void setWorkDir(const std::string &workdir){work_dir_ = workdir;};
  void printHiisto(TH1* h);



   /*class MatchedJetContainer: public PHObject
  {
    public: 
    MatchedJetContainer()=default;
    void Reset();
    GlobalVertex::VTXTYPE vtxtype;
    float reco_pt;
    float reco_jet_nChConstituents;
    float reco_jet_nConstituents;
    std::vector<float> reco_constituents_Sdxy;
    float reco_jet_Sdxy_1N;
    float reco_jet_Sdxy_2N;
    float reco_jet_Sdxy_3N;
    float reco_jet_Sdxy_4N;
    Flavour truth_jet_flavour;
    float truth_pt;
    float truth_jet_nChConstituents;
    float truth_jet_nConstituents;
    ClassDef(MatchedJetContainer,1)
  };*/

  private:
  std::string work_dir_;
  
  TTree *ttree_;
  TTreeReaderValue<FullJetFinder::Container> *jet_container_= nullptr;
  
  TTree *ttree_out_;

  //! reconstructed jets
  float m_reco_jet_pt;
  int m_reco_jet_nConstituents;
  int m_reco_jet_nChConstituents;
  float reco_sdxy_1N;
  float reco_sdxy_2N;
  float reco_sdxy_3N;
  float m_truth_jet_pt;
  int m_truth_jet_nConstituents;
  int m_truth_jet_nChConstituents;
  int m_truth_jet_flavour;


  //TH1I *matching;




};

#endif // JETANALYSIS_H

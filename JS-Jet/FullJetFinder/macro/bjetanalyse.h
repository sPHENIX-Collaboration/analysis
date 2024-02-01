// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef bjetanalyse_H
#define bjetanalyse_H


#include <string>
#include <vector>

#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <THn.h>

#include "JetVertexTagging.h"

class TFile;
 


class bjetanalyse{
 public:

  bjetanalyse(	const std::string &filename = "myjetanalysis.root");

  ~bjetanalyse(){};

  bool analyse();
  std::set<int> findDuplicates(std::vector<int> vec);
  bool CheckValue(ROOT::Internal::TTreeReaderValueBase& value);
  void printProgress(int cur, int total);
  bool Plot();

 private:
   std::string input_file_name_;

   
  TTree *ttree_;
  TTreeReaderValue<JetVertexTagging::Container> *jet_container_= nullptr;
  TTreeReaderValue<JetVertexTagging::Container> *jet_container_2= nullptr;
  //TTreeReaderValue<int> *n_event_ = nullptr;
  //TTreeReaderValue<int> *n_reco_jet_= nullptr;
  //TTreeReaderValue<int> *n_truth_jet_= nullptr;

  //! reconstructed jets
 /* TTreeReaderValue<std::vector<int>> *reco_jet_id_= nullptr;
  TTreeReaderValue<std::vector<int>> *reco_jet_nConstituents_= nullptr;
  TTreeReaderValue<std::vector<int>> *reco_jet_nChConstituents_= nullptr;
  TTreeReaderValue<std::vector<float>> *reco_jet_px_= nullptr;
  TTreeReaderValue<std::vector<float>> *reco_jet_py_= nullptr;
  TTreeReaderValue<std::vector<float>> *reco_jet_pz_= nullptr;
  TTreeReaderValue<std::vector<float>> *reco_jet_pt_= nullptr;
  TTreeReaderValue<std::vector<float>> *reco_jet_eta_= nullptr;
  TTreeReaderValue<std::vector<float>> *reco_jet_phi_= nullptr;
  TTreeReaderValue<std::vector<float>> *reco_jet_m_= nullptr;
  TTreeReaderValue<std::vector<float>> *reco_jet_e_= nullptr;
  TTreeReaderValue<std::vector<std::vector<float>>> *reco_constituents_dxy_= nullptr;
  TTreeReaderValue<std::vector<std::vector<float>>> *reco_constituents_dxy_unc_= nullptr;
  TTreeReaderValue<std::vector<std::vector<float>>> *reco_constituents_Sdxy_old_= nullptr;
  TTreeReaderValue<std::vector<std::vector<float>>> *reco_constituents_Sdxy_= nullptr;
  TTreeReaderValue<std::vector<std::vector<float>>> *reco_constituents_dxy_cut_= nullptr;
  TTreeReaderValue<std::vector<std::vector<float>>> *reco_constituents_dxy_unc_cut_= nullptr;
  TTreeReaderValue<std::vector<std::vector<float>>> *reco_constituents_Sdxy_old_cut_= nullptr;
  TTreeReaderValue<std::vector<std::vector<float>>> *reco_constituents_Sdxy_cut_= nullptr;


  //! truth jets
  TTreeReaderValue<std::vector<int>> *truth_jet_id_= nullptr;
  TTreeReaderValue<std::vector<int>> *truth_jet_nConstituents_= nullptr;
  TTreeReaderValue<std::vector<int>> *truth_jet_nChConstituents_= nullptr;
  TTreeReaderValue<std::vector<float>> *truth_jet_px_= nullptr;
  TTreeReaderValue<std::vector<float>> *truth_jet_py_= nullptr;
  TTreeReaderValue<std::vector<float>> *truth_jet_pz_= nullptr;
  TTreeReaderValue<std::vector<float>> *truth_jet_pt_= nullptr;
  TTreeReaderValue<std::vector<float>> *truth_jet_eta_= nullptr;
  TTreeReaderValue<std::vector<float>> *truth_jet_phi_= nullptr;
  TTreeReaderValue<std::vector<float>> *truth_jet_m_= nullptr;
  TTreeReaderValue<std::vector<float>> *truth_jet_e_= nullptr;
  TTreeReaderValue<std::vector<std::vector<int>>> *truth_constituents_PDG_ID_= nullptr;*/

  THnD *THn_matrix;

  TTree *ttree_out_;

  //! reconstructed jets
  float m_reco_jet_pt;
  int m_reco_jet_nConstituents;
  int m_reco_jet_nChConstituents;
  std::vector<float> m_reco_constituents_dxy;
  std::vector<float> m_reco_constituents_dxy_unc;
  std::vector<float> m_reco_constituents_Sdxy_old;
  std::vector<float> m_reco_constituents_Sdxy;
  std::vector<float> m_reco_constituents_dxy_cut;
  std::vector<float> m_reco_constituents_dxy_unc_cut;
  std::vector<float> m_reco_constituents_Sdxy_old_cut;
  std::vector<float> m_reco_constituents_Sdxy_cut;
  float m_reco_jet_Sdxy_1N;
  float m_reco_jet_Sdxy_2N;
  float m_reco_jet_Sdxy_3N;
  float m_truth_jet_pt;
  int m_truth_jet_nConstituents;
  int m_truth_jet_nChConstituents;
  int m_truth_jet_flavour;




};

#endif // bjetanalyse_H

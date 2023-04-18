#include "JetRhoMedian.h"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/BackgroundEstimatorBase.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TRandom3.h>
#include <TString.h>
#include <TTree.h>
#include <TVector3.h>
#include <algorithm>
#include <cassert>
#include <centrality/CentralityInfo.h>
#include <cmath>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <g4eval/JetEvalStack.h>
#include <g4jets/JetInput.h>
#include <g4jets/JetMapv1.h>
#include <iostream>
#include <limits>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <stdexcept>
#include <trackbase_historic/SvtxTrackMap.h>

// using namespace fastjet;

JetRhoMedian::JetRhoMedian
     ( const std::string& outputfilename
     , const float        pTlb
     , const int          n_print_freq // ... ditto
     , const int          total_jobs   // for progress printout statements
     , const double       min_calo_pt
     , const std::string& truthjetname
     )
  : SubsysReco("JetRhoMedian")
  , m_min_calo_pt       { min_calo_pt    }
  , m_truthJetName      { truthjetname   }
  , m_outputFileName    { outputfilename }
  , m_etaRange          { -0.6, 0.6 }
  , m_ptRange           { 5,    100 }
  , m_T                 { nullptr        }
  , m_pTlb              { -1. }
  , m_CaloJetEta        {}
  , m_CaloJetPhi        {}
  , m_CaloJetE          {}
  , m_CaloJetPtLessRhoA {}
  , m_CaloJetArea       {}
  , m_TruthJetEta       {}
  , m_TruthJetPhi       {}
  , m_TruthJetE         {}
  , m_TruthJetPt        {}
  , m_TruthJetArea      {}
  , m_inputs            {}
  , print_stats         { n_print_freq,  total_jobs }
{ 
  m_pTlb = pTlb;
  m_centrality = -1.;
  m_id = -1;
  m_impactparam = -1.;
}

JetRhoMedian::~JetRhoMedian()
{ 
  for (unsigned int i = 0; i < m_inputs.size(); ++i) delete m_inputs[i];
  m_inputs.clear();
  print_stats.set_get_stats();
  std::cout << " Max memory used: " << print_stats.max_mem/1000. << " MB " << std::endl;

}

int JetRhoMedian::Init(PHCompositeNode* topNode)
{
  if (Verbosity() >= JetRhoMedian::VERBOSITY_SOME)
    std::cout << "JetRhoMedian::Init - Outoput to " << m_outputFileName << std::endl;

  PHTFileServer::get().open(m_outputFileName, "RECREATE");

  //Tree
  m_T = new TTree("T", "JetRhoMedian Tree");

  //      int m_event;
  m_T->Branch("id",          &m_id);
  m_T->Branch("pTlb",        &m_pTlb, "Lower bound on pT. 0 for MB, 10 for jets >10, 30 for jets > 30");
  m_T->Branch("rho",         &m_rho);
  m_T->Branch("rho_sigma",   &m_rho_sigma);
  m_T->Branch("centrality",  &m_centrality);
  m_T->Branch("impactparam", &m_impactparam);

  m_T->Branch("CaloJetEta",        &m_CaloJetEta);
  m_T->Branch("CaloJetPhi",        &m_CaloJetPhi);
  m_T->Branch("CaloJetE",          &m_CaloJetE);
  m_T->Branch("CaloJetPtLessRhoA", &m_CaloJetPtLessRhoA);
  m_T->Branch("CaloJetArea",       &m_CaloJetArea);

  //Truth Jets
  m_T->Branch("TruthJetEta",  &m_TruthJetEta);
  m_T->Branch("TruthJetPhi",  &m_TruthJetPhi);
  m_T->Branch("TruthJetE",    &m_TruthJetE);
  m_T->Branch("TruthJetPt",   &m_TruthJetPt);
  m_T->Branch("TruthJetArea", &m_TruthJetArea);

  return Fun4AllReturnCodes::EVENT_OK;
}

int JetRhoMedian::End(PHCompositeNode* topNode)
{
  std::cout << "JetRhoMedian::End - Output to " << m_outputFileName << std::endl;
  PHTFileServer::get().cd(m_outputFileName);

  /* m_hInclusiveE->Write(); */
  /* m_hInclusiveEta->Write(); */
  /* m_hInclusivePhi->Write(); */
  m_T->Write();

  return Fun4AllReturnCodes::EVENT_OK;
}

int JetRhoMedian::InitRun(PHCompositeNode* topNode)
{
  topNode->print();
  std::cout << " Input Selections:" << std::endl;
  for (unsigned int i = 0; i < m_inputs.size(); ++i) m_inputs[i]->identify();
  return Fun4AllReturnCodes::EVENT_OK;
}

int JetRhoMedian::process_event(PHCompositeNode* topNode)
{
  print_stats.call();
  clear_vectors();

  //interface to truth jets
  JetMap* jetsMC = findNode::getClass<JetMap>(topNode, m_truthJetName);
  if (!jetsMC )
  {
    std::cout
      << "MyJetAnalysis::process_event - Error can not find DST Truth JetMap node "
      << m_truthJetName << std::endl;
    exit(-1);
  }

  // get the inputs for reconstructed jets (from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.cc)
  std::vector<Jet *> inputs;  // owns memory
  for (unsigned int iselect = 0; iselect < m_inputs.size(); ++iselect)
  {
    std::vector<Jet *> parts = m_inputs[iselect]->get_input(topNode);
    for (unsigned int ipart = 0; ipart < parts.size(); ++ipart)
    {
      inputs.push_back(parts[ipart]);
      inputs.back()->set_id(inputs.size() - 1);  // unique ids ensured
    }
  }

  auto& particles=inputs;

  // /direct/sphenix+u/dstewart/vv/coresoftware/offline/packages/jetbackground/FastJetAlgoSub.cc ::58
  std::vector<fastjet::PseudoJet> particles_pseudojets;
  int   smallptcutcnt =0.2;//FIXME
  for (unsigned int ipart = 0; ipart < particles.size(); ++ipart)
  {
    float this_e = particles[ipart]->get_e();

    if (this_e == 0.) continue;

    float this_px = particles[ipart]->get_px();
    float this_py = particles[ipart]->get_py();
    float this_pz = particles[ipart]->get_pz();

    if (this_e < 0)
    {
      // make energy = +1 MeV for purposes of clustering
      float e_ratio = 0.001 / this_e;

      this_e  = this_e * e_ratio;
      this_px = this_px * e_ratio;
      this_py = this_py * e_ratio;
      this_pz = this_pz * e_ratio;
    }

    fastjet::PseudoJet pseudojet(this_px, this_py, this_pz, this_e);

    pseudojet.set_user_index(ipart);

    float _pt = pseudojet.perp();
    if (_pt < m_min_calo_pt) {
      /* std::cout << " CUT SMALL: " << _pt << " < " << m_min_calo_pt << std::endl; */
      ++smallptcutcnt;
    } else {
      particles_pseudojets.push_back(pseudojet);
    }
  }
  for (auto &p : particles) delete p;

  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (cent_node)
  {
    m_centrality  =  cent_node->get_centile(CentralityInfo::PROP::bimp);
    m_impactparam =  cent_node->get_quantity(CentralityInfo::PROP::bimp);
  }

  vector<Jet*> truth_jets;
  for (auto& jet : jetsMC->vec(Jet::SORT::PT)) {  // will return jets in order of descending pT
    float pt  = jet->get_pt();
    float eta = jet->get_eta();
    if  (pt < m_ptRange.first
        || pt  > m_ptRange.second
        || eta < m_etaRange.first
        || eta > m_etaRange.second) continue;
    truth_jets.push_back(jet);
  }

  for (auto jet : truth_jets) {
    m_TruthJetPt .push_back(jet->get_pt());
    m_TruthJetEta.push_back(jet->get_eta());
    m_TruthJetPhi.push_back(jet->get_phi());
    m_TruthJetE  .push_back(jet->get_e());
  }

  if (Verbosity()>5) std::cout << "Starting background density calc" << std::endl;
  fastjet::JetDefinition jet_def(fastjet::kt_algorithm, 0.4);     //  JET DEFINITION

  const double ghost_max_rap { 1.0 };
  const double ghost_R = 0.01;
  const double jet_R = 0.4;
  fastjet::AreaDefinition area_def_bkgd( fastjet::active_area_explicit_ghosts, fastjet::GhostedAreaSpec(ghost_max_rap, 1, ghost_R));
  fastjet::JetDefinition jet_def_bkgd(fastjet::kt_algorithm, jet_R); // <--
  fastjet::Selector selector_rm2 = fastjet::SelectorAbsEtaMax(0.6) * (!fastjet::SelectorNHardest(2)); // <--
  fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def_bkgd};
  bge_rm2.set_particles(particles_pseudojets);

  m_rho = bge_rm2.rho();
  m_rho_sigma = bge_rm2.sigma();

  if (Verbosity()>5) std::cout << "Starting clustered jets" << std::endl;
  // cluster the measured jets:
  double max_rap = 2.0;
  fastjet::Selector jetrap         = fastjet::SelectorAbsEtaMax(0.6);
  fastjet::Selector not_pure_ghost = !fastjet::SelectorIsPureGhost();
  fastjet::Selector selection      = jetrap && not_pure_ghost;
  fastjet::AreaDefinition area_def( fastjet::active_area_explicit_ghosts, fastjet::GhostedAreaSpec(max_rap, 1, ghost_R));
  fastjet::JetDefinition jet_def_antikt(fastjet::antikt_algorithm, jet_R);
  fastjet::ClusterSequenceArea clustSeq(particles_pseudojets, jet_def_antikt, area_def);
  vector<fastjet::PseudoJet> jets = sorted_by_pt( selection( clustSeq.inclusive_jets(m_ptRange.first) ));
  for (auto jet : jets) {
    m_CaloJetEta .push_back( jet.eta()     );
    m_CaloJetPhi .push_back( jet.phi_std() );
    m_CaloJetE   .push_back( jet.E()       );
    m_CaloJetPtLessRhoA   .push_back( jet.pt() - m_rho * jet.area());
    m_CaloJetArea .push_back( jet.area());
  }

  if (Verbosity() > 5) {
    std::cout << "------------------" << std::endl;
    std::cout << "Filling Tree: " << std::endl
      /* << " id:          " << m_id                << std::endl */
      << " pTlb:        " << m_pTlb              << std::endl
      << " rho:         " << m_rho               << std::endl
      /* << " rho_sigma:   " << m_rho_sigma         << std::endl */
      << " centrality:  " << m_centrality        << std::endl
      << " nJetsTruth:  " << m_TruthJetE.size() << std::endl;
    if (m_TruthJetE.size() > 0) {
    std::cout << " TruthJet0 pT/phi/eta: " << m_TruthJetPt[0] <<  "/" << m_TruthJetPhi[0] << "/" << m_TruthJetEta[0] << std::endl;
    }
    std::cout << " nJetsCalo:  " << m_CaloJetArea.size() << std::endl;
    if (m_CaloJetE.size() > 0) {
    std::cout << " CaloJet0 (pT-rho x A)/phi/eta: " << m_CaloJetPtLessRhoA[0] <<  "/" << m_CaloJetPhi[0] << "/" << m_CaloJetEta[0] << std::endl;
    }
  }
  m_T->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}

void JetRhoMedian::clear_vectors() {
  m_CaloJetEta.clear();
  m_CaloJetPhi.clear();
  m_CaloJetE.clear();
  m_CaloJetPtLessRhoA.clear();
  m_CaloJetArea.clear();
  
  m_TruthJetEta.clear();
  m_TruthJetPhi.clear();
  m_TruthJetE.clear();
  m_TruthJetPt.clear();
  m_TruthJetArea.clear();
}

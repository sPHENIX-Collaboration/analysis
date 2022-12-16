#include "JetPlusBackground.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>

// fastjet includes
#include <fastjet/ClusterSequence.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/BackgroundEstimatorBase.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4eval/JetEvalStack.h>

#include <trackbase_historic/SvtxTrackMap.h>

#include <centrality/CentralityInfo.h>

//#include <g4jets/JetMap.h>
#include <g4jets/JetMapv1.h>
#include <g4jets/JetInput.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TString.h>
#include <TTree.h>
#include <TVector3.h>

#include <TRandom3.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;
using namespace fastjet;

/* bool JetPlusBackground::jet_has_index(vector<fastjet::PseudoJet>& jets, int which_jet, int index) { */
/*   if (which_jet >= (int) jets.size()) return false; */
/*   for (auto& cst : jets[which_jet].constituents()) { */
/*     if (cst.user_index() == index) return true; */
/*   } */
/*   return false; */
/* }; */
/* bool JetPlusBackground::jetGT2_has_index(vector<fastjet::PseudoJet>& jets, int index) { */
/*   if (jets.size()<3) return false; */
/*   for (int i=2; i< (int)jets.size(); ++i) { */
/*     if (jet_has_index(jets,i,index)) return true; */
/*   } */
/*   return false; */
/* }; */

JetPlusBackground::JetPlusBackground(
    const double _min_calo_pt,
    const int total_jobs,
    const int n_print_freq,
    const std::string& recojetname,
    /* const std::string& truthjetname, */
    const std::string& outputfilename
    )
: SubsysReco("JetPlusBackground_" + recojetname )
  , min_calo_pt {_min_calo_pt}
, m_recoJetName(recojetname)
  /* , m_truthJetName(truthjetname) */
  , m_outputFileName(outputfilename)
  , m_etaRange    (-1, 1)
  , m_ptRange     (5,  100)
  , m_T           (nullptr)
, m_id          (-1)
  , m_CaloJetEta  {}
  , m_CaloJetPhi  {}
  , m_CaloJetE    {}
  , m_CaloJetPt   {}
  , m_CaloJetArea {}
  , m_embEta      {-100.}
  , m_embPhi      {-100.}
  , m_embPt       {-100.}
  , _inputs        {}
  , print_stats{n_print_freq, total_jobs}
  , rng {}
{ 
}

JetPlusBackground::~JetPlusBackground()
{ 
  for (unsigned int i = 0; i < _inputs.size(); ++i) delete _inputs[i];
  _inputs.clear();
  print_stats.set_get_stats();
  cout << " Max memory used: " << print_stats.max_mem/1000. << " MB " << endl;

}

int JetPlusBackground::Init(PHCompositeNode* topNode)
{
  if (Verbosity() >= JetPlusBackground::VERBOSITY_SOME)
    cout << "JetPlusBackground::Init - Outoput to " << m_outputFileName << endl;

  PHTFileServer::get().open(m_outputFileName, "RECREATE");

  //Tree
  m_T = new TTree("T", "JetPlusBackground Tree");

  // int m_event;
  m_T->Branch("id",          &m_id);
  m_T->Branch("rho",         &m_rho);
  m_T->Branch("rho_sigma",   &m_rho_sigma);
  m_T->Branch("centrality",  &m_centrality);
  m_T->Branch("impactparam", &m_impactparam);

  m_T->Branch("rhoBias",     &m_RhoBias);

  m_T->Branch("CaloJetEta",  &m_CaloJetEta);
  m_T->Branch("CaloJetPhi",  &m_CaloJetPhi);
  m_T->Branch("CaloJetE",    &m_CaloJetE);
  m_T->Branch("CaloJetPt",   &m_CaloJetPt);
  m_T->Branch("CaloJetArea", &m_CaloJetArea);

  m_T->Branch("embEta",      &m_embEta);
  m_T->Branch("embPhi",      &m_embPhi);
  m_T->Branch("embPt",       &m_embPt);


  return Fun4AllReturnCodes::EVENT_OK;
}

int JetPlusBackground::End(PHCompositeNode* topNode)
{
  cout << "JetPlusBackground::End - Output to " << m_outputFileName << endl;
  PHTFileServer::get().cd(m_outputFileName);
  m_T->Write();
  return Fun4AllReturnCodes::EVENT_OK;
}

int JetPlusBackground::InitRun(PHCompositeNode* topNode)
{
  topNode->print();
  cout << " Input Selections:" << endl;
  for (unsigned int i = 0; i < _inputs.size(); ++i) _inputs[i]->identify();
  return Fun4AllReturnCodes::EVENT_OK;
}

int JetPlusBackground::process_event(PHCompositeNode* topNode)
{
  print_stats.call();

  // get the inputs for reconstructed jets (from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.cc
  std::vector<Jet *> inputs;  // owns memory
  for (unsigned int iselect = 0; iselect < _inputs.size(); ++iselect)
  {
    std::vector<Jet *> parts = _inputs[iselect]->get_input(topNode);
    for (unsigned int ipart = 0; ipart < parts.size(); ++ipart)
    {
      inputs.push_back(parts[ipart]);
      inputs.back()->set_id(inputs.size() - 1);  // unique ids ensured
    }
  }

  auto& particles=inputs;

  // /direct/sphenix+u/dstewart/vv/coresoftware/offline/packages/jetbackground/FastJetAlgoSub.cc ::58
  std::vector<fastjet::PseudoJet> particles_pseudojets;
  int   smallptcutcnt =0;//FIXME
  for (unsigned int ipart = 0; ipart < particles.size(); ++ipart)
  {
    float this_e = particles[ipart]->get_e();

    if (this_e == 0. /*|| isnan(this_e)*/ ) continue;

    float this_px = particles[ipart]->get_px();
    float this_py = particles[ipart]->get_py();
    float this_pz = particles[ipart]->get_pz();

  //  if ( isnan(this_px) || isnan(this_py) || isnan(this_pz) ) continue;

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

    //if ( fabs(pseudojet.eta())>1. ) continue;
      
    float _pt = pseudojet.perp();
    if (_pt < min_calo_pt) {
      /* cout << " CUT SMALL: " << _pt << " < " << min_calo_pt << endl; */
      ++smallptcutcnt;
    } else {
      particles_pseudojets.push_back(pseudojet);
    }
  }


  // add a psuedojet dijet (-ish); just require recoil
  m_embPhi = rng.Uniform(-M_PI, M_PI);
  m_embPt  = rng.Uniform(50,80);
  m_embEta = rng.Uniform(-0.6, 0.6);

  auto embjet = fastjet::PseudoJet();
  embjet.reset_PtYPhiM(m_embPt, m_embEta, m_embPhi);

  const int emb_index = particles.size();
  embjet.set_user_index(emb_index);
  particles_pseudojets.push_back(embjet);

  for (auto &p : particles) delete p;
  //centrality
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
  {
    std::cout
      << "MyJetAnalysis::process_event - Error can not find centrality node "
      << std::endl;
    exit(-1);
  }

  //get the event centrality/impact parameter from HIJING
  m_centrality  =  cent_node->get_centile(CentralityInfo::PROP::bimp);
  m_impactparam =  cent_node->get_quantity(CentralityInfo::PROP::bimp);


  if (Verbosity()>5) cout << "Starting background density calc" << endl;
  JetDefinition jet_def(kt_algorithm, 0.4);     //  JET DEFINITION

  const double ghost_max_rap { 2.0 };
  const double ghost_R = 0.01;
  const double jet_R = 0.4;

  AreaDefinition area_def_bkgd( active_area_explicit_ghosts, GhostedAreaSpec(ghost_max_rap, 1, ghost_R));
  JetDefinition jet_def_bkgd(kt_algorithm, jet_R); // <--
  Selector selector_rm2 = SelectorAbsEtaMax(0.6) * (!SelectorNHardest(1)); // <-- only remove the leading jet
  fastjet::JetMedianBackgroundEstimator bge_rm1 {selector_rm2, jet_def_bkgd, area_def_bkgd};
  bge_rm1.set_particles(particles_pseudojets);

  m_rho = bge_rm1.rho();
  m_rho_sigma = bge_rm1.sigma();

  if (Verbosity()>5) cout << "Starting clustered jets" << endl;
  // cluster the measured jets:
  double max_rap = 2.0;
  fastjet::Selector jetrap         = fastjet::SelectorAbsEtaMax(0.6);
  fastjet::Selector not_pure_ghost = !SelectorIsPureGhost();
  fastjet::Selector selection      = jetrap && not_pure_ghost;
  AreaDefinition area_def( active_area_explicit_ghosts, GhostedAreaSpec(max_rap, 1, ghost_R));
  JetDefinition jet_def_antikt(antikt_algorithm, jet_R);
  fastjet::ClusterSequenceArea clustSeq(particles_pseudojets, jet_def_antikt, area_def);
  vector<fastjet::PseudoJet> jets = sorted_by_pt( selection( clustSeq.inclusive_jets(m_ptRange.first) ));

  // find matches for the embedded jets (might not be any, just because eta cuts)
  if (jets.size()==0) {
    clear_vectors();
    return Fun4AllReturnCodes::EVENT_OK;
  }

  bool embtrack_found = false;
  for (auto& cst : jets[0].constituents()) {
    if (cst.user_index() == emb_index) {
      embtrack_found = true;
      break;
    }
  }
  if (!embtrack_found) {
    clear_vectors();
    return Fun4AllReturnCodes::EVENT_OK;
  }

  m_RhoBias = (jets[0].pt() - jets[0].area()*m_rho) - m_embPt;

  for (auto jet : jets) {
    m_CaloJetEta  .push_back( jet.eta());
    m_CaloJetPhi  .push_back( jet.phi_std());
    m_CaloJetE    .push_back( jet.E());
    m_CaloJetPt   .push_back( jet.pt());
    m_CaloJetArea .push_back( jet.area());
  }

  m_T->Fill();
  clear_vectors();
  return Fun4AllReturnCodes::EVENT_OK;
}

void JetPlusBackground::clear_vectors() {
  m_CaloJetEta.clear();
  m_CaloJetPhi.clear();
  m_CaloJetE.clear();
  m_CaloJetPt.clear();
  m_CaloJetArea.clear();
}

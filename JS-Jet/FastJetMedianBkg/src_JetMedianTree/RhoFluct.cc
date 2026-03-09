#include "RhoFluct.h"
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
#include <TH2D.h>
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
#include <jetbase/JetInput.h>
#include <jetbase/JetMapv1.h>
#include <iostream>
#include <limits>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <stdexcept>
#include <trackbase_historic/SvtxTrackMap.h>
#include <globalvertex/GlobalVertexMap.h>

// using namespace fastjet;

RhoFluct::RhoFluct
     ( const std::string& _outputfilename
     , const float        _jet_R   
     )
  : SubsysReco("RhoFluct")
  , m_outputfilename { _outputfilename }
  , jet_R            { _jet_R          }
{ }

RhoFluct::~RhoFluct()
{ 
  for (unsigned int i = 0; i < m_input_Sub1.size(); ++i) delete m_input_Sub1[i];
  m_input_Sub1.clear();

  for (unsigned int i = 0; i < m_input_rhoA.size(); ++i) delete m_input_rhoA[i];
  m_input_rhoA.clear();
}

int RhoFluct::Init(PHCompositeNode* topNode)
{
  if (Verbosity() >= RhoFluct::VERBOSITY_SOME)
    std::cout << "RhoFluct::Init - Outoput to " << m_outputfilename << std::endl;

  std::cout << " File out: " << m_outputfilename << std::endl;
  PHTFileServer::get().open(m_outputfilename, "RECREATE");

  //Tree
  m_T = new TTree("T", "RhoFluct Tree");

  //      int m_event;
  /* m_T->Branch("id",                &m_id); */
  m_T->Branch("rho",         &m_rho);
  m_T->Branch("rho_sigma",   &m_rho_sigma);
  m_T->Branch("cent",        &m_cent);
  m_T->Branch("cent_mdb",    &m_cent_mdb);
  m_T->Branch("cent_epd",    &m_cent_epd);
  m_T->Branch("impactparam", &m_impactparam);


  // embedded objects
  m_T->Branch("emb_1TeV_phi", &m_1TeV_phi);
  m_T->Branch("emb_1TeV_eta", &m_1TeV_eta);

  //Sub1 Jets
  m_T->Branch("sub1_ismatched", &m_Sub1_ismatched);
  m_T->Branch("sub1JetPhi",     &m_Sub1JetPhi);
  m_T->Branch("sub1JetEta",     &m_Sub1JetEta);
  m_T->Branch("sub1JetPt",      &m_Sub1JetPt);
  m_T->Branch("sub1Jet_delPt",  &m_Sub1Jet_delpt);

  //rhoA jets
  m_T->Branch("rhoA_ismatched", &m_rhoA_ismatched);
  m_T->Branch("rhoAJetPhi",     &m_rhoAJetPhi);
  m_T->Branch("rhoAJetEta",     &m_rhoAJetEta);
  m_T->Branch("rhoAJetPt",      &m_rhoAJetPt);
  m_T->Branch("rhoAJetArea",    &m_rhoAJetArea);
  m_T->Branch("rhoAJetPtLessRhoA",    &m_rhoAJetPtLessRhoA);
  m_T->Branch("rhoAJet_delPt",  &m_rhoAJet_delpt);

  return Fun4AllReturnCodes::EVENT_OK;
}

int RhoFluct::End(PHCompositeNode* topNode)
{
  std::cout << "RhoFluct::End - Output to " << m_outputfilename << std::endl;
  PHTFileServer::get().cd(m_outputfilename);

  m_T->Write();
  return Fun4AllReturnCodes::EVENT_OK;
}

int RhoFluct::InitRun(PHCompositeNode* topNode)
{
  topNode->print();
  std::cout << " Input Selections:" << std::endl;
  for (unsigned int i = 0; i < m_input_rhoA.size(); ++i) m_input_rhoA[i]->identify();
  for (unsigned int i = 0; i < m_input_Sub1.size(); ++i) m_input_Sub1[i]->identify();
  return Fun4AllReturnCodes::EVENT_OK;
}

int RhoFluct::process_event(PHCompositeNode* topNode)
{
  ++nevent;

  // cut for only event with good vertices
  GlobalVertexMap *mapVtx = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (mapVtx->size() == 0) return Fun4AllReturnCodes::EVENT_OK;

  // centrality
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (cent_node)
  {
    m_cent_epd  =  cent_node->get_centile(CentralityInfo::PROP::epd_NS);
    m_cent_mdb  =  cent_node->get_centile(CentralityInfo::PROP::mbd_NS);
    m_impactparam =  cent_node->get_quantity(CentralityInfo::PROP::bimp);
    m_cent =  cent_node->get_centile(CentralityInfo::PROP::bimp);
  }

  const double ghost_max_rap = 1.1;
  const double ghost_R = 0.01;
  const double max_jet_rap = 1.1; // - jet_R;

  //get the SUB1 jets
  m_1TeV_phi =  rand3.Uniform(-M_PI, M_PI);
  m_1TeV_eta =  rand3.Uniform(-0.7,  0.7);

  const int TOW_PRINT_INT = 8000;

  // build the towers for rhoA
  int n_inputs {0};
  std::vector<fastjet::PseudoJet> calo_pseudojets;
  for (auto inp : m_input_rhoA) {
    if (Verbosity()>TOW_PRINT_INT) std::cout << " Input Tower Set("<<n_inputs++<<"): ";
    float sum_pt {0.};
    float n_towers {0};
    std::vector<Jet *> particles = inp->get_input(topNode);
    for (unsigned int i = 0; i < particles.size(); ++i)
    {
      auto& part = particles[i];
      float this_e = part->get_e();
      if (this_e == 0.) continue;
      float this_px = particles[i]->get_px();
      float this_py = particles[i]->get_py();
      float this_pz = particles[i]->get_pz();
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
      /* if (pseudojet.pt() < 0.2) continue; */
      calo_pseudojets    .push_back(pseudojet);
      if (Verbosity() > TOW_PRINT_INT) {
        sum_pt += pseudojet.perp();
        ++n_towers;
      }
    }
    if (Verbosity()>TOW_PRINT_INT) std::cout << " sumpt(" << sum_pt <<") from ntowers("<<n_towers <<")" << std::endl;
    for (auto &p : particles) delete p;
  }
  // add the TeV particle
  auto embjet = fastjet::PseudoJet();
  embjet.reset_PtYPhiM(30., m_1TeV_eta, m_1TeV_phi);
  calo_pseudojets.push_back(embjet);

  fastjet::Selector jetrap         = fastjet::SelectorAbsEtaMax(max_jet_rap);
  fastjet::Selector not_pure_ghost = !fastjet::SelectorIsPureGhost();

  fastjet::Selector selection      = jetrap && not_pure_ghost;
  fastjet::AreaDefinition area_def( 
      fastjet::active_area_explicit_ghosts, fastjet::GhostedAreaSpec(ghost_max_rap, 1, ghost_R));

  fastjet::JetDefinition jet_def_antikt(fastjet::antikt_algorithm, jet_R);


  fastjet::JetDefinition jet_def_bkgd(fastjet::kt_algorithm, jet_R); // <--
  fastjet::Selector selector_rm2 = jetrap * (!fastjet::SelectorNHardest(2)); // <--
                                                                                                              //
  fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def};
  bge_rm2.set_particles(calo_pseudojets);

  m_rho = bge_rm2.rho();
  m_rho_sigma = bge_rm2.sigma();

  mBGE_mean_area    = bge_rm2 .mean_area();
  mBGE_empty_area   = bge_rm2 .empty_area();
  mBGE_n_empty_jets = bge_rm2 .n_empty_jets();
  mBGE_n_jets_used  = bge_rm2 .n_jets_used();

  fastjet::ClusterSequenceArea clustSeq(calo_pseudojets, jet_def_antikt, area_def);
  std::vector<fastjet::PseudoJet> jets 
    = sorted_by_pt( selection( clustSeq.inclusive_jets(5.) ));

  int fixmek = 0;
  if (Verbosity()>100 && m_cent_mdb<10) {
    fixmek = 0;
    std::cout << "  --- rhoA jets [set("<<outevent<<")]" << std::endl;
    for (auto jet : jets) {
      if (jet.perp()-m_rho*jet.area() > 5.) {
        auto phi = jet.phi();
        while (phi > M_PI) phi -= 2*M_PI;
         std::cout << Form("k[%i] pt:phi:eta[%5.2f:%5.2f:%5.2f]", fixmek++, (jet.perp()-m_rho*jet.area()), phi, jet.eta()) << std::endl;
      }
    }
  }

  // check that the leading jet is within R=0.4 of the embedded jet
  if (jets.size() == 0) {
    std::cout << " no jets reconstructed; something is very wrong" << std::endl;
    m_rhoA_ismatched    = false;
    m_rhoAJetPhi        = -100.;
    m_rhoAJetEta        = -100.;
    m_rhoAJetPt         = -100.;
    m_rhoAJetArea       = -100.;
    m_rhoAJetPtLessRhoA = -100.;
    m_rhoAJet_delpt     = -100.;
  } else {
    auto& lead_jet = jets[0];
    m_rhoAJetPhi = lead_jet.phi();
    m_rhoAJetEta = lead_jet.eta();
    m_rhoAJetPt  = lead_jet.perp();
    m_rhoAJetArea = lead_jet.area();
    m_rhoAJetPtLessRhoA = m_rhoAJetPt - m_rhoAJetArea*m_rho;

    float  dphi = TMath::Abs(lead_jet.phi_std() - m_1TeV_phi);
    while (dphi>M_PI) dphi = TMath::Abs(dphi-2*M_PI);
    float dR = TMath::Sq(dphi)+TMath::Sq(lead_jet.eta()-m_1TeV_eta);
    if (dR > 0.16) {
      m_rhoA_ismatched = false;
      m_rhoAJet_delpt = -100.;
    } else {
      m_rhoA_ismatched = true;
      m_rhoAJet_delpt = m_rhoAJetPtLessRhoA - 30.;
    }
  }

  // --------------------------------------
  // now make the jets with Sub1
  // --------------------------------------
  std::vector<fastjet::PseudoJet> sub1_pseudojets;
  for (auto inp : m_input_Sub1) {
    if (Verbosity()>TOW_PRINT_INT) std::cout << " Input Tower Set("<<n_inputs++<<"): ";
    float sum_pt {0.};
    float n_towers {0};
    std::vector<Jet *> particles = inp->get_input(topNode);
    for (unsigned int i = 0; i < particles.size(); ++i)
    {
      auto& part = particles[i];
      float this_e = part->get_e();
      if (this_e == 0.) continue;
      float this_px = particles[i]->get_px();
      float this_py = particles[i]->get_py();
      float this_pz = particles[i]->get_pz();
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
      /* if (pseudojet.pt() < 0.2) continue; */
      sub1_pseudojets    .push_back(pseudojet);
      if (Verbosity() > TOW_PRINT_INT) {
        sum_pt += pseudojet.perp();
        ++n_towers;
      }
    }
    if (Verbosity()>TOW_PRINT_INT) std::cout << " sumpt(" << sum_pt <<") from ntowers("<<n_towers <<")" << std::endl;
    for (auto &p : particles) delete p;
  }
  // add the TeV particle
  sub1_pseudojets.push_back(embjet);

  fastjet::ClusterSequenceArea clustSeq_sub1(sub1_pseudojets, jet_def_antikt, area_def);
  std::vector<fastjet::PseudoJet> jets_sub1 
    = sorted_by_pt( selection( clustSeq_sub1.inclusive_jets(5.) ));

  // check that the leading jet is within R=0.4 of the embedded jet
  if (jets_sub1.size() == 0) {
    std::cout << " no jets_sub1 reconstructed; something is very wrong" << std::endl;
    m_Sub1_ismatched    = false;
    m_Sub1JetPhi        = -100.;
    m_Sub1JetEta        = -100.;
    m_Sub1JetPt         = -100.;
    m_Sub1Jet_delpt     = -100.;
  } else {
    auto& lead_jet = jets_sub1[0];
    m_Sub1JetPhi = lead_jet.phi();
    m_Sub1JetEta = lead_jet.eta();
    m_Sub1JetPt  = lead_jet.perp();

    float  dphi = TMath::Abs(lead_jet.phi_std() - m_1TeV_phi);
    while (dphi>M_PI) dphi = TMath::Abs(dphi-2*M_PI);
    float dR = TMath::Sq(dphi)+TMath::Sq(lead_jet.eta()-m_1TeV_eta);
    if (dR > 0.16) {
      m_Sub1_ismatched = false;
      m_Sub1Jet_delpt = -100.;
    } else {
      m_Sub1_ismatched = true;
      m_Sub1Jet_delpt = m_Sub1JetPt - 30.;
    }
  }

  m_T->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}


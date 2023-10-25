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

JetRhoMedian::JetRhoMedian
     ( const std::string& _outputfilename
     , const float        _jet_R   
     , const std::string& truthjetname
     , const std::string& sub1jetname
     , const float min_lead_truth_pt
     )
  : SubsysReco("JetRhoMedian")
  , m_outputfilename { _outputfilename }
  , jet_R            { _jet_R          }
  , m_truthJetName   { truthjetname    }
  , m_sub1JetName    { sub1jetname     }
  , m_min_lead_truth_pt { min_lead_truth_pt }
  , m_T            { nullptr         }
  , m_inputs         {}
{ }

JetRhoMedian::~JetRhoMedian()
{ 
  for (unsigned int i = 0; i < m_inputs.size(); ++i) delete m_inputs[i];
  m_inputs.clear();
}

int JetRhoMedian::Init(PHCompositeNode* topNode)
{
  if (Verbosity() >= JetRhoMedian::VERBOSITY_SOME)
    std::cout << "JetRhoMedian::Init - Outoput to " << m_outputfilename << std::endl;

  std::cout << " File out: " << m_outputfilename << std::endl;
  PHTFileServer::get().open(m_outputfilename, "RECREATE");

  //Tree
  m_T = new TTree("T", "JetRhoMedian Tree");

  //      int m_event;
  /* m_T->Branch("id",                &m_id); */
  m_T->Branch("rho",               &m_rho);
  m_T->Branch("rho_sigma",         &m_rho_sigma);
  m_T->Branch("cent",          &m_cent);
  m_T->Branch("cent_mdb",          &m_cent_mdb);
  m_T->Branch("cent_epd",          &m_cent_epd);
  m_T->Branch("impactparam",       &m_impactparam);


  //Truth Jets
  m_T->Branch("TruthJetEta",  &m_TruthJetEta);
  m_T->Branch("TruthJetPhi",  &m_TruthJetPhi);
  m_T->Branch("TruthJetPt",   &m_TruthJetPt);
  
  //Sub1 Jets
  m_T->Branch("sub1JetEta",  &m_Sub1JetEta);
  m_T->Branch("sub1JetPhi",  &m_Sub1JetPhi);
  m_T->Branch("sub1JetPt",   &m_Sub1JetPt);

  m_T->Branch("rhoAJetEta",        &m_CaloJetEta);
  m_T->Branch("rhoAJetPhi",        &m_CaloJetPhi);
  m_T->Branch("rhoAJetPt",          &m_CaloJetPt);
  m_T->Branch("rhoAJetPtLessRhoA", &m_CaloJetPtLessRhoA);
  m_T->Branch("rhoAJetArea",       &m_CaloJetArea);

  return Fun4AllReturnCodes::EVENT_OK;
}

int JetRhoMedian::End(PHCompositeNode* topNode)
{
  std::cout << "JetRhoMedian::End - Output to " << m_outputfilename << std::endl;
  PHTFileServer::get().cd(m_outputfilename);

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
  clear_vectors();
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


  /* fastjet::JetDefinition jet_def(fastjet::kt_algorithm, 0.4);     //  JET DEFINITION */
  /* const double ghost_max_rap { 1.0 }; */

  /* fastjet::AreaDefinition area_def_bkgd( */ 
  /*     fastjet::active_area_explicit_ghosts, fastjet::GhostedAreaSpec(ghost_max_rap, 1, ghost_R)); */
  /* fastjet::Selector selector_rm2 = jetrap * (!fastjet::SelectorNHardest(2)); // <-- */

  //interface to truth jets
  JetMap* jetsMC = findNode::getClass<JetMap>(topNode, m_truthJetName);
  if (!jetsMC )
  {
    std::cout
      << "MyJetAnalysis::process_event - Error can not find DST Truth JetMap node "
      << m_truthJetName << std::endl;
    exit(-1);
  }

  bool isfirst = true;
  int fixmek = 0;
  auto vjets = jetsMC->vec();
  std::sort(vjets.begin(), vjets.end(), [](Jet* a, Jet* b){return a->get_pt() > b->get_pt();});
  if (Verbosity() > 100) {
    if (m_cent_mdb<10) {
      std::cout << " NEW EVENT[" << (outevent++) << "]" << std::endl;
      std::cout << " ---  Truth Jets [set(" << outevent << ")]" <<  std::endl;
    }
  }
  for (auto& jet : vjets) { 
    float pt  = jet->get_pt();
    if  (pt < 5.) continue;
    float eta = jet->get_eta();
    if (eta < -max_jet_rap || eta > max_jet_rap) continue;
    if (Verbosity()>100 && m_cent_mdb<10) {
      std::cout << Form("k[%i] pt:phi:eta[%5.2f:%5.2f:%5.2f]", fixmek++, jet->get_pt(), jet->get_phi(), jet->get_eta()) << std::endl;
    }

    if (isfirst) {
      isfirst = false;
      if (pt < m_min_lead_truth_pt) return Fun4AllReturnCodes::EVENT_OK;
    }
    m_TruthJetPt .push_back(jet->get_pt());
    m_TruthJetEta.push_back(jet->get_eta());
    m_TruthJetPhi.push_back(jet->get_phi());
  }

  //get the SUB1 jets
    if (Verbosity() > 100 && m_cent_mdb<10) {
      std::cout << "  --- Sub1 Jets [set(" << outevent << ")]" <<  std::endl;
       fixmek = 0;
    }
  JetMap* jets_sub1 = findNode::getClass<JetMap>(topNode, m_sub1JetName);
  if (jets_sub1 != nullptr) {
    vjets = jets_sub1->vec();
    std::sort(vjets.begin(), vjets.end(), [](Jet* a, Jet* b){return a->get_pt() > b->get_pt();});
    fixmek = 0;
    if (Verbosity()>100 && m_cent_mdb<10) std::cout << "SUB1 jets" << std::endl;
    for (auto& jet : jets_sub1->vec()) {
      // will return jets in order of descending pT
      float pt  = jet->get_pt();
      float eta = jet->get_eta();
      if (eta < -max_jet_rap || eta > max_jet_rap) continue;
      if  (pt < 5.) continue;
      if (Verbosity()>100 && m_cent_mdb<10) std::cout << Form("k[%i] pt:phi:eta[%5.2f:%5.2f:%5.2f]", fixmek++, jet->get_pt(), jet->get_phi(), jet->get_eta()) << std::endl;
      m_Sub1JetPt .push_back(jet->get_pt());
      m_Sub1JetEta.push_back(jet->get_eta());
      m_Sub1JetPhi.push_back(jet->get_phi());
    }
  }

  const int TOW_PRINT_INT = 8000;

  int n_inputs {0};
  std::vector<fastjet::PseudoJet> calo_pseudojets;
  for (auto inp : m_inputs) {
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

  for (auto jet : jets) {
    if (false && Verbosity() >= JetRhoMedian::VERBOSITY_SOME) {
        auto cst = fastjet::sorted_by_pt(jet.constituents());
        int i =0;
        for (auto p : cst)  {
          if (p.is_pure_ghost()) continue;
          std::cout << Form(" %3i|%4.2f",i++,p.perp());
          if (i%7==0) std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    m_CaloJetEta        .push_back( jet.eta()     );
    m_CaloJetPhi        .push_back( jet.phi_std() );
    m_CaloJetPt         .push_back( jet.pt()      );
    m_CaloJetPtLessRhoA .push_back( jet.pt()      - m_rho * jet.area());
    m_CaloJetArea       .push_back( jet.area());
  }


  m_T->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}

void JetRhoMedian::clear_vectors() {
  /* if (nevent % 1 == 0) cout << " z0 " << m_CaloJetArea.size(); */
  m_CaloJetEta        .clear();
  m_CaloJetPhi        .clear();
  m_CaloJetPt          .clear();
  m_CaloJetPtLessRhoA .clear();
  m_CaloJetArea       .clear();
  
  /* if (nevent % 1 == 0) cout << " " << m_TruthJetPt.size(); */
  m_TruthJetEta       .clear();
  m_TruthJetPhi       .clear();
  m_TruthJetPt        .clear();

  m_Sub1JetEta       .clear();
  m_Sub1JetPhi       .clear();
  m_Sub1JetPt        .clear();

}

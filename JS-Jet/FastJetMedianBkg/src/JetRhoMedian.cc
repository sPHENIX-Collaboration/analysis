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
  , m_ptRange           { 5.,    100 }
  , m_T                 { nullptr        }
  , m_pTlb              { pTlb }
  , m_inputs            {}
  , print_stats         { n_print_freq,  total_jobs }
{ 
  cout << " n_print_freq: " << n_print_freq << endl;
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

  // Some histograms on the sizes of the particles comming in
  h2d_nHcalOut_vs_cen = new TH2D("nHcalOut_vs_Cen",";centrality;N_{HCALOut Towers}",
   10, 0, 100., 250, 0., 20000.);
  h2d_nHcalIn_vs_cen = new TH2D("nHcalIn_vs_Cen",";centrality;N_{HCALOut Towers}",
   10, 0., 100., 250, 0., 20000.);
  h2d_nCEMC_vs_cen = new TH2D("nCEMC_vs_Cen",";centrality;N_{CEMC Towers}",
   10, 0., 100., 250, 0., 20000.);
  arr_n_v_cen = { h2d_nCEMC_vs_cen, h2d_nHcalIn_vs_cen, h2d_nHcalOut_vs_cen };


  //Tree
  m_T = new TTree("T", "JetRhoMedian Tree");

  //      int m_event;
  /* m_T->Branch("id",                &m_id); */
  m_T->Branch("pTlb",              &m_pTlb, "Lower bound on pT. 0 for MB, 10 for jets >10, 30 for jets > 30");
  m_T->Branch("rho",               &m_rho);
  m_T->Branch("rho_HCALIn",        &m_rho_HCALIn);
  m_T->Branch("rho_HCALOut",       &m_rho_HCALOut);
  m_T->Branch("rho_HCAL",          &m_rho_HCAL);
  m_T->Branch("rho_EMCAL",         &m_rho_EMCAL);
  m_T->Branch("rho_sigma",         &m_rho_sigma);
  m_T->Branch("cent_mdb",          &m_cent_mdb);
  m_T->Branch("cent_epd",          &m_cent_epd);
  m_T->Branch("impactparam",       &m_impactparam);

  m_T->Branch("CaloJetEta",        &m_CaloJetEta);
  m_T->Branch("CaloJetPhi",        &m_CaloJetPhi);
  m_T->Branch("CaloJetE",          &m_CaloJetE);
  m_T->Branch("CaloJetPtLessRhoA", &m_CaloJetPtLessRhoA);
  m_T->Branch("CaloJetArea",       &m_CaloJetArea);

  //HCALin-In
  m_T->Branch("HCALInJetEta",         &m_HCALInJetEta);
  m_T->Branch("HCALInJetPhi",         &m_HCALInJetPhi);
  m_T->Branch("HCALInJetE",           &m_HCALInJetE);
  m_T->Branch("HCALInJetPtLessRhoA",  &m_HCALInJetPtLessRhoA);
  m_T->Branch("HCALInJetArea",        &m_HCALInJetArea);

  //HCALin-Out
  m_T->Branch("HCALOutJetEta",        &m_HCALOutJetEta);
  m_T->Branch("HCALOutJetPhi",        &m_HCALOutJetPhi);
  m_T->Branch("HCALOutJetE",          &m_HCALOutJetE);
  m_T->Branch("HCALOutJetPtLessRhoA", &m_HCALOutJetPtLessRhoA);
  m_T->Branch("HCALOutJetArea",       &m_HCALOutJetArea);

  //HCAL jets in+out
  m_T->Branch("HCALJetEta",           &m_HCALJetEta);
  m_T->Branch("HCALJetPhi",           &m_HCALJetPhi);
  m_T->Branch("HCALJetE",             &m_HCALJetE);
  m_T->Branch("HCALJetPtLessRhoA",    &m_HCALJetPtLessRhoA);
  m_T->Branch("HCALJetArea",          &m_HCALJetArea);
  
  //EMCal jets 
  m_T->Branch("EMCALJetEta",          &m_EMCALJetEta);
  m_T->Branch("EMCALJetPhi",          &m_EMCALJetPhi);
  m_T->Branch("EMCALJetE",            &m_EMCALJetE);
  m_T->Branch("EMCALJetPtLessRhoA",   &m_EMCALJetPtLessRhoA);
  m_T->Branch("EMCALJetArea",         &m_EMCALJetArea);

  //Truth Jets
  m_T->Branch("TruthJetEta",  &m_TruthJetEta);
  m_T->Branch("TruthJetPhi",  &m_TruthJetPhi);
  m_T->Branch("TruthJetE",    &m_TruthJetE);
  m_T->Branch("TruthJetPt",   &m_TruthJetPt);
  m_T->Branch("TruthJetArea", &m_TruthJetArea);

  m_T->Branch("CEMCtowEta"    , &m_CEMCtowEta    );
  m_T->Branch("CEMCtowPhi"    , &m_CEMCtowPhi    );
  m_T->Branch("CEMCtowPt"     , &m_CEMCtowPt     );
  m_T->Branch("HCALINtowEta"  , &m_HCALINtowEta  );
  m_T->Branch("HCALINtowPhi"  , &m_HCALINtowPhi  );
  m_T->Branch("HCALINtowPt"   , &m_HCALINtowPt   );
  m_T->Branch("HCALOUTtowEta" , &m_HCALOUTtowEta );
  m_T->Branch("HCALOUTtowPhi" , &m_HCALOUTtowPhi );
  m_T->Branch("HCALOUTtowPt"  , &m_HCALOUTtowPt  );

  // background kT jets
  m_T->Branch("BGE_JetEta"            ,        &m_BGE_JetEta      );
  m_T->Branch("BGE_JetPhi"            ,        &m_BGE_JetPhi      );
  m_T->Branch("BGE_JetPt"             ,        &m_BGE_JetPt       );
  m_T->Branch("BGE_JetArea"           ,        &m_BGE_JetArea     );

  m_T->Branch("BGE_mean_area"         ,        &mBGE_mean_area    );
  m_T->Branch("BGE_empty_area"        ,        &mBGE_empty_area   );
  m_T->Branch("BGE_n_empty_jets"      ,        &mBGE_n_empty_jets );
  m_T->Branch("BGE_n_jets_used"       ,        &mBGE_n_jets_used  );

  return Fun4AllReturnCodes::EVENT_OK;
}

int JetRhoMedian::End(PHCompositeNode* topNode)
{
  std::cout << "JetRhoMedian::End - Output to " << m_outputFileName << std::endl;
  PHTFileServer::get().cd(m_outputFileName);

  /* m_hInclusiveE->Write(); */
  /* m_hInclusiveEta->Write(); */
  /* m_hInclusivePhi->Write(); */
  for (auto& h : arr_n_v_cen) h->Write();
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
  ++nevent;

  // centrality
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (cent_node)
  {
    m_cent_epd  =  cent_node->get_centile(CentralityInfo::PROP::epd_NS);
    m_cent_mdb  =  cent_node->get_centile(CentralityInfo::PROP::mbd_NS);
    // CentralityInfo::mbd_NS -ish
    m_impactparam =  cent_node->get_quantity(CentralityInfo::PROP::bimp);

    /* cout << " cent: " */ 
    /*   << " " << cent_node->get_centile  (CentralityInfo::PROP::bimp) */
    /*   << " " << cent_node->get_quantity (CentralityInfo::PROP::bimp) */
    /*   << " " << cent_node->get_centile  (CentralityInfo::PROP::epd_NS) */
    /*   << " " << cent_node->get_quantity (CentralityInfo::PROP::epd_NS) */
    /*   << " " << cent_node->get_centile  (CentralityInfo::PROP::mbd_NS) */
    /*   << " " << cent_node->get_quantity (CentralityInfo::PROP::mbd_NS) */
      /* << " " << cent_node->get_centile  (CentralityInfo::PROP::mbd_N) */
      /* << " " << cent_node->get_quantity (CentralityInfo::PROP::mbd_N) */
      /* << " " << cent_node->get_centile  (CentralityInfo::PROP::mbd_S) */
      /* << " " << cent_node->get_quantity (CentralityInfo::PROP::mbd_S) << endl; */
  }

  const double ghost_max_rap = 1.0;
  const double ghost_R = 0.01;
  const double jet_R = 0.4;

  fastjet::Selector jetrap         = fastjet::SelectorAbsEtaMax(0.6);
  fastjet::Selector not_pure_ghost = !fastjet::SelectorIsPureGhost();
  fastjet::Selector selection      = jetrap && not_pure_ghost;
  fastjet::AreaDefinition area_def( 
      fastjet::active_area_explicit_ghosts, fastjet::GhostedAreaSpec(ghost_max_rap, 1, ghost_R));
  fastjet::JetDefinition jet_def_antikt(fastjet::antikt_algorithm, jet_R);

  /* fastjet::JetDefinition jet_def(fastjet::kt_algorithm, 0.4);     //  JET DEFINITION */
  /* const double ghost_max_rap { 1.0 }; */

  /* fastjet::AreaDefinition area_def_bkgd( */ 
  /*     fastjet::active_area_explicit_ghosts, fastjet::GhostedAreaSpec(ghost_max_rap, 1, ghost_R)); */
  fastjet::JetDefinition jet_def_bkgd(fastjet::kt_algorithm, jet_R); // <--
  /* fastjet::Selector selector_rm2 = jetrap * (!fastjet::SelectorNHardest(2)); // <-- */
  fastjet::Selector selector_rm2 = fastjet::SelectorAbsEtaMax(0.6) * (!fastjet::SelectorNHardest(2)); // <--

  //interface to truth jets
  JetMap* jetsMC = findNode::getClass<JetMap>(topNode, m_truthJetName);
  if (!jetsMC )
  {
    std::cout
      << "MyJetAnalysis::process_event - Error can not find DST Truth JetMap node "
      << m_truthJetName << std::endl;
    exit(-1);
  }

  for (auto& jet : jetsMC->vec(Jet::SORT::PT)) {  // will return jets in order of descending pT
    float pt  = jet->get_pt();
    float eta = jet->get_eta();
    if  (pt < m_ptRange.first
        || pt  > m_ptRange.second
        || eta < m_etaRange.first
        || eta > m_etaRange.second) continue;
    m_TruthJetPt .push_back(jet->get_pt());
    m_TruthJetEta.push_back(jet->get_eta());
    m_TruthJetPhi.push_back(jet->get_phi());
    m_TruthJetE  .push_back(jet->get_e());
  }

  // get the inputs for reconstructed jets (from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.cc)
  // get the four vectors of jets to clusters
  vector<fastjet::PseudoJet> calo_pseudojets, HCALIn_pseudojets, 
    HCALOut_pseudojets, HCAL_pseudojets, EMCAL_pseudojets;

  // fill in the tower information
  bool fill_pseudojets = true;
  if (fill_pseudojets) {
    /* std::vector<fastjet::PseudoJet> particles_pseudojets; */
    for (int K=0;K<3;++K) {
      std::vector<Jet *> particles = m_inputs[K]->get_input(topNode);
      std::vector<float>& towEta = (K==0) ? m_CEMCtowEta : (K==1) ? m_HCALINtowEta : m_HCALOUTtowEta;
      std::vector<float>& towPhi = (K==0) ? m_CEMCtowPhi : (K==1) ? m_HCALINtowPhi : m_HCALOUTtowPhi;
      std::vector<float>& towPt  = (K==0) ? m_CEMCtowPt  : (K==1) ? m_HCALINtowPt  : m_HCALOUTtowPt;
      arr_n_v_cen[K]->Fill(m_cent_mdb, particles.size());
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
        if (pseudojet.pt() < 0.2) continue;
        towEta .push_back( pseudojet.eta() );
        towPhi .push_back( pseudojet.phi() );
        towPt  .push_back( pseudojet.pt()  );

        calo_pseudojets    .push_back(pseudojet);
        if (K==0)         EMCAL_pseudojets   .push_back(pseudojet);
        if (K==1 || K==2) HCAL_pseudojets    .push_back(pseudojet);
        if (K==1)         HCALIn_pseudojets  .push_back(pseudojet);
        if (K==2)         HCALOut_pseudojets .push_back(pseudojet);
      }
      /* cout << " FIXME A0 size("<< calo_pseudojets.size()<<") K("<<K<<") calo_pseudojets: "; */
      /* for (auto& jet : calo_pseudojets) cout << " " << jet.perp(); */
      /* cout << endl; */
      for (auto &p : particles) delete p;
      /* cout << " FIXME A1 size("<< calo_pseudojets.size()<<") K("<<K<<") calo_pseudojets: "; */
      /* for (auto& jet : calo_pseudojets) cout << " " << jet.perp(); */
      /* cout << endl; */
    }
  }

  bool do_calo_jets = true;
  if (do_calo_jets) 
  {
    fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def};
    bge_rm2.set_particles(calo_pseudojets);

    m_rho = bge_rm2.rho();
    m_rho_sigma = bge_rm2.sigma();

    mBGE_mean_area    = bge_rm2 .mean_area();
    mBGE_empty_area   = bge_rm2 .empty_area();
    mBGE_n_empty_jets = bge_rm2 .n_empty_jets();
    mBGE_n_jets_used  = bge_rm2 .n_jets_used();

    for (auto& jet : bge_rm2.jets_used()) {
      m_BGE_JetEta  .push_back(jet.eta()  ) ;
      m_BGE_JetPhi  .push_back(jet.phi()  ) ;
      m_BGE_JetPt   .push_back(jet.pt()   ) ;
      m_BGE_JetArea .push_back(jet.area() ) ;
    }

  int cnt_out = 0;

    fastjet::ClusterSequenceArea clustSeq(calo_pseudojets, jet_def_antikt, area_def);
    vector<fastjet::PseudoJet> jets 
      = sorted_by_pt( selection( clustSeq.inclusive_jets(m_ptRange.first) ));
    for (auto jet : jets) {
      if (Verbosity() >= JetRhoMedian::VERBOSITY_SOME) {
        if (cnt_out < 4) {
          cout << " jet("<<(cnt_out++)<<") pt("<<jet.perp()<<") " << endl;
          auto cst = fastjet::sorted_by_pt(jet.constituents());
          int i =0;
          for (auto p : cst)  {
            if (p.is_pure_ghost()) continue;
            cout << Form(" %3i|%4.2f",i++,p.perp());
            if (i%7==0) cout << endl;
          }
          cout << endl;
        }
      }

      m_CaloJetEta .push_back( jet.eta()     );
      m_CaloJetPhi .push_back( jet.phi_std() );
      m_CaloJetE   .push_back( jet.E()       );
      m_CaloJetPtLessRhoA   .push_back( jet.pt() - m_rho * jet.area());
      m_CaloJetArea .push_back( jet.area());
    }
  }

  // get the background and jets for HCAL-In Jets
  bool do_EMCal = true;
  if (do_EMCal) { 
    fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def};
    bge_rm2.set_particles(EMCAL_pseudojets);
    /* cout << " a2 " << bge_rm2.jets_used().size() << endl; */
    m_rho_EMCAL = bge_rm2.rho();

    // make the jets
    fastjet::ClusterSequenceArea clustSeq(EMCAL_pseudojets, jet_def_antikt, area_def);
    vector<fastjet::PseudoJet> jets = 
      sorted_by_pt( selection( clustSeq.inclusive_jets(m_ptRange.first) ));
    /* cout << " a3 " << jets.size() << endl; */
    for (auto jet : jets) {
      m_EMCALJetEta .push_back( jet.eta()     );
      m_EMCALJetPhi .push_back( jet.phi_std() );
      m_EMCALJetE   .push_back( jet.E()       );
      m_EMCALJetPtLessRhoA   .push_back( jet.pt() - m_rho_EMCAL * jet.area());
      m_EMCALJetArea .push_back( jet.area());
    }
  }

  bool do_HCALIn = true;
  if (do_HCALIn) { 
    fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def};
    bge_rm2.set_particles(HCALIn_pseudojets);
    /* cout << " a4 " << bge_rm2.jets_used().size() << endl; */
    m_rho_HCALIn = bge_rm2.rho();

    // make the jets
    fastjet::ClusterSequenceArea clustSeq(HCALIn_pseudojets, jet_def_antikt, area_def);
    vector<fastjet::PseudoJet> jets = 
      sorted_by_pt( selection( clustSeq.inclusive_jets(m_ptRange.first) ));
    /* cout << " a5 " << jets.size() << endl; */
    for (auto jet : jets) {
      m_HCALInJetEta .push_back( jet.eta()     );
      m_HCALInJetPhi .push_back( jet.phi_std() );
      m_HCALInJetE   .push_back( jet.E()       );
      m_HCALInJetPtLessRhoA   .push_back( jet.pt() - m_rho_HCALIn * jet.area());
      m_HCALInJetArea .push_back( jet.area());
    }
  }

  bool do_HCALOut= true;
  if (do_HCALOut) { 
    fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def};
    bge_rm2.set_particles(HCALOut_pseudojets);
    /* cout << " a6 " << bge_rm2.jets_used().size() << endl; */
    m_rho_HCALOut = bge_rm2.rho();

    // make the jets
    fastjet::ClusterSequenceArea clustSeq(HCALOut_pseudojets, jet_def_antikt, area_def);
    vector<fastjet::PseudoJet> jets = 
      sorted_by_pt( selection( clustSeq.inclusive_jets(m_ptRange.first) ));
    /* cout << " a7 " << jets.size() << endl; */
    for (auto jet : jets) {
      m_HCALOutJetEta .push_back( jet.eta()     );
      m_HCALOutJetPhi .push_back( jet.phi_std() );
      m_HCALOutJetE   .push_back( jet.E()       );
      m_HCALOutJetPtLessRhoA   .push_back( jet.pt() - m_rho_HCALOut * jet.area());
      m_HCALOutJetArea .push_back( jet.area());
    }
  }

  bool do_HCAL = true;
  if (do_HCAL ) { 
    fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def};
    bge_rm2.set_particles(HCAL_pseudojets);
    /* cout << " a8 " << bge_rm2.jets_used().size() << endl; */
    /* m_rho_EMCAL = bge_rm2.rho(); */
    m_rho_HCAL = bge_rm2.rho();

    // make the jets
    fastjet::ClusterSequenceArea clustSeq(HCAL_pseudojets, jet_def_antikt, area_def);
    vector<fastjet::PseudoJet> jets = 
      sorted_by_pt( selection( clustSeq.inclusive_jets(m_ptRange.first) ));
    /* if (nevent % 10 == 0) cout << " a20 " << jets.size() << endl; */
    /* cout << " a9 " << jets.size() << endl; */
    for (auto jet : jets) {
      m_HCALJetEta .push_back( jet.eta()     );
      m_HCALJetPhi .push_back( jet.phi_std() );
      m_HCALJetE   .push_back( jet.E()       );
      m_HCALJetPtLessRhoA   .push_back( jet.pt() - m_rho_HCAL * jet.area());
      m_HCALJetArea .push_back( jet.area());
    }
  }

  m_T->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}

void JetRhoMedian::clear_vectors() {
  /* if (nevent % 1 == 0) cout << " z0 " << m_CaloJetArea.size(); */
  m_CaloJetEta        .clear();
  m_CaloJetPhi        .clear();
  m_CaloJetE          .clear();
  m_CaloJetPtLessRhoA .clear();
  m_CaloJetArea       .clear();
  
  /* if (nevent % 1 == 0) cout << " " << m_TruthJetPt.size(); */
  m_TruthJetEta       .clear();
  m_TruthJetPhi       .clear();
  m_TruthJetE         .clear();
  m_TruthJetPt        .clear();
  m_TruthJetArea      .clear();

  /* if (nevent % 1 == 0) cout << " " << m_HCALInJetArea.size(); */
  m_HCALInJetEta        .clear();
  m_HCALInJetPhi        .clear();
  m_HCALInJetE          .clear();
  m_HCALInJetPtLessRhoA .clear();
  m_HCALInJetArea       .clear();
  
  
  /* if (nevent % 1 == 0) cout << " " << m_HCALOutJetArea.size(); */
  m_HCALOutJetEta       .clear();
  m_HCALOutJetPhi       .clear();
  m_HCALOutJetE         .clear();
  m_HCALOutJetPtLessRhoA.clear();
  m_HCALOutJetArea      .clear();
  
  
  /* if (nevent % 1 == 0) cout << " " << m_HCALJetArea.size(); */
  m_HCALJetEta          .clear();
  m_HCALJetPhi          .clear();
  m_HCALJetE            .clear();
  m_HCALJetPtLessRhoA   .clear();
  m_HCALJetArea         .clear();
  
  
  /* if (nevent % 1 == 0) cout << " " << m_EMCALJetArea.size(); */
  m_EMCALJetEta        .clear();
  m_EMCALJetPhi        .clear();
  m_EMCALJetE          .clear();
  m_EMCALJetPtLessRhoA .clear();
  m_EMCALJetArea       .clear();

  /* if (nevent % 1 == 0) cout << " " << m_BGE_JetArea.size(); */
  m_BGE_JetEta  .clear();
  m_BGE_JetPhi  .clear();
  m_BGE_JetPt   .clear();
  m_BGE_JetArea .clear();

  /* if (nevent % 1 == 0) cout << " " << m_CEMCtowPt.size(); */
  m_CEMCtowEta        .clear();
  m_CEMCtowPhi        .clear();
  m_CEMCtowPt         .clear();

  /* if (nevent % 1 == 0) cout << " " << m_HCALINtowPt.size(); */
  m_HCALINtowEta      .clear();
  m_HCALINtowPhi      .clear();
  m_HCALINtowPt       .clear();

  /* if (nevent % 1 == 0) cout << " " << m_HCALOUTtowPt.size() << endl; */
  m_HCALOUTtowEta     .clear();
  m_HCALOUTtowPhi     .clear();
  m_HCALOUTtowPt      .clear();
    
}

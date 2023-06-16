#include "RhoMedianFluct.h"

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
#include <jetbase/JetMapv1.h>
#include <jetbase/JetInput.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TString.h>
#include <TTree.h>
#include <TH2D.h>
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

RhoMedianFluct::RhoMedianFluct
    ( const std::string& outputfilename
    , const int n_print_freq
    , const int total_jobs
    , const double _min_calo_pt
    , const std::string& recojetname
    )
: SubsysReco("JetPlusBackground_" + recojetname )
  , min_calo_pt {_min_calo_pt}
, m_recoJetName(recojetname)
  , m_outputFileName(outputfilename)
  , m_ptRange     (5,  100)
  , m_T           (nullptr)
  , m_CaloJetPtLessRhoA   {}
  , _inputs        {}
  , print_stats{n_print_freq, total_jobs}
  , rng {}
{ 
}

RhoMedianFluct::~RhoMedianFluct()
{ 
  /* cout << " fixme a9 " << endl; */
  for (unsigned int i = 0; i < _inputs.size(); ++i) delete _inputs[i];
  /* cout << " fixme a10 " << endl; */
  _inputs.clear();
  print_stats.set_get_stats();
  cout << " Max memory used: " << print_stats.max_mem/1000. << " MB " << endl;

}

int RhoMedianFluct::Init(PHCompositeNode* topNode)
{
  /* cout << " fixme a0 " << endl; */
  if (Verbosity() >= RhoMedianFluct::VERBOSITY_SOME)
    cout << "RhoMedianFluct::Init - Output to " << m_outputFileName << endl;

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
  m_T = new TTree("T", "RhoMedianFluct Tree");
  /* cout << " fixme a1 " << endl; */

  m_T->Branch("cent_mdb",    &m_cent_mdb);
  m_T->Branch("cent_epd",    &m_cent_epd);
  m_T->Branch("impactparam", &m_impactparam);

  m_T->Branch("embEta", &m_embEta);
  m_T->Branch("embPhi", &m_embPhi);

  m_T->Branch("embPt",         &m_embPt);
  m_T->Branch("embPt_HCALIn",  &m_embPt_HCALIn);
  m_T->Branch("embPt_HCALOut", &m_embPt_HCALOut);
  m_T->Branch("embPt_HCAL",    &m_embPt_HCAL);
  m_T->Branch("embPt_EMCAL",   &m_embPt_EMCAL);

  m_T->Branch("fluct",         &m_fluct         );
  m_T->Branch("fluct_HCALIn",  &m_fluct_HCALIn  );
  m_T->Branch("fluct_HCALOut", &m_fluct_HCALOut );
  m_T->Branch("fluct_HCAL",    &m_fluct_HCAL    );
  m_T->Branch("fluct_EMCAL",   &m_fluct_EMCAL   );

  m_T->Branch("rho",         &m_rho         );
  m_T->Branch("rho_HCALIn",  &m_rho_HCALIn  );
  m_T->Branch("rho_HCALOut", &m_rho_HCALOut );
  m_T->Branch("rho_HCAL",    &m_rho_HCAL    );
  m_T->Branch("rho_EMCAL",   &m_rho_EMCAL   );

  m_T->Branch("rhosigma",         &m_rhosigma);
  m_T->Branch("rhosigma_HCALIn",  &m_rhosigma_HCALIn);
  m_T->Branch("rhosigma_HCALOut", &m_rhosigma_HCALOut);
  m_T->Branch("rhosigma_HCAL",    &m_rhosigma_HCAL);
  m_T->Branch("rhosigma_EMCAL",   &m_rhosigma_EMCAL);

  // jets, call calorimeters
  m_T->Branch("CaloJetEta",           &m_CaloJetEta);
  m_T->Branch("CaloJetPhi",           &m_CaloJetPhi);
  m_T->Branch("CaloJetE",             &m_CaloJetE);
  m_T->Branch("CaloJetPtLessRhoA",    &m_CaloJetPtLessRhoA);
  m_T->Branch("CaloJetArea",          &m_CaloJetArea);

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

  // Background jet statistics
  m_T->Branch("BGE_mean_area"         ,        &mBGE_mean_area    );
  m_T->Branch("BGE_empty_area"        ,        &mBGE_empty_area   );
  m_T->Branch("BGE_n_empty_jets"      ,        &mBGE_n_empty_jets );
  m_T->Branch("BGE_n_jets_used"       ,        &mBGE_n_jets_used  );

  // background jets (all calorimeters)
  m_T->Branch("BGE_JetEta"            ,        &m_BGE_JetEta      );
  m_T->Branch("BGE_JetPhi"            ,        &m_BGE_JetPhi      );
  m_T->Branch("BGE_JetPt"             ,        &m_BGE_JetPt       );
  m_T->Branch("BGE_JetArea"           ,        &m_BGE_JetArea     );

  m_T->Branch("CEMCtowEta"    , &m_CEMCtowEta    );
  m_T->Branch("CEMCtowPhi"    , &m_CEMCtowPhi    );
  m_T->Branch("CEMCtowPt"     , &m_CEMCtowPt     );

  m_T->Branch("HCALINtowEta"  , &m_HCALINtowEta  );
  m_T->Branch("HCALINtowPhi"  , &m_HCALINtowPhi  );
  m_T->Branch("HCALINtowPt"   , &m_HCALINtowPt   );

  m_T->Branch("HCALOUTtowEta" , &m_HCALOUTtowEta );
  m_T->Branch("HCALOUTtowPhi" , &m_HCALOUTtowPhi );
  m_T->Branch("HCALOUTtowPt"  , &m_HCALOUTtowPt  );

  /* cout << " fixme a2 " << endl; */
  return Fun4AllReturnCodes::EVENT_OK;
}

int RhoMedianFluct::End(PHCompositeNode* topNode)
{
  /* cout << " PEAR fixme z0 " << endl; */
  cout << "RhoMedianFluct::End - Output to " << m_outputFileName << endl;
  PHTFileServer::get().cd(m_outputFileName);
  for (auto& h : arr_n_v_cen) h->Write();
  m_T->Write();
  return Fun4AllReturnCodes::EVENT_OK;
}

int RhoMedianFluct::InitRun(PHCompositeNode* topNode)
{
  topNode->print();
  cout << " Input Selections:" << endl;
  for (unsigned int i = 0; i < _inputs.size(); ++i) _inputs[i]->identify();
  return Fun4AllReturnCodes::EVENT_OK;
}

int RhoMedianFluct::process_event(PHCompositeNode* topNode)
{
  /* cout << " fixme a3 " << endl; */
  print_stats.call();
  clear_vectors();
  ++nevent;

  // centrality
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (cent_node)
  {
    m_cent_epd  =  cent_node->get_centile(CentralityInfo::PROP::epd_NS);
    m_cent_mdb  =  cent_node->get_centile(CentralityInfo::PROP::mbd_NS);
    m_impactparam =  cent_node->get_quantity(CentralityInfo::PROP::bimp);
    /* cout << " cent: " */ 
    /*   << " " << cent_node->get_centile  (CentralityInfo::PROP::bimp) */
    /*   << " " << cent_node->get_quantity (CentralityInfo::PROP::bimp) */
    /*   << " " << cent_node->get_centile  (CentralityInfo::PROP::epd_NS) */
    /*   << " " << cent_node->get_quantity (CentralityInfo::PROP::epd_NS) */
    /*   << " " << cent_node->get_centile  (CentralityInfo::PROP::mbd_NS) */
    /*   << " " << cent_node->get_quantity (CentralityInfo::PROP::mbd_NS) */
    /*   << " " << cent_node->get_centile  (CentralityInfo::PROP::mbd_N) */
    /*   << " " << cent_node->get_quantity (CentralityInfo::PROP::mbd_N) */
    /*   << " " << cent_node->get_centile  (CentralityInfo::PROP::mbd_S) */
    /*   << " " << cent_node->get_quantity (CentralityInfo::PROP::mbd_S) << endl; */
    /* m_centrality  =  cent_node->get_centile(CentralityInfo::PROP::mbd_N); */
    /* m_impactparam =  cent_node->get_quantity(CentralityInfo::PROP::mbd_N); */
    /* cout << " __ a0 __ " << m_centrality << " " << m_impactparam << endl; */
  }  else {
    std::cout << " Failed to get centrality. Skipping event" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
 
  // embedding common to all files
  m_embPhi = rng.Uniform(-M_PI, M_PI);
  m_embEta = rng.Uniform(-0.6, 0.6);

  const double ghost_max_rap = 1.0;
  const double ghost_R = 0.01;
  const double jet_R = 0.4;
  /* cout << " fixme a4 " << endl; */

  fastjet::Selector jetrap         = fastjet::SelectorAbsEtaMax(0.6);
  fastjet::Selector not_pure_ghost = !fastjet::SelectorIsPureGhost();
  fastjet::Selector selection      = jetrap && not_pure_ghost;
  fastjet::AreaDefinition area_def( 
      fastjet::active_area_explicit_ghosts, fastjet::GhostedAreaSpec(ghost_max_rap, 1, ghost_R));
  fastjet::JetDefinition jet_def_antikt(fastjet::antikt_algorithm, jet_R);

  fastjet::JetDefinition jet_def_bkgd(fastjet::kt_algorithm, jet_R); // <--
  fastjet::Selector      selector_rm2 = fastjet::SelectorAbsEtaMax(0.6) * (!fastjet::SelectorNHardest(2)); // <--

  // get the inputs for reconstructed jets (from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.cc)
  // get the four vectors of jets to clusters
  vector<fastjet::PseudoJet> calo_pseudojets, HCALIn_pseudojets, 
    HCALOut_pseudojets, HCAL_pseudojets, EMCAL_pseudojets;

  // fill in the tower information
  bool fill_pseudojets = true;
  if (fill_pseudojets) {
    /* std::vector<fastjet::PseudoJet> particles_pseudojets; */
    for (int K=0;K<3;++K) {
      std::vector<Jet *> _particles = _inputs[K]->get_input(topNode);
      arr_n_v_cen[K]->Fill(m_cent_mdb, _particles.size());
      for (unsigned int i = 0; i < _particles.size(); ++i)
      {
        auto& part = _particles[i];
        float this_e = part->get_e();
        if (this_e == 0.) continue;
        float this_px = _particles[i]->get_px();
        float this_py = _particles[i]->get_py();
        float this_pz = _particles[i]->get_pz();

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
        if (pseudojet.perp() >=0.2) {
          v_towEta[K]->push_back( pseudojet.eta() );
          v_towPhi[K]->push_back( pseudojet.phi() );
          v_towPt [K]->push_back( pseudojet.pt()  );

          calo_pseudojets    .push_back(pseudojet);
          if (K==0)         EMCAL_pseudojets   .push_back(pseudojet);
          if (K==1 || K==2) HCAL_pseudojets    .push_back(pseudojet);
          if (K==1)         HCALIn_pseudojets  .push_back(pseudojet);
          if (K==2)         HCALOut_pseudojets .push_back(pseudojet);
        }
      }
      for (auto &p : _particles) delete p;
    }
  }
  /* cout << " fixme a5 " << endl; */

  // embed the particle in each jet -- if not found, don't fill any jet data
  /* int index_calo, index_EMCAL, index_HCAL, index_HCALIn, index_HCALOut; */
  vector< vector<fastjet::PseudoJet>* > particles { &calo_pseudojets, &HCALIn_pseudojets, &HCALOut_pseudojets, &HCAL_pseudojets, &EMCAL_pseudojets };
  for (int K = 0; K<5; ++K) {
    //embed the extra jet
    auto embjet = fastjet::PseudoJet();
    *v_embPt[K]  = 200.;// rng.Uniform(50,80);
    embjet.reset_PtYPhiM(*v_embPt[K], m_embEta, m_embPhi);

    const int emb_index = particles[K]->size();
    embjet.set_user_index(emb_index);
    particles[K]->push_back(embjet);

    //embed the extra particle, get the background, cluster, and be done!
    fastjet::JetMedianBackgroundEstimator bge_rm2 {selector_rm2, jet_def_bkgd, area_def};
    bge_rm2.set_particles(*(particles[K]));
    *v_rho[K] = bge_rm2.rho();
    *v_rhosigma[K] = bge_rm2.sigma();

    if (K==0) {
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
    }

    // make the jets
    fastjet::ClusterSequenceArea clustSeq(*particles[K], jet_def_antikt, area_def);
    vector<fastjet::PseudoJet> jets = sorted_by_pt( selection( clustSeq.inclusive_jets(m_ptRange.first) ));
    bool embtrack_found = false;
    if (jets.size() >= 1) {
      for (auto& cst : jets[0].constituents()) {
        if (cst.user_index() == emb_index) {
          embtrack_found = true;
          break;
        }
      }
    }
    if (embtrack_found) {
      *v_fluct[K] = (jets[0].pt() - jets[0].area()*m_rho) - m_embPt;
    } else {
      *v_embPt[K] = -200;
      *v_fluct[K] = -1000.; // this number is simply bad -- it shouldn't be used.
    }
    for (auto jet : jets) {
      v_JetEta[K]        ->push_back( jet.eta()     );
      v_JetPhi[K]        ->push_back( jet.phi_std() );
      v_JetE[K]          ->push_back( jet.E()       );
      v_JetPtLessRhoA[K] ->push_back( jet.pt() - m_rho_HCALIn * jet.area());
      v_JetArea[K]       ->push_back( jet.area());
    }
  }

  m_T->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}

void RhoMedianFluct::clear_vectors() {
  /* if (nevent % 1 == 0) cout << " z0 " << m_CaloJetArea.size(); */
  /* cout << " -- fixme a8 " << endl; */
  m_CaloJetEta        .clear();
  m_CaloJetPhi        .clear();
  m_CaloJetE          .clear();
  m_CaloJetPtLessRhoA .clear();
  m_CaloJetArea       .clear();
  
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

#include "PrintTowers.h"
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
#include <jetbase/Jet.h>
#include <jetbase/JetInput.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/JetMapv1.h>
#include <iostream>
#include <limits>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <stdexcept>
#include <trackbase_historic/SvtxTrackMap.h>
#include <globalvertex/GlobalVertexMap.h>

// using namespace fastjet;

PrintTowers::PrintTowers
     ( std::vector<std::string> jetnames
     , std::vector<std::pair<std::string,std::string>> matchlist
     , std::vector<std::pair<Jet::SRC,std::string>> src
     , const float  min_jet_pt
     , const float  min_tow_pt
     )
  : SubsysReco("PrintTowers")
  , m_jet_names  { jetnames }
  , m_matchlist  { matchlist }
  , m_SRC        { src }
  , m_min_jet_pt { min_jet_pt }
  , m_min_tow_pt { min_tow_pt }
{ }

PrintTowers::~PrintTowers()
{ 
}

int PrintTowers::Init(PHCompositeNode* topNode)
{ return Fun4AllReturnCodes::EVENT_OK; }

int PrintTowers::End(PHCompositeNode* topNode)
{ return Fun4AllReturnCodes::EVENT_OK; }

int PrintTowers::InitRun(PHCompositeNode* topNode)
{
  topNode->print();
  return Fun4AllReturnCodes::EVENT_OK;
}

int PrintTowers::process_event(PHCompositeNode* topNode)
{

  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  float cent = cent_node->get_centile(CentralityInfo::PROP::epd_NS);
  if (cent>m_max_cent) return Fun4AllReturnCodes::EVENT_OK;

  std::cout << std::endl << " NEW EVENT " << nevnt++ << " cent("<<cent<<")" <<std::endl;



  for (auto& name : m_jet_names) {
    std::cout << std::endl << " --- JetMap named " << name << " --- " << std::endl;
    JetMap* jetmap = findNode::getClass<JetMap>(topNode, name);
    if (jetmap == nullptr )
    { 
      std::cout << " -> is NULL (not present) on the node tree." << std::endl;
      continue;
    }
    auto jets = jetmap->vec();
    std::sort(jets.begin(), jets.end(), [](Jet* a, Jet* b){return a->get_pt() > b->get_pt();});
    std::cout << Form("  -> Has %i jets; printing up to %i jets w/pT>%5.2f", ((int)jets.size()), nmax_jetprint, m_min_jet_pt) << std::endl;
    int i {0};
    for (auto jet : jets) {
      if (jet->get_pt() < m_min_jet_pt) break;
      if (i==nmax_jetprint) break;
      std::cout << Form("k[%i] pt:phi:eta[%5.2f:%5.2f:%5.2f]", i++, jet->get_pt(), jet->get_phi(), jet->get_eta()) << std::endl;
    }
  }

  for (auto match : m_matchlist) {
    JetMap* jetmapA = findNode::getClass<JetMap>(topNode, match.first);
    if (jetmapA == nullptr )
    { 
      std::cout << " Can't jet map " << match.first << std::endl;
      continue;
    }
    auto jetsA = jetmapA->vec();
    std::sort(jetsA.begin(), jetsA.end(), [](Jet* a, Jet* b){return a->get_pt() > b->get_pt();});

    JetMap* jetmapB = findNode::getClass<JetMap>(topNode, match.second);
    if (jetmapB == nullptr )
    { 
      std::cout << " Can't find jet map " << match.second << std::endl;
      continue;
    }
    auto jetsB = jetmapB->vec();
    std::sort(jetsB.begin(), jetsB.end(), [](Jet* a, Jet* b){return a->get_pt() > b->get_pt();});

    std::cout << std::endl << " >>> Matching (up to " << m_pt_min_match << "GeV) " << match.first << " with " << match.second << std::endl;
    // do the matching, assume R=0.4
    std::vector<bool> B_is_matched (jetsB.size(),false);
    for (auto A : jetsA) {
      if (A->get_pt() < m_pt_min_match) break;
      bool found_A = false;
      for (unsigned int iB = 0; iB<jetsB.size(); ++iB) {
        if (B_is_matched[iB]) continue;
        auto B = jetsB[iB];
        float dphi = fabs(A->get_phi()-B->get_phi());
        while (dphi>M_PI) dphi = fabs(dphi - 2*M_PI);
        const float deta = A->get_eta() - B->get_eta();
        const float R2_comp = deta*deta + dphi*dphi;
        if (R2_comp<=0.16) {
          std::cout << Form("Matched pT->pT; phi->phi; eta->eta [ %6.2f->%6.2f, %6.2f->%6.2f, %6.2f->%6.2f ]",
              A->get_pt(), B->get_pt(), A->get_phi(), B->get_phi(), A->get_eta(), B->get_eta()) << std::endl;
          found_A = true;
          B_is_matched[iB] = true;
          break;
        }
      }
      if (!found_A) {
            std::cout << Form("NoMatch pT->??; phi->???? eta->????[ %6.2f->%6s, %6.2f->%6s, %6.2f->%6s ]",
                A->get_pt(), "nf",  A->get_phi(), "nf",  A->get_eta(), "nf")  << std::endl;
      }
    }
  }

  for (auto src : m_SRC) {
    std::cout << std::endl << " --- Printing Tower Input: " << src.second << std::endl;
    auto tow_inp = new TowerJetInput(src.first);
    auto tows = tow_inp->get_input(topNode);
    std::sort(tows.begin(), tows.end(), [](Jet* a, Jet* b){return a->get_pt() > b->get_pt();});
    std::cout << Form("  -> Has %i towers; printing all w/pT>%5.2f", ((int)tows.size()), m_min_tow_pt) << std::endl;
    int i {0};
    for (auto tow : tows) {
      if (tow->get_pt() < m_min_tow_pt) break;
      std::cout << Form("k[%i] pt:phi:eta[%5.2f:%5.2f:%5.2f]", i++, tow->get_pt(), tow->get_phi(), tow->get_eta()) << std::endl;
    }
    for (auto tow : tows) delete tow;
  }
  return Fun4AllReturnCodes::EVENT_OK;

}


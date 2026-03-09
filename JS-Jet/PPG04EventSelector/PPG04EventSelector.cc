#include "PPG04EventSelector.h"

#include <fun4all/Fun4AllReturnCodes.h>

// phool includes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// tower info
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

// standard includes
#include <utility>
#include <string>
#include <cmath>

// jet reco
#include <jetbase/Jet.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>

// minbias
#include <calotrigger/MinimumBiasInfo.h>
#include <calotrigger/MinimumBiasInfov1.h>

// global vertex
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertexMapv1.h>

int PPG04EventSelector::process_event(PHCompositeNode *topNode)
{
  // template for cut
  if(m_do_A_cut)
  {
    if(!A_cut(topNode))
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  // tower chi2 cut
  if(m_do_badChi2_cut)
  {
    if(!badChi2_cut(topNode))
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  
  // MC event selection based on leading R = 0.4 truth jet pT
  if(m_do_MCLeadingTruthJetpT_cut)
  {
    if(!MCLeadingTruthJetpT_cut(topNode))
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  // MinBias event selection
  if(m_do_minBias_cut)
  {
    if(!minBias_cut(topNode))
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  // zvtx cut
  if(m_do_zvtx_cut)
  {
    if(!zvtx_cut(topNode))
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }


  // if we get here, event passes all cuts
  return Fun4AllReturnCodes::EVENT_OK;
}

// template for cut
bool PPG04EventSelector::A_cut(PHCompositeNode */*topNode*/)
{
  // do stuff here

  // get value from node
  // while(false)
  // {
  //   // suppress unused variable warning since we aren't actually doing anything
  //   std::cout << "TopNode: " << topNode->getName() << std::endl;
  //   double cut_value_from_the_node = 0; 

  //   if(cut_value_from_the_node < m_A_cut){ return false; }
  // }

  return true;
}

// tower chi2 cut
bool PPG04EventSelector::badChi2_cut(PHCompositeNode *topNode)
{

  // get tower info containers
  TowerInfoContainer *towers[3];
  towers[0] = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  if (!towers[0])
    {
      std::cout << "PPG04EventSelector::badChi2_cut(PHCompositeNode *topNode) Could not find CEMC Tower Info node" << std::endl;
      exit(-1);
    }

  towers[1] = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  if (!towers[1])
    {
      std::cout << "PPG04EventSelector::badChi2_cut(PHCompositeNode *topNode) Could not find IHCal Tower Info node" << std::endl;
      exit(-1);
    }

  towers[2] = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  if (!towers[2])
    {
      std::cout << "PPG04EventSelector::badChi2_cut(PHCompositeNode *topNode) Could not find OHCal Tower Info node" << std::endl;
      exit(-1);
    }
  
  
  // loop over each calorimeter
  for(int i=0; i<3; i++)
    {
      unsigned int nChannels = towers[i]->size();
      for(unsigned int channel = 0; channel < nChannels; channel++)
	{
	  TowerInfo *tower = towers[i]->get_tower_at_channel(channel);
	  
	  if (tower->get_isBadChi2() && !tower->get_isHot() && !tower->get_isNoCalib())
	    {
        if(Verbosity() > 0)
        {
          std::cout << "PPG04EventSelector::badChi2_cut(PHCompositeNode *topNode) Tower is bad chi2" << std::endl;
        }
	      return false;
	    }
	}
    }
  
  return true;
}

// MC event selection based on leading R = 0.4 truth jet pT
bool PPG04EventSelector::MCLeadingTruthJetpT_cut(PHCompositeNode *topNode)
{
  // get truth jet nodes
  JetContainer *truthjets = findNode::getClass<JetContainer>(topNode, "AntiKt_Truth_r04");
  if(!truthjets) 
  {
    std::cout << "PPG04EventSelector::MCLeadingTruthJetpT_cut(PHCompositeNode *topNode) Could not find truth jet nodes" << std::endl;
    exit(-1); // this is a fatal error
  }

  // get leading truth jet pT
  float leading_truth_pt = -1;
  // for(JetMap::Iter iter = truthjets->begin(); iter != truthjets->end(); ++iter)
  // {
    // Jet *jet = iter->second;
  for(auto jet: *truthjets)
  {
    if(jet->get_pt() > leading_truth_pt) leading_truth_pt = jet->get_pt();
  }

  // check if event passes selection
  if( (leading_truth_pt < m_MCLeadingTruthJetpT_range.first) || (leading_truth_pt > m_MCLeadingTruthJetpT_range.second))
  {
    if(Verbosity() > 0) {std::cout << "PPG04EventSelector::MC_event_select(PHCompositeNode *topNode) Event failed MC event selection. Leading truth jet pT: " << leading_truth_pt << std::endl;}
    return false; 
  }

  return true;
}

// MinBias event selection
bool PPG04EventSelector::minBias_cut(PHCompositeNode *topNode)
{
  // get minbias info
  MinimumBiasInfo *minbias = findNode::getClass<MinimumBiasInfov1>(topNode, "MinimumBiasInfo");
  if(!minbias)
  {
    std::cout << "PPG04EventSelector::minBias_cut(PHCompositeNode *topNode) Could not find MinBiasInfo node" << std::endl;
    exit(-1); // this is a fatal error
  }

  // check if event passes selection
  if(!minbias->isAuAuMinimumBias())
  {
    if(Verbosity() > 0) {std::cout << "PPG04EventSelector::minBias_cut(PHCompositeNode *topNode) Event failed MinBias event selection" << std::endl;}
    return false;
  }
  return true;
}

// zvtx cut
bool PPG04EventSelector::zvtx_cut(PHCompositeNode *topNode)
{
   // get global vertex map node
    GlobalVertexMap *vtxMap = findNode::getClass<GlobalVertexMapv1>(topNode,"GlobalVertexMap");
    if (!vtxMap)
    {
      std::cout << "PPG04EventSelector::zvtx_cut(PHCompositeNode *topNode) Could not find global vertex map node" << std::endl;
      exit(-1); // this is a fatal error
    }

    if (!vtxMap->get(0))
    {
      if(Verbosity() > 0) {std::cout << "PPG04EventSelector::zvtx_cut(PHCompositeNode *topNode) No primary vertex" << std::endl;}
      return false;
    }

    if( (vtxMap->get(0)->get_z() < m_zvtx_range.first) || (vtxMap->get(0)->get_z() > m_zvtx_range.second))
    {
      if(Verbosity() > 0){std::cout << "PPG04EventSelector::zvtx_cut(PHCompositeNode *topNode) Vertex cut failed: " << vtxMap->get(0)->get_z() << std::endl;}
      return false;
    }   

  return true;
}
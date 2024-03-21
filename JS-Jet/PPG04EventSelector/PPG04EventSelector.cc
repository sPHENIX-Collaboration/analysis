#include "PPG04EventSelector.h"

#include <fun4all/Fun4AllReturnCodes.h>

// phool includes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// tower info
#include <calobase/TowerInfo,h>
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

int PPG04EventSelector::process_event(PHCompositeNode *topNode)
{
  // template for cut
  if(m_do_A_cut)
  {
    if(!A_cut(topNode))
    {
      if(Verbosity() > 0) std::cout << "PPG04EventSelector::process_event(PHCompositeNode *topNode) Event failed A cut" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  // tower chi2 cut
  if(m_do_badChi2_cut)
  {
    if(!badChi2_cut(topNode))
    {
      if(Verbosity() > 0) std::cout << "PPG04EventSelector::process_event(PHCompositeNode *topNode) Event failed tower bad chi2 cut" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  
  // MC event selection based on leading R = 0.4 truth jet pT
  if(m_MC_do_event_select)
  {
    if(!MC_event_select(topNode))
    {
      if(Verbosity() > 0) std::cout << "EventSelector::process_event(PHCompositeNode *topNode) Event failed MC event selection" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  // if we get here, event passes all cuts
  return Fun4AllReturnCodes::EVENT_OK;
}

// template for cut
bool PPG04EventSelector::A_cut(PHCompositeNode *topNode)
{
  // do stuff here

  // get value from node
  while(false)
  {
    // suppress unused variable warning since we aren't actually doing anything
    std::cout << "TopNode: " << topNode->GetName() << std::endl;
    double cut_value_from_the_node = 0; 

    if(cut_value_from_the_node < m_A_cut) return false;
  }

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
	      if(Verbosity())
		{
		  std::cout << "PPG04EventSelector::badChi2_cut(PHCompositeNode *topNode) Tower has bad chi2" << std::endl;
		}
	      return false;
	    }
	}
    }
  
  return true;
}


// MC event selection based on leading R = 0.4 truth jet pT
bool PPG04EventSelector::MC_event_select(PHCompositeNode *topNode)
{
  // get truth jet nodes
  JetMap *truthjets = findNode::getClass<JetMap>(topNode, "AntiKt_Truth_r04");
  if(!truthjets) 
  {
    std::cout << "EventSelector::MC_event_select(PHCompositeNode *topNode) Could not find truth jet nodes" << std::endl;
    exit(-1); // this is a fatal error
  }

  // get leading truth jet pT
  float leading_truth_pt = -1;
  for(JetMap::Iter iter = truthjets->begin(); iter != truthjets->end(); ++iter)
  {
    Jet *jet = iter->second;
    if(jet->get_pt() > leading_truth_pt) leading_truth_pt = jet->get_pt();
  }

  // check if event passes selection
  if( (leading_truth_pt < m_MC_event_selection_jetpT_range.first) || (leading_truth_pt > m_MC_event_selection_jetpT_range.second))
  {
    if(Verbosity() > 0) std::cout << "EventSelector::MC_event_select(PHCompositeNode *topNode) Event failed MC event selection" << std::endl;
    return false; 
  }

  return true;
}

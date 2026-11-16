#include "triggerSelector.h"

//____________________________________________________________________________..
triggerSelector::triggerSelector(const std::string &name)
  : SubsysReco(name)
{
}

//____________________________________________________________________________..
int triggerSelector::Init(PHCompositeNode* /*topNode*/)
{
    return Fun4AllReturnCodes::EVENT_OK;
}

int triggerSelector::process_event(PHCompositeNode *topNode)
{

    TriggerAnalyzer *ta = new TriggerAnalyzer();
  	ta->UseEmulator(false);
  	ta->decodeTriggers(topNode);
  	
    bool goodTrigger = false;
    for(auto goodTrig : m_validTrigNames)
    {
        goodTrigger = ta->didTriggerFire("Jet 10 GeV");
        if(goodTrigger) return Fun4AllReturnCodes::EVENT_OK;
    }

    return Fun4AllReturnCodes::ABORTEVENT;
}

int triggerSelector::End(PHCompositeNode* /*topNode */)
{
    return Fun4AllReturnCodes::EVENT_OK;
}
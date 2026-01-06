#include "EventShapeQA.h"

EventShapeQA::EventShapeQA(int verb, const std::string &name):
 SubsysReco(name)
{
  std::cout << "Setting up the event shape QA module" << std::endl;
  this->verbosity=verb;
}

EventShapeQA::~EventShapeQA()
{
  if( verbosity > 1 ) std::cout << "EventShapeQA::~EventShapeQA() Calling dtor" << std::endl;
}

int EventShapeQA::Init(PHCompositeNode *topNode)
{
  	if( verbosity > 0 ) std::cout << "EventShapeQA::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  	//find all available types of objects in order to decided which QA plots to make
	std::string NodeName = topNode->getName();
      	std::map<std::string, PHCompositeNode* >::const_iterator iter;
	std::map<std::string, PHCompositeNode*> topNodeMap;
	topNodeMap[NodeName] = topNode;
	PHPointerList<PHNode>& nodeList;
	HitPlots* dummy =new HitPlots();
	for(iter = topNodeMap.begin(); iter != topNodeMap.end(); ++iter)
	{
		PHNodeIterator nodeIter(iter->second);	
		nodeList=nodeIter.ls();
	}
	for(auto subL = nodeList.begin(); subL != nodeList.end(); ++subL)
	{
		std::string subnodeName = subL->getName();
		bool isJet = false;
		std::string subnodeClass=subL->getClass();
		if(subnodeClass.find("Jet") != std::string::npos) isJet=true;
		if(subnodeName.find("G4TruthInfo") != std::string::npos)
			subnodeName="G4 Truth Map";
		dummy->convertToEnum(subnodeName, isJet);
		if(dummy.ComponentType != -1 || dummy.JetType != -1 ){
			std::pair<CompDataType, JetDataType> dt { dummy.ComponentType, dummy.JetType};
			HitPlots* additional = new HitPlots(subnodeName, dummy.isJet, false);
			HitPlots* additional_shift = new HitPlots(subnodeName, dummy.isJet, true);
			
			std::pair<std::pair<CompDataTypes, JetDataTypes> , HitPlots*>> 
				ca {dt, additional},
			       	cas {dt, additional_shift};
			
			calo_jet_zero_axis->push_back(ca);
			calo_jet_shift_axis->push_back(cas);
			if(subnodeName.find("G4 Truth Map") != std::string::npos && isJet==false){
				subnodeName = "G4 Primary Particle";
				dummy->convertToEnum(subnodeName, isJet);
				std::pair<CompDataType, JetDataType> dt { dummy.ComponentType, dummy.JetType};
				HitPlots* additional = new HitPlots(subnodeName, dummy.isJet, false);
				HitPlots* additional_shift = new HitPlots(subnodeName, dummy.isJet, true);
				
				std::pair<std::pair<CompDataTypes, JetDataTypes> , HitPlots*>> 
					ca {dt, additional},
					cas {dt, additional_shift};
				calo_jet_zero_axis->push_back(ca);
				calo_jet_shift_axis->push_back(cas);
			}
			std::pair<std::pair<CompDataTypes, JetDataTypes> , PHCompositeNode*>> na {dt, subL};
			nodes.push_back(na);
		}
		
	}
 	return Fun4AllReturnCodes::EVENT_OK;
}
void EventShapeQA::doPHG4Analysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>> taggedNode)
{
	PHG4TruthInfoContianer* pn=(PHG4TruthInfoContianer*)taggedNode.second;
	if(pn)
	{
		PHG4TruthInfoContainer::ConstRange range = truthinfo->GetPrimaryParticleRange();
		for(auto iter = range.first; iter != range.second; ++iter)
		{
			PHG4Particle* part = iter->second;
			
	return;
}
void EventShapeQA::doPHG4JetAnalysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>> taggedNode)
{
	return;
}
void EventShapeQA::doHepMCAnalysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>> taggedNode)
{
	return;
}
void EventShapeQA::doCaloAnalysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>> taggedNode)
{
	
	return;
}
void EventShapeQA::doCaloJetAnalysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>> taggedNode)
{
	return;
}

int EventShapeQA::process_event(PHCompositeNode *topNode)
{
  	if( verbosity > 1 ) std::cout << "Processing Event " <<n_evt << std::endl;
 	//
 	return Fun4AllReturnCodes::EVENT_OK;
}

int EventShapeQA::End(PHCompositeNode *topNode)
{
  if( verbosity > 0 ) std::cout << "This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


void EventShapeQA::Print(const std::string &what) const
{
  if( verbosity > 0 ) std::cout << "EventShapeQA::Print(const std::string &what) const Printing info for " << what << std::endl;
}

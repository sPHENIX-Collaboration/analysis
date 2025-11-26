#include "EventShapeQA.h"

EventShapeQA::EventShapeQA(int verb, const std::string &name):
 SubsysReco(name)
{
  std::cout << "Setting up the event shape QA module" << std::endl;
  this->verbosity=verb;
}

EventShapeQA::~EventShapeQA()
{
  std::cout << "EventShapeQA::~EventShapeQA() Calling dtor" << std::endl;
}

int EventShapeQA::Init(PHCompositeNode *topNode)
{
  std::cout << "EventShapeQA::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


int EventShapeQA::process_event(PHCompositeNode *topNode)
{
  if(verbosity > 1) std::cout << "Processing Event " <<n_evt << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int EventShapeQA::End(PHCompositeNode *topNode)
{
  std::cout << "This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


void EventShapeQA::Print(const std::string &what) const
{
  std::cout << "EventShapeQA::Print(const std::string &what) const Printing info for " << what << std::endl;
}

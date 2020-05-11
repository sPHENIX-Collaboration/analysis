

#include "SynRadAna.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

using namespace std;

//____________________________________________________________________________..
SynRadAna::SynRadAna(const std::string &name):
 SubsysReco(name)
{
  cout << "SynRadAna::SynRadAna(const std::string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
SynRadAna::~SynRadAna()
{
  cout << "SynRadAna::~SynRadAna() Calling dtor" << endl;
}

//____________________________________________________________________________..
int SynRadAna::Init(PHCompositeNode *topNode)
{
  cout << "SynRadAna::Init(PHCompositeNode *topNode) Initializing" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::InitRun(PHCompositeNode *topNode)
{
  cout << "SynRadAna::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::process_event(PHCompositeNode *topNode)
{
  cout << "SynRadAna::process_event(PHCompositeNode *topNode) Processing Event" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::ResetEvent(PHCompositeNode *topNode)
{
  cout << "SynRadAna::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::EndRun(const int runnumber)
{
  cout << "SynRadAna::EndRun(const int runnumber) Ending Run for Run " << runnumber << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::End(PHCompositeNode *topNode)
{
  cout << "SynRadAna::End(PHCompositeNode *topNode) This is the End..." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::Reset(PHCompositeNode *topNode)
{
 cout << "SynRadAna::Reset(PHCompositeNode *topNode) being Reset" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void SynRadAna::Print(const std::string &what) const
{
  cout << "SynRadAna::Print(const std::string &what) const Printing info for " << what << endl;
}

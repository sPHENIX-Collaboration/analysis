//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in HCalJetPhiShift.h.
//
// HCalJetPhiShift(const std::string &name = "HCalJetPhiShift")
// everything is keyed to HCalJetPhiShift, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// HCalJetPhiShift::~HCalJetPhiShift()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int HCalJetPhiShift::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int HCalJetPhiShift::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int HCalJetPhiShift::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int HCalJetPhiShift::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int HCalJetPhiShift::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int HCalJetPhiShift::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int HCalJetPhiShift::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void HCalJetPhiShift::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "HCalJetPhiShift.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
HCalJetPhiShift::HCalJetPhiShift(const std::string &name):
 SubsysReco(name)
{
  std::cout << "HCalJetPhiShift::HCalJetPhiShift(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
HCalJetPhiShift::~HCalJetPhiShift()
{
  std::cout << "HCalJetPhiShift::~HCalJetPhiShift() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int HCalJetPhiShift::Init(PHCompositeNode *topNode)
{
  std::cout << "HCalJetPhiShift::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::InitRun(PHCompositeNode *topNode)
{
  std::cout << "HCalJetPhiShift::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::process_event(PHCompositeNode *topNode)
{
  std::cout << "HCalJetPhiShift::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::ResetEvent(PHCompositeNode *topNode)
{
  std::cout << "HCalJetPhiShift::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::EndRun(const int runnumber)
{
  std::cout << "HCalJetPhiShift::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::End(PHCompositeNode *topNode)
{
  std::cout << "HCalJetPhiShift::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::Reset(PHCompositeNode *topNode)
{
 std::cout << "HCalJetPhiShift::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void HCalJetPhiShift::Print(const std::string &what) const
{
  std::cout << "HCalJetPhiShift::Print(const std::string &what) const Printing info for " << what << std::endl;
}

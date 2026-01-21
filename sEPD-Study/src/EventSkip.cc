//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in EventSkip.h.
//
// EventSkip(const std::string &name = "EventSkip")
// everything is keyed to EventSkip, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// EventSkip::~EventSkip()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int EventSkip::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int EventSkip::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int EventSkip::process_event(PHCompositeNode *topNode)
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
// int EventSkip::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int EventSkip::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int EventSkip::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int EventSkip::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void EventSkip::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "EventSkip.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <ffaobjects/EventHeader.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>

// c++
#include <format>

//____________________________________________________________________________..
EventSkip::EventSkip(const std::string& name)
  : SubsysReco(name)
{
  std::cout << "EventSkip::EventSkip(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
int EventSkip::Init([[maybe_unused]] PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EventSkip::process_event([[maybe_unused]] PHCompositeNode* topNode)
{
  ++m_event;

  auto* eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int m_globalEvent = eventInfo->get_EvtSequence();

  if (Verbosity())
  {
    std::cout << std::format("Global: {}\n", m_globalEvent);
  }

  if (m_event <= m_nSkip || m_globalEvent < m_event_id)
  {
    if (m_event % 100 == 0 || (m_nSkip && std::abs(m_event - m_nSkip) < 20) || (m_event_id && std::abs(m_globalEvent - m_event_id) < 20))
    {
      std::cout << std::format("Skipping: {}, Global: {}\n", m_event, m_globalEvent);
    }

    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if (m_event_id && m_globalEvent > m_event_id)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EventSkip::ResetEvent([[maybe_unused]] PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EventSkip::End([[maybe_unused]] PHCompositeNode* topNode)
{
  std::cout << "EventSkip::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

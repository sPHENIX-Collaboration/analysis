//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in CaloCheck.h.
//
// CaloCheck(const std::string &name = "CaloCheck")
// everything is keyed to CaloCheck, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// CaloCheck::~CaloCheck()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int CaloCheck::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int CaloCheck::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int CaloCheck::process_event(PHCompositeNode *topNode)
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
// int CaloCheck::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int CaloCheck::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int CaloCheck::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int CaloCheck::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void CaloCheck::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "CaloCheck.h"

// -- c++
#include <filesystem>
#include <memory>
#include <format>
#include <iostream>
#include <fstream>
#include <stdexcept>

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- DST Node
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- CaloBase
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

// -- event
#include <ffaobjects/EventHeader.h>

//____________________________________________________________________________..
CaloCheck::CaloCheck(const std::string &name)
 : SubsysReco(name)
{
  std::cout << "CaloCheck::CaloCheck(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
CaloCheck::~CaloCheck()
{
  std::cout << "CaloCheck::~CaloCheck() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int CaloCheck::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  std::cout << "CaloCheck::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  int ret = read_event_list(m_event_list);
  if (ret)
  {
    return ret;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int CaloCheck::read_event_list(const std::string &event_list)
{
  std::ifstream input_file(event_list);

  if (!input_file.is_open())
  {
    std::cout << std::format("Error: Could not open file '{}' for reading.", event_list) << std::endl;
    std::filesystem::remove(event_list);
    return Fun4AllReturnCodes::ABORTRUN;
  }

  std::string line;
  int line_num = 0;
  while (std::getline(input_file, line))
  {
    line_num++;
    try
    {
      // std::stoi converts string to int
      int event = std::stoi(line);
      m_events.insert(event);
    }
    catch (const std::invalid_argument &e)
    {
      std::cout << std::format("Warning: Line {} ('{}') is not a valid integer. Skipping.", line_num, line) << std::endl;
    }
    catch (const std::out_of_range &e)
    {
      std::cout << std::format("Warning: Line {} ('{}') is out of integer range. Skipping.", line_num, line) << std::endl;
    }
  }

  if (Verbosity())
  {
    std::cout << "===============" << std::endl;
    std::cout << "Events to Check" << std::endl;
    for (int event : m_events)
    {
      std::cout << std::format("Event: {}", event) << std::endl;
    }
    std::cout << "===============" << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::process_event_check(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    std::cout << PHWHERE << "CaloCheck::process_event - Fatal Error - EventHeader node is missing. " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int m_globalEvent = eventInfo->get_EvtSequence();
  // int m_run         = eventInfo->get_RunNumber();

  if (m_event % 20 == 0)
  {
    std::cout << "Progress: " << m_event << ", Global: " << m_globalEvent << std::endl;
  }
  ++m_event;

  if(!m_events.contains(m_globalEvent))
  {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::process_calo(PHCompositeNode *topNode)
{
  std::cout << "CaloCheck::process_calo(PHCompositeNode *topNode) Processing Event" << std::endl;
  // Stop after all events of interest have been processed
  if (++m_processed_event == m_events.size())
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::process_event(PHCompositeNode *topNode)
{
  int ret = process_event_check(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_calo(topNode);
  if (ret)
  {
    return ret;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "CaloCheck::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


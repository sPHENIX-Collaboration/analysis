#include "EventSkip.h"

#include <ffaobjects/EventHeader.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>

// c++
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

//____________________________________________________________________________..
EventSkip::EventSkip(const std::string &name)
  : SubsysReco(name)
{
  std::cout << "EventSkip::EventSkip(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
void EventSkip::set_event_ids(const std::string &filename)
{
  std::ifstream infile(filename);
  if (!infile.is_open())
  {
    std::cout << std::format("EventSkip::set_event_ids: Could not open event list file: {}\n", filename);
    return;
  }

  m_event_ids.clear();
  std::string line;
  while (std::getline(infile, line))
  {
    if (line.empty() || line[0] == '#')
    {
      continue;
    }
    std::stringstream ss(line);
    int event_id = 0;
    if (ss >> event_id)
    {
      m_event_ids.insert(event_id);
    }
  }

  if (!m_event_ids.empty())
  {
    m_max_event_id = *m_event_ids.rbegin();
    std::cout << std::format("EventSkip: Loaded {} event IDs from {}. Max event ID: {}\n", m_event_ids.size(), filename, m_max_event_id);
  }
  else
  {
    std::cout << std::format("EventSkip: Warning, no valid event IDs found in file: {}\n", filename);
  }
}

//____________________________________________________________________________..
int EventSkip::Init([[maybe_unused]] PHCompositeNode *topNode) { return Fun4AllReturnCodes::EVENT_OK; }

//____________________________________________________________________________..
int EventSkip::process_event(PHCompositeNode *topNode)
{
  ++m_event;

  auto *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int m_globalEvent = eventInfo->get_EvtSequence();

  if (Verbosity())
  {
    std::cout << std::format("Global: {}\n", m_globalEvent);
  }

  // If a list of event IDs was specified
  if (!m_event_ids.empty())
  {
    // If all requested events have already been processed, or if the current
    // event is beyond the max requested event ID, abort run
    if (m_processed_events.size() >= m_event_ids.size() || m_globalEvent > m_max_event_id)
    {
      std::cout << std::format(
          "EventSkip: Finished processing requested events. Aborting run at event #{}, Global: {}\n", m_event, m_globalEvent);
      return Fun4AllReturnCodes::ABORTRUN;
    }

    // Check if the current event matches one of the requested event IDs
    if (m_event <= m_nSkip || !m_event_ids.contains(m_globalEvent))
    {
      if (Verbosity() && (m_event % 100 == 0 || (m_nSkip && std::abs(m_event - m_nSkip) < 20)))
      {
        std::cout << std::format("Skipping: {}, Global: {}\n", m_event, m_globalEvent);
      }

      return Fun4AllReturnCodes::ABORTEVENT;
    }

    // Matched requested event
    std::cout << std::format("EventSkip: Processing matched event #{} (Global: {})\n", m_event, m_globalEvent);
    m_processed_events.insert(m_globalEvent);

    return Fun4AllReturnCodes::EVENT_OK;
  }

  // If no event IDs were specified, fall back to pure skip count
  if (m_event <= m_nSkip)
  {
    if (Verbosity() && (m_event % 100 == 0 || (m_nSkip && std::abs(m_event - m_nSkip) < 20)))
    {
      std::cout << std::format("Skipping: {}, Global: {}\n", m_event, m_globalEvent);
    }

    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EventSkip::ResetEvent([[maybe_unused]] PHCompositeNode *topNode) { return Fun4AllReturnCodes::EVENT_OK; }

//____________________________________________________________________________..
int EventSkip::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "EventSkip::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

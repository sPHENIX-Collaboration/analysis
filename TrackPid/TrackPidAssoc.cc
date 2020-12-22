/**                                                                                                                                                                                                       
 * @file TrackPidAssoc/TrackPidAssoc.cc
 * @author Tony Frawley
 * @date December 2020
 * @brief TrackPidAssoc implementation
 */

#include "TrackPidAssoc.h"

#include <ostream>  // for operator<<, endl, basic_ostream, ostream, basic_o...

TrackPidAssoc::TrackPidAssoc() 
  : m_map()
{
}


TrackPidAssoc::~TrackPidAssoc()
{
}

void 
TrackPidAssoc::Reset()
{
  m_map.clear();
}

void 
TrackPidAssoc::identify(std::ostream &os) const
{
  os << "-----TrackPidAssoc-----" << std::endl;
  os << "Number of associations: " << m_map.size() << std::endl;

  for ( auto& entry : m_map )
  {
    // os << "   cluster key: 0x" << std::hex << entry.first << std::dec
    os << "   particle ID: "  << entry.first << std::dec
       << " track key: " << entry.second << std::endl;
  }

  os << "------------------------------" << std::endl;

  return;

}

void 
TrackPidAssoc::addAssoc(unsigned int pid, unsigned int trid)
{
  m_map.insert(std::make_pair(pid, trid));
}

TrackPidAssoc::ConstRange 
TrackPidAssoc::getTracks(unsigned int pid)
{
  ConstRange retpair;
  retpair.first = m_map.lower_bound(pid);
  retpair.second = m_map.upper_bound(pid);
  return retpair;
}

#include "PidInfoContainer.h"
#include "PidInfo.h"

#include <cstdlib>
#include <iostream>

using namespace std;

PidInfoContainer::ConstRange
PidInfoContainer::getPidInfos( void ) const
{
  return make_pair(_pidinfos.begin(), _pidinfos.end());
}

PidInfoContainer::Range
PidInfoContainer::getPidInfos( void )
{
  return make_pair(_pidinfos.begin(), _pidinfos.end());
}

PidInfoContainer::ConstIterator
PidInfoContainer::AddPidInfo(PidInfo *pidinfo)
{
  unsigned int key = pidinfo->get_track_id();
  _pidinfos[key] = pidinfo;
  return _pidinfos.find(key);
}

PidInfo *
PidInfoContainer::getPidInfo(const int key)
{
  Iterator it = _pidinfos.find(key);
  if (it != _pidinfos.end())
    {
      return it->second;
    }
  return NULL;
}

int
PidInfoContainer::isValid() const
{
  return (!_pidinfos.empty());
}

void
PidInfoContainer::Reset()
{
  while (_pidinfos.begin() != _pidinfos.end())
    {
      delete _pidinfos.begin()->second;
      _pidinfos.erase(_pidinfos.begin());
    }
}

void
PidInfoContainer::identify(std::ostream& os) const
{
  os << "PidInfoContainer, number of pid infos: " << size() << std::endl;
}

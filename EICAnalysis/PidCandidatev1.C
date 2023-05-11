#include "PidCandidatev1.h"

#include <phool/phool.h>

#include <cstdlib>

using namespace std;

ClassImp(PidCandidatev1)

PidCandidatev1::PidCandidatev1():
_candidateid(INT_MAX)
{
}

PidCandidatev1::PidCandidatev1(PidCandidate const &tc)
{
  Copy(tc);
}

void
PidCandidatev1::Reset()
{
  prop_map.clear();
}

void
PidCandidatev1::print() const {
  std::cout << "New PidCandidate 0x" << hex << _candidateid << std::endl;

  for (prop_map_t::const_iterator i = prop_map.begin(); i!= prop_map.end(); ++i)
    {
      PROPERTY prop_id = static_cast<PROPERTY>(i->first);
      pair<const string, PROPERTY_TYPE> property_info = get_property_info(prop_id);
      cout << "\t" << prop_id << ":\t" << property_info.first << " = \t";
      switch(property_info.second)
        {
        case type_int:
          cout << get_property_int(prop_id);
          break;
        case type_uint:
          cout << get_property_uint(prop_id);
          break;
        case type_float:
          cout << get_property_float(prop_id);
          break;
        default:
          cout << " unknown type ";
        }
      cout <<endl;
    }
}

bool
PidCandidatev1::has_property(const PROPERTY prop_id) const
{
  prop_map_t::const_iterator i = prop_map.find(prop_id);
  return i!=prop_map.end();
}

float
PidCandidatev1::get_property_float(const PROPERTY prop_id) const
{
  if (!check_property(prop_id,type_float))
    {
      pair<const string,PROPERTY_TYPE> property_info =get_property_info(prop_id);
      cout << PHWHERE << " Property " << property_info.first << " with id "
           << prop_id << " is of type " << get_property_type(property_info.second)
           << " not " << get_property_type(type_float) << endl;
      exit(1);
    }
  prop_map_t::const_iterator i = prop_map.find(prop_id);

  if (i!=prop_map.end()) return u_property(i->second).fdata;

  return   NAN ;
}

int
PidCandidatev1::get_property_int(const PROPERTY prop_id) const
{
  if (!check_property(prop_id,type_int))
    {
      pair<const string,PROPERTY_TYPE> property_info =get_property_info(prop_id);
      cout << PHWHERE << " Property " << property_info.first << " with id "
           << prop_id << " is of type " << get_property_type(property_info.second)
           << " not " << get_property_type(type_int) << endl;
      exit(1);
    }
  prop_map_t::const_iterator i = prop_map.find(prop_id);

  if (i!=prop_map.end()) return u_property(i->second).idata;

  return INT_MIN;
}

unsigned int
PidCandidatev1::get_property_uint(const PROPERTY prop_id) const
{
  if (!check_property(prop_id,type_uint))
    {
      pair<const string,PROPERTY_TYPE> property_info =get_property_info(prop_id);
      cout << PHWHERE << " Property " << property_info.first << " with id "
           << prop_id << " is of type " << get_property_type(property_info.second)
           << " not " << get_property_type(type_uint) << endl;
      exit(1);
    }
  prop_map_t::const_iterator i = prop_map.find(prop_id);

  if (i!=prop_map.end()) return u_property(i->second).uidata;

  return UINT_MAX ;
}

void
PidCandidatev1::set_property(const PROPERTY prop_id, const float value)
{
  if (!check_property(prop_id,type_float))
    {
      pair<const string,PROPERTY_TYPE> property_info = get_property_info(prop_id);
      cout << PHWHERE << " Property " << property_info.first << " with id "
           << prop_id << " is of type " << get_property_type(property_info.second)
           << " not " << get_property_type(type_float) << endl;
      exit(1);
    }
  prop_map[prop_id] = u_property(value).get_storage();
}

void
PidCandidatev1::set_property(const PROPERTY prop_id, const int value)
{
  if (!check_property(prop_id,type_int))
    {
      pair<const string,PROPERTY_TYPE> property_info = get_property_info(prop_id);
      cout << PHWHERE << " Property " << property_info.first << " with id "
           << prop_id << " is of type " << get_property_type(property_info.second)
           << " not " << get_property_type(type_int) << endl;
      exit(1);
    }
  prop_map[prop_id] = u_property(value).get_storage();
}

void
PidCandidatev1::set_property(const PROPERTY prop_id, const unsigned int value)
{
  if (!check_property(prop_id,type_uint))
    {
      pair<const string,PROPERTY_TYPE> property_info = get_property_info(prop_id);
      cout << PHWHERE << " Property " << property_info.first << " with id "
           << prop_id << " is of type " << get_property_type(property_info.second)
           << " not " << get_property_type(type_uint) << endl;
      exit(1);
    }
  prop_map[prop_id] = u_property(value).get_storage();
}

unsigned int
PidCandidatev1::get_property_nocheck(const PROPERTY prop_id) const
{
  prop_map_t::const_iterator iter = prop_map.find(prop_id);
  if (iter != prop_map.end())
    {
      return iter->second;
    }
  return UINT_MAX;
}

void
PidCandidatev1::identify(ostream& os) const
{
  cout << "PidCandidatev1 with candidate_id: 0x" << hex << _candidateid << dec << endl;
  cout << "Class " << this->ClassName() << endl;

  for (prop_map_t::const_iterator i = prop_map.begin(); i!= prop_map.end(); ++i)
    {
      PROPERTY prop_id = static_cast<PROPERTY>(i->first);
      pair<const string, PROPERTY_TYPE> property_info = get_property_info(prop_id);
      cout << "\t" << prop_id << ":\t" << property_info.first << " = \t";
      switch(property_info.second)
        {
        case type_int:
          cout << get_property_int(prop_id);
          break;
        case type_uint:
          cout << get_property_uint(prop_id);
          break;
        case type_float:
          cout << get_property_float(prop_id);
          break;
        default:
          cout << " unknown type ";
        }
      cout <<endl;
    }
}

#include "sPHElectronPairContainerv1.h"
#include "sPHElectronPairv1.h"

#include <iterator>     // for reverse_iterator
#include <map>          // for _Rb_tree_const_iterator, _Rb_tree_iterator
#include <ostream>      // for operator<<, endl, ostream, basic_ostream, bas...
#include <utility>      // for pair, make_pair

using namespace std;

sPHElectronPairContainerv1::sPHElectronPairContainerv1()
  : _map()
{
}

sPHElectronPairContainerv1::sPHElectronPairContainerv1(const sPHElectronPairContainerv1& container)
  : _map()
{
  for (ConstIter iter = container.begin();
       iter != container.end();
       ++iter)
  {
    sPHElectronPair* epair = dynamic_cast<sPHElectronPair*> (iter->second->CloneMe());
    _map.insert(make_pair(epair->get_id(), epair));
  }
}

sPHElectronPairContainerv1& sPHElectronPairContainerv1::operator=(const sPHElectronPairContainerv1& container)
{
  Reset();
  for (ConstIter iter = container.begin();
       iter != container.end();
       ++iter)
  {
    sPHElectronPair* epair = dynamic_cast<sPHElectronPair*> (iter->second->CloneMe());
    _map.insert(make_pair(epair->get_id(), epair));
  }
  return *this;
}

sPHElectronPairContainerv1::~sPHElectronPairContainerv1()
{
  Reset();
}

void sPHElectronPairContainerv1::Reset()
{
  for (Iter iter = _map.begin();
       iter != _map.end();
       ++iter)
  {
    sPHElectronPair* epair = iter->second;
    delete epair;
  }
  _map.clear();
}

void sPHElectronPairContainerv1::identify(ostream& os) const
{
  os << "sPHElectronPairContainerv1: size = " << _map.size() << endl;
  return;
}

const sPHElectronPair* sPHElectronPairContainerv1::get(unsigned int id) const
{
  ConstIter iter = _map.find(id);
  if (iter == _map.end()) return nullptr;
  return iter->second;
}

sPHElectronPair* sPHElectronPairContainerv1::get(unsigned int id)
{
  Iter iter = _map.find(id);
  if (iter == _map.end()) return nullptr;
  return iter->second;
}

sPHElectronPair* sPHElectronPairContainerv1::insert(const sPHElectronPair* epair)
{
  unsigned int index = 0;
  if (!_map.empty()) index = _map.rbegin()->first + 1;
  _map.insert(make_pair(index, dynamic_cast<sPHElectronPair*> (epair->CloneMe())));
  _map[index]->set_id(index);
  return _map[index];
}


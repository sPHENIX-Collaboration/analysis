#ifndef SPHELECTRONPAIRCONTAINER_V1_H
#define SPHELECTRONPAIRCONTAINER_V1_H

#include "sPHElectronPair.h"
#include "sPHElectronPairv1.h"
#include "sPHElectronPairContainer.h"

#include <cstddef>        // for size_t
#include <iostream>        // for cout, ostream

class sPHElectronPairContainerv1 : public sPHElectronPairContainer
{
public:

  typedef std::map<unsigned int, sPHElectronPairv1*> PairMap;
  typedef std::map<unsigned int, sPHElectronPairv1*>::const_iterator ConstIter;
  typedef std::map<unsigned int, sPHElectronPairv1*>::iterator Iter;

  sPHElectronPairContainerv1();
  sPHElectronPairContainerv1(const sPHElectronPairContainerv1& container);
  sPHElectronPairContainerv1& operator=(const sPHElectronPairContainerv1& container);

  virtual ~sPHElectronPairContainerv1();

  void identify(std::ostream& os = std::cout) const;
  void Reset();
  void clear() { Reset(); }
  int isValid() const { return 1; }
  size_t size() const { return _map.size(); }
  bool empty() const { return _map.empty(); }
  PHObject* CloneMe() const { return new sPHElectronPairContainerv1(*this); }

  const sPHElectronPairv1* get(unsigned int idkey) const;
  sPHElectronPairv1* get(unsigned int idkey);
  void insert(const sPHElectronPairv1* pair);
  size_t erase(unsigned int idkey)
  {
    delete _map[idkey];
    return _map.erase(idkey);
  }

  ConstIter begin() const { return _map.begin(); }
  ConstIter find(unsigned int idkey) const { return _map.find(idkey); }
  ConstIter end() const { return _map.end(); }

  Iter begin() { return _map.begin(); }
  Iter find(unsigned int idkey) { return _map.find(idkey); }
  Iter end() { return _map.end(); }

protected:
  PairMap _map;
  ClassDef(sPHElectronPairContainerv1, 1);

};

#endif


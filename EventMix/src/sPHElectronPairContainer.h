#ifndef __SPHELECTRONPAIRCONTAINER_H__
#define __SPHELECTRONPAIRCONTAINER_H__

#include <phool/PHObject.h>
#include <iostream>
#include <map>

#include "sPHElectronPair.h"

class sPHElectronPairContainer : public PHObject
{
public:
//  typedef std::map<unsigned int, sPHElectronPair*> PairMap;
//  typedef std::map<unsigned int, sPHElectronPair*>::const_iterator ConstIter;
//  typedef std::map<unsigned int, sPHElectronPair*>::iterator Iter;

  virtual ~sPHElectronPairContainer() {}

  virtual void identify(std::ostream& os = std::cout) const 
    { os << "sPHElectronPairContainer base class" << std::endl; }

  virtual void Reset() {}
  virtual void clear() {} 
  virtual int isValid() const { return 0; }
  virtual size_t size() const { return 0; }
  virtual bool empty() const  { return true; } // returns true if container empty
  virtual PHObject* CloneMe() const { return nullptr; }

//  virtual const sPHElectronPair* get(unsigned int id) const    {return nullptr;}
//  virtual sPHElectronPair* get(unsigned int id)                {return nullptr;}
//  virtual void insert(const sPHElectronPair* pair) {return nullptr;}
//  virtual size_t erase(unsigned int id)                        {return 0;}

//  virtual ConstIter begin() const { return PairMap().end(); }
//  virtual ConstIter find(unsigned int id) const { return PairMap().end(); }
//  virtual ConstIter end() const { return PairMap().end(); }

//  virtual Iter begin() { return PairMap().end(); }
//  virtual Iter find(unsigned int id) { return PairMap().end(); }
//  virtual Iter end() { return PairMap().end(); }

protected:
  sPHElectronPairContainer() {}
  ClassDef(sPHElectronPairContainer, 1);
};

#endif


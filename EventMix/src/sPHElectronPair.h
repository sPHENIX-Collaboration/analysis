#ifndef SPHELECTRONPAIR_H
#define SPHELECTRONPAIR_H

#include <phool/PHObject.h>

#include <climits>
#include <cmath>
#include <iostream>

class sPHElectron;

class sPHElectronPair : public PHObject
{
 public:
  virtual ~sPHElectronPair() {}

  virtual void identify(std::ostream& os = std::cout) const
     { os << "sPHElectronPair base class" << std::endl; }
  virtual void Reset() {}
  virtual int isValid() const { return 0; }
  virtual PHObject* CloneMe() const { return nullptr; }

  virtual sPHElectron* get_first()  { return nullptr; }
  virtual sPHElectron* get_second() { return nullptr; }

  virtual int get_id()    const { return -99999; }
  virtual int get_type()    const { return 0; }
  virtual double get_mass() const { return NAN; }
  virtual double get_pt()   const { return NAN; }
  virtual double get_eta()  const { return NAN; }
  virtual double get_phiv() const { return NAN; }
  virtual double get_min_mass() const { return NAN; }

  virtual void set_id(int id) { }
  virtual void set_type(int type) { }
  virtual void set_min_mass(double mm) { }

 protected:
  sPHElectronPair() {}
  ClassDef(sPHElectronPair, 1)
};

#endif 

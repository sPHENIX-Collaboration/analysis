#ifndef SPHELECTRONPAIRV1_H
#define SPHELECTRONPAIRV1_H

#include <climits>
#include <cmath>
#include <iostream>

#include "sPHElectronPair.h"
#include "sPHElectronv1.h"

class sPHElectronPairv1 : public sPHElectronPair
{
 public:
  sPHElectronPairv1();
  sPHElectronPairv1(sPHElectronv1* e1, sPHElectronv1* e2);
  virtual ~sPHElectronPairv1() {}

  virtual void identify(std::ostream& os = std::cout) const
     { os << "sPHElectronPairv1 object class" << std::endl; }
  virtual void Reset() {}
  virtual int isValid() const { return 1; }
  virtual PHObject* CloneMe() const { return new sPHElectronPairv1(*this); }

  virtual sPHElectron* get_first()  { return &_e1; }
  virtual sPHElectron* get_second() { return &_e2; }

  virtual int get_id() const { return _id; }
  virtual int get_type() const { return _type; }
  virtual double get_mass() const;
  virtual double get_pt()   const;
  virtual double get_eta()  const;
  virtual double get_phiv() const;
  virtual double get_min_mass() const { return _min_mass; }
 
  virtual void set_id(int id) { _id = id;}
  virtual void set_type(int type) { _type = type;}
  virtual void set_min_mass(double mm) { _min_mass = mm;}

 protected:

  int _id;
  int _type;
  double _min_mass;
  sPHElectronv1 _e1;
  sPHElectronv1 _e2;

  ClassDef(sPHElectronPairv1, 1)
};

#endif 

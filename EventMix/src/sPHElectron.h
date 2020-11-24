#ifndef SPHELECTRON_H
#define SPHELECTRON_H

#include <phool/PHObject.h>

#include <climits>
#include <cmath>
#include <iostream>

class sPHElectron : public PHObject
{
 public:
  virtual ~sPHElectron() {}

  virtual void identify(std::ostream& os = std::cout) const
     { os << "sPHElectron base class" << std::endl; }
  virtual void Reset() {}
  virtual int isValid() const { return 0; }

  virtual unsigned int get_id()  const { return 99999; }
  virtual int get_charge()       const { return 0; }
  virtual double get_px()        const { return 0.;}
  virtual double get_py()        const { return 0.;}
  virtual double get_pz()        const { return 0.;}
  virtual double get_dphi()      const { return 0.;}
  virtual double get_deta()      const { return 0.;}
  virtual double get_emce()      const { return 0.;}
  virtual double get_e3x3()      const { return 0.;}
  virtual double get_e5x5()      const { return 0.;}

  virtual void set_id(unsigned int id) { }
  virtual void set_charge(int charge)  { }
  virtual void set_px(double px)       { }
  virtual void set_py(double px)       { }
  virtual void set_pz(double pz)       { }
  virtual void set_dphi(double dphi)   { }
  virtual void set_deta(double deta)   { }
  virtual void set_emce(double emce)   { }
  virtual void set_e3x3(double e3x3)   { }
  virtual void set_e5x5(double e5x5)   { }

 protected:
  sPHElectron() {}
  ClassDef(sPHElectron, 1)
};

#endif 

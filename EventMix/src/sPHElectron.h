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
  virtual PHObject* CloneMe() const { return nullptr; }

  virtual unsigned int get_id()  const { return 99999; }
  virtual int get_charge()       const { return 0; }
  virtual double get_px()        const { return NAN;}
  virtual double get_py()        const { return NAN;}
  virtual double get_pz()        const { return NAN;}
  virtual double get_dphi()      const { return NAN;}
  virtual double get_deta()      const { return NAN;}
  virtual double get_emce()      const { return NAN;}
  virtual double get_e3x3()      const { return NAN;}
  virtual double get_e5x5()      const { return NAN;}

  virtual double get_chi2()      const { return NAN;}
  virtual unsigned int get_ndf() const { return 99999;}
  virtual double get_zvtx()      const { return NAN;}
  virtual double get_dca2d()     const { return NAN;}
  virtual double get_dca2d_error()      const { return NAN;}
  virtual double get_dca3d_xy()  const { return NAN;}
  virtual double get_dca3d_z()   const { return NAN;}

  virtual int get_nmvtx()   const { return -1;}
  virtual int get_ntpc()   const { return -1;}

  virtual double get_cemc_ecore()   const { return NAN;}
  virtual double get_cemc_chi2()   const { return NAN;}
  virtual double get_cemc_prob()   const { return NAN;}
  virtual double get_cemc_dphi()   const { return NAN;}
  virtual double get_cemc_deta()   const { return NAN;}
  virtual double get_hcalin_e()   const { return NAN;}
  virtual double get_hcalin_dphi()   const { return NAN;}
  virtual double get_hcalin_deta()   const { return NAN;}

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

  virtual void set_chi2(double a)      { }
  virtual void set_ndf(unsigned int a) { }
  virtual void set_zvtx(double a)      { }
  virtual void set_dca2d(double a)     { }
  virtual void set_dca2d_error(double a)   { }
  virtual void set_dca3d_xy(double a)  { }
  virtual void set_dca3d_z(double a)   { }

  virtual void set_nmvtx(int i)   { }
  virtual void set_ntpc(int i)   { }

  virtual void set_cemc_ecore(double a)   { }
  virtual void set_cemc_chi2(double a)   { }
  virtual void set_cemc_prob(double a)   { }
  virtual void set_cemc_dphi(double a)   { }
  virtual void set_cemc_deta(double a)   { }
  virtual void set_hcalin_e(double a)   { }
  virtual void set_hcalin_dphi(double a)   { }
  virtual void set_hcalin_deta(double a)   { }

 protected:
  sPHElectron() {}
  ClassDef(sPHElectron, 1)
};

#endif 

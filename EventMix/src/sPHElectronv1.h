#ifndef SPHELECTRONV1_H
#define SPHELECTRONV1_H

#include "sPHElectron.h"

#include <iostream>

class SvtxTrack;

class PHObject;

class sPHElectronv1 : public sPHElectron
{
 public:
  sPHElectronv1();
  sPHElectronv1(const SvtxTrack*);
  sPHElectronv1(const sPHElectronv1& electron);
  sPHElectronv1& operator=(const sPHElectronv1& electron);

  virtual ~sPHElectronv1() {}
  
  virtual void identify(std::ostream& os = std::cout) const;
  virtual void Reset() {}
  virtual int isValid() const;
  virtual PHObject* CloneMe() const { return new sPHElectronv1(*this); }

  virtual unsigned int get_id()  const {return _id; }
  virtual int get_charge()       const {return _charge; }
  virtual double get_px()        const {return _px;}
  virtual double get_py()        const {return _py;}
  virtual double get_pz()        const {return _pz;}
  virtual double get_dphi()      const {return _dphi;}
  virtual double get_deta()      const {return _deta;}
  virtual double get_emce()      const {return _emce;}
  virtual double get_e3x3()      const {return _e3x3;}
  virtual double get_e5x5()      const {return _e5x5;}

  virtual double get_chi2()      const {return _chi2;}
  virtual unsigned int get_ndf() const {return _ndf;}
  virtual double get_zvtx()      const {return _zvtx;}
  virtual double get_dca2d()     const {return _dca2d;}
  virtual double get_dca2d_error()      const {return _dca2d_error;}
  virtual double get_dca3d_xy()  const {return _dca3d_xy;}
  virtual double get_dca3d_z()   const {return _dca3d_z;}

  virtual int get_nmvtx()   const {return _nmvtx;}
  virtual int get_ntpc()   const {return _ntpc;}

  virtual double get_cemc_ecore()   const {return _cemc_ecore;}
  virtual double get_cemc_chi2()    const {return _cemc_chi2;}
  virtual double get_cemc_prob()    const {return _cemc_prob;}
  virtual double get_cemc_dphi()    const {return _cemc_dphi;}
  virtual double get_cemc_deta()    const {return _cemc_deta;}
  virtual double get_hcalin_e()     const {return _hcalin_e;}
  virtual double get_hcalin_dphi()  const {return _hcalin_dphi;}
  virtual double get_hcalin_deta()  const {return _hcalin_deta;}

  virtual void set_id(unsigned int id) {_id = id;}
  virtual void set_charge(int charge)  {_charge = charge;}
  virtual void set_px(double px)       {_px = px;}
  virtual void set_py(double py)       {_py = py;}
  virtual void set_pz(double pz)       {_pz = pz;}
  virtual void set_dphi(double dphi)   {_dphi = dphi;}
  virtual void set_deta(double deta)   {_deta = deta;}
  virtual void set_emce(double emce)   {_emce = emce;}
  virtual void set_e3x3(double e3x3)   {_e3x3 = e3x3;}
  virtual void set_e5x5(double e5x5)   {_e5x5 = e5x5;}

  virtual void set_chi2(double a)      {_chi2 = a;}
  virtual void set_ndf(unsigned int a) {_ndf = a;}
  virtual void set_zvtx(double a)      {_zvtx = a;}
  virtual void set_dca2d(double a)     {_dca2d = a;}
  virtual void set_dca2d_error(double a)   {_dca2d_error = a;}
  virtual void set_dca3d_xy(double a)  {_dca3d_xy = a;}
  virtual void set_dca3d_z(double a)   {_dca3d_z = a;}

  virtual void set_nmvtx(int i)   {_nmvtx = i;}
  virtual void set_ntpc(int i)   {_ntpc = i;}

  virtual void set_cemc_ecore(double a)   {_cemc_ecore = a;}
  virtual void set_cemc_chi2(double a)    {_cemc_chi2 = a;}
  virtual void set_cemc_prob(double a)    {_cemc_prob = a;}
  virtual void set_cemc_dphi(double a)    {_cemc_dphi = a;}
  virtual void set_cemc_deta(double a)    {_cemc_deta = a;}
  virtual void set_hcalin_e(double a)     {_hcalin_e = a;}
  virtual void set_hcalin_dphi(double a)  {_hcalin_dphi = a;}
  virtual void set_hcalin_deta(double a)  {_hcalin_deta = a;}

 protected:

  unsigned int _id;
  int _charge;
  double _px;
  double _py;
  double _pz;
  double _dphi;
  double _deta;
  double _emce;
  double _e3x3;
  double _e5x5;

  double _chi2;
  unsigned int _ndf;
  double _zvtx;
  double _dca2d;
  double _dca2d_error;
  double _dca3d_xy;
  double _dca3d_z;

  int _nmvtx;
  int _ntpc;

  double _cemc_ecore;
  double _cemc_chi2;
  double _cemc_prob;
  double _cemc_dphi;
  double _cemc_deta;
  double _hcalin_e;
  double _hcalin_dphi;
  double _hcalin_deta;

  ClassDef(sPHElectronv1, 1)
};

#endif 

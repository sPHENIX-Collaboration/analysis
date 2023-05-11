//===========================================================
/// \file sChargeMap.h
/// \brief Objects that tracks the charge density at all times
/// \author Carlos Perez Lara
//===========================================================

#include "TH3D.h"

class sChargeMap {
 public:
  sChargeMap(int, float, float, int, float, float, int, float, float, float);
  virtual ~sChargeMap() {}
  void Fill(float, float, float, float);
  void Propagate(float);
  void ScreenShot(char *, char*, int);
  void SaveIonMap(char *);
  void SaveRho(char *,int,int,int);

 protected:
  TH3D *fI;
  TH3D *fE;
  float fEcmperus;
  float fIcmperms;
};

//===========================================================
/// \file sHelix.h
/// \brief Idealized helix track from charged particle in TPC
/// \author Carlos Perez Lara
//===========================================================

#include "TMath.h"

class sHelix {
 public:
  sHelix();
  sHelix(float x0, float y0, float z0, float px, float py, float pz, float q, float b=1.5);
  virtual ~sHelix() {}
  float x(float t) {return u(t)+fX0+fR*TMath::Sin(fPhi);}
  float y(float t) {return v(t)+fY0-fR*TMath::Cos(fPhi);}
  float z(float t) {return w(t)+fZ0;}
  float r(float t) {return TMath::Sqrt(x(t)*x(t)+y(t)*y(t));}

  float u(float t) {return -fR*TMath::Sin(fPhi-fW*t);}
  float v(float t) {return fR*TMath::Cos(fPhi-fW*t);}
  float w(float t) {return fC*t;}

  float k() {return fR/(fR*fR+fC*fC);}
  float s(float t1,float t2) {return TMath::Sqrt(fR*fW*fR*fW+fC*fC)*(t2-t1);} // sqrt( x'^2 + y'^2 + z'^2 ) DeltaT
  void breakIntoPieces(float t1, float t2, float x[100][3]);

  float findFirstInterceptTo(float rd, float hz);
  void SaveTracktoRootScript(float ri, float ro, float z, char *filec);

  float W() {return fW;}
  float R() {return fR;}
  float C() {return fC;}
  float Phi() {return fPhi;}
  void Debug() {fDebug = true;}

 protected:
  float fW;
  float fR;
  float fC;
  float fPhi;
  float fX0;
  float fY0;
  float fZ0;
  float fDebug;
};

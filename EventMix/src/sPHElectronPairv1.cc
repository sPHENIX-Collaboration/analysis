#include <TLorentzVector.h>

#include "sPHElectronPairv1.h"

sPHElectronPairv1::sPHElectronPairv1()
{
  _id = -99999;
  _type = 0;
  _e1 = sPHElectronv1();
  _e2 = sPHElectronv1();
}

sPHElectronPairv1::sPHElectronPairv1(sPHElectronv1* e1, sPHElectronv1* e2)
{
  _id = e1->get_id() + e2->get_id()*100000;
  _type = 0;
  _e1 = *e1;
  _e2 = *e2;
}

double sPHElectronPairv1::get_mass() const
{
  double px1 = _e1.get_px();
  double py1 = _e1.get_py();
  double pz1 = _e1.get_pz();
  double ee1 = sqrt(px1*px1+py1*py1+pz1*pz1+0.000511*0.000511);
  TLorentzVector tmp1 = TLorentzVector(px1,py1,pz1,ee1);
  double px2 = _e2.get_px();
  double py2 = _e2.get_py();
  double pz2 = _e2.get_pz();
  double ee2 = sqrt(px2*px2+py2*py2+pz2*pz2+0.000511*0.000511);
  TLorentzVector tmp2 = TLorentzVector(px2,py2,pz2,ee2);
  TLorentzVector pair = tmp1 + tmp2;
  return pair.M();
}

double sPHElectronPairv1::get_pt() const
{
  double px1 = _e1.get_px();
  double py1 = _e1.get_py();
  double pz1 = _e1.get_pz();
  double ee1 = sqrt(px1*px1+py1*py1+pz1*pz1+0.000511*0.000511);
  TLorentzVector tmp1 = TLorentzVector(px1,py1,pz1,ee1);
  double px2 = _e2.get_px();
  double py2 = _e2.get_py();
  double pz2 = _e2.get_pz();
  double ee2 = sqrt(px2*px2+py2*py2+pz2*pz2+0.000511*0.000511);
  TLorentzVector tmp2 = TLorentzVector(px2,py2,pz2,ee2);
  TLorentzVector pair = tmp1 + tmp2;
  return pair.Pt();
}

double sPHElectronPairv1::get_eta() const
{
  double px1 = _e1.get_px();
  double py1 = _e1.get_py();
  double pz1 = _e1.get_pz();
  double ee1 = sqrt(px1*px1+py1*py1+pz1*pz1+0.000511*0.000511);
  TLorentzVector tmp1 = TLorentzVector(px1,py1,pz1,ee1);
  double px2 = _e2.get_px();
  double py2 = _e2.get_py();
  double pz2 = _e2.get_pz();
  double ee2 = sqrt(px2*px2+py2*py2+pz2*pz2+0.000511*0.000511);
  TLorentzVector tmp2 = TLorentzVector(px2,py2,pz2,ee2);
  TLorentzVector pair = tmp1 + tmp2;
  return pair.Eta();
}

double sPHElectronPairv1::get_phiv() const
{
// assumes that the first element of the pair is the positron, and second the electron
  double px1 = _e1.get_px();
  double py1 = _e1.get_py();
  double pz1 = _e1.get_pz();
  double px2 = _e2.get_px();
  double py2 = _e2.get_py();
  double pz2 = _e2.get_pz();
  double px = px1 + px2;
  double py = py1 + py2;
  double pz = pz1 + pz2;
  double pp = sqrt(px*px+py*py+pz*pz);
  double ux = px/pp;
  double uy = py/pp;
  double uz = pz/pp;

// axis defined by (ux,uy,ux)X(0,0,1).
// this is the axis that is perpendicular to the direction of
// pair, and also perpendicular to the Z axis (field direction).
// If the pair is conversion at R!=0, it must have (apparent)
// momentum component in this axis (caused by field intergral from the
// vertex point to the conversion point).
// The sign of the component is opposite for e+ and e-.
//
// (ux,uy,ux)X(0,0,1)=(uy,-ux,0)
//

  double ax =  uy/sqrt(ux*ux+uy*uy);
  double ay = -ux/sqrt(ux*ux+uy*uy);

//momentum of e+ and e- in (ax,ay,az) axis. Note that az=0 by definition.

//vector product of pep X pem
  double vpx  = py1*pz2 - pz1*py2;
  double vpy  = pz1*px2 - px1*pz2;
  double vpz  = px1*py2 - py1*px2;
  double vp   = sqrt(vpx*vpx+vpy*vpy+vpz*vpz);

//unit vector of pep X pem
  double vx = vpx/vp;
  double vy = vpy/vp;
  double vz = vpz/vp;

//The third axis defined by vector product (ux,uy,uz)X(vx,vy,vz)
  double wx = uy*vz - uz*vy;
  double wy = uz*vx - ux*vz;
//  double wz = ux*vy - uy*vx;
//  double wl = sqrt(wx*wx+wy*wy+wz*wz);
//// by construction, (wx,wy,wz) must be a unit vector.
//  if(fabs(wl - 1.0) > 0.00001) std:;cout << "Calculation error in W vector"<<std::endl;

// measure angle between (wx,wy,wz) and (ax,ay,0). The angle between them
// should be small if the pair is conversion
  double cosPhiV = wx*ax + wy*ay;
  double phiV = acos(cosPhiV);
  
  return phiV;
}


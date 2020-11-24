#include <TLorentzVector.h>

#include "sPHElectronPairv1.h"

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


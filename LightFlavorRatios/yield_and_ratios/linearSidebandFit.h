#ifndef TF1_SIDEBAND_FIT_H
#define TF1_SIDEBAND_FIT_H

#include "TF1.h"
#include "kinematicThreshold.h"

Double_t linear_sidebandonly(Double_t* v, Double_t* par)
{
  Double_t x = v[0];
  Double_t slope = par[0];
  Double_t c = par[1];
  Double_t left_sideband_low = par[2];
  Double_t left_sideband_high = par[3];
  Double_t right_sideband_low = par[4];
  Double_t right_sideband_high = par[5];

  if((x<left_sideband_low || x>left_sideband_high) && (x<right_sideband_low || x>right_sideband_high))
  {
    TF1::RejectPoint();
  }

  return std::max(slope*x+c,0.);
}

Double_t linear_threshold(Double_t* v, Double_t* par)
{

  Double_t x = v[0];
  Double_t slope = par[0];
  Double_t c = par[1];
  Double_t mother_pt = par[2];
  Double_t daughter_pt_cutoff = par[3];
  Double_t daughter1_mass = par[4];
  Double_t daughter2_mass = par[5];

  Double_t turnon_pars[4] = {mother_pt,daughter_pt_cutoff,daughter1_mass,daughter2_mass};

  Double_t val = (slope*x+c)*kinematic_threshold_turnon(v,turnon_pars);

  return std::max(val,0.);
}

Double_t linear_sidebandonly_threshold(Double_t* v, Double_t* par)
{
  Double_t x = v[0];
  Double_t left_sideband_low = par[6];
  Double_t left_sideband_high = par[7];
  Double_t right_sideband_low = par[8];
  Double_t right_sideband_high = par[9];

  if((x<left_sideband_low || x>left_sideband_high) && (x<right_sideband_low || x>right_sideband_high))
  {
    TF1::RejectPoint();
  }
  return linear_threshold(v,par);
}

TF1* linear_sideband_TF1(double xlow, double xhigh, std::pair<float,float> left_sideband, std::pair<float,float> right_sideband)
{
  TF1* f = new TF1("linear_sidebandonly",&linear_sidebandonly,xlow,xhigh,6);
  f->FixParameter(2,left_sideband.first);
  f->FixParameter(3,left_sideband.second);
  f->FixParameter(4,right_sideband.first);
  f->FixParameter(5,right_sideband.second);

  return f;
}

TF1* linear_sideband_TF1_threshold(double xlow, double xhigh, double mother_pt, double daughter_pt_cutoff, int daughter1_pdgid, int daughter2_pdgid, std::pair<float,float> left_sideband, std::pair<float,float> right_sideband)
{
  double daughter1_mass = TDatabasePDG::Instance()->GetParticle(daughter1_pdgid)->Mass();
  double daughter2_mass = TDatabasePDG::Instance()->GetParticle(daughter2_pdgid)->Mass();

  TF1* f = new TF1("linear_sidebandonly_threshold",&linear_sidebandonly_threshold,xlow,xhigh,10);
  f->FixParameter(2,mother_pt);
  f->FixParameter(3,daughter_pt_cutoff);
  f->FixParameter(4,daughter1_mass);
  f->FixParameter(5,daughter2_mass);
  f->FixParameter(6,left_sideband.first);
  f->FixParameter(7,left_sideband.second);
  f->FixParameter(8,right_sideband.first);
  f->FixParameter(9,right_sideband.second);

  return f;
}

TF1* linear_background(TF1* sideband_fit)
{
  double xlow, xhigh;
  sideband_fit->GetRange(xlow,xhigh);
  TF1* f = new TF1("linear_background","[0]*x+[1]",xlow,xhigh);
  for(int i=0;i<6;i++)
  {
    f->SetParameter(i,sideband_fit->GetParameter(i));
  }
  return f;
}

TF1* linear_background_threshold(TF1* sideband_fit)
{
  double xlow, xhigh;
  sideband_fit->GetRange(xlow,xhigh);
  TF1* f = new TF1("linear_background_threshold",&linear_threshold,xlow,xhigh,6);
  for(int i=0;i<6;i++)
  {
    f->SetParameter(i,sideband_fit->GetParameter(i));
  }
  return f;
}

#endif

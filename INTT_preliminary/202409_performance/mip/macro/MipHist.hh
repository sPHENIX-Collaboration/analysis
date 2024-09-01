#pragma once

#include "HistSetting.hh"

class MipHist
{
private:
  int bin_num_ = 20;
  int color_ = kBlack;
  
  double xmin_ = 0;
  double xmax_ = 600;
  double modification_factor_ = 0.35;
  double alpha_ = 0.1;
  
  string name_;
  string title_;
  
  TH1D* hist_raw_; // contains all data
  TH1D* hist_; // contains modified data
  TH1D* hist_shc_adc7_; // contains only data of Single-Hit Cluster (SHC)
  TH1D* hist_dhc_adc14_; // contains only data of Double-Hit Cluster (DHC)
  TF1* f_;
  
  void Fill( TH1D* hist, int val );
  void Init();
  void ModifyAdc( TH1D* hist_ref, double power_factor );
  
public:
  MipHist( string name, string title );

  void Fill( int val );
  void FillAll( int val, bool is_single, bool is_double );
  void FillSingleHitCluster( int val );
  void FillDoubleHitCluster( int val );

  void Drawline(){};

  TF1* GetFunction(){ return f_;};
  TH1D* GetHist(){ return hist_;};
  TH1D* GetNormalizedHist();
  void ModifyAdc();
  void SetColorAlpha( int color, double alpha );

  void Print();
};


#ifndef MipHist_cc
#include "MipHist.cc"
#endif

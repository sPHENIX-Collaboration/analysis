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
  string tag_ = "";;

  bool does_adc7_correction_ = true;
  bool does_adc14_correction_ = true;

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

  void DrawLine( TF1* f );

  int		GetColor(){ return color_;};
  double	GetAlpha(){ return alpha_;}
  TF1*		GetFunction(){ return f_;};
  TH1D*		GetHist(){ return hist_;};
  TF1*		GetNormalizedFunction( double top_val = 1.0);
  TH1D*		GetNormalizedHist( double top_val = 1.0 );
  string	GetTag(){ return tag_;};
  
  void ModifyAdc();
  void SetColorAlpha( int color, double alpha, bool does_all = false );
  void SetAdc7Correction( bool flag=true ){ does_adc7_correction_ = flag; };
  void SetAdc14Correction( bool flag=true ){ does_adc14_correction_ = flag; };
  void SetTag( string tag ){ tag_ = tag;}
  
  void Print();
};


#ifndef MipHist_cc
#include "MipHist.cc"
#endif

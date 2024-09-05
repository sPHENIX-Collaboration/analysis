#pragma once
#include <sPhenixStyle.C>

#include "Event.hh"

class Analysis
{
private:
  string root_path_ = "";
  string text_path_ = "";
  string output_ = "results/streaming_timing_run00050889.pdf";
  
  bool is_preliminary_ = false;
  bool is_logy_ = false;
  
  int shift_value_ = 5;
  int process_num_ = 100;
  int streaming_offset_ = 23;
  int event_num_;
  
  double top_margin_ = 0.05; // 0.1;
  double right_margin_ = 0.05; // 0.15;
  double ymin_ = 0.1;
  double ymax_ = 5e5;
  
  TH1D* hist_fphx_bco_;
  TH1D* hist_fphx_bco_raw_;
  
  vector < Event* > events_;
  
  void Init( string data_no_suffix );
  void InitRoot();
  void InitText();

  template < class T >
  void ConfigureHist( T* hist )
  {
    hist->GetXaxis()->CenterTitle();
    hist->GetYaxis()->CenterTitle();
    hist->SetLineWidth( 3 );

    hist->GetXaxis()->SetLabelSize( 0.05 );
    hist->GetXaxis()->SetTitleSize( 0.05 );
    //  hist->GetXaxis()->SetRangeUser( 0, 500 );
    
    hist->GetYaxis()->SetLabelSize( 0.05 );
    hist->GetYaxis()->SetTitleSize( 0.05 );
    //  hist->GetYaxis()->SetRangeUser( 0, 500 );  
  }
  
  string GetDate();
  void DrawWords( Event* event );
  void DrawGl1Timing( Event* event );
  TH1D* GetShiftedHist( TH1D* hist_arg );

public:
  Analysis( string text, bool is_preliminary=false );
  
  void Draw();

  void SetPreliminary( bool flag ){ is_preliminary_ = flag;};
};


#ifndef Analysis_cc
#include "Analysis.cc"
#endif
